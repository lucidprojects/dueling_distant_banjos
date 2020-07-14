'use strict';

const keypress = require('keypress')
const HTTPServer = require('./src/HTTPServer')
const Looper = require('./src/Looper')
const UDPServer = require('./src/UDPServer')
const {
	log,
	logger
} = require('./util/log')

// -----> Create Looper <-----

const looper = new Looper(log)

// -----> Start UDP Server <-----

const udpServer = new UDPServer(log, looper)

udpServer.run()

// -----> Start UDP Server <-----

const httpServer = new HTTPServer(log, logger, looper)

httpServer.run()

// Keypress handler

const onKeyPress = (ch, key) => {
	if ((key && key.ctrl && key.name == 'c') || key.name == 'q') {
		log('process', 'info', 'we got a quitter here')
		process.exit()
	}

	if (key.name == 'space') {
		if (looper.hasData && !looper.looping) {
			looper.start()
		} else {
			looper.reset()
		}
	}

	if (key.name == 'r') {
		looper.toggleRecording()
	}

	if (key.name == 'm') {
		if (looper.looping) {
			looper.mute()
		}
	}
}

keypress(process.stdin)
process.stdin.setRawMode(true)
process.stdin.resume()
process.stdin.on('keypress', onKeyPress)