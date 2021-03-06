'use strict'

const express = require('express');
const http = require('http');
const bodyParser = require('body-parser');
const helmet = require('helmet');
const logger = require('morgan');
// const Traceroute = require('./Traceroute')
const config = require('../config/config');

// TODO: Run this every so often and send data to arduino
// new Traceroute().startTrace({});


// -----> Arduino <-----

const getArduino = () => {
	return new Promise((resolve) => {
		const options = {
			hostname: config.localArduino,
			port: 5555,
			path: '/',
			method: 'GET'
		};

		console.log('api_server', 'info', 'GET', config.localArduino)

		let resData = "";
		const req = http.request(options, (res) => {
			console.log('api_server', 'info', 'status code:', res.statusCode);

			res.on('data', (chunk) => {
				resData += chunk;
			});

			res.on('end', () => {
				let parsedData = JSON.parse(resData);

				// console.log(parsedData);

				if (parsedData.message == "success") {
					console.log('api_server', 'error', 'failed to get data from arduino');
				} else {
					console.log('api_server', 'info', 'successfully requested data from arduino');
				}

				resolve(parsedData);
			});
		});

		req.on('error', err => {
			console.log('api_server', 'error', err);
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

	console.log('api_server', 'info', 'PUT', config.localArduino)

	let resData = "";
	const req = http.request(options, (res) => {
		console.log('api_server', 'info', 'status code:', res.statusCode);

		res.on('data', (chunk) => {
			resData += chunk;
		});

		res.on('end', () => {
			if (JSON.parse(resData).message == "success") {
				console.log('api_server', 'info', 'arduino updated');
			} else {
				console.log('api_server', 'error', 'failed to updated arduino');
			}

			return new Promise((resolve) => resolve());
		})
	});

	req.on('error', err => {
		console.log('api_server', 'error', err);
	});

	req.end();
}

// -----> Create HTTP Server <-----

const app = express();

const server = http.createServer(app);

// -----> Set Middleware <-----

app.use(bodyParser.json());
app.use(logger('common'));
app.use(helmet());

// -----> Set Static Directory <-----

app.use(express.static('public'));

// -----> API Routes <-----

app.get('/api/data', async (req, res) => {
	const data = await getArduino(req.body);
	console.log(data);

	const state = {
		localIp: config.host,
		localPort: config.udpPort,
		remoteIp: config.remoteHost,
		remotePort: config.udpPort,
		capBuff: parseInt(data.capBuff),
	};

	res.json(state)
	// res.status(500).json({ error: 'message' }) // error stuff
})

app.put('/api/data', async (req, res) => {
	await putArduino(req.body);

	res.json({})
	// res.status(500).json({ error: 'message' }) // error stuff
})

module.exports = server;