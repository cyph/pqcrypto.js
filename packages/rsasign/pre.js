var Module	= {};

var isNode	=
	typeof process === 'object' &&
	typeof require === 'function' &&
	typeof window !== 'object' &&
	typeof importScripts !== 'function'
;


var pemJwk		= require('pem-jwk-norecompute');
var sodiumUtil	= require('sodiumutil');


var initiated, nodeCrypto, generateRSAKeypair;
if (isNode) {
	initiated			= Promise.resolve();
	nodeCrypto			= require('crypto');
	generateRSAKeypair	= require('generate-rsa-keypair');
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
