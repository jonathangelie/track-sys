#!/usr/bin/env python

#
#  Copyright (C) 2019  Jonathan Gelie <contact@jonathangelie.com>
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

from PyQt4.QtCore import *
from PyQt4.QtGui import *

#version automatically updating while generating the DEBIAN package
version="1.0"

import sys
# for options parsing 
import textwrap
import argparse

import subprocess

import imp
import os


#Failed to load module "canberra-gtk-module"
#sudo apt-get install libcanberra-gtk-module

import cmd

Debugging = True

class customStdout(QObject):

    newStdoutWrite = pyqtSignal(str)

    def __init__(self, edit, out=None):

        # Initialize the customStdout as a QObject so it can emit signals
        QObject.__init__(self)
        
        """(edit, out=None, color=None) -> can write stdout, stderr to a
        QTextEdit.
        edit = QTextEdit
        out = alternate stream ( can be the original sys.stdout )
        """
        self.edit = edit
        self.out = out
        self._qt = True

    def flush(self):
        """ ignoring sys.stdout.flush() call """

    def write(self, m):
        """ ignoring sys.stdout.write() call """
        if m == '\n':
            '''
            discarded print new line write
            https://github.com/python/cpython/blob/2.7/Python/bltinmodule.c#L1693
            '''
            return
        self.newStdoutWrite.emit(m)

class ui(QMainWindow):
    def __init__(self, parent=None):
        super(ui, self).__init__(parent)
        self._cmd = cmd.cmd()
        self.tracks = ()
        
    def AboutMenu(self):

        msgBox = QMessageBox()
        msgBox.setWindowTitle("About Menu")

        msg = "<span style='text-align: center'>"
        msg += ("<p style='font-size: 12pt; color: black'>Track-ui</p>")
        msg += ("<p style='font-size: 12pt; color: black'>version: %s</p>" % version)
        msg += ("<p style='font-size: 12pt; color: black'>Author: contact@jonathangelie.com</p>")
        msg += ("<p style='font-size: 12pt; color: black'>Copyright (C) 2019, Jonathan Gelie</p>")
        msg += ("</span>")
        msgBox.setText(msg)
        
        msgBox.exec_()

    def LogMenu(self):
        try:
            subprocess.check_output(["xdg-open", '/var/log/track-ui/track.log'])
        except (OSError, subprocess.CalledProcessError) as e:
            print("error: %s %s" % (str(e), '/var/log/track-ui/track.log'))
            pass
        
    def createMenu(self):

        menubar = self.menuBar() 
        FileMenu = menubar.addMenu('&File')
        HelpMenu = menubar.addMenu('&Help')


        LOgAct = QAction("&Log", self,
                statusTip="Show application log file",
                triggered=self.LogMenu)

        FileMenu.addAction(LOgAct)

        aboutAct = QAction("&About", self,
                statusTip="Show the application's About box",
                triggered=self.AboutMenu)

        HelpMenu.addAction(aboutAct)

    def stdout_redirection(self):
        sys.stdout = customStdout(self.outputtext, sys.stdout)
        out = customStdout(self.outputtext, sys.stdout)
        sys.stdout.newStdoutWrite.connect(self.write_to_qTextEdit)

    def closeEvent(self, event):
        print("Closing")
        self._cmd.cmd_exit()
        
    def start(self):
        self.originalPalette = QApplication.palette()

        disableWidgetsCheckBox = QCheckBox("&Disable widgets")

        self.createMenu()
        self.createOutputLabel()
        self.createTrack()
        self.createConnection()
        self.createTrain()
        self.createSignal()
        
        self.track_get_list()
        
        self.create_start_button()
        self.createProgressBar()

        # redirection of output message to the output label
        # Warning disable this if you want to get all debug and system message
        self.stdout_redirection()

        mainLayout = QGridLayout()

        mainLayout.addWidget(self.track, 1, 0,)
        mainLayout.addWidget(self.connection, 2, 0,)
        mainLayout.addWidget(self.train, 3, 0,)
        mainLayout.addWidget(self.signal, 4, 0,)
        
        mainLayout.addWidget(self.buttonstart, 20,0)
        mainLayout.addWidget(self.output, 1, 1, 20, 2)
        
        mainLayout.addWidget(self.progressBar, 21, 1, 1, 2)
        
        mainLayout.setColumnStretch(1, 10)
        mainLayout.setColumnStretch(2, 20)

        test = QWidget()
        test.setLayout(mainLayout)
         
        self.setCentralWidget(test)

        self.setGeometry(10, 10, 1280, 880) #screen_resolution.height() - 60)
        self.setFixedSize(self.size())
        self.setWindowTitle("Track-ui")
        self.changeStyle('Plastique')

    def change_color(self, color):
        template_css = """QProgressBar::chunk { background: %s; }"""
        css = template_css % color
        self.setStyleSheet(css)

    def changeStyle(self, styleName):
        QApplication.setStyle(QStyleFactory.create(styleName))
        self.changePalette()

    def changePalette(self):
        QApplication.setPalette(QApplication.style().standardPalette())
        #QApplication.setPalette(self.originalPalette)

    def write_to_qTextEdit(self, message):

        cursor = self.outputtext.textCursor()
        cursor.movePosition(cursor.End)
        #cursor.insertText('\n')
        cursor.movePosition(cursor.Down)
        cursor.movePosition(cursor.StartOfLine)
        self.outputtext.setTextCursor(cursor)

        html =  "<span style=\" font-family: monospace; font-size:12pt; color:#FFFFFF;\" >"
        html += message.replace(' ', '&nbsp;')
        html += "</span>"
        
        self.outputtext.textCursor().insertHtml(html)
        self.outputtext.textCursor().insertText("\n")
        
        self.outputtext.ensureCursorVisible()

    def error_message(self, title, msg):
        msgBox = QMessageBox()
        msgBox.setIcon(QMessageBox.Warning)
        msgBox.setText(msg)
        msgBox.setWindowTitle(title)
        msgBox.setStandardButtons(QMessageBox.Ok)
        msgBox.exec_()

    def simualation_started(self):
        self.track.setDisabled(True)
        self.connection.setDisabled(True)
        self.train.setDisabled(True)
        self.signal.setDisabled(True)
        
        self.animateProgressbar()
        self.buttonstart.setText("Stop")
            
    def simualtion_stopped(self):
        self.track.setDisabled(False)
        self.connection.setDisabled(False)
        self.train.setDisabled(False)
        self.signal.setDisabled(False)
        
        self.buttonstart.setText("Start")
        self.progressBar.setMinimum(0)
        self.progressBar.setMaximum(100)
            
    def on_simul_termination(self):
        print("simulation done")
        self.simualtion_stopped()
        
    def on_button_start_pressed(self):
        if self.buttonstart.text() == 'Start':
            
            print("simulation-start")
            
            self.simualation_started()

            self._cmd.cmd_register_to_event(cmd.event_simulation_stop, self.on_simul_termination)
            ret = self._cmd.cmd_simulation_start()

        else:
            
            print("simulation-stop")
            ret = self._cmd.cmd_simulation_stop()
            
            self.simualtion_stopped()

    def create_start_button(self):
        self.buttonstart = self.createButton("Start", self.on_button_start_pressed)

    def animateProgressbar(self):

        self.progressBar.setStyleSheet("\
        QProgressBar { max-height: 15px; }")

        self.progressBar.setFormat("")
        self.progressBar.setMinimum(0)
        self.progressBar.setMaximum(0)


    def createProgressBar(self):
        self.progressBar = QProgressBar()
        self.progressBar.setRange(0, 100)
        style = ''' QProgressBar{max-height: 15px;text-align: center;}'''
        self.progressBar.setStyleSheet(style)

    def createButton(self, text, member):
        button = QPushButton(text)
        button.clicked.connect(member)
        return button

    def createOutputLabel(self):

        tab2 = QWidget()
        self.outputtext = QTextEdit()
        # DarkGrey background
        self.outputtext.setStyleSheet("background-color: rgb(30,30,30);")

        topLayout = QHBoxLayout()
        topLayout.addWidget(self.outputtext)

        self.output = topLayout
        self.output = QGroupBox("Output")
        self.output.setLayout(topLayout)

    def on_track_create(self):

        print("train-add <name>")
        ret = self._cmd.cmd_track_add(self.trackNameEdit.text())
        
        self.tracks = self._cmd.cmd_track_list()
        self.track_get_list()

    def createTrack(self):
        self.track = QGroupBox("Track")
        self.track.setChecked(False)

        trackNameLabel = QLabel('Name')
        self.trackNameEdit = QLineEdit()

        NewTrackButton = self.createButton('Create', self.on_track_create)
        
        layout = QGridLayout()
        layout.addWidget(trackNameLabel, 0, 0)
        layout.addWidget(self.trackNameEdit, 0, 1)
        layout.addWidget(NewTrackButton, 1, 0)

        self.track.setLayout(layout)

    def track_get_list(self):
        self.Track1ComboBox.clear()
        self.Track2ComboBox.clear()
        self.TrackComboBox.clear()
        self.SignTrackComboBox.clear()

        #self.tracks = ("track#1", "track#6", "track#5", "track#4")
        
        if (len(self.tracks) == 0):
            self.Track1ComboBox.addItem("None")
            self.Track2ComboBox.addItem("None")
            self.TrackComboBox.addItem("None")
            self.SignTrackComboBox.addItem("None")
        else:   
            for track in self.tracks:
                self.Track1ComboBox.addItem(track)
                self.Track2ComboBox.addItem(track)
                self.TrackComboBox.addItem(track)
                self.SignTrackComboBox.addItem(track)
    
    def on_connection_create(self):
        if self.continuationRB.isChecked():
            print("continuation-add <track1> <position1> <track2> <position2>")
            ret = self._cmd.cmd_continuation_add(self.Track1ComboBox.currentText(),
                                                 self.Track1PositionCB.currentText(),
                                                 self.Track2ComboBox.currentText(),
                                                 self.Track2PositionCB.currentText())
        else:
            print("junction-add <track1> <position1> <track2> <position2>")
            ret = self._cmd.cmd_junction_add(self.Track1ComboBox.currentText(),
                                             self.Track1PositionCB.currentText(),
                                             self.Track2ComboBox.currentText(),
                                             self.Track2PositionCB.currentText())
            

    def createConnection(self):
        self.connection = QGroupBox("Connection")
        self.connection.setChecked(False)

        self.continuationRB = QRadioButton("Continuation")
        junctionRB = QRadioButton("junction")
        self.continuationRB.setChecked(True)
        
        self.Track1ComboBox = QComboBox()
        self.Track1ComboBox.addItem("None")
        
        Track1Label = QLabel("Track#1 Name")
        Track1Label.setBuddy(self.Track1ComboBox)
        
        self.TracPositionCB = {}

        self.TracPositionCB["track1"] = QComboBox()
        self.TracPositionCB["track1"].addItem("west")
        self.TracPositionCB["track1"].addItem("est")
        
        Pos1Label = QLabel("Position")
        Pos1Label.setBuddy(self.TracPositionCB["track1"])
        
        self.Track2ComboBox = QComboBox()
        self.Track2ComboBox.addItem("None")

        Track2Label = QLabel("Track#2 Name")
        Track2Label.setBuddy(self.Track2ComboBox)

        self.TracPositionCB["track2"] = QComboBox()
        self.TracPositionCB["track2"].addItem("est")
        self.TracPositionCB["track2"].addItem("west")

        Pos2Label = QLabel("Position")
        Pos2Label.setBuddy(self.TracPositionCB["track2"])
                
        NewConnectionButton = self.createButton('Create', self.on_connection_create)
        
        layout = QGridLayout()
        layout.addWidget(self.continuationRB, 0, 0)
        layout.addWidget(junctionRB, 0, 1)
        layout.addWidget(Track1Label, 1, 0)
        layout.addWidget(self.Track1ComboBox, 1, 1)
        layout.addWidget(Pos1Label, 2, 0)
        layout.addWidget(self.TracPositionCB["track1"], 2, 1)
        
        layout.addWidget(Track2Label, 3, 0)
        layout.addWidget(self.Track2ComboBox, 3, 1)
        layout.addWidget(Pos2Label, 4, 0)
        layout.addWidget(self.TracPositionCB["track2"], 4, 1)
        layout.addWidget(NewConnectionButton, 5, 0)

        self.connection.setLayout(layout)

    def on_train_create(self):

        print("train add <name> <track> <direction>")
        self._cmd.cmd_train_add(self.trainNameEdit.text(),
                                self.TrackComboBox.currentText(),
                                self.TrainDirectionCB.currentText())

    def createTrain(self):
        self.train = QGroupBox("Train")
        self.train.setChecked(False)

        trainNameLabel = QLabel('Name')
        self.trainNameEdit = QLineEdit()

        TrackLabel = QLabel("Track Name")
        self.TrackComboBox = QComboBox()
        self.TrackComboBox.addItem("None")
        
        TrackLabel.setBuddy(self.Track1ComboBox)

        TrackDirectionLabel = QLabel("Direction")
        self.TrainDirectionCB = QComboBox()
        self.TrainDirectionCB.addItem("westbound")
        self.TrainDirectionCB.addItem("estbound")
        
        NewtrainButton = self.createButton('Create', self.on_train_create)
        
        layout = QGridLayout()
        layout.addWidget(trainNameLabel, 0, 0)
        layout.addWidget(self.trainNameEdit, 0, 1)
        layout.addWidget(TrackLabel, 1, 0)
        layout.addWidget(self.TrackComboBox, 1, 1)
        layout.addWidget(TrackDirectionLabel, 2, 0)
        layout.addWidget(self.TrainDirectionCB, 2, 1)
        layout.addWidget(NewtrainButton, 3, 0)

        self.train.setLayout(layout)

    def on_signal_create(self):

        print("signal add <track> <position>")
        self._cmd.cmd_signal_add(self.SignTrackComboBox.currentText(),
                                 self.signalDirectionCB.currentText())
        
    def createSignal(self):
        self.signal = QGroupBox("Signals")
        self.signal.setChecked(False)

        TrackLabel = QLabel("Track Name")
        self.SignTrackComboBox = QComboBox()
        self.SignTrackComboBox.addItem("None")
        
        TrackLabel.setBuddy(self.Track1ComboBox)

        TrackDirectionLabel = QLabel("Position")
        self.signalDirectionCB = QComboBox()
        self.signalDirectionCB.addItem("west")
        self.signalDirectionCB.addItem("est")
        
        NewsignalButton = self.createButton('Create', self.on_signal_create)
        
        layout = QGridLayout()

        layout.addWidget(TrackLabel, 0, 0)
        layout.addWidget(self.SignTrackComboBox, 0, 1)
        layout.addWidget(TrackDirectionLabel, 1, 0)
        layout.addWidget(self.signalDirectionCB, 1, 1)
        layout.addWidget(NewsignalButton, 2, 0)

        self.signal.setLayout(layout)
        

def main(args):
    class CustomerFormatter(argparse.ArgumentDefaultsHelpFormatter, argparse.RawTextHelpFormatter):
        pass
    
    parser = argparse.ArgumentParser(formatter_class=CustomerFormatter,
                                     description=textwrap.dedent('''
        track-ui
        '''))
    parser.add_argument('-v','--verbose', action='store_true',
                        help='Increase output verbosity')
    arg = parser.parse_args(args)

    u = ui()
    u.start()
    u.show()
    sys.exit(app.exec_()) 

if __name__ == '__main__':
    app = QApplication(sys.argv)
    main(sys.argv[1:])

