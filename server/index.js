'use strict';

const UDPServer = require('./src/UDPServer')
const HTTPServer = require('./src/HTTPServer')
const config = require('./config/config');

// -----> Start UDP Server <-----

const udpServer = new UDPServer();

// udpServer.run();

// -----> Start API <-----

HTTPServer.listen(config.port, (err) => {
	if (err) {
		console.log('api_server', 'error', err);
	}

	console.log('api_server', 'info', 'listening on port:', config.port);
})