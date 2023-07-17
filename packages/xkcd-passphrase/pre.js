(function () {


var isNode	=
	typeof global === 'object' &&
	typeof process === 'object' &&
	typeof require === 'function' &&
	typeof importScripts !== 'function'
;


var nodeCrypto;
if (isNode) {
	nodeCrypto	= eval('require')('crypto');
}


var xkcdPassphrase = (function () {
