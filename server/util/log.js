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

const winston = require('winston')
const moment = require('moment')
require('winston-daily-rotate-file')
const conf = require('../config/config')

const transports = [
	new winston.transports.DailyRotateFile({
		name: 'logs',
		filename: conf.logdir + 'access%DATE%.log',
		maxSize: '1000k',
		maxFiles: '15d',
		zippedArchive: false
	}),
	new winston.transports.DailyRotateFile({
		level: 'error',
		name: 'logs',
		filename: conf.logdir + 'error%DATE%.log',
		maxSize: '1000k',
		maxFiles: '15d',
		zippedArchive: false
	}),
	new winston.transports.Console({
		colorize: true
	})
]

const logger = winston.createLogger({
	// format: winston.format.json(),
	// defaultMeta: { timestamp: new Date() },
	transports: transports
})


// if (process.env.NODE_ENV !== 'production') {
//     logger.add(new winston.transports.Console({ format: winston.format.simple() }))
// }

const log = async (service, level, msg) => {
	logger.log({
		timestamp: moment().format('YYYY-MM-DD HH:mm:ss.SSSS'),
		service: service,
		level: level,
		message: msg
	})
} // end infoLog


module.exports = {
	log
}