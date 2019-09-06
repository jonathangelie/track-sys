import subprocess
import sysv_ipc
import posix_ipc
import threading
import Queue
import time
import struct
import os
import sys

Debugging = True

def DBG(*args):
    if Debugging:
        msg = " ".join([str(a) for a in args])
        print(msg)
        
#helperExe = "../bin/track-helper"

script_path = os.path.join(os.path.abspath(os.path.dirname(__file__)))
helperExe = os.path.join(script_path, "../bin/track-helper")

class ipc:
    
    ipc_msg_rsp = 0
    ipc_msg_event = 1
    ipc_msg_dbg = 2
        
    def __init__(self, event_cb = None):
        self.start()
        self.event_cb = event_cb

    def start(self):
        self._helper = subprocess.Popen([helperExe, "--with_ui"],
                                        stdin=subprocess.PIPE,
                                        stdout=sys.stdout)
        time.sleep(3)
        try:
            self.mq = {}
            self.mq["send"] = posix_ipc.MessageQueue("/cmd")
            self.mq["receive"] = posix_ipc.MessageQueue("/resp")
        except sysv_ipc.ExistentialError as e:
            DBG("ERROR: message queue creation failed " % str(e))
            raise
        
        self.thread_init()
    
    def close(self):
        self.thread_close()
        
        self.mq["receive"].close()
        self.mq["send"].close()
        
    def thread_init(self):
        self.q_msg = Queue.Queue()
        self.q_msg.queue.clear()
        
        self.thread = threading.Thread(target = self.read_thread,
                                       name="ipc_read_thread")
        self.thread.daemon = True
        self.threading = True
        self.thread.start()
    
    def thread_close(self):
        self.threading = False
        self.thread.join(1)
        self.thread = None
        
    def read_thread(self):
        while self.threading:
            try:
                (message, type) = self.mq["receive"].receive()
                #DBG("thread rcv message: %s" % message)
                
                msgType =  int(message.split(' ')[0])
                if ipc.ipc_msg_dbg == msgType:
                    ret = self.parse(message)
                    DBG(ret["data"])
                elif ipc.ipc_msg_event == msgType :
                    ret["eventType"] = ret["status"]
                    ret.pop('status', None)
                    self.event_cb(ret)
                else:
                    self.q_msg.put(message)

            except sysv_ipc.ExistentialError as e:
                DBG("message queue error " + str(e))
                break
            
    def send(self, cmd):
        DBG("ipc send %s" % cmd)
        self.mq["send"].send(cmd)

    def wait_for(self, msgType, _timeout = 5):
        while _timeout:
            try:
                msg = self.q_msg.get(True, 1)
                if msg == None:
                    continue
                
                ret = self.parse(msg)
                if ret["type"] == msgType:
                    return ret
                
                # put it back
                self.q_msg.put(msg)
                continue


            except Queue.Empty:
                pass
    
            _timeout -= 1
        
        DBG("wait_for msgType %d timeout error" % msgType)
     
    def wait_event(self, timeout = 5):
        return self.wait_for(ipc.ipc_msg_event, _timeout = timeout)
       
    def wait_resp(self, timeout = 5):
        return self.wait_for(ipc.ipc_msg_rsp, _timeout = timeout)

    def parse(self, msg):
        
        #DBG("parsing : %s" % msg)
        l = msg.split(' ')

        #"TT SS LL DD"
        resp = {}
        resp["type"] = int(l[0])
        resp["status"] = int(l[1])
        resp["data_len"] = int(l[2])
        if resp["data_len"]:
            resp["data"] = msg[9: 9 + resp["data_len"]]

        #DBG(resp)

        return resp