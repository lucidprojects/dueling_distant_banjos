/*
   SPRAY_TO_PALY
   
   A water spout that is operated by a button
   press. The button does not initially work. 
   Also connected is the carnival game where a
   person uses a water gun to apply pressure to 
   a target. When a person plays the game and 
   hits the target at least every 2 seconds,
   the spout is enabled and can be used.  
   
   adapted from https: //github.com/rodrigoms2004/ServerSocketTCP_UDP/blob/master/udp_server.js
   created 02/15/2020
*/

const udp = require('dgram')
const conf = require('./config/config')

// const {
// 	log
// } = require('./util/loggerTool')

// --------------------creating a udp server --------------------

// creating a udp server
const server = udp.createSocket('udp4')

// emits when any error occurs
server.on('error', (error) => {
	console.log("udp_server", "error", error)
	// log("udp_server", "error", error)
	server.close()
})

// emits on new datagram msg
server.on('message', (msg, info) => {
	console.log("udp_server", "info", msg.toString() + ` | Received ${msg.length} bytes from ${info.address}:${info.port}`)
	// log("udp_server", "info", msg.toString() + ` | Received ${msg.length} bytes from ${info.address}:${info.port}`)

	let timestp = new Date()
	const response = {
		description: 'UDP PORT TEST BY RMS Math',
		serverPort: conf.port,
		timestamp: timestp.toJSON(),
		received: {
			message: msg.toString(),
			fromIP: info.address,
			fromPort: info.port
		}
	}
	const data = Buffer.from(JSON.stringify(response))

	//sending msg
	server.send(data, info.port, info.address, (error, bytes) => {
		if (error) {
			console.log("udp_server", "error", error)
			// log("udp_server", "error", error)
			client.close()
		} else {
			console.log("udp_server", "info", 'Data sent !!!')
			// log("udp_server", "info", 'Data sent !!!')
		}
	})
}) // end server.on


//emits when socket is ready and listening for datagram msgs
server.on('listening', () => {
	const address = server.address()
	const port = address.port
	const family = address.family
	const ipaddr = address.address

	console.log("udp_server", "info", 'Server is listening at port ' + port)
	// log("udp_server", "info", 'Server is listening at port ' + port)
	console.log("udp_server", "info", 'Server ip :' + ipaddr)
	// log("udp_server", "info", 'Server ip :' + ipaddr)
	console.log("udp_server", "info", 'Server ipv : ' + family)
	// log("udp_server", "info", 'Server is IP4/IP6 : ' + family)
})

//emits after the socket is closed using socket.close()
server.on('close', () => {
	console.log("udp_server", "info", 'Socket is closed !')
	// log("udp_server", "info", 'Socket is closed !')
})

server.bind(conf.port)