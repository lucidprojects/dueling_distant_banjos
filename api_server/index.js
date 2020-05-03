/*
	created 14 Feb 2016
	by Tom Igoe

	updated 2 May 2020
	by Noah Kernis
*/

const express = require('express');
const http = require('http');
var bodyParser = require('body-parser');

const app = express();

// app.use(bodyParser.urlencoded({ extended: true }));
app.use(bodyParser.json());
// app.use(bodyParser.raw());

// this runs after the server successfully starts:
function serverStart() {
	var port = server.address().port;
	console.log('Server listening on port ' + port);
}

app.get('/', function (request, response) {
	response.write('\n');
	response.write('ROOT MOTHERFUCKER');
	response.write('\n');
	response.end();
});


app.all('/*', function (request, response) {
	console.log('Got a ' + request.method + ' request');
	console.log(request.headers);
	
	if (request.method == 'GET') {
		console.log(request.query);
	} else {
		console.log(request.body);
	}

	// send the response:
	response.send('YOU MADE A REQUEST --> ' + request.method);
	response.end();
});

function putArduino(url) {
	const data = JSON.stringify({
		buffer: 555,
		ip: '127.0.0.1'
	})

	const options = {
		hostname: url,
		port: 80,
		path: '/',
		method: 'POST',
		headers: {
			'Content-Type': 'application/json',
			'Content-Length': data.length,
			'data': data
		}
	}

	console.log("making request");

	const req = http.request(options, res => {
		console.log(`statusCode: ${res.statusCode}`)

		res.on('data', d => {
			process.stdout.write(d)
		})
	})

	req.on('error', error => {
		console.error(error)
	})

	req.end()
}

putArduino('192.168.0.169')

// start the server:
// var server = app.listen(8080, serverStart);