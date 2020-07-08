# MUSIC_CAN aka MIDI_CAN

## Concept
MUSIC_CAN is an ode to the original project name “MIDI_CAN.”  Built to embrace all that music can do for our psyche, culture, and society,  MUSIC_CAN is a networked, multimode, instrument / MIDI controller.
 
During the pandemic and the new socially distant world we find ourselves in, we are now more separate than ever.  With rolling stay at home orders, what can we do to continue our collaborative nature?  How can we still connect even though we can't physically?
 
We wanted to make something to connect us, even though we are miles apart. So, we built the MUSIC_CAN. 
 
With an Arduino Nano 33 IoT microcontroller, an 8 sensor capacitive touch array and a rotary encoder for its hardware and a software stack that features Arduino, NodeJS, MIDI, UDP, and your DAW of choice, the MIDI_CAN allows for multiple input modes and musical interactions. The unique inputs on the MUSIC_CAN allow for creating distinctive and pleasing electronic music. The MUSIC_CAN can be played by itself or collaboratively over a network and is designed to be used with one or more musicians / devices. 
 
Music is a powerful force for connection. MUSIC_CAN strives to actively bring people together through the act of creative, collaborative musical performance. Through the process of making networked music, we hope to create a venue to expose listeners to inclusiveness, positivity, love, and different philosophical systems to help us all become more aware and compassionate.

The MUSIC_CAN is student work designed and created by Noah Kernis and Jake Sherwood. Jake and Nikhil Kumar will be performing with the MUSIC_CAN at NMF 2020. All 3 are 2021 M.P.S. Candidates at the New York University, Tisch School of the Arts, Interactive Telecommunications Program. 

## Functionality 

### Usage
1) Create repo dir 
```
mkdir music_can
```
2) Enter dir  
```
cd music_can
```
3) Clone repo 
```
git clone https://github.com/lucidprojects/dueling_distant_banjos.git
```

#### Arduino
3) Enter Arduino dir 
```
cd midi_can
```
4) Create 'secrets.h' - This file is ignored from the repo for privacy
```
nano secrets.h
``` 

5) Paste, update with your credientials and save. 
```
#define _SSID "YOUR_WIFI_NETWORK"
#define _PASSWORD "YOUR_WIFI_PASSWORD"
```
6) Connect MIDI_CAN usb to computer
7) Compile,upload to MUSIC_CAN and go through calibration. ([see calibration steps](#calibration)) 

#### Network
8) Update network settings 
     1) Set remote and local IP addresss /server/config/config.js
     ```
     localArduino: process.env.LOCAL_ARDUINO || '127.0.0.1', //UPDATE YOUR LOCAL ARDUINO IP
	remoteHost: process.env.REMOTE_HOST || '127.0.0.1', //UPDATE YOUR PERFORMANCE PARTNERS IP
     ```
     2) Set local Server IP - line 14 udp_server.h
     ```
     IPAddress localServer(192, XXX, XXX, XXX); //UPDATE TO YOUR LOCAL LAPTOP/SERVER IP
     ``` 
     3) Open agreed upon w/ partner UDP port on router 
9) Make sure all require node packages are installed.  In the server dir run
```
npm install
```

10) Once config is set and npm packages installed, run node server
```
node index.js
```
11) visit localhost:5050 - web admin to allow you to confirm network config and make minor changes
12) Network debug cmds
```
Cmd for checking open port
nc -z -v -u XX.XX.XXX.XXX XXXX  (partner IP and port)

cmd for checking public IP 
curl ifconfig.io  
```

### Encoder Functions
0) explore - different instruments all 8 channels
     1) single click selects mode
1) ch select
     1) single click selects channel and exits back to Default
2) chords (use ch select or enc ch select) - w pitch bend
	1) 4 cap btns play chords (C,F,G,Am)	
     2) default - encoder selects channel 
     3) long press switches to switch octave mode
     	1) enc selects octaves (1-6)
     4) Single click exits back to cycling through channels
     5) slides all work to pitch bend the channel done <s>(in progress - currently ch is hard coded)</s>
     6) double click exits back to Default
3) scales (use ch select) 
	1) (all 8 cap sensors)
4) scales w/ pitch bend (use ch select)
	1) 4cap btns scale notes pitch bend sliders
     2) double click exits back to Default
5) slides as slides or notes - defaults to slides (1)
	1) single click from default mode (enc == 5) toggles boolean
	2) double click exits back to Default
6) pot mode
     1) rotating encoder scrolls through 1-110 audible MIDI note vals
     2) double click exits back to Default	
7) <s>volume adjust per channel - to play with things on and off</s><br>
<s> 1) not started - need to add handling to adjust volume per channel</s>
8) broadcast
  	1) rotating encoder scrolls through channels
     2) single click toggles send 1/0
     3) double click exits back to Default
9) track mute on/off
  	1) tappng cap sensor turns sends cmd to mute on/off pre-mapped tracks
     2) requires setup in DAW for MIDI cmd mapping to respective tracks.


### Playing Music
1) use explore mode to experiment with different didgital instruments set up in the DAW
2) use channel select to selet channels for scales
3) scales play preconfigured minor or major scale (all 8 cap sensors)
4) scales pb (pitch bend) pitch bend the scale notes (4 cap buttons as scale notes, slides pb)
5) slides as notes - configure slide sensors to be notes instead
6) use pot for adjust basenote value sent in explore mode
7) <s>volume adjust - not done </s> using mute on/off instead
8) broadcast sets which channels to sent to partner
9) mute on/off preconfigured tracks in DAW


### Calibration
CALIBRATION - iterate through an array of sensors and calibrate each sensor.  Storing values in an array of mins/maxes.
1) Can calibration will run first - canCalibrationTime  eg first 10 seconds - used as threshold to prevent cap readings from holding the can
     1) OLED will show S: C (for can) and V: (CAN_SENSOR_VALUE)
2) Set calibration time per sensor in calibrationTime var, used in runSensorCalibration() fn
3) During calibration tap and release sensor to set min & max values.  It will iterate through the 8 capacitve sensors
     1) OLED will show S: (SENSOR_NUMBER) 
     2) V: (SENSORMAX)
	
     • Assigns min sensorMin[] in sensorMinMIN & max sensorMax[] in sensorMaxMAX
	use these vars for various thresholds

### OLED Info
![MUSIC_CAN Oled details](https://raw.githubusercontent.com/lucidprojects/dueling_distant_banjos/master/img/midi_can_oled_all.jpg)
                          <!-- https://github.com/lucidprojects/dueling_distant_banjos/blob/master/img/midi_can_oled_all.jpg     -->

### Calibration and Modes video 
[![midi_can oled](http://img.youtube.com/vi/G5dJEBQeuGQ/0.jpg)](http://www.youtube.com/watch?v=G5dJEBQeuGQ "MIDI_CAN")

(clicks through to youtube)

## Resources

### Reference

- [traceroute-js](https://github.com/frnkst/traceroute-js/blob/master/traceroute.js)
	- [Building Traceroute in JavaScript](https://medium.com/@frnkst_/building-traceroute-in-javascript-eea519385af1)

### Issues

- If node-gyp throws error and file contains `npm_config_node_gyp`
	- [node-gyp error installing fiber@2.0.2 on macOS Catalina](npm_config_node_gyp)

### Helpful software
[MIDI Monitor](https://www.macupdate.com/app/mac/9950/midi-monitor)
[LoopBack](https://rogueamoeba.com/loopback/)
[SoundFlower - for Catalina](https://github.com/mattingalls/Soundflower/releases/tag/2.0b2)