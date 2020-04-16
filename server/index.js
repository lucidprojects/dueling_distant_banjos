'use strict';

const server = require('./server/src/server')
const config = require('../config/config');

// NOTE: -----> Start Server <-----

server.listen(config.port, (err) => {
	var start = true;

	if (err) {
		console.log('Something went wrong', err);
	}

	console.log(`Server started on port ${config.port}...`);

	if (start) {
		// make sure things that need starting once don't start again 
		start = false;
	}
})