/*
   adapted from https://github.com/rodrigoms2004/ServerSocketTCP_UDP/blob/master/udp_server.js
   created 04/15/2020
*/
'use strict'

// TODO: ust `dtls` (https://github.com/nodertc/dtls/blob/master/example.js)

const dgram = require('dgram')
const config = require('../config/config')

class UDPServer {
	constructor(log, looper) {
		this.server = dgram.createSocket('udp4')
		this.localArduino = config.localArduino
		this.remoteHost = config.remoteHost
		this.port = config.udpPort
		this.log = log
		this.looper = looper
		this.looper.onStep(this.onStep.bind(this))
	}

	run() {
		this.onClose()
		this.onError()
		this.onMessage()
		this.onListen()

		this.server.bind(this.port)
	}

	onClose() {
		this.server.on('close', () => {
			this.log("udp_server", "info", 'Socket is closed !')
		})
	}

	onError() {
		this.server.on('error', (error) => {
			this.log("udp_server", "error", error)

			this.server.close()
		})
	}

	onMessage() {
		this.server.on('message', (buffer, info) => {
			this.log("udp_server", "info", `received [${buffer[0]},${buffer[1]},${buffer[2]}] from ${info.address}:${info.port}`)

			this.handleMessage(buffer, info)
		})

	}

	onListen() {
		this.server.on('listening', () => {
			const address = this.server.address()
			const port = address.port
			const family = address.family
			const ipaddr = address.address

			this.log("udp_server", "info", 'Server is listening at port ' + port)
			this.log("udp_server", "info", 'Server ip :' + ipaddr)
			this.log("udp_server", "info", 'Server is IP4/IP6 : ' + family)
		})
	}

	handleMessage(buffer, info) {
		switch (`${info.address}`) {
			case this.localArduino:
				this.sendMessage(buffer, this.remoteHost, this.port)
				this.looper.add(buffer)
				break

			case this.remoteHost:
				this.sendMessage(buffer, this.localArduino, this.port)
				break

			default:
				this.log("info", "received udp from unknown source", `${info.address}:${info.port}`)
				break
		}
	}

	sendMessage(buffer, host, port) {
		const client = dgram.createSocket('udp4')

		client.send(buffer, port, host, (error, bytes) => {
			if (error) {
				this.log("udp_server", "error", error)

				client.close()
			} else {
				this.log("udp_server", "info", `data sent to ${host}:${port}`)

				client.close()
			}
		})
	}

	onStep(buffer) {
		this.sendMessage(buffer, this.remoteHost, this.port)
		this.sendMessage(buffer, this.localArduino, this.port)
	}
}

module.exports = UDPServer