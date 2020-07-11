'use strict'

// WARN: Currently will not work

const udp = require('dgram')
const config = require('./config/config')

// creating a udp server
const server = udp.createSocket('udp4')

server.on('error', (error) => {
	console.log("udp_server", "error", error)
	server.close()
})

server.on('message', (msg, info) => {
	console.log("udp_server", "info", msg.toString() + ` | Received ${msg.length} bytes from ${info.address}:${info.port}`)
})

server.on('listening', () => {
	const address = server.address()
	const port = address.port
	const family = address.family
	const ipaddr = address.address

	console.log("udp_server", "info", 'Server is listening at port ' + port)
	console.log("udp_server", "info", 'Server ip :' + ipaddr)
	console.log("udp_server", "info", 'Server is IP4/IP6 : ' + family)
})

server.on('close', () => {
	console.log("udp_server", "info", 'Socket is closed !')
})

server.bind(config.testPort)

// creating a client socket
const client = udp.createSocket('udp4')

//buffer msg
const data = Buffer.from('MSG from UDP client')

//sending msg
client.send(data, config.udpPort, config.remoteHost, (error) => {
	if (error) {
		console.console.log(error)
		client.close()
	} else {
		console.console.log('Data sent !!!')
		client.close()
	}
})