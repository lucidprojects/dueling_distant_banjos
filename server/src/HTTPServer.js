'use strict'

const express = require('express');
const http = require('http');
const bodyParser = require('body-parser');
const helmet = require('helmet');
const morgan = require('morgan');
const winston = require('winston')
const config = require('../config/config');
// const Traceroute = require('./Traceroute')

var log
var looper
var logger

// -----> Arduino <-----

const getArduino = () => {
	return new Promise((resolve) => {
		const options = {
			hostname: config.localArduino,
			port: 5555,
			path: '/',
			method: 'GET'
		};

		log('http_server', 'info', 'GET', config.localArduino)

		let resData = "";
		const req = http.request(options, (res) => {
			log('http_server', 'info', 'status code:', res.statusCode);

			res.on('data', (chunk) => {
				resData += chunk;
			});

			res.on('end', () => {
				let parsedData = JSON.parse(resData);

				if (parsedData.message == "success") {
					log('http_server', 'error', 'failed to get data from arduino');
				} else {
					log('http_server', 'info', 'successfully requested data from arduino');
				}

				resolve(parsedData);
			});
		});

		req.on('error', err => {
			log('http_server', 'error', err);
		});

		req.end();
	});
}

const putArduino = (rawData) => {
	const data = JSON.stringify({
		capBuff: parseInt(rawData.capBuff),
	})

	const options = {
		hostname: config.localArduino,
		port: 5555,
		path: '/',
		method: 'PUT',
		headers: {
			'data': data
		}
	};

	log('http_server', 'info', 'PUT', config.localArduino)

	let resData = "";
	const req = http.request(options, (res) => {
		log('http_server', 'info', 'status code:', res.statusCode);

		res.on('data', (chunk) => {
			resData += chunk;
		});

		res.on('end', () => {
			if (JSON.parse(resData).message == "success") {
				log('http_server', 'info', 'arduino updated');
			} else {
				log('http_server', 'error', 'failed to updated arduino');
			}

			return new Promise((resolve) => resolve());
		})
	});

	req.on('error', err => {
		log('http_server', 'error', err);
	});

	req.end();
}

// -----> Create HTTP Server <-----

const app = express();

const server = http.createServer(app);

const init = () => {
	// -----> Set Middleware <-----

	app.use(bodyParser.json());
	app.use(morgan('combined', {
		stream: logger.stream
	}))
	app.use(helmet());

	// -----> Set Static Directory <-----

	app.use(express.static('public'));

	// -----> Default Error Handler <-----

	app.use(function (err, req, res, next) {
		res.locals.message = err.message
		res.locals.error = err
		// res.locals.error = req.app.get('env') === 'development' ? err : {}
		// set locals, only providing error in development

		// log error
		winston.error(`${err.status || 500} - ${err.message} - ${req.originalUrl} - ${req.method} - ${req.ip}`)

		// render the error page
		res.status(err.status || 500)
		res.render('error')
	})

	// -----> API Routes <-----

	app.get('/api/data', async (req, res) => {
		const data = await getArduino(req.body)
		// log(data);

		const state = {
			localIp: config.host,
			localPort: config.udpPort,
			remoteIp: config.remoteHost,
			remotePort: config.udpPort,
			capBuff: parseInt(data.capBuff),
		}

		res.json(state)
		// res.status(500).json({ error: 'message' }) // error stuff
	})

	app.put('/api/data', async (req, res) => {
		await putArduino(req.body);

		res.json({})
		// res.status(500).json({ error: 'message' }) // error stuff
	})

	app.get('/looper/record', async (req, res) => {
		looper.record()

		res.status(200).end()
	})

	app.get('/looper/play', async (req, res) => {
		looper.play()

		res.status(200).end()
	})

	app.get('/looper/stop', async (req, res) => {
		looper.stop()

		res.status(200).end()
	})
}

class HTTPServer {
	constructor(_log, _logger, _looper) {
		this.server = server

		log = _log
		logger = _logger
		looper = _looper
	}

	run() {
		init()

		this.server.listen(config.port, (err) => {
			if (err) {
				log('http_server', 'error', err)
			}

			log('http_server', 'info', 'listening on port: ' + config.port)
		})
	}
}

module.exports = HTTPServer