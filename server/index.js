'use strict';

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