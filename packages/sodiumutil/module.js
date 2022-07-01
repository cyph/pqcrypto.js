var sodium	= require('libsodium-wrappers-sumo');

function from_base64 (data) {
	return sodium.from_base64(data, sodium.base64_variants.ORIGINAL);
}

function from_hex (data) {
	return sodium.from_hex(data);
}

function from_string (data) {
	return sodium.from_string(data);
}

function memcmp (a, b) {
	return sodium.memcmp(a, b);
}

function memzero (data) {
	return sodium.memzero(data);
}

function to_base64 (data) {
	return sodium.to_base64(data, sodium.base64_variants.ORIGINAL);
}

function to_hex (data) {
	return sodium.to_hex(data);
}

function to_string (data) {
	return sodium.to_string(data);
}
