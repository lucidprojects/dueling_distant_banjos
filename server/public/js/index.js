'use strict';

const elements = {
	edit: document.getElementById('edit'),
	submit: document.getElementById('submit'),
	localIp: document.getElementById('local-ip'),
	localPort: document.getElementById('local-port'),
	remoteIp: document.getElementById('remote-ip'),
	remotePort: document.getElementById('remote-port'),
	capBuff: document.getElementById('cap-buff'),
	inputs: document.getElementsByClassName('input-field')
};

// TODO: Remove this and get keys names from keys in state
const dataNames = [
	'localIp',
	'localPort',
	'remoteIp',
	'remotePort',
	'capBuff'
];

const state = {
	localIp: '',
	localPort: '',
	remoteIp: '',
	remotePort: '',
	capBuff: '',
};

var edit = false;

const getData = async () => {
	const response = await fetch("/api/data");

	return response.json();
}


const postData = async () => {
	let payload = {
		method: 'POST',
		headers: {
			'Content-Type': 'application/json'
		},
		body: JSON.stringify(state)
	}

	const response = await fetch("/api/data", payload);

	return response.json(); // parses JSON response into native JavaScript objects
}

const setState = (data) => {
	for (var i = 0; i < dataNames.length; i++) {
		state[dataNames[i]] = data[dataNames[i]];
	}
}

const setValues = () => {
	for (var i = 0; i < dataNames.length; i++) {
		if (dataNames[i].value != '') {
			elements[dataNames[i]].innerHTML = state[dataNames[i]];
		}
	}
}

const handleEdit = () => {
	edit = !edit;

	if (edit) {
		show();
		console.log("show");
	} else {
		hide();
		console.log("hide");
	}
}

const hide = () => {
	for (var i = 0; i < elements.inputs.length; i++) {
		elements.inputs[i].style.visibility = "hidden";
	}

	elements.submit.style.visibility = "hidden";
}

const show = () => {
	for (var i = 0; i < elements.inputs.length; i++) {
		elements.inputs[i].style.visibility = "visible";
	}

	elements.submit.style.visibility = "visible";
}

const handleSubmit = () => {
	for (var i = 0; i < dataNames.length; i++) {
		if (dataNames[i].value != '') {
			state[dataNames[i]] = document.getElementById(dataNames[i]).value;
			document.getElementById(dataNames[i]).value = ""
		}
	}

	setValues();

	// let carla = await postData();

	hide();
}

const init = async () => {
	// assign listeners
	elements.edit.onclick = handleEdit;
	elements.submit.onclick = handleSubmit;

	// get initial data
	let response = await getData();

	if (response.error) {
		console.log("Something went wrong trying to get initial data from Arduino");
	} else {
		setState(response);
		setValues();
	}
}

init();
