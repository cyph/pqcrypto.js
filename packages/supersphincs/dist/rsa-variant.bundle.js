var superSphincs;
/******/ (function() { // webpackBootstrap
/******/ 	var __webpack_modules__ = ([
/* 0 */
/***/ (function(module, __unused_webpack_exports, __webpack_require__) {

var isNode	=
	typeof process === 'object' &&
	"function" === 'function' &&
	typeof window !== 'object' &&
	typeof importScripts !== 'function'
;


var fastSHA512	= __webpack_require__(1);
var rsaSign		= __webpack_require__(4);
var sodiumUtil	= __webpack_require__(3);
var sphincs		= __webpack_require__(29);


var nodeCrypto, Buffer;
if (isNode) {
	nodeCrypto	= eval('require')('crypto');
	Buffer		= eval('global.Buffer');
}


function hashWithAdditionalData (message, additionalData, preHashed) {
	var shouldClearAdditionalData	= typeof additionalData === 'string';
	var shouldClearMessage			= typeof message === 'string';

	return Promise.resolve().then(function () {
		message	= sodiumUtil.from_string(message);

		if (preHashed && message.length !== fastSHA512.bytes) {
			throw new Error('Invalid pre-hashed message.');
		}

		return Promise.all([
			fastSHA512.baseHash(
				additionalData ? sodiumUtil.from_string(additionalData) : new Uint8Array(0),
				shouldClearAdditionalData
			),
			preHashed ? message : fastSHA512.baseHash(message, shouldClearMessage)
		]);
	}).then(function (results) {
		var additionalDataHash	= results[0];
		var messageToHash		= results[1];

		var fullMessage	= new Uint8Array(additionalDataHash.length + fastSHA512.bytes);
		fullMessage.set(additionalDataHash);
		fullMessage.set(messageToHash, additionalDataHash.length);
		sodiumUtil.memzero(additionalDataHash);

		if (shouldClearMessage || !preHashed) {
			sodiumUtil.memzero(messageToHash);
		}

		return fastSHA512.baseHash(fullMessage, true);
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


var publicKeyBytes, privateKeyBytes, bytes, sphincsBytes;

var initiated	= Promise.all([
	sphincs.publicKeyBytes,
	sphincs.privateKeyBytes,
	sphincs.bytes
]).then(function (results) {
	sphincsBytes	= {
		publicKeyBytes: results[0],
		privateKeyBytes: results[1],
		bytes: results[2]
	};

	publicKeyBytes	= rsaSign.publicKeyBytes + sphincsBytes.publicKeyBytes;
	privateKeyBytes	= rsaSign.privateKeyBytes + sphincsBytes.privateKeyBytes;
	bytes			= rsaSign.bytes + sphincsBytes.bytes;
});


var superSphincs	= {
	_sodiumUtil: sodiumUtil,
	publicKeyBytes: initiated.then(function () { return publicKeyBytes; }),
	privateKeyBytes: initiated.then(function () { return privateKeyBytes; }),
	bytes: initiated.then(function () { return bytes; }),
	hashBytes: Promise.resolve(fastSHA512.bytes),

	hash: function (message, onlyBinary) {
		return fastSHA512.hash(message, onlyBinary);
	},

	keyPair: function () { return initiated.then(function () {
		return Promise.all([
			rsaSign.keyPair(),
			sphincs.keyPair()
		]).then(function (results) {
			var rsaKeyPair		= results[0];
			var sphincsKeyPair	= results[1];

			var keyPair	= {
				keyType: 'supersphincs',
				publicKey: new Uint8Array(publicKeyBytes),
				privateKey: new Uint8Array(privateKeyBytes)
			};

			keyPair.publicKey.set(rsaKeyPair.publicKey);
			keyPair.privateKey.set(rsaKeyPair.privateKey);
			keyPair.publicKey.set(sphincsKeyPair.publicKey, rsaSign.publicKeyBytes);
			keyPair.privateKey.set(sphincsKeyPair.privateKey, rsaSign.privateKeyBytes);

			sodiumUtil.memzero(sphincsKeyPair.privateKey);
			sodiumUtil.memzero(rsaKeyPair.privateKey);
			sodiumUtil.memzero(sphincsKeyPair.publicKey);
			sodiumUtil.memzero(rsaKeyPair.publicKey);

			return keyPair;
		});
	}); },

	sign: function (message, privateKey, additionalData) { return initiated.then(function () {
		var shouldClearMessage	= typeof message === 'string';

		return superSphincs.signDetached(
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
		return superSphincs.sign(message, privateKey, additionalData).then(function (signed) {
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
				rsaSign.signDetached(
					hash,
					new Uint8Array(
						privateKey.buffer,
						privateKey.byteOffset,
						rsaSign.privateKeyBytes
					)
				),
				sphincs.signDetached(
					hash,
					new Uint8Array(
						privateKey.buffer,
						privateKey.byteOffset + rsaSign.privateKeyBytes
					)
				)
			]);
		}).then(function (results) {
			var hash				= results[0];
			var rsaSignature		= results[1];
			var sphincsSignature	= results[2];

			var signature	= new Uint8Array(bytes);

			signature.set(rsaSignature);
			signature.set(sphincsSignature, rsaSign.bytes);

			sodiumUtil.memzero(hash);
			sodiumUtil.memzero(sphincsSignature);
			sodiumUtil.memzero(rsaSignature);

			return signature;
		});
	}); },

	signDetachedBase64: function (
		message,
		privateKey,
		additionalData,
		preHashed
	) { return initiated.then(function () {
			return superSphincs.signDetached(
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

			return Promise.all([message, superSphincs.verifyDetached(
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
				throw new Error('Failed to open SuperSPHINCS signed message.');
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
		return superSphincs.open(
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
					superSphincs.importKeys(publicKey).then(function (kp) {
						return kp.publicKey;
					})
			;

			return Promise.all([
				hash,
				hashAlreadyVerified || publicKeyPromise.then(function (pk) {
					return rsaSign.verifyDetached(
						new Uint8Array(signature.buffer, signature.byteOffset, rsaSign.bytes),
						hash,
						new Uint8Array(pk.buffer, pk.byteOffset, rsaSign.publicKeyBytes)
					);
				}),
				hashAlreadyVerified || publicKeyPromise.then(function (pk) {
					return sphincs.verifyDetached(
						new Uint8Array(
							signature.buffer,
							signature.byteOffset + rsaSign.bytes,
							sphincsBytes.bytes
						),
						hash,
						new Uint8Array(pk.buffer, pk.byteOffset + rsaSign.publicKeyBytes)
					);
				})
			]);
		}).then(function (results) {
			var hash			= results[0];
			var rsaIsValid		= results[1];
			var sphincsIsValid	= results[2];
			var valid			= rsaIsValid && sphincsIsValid;

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

			var rsaPrivateKey			= new Uint8Array(
				rsaSign.publicKeyBytes +
				rsaSign.privateKeyBytes
			);

			var sphincsPrivateKey		= new Uint8Array(
				sphincsBytes.publicKeyBytes +
				sphincsBytes.privateKeyBytes
			);

			var superSphincsPrivateKey	= new Uint8Array(
				publicKeyBytes +
				privateKeyBytes
			);

			rsaPrivateKey.set(new Uint8Array(
				keyPair.publicKey.buffer,
				keyPair.publicKey.byteOffset,
				rsaSign.publicKeyBytes
			));
			rsaPrivateKey.set(
				new Uint8Array(
					keyPair.privateKey.buffer,
					keyPair.privateKey.byteOffset,
					rsaSign.privateKeyBytes
				),
				rsaSign.publicKeyBytes
			);

			sphincsPrivateKey.set(new Uint8Array(
				keyPair.publicKey.buffer,
				keyPair.publicKey.byteOffset + rsaSign.publicKeyBytes
			));
			sphincsPrivateKey.set(
				new Uint8Array(
					keyPair.privateKey.buffer,
					keyPair.privateKey.byteOffset + rsaSign.privateKeyBytes
				),
				sphincsBytes.publicKeyBytes
			);

			superSphincsPrivateKey.set(keyPair.publicKey);
			superSphincsPrivateKey.set(keyPair.privateKey, publicKeyBytes);

			if (password != null && password.length > 0) {
				return Promise.all([
					encrypt(rsaPrivateKey, password),
					encrypt(sphincsPrivateKey, password),
					encrypt(superSphincsPrivateKey, password)
				]).then(function (results) {
					sodiumUtil.memzero(superSphincsPrivateKey);
					sodiumUtil.memzero(sphincsPrivateKey);
					sodiumUtil.memzero(rsaPrivateKey);

					return results;
				});
			}
			else {
				return [
					rsaPrivateKey,
					sphincsPrivateKey,
					superSphincsPrivateKey
				];
			}
		}).then(function (results) {
			if (!results) {
				return {
					rsa: null,
					sphincs: null,
					superSphincs: null
				};
			}

			var rsaPrivateKey			= results[0];
			var sphincsPrivateKey		= results[1];
			var superSphincsPrivateKey	= results[2];

			var privateKeyData	= {
				rsa: sodiumUtil.to_base64(rsaPrivateKey),
				sphincs: sodiumUtil.to_base64(sphincsPrivateKey),
				superSphincs: sodiumUtil.to_base64(superSphincsPrivateKey)
			};

			sodiumUtil.memzero(superSphincsPrivateKey);
			sodiumUtil.memzero(sphincsPrivateKey);
			sodiumUtil.memzero(rsaPrivateKey);

			return privateKeyData;
		}).then(function (privateKeyData) {
			return {
				private: privateKeyData,
				public: {
					rsa: sodiumUtil.to_base64(new Uint8Array(
						keyPair.publicKey.buffer,
						keyPair.publicKey.byteOffset,
						rsaSign.publicKeyBytes
					)),
					sphincs: sodiumUtil.to_base64(new Uint8Array(
						keyPair.publicKey.buffer,
						keyPair.publicKey.byteOffset + rsaSign.publicKeyBytes
					)),
					superSphincs: sodiumUtil.to_base64(keyPair.publicKey)
				}
			};
		});
	},

	importKeys: function (keyData, password) {
		return initiated.then(function () {
			if (keyData.private && typeof keyData.private.superSphincs === 'string') {
				var superSphincsPrivateKey	= sodiumUtil.from_base64(keyData.private.superSphincs);

				if (password != null && password.length > 0) {
					return Promise.all([decrypt(superSphincsPrivateKey, password)]);
				}
				else {
					return [superSphincsPrivateKey];
				}
			}
			else if (
				keyData.private &&
				typeof keyData.private.rsa === 'string' &&
				typeof keyData.private.sphincs === 'string'
			) {
				var rsaPrivateKey		= sodiumUtil.from_base64(keyData.private.rsa);
				var sphincsPrivateKey	= sodiumUtil.from_base64(keyData.private.sphincs);

				if (password == null || password.length < 1) {
					return [rsaPrivateKey, sphincsPrivateKey];
				}

				return Promise.all([
					decrypt(
						rsaPrivateKey,
						typeof password === 'string' ? password : password.rsa
					),
					decrypt(
						sphincsPrivateKey,
						typeof password === 'string' ? password : password.sphincs
					)
				]);
			}

			return null;
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
				var superSphincsPrivateKey	= results[0];

				keyPair.publicKey.set(new Uint8Array(
					superSphincsPrivateKey.buffer,
					superSphincsPrivateKey.byteOffset,
					publicKeyBytes
				));

				keyPair.privateKey.set(new Uint8Array(
					superSphincsPrivateKey.buffer,
					superSphincsPrivateKey.byteOffset + publicKeyBytes
				));
			}
			else {
				var rsaPrivateKey		= results[0];
				var sphincsPrivateKey	= results[1];

				keyPair.publicKey.set(
					new Uint8Array(
						rsaPrivateKey.buffer,
						rsaPrivateKey.byteOffset,
						rsaSign.publicKeyBytes
					)
				);
				keyPair.publicKey.set(
					new Uint8Array(
						sphincsPrivateKey.buffer,
						sphincsPrivateKey.byteOffset,
						sphincsBytes.publicKeyBytes
					),
					rsaSign.publicKeyBytes
				);

				keyPair.privateKey.set(
					new Uint8Array(
						rsaPrivateKey.buffer,
						rsaPrivateKey.byteOffset + rsaSign.publicKeyBytes
					)
				);
				keyPair.privateKey.set(
					new Uint8Array(
						sphincsPrivateKey.buffer,
						sphincsPrivateKey.byteOffset + sphincsBytes.publicKeyBytes
					),
					rsaSign.privateKeyBytes
				);
			}

			return keyPair;
		}).then(function (keyPair) {
			if (!keyPair.privateKey) {
				if (keyData.public.superSphincs) {
					keyPair.publicKey.set(sodiumUtil.from_base64(keyData.public.superSphincs));
				}
				else if (keyData.public.rsa && keyData.public.sphincs) {
					keyPair.publicKey.set(sodiumUtil.from_base64(keyData.public.rsa));
					keyPair.publicKey.set(
						sodiumUtil.from_base64(keyData.public.sphincs),
						rsaSign.publicKeyBytes
					);
				}
			}

			return keyPair;
		});
	}
};



superSphincs.superSphincs	= superSphincs;
module.exports				= superSphincs;


/***/ }),
/* 1 */
/***/ (function(module, __unused_webpack_exports, __webpack_require__) {

var isNode	=
	typeof process === 'object' &&
	"function" === 'function' &&
	typeof window !== 'object' &&
	typeof importScripts !== 'function'
;


var naclSHA512	= __webpack_require__(2);
var sodiumUtil	= __webpack_require__(3);


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


/***/ }),
/* 2 */
/***/ (function(module) {

/* Pulled out of https://github.com/dchest/tweetnacl-js/blob/8626b70/nacl-fast.js */

var crypto_hash_BYTES = 64;

var K = [
	0x428a2f98, 0xd728ae22, 0x71374491, 0x23ef65cd,
	0xb5c0fbcf, 0xec4d3b2f, 0xe9b5dba5, 0x8189dbbc,
	0x3956c25b, 0xf348b538, 0x59f111f1, 0xb605d019,
	0x923f82a4, 0xaf194f9b, 0xab1c5ed5, 0xda6d8118,
	0xd807aa98, 0xa3030242, 0x12835b01, 0x45706fbe,
	0x243185be, 0x4ee4b28c, 0x550c7dc3, 0xd5ffb4e2,
	0x72be5d74, 0xf27b896f, 0x80deb1fe, 0x3b1696b1,
	0x9bdc06a7, 0x25c71235, 0xc19bf174, 0xcf692694,
	0xe49b69c1, 0x9ef14ad2, 0xefbe4786, 0x384f25e3,
	0x0fc19dc6, 0x8b8cd5b5, 0x240ca1cc, 0x77ac9c65,
	0x2de92c6f, 0x592b0275, 0x4a7484aa, 0x6ea6e483,
	0x5cb0a9dc, 0xbd41fbd4, 0x76f988da, 0x831153b5,
	0x983e5152, 0xee66dfab, 0xa831c66d, 0x2db43210,
	0xb00327c8, 0x98fb213f, 0xbf597fc7, 0xbeef0ee4,
	0xc6e00bf3, 0x3da88fc2, 0xd5a79147, 0x930aa725,
	0x06ca6351, 0xe003826f, 0x14292967, 0x0a0e6e70,
	0x27b70a85, 0x46d22ffc, 0x2e1b2138, 0x5c26c926,
	0x4d2c6dfc, 0x5ac42aed, 0x53380d13, 0x9d95b3df,
	0x650a7354, 0x8baf63de, 0x766a0abb, 0x3c77b2a8,
	0x81c2c92e, 0x47edaee6, 0x92722c85, 0x1482353b,
	0xa2bfe8a1, 0x4cf10364, 0xa81a664b, 0xbc423001,
	0xc24b8b70, 0xd0f89791, 0xc76c51a3, 0x0654be30,
	0xd192e819, 0xd6ef5218, 0xd6990624, 0x5565a910,
	0xf40e3585, 0x5771202a, 0x106aa070, 0x32bbd1b8,
	0x19a4c116, 0xb8d2d0c8, 0x1e376c08, 0x5141ab53,
	0x2748774c, 0xdf8eeb99, 0x34b0bcb5, 0xe19b48a8,
	0x391c0cb3, 0xc5c95a63, 0x4ed8aa4a, 0xe3418acb,
	0x5b9cca4f, 0x7763e373, 0x682e6ff3, 0xd6b2b8a3,
	0x748f82ee, 0x5defb2fc, 0x78a5636f, 0x43172f60,
	0x84c87814, 0xa1f0ab72, 0x8cc70208, 0x1a6439ec,
	0x90befffa, 0x23631e28, 0xa4506ceb, 0xde82bde9,
	0xbef9a3f7, 0xb2c67915, 0xc67178f2, 0xe372532b,
	0xca273ece, 0xea26619c, 0xd186b8c7, 0x21c0c207,
	0xeada7dd6, 0xcde0eb1e, 0xf57d4f7f, 0xee6ed178,
	0x06f067aa, 0x72176fba, 0x0a637dc5, 0xa2c898a6,
	0x113f9804, 0xbef90dae, 0x1b710b35, 0x131c471b,
	0x28db77f5, 0x23047d84, 0x32caab7b, 0x40c72493,
	0x3c9ebe0a, 0x15c9bebc, 0x431d67c4, 0x9c100d4c,
	0x4cc5d4be, 0xcb3e42b6, 0x597f299c, 0xfc657e2a,
	0x5fcb6fab, 0x3ad6faec, 0x6c44198c, 0x4a475817
];

function ts64(x, i, h, l) {
	x[i]   = (h >> 24) & 0xff;
	x[i+1] = (h >> 16) & 0xff;
	x[i+2] = (h >>  8) & 0xff;
	x[i+3] = h & 0xff;
	x[i+4] = (l >> 24)  & 0xff;
	x[i+5] = (l >> 16)  & 0xff;
	x[i+6] = (l >>  8)  & 0xff;
	x[i+7] = l & 0xff;
}

function crypto_hashblocks_hl(hh, hl, m, n) {
	var wh = new Int32Array(16), wl = new Int32Array(16),
		bh0, bh1, bh2, bh3, bh4, bh5, bh6, bh7,
		bl0, bl1, bl2, bl3, bl4, bl5, bl6, bl7,
		th, tl, i, j, h, l, a, b, c, d;

	var ah0 = hh[0],
		ah1 = hh[1],
		ah2 = hh[2],
		ah3 = hh[3],
		ah4 = hh[4],
		ah5 = hh[5],
		ah6 = hh[6],
		ah7 = hh[7],

		al0 = hl[0],
		al1 = hl[1],
		al2 = hl[2],
		al3 = hl[3],
		al4 = hl[4],
		al5 = hl[5],
		al6 = hl[6],
		al7 = hl[7];

	var pos = 0;
	while (n >= 128) {
		for (i = 0; i < 16; i++) {
		j = 8 * i + pos;
		wh[i] = (m[j+0] << 24) | (m[j+1] << 16) | (m[j+2] << 8) | m[j+3];
		wl[i] = (m[j+4] << 24) | (m[j+5] << 16) | (m[j+6] << 8) | m[j+7];
		}
		for (i = 0; i < 80; i++) {
		bh0 = ah0;
		bh1 = ah1;
		bh2 = ah2;
		bh3 = ah3;
		bh4 = ah4;
		bh5 = ah5;
		bh6 = ah6;
		bh7 = ah7;

		bl0 = al0;
		bl1 = al1;
		bl2 = al2;
		bl3 = al3;
		bl4 = al4;
		bl5 = al5;
		bl6 = al6;
		bl7 = al7;

		// add
		h = ah7;
		l = al7;

		a = l & 0xffff; b = l >>> 16;
		c = h & 0xffff; d = h >>> 16;

		// Sigma1
		h = ((ah4 >>> 14) | (al4 << (32-14))) ^ ((ah4 >>> 18) | (al4 << (32-18))) ^ ((al4 >>> (41-32)) | (ah4 << (32-(41-32))));
		l = ((al4 >>> 14) | (ah4 << (32-14))) ^ ((al4 >>> 18) | (ah4 << (32-18))) ^ ((ah4 >>> (41-32)) | (al4 << (32-(41-32))));

		a += l & 0xffff; b += l >>> 16;
		c += h & 0xffff; d += h >>> 16;

		// Ch
		h = (ah4 & ah5) ^ (~ah4 & ah6);
		l = (al4 & al5) ^ (~al4 & al6);

		a += l & 0xffff; b += l >>> 16;
		c += h & 0xffff; d += h >>> 16;

		// K
		h = K[i*2];
		l = K[i*2+1];

		a += l & 0xffff; b += l >>> 16;
		c += h & 0xffff; d += h >>> 16;

		// w
		h = wh[i%16];
		l = wl[i%16];

		a += l & 0xffff; b += l >>> 16;
		c += h & 0xffff; d += h >>> 16;

		b += a >>> 16;
		c += b >>> 16;
		d += c >>> 16;

		th = c & 0xffff | d << 16;
		tl = a & 0xffff | b << 16;

		// add
		h = th;
		l = tl;

		a = l & 0xffff; b = l >>> 16;
		c = h & 0xffff; d = h >>> 16;

		// Sigma0
		h = ((ah0 >>> 28) | (al0 << (32-28))) ^ ((al0 >>> (34-32)) | (ah0 << (32-(34-32)))) ^ ((al0 >>> (39-32)) | (ah0 << (32-(39-32))));
		l = ((al0 >>> 28) | (ah0 << (32-28))) ^ ((ah0 >>> (34-32)) | (al0 << (32-(34-32)))) ^ ((ah0 >>> (39-32)) | (al0 << (32-(39-32))));

		a += l & 0xffff; b += l >>> 16;
		c += h & 0xffff; d += h >>> 16;

		// Maj
		h = (ah0 & ah1) ^ (ah0 & ah2) ^ (ah1 & ah2);
		l = (al0 & al1) ^ (al0 & al2) ^ (al1 & al2);

		a += l & 0xffff; b += l >>> 16;
		c += h & 0xffff; d += h >>> 16;

		b += a >>> 16;
		c += b >>> 16;
		d += c >>> 16;

		bh7 = (c & 0xffff) | (d << 16);
		bl7 = (a & 0xffff) | (b << 16);

		// add
		h = bh3;
		l = bl3;

		a = l & 0xffff; b = l >>> 16;
		c = h & 0xffff; d = h >>> 16;

		h = th;
		l = tl;

		a += l & 0xffff; b += l >>> 16;
		c += h & 0xffff; d += h >>> 16;

		b += a >>> 16;
		c += b >>> 16;
		d += c >>> 16;

		bh3 = (c & 0xffff) | (d << 16);
		bl3 = (a & 0xffff) | (b << 16);

		ah1 = bh0;
		ah2 = bh1;
		ah3 = bh2;
		ah4 = bh3;
		ah5 = bh4;
		ah6 = bh5;
		ah7 = bh6;
		ah0 = bh7;

		al1 = bl0;
		al2 = bl1;
		al3 = bl2;
		al4 = bl3;
		al5 = bl4;
		al6 = bl5;
		al7 = bl6;
		al0 = bl7;

		if (i%16 === 15) {
			for (j = 0; j < 16; j++) {
			// add
			h = wh[j];
			l = wl[j];

			a = l & 0xffff; b = l >>> 16;
			c = h & 0xffff; d = h >>> 16;

			h = wh[(j+9)%16];
			l = wl[(j+9)%16];

			a += l & 0xffff; b += l >>> 16;
			c += h & 0xffff; d += h >>> 16;

			// sigma0
			th = wh[(j+1)%16];
			tl = wl[(j+1)%16];
			h = ((th >>> 1) | (tl << (32-1))) ^ ((th >>> 8) | (tl << (32-8))) ^ (th >>> 7);
			l = ((tl >>> 1) | (th << (32-1))) ^ ((tl >>> 8) | (th << (32-8))) ^ ((tl >>> 7) | (th << (32-7)));

			a += l & 0xffff; b += l >>> 16;
			c += h & 0xffff; d += h >>> 16;

			// sigma1
			th = wh[(j+14)%16];
			tl = wl[(j+14)%16];
			h = ((th >>> 19) | (tl << (32-19))) ^ ((tl >>> (61-32)) | (th << (32-(61-32)))) ^ (th >>> 6);
			l = ((tl >>> 19) | (th << (32-19))) ^ ((th >>> (61-32)) | (tl << (32-(61-32)))) ^ ((tl >>> 6) | (th << (32-6)));

			a += l & 0xffff; b += l >>> 16;
			c += h & 0xffff; d += h >>> 16;

			b += a >>> 16;
			c += b >>> 16;
			d += c >>> 16;

			wh[j] = (c & 0xffff) | (d << 16);
			wl[j] = (a & 0xffff) | (b << 16);
			}
		}
		}

		// add
		h = ah0;
		l = al0;

		a = l & 0xffff; b = l >>> 16;
		c = h & 0xffff; d = h >>> 16;

		h = hh[0];
		l = hl[0];

		a += l & 0xffff; b += l >>> 16;
		c += h & 0xffff; d += h >>> 16;

		b += a >>> 16;
		c += b >>> 16;
		d += c >>> 16;

		hh[0] = ah0 = (c & 0xffff) | (d << 16);
		hl[0] = al0 = (a & 0xffff) | (b << 16);

		h = ah1;
		l = al1;

		a = l & 0xffff; b = l >>> 16;
		c = h & 0xffff; d = h >>> 16;

		h = hh[1];
		l = hl[1];

		a += l & 0xffff; b += l >>> 16;
		c += h & 0xffff; d += h >>> 16;

		b += a >>> 16;
		c += b >>> 16;
		d += c >>> 16;

		hh[1] = ah1 = (c & 0xffff) | (d << 16);
		hl[1] = al1 = (a & 0xffff) | (b << 16);

		h = ah2;
		l = al2;

		a = l & 0xffff; b = l >>> 16;
		c = h & 0xffff; d = h >>> 16;

		h = hh[2];
		l = hl[2];

		a += l & 0xffff; b += l >>> 16;
		c += h & 0xffff; d += h >>> 16;

		b += a >>> 16;
		c += b >>> 16;
		d += c >>> 16;

		hh[2] = ah2 = (c & 0xffff) | (d << 16);
		hl[2] = al2 = (a & 0xffff) | (b << 16);

		h = ah3;
		l = al3;

		a = l & 0xffff; b = l >>> 16;
		c = h & 0xffff; d = h >>> 16;

		h = hh[3];
		l = hl[3];

		a += l & 0xffff; b += l >>> 16;
		c += h & 0xffff; d += h >>> 16;

		b += a >>> 16;
		c += b >>> 16;
		d += c >>> 16;

		hh[3] = ah3 = (c & 0xffff) | (d << 16);
		hl[3] = al3 = (a & 0xffff) | (b << 16);

		h = ah4;
		l = al4;

		a = l & 0xffff; b = l >>> 16;
		c = h & 0xffff; d = h >>> 16;

		h = hh[4];
		l = hl[4];

		a += l & 0xffff; b += l >>> 16;
		c += h & 0xffff; d += h >>> 16;

		b += a >>> 16;
		c += b >>> 16;
		d += c >>> 16;

		hh[4] = ah4 = (c & 0xffff) | (d << 16);
		hl[4] = al4 = (a & 0xffff) | (b << 16);

		h = ah5;
		l = al5;

		a = l & 0xffff; b = l >>> 16;
		c = h & 0xffff; d = h >>> 16;

		h = hh[5];
		l = hl[5];

		a += l & 0xffff; b += l >>> 16;
		c += h & 0xffff; d += h >>> 16;

		b += a >>> 16;
		c += b >>> 16;
		d += c >>> 16;

		hh[5] = ah5 = (c & 0xffff) | (d << 16);
		hl[5] = al5 = (a & 0xffff) | (b << 16);

		h = ah6;
		l = al6;

		a = l & 0xffff; b = l >>> 16;
		c = h & 0xffff; d = h >>> 16;

		h = hh[6];
		l = hl[6];

		a += l & 0xffff; b += l >>> 16;
		c += h & 0xffff; d += h >>> 16;

		b += a >>> 16;
		c += b >>> 16;
		d += c >>> 16;

		hh[6] = ah6 = (c & 0xffff) | (d << 16);
		hl[6] = al6 = (a & 0xffff) | (b << 16);

		h = ah7;
		l = al7;

		a = l & 0xffff; b = l >>> 16;
		c = h & 0xffff; d = h >>> 16;

		h = hh[7];
		l = hl[7];

		a += l & 0xffff; b += l >>> 16;
		c += h & 0xffff; d += h >>> 16;

		b += a >>> 16;
		c += b >>> 16;
		d += c >>> 16;

		hh[7] = ah7 = (c & 0xffff) | (d << 16);
		hl[7] = al7 = (a & 0xffff) | (b << 16);

		pos += 128;
		n -= 128;
	}

	return n;
	}

	function crypto_hash(out, m, n) {
	var hh = new Int32Array(8),
		hl = new Int32Array(8),
		x = new Uint8Array(256),
		i, b = n;

	hh[0] = 0x6a09e667;
	hh[1] = 0xbb67ae85;
	hh[2] = 0x3c6ef372;
	hh[3] = 0xa54ff53a;
	hh[4] = 0x510e527f;
	hh[5] = 0x9b05688c;
	hh[6] = 0x1f83d9ab;
	hh[7] = 0x5be0cd19;

	hl[0] = 0xf3bcc908;
	hl[1] = 0x84caa73b;
	hl[2] = 0xfe94f82b;
	hl[3] = 0x5f1d36f1;
	hl[4] = 0xade682d1;
	hl[5] = 0x2b3e6c1f;
	hl[6] = 0xfb41bd6b;
	hl[7] = 0x137e2179;

	crypto_hashblocks_hl(hh, hl, m, n);
	n %= 128;

	for (i = 0; i < n; i++) x[i] = m[b-n+i];
	x[n] = 128;

	n = 256-128*(n<112?1:0);
	x[n-9] = 0;
	ts64(x, n-8,  (b / 0x20000000) | 0, b << 3);
	crypto_hashblocks_hl(hh, hl, x, n);

	for (i = 0; i < 8; i++) ts64(out, 8*i, hh[i], hl[i]);

	return 0;
}

module.exports = function(msg) {
	var h = new Uint8Array(crypto_hash_BYTES);
	crypto_hash(h, msg, msg.length);
	return h;
};


/***/ }),
/* 3 */
/***/ (function(module) {

var sodiumUtil=function(){function r(e){if("function"==typeof TextDecoder)return new TextDecoder("utf-8",{fatal:!0}).decode(e);var n=8192,t=Math.ceil(e.length/n);if(t<=1)try{return decodeURIComponent(escape(String.fromCharCode.apply(null,e)))}catch(r){throw new TypeError("The encoded data was not valid.")}for(var o="",i=0,f=0;f<t;f++){var a=Array.prototype.slice.call(e,f*n+i,(f+1)*n+i);if(0!=a.length){var u,c=a.length,l=0;do{var s=a[--c];s>=240?(l=4,u=!0):s>=224?(l=3,u=!0):s>=192?(l=2,u=!0):s<128&&(l=1,u=!0)}while(!u);for(var d=l-(a.length-c),p=0;p<d;p++)i--,a.pop();o+=r(a)}}return o}function e(r){for(var e,n,t,o="",i=0;i<r.length;i++)t=87+(n=15&r[i])+(n-10>>8&-39)<<8|87+(e=r[i]>>>4)+(e-10>>8&-39),o+=String.fromCharCode(255&t)+String.fromCharCode(t>>>8);return o}function n(r,e){function n(r){return r<26?r+65:r<52?r+71:r<62?r-4:62===r?43:63===r?47:65}if(void 0===e&&(e=!0),"string"==typeof r)throw new Error("input has to be an array");for(var t=2,o="",i=r.length,f=0,a=0;a<i;a++)t=a%3,a>0&&4*a/3%76==0&&!e&&(o+="\r\n"),f|=r[a]<<(16>>>t&24),2!==t&&r.length-a!=1||(o+=String.fromCharCode(n(f>>>18&63),n(f>>>12&63),n(f>>>6&63),n(63&f)),f=0);return o.substr(0,o.length-2+t)+(2===t?"":1===t?"=":"==")}return{from_base64:function(r){return"string"==typeof r?function(r,e){for(var n,t,o,i=r.replace(/[^A-Za-z0-9\+\/]/g,""),f=i.length,a=e?Math.ceil((3*f+1>>2)/e)*e:3*f+1>>2,u=new Uint8Array(a),c=0,l=0,s=0;s<f;s++)if(t=3&s,c|=((o=i.charCodeAt(s))>64&&o<91?o-65:o>96&&o<123?o-71:o>47&&o<58?o+4:43===o?62:47===o?63:0)<<18-6*t,3===t||f-s==1){for(n=0;n<3&&l<a;n++,l++)u[l]=c>>>(16>>>n&24)&255;c=0}return u}(r):r},from_base64url:function(r){return"string"==typeof r?this.from_base64(r.replace(/-/g,"+").replace(/_/g,"/")):r},from_hex:function(r){return"string"==typeof r?function(r){if(!function(r){return"string"==typeof r&&/^[0-9a-f]+$/i.test(r)&&r.length%2==0}(r))throw new TypeError("The provided string doesn't look like hex data");for(var e=new Uint8Array(r.length/2),n=0;n<r.length;n+=2)e[n>>>1]=parseInt(r.substr(n,2),16);return e}(r):r},from_string:function(r){return"string"==typeof r?function(r){if("function"==typeof TextEncoder)return new TextEncoder("utf-8").encode(r);r=unescape(encodeURIComponent(r));for(var e=new Uint8Array(r.length),n=0;n<r.length;n++)e[n]=r.charCodeAt(n);return e}(r):r},memcmp:function(r,e){if(!(r instanceof Uint8Array&&e instanceof Uint8Array))throw new TypeError("Only Uint8Array instances can be compared");if(r.length!==e.length)throw new TypeError("Only instances of identical length can be compared");for(var n=0,t=0,o=r.length;t<o;t++)n|=r[t]^e[t];return 0===n},memzero:function(r){r instanceof Uint8Array?function(r){if(!r instanceof Uint8Array)throw new TypeError("Only Uint8Array instances can be wiped");for(var e=0,n=r.length;e<n;e++)r[e]=0}(r):"undefined"!=typeof Buffer&&r instanceof Buffer&&r.fill(0)},to_base64:function(r){return"string"==typeof r?r:n(r).replace(/\s+/g,"")},to_base64url:function(r){return this.to_base64(r).replace(/\+/g,"-").replace(/\//g,"_")},to_hex:function(r){return"string"==typeof r?r:e(r).replace(/\s+/g,"")},to_string:function(e){return"string"==typeof e?e:r(e)}}}(); true&&module.exports?(sodiumUtil.sodiumUtil=sodiumUtil,module.exports=sodiumUtil):self.sodiumUtil=sodiumUtil;

/***/ }),
/* 4 */
/***/ (function(module, __unused_webpack_exports, __webpack_require__) {

var __dirname = "/";
var Module={},isNode="object"==typeof process&&"function"=="function"&&"object"!=typeof window&&"function"!=typeof importScripts,pemJwk=__webpack_require__(5),sodiumUtil=__webpack_require__(28),initiated,nodeCrypto,generateRSAKeypair;if(isNode)initiated=Promise.resolve(),nodeCrypto=eval("require")("crypto"),generateRSAKeypair=eval("require")("generate-rsa-keypair");else{initiated=new Promise((function(A,e){Module.onAbort=e,Module.onRuntimeInitialized=function(){try{Module._rsasignjs_init(),A()}catch(A){e(A)}}}));var Module=void 0!==Module?Module:{},moduleOverrides={},key;for(key in Module)Module.hasOwnProperty(key)&&(moduleOverrides[key]=Module[key]);var arguments_=[],thisProgram="./this.program",quit_=function(A,e){throw e},ENVIRONMENT_IS_WEB=!1,ENVIRONMENT_IS_WORKER=!1,ENVIRONMENT_IS_NODE=!1,ENVIRONMENT_IS_SHELL=!1;ENVIRONMENT_IS_WEB="object"==typeof window,ENVIRONMENT_IS_WORKER="function"==typeof importScripts,ENVIRONMENT_IS_NODE="object"==typeof process&&"object"==typeof process.versions&&"string"==typeof process.versions.node,ENVIRONMENT_IS_SHELL=!ENVIRONMENT_IS_WEB&&!ENVIRONMENT_IS_NODE&&!ENVIRONMENT_IS_WORKER;var scriptDirectory="",read_,readAsync,readBinary,setWindowTitle,nodeFS,nodePath;function locateFile(A){return Module.locateFile?Module.locateFile(A,scriptDirectory):scriptDirectory+A}ENVIRONMENT_IS_NODE?(scriptDirectory=ENVIRONMENT_IS_WORKER?eval("require")("path").dirname(scriptDirectory)+"/":__dirname+"/",read_=function shell_read(filename,binary){var ret=tryParseAsDataURI(filename);return ret?binary?ret:ret.toString():(nodeFS||(nodeFS=eval("require")("fs")),nodePath||(nodePath=eval("require")("path")),filename=nodePath.normalize(filename),nodeFS.readFileSync(filename,binary?null:"utf8"))},readBinary=function(A){var e=read_(A,!0);return e.buffer||(e=new Uint8Array(e)),assert(e.buffer),e},process.argv.length>1&&(thisProgram=process.argv[1].replace(/\\/g,"/")),arguments_=process.argv.slice(2), true&&(module.exports=Module),process.on("uncaughtException",(function(A){if(!(A instanceof ExitStatus))throw A})),process.on("unhandledRejection",abort),quit_=function(A){process.exit(A)},Module.inspect=function(){return"[Emscripten Module object]"}):ENVIRONMENT_IS_SHELL?("undefined"!=typeof read&&(read_=function(A){var e=tryParseAsDataURI(A);return e?intArrayToString(e):read(A)}),readBinary=function(A){var e;return(e=tryParseAsDataURI(A))?e:"function"==typeof readbuffer?new Uint8Array(readbuffer(A)):(assert("object"==typeof(e=read(A,"binary"))),e)},"undefined"!=typeof scriptArgs?arguments_=scriptArgs:"undefined"!=typeof arguments&&(arguments_=arguments),"function"==typeof quit&&(quit_=function(A){quit(A)}),"undefined"!=typeof print&&("undefined"==typeof console&&(console={}),console.log=print,console.warn=console.error="undefined"!=typeof printErr?printErr:print)):(ENVIRONMENT_IS_WEB||ENVIRONMENT_IS_WORKER)&&(ENVIRONMENT_IS_WORKER?scriptDirectory=self.location.href:document.currentScript&&(scriptDirectory=document.currentScript.src),scriptDirectory=0!==scriptDirectory.indexOf("blob:")?scriptDirectory.substr(0,scriptDirectory.lastIndexOf("/")+1):"",read_=function(A){try{var e=new XMLHttpRequest;return e.open("GET",A,!1),e.send(null),e.responseText}catch(e){var r=tryParseAsDataURI(A);if(r)return intArrayToString(r);throw e}},ENVIRONMENT_IS_WORKER&&(readBinary=function(A){try{var e=new XMLHttpRequest;return e.open("GET",A,!1),e.responseType="arraybuffer",e.send(null),new Uint8Array(e.response)}catch(e){var r=tryParseAsDataURI(A);if(r)return r;throw e}}),readAsync=function(A,e,r){var i=new XMLHttpRequest;i.open("GET",A,!0),i.responseType="arraybuffer",i.onload=function(){if(200==i.status||0==i.status&&i.response)e(i.response);else{var n=tryParseAsDataURI(A);n?e(n.buffer):r()}},i.onerror=r,i.send(null)},setWindowTitle=function(A){document.title=A});var out=Module.print||console.log.bind(console),err=Module.printErr||console.warn.bind(console);for(key in moduleOverrides)moduleOverrides.hasOwnProperty(key)&&(Module[key]=moduleOverrides[key]);moduleOverrides=null,Module.arguments&&(arguments_=Module.arguments),Module.thisProgram&&(thisProgram=Module.thisProgram),Module.quit&&(quit_=Module.quit);var STACK_ALIGN=16;function dynamicAlloc(A){var e=HEAP32[DYNAMICTOP_PTR>>2],r=e+A+15&-16;return HEAP32[DYNAMICTOP_PTR>>2]=r,e}function getNativeTypeSize(A){switch(A){case"i1":case"i8":return 1;case"i16":return 2;case"i32":return 4;case"i64":return 8;case"float":return 4;case"double":return 8;default:if("*"===A[A.length-1])return 4;if("i"===A[0]){var e=Number(A.substr(1));return assert(e%8==0,"getNativeTypeSize invalid bits "+e+", type "+A),e/8}return 0}}function warnOnce(A){warnOnce.shown||(warnOnce.shown={}),warnOnce.shown[A]||(warnOnce.shown[A]=1,err(A))}var jsCallStartIndex=1,functionPointers=new Array(0),funcWrappers={};function dynCall(A,e,r){return r&&r.length?Module["dynCall_"+A].apply(null,[e].concat(r)):Module["dynCall_"+A].call(null,e)}var tempRet0=0,setTempRet0=function(A){tempRet0=A},getTempRet0=function(){return tempRet0},GLOBAL_BASE=8,wasmBinary,noExitRuntime;function setValue(A,e,r,i){switch("*"===(r=r||"i8").charAt(r.length-1)&&(r="i32"),r){case"i1":case"i8":HEAP8[A>>0]=e;break;case"i16":HEAP16[A>>1]=e;break;case"i32":HEAP32[A>>2]=e;break;case"i64":tempI64=[e>>>0,(tempDouble=e,+Math_abs(tempDouble)>=1?tempDouble>0?(0|Math_min(+Math_floor(tempDouble/4294967296),4294967295))>>>0:~~+Math_ceil((tempDouble-+(~~tempDouble>>>0))/4294967296)>>>0:0)],HEAP32[A>>2]=tempI64[0],HEAP32[A+4>>2]=tempI64[1];break;case"float":HEAPF32[A>>2]=e;break;case"double":HEAPF64[A>>3]=e;break;default:abort("invalid type for setValue: "+r)}}Module.wasmBinary&&(wasmBinary=Module.wasmBinary),Module.noExitRuntime&&(noExitRuntime=Module.noExitRuntime);var ABORT=!1,EXITSTATUS=0;function assert(A,e){A||abort("Assertion failed: "+e)}function getCFunc(A){var e=Module["_"+A];return assert(e,"Cannot call unknown function "+A+", make sure it is exported"),e}function ccall(A,e,r,i,n){var t={string:function(A){var e=0;if(null!=A&&0!==A){var r=1+(A.length<<2);stringToUTF8(A,e=stackAlloc(r),r)}return e},array:function(A){var e=stackAlloc(A.length);return writeArrayToMemory(A,e),e}};var f=getCFunc(A),a=[],B=0;if(i)for(var o=0;o<i.length;o++){var u=t[r[o]];u?(0===B&&(B=stackSave()),a[o]=u(i[o])):a[o]=i[o]}var l=f.apply(null,a);return l=function(A){return"string"===e?UTF8ToString(A):"boolean"===e?Boolean(A):A}(l),0!==B&&stackRestore(B),l}var ALLOC_NONE=3;function getMemory(A){return runtimeInitialized?_malloc(A):dynamicAlloc(A)}var UTF8Decoder="undefined"!=typeof TextDecoder?new TextDecoder("utf8"):void 0;function UTF8ArrayToString(A,e,r){for(var i=e+r,n=e;A[n]&&!(n>=i);)++n;if(n-e>16&&A.subarray&&UTF8Decoder)return UTF8Decoder.decode(A.subarray(e,n));for(var t="";e<n;){var f=A[e++];if(128&f){var a=63&A[e++];if(192!=(224&f)){var B=63&A[e++];if((f=224==(240&f)?(15&f)<<12|a<<6|B:(7&f)<<18|a<<12|B<<6|63&A[e++])<65536)t+=String.fromCharCode(f);else{var o=f-65536;t+=String.fromCharCode(55296|o>>10,56320|1023&o)}}else t+=String.fromCharCode((31&f)<<6|a)}else t+=String.fromCharCode(f)}return t}function UTF8ToString(A,e){return A?UTF8ArrayToString(HEAPU8,A,e):""}function stringToUTF8Array(A,e,r,i){if(!(i>0))return 0;for(var n=r,t=r+i-1,f=0;f<A.length;++f){var a=A.charCodeAt(f);if(a>=55296&&a<=57343)a=65536+((1023&a)<<10)|1023&A.charCodeAt(++f);if(a<=127){if(r>=t)break;e[r++]=a}else if(a<=2047){if(r+1>=t)break;e[r++]=192|a>>6,e[r++]=128|63&a}else if(a<=65535){if(r+2>=t)break;e[r++]=224|a>>12,e[r++]=128|a>>6&63,e[r++]=128|63&a}else{if(r+3>=t)break;e[r++]=240|a>>18,e[r++]=128|a>>12&63,e[r++]=128|a>>6&63,e[r++]=128|63&a}}return e[r]=0,r-n}function stringToUTF8(A,e,r){return stringToUTF8Array(A,HEAPU8,e,r)}function lengthBytesUTF8(A){for(var e=0,r=0;r<A.length;++r){var i=A.charCodeAt(r);i>=55296&&i<=57343&&(i=65536+((1023&i)<<10)|1023&A.charCodeAt(++r)),i<=127?++e:e+=i<=2047?2:i<=65535?3:4}return e}var UTF16Decoder="undefined"!=typeof TextDecoder?new TextDecoder("utf-16le"):void 0,buffer,HEAP8,HEAPU8,HEAP16,HEAPU16,HEAP32,HEAPU32,HEAPF32,HEAPF64;function allocateUTF8(A){var e=lengthBytesUTF8(A)+1,r=_malloc(e);return r&&stringToUTF8Array(A,HEAP8,r,e),r}function writeArrayToMemory(A,e){HEAP8.set(A,e)}function writeAsciiToMemory(A,e,r){for(var i=0;i<A.length;++i)HEAP8[e++>>0]=A.charCodeAt(i);r||(HEAP8[e>>0]=0)}function updateGlobalBufferAndViews(A){buffer=A,Module.HEAP8=HEAP8=new Int8Array(A),Module.HEAP16=HEAP16=new Int16Array(A),Module.HEAP32=HEAP32=new Int32Array(A),Module.HEAPU8=HEAPU8=new Uint8Array(A),Module.HEAPU16=HEAPU16=new Uint16Array(A),Module.HEAPU32=HEAPU32=new Uint32Array(A),Module.HEAPF32=HEAPF32=new Float32Array(A),Module.HEAPF64=HEAPF64=new Float64Array(A)}var STACK_BASE=83008,DYNAMIC_BASE=8471616,DYNAMICTOP_PTR=82816,INITIAL_INITIAL_MEMORY=Module.INITIAL_MEMORY||16777216;function callRuntimeCallbacks(A){for(;A.length>0;){var e=A.shift();if("function"!=typeof e){var r=e.func;"number"==typeof r?void 0===e.arg?Module.dynCall_v(r):Module.dynCall_vi(r,e.arg):r(void 0===e.arg?null:e.arg)}else e(Module)}}buffer=Module.buffer?Module.buffer:new ArrayBuffer(INITIAL_INITIAL_MEMORY),INITIAL_INITIAL_MEMORY=buffer.byteLength,updateGlobalBufferAndViews(buffer),HEAP32[DYNAMICTOP_PTR>>2]=DYNAMIC_BASE;var __ATPRERUN__=[],__ATINIT__=[],__ATMAIN__=[],__ATEXIT__=[],__ATPOSTRUN__=[],runtimeInitialized=!1,runtimeExited=!1;function preRun(){if(Module.preRun)for("function"==typeof Module.preRun&&(Module.preRun=[Module.preRun]);Module.preRun.length;)addOnPreRun(Module.preRun.shift());callRuntimeCallbacks(__ATPRERUN__)}function initRuntime(){runtimeInitialized=!0,callRuntimeCallbacks(__ATINIT__)}function preMain(){callRuntimeCallbacks(__ATMAIN__)}function exitRuntime(){runtimeExited=!0}function postRun(){if(Module.postRun)for("function"==typeof Module.postRun&&(Module.postRun=[Module.postRun]);Module.postRun.length;)addOnPostRun(Module.postRun.shift());callRuntimeCallbacks(__ATPOSTRUN__)}function addOnPreRun(A){__ATPRERUN__.unshift(A)}function addOnPostRun(A){__ATPOSTRUN__.unshift(A)}var Math_abs=Math.abs,Math_ceil=Math.ceil,Math_floor=Math.floor,Math_min=Math.min,runDependencies=0,runDependencyWatcher=null,dependenciesFulfilled=null;function addRunDependency(A){runDependencies++,Module.monitorRunDependencies&&Module.monitorRunDependencies(runDependencies)}function removeRunDependency(A){if(runDependencies--,Module.monitorRunDependencies&&Module.monitorRunDependencies(runDependencies),0==runDependencies&&(null!==runDependencyWatcher&&(clearInterval(runDependencyWatcher),runDependencyWatcher=null),dependenciesFulfilled)){var e=dependenciesFulfilled;dependenciesFulfilled=null,e()}}function abort(A){throw Module.onAbort&&Module.onAbort(A),out(A+=""),err(A),ABORT=!0,EXITSTATUS=1,A="abort("+A+"). Build with -s ASSERTIONS=1 for more info."}Module.preloadedImages={},Module.preloadedAudios={};var memoryInitializer=null;function hasPrefix(A,e){return String.prototype.startsWith?A.startsWith(e):0===A.indexOf(e)}var dataURIPrefix="data:application/octet-stream;base64,";function isDataURI(A){return hasPrefix(A,dataURIPrefix)}var fileURIPrefix="file://",tempDouble,tempI64,ASM_CONSTS=[function(){return Module.getRandomValue()},function(){if(void 0===Module.getRandomValue)try{var window_="object"==typeof window?window:self,crypto_=void 0!==window_.crypto?window_.crypto:window_.msCrypto,randomValuesStandard=function(){var A=new Uint32Array(1);return crypto_.getRandomValues(A),A[0]>>>0};randomValuesStandard(),Module.getRandomValue=randomValuesStandard}catch(e){try{var crypto=eval("require")("crypto"),randomValueNodeJS=function(){var A=crypto.randomBytes(4);return(A[0]<<24|A[1]<<16|A[2]<<8|A[3])>>>0};randomValueNodeJS(),Module.getRandomValue=randomValueNodeJS}catch(A){throw"No secure random number generator found"}}}];function _emscripten_asm_const_i(A){return ASM_CONSTS[A]()}__ATINIT__.push({func:function(){___emscripten_environ_constructor()}}),memoryInitializer="data:application/octet-stream;base64,AAAAAAAAAAAAAAAAAAAAAAAAAAB2OgEApMsAAAAAAAAAAAAABAAAAOrSAAAMywAAAAAAAAAAAAANAAAAAQAAAEAAAAAGKAAAAAAAAA4AAAACAAAAAgAAAAIAAAACAAAADwAAAAEAAACAAAAABigAAAAAAAAQAAAAAQAAAIAAAAAGKAAAAAAAABEAAAABAAAAQAAAAAYoAAAAAAAAEgAAAAEAAABAAAAABigAAAAAAAAwAAAAAQAAAIAAAAAQAAAAAgAAADEAAAABAAAA/////xYoAAAAAAAANgAAAAEAAAD/////FigAAAAAAAA3AAAAAQAAAP////8GKAAAAAAAAGMAAAABAAAAAIAAAAYoAAAAAAAAZAAAAAEAAAAAgAAABigAAAAAAABlAAAAAQAAAACAAAAGKAAAAAAAAGkAAAABAAAAQAAAAAIAAAACAAAAnAAAAP//////////AAgAAAIAAACtAAAAAQAAAACAAAAGKAAAAAAAAK4AAAD//////////wIAAAACAAAAhwEAAAEAAAD/////EAAAAAIAAAChAQAA//////////8ACAAAAgAAAMwBAAABAAAAAAEAABAAAAACAAAAvQMAAAIAAAACAAAAAgAAAAIAAADsAwAAAQAAAAwAAAABAAAAAgAAAO0DAAABAAAADQAAAAEAAAACAAAA7gMAAAEAAAALAAAAAQAAAAIAAABE0wAABAAAAAEAAABJ0wAABwAAAAEAAAAm+gAABAAAAAUAAABR0wAAAwAAAAIAAABV0wAABwAAAAIAAABd0wAABAAAAAoAAABi0wAACgAAAAoAAABt0wAAAwAAAAYAAABx0wAABgAAAAYAAAB40wAABwAAABcAAACA0wAAAwAAABcAAACE0wAADwAAABgAAACU0wAABwAAABgAAACc0wAAAwAAAAQAAACg0wAACwAAAAQAAACs0wAABgAAAAMAAACz0wAACQAAAAMAAAC90wAADwAAABwAAADN0wAABAAAABwAAADS0wAAAwAAABYAAADW0wAACQAAABYAAAA/0wAABAAAAAwAAADg0wAACgAAAAwAAADr0wAAAwAAAB4AAADv0wAACQAAAB4AAAD50wAADQAAABoAAAAH1AAABwAAABoAAAAP1AAADwAAABMAAAAf1AAACQAAABMAAAAp1AAAAwAAABQAAAAt1AAA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qQIAAKoCAACrAgAAoQEAAFgDAACGAQAAWwAAAM0DAAA7AQAAPAEAAD0BAAA+AQAAPwEAAEABAABBAQAAQgEAAG0BAABuAQAAbwEAAHABAABxAQAAcgEAAHMBAAB0AQAAdQEAAHYBAAB3AQAAmQMAAJoDAACbAwAAnAMAAJ0DAACeAwAAnwMAAKADAAChAwAAogMAAKMDAACkAwAApQMAAKYDAACoAwAArQMAAKIBAACjAQAApAEAAKUBAAAUAwAAfwMAAIADAACBAwAApgEAAKcBAACoAQAAqQEAABUDAACCAwAAgwMAAIQDAACqAQAAqwEAAKwBAACtAQAAFgMAAIUDAACGAwAAhwMAAKACAAChAgAAogIAAKMCAAAiAwAAIwMAAEcAAABIAAAASQAAAEoAAABLAAAATAAAAE0AAABOAAAATwAAAIsAAADKAQAAywEAAMwBAADNAQAAzgEAAM8BAADQAQAA0QEAANIBAADTAQAA1AEAANUBAADWAQAA1wEAANgBAADZAQAA2gEAANsBAADcAQAA3QEAAIcBAADeAQAA3wEAAOABAADhAQAA4gEAAOMBAADkAQAA5QEAAOYBAADnAQAA6AEAAOkBAADqAQAAZgAAAOsBAADsAQAA7QEAAO4BAADvAQAA8AEAAPEBAADyAQAA8wEAAPQBAAD1AQAA9gEAALoBAAC7AQAAvAEAAL0BAAC+AQAAvwEAAMABAADBAQAAiAEAAMIBAADDAQAAxAEAAMUBAADGAQAAxwEAAMgBAADJAQAAvQAAAL4AAAC/AAAAwAAAAMEAAADCAAAAwwAAAJ4AAACfAAAAoAAAAJAAAACRAAAAkgAAAJMAAACUAAAAlQAAAKsAAACGAAAAhwAAAIgAAACJAAAAigAAAIgCAACJAgAAtwMAALgDAAC5AwAAugMAAO8CAADwAgAA8QIAAIsDAACMAwAAjQMAAMQAAADFAAAAxgAAAMcAAADIAAAAyQAAAMoAAADLAAAAzAAAAM0AAADOAAAAzwAAANAAAADRAAAA0gAAANMAAAASAwAAIgQAACMEAAATAwAAJAQAANQAAADVAAAA1gAAANcAAADYAAAA2QAAANoAAADbAAAA3AAAAN0AAADeAAAA3wAAAOAAAADhAAAA4gAAAOMAAADkAAAA5QAAAOYAAADnAAAA6AAAAOkAAADqAAAA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ABAAEAAQABAAEAAQABAAEAAQABAAEAAQABAAEAAQABAAEAAQ/////////////////////////////////////////////////////////////////wABAgMEBQYHCAn/////////CgsMDQ4PEBESExQVFhcYGRobHB0eHyAhIiP///////8KCwwNDg8QERITFBUWFxgZGhscHR4fICEiI/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////8AAAAAAAAAAAAAAAAAAAAMBAAAAAAAAAEAAAAAAAAAAQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABAAAAAQAAAAIAAAD9AwAAAAAAAAIAAAAAAAAAAgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAADAAAAAgAAAAQAAAAAAAAAAgAAAAAAAAAAAAAAlMoAAAAAAADS0gAAAAAAAAAAAAABAAAAAQAAAAAAAAABAAAAAwAAAAEAAAAAAAAAAgAAAAAAAAAAAAAA0MoAAAEAAADZ0gAAAAAAAAAAAAACAAAAAQAAAAAAAAACAAAAAwAAAAEAAAABAAAAEAAAABAAAAACAAAAAAAAAAgAAADh0gAAAAAAAAQAAAAAAAAAAAAAAAAAAAAAAAAA+9IAAAAAAAACAAAAAAAAAAAAAAAAAAAAAAAAAA3TAAAAAAAAAwAAAAAAAAAAAAAAAAAAAAAAAAAa0wAAAAAAAAYAAAAAAAAAAAAAAAAAAAAAAAAAKtMAAAAAAAD8////AAAAAAAAAAAAAAAAAAAAADbTAABABQAAsAUAAGjMAAABAAAAEAAAAPAEAAACAAAAAAAAAAgAAAAY1QAAAAAAAAIAAAAAAAAAAAAAANzLAAD///9/M9UAAAAAAAAAAAAAAwAAAAIAAAACAAAAAwAAAAQAAAACAAAAVwMAAAAAAAADAAAABAAAAAMAAAAAAAAAAAAAAAAAAAAFAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABgAAAAUAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAYAAAAFAAAAVwMAAFcDAAAAAAAA3yABAHXVAAAAAAAAAAAAAAcAAAAAAAAAAAAAAAAAAAAAAAAABAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAQAAAAHAAAABgAAAAgAAAAAAAAAAAAAAEvWAAADAAAABAAAAAUAAAAGAAAACAAAAAQAAAAFAAAABgAAAAAEAAAAAAAAAAAAAAAAAAAAAAAAAQAAABAAAAAgBgAACQAAACzNAABYAAAAStoAAAAAAAAAAAAAAAAAAAAAAAAMAAAAAAAAAAEAAAAQAAAA4AYAAAIAAABgzQAAWAAAAEraAAAAAAAAAAAAAAAAAAAAAAAADAAAAAAAAAABAAAAEAAAABAHAAAEAAAAAAAAABAAAADd1wAABgAAAAIAAAAKAAAADwAAAAYAAAAAAAAAAAAAAAAAAAAQAAAAAAAAAAgAAAAAAAAACQAAAAAAAAAKAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAALAAAAAAAAAAwAAAAAAAAAAAAAAA0AAAAIAAAAqNgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAlBAAACQAAAAEAAAAQAAAAILUAAAIAAAAAAAAACAAAAEU6AQABAAAAEAAAAFC1AAACAAAAbM4AAAwAAABqOgEAAAAAAAAAAAAAAAAAAAAAAA4AAAAAAAAABAAAAAgAAAAQAAAAAAAAAAsAAAAKAAAAEQAAAAAAAAAAAAAAQAAAAGAAAAAAAAAAQAAAAEEAAAAUAAAACAAAAAwAAAALAAAAEgAAAAAAAAAAAAAAQAAAAGQAAAAPAAAA+M4AAAEAAAABAAAAAAAAAAIAAAABAAAAAQAAAP////+rOwEADQAAAA4AAAAPAAAAEAAAAAwAAAATAAAAFAAAABUAAAANAAAAInsgcmV0dXJuIE1vZHVsZS5nZXRSYW5kb21WYWx1ZSgpOyB9IgB7IGlmIChNb2R1bGUuZ2V0UmFuZG9tVmFsdWUgPT09IHVuZGVmaW5lZCkgeyB0cnkgeyB2YXIgd2luZG93XyA9ICdvYmplY3QnID09PSB0eXBlb2Ygd2luZG93ID8gd2luZG93IDogc2VsZjsgdmFyIGNyeXB0b18gPSB0eXBlb2Ygd2luZG93Xy5jcnlwdG8gIT09ICd1bmRlZmluZWQnID8gd2luZG93Xy5jcnlwdG8gOiB3aW5kb3dfLm1zQ3J5cHRvOyB2YXIgcmFuZG9tVmFsdWVzU3RhbmRhcmQgPSBmdW5jdGlvbigpIHsgdmFyIGJ1ZiA9IG5ldyBVaW50MzJBcnJheSgxKTsgY3J5cHRvXy5nZXRSYW5kb21WYWx1ZXMoYnVmKTsgcmV0dXJuIGJ1ZlswXSA+Pj4gMDsgfTsgcmFuZG9tVmFsdWVzU3RhbmRhcmQoKTsgTW9kdWxlLmdldFJhbmRvbVZhbHVlID0gcmFuZG9tVmFsdWVzU3RhbmRhcmQ7IH0gY2F0Y2ggKGUpIHsgdHJ5IHsgdmFyIGNyeXB0byA9IHJlcXVpcmUoJ2NyeXB0bycpOyB2YXIgcmFuZG9tVmFsdWVOb2RlSlMgPSBmdW5jdGlvbigpIHsgdmFyIGJ1ZiA9IGNyeXB0b1sncmFuZG9tQnl0ZXMnXSg0KTsgcmV0dXJuIChidWZbMF0gPDwgMjQgfCBidWZbMV0gPDwgMTYgfCBidWZbMl0gPDwgOCB8IGJ1ZlszXSkgPj4+IDA7IH07IHJhbmRvbVZhbHVlTm9kZUpTKCk7IE1vZHVsZS5nZXRSYW5kb21WYWx1ZSA9IHJhbmRvbVZhbHVlTm9kZUpTOyB9IGNhdGNoIChlKSB7IHRocm93ICdObyBzZWN1cmUgcmFuZG9tIG51bWJlciBnZW5lcmF0b3IgZm91bmQnOyB9IH0gfSB9AHNhbHQAaGV4c2FsdABoZXhpbmZvAG1kAHNlY3JldABoZXhzZWNyZXQAc2VlZABoZXhzZWVkAGFzc2VydGlvbiBmYWlsZWQ6IGNodW5rID49IDAAYXNzZXJ0aW9uIGZhaWxlZDogKih1bnNpZ25lZCBpbnQgKilsb2NrID09IDEAc3RibF9zZWN0aW9uAG1pbgBtYXgAbWFzawBmbGFncwBub21hc2sAZmllbGQ9AEJJR05VTQBDQklHTlVNAFg1MDlfU0lHAGRpZ2VzdAA8SU5WQUxJRD4AQVNOMV9PQ1RFVF9TVFJJTkcAQVNOMV9JTlRFR0VSAEFTTjFfQklUX1NUUklORwBBU04xX09CSkVDVABBU04xX0FOWQBVVEY4AEJPT0wAQk9PTEVBTgBJTlQASU5URUdFUgBFTlVNAEVOVU1FUkFURUQAT0lEAE9CSkVDVABVVENUSU1FAFVUQwBHRU5FUkFMSVpFRFRJTUUAR0VOVElNRQBPQ1QAT0NURVRTVFJJTkcAQklUU1RSAEJJVFNUUklORwBVTklWRVJTQUxTVFJJTkcAVU5JVgBJQTUASUE1U1RSSU5HAFVURjhTdHJpbmcAQk1QAEJNUFNUUklORwBWSVNJQkxFU1RSSU5HAFZJU0lCTEUAUFJJTlRBQkxFU1RSSU5HAFBSSU5UQUJMRQBUNjEAVDYxU1RSSU5HAFRFTEVURVhTVFJJTkcAR2VuZXJhbFN0cmluZwBHRU5TVFIATlVNRVJJQwBOVU1FUklDU1RSSU5HAFNFUVVFTkNFAFNFUQBTRVQARVhQAEVYUExJQ0lUAElNUABJTVBMSUNJVABPQ1RXUkFQAFNFUVdSQVAAU0VUV1JBUABCSVRXUkFQAEZPUk0ARk9STUFUAERJUgAlcyAwCgAlcyAlcyVsdSAoJXMweCVseCkKACAoTmVnYXRpdmUpACVzJXMKAG9pZF9zZWN0aW9uAEZpZWxkPQAsIFR5cGU9AFR5cGU9AFg1MDlfQUxHT1IAcGFyYW1ldGVyADAwAFwKAExPTkcAJWxkCgBrZXkAaGV4a2V5AGFzc2VydGlvbiBmYWlsZWQ6IGogPD0gKGludClzaXplb2YoY3R4LT5rZXkpAE9wZW5TU0wgSE1BQyBtZXRob2QAJTA0eCAtIAAlYwAgICAAJTAyeCVjACAgACAAPE5VTEw+ADAxMjM0NTY3ODkAMHgAMDEyMzQ1Njc4OWFiY2RlZgBhc3NlcnRpb24gZmFpbGVkOiAqc2J1ZmZlciAhPSBOVUxMIHx8IGJ1ZmZlciAhPSBOVUxMAGFzc2VydGlvbiBmYWlsZWQ6ICpjdXJybGVuIDw9ICptYXhsZW4AYXNzZXJ0aW9uIGZhaWxlZDogKnNidWZmZXIgIT0gTlVMTABOQQBPcGVuU1NMIFBLQ1MjMSBSU0EAT3BlblNTTCBSU0EgbWV0aG9kACAoSU5WQUxJRCBQU1MgUEFSQU1FVEVSUykKAEhhc2ggQWxnb3JpdGhtOiAAc2hhMSAoZGVmYXVsdCkATWFzayBBbGdvcml0aG06IAAgd2l0aCAASU5WQUxJRABtZ2YxIHdpdGggc2hhMSAoZGVmYXVsdCkAU2FsdCBMZW5ndGg6IDB4ADE0IChkZWZhdWx0KQBUcmFpbGVyIEZpZWxkOiAweABCQyAoZGVmYXVsdCkAUHJpdmF0ZS1LZXk6ICglZCBiaXQpCgBQdWJsaWMtS2V5OiAoJWQgYml0KQoAbW9kdWx1czoATW9kdWx1czoAcHVibGljRXhwb25lbnQ6AEV4cG9uZW50OgBwcml2YXRlRXhwb25lbnQ6AHByaW1lMToAcHJpbWUyOgBleHBvbmVudDE6AGV4cG9uZW50MjoAY29lZmZpY2llbnQ6AHZlcnNpb24AZQBkAHAAcQBkbXAxAGRtcTEAaXFtcABSU0FfUFNTX1BBUkFNUwBoYXNoQWxnb3JpdGhtAG1hc2tHZW5BbGdvcml0aG0Ac2FsdExlbmd0aAB0cmFpbGVyRmllbGQAcnNhX3BhZGRpbmdfbW9kZQBzc2x2MjMAbm9uZQBvZWFwAG9hZXAAeDkzMQByc2FfcHNzX3NhbHRsZW4AcnNhX2tleWdlbl9iaXRzAHJzYV9rZXlnZW5fcHViZXhwAHJzYV9tZ2YxX21kAHJzYV9vYWVwX21kAHJzYV9vYWVwX2xhYmVsAE5VTEwgc2hhcmVkIGxpYnJhcnkgbWV0aG9kAFVOREVGAHVuZGVmaW5lZAByc2Fkc2kAUlNBIERhdGEgU2VjdXJpdHksIEluYy4AcGtjcwBSU0EgRGF0YSBTZWN1cml0eSwgSW5jLiBQS0NTAE1EMgBtZDIATUQ1AG1kNQBSQzQAcmM0AHJzYUVuY3J5cHRpb24AUlNBLU1EMgBtZDJXaXRoUlNBRW5jcnlwdGlvbgBSU0EtTUQ1AG1kNVdpdGhSU0FFbmNyeXB0aW9uAFBCRS1NRDItREVTAHBiZVdpdGhNRDJBbmRERVMtQ0JDAFBCRS1NRDUtREVTAHBiZVdpdGhNRDVBbmRERVMtQ0JDAFg1MDAAZGlyZWN0b3J5IHNlcnZpY2VzIChYLjUwMCkAWDUwOQBDTgBjb21tb25OYW1lAEMAY291bnRyeU5hbWUATABsb2NhbGl0eU5hbWUAU1QAc3RhdGVPclByb3ZpbmNlTmFtZQBPAG9yZ2FuaXphdGlvbk5hbWUAT1UAb3JnYW5pemF0aW9uYWxVbml0TmFtZQBSU0EAcnNhAHBrY3M3AHBrY3M3LWRhdGEAcGtjczctc2lnbmVkRGF0YQBwa2NzNy1lbnZlbG9wZWREYXRhAHBrY3M3LXNpZ25lZEFuZEVudmVsb3BlZERhdGEAcGtjczctZGlnZXN0RGF0YQBwa2NzNy1lbmNyeXB0ZWREYXRhAHBrY3MzAGRoS2V5QWdyZWVtZW50AERFUy1FQ0IAZGVzLWVjYgBERVMtQ0ZCAGRlcy1jZmIAREVTLUNCQwBkZXMtY2JjAERFUy1FREUAZGVzLWVkZQBERVMtRURFMwBkZXMtZWRlMwBJREVBLUNCQwBpZGVhLWNiYwBJREVBLUNGQgBpZGVhLWNmYgBJREVBLUVDQgBpZGVhLWVjYgBSQzItQ0JDAHJjMi1jYmMAUkMyLUVDQgByYzItZWNiAFJDMi1DRkIAcmMyLWNmYgBSQzItT0ZCAHJjMi1vZmIAU0hBAHNoYQBSU0EtU0hBAHNoYVdpdGhSU0FFbmNyeXB0aW9uAERFUy1FREUtQ0JDAGRlcy1lZGUtY2JjAERFUy1FREUzLUNCQwBkZXMtZWRlMy1jYmMAREVTLU9GQgBkZXMtb2ZiAElERUEtT0ZCAGlkZWEtb2ZiAHBrY3M5AGVtYWlsQWRkcmVzcwB1bnN0cnVjdHVyZWROYW1lAGNvbnRlbnRUeXBlAG1lc3NhZ2VEaWdlc3QAc2lnbmluZ1RpbWUAY291bnRlcnNpZ25hdHVyZQBjaGFsbGVuZ2VQYXNzd29yZAB1bnN0cnVjdHVyZWRBZGRyZXNzAGV4dGVuZGVkQ2VydGlmaWNhdGVBdHRyaWJ1dGVzAE5ldHNjYXBlAE5ldHNjYXBlIENvbW11bmljYXRpb25zIENvcnAuAG5zQ2VydEV4dABOZXRzY2FwZSBDZXJ0aWZpY2F0ZSBFeHRlbnNpb24AbnNEYXRhVHlwZQBOZXRzY2FwZSBEYXRhIFR5cGUAREVTLUVERS1DRkIAZGVzLWVkZS1jZmIAREVTLUVERTMtQ0ZCAGRlcy1lZGUzLWNmYgBERVMtRURFLU9GQgBkZXMtZWRlLW9mYgBERVMtRURFMy1PRkIAZGVzLWVkZTMtb2ZiAFNIQTEAc2hhMQBSU0EtU0hBMQBzaGExV2l0aFJTQUVuY3J5cHRpb24ARFNBLVNIQQBkc2FXaXRoU0hBAERTQS1vbGQAZHNhRW5jcnlwdGlvbi1vbGQAUEJFLVNIQTEtUkMyLTY0AHBiZVdpdGhTSEExQW5kUkMyLUNCQwBQQktERjIARFNBLVNIQTEtb2xkAGRzYVdpdGhTSEExLW9sZABuc0NlcnRUeXBlAE5ldHNjYXBlIENlcnQgVHlwZQBuc0Jhc2VVcmwATmV0c2NhcGUgQmFzZSBVcmwAbnNSZXZvY2F0aW9uVXJsAE5ldHNjYXBlIFJldm9jYXRpb24gVXJsAG5zQ2FSZXZvY2F0aW9uVXJsAE5ldHNjYXBlIENBIFJldm9jYXRpb24gVXJsAG5zUmVuZXdhbFVybABOZXRzY2FwZSBSZW5ld2FsIFVybABuc0NhUG9saWN5VXJsAE5ldHNjYXBlIENBIFBvbGljeSBVcmwAbnNTc2xTZXJ2ZXJOYW1lAE5ldHNjYXBlIFNTTCBTZXJ2ZXIgTmFtZQBuc0NvbW1lbnQATmV0c2NhcGUgQ29tbWVudABuc0NlcnRTZXF1ZW5jZQBOZXRzY2FwZSBDZXJ0aWZpY2F0ZSBTZXF1ZW5jZQBERVNYLUNCQwBkZXN4LWNiYwBpZC1jZQBzdWJqZWN0S2V5SWRlbnRpZmllcgBYNTA5djMgU3ViamVjdCBLZXkgSWRlbnRpZmllcgBrZXlVc2FnZQBYNTA5djMgS2V5IFVzYWdlAHByaXZhdGVLZXlVc2FnZVBlcmlvZABYNTA5djMgUHJpdmF0ZSBLZXkgVXNhZ2UgUGVyaW9kAHN1YmplY3RBbHROYW1lAFg1MDl2MyBTdWJqZWN0IEFsdGVybmF0aXZlIE5hbWUAaXNzdWVyQWx0TmFtZQBYNTA5djMgSXNzdWVyIEFsdGVybmF0aXZlIE5hbWUAYmFzaWNDb25zdHJhaW50cwBYNTA5djMgQmFzaWMgQ29uc3RyYWludHMAY3JsTnVtYmVyAFg1MDl2MyBDUkwgTnVtYmVyAGNlcnRpZmljYXRlUG9saWNpZXMAWDUwOXYzIENlcnRpZmljYXRlIFBvbGljaWVzAGF1dGhvcml0eUtleUlkZW50aWZpZXIAWDUwOXYzIEF1dGhvcml0eSBLZXkgSWRlbnRpZmllcgBCRi1DQkMAYmYtY2JjAEJGLUVDQgBiZi1lY2IAQkYtQ0ZCAGJmLWNmYgBCRi1PRkIAYmYtb2ZiAE1EQzIAbWRjMgBSU0EtTURDMgBtZGMyV2l0aFJTQQBSQzQtNDAAcmM0LTQwAFJDMi00MC1DQkMAcmMyLTQwLWNiYwBHTgBnaXZlbk5hbWUAU04Ac3VybmFtZQBpbml0aWFscwB1aWQAdW5pcXVlSWRlbnRpZmllcgBjcmxEaXN0cmlidXRpb25Qb2ludHMAWDUwOXYzIENSTCBEaXN0cmlidXRpb24gUG9pbnRzAFJTQS1OUC1NRDUAbWQ1V2l0aFJTQQBzZXJpYWxOdW1iZXIAdGl0bGUAZGVzY3JpcHRpb24AQ0FTVDUtQ0JDAGNhc3Q1LWNiYwBDQVNUNS1FQ0IAY2FzdDUtZWNiAENBU1Q1LUNGQgBjYXN0NS1jZmIAQ0FTVDUtT0ZCAGNhc3Q1LW9mYgBwYmVXaXRoTUQ1QW5kQ2FzdDVDQkMARFNBLVNIQTEAZHNhV2l0aFNIQTEATUQ1LVNIQTEAbWQ1LXNoYTEAUlNBLVNIQTEtMgBzaGExV2l0aFJTQQBEU0EAZHNhRW5jcnlwdGlvbgBSSVBFTUQxNjAAcmlwZW1kMTYwAFJTQS1SSVBFTUQxNjAAcmlwZW1kMTYwV2l0aFJTQQBSQzUtQ0JDAHJjNS1jYmMAUkM1LUVDQgByYzUtZWNiAFJDNS1DRkIAcmM1LWNmYgBSQzUtT0ZCAHJjNS1vZmIAWkxJQgB6bGliIGNvbXByZXNzaW9uAGV4dGVuZGVkS2V5VXNhZ2UAWDUwOXYzIEV4dGVuZGVkIEtleSBVc2FnZQBQS0lYAGlkLWtwAHNlcnZlckF1dGgAVExTIFdlYiBTZXJ2ZXIgQXV0aGVudGljYXRpb24AY2xpZW50QXV0aABUTFMgV2ViIENsaWVudCBBdXRoZW50aWNhdGlvbgBjb2RlU2lnbmluZwBDb2RlIFNpZ25pbmcAZW1haWxQcm90ZWN0aW9uAEUtbWFpbCBQcm90ZWN0aW9uAHRpbWVTdGFtcGluZwBUaW1lIFN0YW1waW5nAG1zQ29kZUluZABNaWNyb3NvZnQgSW5kaXZpZHVhbCBDb2RlIFNpZ25pbmcAbXNDb2RlQ29tAE1pY3Jvc29mdCBDb21tZXJjaWFsIENvZGUgU2lnbmluZwBtc0NUTFNpZ24ATWljcm9zb2Z0IFRydXN0IExpc3QgU2lnbmluZwBtc1NHQwBNaWNyb3NvZnQgU2VydmVyIEdhdGVkIENyeXB0bwBtc0VGUwBNaWNyb3NvZnQgRW5jcnlwdGVkIEZpbGUgU3lzdGVtAG5zU0dDAE5ldHNjYXBlIFNlcnZlciBHYXRlZCBDcnlwdG8AZGVsdGFDUkwAWDUwOXYzIERlbHRhIENSTCBJbmRpY2F0b3IAQ1JMUmVhc29uAFg1MDl2MyBDUkwgUmVhc29uIENvZGUAaW52YWxpZGl0eURhdGUASW52YWxpZGl0eSBEYXRlAFNYTmV0SUQAU3Ryb25nIEV4dHJhbmV0IElEAFBCRS1TSEExLVJDNC0xMjgAcGJlV2l0aFNIQTFBbmQxMjhCaXRSQzQAUEJFLVNIQTEtUkM0LTQwAHBiZVdpdGhTSEExQW5kNDBCaXRSQzQAUEJFLVNIQTEtM0RFUwBwYmVXaXRoU0hBMUFuZDMtS2V5VHJpcGxlREVTLUNCQwBQQkUtU0hBMS0yREVTAHBiZVdpdGhTSEExQW5kMi1LZXlUcmlwbGVERVMtQ0JDAFBCRS1TSEExLVJDMi0xMjgAcGJlV2l0aFNIQTFBbmQxMjhCaXRSQzItQ0JDAFBCRS1TSEExLVJDMi00MABwYmVXaXRoU0hBMUFuZDQwQml0UkMyLUNCQwBrZXlCYWcAcGtjczhTaHJvdWRlZEtleUJhZwBjZXJ0QmFnAGNybEJhZwBzZWNyZXRCYWcAc2FmZUNvbnRlbnRzQmFnAGZyaWVuZGx5TmFtZQBsb2NhbEtleUlEAHg1MDlDZXJ0aWZpY2F0ZQBzZHNpQ2VydGlmaWNhdGUAeDUwOUNybABQQkVTMgBQQk1BQzEAaG1hY1dpdGhTSEExAGlkLXF0LWNwcwBQb2xpY3kgUXVhbGlmaWVyIENQUwBpZC1xdC11bm90aWNlAFBvbGljeSBRdWFsaWZpZXIgVXNlciBOb3RpY2UAUkMyLTY0LUNCQwByYzItNjQtY2JjAFNNSU1FLUNBUFMAUy9NSU1FIENhcGFiaWxpdGllcwBQQkUtTUQyLVJDMi02NABwYmVXaXRoTUQyQW5kUkMyLUNCQwBQQkUtTUQ1LVJDMi02NABwYmVXaXRoTUQ1QW5kUkMyLUNCQwBQQkUtU0hBMS1ERVMAcGJlV2l0aFNIQTFBbmRERVMtQ0JDAG1zRXh0UmVxAE1pY3Jvc29mdCBFeHRlbnNpb24gUmVxdWVzdABleHRSZXEARXh0ZW5zaW9uIFJlcXVlc3QAbmFtZQBkblF1YWxpZmllcgBpZC1wZQBpZC1hZABhdXRob3JpdHlJbmZvQWNjZXNzAEF1dGhvcml0eSBJbmZvcm1hdGlvbiBBY2Nlc3MAT0NTUABjYUlzc3VlcnMAQ0EgSXNzdWVycwBPQ1NQU2lnbmluZwBPQ1NQIFNpZ25pbmcASVNPAGlzbwBtZW1iZXItYm9keQBJU08gTWVtYmVyIEJvZHkASVNPLVVTAElTTyBVUyBNZW1iZXIgQm9keQBYOS01NwBYOS41NwBYOWNtAFg5LjU3IENNID8AcGtjczEAcGtjczUAU01JTUUAUy9NSU1FAGlkLXNtaW1lLW1vZABpZC1zbWltZS1jdABpZC1zbWltZS1hYQBpZC1zbWltZS1hbGcAaWQtc21pbWUtY2QAaWQtc21pbWUtc3BxAGlkLXNtaW1lLWN0aQBpZC1zbWltZS1tb2QtY21zAGlkLXNtaW1lLW1vZC1lc3MAaWQtc21pbWUtbW9kLW9pZABpZC1zbWltZS1tb2QtbXNnLXYzAGlkLXNtaW1lLW1vZC1ldHMtZVNpZ25hdHVyZS04OABpZC1zbWltZS1tb2QtZXRzLWVTaWduYXR1cmUtOTcAaWQtc21pbWUtbW9kLWV0cy1lU2lnUG9saWN5LTg4AGlkLXNtaW1lLW1vZC1ldHMtZVNpZ1BvbGljeS05NwBpZC1zbWltZS1jdC1yZWNlaXB0AGlkLXNtaW1lLWN0LWF1dGhEYXRhAGlkLXNtaW1lLWN0LXB1Ymxpc2hDZXJ0AGlkLXNtaW1lLWN0LVRTVEluZm8AaWQtc21pbWUtY3QtVERUSW5mbwBpZC1zbWltZS1jdC1jb250ZW50SW5mbwBpZC1zbWltZS1jdC1EVkNTUmVxdWVzdERhdGEAaWQtc21pbWUtY3QtRFZDU1Jlc3BvbnNlRGF0YQBpZC1zbWltZS1hYS1yZWNlaXB0UmVxdWVzdABpZC1zbWltZS1hYS1zZWN1cml0eUxhYmVsAGlkLXNtaW1lLWFhLW1sRXhwYW5kSGlzdG9yeQBpZC1zbWltZS1hYS1jb250ZW50SGludABpZC1zbWltZS1hYS1tc2dTaWdEaWdlc3QAaWQtc21pbWUtYWEtZW5jYXBDb250ZW50VHlwZQBpZC1zbWltZS1hYS1jb250ZW50SWRlbnRpZmllcgBpZC1zbWltZS1hYS1tYWNWYWx1ZQBpZC1zbWltZS1hYS1lcXVpdmFsZW50TGFiZWxzAGlkLXNtaW1lLWFhLWNvbnRlbnRSZWZlcmVuY2UAaWQtc21pbWUtYWEtZW5jcnlwS2V5UHJlZgBpZC1zbWltZS1hYS1zaWduaW5nQ2VydGlmaWNhdGUAaWQtc21pbWUtYWEtc21pbWVFbmNyeXB0Q2VydHMAaWQtc21pbWUtYWEtdGltZVN0YW1wVG9rZW4AaWQtc21pbWUtYWEtZXRzLXNpZ1BvbGljeUlkAGlkLXNtaW1lLWFhLWV0cy1jb21taXRtZW50VHlwZQBpZC1zbWltZS1hYS1ldHMtc2lnbmVyTG9jYXRpb24AaWQtc21pbWUtYWEtZXRzLXNpZ25lckF0dHIAaWQtc21pbWUtYWEtZXRzLW90aGVyU2lnQ2VydABpZC1zbWltZS1hYS1ldHMtY29udGVudFRpbWVzdGFtcABpZC1zbWltZS1hYS1ldHMtQ2VydGlmaWNhdGVSZWZzAGlkLXNtaW1lLWFhLWV0cy1SZXZvY2F0aW9uUmVmcwBpZC1zbWltZS1hYS1ldHMtY2VydFZhbHVlcwBpZC1zbWltZS1hYS1ldHMtcmV2b2NhdGlvblZhbHVlcwBpZC1zbWltZS1hYS1ldHMtZXNjVGltZVN0YW1wAGlkLXNtaW1lLWFhLWV0cy1jZXJ0Q1JMVGltZXN0YW1wAGlkLXNtaW1lLWFhLWV0cy1hcmNoaXZlVGltZVN0YW1wAGlkLXNtaW1lLWFhLXNpZ25hdHVyZVR5cGUAaWQtc21pbWUtYWEtZHZjcy1kdmMAaWQtc21pbWUtYWxnLUVTREh3aXRoM0RFUwBpZC1zbWltZS1hbGctRVNESHdpdGhSQzIAaWQtc21pbWUtYWxnLTNERVN3cmFwAGlkLXNtaW1lLWFsZy1SQzJ3cmFwAGlkLXNtaW1lLWFsZy1FU0RIAGlkLXNtaW1lLWFsZy1DTVMzREVTd3JhcABpZC1zbWltZS1hbGctQ01TUkMyd3JhcABpZC1zbWltZS1jZC1sZGFwAGlkLXNtaW1lLXNwcS1ldHMtc3F0LXVyaQBpZC1zbWltZS1zcHEtZXRzLXNxdC11bm90aWNlAGlkLXNtaW1lLWN0aS1ldHMtcHJvb2ZPZk9yaWdpbgBpZC1zbWltZS1jdGktZXRzLXByb29mT2ZSZWNlaXB0AGlkLXNtaW1lLWN0aS1ldHMtcHJvb2ZPZkRlbGl2ZXJ5AGlkLXNtaW1lLWN0aS1ldHMtcHJvb2ZPZlNlbmRlcgBpZC1zbWltZS1jdGktZXRzLXByb29mT2ZBcHByb3ZhbABpZC1zbWltZS1jdGktZXRzLXByb29mT2ZDcmVhdGlvbgBNRDQAbWQ0AGlkLXBraXgtbW9kAGlkLXF0AGlkLWl0AGlkLXBraXAAaWQtYWxnAGlkLWNtYwBpZC1vbgBpZC1wZGEAaWQtYWNhAGlkLXFjcwBpZC1jY3QAaWQtcGtpeDEtZXhwbGljaXQtODgAaWQtcGtpeDEtaW1wbGljaXQtODgAaWQtcGtpeDEtZXhwbGljaXQtOTMAaWQtcGtpeDEtaW1wbGljaXQtOTMAaWQtbW9kLWNybWYAaWQtbW9kLWNtYwBpZC1tb2Qta2VhLXByb2ZpbGUtODgAaWQtbW9kLWtlYS1wcm9maWxlLTkzAGlkLW1vZC1jbXAAaWQtbW9kLXF1YWxpZmllZC1jZXJ0LTg4AGlkLW1vZC1xdWFsaWZpZWQtY2VydC05MwBpZC1tb2QtYXR0cmlidXRlLWNlcnQAaWQtbW9kLXRpbWVzdGFtcC1wcm90b2NvbABpZC1tb2Qtb2NzcABpZC1tb2QtZHZjcwBpZC1tb2QtY21wMjAwMABiaW9tZXRyaWNJbmZvAEJpb21ldHJpYyBJbmZvAHFjU3RhdGVtZW50cwBhYy1hdWRpdEVudGl0eQBhYy10YXJnZXRpbmcAYWFDb250cm9scwBzYmdwLWlwQWRkckJsb2NrAHNiZ3AtYXV0b25vbW91c1N5c051bQBzYmdwLXJvdXRlcklkZW50aWZpZXIAdGV4dE5vdGljZQBpcHNlY0VuZFN5c3RlbQBJUFNlYyBFbmQgU3lzdGVtAGlwc2VjVHVubmVsAElQU2VjIFR1bm5lbABpcHNlY1VzZXIASVBTZWMgVXNlcgBEVkNTAGR2Y3MAaWQtaXQtY2FQcm90RW5jQ2VydABpZC1pdC1zaWduS2V5UGFpclR5cGVzAGlkLWl0LWVuY0tleVBhaXJUeXBlcwBpZC1pdC1wcmVmZXJyZWRTeW1tQWxnAGlkLWl0LWNhS2V5VXBkYXRlSW5mbwBpZC1pdC1jdXJyZW50Q1JMAGlkLWl0LXVuc3VwcG9ydGVkT0lEcwBpZC1pdC1zdWJzY3JpcHRpb25SZXF1ZXN0AGlkLWl0LXN1YnNjcmlwdGlvblJlc3BvbnNlAGlkLWl0LWtleVBhaXJQYXJhbVJlcQBpZC1pdC1rZXlQYWlyUGFyYW1SZXAAaWQtaXQtcmV2UGFzc3BocmFzZQBpZC1pdC1pbXBsaWNpdENvbmZpcm0AaWQtaXQtY29uZmlybVdhaXRUaW1lAGlkLWl0LW9yaWdQS0lNZXNzYWdlAGlkLXJlZ0N0cmwAaWQtcmVnSW5mbwBpZC1yZWdDdHJsLXJlZ1Rva2VuAGlkLXJlZ0N0cmwtYXV0aGVudGljYXRvcgBpZC1yZWdDdHJsLXBraVB1YmxpY2F0aW9uSW5mbwBpZC1yZWdDdHJsLXBraUFyY2hpdmVPcHRpb25zAGlkLXJlZ0N0cmwtb2xkQ2VydElEAGlkLXJlZ0N0cmwtcHJvdG9jb2xFbmNyS2V5AGlkLXJlZ0luZm8tdXRmOFBhaXJzAGlkLXJlZ0luZm8tY2VydFJlcQBpZC1hbGctZGVzNDAAaWQtYWxnLW5vU2lnbmF0dXJlAGlkLWFsZy1kaC1zaWctaG1hYy1zaGExAGlkLWFsZy1kaC1wb3AAaWQtY21jLXN0YXR1c0luZm8AaWQtY21jLWlkZW50aWZpY2F0aW9uAGlkLWNtYy1pZGVudGl0eVByb29mAGlkLWNtYy1kYXRhUmV0dXJuAGlkLWNtYy10cmFuc2FjdGlvbklkAGlkLWNtYy1zZW5kZXJOb25jZQBpZC1jbWMtcmVjaXBpZW50Tm9uY2UAaWQtY21jLWFkZEV4dGVuc2lvbnMAaWQtY21jLWVuY3J5cHRlZFBPUABpZC1jbWMtZGVjcnlwdGVkUE9QAGlkLWNtYy1scmFQT1BXaXRuZXNzAGlkLWNtYy1nZXRDZXJ0AGlkLWNtYy1nZXRDUkwAaWQtY21jLXJldm9rZVJlcXVlc3QAaWQtY21jLXJlZ0luZm8AaWQtY21jLXJlc3BvbnNlSW5mbwBpZC1jbWMtcXVlcnlQZW5kaW5nAGlkLWNtYy1wb3BMaW5rUmFuZG9tAGlkLWNtYy1wb3BMaW5rV2l0bmVzcwBpZC1jbWMtY29uZmlybUNlcnRBY2NlcHRhbmNlAGlkLW9uLXBlcnNvbmFsRGF0YQBpZC1wZGEtZGF0ZU9mQmlydGgAaWQtcGRhLXBsYWNlT2ZCaXJ0aABpZC1wZGEtZ2VuZGVyAGlkLXBkYS1jb3VudHJ5T2ZDaXRpemVuc2hpcABpZC1wZGEtY291bnRyeU9mUmVzaWRlbmNlAGlkLWFjYS1hdXRoZW50aWNhdGlvbkluZm8AaWQtYWNhLWFjY2Vzc0lkZW50aXR5AGlkLWFjYS1jaGFyZ2luZ0lkZW50aXR5AGlkLWFjYS1ncm91cABpZC1hY2Etcm9sZQBpZC1xY3MtcGtpeFFDU3ludGF4LXYxAGlkLWNjdC1jcnMAaWQtY2N0LVBLSURhdGEAaWQtY2N0LVBLSVJlc3BvbnNlAGFkX3RpbWVzdGFtcGluZwBBRCBUaW1lIFN0YW1waW5nAEFEX0RWQ1MAYWQgZHZjcwBiYXNpY09DU1BSZXNwb25zZQBCYXNpYyBPQ1NQIFJlc3BvbnNlAE5vbmNlAE9DU1AgTm9uY2UAQ3JsSUQAT0NTUCBDUkwgSUQAYWNjZXB0YWJsZVJlc3BvbnNlcwBBY2NlcHRhYmxlIE9DU1AgUmVzcG9uc2VzAG5vQ2hlY2sAT0NTUCBObyBDaGVjawBhcmNoaXZlQ3V0b2ZmAE9DU1AgQXJjaGl2ZSBDdXRvZmYAc2VydmljZUxvY2F0b3IAT0NTUCBTZXJ2aWNlIExvY2F0b3IAZXh0ZW5kZWRTdGF0dXMARXh0ZW5kZWQgT0NTUCBTdGF0dXMAdmFsaWQAdHJ1c3RSb290AFRydXN0IFJvb3QAYWxnb3JpdGhtAHJzYVNpZ25hdHVyZQBYNTAwYWxnb3JpdGhtcwBkaXJlY3Rvcnkgc2VydmljZXMgLSBhbGdvcml0aG1zAE9SRwBvcmcARE9EAGRvZABJQU5BAGlhbmEAZGlyZWN0b3J5AERpcmVjdG9yeQBtZ210AE1hbmFnZW1lbnQAZXhwZXJpbWVudGFsAEV4cGVyaW1lbnRhbABwcml2YXRlAFByaXZhdGUAc2VjdXJpdHkAU2VjdXJpdHkAc25tcHYyAFNOTVB2MgBNYWlsAGVudGVycHJpc2VzAEVudGVycHJpc2VzAGRjb2JqZWN0AGRjT2JqZWN0AERDAGRvbWFpbkNvbXBvbmVudABkb21haW4ARG9tYWluAE5VTEwAc2VsZWN0ZWQtYXR0cmlidXRlLXR5cGVzAFNlbGVjdGVkIEF0dHJpYnV0ZSBUeXBlcwBjbGVhcmFuY2UAUlNBLU1ENABtZDRXaXRoUlNBRW5jcnlwdGlvbgBhYy1wcm94eWluZwBzdWJqZWN0SW5mb0FjY2VzcwBTdWJqZWN0IEluZm9ybWF0aW9uIEFjY2VzcwBpZC1hY2EtZW5jQXR0cnMAcm9sZQBwb2xpY3lDb25zdHJhaW50cwBYNTA5djMgUG9saWN5IENvbnN0cmFpbnRzAHRhcmdldEluZm9ybWF0aW9uAFg1MDl2MyBBQyBUYXJnZXRpbmcAbm9SZXZBdmFpbABYNTA5djMgTm8gUmV2b2NhdGlvbiBBdmFpbGFibGUAYW5zaS1YOS02MgBBTlNJIFg5LjYyAHByaW1lLWZpZWxkAGNoYXJhY3RlcmlzdGljLXR3by1maWVsZABpZC1lY1B1YmxpY0tleQBwcmltZTE5MnYxAHByaW1lMTkydjIAcHJpbWUxOTJ2MwBwcmltZTIzOXYxAHByaW1lMjM5djIAcHJpbWUyMzl2MwBwcmltZTI1NnYxAGVjZHNhLXdpdGgtU0hBMQBDU1BOYW1lAE1pY3Jvc29mdCBDU1AgTmFtZQBBRVMtMTI4LUVDQgBhZXMtMTI4LWVjYgBBRVMtMTI4LUNCQwBhZXMtMTI4LWNiYwBBRVMtMTI4LU9GQgBhZXMtMTI4LW9mYgBBRVMtMTI4LUNGQgBhZXMtMTI4LWNmYgBBRVMtMTkyLUVDQgBhZXMtMTkyLWVjYgBBRVMtMTkyLUNCQwBhZXMtMTkyLWNiYwBBRVMtMTkyLU9GQgBhZXMtMTkyLW9mYgBBRVMtMTkyLUNGQgBhZXMtMTkyLWNmYgBBRVMtMjU2LUVDQgBhZXMtMjU2LWVjYgBBRVMtMjU2LUNCQwBhZXMtMjU2LWNiYwBBRVMtMjU2LU9GQgBhZXMtMjU2LW9mYgBBRVMtMjU2LUNGQgBhZXMtMjU2LWNmYgBob2xkSW5zdHJ1Y3Rpb25Db2RlAEhvbGQgSW5zdHJ1Y3Rpb24gQ29kZQBob2xkSW5zdHJ1Y3Rpb25Ob25lAEhvbGQgSW5zdHJ1Y3Rpb24gTm9uZQBob2xkSW5zdHJ1Y3Rpb25DYWxsSXNzdWVyAEhvbGQgSW5zdHJ1Y3Rpb24gQ2FsbCBJc3N1ZXIAaG9sZEluc3RydWN0aW9uUmVqZWN0AEhvbGQgSW5zdHJ1Y3Rpb24gUmVqZWN0AGRhdGEAcHNzAHVjbABwaWxvdABwaWxvdEF0dHJpYnV0ZVR5cGUAcGlsb3RBdHRyaWJ1dGVTeW50YXgAcGlsb3RPYmplY3RDbGFzcwBwaWxvdEdyb3VwcwBpQTVTdHJpbmdTeW50YXgAY2FzZUlnbm9yZUlBNVN0cmluZ1N5bnRheABwaWxvdE9iamVjdABwaWxvdFBlcnNvbgBhY2NvdW50AGRvY3VtZW50AHJvb20AZG9jdW1lbnRTZXJpZXMAckZDODIybG9jYWxQYXJ0AGROU0RvbWFpbgBkb21haW5SZWxhdGVkT2JqZWN0AGZyaWVuZGx5Q291bnRyeQBzaW1wbGVTZWN1cml0eU9iamVjdABwaWxvdE9yZ2FuaXphdGlvbgBwaWxvdERTQQBxdWFsaXR5TGFiZWxsZWREYXRhAFVJRAB1c2VySWQAdGV4dEVuY29kZWRPUkFkZHJlc3MAbWFpbAByZmM4MjJNYWlsYm94AGluZm8AZmF2b3VyaXRlRHJpbmsAcm9vbU51bWJlcgBwaG90bwB1c2VyQ2xhc3MAaG9zdABtYW5hZ2VyAGRvY3VtZW50SWRlbnRpZmllcgBkb2N1bWVudFRpdGxlAGRvY3VtZW50VmVyc2lvbgBkb2N1bWVudEF1dGhvcgBkb2N1bWVudExvY2F0aW9uAGhvbWVUZWxlcGhvbmVOdW1iZXIAc2VjcmV0YXJ5AG90aGVyTWFpbGJveABsYXN0TW9kaWZpZWRUaW1lAGxhc3RNb2RpZmllZEJ5AGFSZWNvcmQAcGlsb3RBdHRyaWJ1dGVUeXBlMjcAbVhSZWNvcmQAblNSZWNvcmQAc09BUmVjb3JkAGNOQU1FUmVjb3JkAGFzc29jaWF0ZWREb21haW4AYXNzb2NpYXRlZE5hbWUAaG9tZVBvc3RhbEFkZHJlc3MAcGVyc29uYWxUaXRsZQBtb2JpbGVUZWxlcGhvbmVOdW1iZXIAcGFnZXJUZWxlcGhvbmVOdW1iZXIAZnJpZW5kbHlDb3VudHJ5TmFtZQBvcmdhbml6YXRpb25hbFN0YXR1cwBqYW5ldE1haWxib3gAbWFpbFByZWZlcmVuY2VPcHRpb24AYnVpbGRpbmdOYW1lAGRTQVF1YWxpdHkAc2luZ2xlTGV2ZWxRdWFsaXR5AHN1YnRyZWVNaW5pbXVtUXVhbGl0eQBzdWJ0cmVlTWF4aW11bVF1YWxpdHkAcGVyc29uYWxTaWduYXR1cmUAZElUUmVkaXJlY3QAYXVkaW8AZG9jdW1lbnRQdWJsaXNoZXIAeDUwMFVuaXF1ZUlkZW50aWZpZXIAbWltZS1taHMATUlNRSBNSFMAbWltZS1taHMtaGVhZGluZ3MAbWltZS1taHMtYm9kaWVzAGlkLWhleC1wYXJ0aWFsLW1lc3NhZ2UAaWQtaGV4LW11bHRpcGFydC1tZXNzYWdlAGdlbmVyYXRpb25RdWFsaWZpZXIAcHNldWRvbnltAGlkLXNldABTZWN1cmUgRWxlY3Ryb25pYyBUcmFuc2FjdGlvbnMAc2V0LWN0eXBlAGNvbnRlbnQgdHlwZXMAc2V0LW1zZ0V4dABtZXNzYWdlIGV4dGVuc2lvbnMAc2V0LWF0dHIAc2V0LXBvbGljeQBzZXQtY2VydEV4dABjZXJ0aWZpY2F0ZSBleHRlbnNpb25zAHNldC1icmFuZABzZXRjdC1QQU5EYXRhAHNldGN0LVBBTlRva2VuAHNldGN0LVBBTk9ubHkAc2V0Y3QtT0lEYXRhAHNldGN0LVBJAHNldGN0LVBJRGF0YQBzZXRjdC1QSURhdGFVbnNpZ25lZABzZXRjdC1IT0RJbnB1dABzZXRjdC1BdXRoUmVzQmFnZ2FnZQBzZXRjdC1BdXRoUmV2UmVxQmFnZ2FnZQBzZXRjdC1BdXRoUmV2UmVzQmFnZ2FnZQBzZXRjdC1DYXBUb2tlblNlcQBzZXRjdC1QSW5pdFJlc0RhdGEAc2V0Y3QtUEktVEJTAHNldGN0LVBSZXNEYXRhAHNldGN0LUF1dGhSZXFUQlMAc2V0Y3QtQXV0aFJlc1RCUwBzZXRjdC1BdXRoUmVzVEJTWABzZXRjdC1BdXRoVG9rZW5UQlMAc2V0Y3QtQ2FwVG9rZW5EYXRhAHNldGN0LUNhcFRva2VuVEJTAHNldGN0LUFjcUNhcmRDb2RlTXNnAHNldGN0LUF1dGhSZXZSZXFUQlMAc2V0Y3QtQXV0aFJldlJlc0RhdGEAc2V0Y3QtQXV0aFJldlJlc1RCUwBzZXRjdC1DYXBSZXFUQlMAc2V0Y3QtQ2FwUmVxVEJTWABzZXRjdC1DYXBSZXNEYXRhAHNldGN0LUNhcFJldlJlcVRCUwBzZXRjdC1DYXBSZXZSZXFUQlNYAHNldGN0LUNhcFJldlJlc0RhdGEAc2V0Y3QtQ3JlZFJlcVRCUwBzZXRjdC1DcmVkUmVxVEJTWABzZXRjdC1DcmVkUmVzRGF0YQBzZXRjdC1DcmVkUmV2UmVxVEJTAHNldGN0LUNyZWRSZXZSZXFUQlNYAHNldGN0LUNyZWRSZXZSZXNEYXRhAHNldGN0LVBDZXJ0UmVxRGF0YQBzZXRjdC1QQ2VydFJlc1RCUwBzZXRjdC1CYXRjaEFkbWluUmVxRGF0YQBzZXRjdC1CYXRjaEFkbWluUmVzRGF0YQBzZXRjdC1DYXJkQ0luaXRSZXNUQlMAc2V0Y3QtTWVBcUNJbml0UmVzVEJTAHNldGN0LVJlZ0Zvcm1SZXNUQlMAc2V0Y3QtQ2VydFJlcURhdGEAc2V0Y3QtQ2VydFJlcVRCUwBzZXRjdC1DZXJ0UmVzRGF0YQBzZXRjdC1DZXJ0SW5xUmVxVEJTAHNldGN0LUVycm9yVEJTAHNldGN0LVBJRHVhbFNpZ25lZFRCRQBzZXRjdC1QSVVuc2lnbmVkVEJFAHNldGN0LUF1dGhSZXFUQkUAc2V0Y3QtQXV0aFJlc1RCRQBzZXRjdC1BdXRoUmVzVEJFWABzZXRjdC1BdXRoVG9rZW5UQkUAc2V0Y3QtQ2FwVG9rZW5UQkUAc2V0Y3QtQ2FwVG9rZW5UQkVYAHNldGN0LUFjcUNhcmRDb2RlTXNnVEJFAHNldGN0LUF1dGhSZXZSZXFUQkUAc2V0Y3QtQXV0aFJldlJlc1RCRQBzZXRjdC1BdXRoUmV2UmVzVEJFQgBzZXRjdC1DYXBSZXFUQkUAc2V0Y3QtQ2FwUmVxVEJFWABzZXRjdC1DYXBSZXNUQkUAc2V0Y3QtQ2FwUmV2UmVxVEJFAHNldGN0LUNhcFJldlJlcVRCRVgAc2V0Y3QtQ2FwUmV2UmVzVEJFAHNldGN0LUNyZWRSZXFUQkUAc2V0Y3QtQ3JlZFJlcVRCRVgAc2V0Y3QtQ3JlZFJlc1RCRQBzZXRjdC1DcmVkUmV2UmVxVEJFAHNldGN0LUNyZWRSZXZSZXFUQkVYAHNldGN0LUNyZWRSZXZSZXNUQkUAc2V0Y3QtQmF0Y2hBZG1pblJlcVRCRQBzZXRjdC1CYXRjaEFkbWluUmVzVEJFAHNldGN0LVJlZ0Zvcm1SZXFUQkUAc2V0Y3QtQ2VydFJlcVRCRQBzZXRjdC1DZXJ0UmVxVEJFWABzZXRjdC1DZXJ0UmVzVEJFAHNldGN0LUNSTE5vdGlmaWNhdGlvblRCUwBzZXRjdC1DUkxOb3RpZmljYXRpb25SZXNUQlMAc2V0Y3QtQkNJRGlzdHJpYnV0aW9uVEJTAHNldGV4dC1nZW5DcnlwdABnZW5lcmljIGNyeXB0b2dyYW0Ac2V0ZXh0LW1pQXV0aABtZXJjaGFudCBpbml0aWF0ZWQgYXV0aABzZXRleHQtcGluU2VjdXJlAHNldGV4dC1waW5BbnkAc2V0ZXh0LXRyYWNrMgBzZXRleHQtY3YAYWRkaXRpb25hbCB2ZXJpZmljYXRpb24Ac2V0LXBvbGljeS1yb290AHNldENleHQtaGFzaGVkUm9vdABzZXRDZXh0LWNlcnRUeXBlAHNldENleHQtbWVyY2hEYXRhAHNldENleHQtY0NlcnRSZXF1aXJlZABzZXRDZXh0LXR1bm5lbGluZwBzZXRDZXh0LXNldEV4dABzZXRDZXh0LXNldFF1YWxmAHNldENleHQtUEdXWWNhcGFiaWxpdGllcwBzZXRDZXh0LVRva2VuSWRlbnRpZmllcgBzZXRDZXh0LVRyYWNrMkRhdGEAc2V0Q2V4dC1Ub2tlblR5cGUAc2V0Q2V4dC1Jc3N1ZXJDYXBhYmlsaXRpZXMAc2V0QXR0ci1DZXJ0AHNldEF0dHItUEdXWWNhcABwYXltZW50IGdhdGV3YXkgY2FwYWJpbGl0aWVzAHNldEF0dHItVG9rZW5UeXBlAHNldEF0dHItSXNzQ2FwAGlzc3VlciBjYXBhYmlsaXRpZXMAc2V0LXJvb3RLZXlUaHVtYgBzZXQtYWRkUG9saWN5AHNldEF0dHItVG9rZW4tRU1WAHNldEF0dHItVG9rZW4tQjBQcmltZQBzZXRBdHRyLUlzc0NhcC1DVk0Ac2V0QXR0ci1Jc3NDYXAtVDIAc2V0QXR0ci1Jc3NDYXAtU2lnAHNldEF0dHItR2VuQ3J5cHRncm0AZ2VuZXJhdGUgY3J5cHRvZ3JhbQBzZXRBdHRyLVQyRW5jAGVuY3J5cHRlZCB0cmFjayAyAHNldEF0dHItVDJjbGVhcnR4dABjbGVhcnRleHQgdHJhY2sgMgBzZXRBdHRyLVRva0lDQ3NpZwBJQ0Mgb3IgdG9rZW4gc2lnbmF0dXJlAHNldEF0dHItU2VjRGV2U2lnAHNlY3VyZSBkZXZpY2Ugc2lnbmF0dXJlAHNldC1icmFuZC1JQVRBLUFUQQBzZXQtYnJhbmQtRGluZXJzAHNldC1icmFuZC1BbWVyaWNhbkV4cHJlc3MAc2V0LWJyYW5kLUpDQgBzZXQtYnJhbmQtVmlzYQBzZXQtYnJhbmQtTWFzdGVyQ2FyZABzZXQtYnJhbmQtTm92dXMAREVTLUNETUYAZGVzLWNkbWYAcnNhT0FFUEVuY3J5cHRpb25TRVQASVRVLVQAaXR1LXQASk9JTlQtSVNPLUlUVS1UAGpvaW50LWlzby1pdHUtdABpbnRlcm5hdGlvbmFsLW9yZ2FuaXphdGlvbnMASW50ZXJuYXRpb25hbCBPcmdhbml6YXRpb25zAG1zU21hcnRjYXJkTG9naW4ATWljcm9zb2Z0IFNtYXJ0Y2FyZGxvZ2luAG1zVVBOAE1pY3Jvc29mdCBVbml2ZXJzYWwgUHJpbmNpcGFsIE5hbWUAQUVTLTEyOC1DRkIxAGFlcy0xMjgtY2ZiMQBBRVMtMTkyLUNGQjEAYWVzLTE5Mi1jZmIxAEFFUy0yNTYtQ0ZCMQBhZXMtMjU2LWNmYjEAQUVTLTEyOC1DRkI4AGFlcy0xMjgtY2ZiOABBRVMtMTkyLUNGQjgAYWVzLTE5Mi1jZmI4AEFFUy0yNTYtQ0ZCOABhZXMtMjU2LWNmYjgAREVTLUNGQjEAZGVzLWNmYjEAREVTLUNGQjgAZGVzLWNmYjgAREVTLUVERTMtQ0ZCMQBkZXMtZWRlMy1jZmIxAERFUy1FREUzLUNGQjgAZGVzLWVkZTMtY2ZiOABzdHJlZXQAc3RyZWV0QWRkcmVzcwBwb3N0YWxDb2RlAGlkLXBwbABwcm94eUNlcnRJbmZvAFByb3h5IENlcnRpZmljYXRlIEluZm9ybWF0aW9uAGlkLXBwbC1hbnlMYW5ndWFnZQBBbnkgbGFuZ3VhZ2UAaWQtcHBsLWluaGVyaXRBbGwASW5oZXJpdCBhbGwAbmFtZUNvbnN0cmFpbnRzAFg1MDl2MyBOYW1lIENvbnN0cmFpbnRzAGlkLXBwbC1pbmRlcGVuZGVudABJbmRlcGVuZGVudABSU0EtU0hBMjU2AHNoYTI1NldpdGhSU0FFbmNyeXB0aW9uAFJTQS1TSEEzODQAc2hhMzg0V2l0aFJTQUVuY3J5cHRpb24AUlNBLVNIQTUxMgBzaGE1MTJXaXRoUlNBRW5jcnlwdGlvbgBSU0EtU0hBMjI0AHNoYTIyNFdpdGhSU0FFbmNyeXB0aW9uAFNIQTI1NgBzaGEyNTYAU0hBMzg0AHNoYTM4NABTSEE1MTIAc2hhNTEyAFNIQTIyNABzaGEyMjQAaWRlbnRpZmllZC1vcmdhbml6YXRpb24AY2VydGljb20tYXJjAHdhcAB3YXAtd3NnAGlkLWNoYXJhY3RlcmlzdGljLXR3by1iYXNpcwBvbkJhc2lzAHRwQmFzaXMAcHBCYXNpcwBjMnBuYjE2M3YxAGMycG5iMTYzdjIAYzJwbmIxNjN2MwBjMnBuYjE3NnYxAGMydG5iMTkxdjEAYzJ0bmIxOTF2MgBjMnRuYjE5MXYzAGMyb25iMTkxdjQAYzJvbmIxOTF2NQBjMnBuYjIwOHcxAGMydG5iMjM5djEAYzJ0bmIyMzl2MgBjMnRuYjIzOXYzAGMyb25iMjM5djQAYzJvbmIyMzl2NQBjMnBuYjI3MncxAGMycG5iMzA0dzEAYzJ0bmIzNTl2MQBjMnBuYjM2OHcxAGMydG5iNDMxcjEAc2VjcDExMnIxAHNlY3AxMTJyMgBzZWNwMTI4cjEAc2VjcDEyOHIyAHNlY3AxNjBrMQBzZWNwMTYwcjEAc2VjcDE2MHIyAHNlY3AxOTJrMQBzZWNwMjI0azEAc2VjcDIyNHIxAHNlY3AyNTZrMQBzZWNwMzg0cjEAc2VjcDUyMXIxAHNlY3QxMTNyMQBzZWN0MTEzcjIAc2VjdDEzMXIxAHNlY3QxMzFyMgBzZWN0MTYzazEAc2VjdDE2M3IxAHNlY3QxNjNyMgBzZWN0MTkzcjEAc2VjdDE5M3IyAHNlY3QyMzNrMQBzZWN0MjMzcjEAc2VjdDIzOWsxAHNlY3QyODNrMQBzZWN0MjgzcjEAc2VjdDQwOWsxAHNlY3Q0MDlyMQBzZWN0NTcxazEAc2VjdDU3MXIxAHdhcC13c2ctaWRtLWVjaWQtd3RsczEAd2FwLXdzZy1pZG0tZWNpZC13dGxzMwB3YXAtd3NnLWlkbS1lY2lkLXd0bHM0AHdhcC13c2ctaWRtLWVjaWQtd3RsczUAd2FwLXdzZy1pZG0tZWNpZC13dGxzNgB3YXAtd3NnLWlkbS1lY2lkLXd0bHM3AHdhcC13c2ctaWRtLWVjaWQtd3RsczgAd2FwLXdzZy1pZG0tZWNpZC13dGxzOQB3YXAtd3NnLWlkbS1lY2lkLXd0bHMxMAB3YXAtd3NnLWlkbS1lY2lkLXd0bHMxMQB3YXAtd3NnLWlkbS1lY2lkLXd0bHMxMgBhbnlQb2xpY3kAWDUwOXYzIEFueSBQb2xpY3kAcG9saWN5TWFwcGluZ3MAWDUwOXYzIFBvbGljeSBNYXBwaW5ncwBpbmhpYml0QW55UG9saWN5AFg1MDl2MyBJbmhpYml0IEFueSBQb2xpY3kAT2FrbGV5LUVDMk4tMwBpcHNlYzMAT2FrbGV5LUVDMk4tNABpcHNlYzQAQ0FNRUxMSUEtMTI4LUNCQwBjYW1lbGxpYS0xMjgtY2JjAENBTUVMTElBLTE5Mi1DQkMAY2FtZWxsaWEtMTkyLWNiYwBDQU1FTExJQS0yNTYtQ0JDAGNhbWVsbGlhLTI1Ni1jYmMAQ0FNRUxMSUEtMTI4LUVDQgBjYW1lbGxpYS0xMjgtZWNiAENBTUVMTElBLTE5Mi1FQ0IAY2FtZWxsaWEtMTkyLWVjYgBDQU1FTExJQS0yNTYtRUNCAGNhbWVsbGlhLTI1Ni1lY2IAQ0FNRUxMSUEtMTI4LUNGQgBjYW1lbGxpYS0xMjgtY2ZiAENBTUVMTElBLTE5Mi1DRkIAY2FtZWxsaWEtMTkyLWNmYgBDQU1FTExJQS0yNTYtQ0ZCAGNhbWVsbGlhLTI1Ni1jZmIAQ0FNRUxMSUEtMTI4LUNGQjEAY2FtZWxsaWEtMTI4LWNmYjEAQ0FNRUxMSUEtMTkyLUNGQjEAY2FtZWxsaWEtMTkyLWNmYjEAQ0FNRUxMSUEtMjU2LUNGQjEAY2FtZWxsaWEtMjU2LWNmYjEAQ0FNRUxMSUEtMTI4LUNGQjgAY2FtZWxsaWEtMTI4LWNmYjgAQ0FNRUxMSUEtMTkyLUNGQjgAY2FtZWxsaWEtMTkyLWNmYjgAQ0FNRUxMSUEtMjU2LUNGQjgAY2FtZWxsaWEtMjU2LWNmYjgAQ0FNRUxMSUEtMTI4LU9GQgBjYW1lbGxpYS0xMjgtb2ZiAENBTUVMTElBLTE5Mi1PRkIAY2FtZWxsaWEtMTkyLW9mYgBDQU1FTExJQS0yNTYtT0ZCAGNhbWVsbGlhLTI1Ni1vZmIAc3ViamVjdERpcmVjdG9yeUF0dHJpYnV0ZXMAWDUwOXYzIFN1YmplY3QgRGlyZWN0b3J5IEF0dHJpYnV0ZXMAaXNzdWluZ0Rpc3RyaWJ1dGlvblBvaW50AFg1MDl2MyBJc3N1aW5nIERpc3RyaWJ1dGlvbiBQb2ludABjZXJ0aWZpY2F0ZUlzc3VlcgBYNTA5djMgQ2VydGlmaWNhdGUgSXNzdWVyAEtJU0EAa2lzYQBTRUVELUVDQgBzZWVkLWVjYgBTRUVELUNCQwBzZWVkLWNiYwBTRUVELU9GQgBzZWVkLW9mYgBTRUVELUNGQgBzZWVkLWNmYgBITUFDLU1ENQBobWFjLW1kNQBITUFDLVNIQTEAaG1hYy1zaGExAGlkLVBhc3N3b3JkQmFzZWRNQUMAcGFzc3dvcmQgYmFzZWQgTUFDAGlkLURIQmFzZWRNYWMARGlmZmllLUhlbGxtYW4gYmFzZWQgTUFDAGlkLWl0LXN1cHBMYW5nVGFncwBjYVJlcG9zaXRvcnkAQ0EgUmVwb3NpdG9yeQBpZC1zbWltZS1jdC1jb21wcmVzc2VkRGF0YQBpZC1jdC1hc2NpaVRleHRXaXRoQ1JMRgBpZC1hZXMxMjgtd3JhcABpZC1hZXMxOTItd3JhcABpZC1hZXMyNTYtd3JhcABlY2RzYS13aXRoLVJlY29tbWVuZGVkAGVjZHNhLXdpdGgtU3BlY2lmaWVkAGVjZHNhLXdpdGgtU0hBMjI0AGVjZHNhLXdpdGgtU0hBMjU2AGVjZHNhLXdpdGgtU0hBMzg0AGVjZHNhLXdpdGgtU0hBNTEyAGhtYWNXaXRoTUQ1AGhtYWNXaXRoU0hBMjI0AGhtYWNXaXRoU0hBMjU2AGhtYWNXaXRoU0hBMzg0AGhtYWNXaXRoU0hBNTEyAGRzYV93aXRoX1NIQTIyNABkc2Ffd2l0aF9TSEEyNTYAd2hpcmxwb29sAGNyeXB0b3BybwBjcnlwdG9jb20AaWQtR29zdFIzNDExLTk0LXdpdGgtR29zdFIzNDEwLTIwMDEAR09TVCBSIDM0LjExLTk0IHdpdGggR09TVCBSIDM0LjEwLTIwMDEAaWQtR29zdFIzNDExLTk0LXdpdGgtR29zdFIzNDEwLTk0AEdPU1QgUiAzNC4xMS05NCB3aXRoIEdPU1QgUiAzNC4xMC05NABtZF9nb3N0OTQAR09TVCBSIDM0LjExLTk0AGlkLUhNQUNHb3N0UjM0MTEtOTQASE1BQyBHT1NUIDM0LjExLTk0AGdvc3QyMDAxAEdPU1QgUiAzNC4xMC0yMDAxAGdvc3Q5NABHT1NUIFIgMzQuMTAtOTQAZ29zdDg5AEdPU1QgMjgxNDctODkAZ29zdDg5LWNudABnb3N0LW1hYwBHT1NUIDI4MTQ3LTg5IE1BQwBwcmYtZ29zdHIzNDExLTk0AEdPU1QgUiAzNC4xMS05NCBQUkYAaWQtR29zdFIzNDEwLTIwMDFESABHT1NUIFIgMzQuMTAtMjAwMSBESABpZC1Hb3N0UjM0MTAtOTRESABHT1NUIFIgMzQuMTAtOTQgREgAaWQtR29zdDI4MTQ3LTg5LUNyeXB0b1Byby1LZXlNZXNoaW5nAGlkLUdvc3QyODE0Ny04OS1Ob25lLUtleU1lc2hpbmcAaWQtR29zdFIzNDExLTk0LVRlc3RQYXJhbVNldABpZC1Hb3N0UjM0MTEtOTQtQ3J5cHRvUHJvUGFyYW1TZXQAaWQtR29zdDI4MTQ3LTg5LVRlc3RQYXJhbVNldABpZC1Hb3N0MjgxNDctODktQ3J5cHRvUHJvLUEtUGFyYW1TZXQAaWQtR29zdDI4MTQ3LTg5LUNyeXB0b1Byby1CLVBhcmFtU2V0AGlkLUdvc3QyODE0Ny04OS1DcnlwdG9Qcm8tQy1QYXJhbVNldABpZC1Hb3N0MjgxNDctODktQ3J5cHRvUHJvLUQtUGFyYW1TZXQAaWQtR29zdDI4MTQ3LTg5LUNyeXB0b1Byby1Pc2Nhci0xLTEtUGFyYW1TZXQAaWQtR29zdDI4MTQ3LTg5LUNyeXB0b1Byby1Pc2Nhci0xLTAtUGFyYW1TZXQAaWQtR29zdDI4MTQ3LTg5LUNyeXB0b1Byby1SSUMtMS1QYXJhbVNldABpZC1Hb3N0UjM0MTAtOTQtVGVzdFBhcmFtU2V0AGlkLUdvc3RSMzQxMC05NC1DcnlwdG9Qcm8tQS1QYXJhbVNldABpZC1Hb3N0UjM0MTAtOTQtQ3J5cHRvUHJvLUItUGFyYW1TZXQAaWQtR29zdFIzNDEwLTk0LUNyeXB0b1Byby1DLVBhcmFtU2V0AGlkLUdvc3RSMzQxMC05NC1DcnlwdG9Qcm8tRC1QYXJhbVNldABpZC1Hb3N0UjM0MTAtOTQtQ3J5cHRvUHJvLVhjaEEtUGFyYW1TZXQAaWQtR29zdFIzNDEwLTk0LUNyeXB0b1Byby1YY2hCLVBhcmFtU2V0AGlkLUdvc3RSMzQxMC05NC1DcnlwdG9Qcm8tWGNoQy1QYXJhbVNldABpZC1Hb3N0UjM0MTAtMjAwMS1UZXN0UGFyYW1TZXQAaWQtR29zdFIzNDEwLTIwMDEtQ3J5cHRvUHJvLUEtUGFyYW1TZXQAaWQtR29zdFIzNDEwLTIwMDEtQ3J5cHRvUHJvLUItUGFyYW1TZXQAaWQtR29zdFIzNDEwLTIwMDEtQ3J5cHRvUHJvLUMtUGFyYW1TZXQAaWQtR29zdFIzNDEwLTIwMDEtQ3J5cHRvUHJvLVhjaEEtUGFyYW1TZXQAaWQtR29zdFIzNDEwLTIwMDEtQ3J5cHRvUHJvLVhjaEItUGFyYW1TZXQAaWQtR29zdFIzNDEwLTk0LWEAaWQtR29zdFIzNDEwLTk0LWFCaXMAaWQtR29zdFIzNDEwLTk0LWIAaWQtR29zdFIzNDEwLTk0LWJCaXMAaWQtR29zdDI4MTQ3LTg5LWNjAEdPU1QgMjgxNDctODkgQ3J5cHRvY29tIFBhcmFtU2V0AGdvc3Q5NGNjAEdPU1QgMzQuMTAtOTQgQ3J5cHRvY29tAGdvc3QyMDAxY2MAR09TVCAzNC4xMC0yMDAxIENyeXB0b2NvbQBpZC1Hb3N0UjM0MTEtOTQtd2l0aC1Hb3N0UjM0MTAtOTQtY2MAR09TVCBSIDM0LjExLTk0IHdpdGggR09TVCBSIDM0LjEwLTk0IENyeXB0b2NvbQBpZC1Hb3N0UjM0MTEtOTQtd2l0aC1Hb3N0UjM0MTAtMjAwMS1jYwBHT1NUIFIgMzQuMTEtOTQgd2l0aCBHT1NUIFIgMzQuMTAtMjAwMSBDcnlwdG9jb20AaWQtR29zdFIzNDEwLTIwMDEtUGFyYW1TZXQtY2MAR09TVCBSIDM0MTAtMjAwMSBQYXJhbWV0ZXIgU2V0IENyeXB0b2NvbQBITUFDAGhtYWMATG9jYWxLZXlTZXQATWljcm9zb2Z0IExvY2FsIEtleSBzZXQAZnJlc2hlc3RDUkwAWDUwOXYzIEZyZXNoZXN0IENSTABpZC1vbi1wZXJtYW5lbnRJZGVudGlmaWVyAFBlcm1hbmVudCBJZGVudGlmaWVyAHNlYXJjaEd1aWRlAGJ1c2luZXNzQ2F0ZWdvcnkAcG9zdGFsQWRkcmVzcwBwb3N0T2ZmaWNlQm94AHBoeXNpY2FsRGVsaXZlcnlPZmZpY2VOYW1lAHRlbGVwaG9uZU51bWJlcgB0ZWxleE51bWJlcgB0ZWxldGV4VGVybWluYWxJZGVudGlmaWVyAGZhY3NpbWlsZVRlbGVwaG9uZU51bWJlcgB4MTIxQWRkcmVzcwBpbnRlcm5hdGlvbmFsaVNETk51bWJlcgByZWdpc3RlcmVkQWRkcmVzcwBkZXN0aW5hdGlvbkluZGljYXRvcgBwcmVmZXJyZWREZWxpdmVyeU1ldGhvZABwcmVzZW50YXRpb25BZGRyZXNzAHN1cHBvcnRlZEFwcGxpY2F0aW9uQ29udGV4dABtZW1iZXIAb3duZXIAcm9sZU9jY3VwYW50AHNlZUFsc28AdXNlclBhc3N3b3JkAHVzZXJDZXJ0aWZpY2F0ZQBjQUNlcnRpZmljYXRlAGF1dGhvcml0eVJldm9jYXRpb25MaXN0AGNlcnRpZmljYXRlUmV2b2NhdGlvbkxpc3QAY3Jvc3NDZXJ0aWZpY2F0ZVBhaXIAZW5oYW5jZWRTZWFyY2hHdWlkZQBwcm90b2NvbEluZm9ybWF0aW9uAGRpc3Rpbmd1aXNoZWROYW1lAHVuaXF1ZU1lbWJlcgBob3VzZUlkZW50aWZpZXIAc3VwcG9ydGVkQWxnb3JpdGhtcwBkZWx0YVJldm9jYXRpb25MaXN0AGRtZE5hbWUAaWQtYWxnLVBXUkktS0VLAENNQUMAY21hYwBpZC1hZXMxMjgtR0NNAGFlcy0xMjgtZ2NtAGlkLWFlczEyOC1DQ00AYWVzLTEyOC1jY20AaWQtYWVzMTI4LXdyYXAtcGFkAGlkLWFlczE5Mi1HQ00AYWVzLTE5Mi1nY20AaWQtYWVzMTkyLUNDTQBhZXMtMTkyLWNjbQBpZC1hZXMxOTItd3JhcC1wYWQAaWQtYWVzMjU2LUdDTQBhZXMtMjU2LWdjbQBpZC1hZXMyNTYtQ0NNAGFlcy0yNTYtY2NtAGlkLWFlczI1Ni13cmFwLXBhZABBRVMtMTI4LUNUUgBhZXMtMTI4LWN0cgBBRVMtMTkyLUNUUgBhZXMtMTkyLWN0cgBBRVMtMjU2LUNUUgBhZXMtMjU2LWN0cgBpZC1jYW1lbGxpYTEyOC13cmFwAGlkLWNhbWVsbGlhMTkyLXdyYXAAaWQtY2FtZWxsaWEyNTYtd3JhcABhbnlFeHRlbmRlZEtleVVzYWdlAEFueSBFeHRlbmRlZCBLZXkgVXNhZ2UATUdGMQBtZ2YxAFJTQVNTQS1QU1MAcnNhc3NhUHNzAEFFUy0xMjgtWFRTAGFlcy0xMjgteHRzAEFFUy0yNTYtWFRTAGFlcy0yNTYteHRzAFJDNC1ITUFDLU1ENQByYzQtaG1hYy1tZDUAQUVTLTEyOC1DQkMtSE1BQy1TSEExAGFlcy0xMjgtY2JjLWhtYWMtc2hhMQBBRVMtMTkyLUNCQy1ITUFDLVNIQTEAYWVzLTE5Mi1jYmMtaG1hYy1zaGExAEFFUy0yNTYtQ0JDLUhNQUMtU0hBMQBhZXMtMjU2LWNiYy1obWFjLXNoYTEAUlNBRVMtT0FFUAByc2Flc09hZXAAZGhwdWJsaWNudW1iZXIAWDkuNDIgREgAYnJhaW5wb29sUDE2MHIxAGJyYWlucG9vbFAxNjB0MQBicmFpbnBvb2xQMTkycjEAYnJhaW5wb29sUDE5MnQxAGJyYWlucG9vbFAyMjRyMQBicmFpbnBvb2xQMjI0dDEAYnJhaW5wb29sUDI1NnIxAGJyYWlucG9vbFAyNTZ0MQBicmFpbnBvb2xQMzIwcjEAYnJhaW5wb29sUDMyMHQxAGJyYWlucG9vbFAzODRyMQBicmFpbnBvb2xQMzg0dDEAYnJhaW5wb29sUDUxMnIxAGJyYWlucG9vbFA1MTJ0MQBQU1BFQ0lGSUVEAHBTcGVjaWZpZWQAZGhTaW5nbGVQYXNzLXN0ZERILXNoYTFrZGYtc2NoZW1lAGRoU2luZ2xlUGFzcy1zdGRESC1zaGEyMjRrZGYtc2NoZW1lAGRoU2luZ2xlUGFzcy1zdGRESC1zaGEyNTZrZGYtc2NoZW1lAGRoU2luZ2xlUGFzcy1zdGRESC1zaGEzODRrZGYtc2NoZW1lAGRoU2luZ2xlUGFzcy1zdGRESC1zaGE1MTJrZGYtc2NoZW1lAGRoU2luZ2xlUGFzcy1jb2ZhY3RvckRILXNoYTFrZGYtc2NoZW1lAGRoU2luZ2xlUGFzcy1jb2ZhY3RvckRILXNoYTIyNGtkZi1zY2hlbWUAZGhTaW5nbGVQYXNzLWNvZmFjdG9yREgtc2hhMjU2a2RmLXNjaGVtZQBkaFNpbmdsZVBhc3MtY29mYWN0b3JESC1zaGEzODRrZGYtc2NoZW1lAGRoU2luZ2xlUGFzcy1jb2ZhY3RvckRILXNoYTUxMmtkZi1zY2hlbWUAZGgtc3RkLWtkZgBkaC1jb2ZhY3Rvci1rZGYAQUVTLTEyOC1DQkMtSE1BQy1TSEEyNTYAYWVzLTEyOC1jYmMtaG1hYy1zaGEyNTYAQUVTLTE5Mi1DQkMtSE1BQy1TSEEyNTYAYWVzLTE5Mi1jYmMtaG1hYy1zaGEyNTYAQUVTLTI1Ni1DQkMtSE1BQy1TSEEyNTYAYWVzLTI1Ni1jYmMtaG1hYy1zaGEyNTYAY3RfcHJlY2VydF9zY3RzAENUIFByZWNlcnRpZmljYXRlIFNDVHMAY3RfcHJlY2VydF9wb2lzb24AQ1QgUHJlY2VydGlmaWNhdGUgUG9pc29uAGN0X3ByZWNlcnRfc2lnbmVyAENUIFByZWNlcnRpZmljYXRlIFNpZ25lcgBjdF9jZXJ0X3NjdHMAQ1QgQ2VydGlmaWNhdGUgU0NUcwBqdXJpc2RpY3Rpb25MAGp1cmlzZGljdGlvbkxvY2FsaXR5TmFtZQBqdXJpc2RpY3Rpb25TVABqdXJpc2RpY3Rpb25TdGF0ZU9yUHJvdmluY2VOYW1lAGp1cmlzZGljdGlvbkMAanVyaXNkaWN0aW9uQ291bnRyeU5hbWUAQUVTLTEyOC1PQ0IAYWVzLTEyOC1vY2IAQUVTLTE5Mi1PQ0IAYWVzLTE5Mi1vY2IAQUVTLTI1Ni1PQ0IAYWVzLTI1Ni1vY2IAQ0FNRUxMSUEtMTI4LUdDTQBjYW1lbGxpYS0xMjgtZ2NtAENBTUVMTElBLTEyOC1DQ00AY2FtZWxsaWEtMTI4LWNjbQBDQU1FTExJQS0xMjgtQ1RSAGNhbWVsbGlhLTEyOC1jdHIAQ0FNRUxMSUEtMTI4LUNNQUMAY2FtZWxsaWEtMTI4LWNtYWMAQ0FNRUxMSUEtMTkyLUdDTQBjYW1lbGxpYS0xOTItZ2NtAENBTUVMTElBLTE5Mi1DQ00AY2FtZWxsaWEtMTkyLWNjbQBDQU1FTExJQS0xOTItQ1RSAGNhbWVsbGlhLTE5Mi1jdHIAQ0FNRUxMSUEtMTkyLUNNQUMAY2FtZWxsaWEtMTkyLWNtYWMAQ0FNRUxMSUEtMjU2LUdDTQBjYW1lbGxpYS0yNTYtZ2NtAENBTUVMTElBLTI1Ni1DQ00AY2FtZWxsaWEtMjU2LWNjbQBDQU1FTExJQS0yNTYtQ1RSAGNhbWVsbGlhLTI1Ni1jdHIAQ0FNRUxMSUEtMjU2LUNNQUMAY2FtZWxsaWEtMjU2LWNtYWMAaWQtc2NyeXB0AGlkLXRjMjYAZ29zdDg5LWNudC0xMgBnb3N0LW1hYy0xMgBpZC10YzI2LWFsZ29yaXRobXMAaWQtdGMyNi1zaWduAGdvc3QyMDEyXzI1NgBHT1NUIFIgMzQuMTAtMjAxMiB3aXRoIDI1NiBiaXQgbW9kdWx1cwBnb3N0MjAxMl81MTIAR09TVCBSIDM0LjEwLTIwMTIgd2l0aCA1MTIgYml0IG1vZHVsdXMAaWQtdGMyNi1kaWdlc3QAbWRfZ29zdDEyXzI1NgBHT1NUIFIgMzQuMTEtMjAxMiB3aXRoIDI1NiBiaXQgaGFzaABtZF9nb3N0MTJfNTEyAEdPU1QgUiAzNC4xMS0yMDEyIHdpdGggNTEyIGJpdCBoYXNoAGlkLXRjMjYtc2lnbndpdGhkaWdlc3QAaWQtdGMyNi1zaWdud2l0aGRpZ2VzdC1nb3N0MzQxMC0yMDEyLTI1NgBHT1NUIFIgMzQuMTAtMjAxMiB3aXRoIEdPU1QgUiAzNC4xMS0yMDEyICgyNTYgYml0KQBpZC10YzI2LXNpZ253aXRoZGlnZXN0LWdvc3QzNDEwLTIwMTItNTEyAEdPU1QgUiAzNC4xMC0yMDEyIHdpdGggR09TVCBSIDM0LjExLTIwMTIgKDUxMiBiaXQpAGlkLXRjMjYtbWFjAGlkLXRjMjYtaG1hYy1nb3N0LTM0MTEtMjAxMi0yNTYASE1BQyBHT1NUIDM0LjExLTIwMTIgMjU2IGJpdABpZC10YzI2LWhtYWMtZ29zdC0zNDExLTIwMTItNTEyAEhNQUMgR09TVCAzNC4xMS0yMDEyIDUxMiBiaXQAaWQtdGMyNi1jaXBoZXIAaWQtdGMyNi1hZ3JlZW1lbnQAaWQtdGMyNi1hZ3JlZW1lbnQtZ29zdC0zNDEwLTIwMTItMjU2AGlkLXRjMjYtYWdyZWVtZW50LWdvc3QtMzQxMC0yMDEyLTUxMgBpZC10YzI2LWNvbnN0YW50cwBpZC10YzI2LXNpZ24tY29uc3RhbnRzAGlkLXRjMjYtZ29zdC0zNDEwLTIwMTItNTEyLWNvbnN0YW50cwBpZC10YzI2LWdvc3QtMzQxMC0yMDEyLTUxMi1wYXJhbVNldFRlc3QAR09TVCBSIDM0LjEwLTIwMTIgKDUxMiBiaXQpIHRlc3RpbmcgcGFyYW1ldGVyIHNldABpZC10YzI2LWdvc3QtMzQxMC0yMDEyLTUxMi1wYXJhbVNldEEAR09TVCBSIDM0LjEwLTIwMTIgKDUxMiBiaXQpIFBhcmFtU2V0IEEAaWQtdGMyNi1nb3N0LTM0MTAtMjAxMi01MTItcGFyYW1TZXRCAEdPU1QgUiAzNC4xMC0yMDEyICg1MTIgYml0KSBQYXJhbVNldCBCAGlkLXRjMjYtZGlnZXN0LWNvbnN0YW50cwBpZC10YzI2LWNpcGhlci1jb25zdGFudHMAaWQtdGMyNi1nb3N0LTI4MTQ3LWNvbnN0YW50cwBpZC10YzI2LWdvc3QtMjgxNDctcGFyYW0tWgBHT1NUIDI4MTQ3LTg5IFRDMjYgcGFyYW1ldGVyIHNldABJTk4AT0dSTgBTTklMUwBzdWJqZWN0U2lnblRvb2wAU2lnbmluZyBUb29sIG9mIFN1YmplY3QAaXNzdWVyU2lnblRvb2wAU2lnbmluZyBUb29sIG9mIElzc3VlcgBnb3N0ODktY2JjAGdvc3Q4OS1lY2IAZ29zdDg5LWN0cgBncmFzc2hvcHBlci1lY2IAZ3Jhc3Nob3BwZXItY3RyAGdyYXNzaG9wcGVyLW9mYgBncmFzc2hvcHBlci1jYmMAZ3Jhc3Nob3BwZXItY2ZiAGdyYXNzaG9wcGVyLW1hYwBDaGFDaGEyMC1Qb2x5MTMwNQBjaGFjaGEyMC1wb2x5MTMwNQBDaGFDaGEyMABjaGFjaGEyMAB0bHNmZWF0dXJlAFRMUyBGZWF0dXJlAFRMUzEtUFJGAHRsczEtcHJmAGlwc2VjSUtFAGlwc2VjIEludGVybmV0IEtleSBFeGNoYW5nZQBjYXB3YXBBQwBDdHJsL3Byb3Zpc2lvbiBXQVAgQWNjZXNzAGNhcHdhcFdUUABDdHJsL1Byb3Zpc2lvbiBXQVAgVGVybWluYXRpb24Ac2VjdXJlU2hlbGxDbGllbnQAU1NIIENsaWVudABzZWN1cmVTaGVsbFNlcnZlcgBTU0ggU2VydmVyAHNlbmRSb3V0ZXIAU2VuZCBSb3V0ZXIAc2VuZFByb3hpZWRSb3V0ZXIAU2VuZCBQcm94aWVkIFJvdXRlcgBzZW5kT3duZXIAU2VuZCBPd25lcgBzZW5kUHJveGllZE93bmVyAFNlbmQgUHJveGllZCBPd25lcgBpZC1wa2luaXQAcGtJbml0Q2xpZW50QXV0aABQS0lOSVQgQ2xpZW50IEF1dGgAcGtJbml0S0RDAFNpZ25pbmcgS0RDIFJlc3BvbnNlAFgyNTUxOQBYNDQ4AEhLREYAaGtkZgBLeFJTQQBreC1yc2EAS3hFQ0RIRQBreC1lY2RoZQBLeERIRQBreC1kaGUAS3hFQ0RIRS1QU0sAa3gtZWNkaGUtcHNrAEt4REhFLVBTSwBreC1kaGUtcHNrAEt4UlNBX1BTSwBreC1yc2EtcHNrAEt4UFNLAGt4LXBzawBLeFNSUABreC1zcnAAS3hHT1NUAGt4LWdvc3QAQXV0aFJTQQBhdXRoLXJzYQBBdXRoRUNEU0EAYXV0aC1lY2RzYQBBdXRoUFNLAGF1dGgtcHNrAEF1dGhEU1MAYXV0aC1kc3MAQXV0aEdPU1QwMQBhdXRoLWdvc3QwMQBBdXRoR09TVDEyAGF1dGgtZ29zdDEyAEF1dGhTUlAAYXV0aC1zcnAAQXV0aE5VTEwAYXV0aC1udWxsAEJMQUtFMmI1MTIAYmxha2UyYjUxMgBCTEFLRTJzMjU2AGJsYWtlMnMyNTYAaWQtc21pbWUtY3QtY29udGVudENvbGxlY3Rpb24AaWQtc21pbWUtY3QtYXV0aEVudmVsb3BlZERhdGEAaWQtY3QteG1sAC4lbHUAYXNzZXJ0aW9uIGZhaWxlZDogV0lUSElOX0ZSRUVMSVNUKGxpc3QpAGFzc2VydGlvbiBmYWlsZWQ6IFdJVEhJTl9BUkVOQShwdHIpAGFzc2VydGlvbiBmYWlsZWQ6IHRlbXAtPm5leHQgPT0gTlVMTCB8fCBXSVRISU5fQVJFTkEodGVtcC0+bmV4dCkAYXNzZXJ0aW9uIGZhaWxlZDogKGNoYXIgKiopdGVtcC0+bmV4dC0+cF9uZXh0ID09IGxpc3QAYXNzZXJ0aW9uIGZhaWxlZDogbGlzdCA+PSAwICYmIGxpc3QgPCBzaC5mcmVlbGlzdF9zaXplAGFzc2VydGlvbiBmYWlsZWQ6ICgocHRyIC0gc2guYXJlbmEpICYgKChzaC5hcmVuYV9zaXplID4+IGxpc3QpIC0gMSkpID09IDAAYXNzZXJ0aW9uIGZhaWxlZDogYml0ID4gMCAmJiBiaXQgPCBzaC5iaXR0YWJsZV9zaXplAGFzc2VydGlvbiBmYWlsZWQ6ICFURVNUQklUKHRhYmxlLCBiaXQpAGFzc2VydGlvbiBmYWlsZWQ6ICFzaF90ZXN0Yml0KHRlbXAsIHNsaXN0LCBzaC5iaXRtYWxsb2MpAGFzc2VydGlvbiBmYWlsZWQ6IFdJVEhJTl9GUkVFTElTVCh0ZW1wMi0+cF9uZXh0KSB8fCBXSVRISU5fQVJFTkEodGVtcDItPnBfbmV4dCkAYXNzZXJ0aW9uIGZhaWxlZDogdGVtcCAhPSBzaC5mcmVlbGlzdFtzbGlzdF0AYXNzZXJ0aW9uIGZhaWxlZDogc2guZnJlZWxpc3Rbc2xpc3RdID09IHRlbXAAYXNzZXJ0aW9uIGZhaWxlZDogdGVtcC0oc2guYXJlbmFfc2l6ZSA+PiBzbGlzdCkgPT0gc2hfZmluZF9teV9idWRkeSh0ZW1wLCBzbGlzdCkAYXNzZXJ0aW9uIGZhaWxlZDogc2hfdGVzdGJpdChjaHVuaywgbGlzdCwgc2guYml0dGFibGUpAGFzc2VydGlvbiBmYWlsZWQ6IFdJVEhJTl9BUkVOQShjaHVuaykAYXNzZXJ0aW9uIGZhaWxlZDogKGJpdCAmIDEpID09IDAAYXNzZXJ0aW9uIGZhaWxlZDogc2hfdGVzdGJpdChwdHIsIGxpc3QsIHNoLmJpdHRhYmxlKQBhc3NlcnRpb24gZmFpbGVkOiBURVNUQklUKHRhYmxlLCBiaXQpAGFzc2VydGlvbiBmYWlsZWQ6IHB0ciA9PSBzaF9maW5kX215X2J1ZGR5KGJ1ZGR5LCBsaXN0KQBhc3NlcnRpb24gZmFpbGVkOiBwdHIgIT0gTlVMTABhc3NlcnRpb24gZmFpbGVkOiAhc2hfdGVzdGJpdChwdHIsIGxpc3QsIHNoLmJpdG1hbGxvYykAYXNzZXJ0aW9uIGZhaWxlZDogc2guZnJlZWxpc3RbbGlzdF0gPT0gcHRyAEZBTFNFAFRSVUUAdHJ1ZQBZAHkAWUVTAHllcwBmYWxzZQBOAG4ATk8Abm8Ac2VjdGlvbjoALG5hbWU6ACx2YWx1ZToAWDUwOV9BVFRSSUJVVEUAb2JqZWN0AHNldAAKADoAJTAyeCVzAFg1MDlfUFVCS0VZAGFsZ29yAHB1YmxpY19rZXkAL3Vzci9sb2NhbC9zc2wAYXNzZXJ0aW9uIGZhaWxlZDogY3R4LT5kaWdlc3QtPm1kX3NpemUgPD0gRVZQX01BWF9NRF9TSVpFAGFsZ19zZWN0aW9uAGZpcHNfbW9kZQAwACV1ACUwOXUALQBzc2xfY29uZgBzZWN0aW9uPQBuYW1lPQBFTlYAYXNzZXJ0aW9uIGZhaWxlZDogdnYgPT0gTlVMTABncm91cD0AIG5hbWU9AG9wZW5zc2xfY29uZgBwYXRoAE9QRU5TU0xfaW5pdABPUEVOU1NMX2ZpbmlzaABtb2R1bGU9ACwgcGF0aD0AJS04ZAAsIHZhbHVlPQAsIHJldGNvZGU9AE9QRU5TU0xfQ09ORgAvAG9wZW5zc2wuY25mAE9wZW5TU0wgZGVmYXVsdAByYgBbJXNdICVzPSVzCgBbWyVzXV0KAGRlZmF1bHQAJWxkAGxpbmUgAAABAgQHAwYF";var tempDoublePtr=82992;function demangle(A){return A}function demangleAll(A){return A.replace(/\b__Z[\w\d_]+/g,(function(A){var e=demangle(A);return A===e?A:e+" ["+A+"]"}))}function jsStackTrace(){var A=new Error;if(!A.stack){try{throw new Error}catch(e){A=e}if(!A.stack)return"(no stack trace available)"}return A.stack.toString()}var ENV={};function __getExecutableName(){return thisProgram||"./this.program"}function ___buildEnvironment(A){var e,r;___buildEnvironment.called?(r=HEAP32[A>>2],e=HEAP32[r>>2]):(___buildEnvironment.called=!0,ENV.USER="web_user",ENV.LOGNAME="web_user",ENV.PATH="/",ENV.PWD="/",ENV.HOME="/home/web_user",ENV.LANG=("object"==typeof navigator&&navigator.languages&&navigator.languages[0]||"C").replace("-","_")+".UTF-8",ENV._=__getExecutableName(),e=getMemory(1024),r=getMemory(256),HEAP32[r>>2]=e,HEAP32[A>>2]=r);var i=[],n=0;for(var t in ENV)if("string"==typeof ENV[t]){var f=t+"="+ENV[t];i.push(f),n+=f.length}if(n>1024)throw new Error("Environment size exceeded TOTAL_ENV_SIZE!");for(var a=0;a<i.length;a++){writeAsciiToMemory(f=i[a],e),HEAP32[r+4*a>>2]=e,e+=f.length+1}HEAP32[r+4*i.length>>2]=0}var PATH={splitPath:function(A){return/^(\/?|)([\s\S]*?)((?:\.{1,2}|[^\/]+?|)(\.[^.\/]*|))(?:[\/]*)$/.exec(A).slice(1)},normalizeArray:function(A,e){for(var r=0,i=A.length-1;i>=0;i--){var n=A[i];"."===n?A.splice(i,1):".."===n?(A.splice(i,1),r++):r&&(A.splice(i,1),r--)}if(e)for(;r;r--)A.unshift("..");return A},normalize:function(A){var e="/"===A.charAt(0),r="/"===A.substr(-1);return(A=PATH.normalizeArray(A.split("/").filter((function(A){return!!A})),!e).join("/"))||e||(A="."),A&&r&&(A+="/"),(e?"/":"")+A},dirname:function(A){var e=PATH.splitPath(A),r=e[0],i=e[1];return r||i?(i&&(i=i.substr(0,i.length-1)),r+i):"."},basename:function(A){if("/"===A)return"/";var e=A.lastIndexOf("/");return-1===e?A:A.substr(e+1)},extname:function(A){return PATH.splitPath(A)[3]},join:function(){var A=Array.prototype.slice.call(arguments,0);return PATH.normalize(A.join("/"))},join2:function(A,e){return PATH.normalize(A+"/"+e)}},SYSCALLS={mappings:{},buffers:[null,[],[]],printChar:function(A,e){var r=SYSCALLS.buffers[A];0===e||10===e?((1===A?out:err)(UTF8ArrayToString(r,0)),r.length=0):r.push(e)},varargs:void 0,get:function(){return SYSCALLS.varargs+=4,HEAP32[SYSCALLS.varargs-4>>2]},getStr:function(A){return UTF8ToString(A)},get64:function(A,e){return A}};function ___sys_getegid32(){return 0}function ___sys_getuid32(){return ___sys_getegid32()}function ___syscall199(){return ___sys_getuid32()}function ___sys_getgid32(){return ___sys_getegid32()}function ___syscall200(){return ___sys_getgid32()}function ___sys_geteuid32(){return ___sys_getegid32()}function ___syscall201(){return ___sys_geteuid32()}function ___syscall202(){return ___sys_getegid32()}function _abort(){abort()}function _atexit(A,e){__ATEXIT__.unshift({func:A,arg:e})}function _emscripten_asm_const_int(){}function _emscripten_get_heap_size(){return HEAPU8.length}function abortOnCannotGrowMemory(A){abort("OOM")}function _emscripten_resize_heap(A){abortOnCannotGrowMemory(A>>>=0)}function _getenv(A){return 0===A?0:(A=UTF8ToString(A),ENV.hasOwnProperty(A)?(_getenv.ret&&_free(_getenv.ret),_getenv.ret=allocateUTF8(ENV[A]),_getenv.ret):0)}function _emscripten_memcpy_big(A,e,r){HEAPU8.copyWithin(A,e,e+r)}function _time(A){var e=Date.now()/1e3|0;return A&&(HEAP32[A>>2]=e),e}var __readAsmConstArgsArray=[],ASSERTIONS=!1;function intArrayToString(A){for(var e=[],r=0;r<A.length;r++){var i=A[r];i>255&&(ASSERTIONS&&assert(!1,"Character code "+i+" ("+String.fromCharCode(i)+")  at offset "+r+" not in 0x00-0xFF."),i&=255),e.push(String.fromCharCode(i))}return e.join("")}var decodeBase64="function"==typeof atob?atob:function(A){var e,r,i,n,t,f,a="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=",B="",o=0;A=A.replace(/[^A-Za-z0-9\+\/\=]/g,"");do{e=a.indexOf(A.charAt(o++))<<2|(n=a.indexOf(A.charAt(o++)))>>4,r=(15&n)<<4|(t=a.indexOf(A.charAt(o++)))>>2,i=(3&t)<<6|(f=a.indexOf(A.charAt(o++))),B+=String.fromCharCode(e),64!==t&&(B+=String.fromCharCode(r)),64!==f&&(B+=String.fromCharCode(i))}while(o<A.length);return B};function intArrayFromBase64(A){if("boolean"==typeof ENVIRONMENT_IS_NODE&&ENVIRONMENT_IS_NODE){var e;try{e=Buffer.from(A,"base64")}catch(r){e=new Buffer(A,"base64")}return new Uint8Array(e.buffer,e.byteOffset,e.byteLength)}try{for(var r=decodeBase64(A),i=new Uint8Array(r.length),n=0;n<r.length;++n)i[n]=r.charCodeAt(n);return i}catch(A){throw new Error("Converting base64 string to bytes failed.")}}function tryParseAsDataURI(A){if(isDataURI(A))return intArrayFromBase64(A.slice(dataURIPrefix.length))}var asmGlobalArg={Math:Math,Int8Array:Int8Array,Int16Array:Int16Array,Int32Array:Int32Array,Uint8Array:Uint8Array,Uint16Array:Uint16Array,Float32Array:Float32Array,Float64Array:Float64Array},asmLibraryArg={a:abort,b:setTempRet0,c:getTempRet0,d:___buildEnvironment,e:___sys_getegid32,f:___sys_geteuid32,g:___sys_getgid32,h:___sys_getuid32,i:___syscall199,j:___syscall200,k:___syscall201,l:___syscall202,m:__getExecutableName,n:_abort,o:_atexit,p:_emscripten_asm_const_i,q:_emscripten_asm_const_int,r:_emscripten_get_heap_size,s:_emscripten_memcpy_big,t:_emscripten_resize_heap,u:_getenv,v:_time,w:tempDoublePtr},asm=function(A,e,r){var i=new A.Int8Array(r),n=new A.Int16Array(r),t=new A.Int32Array(r),f=new A.Uint8Array(r),a=new A.Uint16Array(r),B=(new A.Float32Array(r),new A.Float64Array(r)),o=(e.w,A.Math.imul),u=A.Math.clz32,l=e.a,c=e.b,E=e.c,Q=e.d,w=(e.e,e.f,e.g,e.h,e.i),b=e.j,k=e.k,s=e.l,C=(e.m,e.n),d=e.o,I=e.p,g=(e.q,e.r),D=e.s,M=e.t,h=e.u,U=e.v,G=83008;function Z(){I(1)}function N(A,e){A|=0,e|=0;var r=0;for(r=0;(0|r)!=(0|e);)i[A+r>>0]=0|I(0),r=r+1|0}function F(A,e,r,i,n,f,a){e|=0,r|=0,i|=0,n|=0,f|=0,a|=0;var B,o,u,l,c,E=0,Q=0;c=G,G=G+80|0,(0|(B=0|pi(A|=0)))<=-1&&oi(53850,82796,0),o=0|di(),u=0|di(),l=0|di();A:do{if(0==(0|o)|0==(0|u)|0==(0|l)||(ji(l,8),E=0|$i(855,0,e,r),0==(0|E)))E=0,f=0;else if(0!=(0|mi(l,0,A,0,E))&&0!=(0|hi(o,l))){if(0|i&&0==(0|Di(o,i,n))){f=0;break}if(0!=(0|Ji(o,c,c+68|0))&&0!=(0|hi(o,l))){e:do{if(0|i)for(;;){if(!(0|Di(o,c,0|t[c+68>>2]))){f=0;break A}if((A=a>>>0>B>>>0)&&0==(0|hi(u,o))){f=0;break A}if(!(0|Di(o,i,n))){f=0;break A}if(!A)break e;if(!(0|Ji(o,f,c+64|0))){Q=27;break e}if(A=0|t[c+64>>2],!(0|Ji(u,c,c+68|0))){Q=27;break e}if(!(0|hi(o,l))){f=0;break A}a=a-A|0,f=f+A|0}else for(A=a;;){if(!(0|Di(o,c,0|t[c+68>>2]))){f=0;break A}if(A>>>0<=B>>>0){a=A;break e}if(!(0|hi(u,o))){f=0;break A}if(!(0|Ji(o,f,c+64|0))){Q=27;break e}if(a=0|t[c+64>>2],!(0|Ji(u,c,c+68|0))){Q=27;break e}if(!(0|hi(o,l))){f=0;break A}A=A-a|0,f=f+a|0}}while(0);if(27==(0|Q)){f=0;break}0|Ji(o,c,c+68|0)?(If(0|f,0|c,0|a),f=1):f=0}else f=0}else f=0}while(0);return yi(E),Ii(o),Ii(u),Ii(l),$r(c,64),G=c,0|f}function W(){var A=0;return(A=0|Sn(4,82796,0))?t[A>>2]=1:A=0,0|A}function Y(A){return 1==(0|t[(A|=0)>>2])?1:(oi(53879,82796,0),0)}function R(A){return 1==(0|t[(A|=0)>>2])?1:(oi(53879,82796,0),0)}function y(A){return 1==(0|t[(A|=0)>>2])?1:(oi(53879,82796,0),0)}function V(A){(A|=0)&&(t[A>>2]=0,Xn(A,82796,0))}function m(A,e){return e|=0,0|t[(A|=0)>>2]||(ZB[15&e](),t[A>>2]=1),1}function H(A,e){return A|=0,e|=0,(e=0|t[20508])>>>0>255?0|(A=0):(t[20508]=e+1,t[A>>2]=e,t[80880+(e<<2)>>2]=0,0|(A=1))}function v(A){return(A=0|t[(A|=0)>>2])>>>0>255?0|(A=0):0|(A=0|t[80880+(A<<2)>>2])}function J(A,e){return e|=0,(A=0|t[(A|=0)>>2])>>>0>255?0|(e=0):(t[80880+(A<<2)>>2]=e,0|(e=1))}function X(A){return t[(A|=0)>>2]=257,1}function T(A,e,r,i){return e|=0,r|=0,i|=0,i=(0|t[(A|=0)>>2])+e|0,t[A>>2]=i,t[r>>2]=i,1}function L(A,e,r){A|=0,e|=0,r|=0;var i;return i=G,G=G+16|0,t[i>>2]=A,0|t[A+4>>2]&&(x(i,0,0),A=0|t[i>>2]),t[A>>2]=e,1==(0|e)?(t[A+4>>2]=0==(0|r)?0:255,void(G=i)):(t[A+4>>2]=r,void(G=i))}function S(A,e){A|=0;var r=0;return 0|(r=0!=(0|(e|=0))&&16==(0|t[e>>2])&&0!=(0|(r=0|t[e+4>>2]))?0|function(A,e){A|=0,e|=0;var r;if(r=G,G=G+16|0,t[r>>2]=t[A+8>>2],0|(e=0|hA(0,r,0|t[A>>2],e)))return G=r,0|e;return Xe(13,199,110,0,0),G=r,0|e}(r,A):0)}function p(A,e,r){A|=0,e|=0;var i,n=0;if((0==(0|(r|=0))||0==(0|(n=0|t[r>>2])))&&!(n=0|function(){var A=0;return(A=0|Sn(16,82796,0))?(t[A+4>>2]=4,0|A):(Xe(13,130,65,0,0),0|(A=0))}()))return Xe(13,198,65,0,0),0|(r=0);Xn(0|t[(i=n+8|0)>>2],82796,0),t[i>>2]=0,A=0|TA(A,i,e),t[n>>2]=A;do{if(A){if(!(0|t[i>>2])){Xe(13,198,65,0,0);break}return r?(0|t[r>>2]||(t[r>>2]=n),0|(r=n)):0|(r=n)}Xe(13,198,112,0,0)}while(0);return 0|r&&0|t[r>>2]||cA(n),0|(r=0)}function z(A,e){A|=0,e|=0;var r;r=G,G=G+16|0,t[r>>2]=A,K(r,e,0),G=r}function K(A,e,r){A|=0,r|=0;var n=0,f=0,a=0,B=0,o=0;if(f=0|t[(e|=0)+16>>2],A&&!((a=0|i[e>>0])<<24>>24&&0==(0|t[A>>2])))switch(0!=(0|f)&&0!=(0|(n=0|t[f+16>>2]))||(n=0),a<<24>>24|0){case 0:return(n=0|t[e+8>>2])?void j(A,n):void x(A,e,r);case 5:return void x(A,e,r);case 2:if((a=0!=(0|n))&&2==(0|MB[31&n](2,A,e,0)))return;if((0|(f=0|qA(A,e)))>-1&&(0|f)<(0|t[e+12>>2])&&j(0|ne(A,o=(0|t[e+8>>2])+(20*f|0)|0),o),a&&MB[31&n](3,A,e,0),0|r)return;return Xn(0|t[A>>2],82796,0),void(t[A>>2]=0);case 4:if(!f)return;if(!(n=0|t[f+8>>2]))return;return void FB[7&n](A,e);case 1:case 6:if(0|Ae(A,-1,e))return;if((o=0!=(0|n))&&2==(0|MB[31&n](2,A,e,0)))return;if(function(A,e){e|=0;var r;if(!(A|=0))return;if(!(r=0|t[A>>2]))return;if(!(A=0|t[e+16>>2]))return;if(!(2&t[A+4>>2]))return;if(!(A=r+(0|t[A+20>>2])|0))return;Xn(0|t[A>>2],82796,0),t[A>>2]=0,t[A+4>>2]=0,t[A+8>>2]=1}(A,e),(0|(f=0|t[e+12>>2]))>0){B=0,a=(0|t[e+8>>2])+(20*f|0)|0;do{0|(f=0|te(A,a=a+-20|0,0))&&j(0|ne(A,f),f),B=B+1|0}while((0|B)<(0|t[e+12>>2]))}if(o&&MB[31&n](3,A,e,0),0|r)return;return Xn(0|t[A>>2],82796,0),void(t[A>>2]=0);default:return}}function j(A,e){A|=0;var r,i,n,f=0;if(n=G,G=G+16|0,4096&(i=0|t[(e|=0)>>2])&&(t[n+4>>2]=A,A=n+4|0),!(6&i))return K(A,0|t[e+16>>2],4096&i),void(G=n);if((0|Hn(r=0|t[A>>2]))>0){f=0;do{t[n>>2]=0|vn(r,f),K(n,0|t[e+16>>2],4096&i),f=f+1|0}while((0|f)<(0|Hn(r)))}Fn(r),t[A>>2]=0,G=n}function x(A,e,r){A|=0,e|=0,r|=0;var n=0,f=0,a=0;do{if(0|e){if(n=0|t[e+16>>2],r){if(0|n&&0|(f=0|t[n+16>>2]))return void FB[7&f](A,e)}else if(0|n&&0|(a=0|t[n+12>>2]))return void FB[7&a](A,e);if(5==(0|i[e>>0])){if(0|t[A>>2]){f=20;break}return}if(1!=(0|(n=0|t[e+4>>2]))){if(!(0|t[A>>2]))return;f=14}else f=17}else{if(n=0|t[A>>2],!(0|t[n+4>>2]))return;A=n+4|0,n=0|t[n>>2],f=14}}while(0);A:do{if(14==(0|f))switch(0|n){case 5:break A;case 6:$(0|t[A>>2]);break A;case 1:if(0|e){f=17;break A}return void(t[A>>2]=-1);case-4:x(A,0,0),Xn(0|t[A>>2],82796,0);break A;default:f=20;break A}}while(0);17!=(0|f)?(20==(0|f)&&function(A,e){if(e|=0,!(A|=0))return;16&t[A+12>>2]||Xn(0|t[A+8>>2],82796,0);if(0|e)return;Xn(A,82796,0)}(0|t[A>>2],r),t[A>>2]=0):t[A>>2]=t[e+20>>2]}function O(A,e){K(A|=0,e|=0,0)}function P(A,e,r,n){A|=0,e|=0,r|=0;var t,f,a=0,B=0,o=0,u=0,l=0,c=0,E=0,Q=0,w=0,b=0,k=0;switch(f=G,G=G+32|0,0|(n|=0)){case-1:a=0|mt(r);break;case 0:return G=f,0|(k=n);default:a=n}t=0|i[r>>0];A:do{if((t+-48&255)<3){if((0|a)<2){Xe(13,100,138,0,0),n=0;break}do{if(2!=(0|a)){n=0,B=0|i[r+1>>0],w=0,o=a+-2|0,r=r+2|0,a=f,l=24;e:for(;;){switch(0|B){case 32:case 46:break;default:k=11;break e}B=0,E=o,b=r,r=0;r:for(;;){switch(Q=E+-1|0,c=b,b=b+1|0,(c=0|i[c>>0])<<24>>24){case 32:case 46:break r}if((c+-48&255)>9){k=14;break e}if((o=0==(0|r))&B>>>0>429496720){if(!n&&!(n=0|qf())){n=0,k=52;break e}if(!(0|ta(n,B))){k=52;break e}r=1,k=20}else o?(B=(c<<24>>24)-48+(10*B|0)|0,r=0):k=20;if(20==(0|k)){if(k=0,!(0|QB(n,10))){k=52;break e}if(!(0|cB(n,(c<<24>>24)-48|0))){k=52;break e}}if((0|E)<2)break;E=Q}do{if(!w){if(t<<24>>24<50&B>>>0>39){k=26;break e}if(r){if(0|cB(n,(40*(t<<24>>24)|0)-1920|0)){k=32;break}k=52;break e}r=B+((40*(t<<24>>24)|0)-1920)|0,k=31;break}r?k=32:(r=B,k=31)}while(0);if(31==(0|k))for(k=0,o=0;;){if(B=o+1|0,i[a+o>>0]=127&r,!(r>>>=7)){u=a;break}o=B}else if(32==(0|k)){if(k=0,(0|((r=0|Kf(n))+6|0)/7)>(0|l)){if((0|a)!=(0|f)&&Xn(a,82796,0),!(a=0|Ln(32+((r+6|0)/7|0)|0,82796,0))){a=0;break}l=32+((r+6|0)/7|0)|0}if((r+12|0)>>>0<13)B=0,u=a;else for(o=(r+6|0)/7|0,u=0;;){if(o=o+-1|0,-1==(0|(r=0|lB(n,128)))){k=52;break e}if(B=u+1|0,i[a+u>>0]=r,!o){u=a;break}u=B}}if(a=B+w|0,A){if((0|a)>(0|e)){k=43;break}if((0|B)>1)for(r=w;w=B,B=B+-1|0,a=r+1|0,i[A+r>>0]=-128|i[u+B>>0],!((0|w)<=2);)r=a;else a=w;i[A+a>>0]=0|i[u>>0],a=a+1|0}if((0|E)<2){k=49;break}B=c<<24>>24,w=a,o=Q,r=b,a=u}if(11==(0|k))Xe(13,100,131,0,0),k=52;else if(14==(0|k))Xe(13,100,130,0,0),k=52;else if(26==(0|k))Xe(13,100,147,0,0),k=52;else if(43==(0|k))Xe(13,100,107,0,0),a=u,k=52;else if(49==(0|k)){if((0|u)==(0|f))break;Xn(u,82796,0);break}if(52==(0|k)&&(0|a)==(0|f))break A;Xn(a,82796,0);break A}n=0,a=0}while(0);return Pf(n),G=f,0|(k=a)}Xe(13,100,122,0,0),n=0}while(0);return Pf(n),G=f,0|(k=0)}function _(A,e){A|=0,e|=0;var r,i=0,n=0,f=0;r=G,G=G+80|0;do{if(0!=(0|e)&&0!=(0|t[e+16>>2])){if((0|(i=0|Or(r,80,e,0)))<=79){if((0|i)<1){i=0|se(A,54001,9),i=(0|be(A,0|t[e+16>>2],0|t[e+12>>2]))+i|0;break}se(A,r,i);break}if(!(n=0|Ln(i+1|0,82796,0))){i=-1;break}if(Or(n,i+1|0,e,0),se(A,n,i),(0|n)==(0|r))break;Xn(n,82796,0);break}f=3}while(0);return 3==(0|f)&&(i=0|se(A,64038,4)),G=r,0|i}function q(A,e,r){A|=0;var n,f=0,a=0,B=0,o=0,u=0,l=0;if(n=G,G=G+32|0,!(0==(0|(e|=0))|(0|(r|=0))<1)&&0|(B=0|t[e>>2])&&(0|i[B+(r+-1)>>0])>=0){if(t[n+8>>2]=0,t[n+16>>2]=B,t[n+12>>2]=r,t[n+20>>2]=0,0|(a=0|zr(n)))return f=0|Sr(a),0|A&&($(0|t[A>>2]),t[A>>2]=f),t[e>>2]=(0|t[e>>2])+r,G=n,0|(e=f);for(o=0,a=B;;){if(-128==(0|i[a>>0])){if(!o){l=13;break}if((0|i[a+-1>>0])>=0){l=13;break}}if((0|(o=o+1|0))>=(0|r))break;a=a+1|0}if(13==(0|l))return Xe(13,196,216,0,0),G=n,0|(e=0);0!=(0|A)&&0!=(0|(f=0|t[A>>2]))&&0!=(1&t[f+20>>2]|0)||(l=18);do{if(18==(0|l)){if(0|(f=0|Sn(24,82796,0))){t[f+20>>2]=1;break}return Xe(13,123,65,0,0),G=n,0|(e=0)}}while(0);o=0|t[e>>2],a=0|t[(u=f+16|0)>>2],t[u>>2]=0,B=f+12|0,0!=(0|a)&&(0|t[B>>2])>=(0|r)||(l=23);do{if(23==(0|l)){if(t[B>>2]=0,Xn(a,82796,0),0|(a=0|Ln(r,82796,0))){t[(l=f+20|0)>>2]=8|t[l>>2];break}return Xe(13,196,65,0,0),0|A&&(0|t[A>>2])==(0|f)?(G=n,0|(e=0)):($(f),G=n,0|(e=0))}}while(0);return If(0|a,0|o,0|r),t[u>>2]=a,t[B>>2]=r,t[f>>2]=0,t[f+4>>2]=0,0|A&&(t[A>>2]=f),t[e>>2]=o+r,G=n,0|(e=f)}return Xe(13,196,216,0,0),G=n,0|(e=0)}function $(A){var e=0;(A|=0)&&(4&(e=0|t[A+20>>2])&&(Xn(0|t[A>>2],82796,0),Xn(0|t[A+4>>2],82796,0),t[A+4>>2]=0,t[A>>2]=0,e=0|t[A+20>>2]),8&e&&(Xn(0|t[A+16>>2],82796,0),t[A+16>>2]=0,t[A+12>>2]=0,e=0|t[A+20>>2]),1&e&&Xn(A,82796,0))}function AA(A,e){return 0|TA(A|=0,e|=0,51980)}function eA(A){cA(A|=0)}function rA(){return 0|EA(2)}function iA(){return 0|pA(52092)}function nA(A){z(A|=0,52092)}function tA(A,e,r,n,a){e|=0,r|=0,n|=0,a|=0;var B,o=0,u=0,l=0,c=0,E=0;B=0|t[(A|=0)>>2];A:do{if(0|a){if(31==(31&(E=0|f[B>>0])|0)){if(!(a+-1|0))break;if((o=0|i[B+1>>0])<<24>>24<0)for(c=0==(a+-2|0),u=a+-2|0,l=B+2|0,o&=127;;){if(c|(0|o)>16777215)break A;if(c=0|i[l>>0],l=l+1|0,o=o<<7|127&c,u=u+-1|0,!(c<<24>>24<0)){c=0==(0|u);break}c=0==(0|u)}else c=0==(a+-2|0),u=a+-2|0,l=B+2|0,o&=127;if(c)break;c=u,u=l}else{if(!(a+-1|0))break;c=a+-1|0,u=B+1|0,o=31&E}if(t[r>>2]=o,t[n>>2]=192&E,(0|c)>=1){if((l=0|i[u>>0])<<24>>24==-128){if(t[e>>2]=0,!(32&E))break;o=u+1|0,u=0,c=1}else{u=u+1|0;e:do{if(128&l){if((1+(127&l)|0)>=(0|c))break A;if(127&l){for(o=127&l,n=u;!((l=0|i[n>>0])<<24>>24);){if(u=n+1|0,!(o=o+-1|0)){o=u,u=0;break e}n=u}if(o>>>0>4)break A;if(u=o+-1|0){c=u,u=255&l,l=n;do{c=c+-1|0,u=u<<8|0|f[(l=l+1|0)>>0]}while(0!=(0|c))}else u=255&l;if((0|u)<0)break A;o=n+o|0}else o=u,u=0}else o=u,u=127&l}while(0);t[e>>2]=u,c=0}return(0|u)>(B+a-(l=o)|0)?(Xe(13,114,155,0,0),o=32&E|128):o=32&E,t[A>>2]=l,0|(A=o|c)}}}while(0);return Xe(13,114,123,0,0),0|(A=128)}function fA(A,e,r,n,f){e|=0,r|=0,n|=0,f|=0;var a,B=0,o=0,u=0;if(a=0|t[(A|=0)>>2],f=192&f|(0==(0|e)?0:32),(0|n)<31)i[a>>0]=f|31&n,o=a+1|0;else{for(i[a>>0]=31|f,f=0,B=n;o=f+1|0,B>>>=7;)f=o;for(B=o;u=127&n,i[a+B>>0]=(0|B)==(0|o)?u:-128|u,(0|f)>0;)B=f,f=f+-1|0,n>>=7;o=a+1+o|0}if(2==(0|e))return i[o>>0]=-128,u=o+1|0,void(t[A>>2]=u);if((0|r)<128)return i[o>>0]=r,u=o+1|0,void(t[A>>2]=u);B=0,f=r;do{f>>>=8,B=B+1|0}while(0!=(0|f));for(i[o>>0]=128|B,n=B,f=r;i[o+n>>0]=f,(0|n)>1;)n=n+-1|0,f>>=8;u=o+1+B|0,t[A>>2]=u}function aA(A){var e;return e=0|t[(A|=0)>>2],i[e>>0]=0,i[e+1>>0]=0,t[A>>2]=e+2,2}function BA(A,e,r){A|=0;var i=0;if((0|(e|=0))<0)return-1;if((0|(r|=0))>30){i=1;do{r>>>=7,i=i+1|0}while(0!=(0|r))}else i=1;if(2!=(0|A)){if(i=i+1|0,(0|e)>127){r=e;do{r>>>=8,i=i+1|0}while(0!=(0|r))}}else i=i+3|0;return 0|((0|i)<(2147483647-e|0)?i+e|0:-1)}function oA(A,e){var r=0,n=0,f=0,a=0;if(!(e|=0))return 0|(e=0);t[(A|=0)+4>>2]=t[e+4>>2],a=0|t[e+8>>2],r=0|t[e>>2];do{if((0|r)<0){if(a){r=0|mt(a);break}return 0|(e=0)}}while(0);if(n=0|t[A+8>>2],(0|t[A>>2])>(0|r)?n||(n=0,f=7):f=7,7==(0|f)){if(f=0|Tn(n,r+1|0,82796,0),t[A+8>>2]=f,!f)return Xe(13,186,65,0,0),t[A+8>>2]=n,0|(e=0);n=f}return t[A>>2]=r,0|a&&(If(0|n,0|a,0|r),i[(0|t[A+8>>2])+r>>0]=0),a=128&t[A+12>>2],t[A+12>>2]=a,t[A+12>>2]=-129&t[e+12>>2]|a,0|(e=1)}function uA(A,e,r){A|=0,e|=0,r|=0;var n=0,f=0;do{if((0|r)<0){if(e){r=0|mt(e);break}return 0|(A=0)}}while(0);if(n=0|t[A+8>>2],(0|t[A>>2])>(0|r)?n||(n=0,f=6):f=6,6==(0|f)){if(f=0|Tn(n,r+1|0,82796,0),t[A+8>>2]=f,!f)return Xe(13,186,65,0,0),t[A+8>>2]=n,0|(A=0);n=f}return t[A>>2]=r,e?(If(0|n,0|e,0|r),i[(0|t[A+8>>2])+r>>0]=0,0|(A=1)):0|(A=1)}function lA(A){var e=0;return(A|=0)?(e=0|Sn(16,82796,0))?(t[e+4>>2]=4,0|oA(e,A)?0|e:(16&(A=0|t[e+12>>2])||Xn(0|t[e+8>>2],82796,0),128&A|0||Xn(e,82796,0),0|(e=0))):(Xe(13,130,65,0,0),0|(e=0)):0|(e=0)}function cA(A){var e;(A|=0)&&(16&(e=0|t[A+12>>2])||Xn(0|t[A+8>>2],82796,0),128&e|0||Xn(A,82796,0))}function EA(A){A|=0;var e;return(e=0|Sn(16,82796,0))?(t[e+4>>2]=A,0|(A=e)):(Xe(13,130,65,0,0),0|(A=0))}function QA(A,e,r,i,n){A|=0,e|=0,r|=0,i|=0,n|=0;var f,a=0,B=0,o=0,u=0;f=G,G=G+32|0,(a=0|t[20509])?u=3:(a=0|function(A){A|=0;var e,r=0;if(0|(e=0|Sn(20,82796,0))?(r=0|Sn(16,82796,0),t[e+4>>2]=r,0|r):0)return t[e+16>>2]=A,t[e+12>>2]=4,0|(r=e);return Xn(e,82796,0),0|(r=0)}(23),t[20509]=a,0|a&&(u=3));A:do{if(3==(0|u)){t[f>>2]=A,(o=0!=(0|(B=(0|(a=0|Rn(a,f)))>-1?0|vn(0|t[20509],a):0|Pr(f,64,24,20,24))))&&0!=(1&t[B+16>>2]|0)?a=B:u=8;do{if(8==(0|u)){if(!(a=0|Sn(20,82796,0)))break A;if(!(0|yn(0|t[20509],a))){Xn(a,82796,0);break A}if(o){t[a>>2]=t[B>>2],t[a+4>>2]=t[B+4>>2],t[a+8>>2]=t[B+8>>2],t[a+12>>2]=t[B+12>>2],t[a+16>>2]=1|t[B+16>>2];break}t[a>>2]=A,t[a+4>>2]=-1,t[a+8>>2]=-1,t[a+16>>2]=1;break}}while(0);return(0|e)>-1&&(t[a+4>>2]=e),(0|r)>-1&&(t[a+8>>2]=r),0|i&&(t[a+12>>2]=i),n?(t[a+16>>2]=1|n,G=f,0|(e=1)):(G=f,0|(e=1))}}while(0);return Xe(13,129,65,0,0),G=f,0|(e=0)}function wA(A,e){e|=0;var r,n=0,a=0,B=0,o=0,u=0,l=0,c=0;if(0!=(0|(r=0|t[(A|=0)+8>>2]))&0!=(0|(B=0|t[A>>2]))){if(n=0|i[r>>0],A=0==(256&t[A+4>>2]|0))n=A&n<<24>>24<0&1,A=0;else if((255&n)<=128)if(n<<24>>24==-128){if(B>>>0>1){A=1,n=0;do{n=0|n|f[r+A>>0],A=A+1|0}while((0|A)!=(0|B));A=(0!=(0|n))<<31>>31}else A=0;n=1&A}else n=0,A=-1;else n=1,A=-1;c=B,a=n,l=n+B|0}else c=0,a=0,A=0,l=1;if(!e)return 0|l;if(!(n=0|t[e>>2]))return 0|l;if(i[n>>0]=A,0|c)for(u=1&A,B=n+a+c|0,o=c,a=r+c|0;o=o+-1|0,n=u+(255&(i[(a=a+-1|0)>>0]^A))|0,i[(B=B+-1|0)>>0]=n,o;)u=n>>>8;return t[e>>2]=(0|t[e>>2])+l,0|l}function bA(A,e,r){A|=0,r|=0;var n,a,B=0,o=0,u=0,l=0,c=0;if(a=G,G=G+16|0,o=0|t[(e|=0)>>2],!r)return Xe(13,226,222,0,0),G=a,0|(e=0);if(n=0|i[o>>0],1!=(0|r)){switch(n<<24>>24){case 0:c=8;break;case-1:u=1,l=0;do{l=0|l|f[o+u>>0],u=u+1|0}while((0|u)!=(0|r));l?c=8:o=0;break;default:o=0}if(8==(0|c)){if((i[o+1>>0]^n)<<24>>24>-1)return Xe(13,226,221,0,0),G=a,0|(e=0);o=1}if(!(o=r-o|0))return G=a,0|(e=0)}else o=1;0!=(0|A)&&0!=(0|(B=0|t[A>>2]))||(c=13);do{if(13==(0|c)){if(B=0|rA()){t[B+4>>2]=2;break}return G=a,0|(e=0)}}while(0);return 0|uA(B,0,o)?(function(A,e,r,n){A|=0,e|=0,r|=0;var a=0,B=0,o=0,u=0;if(!(n|=0))return Xe(13,226,222,0,0),0|(u=0);u=0|i[r>>0],0|e&&(t[e>>2]=-128&u&255);if(1==(0|n))return A?(e=0|i[r>>0],(-128&u)<<24>>24?(i[A>>0]=1+(255&~e),0|(u=1)):(i[A>>0]=e,0|(u=1))):0|(u=1);switch(0|i[r>>0]){case 0:B=14;break;case-1:e=1,a=0;do{a|=f[r+e>>0],e=e+1|0}while((0|e)!=(0|n));a?B=14:e=0;break;default:e=0}if(14==(0|B)){if((-128&u)<<24>>24==(-128&i[r+1>>0])<<24>>24)return Xe(13,226,221,0,0),0|(u=0);e=1}if(e=n-e|0,!A)return 0|(u=e);if(!e)return 0|(u=0);a=u<<24>>24>>7&1,A=A+e|0,o=e,B=r+n|0;for(;o=o+-1|0,a=a+(255&(i[(B=B+-1|0)>>0]^u<<24>>24>>7))|0,i[(A=A+-1|0)>>0]=a,o;)a>>>=8}(0|t[B+8>>2],a,0|t[e>>2],r),0|t[a>>2]&&(t[(c=B+4|0)>>2]=256|t[c>>2]),t[e>>2]=(0|t[e>>2])+r,A?(t[A>>2]=B,G=a,0|(e=B)):(G=a,0|(e=B))):(Xe(13,194,65,0,0),0|A&&(0|t[A>>2])==(0|B)?(G=a,0|(e=0)):(function(A){cA(A|=0)}(B),G=a,0|(e=0)))}function kA(A,e,r){A|=0,r|=0;var i=0,n=0,a=0,B=0;if(!(e|=0))return Xe(13,227,67,0,0),0|(A=0);if((-257&(B=0|t[e+4>>2])|0)!=(0|r))return Xe(13,227,225,0,0),0|(A=0);if(a=0|t[e+8>>2],(e=0|t[e>>2])>>>0>8)return Xe(13,225,223,0,0),0|(A=0);if(!a)return 0|(A=0);if(e){r=0,i=0,n=0;do{r=0|Cf(0|r,0|i,8),i=0|E(),r=0|r|f[a+n>>0],n=n+1|0}while((0|n)!=(0|e))}else i=0,r=0;return e=(0|i)>-1|-1==(0|i)&r>>>0>4294967295,256&B?e?(a=0|Ef(0,0,0|r,0|i),B=0|E(),t[A>>2]=a,t[A+4>>2]=B,0|(A=1)):0==(0|r)&-2147483648==(0|i)?(t[A>>2]=0,t[A+4>>2]=-2147483648,0|(A=1)):(Xe(13,224,224,0,0),0|(A=0)):e?(t[A>>2]=r,t[A+4>>2]=i,0|(A=1)):(Xe(13,224,223,0,0),0|(A=0))}function sA(A,e){e|=0;var r,n=0,f=0;if(r=G,G=G+16|0,t[(A|=0)+4>>2]=2,(0|e)<0){n=0|Ef(0,0,0|e,((0|e)<0)<<31>>31|0),f=0|E(),e=8;do{i[r+(e=e+-1|0)>>0]=n,n=0|sf(0|n,0|f,8),f=0|E()}while(!(0==(0|n)&0==(0|f)));n=258}else{f=e,n=((0|e)<0)<<31>>31,e=8;do{i[r+(e=e+-1|0)>>0]=f,f=0|sf(0|f,0|n,8),n=0|E()}while(!(0==(0|f)&0==(0|n)));n=2}return t[A+4>>2]=n,A=0|uA(A,r+e|0,8-e|0),G=r,0|A}function CA(A){var e,r,i;return e=G,G=G+16|0,(A|=0)?0|kA(e,A,2)?(r=0|cf(0|(A=0|t[e>>2]),0|t[e+4>>2],-2147483648,0),i=0|E(),G=e,0|(A=i>>>0>0|0==(0|i)&r>>>0>4294967295?-1:A)):(G=e,0|(A=-1)):(G=e,0|(A=0))}function dA(A,e){return A|=0,t[(e|=0)>>2]=0,0|ct(A,124,1,14,e)}function IA(A,e,r,n,a){A|=0,e|=0,n|=0,a|=0;var B,o,u,l=0,c=0;if(u=G,G=G+48|0,!(r|=0))return G=u,0|(c=1);if(n=(n=0==(0|Ia(r)))?82796:80624,!(0|Ie(A,a,128)))return G=u,0|(c=0);if(0|jf(r))return t[u>>2]=e,c=(0|ge(A,54475,u))>0&1,G=u,0|c;if((0|Kf(r))<33)return a=0|t[(0|En(r))>>2],c=0|t[(0|En(r))>>2],t[u+8>>2]=e,t[u+8+4>>2]=n,t[u+8+8>>2]=a,t[u+8+12>>2]=n,t[u+8+16>>2]=c,c=(0|ge(A,54481,u+8|0))>0&1,G=u,0|c;o=0|Ln(B=1+((7+(0|Kf(r))|0)/8|0)|0,82796,0);do{if(0!=(0|o)&&(i[o>>0]=0,l=45==(0|i[n>>0])?54501:82796,t[u+32>>2]=e,t[u+32+4>>2]=l,(0|ge(A,54513,u+32|0))>=1)){r=0|ua(r,o+1|0),l=(n=0|i[o+1>>0])<<24>>24<0?o:o+1|0;A:do{if(r+((255&n)>>>7&255)|0)for(e=0;;){if(!((e>>>0)%15|0)){if(0|e&&(0|Ce(A,80479))<1)break A;if(!(0|Ie(A,a+4|0,128)))break A}if(t[u+40>>2]=f[l+e>>0],t[u+40+4>>2]=(0|e)==(r+((255&n)>>>7&255)+-1|0)?82796:80481,e=e+1|0,(0|ge(A,80483,u+40|0))<1)break A;if(e>>>0>=(r+((255&n)>>>7&255)|0)>>>0){c=17;break}}else c=17}while(0);if(17==(0|c)&&(0|se(A,80479,1))>=1){n=1;break}n=0}else n=0}while(0);return pn(o,B,82796,0),G=u,0|(c=n)}function gA(A,e){A|=0,e|=0;var r,i=0;for(r=G,G=G+128|0;;){if(t[r>>2]=r+8,t[r+8>>2]=e,!(e=0|Pr(r,52120,3,4,26))){i=6;break}if(!(e=0|t[e>>2])){e=0;break}if(!(1&t[e+8>>2]))break;e=0|t[e+4>>2]}return 6==(0|i)&&(e=0),A?(t[A>>2]=0,G=r,0|e):(G=r,0|e)}function DA(A,e){return 0|function(A,e){e|=0;var r,i=0;return 0|(r=(i=0|t[(A|=0)>>2])-(0|t[e>>2])|0)?0|(e=r):0|(i=0|Ht(0|t[A+8>>2],0|t[e+8>>2],i))?0|(e=i):0|(e=(0|t[A+4>>2])-(0|t[e+4>>2])|0)}(A|=0,e|=0)}function MA(A,e,r){return 0|uA(A|=0,e|=0,r|=0)}function hA(A,e,r,n){A|=0,e|=0,r|=0,n|=0;var f;return f=G,G=G+32|0,t[f>>2]=0,A=0==(0|A)?f:A,i[f+8>>0]=0,(0|UA(A,e,r,n,-1,0,0,f+8|0,0))<1?(O(A,n),G=f,0|(r=0)):(r=0|t[A>>2],G=f,0|r)}function UA(A,e,r,n,f,a,B,o,u){A|=0,e|=0,r|=0,n|=0,f|=0,a|=0,B|=0,o|=0,u|=0;var l,c=0,E=0,Q=0,w=0,b=0,k=0,s=0,C=0;if(l=G,G=G+48|0,t[l+28>>2]=r,E=0|t[n+16>>2],t[l+32>>2]=0,!A)return G=l,0|(n=0);C=0|E&&0!=(0|(c=0|t[E+16>>2]))?c:0;A:do{if(!((0|u)>29)){e:do{switch(0|i[n>>0]){case 0:if(!(c=0|t[n+8>>2]))return n=0|ZA(A,e,r,n,f,a,B,o),G=l,0|n;if(!(-1==(0|f)&B<<24>>24==0)){Xe(13,120,170,0,0);break A}return n=0|GA(A,e,r,c,0,o,u+1|0),G=l,0|n;case 5:if(t[l+32>>2]=t[e>>2],!(0|NA(0,l+24|0,l+38|0,0,0,l+32|0,r,-1,0,1,o))){Xe(13,120,58,0,0);break A}if(0|i[l+38>>0]){if(B<<24>>24)return G=l,0|(n=-1);Xe(13,120,139,0,0);break A}if(E=(c=0|t[l+24>>2])>>>0>30?0:0|t[1136+(c<<2)>>2],t[n+4>>2]&E|0)return n=0|ZA(A,e,0|t[l+28>>2],n,c,0,0,o),G=l,0|n;if(B<<24>>24)return G=l,0|(n=-1);Xe(13,120,140,0,0);break A;case 4:return n=0|GB[0&t[E+16>>2]](A,e,r,n,f,a,B,o),G=l,0|n;case 2:if((Q=0!=(0|C))&&0==(0|MB[31&C](4,A,n,0))){s=100;break e}if(0|t[A>>2])(0|(c=0|qA(A,n)))>-1&&(0|c)<(0|t[n+12>>2])&&(j(0|ne(A,k=(0|t[n+8>>2])+(20*c|0)|0),k),$A(A,-1,n));else if(!(0|xA(A,n))){Xe(13,120,58,0,0);break A}t[l+32>>2]=t[e>>2],E=0|t[n+12>>2];r:do{if((0|E)>0){if(-1==(0|(a=0|GA(E=0|ne(A,c=0|t[n+8>>2]),l+32|0,r,c,1,o,u+1|0))))for(E=0;;){if((0|(f=E+1|0))>=(0|(E=0|t[n+12>>2]))){c=f;break r}if(c=c+20|0,a=0|t[l+28>>2],-1!=(0|(a=0|GA(E=0|ne(A,c),l+32|0,a,c,1,o,u+1|0))))break;E=f}else f=0;if((0|a)<=0){if(j(E,c),Xe(13,120,58,0,0),c)break e;break A}E=0|t[n+12>>2],c=f;break}c=0}while(0);if((0|c)==(0|E)){if(!(B<<24>>24)){Xe(13,120,143,0,0);break A}return O(A,n),G=l,0|(n=-1)}if($A(A,c,n),Q&&0==(0|MB[31&C](5,A,n,0))){s=100;break e}return t[e>>2]=t[l+32>>2],G=l,0|(n=1);case 1:case 6:switch(t[l+32>>2]=t[e>>2],0|(c=0|NA(l+28|0,0,0,l+37|0,l+36|0,l+32|0,r,-1==(0|f)?16:f,-1==(0|f)?0:a,B,o))){case 0:Xe(13,120,58,0,0);break A;case-1:return G=l,0|(n=c);default:if(0|E&&0!=(4&t[E+4>>2]|0)?(t[l+28>>2]=r-(0|t[l+32>>2])+(0|t[e>>2]),w=1):w=0|i[l+37>>0],!(0|i[l+36>>0])){Xe(13,120,149,0,0);break A}if(0==(0|t[A>>2])&&0==(0|xA(A,n))){Xe(13,120,58,0,0);break A}if((k=0!=(0|C))&&0==(0|MB[31&C](4,A,n,0))){s=100;break e}E=0|t[n+8>>2];r:do{if((0|t[n+12>>2])>0){for(a=0;768&t[E>>2]|0&&0|(Q=0|te(A,E,0))&&j(0|ne(A,Q),Q),!((0|(a=a+1|0))>=(0|(c=0|t[n+12>>2])));)E=E+20|0;if(E=0|t[n+8>>2],(0|c)>0){for(a=0;;){if(!(c=0|te(A,E,1)))break A;if(r=0|ne(A,c),!(f=0|t[l+28>>2])){s=82;break r}if(Q=0|t[l+32>>2],(0|f)>=2&&0==(0|i[Q>>0])&&0==(0|i[Q+1>>0]))break;switch(0|GA(r,l+32|0,f,c,B=(0|a)==((0|t[n+12>>2])-1|0)?0:1&t[c>>2],o,u+1|0)){case 0:break e;case-1:j(r,c);break;default:t[l+28>>2]=Q-(0|t[l+32>>2])+(0|t[l+28>>2])}if(E=E+20|0,(0|(a=a+1|0))>=(0|t[n+12>>2])){s=82;break r}}if(t[l+32>>2]=Q+2,0|i[l+37>>0]){t[l+28>>2]=f+Q-(Q+2),i[l+37>>0]=0,c=f+Q-(Q+2)|0;break}Xe(13,120,159,0,0);break A}a=0,s=82}else a=0,s=82}while(0);do{if(82==(0|s)&&(c=0|t[l+28>>2],0|i[l+37>>0])){if((0|c)>=2&&(b=0|t[l+32>>2],0==(0|i[b>>0]))&&0==(0|i[b+1>>0])){t[l+32>>2]=b+2;break}Xe(13,120,137,0,0);break A}}while(0);if(w<<24>>24==0&0!=(0|c)){Xe(13,120,148,0,0);break A}r:do{if((0|a)<(0|t[n+12>>2])){for(;;){if(!(c=0|te(A,E,1)))break A;if(!(1&t[c>>2]))break;if(j(0|ne(A,c),c),(0|(a=a+1|0))>=(0|t[n+12>>2]))break r;E=E+20|0}Xe(13,120,121,0,0);break e}}while(0);if(!(0|re(A,s=0|t[e>>2],(c=0|t[l+32>>2])-s|0,n))){s=100;break e}if(k&&0==(0|MB[31&C](5,A,n,0))){s=100;break e}return t[e>>2]=c,G=l,0|(n=1)}default:return G=l,0|(n=0)}}while(0);if(100==(0|s)){Xe(13,120,100,0,0);break}return A=0|t[c+12>>2],n=0|t[n+24>>2],t[l>>2]=54531,t[l+4>>2]=A,t[l+8>>2]=54538,t[l+12>>2]=n,Ke(4,l),G=l,0|(n=0)}Xe(13,120,201,0,0)}while(0);return n=0|t[n+24>>2],t[l+16>>2]=54546,t[l+16+4>>2]=n,Ke(2,l+16|0),G=l,0|(n=0)}function GA(A,e,r,n,f,a,B){e|=0,r|=0,f|=0,a|=0,B|=0;var o,u=0;if(o=G,G=G+16|0,!(A|=0))return G=o,0|(e=0);if(u=0|t[(n|=0)>>2],t[o>>2]=t[e>>2],!(16&u))return e=0|WA(A,e,r,n,f,a,B),G=o,0|e;u=0|NA(o+4|0,0,0,o+9|0,o+8|0,o,r,0|t[n+4>>2],192&u,f,a),r=0|t[o>>2];A:do{switch(0|u){case 0:Xe(13,132,58,0,0),u=0;break;case-1:break;default:if(!(0|i[o+8>>0])){Xe(13,132,120,0,0),u=0;break A}if(!(0|WA(A,o,0|t[o+4>>2],n,0,a,B))){Xe(13,132,58,0,0),u=0;break A}u=0|t[o>>2],r=(0|t[o+4>>2])+(r-u)|0,t[o+4>>2]=r;do{if(0|i[o+9>>0]){if((0|r)>=2&&0==(0|i[u>>0])&&0==(0|i[u+1>>0])){t[o>>2]=u+2,u=u+2|0,r=19;break}Xe(13,132,137,0,0),r=20}else r?(Xe(13,132,119,0,0),r=20):r=19}while(0);if(19==(0|r))return t[e>>2]=u,G=o,0|(e=1);if(20==(0|r))return G=o,0|(e=0)}}while(0);return G=o,0|(e=u)}function ZA(A,e,r,n,a,B,o,u){A|=0,e|=0,r|=0,n|=0,a|=0,B|=0,o|=0,u|=0;var l,c=0,E=0,Q=0,w=0,b=0,k=0,s=0;if(l=G,G=G+48|0,i[l+44>>0]=0,t[l>>2]=0,t[l+4>>2]=0,t[l+8>>2]=0,t[l+12>>2]=0,!A)return Xe(13,108,125,0,0),G=l,0|(s=0);if(5==(0|i[n>>0])?(c=a,a=-1):c=0|t[n+4>>2],t[l+24>>2]=c,-4==(0|c)){if((0|a)>-1)return Xe(13,108,127,0,0),G=l,0|(s=0);if(o<<24>>24)return Xe(13,108,126,0,0),G=l,0|(s=0);if(t[l+16>>2]=t[e>>2],!(0|NA(0,l+24|0,l+40|0,0,0,l+16|0,r,-1,0,0,u)))return Xe(13,108,58,0,0),G=l,0|(s=0);0|i[l+40>>0]?(t[l+24>>2]=-3,c=-3):c=0|t[l+24>>2],b=l+16|0,k=e}else b=l+16|0,k=e;switch(Q=-1==(0|a),t[b>>2]=t[k>>2],0|(a=0|NA(l+20|0,0,0,l+45|0,l+46|0,l+16|0,r,Q?c:a,Q?0:B,o,u))){case 0:return Xe(13,108,58,0,0),G=l,0|(s=0);case-1:return G=l,0|(s=a);default:a=0|t[l+24>>2];A:do{switch(0|a){case-3:u?(i[u>>0]=0,s=24):s=24;break;case 17:case 16:if(!(0|i[l+46>>0]))return Xe(13,108,156,0,0),G=l,0|(s=0);s=24;break;default:if(!(0|i[l+46>>0])){e=0|t[l+16>>2],a=0|t[l+20>>2],t[l+16>>2]=e+a,s=60;break A}switch(0|a){case 1:case 2:case 5:case 6:case 10:return Xe(13,108,195,0,0),G=l,0|(s=0)}if(i[l+44>>0]=1,0|FA(l,l+16|0,0|t[l+20>>2],0|i[l+45>>0],0)){if(0|Gt(l,(a=0|t[l>>2])+1|0)){i[(0|t[l+4>>2])+a>>0]=0,e=0|t[l+4>>2],s=60;break A}Xe(13,108,65,0,0),a=0;break A}a=0}}while(0);A:do{if(24==(0|s)){if(Q=0|t[e>>2],!((a=0|i[l+45>>0])<<24>>24)){s=0|t[l+16>>2],a=0|t[l+20>>2],t[l+16>>2]=s+a,e=Q,a=s-Q+a|0,s=60;break}c=0|t[l+20>>2];e:do{if((0|c)>0){B=1,o=a,a=0|t[b>>2],u=0;r:for(;;){for(e=B,E=a;;){if(1==(0|c)){c=1;break}if(0|i[(B=E)>>0])break;if(0|i[B+1>>0])break;if(!(a=e+-1|0)){s=49;break r}if(!((0|c)>2))break e;e=a,c=c+-2|0,E=B+2|0}if(t[l+28>>2]=E,128&(r=0|tA(l+28|0,l+32|0,l+40|0,l+36|0,c))?(Xe(13,104,102,0,0),a=E,r=u,B=1):(1&r?(B=c+E-(a=0|t[l+28>>2])|0,t[l+32>>2]=B):(a=0|t[l+28>>2],B=0|t[l+32>>2]),o=1&r,r=B,B=0),B){s=40;break}if(o<<24>>24){if(-1==(0|e)){s=43;break}B=e+1|0}else B=e,a=a+r|0;if((0|(c=c+E-a|0))<=0){s=47;break}u=r}if(40==(0|s)){Xe(13,190,58,0,0),a=0;break A}if(43==(0|s)){Xe(13,190,58,0,0),a=0;break A}if(47==(0|s)){if(B)break}else 49==(0|s)&&(a=B+2|0);t[b>>2]=a,e=Q,a=a-Q|0,s=60;break A}}while(0);Xe(13,190,137,0,0),a=0}}while(0);A:do{if(60==(0|s)){if(o=0|t[l+24>>2],t[l+40>>2]=e,u=e,0!=(0|(c=0|t[n+16>>2]))&&0!=(0|(w=0|t[c+20>>2]))){if(!(0|UB[7&w](A,e,a,o,l+44|0,n))){a=0;break}}else s=62;do{if(62==(0|s)){e:do{if(-4==(0|t[n+4>>2])){c=0|t[A>>2];do{if(!c){if(c=0|iA()){t[A>>2]=c;break}nA(0);break e}}while(0);(0|t[c>>2])!=(0|o)&&L(c,o,0),r=A,A=c+4|0,s=71}else r=0,c=0,s=71}while(0);if(71==(0|s)){e:do{switch(0|o){case 6:0|q(A,l+40|0,a)&&(s=99);break;case 5:if(a){Xe(13,204,144,0,0);break e}t[A>>2]=1,s=95;break e;case 1:if(1==(0|a)){t[A>>2]=f[e>>0],s=99;break e}Xe(13,204,106,0,0);break e;case 3:0|SA(A,l+40|0,a)&&(s=99);break;case 10:case 2:0|bA(A,l+40|0,a)&&(s=4+(0|t[A>>2])|0,t[s>>2]=256&t[s>>2]|o,s=95);break;case 30:1&a?Xe(13,204,214,0,0):s=86;break;case 28:3&a?Xe(13,204,215,0,0):s=86;break;default:s=86}}while(0);e:do{if(86==(0|s)){B=0|t[A>>2];do{if(!B){if(B=0|EA(o)){t[A>>2]=B;break}Xe(13,204,65,0,0);break e}t[B+4>>2]=o}while(0);if(0|i[l+44>>0]){Xn(0|t[(s=B+8|0)>>2],82796,0),t[s>>2]=u,t[B>>2]=a,i[l+44>>0]=0,s=99;break}0|uA(B,e,a)?s=99:(Xe(13,204,65,0,0),cA(B),t[A>>2]=0)}else 95==(0|s)&&(5==(0|o)&0!=(0|c)?(t[c+4>>2]=0,s=99):s=99)}while(0);if(99==(0|s))break;nA(c),0|r&&(t[r>>2]=0)}a=0;break A}}while(0);t[k>>2]=t[b>>2],a=1}}while(0);return 0|i[l+44>>0]?(Xn(0|t[l+4>>2],82796,0),G=l,0|(s=a)):(G=l,0|(s=a))}return 0}function NA(A,e,r,n,f,a,B,o,u,l,c){A|=0,e|=0,r|=0,n|=0,f|=0,B|=0,o|=0,u|=0,l|=0,c|=0;var E,Q,w=0,b=0,k=0;Q=G,G=G+16|0,E=0|t[(a|=0)>>2],t[Q>>2]=E;do{if(0|c){if(0|i[c>>0]){b=0|t[c+4>>2],t[Q+4>>2]=t[c+8>>2],t[Q+8>>2]=t[c+16>>2],t[Q+12>>2]=t[c+12>>2],t[Q>>2]=E+(0|t[c+20>>2]);break}if(w=0|tA(Q,Q+4|0,Q+12|0,Q+8|0,B),t[c+4>>2]=w,b=0|t[Q+4>>2],t[c+8>>2]=b,t[c+16>>2]=t[Q+8>>2],t[c+12>>2]=t[Q+12>>2],k=(0|t[Q>>2])-E|0,t[c+20>>2]=k,i[c>>0]=1,0==(129&w|0)&(k+b|0)>(0|B))return Xe(13,104,155,0,0),i[c>>0]=0,G=Q,0|(k=0);b=w}else b=0|tA(Q,Q+4|0,Q+12|0,Q+8|0,B)}while(0);if(128&b|0)return Xe(13,104,102,0,0),c?(i[c>>0]=0,G=Q,0|(k=0)):(G=Q,0|(k=0));do{if((0|o)>-1){if((0|t[Q+12>>2])==(0|o)&&(0|t[Q+8>>2])==(0|u)){if(!c)break;i[c>>0]=0;break}return l<<24>>24?(G=Q,0|(k=-1)):(0|c&&(i[c>>0]=0),Xe(13,104,168,0,0),G=Q,0|(k=0))}}while(0);return 0|(w=1&b)&&(t[Q+4>>2]=E+B-(0|t[Q>>2])),0|n&&(i[n>>0]=w),0|f&&(i[f>>0]=32&b),0|A&&(t[A>>2]=t[Q+4>>2]),0|r&&(i[r>>0]=t[Q+8>>2]),0|e&&(t[e>>2]=t[Q+12>>2]),t[a>>2]=t[Q>>2],G=Q,0|(k=1)}function FA(A,e,r,n,f){A|=0,r|=0,n|=0,f|=0;var a,B=0,o=0,u=0,l=0,c=0,E=0,Q=0,w=0;if(a=G,G=G+32|0,B=0|t[(e|=0)>>2],t[a>>2]=B,!(0!=(0|A)|0!=(1&n)))return t[e>>2]=B+r,G=a,0|(e=1);A:do{if((0|r)>0){e:do{if((0|f)>4){for(Q=B,u=B,E=r,o=0;;){if(1!=(0|E)&&0==(0|i[B>>0])&&0==(0|i[B+1>>0])){r=27;break e}if(t[a+4>>2]=B,128&(l=0|tA(a+4|0,a+8|0,a+16|0,a+12|0,E))?(Xe(13,104,102,0,0),c=B,B=u,r=Q,u=0,l=o,o=1):(1&l?(o=E+B-(r=0|t[a+4>>2])|0,t[a+8>>2]=o):(o=0|t[a+8>>2],r=0|t[a+4>>2]),t[a>>2]=r,c=r,B=r,u=32&l,l=o,o=0),o){r=36;break e}if(u<<24>>24)break;if(l){if(0|A){if(!(0|Gt(A,(B=0|t[A>>2])+l|0))){r=44;break e}If((0|t[A+4>>2])+B|0,0|c,0|l)}B=c+l|0,t[a>>2]=B,r=B}if((0|(E=Q+E-r|0))<=0){r=48;break A}Q=r,u=B,B=r,o=l}return Xe(13,106,197,0,0),G=a,0|(e=0)}for(E=B,w=B,l=0,c=0,Q=r,o=0;;){if(1!=(0|Q)&&0==(0|i[E>>0])&&0==(0|i[E+1>>0])){B=E,r=27;break e}if(t[a+4>>2]=E,128&(u=0|tA(a+4|0,a+8|0,a+16|0,a+12|0,Q))?(Xe(13,104,102,0,0),r=w,u=o,o=1):(1&u?(o=Q+E-(r=0|t[a+4>>2])|0,t[a+8>>2]=o):(r=0|t[a+4>>2],o=0|t[a+8>>2]),t[a>>2]=r,E=r,B=r,l=32&u,c=1&u,u=o,o=0),o){r=36;break e}if(l<<24>>24){if(!(0|FA(A,a,u,c,f+1|0))){B=0;break}r=B=0|t[a>>2]}else if(u){if(0|A){if(!(0|Gt(A,(B=0|t[A>>2])+u|0))){r=44;break e}If((0|t[A+4>>2])+B|0,0|E,0|u)}B=E+u|0,t[a>>2]=B,r=B}if((0|(Q=w+Q-r|0))<=0){r=48;break A}E=r,w=r,o=u}return G=a,0|B}while(0);if(27==(0|r)){if(B=B+2|0,t[a>>2]=B,1&n)break;return Xe(13,106,159,0,0),G=a,0|(e=0)}if(36==(0|r))return Xe(13,106,58,0,0),G=a,0|(e=0);if(44==(0|r))return Xe(13,140,65,0,0),G=a,0|(e=0)}else r=48}while(0);return 48==(0|r)&&1&n?(Xe(13,106,137,0,0),G=a,0|(e=0)):(t[e>>2]=B,G=a,0|(e=1))}function WA(A,e,r,n,f,a,B){A|=0,e|=0,r|=0,n|=0,f|=0,a|=0,B|=0;var o,u,l=0,c=0,E=0;if(u=G,G=G+32|0,t[u+8>>2]=r,!A)return G=u,0|(e=0);o=0|t[n>>2],t[u+4>>2]=t[e>>2],4096&o?(t[u+12>>2]=A,c=u+12|0):c=A;A:do{if(6&o){switch(8&o?(l=192&o,A=0|t[n+4>>2]):(l=0,A=o>>>1&1|16),0|(A=0|NA(u+8|0,0,0,u+16|0,0,u+4|0,r,A,l,f,a))){case 0:Xe(13,131,58,0,0),A=0;break;case-1:break;default:if(A=0|t[c>>2]){if((0|Hn(A))>0)do{t[u>>2]=0|Vn(A),O(u,0|t[n+16>>2])}while((0|Hn(A))>0);A=0|t[c>>2]}else A=0|Wn(),t[c>>2]=A;do{if(A){A=0|t[u+8>>2];e:do{if((0|A)>0){r:for(;;){l=0|t[u+4>>2];do{if(1!=(0|A)&&0==(0|i[l>>0])&&0==(0|i[l+1>>0])){if(t[u+4>>2]=l+2,0|i[u+16>>0]){t[u+8>>2]=A+l-(l+2),i[u+16>>0]=0,A=6;break}Xe(13,131,159,0,0),A=4;break}E=26}while(0);do{if(26==(0|E)){if(E=0,t[u>>2]=0,!(0|UA(u,u+4|0,A,0|t[n+16>>2],-1,0,0,a,B))){Xe(13,131,58,0,0),z(0|t[u>>2],0|t[n+16>>2]),A=4;break}t[u+8>>2]=l-(0|t[u+4>>2])+(0|t[u+8>>2]),0|yn(0|t[c>>2],0|t[u>>2])?A=0:(Xe(13,131,65,0,0),z(0|t[u>>2],0|t[n+16>>2]),A=4)}}while(0);switch(7&A){case 6:break e;case 0:break;default:break r}if((0|(A=0|t[u+8>>2]))<=0)break e}if(!A)break A;return G=u,0|(A=0)}}while(0);if(0|i[u+16>>0]){Xe(13,131,137,0,0);break}break A}Xe(13,131,65,0,0)}while(0);return G=u,0|(e=0)}return G=u,0|(e=A)}if(A=0|t[n+16>>2],8&o){switch(0|(A=0|UA(c,u+4|0,r,A,0|t[n+4>>2],192&o,f,a,B))){case 0:break;case-1:return G=u,0|(e=A);default:break A}return Xe(13,131,58,0,0),G=u,0|(e=0)}switch(0|(A=0|UA(c,u+4|0,r,A,-1,0,f,a,B))){case 0:break;case-1:return G=u,0|(e=A);default:break A}return Xe(13,131,58,0,0),G=u,0|(e=0)}while(0);return t[e>>2]=t[u+4>>2],G=u,0|(e=1)}function YA(){return 0|pA(52132)}function RA(A){z(A|=0,52132)}function yA(A,e,r,i){e|=0,i|=0;var n=0;return(A|=0)?-1!=(0|(r|=0))&&0==(0|t[A+4>>2])&&(n=0|iA(),t[A+4>>2]=n,0==(0|n))?0|(n=0):($(0|t[A>>2]),t[A>>2]=e,r?(e=0|t[A+4>>2],-1==(0|r)?(nA(e),t[A+4>>2]=0,0|(n=1)):(L(e,r,i),0|(n=1))):0|(n=1)):0|(i=0)}function VA(A,e){A|=0;var r,i,n=0;if(r=(i=0==(8&t[(e|=0)+12>>2]|0))?5:-1,e=0|Sr(0|Si(e)),A&&(!i||0!=(0|t[A+4>>2])||(n=0|iA(),t[A+4>>2]=n,0!=(0|n))))return $(0|t[A>>2]),t[A>>2]=e,e=0|t[A+4>>2],i?void L(e,r,0):(nA(e),void(t[A+4>>2]=0))}function mA(A,e,r,i){A|=0,e|=0,r|=0,i|=0;var n,f=0;return n=G,G=G+16|0,t[n>>2]=A,e?0|t[e>>2]?(e=0|HA(n,e,r,-1,i),G=n,0|e):((0|(A=0|HA(n,0,r,-1,i)))>=1&&((f=0|Ln(A,82796,0))?(t[n+4>>2]=f,HA(n,n+4|0,r,-1,i),t[e>>2]=f):A=-1),G=n,0|(e=A)):(e=0|HA(n,0,r,-1,i),G=n,0|e)}function HA(A,e,r,n,f){A|=0,e|=0,n|=0,f|=0;var a,B=0,o=0,u=0,l=0,c=0,E=0,Q=0;a=G,G=G+16|0,B=0|t[(r|=0)+16>>2],(o=0|i[r>>0])<<24>>24!=0&&0==(0|t[A>>2])?B=0:u=3;A:do{if(3==(0|u)){switch(Q=B?0|t[B+16>>2]:0,o<<24>>24|0){case 0:if(B=0|t[r+8>>2]){B=0|vA(A,e,B,n,f);break A}B=0|JA(A,e,r,n,f);break A;case 5:B=0|JA(A,e,r,-1,f);break A;case 2:if((o=0!=(0|Q))&&0==(0|MB[31&Q](6,A,r,0))){B=0;break A}if((0|(B=0|qA(A,r)))>-1&&(0|B)<(0|t[r+12>>2])){B=0|vA(0|ne(A,B=(0|t[r+8>>2])+(20*B|0)|0),e,B,-1,f);break A}if(!o){B=0;break A}MB[31&Q](7,A,r,0),B=0;break A;case 4:B=0|hB[15&t[B+20>>2]](A,e,r,n,f);break A;case 6:E=1+(f>>>11&1)|0;break;case 1:E=o<<24>>24;break;default:B=0;break A}if((0|(B=0|ie(a,e,A,r)))>=0){if(0|B){B=0|t[a>>2];break}if(t[a>>2]=0,c=-1==(0|n)?-193&f:f,f=-1==(0|n)?16:n,(l=0!=(0|Q))&&0==(0|MB[31&Q](6,A,r,0))){B=0;break}if((0|t[r+12>>2])>0)for(u=0,n=0|t[r+8>>2];;){if(!(B=0|te(A,n,1))){B=0;break A}if(-1==(0|(o=0|vA(0|ne(A,B),0,B,-1,c)))){B=-1;break A}if((0|o)>(2147483647-(B=0|t[a>>2])|0)){B=-1;break A}if(t[a>>2]=B+o,(0|(u=u+1|0))>=(0|t[r+12>>2])){B=B+o|0;break}n=n+20|0}else B=0|t[a>>2];if(!(0==(0|e)|-1==(0|(B=0|BA(E,B,f))))){if(fA(e,E,0|t[a>>2],f,c),(0|t[r+12>>2])>0)for(u=0,n=0|t[r+8>>2];;){if(!(o=0|te(A,n,1))){B=0;break A}if(vA(0|ne(A,o),e,o,-1,c),(0|(u=u+1|0))>=(0|t[r+12>>2]))break;n=n+20|0}if(2==(0|E)&&aA(e),l&&0==(0|MB[31&Q](7,A,r,0))){B=0;break}}}else B=0}}while(0);return G=a,0|B}function vA(A,e,r,i,n){A|=0,e|=0,i|=0,n|=0;var f,a,B,o=0,u=0,l=0,c=0,E=0,Q=0;B=G,G=G+16|0,4096&(a=0|t[(r|=0)>>2])&&(t[B+4>>2]=A,A=B+4|0);do{if(24&a){if(-1!=(0|i))return G=B,0|(e=-1);E=192&a,c=0|t[r+4>>2];break}E=-1!=(0|i)?192&n:0,c=i}while(0);if(!(6&a))return i=0|t[r+16>>2],16&a?(o=0|HA(A,0,i,-1,-193&n))?0!=(0|e)&-1!=(0|(i=0|BA(1+((a&n)>>>11&1)|0,o,c)))?(fA(e,1+((a&n)>>>11&1)|0,o,c,E),HA(A,e,0|t[r+16>>2],-1,-193&n),2!=(1+((a&n)>>>11&1)|0)?(G=B,0|(e=i)):(aA(e),G=B,0|(e=i))):(G=B,0|(e=i)):(G=B,0|(e=0)):(e=0|HA(A,e,i,c,E|-193&n),G=B,0|e);f=0|t[A>>2];A:do{if(f){if(l=(u=0==(16&a|0)&-1!=(0|c))?c:(2&a)>>>1|16,u=u?E:0,(0|Hn(f))>0)for(o=0,A=0;;){if(t[B>>2]=0|vn(f,o),-1==(0|(i=0|HA(B,0,0|t[r+16>>2],-1,-193&n)))|(0|A)>(2147483647-i|0)){A=-1;break A}if(A=i+A|0,(0|(o=o+1|0))>=(0|Hn(f))){o=A;break}}else o=0;if(-1!=(0|(i=0|BA(1+((a&n)>>>11&1)|0,o,l)))){if(!(0==(0|e)|-1==(0|(A=16&a|0?0|BA(1+((a&n)>>>11&1)|0,i,c):i)))){16&a|0&&fA(e,1+((a&n)>>>11&1)|0,i,c,E),fA(e,1+((a&n)>>>11&1)|0,o,l,u),E=0|t[r+16>>2],t[B+8>>2]=0;do{if(0!=(2&a|0)&&(0|Hn(f))>=2){if(0|(c=0|Ln(12*(0|Hn(f))|0,82796,0))){if(!(l=0|Ln(o,82796,0))){Xn(c,82796,0);break}if(t[B+8>>2]=l,(0|Hn(f))>0)for(i=0,o=c;;){if(t[B+12>>2]=0|vn(f,i),t[o>>2]=t[B+8>>2],t[o+4>>2]=0|HA(B+12|0,B+8|0,E,-1,-193&n),t[o+8>>2]=t[B+12>>2],(0|(i=i+1|0))>=(0|Hn(f))){i=B+8|0;break}o=o+12|0}else i=B+8|0;if(pt(c,0|Hn(f),12,27),t[i>>2]=t[e>>2],r=(0|Hn(f))>0,i=0|t[B+8>>2],r)for(o=0,u=c;r=u+4|0,If(0|i,0|t[u>>2],0|t[r>>2]),t[B+8>>2]=(0|t[B+8>>2])+(0|t[r>>2]),r=(0|(o=o+1|0))<(0|Hn(f)),i=0|t[B+8>>2],r;)u=u+12|0;if(t[e>>2]=i,4&a|0&&(0|Hn(f))>0)for(i=0,o=c;Jn(f,i,0|t[o+8>>2]),!((0|(i=i+1|0))>=(0|Hn(f)));)o=o+12|0;Xn(c,82796,0),Xn(l,82796,0)}}else Q=24}while(0);if(24==(0|Q)&&(0|Hn(f))>0){i=0;do{t[B+12>>2]=0|vn(f,i),HA(B+12|0,e,E,-1,-193&n),i=i+1|0}while((0|i)<(0|Hn(f)))}2==(1+((a&n)>>>11&1)|0)&&(aA(e),16&a|0)&&aA(e)}}else A=-1}else A=0}while(0);return G=B,0|(e=A)}function JA(A,e,r,i,n){A|=0,e|=0,r|=0,i|=0,n|=0;var f,a=0,B=0,o=0,u=0;switch(f=G,G=G+16|0,t[f>>2]=t[r+4>>2],a=0|XA(A,0,f,r),0|(B=0|t[f>>2])){case 16:case 17:case-3:u=0;break;default:u=1}switch(0|a){case-2:a=0,o=2;break;case-1:return G=f,0|(u=0);default:o=0}B=-1==(0|i)?B:i;do{if(0|e){if(0|u&&fA(e,o,a,B,n),XA(A,0|t[e>>2],f,r),o){aA(e);break}t[e>>2]=(0|t[e>>2])+a;break}}while(0);return u?(u=0|BA(o,a,B),G=f,0|u):(G=f,0|(u=a))}function XA(A,e,r,n){A|=0,e|=0,r|=0,n|=0;var f,a=0,B=0,o=0;if(f=G,G=G+16|0,t[f>>2]=e,0|(a=0|t[n+16>>2])&&0|(B=0|t[a+24>>2]))return e=0|MB[31&B](A,e,r,n),G=f,0|e;o=(a=0|i[n>>0])<<24>>24==0&&1==(0|t[n+4>>2])?11:6;do{if(6==(0|o)){if(!(B=0|t[A>>2]))return G=f,0|(e=-1);if(a<<24>>24==5){a=0|t[B+4>>2],t[r>>2]=a,B=A;break}-4==(0|t[n+4>>2])?(B=0|t[A>>2],a=0|t[B>>2],t[r>>2]=a,B=B+4|0):o=11}}while(0);switch(11==(0|o)&&(B=A,a=0|t[r>>2]),0|a){case 6:if(B=0|t[B>>2],0==(0|(a=0|t[B+16>>2]))|0==(0|(B=0|t[B+12>>2])))return G=f,0|(e=-1);break;case 1:if(-1==(0|(B=0|t[B>>2])))return G=f,0|(e=-1);do{if(-4!=(0|t[n+4>>2])){if(a=0|t[n+20>>2],B){if(!((0|a)>0))break;return G=f,0|(a=-1)}if(a)break;return G=f,0|(a=-1)}}while(0);i[f+4>>0]=B,a=f+4|0,B=1;break;case 3:return e=0|LA(0|t[B>>2],0==(0|e)?0:f),G=f,0|e;case 10:case 2:return e=0|wA(0|t[B>>2],0==(0|e)?0:f),G=f,0|e;case 5:return G=f,0|(e=0);default:if(B=0|t[B>>2],2048==(0|t[n+20>>2])&&16&t[B+12>>2]|0)return e?(t[B+8>>2]=e,t[B>>2]=0,G=f,0|(e=-2)):(G=f,0|(e=-2));a=0|t[B+8>>2],B=0|t[B>>2]}return 0!=(0|e)&0!=(0|B)?(If(0|e,0|a,0|B),G=f,0|(e=B)):(G=f,0|(e=B))}function TA(A,e,r){return 0|mA(A|=0,e|=0,r|=0,0)}function LA(A,e){e|=0;var r=0,n=0,a=0,B=0;if(!(A|=0))return 0|(e=0);n=0|t[A>>2];do{if((0|n)>0){if(8&(r=0|t[A+12>>2])|0){B=7&r;break}for(a=0|t[A+8>>2],r=n;;){if(0|i[a+(n=r+-1|0)>>0]){n=r;break}if(!((0|r)>1))break;r=n}B=1&(r=0|f[a+(n+-1)>>0])?0:2&r?1:4&r?2:8&r?3:16&r?4:32&r?5:0==(64&r|0)?0==(128&r|0)?0:7:6}else B=0}while(0);return a=n+1|0,e?(r=0|t[e>>2],i[r>>0]=B,(0|n)>0?(If(r+1|0,0|t[A+8>>2],0|n),i[(r=r+1+n|0)+-1>>0]=255<<B&f[r+-1>>0]):r=r+1|0,t[e>>2]=r,0|(e=a)):0|(e=a)}function SA(A,e,r){A|=0,e|=0,r|=0;var n=0,a=0,B=0,o=0;do{if((0|r)>=1){if((0==(0|A)||0==(0|(n=0|t[A>>2])))&&!(n=0|EA(3)))return 0|(A=0);if(B=0|t[e>>2],!((255&(o=0|i[B>>0]))>7)){if(t[(a=n+12|0)>>2]=-16&t[a>>2]|255&o|8,(0|r)>1){if(!(a=0|Ln(r+-1|0,82796,0))){a=65;break}If(0|a,B+1|0,r+-1|0),i[a+(r+-2)>>0]=255<<(255&o)&(0|f[a+(r+-2)>>0]),B=B+r|0}else B=B+1|0,a=0;return t[n>>2]=r+-1,Xn(0|t[(r=n+8|0)>>2],82796,0),t[r>>2]=a,t[n+4>>2]=3,0|A&&(t[A>>2]=n),t[e>>2]=B,0|(A=n)}a=220}else a=152,n=0}while(0);return Xe(13,189,a,0,0),0|A&&(0|t[A>>2])==(0|n)||function(A){cA(A|=0)}(n),0|(A=0)}function pA(A){A|=0;var e;return e=G,G=G+16|0,t[e>>2]=0,A=(0|zA(e,A,0))>0,G=e,0|(A?0|t[e>>2]:0)}function zA(A,e,r){A|=0,r|=0;var n,f=0,a=0,B=0,o=0,u=0,l=0,c=0,E=0,Q=0;n=G,G=G+16|0,0!=(0|(a=0|t[(e|=0)+16>>2]))&&0!=(0|(f=0|t[a+16>>2]))||(f=0);A:do{switch(0|i[e>>0]){case 4:if(!a)return G=n,0|(e=1);if(!(f=0|t[a+4>>2]))return G=n,0|(e=1);if(0|gB[63&f](A,e))return G=n,0|(e=1);Q=56;break;case 0:if(!(B=0|t[e+8>>2])){if(!(0|jA(A,e,r))){Q=56;break A}return G=n,0|(f=1)}f=0|t[B+16>>2],4096&(a=0|t[B>>2])&&(t[n>>2]=A,A=n);do{if(!(1&a)){if(768&a|0){t[A>>2]=0;break}if(!(6&a)){if(!(0|zA(A,f,4096&a))){Q=56;break A}return G=n,0|(f=1)}if(f=0|Wn()){t[A>>2]=f;break}Xe(13,133,65,0,0),Q=56;break A}KA(A,B)}while(0);return G=n,0|(e=1);case 5:if(0|jA(A,e,r))return G=n,0|(e=1);Q=56;break;case 2:a=0!=(0|f);e:do{if(a){switch(0|MB[31&f](0,A,e,0)){case 0:break A;case 2:f=1;break;default:break e}return G=n,0|f}}while(0);if(r)Df(0|t[A>>2],0,0|t[e+20>>2]);else if(Q=0|Sn(0|t[e+20>>2],82796,0),t[A>>2]=Q,!Q){Q=56;break A}if($A(A,-1,e),!a)return G=n,0|(e=1);if(0|MB[31&f](1,A,e,0))return G=n,0|(e=1);Q=57;break;case 1:case 6:c=0!=(0|f);e:do{if(c){switch(0|MB[31&f](0,A,e,0)){case 0:break A;case 2:f=1;break;default:break e}return G=n,0|f}}while(0);if(0|r)Df(0|t[A>>2],0,0|t[e+20>>2]);else if(E=0|Sn(0|t[e+20>>2],82796,0),t[A>>2]=E,!E){Q=56;break A}if((0|Ae(A,0,e))<0){if(0|r){Q=56;break A}Xn(0|t[A>>2],82796,0),t[A>>2]=0,Q=56;break A}ee(A,e);e:do{if((0|t[e+12>>2])>0){l=0,E=0|t[e+8>>2];r:for(;;){a=0|ne(A,E),B=0|t[E+16>>2],4096&(o=0|t[E>>2])?(t[n>>2]=a,u=n):u=a;do{if(1&o)KA(u,E),Q=50;else{if(768&o|0){t[u>>2]=0,Q=50;break}if(!(6&o)){if(0|zA(u,B,4096&o))break;break r}if(!(a=0|Wn())){Q=48;break r}t[u>>2]=a,Q=50}}while(0);if(50==(0|Q)&&(Q=0),(0|(l=l+1|0))>=(0|t[e+12>>2]))break e;E=E+20|0}48==(0|Q)&&Xe(13,133,65,0,0),K(A,e,r),Q=56;break A}}while(0);if(!c)return G=n,0|(e=1);if(0|MB[31&f](1,A,e,0))return G=n,0|(e=1);Q=57;break;default:return G=n,0|(e=1)}}while(0);return 56==(0|Q)?(Xe(13,121,65,0,0),G=n,0|(e=0)):(57==(0|Q)&&K(A,e,r),Xe(13,121,100,0,0),G=n,0|(e=0))}function KA(A,e){A|=0,e|=0;var r=0,n=0,f=0;A:do{if(!(774&t[e>>2])){e:for(;;){switch(f=0|t[e+16>>2],(r=0|i[f>>0])<<24>>24|0){case 4:e=6;break e;case 5:e=19;break e;case 6:case 1:case 2:e=28;break e;case 0:break;default:e=29;break e}if(!(e=0|t[f+8>>2])){e=11;break}if(774&t[e>>2]|0)break A}if(6==(0|e))return 0|(e=0|t[f+16>>2])&&0|(n=0|t[e+12>>2])?void FB[7&n](A,f):void(t[A>>2]=0);if(11==(0|e))return 0|(e=0|t[f+16>>2])?(e=0|t[e+16>>2])?void FB[7&e](A,f):void(t[A>>2]=0):r<<24>>24!=5&&1==(0|t[f+4>>2])?void(t[A>>2]=t[f+20>>2]):void(t[A>>2]=0);if(19==(0|e)){do{if(0|f){if(!(e=0|t[f+16>>2])){if(r<<24>>24==5)break;if(1!=(0|t[f+4>>2]))break;return void(t[A>>2]=t[f+20>>2])}return(e=0|t[e+16>>2])?void FB[7&e](A,f):void(t[A>>2]=0)}}while(0);return void(t[A>>2]=0)}if(28==(0|e))return void(t[A>>2]=0);if(29==(0|e))return}}while(0);t[A>>2]=0}function jA(A,e,r){A|=0,r|=0;var n=0,f=0;if(!(e|=0))return 0|(A=0);n=0|t[e+16>>2];do{if(0|n){if(r){if(!(n=0|t[n+16>>2]))break;return FB[7&n](A,e),0|(A=1)}if(!(n=0|t[n+8>>2]))break;return 0|(A=0|gB[63&n](A,e))}}while(0);A:do{if(5==(0|i[e>>0]))n=-1,f=15;else switch(0|(n=0|t[e+4>>2])){case 6:return t[A>>2]=0|Sr(0),0|(A=1);case 1:return t[A>>2]=t[e+20>>2],0|(A=1);case 5:return t[A>>2]=1,0|(A=1);case-4:if(n=0|Ln(8,82796,0)){t[n+4>>2]=0,t[n>>2]=-1,t[A>>2]=n;break A}return 0|(A=0);default:f=15;break A}}while(0);return 15==(0|f)&&(r?(f=0|t[A>>2],t[f>>2]=0,t[f+4>>2]=0,t[f+8>>2]=0,t[f+4>>2]=n,t[f+12>>2]=128,n=f):(n=0|EA(n),t[A>>2]=n),0!=(0|n)&5==(0|i[e>>0])&&(t[(f=n+12|0)>>2]=64|t[f>>2])),0|(A=0!=(0|t[A>>2])&1)}function xA(A,e){return 0|zA(A|=0,e|=0,0)}function OA(A,e,r,i,n,f,a){return A|=0,e|=0,i|=0,n|=0,f|=0,a|=0,(0|(r|=0))>-1&&0==(0|sA(0|t[A>>2],r))?0|(r=0):0|yA(0|t[A+4>>2],e,i,n)?f?(function(A,e,r){e|=0,r|=0,Xn(0|t[(A|=0)+8>>2],82796,0),t[A+8>>2]=e,t[A>>2]=r}(0|t[A+8>>2],f,a),0|(r=1)):0|(r=1):0|(r=0)}function PA(A,e,r,i,n){return e|=0,r|=0,i|=0,n|=0,0|(A|=0)&&(t[A>>2]=t[t[n+4>>2]>>2]),0|e&&(t[e>>2]=0|function(A){return 0|t[(A|=0)+8>>2]}(0|t[n+8>>2]),t[r>>2]=0|function(A){return 0|t[(A|=0)>>2]}(0|t[n+8>>2])),i?(t[i>>2]=t[n+4>>2],1):1}function _A(A,e){A|=0;var r,n=0,a=0,B=0;if(r=G,G=G+16|0,!(e|=0))return G=r,0|(A=0);if(256&t[e+4>>2]){if(1!=(0|se(A,80624,1)))return G=r,0|(A=-1);n=1}else n=0;if(!(a=0|t[e>>2]))return A=2==(0|se(A,54573,2)),G=r,0|(A=A?2|n:-1);if((0|a)<=0)return G=r,0|(A=n);for(a=0;;){if(0!=(0|a)&0==(0|(a>>>0)%35)){if(2!=(0|se(A,54576,2))){n=-1,a=13;break}n=n+2|0}if(B=(0|t[e+8>>2])+a|0,i[r>>0]=0|i[1312+((0|f[B>>0])>>>4)>>0],i[r+1>>0]=0|i[1312+(15&i[B>>0])>>0],2!=(0|se(A,r,2))){n=-1,a=13;break}if(n=n+2|0,(0|(a=a+1|0))>=(0|t[e>>2])){a=13;break}}return 13==(0|a)?(G=r,0|n):0}function qA(A,e){return e|=0,0|t[(0|t[(A|=0)>>2])+(0|t[e+4>>2])>>2]}function $A(A,e,r){return e|=0,r|=0,r=(0|t[(A|=0)>>2])+(0|t[r+4>>2])|0,A=0|t[r>>2],t[r>>2]=e,0|A}function Ae(A,e,r){A|=0,e|=0,r|=0;var n,f=0;n=G,G=G+16|0;A:do{switch(0|i[r>>0]){case 6:case 1:if(0!=(0|(r=0|t[r+16>>2]))&&0!=(1&t[r+4>>2]|0)){if(A=(f=0|t[A>>2])+(0|t[r+8>>2])|0,f=f+(0|t[r+12>>2])|0,!e){if(t[A>>2]=1,e=0|W(),t[f>>2]=e,0|e){r=1;break A}Xe(13,233,65,0,0),r=-1;break A}0|T(A,e,n,0|t[f>>2])?(r=0|t[n>>2])||(V(0|t[f>>2]),t[f>>2]=0,r=0|t[n>>2]):r=-1}else r=0;break;default:r=0}}while(0);return G=n,0|r}function ee(A,e){e|=0;var r;(A|=0)&&(r=0|t[A>>2])&&(A=0|t[e+16>>2])&&2&t[A+4>>2]&&(A=r+(0|t[A+20>>2])|0)&&(t[A>>2]=0,t[A+4>>2]=0,t[A+8>>2]=1)}function re(A,e,r,i){e|=0,r|=0,i|=0;var n=0;return(A|=0)&&(n=0|t[A>>2])&&(A=0|t[i+16>>2])&&2&t[A+4>>2]&&(A=n+(0|t[A+20>>2])|0)?(Xn(0|t[A>>2],82796,0),n=0|Ln(r,82796,0),t[A>>2]=n,n?(If(0|n,0|e,0|r),t[A+4>>2]=r,t[A+8>>2]=0,0|(r=1)):0|(r=0)):0|(r=1)}function ie(A,e,r,i){A|=0,e|=0,i|=0;var n;return(r|=0)&&(n=0|t[r>>2])&&(r=0|t[i+16>>2])&&2&t[r+4>>2]&&(r=n+(0|t[r+20>>2])|0)?0|t[r+8>>2]?0|(e=0):(0|e&&(If(0|t[e>>2],0|t[r>>2],0|t[r+4>>2]),t[e>>2]=(0|t[e>>2])+(0|t[r+4>>2])),A?(t[A>>2]=t[r+4>>2],0|(e=1)):0|(e=1)):0|(e=0)}function ne(A,e){return e|=0,(0|t[(A|=0)>>2])+(0|t[e+8>>2])|0}function te(A,e,r){A|=0,r|=0;var i,n,f=0,a=0;if(n=G,G=G+16|0,!(768&(f=0|t[(e|=0)>>2])))return G=n,0|(r=e);if(i=0|t[e+16>>2],e=0|t[(0|t[A>>2])+(0|t[i+4>>2])>>2]){if(e=256&f?0|zr(e):0|CA(e),t[n>>2]=e,0|(e=0|t[i+8>>2])&&0==(0|IB[31&e](n)))return Xe(13,110,164,0,0),G=n,0|(r=0);e=0|t[i+16>>2];A:do{if((0|e)>0){for(A=0|t[n>>2],f=0|t[i+12>>2],a=0;(0|t[f>>2])!=(0|A);){if((0|(a=a+1|0))>=(0|e))break A;f=f+24|0}return G=n,0|(r=f+4|0)}}while(0);if(0|(e=0|t[i+20>>2]))return G=n,0|(r=e)}else if(0|(e=0|t[i+24>>2]))return G=n,0|(r=e);return r?(Xe(13,110,164,0,0),G=n,0|(r=0)):(G=n,0|(r=0))}function fe(A,e,r,n,f){e|=0,r|=0,n|=0,f|=0;var a,B=0,o=0,u=0;if(a=G,G=G+128|0,B=0|t[(A|=0)>>2],n){if((0|B)!=(0|n)&&0==(0|e)|(0|r)<0)return G=a,0|(f=0);t[A>>2]=n,B=n,n=1}else{if(!B)return G=a,0|(f=0);n=0}A:do{if(e){(0|(n=0|Li(B)))>=129&&oi(54600,82796,0);do{if((0|n)>=(0|r)){if(r>>>0>128)return G=a,0|(f=0);If(A+20|0,0|e,0|r),t[A+16>>2]=r;break}if(!(0|gi(0|t[A+4>>2],B,f)))break A;if(!(0|Di(0|t[A+4>>2],e,r)))break A;if(!(0|Mi(0|t[A+4>>2],A+20|0,A+16|0)))break A;r=0|t[A+16>>2]}while(0);128==(0|r)||Df(A+20+r|0,0,128-r|0),o=19}else n?o=19:(u=A+8|0,o=28)}while(0);if(19==(0|o)){r=0;do{i[a+r>>0]=54^i[A+20+r>>0],r=r+1|0}while(128!=(0|r));if(0|gi(0|t[A+8>>2],B,f)&&0|Di(e=0|t[A+8>>2],a,0|Li(B))){r=0;do{i[a+r>>0]=92^i[A+20+r>>0],r=r+1|0}while(128!=(0|r));0|gi(0|t[A+12>>2],B,f)&&0|Di(f=0|t[A+12>>2],a,0|Li(B))&&(u=A+8|0,o=28)}}return 28==(0|o)&&0|hi(0|t[A+4>>2],0|t[u>>2])?(G=a,0|(f=1)):(G=a,0|(f=0))}function ae(A,e,r){return e|=0,r|=0,0|t[(A|=0)>>2]?0|(r=0|Di(0|t[A+4>>2],e,r)):0|(r=0)}function Be(A,e,r){e|=0,r|=0;var i;return i=G,G=G+80|0,0|t[(A|=0)>>2]&&0|Mi(0|t[A+4>>2],i,i+64|0)&&0|hi(0|t[A+4>>2],0|t[A+12>>2])&&0|Di(0|t[A+4>>2],i,0|t[i+64>>2])&&0|Mi(0|t[A+4>>2],e,r)?(G=i,0|(e=1)):(G=i,0|(e=0))}function oe(){var A=0;return 0!=(0|(A=0|Sn(148,82796,0)))&&0==(0|ue(A))&&(le(A),A=0),0|A}function ue(A){var e=0,r=0;Ci(0|t[(A|=0)+8>>2]),Ci(0|t[A+12>>2]),Ci(0|t[A+4>>2]),t[A>>2]=0,t[A+16>>2]=0,$r(A+20|0,128),(0!=(0|t[A+8>>2])||(r=0|di(),t[A+8>>2]=r,0|r))&&(e=3);do{if(3==(0|e)){if(0==(0|t[A+12>>2])&&(r=0|di(),t[A+12>>2]=r,0==(0|r)))break;if(0==(0|t[A+4>>2])&&(r=0|di(),t[A+4>>2]=r,0==(0|r)))break;return t[A>>2]=0,0|(r=1)}}while(0);return Ci(0|t[A+8>>2]),Ci(0|t[A+12>>2]),Ci(0|t[A+4>>2]),t[A>>2]=0,t[A+16>>2]=0,$r(A+20|0,128),0|(r=0)}function le(A){(A|=0)&&(Ci(0|t[A+8>>2]),Ci(0|t[A+12>>2]),Ci(0|t[A+4>>2]),t[A>>2]=0,t[A+16>>2]=0,$r(A+20|0,128),Ii(0|t[A+8>>2]),Ii(0|t[A+12>>2]),Ii(0|t[A+4>>2]),Xn(A,82796,0))}function ce(A,e,r){e|=0;var n=0,f=0,a=0;if(!(r|=0))return 1;f=0|t[(A|=0)+16>>2],n=0|t[A+20>>2],(f+(r<<3)|0)>>>0<f>>>0&&(t[A+20>>2]=n+1,n=n+1|0),t[A+20>>2]=n+(r>>>29),t[A+16>>2]=f+(r<<3),a=0|t[A+88>>2];do{if(a){if((a+r|r)>>>0>63){If(A+24+a|0,0|e,64-a|0),Ee(A,A+24|0,1),f=(n=A+24|0)+68|0;do{i[n>>0]=0,n=n+1|0}while((0|n)<(0|f));n=e+(64-a)|0,r=r-(64-a)|0;break}return If(A+24+a|0,0|e,0|r),t[A+88>>2]=(0|t[A+88>>2])+r,1}n=e}while(0);return a=-64&r,(f=r>>>6)&&(Ee(A,n,f),n=n+a|0,r=r-a|0),r?(t[A+88>>2]=r,If(A+24|0,0|n,0|r),1):1}function Ee(A,e,r){A|=0,e|=0;var i=0,n=0,a=0,B=0,o=0,u=0,l=0,c=0,E=0,Q=0,w=0,b=0,k=0,s=0,C=0,d=0,I=0,g=0,D=0,M=0,h=0,U=0,G=0,Z=0,N=0;if(r|=0)for(i=0|t[A>>2],n=0|t[A+4>>2],a=0|t[A+8>>2],B=0|t[A+12>>2];r=r+-1|0,C=(0|f[e+1>>0])<<8|0|f[e>>0]|(0|f[e+2>>0])<<16|(0|f[e+3>>0])<<24,g=(0|f[e+5>>0])<<8|0|f[e+4>>0]|(0|f[e+6>>0])<<16|(0|f[e+7>>0])<<24,b=((b=i+-680876936+((a^B)&n^B)+C|0)<<7|b>>>25)+n|0,l=(0|f[e+9>>0])<<8|0|f[e+8>>0]|(0|f[e+10>>0])<<16|(0|f[e+11>>0])<<24,Z=B+-389564586+g+(b&(n^a)^a)|0,M=(0|f[e+13>>0])<<8|0|f[e+12>>0]|(0|f[e+14>>0])<<16|(0|f[e+15>>0])<<24,s=((s=a+606105819+l+((Z<<12|Z>>>20)+b&(b^n)^n)|0)<<17|s>>>15)+((Z<<12|Z>>>20)+b)|0,o=(0|f[e+17>>0])<<8|0|f[e+16>>0]|(0|f[e+18>>0])<<16|(0|f[e+19>>0])<<24,Q=n+-1044525330+M+(s&((Z<<12|Z>>>20)+b^b)^b)|0,U=(0|f[e+21>>0])<<8|0|f[e+20>>0]|(0|f[e+22>>0])<<16|(0|f[e+23>>0])<<24,d=((d=b+-176418897+o+((Q<<22|Q>>>10)+s&(s^(Z<<12|Z>>>20)+b)^(Z<<12|Z>>>20)+b)|0)<<7|d>>>25)+((Q<<22|Q>>>10)+s)|0,c=(0|f[e+25>>0])<<8|0|f[e+24>>0]|(0|f[e+26>>0])<<16|(0|f[e+27>>0])<<24,b=(Z<<12|Z>>>20)+b+1200080426+U+(d&((Q<<22|Q>>>10)+s^s)^s)|0,Z=(0|f[e+29>>0])<<8|0|f[e+28>>0]|(0|f[e+30>>0])<<16|(0|f[e+31>>0])<<24,I=((I=s+-1473231341+c+((b<<12|b>>>20)+d&(d^(Q<<22|Q>>>10)+s)^(Q<<22|Q>>>10)+s)|0)<<17|I>>>15)+((b<<12|b>>>20)+d)|0,u=(0|f[e+33>>0])<<8|0|f[e+32>>0]|(0|f[e+34>>0])<<16|(0|f[e+35>>0])<<24,s=(Q<<22|Q>>>10)+s+-45705983+Z+(I&((b<<12|b>>>20)+d^d)^d)|0,Q=(0|f[e+37>>0])<<8|0|f[e+36>>0]|(0|f[e+38>>0])<<16|(0|f[e+39>>0])<<24,N=((N=u+1770035416+d+((s<<22|s>>>10)+I&(I^(b<<12|b>>>20)+d)^(b<<12|b>>>20)+d)|0)<<7|N>>>25)+((s<<22|s>>>10)+I)|0,E=(0|f[e+41>>0])<<8|0|f[e+40>>0]|(0|f[e+42>>0])<<16|(0|f[e+43>>0])<<24,d=Q+-1958414417+((b<<12|b>>>20)+d)+(N&((s<<22|s>>>10)+I^I)^I)|0,b=(0|f[e+45>>0])<<8|0|f[e+44>>0]|(0|f[e+46>>0])<<16|(0|f[e+47>>0])<<24,G=((G=E+-42063+I+((d<<12|d>>>20)+N&(N^(s<<22|s>>>10)+I)^(s<<22|s>>>10)+I)|0)<<17|G>>>15)+((d<<12|d>>>20)+N)|0,w=(0|f[e+49>>0])<<8|0|f[e+48>>0]|(0|f[e+50>>0])<<16|(0|f[e+51>>0])<<24,I=b+-1990404162+((s<<22|s>>>10)+I)+(G&((d<<12|d>>>20)+N^N)^N)|0,N=(s=(0|f[e+53>>0])<<8|0|f[e+52>>0]|(0|f[e+54>>0])<<16|(0|f[e+55>>0])<<24)+-40341101+((d<<12|d>>>20)+N)+((h=((h=w+1804603682+N+((I<<22|I>>>10)+G&(G^(d<<12|d>>>20)+N)^(d<<12|d>>>20)+N)|0)<<7|h>>>25)+((I<<22|I>>>10)+G)|0)&((I<<22|I>>>10)+G^G)^G)|0,c=Q+-343485551+(((E=s+1309151649+(((k=g+-2054922799+(((I=U+-57434055+(((h=l+-995338651+(((N=c+76029189+(((D=E+-1094730640+(((G=(k=(0|f[e+57>>0])<<8|0|f[e+56>>0]|(0|f[e+58>>0])<<16|(0|f[e+59>>0])<<24)+-35309556+(((I=w+-1926607734+(((h=u+1163531501+(((N=o+-405537848+(((D=C+-373897302+(((G=(d=(0|f[e+61>>0])<<8|0|f[e+60>>0]|(0|f[e+62>>0])<<16|(0|f[e+63>>0])<<24)+1236535329+((I<<22|I>>>10)+G)+((D=((D=k+-1502002290+G+((N<<12|N>>>20)+h&(h^(I<<22|I>>>10)+G)^(I<<22|I>>>10)+G)|0)<<17|D>>>15)+((N<<12|N>>>20)+h)|0)&((N<<12|N>>>20)+h^h)^h)|0)<<22|G>>>10)+D)+(((N=((N=b+643717713+D+((((h=c+-1069501632+((N<<12|N>>>20)+h)+(((I=((I=g+-165796510+h+(((G<<22|G>>>10)+D^D)&(N<<12|N>>>20)+h^D)|0)<<5|I>>>27)+((G<<22|G>>>10)+D)|0)^(G<<22|G>>>10)+D)&D^(G<<22|G>>>10)+D)|0)<<9|h>>>23)+I^I)&(G<<22|G>>>10)+D^I)|0)<<14|N>>>18)+((h<<9|h>>>23)+I)|0)^(h<<9|h>>>23)+I)&I^(h<<9|h>>>23)+I)|0)<<20|D>>>12)+N)+(((h=((h=d+-660478335+N+((((I=E+38016083+((h<<9|h>>>23)+I)+(((G=((G=U+-701558691+I+(((D<<20|D>>>12)+N^N)&(h<<9|h>>>23)+I^N)|0)<<5|G>>>27)+((D<<20|D>>>12)+N)|0)^(D<<20|D>>>12)+N)&N^(D<<20|D>>>12)+N)|0)<<9|I>>>23)+G^G)&(D<<20|D>>>12)+N^G)|0)<<14|h>>>18)+((I<<9|I>>>23)+G)|0)^(I<<9|I>>>23)+G)&G^(I<<9|I>>>23)+G)|0)<<20|N>>>12)+h)+(((I=((I=M+-187363961+h+((((G=k+-1019803690+((I<<9|I>>>23)+G)+(((D=((D=Q+568446438+G+(((N<<20|N>>>12)+h^h)&(I<<9|I>>>23)+G^h)|0)<<5|D>>>27)+((N<<20|N>>>12)+h)|0)^(N<<20|N>>>12)+h)&h^(N<<20|N>>>12)+h)|0)<<9|G>>>23)+D^D)&(N<<20|N>>>12)+h^D)|0)<<14|I>>>18)+((G<<9|G>>>23)+D)|0)^(G<<9|G>>>23)+D)&D^(G<<9|G>>>23)+D)|0)<<20|h>>>12)+I)+(((G=((G=Z+1735328473+I+((((D=l+-51403784+((G<<9|G>>>23)+D)+(((N=((N=s+-1444681467+D+(((h<<20|h>>>12)+I^I)&(G<<9|G>>>23)+D^I)|0)<<5|N>>>27)+((h<<20|h>>>12)+I)|0)^(h<<20|h>>>12)+I)&I^(h<<20|h>>>12)+I)|0)<<9|D>>>23)+N^N)&(h<<20|h>>>12)+I^N)|0)<<14|G>>>18)+((D<<9|D>>>23)+N)|0)^(D<<9|D>>>23)+N)&N^(D<<9|D>>>23)+N)|0)<<20|I>>>12)+G)+(((N=u+-2022574463+((D<<9|D>>>23)+N)+((I<<20|I>>>12)+G^G^(h=((h=U+-378558+N+(G^(D<<9|D>>>23)+N^(I<<20|I>>>12)+G)|0)<<4|h>>>28)+((I<<20|I>>>12)+G)|0))|0)<<11|N>>>21)+h^h^(D=((D=b+1839030562+G+(h^(I<<20|I>>>12)+G^(N<<11|N>>>21)+h)|0)<<16|D>>>16)+((N<<11|N>>>21)+h)|0))|0)<<23|G>>>9)+D)+(((h=o+1272893353+((N<<11|N>>>21)+h)+((G<<23|G>>>9)+D^D^(I=((I=g+-1530992060+h+(D^(N<<11|N>>>21)+h^(G<<23|G>>>9)+D)|0)<<4|I>>>28)+((G<<23|G>>>9)+D)|0))|0)<<11|h>>>21)+I^I^(N=((N=Z+-155497632+D+(I^(G<<23|G>>>9)+D^(h<<11|h>>>21)+I)|0)<<16|N>>>16)+((h<<11|h>>>21)+I)|0))|0)<<23|D>>>9)+N)+(((I=C+-358537222+((h<<11|h>>>21)+I)+((D<<23|D>>>9)+N^N^(G=((G=s+681279174+I+(N^(h<<11|h>>>21)+I^(D<<23|D>>>9)+N)|0)<<4|G>>>28)+((D<<23|D>>>9)+N)|0))|0)<<11|I>>>21)+G^G^(h=((h=M+-722521979+N+(G^(D<<23|D>>>9)+N^(I<<11|I>>>21)+G)|0)<<16|h>>>16)+((I<<11|I>>>21)+G)|0))|0)<<23|N>>>9)+h)+(((G=w+-421815835+((I<<11|I>>>21)+G)+((N<<23|N>>>9)+h^h^(D=((D=Q+-640364487+G+(h^(I<<11|I>>>21)+G^(N<<23|N>>>9)+h)|0)<<4|D>>>28)+((N<<23|N>>>9)+h)|0))|0)<<11|G>>>21)+D^D^(I=((I=d+530742520+h+(D^(N<<23|N>>>9)+h^(G<<11|G>>>21)+D)|0)<<16|I>>>16)+((G<<11|G>>>21)+D)|0))|0)<<23|h>>>9)+I)+(((k=((k=k+-1416354905+I+((((D=Z+1126891415+((G<<11|G>>>21)+D)+(((C=((C=C+-198630844+D+(((h<<23|h>>>9)+I|~((G<<11|G>>>21)+D))^I)|0)<<6|C>>>26)+((h<<23|h>>>9)+I)|0)|~I)^(h<<23|h>>>9)+I)|0)<<10|D>>>22)+C|~((h<<23|h>>>9)+I))^C)|0)<<15|k>>>17)+((D<<10|D>>>22)+C)|0)|~C)^(D<<10|D>>>22)+C)|0)<<21|I>>>11)+k)+(((E=((E=E+-1051523+k+((((C=M+-1894986606+((D<<10|D>>>22)+C)+(((w=((w=w+1700485571+C+(((I<<21|I>>>11)+k|~((D<<10|D>>>22)+C))^k)|0)<<6|w>>>26)+((I<<21|I>>>11)+k)|0)|~k)^(I<<21|I>>>11)+k)|0)<<10|C>>>22)+w|~((I<<21|I>>>11)+k))^w)|0)<<15|E>>>17)+((C<<10|C>>>22)+w)|0)|~w)^(C<<10|C>>>22)+w)|0)<<21|k>>>11)+E)+(((c=((c=c+-1560198380+E+((((w=d+-30611744+((C<<10|C>>>22)+w)+(((u=((u=u+1873313359+w+(((k<<21|k>>>11)+E|~((C<<10|C>>>22)+w))^E)|0)<<6|u>>>26)+((k<<21|k>>>11)+E)|0)|~E)^(k<<21|k>>>11)+E)|0)<<10|w>>>22)+u|~((k<<21|k>>>11)+E))^u)|0)<<15|c>>>17)+((w<<10|w>>>22)+u)|0)|~u)^(w<<10|w>>>22)+u)|0)<<21|E>>>11)+c)+(((l=((l=l+718787259+c+((((u=b+-1120210379+((w<<10|w>>>22)+u)+(((o=((o=o+-145523070+u+(((E<<21|E>>>11)+c|~((w<<10|w>>>22)+u))^c)|0)<<6|o>>>26)+((E<<21|E>>>11)+c)|0)|~c)^(E<<21|E>>>11)+c)|0)<<10|u>>>22)+o|~((E<<21|E>>>11)+c))^o)|0)<<15|l>>>17)+((u<<10|u>>>22)+o)|0)|~o)^(u<<10|u>>>22)+o)|0,i=o+(0|t[A>>2])|0,t[A>>2]=i,n=l+(0|t[A+4>>2])+(c<<21|c>>>11)|0,t[A+4>>2]=n,a=l+(0|t[A+8>>2])|0,t[A+8>>2]=a,B=(u<<10|u>>>22)+o+(0|t[A+12>>2])|0,t[A+12>>2]=B,r;)e=e+64|0}function Qe(A,e){A|=0;var r=0;return r=0|t[(e|=0)+88>>2],i[e+24+r>>0]=-128,(r+1|0)>>>0>56?(Df(e+24+(r+1)|0,0,63-r|0),Ee(e,e+24|0,1),r=0):r=r+1|0,Df(e+24+r|0,0,56-r|0),r=0|t[e+16>>2],i[e+80>>0]=r,i[e+80+1>>0]=r>>>8,i[e+80+2>>0]=r>>>16,i[e+80+3>>0]=r>>>24,r=0|t[e+20>>2],i[e+84>>0]=r,i[e+84+1>>0]=r>>>8,i[e+84+2>>0]=r>>>16,i[e+84+3>>0]=r>>>24,Ee(e,e+24|0,1),t[e+88>>2]=0,$r(e+24|0,64),r=0|t[e>>2],i[A>>0]=r,i[A+1>>0]=r>>>8,i[A+2>>0]=r>>>16,i[A+3>>0]=r>>>24,r=0|t[e+4>>2],i[A+4>>0]=r,i[A+5>>0]=r>>>8,i[A+6>>0]=r>>>16,i[A+7>>0]=r>>>24,r=0|t[e+8>>2],i[A+8>>0]=r,i[A+9>>0]=r>>>8,i[A+10>>0]=r>>>16,i[A+11>>0]=r>>>24,e=0|t[e+12>>2],i[A+12>>0]=e,i[A+13>>0]=e>>>8,i[A+14>>0]=e>>>16,i[A+15>>0]=e>>>24,1}function we(A,e,r,n,a){A|=0,e|=0,r|=0,n|=0;var B,u,l,c=0,E=0,Q=0,w=0;if(l=G,G=G+512|0,c=(0|(E=(0|(a|=0))>0?a:0))<128?E:128,(0|a)>=1?(Df(0|l,32,0|c),i[l+c>>0]=0,(0|c)<6?(c=E,Q=4):a=6):(i[l>>0]=0,c=0,Q=4),4==(0|Q)&&(a=c),(0|(B=((0|n)/(0|(u=16-((c+3-a|0)/4|0)|0))|0)+((0|o(u,(0|n)/(0|u)|0))<(0|n)&1)|0))<=0)return G=l,0|(e=0);if((0|u)<=0){a=0,c=0;do{dt(l+176|0,l,289),t[l+496>>2]=0|o(a,u),Ge(l+144|0,20,54665,l+496|0),It(l+176|0,l+144|0,289),It(l+176|0,54687,289),It(l+176|0,80479,289),r=0|mt(l+176|0),c=(0|DB[31&A](l+176|0,r,e))+c|0,a=a+1|0}while((0|a)!=(0|B));return G=l,0|c}Q=0,c=0;do{dt(l+176|0,l,289),E=0|o(Q,u),t[l+472>>2]=E,Ge(l+144|0,20,54665,l+472|0),It(l+176|0,l+144|0,289),a=0;do{(a+E|0)<(0|n)?(t[l+488>>2]=f[r+E+a>>0],t[l+488+4>>2]=7==(0|a)?45:32,Ge(l+144|0,20,54680,l+488|0),It(l+176|0,l+144|0,289)):It(l+176|0,54676,289),a=a+1|0}while((0|a)!=(0|u));if(It(l+176|0,54687,289),(0|E)<(0|n)){a=0;do{w=0|i[r+E+a>>0],t[l+480>>2]=(w+-32&255)<95?255&w:46,Ge(l+144|0,20,54673,l+480|0),It(l+176|0,l+144|0,289),a=a+1|0}while((0|a)<(0|u)&(a+E|0)<(0|n))}It(l+176|0,80479,289),w=0|mt(l+176|0),c=(0|DB[31&A](l+176|0,w,e))+c|0,Q=Q+1|0}while((0|Q)!=(0|B));return G=l,0|c}function be(A,e,r){return 0|we(16,A|=0,e|=0,r|=0,0)}function ke(A){var e,r=0,i=0,n=0;return e=G,G=G+16|0,(A|=0)?(0|T(A+44|0,-1,e,0|t[A+68>>2]))<1?(G=e,0|(A=0)):(0|t[e>>2])>0?(G=e,0|(A=1)):0|(r=0|t[A+4>>2])&&(i=0|UB[7&r](A,1,0,0,0,1),t[e>>2]=i,(0|i)<1)?(G=e,0|(A=i)):(0|(r=0|t[A>>2])&&0|(n=0|t[r+32>>2])&&IB[31&n](A),bt(12,A,A+64|0),V(0|t[A+68>>2]),Xn(A,82796,0),G=e,0|(A=1)):(G=e,0|(A=0))}function se(A,e,r){e|=0,r|=0;var i,n=0,f=0,a=0;return(A|=0)?(i=0|t[A+4>>2],0|(f=0|t[A>>2])&&0|t[f+8>>2]?0|i&&(0|(n=0|UB[7&i](A,3,e,r,0,1)))<1?0|(f=n):0|t[A+12>>2]?((0|(n=0|DB[31&t[8+(0|t[A>>2])>>2]](A,e,r)))>0&&(a=0|cf(0|t[A+56>>2],0|t[A+56+4>>2],0|n,((0|n)<0)<<31>>31|0),f=0|E(),t[A+56>>2]=a,t[A+56+4>>2]=f),i?0|(a=0|UB[7&i](A,131,e,r,0,n)):0|(a=n)):(Xe(32,113,120,0,0),0|(f=-2)):(Xe(32,113,121,0,0),0|(a=-2))):0|(r=0)}function Ce(A,e){e|=0;var r=0,i=0,n=0,f=0;return 0|(A|=0)&&0|(r=0|t[A>>2])&&0|t[r+16>>2]?0|(i=0|t[A+4>>2])&&(0|(n=0|UB[7&i](A,4,e,0,0,1)))<1?0|(e=n):0|t[A+12>>2]?((0|(r=0|gB[63&t[16+(0|t[A>>2])>>2]](A,e)))>0&&(f=0|cf(0|t[A+56>>2],0|t[A+56+4>>2],0|r,((0|r)<0)<<31>>31|0),n=0|E(),t[A+56>>2]=f,t[A+56+4>>2]=n),i?0|(f=0|UB[7&i](A,132,e,0,0,r)):0|(f=r)):(Xe(32,110,120,0,0),0|(e=-2)):(Xe(32,110,121,0,0),0|(f=-2))}function de(A,e,r){e|=0,r|=0;var i=0,n=0,f=0;return 0|(A|=0)&&0|(i=0|t[A>>2])&&0|t[i+20>>2]?0|(n=0|t[A+4>>2])&&(0|(f=0|UB[7&n](A,5,e,r,0,1)))<1?0|(r=f):0|t[A+12>>2]?(i=0|DB[31&t[20+(0|t[A>>2])>>2]](A,e,r),n?0|(r=0|UB[7&n](A,133,e,r,0,i)):0|(r=i)):(Xe(32,104,120,0,0),0|(r=-2)):(Xe(32,104,121,0,0),0|(r=-2))}function Ie(A,e,r){if(A|=0,!(e=(0|(e=(0|(e|=0))>0?e:0))>(0|(r|=0))?r:e))return 0|(A=1);for(;;){if(e=e+-1|0,1!=(0|Ce(A,54690))){e=0,r=5;break}if(!e){e=1,r=5;break}}return 5==(0|r)?0|e:0}function ge(A,e,r){A|=0,e|=0,r|=0;var i;return i=G,G=G+16|0,t[i>>2]=r,r=0|function(A,e,r){A|=0,e|=0,r|=0;var i;if(i=G,G=G+2080|0,t[i+2060>>2]=i,t[i+2056>>2]=2048,t[i+2052>>2]=0,e=0==(0|De(i+2060|0,i+2052|0,i+2056|0,i+2064|0,i+2048|0,e,r)),r=0|t[i+2052>>2],e)return Xn(r,82796,0),G=i,0|(A=-1);return e=0|t[i+2064>>2],r?(A=0|se(A,r,e),Xn(0|t[i+2052>>2],82796,0),G=i,0|A):(A=0|se(A,i,e),G=i,0|A)}(A,e,i),G=i,0|r}function De(A,e,r,f,a,o,u){A|=0,e|=0,r|=0,f|=0,a|=0,o|=0,u|=0;var l,c=0,E=0,Q=0,w=0,b=0,k=0,s=0,C=0,d=0,I=0;l=G,G=G+16|0,t[l>>2]=0,b=0,k=0|i[o>>0],c=0,o=o+1|0,w=-1,s=0,E=0;A:for(;;){Q=k<<24>>24;e:do{if(k<<24>>24){if(0==(0|e)&&(0|t[l>>2])>>>0>=(0|t[r>>2])>>>0){Q=s,E=7;break}switch(0|E){case 0:if(k<<24>>24!=37){if(!(0|Me(A,e,l,r,Q))){c=0,d=79;break A}E=0}else E=1;k=0|i[o>>0],o=o+1|0,Q=s;break e;case 1:switch(0|Q){case 45:k=0|i[o>>0],c|=1,o=o+1|0,Q=s,E=1;break e;case 43:k=0|i[o>>0],c|=2,o=o+1|0,Q=s,E=1;break e;case 32:k=0|i[o>>0],c|=4,o=o+1|0,Q=s,E=1;break e;case 35:k=0|i[o>>0],c|=8,o=o+1|0,Q=s,E=1;break e;case 48:k=0|i[o>>0],c|=16,o=o+1|0,Q=s,E=1;break e;default:Q=s,E=2;break e}case 2:if(((255&k)-48|0)>>>0<10){k=0|i[o>>0],o=o+1|0,Q=Q+-48+(10*s|0)|0,E=2;break e}if(k<<24>>24!=42){Q=s,E=3;break e}k=3+(0|t[u>>2])&-4,Q=0|t[k>>2],t[u>>2]=k+4,k=0|i[o>>0],o=o+1|0,E=3;break e;case 3:if(k<<24>>24!=46){Q=s,E=5;break e}k=0|i[o>>0],o=o+1|0,Q=s,E=4;break e;case 4:if(((255&k)-48|0)>>>0<10){k=0|i[o>>0],o=o+1|0,w=Q+-48+(10*((0|w)>0?w:0)|0)|0,Q=s,E=4;break e}if(k<<24>>24!=42){Q=s,E=5;break e}k=3+(0|t[u>>2])&-4,w=0|t[k>>2],t[u>>2]=k+4,k=0|i[o>>0],o=o+1|0,Q=s,E=5;break e;case 5:switch(0|Q){case 104:b=1,k=0|i[o>>0],o=o+1|0,Q=s,E=6;break e;case 108:o=(b=108==(0|i[o>>0]))?o+1|0:o,b=b?4:2,k=0|i[o>>0],o=o+1|0,Q=s,E=6;break e;case 113:b=4,k=0|i[o>>0],o=o+1|0,Q=s,E=6;break e;case 76:b=3,k=0|i[o>>0],o=o+1|0,Q=s,E=6;break e;default:Q=s,E=6;break e}case 6:r:do{switch(0|Q){case 105:case 100:switch(0|b){case 1:E=3+(0|t[u>>2])&-4,Q=0|t[E>>2],t[u>>2]=E+4,E=(65535&Q)<<16>>16,Q=(((65535&Q)<<16>>16|0)<0)<<31>>31;break;case 2:E=3+(0|t[u>>2])&-4,Q=0|t[E>>2],t[u>>2]=E+4,E=Q,Q=((0|Q)<0)<<31>>31;break;case 4:k=7+(0|t[u>>2])&-8,E=0|t[k>>2],Q=0|t[k+4>>2],t[u>>2]=k+8;break;default:E=3+(0|t[u>>2])&-4,Q=0|t[E>>2],t[u>>2]=E+4,E=Q,Q=((0|Q)<0)<<31>>31}if(!(0|he(A,e,l,r,E,Q,10,s,w,c))){c=0,d=79;break A}break;case 88:c|=32,d=38;break;case 117:case 111:case 120:d=38;break;case 102:if(k=7+(0|t[u>>2])&-8,I=+B[k>>3],t[u>>2]=k+8,!(0|Ue(A,e,l,r,I,s,w,c,0))){c=0,d=79;break A}break;case 69:c|=32,d=46;break;case 101:d=46;break;case 71:c|=32,d=48;break;case 103:d=48;break;case 99:if(k=3+(0|t[u>>2])&-4,s=0|t[k>>2],t[u>>2]=k+4,!(0|Me(A,e,l,r,s))){c=0,d=79;break A}break;case 115:if(E=3+(0|t[u>>2])&-4,Q=0|t[E>>2],t[u>>2]=E+4,s=((w=s-(b=0|st(Q=0==(0|Q)?54692:Q,(k=(0|(E=(E=(0|w)>-1)|0!=(0|e)?E?w:2147483647:0|t[r>>2]))>-1)?E:-1))|0)|s|0)<0?0:w,k=k?(2147483647-s|0)>(0|E)?s+E|0:2147483647:E,(0|(E=0==(1&c|0)?s:0-s|0))>0&0!=(0|k))for(c=0,w=E;;){if(!(0|Me(A,e,l,r,32))){c=0,d=79;break A}if(E=w+-1|0,!((0|w)>1&k>>>0>(c=c+1|0)>>>0))break;w=E}else c=0;if(0!=(0|b)&k>>>0>c>>>0)for(w=b;;){if(!(0|Me(A,e,l,r,0|i[Q>>0]))){c=0,d=79;break A}if(!(k>>>0>(c=c+1|0)>>>0&0!=(0|(w=w+-1|0))))break;Q=Q+1|0}if((0|E)<0&k>>>0>c>>>0)for(;;){if(!(0|Me(A,e,l,r,32))){c=0,d=79;break A}if(!((0|E)<-1&k>>>0>(c=c+1|0)>>>0))break;E=E+1|0}break;case 112:if(b=3+(0|t[u>>2])&-4,k=0|t[b>>2],t[u>>2]=b+4,!(0|he(A,e,l,r,k,0,16,s,w,8|c))){c=0,d=79;break A}break;case 110:switch(0|b){case 1:k=3+(0|t[u>>2])&-4,s=0|t[k>>2],t[u>>2]=k+4,n[s>>1]=t[l>>2];break r;case 2:k=3+(0|t[u>>2])&-4,s=0|t[k>>2],t[u>>2]=k+4,t[s>>2]=t[l>>2];break r;case 4:k=3+(0|t[u>>2])&-4,s=0|t[k>>2],t[u>>2]=k+4,t[s>>2]=t[l>>2],t[s+4>>2]=0;break r;default:k=3+(0|t[u>>2])&-4,s=0|t[k>>2],t[u>>2]=k+4,t[s>>2]=t[l>>2];break r}case 37:if(!(0|Me(A,e,l,r,37))){c=0,d=79;break A}break;case 119:o=o+1|0}}while(0);if(38==(0|d)){switch(d=0,Q=64|c,0|b){case 1:E=3+(0|t[u>>2])&-4,c=0|t[E>>2],t[u>>2]=E+4,c&=65535,E=0;break;case 2:E=3+(0|t[u>>2])&-4,c=0|t[E>>2],t[u>>2]=E+4,E=0;break;case 4:b=7+(0|t[u>>2])&-8,c=0|t[b>>2],E=0|t[b+4>>2],t[u>>2]=b+8;break;default:E=3+(0|t[u>>2])&-4,c=0|t[E>>2],t[u>>2]=E+4,E=0}if(!(0|he(A,e,l,r,c,E,k<<24>>24==111?8:k<<24>>24==117?10:16,s,w,Q))){c=0,d=79;break A}}else if(46==(0|d)){if(d=0,k=7+(0|t[u>>2])&-8,I=+B[k>>3],t[u>>2]=k+8,!(0|Ue(A,e,l,r,I,s,w,c,1))){c=0,d=79;break A}}else if(48==(0|d)&&(d=0,k=7+(0|t[u>>2])&-8,I=+B[k>>3],t[u>>2]=k+8,!(0|Ue(A,e,l,r,I,s,w,c,2)))){c=0,d=79;break A}b=0,k=0|i[o>>0],c=0,o=o+1|0,w=-1,Q=0,E=0;break e;default:Q=s;break e}}else k=0,Q=s,E=7}while(0);if(7==(0|E)){d=74;break}s=Q}return 74==(0|d)?(0==(0|e)&&(C=(0|t[r>>2])-1|0,d=(0|t[l>>2])>>>0>C>>>0,t[a>>2]=1&d,d)&&(t[l>>2]=C),0|Me(A,e,l,r,0)?(t[f>>2]=(0|t[l>>2])-1,G=l,0|(d=1)):(G=l,0|(d=0))):79==(0|d)?(G=l,0|c):0}function Me(A,e,r,n,f){r|=0,n|=0,f|=0;var a=0,B=0,o=0;0!=(0|(e|=0))|0!=(0|t[(A|=0)>>2])||oi(54730,82796,0),(B=0|t[r>>2])>>>0>(a=0|t[n>>2])>>>0&&oi(54783,82796,0);do{if(0!=(0|e)&(0|B)==(0|a)){if(B>>>0>2147482623)return 0|(r=0);if(t[n>>2]=B+1024,0|(a=0|t[e>>2])){if(a=0|Tn(a,B+1024|0,82796,0)){t[e>>2]=a;break}return 0|(r=0)}if(o=0|Ln(B+1024|0,82796,0),t[e>>2]=o,!o)return 0|(r=0);a=0|t[r>>2];do{if(0|a){if(B=0|t[A>>2]){If(0|o,0|B,0|a);break}oi(54821,82796,0)}}while(0);t[A>>2]=0}}while(0);return(B=0|t[r>>2])>>>0>=(0|t[n>>2])>>>0?0|(r=1):(a=0|t[A>>2])?(t[r>>2]=B+1,i[a+B>>0]=f,0|(r=1)):(e=0|t[e>>2],t[r>>2]=B+1,i[e+B>>0]=f,0|(r=1))}function he(A,e,r,n,t,f,a,B,o,u){A|=0,e|=0,r|=0,n|=0,t|=0,f|=0,a|=0,B|=0,u|=0;var l,c,Q=0,w=0,b=0,k=0,s=0,C=0,d=0;c=G,G=G+32|0,l=(0|(o|=0))>0?o:0;do{if(!(64&u)){if((0|f)<0){t=0|Ef(0,0,0|t,0|f),f=0|E(),k=45;break}k=0==(2&u|0)?u<<3&32:43;break}k=0}while(0);for(b=16==(0|a)?54710:8==(0|a)?80614:82796,w=0==(32&u|0)?54713:1312,Q=0;d=0|Ef(0|(s=t),0|(C=f),0|(d=0|lf(0|(t=0|bf(0|t,0|f,0|a,0)),0|(f=0|E()),0|a,0)),0|E()),E(),o=Q+1|0,i[c+Q>>0]=0|i[w+d>>0],(C>>>0>0|0==(0|C)&s>>>0>=a>>>0)&o>>>0<26;)Q=o;w=0==(8&u|0)?82796:b,i[c+(t=26==(0|o)?Q:o)>>0]=0,f=(l-t|0)>0?l-t|0:0,d=(0|(d=((b=0!=(0|k))<<31>>31)+B-((0|l)<(0|t)?t:l)-(0|mt(w))|0))>0?d:0,o=0==(16&u|0)?d:0,f=0==(16&u|0)?f:(0|f)<(0|d)?d:f,o=0==(1&u|0)?o:0-o|0;A:do{if((0|o)>0){for(Q=o;;){if(!(0|Me(A,e,r,n,32))){o=0;break}if(o=Q+-1|0,!((0|Q)>1))break A;Q=o}return G=c,0|o}}while(0);if(b&&0==(0|Me(A,e,r,n,k)))return G=c,0|(d=0);Q=0|i[w>>0];A:do{if(Q<<24>>24){for(;;){if(w=w+1|0,!(0|Me(A,e,r,n,Q<<24>>24))){o=0;break}if(!((Q=0|i[w>>0])<<24>>24))break A}return G=c,0|o}}while(0);A:do{if((0|f)>0){for(;;){if(!(0|Me(A,e,r,n,48))){o=0;break}if(!((0|f)>1))break A;f=f+-1|0}return G=c,0|o}}while(0);A:do{if(0|t){for(;;){if(f=t,!(0|Me(A,e,r,n,0|i[c+(t=t+-1|0)>>0]))){o=0;break}if((0|f)<=1)break A}return G=c,0|o}}while(0);if((0|o)>=0)return G=c,0|(d=1);for(;;){if(!(0|Me(A,e,r,n,32))){o=0,t=29;break}if(!((0|o)<-1)){o=1,t=29;break}o=o+1|0}return 29==(0|t)?(G=c,0|o):0}function Ue(A,e,r,n,t,f,a,B,o){A|=0,e|=0,r|=0,n|=0,f|=0,B|=0,o|=0;var u,l=0,c=0,E=0,Q=0,w=0,b=0,k=0,s=0,C=0,d=0,I=0;u=G,G=G+96|0,E=(0|(a|=0))<0?6:a,d=(t=+t)<0?45:0==(2&B|0)?B<<3&32:43;do{if(2==(0|o))if(0!=t)if(t<1e-4||t>=10&0==(0|E))l=1,I=13;else{if((0|E)>0){a=E,w=1;do{w*=10,a=a+-1|0}while(0!=(0|a));if(w<=t){l=1,I=12;break}}l=0,I=12}else l=0,I=13;else l=o,I=12}while(0);if(12==(0|I)&&(o?I=13:(C=0,w=t,a=E,k=l)),13==(0|I)){if(0!=t){if(t<1){a=0,w=t;do{w*=10,a=a+-1|0}while(w<1)}else a=0,w=t;if(w>10){do{w/=10,a=a+1|0}while(w>10);c=a}else c=a}else c=0,w=t;if(2==(0|o)){if(a=0==(0|E)?0:E+-1|0,!l&&(0|(a=a-c|0))<0)return G=u,0|(A=0)}else a=E;C=c,w=1==(0|l)?w:t,k=l}if((t=w<0?-w:w)>4294967295)return G=u,0|(A=0);if(b=(0|a)<9?a:9){l=b,w=1;do{w*=10,l=l+-1|0}while(0!=(0|l));c=(w-+(0|(c=~~w))>=.5&1)+c|0,l=b,w=1;do{w*=10,l=l+-1|0}while(0!=(0|l))}else c=1,w=1;for(Q=(l=(((w*=t-+(~~t>>>0>>>0))-+(0|~~w)>=.5&1)+~~w|0)>>>0<c>>>0)?0:c,l=(1&(1^l))+(~~t>>>0)|0,E=0;s=l,l=(l>>>0)/10|0,c=E+1|0,i[u+64+E>>0]=0|i[s-(10*l|0)+54699>>0],s>>>0>9&c>>>0<20;)E=c;l=(w-+(0|~~w)>=.5&1)+~~w-Q|0,i[u+64+(Q=20==(0|c)?E:c)>>0]=0;A:do{if((0|a)>0){if(2!=(0|o))for(c=0,a=l;;){if(s=a,a=(a>>>0)/10|0,l=c+1|0,i[u+32+c>>0]=0|i[s-(10*a|0)+54699>>0],(0|l)>=(0|b)){c=b;break A}c=l}for(a=l;;){for(l=0;(c=a-(10*(E=(a>>>0)/10|0)|0)|0)|l;){if(a=l+1|0,i[u+32+l>>0]=0|i[54699+c>>0],!((0|a)<(0|b))){l=a,c=b;break A}l=a,a=E}if((0|l)>=(0|(c=b+-1|0)))break A;if(!((0|b)>1)){l=0;break}a=(a>>>0)/10|0,b=c}}else l=0,c=b}while(0);if(i[u+32+(E=l+((20==(0|l))<<31>>31)|0)>>0]=0,s=1==(0|k)){for(a=0,b=(0|C)<0?0-C|0:C;k=b,b=(0|b)/10|0,l=a+1|0,i[u+a>>0]=0|i[k-(10*b|0)+54699>>0],(0|k)>9&l>>>0<20;)a=l;if((0|k)>9)return G=u,0|(A=0);a||(i[u+l>>0]=48,l=2)}else l=0;a=((b=0!=(0|d))<<31>>31)+f-Q-c+((o=(0|c)>0)<<31>>31)+(s?-2-l|0:0)|0,c=c-E|0,a=(0|a)>0?a:0,a=0==(1&B|0)?a:0-a|0;A:do{if(0!=(16&B|0)&(0|a)>0){do{if(b){if(0|Me(A,e,r,n,d)){if((0|a)>1){a=a+-1|0;break}a=0;break A}return G=u,0|(A=0)}}while(0);for(b=a;;){if(!(0|Me(A,e,r,n,48))){a=0;break}if(a=b+-1|0,!((0|b)>1)){k=0,I=55;break A}b=a}return G=u,0|a}k=d,I=55}while(0);if(55==(0|I)){A:do{if((0|a)>0){for(b=a;;){if(!(0|Me(A,e,r,n,32))){a=0;break}if(a=b+-1|0,!((0|b)>1))break A;b=a}return G=u,0|a}}while(0);if(0!=(0|k)&&0==(0|Me(A,e,r,n,k)))return G=u,0|(A=0)}A:do{if(0|Q){for(;;){if(b=Q,!(0|Me(A,e,r,n,0|i[u+64+(Q=Q+-1|0)>>0]))){a=0;break}if((0|b)<=1)break A}return G=u,0|a}}while(0);A:do{if(!(0==(8&B|0)&(1^o))){if(!(0|Me(A,e,r,n,46)))return G=u,0|(A=0);if((0|E)>0){for(;;){if(Q=E,!(0|Me(A,e,r,n,0|i[u+32+(E=E+-1|0)>>0]))){a=0;break}if((0|Q)<=1)break A}return G=u,0|a}}}while(0);A:do{if((0|c)>0){for(;;){if(!(0|Me(A,e,r,n,48))){a=0;break}if(!((0|c)>1))break A;c=c+-1|0}return G=u,0|a}}while(0);A:do{if(s){if(!(0|Me(A,e,r,n,32&B^101)))return G=u,0|(A=0);if((0|C)<0){if(!(0|Me(A,e,r,n,45)))return G=u,0|(A=0)}else if(!(0|Me(A,e,r,n,43)))return G=u,0|(A=0);if((0|l)>0){for(;;){if(c=l,!(0|Me(A,e,r,n,0|i[u+(l=l+-1|0)>>0]))){a=0;break}if((0|c)<=1)break A}return G=u,0|a}}}while(0);if((0|a)>=0)return G=u,0|(A=1);for(;;){if(!(0|Me(A,e,r,n,32))){a=0,I=88;break}if(!((0|a)<-1)){a=1,I=88;break}a=a+1|0}return 88==(0|I)?(G=u,0|a):0}function Ge(A,e,r,i){A|=0,e|=0,r|=0,i|=0;var n;return n=G,G=G+32|0,t[n>>2]=i,t[n+28>>2]=A,t[n+24>>2]=e,(i=0!=(0|De(n+28|0,0,n+24|0,n+20|0,n+16|0,r,n)))&0==(0|t[n+16>>2])?(i=0|t[n+20>>2],G=n,0|(i=(0|i)>-1?i:-1)):(G=n,0|(i=-1))}function Ze(A,e){A|=0,e|=0;var r=0,i=0;if(!(r=0|Sn(100,82796,0)))return 0|(r=0);i=0|Sn(64,82796,0),t[r>>2]=i;do{if(i){if(i=0|W(),t[r+12>>2]=i,!i){e=0|t[r>>2];break}return t[r+4>>2]=0==(0|e)?28:e,t[r+8>>2]=0==(0|A)?17:A,t[r+16>>2]=8,t[r+20>>2]=16,t[r+28>>2]=8,t[r+32>>2]=512,t[r+36>>2]=256,0|(i=r)}e=0}while(0);return Xn(e,82796,0),Xn(r,82796,0),0|(i=0)}function Ne(A){var e=0,r=0,n=0,t=0,f=0;if(!(A|=0))return 0|(n=0);if(!((e=0|i[A>>0])<<24>>24))return 0|(n=0);for(r=256,n=0;n=(n>>>(32-(15&((f=e<<24>>24)>>>2^f))|0)|n<<(15&(f>>>2^f)))^(0|o(t=r|f,t)),(e=0|i[(A=A+1|0)>>0])<<24>>24;)r=r+256|0;return 0|(f=n>>>16^n)}function Fe(A){var e=0,r=0,i=0;if(A|=0){if(0|(e=0|t[A+16>>2])){i=0;do{if(r=0|t[(0|t[A>>2])+(i<<2)>>2]){e=r;do{r=e,e=0|t[e+4>>2],Xn(r,82796,0)}while(0!=(0|e));e=0|t[A+16>>2]}i=i+1|0}while(i>>>0<e>>>0)}V(0|t[A+12>>2]),Xn(0|t[A>>2],82796,0),Xn(A,82796,0)}}function We(A,e){e|=0;var r,i=0,n=0,f=0,a=0,B=0,o=0,u=0;if(r=G,G=G+16|0,t[(A|=0)+96>>2]=0,i=0|t[A+16>>2],(0|t[A+32>>2])>>>0<=((t[A+40>>2]<<8>>>0)/(i>>>0)|0)>>>0){B=0|t[A+20>>2],o=0|t[A+24>>2],u=0|t[A+28>>2];do{if((o+1|0)>>>0>=u>>>0){if(i=0|Tn(0|t[A>>2],B<<3,82796,0)){t[A>>2]=i,Df(i+(B<<2)|0,0,B<<2|0),t[A+28>>2]=B,t[A+20>>2]=B<<1,t[A+48>>2]=1+(0|t[A+48>>2]),i=0|t[A+16>>2],n=0;break}return e=A+96|0,A=0,u=(u=0|t[e>>2])+1|0,t[e>>2]=u,G=r,0|A}n=o+1|0}while(0);if(t[A+24>>2]=n,t[A+16>>2]=i+1,t[A+44>>2]=1+(0|t[A+44>>2]),a=0|t[A>>2],t[a+(u+o<<2)>>2]=0,0|(i=0|t[a+(o<<2)>>2]))for(n=a+(o<<2)|0,f=i;i=f+4|0,(0|((0|t[f+8>>2])>>>0)%(B>>>0))!=(0|o)&&(t[n>>2]=t[i>>2],t[i>>2]=t[a+(u+o<<2)>>2],t[a+(u+o<<2)>>2]=f,i=n),f=0|t[i>>2];)n=i}return n=0|Ye(A,e,r),0|(i=0|t[n>>2])?(u=0|t[i>>2],t[i>>2]=e,e=A+72|0,A=u,u=(u=0|t[e>>2])+1|0,t[e>>2]=u,G=r,0|A):(i=0|Ln(12,82796,0))?(t[i>>2]=e,t[i+4>>2]=0,t[i+8>>2]=t[r>>2],t[n>>2]=i,t[A+68>>2]=1+(0|t[A+68>>2]),e=A+40|0,A=0,u=(u=0|t[e>>2])+1|0,t[e>>2]=u,G=r,0|A):(e=A+96|0,A=0,u=(u=0|t[e>>2])+1|0,t[e>>2]=u,G=r,0|A)}function Ye(A,e,r){e|=0,r|=0;var i,n,f,a=0;if(f=G,G=G+16|0,n=0|IB[31&t[(A|=0)+8>>2]](e),T(A+60|0,1,f,0|t[A+12>>2]),t[r>>2]=n,(r=(n>>>0)%((0|t[A+28>>2])>>>0)|0)>>>0<(0|t[A+24>>2])>>>0&&(r=(n>>>0)%((0|t[A+20>>2])>>>0)|0),i=0|t[A+4>>2],r=(0|t[A>>2])+(r<<2)|0,!(a=0|t[r>>2]))return G=f,0|(A=r);for(;;){if(T(A+92|0,1,f,0|t[A+12>>2]),(0|t[a+8>>2])==(0|n)&&(T(A+64|0,1,f,0|t[A+12>>2]),0==(0|gB[63&i](0|t[a>>2],e)))){a=8;break}if(!(a=0|t[(r=a+4|0)>>2])){a=8;break}}return 8==(0|a)?(G=f,0|r):0}function Re(A,e){e|=0;var r,i,n,f=0,a=0,B=0;if(n=G,G=G+16|0,t[(A|=0)+96>>2]=0,e=0|Ye(A,e,n),!(f=0|t[e>>2]))return t[A+80>>2]=1+(0|t[A+80>>2]),G=n,0|(B=0);if(t[e>>2]=t[f+4>>2],B=0|t[f>>2],Xn(f,82796,0),t[A+76>>2]=1+(0|t[A+76>>2]),e=(0|t[A+40>>2])-1|0,t[A+40>>2]=e,(f=0|t[A+16>>2])>>>0<=16)return G=n,0|B;if((0|t[A+36>>2])>>>0<((e<<8>>>0)/(f>>>0)|0)>>>0)return G=n,0|B;r=0|t[A+24>>2],e=0|t[A+28>>2],a=(0|t[A>>2])+(r+-1+e<<2)|0,i=0|t[a>>2],t[a>>2]=0;do{if(!r){if(0|(e=0|Tn(0|t[A>>2],e<<2,82796,0))){t[A+56>>2]=1+(0|t[A+56>>2]),t[A+20>>2]=(0|t[A+20>>2])>>>1,f=(0|t[A+28>>2])>>>1,t[A+28>>2]=f,t[A+24>>2]=f+-1,t[A>>2]=e,a=0|t[A+16>>2],f=f+-1|0;break}return t[A+96>>2]=1+(0|t[A+96>>2]),G=n,0|B}t[A+24>>2]=r+-1,a=f,e=0|t[A>>2],f=r+-1|0}while(0);if(t[A+16>>2]=a+-1,t[A+52>>2]=1+(0|t[A+52>>2]),!(e=0|t[(f=e+(f<<2)|0)>>2]))return t[f>>2]=i,G=n,0|B;for(;f=0|t[e+4>>2];)e=f;return t[e+4>>2]=i,G=n,0|B}function ye(A,e){e|=0;var r;return r=G,G=G+16|0,t[(A|=0)+96>>2]=0,(e=0|t[(0|Ye(A,e,r+4|0))>>2])?(e=0|t[e>>2],T(A+84|0,1,r,0|t[A+12>>2]),G=r,0|(A=e)):(T(A+88|0,1,r,0|t[A+12>>2]),G=r,0|(A=0))}function Ve(A,e){e|=0;var r=0,i=0,n=0;if((A|=0)&&!((0|(r=(0|t[A+16>>2])-1|0))<=-1))do{if(0|(i=0|t[(0|t[A>>2])+(r<<2)>>2]))do{n=i,i=0|t[i+4>>2],NB[31&e](0|t[n>>2])}while(0!=(0|i));r=r+-1|0}while((0|r)>-1)}function me(A,e,r){e|=0,r|=0;var i=0,n=0,f=0;if((A|=0)&&!((0|(i=(0|t[A+16>>2])-1|0))<=-1))do{if(0|(n=0|t[(0|t[A>>2])+(i<<2)>>2]))do{f=n,n=0|t[n+4>>2],FB[7&e](0|t[f>>2],r)}while(0!=(0|n));i=i+-1|0}while((0|i)>-1)}function He(A){var e=0,r=0,n=0,t=0,f=0;if(!(A|=0))return 0|(n=0);if(!((e=0|i[A>>0])<<24>>24))return 0|(n=0);for(r=256,n=0;n=(n>>>(32-(15&((f=0|Vt(e<<24>>24))>>>2^f))|0)|n<<(15&(f>>>2^f)))^(0|o(t=f|r,t)),(e=0|i[(A=A+1|0)>>0])<<24>>24;)r=r+256|0;return 0|(f=n>>>16^n)}function ve(A,e){e|=0,t[(A|=0)+36>>2]=e}function Je(A){return 0|t[(A|=0)+96>>2]}function Xe(A,e,r,i,n){A|=0,e|=0,r|=0,i|=0,n|=0;var f,a=0;(f=0|Te())&&(a=(1+(0|t[f+384>>2])|0)%16|0,t[f+384>>2]=a,(0|a)==(0|t[f+388>>2])&&(t[f+388>>2]=(a+1|0)%16|0),t[f+(a<<2)>>2]=0,a=0|t[f+384>>2],t[f+64+(a<<2)>>2]=e<<12&16773120|A<<24|4095&r,t[f+256+(a<<2)>>2]=i,t[f+320+(a<<2)>>2]=n,i=0|t[f+384>>2],1&t[f+192+(i<<2)>>2]?(Xn(0|t[f+128+(i<<2)>>2],82796,0),i=0|t[f+384>>2],t[f+128+(i<<2)>>2]=0,i=f+192+(i<<2)|0):i=f+192+(i<<2)|0,t[i>>2]=0)}function Te(){var A=0;if(!(0|Kn(262144,0,0)))return 0|(A=0);if(!((A=0!=(0|m(82056,1)))&0!=(0|t[20515])))return 0|(A=0);if(-1==(0|(A=0|v(82052))))return 0|(A=0);if(0|A)return 0|A;if(!(0|J(82052,-1)))return 0|(A=0);if(!(A=0|Sn(392,82796,0)))return J(82052,0),0|(A=0);do{if(0|zn(2,0)){if(!(0|J(82052,A))){Le(A);break}return Kn(2,0,0),0|A}Le(A)}while(0);return J(82052,0),0|(A=0)}function Le(A){(A|=0)&&(1&t[A+192>>2]|0&&(Xn(0|t[A+128>>2],82796,0),t[A+128>>2]=0),t[A+192>>2]=0,1&t[A+196>>2]|0&&(Xn(0|t[A+132>>2],82796,0),t[A+132>>2]=0),t[A+196>>2]=0,1&t[A+200>>2]|0&&(Xn(0|t[A+136>>2],82796,0),t[A+136>>2]=0),t[A+200>>2]=0,1&t[A+204>>2]|0&&(Xn(0|t[A+140>>2],82796,0),t[A+140>>2]=0),t[A+204>>2]=0,1&t[A+208>>2]|0&&(Xn(0|t[A+144>>2],82796,0),t[A+144>>2]=0),t[A+208>>2]=0,1&t[A+212>>2]|0&&(Xn(0|t[A+148>>2],82796,0),t[A+148>>2]=0),t[A+212>>2]=0,1&t[A+216>>2]|0&&(Xn(0|t[A+152>>2],82796,0),t[A+152>>2]=0),t[A+216>>2]=0,1&t[A+220>>2]|0&&(Xn(0|t[A+156>>2],82796,0),t[A+156>>2]=0),t[A+220>>2]=0,1&t[A+224>>2]|0&&(Xn(0|t[A+160>>2],82796,0),t[A+160>>2]=0),t[A+224>>2]=0,1&t[A+228>>2]|0&&(Xn(0|t[A+164>>2],82796,0),t[A+164>>2]=0),t[A+228>>2]=0,1&t[A+232>>2]|0&&(Xn(0|t[A+168>>2],82796,0),t[A+168>>2]=0),t[A+232>>2]=0,1&t[A+236>>2]|0&&(Xn(0|t[A+172>>2],82796,0),t[A+172>>2]=0),t[A+236>>2]=0,1&t[A+240>>2]|0&&(Xn(0|t[A+176>>2],82796,0),t[A+176>>2]=0),t[A+240>>2]=0,1&t[A+244>>2]|0&&(Xn(0|t[A+180>>2],82796,0),t[A+180>>2]=0),t[A+244>>2]=0,1&t[A+248>>2]|0&&(Xn(0|t[A+184>>2],82796,0),t[A+184>>2]=0),t[A+248>>2]=0,1&t[A+252>>2]|0&&(Xn(0|t[A+188>>2],82796,0),t[A+188>>2]=0),t[A+252>>2]=0,Xn(A,82796,0))}function Se(){var A,e=0,r=0,i=0;if(A=0|Te()){i=0;do{t[A+(i<<2)>>2]=0,t[A+64+(i<<2)>>2]=0,r=A+128+(i<<2)|0,1&t[(e=A+192+(i<<2)|0)>>2]|0&&(Xn(0|t[r>>2],82796,0),t[r>>2]=0),t[e>>2]=0,t[A+256+(i<<2)>>2]=0,t[A+320+(i<<2)>>2]=-1,i=i+1|0}while(16!=(0|i));t[A+388>>2]=0,t[A+384>>2]=0}}function pe(){return 0|function(A,e,r,i,n,f){A|=0,e|=0,r|=0,i|=0,n|=0,f|=0;var a,B=0,o=0,u=0,l=0;if(!(a=0|Te()))return 0|(f=0);if(0!=(0|A)&0!=(0|e))return 0|r&&(t[r>>2]=82796),0|i&&(t[i>>2]=0),0|n&&(t[n>>2]=82796),f?(t[f>>2]=0,0|(f=68)):0|(f=68);if((0|(B=0|t[a+388>>2]))==(0|(o=0|t[a+384>>2])))return 0|(f=0);for(;;){if(2&t[(u=a+(o<<2)|0)>>2])t[u>>2]=0,B=0|t[a+384>>2],t[a+64+(B<<2)>>2]=0,1&t[a+192+(B<<2)>>2]?(Xn(0|t[a+128+(B<<2)>>2],82796,0),B=0|t[a+384>>2],t[a+128+(B<<2)>>2]=0,B=a+192+(B<<2)|0):B=a+192+(B<<2)|0,t[B>>2]=0,o=0|t[a+384>>2],t[a+256+(o<<2)>>2]=0,t[a+320+(o<<2)>>2]=-1,o=(0|(o=0|t[a+384>>2]))>0?o+-1|0:15,t[a+384>>2]=o;else{if(!(2&t[a+((B=(B+1|0)%16|0)<<2)>>2]))break;t[a+388>>2]=B,t[a+(B<<2)>>2]=0,B=0|t[a+388>>2],t[a+64+(B<<2)>>2]=0,1&t[a+192+(B<<2)>>2]?(Xn(0|t[a+128+(B<<2)>>2],82796,0),B=0|t[a+388>>2],t[a+128+(B<<2)>>2]=0,B=a+192+(B<<2)|0):B=a+192+(B<<2)|0,t[B>>2]=0,o=0|t[a+388>>2],t[a+256+(o<<2)>>2]=0,t[a+320+(o<<2)>>2]=-1,o=0|t[a+384>>2]}if((0|(B=0|t[a+388>>2]))==(0|o)){B=0,l=39;break}}return 39==(0|l)?0|B:(o=0|t[a+64+((u=0|e?o:B)<<2)>>2],0|A&&(t[a+388>>2]=u,t[a+64+(u<<2)>>2]=0),0!=(0|r)&0!=(0|i)&&((B=0|t[a+256+(u<<2)>>2])?(t[r>>2]=B,B=0|t[a+320+(u<<2)>>2]):(t[r>>2]=54856,B=0),t[i>>2]=B),n?(B=0|t[a+128+(u<<2)>>2])?(t[n>>2]=B,f?(t[f>>2]=t[a+192+(u<<2)>>2],0|(f=o)):0|(f=o)):(t[n>>2]=82796,f?(t[f>>2]=0,0|(f=o)):0|(f=o)):A?(1&t[a+192+(u<<2)>>2]|0&&(Xn(0|t[a+128+(u<<2)>>2],82796,0),t[a+128+(u<<2)>>2]=0),t[a+192+(u<<2)>>2]=0,0|(f=o)):0|(f=o))}(0,1,0,0,0,0)}function ze(){var A;(A=0|v(82052))&&(J(82052,0),Le(A))}function Ke(A,e){A|=0,e|=0;var r;r=G,G=G+16|0,t[r>>2]=e,je(A,r),G=r}function je(A,e){A|=0,e|=0;var r=0,n=0,f=0,a=0,B=0,o=0;if(f=0|Ln(81,82796,0)){i[f>>0]=0;A:do{if((0|A)>0){for(o=0,n=0,r=80;;){if(a=3+(0|t[e>>2])&-4,B=0|t[a>>2],t[e>>2]=a+4,B){if((0|(n=(0|mt(B))+n|0))>(0|r)){if(!(r=0|Tn(f,n+21|0,82796,0)))break;a=n+20|0,f=r}else a=r;It(f,B,a+1|0),r=a}if((0|(o=o+1|0))>=(0|A))break A}return void Xn(f,82796,0)}}while(0);(n=0|Te())&&(r=0|t[n+384>>2],1&t[n+192+(r<<2)>>2]|0&&(Xn(0|t[n+128+(r<<2)>>2],82796,0),t[n+128+(r<<2)>>2]=0),t[n+128+(r<<2)>>2]=f,t[n+192+(r<<2)>>2]=3)}}function xe(){var A,e=0;return(e=0|Te())?(A=0|t[e+384>>2],(0|t[e+388>>2])==(0|A)?0|(e=0):(t[e+(A<<2)>>2]=1|t[e+(A<<2)>>2],0|(e=1))):0|(e=0)}function Oe(){var A,e=0,r=0,i=0;if(!(A=0|Te()))return 0|(i=0);if(e=0|t[A+384>>2],(0|t[A+388>>2])==(0|e))return 0|(i=0);for(;!(1&(e=0|t[(r=A+(e<<2)|0)>>2])|0);)if(t[r>>2]=0,e=0|t[A+384>>2],t[A+64+(e<<2)>>2]=0,1&t[A+192+(e<<2)>>2]?(Xn(0|t[A+128+(e<<2)>>2],82796,0),e=0|t[A+384>>2],t[A+128+(e<<2)>>2]=0,e=A+192+(e<<2)|0):e=A+192+(e<<2)|0,t[e>>2]=0,e=0|t[A+384>>2],t[A+256+(e<<2)>>2]=0,t[A+320+(e<<2)>>2]=-1,e=0==(0|(e=0|t[A+384>>2]))?15:e+-1|0,t[A+384>>2]=e,(0|t[A+388>>2])==(0|e)){e=0,i=9;break}return 9==(0|i)?0|e:(t[r>>2]=-2&e,0|(i=1))}function Pe(A){A|=0;var e,r,i;r=G,G=G+16|0,(e=0|Te())?(i=0|t[e+384>>2],t[r>>2]=(A+-1&(-2147483648^A))>>31,t[r>>2]=~((A+-1&(-2147483648^A))>>31),t[e+(i<<2)>>2]=t[e+(i<<2)>>2]|2&t[r>>2],G=r):G=r}function _e(A,e,r,n){return A|=0,r|=0,((e|=0)-(n|=0)|0)<2?(Xe(4,127,110,0,0),0|(r=-1)):(2==(e-n|0)?(i[A>>0]=106,e=A+1|0):(i[A>>0]=107,(e-n|0)>3?(Df(A+1|0,-69,e-n-3|0),e=A+1+(e-n-3)|0):e=A+1|0,i[e>>0]=-70,e=e+1|0),If(0|e,0|r,0|n),i[e+n>>0]=-52,0|(r=1))}function qe(A,e,r,n,t){A|=0,e|=0,r|=0;var f=0;if((0|(t|=0))==(0|(n|=0))&&(-2&(f=0|i[r>>0]))<<24>>24==106){A:do{if(f<<24>>24==107){do{if((0|t)>3){n=0,f=r+1|0;e:for(;;){switch(r=f,f=f+1|0,0|i[r>>0]){case-70:e=11;break e;case-69:break;default:e=8;break e}if((t+-3|0)<=(0|(n=n+1|0))){e=10;break}}if(8==(0|e))return Xe(4,128,138,0,0),0|(A=-1);if(10==(0|e)){n=t+-3-n|0;break A}if(11==(0|e)){if(n){n=t+-3-n|0;break A}break}}}while(0);return Xe(4,128,138,0,0),0|(A=-1)}n=t+-2|0,f=r+1|0}while(0);return-52==(0|i[f+n>>0])?(If(0|A,0|f,0|n),0|(A=n)):(Xe(4,128,139,0,0),0|(A=-1))}return Xe(4,128,137,0,0),0|(A=-1)}function $e(A){switch(0|(A|=0)){case 64:A=51;break;case 672:A=52;break;case 673:A=54;break;case 674:A=53;break;default:A=-1}return 0|A}function Ar(A,e,r,i){A|=0,r|=0,i|=0;var n,f=0,a=0;n=G,G=G+16|0,f=(f=0|t[(e|=0)+16>>2])?0|Kf(f):0;do{if(0|Ie(A,r,128)){if(0|i&&0!=(0|t[e+24>>2])){if(t[n>>2]=f,(0|ge(A,55073,n))<1){f=0;break}f=55136,a=55118}else{if(t[n+8>>2]=f,(0|ge(A,55096,n+8|0))<1){f=0;break}f=55152,a=55127}if(0!=(0|IA(A,a,0|t[e+16>>2],0,r))&&0!=(0|IA(A,f,0|t[e+20>>2],0,r))){if(0|i){if(!(0|IA(A,55162,0|t[e+24>>2],0,r))){f=0;break}if(!(0|IA(A,55179,0|t[e+28>>2],0,r))){f=0;break}if(!(0|IA(A,55187,0|t[e+32>>2],0,r))){f=0;break}if(!(0|IA(A,55195,0|t[e+36>>2],0,r))){f=0;break}if(!(0|IA(A,55206,0|t[e+40>>2],0,r))){f=0;break}if(!(0|IA(A,55217,0|t[e+44>>2],0,r))){f=0;break}}f=1}else f=0}else f=0}while(0);return G=n,0|f}function er(A,e,r,i){e|=0,r|=0,i|=0;var n=0,f=0,a=0,B=0,o=0,u=0;if(0|(n=0|t[52+(0|t[(A|=0)+8>>2])>>2]))return 0|(u=0|MB[31&n](A,e,r,i));if((0|e)>=16){f=0|eB();A:do{if(0!=(0|f)&&(iB(f),a=0|tB(f),B=0|tB(f),o=0|tB(f),0!=(0|tB(f)))){if(0==(0|t[A+16>>2])&&(n=0|qf(),t[A+16>>2]=n,0==(0|n))){u=56;break}if(0==(0|t[A+24>>2])&&(n=0|$f(),t[A+24>>2]=n,0==(0|n))){u=56;break}if(0==(0|t[A+20>>2])&&(n=0|qf(),t[A+20>>2]=n,0==(0|n))){u=56;break}if(0==(0|t[A+28>>2])&&(n=0|$f(),t[A+28>>2]=n,0==(0|n))){u=56;break}if(0==(0|t[A+32>>2])&&(n=0|$f(),t[A+32>>2]=n,0==(0|n))){u=56;break}if(0==(0|t[A+36>>2])&&(n=0|$f(),t[A+36>>2]=n,0==(0|n))){u=56;break}if(0==(0|t[A+40>>2])&&(n=0|$f(),t[A+40>>2]=n,0==(0|n))){u=56;break}if(0==(0|t[A+44>>2])&&(n=0|$f(),t[A+44>>2]=n,0==(0|n))){u=56;break}if(0!=(0|ra(0|t[A+20>>2],r))&&(ha(0|t[A+28>>2],4),ha(0|t[A+32>>2],4),ha(o,4),0!=(0|Ka(0|t[A+28>>2],(e+1|0)/2|0,0,0,0,i)))){for(n=0;;){if(!(0|cn(o,r=0|t[A+28>>2],52964))){u=56;break A}if(xe(),0|Hf(B,o,0|t[A+20>>2],f))break;if(50331756!=(-16773121&(0|pe())|0)){u=56;break A}if(Oe(),!(0|za(i,2,n))){u=56;break A}if(!(0|Ka(0|t[A+28>>2],(e+1|0)/2|0,0,0,0,i))){u=56;break A}n=n+1|0}if(0|za(i,3,0)){for(;;){do{if(!(0|Ka(0|t[A+32>>2],e-((e+1|0)/2|0)|0,0,0,0,i))){u=56;break A}}while(!(0|ca(0|t[A+28>>2],0|t[A+32>>2])));if(!(0|cn(o,r=0|t[A+32>>2],52964))){u=56;break A}if(xe(),0|Hf(B,o,0|t[A+20>>2],f))break;if(50331756!=(-16773121&(0|pe())|0)){u=56;break A}if(Oe(),!(0|za(i,2,n))){u=56;break A}n=n+1|0}if(0|za(i,3,1))if((0|ca(0|t[A+28>>2],0|t[A+32>>2]))<0?(r=0|t[A+28>>2],n=0|t[A+32>>2],t[A+28>>2]=n,t[A+32>>2]=r):(n=0|t[A+28>>2],r=0|t[A+32>>2]),0|Sf(0|t[A+16>>2],n,r,f)){if(!(0|cn(B,i=0|t[A+28>>2],52964))){u=56;break}if(!(0|cn(o,i=0|t[A+32>>2],52964))){u=56;break}if(!(0|Sf(a,B,o,f))){u=56;break}if(!(n=0|qf())){u=56;break}if(Da(n,a,4),a=0==(0|Hf(0|t[A+24>>2],0|t[A+20>>2],n,f)),Pf(n),a){u=56;break}if(!(n=0|qf())){u=56;break}Da(n,0|t[A+24>>2],4);do{if(0|wB(0,0|t[A+36>>2],n,B,f)){if(!(0|wB(0,0|t[A+40>>2],n,o,f)))break;if(Pf(n),!(n=0|qf())){u=56;break A}if(Da(n,0|t[A+28>>2],4),A=0==(0|Hf(0|t[A+44>>2],0|t[A+32>>2],n,f)),Pf(n),A){u=56;break A}n=1;break A}}while(0);Pf(n),u=56}else u=56;else u=56}else u=56}else u=56}else u=56}while(0);56==(0|u)&&(Xe(4,129,3,0,0),n=0),f?nB(f):f=0}else Xe(4,129,120,0,0),f=0,n=0;return rB(f),0|(u=n)}function rr(A,e,r,i,n,t){return 0|ir(A|=0,e|=0,r|=0,i|=0,n|=0,t|=0,0,0)}function ir(A,e,r,n,t,f,a,B){A|=0,r|=0,n|=0,t|=0,f|=0;var o,u,l=0;if(u=G,G=G+64|0,(a|=0)||(a=52916),l=0==(0|(B|=0))?a:B,((e|=0)+-1-((o=0|pi(a))<<1)|0)<=(0|n))return Xe(4,154,110,0,0),G=u,0|(A=0);if((1|o<<1)>=(0|e))return Xe(4,154,120,0,0),G=u,0|(A=0);i[A>>0]=0;do{if(0!=(0|Ui(t,f,A+o+1|0,0,a,0))&&(Df(A+o+1+o|0,0,e+-1-n-1-(o<<1)|0),i[A+o+1+(e+-1-n-1-o)>>0]=1,If(A+o+1+(e+-1)+(0-n)+(0-o)|0,0|r,0|n),(0|Nt(A+1|0,o))>=1)){if(!(n=0|Ln(a=e+-1-o|0,82796,0))){Xe(4,154,65,0,0),n=0,B=0;break}if((0|nr(n,a,A+1|0,o,l))>=0){if((0|a)>0){B=0;do{i[(e=A+o+1+B|0)>>0]=i[e>>0]^i[n+B>>0],B=B+1|0}while((0|B)!=(0|a))}if((0|nr(u,o,A+o+1|0,a,l))>=0)if((0|o)>0){B=0;do{i[(l=A+1+B|0)>>0]=i[l>>0]^i[u+B>>0],B=B+1|0}while((0|B)!=(0|o));B=1}else B=1;else B=0}else B=0}else n=0,a=0,B=0}while(0);return $r(u,64),pn(n,a,82796,0),G=u,0|(A=B)}function nr(A,e,r,n,t){A|=0,e|=0,r|=0,n|=0,t|=0;var f,a,B=0,o=0,u=0,l=0;a=G,G=G+80|0,f=0|di();A:do{if(0!=(0|f)&&(o=0|pi(t),(0|o)>=0))if((0|e)>0){for(u=0,l=0;;){if(i[a+64>>0]=u>>>24,i[a+64+1>>0]=u>>>16,i[a+64+2>>0]=u>>>8,i[a+64+3>>0]=u,!(0|gi(f,t,0))){B=-1;break A}if(!(0|Di(f,r,n))){B=-1;break A}if(!(0|Di(f,a+64|0,4))){B=-1;break A}if((0|(B=l+o|0))>(0|e))break;if(!(0|Mi(f,A+l|0,0))){B=-1;break A}if(!((0|B)<(0|e))){B=0;break A}u=u+1|0,l=B}0|Mi(f,a,0)?(If(A+l|0,0|a,e-l|0),B=0):B=-1}else B=0;else B=-1}while(0);return $r(a,64),Ii(f),G=a,0|B}function tr(A,e,r,i,n,t,f){return 0|fr(A|=0,e|=0,r|=0,i|=0,n|=0,t|=0,f|=0,0,0)}function fr(A,e,r,n,a,B,o,u,l){A|=0,e|=0,r|=0,n|=0,a|=0,B|=0,o|=0;var c,E,Q=0,w=0,b=0,k=0,s=0,C=0,d=0;if(E=G,G=G+144|0,(u|=0)||(u=52916),k=0==(0|(l|=0))?u:l,c=0|pi(u),(0|e)<1|(0|n)<1)return G=E,0|(a=-1);if((0|a)>=(0|n)&&(2+(c<<1)|0)<=(0|a)){s=0|Ln(C=a-c-1|0,82796,0);do{if(s){if(!(Q=0|Ln(a,82796,0))){Xe(4,153,65,0,0),Q=0,l=0,u=-1;break}if((0|a)>0)for(w=Q+a|0,b=n,l=r+n|0,n=0;l=l+(0-(1&(r=~((b+-1&(-2147483648^b))>>31))))|0,i[(w=w+-1|0)>>0]=(0|f[l>>0])&r,(0|(n=n+1|0))!=(0|a);)b=b-(1&r)|0;else Q=Q+a|0;if(l=(0|f[Q>>0])-1>>31,0|nr(E+64|0,c,(r=Q+1|0)+c|0,C,k))u=-1;else{if((0|c)>0){n=0;do{i[(b=E+64+n|0)>>0]=i[b>>0]^i[r+n>>0],n=n+1|0}while((0|n)!=(0|c))}if(0|nr(s,C,E+64|0,c,k))u=-1;else{if((a-c|0)>1){n=0;do{i[(k=s+n|0)>>0]=i[k>>0]^i[r+c+n>>0],n=n+1|0}while((0|n)<(0|C))}if(0|Ui(B,o,E,0,u,0)){if(u=0|ui(s,E,c),(0|C)>(0|c)){n=0,l&=(u+-1&(-2147483648^u))>>31,r=c,u=0;do{k=(1^(o=0|f[s+r>>0]))-1>>31&~n,t[E+128>>2]=k,B=t[E+128>>2]&r,t[E+128>>2]=~k,u=t[E+128>>2]&u|B,l&=(n|=(1^o)-1>>31)|o+-1>>31,r=r+1|0}while((0|r)!=(0|C))}else n=0,l&=(u+-1&(-2147483648^u))>>31,u=0;if(l=n&l&~(((e-(u=a-c-2-u|0)^u|u^e)^e)>>31),B=C-c-1|0,t[E+128>>2]=((B-e^e|B^e)^B)>>31,k=t[E+128>>2]&B,t[E+128>>2]=~(((B-e^e|B^e)^B)>>31),k=t[E+128>>2]&e|k,(C-c|0)>2){b=1;do{if(r=b&B-u,(c+1|0)<(C-b|0)){n=a+-1-(c+b)|0,w=c+1|0;do{e=s+w|0,o=0|f[s+(w+b)>>0],d=0|f[e>>0],t[E+128>>2]=(r+-1&(-2147483648^r))>>31&255^255,o=t[E+128>>2]&o,t[E+128>>2]=(r+-1&(-2147483648^r))>>31|-256,i[e>>0]=t[E+128>>2]&d|o,w=w+1|0}while((0|w)!=(0|n))}b<<=1}while((0|b)<(0|B))}if((0|k)>0){n=0;do{d=A+n|0,B=255&l&(n-u|u)>>31,e=0|f[s+((n=n+1|0)+c)>>0],o=0|f[d>>0],t[E+128>>2]=B,e=t[E+128>>2]&e,t[E+128>>2]=~B,i[d>>0]=t[E+128>>2]&o|e}while((0|n)!=(0|k))}Xe(4,153,121,0,0),Pe(1&l)}else u=-1}}}else Xe(4,153,65,0,0),Q=0,l=0,u=-1}while(0);return $r(E+64|0,64),pn(s,C,82796,0),pn(Q,a,82796,0),t[E+128>>2]=l,d=t[E+128>>2]&u,t[E+128>>2]=~l,d=t[E+128>>2]|d,G=E,0|d}return Xe(4,153,121,0,0),G=E,0|(d=-1)}function ar(A,e,r,i,n,f){A|=0,e|=0,r|=0,i|=0,n|=0,f|=0;var a,B=0,o=0;if(a=G,G=G+16|0,t[a+4>>2]=0,t[a>>2]=0,0|(B=0|t[44+(0|t[f+8>>2])>>2]))return o=0|UB[7&B](A,e,r,i,n,f),G=a,0|o;do{if(114==(0|A)){if(36==(0|r)){t[a+4>>2]=36,B=36,A=0,o=8;break}return Xe(4,117,131,0,0),G=a,0|(o=0)}A=0==(0|Br(a,a+4|0,A,e,r)),r=0|t[a>>2],B=0|t[a+4>>2],A?e=0:(A=r,e=r,o=8)}while(0);do{if(8==(0|o)){if((0|B)>((0|dr(f))-11|0)){Xe(4,117,112,0,0),r=A,e=0;break}(0|(e=0|gr(B,e,i,f,1)))<1?(r=A,e=0):(t[n>>2]=e,r=A,e=1)}}while(0);return pn(r,B,82796,0),G=a,0|(o=e)}function Br(A,e,r,i,n){A|=0,e|=0,r|=0,i|=0,n|=0;var f;return f=G,G=G+48|0,t[f>>2]=0,t[f+40>>2]=f+32,r=0|Sr(r),t[f+32>>2]=r,r?0|function(A){A=(A|=0)?0|t[A+12>>2]:0;return 0|A}(r)?(t[f+24>>2]=5,t[f+24+4>>2]=0,t[f+32+4>>2]=f+24,t[f+40+4>>2]=f+8,t[f+8+8>>2]=i,t[f+8>>2]=n,(0|(r=0|function(A,e){return 0|TA(A|=0,e|=0,51952)}(f+40|0,f)))<0?(G=f,0|(e=0)):(t[A>>2]=t[f>>2],t[e>>2]=r,G=f,0|(e=1))):(Xe(4,146,116,0,0),G=f,0|(e=0)):(Xe(4,146,117,0,0),G=f,0|(e=0))}function or(A,e,r,n,f,a,B,o){A|=0,e|=0,r|=0,n|=0,f|=0,a|=0,B|=0,o|=0;var u,l;if(l=G,G=G+16|0,t[l+4>>2]=0,t[l>>2]=0,(0|dr(o))!=(0|B))return Xe(4,145,119,0,0),G=l,0|(B=0);u=0|Ln(B,82796,0);A:do{if(u)if((0|(a=0|Mr(B,a,u,o,1)))>=1){if(114==(0|A)){if(36!=(0|a)){Xe(4,145,104,0,0),o=0,r=0;break}if(0|n){r=u,a=(o=n)+36|0;do{i[o>>0]=0|i[r>>0],o=o+1|0,r=r+1|0}while((0|o)<(0|a));t[f>>2]=36,o=0,r=1;break}if(36!=(0|r)){Xe(4,145,131,0,0),o=0,r=0;break}if(!(0|Ht(u,e,36))){o=0,r=1;break}Xe(4,145,104,0,0),o=0,r=0;break}if(95==(0|A)&18==(0|a)&&4==(0|i[u>>0])&&16==(0|i[u+1>>0])){if(0|n){r=u+2|0,a=(o=n)+16|0;do{i[o>>0]=0|i[r>>0],o=o+1|0,r=r+1|0}while((0|o)<(0|a));t[f>>2]=16,o=0,r=1;break}if(16!=(0|r)){Xe(4,145,131,0,0),o=0,r=0;break}if(!(0|Ht(e,u+2|0,16))){o=0,r=1;break}Xe(4,145,104,0,0),o=0,r=0;break}do{if(0|n){if(!(o=0|Xi(0|pr(A)))){Xe(4,145,117,0,0),o=0,r=0;break A}if((o=0|pi(o))>>>0>a>>>0){Xe(4,145,143,0,0),o=0,r=0;break A}e=u+a+(0-o)|0,r=o;break}}while(0);if(A=0==(0|Br(l,l+4|0,A,e,r)),o=0|t[l+4>>2],A)r=0;else{if((0|o)==(0|a)&&0==(0|Ht(0|t[l>>2],u,a))){if(!n){o=a,r=1;break}If(0|n,0|e,0|r),t[f>>2]=r,o=a,r=1;break}Xe(4,145,104,0,0),r=0}}else o=0,r=0;else Xe(4,145,65,0,0),o=0,r=0}while(0);return pn(0|t[l>>2],o,82796,0),pn(u,B,82796,0),G=l,0|(B=r)}function ur(A,e,r,i,n,f){A|=0,e|=0,r|=0,i|=0,n|=0;var a;return(a=0|t[48+(0|t[(f|=0)+8>>2])>>2])?0|(A=0|UB[7&a](A,e,r,i,n,f)):0|(A=0|or(A,e,r,0,0,i,n,f))}function lr(A,e,r,n){A|=0,r|=0;var t=0;if(((e|=0)+-11|0)<(0|(n|=0)))return Xe(4,110,110,0,0),0|(r=0);if(i[A>>0]=0,i[A+1>>0]=2,(0|Nt(A+2|0,e+-11-n|0))<1)return 0|(r=0);A:do{if((e+-11-n|0)>0){t=0,A=A+2|0;e:for(;;){if(!(0|i[A>>0]))do{if((0|Nt(A,1))<1){A=0;break e}}while(!(0|i[A>>0]));if(A=A+1|0,(0|(t=t+1|0))>=(e+-11-n|0))break A}return 0|A}A=A+2|0}while(0);return i[(e=A)>>0]=3,i[e+1>>0]=3,i[e+2>>0]=3,i[e+3>>0]=3,i[e+4>>0]=3,i[e+4+1>>0]=3,i[e+4+2>>0]=3,i[e+4+3>>0]=3,i[A+8>>0]=0,If(A+9|0,0|r,0|n),0|(r=1)}function cr(A,e,r,n,a){A|=0,r|=0;var B,o,u=0,l=0,c=0,E=0,Q=0,w=0,b=0,k=0,s=0;if(o=G,G=G+16|0,(0|(e|=0))<1|(0|(n|=0))<1)return G=o,0|(A=-1);if((0|n)>(0|(a|=0))|(0|a)<11)return Xe(4,114,111,0,0),G=o,0|(A=-1);if(!(B=0|Ln(a,82796,0)))return Xe(4,114,65,0,0),G=o,0|(A=-1);for(l=B+a|0,c=n,n=r+n|0,r=0;n=n+(0-(1&(u=~((c+-1&(-2147483648^c))>>31))))|0,i[(l=l+-1|0)>>0]=(0|f[n>>0])&u,(0|(r=r+1|0))!=(0|a);)c=c-(1&u)|0;if(E=((255&(2^i[B+1>>0]))-1&(0|f[B>>0])-1)>>31,t[o>>2]=E,t[o>>2]=~E,c=107&t[o>>2],(0|a)>2){u=0,l=2,n=0,r=0;do{k=(w=(0|f[(b=B+l|0)>>0])-1>>31)&~u,t[o>>2]=k,Q=t[o>>2]&l,t[o>>2]=~k,r=t[o>>2]&r|Q,u|=w,n=((255&(3^i[b>>0]))-1>>31|u)&(1&u^1)+n,l=l+1|0}while((0|l)!=(0|a))}else n=0,r=0;if(k=(r+-10&(-2147483648^r))>>31,t[o>>2]=~k|~E,Q=c&t[o>>2],t[o>>2]=k&E,Q=113&t[o>>2]|Q,b=E&~k&~(w=(n+-8&(-2147483648^n))>>31),t[o>>2]=~(E&~k)|~w,Q=t[o>>2]&Q,t[o>>2]=E&~k&w,Q=115&t[o>>2]|Q,E=~(((e-(w=a+-1-r|0)^w|w^e)^e)>>31),t[o>>2]=E|~b,Q=t[o>>2]&Q,t[o>>2]=((e-w^w|w^e)^e)>>31&b,Q=109&t[o>>2]|Q,k=((a+-11-e^e|a+-11^e)^a+-11)>>31,t[o>>2]=k,c=t[o>>2]&a+-11,t[o>>2]=~k,c=t[o>>2]&e|c,(0|a)>12){u=1;do{if(n=u&a+-11-w,(0|(l=a-u|0))>11){r=11;do{k=B+r|0,e=0|f[B+(r+u)>>0],s=0|f[k>>0],t[o>>2]=(n+-1&(-2147483648^n))>>31&255^255,e=t[o>>2]&e,t[o>>2]=(n+-1&(-2147483648^n))>>31|-256,i[k>>0]=t[o>>2]&s|e,r=r+1|0}while((0|r)!=(0|l))}u<<=1}while((0|u)<(a+-11|0))}if((0|c)>0){n=0;do{s=A+n|0,l=b&E&255&(n-w|w)>>31,k=0|f[B+(n+11)>>0],e=0|f[s>>0],t[o>>2]=l,k=t[o>>2]&k,t[o>>2]=~l,i[s>>0]=t[o>>2]&e|k,n=n+1|0}while((0|n)!=(0|c))}return pn(B,a,82796,0),Xe(4,114,Q,0,0),Pe(b&E&1),t[o>>2]=b&E,s=t[o>>2]&w,t[o>>2]=~(b&E),s=t[o>>2]|s,G=o,0|s}function Er(A){z(A|=0,52600)}function Qr(A,e,r){return 0|hA(A|=0,e|=0,r|=0,52496)}function wr(A,e){return 0|TA(A|=0,e|=0,52496)}function br(A,e,r){return 0|hA(A|=0,e|=0,r|=0,52548)}function kr(){return 0|function(A){A|=0;var e=0;if(!(e=0|Sn(88,82796,0)))return Xe(4,106,65,0,0),0|(e=0);if(t[e+52>>2]=1,A=0|W(),t[e+84>>2]=A,!A)return Xe(4,106,65,0,0),Xn(e,82796,0),0|(e=0);if(A=52440,t[e+8>>2]=A,t[e+56>>2]=-1025&t[A+36>>2],!(0|function(A,e,r){e|=0,r|=0;var i,n=0,f=0,a=0,B=0;if(i=G,G=G+48|0,(A=A|0)>>>0>13)return Xe(15,113,7,0,0),G=i,0|(e=0);if(!((B=0!=(0|m(82276,14)))&0!=(0|t[20570])))return Xe(15,113,65,0,0),G=i,0|(e=0);if(!(n=0|t[20568]))return G=i,0|(e=0);R(n),t[r>>2]=0,B=0|Hn(0|t[81968+(A<<2)>>2]);do{if((0|B)>0){if((0|B)>=10){if(!(n=0|Ln(B<<2,82796,0))){n=0;break}}else n=i;f=0;do{t[n+(f<<2)>>2]=0|vn(0|t[81968+(A<<2)>>2],f),f=f+1|0}while((0|f)!=(0|B))}else n=0}while(0);if(y(0|t[20568]),(0|B)>0&0==(0|n))return Xe(15,112,65,0,0),G=i,0|(e=0);if((0|B)>0){a=0;do{0|(f=0|t[(A=n+(a<<2)|0)>>2])&&0|t[f+8>>2]&&(f=0!=(0|(f=0|t[r>>2]))&&(0|Hn(f))>(0|a)?0|vn(0|t[r>>2],a):0,A=0|t[A>>2],YB[0&t[A+8>>2]](e,f,r,a,0|t[A>>2],0|t[A+4>>2])),a=a+1|0}while((0|a)!=(0|B))}if((0|n)==(0|i))return G=i,0|(e=1);return Xn(n,82796,0),G=i,0|(e=1)}(9,e,e+48|0)))return sr(e),0|(e=0);return(A=0|t[28+(0|t[e+8>>2])>>2])?0|IB[31&A](e)?0|e:(Xe(4,106,70,0,0),sr(e),0|(e=0)):0|e}(0)}function sr(A){var e,r,i=0;r=G,G=G+16|0,(A|=0)?(T(A+52|0,-1,r,0|t[A+84>>2]),(0|t[r>>2])>0||(0|(e=0|t[A+8>>2])&&0|(i=0|t[e+32>>2])&&IB[31&i](A),bt(9,A,A+48|0),V(0|t[A+84>>2]),xf(0|t[A+16>>2]),xf(0|t[A+20>>2]),xf(0|t[A+24>>2]),xf(0|t[A+28>>2]),xf(0|t[A+32>>2]),xf(0|t[A+36>>2]),xf(0|t[A+40>>2]),xf(0|t[A+44>>2]),wn(0|t[A+76>>2]),wn(0|t[A+80>>2]),Xn(0|t[A+72>>2],82796,0),Xn(A,82796,0)),G=r):G=r}function Cr(A,e,r,i){A|=0,e|=0,r|=0,i|=0;do{if((0|i)<=(0|e)){if((0|i)<(0|e)){Xe(4,107,122,0,0),i=0;break}If(0|A,0|r,0|i),i=1;break}Xe(4,107,110,0,0),i=0}while(0);return 0|i}function dr(A){return 0|(7+(0|Kf(0|t[(A|=0)+16>>2]))|0)/8}function Ir(A,e,r,i,n){return A|=0,e|=0,r|=0,n|=0,0|hB[15&t[4+(0|t[(i|=0)+8>>2])>>2]](A,e,r,i,n)}function gr(A,e,r,i,n){return A|=0,e|=0,r|=0,n|=0,0|hB[15&t[12+(0|t[(i|=0)+8>>2])>>2]](A,e,r,i,n)}function Dr(A,e,r,i,n){return A|=0,e|=0,r|=0,n|=0,0|hB[15&t[16+(0|t[(i|=0)+8>>2])>>2]](A,e,r,i,n)}function Mr(A,e,r,i,n){return A|=0,e|=0,r|=0,n|=0,0|hB[15&t[8+(0|t[(i|=0)+8>>2])>>2]](A,e,r,i,n)}function hr(A,e){A|=0;var r=0,i=0,n=0,f=0,a=0,B=0,o=0;if(e|=0)r=e;else if(!(r=0|eB()))return 0|(A=0);iB(r);A:do{if(0|tB(r)){i=0|t[A+20>>2];e:do{if(!i){i=0|t[A+24>>2],n=0|t[A+28>>2],f=0|t[A+32>>2];do{if(!(0==(0|i)|0==(0|n)|0==(0|f))){if(iB(r),a=0|tB(r),B=0|tB(r),0|(o=0|tB(r))&&0|cn(B,n,52964)&&0|cn(o,f,52964)&&0|Sf(a,B,o,r)){if(i=0|Hf(0,i,a,r),nB(r),i)break e;break}nB(r)}}while(0);Xe(4,136,140,0,0),i=0,n=0;break A}}while(0);if(!(f=0|qf())){Xe(4,136,65,0,0),n=0;break}if(Da(f,0|t[A+16>>2],4),n=0|bn(0,i,f,r,0|t[24+(0|t[A+8>>2])>>2],0|t[A+60>>2]),Pf(f),n){Qn(n);break}Xe(4,136,3,0,0),n=0;break}Xe(4,136,65,0,0),i=0,n=0}while(0);return nB(r),(0|r)!=(0|e)&&rB(r),(0|i)==(0|t[A+20>>2])||Pf(i),0|(A=n)}function Ur(A,e,r,n,a,B){A|=0,e|=0,r|=0,n|=0,a|=0,B|=0;var o,u,l=0,c=0,E=0,Q=0,w=0;u=G,G=G+64|0,o=0|di();A:do{if(0!=(0|o)&&(E=0==(0|n)?r:n,w=0|pi(r),(0|w)>=0)){switch(0|B){case-1:B=w;break;case-2:B=-2;break;default:if((0|B)<-2){Xe(4,126,136,0,0),n=0,B=0;break A}}if(l=7+(0|Kf(0|t[A+16>>2]))&7,n=0|dr(A),255<<l&f[a>>0]|0){Xe(4,126,133,0,0),n=0,B=0;break}if(c=0==(0|l)?a+1|0:a,(n+((0==(0|l))<<31>>31)|0)<(w+2|0)){Xe(4,126,109,0,0),n=0,B=0;break}if((0|B)>((Q=n+((0==(0|l))<<31>>31)-w|0)+-2|0)){Xe(4,126,109,0,0),n=0,B=0;break}if(-68!=(0|i[c+(n+((0==(0|l))<<31>>31)+-1)>>0])){Xe(4,126,134,0,0),n=0,B=0;break}if(!(n=0|Ln(Q+-1|0,82796,0))){Xe(4,126,65,0,0),n=0,B=0;break}if((0|nr(n,Q+-1|0,c+(Q+-1)|0,w,E))>=0){if((0|Q)>1){A=0;do{i[(E=n+A|0)>>0]=i[E>>0]^i[c+A>>0],A=A+1|0}while((0|A)<(Q+-1|0))}0|l&&(i[n>>0]=255>>>(8-l|0)&f[n>>0]),a=0;do{A=0|i[n+a>>0],E=a,a=a+1|0}while((0|E)<(Q+-2|0)&A<<24>>24==0);if(A<<24>>24!=1){Xe(4,126,135,0,0),B=0;break}if(!((0|B)<0|(0|(A=Q+-1-a|0))==(0|B))){Xe(4,126,136,0,0),B=0;break}if(0!=(0|gi(o,r,0))&&0!=(0|Di(o,82788,8))&&0!=(0|Di(o,e,w))){if(0|A&&0==(0|Di(o,n+a|0,A))){B=0;break}0|Mi(o,u,0)?0|Ht(u,c+(Q+-1)|0,w)?(Xe(4,126,104,0,0),B=0):B=1:B=0}else B=0}else B=0}else n=0,B=0}while(0);return Xn(n,82796,0),Ii(o),G=u,0|B}function Gr(A,e,r,n,a,B){A|=0,e|=0,r|=0,n|=0,B|=0;var o,u,l=0,c=0,E=0,Q=0,w=0;u=0==(0|(a|=0))?n:a,o=0|pi(n);A:do{if((0|o)>=0){switch(0|B){case-1:B=o;break;case-2:B=-2;break;default:if((0|B)<-2){Xe(4,152,136,0,0),A=0,e=0,a=0;break A}}if(w=7+(0|Kf(0|t[A+16>>2]))&7,a=0|dr(A),w?(Q=e,E=a):(i[e>>0]=0,Q=e+1|0,E=a+-1|0),(0|E)<(o+2|0)){Xe(4,152,110,0,0),A=0,e=0,a=0;break}if(l=E-o|0,-2!=(0|B)){if((0|B)>(l+-2|0)){Xe(4,152,110,0,0),A=0,e=0,a=0;break}}else B=l+-2|0;e=(0|B)>0;do{if(e){if(a=0|Ln(B,82796,0)){if((0|Nt(a,B))<1){A=0,e=0;break A}break}Xe(4,152,65,0,0),A=0,e=0,a=0;break A}a=0}while(0);if(A=Q+(l+-1)|0,c=0|di())if(0!=(0|gi(c,n,0))&&0!=(0|Di(c,82788,8))&&0!=(0|Di(c,r,o))){if(0|B&&0==(0|Di(c,a,B))){A=c,e=0;break}if(0!=(0|Mi(c,A,0))&&0==(0|nr(Q,l+-1|0,A,o,u))){if(i[(A=Q+(-2-o+E-B)|0)>>0]=1^i[A>>0],e){e=0;do{i[(A=A+1|0)>>0]=i[A>>0]^i[a+e>>0],e=e+1|0}while((0|e)!=(0|B))}0|w&&(i[Q>>0]=255>>>(8-w|0)&(0|f[Q>>0])),i[Q+(E+-1)>>0]=-68,A=c,e=1}else A=c,e=0}else A=c,e=0;else A=0,e=0}else A=0,e=0,a=0}while(0);return Ii(A),pn(a,B,82796,0),0|e}function Zr(A,e,r,n){return A|=0,r|=0,((e|=0)+-11|0)<(0|(n|=0))?(Xe(4,108,110,0,0),0|(A=0)):(i[A>>0]=0,i[A+1>>0]=1,Df(A+2|0,-1,e+-3-n|0),i[(A=A+2+(e+-3-n)|0)>>0]=0,If(A+1|0,0|r,0|n),0|(A=1))}function Nr(A,e,r,n,t){A|=0,e|=0,r|=0,n|=0;var f=0,a=0,B=0,o=0;if((0|(t|=0))<11)return 0|(A=-1);do{if((0|t)==(0|n)){if(!(0|i[r>>0])){n=t+-1|0,r=r+1|0;break}return Xe(4,112,138,0,0),0|(A=-1)}}while(0);if((n+1|0)==(0|t)&&1==(0|i[r>>0])){a=n+-1|0,f=r+1|0;A:do{if((0|n)>1){t=0;e:for(;;){switch(0|i[f>>0]){case 0:B=12;break e;case-1:break;default:break e}if(r=f+1|0,!((0|(t=t+1|0))<(0|a)))break A;o=f,f=r,r=o}if(12==(0|B)){r=r+2|0;break}return Xe(4,112,102,0,0),0|(o=-1)}t=0,r=f}while(0);return(0|t)==(0|a)?(Xe(4,112,113,0,0),0|(o=-1)):t>>>0<8?(Xe(4,112,103,0,0),0|(o=-1)):(0|(n=n+-2-t|0))>(0|e)?(Xe(4,112,109,0,0),0|(o=-1)):(If(0|A,0|r,0|n),0|(o=n))}return Xe(4,112,106,0,0),0|(o=-1)}function Fr(A,e,r,n){A|=0,r|=0;var t=0;if(((e|=0)+-11|0)<(0|(n|=0)))return Xe(4,109,110,0,0),0|(r=0);if(i[A>>0]=0,i[A+1>>0]=2,(0|Nt(A+2|0,e+-3-n|0))<1)return 0|(r=0);A:do{if((e+-3-n|0)>0){t=0,A=A+2|0;e:for(;;){if(!(0|i[A>>0]))do{if((0|Nt(A,1))<1){A=0;break e}}while(!(0|i[A>>0]));if(A=A+1|0,(0|(t=t+1|0))>=(e+-3-n|0))break A}return 0|A}A=A+2|0}while(0);return i[A>>0]=0,If(A+1|0,0|r,0|n),0|(r=1)}function Wr(A,e,r,n,a){A|=0,r|=0;var B,o,u=0,l=0,c=0,E=0,Q=0,w=0,b=0;if(o=G,G=G+16|0,((n|=0)|(e|=0)|0)<0)return G=o,0|(A=-1);if((0|n)>(0|(a|=0))|(0|a)<11)return Xe(4,113,159,0,0),G=o,0|(A=-1);if(!(B=0|Ln(a,82796,0)))return Xe(4,113,65,0,0),G=o,0|(A=-1);for(l=B+a|0,c=n,n=r+n|0,r=0;n=n+(0-(1&(u=~((c+-1&(-2147483648^c))>>31))))|0,i[(l=l+-1|0)>>0]=(0|f[n>>0])&u,(0|(r=r+1|0))!=(0|a);)c=c-(1&u)|0;if(c=((255&(2^i[B+1>>0]))-1&(0|f[B>>0])-1)>>31,(0|a)>2)for(r=0,u=2,n=0;E=(l=(0|f[B+u>>0])-1>>31)&~r,t[o>>2]=E,Q=t[o>>2]&u,t[o>>2]=~E,n=t[o>>2]&n|Q,(0|(u=u+1|0))!=(0|a);)r|=l;else n=0;if(E=c&~((n+-10&(-2147483648^n))>>31)&~(((e-(Q=a+-1-n|0)^Q|Q^e)^e)>>31),l=((a+-11-e^e|a+-11^e)^a+-11)>>31,t[o>>2]=l,c=t[o>>2]&a+-11,t[o>>2]=~l,c=t[o>>2]&e|c,(0|a)>12){u=1;do{if(n=u&a+-11-Q,(0|(l=a-u|0))>11){r=11;do{e=B+r|0,w=0|f[B+(r+u)>>0],b=0|f[e>>0],t[o>>2]=(n+-1&(-2147483648^n))>>31&255^255,w=t[o>>2]&w,t[o>>2]=(n+-1&(-2147483648^n))>>31|-256,i[e>>0]=t[o>>2]&b|w,r=r+1|0}while((0|r)!=(0|l))}u<<=1}while((0|u)<(a+-11|0))}if((0|c)>0){n=0;do{b=A+n|0,l=255&E&(n-Q|Q)>>31,w=0|f[B+(n+11)>>0],e=0|f[b>>0],t[o>>2]=l,w=t[o>>2]&w,t[o>>2]=~l,i[b>>0]=t[o>>2]&e|w,n=n+1|0}while((0|n)!=(0|c))}return pn(B,a,82796,0),Xe(4,113,159,0,0),Pe(1&E),t[o>>2]=E,b=t[o>>2]&Q,t[o>>2]=~E,b=t[o>>2]|b,G=o,0|b}function Yr(A,e,r,i,n){e|=0,r|=0,i|=0,n|=0;var a,B,o=0,u=0;B=G,G=G+16|0,a=0|t[(A|=0)+20>>2],o=0|t[a+20>>2];A:do{if(o)switch(0|t[a+16>>2]){case 5:if(!(o=0|t[a+32>>2])&&(o=0|Ln(0|Zi(0|t[A+8>>2]),82796,0),t[a+32>>2]=o,!o))return G=B,0|(u=-1);if((0|(o=0|Mr(n,i,o,0|t[24+(0|t[A+8>>2])>>2],5)))<1)return G=B,0|(u=0);if(n=0|f[(0|t[a+32>>2])+(o+-1)>>0],(0|$e(0|Si(0|t[a+20>>2])))!=(0|n))return Xe(4,141,100,0,0),G=B,0|(u=0);if((o+-1|0)==(0|pi(0|t[a+20>>2]))){if(!e){o=o+-1|0;break A}If(0|e,0|t[a+32>>2],o+-1|0),o=o+-1|0;break A}return Xe(4,141,143,0,0),G=B,0|(u=0);case 1:if(!((0|or(0|Si(o),0,0,e,B,i,n,0|t[24+(0|t[A+8>>2])>>2]))<1)){o=0|t[B>>2],u=14;break A}return G=B,0|(o=0);default:return G=B,0|(u=-1)}else o=0|Mr(n,i,e,0|t[24+(0|t[A+8>>2])>>2],0|t[a+16>>2]),u=14}while(0);return 14==(0|u)&&(0|o)<0?(G=B,0|(u=o)):(t[r>>2]=o,G=B,0|(u=1))}function Rr(A,e){A|=0,e|=0;A:do{if(A)switch(A=0|Si(A),0|e){case 3:Xe(4,140,141,0,0),A=0;break A;case 5:if(-1!=(0|$e(A))){A=1;break A}Xe(4,140,142,0,0),A=0;break A;default:switch(0|A){case 117:case 95:case 257:case 3:case 114:case 4:case 674:case 673:case 672:case 675:case 64:A=1;break A}Xe(4,140,157,0,0),A=0;break A}else A=1}while(0);return 0|A}function yr(A,e,r,i,n,f){A|=0,e|=0,r|=0,i|=0,n|=0,f|=0;var a;return a=G,G=G+32|0,t[a+8+4>>2]=4,t[a+8>>2]=r,t[a+8+8>>2]=e,(0|(A=0|AA(a+8|0,0)))>((e=0|dr(f))+-11|0)?(Xe(4,118,112,0,0),G=a,0|(n=0)):(r=0|Ln(e+1|0,82796,0))?(t[a>>2]=r,AA(a+8|0,a),(0|(A=0|gr(A,r,i,f,1)))<1?A=0:(t[n>>2]=A,A=1),pn(r,e+1|0,82796,0),G=a,0|(n=A)):(Xe(4,118,65,0,0),G=a,0|(n=0))}function Vr(){var A,e=0,r=0;if(0|(A=0|v(82068))){if(0!=(0|(r=0|t[A>>2]))&&0!=(0|(e=0|Vn(r)))){do{Xn(0|t[e+8>>2],82796,0),Xn(e,82796,0),e=0|Vn(0|t[A>>2])}while(0!=(0|e));e=A}else e=A;Fn(0|t[e>>2]),Xn(A,82796,0),J(82068,0)}e=0|v(82064),0|J(82064,0)&&Xn(e,82796,0)}function mr(){var A=0,e=0;return 0|t[20518]||(t[20518]=52736),(e=0|Sn(40,82796,0))?(A=0|Wn(),t[e+4>>2]=A,A?(t[e>>2]=t[20518],t[e+8>>2]=1,A=0|W(),t[e+36>>2]=A,A?(A=0|t[28+(0|t[e>>2])>>2])?0|IB[31&A](e)?0|e:(Hr(e),0|(e=0)):0|e:(Xe(37,113,65,0,0),Fn(0|t[e+4>>2]),Xn(e,82796,0),0|(e=0))):(Xe(37,113,65,0,0),Xn(e,82796,0),0|(e=0))):(Xe(37,113,65,0,0),0|(e=0))}function Hr(A){var e,r=0;return e=G,G=G+16|0,(A|=0)?(0|T(A+8|0,-1,e,0|t[A+36>>2]))<1?(G=e,0|(A=0)):(0|t[e>>2])>0?(G=e,0|(A=1)):0==(4&t[A+12>>2]|0)&&0|(r=0|t[8+(0|t[A>>2])>>2])&&0==(0|IB[31&r](A))?(Xe(37,111,107,0,0),G=e,0|(A=0)):0|(r=0|t[32+(0|t[A>>2])>>2])&&0==(0|IB[31&r](A))?(Xe(37,111,102,0,0),G=e,0|(A=0)):(Fn(0|t[A+4>>2]),Xn(0|t[A+28>>2],82796,0),Xn(0|t[A+32>>2],82796,0),V(0|t[A+36>>2]),Xn(A,82796,0),G=e,0|(A=1)):(G=e,0|(A=1))}function vr(A,e,r,i){A|=0,e|=0,r|=0,i|=0;do{if(!A){if(0|(r=0|mr())){t[r+12>>2]=i,i=1,A=r;break}return Xe(37,112,65,0,0),0|(e=0)}i=0}while(0);r=A+28|0;do{if(0|t[r>>2])Xe(37,112,110,0,0);else{if(0|e){if(!(0|Jr(A,e))){Xe(37,112,112,0,0);break}if(0|t[r>>2]){if(!(r=0|t[4+(0|t[A>>2])>>2])){Xe(37,112,108,0,0);break}if(0|IB[31&r](A))return 0|(e=A);Xe(37,112,103,0,0);break}}Xe(37,112,111,0,0)}}while(0);return i?(Hr(A),0|(e=0)):0|(e=0)}function Jr(A,e){return 0==(0|(A|=0))|0==(0|(e|=0))?(Xe(37,129,67,0,0),0|(A=0)):0|t[A+32>>2]?(Xe(37,129,110,0,0),0|(A=0)):(e=0|kt(e,82796,0))?(Xn(0|t[A+28>>2],82796,0),t[A+28>>2]=e,0|(A=1)):(Xe(37,129,65,0,0),0|(A=0))}function Xr(A,e){var r=0;return 0==(0|(A|=0))|0==(0|(e|=0))?(Xe(37,108,67,0,0),0|(e=0)):(r=0|t[12+(0|t[A>>2])>>2])?0|(r=0|gB[63&r](A,e))?0|(e=r):(Xe(37,108,106,0,0),0|(e=0)):(Xe(37,108,108,0,0),0|(e=0))}function Tr(A){A|=0;var e,r=0;if(r=0|t[20522])return t[20525]=A,e=0|function(A){return 0|t[(A|=0)+36>>2]}(r),ve(0|t[20522],0),Ve(0|t[20522],12),r=0|t[20522],(0|A)<0?(Fe(r),mn(0|t[20524],13),V(0|t[20523]),t[20522]=0,void(t[20523]=0)):void ve(r,e)}function Lr(A){A|=0;var e=0,r=0,i=0,n=0,f=0,a=0,B=0;if(0==(0|t[20526])&&(B=0|Ze(19,30),t[20526]=B,0==(0|B)))return 0|(B=0);if(B=0|function(A){var e=0,r=0,i=0;if(!(A|=0))return 0|(i=0);if(!(1&t[A+20>>2]))return 0|(i=A);if(!(i=0|function(){var A=0;return(A=0|Sn(24,82796,0))?(t[A+20>>2]=1,0|A):(Xe(13,123,65,0,0),0|(A=0))}()))return Xe(8,101,13,0,0),0|(i=0);t[i+20>>2]=13|t[A+20>>2],(0|(e=0|t[A+12>>2]))>0?(e=0|Ct(0|t[A+16>>2],e,82796,0),t[i+16>>2]=e,0|e&&(e=0|t[A+12>>2],r=8)):r=8;do{if(8==(0|r)){if(t[i+12>>2]=e,t[i+8>>2]=t[A+8>>2],0|(e=0|t[A+4>>2])&&(r=0|kt(e,82796,0),t[i+4>>2]=r,0==(0|r)))break;if(!(e=0|t[A>>2]))return 0|i;if(A=0|kt(e,82796,0),t[i>>2]=A,0|A)return 0|i}}while(0);return $(i),Xe(8,101,65,0,0),0|(i=0)}(A)){e=0|Ln(8,82796,0);do{if(e){if(0!=(0|t[B+12>>2])&&0!=(0|t[A+16>>2])){if(!(A=0|Ln(8,82796,0))){r=0,A=0;break}a=A}else a=0,A=0;if(0|t[B>>2]){if(!(i=0|Ln(8,82796,0))){r=0;break}r=i,f=i}else r=0,f=0;if(0|t[B+4>>2]){if(!(i=0|Ln(8,82796,0)))break;n=i}else n=0,i=0;return 0|a&&(t[a>>2]=0,t[a+4>>2]=B,Xn(0|We(0|t[20526],A),82796,0)),0|f&&(t[f>>2]=1,t[f+4>>2]=B,Xn(0|We(0|t[20526],r),82796,0)),0|n&&(t[n>>2]=2,t[n+4>>2]=B,Xn(0|We(0|t[20526],i),82796,0)),t[e>>2]=3,t[e+4>>2]=B,Xn(0|We(0|t[20526],e),82796,0),t[B+20>>2]=-14&t[B+20>>2],0|(B=0|t[B+8>>2])}r=0,A=0}while(0);Xe(8,105,65,0,0)}else r=0,e=0,A=0;return Xn(A,82796,0),Xn(r,82796,0),Xn(0,82796,0),Xn(e,82796,0),$(B),0|(B=0)}function Sr(A){A|=0;var e,r=0;e=G,G=G+32|0;do{if(A>>>0>=1061){if(r=0|t[20526]){if(t[e+24>>2]=3,t[e+24+4>>2]=e,t[e+8>>2]=A,r=0|ye(r,e+24|0)){r=0|t[r+4>>2];break}Xe(8,103,101,0,0),r=0;break}r=0}else{if(0|A&&0==(0|t[1888+(24*A|0)+8>>2])){Xe(8,103,101,0,0),r=0;break}r=1888+(24*A|0)|0}}while(0);return G=e,0|r}function pr(A){A|=0;var e,r=0;e=G,G=G+32|0;do{if(A>>>0>=1061){if(r=0|t[20526]){if(t[e+24>>2]=3,t[e+24+4>>2]=e,t[e+8>>2]=A,r=0|ye(r,e+24|0)){r=0|t[t[r+4>>2]>>2];break}Xe(8,104,101,0,0),r=0;break}r=0}else{if(0|A&&0==(0|t[1888+(24*A|0)+8>>2])){Xe(8,104,101,0,0),r=0;break}r=0|t[1888+(24*A|0)>>2]}}while(0);return G=e,0|r}function zr(A){var e,r=0,i=0,n=0,f=0,a=0,B=0,o=0;if(e=G,G=G+16|0,!(A|=0))return G=e,0|(A=0);if(0|(r=0|t[A+8>>2]))return G=e,0|(A=r);if(!(r=0|t[A+12>>2]))return G=e,0|(A=0);i=0|t[20526];A:do{if(i){if(t[e>>2]=0,t[e+4>>2]=A,0|(r=0|ye(i,e)))return A=0|t[8+(0|t[r+4>>2])>>2],G=e,0|A;if(!(r=0|t[A+12>>2])){for(f=956,a=0;;){if(r=0|t[34128+((n=(a+f|0)/2|0)<<2)>>2],!(i=0|t[1888+(24*r|0)+12>>2]))break A;if((0|(a=(0|i)>0?a:n+1|0))>=(0|(f=(0|i)>0?n:f))){r=0;break}}return G=e,0|r}B=r,o=11}else B=r,o=11}while(0);A:do{if(11==(0|o)){for(i=956,a=0;;){if(r=0|t[34128+((f=(a+i|0)/2|0)<<2)>>2],(n=B-(0|t[1888+(24*r|0)+12>>2])|0)||(n=0|Ht(0|t[A+16>>2],0|t[1888+(24*r|0)+16>>2],B)),(0|n)<0)i=f,r=a;else{if(!n)break A;r=f+1|0}if(!((0|r)<(0|i))){r=0;break}a=r}return G=e,0|r}}while(0);return A=0|t[1888+(24*r|0)+8>>2],G=e,0|A}function Kr(A,e){A|=0,e|=0;var r,i=0,n=0,f=0,a=0,B=0;r=G,G=G+64|0;A:do{if(!e){t[r+32>>2]=A,e=0|t[20526];do{if(e){if(t[r+8>>2]=1,t[r+8+4>>2]=r+32,e=0|ye(e,r+8|0)){e=8+(0|t[e+4>>2])|0,n=13;break}B=0|t[r+32>>2],n=6;break}B=A,n=6}while(0);do{if(6==(0|n)){for(i=1052,e=0;;){if(f=0|t[37952+((a=(e+i|0)/2|0)<<2)>>2],(0|(n=0|Wt(B,0|t[1888+(24*f|0)>>2])))<0)i=a;else{if(!n){n=12;break}e=a+1|0}if((0|e)>=(0|i)){n=11;break}}if(11==(0|n)){n=14;break}if(12==(0|n)){e=1888+(24*f|0)+8|0,n=13;break}}}while(0);if(13==(0|n)&&((i=0|t[e>>2])||(n=14)),14==(0|n)){t[r+32+4>>2]=A,e=0|t[20526];do{if(e){if(t[r+8>>2]=2,t[r+8+4>>2]=r+32,e=0|ye(e,r+8|0)){e=8+(0|t[e+4>>2])|0;break}B=0|t[r+32+4>>2],n=18;break}B=A,n=18}while(0);do{if(18==(0|n)){for(i=1052,e=0;;){if(f=0|t[42160+((a=(e+i|0)/2|0)<<2)>>2],(0|(n=0|Wt(B,0|t[1888+(24*f|0)+4>>2])))<0)i=a;else{if(!n){n=24;break}e=a+1|0}if((0|e)>=(0|i)){n=23;break}}if(23==(0|n))break A;if(24==(0|n)){e=1888+(24*f|0)+8|0;break}}}while(0);if(!(e=0|t[e>>2]))break;i=e}do{if(i>>>0<1061){if(0|t[1888+(24*i|0)+8>>2]){e=1888+(24*i|0)|0;break}Xe(8,103,101,0,0),e=0;break}if(e=0|t[20526]){if(t[r+32>>2]=3,t[r+32+4>>2]=r+8,t[r+8+8>>2]=i,e=0|ye(e,r+32|0)){e=0|t[e+4>>2];break}Xe(8,103,101,0,0),e=0;break}e=0}while(0);return G=r,0|(A=e)}}while(0);return(0|(n=0|P(0,0,A,-1)))<1||(0|(e=0|BA(0,n,6)))<0?(G=r,0|(A=0)):(i=0|Ln(e,82796,0))?(t[r+4>>2]=i,fA(r+4|0,0,n,6,0),P(0|t[r+4>>2],n,A,-1),t[r>>2]=i,A=0|function(A,e,r){var i;if(A|=0,e|=0,r|=0,i=G,G=G+16|0,t[i+12>>2]=t[e>>2],128&(0|tA(i+12|0,i+8|0,i+4|0,i,r)))A=102;else{if(6==(0|t[i+4>>2]))return(A=0|q(A,i+12|0,0|t[i+8>>2]))?(t[e>>2]=t[i+12>>2],G=i,0|(e=A)):(G=i,0|(e=0));A=116}return Xe(13,147,A,0,0),G=i,0|(e=0)}(0,r,e),Xn(i,82796,0),G=r,0|A):(G=r,0|(A=0))}function jr(A){A|=0;var e,r=0,i=0,n=0,f=0,a=0,B=0;e=G,G=G+32|0,t[e+8>>2]=A,r=0|t[20526];do{if(r){if(t[e>>2]=1,t[e+4>>2]=e+8,!(r=0|ye(r,e))){A=0|t[e+8>>2];break}return B=0|t[8+(0|t[r+4>>2])>>2],G=e,0|B}}while(0);for(i=1052,r=0;;){if(n=0|t[37952+((a=(r+i|0)/2|0)<<2)>>2],(0|(f=0|Wt(A,0|t[1888+(24*n|0)>>2])))<0)i=a;else{if(!f)break;r=a+1|0}if((0|r)>=(0|i)){r=0,B=11;break}}return 11==(0|B)?(G=e,0|r):(B=0|t[1888+(24*n|0)+8>>2],G=e,0|B)}function xr(A){A|=0;var e,r=0,i=0,n=0,f=0,a=0,B=0;e=G,G=G+32|0,t[e+8+4>>2]=A,r=0|t[20526];do{if(r){if(t[e>>2]=2,t[e+4>>2]=e+8,!(r=0|ye(r,e))){A=0|t[e+8+4>>2];break}return B=0|t[8+(0|t[r+4>>2])>>2],G=e,0|B}}while(0);for(i=1052,r=0;;){if(n=0|t[42160+((a=(r+i|0)/2|0)<<2)>>2],(0|(f=0|Wt(A,0|t[1888+(24*n|0)+4>>2])))<0)i=a;else{if(!f)break;r=a+1|0}if((0|r)>=(0|i)){r=0,B=11;break}}return 11==(0|B)?(G=e,0|r):(B=0|t[1888+(24*n|0)+8>>2],G=e,0|B)}function Or(A,e,r,n){r|=0,n|=0;var f,a=0,B=0,u=0,l=0,c=0,E=0,Q=0,w=0,b=0,k=0;if(f=G,G=G+80|0,0!=(0|(A|=0))&(0|(e|=0))>0&&(i[A>>0]=0),!r)return G=f,0|(k=0);if(!(0|t[r+16>>2]))return G=f,0|(k=0);A:do{if(0==(0|n)&&(a=0|zr(r),0|a)){do{if(a>>>0<1061){if(0|t[1888+(24*a|0)+8>>2]){n=1888+(24*a|0)+4|0,k=16;break}Xe(8,102,101,0,0),k=15;break}if(n=0|t[20526]){if(t[f+64>>2]=3,t[f+64+4>>2]=f+40,t[f+40+8>>2]=a,n=0|ye(n,f+64|0)){n=4+(0|t[n+4>>2])|0,k=16;break}Xe(8,102,101,0,0),k=15;break}k=15}while(0);15==(0|k)?k=17:16==(0|k)&&((n=0|t[n>>2])||(k=17));do{if(17==(0|k)){do{if(a>>>0<1061){if(0|t[1888+(24*a|0)+8>>2]){n=1888+(24*a|0)|0,k=26;break}Xe(8,104,101,0,0),k=25;break}if(n=0|t[20526]){if(t[f+64>>2]=3,t[f+64+4>>2]=f+40,t[f+40+8>>2]=a,n=0|ye(n,f+64|0)){n=0|t[n+4>>2],k=26;break}Xe(8,104,101,0,0),k=25;break}k=25}while(0);if(25==(0|k))break A;if(26==(0|k)){if(n=0|t[n>>2])break;break A}}}while(0);return 0|A&&dt(A,n,e),k=0|mt(n),G=f,0|k}}while(0);a=0|t[r+12>>2];A:do{if((0|a)>0){n=0,Q=1,B=a,a=0,r=0|t[r+16>>2];e:for(;!(0==(0|(B=B+-1|0))&(u=0|i[r>>0])<<24>>24<0);){for(c=u,w=B,B=0,b=r,l=u<<24>>24<0,r=0;;){if(b=b+1|0,u=127&c,E=0==(0|r))B|=u;else if(!(0|cB(n,u)))break e;if(!l)break;if(E&B>>>0>33554431){if(!n&&!(n=0|qf())){n=0;break e}if(!(0|ta(n,B)))break e;r=1,k=43}else E?(B<<=7,r=0):k=43;if(43==(0|k)&&(k=0,!(0|rn(n,n,7))))break e;if(0==(0|(u=w+-1|0))&(l=(c=0|i[b>>0])<<24>>24<0))break e;w=u}if(Q){do{if(B>>>0>79){if(E){u=2,B=B+-80|0;break}if(0|EB(n,80)){u=2;break}break e}u=Q=(B>>>0)/40|0,B=(0|o(Q,-40))+B|0}while(0);r=A+1|0,(0|e)>1&0!=(0|A)&&(i[A>>0]=u+48,i[r>>0]=0,A=r,e=e+-1|0),a=a+1|0}if(E){t[f+32>>2]=B,Ge(f,26,79257,f+32|0),B=0|mt(f);do{if((0|e)>0&0!=(0|A)){if(dt(A,f,e),(0|e)<(0|B)){A=A+e|0,e=0;break}A=A+B|0,e=e-B|0;break}}while(0);a=B+a|0}else{if(!(r=0|tn(n)))break;u=0|mt(r);do{if(A){if(B=A+1|0,(0|e)>1&&(i[A>>0]=46,i[B>>0]=0,A=B,e=e+-1|0),dt(A,r,e),(0|e)<(0|u)){A=A+e|0,e=0;break}A=A+u|0,e=e-u|0;break}A=0}while(0);Xn(r,82796,0),a=a+1+u|0}if(!((0|w)>0))break A;Q=0,B=w,r=b}return Pf(n),G=f,0|(k=-1)}n=0,a=0}while(0);return Pf(n),G=f,0|(k=a)}function Pr(A,e,r,i,n){A|=0,e|=0,i|=0,n|=0;var t=0,f=0,a=0,B=0;if((0|(r|=0))<=0)return 0|(i=0);for(t=r,B=0;;){if(r=e+(0|o(a=(B+t|0)/2|0,i))|0,(0|(f=0|gB[63&n](A,r)))<0)t=a,r=B;else{if(!f){t=7;break}r=a+1|0}if(!((0|r)<(0|t))){r=0,t=7;break}B=r}return 7==(0|t)?0|r:0}function _r(A,e,r,i,n,t){A|=0,e|=0,i|=0,n|=0,t|=0;var f=0,a=0,B=0,u=0;if(!(r|=0))return 0|(i=0);A:do{if((0|r)>0){for(u=r,B=0;;){if(f=e+(0|o(r=(u+B|0)/2|0,i))|0,(0|(a=0|gB[63&n](A,f)))<0)a=r;else{if(!a){a=0;break A}a=u,B=r+1|0}if(!((0|B)<(0|a)))break;u=a}if(!(1&t))return 0|(i=0);a=1}else a=0,r=0,f=0}while(0);if(0==(2&t|0)|a)return 0|(i=f);A:do{if((0|r)>0)for(;;){if(t=e+(0|o(f=r+-1|0,i))|0,0|gB[63&n](A,t))break A;if(!((0|r)>1)){r=f;break}r=f}}while(0);return 0|(i=e+(0|o(r,i))|0)}function qr(A,e,r){A|=0,e|=0,r|=0;var i,n=0,f=0,a=0,B=0,o=0,u=0;i=G,G=G+32|0;A:do{if(e){t[i+8>>2]=e,n=0|t[20526];do{if(n){if(t[i>>2]=1,t[i+4>>2]=i+8,n=0|ye(n,i)){n=8+(0|t[n+4>>2])|0;break}u=0|t[i+8>>2],f=6;break}u=e,f=6}while(0);do{if(6==(0|f)){for(f=1052,n=0;;){if(B=0|t[37952+((o=(n+f|0)/2|0)<<2)>>2],(0|(a=0|Wt(u,0|t[1888+(24*B|0)>>2])))<0)f=o;else{if(!a){f=12;break}n=o+1|0}if((0|n)>=(0|f)){f=11;break}}if(11==(0|f)){f=14;break A}if(12==(0|f)){n=1888+(24*B|0)+8|0;break}}}while(0);0|t[n>>2]||(f=14)}else f=14}while(0);A:do{if(14==(0|f)){e:do{if(0|r){t[i+8+4>>2]=r,n=0|t[20526];do{if(n){if(t[i>>2]=2,t[i+4>>2]=i+8,n=0|ye(n,i)){n=8+(0|t[n+4>>2])|0;break}u=0|t[i+8+4>>2],f=19;break}u=r,f=19}while(0);do{if(19==(0|f)){for(f=1052,n=0;;){if(B=0|t[42160+((o=(n+f|0)/2|0)<<2)>>2],(0|(a=0|Wt(u,0|t[1888+(24*B|0)+4>>2])))<0)f=o;else{if(!a){f=25;break}n=o+1|0}if((0|n)>=(0|f)){f=24;break}}if(24==(0|f))break e;if(25==(0|f)){n=1888+(24*B|0)+8|0;break}}}while(0);if(0|t[n>>2])break A}}while(0);return(f=0|Kr(A,1))?(0|zr(f)?(Xe(8,100,102,0,0),n=0):(n=0|t[13195],t[13195]=n+1,t[f+8>>2]=n,t[f>>2]=e,t[f+4>>2]=r,n=0|Lr(f),t[f>>2]=0,t[f+4>>2]=0),$(f),G=i,0|(e=n)):(G=i,0|(e=0))}}while(0);return Xe(8,100,102,0,0),G=i,0|(e=0)}function $r(A,e){A|=0,e|=0,DB[31&t[13196]](A,0,e)}function Ai(A,e,r){return 0|(0|Ln(A|=0,e|=0,r|=0))}function ei(A,e,r){e|=0,r|=0;(A|=0)&&Xn(A,e,r)}function ri(A,e,r,i){e|=0,r|=0,i|=0;(A|=0)&&($r(A,e),Xn(A,r,i))}function ii(A,e,r){A|=0,e|=0;var i,n=0,f=0,a=0;i=0==(0|t[(r|=0)>>2]),A?(A=0|kt(A,82796,0))?a=3:(f=0,A=0,e=0,a=11):(A=0,a=3);do{if(3==(0|a)){if(e){if(!(e=0|kt(e,82796,0))){f=0,e=0,a=11;break}}else e=0;if(f=0|Ln(12,82796,0)){if(i&&(a=0|Wn(),t[r>>2]=a,0==(0|a))){Xe(34,105,65,0,0),n=r,a=13;break}if(n=r,t[f>>2]=0,t[f+4>>2]=A,t[f+8>>2]=e,0|yn(0|t[n>>2],f))return 0|(r=1);a=11}else f=0,a=11}}while(0);return 11==(0|a)&&(Xe(34,105,65,0,0),i&&(n=r,a=13)),13==(0|a)&&(Fn(0|t[n>>2]),t[r>>2]=0),Xn(f,82796,0),Xn(A,82796,0),Xn(e,82796,0),0|(r=0)}function ni(A,e){e|=0;var r,i=0;r=G,G=G+32|0,i=0|t[(A|=0)+8>>2];do{if(0|i){if(0|Wt(i,80391)&&0|Wt(i,80396)&&0|Wt(i,80401)&&0|Wt(i,80403)&&0|Wt(i,80405)&&0|Wt(i,80409)){if(0|Wt(i,80385)&&0|Wt(i,80413)&&0|Wt(i,80419)&&0|Wt(i,80421)&&0|Wt(i,80423)&&0|Wt(i,80426))break;return t[e>>2]=0,G=r,0|(A=1)}return t[e>>2]=255,G=r,0|(A=1)}}while(0);return Xe(34,110,104,0,0),i=0|t[A>>2],e=0|t[A+4>>2],A=0|t[A+8>>2],t[r>>2]=80429,t[r+4>>2]=i,t[r+8>>2]=80438,t[r+12>>2]=e,t[r+16>>2]=80445,t[r+20>>2]=A,Ke(6,r),G=r,0|(A=0)}function ti(A){A|=0;var e,r=0,n=0,a=0,B=0,o=0,u=0,l=0,c=0;e=G,G=G+16|0,t[e>>2]=0,c=0|kt(A,82796,0);A:do{if(c){u=0,o=c,a=c,n=1;e:for(;;){switch((A=0|i[o>>0])<<24>>24){case 10:case 13:case 0:l=45;break e}r:do{switch(0|n){case 1:switch(A<<24>>24){case 58:if(i[o>>0]=0,!((A=0|i[a>>0])<<24>>24)){l=17;break e}for(r=a;0|Yt(255&A);)if(!((A=0|i[(r=r+1|0)>>0])<<24>>24)){l=17;break e}if(((n=r+(0|mt(r))|0)+-1|0)!=(0|r)){for(a=n,B=n+-1|0;;){if(!(0|Yt(0|f[B>>0]))){l=15;break}if((0|(n=B+-1|0))==(0|r))break;a=B,B=n}if(15==(0|l)&&(l=0,i[a>>0]=0,A=0|i[r>>0]),0==(0|r)|A<<24>>24==0){l=17;break e}}A=o+1|0,n=2;break r;case 44:if(i[o>>0]=0,!((A=0|i[a>>0])<<24>>24)){l=29;break e}for(r=a;0|Yt(255&A);)if(!((A=0|i[(r=r+1|0)>>0])<<24>>24)){l=29;break e}if(((n=r+(0|mt(r))|0)+-1|0)!=(0|r)){for(a=n,B=n+-1|0;;){if(!(0|Yt(0|f[B>>0]))){l=27;break}if((0|(n=B+-1|0))==(0|r))break;a=B,B=n}if(27==(0|l)&&(l=0,i[a>>0]=0,A=0|i[r>>0]),0==(0|r)|A<<24>>24==0){l=29;break e}}ii(r,0,e),A=o+1|0,n=1;break r;default:r=u,A=a,n=1;break r}case 2:if(A<<24>>24==44){if(i[o>>0]=0,!((A=0|i[a>>0])<<24>>24)){l=42;break e}for(r=a;0|Yt(255&A);)if(!((A=0|i[(r=r+1|0)>>0])<<24>>24)){l=42;break e}if(((n=r+(0|mt(r))|0)+-1|0)!=(0|r)){for(a=n,B=n+-1|0;;){if(!(0|Yt(0|f[B>>0]))){l=40;break}if((0|(n=B+-1|0))==(0|r))break;a=B,B=n}if(40==(0|l)&&(l=0,i[a>>0]=0,A=0|i[r>>0]),0==(0|r)|A<<24>>24==0){l=42;break e}}ii(u,r,e),r=0,A=o+1|0,n=1}else r=u,A=a,n=2;break;default:r=u,A=a}}while(0);u=r,o=o+1|0,a=A}if(17==(0|l)){Xe(34,109,108,0,0);break}if(29==(0|l)){Xe(34,109,108,0,0);break}if(42==(0|l)){Xe(34,109,109,0,0);break}if(45==(0|l)){o=2==(0|n),A=0|i[a>>0];e:do{if(A<<24>>24){for(r=a;0|Yt(255&A);)if(!((A=0|i[(r=r+1|0)>>0])<<24>>24)){A=0;break e}n=r+(0|mt(r))|0;r:do{if((n+-1|0)!=(0|r)){for(a=n,B=n+-1|0;0|Yt(0|f[B>>0]);){if((0|(n=B+-1|0))==(0|r))break r;a=B,B=n}i[a>>0]=0,A=0|i[r>>0]}}while(0);A=A<<24>>24==0?0:r}else A=0}while(0);r=0==(0|A);do{if(o){if(r){Xe(34,109,109,0,0);break A}ii(u,A,e);break}if(r){Xe(34,109,108,0,0);break A}ii(A,0,e);break}while(0);return Xn(c,82796,0),c=0|t[e>>2],G=e,0|c}}else Xe(34,109,65,0,0)}while(0);return Xn(c,82796,0),mn(0|t[e>>2],8),G=e,0|(c=0)}function fi(A,e,r){A|=0,r|=0;var i,n,a=0;n=G,G=G+16|0,i=0|t[(e|=0)>>2],e=0|t[e+8>>2];A:do{if((0|i)>0){for(a=0;;){if(!((a>>>0)%18|0)){if((0|se(A,80479,1))<1){e=0,a=9;break}if((0|Ie(A,r,r))<1){e=0,a=9;break}}if(t[n>>2]=f[e+a>>0],a=a+1|0,t[n+4>>2]=(0|a)==(0|i)?82796:80481,(0|ge(A,80483,n))<1){e=0,a=9;break}if((0|a)>=(0|i))break A}if(9==(0|a))return G=n,0|e}}while(0);return A=1==(0|se(A,80479,1))&1,G=n,0|A}function ai(A,e){e|=0;var r,i=0;return r=G,G=G+16|0,(A|=0)?(i=0|Fi())?(function(A,e){e|=0;var r,i=0,n=0,f=0;if(r=G,G=G+16|0,!(A=A|0))return G=r,0|(A=0);t[r>>2]=0,(0|t[A+24>>2]&&0|(i=0|t[A+12>>2])?0|(n=0|t[i+88>>2]):0)&&(NB[31&n](A),t[A+24>>2]=0);6==(0|t[A+4>>2])&&0!=(0|t[A+12>>2])||(f=8);do{if(8==(0|f)){if(0|(i=0|gA(r,6))){t[A+12>>2]=i,t[A+16>>2]=t[r>>2],t[A>>2]=t[i>>2],t[A+4>>2]=6;break}return Xe(6,158,156,0,0),G=r,0|(A=0)}}while(0);if(t[A+24>>2]=e,!e)return G=r,0|(A=0);(function(A){var e;e=G,G=G+16|0,A=(0|T((A|=0)+52|0,1,e,0|t[A+84>>2]))>0,G=e,t[e>>2]})(e),G=r,A=1}(i,A),t[r>>2]=0,0|function(A,e){e|=0;var r,i=0;if(!(A|=0))return 0|(A=0);r=0|pA(52816);do{if(0|r){if(!(i=0|t[e+12>>2])){Xe(11,120,111,0,0);break}if(!(i=0|t[i+24>>2])){Xe(11,120,124,0,0);break}if(!(0|gB[63&i](r,e))){Xe(11,120,126,0,0);break}return z(0|t[A>>2],52816),t[A>>2]=r,t[r+8>>2]=e,Wi(e),0|(A=1)}}while(0);return z(r,52816),0|(A=0)}(r,i)?(A=0|TA(0|t[r>>2],e,52816),z(0|t[r>>2],52816)):A=0,yi(i),G=r,0|(i=A)):(Xe(13,165,65,0,0),G=r,0|(i=0)):(G=r,0|(i=0))}function Bi(A,e,r,i,n){return e|=0,r|=0,i|=0,n|=0,0|(A|=0)&&(t[A>>2]=t[t[n>>2]>>2]),0|e&&(t[e>>2]=t[8+(0|t[n+4>>2])>>2],t[r>>2]=t[t[n+4>>2]>>2]),i?(t[i>>2]=t[n>>2],1):1}function oi(A,e,r){C()}function ui(A,e,r){A|=0,e|=0,r|=0;var n,f,a;if(a=G,G=G+16|0,t[a+4>>2]=A,t[a>>2]=e,n=0|t[a+4>>2],f=0|t[a>>2],!r)return r=0,G=a,0|(r&=255);e=0,A=0;do{A=i[f+e>>0]^i[n+e>>0]|A,e=e+1|0}while((0|e)!=(0|r));return G=a,0|(r=255&A)}function li(A,e,r){A|=0,e|=0,r|=0;var i,n=0,f=0;i=G,G=G+112|0;do{if(-1==(0|r)){if(!A)return G=i,0|(f=0);if(r=0|t[A+12>>2]){r=0|t[r>>2];break}return G=i,0|(f=0)}}while(0);t[i>>2]=i+4,t[i+4>>2]=r,n=8;do{if(8==(0|n)){if(r=0|Pr(i,46464,4,4,31)){f=0|t[r>>2],n=11;break}break}}while(0);return 11==(0|n)&&0|f?(n=0|Sn(40,82796,0))?(t[n+4>>2]=e,t[n>>2]=f,t[n+16>>2]=0,t[n+8>>2]=A,0|A&&Wi(A),(r=0|t[f+8>>2])?(0|IB[31&r](n))>=1?(G=i,0|(f=n)):(t[n>>2]=0,yi(0|t[n+8>>2]),yi(0|t[n+12>>2]),Xn(n,82796,0),G=i,0|(f=0)):(G=i,0|(f=n))):(Xe(6,157,65,0,0),G=i,0|(f=0)):(Xe(6,157,156,0,0),G=i,0|(f=0))}function ci(A){var e,r=0;return(r=0|t[(A|=0)>>2])&&0|t[r+12>>2]?(e=0|Ln(40,82796,0))?(t[e>>2]=t[A>>2],(r=0|t[A+8>>2])&&(Wi(r),r=0|t[A+8>>2]),t[e+8>>2]=r,(r=0|t[A+12>>2])&&(Wi(r),r=0|t[A+12>>2]),t[e+12>>2]=r,t[e+20>>2]=0,t[e+24>>2]=0,t[e+16>>2]=t[A+16>>2],(0|gB[63&t[12+(0|t[A>>2])>>2]](e,A))>0?0|(A=e):(t[e>>2]=0,yi(0|t[e+8>>2]),yi(0|t[e+12>>2]),Xn(e,82796,0),0|(A=0))):(Xe(6,156,65,0,0),0|(A=0)):0|(A=0)}function Ei(A){var e,r=0;(A|=0)&&(0|(e=0|t[A>>2])&&0|(r=0|t[e+16>>2])&&NB[31&r](A),yi(0|t[A+8>>2]),yi(0|t[A+12>>2]),Xn(A,82796,0))}function Qi(A,e,r,i,n,f){e|=0,r|=0,i|=0,n|=0,f|=0;var a=0,B=0;return 0|(A|=0)&&0|(a=0|t[A>>2])&&0|(B=0|t[a+100>>2])?-1!=(0|e)&&(0|t[a>>2])!=(0|e)?0|(f=-1):(a=0|t[A+16>>2])?-1!=(0|r)&0==(a&r|0)?(Xe(6,137,148,0,0),0|(f=-1)):-2!=(0|(a=0|MB[31&B](A,i,n,f)))?0|(f=a):(Xe(6,137,147,0,0),0|(f=-2)):(Xe(6,137,149,0,0),0|(f=-1)):(Xe(6,137,147,0,0),0|(f=-2))}function wi(A,e,r){A|=0,e|=0;var i;return(0|(i=0|mt(r|=0)))<0?0|(r=-1):0|(r=0|MB[31&t[100+(0|t[A>>2])>>2]](A,e,i,r))}function bi(A,e,r){A|=0,e|=0;var i;return i=G,G=G+16|0,(r=0|Dt(r|=0,i))?(A=0|MB[31&t[100+(0|t[A>>2])>>2]](A,e,0|t[i>>2],r),Xn(r,82796,0),G=i,0|A):(G=i,0|(A=0))}function ki(A,e){e|=0,t[(A|=0)+20>>2]=e}function si(A){return 0|t[(A|=0)+20>>2]}function Ci(A){var e=0;return(A|=0)?(0|(e=0|t[A>>2])&&(0|t[e+32>>2]&&0==(0|xi(A,2))&&IB[31&t[32+(0|t[A>>2])>>2]](A),0|(e=0|t[A>>2])&&0|t[e+40>>2]&&0|t[A+12>>2]&&0==(0|xi(A,4))&&pn(0|t[A+12>>2],0|t[40+(0|t[A>>2])>>2],82796,0)),Ei(0|t[A+16>>2]),$r(A,24),1):1}function di(){return 0|Sn(24,82796,0)}function Ii(A){Ci(A|=0),Xn(A,82796,0)}function gi(A,e,r){e|=0,r|=0;var i=0,n=0;return function(A,e){e|=0,t[(A|=0)+8>>2]=t[A+8>>2]&~e}(A|=0,2),(0|(r=0|t[A>>2]))!=(0|e)&&(0|r&&0|(i=0|t[r+40>>2])&&(pn(0|t[A+12>>2],i,82796,0),t[A+12>>2]=0),t[A>>2]=e,0==(256&t[A+8>>2]|0)&&0|(n=0|t[e+40>>2])&&(t[A+20>>2]=t[e+20>>2],e=0|Sn(n,82796,0),t[A+12>>2]=e,0==(0|e)))?(Xe(6,128,65,0,0),0|(A=0)):0|(r=0|t[A+16>>2])&&!(-2==(0|(e=0|Qi(r,-1,248,7,0,A)))|(0|e)>0)?0|(A=0):256&t[A+8>>2]|0?0|(A=1):0|(A=0|IB[31&t[16+(0|t[A>>2])>>2]](A))}function Di(A,e,r){return e|=0,r|=0,0|DB[31&t[(A|=0)+20>>2]](A,e,r)}function Mi(A,e,r){e|=0,r|=0;var i,n=0;return n=0|t[(A|=0)>>2],(0|t[n+8>>2])>=65&&oi(),i=0|gB[63&t[n+24>>2]](A,e),n=0|t[A>>2],0|r&&(t[r>>2]=t[n+8>>2]),(e=0|t[n+32>>2])&&(IB[31&e](A),ji(A,2),n=0|t[A>>2]),$r(0|t[A+12>>2],0|t[n+40>>2]),0|i}function hi(A,e){A|=0;var r=0,i=0;if(0|(e|=0)&&0|(r=0|t[e>>2])){if((0|t[A>>2])==(0|r)?(r=0|t[A+12>>2],ji(A,4)):r=0,Ci(A),t[A>>2]=t[e>>2],t[A+4>>2]=t[e+4>>2],t[A+8>>2]=t[e+8>>2],t[A+12>>2]=t[e+12>>2],t[A+16>>2]=t[e+16>>2],t[A+20>>2]=t[e+20>>2],t[A+12>>2]=0,t[A+16>>2]=0,0|t[e+12>>2]&&0|(i=0|t[40+(0|t[A>>2])>>2])){do{if(!r){if(r=0|Ln(i,82796,0),t[A+12>>2]=r,0|r){i=0|t[40+(0|t[A>>2])>>2];break}return Xe(6,110,65,0,0),0|(A=0)}t[A+12>>2]=r}while(0);If(0|r,0|t[e+12>>2],0|i)}return t[A+20>>2]=t[e+20>>2],0|(r=0|t[e+16>>2])&&(i=0|ci(r),t[A+16>>2]=i,0==(0|i))?(Ci(A),0|(A=0)):(r=0|t[28+(0|t[A>>2])>>2])?0|(A=0|gB[63&r](A,e)):0|(A=1)}return Xe(6,110,111,0,0),0|(A=0)}function Ui(A,e,r,i,n,f){A|=0,e|=0,r|=0,i|=0,n|=0,f|=0;var a=0;return(a=0|Sn(24,82796,0))?(ji(a,1),0!=(0|gi(a,n,0))&&0!=(0|DB[31&t[a+20>>2]](a,A,e))?(f=0|t[a>>2],(0|t[f+8>>2])>=65&&oi(),A=0|gB[63&t[f+24>>2]](a,r),f=0|t[a>>2],0|i&&(t[i>>2]=t[f+8>>2]),(e=0|t[f+32>>2])&&(IB[31&e](a),ji(a,2),f=0|t[a>>2]),$r(0|t[a+12>>2],0|t[f+40>>2]),f=0!=(0|A)&1):f=0,Ci(a),Xn(a,82796,0),0|(a=f)):0|(a=0)}function Gi(A){var e=0,r=0;return 0|(e=0!=(0|(A|=0))&&0!=(0|(e=0|t[A+12>>2]))&&0!=(0|(r=0|t[e+52>>2]))?0|IB[31&r](A):0)}function Zi(A){var e=0,r=0;return 0|(e=0!=(0|(A|=0))&&0!=(0|(e=0|t[A+12>>2]))&&0!=(0|(r=0|t[e+48>>2]))?0|IB[31&r](A):0)}function Ni(A,e){A|=0,e|=0;var r,i=0,n=0;return r=G,G=G+16|0,t[r>>2]=0,0|A&&(0|t[A+24>>2]&&0|(i=0|t[A+12>>2])&&0|(n=0|t[i+88>>2])&&(NB[31&n](A),t[A+24>>2]=0),(0|t[A+4>>2])==(0|e)&&0|t[A+12>>2])?(G=r,0|(e=1)):(i=0|gA(r,e))?A?(t[A+12>>2]=i,t[A+16>>2]=t[r>>2],t[A>>2]=t[i>>2],t[A+4>>2]=e,G=r,0|(e=1)):(G=r,0|(e=1)):(Xe(6,158,156,0,0),G=r,0|(e=0))}function Fi(){var A=0,e=0;return(A=0|Sn(40,82796,0))?(t[A>>2]=0,t[A+4>>2]=0,t[A+8>>2]=1,t[A+28>>2]=1,e=0|W(),t[A+36>>2]=e,0|e?0|(e=A):(Xe(6,106,65,0,0),Xn(A,82796,0),0|(e=0))):(Xe(6,106,65,0,0),0|(A=0))}function Wi(A){var e;return e=G,G=G+16|0,A=(0|T((A|=0)+8|0,1,e,0|t[A+36>>2]))>0,G=e,A&(0|t[e>>2])>1&1|0}function Yi(A,e,r){e|=0,r|=0;var i,n=0,f=0,a=0;if(i=G,G=G+16|0,!(A|=0))return G=i,0|(e=0);t[i>>2]=0,0|t[A+24>>2]&&0|(n=0|t[A+12>>2])&&0|(f=0|t[n+88>>2])&&(NB[31&f](A),t[A+24>>2]=0),(0|t[A+4>>2])==(0|e)&&0!=(0|t[A+12>>2])||(a=8);do{if(8==(0|a)){if(0|(n=0|gA(i,e))){t[A+12>>2]=n,t[A+16>>2]=t[i>>2],t[A>>2]=t[n>>2],t[A+4>>2]=e;break}return Xe(6,158,156,0,0),G=i,0|(e=0)}}while(0);return t[A+24>>2]=r,G=i,0|(e=0!=(0|r)&1)}function Ri(A){return 0|t[(A|=0)+24>>2]}function yi(A){var e,r,i=0;r=G,G=G+16|0,(A|=0)?(T(A+8|0,-1,r,0|t[A+36>>2]),(0|t[r>>2])>0||(0|(e=0|t[A+12>>2])&&0|(i=0|t[e+88>>2])&&(NB[31&i](A),t[A+24>>2]=0),V(0|t[A+36>>2]),mn(0|t[A+32>>2],18),Xn(A,82796,0)),G=r):G=r}function Vi(A,e){e|=0;var r=0;return(r=0|t[(A|=0)+12>>2])&&(r=0|t[r+92>>2])?0|(e=0|MB[31&r](A,3,0,e)):0|(e=-2)}function mi(A,e,r,i,n){return 0|Hi(A|=0,e|=0,r|=0,i|=0,n|=0,0)}function Hi(A,e,r,i,n,f){e|=0,r|=0,i|=0,n|=0,f|=0;var a,B=0,o=0;if(a=G,G=G+16|0,!(B=0|t[(A|=0)+16>>2])&&(B=0|function(A,e){return 0|li(A|=0,e|=0,-1)}(n,i),t[A+16>>2]=B,!B))return G=a,0|(e=0);o=0|t[B>>2];do{if(0==(0|r)&&0==(4&t[o+4>>2]|0)){if((0|Vi(n,a))>0&&0|(B=0|Xi(0|pr(0|t[a>>2])))){n=0|t[A+16>>2],o=0|t[n>>2],r=B;break}return Xe(6,161,158,0,0),G=a,0|(e=0)}n=B}while(0);do{if(f){if(!(B=0|t[o+68>>2])){if(!((0|_i(n))<1))break;return G=a,0|(B=0)}if((0|gB[63&B](n,A))<1)return G=a,0|(e=0);t[16+(0|t[A+16>>2])>>2]=128;break}if(!(B=0|t[o+60>>2])){if(!((0|Oi(n))<1))break;return G=a,0|(B=0)}if((0|gB[63&B](n,A))<1)return G=a,0|(e=0);t[16+(0|t[A+16>>2])>>2]=64;break}while(0);return(0|Qi(0|t[A+16>>2],-1,248,1,0,r))<1?(G=a,0|(e=0)):(0|e&&(t[e>>2]=t[A+16>>2]),4&t[4+(0|t[t[A+16>>2]>>2])>>2]|0?(G=a,0|(e=1)):(e=0!=(0|gi(A,r,i))&1,G=a,0|e))}function vi(A,e,r,i,n){return 0|Hi(A|=0,e|=0,r|=0,i|=0,n|=0,1)}function Ji(A,e,r){e|=0,r|=0;var i,n,f=0,a=0,B=0;if(n=G,G=G+80|0,a=0|t[(A|=0)+16>>2],f=0|t[a>>2],4&t[f+4>>2]|0)return e?512&t[A+8>>2]|0?(r=0|MB[31&t[f+64>>2]](a,e,r,A),G=n,0|r):(f=0|ci(a))?(r=0|MB[31&t[64+(0|t[f>>2])>>2]](f,e,r,A),Ei(f),G=n,0|r):(G=n,0|(r=0)):(r=0|MB[31&t[f+64>>2]](a,0,r,A),G=n,0|r);i=0|t[f+64>>2];do{if(e){t[n+64>>2]=0;do{if(512&t[A+8>>2]){if(i){f=0|MB[31&i](a,e,r,A);break}f=0|Mi(A,n,n+64|0),B=20;break}if(a=0|di()){if(!(0|hi(a,A))){Ii(a),f=0;break}i?(f=0|t[a+16>>2],f=0|MB[31&t[64+(0|t[f>>2])>>2]](f,e,r,a)):f=0|Mi(a,n,n+64|0),Ii(a),B=20}else f=0}while(0);if(20==(0|B)&&0==(0|i)&0!=(0|f)){if(!((0|Pi(0|t[A+16>>2],e,r,n,0|t[n+64>>2]))<1))break;f=0}return G=n,0|(r=f)}if(0|i){if(!((0|MB[31&i](a,0,r,A))<1))break;return G=n,0|(f=0)}if((0|(f=0|pi(0|t[A>>2])))<0)return G=n,0|(r=0);if((0|Pi(a,0,r,0,f))<1)return G=n,0|(r=0)}while(0);return G=n,0|(r=1)}function Xi(A){return A|=0,0|Kn(8,0,0)?0|(A=0|function(A,e){var r;if(e|=0,r=G,G=G+16|0,!(A|=0))return G=r,0|(e=0);if(0==(0|m(82080,2))|0==(0|t[20521]))return G=r,0|0;Y(0|t[20523]),t[r+8>>2]=A,t[r>>2]=-32769&e,A=0|ye(0|t[20522],r);A:do{if(A){e:do{if(!(32768&e))for(e=0;;){if(!(0|t[A+4>>2]))break e;if(e>>>0>9){A=0;break A}if(t[r+8>>2]=t[A+12>>2],!(A=0|ye(0|t[20522],r))){A=0;break A}e=e+1|0}}while(0);A=0|t[A+12>>2]}else A=0}while(0);return y(0|t[20523]),G=r,0|A}(A,1)):0|(A=0)}function Ti(){Tr(2),Tr(1),Tr(-1),mn(0|t[20539],17),mn(0|t[20519],11),Fn(0|t[20519])}function Li(A){return 0|t[(A|=0)+36>>2]}function Si(A){return 0|t[(A|=0)>>2]}function pi(A){return(A|=0)?0|(A=0|t[A+8>>2]):(Xe(6,162,159,0,0),0|(A=-1))}function zi(A){return 0|t[(A|=0)+16>>2]}function Ki(A){return 0|t[(A|=0)+12>>2]}function ji(A,e){e|=0,t[(A|=0)+8>>2]=t[A+8>>2]|e}function xi(A,e){return e|=0,t[(A|=0)+8>>2]&e|0}function Oi(A){var e=0;return 0|(A|=0)&&0|(e=0|t[A>>2])&&0|t[e+40>>2]?(t[A+16>>2]=8,(e=0|t[e+36>>2])?((0|(e=0|IB[31&e](A)))>=1||(t[A+16>>2]=0),0|(A=e)):0|(A=1)):(Xe(6,141,150,0,0),0|(A=-2))}function Pi(A,e,r,i,n){e|=0,r|=0,i|=0,n|=0;var f=0,a=0;if(0|(A|=0)&&0|(a=0|t[A>>2])&&0|(f=0|t[a+40>>2])){if(8!=(0|t[A+16>>2]))return Xe(6,140,151,0,0),0|(n=-1);do{if(2&t[a+4>>2]){if(!(f=0|Zi(0|t[A+8>>2])))return Xe(6,140,163,0,0),0|(n=0);if(!e)return t[r>>2]=f,0|(n=1);if((0|t[r>>2])>>>0>=f>>>0){f=0|t[40+(0|t[A>>2])>>2];break}return Xe(6,140,155,0,0),0|(n=0)}}while(0);return 0|(n=0|hB[15&f](A,e,r,i,n))}return Xe(6,140,150,0,0),0|(n=-2)}function _i(A){var e=0;return 0|(A|=0)&&0|(e=0|t[A>>2])&&0|t[e+48>>2]?(t[A+16>>2]=16,(e=0|t[e+44>>2])?((0|(e=0|IB[31&e](A)))>=1||(t[A+16>>2]=0),0|(A=e)):0|(A=1)):(Xe(6,143,150,0,0),0|(A=-2))}function qi(A,e){!function(A,e,r){e|=0,r|=0,t[(A|=0)>>2]=2,t[A+4>>2]=r,t[A+8>>2]=e}(A|=0,17,e|=0)}function $i(A,e,r,i){r|=0,i|=0;var n=0;if(!(A=0|function(A,e){return 0|li(0,e|=0,A|=0)}(A|=0,e|=0)))return 0|(n=0);e=0|t[A>>2];do{if(0!=(0|e)&&0!=(0|t[e+32>>2])){if(t[A+16>>2]=4,0|(e=0|t[e+28>>2])&&(0|IB[31&e](A))<1){t[A+16>>2]=0,e=0;break}if((0|Qi(A,-1,4,6,i,r))<1)e=0;else{if(0|(i=0|t[A>>2])&&0|t[i+32>>2]){if(4!=(0|t[A+16>>2])){Xe(6,146,151,0,0),e=0;break}if(!(e=0|Fi())){e=0;break}if((0|gB[63&t[32+(0|t[A>>2])>>2]](A,e))>=1)break;yi(e),e=0;break}Xe(6,146,150,0,0),e=0}}else n=4}while(0);return 4==(0|n)&&(Xe(6,147,150,0,0),e=0),Ei(A),0|(n=e)}function An(A,e){A|=0,e|=0;var r=0,i=0,n=0,f=0,a=0,B=0;do{if((0|A)==(0|e)){if(0|ia(A,1+(0|t[A+4>>2])|0)){f=e+4|0;break}return 0|(f=0)}if(t[A+12>>2]=t[e+12>>2],0|ia(A,1+(0|t[e+4>>2])|0)){t[A+4>>2]=t[e+4>>2],f=e+4|0;break}return 0|(f=0)}while(0);if((0|t[f>>2])<=0)return 0|(f=1);for(e=0|t[e>>2],r=0,i=0,n=0|t[A>>2];a=0|t[e>>2],B=n,n=n+4|0,t[B>>2]=a<<1|r,r=a>>>31,!((0|(i=i+1|0))>=(0|t[f>>2]));)e=e+4|0;return r?(t[n>>2]=1,t[A+4>>2]=1+(0|t[A+4>>2]),0|(B=1)):0|(B=1)}function en(A,e){A|=0;var r,i,n,f,a=0,B=0,o=0;if(0|jf(e|=0))return ba(A),0|(A=1);i=0|t[e+4>>2],n=0|t[e>>2],f=0|t[n+(i+-1<<2)>>2];do{if((0|e)!=(0|A)){if(0|ia(A,i-(1==(0|f)&1)|0)){t[A+12>>2]=t[e+12>>2],e=0|t[n+(i+-1<<2)>>2];break}return 0|(A=0)}e=f}while(0);if(r=0|t[A>>2],0|(a=e>>>1)&&(t[r+(i+-1<<2)>>2]=a),(0|i)>1){a=i+-1|0;do{B=a,o=e,e=0|t[n+((a=a+-1|0)<<2)>>2],t[r+(a<<2)>>2]=e>>>1|o<<31}while((0|B)>1)}return t[A+4>>2]=i-(1==(0|f)&1),i-(1==(0|f)&1)|0||(t[A+12>>2]=0),0|(o=1)}function rn(A,e,r){A|=0,e|=0;var i,n,f=0,a=0,B=0;if((0|(r|=0))<0)return Xe(3,145,119,0,0),0|(A=0);if(!(0|ia(A,1+(r>>>5)+(0|t[e+4>>2])|0)))return 0|(A=0);if(t[A+12>>2]=t[e+12>>2],i=0|t[e>>2],n=0|t[A>>2],t[n+((0|t[e+4>>2])+(r>>>5)<<2)>>2]=0,f=0|t[e+4>>2],31&r){if((0|f)>0)for(f=f+-1|0;B=0|t[i+(f<<2)>>2],t[n+((a=f+(r>>>5)|0)+1<<2)>>2]=t[n+(a+1<<2)>>2]|B>>>(32-(31&r)|0),t[n+(a<<2)>>2]=B<<(31&r),(0|f)>0;)f=f+-1|0}else if((0|f)>0)for(f=f+-1|0;t[n+(f+(r>>>5)<<2)>>2]=t[i+(f<<2)>>2],(0|f)>0;)f=f+-1|0;return Df(0|n,0,r>>>5<<2|0),t[A+4>>2]=1+(r>>>5)+(0|t[e+4>>2]),aa(A),0|(B=1)}function nn(A,e,r){A|=0,e|=0;var i=0,n=0,f=0,a=0,B=0,o=0;if((0|(r|=0))<0)return Xe(3,146,119,0,0),0|(A=0);if((r>>>5|0)>=(0|(B=0|t[e+4>>2]))|0==(0|B))return ba(A),0|(A=1);i=(31-r+(0|Kf(e))|0)/32|0;do{if((0|A)!=(0|e)){if(0|ia(A,i)){t[A+12>>2]=t[e+12>>2];break}return 0|(A=0)}if(!r)return 0|(A=1)}while(0);if(f=(0|t[e>>2])+(r>>>5<<2)|0,B=0|t[A>>2],e=(0|t[e+4>>2])-(r>>>5)|0,t[A+4>>2]=i,31&r){if(n=(0|t[f>>2])>>>(31&r),e+-1|0){for(a=e+-1|0,i=B;o=0|t[(f=f+4|0)>>2],t[i>>2]=o<<32-(31&r)|n,n=o>>>(31&r),a=a+-1|0;)i=i+4|0;i=B+(e+-1<<2)|0}else i=B;n?(t[i>>2]=n,n=19):n=19}else if(e)for(n=f,i=B;;){if(t[i>>2]=t[n>>2],!(e=e+-1|0)){n=19;break}n=n+4|0,i=i+4|0}return 19==(0|n)&&(i=0|t[A+4>>2]),0|i||(t[A+12>>2]=0),0|(o=1)}function tn(A){var e,r,n,f=0,a=0,B=0,o=0,u=0;n=G,G=G+16|0,e=0|Ln(4+((a=(((0|(o=3*(0|Kf(A|=0))|0))/10|0)+((0|o)/1e3|0)+2|0)/9|0)<<2)|0,82796,0),r=0|Ln(o=((0|o)/10|0)+((0|o)/1e3|0)+5|0,82796,0);A:do{if(!(0==(0|e)|0==(0|r))){if(!(B=0|ea(A))){Xn(e,82796,0),Pf(0);break}if(0|jf(B))i[r>>0]=48,i[r+1>>0]=0;else{0|Ia(B)?(i[r>>0]=45,A=r+1|0):A=r;e:do{if(0|jf(B))f=e;else{for(f=e;;){if((f-e>>2|0)>(0|a)){a=22;break}if(u=0|lB(B,1e9),t[f>>2]=u,-1==(0|u)){a=23;break}if(f=f+4|0,0|jf(B))break e}if(22==(0|a)){Xn(e,82796,0),Pf(B);break A}if(23==(0|a)){Xn(e,82796,0),Pf(B);break A}}}while(0);for(f=f+-4|0,t[n>>2]=t[f>>2],Ge(A,o+r-A|0,80616,n);0|i[A>>0];)A=A+1|0;if((0|f)!=(0|e))do{for(f=f+-4|0,t[n+8>>2]=t[f>>2],Ge(A,o+r-A|0,80619,n+8|0);0|i[A>>0];)A=A+1|0}while((0|f)!=(0|e))}return Xn(e,82796,0),Pf(B),G=n,0|(u=r)}Xe(3,104,65,0,0),Xn(e,82796,0),Pf(0)}while(0);return Xn(r,82796,0),G=n,0|(u=0)}function fn(A,e){A|=0;var r=0,n=0,a=0,B=0,o=0,u=0,l=0,c=0,E=0;if(!(e|=0))return 0|(A=0);switch(0|i[e>>0]){case 45:e=e+1|0,c=1;break;case 0:return 0|(A=0);default:c=0}for(r=0;;){if(!(0|Rt(0|f[e+r>>0]))){n=7;break}if((r=r+1|0)>>>0>=536870912){e=0;break}}do{if(7==(0|n))if((r+-1|0)>>>0<=536870910){if(l=r+c|0,!A)return 0|(A=l);if(n=0|t[A>>2])ba(n);else if(!(n=0|qf()))return 0|(A=0);if(!(r>>>0>536870904)){if((0|(a=(31+(r<<2)|0)>>>5))>(0|t[n+8>>2])&&0==(0|Aa(n,a))){e=n;break}for(o=1+((r+((o=~r)>>>0>4294967287?o:-9)+8|0)>>>3)|0,u=0;;){for(a=0,B=(0|r)<8?r:8;a=((0|(E=0|gt(0|i[e+(r-B)>>0])))>0?E:0)|a<<4,!((0|B)<2);)B=B+-1|0;if(t[(0|t[n>>2])+(u<<2)>>2]=a,(0|(u=u+1|0))==(0|o))break;r=r+-8|0}return t[(E=n+4|0)>>2]=o,aa(n),t[A>>2]=n,0|t[E>>2]?(t[n+12>>2]=c,0|(E=l)):0|(E=l)}e=n}else e=0}while(0);return 0|t[A>>2]||Pf(e),0|(E=0)}function an(A,e){A|=0;var r=0,n=0,a=0,B=0,o=0,u=0,l=0;if(!(e|=0))return 0|(A=0);switch(0|i[e>>0]){case 45:e=e+1|0,l=1;break;case 0:return 0|(A=0);default:l=0}for(r=0;;){if(((0|f[e+r>>0])-48|0)>>>0>=10){n=7;break}if((r=r+1|0)>>>0>=536870912){e=0;break}}A:do{if(7==(0|n))if((r+-1|0)>>>0<=536870910){if(u=r+l|0,!A)return 0|(A=u);if(n=0|t[A>>2])ba(n);else if(!(n=0|qf()))return 0|(A=0);if(!(r>>>0>536870904)){if((0|(a=(31+(r<<2)|0)>>>5))>(0|t[n+8>>2])&&0==(0|Aa(n,a))){e=n;break}a=0==(0|(a=(r>>>0)%9|0))?0:9-a|0,B=0;do{if(o=r,r=r+-1|0,B=(10*B|0)-48+(0|i[e>>0])|0,e=e+1|0,9==(0|(a=a+1|0))){if(!(0|QB(n,1e9))){e=n;break A}if(!(0|cB(n,B))){e=n;break A}a=0,B=0}}while((0|o)>1);return aa(n),t[A>>2]=n,0|t[n+4>>2]?(t[n+12>>2]=l,0|(A=u)):0|(A=u)}e=n}else e=0}while(0);return 0|t[A>>2]||Pf(e),0|(A=0)}function Bn(A,e){A|=0;var r,n=0;r=45==(0|i[(e|=0)>>0])?e+1|0:e;A:do{if(48==(0|i[r>>0])){switch(0|i[r+1>>0]){case 120:case 88:break;default:n=4;break A}if(!(0|fn(A,r+2|0)))return 0|(n=0)}else n=4}while(0);return 4==(0|n)&&0==(0|an(A,r))?0|(n=0):45!=(0|i[e>>0])?0|(n=1):(e=0|t[A>>2],0|t[e+4>>2]?(t[e+12>>2]=1,0|(n=1)):0|(n=1))}function on(A,e,r){A|=0,r|=0;var i=0,n=0,f=0,a=0,B=0,o=0;if((0|(o=0|t[(e|=0)+12>>2]))!=(0|t[r+12>>2]))return i=0==(0|o)?r:e,(0|la(e=0==(0|o)?e:r,i))<0?0|un(A,i,e)?(t[A+12>>2]=1,0|(o=1)):0|(o=0):0|un(A,e,i)?(t[A+12>>2]=0,0|(o=1)):0|(o=0);if(i=(B=(0|t[e+4>>2])<(0|t[r+4>>2]))?e:r,B=0|t[(e=B?r:e)+4>>2],f=0|t[i+4>>2],0|ia(A,B+1|0)){if(t[A+4>>2]=B,e=0|t[e>>2],r=0|hn(a=0|t[A>>2],e,0|t[i>>2],f),B-f|0){for(n=e+(f<<2)|0,i=B-f|0,e=a+(f<<2)|0;i=i+-1|0,f=(0|t[n>>2])+r|0,t[e>>2]=f,r&=0==(0|f),i;)n=n+4|0,e=e+4|0;e=a+(B<<2)|0}else e=a+(f<<2)|0;t[e>>2]=r,t[A+4>>2]=(0|t[A+4>>2])+r,t[A+12>>2]=0,e=A+12|0,r=1}else e=A+12|0,r=0;return t[e>>2]=o,0|(o=r)}function un(A,e,r){A|=0,r|=0;var i,n=0,f=0,a=0,B=0;if(((n=0|t[(e|=0)+4>>2])-(f=0|t[r+4>>2])|0)<0)return Xe(3,115,100,0,0),0|(A=0);if(!(0|ia(A,n)))return 0|(A=0);if(e=0|t[e>>2],r=0|Un(i=0|t[A>>2],e,0|t[r>>2],f),n-f|0){for(B=e+(f<<2)|0,a=n-f|0,f=i+(f<<2)|0;a=a+-1|0,e=0|t[B>>2],t[f>>2]=e-r,a;)B=B+4|0,r&=0==(0|e),f=f+4|0;e=i+(n<<2)|0}else e=i+(f<<2)|0;A:do{if(n)for(;;){if(0|t[(e=e+-4|0)>>2])break A;if(!(n=n+-1|0)){n=0;break}}else n=0}while(0);return t[A+4>>2]=n,t[A+12>>2]=0,0|(A=1)}function ln(A,e,r){A|=0,r|=0;var i,n=0,f=0,a=0,B=0;if(n=(B=(0|t[(e|=0)+4>>2])<(0|t[r+4>>2]))?e:r,B=0|t[(e=B?r:e)+4>>2],a=0|t[n+4>>2],!(0|ia(A,B+1|0)))return 0|(A=0);if(t[A+4>>2]=B,e=0|t[e>>2],r=0|hn(i=0|t[A>>2],e,0|t[n>>2],a),B-a|0){for(f=e+(a<<2)|0,n=B-a|0,e=i+(a<<2)|0;n=n+-1|0,a=(0|t[f>>2])+r|0,t[e>>2]=a,r&=0==(0|a),n;)f=f+4|0,e=e+4|0;e=i+(B<<2)|0}else e=i+(a<<2)|0;return t[e>>2]=r,t[A+4>>2]=(0|t[A+4>>2])+r,t[A+12>>2]=0,0|(A=1)}function cn(A,e,r){A|=0,e|=0;var i=0,n=0,f=0,a=0,B=0,o=0;if(i=0!=(0|t[(r|=0)+12>>2]),0|t[e+12>>2]?i?i=r:(o=1&(1^i),n=4):i?(o=0,n=4):(i=e,e=r),4==(0|n)){if(i=(B=(0|t[e+4>>2])<(0|t[r+4>>2]))?e:r,B=0|t[(e=B?r:e)+4>>2],f=0|t[i+4>>2],!(0|ia(A,B+1|0)))return 0|(A=0);if(t[A+4>>2]=B,e=0|t[e>>2],i=0|hn(a=0|t[A>>2],e,0|t[i>>2],f),B-f|0){for(n=e+(f<<2)|0,r=B-f|0,e=a+(f<<2)|0;r=r+-1|0,f=(0|t[n>>2])+i|0,t[e>>2]=f,i&=0==(0|f),r;)n=n+4|0,e=e+4|0;e=a+(B<<2)|0}else e=a+(f<<2)|0;return t[e>>2]=i,t[A+4>>2]=(0|t[A+4>>2])+i,t[A+12>>2]=o,0|(A=1)}return 0|ia(A,(0|(B=0|t[i+4>>2]))>(0|(o=0|t[e+4>>2]))?B:o)?(0|la(i,e))<0?0|un(A,e,i)?(t[A+12>>2]=1,0|(A=1)):0|(A=0):0|un(A,i,e)?(t[A+12>>2]=0,0|(A=1)):0|(A=0):0|(A=0)}function En(A){return 0|t[(A|=0)>>2]}function Qn(A){t[(A|=0)+16>>2]=0}function wn(A){(A|=0)&&(Pf(0|t[A>>2]),Pf(0|t[A+4>>2]),Pf(0|t[A+8>>2]),Pf(0|t[A+12>>2]),V(0|t[A+36>>2]),Xn(A,82796,0))}function bn(A,e,r,i,n,f){e|=0,r|=0,i|=0,n|=0,f|=0;var a,B=0,o=0,u=0,l=0,c=0,E=0;if(a=G,G=G+16|0,A|=0)r=A;else if(!(r=0|function(A,e,r){A|=0,e|=0,r|=0;var i,n=0,f=0;if(!(i=0|Sn(40,82796,0)))return Xe(3,102,65,0,0),0|(n=0);if(f=0|W(),t[i+36>>2]=f,!f)return Xe(3,102,65,0,0),Xn(i,82796,0),0|(f=0);t[i+16>>2]=0,(0==(0|A)||(f=0|ea(A),t[i>>2]=f,0|f))&&(n=7);do{if(7==(0|n)){if(0|e&&(f=0|ea(e),t[i+4>>2]=f,0==(0|f)))break;if(f=0|ea(r),t[i+12>>2]=f,0|f)return 0|Of(r,4)&&ha(0|t[i+12>>2],4),t[i+20>>2]=-1,0|(f=i)}}while(0);return Pf(0|t[i>>2]),Pf(0|t[i+4>>2]),Pf(0|t[i+8>>2]),Pf(0|t[i+12>>2]),V(0|t[i+36>>2]),Xn(i,82796,0),0|(f=0)}(0,0,r)))return G=a,0|(E=0);E=0==(0|t[r>>2])&&(E=0|qf(),t[r>>2]=E,0==(0|E))?30:5;A:do{if(5==(0|E)){if(0==(0|t[(u=r+4|0)>>2])&&(o=0|qf(),t[u>>2]=o,0==(0|o))){E=30;break}if(B=0|t[(o=r+8|0)>>2],e&&(Pf(B),B=0|ea(e),t[o>>2]=B),B){0|n&&(t[r+32>>2]=n),0|f&&(t[r+28>>2]=f),B=r+12|0;e:do{if(0|Ya(0|t[r>>2],0|t[B>>2])){for(e=32;!(0|vf(0|t[u>>2],0|t[r>>2],0|t[B>>2],i,a));){if(!(0|t[a>>2]))break e;if(!e){E=19;break}if(!(0|Ya(0|t[r>>2],0|t[B>>2])))break e;e=e+-1|0}if(19==(0|E)){Xe(3,128,113,0,0);break}if(0!=(0|(e=0|t[r+32>>2]))&&0!=(0|(c=0|t[(l=r+28|0)>>2]))){if(f=0|t[r>>2],!(0|UB[7&e](f,f,0|t[o>>2],0|t[B>>2],i,c))){E=30;break A}}else{if(!(0|Oa(c=0|t[r>>2],c,0|t[o>>2],0|t[B>>2],i))){E=30;break A}l=r+28|0}if(!(e=0|t[l>>2]))return G=a,0|(E=r);if(!(0|Xa(c=0|t[u>>2],c,e,i))){E=30;break A}if(!(0!=(0|Xa(c=0|t[r>>2],c,0|t[l>>2],i))|0==(0|A)^1))break A;return G=a,0|r}}while(0);E=30}else E=30}}while(0);do{if(30==(0|E)){if(A)return G=a,0|(E=r);u=r+4|0,o=r+8|0,B=r+12|0;break}}while(0);return Pf(0|t[r>>2]),Pf(0|t[u>>2]),Pf(0|t[o>>2]),Pf(0|t[B>>2]),V(0|t[r+36>>2]),Xn(r,82796,0),G=a,0|(E=0)}function kn(A,e,r,i){A|=0,e|=0,i|=0;var n=0;if(0|t[(r|=0)>>2]&&0|t[r+4>>2]){if(-1!=(0|t[r+20>>2])){if(!(0|function(A,e){A|=0,e|=0;var r=0,i=0,n=0;A:do{if(0!=(0|t[A>>2])&&(i=0|t[A+4>>2],0!=(0|i))){if(-1!=(0|(r=0|t[A+20>>2])))if(t[A+20>>2]=r+1,32==(r+1|0)&&0!=(0|t[A+8>>2])&&0==(2&t[A+24>>2]|0)){if(!(0|bn(A,0,0,e,0,0))){r=A+20|0,i=0;break}}else n=10;else t[A+20>>2]=1,n=10;do{if(10==(0|n)&&0==(1&t[A+24>>2]|0)){if(r=0|t[A+28>>2]){if(!(0|ma(i,i,i,r,e))){r=A+20|0,i=0;break A}if(0|ma(i=0|t[A>>2],i,i,0|t[A+28>>2],e))break;r=A+20|0,i=0;break A}if(!(0|AB(i,i,i,0|t[A+12>>2],e))){r=A+20|0,i=0;break A}if(0|AB(i=0|t[A>>2],i,i,0|t[A+12>>2],e))break;r=A+20|0,i=0;break A}}while(0);r=A+20|0,i=1}else n=3}while(0);return 3==(0|n)&&(Xe(3,103,107,0,0),r=A+20|0,i=0),32!=(0|t[r>>2])||(t[r>>2]=0),0|i}(r,i)))return 0|(A=0)}else t[r+20>>2]=0;return 0|e&&0==(0|ra(e,0|t[r+4>>2]))?0|(A=0):(e=0|t[r+28>>2],n=0|t[r>>2],e?0|(A=0|Va(A,A,n,e,i)):0|(A=0|AB(A,A,n,0|t[r+12>>2],i)))}return Xe(3,100,107,0,0),0|(A=0)}function sn(A,e,r,i){A|=0,r|=0,i|=0;var n,f=0,a=0,B=0,o=0;if(!(e|=0)&&!(e=0|t[r+4>>2]))return Xe(3,101,107,0,0),0|(A=0);if(!(n=0|t[r+28>>2]))return 0|(A=0|AB(A,A,e,0|t[r+12>>2],i));if(r=0|t[e+4>>2],(0|t[A+8>>2])>=(0|r)){if(f=0|t[A+4>>2],0|r){a=0|t[A>>2],B=0;do{t[(o=a+(B<<2)|0)>>2]=t[o>>2]&B-f>>31,B=B+1|0}while((0|B)!=(0|r))}t[A+4>>2]=r&~(r-f>>31)|r-f>>31&f}return 0|(o=0|Va(A,A,e,n,i))}function Cn(A){return 0,0|function(A,e){return(0|(A|=0))==(0|(e|=0))|0}(0,0|t[(A|=0)+16>>2])}function dn(A){return 0|R(0|t[(A|=0)+36>>2])}function In(A){return 0|y(0|t[(A|=0)+36>>2])}function gn(A,e,r,i){A|=0,e|=0,i|=0;var n=0,f=0,a=0,B=0,o=0,u=0,l=0,c=0;if((0|(r|=0))<1)return 0|(i=0);if(r>>>0>3){for(f=e,n=0,a=r,B=A;u=0|lf(0|t[f>>2],0,0|i,0),c=0|E(),l=0|cf(0|t[B>>2],0,0|n,0),c=0|cf(0|l,0|E(),0|u,0|c),u=0|E(),t[B>>2]=c,l=0|cf(0|(c=0|lf(0|t[f+4>>2],0,0|i,0)),0|(l=0|E()),0|t[(o=B+4|0)>>2],0),u=0|cf(0|l,0|E(),0|u,0),l=0|E(),t[o>>2]=u,u=0|cf(0|(o=0|lf(0|t[f+8>>2],0,0|i,0)),0|(u=0|E()),0|t[(c=B+8|0)>>2],0),l=0|cf(0|u,0|E(),0|l,0),u=0|E(),t[c>>2]=l,l=0|cf(0|(c=0|lf(0|t[f+12>>2],0,0|i,0)),0|(l=0|E()),0|t[(o=B+12|0)>>2],0),u=0|cf(0|l,0|E(),0|u,0),n=0|E(),t[o>>2]=u,!((a=a+-4|0)>>>0<=3);)f=f+16|0,B=B+16|0;if(!(f=r+-4-(r+-4&-4)|0))return 0|(c=n);e=e+(4+(r+-4&-4)<<2)|0,A=A+(4+(r+-4&-4)<<2)|0}else n=0,f=r;for(;l=0|lf(0|t[e>>2],0,0|i,0),c=0|E(),n=0|cf(0|t[A>>2],0,0|n,0),c=0|cf(0|n,0|E(),0|l,0|c),n=0|E(),t[A>>2]=c,f=f+-1|0;)e=e+4|0,A=A+4|0;return 0|n}function Dn(A,e,r,i){A|=0,e|=0,i|=0;var n=0,f=0,a=0,B=0,o=0,u=0;if((0|(r|=0))<1)return 0|(i=0);if(r>>>0>3){for(f=e,n=0,a=r,B=A;u=0|cf(0|(u=0|lf(0|t[f>>2],0,0|i,0)),0|E(),0|n,0),o=0|E(),t[B>>2]=u,o=0|cf(0|(u=0|lf(0|t[f+4>>2],0,0|i,0)),0|E(),0|o,0),u=0|E(),t[B+4>>2]=o,u=0|cf(0|(o=0|lf(0|t[f+8>>2],0,0|i,0)),0|E(),0|u,0),o=0|E(),t[B+8>>2]=u,o=0|cf(0|(u=0|lf(0|t[f+12>>2],0,0|i,0)),0|E(),0|o,0),n=0|E(),t[B+12>>2]=o,!((a=a+-4|0)>>>0<=3);)f=f+16|0,B=B+16|0;if(!(f=r+-4-(r+-4&-4)|0))return 0|(u=n);e=e+(4+(r+-4&-4)<<2)|0,A=A+(4+(r+-4&-4)<<2)|0}else n=0,f=r;for(;u=0|cf(0|(u=0|lf(0|t[e>>2],0,0|i,0)),0|E(),0|n,0),n=0|E(),t[A>>2]=u,f=f+-1|0;)e=e+4|0,A=A+4|0;return 0|n}function Mn(A,e,r){e|=0;var i,n,t=0,f=0,a=0,B=0,u=0,l=0,c=0;if(!(r|=0))return 0|(r=-1);for(c=(A|=0)-(A>>>0<r>>>0?0:r)|0,i=r<<32-(n=0|zf(r))>>>16,A=2,c=0==(32-n|0)?c:c<<32-n|e>>>n,u=e<<32-n,l=0;;){a=0|o(t=(c>>>16|0)==(0|i)?65535:(c>>>0)/(i>>>0)|0,i),e=0|o(t,r<<32-n&65535),f=c-a|0;A:do{if(f>>>0>65535)f=a;else for(B=u>>>16;;){if(e>>>0<=(f<<16|B)>>>0){f=a;break A}if(t=t+-1|0,e=e-(r<<32-n&65535)|0,(f=c-(a=a-i|0)|0)>>>0>65535){f=a;break}}}while(0);if(a=(e>>>16)+f+(u>>>0<(B=e<<16)>>>0&1)|0,f=u-B|0,t=t+((e=c>>>0<a>>>0)<<31>>31)|0,!(A=A+-1|0))break;c=c-a+(e?r<<32-n:0)<<16|f>>>16,u=f<<16,l=t<<16}return 0|(r=t|l)}function hn(A,e,r,i){A|=0,e|=0,r|=0;var n=0,f=0,a=0,B=0,o=0,u=0,l=0,c=0;if((0|(i|=0))<1)return 0|(l=0);if(i>>>0>3){for(l=4+(i+-4&-4)|0,f=0,n=0,a=e,B=r,o=i,u=A;c=0|t[B>>2],n=0|cf(0|f,0|n,0|t[a>>2],0),c=0|cf(0|n,0|E(),0|c,0),n=0|E(),t[u>>2]=c,c=0|cf(0|t[B+4>>2],0,0|t[a+4>>2],0),n=0|cf(0|c,0|E(),0|n,0),c=0|E(),t[u+4>>2]=n,n=0|cf(0|t[B+8>>2],0,0|t[a+8>>2],0),c=0|cf(0|n,0|E(),0|c,0),n=0|E(),t[u+8>>2]=c,c=0|cf(0|t[B+12>>2],0,0|t[a+12>>2],0),n=0|cf(0|c,0|E(),0|n,0),f=0|E(),t[u+12>>2]=n,!((o=o+-4|0)>>>0<=3);)n=0,a=a+16|0,B=B+16|0,u=u+16|0;if(!(i=i+-4-(i+-4&-4)|0))return 0|(c=f);n=0,e=e+(l<<2)|0,r=r+(l<<2)|0,A=A+(l<<2)|0}else f=0,n=0;for(;c=0|t[r>>2],l=0|cf(0|f,0|n,0|t[e>>2],0),c=0|cf(0|l,0|E(),0|c,0),f=0|E(),t[A>>2]=c,i=i+-1|0;)n=0,e=e+4|0,r=r+4|0,A=A+4|0;return 0|(c=f)}function Un(A,e,r,i){A|=0,e|=0,r|=0;var n=0,f=0,a=0,B=0,o=0,u=0,l=0,c=0,E=0;if((0|(i|=0))<1)return 0|(u=0);if(i>>>0>3){for(u=4+(i+-4&-4)|0,f=e,a=r,n=0,B=i,o=A;c=0|t[f>>2],E=0|t[a>>2],t[o>>2]=c-n-E,E=(0|c)==(0|E)?n:c>>>0<E>>>0&1,c=0|t[f+4>>2],l=0|t[a+4>>2],t[o+4>>2]=c-l-E,l=(0|c)==(0|l)?E:c>>>0<l>>>0&1,c=0|t[f+8>>2],E=0|t[a+8>>2],t[o+8>>2]=c-E-l,E=(0|c)==(0|E)?l:c>>>0<E>>>0&1,c=0|t[f+12>>2],l=0|t[a+12>>2],t[o+12>>2]=c-l-E,n=(0|c)==(0|l)?E:c>>>0<l>>>0&1,!((B=B+-4|0)>>>0<=3);)f=f+16|0,a=a+16|0,o=o+16|0;if(!(i=i+-4-(i+-4&-4)|0))return 0|(E=n);f=e+(u<<2)|0,r=r+(u<<2)|0,A=A+(u<<2)|0}else f=e,n=0;for(;c=0|t[f>>2],E=0|t[r>>2],t[A>>2]=c-n-E,n=(0|c)==(0|E)?n:c>>>0<E>>>0&1,i=i+-1|0;)f=f+4|0,r=r+4|0,A=A+4|0;return 0|n}function Gn(A,e,r){A|=0,e|=0;var i,n=0,a=0,B=0,o=0,u=0,l=0,c=0,E=0,Q=0,w=0,b=0,k=0,s=0,C=0,d=0,I=0,g=0,D=0,M=0,h=0,U=0,Z=0,N=0,F=0,W=0,Y=0,R=0,y=0,V=0;if(i=G,G=G+64|0,r|=0){for(w=0|t[A+8>>2],b=0|t[A+12>>2],k=0|t[A+16>>2],s=0|t[A+20>>2],C=0|t[A+24>>2],d=0|t[A+28>>2],I=0|t[A>>2],g=0|t[A+4>>2];;){D=(0|f[e+1>>0])<<16|(0|f[e>>0])<<24|(0|f[e+2>>0])<<8|0|f[e+3>>0],t[i>>2]=D,B=(D=d+1116352408+((k<<26|k>>>6)^(k<<21|k>>>11)^(k<<7|k>>>25))+(C&~k^s&k)+D|0)+b|0,D=(w&I^(l=g&I)^w&g)+((I<<30|I>>>2)^(I<<19|I>>>13)^(I<<10|I>>>22))+D|0,c=(0|f[e+5>>0])<<16|(0|f[e+4>>0])<<24|(0|f[e+6>>0])<<8|0|f[e+7>>0],t[i+4>>2]=c,a=(c=C+1899447441+(B&k^s&~B)+c+((B<<26|B>>>6)^(B<<21|B>>>11)^(B<<7|B>>>25))|0)+w|0,c=((D<<30|D>>>2)^(D<<19|D>>>13)^(D<<10|D>>>22))+(D&g^l^(u=D&I))+c|0,l=(0|f[e+9>>0])<<16|(0|f[e+8>>0])<<24|(0|f[e+10>>0])<<8|0|f[e+11>>0],t[i+8>>2]=l,Q=(l=s+-1245643825+l+(a&B^k&~a)+((a<<26|a>>>6)^(a<<21|a>>>11)^(a<<7|a>>>25))|0)+g|0,l=((c<<30|c>>>2)^(c<<19|c>>>13)^(c<<10|c>>>22))+(c&I^u^c&D)+l|0,u=(0|f[e+13>>0])<<16|(0|f[e+12>>0])<<24|(0|f[e+14>>0])<<8|0|f[e+15>>0],t[i+12>>2]=u,E=(u=k+-373957723+u+(Q&a^B&~Q)+((Q<<26|Q>>>6)^(Q<<21|Q>>>11)^(Q<<7|Q>>>25))|0)+I|0,u=((l<<30|l>>>2)^(l<<19|l>>>13)^(l<<10|l>>>22))+(l&D^c&D^l&c)+u|0,o=(0|f[e+17>>0])<<16|(0|f[e+16>>0])<<24|(0|f[e+18>>0])<<8|0|f[e+19>>0],t[i+16>>2]=o,D=B+961987163+o+(E&Q^a&~E)+((E<<26|E>>>6)^(E<<21|E>>>11)^(E<<7|E>>>25))+D|0,o=((u<<30|u>>>2)^(u<<19|u>>>13)^(u<<10|u>>>22))+(u&c^l&c^u&l)+(B+961987163+o+(E&Q^a&~E)+((E<<26|E>>>6)^(E<<21|E>>>11)^(E<<7|E>>>25)))|0,B=(0|f[e+21>>0])<<16|(0|f[e+20>>0])<<24|(0|f[e+22>>0])<<8|0|f[e+23>>0],t[i+20>>2]=B,c=a+1508970993+B+(D&E^Q&~D)+((D<<26|D>>>6)^(D<<21|D>>>11)^(D<<7|D>>>25))+c|0,B=((o<<30|o>>>2)^(o<<19|o>>>13)^(o<<10|o>>>22))+(o&l^u&l^o&u)+(a+1508970993+B+(D&E^Q&~D)+((D<<26|D>>>6)^(D<<21|D>>>11)^(D<<7|D>>>25)))|0,a=(0|f[e+25>>0])<<16|(0|f[e+24>>0])<<24|(0|f[e+26>>0])<<8|0|f[e+27>>0],t[i+24>>2]=a,l=Q+-1841331548+a+(c&D^E&~c)+((c<<26|c>>>6)^(c<<21|c>>>11)^(c<<7|c>>>25))+l|0,a=((B<<30|B>>>2)^(B<<19|B>>>13)^(B<<10|B>>>22))+(B&u^o&u^B&o)+(Q+-1841331548+a+(c&D^E&~c)+((c<<26|c>>>6)^(c<<21|c>>>11)^(c<<7|c>>>25)))|0,Q=(0|f[e+29>>0])<<16|(0|f[e+28>>0])<<24|(0|f[e+30>>0])<<8|0|f[e+31>>0],t[i+28>>2]=Q,u=E+-1424204075+Q+(l&c^D&~l)+((l<<26|l>>>6)^(l<<21|l>>>11)^(l<<7|l>>>25))+u|0,Q=((a<<30|a>>>2)^(a<<19|a>>>13)^(a<<10|a>>>22))+(a&o^B&o^a&B)+(E+-1424204075+Q+(l&c^D&~l)+((l<<26|l>>>6)^(l<<21|l>>>11)^(l<<7|l>>>25)))|0,E=(0|f[e+33>>0])<<16|(0|f[e+32>>0])<<24|(0|f[e+34>>0])<<8|0|f[e+35>>0],t[i+32>>2]=E,o=D+-670586216+E+(u&l^c&~u)+((u<<26|u>>>6)^(u<<21|u>>>11)^(u<<7|u>>>25))+o|0,E=((Q<<30|Q>>>2)^(Q<<19|Q>>>13)^(Q<<10|Q>>>22))+(Q&B^a&B^Q&a)+(D+-670586216+E+(u&l^c&~u)+((u<<26|u>>>6)^(u<<21|u>>>11)^(u<<7|u>>>25)))|0,D=(0|f[e+37>>0])<<16|(0|f[e+36>>0])<<24|(0|f[e+38>>0])<<8|0|f[e+39>>0],t[i+36>>2]=D,B=D+310598401+c+(o&u^l&~o)+((o<<26|o>>>6)^(o<<21|o>>>11)^(o<<7|o>>>25))+B|0,c=((E<<30|E>>>2)^(E<<19|E>>>13)^(E<<10|E>>>22))+(E&a^Q&a^E&Q)+(D+310598401+c+(o&u^l&~o)+((o<<26|o>>>6)^(o<<21|o>>>11)^(o<<7|o>>>25)))|0,D=(0|f[e+41>>0])<<16|(0|f[e+40>>0])<<24|(0|f[e+42>>0])<<8|0|f[e+43>>0],t[i+40>>2]=D,a=D+607225278+l+(B&o^u&~B)+((B<<26|B>>>6)^(B<<21|B>>>11)^(B<<7|B>>>25))+a|0,l=((c<<30|c>>>2)^(c<<19|c>>>13)^(c<<10|c>>>22))+(c&Q^E&Q^c&E)+(D+607225278+l+(B&o^u&~B)+((B<<26|B>>>6)^(B<<21|B>>>11)^(B<<7|B>>>25)))|0,D=(0|f[e+45>>0])<<16|(0|f[e+44>>0])<<24|(0|f[e+46>>0])<<8|0|f[e+47>>0],t[i+44>>2]=D,Q=D+1426881987+u+(a&B^o&~a)+((a<<26|a>>>6)^(a<<21|a>>>11)^(a<<7|a>>>25))+Q|0,u=((l<<30|l>>>2)^(l<<19|l>>>13)^(l<<10|l>>>22))+(l&E^c&E^l&c)+(D+1426881987+u+(a&B^o&~a)+((a<<26|a>>>6)^(a<<21|a>>>11)^(a<<7|a>>>25)))|0,D=(0|f[e+49>>0])<<16|(0|f[e+48>>0])<<24|(0|f[e+50>>0])<<8|0|f[e+51>>0],t[i+48>>2]=D,E=D+1925078388+o+(Q&a^B&~Q)+((Q<<26|Q>>>6)^(Q<<21|Q>>>11)^(Q<<7|Q>>>25))+E|0,o=((u<<30|u>>>2)^(u<<19|u>>>13)^(u<<10|u>>>22))+(u&c^l&c^u&l)+(D+1925078388+o+(Q&a^B&~Q)+((Q<<26|Q>>>6)^(Q<<21|Q>>>11)^(Q<<7|Q>>>25)))|0,D=(0|f[e+53>>0])<<16|(0|f[e+52>>0])<<24|(0|f[e+54>>0])<<8|0|f[e+55>>0],t[i+52>>2]=D,c=D+-2132889090+B+(E&Q^a&~E)+((E<<26|E>>>6)^(E<<21|E>>>11)^(E<<7|E>>>25))+c|0,B=((o<<30|o>>>2)^(o<<19|o>>>13)^(o<<10|o>>>22))+(o&l^u&l^o&u)+(D+-2132889090+B+(E&Q^a&~E)+((E<<26|E>>>6)^(E<<21|E>>>11)^(E<<7|E>>>25)))|0,D=(0|f[e+57>>0])<<16|(0|f[e+56>>0])<<24|(0|f[e+58>>0])<<8|0|f[e+59>>0],t[i+56>>2]=D,l=D+-1680079193+a+(c&E^Q&~c)+((c<<26|c>>>6)^(c<<21|c>>>11)^(c<<7|c>>>25))+l|0,a=((B<<30|B>>>2)^(B<<19|B>>>13)^(B<<10|B>>>22))+(B&u^o&u^B&o)+(D+-1680079193+a+(c&E^Q&~c)+((c<<26|c>>>6)^(c<<21|c>>>11)^(c<<7|c>>>25)))|0,D=(0|f[e+61>>0])<<16|(0|f[e+60>>0])<<24|(0|f[e+62>>0])<<8|0|f[e+63>>0],t[i+60>>2]=D,n=((a<<30|a>>>2)^(a<<19|a>>>13)^(a<<10|a>>>22))+(a&(B^o)^B&o)+(D+-1046744716+Q+(l&c^E&~l)+((l<<26|l>>>6)^(l<<21|l>>>11)^(l<<7|l>>>25)))|0,u=D+-1046744716+Q+(l&c^E&~l)+((l<<26|l>>>6)^(l<<21|l>>>11)^(l<<7|l>>>25))+u|0,Q=16;do{y=0|t[i+((9&(U=1|Q))<<2)>>2],R=0|t[i+((Q+14&14)<<2)>>2],R=(0|t[i+((W=8&Q)<<2)>>2])+(0|t[i+((Q+9&9)<<2)>>2])+((y<<14|y>>>18)^y>>>3^(y<<25|y>>>7))+((R<<13|R>>>19)^R>>>10^(R<<15|R>>>17))|0,t[i+(W<<2)>>2]=R,F=(R=((u<<26|u>>>6)^(u<<21|u>>>11)^(u<<7|u>>>25))+E+(c&~u^l&u)+(0|t[50576+(Q<<2)>>2])+R|0)+o|0,R=((n<<30|n>>>2)^(n<<19|n>>>13)^(n<<10|n>>>22))+((n^a)&B^(M=n&a))+R|0,N=0|t[i+((U+1&10)<<2)>>2],Z=0|t[i+((U+14&15)<<2)>>2],Z=(0|t[i+((U+9&10)<<2)>>2])+y+((N<<14|N>>>18)^N>>>3^(N<<25|N>>>7))+((Z<<13|Z>>>19)^Z>>>10^(Z<<15|Z>>>17))|0,t[i+((9&U)<<2)>>2]=Z,N=(Z=(0|t[50576+(U<<2)>>2])+c+(F&u^l&~F)+((F<<26|F>>>6)^(F<<21|F>>>11)^(F<<7|F>>>25))+Z|0)+B|0,Z=((R<<30|R>>>2)^(R<<19|R>>>13)^(R<<10|R>>>22))+(R&a^M^(U=R&n))+Z|0,M=2|Q,y=0|t[i+((3|W)<<2)>>2],D=0|t[i+((M+14&8)<<2)>>2],D=(0|t[i+((10&M)<<2)>>2])+(0|t[i+((M+9&11)<<2)>>2])+((y<<14|y>>>18)^y>>>3^(y<<25|y>>>7))+((D<<13|D>>>19)^D>>>10^(D<<15|D>>>17))|0,t[i+((10&M)<<2)>>2]=D,M=(D=(0|t[50576+(M<<2)>>2])+l+D+(N&F^u&~N)+((N<<26|N>>>6)^(N<<21|N>>>11)^(N<<7|N>>>25))|0)+a|0,D=((Z<<30|Z>>>2)^(Z<<19|Z>>>13)^(Z<<10|Z>>>22))+(Z&n^U^Z&R)+D|0,y=0|t[i+(((U=3|Q)+1&12)<<2)>>2],h=0|t[i+((U+14&9)<<2)>>2],h=(0|t[i+((11&U)<<2)>>2])+(0|t[i+((U+9&12)<<2)>>2])+((y<<14|y>>>18)^y>>>3^(y<<25|y>>>7))+((h<<13|h>>>19)^h>>>10^(h<<15|h>>>17))|0,t[i+((11&U)<<2)>>2]=h,U=(h=(0|t[50576+(U<<2)>>2])+u+h+(M&N^F&~M)+((M<<26|M>>>6)^(M<<21|M>>>11)^(M<<7|M>>>25))|0)+n|0,h=((D<<30|D>>>2)^(D<<19|D>>>13)^(D<<10|D>>>22))+(D&R^Z&R^D&Z)+h|0,y=4|Q,V=0|t[i+((5|W)<<2)>>2],Y=0|t[i+((y+14&10)<<2)>>2],Y=(0|t[i+((12&y)<<2)>>2])+(0|t[i+((y+9&13)<<2)>>2])+((V<<14|V>>>18)^V>>>3^(V<<25|V>>>7))+((Y<<13|Y>>>19)^Y>>>10^(Y<<15|Y>>>17))|0,t[i+((12&y)<<2)>>2]=Y,E=(Y=(0|t[50576+(y<<2)>>2])+F+Y+(U&M^N&~U)+((U<<26|U>>>6)^(U<<21|U>>>11)^(U<<7|U>>>25))|0)+R|0,o=((h<<30|h>>>2)^(h<<19|h>>>13)^(h<<10|h>>>22))+(h&Z^D&Z^h&D)+Y|0,R=0|t[i+(((Y=5|Q)+1&14)<<2)>>2],F=0|t[i+((Y+14&11)<<2)>>2],F=(0|t[i+((13&Y)<<2)>>2])+(0|t[i+((Y+9&14)<<2)>>2])+((R<<14|R>>>18)^R>>>3^(R<<25|R>>>7))+((F<<13|F>>>19)^F>>>10^(F<<15|F>>>17))|0,t[i+((13&Y)<<2)>>2]=F,c=(F=(0|t[50576+(Y<<2)>>2])+N+F+(E&U^M&~E)+((E<<26|E>>>6)^(E<<21|E>>>11)^(E<<7|E>>>25))|0)+Z|0,B=((o<<30|o>>>2)^(o<<19|o>>>13)^(o<<10|o>>>22))+(o&D^h&D^(N=o&h))+F|0,F=6|Q,W=0|t[i+((7|W)<<2)>>2],Z=0|t[i+((F+14&12)<<2)>>2],Z=(0|t[i+((14&F)<<2)>>2])+(0|t[i+((F+9&15)<<2)>>2])+((W<<14|W>>>18)^W>>>3^(W<<25|W>>>7))+((Z<<13|Z>>>19)^Z>>>10^(Z<<15|Z>>>17))|0,t[i+((14&F)<<2)>>2]=Z,l=(Z=(0|t[50576+(F<<2)>>2])+M+Z+(c&E^U&~c)+((c<<26|c>>>6)^(c<<21|c>>>11)^(c<<7|c>>>25))|0)+D|0,a=((B<<30|B>>>2)^(B<<19|B>>>13)^(B<<10|B>>>22))+(B&h^N^(M=B&o))+Z|0,N=0|t[i+(((Z=7|Q)+1&8)<<2)>>2],D=0|t[i+((Z+14&13)<<2)>>2],D=(0|t[i+((15&Z)<<2)>>2])+(0|t[i+((Z+9&8)<<2)>>2])+((N<<14|N>>>18)^N>>>3^(N<<25|N>>>7))+((D<<13|D>>>19)^D>>>10^(D<<15|D>>>17))|0,t[i+((15&Z)<<2)>>2]=D,u=(D=(0|t[50576+(Z<<2)>>2])+U+D+(l&c^E&~l)+((l<<26|l>>>6)^(l<<21|l>>>11)^(l<<7|l>>>25))|0)+h|0,n=((a<<30|a>>>2)^(a<<19|a>>>13)^(a<<10|a>>>22))+(a&(B^o)^M)+D|0,Q=Q+8|0}while(Q>>>0<64);if(r=r+-1|0,I=n+I|0,t[A>>2]=I,g=a+g|0,t[A+4>>2]=g,w=B+w|0,t[A+8>>2]=w,b=o+b|0,t[A+12>>2]=b,k=u+k|0,t[A+16>>2]=k,s=l+s|0,t[A+20>>2]=s,C=c+C|0,t[A+24>>2]=C,d=E+d|0,t[A+28>>2]=d,!r)break;e=e+64|0}G=i}else G=i}function Zn(A,e){A|=0;var r=0,n=0;switch(r=0|t[(e|=0)+104>>2],i[e+40+r>>0]=-128,(r+1|0)>>>0>56?(Df(e+40+(r+1)|0,0,63-r|0),Gn(e,e+40|0,1),r=0):r=r+1|0,Df(e+40+r|0,0,56-r|0),r=0|t[e+36>>2],i[e+96>>0]=r>>>24,i[e+96+1>>0]=r>>>16,i[e+96+2>>0]=r>>>8,i[e+96+3>>0]=r,r=0|t[e+32>>2],i[e+100>>0]=r>>>24,i[e+100+1>>0]=r>>>16,i[e+100+2>>0]=r>>>8,i[e+100+3>>0]=r,Gn(e,e+40|0,1),t[e+104>>2]=0,$r(e+40|0,64),0|(r=0|t[e+108>>2])){case 28:return r=0|t[e>>2],i[A>>0]=r>>>24,i[A+1>>0]=r>>>16,i[A+2>>0]=r>>>8,i[A+3>>0]=r,r=0|t[e+4>>2],i[A+4>>0]=r>>>24,i[A+5>>0]=r>>>16,i[A+6>>0]=r>>>8,i[A+7>>0]=r,r=0|t[e+8>>2],i[A+8>>0]=r>>>24,i[A+9>>0]=r>>>16,i[A+10>>0]=r>>>8,i[A+11>>0]=r,r=0|t[e+12>>2],i[A+12>>0]=r>>>24,i[A+13>>0]=r>>>16,i[A+14>>0]=r>>>8,i[A+15>>0]=r,r=0|t[e+16>>2],i[A+16>>0]=r>>>24,i[A+17>>0]=r>>>16,i[A+18>>0]=r>>>8,i[A+19>>0]=r,r=0|t[e+20>>2],i[A+20>>0]=r>>>24,i[A+21>>0]=r>>>16,i[A+22>>0]=r>>>8,i[A+23>>0]=r,e=0|t[e+24>>2],i[A+24>>0]=e>>>24,i[A+25>>0]=e>>>16,i[A+26>>0]=e>>>8,i[A+27>>0]=e,0|(e=1);case 32:return r=0|t[e>>2],i[A>>0]=r>>>24,i[A+1>>0]=r>>>16,i[A+2>>0]=r>>>8,i[A+3>>0]=r,r=0|t[e+4>>2],i[A+4>>0]=r>>>24,i[A+5>>0]=r>>>16,i[A+6>>0]=r>>>8,i[A+7>>0]=r,r=0|t[e+8>>2],i[A+8>>0]=r>>>24,i[A+9>>0]=r>>>16,i[A+10>>0]=r>>>8,i[A+11>>0]=r,r=0|t[e+12>>2],i[A+12>>0]=r>>>24,i[A+13>>0]=r>>>16,i[A+14>>0]=r>>>8,i[A+15>>0]=r,r=0|t[e+16>>2],i[A+16>>0]=r>>>24,i[A+17>>0]=r>>>16,i[A+18>>0]=r>>>8,i[A+19>>0]=r,r=0|t[e+20>>2],i[A+20>>0]=r>>>24,i[A+21>>0]=r>>>16,i[A+22>>0]=r>>>8,i[A+23>>0]=r,r=0|t[e+24>>2],i[A+24>>0]=r>>>24,i[A+25>>0]=r>>>16,i[A+26>>0]=r>>>8,i[A+27>>0]=r,e=0|t[e+28>>2],i[A+28>>0]=e>>>24,i[A+29>>0]=e>>>16,i[A+30>>0]=e>>>8,i[A+31>>0]=e,0|(e=1);default:if(r>>>0>32)return 0|(e=0);if(r>>>0<=3)return 0|(e=1);for(r=A,A=0;;){if(n=0|t[e+(A<<2)>>2],i[r>>0]=n>>>24,i[r+1>>0]=n>>>16,i[r+2>>0]=n>>>8,i[r+3>>0]=n,(A=A+1|0)>>>0>=(0|t[e+108>>2])>>>2>>>0){r=1;break}r=r+4|0}return 0|r}return 0}function Nn(A,e,r){e|=0;var n=0,f=0,a=0;if(!(r|=0))return 1;f=0|t[(A|=0)+32>>2],n=0|t[A+36>>2],(f+(r<<3)|0)>>>0<f>>>0&&(t[A+36>>2]=n+1,n=n+1|0),t[A+36>>2]=n+(r>>>29),t[A+32>>2]=f+(r<<3),a=0|t[A+104>>2];do{if(a){if((a+r|r)>>>0>63){If(A+40+a|0,0|e,64-a|0),Gn(A,A+40|0,1),f=(n=A+40|0)+68|0;do{i[n>>0]=0,n=n+1|0}while((0|n)<(0|f));n=e+(64-a)|0,r=r-(64-a)|0;break}return If(A+40+a|0,0|e,0|r),t[A+104>>2]=(0|t[A+104>>2])+r,1}n=e}while(0);return a=-64&r,(f=r>>>6)&&(Gn(A,n,f),n=n+a|0,r=r-a|0),r?(t[A+104>>2]=r,If(A+40|0,0|n,0|r),1):1}function Fn(A){(A|=0)&&(Xn(0|t[A+4>>2],82796,0),Xn(A,82796,0))}function Wn(){var A,e=0;return 0|(A=0|Sn(20,82796,0))&&(e=0|Sn(16,82796,0),t[A+4>>2]=e,0|e)?(t[A+16>>2]=0,t[A+12>>2]=4,0|(e=A)):(Xn(A,82796,0),0|(e=0))}function Yn(A,e){var r,i,n=0;return 0==(0|(A|=0))|(0|(e|=0))<0||(0|(n=0|t[A>>2]))<=(0|e)?0|(A=0):(r=0|t[A+4>>2],i=0|t[r+(e<<2)>>2],(n+-1|0)==(0|e)?n=n+-1|0:(gf(r+(e<<2)|0,r+(e+1<<2)|0,(n-e<<2)-4|0),n=(0|t[A>>2])-1|0),t[A>>2]=n,0|(A=i))}function Rn(A,e){A|=0,e|=0;var r,i=0,n=0,f=0;r=G,G=G+16|0,t[r>>2]=e;A:do{if(A){if(!(i=0|t[A+16>>2])){if((0|(f=0|t[A>>2]))<=0){i=-1;break}for(n=0|t[A+4>>2],i=0;;){if((0|t[n+(i<<2)>>2])==(0|e))break A;if((0|(i=i+1|0))>=(0|f)){i=-1;break A}}}0|t[A+8>>2]||(pt(0|t[A+4>>2],0|t[A>>2],4,i),t[A+8>>2]=1),i=0!=(0|e)&&0!=(0|(n=0|_r(r,0|t[A+4>>2],0|t[A>>2],4,0|t[A+16>>2],2)))?n-(0|t[A+4>>2])>>2:-1}else i=-1}while(0);return G=r,0|i}function yn(A,e){e|=0;var r,i=0,n=0,f=0;if(r=0|t[(A|=0)>>2],!A)return 0|(f=0);if((0|r)<0|2147483647==(0|r))return 0|(f=0);if((i=0|t[A+12>>2])>>>0<=(r+1|0)>>>0){if(i<<1>>>0<i>>>0|i<<1>>>0>1073741823)return 0|(f=0);if(!(n=0|Tn(0|t[A+4>>2],i<<3,82796,0)))return 0|(f=0);t[A+4>>2]=n,t[A+12>>2]=i<<1,(0|(i=0|t[A>>2]))>(0|r)?(gf(n+(r+1<<2)|0,n+(r<<2)|0,i-r<<2|0),t[(0|t[A+4>>2])+(r<<2)>>2]=e,i=0|t[A>>2]):f=8}else n=0|t[A+4>>2],i=r,f=8;return 8==(0|f)&&(t[n+(i<<2)>>2]=e),f=i+1|0,t[A>>2]=f,t[A+8>>2]=0,0|f}function Vn(A){var e,r;return(A|=0)?(0|(e=0|t[A>>2]))<1?0|(A=0):(r=0|t[(0|t[A+4>>2])+(e+-1<<2)>>2],t[A>>2]=e+-1,0|(A=r)):0|(A=0)}function mn(A,e){e|=0;var r=0,i=0,n=0;if(A|=0){if((0|(r=0|t[A>>2]))>0){n=0;do{(i=0|t[(0|t[A+4>>2])+(n<<2)>>2])&&(NB[31&e](i),r=0|t[A>>2]),n=n+1|0}while((0|n)<(0|r))}Xn(0|t[A+4>>2],82796,0),Xn(A,82796,0)}}function Hn(A){return 0|(A=(A|=0)?0|t[A>>2]:-1)}function vn(A,e){return 0==(0|(A|=0))|(0|(e|=0))<0||(0|t[A>>2])<=(0|e)?0|(A=0):0|(A=0|t[(0|t[A+4>>2])+(e<<2)>>2])}function Jn(A,e,r){return r|=0,0==(0|(A|=0))|(0|(e|=0))<0||(0|t[A>>2])<=(0|e)?0|(A=0):(t[(0|t[A+4>>2])+(e<<2)>>2]=r,0|(A=0|t[(0|t[A+4>>2])+(e<<2)>>2]))}function Xn(A,e,r){af(A|=0)}function Tn(A,e,r,i){A|=0,e|=0,r|=0;do{if(0|A){if(0|e){r=0|Bf(A,e);break}af(A),r=0;break}e?(0|t[13247]&&(t[13247]=0),r=0|ff(e)):r=0}while(0);return 0|r}function Ln(A,e,r){return 0,0,(A|=0)?(0|t[13247]&&(t[13247]=0),0|(A=0|ff(A))):0|(A=0)}function Sn(A,e,r){return e|=0,0,(A|=0)?(0|t[13247]&&(t[13247]=0),(e=0|ff(A))?(Df(0|e,0,0|A),0|(A=e)):0|(A=0)):0|(A=0)}function pn(A,e,r,i){if(e|=0,0,0,A|=0)return e?($r(A,e),void af(A)):void af(A)}function zn(A,e){return A|=0,e|=0,0|t[20540]?(Xe(15,116,70,0,0),0|(A=0)):(e=0!=(0|m(82164,3)))&0!=(0|t[20542])&&(e=0!=(0|m(82172,4)))&0!=(0|t[20544])?(e=0|v(52992))||0==(0|(e=0|Sn(8,82796)))||0!=(0|J(52992,e))?e?(0==(1&A|0)&!0||(t[e>>2]=1),0==(2&A|0)&!0||(t[e+4>>2]=1),0|(A=1)):0|(A=0):(Xn(e),0|(A=0)):0|(A=0)}function Kn(A,e,r){return A|=0,e|=0,r|=0,0|t[20540]?0==(262144&A|0)&!0?(Xe(15,116,70,0,0),0|(A=0)):0|(A=0):(e=0!=(0|m(82164,3)))&0!=(0|t[20542])?0==(262144&A|0)&!0&&!((e=0!=(0|m(82172,4)))&0!=(0|t[20544]))?0|(A=0):(0==(1&A|0)&!0||(e=0!=(0|m(82192,6)))&0!=(0|t[20549]))&&(0==(2&A|0)&!0||(e=0!=(0|m(82192,7)))&0!=(0|t[20550]))&&(0==(16&A|0)&!0||(e=0!=(0|m(82204,8)))&0!=(0|t[20552]))&&(0==(4&A|0)&!0||(e=0!=(0|m(82204,9)))&0!=(0|t[20553]))&&(0==(32&A|0)&!0||(e=0!=(0|m(82216,8)))&0!=(0|t[20552]))&&(0==(8&A|0)&!0||(e=0!=(0|m(82216,10)))&0!=(0|t[20555]))&&(0==(128&A|0)&!0||(e=0!=(0|m(82224,11)))&0!=(0|t[20557]))&&(0==(64&A|0)&!0||(R(0|t[20546]),e=r?0|t[r>>2]:0,t[20558]=e,e=0!=(0|m(82224,12)),r=0|t[20559],y(0|t[20546]),e&0!=(0|r)))&&(0==(256&A|0)&!0||(A=0!=(0|m(82240,13)))&0!=(0|t[20561]))?0|(A=1):0|(A=0):0|(A=0)}function jn(A){A|=0;var e=0,r=0,i=0,n=0;if(A=0|t[20562]){if(0|t[20563]){for(e=0;;){if(Xn(0|t[A+(12*e|0)>>2]),n=A+(12*e|0)+8|0,A=0|t[(i=A+(12*e|0)+4|0)>>2],0|t[n>>2]){r=0;do{Xn(0|t[A+(r<<3)>>2]),Xn(0|t[(0|t[i>>2])+(r<<3)+4>>2]),r=r+1|0,A=0|t[i>>2]}while(r>>>0<(0|t[n>>2])>>>0)}if(Xn(A),(e=e+1|0)>>>0>=(0|t[20563])>>>0)break;A=0|t[20562]}A=0|t[20562]}Xn(A),t[20562]=0,t[20563]=0}}function xn(A,e){var r;return r=G,G=G+16|0,0==(0|(A|=0))|0==(0|(e|=0))?(G=r,0|(e=0)):(t[r+4>>2]=0,t[r>>2]=e,e=0|ye(0|t[A+8>>2],r),G=r,0|e)}function On(A,e){var r;return r=G,G=G+16|0,0==(0|(A|=0))|0==(0|(e|=0))?(G=r,0|(e=0)):(t[r+4>>2]=0,t[r>>2]=e,(A=0|ye(0|t[A+8>>2],r))?(e=0|t[A+8>>2],G=r,0|e):(G=r,0|(e=0)))}function Pn(A,e,r){A|=0,r|=0;var i=0;return i=0|t[(e|=0)+8>>2],t[r>>2]=t[e>>2],0|yn(i,r)?(A=0|We(0|t[A+8>>2],r))?(function(A,e){e|=0;var r,i,n=0,f=0,a=0;if((0|(r=0|t[(A|=0)>>2]))<=0)return 0|(a=0);for(i=0|t[A+4>>2],n=0;(0|t[(f=i+(n<<2)|0)>>2])!=(0|e);)if((0|r)<=(0|(n=n+1|0))){n=0,a=8;break}8==(0|a)||((r+-1|0)!=(0|n)&&(gf(0|f,i+(n+1<<2)|0,(r-n<<2)-4|0),n=(0|t[A>>2])-1|0),t[A>>2]=n,a=e)}(i,A),Xn(0|t[A+4>>2]),Xn(0|t[A+8>>2]),Xn(A),0|(i=1)):0|(i=1):0|(i=0)}function _n(A,e,r){A|=0,e|=0,r|=0;var i,n=0,f=0;i=G,G=G+16|0;do{if(r){if(!A){n=0|wt(r);break}if(e){if(t[i+4>>2]=r,t[i>>2]=e,0|(f=0|ye(0|t[A+8>>2],i))){n=0|t[f+8>>2];break}if(0==(0|Wt(e,80650))&&0!=(0|(n=0|wt(r))))break;n=A+8|0,f=i+4|0,A=i}else n=A+8|0,f=i+4|0,A=i;t[A>>2]=80850,t[f>>2]=r,n=(n=0|ye(0|t[n>>2],i))?0|t[n+8>>2]:0}else n=0}while(0);return G=i,0|n}function qn(A){A|=0;var e=0;do{if(A){if(0==(0|t[A+8>>2])&&(e=0|Ze(20,34),t[A+8>>2]=e,0==(0|e))){A=0;break}A=1}else A=0}while(0);return 0|A}function $n(A){var e=0;(A|=0)&&(e=0|t[A+8>>2])&&(ve(e,0),me(e=0|t[A+8>>2],3,e),Ve(0|t[A+8>>2],21),Fe(0|t[A+8>>2]))}function At(A,e){A|=0,e|=0;var r,i=0,n=0,f=0;r=0|Wn();do{if(r){if(!(i=0|Ln(12))){Fn(r),i=0;break}if(f=0|Ln(n=1+(0|mt(e))|0),t[i>>2]=f,0|f&&(If(0|f,0|e,0|n),t[i+4>>2]=0,t[i+8>>2]=r,0|We(0|t[A+8>>2],i)&&oi(),(0|Je(0|t[A+8>>2]))<=0))return 0|(e=i);Fn(r),Xn(0|t[i>>2])}else Fn(0),i=0}while(0);return Xn(i),0|(e=0)}function et(A,e){A|=0,e|=0;do{if(A){if(e){A=0|On(A,e);break}Xe(14,108,107,0,0),A=0;break}Xe(14,108,105,0,0),A=0}while(0);return 0|A}function rt(A,e,r){var i,n;return n=G,G=G+16|0,0|(i=0|_n(A|=0,e|=0,r|=0))?(G=n,0|(r=i)):A?(Xe(14,109,108,0,0),t[n>>2]=80683,t[n+4>>2]=e,t[n+8>>2]=80690,t[n+12>>2]=r,Ke(4,n),G=n,0|(r=0)):(Xe(14,109,106,0,0),G=n,0|(r=0))}function it(A){return(A|=0)||(A=52996),0|(A=0|IB[31&t[A+4>>2]](A))?0|A:(Xe(14,111,65,0,0),0|(A=0))}function nt(A){(A|=0)&&IB[31&t[12+(0|t[A>>2])>>2]](A)}function tt(A,e,r){return e|=0,r|=0,(A|=0)?0|(e=0|DB[31&t[36+(0|t[A>>2])>>2]](A,e,r)):(Xe(14,113,105,0,0),0|(e=0))}function ft(A,e,r){r|=0;var i,n,f=0,a=0,B=0,o=0,u=0,l=0,c=0;if(n=G,G=G+80|0,!(A|=0))return G=n,0|(r=1);if(0!=(0|(e|=0))&&0==(32&r|0)|0!=(0|(f=0|rt(A,0,e)))||(f=0|rt(A,0,80697)),!f)return Se(),G=n,0|(r=1);if(!(i=0|et(A,f)))return G=n,0|(r=0);if((0|Hn(i))<=0)return G=n,0|(r=1);for(c=0;;){u=0|vn(i,c),l=0|t[u+4>>2],u=0|t[u+8>>2],a=(f=0|ef(l,46))?f-l|0:0|mt(l);A:do{if((0|Hn(0|t[20565]))>0){for(e=0;f=0|vn(0|t[20565],e),e=e+1|0,0|Af(0|t[f+4>>2],l,a);)if((0|e)>=(0|Hn(0|t[20565]))){f=0;break A}}else f=0}while(0);e=0==(0|f);do{if(0==(8&r|0)&e){if((f=0|rt(A,u,80710))||(Se(),f=l),a=0|vr(0,f,0,0))if(e=0|Xr(a,80715)){if(e=0|at(a,l,e,0|Xr(a,80728))){f=e,o=28;break}e=0}else e=112;else e=110;Hr(a),Xe(14,117,e,0,0),t[n>>2]=80743,t[n+4>>2]=l,t[n+8>>2]=80751,t[n+12>>2]=f,Ke(4,n),o=26}else o=e?26:28}while(0);A:do{if(26==(0|o))4&r||(Xe(14,118,113,0,0),t[n+16>>2]=80743,t[n+16+4>>2]=l,Ke(2,n+16|0)),o=43;else if(28==(0|o)){if(o=0,0|(B=0|Ln(20))){if(t[B>>2]=f,t[B+4>>2]=0|kt(l,82796,0),a=0|kt(u,82796,0),t[B+8>>2]=a,t[B+16>>2]=0,!(0==(0|a)|0==(0|t[B+4>>2]))){(e=0|t[f+8>>2])?(0|gB[63&e](B,A))<1?e=1:(a=1,o=32):(a=0,o=32);do{if(32==(0|o)){if(o=0,!(e=0|t[20566])&&(e=0|Wn(),t[20566]=e,!e)){Xe(14,115,65,0,0),e=a;break}if(0|yn(e,B)){t[(l=f+16|0)>>2]=1+(0|t[l>>2]);break A}Xe(14,115,65,0,0),e=a;break}}while(0);0!=(0|e)&0!=(0|(f=0|t[f+12>>2]))&&NB[31&f](B)}Xn(0|t[B+4>>2]),Xn(0|t[B+8>>2]),Xn(B)}4&r||(Xe(14,118,109,0,0),t[n+24>>2]=-1,Ge(n+56|0,13,80759,n+24|0),t[n+32>>2]=80743,t[n+32+4>>2]=l,t[n+32+8>>2]=80764,t[n+32+12>>2]=u,t[n+32+16>>2]=80773,t[n+32+20>>2]=n+56,Ke(6,n+32|0)),o=43}}while(0);if(43==(0|o)&&0==(1&r|0)){f=-1,o=45;break}if((0|(c=c+1|0))>=(0|Hn(i))){f=1,o=45;break}}return 45==(0|o)?(G=n,0|f):0}function at(A,e,r,i){A|=0,e|=0,r|=0,i|=0;var n=0;return 0==(0|t[20565])&&(n=0|Wn(),t[20565]=n,0==(0|n))?0|(e=0):(n=0|Sn(24,82796))?(t[n>>2]=A,e=0|kt(e,82796,0),t[n+4>>2]=e,t[n+8>>2]=r,t[n+12>>2]=i,e?0|yn(0|t[20565],n)?0|(e=n):(Xn(0|t[n+4>>2]),Xn(n),0|(e=0)):(Xn(n),0|(e=0))):0|(e=0)}function Bt(){var A=0,e=0;return 0|(A=0|wt(80784))?0|(e=0|kt(A,82796,0)):(e=0|Ln(A=13+(0|mt(80519))|0))?(dt(e,80519,A),It(e,80797,A),It(e,80799,A),0|e):0|(e=0)}function ot(){var A=0,e=0,r=0;if(r=(0|Hn(0|t[20566]))>0,A=0|t[20566],!r)return Fn(r=A),void(t[20566]=0);do{0|(r=0|Vn(A))&&(A=0|t[r>>2],(e=0|t[A+12>>2])&&(NB[31&e](r),A=0|t[r>>2]),t[(e=A+16|0)>>2]=(0|t[e>>2])-1,Xn(0|t[r+4>>2]),Xn(0|t[r+8>>2]),Xn(r)),r=(0|Hn(0|t[20566]))>0,A=0|t[20566]}while(r);Fn(A),t[20566]=0}function ut(A,e,r){return 0!=(0|at(0,A|=0,e|=0,r|=0))|0}function lt(A){return 0|t[(A|=0)+8>>2]}function ct(A,e,r,n,t){e|=0,r|=0,n|=0,t|=0;var a=0,B=0;if(!(A|=0))return Xe(14,119,115,0,0),0|(e=0);if(!r){for(a=A;;){if((0|(B=0|qt(a,e)))!=(0|a)&&0!=(0|i[a>>0])?(r=B||a+(0|mt(a))|0,r=0|DB[31&n](a,1-a+(r+-1)|0,t)):r=0|DB[31&n](0,0,t),(0|r)<1){a=27;break}if(!B){r=1,a=27;break}a=B+1|0}if(27==(0|a))return 0|r}for(;;){r=0|i[A>>0];A:do{if(r<<24>>24)for(;;){if(!(0|Yt(255&r)))break A;if(!((r=0|i[(A=A+1|0)>>0])<<24>>24)){r=0;break}}else r=0}while(0);if((0|(a=0|qt(A,e)))==(0|A)|r<<24>>24==0)r=0|DB[31&n](0,0,t);else{r=a||A+(0|mt(A))|0;do{r=r+-1|0}while(0!=(0|Yt(0|f[r>>0])));r=0|DB[31&n](A,1-A+r|0,t)}if((0|r)<1){a=27;break}if(!a){r=1,a=27;break}A=a+1|0}return 27==(0|a)?0|r:0}function Et(A,e,r){A|=0,e|=0,r|=0;var B,o,u,l=0,c=0,E=0,Q=0,w=0,b=0,k=0,s=0,C=0,d=0,I=0,g=0;u=G,G=G+48|0,t[u+16>>2]=0,o=0|t[A+8>>2],B=0|Mt();A:do{if(B){if(l=0|kt(80850,82796,0),t[u+16>>2]=l,!l){Xe(14,121,65,0,0),E=0,l=0,c=0;break}if(!(0|qn(A))){Xe(14,121,65,0,0),E=0,l=0,c=0;break}if(!(c=0|At(A,l))){Xe(14,121,103,0,0),E=0,l=0,c=0;break}e:do{if(0|Ut(B,512)){l=0;r:for(;;){for(Q=0,E=0;;){if(b=(0|t[B+4>>2])+E|0,i[b>>0]=0,de(e,b,511),i[b+511>>0]=0,!((k=0|mt(b))|Q)){g=113;break r}i:do{if((0|k)>0)for(Q=k;;){switch(0|i[b+(w=Q+-1|0)>>0]){case 10:case 13:break;default:w=Q;break i}if(!((0|Q)>1))break i;Q=w}else w=k}while(0);if(0!=(0|k)&(0|w)==(0|k)?Q=1:(i[b+w>>0]=0,Q=0,l=l+1|0),(0|(E=w+E|0))>0&&(I=(0|t[B+4>>2])+(E+-1)|0,d=0|t[A+4>>2],0!=(32&n[d+(f[I>>0]<<1)>>1]))?(0|E)>=2&&0!=(32&n[d+(f[I+-1>>0]<<1)>>1])?g=24:(Q=1,E=E+-1|0):g=24,24==(0|g))if(g=0,Q)Q=1;else{s=0|t[B+4>>2],C=0|t[A+4>>2],E=0|i[s>>0],Q=0|n[C+((255&E)<<1)>>1];i:do{if(2048&Q)E=s,g=51;else{for(k=Q,w=E,b=65535&Q,E=s;16&b;){if(w=0|i[(E=E+1|0)>>0],2048&(b=65535&(Q=0|n[C+((255&w)<<1)>>1]))|0){g=51;break i}k=Q}if(128&(Q=65535&k))g=51;else for(;;){n:do{if(1024&Q){Q=0|i[(b=E+1|0)>>0];t:do{if(!(8&n[C+((255&Q)<<1)>>1]))for(k=E;;){if(Q<<24>>24==w<<24>>24){if((0|i[(E=k+2|0)>>0])!=w<<24>>24){Q=w,E=k;break t}}else E=b;if(Q=0|i[(b=E+1|0)>>0],8&n[C+((255&Q)<<1)>>1])break;k=E}}while(0);E=Q<<24>>24==w<<24>>24?E+2|0:b}else{if(!(64&Q)){if(32&Q|0){E=0==(8&n[C+(f[(k=E+1|0)>>0]<<1)>>1])?E+2|0:k;break}if(8&Q|0)break i;E=E+1|0;break}if(Q=0|i[(k=E+1|0)>>0],b=0|a[C+((255&Q)<<1)>>1],(Q=Q<<24>>24==w<<24>>24)|0!=(8&b|0))w=k;else for(;;){if(32&b){if(8&n[C+(f[(E=E+2|0)>>0]<<1)>>1])break n}else E=k;if(Q=0|i[(k=E+1|0)>>0],b=0|a[C+((255&Q)<<1)>>1],(Q=Q<<24>>24==w<<24>>24)|0!=(8&b|0)){w=k;break}}E=Q?E+2|0:w}}while(0);if(w=0|i[E>>0],128&(Q=0|a[C+((255&w)<<1)>>1])|0){g=51;break}}}}while(0);for(51==(0|g)&&(g=0,i[E>>0]=0),b=0|t[A+4>>2];w=0|i[s>>0],Q=s+1|0,16==(24&(E=0|n[b+((255&w)<<1)>>1]));)s=Q;if(8&E)Q=0,E=0;else{if(w<<24>>24==91)break;for(Q=s;;){if(32&(E&=65535))Q=0==(8&n[b+(f[(C=Q+1|0)>>0]<<1)>>1])?Q+2|0:C;else{if(!(775&E))break;Q=Q+1|0}w=0|i[Q>>0],E=0|n[b+((255&w)<<1)>>1]}i:do{if(w<<24>>24==58){if(58!=(0|i[Q+1>>0])){w=58,k=s,C=0;break}for(i[Q>>0]=0,k=Q+2|0,b=0|t[A+4>>2],E=k;;)if(w=0|i[E>>0],32&(Q=0|a[b+((255&w)<<1)>>1]))E=0==(8&n[b+(f[(C=E+1|0)>>0]<<1)>>1])?E+2|0:C;else{if(!(775&Q)){Q=E,C=s;break i}E=E+1|0}}else k=s,C=0}while(0);if(E=Q+1|0,16==(24&n[b+((255&w)<<1)>>1]))do{w=0|i[E>>0],E=E+1|0}while(16==(24&n[b+((255&w)<<1)>>1]));if(w<<24>>24!=61){g=93;break r}for(i[Q>>0]=0,w=0|t[A+4>>2],s=E;16==(24&n[w+(f[s>>0]<<1)>>1]);)s=s+1|0;for(;!(8&n[w+(f[E>>0]<<1)>>1]);)E=E+1|0;Q=E+-1|0;i:do{if((0|Q)!=(0|s))for(;;){if(!(16&n[w+(f[Q>>0]<<1)>>1]))break i;if((0|(E=Q+-1|0))==(0|s)){E=Q;break}b=Q,Q=E,E=b}}while(0);if(i[E>>0]=0,!(b=0|Ln(12))){g=103;break r}if(Q=0|t[u+16>>2],w=0==(0|C)?Q:C,E=0|Ln(1+(0|mt(k))|0),t[b+4>>2]=E,t[b+8>>2]=0,!E){g=105;break r}if(dt(E,k,1+(0|mt(k))|0),!(0|Qt(A,w,b+8|0,s))){g=114;break r}do{if(0|Wt(w,Q)){if(0|(E=0|xn(A,w)))break;if(!(E=0|At(A,w))){g=110;break r}}else E=c}while(0);if(!(0|Pn(A,E,b))){g=112;break r}Q=0,E=0}}if(!(0|Ut(B,E+512|0)))break e}for(;c=0|i[Q>>0],16==(24&n[b+((255&c)<<1)>>1]);)Q=Q+1|0;k=Q;i:for(;;){for(E=k;;){if(32&(c=0|a[b+((255&c)<<1)>>1]))E=0==(8&n[b+(f[(C=E+1|0)>>0]<<1)>>1])?E+2|0:C;else{if(!(775&c))break;E=E+1|0}c=0|i[E>>0]}for(w=E;c=0|i[w>>0],16==(24&n[b+((255&c)<<1)>>1]);)w=w+1|0;switch(c<<24>>24){case 0:g=70;break r;case 93:break i}if((0|k)==(0|w)){g=70;break r}k=w}if(i[E>>0]=0,!(0|Qt(A,0,u+16|0,Q))){E=0,c=0;break A}if(!(c=0|xn(A,E=0|t[u+16>>2]))&&!(c=0|At(A,E))){g=74;break}if(!(0|Ut(B,512)))break e}if(70==(0|g)){Xe(14,121,100,0,0),E=0,c=0;break A}if(74==(0|g)){Xe(14,121,103,0,0),E=0,c=0;break A}if(93==(0|g)){Xe(14,121,101,0,0),E=0,c=0;break A}if(103==(0|g)){Xe(14,121,65,0,0),E=0,c=0;break A}if(105==(0|g)){Xe(14,121,65,0,0),E=b,c=b;break A}if(110==(0|g)){Xe(14,121,103,0,0),E=b,c=b;break A}if(112==(0|g)){Xe(14,121,65,0,0),E=b,c=b;break A}if(113==(0|g))return ht(B),Xn(0|t[u+16>>2]),G=u,0|(A=1);if(114==(0|g)){E=b,c=b;break A}}else l=0}while(0);Xe(14,121,7,0,0),E=0,c=0}else Xe(14,121,7,0,0),E=0,l=0,c=0}while(0);return ht(B),Xn(0|t[u+16>>2]),0|r&&(t[r>>2]=l),t[u>>2]=l,Ge(u+20|0,13,80858,u),t[u+8>>2]=80862,t[u+8+4>>2]=u+20,Ke(2,u+8|0),(0|o)!=(0|(l=0|t[A+8>>2]))&&(!function(A){A|=0;var e,r=0;e=G,G=G+16|0,(r=0|t[20564])||(r=52996,t[20564]=r),IB[31&t[r+8>>2]](e),t[e+8>>2]=A,IB[31&t[16+(0|t[e>>2])>>2]](e),G=e}(l),t[A+8>>2]=0),c?(Xn(0|t[c+4>>2]),Xn(0|t[c+8>>2]),Xn(E),G=u,0|(A=0)):(G=u,0|(A=0))}function Qt(A,e,r,f){A|=0,e|=0,r|=0,f|=0;var B,o=0,u=0,l=0,c=0,E=0,Q=0,w=0,b=0,k=0,s=0,C=0,d=0,I=0;if(!(B=0|Mt()))return 0|(r=0);do{if(0|Ut(B,1+(0|mt(f))|0)){I=0,o=0;A:for(;;){d=f;e:for(;;){c=0|t[A+4>>2],w=0|i[d>>0],f=0|a[c+((255&w)<<1)>>1];do{if(64&f){u=0|i[(l=d+1|0)>>0],f=0|a[c+((255&u)<<1)>>1];r:do{if(8&f|0||u<<24>>24==w<<24>>24)f=o;else for(Q=c,E=f,c=d;;){if(32&E){if(f=0|i[(l=c+2|0)>>0],8&n[Q+((255&f)<<1)>>1]){u=f,f=o;break r}u=f,c=l}else c=l;if(f=o+1|0,i[(0|t[B+4>>2])+o>>0]=u,l=c+1|0,Q=0|t[A+4>>2],u=0|i[l>>0],8&(E=0|a[Q+((255&u)<<1)>>1])|0||u<<24>>24==w<<24>>24)break;o=f}}while(0);u=u<<24>>24==w<<24>>24?l+1|0:l}else{if(1024&f|0){f=0|i[(l=d+1|0)>>0];r:do{if(8&n[c+((255&f)<<1)>>1])E=f,c=d,u=l,f=o;else for(u=f,c=d;;){if(u<<24>>24==w<<24>>24){if((0|i[(f=c+2|0)>>0])!=w<<24>>24){E=w,u=l,f=o;break r}u=w,c=f}else c=l;if(f=o+1|0,i[(0|t[B+4>>2])+o>>0]=u,o=0|i[(l=c+1|0)>>0],8&n[(0|t[A+4>>2])+((255&o)<<1)>>1]){E=o,u=l;break}u=o,o=f}}while(0);u=E<<24>>24==w<<24>>24?c+2|0:u;break}if(!(32&f)){if(8&f|0){f=53;break A}if(u=d+1|0,w<<24>>24==36)break e;i[(0|t[B+4>>2])+o>>0]=w,f=o+1|0;break}if(u=d+2|0,f=0|i[d+1>>0],8&n[c+((255&f)<<1)>>1]){f=53;break A}switch(((f+-98&255)>>>1|f+-98<<24>>24<<7&255)<<24>>24){case 8:f=13;break;case 6:f=10;break;case 0:f=8;break;case 9:f=9}i[(0|t[B+4>>2])+o>>0]=f,f=o+1|0}}while(0);d=u,o=f}for(b=Q=0|(k=(k=0|i[u>>0])<<24>>24==123?125:k<<24>>24==40?41:0)?d+2|0:u;w=0|i[b>>0],f=b+1|0,263&n[c+((255&w)<<1)>>1];)b=f;if(w<<24>>24==58)if(58==(0|i[f>>0]))for(i[b>>0]=0,c=b+2|0,f=0|t[A+4>>2],l=c;;){if(u=0|i[l>>0],!(263&n[f+((255&u)<<1)>>1])){C=u,E=Q,s=l,Q=w<<24>>24,u=b;break}l=l+1|0}else C=58,E=e,s=b,c=Q,Q=I,u=0;else C=w,E=e,s=b,c=Q,Q=I,u=0;if(i[s>>0]=0,0|k){if((0|k)!=(C<<24>>24|0)){f=38;break}f=s+1|0}else f=s;if(c=0|_n(A,E,c),0|u&&(i[u>>0]=Q),i[s>>0]=C,!c){f=43;break}if((u=(u=0|mt(c))+(d-f)+(0|t[B>>2])|0)>>>0>65536){f=45;break}if(!(0|Gt(B,u))){f=47;break}if((u=0|i[c>>0])<<24>>24)for(l=o;c=c+1|0,o=l+1|0,i[(0|t[B+4>>2])+l>>0]=u,(u=0|i[c>>0])<<24>>24;)l=o;i[s>>0]=C,I=Q}if(38==(0|f)){Xe(14,101,102,0,0);break}if(43==(0|f)){Xe(14,101,104,0,0);break}if(45==(0|f)){Xe(14,101,116,0,0);break}if(47==(0|f)){Xe(14,101,65,0,0);break}if(53==(0|f))return i[(0|t[B+4>>2])+o>>0]=0,Xn(0|t[r>>2]),t[r>>2]=t[B+4>>2],Xn(B),0|(r=1)}}while(0);return ht(B),0|(r=0)}function wt(A){return A|=0,0|function(){if((0|(0|function(){return 0|w()}()))!=(0|function(){return 0|k()}()))return 0|1;return 0|(0|(0|function(){return 0|b()}()))!=(0|function(){return 0|s()}())&1}()?0|(A=0):0|(A=0|h(0|A))}function bt(A,e,r){e|=0,r|=0;var i,n=0,f=0,a=0,B=0;if(i=G,G=G+48|0,(A|=0)>>>0>13)return Xe(15,113,7,0,0),Fn(e=0|t[r>>2]),t[r>>2]=0,void(G=i);if(!((B=0!=(0|m(82276,14)))&0!=(0|t[20570])))return Xe(15,113,65,0,0),Fn(e=0|t[r>>2]),t[r>>2]=0,void(G=i);if(!(n=0|t[20568]))return Fn(e=0|t[r>>2]),t[r>>2]=0,void(G=i);R(n),B=0|Hn(0|t[81968+(A<<2)>>2]);A:do{if((0|B)>0){if((0|B)>=10){if(!(n=0|Ln(B<<2)))for(y(0|t[20568]),a=0;;)if(R(0|t[20568]),f=0|vn(0|t[81968+(A<<2)>>2],a),y(0|t[20568]),0|f&&0|t[f+12>>2]&&(n=0!=(0|(n=0|t[r>>2]))&&(0|Hn(n))>(0|a)?0|vn(0|t[r>>2],a):0,YB[0&t[f+12>>2]](e,n,r,a,0|t[f>>2],0|t[f+4>>2])),(0|(a=a+1|0))==(0|B)){n=0;break A}}else n=i;f=0;do{t[n+(f<<2)>>2]=0|vn(0|t[81968+(A<<2)>>2],f),f=f+1|0}while((0|f)!=(0|B));if(y(0|t[20568]),(0|B)>0){A=0;do{0|(a=0|t[n+(A<<2)>>2])&&0|t[a+12>>2]&&(f=0!=(0|(f=0|t[r>>2]))&&(0|Hn(f))>(0|A)?0|vn(0|t[r>>2],A):0,YB[0&t[a+12>>2]](e,f,r,A,0|t[a>>2],0|t[a+4>>2])),A=A+1|0}while((0|A)!=(0|B))}if((0|n)==(0|i))return Fn(e=0|t[r>>2]),t[r>>2]=0,void(G=i)}else y(0|t[20568]),n=0}while(0);Xn(n),Fn(e=0|t[r>>2]),t[r>>2]=0,G=i}function kt(A,e,r){var i=0,n=0;return 0!=(0|(A|=0))&&0!=(0|(i=0|Ln(n=1+(0|mt(A))|0)))?If(0|i,0|A,0|n):i=0,0|i}function st(A,e){A|=0,e|=0;var r=0;A:do{if(e)for(r=A;;){if(e=e+-1|0,!(0|i[r>>0])){e=r;break A}if(r=r+1|0,!e){e=r;break}}else e=A}while(0);return e-A|0}function Ct(A,e,r,i){return r|=0,0,0==(0|(A|=0))|(e|=0)>>>0>2147483646?0|(e=0):(r=0|Ln(e))?(If(0|r,0|A,0|e),0|(e=r)):(Xe(15,115,65,0,0),0|(e=0))}function dt(A,e,r){A|=0,e|=0,r|=0;var n=0,t=0,f=0,a=0;A:do{if(r>>>0>1)for(n=0;;){if(!((t=0|i[e>>0])<<24>>24))break A;if(e=e+1|0,f=A+1|0,i[A>>0]=t,n=n+1|0,(r=r+-1|0)>>>0<=1){A=f,r=1,a=5;break}A=f}else n=0,a=5}while(0);return 5!=(0|a)||r?(i[A>>0]=0,a=n,0|(a=(f=0|mt(f=e))+a|0)):(a=n,0|(a=(f=0|mt(f=e))+a|0))}function It(A,e,r){A|=0,e|=0,r|=0;var n=0,t=0,f=0,a=0;A:do{if(r){for(n=0;0|i[A>>0];){if(n=n+1|0,!(r=r+-1|0)){t=0,r=n,A=e;break A}A=A+1|0}e:do{if(r>>>0>1)for(t=0;;){if(!((f=0|i[e>>0])<<24>>24))break e;if(e=e+1|0,a=A+1|0,i[A>>0]=f,t=t+1|0,(r=r+-1|0)>>>0<=1){A=a;break}A=a}else t=0}while(0);i[A>>0]=0,r=n,A=e}else t=0,r=0,A=e}while(0);return t+r+(0|mt(A))|0}function gt(A){switch((A|=0)<<24>>24){case 48:A=0;break;case 49:A=1;break;case 50:A=2;break;case 51:A=3;break;case 52:A=4;break;case 53:A=5;break;case 54:A=6;break;case 55:A=7;break;case 56:A=8;break;case 57:A=9;break;case 65:case 97:A=10;break;case 66:case 98:A=11;break;case 67:case 99:A=12;break;case 68:case 100:A=13;break;case 69:case 101:A=14;break;case 70:case 102:A=15;break;default:A=-1}return 0|A}function Dt(A,e){e|=0;var r,n=0,f=0,a=0,B=0;if(!(r=0|Ln((0|mt(A|=0))>>>1)))return Xe(15,118,65,0,0),0|(e=0);n=0|i[A>>0];A:do{if(n<<24>>24){for(B=r;;){for(;f=A+1|0,n<<24>>24==58;){if(!((n=0|i[f>>0])<<24>>24)){n=B;break A}A=f}if(A=A+2|0,!((f=0|i[f>>0])<<24>>24)){A=9;break}if(f=0|gt(f),((n=0|gt(n))|f|0)<0){A=11;break}if(a=B+1|0,i[B>>0]=n<<4|f,!((n=0|i[A>>0])<<24>>24)){n=a;break A}B=a}if(9==(0|A))return Xe(15,118,103,0,0),Xn(r),0|(e=0);if(11==(0|A))return Xn(r),Xe(15,118,102,0,0),0|(e=0)}else n=r}while(0);return e?(t[e>>2]=n-r,0|(e=r)):0|(e=r)}function Mt(){var A=0;return(A=0|Sn(16,82796))?0|A:(Xe(7,101,65,0,0),0|(A=0))}function ht(A){var e,r=0;if(A|=0){e=0|t[A+4>>2];do{if(0|e){if(r=0|t[A+8>>2],1&t[A+12>>2]){ri(e,r,82796,0);break}pn(e,r);break}}while(0);Xn(A)}}function Ut(A,e){e|=0;var r=0,i=0,n=0,f=0;if((i=0|t[(A|=0)>>2])>>>0>=e>>>0)return t[A>>2]=e,0|(A=e);if((0|t[A+8>>2])>>>0>=e>>>0)return 0|(r=0|t[A+4>>2])&&Df(r+i|0,0,e-i|0),t[A>>2]=e,0|(A=e);if(e>>>0>1610612732)return Xe(7,100,65,0,0),0|(A=0);1&t[A+12>>2]?0!=(0|(i=0|Ai((((e+3|0)>>>0)/3|0)<<2,82796,0)))&0!=(0|(r=0|t[A+4>>2]))?(If(0|i,0|r,0|t[A>>2]),ri(0|t[A+4>>2],0|t[A>>2],82796,0),t[A+4>>2]=0,n=A+4|0,r=i):(r=i,f=13):(r=0|Tn(0|t[A+4>>2],(((e+3|0)>>>0)/3|0)<<2,82796),f=13);do{if(13==(0|f)){if(0|r){n=A+4|0;break}return Xe(7,100,65,0,0),0|(A=0)}}while(0);return t[n>>2]=r,t[A+8>>2]=(((e+3|0)>>>0)/3|0)<<2,Df(r+(f=0|t[A>>2])|0,0,e-f|0),t[A>>2]=e,0|(A=e)}function Gt(A,e){e|=0;var r=0,i=0,n=0,f=0;if((i=0|t[(A|=0)>>2])>>>0>=e>>>0)return 0|(r=0|t[A+4>>2])&&Df(r+e|0,0,i-e|0),t[A>>2]=e,0|(A=e);if((r=0|t[A+8>>2])>>>0>=e>>>0)return Df((0|t[A+4>>2])+i|0,0,e-i|0),t[A>>2]=e,0|(A=e);if(e>>>0>1610612732)return Xe(7,105,65,0,0),0|(A=0);1&t[A+12>>2]?0!=(0|(i=0|Ai((((e+3|0)>>>0)/3|0)<<2,82796,0)))&0!=(0|(r=0|t[A+4>>2]))?(If(0|i,0|r,0|t[A>>2]),ri(0|t[A+4>>2],0|t[A>>2],82796,0),t[A+4>>2]=0,n=A+4|0,r=i):(r=i,f=13):(r=0|function(A,e,r,i,n){return e|=0,r|=0,0,0,(A|=0)?r?e>>>0>r>>>0?($r(A+r|0,e-r|0),0|A):(0|t[13247]&&(t[13247]=0),(r=0|ff(r))?(If(0|r,0|A,0|e),e?($r(A,e),af(A),0|(A=r)):(af(A),0|(A=r))):0|(A=0)):e?($r(A,e),af(A),0|(A=0)):(af(A),0|(A=0)):r?(0|t[13247]&&(t[13247]=0),0|(A=0|ff(r))):0|(A=0)}(0|t[A+4>>2],r,(((e+3|0)>>>0)/3|0)<<2),f=13);do{if(13==(0|f)){if(0|r){n=A+4|0;break}return Xe(7,105,65,0,0),0|(A=0)}}while(0);return t[n>>2]=r,t[A+8>>2]=(((e+3|0)>>>0)/3|0)<<2,Df(r+(f=0|t[A>>2])|0,0,e-f|0),t[A>>2]=e,0|(A=e)}function Zt(A,e,r){r=+r,Z()}function Nt(A,e){return N(A|=0,e|=0),1}function Ft(){return 82284}function Wt(A,e){e|=0;var r=0,n=0;if(r=0|i[(A|=0)>>0],n=0|i[e>>0],r<<24>>24!=0&&r<<24>>24==n<<24>>24)do{e=e+1|0,r=0|i[(A=A+1|0)>>0],n=0|i[e>>0]}while(r<<24>>24!=0&&r<<24>>24==n<<24>>24);return(255&r)-(255&n)|0}function Yt(A){return 1&(32==(0|(A|=0))|(A+-9|0)>>>0<5)|0}function Rt(A){return 1&(((32|(A|=0))-97|0)>>>0<6|0!=(0|yt(A)))|0}function yt(A){return((A|=0)+-48|0)>>>0<10|0}function Vt(A){return 0|(0==(0|function(A){return((A|=0)+-65|0)>>>0<26|0}(A|=0))?A:32|A)}function mt(A){A|=0;var e=0,r=0,n=0;A:do{if(3&A)for(e=A,r=A;;){if(!(0|i[r>>0]))break A;if(!(3&(e=r=r+1|0))){e=r,n=5;break}}else e=A,n=5}while(0);if(5==(0|n)){for(;!((-2139062144&(r=0|t[e>>2])^-2139062144)&r+-16843009);)e=e+4|0;if((255&r)<<24>>24)do{e=e+1|0}while(0!=(0|i[e>>0]))}return e-A|0}function Ht(A,e,r){A|=0,e|=0,r|=0;var n=0,t=0;A:do{if(r){for(;(n=0|i[A>>0])<<24>>24==(t=0|i[e>>0])<<24>>24;){if(!(r=r+-1|0)){A=0;break A}A=A+1|0,e=e+1|0}A=(255&n)-(255&t)|0}else A=0}while(0);return 0|A}function vt(A,e,r,i,n){A|=0,e|=0,r|=0,i|=0,n|=0;var f;return f=G,G=G+144|0,t[f>>2]=0,t[f+4>>2]=A,t[f+44>>2]=A,t[f+8>>2]=(0|A)<0?-1:A+2147483647|0,t[f+76>>2]=-1,Jt(f,0,0),i=0|Xt(f,r,1,i,n),n=0|E(),0|e&&(t[e>>2]=A+((0|t[f+4>>2])+(0|t[f+120>>2])-(0|t[f+8>>2]))),c(0|n),G=f,0|i}function Jt(A,e,r){e|=0,r|=0;var i,n;t[(A|=0)+112>>2]=e,t[A+112+4>>2]=r,i=0|t[A+8>>2],n=0|t[A+4>>2],t[A+120>>2]=i-n,t[A+120+4>>2]=((i-n|0)<0)<<31>>31,t[A+104>>2]=(0!=(0|e)|0!=(0|r))&((((i-n|0)<0)<<31>>31|0)>(0|r)|((i-n|0)>>>0>e>>>0?(((i-n|0)<0)<<31>>31|0)==(0|r):0))?n+e:i}function Xt(A,e,r,n,a){A|=0,e|=0,r|=0,n|=0,a|=0;var B=0,u=0,l=0,Q=0,w=0,b=0,k=0,s=0;A:do{if(e>>>0>36)t[20571]=28,a=0,n=0;else{do{(B=0|t[A+4>>2])>>>0<(0|t[A+104>>2])>>>0?(t[A+4>>2]=B+1,B=0|f[B>>0]):B=0|Tt(A)}while(0!=(0|Yt(B)));e:do{switch(0|B){case 43:case 45:if(u=(45==(0|B))<<31>>31,(B=0|t[A+4>>2])>>>0<(0|t[A+104>>2])>>>0){t[A+4>>2]=B+1,B=0|f[B>>0];break e}B=0|Tt(A);break e;default:u=0}}while(0);do{if(16==(16|e)&48==(0|B)){if((B=0|t[A+4>>2])>>>0<(0|t[A+104>>2])>>>0?(t[A+4>>2]=B+1,B=0|f[B>>0]):B=0|Tt(A),120!=(32|B)){if(e){k=32;break}e=8,k=47;break}if((B=0|t[A+4>>2])>>>0<(0|t[A+104>>2])>>>0?(t[A+4>>2]=B+1,B=0|f[B>>0]):B=0|Tt(A),(0|f[51345+B>>0])>15){if((n=0==(0|t[A+104>>2]))||(t[A+4>>2]=(0|t[A+4>>2])-1),!r){Jt(A,0,0),a=0,n=0;break A}if(n){a=0,n=0;break A}t[A+4>>2]=(0|t[A+4>>2])-1,a=0,n=0;break A}e=16,k=47}else{if(!((e=0==(0|e)?10:e)>>>0>(0|f[51345+B>>0])>>>0)){0|t[A+104>>2]&&(t[A+4>>2]=(0|t[A+4>>2])-1),Jt(A,0,0),t[20571]=28,a=0,n=0;break A}k=32}}while(0);e:do{if(32==(0|k))if(10==(0|e))if((B=B+-48|0)>>>0<10){l=B,e=0;do{e=(10*e|0)+l|0,(B=0|t[A+4>>2])>>>0<(0|t[A+104>>2])>>>0?(t[A+4>>2]=B+1,B=0|f[B>>0]):B=0|Tt(A),l=B+-48|0}while(l>>>0<10&e>>>0<429496729);if(l>>>0<10){r=0;do{if(Q=0|lf(0|e,0|r,10,0),(w=0|E())>>>0>~(b=((0|l)<0)<<31>>31)>>>0|(0|w)==(0|~b)&Q>>>0>~l>>>0){Q=e,e=10,k=76;break e}e=0|cf(0|Q,0|w,0|l,0|b),r=0|E(),(B=0|t[A+4>>2])>>>0<(0|t[A+104>>2])>>>0?(t[A+4>>2]=B+1,B=0|f[B>>0]):B=0|Tt(A),l=B+-48|0}while(l>>>0<10&(r>>>0<429496729|429496729==(0|r)&e>>>0<2576980378));l>>>0>9?B=u:(Q=e,e=10,k=76)}else r=0,B=u}else r=0,e=0,B=u;else k=47}while(0);e:do{if(47==(0|k)){if(!(e+-1&e)){if(s=0|i[80868+((23*e|0)>>>5&7)>>0],e>>>0>(255&(l=0|i[51345+B>>0]))>>>0){l&=255,Q=0;do{Q=l|Q<<s,(B=0|t[A+4>>2])>>>0<(0|t[A+104>>2])>>>0?(t[A+4>>2]=B+1,B=0|f[B>>0]):B=0|Tt(A),l=255&(r=0|i[51345+B>>0])}while(Q>>>0<134217728&e>>>0>l>>>0);k=0}else r=l,k=0,Q=0,l&=255;if(w=0|sf(-1,-1,0|s),e>>>0<=l>>>0|(b=0|E())>>>0<k>>>0|(0|b)==(0|k)&w>>>0<Q>>>0){r=k,k=76;break}for(B=k;;){if(Q=0|Cf(0|Q,0|B,0|s),l=0|E(),Q|=255&r,(B=0|t[A+4>>2])>>>0<(0|t[A+104>>2])>>>0?(t[A+4>>2]=B+1,B=0|f[B>>0]):B=0|Tt(A),e>>>0<=(255&(r=0|i[51345+B>>0]))>>>0|l>>>0>b>>>0|(0|l)==(0|b)&Q>>>0>w>>>0){r=l,k=76;break e}B=l}}if(e>>>0>(255&(l=0|i[51345+B>>0]))>>>0){l&=255,Q=0;do{Q=l+(0|o(Q,e))|0,(B=0|t[A+4>>2])>>>0<(0|t[A+104>>2])>>>0?(t[A+4>>2]=B+1,B=0|f[B>>0]):B=0|Tt(A),l=255&(r=0|i[51345+B>>0])}while(Q>>>0<119304647&e>>>0>l>>>0);w=0}else r=l,Q=0,w=0,l&=255;if(e>>>0>l>>>0)for(k=0|bf(-1,-1,0|e,0),s=0|E(),l=w;;){if(l>>>0>s>>>0|(0|l)==(0|s)&Q>>>0>k>>>0){r=l,k=76;break e}if(w=0|lf(0|Q,0|l,0|e,0),(b=0|E())>>>0>4294967295|-1==(0|b)&w>>>0>~(r&=255)>>>0){r=l,k=76;break e}if(Q=0|cf(0|w,0|b,0|r,0),l=0|E(),(B=0|t[A+4>>2])>>>0<(0|t[A+104>>2])>>>0?(t[A+4>>2]=B+1,B=0|f[B>>0]):B=0|Tt(A),e>>>0<=(255&(r=0|i[51345+B>>0]))>>>0){r=l,k=76;break}}else r=w,k=76}}while(0);if(76==(0|k))if(e>>>0>(0|f[51345+B>>0])>>>0){do{(B=0|t[A+4>>2])>>>0<(0|t[A+104>>2])>>>0?(t[A+4>>2]=B+1,B=0|f[B>>0]):B=0|Tt(A)}while(e>>>0>(0|f[51345+B>>0])>>>0);t[20571]=68,r=a,e=n,B=0==(1&n|0)&!0?u:0}else e=Q,B=u;if(0|t[A+104>>2]&&(t[A+4>>2]=(0|t[A+4>>2])-1),!(r>>>0<a>>>0|(0|r)==(0|a)&e>>>0<n>>>0)){if(!(0!=(1&n|0)|!1|0!=(0|B))){t[20571]=68,n=0|cf(0|n,0|a,-1,-1),a=0|E();break}if(r>>>0>a>>>0|(0|r)==(0|a)&e>>>0>n>>>0){t[20571]=68;break}}n=0|Ef(e^B|0,r^(n=((0|B)<0)<<31>>31)|0,0|B,0|n),a=0|E()}}while(0);return c(0|a),0|n}function Tt(A){var e=0,r=0,n=0,a=0,B=0,o=0,u=0;return(0==(0|(e=0|t[(A|=0)+112>>2]))&0==(0|(r=0|t[A+112+4>>2]))||(0|(a=0|t[A+120+4>>2]))<(0|r)|((0|a)==(0|r)?(0|t[A+120>>2])>>>0<e>>>0:0))&&(0|(e=0|function(A){var e;e=G,G=G+16|0,A=(0==(0|Lt(A=A|0))?1==(0|DB[31&t[A+32>>2]](A,e,1)):0)?0|f[e>>0]:-1;return G=e,0|A}(A)))>=0?(n=0|t[A+112>>2],a=0|t[A+112+4>>2],r=0|t[A+8>>2],0==(0|n)&0==(0|a)||(B=0|t[A+4>>2],o=0|Ef(0|n,0|a,0|t[A+120>>2],0|t[A+120+4>>2]),(0|(a=0|E()))>(((r-B|0)<0)<<31>>31|0)|(o>>>0>(r-B|0)>>>0?(0|a)==(((r-B|0)<0)<<31>>31|0):0))?(n=r,u=9):t[A+104>>2]=B+(o+-1),9==(0|u)&&(t[A+104>>2]=r,r=n),r?(B=r+1-(o=0|t[A+4>>2])|0,B=0|cf(0|t[A+120>>2],0|t[A+120+4>>2],0|B,((0|B)<0)<<31>>31|0),r=0|E(),t[A+120>>2]=B,t[A+120+4>>2]=r,r=o):r=0|t[A+4>>2],(0|e)!=(0|f[(r=r+-1|0)>>0])&&(i[r>>0]=e)):u=4,4==(0|u)&&(t[A+104>>2]=0,e=-1),0|e}function Lt(A){var e=0,r=0;return e=0|i[(A|=0)+74>>0],i[A+74>>0]=e+255|e,(0|t[A+20>>2])>>>0>(0|t[A+28>>2])>>>0&&DB[31&t[A+36>>2]](A,0,0),t[A+16>>2]=0,t[A+28>>2]=0,t[A+20>>2]=0,4&(e=0|t[A>>2])?(t[A>>2]=32|e,e=-1):(r=(0|t[A+44>>2])+(0|t[A+48>>2])|0,t[A+8>>2]=r,t[A+4>>2]=r,e=e<<27>>31),0|e}function St(A,e,r){return A=0|vt(A|=0,e|=0,r|=0,-1,0),E(),0|A}function pt(A,e,r,i){A|=0,i|=0;var n,f,a=0,B=0,u=0,l=0;f=G,G=G+208|0,n=0|o(r|=0,e|=0),t[f+192>>2]=1,t[f+192+4>>2]=0;A:do{if(0|n){for(t[f+4>>2]=r,t[f>>2]=r,e=r,a=r,B=2;e=e+r+a|0,t[f+(B<<2)>>2]=e,e>>>0<n>>>0;)l=a,a=e,B=B+1|0,e=l;if((A+n+(0-r)|0)>>>0>A>>>0){e=1,a=A,B=1;do{do{if(3!=(3&e|0)){if((0|t[f+((e=B+-1|0)<<2)>>2])>>>0<(A+n+(0-r)-a|0)>>>0?zt(a,r,i,B,f):jt(a,r,i,f+192|0,B,0,f),1==(0|B)){xt(f+192|0,1),B=0;break}xt(f+192|0,e),B=1;break}zt(a,r,i,B,f),Kt(f+192|0,2),B=B+2|0}while(0);e=1|t[f+192>>2],t[f+192>>2]=e,a=a+r|0}while(a>>>0<(A+n+(0-r)|0)>>>0)}else e=1,a=A,B=1;for(jt(a,r,i,f+192|0,B,0,f);;){if(1==(0|B)&1==(0|e)){if(!(0|t[f+192+4>>2]))break A;u=19}else(0|B)<2?u=19:(xt(f+192|0,2),l=B+-2|0,t[f+192>>2]=7^t[f+192>>2],Kt(f+192|0,1),jt(a+(0-(0|t[f+(l<<2)>>2]))+(0-r)|0,r,i,f+192|0,B+-1|0,1,f),xt(f+192|0,1),e=1|t[f+192>>2],t[f+192>>2]=e,jt(a=a+(0-r)|0,r,i,f+192|0,l,1,f),B=l);19==(0|u)&&(u=0,Kt(f+192|0,l=0|Ot(f+192|0)),e=0|t[f+192>>2],a=a+(0-r)|0,B=l+B|0)}}}while(0);G=f}function zt(A,e,r,i,n){A|=0,e|=0,r|=0,i|=0,n|=0;var f,a=0,B=0,o=0,u=0;f=G,G=G+240|0,t[f>>2]=A;A:do{if((0|i)>1)for(a=A,B=A,A=1;;){if(o=(B=B+(0-e)|0)+(0-(0|t[n+((u=i+-2|0)<<2)>>2]))|0,(0|gB[63&r](a,o))>-1&&(0|gB[63&r](a,B))>-1)break A;if(a=f+(A<<2)|0,(0|gB[63&r](o,B))>-1?(t[a>>2]=o,B=o,i=i+-1|0):(t[a>>2]=B,i=u),A=A+1|0,(0|i)<=1)break A;a=0|t[f>>2]}else A=1}while(0);_t(e,f,A),G=f}function Kt(A,e){A|=0;var r=0,i=0;(e|=0)>>>0>31?(r=0|t[A+4>>2],t[A>>2]=r,t[A+4>>2]=0,i=0,e=e+-32|0):(r=0|t[A>>2],i=0|t[A+4>>2]),t[A>>2]=i<<32-e|r>>>e,t[A+4>>2]=i>>>e}function jt(A,e,r,i,n,f,a){A|=0,e|=0,r|=0,n|=0,f|=0,a|=0;var B,o=0,u=0,l=0;B=G,G=G+240|0,u=0|t[(i|=0)>>2],t[B+232>>2]=u,l=0|t[i+4>>2],t[B+232+4>>2]=l,t[B>>2]=A;A:do{if(1!=(0|u)|0!=(0|l)&&(o=A+(0-(0|t[a+(n<<2)>>2]))|0,(0|gB[63&r](o,A))>=1))for(u=A,l=1,i=0==(0|f);;){if(i&(0|n)>1){if(A=u+(0-e)|0,i=0|t[a+(n+-2<<2)>>2],(0|gB[63&r](A,o))>-1){o=u,i=l,A=10;break A}if((0|gB[63&r](A+(0-i)|0,o))>-1){o=u,i=l,A=10;break A}}if(i=l+1|0,t[B+(l<<2)>>2]=o,Kt(B+232|0,A=0|Ot(B+232|0)),A=A+n|0,1==(0|t[B+232>>2])&&0==(0|t[B+232+4>>2])){n=A,A=10;break A}if(n=o+(0-(0|t[a+(A<<2)>>2]))|0,(0|gB[63&r](n,0|t[B>>2]))<1){u=i,n=A,i=0,A=9;break}u=o,o=n,l=i,n=A,i=1}else o=A,u=1,i=f,A=9}while(0);9==(0|A)&&0==(0|i)&&(i=u,A=10),10==(0|A)&&(_t(e,B,i),zt(o,e,r,n,a)),G=B}function xt(A,e){A|=0;var r=0,i=0;(e|=0)>>>0>31?(r=0|t[A>>2],t[A+4>>2]=r,t[A>>2]=0,i=0,e=e+-32|0):(r=0|t[A+4>>2],i=0|t[A>>2]),t[A+4>>2]=i>>>(32-e|0)|r<<e,t[A>>2]=i<<e}function Ot(A){var e;return(e=0|Pt((0|t[(A|=0)>>2])-1|0))?0|e:0|(0==(0|(A=0|Pt(0|t[A+4>>2])))?0:A+32|0)}function Pt(A){var e=0;if(A|=0)if(1&A)e=0;else for(e=0;e=e+1|0,!(2&A);)A>>>=1;else e=32;return 0|e}function _t(A,e,r){A|=0,e|=0,r|=0;var i,n=0,f=0,a=0;i=G,G=G+256|0;A:do{if((0|r)>=2&&(t[e+(r<<2)>>2]=i,0|A))for(n=i;;){f=A>>>0<256?A:256,If(0|n,0|t[e>>2],0|f),n=0;do{a=e+(n<<2)|0,n=n+1|0,If(0|t[a>>2],0|t[e+(n<<2)>>2],0|f),t[a>>2]=(0|t[a>>2])+f}while((0|n)!=(0|r));if(!(A=A-f|0))break A;n=0|t[e+(r<<2)>>2]}}while(0);G=i}function qt(A,e){return A=0|$t(A|=0,e|=0),0|((0|i[A>>0])==(255&e)<<24>>24?A:0)}function $t(A,e){A|=0,e|=0;var r=0,n=0;A:do{if(255&e){if(3&A)do{if((n=0|i[A>>0])<<24>>24==0||n<<24>>24==(255&e)<<24>>24)break A;A=A+1|0}while(0!=(3&A|0));n=0|o(255&e,16843009),r=0|t[A>>2];e:do{if(!((-2139062144&r^-2139062144)&r+-16843009))do{if((-2139062144&(r^=n)^-2139062144)&r+-16843009|0)break e;r=0|t[(A=A+4|0)>>2]}while(!((-2139062144&r^-2139062144)&r+-16843009|0))}while(0);for(;(n=0|i[A>>0])<<24>>24!=0&&n<<24>>24!=(255&e)<<24>>24;)A=A+1|0}else A=A+(0|mt(A))|0}while(0);return 0|A}function Af(A,e,r){A|=0,e|=0;var n=0,t=0;if(r|=0){n=0|i[A>>0];A:do{if(n<<24>>24)for(;;){if(r=r+-1|0,!(n<<24>>24==(t=0|i[e>>0])<<24>>24&0!=(0|r)&t<<24>>24!=0))break A;if(e=e+1|0,!((n=0|i[(A=A+1|0)>>0])<<24>>24)){n=0;break}}else n=0}while(0);e=(255&n)-(0|f[e>>0])|0}else e=0;return 0|e}function ef(A,e){return 0|rf(A|=0,e|=0,1+(0|mt(A))|0)}function rf(A,e,r){A|=0,e|=0,r|=0;A:do{if(r){for(;(0|i[A+(r=r+-1|0)>>0])!=(255&e)<<24>>24;)if(!r){r=0;break A}r=A+r|0}else r=0}while(0);return 0|r}function nf(A,e){e|=0;var r=0,n=0,t=0;r=0|i[(A|=0)>>0];A:do{if(r<<24>>24){for(t=r,n=255&r;;){if(!((r=0|i[e>>0])<<24>>24)){r=t;break A}if(t<<24>>24!=r<<24>>24&&(0|(t=0|Vt(n)))!=(0|Vt(0|f[e>>0])))break;if(e=e+1|0,!((r=0|i[(A=A+1|0)>>0])<<24>>24)){r=0;break A}t=r,n=255&r}r=0|i[A>>0]}else r=0}while(0);return(t=0|Vt(255&r))-(0|Vt(0|f[e>>0]))|0}function tf(A){A|=0;for(var e=0,r=0,n=0,t=0;r=A+1|0,0|Yt(0|i[A>>0]);)A=r;switch(0|(e=0|i[A>>0])){case 45:A=1,n=5;break;case 43:A=0,n=5;break;default:t=0,r=A}if(5==(0|n)&&(e=0|i[r>>0],t=A),0|yt(e)){A=0,e=r;do{A=48+(10*A|0)-(0|i[e>>0])|0,e=e+1|0}while(0!=(0|yt(0|i[e>>0])))}else A=0;return 0|(0==(0|t)?0-A|0:A)}function ff(A){A|=0;var e,r=0,i=0,n=0,f=0,a=0,B=0,o=0,u=0,l=0,c=0,E=0,Q=0,w=0,b=0,k=0,s=0,C=0,d=0,I=0;e=G,G=G+16|0;do{if(A>>>0<245){if(Q=A>>>0<11?16:A+11&-8,(c=0|t[20573])>>>(Q>>>3)&3|0)return A=0|t[(i=82332+((c>>>(Q>>>3)&1^1)+(Q>>>3)<<1<<2)|0)+8>>2],(0|(r=0|t[A+8>>2]))==(0|i)?t[20573]=c&~(1<<(c>>>(Q>>>3)&1^1)+(Q>>>3)):(t[r+12>>2]=i,t[i+8>>2]=r),I=(c>>>(Q>>>3)&1^1)+(Q>>>3)<<3,t[A+4>>2]=3|I,t[A+I+4>>2]=1|t[A+I+4>>2],G=e,0|(I=A+8|0);if(Q>>>0>(E=0|t[20575])>>>0){if(c>>>(Q>>>3)|0)return n=0|t[82332+((f=((n=(((A=c>>>(Q>>>3)<<(Q>>>3)&(2<<(Q>>>3)|0-(2<<(Q>>>3))))&0-A)-1|0)>>>(((A&0-A)-1|0)>>>12&16))>>>5&8|((A&0-A)-1|0)>>>12&16|n>>>(n>>>5&8)>>>2&4|(f=n>>>(n>>>5&8)>>>(n>>>(n>>>5&8)>>>2&4))>>>1&2|f>>>(f>>>1&2)>>>1&1)+(f>>>(f>>>1&2)>>>(f>>>(f>>>1&2)>>>1&1))|0)<<1<<2)+8>>2],(0|(A=0|t[n+8>>2]))==(82332+(f<<1<<2)|0)?(t[20573]=c&~(1<<f),A=c&~(1<<f)):(t[A+12>>2]=82332+(f<<1<<2),t[82332+(f<<1<<2)+8>>2]=A,A=c),t[n+4>>2]=3|Q,t[n+Q+4>>2]=(f<<3)-Q|1,t[n+(f<<3)>>2]=(f<<3)-Q,0|E&&(i=0|t[20578],A&1<<(E>>>3)?(A=82332+(E>>>3<<1<<2)+8|0,r=0|t[82332+(E>>>3<<1<<2)+8>>2]):(t[20573]=A|1<<(E>>>3),A=82332+(E>>>3<<1<<2)+8|0,r=82332+(E>>>3<<1<<2)|0),t[A>>2]=i,t[r+12>>2]=i,t[i+8>>2]=r,t[i+12>>2]=82332+(E>>>3<<1<<2)),t[20575]=(f<<3)-Q,t[20578]=n+Q,G=e,0|(I=n+8|0);if(B=0|t[20574]){for(l=0|t[82596+(((u=((B&0-B)-1|0)>>>(((B&0-B)-1|0)>>>12&16))>>>5&8|((B&0-B)-1|0)>>>12&16|u>>>(u>>>5&8)>>>2&4|(l=u>>>(u>>>5&8)>>>(u>>>(u>>>5&8)>>>2&4))>>>1&2|l>>>(l>>>1&2)>>>1&1)+(l>>>(l>>>1&2)>>>(l>>>(l>>>1&2)>>>1&1))<<2)>>2],u=(-8&t[l+4>>2])-Q|0,r=l;(A=0|t[r+16>>2])||(A=0|t[r+20>>2]);)u=(o=(r=(-8&t[A+4>>2])-Q|0)>>>0<u>>>0)?r:u,r=A,l=o?A:l;if((o=l+Q|0)>>>0>l>>>0){f=0|t[l+24>>2],A=0|t[l+12>>2];do{if((0|A)==(0|l)){if(!(A=0|t[(r=l+20|0)>>2])&&!(A=0|t[(r=l+16|0)>>2])){r=0;break}for(;;)if(i=0|t[(n=A+20|0)>>2])A=i,r=n;else{if(!(i=0|t[(n=A+16|0)>>2]))break;A=i,r=n}t[r>>2]=0,r=A}else r=0|t[l+8>>2],t[r+12>>2]=A,t[A+8>>2]=r,r=A}while(0);do{if(0|f){if(A=0|t[l+28>>2],(0|l)==(0|t[82596+(A<<2)>>2])){if(t[82596+(A<<2)>>2]=r,!r){t[20574]=B&~(1<<A);break}}else if(t[((0|t[f+16>>2])==(0|l)?f+16|0:f+20|0)>>2]=r,!r)break;t[r+24>>2]=f,0|(A=0|t[l+16>>2])&&(t[r+16>>2]=A,t[A+24>>2]=r),0|(A=0|t[l+20>>2])&&(t[r+20>>2]=A,t[A+24>>2]=r)}}while(0);return u>>>0<16?(I=u+Q|0,t[l+4>>2]=3|I,t[(I=l+I+4|0)>>2]=1|t[I>>2]):(t[l+4>>2]=3|Q,t[o+4>>2]=1|u,t[o+u>>2]=u,0|E&&(i=0|t[20578],1<<(E>>>3)&c?(A=82332+(E>>>3<<1<<2)+8|0,r=0|t[82332+(E>>>3<<1<<2)+8>>2]):(t[20573]=1<<(E>>>3)|c,A=82332+(E>>>3<<1<<2)+8|0,r=82332+(E>>>3<<1<<2)|0),t[A>>2]=i,t[r+12>>2]=i,t[i+8>>2]=r,t[i+12>>2]=82332+(E>>>3<<1<<2)),t[20575]=u,t[20578]=o),G=e,0|(I=l+8|0)}}}}else if(A>>>0<=4294967231){if(Q=A+11&-8,n=0|t[20574]){u=(A+11|0)>>>8?Q>>>0>16777215?31:Q>>>((u=14-(((u=(A+11|0)>>>8<<((1048320+((A+11|0)>>>8)|0)>>>16&8))+520192|0)>>>16&4|(1048320+((A+11|0)>>>8)|0)>>>16&8|(245760+(u<<((u+520192|0)>>>16&4))|0)>>>16&2)+(u<<((u+520192|0)>>>16&4)<<((245760+(u<<((u+520192|0)>>>16&4))|0)>>>16&2)>>>15)|0)+7|0)&1|u<<1:0,A=0|t[82596+(u<<2)>>2];A:do{if(A)for(i=0-Q|0,r=0,o=Q<<(31==(0|u)?0:25-(u>>>1)|0),B=0;;){if((f=(-8&t[A+4>>2])-Q|0)>>>0<i>>>0){if(!f){i=0,r=A,b=65;break A}i=f,B=A}if(r=0==(0|(b=0|t[A+20>>2]))|(0|b)==(0|(A=0|t[A+16+(o>>>31<<2)>>2]))?r:b,!A){A=B,b=61;break}o<<=1}else i=0-Q|0,r=0,A=0,b=61}while(0);if(61==(0|b)){if(0==(0|r)&0==(0|A)){if(!(((A=2<<u)|0-A)&n))break;r=0|t[82596+(((E=((A|0-A)&n&0-((A|0-A)&n))-1|0)>>>(E>>>12&16)>>>5&8|E>>>12&16|(A=E>>>(E>>>12&16)>>>(E>>>(E>>>12&16)>>>5&8))>>>2&4|A>>>(A>>>2&4)>>>1&2|(r=A>>>(A>>>2&4)>>>(A>>>(A>>>2&4)>>>1&2))>>>1&1)+(r>>>(r>>>1&1))<<2)>>2],A=0}r?b=65:(B=i,o=A)}if(65==(0|b))for(;;){if(i=(f=(E=(-8&t[r+4>>2])-Q|0)>>>0<i>>>0)?E:i,f=f?r:A,(A=0|t[r+16>>2])||(A=0|t[r+20>>2]),!A){B=i,o=f;break}r=A,A=f}if(0!=(0|o)&&B>>>0<((0|t[20575])-Q|0)>>>0&&(l=o+Q|0)>>>0>o>>>0){a=0|t[o+24>>2],A=0|t[o+12>>2];do{if((0|A)==(0|o)){if(!(A=0|t[(r=o+20|0)>>2])&&!(A=0|t[(r=o+16|0)>>2])){A=0;break}for(;;)if(i=0|t[(f=A+20|0)>>2])A=i,r=f;else{if(!(i=0|t[(f=A+16|0)>>2]))break;A=i,r=f}t[r>>2]=0}else I=0|t[o+8>>2],t[I+12>>2]=A,t[A+8>>2]=I}while(0);do{if(a){if(r=0|t[o+28>>2],(0|o)==(0|t[82596+(r<<2)>>2])){if(t[82596+(r<<2)>>2]=A,!A){t[20574]=n&~(1<<r),n&=~(1<<r);break}}else if(t[((0|t[a+16>>2])==(0|o)?a+16|0:a+20|0)>>2]=A,!A)break;t[A+24>>2]=a,0|(r=0|t[o+16>>2])&&(t[A+16>>2]=r,t[r+24>>2]=A),(r=0|t[o+20>>2])&&(t[A+20>>2]=r,t[r+24>>2]=A)}}while(0);A:do{if(B>>>0<16)I=B+Q|0,t[o+4>>2]=3|I,t[(I=o+I+4|0)>>2]=1|t[I>>2];else{if(t[o+4>>2]=3|Q,t[l+4>>2]=1|B,t[l+B>>2]=B,i=B>>>3,B>>>0<256){(A=0|t[20573])&1<<i?(A=82332+(i<<1<<2)+8|0,r=0|t[82332+(i<<1<<2)+8>>2]):(t[20573]=A|1<<i,A=82332+(i<<1<<2)+8|0,r=82332+(i<<1<<2)|0),t[A>>2]=l,t[r+12>>2]=l,t[l+8>>2]=r,t[l+12>>2]=82332+(i<<1<<2);break}if(A=82596+((i=(A=B>>>8)?B>>>0>16777215?31:B>>>((i=14-((520192+(A<<((A+1048320|0)>>>16&8))|0)>>>16&4|(A+1048320|0)>>>16&8|((i=A<<((A+1048320|0)>>>16&8)<<((520192+(A<<((A+1048320|0)>>>16&8))|0)>>>16&4))+245760|0)>>>16&2)+(i<<((i+245760|0)>>>16&2)>>>15)|0)+7|0)&1|i<<1:0)<<2)|0,t[l+28>>2]=i,t[l+16+4>>2]=0,t[l+16>>2]=0,!(n&(r=1<<i))){t[20574]=n|r,t[A>>2]=l,t[l+24>>2]=A,t[l+12>>2]=l,t[l+8>>2]=l;break}A=0|t[A>>2];e:do{if((-8&t[A+4>>2]|0)!=(0|B)){for(n=B<<(31==(0|i)?0:25-(i>>>1)|0);r=0|t[(i=A+16+(n>>>31<<2)|0)>>2];){if((-8&t[r+4>>2]|0)==(0|B)){A=r;break e}n<<=1,A=r}t[i>>2]=l,t[l+24>>2]=A,t[l+12>>2]=l,t[l+8>>2]=l;break A}}while(0);I=0|t[(d=A+8|0)>>2],t[I+12>>2]=l,t[d>>2]=l,t[l+8>>2]=I,t[l+12>>2]=A,t[l+24>>2]=0}}while(0);return G=e,0|(I=o+8|0)}}}else Q=-1}while(0);if((i=0|t[20575])>>>0>=Q>>>0)return r=i-Q|0,A=0|t[20578],r>>>0>15?(I=A+Q|0,t[20578]=I,t[20575]=r,t[I+4>>2]=1|r,t[A+i>>2]=r,t[A+4>>2]=3|Q):(t[20575]=0,t[20578]=0,t[A+4>>2]=3|i,t[A+i+4>>2]=1|t[A+i+4>>2]),G=e,0|(I=A+8|0);if((f=0|t[20576])>>>0>Q>>>0)return C=f-Q|0,t[20576]=C,d=(I=0|t[20579])+Q|0,t[20579]=d,t[d+4>>2]=1|C,t[I+4>>2]=3|Q,G=e,0|(I=I+8|0);if(0|t[20691]?A=0|t[20693]:(t[20693]=4096,t[20692]=4096,t[20694]=-1,t[20695]=-1,t[20696]=0,t[20684]=0,t[20691]=-16&e^1431655768,A=4096),B=Q+48|0,((l=A+(o=Q+47|0)|0)&(u=0-A|0))>>>0<=Q>>>0)return G=e,0|(I=0);if(0|(A=0|t[20683])&&(((E=0|t[20681])+(l&u)|0)>>>0<=E>>>0||(E+(l&u)|0)>>>0>A>>>0))return G=e,0|(I=0);A:do{if(4&t[20684])A=0,b=143;else{r=0|t[20579];e:do{if(r){for(i=82740;!((E=0|t[i>>2])>>>0<=r>>>0&&(E+(0|t[i+4>>2])|0)>>>0>r>>>0);){if(!(A=0|t[i+8>>2])){b=128;break e}i=A}if((l-f&u)>>>0<2147483647)if((0|(A=0|uf(l-f&u)))==((0|t[i>>2])+(0|t[i+4>>2])|0)){if(-1!=(0|A)){B=A,a=l-f&u,b=145;break A}A=l-f&u}else n=A,i=l-f&u,b=136;else A=0}else b=128}while(0);do{if(128==(0|b))if(-1!=(0|(r=0|uf(0)))&&(a=(0==((a=0|t[20692])+-1&r|0)?0:(a+-1+r&0-a)-r|0)+(l&u)|0,w=0|t[20681],a>>>0>Q>>>0&a>>>0<2147483647)){if(0|(E=0|t[20683])&&(a+w|0)>>>0<=w>>>0|(a+w|0)>>>0>E>>>0){A=0;break}if((0|(A=0|uf(a)))==(0|r)){B=r,b=145;break A}n=A,i=a,b=136}else A=0}while(0);do{if(136==(0|b)){if(r=0-i|0,!(B>>>0>i>>>0&i>>>0<2147483647&-1!=(0|n))){if(-1==(0|n)){A=0;break}B=n,a=i,b=145;break A}if((A=o-i+(A=0|t[20693])&0-A)>>>0>=2147483647){B=n,a=i,b=145;break A}if(-1==(0|uf(A))){uf(r),A=0;break}B=n,a=A+i|0,b=145;break A}}while(0);t[20684]=4|t[20684],b=143}}while(0);if(143==(0|b)&&(l&u)>>>0<2147483647&&!(-1==(0|(k=0|uf(l&u)))|1^(C=((s=0|uf(0))-k|0)>>>0>(Q+40|0)>>>0)|k>>>0<s>>>0&-1!=(0|k)&-1!=(0|s)^1)&&(B=k,a=C?s-k|0:A,b=145),145==(0|b)){A=(0|t[20681])+a|0,t[20681]=A,A>>>0>(0|t[20682])>>>0&&(t[20682]=A),u=0|t[20579];A:do{if(u){for(n=82740;;){if((0|B)==((A=0|t[n>>2])+(r=0|t[n+4>>2])|0)){b=154;break}if(!(i=0|t[n+8>>2]))break;n=i}if(154==(0|b)&&(d=n+4|0,0==(8&t[n+12>>2]|0))&&B>>>0>u>>>0&A>>>0<=u>>>0){t[d>>2]=r+a,I=(0|t[20576])+a|0,d=0==(u+8&7|0)?0:0-(u+8)&7,t[20579]=u+d,t[20576]=I-d,t[u+d+4>>2]=I-d|1,t[u+I+4>>2]=40,t[20580]=t[20695];break}for(B>>>0<(0|t[20577])>>>0&&(t[20577]=B),i=B+a|0,r=82740;;){if((0|t[r>>2])==(0|i)){b=162;break}if(!(A=0|t[r+8>>2]))break;r=A}if(162==(0|b)&&0==(8&t[r+12>>2]|0)){t[r>>2]=B,t[(c=r+4|0)>>2]=(0|t[c>>2])+a,l=(c=B+(0==(7&(c=B+8|0)|0)?0:0-c&7)|0)+Q|0,o=(A=i+(0==(i+8&7|0)?0:0-(i+8)&7)|0)-c-Q|0,t[c+4>>2]=3|Q;e:do{if((0|u)==(0|A))I=(0|t[20576])+o|0,t[20576]=I,t[20579]=l,t[l+4>>2]=1|I;else{if((0|t[20578])==(0|A)){I=(0|t[20575])+o|0,t[20575]=I,t[20578]=l,t[l+4>>2]=1|I,t[l+I>>2]=I;break}if(1==(3&(B=0|t[A+4>>2])|0)){r:do{if(B>>>0<256){if(r=0|t[A+8>>2],(0|(i=0|t[A+12>>2]))==(0|r)){t[20573]=t[20573]&~(1<<(B>>>3));break}t[r+12>>2]=i,t[i+8>>2]=r;break}a=0|t[A+24>>2],r=0|t[A+12>>2];do{if((0|r)==(0|A)){if(r=0|t[A+16+4>>2])i=A+16+4|0;else{if(!(r=0|t[A+16>>2])){r=0;break}i=A+16|0}for(;;)if(n=0|t[(f=r+20|0)>>2])r=n,i=f;else{if(!(n=0|t[(f=r+16|0)>>2]))break;r=n,i=f}t[i>>2]=0}else I=0|t[A+8>>2],t[I+12>>2]=r,t[r+8>>2]=I}while(0);if(!a)break;i=0|t[A+28>>2];do{if((0|t[82596+(i<<2)>>2])==(0|A)){if(t[82596+(i<<2)>>2]=r,0|r)break;t[20574]=t[20574]&~(1<<i);break r}if(t[((0|t[a+16>>2])==(0|A)?a+16|0:a+20|0)>>2]=r,!r)break r}while(0);if(t[r+24>>2]=a,0|(i=0|t[A+16>>2])&&(t[r+16>>2]=i,t[i+24>>2]=r),!(i=0|t[A+16+4>>2]))break;t[r+20>>2]=i,t[i+24>>2]=r}while(0);A=A+(-8&B)|0,f=(-8&B)+o|0}else f=o;if(t[(i=A+4|0)>>2]=-2&t[i>>2],t[l+4>>2]=1|f,t[l+f>>2]=f,i=f>>>3,f>>>0<256){(A=0|t[20573])&1<<i?(A=82332+(i<<1<<2)+8|0,r=0|t[82332+(i<<1<<2)+8>>2]):(t[20573]=A|1<<i,A=82332+(i<<1<<2)+8|0,r=82332+(i<<1<<2)|0),t[A>>2]=l,t[r+12>>2]=l,t[l+8>>2]=r,t[l+12>>2]=82332+(i<<1<<2);break}A=f>>>8;do{if(A){if(f>>>0>16777215){n=31;break}n=f>>>((n=14-((520192+(A<<((A+1048320|0)>>>16&8))|0)>>>16&4|(A+1048320|0)>>>16&8|((n=A<<((A+1048320|0)>>>16&8)<<((520192+(A<<((A+1048320|0)>>>16&8))|0)>>>16&4))+245760|0)>>>16&2)+(n<<((n+245760|0)>>>16&2)>>>15)|0)+7|0)&1|n<<1}else n=0}while(0);if(r=82596+(n<<2)|0,t[l+28>>2]=n,t[l+16+4>>2]=0,t[l+16>>2]=0,!((A=0|t[20574])&(i=1<<n))){t[20574]=A|i,t[r>>2]=l,t[l+24>>2]=r,t[l+12>>2]=l,t[l+8>>2]=l;break}A=0|t[r>>2];r:do{if((-8&t[A+4>>2]|0)!=(0|f)){for(n=f<<(31==(0|n)?0:25-(n>>>1)|0);r=0|t[(i=A+16+(n>>>31<<2)|0)>>2];){if((-8&t[r+4>>2]|0)==(0|f)){A=r;break r}n<<=1,A=r}t[i>>2]=l,t[l+24>>2]=A,t[l+12>>2]=l,t[l+8>>2]=l;break e}}while(0);I=0|t[(d=A+8|0)>>2],t[I+12>>2]=l,t[d>>2]=l,t[l+8>>2]=I,t[l+12>>2]=A,t[l+24>>2]=0}}while(0);return G=e,0|(I=c+8|0)}for(r=82740;!((A=0|t[r>>2])>>>0<=u>>>0&&(I=A+(0|t[r+4>>2])|0)>>>0>u>>>0);)r=0|t[r+8>>2];f=(f=I+-47+(0==(I+-47+8&7|0)?0:0-(I+-47+8)&7)|0)>>>0<(u+16|0)>>>0?u:f,A=a+-40|0,d=B+(C=0==(7&(C=B+8|0)|0)?0:0-C&7)|0,t[20579]=d,t[20576]=A-C,t[d+4>>2]=A-C|1,t[B+A+4>>2]=40,t[20580]=t[20695],t[f+4>>2]=27,t[f+8>>2]=t[20685],t[f+8+4>>2]=t[20686],t[f+8+8>>2]=t[20687],t[f+8+12>>2]=t[20688],t[20685]=B,t[20686]=a,t[20688]=0,t[20687]=f+8,A=f+24|0;do{d=A,t[(A=A+4|0)>>2]=7}while((d+8|0)>>>0<I>>>0);if((0|f)!=(0|u)){if(t[f+4>>2]=-2&t[f+4>>2],t[u+4>>2]=f-u|1,t[f>>2]=f-u,(f-u|0)>>>0<256){i=82332+((f-u|0)>>>3<<1<<2)|0,(A=0|t[20573])&1<<((f-u|0)>>>3)?(A=i+8|0,r=0|t[i+8>>2]):(t[20573]=A|1<<((f-u|0)>>>3),A=i+8|0,r=i),t[A>>2]=u,t[r+12>>2]=u,t[u+8>>2]=r,t[u+12>>2]=i;break}if(r=82596+((n=(f-u|0)>>>8?(f-u|0)>>>0>16777215?31:(f-u|0)>>>((n=14-(((n=(f-u|0)>>>8<<((1048320+((f-u|0)>>>8)|0)>>>16&8))+520192|0)>>>16&4|(1048320+((f-u|0)>>>8)|0)>>>16&8|(245760+(n<<((n+520192|0)>>>16&4))|0)>>>16&2)+(n<<((n+520192|0)>>>16&4)<<((245760+(n<<((n+520192|0)>>>16&4))|0)>>>16&2)>>>15)|0)+7|0)&1|n<<1:0)<<2)|0,t[u+28>>2]=n,t[u+20>>2]=0,t[u+16>>2]=0,!((A=0|t[20574])&(i=1<<n))){t[20574]=A|i,t[r>>2]=u,t[u+24>>2]=r,t[u+12>>2]=u,t[u+8>>2]=u;break}A=0|t[r>>2];e:do{if((-8&t[A+4>>2]|0)!=(f-u|0)){for(n=f-u<<(31==(0|n)?0:25-(n>>>1)|0);r=0|t[(i=A+16+(n>>>31<<2)|0)>>2];){if((-8&t[r+4>>2]|0)==(f-u|0)){A=r;break e}n<<=1,A=r}t[i>>2]=u,t[u+24>>2]=A,t[u+12>>2]=u,t[u+8>>2]=u;break A}}while(0);I=0|t[(d=A+8|0)>>2],t[I+12>>2]=u,t[d>>2]=u,t[u+8>>2]=I,t[u+12>>2]=A,t[u+24>>2]=0}}else 0==(0|(I=0|t[20577]))|B>>>0<I>>>0&&(t[20577]=B),t[20685]=B,t[20686]=a,t[20688]=0,t[20582]=t[20691],t[20581]=-1,t[20586]=82332,t[20585]=82332,t[20588]=82340,t[20587]=82340,t[20590]=82348,t[20589]=82348,t[20592]=82356,t[20591]=82356,t[20594]=82364,t[20593]=82364,t[20596]=82372,t[20595]=82372,t[20598]=82380,t[20597]=82380,t[20600]=82388,t[20599]=82388,t[20602]=82396,t[20601]=82396,t[20604]=82404,t[20603]=82404,t[20606]=82412,t[20605]=82412,t[20608]=82420,t[20607]=82420,t[20610]=82428,t[20609]=82428,t[20612]=82436,t[20611]=82436,t[20614]=82444,t[20613]=82444,t[20616]=82452,t[20615]=82452,t[20618]=82460,t[20617]=82460,t[20620]=82468,t[20619]=82468,t[20622]=82476,t[20621]=82476,t[20624]=82484,t[20623]=82484,t[20626]=82492,t[20625]=82492,t[20628]=82500,t[20627]=82500,t[20630]=82508,t[20629]=82508,t[20632]=82516,t[20631]=82516,t[20634]=82524,t[20633]=82524,t[20636]=82532,t[20635]=82532,t[20638]=82540,t[20637]=82540,t[20640]=82548,t[20639]=82548,t[20642]=82556,t[20641]=82556,t[20644]=82564,t[20643]=82564,t[20646]=82572,t[20645]=82572,t[20648]=82580,t[20647]=82580,I=a+-40|0,d=B+(C=0==(7&(C=B+8|0)|0)?0:0-C&7)|0,t[20579]=d,t[20576]=I-C,t[d+4>>2]=I-C|1,t[B+I+4>>2]=40,t[20580]=t[20695]}while(0);if((A=0|t[20576])>>>0>Q>>>0)return C=A-Q|0,t[20576]=C,d=(I=0|t[20579])+Q|0,t[20579]=d,t[d+4>>2]=1|C,t[I+4>>2]=3|Q,G=e,0|(I=I+8|0)}return t[20571]=48,G=e,0|(I=0)}function af(A){var e=0,r=0,i=0,n=0,f=0,a=0,B=0,o=0,u=0;if(A|=0){e=0|t[20577],u=A+-8+(-8&(r=0|t[A+-4>>2]))|0;do{if(1&r)B=A+-8|0,o=A+-8|0,A=-8&r;else{if(i=0|t[A+-8>>2],!(3&r))return;if(a=i+(-8&r)|0,(f=A+-8+(0-i)|0)>>>0<e>>>0)return;if((0|t[20578])==(0|f)){if(3!=(3&(A=0|t[u+4>>2])|0)){B=f,o=f,A=a;break}return t[20575]=a,t[u+4>>2]=-2&A,t[f+4>>2]=1|a,void(t[f+a>>2]=a)}if(i>>>0<256){if(A=0|t[f+8>>2],(0|(e=0|t[f+12>>2]))==(0|A)){t[20573]=t[20573]&~(1<<(i>>>3)),B=f,o=f,A=a;break}t[A+12>>2]=e,t[e+8>>2]=A,B=f,o=f,A=a;break}n=0|t[f+24>>2],A=0|t[f+12>>2];do{if((0|A)==(0|f)){if(A=0|t[f+16+4>>2])e=f+16+4|0;else{if(!(A=0|t[f+16>>2])){A=0;break}e=f+16|0}for(;;)if(r=0|t[(i=A+20|0)>>2])A=r,e=i;else{if(!(r=0|t[(i=A+16|0)>>2]))break;A=r,e=i}t[e>>2]=0}else o=0|t[f+8>>2],t[o+12>>2]=A,t[A+8>>2]=o}while(0);if(n){if(e=0|t[f+28>>2],(0|t[82596+(e<<2)>>2])==(0|f)){if(t[82596+(e<<2)>>2]=A,!A){t[20574]=t[20574]&~(1<<e),B=f,o=f,A=a;break}}else if(t[((0|t[n+16>>2])==(0|f)?n+16|0:n+20|0)>>2]=A,!A){B=f,o=f,A=a;break}t[A+24>>2]=n,0|(e=0|t[f+16>>2])&&(t[A+16>>2]=e,t[e+24>>2]=A),(e=0|t[f+16+4>>2])?(t[A+20>>2]=e,t[e+24>>2]=A,B=f,o=f,A=a):(B=f,o=f,A=a)}else B=f,o=f,A=a}}while(0);if(!(B>>>0>=u>>>0)&&1&(r=0|t[u+4>>2])){if(2&r)t[u+4>>2]=-2&r,t[o+4>>2]=1|A,t[B+A>>2]=A,n=A;else{if((0|t[20579])==(0|u)){if(u=(0|t[20576])+A|0,t[20576]=u,t[20579]=o,t[o+4>>2]=1|u,(0|o)!=(0|t[20578]))return;return t[20578]=0,void(t[20575]=0)}if((0|t[20578])==(0|u))return u=(0|t[20575])+A|0,t[20575]=u,t[20578]=B,t[o+4>>2]=1|u,void(t[B+u>>2]=u);n=(-8&r)+A|0;do{if(r>>>0<256){if(e=0|t[u+8>>2],(0|(A=0|t[u+12>>2]))==(0|e)){t[20573]=t[20573]&~(1<<(r>>>3));break}t[e+12>>2]=A,t[A+8>>2]=e;break}f=0|t[u+24>>2],A=0|t[u+12>>2];do{if((0|A)==(0|u)){if(A=0|t[u+16+4>>2])e=u+16+4|0;else{if(!(A=0|t[u+16>>2])){e=0;break}e=u+16|0}for(;;)if(r=0|t[(i=A+20|0)>>2])A=r,e=i;else{if(!(r=0|t[(i=A+16|0)>>2]))break;A=r,e=i}t[e>>2]=0,e=A}else e=0|t[u+8>>2],t[e+12>>2]=A,t[A+8>>2]=e,e=A}while(0);if(0|f){if(A=0|t[u+28>>2],(0|t[82596+(A<<2)>>2])==(0|u)){if(t[82596+(A<<2)>>2]=e,!e){t[20574]=t[20574]&~(1<<A);break}}else if(t[((0|t[f+16>>2])==(0|u)?f+16|0:f+20|0)>>2]=e,!e)break;t[e+24>>2]=f,0|(A=0|t[u+16>>2])&&(t[e+16>>2]=A,t[A+24>>2]=e),0|(A=0|t[u+16+4>>2])&&(t[e+20>>2]=A,t[A+24>>2]=e)}}while(0);if(t[o+4>>2]=1|n,t[B+n>>2]=n,(0|o)==(0|t[20578]))return void(t[20575]=n)}if(r=n>>>3,n>>>0<256)return(A=0|t[20573])&1<<r?(A=82332+(r<<1<<2)+8|0,e=0|t[82332+(r<<1<<2)+8>>2]):(t[20573]=A|1<<r,A=82332+(r<<1<<2)+8|0,e=82332+(r<<1<<2)|0),t[A>>2]=o,t[e+12>>2]=o,t[o+8>>2]=e,void(t[o+12>>2]=82332+(r<<1<<2));e=82596+((i=(A=n>>>8)?n>>>0>16777215?31:n>>>((i=14-((520192+(A<<((A+1048320|0)>>>16&8))|0)>>>16&4|(A+1048320|0)>>>16&8|((i=A<<((A+1048320|0)>>>16&8)<<((520192+(A<<((A+1048320|0)>>>16&8))|0)>>>16&4))+245760|0)>>>16&2)+(i<<((i+245760|0)>>>16&2)>>>15)|0)+7|0)&1|i<<1:0)<<2)|0,t[o+28>>2]=i,t[o+20>>2]=0,t[o+16>>2]=0,A=0|t[20574],r=1<<i;A:do{if(A&r){A=0|t[e>>2];e:do{if((-8&t[A+4>>2]|0)!=(0|n)){for(i=n<<(31==(0|i)?0:25-(i>>>1)|0);e=0|t[(r=A+16+(i>>>31<<2)|0)>>2];){if((-8&t[e+4>>2]|0)==(0|n)){A=e;break e}i<<=1,A=e}t[r>>2]=o,t[o+24>>2]=A,t[o+12>>2]=o,t[o+8>>2]=o;break A}}while(0);u=0|t[(B=A+8|0)>>2],t[u+12>>2]=o,t[B>>2]=o,t[o+8>>2]=u,t[o+12>>2]=A,t[o+24>>2]=0}else t[20574]=A|r,t[e>>2]=o,t[o+24>>2]=e,t[o+12>>2]=o,t[o+8>>2]=o}while(0);if(u=(0|t[20581])-1|0,t[20581]=u,!(0|u)){for(A=82748;A=0|t[A>>2];)A=A+8|0;t[20581]=-1}}}}function Bf(A,e){e|=0;var r=0,i=0;return(A|=0)?e>>>0>4294967231?(t[20571]=48,0|(A=0)):0|(r=0|function(A,e){e|=0;var r,i,n,f,a=0,B=0,o=0,u=0,l=0;if(f=0|t[(A=A|0)+4>>2],r=A+(-8&f)|0,!(3&f))return e>>>0<256?0|(A=0):(-8&f)>>>0>=(e+4|0)>>>0&&((-8&f)-e|0)>>>0<=t[20693]<<1>>>0?0|A:0|(A=0);if((-8&f)>>>0>=e>>>0)return((-8&f)-e|0)>>>0<=15||(t[A+4>>2]=1&f|e|2,t[A+e+4>>2]=(-8&f)-e|3,t[r+4>>2]=1|t[r+4>>2],of(A+e|0,(-8&f)-e|0)),0|A;if((0|t[20579])==(0|r))return(a=(0|t[20576])+(-8&f)|0)>>>0<=e>>>0?0|(A=0):(t[A+4>>2]=1&f|e|2,t[A+e+4>>2]=a-e|1,t[20579]=A+e,t[20576]=a-e,0|A);if((0|t[20578])==(0|r))return(a=(0|t[20575])+(-8&f)|0)>>>0<e>>>0?0|(A=0):((a-e|0)>>>0>15?(t[A+4>>2]=1&f|e|2,t[A+e+4>>2]=a-e|1,t[A+a>>2]=a-e,t[A+a+4>>2]=-2&t[A+a+4>>2],B=A+e|0,a=a-e|0):(t[A+4>>2]=1&f|a|2,t[A+a+4>>2]=1|t[A+a+4>>2],B=0,a=0),t[20575]=a,t[20578]=B,0|A);if(2&(i=0|t[r+4>>2])|0)return 0|(A=0);if(((-8&i)+(-8&f)|0)>>>0<e>>>0)return 0|(A=0);n=(-8&i)+(-8&f)-e|0;do{if(i>>>0<256){if(B=0|t[r+8>>2],(0|(a=0|t[r+12>>2]))==(0|B)){t[20573]=t[20573]&~(1<<(i>>>3));break}t[B+12>>2]=a,t[a+8>>2]=B;break}l=0|t[r+24>>2],a=0|t[r+12>>2];do{if((0|a)==(0|r)){if(a=0|t[r+16+4>>2])B=r+16+4|0;else{if(!(a=0|t[r+16>>2])){B=0;break}B=r+16|0}for(;;)if(o=0|t[(u=a+20|0)>>2])a=o,B=u;else{if(!(o=0|t[(u=a+16|0)>>2]))break;a=o,B=u}t[B>>2]=0,B=a}else B=0|t[r+8>>2],t[B+12>>2]=a,t[a+8>>2]=B,B=a}while(0);if(0|l){if(a=0|t[r+28>>2],(0|t[82596+(a<<2)>>2])==(0|r)){if(t[82596+(a<<2)>>2]=B,!B){t[20574]=t[20574]&~(1<<a);break}}else if(t[((0|t[l+16>>2])==(0|r)?l+16|0:l+20|0)>>2]=B,!B)break;t[B+24>>2]=l,0|(a=0|t[r+16>>2])&&(t[B+16>>2]=a,t[a+24>>2]=B),0|(a=0|t[r+16+4>>2])&&(t[B+20>>2]=a,t[a+24>>2]=B)}}while(0);return n>>>0<16?(t[A+4>>2]=1&f|(-8&i)+(-8&f)|2,t[A+((-8&i)+(-8&f))+4>>2]=1|t[A+((-8&i)+(-8&f))+4>>2],0|A):(t[A+4>>2]=1&f|e|2,t[A+e+4>>2]=3|n,t[A+((-8&i)+(-8&f))+4>>2]=1|t[A+((-8&i)+(-8&f))+4>>2],of(A+e|0,n),0|A)}(A+-8|0,e>>>0<11?16:e+11&-8))?0|(A=r+8|0):(r=0|ff(e))?(If(0|r,0|A,0|((i=(-8&(i=0|t[A+-4>>2]))-(0==(3&i|0)?8:4)|0)>>>0<e>>>0?i:e)),af(A),0|(A=r)):0|(A=0):0|(A=0|ff(e))}function of(A,e){e|=0;var r=0,i=0,n=0,f=0,a=0,B=0,o=0;r=0|t[(A|=0)+4>>2];do{if(1&r)o=A,r=e;else{if(B=0|t[A>>2],!(3&r))return;if((0|t[20578])==(A+(0-B)|0)){if(3!=(3&(r=0|t[A+e+4>>2])|0)){o=A+(0-B)|0,r=B+e|0;break}return t[20575]=B+e,t[A+e+4>>2]=-2&r,t[A+(0-B)+4>>2]=B+e|1,void(t[A+e>>2]=B+e)}if(B>>>0<256){if(r=0|t[A+(0-B)+8>>2],(0|(i=0|t[A+(0-B)+12>>2]))==(0|r)){t[20573]=t[20573]&~(1<<(B>>>3)),o=A+(0-B)|0,r=B+e|0;break}t[r+12>>2]=i,t[i+8>>2]=r,o=A+(0-B)|0,r=B+e|0;break}a=0|t[A+(0-B)+24>>2],r=0|t[A+(0-B)+12>>2];do{if((0|r)==(A+(0-B)|0)){if(r=0|t[(i=A+(0-B)+16|0)+4>>2])i=i+4|0;else if(!(r=0|t[i>>2])){i=0;break}for(;;)if(n=0|t[(f=r+20|0)>>2])r=n,i=f;else{if(!(n=0|t[(f=r+16|0)>>2]))break;r=n,i=f}t[i>>2]=0,i=r}else i=0|t[A+(0-B)+8>>2],t[i+12>>2]=r,t[r+8>>2]=i,i=r}while(0);if(a){if(r=0|t[A+(0-B)+28>>2],(0|t[82596+(r<<2)>>2])==(A+(0-B)|0)){if(t[82596+(r<<2)>>2]=i,!i){t[20574]=t[20574]&~(1<<r),o=A+(0-B)|0,r=B+e|0;break}}else if(t[((0|t[a+16>>2])==(A+(0-B)|0)?a+16|0:a+20|0)>>2]=i,!i){o=A+(0-B)|0,r=B+e|0;break}t[i+24>>2]=a,0|(r=0|t[A+(0-B)+16>>2])&&(t[i+16>>2]=r,t[r+24>>2]=i),(r=0|t[A+(0-B)+16+4>>2])?(t[i+20>>2]=r,t[r+24>>2]=i,o=A+(0-B)|0,r=B+e|0):(o=A+(0-B)|0,r=B+e|0)}else o=A+(0-B)|0,r=B+e|0}}while(0);if(2&(n=0|t[A+e+4>>2]))t[A+e+4>>2]=-2&n,t[o+4>>2]=1|r,t[o+r>>2]=r,a=r;else{if((0|t[20579])==(A+e|0)){if(e=(0|t[20576])+r|0,t[20576]=e,t[20579]=o,t[o+4>>2]=1|e,(0|o)!=(0|t[20578]))return;return t[20578]=0,void(t[20575]=0)}if((0|t[20578])==(A+e|0))return e=(0|t[20575])+r|0,t[20575]=e,t[20578]=o,t[o+4>>2]=1|e,void(t[o+e>>2]=e);a=(-8&n)+r|0;do{if(n>>>0<256){if(i=0|t[A+e+8>>2],(0|(r=0|t[A+e+12>>2]))==(0|i)){t[20573]=t[20573]&~(1<<(n>>>3));break}t[i+12>>2]=r,t[r+8>>2]=i;break}B=0|t[A+e+24>>2],r=0|t[A+e+12>>2];do{if((0|r)==(A+e|0)){if(r=0|t[A+e+16+4>>2])i=A+e+16+4|0;else{if(!(r=0|t[A+e+16>>2])){i=0;break}i=A+e+16|0}for(;;)if(n=0|t[(f=r+20|0)>>2])r=n,i=f;else{if(!(n=0|t[(f=r+16|0)>>2]))break;r=n,i=f}t[i>>2]=0,i=r}else i=0|t[A+e+8>>2],t[i+12>>2]=r,t[r+8>>2]=i,i=r}while(0);if(0|B){if(r=0|t[A+e+28>>2],(0|t[82596+(r<<2)>>2])==(A+e|0)){if(t[82596+(r<<2)>>2]=i,!i){t[20574]=t[20574]&~(1<<r);break}}else if(t[((0|t[B+16>>2])==(A+e|0)?B+16|0:B+20|0)>>2]=i,!i)break;t[i+24>>2]=B,0|(r=0|t[A+e+16>>2])&&(t[i+16>>2]=r,t[r+24>>2]=i),0|(r=0|t[A+e+16+4>>2])&&(t[i+20>>2]=r,t[r+24>>2]=i)}}while(0);if(t[o+4>>2]=1|a,t[o+a>>2]=a,(0|o)==(0|t[20578]))return void(t[20575]=a)}if(n=a>>>3,a>>>0<256)return(r=0|t[20573])&1<<n?(r=82332+(n<<1<<2)+8|0,i=0|t[82332+(n<<1<<2)+8>>2]):(t[20573]=r|1<<n,r=82332+(n<<1<<2)+8|0,i=82332+(n<<1<<2)|0),t[r>>2]=o,t[i+12>>2]=o,t[o+8>>2]=i,void(t[o+12>>2]=82332+(n<<1<<2));if(i=82596+((f=(r=a>>>8)?a>>>0>16777215?31:a>>>((f=14-((520192+(r<<((r+1048320|0)>>>16&8))|0)>>>16&4|(r+1048320|0)>>>16&8|((f=r<<((r+1048320|0)>>>16&8)<<((520192+(r<<((r+1048320|0)>>>16&8))|0)>>>16&4))+245760|0)>>>16&2)+(f<<((f+245760|0)>>>16&2)>>>15)|0)+7|0)&1|f<<1:0)<<2)|0,t[o+28>>2]=f,t[o+20>>2]=0,t[o+16>>2]=0,!((r=0|t[20574])&(n=1<<f)))return t[20574]=r|n,t[i>>2]=o,t[o+24>>2]=i,t[o+12>>2]=o,void(t[o+8>>2]=o);r=0|t[i>>2];A:do{if((-8&t[r+4>>2]|0)!=(0|a)){for(f=a<<(31==(0|f)?0:25-(f>>>1)|0);i=0|t[(n=r+16+(f>>>31<<2)|0)>>2];){if((-8&t[i+4>>2]|0)==(0|a)){r=i;break A}f<<=1,r=i}return t[n>>2]=o,t[o+24>>2]=r,t[o+12>>2]=o,void(t[o+8>>2]=o)}}while(0);e=0|t[(A=r+8|0)>>2],t[e+12>>2]=o,t[A>>2]=o,t[o+8>>2]=e,t[o+12>>2]=r,t[o+24>>2]=0}function uf(A){A|=0;var e,r;r=(e=0|t[20704])+(A+3&-4)|0;do{if((A+3&-4|0)<1|r>>>0>e>>>0){if(r>>>0>(0|g())>>>0&&0==(0|M(0|r)))break;return t[20704]=r,0|(A=e)}}while(0);return t[20571]=48,0|(A=-1)}function lf(A,e,r,i){e|=0,i|=0;var n,t;return n=0|function(A,e){var r,i,n;return n=((r=0|o(65535&(e|=0),65535&(A|=0)))>>>16)+(0|o(65535&e,A>>>16))|0,i=0|o(e>>>16,65535&A),0|(c((n>>>16)+(0|o(e>>>16,A>>>16))+(((65535&n)+i|0)>>>16)|0),n+i<<16|65535&r|0)}(A|=0,r|=0),t=0|E(),0|(c((0|o(e,r))+(0|o(i,A))+t|0&t|0),0|n)}function cf(A,e,r,i){return 0|(c((e|=0)+(i|=0)+((A|=0)+(r|=0)>>>0>>>0<A>>>0|0)>>>0|0),A+r>>>0|0)}function Ef(A,e,r,i){return 0|(c(0|(i=(e|=0)-(i|=0)-((r|=0)>>>0>(A|=0)>>>0|0)>>>0)),A-r>>>0|0)}function Qf(A){return 0|((A|=0)?31-(0|u(A^A-1))|0:32)}function wf(A,e,r,i,n){A|=0,r|=0,i|=0,n|=0;var f=0,a=0,B=0,o=0,l=0,Q=0,w=0,b=0,k=0,s=0;if(!(e|=0))return i?n?(t[n>>2]=0|A,t[n+4>>2]=0&e,n=0,0|(c(0|(i=0)),n)):(n=0,0|(c(0|(i=0)),n)):(0|n&&(t[n>>2]=(A>>>0)%(r>>>0),t[n+4>>2]=0),n=(A>>>0)/(r>>>0)>>>0,0|(c(0|(i=0)),n));do{if(r){if(0|i){if((a=(0|u(0|i))-(0|u(0|e))|0)>>>0<=31){b=a+1|0,B=A>>>((a+1|0)>>>0)&a-31>>31|e<<31-a,w=e>>>((a+1|0)>>>0)&a-31>>31,f=0,a=A<<31-a;break}return n?(t[n>>2]=0|A,t[n+4>>2]=e|0&e,n=0,0|(c(0|(i=0)),n)):(n=0,0|(c(0|(i=0)),n))}if(r-1&r|0){b=a=33+(0|u(0|r))-(0|u(0|e))|0,B=32-a-1>>31&e>>>((a-32|0)>>>0)|(e<<32-a|A>>>(a>>>0))&a-32>>31,w=a-32>>31&e>>>(a>>>0),f=A<<64-a&32-a>>31,a=(e<<64-a|A>>>((a-32|0)>>>0))&32-a>>31|A<<32-a&a-33>>31;break}return 0|n&&(t[n>>2]=r-1&A,t[n+4>>2]=0),1==(0|r)?(n=0|A,0|(c(0|(i=e|0&e)),n)):(i=e>>>((n=0|Qf(0|r))>>>0)|0,n=e<<32-n|A>>>(n>>>0)|0,0|(c(0|i),n))}if(!i)return 0|n&&(t[n>>2]=(e>>>0)%(r>>>0),t[n+4>>2]=0),n=(e>>>0)/(r>>>0)>>>0,0|(c(0|(i=0)),n);if(!A)return 0|n&&(t[n>>2]=0,t[n+4>>2]=(e>>>0)%(i>>>0)),n=(e>>>0)/(i>>>0)>>>0,0|(c(0|(r=0)),n);if(!(i-1&i))return 0|n&&(t[n>>2]=0|A,t[n+4>>2]=i-1&e|0&e),r=0,n=e>>>((0|Qf(0|i))>>>0),0|(c(0|r),n);if((a=(0|u(0|i))-(0|u(0|e))|0)>>>0<=30){b=a+1|0,B=e<<31-a|A>>>((a+1|0)>>>0),w=e>>>((a+1|0)>>>0),f=0,a=A<<31-a;break}return n?(t[n>>2]=0|A,t[n+4>>2]=e|0&e,n=0,0|(c(0|(i=0)),n)):(n=0,0|(c(0|(i=0)),n))}while(0);if(b){l=0|cf(0|r,i|0&i|0,-1,-1),Q=0|E(),o=a,e=w,A=b,a=0;do{s=o,o=f>>>31|o<<1,f=a|f<<1,Ef(0|l,0|Q,0|(s=B<<1|s>>>31|0),0|(k=B>>>31|e<<1|0)),a=1&(w=(b=0|E())>>31|((0|b)<0?-1:0)<<1),B=0|Ef(0|s,0|k,w&(0|r)|0,(((0|b)<0?-1:0)>>31|((0|b)<0?-1:0)<<1)&(i|0&i)|0),e=0|E(),A=A-1|0}while(0!=(0|A));A=0}else o=a,e=w,A=0,a=0;return 0|n&&(t[n>>2]=B,t[n+4>>2]=e),s=-2&(f<<1|0)|a,0|(c(0|(k=(0|f)>>>31|o<<1|0&(0|f>>>31)|A)),s)}function bf(A,e,r,i){return 0|wf(A|=0,e|=0,r|=0,i|=0,0)}function kf(A,e,r,i){var n;return n=G,G=G+16|0,wf(A|=0,e|=0,r|=0,i|=0,0|n),G=n,0|(c(0|t[n+4>>2]),0|t[n>>2])}function sf(A,e,r){return A|=0,e|=0,(0|(r|=0))<32?(c(e>>>r|0),A>>>r|(e&(1<<r)-1)<<32-r):(c(0),e>>>r-32|0)}function Cf(A,e,r){return A|=0,e|=0,(0|(r|=0))<32?(c(e<<r|(A&(1<<r)-1<<32-r)>>>32-r|0),A<<r):(c(A<<r-32|0),0)}function df(){return 82816}function If(A,e,r){A|=0,e|=0;var n,f,a=0;if((0|(r|=0))>=512)return D(0|A,0|e,0|r),0|A;if(f=0|A,n=A+r|0,(3&A)==(3&e)){for(;3&A;){if(!r)return 0|f;i[A>>0]=0|i[e>>0],A=A+1|0,e=e+1|0,r=r-1|0}for(a=(r=-4&n|0)-64|0;(0|A)<=(0|a);)t[A>>2]=t[e>>2],t[A+4>>2]=t[e+4>>2],t[A+8>>2]=t[e+8>>2],t[A+12>>2]=t[e+12>>2],t[A+16>>2]=t[e+16>>2],t[A+20>>2]=t[e+20>>2],t[A+24>>2]=t[e+24>>2],t[A+28>>2]=t[e+28>>2],t[A+32>>2]=t[e+32>>2],t[A+36>>2]=t[e+36>>2],t[A+40>>2]=t[e+40>>2],t[A+44>>2]=t[e+44>>2],t[A+48>>2]=t[e+48>>2],t[A+52>>2]=t[e+52>>2],t[A+56>>2]=t[e+56>>2],t[A+60>>2]=t[e+60>>2],A=A+64|0,e=e+64|0;for(;(0|A)<(0|r);)t[A>>2]=t[e>>2],A=A+4|0,e=e+4|0}else for(r=n-4|0;(0|A)<(0|r);)i[A>>0]=0|i[e>>0],i[A+1>>0]=0|i[e+1>>0],i[A+2>>0]=0|i[e+2>>0],i[A+3>>0]=0|i[e+3>>0],A=A+4|0,e=e+4|0;for(;(0|A)<(0|n);)i[A>>0]=0|i[e>>0],A=A+1|0,e=e+1|0;return 0|f}function gf(A,e,r){var n=0;if((0|(e|=0))<(0|(A|=0))&(0|A)<(e+(r|=0)|0)){for(n=A,e=e+r|0,A=A+r|0;(0|r)>0;)e=e-1|0,r=r-1|0,i[(A=A-1|0)>>0]=0|i[e>>0];A=n}else If(A,e,r);return 0|A}function Df(A,e,r){e|=0;var n,f=0;if(n=(A|=0)+(r|=0)|0,e&=255,(0|r)>=67){for(;3&A;)i[A>>0]=e,A=A+1|0;for(f=e|e<<8|e<<16|e<<24;(0|A)<=((-4&n)-64|0);)t[A>>2]=f,t[A+4>>2]=f,t[A+8>>2]=f,t[A+12>>2]=f,t[A+16>>2]=f,t[A+20>>2]=f,t[A+24>>2]=f,t[A+28>>2]=f,t[A+32>>2]=f,t[A+36>>2]=f,t[A+40>>2]=f,t[A+44>>2]=f,t[A+48>>2]=f,t[A+52>>2]=f,t[A+56>>2]=f,t[A+60>>2]=f,A=A+64|0;for(;(0|A)<(-4&n|0);)t[A>>2]=f,A=A+4|0}for(;(0|A)<(0|n);)i[A>>0]=e,A=A+1|0;return n-r|0}function Mf(A){return 0,l(0),0}function hf(A,e){return 0,0,l(1),0}function Uf(A,e,r){return 0,0,0,l(2),0}function Gf(A,e,r,i){return 0,0,0,0,l(3),0}function Zf(A,e,r,i,n){return 0,0,0,0,0,l(4),0}function Nf(A,e,r,i,n,t){return 0,0,0,0,0,0,l(5),0}function Ff(){l(7)}function Wf(A){l(8)}function Yf(A,e){l(9)}function Rf(A,e,r){A|=0,e|=0;var i,n,f=0,a=0,B=0,o=0,u=0,l=0,c=0,Q=0,w=0,b=0;w=0|lf(0|t[(r|=0)>>2],0,0|t[e>>2],0),a=0|E(),t[A>>2]=w,a=0|cf(0|(w=0|lf(0|t[r+4>>2],0,0|t[e>>2],0)),0|E(),0|a,0),w=0|E(),a=0|cf(0|(f=0|lf(0|t[r>>2],0,0|t[e+4>>2],0)),0|E(),0|a,0),f=0|E(),t[A+4>>2]=a,a=0|cf(0|(a=0|lf(0|t[r>>2],0,0|t[e+8>>2],0)),0|E(),f+w|0,0),Q=((b=0|E())+((f+w|0)>>>0<f>>>0&1)|0)>>>0<b>>>0,B=0|cf(0|a,0,0|(B=0|lf(0|t[r+4>>2],0,0|t[e+4>>2],0)),0|E()),f=b+((f+w|0)>>>0<f>>>0&1)+(a=0|E())|0,w=0|cf(0|B,0,0|(w=0|lf(0|t[r+8>>2],0,0|t[e>>2],0)),0|E()),B=0|E(),t[A+8>>2]=w,w=0|cf(0|(w=0|lf(0|t[r+12>>2],0,0|t[e>>2],0)),0|E(),f+B|0,0),B=(f>>>0<a>>>0?Q?2:1:1&Q)+((f+B|0)>>>0<B>>>0&1)+(b=0|E())|0,f=0|cf(0|w,0,0|(f=0|lf(0|t[r+8>>2],0,0|t[e+4>>2],0)),0|E()),w=0|E(),Q=0|cf(0|f,0,0|(Q=0|lf(0|t[r+4>>2],0,0|t[e+8>>2],0)),0|E()),f=0|E(),a=0|cf(0|Q,0,0|(a=0|lf(0|t[r>>2],0,0|t[e+12>>2],0)),0|E()),Q=0|E(),t[A+12>>2]=a,a=0|cf(0|(a=0|lf(0|t[r>>2],0,0|t[e+16>>2],0)),0|E(),B+w+f+Q|0,0),Q=((B+w|0)>>>0<w>>>0?B>>>0<b>>>0?2:1:B>>>0<b>>>0&1)+((B+w+f|0)>>>0<f>>>0&1)+((B+w+f+Q|0)>>>0<Q>>>0&1)+(l=0|E())|0,f=0|cf(0|a,0,0|(f=0|lf(0|t[r+4>>2],0,0|t[e+12>>2],0)),0|E()),a=0|E(),w=0|cf(0|f,0,0|(w=0|lf(0|t[r+8>>2],0,0|t[e+8>>2],0)),0|E()),f=0|E(),B=0|cf(0|w,0,0|(B=0|lf(0|t[r+12>>2],0,0|t[e+4>>2],0)),0|E()),w=0|E(),b=0|cf(0|B,0,0|(b=0|lf(0|t[r+16>>2],0,0|t[e>>2],0)),0|E()),B=0|E(),t[A+16>>2]=b,b=0|cf(0|(b=0|lf(0|t[r+20>>2],0,0|t[e>>2],0)),0|E(),Q+a+f+w+B|0,0),B=((Q+a|0)>>>0<a>>>0?Q>>>0<l>>>0?2:1:Q>>>0<l>>>0&1)+((Q+a+f|0)>>>0<f>>>0&1)+((Q+a+f+w|0)>>>0<w>>>0&1)+((Q+a+f+w+B|0)>>>0<B>>>0&1)+(c=0|E())|0,w=0|cf(0|b,0,0|(w=0|lf(0|t[r+16>>2],0,0|t[e+4>>2],0)),0|E()),b=0|E(),f=0|cf(0|w,0,0|(f=0|lf(0|t[r+12>>2],0,0|t[e+8>>2],0)),0|E()),w=0|E(),a=0|cf(0|f,0,0|(a=0|lf(0|t[r+8>>2],0,0|t[e+12>>2],0)),0|E()),f=0|E(),Q=0|cf(0|a,0,0|(Q=0|lf(0|t[r+4>>2],0,0|t[e+16>>2],0)),0|E()),a=0|E(),l=0|cf(0|Q,0,0|(l=0|lf(0|t[r>>2],0,0|t[e+20>>2],0)),0|E()),o=B+b+w+f+a+(Q=0|E())|0,t[A+20>>2]=l,l=0|cf(0|(l=0|lf(0|t[r>>2],0,0|t[e+24>>2],0)),0|E(),0|o,0),Q=((B+b|0)>>>0<b>>>0?B>>>0<c>>>0?2:1:B>>>0<c>>>0&1)+((B+b+w|0)>>>0<w>>>0&1)+((B+b+w+f|0)>>>0<f>>>0&1)+((B+b+w+f+a|0)>>>0<a>>>0&1)+(o>>>0<Q>>>0&1)+(u=0|E())|0,o=0|cf(0|l,0,0|(o=0|lf(0|t[r+4>>2],0,0|t[e+20>>2],0)),0|E()),l=0|E(),a=0|cf(0|o,0,0|(a=0|lf(0|t[r+8>>2],0,0|t[e+16>>2],0)),0|E()),o=0|E(),f=0|cf(0|a,0,0|(f=0|lf(0|t[r+12>>2],0,0|t[e+12>>2],0)),0|E()),a=0|E(),w=0|cf(0|f,0,0|(w=0|lf(0|t[r+16>>2],0,0|t[e+8>>2],0)),0|E()),f=0|E(),b=0|cf(0|w,0,0|(b=0|lf(0|t[r+20>>2],0,0|t[e+4>>2],0)),0|E()),B=Q+l+o+a+f+(w=0|E())|0,c=0|cf(0|b,0,0|(c=0|lf(0|t[r+24>>2],0,0|t[e>>2],0)),0|E()),b=0|E(),t[A+24>>2]=c,c=0|cf(0|(c=0|lf(0|t[r+28>>2],0,0|t[e>>2],0)),0|E(),B+b|0,0),b=((Q+l|0)>>>0<l>>>0?Q>>>0<u>>>0?2:1:Q>>>0<u>>>0&1)+((Q+l+o|0)>>>0<o>>>0&1)+((Q+l+o+a|0)>>>0<a>>>0&1)+((Q+l+o+a+f|0)>>>0<f>>>0&1)+(B>>>0<w>>>0&1)+((B+b|0)>>>0<b>>>0&1)+(n=0|E())|0,B=0|cf(0|c,0,0|(B=0|lf(0|t[r+24>>2],0,0|t[e+4>>2],0)),0|E()),c=0|E(),w=0|cf(0|B,0,0|(w=0|lf(0|t[r+20>>2],0,0|t[e+8>>2],0)),0|E()),B=0|E(),f=0|cf(0|w,0,0|(f=0|lf(0|t[r+16>>2],0,0|t[e+12>>2],0)),0|E()),w=0|E(),a=0|cf(0|f,0,0|(a=0|lf(0|t[r+12>>2],0,0|t[e+16>>2],0)),0|E()),f=0|E(),o=0|cf(0|a,0,0|(o=0|lf(0|t[r+8>>2],0,0|t[e+20>>2],0)),0|E()),l=b+c+B+w+f+(a=0|E())|0,Q=0|cf(0|o,0,0|(Q=0|lf(0|t[r+4>>2],0,0|t[e+24>>2],0)),0|E()),o=0|E(),u=0|cf(0|Q,0,0|(u=0|lf(0|t[r>>2],0,0|t[e+28>>2],0)),0|E()),Q=0|E(),t[A+28>>2]=u,u=0|cf(0|(u=0|lf(0|t[r+4>>2],0,0|t[e+28>>2],0)),0|E(),l+o+Q|0,0),Q=((b+c|0)>>>0<c>>>0?b>>>0<n>>>0?2:1:b>>>0<n>>>0&1)+((b+c+B|0)>>>0<B>>>0&1)+((b+c+B+w|0)>>>0<w>>>0&1)+((b+c+B+w+f|0)>>>0<f>>>0&1)+(l>>>0<a>>>0&1)+((l+o|0)>>>0<o>>>0&1)+((l+o+Q|0)>>>0<Q>>>0&1)+(i=0|E())|0,o=0|cf(0|u,0,0|(o=0|lf(0|t[r+8>>2],0,0|t[e+24>>2],0)),0|E()),u=0|E(),l=0|cf(0|o,0,0|(l=0|lf(0|t[r+12>>2],0,0|t[e+20>>2],0)),0|E()),o=0|E(),a=0|cf(0|l,0,0|(a=0|lf(0|t[r+16>>2],0,0|t[e+16>>2],0)),0|E()),l=0|E(),f=0|cf(0|a,0,0|(f=0|lf(0|t[r+20>>2],0,0|t[e+12>>2],0)),0|E()),a=0|E(),w=0|cf(0|f,0,0|(w=0|lf(0|t[r+24>>2],0,0|t[e+8>>2],0)),0|E()),B=Q+u+o+l+a+(f=0|E())|0,c=0|cf(0|w,0,0|(c=0|lf(0|t[r+28>>2],0,0|t[e+4>>2],0)),0|E()),w=0|E(),t[A+32>>2]=c,c=0|cf(0|(c=0|lf(0|t[r+28>>2],0,0|t[e+8>>2],0)),0|E(),B+w|0,0),w=((Q+u|0)>>>0<u>>>0?Q>>>0<i>>>0?2:1:Q>>>0<i>>>0&1)+((Q+u+o|0)>>>0<o>>>0&1)+((Q+u+o+l|0)>>>0<l>>>0&1)+((Q+u+o+l+a|0)>>>0<a>>>0&1)+(B>>>0<f>>>0&1)+((B+w|0)>>>0<w>>>0&1)+(b=0|E())|0,B=0|cf(0|c,0,0|(B=0|lf(0|t[r+24>>2],0,0|t[e+12>>2],0)),0|E()),c=0|E(),f=0|cf(0|B,0,0|(f=0|lf(0|t[r+20>>2],0,0|t[e+16>>2],0)),0|E()),B=0|E(),a=0|cf(0|f,0,0|(a=0|lf(0|t[r+16>>2],0,0|t[e+20>>2],0)),0|E()),f=0|E(),l=0|cf(0|a,0,0|(l=0|lf(0|t[r+12>>2],0,0|t[e+24>>2],0)),0|E()),a=0|E(),o=0|cf(0|l,0,0|(o=0|lf(0|t[r+8>>2],0,0|t[e+28>>2],0)),0|E()),u=w+c+B+f+a+(l=0|E())|0,t[A+36>>2]=o,o=0|cf(0|(o=0|lf(0|t[r+12>>2],0,0|t[e+28>>2],0)),0|E(),0|u,0),l=((w+c|0)>>>0<c>>>0?w>>>0<b>>>0?2:1:w>>>0<b>>>0&1)+((w+c+B|0)>>>0<B>>>0&1)+((w+c+B+f|0)>>>0<f>>>0&1)+((w+c+B+f+a|0)>>>0<a>>>0&1)+(u>>>0<l>>>0&1)+(Q=0|E())|0,u=0|cf(0|o,0,0|(u=0|lf(0|t[r+16>>2],0,0|t[e+24>>2],0)),0|E()),o=0|E(),a=0|cf(0|u,0,0|(a=0|lf(0|t[r+20>>2],0,0|t[e+20>>2],0)),0|E()),u=0|E(),f=0|cf(0|a,0,0|(f=0|lf(0|t[r+24>>2],0,0|t[e+16>>2],0)),0|E()),a=0|E(),B=0|cf(0|f,0,0|(B=0|lf(0|t[r+28>>2],0,0|t[e+12>>2],0)),0|E()),f=0|E(),t[A+40>>2]=B,B=0|cf(0|(B=0|lf(0|t[r+28>>2],0,0|t[e+16>>2],0)),0|E(),l+o+u+a+f|0,0),f=((l+o|0)>>>0<o>>>0?l>>>0<Q>>>0?2:1:l>>>0<Q>>>0&1)+((l+o+u|0)>>>0<u>>>0&1)+((l+o+u+a|0)>>>0<a>>>0&1)+((l+o+u+a+f|0)>>>0<f>>>0&1)+(c=0|E())|0,a=0|cf(0|B,0,0|(a=0|lf(0|t[r+24>>2],0,0|t[e+20>>2],0)),0|E()),B=0|E(),u=0|cf(0|a,0,0|(u=0|lf(0|t[r+20>>2],0,0|t[e+24>>2],0)),0|E()),a=0|E(),o=0|cf(0|u,0,0|(o=0|lf(0|t[r+16>>2],0,0|t[e+28>>2],0)),0|E()),u=0|E(),t[A+44>>2]=o,o=0|cf(0|(o=0|lf(0|t[r+20>>2],0,0|t[e+28>>2],0)),0|E(),f+B+a+u|0,0),u=((f+B|0)>>>0<B>>>0?f>>>0<c>>>0?2:1:f>>>0<c>>>0&1)+((f+B+a|0)>>>0<a>>>0&1)+((f+B+a+u|0)>>>0<u>>>0&1)+(l=0|E())|0,a=0|cf(0|o,0,0|(a=0|lf(0|t[r+24>>2],0,0|t[e+24>>2],0)),0|E()),o=0|E(),B=0|cf(0|a,0,0|(B=0|lf(0|t[r+28>>2],0,0|t[e+20>>2],0)),0|E()),a=0|E(),t[A+48>>2]=B,B=0|cf(0|(B=0|lf(0|t[r+28>>2],0,0|t[e+24>>2],0)),0|E(),u+o+a|0,0),a=((u+o|0)>>>0<o>>>0?u>>>0<l>>>0?2:1:u>>>0<l>>>0&1)+((u+o+a|0)>>>0<a>>>0&1)+(f=0|E())|0,o=0|cf(0|B,0,0|(o=0|lf(0|t[r+24>>2],0,0|t[e+28>>2],0)),0|E()),B=0|E(),t[A+52>>2]=o,e=0|cf(0|(e=0|lf(0|t[r+28>>2],0,0|t[e+28>>2],0)),0|E(),a+B|0,0),r=((a+B|0)>>>0<B>>>0?a>>>0<f>>>0?2:1:a>>>0<f>>>0&1)+(0|E())|0,t[A+56>>2]=e,t[A+60>>2]=r}function yf(A,e,r){A|=0,e|=0;var i,n=0,f=0,a=0,B=0,o=0,u=0;a=0|lf(0|t[(r|=0)>>2],0,0|t[e>>2],0),B=0|E(),t[A>>2]=a,B=0|cf(0|(a=0|lf(0|t[r+4>>2],0,0|t[e>>2],0)),0|E(),0|B,0),a=0|E(),B=0|cf(0|(f=0|lf(0|t[r>>2],0,0|t[e+4>>2],0)),0|E(),0|B,0),f=0|E(),t[A+4>>2]=B,B=0|cf(0|(B=0|lf(0|t[r>>2],0,0|t[e+8>>2],0)),0|E(),f+a|0,0),o=((u=0|E())+((f+a|0)>>>0<f>>>0&1)|0)>>>0<u>>>0,n=0|cf(0|B,0,0|(n=0|lf(0|t[r+4>>2],0,0|t[e+4>>2],0)),0|E()),f=u+((f+a|0)>>>0<f>>>0&1)+(B=0|E())|0,a=0|cf(0|n,0,0|(a=0|lf(0|t[r+8>>2],0,0|t[e>>2],0)),0|E()),n=0|E(),t[A+8>>2]=a,a=0|cf(0|(a=0|lf(0|t[r+12>>2],0,0|t[e>>2],0)),0|E(),f+n|0,0),n=(f>>>0<B>>>0?o?2:1:1&o)+((f+n|0)>>>0<n>>>0&1)+(u=0|E())|0,f=0|cf(0|a,0,0|(f=0|lf(0|t[r+8>>2],0,0|t[e+4>>2],0)),0|E()),a=0|E(),o=0|cf(0|f,0,0|(o=0|lf(0|t[r+4>>2],0,0|t[e+8>>2],0)),0|E()),f=0|E(),B=0|cf(0|o,0,0|(B=0|lf(0|t[r>>2],0,0|t[e+12>>2],0)),0|E()),o=0|E(),t[A+12>>2]=B,B=0|cf(0|(B=0|lf(0|t[r+4>>2],0,0|t[e+12>>2],0)),0|E(),n+a+f+o|0,0),o=((n+a|0)>>>0<a>>>0?n>>>0<u>>>0?2:1:n>>>0<u>>>0&1)+((n+a+f|0)>>>0<f>>>0&1)+((n+a+f+o|0)>>>0<o>>>0&1)+(i=0|E())|0,f=0|cf(0|B,0,0|(f=0|lf(0|t[r+8>>2],0,0|t[e+8>>2],0)),0|E()),B=0|E(),a=0|cf(0|f,0,0|(a=0|lf(0|t[r+12>>2],0,0|t[e+4>>2],0)),0|E()),f=0|E(),t[A+16>>2]=a,a=0|cf(0|(a=0|lf(0|t[r+12>>2],0,0|t[e+8>>2],0)),0|E(),o+B+f|0,0),f=((o+B|0)>>>0<B>>>0?o>>>0<i>>>0?2:1:o>>>0<i>>>0&1)+((o+B+f|0)>>>0<f>>>0&1)+(n=0|E())|0,B=0|cf(0|a,0,0|(B=0|lf(0|t[r+8>>2],0,0|t[e+12>>2],0)),0|E()),a=0|E(),t[A+20>>2]=B,e=0|cf(0|(e=0|lf(0|t[r+12>>2],0,0|t[e+12>>2],0)),0|E(),f+a|0,0),r=((f+a|0)>>>0<a>>>0?f>>>0<n>>>0?2:1:f>>>0<n>>>0&1)+(0|E())|0,t[A+24>>2]=e,t[A+28>>2]=r}function Vf(A,e){A|=0;var r=0,i=0,n=0,f=0,a=0,B=0,o=0,u=0,l=0,c=0,Q=0,w=0,b=0;f=0|lf(0|(f=0|t[(e|=0)>>2]),0,0|f,0),l=0|E(),t[A>>2]=f,l=0|cf(0|(f=0|lf(0|t[e>>2],0,0|t[e+4>>2],0)),0|(a=0|E()),0|l,0),n=0|E(),a=0|cf(0|l,0,0|f,0|a),f=0|E(),t[A+4>>2]=a,a=0|cf(0|(a=0|lf(0|(a=0|t[e+4>>2]),0,0|a,0)),0|E(),f+n|0,0),Q=((l=0|E())+((f+n|0)>>>0<f>>>0&1)|0)>>>0<l>>>0,a=0|cf(0|a,0,0|(b=0|lf(0|t[e>>2],0,0|t[e+8>>2],0)),0|(c=0|E())),f=l+((f+n|0)>>>0<f>>>0&1)+(w=0|E())|0,c=0|cf(0|a,0,0|b,0|c),b=0|E(),t[A+8>>2]=c,n=0|cf(0|(c=0|lf(0|t[e>>2],0,0|t[e+12>>2],0)),0|(a=0|E()),f+b|0,0),b=(f>>>0<w>>>0?Q?2:1:1&Q)+((f+b|0)>>>0<b>>>0&1)+(l=0|E())|0,a=0|cf(0|n,0,0|c,0|a),c=0|E(),a=0|cf(0|a,0,0|(n=0|lf(0|t[e+4>>2],0,0|t[e+8>>2],0)),0|(f=0|E())),Q=0|E(),f=0|cf(0|a,0,0|n,0|f),n=0|E(),t[A+12>>2]=f,f=0|cf(0|(f=0|lf(0|(f=0|t[e+8>>2]),0,0|f,0)),0|E(),b+c+Q+n|0,0),n=((b+c|0)>>>0<c>>>0?b>>>0<l>>>0?2:1:b>>>0<l>>>0&1)+((b+c+Q|0)>>>0<Q>>>0&1)+((b+c+Q+n|0)>>>0<n>>>0&1)+(a=0|E())|0,f=0|cf(0|f,0,0|(Q=0|lf(0|t[e+4>>2],0,0|t[e+12>>2],0)),0|(c=0|E())),b=0|E(),c=0|cf(0|f,0,0|Q,0|c),Q=0|E(),c=0|cf(0|c,0,0|(f=0|lf(0|t[e>>2],0,0|t[e+16>>2],0)),0|(l=0|E())),w=0|E(),l=0|cf(0|c,0,0|f,0|l),f=0|E(),t[A+16>>2]=l,B=0|cf(0|(l=0|lf(0|t[e>>2],0,0|t[e+20>>2],0)),0|(c=0|E()),n+b+Q+w+f|0,0),f=((n+b|0)>>>0<b>>>0?n>>>0<a>>>0?2:1:n>>>0<a>>>0&1)+((n+b+Q|0)>>>0<Q>>>0&1)+((n+b+Q+w|0)>>>0<w>>>0&1)+((n+b+Q+w+f|0)>>>0<f>>>0&1)+(r=0|E())|0,c=0|cf(0|B,0,0|l,0|c),l=0|E(),c=0|cf(0|c,0,0|(B=0|lf(0|t[e+4>>2],0,0|t[e+16>>2],0)),0|(w=0|E())),Q=0|E(),w=0|cf(0|c,0,0|B,0|w),B=0|E(),w=0|cf(0|w,0,0|(c=0|lf(0|t[e+8>>2],0,0|t[e+12>>2],0)),0|(b=0|E())),n=0|E(),b=0|cf(0|w,0,0|c,0|b),w=f+l+Q+B+n+(c=0|E())|0,t[A+20>>2]=b,b=0|cf(0|(b=0|lf(0|(b=0|t[e+12>>2]),0,0|b,0)),0|E(),0|w,0),c=((f+l|0)>>>0<l>>>0?f>>>0<r>>>0?2:1:f>>>0<r>>>0&1)+((f+l+Q|0)>>>0<Q>>>0&1)+((f+l+Q+B|0)>>>0<B>>>0&1)+((f+l+Q+B+n|0)>>>0<n>>>0&1)+(w>>>0<c>>>0&1)+(a=0|E())|0,b=0|cf(0|b,0,0|(w=0|lf(0|t[e+8>>2],0,0|t[e+16>>2],0)),0|(n=0|E())),B=0|E(),n=0|cf(0|b,0,0|w,0|n),w=0|E(),n=0|cf(0|n,0,0|(b=0|lf(0|t[e+4>>2],0,0|t[e+20>>2],0)),0|(Q=0|E())),l=0|E(),Q=0|cf(0|n,0,0|b,0|Q),b=0|E(),Q=0|cf(0|Q,0,0|(n=0|lf(0|t[e>>2],0,0|t[e+24>>2],0)),0|(f=0|E())),o=c+B+w+l+b+(r=0|E())|0,f=0|cf(0|Q,0,0|n,0|f),n=0|E(),t[A+24>>2]=f,u=0|cf(0|(f=0|lf(0|t[e>>2],0,0|t[e+28>>2],0)),0|(Q=0|E()),o+n|0,0),n=((c+B|0)>>>0<B>>>0?c>>>0<a>>>0?2:1:c>>>0<a>>>0&1)+((c+B+w|0)>>>0<w>>>0&1)+((c+B+w+l|0)>>>0<l>>>0&1)+((c+B+w+l+b|0)>>>0<b>>>0&1)+(o>>>0<r>>>0&1)+((o+n|0)>>>0<n>>>0&1)+(i=0|E())|0,Q=0|cf(0|u,0,0|f,0|Q),f=0|E(),Q=0|cf(0|Q,0,0|(u=0|lf(0|t[e+4>>2],0,0|t[e+24>>2],0)),0|(o=0|E())),r=0|E(),o=0|cf(0|Q,0,0|u,0|o),u=0|E(),o=0|cf(0|o,0,0|(Q=0|lf(0|t[e+8>>2],0,0|t[e+20>>2],0)),0|(b=0|E())),l=0|E(),b=0|cf(0|o,0,0|Q,0|b),o=n+f+r+u+l+(Q=0|E())|0,b=0|cf(0|b,0,0|(w=0|lf(0|t[e+12>>2],0,0|t[e+16>>2],0)),0|(B=0|E())),c=0|E(),B=0|cf(0|b,0,0|w,0|B),w=0|E(),t[A+28>>2]=B,B=0|cf(0|(B=0|lf(0|(B=0|t[e+16>>2]),0,0|B,0)),0|E(),o+c+w|0,0),w=((n+f|0)>>>0<f>>>0?n>>>0<i>>>0?2:1:n>>>0<i>>>0&1)+((n+f+r|0)>>>0<r>>>0&1)+((n+f+r+u|0)>>>0<u>>>0&1)+((n+f+r+u+l|0)>>>0<l>>>0&1)+(o>>>0<Q>>>0&1)+((o+c|0)>>>0<c>>>0&1)+((o+c+w|0)>>>0<w>>>0&1)+(b=0|E())|0,B=0|cf(0|B,0,0|(c=0|lf(0|t[e+12>>2],0,0|t[e+20>>2],0)),0|(o=0|E())),Q=0|E(),o=0|cf(0|B,0,0|c,0|o),c=0|E(),o=0|cf(0|o,0,0|(B=0|lf(0|t[e+8>>2],0,0|t[e+24>>2],0)),0|(l=0|E())),u=0|E(),l=0|cf(0|o,0,0|B,0|l),B=0|E(),l=0|cf(0|l,0,0|(o=0|lf(0|t[e+4>>2],0,0|t[e+28>>2],0)),0|(r=0|E())),n=w+Q+c+u+B+(f=0|E())|0,r=0|cf(0|l,0,0|o,0|r),o=0|E(),t[A+32>>2]=r,i=0|cf(0|(r=0|lf(0|t[e+8>>2],0,0|t[e+28>>2],0)),0|(l=0|E()),n+o|0,0),o=((w+Q|0)>>>0<Q>>>0?w>>>0<b>>>0?2:1:w>>>0<b>>>0&1)+((w+Q+c|0)>>>0<c>>>0&1)+((w+Q+c+u|0)>>>0<u>>>0&1)+((w+Q+c+u+B|0)>>>0<B>>>0&1)+(n>>>0<f>>>0&1)+((n+o|0)>>>0<o>>>0&1)+(a=0|E())|0,l=0|cf(0|i,0,0|r,0|l),r=0|E(),l=0|cf(0|l,0,0|(i=0|lf(0|t[e+12>>2],0,0|t[e+24>>2],0)),0|(n=0|E())),f=0|E(),n=0|cf(0|l,0,0|i,0|n),i=0|E(),n=0|cf(0|n,0,0|(l=0|lf(0|t[e+16>>2],0,0|t[e+20>>2],0)),0|(B=0|E())),u=0|E(),B=0|cf(0|n,0,0|l,0|B),n=o+r+f+i+u+(l=0|E())|0,t[A+36>>2]=B,B=0|cf(0|(B=0|lf(0|(B=0|t[e+20>>2]),0,0|B,0)),0|E(),0|n,0),l=((o+r|0)>>>0<r>>>0?o>>>0<a>>>0?2:1:o>>>0<a>>>0&1)+((o+r+f|0)>>>0<f>>>0&1)+((o+r+f+i|0)>>>0<i>>>0&1)+((o+r+f+i+u|0)>>>0<u>>>0&1)+(n>>>0<l>>>0&1)+(c=0|E())|0,B=0|cf(0|B,0,0|(n=0|lf(0|t[e+16>>2],0,0|t[e+24>>2],0)),0|(u=0|E())),i=0|E(),u=0|cf(0|B,0,0|n,0|u),n=0|E(),u=0|cf(0|u,0,0|(B=0|lf(0|t[e+12>>2],0,0|t[e+28>>2],0)),0|(f=0|E())),r=0|E(),f=0|cf(0|u,0,0|B,0|f),B=0|E(),t[A+40>>2]=f,o=0|cf(0|(f=0|lf(0|t[e+16>>2],0,0|t[e+28>>2],0)),0|(u=0|E()),l+i+n+r+B|0,0),B=((l+i|0)>>>0<i>>>0?l>>>0<c>>>0?2:1:l>>>0<c>>>0&1)+((l+i+n|0)>>>0<n>>>0&1)+((l+i+n+r|0)>>>0<r>>>0&1)+((l+i+n+r+B|0)>>>0<B>>>0&1)+(a=0|E())|0,u=0|cf(0|o,0,0|f,0|u),f=0|E(),u=0|cf(0|u,0,0|(o=0|lf(0|t[e+20>>2],0,0|t[e+24>>2],0)),0|(r=0|E())),n=0|E(),r=0|cf(0|u,0,0|o,0|r),o=0|E(),t[A+44>>2]=r,r=0|cf(0|(r=0|lf(0|(r=0|t[e+24>>2]),0,0|r,0)),0|E(),B+f+n+o|0,0),o=((B+f|0)>>>0<f>>>0?B>>>0<a>>>0?2:1:B>>>0<a>>>0&1)+((B+f+n|0)>>>0<n>>>0&1)+((B+f+n+o|0)>>>0<o>>>0&1)+(u=0|E())|0,r=0|cf(0|r,0,0|(n=0|lf(0|t[e+20>>2],0,0|t[e+28>>2],0)),0|(f=0|E())),B=0|E(),f=0|cf(0|r,0,0|n,0|f),n=0|E(),t[A+48>>2]=f,a=0|cf(0|(f=0|lf(0|t[e+24>>2],0,0|t[e+28>>2],0)),0|(r=0|E()),o+B+n|0,0),n=((o+B|0)>>>0<B>>>0?o>>>0<u>>>0?2:1:o>>>0<u>>>0&1)+((o+B+n|0)>>>0<n>>>0&1)+(i=0|E())|0,r=0|cf(0|a,0,0|f,0|r),f=0|E(),t[A+52>>2]=r,r=0|cf(0|(r=0|lf(0|(r=0|t[e+28>>2]),0,0|r,0)),0|E(),n+f|0,0),e=((n+f|0)>>>0<f>>>0?n>>>0<i>>>0?2:1:n>>>0<i>>>0&1)+(0|E())|0,t[A+56>>2]=r,t[A+60>>2]=e}function mf(A,e){A|=0;var r=0,i=0,n=0,f=0,a=0,B=0,o=0,u=0;r=0|lf(0|(r=0|t[(e|=0)>>2]),0,0|r,0),a=0|E(),t[A>>2]=r,a=0|cf(0|(r=0|lf(0|t[e>>2],0,0|t[e+4>>2],0)),0|(u=0|E()),0|a,0),o=0|E(),u=0|cf(0|a,0,0|r,0|u),r=0|E(),t[A+4>>2]=u,u=0|cf(0|(u=0|lf(0|(u=0|t[e+4>>2]),0,0|u,0)),0|E(),r+o|0,0),n=((a=0|E())+((r+o|0)>>>0<r>>>0&1)|0)>>>0<a>>>0,u=0|cf(0|u,0,0|(B=0|lf(0|t[e>>2],0,0|t[e+8>>2],0)),0|(f=0|E())),r=a+((r+o|0)>>>0<r>>>0&1)+(i=0|E())|0,f=0|cf(0|u,0,0|B,0|f),B=0|E(),t[A+8>>2]=f,o=0|cf(0|(f=0|lf(0|t[e>>2],0,0|t[e+12>>2],0)),0|(u=0|E()),r+B|0,0),B=(r>>>0<i>>>0?n?2:1:1&n)+((r+B|0)>>>0<B>>>0&1)+(a=0|E())|0,u=0|cf(0|o,0,0|f,0|u),f=0|E(),u=0|cf(0|u,0,0|(o=0|lf(0|t[e+4>>2],0,0|t[e+8>>2],0)),0|(r=0|E())),n=0|E(),r=0|cf(0|u,0,0|o,0|r),o=0|E(),t[A+12>>2]=r,r=0|cf(0|(r=0|lf(0|(r=0|t[e+8>>2]),0,0|r,0)),0|E(),B+f+n+o|0,0),o=((B+f|0)>>>0<f>>>0?B>>>0<a>>>0?2:1:B>>>0<a>>>0&1)+((B+f+n|0)>>>0<n>>>0&1)+((B+f+n+o|0)>>>0<o>>>0&1)+(u=0|E())|0,r=0|cf(0|r,0,0|(n=0|lf(0|t[e+4>>2],0,0|t[e+12>>2],0)),0|(f=0|E())),B=0|E(),f=0|cf(0|r,0,0|n,0|f),n=0|E(),t[A+16>>2]=f,a=0|cf(0|(f=0|lf(0|t[e+8>>2],0,0|t[e+12>>2],0)),0|(r=0|E()),o+B+n|0,0),n=((o+B|0)>>>0<B>>>0?o>>>0<u>>>0?2:1:o>>>0<u>>>0&1)+((o+B+n|0)>>>0<n>>>0&1)+(i=0|E())|0,r=0|cf(0|a,0,0|f,0|r),f=0|E(),t[A+20>>2]=r,r=0|cf(0|(r=0|lf(0|(r=0|t[e+12>>2]),0,0|r,0)),0|E(),n+f|0,0),e=((n+f|0)>>>0<f>>>0?n>>>0<i>>>0?2:1:n>>>0<i>>>0&1)+(0|E())|0,t[A+24>>2]=r,t[A+28>>2]=e}function Hf(A,e,r,i){var n;return n=G,G=G+16|0,e=0|vf(A|=0,e|=0,r|=0,i|=0,n),0|t[n>>2]?(Xe(3,110,108,0,0),G=n,0|e):(G=n,0|e)}function vf(A,e,r,i,n){A|=0,e|=0,i|=0,n|=0;var f,a=0,B=0,o=0,u=0,l=0,c=0,E=0,Q=0,w=0,b=0,k=0,s=0;if(f=G,G=G+32|0,0==(0|ka(r|=0,1))&&0==(0|jf(r))){if(0|n&&(t[n>>2]=0),0==(0|Of(e,4))&&0==(0|Of(r,4))){iB(i),u=0|tB(i),B=0|tB(i),a=0|tB(i),E=0|tB(i),l=0|tB(i),w=0|tB(i),Q=0|tB(i);A:do{if(Q){if(A)b=A;else{if(!(o=0|qf())){B=0,a=0;break}b=o}if(ta(a,1),ba(w),0!=(0|ra(B,e))&&0!=(0|ra(u,r))){if(t[u+12>>2]=0,0==(0|t[B+12>>2])&&(0|la(B,u))<=-1||(k=42),42==(0|k)&&0==(0|$a(B,B,u,i))){B=b,a=0;break}if(0!=(0|da(r))&&(0|Kf(r))<2049)if(0|jf(B))B=u,a=w,k=97;else for(;;){if(!(0|Qa(B,0))){o=0;do{if(o=o+1|0,0|da(a)&&0==(0|ln(a,a,r))){B=b,a=0;break A}if(!(0|en(a,a))){B=b,a=0;break A}}while(!(0|Qa(B,o)));if(!(0|nn(B,B,o))){B=b,a=0;break A}}if(!(0|Qa(u,0))){o=0;do{if(o=o+1|0,0|da(w)&&0==(0|ln(w,w,r))){B=b,a=0;break A}if(!(0|en(w,w))){B=b,a=0;break A}}while(!(0|Qa(u,o)));if(!(0|nn(u,u,o))){B=b,a=0;break A}}if((0|la(B,u))>-1){if(!(0|ln(a,a,w))){B=b,a=0;break A}if(!(0|un(B,B,u))){B=b,a=0;break A}}else{if(!(0|ln(w,w,a))){B=b,a=0;break A}if(!(0|un(u,u,B))){B=b,a=0;break A}}if(0|jf(B)){B=u,a=w,k=97;break}}else if(0|jf(B))B=u,a=w,k=97;else{for(c=u,o=w,u=-1;;){e=0|Kf(c);do{if((0|e)==(0|Kf(B))){if(!(0|ta(E,1))){B=b,a=0;break A}if(!(0|cn(l,c,B))){B=b,a=0;break A}}else{if((0|(e=0|Kf(c)))!=(1+(0|Kf(B))|0)){if(0|wB(E,l,c,B,i))break;B=b,a=0;break A}if(!(0|An(Q,B))){B=b,a=0;break A}if((0|la(c,Q))<0){if(!(0|ta(E,1))){B=b,a=0;break A}if(0|cn(l,c,B))break;B=b,a=0;break A}if(!(0|cn(l,c,Q))){B=b,a=0;break A}if(!(0|on(E,Q,B))){B=b,a=0;break A}if((0|la(c,E))<0){if(0|ta(E,2))break;B=b,a=0;break A}if(!(0|ta(E,3))){B=b,a=0;break A}if(!(0|cn(l,l,B))){B=b,a=0;break A}}}while(0);if(0|sa(E)){if(!(0|on(c,a,o))){B=b,a=0;break A}}else{do{if(0|Ca(E,2)){if(!(0|An(c,a))){B=b,a=0;break A}}else{if(0|Ca(E,4)){if(0|rn(c,a,2))break;B=b,a=0;break A}if(1!=(0|t[E+4>>2])){if(0|Sf(c,E,a,i))break;B=b,a=0;break A}if(!(0|ra(c,a))){B=b,a=0;break A}if(!(0|QB(c,0|t[t[E>>2]>>2]))){B=b,a=0;break A}}}while(0);if(!(0|on(c,c,o))){B=b,a=0;break A}}if(0|jf(l))break;s=o,w=l,e=B,u=0-u|0,o=a,a=c,l=s,B=w,c=e}(0|u)>0&&(k=97)}if(97==(0|k)&&!(0|cn(a,r,a))){B=b,a=0;break}if(!(0|sa(B))){if(!n){B=b,a=0;break}t[n>>2]=1,B=b,a=0;break}if(0==(0|t[a+12>>2])&&(0|la(a,r))<0){if(!(0|ra(b,a))){B=b,a=0;break}}else k=102;if(102==(0|k)&&0==(0|$a(b,a,r,i))){B=b,a=0;break}B=b,a=b}else B=b,a=0}else B=0,a=0}while(0);return 0==(0|A)&0==(0|a)&&Pf(B),nB(i),G=f,0|(s=a)}iB(i),l=0|tB(i),B=0|tB(i),a=0|tB(i),Q=0|tB(i),c=0|tB(i),o=0|tB(i);A:do{if(0|tB(i)){if(A)u=A;else if(!(u=0|qf())){B=0,a=0;break}if(ta(a,1),ba(o),0!=(0|ra(B,e))&&0!=(0|ra(l,r))){if(t[l+12>>2]=0,0==(0|t[B+12>>2])&&(0|la(B,l))<=-1||(k=16),16==(0|k)&&(_f(f),Da(f,B,4),0==(0|$a(B,f,l,i)))){B=u,a=0;break}if(0|jf(B))B=l,a=o,k=24;else{for(E=l,l=-1;;){if(_f(f),Da(f,E,4),!(0|wB(Q,c,f,B,i))){B=u,a=0;break A}if(!(0|Sf(E,Q,a,i))){B=u,a=0;break A}if(!(0|on(E,E,o))){B=u,a=0;break A}if(0|jf(c))break;b=o,n=c,s=B,l=0-l|0,o=a,a=E,c=b,B=n,E=s}(0|l)>0&&(k=24)}if(24==(0|k)&&!(0|cn(a,r,a))){B=u,a=0;break}if(!(0|sa(B))){Xe(3,139,108,0,0),B=u,a=0;break}if(0==(0|t[a+12>>2])&&(0|la(a,r))<0){if(!(0|ra(u,a))){B=u,a=0;break}}else k=29;if(29==(0|k)&&0==(0|$a(u,a,r,i))){B=u,a=0;break}B=u,a=u}else B=u,a=0}else B=0,a=0}while(0);return 0==(0|A)&0==(0|a)&&Pf(B),nB(i),G=f,0|(s=a)}return n?(t[n>>2]=1,G=f,0|(s=0)):(G=f,0|(s=0))}function Jf(A,e,r,i,n){n|=0;var f=0,a=0,B=0,o=0,u=0,l=0,c=0,E=0,Q=0;if(o=0|Un(A|=0,e|=0,r|=0,i|=0),!n)return 0|(Q=o);if((0|n)<0){if(u=0|t[r+(i<<2)>>2],t[A+(i<<2)>>2]=0-(u+o),u=0==(0|u)?o:1,-1==(0|n))return 0|(Q=u);for(l=r+(i<<2)|0,c=n,E=A+(i<<2)|0,r=u;;){if(u=0|t[l+4>>2],t[E+4>>2]=0-(u+r),r=0==(0|u)?r:1,-2==(0|c)){c=r,r=23;break}if(u=0|t[l+8>>2],t[E+8>>2]=0-(u+r),r=0==(0|u)?r:1,(0|c)>-4){c=r,r=23;break}if(u=0|t[l+12>>2],t[E+12>>2]=0-(u+r),r=0==(0|u)?r:1,-4==(0|c)){c=r,r=23;break}if(E=E+16|0,u=0|t[(l=l+16|0)>>2],t[E>>2]=0-(u+r),r=0==(0|u)?r:1,(0|c)>-6){c=r,r=23;break}c=c+4|0}if(23==(0|r))return 0|c}do{if(o){for(l=e+(i<<2)|0,u=A+(i<<2)|0;;){if(r=0|t[l>>2],t[u>>2]=r-o,r=0==(0|r)?o:0,(0|n)<2){c=r,r=23;break}if(A=0|t[l+4>>2],t[u+4>>2]=A-r,r=0==(0|A)?r:0,2==(0|n)){c=r,r=23;break}if(o=0|t[l+8>>2],t[u+8>>2]=o-r,o=0==(0|o)?r:0,(0|n)<4){c=o,r=23;break}if(r=0|t[l+12>>2],t[u+12>>2]=r-o,r=0==(0|r)?o:0,Q=n+-4|0,4==(0|n)){c=r,r=23;break}if(l=l+16|0,u=u+16|0,!r){r=16;break}n=Q}if(16==(0|r)){if((0|n)>4){B=l,a=Q,f=u;break}return 0|(c=0)}if(23==(0|r))return 0|c}else B=e+(i<<2)|0,a=n,f=A+(i<<2)|0}while(0);if(t[f>>2]=t[B>>2],1==(0|a))return 0|(Q=0);for(;;){if(t[f+4>>2]=t[B+4>>2],2==(0|a)){c=0,r=23;break}if(t[f+8>>2]=t[B+8>>2],(0|a)<4){c=0,r=23;break}if(t[f+12>>2]=t[B+12>>2],4==(0|a)){c=0,r=23;break}if(B=B+16|0,t[(f=f+16|0)>>2]=t[B>>2],(0|a)<6){c=0,r=23;break}a=a+-4|0}return 23==(0|r)?0|c:0}function Xf(A,e,r,i,n,f,a){A|=0,e|=0,r|=0,a|=0;var B,o,u=0,l=0,c=0;if(u=((0|(i|=0))/2|0)+(n|=0)|0,l=((0|i)/2|0)+(f|=0)|0,8==(0|i)&0==(f|n|0))Rf(A,e,r);else if((0|i)<16){if(Tf(A,e,n+i|0,r,f+i|0),(f+n|0)>=0)return;Df(A+(f+n+(i<<1)<<2)|0,0,0-(f+n<<2)|0)}else{switch((3*(c=0|wa(e,B=e+(((0|i)/2|0)<<2)|0,u,0-n|0))|0)+(0|wa(o=r+(((0|i)/2|0)<<2)|0,r,l,f))|0){case-4:Jf(a,B,e,u,n),Jf(a+(((0|i)/2|0)<<2)|0,r,o,l,0-f|0),c=0,u=0;break;case-3:c=0,u=1;break;case-2:Jf(a,B,e,u,n),Jf(a+(((0|i)/2|0)<<2)|0,o,r,l,f),c=1,u=0;break;case 1:case 0:case-1:c=0,u=1;break;case 2:Jf(a,e,B,u,0-n|0),Jf(a+(((0|i)/2|0)<<2)|0,r,o,l,0-f|0),c=1,u=0;break;case 3:c=0,u=1;break;case 4:Jf(a,e,B,u,0-n|0),Jf(a+(((0|i)/2|0)<<2)|0,o,r,l,f),c=0,u=0;break;default:c=0,u=0}do{if(!(8==(-2&i|0)&0==(f|n|0))){if(16==(-2&i|0)&0==(f|n|0)){if(u){l=(u=a+(i<<2)|0)+64|0;do{t[u>>2]=0,u=u+4|0}while((0|u)<(0|l))}else Rf(a+(i<<2)|0,a,a+(((0|i)/2|0)<<2)|0);Rf(A,e,r),Rf(A+(i<<2)|0,B,o),u=A+(i<<2)|0,l=a+(i<<2)|0;break}u?Df(a+(i<<2)|0,0,i<<2|0):Xf(a+(i<<2)|0,a,a+(((0|i)/2|0)<<2)|0,(0|i)/2|0,0,0,a+(i<<1<<2)|0),Xf(A,e,r,(0|i)/2|0,0,0,a+(i<<1<<2)|0),Xf(A+(i<<2)|0,B,o,(0|i)/2|0,n,f,a+(i<<1<<2)|0),u=A+(i<<2)|0,l=a+(i<<2)|0;break}u?(t[a+(i<<2)>>2]=0,t[a+(i<<2)+4>>2]=0,t[a+(i<<2)+8>>2]=0,t[a+(i<<2)+12>>2]=0,t[a+(i<<2)+16>>2]=0,t[a+(i<<2)+20>>2]=0,t[a+(i<<2)+24>>2]=0,t[a+(i<<2)+28>>2]=0):yf(a+(i<<2)|0,a,a+(((0|i)/2|0)<<2)|0),yf(A,e,r),yf(A+(i<<2)|0,B,o),u=A+(i<<2)|0,l=a+(i<<2)|0}while(0);if(u=0|hn(a,A,u,i),u=c?u-(0|Un(l,a,l,i))|0:(0|hn(l,l,a,i))+u|0,(l=(0|hn(A+(((0|i)/2|0)<<2)|0,A+(((0|i)/2|0)<<2)|0,l,i))+u|0)&&(A=(0|t[(u=A+(((0|i)/2|0)+i<<2)|0)>>2])+l|0,t[u>>2]=A,!(A>>>0>=l>>>0)))do{A=1+(0|t[(u=u+4|0)>>2])|0,t[u>>2]=A}while(!(0|A))}}function Tf(A,e,r,i,n){A|=0,e|=0,i|=0;var f,a=0,B=0;if(f=(0|(r|=0))<(0|(n|=0))?n:r,B=(0|r)<(0|n)?e:i,a=(0|r)<(0|n)?r:n,r=(0|r)<(0|n)?i:e,(0|a)<1)Dn(A,r,f,0);else if(t[A+(f<<2)>>2]=0|Dn(A,r,f,0|t[B>>2]),1!=(0|a))for(i=A,e=A+(f<<2)|0;;){if(t[e+4>>2]=0|gn(i+4|0,r,f,0|t[B+4>>2]),2==(0|a)){e=9;break}if(t[e+8>>2]=0|gn(i+8|0,r,f,0|t[B+8>>2]),(0|a)<4){e=9;break}if(t[e+12>>2]=0|gn(i+12|0,r,f,0|t[B+12>>2]),4==(0|a)){e=9;break}if(i=i+16|0,B=B+16|0,t[(e=e+16|0)>>2]=0|gn(i,r,f,0|t[B>>2]),(0|a)<6){e=9;break}a=a+-4|0}}function Lf(A,e,r,i,n,f,a){A|=0,e|=0,r|=0,n|=0,f|=0,a|=0;var B=0,o=0,u=0;if((0|(i|=0))<8)Tf(A,e,n+i|0,r,f+i|0);else{switch((3*(u=0|wa(e,e+(i<<2)|0,n,i-n|0))|0)+(0|wa(r+(i<<2)|0,r,f,f-i|0))|0){case-4:Jf(a,e+(i<<2)|0,e,n,n-i|0),Jf(a+(i<<2)|0,r,r+(i<<2)|0,f,i-f|0),u=0;break;case-2:case-3:Jf(a,e+(i<<2)|0,e,n,n-i|0),Jf(a+(i<<2)|0,r+(i<<2)|0,r,f,f-i|0),u=1;break;case 2:case 1:case 0:case-1:Jf(a,e,e+(i<<2)|0,n,i-n|0),Jf(a+(i<<2)|0,r,r+(i<<2)|0,f,i-f|0),u=1;break;case 4:case 3:Jf(a,e,e+(i<<2)|0,n,i-n|0),Jf(a+(i<<2)|0,r+(i<<2)|0,r,f,f-i|0),u=0;break;default:u=0}A:do{if(8==(0|i))Rf(a+(i<<1<<2)|0,a,a+32|0),Rf(A,e,r),Tf(A+(i<<1<<2)|0,e+(i<<2)|0,n,r+(i<<2)|0,f),Df(A+((i<<1)+n+f<<2)|0,0,(i<<1)-n-f<<2|0),B=A+(i<<1<<2)|0,o=a+(i<<1<<2)|0;else{if(Xf(a+(i<<1<<2)|0,a,a+(i<<2)|0,i,0,0,a+(i<<2<<2)|0),Xf(A,e,r,i,0,0,a+(i<<2<<2)|0),!(o=((0|n)>(0|f)?n:f)-(i>>>1)|0)){Xf(A+(i<<1<<2)|0,e+(i<<2)|0,r+(i<<2)|0,i>>>1,n-(i>>>1)|0,f-(i>>>1)|0,a+(i<<2<<2)|0),Df(A+((i<<1)+(-2&i)<<2)|0,0,(i<<1)-(-2&i)<<2|0),B=A+(i<<1<<2)|0,o=a+(i<<1<<2)|0;break}if(B=A+(i<<1<<2)|0,(0|o)>0){Lf(B,e+(i<<2)|0,r+(i<<2)|0,i>>>1,n-(i>>>1)|0,f-(i>>>1)|0,a+(i<<2<<2)|0),Df(A+((i<<1)+n+f<<2)|0,0,(i<<1)-n-f<<2|0),o=a+(i<<1<<2)|0;break}if(Df(0|B,0,i<<3|0),(0|n)<16&(0|f)<16){Tf(B,e+(i<<2)|0,n,r+(i<<2)|0,f),o=a+(i<<1<<2)|0;break}e:do{if(!((i>>>2|0)<(0|n)|(i>>>2|0)<(0|f))){for(o=i>>>2;!((0|o)==(0|n)|(0|o)==(0|f));)if((0|(o=(0|o)/2|0))<(0|n)|(0|o)<(0|f))break e;Xf(B,e+(i<<2)|0,r+(i<<2)|0,o,n-o|0,f-o|0,a+(i<<2<<2)|0),o=a+(i<<1<<2)|0;break A}o=i>>>2}while(0);Lf(B,e+(i<<2)|0,r+(i<<2)|0,o,n-o|0,f-o|0,a+(i<<2<<2)|0),o=a+(i<<1<<2)|0}}while(0);if(B=0|hn(a,A,B,i<<1),B=u?B-(0|Un(o,a,o,i<<1))|0:(0|hn(o,o,a,i<<1))+B|0,(B=(0|hn(A+(i<<2)|0,A+(i<<2)|0,o,i<<1))+B|0)&&(a=(0|t[A+(3*i<<2)>>2])+B|0,t[A+(3*i<<2)>>2]=a,!(a>>>0>=B>>>0))){B=A+(3*i<<2)|0;do{A=1+(0|t[(B=B+4|0)>>2])|0,t[B>>2]=A}while(!(0|A))}}}function Sf(A,e,r,i){return i=0|pf(A|=0,e|=0,r|=0,i|=0),aa(A),0|i}function pf(A,e,r,i){A|=0,r|=0,i|=0;var n,f,a=0,B=0,o=0;if(0==(0|(n=0|t[(e|=0)+4>>2]))|0==(0|(f=0|t[r+4>>2])))return ba(A),0|(i=1);iB(i),(0|A)==(0|e)|(0|A)==(0|r)?(a=0|tB(i))?B=5:a=0:(a=A,B=5);A:do{if(5==(0|B)){do{if(8==(0|n)&0==(n-f|0)){if(!(0|ia(a,16))){a=0;break A}t[a+4>>2]=16,Rf(0|t[a>>2],0|t[e>>2],0|t[r>>2])}else{if(!((0|n)>15&(0|f)>15&(n-f+1|0)>>>0<3)){if(!(0|ia(a,f+n|0))){a=0;break A}t[a+4>>2]=f+n,Tf(0|t[a>>2],0|t[e>>2],n,0|t[r>>2],f);break}if(o=1<<(0|zf((n-f|0)>-1?n:f))-1,!(B=0|tB(i))){a=0;break A}if((0|n)>(0|o)|(0|f)>(0|o)){if(!(0|ia(B,o<<3))){a=0;break A}if(!(0|ia(a,o<<3))){a=0;break A}Lf(0|t[a>>2],0|t[e>>2],0|t[r>>2],o,n-o|0,f-o|0,0|t[B>>2])}else{if(!(0|ia(B,o<<2))){a=0;break A}if(!(0|ia(a,o<<2))){a=0;break A}Xf(0|t[a>>2],0|t[e>>2],0|t[r>>2],o,n-o|0,f-o|0,0|t[B>>2])}t[a+4>>2]=f+n}}while(0);if(t[a+12>>2]=t[r+12>>2]^t[e+12>>2],(0|a)!=(0|A)&&0==(0|ra(A,a))){a=0;break}a=1}}while(0);return nB(i),0|(i=a)}function zf(A){var e,r,i;return((0-((((e=((r=((i=((A|=0)>>>16^A)&0-(A>>>16)>>31^A)>>>8^i)&0-(i>>>8)>>31^i)>>>4^r)&0-(r>>>4)>>31^r)>>>2^e)&0-(e>>>2)>>31^e)>>>1)|0)>>>31)+(0-(A>>>16)>>31&16|0!=(0|A)|0-(i>>>8)>>31&8|0-(r>>>4)>>31&4|0-(e>>>2)>>31&2)|0}function Kf(A){var e=0,r=0,i=0,n=0,f=0,a=0,B=0,o=0,u=0,l=0;if(f=0|t[(A|=0)+4>>2],!(4&t[A+16>>2]))return f?0|(f=(0-((e=0|t[(0|t[A>>2])+(f+-1<<2)>>2])>>>16)>>31&16|0!=(0|e)|0-((r=(e>>>16^e)&0-(e>>>16)>>31^e)>>>8)>>31&8|0-((i=(r>>>8^r)&0-(r>>>8)>>31^r)>>>4)>>31&4|0-((n=(i>>>4^i)&0-(i>>>4)>>31^i)>>>2)>>31&2|f+-1<<5)+((0-(((n>>>2^n)&0-(n>>>2)>>31^n)>>>1)|0)>>>31)|0):0|(f=0);if((0|(n=0|t[A+8>>2]))>0){e=0|t[A>>2],r=0,i=0,A=0;do{A=(32&(i|=a=((r^f+-1)-1&f+2147483647)>>31)^32)+A+(((0-((((B=((o=((u=((l=0|t[e+(r<<2)>>2])>>>16^l)&0-(l>>>16)>>31^l)>>>8^u)&0-(u>>>8)>>31^u)>>>4^o)&0-(o>>>4)>>31^o)>>>2^B)&0-(B>>>2)>>31^B)>>>1)|0)>>>31)+(0-(l>>>16)>>31&16|0!=(0|l)|0-(u>>>8)>>31&8|0-(o>>>4)>>31&4|0-(B>>>2)>>31&2)&a)|0,r=r+1|0}while((0|r)!=(0|n))}else A=0;return 0|(l=A&~((-2147483648-f&(-2147483648^f))>>31))}function jf(A){return 0==(0|t[(A|=0)+4>>2])|0}function xf(A){var e=0,r=0;if(A|=0){e=0|t[A>>2];do{if(0!=(0|e)&&($r(e,t[A+8>>2]<<2),0==(2&(r=0|t[A+16>>2])|0))){if(e=0|t[A>>2],8&r){ei(e,82796,0),e=A+16|0;break}Xn(e),e=A+16|0;break}e=A+16|0}while(0);r=1&t[e>>2],$r(A,20),r&&Xn(A)}}function Of(A,e){return e|=0,t[(A|=0)+16>>2]&e|0}function Pf(A){var e,r=0;if(A|=0){e=0|t[A+16>>2];do{if(!(2&e)){if(r=0|t[A>>2],8&e){ei(r,82796,0);break}Xn(r);break}}while(0);return 1&t[A+16>>2]?void Xn(A):void(t[A>>2]=0)}}function _f(A){t[(A|=0)>>2]=0,t[A+4>>2]=0,t[A+8>>2]=0,t[A+12>>2]=0,t[A+16>>2]=0}function qf(){var A=0;return(A=0|Sn(20,82796))?(t[A+16>>2]=1,0|A):(Xe(3,113,65,0,0),0|(A=0))}function $f(){var A=0;return(A=0|Sn(20,82796))?(t[A+16>>2]=9,0|A):(Xe(3,113,65,0,0),0|(A=0))}function Aa(A,e){e|=0;var r,i=0,n=0,f=0,a=0,B=0,o=0,u=0,l=0,c=0;if((0|t[(A|=0)+8>>2])>=(0|e))return 0|(o=A);if((0|e)>16777215)return Xe(3,120,114,0,0),0|(o=0);if(2&(i=0|t[A+16>>2])|0)return Xe(3,120,105,0,0),0|(o=0);if(!(B=8&i?0|function(A,e,r){return 0|(A=0|Sn(A|=0,e|=0,r|=0))}(e<<2,82796,0):0|Sn(e<<2,82796)))return Xe(3,120,65,0,0),0|(o=0);r=0|t[A>>2];do{if(r){if((0|(i=0|t[A+4>>2]))>3){for(a=(0==(i>>>2|0)?i>>>2^1073741823:1073741822)+(i>>>2)<<2,n=B,f=r,i>>>=2;c=0|t[f+4>>2],l=0|t[f+8>>2],u=0|t[f+12>>2],t[n>>2]=t[f>>2],t[n+4>>2]=c,t[n+8>>2]=l,t[n+12>>2]=u,n=n+16|0,!((0|i)<=1);)f=f+16|0,i=i+-1|0;f=0|t[A+4>>2],i=r+(a+8<<2)|0}else f=i,n=B,i=r;switch(3&f){case 3:t[n+8>>2]=t[i+8>>2],o=18;break;case 2:o=18;break;case 1:o=19}if(18==(0|o)&&(t[n+4>>2]=t[i+4>>2],o=19),19==(0|o)&&(t[n>>2]=t[i>>2]),r){if($r(r,t[A+8>>2]<<2),i=0|t[A>>2],8&t[A+16>>2]){ei(i,82796,0),i=A;break}Xn(i),i=A;break}o=22}else o=22}while(0);return 22==(0|o)&&(i=A),t[i>>2]=B,t[A+8>>2]=e,0|(c=A)}function ea(A){var e=0,r=0,i=0,n=0,f=0,a=0,B=0,o=0,u=0;if(!(A|=0))return 0|(a=0);if(i=0==(8&t[A+16>>2]|0),a=0|Sn(20,82796),i){if(!a)return Xe(3,113,65,0,0),0|(a=0);e=1}else{if(!a)return Xe(3,113,65,0,0),0|(a=0);e=9}if(t[a+16>>2]=e,(0|a)==(0|A))return 0|(a=A);e=0|t[A+4>>2];do{if((0|t[a+8>>2])<(0|e)){if(0|Aa(a,e)){e=0|t[A+4>>2];break}e=0|t[a+16>>2];do{if(!(2&e)){if(r=0|t[a>>2],8&e){ei(r,82796,0);break}Xn(r);break}}while(0);return 1&t[a+16>>2]?(Xn(a),0|(a=0)):(t[a>>2]=0,0|(a=0))}}while(0);if(i=0|t[a>>2],r=0|t[A>>2],(0|e)>3){for(e>>>=2;u=0|t[r+4>>2],o=0|t[r+8>>2],B=0|t[r+12>>2],t[i>>2]=t[r>>2],t[i+4>>2]=u,t[i+8>>2]=o,t[i+12>>2]=B,i=i+16|0,r=r+16|0,!((0|e)<=1);)e=e+-1|0;e=0|t[A+4>>2]}switch(3&e){case 3:t[i+8>>2]=t[r+8>>2],f=17;break;case 2:f=17;break;case 1:f=18;break;case 0:n=e}return 17==(0|f)&&(t[i+4>>2]=t[r+4>>2],f=18),18==(0|f)&&(t[i>>2]=t[r>>2],n=0|t[A+4>>2]),t[a+12>>2]=t[A+12>>2],t[a+4>>2]=n,0|(u=a)}function ra(A,e){var r=0,i=0,n=0,f=0,a=0,B=0,o=0,u=0;if((0|(A|=0))==(0|(e|=0)))return 0|(a=A);r=0|t[e+4>>2];do{if((0|t[A+8>>2])<(0|r)){if(0|Aa(A,r)){r=0|t[e+4>>2];break}return 0|(a=0)}}while(0);if(n=0|t[A>>2],i=0|t[e>>2],(0|r)>3){for(r>>>=2;u=0|t[i+4>>2],o=0|t[i+8>>2],B=0|t[i+12>>2],t[n>>2]=t[i>>2],t[n+4>>2]=u,t[n+8>>2]=o,t[n+12>>2]=B,n=n+16|0,i=i+16|0,!((0|r)<=1);)r=r+-1|0;r=0|t[e+4>>2]}switch(3&r){case 3:t[n+8>>2]=t[i+8>>2],a=11;break;case 2:a=11;break;case 1:a=12;break;case 0:f=r}return 11==(0|a)&&(t[n+4>>2]=t[i+4>>2],a=12),12==(0|a)&&(t[n>>2]=t[i>>2],f=0|t[e+4>>2]),t[A+12>>2]=t[e+12>>2],t[A+4>>2]=f,0|(u=A)}function ia(A,e){return e|=0,(0|t[(A|=0)+8>>2])>=(0|e)?0|(e=A):0|(e=0|Aa(A,e))}function na(A){var e=0;return 0|(e=(0|(e=0|t[(A|=0)+4>>2]))<=1?1==(0|e)?0|t[t[A>>2]>>2]:0:-1)}function ta(A,e){return e|=0,(0|t[(A|=0)+8>>2])<=0&&0==(0|Aa(A,1))?0|(e=0):(t[A+12>>2]=0,t[t[A>>2]>>2]=e,t[A+4>>2]=0!=(0|e)&1,0|(e=1))}function fa(A,e,r){A|=0,e|=0,r|=0;var n,a,B=0,o=0,u=0,l=0,c=0;do{if(!r){if(0|(r=0|Sn(20,82796))){t[r+16>>2]=1,B=r,o=r,c=r;break}return Xe(3,113,65,0,0),0|(c=0)}B=0,o=0,c=r}while(0);A:do{if((0|e)>0)for(;;){if(0|i[A>>0]){r=e;break A}if(A=A+1|0,r=e+-1|0,!((0|e)>1)){l=9;break}e=r}else r=e,l=9}while(0);if(9==(0|l)&&!r)return t[c+4>>2]=0,0|c;if(e=r+-1|0,(0|t[c+8>>2])<=(e>>>2|0)&&0==(0|Aa(c,1+(e>>>2)|0))){if(!o)return 0|(c=0);r=0|t[(e=o+16|0)>>2];do{if(!(2&r)){if(A=0|t[o>>2],8&r){ei(A,82796,0);break}Xn(A);break}}while(0);return 1&t[e>>2]?(Xn(B),0|(c=0)):(t[o>>2]=0,0|(c=0))}if(t[(n=c+4|0)>>2]=1+(e>>>2),t[(a=c+12|0)>>2]=0,r){for(u=1+(e>>>2)|0,B=0,o=3&e;r=r+-1|0,B=B<<8|f[A>>0],e=u+-1|0,o?(e=u,o=o+-1|0):(t[(0|t[c>>2])+(e<<2)>>2]=B,B=0,o=3),r;)u=e,A=A+1|0;(0|(r=0|t[n>>2]))>0&&(l=27)}else r=1+(e>>>2)|0,l=27;if(27==(0|l)){for(e=(0|t[c>>2])+(r<<2)|0;!(0|t[(e=e+-4|0)>>2]);){if(A=r+-1|0,(0|r)<=1){r=A;break}r=A}t[n>>2]=r}return 0|r||(t[a>>2]=0),0|c}function aa(A){var e=0,r=0,i=0;if((0|(e=0|t[(A|=0)+4>>2]))>0){for(i=(0|t[A>>2])+(e<<2)|0;!(0|t[(i=i+-4|0)>>2]);){if(r=e+-1|0,(0|e)<=1){e=r;break}e=r}t[A+4>>2]=e}0|e||(t[A+12>>2]=0)}function Ba(A,e,r){return A|=0,e|=0,0|(A=(0|(r|=0))<0?-1:0|oa(A,e,r,0))}function oa(A,e,r,n){e|=0,r|=0,n|=0;var f,a,B,u=0,l=0,c=0;if(B=G,G=G+32|0,u=(7+(0|Kf(A|=0))|0)/8|0,-1!=(0|r))if((0|u)>(0|r)){if(t[B>>2]=t[A>>2],t[B+4>>2]=t[A+4>>2],t[B+8>>2]=t[A+8>>2],t[B+12>>2]=t[A+12>>2],t[B+16>>2]=t[A+16>>2],(0|(u=0|t[B+4>>2]))>0){for(c=(0|t[B>>2])+(u<<2)|0;!(0|t[(c=c+-4|0)>>2]);){if(l=u+-1|0,(0|u)<=1){u=l;break}u=l}t[B+4>>2]=u}if(u||(t[B+12>>2]=0),(0|(7+(0|Kf(B))|0)/8)>(0|r))return G=B,0|(A=-1);u=r}else u=r;if(!(l=0|t[A+8>>2]))return $r(e,u),G=B,0|(A=u);if(a=t[A+4>>2]<<2,!u)return G=B,0|(A=0);if(f=1|o(l,-4),n){for(c=0,r=0,l=e;i[l>>0]=(0|t[(0|t[A>>2])+(c>>>2<<2)>>2])>>>(c<<3&24)&r-a>>31,(0|(r=r+1|0))!=(0|u);)c=((f+c|0)>>>31)+c|0,l=l+1|0;return G=B,0|u}for(c=0,r=0,l=e+u|0;i[(l=l+-1|0)>>0]=(0|t[(0|t[A>>2])+(c>>>2<<2)>>2])>>>(c<<3&24)&r-a>>31,(0|(r=r+1|0))!=(0|u);)c=((f+c|0)>>>31)+c|0;return G=B,0|u}function ua(A,e){e|=0;var r,n,f=0,a=0,B=0,u=0;if(n=0|Kf(A|=0),!(f=0|t[A+8>>2]))return $r(e,(n+7|0)/8|0),0|(u=(n+7|0)/8|0);if(u=t[A+4>>2]<<2,(n+14|0)>>>0<15)return 0|(u=0);for(r=1|o(f,-4),a=0,B=0,f=e+((n+7|0)/8|0)|0;;){if(i[(f=f+-1|0)>>0]=(0|t[(0|t[A>>2])+(a>>>2<<2)>>2])>>>(a<<3&24)&B-u>>31,(0|(B=B+1|0))==(0|(n+7|0)/8)){f=(n+7|0)/8|0;break}a=((a+r|0)>>>31)+a|0}return 0|f}function la(A,e){e|=0;var r,i,n=0,f=0,a=0;if(0|(n=(f=0|t[(A|=0)+4>>2])-(0|t[e+4>>2])|0))return 0|(a=n);if(i=0|t[A>>2],r=0|t[e>>2],(0|f)<=0)return 0|(a=0);for(;e=f,(0|(A=0|t[i+((f=f+-1|0)<<2)>>2]))==(0|(n=0|t[r+(f<<2)>>2]));)if((0|e)<=1){A=0,a=7;break}return 7==(0|a)?0|A:0|(a=A>>>0>n>>>0?1:-1)}function ca(A,e){var r,i,n=0,f=0,a=0,B=0;if(0==(0|(A|=0))|0==(0|(e|=0)))return 0|(0==(0|A)?1&(0==(0|e)^1):-1);if(n=0==(0|(f=0|t[A+12>>2]))?1:-1,(0|f)!=(0|t[e+12>>2]))return 0|(B=n);if(B=0==(0|f)?-1:1,(0|(a=0|t[A+4>>2]))>(0|(f=0|t[e+4>>2])))return 0|(B=n);if((0|a)<(0|f))return 0|B;if((0|a)<=0)return 0|(B=0);for(i=0|t[A>>2],r=0|t[e>>2];;){if(e=a,(f=0|t[i+((a=a+-1|0)<<2)>>2])>>>0>(A=0|t[r+(a<<2)>>2])>>>0){f=11;break}if(f>>>0<A>>>0){n=B,f=11;break}if((0|e)<=1){n=0,f=11;break}}return 11==(0|f)?0|n:0}function Ea(A,e){var r=0;if((0|(e|=0))<0)return 0|(e=0);if((0|(r=0|t[(A|=0)+4>>2]))<=(e>>>5|0)){do{if((0|t[A+8>>2])<=(e>>>5|0)){if(0|Aa(A,1+(e>>>5)|0)){r=0|t[A+4>>2];break}return 0|(e=0)}}while(0);(0|r)<=(e>>>5|0)&&Df((0|t[A>>2])+(r<<2)|0,0,1+(e>>>5)-r<<2|0),t[A+4>>2]=1+(e>>>5)}return A=(0|t[A>>2])+(e>>>5<<2)|0,t[A>>2]=t[A>>2]|1<<(31&e),0|(e=1)}function Qa(A,e){return(0|(e|=0))<0||(0|t[(A|=0)+4>>2])<=(e>>>5|0)?0|(e=0):0|(e=(0|t[(0|t[A>>2])+(e>>>5<<2)>>2])>>>(31&e)&1)}function wa(A,e,r,i){A|=0,e|=0,r|=0,i|=0;var n=0,f=0;A:do{if((0|i)<0){for(n=i;;){if(0|t[e+(r+-1-n<<2)>>2]){i=-1;break}if(!((0|n)<-1))break A;n=n+1|0}return 0|i}}while(0);A:do{if((0|i)>0){for(;;){if(0|t[A+(i+(r+-1)<<2)>>2]){i=1;break}if(!((0|i)>1))break A;i=i+-1|0}return 0|i}}while(0);if(!r)return 0|(f=0);if((0|(i=0|t[A+(r+-1<<2)>>2]))!=(0|(n=0|t[e+(r+-1<<2)>>2])))return 0|(f=i>>>0>n>>>0?1:-1);if((0|r)<=1)return 0|(f=0);for(r=r+-2|0;(0|(i=0|t[A+(r<<2)>>2]))==(0|(n=0|t[e+(r<<2)>>2]));){if(!((0|r)>0)){i=0,f=17;break}r=r+-1|0}return 17==(0|f)?0|i:0|(f=i>>>0>n>>>0?1:-1)}function ba(A){t[(A|=0)+12>>2]=0,t[A+4>>2]=0}function ka(A,e){e|=0;var r;return 1==(0|(r=0|t[(A|=0)+4>>2]))&&(0|t[t[A>>2]>>2])==(0|e)?1:0==(r|e|0)|0}function sa(A){return 1!=(0|t[(A|=0)+4>>2])||1!=(0|t[t[A>>2]>>2])?0|(A=0):0|(A=0==(0|t[A+12>>2])&1)}function Ca(A,e){e|=0;var r;if(1==(0|(r=0|t[(A|=0)+4>>2]))){if((0|t[t[A>>2]>>2])!=(0|e))return 0|(e=0)}else if(r|e|0)return 0|(e=0);return e?0|(e=0==(0|t[A+12>>2])&1):0|(e=1)}function da(A){return(0|t[(A|=0)+4>>2])<=0?0|(A=0):0|(A=1&t[t[A>>2]>>2])}function Ia(A){return 0!=(0|t[(A|=0)+12>>2])|0}function ga(A,e,r,i){return 0|Va(A|=0,e|=0,(r|=0)+4|0,r,i|=0)}function Da(A,e,r){e|=0,r|=0,t[(A|=0)>>2]=t[e>>2],t[A+4>>2]=t[e+4>>2],t[A+8>>2]=t[e+8>>2],t[A+12>>2]=t[e+12>>2],t[A+16>>2]=1&t[A+16>>2]|r|-4&t[e+16>>2]|2}function Ma(){var A=0;return(A=0|Ln(12))?0|A:(Xe(3,143,65,0,0),0|(A=0))}function ha(A,e){e|=0,t[(A|=0)+16>>2]=t[A+16>>2]|e}function Ua(A){var e,r=0;e=(r=A|=0)+52|0;do{t[r>>2]=0,r=r+4|0}while((0|r)<(0|e));_f(A),_f(A+20|0)}function Ga(A,e,r){return 0,0|ra(A|=0,e|=0)?(ba(A+20|0),t[A+40>>2]=0|Kf(e),t[A+44>>2]=0,0|(A=1)):0|(A=0)}function Za(A,e,r,i,n){A|=0,e|=0,r|=0,i|=0;var t=0;iB(n|=0),t=0|tB(n);A:do{if(t){do{if(r){if((0|e)==(0|r)){if(0|fB(t,r,n))break;t=0;break A}if(0|Sf(t,e,r,n))break;t=0;break A}t=e}while(0);t=0|Na(0,A,t,i,n)}else t=0}while(0);return nB(n),0|t}function Na(A,e,r,i,n){A|=0,e|=0,r|=0,i|=0;var f,a,B=0,o=0,u=0;iB(n|=0),f=0|tB(n),a=0|tB(n),A||(A=0|tB(n)),o=e||0|tB(n);A:do{if(0==(0|f)|0==(0|a)|0==(0|A)|0==(0|o))A=0;else{if((0|la(r,i))<0)return ba(A),i=0!=(0|ra(o,r)),nB(n),0|(n=1&i);if(B=0|Kf(r),(0|(B=(0|(e=t[i+40>>2]<<1))>(0|B)?e:B))==(0|t[i+44>>2])?e=B:(iB(n),e=0!=(0|(e=0|tB(n)))&&0!=(0|Ea(e,B))?(e=0==(0|wB(i+20|0,0,e,i,n)))?-1:B:-1,nB(n),t[i+44>>2]=e),-1!=(0|e)&&0!=(0|nn(f,r,0|t[i+40>>2]))&&0!=(0|Sf(a,f,i+20|0,n))&&0!=(0|nn(A,a,B-(0|t[i+40>>2])|0))&&(t[(u=A+12|0)>>2]=0,0!=(0|Sf(a,i,A,n)))&&0!=(0|un(o,r,a))){t[(B=o+12|0)>>2]=0;e:do{if((0|la(o,i))>-1){for(e=0;!(e>>>0>2);){if(!(0|un(o,o,i))){A=0;break A}if(!(0|cB(A,1))){A=0;break A}if(!((0|la(o,i))>-1))break e;e=e+1|0}Xe(3,130,101,0,0),A=0;break A}}while(0);A=0|jf(o)?0:0|t[r+12>>2],t[B>>2]=A,t[u>>2]=t[i+12>>2]^t[r+12>>2],A=1}else A=0}}while(0);return nB(n),0|(n=A)}function Fa(A,e,r,i){return 0|Wa(0,A|=0,e|=0,r|=0,i|=0)}function Wa(A,e,r,n,t){A|=0,e|=0,n|=0,t|=0;var a,B=0,o=0,u=0;if(a=G,G=G+16|0,r|=0){if((0|r)>=0&&!(1==(0|r)&(0|n)>0)){u=0|Ln((r+7|0)/8|0);A:do{if(u)if(U(0|a),Zt(0,0,0),(0|Nt(u,(r+7|0)/8|0))<1)A=0;else{do{if(2==(0|A)){for(B=0;;){if((0|Nt(a+4|0,1))<1){o=19;break}if(0!=(0|B)&(A=0|i[a+4>>0])<<24>>24<0?(A=0|i[u+(B+-1)>>0],o=17):(255&A)>=42?(255&A)<84&&(A=-1,o=17):(A=0,o=17),17==(0|o)&&(o=0,i[u+B>>0]=A),(0|(B=B+1|0))>=(0|(r+7|0)/8)){o=11;break}}if(11==(0|o))break;if(19==(0|o)){A=0;break A}}}while(0);do{if((0|n)>-1){if(!n){A=255&(1<<((r+-1|0)%8|0)|0|f[u>>0]),i[u>>0]=A;break}if((r+-1|0)%8|0){A=255&(3<<((r+-1|0)%8|0)-1|0|f[u>>0]),i[u>>0]=A;break}i[u>>0]=1,i[u+1>>0]=-128|i[u+1>>0],A=1;break}A=0|i[u>>0]}while(0);i[u>>0]=255&A&(255<<1+((r+-1|0)%8|0)^255),0|t&&(i[u+(((r+7|0)/8|0)-1)>>0]=1|i[u+(((r+7|0)/8|0)-1)>>0]),A=0!=(0|fa(u,(r+7|0)/8|0,e))&1}else Xe(3,127,65,0,0),A=0}while(0);return pn(u,(r+7|0)/8|0),G=a,0|(u=A)}}else if(!(-1!=(0|n)|0!=(0|t)))return ba(e),G=a,0|(u=1);return Xe(3,127,118,0,0),G=a,0|(u=0)}function Ya(A,e){return 0|Ra(0,A|=0,e|=0)}function Ra(A,e,r){e|=0;var i,n=0,f=0;if(i=0==(0|(A|=0))?16:17,0==(0|t[(r|=0)+12>>2])&&0==(0|jf(r))){if(1==(0|(n=0|Kf(r))))return ba(e),0|(f=1);if(0==(0|Qa(r,n+-2|0))&&0==(0|Qa(r,n+-3|0))){for(A=100;;){if(!(0|MB[31&i](e,n+1|0,-1,0))){f=0,A=22;break}if((0|ca(e,r))>-1){if(!(0|cn(e,e,r))){f=0,A=22;break}if((0|ca(e,r))>-1&&0==(0|cn(e,e,r))){f=0,A=22;break}}if(!(A=A+-1|0)){A=16;break}if((0|ca(e,r))<=-1){f=1,A=22;break}}if(16==(0|A))return Xe(3,122,113,0,0),0|(f=0);if(22==(0|A))return 0|f}for(A=100;;){if(!(0|MB[31&i](e,n,-1,0))){f=0,A=22;break}if(!(A=A+-1|0)){A=20;break}if((0|ca(e,r))<=-1){f=1,A=22;break}}if(20==(0|A))return Xe(3,122,113,0,0),0|(f=0);if(22==(0|A))return 0|f}return Xe(3,122,115,0,0),0|(f=0)}function ya(A,e){return 0|Ra(1,A|=0,e|=0)}function Va(A,e,r,i,n){return i=0|ma(A|=0,e|=0,r|=0,i|=0,n|=0),aa(A),0|i}function ma(A,e,r,i,n){A|=0,e|=0,i|=0,n|=0;var f;if(((0|t[(r|=0)+4>>2])+(0|t[e+4>>2])|0)>(t[i+28>>2]<<1|0))return 0|(n=0);iB(n),f=0|tB(n);do{if(f){if((0|e)==(0|r)){if(!(0|aB(f,e,n))){e=0;break}}else if(!(0|pf(f,e,r,n))){e=0;break}e=0!=(0|Ha(A,f,i))&1}else e=0}while(0);return nB(n),0|(n=e)}function Ha(A,e,r){A|=0,e|=0;var i,n=0,f=0,a=0,B=0,u=0,l=0,c=0;if(!(u=0|t[(r|=0)+28>>2]))return t[A+4>>2]=0,0|(u=1);if(!(0|ia(e,u<<1)))return 0|(u=0);if(t[e+12>>2]=t[e+12>>2]^t[r+36>>2],i=0|t[r+24>>2],B=0|t[e>>2],n=0|t[e+4>>2],(0|u)>0){f=0;do{t[(a=B+(f<<2)|0)>>2]=t[a>>2]&f-n>>31,f=f+1|0}while((0|f)<(u<<1|0));if(t[e+4>>2]=u<<1,r=0|t[r+64>>2],(0|u)>0)for(n=0,a=0,f=B;n=(n|0!=(0|(c=(0|gn(f,i,u,0|o(0|t[f>>2],r)))+n|0)))&(c+(l=0|t[(B=f+(u<<2)|0)>>2])|0)>>>0<=l>>>0,t[B>>2]=c+l,(0|(a=a+1|0))!=(0|u);)f=f+4|0;else n=0}else t[e+4>>2]=u<<1,n=0;if(!(0|ia(A,u)))return 0|(c=0);if(t[A+4>>2]=u,t[A+12>>2]=t[e+12>>2],f=n-(0|Un(a=0|t[A>>2],r=(0|t[e>>2])+(u<<2)|0,i,u))|0,(0|u)<=0)return 0|(c=1);n=0;do{c=r+(n<<2)|0,t[(l=a+(n<<2)|0)>>2]=t[l>>2]&~f|t[c>>2]&f,t[c>>2]=0,n=n+1|0}while((0|n)!=(0|u));return 0|(n=1)}function va(A,e,r,i){A|=0,e|=0,r|=0;var n;return iB(i|=0),e=0!=(0|(n=0|tB(i)))&&0!=(0|ra(n,e))?0|Ha(A,n,r):0,nB(i),aa(A),0|e}function Ja(A,e,r,i){A|=0,e|=0,r|=0;var n;return iB(i|=0),e=0!=(0|(n=0|tB(i)))&&0!=(0|ra(n,e))?0|Ha(A,n,r):0,nB(i),0|e}function Xa(A,e,r,i){return 0|ma(A|=0,e|=0,(r|=0)+4|0,r,i|=0)}function Ta(){var A=0;return(A=0|Ln(76))?(t[A>>2]=0,_f(A+4|0),_f(A+24|0),_f(A+44|0),t[A+68>>2]=0,t[A+64>>2]=0,t[A+72>>2]=1,0|A):0|(A=0)}function La(A){(A|=0)&&(xf(A+4|0),xf(A+24|0),xf(A+44|0),1&t[A+72>>2]&&Xn(A))}function Sa(A,e,r){A|=0,r|=0;var i,n=0;if(i=G,G=G+32|0,0|jf(e|=0))return G=i,0|(r=0);iB(r),n=0|tB(r);A:do{if(0!=(0|n)&&0!=(0|ra(A+24|0,e))){0|Of(e,4)&&ha(A+24|0,4),t[A+36>>2]=0,_f(i+8|0),t[i+8>>2]=i,t[i+8+8>>2]=2,t[i+8+12>>2]=0,0|Of(e,4)&&ha(i+8|0,4),t[A>>2]=((31+(0|Kf(e))|0)/32|0)<<5,ba(A+4|0);do{if(0|Ea(A+4|0,32)){if(e=0|t[t[e>>2]>>2],t[i>>2]=e,t[i+4>>2]=0,t[i+8+4>>2]=0!=(0|e)&1,0|sa(i+8|0))ba(n);else if(!(0|Hf(n,A+4|0,i+8|0,r)))break;if(0|rn(n,n,32)){if(0|jf(n)){if(!(0|ta(n,-1)))break}else if(!(0|EB(n,1)))break;if(0|wB(n,0,n,i+8|0,r)){if(n=(0|t[n+4>>2])>0?0|t[t[n>>2]>>2]:0,t[A+64>>2]=n,t[A+68>>2]=0,ba(A+4|0),!(0|Ea(A+4|0,t[A>>2]<<1))){n=0;break A}if(!(0|wB(0,A+4|0,A+4|0,A+24|0,r))){n=0;break A}n=0|t[A+8>>2],(0|(e=0|t[A+28>>2]))>(0|n)&&Df((0|t[A+4>>2])+(n<<2)|0,0,e-n<<2|0),t[A+8>>2]=e,n=1;break A}}}}while(0);n=0}else n=0}while(0);return nB(r),G=i,0|(r=n)}function pa(A,e,r,i){A|=0,r|=0,i|=0;var n=0;return Y(e|=0),n=0|t[A>>2],y(e),0|n?0|(e=n):(n=0|Ln(76))?(t[n>>2]=0,_f(n+4|0),_f(n+24|0),_f(n+44|0),t[n+68>>2]=0,t[n+64>>2]=0,t[n+72>>2]=1,0|Sa(n,r,i)?(R(e),0|t[A>>2]?(xf(n+4|0),xf(n+24|0),xf(n+44|0),1&t[n+72>>2]|0&&Xn(n),n=0|t[A>>2]):t[A>>2]=n,y(e),0|(e=n)):(xf(n+4|0),xf(n+24|0),xf(n+44|0),1&t[n+72>>2]?(Xn(n),0|(e=0)):0|(e=0))):0|(e=0)}function za(A,e,r){A|=0,e|=0,r|=0;var i=0;A:do{if(A){switch(0|t[A>>2]){case 1:break;case 2:i=0|DB[31&t[A+8>>2]](e,r,A);break A;default:i=0;break A}(i=0|t[A+8>>2])?(WB[1&i](e,r,0|t[A+4>>2]),i=1):i=1}else i=1}while(0);return 0|i}function Ka(A,e,r,i,f,B){A|=0,r|=0,i|=0,f|=0,B|=0;var o,u=0,l=0,c=0,E=0,Q=0,w=0,b=0,k=0,s=0,C=0;if((0|(e|=0))<=3746)if((0|e)<=1344)if((0|e)<=475)if((0|e)<=399)if((0|e)<=346)if(u=(0|e)>54?27:34,(0|e)<=307){if((0|e)<2)return Xe(3,141,118,0,0),0|(C=0);if(2==(0|e)&0!=(0|r))return Xe(3,141,118,0,0),0|(C=0);s=0!=(0|r)}else u=8,C=7;else u=7,C=7;else u=6,C=7;else u=5,C=7;else u=4,C=7;else u=3,C=7;if(7==(0|C)&&(s=0!=(0|r)),0!=(0|(o=0|Sn(4096,82796)))&&0!=(0|(l=0|eB()))){if(iB(l),k=0|tB(l)){b=0;A:for(;;){e:do{if(i){if(!s){if(0|ja(A,e,i,f,l))break;C=90;break A}if(iB(l),r=0|tB(l),Q=0|tB(l),!(w=0|tB(l))){C=64;break A}if(!(0|en(w,i))){C=64;break A}if(!(0|Fa(Q,e+-1|0,0,1))){C=64;break A}if(!(0|wB(0,r,Q,w,l))){C=64;break A}if(!(0|cn(Q,Q,r))){C=64;break A}if(f){if(!(0|en(r,f))){C=64;break A}if(!(0|on(Q,Q,r))){C=64;break A}}else if(!(0|cB(Q,1))){C=64;break A}if(!(0|An(A,Q))){C=64;break A}if(!(0|cB(A,1))){C=64;break A}for(r=1;;){if(-1==(0|(c=0|uB(A,E=0|a[46480+(r<<1)>>1])))|-1==(0|(E=0|uB(Q,E)))){C=64;break A}if(r=r+1|0,0==(0|c)|0==(0|E)){if(!(0|on(A,A,i))){C=64;break A}if(!(0|on(Q,Q,w))){C=64;break A}r=1}else if(r>>>0>=2048)break}nB(l)}else{if(!(0|Fa(A,e,1,1))){C=90;break A}if((0|e)>=33)for(r=1;;){if(-1==(0|(c=0|uB(A,0|a[46480+(r<<1)>>1])))){u=0;break A}if(n[o+(r<<1)>>1]=c,(r=r+1|0)>>>0>=2048){r=0;r:do{for(c=1;!((((r+(0|a[o+(c<<1)>>1])|0)>>>0)%((0|a[46480+(c<<1)>>1])>>>0)|0)>>>0<2);)if((c=c+1|0)>>>0>=2048){C=43;break r}r=r+2|0}while(r>>>0<=4294949432);if(43==(0|C)){if(C=0,!(0|cB(A,r))){u=0;break A}if((0|Kf(A))==(0|e))break e}if(!(0|Fa(A,e,1,1))){u=0;break A}r=1}}for(w=-17864;;){c=1;do{if(-1==(0|(r=0|uB(A,0|a[46480+(c<<1)>>1])))){u=0;break A}n[o+(c<<1)>>1]=r,c=c+1|0}while(c>>>0<2048);r=0|na(A),w=(r=32==(0|e)?~r:(1<<e)-1-r|0)>>>0<w>>>0?r:w,r=0;r:do{for(c=0|na(A),Q=1;;){if(c>>>0<=(E=0|a[46480+(Q<<1)>>1])>>>0){C=33;break r}if(!(((r+(0|a[o+(Q<<1)>>1])|0)>>>0)%(E>>>0)|0))break;if((Q=Q+1|0)>>>0>=2048){C=33;break r}}r=r+2|0}while(r>>>0<=w>>>0);if(33==(0|C)){if(C=0,!(0|cB(A,r))){u=0;break A}if((0|Kf(A))==(0|e))break e}if(!(0|Fa(A,e,1,1))){u=0;break A}}}}while(0);E=b+1|0;e:do{if(0|B){switch(0|t[B>>2]){case 1:break;case 2:if(0|DB[31&t[B+8>>2]](0,b,B))break e;C=90;break A;default:C=90;break A}0|(r=0|t[B+8>>2])&&WB[1&r](0,b,0|t[B+4>>2])}}while(0);e:do{if(s){if(!(0|en(k,A))){C=90;break A}if(!B)for(r=0;;){switch(0|xa(A,1,l,0,0)){case-1:u=0;break A;case 0:break e}switch(0|xa(k,1,l,0,0)){case-1:u=0;break A;case 0:break e}if((r=r+1|0)>>>0>=u>>>0){u=1;break A}}for(c=0;;){switch(0|xa(A,1,l,0,B)){case-1:u=0;break A;case 0:break e}switch(0|xa(k,1,l,0,B)){case-1:u=0;break A;case 0:break e}switch(0|t[B>>2]){case 1:0|(r=0|t[B+8>>2])&&WB[1&r](2,b,0|t[B+4>>2]);break;case 2:if(!(0|DB[31&t[B+8>>2]](2,b,B))){u=0;break A}break;default:u=0;break A}if((c=c+1|0)>>>0>=u>>>0){u=1;break A}}}else switch(0|xa(A,u,l,0,B)){case-1:C=90;break A;case 0:break;default:u=1;break A}}while(0);b=E}64==(0|C)?(nB(l),u=0):90==(0|C)&&(u=0),Xn(o)}else Xn(o),u=0;nB(l)}else Xn(o),l=0,u=0;return rB(l),0|(C=u)}function ja(A,e,r,i,n){A|=0,e|=0,r|=0,i|=0;var t=0;iB(n|=0),t=0|tB(n);A:do{if(0!=(0|t)&&0!=(0|Fa(A,e,0,1))&&0!=(0|wB(0,t,A,r,n))&&0!=(0|cn(A,A,t))){if(i){if(!(0|on(A,A,i))){e=0;break}}else if(!(0|cB(A,1))){e=0;break}for(e=1;;){if(-1==(0|(t=0|uB(A,0|a[46480+(e<<1)>>1])))){e=0;break A}if(e=e+1|0,t>>>0<2){if(!(0|on(A,A,r))){e=0;break A}e=1}else if(e>>>0>=2048){e=1;break A}}}else e=0}while(0);return nB(n),0|e}function xa(A,e,r,i,n){e|=0,r|=0,i|=0,n|=0;var f=0,B=0,o=0,u=0,l=0,c=0,E=0;if((0|ca(A|=0,52964))<1)return 0|(r=0);if(E=e||((0|Kf(A))<=3746?(0|Kf(A))<=1344?(0|Kf(A))<=475?(0|Kf(A))<=399?(0|Kf(A))<=346?(0|Kf(A))>307?8:(E=(0|Kf(A))>54)?27:34:7:6:5:4:3),!(0|da(A)))return 0|(r=0|Ca(A,2));A:do{if(i){e=1;e:do{switch(7&(-1==(0|(c=0|uB(A,0|a[46480+(e<<1)>>1])))?5:0==(0|c)&1)){case 5:f=0,e=-1;break A;case 0:break;default:e=0,B=67;break e}e=e+1|0}while(e>>>0<2048);if(67==(0|B))return 0|e;if(n){switch(0|t[n>>2]){case 1:break;case 2:if(0|DB[31&t[n+8>>2]](1,-1,n)){B=21;break A}f=0,e=-1;break A;default:f=0,e=-1;break A}(e=0|t[n+8>>2])?(WB[1&e](1,-1,0|t[n+4>>2]),B=21):B=21}else B=21}else B=21}while(0);do{if(21==(0|B)){if(r)c=r;else{if(!(e=0|eB())){f=0,e=-1;break}c=e}if(iB(c),o=0|tB(c),u=0|tB(c),0!=(0|(l=0|tB(c)))&&0!=(0|ra(o,A))&&0!=(0|EB(o,1)))if(0|jf(o))f=0,e=0;else{for(e=1;!(0|Qa(o,e));)e=e+1|0;A:do{if(0!=(0|nn(u,o,e))&&(f=0|Ta(),0!=(0|f)))if(0|Sa(f,A,c))if((0|E)>0){if(!(i=e+-1|0))for(i=0;;){if(!(0|ya(l,o))){e=-1;break A}if(!(0|cB(l,1))){e=-1;break A}if(!(0|Pa(l,l,u,A,c,f))){e=-1;break A}if(0==(0|sa(l))&&0|ca(l,o)){e=0;break A}e:do{if(0|n){switch(0|t[n>>2]){case 1:break;case 2:if(0|DB[31&t[n+8>>2]](1,i,n))break e;e=-1;break A;default:e=-1;break A}if(!(e=0|t[n+8>>2]))break;WB[1&e](1,i,0|t[n+4>>2])}}while(0);if((0|(i=i+1|0))>=(0|E)){e=1;break A}}for(B=0;;){if(!(0|ya(l,o))){e=-1;break A}if(!(0|cB(l,1))){e=-1;break A}if(!(0|Pa(l,l,u,A,c,f))){e=-1;break A}e:do{if(!(0|sa(l))){if(!(0|ca(l,o)))break;for(e=i;;){if(!(0|AB(l,l,l,A,c))){e=-1;break A}if(0|sa(l)){e=0;break A}if(!(0|ca(l,o)))break e;if(!(e=e+-1|0)){e=0;break A}}}}while(0);e:do{if(0|n){switch(0|t[n>>2]){case 1:break;case 2:if(0|DB[31&t[n+8>>2]](1,B,n))break e;e=-1;break A;default:e=-1;break A}if(!(e=0|t[n+8>>2]))break;WB[1&e](1,B,0|t[n+4>>2])}}while(0);if((0|(B=B+1|0))>=(0|E)){e=1;break}}}else e=1;else e=-1;else f=0,e=-1}while(0);if(!c)break}else f=0,e=-1;nB(c),r||rB(c)}}while(0);return La(f),0|(r=e)}function Oa(A,e,r,i,n){return A|=0,e|=0,r|=0,n|=0,0|da(i|=0)?1==(0|t[e+4>>2])&&0==(0|t[e+12>>2])&&0==(0|Of(r,4))&&0==(0|Of(e,4))&&0==(0|Of(i,4))?0|(A=0|function(A,e,r,i,n,f){A|=0,e|=0,i|=0,n|=0,f|=0;var a=0,B=0,u=0,l=0,c=0,E=0,Q=0,w=0,b=0,k=0;if(0==(0|Of(r|=0,4))&&0==(0|Of(i,4))){if(!(0|da(i)))return Xe(3,117,102,0,0),0|(k=0);if(1==(0|t[i+4>>2])&&(e=(e>>>0)%((0|t[t[i>>2]>>2])>>>0)|0),!(B=0|Kf(r)))return 0|ka(i,1)?(ba(A),0|(k=1)):0|(k=0|ta(A,1));if(!e)return ba(A),0|(k=1);iB(n),w=0|tB(n),u=0|tB(n),l=0|tB(n);A:do{if(0==(0|w)|0==(0|u)|0==(0|l))a=0,e=0,k=46;else{if(f)a=f;else{if(!(a=0|Ta())){a=0,e=0,k=47;break}if(!(0|Sa(a,i,n))){e=0,k=47;break}}if((0|B)>1)for(w=B+-2|0,c=1,Q=e;;){B=0|o(Q,Q),E=0==(0|c);do{if((0|(B>>>0)/(Q>>>0))!=(0|Q)){if(E){if(!(0|QB(u,Q))){e=0,k=46;break A}if(0|wB(0,l,u,i,n)){B=1,c=l,k=27;break}e=0,k=46;break A}if(!(0|ta(u,Q))){e=0,k=46;break A}if(0|ga(u,u,a,n)){B=1,c=u,u=l,k=27;break}e=0,k=46;break A}E?(c=u,u=l,k=27):(Q=u,u=1)}while(0);if(27==(0|k)){if(k=0,!(0|Va(c,c,c,a,n))){e=0,k=46;break A}Q=c,c=0,l=u,u=0}do{if(0|Qa(r,w)){if((0|((E=0|o(B,e))>>>0)/(e>>>0))!=(0|B)){if(u){if(!(0|ta(Q,B))){e=0,k=46;break A}if(0|ga(Q,Q,a,n)){u=Q,c=0,B=e;break}e=0,k=46;break A}if(!(0|QB(Q,B))){e=0,k=46;break A}if(0|wB(0,l,Q,i,n)){u=l,l=Q,B=e;break}e=0,k=46;break A}u=Q,B=E}else u=Q}while(0);if(!((0|w)>0)){e=B;break}w=w+-1|0,Q=B}else c=1;B=0==(0|c);do{if(1!=(0|e)){if(B){if(!(0|QB(u,e))){e=0,k=46;break A}if(0|wB(0,l,u,i,n)){b=l,k=44;break}e=0,k=46;break A}if(!(0|ta(u,e))){e=0,k=46;break A}if(0|ga(u,u,a,n)){b=u,k=44;break}e=0,k=46;break A}if(B)b=u,k=44;else if(!(0|ta(A,1))){e=0,k=46;break A}}while(0);if(44==(0|k)&&0==(0|va(A,b,a,n))){e=0,k=46;break}e=1,k=46}}while(0);return 46==(0|k)&&(f||(k=47)),47==(0|k)&&La(a),nB(n),0|(k=e)}return Xe(3,117,66,0,0),0|(k=0)}(A,0|t[t[e>>2]>>2],r,i,n,0)):0|(A=0|Pa(A,e,r,i,n,0)):0|(A=0|function(A,e,r,i,n){A|=0,e|=0,i|=0,n|=0;var f,a=0,B=0,o=0,u=0,l=0,c=0;if(f=G,G=G+192|0,0==(0|Of(r=r|0,4))&&0==(0|Of(e,4))?0==(0|Of(i,4)):0){if(!(l=0|Kf(r)))return 0|ka(i,1)?(ba(A),G=f,0|(n=1)):(n=0|ta(A,1),G=f,0|n);iB(n),u=0|tB(n),a=0|tB(n),t[f>>2]=a;A:do{if(0==(0|u)|0==(0|a))e=0;else{if(Ua(f+128|0),0|t[i+12>>2]){if(!(0|ra(u,i))){e=0;break}if(t[u+12>>2]=0,(0|Ga(f+128|0,u))<1){e=0;break}}else if((0|Ga(f+128|0,i))<1){e=0;break}if(0|$a(a,e,i,n)){if(0|jf(a)){ba(A),e=1;break}if((0|l)<=671?(0|l)<=239?(0|l)<=79?(0|l)>23?(B=3,o=21):B=1:(B=4,o=21):(B=5,o=21):(B=6,o=21),21==(0|o)){if(!(0|Za(u,a,a,f+128|0,n))){e=0;break}if((0|(a=1<<B+-1))>1){i=1;do{if(e=0|tB(n),t[f+(i<<2)>>2]=e,!e){e=0;break A}if(!(0|Za(e,0|t[f+(i+-1<<2)>>2],u,f+128|0,n))){e=0;break A}i=i+1|0}while((0|i)<(0|a))}}if(0|ta(A,1)){if(B>>>0<=1)for(i=0|t[f>>2],a=1,e=l+-1|0;;){e:do{if(!(0|Qa(r,e))){if(0|a)for(;;){if(!e){e=1;break A}if(0|Qa(r,e=e+-1|0))break e}do{if(!(0|Za(A,A,A,f+128|0,n))){e=0;break A}if(!e){e=1;break A}e=e+-1|0}while(0==(0|Qa(r,e)))}}while(0);if(0==(0|a)&&0==(0|Za(A,A,A,f+128|0,n))){e=0;break A}if(!(0|Za(A,A,i,f+128|0,n))){e=0;break A}if((0|(e=e+-1|0))<0){e=1;break A}a=0}for(c=1,e=l+-1|0;;){e:do{if(!(0|Qa(r,e))){if(0|c)for(;;){if(!e){e=1;break A}if(0|Qa(r,e=e+-1|0))break e}do{if(!(0|Za(A,A,A,f+128|0,n))){e=0;break A}if(!e){e=1;break A}e=e+-1|0}while(0==(0|Qa(r,e)))}}while(0);for(u=1,o=0,a=1;!((0|(i=e-u|0))<0);){if(l=0==(0|Qa(r,i)),i=l?o:u,a=l?a:a<<u-o|1,(u=u+1|0)>>>0>=B>>>0){o=i;break}o=i}if(!(0!=(0|c)|(0|o)<0))for(i=0;;){if(!(0|Za(A,A,A,f+128|0,n))){e=0;break A}if(!((0|i)<(0|o)))break;i=i+1|0}if(!(0|Za(A,A,0|t[f+(a>>1<<2)>>2],f+128|0,n))){e=0;break A}if((0|(e=e+-1-o|0))<0){e=1;break}c=0}}else e=0}else e=0}}while(0);return nB(n),function(A){(A|=0)&&(Pf(A),Pf(A+20|0),1&t[A+48>>2]&&Xn(A))}(f+128|0),G=f,0|(n=e)}return Xe(3,125,66,0,0),G=f,0|(n=0)}(A,e,r,i,n))}function Pa(A,e,r,i,n,f){A|=0,e|=0,i|=0,n|=0,f|=0;var a,B=0,o=0,u=0,l=0,c=0,E=0,Q=0,w=0;if(a=G,G=G+128|0,0==(0|Of(r|=0,4))&&0==(0|Of(e,4))&&0==(0|Of(i,4))){if(!(0|da(i)))return Xe(3,109,102,0,0),G=a,0|(n=0);if(!(E=0|Kf(r)))return 0|ka(i,1)?(ba(A),G=a,0|(n=1)):(n=0|ta(A,1),G=a,0|n);iB(n),c=0|tB(n),w=0|tB(n),o=0|tB(n),t[a>>2]=o;A:do{if(0==(0|c)|0==(0|w)|0==(0|o))o=0,e=0,u=73;else{if(f)B=f;else{if(!(B=0|Ta())){B=0,e=0,u=74;break}if(!(0|Sa(B,i,n))){e=0,u=74;break}}if(!(0==(0|t[e+12>>2])&&(0|la(e,i))<=-1)){if(!(0|$a(o,e,i,n))){o=B,e=0,u=73;break}e=o}if(0|jf(e)){ba(A),o=B,e=1,u=73;break}if(0|Xa(o,e,B,n)){if((0|E)<=671?(0|E)<=239?(0|E)<=79?(0|E)>23?(l=3,u=25):Q=1:(l=4,u=25):(l=5,u=25):(l=6,u=25),25==(0|u)){if(!(0|ma(c,o,o,B,n))){o=B,e=0,u=73;break}if((0|(u=1<<l+-1))>1)for(o=1;;){if(e=0|tB(n),t[a+(o<<2)>>2]=e,!e){o=B,e=0,u=73;break A}if(!(0|ma(e,0|t[a+(o+-1<<2)>>2],c,B,n))){o=B,e=0,u=73;break A}if((0|(o=o+1|0))>=(0|u)){Q=l;break}}else Q=l}if(l=0|t[i+4>>2],(0|t[(0|t[i>>2])+(l+-1<<2)>>2])>=0){if(!(0|Xa(w,52964,B,n))){o=B,e=0,u=73;break}}else{if(!(0|ia(w,l))){o=B,e=0,u=73;break}if(e=0|t[i>>2],o=0|t[w>>2],t[o>>2]=0-(0|t[e>>2]),(0|l)>1){u=1;do{t[o+(u<<2)>>2]=~t[e+(u<<2)>>2],u=u+1|0}while((0|u)!=(0|l))}t[w+4>>2]=l}e:do{if(Q>>>0>1)for(i=1,e=E+-1|0;;){r:do{if(!(0|Qa(r,e))){if(0|i)for(;;){if(!e)break e;if(0|Qa(r,e=e+-1|0))break r}do{if(!(0|ma(w,w,w,B,n))){o=B,e=0,u=73;break A}if(!e)break e;e=e+-1|0}while(0==(0|Qa(r,e)))}}while(0);for(c=1,l=0,u=1;!((0|(o=e-c|0))<0);){if(o=(E=0==(0|Qa(r,o)))?l:c,u=E?u:u<<c-l|1,(c=c+1|0)>>>0>=Q>>>0){l=o;break}l=o}if(!(0!=(0|i)|(0|l)<0))for(o=0;;){if(!(0|ma(w,w,w,B,n))){o=B,e=0,u=73;break A}if(!((0|o)<(0|l)))break;o=o+1|0}if(!(0|ma(w,w,0|t[a+(u>>1<<2)>>2],B,n))){o=B,e=0,u=73;break A}if((0|(e=e+-1-l|0))<0)break;i=0}else for(o=0|t[a>>2],u=1,e=E+-1|0;;){r:do{if(!(0|Qa(r,e))){if(0|u)for(;;){if(!e)break e;if(0|Qa(r,e=e+-1|0))break r}do{if(!(0|ma(w,w,w,B,n))){o=B,e=0,u=73;break A}if(!e)break e;e=e+-1|0}while(0==(0|Qa(r,e)))}}while(0);if(0==(0|u)&&0==(0|ma(w,w,w,B,n))){o=B,e=0,u=73;break A}if(!(0|ma(w,w,o,B,n))){o=B,e=0,u=73;break A}if((0|(e=e+-1|0))<0)break;u=0}}while(0);o=B,e=0!=(0|va(A,w,B,n))&1,u=73}else o=B,e=0,u=73}}while(0);return 73==(0|u)&&(f||(B=o,u=74)),74==(0|u)&&La(B),nB(n),G=a,0|(n=e)}return n=0|_a(A,e,r,i,n,f),G=a,0|n}function _a(A,e,r,i,n,f){A|=0,e|=0,r|=0,n|=0,f|=0;var a,B,o,u=0,l=0,c=0,E=0,Q=0,w=0,b=0,k=0,s=0,C=0,d=0;if(o=G,G=G+48|0,!(0|da(i|=0)))return Xe(3,124,102,0,0),G=o,0|(n=0);if(B=0|t[i+4>>2],!(a=0|t[r+4>>2]))return 0|ka(i,1)?(ba(A),G=o,0|(n=1)):(n=0|ta(A,1),G=o,0|n);if(iB(n),f?(Q=f,d=10):(u=0|Ta())?0|Sa(u,i,n)?(Q=u,d=10):(Q=u,c=0,E=0,l=0,u=0,d=59):(Q=0,c=0,E=0,l=0,u=0,d=59),10==(0|d)){(0|(l=((B<<1|0)>(0|(s=1<<(C=(0|a)<=29?(0|a)>9?5:(0|a)>2?4:(0|a)>0?3:1:6)))?B<<1:s)+(B<<C)<<2))>=3072?(u=0|Ln(l+64|0))?d=16:(c=0,E=0,u=0):(u=G,G=G+(15+(1*(l+64|0)|0)&-16)|0,d=16);A:do{if(16==(0|d)){if(Df(0|(c=u+(64-(63&u))|0),0,0|l),E=(0|l)<3072?0:u,w=c+(B<<2<<C)|0,t[o+20>>2]=w,t[o>>2]=w+(B<<2),t[o+4>>2]=0,t[(k=o+20+4|0)>>2]=0,t[o+8>>2]=B,t[o+20+8>>2]=B,t[o+12>>2]=0,t[o+20+12>>2]=0,t[o+16>>2]=2,t[o+20+16>>2]=2,u=0|t[i>>2],(0|t[u+(B+-1<<2)>>2])>=0){if(!(0|Xa(o+20|0,52964,Q,n))){u=0;break}}else{if(t[w>>2]=0-(0|t[u>>2]),(0|B)>1){b=1;do{t[w+(b<<2)>>2]=~t[u+(b<<2)>>2],b=b+1|0}while((0|b)!=(0|B))}t[k>>2]=B}if(0==(0|t[e+12>>2])&&(0|la(e,i))<=-1){if(!(0|Xa(o,e,Q,n))){u=0;break}}else{if(!(0|wB(0,o,e,i,n))){u=0;break}if(!(0|Xa(o,o,Q,n))){u=0;break}}if(e=0|t[o+20>>2],(0|(b=(0|(b=0|t[k>>2]))<(0|B)?b:B))>0)for(u=0,w=0;t[c+(w<<2)>>2]=t[e+(u<<2)>>2],!((0|(u=u+1|0))>=(0|b));)w=w+s|0;if(u=0|t[o>>2],(0|(e=(0|(e=0|t[o+4>>2]))<(0|B)?e:B))>0)for(w=0,b=1;t[c+(b<<2)>>2]=t[u+(w<<2)>>2],!((0|(w=w+1|0))>=(0|e));)b=b+s|0;if(C>>>0>1){if(!(0|ma(o+20|0,o,o,Q,n))){u=0;break}if(e=0|t[o+20>>2],(0|(b=(0|(b=0|t[k>>2]))<(0|B)?b:B))>0)for(u=0,w=2;t[c+(w<<2)>>2]=t[e+(u<<2)>>2],!((0|(u=u+1|0))>=(0|b));)w=w+s|0;if((0|s)>3){w=3;do{if(!(0|ma(o+20|0,o,o+20|0,Q,n))){u=0;break A}if(i=0|t[o+20>>2],(0|(e=(0|(e=0|t[k>>2]))<(0|B)?e:B))>0)for(u=0,b=w;t[c+(b<<2)>>2]=t[i+(u<<2)>>2],!((0|(u=u+1|0))>=(0|e));)b=b+s|0;w=w+1|0}while((0|w)<(0|s))}}if((0|(u=((a<<5)-1|0)%(0|C)|0))>-1)for(w=(a<<5)-1|0,b=u,u=0;u=(0|Qa(r,w))+(u<<1)|0,w=w+-1|0,(0|b)>0;)b=b+-1|0;else w=(a<<5)-1|0,u=0;if(0|qa(o+20|0,B,c,u,C)){e:do{if((0|w)>-1)for(b=0,u=0;;){if(!(0|ma(o+20|0,o+20|0,o+20|0,Q,n))){u=0;break A}if(u=(0|Qa(r,w))+(u<<1)|0,e=w+-1|0,(b=b+1|0)>>>0>=C>>>0){if(!(0|qa(o,B,c,u,C))){u=0;break A}if(!(0|ma(o+20|0,o+20|0,o,Q,n))){u=0;break A}if(!((0|w)>0))break e;b=0,u=0}w=e}}while(0);u=0!=(0|va(A,o+20|0,Q,n))&1}else u=0}}while(0);f||(d=59)}return 59==(0|d)&&La(Q),0|c&&($r(c,l),Xn(E)),nB(n),G=o,0|(n=u)}function qa(A,e,r,i,n){r|=0,i|=0,n|=0;var f=0,a=0,B=0,o=0,u=0;if(!(0|ia(A|=0,e|=0)))return 0|(e=0);A:do{if((0|n)<4){if((0|e)>0)for(B=0;;){f=0,a=0;do{f=t[r+(a<<2)>>2]&((a^i)-1&(-2147483648^i))>>31|f,a=a+1|0}while((0|a)<(1<<n|0));if(t[(0|t[A>>2])+(B<<2)>>2]=f,(0|(B=B+1|0))==(0|e))break;r=r+(1<<n<<2)|0}}else if(o=1<<n+-2,u=i>>n+-2,(0|e)>0){if(31==(n+-2|0))for(f=0|t[A>>2],a=0;;)if(t[f+(a<<2)>>2]=0,(0|(a=a+1|0))==(0|e))break A;for(B=0;;){f=0,a=0;do{f=(t[r+(a+o<<2)>>2]&((1^u)-1&(-2147483648^u))>>31|t[r+(a<<2)>>2]&(u+-1&(-2147483648^u))>>31|t[r+(a+(o<<1)<<2)>>2]&((2^u)-1&(-2147483648^u))>>31|t[r+(a+(3<<n+-2)<<2)>>2]&((3^u)-1&(-2147483648^u))>>31)&((a^o+-1&i)-1&(o+-1&i^-2147483648))>>31|f,a=a+1|0}while((0|a)<(0|o));if(t[(0|t[A>>2])+(B<<2)>>2]=f,(0|(B=B+1|0))==(0|e))break;r=r+(1<<n<<2)|0}}}while(0);return t[A+4>>2]=e,0|(e=1)}function $a(A,e,r,i){return 0|wB(0,A|=0,e|=0,r|=0,i|=0)?0|t[A+12>>2]?0|(A=0|DB[31&(0==(0|t[r+12>>2])?18:19)](A,A,r)):0|(A=1):0|(A=0)}function AB(A,e,r,i,n){A|=0,e|=0,r|=0,i|=0;var f,a=0,B=0;iB(n|=0),f=0|tB(n);do{if(f){if((0|e)==(0|r)){if(!(0|fB(f,e,n))){e=0;break}if(0|wB(0,A,f,i,n)){if(!(0|t[A+12>>2])){e=1;break}a=0|DB[31&(0==(0|t[i+12>>2])?18:19)](A,A,i),B=11}}else{if(!(0|Sf(f,e,r,n))){e=0;break}if(0|wB(0,A,f,i,n)){if(!(0|t[A+12>>2])){e=1;break}a=0|DB[31&(0==(0|t[i+12>>2])?18:19)](A,A,i),B=11}}if(11==(0|B)&&0|a){e=1;break}e=0}else e=0}while(0);return nB(n),0|e}function eB(){var A=0;return(A=0|Sn(48,82796))?(t[A>>2]=0,t[A+4>>2]=0,t[A+8>>2]=0,t[A+12>>2]=0,t[A+16>>2]=0,t[A+20>>2]=0,t[A+24>>2]=0,t[A+28>>2]=0,0|A):(Xe(3,106,65,0,0),0|(A=0))}function rB(A){var e=0,r=0;if(A|=0){if(Xn(0|t[A+20>>2]),t[A+20>>2]=0,0|(e=0|t[A>>2]))do{0|t[e>>2]&&xf(e),0|t[(r=e+20|0)>>2]&&xf(r),0|t[(r=e+40|0)>>2]&&xf(r),0|t[(r=e+60|0)>>2]&&xf(r),0|t[(r=e+80|0)>>2]&&xf(r),0|t[(r=e+100|0)>>2]&&xf(r),0|t[(r=e+120|0)>>2]&&xf(r),0|t[(r=e+140|0)>>2]&&xf(r),0|t[(r=e+160|0)>>2]&&xf(r),0|t[(r=e+180|0)>>2]&&xf(r),0|t[(r=e+200|0)>>2]&&xf(r),0|t[(r=e+220|0)>>2]&&xf(r),0|t[(r=e+240|0)>>2]&&xf(r),0|t[(r=e+260|0)>>2]&&xf(r),0|t[(r=e+280|0)>>2]&&xf(r),0|t[(e=e+300|0)>>2]&&xf(e),e=0|t[A>>2],t[A+4>>2]=t[e+324>>2],Xn(e),e=0|t[A+4>>2],t[A>>2]=e}while(0!=(0|e));Xn(A)}}function iB(A){var e=0,r=0,i=0,n=0;if(0==(0|(e=0|t[(A|=0)+36>>2]))&&0==(0|t[A+40>>2])){if(n=0|t[A+32>>2],(0|(e=0|t[A+24>>2]))==(0|t[A+28>>2])){if(!(r=0|Ln((i=0==(0|e)?32:(3*e|0)>>>1)<<2)))return Xe(3,129,109,0,0),void(t[A+36>>2]=1+(0|t[A+36>>2]));(e=0|t[A+24>>2])?(If(0|r,0|t[A+20>>2],e<<2|0),e=A+20|0):e=A+20|0,Xn(0|t[e>>2]),t[e>>2]=r,t[A+28>>2]=i,e=0|t[A+24>>2]}else r=0|t[A+20>>2];return t[A+24>>2]=e+1,void(t[r+(e<<2)>>2]=n)}t[A+36>>2]=e+1}function nB(A){var e=0,r=0,i=0,n=0;if(0|(e=0|t[(A|=0)+36>>2]))t[A+36>>2]=e+-1;else{if(e=0|t[A+20>>2],n=(0|t[A+24>>2])-1|0,t[A+24>>2]=n,n=0|t[e+(n<<2)>>2],(e=0|t[A+32>>2])>>>0>n>>>0&&(r=0|t[A+12>>2],t[A+12>>2]=r-(e-n),e-n|0)){i=e-n|0,e=r+15&15;do{i=i+-1|0,e?e=e+-1|0:(t[A+4>>2]=t[320+(0|t[A+4>>2])>>2],e=15)}while(0!=(0|i))}t[A+32>>2]=n,t[A+40>>2]=0}}function tB(A){var e,r=0,i=0,n=0;return 0|t[(A|=0)+36>>2]||0|t[A+40>>2]?0|(A=0):(i=0|t[A+44>>2],(0|(e=0|t[A+12>>2]))==(0|t[A+16>>2])?(r=0|Ln(328))?(_f(r),8&i?(ha(r,8),_f(r+20|0),ha(r+20|0,8),_f(r+40|0),ha(r+40|0,8),_f(r+60|0),ha(r+60|0,8),_f(r+80|0),ha(r+80|0,8),_f(r+100|0),ha(r+100|0,8),_f(r+120|0),ha(r+120|0,8),_f(r+140|0),ha(r+140|0,8),_f(r+160|0),ha(r+160|0,8),_f(r+180|0),ha(r+180|0,8),_f(r+200|0),ha(r+200|0,8),_f(r+220|0),ha(r+220|0,8),_f(r+240|0),ha(r+240|0,8),_f(r+260|0),ha(r+260|0,8),_f(r+280|0),ha(r+280|0,8),_f(r+300|0),ha(r+300|0,8)):(_f(r+20|0),_f(r+40|0),_f(r+60|0),_f(r+80|0),_f(r+100|0),_f(r+120|0),_f(r+140|0),_f(r+160|0),_f(r+180|0),_f(r+200|0),_f(r+220|0),_f(r+240|0),_f(r+260|0),_f(r+280|0),_f(r+300|0)),i=0|t[A+8>>2],t[r+320>>2]=i,t[r+324>>2]=0,0|t[A>>2]?(t[i+324>>2]=r,t[A+8>>2]=r,i=A+4|0):(t[A+8>>2]=r,t[A+4>>2]=r,i=A),t[i>>2]=r,t[A+16>>2]=16+(0|t[A+16>>2]),t[A+12>>2]=1+(0|t[A+12>>2]),n=19):n=18:(e?(r=0|t[A+4>>2],15&e?i=15&e:(r=r+324|0,i=A+4|0,n=16)):(r=A,i=A+4|0,n=16),16==(0|n)&&(r=0|t[r>>2],t[i>>2]=r,i=0),t[A+12>>2]=e+1,n=(r=r+(20*i|0)|0)?19:18),18==(0|n)?(t[A+40>>2]=1,Xe(3,116,109,0,0),0|(A=0)):19==(0|n)?(ba(r),t[(n=r+16|0)>>2]=-5&t[n>>2],t[A+32>>2]=1+(0|t[A+32>>2]),0|(A=r)):0)}function fB(A,e,r){return r=0|aB(A|=0,e|=0,r|=0),aa(A),0|r}function aB(A,e,r){A|=0,r|=0;var i,n,f=0,a=0;if(n=G,G=G+128|0,(0|(f=0|t[(e|=0)+4>>2]))<1)return t[A+4>>2]=0,t[A+12>>2]=0,G=n,0|(r=1);iB(r),a=(0|e)==(0|A)?0|tB(r):A,i=0|tB(r);A:do{if(0!=(0|a)&0!=(0|i)&&0!=(0|ia(a,f<<1))){e:do{switch(0|f){case 4:mf(0|t[a>>2],0|t[e>>2]);break;case 8:Vf(0|t[a>>2],0|t[e>>2]);break;default:if((0|f)<16){BB(0|t[a>>2],0|t[e>>2],f,n);break e}if((0|f)==(1<<(0|zf(f))-1|0)){if(!(0|ia(i,f<<2))){f=0;break A}oB(0|t[a>>2],0|t[e>>2],f,0|t[i>>2]);break e}if(!(0|ia(i,f<<1))){f=0;break A}BB(0|t[a>>2],0|t[e>>2],f,0|t[i>>2]);break e}}while(0);if(t[a+12>>2]=0,t[a+4>>2]=f<<1,(0|a)!=(0|A)&&0==(0|ra(A,a))){f=0;break}f=1}else f=0}while(0);return nB(r),G=n,0|(r=f)}function BB(A,e,r,i){e|=0,i|=0;var n=0,f=0,a=0,B=0,o=0;if(t[(A|=0)+(((r|=0)<<1)-1<<2)>>2]=0,t[A>>2]=0,(0|r)>1&&(t[A+(r<<2)>>2]=0|Dn(A+4|0,e+4|0,r+-1|0,0|t[e>>2]),2!=(0|r)))for(n=e+4|0,f=r+-2|0,a=r+-1|0,B=A+12|0;o=n,n=n+4|0,t[B+((a=a+-1|0)<<2)>>2]=0|gn(B,n,a,0|t[o>>2]),!((0|f)<=1);)f=f+-1|0,B=B+8|0;hn(A,A,A,r<<1),function(A,e,r){A|=0,e|=0;var i=0,n=0,f=0,a=0,B=0;if(!((0|(r|=0))<1)){if(r>>>0>3){for(i=e,n=r,f=A;a=0|lf(0|(a=0|t[i>>2]),0,0|a,0),B=0|E(),t[f>>2]=a,t[f+4>>2]=B,B=0|lf(0|(B=0|t[i+4>>2]),0,0|B,0),a=0|E(),t[f+8>>2]=B,t[f+12>>2]=a,a=0|lf(0|(a=0|t[i+8>>2]),0,0|a,0),B=0|E(),t[f+16>>2]=a,t[f+20>>2]=B,B=0|lf(0|(B=0|t[i+12>>2]),0,0|B,0),a=0|E(),t[f+24>>2]=B,t[f+28>>2]=a,!((n=n+-4|0)>>>0<=3);)i=i+16|0,f=f+32|0;if(!(i=r+-4-(r+-4&-4)|0))return;e=e+(4+(r+-4&-4)<<2)|0,A=A+(8+((r+-4|0)>>>2<<3)<<2)|0}else i=r;for(;a=0|lf(0|(a=0|t[e>>2]),0,0|a,0),B=0|E(),t[A>>2]=a,t[A+4>>2]=B,i=i+-1|0;)e=e+4|0,A=A+8|0}}(i,e,r),hn(A,A,i,r<<1)}function oB(A,e,r,i){A|=0,e|=0,i|=0;var n=0,f=0,a=0,B=0;switch(0|(r|=0)){case 4:return void mf(A,e);case 8:return void Vf(A,e);default:if((0|r)<16)return void BB(A,e,r,i);n=0|function(A,e,r){e|=0;var i=0,n=0,f=0;if(!(r|=0))return 0|(f=0);if((0|(i=0|t[(A|=0)+(r+-1<<2)>>2]))!=(0|(n=0|t[e+(r+-1<<2)>>2])))return 0|(f=i>>>0>n>>>0?1:-1);if((0|r)<=1)return 0|(f=0);for(r=r+-2|0;(0|(i=0|t[A+(r<<2)>>2]))==(0|(n=0|t[e+(r<<2)>>2]));){if(!((0|r)>0)){i=0,f=9;break}r=r+-1|0}return 9==(0|f)?0|i:0|(f=i>>>0>n>>>0?1:-1)}(e,a=e+(((0|r)/2|0)<<2)|0,(0|r)/2|0);do{if((0|n)<=0){if((0|n)<0){Un(i,a,e,(0|r)/2|0),B=10;break}Df(i+(r<<2)|0,0,r<<2|0),n=i+(r<<1<<2)|0,f=i+(r<<2)|0;break}Un(i,e,a,(0|r)/2|0),B=10}while(0);if(10==(0|B)&&(oB(i+(r<<2)|0,i,(0|r)/2|0,i+(r<<1<<2)|0),n=i+(r<<1<<2)|0,f=i+(r<<2)|0),oB(A,e,(0|r)/2|0,n),oB(A+(r<<2)|0,a,(0|r)/2|0,n),!(f=(0|hn(i,A,A+(r<<2)|0,r))-(0|Un(f,i,f,r))+(0|hn(A+(((0|r)/2|0)<<2)|0,A+(((0|r)/2|0)<<2)|0,f,r))|0))return;if(A=(0|t[(n=A+(((0|r)/2|0)+r<<2)|0)>>2])+f|0,t[n>>2]=A,A>>>0>=f>>>0)return;do{A=1+(0|t[(n=n+4|0)>>2])|0,t[n>>2]=A}while(!(0|A));return}}function uB(A,e){var r,i=0,n=0;if(!(e|=0))return 0|(e=-1);if((0|(i=0|t[(A|=0)+4>>2]))<=0)return 0|(e=0);r=0|t[A>>2],n=0,A=i;do{i=A,n=0|kf(0|t[r+((A=A+-1|0)<<2)>>2],0|n,0|e,0),E()}while((0|i)>1);return 0|(e=n)}function lB(A,e){A|=0;var r,i=0,n=0,f=0,a=0,B=0,u=0;if(!(e|=0))return 0|(A=-1);if(!(0|t[A+4>>2]))return 0|(A=0);if(!(0|rn(A,A,r=32-(0|zf(e))|0)))return 0|(A=-1);i=0|t[A+4>>2];do{if((0|i)>0){n=0|t[A>>2],f=0;do{a=i,B=0|Mn(f,u=0|t[n+((i=i+-1|0)<<2)>>2],e<<r),f=u-(0|o(B,e<<r))|0,n=0|t[A>>2],t[n+(i<<2)>>2]=B}while((0|a)>1);if((0|(i=0|t[A+4>>2]))>0){if(!(0|t[(0|t[A>>2])+(i+-1<<2)>>2])){t[A+4>>2]=i+-1,n=i+-1|0,i=f;break}return 0|(u=f>>>r)}n=i,i=f}else n=i,i=0}while(0);return i>>>=r,0|n||(t[A+12>>2]=0),0|(u=i)}function cB(A,e){var r=0,i=0,n=0,f=0;if(!(e|=0))return 0|(n=1);if(0|jf(A|=0))return 0|(n=0|ta(A,e));if(0|t[A+12>>2])return t[A+12>>2]=0,e=0|EB(A,e),0|jf(A)||(t[A+12>>2]=0==(0|t[A+12>>2])&1),0|(n=e);for(i=0;!((0|i)>=(0|(r=0|t[A+4>>2])));){if(f=(0|t[A>>2])+(i<<2)|0,r=(0|t[f>>2])+e|0,t[f>>2]=r,!(e>>>0>r>>>0)){e=1,n=13;break}i=i+1|0,e=1}return 13==(0|n)?0|e:(0|i)!=(0|r)?0|(f=1):0|ia(A,i+1|0)?(t[A+4>>2]=1+(0|t[A+4>>2]),t[(0|t[A>>2])+(i<<2)>>2]=e,0|(f=1)):0|(f=0)}function EB(A,e){A|=0;var r=0,i=0,n=0;if(!(e|=0))return 0|(A=1);if(0|jf(A))return(r=0|ta(A,e))?(function(A,e){A|=0,e=0!=(0|(e|=0))&&0!=(0|t[A+4>>2])?1:0,t[A+12>>2]=e}(A,1),0|(A=r)):0|(A=0);if(0|t[A+12>>2])return t[A+12>>2]=0,i=0|cB(A,e),t[A+12>>2]=1,0|(A=i);if(i=0|t[A>>2],(r=0|t[i>>2])>>>0<e>>>0&&1==(0|t[A+4>>2]))return t[i>>2]=e-r,t[A+12>>2]=1,0|(A=1);if(t[i>>2]=r-e,r>>>0<e>>>0)for(r=1;;){if(e=0|t[(n=i+(r<<2)|0)>>2],t[n>>2]=e+-1,e){i=r,r=e+-1|0;break}r=r+1|0}else i=0,r=r-e|0;return 0|r||(0|i)!=((0|t[A+4>>2])-1|0)||(t[A+4>>2]=i),0|(n=1)}function QB(A,e){e|=0;var r=0,i=0;return(r=0|t[(A|=0)+4>>2])?e?(r=0|Dn(i=0|t[A>>2],i,r,e))?0|ia(A,1+(0|t[A+4>>2])|0)?(e=0|t[A>>2],i=0|t[A+4>>2],t[A+4>>2]=i+1,t[e+(i<<2)>>2]=r,0|(i=1)):0|(i=0):0|(i=1):(ba(A),0|(A=1)):0|(A=1)}function wB(A,e,r,i,n){A|=0,e|=0,i|=0,n|=0;var f,a=0,B=0,u=0,l=0,c=0,Q=0,w=0,b=0,k=0,s=0,C=0,d=0,I=0,g=0,D=0,M=0,h=0,U=0;f=G,G=G+32|0,(0|(U=0|t[(r|=0)+4>>2]))>0&&!(0|t[(0|t[r>>2])+(U+-1<<2)>>2])||(w=3);do{if(3==(0|w)){if((0|(U=0|t[i+4>>2]))>0&&0==(0|t[(0|t[i>>2])+(U+-1<<2)>>2]))break;if(u=0==(0|Of(r,4))&&0==(0|Of(i,4))?0:1,0|jf(i))return Xe(3,107,103,0,0),G=f,0|(U=0);if(!(U=0!=(0|u))&&(0|la(r,i))<0)return 0|e&&0==(0|ra(e,r))?(G=f,0|(U=0)):A?(ba(A),G=f,0|(U=1)):(G=f,0|(U=1));iB(n),I=0|tB(n),M=0|tB(n),g=0|tB(n),D=A||0|tB(n);do{if(!(0==(0|M)|0==(0|I)|0==(0|g)|0==(0|D))&&0|rn(g,i,h=32-((0|Kf(i))%32|0)|0)&&(t[g+12>>2]=0,0|rn(M,r,h+32|0))){if(t[M+12>>2]=0,A=0|t[M+4>>2],U){if((0|A)<=((a=0|t[g+4>>2])+1|0)){if(!(0|ia(M,a+2|0)))break;if((0|(a=0|t[M+4>>2]))<(0|(A=2+(0|t[g+4>>2])|0))){B=0|t[M>>2];do{t[B+(a<<2)>>2]=0,a=a+1|0,A=2+(0|t[g+4>>2])|0}while((0|a)<(0|A))}}else{if(!(0|ia(M,A+1|0)))break;t[(0|t[M>>2])+(t[M+4>>2]<<2)>>2]=0,A=1+(0|t[M+4>>2])|0}t[M+4>>2]=A}if(d=A-(s=0|t[g+4>>2])|0,t[f+12>>2]=0,t[f>>2]=(0|t[M>>2])+(d<<2),t[f+4>>2]=s,t[f+16>>2]=2,t[f+8>>2]=(0|t[M+8>>2])-d,a=0|t[g>>2],C=0|t[a+(s+-1<<2)>>2],1==(0|s)?(b=0,k=0):(b=0|t[a+(s+-2<<2)>>2],k=0),a=(0|t[M>>2])+(A+-1<<2)|0,0|ia(D,d+1|0)&&(t[(Q=D+12|0)>>2]=t[i+12>>2]^t[r+12>>2],t[(l=D+4|0)>>2]=d-u,c=0|t[D>>2],0|ia(I,s+1|0))){do{if(!U){if((0|la(f,g))>-1){Un(w=0|t[f>>2],w,0|t[g>>2],s),t[c+(d+-1<<2)>>2]=1,w=39;break}A=(0|t[l>>2])-1|0,t[l>>2]=A;break}w=39}while(0);if(39==(0|w)&&(A=0|t[l>>2]),A?A=c+(d<<2)+-4|0:(t[Q>>2]=0,A=c+(d<<2)|0),(0|d)>1){w=0,Q=a;do{a=0|t[Q>>2],c=Q,B=0|t[(Q=Q+-4|0)>>2];A:do{if((0|a)==(0|C))a=-1;else{if(a=0|Mn(a,B,C),u=B-(0|o(a,C))|0,B=0|lf(0|b,0|k,0|a,0),!((i=0|E())>>>0>u>>>0|(0|i)==(0|u)&B>>>0>(l=0|t[c+-8>>2])>>>0))break;do{if(a=a+-1|0,u=u+C|0,B=0|Ef(0|B,0|i,0|b,0|k),i=0|E(),u>>>0<C>>>0)break A}while(i>>>0>u>>>0|(0|i)==(0|u)&B>>>0>l>>>0)}}while(0);l=0|Dn(0|t[I>>2],0|t[g>>2],s,a),t[(0|t[I>>2])+(s<<2)>>2]=l,l=(0|t[f>>2])-4|0,t[f>>2]=l;do{if(0|Un(l,l,0|t[I>>2],s+1|0)){if(a=a+-1|0,!(0|hn(l=0|t[f>>2],l,0|t[g>>2],s)))break;t[c>>2]=1+(0|t[c>>2])}}while(0);t[(A=A+-4|0)>>2]=a,w=w+1|0}while((0|w)<(d+-1|0))}aa(M);do{if(0|e){if(A=0|t[r+12>>2],nn(e,M,h+32|0),0|jf(e))break;t[e+12>>2]=A}}while(0);return U&&aa(D),nB(n),G=f,0|(U=1)}}}while(0);return nB(n),G=f,0|(U=0)}}while(0);return Xe(3,107,107,0,0),G=f,0|(U=0)}function bB(A,e,r){e|=0;var n=0,f=0,a=0;if(!(r|=0))return 1;f=0|t[(A|=0)+20>>2],n=0|t[A+24>>2],(f+(r<<3)|0)>>>0<f>>>0&&(t[A+24>>2]=n+1,n=n+1|0),t[A+24>>2]=n+(r>>>29),t[A+20>>2]=f+(r<<3),a=0|t[A+92>>2];do{if(a){if((a+r|r)>>>0>63){If(A+28+a|0,0|e,64-a|0),kB(A,A+28|0,1),f=(n=A+28|0)+68|0;do{i[n>>0]=0,n=n+1|0}while((0|n)<(0|f));n=e+(64-a)|0,r=r-(64-a)|0;break}return If(A+28+a|0,0|e,0|r),t[A+92>>2]=(0|t[A+92>>2])+r,1}n=e}while(0);return a=-64&r,(f=r>>>6)&&(kB(A,n,f),n=n+a|0,r=r-a|0),r?(t[A+92>>2]=r,If(A+28|0,0|n,0|r),1):1}function kB(A,e,r){e|=0,r|=0;var i=0,n=0,a=0,B=0,o=0,u=0,l=0,c=0,E=0,Q=0,w=0,b=0,k=0,s=0,C=0,d=0,I=0,g=0,D=0,M=0,h=0,U=0,G=0,Z=0,N=0,F=0;for(i=0|t[(A|=0)>>2],n=0|t[A+4>>2],a=0|t[A+8>>2],B=0|t[A+12>>2],o=0|t[A+16>>2];I=(0|f[e+1>>0])<<16|(0|f[e>>0])<<24|(0|f[e+2>>0])<<8|0|f[e+3>>0],k=(0|f[e+5>>0])<<16|(0|f[e+4>>0])<<24|(0|f[e+6>>0])<<8|0|f[e+7>>0],u=o+1518500249+((a^B)&n^B)+(i<<5|i>>>27)+I|0,E=n<<30|n>>>2,D=(0|f[e+9>>0])<<16|(0|f[e+8>>0])<<24|(0|f[e+10>>0])<<8|0|f[e+11>>0],l=B+1518500249+((E^a)&i^a)+k+(u<<5|u>>>27)|0,Q=i<<30|i>>>2,F=(0|f[e+13>>0])<<16|(0|f[e+12>>0])<<24|(0|f[e+14>>0])<<8|0|f[e+15>>0],c=a+1518500249+(u&(Q^E)^E)+D+(l<<5|l>>>27)|0,N=(0|f[e+17>>0])<<16|(0|f[e+16>>0])<<24|(0|f[e+18>>0])<<8|0|f[e+19>>0],E=E+1518500249+(l&((u<<30|u>>>2)^Q)^Q)+F+(c<<5|c>>>27)|0,U=(0|f[e+21>>0])<<16|(0|f[e+20>>0])<<24|(0|f[e+22>>0])<<8|0|f[e+23>>0],Q=Q+1518500249+(c&((l<<30|l>>>2)^(u<<30|u>>>2))^(u<<30|u>>>2))+N+(E<<5|E>>>27)|0,d=(0|f[e+25>>0])<<16|(0|f[e+24>>0])<<24|(0|f[e+26>>0])<<8|0|f[e+27>>0],u=1518500249+(u<<30|u>>>2)+(E&((c<<30|c>>>2)^(l<<30|l>>>2))^(l<<30|l>>>2))+U+(Q<<5|Q>>>27)|0,b=(0|f[e+29>>0])<<16|(0|f[e+28>>0])<<24|(0|f[e+30>>0])<<8|0|f[e+31>>0],l=1518500249+(l<<30|l>>>2)+(Q&((E<<30|E>>>2)^(c<<30|c>>>2))^(c<<30|c>>>2))+d+(u<<5|u>>>27)|0,C=(0|f[e+33>>0])<<16|(0|f[e+32>>0])<<24|(0|f[e+34>>0])<<8|0|f[e+35>>0],c=1518500249+(c<<30|c>>>2)+(u&((Q<<30|Q>>>2)^(E<<30|E>>>2))^(E<<30|E>>>2))+b+(l<<5|l>>>27)|0,Z=(0|f[e+37>>0])<<16|(0|f[e+36>>0])<<24|(0|f[e+38>>0])<<8|0|f[e+39>>0],E=1518500249+(E<<30|E>>>2)+(l&((u<<30|u>>>2)^(Q<<30|Q>>>2))^(Q<<30|Q>>>2))+C+(c<<5|c>>>27)|0,M=(0|f[e+41>>0])<<16|(0|f[e+40>>0])<<24|(0|f[e+42>>0])<<8|0|f[e+43>>0],Q=1518500249+(Q<<30|Q>>>2)+(c&((l<<30|l>>>2)^(u<<30|u>>>2))^(u<<30|u>>>2))+Z+(E<<5|E>>>27)|0,w=(0|f[e+45>>0])<<16|(0|f[e+44>>0])<<24|(0|f[e+46>>0])<<8|0|f[e+47>>0],u=1518500249+(u<<30|u>>>2)+(E&((c<<30|c>>>2)^(l<<30|l>>>2))^(l<<30|l>>>2))+M+(Q<<5|Q>>>27)|0,h=(0|f[e+49>>0])<<16|(0|f[e+48>>0])<<24|(0|f[e+50>>0])<<8|0|f[e+51>>0],l=1518500249+(l<<30|l>>>2)+(Q&((E<<30|E>>>2)^(c<<30|c>>>2))^(c<<30|c>>>2))+w+(u<<5|u>>>27)|0,G=(0|f[e+53>>0])<<16|(0|f[e+52>>0])<<24|(0|f[e+54>>0])<<8|0|f[e+55>>0],c=1518500249+(c<<30|c>>>2)+(u&((Q<<30|Q>>>2)^(E<<30|E>>>2))^(E<<30|E>>>2))+h+(l<<5|l>>>27)|0,g=(0|f[e+57>>0])<<16|(0|f[e+56>>0])<<24|(0|f[e+58>>0])<<8|0|f[e+59>>0],E=1518500249+(E<<30|E>>>2)+(l&((u<<30|u>>>2)^(Q<<30|Q>>>2))^(Q<<30|Q>>>2))+G+(c<<5|c>>>27)|0,I=(D^I^C^G)<<1|(D^I^C^G)>>>31,D=(N^D^M^(s=(0|f[e+61>>0])<<16|(0|f[e+60>>0])<<24|(0|f[e+62>>0])<<8|0|f[e+63>>0]))<<1|(N^D^M^s)>>>31,N=(d^N^h^(k=(F^k^Z^g)<<1|(F^k^Z^g)>>>31))<<1|(d^N^h^k)>>>31,c=(d=(C^d^g^(F=(U^F^w^I)<<1|(U^F^w^I)>>>31))<<1|(C^d^g^F)>>>31)+1859775393+((c=1518500249+(c<<30|c>>>2)+k+((u=1518500249+(u<<30|u>>>2)+(E&((c<<30|c>>>2)^(l<<30|l>>>2))^(l<<30|l>>>2))+s+((Q=1518500249+(Q<<30|Q>>>2)+(c&((l<<30|l>>>2)^(u<<30|u>>>2))^(u<<30|u>>>2))+g+(E<<5|E>>>27)|0)<<5|Q>>>27)|0)&((Q<<30|Q>>>2)^(E<<30|E>>>2))^(E<<30|E>>>2))+((l=1518500249+(l<<30|l>>>2)+I+(Q&((E<<30|E>>>2)^(c<<30|c>>>2))^(c<<30|c>>>2))+(u<<5|u>>>27)|0)<<5|l>>>27)|0)<<30|c>>>2)+(((Q=F+1518500249+(Q<<30|Q>>>2)+(c&((l<<30|l>>>2)^(u<<30|u>>>2))^(u<<30|u>>>2))+((E=1518500249+(E<<30|E>>>2)+D+(l&((u<<30|u>>>2)^(Q<<30|Q>>>2))^(Q<<30|Q>>>2))+(c<<5|c>>>27)|0)<<5|E>>>27)|0)<<30|Q>>>2)^(E<<30|E>>>2)^(u=N+1859775393+(u<<30|u>>>2)+((c<<30|c>>>2)^(l<<30|l>>>2)^E)+(Q<<5|Q>>>27)|0))+((l=(U=(b^U^G^D)<<1|(b^U^G^D)>>>31)+1859775393+(l<<30|l>>>2)+((E<<30|E>>>2)^(c<<30|c>>>2)^Q)+(u<<5|u>>>27)|0)<<5|l>>>27)|0,C=(M^C^I^U)<<1|(M^C^I^U)>>>31,M=(h^M^D^(b=(Z^b^s^N)<<1|(Z^b^s^N)>>>31))<<1|(h^M^D^b)>>>31,h=(g^h^N^(Z=(w^Z^k^d)<<1|(w^Z^k^d)>>>31))<<1|(g^h^N^Z)>>>31,g=(g^I^d^(w=(G^w^F^C)<<1|(G^w^F^C)>>>31))<<1|(g^I^d^w)>>>31,I=(D^I^C^(G=(s^G^U^M)<<1|(s^G^U^M)>>>31))<<1|(D^I^C^G)>>>31,D=(D^N^M^(s=(s^k^b^h)<<1|(s^k^b^h)>>>31))<<1|(D^N^M^s)>>>31,N=(N^d^h^(k=(k^F^Z^g)<<1|(k^F^Z^g)>>>31))<<1|(N^d^h^k)>>>31,E=(d=(C^d^g^(F=(U^F^w^I)<<1|(U^F^w^I)>>>31))<<1|(C^d^g^F)>>>31)+1859775393+((E=k+1859775393+((E=h+1859775393+((E=b+1859775393+(E<<30|E>>>2)+((u<<30|u>>>2)^(Q<<30|Q>>>2)^l)+(c<<5|c>>>27)|0)<<30|E>>>2)+(((u=Z+1859775393+(u<<30|u>>>2)+((c<<30|c>>>2)^(l<<30|l>>>2)^E)+((Q=C+1859775393+(Q<<30|Q>>>2)+((l<<30|l>>>2)^(u<<30|u>>>2)^c)+(E<<5|E>>>27)|0)<<5|Q>>>27)|0)<<30|u>>>2)^(Q<<30|Q>>>2)^(l=M+1859775393+(l<<30|l>>>2)+((E<<30|E>>>2)^(c<<30|c>>>2)^Q)+(u<<5|u>>>27)|0))+((c=w+1859775393+(c<<30|c>>>2)+((Q<<30|Q>>>2)^(E<<30|E>>>2)^u)+(l<<5|l>>>27)|0)<<5|c>>>27)|0)<<30|E>>>2)+(((u=g+1859775393+(u<<30|u>>>2)+((c<<30|c>>>2)^(l<<30|l>>>2)^E)+((Q=G+1859775393+(Q<<30|Q>>>2)+((l<<30|l>>>2)^(u<<30|u>>>2)^c)+(E<<5|E>>>27)|0)<<5|Q>>>27)|0)<<30|u>>>2)^(Q<<30|Q>>>2)^(l=s+1859775393+(l<<30|l>>>2)+((E<<30|E>>>2)^(c<<30|c>>>2)^Q)+(u<<5|u>>>27)|0))+((c=I+1859775393+(c<<30|c>>>2)+((Q<<30|Q>>>2)^(E<<30|E>>>2)^u)+(l<<5|l>>>27)|0)<<5|c>>>27)|0)<<30|E>>>2)+(((u=F+1859775393+(u<<30|u>>>2)+((c<<30|c>>>2)^(l<<30|l>>>2)^E)+((Q=D+1859775393+(Q<<30|Q>>>2)+((l<<30|l>>>2)^(u<<30|u>>>2)^c)+(E<<5|E>>>27)|0)<<5|Q>>>27)|0)<<30|u>>>2)^(Q<<30|Q>>>2)^(l=N+1859775393+(l<<30|l>>>2)+((E<<30|E>>>2)^(c<<30|c>>>2)^Q)+(u<<5|u>>>27)|0))+((c=(U=(U^b^G^D)<<1|(U^b^G^D)>>>31)+1859775393+(c<<30|c>>>2)+((Q<<30|Q>>>2)^(E<<30|E>>>2)^u)+(l<<5|l>>>27)|0)<<5|c>>>27)|0,C=(C^M^I^U)<<1|(C^M^I^U)>>>31,M=(M^h^D^(b=(b^Z^s^N)<<1|(b^Z^s^N)>>>31))<<1|(M^h^D^b)>>>31,h=(g^h^N^(Z=(w^Z^k^d)<<1|(w^Z^k^d)>>>31))<<1|(g^h^N^Z)>>>31,g=(g^I^d^(w=(w^G^F^C)<<1|(w^G^F^C)>>>31))<<1|(g^I^d^w)>>>31,I=(I^D^C^(G=(G^s^U^M)<<1|(G^s^U^M)>>>31))<<1|(I^D^C^G)>>>31,D=(N^D^M^(s=(k^s^b^h)<<1|(k^s^b^h)>>>31))<<1|(N^D^M^s)>>>31,N=(N^d^h^(k=(k^F^Z^g)<<1|(k^F^Z^g)>>>31))<<1|(N^d^h^k)>>>31,Q=(d=(d^C^g^(F=(F^U^w^I)<<1|(F^U^w^I)>>>31))<<1|(d^C^g^F)>>>31)+-1894007588+((Q=k+-1894007588+((Q=h+-1894007588+((Q=b+1859775393+(Q<<30|Q>>>2)+((l<<30|l>>>2)^(u<<30|u>>>2)^c)+(E<<5|E>>>27)|0)<<30|Q>>>2)+(((c=M+-1894007588+(c<<30|c>>>2)+(((u=C+-1894007588+(u<<30|u>>>2)+((E|c<<30|c>>>2)&(l<<30|l>>>2)|E&(c<<30|c>>>2))+(Q<<5|Q>>>27)|0)|Q<<30|Q>>>2)&(E<<30|E>>>2)|u&(Q<<30|Q>>>2))+((l=Z+-1894007588+(l<<30|l>>>2)+((Q|E<<30|E>>>2)&(c<<30|c>>>2)|Q&(E<<30|E>>>2))+(u<<5|u>>>27)|0)<<5|l>>>27)|0)|l<<30|l>>>2)&(u<<30|u>>>2)|c&(l<<30|l>>>2))+((E=w+-1894007588+(E<<30|E>>>2)+((l|u<<30|u>>>2)&(Q<<30|Q>>>2)|l&(u<<30|u>>>2))+(c<<5|c>>>27)|0)<<5|E>>>27)|0)<<30|Q>>>2)+(((c=s+-1894007588+(c<<30|c>>>2)+(((u=G+-1894007588+(u<<30|u>>>2)+((E|c<<30|c>>>2)&(l<<30|l>>>2)|E&(c<<30|c>>>2))+(Q<<5|Q>>>27)|0)|Q<<30|Q>>>2)&(E<<30|E>>>2)|u&(Q<<30|Q>>>2))+((l=g+-1894007588+(l<<30|l>>>2)+((Q|E<<30|E>>>2)&(c<<30|c>>>2)|Q&(E<<30|E>>>2))+(u<<5|u>>>27)|0)<<5|l>>>27)|0)|l<<30|l>>>2)&(u<<30|u>>>2)|c&(l<<30|l>>>2))+((E=I+-1894007588+(E<<30|E>>>2)+((l|u<<30|u>>>2)&(Q<<30|Q>>>2)|l&(u<<30|u>>>2))+(c<<5|c>>>27)|0)<<5|E>>>27)|0)<<30|Q>>>2)+(((c=N+-1894007588+(c<<30|c>>>2)+(((u=D+-1894007588+(u<<30|u>>>2)+((E|c<<30|c>>>2)&(l<<30|l>>>2)|E&(c<<30|c>>>2))+(Q<<5|Q>>>27)|0)|Q<<30|Q>>>2)&(E<<30|E>>>2)|u&(Q<<30|Q>>>2))+((l=F+-1894007588+(l<<30|l>>>2)+((Q|E<<30|E>>>2)&(c<<30|c>>>2)|Q&(E<<30|E>>>2))+(u<<5|u>>>27)|0)<<5|l>>>27)|0)|l<<30|l>>>2)&(u<<30|u>>>2)|c&(l<<30|l>>>2))+((E=(U=(b^U^G^D)<<1|(b^U^G^D)>>>31)+-1894007588+(E<<30|E>>>2)+((l|u<<30|u>>>2)&(Q<<30|Q>>>2)|l&(u<<30|u>>>2))+(c<<5|c>>>27)|0)<<5|E>>>27)|0,C=(M^C^I^U)<<1|(M^C^I^U)>>>31,M=(M^h^D^(b=(b^Z^s^N)<<1|(b^Z^s^N)>>>31))<<1|(M^h^D^b)>>>31,h=(h^g^N^(Z=(Z^w^k^d)<<1|(Z^w^k^d)>>>31))<<1|(h^g^N^Z)>>>31,g=(I^g^d^(w=(G^w^F^C)<<1|(G^w^F^C)>>>31))<<1|(I^g^d^w)>>>31,I=(I^D^C^(G=(G^s^U^M)<<1|(G^s^U^M)>>>31))<<1|(I^D^C^G)>>>31,D=(D^N^M^(s=(s^k^b^h)<<1|(s^k^b^h)>>>31))<<1|(D^N^M^s)>>>31,N=(d^N^h^(k=(F^k^Z^g)<<1|(F^k^Z^g)>>>31))<<1|(d^N^h^k)>>>31,E=((Z^w^k^(d=(d^C^g^(F=(F^U^w^I)<<1|(F^U^w^I)>>>31))<<1|(d^C^g^F)>>>31))<<1|(Z^w^k^d)>>>31)-899497514+((E=N+-899497514+((E=s+-899497514+((E=M+-1894007588+(E<<30|E>>>2)+(((l=C+-1894007588+(l<<30|l>>>2)+((Q|E<<30|E>>>2)&(c<<30|c>>>2)|Q&(E<<30|E>>>2))+((u=b+-1894007588+(u<<30|u>>>2)+((E|c<<30|c>>>2)&(l<<30|l>>>2)|E&(c<<30|c>>>2))+(Q<<5|Q>>>27)|0)<<5|u>>>27)|0)|u<<30|u>>>2)&(Q<<30|Q>>>2)|l&(u<<30|u>>>2))+((c=Z+-1894007588+(c<<30|c>>>2)+((u|Q<<30|Q>>>2)&(E<<30|E>>>2)|u&(Q<<30|Q>>>2))+(l<<5|l>>>27)|0)<<5|c>>>27)|0)<<30|E>>>2)+(((u=h+-899497514+(u<<30|u>>>2)+((c<<30|c>>>2)^(l<<30|l>>>2)^E)+((Q=w+-1894007588+(Q<<30|Q>>>2)+((c|l<<30|l>>>2)&(u<<30|u>>>2)|c&(l<<30|l>>>2))+(E<<5|E>>>27)|0)<<5|Q>>>27)|0)<<30|u>>>2)^(Q<<30|Q>>>2)^(l=G+-899497514+(l<<30|l>>>2)+((E<<30|E>>>2)^(c<<30|c>>>2)^Q)+(u<<5|u>>>27)|0))+((c=g+-899497514+(c<<30|c>>>2)+((Q<<30|Q>>>2)^(E<<30|E>>>2)^u)+(l<<5|l>>>27)|0)<<5|c>>>27)|0)<<30|E>>>2)+(((u=k+-899497514+(u<<30|u>>>2)+((c<<30|c>>>2)^(l<<30|l>>>2)^E)+((Q=I+-899497514+(Q<<30|Q>>>2)+((l<<30|l>>>2)^(u<<30|u>>>2)^c)+(E<<5|E>>>27)|0)<<5|Q>>>27)|0)<<30|u>>>2)^(Q<<30|Q>>>2)^(l=D+-899497514+(l<<30|l>>>2)+((E<<30|E>>>2)^(c<<30|c>>>2)^Q)+(u<<5|u>>>27)|0))+((c=F+-899497514+(c<<30|c>>>2)+((Q<<30|Q>>>2)^(E<<30|E>>>2)^u)+(l<<5|l>>>27)|0)<<5|c>>>27)|0)<<30|E>>>2)+(((u=d+-899497514+(u<<30|u>>>2)+((c<<30|c>>>2)^(l<<30|l>>>2)^E)+((Q=(U=(U^b^G^D)<<1|(U^b^G^D)>>>31)+-899497514+(Q<<30|Q>>>2)+((l<<30|l>>>2)^(u<<30|u>>>2)^c)+(E<<5|E>>>27)|0)<<5|Q>>>27)|0)<<30|u>>>2)^(Q<<30|Q>>>2)^(l=(b=(Z^b^s^N)<<1|(Z^b^s^N)>>>31)+-899497514+(l<<30|l>>>2)+((E<<30|E>>>2)^(c<<30|c>>>2)^Q)+(u<<5|u>>>27)|0))+((c=((C^M^I^U)<<1|(C^M^I^U)>>>31)-899497514+(c<<30|c>>>2)+((Q<<30|Q>>>2)^(E<<30|E>>>2)^u)+(l<<5|l>>>27)|0)<<5|c>>>27)|0,C=w^G^F^((C^M^I^U)<<1|(C^M^I^U)>>>31),i=i+-899497514+((s^k^b^((w=h^g^N^((Z^w^k^d)<<1|(Z^w^k^d)>>>31))<<1|w>>>31))<<1|(s^k^b^(w<<1|w>>>31))>>>31)+((Q=((h^M^D^b)<<1|(h^M^D^b)>>>31)-899497514+(Q<<30|Q>>>2)+((l<<30|l>>>2)^(u<<30|u>>>2)^c)+(E<<5|E>>>27)|0)<<30|Q>>>2)+(((l=(w<<1|w>>>31)-899497514+(l<<30|l>>>2)+((E<<30|E>>>2)^(c<<30|c>>>2)^Q)+((u=(C<<1|C>>>31)-899497514+(u<<30|u>>>2)+((c<<30|c>>>2)^(l<<30|l>>>2)^E)+(Q<<5|Q>>>27)|0)<<5|u>>>27)|0)<<30|l>>>2)^(u<<30|u>>>2)^(c=((D=s^G^U^((h^M^D^b)<<1|(h^M^D^b)>>>31))<<1|D>>>31)-899497514+(c<<30|c>>>2)+((Q<<30|Q>>>2)^(E<<30|E>>>2)^u)+(l<<5|l>>>27)|0))+((E=((g^I^d^(C<<1|C>>>31))<<1|(g^I^d^(C<<1|C>>>31))>>>31)-899497514+(E<<30|E>>>2)+((u<<30|u>>>2)^(Q<<30|Q>>>2)^l)+(c<<5|c>>>27)|0)<<5|E>>>27)|0,t[A>>2]=i,n=E+n|0,t[A+4>>2]=n,a=(c<<30|c>>>2)+a|0,t[A+8>>2]=a,B=(l<<30|l>>>2)+B|0,t[A+12>>2]=B,o=(u<<30|u>>>2)+(0|t[A+16>>2])|0,t[A+16>>2]=o,r=r+-1|0;)e=e+64|0}function sB(A,e){A|=0;var r=0;return r=0|t[(e|=0)+92>>2],i[e+28+r>>0]=-128,(r+1|0)>>>0>56?(Df(e+28+(r+1)|0,0,63-r|0),kB(e,e+28|0,1),r=0):r=r+1|0,Df(e+28+r|0,0,56-r|0),r=0|t[e+24>>2],i[e+84>>0]=r>>>24,i[e+84+1>>0]=r>>>16,i[e+84+2>>0]=r>>>8,i[e+84+3>>0]=r,r=0|t[e+20>>2],i[e+88>>0]=r>>>24,i[e+88+1>>0]=r>>>16,i[e+88+2>>0]=r>>>8,i[e+88+3>>0]=r,kB(e,e+28|0,1),t[e+92>>2]=0,$r(e+28|0,64),r=0|t[e>>2],i[A>>0]=r>>>24,i[A+1>>0]=r>>>16,i[A+2>>0]=r>>>8,i[A+3>>0]=r,r=0|t[e+4>>2],i[A+4>>0]=r>>>24,i[A+5>>0]=r>>>16,i[A+6>>0]=r>>>8,i[A+7>>0]=r,r=0|t[e+8>>2],i[A+8>>0]=r>>>24,i[A+9>>0]=r>>>16,i[A+10>>0]=r>>>8,i[A+11>>0]=r,r=0|t[e+12>>2],i[A+12>>0]=r>>>24,i[A+13>>0]=r>>>16,i[A+14>>0]=r>>>8,i[A+15>>0]=r,e=0|t[e+16>>2],i[A+16>>0]=e>>>24,i[A+17>>0]=e>>>16,i[A+18>>0]=e>>>8,i[A+19>>0]=e,1}function CB(A){var e,r=0;e=(r=(A|=0)+20|0)+76|0;do{t[r>>2]=0,r=r+4|0}while((0|r)<(0|e));return t[A>>2]=1732584193,t[A+4>>2]=-271733879,t[A+8>>2]=-1732584194,t[A+12>>2]=271733878,t[A+16>>2]=-1009589776,1}function dB(A){var e,r=0;e=(r=(A|=0)+32|0)+76|0;do{t[r>>2]=0,r=r+4|0}while((0|r)<(0|e));return t[A>>2]=1779033703,t[A+4>>2]=-1150833019,t[A+8>>2]=1013904242,t[A+12>>2]=-1521486534,t[A+16>>2]=1359893119,t[A+20>>2]=-1694144372,t[A+24>>2]=528734635,t[A+28>>2]=1541459225,t[A+108>>2]=32,1}var IB=[Mf,function(A){var e;return A|=0,(e=0|Sn(1048,82796,0))?(t[A+20>>2]=e,0|(A=1)):0|(A=0)},function(A){var e;return A|=0,(e=0|Sn(1040,82796,0))?(t[A+20>>2]=e,0|(A=1)):0|(A=0)},function(A){A|=0;var e,r=0;return(e=0|Sn(24,82796,0))?(t[e+8>>2]=4,r=0|oe(),t[e+20>>2]=r,r?(t[A+20>>2]=e,t[A+36>>2]=0,0|(r=1)):(Xn(e,82796,0),0|(r=0))):0|(A=0)},function(A){return 0,64},function(A){return t[(A|=0)+56>>2]=6|t[A+56>>2],1},function(A){return La(0|t[(A|=0)+60>>2]),La(0|t[A+64>>2]),La(0|t[A+68>>2]),1},function(A){return 0|dr(0|t[(A|=0)+24>>2])},function(A){return 0|Kf(0|t[16+(0|t[(A|=0)+24>>2])>>2])},function(A){return 0|function(A){return 0|function(A,e){if(e|=0,(0|(A|=0))<=15359)if((0|A)<=7679)if((0|A)<=3071)if((0|A)<=2047){if(!((0|A)>1023))return 0|(e=0);A=80}else A=112;else A=128;else A=192;else A=256;return-1==(0|e)?0|(e=A):0|((0|e)<160?0:(0|(0|e)/2)<(0|A)?(0|e)/2|0:A)}(0|Kf(0|t[(A|=0)+16>>2]),-1)}(0|t[(A|=0)+24>>2])},function(A){var e;return A|=0,(e=0|Sn(44,82796,0))?(t[e>>2]=2048,t[e+16>>2]=1,t[e+28>>2]=-2,t[A+20>>2]=e,t[A+32>>2]=e+8,t[A+36>>2]=2,0|(A=1)):0|(A=0)},function(A){return 0|function(A){var e,r=0;e=(r=(A|=0)+16|0)+76|0;do{t[r>>2]=0,r=r+4|0}while((0|r)<(0|e));return t[A>>2]=1732584193,t[A+4>>2]=-271733879,t[A+8>>2]=-1732584194,t[A+12>>2]=271733878,1}(0|Ki(A|=0))},function(A){return 0|CB(0|Ki(A|=0))},function(A){var e;return A|=0,(e=0|Ln(12))?0|IB[31&t[A+8>>2]](e)?0|(A=e):(Xn(e),0|(A=0)):0|(A=e)},function(A){return(A|=0)?(t[A>>2]=52996,t[A+4>>2]=50832,t[A+8>>2]=0,0|(A=1)):0|(A=0)},function(A){return(A|=0)?($n(A),Xn(A),A=1):A=0,0|A},function(A){return(A|=0)?($n(A),A=1):A=0,0|A},Ne,function(A){var e=0,r=0;return e=A|=0,r=0|He(0|t[A+8>>2]),0|(r^=A=0|t[(A=e)>>2])},function(A){var e=0,r=0,i=0,n=0;switch(e=0|t[(A|=0)+4>>2],0|t[A>>2]){case 0:if(n=0|t[e+12>>2],r=0|t[e+16>>2],(0|n)>0){i=0,e=n<<20;do{e=(0|f[r+i>>0])<<(((3*i|0)>>>0)%24|0)^e,i=i+1|0}while((0|i)!=(0|n))}else e=n<<20;break;case 1:e=0|Ne(0|t[e>>2]);break;case 2:e=0|Ne(0|t[e+4>>2]);break;case 3:e=0|t[e+8>>2];break;default:return 0|(A=0)}return 0|(A=t[A>>2]<<30|1073741823&e)},function(A){var e;return e=(0|Ne(0|t[(A|=0)>>2]))<<2,(0|Ne(0|t[A+4>>2]))^e|0},Mf,Mf,Mf,Mf,Mf,Mf,Mf,Mf,Mf,Mf,Mf],gB=[hf,function(A,e){return A|=0,e|=0,e=0|qf(),t[A>>2]=e,0!=(0|e)|0},function(A,e){return A|=0,e|=0,e=0|$f(),t[A>>2]=e,0!=(0|e)|0},function(A,e){return e|=0,t[(A|=0)>>2]=t[e+20>>2],1},function(A,e){A|=0,e|=0;var r=0,i=0;if(!(r=0|Sn(24,82796,0)))return 0|(A=0);if(t[r+8>>2]=4,i=0|oe(),t[r+20>>2]=i,!i)return Xn(r,82796,0),0|(A=0);if(t[A+20>>2]=r,t[A+36>>2]=0,e=0|si(e),i=0|si(A),t[i>>2]=t[e>>2],0|function(A,e){e|=0;var r=0,i=0,n=0;if(0|ue(A|=0)&&0|hi(0|t[A+8>>2],0|t[e+8>>2])&&0|hi(0|t[A+12>>2],0|t[e+12>>2])&&0|hi(0|t[A+4>>2],0|t[e+4>>2])){i=e+20|0,n=(r=A+20|0)+128|0;do{t[r>>2]=t[i>>2],r=r+4|0,i=i+4|0}while((0|r)<(0|n));return t[A+16>>2]=t[e+16>>2],t[A>>2]=t[e>>2],0|(n=1)}return Ci(0|t[A+8>>2]),Ci(0|t[A+12>>2]),Ci(0|t[A+4>>2]),t[A>>2]=0,t[A+16>>2]=0,$r(A+20|0,128),0|(n=0)}(0|t[i+20>>2],0|t[e+20>>2])){if(!(r=0|t[e+12>>2]))return 0|(A=1);if(0|MA(i+4|0,r,0|t[e+4>>2]))return 0|(A=1)}return(r=0|si(A))?(le(0|t[r+20>>2]),pn(0|t[r+12>>2],0|t[r+4>>2],82796,0),Xn(r,82796,0),ki(A,0),0|(A=0)):0|(A=0)},function(A,e){return e|=0,A=0|t[(A|=0)+20>>2],0|t[A+12>>2]&&(A=0|function(A){return 0|lA(A|=0)}(A+4|0))?(Yi(e,855,A),0|(e=1)):0|(e=0)},function(A,e){return e|=0,function(A,e){e|=0,ji(0|t[(A|=0)+8>>2],e),ji(0|t[A+12>>2],e),ji(0|t[A+4>>2],e)}(A=0|t[20+(0|t[(A|=0)+20>>2])>>2],0|xi(e,-257)),ji(e,256),function(A,e){e|=0,t[(A|=0)+20>>2]=e}(e,15),1},function(A,e){return e|=0,0|DA(A=0|Ri(A|=0),0|Ri(e))},function(A,e){e|=0;var r=0;A=0|Ri(A|=0);do{if(e){if(0|(r=0|t[e>>2])){If(0|r,0|t[A+8>>2],0|t[A>>2]),t[e>>2]=(0|t[e>>2])+(0|t[A>>2]);break}if(r=0|Ln(0|t[A>>2],82796,0),t[e>>2]=r,r){If(0|r,0|t[A+8>>2],0|t[A>>2]);break}return 0|(e=-1)}}while(0);return 0|(e=0|t[A>>2])},function(A,e){var r;A|=0,e|=0,r=G,G=G+16|0;do{if(0|Bi(0,r+4|0,r,0,e)){if(e=0|br(0,r+4|0,0|t[r>>2])){Yi(A,6,e),e=1;break}Xe(4,139,4,0,0),e=0;break}e=0}while(0);return G=r,0|e},function(A,e){var r;return A|=0,e|=0,r=G,G=G+16|0,t[r>>2]=0,(0|(e=0|function(A,e){return 0|TA(A|=0,e|=0,52548)}(0|t[e+24>>2],r)))<1?(G=r,0|(A=0)):0|function(A,e,r,i,n,f){if(e|=0,r|=0,i|=0,n|=0,f|=0,!(0|yA(0|t[(A|=0)>>2],e,r,i)))return 0|(i=0);if(!n)return 0|(i=1);return Xn(0|t[8+(0|t[A+4>>2])>>2],82796,0),i=0|t[A+4>>2],t[i+8>>2]=n,t[i>>2]=f,t[i+12>>2]=-16&t[i+12>>2]|8,0|(i=1)}(A,0|Sr(6),5,0,0|t[r>>2],e)?(G=r,0|1):(Xn(0|t[r>>2],82796,0),G=r,0|0)},function(A,e){return A|=0,0|ca(0|t[16+(0|t[(e|=0)+24>>2])>>2],0|t[16+(0|t[A+24>>2])>>2])?0:0==(0|ca(0|t[20+(0|t[e+24>>2])>>2],0|t[20+(0|t[A+24>>2])>>2]))|0},function(A,e){var r;A|=0,e|=0,r=G,G=G+16|0;do{if(0|PA(0,r+4|0,r,0,e)){if(e=0|Qr(0,r+4|0,0|t[r>>2])){Yi(A,6,e),e=1;break}Xe(4,147,4,0,0),e=0;break}e=0}while(0);return G=r,0|e},function(A,e){var r;return A|=0,e|=0,r=G,G=G+16|0,t[r>>2]=0,(0|(e=0|wr(0|t[e+24>>2],r)))<1?(Xe(4,138,65,0,0),G=r,0|(A=0)):0|OA(A,0|Sr(6),0,5,0,0|t[r>>2],e)?(G=r,0|1):(Xe(4,138,65,0,0),G=r,0|0)},function(A,e){return e|=0,0|wr(0|t[(A|=0)+24>>2],e)},function(A,e){A|=0,e|=0;var r=0;return(r=0|Sn(44,82796,0))?(t[r>>2]=2048,t[r+16>>2]=1,t[r+28>>2]=-2,t[A+20>>2]=r,t[A+32>>2]=r+8,t[A+36>>2]=2,A=0|t[e+20>>2],t[r>>2]=t[A>>2],0|(e=0|t[A+4>>2])&&(e=0|ea(e),t[r+4>>2]=e,0==(0|e))?0|(r=0):(t[r+16>>2]=t[A+16>>2],t[r+20>>2]=t[A+20>>2],t[r+24>>2]=t[A+24>>2],0|t[A+36>>2]?(Xn(0|t[r+36>>2],82796,0),e=0|Ct(0|t[A+36>>2],0|t[A+40>>2],82796,0),t[r+36>>2]=e,e?(t[r+40>>2]=t[A+40>>2],0|(r=1)):0|(r=0)):0|(r=1))):0|(r=0)},function(A,e){e|=0;var r,i,n=0;if(r=0|t[(A|=0)+20>>2],!(0|t[r+4>>2])){if(n=0|qf(),t[r+4>>2]=n,!n)return 0|(e=0);if(!(0|ta(n,65537)))return 0|(e=0)}if(!(i=0|kr()))return 0|(e=0);do{if(0|t[A+28>>2]){if(0|(n=0|Ma())){qi(n,A),A=n;break}return sr(i),0|(e=0)}A=0}while(0);return n=0|er(i,0|t[r>>2],0|t[r+4>>2],A),function(A){if(!(A|=0))return;Xn(A,82796,0)}(A),(0|n)>0?(Yi(e,6,i),0|(e=n)):(sr(i),0|(e=n))},function(A,e){return 0|Qe(e|=0,0|Ki(A|=0))},function(A,e){return 0|sB(e|=0,0|Ki(A|=0))},function(A,e){return e|=0,me(0|t[(A|=0)+8>>2],4,e),1},function(A,e){return e|=0,1&n[(0|t[(A|=0)+4>>2])+((255&e)<<1)>>1]|0},function(A,e){return 0,((e|=0)<<24>>24)-48|0},function(A,e){var r,n,f=0,a=0,B=0,o=0,u=0,l=0,c=0,E=0,Q=0;if(n=G,G=G+48|0,!(r=0|et(e|=0,0|lt(A|=0))))return Xe(13,223,172,0,0),G=n,0|(Q=0);if((0|Hn(r))<=0)return G=n,0|(Q=1);for(Q=0;;){E=0|vn(r,Q),c=0|t[E+8>>2],E=0|t[E+4>>2],t[n+32>>2]=0,l=(e=0|jr(E))||(e=0|xr(E))?7:21;A:do{if(7==(0|l))if(l=0,A=0|ti(c)){do{if((0|Hn(A))>0){l=0,B=0,a=-1,f=-1;e:for(;;){u=0|vn(A,l),o=0|t[u+4>>2];do{if(0|Wt(o,53937)){if(!(0|Wt(o,53941))){if(a=0|St(0|t[u+8>>2],n+36|0,0),0|i[t[n+36>>2]>>0]){l=20;break e}break}if(!(0|Wt(o,53945))){if((o=0!=(0|dA(0|t[u+8>>2],n+32|0)))&0!=(0|t[n+32>>2]))break;l=20;break e}if(0|Wt(o,53950)){l=22;break e}if(0|Wt(B=0|t[u+8>>2],53956)){if(0|Wt(B,55355)){l=20;break e}B=1}else B=2}else if(f=0|St(0|t[u+8>>2],n+36|0,0),0|i[t[n+36>>2]>>0]){l=20;break e}}while(0);if((0|(l=l+1|0))>=(0|Hn(A))){l=25;break}}if(20==(0|l))l=0,Xe(13,222,218,0,0);else if(22==(0|l)){if(l=0,Xe(13,222,218,0,0),!u){l=24;break A}}else if(25==(0|l)){l=0,o=0|t[n+32>>2];break}E=0|t[u+4>>2],e=0|t[u+8>>2],t[n>>2]=53963,t[n+4>>2]=E,t[n+8>>2]=80764,t[n+12>>2]=e,Ke(4,n),e=0;break A}o=0,B=0,a=-1,f=-1}while(0);(e=0|QA(e,f,a,o,B))||(Xe(13,222,65,0,0),e=0)}else l=21}while(0);if(21==(0|l)&&(Xe(13,222,218,0,0),A=0,l=24),24==(0|l)&&(l=0,t[n+16>>2]=80644,t[n+16+4>>2]=E,t[n+16+8>>2]=80764,t[n+16+12>>2]=c,Ke(4,n+16|0),e=0),mn(A,8),!e)break;if((0|(Q=Q+1|0))>=(0|Hn(r))){e=1,l=31;break}}return 31==(0|l)?(G=n,0|e):(Xe(13,223,219,0,0),G=n,0|(Q=0))},function(A,e){return e|=0,(0|t[t[(A|=0)>>2]>>2])-(0|t[t[e>>2]>>2])|0},function(A,e){return e|=0,(0|t[(A|=0)>>2])-(0|t[e>>2])|0},function(A,e){var r,n=0,a=0,B=0,o=0,u=0;if(!(r=0|et(e|=0,0|lt(A|=0))))return Xe(13,174,172,0,0),0|(u=0);if((0|Hn(r))<=0)return 0|(u=1);o=0;A:for(;;){if(a=0|vn(r,o),B=0|t[a+8>>2],a=0|t[a+4>>2],0|(e=0|ef(B,44))){if(!((A=0|i[e+1>>0])<<24>>24))break;if(0|Yt(255&A))for(A=e+2|0;;){if(!(0|Yt(0|f[A>>0]))){n=0;break}A=A+1|0}else n=0,A=e+1|0}else n=a,A=B;if(0==(0|(n=0|qr(A,a,n)))|0!=(0|e)^1){if(!n)break}else{for(;0|Yt(0|f[B>>0]);)B=B+1|0;if(0|Yt(0|f[e+-1>>0]))for(e=e+-1|0;;){if((0|e)==(0|B))break A;if(!(0|Yt(0|f[(A=e+-1|0)>>0])))break;e=A}if(!(e=0|Ln((A=e-B|0)+1|0,82796,0)))break;If(0|e,0|B,0|A),i[e+A>>0]=0,t[4+(0|Sr(n))>>2]=e}if((0|(o=o+1|0))>=(0|Hn(r))){e=1,u=22;break}}return 22==(0|u)?0|e:(Xe(13,174,171,0,0),0|(u=0))},function(A,e){return e|=0,(0|t[t[(A|=0)>>2]>>2])-(0|t[t[e>>2]>>2])|0},function(A,e){var r,i;return e|=0,i=0|t[(A|=0)+4>>2],r=0|t[e+4>>2],0|(0==(0|(e=0|Ht(0|t[A>>2],0|t[e>>2],(0|i)<(0|r)?i:r)))?i-r|0:e)},Wt,function(A,e){e|=0;var r=0;return 0|(r=(0|t[(A|=0)>>2])-(0|t[e>>2])|0)?0|r:0|(r=0|nf(0|t[A+8>>2],0|t[e+8>>2]))},function(A,e){e|=0;var r,i=0,n=0;i=(r=0|t[(A|=0)>>2])-(0|t[e>>2])|0;A:do{if(!i)switch(n=0|t[A+4>>2],e=0|t[e+4>>2],0|r){case 0:if(0|(i=(A=0|t[n+12>>2])-(0|t[e+12>>2])|0))break A;i=0|Ht(0|t[n+16>>2],0|t[e+16>>2],A);break A;case 1:if(!(A=0|t[n>>2])){i=-1;break A}if(!(i=0|t[e>>2])){i=1;break A}i=0|Wt(A,i);break A;case 2:if(!(A=0|t[n+4>>2])){i=-1;break A}if(!(i=0|t[e+4>>2])){i=1;break A}i=0|Wt(A,i);break A;case 3:i=(0|t[n+8>>2])-(0|t[e+8>>2])|0;break A;default:i=0;break A}}while(0);return 0|i},function(A,e){return e|=0,(0|t[t[(A|=0)>>2]>>2])-(0|t[t[e>>2]>>2])|0},function(A,e){var r,i=0,n=0;if(r=G,G=G+32|0,!(A=0|et(e|=0,0|lt(A|=0))))return Xe(6,177,165,0,0),G=r,0|(i=0);if((0|Hn(A))<=0)return G=r,0|(i=1);for(i=0;;){if(e=0|vn(A,i),0|Wt(0|t[e+4>>2],80604))Xe(6,177,169,0,0),n=0|t[e+4>>2],e=0|t[e+8>>2],t[r>>2]=80644,t[r+4>>2]=n,t[r+8>>2]=80764,t[r+12>>2]=e,Ke(4,r);else{if(!(0|ni(e,r+16|0))){A=7;break}if((0|t[r+16>>2])>0){A=9;break}}if((0|(i=i+1|0))>=(0|Hn(A))){e=1,A=13;break}}return 7==(0|A)?(Xe(6,177,168,0,0),G=r,0|(n=0)):9==(0|A)?(Xe(6,177,167,0,0),G=r,0|(n=0)):13==(0|A)?(G=r,0|e):0},function(A,e){var r,i,n=0,f=0,a=0,B=0,o=0,u=0,l=0,c=0,E=0,Q=0;i=G,G=G+32|0,r=0|et(e|=0,n=0|lt(A|=0));A:do{if((0|Hn(r))<1)Xe(14,123,r?119:120,0,0),t[i>>2]=80635,t[i+4>>2]=n,Ke(2,i);else{if(n=0|Sn(12*(o=0|Hn(r))|0,82796),t[20562]=n,t[20563]=o,!o)return G=i,0|(A=1);a=0|vn(r,0),f=0|et(e,0|t[a+8>>2]);e:do{if(!((0|Hn(f))<1)){for(o=0;;){if(B=0|kt(0|t[a+4>>2],82796,0),t[n+(12*o|0)>>2]=B,!B)break A;if(u=0|Sn((a=0|Hn(f))<<3,82796),t[(B=n+(12*o|0)+4|0)>>2]=u,!u)break A;if(t[n+(12*o|0)+8>>2]=a,0|a){n=0;do{if(l=0|vn(f,n),c=0|t[B>>2],E=0|qt(Q=0|t[l+4>>2],46),t[(u=c+(n<<3)|0)>>2]=0|kt(0==(0|E)?Q:E+1|0,82796,0),l=0|kt(0|t[l+8>>2],82796,0),t[c+(n<<3)+4>>2]=l,n=n+1|0,!(0!=(0|l)&0!=(0|t[u>>2])))break A}while(n>>>0<a>>>0)}if((o=o+1|0)>>>0>=(0|t[20563])>>>0){n=1;break}if(n=0|t[20562],a=0|vn(r,o),(0|Hn(f=0|et(e,0|t[a+8>>2])))<1){B=a,n=a+8|0;break e}}return G=i,0|n}B=a,n=a+8|0}while(0);Xe(14,123,f?117:118,0,0),E=0|t[B+4>>2],Q=0|t[n>>2],t[i+8>>2]=80644,t[i+8+4>>2]=E,t[i+8+8>>2]=80764,t[i+8+12>>2]=Q,Ke(4,i+8|0)}}while(0);return jn(A),G=i,0|(Q=0)},function(A,e){e|=0;var r=0,i=0,n=0;if((0|(r=0|t[(A|=0)>>2]))!=(0|(i=0|t[e>>2]))&&0|(n=0|Wt(r,i)))return 0|(e=n);if(i=0|t[A+4>>2],r=0|t[e+4>>2],i){if(r)return 0|(e=0|Wt(i,r));r=0}return 0|((0|i)==(0|r)?0:0==(0|i)?-1:1)},hf,hf,hf,hf,hf,hf,hf,hf,hf,hf,hf,hf,hf,hf,hf,hf,hf,hf,hf,hf,hf,hf,hf,hf,hf,hf,hf,hf,hf],DB=[Uf,function(A,e,r){e|=0,r|=0;var n,f,a,B,u=0,l=0,c=0,E=0;if(B=G,G=G+144|0,f=0|t[(A|=0)+20>>2],!(u=0|t[f>>2]))return G=B,0|(E=0);if(!(A=0|t[f+12>>2]))return G=B,0|(E=0);if(a=0|t[f+1044>>2],n=0|t[r>>2],0|function(A,e,r,i,n,f,a){A|=0,e|=0,r|=0,i|=0,n|=0,a|=0;var B,o;o=G,G=G+80|0,B=0==(0|(f|=0))?81904:f,f=0|Sn(148,82796,0);do{if(f){if(!(0|ue(f))){le(f),f=0;break}if(0!=(0|(A=0==(0|e)&0==(0|r)?0|fe(f,82796,0,A,0):0|fe(f,e,r,A,0)))&&0!=(0|t[f>>2])&&0!=(0|Di(0|t[f+4>>2],i,n))&&0|t[f>>2]&&0|Mi(0|t[f+4>>2],o,o+64|0)&&0|hi(0|t[f+4>>2],0|t[f+12>>2])&&0|Di(0|t[f+4>>2],o,0|t[o+64>>2])&&0|Mi(0|t[f+4>>2],B,a))return Ci(0|t[f+8>>2]),Ci(0|t[f+12>>2]),Ci(0|t[f+4>>2]),t[f>>2]=0,t[f+16>>2]=0,$r(f+20|0,128),Ii(0|t[f+8>>2]),Ii(0|t[f+12>>2]),Ii(0|t[f+4>>2]),Xn(f,82796,0),G=o,0|(n=B)}else f=0}while(0);return le(f),G=o,0|(n=0)}(u,0|t[f+4>>2],0|t[f+8>>2],A,0|t[f+16>>2],B,B+64|0))if(A=0|t[B+64>>2],(c=((n>>>0)/((l=0|pi(u))>>>0)|0)+(0!=(n-(0|o((n>>>0)/(l>>>0)|0,l))|0)&1)|0)>>>0<=255&&0!=(0|(E=0|oe()))){A:do{if(0|fe(E,B,A,u,0)){if(c){for(r=0,u=1;;){if(i[B+128>>0]=u,u>>>0>1){if(!(0|fe(E,0,0,0,0)))break;if(!(0|ae(E,B+64|0,l)))break}if(!(0|ae(E,f+20|0,a)))break;if(!(0|ae(E,B+128|0,1)))break;if(A=(r+l|0)>>>0>n>>>0?n-r|0:l,0==(0|Be(E,B+64|0,0)))break;if(If(e+r|0,B+64|0,0|A),(u=u+1|0)>>>0>c>>>0)break A;r=A+r|0}e=0}}else e=0}while(0);$r(B+64|0,64),le(E)}else e=0;else e=0;return G=B,0|0!=(0|e)&1},function(A,e,r){return A|=0,r|=0,0|Wt(e|=0,53817)?0|Wt(e,53796)?0|Wt(e,53801)?0|Wt(e,54589)?0|Wt(e,54593)?0|Wt(e,65373)?0|Wt(e,53809)?0|(r=-2):0|(r=0|bi(A,4102,r)):0|(r=0|wi(A,4102,r)):0|(r=0|bi(A,4101,r)):0|(r=0|wi(A,4101,r)):0|(r=0|bi(A,4100,r)):0|(r=0|wi(A,4100,r)):0|(r=0|Qi(A,-1,1024,4099,0,0|Xi(r)))},function(A,e,r){e|=0,r|=0;var n,f=0,a=0,B=0,o=0;if(n=0|t[(A|=0)+20>>2],0|(A=0|t[n>>2])&&0|(B=0|t[n+4>>2])&&0|(o=0|t[n+1036>>2])){if(f=0|t[n+8>>2],a=0|t[r>>2],114!=(0|Si(A)))return 0|(e=0!=(0|F(A,B,f,n+12|0,o,e,a))&1);if(!(0|F(52868,B,(f>>>1)+(1&f)|0,n+12|0,o,e,a)))return 0|(e=0);if(!(r=0|Ln(a,82796,0)))return 0|(e=0);if(!(0|F(52916,B+(f>>>1)|0,(f>>>1)+(1&f)|0,n+12|0,o,r,a)))return pn(r,a,82796,0),0|(e=0);if(0|a){A=0;do{i[(o=e+A|0)>>0]=i[o>>0]^i[r+A>>0],A=A+1|0}while((0|A)!=(0|a))}return pn(r,a,82796,0),0|(e=1)}return Xe(52,101,101,0,0),0|(e=0)},function(A,e,r){return A|=0,e|=0,(r|=0)?0|Wt(e,53817)?0|Wt(e,53820)?0|Wt(e,53827)?0|Wt(e,53837)?0|Wt(e,53842)?0|(r=-2):0|(r=0|bi(A,4098,r)):0|(r=0|wi(A,4098,r)):0|(r=0|bi(A,4097,r)):0|(r=0|wi(A,4097,r)):(e=0|t[A+20>>2],(A=0|Xi(r))?(t[e>>2]=A,0|(r=1)):(Xe(52,100,100,0,0),0|(r=0))):(Xe(52,100,102,0,0),0|(r=0))},function(A,e,r){A|=0,e|=0,r|=0;do{if(r){if(!(0|Wt(e,54589))){A=0|wi(A,6,r);break}A=0|Wt(e,54593)?-2:0|bi(A,6,r)}else A=0}while(0);return 0|A},function(A,e,r){var i;return A|=0,e|=0,r|=0,0|(i=0|EA(4))&&0|MA(i,0|t[e>>2],r)&&0|Yi(A,855,i)?0|(A=1):(eA(i),0|(A=0))},function(A,e,r){return A|=0,(r=0|Qr(0,e|=0,r|=0))?(Yi(A,6,r),0|(A=1)):(Xe(4,147,4,0,0),0|(A=0))},function(A,e,r){var i;if(A|=0,e|=0,i=G,G=G+16|0,!(r|=0))return Xe(4,144,147,0,0),G=i,0|(A=0);if(!(0|Wt(e,55331))){if(0|Wt(r,59750))if(0|Wt(r,55348))if(0|Wt(r,55355))if(0!=(0|Wt(r,55360))&&0!=(0|Wt(r,55365)))if(0|Wt(r,55370)){if(0|Wt(r,65006))return Xe(4,144,118,0,0),G=i,0|(A=-2);e=6}else e=5;else e=4;else e=3;else e=2;else e=1;return A=0|Qi(A,6,-1,4097,e,0),G=i,0|A}return 0|Wt(e,55375)?0|Wt(e,55391)?0|Wt(e,55407)?0|Wt(e,55425)?0|Wt(e,55437)?0|Wt(e,55449)?(G=i,0|(A=-2)):((r=0|Dt(r,i))?(0|(e=0|Qi(A,6,768,4106,0|t[i>>2],r)))<1&&Xn(r,82796,0):e=0,G=i,0|(A=e)):(e=0|Xi(r))?(A=0|Qi(A,6,768,4105,0,e),G=i,0|A):(Xe(4,144,157,0,0),G=i,0|(A=0)):(e=0|Xi(r))?(A=0|Qi(A,6,1016,4101,0,e),G=i,0|A):(Xe(4,144,157,0,0),G=i,0|(A=0)):(t[i>>2]=0,0|Bn(i,r)?(0|(e=0|Qi(A,6,4,4100,0,0|t[i>>2])))<1&&Pf(0|t[i>>2]):e=0,G=i,0|(A=e)):(A=0|Qi(A,6,4,4099,0|tf(r),0),G=i,0|A):(A=0|Qi(A,6,24,4098,0|tf(r),0),G=i,0|A)},function(A,e,r){return 0|Df(0|(A|=0),0|(e|=0),0|(r|=0))},function(A,e,r){return e|=0,r|=0,0|ce(0|Ki(A|=0),e,r)},function(A,e,r){return e|=0,r|=0,0|bB(0|Ki(A|=0),e,r)},Et,function(A,e,r){return A|=0,e|=0,r|=0,0|(e=0)?(r=0|Et(A,e,r),ke(e),0|r):128==(4095&(0|pe())|0)?(Xe(14,120,114,0,0),0|(r=0)):(Xe(14,120,2,0,0),0|(r=0))},function(A,e,r){e|=0,r|=0;var i=0,n=0,f=0;if(!(A|=0))return 0|(r=0);A:do{switch(0|e){case 3:if(!(0|Af(A,54471,3)))return t[r>>2]=10502|t[r>>2],0|(r=1);for(i=0,e=544;;){if(3==(0|t[e+4>>2])&&0==(0|Af(0|t[e>>2],A,3)))break A;if(i=i+1|0,e=e+12|0,t[20510]=e,i>>>0>=49){e=0;break}}return 0|e;case-1:n=0|mt(A),f=7;break;default:n=e,f=7}}while(0);A:do{if(7==(0|f)){for(i=0,e=544;;){if((0|n)==(0|t[e+4>>2])&&0==(0|Af(0|t[e>>2],A,n)))break A;if(i=i+1|0,e=e+12|0,t[20510]=e,i>>>0>=49){e=0;break}}return 0|e}}while(0);return 0!=(0|(e=0|t[e+8>>2]))&0==(65536&e|0)&&(e=0|function(A){A=(A|=0)>>>0>30?0:0|t[1136+(A<<2)>>2];return 0|A}(e))?(t[r>>2]=t[r>>2]|e,0|(r=1)):0|(r=0)},function(A,e,r){return e|=0,r|=0,0!=(0|ae(0|t[20+(0|t[20+(0|zi(A|=0))>>2])>>2],e,r))|0},function(A,e,r){return 0|se(r|=0,A|=0,e|=0)},function(A,e,r){var i;return A|=0,e|=0,r=0|function(A){return 0|t[(A|=0)+4>>2]}(r|=0),i=0|t[r+32>>2],t[i>>2]=A,t[i+4>>2]=e,0|IB[31&t[r+28>>2]](r)},on,cn,Ln,Uf,Uf,Uf,Uf,Uf,Uf,Uf,Uf,Uf,Uf,Uf],MB=[Gf,function(A,e,r,i){e|=0,r|=0,i|=0;var n,f=0;switch(n=0|t[(A|=0)+20>>2],0|e){case 4099:i?(t[n>>2]=i,A=1):A=0;break;case 4100:0==(0|r)|0==(0|i)?A=1:(0|r)>=0?(0|(A=0|t[n+4>>2])&&pn(A,0|t[n+8>>2],82796,0),i=0|Ct(i,r,82796,0),t[n+4>>2]=i,i?(t[n+8>>2]=r,A=1):A=0):A=0;break;case 4101:(0|r)>=0?(0|(A=0|t[n+12>>2])&&pn(A,0|t[n+16>>2],82796,0),i=0|Ct(i,r,82796,0),t[n+12>>2]=i,i?(t[n+16>>2]=r,A=1):A=0):A=0;break;case 4102:0==(0|r)|0==(0|i)?A=1:(0|r)>=0&&(1024-(f=0|t[n+1044>>2])|0)>=(0|r)?(If(n+20+f|0,0|i,0|r),t[n+1044>>2]=(0|t[n+1044>>2])+r,A=1):A=0;break;default:A=-2}return 0|A},function(A,e,r,i){var n;switch(e|=0,r|=0,i|=0,n=0|t[(A|=0)+20>>2],0|e){case 4096:return t[n>>2]=i,0|(i=1);case 4097:return(0|r)<0?0|(i=0):(0|(A=0|t[n+4>>2])&&pn(A,0|t[n+8>>2],82796,0),$r(n+12|0,0|t[n+1036>>2]),t[n+1036>>2]=0,i=0|Ct(i,r,82796,0),t[n+4>>2]=i,i?(t[n+8>>2]=r,0|(i=1)):0|(i=0));case 4098:return 0==(0|r)|0==(0|i)?0|(i=1):(0|r)<0||(1024-(A=0|t[n+1036>>2])|0)<(0|r)?0|(i=0):(If(n+12+A|0,0|i,0|r),t[n+1036>>2]=(0|t[n+1036>>2])+r,0|(i=1));default:return 0|(i=-2)}return 0},function(A,e,r,n){return e|=0,r|=0,n|=0,(n=0|t[(A|=0)>>2])?(r=0==(7&(0|Kf(n))|0),0|e&&(r&&(i[e>>0]=0,e=e+1|0),ua(n,e)),0|(A=((7+(0|Kf(n))|0)/8|0)+(1&r)|0)):0|(A=-1)},function(A,e,r,n){e|=0,r|=0,n|=0;var a,B,o=0;if((0|(o=f[(A|=0)>>0]|f[A+1>>0]<<8|f[A+2>>0]<<16|f[A+3>>0]<<24))==(0|t[n+20>>2]))return 0|(o=-1);if(B=0==(((r=o>>31^o)>>>31)+(r>>>0>1&1)+(0!=(0|r)&1)+(r>>>0>3&1)+(r>>>0>7&1)+(r>>>0>15&1)+(r>>>0>31&1)+(r>>>0>63&1)+(r>>>0>127&1)+(r>>>0>255&1)+(r>>>0>511&1)+(r>>>0>1023&1)+(r>>>0>2047&1)+(r>>>0>4095&1)+(r>>>0>8191&1)+(r>>>0>16383&1)+(r>>>0>32767&1)+(r>>>0>65535&1)+(r>>>0>131071&1)+(r>>>0>262143&1)+(r>>>0>524287&1)+(r>>>0>1048575&1)+(r>>>0>2097151&1)+(r>>>0>4194303&1)+(r>>>0>8388607&1)+(r>>>0>16777215&1)+(r>>>0>33554431&1)+(r>>>0>67108863&1)+(r>>>0>134217727&1)+(r>>>0>268435455&1)+(r>>>0>536870911&1)+(r>>>0>1073741823&1)&7|0),a=(r>>>31)+(r>>>0>1&1)+(0!=(0|r)&1)+(r>>>0>3&1)+(r>>>0>7&1)+(r>>>0>15&1)+(r>>>0>31&1)+(r>>>0>63&1)+(r>>>0>127&1)+(r>>>0>255&1)+(r>>>0>511&1)+(r>>>0>1023&1)+(r>>>0>2047&1)+(r>>>0>4095&1)+(r>>>0>8191&1)+(r>>>0>16383&1)+(r>>>0>32767&1)+(r>>>0>65535&1)+(r>>>0>131071&1)+(r>>>0>262143&1)+(r>>>0>524287&1)+(r>>>0>1048575&1)+(r>>>0>2097151&1)+(r>>>0>4194303&1)+(r>>>0>8388607&1)+(r>>>0>16777215&1)+(r>>>0>33554431&1)+(r>>>0>67108863&1)+(r>>>0>134217727&1)+(r>>>0>268435455&1)+(r>>>0>536870911&1)+(r>>>0>1073741823&1)+7>>3,0|e&&(B?(i[e>>0]=o>>31,A=e+1|0):A=e,((r>>>31)+(r>>>0>1&1)+(0!=(0|r)&1)+(r>>>0>3&1)+(r>>>0>7&1)+(r>>>0>15&1)+(r>>>0>31&1)+(r>>>0>63&1)+(r>>>0>127&1)+(r>>>0>255&1)+(r>>>0>511&1)+(r>>>0>1023&1)+(r>>>0>2047&1)+(r>>>0>4095&1)+(r>>>0>8191&1)+(r>>>0>16383&1)+(r>>>0>32767&1)+(r>>>0>65535&1)+(r>>>0>131071&1)+(r>>>0>262143&1)+(r>>>0>524287&1)+(r>>>0>1048575&1)+(r>>>0>2097151&1)+(r>>>0>4194303&1)+(r>>>0>8388607&1)+(r>>>0>16777215&1)+(r>>>0>33554431&1)+(r>>>0>67108863&1)+(r>>>0>134217727&1)+(r>>>0>268435455&1)+(r>>>0>536870911&1)+(r>>>0>1073741823&1)|0)>0))for(n=a;e=n,i[A+(n=n+-1|0)>>0]=r^o>>31,!((0|e)<=1);)r>>>=8;return 0|(o=a+(1&B)|0)},function(A,e,r,i){var n,f;return e|=0,r|=0,i|=0,f=G,G=G+16|0,n=0|t[(A|=0)+20>>2],(0|(A=0|pi(0|function(A){A=(A|=0)?0|t[A>>2]:0;return 0|A}(i))))>=0?(t[r>>2]=A,e?0|Be(0|t[n+20>>2],e,f)?(t[r>>2]=t[f>>2],A=1):A=0:A=1):A=0,G=f,0|A},function(A,e,r,i){var n;switch(e|=0,r|=0,i|=0,n=0|t[(A|=0)+20>>2],0|e){case 6:if((0|r)<-1|(0|r)>0&0==(0|i))return 0|(e=0);if(!(0|MA(n+4|0,i,r)))return 0|(e=0);break;case 1:t[n>>2]=i;break;case 7:if(e=0|t[24+(0|t[A+8>>2])>>2],!(0|fe(0|t[n+20>>2],0|t[e+8>>2],0|t[e>>2],0|t[n>>2],0|t[A+4>>2])))return 0|(e=0);break;default:return 0|(e=-2)}return 0|(e=1)},function(A,e,r,i){return 0,r|=0,i|=0,3==(0|(e|=0))?(t[i>>2]=672,r=1):r=-2,0|r},function(A,e,r,i){A|=0,e|=0,r|=0;var n,f,a,B=0,o=0,u=0;if(iB(i|=0),n=0|tB(i),f=0|tB(i),!(a=0|tB(i)))return r=0,nB(i),0|r;B=0|t[r+56>>2];do{if(4&B){if(!(B=0|qf()))return r=0,nB(i),0|r;if(Da(B,0|t[r+28>>2],4),0|pa(r+64|0,0|t[r+84>>2],B,i)&&(Da(B,0|t[r+32>>2],4),0|pa(r+68|0,0|t[r+84>>2],B,i))){Pf(B),o=4==(0|t[24+(0|t[r+8>>2])>>2])?(0|(o=0|Kf(0|t[r+32>>2])))==(0|Kf(0|t[r+28>>2])):0,B=0|t[r+56>>2],o&=1;break}return Pf(B),r=0,nB(i),0|r}o=0}while(0);if(2&B|0&&0==(0|pa(r+60|0,0|t[r+84>>2],0|t[r+16>>2],i)))return r=0,nB(i),0|r;if(o){if(!(0|Ja(f,e,0|t[r+68>>2],i)))return r=0,nB(i),0|r;if(!(0|Xa(f,f,0|t[r+68>>2],i)))return r=0,nB(i),0|r;if(!(0|_a(f,f,0|t[r+40>>2],0|t[r+32>>2],i,0|t[r+68>>2])))return r=0,nB(i),0|r;if(!(0|Ja(n,e,0|t[r+64>>2],i)))return r=0,nB(i),0|r;if(!(0|Xa(n,n,0|t[r+64>>2],i)))return r=0,nB(i),0|r;if(!(0|_a(n,n,0|t[r+36>>2],0|t[r+28>>2],i,0|t[r+64>>2])))return r=0,nB(i),0|r;if(!(0|function(A,e,r,i){A|=0,e|=0,r|=0;var n,f,a=0,B=0,o=0,u=0,l=0,c=0,E=0,Q=0,w=0;if(n=0|t[(i=i|0)+4>>2],!(0|ia(A,n)))return 0|(A=0);if(f=0|t[A>>2],B=0==(0|(B=0|t[e>>2]))?f:B,l=0==(0|(l=0|t[r>>2]))?f:l,0|n){for(o=0,u=0,c=0,a=0,E=0;w=a-(0|t[e+4>>2])>>31&t[B+(o<<2)>>2],Q=a-(0|t[r+4>>2])>>31&t[l+(u<<2)>>2],t[f+(a<<2)>>2]=w+E-Q,E=0-(c=(0|w)==(0|Q)?c:w>>>0<Q>>>0&1)|0,(0|(a=a+1|0))!=(0|n);)o=((a-(0|t[e+8>>2])|0)>>>31)+o|0,u=((a-(0|t[r+8>>2])|0)>>>31)+u|0;if(u=0|t[i>>2],0|n){a=0,B=0;do{w=(t[u+(B<<2)>>2]&E)+a|0,Q=w+(0|t[(i=f+(B<<2)|0)>>2])|0,t[i>>2]=Q,a=(Q>>>0<w>>>0&1)+(w>>>0<a>>>0&1)|0,B=B+1|0}while((0|B)!=(0|n));if(r=a-c|0,0|n)for(e=0,o=0;a=(t[u+(o<<2)>>2]&r)+e|0,B=a+(0|t[(w=f+(o<<2)|0)>>2])|0,t[w>>2]=B,(0|(o=o+1|0))!=(0|n);)e=(B>>>0<a>>>0&1)+(a>>>0<e>>>0&1)|0}}return t[A+4>>2]=n,t[A+12>>2]=0,0|(w=1)}(n,n,f,0|t[r+28>>2])))return r=0,nB(i),0|r;if(!(0|Xa(n,n,0|t[r+64>>2],i)))return r=0,nB(i),0|r;if(!(0|ma(n,n,0|t[r+44>>2],0|t[r+64>>2],i)))return r=0,nB(i),0|r;if(!(0|pf(A,n,0|t[r+32>>2],i)))return r=0,nB(i),0|r;if(!(0|function(A,e,r,i){A|=0,e|=0,r|=0;var n,f,a=0,B=0,o=0,u=0,l=0,c=0,E=0,Q=0,w=0;if(f=G,G=G+128|0,n=0|t[(i=i|0)+4>>2],!(0|ia(A,n)))return G=f,0|(A=0);if(n>>>0>32){if(!(a=0|Ln(n<<2,82796,0)))return G=f,0|(A=0)}else a=f;if(E=0==(0|(E=0|t[e>>2]))?a:E,c=0==(0|(c=0|t[r>>2]))?a:c,n)for(u=0,l=0,B=0,o=0;Q=(o-(0|t[e+4>>2])>>31&t[E+(u<<2)>>2])+B|0,w=(o-(0|t[r+4>>2])>>31&t[c+(l<<2)>>2])+Q|0,t[a+(o<<2)>>2]=w,B=(w>>>0<Q>>>0&1)+(Q>>>0<B>>>0&1)|0,(0|(o=o+1|0))!=(0|n);)u=((o-(0|t[e+8>>2])|0)>>>31)+u|0,l=((o-(0|t[r+8>>2])|0)>>>31)+l|0;else B=0;if(u=0|t[A>>2],o=B-(0|Un(u,a,0|t[i>>2],n))|0,0|n){B=0;do{w=a+(B<<2)|0,t[(Q=u+(B<<2)|0)>>2]=t[Q>>2]&~o|t[w>>2]&o,t[w>>2]=0,B=B+1|0}while((0|B)!=(0|n))}if(t[A+4>>2]=n,t[A+12>>2]=0,(0|a)==(0|f))return G=f,0|(w=1);return Xn(a,82796,0),G=f,0|(w=1)}(A,A,f,0|t[r+16>>2])))return r=0,nB(i),0|r}else{if(!(B=0|qf()))return r=0,nB(i),0|r;if(Da(B,e,4),!(0|wB(0,n,B,0|t[r+32>>2],i)))return Pf(B),r=0,nB(i),0|r;if(!(o=0|qf()))return Pf(B),r=0,nB(i),0|r;if(Da(o,0|t[r+40>>2],4),!(0|UB[7&t[24+(0|t[r+8>>2])>>2]](f,n,o,0|t[r+32>>2],i,0|t[r+68>>2])))return Pf(B),Pf(o),r=0,nB(i),0|r;if(Pf(o),o=0==(0|wB(0,n,B,0|t[r+28>>2],i)),Pf(B),o)return r=0,nB(i),0|r;if(!(B=0|qf()))return r=0,nB(i),0|r;if(Da(B,0|t[r+36>>2],4),o=0==(0|UB[7&t[24+(0|t[r+8>>2])>>2]](A,n,B,0|t[r+28>>2],i,0|t[r+64>>2])),Pf(B),o)return r=0,nB(i),0|r;if(!(0|cn(A,A,f)))return r=0,nB(i),0|r;if(0|Ia(A)&&0==(0|on(A,A,0|t[r+28>>2])))return r=0,nB(i),0|r;if(!(0|Sf(n,A,0|t[r+44>>2],i)))return r=0,nB(i),0|r;if(!(B=0|qf()))return r=0,nB(i),0|r;if(Da(B,n,4),o=0==(0|wB(0,A,B,0|t[r+28>>2],i)),Pf(B),o)return r=0,nB(i),0|r;if(0|Ia(A)&&0==(0|on(A,A,0|t[r+28>>2])))return r=0,nB(i),0|r;if(!(0|Sf(n,A,0|t[r+32>>2],i)))return r=0,nB(i),0|r;if(!(0|on(A,n,f)))return r=0,nB(i),0|r}if(0|(B=0|t[r+20>>2])&&0|(u=0|t[r+16>>2])){if(4==(0|t[24+(0|t[r+8>>2])>>2])){if(!(0|Pa(a,A,B,u,i,0|t[r+60>>2])))return r=0,nB(i),0|r}else if(aa(A),!(0|UB[7&t[24+(0|t[r+8>>2])>>2]](a,A,0|t[r+20>>2],0|t[r+16>>2],i,0|t[r+60>>2])))return r=0,nB(i),0|r;if(!(0|cn(a,a,e)))return r=0,nB(i),0|r;if(0|jf(a))return aa(A),r=1,nB(i),0|r;if(!(0|wB(0,a,a,0|t[r+16>>2],i)))return r=0,nB(i),0|r;if(0|Ia(a)&&0==(0|on(a,a,0|t[r+16>>2])))return r=0,nB(i),0|r;if(!(0|jf(a))){if(!(B=0|qf()))return r=0,nB(i),0|r;if(Da(B,0|t[r+24>>2],4),r=0==(0|UB[7&t[24+(0|t[r+8>>2])>>2]](A,e,B,0|t[r+16>>2],i,0|t[r+60>>2])),Pf(B),r)return r=0,nB(i),0|r}}return aa(A),r=1,nB(i),0|r},function(A,e,r,i){return r|=0,0,0|Ar(A|=0,0|t[(e|=0)+24>>2],r,0)},function(A,e,r,i){return r|=0,0,0|Ar(A|=0,0|t[(e|=0)+24>>2],r,1)},function(A,e,r,i){A|=0,e|=0,r|=0,i|=0;var n,f=0;switch(n=G,G=G+16|0,t[n>>2]=0,0|e){case 1:r?A=1:(!function(A,e,r,i){A|=0,r|=0,i|=0,0|(e|=0)&&(t[e>>2]=t[A+28>>2]);0|r&&(t[r>>2]=t[A+8>>2]);if(!i)return;t[i>>2]=t[A+16>>2]}(i,0,0,n),f=7);break;case 2:r?A=1:(!function(A,e){if(A|=0,!(e|=0))return;t[e>>2]=t[A+8>>2]}(i,n),f=7);break;case 3:t[i>>2]=672,A=1;break;default:A=-2}return 7==(0|f)&&((A=0|t[n>>2])?(yA(A,0|Sr(6),5,0),A=1):A=1),G=n,0|A},function(A,e,r,i){switch(e|=0,0,i|=0,0|(A|=0)){case 0:i=0|kr(),t[e>>2]=i,i=0==(0|i)?0:2;break;case 2:sr(0|t[e>>2]),t[e>>2]=0,i=2;break;default:i=1}return 0|i},function(A,e,r,i){var n;e|=0,r|=0,i|=0,n=0|t[(A|=0)+20>>2];A:do{switch(0|e){case 4097:e:do{if((r+-1|0)>>>0<6){if(!(0|Rr(0|t[n+20>>2],r))){A=0;break A}switch(0|r){case 6:if(!(24&t[A+16>>2]))break e;0|t[n+20>>2]||(t[n+20>>2]=52916);break;case 4:if(!(768&t[A+16>>2]))break e;0|t[n+20>>2]||(t[n+20>>2]=52916)}t[n+16>>2]=r,A=1;break A}}while(0);Xe(4,143,144,0,0),A=-2;break;case 4102:t[i>>2]=t[n+16>>2],A=1;break;case 4103:case 4098:if(6!=(0|t[n+16>>2])){Xe(4,143,146,0,0),A=-2;break A}if(4103==(0|e)){t[i>>2]=t[n+28>>2],A=1;break A}(0|r)<-2?A=-2:(t[n+28>>2]=r,A=1);break;case 4099:if((0|r)<512){Xe(4,143,120,0,0),A=-2;break A}t[n>>2]=r,A=1;break A;case 4100:if(0|i&&0|da(i)&&0==(0|sa(i))){Pf(0|t[n+4>>2]),t[n+4>>2]=i,A=1;break A}Xe(4,143,101,0,0),A=-2;break;case 4107:case 4105:if(4!=(0|t[n+16>>2])){Xe(4,143,141,0,0),A=-2;break A}if(4107==(0|e)){t[i>>2]=t[n+20>>2],A=1;break A}t[n+20>>2]=i,A=1;break A;case 1:0|Rr(i,0|t[n+16>>2])?(t[n+20>>2]=i,A=1):A=0;break;case 13:t[i>>2]=t[n+20>>2],A=1;break;case 4104:case 4101:switch(0|t[n+16>>2]){case 4:case 6:break;default:Xe(4,143,156,0,0),A=-2;break A}if(4104!=(0|e)){t[n+24>>2]=i,A=1;break A}if(A=0|t[n+24>>2]){t[i>>2]=A,A=1;break A}t[i>>2]=t[n+20>>2],A=1;break A;case 4106:if(4!=(0|t[n+16>>2])){Xe(4,143,141,0,0),A=-2;break A}if(Xn(0|t[n+36>>2],82796,0),(0|r)>0&0!=(0|i)){t[n+36>>2]=i,t[n+40>>2]=r,A=1;break A}t[n+36>>2]=0,t[n+40>>2]=0,A=1;break A;case 4108:if(4==(0|t[n+16>>2])){t[i>>2]=t[n+36>>2],A=0|t[n+40>>2];break A}Xe(4,143,141,0,0),A=-2;break A;case 5:case 4:case 3:case 7:A=1;break;case 2:Xe(4,143,148,0,0),A=-2;break;default:A=-2}}while(0);return 0|A},function(A,e,r,i){switch(e|=0,0,0,0|(A|=0)){case 3:return yi(0|t[8+(0|t[e>>2])>>2]),0|(e=1);case 5:return e=0|t[e>>2],yi(0|t[e+8>>2]),t[e+8>>2]=0,xe(),-1==(0|function(A,e){A|=0,e|=0;var r,i=0;if(!(r=0|Fi()))return Xe(11,148,65,0,0),0|(A=-1);do{if(0|Ni(r,0|zr(0|t[t[e>>2]>>2]))){if(!(i=0|t[20+(0|t[r+12>>2])>>2])){Xe(11,148,124,0,0);break}if(!(0|gB[63&i](r,e))){Xe(11,148,125,0,0);break}return t[A>>2]=r,0|(A=1)}Xe(11,148,111,0,0)}while(0);return yi(r),0|(A=0)}(e+8|0,e))?0|(e=0):(Oe(),0|(e=1));default:return 0|(e=1)}return 0},function(A,e,r,i){A|=0,r|=0,i|=0;var n,f=0;if(n=G,G=G+80|0,29==(0|(e|=0)))if(0!=(0|A)&&(f=0|Ki(A),48==(0|r))&&(0|bB(f,i,48))>=1){A=(e=n+32|0)+40|0;do{t[e>>2]=909522486,e=e+4|0}while((0|e)<(0|A));if(0!=(0|bB(f,n+32|0,40))&&0!=(0|sB(n,f))&&0!=(0|CB(f))&&(0|bB(f,i,48))>=1){A=(e=n+32|0)+40|0;do{t[e>>2]=1549556828,e=e+4|0}while((0|e)<(0|A));0!=(0|bB(f,n+32|0,40))&&0!=(0|bB(f,n,20))?($r(n,20),e=1):e=0}else e=0}else e=0;else e=-2;return G=n,0|e},Fa,function(A,e,r,i){return 0|Wa(1,A|=0,e|=0,r|=0,i|=0)},Tn,Gf,Gf,Gf,Gf,Gf,Gf,Gf,Gf,Gf,Gf,Gf,Gf,Gf],hB=[Zf,function(A,e,r,i,n){return 0,0,0,0|function(A,e){A|=0;var r=0,i=0,n=0,f=0,a=0;if(0|t[12+(e|=0)>>2]&&1!=(0|se(A,80624,1)))return 0|(A=0);if(0|jf(e)&&1!=(0|se(A,80614,1)))return 0|(A=0);if((0|(r=0|t[e+4>>2]))<=0)return 0|(A=1);i=0;for(;;){if(a=r,r=r+-1|0,(n=0|t[(0|t[e>>2])+(r<<2)>>2])>>>28|i){if(1!=(0|se(A,1312+(n>>>28)|0,1))){r=0,i=11;break}n=0|t[(0|t[e>>2])+(r<<2)>>2],i=1}if((f=n>>>24&15)|i){if(1!=(0|se(A,1312+f|0,1))){r=0,i=11;break}n=0|t[(0|t[e>>2])+(r<<2)>>2],i=1}if((f=n>>>20&15)|i){if(1!=(0|se(A,1312+f|0,1))){r=0,i=11;break}n=0|t[(0|t[e>>2])+(r<<2)>>2],i=1}if((f=n>>>16&15)|i){if(1!=(0|se(A,1312+f|0,1))){r=0,i=11;break}n=0|t[(0|t[e>>2])+(r<<2)>>2],i=1}if((f=n>>>12&15)|i){if(1!=(0|se(A,1312+f|0,1))){r=0,i=11;break}n=0|t[(0|t[e>>2])+(r<<2)>>2],i=1}if((f=n>>>8&15)|i){if(1!=(0|se(A,1312+f|0,1))){r=0,i=11;break}n=0|t[(0|t[e>>2])+(r<<2)>>2],i=1}if((f=n>>>4&15)|i){if(1!=(0|se(A,1312+f|0,1))){r=0,i=11;break}n=0|t[(0|t[e>>2])+(r<<2)>>2],i=1}if((n&=15)|i){if(1!=(0|se(A,1312+n|0,1))){r=0,i=11;break}i=1}if((0|a)<=1){r=1,i=11;break}}return 11==(0|i)?0|r:0}(A|=0,0|t[(e|=0)>>2])?0|(e=(0|Ce(A,80479))>0&1):0|(e=0)},function(A,e,r,i,n){return A|=0,e|=0,0,0,n|=0,n=G,G=G+16|0,t[n>>2]=t[e>>2],e=0|ge(A,54584,n),G=n,0|e},function(A,e,r,i,n){A|=0,e|=0,r|=0,n|=0;var f=0,a=0,B=0,o=0,u=0;if((0|Kf(0|t[(i|=0)+16>>2]))>16384)return Xe(4,104,105,0,0),0|(u=-1);if((0|la(0|t[i+16>>2],0|t[i+20>>2]))<1)return Xe(4,104,101,0,0),0|(u=-1);if((0|Kf(0|t[i+16>>2]))>3072&&(0|Kf(0|t[i+20>>2]))>64)return Xe(4,104,101,0,0),0|(u=-1);if(u=0|eB()){iB(u),B=0|tB(u),o=0|tB(u),a=0|Ln(f=(7+(0|Kf(0|t[i+16>>2]))|0)/8|0,82796,0);A:do{if(0==(0|B)|0==(0|o)|0==(0|a))Xe(4,104,65,0,0),A=-1;else{switch(0|n){case 1:A=0|Fr(a,f,e,A);break;case 4:A=0|rr(a,f,e,A,0,0);break;case 2:A=0|lr(a,f,e,A);break;case 3:A=0|Cr(a,f,e,A);break;default:Xe(4,104,118,0,0),A=-1;break A}if((0|A)>=1&&0!=(0|fa(a,f,B))){if((0|la(B,0|t[i+16>>2]))>-1){Xe(4,104,132,0,0),A=-1;break}if(2&t[i+56>>2]|0&&0==(0|pa(i+60|0,0|t[i+84>>2],0|t[i+16>>2],u))){A=-1;break}A=0|UB[7&t[24+(0|t[i+8>>2])>>2]](o,B,0|t[i+20>>2],0|t[i+16>>2],u,0|t[i+60>>2])?0|Ba(o,r,f):-1}else A=-1}}while(0);nB(u)}else a=0,f=0,A=-1;return rB(u),pn(a,f,82796,0),0|(u=A)},function(A,e,r,i,n){A|=0,e|=0,r|=0,n|=0;var f=0,a=0,B=0,o=0,u=0;if((0|Kf(0|t[(i|=0)+16>>2]))>16384)return Xe(4,103,105,0,0),0|(u=-1);if((0|la(0|t[i+16>>2],0|t[i+20>>2]))<1)return Xe(4,103,101,0,0),0|(u=-1);if((0|Kf(0|t[i+16>>2]))>3072&&(0|Kf(0|t[i+20>>2]))>64)return Xe(4,103,101,0,0),0|(u=-1);if(u=0|eB()){iB(u),f=0|tB(u),o=0|tB(u),B=0|Ln(a=(7+(0|Kf(0|t[i+16>>2]))|0)/8|0,82796,0);A:do{if(0==(0|f)|0==(0|o)|0==(0|B))Xe(4,103,65,0,0),f=-1;else{if((0|a)<(0|A)){Xe(4,103,108,0,0),f=-1;break}if(0|fa(e,A,f)){if((0|la(f,0|t[i+16>>2]))>-1){Xe(4,103,132,0,0),f=-1;break}if(2&t[i+56>>2]|0&&0==(0|pa(i+60|0,0|t[i+84>>2],0|t[i+16>>2],u))){f=-1;break}if(0|UB[7&t[24+(0|t[i+8>>2])>>2]](o,f,0|t[i+20>>2],0|t[i+16>>2],u,0|t[i+60>>2])){e:do{if(5==(0|n)){if(12!=(15&t[(0|En(o))>>2]|0)&&0==(0|cn(o,0|t[i+16>>2],o))){f=-1;break A}f=0|qe(r,a,B,0|Ba(o,B,a),a)}else switch(f=0|Ba(o,B,a),0|n){case 1:f=0|Nr(r,a,B,f,a);break e;case 3:If(0|r,0|B,0|f);break e;default:Xe(4,103,118,0,0),f=-1;break A}}while(0);(0|f)<0&&Xe(4,103,114,0,0)}else f=-1}else f=-1}}while(0);nB(u)}else B=0,a=0,f=-1;return rB(u),pn(B,a,82796,0),0|(u=f)},function(A,e,r,i,n){A|=0,e|=0,r|=0,i|=0,n|=0;var f,a,B=0,o=0,u=0,l=0,c=0,E=0,Q=0;if(!(a=0|eB()))return r=0,c=0,E=-1,rB(a),pn(r,c,82796,0),0|E;iB(a),f=0|tB(a),B=0|tB(a),c=0|Ln(E=(7+(0|Kf(0|t[i+16>>2]))|0)/8|0,82796,0);A:do{if(0==(0|f)|0==(0|B)|0==(0|c))Xe(4,102,65,0,0),B=-1;else{switch(0|n){case 1:A=0|Zr(c,E,e,A);break;case 5:A=0|_e(c,E,e,A);break;case 3:A=0|Cr(c,E,e,A);break;default:Xe(4,102,118,0,0),B=-1;break A}if((0|A)>=1&&0!=(0|fa(c,E,f))){if((0|la(f,0|t[i+16>>2]))>-1){Xe(4,102,132,0,0),B=-1;break}if(2&(A=0|t[i+56>>2])){if(!(0|pa(i+60|0,0|t[i+84>>2],0|t[i+16>>2],a))){B=-1;break}A=0|t[i+56>>2]}e:do{if(!(128&A)){R(0|t[i+84>>2]),(A=0|t[i+76>>2])?(e=A,l=20):(A=0|hr(i,a),t[i+76>>2]=A,A?(e=A,l=20):y(0|t[i+84>>2]));r:do{if(20==(0|l)){do{if(!(0|Cn(e))){if(A=0|t[i+80>>2])y(0|t[i+84>>2]);else if(A=0|hr(i,a),t[i+80>>2]=A,y(0|t[i+84>>2]),!A)break r;if(e=0|tB(a)){dn(A),o=0|kn(f,e,A,a),In(A);break}Xe(4,102,65,0,0),B=-1;break A}y(0|t[i+84>>2]),A=e,o=0|kn(f,0,e,a),e=0}while(0);if(!o){B=-1;break A}o=0|t[i+56>>2],u=1;break e}}while(0);Xe(4,102,68,0,0),B=-1;break A}o=A,A=0,u=0,e=0}while(0);do{if(!(32&o)){if(0|t[i+28>>2]&&0|t[i+32>>2]&&0|t[i+36>>2]&&0|t[i+40>>2]&&0|t[i+44>>2]){l=37;break}if(o=0|qf()){if(Da(o,0|t[i+24>>2],4),Q=0==(0|UB[7&t[24+(0|t[i+8>>2])>>2]](B,f,o,0|t[i+16>>2],a,0|t[i+60>>2])),Pf(o),Q){B=-1;break A}break}Xe(4,102,65,0,0),B=-1;break A}l=37}while(0);if(37==(0|l)&&0==(0|MB[31&t[20+(0|t[i+8>>2])>>2]](B,f,i,a))){B=-1;break}if(u&&0==(0|sn(B,e,A,a))){B=-1;break}if(5==(0|n)){if(!(0|cn(f,0|t[i+16>>2],B))){B=-1;break}B=(Q=(0|ca(B,f))>0)?f:B}B=0|Ba(B,r,E)}else B=-1}}while(0);return nB(a),Q=B,rB(a),pn(c,E,82796,0),0|Q},function(A,e,r,i,n){A|=0,e|=0,r|=0,i|=0,n|=0;var f,a,B,o=0,u=0,l=0,c=0,E=0,Q=0;if(!(B=0|eB()))return r=0,c=0,E=-1,rB(B),pn(r,c,82796,0),0|E;iB(B),f=0|tB(B),a=0|tB(B),c=0|Ln(E=(7+(0|Kf(0|t[i+16>>2]))|0)/8|0,82796,0);A:do{if(0==(0|f)|0==(0|a)|0==(0|c))Xe(4,101,65,0,0),A=-1;else{if((0|E)<(0|A)){Xe(4,101,108,0,0),A=-1;break}if(0|fa(e,A,f)){if((0|la(f,0|t[i+16>>2]))>-1){Xe(4,101,132,0,0),A=-1;break}A=0|t[i+56>>2];e:do{if(!(128&A)){R(0|t[i+84>>2]),(A=0|t[i+76>>2])?l=13:(A=0|hr(i,B),t[i+76>>2]=A,A?l=13:y(0|t[i+84>>2]));r:do{if(13==(0|l)){do{if(!(0|Cn(A))){if(A=0|t[i+80>>2])y(0|t[i+84>>2]);else if(A=0|hr(i,B),t[i+80>>2]=A,y(0|t[i+84>>2]),!A)break r;if(e=0|tB(B)){dn(A),u=0|kn(f,e,A,B),In(A),o=A,A=u;break}Xe(4,101,65,0,0),A=-1;break A}y(0|t[i+84>>2]),o=A,A=0|kn(f,0,A,B),e=0}while(0);if(!A){A=-1;break A}A=0|t[i+56>>2],u=1;break e}}while(0);Xe(4,101,68,0,0),A=-1;break A}o=0,u=0,e=0}while(0);do{if(32&A)l=30;else{if(0|t[i+28>>2]&&0|t[i+32>>2]&&0|t[i+36>>2]&&0|t[i+40>>2]&&0|t[i+44>>2]){l=30;break}if(!(A=0|qf())){Xe(4,101,65,0,0),A=-1;break A}if(Da(A,0|t[i+24>>2],4),2&t[i+56>>2]|0&&0==(0|pa(i+60|0,0|t[i+84>>2],0|t[i+16>>2],B))){Pf(A),A=-1;break A}if(Q=0==(0|UB[7&t[24+(0|t[i+8>>2])>>2]](a,f,A,0|t[i+16>>2],B,0|t[i+60>>2])),Pf(A),Q){A=-1;break A}}}while(0);if(30==(0|l)&&0==(0|MB[31&t[20+(0|t[i+8>>2])>>2]](a,f,i,B))){A=-1;break}if(u&&0==(0|sn(a,e,o,B))){A=-1;break}switch(A=0|Ba(a,c,E),0|n){case 1:A=0|Wr(r,E,c,A,E);break;case 4:A=0|tr(r,E,c,A,E,0,0);break;case 2:A=0|cr(r,E,c,A,E);break;case 3:If(0|r,0|c,0|A);break;default:Xe(4,101,118,0,0),A=-1;break A}Xe(4,101,114,0,0),Pe(A>>>31^1)}else A=-1}}while(0);return nB(B),Q=A,rB(B),pn(c,E,82796,0),0|Q},function(A,e,r,i,n){A|=0,r|=0,i|=0,n|=0;var f=0;if(912==(0|zr(0|t[(e|=0)>>2]))){if(f=0|S(52600,0|t[e+4>>2])){if(e=0!=(0|(n=0|t[f+4>>2]))&&911==(0|zr(0|t[n>>2]))?0|S(52132,0|t[n+4>>2]):0,(0|Ce(A,80479))<1)return Er(f),RA(e),0|(r=0);if(!(0|Ie(A,i,128)))return Er(f),RA(e),0|(r=0);if((0|Ce(A,54924))<1)return Er(f),RA(e),0|(r=0);if(n=0|t[f>>2]){if((0|_(A,0|t[n>>2]))<1)return Er(f),RA(e),0|(r=0)}else if((0|Ce(A,54941))<1)return Er(f),RA(e),0|(r=0);if((0|Ce(A,80479))<1)return Er(f),RA(e),0|(r=0);if(!(0|Ie(A,i,128)))return Er(f),RA(e),0|(r=0);if((0|Ce(A,54956))<1)return Er(f),RA(e),0|(r=0);n=0|t[f+4>>2];do{if(n){if((0|_(A,0|t[n>>2]))<1)return Er(f),RA(e),0|(r=0);if((0|Ce(A,54973))<1)return Er(f),RA(e),0|(r=0);if(e){if((0|_(A,0|t[e>>2]))>=1)break;return Er(f),RA(e),0|(r=0)}if((0|Ce(A,54980))>=1)break;return Er(f),RA(0),0|(r=0)}if((0|Ce(A,54988))<1)return Er(f),RA(e),0|(r=0)}while(0);if(Ce(A,80479),!(0|Ie(A,i,128)))return Er(f),RA(e),0|(r=0);if((0|Ce(A,55013))<1)return Er(f),RA(e),0|(r=0);if(n=0|t[f+8>>2]){if((0|_A(A,n))<1)return Er(f),RA(e),0|(r=0)}else if((0|Ce(A,55029))<1)return Er(f),RA(e),0|(r=0);if(Ce(A,80479),!(0|Ie(A,i,128)))return Er(f),RA(e),0|(r=0);if((0|Ce(A,55042))<1)return Er(f),RA(e),0|(r=0);n=0|t[f+12>>2];do{if(n){if((0|_A(A,n))>=1)break;return Er(f),RA(e),0|(r=0)}if((0|Ce(A,55060))>=1)break;return Er(f),RA(e),0|(r=0)}while(0);Ce(A,80479),Er(f),RA(e)}else if(f=(0|Ce(A,54897))<1,Er(0),RA(0),f)return 0|(r=0);if(!r)return 0|(r=1)}else if(!r)return(0|Ce(A,80479))>0|0;return 0|(r=0|fi(A,r,i))},function(A,e,r,n,f){e|=0,r|=0,n|=0,f|=0;var a,B,o,u=0;o=G,G=G+16|0,a=0|t[(A|=0)+20>>2],B=0|t[24+(0|t[A+8>>2])>>2],u=0|t[a+20>>2];A:do{if(u){if((0|pi(u))!=(0|f))return Xe(4,142,143,0,0),G=o,0|(r=-1);if(95==(0|Si(0|t[a+20>>2]))){if(1==(0|t[a+16>>2])){if((0|(u=0|yr(0,n,f,e,o,B)))>=1){u=0|t[o>>2];break}return G=o,0|u}return G=o,0|(r=-1)}switch(0|t[a+16>>2]){case 5:if((0|Zi(0|t[A+8>>2]))>>>0<(f+1|0)>>>0)return Xe(4,142,120,0,0),G=o,0|(r=-1);if(!(u=0|t[a+32>>2])&&(u=0|Ln(0|Zi(0|t[A+8>>2]),82796,0),t[a+32>>2]=u,!u))return Xe(4,142,65,0,0),G=o,0|(r=-1);If(0|u,0|n,0|f),u=255&(0|$e(0|Si(0|t[a+20>>2]))),i[(0|t[a+32>>2])+f>>0]=u,u=0|gr(f+1|0,0|t[a+32>>2],e,B,5);break A;case 1:if((0|(u=0|ar(0|Si(0|t[a+20>>2]),n,f,e,o,B)))>=1){u=0|t[o>>2];break A}return G=o,0|u;case 6:if(!(u=0|t[a+32>>2])&&(u=0|Ln(0|Zi(0|t[A+8>>2]),82796,0),t[a+32>>2]=u,!u))return G=o,0|(r=-1);if(0|Gr(B,u,n,0|t[a+20>>2],0|t[a+24>>2],0|t[a+28>>2])){u=0|gr(u=0|dr(B),0|t[a+32>>2],e,B,3);break A}return G=o,0|(r=-1);default:return G=o,0|(r=-1)}}else u=0|gr(f,n,e,B,0|t[a+16>>2])}while(0);return(0|u)<0?(G=o,0|(r=u)):(t[r>>2]=u,G=o,0|(r=1))},function(A,e,r,i,n){e|=0,r|=0,i|=0,n|=0;var f,a,B,o,u=0;o=G,G=G+16|0,B=0|t[(A|=0)+20>>2],f=0|t[A+8>>2],a=0|t[f+24>>2],u=0|t[B+20>>2];A:do{if(u){if(1==(0|t[B+16>>2]))return i=0|ur(0|Si(u),i,n,e,r,a),G=o,0|i;if((0|pi(u))!=(0|n))return Xe(4,149,143,0,0),G=o,0|(i=-1);switch(0|t[B+16>>2]){case 5:if((0|Yr(A,0,o,e,r))<1)return G=o,0|(i=0);u=0|t[o>>2];break A;case 6:return(u=0|t[B+32>>2])||(u=0|Ln(0|Zi(0|t[A+8>>2]),82796,0),t[B+32>>2]=u,u)?(0|Mr(r,e,u,a,3))<1?(G=o,0|(i=0)):(i=(0|Ur(a,i,0|t[B+20>>2],0|t[B+24>>2],0|t[B+32>>2],0|t[B+28>>2]))>0&1,G=o,0|i):(G=o,0|(i=-1));default:return G=o,0|(i=-1)}}else{if(!(u=0|t[B+32>>2])&&(u=0|Ln(0|Zi(f),82796,0),t[B+32>>2]=u,!u))return G=o,0|(i=-1);if(u=0|Mr(r,e,u,a,0|t[B+16>>2]),t[o>>2]=u,!u)return G=o,0|(i=0)}}while(0);return(0|u)!=(0|n)?(G=o,0|(i=0)):(i=0==(0|Ht(i,0|t[B+32>>2],n))&1,G=o,0|i)},Yr,function(A,e,r,i,n){e|=0,r|=0,i|=0,n|=0;var f,a=0,B=0;f=0|t[(A|=0)+20>>2],a=0|t[f+16>>2],B=0|t[24+(0|t[A+8>>2])>>2];do{if(4==(0|a)){if(B=0|dr(B),!(a=0|t[f+32>>2])&&(a=0|Ln(0|Zi(0|t[A+8>>2]),82796,0),t[f+32>>2]=a,!a))return 0|(r=-1);if(0|ir(a,B,i,n,0|t[f+36>>2],0|t[f+40>>2],0|t[f+20>>2],0|t[f+24>>2])){a=0|Ir(B,0|t[f+32>>2],e,0|t[24+(0|t[A+8>>2])>>2],3);break}return 0|(r=-1)}a=0|Ir(n,i,e,B,a)}while(0);return(0|a)<0?0|(r=a):(t[r>>2]=a,0|(r=1))},function(A,e,r,i,n){e|=0,r|=0,i|=0,n|=0;var f,a=0;f=0|t[(A|=0)+20>>2],a=0|t[f+16>>2];do{if(4==(0|a)){if(!(a=0|t[f+32>>2])&&(a=0|Ln(0|Zi(0|t[A+8>>2]),82796,0),t[f+32>>2]=a,!a))return 0|(r=-1);if((0|(a=0|Dr(n,i,a,0|t[24+(0|t[A+8>>2])>>2],3)))<1)return 0|(r=a);a=0|fr(e,a,0|t[f+32>>2],a,a,0|t[f+36>>2],0|t[f+40>>2],0|t[f+20>>2],0|t[f+24>>2]);break}a=0|Dr(n,i,e,0|t[24+(0|t[A+8>>2])>>2],a)}while(0);return(0|a)<0?0|(r=a):(t[r>>2]=a,0|(r=1))},Zf,Zf,Zf],UB=[Nf,function(A,e,r,i,n,f){return e|=0,r|=0,0,n|=0,f|=0,(n=0|t[(A|=0)>>2])||(n=0|qf(),t[A>>2]=n,n)?0|fa(e,r,n)?0|(A=1):(n=0|t[A>>2])?(1&t[f+20>>2]?xf(n):Pf(n),t[A>>2]=0,0|(A=0)):0|(A=0):0|(A=0)},function(A,e,r,i,n,f){e|=0,r|=0,n|=0,f|=0,n=0|t[(A|=0)>>2];do{if(!n){if(n=0|$f(),t[A>>2]=n,n)break;return 0|(A=0)}}while(0);return 0|fa(e,r,n)?(ha(0|t[A>>2],4),0|(A=1)):(n=0|t[A>>2])?(1&t[f+20>>2]?xf(n):Pf(n),t[A>>2]=0,0|(A=0)):0|(A=0)},function(A,e,r,n,a,B){A|=0,e|=0,n|=0,a|=0,B|=0;var o=0,u=0;if((0|(r|=0))>1){switch(0|i[e>>0]){case-1:e=e+1|0,a=r+-1|0,n=255;break;case 0:e=e+1|0,a=r+-1|0,n=0;break;default:a=r,n=256}if((0|a)>4)return Xe(13,166,128,0,0),0|(A=0);if(256!=(0|n)){if(!(128&(n^f[e>>0])))return Xe(13,166,221,0,0),0|(A=0);o=e,r=n,u=14}else u=9}else r?(a=r,u=9):(a=0,u=10);if(9==(0|u)&&((0|i[e>>0])<0?(n=255,u=13):u=10),10==(0|u)&&(n=0,u=13),13==(0|u)&&((0|a)>0?(o=e,r=n,u=14):(a=n,e=0)),14==(0|u)){n=0,e=0;do{e=r^f[o+n>>0]|e<<8,n=n+1|0}while((0|n)!=(0|a));if((0|e)<0)return Xe(13,166,128,0,0),0|(A=0);a=r}return(0|(e^=(0!=(0|a))<<31>>31))==(0|t[B+20>>2])?(Xe(13,166,128,0,0),0|(A=0)):(i[A>>0]=e,i[A+1>>0]=e>>8,i[A+2>>0]=e>>16,i[A+3>>0]=e>>24,0|(A=1))},Pa,function(A,e,r,i,n,f){A|=0,e|=0,r|=0,n|=0,f|=0;var a,B=0,o=0;if(a=G,G=G+16|0,912!=(0|zr(0|t[(i|=0)>>2])))return Xe(4,148,155,0,0),G=a,-1;if(t[a+4>>2]=0,912!=(0|zr(0|t[i>>2])))return Xe(4,155,155,0,0),G=a,-1;r=0|S(52600,0|t[i+4>>2]);A:do{if(r){if(e=0|t[r+4>>2])if(n=911==(0|zr(0|t[e>>2]))?0|S(52132,0|t[e+4>>2]):0,e=0|t[r+4>>2]){if(911!=(0|zr(0|t[e>>2]))){Xe(4,157,153,0,0),e=-1;break}if(!n){Xe(4,157,154,0,0),n=0,e=-1;break}if(!(e=0|Xi(0|pr(0|zr(0|t[n>>2]))))){Xe(4,157,151,0,0),e=-1;break}o=e}else B=17;else n=0,B=17;if(17==(0|B)){if(!(e=52916)){e=-1;break}o=e}if(e=0|t[r>>2]){if(!(e=0|Xi(0|pr(0|zr(0|t[e>>2]))))){Xe(4,156,166,0,0),e=-1;break}}else if(!(e=52916)){e=-1;break}if(i=0|t[r+8>>2]){if((0|(i=0|CA(i)))<0){Xe(4,155,150,0,0),e=-1;break}}else i=20;if(0|(B=0|t[r+12>>2])&&1!=(0|CA(B))){Xe(4,155,139,0,0),e=-1;break}e:do{if(!f){do{if((0|Qi(0,-1,248,13,0,a))>=1){if((0|(f=0|Si(e)))==(0|Si(0|t[a>>2])))break e;Xe(4,155,158,0,0);break}}while(0);e=-1;break A}if(!(0|vi(A,a+4|0,e,0,f))){e=-1;break A}}while(0);e=(0|Qi(0|t[a+4>>2],6,-1,4097,6,0))>=1&&(0|Qi(0|t[a+4>>2],6,24,4098,i,0))>=1?(e=(0|Qi(0|t[a+4>>2],6,1016,4101,0,o))<1)?-1:2:-1}else Xe(4,155,149,0,0),n=0,r=0,e=-1}while(0);return Er(r),RA(n),G=a,0|e},function(A,e,r,i,n,f){e|=0,r|=0,i|=0,n|=0,f|=0;var a,B=0,o=0;if(a=G,G=G+32|0,(0|Qi(r=0|zi(A|=0),6,-1,4102,0,a))<1)return G=a,0|(n=0);if(6!=(0|t[a>>2]))return G=a,0|(n=2);t[a+8>>2]=0,e=0|function(A){return 0|t[(A|=0)+8>>2]}(r);do{if((0|Qi(r,-1,248,13,0,a+16|0))>=1&&(0|Qi(r,6,1016,4104,0,a+12|0))>=1&&0!=(0|Qi(r,6,24,4103,0,a+4|0))){switch(0|t[a+4>>2]){case-1:t[a+4>>2]=0|pi(0|t[a+16>>2]);break;case-2:t[a+4>>2]=(0|Zi(e))-2-(0|pi(0|t[a+16>>2])),7+(0|Gi(e))&7||(t[a+4>>2]=(0|t[a+4>>2])-1)}if(r=0|pA(52600)){if(20!=(0|t[a+4>>2])){if(e=0|rA(),t[r+8>>2]=e,!e){o=26;break}if(!(0|sA(e,0|t[a+4>>2]))){o=26;break}}if(64!=(0|Si(e=0|t[a+16>>2]))){if(f=0|YA(),t[r>>2]=f,!f){o=26;break}VA(f,e)}if(f=0|t[a+12>>2],t[a+20>>2]=0,t[r+4>>2]=0,64!=(0|Si(f))&&(64!=(0|Si(f))?(e=0|YA())?(VA(e,f),o=22):e=0:(e=0,o=22),22==(0|o)&&0!=(0|p(e,52132,a+20|0))&&(B=0|YA(),t[r+4>>2]=B,0!=(0|B))&&(yA(B,A=0|Sr(911),16,0|t[a+20>>2]),t[a+20>>2]=0),cA(0|t[a+20>>2]),RA(e),!(0|t[r+4>>2]))){o=26;break}B=0==(0|p(r,52600,a+8|0)),Er(r),r=0|t[a+8>>2],B&&(o=28)}else r=0,o=26}else r=0,o=26}while(0);if(26==(0|o)&&(Er(r),r=0|t[a+8>>2],o=28),28==(0|o)&&(cA(r),r=0),!r)return G=a,0|(n=0);do{if(0|n){if(0|(e=0|lA(r))){yA(n,0|Sr(912),16,e);break}return cA(r),G=a,0|(n=0)}}while(0);return yA(i,0|Sr(912),16,r),G=a,0|(n=3)},Nf],GB=[function(A,e,r,i,n,t,f,a){return 0,0,0,0,0,0,0,0,l(6),0}],ZB=[Ff,function(){t[20512]=1,t[20515]=0|H(82052,0)},function(){t[20522]=0|Ze(18,29),t[20523]=0|W(),t[20521]=0!=(0|t[20522])&0!=(0|t[20523])&1},function(){var A,e=0;A=G,G=G+16|0;A:do{if(0|H(A,19)){e=0|W(),t[20546]=e;do{if(e){if(0|d(5)){e=0|t[20546];break}t[13248]=t[A>>2],t[20545]=1,e=1;break A}e=0}while(0);V(e),t[20546]=0,X(A),e=0}else e=0}while(0);t[20542]=e,G=A},function(){var A,e=0;return A=G,G=G+16|0,0|function(A){return A|=0,0|Kn(262144,0,0)?0!=(0|m(82056,1))&0!=(0|t[20515])?(t[A>>2]=0|v(82052),0|0!=(0|J(82052,-1))&1):0|0:0|(A=0)}(A)?(Hr(0|function(A,e){A|=0,e|=0;var r,i,n=0;return(n=0|t[20518])||(n=52736),(n=0|t[n+36>>2])?(0|(r=0|DB[31&n](A,0,0)))<0?0|(e=0):((i=0|Ln(r,82796,0))?((n=0|t[20518])||(n=52736),(n=0|t[n+36>>2])?n=0|DB[31&n](A,i,r):(Xe(37,105,108,0,0),n=-1),n=(0|n)==(0|r)?0|vr(0,i,0,e):0):n=0,Xn(i,82796,0),0|(e=n)):(Xe(37,105,108,0,0),0|(e=0))}(82180,4)),function(A){-1!=(0|(A|=0))&&J(82052,A)}(0|t[A>>2]),e=1,t[20544]=e,void(G=A)):(e=0,t[20544]=e,void(G=A))},function(){var A,e=0;A=G,G=G+16|0,0!=(0|t[20545])&0==(0|t[20540])?(t[20540]=1,e=0|v(52992),J(52992,0),0|e&&(0|t[e>>2]&&Vr(),0|t[e+4>>2]&&ze(),Xn(e)),V(0|t[20546]),0|t[20547]&&(X(82064),X(82068)),t[A>>2]=t[13248],t[13248]=-1,X(A),ot(),function(A){A|=0;var e,r=0;ot(),e=0|Hn(0|t[20565]),r=0|t[20565];A:do{if((0|e)>0){if(0|A)for(A=e+-1|0;;){if(r=0|vn(r,A),Yn(0|t[20565],A),Hr(0|t[r>>2]),Xn(0|t[r+4>>2]),Xn(r),r=0|t[20565],!((0|A)>0))break A;A=A+-1|0}for(A=e+-1|0;r=0|vn(r,A),(0|t[r+16>>2])<=0&&0|t[r>>2]&&(Yn(0|t[20565],A),Hr(0|t[r>>2]),Xn(0|t[r+4>>2]),Xn(r)),r=0|t[20565],(0|A)>0;)A=A+-1|0}}while(0);0|Hn(r)||(Fn(0|t[20565]),t[20565]=0)}(1),mn(0|t[20492],22),t[20492]=0,mn(0|t[20493],22),t[20493]=0,mn(0|t[20494],22),t[20494]=0,mn(0|t[20495],22),t[20495]=0,mn(0|t[20496],22),t[20496]=0,mn(0|t[20497],22),t[20497]=0,mn(0|t[20498],22),t[20498]=0,mn(0|t[20499],22),t[20499]=0,mn(0|t[20500],22),t[20500]=0,mn(0|t[20501],22),t[20501]=0,mn(0|t[20502],22),t[20502]=0,mn(0|t[20503],22),t[20503]=0,mn(0|t[20504],22),t[20504]=0,mn(0|t[20505],22),t[20505]=0,V(0|t[20568]),t[20568]=0,V(0),Ti(),function(){var A;(A=0|t[20526])&&(ve(A,0),Ve(0|t[20526],14),Ve(0|t[20526],15),Ve(0|t[20526],16),Fe(0|t[20526]),t[20526]=0)}(),0|t[20512]&&X(82052),V(0),t[20545]=0,G=A):G=A},function(){t[20549]=1},function(){t[20550]=1},function(){t[20552]=1},function(){t[20553]=1},function(){t[20555]=1},function(){t[20567]=1,t[20557]=1},function(){!function(A){if(A|=0,0|t[20567])return;(function(){(function(){ut(54519,25,10)})(),function(){ut(53924,22,7)}(),function(){ut(80592,32,0)}(),function(){ut(80626,33,20)}()})(),Se(),function(A,e,r){A|=0,e|=0,r|=0;var i,n=0,t=0,f=0;i=0|it(0);do{if(i){if(A)n=A;else if(!(n=0|Bt())){t=0,n=0;break}if((0|tt(i,n,0))>=1){t=n,n=0|ft(i,e,r),f=9;break}0!=(16&r|0)&&114==(4095&(0|pe())|0)?(Se(),t=n,n=1,f=9):(t=n,n=0,f=9)}else t=0,n=0,f=9}while(0);if(9==(0|f)&&A)return f=n,nt(i),0|f;Xn(t),f=n,nt(i)}(0,A,48),t[20567]=1}(0|t[20558]),t[20559]=1},function(){var A=0;0|function(){var A=0;return 0|H(82064,0)?0|H(82068,0)?A=1:(X(82064),A=0):A=0,0|A}()?(t[20547]=1,A=1):A=0,t[20561]=A},function(){var A=0;if(!(0|Kn(0,0,0)))return A=0,void(t[20570]=A);A=0|W(),t[20568]=A,A=0!=(0|A)&1,t[20570]=A},Ff],NB=[Wf,function(A){A=0|t[(A|=0)+20>>2],pn(0|t[A+4>>2],0|t[A+8>>2],82796,0),pn(0|t[A+12>>2],0|t[A+16>>2],82796,0),$r(A+20|0,0|t[A+1044>>2]),Xn(A,82796,0)},function(A){A=0|t[(A|=0)+20>>2],pn(0|t[A+4>>2],0|t[A+8>>2],82796,0),$r(A+12|0,0|t[A+1036>>2]),Xn(A,82796,0)},function(A){var e;(e=0|si(A|=0))&&(le(0|t[e+20>>2]),pn(0|t[e+12>>2],0|t[e+4>>2],82796,0),Xn(e,82796,0),ki(A,0))},function(A){var e;(e=0|Ri(A|=0))&&(0|(A=0|t[e+8>>2])&&$r(A,0|t[e>>2]),eA(e))},function(A){sr(0|t[(A|=0)+24>>2])},function(A){(A=0|t[(A|=0)+20>>2])&&(Pf(0|t[A+4>>2]),Xn(0|t[A+32>>2],82796,0),Xn(0|t[A+36>>2],82796,0),Xn(A,82796,0))},function(A){!function(){var A;if(!(A=0|t[20509]))return;t[20509]=0,mn(A,9)}()},function(A){(A|=0)&&(Xn(0|t[A+4>>2],82796,0),Xn(0|t[A+8>>2],82796,0),Xn(0|t[A>>2],82796,0),Xn(A,82796,0))},function(A){1&t[(A|=0)+16>>2]&&Xn(A,82796,0)},function(A){0},function(A){Xn(A|=0,82796,0)},function(A){var e,r=0;if(A|=0){if(e=0|t[20525],r=0|t[A>>2],(0|e)>=0){if((0|e)!=(0|r))return;r=e}!function(A,e){var r;A|=0,e|=0,r=G,G=G+16|0,0==(0|m(82080,2))|0==(0|t[20521])?G=r:(R(0|t[20523]),t[r+8>>2]=A,t[r>>2]=-32769&e,(A=0|Re(0|t[20522],r))?(Xn(A,82796,0),A=1):A=0,y(0|t[20523]),G=r)}(0|t[A+8>>2],r)}},function(A){Xn(A|=0,82796,0)},function(A){A=0|t[(A|=0)+4>>2],t[A+8>>2]=0,t[A+20>>2]=13|t[A+20>>2]},function(A){A=8+(0|t[(A|=0)+4>>2])|0,t[A>>2]=1+(0|t[A>>2])},function(A){var e,r;e=0|t[(A|=0)+4>>2],r=(0|t[e+8>>2])-1|0,t[e+8>>2]=r,0|r||$(e),Xn(A,82796,0)},function(A){Xn(A|=0,82796,0)},function(A){z(A|=0,52788)},function(A){(A|=0)&&(0|t[A>>2]&&Vr(),0|t[A+4>>2]&&ze(),Xn(A))},jn,function(A){var e,r=0,i=0,n=0;if(!(0|t[(A|=0)+4>>2])){if((0|(r=0|Hn(e=0|t[A+8>>2])))>0)do{i=r,n=0|vn(e,r=r+-1|0),Xn(0|t[n+8>>2]),Xn(0|t[n+4>>2]),Xn(n)}while((0|i)>1);Fn(e),Xn(0|t[A>>2]),Xn(A)}},function(A){Xn(A|=0)},Wf,Wf,Wf,Wf,Wf,Wf,Wf,Wf,Wf],FB=[Yf,function(A,e){var r;e|=0,(r=0|t[(A|=0)>>2])&&(1&t[e+20>>2]?xf(r):Pf(r),t[A>>2]=0)},function(A,e){e|=0,t[(A|=0)>>2]=t[e+20>>2]},function(A,e){e|=0,0|t[(A|=0)+4>>2]&&Re(e,A)},function(A,e){var r,i,n;return e|=0,n=G,G=G+32|0,r=0|t[(A|=0)+4>>2],i=0|t[A>>2],r?(A=0|t[A+8>>2],t[n>>2]=i,t[n+4>>2]=r,t[n+8>>2]=A,ge(e,80830,n),void(G=n)):(t[n+16>>2]=i,ge(e,80842,n+16|0),void(G=n))},Yf,Yf,Yf],WB=[function(A,e,r){l(10)},Xn],YB=[function(A,e,r,i,n,t){l(11)}];return{___emscripten_environ_constructor:function(){Q(82288)},___errno_location:Ft,___muldi3:lf,___udivdi3:bf,___uremdi3:kf,__get_environ:function(){return 82288},_bitshift64Lshr:sf,_bitshift64Shl:Cf,_emscripten_get_sbrk_ptr:df,_free:af,_i64Add:cf,_i64Subtract:Ef,_malloc:ff,_memalign:function(A,e){return e|=0,(A|=0)>>>0<9?0|(e=0|ff(e)):0|(e=0|function(A,e){e|=0;var r=0,i=0,n=0,f=0;if((r=(A=A|0)>>>0>16?A:16)+-1&r)for(A=16;A>>>0<r>>>0;)A<<=1;else A=r;if((-64-A|0)>>>0<=e>>>0)return t[20571]=48,0|(f=0);if(!(n=0|ff((f=e>>>0<11?16:e+11&-8)+12+A|0)))return 0|(f=0);do{if(A+-1&n){if(r=(e=((e=(n+A+-1&0-A)-8|0)-(n+-8)|0)>>>0>15?e:e+A|0)-(n+-8)|0,3&(A=0|t[n+-4>>2])){t[e+4>>2]=(-8&A)-r|1&t[e+4>>2]|2,t[e+((-8&A)-r)+4>>2]=1|t[e+((-8&A)-r)+4>>2],t[n+-4>>2]=r|1&t[n+-4>>2]|2,t[e+4>>2]=1|t[e+4>>2],of(n+-8|0,r),i=e,A=e;break}t[e>>2]=(0|t[n+-8>>2])+r,t[e+4>>2]=(-8&A)-r,i=e,A=e;break}i=n+-8|0,A=n+-8|0}while(0);(3&(A=0|t[(r=A+4|0)>>2])|0?(-8&A)>>>0>(f+16|0)>>>0:0)&&(n=i+f|0,t[r>>2]=f|1&A|2,t[n+4>>2]=(-8&A)-f|3,t[(e=i+(-8&A)+4|0)>>2]=1|t[e>>2],of(n,(-8&A)-f|0));return 0|(f=i+8|0)}(A,e))},_memcpy:If,_memmove:gf,_memset:Df,_rsasignjs_init:function(){Z()},_rsasignjs_keypair:function(A,e){var r;return A|=0,e|=0,r=G,G=G+16|0,t[r+4>>2]=A,t[r>>2]=e,e=0|qf(),A=0|kr(),cB(e,65537),1==(0|er(A,2048,e,0))?(ai(A,r+4|0),wr(A,r),sr(A),Pf(e),e=0):e=1,G=r,0|e},_rsasignjs_public_key_bytes:function(){return 450},_rsasignjs_secret_key_bytes:function(){return 1700},_rsasignjs_sign:function(A,e,r,i,n){var f;return A|=0,e|=0,r|=0,i|=0,n|=0,f=G,G=G+160|0,t[f+144>>2]=i,t[f+148>>2]=0|kr(),0|Qr(f+148|0,f+144|0,n)?(dB(f+32|0),Nn(f+32|0,e,r),Zn(f,f+32|0),e=0|ar(672,f,32,A,0,0|t[f+148>>2]),sr(0|t[f+148>>2])):e=-1,G=f,0|e},_rsasignjs_signature_bytes:function(){return 256},_rsasignjs_verify:function(A,e,r,i,n){var f;return A|=0,e|=0,r|=0,i|=0,n|=0,f=G,G=G+160|0,t[f+144>>2]=i,t[f+148>>2]=0|kr(),0|br(f+148|0,f+144|0,n)?(dB(f+32|0),Nn(f+32|0,e,r),Zn(f,f+32|0),e=0|ur(672,f,32,A,256,0|t[f+148>>2]),sr(0|t[f+148>>2])):e=-1,G=f,0|e},dynCall_ii:function(A,e){return e|=0,0|IB[31&(A|=0)](0|e)},dynCall_iii:function(A,e,r){return e|=0,r|=0,0|gB[63&(A|=0)](0|e,0|r)},dynCall_iiii:function(A,e,r,i){return e|=0,r|=0,i|=0,0|DB[31&(A|=0)](0|e,0|r,0|i)},dynCall_iiiii:function(A,e,r,i,n){return e|=0,r|=0,i|=0,n|=0,0|MB[31&(A|=0)](0|e,0|r,0|i,0|n)},dynCall_iiiiii:function(A,e,r,i,n,t){return e|=0,r|=0,i|=0,n|=0,t|=0,0|hB[15&(A|=0)](0|e,0|r,0|i,0|n,0|t)},dynCall_iiiiiii:function(A,e,r,i,n,t,f){return e|=0,r|=0,i|=0,n|=0,t|=0,f|=0,0|UB[7&(A|=0)](0|e,0|r,0|i,0|n,0|t,0|f)},dynCall_iiiiiiiii:function(A,e,r,i,n,t,f,a,B){return e|=0,r|=0,i|=0,n|=0,t|=0,f|=0,a|=0,B|=0,0|GB[0&(A|=0)](0|e,0|r,0|i,0|n,0|t,0|f,0|a,0|B)},dynCall_v:function(A){ZB[15&(A|=0)]()},dynCall_vi:function(A,e){e|=0,NB[31&(A|=0)](0|e)},dynCall_vii:function(A,e,r){e|=0,r|=0,FB[7&(A|=0)](0|e,0|r)},dynCall_viii:function(A,e,r,i){e|=0,r|=0,i|=0,WB[1&(A|=0)](0|e,0|r,0|i)},dynCall_viiiiii:function(A,e,r,i,n,t,f){e|=0,r|=0,i|=0,n|=0,t|=0,f|=0,YB[0&(A|=0)](0|e,0|r,0|i,0|n,0|t,0|f)},stackAlloc:function(A){var e;return e=G,G=(G=G+(A|=0)|0)+15&-16,0|e},stackRestore:function(A){G=A|=0},stackSave:function(){return 0|G}}}(asmGlobalArg,asmLibraryArg,buffer),___emscripten_environ_constructor=Module.___emscripten_environ_constructor=asm.___emscripten_environ_constructor,___errno_location=Module.___errno_location=asm.___errno_location,___muldi3=Module.___muldi3=asm.___muldi3,___udivdi3=Module.___udivdi3=asm.___udivdi3,___uremdi3=Module.___uremdi3=asm.___uremdi3,__get_environ=Module.__get_environ=asm.__get_environ,_bitshift64Lshr=Module._bitshift64Lshr=asm._bitshift64Lshr,_bitshift64Shl=Module._bitshift64Shl=asm._bitshift64Shl,_emscripten_get_sbrk_ptr=Module._emscripten_get_sbrk_ptr=asm._emscripten_get_sbrk_ptr,_free=Module._free=asm._free,_i64Add=Module._i64Add=asm._i64Add,_i64Subtract=Module._i64Subtract=asm._i64Subtract,_malloc=Module._malloc=asm._malloc,_memalign=Module._memalign=asm._memalign,_memcpy=Module._memcpy=asm._memcpy,_memmove=Module._memmove=asm._memmove,_memset=Module._memset=asm._memset,_rsasignjs_init=Module._rsasignjs_init=asm._rsasignjs_init,_rsasignjs_keypair=Module._rsasignjs_keypair=asm._rsasignjs_keypair,_rsasignjs_public_key_bytes=Module._rsasignjs_public_key_bytes=asm._rsasignjs_public_key_bytes,_rsasignjs_secret_key_bytes=Module._rsasignjs_secret_key_bytes=asm._rsasignjs_secret_key_bytes,_rsasignjs_sign=Module._rsasignjs_sign=asm._rsasignjs_sign,_rsasignjs_signature_bytes=Module._rsasignjs_signature_bytes=asm._rsasignjs_signature_bytes,_rsasignjs_verify=Module._rsasignjs_verify=asm._rsasignjs_verify,stackAlloc=Module.stackAlloc=asm.stackAlloc,stackRestore=Module.stackRestore=asm.stackRestore,stackSave=Module.stackSave=asm.stackSave,dynCall_ii=Module.dynCall_ii=asm.dynCall_ii,dynCall_iii=Module.dynCall_iii=asm.dynCall_iii,dynCall_iiii=Module.dynCall_iiii=asm.dynCall_iiii,dynCall_iiiii=Module.dynCall_iiiii=asm.dynCall_iiiii,dynCall_iiiiii=Module.dynCall_iiiiii=asm.dynCall_iiiiii,dynCall_iiiiiii=Module.dynCall_iiiiiii=asm.dynCall_iiiiiii,dynCall_iiiiiiiii=Module.dynCall_iiiiiiiii=asm.dynCall_iiiiiiiii,dynCall_v=Module.dynCall_v=asm.dynCall_v,dynCall_vi=Module.dynCall_vi=asm.dynCall_vi,dynCall_vii=Module.dynCall_vii=asm.dynCall_vii,dynCall_viii=Module.dynCall_viii=asm.dynCall_viii,dynCall_viiiiii=Module.dynCall_viiiiii=asm.dynCall_viiiiii,calledRun;if(Module.asm=asm,Module.writeArrayToMemory=writeArrayToMemory,memoryInitializer)if(isDataURI(memoryInitializer)||(memoryInitializer=locateFile(memoryInitializer)),ENVIRONMENT_IS_NODE||ENVIRONMENT_IS_SHELL){var data=readBinary(memoryInitializer);HEAPU8.set(data,GLOBAL_BASE)}else{addRunDependency("memory initializer");var applyMemoryInitializer=function(A){A.byteLength&&(A=new Uint8Array(A)),HEAPU8.set(A,GLOBAL_BASE),Module.memoryInitializerRequest&&delete Module.memoryInitializerRequest.response,removeRunDependency("memory initializer")},doBrowserLoad=function(){readAsync(memoryInitializer,applyMemoryInitializer,(function(){throw new Error("could not load memory initializer "+memoryInitializer)}))},memoryInitializerBytes=tryParseAsDataURI(memoryInitializer);if(memoryInitializerBytes)applyMemoryInitializer(memoryInitializerBytes.buffer);else if(Module.memoryInitializerRequest){var useRequest=function(){var A=Module.memoryInitializerRequest,e=A.response;if(200!==A.status&&0!==A.status){var r=tryParseAsDataURI(Module.memoryInitializerRequestURL);if(!r)return console.warn("a problem seems to have happened with Module.memoryInitializerRequest, status: "+A.status+", retrying "+memoryInitializer),void doBrowserLoad();e=r.buffer}applyMemoryInitializer(e)};Module.memoryInitializerRequest.response?setTimeout(useRequest,0):Module.memoryInitializerRequest.addEventListener("load",useRequest)}else doBrowserLoad()}function ExitStatus(A){this.name="ExitStatus",this.message="Program terminated with exit("+A+")",this.status=A}function run(A){function e(){calledRun||(calledRun=!0,Module.calledRun=!0,ABORT||(initRuntime(),preMain(),Module.onRuntimeInitialized&&Module.onRuntimeInitialized(),postRun()))}A=A||arguments_,runDependencies>0||(preRun(),runDependencies>0||(Module.setStatus?(Module.setStatus("Running..."),setTimeout((function(){setTimeout((function(){Module.setStatus("")}),1),e()}),1)):e()))}if(dependenciesFulfilled=function A(){calledRun||run(),calledRun||(dependenciesFulfilled=A)},Module.run=run,Module.preInit)for("function"==typeof Module.preInit&&(Module.preInit=[Module.preInit]);Module.preInit.length>0;)Module.preInit.pop()();noExitRuntime=!0,run()}function dataResult(A,e){return new Uint8Array(new Uint8Array(Module.HEAPU8.buffer,A,e))}function dataFree(A){try{Module._free(A)}catch(A){setTimeout((function(){throw A}),0)}}function importJWK(A,e){return Promise.resolve().then((function(){var r=A.indexOf(0),i=JSON.parse(sodiumUtil.to_string(r>-1?new Uint8Array(A.buffer,A.byteOffset,r):A));return Promise.resolve().then((function(){return crypto.subtle.importKey("jwk",i,algorithm,!1,[e])})).catch((function(){return pemJwk.jwk2pem(i)}))}))}function exportJWK(A,e){return Promise.resolve().then((function(){return"string"==typeof A?pemJwk.pem2jwk(A):crypto.subtle.exportKey("jwk",A,algorithm.name)})).then((function(A){var r=sodiumUtil.from_string(JSON.stringify(A)),i=new Uint8Array(e);return i.set(r),sodiumUtil.memzero(r),i}))}function exportKeyPair(A){return Promise.all([exportJWK(A.publicKey,rsaSign.publicKeyBytes),exportJWK(A.privateKey,rsaSign.privateKeyBytes)]).then((function(A){return{publicKey:A[0],privateKey:A[1]}}))}[Float32Array,Float64Array,Int8Array,Int16Array,Int32Array,Uint8Array,Uint16Array,Uint32Array,Uint8ClampedArray].forEach((function(A){A.prototype.indexOf||(A.prototype.indexOf=function(A){for(var e=0;e<this.length;++e)if(this[e]===A)return e;return-1})}));var algorithm=isNode?"RSA-SHA256":{name:"RSASSA-PKCS1-v1_5",hash:{name:"SHA-256"},modulusLength:2048,publicExponent:new Uint8Array([1,0,1])},rsaSign={publicKeyBytes:450,privateKeyBytes:1700,bytes:256,keyPair:function(){return Promise.resolve().then((function(){if(isNode){var A=generateRSAKeypair();return{publicKey:A.public,privateKey:A.private}}return crypto.subtle.generateKey(algorithm,!0,["sign","verify"])})).then(exportKeyPair).catch((function(){return initiated.then((function(){var A=Module._malloc(rsaSign.publicKeyBytes),e=Module._malloc(rsaSign.privateKeyBytes);try{var r=Module._rsasignjs_keypair(A,e);if(0!==r)throw new Error("RSA Sign error: keyPair failed ("+r+")");return exportKeyPair({publicKey:"-----BEGIN PUBLIC KEY-----\n"+sodiumUtil.to_base64(dataResult(A,rsaSign.publicKeyBytes))+"\n-----END PUBLIC KEY-----",privateKey:"-----BEGIN RSA PRIVATE KEY-----\n"+sodiumUtil.to_base64(dataResult(e,rsaSign.privateKeyBytes))+"\n-----END RSA PRIVATE KEY-----"})}finally{dataFree(A,rsaSign.publicKeyBytes),dataFree(e,rsaSign.privateKeyBytes)}}))}))},sign:function(A,e){return rsaSign.signDetached(A,e).then((function(e){var r=new Uint8Array(rsaSign.bytes+A.length);return r.set(e),r.set(A,rsaSign.bytes),r}))},signDetached:function(A,e){return importJWK(e,"sign").then((function(e){return Promise.resolve().then((function(){if(isNode){var r=Buffer.from(A),i=nodeCrypto.createSign(algorithm);i.write(r),i.end();var n=i.sign(e);return sodiumUtil.memzero(r),n}return crypto.subtle.sign(algorithm,e,A)})).catch((function(){return initiated.then((function(){e=sodiumUtil.from_base64(e.split("-----")[2]);var r=Module._malloc(rsaSign.bytes),i=Module._malloc(A.length),n=Module._malloc(e.length);Module.writeArrayToMemory(A,i),Module.writeArrayToMemory(e,n);try{var t=Module._rsasignjs_sign(r,i,A.length,n,e.length);if(1!==t)throw new Error("RSA Sign error: sign failed ("+t+")");return dataResult(r,rsaSign.bytes)}finally{dataFree(r),dataFree(i),dataFree(n)}}))})).then((function(A){return sodiumUtil.memzero(e),new Uint8Array(A)}))}))},open:function(A,e){return Promise.resolve().then((function(){var r=new Uint8Array(A.buffer,A.byteOffset,rsaSign.bytes),i=new Uint8Array(A.buffer,A.byteOffset+rsaSign.bytes);return rsaSign.verifyDetached(r,i,e).then((function(A){if(A)return i;throw new Error("Failed to open RSA signed message.")}))}))},verifyDetached:function(A,e,r){return importJWK(r,"verify").then((function(r){return Promise.resolve().then((function(){if(isNode){var i=nodeCrypto.createVerify(algorithm);return i.update(Buffer.from(e)),i.verify(r,A)}return crypto.subtle.verify(algorithm,r,A,e)})).catch((function(){return initiated.then((function(){r=sodiumUtil.from_base64(r.split("-----")[2]);var i=Module._malloc(rsaSign.bytes),n=Module._malloc(e.length),t=Module._malloc(r.length);Module.writeArrayToMemory(A,i),Module.writeArrayToMemory(e,n),Module.writeArrayToMemory(r,t);try{return 1===Module._rsasignjs_verify(i,n,e.length,t,r.length)}finally{dataFree(i),dataFree(n),dataFree(t)}}))})).then((function(A){return sodiumUtil.memzero(r),A}))}))}};rsaSign.rsaSign=rsaSign,module.exports=rsaSign;


/***/ }),
/* 5 */
/***/ (function(module, __unused_webpack_exports, __webpack_require__) {

var asn = __webpack_require__(6)
var one = new asn.bignum(1)

function urlize(base64) {
  return base64.replace(/\+/g, '-')
    .replace(/\//g, '_')
    .replace(/=/g, '')
}

function hex2b64url(str) {
  return urlize(Buffer(str, 'hex').toString('base64'))
}

function fromPEM(data) {
  var text = data.toString().split(/(\r\n|\r|\n)+/g);
  text = text.filter(function(line) {
    return line.trim().length !== 0;
  });
  text = text.slice(1, -1).join('');
  return new Buffer(text.replace(/[^\w\d\+\/=]+/g, ''), 'base64');
}

var RSAPublicKey = asn.define('RSAPublicKey', function () {
  this.seq().obj(
    this.key('n').int(),
    this.key('e').int()
  )
})

var AlgorithmIdentifier = asn.define('AlgorithmIdentifier', function () {
  this.seq().obj(
    this.key('algorithm').objid(),
    this.key('parameters').optional().any()
  )
})

var PublicKeyInfo = asn.define('PublicKeyInfo', function () {
  this.seq().obj(
    this.key('algorithm').use(AlgorithmIdentifier),
    this.key('publicKey').bitstr()
  )
})

var Version = asn.define('Version', function () {
  this.int({
    0: 'two-prime',
    1: 'multi'
  })
})

var OtherPrimeInfos = asn.define('OtherPrimeInfos', function () {
  this.seq().obj(
    this.key('ri').int(),
    this.key('di').int(),
    this.key('ti').int()
  )
})

var RSAPrivateKey = asn.define('RSAPrivateKey', function () {
  this.seq().obj(
    this.key('version').use(Version),
    this.key('n').int(),
    this.key('e').int(),
    this.key('d').int(),
    this.key('p').int(),
    this.key('q').int(),
    this.key('dp').int(),
    this.key('dq').int(),
    this.key('qi').int(),
    this.key('other').optional().use(OtherPrimeInfos)
  )
})

var PrivateKeyInfo = asn.define('PrivateKeyInfo', function () {
  this.seq().obj(
    this.key('version').use(Version),
    this.key('algorithm').use(AlgorithmIdentifier),
    this.key('privateKey').bitstr()
  )
})

function addExtras(obj, extras) {
  extras = extras || {}
  Object.keys(extras).forEach(
    function (key) {
      obj[key] = extras[key]
    }
  )
  return obj
}

function pad(hex) {
  return (hex.length % 2 === 1) ? '0' + hex : hex
}

function decodeRsaPublic(buffer, extras) {
  var key = RSAPublicKey.decode(buffer, 'der')
  var e = pad(key.e.toString(16))
  var jwk = {
    kty: 'RSA',
    n: bn2base64url(key.n),
    e: hex2b64url(e)
  }
  return addExtras(jwk, extras)
}

function decodeRsaPrivate(buffer, extras) {
  var key = RSAPrivateKey.decode(buffer, 'der')
  var e = pad(key.e.toString(16))
  var jwk = {
    kty: 'RSA',
    n: bn2base64url(key.n),
    e: hex2b64url(e),
    d: bn2base64url(key.d),
    p: bn2base64url(key.p),
    q: bn2base64url(key.q),
    dp: bn2base64url(key.dp),
    dq: bn2base64url(key.dq),
    qi: bn2base64url(key.qi)
  }
  return addExtras(jwk, extras)
}

function decodePublic(buffer, extras) {
  var info = PublicKeyInfo.decode(buffer, 'der')
  return decodeRsaPublic(info.publicKey.data, extras)
}

function decodePrivate(buffer, extras) {
  var info = PrivateKeyInfo.decode(buffer, 'der')
  return decodeRsaPrivate(info.privateKey.data, extras)
}

function getDecoder(header) {
  var match = /^-----BEGIN (RSA )?(PUBLIC|PRIVATE) KEY-----$/.exec(header)
  if (!match) { return null }
  var isRSA = !!(match[1])
  var isPrivate = (match[2] === 'PRIVATE')
  if (isPrivate) {
    return isRSA ? decodeRsaPrivate : decodePrivate
  }
  else {
    return isRSA ? decodeRsaPublic : decodePublic
  }
}

function pem2jwk(pem, extras) {
  var text = pem.toString().split(/(\r\n|\r|\n)+/g)
  text = text.filter(function(line) {
    return line.trim().length !== 0
  });
  var decoder = getDecoder(text[0])

  text = text.slice(1, -1).join('')
  return decoder(new Buffer(text.replace(/[^\w\d\+\/=]+/g, ''), 'base64'), extras)
}

function parse(jwk) {
  return {
    n: string2bn(jwk.n),
    e: string2bn(jwk.e),
    d: jwk.d && string2bn(jwk.d),
    p: jwk.p && string2bn(jwk.p),
    q: jwk.q && string2bn(jwk.q),
    dp: jwk.dp && string2bn(jwk.dp),
    dq: jwk.dq && string2bn(jwk.dq),
    qi: jwk.qi && string2bn(jwk.qi)
  }
}

function jwk2pem(json) {
  var jwk = parse(json)
  var isPrivate = !!(jwk.d)
  var t = isPrivate ? 'PRIVATE' : 'PUBLIC'
  var header = '-----BEGIN RSA ' + t + ' KEY-----\n'
  var footer = '\n-----END RSA ' + t + ' KEY-----\n'
  var data = Buffer(0)
  if (isPrivate) {
    if (!jwk.p) {
      throw new Error('JWK primes not computed.')
    }
    jwk.version = 'two-prime'
    data = RSAPrivateKey.encode(jwk, 'der')
  }
  else {
    data = RSAPublicKey.encode(jwk, 'der')
  }
  var body = data.toString('base64').match(/.{1,64}/g).join('\n')
  return header + body + footer
}

function bn2base64url(bn) {
  return hex2b64url(pad(bn.toString(16)))
}

function base64url2bn(str) {
  return new asn.bignum(Buffer(str, 'base64'))
}

function string2bn(str) {
  if (/^[0-9]+$/.test(str)) {
    return new asn.bignum(str, 10)
  }
  return base64url2bn(str)
}

module.exports = {
  pem2jwk: pem2jwk,
  jwk2pem: jwk2pem,
  BN: asn.bignum
}


/***/ }),
/* 6 */
/***/ (function(__unused_webpack_module, exports, __webpack_require__) {

var asn1 = exports;

asn1.bignum = __webpack_require__(7);

asn1.define = (__webpack_require__(9).define);
asn1.base = __webpack_require__(12);
asn1.constants = __webpack_require__(20);
asn1.decoders = __webpack_require__(22);
asn1.encoders = __webpack_require__(25);


/***/ }),
/* 7 */
/***/ (function(module, __unused_webpack_exports, __webpack_require__) {

/* module decorator */ module = __webpack_require__.nmd(module);
(function (module, exports) {
  'use strict';

  // Utils
  function assert (val, msg) {
    if (!val) throw new Error(msg || 'Assertion failed');
  }

  // Could use `inherits` module, but don't want to move from single file
  // architecture yet.
  function inherits (ctor, superCtor) {
    ctor.super_ = superCtor;
    var TempCtor = function () {};
    TempCtor.prototype = superCtor.prototype;
    ctor.prototype = new TempCtor();
    ctor.prototype.constructor = ctor;
  }

  // BN

  function BN (number, base, endian) {
    if (BN.isBN(number)) {
      return number;
    }

    this.negative = 0;
    this.words = null;
    this.length = 0;

    // Reduction context
    this.red = null;

    if (number !== null) {
      if (base === 'le' || base === 'be') {
        endian = base;
        base = 10;
      }

      this._init(number || 0, base || 10, endian || 'be');
    }
  }
  if (typeof module === 'object') {
    module.exports = BN;
  } else {
    exports.BN = BN;
  }

  BN.BN = BN;
  BN.wordSize = 26;

  var Buffer;
  try {
    if (typeof window !== 'undefined' && typeof window.Buffer !== 'undefined') {
      Buffer = window.Buffer;
    } else {
      Buffer = (__webpack_require__(8).Buffer);
    }
  } catch (e) {
  }

  BN.isBN = function isBN (num) {
    if (num instanceof BN) {
      return true;
    }

    return num !== null && typeof num === 'object' &&
      num.constructor.wordSize === BN.wordSize && Array.isArray(num.words);
  };

  BN.max = function max (left, right) {
    if (left.cmp(right) > 0) return left;
    return right;
  };

  BN.min = function min (left, right) {
    if (left.cmp(right) < 0) return left;
    return right;
  };

  BN.prototype._init = function init (number, base, endian) {
    if (typeof number === 'number') {
      return this._initNumber(number, base, endian);
    }

    if (typeof number === 'object') {
      return this._initArray(number, base, endian);
    }

    if (base === 'hex') {
      base = 16;
    }
    assert(base === (base | 0) && base >= 2 && base <= 36);

    number = number.toString().replace(/\s+/g, '');
    var start = 0;
    if (number[0] === '-') {
      start++;
      this.negative = 1;
    }

    if (start < number.length) {
      if (base === 16) {
        this._parseHex(number, start, endian);
      } else {
        this._parseBase(number, base, start);
        if (endian === 'le') {
          this._initArray(this.toArray(), base, endian);
        }
      }
    }
  };

  BN.prototype._initNumber = function _initNumber (number, base, endian) {
    if (number < 0) {
      this.negative = 1;
      number = -number;
    }
    if (number < 0x4000000) {
      this.words = [ number & 0x3ffffff ];
      this.length = 1;
    } else if (number < 0x10000000000000) {
      this.words = [
        number & 0x3ffffff,
        (number / 0x4000000) & 0x3ffffff
      ];
      this.length = 2;
    } else {
      assert(number < 0x20000000000000); // 2 ^ 53 (unsafe)
      this.words = [
        number & 0x3ffffff,
        (number / 0x4000000) & 0x3ffffff,
        1
      ];
      this.length = 3;
    }

    if (endian !== 'le') return;

    // Reverse the bytes
    this._initArray(this.toArray(), base, endian);
  };

  BN.prototype._initArray = function _initArray (number, base, endian) {
    // Perhaps a Uint8Array
    assert(typeof number.length === 'number');
    if (number.length <= 0) {
      this.words = [ 0 ];
      this.length = 1;
      return this;
    }

    this.length = Math.ceil(number.length / 3);
    this.words = new Array(this.length);
    for (var i = 0; i < this.length; i++) {
      this.words[i] = 0;
    }

    var j, w;
    var off = 0;
    if (endian === 'be') {
      for (i = number.length - 1, j = 0; i >= 0; i -= 3) {
        w = number[i] | (number[i - 1] << 8) | (number[i - 2] << 16);
        this.words[j] |= (w << off) & 0x3ffffff;
        this.words[j + 1] = (w >>> (26 - off)) & 0x3ffffff;
        off += 24;
        if (off >= 26) {
          off -= 26;
          j++;
        }
      }
    } else if (endian === 'le') {
      for (i = 0, j = 0; i < number.length; i += 3) {
        w = number[i] | (number[i + 1] << 8) | (number[i + 2] << 16);
        this.words[j] |= (w << off) & 0x3ffffff;
        this.words[j + 1] = (w >>> (26 - off)) & 0x3ffffff;
        off += 24;
        if (off >= 26) {
          off -= 26;
          j++;
        }
      }
    }
    return this.strip();
  };

  function parseHex4Bits (string, index) {
    var c = string.charCodeAt(index);
    // 'A' - 'F'
    if (c >= 65 && c <= 70) {
      return c - 55;
    // 'a' - 'f'
    } else if (c >= 97 && c <= 102) {
      return c - 87;
    // '0' - '9'
    } else {
      return (c - 48) & 0xf;
    }
  }

  function parseHexByte (string, lowerBound, index) {
    var r = parseHex4Bits(string, index);
    if (index - 1 >= lowerBound) {
      r |= parseHex4Bits(string, index - 1) << 4;
    }
    return r;
  }

  BN.prototype._parseHex = function _parseHex (number, start, endian) {
    // Create possibly bigger array to ensure that it fits the number
    this.length = Math.ceil((number.length - start) / 6);
    this.words = new Array(this.length);
    for (var i = 0; i < this.length; i++) {
      this.words[i] = 0;
    }

    // 24-bits chunks
    var off = 0;
    var j = 0;

    var w;
    if (endian === 'be') {
      for (i = number.length - 1; i >= start; i -= 2) {
        w = parseHexByte(number, start, i) << off;
        this.words[j] |= w & 0x3ffffff;
        if (off >= 18) {
          off -= 18;
          j += 1;
          this.words[j] |= w >>> 26;
        } else {
          off += 8;
        }
      }
    } else {
      var parseLength = number.length - start;
      for (i = parseLength % 2 === 0 ? start + 1 : start; i < number.length; i += 2) {
        w = parseHexByte(number, start, i) << off;
        this.words[j] |= w & 0x3ffffff;
        if (off >= 18) {
          off -= 18;
          j += 1;
          this.words[j] |= w >>> 26;
        } else {
          off += 8;
        }
      }
    }

    this.strip();
  };

  function parseBase (str, start, end, mul) {
    var r = 0;
    var len = Math.min(str.length, end);
    for (var i = start; i < len; i++) {
      var c = str.charCodeAt(i) - 48;

      r *= mul;

      // 'a'
      if (c >= 49) {
        r += c - 49 + 0xa;

      // 'A'
      } else if (c >= 17) {
        r += c - 17 + 0xa;

      // '0' - '9'
      } else {
        r += c;
      }
    }
    return r;
  }

  BN.prototype._parseBase = function _parseBase (number, base, start) {
    // Initialize as zero
    this.words = [ 0 ];
    this.length = 1;

    // Find length of limb in base
    for (var limbLen = 0, limbPow = 1; limbPow <= 0x3ffffff; limbPow *= base) {
      limbLen++;
    }
    limbLen--;
    limbPow = (limbPow / base) | 0;

    var total = number.length - start;
    var mod = total % limbLen;
    var end = Math.min(total, total - mod) + start;

    var word = 0;
    for (var i = start; i < end; i += limbLen) {
      word = parseBase(number, i, i + limbLen, base);

      this.imuln(limbPow);
      if (this.words[0] + word < 0x4000000) {
        this.words[0] += word;
      } else {
        this._iaddn(word);
      }
    }

    if (mod !== 0) {
      var pow = 1;
      word = parseBase(number, i, number.length, base);

      for (i = 0; i < mod; i++) {
        pow *= base;
      }

      this.imuln(pow);
      if (this.words[0] + word < 0x4000000) {
        this.words[0] += word;
      } else {
        this._iaddn(word);
      }
    }

    this.strip();
  };

  BN.prototype.copy = function copy (dest) {
    dest.words = new Array(this.length);
    for (var i = 0; i < this.length; i++) {
      dest.words[i] = this.words[i];
    }
    dest.length = this.length;
    dest.negative = this.negative;
    dest.red = this.red;
  };

  BN.prototype.clone = function clone () {
    var r = new BN(null);
    this.copy(r);
    return r;
  };

  BN.prototype._expand = function _expand (size) {
    while (this.length < size) {
      this.words[this.length++] = 0;
    }
    return this;
  };

  // Remove leading `0` from `this`
  BN.prototype.strip = function strip () {
    while (this.length > 1 && this.words[this.length - 1] === 0) {
      this.length--;
    }
    return this._normSign();
  };

  BN.prototype._normSign = function _normSign () {
    // -0 = 0
    if (this.length === 1 && this.words[0] === 0) {
      this.negative = 0;
    }
    return this;
  };

  BN.prototype.inspect = function inspect () {
    return (this.red ? '<BN-R: ' : '<BN: ') + this.toString(16) + '>';
  };

  /*

  var zeros = [];
  var groupSizes = [];
  var groupBases = [];

  var s = '';
  var i = -1;
  while (++i < BN.wordSize) {
    zeros[i] = s;
    s += '0';
  }
  groupSizes[0] = 0;
  groupSizes[1] = 0;
  groupBases[0] = 0;
  groupBases[1] = 0;
  var base = 2 - 1;
  while (++base < 36 + 1) {
    var groupSize = 0;
    var groupBase = 1;
    while (groupBase < (1 << BN.wordSize) / base) {
      groupBase *= base;
      groupSize += 1;
    }
    groupSizes[base] = groupSize;
    groupBases[base] = groupBase;
  }

  */

  var zeros = [
    '',
    '0',
    '00',
    '000',
    '0000',
    '00000',
    '000000',
    '0000000',
    '00000000',
    '000000000',
    '0000000000',
    '00000000000',
    '000000000000',
    '0000000000000',
    '00000000000000',
    '000000000000000',
    '0000000000000000',
    '00000000000000000',
    '000000000000000000',
    '0000000000000000000',
    '00000000000000000000',
    '000000000000000000000',
    '0000000000000000000000',
    '00000000000000000000000',
    '000000000000000000000000',
    '0000000000000000000000000'
  ];

  var groupSizes = [
    0, 0,
    25, 16, 12, 11, 10, 9, 8,
    8, 7, 7, 7, 7, 6, 6,
    6, 6, 6, 6, 6, 5, 5,
    5, 5, 5, 5, 5, 5, 5,
    5, 5, 5, 5, 5, 5, 5
  ];

  var groupBases = [
    0, 0,
    33554432, 43046721, 16777216, 48828125, 60466176, 40353607, 16777216,
    43046721, 10000000, 19487171, 35831808, 62748517, 7529536, 11390625,
    16777216, 24137569, 34012224, 47045881, 64000000, 4084101, 5153632,
    6436343, 7962624, 9765625, 11881376, 14348907, 17210368, 20511149,
    24300000, 28629151, 33554432, 39135393, 45435424, 52521875, 60466176
  ];

  BN.prototype.toString = function toString (base, padding) {
    base = base || 10;
    padding = padding | 0 || 1;

    var out;
    if (base === 16 || base === 'hex') {
      out = '';
      var off = 0;
      var carry = 0;
      for (var i = 0; i < this.length; i++) {
        var w = this.words[i];
        var word = (((w << off) | carry) & 0xffffff).toString(16);
        carry = (w >>> (24 - off)) & 0xffffff;
        if (carry !== 0 || i !== this.length - 1) {
          out = zeros[6 - word.length] + word + out;
        } else {
          out = word + out;
        }
        off += 2;
        if (off >= 26) {
          off -= 26;
          i--;
        }
      }
      if (carry !== 0) {
        out = carry.toString(16) + out;
      }
      while (out.length % padding !== 0) {
        out = '0' + out;
      }
      if (this.negative !== 0) {
        out = '-' + out;
      }
      return out;
    }

    if (base === (base | 0) && base >= 2 && base <= 36) {
      // var groupSize = Math.floor(BN.wordSize * Math.LN2 / Math.log(base));
      var groupSize = groupSizes[base];
      // var groupBase = Math.pow(base, groupSize);
      var groupBase = groupBases[base];
      out = '';
      var c = this.clone();
      c.negative = 0;
      while (!c.isZero()) {
        var r = c.modn(groupBase).toString(base);
        c = c.idivn(groupBase);

        if (!c.isZero()) {
          out = zeros[groupSize - r.length] + r + out;
        } else {
          out = r + out;
        }
      }
      if (this.isZero()) {
        out = '0' + out;
      }
      while (out.length % padding !== 0) {
        out = '0' + out;
      }
      if (this.negative !== 0) {
        out = '-' + out;
      }
      return out;
    }

    assert(false, 'Base should be between 2 and 36');
  };

  BN.prototype.toNumber = function toNumber () {
    var ret = this.words[0];
    if (this.length === 2) {
      ret += this.words[1] * 0x4000000;
    } else if (this.length === 3 && this.words[2] === 0x01) {
      // NOTE: at this stage it is known that the top bit is set
      ret += 0x10000000000000 + (this.words[1] * 0x4000000);
    } else if (this.length > 2) {
      assert(false, 'Number can only safely store up to 53 bits');
    }
    return (this.negative !== 0) ? -ret : ret;
  };

  BN.prototype.toJSON = function toJSON () {
    return this.toString(16);
  };

  BN.prototype.toBuffer = function toBuffer (endian, length) {
    assert(typeof Buffer !== 'undefined');
    return this.toArrayLike(Buffer, endian, length);
  };

  BN.prototype.toArray = function toArray (endian, length) {
    return this.toArrayLike(Array, endian, length);
  };

  BN.prototype.toArrayLike = function toArrayLike (ArrayType, endian, length) {
    var byteLength = this.byteLength();
    var reqLength = length || Math.max(1, byteLength);
    assert(byteLength <= reqLength, 'byte array longer than desired length');
    assert(reqLength > 0, 'Requested array length <= 0');

    this.strip();
    var littleEndian = endian === 'le';
    var res = new ArrayType(reqLength);

    var b, i;
    var q = this.clone();
    if (!littleEndian) {
      // Assume big-endian
      for (i = 0; i < reqLength - byteLength; i++) {
        res[i] = 0;
      }

      for (i = 0; !q.isZero(); i++) {
        b = q.andln(0xff);
        q.iushrn(8);

        res[reqLength - i - 1] = b;
      }
    } else {
      for (i = 0; !q.isZero(); i++) {
        b = q.andln(0xff);
        q.iushrn(8);

        res[i] = b;
      }

      for (; i < reqLength; i++) {
        res[i] = 0;
      }
    }

    return res;
  };

  if (Math.clz32) {
    BN.prototype._countBits = function _countBits (w) {
      return 32 - Math.clz32(w);
    };
  } else {
    BN.prototype._countBits = function _countBits (w) {
      var t = w;
      var r = 0;
      if (t >= 0x1000) {
        r += 13;
        t >>>= 13;
      }
      if (t >= 0x40) {
        r += 7;
        t >>>= 7;
      }
      if (t >= 0x8) {
        r += 4;
        t >>>= 4;
      }
      if (t >= 0x02) {
        r += 2;
        t >>>= 2;
      }
      return r + t;
    };
  }

  BN.prototype._zeroBits = function _zeroBits (w) {
    // Short-cut
    if (w === 0) return 26;

    var t = w;
    var r = 0;
    if ((t & 0x1fff) === 0) {
      r += 13;
      t >>>= 13;
    }
    if ((t & 0x7f) === 0) {
      r += 7;
      t >>>= 7;
    }
    if ((t & 0xf) === 0) {
      r += 4;
      t >>>= 4;
    }
    if ((t & 0x3) === 0) {
      r += 2;
      t >>>= 2;
    }
    if ((t & 0x1) === 0) {
      r++;
    }
    return r;
  };

  // Return number of used bits in a BN
  BN.prototype.bitLength = function bitLength () {
    var w = this.words[this.length - 1];
    var hi = this._countBits(w);
    return (this.length - 1) * 26 + hi;
  };

  function toBitArray (num) {
    var w = new Array(num.bitLength());

    for (var bit = 0; bit < w.length; bit++) {
      var off = (bit / 26) | 0;
      var wbit = bit % 26;

      w[bit] = (num.words[off] & (1 << wbit)) >>> wbit;
    }

    return w;
  }

  // Number of trailing zero bits
  BN.prototype.zeroBits = function zeroBits () {
    if (this.isZero()) return 0;

    var r = 0;
    for (var i = 0; i < this.length; i++) {
      var b = this._zeroBits(this.words[i]);
      r += b;
      if (b !== 26) break;
    }
    return r;
  };

  BN.prototype.byteLength = function byteLength () {
    return Math.ceil(this.bitLength() / 8);
  };

  BN.prototype.toTwos = function toTwos (width) {
    if (this.negative !== 0) {
      return this.abs().inotn(width).iaddn(1);
    }
    return this.clone();
  };

  BN.prototype.fromTwos = function fromTwos (width) {
    if (this.testn(width - 1)) {
      return this.notn(width).iaddn(1).ineg();
    }
    return this.clone();
  };

  BN.prototype.isNeg = function isNeg () {
    return this.negative !== 0;
  };

  // Return negative clone of `this`
  BN.prototype.neg = function neg () {
    return this.clone().ineg();
  };

  BN.prototype.ineg = function ineg () {
    if (!this.isZero()) {
      this.negative ^= 1;
    }

    return this;
  };

  // Or `num` with `this` in-place
  BN.prototype.iuor = function iuor (num) {
    while (this.length < num.length) {
      this.words[this.length++] = 0;
    }

    for (var i = 0; i < num.length; i++) {
      this.words[i] = this.words[i] | num.words[i];
    }

    return this.strip();
  };

  BN.prototype.ior = function ior (num) {
    assert((this.negative | num.negative) === 0);
    return this.iuor(num);
  };

  // Or `num` with `this`
  BN.prototype.or = function or (num) {
    if (this.length > num.length) return this.clone().ior(num);
    return num.clone().ior(this);
  };

  BN.prototype.uor = function uor (num) {
    if (this.length > num.length) return this.clone().iuor(num);
    return num.clone().iuor(this);
  };

  // And `num` with `this` in-place
  BN.prototype.iuand = function iuand (num) {
    // b = min-length(num, this)
    var b;
    if (this.length > num.length) {
      b = num;
    } else {
      b = this;
    }

    for (var i = 0; i < b.length; i++) {
      this.words[i] = this.words[i] & num.words[i];
    }

    this.length = b.length;

    return this.strip();
  };

  BN.prototype.iand = function iand (num) {
    assert((this.negative | num.negative) === 0);
    return this.iuand(num);
  };

  // And `num` with `this`
  BN.prototype.and = function and (num) {
    if (this.length > num.length) return this.clone().iand(num);
    return num.clone().iand(this);
  };

  BN.prototype.uand = function uand (num) {
    if (this.length > num.length) return this.clone().iuand(num);
    return num.clone().iuand(this);
  };

  // Xor `num` with `this` in-place
  BN.prototype.iuxor = function iuxor (num) {
    // a.length > b.length
    var a;
    var b;
    if (this.length > num.length) {
      a = this;
      b = num;
    } else {
      a = num;
      b = this;
    }

    for (var i = 0; i < b.length; i++) {
      this.words[i] = a.words[i] ^ b.words[i];
    }

    if (this !== a) {
      for (; i < a.length; i++) {
        this.words[i] = a.words[i];
      }
    }

    this.length = a.length;

    return this.strip();
  };

  BN.prototype.ixor = function ixor (num) {
    assert((this.negative | num.negative) === 0);
    return this.iuxor(num);
  };

  // Xor `num` with `this`
  BN.prototype.xor = function xor (num) {
    if (this.length > num.length) return this.clone().ixor(num);
    return num.clone().ixor(this);
  };

  BN.prototype.uxor = function uxor (num) {
    if (this.length > num.length) return this.clone().iuxor(num);
    return num.clone().iuxor(this);
  };

  // Not ``this`` with ``width`` bitwidth
  BN.prototype.inotn = function inotn (width) {
    assert(typeof width === 'number' && width >= 0);

    var bytesNeeded = Math.ceil(width / 26) | 0;
    var bitsLeft = width % 26;

    // Extend the buffer with leading zeroes
    this._expand(bytesNeeded);

    if (bitsLeft > 0) {
      bytesNeeded--;
    }

    // Handle complete words
    for (var i = 0; i < bytesNeeded; i++) {
      this.words[i] = ~this.words[i] & 0x3ffffff;
    }

    // Handle the residue
    if (bitsLeft > 0) {
      this.words[i] = ~this.words[i] & (0x3ffffff >> (26 - bitsLeft));
    }

    // And remove leading zeroes
    return this.strip();
  };

  BN.prototype.notn = function notn (width) {
    return this.clone().inotn(width);
  };

  // Set `bit` of `this`
  BN.prototype.setn = function setn (bit, val) {
    assert(typeof bit === 'number' && bit >= 0);

    var off = (bit / 26) | 0;
    var wbit = bit % 26;

    this._expand(off + 1);

    if (val) {
      this.words[off] = this.words[off] | (1 << wbit);
    } else {
      this.words[off] = this.words[off] & ~(1 << wbit);
    }

    return this.strip();
  };

  // Add `num` to `this` in-place
  BN.prototype.iadd = function iadd (num) {
    var r;

    // negative + positive
    if (this.negative !== 0 && num.negative === 0) {
      this.negative = 0;
      r = this.isub(num);
      this.negative ^= 1;
      return this._normSign();

    // positive + negative
    } else if (this.negative === 0 && num.negative !== 0) {
      num.negative = 0;
      r = this.isub(num);
      num.negative = 1;
      return r._normSign();
    }

    // a.length > b.length
    var a, b;
    if (this.length > num.length) {
      a = this;
      b = num;
    } else {
      a = num;
      b = this;
    }

    var carry = 0;
    for (var i = 0; i < b.length; i++) {
      r = (a.words[i] | 0) + (b.words[i] | 0) + carry;
      this.words[i] = r & 0x3ffffff;
      carry = r >>> 26;
    }
    for (; carry !== 0 && i < a.length; i++) {
      r = (a.words[i] | 0) + carry;
      this.words[i] = r & 0x3ffffff;
      carry = r >>> 26;
    }

    this.length = a.length;
    if (carry !== 0) {
      this.words[this.length] = carry;
      this.length++;
    // Copy the rest of the words
    } else if (a !== this) {
      for (; i < a.length; i++) {
        this.words[i] = a.words[i];
      }
    }

    return this;
  };

  // Add `num` to `this`
  BN.prototype.add = function add (num) {
    var res;
    if (num.negative !== 0 && this.negative === 0) {
      num.negative = 0;
      res = this.sub(num);
      num.negative ^= 1;
      return res;
    } else if (num.negative === 0 && this.negative !== 0) {
      this.negative = 0;
      res = num.sub(this);
      this.negative = 1;
      return res;
    }

    if (this.length > num.length) return this.clone().iadd(num);

    return num.clone().iadd(this);
  };

  // Subtract `num` from `this` in-place
  BN.prototype.isub = function isub (num) {
    // this - (-num) = this + num
    if (num.negative !== 0) {
      num.negative = 0;
      var r = this.iadd(num);
      num.negative = 1;
      return r._normSign();

    // -this - num = -(this + num)
    } else if (this.negative !== 0) {
      this.negative = 0;
      this.iadd(num);
      this.negative = 1;
      return this._normSign();
    }

    // At this point both numbers are positive
    var cmp = this.cmp(num);

    // Optimization - zeroify
    if (cmp === 0) {
      this.negative = 0;
      this.length = 1;
      this.words[0] = 0;
      return this;
    }

    // a > b
    var a, b;
    if (cmp > 0) {
      a = this;
      b = num;
    } else {
      a = num;
      b = this;
    }

    var carry = 0;
    for (var i = 0; i < b.length; i++) {
      r = (a.words[i] | 0) - (b.words[i] | 0) + carry;
      carry = r >> 26;
      this.words[i] = r & 0x3ffffff;
    }
    for (; carry !== 0 && i < a.length; i++) {
      r = (a.words[i] | 0) + carry;
      carry = r >> 26;
      this.words[i] = r & 0x3ffffff;
    }

    // Copy rest of the words
    if (carry === 0 && i < a.length && a !== this) {
      for (; i < a.length; i++) {
        this.words[i] = a.words[i];
      }
    }

    this.length = Math.max(this.length, i);

    if (a !== this) {
      this.negative = 1;
    }

    return this.strip();
  };

  // Subtract `num` from `this`
  BN.prototype.sub = function sub (num) {
    return this.clone().isub(num);
  };

  function smallMulTo (self, num, out) {
    out.negative = num.negative ^ self.negative;
    var len = (self.length + num.length) | 0;
    out.length = len;
    len = (len - 1) | 0;

    // Peel one iteration (compiler can't do it, because of code complexity)
    var a = self.words[0] | 0;
    var b = num.words[0] | 0;
    var r = a * b;

    var lo = r & 0x3ffffff;
    var carry = (r / 0x4000000) | 0;
    out.words[0] = lo;

    for (var k = 1; k < len; k++) {
      // Sum all words with the same `i + j = k` and accumulate `ncarry`,
      // note that ncarry could be >= 0x3ffffff
      var ncarry = carry >>> 26;
      var rword = carry & 0x3ffffff;
      var maxJ = Math.min(k, num.length - 1);
      for (var j = Math.max(0, k - self.length + 1); j <= maxJ; j++) {
        var i = (k - j) | 0;
        a = self.words[i] | 0;
        b = num.words[j] | 0;
        r = a * b + rword;
        ncarry += (r / 0x4000000) | 0;
        rword = r & 0x3ffffff;
      }
      out.words[k] = rword | 0;
      carry = ncarry | 0;
    }
    if (carry !== 0) {
      out.words[k] = carry | 0;
    } else {
      out.length--;
    }

    return out.strip();
  }

  // TODO(indutny): it may be reasonable to omit it for users who don't need
  // to work with 256-bit numbers, otherwise it gives 20% improvement for 256-bit
  // multiplication (like elliptic secp256k1).
  var comb10MulTo = function comb10MulTo (self, num, out) {
    var a = self.words;
    var b = num.words;
    var o = out.words;
    var c = 0;
    var lo;
    var mid;
    var hi;
    var a0 = a[0] | 0;
    var al0 = a0 & 0x1fff;
    var ah0 = a0 >>> 13;
    var a1 = a[1] | 0;
    var al1 = a1 & 0x1fff;
    var ah1 = a1 >>> 13;
    var a2 = a[2] | 0;
    var al2 = a2 & 0x1fff;
    var ah2 = a2 >>> 13;
    var a3 = a[3] | 0;
    var al3 = a3 & 0x1fff;
    var ah3 = a3 >>> 13;
    var a4 = a[4] | 0;
    var al4 = a4 & 0x1fff;
    var ah4 = a4 >>> 13;
    var a5 = a[5] | 0;
    var al5 = a5 & 0x1fff;
    var ah5 = a5 >>> 13;
    var a6 = a[6] | 0;
    var al6 = a6 & 0x1fff;
    var ah6 = a6 >>> 13;
    var a7 = a[7] | 0;
    var al7 = a7 & 0x1fff;
    var ah7 = a7 >>> 13;
    var a8 = a[8] | 0;
    var al8 = a8 & 0x1fff;
    var ah8 = a8 >>> 13;
    var a9 = a[9] | 0;
    var al9 = a9 & 0x1fff;
    var ah9 = a9 >>> 13;
    var b0 = b[0] | 0;
    var bl0 = b0 & 0x1fff;
    var bh0 = b0 >>> 13;
    var b1 = b[1] | 0;
    var bl1 = b1 & 0x1fff;
    var bh1 = b1 >>> 13;
    var b2 = b[2] | 0;
    var bl2 = b2 & 0x1fff;
    var bh2 = b2 >>> 13;
    var b3 = b[3] | 0;
    var bl3 = b3 & 0x1fff;
    var bh3 = b3 >>> 13;
    var b4 = b[4] | 0;
    var bl4 = b4 & 0x1fff;
    var bh4 = b4 >>> 13;
    var b5 = b[5] | 0;
    var bl5 = b5 & 0x1fff;
    var bh5 = b5 >>> 13;
    var b6 = b[6] | 0;
    var bl6 = b6 & 0x1fff;
    var bh6 = b6 >>> 13;
    var b7 = b[7] | 0;
    var bl7 = b7 & 0x1fff;
    var bh7 = b7 >>> 13;
    var b8 = b[8] | 0;
    var bl8 = b8 & 0x1fff;
    var bh8 = b8 >>> 13;
    var b9 = b[9] | 0;
    var bl9 = b9 & 0x1fff;
    var bh9 = b9 >>> 13;

    out.negative = self.negative ^ num.negative;
    out.length = 19;
    /* k = 0 */
    lo = Math.imul(al0, bl0);
    mid = Math.imul(al0, bh0);
    mid = (mid + Math.imul(ah0, bl0)) | 0;
    hi = Math.imul(ah0, bh0);
    var w0 = (((c + lo) | 0) + ((mid & 0x1fff) << 13)) | 0;
    c = (((hi + (mid >>> 13)) | 0) + (w0 >>> 26)) | 0;
    w0 &= 0x3ffffff;
    /* k = 1 */
    lo = Math.imul(al1, bl0);
    mid = Math.imul(al1, bh0);
    mid = (mid + Math.imul(ah1, bl0)) | 0;
    hi = Math.imul(ah1, bh0);
    lo = (lo + Math.imul(al0, bl1)) | 0;
    mid = (mid + Math.imul(al0, bh1)) | 0;
    mid = (mid + Math.imul(ah0, bl1)) | 0;
    hi = (hi + Math.imul(ah0, bh1)) | 0;
    var w1 = (((c + lo) | 0) + ((mid & 0x1fff) << 13)) | 0;
    c = (((hi + (mid >>> 13)) | 0) + (w1 >>> 26)) | 0;
    w1 &= 0x3ffffff;
    /* k = 2 */
    lo = Math.imul(al2, bl0);
    mid = Math.imul(al2, bh0);
    mid = (mid + Math.imul(ah2, bl0)) | 0;
    hi = Math.imul(ah2, bh0);
    lo = (lo + Math.imul(al1, bl1)) | 0;
    mid = (mid + Math.imul(al1, bh1)) | 0;
    mid = (mid + Math.imul(ah1, bl1)) | 0;
    hi = (hi + Math.imul(ah1, bh1)) | 0;
    lo = (lo + Math.imul(al0, bl2)) | 0;
    mid = (mid + Math.imul(al0, bh2)) | 0;
    mid = (mid + Math.imul(ah0, bl2)) | 0;
    hi = (hi + Math.imul(ah0, bh2)) | 0;
    var w2 = (((c + lo) | 0) + ((mid & 0x1fff) << 13)) | 0;
    c = (((hi + (mid >>> 13)) | 0) + (w2 >>> 26)) | 0;
    w2 &= 0x3ffffff;
    /* k = 3 */
    lo = Math.imul(al3, bl0);
    mid = Math.imul(al3, bh0);
    mid = (mid + Math.imul(ah3, bl0)) | 0;
    hi = Math.imul(ah3, bh0);
    lo = (lo + Math.imul(al2, bl1)) | 0;
    mid = (mid + Math.imul(al2, bh1)) | 0;
    mid = (mid + Math.imul(ah2, bl1)) | 0;
    hi = (hi + Math.imul(ah2, bh1)) | 0;
    lo = (lo + Math.imul(al1, bl2)) | 0;
    mid = (mid + Math.imul(al1, bh2)) | 0;
    mid = (mid + Math.imul(ah1, bl2)) | 0;
    hi = (hi + Math.imul(ah1, bh2)) | 0;
    lo = (lo + Math.imul(al0, bl3)) | 0;
    mid = (mid + Math.imul(al0, bh3)) | 0;
    mid = (mid + Math.imul(ah0, bl3)) | 0;
    hi = (hi + Math.imul(ah0, bh3)) | 0;
    var w3 = (((c + lo) | 0) + ((mid & 0x1fff) << 13)) | 0;
    c = (((hi + (mid >>> 13)) | 0) + (w3 >>> 26)) | 0;
    w3 &= 0x3ffffff;
    /* k = 4 */
    lo = Math.imul(al4, bl0);
    mid = Math.imul(al4, bh0);
    mid = (mid + Math.imul(ah4, bl0)) | 0;
    hi = Math.imul(ah4, bh0);
    lo = (lo + Math.imul(al3, bl1)) | 0;
    mid = (mid + Math.imul(al3, bh1)) | 0;
    mid = (mid + Math.imul(ah3, bl1)) | 0;
    hi = (hi + Math.imul(ah3, bh1)) | 0;
    lo = (lo + Math.imul(al2, bl2)) | 0;
    mid = (mid + Math.imul(al2, bh2)) | 0;
    mid = (mid + Math.imul(ah2, bl2)) | 0;
    hi = (hi + Math.imul(ah2, bh2)) | 0;
    lo = (lo + Math.imul(al1, bl3)) | 0;
    mid = (mid + Math.imul(al1, bh3)) | 0;
    mid = (mid + Math.imul(ah1, bl3)) | 0;
    hi = (hi + Math.imul(ah1, bh3)) | 0;
    lo = (lo + Math.imul(al0, bl4)) | 0;
    mid = (mid + Math.imul(al0, bh4)) | 0;
    mid = (mid + Math.imul(ah0, bl4)) | 0;
    hi = (hi + Math.imul(ah0, bh4)) | 0;
    var w4 = (((c + lo) | 0) + ((mid & 0x1fff) << 13)) | 0;
    c = (((hi + (mid >>> 13)) | 0) + (w4 >>> 26)) | 0;
    w4 &= 0x3ffffff;
    /* k = 5 */
    lo = Math.imul(al5, bl0);
    mid = Math.imul(al5, bh0);
    mid = (mid + Math.imul(ah5, bl0)) | 0;
    hi = Math.imul(ah5, bh0);
    lo = (lo + Math.imul(al4, bl1)) | 0;
    mid = (mid + Math.imul(al4, bh1)) | 0;
    mid = (mid + Math.imul(ah4, bl1)) | 0;
    hi = (hi + Math.imul(ah4, bh1)) | 0;
    lo = (lo + Math.imul(al3, bl2)) | 0;
    mid = (mid + Math.imul(al3, bh2)) | 0;
    mid = (mid + Math.imul(ah3, bl2)) | 0;
    hi = (hi + Math.imul(ah3, bh2)) | 0;
    lo = (lo + Math.imul(al2, bl3)) | 0;
    mid = (mid + Math.imul(al2, bh3)) | 0;
    mid = (mid + Math.imul(ah2, bl3)) | 0;
    hi = (hi + Math.imul(ah2, bh3)) | 0;
    lo = (lo + Math.imul(al1, bl4)) | 0;
    mid = (mid + Math.imul(al1, bh4)) | 0;
    mid = (mid + Math.imul(ah1, bl4)) | 0;
    hi = (hi + Math.imul(ah1, bh4)) | 0;
    lo = (lo + Math.imul(al0, bl5)) | 0;
    mid = (mid + Math.imul(al0, bh5)) | 0;
    mid = (mid + Math.imul(ah0, bl5)) | 0;
    hi = (hi + Math.imul(ah0, bh5)) | 0;
    var w5 = (((c + lo) | 0) + ((mid & 0x1fff) << 13)) | 0;
    c = (((hi + (mid >>> 13)) | 0) + (w5 >>> 26)) | 0;
    w5 &= 0x3ffffff;
    /* k = 6 */
    lo = Math.imul(al6, bl0);
    mid = Math.imul(al6, bh0);
    mid = (mid + Math.imul(ah6, bl0)) | 0;
    hi = Math.imul(ah6, bh0);
    lo = (lo + Math.imul(al5, bl1)) | 0;
    mid = (mid + Math.imul(al5, bh1)) | 0;
    mid = (mid + Math.imul(ah5, bl1)) | 0;
    hi = (hi + Math.imul(ah5, bh1)) | 0;
    lo = (lo + Math.imul(al4, bl2)) | 0;
    mid = (mid + Math.imul(al4, bh2)) | 0;
    mid = (mid + Math.imul(ah4, bl2)) | 0;
    hi = (hi + Math.imul(ah4, bh2)) | 0;
    lo = (lo + Math.imul(al3, bl3)) | 0;
    mid = (mid + Math.imul(al3, bh3)) | 0;
    mid = (mid + Math.imul(ah3, bl3)) | 0;
    hi = (hi + Math.imul(ah3, bh3)) | 0;
    lo = (lo + Math.imul(al2, bl4)) | 0;
    mid = (mid + Math.imul(al2, bh4)) | 0;
    mid = (mid + Math.imul(ah2, bl4)) | 0;
    hi = (hi + Math.imul(ah2, bh4)) | 0;
    lo = (lo + Math.imul(al1, bl5)) | 0;
    mid = (mid + Math.imul(al1, bh5)) | 0;
    mid = (mid + Math.imul(ah1, bl5)) | 0;
    hi = (hi + Math.imul(ah1, bh5)) | 0;
    lo = (lo + Math.imul(al0, bl6)) | 0;
    mid = (mid + Math.imul(al0, bh6)) | 0;
    mid = (mid + Math.imul(ah0, bl6)) | 0;
    hi = (hi + Math.imul(ah0, bh6)) | 0;
    var w6 = (((c + lo) | 0) + ((mid & 0x1fff) << 13)) | 0;
    c = (((hi + (mid >>> 13)) | 0) + (w6 >>> 26)) | 0;
    w6 &= 0x3ffffff;
    /* k = 7 */
    lo = Math.imul(al7, bl0);
    mid = Math.imul(al7, bh0);
    mid = (mid + Math.imul(ah7, bl0)) | 0;
    hi = Math.imul(ah7, bh0);
    lo = (lo + Math.imul(al6, bl1)) | 0;
    mid = (mid + Math.imul(al6, bh1)) | 0;
    mid = (mid + Math.imul(ah6, bl1)) | 0;
    hi = (hi + Math.imul(ah6, bh1)) | 0;
    lo = (lo + Math.imul(al5, bl2)) | 0;
    mid = (mid + Math.imul(al5, bh2)) | 0;
    mid = (mid + Math.imul(ah5, bl2)) | 0;
    hi = (hi + Math.imul(ah5, bh2)) | 0;
    lo = (lo + Math.imul(al4, bl3)) | 0;
    mid = (mid + Math.imul(al4, bh3)) | 0;
    mid = (mid + Math.imul(ah4, bl3)) | 0;
    hi = (hi + Math.imul(ah4, bh3)) | 0;
    lo = (lo + Math.imul(al3, bl4)) | 0;
    mid = (mid + Math.imul(al3, bh4)) | 0;
    mid = (mid + Math.imul(ah3, bl4)) | 0;
    hi = (hi + Math.imul(ah3, bh4)) | 0;
    lo = (lo + Math.imul(al2, bl5)) | 0;
    mid = (mid + Math.imul(al2, bh5)) | 0;
    mid = (mid + Math.imul(ah2, bl5)) | 0;
    hi = (hi + Math.imul(ah2, bh5)) | 0;
    lo = (lo + Math.imul(al1, bl6)) | 0;
    mid = (mid + Math.imul(al1, bh6)) | 0;
    mid = (mid + Math.imul(ah1, bl6)) | 0;
    hi = (hi + Math.imul(ah1, bh6)) | 0;
    lo = (lo + Math.imul(al0, bl7)) | 0;
    mid = (mid + Math.imul(al0, bh7)) | 0;
    mid = (mid + Math.imul(ah0, bl7)) | 0;
    hi = (hi + Math.imul(ah0, bh7)) | 0;
    var w7 = (((c + lo) | 0) + ((mid & 0x1fff) << 13)) | 0;
    c = (((hi + (mid >>> 13)) | 0) + (w7 >>> 26)) | 0;
    w7 &= 0x3ffffff;
    /* k = 8 */
    lo = Math.imul(al8, bl0);
    mid = Math.imul(al8, bh0);
    mid = (mid + Math.imul(ah8, bl0)) | 0;
    hi = Math.imul(ah8, bh0);
    lo = (lo + Math.imul(al7, bl1)) | 0;
    mid = (mid + Math.imul(al7, bh1)) | 0;
    mid = (mid + Math.imul(ah7, bl1)) | 0;
    hi = (hi + Math.imul(ah7, bh1)) | 0;
    lo = (lo + Math.imul(al6, bl2)) | 0;
    mid = (mid + Math.imul(al6, bh2)) | 0;
    mid = (mid + Math.imul(ah6, bl2)) | 0;
    hi = (hi + Math.imul(ah6, bh2)) | 0;
    lo = (lo + Math.imul(al5, bl3)) | 0;
    mid = (mid + Math.imul(al5, bh3)) | 0;
    mid = (mid + Math.imul(ah5, bl3)) | 0;
    hi = (hi + Math.imul(ah5, bh3)) | 0;
    lo = (lo + Math.imul(al4, bl4)) | 0;
    mid = (mid + Math.imul(al4, bh4)) | 0;
    mid = (mid + Math.imul(ah4, bl4)) | 0;
    hi = (hi + Math.imul(ah4, bh4)) | 0;
    lo = (lo + Math.imul(al3, bl5)) | 0;
    mid = (mid + Math.imul(al3, bh5)) | 0;
    mid = (mid + Math.imul(ah3, bl5)) | 0;
    hi = (hi + Math.imul(ah3, bh5)) | 0;
    lo = (lo + Math.imul(al2, bl6)) | 0;
    mid = (mid + Math.imul(al2, bh6)) | 0;
    mid = (mid + Math.imul(ah2, bl6)) | 0;
    hi = (hi + Math.imul(ah2, bh6)) | 0;
    lo = (lo + Math.imul(al1, bl7)) | 0;
    mid = (mid + Math.imul(al1, bh7)) | 0;
    mid = (mid + Math.imul(ah1, bl7)) | 0;
    hi = (hi + Math.imul(ah1, bh7)) | 0;
    lo = (lo + Math.imul(al0, bl8)) | 0;
    mid = (mid + Math.imul(al0, bh8)) | 0;
    mid = (mid + Math.imul(ah0, bl8)) | 0;
    hi = (hi + Math.imul(ah0, bh8)) | 0;
    var w8 = (((c + lo) | 0) + ((mid & 0x1fff) << 13)) | 0;
    c = (((hi + (mid >>> 13)) | 0) + (w8 >>> 26)) | 0;
    w8 &= 0x3ffffff;
    /* k = 9 */
    lo = Math.imul(al9, bl0);
    mid = Math.imul(al9, bh0);
    mid = (mid + Math.imul(ah9, bl0)) | 0;
    hi = Math.imul(ah9, bh0);
    lo = (lo + Math.imul(al8, bl1)) | 0;
    mid = (mid + Math.imul(al8, bh1)) | 0;
    mid = (mid + Math.imul(ah8, bl1)) | 0;
    hi = (hi + Math.imul(ah8, bh1)) | 0;
    lo = (lo + Math.imul(al7, bl2)) | 0;
    mid = (mid + Math.imul(al7, bh2)) | 0;
    mid = (mid + Math.imul(ah7, bl2)) | 0;
    hi = (hi + Math.imul(ah7, bh2)) | 0;
    lo = (lo + Math.imul(al6, bl3)) | 0;
    mid = (mid + Math.imul(al6, bh3)) | 0;
    mid = (mid + Math.imul(ah6, bl3)) | 0;
    hi = (hi + Math.imul(ah6, bh3)) | 0;
    lo = (lo + Math.imul(al5, bl4)) | 0;
    mid = (mid + Math.imul(al5, bh4)) | 0;
    mid = (mid + Math.imul(ah5, bl4)) | 0;
    hi = (hi + Math.imul(ah5, bh4)) | 0;
    lo = (lo + Math.imul(al4, bl5)) | 0;
    mid = (mid + Math.imul(al4, bh5)) | 0;
    mid = (mid + Math.imul(ah4, bl5)) | 0;
    hi = (hi + Math.imul(ah4, bh5)) | 0;
    lo = (lo + Math.imul(al3, bl6)) | 0;
    mid = (mid + Math.imul(al3, bh6)) | 0;
    mid = (mid + Math.imul(ah3, bl6)) | 0;
    hi = (hi + Math.imul(ah3, bh6)) | 0;
    lo = (lo + Math.imul(al2, bl7)) | 0;
    mid = (mid + Math.imul(al2, bh7)) | 0;
    mid = (mid + Math.imul(ah2, bl7)) | 0;
    hi = (hi + Math.imul(ah2, bh7)) | 0;
    lo = (lo + Math.imul(al1, bl8)) | 0;
    mid = (mid + Math.imul(al1, bh8)) | 0;
    mid = (mid + Math.imul(ah1, bl8)) | 0;
    hi = (hi + Math.imul(ah1, bh8)) | 0;
    lo = (lo + Math.imul(al0, bl9)) | 0;
    mid = (mid + Math.imul(al0, bh9)) | 0;
    mid = (mid + Math.imul(ah0, bl9)) | 0;
    hi = (hi + Math.imul(ah0, bh9)) | 0;
    var w9 = (((c + lo) | 0) + ((mid & 0x1fff) << 13)) | 0;
    c = (((hi + (mid >>> 13)) | 0) + (w9 >>> 26)) | 0;
    w9 &= 0x3ffffff;
    /* k = 10 */
    lo = Math.imul(al9, bl1);
    mid = Math.imul(al9, bh1);
    mid = (mid + Math.imul(ah9, bl1)) | 0;
    hi = Math.imul(ah9, bh1);
    lo = (lo + Math.imul(al8, bl2)) | 0;
    mid = (mid + Math.imul(al8, bh2)) | 0;
    mid = (mid + Math.imul(ah8, bl2)) | 0;
    hi = (hi + Math.imul(ah8, bh2)) | 0;
    lo = (lo + Math.imul(al7, bl3)) | 0;
    mid = (mid + Math.imul(al7, bh3)) | 0;
    mid = (mid + Math.imul(ah7, bl3)) | 0;
    hi = (hi + Math.imul(ah7, bh3)) | 0;
    lo = (lo + Math.imul(al6, bl4)) | 0;
    mid = (mid + Math.imul(al6, bh4)) | 0;
    mid = (mid + Math.imul(ah6, bl4)) | 0;
    hi = (hi + Math.imul(ah6, bh4)) | 0;
    lo = (lo + Math.imul(al5, bl5)) | 0;
    mid = (mid + Math.imul(al5, bh5)) | 0;
    mid = (mid + Math.imul(ah5, bl5)) | 0;
    hi = (hi + Math.imul(ah5, bh5)) | 0;
    lo = (lo + Math.imul(al4, bl6)) | 0;
    mid = (mid + Math.imul(al4, bh6)) | 0;
    mid = (mid + Math.imul(ah4, bl6)) | 0;
    hi = (hi + Math.imul(ah4, bh6)) | 0;
    lo = (lo + Math.imul(al3, bl7)) | 0;
    mid = (mid + Math.imul(al3, bh7)) | 0;
    mid = (mid + Math.imul(ah3, bl7)) | 0;
    hi = (hi + Math.imul(ah3, bh7)) | 0;
    lo = (lo + Math.imul(al2, bl8)) | 0;
    mid = (mid + Math.imul(al2, bh8)) | 0;
    mid = (mid + Math.imul(ah2, bl8)) | 0;
    hi = (hi + Math.imul(ah2, bh8)) | 0;
    lo = (lo + Math.imul(al1, bl9)) | 0;
    mid = (mid + Math.imul(al1, bh9)) | 0;
    mid = (mid + Math.imul(ah1, bl9)) | 0;
    hi = (hi + Math.imul(ah1, bh9)) | 0;
    var w10 = (((c + lo) | 0) + ((mid & 0x1fff) << 13)) | 0;
    c = (((hi + (mid >>> 13)) | 0) + (w10 >>> 26)) | 0;
    w10 &= 0x3ffffff;
    /* k = 11 */
    lo = Math.imul(al9, bl2);
    mid = Math.imul(al9, bh2);
    mid = (mid + Math.imul(ah9, bl2)) | 0;
    hi = Math.imul(ah9, bh2);
    lo = (lo + Math.imul(al8, bl3)) | 0;
    mid = (mid + Math.imul(al8, bh3)) | 0;
    mid = (mid + Math.imul(ah8, bl3)) | 0;
    hi = (hi + Math.imul(ah8, bh3)) | 0;
    lo = (lo + Math.imul(al7, bl4)) | 0;
    mid = (mid + Math.imul(al7, bh4)) | 0;
    mid = (mid + Math.imul(ah7, bl4)) | 0;
    hi = (hi + Math.imul(ah7, bh4)) | 0;
    lo = (lo + Math.imul(al6, bl5)) | 0;
    mid = (mid + Math.imul(al6, bh5)) | 0;
    mid = (mid + Math.imul(ah6, bl5)) | 0;
    hi = (hi + Math.imul(ah6, bh5)) | 0;
    lo = (lo + Math.imul(al5, bl6)) | 0;
    mid = (mid + Math.imul(al5, bh6)) | 0;
    mid = (mid + Math.imul(ah5, bl6)) | 0;
    hi = (hi + Math.imul(ah5, bh6)) | 0;
    lo = (lo + Math.imul(al4, bl7)) | 0;
    mid = (mid + Math.imul(al4, bh7)) | 0;
    mid = (mid + Math.imul(ah4, bl7)) | 0;
    hi = (hi + Math.imul(ah4, bh7)) | 0;
    lo = (lo + Math.imul(al3, bl8)) | 0;
    mid = (mid + Math.imul(al3, bh8)) | 0;
    mid = (mid + Math.imul(ah3, bl8)) | 0;
    hi = (hi + Math.imul(ah3, bh8)) | 0;
    lo = (lo + Math.imul(al2, bl9)) | 0;
    mid = (mid + Math.imul(al2, bh9)) | 0;
    mid = (mid + Math.imul(ah2, bl9)) | 0;
    hi = (hi + Math.imul(ah2, bh9)) | 0;
    var w11 = (((c + lo) | 0) + ((mid & 0x1fff) << 13)) | 0;
    c = (((hi + (mid >>> 13)) | 0) + (w11 >>> 26)) | 0;
    w11 &= 0x3ffffff;
    /* k = 12 */
    lo = Math.imul(al9, bl3);
    mid = Math.imul(al9, bh3);
    mid = (mid + Math.imul(ah9, bl3)) | 0;
    hi = Math.imul(ah9, bh3);
    lo = (lo + Math.imul(al8, bl4)) | 0;
    mid = (mid + Math.imul(al8, bh4)) | 0;
    mid = (mid + Math.imul(ah8, bl4)) | 0;
    hi = (hi + Math.imul(ah8, bh4)) | 0;
    lo = (lo + Math.imul(al7, bl5)) | 0;
    mid = (mid + Math.imul(al7, bh5)) | 0;
    mid = (mid + Math.imul(ah7, bl5)) | 0;
    hi = (hi + Math.imul(ah7, bh5)) | 0;
    lo = (lo + Math.imul(al6, bl6)) | 0;
    mid = (mid + Math.imul(al6, bh6)) | 0;
    mid = (mid + Math.imul(ah6, bl6)) | 0;
    hi = (hi + Math.imul(ah6, bh6)) | 0;
    lo = (lo + Math.imul(al5, bl7)) | 0;
    mid = (mid + Math.imul(al5, bh7)) | 0;
    mid = (mid + Math.imul(ah5, bl7)) | 0;
    hi = (hi + Math.imul(ah5, bh7)) | 0;
    lo = (lo + Math.imul(al4, bl8)) | 0;
    mid = (mid + Math.imul(al4, bh8)) | 0;
    mid = (mid + Math.imul(ah4, bl8)) | 0;
    hi = (hi + Math.imul(ah4, bh8)) | 0;
    lo = (lo + Math.imul(al3, bl9)) | 0;
    mid = (mid + Math.imul(al3, bh9)) | 0;
    mid = (mid + Math.imul(ah3, bl9)) | 0;
    hi = (hi + Math.imul(ah3, bh9)) | 0;
    var w12 = (((c + lo) | 0) + ((mid & 0x1fff) << 13)) | 0;
    c = (((hi + (mid >>> 13)) | 0) + (w12 >>> 26)) | 0;
    w12 &= 0x3ffffff;
    /* k = 13 */
    lo = Math.imul(al9, bl4);
    mid = Math.imul(al9, bh4);
    mid = (mid + Math.imul(ah9, bl4)) | 0;
    hi = Math.imul(ah9, bh4);
    lo = (lo + Math.imul(al8, bl5)) | 0;
    mid = (mid + Math.imul(al8, bh5)) | 0;
    mid = (mid + Math.imul(ah8, bl5)) | 0;
    hi = (hi + Math.imul(ah8, bh5)) | 0;
    lo = (lo + Math.imul(al7, bl6)) | 0;
    mid = (mid + Math.imul(al7, bh6)) | 0;
    mid = (mid + Math.imul(ah7, bl6)) | 0;
    hi = (hi + Math.imul(ah7, bh6)) | 0;
    lo = (lo + Math.imul(al6, bl7)) | 0;
    mid = (mid + Math.imul(al6, bh7)) | 0;
    mid = (mid + Math.imul(ah6, bl7)) | 0;
    hi = (hi + Math.imul(ah6, bh7)) | 0;
    lo = (lo + Math.imul(al5, bl8)) | 0;
    mid = (mid + Math.imul(al5, bh8)) | 0;
    mid = (mid + Math.imul(ah5, bl8)) | 0;
    hi = (hi + Math.imul(ah5, bh8)) | 0;
    lo = (lo + Math.imul(al4, bl9)) | 0;
    mid = (mid + Math.imul(al4, bh9)) | 0;
    mid = (mid + Math.imul(ah4, bl9)) | 0;
    hi = (hi + Math.imul(ah4, bh9)) | 0;
    var w13 = (((c + lo) | 0) + ((mid & 0x1fff) << 13)) | 0;
    c = (((hi + (mid >>> 13)) | 0) + (w13 >>> 26)) | 0;
    w13 &= 0x3ffffff;
    /* k = 14 */
    lo = Math.imul(al9, bl5);
    mid = Math.imul(al9, bh5);
    mid = (mid + Math.imul(ah9, bl5)) | 0;
    hi = Math.imul(ah9, bh5);
    lo = (lo + Math.imul(al8, bl6)) | 0;
    mid = (mid + Math.imul(al8, bh6)) | 0;
    mid = (mid + Math.imul(ah8, bl6)) | 0;
    hi = (hi + Math.imul(ah8, bh6)) | 0;
    lo = (lo + Math.imul(al7, bl7)) | 0;
    mid = (mid + Math.imul(al7, bh7)) | 0;
    mid = (mid + Math.imul(ah7, bl7)) | 0;
    hi = (hi + Math.imul(ah7, bh7)) | 0;
    lo = (lo + Math.imul(al6, bl8)) | 0;
    mid = (mid + Math.imul(al6, bh8)) | 0;
    mid = (mid + Math.imul(ah6, bl8)) | 0;
    hi = (hi + Math.imul(ah6, bh8)) | 0;
    lo = (lo + Math.imul(al5, bl9)) | 0;
    mid = (mid + Math.imul(al5, bh9)) | 0;
    mid = (mid + Math.imul(ah5, bl9)) | 0;
    hi = (hi + Math.imul(ah5, bh9)) | 0;
    var w14 = (((c + lo) | 0) + ((mid & 0x1fff) << 13)) | 0;
    c = (((hi + (mid >>> 13)) | 0) + (w14 >>> 26)) | 0;
    w14 &= 0x3ffffff;
    /* k = 15 */
    lo = Math.imul(al9, bl6);
    mid = Math.imul(al9, bh6);
    mid = (mid + Math.imul(ah9, bl6)) | 0;
    hi = Math.imul(ah9, bh6);
    lo = (lo + Math.imul(al8, bl7)) | 0;
    mid = (mid + Math.imul(al8, bh7)) | 0;
    mid = (mid + Math.imul(ah8, bl7)) | 0;
    hi = (hi + Math.imul(ah8, bh7)) | 0;
    lo = (lo + Math.imul(al7, bl8)) | 0;
    mid = (mid + Math.imul(al7, bh8)) | 0;
    mid = (mid + Math.imul(ah7, bl8)) | 0;
    hi = (hi + Math.imul(ah7, bh8)) | 0;
    lo = (lo + Math.imul(al6, bl9)) | 0;
    mid = (mid + Math.imul(al6, bh9)) | 0;
    mid = (mid + Math.imul(ah6, bl9)) | 0;
    hi = (hi + Math.imul(ah6, bh9)) | 0;
    var w15 = (((c + lo) | 0) + ((mid & 0x1fff) << 13)) | 0;
    c = (((hi + (mid >>> 13)) | 0) + (w15 >>> 26)) | 0;
    w15 &= 0x3ffffff;
    /* k = 16 */
    lo = Math.imul(al9, bl7);
    mid = Math.imul(al9, bh7);
    mid = (mid + Math.imul(ah9, bl7)) | 0;
    hi = Math.imul(ah9, bh7);
    lo = (lo + Math.imul(al8, bl8)) | 0;
    mid = (mid + Math.imul(al8, bh8)) | 0;
    mid = (mid + Math.imul(ah8, bl8)) | 0;
    hi = (hi + Math.imul(ah8, bh8)) | 0;
    lo = (lo + Math.imul(al7, bl9)) | 0;
    mid = (mid + Math.imul(al7, bh9)) | 0;
    mid = (mid + Math.imul(ah7, bl9)) | 0;
    hi = (hi + Math.imul(ah7, bh9)) | 0;
    var w16 = (((c + lo) | 0) + ((mid & 0x1fff) << 13)) | 0;
    c = (((hi + (mid >>> 13)) | 0) + (w16 >>> 26)) | 0;
    w16 &= 0x3ffffff;
    /* k = 17 */
    lo = Math.imul(al9, bl8);
    mid = Math.imul(al9, bh8);
    mid = (mid + Math.imul(ah9, bl8)) | 0;
    hi = Math.imul(ah9, bh8);
    lo = (lo + Math.imul(al8, bl9)) | 0;
    mid = (mid + Math.imul(al8, bh9)) | 0;
    mid = (mid + Math.imul(ah8, bl9)) | 0;
    hi = (hi + Math.imul(ah8, bh9)) | 0;
    var w17 = (((c + lo) | 0) + ((mid & 0x1fff) << 13)) | 0;
    c = (((hi + (mid >>> 13)) | 0) + (w17 >>> 26)) | 0;
    w17 &= 0x3ffffff;
    /* k = 18 */
    lo = Math.imul(al9, bl9);
    mid = Math.imul(al9, bh9);
    mid = (mid + Math.imul(ah9, bl9)) | 0;
    hi = Math.imul(ah9, bh9);
    var w18 = (((c + lo) | 0) + ((mid & 0x1fff) << 13)) | 0;
    c = (((hi + (mid >>> 13)) | 0) + (w18 >>> 26)) | 0;
    w18 &= 0x3ffffff;
    o[0] = w0;
    o[1] = w1;
    o[2] = w2;
    o[3] = w3;
    o[4] = w4;
    o[5] = w5;
    o[6] = w6;
    o[7] = w7;
    o[8] = w8;
    o[9] = w9;
    o[10] = w10;
    o[11] = w11;
    o[12] = w12;
    o[13] = w13;
    o[14] = w14;
    o[15] = w15;
    o[16] = w16;
    o[17] = w17;
    o[18] = w18;
    if (c !== 0) {
      o[19] = c;
      out.length++;
    }
    return out;
  };

  // Polyfill comb
  if (!Math.imul) {
    comb10MulTo = smallMulTo;
  }

  function bigMulTo (self, num, out) {
    out.negative = num.negative ^ self.negative;
    out.length = self.length + num.length;

    var carry = 0;
    var hncarry = 0;
    for (var k = 0; k < out.length - 1; k++) {
      // Sum all words with the same `i + j = k` and accumulate `ncarry`,
      // note that ncarry could be >= 0x3ffffff
      var ncarry = hncarry;
      hncarry = 0;
      var rword = carry & 0x3ffffff;
      var maxJ = Math.min(k, num.length - 1);
      for (var j = Math.max(0, k - self.length + 1); j <= maxJ; j++) {
        var i = k - j;
        var a = self.words[i] | 0;
        var b = num.words[j] | 0;
        var r = a * b;

        var lo = r & 0x3ffffff;
        ncarry = (ncarry + ((r / 0x4000000) | 0)) | 0;
        lo = (lo + rword) | 0;
        rword = lo & 0x3ffffff;
        ncarry = (ncarry + (lo >>> 26)) | 0;

        hncarry += ncarry >>> 26;
        ncarry &= 0x3ffffff;
      }
      out.words[k] = rword;
      carry = ncarry;
      ncarry = hncarry;
    }
    if (carry !== 0) {
      out.words[k] = carry;
    } else {
      out.length--;
    }

    return out.strip();
  }

  function jumboMulTo (self, num, out) {
    var fftm = new FFTM();
    return fftm.mulp(self, num, out);
  }

  BN.prototype.mulTo = function mulTo (num, out) {
    var res;
    var len = this.length + num.length;
    if (this.length === 10 && num.length === 10) {
      res = comb10MulTo(this, num, out);
    } else if (len < 63) {
      res = smallMulTo(this, num, out);
    } else if (len < 1024) {
      res = bigMulTo(this, num, out);
    } else {
      res = jumboMulTo(this, num, out);
    }

    return res;
  };

  // Cooley-Tukey algorithm for FFT
  // slightly revisited to rely on looping instead of recursion

  function FFTM (x, y) {
    this.x = x;
    this.y = y;
  }

  FFTM.prototype.makeRBT = function makeRBT (N) {
    var t = new Array(N);
    var l = BN.prototype._countBits(N) - 1;
    for (var i = 0; i < N; i++) {
      t[i] = this.revBin(i, l, N);
    }

    return t;
  };

  // Returns binary-reversed representation of `x`
  FFTM.prototype.revBin = function revBin (x, l, N) {
    if (x === 0 || x === N - 1) return x;

    var rb = 0;
    for (var i = 0; i < l; i++) {
      rb |= (x & 1) << (l - i - 1);
      x >>= 1;
    }

    return rb;
  };

  // Performs "tweedling" phase, therefore 'emulating'
  // behaviour of the recursive algorithm
  FFTM.prototype.permute = function permute (rbt, rws, iws, rtws, itws, N) {
    for (var i = 0; i < N; i++) {
      rtws[i] = rws[rbt[i]];
      itws[i] = iws[rbt[i]];
    }
  };

  FFTM.prototype.transform = function transform (rws, iws, rtws, itws, N, rbt) {
    this.permute(rbt, rws, iws, rtws, itws, N);

    for (var s = 1; s < N; s <<= 1) {
      var l = s << 1;

      var rtwdf = Math.cos(2 * Math.PI / l);
      var itwdf = Math.sin(2 * Math.PI / l);

      for (var p = 0; p < N; p += l) {
        var rtwdf_ = rtwdf;
        var itwdf_ = itwdf;

        for (var j = 0; j < s; j++) {
          var re = rtws[p + j];
          var ie = itws[p + j];

          var ro = rtws[p + j + s];
          var io = itws[p + j + s];

          var rx = rtwdf_ * ro - itwdf_ * io;

          io = rtwdf_ * io + itwdf_ * ro;
          ro = rx;

          rtws[p + j] = re + ro;
          itws[p + j] = ie + io;

          rtws[p + j + s] = re - ro;
          itws[p + j + s] = ie - io;

          /* jshint maxdepth : false */
          if (j !== l) {
            rx = rtwdf * rtwdf_ - itwdf * itwdf_;

            itwdf_ = rtwdf * itwdf_ + itwdf * rtwdf_;
            rtwdf_ = rx;
          }
        }
      }
    }
  };

  FFTM.prototype.guessLen13b = function guessLen13b (n, m) {
    var N = Math.max(m, n) | 1;
    var odd = N & 1;
    var i = 0;
    for (N = N / 2 | 0; N; N = N >>> 1) {
      i++;
    }

    return 1 << i + 1 + odd;
  };

  FFTM.prototype.conjugate = function conjugate (rws, iws, N) {
    if (N <= 1) return;

    for (var i = 0; i < N / 2; i++) {
      var t = rws[i];

      rws[i] = rws[N - i - 1];
      rws[N - i - 1] = t;

      t = iws[i];

      iws[i] = -iws[N - i - 1];
      iws[N - i - 1] = -t;
    }
  };

  FFTM.prototype.normalize13b = function normalize13b (ws, N) {
    var carry = 0;
    for (var i = 0; i < N / 2; i++) {
      var w = Math.round(ws[2 * i + 1] / N) * 0x2000 +
        Math.round(ws[2 * i] / N) +
        carry;

      ws[i] = w & 0x3ffffff;

      if (w < 0x4000000) {
        carry = 0;
      } else {
        carry = w / 0x4000000 | 0;
      }
    }

    return ws;
  };

  FFTM.prototype.convert13b = function convert13b (ws, len, rws, N) {
    var carry = 0;
    for (var i = 0; i < len; i++) {
      carry = carry + (ws[i] | 0);

      rws[2 * i] = carry & 0x1fff; carry = carry >>> 13;
      rws[2 * i + 1] = carry & 0x1fff; carry = carry >>> 13;
    }

    // Pad with zeroes
    for (i = 2 * len; i < N; ++i) {
      rws[i] = 0;
    }

    assert(carry === 0);
    assert((carry & ~0x1fff) === 0);
  };

  FFTM.prototype.stub = function stub (N) {
    var ph = new Array(N);
    for (var i = 0; i < N; i++) {
      ph[i] = 0;
    }

    return ph;
  };

  FFTM.prototype.mulp = function mulp (x, y, out) {
    var N = 2 * this.guessLen13b(x.length, y.length);

    var rbt = this.makeRBT(N);

    var _ = this.stub(N);

    var rws = new Array(N);
    var rwst = new Array(N);
    var iwst = new Array(N);

    var nrws = new Array(N);
    var nrwst = new Array(N);
    var niwst = new Array(N);

    var rmws = out.words;
    rmws.length = N;

    this.convert13b(x.words, x.length, rws, N);
    this.convert13b(y.words, y.length, nrws, N);

    this.transform(rws, _, rwst, iwst, N, rbt);
    this.transform(nrws, _, nrwst, niwst, N, rbt);

    for (var i = 0; i < N; i++) {
      var rx = rwst[i] * nrwst[i] - iwst[i] * niwst[i];
      iwst[i] = rwst[i] * niwst[i] + iwst[i] * nrwst[i];
      rwst[i] = rx;
    }

    this.conjugate(rwst, iwst, N);
    this.transform(rwst, iwst, rmws, _, N, rbt);
    this.conjugate(rmws, _, N);
    this.normalize13b(rmws, N);

    out.negative = x.negative ^ y.negative;
    out.length = x.length + y.length;
    return out.strip();
  };

  // Multiply `this` by `num`
  BN.prototype.mul = function mul (num) {
    var out = new BN(null);
    out.words = new Array(this.length + num.length);
    return this.mulTo(num, out);
  };

  // Multiply employing FFT
  BN.prototype.mulf = function mulf (num) {
    var out = new BN(null);
    out.words = new Array(this.length + num.length);
    return jumboMulTo(this, num, out);
  };

  // In-place Multiplication
  BN.prototype.imul = function imul (num) {
    return this.clone().mulTo(num, this);
  };

  BN.prototype.imuln = function imuln (num) {
    assert(typeof num === 'number');
    assert(num < 0x4000000);

    // Carry
    var carry = 0;
    for (var i = 0; i < this.length; i++) {
      var w = (this.words[i] | 0) * num;
      var lo = (w & 0x3ffffff) + (carry & 0x3ffffff);
      carry >>= 26;
      carry += (w / 0x4000000) | 0;
      // NOTE: lo is 27bit maximum
      carry += lo >>> 26;
      this.words[i] = lo & 0x3ffffff;
    }

    if (carry !== 0) {
      this.words[i] = carry;
      this.length++;
    }

    return this;
  };

  BN.prototype.muln = function muln (num) {
    return this.clone().imuln(num);
  };

  // `this` * `this`
  BN.prototype.sqr = function sqr () {
    return this.mul(this);
  };

  // `this` * `this` in-place
  BN.prototype.isqr = function isqr () {
    return this.imul(this.clone());
  };

  // Math.pow(`this`, `num`)
  BN.prototype.pow = function pow (num) {
    var w = toBitArray(num);
    if (w.length === 0) return new BN(1);

    // Skip leading zeroes
    var res = this;
    for (var i = 0; i < w.length; i++, res = res.sqr()) {
      if (w[i] !== 0) break;
    }

    if (++i < w.length) {
      for (var q = res.sqr(); i < w.length; i++, q = q.sqr()) {
        if (w[i] === 0) continue;

        res = res.mul(q);
      }
    }

    return res;
  };

  // Shift-left in-place
  BN.prototype.iushln = function iushln (bits) {
    assert(typeof bits === 'number' && bits >= 0);
    var r = bits % 26;
    var s = (bits - r) / 26;
    var carryMask = (0x3ffffff >>> (26 - r)) << (26 - r);
    var i;

    if (r !== 0) {
      var carry = 0;

      for (i = 0; i < this.length; i++) {
        var newCarry = this.words[i] & carryMask;
        var c = ((this.words[i] | 0) - newCarry) << r;
        this.words[i] = c | carry;
        carry = newCarry >>> (26 - r);
      }

      if (carry) {
        this.words[i] = carry;
        this.length++;
      }
    }

    if (s !== 0) {
      for (i = this.length - 1; i >= 0; i--) {
        this.words[i + s] = this.words[i];
      }

      for (i = 0; i < s; i++) {
        this.words[i] = 0;
      }

      this.length += s;
    }

    return this.strip();
  };

  BN.prototype.ishln = function ishln (bits) {
    // TODO(indutny): implement me
    assert(this.negative === 0);
    return this.iushln(bits);
  };

  // Shift-right in-place
  // NOTE: `hint` is a lowest bit before trailing zeroes
  // NOTE: if `extended` is present - it will be filled with destroyed bits
  BN.prototype.iushrn = function iushrn (bits, hint, extended) {
    assert(typeof bits === 'number' && bits >= 0);
    var h;
    if (hint) {
      h = (hint - (hint % 26)) / 26;
    } else {
      h = 0;
    }

    var r = bits % 26;
    var s = Math.min((bits - r) / 26, this.length);
    var mask = 0x3ffffff ^ ((0x3ffffff >>> r) << r);
    var maskedWords = extended;

    h -= s;
    h = Math.max(0, h);

    // Extended mode, copy masked part
    if (maskedWords) {
      for (var i = 0; i < s; i++) {
        maskedWords.words[i] = this.words[i];
      }
      maskedWords.length = s;
    }

    if (s === 0) {
      // No-op, we should not move anything at all
    } else if (this.length > s) {
      this.length -= s;
      for (i = 0; i < this.length; i++) {
        this.words[i] = this.words[i + s];
      }
    } else {
      this.words[0] = 0;
      this.length = 1;
    }

    var carry = 0;
    for (i = this.length - 1; i >= 0 && (carry !== 0 || i >= h); i--) {
      var word = this.words[i] | 0;
      this.words[i] = (carry << (26 - r)) | (word >>> r);
      carry = word & mask;
    }

    // Push carried bits as a mask
    if (maskedWords && carry !== 0) {
      maskedWords.words[maskedWords.length++] = carry;
    }

    if (this.length === 0) {
      this.words[0] = 0;
      this.length = 1;
    }

    return this.strip();
  };

  BN.prototype.ishrn = function ishrn (bits, hint, extended) {
    // TODO(indutny): implement me
    assert(this.negative === 0);
    return this.iushrn(bits, hint, extended);
  };

  // Shift-left
  BN.prototype.shln = function shln (bits) {
    return this.clone().ishln(bits);
  };

  BN.prototype.ushln = function ushln (bits) {
    return this.clone().iushln(bits);
  };

  // Shift-right
  BN.prototype.shrn = function shrn (bits) {
    return this.clone().ishrn(bits);
  };

  BN.prototype.ushrn = function ushrn (bits) {
    return this.clone().iushrn(bits);
  };

  // Test if n bit is set
  BN.prototype.testn = function testn (bit) {
    assert(typeof bit === 'number' && bit >= 0);
    var r = bit % 26;
    var s = (bit - r) / 26;
    var q = 1 << r;

    // Fast case: bit is much higher than all existing words
    if (this.length <= s) return false;

    // Check bit and return
    var w = this.words[s];

    return !!(w & q);
  };

  // Return only lowers bits of number (in-place)
  BN.prototype.imaskn = function imaskn (bits) {
    assert(typeof bits === 'number' && bits >= 0);
    var r = bits % 26;
    var s = (bits - r) / 26;

    assert(this.negative === 0, 'imaskn works only with positive numbers');

    if (this.length <= s) {
      return this;
    }

    if (r !== 0) {
      s++;
    }
    this.length = Math.min(s, this.length);

    if (r !== 0) {
      var mask = 0x3ffffff ^ ((0x3ffffff >>> r) << r);
      this.words[this.length - 1] &= mask;
    }

    return this.strip();
  };

  // Return only lowers bits of number
  BN.prototype.maskn = function maskn (bits) {
    return this.clone().imaskn(bits);
  };

  // Add plain number `num` to `this`
  BN.prototype.iaddn = function iaddn (num) {
    assert(typeof num === 'number');
    assert(num < 0x4000000);
    if (num < 0) return this.isubn(-num);

    // Possible sign change
    if (this.negative !== 0) {
      if (this.length === 1 && (this.words[0] | 0) < num) {
        this.words[0] = num - (this.words[0] | 0);
        this.negative = 0;
        return this;
      }

      this.negative = 0;
      this.isubn(num);
      this.negative = 1;
      return this;
    }

    // Add without checks
    return this._iaddn(num);
  };

  BN.prototype._iaddn = function _iaddn (num) {
    this.words[0] += num;

    // Carry
    for (var i = 0; i < this.length && this.words[i] >= 0x4000000; i++) {
      this.words[i] -= 0x4000000;
      if (i === this.length - 1) {
        this.words[i + 1] = 1;
      } else {
        this.words[i + 1]++;
      }
    }
    this.length = Math.max(this.length, i + 1);

    return this;
  };

  // Subtract plain number `num` from `this`
  BN.prototype.isubn = function isubn (num) {
    assert(typeof num === 'number');
    assert(num < 0x4000000);
    if (num < 0) return this.iaddn(-num);

    if (this.negative !== 0) {
      this.negative = 0;
      this.iaddn(num);
      this.negative = 1;
      return this;
    }

    this.words[0] -= num;

    if (this.length === 1 && this.words[0] < 0) {
      this.words[0] = -this.words[0];
      this.negative = 1;
    } else {
      // Carry
      for (var i = 0; i < this.length && this.words[i] < 0; i++) {
        this.words[i] += 0x4000000;
        this.words[i + 1] -= 1;
      }
    }

    return this.strip();
  };

  BN.prototype.addn = function addn (num) {
    return this.clone().iaddn(num);
  };

  BN.prototype.subn = function subn (num) {
    return this.clone().isubn(num);
  };

  BN.prototype.iabs = function iabs () {
    this.negative = 0;

    return this;
  };

  BN.prototype.abs = function abs () {
    return this.clone().iabs();
  };

  BN.prototype._ishlnsubmul = function _ishlnsubmul (num, mul, shift) {
    var len = num.length + shift;
    var i;

    this._expand(len);

    var w;
    var carry = 0;
    for (i = 0; i < num.length; i++) {
      w = (this.words[i + shift] | 0) + carry;
      var right = (num.words[i] | 0) * mul;
      w -= right & 0x3ffffff;
      carry = (w >> 26) - ((right / 0x4000000) | 0);
      this.words[i + shift] = w & 0x3ffffff;
    }
    for (; i < this.length - shift; i++) {
      w = (this.words[i + shift] | 0) + carry;
      carry = w >> 26;
      this.words[i + shift] = w & 0x3ffffff;
    }

    if (carry === 0) return this.strip();

    // Subtraction overflow
    assert(carry === -1);
    carry = 0;
    for (i = 0; i < this.length; i++) {
      w = -(this.words[i] | 0) + carry;
      carry = w >> 26;
      this.words[i] = w & 0x3ffffff;
    }
    this.negative = 1;

    return this.strip();
  };

  BN.prototype._wordDiv = function _wordDiv (num, mode) {
    var shift = this.length - num.length;

    var a = this.clone();
    var b = num;

    // Normalize
    var bhi = b.words[b.length - 1] | 0;
    var bhiBits = this._countBits(bhi);
    shift = 26 - bhiBits;
    if (shift !== 0) {
      b = b.ushln(shift);
      a.iushln(shift);
      bhi = b.words[b.length - 1] | 0;
    }

    // Initialize quotient
    var m = a.length - b.length;
    var q;

    if (mode !== 'mod') {
      q = new BN(null);
      q.length = m + 1;
      q.words = new Array(q.length);
      for (var i = 0; i < q.length; i++) {
        q.words[i] = 0;
      }
    }

    var diff = a.clone()._ishlnsubmul(b, 1, m);
    if (diff.negative === 0) {
      a = diff;
      if (q) {
        q.words[m] = 1;
      }
    }

    for (var j = m - 1; j >= 0; j--) {
      var qj = (a.words[b.length + j] | 0) * 0x4000000 +
        (a.words[b.length + j - 1] | 0);

      // NOTE: (qj / bhi) is (0x3ffffff * 0x4000000 + 0x3ffffff) / 0x2000000 max
      // (0x7ffffff)
      qj = Math.min((qj / bhi) | 0, 0x3ffffff);

      a._ishlnsubmul(b, qj, j);
      while (a.negative !== 0) {
        qj--;
        a.negative = 0;
        a._ishlnsubmul(b, 1, j);
        if (!a.isZero()) {
          a.negative ^= 1;
        }
      }
      if (q) {
        q.words[j] = qj;
      }
    }
    if (q) {
      q.strip();
    }
    a.strip();

    // Denormalize
    if (mode !== 'div' && shift !== 0) {
      a.iushrn(shift);
    }

    return {
      div: q || null,
      mod: a
    };
  };

  // NOTE: 1) `mode` can be set to `mod` to request mod only,
  //       to `div` to request div only, or be absent to
  //       request both div & mod
  //       2) `positive` is true if unsigned mod is requested
  BN.prototype.divmod = function divmod (num, mode, positive) {
    assert(!num.isZero());

    if (this.isZero()) {
      return {
        div: new BN(0),
        mod: new BN(0)
      };
    }

    var div, mod, res;
    if (this.negative !== 0 && num.negative === 0) {
      res = this.neg().divmod(num, mode);

      if (mode !== 'mod') {
        div = res.div.neg();
      }

      if (mode !== 'div') {
        mod = res.mod.neg();
        if (positive && mod.negative !== 0) {
          mod.iadd(num);
        }
      }

      return {
        div: div,
        mod: mod
      };
    }

    if (this.negative === 0 && num.negative !== 0) {
      res = this.divmod(num.neg(), mode);

      if (mode !== 'mod') {
        div = res.div.neg();
      }

      return {
        div: div,
        mod: res.mod
      };
    }

    if ((this.negative & num.negative) !== 0) {
      res = this.neg().divmod(num.neg(), mode);

      if (mode !== 'div') {
        mod = res.mod.neg();
        if (positive && mod.negative !== 0) {
          mod.isub(num);
        }
      }

      return {
        div: res.div,
        mod: mod
      };
    }

    // Both numbers are positive at this point

    // Strip both numbers to approximate shift value
    if (num.length > this.length || this.cmp(num) < 0) {
      return {
        div: new BN(0),
        mod: this
      };
    }

    // Very short reduction
    if (num.length === 1) {
      if (mode === 'div') {
        return {
          div: this.divn(num.words[0]),
          mod: null
        };
      }

      if (mode === 'mod') {
        return {
          div: null,
          mod: new BN(this.modn(num.words[0]))
        };
      }

      return {
        div: this.divn(num.words[0]),
        mod: new BN(this.modn(num.words[0]))
      };
    }

    return this._wordDiv(num, mode);
  };

  // Find `this` / `num`
  BN.prototype.div = function div (num) {
    return this.divmod(num, 'div', false).div;
  };

  // Find `this` % `num`
  BN.prototype.mod = function mod (num) {
    return this.divmod(num, 'mod', false).mod;
  };

  BN.prototype.umod = function umod (num) {
    return this.divmod(num, 'mod', true).mod;
  };

  // Find Round(`this` / `num`)
  BN.prototype.divRound = function divRound (num) {
    var dm = this.divmod(num);

    // Fast case - exact division
    if (dm.mod.isZero()) return dm.div;

    var mod = dm.div.negative !== 0 ? dm.mod.isub(num) : dm.mod;

    var half = num.ushrn(1);
    var r2 = num.andln(1);
    var cmp = mod.cmp(half);

    // Round down
    if (cmp < 0 || r2 === 1 && cmp === 0) return dm.div;

    // Round up
    return dm.div.negative !== 0 ? dm.div.isubn(1) : dm.div.iaddn(1);
  };

  BN.prototype.modn = function modn (num) {
    assert(num <= 0x3ffffff);
    var p = (1 << 26) % num;

    var acc = 0;
    for (var i = this.length - 1; i >= 0; i--) {
      acc = (p * acc + (this.words[i] | 0)) % num;
    }

    return acc;
  };

  // In-place division by number
  BN.prototype.idivn = function idivn (num) {
    assert(num <= 0x3ffffff);

    var carry = 0;
    for (var i = this.length - 1; i >= 0; i--) {
      var w = (this.words[i] | 0) + carry * 0x4000000;
      this.words[i] = (w / num) | 0;
      carry = w % num;
    }

    return this.strip();
  };

  BN.prototype.divn = function divn (num) {
    return this.clone().idivn(num);
  };

  BN.prototype.egcd = function egcd (p) {
    assert(p.negative === 0);
    assert(!p.isZero());

    var x = this;
    var y = p.clone();

    if (x.negative !== 0) {
      x = x.umod(p);
    } else {
      x = x.clone();
    }

    // A * x + B * y = x
    var A = new BN(1);
    var B = new BN(0);

    // C * x + D * y = y
    var C = new BN(0);
    var D = new BN(1);

    var g = 0;

    while (x.isEven() && y.isEven()) {
      x.iushrn(1);
      y.iushrn(1);
      ++g;
    }

    var yp = y.clone();
    var xp = x.clone();

    while (!x.isZero()) {
      for (var i = 0, im = 1; (x.words[0] & im) === 0 && i < 26; ++i, im <<= 1);
      if (i > 0) {
        x.iushrn(i);
        while (i-- > 0) {
          if (A.isOdd() || B.isOdd()) {
            A.iadd(yp);
            B.isub(xp);
          }

          A.iushrn(1);
          B.iushrn(1);
        }
      }

      for (var j = 0, jm = 1; (y.words[0] & jm) === 0 && j < 26; ++j, jm <<= 1);
      if (j > 0) {
        y.iushrn(j);
        while (j-- > 0) {
          if (C.isOdd() || D.isOdd()) {
            C.iadd(yp);
            D.isub(xp);
          }

          C.iushrn(1);
          D.iushrn(1);
        }
      }

      if (x.cmp(y) >= 0) {
        x.isub(y);
        A.isub(C);
        B.isub(D);
      } else {
        y.isub(x);
        C.isub(A);
        D.isub(B);
      }
    }

    return {
      a: C,
      b: D,
      gcd: y.iushln(g)
    };
  };

  // This is reduced incarnation of the binary EEA
  // above, designated to invert members of the
  // _prime_ fields F(p) at a maximal speed
  BN.prototype._invmp = function _invmp (p) {
    assert(p.negative === 0);
    assert(!p.isZero());

    var a = this;
    var b = p.clone();

    if (a.negative !== 0) {
      a = a.umod(p);
    } else {
      a = a.clone();
    }

    var x1 = new BN(1);
    var x2 = new BN(0);

    var delta = b.clone();

    while (a.cmpn(1) > 0 && b.cmpn(1) > 0) {
      for (var i = 0, im = 1; (a.words[0] & im) === 0 && i < 26; ++i, im <<= 1);
      if (i > 0) {
        a.iushrn(i);
        while (i-- > 0) {
          if (x1.isOdd()) {
            x1.iadd(delta);
          }

          x1.iushrn(1);
        }
      }

      for (var j = 0, jm = 1; (b.words[0] & jm) === 0 && j < 26; ++j, jm <<= 1);
      if (j > 0) {
        b.iushrn(j);
        while (j-- > 0) {
          if (x2.isOdd()) {
            x2.iadd(delta);
          }

          x2.iushrn(1);
        }
      }

      if (a.cmp(b) >= 0) {
        a.isub(b);
        x1.isub(x2);
      } else {
        b.isub(a);
        x2.isub(x1);
      }
    }

    var res;
    if (a.cmpn(1) === 0) {
      res = x1;
    } else {
      res = x2;
    }

    if (res.cmpn(0) < 0) {
      res.iadd(p);
    }

    return res;
  };

  BN.prototype.gcd = function gcd (num) {
    if (this.isZero()) return num.abs();
    if (num.isZero()) return this.abs();

    var a = this.clone();
    var b = num.clone();
    a.negative = 0;
    b.negative = 0;

    // Remove common factor of two
    for (var shift = 0; a.isEven() && b.isEven(); shift++) {
      a.iushrn(1);
      b.iushrn(1);
    }

    do {
      while (a.isEven()) {
        a.iushrn(1);
      }
      while (b.isEven()) {
        b.iushrn(1);
      }

      var r = a.cmp(b);
      if (r < 0) {
        // Swap `a` and `b` to make `a` always bigger than `b`
        var t = a;
        a = b;
        b = t;
      } else if (r === 0 || b.cmpn(1) === 0) {
        break;
      }

      a.isub(b);
    } while (true);

    return b.iushln(shift);
  };

  // Invert number in the field F(num)
  BN.prototype.invm = function invm (num) {
    return this.egcd(num).a.umod(num);
  };

  BN.prototype.isEven = function isEven () {
    return (this.words[0] & 1) === 0;
  };

  BN.prototype.isOdd = function isOdd () {
    return (this.words[0] & 1) === 1;
  };

  // And first word and num
  BN.prototype.andln = function andln (num) {
    return this.words[0] & num;
  };

  // Increment at the bit position in-line
  BN.prototype.bincn = function bincn (bit) {
    assert(typeof bit === 'number');
    var r = bit % 26;
    var s = (bit - r) / 26;
    var q = 1 << r;

    // Fast case: bit is much higher than all existing words
    if (this.length <= s) {
      this._expand(s + 1);
      this.words[s] |= q;
      return this;
    }

    // Add bit and propagate, if needed
    var carry = q;
    for (var i = s; carry !== 0 && i < this.length; i++) {
      var w = this.words[i] | 0;
      w += carry;
      carry = w >>> 26;
      w &= 0x3ffffff;
      this.words[i] = w;
    }
    if (carry !== 0) {
      this.words[i] = carry;
      this.length++;
    }
    return this;
  };

  BN.prototype.isZero = function isZero () {
    return this.length === 1 && this.words[0] === 0;
  };

  BN.prototype.cmpn = function cmpn (num) {
    var negative = num < 0;

    if (this.negative !== 0 && !negative) return -1;
    if (this.negative === 0 && negative) return 1;

    this.strip();

    var res;
    if (this.length > 1) {
      res = 1;
    } else {
      if (negative) {
        num = -num;
      }

      assert(num <= 0x3ffffff, 'Number is too big');

      var w = this.words[0] | 0;
      res = w === num ? 0 : w < num ? -1 : 1;
    }
    if (this.negative !== 0) return -res | 0;
    return res;
  };

  // Compare two numbers and return:
  // 1 - if `this` > `num`
  // 0 - if `this` == `num`
  // -1 - if `this` < `num`
  BN.prototype.cmp = function cmp (num) {
    if (this.negative !== 0 && num.negative === 0) return -1;
    if (this.negative === 0 && num.negative !== 0) return 1;

    var res = this.ucmp(num);
    if (this.negative !== 0) return -res | 0;
    return res;
  };

  // Unsigned comparison
  BN.prototype.ucmp = function ucmp (num) {
    // At this point both numbers have the same sign
    if (this.length > num.length) return 1;
    if (this.length < num.length) return -1;

    var res = 0;
    for (var i = this.length - 1; i >= 0; i--) {
      var a = this.words[i] | 0;
      var b = num.words[i] | 0;

      if (a === b) continue;
      if (a < b) {
        res = -1;
      } else if (a > b) {
        res = 1;
      }
      break;
    }
    return res;
  };

  BN.prototype.gtn = function gtn (num) {
    return this.cmpn(num) === 1;
  };

  BN.prototype.gt = function gt (num) {
    return this.cmp(num) === 1;
  };

  BN.prototype.gten = function gten (num) {
    return this.cmpn(num) >= 0;
  };

  BN.prototype.gte = function gte (num) {
    return this.cmp(num) >= 0;
  };

  BN.prototype.ltn = function ltn (num) {
    return this.cmpn(num) === -1;
  };

  BN.prototype.lt = function lt (num) {
    return this.cmp(num) === -1;
  };

  BN.prototype.lten = function lten (num) {
    return this.cmpn(num) <= 0;
  };

  BN.prototype.lte = function lte (num) {
    return this.cmp(num) <= 0;
  };

  BN.prototype.eqn = function eqn (num) {
    return this.cmpn(num) === 0;
  };

  BN.prototype.eq = function eq (num) {
    return this.cmp(num) === 0;
  };

  //
  // A reduce context, could be using montgomery or something better, depending
  // on the `m` itself.
  //
  BN.red = function red (num) {
    return new Red(num);
  };

  BN.prototype.toRed = function toRed (ctx) {
    assert(!this.red, 'Already a number in reduction context');
    assert(this.negative === 0, 'red works only with positives');
    return ctx.convertTo(this)._forceRed(ctx);
  };

  BN.prototype.fromRed = function fromRed () {
    assert(this.red, 'fromRed works only with numbers in reduction context');
    return this.red.convertFrom(this);
  };

  BN.prototype._forceRed = function _forceRed (ctx) {
    this.red = ctx;
    return this;
  };

  BN.prototype.forceRed = function forceRed (ctx) {
    assert(!this.red, 'Already a number in reduction context');
    return this._forceRed(ctx);
  };

  BN.prototype.redAdd = function redAdd (num) {
    assert(this.red, 'redAdd works only with red numbers');
    return this.red.add(this, num);
  };

  BN.prototype.redIAdd = function redIAdd (num) {
    assert(this.red, 'redIAdd works only with red numbers');
    return this.red.iadd(this, num);
  };

  BN.prototype.redSub = function redSub (num) {
    assert(this.red, 'redSub works only with red numbers');
    return this.red.sub(this, num);
  };

  BN.prototype.redISub = function redISub (num) {
    assert(this.red, 'redISub works only with red numbers');
    return this.red.isub(this, num);
  };

  BN.prototype.redShl = function redShl (num) {
    assert(this.red, 'redShl works only with red numbers');
    return this.red.shl(this, num);
  };

  BN.prototype.redMul = function redMul (num) {
    assert(this.red, 'redMul works only with red numbers');
    this.red._verify2(this, num);
    return this.red.mul(this, num);
  };

  BN.prototype.redIMul = function redIMul (num) {
    assert(this.red, 'redMul works only with red numbers');
    this.red._verify2(this, num);
    return this.red.imul(this, num);
  };

  BN.prototype.redSqr = function redSqr () {
    assert(this.red, 'redSqr works only with red numbers');
    this.red._verify1(this);
    return this.red.sqr(this);
  };

  BN.prototype.redISqr = function redISqr () {
    assert(this.red, 'redISqr works only with red numbers');
    this.red._verify1(this);
    return this.red.isqr(this);
  };

  // Square root over p
  BN.prototype.redSqrt = function redSqrt () {
    assert(this.red, 'redSqrt works only with red numbers');
    this.red._verify1(this);
    return this.red.sqrt(this);
  };

  BN.prototype.redInvm = function redInvm () {
    assert(this.red, 'redInvm works only with red numbers');
    this.red._verify1(this);
    return this.red.invm(this);
  };

  // Return negative clone of `this` % `red modulo`
  BN.prototype.redNeg = function redNeg () {
    assert(this.red, 'redNeg works only with red numbers');
    this.red._verify1(this);
    return this.red.neg(this);
  };

  BN.prototype.redPow = function redPow (num) {
    assert(this.red && !num.red, 'redPow(normalNum)');
    this.red._verify1(this);
    return this.red.pow(this, num);
  };

  // Prime numbers with efficient reduction
  var primes = {
    k256: null,
    p224: null,
    p192: null,
    p25519: null
  };

  // Pseudo-Mersenne prime
  function MPrime (name, p) {
    // P = 2 ^ N - K
    this.name = name;
    this.p = new BN(p, 16);
    this.n = this.p.bitLength();
    this.k = new BN(1).iushln(this.n).isub(this.p);

    this.tmp = this._tmp();
  }

  MPrime.prototype._tmp = function _tmp () {
    var tmp = new BN(null);
    tmp.words = new Array(Math.ceil(this.n / 13));
    return tmp;
  };

  MPrime.prototype.ireduce = function ireduce (num) {
    // Assumes that `num` is less than `P^2`
    // num = HI * (2 ^ N - K) + HI * K + LO = HI * K + LO (mod P)
    var r = num;
    var rlen;

    do {
      this.split(r, this.tmp);
      r = this.imulK(r);
      r = r.iadd(this.tmp);
      rlen = r.bitLength();
    } while (rlen > this.n);

    var cmp = rlen < this.n ? -1 : r.ucmp(this.p);
    if (cmp === 0) {
      r.words[0] = 0;
      r.length = 1;
    } else if (cmp > 0) {
      r.isub(this.p);
    } else {
      if (r.strip !== undefined) {
        // r is BN v4 instance
        r.strip();
      } else {
        // r is BN v5 instance
        r._strip();
      }
    }

    return r;
  };

  MPrime.prototype.split = function split (input, out) {
    input.iushrn(this.n, 0, out);
  };

  MPrime.prototype.imulK = function imulK (num) {
    return num.imul(this.k);
  };

  function K256 () {
    MPrime.call(
      this,
      'k256',
      'ffffffff ffffffff ffffffff ffffffff ffffffff ffffffff fffffffe fffffc2f');
  }
  inherits(K256, MPrime);

  K256.prototype.split = function split (input, output) {
    // 256 = 9 * 26 + 22
    var mask = 0x3fffff;

    var outLen = Math.min(input.length, 9);
    for (var i = 0; i < outLen; i++) {
      output.words[i] = input.words[i];
    }
    output.length = outLen;

    if (input.length <= 9) {
      input.words[0] = 0;
      input.length = 1;
      return;
    }

    // Shift by 9 limbs
    var prev = input.words[9];
    output.words[output.length++] = prev & mask;

    for (i = 10; i < input.length; i++) {
      var next = input.words[i] | 0;
      input.words[i - 10] = ((next & mask) << 4) | (prev >>> 22);
      prev = next;
    }
    prev >>>= 22;
    input.words[i - 10] = prev;
    if (prev === 0 && input.length > 10) {
      input.length -= 10;
    } else {
      input.length -= 9;
    }
  };

  K256.prototype.imulK = function imulK (num) {
    // K = 0x1000003d1 = [ 0x40, 0x3d1 ]
    num.words[num.length] = 0;
    num.words[num.length + 1] = 0;
    num.length += 2;

    // bounded at: 0x40 * 0x3ffffff + 0x3d0 = 0x100000390
    var lo = 0;
    for (var i = 0; i < num.length; i++) {
      var w = num.words[i] | 0;
      lo += w * 0x3d1;
      num.words[i] = lo & 0x3ffffff;
      lo = w * 0x40 + ((lo / 0x4000000) | 0);
    }

    // Fast length reduction
    if (num.words[num.length - 1] === 0) {
      num.length--;
      if (num.words[num.length - 1] === 0) {
        num.length--;
      }
    }
    return num;
  };

  function P224 () {
    MPrime.call(
      this,
      'p224',
      'ffffffff ffffffff ffffffff ffffffff 00000000 00000000 00000001');
  }
  inherits(P224, MPrime);

  function P192 () {
    MPrime.call(
      this,
      'p192',
      'ffffffff ffffffff ffffffff fffffffe ffffffff ffffffff');
  }
  inherits(P192, MPrime);

  function P25519 () {
    // 2 ^ 255 - 19
    MPrime.call(
      this,
      '25519',
      '7fffffffffffffff ffffffffffffffff ffffffffffffffff ffffffffffffffed');
  }
  inherits(P25519, MPrime);

  P25519.prototype.imulK = function imulK (num) {
    // K = 0x13
    var carry = 0;
    for (var i = 0; i < num.length; i++) {
      var hi = (num.words[i] | 0) * 0x13 + carry;
      var lo = hi & 0x3ffffff;
      hi >>>= 26;

      num.words[i] = lo;
      carry = hi;
    }
    if (carry !== 0) {
      num.words[num.length++] = carry;
    }
    return num;
  };

  // Exported mostly for testing purposes, use plain name instead
  BN._prime = function prime (name) {
    // Cached version of prime
    if (primes[name]) return primes[name];

    var prime;
    if (name === 'k256') {
      prime = new K256();
    } else if (name === 'p224') {
      prime = new P224();
    } else if (name === 'p192') {
      prime = new P192();
    } else if (name === 'p25519') {
      prime = new P25519();
    } else {
      throw new Error('Unknown prime ' + name);
    }
    primes[name] = prime;

    return prime;
  };

  //
  // Base reduction engine
  //
  function Red (m) {
    if (typeof m === 'string') {
      var prime = BN._prime(m);
      this.m = prime.p;
      this.prime = prime;
    } else {
      assert(m.gtn(1), 'modulus must be greater than 1');
      this.m = m;
      this.prime = null;
    }
  }

  Red.prototype._verify1 = function _verify1 (a) {
    assert(a.negative === 0, 'red works only with positives');
    assert(a.red, 'red works only with red numbers');
  };

  Red.prototype._verify2 = function _verify2 (a, b) {
    assert((a.negative | b.negative) === 0, 'red works only with positives');
    assert(a.red && a.red === b.red,
      'red works only with red numbers');
  };

  Red.prototype.imod = function imod (a) {
    if (this.prime) return this.prime.ireduce(a)._forceRed(this);
    return a.umod(this.m)._forceRed(this);
  };

  Red.prototype.neg = function neg (a) {
    if (a.isZero()) {
      return a.clone();
    }

    return this.m.sub(a)._forceRed(this);
  };

  Red.prototype.add = function add (a, b) {
    this._verify2(a, b);

    var res = a.add(b);
    if (res.cmp(this.m) >= 0) {
      res.isub(this.m);
    }
    return res._forceRed(this);
  };

  Red.prototype.iadd = function iadd (a, b) {
    this._verify2(a, b);

    var res = a.iadd(b);
    if (res.cmp(this.m) >= 0) {
      res.isub(this.m);
    }
    return res;
  };

  Red.prototype.sub = function sub (a, b) {
    this._verify2(a, b);

    var res = a.sub(b);
    if (res.cmpn(0) < 0) {
      res.iadd(this.m);
    }
    return res._forceRed(this);
  };

  Red.prototype.isub = function isub (a, b) {
    this._verify2(a, b);

    var res = a.isub(b);
    if (res.cmpn(0) < 0) {
      res.iadd(this.m);
    }
    return res;
  };

  Red.prototype.shl = function shl (a, num) {
    this._verify1(a);
    return this.imod(a.ushln(num));
  };

  Red.prototype.imul = function imul (a, b) {
    this._verify2(a, b);
    return this.imod(a.imul(b));
  };

  Red.prototype.mul = function mul (a, b) {
    this._verify2(a, b);
    return this.imod(a.mul(b));
  };

  Red.prototype.isqr = function isqr (a) {
    return this.imul(a, a.clone());
  };

  Red.prototype.sqr = function sqr (a) {
    return this.mul(a, a);
  };

  Red.prototype.sqrt = function sqrt (a) {
    if (a.isZero()) return a.clone();

    var mod3 = this.m.andln(3);
    assert(mod3 % 2 === 1);

    // Fast case
    if (mod3 === 3) {
      var pow = this.m.add(new BN(1)).iushrn(2);
      return this.pow(a, pow);
    }

    // Tonelli-Shanks algorithm (Totally unoptimized and slow)
    //
    // Find Q and S, that Q * 2 ^ S = (P - 1)
    var q = this.m.subn(1);
    var s = 0;
    while (!q.isZero() && q.andln(1) === 0) {
      s++;
      q.iushrn(1);
    }
    assert(!q.isZero());

    var one = new BN(1).toRed(this);
    var nOne = one.redNeg();

    // Find quadratic non-residue
    // NOTE: Max is such because of generalized Riemann hypothesis.
    var lpow = this.m.subn(1).iushrn(1);
    var z = this.m.bitLength();
    z = new BN(2 * z * z).toRed(this);

    while (this.pow(z, lpow).cmp(nOne) !== 0) {
      z.redIAdd(nOne);
    }

    var c = this.pow(z, q);
    var r = this.pow(a, q.addn(1).iushrn(1));
    var t = this.pow(a, q);
    var m = s;
    while (t.cmp(one) !== 0) {
      var tmp = t;
      for (var i = 0; tmp.cmp(one) !== 0; i++) {
        tmp = tmp.redSqr();
      }
      assert(i < m);
      var b = this.pow(c, new BN(1).iushln(m - i - 1));

      r = r.redMul(b);
      c = b.redSqr();
      t = t.redMul(c);
      m = i;
    }

    return r;
  };

  Red.prototype.invm = function invm (a) {
    var inv = a._invmp(this.m);
    if (inv.negative !== 0) {
      inv.negative = 0;
      return this.imod(inv).redNeg();
    } else {
      return this.imod(inv);
    }
  };

  Red.prototype.pow = function pow (a, num) {
    if (num.isZero()) return new BN(1).toRed(this);
    if (num.cmpn(1) === 0) return a.clone();

    var windowSize = 4;
    var wnd = new Array(1 << windowSize);
    wnd[0] = new BN(1).toRed(this);
    wnd[1] = a;
    for (var i = 2; i < wnd.length; i++) {
      wnd[i] = this.mul(wnd[i - 1], a);
    }

    var res = wnd[0];
    var current = 0;
    var currentLen = 0;
    var start = num.bitLength() % 26;
    if (start === 0) {
      start = 26;
    }

    for (i = num.length - 1; i >= 0; i--) {
      var word = num.words[i];
      for (var j = start - 1; j >= 0; j--) {
        var bit = (word >> j) & 1;
        if (res !== wnd[0]) {
          res = this.sqr(res);
        }

        if (bit === 0 && current === 0) {
          currentLen = 0;
          continue;
        }

        current <<= 1;
        current |= bit;
        currentLen++;
        if (currentLen !== windowSize && (i !== 0 || j !== 0)) continue;

        res = this.mul(res, wnd[current]);
        currentLen = 0;
        current = 0;
      }
      start = 26;
    }

    return res;
  };

  Red.prototype.convertTo = function convertTo (num) {
    var r = num.umod(this.m);

    return r === num ? r.clone() : r;
  };

  Red.prototype.convertFrom = function convertFrom (num) {
    var res = num.clone();
    res.red = null;
    return res;
  };

  //
  // Montgomery method engine
  //

  BN.mont = function mont (num) {
    return new Mont(num);
  };

  function Mont (m) {
    Red.call(this, m);

    this.shift = this.m.bitLength();
    if (this.shift % 26 !== 0) {
      this.shift += 26 - (this.shift % 26);
    }

    this.r = new BN(1).iushln(this.shift);
    this.r2 = this.imod(this.r.sqr());
    this.rinv = this.r._invmp(this.m);

    this.minv = this.rinv.mul(this.r).isubn(1).div(this.m);
    this.minv = this.minv.umod(this.r);
    this.minv = this.r.sub(this.minv);
  }
  inherits(Mont, Red);

  Mont.prototype.convertTo = function convertTo (num) {
    return this.imod(num.ushln(this.shift));
  };

  Mont.prototype.convertFrom = function convertFrom (num) {
    var r = this.imod(num.mul(this.rinv));
    r.red = null;
    return r;
  };

  Mont.prototype.imul = function imul (a, b) {
    if (a.isZero() || b.isZero()) {
      a.words[0] = 0;
      a.length = 1;
      return a;
    }

    var t = a.imul(b);
    var c = t.maskn(this.shift).mul(this.minv).imaskn(this.shift).mul(this.m);
    var u = t.isub(c).iushrn(this.shift);
    var res = u;

    if (u.cmp(this.m) >= 0) {
      res = u.isub(this.m);
    } else if (u.cmpn(0) < 0) {
      res = u.iadd(this.m);
    }

    return res._forceRed(this);
  };

  Mont.prototype.mul = function mul (a, b) {
    if (a.isZero() || b.isZero()) return new BN(0)._forceRed(this);

    var t = a.mul(b);
    var c = t.maskn(this.shift).mul(this.minv).imaskn(this.shift).mul(this.m);
    var u = t.isub(c).iushrn(this.shift);
    var res = u;
    if (u.cmp(this.m) >= 0) {
      res = u.isub(this.m);
    } else if (u.cmpn(0) < 0) {
      res = u.iadd(this.m);
    }

    return res._forceRed(this);
  };

  Mont.prototype.invm = function invm (a) {
    // (AR)^-1 * R^2 = (A^-1 * R^-1) * R^2 = A^-1 * R
    var res = this.imod(a._invmp(this.m).mul(this.r2));
    return res._forceRed(this);
  };
})( false || module, this);


/***/ }),
/* 8 */
/***/ (function() {

/* (ignored) */

/***/ }),
/* 9 */
/***/ (function(__unused_webpack_module, exports, __webpack_require__) {

var asn1 = __webpack_require__(6);
var inherits = __webpack_require__(10);

var api = exports;

api.define = function define(name, body) {
  return new Entity(name, body);
};

function Entity(name, body) {
  this.name = name;
  this.body = body;

  this.decoders = {};
  this.encoders = {};
};

Entity.prototype._createNamed = function createNamed(base) {
  var named;
  try {
    named = (__webpack_require__(11).runInThisContext)(
      '(function ' + this.name + '(entity) {\n' +
      '  this._initNamed(entity);\n' +
      '})'
    );
  } catch (e) {
    named = function (entity) {
      this._initNamed(entity);
    };
  }
  inherits(named, base);
  named.prototype._initNamed = function initnamed(entity) {
    base.call(this, entity);
  };

  return new named(this);
};

Entity.prototype._getDecoder = function _getDecoder(enc) {
  enc = enc || 'der';
  // Lazily create decoder
  if (!this.decoders.hasOwnProperty(enc))
    this.decoders[enc] = this._createNamed(asn1.decoders[enc]);
  return this.decoders[enc];
};

Entity.prototype.decode = function decode(data, enc, options) {
  return this._getDecoder(enc).decode(data, options);
};

Entity.prototype._getEncoder = function _getEncoder(enc) {
  enc = enc || 'der';
  // Lazily create encoder
  if (!this.encoders.hasOwnProperty(enc))
    this.encoders[enc] = this._createNamed(asn1.encoders[enc]);
  return this.encoders[enc];
};

Entity.prototype.encode = function encode(data, enc, /* internal */ reporter) {
  return this._getEncoder(enc).encode(data, reporter);
};


/***/ }),
/* 10 */
/***/ (function(module) {

if (typeof Object.create === 'function') {
  // implementation from standard node.js 'util' module
  module.exports = function inherits(ctor, superCtor) {
    if (superCtor) {
      ctor.super_ = superCtor
      ctor.prototype = Object.create(superCtor.prototype, {
        constructor: {
          value: ctor,
          enumerable: false,
          writable: true,
          configurable: true
        }
      })
    }
  };
} else {
  // old school shim for old browsers
  module.exports = function inherits(ctor, superCtor) {
    if (superCtor) {
      ctor.super_ = superCtor
      var TempCtor = function () {}
      TempCtor.prototype = superCtor.prototype
      ctor.prototype = new TempCtor()
      ctor.prototype.constructor = ctor
    }
  }
}


/***/ }),
/* 11 */
/***/ (function(__unused_webpack_module, exports) {

var indexOf = function (xs, item) {
    if (xs.indexOf) return xs.indexOf(item);
    else for (var i = 0; i < xs.length; i++) {
        if (xs[i] === item) return i;
    }
    return -1;
};
var Object_keys = function (obj) {
    if (Object.keys) return Object.keys(obj)
    else {
        var res = [];
        for (var key in obj) res.push(key)
        return res;
    }
};

var forEach = function (xs, fn) {
    if (xs.forEach) return xs.forEach(fn)
    else for (var i = 0; i < xs.length; i++) {
        fn(xs[i], i, xs);
    }
};

var defineProp = (function() {
    try {
        Object.defineProperty({}, '_', {});
        return function(obj, name, value) {
            Object.defineProperty(obj, name, {
                writable: true,
                enumerable: false,
                configurable: true,
                value: value
            })
        };
    } catch(e) {
        return function(obj, name, value) {
            obj[name] = value;
        };
    }
}());

var globals = ['Array', 'Boolean', 'Date', 'Error', 'EvalError', 'Function',
'Infinity', 'JSON', 'Math', 'NaN', 'Number', 'Object', 'RangeError',
'ReferenceError', 'RegExp', 'String', 'SyntaxError', 'TypeError', 'URIError',
'decodeURI', 'decodeURIComponent', 'encodeURI', 'encodeURIComponent', 'escape',
'eval', 'isFinite', 'isNaN', 'parseFloat', 'parseInt', 'undefined', 'unescape'];

function Context() {}
Context.prototype = {};

var Script = exports.Script = function NodeScript (code) {
    if (!(this instanceof Script)) return new Script(code);
    this.code = code;
};

Script.prototype.runInContext = function (context) {
    if (!(context instanceof Context)) {
        throw new TypeError("needs a 'context' argument.");
    }
    
    var iframe = document.createElement('iframe');
    if (!iframe.style) iframe.style = {};
    iframe.style.display = 'none';
    
    document.body.appendChild(iframe);
    
    var win = iframe.contentWindow;
    var wEval = win.eval, wExecScript = win.execScript;

    if (!wEval && wExecScript) {
        // win.eval() magically appears when this is called in IE:
        wExecScript.call(win, 'null');
        wEval = win.eval;
    }
    
    forEach(Object_keys(context), function (key) {
        win[key] = context[key];
    });
    forEach(globals, function (key) {
        if (context[key]) {
            win[key] = context[key];
        }
    });
    
    var winKeys = Object_keys(win);

    var res = wEval.call(win, this.code);
    
    forEach(Object_keys(win), function (key) {
        // Avoid copying circular objects like `top` and `window` by only
        // updating existing context properties or new properties in the `win`
        // that was only introduced after the eval.
        if (key in context || indexOf(winKeys, key) === -1) {
            context[key] = win[key];
        }
    });

    forEach(globals, function (key) {
        if (!(key in context)) {
            defineProp(context, key, win[key]);
        }
    });
    
    document.body.removeChild(iframe);
    
    return res;
};

Script.prototype.runInThisContext = function () {
    return eval(this.code); // maybe...
};

Script.prototype.runInNewContext = function (context) {
    var ctx = Script.createContext(context);
    var res = this.runInContext(ctx);

    if (context) {
        forEach(Object_keys(ctx), function (key) {
            context[key] = ctx[key];
        });
    }

    return res;
};

forEach(Object_keys(Script.prototype), function (name) {
    exports[name] = Script[name] = function (code) {
        var s = Script(code);
        return s[name].apply(s, [].slice.call(arguments, 1));
    };
});

exports.isContext = function (context) {
    return context instanceof Context;
};

exports.createScript = function (code) {
    return exports.Script(code);
};

exports.createContext = Script.createContext = function (context) {
    var copy = new Context();
    if(typeof context === 'object') {
        forEach(Object_keys(context), function (key) {
            copy[key] = context[key];
        });
    }
    return copy;
};


/***/ }),
/* 12 */
/***/ (function(__unused_webpack_module, exports, __webpack_require__) {

var base = exports;

base.Reporter = (__webpack_require__(13).Reporter);
base.DecoderBuffer = (__webpack_require__(14).DecoderBuffer);
base.EncoderBuffer = (__webpack_require__(14).EncoderBuffer);
base.Node = __webpack_require__(18);


/***/ }),
/* 13 */
/***/ (function(__unused_webpack_module, exports, __webpack_require__) {

var inherits = __webpack_require__(10);

function Reporter(options) {
  this._reporterState = {
    obj: null,
    path: [],
    options: options || {},
    errors: []
  };
}
exports.Reporter = Reporter;

Reporter.prototype.isError = function isError(obj) {
  return obj instanceof ReporterError;
};

Reporter.prototype.save = function save() {
  var state = this._reporterState;

  return { obj: state.obj, pathLen: state.path.length };
};

Reporter.prototype.restore = function restore(data) {
  var state = this._reporterState;

  state.obj = data.obj;
  state.path = state.path.slice(0, data.pathLen);
};

Reporter.prototype.enterKey = function enterKey(key) {
  return this._reporterState.path.push(key);
};

Reporter.prototype.exitKey = function exitKey(index) {
  var state = this._reporterState;

  state.path = state.path.slice(0, index - 1);
};

Reporter.prototype.leaveKey = function leaveKey(index, key, value) {
  var state = this._reporterState;

  this.exitKey(index);
  if (state.obj !== null)
    state.obj[key] = value;
};

Reporter.prototype.path = function path() {
  return this._reporterState.path.join('/');
};

Reporter.prototype.enterObject = function enterObject() {
  var state = this._reporterState;

  var prev = state.obj;
  state.obj = {};
  return prev;
};

Reporter.prototype.leaveObject = function leaveObject(prev) {
  var state = this._reporterState;

  var now = state.obj;
  state.obj = prev;
  return now;
};

Reporter.prototype.error = function error(msg) {
  var err;
  var state = this._reporterState;

  var inherited = msg instanceof ReporterError;
  if (inherited) {
    err = msg;
  } else {
    err = new ReporterError(state.path.map(function(elem) {
      return '[' + JSON.stringify(elem) + ']';
    }).join(''), msg.message || msg, msg.stack);
  }

  if (!state.options.partial)
    throw err;

  if (!inherited)
    state.errors.push(err);

  return err;
};

Reporter.prototype.wrapResult = function wrapResult(result) {
  var state = this._reporterState;
  if (!state.options.partial)
    return result;

  return {
    result: this.isError(result) ? null : result,
    errors: state.errors
  };
};

function ReporterError(path, msg) {
  this.path = path;
  this.rethrow(msg);
};
inherits(ReporterError, Error);

ReporterError.prototype.rethrow = function rethrow(msg) {
  this.message = msg + ' at: ' + (this.path || '(shallow)');
  if (Error.captureStackTrace)
    Error.captureStackTrace(this, ReporterError);

  if (!this.stack) {
    try {
      // IE only adds stack when thrown
      throw new Error(this.message);
    } catch (e) {
      this.stack = e.stack;
    }
  }
  return this;
};


/***/ }),
/* 14 */
/***/ (function(__unused_webpack_module, exports, __webpack_require__) {

var inherits = __webpack_require__(10);
var Reporter = (__webpack_require__(12).Reporter);
var Buffer = (__webpack_require__(15).Buffer);

function DecoderBuffer(base, options) {
  Reporter.call(this, options);
  if (!Buffer.isBuffer(base)) {
    this.error('Input not Buffer');
    return;
  }

  this.base = base;
  this.offset = 0;
  this.length = base.length;
}
inherits(DecoderBuffer, Reporter);
exports.DecoderBuffer = DecoderBuffer;

DecoderBuffer.prototype.save = function save() {
  return { offset: this.offset, reporter: Reporter.prototype.save.call(this) };
};

DecoderBuffer.prototype.restore = function restore(save) {
  // Return skipped data
  var res = new DecoderBuffer(this.base);
  res.offset = save.offset;
  res.length = this.offset;

  this.offset = save.offset;
  Reporter.prototype.restore.call(this, save.reporter);

  return res;
};

DecoderBuffer.prototype.isEmpty = function isEmpty() {
  return this.offset === this.length;
};

DecoderBuffer.prototype.readUInt8 = function readUInt8(fail) {
  if (this.offset + 1 <= this.length)
    return this.base.readUInt8(this.offset++, true);
  else
    return this.error(fail || 'DecoderBuffer overrun');
}

DecoderBuffer.prototype.skip = function skip(bytes, fail) {
  if (!(this.offset + bytes <= this.length))
    return this.error(fail || 'DecoderBuffer overrun');

  var res = new DecoderBuffer(this.base);

  // Share reporter state
  res._reporterState = this._reporterState;

  res.offset = this.offset;
  res.length = this.offset + bytes;
  this.offset += bytes;
  return res;
}

DecoderBuffer.prototype.raw = function raw(save) {
  return this.base.slice(save ? save.offset : this.offset, this.length);
}

function EncoderBuffer(value, reporter) {
  if (Array.isArray(value)) {
    this.length = 0;
    this.value = value.map(function(item) {
      if (!(item instanceof EncoderBuffer))
        item = new EncoderBuffer(item, reporter);
      this.length += item.length;
      return item;
    }, this);
  } else if (typeof value === 'number') {
    if (!(0 <= value && value <= 0xff))
      return reporter.error('non-byte EncoderBuffer value');
    this.value = value;
    this.length = 1;
  } else if (typeof value === 'string') {
    this.value = value;
    this.length = Buffer.byteLength(value);
  } else if (Buffer.isBuffer(value)) {
    this.value = value;
    this.length = value.length;
  } else {
    return reporter.error('Unsupported type: ' + typeof value);
  }
}
exports.EncoderBuffer = EncoderBuffer;

EncoderBuffer.prototype.join = function join(out, offset) {
  if (!out)
    out = new Buffer(this.length);
  if (!offset)
    offset = 0;

  if (this.length === 0)
    return out;

  if (Array.isArray(this.value)) {
    this.value.forEach(function(item) {
      item.join(out, offset);
      offset += item.length;
    });
  } else {
    if (typeof this.value === 'number')
      out[offset] = this.value;
    else if (typeof this.value === 'string')
      out.write(this.value, offset);
    else if (Buffer.isBuffer(this.value))
      this.value.copy(out, offset);
    offset += this.length;
  }

  return out;
};


/***/ }),
/* 15 */
/***/ (function(__unused_webpack_module, exports, __webpack_require__) {

"use strict";
/*!
 * The buffer module from node.js, for the browser.
 *
 * @author   Feross Aboukhadijeh <https://feross.org>
 * @license  MIT
 */
/* eslint-disable no-proto */



var base64 = __webpack_require__(16)
var ieee754 = __webpack_require__(17)
var customInspectSymbol =
  (typeof Symbol === 'function' && typeof Symbol['for'] === 'function') // eslint-disable-line dot-notation
    ? Symbol['for']('nodejs.util.inspect.custom') // eslint-disable-line dot-notation
    : null

exports.Buffer = Buffer
exports.SlowBuffer = SlowBuffer
exports.INSPECT_MAX_BYTES = 50

var K_MAX_LENGTH = 0x7fffffff
exports.kMaxLength = K_MAX_LENGTH

/**
 * If `Buffer.TYPED_ARRAY_SUPPORT`:
 *   === true    Use Uint8Array implementation (fastest)
 *   === false   Print warning and recommend using `buffer` v4.x which has an Object
 *               implementation (most compatible, even IE6)
 *
 * Browsers that support typed arrays are IE 10+, Firefox 4+, Chrome 7+, Safari 5.1+,
 * Opera 11.6+, iOS 4.2+.
 *
 * We report that the browser does not support typed arrays if the are not subclassable
 * using __proto__. Firefox 4-29 lacks support for adding new properties to `Uint8Array`
 * (See: https://bugzilla.mozilla.org/show_bug.cgi?id=695438). IE 10 lacks support
 * for __proto__ and has a buggy typed array implementation.
 */
Buffer.TYPED_ARRAY_SUPPORT = typedArraySupport()

if (!Buffer.TYPED_ARRAY_SUPPORT && typeof console !== 'undefined' &&
    typeof console.error === 'function') {
  console.error(
    'This browser lacks typed array (Uint8Array) support which is required by ' +
    '`buffer` v5.x. Use `buffer` v4.x if you require old browser support.'
  )
}

function typedArraySupport () {
  // Can typed array instances can be augmented?
  try {
    var arr = new Uint8Array(1)
    var proto = { foo: function () { return 42 } }
    Object.setPrototypeOf(proto, Uint8Array.prototype)
    Object.setPrototypeOf(arr, proto)
    return arr.foo() === 42
  } catch (e) {
    return false
  }
}

Object.defineProperty(Buffer.prototype, 'parent', {
  enumerable: true,
  get: function () {
    if (!Buffer.isBuffer(this)) return undefined
    return this.buffer
  }
})

Object.defineProperty(Buffer.prototype, 'offset', {
  enumerable: true,
  get: function () {
    if (!Buffer.isBuffer(this)) return undefined
    return this.byteOffset
  }
})

function createBuffer (length) {
  if (length > K_MAX_LENGTH) {
    throw new RangeError('The value "' + length + '" is invalid for option "size"')
  }
  // Return an augmented `Uint8Array` instance
  var buf = new Uint8Array(length)
  Object.setPrototypeOf(buf, Buffer.prototype)
  return buf
}

/**
 * The Buffer constructor returns instances of `Uint8Array` that have their
 * prototype changed to `Buffer.prototype`. Furthermore, `Buffer` is a subclass of
 * `Uint8Array`, so the returned instances will have all the node `Buffer` methods
 * and the `Uint8Array` methods. Square bracket notation works as expected -- it
 * returns a single octet.
 *
 * The `Uint8Array` prototype remains unmodified.
 */

function Buffer (arg, encodingOrOffset, length) {
  // Common case.
  if (typeof arg === 'number') {
    if (typeof encodingOrOffset === 'string') {
      throw new TypeError(
        'The "string" argument must be of type string. Received type number'
      )
    }
    return allocUnsafe(arg)
  }
  return from(arg, encodingOrOffset, length)
}

Buffer.poolSize = 8192 // not used by this implementation

function from (value, encodingOrOffset, length) {
  if (typeof value === 'string') {
    return fromString(value, encodingOrOffset)
  }

  if (ArrayBuffer.isView(value)) {
    return fromArrayView(value)
  }

  if (value == null) {
    throw new TypeError(
      'The first argument must be one of type string, Buffer, ArrayBuffer, Array, ' +
      'or Array-like Object. Received type ' + (typeof value)
    )
  }

  if (isInstance(value, ArrayBuffer) ||
      (value && isInstance(value.buffer, ArrayBuffer))) {
    return fromArrayBuffer(value, encodingOrOffset, length)
  }

  if (typeof SharedArrayBuffer !== 'undefined' &&
      (isInstance(value, SharedArrayBuffer) ||
      (value && isInstance(value.buffer, SharedArrayBuffer)))) {
    return fromArrayBuffer(value, encodingOrOffset, length)
  }

  if (typeof value === 'number') {
    throw new TypeError(
      'The "value" argument must not be of type number. Received type number'
    )
  }

  var valueOf = value.valueOf && value.valueOf()
  if (valueOf != null && valueOf !== value) {
    return Buffer.from(valueOf, encodingOrOffset, length)
  }

  var b = fromObject(value)
  if (b) return b

  if (typeof Symbol !== 'undefined' && Symbol.toPrimitive != null &&
      typeof value[Symbol.toPrimitive] === 'function') {
    return Buffer.from(
      value[Symbol.toPrimitive]('string'), encodingOrOffset, length
    )
  }

  throw new TypeError(
    'The first argument must be one of type string, Buffer, ArrayBuffer, Array, ' +
    'or Array-like Object. Received type ' + (typeof value)
  )
}

/**
 * Functionally equivalent to Buffer(arg, encoding) but throws a TypeError
 * if value is a number.
 * Buffer.from(str[, encoding])
 * Buffer.from(array)
 * Buffer.from(buffer)
 * Buffer.from(arrayBuffer[, byteOffset[, length]])
 **/
Buffer.from = function (value, encodingOrOffset, length) {
  return from(value, encodingOrOffset, length)
}

// Note: Change prototype *after* Buffer.from is defined to workaround Chrome bug:
// https://github.com/feross/buffer/pull/148
Object.setPrototypeOf(Buffer.prototype, Uint8Array.prototype)
Object.setPrototypeOf(Buffer, Uint8Array)

function assertSize (size) {
  if (typeof size !== 'number') {
    throw new TypeError('"size" argument must be of type number')
  } else if (size < 0) {
    throw new RangeError('The value "' + size + '" is invalid for option "size"')
  }
}

function alloc (size, fill, encoding) {
  assertSize(size)
  if (size <= 0) {
    return createBuffer(size)
  }
  if (fill !== undefined) {
    // Only pay attention to encoding if it's a string. This
    // prevents accidentally sending in a number that would
    // be interpreted as a start offset.
    return typeof encoding === 'string'
      ? createBuffer(size).fill(fill, encoding)
      : createBuffer(size).fill(fill)
  }
  return createBuffer(size)
}

/**
 * Creates a new filled Buffer instance.
 * alloc(size[, fill[, encoding]])
 **/
Buffer.alloc = function (size, fill, encoding) {
  return alloc(size, fill, encoding)
}

function allocUnsafe (size) {
  assertSize(size)
  return createBuffer(size < 0 ? 0 : checked(size) | 0)
}

/**
 * Equivalent to Buffer(num), by default creates a non-zero-filled Buffer instance.
 * */
Buffer.allocUnsafe = function (size) {
  return allocUnsafe(size)
}
/**
 * Equivalent to SlowBuffer(num), by default creates a non-zero-filled Buffer instance.
 */
Buffer.allocUnsafeSlow = function (size) {
  return allocUnsafe(size)
}

function fromString (string, encoding) {
  if (typeof encoding !== 'string' || encoding === '') {
    encoding = 'utf8'
  }

  if (!Buffer.isEncoding(encoding)) {
    throw new TypeError('Unknown encoding: ' + encoding)
  }

  var length = byteLength(string, encoding) | 0
  var buf = createBuffer(length)

  var actual = buf.write(string, encoding)

  if (actual !== length) {
    // Writing a hex string, for example, that contains invalid characters will
    // cause everything after the first invalid character to be ignored. (e.g.
    // 'abxxcd' will be treated as 'ab')
    buf = buf.slice(0, actual)
  }

  return buf
}

function fromArrayLike (array) {
  var length = array.length < 0 ? 0 : checked(array.length) | 0
  var buf = createBuffer(length)
  for (var i = 0; i < length; i += 1) {
    buf[i] = array[i] & 255
  }
  return buf
}

function fromArrayView (arrayView) {
  if (isInstance(arrayView, Uint8Array)) {
    var copy = new Uint8Array(arrayView)
    return fromArrayBuffer(copy.buffer, copy.byteOffset, copy.byteLength)
  }
  return fromArrayLike(arrayView)
}

function fromArrayBuffer (array, byteOffset, length) {
  if (byteOffset < 0 || array.byteLength < byteOffset) {
    throw new RangeError('"offset" is outside of buffer bounds')
  }

  if (array.byteLength < byteOffset + (length || 0)) {
    throw new RangeError('"length" is outside of buffer bounds')
  }

  var buf
  if (byteOffset === undefined && length === undefined) {
    buf = new Uint8Array(array)
  } else if (length === undefined) {
    buf = new Uint8Array(array, byteOffset)
  } else {
    buf = new Uint8Array(array, byteOffset, length)
  }

  // Return an augmented `Uint8Array` instance
  Object.setPrototypeOf(buf, Buffer.prototype)

  return buf
}

function fromObject (obj) {
  if (Buffer.isBuffer(obj)) {
    var len = checked(obj.length) | 0
    var buf = createBuffer(len)

    if (buf.length === 0) {
      return buf
    }

    obj.copy(buf, 0, 0, len)
    return buf
  }

  if (obj.length !== undefined) {
    if (typeof obj.length !== 'number' || numberIsNaN(obj.length)) {
      return createBuffer(0)
    }
    return fromArrayLike(obj)
  }

  if (obj.type === 'Buffer' && Array.isArray(obj.data)) {
    return fromArrayLike(obj.data)
  }
}

function checked (length) {
  // Note: cannot use `length < K_MAX_LENGTH` here because that fails when
  // length is NaN (which is otherwise coerced to zero.)
  if (length >= K_MAX_LENGTH) {
    throw new RangeError('Attempt to allocate Buffer larger than maximum ' +
                         'size: 0x' + K_MAX_LENGTH.toString(16) + ' bytes')
  }
  return length | 0
}

function SlowBuffer (length) {
  if (+length != length) { // eslint-disable-line eqeqeq
    length = 0
  }
  return Buffer.alloc(+length)
}

Buffer.isBuffer = function isBuffer (b) {
  return b != null && b._isBuffer === true &&
    b !== Buffer.prototype // so Buffer.isBuffer(Buffer.prototype) will be false
}

Buffer.compare = function compare (a, b) {
  if (isInstance(a, Uint8Array)) a = Buffer.from(a, a.offset, a.byteLength)
  if (isInstance(b, Uint8Array)) b = Buffer.from(b, b.offset, b.byteLength)
  if (!Buffer.isBuffer(a) || !Buffer.isBuffer(b)) {
    throw new TypeError(
      'The "buf1", "buf2" arguments must be one of type Buffer or Uint8Array'
    )
  }

  if (a === b) return 0

  var x = a.length
  var y = b.length

  for (var i = 0, len = Math.min(x, y); i < len; ++i) {
    if (a[i] !== b[i]) {
      x = a[i]
      y = b[i]
      break
    }
  }

  if (x < y) return -1
  if (y < x) return 1
  return 0
}

Buffer.isEncoding = function isEncoding (encoding) {
  switch (String(encoding).toLowerCase()) {
    case 'hex':
    case 'utf8':
    case 'utf-8':
    case 'ascii':
    case 'latin1':
    case 'binary':
    case 'base64':
    case 'ucs2':
    case 'ucs-2':
    case 'utf16le':
    case 'utf-16le':
      return true
    default:
      return false
  }
}

Buffer.concat = function concat (list, length) {
  if (!Array.isArray(list)) {
    throw new TypeError('"list" argument must be an Array of Buffers')
  }

  if (list.length === 0) {
    return Buffer.alloc(0)
  }

  var i
  if (length === undefined) {
    length = 0
    for (i = 0; i < list.length; ++i) {
      length += list[i].length
    }
  }

  var buffer = Buffer.allocUnsafe(length)
  var pos = 0
  for (i = 0; i < list.length; ++i) {
    var buf = list[i]
    if (isInstance(buf, Uint8Array)) {
      if (pos + buf.length > buffer.length) {
        Buffer.from(buf).copy(buffer, pos)
      } else {
        Uint8Array.prototype.set.call(
          buffer,
          buf,
          pos
        )
      }
    } else if (!Buffer.isBuffer(buf)) {
      throw new TypeError('"list" argument must be an Array of Buffers')
    } else {
      buf.copy(buffer, pos)
    }
    pos += buf.length
  }
  return buffer
}

function byteLength (string, encoding) {
  if (Buffer.isBuffer(string)) {
    return string.length
  }
  if (ArrayBuffer.isView(string) || isInstance(string, ArrayBuffer)) {
    return string.byteLength
  }
  if (typeof string !== 'string') {
    throw new TypeError(
      'The "string" argument must be one of type string, Buffer, or ArrayBuffer. ' +
      'Received type ' + typeof string
    )
  }

  var len = string.length
  var mustMatch = (arguments.length > 2 && arguments[2] === true)
  if (!mustMatch && len === 0) return 0

  // Use a for loop to avoid recursion
  var loweredCase = false
  for (;;) {
    switch (encoding) {
      case 'ascii':
      case 'latin1':
      case 'binary':
        return len
      case 'utf8':
      case 'utf-8':
        return utf8ToBytes(string).length
      case 'ucs2':
      case 'ucs-2':
      case 'utf16le':
      case 'utf-16le':
        return len * 2
      case 'hex':
        return len >>> 1
      case 'base64':
        return base64ToBytes(string).length
      default:
        if (loweredCase) {
          return mustMatch ? -1 : utf8ToBytes(string).length // assume utf8
        }
        encoding = ('' + encoding).toLowerCase()
        loweredCase = true
    }
  }
}
Buffer.byteLength = byteLength

function slowToString (encoding, start, end) {
  var loweredCase = false

  // No need to verify that "this.length <= MAX_UINT32" since it's a read-only
  // property of a typed array.

  // This behaves neither like String nor Uint8Array in that we set start/end
  // to their upper/lower bounds if the value passed is out of range.
  // undefined is handled specially as per ECMA-262 6th Edition,
  // Section 13.3.3.7 Runtime Semantics: KeyedBindingInitialization.
  if (start === undefined || start < 0) {
    start = 0
  }
  // Return early if start > this.length. Done here to prevent potential uint32
  // coercion fail below.
  if (start > this.length) {
    return ''
  }

  if (end === undefined || end > this.length) {
    end = this.length
  }

  if (end <= 0) {
    return ''
  }

  // Force coercion to uint32. This will also coerce falsey/NaN values to 0.
  end >>>= 0
  start >>>= 0

  if (end <= start) {
    return ''
  }

  if (!encoding) encoding = 'utf8'

  while (true) {
    switch (encoding) {
      case 'hex':
        return hexSlice(this, start, end)

      case 'utf8':
      case 'utf-8':
        return utf8Slice(this, start, end)

      case 'ascii':
        return asciiSlice(this, start, end)

      case 'latin1':
      case 'binary':
        return latin1Slice(this, start, end)

      case 'base64':
        return base64Slice(this, start, end)

      case 'ucs2':
      case 'ucs-2':
      case 'utf16le':
      case 'utf-16le':
        return utf16leSlice(this, start, end)

      default:
        if (loweredCase) throw new TypeError('Unknown encoding: ' + encoding)
        encoding = (encoding + '').toLowerCase()
        loweredCase = true
    }
  }
}

// This property is used by `Buffer.isBuffer` (and the `is-buffer` npm package)
// to detect a Buffer instance. It's not possible to use `instanceof Buffer`
// reliably in a browserify context because there could be multiple different
// copies of the 'buffer' package in use. This method works even for Buffer
// instances that were created from another copy of the `buffer` package.
// See: https://github.com/feross/buffer/issues/154
Buffer.prototype._isBuffer = true

function swap (b, n, m) {
  var i = b[n]
  b[n] = b[m]
  b[m] = i
}

Buffer.prototype.swap16 = function swap16 () {
  var len = this.length
  if (len % 2 !== 0) {
    throw new RangeError('Buffer size must be a multiple of 16-bits')
  }
  for (var i = 0; i < len; i += 2) {
    swap(this, i, i + 1)
  }
  return this
}

Buffer.prototype.swap32 = function swap32 () {
  var len = this.length
  if (len % 4 !== 0) {
    throw new RangeError('Buffer size must be a multiple of 32-bits')
  }
  for (var i = 0; i < len; i += 4) {
    swap(this, i, i + 3)
    swap(this, i + 1, i + 2)
  }
  return this
}

Buffer.prototype.swap64 = function swap64 () {
  var len = this.length
  if (len % 8 !== 0) {
    throw new RangeError('Buffer size must be a multiple of 64-bits')
  }
  for (var i = 0; i < len; i += 8) {
    swap(this, i, i + 7)
    swap(this, i + 1, i + 6)
    swap(this, i + 2, i + 5)
    swap(this, i + 3, i + 4)
  }
  return this
}

Buffer.prototype.toString = function toString () {
  var length = this.length
  if (length === 0) return ''
  if (arguments.length === 0) return utf8Slice(this, 0, length)
  return slowToString.apply(this, arguments)
}

Buffer.prototype.toLocaleString = Buffer.prototype.toString

Buffer.prototype.equals = function equals (b) {
  if (!Buffer.isBuffer(b)) throw new TypeError('Argument must be a Buffer')
  if (this === b) return true
  return Buffer.compare(this, b) === 0
}

Buffer.prototype.inspect = function inspect () {
  var str = ''
  var max = exports.INSPECT_MAX_BYTES
  str = this.toString('hex', 0, max).replace(/(.{2})/g, '$1 ').trim()
  if (this.length > max) str += ' ... '
  return '<Buffer ' + str + '>'
}
if (customInspectSymbol) {
  Buffer.prototype[customInspectSymbol] = Buffer.prototype.inspect
}

Buffer.prototype.compare = function compare (target, start, end, thisStart, thisEnd) {
  if (isInstance(target, Uint8Array)) {
    target = Buffer.from(target, target.offset, target.byteLength)
  }
  if (!Buffer.isBuffer(target)) {
    throw new TypeError(
      'The "target" argument must be one of type Buffer or Uint8Array. ' +
      'Received type ' + (typeof target)
    )
  }

  if (start === undefined) {
    start = 0
  }
  if (end === undefined) {
    end = target ? target.length : 0
  }
  if (thisStart === undefined) {
    thisStart = 0
  }
  if (thisEnd === undefined) {
    thisEnd = this.length
  }

  if (start < 0 || end > target.length || thisStart < 0 || thisEnd > this.length) {
    throw new RangeError('out of range index')
  }

  if (thisStart >= thisEnd && start >= end) {
    return 0
  }
  if (thisStart >= thisEnd) {
    return -1
  }
  if (start >= end) {
    return 1
  }

  start >>>= 0
  end >>>= 0
  thisStart >>>= 0
  thisEnd >>>= 0

  if (this === target) return 0

  var x = thisEnd - thisStart
  var y = end - start
  var len = Math.min(x, y)

  var thisCopy = this.slice(thisStart, thisEnd)
  var targetCopy = target.slice(start, end)

  for (var i = 0; i < len; ++i) {
    if (thisCopy[i] !== targetCopy[i]) {
      x = thisCopy[i]
      y = targetCopy[i]
      break
    }
  }

  if (x < y) return -1
  if (y < x) return 1
  return 0
}

// Finds either the first index of `val` in `buffer` at offset >= `byteOffset`,
// OR the last index of `val` in `buffer` at offset <= `byteOffset`.
//
// Arguments:
// - buffer - a Buffer to search
// - val - a string, Buffer, or number
// - byteOffset - an index into `buffer`; will be clamped to an int32
// - encoding - an optional encoding, relevant is val is a string
// - dir - true for indexOf, false for lastIndexOf
function bidirectionalIndexOf (buffer, val, byteOffset, encoding, dir) {
  // Empty buffer means no match
  if (buffer.length === 0) return -1

  // Normalize byteOffset
  if (typeof byteOffset === 'string') {
    encoding = byteOffset
    byteOffset = 0
  } else if (byteOffset > 0x7fffffff) {
    byteOffset = 0x7fffffff
  } else if (byteOffset < -0x80000000) {
    byteOffset = -0x80000000
  }
  byteOffset = +byteOffset // Coerce to Number.
  if (numberIsNaN(byteOffset)) {
    // byteOffset: it it's undefined, null, NaN, "foo", etc, search whole buffer
    byteOffset = dir ? 0 : (buffer.length - 1)
  }

  // Normalize byteOffset: negative offsets start from the end of the buffer
  if (byteOffset < 0) byteOffset = buffer.length + byteOffset
  if (byteOffset >= buffer.length) {
    if (dir) return -1
    else byteOffset = buffer.length - 1
  } else if (byteOffset < 0) {
    if (dir) byteOffset = 0
    else return -1
  }

  // Normalize val
  if (typeof val === 'string') {
    val = Buffer.from(val, encoding)
  }

  // Finally, search either indexOf (if dir is true) or lastIndexOf
  if (Buffer.isBuffer(val)) {
    // Special case: looking for empty string/buffer always fails
    if (val.length === 0) {
      return -1
    }
    return arrayIndexOf(buffer, val, byteOffset, encoding, dir)
  } else if (typeof val === 'number') {
    val = val & 0xFF // Search for a byte value [0-255]
    if (typeof Uint8Array.prototype.indexOf === 'function') {
      if (dir) {
        return Uint8Array.prototype.indexOf.call(buffer, val, byteOffset)
      } else {
        return Uint8Array.prototype.lastIndexOf.call(buffer, val, byteOffset)
      }
    }
    return arrayIndexOf(buffer, [val], byteOffset, encoding, dir)
  }

  throw new TypeError('val must be string, number or Buffer')
}

function arrayIndexOf (arr, val, byteOffset, encoding, dir) {
  var indexSize = 1
  var arrLength = arr.length
  var valLength = val.length

  if (encoding !== undefined) {
    encoding = String(encoding).toLowerCase()
    if (encoding === 'ucs2' || encoding === 'ucs-2' ||
        encoding === 'utf16le' || encoding === 'utf-16le') {
      if (arr.length < 2 || val.length < 2) {
        return -1
      }
      indexSize = 2
      arrLength /= 2
      valLength /= 2
      byteOffset /= 2
    }
  }

  function read (buf, i) {
    if (indexSize === 1) {
      return buf[i]
    } else {
      return buf.readUInt16BE(i * indexSize)
    }
  }

  var i
  if (dir) {
    var foundIndex = -1
    for (i = byteOffset; i < arrLength; i++) {
      if (read(arr, i) === read(val, foundIndex === -1 ? 0 : i - foundIndex)) {
        if (foundIndex === -1) foundIndex = i
        if (i - foundIndex + 1 === valLength) return foundIndex * indexSize
      } else {
        if (foundIndex !== -1) i -= i - foundIndex
        foundIndex = -1
      }
    }
  } else {
    if (byteOffset + valLength > arrLength) byteOffset = arrLength - valLength
    for (i = byteOffset; i >= 0; i--) {
      var found = true
      for (var j = 0; j < valLength; j++) {
        if (read(arr, i + j) !== read(val, j)) {
          found = false
          break
        }
      }
      if (found) return i
    }
  }

  return -1
}

Buffer.prototype.includes = function includes (val, byteOffset, encoding) {
  return this.indexOf(val, byteOffset, encoding) !== -1
}

Buffer.prototype.indexOf = function indexOf (val, byteOffset, encoding) {
  return bidirectionalIndexOf(this, val, byteOffset, encoding, true)
}

Buffer.prototype.lastIndexOf = function lastIndexOf (val, byteOffset, encoding) {
  return bidirectionalIndexOf(this, val, byteOffset, encoding, false)
}

function hexWrite (buf, string, offset, length) {
  offset = Number(offset) || 0
  var remaining = buf.length - offset
  if (!length) {
    length = remaining
  } else {
    length = Number(length)
    if (length > remaining) {
      length = remaining
    }
  }

  var strLen = string.length

  if (length > strLen / 2) {
    length = strLen / 2
  }
  for (var i = 0; i < length; ++i) {
    var parsed = parseInt(string.substr(i * 2, 2), 16)
    if (numberIsNaN(parsed)) return i
    buf[offset + i] = parsed
  }
  return i
}

function utf8Write (buf, string, offset, length) {
  return blitBuffer(utf8ToBytes(string, buf.length - offset), buf, offset, length)
}

function asciiWrite (buf, string, offset, length) {
  return blitBuffer(asciiToBytes(string), buf, offset, length)
}

function base64Write (buf, string, offset, length) {
  return blitBuffer(base64ToBytes(string), buf, offset, length)
}

function ucs2Write (buf, string, offset, length) {
  return blitBuffer(utf16leToBytes(string, buf.length - offset), buf, offset, length)
}

Buffer.prototype.write = function write (string, offset, length, encoding) {
  // Buffer#write(string)
  if (offset === undefined) {
    encoding = 'utf8'
    length = this.length
    offset = 0
  // Buffer#write(string, encoding)
  } else if (length === undefined && typeof offset === 'string') {
    encoding = offset
    length = this.length
    offset = 0
  // Buffer#write(string, offset[, length][, encoding])
  } else if (isFinite(offset)) {
    offset = offset >>> 0
    if (isFinite(length)) {
      length = length >>> 0
      if (encoding === undefined) encoding = 'utf8'
    } else {
      encoding = length
      length = undefined
    }
  } else {
    throw new Error(
      'Buffer.write(string, encoding, offset[, length]) is no longer supported'
    )
  }

  var remaining = this.length - offset
  if (length === undefined || length > remaining) length = remaining

  if ((string.length > 0 && (length < 0 || offset < 0)) || offset > this.length) {
    throw new RangeError('Attempt to write outside buffer bounds')
  }

  if (!encoding) encoding = 'utf8'

  var loweredCase = false
  for (;;) {
    switch (encoding) {
      case 'hex':
        return hexWrite(this, string, offset, length)

      case 'utf8':
      case 'utf-8':
        return utf8Write(this, string, offset, length)

      case 'ascii':
      case 'latin1':
      case 'binary':
        return asciiWrite(this, string, offset, length)

      case 'base64':
        // Warning: maxLength not taken into account in base64Write
        return base64Write(this, string, offset, length)

      case 'ucs2':
      case 'ucs-2':
      case 'utf16le':
      case 'utf-16le':
        return ucs2Write(this, string, offset, length)

      default:
        if (loweredCase) throw new TypeError('Unknown encoding: ' + encoding)
        encoding = ('' + encoding).toLowerCase()
        loweredCase = true
    }
  }
}

Buffer.prototype.toJSON = function toJSON () {
  return {
    type: 'Buffer',
    data: Array.prototype.slice.call(this._arr || this, 0)
  }
}

function base64Slice (buf, start, end) {
  if (start === 0 && end === buf.length) {
    return base64.fromByteArray(buf)
  } else {
    return base64.fromByteArray(buf.slice(start, end))
  }
}

function utf8Slice (buf, start, end) {
  end = Math.min(buf.length, end)
  var res = []

  var i = start
  while (i < end) {
    var firstByte = buf[i]
    var codePoint = null
    var bytesPerSequence = (firstByte > 0xEF)
      ? 4
      : (firstByte > 0xDF)
          ? 3
          : (firstByte > 0xBF)
              ? 2
              : 1

    if (i + bytesPerSequence <= end) {
      var secondByte, thirdByte, fourthByte, tempCodePoint

      switch (bytesPerSequence) {
        case 1:
          if (firstByte < 0x80) {
            codePoint = firstByte
          }
          break
        case 2:
          secondByte = buf[i + 1]
          if ((secondByte & 0xC0) === 0x80) {
            tempCodePoint = (firstByte & 0x1F) << 0x6 | (secondByte & 0x3F)
            if (tempCodePoint > 0x7F) {
              codePoint = tempCodePoint
            }
          }
          break
        case 3:
          secondByte = buf[i + 1]
          thirdByte = buf[i + 2]
          if ((secondByte & 0xC0) === 0x80 && (thirdByte & 0xC0) === 0x80) {
            tempCodePoint = (firstByte & 0xF) << 0xC | (secondByte & 0x3F) << 0x6 | (thirdByte & 0x3F)
            if (tempCodePoint > 0x7FF && (tempCodePoint < 0xD800 || tempCodePoint > 0xDFFF)) {
              codePoint = tempCodePoint
            }
          }
          break
        case 4:
          secondByte = buf[i + 1]
          thirdByte = buf[i + 2]
          fourthByte = buf[i + 3]
          if ((secondByte & 0xC0) === 0x80 && (thirdByte & 0xC0) === 0x80 && (fourthByte & 0xC0) === 0x80) {
            tempCodePoint = (firstByte & 0xF) << 0x12 | (secondByte & 0x3F) << 0xC | (thirdByte & 0x3F) << 0x6 | (fourthByte & 0x3F)
            if (tempCodePoint > 0xFFFF && tempCodePoint < 0x110000) {
              codePoint = tempCodePoint
            }
          }
      }
    }

    if (codePoint === null) {
      // we did not generate a valid codePoint so insert a
      // replacement char (U+FFFD) and advance only 1 byte
      codePoint = 0xFFFD
      bytesPerSequence = 1
    } else if (codePoint > 0xFFFF) {
      // encode to utf16 (surrogate pair dance)
      codePoint -= 0x10000
      res.push(codePoint >>> 10 & 0x3FF | 0xD800)
      codePoint = 0xDC00 | codePoint & 0x3FF
    }

    res.push(codePoint)
    i += bytesPerSequence
  }

  return decodeCodePointsArray(res)
}

// Based on http://stackoverflow.com/a/22747272/680742, the browser with
// the lowest limit is Chrome, with 0x10000 args.
// We go 1 magnitude less, for safety
var MAX_ARGUMENTS_LENGTH = 0x1000

function decodeCodePointsArray (codePoints) {
  var len = codePoints.length
  if (len <= MAX_ARGUMENTS_LENGTH) {
    return String.fromCharCode.apply(String, codePoints) // avoid extra slice()
  }

  // Decode in chunks to avoid "call stack size exceeded".
  var res = ''
  var i = 0
  while (i < len) {
    res += String.fromCharCode.apply(
      String,
      codePoints.slice(i, i += MAX_ARGUMENTS_LENGTH)
    )
  }
  return res
}

function asciiSlice (buf, start, end) {
  var ret = ''
  end = Math.min(buf.length, end)

  for (var i = start; i < end; ++i) {
    ret += String.fromCharCode(buf[i] & 0x7F)
  }
  return ret
}

function latin1Slice (buf, start, end) {
  var ret = ''
  end = Math.min(buf.length, end)

  for (var i = start; i < end; ++i) {
    ret += String.fromCharCode(buf[i])
  }
  return ret
}

function hexSlice (buf, start, end) {
  var len = buf.length

  if (!start || start < 0) start = 0
  if (!end || end < 0 || end > len) end = len

  var out = ''
  for (var i = start; i < end; ++i) {
    out += hexSliceLookupTable[buf[i]]
  }
  return out
}

function utf16leSlice (buf, start, end) {
  var bytes = buf.slice(start, end)
  var res = ''
  // If bytes.length is odd, the last 8 bits must be ignored (same as node.js)
  for (var i = 0; i < bytes.length - 1; i += 2) {
    res += String.fromCharCode(bytes[i] + (bytes[i + 1] * 256))
  }
  return res
}

Buffer.prototype.slice = function slice (start, end) {
  var len = this.length
  start = ~~start
  end = end === undefined ? len : ~~end

  if (start < 0) {
    start += len
    if (start < 0) start = 0
  } else if (start > len) {
    start = len
  }

  if (end < 0) {
    end += len
    if (end < 0) end = 0
  } else if (end > len) {
    end = len
  }

  if (end < start) end = start

  var newBuf = this.subarray(start, end)
  // Return an augmented `Uint8Array` instance
  Object.setPrototypeOf(newBuf, Buffer.prototype)

  return newBuf
}

/*
 * Need to make sure that buffer isn't trying to write out of bounds.
 */
function checkOffset (offset, ext, length) {
  if ((offset % 1) !== 0 || offset < 0) throw new RangeError('offset is not uint')
  if (offset + ext > length) throw new RangeError('Trying to access beyond buffer length')
}

Buffer.prototype.readUintLE =
Buffer.prototype.readUIntLE = function readUIntLE (offset, byteLength, noAssert) {
  offset = offset >>> 0
  byteLength = byteLength >>> 0
  if (!noAssert) checkOffset(offset, byteLength, this.length)

  var val = this[offset]
  var mul = 1
  var i = 0
  while (++i < byteLength && (mul *= 0x100)) {
    val += this[offset + i] * mul
  }

  return val
}

Buffer.prototype.readUintBE =
Buffer.prototype.readUIntBE = function readUIntBE (offset, byteLength, noAssert) {
  offset = offset >>> 0
  byteLength = byteLength >>> 0
  if (!noAssert) {
    checkOffset(offset, byteLength, this.length)
  }

  var val = this[offset + --byteLength]
  var mul = 1
  while (byteLength > 0 && (mul *= 0x100)) {
    val += this[offset + --byteLength] * mul
  }

  return val
}

Buffer.prototype.readUint8 =
Buffer.prototype.readUInt8 = function readUInt8 (offset, noAssert) {
  offset = offset >>> 0
  if (!noAssert) checkOffset(offset, 1, this.length)
  return this[offset]
}

Buffer.prototype.readUint16LE =
Buffer.prototype.readUInt16LE = function readUInt16LE (offset, noAssert) {
  offset = offset >>> 0
  if (!noAssert) checkOffset(offset, 2, this.length)
  return this[offset] | (this[offset + 1] << 8)
}

Buffer.prototype.readUint16BE =
Buffer.prototype.readUInt16BE = function readUInt16BE (offset, noAssert) {
  offset = offset >>> 0
  if (!noAssert) checkOffset(offset, 2, this.length)
  return (this[offset] << 8) | this[offset + 1]
}

Buffer.prototype.readUint32LE =
Buffer.prototype.readUInt32LE = function readUInt32LE (offset, noAssert) {
  offset = offset >>> 0
  if (!noAssert) checkOffset(offset, 4, this.length)

  return ((this[offset]) |
      (this[offset + 1] << 8) |
      (this[offset + 2] << 16)) +
      (this[offset + 3] * 0x1000000)
}

Buffer.prototype.readUint32BE =
Buffer.prototype.readUInt32BE = function readUInt32BE (offset, noAssert) {
  offset = offset >>> 0
  if (!noAssert) checkOffset(offset, 4, this.length)

  return (this[offset] * 0x1000000) +
    ((this[offset + 1] << 16) |
    (this[offset + 2] << 8) |
    this[offset + 3])
}

Buffer.prototype.readIntLE = function readIntLE (offset, byteLength, noAssert) {
  offset = offset >>> 0
  byteLength = byteLength >>> 0
  if (!noAssert) checkOffset(offset, byteLength, this.length)

  var val = this[offset]
  var mul = 1
  var i = 0
  while (++i < byteLength && (mul *= 0x100)) {
    val += this[offset + i] * mul
  }
  mul *= 0x80

  if (val >= mul) val -= Math.pow(2, 8 * byteLength)

  return val
}

Buffer.prototype.readIntBE = function readIntBE (offset, byteLength, noAssert) {
  offset = offset >>> 0
  byteLength = byteLength >>> 0
  if (!noAssert) checkOffset(offset, byteLength, this.length)

  var i = byteLength
  var mul = 1
  var val = this[offset + --i]
  while (i > 0 && (mul *= 0x100)) {
    val += this[offset + --i] * mul
  }
  mul *= 0x80

  if (val >= mul) val -= Math.pow(2, 8 * byteLength)

  return val
}

Buffer.prototype.readInt8 = function readInt8 (offset, noAssert) {
  offset = offset >>> 0
  if (!noAssert) checkOffset(offset, 1, this.length)
  if (!(this[offset] & 0x80)) return (this[offset])
  return ((0xff - this[offset] + 1) * -1)
}

Buffer.prototype.readInt16LE = function readInt16LE (offset, noAssert) {
  offset = offset >>> 0
  if (!noAssert) checkOffset(offset, 2, this.length)
  var val = this[offset] | (this[offset + 1] << 8)
  return (val & 0x8000) ? val | 0xFFFF0000 : val
}

Buffer.prototype.readInt16BE = function readInt16BE (offset, noAssert) {
  offset = offset >>> 0
  if (!noAssert) checkOffset(offset, 2, this.length)
  var val = this[offset + 1] | (this[offset] << 8)
  return (val & 0x8000) ? val | 0xFFFF0000 : val
}

Buffer.prototype.readInt32LE = function readInt32LE (offset, noAssert) {
  offset = offset >>> 0
  if (!noAssert) checkOffset(offset, 4, this.length)

  return (this[offset]) |
    (this[offset + 1] << 8) |
    (this[offset + 2] << 16) |
    (this[offset + 3] << 24)
}

Buffer.prototype.readInt32BE = function readInt32BE (offset, noAssert) {
  offset = offset >>> 0
  if (!noAssert) checkOffset(offset, 4, this.length)

  return (this[offset] << 24) |
    (this[offset + 1] << 16) |
    (this[offset + 2] << 8) |
    (this[offset + 3])
}

Buffer.prototype.readFloatLE = function readFloatLE (offset, noAssert) {
  offset = offset >>> 0
  if (!noAssert) checkOffset(offset, 4, this.length)
  return ieee754.read(this, offset, true, 23, 4)
}

Buffer.prototype.readFloatBE = function readFloatBE (offset, noAssert) {
  offset = offset >>> 0
  if (!noAssert) checkOffset(offset, 4, this.length)
  return ieee754.read(this, offset, false, 23, 4)
}

Buffer.prototype.readDoubleLE = function readDoubleLE (offset, noAssert) {
  offset = offset >>> 0
  if (!noAssert) checkOffset(offset, 8, this.length)
  return ieee754.read(this, offset, true, 52, 8)
}

Buffer.prototype.readDoubleBE = function readDoubleBE (offset, noAssert) {
  offset = offset >>> 0
  if (!noAssert) checkOffset(offset, 8, this.length)
  return ieee754.read(this, offset, false, 52, 8)
}

function checkInt (buf, value, offset, ext, max, min) {
  if (!Buffer.isBuffer(buf)) throw new TypeError('"buffer" argument must be a Buffer instance')
  if (value > max || value < min) throw new RangeError('"value" argument is out of bounds')
  if (offset + ext > buf.length) throw new RangeError('Index out of range')
}

Buffer.prototype.writeUintLE =
Buffer.prototype.writeUIntLE = function writeUIntLE (value, offset, byteLength, noAssert) {
  value = +value
  offset = offset >>> 0
  byteLength = byteLength >>> 0
  if (!noAssert) {
    var maxBytes = Math.pow(2, 8 * byteLength) - 1
    checkInt(this, value, offset, byteLength, maxBytes, 0)
  }

  var mul = 1
  var i = 0
  this[offset] = value & 0xFF
  while (++i < byteLength && (mul *= 0x100)) {
    this[offset + i] = (value / mul) & 0xFF
  }

  return offset + byteLength
}

Buffer.prototype.writeUintBE =
Buffer.prototype.writeUIntBE = function writeUIntBE (value, offset, byteLength, noAssert) {
  value = +value
  offset = offset >>> 0
  byteLength = byteLength >>> 0
  if (!noAssert) {
    var maxBytes = Math.pow(2, 8 * byteLength) - 1
    checkInt(this, value, offset, byteLength, maxBytes, 0)
  }

  var i = byteLength - 1
  var mul = 1
  this[offset + i] = value & 0xFF
  while (--i >= 0 && (mul *= 0x100)) {
    this[offset + i] = (value / mul) & 0xFF
  }

  return offset + byteLength
}

Buffer.prototype.writeUint8 =
Buffer.prototype.writeUInt8 = function writeUInt8 (value, offset, noAssert) {
  value = +value
  offset = offset >>> 0
  if (!noAssert) checkInt(this, value, offset, 1, 0xff, 0)
  this[offset] = (value & 0xff)
  return offset + 1
}

Buffer.prototype.writeUint16LE =
Buffer.prototype.writeUInt16LE = function writeUInt16LE (value, offset, noAssert) {
  value = +value
  offset = offset >>> 0
  if (!noAssert) checkInt(this, value, offset, 2, 0xffff, 0)
  this[offset] = (value & 0xff)
  this[offset + 1] = (value >>> 8)
  return offset + 2
}

Buffer.prototype.writeUint16BE =
Buffer.prototype.writeUInt16BE = function writeUInt16BE (value, offset, noAssert) {
  value = +value
  offset = offset >>> 0
  if (!noAssert) checkInt(this, value, offset, 2, 0xffff, 0)
  this[offset] = (value >>> 8)
  this[offset + 1] = (value & 0xff)
  return offset + 2
}

Buffer.prototype.writeUint32LE =
Buffer.prototype.writeUInt32LE = function writeUInt32LE (value, offset, noAssert) {
  value = +value
  offset = offset >>> 0
  if (!noAssert) checkInt(this, value, offset, 4, 0xffffffff, 0)
  this[offset + 3] = (value >>> 24)
  this[offset + 2] = (value >>> 16)
  this[offset + 1] = (value >>> 8)
  this[offset] = (value & 0xff)
  return offset + 4
}

Buffer.prototype.writeUint32BE =
Buffer.prototype.writeUInt32BE = function writeUInt32BE (value, offset, noAssert) {
  value = +value
  offset = offset >>> 0
  if (!noAssert) checkInt(this, value, offset, 4, 0xffffffff, 0)
  this[offset] = (value >>> 24)
  this[offset + 1] = (value >>> 16)
  this[offset + 2] = (value >>> 8)
  this[offset + 3] = (value & 0xff)
  return offset + 4
}

Buffer.prototype.writeIntLE = function writeIntLE (value, offset, byteLength, noAssert) {
  value = +value
  offset = offset >>> 0
  if (!noAssert) {
    var limit = Math.pow(2, (8 * byteLength) - 1)

    checkInt(this, value, offset, byteLength, limit - 1, -limit)
  }

  var i = 0
  var mul = 1
  var sub = 0
  this[offset] = value & 0xFF
  while (++i < byteLength && (mul *= 0x100)) {
    if (value < 0 && sub === 0 && this[offset + i - 1] !== 0) {
      sub = 1
    }
    this[offset + i] = ((value / mul) >> 0) - sub & 0xFF
  }

  return offset + byteLength
}

Buffer.prototype.writeIntBE = function writeIntBE (value, offset, byteLength, noAssert) {
  value = +value
  offset = offset >>> 0
  if (!noAssert) {
    var limit = Math.pow(2, (8 * byteLength) - 1)

    checkInt(this, value, offset, byteLength, limit - 1, -limit)
  }

  var i = byteLength - 1
  var mul = 1
  var sub = 0
  this[offset + i] = value & 0xFF
  while (--i >= 0 && (mul *= 0x100)) {
    if (value < 0 && sub === 0 && this[offset + i + 1] !== 0) {
      sub = 1
    }
    this[offset + i] = ((value / mul) >> 0) - sub & 0xFF
  }

  return offset + byteLength
}

Buffer.prototype.writeInt8 = function writeInt8 (value, offset, noAssert) {
  value = +value
  offset = offset >>> 0
  if (!noAssert) checkInt(this, value, offset, 1, 0x7f, -0x80)
  if (value < 0) value = 0xff + value + 1
  this[offset] = (value & 0xff)
  return offset + 1
}

Buffer.prototype.writeInt16LE = function writeInt16LE (value, offset, noAssert) {
  value = +value
  offset = offset >>> 0
  if (!noAssert) checkInt(this, value, offset, 2, 0x7fff, -0x8000)
  this[offset] = (value & 0xff)
  this[offset + 1] = (value >>> 8)
  return offset + 2
}

Buffer.prototype.writeInt16BE = function writeInt16BE (value, offset, noAssert) {
  value = +value
  offset = offset >>> 0
  if (!noAssert) checkInt(this, value, offset, 2, 0x7fff, -0x8000)
  this[offset] = (value >>> 8)
  this[offset + 1] = (value & 0xff)
  return offset + 2
}

Buffer.prototype.writeInt32LE = function writeInt32LE (value, offset, noAssert) {
  value = +value
  offset = offset >>> 0
  if (!noAssert) checkInt(this, value, offset, 4, 0x7fffffff, -0x80000000)
  this[offset] = (value & 0xff)
  this[offset + 1] = (value >>> 8)
  this[offset + 2] = (value >>> 16)
  this[offset + 3] = (value >>> 24)
  return offset + 4
}

Buffer.prototype.writeInt32BE = function writeInt32BE (value, offset, noAssert) {
  value = +value
  offset = offset >>> 0
  if (!noAssert) checkInt(this, value, offset, 4, 0x7fffffff, -0x80000000)
  if (value < 0) value = 0xffffffff + value + 1
  this[offset] = (value >>> 24)
  this[offset + 1] = (value >>> 16)
  this[offset + 2] = (value >>> 8)
  this[offset + 3] = (value & 0xff)
  return offset + 4
}

function checkIEEE754 (buf, value, offset, ext, max, min) {
  if (offset + ext > buf.length) throw new RangeError('Index out of range')
  if (offset < 0) throw new RangeError('Index out of range')
}

function writeFloat (buf, value, offset, littleEndian, noAssert) {
  value = +value
  offset = offset >>> 0
  if (!noAssert) {
    checkIEEE754(buf, value, offset, 4, 3.4028234663852886e+38, -3.4028234663852886e+38)
  }
  ieee754.write(buf, value, offset, littleEndian, 23, 4)
  return offset + 4
}

Buffer.prototype.writeFloatLE = function writeFloatLE (value, offset, noAssert) {
  return writeFloat(this, value, offset, true, noAssert)
}

Buffer.prototype.writeFloatBE = function writeFloatBE (value, offset, noAssert) {
  return writeFloat(this, value, offset, false, noAssert)
}

function writeDouble (buf, value, offset, littleEndian, noAssert) {
  value = +value
  offset = offset >>> 0
  if (!noAssert) {
    checkIEEE754(buf, value, offset, 8, 1.7976931348623157E+308, -1.7976931348623157E+308)
  }
  ieee754.write(buf, value, offset, littleEndian, 52, 8)
  return offset + 8
}

Buffer.prototype.writeDoubleLE = function writeDoubleLE (value, offset, noAssert) {
  return writeDouble(this, value, offset, true, noAssert)
}

Buffer.prototype.writeDoubleBE = function writeDoubleBE (value, offset, noAssert) {
  return writeDouble(this, value, offset, false, noAssert)
}

// copy(targetBuffer, targetStart=0, sourceStart=0, sourceEnd=buffer.length)
Buffer.prototype.copy = function copy (target, targetStart, start, end) {
  if (!Buffer.isBuffer(target)) throw new TypeError('argument should be a Buffer')
  if (!start) start = 0
  if (!end && end !== 0) end = this.length
  if (targetStart >= target.length) targetStart = target.length
  if (!targetStart) targetStart = 0
  if (end > 0 && end < start) end = start

  // Copy 0 bytes; we're done
  if (end === start) return 0
  if (target.length === 0 || this.length === 0) return 0

  // Fatal error conditions
  if (targetStart < 0) {
    throw new RangeError('targetStart out of bounds')
  }
  if (start < 0 || start >= this.length) throw new RangeError('Index out of range')
  if (end < 0) throw new RangeError('sourceEnd out of bounds')

  // Are we oob?
  if (end > this.length) end = this.length
  if (target.length - targetStart < end - start) {
    end = target.length - targetStart + start
  }

  var len = end - start

  if (this === target && typeof Uint8Array.prototype.copyWithin === 'function') {
    // Use built-in when available, missing from IE11
    this.copyWithin(targetStart, start, end)
  } else {
    Uint8Array.prototype.set.call(
      target,
      this.subarray(start, end),
      targetStart
    )
  }

  return len
}

// Usage:
//    buffer.fill(number[, offset[, end]])
//    buffer.fill(buffer[, offset[, end]])
//    buffer.fill(string[, offset[, end]][, encoding])
Buffer.prototype.fill = function fill (val, start, end, encoding) {
  // Handle string cases:
  if (typeof val === 'string') {
    if (typeof start === 'string') {
      encoding = start
      start = 0
      end = this.length
    } else if (typeof end === 'string') {
      encoding = end
      end = this.length
    }
    if (encoding !== undefined && typeof encoding !== 'string') {
      throw new TypeError('encoding must be a string')
    }
    if (typeof encoding === 'string' && !Buffer.isEncoding(encoding)) {
      throw new TypeError('Unknown encoding: ' + encoding)
    }
    if (val.length === 1) {
      var code = val.charCodeAt(0)
      if ((encoding === 'utf8' && code < 128) ||
          encoding === 'latin1') {
        // Fast path: If `val` fits into a single byte, use that numeric value.
        val = code
      }
    }
  } else if (typeof val === 'number') {
    val = val & 255
  } else if (typeof val === 'boolean') {
    val = Number(val)
  }

  // Invalid ranges are not set to a default, so can range check early.
  if (start < 0 || this.length < start || this.length < end) {
    throw new RangeError('Out of range index')
  }

  if (end <= start) {
    return this
  }

  start = start >>> 0
  end = end === undefined ? this.length : end >>> 0

  if (!val) val = 0

  var i
  if (typeof val === 'number') {
    for (i = start; i < end; ++i) {
      this[i] = val
    }
  } else {
    var bytes = Buffer.isBuffer(val)
      ? val
      : Buffer.from(val, encoding)
    var len = bytes.length
    if (len === 0) {
      throw new TypeError('The value "' + val +
        '" is invalid for argument "value"')
    }
    for (i = 0; i < end - start; ++i) {
      this[i + start] = bytes[i % len]
    }
  }

  return this
}

// HELPER FUNCTIONS
// ================

var INVALID_BASE64_RE = /[^+/0-9A-Za-z-_]/g

function base64clean (str) {
  // Node takes equal signs as end of the Base64 encoding
  str = str.split('=')[0]
  // Node strips out invalid characters like \n and \t from the string, base64-js does not
  str = str.trim().replace(INVALID_BASE64_RE, '')
  // Node converts strings with length < 2 to ''
  if (str.length < 2) return ''
  // Node allows for non-padded base64 strings (missing trailing ===), base64-js does not
  while (str.length % 4 !== 0) {
    str = str + '='
  }
  return str
}

function utf8ToBytes (string, units) {
  units = units || Infinity
  var codePoint
  var length = string.length
  var leadSurrogate = null
  var bytes = []

  for (var i = 0; i < length; ++i) {
    codePoint = string.charCodeAt(i)

    // is surrogate component
    if (codePoint > 0xD7FF && codePoint < 0xE000) {
      // last char was a lead
      if (!leadSurrogate) {
        // no lead yet
        if (codePoint > 0xDBFF) {
          // unexpected trail
          if ((units -= 3) > -1) bytes.push(0xEF, 0xBF, 0xBD)
          continue
        } else if (i + 1 === length) {
          // unpaired lead
          if ((units -= 3) > -1) bytes.push(0xEF, 0xBF, 0xBD)
          continue
        }

        // valid lead
        leadSurrogate = codePoint

        continue
      }

      // 2 leads in a row
      if (codePoint < 0xDC00) {
        if ((units -= 3) > -1) bytes.push(0xEF, 0xBF, 0xBD)
        leadSurrogate = codePoint
        continue
      }

      // valid surrogate pair
      codePoint = (leadSurrogate - 0xD800 << 10 | codePoint - 0xDC00) + 0x10000
    } else if (leadSurrogate) {
      // valid bmp char, but last char was a lead
      if ((units -= 3) > -1) bytes.push(0xEF, 0xBF, 0xBD)
    }

    leadSurrogate = null

    // encode utf8
    if (codePoint < 0x80) {
      if ((units -= 1) < 0) break
      bytes.push(codePoint)
    } else if (codePoint < 0x800) {
      if ((units -= 2) < 0) break
      bytes.push(
        codePoint >> 0x6 | 0xC0,
        codePoint & 0x3F | 0x80
      )
    } else if (codePoint < 0x10000) {
      if ((units -= 3) < 0) break
      bytes.push(
        codePoint >> 0xC | 0xE0,
        codePoint >> 0x6 & 0x3F | 0x80,
        codePoint & 0x3F | 0x80
      )
    } else if (codePoint < 0x110000) {
      if ((units -= 4) < 0) break
      bytes.push(
        codePoint >> 0x12 | 0xF0,
        codePoint >> 0xC & 0x3F | 0x80,
        codePoint >> 0x6 & 0x3F | 0x80,
        codePoint & 0x3F | 0x80
      )
    } else {
      throw new Error('Invalid code point')
    }
  }

  return bytes
}

function asciiToBytes (str) {
  var byteArray = []
  for (var i = 0; i < str.length; ++i) {
    // Node's code seems to be doing this and not & 0x7F..
    byteArray.push(str.charCodeAt(i) & 0xFF)
  }
  return byteArray
}

function utf16leToBytes (str, units) {
  var c, hi, lo
  var byteArray = []
  for (var i = 0; i < str.length; ++i) {
    if ((units -= 2) < 0) break

    c = str.charCodeAt(i)
    hi = c >> 8
    lo = c % 256
    byteArray.push(lo)
    byteArray.push(hi)
  }

  return byteArray
}

function base64ToBytes (str) {
  return base64.toByteArray(base64clean(str))
}

function blitBuffer (src, dst, offset, length) {
  for (var i = 0; i < length; ++i) {
    if ((i + offset >= dst.length) || (i >= src.length)) break
    dst[i + offset] = src[i]
  }
  return i
}

// ArrayBuffer or Uint8Array objects from other contexts (i.e. iframes) do not pass
// the `instanceof` check but they should be treated as of that type.
// See: https://github.com/feross/buffer/issues/166
function isInstance (obj, type) {
  return obj instanceof type ||
    (obj != null && obj.constructor != null && obj.constructor.name != null &&
      obj.constructor.name === type.name)
}
function numberIsNaN (obj) {
  // For IE11 support
  return obj !== obj // eslint-disable-line no-self-compare
}

// Create lookup table for `toString('hex')`
// See: https://github.com/feross/buffer/issues/219
var hexSliceLookupTable = (function () {
  var alphabet = '0123456789abcdef'
  var table = new Array(256)
  for (var i = 0; i < 16; ++i) {
    var i16 = i * 16
    for (var j = 0; j < 16; ++j) {
      table[i16 + j] = alphabet[i] + alphabet[j]
    }
  }
  return table
})()


/***/ }),
/* 16 */
/***/ (function(__unused_webpack_module, exports) {

"use strict";


exports.byteLength = byteLength
exports.toByteArray = toByteArray
exports.fromByteArray = fromByteArray

var lookup = []
var revLookup = []
var Arr = typeof Uint8Array !== 'undefined' ? Uint8Array : Array

var code = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/'
for (var i = 0, len = code.length; i < len; ++i) {
  lookup[i] = code[i]
  revLookup[code.charCodeAt(i)] = i
}

// Support decoding URL-safe base64 strings, as Node.js does.
// See: https://en.wikipedia.org/wiki/Base64#URL_applications
revLookup['-'.charCodeAt(0)] = 62
revLookup['_'.charCodeAt(0)] = 63

function getLens (b64) {
  var len = b64.length

  if (len % 4 > 0) {
    throw new Error('Invalid string. Length must be a multiple of 4')
  }

  // Trim off extra bytes after placeholder bytes are found
  // See: https://github.com/beatgammit/base64-js/issues/42
  var validLen = b64.indexOf('=')
  if (validLen === -1) validLen = len

  var placeHoldersLen = validLen === len
    ? 0
    : 4 - (validLen % 4)

  return [validLen, placeHoldersLen]
}

// base64 is 4/3 + up to two characters of the original data
function byteLength (b64) {
  var lens = getLens(b64)
  var validLen = lens[0]
  var placeHoldersLen = lens[1]
  return ((validLen + placeHoldersLen) * 3 / 4) - placeHoldersLen
}

function _byteLength (b64, validLen, placeHoldersLen) {
  return ((validLen + placeHoldersLen) * 3 / 4) - placeHoldersLen
}

function toByteArray (b64) {
  var tmp
  var lens = getLens(b64)
  var validLen = lens[0]
  var placeHoldersLen = lens[1]

  var arr = new Arr(_byteLength(b64, validLen, placeHoldersLen))

  var curByte = 0

  // if there are placeholders, only get up to the last complete 4 chars
  var len = placeHoldersLen > 0
    ? validLen - 4
    : validLen

  var i
  for (i = 0; i < len; i += 4) {
    tmp =
      (revLookup[b64.charCodeAt(i)] << 18) |
      (revLookup[b64.charCodeAt(i + 1)] << 12) |
      (revLookup[b64.charCodeAt(i + 2)] << 6) |
      revLookup[b64.charCodeAt(i + 3)]
    arr[curByte++] = (tmp >> 16) & 0xFF
    arr[curByte++] = (tmp >> 8) & 0xFF
    arr[curByte++] = tmp & 0xFF
  }

  if (placeHoldersLen === 2) {
    tmp =
      (revLookup[b64.charCodeAt(i)] << 2) |
      (revLookup[b64.charCodeAt(i + 1)] >> 4)
    arr[curByte++] = tmp & 0xFF
  }

  if (placeHoldersLen === 1) {
    tmp =
      (revLookup[b64.charCodeAt(i)] << 10) |
      (revLookup[b64.charCodeAt(i + 1)] << 4) |
      (revLookup[b64.charCodeAt(i + 2)] >> 2)
    arr[curByte++] = (tmp >> 8) & 0xFF
    arr[curByte++] = tmp & 0xFF
  }

  return arr
}

function tripletToBase64 (num) {
  return lookup[num >> 18 & 0x3F] +
    lookup[num >> 12 & 0x3F] +
    lookup[num >> 6 & 0x3F] +
    lookup[num & 0x3F]
}

function encodeChunk (uint8, start, end) {
  var tmp
  var output = []
  for (var i = start; i < end; i += 3) {
    tmp =
      ((uint8[i] << 16) & 0xFF0000) +
      ((uint8[i + 1] << 8) & 0xFF00) +
      (uint8[i + 2] & 0xFF)
    output.push(tripletToBase64(tmp))
  }
  return output.join('')
}

function fromByteArray (uint8) {
  var tmp
  var len = uint8.length
  var extraBytes = len % 3 // if we have 1 byte left, pad 2 bytes
  var parts = []
  var maxChunkLength = 16383 // must be multiple of 3

  // go through the array every three bytes, we'll deal with trailing stuff later
  for (var i = 0, len2 = len - extraBytes; i < len2; i += maxChunkLength) {
    parts.push(encodeChunk(uint8, i, (i + maxChunkLength) > len2 ? len2 : (i + maxChunkLength)))
  }

  // pad the end with zeros, but make sure to not forget the extra bytes
  if (extraBytes === 1) {
    tmp = uint8[len - 1]
    parts.push(
      lookup[tmp >> 2] +
      lookup[(tmp << 4) & 0x3F] +
      '=='
    )
  } else if (extraBytes === 2) {
    tmp = (uint8[len - 2] << 8) + uint8[len - 1]
    parts.push(
      lookup[tmp >> 10] +
      lookup[(tmp >> 4) & 0x3F] +
      lookup[(tmp << 2) & 0x3F] +
      '='
    )
  }

  return parts.join('')
}


/***/ }),
/* 17 */
/***/ (function(__unused_webpack_module, exports) {

/*! ieee754. BSD-3-Clause License. Feross Aboukhadijeh <https://feross.org/opensource> */
exports.read = function (buffer, offset, isLE, mLen, nBytes) {
  var e, m
  var eLen = (nBytes * 8) - mLen - 1
  var eMax = (1 << eLen) - 1
  var eBias = eMax >> 1
  var nBits = -7
  var i = isLE ? (nBytes - 1) : 0
  var d = isLE ? -1 : 1
  var s = buffer[offset + i]

  i += d

  e = s & ((1 << (-nBits)) - 1)
  s >>= (-nBits)
  nBits += eLen
  for (; nBits > 0; e = (e * 256) + buffer[offset + i], i += d, nBits -= 8) {}

  m = e & ((1 << (-nBits)) - 1)
  e >>= (-nBits)
  nBits += mLen
  for (; nBits > 0; m = (m * 256) + buffer[offset + i], i += d, nBits -= 8) {}

  if (e === 0) {
    e = 1 - eBias
  } else if (e === eMax) {
    return m ? NaN : ((s ? -1 : 1) * Infinity)
  } else {
    m = m + Math.pow(2, mLen)
    e = e - eBias
  }
  return (s ? -1 : 1) * m * Math.pow(2, e - mLen)
}

exports.write = function (buffer, value, offset, isLE, mLen, nBytes) {
  var e, m, c
  var eLen = (nBytes * 8) - mLen - 1
  var eMax = (1 << eLen) - 1
  var eBias = eMax >> 1
  var rt = (mLen === 23 ? Math.pow(2, -24) - Math.pow(2, -77) : 0)
  var i = isLE ? 0 : (nBytes - 1)
  var d = isLE ? 1 : -1
  var s = value < 0 || (value === 0 && 1 / value < 0) ? 1 : 0

  value = Math.abs(value)

  if (isNaN(value) || value === Infinity) {
    m = isNaN(value) ? 1 : 0
    e = eMax
  } else {
    e = Math.floor(Math.log(value) / Math.LN2)
    if (value * (c = Math.pow(2, -e)) < 1) {
      e--
      c *= 2
    }
    if (e + eBias >= 1) {
      value += rt / c
    } else {
      value += rt * Math.pow(2, 1 - eBias)
    }
    if (value * c >= 2) {
      e++
      c /= 2
    }

    if (e + eBias >= eMax) {
      m = 0
      e = eMax
    } else if (e + eBias >= 1) {
      m = ((value * c) - 1) * Math.pow(2, mLen)
      e = e + eBias
    } else {
      m = value * Math.pow(2, eBias - 1) * Math.pow(2, mLen)
      e = 0
    }
  }

  for (; mLen >= 8; buffer[offset + i] = m & 0xff, i += d, m /= 256, mLen -= 8) {}

  e = (e << mLen) | m
  eLen += mLen
  for (; eLen > 0; buffer[offset + i] = e & 0xff, i += d, e /= 256, eLen -= 8) {}

  buffer[offset + i - d] |= s * 128
}


/***/ }),
/* 18 */
/***/ (function(module, __unused_webpack_exports, __webpack_require__) {

var Reporter = (__webpack_require__(12).Reporter);
var EncoderBuffer = (__webpack_require__(12).EncoderBuffer);
var DecoderBuffer = (__webpack_require__(12).DecoderBuffer);
var assert = __webpack_require__(19);

// Supported tags
var tags = [
  'seq', 'seqof', 'set', 'setof', 'objid', 'bool',
  'gentime', 'utctime', 'null_', 'enum', 'int', 'objDesc',
  'bitstr', 'bmpstr', 'charstr', 'genstr', 'graphstr', 'ia5str', 'iso646str',
  'numstr', 'octstr', 'printstr', 't61str', 'unistr', 'utf8str', 'videostr'
];

// Public methods list
var methods = [
  'key', 'obj', 'use', 'optional', 'explicit', 'implicit', 'def', 'choice',
  'any', 'contains'
].concat(tags);

// Overrided methods list
var overrided = [
  '_peekTag', '_decodeTag', '_use',
  '_decodeStr', '_decodeObjid', '_decodeTime',
  '_decodeNull', '_decodeInt', '_decodeBool', '_decodeList',

  '_encodeComposite', '_encodeStr', '_encodeObjid', '_encodeTime',
  '_encodeNull', '_encodeInt', '_encodeBool'
];

function Node(enc, parent) {
  var state = {};
  this._baseState = state;

  state.enc = enc;

  state.parent = parent || null;
  state.children = null;

  // State
  state.tag = null;
  state.args = null;
  state.reverseArgs = null;
  state.choice = null;
  state.optional = false;
  state.any = false;
  state.obj = false;
  state.use = null;
  state.useDecoder = null;
  state.key = null;
  state['default'] = null;
  state.explicit = null;
  state.implicit = null;
  state.contains = null;

  // Should create new instance on each method
  if (!state.parent) {
    state.children = [];
    this._wrap();
  }
}
module.exports = Node;

var stateProps = [
  'enc', 'parent', 'children', 'tag', 'args', 'reverseArgs', 'choice',
  'optional', 'any', 'obj', 'use', 'alteredUse', 'key', 'default', 'explicit',
  'implicit', 'contains'
];

Node.prototype.clone = function clone() {
  var state = this._baseState;
  var cstate = {};
  stateProps.forEach(function(prop) {
    cstate[prop] = state[prop];
  });
  var res = new this.constructor(cstate.parent);
  res._baseState = cstate;
  return res;
};

Node.prototype._wrap = function wrap() {
  var state = this._baseState;
  methods.forEach(function(method) {
    this[method] = function _wrappedMethod() {
      var clone = new this.constructor(this);
      state.children.push(clone);
      return clone[method].apply(clone, arguments);
    };
  }, this);
};

Node.prototype._init = function init(body) {
  var state = this._baseState;

  assert(state.parent === null);
  body.call(this);

  // Filter children
  state.children = state.children.filter(function(child) {
    return child._baseState.parent === this;
  }, this);
  assert.equal(state.children.length, 1, 'Root node can have only one child');
};

Node.prototype._useArgs = function useArgs(args) {
  var state = this._baseState;

  // Filter children and args
  var children = args.filter(function(arg) {
    return arg instanceof this.constructor;
  }, this);
  args = args.filter(function(arg) {
    return !(arg instanceof this.constructor);
  }, this);

  if (children.length !== 0) {
    assert(state.children === null);
    state.children = children;

    // Replace parent to maintain backward link
    children.forEach(function(child) {
      child._baseState.parent = this;
    }, this);
  }
  if (args.length !== 0) {
    assert(state.args === null);
    state.args = args;
    state.reverseArgs = args.map(function(arg) {
      if (typeof arg !== 'object' || arg.constructor !== Object)
        return arg;

      var res = {};
      Object.keys(arg).forEach(function(key) {
        if (key == (key | 0))
          key |= 0;
        var value = arg[key];
        res[value] = key;
      });
      return res;
    });
  }
};

//
// Overrided methods
//

overrided.forEach(function(method) {
  Node.prototype[method] = function _overrided() {
    var state = this._baseState;
    throw new Error(method + ' not implemented for encoding: ' + state.enc);
  };
});

//
// Public methods
//

tags.forEach(function(tag) {
  Node.prototype[tag] = function _tagMethod() {
    var state = this._baseState;
    var args = Array.prototype.slice.call(arguments);

    assert(state.tag === null);
    state.tag = tag;

    this._useArgs(args);

    return this;
  };
});

Node.prototype.use = function use(item) {
  assert(item);
  var state = this._baseState;

  assert(state.use === null);
  state.use = item;

  return this;
};

Node.prototype.optional = function optional() {
  var state = this._baseState;

  state.optional = true;

  return this;
};

Node.prototype.def = function def(val) {
  var state = this._baseState;

  assert(state['default'] === null);
  state['default'] = val;
  state.optional = true;

  return this;
};

Node.prototype.explicit = function explicit(num) {
  var state = this._baseState;

  assert(state.explicit === null && state.implicit === null);
  state.explicit = num;

  return this;
};

Node.prototype.implicit = function implicit(num) {
  var state = this._baseState;

  assert(state.explicit === null && state.implicit === null);
  state.implicit = num;

  return this;
};

Node.prototype.obj = function obj() {
  var state = this._baseState;
  var args = Array.prototype.slice.call(arguments);

  state.obj = true;

  if (args.length !== 0)
    this._useArgs(args);

  return this;
};

Node.prototype.key = function key(newKey) {
  var state = this._baseState;

  assert(state.key === null);
  state.key = newKey;

  return this;
};

Node.prototype.any = function any() {
  var state = this._baseState;

  state.any = true;

  return this;
};

Node.prototype.choice = function choice(obj) {
  var state = this._baseState;

  assert(state.choice === null);
  state.choice = obj;
  this._useArgs(Object.keys(obj).map(function(key) {
    return obj[key];
  }));

  return this;
};

Node.prototype.contains = function contains(item) {
  var state = this._baseState;

  assert(state.use === null);
  state.contains = item;

  return this;
};

//
// Decoding
//

Node.prototype._decode = function decode(input, options) {
  var state = this._baseState;

  // Decode root node
  if (state.parent === null)
    return input.wrapResult(state.children[0]._decode(input, options));

  var result = state['default'];
  var present = true;

  var prevKey = null;
  if (state.key !== null)
    prevKey = input.enterKey(state.key);

  // Check if tag is there
  if (state.optional) {
    var tag = null;
    if (state.explicit !== null)
      tag = state.explicit;
    else if (state.implicit !== null)
      tag = state.implicit;
    else if (state.tag !== null)
      tag = state.tag;

    if (tag === null && !state.any) {
      // Trial and Error
      var save = input.save();
      try {
        if (state.choice === null)
          this._decodeGeneric(state.tag, input, options);
        else
          this._decodeChoice(input, options);
        present = true;
      } catch (e) {
        present = false;
      }
      input.restore(save);
    } else {
      present = this._peekTag(input, tag, state.any);

      if (input.isError(present))
        return present;
    }
  }

  // Push object on stack
  var prevObj;
  if (state.obj && present)
    prevObj = input.enterObject();

  if (present) {
    // Unwrap explicit values
    if (state.explicit !== null) {
      var explicit = this._decodeTag(input, state.explicit);
      if (input.isError(explicit))
        return explicit;
      input = explicit;
    }

    var start = input.offset;

    // Unwrap implicit and normal values
    if (state.use === null && state.choice === null) {
      if (state.any)
        var save = input.save();
      var body = this._decodeTag(
        input,
        state.implicit !== null ? state.implicit : state.tag,
        state.any
      );
      if (input.isError(body))
        return body;

      if (state.any)
        result = input.raw(save);
      else
        input = body;
    }

    if (options && options.track && state.tag !== null)
      options.track(input.path(), start, input.length, 'tagged');

    if (options && options.track && state.tag !== null)
      options.track(input.path(), input.offset, input.length, 'content');

    // Select proper method for tag
    if (state.any)
      result = result;
    else if (state.choice === null)
      result = this._decodeGeneric(state.tag, input, options);
    else
      result = this._decodeChoice(input, options);

    if (input.isError(result))
      return result;

    // Decode children
    if (!state.any && state.choice === null && state.children !== null) {
      state.children.forEach(function decodeChildren(child) {
        // NOTE: We are ignoring errors here, to let parser continue with other
        // parts of encoded data
        child._decode(input, options);
      });
    }

    // Decode contained/encoded by schema, only in bit or octet strings
    if (state.contains && (state.tag === 'octstr' || state.tag === 'bitstr')) {
      var data = new DecoderBuffer(result);
      result = this._getUse(state.contains, input._reporterState.obj)
          ._decode(data, options);
    }
  }

  // Pop object
  if (state.obj && present)
    result = input.leaveObject(prevObj);

  // Set key
  if (state.key !== null && (result !== null || present === true))
    input.leaveKey(prevKey, state.key, result);
  else if (prevKey !== null)
    input.exitKey(prevKey);

  return result;
};

Node.prototype._decodeGeneric = function decodeGeneric(tag, input, options) {
  var state = this._baseState;

  if (tag === 'seq' || tag === 'set')
    return null;
  if (tag === 'seqof' || tag === 'setof')
    return this._decodeList(input, tag, state.args[0], options);
  else if (/str$/.test(tag))
    return this._decodeStr(input, tag, options);
  else if (tag === 'objid' && state.args)
    return this._decodeObjid(input, state.args[0], state.args[1], options);
  else if (tag === 'objid')
    return this._decodeObjid(input, null, null, options);
  else if (tag === 'gentime' || tag === 'utctime')
    return this._decodeTime(input, tag, options);
  else if (tag === 'null_')
    return this._decodeNull(input, options);
  else if (tag === 'bool')
    return this._decodeBool(input, options);
  else if (tag === 'objDesc')
    return this._decodeStr(input, tag, options);
  else if (tag === 'int' || tag === 'enum')
    return this._decodeInt(input, state.args && state.args[0], options);

  if (state.use !== null) {
    return this._getUse(state.use, input._reporterState.obj)
        ._decode(input, options);
  } else {
    return input.error('unknown tag: ' + tag);
  }
};

Node.prototype._getUse = function _getUse(entity, obj) {

  var state = this._baseState;
  // Create altered use decoder if implicit is set
  state.useDecoder = this._use(entity, obj);
  assert(state.useDecoder._baseState.parent === null);
  state.useDecoder = state.useDecoder._baseState.children[0];
  if (state.implicit !== state.useDecoder._baseState.implicit) {
    state.useDecoder = state.useDecoder.clone();
    state.useDecoder._baseState.implicit = state.implicit;
  }
  return state.useDecoder;
};

Node.prototype._decodeChoice = function decodeChoice(input, options) {
  var state = this._baseState;
  var result = null;
  var match = false;

  Object.keys(state.choice).some(function(key) {
    var save = input.save();
    var node = state.choice[key];
    try {
      var value = node._decode(input, options);
      if (input.isError(value))
        return false;

      result = { type: key, value: value };
      match = true;
    } catch (e) {
      input.restore(save);
      return false;
    }
    return true;
  }, this);

  if (!match)
    return input.error('Choice not matched');

  return result;
};

//
// Encoding
//

Node.prototype._createEncoderBuffer = function createEncoderBuffer(data) {
  return new EncoderBuffer(data, this.reporter);
};

Node.prototype._encode = function encode(data, reporter, parent) {
  var state = this._baseState;
  if (state['default'] !== null && state['default'] === data)
    return;

  var result = this._encodeValue(data, reporter, parent);
  if (result === undefined)
    return;

  if (this._skipDefault(result, reporter, parent))
    return;

  return result;
};

Node.prototype._encodeValue = function encode(data, reporter, parent) {
  var state = this._baseState;

  // Decode root node
  if (state.parent === null)
    return state.children[0]._encode(data, reporter || new Reporter());

  var result = null;

  // Set reporter to share it with a child class
  this.reporter = reporter;

  // Check if data is there
  if (state.optional && data === undefined) {
    if (state['default'] !== null)
      data = state['default']
    else
      return;
  }

  // Encode children first
  var content = null;
  var primitive = false;
  if (state.any) {
    // Anything that was given is translated to buffer
    result = this._createEncoderBuffer(data);
  } else if (state.choice) {
    result = this._encodeChoice(data, reporter);
  } else if (state.contains) {
    content = this._getUse(state.contains, parent)._encode(data, reporter);
    primitive = true;
  } else if (state.children) {
    content = state.children.map(function(child) {
      if (child._baseState.tag === 'null_')
        return child._encode(null, reporter, data);

      if (child._baseState.key === null)
        return reporter.error('Child should have a key');
      var prevKey = reporter.enterKey(child._baseState.key);

      if (typeof data !== 'object')
        return reporter.error('Child expected, but input is not object');

      var res = child._encode(data[child._baseState.key], reporter, data);
      reporter.leaveKey(prevKey);

      return res;
    }, this).filter(function(child) {
      return child;
    });
    content = this._createEncoderBuffer(content);
  } else {
    if (state.tag === 'seqof' || state.tag === 'setof') {
      // TODO(indutny): this should be thrown on DSL level
      if (!(state.args && state.args.length === 1))
        return reporter.error('Too many args for : ' + state.tag);

      if (!Array.isArray(data))
        return reporter.error('seqof/setof, but data is not Array');

      var child = this.clone();
      child._baseState.implicit = null;
      content = this._createEncoderBuffer(data.map(function(item) {
        var state = this._baseState;

        return this._getUse(state.args[0], data)._encode(item, reporter);
      }, child));
    } else if (state.use !== null) {
      result = this._getUse(state.use, parent)._encode(data, reporter);
    } else {
      content = this._encodePrimitive(state.tag, data);
      primitive = true;
    }
  }

  // Encode data itself
  var result;
  if (!state.any && state.choice === null) {
    var tag = state.implicit !== null ? state.implicit : state.tag;
    var cls = state.implicit === null ? 'universal' : 'context';

    if (tag === null) {
      if (state.use === null)
        reporter.error('Tag could be omitted only for .use()');
    } else {
      if (state.use === null)
        result = this._encodeComposite(tag, primitive, cls, content);
    }
  }

  // Wrap in explicit
  if (state.explicit !== null)
    result = this._encodeComposite(state.explicit, false, 'context', result);

  return result;
};

Node.prototype._encodeChoice = function encodeChoice(data, reporter) {
  var state = this._baseState;

  var node = state.choice[data.type];
  if (!node) {
    assert(
        false,
        data.type + ' not found in ' +
            JSON.stringify(Object.keys(state.choice)));
  }
  return node._encode(data.value, reporter);
};

Node.prototype._encodePrimitive = function encodePrimitive(tag, data) {
  var state = this._baseState;

  if (/str$/.test(tag))
    return this._encodeStr(data, tag);
  else if (tag === 'objid' && state.args)
    return this._encodeObjid(data, state.reverseArgs[0], state.args[1]);
  else if (tag === 'objid')
    return this._encodeObjid(data, null, null);
  else if (tag === 'gentime' || tag === 'utctime')
    return this._encodeTime(data, tag);
  else if (tag === 'null_')
    return this._encodeNull();
  else if (tag === 'int' || tag === 'enum')
    return this._encodeInt(data, state.args && state.reverseArgs[0]);
  else if (tag === 'bool')
    return this._encodeBool(data);
  else if (tag === 'objDesc')
    return this._encodeStr(data, tag);
  else
    throw new Error('Unsupported tag: ' + tag);
};

Node.prototype._isNumstr = function isNumstr(str) {
  return /^[0-9 ]*$/.test(str);
};

Node.prototype._isPrintstr = function isPrintstr(str) {
  return /^[A-Za-z0-9 '\(\)\+,\-\.\/:=\?]*$/.test(str);
};


/***/ }),
/* 19 */
/***/ (function(module) {

module.exports = assert;

function assert(val, msg) {
  if (!val)
    throw new Error(msg || 'Assertion failed');
}

assert.equal = function assertEqual(l, r, msg) {
  if (l != r)
    throw new Error(msg || ('Assertion failed: ' + l + ' != ' + r));
};


/***/ }),
/* 20 */
/***/ (function(__unused_webpack_module, exports, __webpack_require__) {

var constants = exports;

// Helper
constants._reverse = function reverse(map) {
  var res = {};

  Object.keys(map).forEach(function(key) {
    // Convert key to integer if it is stringified
    if ((key | 0) == key)
      key = key | 0;

    var value = map[key];
    res[value] = key;
  });

  return res;
};

constants.der = __webpack_require__(21);


/***/ }),
/* 21 */
/***/ (function(__unused_webpack_module, exports, __webpack_require__) {

var constants = __webpack_require__(20);

exports.tagClass = {
  0: 'universal',
  1: 'application',
  2: 'context',
  3: 'private'
};
exports.tagClassByName = constants._reverse(exports.tagClass);

exports.tag = {
  0x00: 'end',
  0x01: 'bool',
  0x02: 'int',
  0x03: 'bitstr',
  0x04: 'octstr',
  0x05: 'null_',
  0x06: 'objid',
  0x07: 'objDesc',
  0x08: 'external',
  0x09: 'real',
  0x0a: 'enum',
  0x0b: 'embed',
  0x0c: 'utf8str',
  0x0d: 'relativeOid',
  0x10: 'seq',
  0x11: 'set',
  0x12: 'numstr',
  0x13: 'printstr',
  0x14: 't61str',
  0x15: 'videostr',
  0x16: 'ia5str',
  0x17: 'utctime',
  0x18: 'gentime',
  0x19: 'graphstr',
  0x1a: 'iso646str',
  0x1b: 'genstr',
  0x1c: 'unistr',
  0x1d: 'charstr',
  0x1e: 'bmpstr'
};
exports.tagByName = constants._reverse(exports.tag);


/***/ }),
/* 22 */
/***/ (function(__unused_webpack_module, exports, __webpack_require__) {

var decoders = exports;

decoders.der = __webpack_require__(23);
decoders.pem = __webpack_require__(24);


/***/ }),
/* 23 */
/***/ (function(module, __unused_webpack_exports, __webpack_require__) {

var inherits = __webpack_require__(10);

var asn1 = __webpack_require__(6);
var base = asn1.base;
var bignum = asn1.bignum;

// Import DER constants
var der = asn1.constants.der;

function DERDecoder(entity) {
  this.enc = 'der';
  this.name = entity.name;
  this.entity = entity;

  // Construct base tree
  this.tree = new DERNode();
  this.tree._init(entity.body);
};
module.exports = DERDecoder;

DERDecoder.prototype.decode = function decode(data, options) {
  if (!(data instanceof base.DecoderBuffer))
    data = new base.DecoderBuffer(data, options);

  return this.tree._decode(data, options);
};

// Tree methods

function DERNode(parent) {
  base.Node.call(this, 'der', parent);
}
inherits(DERNode, base.Node);

DERNode.prototype._peekTag = function peekTag(buffer, tag, any) {
  if (buffer.isEmpty())
    return false;

  var state = buffer.save();
  var decodedTag = derDecodeTag(buffer, 'Failed to peek tag: "' + tag + '"');
  if (buffer.isError(decodedTag))
    return decodedTag;

  buffer.restore(state);

  return decodedTag.tag === tag || decodedTag.tagStr === tag ||
    (decodedTag.tagStr + 'of') === tag || any;
};

DERNode.prototype._decodeTag = function decodeTag(buffer, tag, any) {
  var decodedTag = derDecodeTag(buffer,
                                'Failed to decode tag of "' + tag + '"');
  if (buffer.isError(decodedTag))
    return decodedTag;

  var len = derDecodeLen(buffer,
                         decodedTag.primitive,
                         'Failed to get length of "' + tag + '"');

  // Failure
  if (buffer.isError(len))
    return len;

  if (!any &&
      decodedTag.tag !== tag &&
      decodedTag.tagStr !== tag &&
      decodedTag.tagStr + 'of' !== tag) {
    return buffer.error('Failed to match tag: "' + tag + '"');
  }

  if (decodedTag.primitive || len !== null)
    return buffer.skip(len, 'Failed to match body of: "' + tag + '"');

  // Indefinite length... find END tag
  var state = buffer.save();
  var res = this._skipUntilEnd(
      buffer,
      'Failed to skip indefinite length body: "' + this.tag + '"');
  if (buffer.isError(res))
    return res;

  len = buffer.offset - state.offset;
  buffer.restore(state);
  return buffer.skip(len, 'Failed to match body of: "' + tag + '"');
};

DERNode.prototype._skipUntilEnd = function skipUntilEnd(buffer, fail) {
  while (true) {
    var tag = derDecodeTag(buffer, fail);
    if (buffer.isError(tag))
      return tag;
    var len = derDecodeLen(buffer, tag.primitive, fail);
    if (buffer.isError(len))
      return len;

    var res;
    if (tag.primitive || len !== null)
      res = buffer.skip(len)
    else
      res = this._skipUntilEnd(buffer, fail);

    // Failure
    if (buffer.isError(res))
      return res;

    if (tag.tagStr === 'end')
      break;
  }
};

DERNode.prototype._decodeList = function decodeList(buffer, tag, decoder,
                                                    options) {
  var result = [];
  while (!buffer.isEmpty()) {
    var possibleEnd = this._peekTag(buffer, 'end');
    if (buffer.isError(possibleEnd))
      return possibleEnd;

    var res = decoder.decode(buffer, 'der', options);
    if (buffer.isError(res) && possibleEnd)
      break;
    result.push(res);
  }
  return result;
};

DERNode.prototype._decodeStr = function decodeStr(buffer, tag) {
  if (tag === 'bitstr') {
    var unused = buffer.readUInt8();
    if (buffer.isError(unused))
      return unused;
    return { unused: unused, data: buffer.raw() };
  } else if (tag === 'bmpstr') {
    var raw = buffer.raw();
    if (raw.length % 2 === 1)
      return buffer.error('Decoding of string type: bmpstr length mismatch');

    var str = '';
    for (var i = 0; i < raw.length / 2; i++) {
      str += String.fromCharCode(raw.readUInt16BE(i * 2));
    }
    return str;
  } else if (tag === 'numstr') {
    var numstr = buffer.raw().toString('ascii');
    if (!this._isNumstr(numstr)) {
      return buffer.error('Decoding of string type: ' +
                          'numstr unsupported characters');
    }
    return numstr;
  } else if (tag === 'octstr') {
    return buffer.raw();
  } else if (tag === 'objDesc') {
    return buffer.raw();
  } else if (tag === 'printstr') {
    var printstr = buffer.raw().toString('ascii');
    if (!this._isPrintstr(printstr)) {
      return buffer.error('Decoding of string type: ' +
                          'printstr unsupported characters');
    }
    return printstr;
  } else if (/str$/.test(tag)) {
    return buffer.raw().toString();
  } else {
    return buffer.error('Decoding of string type: ' + tag + ' unsupported');
  }
};

DERNode.prototype._decodeObjid = function decodeObjid(buffer, values, relative) {
  var result;
  var identifiers = [];
  var ident = 0;
  while (!buffer.isEmpty()) {
    var subident = buffer.readUInt8();
    ident <<= 7;
    ident |= subident & 0x7f;
    if ((subident & 0x80) === 0) {
      identifiers.push(ident);
      ident = 0;
    }
  }
  if (subident & 0x80)
    identifiers.push(ident);

  var first = (identifiers[0] / 40) | 0;
  var second = identifiers[0] % 40;

  if (relative)
    result = identifiers;
  else
    result = [first, second].concat(identifiers.slice(1));

  if (values) {
    var tmp = values[result.join(' ')];
    if (tmp === undefined)
      tmp = values[result.join('.')];
    if (tmp !== undefined)
      result = tmp;
  }

  return result;
};

DERNode.prototype._decodeTime = function decodeTime(buffer, tag) {
  var str = buffer.raw().toString();
  if (tag === 'gentime') {
    var year = str.slice(0, 4) | 0;
    var mon = str.slice(4, 6) | 0;
    var day = str.slice(6, 8) | 0;
    var hour = str.slice(8, 10) | 0;
    var min = str.slice(10, 12) | 0;
    var sec = str.slice(12, 14) | 0;
  } else if (tag === 'utctime') {
    var year = str.slice(0, 2) | 0;
    var mon = str.slice(2, 4) | 0;
    var day = str.slice(4, 6) | 0;
    var hour = str.slice(6, 8) | 0;
    var min = str.slice(8, 10) | 0;
    var sec = str.slice(10, 12) | 0;
    if (year < 70)
      year = 2000 + year;
    else
      year = 1900 + year;
  } else {
    return buffer.error('Decoding ' + tag + ' time is not supported yet');
  }

  return Date.UTC(year, mon - 1, day, hour, min, sec, 0);
};

DERNode.prototype._decodeNull = function decodeNull(buffer) {
  return null;
};

DERNode.prototype._decodeBool = function decodeBool(buffer) {
  var res = buffer.readUInt8();
  if (buffer.isError(res))
    return res;
  else
    return res !== 0;
};

DERNode.prototype._decodeInt = function decodeInt(buffer, values) {
  // Bigint, return as it is (assume big endian)
  var raw = buffer.raw();
  var res = new bignum(raw);

  if (values)
    res = values[res.toString(10)] || res;

  return res;
};

DERNode.prototype._use = function use(entity, obj) {
  if (typeof entity === 'function')
    entity = entity(obj);
  return entity._getDecoder('der').tree;
};

// Utility methods

function derDecodeTag(buf, fail) {
  var tag = buf.readUInt8(fail);
  if (buf.isError(tag))
    return tag;

  var cls = der.tagClass[tag >> 6];
  var primitive = (tag & 0x20) === 0;

  // Multi-octet tag - load
  if ((tag & 0x1f) === 0x1f) {
    var oct = tag;
    tag = 0;
    while ((oct & 0x80) === 0x80) {
      oct = buf.readUInt8(fail);
      if (buf.isError(oct))
        return oct;

      tag <<= 7;
      tag |= oct & 0x7f;
    }
  } else {
    tag &= 0x1f;
  }
  var tagStr = der.tag[tag];

  return {
    cls: cls,
    primitive: primitive,
    tag: tag,
    tagStr: tagStr
  };
}

function derDecodeLen(buf, primitive, fail) {
  var len = buf.readUInt8(fail);
  if (buf.isError(len))
    return len;

  // Indefinite form
  if (!primitive && len === 0x80)
    return null;

  // Definite form
  if ((len & 0x80) === 0) {
    // Short form
    return len;
  }

  // Long form
  var num = len & 0x7f;
  if (num > 4)
    return buf.error('length octect is too long');

  len = 0;
  for (var i = 0; i < num; i++) {
    len <<= 8;
    var j = buf.readUInt8(fail);
    if (buf.isError(j))
      return j;
    len |= j;
  }

  return len;
}


/***/ }),
/* 24 */
/***/ (function(module, __unused_webpack_exports, __webpack_require__) {

var inherits = __webpack_require__(10);
var Buffer = (__webpack_require__(15).Buffer);

var DERDecoder = __webpack_require__(23);

function PEMDecoder(entity) {
  DERDecoder.call(this, entity);
  this.enc = 'pem';
};
inherits(PEMDecoder, DERDecoder);
module.exports = PEMDecoder;

PEMDecoder.prototype.decode = function decode(data, options) {
  var lines = data.toString().split(/[\r\n]+/g);

  var label = options.label.toUpperCase();

  var re = /^-----(BEGIN|END) ([^-]+)-----$/;
  var start = -1;
  var end = -1;
  for (var i = 0; i < lines.length; i++) {
    var match = lines[i].match(re);
    if (match === null)
      continue;

    if (match[2] !== label)
      continue;

    if (start === -1) {
      if (match[1] !== 'BEGIN')
        break;
      start = i;
    } else {
      if (match[1] !== 'END')
        break;
      end = i;
      break;
    }
  }
  if (start === -1 || end === -1)
    throw new Error('PEM section not found for: ' + label);

  var base64 = lines.slice(start + 1, end).join('');
  // Remove excessive symbols
  base64.replace(/[^a-z0-9\+\/=]+/gi, '');

  var input = new Buffer(base64, 'base64');
  return DERDecoder.prototype.decode.call(this, input, options);
};


/***/ }),
/* 25 */
/***/ (function(__unused_webpack_module, exports, __webpack_require__) {

var encoders = exports;

encoders.der = __webpack_require__(26);
encoders.pem = __webpack_require__(27);


/***/ }),
/* 26 */
/***/ (function(module, __unused_webpack_exports, __webpack_require__) {

var inherits = __webpack_require__(10);
var Buffer = (__webpack_require__(15).Buffer);

var asn1 = __webpack_require__(6);
var base = asn1.base;

// Import DER constants
var der = asn1.constants.der;

function DEREncoder(entity) {
  this.enc = 'der';
  this.name = entity.name;
  this.entity = entity;

  // Construct base tree
  this.tree = new DERNode();
  this.tree._init(entity.body);
};
module.exports = DEREncoder;

DEREncoder.prototype.encode = function encode(data, reporter) {
  return this.tree._encode(data, reporter).join();
};

// Tree methods

function DERNode(parent) {
  base.Node.call(this, 'der', parent);
}
inherits(DERNode, base.Node);

DERNode.prototype._encodeComposite = function encodeComposite(tag,
                                                              primitive,
                                                              cls,
                                                              content) {
  var encodedTag = encodeTag(tag, primitive, cls, this.reporter);

  // Short form
  if (content.length < 0x80) {
    var header = new Buffer(2);
    header[0] = encodedTag;
    header[1] = content.length;
    return this._createEncoderBuffer([ header, content ]);
  }

  // Long form
  // Count octets required to store length
  var lenOctets = 1;
  for (var i = content.length; i >= 0x100; i >>= 8)
    lenOctets++;

  var header = new Buffer(1 + 1 + lenOctets);
  header[0] = encodedTag;
  header[1] = 0x80 | lenOctets;

  for (var i = 1 + lenOctets, j = content.length; j > 0; i--, j >>= 8)
    header[i] = j & 0xff;

  return this._createEncoderBuffer([ header, content ]);
};

DERNode.prototype._encodeStr = function encodeStr(str, tag) {
  if (tag === 'bitstr') {
    return this._createEncoderBuffer([ str.unused | 0, str.data ]);
  } else if (tag === 'bmpstr') {
    var buf = new Buffer(str.length * 2);
    for (var i = 0; i < str.length; i++) {
      buf.writeUInt16BE(str.charCodeAt(i), i * 2);
    }
    return this._createEncoderBuffer(buf);
  } else if (tag === 'numstr') {
    if (!this._isNumstr(str)) {
      return this.reporter.error('Encoding of string type: numstr supports ' +
                                 'only digits and space');
    }
    return this._createEncoderBuffer(str);
  } else if (tag === 'printstr') {
    if (!this._isPrintstr(str)) {
      return this.reporter.error('Encoding of string type: printstr supports ' +
                                 'only latin upper and lower case letters, ' +
                                 'digits, space, apostrophe, left and rigth ' +
                                 'parenthesis, plus sign, comma, hyphen, ' +
                                 'dot, slash, colon, equal sign, ' +
                                 'question mark');
    }
    return this._createEncoderBuffer(str);
  } else if (/str$/.test(tag)) {
    return this._createEncoderBuffer(str);
  } else if (tag === 'objDesc') {
    return this._createEncoderBuffer(str);
  } else {
    return this.reporter.error('Encoding of string type: ' + tag +
                               ' unsupported');
  }
};

DERNode.prototype._encodeObjid = function encodeObjid(id, values, relative) {
  if (typeof id === 'string') {
    if (!values)
      return this.reporter.error('string objid given, but no values map found');
    if (!values.hasOwnProperty(id))
      return this.reporter.error('objid not found in values map');
    id = values[id].split(/[\s\.]+/g);
    for (var i = 0; i < id.length; i++)
      id[i] |= 0;
  } else if (Array.isArray(id)) {
    id = id.slice();
    for (var i = 0; i < id.length; i++)
      id[i] |= 0;
  }

  if (!Array.isArray(id)) {
    return this.reporter.error('objid() should be either array or string, ' +
                               'got: ' + JSON.stringify(id));
  }

  if (!relative) {
    if (id[1] >= 40)
      return this.reporter.error('Second objid identifier OOB');
    id.splice(0, 2, id[0] * 40 + id[1]);
  }

  // Count number of octets
  var size = 0;
  for (var i = 0; i < id.length; i++) {
    var ident = id[i];
    for (size++; ident >= 0x80; ident >>= 7)
      size++;
  }

  var objid = new Buffer(size);
  var offset = objid.length - 1;
  for (var i = id.length - 1; i >= 0; i--) {
    var ident = id[i];
    objid[offset--] = ident & 0x7f;
    while ((ident >>= 7) > 0)
      objid[offset--] = 0x80 | (ident & 0x7f);
  }

  return this._createEncoderBuffer(objid);
};

function two(num) {
  if (num < 10)
    return '0' + num;
  else
    return num;
}

DERNode.prototype._encodeTime = function encodeTime(time, tag) {
  var str;
  var date = new Date(time);

  if (tag === 'gentime') {
    str = [
      two(date.getFullYear()),
      two(date.getUTCMonth() + 1),
      two(date.getUTCDate()),
      two(date.getUTCHours()),
      two(date.getUTCMinutes()),
      two(date.getUTCSeconds()),
      'Z'
    ].join('');
  } else if (tag === 'utctime') {
    str = [
      two(date.getFullYear() % 100),
      two(date.getUTCMonth() + 1),
      two(date.getUTCDate()),
      two(date.getUTCHours()),
      two(date.getUTCMinutes()),
      two(date.getUTCSeconds()),
      'Z'
    ].join('');
  } else {
    this.reporter.error('Encoding ' + tag + ' time is not supported yet');
  }

  return this._encodeStr(str, 'octstr');
};

DERNode.prototype._encodeNull = function encodeNull() {
  return this._createEncoderBuffer('');
};

DERNode.prototype._encodeInt = function encodeInt(num, values) {
  if (typeof num === 'string') {
    if (!values)
      return this.reporter.error('String int or enum given, but no values map');
    if (!values.hasOwnProperty(num)) {
      return this.reporter.error('Values map doesn\'t contain: ' +
                                 JSON.stringify(num));
    }
    num = values[num];
  }

  // Bignum, assume big endian
  if (typeof num !== 'number' && !Buffer.isBuffer(num)) {
    var numArray = num.toArray();
    if (!num.sign && numArray[0] & 0x80) {
      numArray.unshift(0);
    }
    num = new Buffer(numArray);
  }

  if (Buffer.isBuffer(num)) {
    var size = num.length;
    if (num.length === 0)
      size++;

    var out = new Buffer(size);
    num.copy(out);
    if (num.length === 0)
      out[0] = 0
    return this._createEncoderBuffer(out);
  }

  if (num < 0x80)
    return this._createEncoderBuffer(num);

  if (num < 0x100)
    return this._createEncoderBuffer([0, num]);

  var size = 1;
  for (var i = num; i >= 0x100; i >>= 8)
    size++;

  var out = new Array(size);
  for (var i = out.length - 1; i >= 0; i--) {
    out[i] = num & 0xff;
    num >>= 8;
  }
  if(out[0] & 0x80) {
    out.unshift(0);
  }

  return this._createEncoderBuffer(new Buffer(out));
};

DERNode.prototype._encodeBool = function encodeBool(value) {
  return this._createEncoderBuffer(value ? 0xff : 0);
};

DERNode.prototype._use = function use(entity, obj) {
  if (typeof entity === 'function')
    entity = entity(obj);
  return entity._getEncoder('der').tree;
};

DERNode.prototype._skipDefault = function skipDefault(dataBuffer, reporter, parent) {
  var state = this._baseState;
  var i;
  if (state['default'] === null)
    return false;

  var data = dataBuffer.join();
  if (state.defaultBuffer === undefined)
    state.defaultBuffer = this._encodeValue(state['default'], reporter, parent).join();

  if (data.length !== state.defaultBuffer.length)
    return false;

  for (i=0; i < data.length; i++)
    if (data[i] !== state.defaultBuffer[i])
      return false;

  return true;
};

// Utility methods

function encodeTag(tag, primitive, cls, reporter) {
  var res;

  if (tag === 'seqof')
    tag = 'seq';
  else if (tag === 'setof')
    tag = 'set';

  if (der.tagByName.hasOwnProperty(tag))
    res = der.tagByName[tag];
  else if (typeof tag === 'number' && (tag | 0) === tag)
    res = tag;
  else
    return reporter.error('Unknown tag: ' + tag);

  if (res >= 0x1f)
    return reporter.error('Multi-octet tag encoding unsupported');

  if (!primitive)
    res |= 0x20;

  res |= (der.tagClassByName[cls || 'universal'] << 6);

  return res;
}


/***/ }),
/* 27 */
/***/ (function(module, __unused_webpack_exports, __webpack_require__) {

var inherits = __webpack_require__(10);

var DEREncoder = __webpack_require__(26);

function PEMEncoder(entity) {
  DEREncoder.call(this, entity);
  this.enc = 'pem';
};
inherits(PEMEncoder, DEREncoder);
module.exports = PEMEncoder;

PEMEncoder.prototype.encode = function encode(data, options) {
  var buf = DEREncoder.prototype.encode.call(this, data);

  var p = buf.toString('base64');
  var out = [ '-----BEGIN ' + options.label + '-----' ];
  for (var i = 0; i < p.length; i += 64)
    out.push(p.slice(i, i + 64));
  out.push('-----END ' + options.label + '-----');
  return out.join('\n');
};


/***/ }),
/* 28 */
/***/ (function(module) {

var sodiumUtil=function(){function r(e){if("function"==typeof TextDecoder)return new TextDecoder("utf-8",{fatal:!0}).decode(e);var n=8192,t=Math.ceil(e.length/n);if(t<=1)try{return decodeURIComponent(escape(String.fromCharCode.apply(null,e)))}catch(r){throw new TypeError("The encoded data was not valid.")}for(var o="",i=0,f=0;f<t;f++){var a=Array.prototype.slice.call(e,f*n+i,(f+1)*n+i);if(0!=a.length){var u,c=a.length,l=0;do{var s=a[--c];s>=240?(l=4,u=!0):s>=224?(l=3,u=!0):s>=192?(l=2,u=!0):s<128&&(l=1,u=!0)}while(!u);for(var d=l-(a.length-c),p=0;p<d;p++)i--,a.pop();o+=r(a)}}return o}function e(r){for(var e,n,t,o="",i=0;i<r.length;i++)t=87+(n=15&r[i])+(n-10>>8&-39)<<8|87+(e=r[i]>>>4)+(e-10>>8&-39),o+=String.fromCharCode(255&t)+String.fromCharCode(t>>>8);return o}function n(r,e){function n(r){return r<26?r+65:r<52?r+71:r<62?r-4:62===r?43:63===r?47:65}if(void 0===e&&(e=!0),"string"==typeof r)throw new Error("input has to be an array");for(var t=2,o="",i=r.length,f=0,a=0;a<i;a++)t=a%3,a>0&&4*a/3%76==0&&!e&&(o+="\r\n"),f|=r[a]<<(16>>>t&24),2!==t&&r.length-a!=1||(o+=String.fromCharCode(n(f>>>18&63),n(f>>>12&63),n(f>>>6&63),n(63&f)),f=0);return o.substr(0,o.length-2+t)+(2===t?"":1===t?"=":"==")}return{from_base64:function(r){return"string"==typeof r?function(r,e){for(var n,t,o,i=r.replace(/[^A-Za-z0-9\+\/]/g,""),f=i.length,a=e?Math.ceil((3*f+1>>2)/e)*e:3*f+1>>2,u=new Uint8Array(a),c=0,l=0,s=0;s<f;s++)if(t=3&s,c|=((o=i.charCodeAt(s))>64&&o<91?o-65:o>96&&o<123?o-71:o>47&&o<58?o+4:43===o?62:47===o?63:0)<<18-6*t,3===t||f-s==1){for(n=0;n<3&&l<a;n++,l++)u[l]=c>>>(16>>>n&24)&255;c=0}return u}(r):r},from_base64url:function(r){return"string"==typeof r?this.from_base64(r.replace(/-/g,"+").replace(/_/g,"/")):r},from_hex:function(r){return"string"==typeof r?function(r){if(!function(r){return"string"==typeof r&&/^[0-9a-f]+$/i.test(r)&&r.length%2==0}(r))throw new TypeError("The provided string doesn't look like hex data");for(var e=new Uint8Array(r.length/2),n=0;n<r.length;n+=2)e[n>>>1]=parseInt(r.substr(n,2),16);return e}(r):r},from_string:function(r){return"string"==typeof r?function(r){if("function"==typeof TextEncoder)return new TextEncoder("utf-8").encode(r);r=unescape(encodeURIComponent(r));for(var e=new Uint8Array(r.length),n=0;n<r.length;n++)e[n]=r.charCodeAt(n);return e}(r):r},memcmp:function(r,e){if(!(r instanceof Uint8Array&&e instanceof Uint8Array))throw new TypeError("Only Uint8Array instances can be compared");if(r.length!==e.length)throw new TypeError("Only instances of identical length can be compared");for(var n=0,t=0,o=r.length;t<o;t++)n|=r[t]^e[t];return 0===n},memzero:function(r){r instanceof Uint8Array?function(r){if(!r instanceof Uint8Array)throw new TypeError("Only Uint8Array instances can be wiped");for(var e=0,n=r.length;e<n;e++)r[e]=0}(r):"undefined"!=typeof Buffer&&r instanceof Buffer&&r.fill(0)},to_base64:function(r){return"string"==typeof r?r:n(r).replace(/\s+/g,"")},to_base64url:function(r){return this.to_base64(r).replace(/\+/g,"-").replace(/\//g,"_")},to_hex:function(r){return"string"==typeof r?r:e(r).replace(/\s+/g,"")},to_string:function(e){return"string"==typeof e?e:r(e)}}}(); true&&module.exports?(sodiumUtil.sodiumUtil=sodiumUtil,module.exports=sodiumUtil):self.sodiumUtil=sodiumUtil;

/***/ }),
/* 29 */
/***/ (function(module) {

var __dirname = "/";
var sphincs=function(){var A,I,g,B={};function i(A,I){if(0===A)return I;throw new Error("SPHINCS+ error: "+A)}function C(A,I){return new Uint8Array(new Uint8Array(B.HEAPU8.buffer,A,I))}function n(A){try{B._free(A)}catch(A){setTimeout((function(){throw A}),0)}}B.ready=new Promise((function(A,I){function g(A){return g="function"==typeof Symbol&&"symbol"==typeof Symbol.iterator?function(A){return typeof A}:function(A){return A&&"function"==typeof Symbol&&A.constructor===Symbol&&A!==Symbol.prototype?"symbol":typeof A},g(A)}(r={}).onAbort=I,r.onRuntimeInitialized=function(){try{r._sphincsjs_public_key_bytes(),A(r)}catch(A){I(A)}};var B,i,C,n,Q,E,r=void 0!==r?r:{},e=Object.assign({},r),t=[],o=function(A,I){throw I},a="object"==("undefined"==typeof window?"undefined":g(window)),f="function"==typeof importScripts,c="object"==("undefined"==typeof process?"undefined":g(process))&&"object"==g(process.versions)&&"string"==typeof process.versions.node,s="";c?(s=f?eval("require")("path").dirname(s)+"/":__dirname+"/",E=function(){Q||(n=eval("require")("fs"),Q=eval("require")("path"))},B=function(A,I){var g=X(A);return g?I?g:g.toString():(E(),A=Q.normalize(A),n.readFileSync(A,I?void 0:"utf8"))},C=function(A){var I=B(A,!0);return I.buffer||(I=new Uint8Array(I)),I},i=function(A,I,g){var B=X(A);B&&I(B),E(),A=Q.normalize(A),n.readFile(A,(function(A,B){A?g(A):I(B.buffer)}))},process.argv.length>1&&process.argv[1].replace(/\\/g,"/"),t=process.argv.slice(2), true&&(module.exports=r),process.on("uncaughtException",(function(A){if(!(A instanceof V))throw A})),process.on("unhandledRejection",(function(A){throw A})),o=function(A,I){if(G())throw process.exitCode=A,I;var g;(g=I)instanceof V||h("exiting due to exception: "+g),process.exit(A)},r.inspect=function(){return"[Emscripten Module object]"}):(a||f)&&(f?s=self.location.href:"undefined"!=typeof document&&document.currentScript&&(s=document.currentScript.src),s=0!==s.indexOf("blob:")?s.substr(0,s.replace(/[?#].*/,"").lastIndexOf("/")+1):"",B=function(A){try{var I=new XMLHttpRequest;return I.open("GET",A,!1),I.send(null),I.responseText}catch(I){var g=X(A);if(g)return function(A){for(var I=[],g=0;g<A.length;g++){var B=A[g];B>255&&(B&=255),I.push(String.fromCharCode(B))}return I.join("")}(g);throw I}},f&&(C=function(A){try{var I=new XMLHttpRequest;return I.open("GET",A,!1),I.responseType="arraybuffer",I.send(null),new Uint8Array(I.response)}catch(I){var g=X(A);if(g)return g;throw I}}),i=function(A,I,g){var B=new XMLHttpRequest;B.open("GET",A,!0),B.responseType="arraybuffer",B.onload=function(){if(200==B.status||0==B.status&&B.response)I(B.response);else{var i=X(A);i?I(i.buffer):g()}},B.onerror=g,B.send(null)});r.print||console.log.bind(console);var u,h=r.printErr||console.warn.bind(console);Object.assign(r,e),e=null,r.arguments&&(t=r.arguments),r.thisProgram&&r.thisProgram,r.quit&&(o=r.quit),r.wasmBinary&&(u=r.wasmBinary);var D,y=r.noExitRuntime||!0;"object"!=("undefined"==typeof WebAssembly?"undefined":g(WebAssembly))&&m("no native wasm support detected");var F=!1;var w,k,p,N,d="undefined"!=typeof TextDecoder?new TextDecoder("utf8"):void 0;function l(A,I){return A?function(A,I,g){for(var B=I+g,i=I;A[i]&&!(i>=B);)++i;if(i-I>16&&A.buffer&&d)return d.decode(A.subarray(I,i));for(var C="";I<i;){var n=A[I++];if(128&n){var Q=63&A[I++];if(192!=(224&n)){var E=63&A[I++];if((n=224==(240&n)?(15&n)<<12|Q<<6|E:(7&n)<<18|Q<<12|E<<6|63&A[I++])<65536)C+=String.fromCharCode(n);else{var r=n-65536;C+=String.fromCharCode(55296|r>>10,56320|1023&r)}}else C+=String.fromCharCode((31&n)<<6|Q)}else C+=String.fromCharCode(n)}return C}(k,A,I):""}r.INITIAL_MEMORY;var S=[],U=[],R=[];function G(){return y}var Y=0,M=null,b=null;function m(A){throw r.onAbort&&r.onAbort(A),h(A="Aborted("+A+")"),F=!0,1,A+=". Build with -sASSERTIONS for more info.",new WebAssembly.RuntimeError(A)}var J,H,L="data:application/octet-stream;base64,";function v(A){return A.startsWith(L)}function q(A){return A.startsWith("file://")}function K(A){try{if(A==J&&u)return new Uint8Array(u);var I=X(A);if(I)return I;if(C)return C(A);throw"both async and sync fetching of the wasm failed"}catch(A){m(A)}}v(J="data:application/octet-stream;base64,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GCAEIAIpABA3AxAgBCACKQAANwMAIAQgAikACDcDCCAEQSBqIAMQCyAEQUBrIgNBwAAgBEHAABAHQQAhAgNAIAIgBGoiBUFAayACIANqLQAAIAEgAmotAABzOgAAIAUgAyACQQFyIgVqLQAAIAEgBWotAABzOgBBIAJBAmoiAkHAAEcNAAsgAEEgIARBgAEQByAEQYABaiQACwMAAQsJACAAKAIAEA4L6gECAn8DfiACKAIAIQMCf0EAIAFFDQAaIAGtIQcDQCAFpyICIAMpA8gBIgYgBVgNARogACACaiADIAUgBn1CiAF8IganQXhxaikDACAGQgOGiDwAACAFQgF8IgUgB1INAAsgAQshAiADIAMpA8gBIAKtfTcDyAEgASACayIEBEAgACACaiEBA0AgAxAMQQAhAANAAkAgASAAIgJqIAMgAkF4cWopAwAgAkEDdEE4ca2IPAAAIAJBAWohACACQYYBSw0AIAAgBEkNAQsLIANBhwEgAmutNwPIASAAIAFqIQEgBCAAayIEDQALCwtNAgF+AX8gACgCACIAIAApA8gBIgGnQXhxaiICIAIpAwBCHyABQgOGhoU3AwAgAEIANwPIASAAIAApA4ABQoCAgICAgICAgH+FNwOAAQuABAEDfyACQYAETwRAIAAgASACEAMgAA8LIAAgAmohAwJAIAAgAXNBA3FFBEACQCAAQQNxRQRAIAAhAgwBCyACRQRAIAAhAgwBCyAAIQIDQCACIAEtAAA6AAAgAUEBaiEBIAJBAWoiAkEDcUUNASACIANJDQALCwJAIANBfHEiBEHAAEkNACACIARBQGoiBUsNAANAIAIgASgCADYCACACIAEoAgQ2AgQgAiABKAIINgIIIAIgASgCDDYCDCACIAEoAhA2AhAgAiABKAIUNgIUIAIgASgCGDYCGCACIAEoAhw2AhwgAiABKAIgNgIgIAIgASgCJDYCJCACIAEoAig2AiggAiABKAIsNgIsIAIgASgCMDYCMCACIAEoAjQ2AjQgAiABKAI4NgI4IAIgASgCPDYCPCABQUBrIQEgAkFAayICIAVNDQALCyACIARPDQEDQCACIAEoAgA2AgAgAUEEaiEBIAJBBGoiAiAESQ0ACwwBCyADQQRJBEAgACECDAELIAAgA0EEayIESwRAIAAhAgwBCyAAIQIDQCACIAEtAAA6AAAgAiABLQABOgABIAIgAS0AAjoAAiACIAEtAAM6AAMgAUEEaiEBIAJBBGoiAiAETQ0ACwsgAiADSQRAA0AgAiABLQAAOgAAIAFBAWohASACQQFqIgIgA0cNAAsLIAALIgAgAEHQARAPIgA2AgAgAEUEQEHvABAAAAsgAEHQARAQGgszAQF/IwBB0AJrIgckACAAIAEgB0EwaiAHIAIgAyAEQQBBCEECIAUgBhAZIAdB0AJqJAAL6wIBCH8gBkEBcyESA0AgAiAMQQV0aiINIAQgBSAHIA9qIAogCyAJEQUAIAMgDEECdGpBADYCACAMQQFqIRAgDyASRgRAIAEgDSkAADcAACABIA0pABg3ABggASANKQAQNwAQIAEgDSkACDcACAsCQCAQIgxBAkkNAANAIAMgECIMQQFrIhBBAnRqIhMoAgAiDiADIAxBAmsiDUECdGoiESgCAEcNASAKIA5BAWoiDDYCGCAKIA8gDHYiDCAHIBMoAgBBAWp2ajYCHCACIA1BBXRqIg4gDiAFIAoQESARIBEoAgBBAWoiDTYCACAMIAYgDXZBAXNGBEAgASANQQV0aiIMIA4pAAA3AAAgDCAOKQAYNwAYIAwgDikAEDcAECAMIA4pAAg3AAgLQQEhDCAQQQFLDQALCyAPQQFqIg8gCHZFDQALIAAgAikAADcAACAAIAIpABg3ABggACACKQAQNwAQIAAgAikACDcACAvYAgEEfyMAQUBqIggkAAJ/IAJBAXEEQCAIIAEpABg3AzggCCABKQAQNwMwIAggASkACDcDKCAIIAEpAAA3AyAgCAwBCyAIIAEpABg3AxggCCABKQAQNwMQIAggASkAADcDACAIIAEpAAg3AwggCEEgagsiASAEKQAANwAAIAEgBCkAGDcAGCABIAQpABA3ABAgASAEKQAINwAIAkAgBUEBayILRQRAIAIhCQwBCyAIQSBqIQpBACEBA0AgByABQQFqIgE2AhggByACQQF2IgkgA0EBdiIDajYCHCAKIAggAkECcSICGyAIIAYgBxARIAggCiACGyICIAQpADg3ABggAiAEKQAwNwAQIAIgBCkAKDcACCACIAQpACA3AAAgBEEgaiEEIAkhAiABIAtHDQALCyAHIAU2AhggByAJQQF2IANBAXZqNgIcIAAgCCAGIAcQESAIQUBrJAALvgECBX4DfyABRQRAQgAPCyABrSICQgGDIQUCfyABQQFGBEBCACECQX8hB0EADAELIAJC/v///w+DIQZCACECA0AgACADpyIJQQFyajEAACABIAgiB2tBA3RBEGuthiAAIAlqMQAAIAdBf3MgAWpBA3SthiAChIQhAiAHQQJqIQggA0ICfCEDIARCAnwiBCAGUg0AC0F9IAdrIQcgA6cLIQggBVAEfiACBSAAIAhqMQAAIAEgB2pBA3SthiAChAsLsAEBAn8jAEGABmsiBCQAIAQgAikAGDcDGCAEIAIpABA3AxAgBCACKQAANwMAIAQgAikACDcDCCAEQSBqIAMQCyAEQUBrIgNBwAUgBEHAABAHQQAhAgNAIAIgBGoiBUFAayACIANqLQAAIAEgAmotAABzOgAAIAUgAyACQQFyIgVqLQAAIAEgBWotAABzOgBBIAJBAmoiAkHABUcNAAsgAEEgIARBgAYQByAEQYAGaiQAC7ABAQJ/IwBBoBFrIgQkACAEIAIpABg3AxggBCACKQAQNwMQIAQgAikAADcDACAEIAIpAAg3AwggBEEgaiADEAsgBEFAayIDQeAQIARBwAAQB0EAIQIDQCACIARqIgVBQGsgAiADai0AACABIAJqLQAAczoAACAFIAMgAkEBciIFai0AACABIAVqLQAAczoAQSACQQJqIgJB4BBHDQALIABBICAEQaAREAcgBEGgEWokAAunAQECfyMAQUBqIgckACAHQQhqIggQFyAIIANBIBAKIAggBEHAABAKIAggBSAGEAogCBAVIAdBEGpBLyAIEBQgCBATIAAgBykALzcAHyAAIAcpAyg3ABggACAHKQMgNwAQIAAgBykDGDcACCAAIAcpAxA3AAAgASAHQTdqQQcQG0L//////////wCDNwMAIAIgB0E+akEBEBunQf8BcTYCACAHQUBrJAALrQQBBn8DQCAAIAZBAnRqIgVBADYCACAFIAEgBEEDdmoiAi0AACAEQQZxIgN2QQFxIgc2AgAgBSACLQAAIANBAXJ2QQF0QQJxIAdyIgI2AgAgBSACIAEgBEECaiICQQN2ai0AACACQQZxdkECdEEEcXIiAjYCACAFIAIgASAEQQNqIgJBA3ZqLQAAIAJBB3F2QQN0QQhxciICNgIAIAUgAiABIARBBGoiAkEDdmotAAAgAkEGcXZBBHRBEHFy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JAAgBEIANwNIIARBQGtCADcDACAEQgA3AzggBEIANwMwIARCADcDKCAEQgA3AyAgBEIANwMYIARCADcDECAEQTBqIgZBADYCECAEQQI2AiAgBEGwAWoiB0IgECAjAEEQayIJJAAgCUEIaiIFEBcgBSADQSBqQSAQCiAFIAdBIBAKIAUgASACEAogBRAVIABBAmoiB0EgIAUQFCAFEBMgCUEQaiQAIARBgAFqIgkgBEHYAGogBEHUAGogByADQUBrIg0gASACEB4gBiAEKQNYIhE+AgwgBkEANgIEIAYgEUIgiD4CCCAGIAQoAlQ2AhQgB0EgaiEFIARB4ABqIQggBEEMaiELIwBB4AZrIgEkACABQgA3AzggAUIANwMwIAFCADcDKCABQgA3AyAgAUIANwMYIAFCADcDECABQgA3AwggAUIANwMAIAFBIGogBhAJIAEgBhAJIAFBAzYCMCABQQQ2AhAgAUGABmogCRAfQQAhCQNAIAFBIGoiAkEANgIYIAIgAUGABmogCUECdGooAgAiCiAJQQ50IgxqNgIcIAUgAyACEA0jAEGgBGsiBiQAIAFBQGsgCUEFdGogBUEgaiAGQUBrIAYgAyANIAogDEEOQQEgAiALEBkgBkGgBGokACAFQeADaiEFIAlBAWoiCUEWRw0ACyAIIAFBQGsgDSABEBwgAUHgBmokACAHQeDSAGohAUEAIQkDQCAEQRBqIgsgCTYCACALIAQpA1giET4CDCALQQA2AgQgCyARQiCIPgIIIARBMGoiByALKAIANgIAIAcgCygCBDYCBCAHIAsoAgg2AgggByALKAIMNgIMIAcgBCgCVDYCFCABIQIgBEHgAGohDCAEQQxqIQ9BACEFQQAhAUEAIQhBACEKIwBBkAJrIgYkAANAIAVFBEAgASAMai0AACEKQQghBSABQQFqIQELIAYgCEECdGogCkH/AXEgBUEEayIFdkEPcTYCACAFRQRAIAEgDGotAAAhCkEIIQUgAUEBaiEBCyAGIAhBAXJBAnRqIApB/wFxIAVBBGsiBXZBD3E2AgAgCEECaiIIQcAARw0AC0EAIQVBACEKA0AgCiAGIAVBAnQiAWooAgAgBiABQQRyaigCAGogBiABQQhyaigCAGogBiABQQxyaigCAGprQTxqIQogBUEEaiIFQcAARw0ACyAGQY4CakECIApBBHStEAYgBiAGLQCOAiIBQQ9xNgKEAiAGIAFBBHY2AoACIAYgBi0AjwJBBHY2AogCQQAhAQNAIAcgATYCGCAHQQA2AhwgAiABQQV0aiIFIAMgBxANIAYgAUECdGooAgAiCARAIAhBAWsiCEEPIAhBD0kbIQpBACEIA0AgByAINgIcIAUgBSANIAcQBSAIIApHIRAgCEEBaiEIIBANAAsLIAFBAWoiAUHDAEcNAAsgBkGQAmokACAMIAJB4BBqIAMgDSAEKAJUIAsgDxAYIAQgBCkDWCIRQgiINwNYIAQgEadB/wFxNgJUIAJB4BJqIQEgCUEBaiIJQQhHDQALIA5B4OgBNgIMIARB0AFqJAAgACAOKAIMOwEAIA5BEGokAEEAC8kBAQJ/IwBBkANrIgIkACACQuAAECAgAkIANwOIASACQgA3A4ABIAJCADcDeCACQgA3A3AgAkHwAGoiA0EHNgIAIANBAjYCECAAIAEgAkHgABAWIgEpAFg3ABggACABKQBQNwAQIAAgASkASDcACCAAIAEpAEA3AAAgAUHgAGogAkGQAWogASABQUBrQQAgAyACQewAahAYIAAgASkAeDcAOCAAIAEpAHA3ADAgACABKQBoNwAoIAAgASkAYDcAICACQZADaiQAQQALBgBB4ugBCwUAQYABCwUAQcAACygBAX8jAEEQayIAJAAgAEEAOgAPQcgKIABBD2pBABABGiAAQRBqJAAL2AMBAn8jAEGgEWsiBiQAIAZCADcDOCAGQgA3AzAgBkIANwMoIAZCADcDICAGQgA3AxggBkIANwMQIAZCADcDCCAGQgA3AwAgBkEgaiIFQQA2AhAgBkEBNgIQIAUgBCgCADYCACAFIAQoAgQ2AgQgBSAEKAIINgIIIAUgBCgCDDYCDCAFIAM2AhQgBkFAayEHQQAhBANAIAUgBDYCGCAFQQA2AhwgByAEQQV0aiIDIAEgBRANIAVBADYCHCADIAMgAiAFEAUgBUEBNgIcIAMgAyACIAUQBSAFQQI2AhwgAyADIAIgBRAFIAVBAzYCHCADIAMgAiAFEAUgBUEENgIcIAMgAyACIAUQBSAFQQU2AhwgAyADIAIgBRAFIAVBBjYCHCADIAMgAiAFEAUgBUEHNgIcIAMgAyACIAUQBSAFQQg2AhwgAyADIAIgBRAFIAVBCTYCHCADIAMgAiAFEAUgBUEKNgIcIAMgAyACIAUQBSAFQQs2AhwgAyADIAIgBRAFIAVBDDYCHCADIAMgAiAFEAUgBUENNgIcIAMgAyACIAUQBSAFQQ42AhwgAyADIAIgBRAFIARBAWoiBEHDAEcNAAsgBiAFEAkgACAHIAIgBhAdIAZBoBFqJAALVAAjAEEgayIFJAAgBUIANwMYIAVCADcDECAFQgA3AwggBUIANwMAIAUgBBAJIAVBAzYCECAFIAM2AhwgACABIAUQDSAAIAAgAiAFEAUgBUEgaiQACwuoAgMAQYAIC1FyYW5kb21ieXRlcwBsaWJzb2RpdW0vc3JjL2xpYnNvZGl1bS9yYW5kb21ieXRlcy9yYW5kb21ieXRlcy5jAGJ1Zl9sZW4gPD0gU0laRV9NQVgAQeAIC8ABAQAAAAAAAACCgAAAAAAAAIqAAAAAAACAAIAAgAAAAICLgAAAAAAAAAEAAIAAAAAAgYAAgAAAAIAJgAAAAAAAgIoAAAAAAAAAiAAAAAAAAAAJgACAAAAAAAoAAIAAAAAAi4AAgAAAAACLAAAAAAAAgImAAAAAAACAA4AAAAAAAIACgAAAAAAAgIAAAAAAAACACoAAAAAAAAAKAACAAAAAgIGAAIAAAACAgIAAAAAAAIABAACAAAAAAAiAAIAAAACAAEGgCgsDEAqA")||(H=J,J=r.locateFile?r.locateFile(H,s):s+H);var x={1316:function(){return r.getRandomValue()},1352:function(){if(void 0===r.getRandomValue)try{var A="object"===("undefined"==typeof window?"undefined":g(window))?window:self,I=void 0!==A.crypto?A.crypto:A.msCrypto,B=function(){var A=new Uint32Array(1);return I.getRandomValues(A),A[0]>>>0};B(),r.getRandomValue=B}catch(A){try{var i=eval("require")("crypto"),C=function(){var A=i.randomBytes(4);return(A[0]<<24|A[1]<<16|A[2]<<8|A[3])>>>0};C(),r.getRandomValue=C}catch(A){throw"No secure random number generator found"}}}};function _(A){for(;A.length>0;)A.shift()(r)}var j=[];var W="function"==typeof atob?atob:function(A){var I,g,B,i,C,n,Q="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=",E="",r=0;A=A.replace(/[^A-Za-z0-9\+\/\=]/g,"");do{I=Q.indexOf(A.charAt(r++))<<2|(i=Q.indexOf(A.charAt(r++)))>>4,g=(15&i)<<4|(C=Q.indexOf(A.charAt(r++)))>>2,B=(3&C)<<6|(n=Q.indexOf(A.charAt(r++))),E+=String.fromCharCode(I),64!==C&&(E+=String.fromCharCode(g)),64!==n&&(E+=String.fromCharCode(B))}while(r<A.length);return E};function X(A){if(v(A))return function(A){if("boolean"==typeof c&&c){var I=Buffer.from(A,"base64");return new Uint8Array(I.buffer,I.byteOffset,I.byteLength)}try{for(var g=W(A),B=new Uint8Array(g.length),i=0;i<g.length;++i)B[i]=g.charCodeAt(i);return B}catch(A){throw new Error("Converting base64 string to bytes failed.")}}(A.slice(L.length))}var T,O={e:function(A,I,g,B){m("Assertion failed: "+l(A)+", at: "+[I?l(I):"unknown filename",g,B?l(B):"unknown function"])},b:function(A,I,g){var B=function(A,I){var g;for(j.length=0,I>>=2;g=k[A++];)I+=105!=g&I,j.push(105==g?p[I]:N[I++>>1]),++I;return j}(I,g);return x[A].apply(null,B)},d:function(A,I,g){k.copyWithin(A,I,I+g)},c:function(A){k.length,m("OOM")},a:function(A){!function(A,I){A,function(A){A,G()||(r.onExit&&r.onExit(A),F=!0);o(A,new V(A))}(A)}(A)}};(function(){var A={a:O};function I(A,I){var g,B,i=A.exports;r.asm=i,D=r.asm.f,g=D.buffer,g,r.HEAP8=w=new Int8Array(g),r.HEAP16=new Int16Array(g),r.HEAP32=p=new Int32Array(g),r.HEAPU8=k=new Uint8Array(g),r.HEAPU16=new Uint16Array(g),r.HEAPU32=new Uint32Array(g),r.HEAPF32=new Float32Array(g),r.HEAPF64=N=new Float64Array(g),r.asm.h,B=r.asm.g,U.unshift(B),function(A){if(Y--,r.monitorRunDependencies&&r.monitorRunDependencies(Y),0==Y&&(null!==M&&(clearInterval(M),M=null),b)){var I=b;b=null,I()}}()}function g(A){I(A.instance)}function B(I){return function(){if(!u&&(a||f)){if("function"==typeof fetch&&!q(J))return fetch(J,{credentials:"same-origin"}).then((function(A){if(!A.ok)throw"failed to load wasm binary file at '"+J+"'";return A.arrayBuffer()})).catch((function(){return K(J)}));if(i)return new Promise((function(A,I){i(J,(function(I){A(new Uint8Array(I))}),I)}))}return Promise.resolve().then((function(){return K(J)}))}().then((function(I){return WebAssembly.instantiate(I,A)})).then((function(A){return A})).then(I,(function(A){h("failed to asynchronously prepare wasm: "+A),m(A)}))}if(Y++,r.monitorRunDependencies&&r.monitorRunDependencies(Y),r.instantiateWasm)try{return r.instantiateWasm(A,I)}catch(A){return h("Module.instantiateWasm callback failed with error: "+A),!1}u||"function"!=typeof WebAssembly.instantiateStreaming||v(J)||q(J)||c||"function"!=typeof fetch?B(g):fetch(J,{credentials:"same-origin"}).then((function(I){return WebAssembly.instantiateStreaming(I,A).then(g,(function(A){return h("wasm streaming compile failed: "+A),h("falling back to ArrayBuffer instantiation"),B(g)}))}))})(),r.___wasm_call_ctors=function(){return(r.___wasm_call_ctors=r.asm.g).apply(null,arguments)},r._malloc=function(){return(r._malloc=r.asm.i).apply(null,arguments)},r._free=function(){return(r._free=r.asm.j).apply(null,arguments)},r._sphincsjs_init=function(){return(r._sphincsjs_init=r.asm.k).apply(null,arguments)},r._sphincsjs_public_key_bytes=function(){return(r._sphincsjs_public_key_bytes=r.asm.l).apply(null,arguments)},r._sphincsjs_secret_key_bytes=function(){return(r._sphincsjs_secret_key_bytes=r.asm.m).apply(null,arguments)},r._sphincsjs_signature_bytes=function(){return(r._sphincsjs_signature_bytes=r.asm.n).apply(null,arguments)},r._sphincsjs_keypair=function(){return(r._sphincsjs_keypair=r.asm.o).apply(null,arguments)},r._sphincsjs_sign=function(){return(r._sphincsjs_sign=r.asm.p).apply(null,arguments)},r._sphincsjs_verify=function(){return(r._sphincsjs_verify=r.asm.q).apply(null,arguments)};function V(A){this.name="ExitStatus",this.message="Program terminated with exit("+A+")",this.status=A}function Z(A){function I(){T||(T=!0,r.calledRun=!0,F||(!0,_(U),r.onRuntimeInitialized&&r.onRuntimeInitialized(),function(){if(r.postRun)for("function"==typeof r.postRun&&(r.postRun=[r.postRun]);r.postRun.length;)A=r.postRun.shift(),R.unshift(A);var A;_(R)}()))}A=A||t,Y>0||(!function(){if(r.preRun)for("function"==typeof r.preRun&&(r.preRun=[r.preRun]);r.preRun.length;)A=r.preRun.shift(),S.unshift(A);var A;_(S)}(),Y>0||(r.setStatus?(r.setStatus("Running..."),setTimeout((function(){setTimeout((function(){r.setStatus("")}),1),I()}),1)):I()))}if(r.writeArrayToMemory=function(A,I){w.set(A,I)},b=function A(){T||Z(),T||(b=A)},r.run=Z,r.preInit)for("function"==typeof r.preInit&&(r.preInit=[r.preInit]);r.preInit.length>0;)r.preInit.pop()();Z()})).catch((function(){function A(I){return A="function"==typeof Symbol&&"symbol"==typeof Symbol.iterator?function(A){return typeof A}:function(A){return A&&"function"==typeof Symbol&&A.constructor===Symbol&&A!==Symbol.prototype?"symbol":typeof A},A(I)}var I,g,B,i,C,n,Q=void 0!==(Q={})?Q:{},E=Object.assign({},Q),r=[],e=function(A,I){throw I},t="object"==("undefined"==typeof window?"undefined":A(window)),o="function"==typeof importScripts,a="object"==("undefined"==typeof process?"undefined":A(process))&&"object"==A(process.versions)&&"string"==typeof process.versions.node,f="";a?(f=o?eval("require")("path").dirname(f)+"/":__dirname+"/",n=function(){C||(i=eval("require")("fs"),C=eval("require")("path"))},I=function(A,I){var g=O(A);return g?I?g:g.toString():(n(),A=C.normalize(A),i.readFileSync(A,I?void 0:"utf8"))},B=function(A){var g=I(A,!0);return g.buffer||(g=new Uint8Array(g)),g},g=function(A,I,g){var B=O(A);B&&I(B),n(),A=C.normalize(A),i.readFile(A,(function(A,B){A?g(A):I(B.buffer)}))},process.argv.length>1&&process.argv[1].replace(/\\/g,"/"),r=process.argv.slice(2), true&&(module.exports=Q),process.on("uncaughtException",(function(A){if(!(A instanceof P))throw A})),process.on("unhandledRejection",(function(A){throw A})),e=function(A,I){if(M())throw process.exitCode=A,I;var g;(g=I)instanceof P||s("exiting due to exception: "+g),process.exit(A)},Q.inspect=function(){return"[Emscripten Module object]"}):(t||o)&&(o?f=self.location.href:"undefined"!=typeof document&&document.currentScript&&(f=document.currentScript.src),f=0!==f.indexOf("blob:")?f.substr(0,f.replace(/[?#].*/,"").lastIndexOf("/")+1):"",I=function(A){try{var I=new XMLHttpRequest;return I.open("GET",A,!1),I.send(null),I.responseText}catch(I){var g=O(A);if(g)return function(A){for(var I=[],g=0;g<A.length;g++){var B=A[g];B>255&&(B&=255),I.push(String.fromCharCode(B))}return I.join("")}(g);throw I}},o&&(B=function(A){try{var I=new XMLHttpRequest;return I.open("GET",A,!1),I.responseType="arraybuffer",I.send(null),new Uint8Array(I.response)}catch(I){var g=O(A);if(g)return g;throw I}}),g=function(A,I,g){var B=new XMLHttpRequest;B.open("GET",A,!0),B.responseType="arraybuffer",B.onload=function(){if(200==B.status||0==B.status&&B.response)I(B.response);else{var i=O(A);i?I(i.buffer):g()}},B.onerror=g,B.send(null)});Q.print||console.log.bind(console);var c,s=Q.printErr||console.warn.bind(console);Object.assign(Q,E),E=null,Q.arguments&&(r=Q.arguments),Q.thisProgram&&Q.thisProgram,Q.quit&&(e=Q.quit),Q.wasmBinary&&(c=Q.wasmBinary);var u,h=Q.noExitRuntime||!0,D={Memory:function(A){this.buffer=new ArrayBuffer(65536*A.initial)},Module:function(A){},Instance:function(A,I){this.exports=function(A){for(var I,g=new Uint8Array(123),B=25;B>=0;--B)g[48+B]=52+B,g[65+B]=B,g[97+B]=26+B;function i(A,I,B){for(var i,C,n=0,Q=I,E=B.length,r=I+(3*E>>2)-("="==B[E-2])-("="==B[E-1]);n<E;n+=4)i=g[B.charCodeAt(n+1)],C=g[B.charCodeAt(n+2)],A[Q++]=g[B.charCodeAt(n)]<<2|i>>4,Q<r&&(A[Q++]=i<<4|C>>2),Q<r&&(A[Q++]=C<<6|g[B.charCodeAt(n+3)])}return g[43]=62,g[47]=63,function(A){var g=A.a.buffer,B=new Int8Array(g),C=new Int16Array(g),n=new Int32Array(g),Q=new Uint8Array(g),E=new Uint16Array(g),r=new Uint32Array(g),e=(new Float32Array(g),new Float64Array(g),Math.imul),t=(Math.fround,Math.abs,Math.clz32,Math.min,Math.max,Math.floor,Math.ceil,Math.trunc,Math.sqrt,A.abort),o=A.b,a=A.c,f=A.d,c=8391184,s=0;function u(A){var I,g=0,B=0,i=0,C=0,E=0,e=0,t=0,o=0,a=0,f=0;c=I=c-16|0;A:{I:{g:{B:{i:{C:{n:{Q:{E:{r:{e:{if((A|=0)>>>0<=244){if(3&(g=(C=n[520])>>>(B=(t=A>>>0<11?16:A+11&-8)>>>3|0)|0)){g=(A=(B=B+(1&(-1^g))|0)<<3)+2120|0,i=n[A+2128>>2],(0|g)!=(0|(A=n[i+8>>2]))?(n[A+12>>2]=g,n[g+8>>2]=A):n[520]=X(B)&C,A=i+8|0,g=B<<3,n[i+4>>2]=3|g,n[(g=g+i|0)+4>>2]=1|n[g+4>>2];break A}if((f=n[522])>>>0>=t>>>0)break e;if(g){B=A=(g=(0-(A=(0-(A=2<<B)|A)&g<<B)&A)-1|0)>>>12&16,B|=A=(g=g>>>A|0)>>>5&8,B|=A=(g=g>>>A|0)>>>2&4,g=(A=(i=((B|=A=(g=g>>>A|0)>>>1&2)|(A=(g=g>>>A|0)>>>1&1))+(g>>>A|0)|0)<<3)+2120|0,e=n[A+2128>>2],(0|g)!=(0|(A=n[e+8>>2]))?(n[A+12>>2]=g,n[g+8>>2]=A):(C=X(i)&C,n[520]=C),n[e+4>>2]=3|t,i=(A=i<<3)-t|0,n[(B=e+t|0)+4>>2]=1|i,n[A+e>>2]=i,f&&(g=2120+(-8&f)|0,E=n[525],(A=1<<(f>>>3))&C?A=n[g+8>>2]:(n[520]=A|C,A=g),n[g+8>>2]=E,n[A+12>>2]=E,n[E+12>>2]=g,n[E+8>>2]=A),A=e+8|0,n[525]=B,n[522]=i;break A}if(!(a=n[521]))break e;for(B=A=(g=(0-a&a)-1|0)>>>12&16,B|=A=(g=g>>>A|0)>>>5&8,B|=A=(g=g>>>A|0)>>>2&4,B=n[2384+(((B|=A=(g=g>>>A|0)>>>1&2)|(A=(g=g>>>A|0)>>>1&1))+(g>>>A|0)<<2)>>2],E=(-8&n[B+4>>2])-t|0,g=B;(A=n[g+16>>2])||(A=n[g+20>>2]);)E=(i=(g=(-8&n[A+4>>2])-t|0)>>>0<E>>>0)?g:E,B=i?A:B,g=A;if(o=n[B+24>>2],(0|(i=n[B+12>>2]))!=(0|B)){A=n[B+8>>2],n[A+12>>2]=i,n[i+8>>2]=A;break I}if(!(A=n[(g=B+20|0)>>2])){if(!(A=n[B+16>>2]))break r;g=B+16|0}for(;e=g,i=A,(A=n[(g=A+20|0)>>2])||(g=i+16|0,A=n[i+16>>2]););n[e>>2]=0;break I}if(t=-1,!(A>>>0>4294967231)&&(t=-8&(A=A+11|0),a=n[521])){E=0-t|0,C=0,t>>>0<256||(C=31,t>>>0>16777215||(A=A>>>8|0,A<<=e=A+1048320>>>16&8,C=28+((A=((A<<=B=A+520192>>>16&4)<<(g=A+245760>>>16&2)>>>15|0)-(g|B|e)|0)<<1|t>>>A+21&1)|0));t:{o:{if(g=n[2384+(C<<2)>>2])for(A=0,B=t<<(31==(0|C)?0:25-(C>>>1|0)|0);;){if(!((e=(-8&n[g+4>>2])-t|0)>>>0>=E>>>0||(i=g,E=e))){E=0,A=g;break o}if(e=n[g+20>>2],g=n[16+((B>>>29&4)+g|0)>>2],A=e?(0|e)==(0|g)?A:e:A,B<<=1,!g)break}else A=0;if(!(A|i)){if(i=0,!(A=(0-(A=2<<C)|A)&a))break e;B=A=(g=(A&0-A)-1|0)>>>12&16,B|=A=(g=g>>>A|0)>>>5&8,B|=A=(g=g>>>A|0)>>>2&4,A=n[2384+(((B|=A=(g=g>>>A|0)>>>1&2)|(A=(g=g>>>A|0)>>>1&1))+(g>>>A|0)<<2)>>2]}if(!A)break t}for(;E=(B=(g=(-8&n[A+4>>2])-t|0)>>>0<E>>>0)?g:E,i=B?A:i,A=(g=n[A+16>>2])||n[A+20>>2];);}if(!(!i|n[522]-t>>>0<=E>>>0)){if(C=n[i+24>>2],(0|i)!=(0|(B=n[i+12>>2]))){A=n[i+8>>2],n[A+12>>2]=B,n[B+8>>2]=A;break g}if(!(A=n[(g=i+20|0)>>2])){if(!(A=n[i+16>>2]))break E;g=i+16|0}for(;e=g,B=A,(A=n[(g=A+20|0)>>2])||(g=B+16|0,A=n[B+16>>2]););n[e>>2]=0;break g}}}if((B=n[522])>>>0>=t>>>0){i=n[525],(g=B-t|0)>>>0>=16?(n[522]=g,A=i+t|0,n[525]=A,n[A+4>>2]=1|g,n[B+i>>2]=g,n[i+4>>2]=3|t):(n[525]=0,n[522]=0,n[i+4>>2]=3|B,n[(A=B+i|0)+4>>2]=1|n[A+4>>2]),A=i+8|0;break A}if((o=n[523])>>>0>t>>>0){g=o-t|0,n[523]=g,A=(B=n[526])+t|0,n[526]=A,n[A+4>>2]=1|g,n[B+4>>2]=3|t,A=B+8|0;break A}if(A=0,a=t+47|0,n[638]?B=n[640]:(n[641]=-1,n[642]=-1,n[639]=4096,n[640]=4096,n[638]=I+12&-16^1431655768,n[643]=0,n[631]=0,B=4096),(g=(e=a+B|0)&(E=0-B|0))>>>0<=t>>>0)break A;if((i=n[630])&&i>>>0<(C=(B=n[628])+g|0)>>>0|B>>>0>=C>>>0)break A;if(4&Q[2524])break C;e:{t:{if(i=n[526])for(A=2528;;){if((B=n[A>>2])>>>0<=i>>>0&i>>>0<B+n[A+4>>2]>>>0)break t;if(!(A=n[A+8>>2]))break}if(-1==(0|(B=H(0))))break n;if(C=g,(A=(i=n[639])-1|0)&B&&(C=(g-B|0)+(A+B&0-i)|0),C>>>0<=t>>>0|C>>>0>2147483646)break n;if((i=n[630])&&i>>>0<(E=(A=n[628])+C|0)>>>0|A>>>0>=E>>>0)break n;if((0|B)!=(0|(A=H(C))))break e;break i}if((C=E&e-o)>>>0>2147483646)break n;if((0|(B=H(C)))==(n[A>>2]+n[A+4>>2]|0))break Q;A=B}if(!(-1==(0|A)|t+48>>>0<=C>>>0)){if((B=(B=n[640])+(a-C|0)&0-B)>>>0>2147483646){B=A;break i}if(-1!=(0|H(B))){C=B+C|0,B=A;break i}H(0-C|0);break n}if(B=A,-1!=(0|A))break i;break n}i=0;break I}B=0;break g}if(-1!=(0|B))break i}n[631]=4|n[631]}if(g>>>0>2147483646)break B;if(-1==(0|(B=H(g)))|-1==(0|(A=H(0)))|A>>>0<=B>>>0)break B;if((C=A-B|0)>>>0<=t+40>>>0)break B}A=n[628]+C|0,n[628]=A,A>>>0>r[629]&&(n[629]=A);i:{C:{n:{if(e=n[526]){for(A=2528;;){if(((i=n[A>>2])+(g=n[A+4>>2])|0)==(0|B))break n;if(!(A=n[A+8>>2]))break}break C}for((A=n[524])>>>0<=B>>>0&&A||(n[524]=B),A=0,n[633]=C,n[632]=B,n[528]=-1,n[529]=n[638],n[635]=0;g=(i=A<<3)+2120|0,n[i+2128>>2]=g,n[i+2132>>2]=g,32!=(0|(A=A+1|0)););g=(i=C-40|0)-(A=B+8&7?-8-B&7:0)|0,n[523]=g,A=A+B|0,n[526]=A,n[A+4>>2]=1|g,n[4+(B+i|0)>>2]=40,n[527]=n[642];break i}if(!(8&Q[A+12|0]|i>>>0>e>>>0|B>>>0<=e>>>0)){n[A+4>>2]=g+C,B=(A=e+8&7?-8-e&7:0)+e|0,n[526]=B,A=(g=n[523]+C|0)-A|0,n[523]=A,n[B+4>>2]=1|A,n[4+(g+e|0)>>2]=40,n[527]=n[642];break i}}r[524]>B>>>0&&(n[524]=B),g=B+C|0,A=2528;C:{n:{Q:{E:{r:{e:{for(;;){if((0|g)!=n[A>>2]){if(A=n[A+8>>2])continue;break e}break}if(!(8&Q[A+12|0]))break r}for(A=2528;;){if((g=n[A>>2])>>>0<=e>>>0&&(E=g+n[A+4>>2]|0)>>>0>e>>>0)break E;A=n[A+8>>2]}}if(n[A>>2]=B,n[A+4>>2]=n[A+4>>2]+C,n[(a=(B+8&7?-8-B&7:0)+B|0)+4>>2]=3|t,A=(C=g+(g+8&7?-8-g&7:0)|0)-(o=t+a|0)|0,(0|C)==(0|e)){n[526]=o,A=n[523]+A|0,n[523]=A,n[o+4>>2]=1|A;break n}if(n[525]==(0|C)){n[525]=o,A=n[522]+A|0,n[522]=A,n[o+4>>2]=1|A,n[A+o>>2]=A;break n}if(1==(3&(E=n[C+4>>2]))){e=-8&E;r:if(E>>>0<=255){if(i=n[C+8>>2],g=E>>>3|0,(0|(B=n[C+12>>2]))==(0|i)){n[520]=n[520]&X(g);break r}n[i+12>>2]=B,n[B+8>>2]=i}else{if(t=n[C+24>>2],(0|C)==(0|(B=n[C+12>>2])))if((g=n[(E=C+20|0)>>2])||(g=n[(E=C+16|0)>>2])){for(;i=E,B=g,(g=n[(E=g+20|0)>>2])||(E=B+16|0,g=n[B+16>>2]););n[i>>2]=0}else B=0;else g=n[C+8>>2],n[g+12>>2]=B,n[B+8>>2]=g;if(t){i=n[C+28>>2];e:{if(n[(g=2384+(i<<2)|0)>>2]==(0|C)){if(n[g>>2]=B,B)break e;n[521]=n[521]&X(i);break r}if(n[t+(n[t+16>>2]==(0|C)?16:20)>>2]=B,!B)break r}n[B+24>>2]=t,(g=n[C+16>>2])&&(n[B+16>>2]=g,n[g+24>>2]=B),(g=n[C+20>>2])&&(n[B+20>>2]=g,n[g+24>>2]=B)}}E=n[(C=C+e|0)+4>>2],A=A+e|0}if(n[C+4>>2]=-2&E,n[o+4>>2]=1|A,n[A+o>>2]=A,A>>>0<=255){g=2120+(-8&A)|0,(B=n[520])&(A=1<<(A>>>3))?A=n[g+8>>2]:(n[520]=A|B,A=g),n[g+8>>2]=o,n[A+12>>2]=o,n[o+12>>2]=g,n[o+8>>2]=A;break n}if(E=31,A>>>0<=16777215&&(g=A>>>8|0,g<<=E=g+1048320>>>16&8,E=28+((g=((g<<=i=g+520192>>>16&4)<<(B=g+245760>>>16&2)>>>15|0)-(B|i|E)|0)<<1|A>>>g+21&1)|0),n[o+28>>2]=E,n[o+16>>2]=0,n[o+20>>2]=0,g=2384+(E<<2)|0,(i=n[521])&(B=1<<E)){for(E=A<<(31==(0|E)?0:25-(E>>>1|0)|0),B=n[g>>2];;){if((-8&n[(g=B)+4>>2])==(0|A))break Q;if(B=E>>>29|0,E<<=1,!(B=n[(i=(4&B)+g|0)+16>>2]))break}n[i+16>>2]=o}else n[521]=B|i,n[g>>2]=o;n[o+24>>2]=g,n[o+12>>2]=o,n[o+8>>2]=o;break n}for(g=(i=C-40|0)-(A=B+8&7?-8-B&7:0)|0,n[523]=g,A=A+B|0,n[526]=A,n[A+4>>2]=1|g,n[4+(B+i|0)>>2]=40,n[527]=n[642],n[(i=(A=(E+(E-39&7?39-E&7:0)|0)-47|0)>>>0<e+16>>>0?e:A)+4>>2]=27,A=n[635],n[i+16>>2]=n[634],n[i+20>>2]=A,A=n[633],n[i+8>>2]=n[632],n[i+12>>2]=A,n[634]=i+8,n[633]=C,n[632]=B,n[635]=0,A=i+24|0;n[A+4>>2]=7,g=A+8|0,A=A+4|0,g>>>0<E>>>0;);if((0|i)==(0|e))break i;if(n[i+4>>2]=-2&n[i+4>>2],E=i-e|0,n[e+4>>2]=1|E,n[i>>2]=E,E>>>0<=255){g=2120+(-8&E)|0,(B=n[520])&(A=1<<(E>>>3))?A=n[g+8>>2]:(n[520]=A|B,A=g),n[g+8>>2]=e,n[A+12>>2]=e,n[e+12>>2]=g,n[e+8>>2]=A;break i}if(A=31,E>>>0<=16777215&&(A=E>>>8|0,A<<=i=A+1048320>>>16&8,A=28+((A=((A<<=B=A+520192>>>16&4)<<(g=A+245760>>>16&2)>>>15|0)-(g|B|i)|0)<<1|E>>>A+21&1)|0),n[e+28>>2]=A,n[e+16>>2]=0,n[e+20>>2]=0,g=2384+(A<<2)|0,(i=n[521])&(B=1<<A)){for(A=E<<(31==(0|A)?0:25-(A>>>1|0)|0),i=n[g>>2];;){if((0|E)==(-8&n[(g=i)+4>>2]))break C;if(B=A>>>29|0,A<<=1,!(i=n[(B=(4&B)+g|0)+16>>2]))break}n[B+16>>2]=e}else n[521]=B|i,n[g>>2]=e;n[e+24>>2]=g,n[e+12>>2]=e,n[e+8>>2]=e;break i}A=n[g+8>>2],n[A+12>>2]=o,n[g+8>>2]=o,n[o+24>>2]=0,n[o+12>>2]=g,n[o+8>>2]=A}A=a+8|0;break A}A=n[g+8>>2],n[A+12>>2]=e,n[g+8>>2]=e,n[e+24>>2]=0,n[e+12>>2]=g,n[e+8>>2]=A}if(!((A=n[523])>>>0<=t>>>0)){g=A-t|0,n[523]=g,A=(B=n[526])+t|0,n[526]=A,n[A+4>>2]=1|g,n[B+4>>2]=3|t,A=B+8|0;break A}}n[519]=48,A=0;break A}g:if(C){g=n[i+28>>2];B:{if(n[(A=2384+(g<<2)|0)>>2]==(0|i)){if(n[A>>2]=B,B)break B;a=X(g)&a,n[521]=a;break g}if(n[C+(n[C+16>>2]==(0|i)?16:20)>>2]=B,!B)break g}n[B+24>>2]=C,(A=n[i+16>>2])&&(n[B+16>>2]=A,n[A+24>>2]=B),(A=n[i+20>>2])&&(n[B+20>>2]=A,n[A+24>>2]=B)}g:if(E>>>0<=15)A=E+t|0,n[i+4>>2]=3|A,n[(A=A+i|0)+4>>2]=1|n[A+4>>2];else if(n[i+4>>2]=3|t,n[(C=i+t|0)+4>>2]=1|E,n[C+E>>2]=E,E>>>0<=255)g=2120+(-8&E)|0,(B=n[520])&(A=1<<(E>>>3))?A=n[g+8>>2]:(n[520]=A|B,A=g),n[g+8>>2]=C,n[A+12>>2]=C,n[C+12>>2]=g,n[C+8>>2]=A;else{A=31,E>>>0<=16777215&&(A=E>>>8|0,A<<=e=A+1048320>>>16&8,A=28+((A=((A<<=B=A+520192>>>16&4)<<(g=A+245760>>>16&2)>>>15|0)-(g|B|e)|0)<<1|E>>>A+21&1)|0),n[C+28>>2]=A,n[C+16>>2]=0,n[C+20>>2]=0,g=2384+(A<<2)|0;B:{if((B=1<<A)&a){for(A=E<<(31==(0|A)?0:25-(A>>>1|0)|0),t=n[g>>2];;){if((-8&n[(g=t)+4>>2])==(0|E))break B;if(B=A>>>29|0,A<<=1,!(t=n[(B=(4&B)+g|0)+16>>2]))break}n[B+16>>2]=C}else n[521]=B|a,n[g>>2]=C;n[C+24>>2]=g,n[C+12>>2]=C,n[C+8>>2]=C;break g}A=n[g+8>>2],n[A+12>>2]=C,n[g+8>>2]=C,n[C+24>>2]=0,n[C+12>>2]=g,n[C+8>>2]=A}A=i+8|0;break A}I:if(o){g=n[B+28>>2];g:{if(n[(A=2384+(g<<2)|0)>>2]==(0|B)){if(n[A>>2]=i,i)break g;n[521]=X(g)&a;break I}if(n[o+(n[o+16>>2]==(0|B)?16:20)>>2]=i,!i)break I}n[i+24>>2]=o,(A=n[B+16>>2])&&(n[i+16>>2]=A,n[A+24>>2]=i),(A=n[B+20>>2])&&(n[i+20>>2]=A,n[A+24>>2]=i)}E>>>0<=15?(A=E+t|0,n[B+4>>2]=3|A,n[(A=A+B|0)+4>>2]=1|n[A+4>>2]):(n[B+4>>2]=3|t,n[(i=B+t|0)+4>>2]=1|E,n[i+E>>2]=E,f&&(g=2120+(-8&f)|0,e=n[525],(A=1<<(f>>>3))&C?A=n[g+8>>2]:(n[520]=A|C,A=g),n[g+8>>2]=e,n[A+12>>2]=e,n[e+12>>2]=g,n[e+8>>2]=A),n[525]=i,n[522]=E),A=B+8|0}return c=I+16|0,0|A}function h(A){var I=0,g=0,B=0,i=0,C=0,Q=0,E=0,r=0,e=0,t=0,o=0,a=0,f=0,c=0,u=0,h=0,D=0,y=0,F=0,w=0,k=0,p=0,N=0,d=0,l=0,U=0,R=0,G=0,Y=0,M=0,b=0,m=0,J=0,H=0,L=0,v=0,q=0,K=0,x=0,_=0,j=0,W=0,X=0,T=0,O=0,V=0,Z=0,P=0,z=0,$=0,AA=0,IA=0,gA=0,BA=0,iA=0,CA=0,nA=0,QA=0,EA=0,rA=0,eA=0,tA=0,oA=0,aA=0,fA=0,cA=0,sA=0,uA=0,hA=0,DA=0,yA=0,FA=0,wA=0,kA=0,pA=0,NA=0,dA=0,lA=0,SA=0,UA=0,RA=0,GA=0,YA=0,MA=0,bA=0,mA=0,JA=0,HA=0,LA=0,vA=0,qA=0,KA=0,xA=0,_A=0,jA=0;for(P=n[A+192>>2],v=n[A+196>>2],z=n[A+184>>2],$=n[A+188>>2],y=n[A+176>>2],x=n[A+180>>2],F=n[A+168>>2],_=n[A+172>>2],j=n[A+160>>2],w=n[A+164>>2],k=n[A+152>>2],Q=n[A+156>>2],p=n[A+144>>2],N=n[A+148>>2],d=n[A+136>>2],W=n[A+140>>2],AA=n[A+128>>2],IA=n[A+132>>2],l=n[A+120>>2],X=n[A+124>>2],U=n[A+112>>2],e=n[A+116>>2],E=n[A+104>>2],t=n[A+108>>2],T=n[A+96>>2],R=n[A+100>>2],g=n[A+88>>2],B=n[A+92>>2],G=n[A+80>>2],O=n[A+84>>2],r=n[A+72>>2],o=n[A+76>>2],u=n[A+64>>2],a=n[A+68>>2],h=n[A+56>>2],i=n[A+60>>2],D=n[A+48>>2],Y=n[A+52>>2],C=n[A+40>>2],M=n[A+44>>2],EA=n[A+32>>2],gA=n[A+36>>2],b=n[A+24>>2],m=n[A+28>>2],f=n[A+16>>2],J=n[A+20>>2],I=n[A+8>>2],c=n[A+12>>2],q=n[A>>2],H=n[A+4>>2];sA>>>0<=23;)BA=V=H^M^O^w^X,nA=b^u^E^p^z,V=S(Z=q^C^G^l^j,V,1)^nA,L=S(EA^V,gA^(QA=(oA=m^a^t^N^$)^s),27),iA=s,rA=K=J^i^R^W^x,Z=S(aA=f^h^T^d^y,K,1)^Z,K=S(g^Z,B^(BA^=s),10),CA=s,fA=B=c^Y^B^_^IA,eA=EA^r^U^k^P,g=S(cA=I^D^g^F^AA,B,1)^eA,C=S(C^g,M^(B=(gA=gA^o^e^Q^v)^s),36),uA=s,M=S(I^Z,c^BA,1),EA=s,c=E,E=S(eA,gA,1)^aA,gA=S(c^E,(tA=t)^(t=s^rA),25),I=s,tA=h,h=S(nA,oA,1)^cA,c=S(tA^h,(rA=i)^(i=s^fA),6),oA=s,b=S(E^b,t^m,28),m=s,G=S(g^G,B^O,3),O=s,nA=S(r^V,o^QA,20),rA=r=s,o=S(f^h,i^J,62),f=s,U=S(U^V,e^QA,39),e=s,J=S(E^u,a^t,55),aA=u=s,eA=n[(a=(hA=sA<<3)+1120|0)>>2],a=n[a+4>>2],T=S(h^T,i^R,43),R=s,D=S(D^Z,Y^BA,44),a=r=(DA=(fA=B^H)^(-1^(Y=s))&R^a)^(yA=(-1^u)&e^f)^(FA=(-1^r)&O^m)^(wA=(-1^uA)&CA^iA)^(kA=(-1^oA)&I^EA),r=S(u=(pA=(tA=eA)^(eA=g^q)^(-1^D)&T)^(NA=(-1^J)&U^o)^(dA=(-1^nA)&G^b)^(lA=(-1^C)&K^L)^(SA=(-1^c)&gA^M),r,1),q=s,d=S(d^h,i^W,15),W=s,z=S(E^z,t^$,56),cA=H=s,$=S(E^p,t^N,21),p=s,P=S(P^V,v^QA,14),E=s,v=S(g^l,B^X,41),N=s,F=S(F^Z,_^BA,2),l=s,_=S(k^V,Q^QA,8),k=s,j=S(g^j,B^w,18),X=s,w=S(Z^AA,IA^BA,45),Q=s,UA=(-1^z)&L^d,RA=(-1^P)&eA^$,AA=(-1^F)&o^v,GA=(-1^j)&M^_,y=S(h^y,i^x,61),q=S((r^=t=UA^RA^AA^GA^(IA=(-1^y)&b^w))^(g=(-1^o)&J^F),(o=(JA=(YA=(-1^H)&iA^W)^(MA=(-1^E)&fA^p)^(B=(-1^l)&f^N)^(bA=(-1^X)&EA^k)^(mA=(-1^(x=s))&m^Q))^q)^(i=(-1^f)&aA^l),14),H=s,u=S(F=(HA=gA^(-1^_)&j)^(f=T^(-1^$)&P)^(LA=(-1^v)&F^U)^(vA=G^(-1^w)&y)^(qA=(-1^d)&z^K),l=(KA=I^(-1^k)&X)^(xA=R^(-1^p)&E)^(_A=(-1^N)&l^e)^(jA=O^(-1^Q)&x)^(tA=(-1^W)&cA^CA),1)^u,V=S(u^(w=(-1^G)&w^nA),(a^=s)^(Q=(-1^O)&Q^rA),44),QA=s,N=aA^(-1^e)&N,G=oA^(-1^I)&k,I=(-1^R)&p^Y,h=(P=(L=z^(-1^L)&C)^(nA=y^(-1^b)&nA)^(p=j^(-1^M)&c)^g^(e=(-1^eA)&D^P))^S(z=(v=J^(-1^U)&v)^(R=c^(-1^gA)&_)^(K=C^(-1^K)&d)^(g=(-1^T)&$^D)^w,k=N^G^(CA=uA^(-1^CA)&W)^I^Q,1),BA=y=(i=($=(C=cA^(-1^iA)&uA)^(M=x^(-1^m)&rA)^(d=X^(-1^EA)&oA)^i^(U=(-1^fA)&Y^E))^s)^DA,EA=(-1^(Z=h^pA))&V^q,gA=(-1^y)&QA^H,y=S(u^v,a^N,2),x=s,b=S(P,$,1)^F,j=S(IA^b,mA^(m=l^s),55),w=s,rA=f,f=S(t,JA,1)^z,P=(-1^(k=S(rA^f,(J=k^s)^xA,62)))&j^y,v=(-1^(Q=v=s))&w^x,z=(F=S(h^lA,i^wA,41))^(-1^y)&k,$=(_=s)^(-1^x)&Q,y=(p=S(r^p,o^d,39))^(-1^F)&y,x=(N=s)^(-1^_)&x,F=j^(-1^p)&F,_=w^(-1^N)&_,j=k^(-1^j)&p,w=Q^(-1^w)&N,d=S(b^AA,B^m,56),W=s,l=S(h^dA,i^FA,36),X=s,U=S(e^r,o^U,27),e=Q=s,k=(-1^U)&l^d,Q=(-1^Q)&X^W,p=(AA=S(f^qA,J^tA,15))^(-1^d)&U,N=(IA=s)^(-1^W)&e,d=(E=S(R^u,a^G,10))^(-1^AA)&d,W=(t=s)^(-1^IA)&W,AA=l^(-1^E)&AA,IA=X^(-1^t)&IA,l=(-1^l)&E^U,X=(-1^X)&t^e,T=S(h^NA,i^yA,18),R=s,G=S(f^vA,J^jA,6),O=s,D=S(g^u,I^a,1),Y=e=s,U=(-1^D)&G^T,e=(-1^e)&O^R,E=(g=S(r^L,C^o,8))^(-1^T)&D,t=(B=s)^(-1^R)&Y,T=(C=S(b^GA,m^bA,25))^(-1^g)&T,R=(I=s)^(-1^B)&R,g=G^(-1^C)&g,B=O^(-1^I)&B,G=(-1^G)&C^D,O=(-1^O)&I^Y,D=S(f^LA,J^_A,61),Y=s,C=S(r^nA,o^M,20),M=s,I=S(b^RA,m^MA,28),c=o=s,r=(-1^I)&C^D,o=(-1^o)&M^Y,u=(L=S(u^K,a^CA,45))^(-1^D)&I,a=(iA=s)^(-1^Y)&c,h=(K=S(h^SA,i^kA,3))^(-1^L)&D,i=(CA=s)^(-1^iA)&Y,D=(-1^K)&L^C,Y=(-1^CA)&iA^M,C=I^(-1^C)&K,M=c^(-1^M)&CA,b=(I=S(b^UA,m^YA,21))^(-1^q)&Z,m=(c=s)^(-1^H)&BA,f=(L=S(f^HA,J^KA,43))^(-1^I)&q,J=(q=iA=s)^(-1^c)&H,I=(-1^L)&I^V,c=(-1^q)&c^QA,q=(-1^V)&L^n[(H=1120+(8|hA)|0)>>2]^Z,H=(-1^QA)&iA^n[H+4>>2]^BA,sA=sA+2|0;n[A+192>>2]=P,n[A+196>>2]=v,n[A+184>>2]=z,n[A+188>>2]=$,n[A+176>>2]=y,n[A+180>>2]=x,n[A+168>>2]=F,n[A+172>>2]=_,n[A+160>>2]=j,n[A+164>>2]=w,n[A+152>>2]=k,n[A+156>>2]=Q,n[A+144>>2]=p,n[A+148>>2]=N,n[A+136>>2]=d,n[A+140>>2]=W,n[A+128>>2]=AA,n[A+132>>2]=IA,n[A+120>>2]=l,n[A+124>>2]=X,n[A+112>>2]=U,n[A+116>>2]=e,n[A+104>>2]=E,n[A+108>>2]=t,n[A+96>>2]=T,n[A+100>>2]=R,n[A+88>>2]=g,n[A+92>>2]=B,n[A+80>>2]=G,n[A+84>>2]=O,n[A+72>>2]=r,n[A+76>>2]=o,n[A+64>>2]=u,n[A+68>>2]=a,n[A+56>>2]=h,n[A+60>>2]=i,n[A+48>>2]=D,n[A+52>>2]=Y,n[A+40>>2]=C,n[A+44>>2]=M,n[A+32>>2]=EA,n[A+36>>2]=gA,n[A+24>>2]=b,n[A+28>>2]=m,n[A+16>>2]=f,n[A+20>>2]=J,n[A+8>>2]=I,n[A+12>>2]=c,n[A>>2]=q,n[A+4>>2]=H}function D(A,I,g,i,C,E,r,e,t,o,a,f){var c,s=0,u=0,h=0,D=0,y=0,F=0,w=0;for(c=1^r;!(F>>>t|0);){for(s=(y<<5)+g|0,EA[0|o](s,C,E,e+F|0,a,f),n[(y<<2)+i>>2]=0,(0|F)==(0|c)&&(u=Q[s+4|0]|Q[s+5|0]<<8|Q[s+6|0]<<16|Q[s+7|0]<<24,h=Q[0|s]|Q[s+1|0]<<8|Q[s+2|0]<<16|Q[s+3|0]<<24,B[0|I]=h,B[I+1|0]=h>>>8,B[I+2|0]=h>>>16,B[I+3|0]=h>>>24,B[I+4|0]=u,B[I+5|0]=u>>>8,B[I+6|0]=u>>>16,B[I+7|0]=u>>>24,u=Q[s+28|0]|Q[s+29|0]<<8|Q[s+30|0]<<16|Q[s+31|0]<<24,h=Q[s+24|0]|Q[s+25|0]<<8|Q[s+26|0]<<16|Q[s+27|0]<<24,B[I+24|0]=h,B[I+25|0]=h>>>8,B[I+26|0]=h>>>16,B[I+27|0]=h>>>24,B[I+28|0]=u,B[I+29|0]=u>>>8,B[I+30|0]=u>>>16,B[I+31|0]=u>>>24,u=Q[s+20|0]|Q[s+21|0]<<8|Q[s+22|0]<<16|Q[s+23|0]<<24,h=Q[s+16|0]|Q[s+17|0]<<8|Q[s+18|0]<<16|Q[s+19|0]<<24,B[I+16|0]=h,B[I+17|0]=h>>>8,B[I+18|0]=h>>>16,B[I+19|0]=h>>>24,B[I+20|0]=u,B[I+21|0]=u>>>8,B[I+22|0]=u>>>16,B[I+23|0]=u>>>24,u=Q[s+12|0]|Q[s+13|0]<<8|Q[s+14|0]<<16|Q[s+15|0]<<24,s=Q[s+8|0]|Q[s+9|0]<<8|Q[s+10|0]<<16|Q[s+11|0]<<24,B[I+8|0]=s,B[I+9|0]=s>>>8,B[I+10|0]=s>>>16,B[I+11|0]=s>>>24,B[I+12|0]=u,B[I+13|0]=u>>>8,B[I+14|0]=u>>>16,B[I+15|0]=u>>>24),y=y+1|0;!(y>>>0<2||(0|(w=n[(D=((u=y-1|0)<<2)+i|0)>>2]))!=n[(h=((s=y-2|0)<<2)+i|0)>>2]);)IA(a,y=w+1|0),gA(a,(w=e>>>n[D>>2]+1|0)+(D=F>>>y|0)|0),V(s=(s<<5)+g|0,s,E,a),y=h,h=n[h>>2]+1|0,n[y>>2]=h,y=u,(0|D)==(r>>>h^1)&&(u=(h<<5)+I|0,h=Q[s+4|0]|Q[s+5|0]<<8|Q[s+6|0]<<16|Q[s+7|0]<<24,D=Q[0|s]|Q[s+1|0]<<8|Q[s+2|0]<<16|Q[s+3|0]<<24,B[0|u]=D,B[u+1|0]=D>>>8,B[u+2|0]=D>>>16,B[u+3|0]=D>>>24,B[u+4|0]=h,B[u+5|0]=h>>>8,B[u+6|0]=h>>>16,B[u+7|0]=h>>>24,h=Q[s+28|0]|Q[s+29|0]<<8|Q[s+30|0]<<16|Q[s+31|0]<<24,D=Q[s+24|0]|Q[s+25|0]<<8|Q[s+26|0]<<16|Q[s+27|0]<<24,B[u+24|0]=D,B[u+25|0]=D>>>8,B[u+26|0]=D>>>16,B[u+27|0]=D>>>24,B[u+28|0]=h,B[u+29|0]=h>>>8,B[u+30|0]=h>>>16,B[u+31|0]=h>>>24,h=Q[s+20|0]|Q[s+21|0]<<8|Q[s+22|0]<<16|Q[s+23|0]<<24,D=Q[s+16|0]|Q[s+17|0]<<8|Q[s+18|0]<<16|Q[s+19|0]<<24,B[u+16|0]=D,B[u+17|0]=D>>>8,B[u+18|0]=D>>>16,B[u+19|0]=D>>>24,B[u+20|0]=h,B[u+21|0]=h>>>8,B[u+22|0]=h>>>16,B[u+23|0]=h>>>24,h=Q[s+12|0]|Q[s+13|0]<<8|Q[s+14|0]<<16|Q[s+15|0]<<24,s=Q[s+8|0]|Q[s+9|0]<<8|Q[s+10|0]<<16|Q[s+11|0]<<24,B[u+8|0]=s,B[u+9|0]=s>>>8,B[u+10|0]=s>>>16,B[u+11|0]=s>>>24,B[u+12|0]=h,B[u+13|0]=h>>>8,B[u+14|0]=h>>>16,B[u+15|0]=h>>>24);F=F+1|0}I=Q[g+4|0]|Q[g+5|0]<<8|Q[g+6|0]<<16|Q[g+7|0]<<24,i=Q[0|g]|Q[g+1|0]<<8|Q[g+2|0]<<16|Q[g+3|0]<<24,B[0|A]=i,B[A+1|0]=i>>>8,B[A+2|0]=i>>>16,B[A+3|0]=i>>>24,B[A+4|0]=I,B[A+5|0]=I>>>8,B[A+6|0]=I>>>16,B[A+7|0]=I>>>24,I=Q[g+28|0]|Q[g+29|0]<<8|Q[g+30|0]<<16|Q[g+31|0]<<24,i=Q[g+24|0]|Q[g+25|0]<<8|Q[g+26|0]<<16|Q[g+27|0]<<24,B[A+24|0]=i,B[A+25|0]=i>>>8,B[A+26|0]=i>>>16,B[A+27|0]=i>>>24,B[A+28|0]=I,B[A+29|0]=I>>>8,B[A+30|0]=I>>>16,B[A+31|0]=I>>>24,I=Q[g+20|0]|Q[g+21|0]<<8|Q[g+22|0]<<16|Q[g+23|0]<<24,i=Q[g+16|0]|Q[g+17|0]<<8|Q[g+18|0]<<16|Q[g+19|0]<<24,B[A+16|0]=i,B[A+17|0]=i>>>8,B[A+18|0]=i>>>16,B[A+19|0]=i>>>24,B[A+20|0]=I,B[A+21|0]=I>>>8,B[A+22|0]=I>>>16,B[A+23|0]=I>>>24,I=Q[g+12|0]|Q[g+13|0]<<8|Q[g+14|0]<<16|Q[g+15|0]<<24,g=Q[g+8|0]|Q[g+9|0]<<8|Q[g+10|0]<<16|Q[g+11|0]<<24,B[A+8|0]=g,B[A+9|0]=g>>>8,B[A+10|0]=g>>>16,B[A+11|0]=g>>>24,B[A+12|0]=I,B[A+13|0]=I>>>8,B[A+14|0]=I>>>16,B[A+15|0]=I>>>24}function y(A,I,g,i,C,E,r,e){var t,o,a,f=0,s=0,u=0;for(c=t=c+-64|0,1&g?(s=Q[I+28|0]|Q[I+29|0]<<8|Q[I+30|0]<<16|Q[I+31|0]<<24,n[t+56>>2]=Q[I+24|0]|Q[I+25|0]<<8|Q[I+26|0]<<16|Q[I+27|0]<<24,n[t+60>>2]=s,s=Q[I+20|0]|Q[I+21|0]<<8|Q[I+22|0]<<16|Q[I+23|0]<<24,n[t+48>>2]=Q[I+16|0]|Q[I+17|0]<<8|Q[I+18|0]<<16|Q[I+19|0]<<24,n[t+52>>2]=s,s=Q[I+12|0]|Q[I+13|0]<<8|Q[I+14|0]<<16|Q[I+15|0]<<24,n[t+40>>2]=Q[I+8|0]|Q[I+9|0]<<8|Q[I+10|0]<<16|Q[I+11|0]<<24,n[t+44>>2]=s,s=Q[I+4|0]|Q[I+5|0]<<8|Q[I+6|0]<<16|Q[I+7|0]<<24,n[t+32>>2]=Q[0|I]|Q[I+1|0]<<8|Q[I+2|0]<<16|Q[I+3|0]<<24,n[t+36>>2]=s,I=t):(s=Q[I+28|0]|Q[I+29|0]<<8|Q[I+30|0]<<16|Q[I+31|0]<<24,n[t+24>>2]=Q[I+24|0]|Q[I+25|0]<<8|Q[I+26|0]<<16|Q[I+27|0]<<24,n[t+28>>2]=s,s=Q[I+20|0]|Q[I+21|0]<<8|Q[I+22|0]<<16|Q[I+23|0]<<24,n[t+16>>2]=Q[I+16|0]|Q[I+17|0]<<8|Q[I+18|0]<<16|Q[I+19|0]<<24,n[t+20>>2]=s,s=Q[I+4|0]|Q[I+5|0]<<8|Q[I+6|0]<<16|Q[I+7|0]<<24,n[t>>2]=Q[0|I]|Q[I+1|0]<<8|Q[I+2|0]<<16|Q[I+3|0]<<24,n[t+4>>2]=s,s=Q[I+12|0]|Q[I+13|0]<<8|Q[I+14|0]<<16|Q[I+15|0]<<24,n[t+8>>2]=Q[I+8|0]|Q[I+9|0]<<8|Q[I+10|0]<<16|Q[I+11|0]<<24,n[t+12>>2]=s,I=t+32|0),s=Q[C+4|0]|Q[C+5|0]<<8|Q[C+6|0]<<16|Q[C+7|0]<<24,f=Q[0|C]|Q[C+1|0]<<8|Q[C+2|0]<<16|Q[C+3|0]<<24,B[0|I]=f,B[I+1|0]=f>>>8,B[I+2|0]=f>>>16,B[I+3|0]=f>>>24,B[I+4|0]=s,B[I+5|0]=s>>>8,B[I+6|0]=s>>>16,B[I+7|0]=s>>>24,s=Q[C+28|0]|Q[C+29|0]<<8|Q[C+30|0]<<16|Q[C+31|0]<<24,f=Q[C+24|0]|Q[C+25|0]<<8|Q[C+26|0]<<16|Q[C+27|0]<<24,B[I+24|0]=f,B[I+25|0]=f>>>8,B[I+26|0]=f>>>16,B[I+27|0]=f>>>24,B[I+28|0]=s,B[I+29|0]=s>>>8,B[I+30|0]=s>>>16,B[I+31|0]=s>>>24,s=Q[C+20|0]|Q[C+21|0]<<8|Q[C+22|0]<<16|Q[C+23|0]<<24,f=Q[C+16|0]|Q[C+17|0]<<8|Q[C+18|0]<<16|Q[C+19|0]<<24,B[I+16|0]=f,B[I+17|0]=f>>>8,B[I+18|0]=f>>>16,B[I+19|0]=f>>>24,B[I+20|0]=s,B[I+21|0]=s>>>8,B[I+22|0]=s>>>16,B[I+23|0]=s>>>24,s=Q[C+12|0]|Q[C+13|0]<<8|Q[C+14|0]<<16|Q[C+15|0]<<24,f=Q[C+8|0]|Q[C+9|0]<<8|Q[C+10|0]<<16|Q[C+11|0]<<24,B[I+8|0]=f,B[I+9|0]=f>>>8,B[I+10|0]=f>>>16,B[I+11|0]=f>>>24,B[I+12|0]=s,B[I+13|0]=s>>>8,B[I+14|0]=s>>>16,B[I+15|0]=s>>>24,a=E-1|0,o=t+32|0,I=0;(0|I)!=(0|a);)IA(e,I=I+1|0),gA(e,(i=i>>>1|0)+(s=g>>>1|0)|0),V((g&=2)?o:t,t,r,e),f=Q[C+60|0]|Q[C+61|0]<<8|Q[C+62|0]<<16|Q[C+63|0]<<24,g=g?t:o,u=Q[C+56|0]|Q[C+57|0]<<8|Q[C+58|0]<<16|Q[C+59|0]<<24,B[g+24|0]=u,B[g+25|0]=u>>>8,B[g+26|0]=u>>>16,B[g+27|0]=u>>>24,B[g+28|0]=f,B[g+29|0]=f>>>8,B[g+30|0]=f>>>16,B[g+31|0]=f>>>24,f=Q[C+52|0]|Q[C+53|0]<<8|Q[C+54|0]<<16|Q[C+55|0]<<24,u=Q[C+48|0]|Q[C+49|0]<<8|Q[C+50|0]<<16|Q[C+51|0]<<24,B[g+16|0]=u,B[g+17|0]=u>>>8,B[g+18|0]=u>>>16,B[g+19|0]=u>>>24,B[g+20|0]=f,B[g+21|0]=f>>>8,B[g+22|0]=f>>>16,B[g+23|0]=f>>>24,f=Q[C+44|0]|Q[C+45|0]<<8|Q[C+46|0]<<16|Q[C+47|0]<<24,u=Q[C+40|0]|Q[C+41|0]<<8|Q[C+42|0]<<16|Q[C+43|0]<<24,B[g+8|0]=u,B[g+9|0]=u>>>8,B[g+10|0]=u>>>16,B[g+11|0]=u>>>24,B[g+12|0]=f,B[g+13|0]=f>>>8,B[g+14|0]=f>>>16,B[g+15|0]=f>>>24,f=Q[C+36|0]|Q[C+37|0]<<8|Q[C+38|0]<<16|Q[C+39|0]<<24,u=Q[C+32|0]|Q[C+33|0]<<8|Q[C+34|0]<<16|Q[C+35|0]<<24,B[0|g]=u,B[g+1|0]=u>>>8,B[g+2|0]=u>>>16,B[g+3|0]=u>>>24,B[g+4|0]=f,B[g+5|0]=f>>>8,B[g+6|0]=f>>>16,B[g+7|0]=f>>>24,C=C+32|0,g=s;IA(e,E),gA(e,(g>>>1)+(i>>>1)|0),V(A,t,r,e),c=t- -64|0}function F(A){var I=0,g=0,B=0,i=0,C=0,Q=0,E=0;A:if(A|=0){C=(B=A-8|0)+(A=-8&(I=n[A-4>>2]))|0;I:if(!(1&I)){if(!(3&I))break A;if((B=B-(I=n[B>>2])|0)>>>0<r[524])break A;if(A=A+I|0,n[525]==(0|B)){if(3==(3&(I=n[C+4>>2])))return n[522]=A,n[C+4>>2]=-2&I,n[B+4>>2]=1|A,void(n[A+B>>2]=A)}else{if(I>>>0<=255){if(i=n[B+8>>2],I=I>>>3|0,(0|(g=n[B+12>>2]))==(0|i)){n[520]=n[520]&X(I);break I}n[i+12>>2]=g,n[g+8>>2]=i;break I}if(E=n[B+24>>2],(0|B)==(0|(I=n[B+12>>2])))if((g=n[(i=B+20|0)>>2])||(g=n[(i=B+16|0)>>2])){for(;Q=i,(g=n[(i=(I=g)+20|0)>>2])||(i=I+16|0,g=n[I+16>>2]););n[Q>>2]=0}else I=0;else g=n[B+8>>2],n[g+12>>2]=I,n[I+8>>2]=g;if(!E)break I;i=n[B+28>>2];g:{if(n[(g=2384+(i<<2)|0)>>2]==(0|B)){if(n[g>>2]=I,I)break g;n[521]=n[521]&X(i);break I}if(n[E+(n[E+16>>2]==(0|B)?16:20)>>2]=I,!I)break I}if(n[I+24>>2]=E,(g=n[B+16>>2])&&(n[I+16>>2]=g,n[g+24>>2]=I),!(g=n[B+20>>2]))break I;n[I+20>>2]=g,n[g+24>>2]=I}}if(!(B>>>0>=C>>>0)&&1&(I=n[C+4>>2])){I:{if(!(2&I)){if(n[526]==(0|C)){if(n[526]=B,A=n[523]+A|0,n[523]=A,n[B+4>>2]=1|A,n[525]!=(0|B))break A;return n[522]=0,void(n[525]=0)}if(n[525]==(0|C))return n[525]=B,A=n[522]+A|0,n[522]=A,n[B+4>>2]=1|A,void(n[A+B>>2]=A);A=(-8&I)+A|0;g:if(I>>>0<=255){if(i=n[C+8>>2],I=I>>>3|0,(0|(g=n[C+12>>2]))==(0|i)){n[520]=n[520]&X(I);break g}n[i+12>>2]=g,n[g+8>>2]=i}else{if(E=n[C+24>>2],(0|C)==(0|(I=n[C+12>>2])))if((g=n[(i=C+20|0)>>2])||(g=n[(i=C+16|0)>>2])){for(;Q=i,(g=n[(i=(I=g)+20|0)>>2])||(i=I+16|0,g=n[I+16>>2]););n[Q>>2]=0}else I=0;else g=n[C+8>>2],n[g+12>>2]=I,n[I+8>>2]=g;if(E){i=n[C+28>>2];B:{if(n[(g=2384+(i<<2)|0)>>2]==(0|C)){if(n[g>>2]=I,I)break B;n[521]=n[521]&X(i);break g}if(n[E+(n[E+16>>2]==(0|C)?16:20)>>2]=I,!I)break g}n[I+24>>2]=E,(g=n[C+16>>2])&&(n[I+16>>2]=g,n[g+24>>2]=I),(g=n[C+20>>2])&&(n[I+20>>2]=g,n[g+24>>2]=I)}}if(n[B+4>>2]=1|A,n[A+B>>2]=A,n[525]!=(0|B))break I;return void(n[522]=A)}n[C+4>>2]=-2&I,n[B+4>>2]=1|A,n[A+B>>2]=A}if(A>>>0<=255)return I=2120+(-8&A)|0,(g=n[520])&(A=1<<(A>>>3))?A=n[I+8>>2]:(n[520]=A|g,A=I),n[I+8>>2]=B,n[A+12>>2]=B,n[B+12>>2]=I,void(n[B+8>>2]=A);i=31,A>>>0<=16777215&&(I=A>>>8|0,I<<=Q=I+1048320>>>16&8,i=28+((I=((I<<=i=I+520192>>>16&4)<<(g=I+245760>>>16&2)>>>15|0)-(g|i|Q)|0)<<1|A>>>I+21&1)|0),n[B+28>>2]=i,n[B+16>>2]=0,n[B+20>>2]=0,Q=2384+(i<<2)|0;I:{g:{if((g=n[521])&(I=1<<i)){for(i=A<<(31==(0|i)?0:25-(i>>>1|0)|0),I=n[Q>>2];;){if(g=I,(-8&n[I+4>>2])==(0|A))break g;if(I=i>>>29|0,i<<=1,!(I=n[(Q=g+(4&I)|0)+16>>2]))break}n[Q+16>>2]=B,n[B+24>>2]=g}else n[521]=I|g,n[Q>>2]=B,n[B+24>>2]=Q;n[B+12>>2]=B,n[B+8>>2]=B;break I}A=n[g+8>>2],n[A+12>>2]=B,n[g+8>>2]=B,n[B+24>>2]=0,n[B+12>>2]=g,n[B+8>>2]=A}A=n[528]-1|0,n[528]=A||-1}}}function w(A,I,g,i,C,n){var E=0,r=0;for(E=Q[C+4|0]|Q[C+5|0]<<8|Q[C+6|0]<<16|Q[C+7|0]<<24,r=Q[0|C]|Q[C+1|0]<<8|Q[C+2|0]<<16|Q[C+3|0]<<24,B[0|I]=r,B[I+1|0]=r>>>8,B[I+2|0]=r>>>16,B[I+3|0]=r>>>24,B[I+4|0]=E,B[I+5|0]=E>>>8,B[I+6|0]=E>>>16,B[I+7|0]=E>>>24,E=Q[C+28|0]|Q[C+29|0]<<8|Q[C+30|0]<<16|Q[C+31|0]<<24,r=Q[C+24|0]|Q[C+25|0]<<8|Q[C+26|0]<<16|Q[C+27|0]<<24,B[I+24|0]=r,B[I+25|0]=r>>>8,B[I+26|0]=r>>>16,B[I+27|0]=r>>>24,B[I+28|0]=E,B[I+29|0]=E>>>8,B[I+30|0]=E>>>16,B[I+31|0]=E>>>24,E=Q[C+20|0]|Q[C+21|0]<<8|Q[C+22|0]<<16|Q[C+23|0]<<24,r=Q[C+16|0]|Q[C+17|0]<<8|Q[C+18|0]<<16|Q[C+19|0]<<24,B[I+16|0]=r,B[I+17|0]=r>>>8,B[I+18|0]=r>>>16,B[I+19|0]=r>>>24,B[I+20|0]=E,B[I+21|0]=E>>>8,B[I+22|0]=E>>>16,B[I+23|0]=E>>>24,E=Q[C+12|0]|Q[C+13|0]<<8|Q[C+14|0]<<16|Q[C+15|0]<<24,C=Q[C+8|0]|Q[C+9|0]<<8|Q[C+10|0]<<16|Q[C+11|0]<<24,B[I+8|0]=C,B[I+9|0]=C>>>8,B[I+10|0]=C>>>16,B[I+11|0]=C>>>24,B[I+12|0]=E,B[I+13|0]=E>>>8,B[I+14|0]=E>>>16,B[I+15|0]=E>>>24,_(I+32|0,n),p(n=I- -64|0,i<<=5,I,64),C=0;(0|i)!=(0|C);)B[(I+C|0)- -64|0]=Q[C+n|0]^Q[g+C|0],C=C+1|0;p(A,32,I,i- -64|0)}function k(A,I,g,i,C,n){var E=0,r=0;for(E=Q[I+4|0]|Q[I+5|0]<<8|Q[I+6|0]<<16|Q[I+7|0]<<24,r=Q[0|I]|Q[I+1|0]<<8|Q[I+2|0]<<16|Q[I+3|0]<<24,B[0|A]=r,B[A+1|0]=r>>>8,B[A+2|0]=r>>>16,B[A+3|0]=r>>>24,B[A+4|0]=E,B[A+5|0]=E>>>8,B[A+6|0]=E>>>16,B[A+7|0]=E>>>24,E=Q[I+28|0]|Q[I+29|0]<<8|Q[I+30|0]<<16|Q[I+31|0]<<24,r=Q[I+24|0]|Q[I+25|0]<<8|Q[I+26|0]<<16|Q[I+27|0]<<24,B[A+24|0]=r,B[A+25|0]=r>>>8,B[A+26|0]=r>>>16,B[A+27|0]=r>>>24,B[A+28|0]=E,B[A+29|0]=E>>>8,B[A+30|0]=E>>>16,B[A+31|0]=E>>>24,E=Q[I+20|0]|Q[I+21|0]<<8|Q[I+22|0]<<16|Q[I+23|0]<<24,r=Q[I+16|0]|Q[I+17|0]<<8|Q[I+18|0]<<16|Q[I+19|0]<<24,B[A+16|0]=r,B[A+17|0]=r>>>8,B[A+18|0]=r>>>16,B[A+19|0]=r>>>24,B[A+20|0]=E,B[A+21|0]=E>>>8,B[A+22|0]=E>>>16,B[A+23|0]=E>>>24,E=Q[I+12|0]|Q[I+13|0]<<8|Q[I+14|0]<<16|Q[I+15|0]<<24,I=Q[I+8|0]|Q[I+9|0]<<8|Q[I+10|0]<<16|Q[I+11|0]<<24,B[A+8|0]=I,B[A+9|0]=I>>>8,B[A+10|0]=I>>>16,B[A+11|0]=I>>>24,B[A+12|0]=E,B[A+13|0]=E>>>8,B[A+14|0]=E>>>16,B[A+15|0]=E>>>24,I=g+i|0;!(I>>>0<=g>>>0|g>>>0>15);)gA(n,g),Z(A,A,C,n),g=g+1|0}function p(A,I,g,i){var C,E,r,a,f=0,D=0,y=0,F=0,w=0;for(c=E=c-160|0,a=E+8|0,r=u(200),n[a>>2]=r,r||(o(111),t()),c=C=c-208|0;;){if(25==(0|f)){for(;;){if(f=0,i>>>0<136){for(D=0;;){if(136==(0|f)){for(;(0|i)!=(0|D);)B[D+C|0]=Q[g+D|0],D=D+1|0;break}B[f+C|0]=0,f=f+1|0}for(B[i+C|0]=31,B[C+135|0]=128|Q[C+135|0],f=0;17!=(0|f);)D=n[(g=(i=f<<3)+r|0)>>2],y=n[g+4>>2],n[g>>2]=Y(i+C|0)^D,n[g+4>>2]=s^y,f=f+1|0;c=C+208|0;break}for(;17!=(0|f);)F=n[(D=(y=f<<3)+r|0)>>2],w=n[D+4>>2],n[D>>2]=Y(g+y|0)^F,n[D+4>>2]=s^w,f=f+1|0;h(r),g=g+136|0,i=i-136|0}break}n[(D=(f<<3)+r|0)>>2]=0,n[D+4>>2]=0,f=f+1|0}U(A,g=(I>>>0)/136|0,a);A:if((0|(g=e(g,136)))!=(0|I))for(i=I-g|0,A=A+g|0,U(E+16|0,1,E+8|0),I=0;;){if((0|I)==(0|i))break A;B[A+I|0]=Q[(E+16|0)+I|0],I=I+1|0}nA(E+8|0),c=E+160|0}function N(A,I,g){var B,i=0,C=0,E=0,r=0,e=0,t=0,o=0,a=0;for(B=n[A>>2];;){A:{if(E=0,A=0,i=e=n[B+204>>2],!(!(i=(r=g+(C=n[B+200>>2])|0)>>>0<g>>>0?i+1|0:i)&r>>>0<136)){for(;!A&E>>>0<(i=136-C|0)>>>0;)a=n[(i=(-8&(r=(i=C)+E|0))+B|0)+4>>2],e=Q[I+E|0],r=31&(C=r<<3),(63&C)>>>0>=32?(C=e<<r,e=0):(C=(1<<r)-1&e>>>32-r,e<<=r),n[i>>2]=e^n[i>>2],n[i+4>>2]=C^a,A=(C=E+1|0)?A:A+1|0,E=C,C=n[B+200>>2],e=n[B+204>>2];n[B+200>>2]=0,n[B+204>>2]=0,h(B),I=I+i|0,g=g-i|0;continue}for(;;){if(!t&(0|g)==(0|o))break A;E=A=o+n[B+200>>2]|0,r=n[(A=(-8&A)+B|0)+4>>2],i=Q[I+o|0],C=31&(E<<=3),(63&E)>>>0>=32?(E=i<<C,i=0):(E=(1<<C)-1&i>>>32-C,i<<=C),n[A>>2]=i^n[A>>2],n[A+4>>2]=r^E,A=t,o=t=o+1|0,t=A=t?A:A+1|0}}break}A=n[B+204>>2],A=(I=g+n[B+200>>2]|0)>>>0<g>>>0?A+1|0:A,n[B+200>>2]=I,n[B+204>>2]=A}function d(A,I,g){var i,C=0,Q=0,E=0,r=0,e=0,t=0,o=0;i=n[g>>2],r=I;A:{for(;;){if(C=n[i+200>>2],Q=n[i+204>>2],!e&(0|E)==(0|r)){g=I;break A}if(!((0|e)==(0|Q)&C>>>0>(g=E)>>>0|e>>>0<Q>>>0))break;o=A+g|0,t=n[(C=(-8&(Q=t=E+(136-C|0)|0))+i|0)>>2],C=n[C+4>>2],g=31&(Q<<=3),B[0|o]=(63&Q)>>>0>=32?C>>>g|0:((1<<g)-1&C)<<32-g|t>>>g,e=(g=E+1|0)?e:e+1|0,E=g}r=E}for(E=C-r|0,e=Q-((C>>>0<r>>>0)+0|0)|0,I=I-g|0,A=A+g|0;n[i+200>>2]=E,n[i+204>>2]=e,I;){for(h(i),E=I>>>0<136?I:136,g=0;(0|g)!=(0|E);)C=n[(r=(-8&g)+i|0)>>2],e=n[r+4>>2],r=31&(Q=g<<3&56),B[A+g|0]=(63&Q)>>>0>=32?e>>>r|0:((1<<r)-1&e)<<32-r|C>>>r,g=g+1|0;I=I-E|0,A=A+E|0,E=136-E|0,e=0}}function l(A,I,g){var B,i=0;c=B=c+-64|0,i=Q[I+28|0]|Q[I+29|0]<<8|Q[I+30|0]<<16|Q[I+31|0]<<24,n[B+24>>2]=Q[I+24|0]|Q[I+25|0]<<8|Q[I+26|0]<<16|Q[I+27|0]<<24,n[B+28>>2]=i,i=Q[I+20|0]|Q[I+21|0]<<8|Q[I+22|0]<<16|Q[I+23|0]<<24,n[B+16>>2]=Q[I+16|0]|Q[I+17|0]<<8|Q[I+18|0]<<16|Q[I+19|0]<<24,n[B+20>>2]=i,i=Q[I+4|0]|Q[I+5|0]<<8|Q[I+6|0]<<16|Q[I+7|0]<<24,n[B>>2]=Q[0|I]|Q[I+1|0]<<8|Q[I+2|0]<<16|Q[I+3|0]<<24,n[B+4>>2]=i,i=Q[I+12|0]|Q[I+13|0]<<8|Q[I+14|0]<<16|Q[I+15|0]<<24,n[B+8>>2]=Q[I+8|0]|Q[I+9|0]<<8|Q[I+10|0]<<16|Q[I+11|0]<<24,n[B+12>>2]=i,_(B+32|0,g),p(A,32,B,64),c=B- -64|0}function S(A,I,g){var B=0,i=0,C=0,n=0;return i=31&(C=n=63&g),C=C>>>0>=32?-1>>>i|0:(B=-1>>>i|0)|(1<<i)-1<<32-i,C&=A,B&=I,i=31&n,n>>>0>=32?(B=C<<i,n=0):(B=(1<<i)-1&C>>>32-i|B<<i,n=C<<i),C=B,B=31&(i=0-g&63),i>>>0>=32?(B=-1<<B,g=0):B=(g=-1<<B)|(1<<B)-1&-1>>>32-B,A&=g,I&=B,B=31&i,i>>>0>=32?(g=0,A=I>>>B|0):(g=I>>>B|0,A=((1<<B)-1&I)<<32-B|A>>>B),s=g|C,A|=n}function U(A,I,g){var i,C=0,Q=0,E=0,r=0,e=0,t=0,o=0;i=n[g>>2];A:for(;;){if(I)for(h(i),g=0;;){if(17==(0|g)){I=I-1|0,A=A+136|0;continue A}for(t=(C=g<<3)+A|0,o=n[(C=C+i|0)>>2],r=n[C+4>>2],Q=0,C=0;8!=(0|Q)|C;)E=31&(e=Q<<3),B[Q+t|0]=(63&e)>>>0>=32?r>>>E|0:((1<<E)-1&r)<<32-E|o>>>E,C=(Q=Q+1|0)?C:C+1|0;g=g+1|0}break}}function R(A){var I,g,B=0,i=0;I=n[A>>2],B=n[I+200>>2],g=n[(A=(-8&B)+I|0)+4>>2],i=31&(B<<=3),(63&B)>>>0>=32?(B=31<<i,i=0):(B=(1<<i)-1&31>>>32-i,i=31<<i),n[A>>2]=i^n[A>>2],n[A+4>>2]=B^g,A=-2147483648^n[I+132>>2],n[I+128>>2]=n[I+128>>2],n[I+132>>2]=A,n[I+200>>2]=0,n[I+204>>2]=0}function G(A,I){for(var g=0,B=0,i=0,C=0,n=0,E=0,r=0,e=0;;){if(!i&(0|I)==(0|C)){A=E;break}n=Q[A+C|0],g=31&(B=(-1^r)+I<<3),(63&B)>>>0>=32?(B=n<<g,g=0):(B=(1<<g)-1&n>>>32-g,g=n<<g),E|=g,e|=B,r=r+1|0,i=(g=C+1|0)?i:i+1|0,C=g}return s=e,A}function Y(A){for(var I=0,g=0,B=0,i=0,C=0,n=0,E=0;;){if(!i&8==(0|g)){A=n;break}C=Q[A+g|0],I=31&(B=g<<3),(63&B)>>>0>=32?(B=C<<I,I=0):(B=(1<<I)-1&C>>>32-I,I=C<<I),n|=I,E|=B,i=(g=g+1|0)?i:i+1|0}return s=E,A}function M(A,I,g,B,i,C,Q){var E,r;c=r=c+-64|0,J(E=r+8|0),N(E,B,32),N(E,i,64),N(E,C,Q),R(E),d(B=r+16|0,47,E),nA(E),K(A,B,39),n[I>>2]=G(r+55|0,7),n[I+4>>2]=16777215&s,n[g>>2]=255&G(r+62|0,1),c=r- -64|0}function b(A,I){for(var g=0,B=0,i=0,C=0,E=0;22!=(0|B);){for(i=0,n[(E=(B<<2)+A|0)>>2]=0,g=0;14!=(0|g);)i=(Q[(C>>>3|0)+I|0]>>>(7&C)&1)<<g^i,n[E>>2]=i,g=g+1|0,C=C+1|0;B=B+1|0}}function m(A,I){var g,B,i=0;for(L(A,64,I),B=A+256|0,I=0,c=g=c-16|0;64!=(0|I);)i=15+(i-n[(I<<2)+A>>2]|0)|0,I=I+1|0;x(A=g+14|0,2,i<<4),L(B,3,A),c=g+16|0}function J(A){var I=0,g=0;for(I=A,A=u(208),n[I>>2]=A,A||(o(111),t());25!=(0|g);)n[(I=A+(g<<3)|0)>>2]=0,n[I+4>>2]=0,g=g+1|0;n[A+200>>2]=0,n[A+204>>2]=0}function H(A){var I,B;return(A=(I=n[328])+(B=A+3&-4)|0)>>>0<=I>>>0&&B||A>>>0>(g.byteLength/65536|0)<<16>>>0&&!(0|f(0|A))?(n[519]=48,-1):(n[328]=A,I)}function L(A,I,g){for(var B=0,i=0,C=0,E=0;(0|I)!=(0|i);)B||(E=Q[g+C|0],C=C+1|0,B=8),B=B-4|0,n[(i<<2)+A>>2]=E>>>B&15,i=i+1|0}function v(A,I){for(var g=0,i=0,C=0;(0|I)!=(0|i);)c=g=c-16|0,B[g+15|0]=0,C=0|a(1316,g+15|0,0),c=g+16|0,B[A+i|0]=C,i=i+1|0}function q(A,I){n[A>>2]=n[I>>2],n[A+4>>2]=n[I+4>>2],n[A+8>>2]=n[I+8>>2],n[A+12>>2]=n[I+12>>2],n[A+20>>2]=n[I+20>>2]}function K(A,I,g){var i=0;if(g)for(i=A;B[0|i]=Q[0|I],i=i+1|0,I=I+1|0,g=g-1|0;);return A}function x(A,I,g){for(var i=0;I;)B[(I=I-1|0)+A|0]=g,g=(255&i)<<24|g>>>8,i=i>>>8|0}function _(A,I){for(var g=0,B=0;8!=(0|g);)x((B=g<<2)+A|0,4,n[I+B>>2]),g=g+1|0}function j(A,I){n[A>>2]=n[I>>2],n[A+4>>2]=n[I+4>>2],n[A+8>>2]=n[I+8>>2],n[A+12>>2]=n[I+12>>2]}function W(A,I,g,B,i,C,n){var Q;c=Q=c-336|0,D(A,I,Q+48|0,Q,g,B,i,0,8,2,C,n),c=Q+336|0}function X(A){var I;return(-1>>>(I=31&A)&-2)<<I|(-1<<(A=0-A&31)&-2)>>>A}function T(A,I,g,B){var i;c=i=c-2208|0,w(A,i,I,67,g,B),c=i+2208|0}function O(A,I,g,B){var i;c=i=c-768|0,w(A,i,I,22,g,B),c=i+768|0}function V(A,I,g,B){var i;c=i=c-128|0,w(A,i,I,2,g,B),c=i+128|0}function Z(A,I,g,B){var i;c=i=c-96|0,w(A,i,I,1,g,B),c=i+96|0}function P(A,I,g){n[A+12>>2]=I,n[A+4>>2]=0,n[A+8>>2]=g}function z(A,I,g){gA(g,0),l(A,I,g)}function $(A,I,g,B){Z(A,I,g,B)}function AA(A,I){n[A+20>>2]=I}function IA(A,I){n[A+24>>2]=I}function gA(A,I){n[A+28>>2]=I}function BA(A,I){n[A+16>>2]=I}function iA(A,I,g){l(A,I,g)}function CA(A,I){n[A>>2]=I}function nA(A){F(n[A>>2])}i(I=Q,1024,"cmFuZG9tYnl0ZXMAbGlic29kaXVtL3NyYy9saWJzb2RpdW0vcmFuZG9tYnl0ZXMvcmFuZG9tYnl0ZXMuYwBidWZfbGVuIDw9IFNJWkVfTUFY"),i(I,1120,"AQAAAAAAAACCgAAAAAAAAIqAAAAAAACAAIAAgAAAAICLgAAAAAAAAAEAAIAAAAAAgYAAgAAAAIAJgAAAAAAAgIoAAAAAAAAAiAAAAAAAAAAJgACAAAAAAAoAAIAAAAAAi4AAgAAAAACLAAAAAAAAgImAAAAAAACAA4AAAAAAAIACgAAAAAAAgIAAAAAAAACACoAAAAAAAAAKAACAAAAAgIGAAIAAAACAgIAAAAAAAIABAACAAAAAAAiAAIAAAACA"),i(I,1312,"EAqA");var QA,EA=(QA=[null,function(A,I,g,B,i,C){A|=0,I|=0,g|=0,B|=0,i|=0,C|=0,c=C=c-32|0,n[C+24>>2]=0,n[C+28>>2]=0,n[C+16>>2]=0,n[C+20>>2]=0,n[C+8>>2]=0,n[C+12>>2]=0,n[C>>2]=0,n[C+4>>2]=0,q(C,i),BA(C,3),gA(C,B),iA(A,I,C),$(A,A,g,C),c=C+32|0},function(A,I,g,B,i,C){var Q,E;for(A|=0,I|=0,g|=0,B|=0,i|=0,C|=0,c=Q=c-2208|0,n[Q+56>>2]=0,n[Q+60>>2]=0,n[Q+48>>2]=0,n[Q+52>>2]=0,n[Q+40>>2]=0,n[Q+44>>2]=0,n[Q+32>>2]=0,n[Q+36>>2]=0,n[Q+24>>2]=0,n[Q+28>>2]=0,n[Q+16>>2]=0,n[Q+20>>2]=0,n[Q+8>>2]=0,n[Q+12>>2]=0,n[Q>>2]=0,n[Q+4>>2]=0,BA(C=Q+32|0,0),BA(Q,1),j(C,i),AA(C,B),E=Q- -64|0,B=0;67!=(0|B);)IA(C,B),z(i=(B<<5)+E|0,I,C),k(i,i,0,15,g,C),B=B+1|0;q(Q,C),T(A,E,g,Q),c=Q+2208|0}],QA.set=function(A,I){this[A]=I},QA.get=function(A){return this[A]},QA);return{f:function(){},g:EA,h:u,i:F,j:function(){var A;c=A=c-16|0,B[A+15|0]=0,a(1352,A+15|0,0),c=A+16|0},k:function(){return 64},l:function(){return 128},m:function(){return 29794},n:function(A,I){A|=0,I|=0;var g,i,C,E=0,r=0;return c=i=c-96|0,v(i,96),c=g=c-304|0,n[g+40>>2]=0,n[g+44>>2]=0,n[g+32>>2]=0,n[g+36>>2]=0,n[g+24>>2]=0,n[g+28>>2]=0,n[g+16>>2]=0,n[g+20>>2]=0,CA(C=g+16|0,7),BA(C,2),I=K(I,i,96),E=Q[I+92|0]|Q[I+93|0]<<8|Q[I+94|0]<<16|Q[I+95|0]<<24,r=Q[I+88|0]|Q[I+89|0]<<8|Q[I+90|0]<<16|Q[I+91|0]<<24,B[A+24|0]=r,B[A+25|0]=r>>>8,B[A+26|0]=r>>>16,B[A+27|0]=r>>>24,B[A+28|0]=E,B[A+29|0]=E>>>8,B[A+30|0]=E>>>16,B[A+31|0]=E>>>24,E=Q[I+84|0]|Q[I+85|0]<<8|Q[I+86|0]<<16|Q[I+87|0]<<24,r=Q[I+80|0]|Q[I+81|0]<<8|Q[I+82|0]<<16|Q[I+83|0]<<24,B[A+16|0]=r,B[A+17|0]=r>>>8,B[A+18|0]=r>>>16,B[A+19|0]=r>>>24,B[A+20|0]=E,B[A+21|0]=E>>>8,B[A+22|0]=E>>>16,B[A+23|0]=E>>>24,E=Q[I+76|0]|Q[I+77|0]<<8|Q[I+78|0]<<16|Q[I+79|0]<<24,r=Q[I+72|0]|Q[I+73|0]<<8|Q[I+74|0]<<16|Q[I+75|0]<<24,B[A+8|0]=r,B[A+9|0]=r>>>8,B[A+10|0]=r>>>16,B[A+11|0]=r>>>24,B[A+12|0]=E,B[A+13|0]=E>>>8,B[A+14|0]=E>>>16,B[A+15|0]=E>>>24,E=Q[I+68|0]|Q[I+69|0]<<8|Q[I+70|0]<<16|Q[I+71|0]<<24,r=Q[I+64|0]|Q[I+65|0]<<8|Q[I+66|0]<<16|Q[I+67|0]<<24,B[0|A]=r,B[A+1|0]=r>>>8,B[A+2|0]=r>>>16,B[A+3|0]=r>>>24,B[A+4|0]=E,B[A+5|0]=E>>>8,B[A+6|0]=E>>>16,B[A+7|0]=E>>>24,W(I+96|0,g+48|0,I,I- -64|0,0,C,g+12|0),E=Q[I+124|0]|Q[I+125|0]<<8|Q[I+126|0]<<16|Q[I+127|0]<<24,r=Q[I+120|0]|Q[I+121|0]<<8|Q[I+122|0]<<16|Q[I+123|0]<<24,B[A+56|0]=r,B[A+57|0]=r>>>8,B[A+58|0]=r>>>16,B[A+59|0]=r>>>24,B[A+60|0]=E,B[A+61|0]=E>>>8,B[A+62|0]=E>>>16,B[A+63|0]=E>>>24,E=Q[I+116|0]|Q[I+117|0]<<8|Q[I+118|0]<<16|Q[I+119|0]<<24,r=Q[I+112|0]|Q[I+113|0]<<8|Q[I+114|0]<<16|Q[I+115|0]<<24,B[A+48|0]=r,B[A+49|0]=r>>>8,B[A+50|0]=r>>>16,B[A+51|0]=r>>>24,B[A+52|0]=E,B[A+53|0]=E>>>8,B[A+54|0]=E>>>16,B[A+55|0]=E>>>24,E=Q[I+108|0]|Q[I+109|0]<<8|Q[I+110|0]<<16|Q[I+111|0]<<24,r=Q[I+104|0]|Q[I+105|0]<<8|Q[I+106|0]<<16|Q[I+107|0]<<24,B[A+40|0]=r,B[A+41|0]=r>>>8,B[A+42|0]=r>>>16,B[A+43|0]=r>>>24,B[A+44|0]=E,B[A+45|0]=E>>>8,B[A+46|0]=E>>>16,B[A+47|0]=E>>>24,E=Q[I+100|0]|Q[I+101|0]<<8|Q[I+102|0]<<16|Q[I+103|0]<<24,I=Q[I+96|0]|Q[I+97|0]<<8|Q[I+98|0]<<16|Q[I+99|0]<<24,B[A+32|0]=I,B[A+33|0]=I>>>8,B[A+34|0]=I>>>16,B[A+35|0]=I>>>24,B[A+36|0]=E,B[A+37|0]=E>>>8,B[A+38|0]=E>>>16,B[A+39|0]=E>>>24,c=g+304|0,c=i+96|0,0},o:function(A,I,g,B){A|=0,I|=0,g|=0,B|=0;var i,Q,E=0,r=0,e=0,t=0,o=0,a=0,f=0,s=0,u=0;for(c=i=(c=Q=c-16|0)-208|0,n[i+72>>2]=0,n[i+76>>2]=0,n[(r=i- -64|0)>>2]=0,n[r+4>>2]=0,n[i+56>>2]=0,n[i+60>>2]=0,n[i+48>>2]=0,n[i+52>>2]=0,n[i+40>>2]=0,n[i+44>>2]=0,n[i+32>>2]=0,n[i+36>>2]=0,n[i+24>>2]=0,n[i+28>>2]=0,n[i+16>>2]=0,n[i+20>>2]=0,BA(e=i+48|0,0),BA(i+16|0,2),v(t=i+176|0,32),c=E=c-16|0,J(r=E+8|0),N(r,B+32|0,32),N(r,t,32),N(r,I,g),R(r),d(t=A+2|0,32,r),nA(r),c=E+16|0,M(o=i+128|0,i+88|0,i+84|0,t,r=B- -64|0,I,g),P(e,n[i+88>>2],n[i+92>>2]),AA(e,n[i+84>>2]),E=t+32|0,a=i+96|0,s=i+12|0,c=I=c-864|0,n[I+56>>2]=0,n[I+60>>2]=0,n[I+48>>2]=0,n[I+52>>2]=0,n[I+40>>2]=0,n[I+44>>2]=0,n[I+32>>2]=0,n[I+36>>2]=0,n[I+24>>2]=0,n[I+28>>2]=0,n[I+16>>2]=0,n[I+20>>2]=0,n[I+8>>2]=0,n[I+12>>2]=0,n[I>>2]=0,n[I+4>>2]=0,q(g=I+32|0,e),q(I,e),BA(g,3),BA(I,4),b(I+768|0,o),g=0;22!=(0|g);)IA(e=I+32|0,0),gA(e,(f=g<<14)+(u=n[(I+768|0)+(g<<2)>>2])|0),iA(E,B,e),c=o=c-544|0,D((I- -64|0)+(g<<5)|0,E+32|0,o- -64|0,o,B,r,u,f,14,1,e,s),c=o+544|0,g=g+1|0,E=E+480|0;for(O(a,I- -64|0,r,I),c=I+864|0,g=t+10592|0,e=0;8!=(0|e);){for(CA(t=i+16|0,e),P(t,n[i+88>>2],n[i+92>>2]),j(E=i+48|0,t),AA(E,n[i+84>>2]),s=i+12|0,c=o=c-272|0,m(o,f=i+96|0),I=0;67!=(0|I);)IA(E,I),z(a=(I<<5)+g|0,B,E),k(a,a,0,n[o+(I<<2)>>2],r,E),I=I+1|0;c=o+272|0,W(f,g+2144|0,B,r,n[i+84>>2],t,s),t=(I=n[i+92>>2])>>>8|0,E=n[i+88>>2],n[i+88>>2]=(255&I)<<24|E>>>8,n[i+92>>2]=t,n[i+84>>2]=255&E,g=g+2400|0,e=e+1|0}return n[Q+12>>2]=29792,c=i+208|0,C[A>>1]=n[Q+12>>2],c=Q+16|0,0},p:function(A,I,g,B){A|=0,I|=0,B|=0;var i=0,C=0,r=0,e=0,t=0,o=0,a=0,f=0,s=0;if(r=(g|=0)+2|0,t=E[g>>1],c=g=c-2384|0,n[g+104>>2]=0,n[g+108>>2]=0,n[g+96>>2]=0,n[g+100>>2]=0,n[g+88>>2]=0,n[g+92>>2]=0,n[g+80>>2]=0,n[g+84>>2]=0,n[g+72>>2]=0,n[g+76>>2]=0,n[(C=g- -64|0)>>2]=0,n[C+4>>2]=0,n[g+56>>2]=0,n[g+60>>2]=0,n[g+48>>2]=0,n[g+52>>2]=0,n[g+40>>2]=0,n[g+44>>2]=0,n[g+32>>2]=0,n[g+36>>2]=0,n[g+24>>2]=0,n[g+28>>2]=0,n[g+16>>2]=0,n[g+20>>2]=0,C=-1,29792==(0|t)){for(t=B+32|0,BA(i=g+80|0,0),BA(g+48|0,2),BA(g+16|0,1),M(e=g+2336|0,g+120|0,g+116|0,r,B,A,I),P(i,n[g+120>>2],n[g+124>>2]),AA(i,n[g+116>>2]),s=g+160|0,C=r+32|0,c=A=c-896|0,n[A+56>>2]=0,n[A+60>>2]=0,n[A+48>>2]=0,n[A+52>>2]=0,n[A+40>>2]=0,n[A+44>>2]=0,n[A+32>>2]=0,n[A+36>>2]=0,n[A+24>>2]=0,n[A+28>>2]=0,n[A+16>>2]=0,n[A+20>>2]=0,n[A+8>>2]=0,n[A+12>>2]=0,n[A>>2]=0,n[A+4>>2]=0,q(I=A+32|0,i),q(A,i),BA(I,3),BA(A,4),b(A+800|0,e),I=0;22!=(0|I);)IA(i=A+32|0,0),gA(i,(e=n[(A+800|0)+(I<<2)>>2])+(o=I<<14)|0),$(a=A- -64|0,C,B,i),y((A+96|0)+(I<<5)|0,a,e,o,C+32|0,14,B,i),I=I+1|0,C=C+480|0;for(O(s,A+96|0,B,A),c=A+896|0,C=r+10592|0,A=0;8!=(0|A);){for(CA(r=g+48|0,A),P(r,n[g+120>>2],n[g+124>>2]),j(i=g+80|0,r),AA(i,n[g+116>>2]),q(o=g+16|0,i),s=g+192|0,c=e=c-272|0,m(e,a=g+160|0),I=0;67!=(0|I);)IA(i,I),k((f=I<<5)+s|0,C+f|0,f=n[e+(I<<2)>>2],15-f|0,B,i),I=I+1|0;c=e+272|0,T(I=g+128|0,s,B,o),y(a,I,n[g+116>>2],0,C+2144|0,8,B,r),r=(I=n[g+124>>2])>>>8|0,i=n[g+120>>2],n[g+120>>2]=(255&I)<<24|i>>>8,n[g+124>>2]=r,n[g+116>>2]=255&i,C=C+2400|0,A=A+1|0}B=g+160|0,A=t,I=0,C=32;A:{for(;;){if((0|(t=Q[0|B]))==(0|(i=Q[0|A]))){if(A=A+1|0,B=B+1|0,C=C-1|0)continue;break A}break}I=t-i|0}C=I?-1:0}return c=g+2384|0,0|C}}}(A)}(Z)},instantiate:function(A,I){return{then:function(I){var g=new D.Module(A);I({instance:new D.Instance(g)})}}},RuntimeError:Error};c=[],"object"!=A(D)&&H("no native wasm support detected");var y=!1;var F,w,k,p,N,d="undefined"!=typeof TextDecoder?new TextDecoder("utf8"):void 0;function l(A,I){return A?function(A,I,g){for(var B=I+g,i=I;A[i]&&!(i>=B);)++i;if(i-I>16&&A.buffer&&d)return d.decode(A.subarray(I,i));for(var C="";I<i;){var n=A[I++];if(128&n){var Q=63&A[I++];if(192!=(224&n)){var E=63&A[I++];if((n=224==(240&n)?(15&n)<<12|Q<<6|E:(7&n)<<18|Q<<12|E<<6|63&A[I++])<65536)C+=String.fromCharCode(n);else{var r=n-65536;C+=String.fromCharCode(55296|r>>10,56320|1023&r)}}else C+=String.fromCharCode((31&n)<<6|Q)}else C+=String.fromCharCode(n)}return C}(k,A,I):""}var S,U=Q.INITIAL_MEMORY||16777216;(u=Q.wasmMemory?Q.wasmMemory:new D.Memory({initial:U/65536,maximum:U/65536}))&&(F=u.buffer),U=F.byteLength,F=S=F,Q.HEAP8=w=new Int8Array(S),Q.HEAP16=new Int16Array(S),Q.HEAP32=p=new Int32Array(S),Q.HEAPU8=k=new Uint8Array(S),Q.HEAPU16=new Uint16Array(S),Q.HEAPU32=new Uint32Array(S),Q.HEAPF32=new Float32Array(S),Q.HEAPF64=N=new Float64Array(S);var R=[],G=[],Y=[];function M(){return h}var b=0,m=null,J=null;function H(A){throw Q.onAbort&&Q.onAbort(A),s(A="Aborted("+A+")"),y=!0,1,A+=". Build with -sASSERTIONS for more info.",new D.RuntimeError(A)}var L,v,q="data:application/octet-stream;base64,";function K(A){return A.startsWith(q)}function x(A){return A.startsWith("file://")}function _(A){try{if(A==L&&c)return new Uint8Array(c);var I=O(A);if(I)return I;if(B)return B(A);throw"both async and sync fetching of the wasm failed"}catch(A){H(A)}}K(L="<<< WASM_BINARY_FILE >>>")||(v=L,L=Q.locateFile?Q.locateFile(v,f):f+v);var j={1316:function(){return Q.getRandomValue()},1352:function(){if(void 0===Q.getRandomValue)try{var I="object"===("undefined"==typeof window?"undefined":A(window))?window:self,g=void 0!==I.crypto?I.crypto:I.msCrypto,B=function(){var A=new Uint32Array(1);return g.getRandomValues(A),A[0]>>>0};B(),Q.getRandomValue=B}catch(A){try{var i=eval("require")("crypto"),C=function(){var A=i.randomBytes(4);return(A[0]<<24|A[1]<<16|A[2]<<8|A[3])>>>0};C(),Q.getRandomValue=C}catch(A){throw"No secure random number generator found"}}}};function W(A){for(;A.length>0;)A.shift()(Q)}var X=[];var T="function"==typeof atob?atob:function(A){var I,g,B,i,C,n,Q="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=",E="",r=0;A=A.replace(/[^A-Za-z0-9\+\/\=]/g,"");do{I=Q.indexOf(A.charAt(r++))<<2|(i=Q.indexOf(A.charAt(r++)))>>4,g=(15&i)<<4|(C=Q.indexOf(A.charAt(r++)))>>2,B=(3&C)<<6|(n=Q.indexOf(A.charAt(r++))),E+=String.fromCharCode(I),64!==C&&(E+=String.fromCharCode(g)),64!==n&&(E+=String.fromCharCode(B))}while(r<A.length);return E};function O(A){if(K(A))return function(A){if("boolean"==typeof a&&a){var I=Buffer.from(A,"base64");return new Uint8Array(I.buffer,I.byteOffset,I.byteLength)}try{for(var g=T(A),B=new Uint8Array(g.length),i=0;i<g.length;++i)B[i]=g.charCodeAt(i);return B}catch(A){throw new Error("Converting base64 string to bytes failed.")}}(A.slice(q.length))}var V,Z={e:function(A,I,g,B){H("Assertion failed: "+l(A)+", at: "+[I?l(I):"unknown filename",g,B?l(B):"unknown function"])},c:function(A,I,g){var B=function(A,I){var g;for(X.length=0,I>>=2;g=k[A++];)I+=105!=g&I,X.push(105==g?p[I]:N[I++>>1]),++I;return X}(I,g);return j[A].apply(null,B)},d:function(A){k.length,H("OOM")},b:function(A){!function(A,I){A,function(A){A,M()||(Q.onExit&&Q.onExit(A),y=!0);e(A,new P(A))}(A)}(A)},a:u};(function(){var A={a:Z};function I(A,I){var g,B=A.exports;Q.asm=B,Q.asm.g,g=Q.asm.f,G.unshift(g),function(A){if(b--,Q.monitorRunDependencies&&Q.monitorRunDependencies(b),0==b&&(null!==m&&(clearInterval(m),m=null),J)){var I=J;J=null,I()}}()}function B(A){I(A.instance)}function i(I){return function(){if(!c&&(t||o)){if("function"==typeof fetch&&!x(L))return fetch(L,{credentials:"same-origin"}).then((function(A){if(!A.ok)throw"failed to load wasm binary file at '"+L+"'";return A.arrayBuffer()})).catch((function(){return _(L)}));if(g)return new Promise((function(A,I){g(L,(function(I){A(new Uint8Array(I))}),I)}))}return Promise.resolve().then((function(){return _(L)}))}().then((function(I){return D.instantiate(I,A)})).then((function(A){return A})).then(I,(function(A){s("failed to asynchronously prepare wasm: "+A),H(A)}))}if(b++,Q.monitorRunDependencies&&Q.monitorRunDependencies(b),Q.instantiateWasm)try{return Q.instantiateWasm(A,I)}catch(A){return s("Module.instantiateWasm callback failed with error: "+A),!1}c||"function"!=typeof D.instantiateStreaming||K(L)||x(L)||a||"function"!=typeof fetch?i(B):fetch(L,{credentials:"same-origin"}).then((function(I){return D.instantiateStreaming(I,A).then(B,(function(A){return s("wasm streaming compile failed: "+A),s("falling back to ArrayBuffer instantiation"),i(B)}))}))})(),Q.___wasm_call_ctors=function(){return(Q.___wasm_call_ctors=Q.asm.f).apply(null,arguments)},Q._malloc=function(){return(Q._malloc=Q.asm.h).apply(null,arguments)},Q._free=function(){return(Q._free=Q.asm.i).apply(null,arguments)},Q._sphincsjs_init=function(){return(Q._sphincsjs_init=Q.asm.j).apply(null,arguments)},Q._sphincsjs_public_key_bytes=function(){return(Q._sphincsjs_public_key_bytes=Q.asm.k).apply(null,arguments)},Q._sphincsjs_secret_key_bytes=function(){return(Q._sphincsjs_secret_key_bytes=Q.asm.l).apply(null,arguments)},Q._sphincsjs_signature_bytes=function(){return(Q._sphincsjs_signature_bytes=Q.asm.m).apply(null,arguments)},Q._sphincsjs_keypair=function(){return(Q._sphincsjs_keypair=Q.asm.n).apply(null,arguments)},Q._sphincsjs_sign=function(){return(Q._sphincsjs_sign=Q.asm.o).apply(null,arguments)},Q._sphincsjs_verify=function(){return(Q._sphincsjs_verify=Q.asm.p).apply(null,arguments)};function P(A){this.name="ExitStatus",this.message="Program terminated with exit("+A+")",this.status=A}function z(A){function I(){V||(V=!0,Q.calledRun=!0,y||(!0,W(G),Q.onRuntimeInitialized&&Q.onRuntimeInitialized(),function(){if(Q.postRun)for("function"==typeof Q.postRun&&(Q.postRun=[Q.postRun]);Q.postRun.length;)A=Q.postRun.shift(),Y.unshift(A);var A;W(Y)}()))}A=A||r,b>0||(!function(){if(Q.preRun)for("function"==typeof Q.preRun&&(Q.preRun=[Q.preRun]);Q.preRun.length;)A=Q.preRun.shift(),R.unshift(A);var A;W(R)}(),b>0||(Q.setStatus?(Q.setStatus("Running..."),setTimeout((function(){setTimeout((function(){Q.setStatus("")}),1),I()}),1)):I()))}if(Q.writeArrayToMemory=function(A,I){w.set(A,I)},J=function A(){V||z(),V||(J=A)},Q.run=z,Q.preInit)for("function"==typeof Q.preInit&&(Q.preInit=[Q.preInit]);Q.preInit.length>0;)Q.preInit.pop()();return z(),new Promise((function(A,I){Q.onAbort=I,Q.onRuntimeInitialized=function(){A(Q)}}))})).then((function(A){Object.keys(A).forEach((function(I){B[I]=A[I]}))}));var Q=B.ready.then((function(){B._sphincsjs_init(),A=B._sphincsjs_public_key_bytes(),I=B._sphincsjs_secret_key_bytes(),g=B._sphincsjs_signature_bytes()})),E={publicKeyBytes:Q.then((function(){return A})),privateKeyBytes:Q.then((function(){return I})),bytes:Q.then((function(){return g})),keyPair:function(){return Q.then((function(){var g=B._malloc(A),Q=B._malloc(I);try{return i(B._sphincsjs_keypair(g,Q),{publicKey:C(g,A),privateKey:C(Q,I)})}finally{n(g),n(Q)}}))},sign:function(A,I){return E.signDetached(A,I).then((function(I){var B=new Uint8Array(g+A.length);return B.set(I),B.set(A,g),B}))},signDetached:function(A,E){return Q.then((function(){var Q=B._malloc(g),r=B._malloc(A.length),e=B._malloc(I);B.writeArrayToMemory(new Uint8Array(g),Q),B.writeArrayToMemory(A,r),B.writeArrayToMemory(E,e);try{return i(B._sphincsjs_sign(Q,r,A.length,e),C(Q,g))}finally{n(Q),n(r),n(e)}}))},open:function(A,I){return Q.then((function(){var B=new Uint8Array(A.buffer,A.byteOffset,g),i=new Uint8Array(A.buffer,A.byteOffset+g);return E.verifyDetached(B,i,I).then((function(A){if(A)return i;throw new Error("Failed to open SPHINCS+ signed message.")}))}))},verifyDetached:function(I,i,C){return Q.then((function(){var Q=B._malloc(g),E=B._malloc(i.length),r=B._malloc(A);B.writeArrayToMemory(I,Q),B.writeArrayToMemory(i,E),B.writeArrayToMemory(C,r);try{return 0===B._sphincsjs_verify(E,i.length,Q,r)}finally{n(Q),n(E),n(r)}}))}};return E}(); true&&module.exports?(sphincs.sphincs=sphincs,module.exports=sphincs):self.sphincs=sphincs;

/***/ })
/******/ 	]);
/************************************************************************/
/******/ 	// The module cache
/******/ 	var __webpack_module_cache__ = {};
/******/ 	
/******/ 	// The require function
/******/ 	function __webpack_require__(moduleId) {
/******/ 		// Check if module is in cache
/******/ 		var cachedModule = __webpack_module_cache__[moduleId];
/******/ 		if (cachedModule !== undefined) {
/******/ 			return cachedModule.exports;
/******/ 		}
/******/ 		// Create a new module (and put it into the cache)
/******/ 		var module = __webpack_module_cache__[moduleId] = {
/******/ 			id: moduleId,
/******/ 			loaded: false,
/******/ 			exports: {}
/******/ 		};
/******/ 	
/******/ 		// Execute the module function
/******/ 		__webpack_modules__[moduleId].call(module.exports, module, module.exports, __webpack_require__);
/******/ 	
/******/ 		// Flag the module as loaded
/******/ 		module.loaded = true;
/******/ 	
/******/ 		// Return the exports of the module
/******/ 		return module.exports;
/******/ 	}
/******/ 	
/************************************************************************/
/******/ 	/* webpack/runtime/node module decorator */
/******/ 	!function() {
/******/ 		__webpack_require__.nmd = function(module) {
/******/ 			module.paths = [];
/******/ 			if (!module.children) module.children = [];
/******/ 			return module;
/******/ 		};
/******/ 	}();
/******/ 	
/************************************************************************/
/******/ 	
/******/ 	// startup
/******/ 	// Load entry module and return exports
/******/ 	// This entry module is referenced by other modules so it can't be inlined
/******/ 	var __webpack_exports__ = __webpack_require__(0);
/******/ 	superSphincs = __webpack_exports__;
/******/ 	
/******/ })()
;