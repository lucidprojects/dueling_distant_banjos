'use strict';

const HTTPServer = require('./src/HTTPServer')
const Looper = require('./src/Looper')
const UDPServer = require('./src/UDPServer')
const {
	log
} = require('./util/log')

// NOTE: 
// 		 in order for http server to interact with data from udp server,
// 		 will most likely need to add methods to udp server that return data.
// 		 When the http server inits, will need to consume the initialized udp server.
// 		 The web page will initially control looper, so need to add api routes 
// 		 to http server and abilities to web page. 	 

// -----> Create Looper <-----

const looper = new Looper(log)

// -----> Start UDP Server <-----

const udpServer = new UDPServer(log, looper)

udpServer.run()

// -----> Start UDP Server <-----

const httpServer = new HTTPServer(log, looper)

httpServer.run()