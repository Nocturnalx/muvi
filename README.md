# muvi
This is an audio visualiser project primarily targeting the raspberry pi but also for desktop linux as well.

It uses gtk 3 for the window, expoy for openGL rendering and the pulse audio api to capture audio so to build you need:

`sudo apt install libgtk-3-dev libepoxy-dev libpulse-dev`

the binary in /bin is only for raspberry pi.

Planned features:

*I/O controll (setting audio source in command line or in app)
*spotify API integration to display album art etc
