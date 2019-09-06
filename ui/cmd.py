import struct
import ipc
from __builtin__ import False
from _curses import cbreak

event_simulation_stop = 0
    
class cmd:

    
    def __init__(self):
        self._ipc = ipc.ipc(self.cmd_on_event)
        self.evtcb = {}
        
    def cmd_on_event(self, d):
        if d["eventType"] in self.evtcb:
            self.evtcb[d["eventType"]]()
    
    def cmd_register_to_event(self, eventType, cb):
        self.evtcb[eventType] = cb

    def cmd_track_add(self, name):
        self._ipc.send("track-add %s" % name)
        r = self._ipc.wait_resp()
        return self._cmd_checl_status(r)
    
    def cmd_track_list(self):
        self._ipc.send("track-list")
        r = self._ipc.wait_resp()
        if r == None:
            return False
        
        if r["status"] != 0:
            return False
             
        cnt = int(r["data"])
        
        tracks = []
        while cnt:
            r = self._ipc.wait_resp()
            tracks.append(r["data"])
            cnt -= 1
            
        return tracks
    
    def _cmd_checl_status(self, r):
        if r == None:
            return False
        
        if r["status"] != 0:
            return False
        
        return True
    
    def cmd_continuation_add(self, track1, pos1, track2, pos2):
        self._ipc.send("continuation-add %s %s %s %s" %
                       (track1, pos1, track2, pos2))
        r = self._ipc.wait_resp()
        return self._cmd_checl_status(r)
    
    def cmd_junction_add(self, track1, pos1, track2, pos2):
        self._ipc.send("junction-add %s %s %s %s" %
                       (track1, pos1, track2, pos2))
        r = self._ipc.wait_resp()
        return self._cmd_checl_status(r)
    
    def cmd_train_add(self, name, track, dir):
        self._ipc.send("train-add %s %s %s" % (name, track, dir))
        r = self._ipc.wait_resp()
        return self._cmd_checl_status(r)
    
    def cmd_signal_add(self, track, dir):
        self._ipc.send("signal-add %s %s" % (track, dir))
        r = self._ipc.wait_resp()
        return self._cmd_checl_status(r)
        
    def cmd_simulation_start(self):
        self._ipc.send("simulation-start")
        r = self._ipc.wait_resp()
        return self._cmd_checl_status(r) 
    
    def cmd_simulation_stop(self):
        self._ipc.send("simulation-stop")
        r = self._ipc.wait_resp()
        return self._cmd_checl_status(r)

    def cmd_exit(self):
        self._ipc.send("exit")
        self._ipc.close()
