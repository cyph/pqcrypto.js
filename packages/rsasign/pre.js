var Module	= {};

var isNode	=
	typeof global === 'object' &&
	typeof process === 'object' &&
	typeof require === 'function' &&
	typeof importScripts !== 'function'
;


var pemJwk		= require('pem-jwk-norecompute');
var sodiumUtil	= require('sodiumutil');


var initiated, nodeCrypto, generateNodeKeyPair;
if (isNode) {
	initiated			= Promise.resolve();
	nodeCrypto			= require('crypto');
	generateNodeKeyPair	= require('util').promisify(nodeCrypto.generateKeyPair);
}
else {
	initiated	= new Promise(function (resolve, reject) {
		Module.onAbort = reject;
		Module.onRuntimeInitialized = function () {
			try {
				Module._rsasignjs_init();
				resolve();
			}
			catch (err) {
				reject(err);
			}
		};
	});
