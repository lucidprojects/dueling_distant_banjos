/*
   
   adapted from https: //github.com/frnkst/traceroute-js
   created 04/17/2020
*/
'use strict';

const dgram = require('dgram');
const rSocket = require('raw-socket');
const dns = require('dns-then');
const config = require('../config/config');

// TODO:
// 1) Return data needed for midi stuff
// 2) Remove DNS?
// 3) Clean up code

const icmpSocket = rSocket.createSocket({
	protocol: rSocket.Protocol.ICMP
});

class Traceroute {
	constructor() {
		this.udpSocket = dgram.createSocket('udp4');
		this.icmpSocket = rSocket.createSocket({protocol: rSocket.Protocol.ICMP});
		this.MAX_HOPS;
		this.MAX_TIMEOUT; 
		this.DESTINATION_HOST;
		this.DESTINATION_IP;
		this.NO_REVERSE_LOOKUP = false;
		this.port = config.trPort;
		this.ttl = 1;
		this.tries = 0;
		this.startTime;
		this.timeout;
		this.previousIP;
	}

	startTrace(params) {
		this.MAX_HOPS = params.max_hops || 10; // 64;
		this.MAX_TIMEOUT = params.max_timeout || 1000;
		this.DESTINATION_HOST = params.host || 'noahkernis.com'

		this.init();
	}

	async init() {
		this.DESTINATION_IP = await dns.lookup(this.DESTINATION_HOST);

		console.log(`traceroute to ${this.DESTINATION_HOST} (${this.DESTINATION_IP}), ${this.MAX_HOPS} hops max, 42 byte packets`);
		this.udpSocket.bind(this.port, () => this.sendPacket());

		setImmediate(() => this.listenIcmpSocket());
	}

	listenIcmpSocket() {
		icmpSocket.on('message', async (buffer, ip) => {
			let p = buffer.toString('hex').substr(100, 4);
			let portNumber = parseInt(p, 16);
			if (this.port === portNumber) {
				try {
					let symbolicAddress;
					if (!this.NO_REVERSE_LOOKUP) {
						symbolicAddress = await dns.reverse(ip);
					}
					this.handleReply(ip, symbolicAddress[0]);
				} catch (e) {
					this.handleReply(ip);
				}
			}
		});
	}

	sendPacket() {
		this.port++;

		if (this.tries >= 3) {
			this.tries = 0;
			this.ttl++;
		}
		this.tries++;

		this.udpSocket.setTTL(this.ttl);
		this.startTime = process.hrtime();
		this.udpSocket.send(new Buffer(''), 0, 0, this.port, this.DESTINATION_IP, (err) => {
			if (err) throw err;
			this.timeout = setTimeout(() => this.handleReply(), this.MAX_TIMEOUT);
		});
	}

	handleReply(ip, symbolicAddress) {
		if (this.timeout) {
			clearTimeout(this.timeout);
		}

		if (ip) {
			const elapsedTime = `${(process.hrtime(this.startTime)[1] / 1000000).toFixed(3)} ms`;

			if (ip === this.previousIP) {
				process.stdout.write(`  ${elapsedTime}`);
			} else if (this.tries === 1) {
				process.stdout.write(`\n ${this.ttl}  ${symbolicAddress ? symbolicAddress : ip} (${ip}) ${elapsedTime}`);
			} else {
				process.stdout.write(`\n    ${symbolicAddress ? symbolicAddress : ip} (${ip}) ${elapsedTime}`);
			}
		} else {
			if (this.tries === 1) {
				process.stdout.write(`\n ${this.ttl}  * `);
			} else {
				process.stdout.write(`* `);
			}
		}

		if ((ip == this.DESTINATION_IP && this.tries === 3) || this.ttl >= this.MAX_HOPS) {
			process.exit();
		}

		this.previousIP = ip;
		setImmediate(() => this.sendPacket());
	}
}

module.exports = Traceroute;
