/*
   adapted from https://github.com/rodrigoms2004/ServerSocketTCP_UDP/blob/master/udp_server.js
   created 04/15/2020
*/
'use strict';

// TODO: ust `dtls` (https://github.com/nodertc/dtls/blob/master/example.js)

const dgram = require('dgram');
const config = require('../config/config');
// const { log } = require('./util/loggerTool');

class UDPServer {
	constructor() {
		this.server = dgram.createSocket('udp4');
		this.localArduino = config.localArduino;
		this.remoteHost = config.remoteHost;
		this.port = config.udpPort;
	}

	run() {
		this.onClose();
		this.onError();
		this.onMessage();
		this.onListen();

		this.server.bind(this.port)
	}

	onClose() {
		//emits after the socket is closed using socket.close()
		this.server.on('close', () => {
			console.log("udp_server", "info", 'Socket is closed!');
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
		this.server.on('message', (buffer, info) => {
			console.log("udp_server", `| Received ${buffer.length} bytes from ${info.address}:${info.port}`);
			console.log("cmd", buffer[0]);
			console.log("data1", buffer[1]);
			console.log("dat2", buffer[2]);
			console.log('');
			// log("udp_server", "info", msg.toString() + ` | Received ${msg.length} bytes from ${info.address}:${info.port}`)

			this.handleMessage(buffer, info)
		});

	}

	onListen() {
		//emits when socket is ready and listening for datagram msgs
		this.server.on('listening', () => {
			const address = this.server.address();
			const port = address.port;
			const family = address.family;
			const ipaddr = address.address;

			console.log("udp_server", "info", 'listening on port: ' + port);
			// log("udp_server", "info", 'Server is listening at port ' + port)
			console.log("udp_server", "info", 'Server ip: ' + ipaddr);
			// log("udp_server", "info", 'Server ip :' + ipaddr)
			console.log("udp_server", "info", 'Server ipv: ' + family);
			// log("udp_server", "info", 'Server is IP4/IP6 : ' + family)
		});
	}

	handleMessage(buffer, info) {
		switch (`${info.address}`) {
			case this.localArduino:
				this.sendMessage(buffer, this.remoteHost, this.port);
				break;

			case this.remoteHost:
				this.sendMessage(buffer, this.localArduino, this.port);
				break;

			default:
				console.log("info", "received udp from unknown source", `${info.address}:${info.port}`);
				break;
		}
	}

	sendMessage(buffer, host, port) {
		const client = dgram.createSocket('udp4');

		client.send(buffer, port, host, (error, bytes) => {
			if (error) {
				console.log("udp_server", "error", error);
				// log("udp_server", "error", error)
				client.close();
			} else {
				console.log("udp_server", "info", 'data sent to', `${host}:${port}`);
				// log("udp_server", "info", 'Data sent !!!')
				client.close();
			}
		});
	}
}

module.exports = UDPServer;