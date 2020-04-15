'use strict';

module.default = {
	host: process.env.HOST || '127.0.0.1',
	port: process.env.PORT || 5050, 
	timeout: 10000,
	log: "./log/"
};
