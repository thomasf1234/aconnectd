# aconnectd

Small POC service to listen for udev "add" events and auto subscribe them to an ALSA MIDI receiver. My initial requirements were autoconnect from my MIDI keyboard to the Midi Through client on Ubuntu. 

# Building package

```
$ make 
$ cp build/aconnectd packages/aconnectd_0.1.0_amd64/opt/aconnectd/bin/aconnectd
$ cd packages
$ dpkg-deb --build ./aconnectd_0.1.0_amd64
```