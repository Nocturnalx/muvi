# muvi
This is an audio visualiser project primarily targeting the raspberry pi but also for desktop linux as well.

- (it has version checking for openGL 3.0 if it can't find it then it will aim for 2.1 - i couldn't get the gtk gl area to find openGL ES but raspberry pi can do desktop 2.1 so i implemented my own instanced rendering and v140 glsl shaders for that meaning it is not as efficent but its not a noticable difference)

It uses gtk 3 for the window, expoy for openGL rendering and the pulse audio api to capture audio so to build you need:

`sudo apt install libgtk-3-dev libepoxy-dev libpulse-dev`

the binary in /bin is only for raspberry pi.

Planned features:

- I/O controll (setting audio source in command line or in app)
- spotify API integration to display album art etc
