(function () {


var isNode	= false;
try {
	isNode	= eval('global').process.release.name === 'node';
}
catch (_) {}


var nodeCrypto;
if (isNode) {
	nodeCrypto	= eval('require')('crypto');
}


var xkcdPassphrase = (function () {
