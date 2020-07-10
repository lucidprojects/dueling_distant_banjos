'use strict';

const UDPServer = require('./src/UDPServer')
const HTTPServer = require('./src/HTTPServer')
const config = require('./config/config');

// TODO: implement logger in `util`
// NOTE: 
// 		 in order for http server to interact with data from udp server,
// 		 will most likely need to add methods to udp server that return data.
// 		 When the http server inits, will need to consume the initialized udp server.
// 		The web page will initially control looper, so need to add api routes 

// 		to http server and abilities to web page. 
// 		 

// -----> Start UDP Server <-----

const udpServer = new UDPServer();

udpServer.run();

// -----> Start API <-----

HTTPServer.listen(config.port, (err) => {
	if (err) {
		console.log('api_server', 'error', err);
	}

	console.log('api_server', 'info', 'listening on port:', config.port);
})