'use strict';

module.exports = {
	host: process.env.HOST || '127.0.0.0',
	webPort: process.env.WEB_PORT || 5050,
	udpPort: process.env.UDP_PORT || 5000,
	trPort: process.env.TR_PORT || 5500,
	localArduino: process.env.LOCAL_ARDUINO || '192.168.0.169',
	remoteHost: process.env.REMOTE_RPI || '66.108.97.246', // '127.0.0.1',
	timeout: 10000,
	log: "./log/"
};
