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
		this.state.isRecording = !this.state.isRecording

		if (this.state.isRecording) {
			this.startTime = Date.now()

			this.log('looper', 'info', 'start recording')
		} else {

			this.log('looper', 'info', 'start recording')
		}
	}

	play() {
		this.state.isPlaying = true

		this.log('looper', 'info', 'playing loop')

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

		this.log('looper', 'info', 'stopping loop')
	}

	getData() {
		return this.recordedData[this.elapsedMillis()]
	}

	saveData(buffer) {
		this.recordedData[this.elapsedMillis()] = buffer

		this.log('looper', 'info', `saving data: [${buffer[0]},${buffer[1]},${buffer[2]}]`)
	}

	elapsedMillis() {
		return `${Date.now() - this.startTime}`
	}
}

module.exports = Looper