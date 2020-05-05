'use strict';

module.exports = {
	host: process.env.HOST || '127.0.0.1',
	port: process.env.WEB_PORT || 5050,
	udpPort: process.env.UDP_PORT || 5000,
	trPort: process.env.TR_PORT || 5500,
	localArduino: process.env.LOCAL_ARDUINO || '192.168.0.169',
	remoteHost: process.env.REMOTE_HOST || '66.108.97.246',
	timeout: 10000,
	log: "./log/"
};
