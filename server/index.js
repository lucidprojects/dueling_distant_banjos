'use strict';

const UDPServer = require('./src/UDPServer')
const Traceroute = require('./src/Traceroute')
const HTTPServer = require('./src/HTTPServer')


// server.listen(config.port, (err) => {
// 	var start = true;

// 	if (err) {
// 		console.log('Something went wrong', err);
// 	}

// 	console.log(`Server started on port ${config.port}...`);

// 	if (start) {
// 		// make sure things that need starting once don't start again 
// 		start = false;
// 	}
// })

// NOTE: -----> Start UDP Server <-----

const udpServer = new UDPServer();


udpServer.run();


// HTTP server will accept udpserv as arg to us it. same with TR

// new Traceroute().startTrace({});

// const server = new HTTPServer(udpServer, )
