/*
   
   adapted from https://github.com/rodrigoms2004/ServerSocketTCP_UDP/blob/master/udp_server.js
   created 04/15/2020
*/
'use strict';

// TODO:
// 1) Test!
// 2) Send data to remote rpi
// 3) Receive data from remote rpi and send to local ard

const dgram = require('dgram');
const config = require('../config/config');

// const {
// 	log
// } = require('./util/loggerTool');

class UDPServer {
	constructor() {
		this.server = dgram.createSocket('udp4');
	}

	run() {
		this.onClose();
		this.onError();
		this.onMessage();
		this.onListen();

		this.server.bind(config.udpPort)
	}

	onClose() {
		//emits after the socket is closed using socket.close()
		this.server.on('close', () => {
			console.log("udp_server", "info", 'Socket is closed !');
			// log("udp_server", "info", 'Socket is closed !')
		})
	}

	onError() {
		// emits when any error occurs
		this.server.on('error', (error) => {
			console.log("udp_server", "error", error);
			// log("udp_server", "error", error)
			this.server.close();
		});
	}

	onMessage() {
		// receive msg
		this.server.on('message', (msg, info) => {
			// console.log("udp_server", "info", msg.toString() + ` | Received ${msg.length} bytes from ${info.address}:${info.port}`);
			// log("udp_server", "info", msg.toString() + ` | Received ${msg.length} bytes from ${info.address}:${info.port}`)

			console.log(msg[0]);
			console.log(msg[1]);
			console.log(msg[2]);
			console.log('');
		});

		// TODO: data will go to arduino
	}

	onListen() {
		//emits when socket is ready and listening for datagram msgs
		this.server.on('listening', () => {
			const address = this.server.address();
			const port = address.port;
			const family = address.family;
			const ipaddr = address.address;

			console.log("udp_server", "info", 'Server is listening at port ' + port);
			// log("udp_server", "info", 'Server is listening at port ' + port)
			console.log("udp_server", "info", 'Server ip :' + ipaddr);
			// log("udp_server", "info", 'Server ip :' + ipaddr)
			console.log("udp_server", "info", 'Server ipv : ' + family);
			// log("udp_server", "info", 'Server is IP4/IP6 : ' + family)
		});
	}

	sendMessage(message) {
		// TODO: 
		// Use server obj or use this new client??
		const client = dgram.createSocket('udp4');

		const data = {
			body: message,
			timestamp: new Date().toJSON()
		};

		const buffer = Buffer.from(JSON.stringify(data));

		client.send(buffer, "remote_port", "remote_host", (error, bytes) => {
			if (error) {
				console.log("udp_server", "error", error);
				// log("udp_server", "error", error)
				client.close();
			} else {
				console.log("udp_server", "info", 'Data sent !!!');
				// log("udp_server", "info", 'Data sent !!!')
				client.close();
			}
		});

		// TODO: need to trigger when data to send from arduino
	}
}

module.exports = UDPServer;
