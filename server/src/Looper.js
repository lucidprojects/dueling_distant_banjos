/*
   adapted from https://github.com/bartjakobs/node-looper/blob/master/lib/looper.js
   created 07/16/2020
*/
'use strict'

class Looper {
	constructor(log) {
		this.sequence = []
		this.length = false
		this.looping = false
		this.muted = false
		this.started = false
		this.recording = false
		this.log = log
	}

	// start looping
	start() {
		this.started = true
		this.startTime = new Date
		this.lastLoopStartTime = new Date
		this.lastStepTime = new Date
		this.lastPlayedIndex = 0
		// this.intervalId = setInterval(this.step.bind(this), 1)
		this.intervalId = setInterval(this.step.bind(this), 2)
		this.log('looper', 'info', 'playing loop')
	}

	// reset loop and state
	reset() {
		if (typeof this.muteCb == 'function') this.muteCb()

		this.sequence = []
		this.length = false
		this.looping = false
		this.muted = false
		this.started = false
		this.recording = false

		clearInterval(this.intervalId)

		this.log('looper', 'info', 'loop reset')
	}

	// TODO: how to use (does it need to log?)
	// starts looping???
	loop() {
		if (this.looping) return
		this.looping = true
		this.length = new Date - this.startTime
	}


	// toggles recording (`Looper.add` calls)
	toggleRecording() {
		this.recording = !this.recording

		if (this.recording) this.log('looper', 'info', 'start recording')
		else this.log('looper', 'info', 'stop recording')

		return this.recording
	}

	// add callback function that should be called when an  event should be repeated every loop step
	onStep(cb) {
		this.stepCb = cb
	}

	// add callback function that should be called when muted
	onMute(cb) {
		this.muteCb = cb
	}

	// mute looper
	mute() {
		this.muted = !this.muted

		if (!this.muted) {
			this.lastLoopStartTime = new Date
			this.lastPlayedIndex = 0 // TODO: does this mean muting restarts 
		}

		if (typeof this.muteCb == 'function') this.muteCb()

		return this.muted
	}

	// add data to the loop sequence
	add(buffer) {
		if (!this.recording) return
		if (this.muted) return
		if (!this.started) return

		this.log('looper', 'info', `saving data: [${buffer[0]},${buffer[1]},${buffer[2]}]`)

		var obj = {
			time: new Date - this.lastLoopStartTime,
			data: buffer,
			isNew: true
		}

		this.sequence.push(obj)
		this.sequence.sort(this.compare)
	}

	// step event, called by a setInterval.
	step() {
		let restarted = false
		let currentTime = new Date - this.lastLoopStartTime // current time in this loop.

		// If this is the end of a loop.
		if (this.length && currentTime > this.length) {
			this.lastLoopStartTime = new Date(this.lastLoopStartTime.getTime() + this.length)
			restarted = true
			currentTime = new Date - this.lastLoopStartTime
		}

		if (typeof this.stepCb != 'function') {
			this.log('looper', 'error', 'no callback set for each loop step')
			return
		}

		// flush events at the end
		if (restarted) {
			while (this.lastPlayedIndex < this.sequence.length) {
				if (!this.muted) this.stepCb(this.sequence[this.lastPlayedIndex++].data)
				else break
			}

			this.lastPlayedIndex = 0
		}

		while ((this.sequence.length > this.lastPlayedIndex) && this.sequence[this.lastPlayedIndex].time < currentTime) {
			let obj = this.sequence[this.lastPlayedIndex]
			this.lastPlayedIndex++

			// don't repeat, just new events
			if (obj.isNew === true) {
				delete obj.isNew
				continue
			}

			if (!this.muted) this.stepCb(obj.data)
		}

	}

	// sort events in the sequence
	compare(a, b) {
		if (a.time < b.time) {
			return -1
		}
		if (a.time > b.time) {
			return 1
		}
		return 0
	}

	// play() {
	// 	this.state.isPlaying = true

	// 	this.log('looper', 'info', 'playing loop')

	// 	console.log(this.recordedData)

	// 	this.player = setInterval(() => {
	// 		const buffer = this.getData()

	// 		if (buffer) {
	// 			this.log('looper', 'info', 'it was found')

	// 			this.udpServer.sendMessage(buffer, remoteHost, port)
	// 			this.udpServer.sendMessage(buffer, localArduino, port)
	// 		}
	// 	}, 1)
	// }
}

module.exports = Looper