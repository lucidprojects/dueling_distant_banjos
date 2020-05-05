'use strict'

const express = require('express');
const helmet = require('helmet');
const http = require('http');
const logger = require('morgan');
const config = require('../config/config');
const fs = require('fs');

// -----> Create Config File For Client <-----

const toWrite = `
	const API_URL = '${config.host}';
	const API_PORT = '${config.port}';
`

fs.writeFileSync(__dirname + '/../public/js/env.js', toWrite)

// -----> Create HTTP Server <-----

const app = express();

const server = http.createServer(app);

// -----> Set Middleware <-----

app.use(logger('common'));
app.use(helmet());

// -----> Set Static Directory <-----

app.use(express.static('public'));

// -----> API Routes <-----

app.get('/api/data', function (req, res) {
	// TODO: get data from arduino then send back to client

	let tempState = {
		localIp: '1',
		localPort: '2',
		remoteIp: '3',
		remotePort: '4',
		capBuff: '5',
	};

	res.json(tempState)
	// res.status(500).json({ error: 'message' }) // error stuff
})

app.post('/api/data', function (req, res) {
	res.send('POST request to homepage')
})

module.exports = server;
