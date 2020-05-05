'use strict';

// const UDPServer = require('./src/UDPServer')
// const Traceroute = require('./src/Traceroute')
const HTTPServer = require('./src/HTTPServer')
const config = require('./config/config');

// -----> Start UDP Server <-----

// const udpServer = new UDPServer();
// udpServer.run();

// HTTP server will accept udpserv as arg to us it. same with TR

// new Traceroute().startTrace({});

// -----> Start Server <-----

HTTPServer.listen(config.port, (err) => {
	if (err) {
		console.log('Something went wrong', err);
	}

	console.log(`Server started on port ${config.port}...`);
})