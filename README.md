# MUSIC_CAN aka MIDD_CAN

## Concept
MUSIC_CAN is an ode to the original project name “MIDI_CAN.”  Built to embrace all that music can do for our psyche, culture, and society,  MUSIC_CAN is a networked, multimode, instrument / MIDI controller.
 
During the pandemic and the new socially distant world we find ourselves in, we are now more separate than ever.  With rolling stay at home orders, what can we do to continue our collaborative nature?  How can we still connect even though we can't physically?
 
We wanted to make something to connect us, even though we are miles apart. So, we built the MUSIC_CAN. 
 
With an Arduino Nano 33 IoT microcontroller, an 8 sensor capacitive touch array and a rotary encoder for its hardware and a software stack that features Arduino, NodeJS, MIDI, UDP, and your DAW of choice, the MIDI_CAN allows for multiple input modes and musical interactions. The unique inputs on the MUSIC_CAN allow for creating distinctive and pleasing electronic music. The MUSIC_CAN can be played by itself or collaboratively over a network and is designed to be used with one or more musicians / devices. 
 
Music is a powerful force for connection. MUSIC_CAN strives to actively bring people together through the act of creative, collaborative musical performance. Through the process of making networked music, we hope to create a venue to expose listeners to inclusiveness, positivity, love, and different philosophical systems to help us all become more aware and compassionate.

The MUSIC_CAN is student work designed and created by Noah Kernis and Jake Sherwood. Jake and Nikhil Kumar will be performing with the MUSIC_CAN at NMF 2020. All 3 are 2021 M.P.S. Candidates at the New York University, Tisch School of the Arts, Interactive Telecommunications Program. 

## Functionality 

### Usage
1) Create repo dir ```mkdir music_can```
1) Clone repo ```git clone https://github.com/lucidprojects/dueling_distant_banjos.git```
2) Enter dir  ```cd music_can```

#### Arduino
3) Enter Arduino dir ```cd midi_can```
4) Create a 'secrets.h' e.g. ```nano secrets.h``` 
   This file is ignored from the repo for privacy
5) Paste, update with your credientials and save. 
     ```
     #define _SSID "YOUR_WIFI_NETWORK"
     #define _PASSWORD "YOUR_WIFI_PASSWORD"`
     ```
6) Connect MIDI_CAN usb to computer
7) Compile,upload to MUSIC_CAN and go through calibration. ([see calibration steps](#calibration)) 

#### Network
8) Update network settings 
     1) Set remote and local IP addresss /server/config/config.js
     ```
     localArduino: process.env.LOCAL_ARDUINO || '127.0.0.1', //YOUR LOCAL ARDUINO IP
	remoteHost: process.env.REMOTE_HOST || '127.0.0.1', //YOUR PERFORMANCE PARTNERS IP
     ```
     2) Set local Server IP 
     ```IPAddress localServer(192, XXX, XXX, XXX);```
     3) Open agreed upon w/ partner UDP port on router 
     4) Once config is set run node server
     ```node index.js```
     5) Network debug cmds
     ```
     Cmd for checking open port
     nc -z -v -u XX.XX.XXX.XXX XXXX  (partner IP and port)

     cmd for checking public IP 
     curl ifconfig.io  
     ```




### Encoder Functions

New structure notes:

0) explore - different instruments all 8 channels
     1) single click selects mode
1) ch select
     1) single click selects channel and exits back to Default
2) chords (use ch select or enc ch select) - w pitch bend
	 1) 4 cap btns play chords (C,F,G,Am)	
     2) default - encoder selects channel 
     3) long press switches to switch octave mode
     	1) fixed <s>this mode is a litte buggy</s>
     	2) enc selects octaves (1-6)
     	3) fixed <s>inact you have to press one of the cap btns to set octave (this is the buggy part)</s>
     4) Single click exits back to cycling through channels
     5) slides all work to pitch bend the channel done <s>(in progress - currently ch is hard coded)</s>
     6) double click exits back to Default
3) scales (use ch select) 
	 1) (all 8 cap sensors) - fixed <s>need to figure out why slides play scale & explore???</s>
4) scales w/ pitch bend (use ch select)
	 1) 4cap btns scale notes pitch bend sliders
     2) double click exits back to Default
5) slides as slides or notes - defaults to slides (1)
	 1) single click from default mode (enc == 5) toggles boolean
	 2) double click exits back to Default
6) pot mode
     1) rotating encoder scrolls through 1-110 audible MIDI note vals
     2) double click exits back to Default	
7) volume adjust per channel - to play with things on and off
	 1) not started - need to add handling to adjust volume per channel
8) broadcast
  	 1) rotating encoder scrolls through channels
     2) single click toggles send 1/0
     3) double click exits back to Default

<s>
0) Default / start 
     1) single click selects mode
1) Channel select mode  
     1) single click selects channel and exits back to Default (need to set channel prior to ADSR)
2) ADSR
     1) click and hold selects adsrType (attack, decay, sustain, or release)
     2) rotating encoder scrolls through 1 -127 ADSR vals
     3) single click brings you back to adsrType selection
     4) double click exits back to Default
3) Pot mode
     1) rotating encoder scrolls through 1 -110 audible MIID note vals
     2) double click exits back to Default
4) SLIDES as slides or notes mode
     1) single click from default mode (enc == 4) toggles boolean
5) All cap inputs as scale buttons 
     1) single click from default mode (enc == 5) toggles boolean
6) Record / loop 
     1) single click sends testing MIDI cmd *note incomplete
7) Broadcast n CHANNEL
     1) rotating encoder scrolls through channels
     2) single click toggles send 1/0
</s>

## Resources

### Reference

- [traceroute-js](https://github.com/frnkst/traceroute-js/blob/master/traceroute.js)
	- [Building Traceroute in JavaScript](https://medium.com/@frnkst_/building-traceroute-in-javascript-eea519385af1)

### Issues

- If node-gyp throws error and file contains `npm_config_node_gyp`
	- [node-gyp error installing fiber@2.0.2 on macOS Catalina](npm_config_node_gyp)