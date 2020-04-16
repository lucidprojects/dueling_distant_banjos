/*
   
   adapted from https: //github.com/rodrigoms2004/ServerSocketTCP_UDP/blob/master/udp_server.js
   created 02/15/2020
*/
'use strict';

const udp = require('dgram');
const conf = require('./config/config');

// const {
// 	log
// } = require('./util/loggerTool');

class UDPServer {
	constructor() {
		this.server;
	}

	run() {
		this.server = udp.createSocket('udp4');
		this.onClose();
		this.onError();
		this.onMessage();
		this.onListen();

		this.server.bind(conf.port)
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
		// emits on new datagram msg
		this.server.on('message', (msg, info) => {
			console.log("udp_server", "info", msg.toString() + ` | Received ${msg.length} bytes from ${info.address}:${info.port}`);
			// log("udp_server", "info", msg.toString() + ` | Received ${msg.length} bytes from ${info.address}:${info.port}`)

			let timestp = new Date();
			const response = {
				description: 'UDP PORT TEST BY RMS Math',
				serverPort: conf.port,
				timestamp: timestp.toJSON(),
				received: {
					message: msg.toString(),
					fromIP: info.address,
					fromPort: info.port
				}
			};

			const data = Buffer.from(JSON.stringify(response));

			//sending msg
			this.server.send(data, info.port, info.address, (error, bytes) => {
				if (error) {
					console.log("udp_server", "error", error);
					// log("udp_server", "error", error)
					client.close()
				} else {
					console.log("udp_server", "info", 'Data sent !!!');
					// log("udp_server", "info", 'Data sent !!!')
				}
			})
		})
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

}; 

module.default = UDPServer;
