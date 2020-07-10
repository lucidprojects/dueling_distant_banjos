/*
   adapted from https://github.com/rodrigoms2004/ServerSocketTCP_UDP/blob/master/util/loggerTool.js
udp_server.js
	created 04/15/2020
*/
'use strict';

// NOTE: notes on use
// 		 - const { log } = require('./util/loggerTool')
// 		 - log("udp_server", "error", error)
// 		 - log("udp_server", "info", msg.toString() + ` | Received ${msg.length} bytes from ${info.address}:${info.port}`)
// 		 -  log("udp_server", "info", 'Server is listening at port ' + port)
// 		 -  log("udp_server", "info", 'Server ip :' + ipaddr)
// 		 -  log("udp_server", "info", 'Server is IP4/IP6 : ' + family)


const path = require('path')
const winston = require('winston')
const moment = require('moment')

const filename = path.join(__dirname, +'log/' + moment().format('YYYYMMDDHHmmss') + '.log')

const transports = [
	new winston.transports.File({
		filename: filename
	}),
	// new winston.transports.Console({
	// 	colorize: true
	// }),
	new winston.transports.Console({
		format: winston.format.combine(
			winston.format.colorize(),
			winston.format.simple()
		)
	})
]

const logger = winston.createLogger({
	transports: transports
})

const log = async (service, level, msg) => {
	logger.log({
		timestamp: moment().format('YYYY-MM-DD HH:mm:ss.SSSS'),
		service: service,
		level: level,
		message: msg
	})
}

module.exports = {
	log
}