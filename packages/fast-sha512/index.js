var isNode	= false;
try {
	isNode	= eval('global').process.release.name === 'node';
}
catch (_) {}


var naclSHA512	= require('./dist/nacl-sha512');
var sodiumUtil	= require('sodiumutil');


var nodeCrypto, Buffer;
if (isNode) {
	nodeCrypto	= eval('require')('crypto');
	Buffer		= eval('global.Buffer');
}


var fastSHA512	= {
	bytes: 64,

	baseHash: function (message, shouldClearMessage) {
		return Promise.resolve().then(function () {
			if (message === undefined) {
				message	= new Uint8Array(0);
			}
			if (!ArrayBuffer.isView(message)) {
				throw new Error('Cannot hash invalid input.');
			}

			if (isNode) {
				var hasher	= nodeCrypto.createHash('sha512');
				hasher.update(Buffer.from(message));

				return hasher.digest();
			}
			else {
				return crypto.subtle.digest(
					{
						name: 'SHA-512'
					},
					message
				);
			}
		}).then(function (hash) {
			return new Uint8Array(hash);
		}).catch(function () {
			return naclSHA512(message);
		}).then(function (hash) {
			if (shouldClearMessage) {
				sodiumUtil.memzero(message);
			}
			return hash;
		});
	},

	hash: function (message, onlyBinary) {
		return Promise.resolve().then(function () {
			return fastSHA512.baseHash(
				sodiumUtil.from_string(message),
				typeof message === 'string'
			);
		}).then(function (hash) {
			if (onlyBinary) {
				return hash;
			}
			else {
				return {binary: hash, hex: sodiumUtil.to_hex(hash)};
			}
		});
	}
};



fastSHA512.fastSHA512	= fastSHA512;
module.exports			= fastSHA512;
