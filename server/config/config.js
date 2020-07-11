'use strict';

module.exports = {
	host: process.env.HOST || '127.0.0.1',
	port: process.env.WEB_PORT || 5050,
	udpPort: process.env.UDP_PORT || 5000,
	trPort: process.env.TR_PORT || 5500,
	localArduino: process.env.LOCAL_ARDUINO || '192.168.86.249', // local Arduino address
	remoteHost: process.env.REMOTE_HOST || '38.105.253.109', // remote server address
}