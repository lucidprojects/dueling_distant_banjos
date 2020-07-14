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
		this.recording = false
		this.hasData = false
		this.log = log
	}

	// starts looping
	start() {
		if (this.looping) return

		this.looping = true
		this.lastLoopStartTime = new Date
		this.lastPlayedIndex = 0
		this.intervalId = setInterval(this.step.bind(this), 2)

		this.log('looper', 'info', 'playing loop')
	}

	// reset loop and state
	reset() {
		if (typeof this.muteCb == 'function') this.muteCb()

		// this.sequence = []
		// this.hasData = false
		// this.length = false
		this.looping = false
		this.muted = false
		this.recording = false

		clearInterval(this.intervalId)

		this.log('looper', 'info', 'loop reset')
	}

	// toggles recording (enables `Looper.add` calls)
	toggleRecording() {
		if (this.looping) return

		this.recording = !this.recording

		if (this.recording) {
			this.startTime = new Date

			if (!this.hasData) this.hasData = true

			this.log('looper', 'info', 'start recording')

		} else {
			this.length = new Date - this.startTime

			this.log('looper', 'info', 'stop recording')
		}

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
		if (this.recording) return

		this.muted = !this.muted

		if (this.looping && this.muted) this.log('looper', 'info', 'muted')
		else this.log('looper', 'info', 'unmuted')

		if (!this.muted) {
			this.lastLoopStartTime = new Date
			this.lastPlayedIndex = 0
		}

		if (typeof this.muteCb == 'function') this.muteCb()
	}

	// add data to the loop sequence
	add(buffer) {
		if (!this.recording) return
		if (this.muted) return
		if (!this.hasData) return

		this.log('looper', 'info', `saving data: [${buffer[0]}, ${buffer[1]}, ${buffer[2]}]`)

		var obj = {
			time: new Date - this.lastLoopStartTime,
			data: buffer,
			isNew: true
		}

		this.sequence.push(obj)
		this.sequence.sort(this.compare)
	}

	// step event, called by a setInterval
	step() {
		let restarted = false
		let currentTime = new Date - this.lastLoopStartTime

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
		if (a.time < b.time) return -1
		if (a.time > b.time) return 1
		return 0
	}
}

module.exports = Looper