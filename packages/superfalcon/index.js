var isNode	=
	typeof process === 'object' &&
	typeof require === 'function' &&
	typeof window !== 'object' &&
	typeof importScripts !== 'function'
;


var sha512		= require('./nacl-sha512');
var eccSign		= require('eccsign');
var sodiumUtil	= require('sodiumutil');
var falcon		= require('falcon');


var nodeCrypto, Buffer;
if (isNode) {
	nodeCrypto	= eval('require')('crypto');
	Buffer		= eval('global.Buffer');
}


var hashBytes	= 64;


function hashInternal (message, shouldClearMessage) {
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
		return sha512(message);
	}).then(function (hash) {
		if (shouldClearMessage) {
			sodiumUtil.memzero(message);
		}
		return hash;
	});
}

function hashWithAdditionalData (message, additionalData, preHashed) {
	var shouldClearAdditionalData	= typeof additionalData === 'string';
	var shouldClearMessage			= typeof message === 'string';

	return Promise.resolve().then(function () {
		message	= sodiumUtil.from_string(message);

		if (preHashed && message.length !== hashBytes) {
			throw new Error('Invalid pre-hashed message.');
		}

		return Promise.all([
			hashInternal(
				additionalData ? sodiumUtil.from_string(additionalData) : new Uint8Array(0),
				shouldClearAdditionalData
			),
			preHashed ? message : hashInternal(message, shouldClearMessage)
		]);
	}).then(function (results) {
		var additionalDataHash	= results[0];
		var messageToHash		= results[1];

		var fullMessage	= new Uint8Array(additionalDataHash.length + hashBytes);
		fullMessage.set(additionalDataHash);
		fullMessage.set(messageToHash, additionalDataHash.length);
		sodiumUtil.memzero(additionalDataHash);

		if (shouldClearMessage || !preHashed) {
			sodiumUtil.memzero(messageToHash);
		}

		return hashInternal(fullMessage, true);
	});
}

function deriveEncryptionKey (password, salt) {
	if (isNode) {
		return new Promise(function (resolve, reject) {
			nodeCrypto.pbkdf2(
				Buffer.from(password),
				Buffer.from(salt),
				aes.keyDerivation.iterations,
				aes.keyBytes,
				aes.keyDerivation.hashFunction,
				function (err, key) {
					if (err) {
						reject(err);
					}
					else {
						resolve(key);
					}
				}
			);
		});
	}
	else {
		return Promise.resolve().then(function () {
			return crypto.subtle.importKey(
				'raw',
				sodiumUtil.from_string(password),
				{
					name: aes.keyDerivation.algorithm,
				},
				false,
				['deriveKey']
			);
		}).then(function (keyOrigin) {
			return crypto.subtle.deriveKey(
				{
					name: aes.keyDerivation.algorithm,
					salt: salt,
					iterations: aes.keyDerivation.iterations,
					hash: {
						name: aes.keyDerivation.hashFunction
					},
				},
				keyOrigin,
				{
					name: aes.algorithm,
					length: aes.keyBits
				},
				false,
				['encrypt', 'decrypt']
			);
		});
	}
}

function encrypt (plaintext, password) {
	var setup	= Promise.resolve().then(function () {
		var iv		= isNode ?
			nodeCrypto.randomBytes(aes.ivBytes) :
			crypto.getRandomValues(new Uint8Array(aes.ivBytes))
		;

		var salt	= isNode ?
			nodeCrypto.randomBytes(aes.keyDerivation.saltBytes) :
			crypto.getRandomValues(new Uint8Array(aes.keyDerivation.saltBytes))
		;

		return Promise.all([iv, salt, deriveEncryptionKey(password, salt)]);
	}).then(function (results) {
		return {
			iv: results[0],
			salt: results[1],
			key: results[2]
		};
	});

	if (isNode) {
		return setup.then(function (o) {
			var cipher	= nodeCrypto.createCipheriv(aes.algorithm, o.key, o.iv);
			var buf1	= cipher.update(Buffer.from(plaintext));
			var buf2	= cipher.final();
			var buf3	= cipher.getAuthTag();

			var cyphertext	= new Uint8Array(Buffer.concat([o.iv, o.salt, buf1, buf2, buf3]));

			sodiumUtil.memzero(o.iv);
			sodiumUtil.memzero(o.salt);
			sodiumUtil.memzero(o.key);
			sodiumUtil.memzero(buf1);
			sodiumUtil.memzero(buf2);
			sodiumUtil.memzero(buf3);

			return cyphertext;
		});
	}
	else {
		return setup.then(function (o) {
			return Promise.all([o, crypto.subtle.encrypt(
				{
					name: aes.algorithm,
					iv: o.iv,
					tagLength: aes.tagBits
				},
				o.key,
				plaintext
			)]);
		}).then(function (results) {
			var o			= results[0];
			var encrypted	= new Uint8Array(results[1]);

			var cyphertext	= new Uint8Array(
				aes.ivBytes + aes.keyDerivation.saltBytes + encrypted.length
			);

			cyphertext.set(o.iv);
			cyphertext.set(o.salt, aes.ivBytes);
			cyphertext.set(encrypted, aes.ivBytes + aes.keyDerivation.saltBytes);

			sodiumUtil.memzero(o.iv);
			sodiumUtil.memzero(o.salt);
			sodiumUtil.memzero(o.key);
			sodiumUtil.memzero(encrypted);

			return cyphertext;
		});
	}
}

function decrypt (cyphertext, password) {
	return Promise.resolve().then(function () {
		var iv		= new Uint8Array(cyphertext.buffer, cyphertext.byteOffset, aes.ivBytes);

		var salt	= new Uint8Array(
			cyphertext.buffer,
			cyphertext.byteOffset + aes.ivBytes,
			aes.keyDerivation.saltBytes
		);

		return Promise.all([iv, deriveEncryptionKey(password, salt)]);
	}).then(function (results) {
		var iv	= results[0];
		var key	= results[1];

		var decrypted;

		if (isNode) {
			var encrypted	= new Uint8Array(
				cyphertext.buffer,
				cyphertext.byteOffset + aes.ivBytes + aes.keyDerivation.saltBytes,
				cyphertext.length -
					aes.ivBytes -
					aes.keyDerivation.saltBytes -
					aes.tagBytes
			);

			var authTag		= new Uint8Array(
				cyphertext.buffer,
				cyphertext.byteOffset + cyphertext.length - aes.tagBytes
			);

			var decipher	= nodeCrypto.createDecipheriv(
				aes.algorithm,
				Buffer.from(key),
				Buffer.from(iv)
			);

			decipher.setAuthTag(Buffer.from(authTag));

			var buf1	= decipher.update(Buffer.from(encrypted));
			var buf2	= decipher.final();

			decrypted	= Buffer.concat([buf1, buf2]);

			sodiumUtil.memzero(buf1);
			sodiumUtil.memzero(buf2);
		}
		else {
			var encrypted	= new Uint8Array(
				cyphertext.buffer,
				cyphertext.byteOffset + aes.ivBytes + aes.keyDerivation.saltBytes
			);

			decrypted	= crypto.subtle.decrypt(
				{
					name: aes.algorithm,
					iv: iv,
					tagLength: aes.tagBits
				},
				key,
				encrypted
			);
		}

		return Promise.all([key, decrypted]);
	}).then(function (results) {
		var key			= results[0];
		var decrypted	= results[1];

		sodiumUtil.memzero(key);

		return new Uint8Array(decrypted);
	});
}


var aes	= {
	algorithm: isNode ? 'aes-256-gcm' : 'AES-GCM',
	ivBytes: 12,
	keyBytes: 32,
	keyBits: 256,
	tagBytes: 16,
	tagBits: 128,

	keyDerivation: {
		algorithm: 'PBKDF2',
		hashFunction: isNode ? 'sha512' : 'SHA-512',
		iterations: 1000000,
		saltBytes: 32
	}
};


var publicKeyBytes, privateKeyBytes, bytes, falconBytes;

var initiated	= Promise.all([
	falcon.publicKeyBytes,
	falcon.privateKeyBytes,
	falcon.bytes
]).then(function (results) {
	falconBytes	= {
		publicKeyBytes: results[0],
		privateKeyBytes: results[1],
		bytes: results[2]
	};

	publicKeyBytes	= eccSign.publicKeyBytes + falconBytes.publicKeyBytes;
	privateKeyBytes	= eccSign.privateKeyBytes + falconBytes.privateKeyBytes;
	bytes			= eccSign.bytes + falconBytes.bytes;
});


var superFalcon	= {
	_sodiumUtil: sodiumUtil,
	publicKeyBytes: initiated.then(function () { return publicKeyBytes; }),
	privateKeyBytes: initiated.then(function () { return privateKeyBytes; }),
	bytes: initiated.then(function () { return bytes; }),
	hashBytes: Promise.resolve(hashBytes),

	hash: function (message, onlyBinary) {
		return Promise.resolve().then(function () {
			return hashInternal(sodiumUtil.from_string(message), typeof message === 'string');
		}).then(function (hash) {
			if (onlyBinary) {
				return hash;
			}
			else {
				return {binary: hash, hex: sodiumUtil.to_hex(hash)};
			}
		});
	},

	keyPair: function () { return initiated.then(function () {
		return Promise.all([
			eccSign.keyPair(),
			falcon.keyPair()
		]).then(function (results) {
			var eccKeyPair		= results[0];
			var falconKeyPair	= results[1];

			var keyPair	= {
				keyType: 'superfalcon',
				publicKey: new Uint8Array(publicKeyBytes),
				privateKey: new Uint8Array(privateKeyBytes)
			};

			keyPair.publicKey.set(eccKeyPair.publicKey);
			keyPair.privateKey.set(eccKeyPair.privateKey);
			keyPair.publicKey.set(falconKeyPair.publicKey, eccSign.publicKeyBytes);
			keyPair.privateKey.set(falconKeyPair.privateKey, eccSign.privateKeyBytes);

			sodiumUtil.memzero(falconKeyPair.privateKey);
			sodiumUtil.memzero(eccKeyPair.privateKey);
			sodiumUtil.memzero(falconKeyPair.publicKey);
			sodiumUtil.memzero(eccKeyPair.publicKey);

			return keyPair;
		});
	}); },

	sign: function (message, privateKey, additionalData) { return initiated.then(function () {
		var shouldClearMessage	= typeof message === 'string';

		return superFalcon.signDetached(
			message,
			privateKey,
			additionalData
		).then(function (signature) {
			message		= sodiumUtil.from_string(message);

			var signed	= new Uint8Array(
				bytes + message.length
			);

			signed.set(signature);
			signed.set(message, bytes);

			if (shouldClearMessage) {
				sodiumUtil.memzero(message);
			}

			sodiumUtil.memzero(signature);

			return signed;
		}).catch(function (err) {
			if (shouldClearMessage) {
				sodiumUtil.memzero(message);
			}

			throw err;
		});
	}); },

	signBase64: function (message, privateKey, additionalData) { return initiated.then(function () {
		return superFalcon.sign(message, privateKey, additionalData).then(function (signed) {
			var s	= sodiumUtil.to_base64(signed);
			sodiumUtil.memzero(signed);
			return s;
		});
	}); },

	signDetached: function (
		message,
		privateKey,
		additionalData,
		preHashed
	) { return initiated.then(function () {
		return hashWithAdditionalData(message, additionalData, preHashed).then(function (hash) {
			return Promise.all([
				hash,
				eccSign.signDetached(
					hash,
					new Uint8Array(
						privateKey.buffer,
						privateKey.byteOffset,
						eccSign.privateKeyBytes
					)
				),
				falcon.signDetached(
					hash,
					new Uint8Array(
						privateKey.buffer,
						privateKey.byteOffset + eccSign.privateKeyBytes
					)
				)
			]);
		}).then(function (results) {
			var hash				= results[0];
			var eccSignature		= results[1];
			var falconSignature	= results[2];

			var signature	= new Uint8Array(bytes);

			signature.set(eccSignature);
			signature.set(falconSignature, eccSign.bytes);

			sodiumUtil.memzero(hash);
			sodiumUtil.memzero(falconSignature);
			sodiumUtil.memzero(eccSignature);

			return signature;
		});
	}); },

	signDetachedBase64: function (
		message,
		privateKey,
		additionalData,
		preHashed
	) { return initiated.then(function () {
			return superFalcon.signDetached(
				message,
				privateKey,
				additionalData,
				preHashed
			).then(function (signature) {
				var s	= sodiumUtil.to_base64(signature);
				sodiumUtil.memzero(signature);
				return s;
			});
		});
	},

	open: function (
		signed,
		publicKey,
		additionalData,
		knownGoodHash,
		includeHash
	) { return initiated.then(function () {
		var shouldClearSigned	= typeof signed === 'string';

		return Promise.resolve().then(function () {
			signed	= sodiumUtil.from_base64(signed);

			var signature	= new Uint8Array(
				signed.buffer,
				signed.byteOffset,
				bytes
			);

			var message		= new Uint8Array(
				signed.buffer,
				signed.byteOffset + bytes
			);

			return Promise.all([message, superFalcon.verifyDetached(
				signature,
				message,
				publicKey,
				additionalData,
				knownGoodHash,
				includeHash
			)]);
		}).then(function (results) {
			var message	= new Uint8Array(results[0]);
			var hash	= includeHash ? results[1].hash : undefined;
			var isValid	= includeHash ? results[1].valid : results[1];

			if (shouldClearSigned) {
				sodiumUtil.memzero(signed);
			}

			if (isValid) {
				return includeHash ? {hash: hash, message: message} : message;
			}
			else {
				throw new Error('Failed to open SuperFALCON signed message.');
			}
		}).catch(function (err) {
			if (shouldClearSigned) {
				sodiumUtil.memzero(signed);
			}

			throw err;
		});
	}); },

	openString: function (
		signed,
		publicKey,
		additionalData,
		knownGoodHash,
		includeHash
	) { return initiated.then(function () {
		return superFalcon.open(
			signed,
			publicKey,
			additionalData,
			knownGoodHash,
			includeHash
		).then(function (message) {
			var hash	= undefined;

			if (includeHash) {
				hash	= sodiumUtil.to_hex(message.hash);
				sodiumUtil.memzero(message.hash);
				message	= message.message;
			}

			var s	= sodiumUtil.to_string(message);
			sodiumUtil.memzero(message);

			return includeHash ? {hash: hash, message: s} : s;
		});
	}); },

	verifyDetached: function (
		signature,
		message,
		publicKey,
		additionalData,
		knownGoodHash,
		includeHash
	) { return initiated.then(function () {
		var shouldClearSignature	= typeof signature === 'string';

		return hashWithAdditionalData(message, additionalData).then(function (hash) {
			signature	= sodiumUtil.from_base64(signature);

			var shouldClearKnownGoodHash	= false;
			if (typeof knownGoodHash === 'string' && knownGoodHash.length > 0) {
				knownGoodHash				= sodiumUtil.from_hex(knownGoodHash);
				shouldClearKnownGoodHash	= true;
			}
			var hashAlreadyVerified	=
				knownGoodHash instanceof Uint8Array &&
				knownGoodHash.length > 0 &&
				sodiumUtil.memcmp(hash, knownGoodHash)
			;
			if (shouldClearKnownGoodHash) {
				sodiumUtil.memzero(knownGoodHash);
			}

			var publicKeyPromise	=
				hashAlreadyVerified ?
					undefined :
				publicKey instanceof Uint8Array ?
					Promise.resolve(publicKey) :
					superFalcon.importKeys(publicKey).then(function (kp) {
						return kp.publicKey;
					})
			;

			return Promise.all([
				hash,
				hashAlreadyVerified || publicKeyPromise.then(function (pk) {
					return eccSign.verifyDetached(
						new Uint8Array(signature.buffer, signature.byteOffset, eccSign.bytes),
						hash,
						new Uint8Array(pk.buffer, pk.byteOffset, eccSign.publicKeyBytes)
					);
				}),
				hashAlreadyVerified || publicKeyPromise.then(function (pk) {
					return falcon.verifyDetached(
						new Uint8Array(
							signature.buffer,
							signature.byteOffset + eccSign.bytes,
							falconBytes.bytes
						),
						hash,
						new Uint8Array(pk.buffer, pk.byteOffset + eccSign.publicKeyBytes)
					);
				})
			]);
		}).then(function (results) {
			var hash			= results[0];
			var eccIsValid		= results[1];
			var falconIsValid	= results[2];
			var valid			= eccIsValid && falconIsValid;

			if (shouldClearSignature) {
				sodiumUtil.memzero(signature);
			}

			if (includeHash) {
				return {hash: hash, valid: valid};
			}

			sodiumUtil.memzero(hash);

			return valid;
		}).catch(function (err) {
			if (shouldClearSignature) {
				sodiumUtil.memzero(signature);
			}

			throw err;
		});;
	}); },

	exportKeys: function (keyPair, password) {
		return initiated.then(function () {
			if (!keyPair.privateKey) {
				return null;
			}

			var eccPrivateKey			= new Uint8Array(
				eccSign.publicKeyBytes +
				eccSign.privateKeyBytes
			);

			var falconPrivateKey		= new Uint8Array(
				falconBytes.publicKeyBytes +
				falconBytes.privateKeyBytes
			);

			var superFalconPrivateKey	= new Uint8Array(
				publicKeyBytes +
				privateKeyBytes
			);

			eccPrivateKey.set(new Uint8Array(
				keyPair.publicKey.buffer,
				keyPair.publicKey.byteOffset,
				eccSign.publicKeyBytes
			));
			eccPrivateKey.set(
				new Uint8Array(
					keyPair.privateKey.buffer,
					keyPair.privateKey.byteOffset,
					eccSign.privateKeyBytes
				),
				eccSign.publicKeyBytes
			);

			falconPrivateKey.set(new Uint8Array(
				keyPair.publicKey.buffer,
				keyPair.publicKey.byteOffset + eccSign.publicKeyBytes
			));
			falconPrivateKey.set(
				new Uint8Array(
					keyPair.privateKey.buffer,
					keyPair.privateKey.byteOffset + eccSign.privateKeyBytes
				),
				falconBytes.publicKeyBytes
			);

			superFalconPrivateKey.set(keyPair.publicKey);
			superFalconPrivateKey.set(keyPair.privateKey, publicKeyBytes);

			if (password) {
				return Promise.all([
					encrypt(eccPrivateKey, password),
					encrypt(falconPrivateKey, password),
					encrypt(superFalconPrivateKey, password)
				]).then(function (results) {
					sodiumUtil.memzero(superFalconPrivateKey);
					sodiumUtil.memzero(falconPrivateKey);
					sodiumUtil.memzero(eccPrivateKey);

					return results;
				});
			}
			else {
				return [
					eccPrivateKey,
					falconPrivateKey,
					superFalconPrivateKey
				];
			}
		}).then(function (results) {
			if (!results) {
				return {
					ecc: null,
					falcon: null,
					superFalcon: null
				};
			}

			var eccPrivateKey			= results[0];
			var falconPrivateKey		= results[1];
			var superFalconPrivateKey	= results[2];

			var privateKeyData	= {
				ecc: sodiumUtil.to_base64(eccPrivateKey),
				falcon: sodiumUtil.to_base64(falconPrivateKey),
				superFalcon: sodiumUtil.to_base64(superFalconPrivateKey)
			};

			sodiumUtil.memzero(superFalconPrivateKey);
			sodiumUtil.memzero(falconPrivateKey);
			sodiumUtil.memzero(eccPrivateKey);

			return privateKeyData;
		}).then(function (privateKeyData) {
			return {
				private: privateKeyData,
				public: {
					ecc: sodiumUtil.to_base64(new Uint8Array(
						keyPair.publicKey.buffer,
						keyPair.publicKey.byteOffset,
						eccSign.publicKeyBytes
					)),
					falcon: sodiumUtil.to_base64(new Uint8Array(
						keyPair.publicKey.buffer,
						keyPair.publicKey.byteOffset + eccSign.publicKeyBytes
					)),
					superFalcon: sodiumUtil.to_base64(keyPair.publicKey)
				}
			};
		});
	},

	importKeys: function (keyData, password) {
		return initiated.then(function () {
			if (keyData.private && typeof keyData.private.superFalcon === 'string') {
				var superFalconPrivateKey	= sodiumUtil.from_base64(keyData.private.superFalcon);

				if (password) {
					return Promise.all([decrypt(superFalconPrivateKey, password)]);
				}
				else {
					return [superFalconPrivateKey];
				}
			}
			else if (
				keyData.private &&
				typeof keyData.private.ecc === 'string' &&
				typeof keyData.private.falcon === 'string'
			) {
				var eccPrivateKey		= sodiumUtil.from_base64(keyData.private.ecc);
				var falconPrivateKey	= sodiumUtil.from_base64(keyData.private.falcon);

				if (password) {
					return Promise.all([
						decrypt(
							eccPrivateKey,
							typeof password === 'string' ? password : password.ecc
						),
						decrypt(
							falconPrivateKey,
							typeof password === 'string' ? password : password.falcon
						)
					]);
				}
				else {
					return [eccPrivateKey, falconPrivateKey];
				}

				return null;
			}
		}).then(function (results) {
			var keyPair	= {
				publicKey: new Uint8Array(publicKeyBytes),
				privateKey: null
			};

			if (!results) {
				return keyPair;
			}

			keyPair.privateKey	= new Uint8Array(privateKeyBytes);

			if (results.length === 1) {
				var superFalconPrivateKey	= results[0];

				keyPair.publicKey.set(new Uint8Array(
					superFalconPrivateKey.buffer,
					superFalconPrivateKey.byteOffset,
					publicKeyBytes
				));

				keyPair.privateKey.set(new Uint8Array(
					superFalconPrivateKey.buffer,
					superFalconPrivateKey.byteOffset + publicKeyBytes
				));
			}
			else {
				var eccPrivateKey		= results[0];
				var falconPrivateKey	= results[1];

				keyPair.publicKey.set(
					new Uint8Array(
						eccPrivateKey.buffer,
						eccPrivateKey.byteOffset,
						eccSign.publicKeyBytes
					)
				);
				keyPair.publicKey.set(
					new Uint8Array(
						falconPrivateKey.buffer,
						falconPrivateKey.byteOffset,
						falconBytes.publicKeyBytes
					),
					eccSign.publicKeyBytes
				);

				keyPair.privateKey.set(
					new Uint8Array(
						eccPrivateKey.buffer,
						eccPrivateKey.byteOffset + eccSign.publicKeyBytes
					)
				);
				keyPair.privateKey.set(
					new Uint8Array(
						falconPrivateKey.buffer,
						falconPrivateKey.byteOffset + falconBytes.publicKeyBytes
					),
					eccSign.privateKeyBytes
				);
			}

			return keyPair;
		}).then(function (keyPair) {
			if (!keyPair.privateKey) {
				if (keyData.public.superFalcon) {
					keyPair.publicKey.set(sodiumUtil.from_base64(keyData.public.superFalcon));
				}
				else if (keyData.public.ecc && keyData.public.falcon) {
					keyPair.publicKey.set(sodiumUtil.from_base64(keyData.public.ecc));
					keyPair.publicKey.set(
						sodiumUtil.from_base64(keyData.public.falcon),
						eccSign.publicKeyBytes
					);
				}
			}

			return keyPair;
		});
	}
};



superFalcon.superFalcon	= superFalcon;
module.exports				= superFalcon;
