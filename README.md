# WebCan

## Functionality 

### Encoder

New structure notes:
0) explore - different instruments all 8 channels
     1) single click selects mode
1) ch select
     1) single click selects channel and exits back to Default (need to set channel prior to ADSR)
2) chords (use ch select or enc ch select)  -  w pitch bend
	 1) 4 cap btns play chords (C,F,G,Am)	
     2) default - encoder selects channel 
     3) long press switches to switch octave mode
     	1) this mode is a litte buggy
     	2) enc selects octaves (1-6)
     	3) to inact you have to press one of the cpa btns to set octave (this is the buggy part)
     4) single click exits back to cycling through channels
     5) slides all work to pitch bend the channel (in progress - currently ch is hard coded)
     6) double click exits back to Default
3) scales (use ch select) 
	 1) (all 8 cap sensors) - need to figure out why slides play scale & explore???
4) scales (use ch select)
	 1) 4cap btns scale notes pitch bend sliders
     2) double click exits back to Default
5) slides as slides - defaults to slides (1)
	 1) single click from default mode (enc == 5) toggles boolean
	 2) double click exits back to Default
6) pot mode
     1) rotating encoder scrolls through 1 -110 audible MIID note vals
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