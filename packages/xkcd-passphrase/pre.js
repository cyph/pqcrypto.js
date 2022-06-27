(function () {


var isNode	=
	typeof process === 'object' &&
	typeof require === 'function' &&
	typeof window !== 'object' &&
	typeof importScripts !== 'function'
;


var nodeCrypto;
if (isNode) {
	nodeCrypto	= require('crypto');
}


var xkcdPassphrase = (function () {
