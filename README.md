# WebCan

## Functionality 

### Encoder

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

## Resources

### Reference

- [traceroute-js](https://github.com/frnkst/traceroute-js/blob/master/traceroute.js)
	- [Building Traceroute in JavaScript](https://medium.com/@frnkst_/building-traceroute-in-javascript-eea519385af1)

### Issues

- If node-gyp throws error and file contains `npm_config_node_gyp`
	- [node-gyp error installing fiber@2.0.2 on macOS Catalina](npm_config_node_gyp)