'use strict';

const localServer = window.location.hostname;

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
	localIp: '1',
	localPort: '2',
	remoteIp: '3',
	remotePort: '4',
	capBuff: '5',
};

var edit = false;

const getData = async () => {
	let host = localServer + "/api/data";

	const response = await fetch(host);

	// return response.json(); // parses JSON response into native JavaScript objects
	setState(response.json);
}

const setState = (data) => {
	console.log(data);

	// for (var i = 0; i < dataNames.length; i++) {
	// 	if (dataNames[i].value != '') {
	// 		state[dataNames[i]] = document.getElementById(dataNames[i]).value;
	// 		document.getElementById(dataNames[i]).value = ""
	// 	}
	// }
}

const postData = async () => {
	let host = localServer + "/api/data";

	let payload = {
		method: 'POST',
		headers: {
			'Content-Type': 'application/json'
		},
		body: JSON.stringify(state)
	}

	const response = await fetch(host, payload);

	return response.json(); // parses JSON response into native JavaScript objects
}

const setValues = () => {
	for (var i = 0; i < dataNames.length; i++) {
		elements[dataNames[i]].innerHTML = state[dataNames[i]];
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

// assign listeners
elements.edit.onclick = handleEdit;
elements.submit.onclick = handleSubmit;

// let carl = await getData();

setValues();