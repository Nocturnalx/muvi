# muvi
This is an audio visualiser project primarily targeting the raspberry pi but also for desktop linux as well. Also works with the pipewire pulseaudio server too.

- (it has version checking for openGL 3.0 if it can't find it then it will aim for 2.1 - i couldn't get the gtk gl area to find openGL ES but raspberry pi can do desktop 2.1 so i implemented my own instanced rendering and v140 glsl shaders for that meaning it is not as efficent but its not a noticable difference)

It uses gtk 3 for the window, expoy for openGL rendering and the pulse audio api to capture audio so to build you need:

`sudo apt install libgtk-3-dev libepoxy-dev libpulse-dev`
`sudo pacman -S sudo pacman -S gtk3 libepoxy libpulse`

As well as the typical:

`sudo apt install cmake build-essential`
`sudo pacman -S cmake base-devel`

and then:

```
cmake
cd build
make
```

the binary in /bin is only for raspberry pi.

# Usage
run on the command line with ./muvi when run the command line will print out the available audio sources, you can run again with the -d flag followed by the number to specify the device. If the flag is not set then it will always choose 0 which will be the default pulse audio device.

# Planned features:
- I/O controll (setting audio source in command line or in app)
- spotify API integration to display album art etc

My main idea for this is that it is to be used alongside a tool like spotifyd and captures the audio out from the pulse audio mixer loopback but it is also possible to set the audio jack to an input and capture audio from that. In the very long term I am thinking about possibly allowing a user to set the visualiser up as a full spotify client so that it can both play spotify audio and to the visualisations on top so you can just deamonize it to launch on startup and the pi will be its own contained spotify player/visualiser.
