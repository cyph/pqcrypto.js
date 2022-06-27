}


[
	Float32Array,
	Float64Array,
	Int8Array,
	Int16Array,
	Int32Array,
	Uint8Array,
	Uint16Array,
	Uint32Array,
	Uint8ClampedArray
].forEach(function (TypedArray) {
	if (!TypedArray.prototype.indexOf) {
		TypedArray.prototype.indexOf	= function (n) {
			for (var i = 0 ; i < this.length ; ++i) {
				if (this[i] === n) {
					return i;
				}
			}

			return -1;
		};
	}
});

function dataResult (buffer, bytes) {
	return new Uint8Array(
		new Uint8Array(Module.HEAPU8.buffer, buffer, bytes)
	);
}

function dataFree (buffer) {
	try {
		Module._free(buffer);
	}
	catch (err) {
		setTimeout(function () { throw err; }, 0);
	}
}

function importJWK (key, purpose) {
	return Promise.resolve().then(function () {
		var zeroIndex	= key.indexOf(0);
		var jwk			= JSON.parse(
			sodiumUtil.to_string(
				zeroIndex > -1 ?
					new Uint8Array(key.buffer, key.byteOffset, zeroIndex) :
					key
			)
		);

		return Promise.resolve().then(function () {
			return crypto.subtle.importKey(
				'jwk',
				jwk,
				algorithm,
				false,
				[purpose]
			);
		}).catch(function () {
			return pemJwk.jwk2pem(jwk);
		});
	});
}
	
function exportJWK (key, bytes) {
	return Promise.resolve().then(function () {
		if (typeof key === 'string') {
			return pemJwk.pem2jwk(key);
		}
		else {
			return crypto.subtle.exportKey(
				'jwk',
				key,
				algorithm.name
			);
		}
	}).then(function (jwk) {
		var a	= sodiumUtil.from_string(JSON.stringify(jwk));
		var b	= new Uint8Array(bytes);
		b.set(a);
		sodiumUtil.memzero(a);
		return b;
	});
}

function exportKeyPair (keyPair) {
	return Promise.all([
		exportJWK(keyPair.publicKey, rsaSign.publicKeyBytes),
		exportJWK(keyPair.privateKey, rsaSign.privateKeyBytes)
	]).then(function (results) {
		return {
			publicKey: results[0],
			privateKey: results[1]
		};
	});
}


var algorithm	= isNode ?
	'RSA-SHA256' :
	{
		name: 'RSASSA-PKCS1-v1_5',
		hash: {
			name: 'SHA-256'
		},
		modulusLength: 2048,
		publicExponent: new Uint8Array([0x01, 0x00, 0x01])
	}
;


var rsaSign	= {
	publicKeyBytes: RSASIGNJS_PUBLEN,
	privateKeyBytes: RSASIGNJS_PRIVLEN,
	bytes: RSASIGNJS_SIGLEN,

	keyPair: function () {
		return Promise.resolve().then(function () {
			if (isNode) {
				var keyPair	= generateRSAKeypair();

				return {
					publicKey: keyPair.public,
					privateKey: keyPair.private
				};
			}
			else {
				return crypto.subtle.generateKey(
					algorithm,
					true,
					['sign', 'verify']
				);
			}
		}).then(
			exportKeyPair
		).catch(function () { return initiated.then(function () {
			var publicKeyBuffer		= Module._malloc(rsaSign.publicKeyBytes);
			var privateKeyBuffer	= Module._malloc(rsaSign.privateKeyBytes);

			try {
				var returnValue	= Module._rsasignjs_keypair(
					publicKeyBuffer,
					privateKeyBuffer
				);

				if (returnValue !== 0) {
					throw new Error('RSA Sign error: keyPair failed (' + returnValue + ')');
				}

				return exportKeyPair({
					publicKey:
						'-----BEGIN PUBLIC KEY-----\n' +
						sodiumUtil.to_base64(dataResult(publicKeyBuffer, rsaSign.publicKeyBytes)) +
						'\n-----END PUBLIC KEY-----'
					,
					privateKey:
						'-----BEGIN RSA PRIVATE KEY-----\n' +
						sodiumUtil.to_base64(dataResult(privateKeyBuffer, rsaSign.privateKeyBytes)) +
						'\n-----END RSA PRIVATE KEY-----'
				});
			}
			finally {
				dataFree(publicKeyBuffer, rsaSign.publicKeyBytes);
				dataFree(privateKeyBuffer, rsaSign.privateKeyBytes);
			}
		}); });
	},

	sign: function (message, privateKey) {
		return rsaSign.signDetached(message, privateKey).then(function (signature) {
			var signed	= new Uint8Array(rsaSign.bytes + message.length);
			signed.set(signature);
			signed.set(message, rsaSign.bytes);
			return signed;
		});
	},

	signDetached: function (message, privateKey) {
		return importJWK(privateKey, 'sign').then(function (sk) {
			return Promise.resolve().then(function () {
				if (isNode) {
					var messageBuffer	= Buffer.from(message);
					var signer			= nodeCrypto.createSign(algorithm);
					signer.write(messageBuffer);
					signer.end();

					var signature	= signer.sign(sk);
					sodiumUtil.memzero(messageBuffer);
					return signature;
				}
				else {
					return crypto.subtle.sign(algorithm, sk, message);
				}
			}).catch(function () { return initiated.then(function () {
				sk	= sodiumUtil.from_base64(sk.split('-----')[2]);

				var signatureBuffer		= Module._malloc(rsaSign.bytes);
				var messageBuffer		= Module._malloc(message.length);
				var privateKeyBuffer	= Module._malloc(sk.length);

				Module.writeArrayToMemory(message, messageBuffer);
				Module.writeArrayToMemory(sk, privateKeyBuffer);

				try {
					var returnValue	= Module._rsasignjs_sign(
						signatureBuffer,
						messageBuffer,
						message.length,
						privateKeyBuffer,
						sk.length
					);

					if (returnValue !== 1) {
						throw new Error('RSA Sign error: sign failed (' + returnValue + ')');
					}

					return dataResult(signatureBuffer, rsaSign.bytes);
				}
				finally {
					dataFree(signatureBuffer);
					dataFree(messageBuffer);
					dataFree(privateKeyBuffer);
				}
			}); }).then(function (signature) {
				sodiumUtil.memzero(sk);
				return new Uint8Array(signature);
			});
		});
	},

	open: function (signed, publicKey) {
		return Promise.resolve().then(function () {
			var signature	= new Uint8Array(signed.buffer, signed.byteOffset, rsaSign.bytes);
			var message		= new Uint8Array(signed.buffer, signed.byteOffset + rsaSign.bytes);

			return rsaSign.verifyDetached(signature, message, publicKey).then(function (isValid) {
				if (isValid) {
					return message;
				}
				else {
					throw new Error('Failed to open RSA signed message.');
				}
			});
		});
	},

	verifyDetached: function (signature, message, publicKey) {
		return importJWK(publicKey, 'verify').then(function (pk) {
			return Promise.resolve().then(function () {
				if (isNode) {
					var verifier	= nodeCrypto.createVerify(algorithm);
					verifier.update(Buffer.from(message));
					return verifier.verify(pk, signature);
				}
				else {
					return crypto.subtle.verify(algorithm, pk, signature, message);
				}
			}).catch(function () { return initiated.then(function () {
				pk	= sodiumUtil.from_base64(pk.split('-----')[2]);

				var signatureBuffer	= Module._malloc(rsaSign.bytes);
				var messageBuffer	= Module._malloc(message.length);
				var publicKeyBuffer	= Module._malloc(pk.length);

				Module.writeArrayToMemory(signature, signatureBuffer);
				Module.writeArrayToMemory(message, messageBuffer);
				Module.writeArrayToMemory(pk, publicKeyBuffer);

				try {
					var returnValue	= Module._rsasignjs_verify(
						signatureBuffer,
						messageBuffer,
						message.length,
						publicKeyBuffer,
						pk.length
					);

					return returnValue === 1;
				}
				finally {
					dataFree(signatureBuffer);
					dataFree(messageBuffer);
					dataFree(publicKeyBuffer);
				}
			}); }).then(function (isValid) {
				sodiumUtil.memzero(pk);
				return isValid;
			});
		});
	}
};



rsaSign.rsaSign	= rsaSign;
module.exports	= rsaSign;
