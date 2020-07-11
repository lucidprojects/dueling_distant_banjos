/*
   adapted from https://github.com/rodrigoms2004/ServerSocketTCP_UDP/blob/master/util/loggerTool.js
udp_server.js
	created 04/15/2020
*/
'use strict';

const path = require('path')
const winston = require('winston')
const moment = require('moment')

const filename = path.join(__dirname, '../log/' + moment().format('YYYYMMDDHHmmss') + '.log')

// TODO: create custom format: https://github.com/winstonjs/winston#formats

const transports = [
	new winston.transports.File({
		filename: filename
	}),
	new winston.transports.Console({
		handleExceptions: true,
		json: false,
		colorize: true,
	})
	// new winston.transports.Console({
	// 	format: winston.format.combine(
	// 		winston.format.colorize(),
	// 		winston.format.simple()
	// 	)
	// })
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