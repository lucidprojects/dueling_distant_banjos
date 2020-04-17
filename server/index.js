'use strict';

// const server = require('./src/HTTPServer')
// const UDPServer = require('./src/UDPServer')
const Traceroute = require('./src/Traceroute')
// const config = require('./config/config');

// NOTE: -----> Start Server <-----

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

// const udpServer = new UDPServer();

// udpServer.run();

new Traceroute().startTrace({});
