'use strict';

const UDPServer = require('./src/UDPServer')
const HTTPServer = require('./src/HTTPServer')
const {
	log
} = require('./util/log')

// NOTE: 
// 		 in order for http server to interact with data from udp server,
// 		 will most likely need to add methods to udp server that return data.
// 		 When the http server inits, will need to consume the initialized udp server.
// 		 The web page will initially control looper, so need to add api routes 
// 		 to http server and abilities to web page. 	 

// -----> Start UDP Server <-----

const udpServer = new UDPServer(log)

udpServer.run()

// -----> Start HTTP Server <-----

const httpServer = new HTTPServer(udpServer, log)

httpServer.run()