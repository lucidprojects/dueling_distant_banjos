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

app.put('/api/data', function (req, res) {
	console.log(req.body);

	res.json({})
	// res.status(500).json({ error: 'message' }) // error stuff
})

module.exports = server;
