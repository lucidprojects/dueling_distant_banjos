'use strict';

module.exports = {
	host: process.env.HOST || '127.0.0.1',
	webPort: process.env.WEB_PORT || 5050,
	udpPort: process.env.UDP_PORT || 5000,
	trPort: process.env.TR_PORT || 5500,
	timeout: 10000,
	log: "./log/"
};
