'use strict'

const elements = {
	edit: document.getElementById('edit'),
	submit: document.getElementById('submit'),
	localIp: document.getElementById('local-ip'),
	localPort: document.getElementById('local-port'),
	remoteIp: document.getElementById('remote-ip'),
	remotePort: document.getElementById('remote-port'),
	capBuff: document.getElementById('cap-buff'),
	capBuffInput: document.getElementById('capBuff'),
	inputs: document.getElementsByClassName('input-field')
}

// TODO: Remove this and get keys names from keys in state
const dataNames = [
	'localIp',
	'localPort',
	'remoteIp',
	'remotePort',
	'capBuff'
]

const state = {
	localIp: '',
	localPort: '',
	remoteIp: '',
	remotePort: '',
	capBuff: 0,
}

var edit = false

const getData = async () => {
	const response = await fetch("/api/data")

	return response.json()
}

const putData = async () => {
	let payload = {
		method: 'PUT',
		headers: {
			'Content-Type': 'application/json'
		},
		body: JSON.stringify(state)
	}

	const response = await fetch("/api/data", payload)

	return response.json()
}

const setState = (data) => {
	for (let i = 0; i < dataNames.length; i++) {
		state[dataNames[i]] = data[dataNames[i]]
	}
}

const setValues = () => {
	for (let i = 0; i < dataNames.length; i++) {
		elements[dataNames[i]].innerHTML = state[dataNames[i]]
	}
}

const handleEdit = () => {
	edit = !edit

	if (edit) {
		show()
	} else {
		hide()
	}
}

const hide = () => {
	elements.capBuffInput.style.visibility = "hidden";
	elements.submit.style.visibility = "hidden";
}

const show = () => {
	elements.capBuffInput.style.visibility = "visible";
	elements.submit.style.visibility = "visible";
}

const handleSubmit = async () => {
	let val

	for (let i = 0; i < dataNames.length; i++) {
		val = document.getElementById(dataNames[i]).value

		if (val != '') {
			state[dataNames[i]] = val
			document.getElementById(dataNames[i]).value = ""
		}
	}

	hide()

	let response = await putData()

	if (response.error) {
		console.log("Something went wrong trying to update data")
	} else {
		setValues()
	}
}

const init = async () => {
	// assign listeners
	elements.edit.onclick = handleEdit
	elements.submit.onclick = handleSubmit

	// get initial data
	let response = await getData()

	if (response.error) {
		console.log("Something went wrong trying to get initial data from Arduino")
	} else {
		setState(response)
		setValues()
	}
}

init()