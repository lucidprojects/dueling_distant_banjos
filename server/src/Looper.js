'use strict'

class Looper {
	constructor(log) {
		this.startTime
		this.log = log
		this.recordedData = {}
		this.player = {}
		this.state = {
			isPlaying: false,
			isRecording: false,
		}
	}

	record() {
		// log('http_server', 'info', 'listening on port: ' + config.port)
		this.state.isRecording = !this.state.isRecording

		if (this.state.isRecording) {
			this.startTime = Date.now()

			log('looper', 'info', 'playing loop')
		}
	}

	play() {
		this.state.isPlaying = true

		log('looper', 'info', 'playing loop')

		this.player = setInterval(() => {
			const buffer = this.getData()

			if (buffer != undefined) {
				const {
					sendMessage,
					remoteHost,
					localArduino,
					port
				} = this.udpServer

				sendMessage(buffer, remoteHost, port)
				sendMessage(buffer, localArduino, port)
			}
		})
	}

	stop() {
		this.state.isPlaying = false

		clearInterval(this.player)

		log('looper', 'info', 'stopping loop')
	}

	getData() {
		return this.recordedData[this.elapsedMillis()]
	}

	saveData(buffer) {
		this.recordedDuffer[this.elapsedMillis()] = buffer

		log('looper', 'info', 'stopping loop')
	}

	elapsedMillis() {
		return Date.now() - this.startTime
	}
}

module.exports = Looper