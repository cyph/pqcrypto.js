var Module	= {};

var isNode	= false;
try {
	isNode	= eval('global').process.release.name === 'node';
}
catch (_) {}


var pemJwk		= require('pem-jwk-norecompute');
var sodiumUtil	= require('sodiumutil');


var initiated, nodeCrypto, generateNodeKeyPair;
if (isNode) {
	initiated			= Promise.resolve();
	nodeCrypto			= eval('require')('crypto');
	generateNodeKeyPair	= eval('require')('util').promisify(nodeCrypto.generateKeyPair);
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
