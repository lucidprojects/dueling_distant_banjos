'use strict';

module.exports = {
	host: process.env.HOST || '127.0.0.1',
	webPort: process.env.WEB_PORT || 5050,
	udpPort: process.env.UDP_PORT || 5000,
	trPort: process.env.TR_PORT || 5500,
	localCanHost: process.env.LOCAL_CAN_HOST || '192.168.0.169',
	remoteHost: process.env.REMOTE_HOST || '127.0.0.1',
	timeout: 10000,
	log: "./log/"
};
