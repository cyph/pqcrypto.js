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
					classical: null,
					combined: null,
					postQuantum: null
				};
			}

			var rsaPrivateKey			= results[0];
			var sphincsPrivateKey		= results[1];
			var superSphincsPrivateKey	= results[2];

			var privateKeyData	= {
				classical: sodiumUtil.to_base64(rsaPrivateKey),
				combined: sodiumUtil.to_base64(superSphincsPrivateKey),
				postQuantum: sodiumUtil.to_base64(sphincsPrivateKey)
			};

			sodiumUtil.memzero(superSphincsPrivateKey);
			sodiumUtil.memzero(sphincsPrivateKey);
			sodiumUtil.memzero(rsaPrivateKey);

			return privateKeyData;
		}).then(function (privateKeyData) {
			return {
				private: privateKeyData,
				public: {
					classical: sodiumUtil.to_base64(new Uint8Array(
						keyPair.publicKey.buffer,
						keyPair.publicKey.byteOffset,
						rsaSign.publicKeyBytes
					)),
					combined: sodiumUtil.to_base64(keyPair.publicKey),
					postQuantum: sodiumUtil.to_base64(new Uint8Array(
						keyPair.publicKey.buffer,
						keyPair.publicKey.byteOffset + rsaSign.publicKeyBytes
					))
				}
			};
		});
	},

	importKeys: function (keyData, password) {
		return initiated.then(function () {
			if (keyData.private && typeof keyData.private.combined === 'string') {
				var superSphincsPrivateKey	= sodiumUtil.from_base64(keyData.private.combined);

				if (password != null && password.length > 0) {
					return Promise.all([decrypt(superSphincsPrivateKey, password)]);
				}
				else {
					return [superSphincsPrivateKey];
				}
			}
			else if (
				keyData.private &&
				typeof keyData.private.classical === 'string' &&
				typeof keyData.private.postQuantum === 'string'
			) {
				var rsaPrivateKey		= sodiumUtil.from_base64(keyData.private.classical);
				var sphincsPrivateKey	= sodiumUtil.from_base64(keyData.private.postQuantum);

				if (password == null || password.length < 1) {
					return [rsaPrivateKey, sphincsPrivateKey];
				}

				return Promise.all([
					decrypt(
						rsaPrivateKey,
						typeof password === 'string' ? password : password.classical
					),
					decrypt(
						sphincsPrivateKey,
						typeof password === 'string' ? password : password.postQuantum
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
				if (keyData.public.combined) {
					keyPair.publicKey.set(sodiumUtil.from_base64(keyData.public.combined));
				}
				else if (keyData.public.classical && keyData.public.postQuantum) {
					keyPair.publicKey.set(sodiumUtil.from_base64(keyData.public.classical));
					keyPair.publicKey.set(
						sodiumUtil.from_base64(keyData.public.postQuantum),
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
var Module={},isNode="object"==typeof process&&"function"=="function"&&"object"!=typeof window&&"function"!=typeof importScripts,pemJwk=__webpack_require__(5),sodiumUtil=__webpack_require__(28),initiated,nodeCrypto,generateRSAKeypair;if(isNode)initiated=Promise.resolve(),nodeCrypto=eval("require")("crypto"),generateRSAKeypair=eval("require")("generate-rsa-keypair");else{function _typeof(A){return _typeof="function"==typeof Symbol&&"symbol"==typeof Symbol.iterator?function(A){return typeof A}:function(A){return A&&"function"==typeof Symbol&&A.constructor===Symbol&&A!==Symbol.prototype?"symbol":typeof A},_typeof(A)}initiated=new Promise((function(A,r){Module.onAbort=r,Module.onRuntimeInitialized=function(){try{Module._rsasignjs_init(),A()}catch(A){r(A)}}}));var Module=void 0!==Module?Module:{},moduleOverrides=Object.assign({},Module),arguments_=[],thisProgram="./this.program",quit_=function(A,r){throw r},ENVIRONMENT_IS_WEB="object"==("undefined"==typeof window?"undefined":_typeof(window)),ENVIRONMENT_IS_WORKER="function"==typeof importScripts,ENVIRONMENT_IS_NODE="object"==("undefined"==typeof process?"undefined":_typeof(process))&&"object"==_typeof(process.versions)&&"string"==typeof process.versions.node,scriptDirectory="",read_,readAsync,readBinary,setWindowTitle,fs,nodePath,requireNodeFS;function locateFile(A){return Module.locateFile?Module.locateFile(A,scriptDirectory):scriptDirectory+A}function logExceptionOnExit(A){A instanceof ExitStatus||err("exiting due to exception: "+A)}ENVIRONMENT_IS_NODE?(scriptDirectory=ENVIRONMENT_IS_WORKER?eval("require")("path").dirname(scriptDirectory)+"/":__dirname+"/",requireNodeFS=function requireNodeFS(){nodePath||(fs=eval("require")("fs"),nodePath=eval("require")("path"))},read_=function(A,r){var e=tryParseAsDataURI(A);return e?r?e:e.toString():(requireNodeFS(),A=nodePath.normalize(A),fs.readFileSync(A,r?void 0:"utf8"))},readBinary=function(A){var r=read_(A,!0);return r.buffer||(r=new Uint8Array(r)),r},readAsync=function(A,r,e){var i=tryParseAsDataURI(A);i&&r(i),requireNodeFS(),A=nodePath.normalize(A),fs.readFile(A,(function(A,i){A?e(A):r(i.buffer)}))},process.argv.length>1&&(thisProgram=process.argv[1].replace(/\\/g,"/")),arguments_=process.argv.slice(2), true&&(module.exports=Module),process.on("uncaughtException",(function(A){if(!(A instanceof ExitStatus))throw A})),process.on("unhandledRejection",(function(A){throw A})),quit_=function(A,r){if(keepRuntimeAlive())throw process.exitCode=A,r;logExceptionOnExit(r),process.exit(A)},Module.inspect=function(){return"[Emscripten Module object]"}):(ENVIRONMENT_IS_WEB||ENVIRONMENT_IS_WORKER)&&(ENVIRONMENT_IS_WORKER?scriptDirectory=self.location.href:"undefined"!=typeof document&&document.currentScript&&(scriptDirectory=document.currentScript.src),scriptDirectory=0!==scriptDirectory.indexOf("blob:")?scriptDirectory.substr(0,scriptDirectory.replace(/[?#].*/,"").lastIndexOf("/")+1):"",read_=function(A){try{var r=new XMLHttpRequest;return r.open("GET",A,!1),r.send(null),r.responseText}catch(r){var e=tryParseAsDataURI(A);if(e)return intArrayToString(e);throw r}},ENVIRONMENT_IS_WORKER&&(readBinary=function(A){try{var r=new XMLHttpRequest;return r.open("GET",A,!1),r.responseType="arraybuffer",r.send(null),new Uint8Array(r.response)}catch(r){var e=tryParseAsDataURI(A);if(e)return e;throw r}}),readAsync=function(A,r,e){var i=new XMLHttpRequest;i.open("GET",A,!0),i.responseType="arraybuffer",i.onload=function(){if(200==i.status||0==i.status&&i.response)r(i.response);else{var Q=tryParseAsDataURI(A);Q?r(Q.buffer):e()}},i.onerror=e,i.send(null)},setWindowTitle=function(A){return document.title=A});var out=Module.print||console.log.bind(console),err=Module.printErr||console.warn.bind(console),wasmBinary;Object.assign(Module,moduleOverrides),moduleOverrides=null,Module.arguments&&(arguments_=Module.arguments),Module.thisProgram&&(thisProgram=Module.thisProgram),Module.quit&&(quit_=Module.quit),Module.wasmBinary&&(wasmBinary=Module.wasmBinary);var noExitRuntime=Module.noExitRuntime||!0,WebAssembly={Memory:function(A){this.buffer=new ArrayBuffer(65536*A.initial)},Module:function(A){},Instance:function(A,r){this.exports=function(A){for(var r,e=new Uint8Array(123),i=25;i>=0;--i)e[48+i]=52+i,e[65+i]=i,e[97+i]=26+i;function Q(A,r,i){for(var Q,f,a=0,B=r,t=i.length,b=r+(3*t>>2)-("="==i[t-2])-("="==i[t-1]);a<t;a+=4)Q=e[i.charCodeAt(a+1)],f=e[i.charCodeAt(a+2)],A[B++]=e[i.charCodeAt(a)]<<2|Q>>4,B<b&&(A[B++]=Q<<4|f>>2),B<b&&(A[B++]=f<<6|e[i.charCodeAt(a+3)])}return e[43]=62,e[47]=63,function(A){var e=A.a.buffer,i=new Int8Array(e),f=new Int16Array(e),a=new Int32Array(e),B=new Uint8Array(e),t=new Uint16Array(e),b=new Uint32Array(e),k=(new Float32Array(e),new Float64Array(e)),n=Math.imul,c=(Math.fround,Math.abs),C=Math.clz32,E=(Math.min,Math.max,Math.floor,Math.ceil,Math.trunc,Math.sqrt,A.abort),o=A.b,l=A.c,w=A.d,g=A.e,u=A.f,M=A.g,s=8482448,D=0,I=0,N=0;function G(A){var r,e=0,i=0,Q=0,f=0,t=0,k=0,n=0,c=0,C=0,E=0;s=r=s-16|0;A:{r:{e:{i:{Q:{f:{a:{B:{t:{b:{k:{if((A|=0)>>>0<=244){if(3&(e=(f=a[23336])>>>(i=(n=A>>>0<11?16:A+11&-8)>>>3|0)|0)){e=(A=(i=i+(1&(-1^e))|0)<<3)+93384|0,Q=a[A+93392>>2],(0|e)!=(0|(A=a[Q+8>>2]))?(a[A+12>>2]=e,a[e+8>>2]=A):a[23336]=OQ(-2,i)&f,A=Q+8|0,e=i<<3,a[Q+4>>2]=3|e,a[(e=e+Q|0)+4>>2]=1|a[e+4>>2];break A}if((E=a[23338])>>>0>=n>>>0)break k;if(e){i=A=(e=(0-(A=(0-(A=2<<i)|A)&e<<i)&A)-1|0)>>>12&16,i|=A=(e=e>>>A|0)>>>5&8,i|=A=(e=e>>>A|0)>>>2&4,e=(A=(Q=((i|=A=(e=e>>>A|0)>>>1&2)|(A=(e=e>>>A|0)>>>1&1))+(e>>>A|0)|0)<<3)+93384|0,k=a[A+93392>>2],(0|e)!=(0|(A=a[k+8>>2]))?(a[A+12>>2]=e,a[e+8>>2]=A):(f=OQ(-2,Q)&f,a[23336]=f),a[k+4>>2]=3|n,Q=(A=Q<<3)-n|0,a[(i=k+n|0)+4>>2]=1|Q,a[A+k>>2]=Q,E&&(e=93384+(-8&E)|0,t=a[23341],(A=1<<(E>>>3))&f?A=a[e+8>>2]:(a[23336]=A|f,A=e),a[e+8>>2]=t,a[A+12>>2]=t,a[t+12>>2]=e,a[t+8>>2]=A),A=k+8|0,a[23341]=i,a[23338]=Q;break A}if(!(C=a[23337]))break k;for(i=A=(e=(0-C&C)-1|0)>>>12&16,i|=A=(e=e>>>A|0)>>>5&8,i|=A=(e=e>>>A|0)>>>2&4,i=a[93648+(((i|=A=(e=e>>>A|0)>>>1&2)|(A=(e=e>>>A|0)>>>1&1))+(e>>>A|0)<<2)>>2],t=(-8&a[i+4>>2])-n|0,e=i;(A=a[e+16>>2])||(A=a[e+20>>2]);)t=(Q=(e=(-8&a[A+4>>2])-n|0)>>>0<t>>>0)?e:t,i=Q?A:i,e=A;if(c=a[i+24>>2],(0|(Q=a[i+12>>2]))!=(0|i)){A=a[i+8>>2],a[A+12>>2]=Q,a[Q+8>>2]=A;break r}if(!(A=a[(e=i+20|0)>>2])){if(!(A=a[i+16>>2]))break b;e=i+16|0}for(;k=e,Q=A,(A=a[(e=A+20|0)>>2])||(e=Q+16|0,A=a[Q+16>>2]););a[k>>2]=0;break r}if(n=-1,!(A>>>0>4294967231)&&(n=-8&(A=A+11|0),C=a[23337])){t=0-n|0,f=0,n>>>0<256||(f=31,n>>>0>16777215||(A=A>>>8|0,A<<=k=A+1048320>>>16&8,f=28+((A=((A<<=i=A+520192>>>16&4)<<(e=A+245760>>>16&2)>>>15|0)-(e|i|k)|0)<<1|n>>>A+21&1)|0));n:{c:{if(e=a[93648+(f<<2)>>2])for(A=0,i=n<<(31==(0|f)?0:25-(f>>>1|0)|0);;){if(!((k=(-8&a[e+4>>2])-n|0)>>>0>=t>>>0||(Q=e,t=k))){t=0,A=e;break c}if(k=a[e+20>>2],e=a[16+((i>>>29&4)+e|0)>>2],A=k?(0|k)==(0|e)?A:k:A,i<<=1,!e)break}else A=0;if(!(A|Q)){if(Q=0,!(A=(0-(A=2<<f)|A)&C))break k;i=A=(e=(A&0-A)-1|0)>>>12&16,i|=A=(e=e>>>A|0)>>>5&8,i|=A=(e=e>>>A|0)>>>2&4,A=a[93648+(((i|=A=(e=e>>>A|0)>>>1&2)|(A=(e=e>>>A|0)>>>1&1))+(e>>>A|0)<<2)>>2]}if(!A)break n}for(;t=(i=(e=(-8&a[A+4>>2])-n|0)>>>0<t>>>0)?e:t,Q=i?A:Q,A=(e=a[A+16>>2])||a[A+20>>2];);}if(!(!Q|a[23338]-n>>>0<=t>>>0)){if(f=a[Q+24>>2],(0|Q)!=(0|(i=a[Q+12>>2]))){A=a[Q+8>>2],a[A+12>>2]=i,a[i+8>>2]=A;break e}if(!(A=a[(e=Q+20|0)>>2])){if(!(A=a[Q+16>>2]))break t;e=Q+16|0}for(;k=e,i=A,(A=a[(e=A+20|0)>>2])||(e=i+16|0,A=a[i+16>>2]););a[k>>2]=0;break e}}}if((i=a[23338])>>>0>=n>>>0){Q=a[23341],(e=i-n|0)>>>0>=16?(a[23338]=e,A=Q+n|0,a[23341]=A,a[A+4>>2]=1|e,a[i+Q>>2]=e,a[Q+4>>2]=3|n):(a[23341]=0,a[23338]=0,a[Q+4>>2]=3|i,a[(A=i+Q|0)+4>>2]=1|a[A+4>>2]),A=Q+8|0;break A}if((c=a[23339])>>>0>n>>>0){e=c-n|0,a[23339]=e,A=(i=a[23342])+n|0,a[23342]=A,a[A+4>>2]=1|e,a[i+4>>2]=3|n,A=i+8|0;break A}if(A=0,C=n+47|0,a[23454]?i=a[23456]:(a[23457]=-1,a[23458]=-1,a[23455]=4096,a[23456]=4096,a[23454]=r+12&-16^1431655768,a[23459]=0,a[23447]=0,i=4096),(e=(k=C+i|0)&(t=0-i|0))>>>0<=n>>>0)break A;if((Q=a[23446])&&Q>>>0<(f=(i=a[23444])+e|0)>>>0|i>>>0>=f>>>0)break A;if(4&B[93788])break f;k:{n:{if(Q=a[23342])for(A=93792;;){if((i=a[A>>2])>>>0<=Q>>>0&Q>>>0<i+a[A+4>>2]>>>0)break n;if(!(A=a[A+8>>2]))break}if(-1==(0|(i=Oi(0))))break a;if(f=e,(A=(Q=a[23455])-1|0)&i&&(f=(e-i|0)+(A+i&0-Q)|0),f>>>0<=n>>>0|f>>>0>2147483646)break a;if((Q=a[23446])&&Q>>>0<(t=(A=a[23444])+f|0)>>>0|A>>>0>=t>>>0)break a;if((0|i)!=(0|(A=Oi(f))))break k;break Q}if((f=t&k-c)>>>0>2147483646)break a;if((0|(i=Oi(f)))==(a[A>>2]+a[A+4>>2]|0))break B;A=i}if(!(-1==(0|A)|n+48>>>0<=f>>>0)){if((i=(i=a[23456])+(C-f|0)&0-i)>>>0>2147483646){i=A;break Q}if(-1!=(0|Oi(i))){f=i+f|0,i=A;break Q}Oi(0-f|0);break a}if(i=A,-1!=(0|A))break Q;break a}Q=0;break r}i=0;break e}if(-1!=(0|i))break Q}a[23447]=4|a[23447]}if(e>>>0>2147483646)break i;if(-1==(0|(i=Oi(e)))|-1==(0|(A=Oi(0)))|A>>>0<=i>>>0)break i;if((f=A-i|0)>>>0<=n+40>>>0)break i}A=a[23444]+f|0,a[23444]=A,A>>>0>b[23445]&&(a[23445]=A);Q:{f:{a:{if(k=a[23342]){for(A=93792;;){if(((Q=a[A>>2])+(e=a[A+4>>2])|0)==(0|i))break a;if(!(A=a[A+8>>2]))break}break f}for((A=a[23340])>>>0<=i>>>0&&A||(a[23340]=i),A=0,a[23449]=f,a[23448]=i,a[23344]=-1,a[23345]=a[23454],a[23451]=0;e=(Q=A<<3)+93384|0,a[Q+93392>>2]=e,a[Q+93396>>2]=e,32!=(0|(A=A+1|0)););e=(Q=f-40|0)-(A=i+8&7?-8-i&7:0)|0,a[23339]=e,A=A+i|0,a[23342]=A,a[A+4>>2]=1|e,a[4+(i+Q|0)>>2]=40,a[23343]=a[23458];break Q}if(!(8&B[A+12|0]|Q>>>0>k>>>0|i>>>0<=k>>>0)){a[A+4>>2]=e+f,i=(A=k+8&7?-8-k&7:0)+k|0,a[23342]=i,A=(e=a[23339]+f|0)-A|0,a[23339]=A,a[i+4>>2]=1|A,a[4+(e+k|0)>>2]=40,a[23343]=a[23458];break Q}}b[23340]>i>>>0&&(a[23340]=i),e=i+f|0,A=93792;f:{a:{B:{t:{b:{k:{for(;;){if((0|e)!=a[A>>2]){if(A=a[A+8>>2])continue;break k}break}if(!(8&B[A+12|0]))break b}for(A=93792;;){if((e=a[A>>2])>>>0<=k>>>0&&(t=e+a[A+4>>2]|0)>>>0>k>>>0)break t;A=a[A+8>>2]}}if(a[A>>2]=i,a[A+4>>2]=a[A+4>>2]+f,a[(C=(i+8&7?-8-i&7:0)+i|0)+4>>2]=3|n,A=(f=e+(e+8&7?-8-e&7:0)|0)-(c=n+C|0)|0,(0|f)==(0|k)){a[23342]=c,A=a[23339]+A|0,a[23339]=A,a[c+4>>2]=1|A;break a}if(a[23341]==(0|f)){a[23341]=c,A=a[23338]+A|0,a[23338]=A,a[c+4>>2]=1|A,a[A+c>>2]=A;break a}if(1==(3&(t=a[f+4>>2]))){k=-8&t;b:if(t>>>0<=255){if(Q=a[f+8>>2],e=t>>>3|0,(0|(i=a[f+12>>2]))==(0|Q)){a[23336]=a[23336]&OQ(-2,e);break b}a[Q+12>>2]=i,a[i+8>>2]=Q}else{if(n=a[f+24>>2],(0|f)==(0|(i=a[f+12>>2])))if((e=a[(t=f+20|0)>>2])||(e=a[(t=f+16|0)>>2])){for(;Q=t,i=e,(e=a[(t=e+20|0)>>2])||(t=i+16|0,e=a[i+16>>2]););a[Q>>2]=0}else i=0;else e=a[f+8>>2],a[e+12>>2]=i,a[i+8>>2]=e;if(n){Q=a[f+28>>2];k:{if(a[(e=93648+(Q<<2)|0)>>2]==(0|f)){if(a[e>>2]=i,i)break k;a[23337]=a[23337]&OQ(-2,Q);break b}if(a[n+(a[n+16>>2]==(0|f)?16:20)>>2]=i,!i)break b}a[i+24>>2]=n,(e=a[f+16>>2])&&(a[i+16>>2]=e,a[e+24>>2]=i),(e=a[f+20>>2])&&(a[i+20>>2]=e,a[e+24>>2]=i)}}t=a[(f=f+k|0)+4>>2],A=A+k|0}if(a[f+4>>2]=-2&t,a[c+4>>2]=1|A,a[A+c>>2]=A,A>>>0<=255){e=93384+(-8&A)|0,(i=a[23336])&(A=1<<(A>>>3))?A=a[e+8>>2]:(a[23336]=A|i,A=e),a[e+8>>2]=c,a[A+12>>2]=c,a[c+12>>2]=e,a[c+8>>2]=A;break a}if(t=31,A>>>0<=16777215&&(e=A>>>8|0,e<<=t=e+1048320>>>16&8,t=28+((e=((e<<=Q=e+520192>>>16&4)<<(i=e+245760>>>16&2)>>>15|0)-(i|Q|t)|0)<<1|A>>>e+21&1)|0),a[c+28>>2]=t,a[c+16>>2]=0,a[c+20>>2]=0,e=93648+(t<<2)|0,(Q=a[23337])&(i=1<<t)){for(t=A<<(31==(0|t)?0:25-(t>>>1|0)|0),i=a[e>>2];;){if((-8&a[(e=i)+4>>2])==(0|A))break B;if(i=t>>>29|0,t<<=1,!(i=a[(Q=(4&i)+e|0)+16>>2]))break}a[Q+16>>2]=c}else a[23337]=i|Q,a[e>>2]=c;a[c+24>>2]=e,a[c+12>>2]=c,a[c+8>>2]=c;break a}for(e=(Q=f-40|0)-(A=i+8&7?-8-i&7:0)|0,a[23339]=e,A=A+i|0,a[23342]=A,a[A+4>>2]=1|e,a[4+(i+Q|0)>>2]=40,a[23343]=a[23458],a[(Q=(A=(t+(t-39&7?39-t&7:0)|0)-47|0)>>>0<k+16>>>0?k:A)+4>>2]=27,A=a[23451],a[Q+16>>2]=a[23450],a[Q+20>>2]=A,A=a[23449],a[Q+8>>2]=a[23448],a[Q+12>>2]=A,a[23450]=Q+8,a[23449]=f,a[23448]=i,a[23451]=0,A=Q+24|0;a[A+4>>2]=7,e=A+8|0,A=A+4|0,e>>>0<t>>>0;);if((0|Q)==(0|k))break Q;if(a[Q+4>>2]=-2&a[Q+4>>2],t=Q-k|0,a[k+4>>2]=1|t,a[Q>>2]=t,t>>>0<=255){e=93384+(-8&t)|0,(i=a[23336])&(A=1<<(t>>>3))?A=a[e+8>>2]:(a[23336]=A|i,A=e),a[e+8>>2]=k,a[A+12>>2]=k,a[k+12>>2]=e,a[k+8>>2]=A;break Q}if(A=31,t>>>0<=16777215&&(A=t>>>8|0,A<<=Q=A+1048320>>>16&8,A=28+((A=((A<<=i=A+520192>>>16&4)<<(e=A+245760>>>16&2)>>>15|0)-(e|i|Q)|0)<<1|t>>>A+21&1)|0),a[k+28>>2]=A,a[k+16>>2]=0,a[k+20>>2]=0,e=93648+(A<<2)|0,(Q=a[23337])&(i=1<<A)){for(A=t<<(31==(0|A)?0:25-(A>>>1|0)|0),Q=a[e>>2];;){if((0|t)==(-8&a[(e=Q)+4>>2]))break f;if(i=A>>>29|0,A<<=1,!(Q=a[(i=(4&i)+e|0)+16>>2]))break}a[i+16>>2]=k}else a[23337]=i|Q,a[e>>2]=k;a[k+24>>2]=e,a[k+12>>2]=k,a[k+8>>2]=k;break Q}A=a[e+8>>2],a[A+12>>2]=c,a[e+8>>2]=c,a[c+24>>2]=0,a[c+12>>2]=e,a[c+8>>2]=A}A=C+8|0;break A}A=a[e+8>>2],a[A+12>>2]=k,a[e+8>>2]=k,a[k+24>>2]=0,a[k+12>>2]=e,a[k+8>>2]=A}if(!((A=a[23339])>>>0<=n>>>0)){e=A-n|0,a[23339]=e,A=(i=a[23342])+n|0,a[23342]=A,a[A+4>>2]=1|e,a[i+4>>2]=3|n,A=i+8|0;break A}}a[23332]=48,A=0;break A}e:if(f){e=a[Q+28>>2];i:{if(a[(A=93648+(e<<2)|0)>>2]==(0|Q)){if(a[A>>2]=i,i)break i;C=OQ(-2,e)&C,a[23337]=C;break e}if(a[f+(a[f+16>>2]==(0|Q)?16:20)>>2]=i,!i)break e}a[i+24>>2]=f,(A=a[Q+16>>2])&&(a[i+16>>2]=A,a[A+24>>2]=i),(A=a[Q+20>>2])&&(a[i+20>>2]=A,a[A+24>>2]=i)}e:if(t>>>0<=15)A=t+n|0,a[Q+4>>2]=3|A,a[(A=A+Q|0)+4>>2]=1|a[A+4>>2];else if(a[Q+4>>2]=3|n,a[(f=Q+n|0)+4>>2]=1|t,a[f+t>>2]=t,t>>>0<=255)e=93384+(-8&t)|0,(i=a[23336])&(A=1<<(t>>>3))?A=a[e+8>>2]:(a[23336]=A|i,A=e),a[e+8>>2]=f,a[A+12>>2]=f,a[f+12>>2]=e,a[f+8>>2]=A;else{A=31,t>>>0<=16777215&&(A=t>>>8|0,A<<=k=A+1048320>>>16&8,A=28+((A=((A<<=i=A+520192>>>16&4)<<(e=A+245760>>>16&2)>>>15|0)-(e|i|k)|0)<<1|t>>>A+21&1)|0),a[f+28>>2]=A,a[f+16>>2]=0,a[f+20>>2]=0,e=93648+(A<<2)|0;i:{if((i=1<<A)&C){for(A=t<<(31==(0|A)?0:25-(A>>>1|0)|0),n=a[e>>2];;){if((-8&a[(e=n)+4>>2])==(0|t))break i;if(i=A>>>29|0,A<<=1,!(n=a[(i=(4&i)+e|0)+16>>2]))break}a[i+16>>2]=f}else a[23337]=i|C,a[e>>2]=f;a[f+24>>2]=e,a[f+12>>2]=f,a[f+8>>2]=f;break e}A=a[e+8>>2],a[A+12>>2]=f,a[e+8>>2]=f,a[f+24>>2]=0,a[f+12>>2]=e,a[f+8>>2]=A}A=Q+8|0;break A}r:if(c){e=a[i+28>>2];e:{if(a[(A=93648+(e<<2)|0)>>2]==(0|i)){if(a[A>>2]=Q,Q)break e;a[23337]=OQ(-2,e)&C;break r}if(a[c+(a[c+16>>2]==(0|i)?16:20)>>2]=Q,!Q)break r}a[Q+24>>2]=c,(A=a[i+16>>2])&&(a[Q+16>>2]=A,a[A+24>>2]=Q),(A=a[i+20>>2])&&(a[Q+20>>2]=A,a[A+24>>2]=Q)}t>>>0<=15?(A=t+n|0,a[i+4>>2]=3|A,a[(A=A+i|0)+4>>2]=1|a[A+4>>2]):(a[i+4>>2]=3|n,a[(Q=i+n|0)+4>>2]=1|t,a[Q+t>>2]=t,E&&(e=93384+(-8&E)|0,k=a[23341],(A=1<<(E>>>3))&f?A=a[e+8>>2]:(a[23336]=A|f,A=e),a[e+8>>2]=k,a[A+12>>2]=k,a[k+12>>2]=e,a[k+8>>2]=A),a[23341]=Q,a[23338]=t),A=i+8|0}return s=r+16|0,0|A}function U(A,r,e){var i=0,Q=0,f=0,t=0,b=0,k=0,n=0,c=0,C=0,E=0,o=0,l=0,w=0,g=0,u=0,M=0,s=0,D=0,I=0,N=0,G=0,U=0,F=0,Z=0,d=0,W=0,V=0,Y=0,R=0,y=0,h=0,J=0,T=0,m=0,L=0,X=0,S=0,H=0,p=0,v=0,z=0,j=0,x=0,K=0,O=0,P=0,q=0,_=0,$=0,AA=0,rA=0,eA=0,iA=0,QA=0,fA=0,aA=0,BA=0,tA=0,bA=0,kA=0,nA=0,cA=0,CA=0,EA=0,oA=0,lA=0,wA=0,gA=0,uA=0,MA=0,sA=0,DA=0,IA=0,NA=0,GA=0,UA=0,FA=0,ZA=0,dA=0;for(E=a[A+16>>2],c=a[A+12>>2],Q=a[A+8>>2],o=a[A+4>>2],C=a[A>>2];dA=a[A>>2],t=(t=B[r+52|0]|B[r+53|0]<<8|B[r+54|0]<<16|B[r+55|0]<<24)<<24|t<<8&16711680|t>>>8&65280|t>>>24,f=OQ((l=(f=B[r+8|0]|B[r+9|0]<<8|B[r+10|0]<<16|B[r+11|0]<<24)<<24|f<<8&16711680|f>>>8&65280|f>>>24)^(gA=(f=B[r+16|0]|B[r+17|0]<<8|B[r+18|0]<<16|B[r+19|0]<<24)<<24|f<<8&16711680|f>>>8&65280|f>>>24)^(N=(f=B[r+40|0]|B[r+41|0]<<8|B[r+42|0]<<16|B[r+43|0]<<24)<<24|f<<8&16711680|f>>>8&65280|f>>>24)^(D=(f=B[r+60|0]|B[r+61|0]<<8|B[r+62|0]<<16|B[r+63|0]<<24)<<24|f<<8&16711680|f>>>8&65280|f>>>24),1),k=OQ(f^t^(uA=(k=B[r+28|0]|B[r+29|0]<<8|B[r+30|0]<<16|B[r+31|0]<<24)<<24|k<<8&16711680|k>>>8&65280|k>>>24)^(MA=(k=B[r+20|0]|B[r+21|0]<<8|B[r+22|0]<<16|B[r+23|0]<<24)<<24|k<<8&16711680|k>>>8&65280|k>>>24),1),u=(i=B[r+48|0]|B[r+49|0]<<8|B[r+50|0]<<16|B[r+51|0]<<24)<<24|i<<8&16711680|i>>>8&65280|i>>>24,i=OQ((U=(i=B[r+12|0]|B[r+13|0]<<8|B[r+14|0]<<16|B[r+15|0]<<24)<<24|i<<8&16711680|i>>>8&65280|i>>>24)^(UA=(i=B[r+4|0]|B[r+5|0]<<8|B[r+6|0]<<16|B[r+7|0]<<24)<<24|i<<8&16711680|i>>>8&65280|i>>>24)^(CA=(i=B[r+36|0]|B[r+37|0]<<8|B[r+38|0]<<16|B[r+39|0]<<24)<<24|i<<8&16711680|i>>>8&65280|i>>>24)^(I=(i=B[r+56|0]|B[r+57|0]<<8|B[r+58|0]<<16|B[r+59|0]<<24)<<24|i<<8&16711680|i>>>8&65280|i>>>24),1),Z=OQ(i^gA^(sA=(n=B[r+24|0]|B[r+25|0]<<8|B[r+26|0]<<16|B[r+27|0]<<24)<<24|n<<8&16711680|n>>>8&65280|n>>>24)^u,1),d=OQ(Z^D^CA^uA,1),W=OQ(d^f^u^N,1),n=OQ(W^k^t^D,1),EA=(b=B[r+44|0]|B[r+45|0]<<8|B[r+46|0]<<16|B[r+47|0]<<24)<<24|b<<8&16711680|b>>>8&65280|b>>>24,V=OQ((F=(b=B[0|r]|B[r+1|0]<<8|B[r+2|0]<<16|B[r+3|0]<<24)<<24|b<<8&16711680|b>>>8&65280|b>>>24)^(oA=(b=B[r+32|0]|B[r+33|0]<<8|B[r+34|0]<<16|B[r+35|0]<<24)<<24|b<<8&16711680|b>>>8&65280|b>>>24)^t^l,1),b=OQ(EA^MA^V^U,1),Y=OQ(k^N^V^oA,1),w=OQ(Y^t^EA^b,1),R=OQ(f^V^Y^n,1),M=OQ(R^k^b^w,1),y=OQ(I^oA^sA^b,1),h=OQ(y^i^CA^EA,1),J=OQ(h^Z^u^I,1),T=OQ(J^d^i^D,1),m=OQ(T^W^f^Z,1),L=OQ(m^k^d^n,1),X=OQ(L^W^Y^R,1),s=OQ(X^n^w^M,1),S=OQ(I^V^y^w,1),H=OQ(S^i^b^h,1),p=OQ(H^Z^y^J,1),v=OQ(p^d^h^T,1),z=OQ(v^W^J^m,1),j=OQ(z^n^T^L,1),x=OQ(j^R^m^X,1),G=OQ(x^M^L^s,1),K=OQ(Y^y^S^M,1),O=OQ(K^w^h^H,1),P=OQ(O^J^S^p,1),q=OQ(P^T^H^v,1),_=OQ(q^m^p^z,1),$=OQ(_^L^v^j,1),AA=OQ($^X^z^x,1),rA=OQ(AA^s^j^G,1),eA=OQ(R^S^K^s,1),iA=OQ(eA^M^H^O,1),QA=OQ(iA^p^K^P,1),fA=OQ(QA^v^O^q,1),aA=OQ(fA^z^P^_,1),BA=OQ(aA^j^q^$,1),lA=OQ(BA^x^_^AA,1),DA=OQ(lA^G^$^rA,1),tA=OQ(X^K^eA^G,1),bA=OQ(tA^s^O^iA,1),kA=OQ(bA^P^eA^QA,1),nA=OQ(kA^q^iA^fA,1),IA=OQ(nA^_^QA^aA,1),NA=OQ(IA^$^fA^BA,1),FA=OQ(NA^AA^aA^lA,1),ZA=OQ(FA^rA^BA^DA,1),cA=OQ(x^eA^tA^rA,1),GA=OQ(cA^AA^tA^DA,1),wA=OQ(G^iA^bA^cA,1),F=1518500249+(((OQ(C,5)+E|0)+((Q^c)&o^c)|0)+F|0)|0,E=OQ(F,30),o=1518500249+((((g=c)+(((c=OQ(o,30))^Q)&C^Q)|0)+UA|0)+OQ(F,5)|0)|0,l=1518500249+(((Q+l|0)+(c^F&((C=OQ(C,30))^c))|0)+OQ(o,5)|0)|0,U=1518500249+(OQ(l,5)+((c+U|0)+(C^o&(C^E))|0)|0)|0,Q=OQ(l,30),C=1518500249+(gA+((C+(E^l&((o=OQ(o,30))^E))|0)+OQ(U,5)|0)|0)|0,l=1518500249+(OQ(C,5)+((E+MA|0)+(o^U&(Q^o))|0)|0)|0,c=OQ(l,30),o=1518500249+(((o+sA|0)+(Q^C&((E=OQ(U,30))^Q))|0)+OQ(l,5)|0)|0,l=1518500249+(((Q+uA|0)+(E^l&((C=OQ(C,30))^E))|0)+OQ(o,5)|0)|0,E=1518500249+(OQ(l,5)+((E+oA|0)+(C^o&(C^c))|0)|0)|0,Q=OQ(l,30),C=1518500249+((CA+(C+(c^l&((o=OQ(o,30))^c))|0)|0)+OQ(E,5)|0)|0,N=1518500249+(OQ(C,5)+((c+N|0)+(o^E&(Q^o))|0)|0)|0,c=OQ(N,30),o=1518500249+(((o+EA|0)+(Q^C&((E=OQ(E,30))^Q))|0)+OQ(N,5)|0)|0,u=1518500249+(((Q+u|0)+(E^N&((C=OQ(C,30))^E))|0)+OQ(o,5)|0)|0,E=1518500249+(OQ(u,5)+((t+E|0)+(C^o&(C^c))|0)|0)|0,Q=OQ(u,30),u=1518500249+(((g=C+I|0)+(c^u&((C=OQ(o,30))^c))|0)+OQ(E,5)|0)|0,I=1518500249+(OQ(u,5)+((c+D|0)+(C^E&(Q^C))|0)|0)|0,t=OQ(I,30),c=OQ(u,30),D=1518500249+(((g=C+V|0)+(Q^u&((C=OQ(E,30))^Q))|0)+OQ(I,5)|0)|0,i=1518500249+(OQ(D,5)+((Q+i|0)+(C^I&(C^c))|0)|0)|0,Q=OQ(i,30),g=b+c|0,b=OQ(D,30),c=1518500249+(OQ(i,5)+((f+C|0)+(c^D&(t^c))|0)|0)|0,f=1518500249+((g+(t^i&(b^t))|0)+OQ(c,5)|0)|0,C=1859775393+(OQ(f,5)+((t+Z|0)+(c^Q^b)|0)|0)|0,t=OQ(C,30),i=OQ(f,30),f=1859775393+(((b=k+b|0)+(f^(k=OQ(c,30))^Q)|0)+OQ(C,5)|0)|0,b=1859775393+(OQ(f,5)+((Q+y|0)+(C^i^k)|0)|0)|0,Q=OQ(b,30),g=(i+Y|0)+(b^(c=OQ(f,30))^t)|0,i=1859775393+(OQ(b,5)+((k+d|0)+(f^i^t)|0)|0)|0,f=1859775393+(g+OQ(i,5)|0)|0,b=1859775393+(OQ(f,5)+((t+h|0)+(i^Q^c)|0)|0)|0,t=OQ(b,30),k=OQ(f,30),f=1859775393+(((c+W|0)+(f^(i=OQ(i,30))^Q)|0)+OQ(b,5)|0)|0,b=1859775393+(OQ(f,5)+((Q+w|0)+(b^i^k)|0)|0)|0,Q=OQ(b,30),c=k+n|0,n=OQ(f,30),i=1859775393+(OQ(b,5)+((i+J|0)+(f^t^k)|0)|0)|0,f=1859775393+((c+(b^n^t)|0)+OQ(i,5)|0)|0,b=1859775393+(OQ(f,5)+((t+S|0)+(i^Q^n)|0)|0)|0,t=OQ(b,30),k=OQ(f,30),f=1859775393+(((n+T|0)+(f^(i=OQ(i,30))^Q)|0)+OQ(b,5)|0)|0,n=1859775393+(OQ(f,5)+((Q+R|0)+(b^i^k)|0)|0)|0,Q=OQ(n,30),b=OQ(f,30),i=1859775393+(OQ(n,5)+((i+H|0)+(f^t^k)|0)|0)|0,f=1859775393+(((k+m|0)+(n^b^t)|0)+OQ(i,5)|0)|0,t=1859775393+(OQ(f,5)+((t+M|0)+(i^Q^b)|0)|0)|0,k=OQ(t,30),i=1859775393+(((b+p|0)+(f^(n=OQ(i,30))^Q)|0)+OQ(t,5)|0)|0,f=1859775393+(((Q+L|0)+(t^(b=OQ(f,30))^n)|0)+OQ(i,5)|0)|0,Q=1859775393+(OQ(f,5)+((n+K|0)+(i^k^b)|0)|0)|0,t=OQ(f,30),f=1859775393+(((c=b+v|0)+(f^(b=OQ(i,30))^k)|0)+OQ(Q,5)|0)|0,i=(OQ(f,5)+((k+X|0)+(b&(Q|t)|Q&t)|0)|0)-1894007588|0,k=OQ(i,30),c=(t+z|0)+((Q=OQ(Q,30))&((n=OQ(f,30))|i)|i&n)|0,i=(OQ(i,5)+((b+O|0)+(t&(Q|f)|Q&f)|0)|0)-1894007588|0,t=(c+OQ(i,5)|0)-1894007588|0,Q=(OQ(t,5)+((Q+s|0)+(n&(i|k)|i&k)|0)|0)-1894007588|0,f=OQ(t,30),t=(((n+P|0)+(k&((i=OQ(i,30))|t)|i&t)|0)+OQ(Q,5)|0)-1894007588|0,n=(OQ(t,5)+((k+j|0)+(i&(Q|f)|Q&f)|0)|0)-1894007588|0,k=OQ(n,30),Q=OQ(Q,30),b=OQ(t,30),i=(OQ(n,5)+((i+eA|0)+(f&(Q|t)|Q&t)|0)|0)-1894007588|0,t=(((f+q|0)+(Q&(b|n)|b&n)|0)+OQ(i,5)|0)-1894007588|0,Q=(OQ(t,5)+((Q+x|0)+(b&(i|k)|i&k)|0)|0)-1894007588|0,f=OQ(t,30),t=(((b+iA|0)+(k&((i=OQ(i,30))|t)|i&t)|0)+OQ(Q,5)|0)-1894007588|0,n=(OQ(t,5)+((k+_|0)+(i&(Q|f)|Q&f)|0)|0)-1894007588|0,k=OQ(n,30),Q=OQ(Q,30),b=OQ(t,30),i=(OQ(n,5)+((i+G|0)+(f&(Q|t)|Q&t)|0)|0)-1894007588|0,t=(((f+QA|0)+(Q&(b|n)|b&n)|0)+OQ(i,5)|0)-1894007588|0,Q=(OQ(t,5)+((Q+$|0)+(b&(i|k)|i&k)|0)|0)-1894007588|0,f=OQ(t,30),t=(((b+tA|0)+(k&((i=OQ(i,30))|t)|i&t)|0)+OQ(Q,5)|0)-1894007588|0,n=(OQ(t,5)+((k+fA|0)+(i&(Q|f)|Q&f)|0)|0)-1894007588|0,k=OQ(n,30),Q=OQ(Q,30),b=OQ(t,30),t=(OQ(n,5)+((i+AA|0)+(f&(Q|t)|Q&t)|0)|0)-1894007588|0,f=(((f+bA|0)+(Q&(b|n)|b&n)|0)+OQ(t,5)|0)-1894007588|0,i=OQ(f,30),c=(b+rA|0)+(k&((n=OQ(t,30))|f)|f&n)|0,b=(OQ(f,5)+((Q+aA|0)+(b&(t|k)|t&k)|0)|0)-1894007588|0,Q=(c+OQ(b,5)|0)-1894007588|0,k=(OQ(Q,5)+((k+kA|0)+(b^i^n)|0)|0)-899497514|0,t=OQ(k,30),f=OQ(Q,30),Q=(((c=n+BA|0)+(Q^(n=OQ(b,30))^i)|0)+OQ(k,5)|0)-899497514|0,i=(OQ(Q,5)+((i+cA|0)+(k^f^n)|0)|0)-899497514|0,k=OQ(i,30),c=(f+lA|0)+(i^(b=OQ(Q,30))^t)|0,i=(OQ(i,5)+((n+nA|0)+(Q^f^t)|0)|0)-899497514|0,Q=(c+OQ(i,5)|0)-899497514|0,n=(OQ(Q,5)+((t+wA|0)+(i^k^b)|0)|0)-899497514|0,t=OQ(n,30),f=OQ(Q,30),Q=(((b+IA|0)+(Q^(i=OQ(i,30))^k)|0)+OQ(n,5)|0)-899497514|0,n=(OQ(Q,5)+((k+DA|0)+(n^i^f)|0)|0)-899497514|0,k=OQ(n,30),g=(f+NA|0)+(n^(b=OQ(Q,30))^t)|0,f=((((c=i)+(i=OQ(QA^tA^kA^wA,1))|0)+(Q^f^t)|0)+OQ(n,5)|0)-899497514|0,Q=(g+OQ(f,5)|0)-899497514|0,t=(OQ(Q,5)+((t+GA|0)+(f^k^b)|0)|0)-899497514|0,n=OQ(t,30),w=OQ(Q,30),f=OQ(f,30),Q=((((c=b)+(b=OQ(i^fA^bA^nA,1))|0)+(Q^f^k)|0)+OQ(t,5)|0)-899497514|0,t=(((k+FA|0)+(t^w^f)|0)+OQ(Q,5)|0)-899497514|0,k=OQ(t,30),M=OQ(Q,30),Q=((((s=OQ(rA^bA^wA^GA,1))+f|0)+(Q^n^w)|0)+OQ(t,5)|0)-899497514|0,f=((((G=OQ(b^aA^kA^IA,1))+w|0)+(t^n^M)|0)+OQ(Q,5)|0)-899497514|0,n=(((n+ZA|0)+(k^M^Q)|0)+OQ(f,5)|0)-899497514|0,E=(t=OQ(n,30))+a[A+16>>2]|0,a[A+16>>2]=E,w=((((s=OQ(s^i^kA^cA,1))+M|0)+(f^(i=OQ(Q,30))^k)|0)+OQ(n,5)|0)-899497514|0,c=(M=OQ(w,30))+a[A+12>>2]|0,a[A+12>>2]=c,g=a[A+8>>2],k=(((Q=OQ(G^BA^nA^NA,1)+k|0)+(n^(f=OQ(f,30))^i)|0)+OQ(w,5)|0)-899497514|0,Q=g+OQ(k,30)|0,a[A+8>>2]=Q,o=(i=(((OQ(cA^lA^GA^ZA,1)+i|0)+(w^f^t)|0)+OQ(k,5)|0)-899497514|0)+a[A+4>>2]|0,a[A+4>>2]=o,C=(((f+(OQ(s^b^nA^wA,1)+dA|0)|0)+(k^t^M)|0)+OQ(i,5)|0)-899497514|0,a[A>>2]=C,r=r- -64|0,e=e-1|0;);}function F(A,r,e){var i,Q=0,f=0,t=0,b=0,k=0,n=0,c=0,C=0,E=0,o=0,l=0,w=0,g=0,u=0,M=0,D=0,I=0,N=0,G=0,U=0,F=0,Z=0,d=0,W=0,V=0,Y=0,R=0,y=0,h=0,J=0,T=0,m=0,L=0,X=0,S=0;if(i=s+-64|0,e)for(N=a[A>>2],Z=a[A+4>>2],Y=a[A+8>>2],L=a[A+12>>2],G=a[A+16>>2],W=a[A+20>>2],R=a[A+24>>2],X=a[A+28>>2];;){for(k=(Q=B[0|r]|B[r+1|0]<<8|B[r+2|0]<<16|B[r+3|0]<<24)<<24|Q<<8&16711680|Q>>>8&65280|Q>>>24,a[i>>2]=k,t=(Q=B[r+4|0]|B[r+5|0]<<8|B[r+6|0]<<16|B[r+7|0]<<24)<<24|Q<<8&16711680|Q>>>8&65280|Q>>>24,a[i+4>>2]=t,C=(Q=B[r+8|0]|B[r+9|0]<<8|B[r+10|0]<<16|B[r+11|0]<<24)<<24|Q<<8&16711680|Q>>>8&65280|Q>>>24,a[i+8>>2]=C,l=(Q=B[r+12|0]|B[r+13|0]<<8|B[r+14|0]<<16|B[r+15|0]<<24)<<24|Q<<8&16711680|Q>>>8&65280|Q>>>24,a[i+12>>2]=l,n=(Q=B[r+16|0]|B[r+17|0]<<8|B[r+18|0]<<16|B[r+19|0]<<24)<<24|Q<<8&16711680|Q>>>8&65280|Q>>>24,a[i+16>>2]=n,o=(Q=B[r+20|0]|B[r+21|0]<<8|B[r+22|0]<<16|B[r+23|0]<<24)<<24|Q<<8&16711680|Q>>>8&65280|Q>>>24,a[i+20>>2]=o,E=(Q=B[r+24|0]|B[r+25|0]<<8|B[r+26|0]<<16|B[r+27|0]<<24)<<24|Q<<8&16711680|Q>>>8&65280|Q>>>24,a[i+24>>2]=E,u=(Q=B[r+28|0]|B[r+29|0]<<8|B[r+30|0]<<16|B[r+31|0]<<24)<<24|Q<<8&16711680|Q>>>8&65280|Q>>>24,a[i+28>>2]=u,M=(Q=B[r+32|0]|B[r+33|0]<<8|B[r+34|0]<<16|B[r+35|0]<<24)<<24|Q<<8&16711680|Q>>>8&65280|Q>>>24,a[i+32>>2]=M,w=(Q=B[r+36|0]|B[r+37|0]<<8|B[r+38|0]<<16|B[r+39|0]<<24)<<24|Q<<8&16711680|Q>>>8&65280|Q>>>24,a[i+36>>2]=w,I=(Q=B[r+40|0]|B[r+41|0]<<8|B[r+42|0]<<16|B[r+43|0]<<24)<<24|Q<<8&16711680|Q>>>8&65280|Q>>>24,a[i+40>>2]=I,y=(Q=B[r+44|0]|B[r+45|0]<<8|B[r+46|0]<<16|B[r+47|0]<<24)<<24|Q<<8&16711680|Q>>>8&65280|Q>>>24,a[i+44>>2]=y,h=(Q=B[r+48|0]|B[r+49|0]<<8|B[r+50|0]<<16|B[r+51|0]<<24)<<24|Q<<8&16711680|Q>>>8&65280|Q>>>24,a[i+48>>2]=h,J=(Q=B[r+52|0]|B[r+53|0]<<8|B[r+54|0]<<16|B[r+55|0]<<24)<<24|Q<<8&16711680|Q>>>8&65280|Q>>>24,a[i+52>>2]=J,U=(Q=B[r+56|0]|B[r+57|0]<<8|B[r+58|0]<<16|B[r+59|0]<<24)<<24|Q<<8&16711680|Q>>>8&65280|Q>>>24,a[i+56>>2]=U,T=(Q=B[r+60|0]|B[r+61|0]<<8|B[r+62|0]<<16|B[r+63|0]<<24)<<24|Q<<8&16711680|Q>>>8&65280|Q>>>24,a[i+60>>2]=T,g=(c=N&Z)^(Q=(k=1116352408+(k+(((((-1^G)&R)+X|0)+(G&W)|0)+(OQ(G,26)^OQ(G,21)^OQ(G,7))|0)|0)|0)+((OQ(N,30)^OQ(N,19)^OQ(N,10))+(c^(N^Z)&Y)|0)|0)&Z,c=Q&N,b=(OQ(Q,30)^OQ(Q,19)^OQ(Q,10))+(g^c)|0,f=OQ(k=k+L|0,26)^OQ(k,21),f=Q&(t=(g=b)+(b=1899447441+(t+(((((-1^k)&W)+R|0)+(k&G)|0)+(OQ(k,7)^f)|0)|0)|0)|0),g=(OQ(t,30)^OQ(t,19)^OQ(t,10))+(f^c^t&N)|0,b=OQ(c=b+Y|0,26)^OQ(c,21),g=f^Q&(C=(b=((((C+W|0)+((-1^c)&G)|0)+(k&c)|0)+(OQ(c,7)^b)|0)-1245643825|0)+g|0),f=t&C,g=(OQ(C,30)^OQ(C,19)^OQ(C,10))+(g^f)|0,F=OQ(b=b+Z|0,26)^OQ(b,21),F=C&(l=(D=g)+(g=((((l+G|0)+(k&(-1^b))|0)+(b&c)|0)+(OQ(b,7)^F)|0)-373957723|0)|0),d=(OQ(l,30)^OQ(l,19)^OQ(l,10))+(F^f^t&l)|0,g=OQ(f=g+N|0,26)^OQ(f,21),g=(k=(n=961987163+((((k+n|0)+(c&(-1^f))|0)+(f&b)|0)+(OQ(f,7)^g)|0)|0)+d|0)&l,F=(OQ(k,30)^OQ(k,19)^OQ(k,10))+(g^F^k&C)|0,n=OQ(Q=Q+n|0,26)^OQ(Q,21),o=k&(c=(n=1508970993+((((c+o|0)+(b&(-1^Q))|0)+(Q&f)|0)+(OQ(Q,7)^n)|0)|0)+F|0),g=(OQ(c,30)^OQ(c,19)^OQ(c,10))+(o^g^c&l)|0,n=OQ(t=t+n|0,26)^OQ(t,21),g=o^(b=(n=((((b+E|0)+(f&(-1^t))|0)+(Q&t)|0)+(OQ(t,7)^n)|0)-1841331548|0)+g|0)&k,o=b&c,E=(OQ(b,30)^OQ(b,19)^OQ(b,10))+(g^o)|0,n=OQ(C=n+C|0,26)^OQ(C,21),g=o^(f=(n=((((f+u|0)+(Q&(-1^C))|0)+(t&C)|0)+(OQ(C,7)^n)|0)-1424204075|0)+E|0)&c,o=f&b,E=(OQ(f,30)^OQ(f,19)^OQ(f,10))+(g^o)|0,n=OQ(l=n+l|0,26)^OQ(l,21),g=o^(Q=(n=((((Q+M|0)+(t&(-1^l))|0)+(C&l)|0)+(OQ(l,7)^n)|0)-670586216|0)+E|0)&b,o=Q&f,E=(OQ(Q,30)^OQ(Q,19)^OQ(Q,10))+(g^o)|0,n=OQ(k=k+n|0,26)^OQ(k,21),E=Q&(n=(t=310598401+((((t+w|0)+(C&(-1^k))|0)+(k&l)|0)+(OQ(k,7)^n)|0)|0)+E|0),o=(OQ(n,30)^OQ(n,19)^OQ(n,10))+(E^o^f&n)|0,t=OQ(c=t+c|0,26)^OQ(c,21),C=n&(o=(t=607225278+((((C+I|0)+(l&(-1^c))|0)+(k&c)|0)+(OQ(c,7)^t)|0)|0)+o|0),E=(OQ(o,30)^OQ(o,19)^OQ(o,10))+(C^E^Q&o)|0,t=OQ(b=t+b|0,26)^OQ(b,21),w=C^n&(E=(t=1426881987+((((l+y|0)+(k&(-1^b))|0)+(b&c)|0)+(OQ(b,7)^t)|0)|0)+E|0),C=E&o,l=(OQ(E,30)^OQ(E,19)^OQ(E,10))+(w^C)|0,t=OQ(f=f+t|0,26)^OQ(f,21),w=C^(k=(t=1925078388+((((k+h|0)+(c&(-1^f))|0)+(f&b)|0)+(OQ(f,7)^t)|0)|0)+l|0)&o,C=k&E,l=(OQ(k,30)^OQ(k,19)^OQ(k,10))+(w^C)|0,Q=OQ(t=Q+t|0,26)^OQ(t,21),u=k&(c=(Q=((((c+J|0)+(b&(-1^t))|0)+(f&t)|0)+(OQ(t,7)^Q)|0)-2132889090|0)+l|0),l=(OQ(c,30)^OQ(c,19)^OQ(c,10))+(u^C^c&E)|0,Q=OQ(C=Q+n|0,26)^OQ(C,21),Q=((((b+U|0)+(f&(-1^C))|0)+(t&C)|0)+(OQ(C,7)^Q)|0)-1680079193|0,n=(OQ(l=Q+l|0,30)^OQ(l,19)^OQ(l,10))+(u^(k^c)&l)|0,Q=OQ(b=Q+o|0,26)^OQ(b,21),n=(Q=((((f+T|0)+((-1^b)&t)|0)+(b&C)|0)+(OQ(b,7)^Q)|0)-1046744716|0)+n|0,o=Q+E|0,Q=16;f=((I=8&Q)<<2)+i|0,u=a[(w=((9&(y=1|Q))<<2)+i|0)>>2],E=a[f>>2]+(a[((Q+9&9)<<2)+i>>2]+(OQ(u,25)^OQ(u,14)^u>>>3)|0)|0,E=(OQ(M=a[((Q+14&14)<<2)+i>>2],15)^OQ(M,13)^M>>>10)+E|0,a[f>>2]=E,h=(g=(g=u+a[((Q+10&10)<<2)+i>>2]|0)+(OQ(u=a[f+8>>2],25)^OQ(u,14)^u>>>3)|0)+(OQ(u=a[((Q-1&15)<<2)+i>>2],15)^OQ(u,13)^u>>>10)|0,a[w>>2]=h,U=a[(w=((10&(J=2|Q))<<2)+i|0)>>2]+(a[((Q+11&11)<<2)+i>>2]+(OQ(E,15)^OQ(E,13)^E>>>10)|0)|0,g=w,U=(OQ(w=a[f+12>>2],25)^OQ(w,14)^w>>>3)+U|0,a[g>>2]=U,g=((11&(T=3|Q))<<2)+i|0,w=a[f+16>>2],D=g,g=(g=w=a[g>>2]+(a[((Q+12&12)<<2)+i>>2]+(OQ(w,25)^OQ(w,14)^w>>>3)|0)|0)+(OQ(w=a[f+4>>2],15)^OQ(w,13)^w>>>10)|0,a[D>>2]=g,d=((12&(F=4|Q))<<2)+i|0,w=a[f+20>>2],w=(D=w=a[d>>2]+(a[((Q+13&13)<<2)+i>>2]+(OQ(w,25)^OQ(w,14)^w>>>3)|0)|0)+(OQ(w=a[f+8>>2],15)^OQ(w,13)^w>>>10)|0,a[d>>2]=w,M=(D=M=(D=M+a[(m=((13&(d=5|Q))<<2)+i|0)>>2]|0)+(OQ(M=a[f+24>>2],25)^OQ(M,14)^M>>>3)|0)+(OQ(M=a[f+12>>2],15)^OQ(M,13)^M>>>10)|0,a[m>>2]=M,u=(V=u+a[(D=((14&(m=6|Q))<<2)+i|0)>>2]|0)+(OQ(u=a[f+28>>2],25)^OQ(u,14)^u>>>3)|0,V=D,D=(D=u)+(OQ(u=a[f+16>>2],15)^OQ(u,13)^u>>>10)|0,a[V>>2]=D,S=((15&(V=7|Q))<<2)+i|0,u=a[((8^I)<<2)+i>>2],u=(E+a[S>>2]|0)+(OQ(u,25)^OQ(u,14)^u>>>3)|0,u=(OQ(f=a[f+20>>2],15)^OQ(f,13)^f>>>10)+u|0,a[S>>2]=u,f=E+(a[90032+(Q<<2)>>2]+((((b&o)+(OQ(o,26)^OQ(o,21)^OQ(o,7))|0)+t|0)+((-1^o)&C)|0)|0)|0,I=(E=n&l)^(t=f+((OQ(n,30)^OQ(n,19)^OQ(n,10))+(E^(n^l)&c)|0)|0)&l,k=f+k|0,I=(E=t&n)^n&(C=(I=(OQ(t,30)^OQ(t,19)^OQ(t,10))+(I^E)|0)+(f=h+((a[90032+(y<<2)>>2]+((((-1^k)&b)+C|0)+(k&o)|0)|0)+(OQ(k,26)^OQ(k,21)^OQ(k,7))|0)|0)|0),c=f+c|0,U=(E=t&C)^t&(b=(I=(OQ(C,30)^OQ(C,19)^OQ(C,10))+(I^E)|0)+(f=(((U+(a[90032+(J<<2)>>2]+b|0)|0)+((-1^c)&o)|0)+(k&c)|0)+(OQ(c,26)^OQ(c,21)^OQ(c,7))|0)|0),l=f+l|0,g=(E=b&C)^(f=(U=(OQ(b,30)^OQ(b,19)^OQ(b,10))+(U^E)|0)+(o=(((g+(a[90032+(T<<2)>>2]+o|0)|0)+(k&(-1^l))|0)+(c&l)|0)+(OQ(l,26)^OQ(l,21)^OQ(l,7))|0)|0)&C,n=n+o|0,w=(E=f&b)^b&(k=(g=(OQ(f,30)^OQ(f,19)^OQ(f,10))+(g^E)|0)+(o=(((w+(k+a[90032+(F<<2)>>2]|0)|0)+(c&(-1^n))|0)+(n&l)|0)+(OQ(n,26)^OQ(n,21)^OQ(n,7))|0)|0),t=t+o|0,w=(E=f&k)^f&(c=(w=(OQ(k,30)^OQ(k,19)^OQ(k,10))+(w^E)|0)+(o=(((M+(c+a[90032+(d<<2)>>2]|0)|0)+(l&(-1^t))|0)+(t&n)|0)+(OQ(t,26)^OQ(t,21)^OQ(t,7))|0)|0),E=k&c,w=(OQ(c,30)^OQ(c,19)^OQ(c,10))+(w^E)|0,C=C+o|0,b=b+(o=(((D+(l+a[90032+(m<<2)>>2]|0)|0)+(n&(-1^C))|0)+(t&C)|0)+(OQ(C,26)^OQ(C,21)^OQ(C,7))|0)|0,n=(w=(OQ(l=w+o|0,30)^OQ(l,19)^OQ(l,10))+(E^(k^c)&l)|0)+(o=(((u+(n+a[90032+(V<<2)>>2]|0)|0)+((-1^b)&t)|0)+(b&C)|0)+(OQ(b,26)^OQ(b,21)^OQ(b,7))|0)|0,o=f+o|0,f=Q>>>0<56,Q=Q+8|0,f;);if(X=t+X|0,a[A+28>>2]=X,R=C+R|0,a[A+24>>2]=R,W=b+W|0,a[A+20>>2]=W,G=o+G|0,a[A+16>>2]=G,L=k+L|0,a[A+12>>2]=L,Y=c+Y|0,a[A+8>>2]=Y,Z=l+Z|0,a[A+4>>2]=Z,N=n+N|0,a[A>>2]=N,r=r- -64|0,!(e=e-1|0))break}}function Z(A,r,e){var i=0,Q=0,f=0,B=0,t=0,b=0,k=0,n=0,c=0,C=0,E=0,o=0,l=0,w=0,g=0,u=0,M=0,s=0,D=0,I=0,G=0,U=0,F=0,Z=0,d=0,W=0,V=0,Y=0,R=0,y=0,h=0,J=0;Q=Zi(a[e>>2],0,a[r>>2],0),i=N,a[A>>2]=Q,B=Zi(a[e>>2],0,a[r+4>>2],0),t=N,Q=i,f=Zi(a[e+4>>2],0,a[r>>2],0)+i|0,i=N,i=Q>>>0>f>>>0?i+1|0:i,Q=f,f=i,i=t,i=(B=Q+B|0)>>>0<Q>>>0?i+1|0:i,a[A+4>>2]=B,b=Zi(a[e+8>>2],0,a[r>>2],0),k=N,t=Zi(a[e+4>>2],0,a[r+4>>2],0),n=N,w=i,c=f+i|0,Q=Zi(a[e>>2],0,a[r+8>>2],0),i=N,B=f=c+Q|0,f=Q>>>0>f>>>0?i+1|0:i,i=n,t=Q=B+t|0,Q=i=Q>>>0<B>>>0?i+1|0:i,i=k,i=(b=t+b|0)>>>0<t>>>0?i+1|0:i,a[A+8>>2]=b,C=Zi(a[e>>2],0,a[r+12>>2],0),o=N,k=Zi(a[e+4>>2],0,a[r+8>>2],0),E=N,g=Zi(a[e+8>>2],0,a[r+4>>2],0),M=N,u=f,l=i+(w=Q+(n=f+(c>>>0<w>>>0)|0)|0)|0,Q=Zi(a[e+12>>2],0,a[r>>2],0),i=N,t=f=l+Q|0,f=Q>>>0>f>>>0?i+1|0:i,i=M,b=Q=t+g|0,Q=i=Q>>>0<t>>>0?i+1|0:i,i=E,k=B=b+k|0,B=i=B>>>0<b>>>0?i+1|0:i,i=o,i=(c=k+C|0)>>>0<k>>>0?i+1|0:i,a[A+12>>2]=c,g=Zi(a[e+16>>2],0,a[r>>2],0),M=N,s=Zi(a[e+12>>2],0,a[r+4>>2],0),D=N,I=Zi(a[e+8>>2],0,a[r+8>>2],0),U=N,G=Zi(a[e+4>>2],0,a[r+12>>2],0),Z=N,C=n>>>0<u>>>0,l=i+(E=B+(o=Q+(C=f+(u=(w>>>0>l>>>0)+(n>>>0>w>>>0?C?2:1:C)|0)|0)|0)|0)|0,Q=Zi(a[e>>2],0,a[r+16>>2],0),i=N,b=f=l+Q|0,f=Q>>>0>f>>>0?i+1|0:i,i=Z,k=Q=b+G|0,Q=i=Q>>>0<b>>>0?i+1|0:i,i=U,n=B=k+I|0,B=i=B>>>0<k>>>0?i+1|0:i,i=D,w=t=n+s|0,t=i=t>>>0<n>>>0?i+1|0:i,i=M,i=(c=w+g|0)>>>0<w>>>0?i+1|0:i,a[A+16>>2]=c,s=Zi(a[e>>2],0,a[r+20>>2],0),D=N,I=Zi(a[e+4>>2],0,a[r+16>>2],0),U=N,G=Zi(a[e+8>>2],0,a[r+12>>2],0),Z=N,d=Zi(a[e+12>>2],0,a[r+8>>2],0),W=N,V=Zi(a[e+16>>2],0,a[r+4>>2],0),Y=N,g=C>>>0<u>>>0,F=i+(M=t+(g=B+(E=Q+(o=f+(l=((E>>>0<o>>>0)+(C>>>0>o>>>0?g?2:1:g)|0)+(E>>>0>l>>>0)|0)|0)|0)|0)|0)|0,Q=Zi(a[e+20>>2],0,a[r>>2],0),i=N,k=f=F+Q|0,f=Q>>>0>f>>>0?i+1|0:i,i=Y,n=Q=k+V|0,Q=i=Q>>>0<k>>>0?i+1|0:i,i=W,w=B=n+d|0,B=i=B>>>0<n>>>0?i+1|0:i,i=Z,c=t=w+G|0,t=i=t>>>0<w>>>0?i+1|0:i,i=U,C=b=c+I|0,b=i=b>>>0<c>>>0?i+1|0:i,i=D,i=(u=C+s|0)>>>0<C>>>0?i+1|0:i,a[A+20>>2]=u,I=Zi(a[e+24>>2],0,a[r>>2],0),U=N,G=Zi(a[e+20>>2],0,a[r+4>>2],0),Z=N,d=Zi(a[e+16>>2],0,a[r+8>>2],0),W=N,V=Zi(a[e+12>>2],0,a[r+12>>2],0),Y=N,u=Zi(a[e+8>>2],0,a[r+16>>2],0),R=N,y=Zi(a[e+4>>2],0,a[r+20>>2],0),h=N,l=o>>>0<l>>>0,J=i+(D=b+(s=t+(l=B+(M=Q+(g=f+(F=(((E>>>0>g>>>0)+(E>>>0<o>>>0?l?2:1:l)|0)+(g>>>0>M>>>0)|0)+(M>>>0>F>>>0)|0)|0)|0)|0)|0)|0)|0,Q=Zi(a[e>>2],0,a[r+24>>2],0),i=N,n=f=J+Q|0,f=Q>>>0>f>>>0?i+1|0:i,i=h,w=Q=n+y|0,Q=i=Q>>>0<n>>>0?i+1|0:i,i=R,c=B=w+u|0,B=i=B>>>0<w>>>0?i+1|0:i,i=Y,C=t=c+V|0,t=i=t>>>0<c>>>0?i+1|0:i,i=W,o=b=C+d|0,b=i=b>>>0<C>>>0?i+1|0:i,i=Z,E=k=o+G|0,k=i=k>>>0<o>>>0?i+1|0:i,i=U,i=(u=E+I|0)>>>0<E>>>0?i+1|0:i,a[A+24>>2]=u,Z=Zi(a[e>>2],0,a[r+28>>2],0),d=N,W=Zi(a[e+4>>2],0,a[r+24>>2],0),V=N,Y=Zi(a[e+8>>2],0,a[r+20>>2],0),u=N,R=Zi(a[e+12>>2],0,a[r+16>>2],0),y=N,h=Zi(a[e+16>>2],0,a[r+12>>2],0),E=N,C=Zi(a[e+20>>2],0,a[r+8>>2],0),o=N,n=Zi(a[e+24>>2],0,a[r+4>>2],0),c=N,I=g>>>0<F>>>0,J=i+(G=k+(U=b+(I=t+(D=B+(s=Q+(l=f+(F=((((l>>>0<M>>>0)+(g>>>0>M>>>0?I?2:1:I)|0)+(l>>>0>s>>>0)|0)+(s>>>0>D>>>0)|0)+(D>>>0>J>>>0)|0)|0)|0)|0)|0)|0)|0)|0,Q=Zi(a[e+28>>2],0,a[r>>2],0),i=N,w=f=J+Q|0,f=Q>>>0>f>>>0?i+1|0:i,i=c,c=Q=n+w|0,Q=i=Q>>>0<w>>>0?i+1|0:i,i=o,C=B=C+c|0,B=i=B>>>0<c>>>0?i+1|0:i,i=E,o=t=C+h|0,t=i=t>>>0<C>>>0?i+1|0:i,i=y,E=b=o+R|0,b=i=b>>>0<o>>>0?i+1|0:i,i=u,g=k=E+Y|0,k=i=k>>>0<E>>>0?i+1|0:i,i=V,M=n=g+W|0,n=i=n>>>0<g>>>0?i+1|0:i,i=d,i=(u=M+Z|0)>>>0<M>>>0?i+1|0:i,a[A+28>>2]=u,Z=Zi(a[e+28>>2],0,a[r+4>>2],0),d=N,W=Zi(a[e+24>>2],0,a[r+8>>2],0),V=N,Y=Zi(a[e+20>>2],0,a[r+12>>2],0),u=N,R=Zi(a[e+16>>2],0,a[r+16>>2],0),y=N,h=Zi(a[e+12>>2],0,a[r+20>>2],0),E=N,C=Zi(a[e+8>>2],0,a[r+24>>2],0),o=N,F=l>>>0<F>>>0,F=i+(M=n+(g=k+(U=b+(I=t+(D=B+(s=Q+(l=f+(G=(((((s>>>0>D>>>0)+(l>>>0>s>>>0?F?2:1:F)|0)+(D>>>0>I>>>0)|0)+(I>>>0>U>>>0)|0)+(G>>>0<U>>>0)|0)+(G>>>0>J>>>0)|0)|0)|0)|0)|0)|0)|0)|0)|0,Q=Zi(a[e+4>>2],0,a[r+28>>2],0),i=N,n=f=F+Q|0,f=Q>>>0>f>>>0?i+1|0:i,i=o,w=Q=n+C|0,Q=i=Q>>>0<n>>>0?i+1|0:i,i=E,c=B=w+h|0,B=i=B>>>0<w>>>0?i+1|0:i,i=y,C=t=c+R|0,t=i=t>>>0<c>>>0?i+1|0:i,i=u,o=b=C+Y|0,b=i=b>>>0<C>>>0?i+1|0:i,i=V,E=k=o+W|0,k=i=k>>>0<o>>>0?i+1|0:i,i=d,i=(u=E+Z|0)>>>0<E>>>0?i+1|0:i,a[A+32>>2]=u,Z=Zi(a[e+8>>2],0,a[r+28>>2],0),d=N,W=Zi(a[e+12>>2],0,a[r+24>>2],0),V=N,Y=Zi(a[e+16>>2],0,a[r+20>>2],0),u=N,R=Zi(a[e+20>>2],0,a[r+16>>2],0),y=N,h=Zi(a[e+24>>2],0,a[r+12>>2],0),J=N,G=l>>>0<G>>>0,I=i+(E=k+(o=b+(s=t+(l=B+(M=Q+(g=f+(D=((((((s>>>0>D>>>0)+(l>>>0>s>>>0?G?2:1:G)|0)+(D>>>0>I>>>0)|0)+(I>>>0>U>>>0)|0)+(g>>>0<U>>>0)|0)+(g>>>0>M>>>0)|0)+(M>>>0>F>>>0)|0)|0)|0)|0)|0)|0)|0)|0,Q=Zi(a[e+28>>2],0,a[r+8>>2],0),i=N,k=f=I+Q|0,f=Q>>>0>f>>>0?i+1|0:i,i=J,n=Q=k+h|0,Q=i=Q>>>0<k>>>0?i+1|0:i,i=y,w=B=n+R|0,B=i=B>>>0<n>>>0?i+1|0:i,i=u,c=t=w+Y|0,t=i=t>>>0<w>>>0?i+1|0:i,i=V,C=b=c+W|0,b=i=b>>>0<c>>>0?i+1|0:i,i=d,i=(u=C+Z|0)>>>0<C>>>0?i+1|0:i,a[A+36>>2]=u,U=Zi(a[e+28>>2],0,a[r+12>>2],0),G=N,Z=Zi(a[e+24>>2],0,a[r+16>>2],0),d=N,W=Zi(a[e+20>>2],0,a[r+20>>2],0),V=N,Y=Zi(a[e+16>>2],0,a[r+24>>2],0),F=N,D=g>>>0<D>>>0,u=i+(C=b+(c=t+(g=B+(E=Q+(o=f+(l=(((((l>>>0<M>>>0)+(g>>>0>M>>>0?D?2:1:D)|0)+(l>>>0>s>>>0)|0)+(o>>>0<s>>>0)|0)+(E>>>0<o>>>0)|0)+(E>>>0>I>>>0)|0)|0)|0)|0)|0)|0)|0,Q=Zi(a[e+12>>2],0,a[r+28>>2],0),i=N,b=f=u+Q|0,f=Q>>>0>f>>>0?i+1|0:i,i=F,k=Q=b+Y|0,Q=i=Q>>>0<b>>>0?i+1|0:i,i=V,n=B=k+W|0,B=i=B>>>0<k>>>0?i+1|0:i,i=d,w=t=n+Z|0,t=i=t>>>0<n>>>0?i+1|0:i,i=G,i=(M=w+U|0)>>>0<w>>>0?i+1|0:i,a[A+40>>2]=M,s=Zi(a[e+16>>2],0,a[r+28>>2],0),D=N,I=Zi(a[e+20>>2],0,a[r+24>>2],0),U=N,G=Zi(a[e+24>>2],0,a[r+20>>2],0),Z=N,l=o>>>0<l>>>0,g=i+(w=t+(n=B+(C=Q+(c=f+(E=((((E>>>0>g>>>0)+(E>>>0<o>>>0?l?2:1:l)|0)+(c>>>0<g>>>0)|0)+(C>>>0<c>>>0)|0)+(C>>>0>u>>>0)|0)|0)|0)|0)|0)|0,Q=Zi(a[e+28>>2],0,a[r+16>>2],0),i=N,t=f=g+Q|0,f=Q>>>0>f>>>0?i+1|0:i,i=Z,b=Q=t+G|0,Q=i=Q>>>0<t>>>0?i+1|0:i,i=U,k=B=b+I|0,B=i=B>>>0<b>>>0?i+1|0:i,i=D,i=(o=k+s|0)>>>0<k>>>0?i+1|0:i,a[A+44>>2]=o,M=Zi(a[e+28>>2],0,a[r+20>>2],0),u=N,l=Zi(a[e+24>>2],0,a[r+24>>2],0),s=N,E=c>>>0<E>>>0,C=i+(k=B+(b=Q+(n=f+(c=(((n>>>0<C>>>0)+(C>>>0<c>>>0?E?2:1:E)|0)+(n>>>0>w>>>0)|0)+(w>>>0>g>>>0)|0)|0)|0)|0)|0,Q=Zi(a[e+20>>2],0,a[r+28>>2],0),i=N,B=f=C+Q|0,f=Q>>>0>f>>>0?i+1|0:i,i=s,t=Q=B+l|0,Q=i=Q>>>0<B>>>0?i+1|0:i,i=u,i=(w=t+M|0)>>>0<t>>>0?i+1|0:i,a[A+48>>2]=w,o=Zi(a[e+24>>2],0,a[r+28>>2],0),E=N,c=n>>>0<c>>>0,n=i+(t=Q+(B=f+(k=((b>>>0>k>>>0)+(b>>>0<n>>>0?c?2:1:c)|0)+(k>>>0>C>>>0)|0)|0)|0)|0,Q=Zi(a[e+28>>2],0,a[r+24>>2],0),i=N,i=Q>>>0>(f=n+Q|0)>>>0?i+1|0:i,Q=f,f=i,i=E,i=(b=Q+o|0)>>>0<Q>>>0?i+1|0:i,a[A+52>>2]=b,k=B>>>0<k>>>0,Q=i+(f=f+(B=(t>>>0>n>>>0)+(B>>>0>t>>>0?k?2:1:k)|0)|0)|0,e=Zi(a[e+28>>2],0,a[r+28>>2],0),i=N,i=(r=Q+e|0)>>>0<e>>>0?i+1|0:i,a[A+56>>2]=r,e=f>>>0<B>>>0,a[A+60>>2]=(Q>>>0<f>>>0?e?2:1:e)+i}function d(A,r){var e=0,i=0,Q=0,f=0,B=0,t=0,b=0,k=0,n=0,c=0,C=0,E=0,o=0,l=0,w=0,g=0,u=0,M=0,s=0,D=0,I=0,G=0,U=0,F=0,Z=0,d=0,W=0,V=0;f=Zi(e=a[r>>2],0,e,0),e=N,a[A>>2]=f,b=Zi(a[r>>2],0,a[r+4>>2],0),B=i=N,f=e,Q=e+b|0,e=i,i=e=Q>>>0<f>>>0?e+1|0:e,e=B,e=(Q=Q+b|0)>>>0<b>>>0?e+1|0:e,a[A+4>>2]=Q,b=Zi(a[r>>2],0,a[r+8>>2],0),t=Q=N,w=e,n=i+e|0,f=Zi(e=a[r+4>>2],0,e,0),e=N,B=i=n+f|0,i=i>>>0<f>>>0?e+1|0:e,e=Q,Q=e=(f=b+B|0)>>>0<B>>>0?e+1|0:e,e=t,e=(f=b+f|0)>>>0<b>>>0?e+1|0:e,a[A+8>>2]=f,b=Zi(a[r+4>>2],0,a[r+8>>2],0),k=f=N,t=Zi(a[r>>2],0,a[r+12>>2],0),g=i,o=e+(n=Q+(w=i+(w>>>0>n>>>0)|0)|0)|0,e=f=N,Q=(i=t+(Q=o)|0)>>>0<Q>>>0?e+1|0:e,e=(i=t+i|0)>>>0<t>>>0?e+1|0:e,t=i,f=e,e=k,B=e=(i=b+i|0)>>>0<t>>>0?e+1|0:e,e=k,e=(i=b+i|0)>>>0<b>>>0?e+1|0:e,a[A+12>>2]=i,b=Zi(a[r>>2],0,a[r+16>>2],0),k=i=N,i=Zi(a[r+4>>2],0,a[r+12>>2],0),l=c=N,c=w>>>0<g>>>0,M=e+(s=B+(o=f+(g=Q+(C=(n>>>0>o>>>0)+(w>>>0>n>>>0?c?2:1:c)|0)|0)|0)|0)|0,f=Zi(e=a[r+8>>2],0,e,0),e=N,c=Q=M+f|0,Q=Q>>>0<f>>>0?e+1|0:e,e=l,u=f=i+c|0,f=e=f>>>0<c>>>0?e+1|0:e,e=l,l=B=i+u|0,B=e=B>>>0<i>>>0?e+1|0:e,e=k,t=e=(i=b+l|0)>>>0<l>>>0?e+1|0:e,e=k,e=(i=b+i|0)>>>0<b>>>0?e+1|0:e,a[A+16>>2]=i,b=Zi(a[r+8>>2],0,a[r+12>>2],0),n=i=N,i=Zi(a[r+4>>2],0,a[r+16>>2],0),E=k=N,w=Zi(a[r>>2],0,a[r+20>>2],0),C=C>>>0>g>>>0,W=e+(D=t+(C=B+(o=f+(g=Q+(I=((s>>>0<o>>>0)+(o>>>0<g>>>0?C?2:1:C)|0)+(s>>>0>M>>>0)|0)|0)|0)|0)|0)|0,e=k=N,f=(Q=w+(f=W)|0)>>>0<f>>>0?e+1|0:e,l=Q=w+Q|0,B=e=Q>>>0<w>>>0?e+1|0:e,e=E,t=e=(Q=i+Q|0)>>>0<l>>>0?e+1|0:e,e=E,k=e=(Q=i+Q|0)>>>0<i>>>0?e+1|0:e,e=n,c=e=(i=b+Q|0)>>>0<Q>>>0?e+1|0:e,e=n,e=(i=b+i|0)>>>0<b>>>0?e+1|0:e,a[A+20>>2]=i,b=Zi(a[r>>2],0,a[r+24>>2],0),u=i=N,i=Zi(a[r+4>>2],0,a[r+20>>2],0),s=Q=N,Q=Zi(a[r+8>>2],0,a[r+16>>2],0),E=n=N,n=g>>>0<I>>>0,F=e+(Z=c+(I=k+(M=t+(C=B+(G=f+(d=(((o>>>0>C>>>0)+(o>>>0<g>>>0?n?2:1:n)|0)+(C>>>0>D>>>0)|0)+(D>>>0>W>>>0)|0)|0)|0)|0)|0)|0)|0,B=Zi(e=a[r+12>>2],0,e,0),e=N,n=f=F+B|0,f=f>>>0<B>>>0?e+1|0:e,e=E,U=B=Q+n|0,B=e=B>>>0<n>>>0?e+1|0:e,e=E,E=t=Q+U|0,t=e=t>>>0<Q>>>0?e+1|0:e,e=s,k=e=(Q=i+E|0)>>>0<E>>>0?e+1|0:e,e=s,c=e=(Q=i+Q|0)>>>0<i>>>0?e+1|0:e,e=u,l=e=(i=b+Q|0)>>>0<Q>>>0?e+1|0:e,e=u,e=(i=b+i|0)>>>0<b>>>0?e+1|0:e,a[A+24>>2]=i,b=Zi(a[r+12>>2],0,a[r+16>>2],0),g=i=N,i=Zi(a[r+8>>2],0,a[r+20>>2],0),o=Q=N,Q=Zi(a[r+4>>2],0,a[r+24>>2],0),V=w=N,D=w,u=Zi(a[r>>2],0,a[r+28>>2],0),d=G>>>0<d>>>0,d=e+(I=l+(U=c+(Z=k+(M=t+(C=B+(G=f+(F=((((C>>>0>M>>>0)+(C>>>0<G>>>0?d?2:1:d)|0)+(M>>>0>I>>>0)|0)+(I>>>0>Z>>>0)|0)+(Z>>>0>F>>>0)|0)|0)|0)|0)|0)|0)|0)|0,e=w=N,W=f=u+(B=d)|0,f=f>>>0<B>>>0?e+1|0:e,n=B=u+W|0,B=e=B>>>0<u>>>0?e+1|0:e,e=D,D=t=Q+n|0,t=e=t>>>0<n>>>0?e+1|0:e,e=V,E=k=Q+D|0,k=e=k>>>0<Q>>>0?e+1|0:e,e=o,c=e=(Q=i+E|0)>>>0<E>>>0?e+1|0:e,e=o,l=e=(Q=i+Q|0)>>>0<i>>>0?e+1|0:e,e=g,w=e=(i=b+Q|0)>>>0<Q>>>0?e+1|0:e,e=g,e=(i=i+b|0)>>>0<b>>>0?e+1|0:e,a[A+28>>2]=i,b=Zi(a[r+4>>2],0,a[r+28>>2],0),u=i=N,i=Zi(a[r+8>>2],0,a[r+24>>2],0),g=Q=N,Q=Zi(a[r+12>>2],0,a[r+20>>2],0),V=o=N,F=G>>>0<F>>>0,d=e+(I=w+(U=l+(Z=c+(M=k+(D=t+(C=B+(G=f+(F=(((((C>>>0>M>>>0)+(C>>>0<G>>>0?F?2:1:F)|0)+(M>>>0>Z>>>0)|0)+(Z>>>0>U>>>0)|0)+(I>>>0<U>>>0)|0)+(I>>>0>d>>>0)|0)|0)|0)|0)|0)|0)|0)|0)|0,B=Zi(e=a[r+16>>2],0,e,0),e=N,w=f=d+B|0,f=f>>>0<B>>>0?e+1|0:e,e=o,o=B=Q+w|0,B=e=B>>>0<w>>>0?e+1|0:e,e=V,n=t=Q+o|0,Q=e=t>>>0<Q>>>0?e+1|0:e,e=g,W=t=i+t|0,t=e=t>>>0<n>>>0?e+1|0:e,e=g,E=k=i+W|0,k=e=k>>>0<i>>>0?e+1|0:e,e=u,c=e=(i=b+E|0)>>>0<E>>>0?e+1|0:e,e=u,e=(i=b+i|0)>>>0<b>>>0?e+1|0:e,a[A+32>>2]=i,b=Zi(a[r+16>>2],0,a[r+20>>2],0),V=i=N,u=i,i=Zi(a[r+12>>2],0,a[r+24>>2],0),g=l=N,s=Zi(a[r+8>>2],0,a[r+28>>2],0),F=G>>>0<F>>>0,I=e+(M=c+(E=k+(C=t+(G=Q+(o=B+(D=f+(U=((((((C>>>0>D>>>0)+(C>>>0<G>>>0?F?2:1:F)|0)+(M>>>0<D>>>0)|0)+(M>>>0>Z>>>0)|0)+(Z>>>0>U>>>0)|0)+(I>>>0<U>>>0)|0)+(I>>>0>d>>>0)|0)|0)|0)|0)|0)|0)|0)|0,e=l=N,Z=Q=s+(f=I)|0,Q=Q>>>0<f>>>0?e+1|0:e,c=f=s+Z|0,f=e=f>>>0<s>>>0?e+1|0:e,e=g,s=B=i+c|0,B=e=B>>>0<c>>>0?e+1|0:e,e=g,l=t=i+s|0,t=e=t>>>0<i>>>0?e+1|0:e,e=u,k=e=(i=b+l|0)>>>0<l>>>0?e+1|0:e,e=V,e=(i=b+i|0)>>>0<b>>>0?e+1|0:e,a[A+36>>2]=i,b=Zi(a[r+12>>2],0,a[r+28>>2],0),w=i=N,i=Zi(a[r+16>>2],0,a[r+24>>2],0),W=n=N,U=D>>>0<U>>>0,M=e+(u=k+(D=t+(s=B+(o=f+(E=Q+(C=(((((o>>>0>G>>>0)+(o>>>0<D>>>0?U?2:1:U)|0)+(C>>>0<G>>>0)|0)+(E>>>0<C>>>0)|0)+(E>>>0>M>>>0)|0)+(M>>>0>I>>>0)|0)|0)|0)|0)|0)|0)|0,f=Zi(e=a[r+20>>2],0,e,0),e=N,k=Q=M+f|0,Q=Q>>>0<f>>>0?e+1|0:e,e=n,n=f=i+k|0,f=e=f>>>0<k>>>0?e+1|0:e,e=W,c=B=i+n|0,i=e=B>>>0<i>>>0?e+1|0:e,e=w,g=B=b+B|0,B=e=B>>>0<c>>>0?e+1|0:e,e=w,e=(t=b+g|0)>>>0<b>>>0?e+1|0:e,a[A+40>>2]=t,b=Zi(a[r+20>>2],0,a[r+24>>2],0),w=t=N,l=Zi(a[r+16>>2],0,a[r+28>>2],0),C=E>>>0<C>>>0,g=e+(E=B+(c=i+(n=f+(k=Q+(u=((((s>>>0<o>>>0)+(E>>>0>o>>>0?C?2:1:C)|0)+(s>>>0>D>>>0)|0)+(u>>>0<D>>>0)|0)+(u>>>0>M>>>0)|0)|0)|0)|0)|0)|0,e=t=N,s=i=l+(Q=g)|0,i=i>>>0<Q>>>0?e+1|0:e,B=Q=l+s|0,Q=e=Q>>>0<l>>>0?e+1|0:e,e=w,l=f=b+B|0,f=e=f>>>0<B>>>0?e+1|0:e,e=w,e=(t=b+l|0)>>>0<b>>>0?e+1|0:e,a[A+44>>2]=t,b=Zi(a[r+20>>2],0,a[r+28>>2],0),o=t=N,u=k>>>0<u>>>0,E=e+(l=f+(c=Q+(k=i+(n=(((n>>>0>c>>>0)+(k>>>0>n>>>0?u?2:1:u)|0)+(c>>>0>E>>>0)|0)+(E>>>0>g>>>0)|0)|0)|0)|0)|0,Q=Zi(e=a[r+24>>2],0,e,0),e=N,f=i=E+Q|0,i=i>>>0<Q>>>0?e+1|0:e,e=t,t=Q=f+b|0,Q=e=Q>>>0<f>>>0?e+1|0:e,e=o,e=(B=b+t|0)>>>0<b>>>0?e+1|0:e,a[A+48>>2]=B,B=Zi(a[r+24>>2],0,a[r+28>>2],0),s=b=N,n=k>>>0<n>>>0,c=e+(Q=Q+(i=i+(k=((c>>>0>l>>>0)+(k>>>0>c>>>0?n?2:1:n)|0)+(E>>>0<l>>>0)|0)|0)|0)|0,e=b,b=e=(f=(t=c)+B|0)>>>0<t>>>0?e+1|0:e,e=s,e=(t=f+B|0)>>>0<B>>>0?e+1|0:e,a[A+52>>2]=t,t=i>>>0<k>>>0,Q=e+(b=b+(i=(Q>>>0>c>>>0)+(i>>>0>Q>>>0?t?2:1:t)|0)|0)|0,f=Zi(r=a[r+28>>2],0,r,0),e=N,e=(r=Q+f|0)>>>0<f>>>0?e+1|0:e,a[A+56>>2]=r,i=i>>>0>b>>>0,a[A+60>>2]=(Q>>>0<b>>>0?i?2:1:i)+e}function W(A,r,e){var i=0,Q=0,f=0,t=0,b=0,k=0,n=0,c=0,C=0,E=0,o=0,l=0,w=0,g=0,u=0,M=0,s=0,D=0,I=0,N=0,G=0,U=0;if(e)for(n=a[A>>2],Q=a[A+4>>2],i=a[A+8>>2],t=a[A+12>>2];c=B[r+16|0]|B[r+17|0]<<8|B[r+18|0]<<16|B[r+19|0]<<24,o=B[r+32|0]|B[r+33|0]<<8|B[r+34|0]<<16|B[r+35|0]<<24,l=B[r+48|0]|B[r+49|0]<<8|B[r+50|0]<<16|B[r+51|0]<<24,w=B[r+36|0]|B[r+37|0]<<8|B[r+38|0]<<16|B[r+39|0]<<24,g=B[r+52|0]|B[r+53|0]<<8|B[r+54|0]<<16|B[r+55|0]<<24,u=B[r+4|0]|B[r+5|0]<<8|B[r+6|0]<<16|B[r+7|0]<<24,C=B[r+20|0]|B[r+21|0]<<8|B[r+22|0]<<16|B[r+23|0]<<24,f=OQ(((E=B[0|r]|B[r+1|0]<<8|B[r+2|0]<<16|B[r+3|0]<<24)+(((i^t)&Q^t)+n|0)|0)-680876936|0,7)+Q|0,n=B[r+12|0]|B[r+13|0]<<8|B[r+14|0]<<16|B[r+15|0]<<24,M=B[r+8|0]|B[r+9|0]<<8|B[r+10|0]<<16|B[r+11|0]<<24,t=OQ(((t+u|0)+(f&(i^Q)^i)|0)-389564586|0,12)+f|0,i=OQ(606105819+((M+i|0)+(t&(Q^f)^Q)|0)|0,17)+t|0,k=OQ(((Q+n|0)+(f^i&(f^t))|0)-1044525330|0,22)+i|0,Q=OQ(((f+c|0)+(t^k&(i^t))|0)-176418897|0,7)+k|0,s=B[r+28|0]|B[r+29|0]<<8|B[r+30|0]<<16|B[r+31|0]<<24,f=i+(D=B[r+24|0]|B[r+25|0]<<8|B[r+26|0]<<16|B[r+27|0]<<24)|0,i=OQ(1200080426+((t+C|0)+(i^Q&(i^k))|0)|0,12)+Q|0,f=OQ((f+(k^i&(Q^k))|0)-1473231341|0,17)+i|0,t=OQ(((k+s|0)+(Q^f&(i^Q))|0)-45705983|0,22)+f|0,Q=OQ(1770035416+((Q+o|0)+(i^t&(i^f))|0)|0,7)+t|0,k=B[r+44|0]|B[r+45|0]<<8|B[r+46|0]<<16|B[r+47|0]<<24,I=B[r+40|0]|B[r+41|0]<<8|B[r+42|0]<<16|B[r+43|0]<<24,i=OQ(((i+w|0)+(f^Q&(f^t))|0)-1958414417|0,12)+Q|0,f=OQ(((f+I|0)+(t^i&(Q^t))|0)-42063|0,17)+i|0,t=OQ(((t+k|0)+(Q^f&(i^Q))|0)-1990404162|0,22)+f|0,Q=OQ(1804603682+((Q+l|0)+(i^t&(i^f))|0)|0,7)+t|0,N=B[r+60|0]|B[r+61|0]<<8|B[r+62|0]<<16|B[r+63|0]<<24,U=Q+u|0,G=B[r+56|0]|B[r+57|0]<<8|B[r+58|0]<<16|B[r+59|0]<<24,b=OQ(((i+g|0)+(f^Q&(f^t))|0)-40341101|0,12)+Q|0,f=OQ(((f+G|0)+(t^b&(Q^t))|0)-1502002290|0,17)+b|0,Q=OQ(1236535329+((t+N|0)+(Q^f&(Q^b))|0)|0,22)+f|0,i=OQ((U+((f^Q)&b^f)|0)-165796510|0,5)+Q|0,t=f+k|0,f=OQ(((b+D|0)+(Q^f&(i^Q))|0)-1069501632|0,9)+i|0,t=OQ(643717713+(t+(i^Q&(f^i))|0)|0,14)+f|0,Q=OQ(((Q+E|0)+(f^i&(f^t))|0)-373897302|0,20)+t|0,i=OQ(((i+C|0)+((t^Q)&f^t)|0)-701558691|0,5)+Q|0,f=OQ(38016083+((f+I|0)+(Q^t&(i^Q))|0)|0,9)+i|0,t=OQ(((t+N|0)+(i^Q&(f^i))|0)-660478335|0,14)+f|0,Q=OQ(((Q+c|0)+(f^i&(f^t))|0)-405537848|0,20)+t|0,b=(i=OQ(568446438+((i+w|0)+((t^Q)&f^t)|0)|0,5)+Q|0)+g|0,f=OQ(((f+G|0)+(Q^t&(i^Q))|0)-1019803690|0,9)+i|0,t=OQ(((t+n|0)+(i^Q&(f^i))|0)-187363961|0,14)+f|0,i=OQ(1163531501+((Q+o|0)+(f^i&(f^t))|0)|0,20)+t|0,Q=OQ((b+((t^i)&f^t)|0)-1444681467|0,5)+i|0,f=OQ(((f+M|0)+(i^t&(i^Q))|0)-51403784|0,9)+Q|0,t=OQ(1735328473+((t+s|0)+(Q^i&(f^Q))|0)|0,14)+f|0,b=f+o|0,f=OQ(((U=i+l|0)+(f^(i=f^t)&Q)|0)-1926607734|0,20)+t|0,i=OQ(((Q+C|0)+(i^f)|0)-378558|0,4)+f|0,Q=OQ((b+(f^t^i)|0)-2022574463|0,11)+i|0,t=OQ(1839030562+((t+k|0)+(Q^i^f)|0)|0,16)+Q|0,f=OQ(((f+G|0)+(t^i^Q)|0)-35309556|0,23)+t|0,i=OQ(((i+u|0)+(f^Q^t)|0)-1530992060|0,4)+f|0,Q=OQ(1272893353+((Q+c|0)+(i^f^t)|0)|0,11)+i|0,t=OQ(((t+s|0)+(Q^i^f)|0)-155497632|0,16)+Q|0,f=OQ(((f+I|0)+(t^i^Q)|0)-1094730640|0,23)+t|0,i=OQ(681279174+((i+g|0)+(f^Q^t)|0)|0,4)+f|0,Q=OQ(((Q+E|0)+(i^f^t)|0)-358537222|0,11)+i|0,t=OQ(((t+n|0)+(Q^i^f)|0)-722521979|0,16)+Q|0,f=OQ(76029189+((f+D|0)+(t^i^Q)|0)|0,23)+t|0,b=(i=OQ(((i+w|0)+(f^Q^t)|0)-640364487|0,4)+f|0)+E|0,E=i^(Q=OQ(((Q+l|0)+(i^f^t)|0)-421815835|0,11)+i|0),i=OQ(530742520+((t+N|0)+(Q^i^f)|0)|0,16)+Q|0,t=OQ(((f+M|0)+(E^i)|0)-995338651|0,23)+i|0,f=OQ((b+((t|-1^Q)^i)|0)-198630844|0,6)+t|0,b=t+C|0,C=i+G|0,i=OQ(1126891415+((Q+s|0)+(t^(f|-1^i))|0)|0,10)+f|0,t=OQ((C+(f^(i|-1^t))|0)-1416354905|0,15)+i|0,Q=OQ((b+((t|-1^f)^i)|0)-57434055|0,21)+t|0,b=t+I|0,n=i+n|0,i=OQ(1700485571+((f+l|0)+(t^(Q|-1^i))|0)|0,6)+Q|0,t=OQ((n+(Q^(i|-1^t))|0)-1894986606|0,10)+i|0,f=OQ((b+((t|-1^Q)^i)|0)-1051523|0,15)+t|0,n=t+N|0,b=i+o|0,i=OQ(((Q+u|0)+(t^(f|-1^i))|0)-2054922799|0,21)+f|0,t=OQ(1873313359+(b+(f^(i|-1^t))|0)|0,6)+i|0,Q=OQ((n+((t|-1^f)^i)|0)-30611744|0,10)+t|0,n=t+c|0,c=i+g|0,i=OQ(((f+D|0)+(t^(Q|-1^i))|0)-1560198380|0,15)+Q|0,c=OQ(1309151649+(c+(Q^(i|-1^t))|0)|0,21)+i|0,n=(f=OQ((n+((c|-1^Q)^i)|0)-145523070|0,6)+c|0)+a[A>>2]|0,a[A>>2]=n,t=(Q=OQ(((Q+k|0)+(c^(f|-1^i))|0)-1120210379|0,10)+f|0)+a[A+12>>2]|0,a[A+12>>2]=t,i=(k=OQ(718787259+((i+M|0)+(f^(Q|-1^c))|0)|0,15)+Q|0)+a[A+8>>2]|0,a[A+8>>2]=i,Q=(k+a[A+4>>2]|0)+OQ(((c+w|0)+(Q^(k|-1^f))|0)-343485551|0,21)|0,a[A+4>>2]=Q,r=r- -64|0,e=e-1|0;);}function V(A,r,e,i,Q){var f,B=0,t=0,b=0,k=0,c=0,C=0,E=0,o=0,l=0,w=0,g=0,u=0,M=0,D=0,I=0,N=0,G=0,U=0,F=0;s=f=s-32|0,B=a[A+8>>2];A:if(t=a[B+56>>2])B=0|Ga[0|t](A,r,e,i,Q);else if(t=a[B+52>>2]){if(B=0,2!=(0|e))break A;B=0|Ga[0|t](A,r,i,Q)}else{r:if((0|r)<=511)Ve(4,129,120),B=0;else if((0|e)>=2&&(0|mQ(r))>=(0|e)){e:{i:if((b=Pi())&&(ne(b),u=sA(b),l=sA(b),o=sA(b))){if(t=(B=r)-n(r=(r>>>0)/(e>>>0)|0,e)|0,B=0,e-1>>>0>=3)for(C=-4&e;a[(B<<2)+f>>2]=r+((0|B)<(0|t)),a[((k=1|B)<<2)+f>>2]=r+((0|t)>(0|k)),a[((k=2|B)<<2)+f>>2]=r+((0|t)>(0|k)),a[((k=3|B)<<2)+f>>2]=r+((0|t)>(0|k)),B=B+4|0,(0|C)!=(0|(E=E+4|0)););if(C=3&e)for(;a[(B<<2)+f>>2]=r+((0|B)<(0|t)),B=B+1|0,(0|C)!=(0|(c=c+1|0)););if((a[A+16>>2]||(r=KQ(1),a[A+16>>2]=r,r))&&(a[A+24>>2]||(r=KQ(9),a[A+24>>2]=r,r))&&(a[A+20>>2]||(r=KQ(1),a[A+20>>2]=r,r))&&(k=A+28|0,(a[A+28>>2]||(r=KQ(9),a[k>>2]=r,r))&&(C=A+32|0,(a[A+32>>2]||(r=KQ(9),a[C>>2]=r,r))&&(a[A+36>>2]||(r=KQ(9),a[A+36>>2]=r,r))&&(a[A+40>>2]||(r=KQ(9),a[A+40>>2]=r,r))&&(a[A+44>>2]||(r=KQ(9),a[A+44>>2]=r,r))))){if(E=0,(0|e)>=3){a[A+4>>2]=1,B=e-2|0;Q:{f:{if(r=Ki(20,31289)){if(a[r+16>>2]=0,!((0|B)<=0)){if((0|B)>(1073741823-(t=a[r>>2])|0))break f;if(B=(0|(B=B+t|0))>4?B:4,t=a[r+4>>2]){if((0|B)!=a[r+12>>2]){if(!(t=K(t,B<<2,31289,0)))break f;a[r+12>>2]=B,a[r+4>>2]=t}}else{if(t=Ki(B<<2,31289),a[r+4>>2]=t,!t){Ve(15,129,65);break f}a[r+12>>2]=B}}}else r=0;break Q}SQ(a[r+4>>2],31289,0),SQ(r,31289,0),r=0}if(E=r,!r)break i;for((r=a[A+48>>2])&&Ti(r,140),a[A+48>>2]=E,B=2;;){if((r=Ki(20,31289))?(t=KQ(9),a[r>>2]=t,t&&(t=KQ(9),a[r+4>>2]=t,t&&(t=KQ(9),a[r+8>>2]=t,t&&(t=KQ(9),a[r+12>>2]=t,t)))||(xi(a[r>>2]),xi(a[r+4>>2]),xi(a[r+8>>2]),xi(a[r+12>>2]),SQ(r,31289,0),r=0)):(Ve(4,166,65),r=0),D=r,!r)break i;if(ir(E,D),(0|(B=B+1|0))==(0|e))break}}if(ni(a[A+20>>2],i)){for(U=(0|e)<5,c=0,t=0;;){B=k;Q:{f:switch(0|c){case 1:B=C;break Q;case 0:break Q;default:break f}D=B=UQ(E,c-2|0)}jf(w=a[B>>2],4),F=(N=(I=a[(c<<2)+f>>2])+N|0)-4|0,g=0,G=0;Q:{f:for(;;){if(i=t,!$(w,M=g+I|0,Q))break i;for(;;){B=0;a:if(!((0|c)<=0)){for(;;){r=k;B:{t:switch(0|B){case 1:r=C;break B;case 0:break B;default:break t}r=UQ(E,B-2|0)}if(Ye(w,a[r>>2])){if((0|c)==(0|(B=B+1|0)))break a}else if(B=0,!$(w,M,Q))break}break i}if(!OA(o,w,32936))break i;if(uQ(),jf(o,4),!hi(l,o,a[A+20>>2],b)){if(50331756!=(-16773121&fr()))break i;if(t=i+1|0,$r(),Xi(Q,2,i))continue f;break i}a:{B:switch(0|c){default:if(df(l,a[A+16>>2],w,b))break a;break i;case 0:if(c=0,Xi(Q,3,0))break Q;break i;case 1:break B}if(!df(l,a[k>>2],a[C>>2],b))break i}if(!lr(o,l,F))break i;if((r=WQ(o))-16>>>0<=4294967288){if(!Xi(Q,2,i))break i;i=i+1|0;a:{B:{if(!U){if(r>>>0>8)break B;g=g+1|0;break a}if(4!=(0|G))break a;c=-1,N=0,t=i;break Q}g=g-1|0}if(G=G+1|0,$(w,M=g+I|0,Q))continue;break i}break}break}if((0|c)>=2&&!ni(a[D+12>>2],a[A+16>>2]))break i;if(!ni(a[A+16>>2],l))break i;if(t=i,!Xi(Q,3,c))break i}if(!((0|(c=c+1|0))<(0|e)))break}if(i=Ye(a[k>>2],a[C>>2]),r=a[k>>2],(0|i)>=0?B=r:(B=a[C>>2],a[k>>2]=B,a[C>>2]=r),OA(l,B,32936)&&OA(o,a[C>>2],32936)&&df(u,l,o,b)){if(B=2,(0|e)>2)for(;;){if(r=UQ(E,B-2|0),!OA(a[r+4>>2],a[r>>2],32936))break i;if(!df(u,u,a[r+4>>2],b))break i;if((0|(B=B+1|0))==(0|e))break}if((r=KQ(1))&&(nQ(r,u),i=hi(a[A+24>>2],a[A+20>>2],r,b),xi(r),i&&(B=KQ(1)))){nQ(B,a[A+24>>2]);Q:if(H(0,a[A+36>>2],B,l,b)&&H(0,a[A+40>>2],B,o,b)){if(r=2,(0|e)>2)for(;;){if(!H(0,i=a[UQ(E,r-2|0)+4>>2],B,i,b))break Q;if((0|e)==(0|(r=r+1|0)))break}if(xi(B),!(B=KQ(1)))break i;if(nQ(B,a[A+28>>2]),hi(a[A+44>>2],a[A+32>>2],B,b)){if(A=2,(0|e)<=2)break e;for(;;){if(r=UQ(E,A-2|0),nQ(B,a[r>>2]),!hi(a[r+8>>2],a[r+12>>2],B,b))break Q;if((0|e)==(0|(A=A+1|0)))break}break e}}xi(B)}}}}}Ve(4,129,3),B=0;break r}xi(B),B=1}else Ve(4,129,165),B=0;Yr(b),Gr(b)}return s=f+32|0,B}function Y(A,r,e,Q,f,B){var t,k=0,n=0,c=0,C=0,E=0,o=0,l=0,w=0,g=0,u=0;s=t=s-16|0;A:if((k=A)||((k=Ki(40,31289))?(c=uf(),a[k+36>>2]=c,c?(a[k+16>>2]=0,c=Sr(e),a[k+12>>2]=c,c?(ia(e)&&jf(a[k+12>>2],4),a[k+20>>2]=-1):(xi(a[k>>2]),xi(a[k+4>>2]),xi(a[k+8>>2]),xi(a[k+12>>2]),Yf(a[k+36>>2]),SQ(k,31289,0),k=0)):(Ve(3,102,65),SQ(k,31289,0),k=0)):(Ve(3,102,65),k=0),k)){r:{e:if((a[k>>2]||(e=KQ(1),a[k>>2]=e,e))&&(a[k+4>>2]||(e=KQ(1),a[k+4>>2]=e,e))&&(n=a[k+8>>2],r&&(xi(n),n=Sr(r),a[k+8>>2]=n),n&&(f&&(a[k+32>>2]=f),B&&(a[k+28>>2]=B),Rr(a[k>>2],a[k+12>>2]))))for(n=32;;){i:{if(!L(a[k+4>>2],a[k>>2],a[k+12>>2],Q,t+12|0)){if(!a[t+12>>2])break e;if(n)break i;Ve(3,128,113);break e}Q:{if((r=a[k+32>>2])&&(e=a[k+28>>2])){if(f=a[k>>2],0|Ga[0|r](f,f,a[k+8>>2],a[k+12>>2],Q,e))break Q;break e}C=a[k>>2],w=a[k+8>>2];f:if(lf(o=a[k+12>>2])){if(!(a[C+12>>2]|1!=a[C+4>>2]||ia(w)||ia(C)||ia(o))){r=a[a[C>>2]>>2];a:if(ia(w)||ia(o))Ve(3,117,66),E=0;else if(lf(o))if(B=1==a[o+4>>2]?(r>>>0)%b[a[o>>2]>>2]|0:r,e=Dr(w))if(B){ne(Q),f=sA(Q);B:if(r=sA(Q))if(l=QQ()){if(Br(l,o,Q)){t:{b:{k:{n:{if((0|e)>=2){for(e=e-2|0,g=1,n=B;;){c=e,e=Zi(n,0,n,0);c:{C:{if(N){if(g){if(!MQ(f,n))break B;if(n=1,e=r,Rf(r=f,r,l,Q))break C;break B}if(!Wi(f,n))break B;if(n=1,H(0,r,e=f,o,Q))break C;break B}if(n=e,u=0,e=r,r=f,g)break c}if(u=1,g=0,!Zf(r,r,r,l,Q))break B}c:{if(gQ(w,c)){if(f=Zi(B,0,n,0),N){if(!u){if(!MQ(r,n))break B;if(g=0,u=Rf(r,r,l,Q),n=B,f=r,r=e,u)break c;break B}if(!Wi(r,n))break B;if(g=0,n=B,f=e,H(0,e,r,o,Q))break c;break B}n=f}f=r,r=e}if(e=c-1|0,!((0|c)>0))break}if(1!=(0|n)){if(g){B=n;break n}if(!Wi(f,n))break B;if(e=H(0,r,f,o,Q),f=r,e)break k;break B}if(!g)break k;break b}if(1==(0|B))break b}if(!MQ(f,B))break B;if(!Rf(f,f,l,Q))break B}if(FQ(C,f,l,Q))break t;break B}if(!MQ(C,1))break B}E=1}}else l=0;TQ(l),Yr(Q)}else xf(C),E=1;else{if(Ff(o)){xf(C),E=1;break a}E=MQ(C,1)}else Ve(3,117,102),E=0;break f}E=P(C,C,w,o,Q,0)}else{s=r=s-192|0;a:if(ia(w)||ia(C)||ia(o))Ve(3,125,66);else if(B=Dr(w)){a[(e=r+8|0)>>2]=0,a[e+4>>2]=0,a[e+48>>2]=0,a[e+40>>2]=0,a[e+44>>2]=0,a[e+32>>2]=0,a[e+36>>2]=0,a[e+24>>2]=0,a[e+28>>2]=0,a[e+16>>2]=0,a[e+20>>2]=0,a[e+8>>2]=0,a[e+12>>2]=0,rf(e),rf(e+20|0),ne(Q),n=sA(Q),f=sA(Q),a[r+64>>2]=f;B:if(f){t:{b:{if(a[o+12>>2]){if(ni(n,o))break b;break B}if((0|zQ(r+8|0,o))>0)break t;break B}if(a[n+12>>2]=0,(0|zQ(r+8|0,n))<=0)break B}if(dQ(f,C,o,Q))if(ka(f))xf(C),E=1;else{e=6;t:{if((0|B)>671||(e=5,(0|B)>239||(e=4,(0|B)>79||(e=3,c=1,!((0|B)<24))))){if(!FA(n,f,f,r+8|0,Q))break B;for(o=1<<e-1,f=1;;){if(l=f<<2,c=sA(Q),a[l+(r- -64|0)>>2]=c,!c)break B;if(!FA(c,a[60+(r+l|0)>>2],n,r+8|0,Q))break t;if((0|o)==(0|(f=f+1|0)))break}c=e}if(o=c,!MQ(C,1))break B;for(c=B-1|0,e=1;;){b:if(!gQ(w,c)){if(1&E)for(;;){if(!FA(C,C,C,r+8|0,Q)){E=0;break B}if(!c){E=1;break B}if(gQ(w,c=c-1|0))break b}for(;;){if(c){if(!gQ(w,c=c-1|0))continue;break b}break}E=1;break B}B=1,n=0,E=1,f=0;b:if(!(o>>>0<=1))for(;;){if((0|(l=c-E|0))<0)break b;if(B=(l=gQ(w,l))?B<<E-f|1:B,f=l?E:f,(0|o)==(0|(E=E+1|0)))break}b:if(!(1&((0|f)<0|e))){for(;;){if(FA(C,C,C,r+8|0,Q)){if(e=(0|f)==(0|n),n=n+1|0,!e)continue;break b}break}E=0;break B}if(!FA(C,C,a[(r- -64|0)+(B<<1&-4)>>2],r+8|0,Q)){E=0;break B}if(E=1,e=0,!((0|(c=(-1^f)+c|0))>=0))break}}}}Yr(Q),(e=r+8|0)&&(xi(e),xi(e+20|0),1&i[e+48|0]&&SQ(e,31289,0))}else{if(E=1,Ff(o)){xf(C);break a}E=MQ(C,1)}s=r+192|0}if(!E)break e}if(!(r=a[k+28>>2])){n=k;break A}if(!yf(e=a[k+4>>2],e,r,Q))break e;if(r=yf(r=a[k>>2],r,a[k+28>>2],Q),A){n=k;break A}if(!r)break r;n=k;break A}if(n=n-1|0,!Rr(a[k>>2],a[k+12>>2]))break}if(n=A)break A}xi(a[k>>2]),xi(a[k+4>>2]),xi(a[k+8>>2]),xi(a[k+12>>2]),Yf(a[k+36>>2]),n=0,SQ(k,31289,0)}return s=t+16|0,n}function R(A,r,e,i,Q,f){var B,t=0,b=0,k=0,n=0,c=0,C=0,E=0,o=0,l=0,w=0,g=0,u=0,M=0,D=0,I=0,N=0,G=0,U=0;s=B=s-48|0;A:if(lf(i))if(u=a[e+4>>2]){o=a[i+4>>2],ne(Q);r:{e:{if(!(g=f)){if(!(g=QQ())){g=0;break e}if(!Br(g,i,Q))break e}i:{if(a[r+12>>2]||(0|Fi(r,i))>=0){if(!(t=sA(Q)))break i;if(!dQ(t,r,i,Q))break i}else t=r;if(w=6,(0|u)>29||(w=5,(0|u)>9||(w=4,(0|u)>2||(w=(0|u)<=0?1:3))),r=(D=(o<<w)+((0|(r=o<<1))>(0|(c=1<<w))?r:c)<<2)- -64|0,(0|D)<=3071)s=r=B-(r+15&-16)|0;else if(!(r=I=iQ(r,31289,0))){I=0;break i}if(E=IQ(N=(-64&r)- -64|0,0,D),a[B+4>>2]=0,r=((k=o<<2)<<w)+E|0,a[B+24>>2]=r,a[B+28>>2]=0,a[B+8>>2]=o,a[B+32>>2]=o,a[B+12>>2]=0,a[B+16>>2]=2,a[B+36>>2]=0,a[B+40>>2]=2,a[B>>2]=r+k,b=a[i>>2],a[(b+k|0)-4>>2]<0){if(a[r>>2]=0-a[b>>2],!((0|o)<2)){if(M=3&(k=o-1|0),i=1,o-2>>>0>=3)for(C=-4&k,k=0;a[(n=i<<2)+r>>2]=-1^a[b+n>>2],a[(l=n+4|0)+r>>2]=-1^a[b+l>>2],a[(l=n+8|0)+r>>2]=-1^a[b+l>>2],a[(n=n+12|0)+r>>2]=-1^a[b+n>>2],i=i+4|0,(0|C)!=(0|(k=k+4|0)););if(M)for(k=0;a[(n=i<<2)+r>>2]=-1^a[b+n>>2],i=i+1|0,(0|M)!=(0|(k=k+1|0)););}a[B+28>>2]=o}else if(!yf(B+24|0,32936,g,Q))break i;if(M=0,yf(B,t,g,Q)){if(!((0|(n=(0|(r=a[B+28>>2]))<(0|o)?r:o))<=0)){if(t=a[B+24>>2],k=0,i=0,r=0,n-1>>>0>=3)for(l=-4&n,b=0;C=i<<2,a[(r<<2)+E>>2]=a[C+t>>2],a[((r=r+c|0)<<2)+E>>2]=a[t+(4|C)>>2],a[((r=r+c|0)<<2)+E>>2]=a[t+(8|C)>>2],a[((r=r+c|0)<<2)+E>>2]=a[t+(12|C)>>2],i=i+4|0,r=r+c|0,(0|l)!=(0|(b=b+4|0)););if(b=3&n)for(;a[(r<<2)+E>>2]=a[t+(i<<2)>>2],r=r+c|0,i=i+1|0,(0|b)!=(0|(k=k+1|0)););}if(!((0|(n=(0|(r=a[B+4>>2]))<(0|o)?r:o))<=0)){if(t=a[B>>2],i=1,k=0,r=0,n-1>>>0>=3)for(l=-4&n,b=0;C=r<<2,a[(i<<2)+E>>2]=a[C+t>>2],a[((i=i+c|0)<<2)+E>>2]=a[t+(4|C)>>2],a[((i=i+c|0)<<2)+E>>2]=a[t+(8|C)>>2],a[((i=i+c|0)<<2)+E>>2]=a[t+(12|C)>>2],r=r+4|0,i=i+c|0,(0|l)!=(0|(b=b+4|0)););if(b=3&n)for(;a[(i<<2)+E>>2]=a[t+(r<<2)>>2],i=i+c|0,r=r+1|0,(0|b)!=(0|(k=k+1|0)););}if(w>>>0>=2){if(!Gi(B+24|0,B,B,g,Q))break i;if(!((0|(n=(0|(r=a[B+28>>2]))<(0|o)?r:o))<=0)){if(t=a[B+24>>2],i=2,k=0,r=0,n-1>>>0>=3)for(l=-4&n,b=0;C=r<<2,a[(i<<2)+E>>2]=a[C+t>>2],a[((i=i+c|0)<<2)+E>>2]=a[t+(4|C)>>2],a[((i=i+c|0)<<2)+E>>2]=a[t+(8|C)>>2],a[((i=i+c|0)<<2)+E>>2]=a[t+(12|C)>>2],r=r+4|0,i=i+c|0,(0|l)!=(0|(b=b+4|0)););if(b=3&n)for(;a[(i<<2)+E>>2]=a[t+(r<<2)>>2],i=i+c|0,r=r+1|0,(0|b)!=(0|(k=k+1|0)););}for(G=c>>>0>4?c:4,t=3;;){if(!Gi(r=B+24|0,B,r,g,Q))break i;if(!((0|(C=(0|(r=a[B+28>>2]))<(0|o)?r:o))<=0)){if(n=a[B+24>>2],k=0,i=0,r=t,C-1>>>0>=3)for(U=-4&C,b=0;l=i<<2,a[(r<<2)+E>>2]=a[l+n>>2],a[((r=r+c|0)<<2)+E>>2]=a[n+(4|l)>>2],a[((r=r+c|0)<<2)+E>>2]=a[n+(8|l)>>2],a[((r=r+c|0)<<2)+E>>2]=a[n+(12|l)>>2],i=i+4|0,r=r+c|0,(0|U)!=(0|(b=b+4|0)););if(b=3&C)for(;a[(r<<2)+E>>2]=a[n+(i<<2)>>2],r=r+c|0,i=i+1|0,(0|b)!=(0|(k=k+1|0)););}if((0|G)==(0|(t=t+1|0)))break}}if(t=(0|(i=(i=u<<5)-(r=1+((i-1|0)%(0|w)|0)|0)|0))/32|0,b=-1<<r^-1,l=B+24|0,r=0,(0|i)<-31||(r=0,(0|(n=a[e+4>>2]))<=(0|t)||(C=a[e>>2],r=a[C+(t<<2)>>2],(k=i-(t<<5)|0)&&(r=r>>>k|0,(0|(t=t+1|0))>=(0|n)||(r|=a[C+(t<<2)>>2]<<32-k)))),SA(l,o,E,r&b,w))for(k=c-1|0;;){if(r=0,(0|i)<=0){M=0!=(0|FQ(A,B+24|0,g,Q));break i}for(;;){if(!Gi(t=B+24|0,t,t,g,Q))break i;if((0|w)==(0|(r=r+1|0)))break}if(t=(0|(i=i-w|0))/32|0,r=0,(0|i)<-31||(r=0,(0|(b=a[e+4>>2]))<=(0|t)||(u=a[e>>2],r=a[u+(t<<2)>>2],(c=i-(t<<5)|0)&&(r=r>>>c|0,(0|(t=t+1|0))>=(0|b)||(r|=a[u+(t<<2)>>2]<<32-c)))),!SA(B,o,E,r&k,w))break i;if(!Gi(r=B+24|0,r,B,g,Q))break}}}if(f)break r}TQ(g)}N&&(Of(N,D),SQ(I,31289,0)),Yr(Q)}else{if(M=1,Ff(i)){xf(A);break A}M=MQ(A,1)}else Ve(3,124,102);return s=B+48|0,M}function y(A,r,e,Q,f,t,b,k){var n,c=0,C=0,E=0,o=0,l=0,w=0,g=0;s=n=s-48|0,i[n+21|0]=0,a[n+8>>2]=0,a[n+12>>2]=0,a[n>>2]=0,a[n+4>>2]=0;A:{r:{e:{i:{if(5!=B[0|Q]){if(c=a[Q+4>>2],a[n+28>>2]=c,-4!=(0|c))break e;if(c=0,(0|f)<0)break i;Ve(13,108,127);break A}if(a[n+28>>2]=f,c=f,C=-1,-4!=(0|f))break r;f=-1}if(b){c=0,Ve(13,108,126);break A}if(a[n+16>>2]=a[r>>2],c=0,!vA(0,n+28|0,n+44|0,0,0,n+16|0,e,-1,0,0,k)){Ve(13,108,58);break A}B[n+44|0]?(c=-3,a[n+28>>2]=-3):c=a[n+28>>2]}C=f}a[n+16>>2]=a[r>>2];r:{e:switch((c=vA(n+24|0,0,0,n+22|0,n+23|0,n+16|0,f=e,(e=-1==(0|C))?c:C,e?0:t,b,k))+1|0){case 0:break A;case 1:break e;default:break r}Ve(13,108,58),c=0;break A}r:{e:{i:{Q:{f:{a:switch((e=a[n+28>>2])+3|0){case 19:case 20:break f;case 0:break a;default:break Q}if(!k)break i;i[0|k]=0;break i}if(B[n+23|0])break i;c=0,Ve(13,108,156);break A}if(B[n+23|0]){if(!(!(1<<e&1126)|e>>>0>10)){c=0,Ve(13,108,195);break A}if(i[n+21|0]=1,c=0,!$A(n,n+16|0,a[n+24>>2],i[n+22|0],0))break r;if(!mA(n,(f=a[n>>2])+1|0)){Ve(13,108,65),c=0;break r}i[a[n+4>>2]+f|0]=0,k=a[n+4>>2];break e}f=a[n+24>>2],k=a[n+16>>2],a[n+16>>2]=f+k;break e}if(k=a[r>>2],B[n+22|0]){i:{if((0|(f=a[n+24>>2]))>0){for(c=a[n+16>>2],b=1;;){Q:if(B[c+1|0]|B[0|c]|1==(0|f)){if(a[n+32>>2]=c,128&(e=MA(n+32|0,n+36|0,n+44|0,n+40|0,f))){Ve(13,104,102),Ve(13,190,58),c=0;break r}if(1&e){if(-1==(0|b)){Ve(13,190,58),c=0;break r}b=b+1|0,f=(e=f+c|0)-(c=a[n+32>>2])|0;break Q}f=(e=f+c|0)-(c=a[n+32>>2]+a[n+36>>2]|0)|0}else{if(c=c+2|0,!(b=b-1|0))break i;f=f-2|0}if(!((0|f)>0))break}if(!b)break i}Ve(13,190,137),c=0;break r}a[n+16>>2]=c,f=c-k|0}else e=a[n+16>>2],f=a[n+24>>2],a[n+16>>2]=e+f,f=f+(e-k|0)|0}t=a[n+28>>2],a[n+44>>2]=k;e:{i:{Q:{if((e=a[Q+16>>2])&&(e=a[e+20>>2])){if(0|Ga[0|e](A,k,f,t,n+21|0,Q))break e;break i}if(c=0,-4==a[Q+4>>2]){if(!(c=a[A>>2])){if(!(c=HQ(32672))){oa(0);break i}a[A>>2]=c}(0|t)!=a[c>>2]&&gi(c,t,0),b=A,A=c+4|0}else b=0;f:{a:{B:{t:{b:{k:switch(t-1|0){case 4:if(!f)break t;Ve(13,204,144);break Q;case 0:if(1!=(0|f)){Ve(13,204,106);break Q}a[A>>2]=B[0|k];break e;case 1:case 9:e=0,s=k=s-16|0;n:if(f){Q=1;c:{C:{if(1!=(0|f)){E:{if(C=a[n+44>>2],E=B[0|C]){if(e=0,255!=(0|E))break E;if(o=3&(e=f-1|0),f-2>>>0<3)e=1,Q=0;else for(g=-4&e,Q=0,e=1;Q=Q|B[0|(Q=e+C|0)]|B[Q+1|0]|B[Q+2|0]|B[Q+3|0],e=e+4|0,(0|g)!=(0|(l=l+4|0)););if(o)for(;Q=B[e+C|0]|Q,e=e+1|0,(0|o)!=(0|(w=w+1|0)););if(!Q){e=0;break E}}if(e=0,(B[C+1|0]^E)<<24>>24>=0)break c;e=-1}if(!(Q=e+f|0))break C}if(!A||!(e=a[A>>2])){if(!(e=xQ(2)))break C;a[e+4>>2]=2}if(ze(e,0,Q)){if(pA(a[e+8>>2],k+12|0,a[n+44>>2],f),a[k+12>>2]&&(a[e+4>>2]=256|a[e+4>>2]),a[n+44>>2]=a[n+44>>2]+f,!A)break n;a[A>>2]=e;break n}Ve(13,194,65),a[A>>2]==(0|e)&&A||wQ(e)}e=0;break n}Ve(13,226,221)}else Ve(13,226,222);if(s=k+16|0,!e)break Q;A=a[A>>2],a[A+4>>2]=t|256&a[A+4>>2];break B;case 29:if(!(1&f))break b;Ve(13,204,214);break Q;case 5:break f;case 2:break a;case 27:break k;default:break b}if(3&f){Ve(13,204,215);break Q}}if(e=a[A>>2])a[e+4>>2]=t;else{if(!(e=xQ(t))){Ve(13,204,65);break Q}a[A>>2]=e}if(B[n+21|0]){SQ(a[e+8>>2],31289,0),a[e>>2]=f,a[e+8>>2]=k,i[n+21|0]=0;break e}if(ze(e,k,f))break e;Ve(13,204,65),wQ(e),a[A>>2]=0;break Q}a[A>>2]=1}if(!c|5!=(0|t))break e;a[c+4>>2]=0;break e}e=0,t=0;a:{B:{t:if((0|f)<=0)Q=152;else{if(!(A&&(e=a[A>>2])||(e=xQ(3))))break B;if(Q=220,C=a[n+44>>2],!((E=B[0|C])>>>0>7)){if(Q=C+1|0,a[e+12>>2]=E|-16&a[e+12>>2]|8,k=f-1|0,f>>>0>=2){if(!(t=iQ(k,31289,0))){Q=65;break t}Q=(yQ(t,Q,k)+f|0)-2|0,i[0|Q]=B[0|Q]&255<<E,Q=f+C|0}a[e>>2]=k,SQ(a[e+8>>2],31289,0),a[e+4>>2]=3,a[e+8>>2]=t,A&&(a[A>>2]=e),a[n+44>>2]=Q;break a}}Ve(13,189,Q),a[A>>2]==(0|e)&&A||wQ(e)}e=0}if(e)break e;break Q}if(LA(A,n+44|0,f))break e}oa(c),b&&(a[b>>2]=0)}c=0;break r}a[r>>2]=a[n+16>>2],c=1}B[n+21|0]&&SQ(a[n+4>>2],31289,0)}return s=n+48|0,c}function h(A,r,e,Q,c,C,E){var o,l=0,w=0,g=0,u=0,M=0,D=0,I=0,N=0,G=0,U=0,F=0,Z=0,d=0;s=o=s-16|0,a[o+12>>2]=0,w=C+1|0,Z=B[0|C],D=-1;A:{r:{e:{for(;;){if(C=w,u=l,Z){if(!r&b[o+12>>2]>=b[e>>2])break e;d=Z<<24>>24,N=I,G=D;i:{Q:{f:{a:{B:{t:{b:{k:{n:{c:switch(0|M){case 0:if(U=1,37==(0|Z))break t;if(U=0,F=g,w=0,zr(A,r,o+12|0,e,d))break Q;break A;case 1:M=2;C:switch(d-32|0){case 13:l|=1;break f;case 11:l|=2;break f;case 0:l|=4;break f;case 3:l|=8;break f;case 16:break C;default:break i}l|=16;break f;case 2:if(Mf(d,4)){I=(n(N,10)+d|0)-48|0,U=2;break t}if(M=3,42!=(0|Z))break n;I=a[E>>2],U=3,E=E+4|0;break t;case 3:if(M=5,U=4,F=g,46==(0|Z))break Q;break i;case 6:break a;case 5:break B;case 4:break c;default:break i}if(Mf(d,4)){D=(n((0|G)>0?G:0,10)+d|0)-48|0,U=4;break b}if(M=5,42==(0|Z))break k}w=C,I=N,D=G,l=u;break i}D=a[E>>2],E=E+4|0,U=5}I=N}l=u,F=g;break Q}F=1,U=6,M=6;B:switch(d-76|0){case 32:F=(u=108==B[0|C])?4:2,C=C+u|0;break Q;case 30:case 37:F=4;break Q;case 0:F=3;break Q;case 46:break B;case 28:break Q;default:break i}F=5;break Q}D=-1,I=0,U=0,l=0,F=0;a:{B:{t:{b:{k:{n:{c:{C:switch(d-37|0){case 63:case 68:E:{o:switch(g-1|0){case 0:g=l=f[E>>1],E=E+4|0,M=l>>31;break E;case 1:case 4:g=l=a[E>>2],E=E+4|0,M=l>>31;break E;case 3:E=(l=E+7&-8)+8|0,g=a[l>>2],M=a[l+4>>2];break E;default:break o}g=l=a[E>>2],E=E+4|0,M=l>>31}if(l=0,w=0,IA(A,r,o+12|0,e,g,M,10,N,G,u))break Q;break A;case 51:u|=32;case 74:case 80:case 83:u|=64;E:{o:switch(g-1|0){case 0:g=t[E>>1],E=E+4|0,M=0;break E;case 1:case 4:g=a[E>>2],E=E+4|0,M=0;break E;case 3:E=(l=E+7&-8)+8|0,g=a[l>>2],M=a[l+4>>2];break E;default:break o}g=a[E>>2],E=E+4|0,M=0}if(l=0,w=0,IA(A,r,o+12|0,e,g,M,111==(0|Z)?8:117==(0|Z)?10:16,N,G,u))break Q;break A;case 65:if(!J(A,r,o+12|0,e,k[(E=E+7&-8)>>3],N,G,u,0)){w=0;break A}E=E+8|0;break a;case 32:u|=32;case 64:if(!J(A,r,o+12|0,e,k[(E=E+7&-8)>>3],N,G,u,1)){w=0;break A}E=E+8|0;break a;case 34:u|=32;case 66:if(!J(A,r,o+12|0,e,k[(E=E+7&-8)>>3],N,G,u,2)){w=0;break A}E=E+8|0;break a;case 62:if(zr(A,r,o+12|0,e,a[E>>2]))break c;w=0;break A;case 78:l=(w=(0|G)>=0)?G:2147483647,g=a[E>>2],l=r|w?l:a[e>>2],w=I=g||24336;E:if(D=(0|l)>=0?l:-1){for(g=I+D|0;;){if(!B[0|w])break E;if(w=w+1|0,!(D=D-1|0))break}w=g}if(w=((g=N-(M=w-I|0)|0)|N)<0?0:g,g=(0|l)<0?l:(2147483647-w|0)>(0|l)?w+l|0:2147483647,(0|(l=1&u?0-w|0:w))<=0){u=l,l=0;break B}if(w=0,D=0,(0|g)<0)break b;for(u=l-g|0;;){if((0|w)==(0|g)){l=g;break B}if(!zr(A,r,o+12|0,e,32)){w=0;break A}if((0|(w=w+1|0))==(0|l))break}break t;case 75:if(IA(A,r,o+12|0,e,a[E>>2],0,16,N,G,8|u))break c;w=0;break A;case 82:break k;case 0:break n;case 73:break C;default:break Q}a[a[E>>2]>>2]=a[o+12>>2]}E=E+4|0;break a}if(w=0,zr(A,r,o+12|0,e,37))break Q;break A}C=C+1|0;break a}for(;;){if(!zr(A,r,o+12|0,e,32))break A;if((0|(D=D+1|0))==(0|l))break}}u=0}B:if(M)if((0|g)>=0){for(w=(0|l)<(0|g)?g:l,N=l+M|0;;){if((0|l)==(0|w)){l=w;break B}if(!zr(A,r,o+12|0,e,i[0|I])){w=0;break A}if(l=l+1|0,I=I+1|0,!(M=M-1|0))break}l=N}else for(l=l+M|0;;){if(!zr(A,r,o+12|0,e,i[0|I])){w=0;break A}if(I=I+1|0,!(M=M-1|0))break}if(E=E+4|0,D=-1,I=0,!((0|u)>=0)){if(!((0|g)>=0)){for(;;){if(zr(A,r,o+12|0,e,32)){if(u=u+1|0)continue;break a}break}w=0;break A}for(g=(0|l)<(0|g)?g:l;;){if((0|l)==(0|g))break a;if(!zr(A,r,o+12|0,e,32)){w=0;break A}if(l=l+1|0,!(u=u+1|0))break}}}l=0;break Q}U=1,F=g}w=C+1|0,Z=B[0|C],M=U,g=F}if(7!=(0|M))continue}break}if(r)break r}E=c,C=a[o+12>>2],c=a[e>>2]-1|0,a[E>>2]=C>>>0>c>>>0,c>>>0>=C>>>0||(a[o+12>>2]=c)}w=0,zr(A,r,o+12|0,e,0)&&(a[Q>>2]=a[o+12>>2]-1,w=1)}return s=o+16|0,w}function J(A,r,e,Q,f,a,t,b,k){var C,E=0,o=0,l=0,w=0,g=0,u=0,M=0,D=0,I=0,N=0,G=0,U=0,F=0,Z=0;s=C=s-96|0,F=45,f<0||(F=43,2&b||(F=(g=4&b)<<3,Z=!g)),o=(o=(0|t)<0)?6:t;A:{r:{e:{I=k;i:if(2==(0|k)){if(I=0,0==f)break e;if(I=1,f<1e-4|(o?0:f>=10))break e;if(o){if(w=o-1|0,l=1,g=7&o)for(t=o;t=t-1|0,l*=10,(0|g)!=(0|(E=E+1|0)););else t=o;if(w>>>0>=7)for(;l=10*l*10*10*10*10*10*10*10,t=t-8|0;);if(f>=l)break i}I=0}if(!k)break r}if(0!=f){if(f<1)for(l=f;G=G-1|0,(l*=10)<1;);else l=f;if(l>10)for(;G=G+1|0,(l/=10)>10;);}else l=f;e:if(2==(0|k)){if(t=o||1,!I){if(E=0,(0|(o=t+(-1^G)|0))>=0)break e;break A}o=t-1|0}f=1==(0|I)?l:f}if(E=0,!((f=f<0?-f:f)>=4294967296)){if(u=f<4294967296&f>=0?~~f>>>0:0,g=(0|o)<9?o:9){if(M=g-1|0,l=1,w=7&g)for(t=g;t=t-1|0,l*=10,(0|w)!=(0|(E=E+1|0)););else t=g;if(M>>>0>=7)for(;l=10*l*10*10*10*10*10*10*10,t=t-8|0;);if(N=7&g,D=l-+(0|(w=c(l)<2147483648?~~l:-2147483648))>=.5,l=1,N)for(E=0,t=g;t=t-1|0,l*=10,(0|N)!=(0|(E=E+1|0)););else t=g;if(E=w+D|0,!(M>>>0<7))for(;l=10*l*10*10*10*10*10*10*10,t=t-8|0;);}else l=1,E=1;for(w=0,t=E,D=(N=(D=c(f=(f-+(u>>>0))*l)<2147483648?~~f:-2147483648)+(f-+(0|D)>=.5)|0)>>>0<E>>>0?0:t,t=u+(E>>>0<=N>>>0)|0;u=(t>>>0)/10|0,i[(E=w)+(C- -64|0)|0]=B[24669+(t-n(u,10)|0)|0],w=E+1|0,!(t>>>0<10)&&(t=u,E>>>0<19););i[(M=20==(0|w)?E:w)+(C- -64|0)|0]=0,t=N-D|0;r:{e:{if(2==(0|k)){for(w=o>>31&o,o=((0|o)<=0?o:1)-1|0;;){if(E=0,(0|g)<=0){g=w;break r}i:{for(;;){if(!((u=t-n(k=(t>>>0)/10|0,10)|0)|E))break i;if(i[(C+32|0)+E|0]=B[u+24669|0],t=(t>>>0)/10|0,(0|g)==(0|(E=E+1|0)))break}o=g;break e}if(u=(0|g)>1,g=g-1|0,t=k,!u)break}g=o;break r}if((0|o)<=0)break r;if(N=1&(o=(0|g)>1?g:1),E=0,(0|g)>=2)for(D=2147483646&o,w=0;k=(t>>>0)/10|0,i[(u=C+32|0)+E|0]=B[24669+(t-n(k,10)|0)|0],i[u+(1|E)|0]=B[24669+((k>>>0)%10|0)|0],t=(t>>>0)/100|0,E=E+2|0,(0|D)!=(0|(w=w+2|0)););N&&(i[(C+32|0)+E|0]=B[24669+((t>>>0)%10|0)|0])}U=20==(0|o)?19:o}if(i[(C+32|0)+U|0]=0,w=0,1==(0|I)){for(o=((t=G>>31)^G)-t|0;k=w,o=(0|(t=o))/10|0,i[w+C|0]=B[24669+(t-n(o,10)|0)|0],w=w+1|0,!((0|t)<10)&&k>>>0<19;);if((t=(0|t)>9)|k||(i[C+1|0]=48,w=2),E=0,t)break A}r:{a=(0|(a=((a-!Z-(g+M)|0)-((0|g)>0)|0)+(1==(0|I)?-2-w|0:0)|0))>0?a:0;e:if(!(16&b)|(0|(t=1&b?0-a|0:a))<=0){if((0|t)>0){for(;;){if(!zr(A,r,e,Q,32))break r;if(a=(0|t)>1,t=t-1|0,!a)break}t=0}if(F&&!zr(A,r,e,Q,F))break r}else{if(!Z){if(!zr(A,r,e,Q,F))break r;if(!(t=t-1|0)){t=0;break e}}for(;;){if(!zr(A,r,e,Q,48))break r;if(a=(0|t)<2,t=t-1|0,a)break}t=0}for(o=(0|(a=g-U|0))>0?a:0;;){if((0|M)>0){if(zr(A,r,e,Q,i[(M=M-1|0)+(C- -64|0)|0]))continue;break r}break}e:if(!(!(8&b)&(0|g)<=0)){if(E=0,!zr(A,r,e,Q,46))break A;for(;;){if((0|U)<=0)break e;if(!zr(A,r,e,Q,i[(U=U-1|0)+(C+32|0)|0]))break}break A}if((0|a)>0)for(;;){if(!zr(A,r,e,Q,48))break r;if(a=(0|o)<2,o=o-1|0,a)break}e:if(1==(0|I)){if(E=0,!zr(A,r,e,Q,32&b^101))break A;i:{if((0|G)<0){if(zr(A,r,e,Q,45))break i;break A}if(!zr(A,r,e,Q,43))break A}for(;;){if((0|w)<=0)break e;if(!zr(A,r,e,Q,i[C+(w=w-1|0)|0]))break}break A}if(E=1,(0|t)>=0)break A;for(;;){if(!zr(A,r,e,Q,32))break r;if(!(t=t+1|0))break}break A}E=0}}return s=C+96|0,E}function T(A,r,e,Q,f,t,b,k,c){var C,E=0,o=0,l=0,w=0;s=C=s-48|0,a[C+44>>2]=e,o=a[Q+16>>2],a[C+40>>2]=0;A:if(A){o&&(l=a[o+16>>2])||(l=0);r:{e:if((0|c)>=30)Ve(13,120,201);else{w=c+1|0;i:{Q:{f:{a:{B:{t:switch(i[0|Q]){case 0:if(c=a[Q+8>>2]){if(b||-1!=(0|f)){Ve(13,120,170);break e}E=Wr(A,r,e,c,0,k,w);break A}E=y(A,r,e,Q,f,t,b,k);break A;case 5:if(-1!=(0|f)){Ve(13,120,230);break e}if(a[C+40>>2]=a[r>>2],!vA(0,C+32|0,C+39|0,0,0,C+40|0,e,-1,0,1,k)){Ve(13,120,58);break e}if(B[C+39|0]){if(E=-1,b)break A;Ve(13,120,139);break e}if(!((e=(f=a[C+32>>2])>>>0<=30?a[32432+(f<<2)>>2]:0)&a[Q+4>>2])){if(E=-1,b)break A;Ve(13,120,140);break e}E=y(A,r,a[C+44>>2],Q,f,0,0,k);break A;case 4:E=0|Ga[a[o+16>>2]](A,r,e,Q,f,t,b,k);break A;case 2:if(-1!=(0|f)){Ve(13,120,230);break e}if(l&&!(0|Ga[0|l](4,A,Q,0)))break a;if(a[A>>2]){if((0|(e=Wf(A,Q)))<0|(0|e)>=a[Q+12>>2])break B;ii(Lf(A,e=a[Q+8>>2]+n(e,20)|0),e),Tf(A,-1,Q);break B}if(Ba(A,Q))break B;Ve(13,120,58);break e;case 1:case 6:break t;default:break A}a[C+40>>2]=a[r>>2];t:{b:switch(c=f,(E=vA(C+44|0,0,0,C+32|0,C+39|0,C+40|0,e,(f=-1==(0|f))?16:c,f?0:t,b,k))+1|0){case 0:break A;case 1:break b;default:break t}Ve(13,120,58);break e}if(!o|!(4&B[o+4|0])?c=B[C+32|0]:(a[C+44>>2]=a[r>>2]+(e-a[C+40>>2]|0),c=1),!B[C+39|0]){Ve(13,120,149);break e}if(!a[A>>2]&&!Ba(A,Q)){Ve(13,120,58);break e}if(l&&!(0|Ga[0|l](4,A,Q,0)))break a;E=a[Q+8>>2],f=0;t:{b:{k:if(!(a[Q+12>>2]<=0)){for(;3&B[E+1|0]&&(e=Lr(A,E,0))&&ii(Lf(A,e),e),E=E+20|0,(0|(f=f+1|0))<(0|(e=a[Q+12>>2])););if(E=a[Q+8>>2],f=0,!((0|e)<=0))for(;;){if(!(e=Lr(A,E,1)))break e;if(b=Lf(A,e),!(t=a[C+44>>2]))break k;if(o=a[C+40>>2],!(B[0|o]|(0|t)<2|B[o+1|0])){if(e=o+2|0,a[C+40>>2]=e,B[C+32|0])break b;Ve(13,120,159);break e}n:{c:switch(Wr(b,C+40|0,t,e,1&a[e>>2]&(a[Q+12>>2]-1|0)!=(0|f),k,w)+1|0){case 0:ii(b,e);break n;case 1:break Q;default:break c}a[C+44>>2]=a[C+44>>2]+(o-a[C+40>>2]|0)}if(E=E+20|0,!((0|(f=f+1|0))<a[Q+12>>2]))break}}if(e=a[C+44>>2],!B[C+32|0])break t;if(!((0|e)<2||(e=a[C+40>>2],B[0|e]|B[e+1|0]))){a[C+40>>2]=e+2,e=1;break t}Ve(13,120,137);break e}i[C+32|0]=0,e=(t+o|0)-e|0,a[C+44>>2]=e}if(!e||c){if(a[Q+12>>2]>(0|f))for(;;){if(!(e=Lr(A,E,1)))break e;if(!(1&i[0|e])){Ve(13,120,121);break Q}if(ii(Lf(A,e),e),E=E+20|0,!((0|(f=f+1|0))<a[Q+12>>2]))break}f=e=a[r>>2],b=a[C+40>>2]-e|0;t:{if(A&&(e=a[A>>2])&&!(!(t=a[Q+16>>2])|!(2&B[t+4|0]))){if(t=e+a[t+20>>2]|0,SQ(a[t>>2],31289,0),e=iQ(b,31289,0),a[t>>2]=e,!e){Ve(13,115,65),e=0;break t}yQ(e,f,b),a[t+8>>2]=0,a[t+4>>2]=b}e=1}if(!e)break a;if(l&&!(0|Ga[0|l](5,A,Q,0)))break a;a[r>>2]=a[C+40>>2],E=1;break A}Ve(13,120,148);break e}a[C+40>>2]=a[r>>2];B:if(!((0|(f=a[Q+12>>2]))<=0))for(e=a[Q+8>>2];;){if(-1!=(0|(f=Wr(t=Lf(A,e),C+40|0,a[C+44>>2],e,1,k,w)))){if((0|f)<=0)break f;f=a[Q+12>>2];break B}if(e=e+20|0,!((0|(f=a[Q+12>>2]))>(0|(E=E+1|0))))break}if((0|f)==(0|E)){if(!b)break i;ga(A,Q),E=-1;break A}if(Tf(A,E,Q),!l||0|Ga[0|l](5,A,Q,0)){a[r>>2]=a[C+40>>2],E=1;break A}}Ve(13,120,100);break e}if(ii(t,e),Ve(13,120,58),!e)break e}A=a[e+12>>2],a[C+28>>2]=a[Q+24>>2],a[C+24>>2]=24386,a[C+20>>2]=A,a[C+16>>2]=24427,br(4,C+16|0);break r}Ve(13,120,143)}a[C+4>>2]=a[Q+24>>2],a[C>>2]=24388,br(2,C)}E=0}return s=C+48|0,E}function m(A){var r=0,e=0,i=0,Q=0,f=0,B=0,t=0;A:if(A|=0){f=(i=A-8|0)+(A=-8&(r=a[A-4>>2]))|0;r:if(!(1&r)){if(!(3&r))break A;if((i=i-(r=a[i>>2])|0)>>>0<b[23340])break A;if(A=A+r|0,a[23341]==(0|i)){if(3==(3&(r=a[f+4>>2])))return a[23338]=A,a[f+4>>2]=-2&r,a[i+4>>2]=1|A,void(a[A+i>>2]=A)}else{if(r>>>0<=255){if(Q=a[i+8>>2],r=r>>>3|0,(0|(e=a[i+12>>2]))==(0|Q)){a[23336]=a[23336]&OQ(-2,r);break r}a[Q+12>>2]=e,a[e+8>>2]=Q;break r}if(t=a[i+24>>2],(0|i)==(0|(r=a[i+12>>2])))if((e=a[(Q=i+20|0)>>2])||(e=a[(Q=i+16|0)>>2])){for(;B=Q,(e=a[(Q=(r=e)+20|0)>>2])||(Q=r+16|0,e=a[r+16>>2]););a[B>>2]=0}else r=0;else e=a[i+8>>2],a[e+12>>2]=r,a[r+8>>2]=e;if(!t)break r;Q=a[i+28>>2];e:{if(a[(e=93648+(Q<<2)|0)>>2]==(0|i)){if(a[e>>2]=r,r)break e;a[23337]=a[23337]&OQ(-2,Q);break r}if(a[t+(a[t+16>>2]==(0|i)?16:20)>>2]=r,!r)break r}if(a[r+24>>2]=t,(e=a[i+16>>2])&&(a[r+16>>2]=e,a[e+24>>2]=r),!(e=a[i+20>>2]))break r;a[r+20>>2]=e,a[e+24>>2]=r}}if(!(i>>>0>=f>>>0)&&1&(r=a[f+4>>2])){r:{if(!(2&r)){if(a[23342]==(0|f)){if(a[23342]=i,A=a[23339]+A|0,a[23339]=A,a[i+4>>2]=1|A,a[23341]!=(0|i))break A;return a[23338]=0,void(a[23341]=0)}if(a[23341]==(0|f))return a[23341]=i,A=a[23338]+A|0,a[23338]=A,a[i+4>>2]=1|A,void(a[A+i>>2]=A);A=(-8&r)+A|0;e:if(r>>>0<=255){if(Q=a[f+8>>2],r=r>>>3|0,(0|(e=a[f+12>>2]))==(0|Q)){a[23336]=a[23336]&OQ(-2,r);break e}a[Q+12>>2]=e,a[e+8>>2]=Q}else{if(t=a[f+24>>2],(0|f)==(0|(r=a[f+12>>2])))if((e=a[(Q=f+20|0)>>2])||(e=a[(Q=f+16|0)>>2])){for(;B=Q,(e=a[(Q=(r=e)+20|0)>>2])||(Q=r+16|0,e=a[r+16>>2]););a[B>>2]=0}else r=0;else e=a[f+8>>2],a[e+12>>2]=r,a[r+8>>2]=e;if(t){Q=a[f+28>>2];i:{if(a[(e=93648+(Q<<2)|0)>>2]==(0|f)){if(a[e>>2]=r,r)break i;a[23337]=a[23337]&OQ(-2,Q);break e}if(a[t+(a[t+16>>2]==(0|f)?16:20)>>2]=r,!r)break e}a[r+24>>2]=t,(e=a[f+16>>2])&&(a[r+16>>2]=e,a[e+24>>2]=r),(e=a[f+20>>2])&&(a[r+20>>2]=e,a[e+24>>2]=r)}}if(a[i+4>>2]=1|A,a[A+i>>2]=A,a[23341]!=(0|i))break r;return void(a[23338]=A)}a[f+4>>2]=-2&r,a[i+4>>2]=1|A,a[A+i>>2]=A}if(A>>>0<=255)return r=93384+(-8&A)|0,(e=a[23336])&(A=1<<(A>>>3))?A=a[r+8>>2]:(a[23336]=A|e,A=r),a[r+8>>2]=i,a[A+12>>2]=i,a[i+12>>2]=r,void(a[i+8>>2]=A);Q=31,A>>>0<=16777215&&(r=A>>>8|0,r<<=B=r+1048320>>>16&8,Q=28+((r=((r<<=Q=r+520192>>>16&4)<<(e=r+245760>>>16&2)>>>15|0)-(e|Q|B)|0)<<1|A>>>r+21&1)|0),a[i+28>>2]=Q,a[i+16>>2]=0,a[i+20>>2]=0,B=93648+(Q<<2)|0;r:{e:{if((e=a[23337])&(r=1<<Q)){for(Q=A<<(31==(0|Q)?0:25-(Q>>>1|0)|0),r=a[B>>2];;){if(e=r,(-8&a[r+4>>2])==(0|A))break e;if(r=Q>>>29|0,Q<<=1,!(r=a[(B=e+(4&r)|0)+16>>2]))break}a[B+16>>2]=i,a[i+24>>2]=e}else a[23337]=r|e,a[B>>2]=i,a[i+24>>2]=B;a[i+12>>2]=i,a[i+8>>2]=i;break r}A=a[e+8>>2],a[A+12>>2]=i,a[e+8>>2]=i,a[i+24>>2]=0,a[i+12>>2]=e,a[i+8>>2]=A}A=a[23344]-1|0,a[23344]=A||-1}}}function L(A,r,e,i,Q){var f,B=0,t=0,b=0,k=0,n=0,c=0,C=0,E=0,o=0,l=0,w=0,g=0,u=0;if(s=f=s-32|0,Ff(e)||ka(e))a[Q>>2]=1;else{a[Q>>2]=0;A:if(ia(r)||ia(e)){ne(i),b=sA(i),B=sA(i),n=sA(i),g=sA(i),o=sA(i),k=sA(i);r:if(sA(i))if((C=A)||(C=KQ(1))){if(MQ(n,1)&&(xf(k),ni(B,r)&&ni(b,e)&&(a[b+12>>2]=0,!a[B+12>>2]&&(0|Fi(B,b))<0||(rf(r=f+8|0),nQ(r,B),dQ(B,r,b,i))))){e:{if(ka(B))r=k;else{for(E=-1,t=1;;){if(r=n,l=t,rf(c=f+8|0),n=b,nQ(c,b),t=0,b=B,!H(g,o,c,B,i))break r;if(!df(n,g,r,i))break r;if(!PA(n,n,k))break r;if(c=ka(o),t=E,E=l,B=o,o=k,k=r,c)break}if((0|E)>=0)break e}if(!OA(r,e,r)){t=0;break r}}if(Qf(b)){e:{if(!(a[r+12>>2]||(0|Fi(r,e))>=0)){if(t=0,ni(C,r))break e;break r}if(t=0,!dQ(C,r,e,i))break r}t=C,r=0}else t=0,r=1;a[Q>>2]=r}}else C=0;if(A|t)break A;xi(C)}else{ne(i),b=sA(i),k=sA(i),B=sA(i),c=sA(i),C=sA(i),n=sA(i);r:if(w=sA(i))if((o=A)||(o=KQ(1))){if(MQ(B,1)&&(xf(n),ni(k,r)&&ni(b,e)&&(a[b+12>>2]=0,!a[k+12>>2]&&(0|Fi(k,b))<0||dQ(k,k,b,i)))){e:{i:{Q:if(!lf(e)||(0|Dr(e))>2048){if(!ka(k)){for(t=1,l=-1;;){E=B,g=t;f:{if(r=k,(0|Dr(B=b))==(0|Dr(r))){if(t=0,!MQ(c,1))break r;if(OA(C,B,r))break f;break r}if((0|Dr(B))!=(Dr(r)+1|0)){if(!H(c,C,B,r,i)){t=0;break r}}else{if(t=0,!Ne(w,r))break r;if((0|Fi(B,w))<0){if(!MQ(c,1))break r;if(OA(C,B,r))break f;break r}if(!OA(C,B,w))break r;if(!PA(c,w,r))break r;if((0|Fi(B,c))<0){if(MQ(c,2))break f;break r}if(!MQ(c,3))break r;if(!OA(C,C,r))break r}}f:{if(Qf(c)){if(PA(B,E,n))break f;t=0;break r}a:{if(GQ(c,2)){if(Ne(B,E))break a;t=0;break r}if(GQ(c,4)){if($Q(B,E,2))break a;t=0;break r}if(1!=a[c+4>>2]){if(!df(B,c,E,i)){t=0;break r}}else{if(t=0,!ni(B,E))break r;if(!Wi(B,a[a[c>>2]>>2]))break r}}if(!PA(B,B,n)){t=0;break r}}if(u=ka(C),t=l,l=g,b=r,k=C,C=n,n=E,u)break}if((0|l)>=0)break e;break i}}else{if(ka(k))break Q;for(r=0;;){if(!gQ(k,r)){if(lf(B)&&!Ir(B,B,e))break r;if(r=r+1|0,vr(B,B))continue;break r}if(r&&!lr(k,k,r))break r;for(r=0;;){if(!gQ(b,r)){if(lf(n)&&!Ir(n,n,e))break r;if(r=r+1|0,vr(n,n))continue;break r}break}if(r&&!lr(b,b,r))break r;f:{if((0|Fi(k,b))>=0){if(!Ir(B,B,n))break r;if(Er(k,k,b))break f;break r}if(!Ir(n,n,B))break r;if(!Er(b,b,k))break r}if(r=0,ka(k))break}}r=b,E=n}if(!OA(E,e,E)){t=0;break r}}e:{if(Qf(r)){if(!(a[E+12>>2]||(0|Fi(E,e))>=0)){if(ni(o,E))break e;t=0;break r}if(dQ(o,E,e,i))break e;t=0;break r}a[Q>>2]=1,t=0;break r}t=o}}else o=0;A|t||xi(o)}Yr(i)}return s=f+32|0,t}function X(A,r,e,Q,f,t,b,k,n){var c,C,E=0,o=0,l=0,w=0,g=0,u=0,M=0,D=0,I=0,N=0,G=0,U=0,F=0;if(s=c=s-144|0,D=-1,C=wf(G=k||37664),!((0|r)<=0|(0|Q)<=0))if((0|Q)<=(0|f)&(2+(C<<1)|0)<=(0|f)){if(u=-1,w=iQ(M=(U=-1^C)+f|0,31289,0))if(D=iQ(f,31289,0)){if(g=n||G,n=f+D|0,(0|f)<=0)D=n;else{if(E=e+Q|0,1!=(0|f))for(k=-2&f;E=(e=(Q-1&(-1^Q))>>31^-1)+E|0,i[n-1|0]=B[0|E]&e,E=(Q=((e=e+Q|0)-1&(-1^e))>>31^-1)+E|0,i[0|(n=n-2|0)]=B[0|E]&Q,Q=e+Q|0,(0|k)!=(0|(o=o+2|0)););1&f&&(e=(Q-1&(-1^Q))>>31^-1,i[n-1|0]=B[e+E|0]&e)}if(E=B[0|D]-1>>31,!yr(c- -64|0,C,I=(k=D+1|0)+C|0,M,g)){if(!((0|C)<=0)){if(e=0,Q=0,C-1>>>0>=3)for(F=-4&C,o=0;i[0|(l=(n=c- -64|0)+Q|0)]=B[0|l]^B[Q+k|0],i[0|(N=(l=1|Q)+n|0)]=B[0|N]^B[k+l|0],i[0|(N=(l=2|Q)+n|0)]=B[0|N]^B[k+l|0],i[0|(l=(l=n)+(n=3|Q)|0)]=B[0|l]^B[k+n|0],Q=Q+4|0,(0|F)!=(0|(o=o+4|0)););if(n=3&C)for(;i[0|(o=(c- -64|0)+Q|0)]=B[0|o]^B[Q+k|0],Q=Q+1|0,(0|n)!=(0|(e=e+1|0)););}if(!yr(w,M,c- -64|0,C,g)){if(!((0|M)<=0)){if(n=0,Q=0,(f-C|0)-2>>>0>=3)for(k=-4&M,e=0;i[0|(o=Q+w|0)]=B[0|o]^B[Q+I|0],i[0|(g=(o=1|Q)+w|0)]=B[0|g]^B[o+I|0],i[0|(g=(o=2|Q)+w|0)]=B[0|g]^B[o+I|0],i[0|(g=(o=3|Q)+w|0)]=B[0|g]^B[o+I|0],Q=Q+4|0,(0|k)!=(0|(e=e+4|0)););if(e=3&M)for(;i[0|(k=Q+w|0)]=B[0|k]^B[Q+I|0],Q=Q+1|0,(0|e)!=(0|(n=n+1|0)););}if(n=0,he(t,b,c,G)){if(Q=0,t=0,b=0,e=0,C){if(k=3&C,C-1>>>0<3)e=0;else for(u=-4&C,e=0;l=B[e+w|0]^B[e+c|0]|Q,l|=B[(Q=1|e)+w|0]^B[Q+c|0],Q=(l|=B[(Q=2|e)+w|0]^B[Q+c|0])|B[(Q=3|e)+w|0]^B[Q+c|0],e=e+4|0,(0|u)!=(0|(b=b+4|0)););if(k)for(;Q=B[e+w|0]^B[e+c|0]|Q,e=e+1|0,(0|k)!=(0|(t=t+1|0)););e=255&Q}if(k=e?0:E,(0|C)>=(0|M))Q=-1;else{for(e=0,Q=C;E=(b=(1^(t=B[Q+w|0]))-1>>31)&(-1^n),a[c+140>>2]=E,u=a[c+140>>2],a[c+140>>2]=-1^E,k&=(n|=b)|t-1>>31,e=a[c+140>>2]&e|Q&u,(0|M)!=(0|(Q=Q+1|0)););Q=-1^e}if(t=(((e=M+U|0)-r^r|r^e)^e)>>31,a[c+140>>2]=t,b=a[c+140>>2],a[c+140>>2]=-1^t,I=(((u=Q+M|0)^r-u|r^u)^r)<0,t=a[c+140>>2]&r|e&b,(0|e)>=2)for(r=C+1|0,b=e-u|0,E=1;;){if((0|(o=M-E|0))>(0|r))for(g=-1^(G=((b&E)-1|0)<0?0:255),Q=r;F=B[0|(U=Q+w|0)],l=B[(Q+E|0)+w|0],a[c+140>>2]=G,N=a[c+140>>2],a[c+140>>2]=g,i[0|U]=F&a[c+140>>2]|l&N,(0|o)>(0|(Q=Q+1|0)););if(!((0|e)>(0|(E<<=1))))break}if(E=I?0:k&n,(0|t)>0)for(e=255&E,Q=0;k=B[0|(b=A+Q|0)],n=B[((r=Q+1|0)+C|0)+w|0],Q=e&(Q-u|u)>>31,a[c+140>>2]=Q,I=a[c+140>>2],a[c+140>>2]=-1^Q,i[0|b]=k&a[c+140>>2]|n&I,(0|t)!=(0|(Q=r)););Ve(4,153,121),_i(1&E)}}}}else Ve(4,153,65),D=0;else Ve(4,153,65),D=0;Of(c- -64|0,64),pQ(w,M),pQ(D,f),a[c+140>>2]=E,A=a[c+140>>2],a[c+140>>2]=-1^E,D=a[c+140>>2]|A&u}else Ve(4,153,121);return s=c+144|0,D}function S(A,r){var e,i=0,Q=0,f=0,B=0,t=0;e=A+r|0;A:{r:if(!(1&(i=a[A+4>>2]))){if(!(3&i))break A;r=(i=a[A>>2])+r|0;e:{if((0|(A=A-i|0))!=a[23341]){if(i>>>0<=255){if(f=a[A+8>>2],i=i>>>3|0,(0|(Q=a[A+12>>2]))!=(0|f))break e;a[23336]=a[23336]&OQ(-2,i);break r}if(t=a[A+24>>2],(0|(i=a[A+12>>2]))==(0|A))if((Q=a[(f=A+20|0)>>2])||(Q=a[(f=A+16|0)>>2])){for(;B=f,(Q=a[(f=(i=Q)+20|0)>>2])||(f=i+16|0,Q=a[i+16>>2]););a[B>>2]=0}else i=0;else Q=a[A+8>>2],a[Q+12>>2]=i,a[i+8>>2]=Q;if(!t)break r;f=a[A+28>>2];i:{if(a[(Q=93648+(f<<2)|0)>>2]==(0|A)){if(a[Q>>2]=i,i)break i;a[23337]=a[23337]&OQ(-2,f);break r}if(a[t+(a[t+16>>2]==(0|A)?16:20)>>2]=i,!i)break r}if(a[i+24>>2]=t,(Q=a[A+16>>2])&&(a[i+16>>2]=Q,a[Q+24>>2]=i),!(Q=a[A+20>>2]))break r;a[i+20>>2]=Q,a[Q+24>>2]=i;break r}if(3!=(3&(i=a[e+4>>2])))break r;return a[23338]=r,a[e+4>>2]=-2&i,a[A+4>>2]=1|r,void(a[e>>2]=r)}a[f+12>>2]=Q,a[Q+8>>2]=f}r:{if(!(2&(i=a[e+4>>2]))){if(a[23342]==(0|e)){if(a[23342]=A,r=a[23339]+r|0,a[23339]=r,a[A+4>>2]=1|r,a[23341]!=(0|A))break A;return a[23338]=0,void(a[23341]=0)}if(a[23341]==(0|e))return a[23341]=A,r=a[23338]+r|0,a[23338]=r,a[A+4>>2]=1|r,void(a[A+r>>2]=r);r=(-8&i)+r|0;e:if(i>>>0<=255){if(f=a[e+8>>2],i=i>>>3|0,(0|(Q=a[e+12>>2]))==(0|f)){a[23336]=a[23336]&OQ(-2,i);break e}a[f+12>>2]=Q,a[Q+8>>2]=f}else{if(t=a[e+24>>2],(0|e)==(0|(i=a[e+12>>2])))if((f=a[(Q=e+20|0)>>2])||(f=a[(Q=e+16|0)>>2])){for(;B=Q,(f=a[(Q=(i=f)+20|0)>>2])||(Q=i+16|0,f=a[i+16>>2]););a[B>>2]=0}else i=0;else Q=a[e+8>>2],a[Q+12>>2]=i,a[i+8>>2]=Q;if(t){f=a[e+28>>2];i:{if(a[(Q=93648+(f<<2)|0)>>2]==(0|e)){if(a[Q>>2]=i,i)break i;a[23337]=a[23337]&OQ(-2,f);break e}if(a[t+(a[t+16>>2]==(0|e)?16:20)>>2]=i,!i)break e}a[i+24>>2]=t,(Q=a[e+16>>2])&&(a[i+16>>2]=Q,a[Q+24>>2]=i),(Q=a[e+20>>2])&&(a[i+20>>2]=Q,a[Q+24>>2]=i)}}if(a[A+4>>2]=1|r,a[A+r>>2]=r,a[23341]!=(0|A))break r;return void(a[23338]=r)}a[e+4>>2]=-2&i,a[A+4>>2]=1|r,a[A+r>>2]=r}if(r>>>0<=255)return i=93384+(-8&r)|0,(Q=a[23336])&(r=1<<(r>>>3))?r=a[i+8>>2]:(a[23336]=r|Q,r=i),a[i+8>>2]=A,a[r+12>>2]=A,a[A+12>>2]=i,void(a[A+8>>2]=r);f=31,r>>>0<=16777215&&(i=r>>>8|0,i<<=B=i+1048320>>>16&8,f=28+((i=((i<<=f=i+520192>>>16&4)<<(Q=i+245760>>>16&2)>>>15|0)-(Q|f|B)|0)<<1|r>>>i+21&1)|0),a[A+28>>2]=f,a[A+16>>2]=0,a[A+20>>2]=0,B=93648+(f<<2)|0;r:{if((Q=a[23337])&(i=1<<f)){for(f=r<<(31==(0|f)?0:25-(f>>>1|0)|0),i=a[B>>2];;){if(Q=i,(-8&a[i+4>>2])==(0|r))break r;if(i=f>>>29|0,f<<=1,!(i=a[(B=Q+(4&i)|0)+16>>2]))break}a[B+16>>2]=A,a[A+24>>2]=Q}else a[23337]=i|Q,a[B>>2]=A,a[A+24>>2]=B;return a[A+12>>2]=A,void(a[A+8>>2]=A)}r=a[Q+8>>2],a[r+12>>2]=A,a[Q+8>>2]=A,a[A+24>>2]=0,a[A+12>>2]=Q,a[A+8>>2]=r}}function H(A,r,e,i,Q){var f,B,t=0,b=0,k=0,c=0,C=0,E=0,o=0,l=0,w=0,g=0,u=0,M=0,s=0,D=0,I=0,G=0,U=0,F=0,Z=0,d=0,W=0,V=0,Y=0;if(ka(i))return Ve(3,107,103),0;if(!a[(a[i>>2]+(a[i+4>>2]<<2)|0)-4>>2])return Ve(3,107,107),0;B=Q,ne(Q),(Q=A)||(Q=sA(B)),D=sA(B),f=sA(B);A:{r:if((E=sA(B))&&ni(E,i)){if(c=a[E>>2],b=a[E+4>>2],C=(0|(k=pi(a[(c+(b<<2)|0)-4>>2])))%32|0,g=32-k|0,!((0|b)<=0)){if(1!=(0|b))for(k=0-C|0,k|=k>>>8,u=-2&b;w=a[(I=(s=t<<2)+c|0)>>2],a[I>>2]=w<<g|l,l=a[(s=c+(4|s)|0)>>2],a[s>>2]=k&w>>>C|l<<g,t=t+2|0,l=k&l>>>C,(0|u)!=(0|(o=o+2|0)););1&b&&(a[(t=c+(t<<2)|0)>>2]=a[t>>2]<<g|l)}if(a[E+12>>2]=0,Xr(f,e,g)){if((0|(k=a[E+4>>2]))<(0|(b=a[f+4>>2])))t=b;else{if(!Gf(f,t=k+1|0))break r;IQ(a[f>>2]+(b<<2)|0,0,4+(k-b<<2)|0),a[f+4>>2]=t}if(c=(b=a[E>>2])+((Z=k-1|0)<<2)|0,M=1!=(0|k)?a[(b+(k<<2)|0)-8>>2]:M,b=a[f>>2],l=a[c>>2],Gf(Q,u=t-k|0)&&(i=a[i+12>>2],s=a[e+12>>2],a[Q+4>>2]=u,a[Q+12>>2]=i^s,i=a[Q>>2],Gf(D,d=k+1|0))){if((0|u)>0)for(e=(b+(t<<2)|0)-4|0,w=(Q=u<<2)+b|0,V=-4&k,I=3&k,U=i+Q|0;;){e=(b=e)-4|0,Q=-1;e:if((0|(i=a[b>>2]))!=(0|l)&&(C=a[e>>2],t=(0|b)==(0|w)?0:a[b-8>>2],i=Zi(M,0,Q=_A(i,C,l),0),o=t,!((0|(t=C-n(Q,l)|0))==(0|(c=N))&i>>>0<=(C=0|o)>>>0|t>>>0>c>>>0)))for(;;){if(Q=Q-1|0,(C=t)>>>0>(t=t+l|0)>>>0)break e;if(!((0|t)==(0|(c=c-(i>>>0<M>>>0)|0))&(i=i-M|0)>>>0>o>>>0|t>>>0<c>>>0))break}if(i=XA(a[D>>2],a[E>>2],k,Q),t=a[D>>2],a[t+(k<<2)>>2]=i,Y=Q-(i=rr(w=w-4|0,w,t,d))|0,(0|k)<=0)Q=a[D>>2];else{if(i=0-i|0,Q=a[D>>2],c=a[E>>2],o=0,t=0,F=0,Z>>>0>=3)for(;a[(C=t<<2)+Q>>2]=i&a[c+C>>2],a[(G=4|C)+Q>>2]=i&a[c+G>>2],a[(G=8|C)+Q>>2]=i&a[c+G>>2],a[(C|=12)+Q>>2]=i&a[c+C>>2],t=t+4|0,(0|V)!=(0|(F=F+4|0)););if(I)for(;a[(C=t<<2)+Q>>2]=i&a[c+C>>2],t=t+1|0,(0|I)!=(0|(o=o+1|0)););}if(a[b>>2]=oA(w,w,Q,k)+a[b>>2],a[(U=U-4|0)>>2]=Y,(0|u)==(0|(W=W+1|0)))break}if(a[f+4>>2]=k,a[f+12>>2]=s,t=1,!r)break A;e=0,c=0;e:{if((0|(E=a[f+4>>2]))<=(0|(i=(0|g)/32|0)))xf(r);else{if(Q=E-i|0,(0|r)!=(0|f)&&!Gf(r,Q))break e;if(b=31&g,o=a[f>>2]+(i<<2)|0,e=a[o>>2],M=a[r>>2],!((0|Q)<2)){if(k=(c=0-(C=0-g&31)|0)>>>8|c,w=1&(c=Q-1|0),(0|i)!=(E-2|0))for(D=-2&c,i=0,E=0;g=a[(u=4|(l=i<<2))+o>>2],a[l+M>>2]=k&g<<C|e>>>b,e=a[o+((i=i+2|0)<<2)>>2],a[M+u>>2]=k&e<<C|g>>>b,(0|D)!=(0|(E=E+2|0)););else i=0;w&&(E=e>>>b|0,e=a[4+((i<<=2)+o|0)>>2],a[i+M>>2]=k&e<<C|E)}a[M+(c<<2)>>2]=e>>>b,e=a[f+12>>2],a[r+4>>2]=Q,a[r+12>>2]=e}e=1}if(e)break A}}}t=0}return Yr(B),t?(A&&Hi(A),r&&Hi(r),t):0}function p(A,r,e){var Q,f=0,B=0,t=0,b=0,k=0,c=0,C=0,E=0,o=0,l=0,w=0,g=0;s=Q=s+-64|0,!A|(0|r)<=0||(i[0|A]=0);A:if(!(!e|!a[e+16>>2])){r:if(f=gr(e)){e:{i:{Q:{f:{a:{if(f>>>0<=1194){if(B=n(f,24),a[B+38488>>2])break a;Ve(8,102,101);break Q}if(!(B=a[23070]))break Q;if(a[Q+56>>2]=3,a[Q+24>>2]=f,a[Q+60>>2]=Q+16,B=Kr(B,Q+56|0)){B=a[B+4>>2]+4|0;break f}Ve(8,102,101);break Q}B=B+38484|0}if(k=a[B>>2])break i}if(f>>>0<=1194){if(f=n(f,24),!a[f+38488>>2])break e;f=f+38480|0}else{if(!(B=a[23070]))break r;if(a[Q+56>>2]=3,a[Q+24>>2]=f,a[Q+60>>2]=Q+16,!(f=Kr(B,Q+56|0)))break e;f=a[f+4>>2]}if(!(k=a[f>>2]))break r}A&&Li(A,k,r),E=ci(k);break A}Ve(8,104,101)}if((0|(o=a[e+12>>2]))<=0)xi(0);else{k=a[e+16>>2],e=0;r:for(;;){e:{if(B=(0|(t=i[0|k]))>=0,o=o-1|0)f=0,b=0;else if(f=0,b=0,(0|t)<0)break e;for(;;){t&=127;i:{if(b){if(Be(e,t))break i;break e}f|=t}k=k+1|0;i:{Q:{f:{if(!B){if(!(f>>>0<33554432|b)){if(e)break f;if(e=KQ(1))break f;e=0;break e}if(b)break Q;f<<=7,b=0;break i}if(!c){a:if(f>>>0>=80){if(b){if(t=2,pr(e,80))break a;break e}f=f-80|0,t=2}else f=n(t=((255&f)>>>0)/40|0,-40)+f|0;E=E+1|0,!A|(0|r)<2||(i[A+1|0]=0,i[0|A]=48|t,r=r-1|0,A=A+1|0)}if(b){s=l=s-32|0,t=iQ(4+((C=((f=((0|(f=n(Dr(e),3)))/10|0)+((0|f)/1e3|0)|0)+2|0)/9|0)<<2)|0,31289,0),f=iQ(w=f+5|0,31289,0);a:{B:if(t&&f)if(b=Sr(e)){t:{b:{k:{n:{if(ka(b))i[0|f]=48,i[f+1|0]=0;else{if(c=f,ra(b)&&(i[0|f]=45,c=f+1|0),B=t,!ka(b))for(;;){if((0|C)<B-t>>2)break t;if(g=ie(b,1e9),a[B>>2]=g,-1==(0|g))break b;if(B=B+4|0,ka(b))break}if(C=B-4|0,a[l+16>>2]=a[C>>2],(0|(B=ji(c,(w=f+w|0)-c|0,1867,l+16|0)))<0)break k;if((0|t)!=(0|C))for(B=B+c|0;;){if(C=C-4|0,a[l>>2]=a[C>>2],(0|(c=ji(B,w-B|0,1862,l)))<0)break n;if(B=B+c|0,(0|t)==(0|C))break}}SQ(t,31289,0),xi(b);break a}SQ(t,31289,0),xi(b);break B}SQ(t,31289,0),xi(b);break B}SQ(t,31289,0),xi(b);break B}SQ(t,31289,0),xi(b)}else SQ(t,31289,0),xi(0);else Ve(3,104,65),SQ(t,31289,0),xi(0);SQ(f,31289,0),f=0}if(s=l+32|0,!f)break e;B=ci(f),A?((0|r)>=2&&(i[0|A]=46,i[A+1|0]=0,r=r-1|0,A=A+1|0),Li(A,f,r),(0|r)<(0|B)?(A=A+r|0,r=0):(r=r-B|0,A=A+B|0)):A=0,SQ(f,31289,0),E=1+(B+E|0)|0}else a[Q>>2]=f,ji(f=Q+16|0,26,1857,Q),f=ci(f),!A|(0|r)<=0||(Li(A,Q+16|0,r),(0|r)<(0|f)?(A=A+r|0,r=0):(r=r-f|0,A=A+f|0)),E=f+E|0;if(c=1,(0|o)>0)continue r;xi(e);break A}if(b=1,!MQ(e,f))break e}if(!$Q(e,e,7))break e}if(B=(0|(t=i[0|k]))>=0,!((o=o-1|0)|(0|t)>=0))break}}break}xi(e),E=-1}}return s=Q- -64|0,E}function v(A,r,e,Q,f,t,b,k){var n,c=0,C=0;if(s=n=s-16|0,a[n+12>>2]=0,a[n+8>>2]=0,(0|hf(k))==(0|b)){A:if(c=iQ(b,31289,0))if((0|(t=of(b,t,c,k,1)))<=0)k=0;else{r:{if(114==(0|A)){if(36!=(0|t)){k=0,Ve(4,145,104);break A}if(Q){A=B[c+4|0]|B[c+5|0]<<8|B[c+6|0]<<16|B[c+7|0]<<24,r=B[0|c]|B[c+1|0]<<8|B[c+2|0]<<16|B[c+3|0]<<24,i[0|Q]=r,i[Q+1|0]=r>>>8,i[Q+2|0]=r>>>16,i[Q+3|0]=r>>>24,i[Q+4|0]=A,i[Q+5|0]=A>>>8,i[Q+6|0]=A>>>16,i[Q+7|0]=A>>>24,A=B[c+32|0]|B[c+33|0]<<8|B[c+34|0]<<16|B[c+35|0]<<24,i[Q+32|0]=A,i[Q+33|0]=A>>>8,i[Q+34|0]=A>>>16,i[Q+35|0]=A>>>24,A=B[c+28|0]|B[c+29|0]<<8|B[c+30|0]<<16|B[c+31|0]<<24,r=B[c+24|0]|B[c+25|0]<<8|B[c+26|0]<<16|B[c+27|0]<<24,i[Q+24|0]=r,i[Q+25|0]=r>>>8,i[Q+26|0]=r>>>16,i[Q+27|0]=r>>>24,i[Q+28|0]=A,i[Q+29|0]=A>>>8,i[Q+30|0]=A>>>16,i[Q+31|0]=A>>>24,A=B[c+20|0]|B[c+21|0]<<8|B[c+22|0]<<16|B[c+23|0]<<24,r=B[c+16|0]|B[c+17|0]<<8|B[c+18|0]<<16|B[c+19|0]<<24,i[Q+16|0]=r,i[Q+17|0]=r>>>8,i[Q+18|0]=r>>>16,i[Q+19|0]=r>>>24,i[Q+20|0]=A,i[Q+21|0]=A>>>8,i[Q+22|0]=A>>>16,i[Q+23|0]=A>>>24,A=B[c+12|0]|B[c+13|0]<<8|B[c+14|0]<<16|B[c+15|0]<<24,r=B[c+8|0]|B[c+9|0]<<8|B[c+10|0]<<16|B[c+11|0]<<24,i[Q+8|0]=r,i[Q+9|0]=r>>>8,i[Q+10|0]=r>>>16,i[Q+11|0]=r>>>24,i[Q+12|0]=A,i[Q+13|0]=A>>>8,i[Q+14|0]=A>>>16,i[Q+15|0]=A>>>24,a[f>>2]=36;break r}if(36!=(0|e)){k=0,Ve(4,145,131);break A}if(!$i(c,r,36))break r;k=0,Ve(4,145,104);break A}if(!(95!=(0|A)|18!=(0|t)|4!=B[0|c]|16!=B[c+1|0])){if(Q){A=B[c+6|0]|B[c+7|0]<<8|B[c+8|0]<<16|B[c+9|0]<<24,r=B[c+2|0]|B[c+3|0]<<8|B[c+4|0]<<16|B[c+5|0]<<24,i[0|Q]=r,i[Q+1|0]=r>>>8,i[Q+2|0]=r>>>16,i[Q+3|0]=r>>>24,i[Q+4|0]=A,i[Q+5|0]=A>>>8,i[Q+6|0]=A>>>16,i[Q+7|0]=A>>>24,A=B[c+14|0]|B[c+15|0]<<8|B[c+16|0]<<16|B[c+17|0]<<24,r=B[c+10|0]|B[c+11|0]<<8|B[c+12|0]<<16|B[c+13|0]<<24,i[Q+8|0]=r,i[Q+9|0]=r>>>8,i[Q+10|0]=r>>>16,i[Q+11|0]=r>>>24,i[Q+12|0]=A,i[Q+13|0]=A>>>8,i[Q+14|0]=A>>>16,i[Q+15|0]=A>>>24,a[f>>2]=16;break r}if(16!=(0|e)){k=0,Ve(4,145,131);break A}if(!$i(r,c+2|0,16))break r;k=0,Ve(4,145,104);break A}if(Q){if(!(r=TA(pe(A)))){k=0,Ve(4,145,117);break A}if(t>>>0<(e=wf(r))>>>0){k=0,Ve(4,145,143);break A}r=(t+c|0)-e|0}if(A=Re(n+8|0,n+12|0,A,r,e),k=a[n+12>>2],!A)break A;if((0|t)!=(0|k)||$i(a[n+8>>2],c,t)){Ve(4,145,104);break A}C=1,Q&&(yQ(Q,r,e),a[f>>2]=e),k=t;break A}C=1,k=0}else k=0,Ve(4,145,65);pQ(a[n+8>>2],k),pQ(c,b)}else Ve(4,145,119);return s=n+16|0,C}function z(A,r,e,Q){var f,t=0,b=0,k=0,n=0,c=0,C=0;s=f=s-16|0,a[f+12>>2]=r;A:if((t=a[Q+16>>2])&&(t=a[t+24>>2]))e=0|Ga[0|t](A,r,e,Q);else{r:{e:{i:{Q:{if(!(t=B[0|Q])){if(1==(0|(t=a[Q+4>>2])))break i;if(a[A>>2])break Q;e=-1;break A}if(!(b=a[A>>2])){e=-1;break A}if(5==(0|t))break e;t=a[Q+4>>2]}if(-4==(0|t)){A=a[A>>2],t=a[A>>2],a[e>>2]=t,A=A+4|0;break r}}t=a[e>>2];break r}t=a[b+4>>2],a[e>>2]=t}e=0;r:{e:switch(t-1|0){case 5:if(Q=a[A>>2],!(A=a[Q+16>>2])){e=-1;break A}if(e=-1,Q=a[Q+12>>2])break r;break A;case 0:if(e=-1,-1==(0|(A=a[A>>2])))break A;i:if(-4!=a[Q+4>>2]){if(Q=a[Q+20>>2],A){if((0|Q)<=0)break i;break A}if(!Q)break A}i[f+11|0]=A,Q=1,A=f+11|0;break r;case 2:if(Q=0,e=0,t=a[A>>2]){if(k=r?f+12|0:0,!((0|(r=a[t>>2]))<=0))if(8&(A=a[t+12>>2]))Q=7&A;else{for(Q=a[t+8>>2];;){if((0|(A=r))<=0)e=B[Q-1|0],A=0;else if(!(e=B[Q+(r=A-1|0)|0]))continue;break}Q=0,1&e||(Q=2,2&e?Q=1:4&e||(Q=3,8&e||(Q=4,16&e||(Q=5,32&e||(Q=6,64&e||(Q=e<<24>>31&7)))))),r=A}k&&(A=a[k>>2],i[0|A]=Q,A=A+1|0,(0|r)>0&&(A=yQ(A,a[t+8>>2],r)+r|0,i[0|(e=A-1|0)]=B[0|e]&255<<Q),a[k>>2]=A),e=r+1|0}break A;case 1:case 9:if(c=r?f+12|0:0,r=0,t=1,Q=a[A>>2],(n=a[Q+8>>2])&&(A=a[Q>>2])){if(r=i[0|n],e=1,1&i[Q+5|0]){if(k=255,!(r>>>0>4294967168)&&(e=0,-128==(0|r))){if(A>>>0>=2){if(t=3&(r=A-1|0),A-2>>>0<3)b=1,k=0;else for(C=-4&r,k=0,b=1,r=0;k=B[0|(Q=b+n|0)]|k|B[Q+1|0]|B[Q+2|0]|B[Q+3|0],b=b+4|0,(0|C)!=(0|(r=r+4|0)););if(t)for(;k=B[b+n|0]|k,b=b+1|0,(0|t)!=(0|(e=e+1|0)););e=0!=(0|k)}k=0-e|0}}else e=(0|r)<0;t=A+e|0,r=A}if(c&&(A=a[c>>2])){if(i[0|A]=k,r&&(Q=1&k,b=r+n|0,A=(A+e|0)+r|0,1&r?(A=A-1|0,e=(B[0|(b=b-1|0)]^255&k)+Q|0,i[0|A]=e,Q=e>>>8|0,e=r-1|0):e=r,1!=(0|r)))for(;r=(255&(B[b-1|0]^k))+Q|0,i[A-1|0]=r,A=A-2|0,r=(255&(B[0|(b=b-2|0)]^k))+(r>>>8|0)|0,i[0|A]=r,Q=r>>>8|0,e=e-2|0;);a[c>>2]=a[c>>2]+t}e=t;break A;case 4:break A;default:break e}if(A=a[A>>2],!(!(16&B[A+12|0])|2048!=a[Q+20>>2])){if(e=-2,!r)break A;a[A>>2]=0,a[A+8>>2]=r;break A}Q=a[A>>2],A=a[A+8>>2]}!r|!Q||yQ(r,A,Q),e=Q}return s=f+16|0,e}function j(A){var r=0,e=0,Q=0,f=0,t=0,k=0,c=0,C=0,o=0,l=0,w=0,g=0,u=0,M=0;if(!B[92204])return iQ(A,31289,0);A:{e=0;r:if(!((f=a[23057])>>>0<A>>>0)){for(Q=a[23060],t=c=a[23059];w=t,t=t-1|0,e=A>>>0>Q>>>0,Q<<=1,e;);if(e=0,!((0|w)<=0)){k=a[23058],e=t;e:{i:{for(;;){if(Q=a[(e<<2)+k>>2]){if((0|e)!=(0|t))break i;l=a[23061];break e}if(A=(0|e)>0,e=e-1|0,!A)break}e=0;break r}i:{for(;;){if(a[23059]<=(0|e))break A;if((A=Q-a[23056]|0)&(r=f>>>e|0)-1)break A;if(!(A=((A>>>0)/(r>>>0)|0)+(1<<e)|0)|A>>>0>=b[23063]|B[a[23062]+(A>>>3|0)|0]>>>(7&A)&1)break A;if(Oe(Q,e,a[23061]),(A=a[Q>>2])&&(a[A+4>>2]=a[Q+4>>2]),a[a[Q+4>>2]>>2]=A,r=a[Q>>2]){if(!((A=a[23058])>>>0<=(r=a[r+4>>2])>>>0&r>>>0<(a[23059]<<2)+A>>>0)&&(f=a[23056])>>>0>r>>>0|r>>>0>=f+a[23057]>>>0)break A}else A=a[23058];if(a[(e<<2)+A>>2]==(0|Q))break A;if((0|(A=e+1|0))>=a[23059])break A;if((r=Q-a[23056]|0)&(f=a[23057]>>>A|0)-1)break A;if(!(r=(e=2<<e)+((r>>>0)/(f>>>0)|0)|0)|r>>>0>=b[23063]|B[a[23062]+(r>>>3|0)|0]>>>(7&r)&1)break A;if(_e(Q,A,a[23061]),Ue((r=A<<2)+a[23058]|0,Q),a[r+a[23058]>>2]!=(0|Q)|(0|A)>=a[23059])break A;if((k=(Q=(f=a[23057]>>>A|0)+Q|0)-a[23056]|0)&f-1)break A;if(!(f=e+((k>>>0)/(f>>>0)|0)|0)|f>>>0>=b[23063]|B[a[23062]+(f>>>3|0)|0]>>>(7&f)&1)break A;if(_e(Q,A,a[23061]),Ue(r+a[23058]|0,Q),k=a[23058],a[r+k>>2]!=(0|Q))break A;if(c=Q-(r=(f=a[23057])>>>A|0)|0,M=1<<(7&(u=1^(C=e+((Q-(g=a[23056])>>>0)/(r>>>0)|0)|0))),l=a[23061],o=0,M&B[l+(C=C>>>3|0)|0]&&(o=0,B[C+a[23062]|0]&M||(o=n(r,e-1&u)+g|0)),(0|o)!=(0|c))break;if(e=A,(0|A)==(0|t))break i}break A}c=a[23059]}if((0|c)<(0|w))break A;if((A=(r=a[(t<<2)+k>>2])-a[23056]|0)&(e=f>>>t|0)-1)break A;if(!(A=((A>>>0)/(e>>>0)|0)+(1<<t)|0)|A>>>0>=b[23063]|!(B[(A>>>3|0)+l|0]>>>(7&A)&1))break A;if(_e(r,t,a[23062]),(A=a[r>>2])&&(a[A+4>>2]=a[r+4>>2]),a[a[r+4>>2]>>2]=A,(A=a[r>>2])&&!((A=a[A+4>>2])>>>0>=(e=a[23058])>>>0&A>>>0<e+(a[23059]<<2)>>>0)&&(e=a[23056])>>>0>A>>>0|A>>>0>=e+a[23057]>>>0)break A;if((A=a[23056])>>>0>r>>>0|A+a[23057]>>>0<=r>>>0)break A;i[0|r]=0,i[r+1|0]=0,i[r+2|0]=0,i[r+3|0]=0,i[r+4|0]=0,i[r+5|0]=0,i[r+6|0]=0,i[r+7|0]=0,e=Tr(r)}}return a[23053]=e+a[23053],r}Ia(),E()}function x(A,r){var e=0,Q=0,f=0,t=0,k=0,c=0,C=0,o=0,l=0,w=0,g=0,u=0;A:{if(A){if(!B[92204]||(e=a[23056])>>>0>A>>>0||!(e+a[23057]>>>0>A>>>0))return Of(A,r),void SQ(A,31289,0);if(Of(A,r=Tr(A)),a[23053]=a[23053]-r,(Q=a[23056])>>>0>A>>>0)break A;if((k=a[23057])+Q>>>0<=A>>>0)break A;r=(t=a[23059])-1|0;r:{if(!((e=a[23060])>>>0>(f=(A+k|0)-Q|0)>>>0)){for(e=(f>>>0)/(e>>>0)|0,f=a[23061];;){if(B[(e>>>3|0)+f|0]>>>(7&e)&1)break r;if(1&e)break;if(r=r-1|0,C=e>>>0<2,e=e>>>1|0,C)break r}break A}f=a[23061]}if(!((0|r)<(0|t)&(0|r)>=0))break A;if((e=A-Q|0)&(Q=k>>>r|0)-1)break A;if(!(e=((e>>>0)/(Q>>>0)|0)+(1<<r)|0)|e>>>0>=b[23063]|!(B[(e>>>3|0)+f|0]>>>(7&e)&1))break A;Oe(A,r,a[23062]),Ue(a[23058]+(r<<2)|0,A);r:{for(;;){if(t=1<<(7&(l=1^(Q=(f=1<<r)+(((o=A-(k=a[23056])|0)>>>0)/((e=a[23057]>>>r|0)>>>0)|0)|0))),C=a[23061],!(t&B[C+(w=Q>>>3|0)|0]))break r;if(c=t,t=a[23062],!k|c&(w=B[w+t|0]))break r;if(e-1&o|a[23059]<=(0|r)|(0|(t=!((u=1<<(7&(g=1^(c=f+(((l=n(e,l&(o=f-1|0)))>>>0)/(e>>>0)|0)|0))))&B[(c=c>>>3|0)+C|0])|B[t+c|0]&u?0:k+n(e,o&g)|0))!=(0|A)|!Q|w>>>(7&Q)&1|Q>>>0>=b[23063])break A;if(Oe(A,r,C),(e=a[A>>2])&&(a[e+4>>2]=a[A+4>>2]),a[a[A+4>>2]>>2]=e,(e=a[A>>2])&&!((e=a[e+4>>2])>>>0>=(Q=a[23058])>>>0&e>>>0<Q+(a[23059]<<2)>>>0)&&(Q=a[23056])>>>0>e>>>0|e>>>0>=Q+a[23057]>>>0)break A;if(a[23059]<=(0|r))break A;if((e=A-a[23056]|0)&(Q=a[23057]>>>r|0)-1)break A;if(!(e=f+((e>>>0)/(Q>>>0)|0)|0)|e>>>0>=b[23063]|B[a[23062]+(e>>>3|0)|0]>>>(7&e)&1)break A;if(Oe(e=k+l|0,r,a[23061]),(Q=a[e>>2])&&(a[Q+4>>2]=a[e+4>>2]),a[a[e+4>>2]>>2]=Q,(Q=a[e>>2])&&!((Q=a[Q+4>>2])>>>0>=(f=a[23058])>>>0&Q>>>0<f+(a[23059]<<2)>>>0)&&(f=a[23056])>>>0>Q>>>0|Q>>>0>=f+a[23057]>>>0)break A;if(i[0|(Q=(f=A>>>0>e>>>0)?A:e)]=0,i[Q+1|0]=0,i[Q+2|0]=0,i[Q+3|0]=0,i[Q+4|0]=0,i[Q+5|0]=0,i[Q+6|0]=0,i[Q+7|0]=0,a[23059]<(0|r)|(0|r)<=0)break A;if(r=r-1|0,(e=(A=f?e:A)-a[23056]|0)&(Q=a[23057]>>>r|0)-1)break A;if(!(e=((e>>>0)/(Q>>>0)|0)+(1<<r)|0)|e>>>0>=b[23063]|B[a[23062]+(e>>>3|0)|0]>>>(7&e)&1)break A;if(_e(A,r,a[23061]),Ue((e=r<<2)+a[23058]|0,A),a[e+a[23058]>>2]!=(0|A))break}break A}}return}Ia(),E()}function K(A,r,e,Q){A|=0,r|=0,e|=0,Q|=0;var f=0,t=0,b=0,k=0,n=0,c=0,C=0,E=0,o=0;if(!(!(f=a[22756])|105==(0|f)))return 0|Ga[0|f](A,r,e,Q);A:{if(!A){if(A=0,!(!(f=a[22755])|104==(0|f)))return 0|Ga[0|f](r,e,Q);if(!r)break A;return B[92200]||(i[92200]=1),0|G(r)}if(!r)return!(r=a[22757])|106==(0|r)?(m(A),0):(Ga[0|r](A,e,Q),0);if(A)if(r>>>0>=4294967232)a[23332]=48,f=0;else{b=r>>>0<11?16:r+11&-8,e=0,f=-8&(n=a[(Q=A-8|0)+4>>2]);r:if(3&n){k=Q+f|0;e:if(f>>>0>=b>>>0){if((e=f-b|0)>>>0<16)break e;a[Q+4>>2]=1&n|b|2,a[(f=Q+b|0)+4>>2]=3|e,a[k+4>>2]=1|a[k+4>>2],S(f,e)}else if(a[23342]!=(0|k))if(a[23341]!=(0|k)){if(2&(t=a[k+4>>2]))break r;if((c=f+(-8&t)|0)>>>0<b>>>0)break r;E=c-b|0;i:if(t>>>0<=255){if(e=a[k+8>>2],t=t>>>3|0,(0|(f=a[k+12>>2]))==(0|e)){a[23336]=a[23336]&OQ(-2,t);break i}a[e+12>>2]=f,a[f+8>>2]=e}else{if(C=a[k+24>>2],(0|k)==(0|(f=a[k+12>>2])))if((e=a[(t=k+20|0)>>2])||(e=a[(t=k+16|0)>>2])){for(;o=t,(e=a[(t=(f=e)+20|0)>>2])||(t=f+16|0,e=a[f+16>>2]););a[o>>2]=0}else f=0;else e=a[k+8>>2],a[e+12>>2]=f,a[f+8>>2]=e;if(C){e=a[k+28>>2];Q:{if(a[(t=93648+(e<<2)|0)>>2]==(0|k)){if(a[t>>2]=f,f)break Q;a[23337]=a[23337]&OQ(-2,e);break i}if(a[(a[C+16>>2]==(0|k)?16:20)+C>>2]=f,!f)break i}a[f+24>>2]=C,(e=a[k+16>>2])&&(a[f+16>>2]=e,a[e+24>>2]=f),(e=a[k+20>>2])&&(a[f+20>>2]=e,a[e+24>>2]=f)}}E>>>0<=15?(a[Q+4>>2]=1&n|c|2,a[(e=Q+c|0)+4>>2]=1|a[e+4>>2]):(a[Q+4>>2]=1&n|b|2,a[(e=Q+b|0)+4>>2]=3|E,a[(f=Q+c|0)+4>>2]=1|a[f+4>>2],S(e,E))}else{if((f=f+a[23338]|0)>>>0<b>>>0)break r;(e=f-b|0)>>>0>=16?(a[Q+4>>2]=1&n|b|2,a[(t=Q+b|0)+4>>2]=1|e,a[(f=Q+f|0)>>2]=e,a[f+4>>2]=-2&a[f+4>>2]):(a[Q+4>>2]=f|1&n|2,a[(e=Q+f|0)+4>>2]=1|a[e+4>>2],e=0),a[23341]=t,a[23338]=e}else{if((f=f+a[23339]|0)>>>0<=b>>>0)break r;a[Q+4>>2]=1&n|b|2,f=f-b|0,a[(e=Q+b|0)+4>>2]=1|f,a[23339]=f,a[23342]=e}e=Q}else{if(b>>>0<256)break r;if(f>>>0>=b+4>>>0&&(e=Q,f-b>>>0<=a[23456]<<1>>>0))break r;e=0}f=e+8|0,e||(f=0,(Q=G(r))&&(yQ(Q,A,r>>>0>(e=(3&(e=a[A-4>>2])?-4:-8)+(-8&e)|0)>>>0?e:r),m(A),f=Q))}else f=G(r);A=f}return 0|A}function O(A,r,e,Q){var f=0,b=0,k=0,n=0,c=0,C=0,E=0,o=0,l=0,w=0,g=0;A:{r:{e:if(o=Bf()){if(!re(o,ci(Q)+1|0))break A;i:for(;;){Q:{f:{a:{B:{if(!((0|(b=(f=B[0|Q])<<24>>24))<0)){if(k=a[A+4>>2],64&(f=t[k+(f<<1)>>1]))for(;;){n=Q,Q=Q+1|0;t:{b:if((0|(f=(c=B[n+1|0])<<24>>24))>=0){if(k=c<<1,c=a[A+4>>2],l=t[k+c>>1],k=1,8&l|(0|f)==(0|b))break b;if(!(32&l))break t;if(Q=n+2|0,(0|(f=(k=B[n+2|0])<<24>>24))<0|!(8&B[c+(k<<1)|0]))break t;k=2}else{if((0|f)!=(0|b))break t;f=b,k=1}Q=(k+n|0)+((0|f)==(0|b))|0;continue i}i[a[o+4>>2]+C|0]=f,C=C+1|0}if(1024&f)for(;;){t:{b:{if(c=Q+2|0,n=Q+1|0,f=(k=B[Q+1|0])<<24>>24,!(8&B[a[A+4>>2]+(k<<1)|0]&&(0|f)>=0)){if((0|f)!=(0|b))break t;if(B[Q+2|0]==(255&b))break b;f=b}Q=(0|f)==(0|b)?c:n;continue i}f=b,n=Q+2|0}Q=n,i[a[o+4>>2]+C|0]=f,C=C+1|0}if(32&f){if(n=(b=B[Q+1|0])<<24>>24,8&B[k+(b<<1)|0]&&(0|n)>=0)break B;f=13;t:{b:switch(b-98|0){case 12:f=10;break t;case 0:f=8;break t;case 16:break t;default:break b}f=116==(0|n)?9:n}Q=Q+2|0,i[a[o+4>>2]+C|0]=f,C=C+1|0;continue}if(8&f)break B;if(36==(0|b)){t:{if(123==(0|(E=B[Q+1|0])))l=125;else{if(40!=(0|E)){w=1,l=0,n=Q+1|0;break t}l=41}E=B[Q+2|0],w=0,n=Q+2|0}if(c=n,E<<24>>24<0){b=n;break f}for(;;){if(263&t[k+((f=255&E)<<1)>>1]){if(E=i[c+1|0],c=b=c+1|0,(0|E)>=0)continue;break f}break}if(58!=(0|f))break a;if(E=58,58!=B[c+1|0])break a;if(i[0|c]=0,f=c+2|0,g=58,(0|(E=i[c+2|0]))<0){b=f;break Q}for(k=a[A+4>>2],b=f;;){if(!(263&t[k+((255&E)<<1)>>1]))break Q;if(E=i[b+1|0],b=b+1|0,!((0|E)>=0))break}break Q}}i[a[o+4>>2]+C|0]=b,C=C+1|0,Q=Q+1|0;continue}i[a[o+4>>2]+C|0]=0,SQ(a[e>>2],31289,0),a[e>>2]=a[o+4>>2],SQ(o,31289,0),C=1;break e}b=c}c=0,f=n,n=r}if(i[0|b]=0,k=b,!w){if(E<<24>>24!=(0|l)){Q=102;break r}k=b+1|0}if(f=He(A,n,f),c&&(i[0|c]=g),i[0|b]=E,!f){Q=104;break r}if((Q=(ci(f)+(Q-k|0)|0)+a[o>>2]|0)>>>0>65536){Q=116;break r}if(!mA(o,Q)){Q=65;break r}if(Q=B[0|f])for(;i[a[o+4>>2]+C|0]=Q,C=C+1|0,Q=B[f+1|0],f=f+1|0,Q;);i[0|b]=E,Q=k}}return C}Ve(14,101,Q)}return lQ(o),0}function P(A,r,e,i,Q,f){A|=0,r|=0,i|=0,Q|=0,f|=0;var B,t=0,b=0,k=0,n=0,c=0,C=0,E=0,o=0,l=0,w=0,g=0;s=B=s-128|0;A:if(ia(e|=0)||ia(r)||ia(i))b=R(A,r,e,i,Q,f);else if(lf(i))if(o=Dr(e)){ne(Q),c=sA(Q),C=sA(Q),t=sA(Q),a[B>>2]=t;r:{e:{i:if(t){if(!(k=f)){if(!(k=QQ())){k=0;break e}if(!Br(k,i,Q))break e}if((!a[r+12>>2]&&(0|Fi(r,i))<0||(E=dQ(t,r,i,Q),r=t,E))&&yf(t,r,k,Q)){if(r=6,(0|o)>671||(r=5,(0|o)>239||(r=4,(0|o)>79||(r=3,E=1,!((0|o)<24))))){if(!Gi(c,t,t,k,Q))break i;for(n=1<<r-1,t=1;;){if(l=t<<2,E=sA(Q),a[l+B>>2]=E,!E)break i;if(!Gi(E,a[(B+l|0)-4>>2],c,k,Q))break i;if((0|n)==(0|(t=t+1|0)))break}E=r}if(n=a[i+4>>2],a[(a[i>>2]+(n<<2)|0)-4>>2]<0){if(!Gf(C,n))break i;if(b=a[C>>2],i=a[i>>2],a[b>>2]=0-a[i>>2],!((0|n)<2)){if(l=3&(t=n-1|0),r=1,n-2>>>0>=3)for(g=-4&t,c=0;a[(t=r<<2)+b>>2]=-1^a[i+t>>2],a[(w=t+4|0)+b>>2]=-1^a[i+w>>2],a[(w=t+8|0)+b>>2]=-1^a[i+w>>2],a[(t=t+12|0)+b>>2]=-1^a[i+t>>2],r=r+4|0,(0|g)!=(0|(c=c+4|0)););if(l)for(t=0;a[(c=r<<2)+b>>2]=-1^a[i+c>>2],r=r+1|0,(0|l)!=(0|(t=t+1|0)););}a[C+4>>2]=n}else if(!yf(C,32936,k,Q))break i;for(b=o-1|0,o=1,r=0;;){Q:{f:if(!gQ(e,b)){if(1&r)for(;;){if(!Gi(C,C,C,k,Q)){b=0;break i}if(!b)break Q;if(gQ(e,b=b-1|0))break f}for(;;){if(!b)break Q;if(gQ(e,b=b-1|0))break}}r=1,c=0,t=1,i=0;f:if(!(E>>>0<=1))for(;;){if((0|(n=b-r|0))<0)break f;if(t=(n=gQ(e,n))?t<<r-i|1:t,i=n?r:i,(0|E)==(0|(r=r+1|0)))break}f:if(!((0|i)<0|o)){for(;;){if(Gi(C,C,C,k,Q)){if(r=(0|i)==(0|c),c=c+1|0,!r)continue;break f}break}b=0;break i}if(!Gi(C,C,a[(t<<1&-4)+B>>2],k,Q)){b=0;break i}if(r=1,o=0,(0|(b=(-1^i)+b|0))>=0)continue}break}b=0!=(0|FQ(A,C,k,Q))}}if(f)break r}TQ(k)}Yr(Q)}else{if(b=1,Ff(i)){xf(A);break A}b=MQ(A,1)}else Ve(3,109,102);return s=B+128|0,0|b}function q(A,r,e,i){var Q=0,f=0,B=0,t=0,b=0,k=0,c=0,C=0,E=0,o=0,l=0,w=0,g=0,u=0,M=0,s=0,D=0,I=0,N=0,G=0,U=0,F=0,Z=0,d=0,W=0,V=0,Y=0,R=0,y=0,h=0,J=0,T=0,m=0,L=0,X=0,S=0,H=0;if(ka(e))r=ni(A,r);else{if(!ka(r)){ne(i),X=sA(i);A:if((Q=sA(i))&&Ne(Q,e)&&Ne(A,r)){r:if(!((0|(t=a[A+8>>2]))<=0))for(S=(0|(r=a[Q+8>>2]))>0?r:0,f=1,r=0;;){if((0|r)==(0|S))break r;if(B=(e=-1^(a[(e=r<<2)+a[Q>>2]>>2]|a[e+a[A>>2]>>2]))&f,f=e>>>31|0,b=e>>>30|0,k=e>>>29|0,c=e>>>28|0,C=e>>>27|0,E=e>>>26|0,o=e>>>25|0,l=e>>>24|0,w=e>>>23|0,g=e>>>22|0,u=e>>>21|0,M=e>>>20|0,s=e>>>19|0,D=e>>>18|0,I=e>>>17|0,N=e>>>16|0,G=e>>>15|0,U=e>>>14|0,F=e>>>13|0,Z=e>>>12|0,d=e>>>11|0,W=e>>>10|0,V=e>>>9|0,Y=e>>>8|0,R=e>>>7|0,y=e>>>6|0,h=e>>>5|0,J=e>>>4|0,T=e>>>3|0,m=e>>>2|0,L=(f&=b&=k&=c&=C&=E&=o&=l&=w&=g&=u&=M&=s&=D&=I&=N&=G&=U&=F&=Z&=d&=W&=V&=Y&=R&=y&=h&=J&=T&=m&=e=B&e>>>1)+(b+(k+(c+(C+(E+(o+(l+(w+(g+(u+(M+(s+(D+(I+(N+(G+(U+(F+(Z+(d+(W+(V+(Y+(R+(y+(h+(J+(T+(m+(e+(B+L|0)|0)|0)|0)|0)|0)|0)|0)|0)|0)|0)|0)|0)|0)|0)|0)|0)|0)|0)|0)|0)|0)|0)|0)|0)|0)|0)|0)|0)|0)|0)|0,(0|t)==(0|(r=r+1|0)))break}if(lr(A,A,L)&&lr(Q,Q,L)&&Gf(A,r=((0|(r=a[A+4>>2]))>(0|(e=a[Q+4>>2]))?r:e)+1|0)&&Gf(Q,r)&&Gf(X,r)){Cr(1&(-1^a[a[A>>2]>>2]),A,Q,r);r:if(e=Dr(A),f=Dr(Q),!((0|(e=n((0|e)>(0|f)?e:f,3)))<-3)){for(t=(0|(e=e+3|0))>0?e:0,e=1,f=0;;){if(B=1&a[a[Q>>2]>>2]&(0|e)>0&a[Q+4>>2]>0,a[A+12>>2]=B^a[A+12>>2],Cr(B,A,Q,r),PA(X,Q,A)){if(Cr(1&a[a[Q>>2]>>2]&a[Q+4>>2]>0,Q,X,r),!vr(Q,Q))break A;if(e=1+(B-1&e|0-B&0-e)|0,B=(0|f)==(0|t),f=f+1|0,!B)continue;break r}break}return Yr(i),0}a[A+12>>2]=0,$Q(A,A,L)&&(H=0!=(0|vr(A,A)))}}return Yr(i),H}r=ni(A,e)}return a[A+12>>2]=0,0!=(0|r)}function _(A,r){var e=0,Q=0,f=0,B=0,t=0,b=0,k=0,n=0,c=0,C=0,E=0,o=0,l=0,w=0,g=0,u=0,M=0,s=0,D=0,I=0,N=0,G=0,U=0,Z=0,d=0,W=0,V=0,Y=0,R=0,y=0,h=0,J=0,T=0,m=0,L=0,X=0;e=r+40|0,Q=a[r+104>>2],i[e+Q|0]=128,t=1,(f=Q+1|0)>>>0>=57&&(IQ(e+f|0,0,63-Q|0),F(r,e,1),f=0),IQ(e+f|0,0,56-f|0),Q=(Q=a[r+36>>2])<<24|Q<<8&16711680|Q>>>8&65280|Q>>>24,i[r+96|0]=Q,i[r+97|0]=Q>>>8,i[r+98|0]=Q>>>16,i[r+99|0]=Q>>>24,Q=(Q=a[r+32>>2])<<24|Q<<8&16711680|Q>>>8&65280|Q>>>24,i[r+100|0]=Q,i[r+101|0]=Q>>>8,i[r+102|0]=Q>>>16,i[r+103|0]=Q>>>24,F(r,e,1),a[r+104>>2]=0,Of(e,64),f=4,b=27,k=26,n=25,c=23,C=22,E=21,o=20,l=19,w=18,g=17,u=15,M=14,s=13,D=12,I=11,N=10,G=9,U=7,Z=6,d=5,W=3,e=A,V=2,Y=1,R=8,y=2,h=3,J=16,T=4,m=5,L=24,X=6,Q=r;A:{r:{e:{i:switch((B=a[r+108>>2])-28|0){case 0:break r;case 4:break e;default:break i}if(B>>>0<4|B>>>0>32)break A;for(f=0;e=(e=a[(f<<2)+r>>2])<<24|e<<8&16711680|e>>>8&65280|e>>>24,i[0|A]=e,i[A+1|0]=e>>>8,i[A+2|0]=e>>>16,i[A+3|0]=e>>>24,A=A+4|0,(f=f+1|0)>>>0<a[r+108>>2]>>>2>>>0;);break A}o=24,f=8,e=(e=a[r>>2])<<24|e<<8&16711680|e>>>8&65280|e>>>24,i[0|A]=e,i[A+1|0]=e>>>8,i[A+2|0]=e>>>16,i[A+3|0]=e>>>24,h=4,Q=r+4|0,e=A+4|0,b=31,k=30,n=29,L=28,c=27,C=26,E=25,l=23,w=22,g=21,J=20,u=19,M=18,s=17,D=16,I=15,N=14,G=13,y=3,R=12,U=11,Z=10,d=9,Y=2,W=7,V=6,t=5,T=5,m=6,X=7}B=e,e=a[Q>>2],i[0|B]=e>>>24,i[A+t|0]=e>>>16,i[A+V|0]=e>>>8,i[A+W|0]=e,e=a[(Y<<2)+r>>2],i[A+f|0]=e>>>24,i[A+d|0]=e>>>16,i[A+Z|0]=e>>>8,i[A+U|0]=e,e=a[(y<<2)+r>>2],i[A+R|0]=e>>>24,i[A+G|0]=e>>>16,i[A+N|0]=e>>>8,i[A+I|0]=e,e=a[(h<<2)+r>>2],i[A+D|0]=e>>>24,i[A+s|0]=e>>>16,i[A+M|0]=e>>>8,i[A+u|0]=e,e=a[(T<<2)+r>>2],i[A+J|0]=e>>>24,i[A+g|0]=e>>>16,i[A+w|0]=e>>>8,i[A+l|0]=e,e=a[(m<<2)+r>>2],i[A+o|0]=e>>>24,i[A+E|0]=e>>>16,i[A+C|0]=e>>>8,i[A+c|0]=e,r=a[(X<<2)+r>>2],i[A+L|0]=r>>>24,i[A+n|0]=r>>>16,i[A+k|0]=r>>>8,i[A+b|0]=r}}function $(A,r,e){var i=0,Q=0,B=0,b=0,k=0,c=0,C=0,E=0,o=0;A:{if(b=3,!((0|r)>3746||(b=4,(0|r)>1344||(b=5,(0|r)>475||(b=6,(0|r)>399||(b=7,(0|r)>346||(b=8,(0|r)>307))))))){if((0|r)<=1)break A;b=(0|r)>54?27:34}r:if((k=Ki(4096,31289))&&(c=Pi())){ne(c);e:if(sA(c)){for(o=r>>>0<32;;){if(!ea(A,r))break e;C=Q,Q=1;i:{if(o){for(;;){if(-1==(0|(i=me(A,t[(B=Q<<1)+32960>>1]))))break e;if(f[B+k>>1]=i,i=0,2048==(0|(Q=Q+1|0))){Q:{for(;;){f:{Q=1;a:{B=i;B:{if((0|i)<0)for(;;){if(!((B+t[(i=Q<<1)+k>>1]>>>0)%t[i+32960>>1]|0))break a;if(2048==(0|(i=Q+1|0)))break B;if(!((B+t[(i<<=1)+k>>1]>>>0)%t[i+32960>>1]|0))break a;Q=Q+2|0}for(;;){if(E=t[(i=Q<<1)+32960>>1],n(E,E)>>>0>WQ(A)+B>>>0)break B;if(!((B+t[i+k>>1]>>>0)%(E>>>0)|0))break a;if(2048==(0|(Q=Q+1|0)))break}}if(Be(A,B))break f;break e}if(i=B+2|0,B>>>0<=4294949430)continue;break Q}break}if((0|Dr(A))==(0|r))break i}if(Q=1,i=0,!ea(A,r))break}}break r}for(;;){if(-1==(0|(i=me(A,t[(B=Q<<1)+32960>>1]))))break e;if(f[B+k>>1]=i,i=0,2048==(0|(Q=Q+1|0))){for(;;){B=i,Q=1;Q:{f:{for(;;){if(!((B+t[(i=Q<<1)+k>>1]>>>0)%t[i+32960>>1]|0))break f;if(2048==(0|(i=Q+1|0)))break;if(!((B+t[(i<<=1)+k>>1]>>>0)%t[i+32960>>1]|0))break f;Q=Q+2|0}if(!Be(A,B))break e;if((0|Dr(A))==(0|r))break i;break Q}if(i=B+2|0,B>>>0<=4294949430)continue}break}if(Q=1,i=0,!ea(A,r))break}}break r}i:if(e){Q:switch(a[e>>2]-1|0){case 0:if(!(i=a[e+8>>2]))break i;Ga[0|i](0,C,a[e+4>>2]);break i;case 1:break Q;default:break e}if(!(0|Ga[a[e+8>>2]](0,C,e)))break e}if(Q=C+1|0,C=wA(A,b,c,e))break}if(i=1,-1!=(0|C))break r}i=0}else c=0;return SQ(k,31289,0),Yr(c),Gr(c),i}return Ve(3,141,118),0}function AA(A,r,e,i,Q){var f,B,t=0,b=0,k=0,c=0,C=0,E=0,o=0,l=0,w=0,g=0;s=f=s-16|0,4096&(B=a[e>>2])&&(a[f+4>>2]=A,A=f+4|0);A:{if(24&B){if(b=-1,-1!=(0|i))break A;i=a[e+4>>2],o=192&B}else o=-1==(0|i)?0:192&Q;if(c=(l=Q&B&2048)?2:1,C=-193&Q,6&B){if(!(t=a[A>>2])){b=0;break A}if(A=0,(0|Vf(t))>0)for(;;){if(a[f>>2]=UQ(t,k),b=-1,-1==(0|(Q=QA(f,0,a[e+16>>2],-1,C)))|(2147483647-Q|0)<(0|A))break A;if(A=A+Q|0,k=k+1|0,!((0|Vf(t))>(0|k)))break}if(b=-1,Q=2&B,-1==(0|(E=Ke(c,A,g=(k=!(w=16&B)&-1!=(0|i))?i:Q?17:16))))break A;r:{if(!w){if(b=E,r)break r;break A}if(!r|-1==(0|(b=Ke(c,E,i))))break A;jA(r,c,E,i,o),E=b}jA(r,c,A,g,k?o:0),b=a[e+16>>2];r:{e:{i:{if(Q&&!((0|Vf(t))<2)){if(!(Q=iQ(n(Vf(t),12),31289,0)))break r;if(i=iQ(A,31289,0))break i;i=Q;break e}if((0|Vf(t))<=0)break r;for(A=0;a[f+12>>2]=UQ(t,A),QA(f+12|0,r,b,-1,C),A=A+1|0,(0|Vf(t))>(0|A););break r}if(a[f+8>>2]=i,(0|Vf(t))>0)for(e=0,A=Q;a[f+12>>2]=UQ(t,e),a[A>>2]=a[f+8>>2],a[A+4>>2]=QA(f+12|0,f+8|0,b,-1,C),a[A+8>>2]=a[f+12>>2],A=A+12|0,e=e+1|0,(0|Vf(t))>(0|e););if(VA(Q,Vf(t),12,11),a[f+8>>2]=a[r>>2],(0|Vf(t))>0)for(k=0,A=Q;yQ(a[f+8>>2],a[A>>2],a[A+4>>2]),a[f+8>>2]=a[f+8>>2]+a[A+4>>2],A=A+12|0,k=k+1|0,(0|Vf(t))>(0|k););if(a[r>>2]=a[f+8>>2],4&B&&!((0|Vf(t))<=0))for(A=0,k=Q;e=a[k+8>>2],!t|(0|A)<0|a[t>>2]<=(0|A)||(a[a[t+4>>2]+(A<<2)>>2]=e,a[t+8>>2]=0),k=k+12|0,A=A+1|0,(0|Vf(t))>(0|A););SQ(Q,31289,0)}SQ(i,31289,0)}l&&(bf(r),w&&bf(r)),b=E}else if(Q=a[e+16>>2],16&B){if(b=0,!(Q=QA(A,0,Q,-1,C)))break A;if(!r|-1==(0|(b=Ke(c,Q,i))))break A;if(jA(r,c,Q,i,o),QA(A,r,a[e+16>>2],-1,C),!l)break A;bf(r)}else b=QA(A,r,Q,i,C|o)}return s=f+16|0,b}function rA(A,r,e,i,Q){var f,t=0,b=0,k=0,n=0;s=f=s-16|0;A:{if(!(t=a[A+16>>2])){s=k=s-144|0;r:if(i){a[k+8>>2]=a[i>>2],a[k+4>>2]=k+8;e:{i:{if(!(t=a[22990])||(0|(t=nr(t,k+8|0)))<0){if(!(t=Yi(k+4|0,90992,6,4,57)))break i;t=a[t>>2]}else t=UQ(a[22990],t);if(n=t)break e}Ve(6,157,156);break r}(t=Ki(40,31289))?(a[t+16>>2]=0,a[t>>2]=n,a[t+4>>2]=0,a[t+8>>2]=i,i&&_f(i),(n=a[n+8>>2])?(0|Ga[0|n](t))>0?b=t:(a[t>>2]=0,Ii(a[t+8>>2]),Ii(a[t+12>>2]),SQ(t,31289,0)):b=t):Ve(6,157,65)}if(s=k+144|0,t=b,a[A+16>>2]=t,!t){t=0;break A}}if(4&a[a[t>>2]+4>>2]|e||(t=f+12|0,e=-2,(b=a[i+12>>2])&&(b=a[b+92>>2])&&(e=0|Ga[0|b](i,3,0,t)),(0|e)>0&&(e=TA(pe(a[f+12>>2]))))){t=a[A+16>>2],b=a[t>>2];r:{e:{if(Q){if(i=a[b+68>>2]){if((0|Ga[0|i](t,A))>0)break e;t=0;break A}if(a[b+112>>2]){a[t+16>>2]=16,a[A+20>>2]=54;break r}if(t&&(Q=a[t>>2])&&a[Q+48>>2]?(a[t+16>>2]=16,i=1,(Q=a[Q+44>>2])&&(0|(i=0|Ga[0|Q](t)))<=0&&(a[t+16>>2]=0)):(Ve(6,143,150),i=-2),(0|i)>0)break r;t=0;break A}if(i=a[b+60>>2]){if((0|Ga[0|i](t,A))<=0){t=0;break A}a[a[A+16>>2]+16>>2]=64;break r}if(a[b+108>>2]){a[t+16>>2]=8,a[A+20>>2]=54;break r}if(t&&(Q=a[t>>2])&&a[Q+40>>2]?(a[t+16>>2]=8,i=1,(Q=a[Q+36>>2])&&(0|(i=0|Ga[0|Q](t)))<=0&&(a[t+16>>2]=0)):(Ve(6,141,150),i=-2),(0|i)>0)break r;t=0;break A}a[a[A+16>>2]+16>>2]=128}t=0,(0|Ie(a[A+16>>2],-1,248,1,0,e))<=0||(r&&(a[r>>2]=a[A+16>>2]),4&B[a[a[A+16>>2]>>2]+4|0]?t=1:Vr(A,e)&&(t=1,e=a[A+16>>2],(r=a[a[e>>2]+128>>2])&&(t=0|Ga[0|r](e,A))))}else t=0,Ve(6,161,158)}return s=f+16|0,t}function eA(A,r,e){var Q,f=0,t=0,b=0,k=0,n=0,c=0,C=0;s=Q=s-16|0,(f=a[r+16>>2])&&(t=a[f+16>>2])||(t=0),n=1;A:{r:{e:{i:{Q:{f:switch(i[0|r]){case 4:if(!f)break A;if(!(e=a[f+4>>2]))break A;if(!(0|Ga[0|e](A,r)))break r;break A;case 0:if(t=a[r+8>>2]){if((e=4096&(r=a[t>>2]))&&(a[Q+8>>2]=A,A=Q+8|0),1&r){Hr(A,t);break A}if(768&r){a[A>>2]=0;break A}if(6&r){if(!(r=kf())){Ve(13,133,65);break r}a[A>>2]=r;break A}if(!eA(A,a[t+16>>2],e))break r;break A}if(!sr(A,r,e))break r;break A;case 5:if(!sr(A,r,e))break r;break A;case 2:a:if(t)switch(0|Ga[0|t](0,A,r,0)){case 2:break A;case 0:break i;default:break a}if(e)IQ(a[A>>2],0,a[r+20>>2]);else if(f=Ki(a[r+20>>2],31289),a[A>>2]=f,!f)break r;if(Tf(A,-1,r),!t)break A;if(!(0|Ga[0|t](1,A,r,0)))break Q;break A;case 1:case 6:break f;default:break A}f:if(t)switch(0|Ga[0|t](0,A,r,0)){case 2:break A;case 0:break i;default:break f}f:{if(!e){if(f=Ki(a[r+20>>2],31289),a[A>>2]=f,!f)break r;if((0|te(A,0,r))>=0)break f;SQ(a[A>>2],31289,0),a[A>>2]=0;break r}if(IQ(a[A>>2],0,a[r+20>>2]),(0|te(A,0,r))<0)break r}if(A&&(b=a[A>>2])&&(!(f=a[r+16>>2])|!(2&B[f+4|0])||(f=b+a[f+20>>2]|0,a[f+8>>2]=1,a[f>>2]=0,a[f+4>>2]=0)),a[r+12>>2]>0)for(f=a[r+8>>2];;){if(b=Lf(A,f),(c=4096&(k=a[f>>2]))&&(a[Q+12>>2]=b,b=Q+12|0),1&k)Hr(b,f);else if(768&k)a[b>>2]=0;else if(6&k){if(!(k=kf())){Ve(13,133,65);break e}a[b>>2]=k}else if(!eA(b,a[f+16>>2],c))break e;if(f=f+20|0,!((0|(C=C+1|0))<a[r+12>>2]))break}if(!t)break A;if(0|Ga[0|t](1,A,r,0))break A}dA(A,r,e)}n=0,Ve(13,121,100);break A}dA(A,r,e)}n=0,Ve(13,121,65)}return s=Q+16|0,n}function iA(A,r,e,i,Q,f,B){var t,b,k=0,n=0,c=0,C=0,E=0,o=0,l=0,w=0;s=t=s-80|0;A:if(!((0|(b=wf(A)))<=0||!(c=ta())|!(E=ta())|!(C=ta()))){Kf(C,8),s=o=s-16|0;r:if(k=Ki(40,31289))if(a[k+28>>2]=1,a[k+8>>2]=1,a[k>>2]=0,a[k+4>>2]=0,n=uf(),a[k+36>>2]=n,n){a[o+12>>2]=0,a[k+24>>2]&&(n=a[k+12>>2])&&(n=a[n+88>>2])&&(Ga[0|n](k),a[k+24>>2]=0);e:{i:{if(855!=a[k+4>>2]||!(n=a[k+12>>2])){if(!(n=Je(o+12|0,855)))break i;a[k+12>>2]=n,l=a[n>>2],a[k+4>>2]=855,a[k>>2]=l,a[k+16>>2]=a[o+12>>2]}if(!(n=a[n+128>>2])){Ve(6,191,150);break e}if(0|Ga[0|n](k,r,e)){n=k;break r}Ve(6,191,180);break e}Ve(6,158,156)}r=a[k+8>>2],a[k+8>>2]=r-1,1!=(0|r)&&(n=0,(0|r)>1)||((r=a[k+12>>2])&&(r=a[r+88>>2])&&(Ga[0|r](k),a[k+24>>2]=0),Yf(a[k+36>>2]),Ti(a[k+32>>2],55),n=0,SQ(k,31289,0))}else n=0,Ve(6,106,65),SQ(k,31289,0);else Ve(6,106,65);if(s=o+16|0,n){if(rA(C,0,A,n,0)&&ur(c,C)&&(!i||nf(c,i,Q))&&tr(c,t+16|0,t+12|0)&&ur(c,C)){r:{if(!i)for(;;){if(!nf(c,t+16|0,a[t+12>>2]))break A;if(B>>>0<=b>>>0)break r;if(!ur(E,c))break A;if(!tr(c,f,t+8|0))break A;if(A=a[t+8>>2],!tr(E,t+16|0,t+12|0))break A;if(f=A+f|0,B=B-A|0,!ur(c,C))break A}for(;;){if(!nf(c,t+16|0,a[t+12>>2]))break A;if(B>>>0>b>>>0){if(!ur(E,c))break A;if(!nf(c,i,Q))break A;if(!tr(c,f,t+8|0))break A;if(A=a[t+8>>2],!tr(E,t+16|0,t+12|0))break A;if(f=A+f|0,B=B-A|0,ur(c,C))continue;break A}break}if(!nf(c,i,Q))break A}tr(c,t+16|0,t+12|0)&&(yQ(f,t+16|0,B),w=1)}}else n=0}return Ii(n),Qa(c),Qa(E),Qa(C),Of(t+16|0,64),s=t+80|0,w}function QA(A,r,e,Q,f){var t,b=0,k=0,c=0,C=0,E=0,o=0,l=0,w=0,g=0;s=t=s-16|0,o=a[e+16>>2],C=i[0|e];A:if(a[A>>2]|!C){E=o?a[o+16>>2]:E;r:switch(0|C){case 0:if(b=a[e+8>>2]){b=AA(A,r,b,Q,f);break A}b=oe(A,r,e,Q,f);break A;case 5:if(b=-1,-1!=(0|Q)){Ve(13,144,230);break A}b=oe(A,r,e,-1,f);break A;case 2:if(b=-1,-1!=(0|Q)){Ve(13,144,230);break A}if(E&&(b=0,!(0|Ga[0|E](6,A,e,0))))break A;if(b=0,!((0|(Q=Wf(A,e)))<0|(0|Q)>=a[e+12>>2])){b=AA(Lf(b=A,A=a[e+8>>2]+n(Q,20)|0),r,A,-1,f);break A}if(!E)break A;Ga[0|E](7,A,e,0);break A;case 4:b=0|Ga[a[o+20>>2]](A,r,e,Q,f);break A;case 6:C=(b=2048&f)?2:1,b=b>>>11|0;break;case 1:break r;default:break A}if(o=b,b=0,k=t+12|0,A&&(l=a[A>>2])&&(!(c=a[e+16>>2])|!(2&B[c+4|0])||(c=l+a[c+20>>2]|0,a[c+8>>2]||(r&&(yQ(a[r>>2],a[c>>2],a[c+4>>2]),a[r>>2]=a[r>>2]+a[c+4>>2]),w=1,k&&(a[k>>2]=a[c+4>>2])))),w)b=a[t+12>>2];else if(a[t+12>>2]=0,!E||0|Ga[0|E](6,A,e,0)){if(c=(l=-1==(0|Q))?-193&f:f,a[e+12>>2]<=0)b=a[t+12>>2];else for(f=a[e+8>>2];;){if(!(k=Lr(A,f,1))){b=0;break A}if(b=-1,-1==(0|(k=AA(Lf(A,k),0,k,-1,c))))break A;if((0|k)>(2147483647-(w=a[t+12>>2])|0))break A;if(b=k+w|0,a[t+12>>2]=b,f=f+20|0,!((0|(g=g+1|0))<a[e+12>>2]))break}if(!(!r|-1==(0|(f=Ke(C,b,Q=l?16:Q))))){if(jA(r,C,a[t+12>>2],Q,c),a[e+12>>2]>0)for(C=a[e+8>>2],b=0,Q=0;;){if(!(k=Lr(A,C,1)))break A;if(AA(Lf(A,k),r,k,-1,c),C=C+20|0,!((0|(Q=Q+1|0))<a[e+12>>2]))break}if(o&&bf(r),E&&(b=0,!(0|Ga[0|E](7,A,e,0))))break A}b=f}}return s=t+16|0,b}function fA(A,r,e){var i=0,Q=0,f=0,B=0,t=0,b=0,k=0,n=0,c=0,C=0,E=0,o=0,l=0,w=0,g=0;B=Zi(a[e>>2],0,a[r>>2],0),Q=N,a[A>>2]=B,b=Zi(a[e>>2],0,a[r+4>>2],0),t=N,B=Zi(a[e+4>>2],0,a[r>>2],0)+Q|0,i=N,Q=Q>>>0>(f=B)>>>0?i+1|0:i,i=t,i=(b=f+b|0)>>>0<f>>>0?i+1|0:i,a[A+4>>2]=b,k=Zi(a[e+8>>2],0,a[r>>2],0),n=N,B=Zi(a[e+4>>2],0,a[r+4>>2],0),t=N,c=i,b=Q+i|0,i=Zi(a[e>>2],0,a[r+8>>2],0),f=N,f=i>>>0>(Q=b+i|0)>>>0?f+1|0:f,i=t,t=B=Q+B|0,Q=Q>>>0>B>>>0?i+1|0:i,i=n,i=(B=B+k|0)>>>0<t>>>0?i+1|0:i,a[A+8>>2]=B,C=Zi(a[e>>2],0,a[r+12>>2],0),l=N,w=Zi(a[e+4>>2],0,a[r+8>>2],0),g=N,k=Zi(a[e+8>>2],0,a[r+4>>2],0),B=N,n=f,c=i+(o=Q+(E=f+(b>>>0<c>>>0)|0)|0)|0,i=Zi(a[e+12>>2],0,a[r>>2],0),Q=N,t=i>>>0>(f=c+i|0)>>>0?Q+1|0:Q,i=B,B=f=(Q=f)+k|0,f=Q>>>0>f>>>0?i+1|0:i,i=g,Q=i=(Q=B)>>>0>(b=Q+w|0)>>>0?i+1|0:i,i=l,i=(k=b+C|0)>>>0<b>>>0?i+1|0:i,a[A+12>>2]=k,C=Zi(a[e+12>>2],0,a[r+4>>2],0),l=N,w=Zi(a[e+8>>2],0,a[r+8>>2],0),g=N,B=C,b=c>>>0<o>>>0,c=n>>>0>E>>>0,n=i+(o=Q+(E=f+(C=t+(k=b+(E>>>0>o>>>0?c?2:1:c)|0)|0)|0)|0)|0,i=Zi(a[e+4>>2],0,a[r+12>>2],0),f=N,f=i>>>0>(Q=n+i|0)>>>0?f+1|0:f,i=Q,Q=g,Q=i>>>0>(t=i+w|0)>>>0?Q+1|0:Q,i=l,i=(B=B+t|0)>>>0<t>>>0?i+1|0:i,a[A+16>>2]=B,c=Zi(a[e+8>>2],0,a[r+12>>2],0),b=N,B=k>>>0>C>>>0,t=i+(n=Q+(k=f+(B=((E>>>0>o>>>0)+(C>>>0>E>>>0?B?2:1:B)|0)+(n>>>0<o>>>0)|0)|0)|0)|0,Q=Zi(a[e+12>>2],0,a[r+8>>2],0),i=N,Q=i=Q>>>0>(f=t+Q|0)>>>0?i+1|0:i,i=b,i=(b=f+c|0)>>>0<f>>>0?i+1|0:i,a[A+20>>2]=b,f=t>>>0<n>>>0,t=B>>>0>k>>>0,i=i+(f=Q+(t=f+(k>>>0>n>>>0?t?2:1:t)|0)|0)|0,r=Zi(a[e+12>>2],0,a[r+12>>2],0),Q=N,Q=r>>>0>(e=i+r|0)>>>0?Q+1|0:Q,a[A+24>>2]=e,e=f>>>0<t>>>0,a[A+28>>2]=(i>>>0<f>>>0?e?2:1:e)+Q}function aA(A,r,e,i,Q,f,B){var t,b,k=0,c=0,C=0,E=0,o=0,l=0,w=0,g=0;if((b=Q|f)|8!=(0|i)){t=(0|i)/2|0;A:{if((0|i)<=15){if(ee(A,r,i+Q|0,e,i+f|0),(0|(r=Q+f|0))>=0)break A;return void IQ((r+(i<<1)<<2)+A|0,0,0-(r<<2)|0)}l=1,w=1;r:{e:{i:{Q:{f:{a:switch(E=e+(k=t<<2)|0,o=f+t|0,4+(n(jr(r,c=k+r|0,C=Q+t|0,g=0-Q|0),3)+jr(E,e,o,f)|0)|0){case 0:break a;case 8:break e;case 1:case 3:case 4:case 5:case 7:break i;case 6:break Q;case 2:break f;default:break r}RA(B,c,r,C,Q),RA(B+k|0,e,E,o,0-f|0);break r}RA(B,c,r,C,Q),RA(B+k|0,E,e,o,f),w=0;break r}RA(B,r,c,C,g),w=0,RA(B+k|0,e,E,o,0-f|0);break r}l=0;break r}RA(B,r,c,C,g),RA(B+k|0,E,e,o,f)}if(8!=(0|(k=-2&i))|b?16!=(0|k)|b?(C=(o=i<<2)+B|0,k=(i<<3)+B|0,l?aA(C,B,(t<<2)+B|0,t,0,0,k):IQ(C,0,o),aA(A,r,e,t,0,0,k),aA((i<<2)+A|0,c,E,t,Q,f,k)):(Q=(i<<2)+B|0,l?Z(Q,B,B+32|0):(a[Q>>2]=0,a[Q+4>>2]=0,a[Q+56>>2]=0,a[Q+60>>2]=0,a[Q+48>>2]=0,a[Q+52>>2]=0,a[Q+40>>2]=0,a[Q+44>>2]=0,a[Q+32>>2]=0,a[Q+36>>2]=0,a[Q+24>>2]=0,a[Q+28>>2]=0,a[Q+16>>2]=0,a[Q+20>>2]=0,a[Q+8>>2]=0,a[Q+12>>2]=0),Z(A,r,e),Z((i<<2)+A|0,c,E)):(Q=(i<<2)+B|0,l?fA(Q,B,B+16|0):(a[Q>>2]=0,a[Q+4>>2]=0,a[Q+24>>2]=0,a[Q+28>>2]=0,a[Q+16>>2]=0,a[Q+20>>2]=0,a[Q+8>>2]=0,a[Q+12>>2]=0),fA(A,r,e),fA((i<<2)+A|0,c,E)),r=(e=i<<2)+B|0,e=oA(B,A,A+e|0,i),r=w?oA(r,r,B,i)+e|0:e-rr(r,B,r,i)|0,(r=oA(e=(t<<2)+A|0,e,(i<<2)+B|0,i)+r|0)&&(A=(e=r)+(r=a[(i=(i+t<<2)+A|0)>>2])|0,a[i>>2]=A,!(A>>>0>=r>>>0)))for(;A=a[i+4>>2]+1|0,a[i+4>>2]=A,i=i+4|0,!A;);}}else Z(A,r,e)}function BA(A,r,e){var Q,f=0,a=0,t=0,b=0,k=0,c=0,C=0,E=0,o=0,l=0,w=0,g=0;s=Q=s-32|0,k=ci(e);A:{r:{e:{i:{Q:{if(((f=B[0|e])-48&255)>>>0<=2){if((0|k)<2){Ve(13,100,138);break r}if(2==(0|k))break A;for(o=e+2|0,k=k-2|0,w=n(f,40)-1920|0,E=B[e+1|0],b=Q,g=f>>>0>49,l=24;;){if(f=131,14!=(0|(e=(E<<24>>24)-32|0))&&e)break Q;e=0,a=0,f=k;f:{for(;;){if(E=B[0|o],o=o+1|0,k=f-1|0,!(t=E-32|0)|14==(0|t))break f;if(!Mf(t=E<<24>>24,4)){f=130;break Q}a:{B:{if(e>>>0<429496721|a){if(!a)break B}else{if(!c&&!(c=KQ(1))){c=0;break i}if(a=1,!MQ(c,e))break i}if(!Wi(c,10))break i;if(!Be(c,t-48|0))break i;break a}e=(t+n(e,10)|0)-48|0,a=0}if(t=f>>>0>1,f=k,!t)break}k=0}f:{a:{if(C){if(a)break a}else{if(!(e>>>0<=39|g)){f=147;break Q}if(a){if(!Be(c,w))break i;break a}e=e+w|0}for(f=0;i[f+b|0]=127&e,f=f+1|0,a=e>>>0>127,e=e>>>7|0,a;);break f}if((0|l)<(0|(f=((t=Dr(c))+6|0)/7|0))&&((0|b)!=(0|Q)&&SQ(b,31289,0),e=0,!(b=iQ(l=f+32|0,31289,0))))break e;if(e=0,a=f,t+12>>>0<13)f=0;else for(;;){if(-1==(0|(t=ie(c,128))))break i;if(i[e+b|0]=t,e=e+1|0,!(a=a-1|0))break}}if(e=f+C|0,A){if((0|r)<(0|e)){f=107;break Q}if((0|f)>=2)for(;e=f-1|0,i[A+C|0]=128|B[e+b|0],C=C+1|0,a=f>>>0>2,f=e,a;);i[A+C|0]=B[0|b],e=C+1|0}if(C=e,!((0|k)>0))break}if((0|b)==(0|Q))break A;SQ(b,31289,0);break A}Ve(13,100,122);break A}Ve(13,100,f)}if((0|b)==(0|Q))break r;e=b}C=0,SQ(A=e,31289,0);break A}C=0}return xi(c),s=Q+32|0,C}function tA(A,r){var e=0,i=0,Q=0,f=0,B=0,t=0,b=0,k=0,n=0,c=0,C=0,E=0,o=0,l=0;Q=Zi(e=a[r>>2],0,e,0),e=N,a[A>>2]=Q,B=Zi(a[r>>2],0,a[r+4>>2],0),b=f=N,Q=e,i=e+B|0,e=f,f=e=Q>>>0>i>>>0?e+1|0:e,e=b,e=(i=i+B|0)>>>0<B>>>0?e+1|0:e,a[A+4>>2]=i,B=Zi(a[r>>2],0,a[r+8>>2],0),t=i=N,E=e,C=f+e|0,Q=Zi(e=a[r+4>>2],0,e,0),e=N,b=f=C+Q|0,f=Q>>>0>f>>>0?e+1|0:e,e=i,i=e=(Q=B+b|0)>>>0<b>>>0?e+1|0:e,e=t,e=(Q=B+Q|0)>>>0<B>>>0?e+1|0:e,a[A+8>>2]=Q,B=Zi(a[r+4>>2],0,a[r+8>>2],0),o=Q=N,k=Q,t=Zi(a[r>>2],0,a[r+12>>2],0),n=f,C=e+(c=i+(E=f+(C>>>0<E>>>0)|0)|0)|0,e=Q=N,l=f=t+(i=C)|0,f=f>>>0<i>>>0?e+1|0:e,b=i=t+l|0,i=e=i>>>0<t>>>0?e+1|0:e,e=k,k=Q=B+b|0,Q=e=Q>>>0<b>>>0?e+1|0:e,e=o,e=(t=B+k|0)>>>0<B>>>0?e+1|0:e,a[A+12>>2]=t,B=Zi(a[r+4>>2],0,a[r+12>>2],0),o=t=N,n=n>>>0>E>>>0,C=e+(k=Q+(l=i+(c=f+(n=(c>>>0>C>>>0)+(c>>>0<E>>>0?n?2:1:n)|0)|0)|0)|0)|0,i=Zi(e=a[r+8>>2],0,e,0),e=N,f=(Q=f=C+i|0)>>>0<i>>>0?e+1|0:e,e=t,t=i=Q+B|0,i=e=i>>>0<Q>>>0?e+1|0:e,e=o,e=(b=B+t|0)>>>0<B>>>0?e+1|0:e,a[A+16>>2]=b,b=Zi(a[r+8>>2],0,a[r+12>>2],0),o=B=N,n=c>>>0<n>>>0,c=e+(i=i+(f=f+(k=((k>>>0<l>>>0)+(c>>>0>l>>>0?n?2:1:n)|0)+(k>>>0>C>>>0)|0)|0)|0)|0,e=B,B=e=(Q=(t=c)+b|0)>>>0<t>>>0?e+1|0:e,e=o,e=(t=Q+b|0)>>>0<b>>>0?e+1|0:e,a[A+20>>2]=t,t=f>>>0<k>>>0,i=e+(B=B+(f=(i>>>0>c>>>0)+(f>>>0>i>>>0?t?2:1:t)|0)|0)|0,Q=Zi(r=a[r+12>>2],0,r,0),e=N,e=(r=i+Q|0)>>>0<Q>>>0?e+1|0:e,a[A+24>>2]=r,f=f>>>0>B>>>0,a[A+28>>2]=(i>>>0<B>>>0?f?2:1:f)+e}function bA(A,r,e,i,Q,f,B){var t=0,b=0,k=0,c=0,C=0,E=0;if((0|i)<=7)ee(A,r,i+Q|0,e,i+f|0);else{E=1;A:{r:{e:{i:{Q:switch(c=e+(t=i<<2)|0,b=f-i|0,4+(n(jr(r,k=t+r|0,Q,C=i-Q|0),3)+jr(c,e,f,b)|0)|0){case 7:case 8:break r;case 3:case 4:case 5:case 6:break e;case 1:case 2:break i;case 0:break Q;default:break A}RA(B,k,r,Q,Q-i|0),RA(B+t|0,e,c,f,i-f|0);break A}RA(B,k,r,Q,Q-i|0),RA(B+t|0,c,e,f,b),E=0;break A}RA(B,r,k,Q,C),RA(B+t|0,e,c,f,i-f|0),E=0;break A}RA(B,r,k,Q,C),RA(B+t|0,c,e,f,b)}t=i<<1;A:if(8!=(0|i))if(aA((b=t<<2)+B|0,B,(i<<2)+B|0,i,0,0,C=(i<<4)+B|0),aA(A,r,e,i,0,0,C),(0|(e=i>>>1|0))!=(0|(r=(0|Q)>(0|f)?Q:f)))if(b=A+b|0,(0|r)>(0|e))bA(b,k,c,e,Q-e|0,f-e|0,C),IQ(((r=Q+f|0)+t<<2)+A|0,0,t-r<<2);else if(r=IQ(b,0,i<<3),(0|Q)>15|(0|f)>15){for(;;){if(!((0|Q)<=(0|(e=(0|e)/2|0))&(0|e)>=(0|f))){bA(r,k,c,e,Q-e|0,f-e|0,C);break A}if(!((0|e)!=(0|Q)&(0|e)!=(0|f)))break}aA(r,k,c,e,Q-e|0,f-e|0,C)}else ee(r,k,Q,c,f);else aA(A+b|0,k,c,e,Q-e|0,f-e|0,C),IQ(((r=-2&i)+t<<2)+A|0,0,t-r<<2);else Z((b=t<<2)+B|0,B,B+32|0),Z(A,r,e),ee(A+b|0,k,Q,c,f),IQ(((r=Q+f|0)+t<<2)+A|0,0,t-r<<2);if(e=(r=t<<2)+B|0,r=oA(B,A,A+r|0,t),e=E?oA(e,e,B,t)+r|0:r-rr(e,B,e,t)|0,(r=oA(r=(i<<2)+A|0,r,(t<<2)+B|0,t)+e|0)&&(i=n(i,12)+A|0,r=(A=a[i>>2])+r|0,a[i>>2]=r,!(A>>>0<=r>>>0)))for(;A=a[i+4>>2]+1|0,a[i+4>>2]=A,i=i+4|0,!A;);}}function kA(A,r,e,i,Q,f,t){var b,k=0,n=0,c=0,C=0,E=0;s=b=s-32|0,a[b+28>>2]=e,k=a[i>>2],a[b+20>>2]=a[r>>2],4096&k&&(a[b+24>>2]=A,A=b+24|0),c=192&k;A:{r:{e:{if(6&k){i:{Q:{f:switch(n=b+28|0,C=b+19|0,E=b+20|0,8&k?k=a[i+4>>2]:(c=0,k=2&k?17:16),(e=vA(n,0,0,C,0,E,e,k,c,Q,f))+1|0){case 0:break A;case 1:break Q;default:break f}if(e=a[A>>2]){if((0|Vf(e))>0)for(;a[b+12>>2]=NQ(e),ga(b+12|0,a[i+16>>2]),(0|Vf(e))>0;);e=a[A>>2]}else e=kf(),a[A>>2]=e;if(!e)break e;if((0|(e=a[b+28>>2]))>0)for(;;){if(Q=a[b+20>>2],!(B[0|Q]|1==(0|e)|B[Q+1|0])){if(A=Q+2|0,a[b+20>>2]=A,B[b+19|0])break i;e=0,Ve(13,131,159);break A}if(a[b+12>>2]=0,!T(b+12|0,b+20|0,e,a[i+16>>2],-1,0,0,f,t)){e=0,Ve(13,131,58),ef(a[b+12>>2],a[i+16>>2]);break A}if(a[b+28>>2]=a[b+28>>2]+(Q-a[b+20>>2]|0),!ir(a[A>>2],a[b+12>>2])){e=0,Ve(13,131,65),ef(a[b+12>>2],a[i+16>>2]);break A}if(!((0|(e=a[b+28>>2]))>0))break}if(!B[b+19|0])break r;e=0,Ve(13,131,137);break A}Ve(13,131,58);break A}a[b+28>>2]=(e+Q|0)-A;break r}if(n=a[i+16>>2],8&k){i:switch((e=T(A,b+20|0,e,n,a[i+4>>2],c,Q,f,t))+1|0){case 0:break A;case 1:break i;default:break r}e=0,Ve(13,131,58);break A}i:switch((e=T(A,b+20|0,e,n,-1,0,Q,f,t))+1|0){case 0:break A;case 1:break i;default:break r}Ve(13,131,58),e=0;break A}e=0,Ve(13,131,65);break A}a[r>>2]=a[b+20>>2],e=1}return s=b+32|0,e}function nA(A,r,e,Q,f,a,t,b){var k,n,c=0,C=0,E=0;if(s=n=s+-64|0,((E=-1^(C=(k=wf(c=t||37664))<<1))+(t=r-1|0)|0)<(0|Q))a=0,Ve(4,154,110);else if((1|C)>=(0|r))a=0,Ve(4,154,120);else{i[0|A]=0;A:{r:{if(he(f,a,A=(C=A+1|0)+k|0,c)){if(a=0,IQ(A+k|0,0,(f=t-Q|0)+E|0),i[A+(f+(-1^k)|0)|0]=1,yQ(((A+t|0)-Q|0)-k|0,e,Q),(0|fQ(C,k))>0)break r}else a=0;Q=0,f=0;break A}if(Q=iQ(f=t-k|0,31289,0)){if(!((0|yr(Q,f,C,k,e=b||c))<0)){if(!((0|f)<=0)){if(t=0,(r-k|0)-2>>>0>=3)for(b=-4&f,r=0;i[0|(c=A+t|0)]=B[0|c]^B[Q+t|0],i[0|(E=(c=1|t)+A|0)]=B[0|E]^B[Q+c|0],i[0|(E=(c=2|t)+A|0)]=B[0|E]^B[Q+c|0],i[0|(E=(c=3|t)+A|0)]=B[0|E]^B[Q+c|0],t=t+4|0,(0|b)!=(0|(r=r+4|0)););if(r=3&f)for(;i[0|(b=A+t|0)]=B[0|b]^B[Q+t|0],t=t+1|0,(0|r)!=(0|(a=a+1|0)););}if(a=0,!((0|yr(n,k,A,f,e))<0||(a=1,(0|k)<=0))){if(A=0,t=0,k-1>>>0>=3)for(e=-4&k,r=0;i[0|(b=t+C|0)]=B[0|b]^B[t+n|0],i[0|(c=(b=1|t)+C|0)]=B[0|c]^B[b+n|0],i[0|(c=(b=2|t)+C|0)]=B[0|c]^B[b+n|0],i[0|(c=(b=3|t)+C|0)]=B[0|c]^B[b+n|0],t=t+4|0,(0|e)!=(0|(r=r+4|0)););if(r=3&k)for(;i[0|(e=t+C|0)]=B[0|e]^B[t+n|0],t=t+1|0,(0|r)!=(0|(A=A+1|0)););}}}else Ve(4,154,65),Q=0}Of(n,64),pQ(Q,f)}return s=n- -64|0,a}function cA(A,r,e){var i,Q=0,f=0,B=0,t=0,b=0,k=0,c=0,C=0,E=0,o=0,l=0;if(!(i=a[e+28>>2]))return a[A+4>>2]=0,1;if(!Gf(r,B=i<<1))return 0;if(a[r+12>>2]=a[r+12>>2]^a[e+36>>2],E=a[e+24>>2],(0|i)<=0)a[r+4>>2]=B;else{if(k=a[r+4>>2],b=a[r>>2],C=3&(f=(0|B)>1?B:1),f-1>>>0>=3)for(o=2147483644&f,f=0;a[(t=(Q<<2)+b|0)>>2]=a[t>>2]&Q-k>>31,a[(c=((t=1|Q)<<2)+b|0)>>2]=a[c>>2]&t-k>>31,a[(c=((t=2|Q)<<2)+b|0)>>2]=a[c>>2]&t-k>>31,a[(c=((t=3|Q)<<2)+b|0)>>2]=a[c>>2]&t-k>>31,Q=Q+4|0,(0|o)!=(0|(f=f+4|0)););if(C)for(;a[(f=(Q<<2)+b|0)>>2]=a[f>>2]&Q-k>>31,Q=Q+1|0,(0|C)!=(0|(l=l+1|0)););if(a[r+4>>2]=B,k=0,(0|i)<=0)Q=0;else for(B=a[e+64>>2],f=i<<2,Q=0;e=(C=NA(b,E,i,n(B,a[b>>2]))+Q|0)+(t=a[(o=f+b|0)>>2])|0,a[o>>2]=e,Q=(0!=(0|C)|Q)&e>>>0<=t>>>0,b=b+4|0,(0|(k=k+1|0))!=(0|i););}if(!Gf(A,i))return 0;if(a[A+4>>2]=i,a[A+12>>2]=a[r+12>>2],e=rr(A=a[A>>2],r=a[r>>2]+(i<<2)|0,E,i),!((0|i)<=0)){if(b=-1^(e=Q-e|0),Q=0,1!=(0|i))for(E=-2&i,k=0;t=f=(B=Q<<2)+A|0,c=b&a[f>>2],f=r+B|0,a[t>>2]=c|e&a[f>>2],a[f>>2]=0,f=(B|=4)+A|0,B=r+B|0,a[f>>2]=b&a[f>>2]|e&a[B>>2],a[B>>2]=0,Q=Q+2|0,(0|E)!=(0|(k=k+2|0)););1&i&&(Q=(f=A)+(A=Q<<2)|0,A=A+r|0,a[Q>>2]=b&a[Q>>2]|e&a[A>>2],a[A>>2]=0)}return 1}function CA(A,r,e,i){var Q,f,B=0,t=0,b=0;s=Q=s-96|0,f=a[r+24>>2],(B=a[f+16>>2])&&(t=Dr(B)),B=Vf(a[f+48>>2]);A:if(tQ(A,e,128)&&(a[Q+80>>2]=912==a[a[r+12>>2]>>2]?19614:24175,!((0|Xe(A,31049,Q+80|0))<=0))){if(!i|!a[f+24>>2]){if(a[Q+48>>2]=t,(0|Xe(A,31208,Q+48|0))<=0)break A;B=24489,t=24457}else{if(a[Q+64>>2]=t,a[Q+68>>2]=(0|B)<=0?2:B+2|0,(0|Xe(A,31230,Q- -64|0))<=0)break A;B=24480,t=24451}if(KA(A,B,a[f+16>>2],e)&&KA(A,t,a[f+20>>2],e)){if(i){if(!KA(A,24434,a[f+24>>2],e))break A;if(!KA(A,24616,a[f+28>>2],e))break A;if(!KA(A,24597,a[f+32>>2],e))break A;if(!KA(A,24605,a[f+36>>2],e))break A;if(!KA(A,24586,a[f+40>>2],e))break A;if(!KA(A,24467,a[f+44>>2],e))break A;if(!((0|Vf(a[f+48>>2]))<=0))for(t=0;;){if(i=UQ(a[f+48>>2],t),!tQ(A,e,128))break A;if(B=t+3|0,a[Q+32>>2]=B,(0|Xe(A,24577,Q+32|0))<=0)break A;if(!KA(A,31289,a[i>>2],e))break A;if(!tQ(A,e,128))break A;if(a[Q+16>>2]=B,(0|Xe(A,24550,Q+16|0))<=0)break A;if(!KA(A,31289,a[i+4>>2],e))break A;if(!tQ(A,e,128))break A;if(a[Q>>2]=B,(0|Xe(A,24562,Q))<=0)break A;if(!KA(A,31289,a[i+8>>2],e))break A;if(t=t+1|0,!((0|Vf(a[f+48>>2]))>(0|t)))break}}(912!=a[a[r+12>>2]>>2]||gA(A,1,a[f+52>>2],e))&&(b=1)}}return s=Q+96|0,b}function EA(A,r){var e,Q=0,f=0,t=0,b=0,k=0,n=0,c=0,C=0,E=0,o=0,l=0,w=0,g=0;s=e=s-80|0;A:if(a[r+16>>2]&&r){r:{if((0|(t=p(e,80,r)))>=80){if(Q=iQ(f=t+1|0,31289,0))break r;Ve(13,126,65),t=-1;break A}if((0|t)>0){Ar(A,e,t);break A}if(w=Ar(A,24343,9),t=A,C=a[r+16>>2],s=Q=s-336|0,(0|(E=(A=(0|(k=a[r+12>>2]))/16|0)+(A<<4<(0|k))|0))>0)for(;;){for(a[Q+20>>2]=31289,a[Q+16>>2]=0,b=c<<4,a[Q+24>>2]=b,A=ji(Q+32|0,289,31100,Q+16|0),n=b+C|0,f=0;A-290>>>0<=4294967291&&((f+b|0)>=(0|k)?(i[0|(r=(Q+32|0)+A|0)]=32,i[r+1|0]=32,i[r+2|0]=32,i[r+3|0]=0):(r=B[f+n|0],a[Q+4>>2]=7==(0|f)?45:32,a[Q>>2]=r,ji((Q+32|0)+A|0,4,17440,Q)),A=A+3|0),16!=(0|(f=f+1|0)););A-290>>>0<=4294967292&&(r=(Q+32|0)+A|0,f=B[31111]|B[31112]<<8,i[0|r]=f,i[r+1|0]=f>>>8,i[r+2|0]=B[31113],A=A+2|0);e:if(!((0|b)>=(0|k)))for(g=b+C|0,f=0;;){if(288!=(-2&A)&&(n=B[f+g|0],i[(o=Q+32|0)+(r=A+1|0)|0]=0,i[A+o|0]=(n-32&255)>>>0<95?n:46,A=r),(f=f+1|0)>>>0>=16)break e;if(!((f+b|0)<(0|k)))break}if(r=Q+32|0,288!=(-2&A)&&(i[0|(f=(Q+32|0)+A|0)]=10,i[f+1|0]=0,A=A+1|0),l=(0|Ga[25](r,A,t))+l|0,(0|E)==(0|(c=c+1|0)))break}s=Q+336|0,t=l+w|0;break A}p(Q,f,r),Ar(A,Q,t),(0|Q)!=(0|e)&&SQ(Q,31289,0)}else t=Ar(A,20996,4);return s=e+80|0,t}function oA(A,r,e,i){var Q=0,f=0,B=0,t=0,b=0,k=0,n=0;if((0|i)<=0)return 0;A:{if(i>>>0>=4){for(;B=(Q=(f=a[r>>2])+Q|0)>>>0<f>>>0?B+1|0:B,t=Q,f=Q+a[e>>2]|0,Q=B,Q=f>>>0<t>>>0?Q+1|0:Q,a[A>>2]=f,B=0,B=(f=(b=a[r+4>>2])+a[e+4>>2]|0)>>>0<b>>>0?1:B,b=Q,t=Q+f|0,Q=B,B=Q=t>>>0<b>>>0?Q+1|0:Q,a[A+4>>2]=t,Q=0,Q=(f=(b=a[r+8>>2])+a[e+8>>2]|0)>>>0<b>>>0?1:Q,Q=(f=B+f|0)>>>0<B>>>0?Q+1|0:Q,a[A+8>>2]=f,B=0,B=(f=(b=a[r+12>>2])+a[e+12>>2]|0)>>>0<b>>>0?1:B,b=Q,t=Q+f|0,Q=B,a[A+12>>2]=t,B=0,Q=t>>>0<b>>>0?Q+1|0:Q,A=A+16|0,e=e+16|0,r=r+16|0,(i=i-4|0)>>>0>=4;);if(!i)break A}if(t=i-1|0,b=3&i)for(;f=(k=a[r>>2])+Q|0,Q=B,Q=f>>>0<k>>>0?Q+1|0:Q,Q=f>>>0>(B=f+a[e>>2]|0)>>>0?Q+1|0:Q,a[A>>2]=B,i=i-1|0,A=A+4|0,e=e+4|0,r=r+4|0,B=0,(0|b)!=(0|(n=n+1|0)););if(!(t>>>0<3))for(;B=(Q=(f=a[r>>2])+Q|0)>>>0<f>>>0?B+1|0:B,t=Q,f=Q+a[e>>2]|0,Q=B,a[A>>2]=f,t=f>>>0<t>>>0?Q+1|0:Q,Q=0,Q=(f=t+a[r+4>>2]|0)>>>0<t>>>0?Q+1|0:Q,Q=f>>>0>(B=f+a[e+4>>2]|0)>>>0?Q+1|0:Q,a[A+4>>2]=B,B=0,f=Q,B=(Q=Q+a[r+8>>2]|0)>>>0<f>>>0?B+1|0:B,t=Q,f=Q+a[e+8>>2]|0,Q=B,a[A+8>>2]=f,t=f>>>0<t>>>0?Q+1|0:Q,Q=0,Q=(f=t+a[r+12>>2]|0)>>>0<t>>>0?Q+1|0:Q,Q=f>>>0>(B=f+a[e+12>>2]|0)>>>0?Q+1|0:Q,a[A+12>>2]=B,A=A+16|0,e=e+16|0,r=r+16|0,B=0,i=i-4|0;);}return Q}function lA(A,r){var e,Q,f=0,t=0,b=0,k=0,n=0,c=0,C=0,E=0,o=0,l=0,w=0,g=0,u=0,M=0,D=0,I=0;Q=r,g=-1,s=e=s-16|0;A:if(r=B[0|A]){f=A;r:{for(;;){if(!pf(r<<24>>24))break r;if(r=B[f+1|0],f=f+1|0,!r)break}break A}r:switch((r=B[0|f])-43|0){case 0:case 2:break r;default:break A}u=45==(0|r)?-1:0,f=f+1|0}else f=A;A:if(48!=B[0|f])c=10;else{if(M=1,88==(223&B[f+1|0])){f=f+2|0,c=16;break A}f=f+1|0,c=8}for(;r=-48,(((t=i[0|f])-48&255)>>>0<10||(r=-87,(t-97&255)>>>0<26||(r=-55,!((t-65&255)>>>0>25))))&&!((0|(D=r+t|0))>=(0|c));)r=Zi(t=k,r=0,0,0),C=N,w=Zi(E,n=0,c,0),l=(b=N)+(o=Zi(0,0,E,n))|0,b=N,o=b=l>>>0<o>>>0?b+1|0:b,b=b+r|0,r=C,C=b,b=b>>>0<o>>>0?r+1|0:r,r=Zi(t,n,c,0)+l|0,t=N,n=r,l=(o=C)+(C=r>>>0<l>>>0?t+1|0:t)|0,r=b,a[e+8>>2]=l,a[e+12>>2]=l>>>0<C>>>0?r+1|0:r,a[e>>2]=w,a[e+4>>2]=n,r=1,a[e+8>>2]|a[e+12>>2]||(n=Zi(E,k,c,0),-1==(0|(w=N))&(-1^(t=D))>>>0<n>>>0||(r=w,E=k=t+n|0,k=r=k>>>0<t>>>0?r+1|0:r,M=1,r=I)),f=f+1|0,I=r;Q&&(a[Q>>2]=M?f:A);A:{r:{if(I)a[23332]=68,u=0,E=-1,k=0;else if(!k&-1!=(0|E))break r;if(!(1|u)){a[23332]=68,g=-2;break A}if(k){a[23332]=68;break A}}g=((A=u)^E)-A|0}return s=e+16|0,g}function wA(A,r,e,i){var Q=0,f=0,a=0,B=0,t=0,b=0,k=0,n=0,c=0,C=0,E=0,o=0;if(f=1,!GQ(A,2)&&!GQ(A,3)&&(f=0,lf(A)&&!((0|Ye(A,32936))<=0))){r||(r=3,(0|Dr(A))>3746||(r=4,(0|Dr(A))>1344||(r=5,(0|Dr(A))>475||(r=6,(0|Dr(A))>399||(r=7,(0|Dr(A))>346||(r=8,(0|Dr(A))>307||(r=(0|Dr(A))>54?27:34)))))));A:if((Q=e)||(Q=Pi())){if(ne(Q),f=-1,t=sA(Q),c=sA(Q),E=sA(Q),(a=sA(Q))&&ni(t,A)&&pr(t,1)&&ni(c,A)&&pr(c,3)){for(f=1;f=(n=f)+1|0,!gQ(t,n););f=-1;r:if(lr(E,t,n))if(b=QQ()){if(Br(b,A,Q))if((0|r)<=0)f=1;else for(;;){if(f=-1,!Rr(a,c))break r;if(!Be(a,2))break r;B=n,k=-1;e:if(P(a,a,E,A,Q,b)){i:{if(!Qf(a)&&Ye(a,t))for(;;){if(o=1,!(B=B-1|0))break i;if(k=-1,!ki(a,a,a,A,Q))break e;if(Qf(a))break i;if(!Ye(a,t))break}o=0}k=o}if(B=k){if(f=B,-1==(0|B))break r;f=0;break r}if(!Xi(i,1,C))break r;if(f=1,(0|(C=C+1|0))==(0|r))break}}else b=0;if(!Q)break A}Yr(Q),e||Gr(Q)}else f=-1;TQ(b)}return f}function gA(A,r,e,i){var Q,f=0,B=0;s=Q=s-16|0;A:{r:{e:if(tQ(A,i,128)){i:{if(r){if(!e){i=(0|Qr(A,31118))>0;break A}if((0|Qr(A,24498))>0)break i;i=0;break A}if(!e){i=(0|Qr(A,31264))>0;break A}}if(!((0|Qr(A,31288))<=0)&&tQ(A,B=r?i+2|0:i,128)&&!((0|Qr(A,31083))<=0)){if(i=a[e>>2]){if((0|EA(A,a[i>>2]))<=0)break e}else if((0|Qr(A,30522))<=0)break e;if(!((0|Qr(A,31288))<=0)&&tQ(A,B,128)&&!((0|Qr(A,31066))<=0)){i:{Q:{f:{if(i=a[e+4>>2]){if((0|EA(A,a[i>>2]))<=0)break e;if((0|Qr(A,31053))<=0)break e;if(i=a[e+4>>2],f=911==(0|gr(a[i>>2]))?zi(32744,a[i+4>>2]):0){if((0|EA(A,a[f>>2]))>0)break Q;i=0;break r}if(i=0,(0|Qr(A,22183))>0)break f;f=0;break r}if(i=0,(0|Qr(A,30512))<=0)break i}f=0}if(Qr(A,31288),i=0,!tQ(A,B,128))break r;if(a[Q>>2]=r?10571:31289,(0|Xe(A,1810,Q))<=0)break r;Q:{if(r=a[e+8>>2]){if((0|Or(A,r))>0)break Q;break r}if((0|Qr(A,30499))<=0)break r}if(Qr(A,31288),!tQ(A,B,128))break r;if((0|Qr(A,1829))<=0)break r;Q:{if(r=a[e+12>>2]){if((0|Or(A,r))>0)break Q;break r}if((0|Qr(A,30486))<=0)break r}Qr(A,31288),i=1;break r}break r}}}i=0}la(f)}return s=Q+16|0,i}function uA(A,r){var e=0,i=0,Q=0,f=0,B=0,t=0,k=0;a[A+92>>2]=0,t=A+92|0,e=a[A+12>>2];A:{if(!(b[A+28>>2]>(a[A+36>>2]<<8>>>0)/(e>>>0)>>>0)){if(i=a[A+16>>2],(Q=(f=a[A+20>>2])+1|0)>>>0>=(k=a[A+24>>2])>>>0){if(e=0,!(B=K(a[A>>2],i<<3,31289,0)))break A;a[A>>2]=B,Q=0,IQ((e=i<<2)+B|0,0,e),a[A+16>>2]=i<<1,a[A+24>>2]=i,a[A+44>>2]=a[A+44>>2]+1,e=a[A+12>>2]}if(a[A+20>>2]=Q,a[A+12>>2]=e+1,a[A+40>>2]=a[A+40>>2]+1,e=a[A>>2],a[(B=e+(f+k<<2)|0)>>2]=0,e=a[(Q=e+(f<<2)|0)>>2])for(;(0|f)!=(b[e+8>>2]%(i>>>0)|0)?(a[Q>>2]=a[e+4>>2],a[e+4>>2]=a[B>>2],a[B>>2]=e):Q=e+4|0,e=a[Q>>2];);}Q=0|Ga[a[A+8>>2]](r),a[A+56>>2]=a[A+56>>2]+1,i=a[A>>2],(e=(Q>>>0)%b[A+24>>2]|0)>>>0<b[A+20>>2]&&(e=(Q>>>0)%b[A+16>>2]|0);r:{e:if(e=a[(f=(e<<2)+i|0)>>2]){for(i=a[A+4>>2];;){if(a[A+88>>2]=a[A+88>>2]+1,(0|Q)!=a[e+8>>2]||(a[A+60>>2]=a[A+60>>2]+1,0|Ga[0|i](a[e>>2],r))){if(f=e+4|0,e=a[e+4>>2])continue;break e}break}if(i=a[f>>2])break r}if(e=0,!(i=iQ(12,31289,0)))break A;t=A+36|0,a[i+8>>2]=Q,a[i+4>>2]=0,a[i>>2]=r,a[f>>2]=i,a[A+64>>2]=a[A+64>>2]+1;break A}e=a[i>>2],a[i>>2]=r,t=A+68|0}return a[t>>2]=a[t>>2]+1,e}function MA(A,r,e,i,Q){var f=0,t=0,b=0,k=0,n=0,c=0,C=0,E=0;A:if(Q){if(f=Q-1|0,c=a[A>>2],31!=(0|(t=31&(n=B[0|c])))){if(k=c,!f)break A}else{if(!f)break A;for(t=c;;){if(C=k<<7,k=t+1|0,!((0|(b=(t=B[t+1|0])<<24>>24))>=0)){if(!(f=f-1|0))break A;if(t=k,(0|(k=C|127&b))<=16777215)continue;break A}break}if(!(f=f-1|0))break A;t|=C}if(a[e>>2]=t,a[i>>2]=192&n,!((0|f)<=0)){if(E=32&n,128==(0|(e=B[0|(b=k+1|0)]))){if(f=0,a[r>>2]=0,!E)break A;t=k+2|0,r=1}else{t=k+2|0,i=127&e;r:if(128&e){if((e=i+1|0)>>>0>=f>>>0)break A;if(i){e=e+b|0,b=0,f=i;e:{for(;;){if(B[0|t])break e;if(b=b+1|0,t=t+1|0,!(f=f-1|0))break}f=0,t=e;break r}if(f>>>0>4)break A;if(f){if(n=3&f,e=0,i+(-1^b)>>>0<3)f=0;else{for(C=-4&f,f=0;b=t,t=t+4|0,(0|C)!=(0|(f=f+4|0)););f=(f=B[0|b]|B[b+1|0]<<8|B[b+2|0]<<16|B[b+3|0]<<24)<<24|f<<8&16711680|f>>>8&65280|f>>>24}if(n)for(;f=B[0|t]|f<<8,t=t+1|0,(0|n)!=(0|(e=e+1|0)););if((0|f)<0)break A;t=(i+2|0)+k|0}else f=0}else f=0}else f=i;a[r>>2]=f,r=0}return((Q+c|0)-t|0)<(0|f)&&(Ve(13,114,155),E|=128),a[A>>2]=t,r|E}}return Ve(13,114,123),128}function sA(A){var r=0,e=0,i=0;A:{if(!(a[A+36>>2]|a[A+40>>2])){r:{e:{if((0|(i=a[A+12>>2]))==a[A+16>>2]){if(r=a[A+44>>2],!(e=iQ(328,31289,0)))break e;rf(e),8&r?(jf(e,8),rf(r=e+20|0),jf(r,8),rf(r=e+40|0),jf(r,8),rf(r=e+60|0),jf(r,8),rf(r=e+80|0),jf(r,8),rf(r=e+100|0),jf(r,8),rf(r=e+120|0),jf(r,8),rf(r=e+140|0),jf(r,8),rf(r=e+160|0),jf(r,8),rf(r=e+180|0),jf(r,8),rf(r=e+200|0),jf(r,8),rf(r=e+220|0),jf(r,8),rf(r=e+240|0),jf(r,8),rf(r=e+260|0),jf(r,8),rf(r=e+280|0),jf(r,8),rf(r=e+300|0),jf(r,8)):(rf(e+20|0),rf(e+40|0),rf(e+60|0),rf(e+80|0),rf(e+100|0),rf(e+120|0),rf(e+140|0),rf(e+160|0),rf(e+180|0),rf(e+200|0),rf(e+220|0),rf(e+240|0),rf(e+260|0),rf(e+280|0),rf(e+300|0)),r=a[A+8>>2],a[e+324>>2]=0,a[e+320>>2]=r,i=r+324|0,r=a[A>>2],a[(r?i:A+8|0)>>2]=e,a[((0!=(0|r))<<3)+A>>2]=e,a[A+4>>2]=e,a[A+16>>2]=a[A+16>>2]+16,a[A+12>>2]=a[A+12>>2]+1;break A}i:{if(i){if(e=a[A+4>>2],r=15&i)break i;e=a[e+324>>2]}else e=a[A>>2];a[A+4>>2]=e,r=0}if(a[A+12>>2]=i+1,!e)break r;e=n(r,20)+e|0;break A}Ve(3,147,65)}a[A+40>>2]=1,Ve(3,116,109)}return 0}return xf(e),a[e+16>>2]=-5&a[e+16>>2],a[A+32>>2]=a[A+32>>2]+1,e}function DA(A){1&i[A+192|0]&&(SQ(a[A+128>>2],31289,0),a[A+128>>2]=0),a[A+192>>2]=0,1&i[A+196|0]&&(SQ(a[A+132>>2],31289,0),a[A+132>>2]=0),a[A+196>>2]=0,1&i[A+200|0]&&(SQ(a[A+136>>2],31289,0),a[A+136>>2]=0),a[A+200>>2]=0,1&i[A+204|0]&&(SQ(a[A+140>>2],31289,0),a[A+140>>2]=0),a[A+204>>2]=0,1&i[A+208|0]&&(SQ(a[A+144>>2],31289,0),a[A+144>>2]=0),a[A+208>>2]=0,1&i[A+212|0]&&(SQ(a[A+148>>2],31289,0),a[A+148>>2]=0),a[A+212>>2]=0,1&i[A+216|0]&&(SQ(a[A+152>>2],31289,0),a[A+152>>2]=0),a[A+216>>2]=0,1&i[A+220|0]&&(SQ(a[A+156>>2],31289,0),a[A+156>>2]=0),a[A+220>>2]=0,1&i[A+224|0]&&(SQ(a[A+160>>2],31289,0),a[A+160>>2]=0),a[A+224>>2]=0,1&i[A+228|0]&&(SQ(a[A+164>>2],31289,0),a[A+164>>2]=0),a[A+228>>2]=0,1&i[A+232|0]&&(SQ(a[A+168>>2],31289,0),a[A+168>>2]=0),a[A+232>>2]=0,1&i[A+236|0]&&(SQ(a[A+172>>2],31289,0),a[A+172>>2]=0),a[A+236>>2]=0,1&i[A+240|0]&&(SQ(a[A+176>>2],31289,0),a[A+176>>2]=0),a[A+240>>2]=0,1&i[A+244|0]&&(SQ(a[A+180>>2],31289,0),a[A+180>>2]=0),a[A+244>>2]=0,1&i[A+248|0]&&(SQ(a[A+184>>2],31289,0),a[A+184>>2]=0),a[A+248>>2]=0,1&i[A+252|0]&&(SQ(a[A+188>>2],31289,0),a[A+188>>2]=0),a[A+252>>2]=0,SQ(A,31289,0)}function IA(A,r,e,Q,f,a,t,b,k,n){var c,C,E,o,l,w,g=0,u=0,M=0,D=0;for(s=c=s-32|0,u=1,64&n||((0|a)<0?(u=f,f=0-f|0,a=0-((0!=(0|u))+a|0)|0,D=45,u=0):2&n?(D=43,u=0):(D=(u=4&n)<<3,u=!u)),E=u,C=(M=(0|k)>0)?k:0,o=16==(0|t)?1844:8==(0|t)?30105:31289,l=32&n?21439:13082,w=8&n;k=g,u=HA(f,a,t),M=N,i[g+c|0]=B[l+(f-Zi(u,M,t,0)|0)|0],g=g+1|0,!(!a&f>>>0<t>>>0)&&(f=u,a=M,k>>>0<25););i[c+(f=26==(0|g)?k:g)|0]=0;A:{r:{if(a=(0|(a=b-!E-(ci(k=w?o:31289)+((0|f)<(0|C)?C:f))|0))>0?a:0,t=(u=16&n)?0:a,(0|(t=1&n?0-t|0:t))>0){for(;;){if(!zr(A,r,e,Q,32))break r;if(b=(0|t)>1,t=t-1|0,!b)break}t=0}if(E||zr(A,r,e,Q,D)){if(g=B[0|k])for(;;){if(!zr(A,r,e,Q,g<<24>>24))break r;if(g=B[k+1|0],k=k+1|0,!g)break}if(b=(0|(b=C-f|0))>0?b:0,g=u?a>>>0<b>>>0?b:a:b)for(;;){if(!zr(A,r,e,Q,48))break r;if(a=(0|g)>1,g=g-1|0,!a)break}for(;;){if(!((0|f)<=0)){if(zr(A,r,e,Q,i[c+(f=f-1|0)|0]))continue;break r}if(g=1,(0|t)>=0)break A;break}for(;;){if(!zr(A,r,e,Q,32))break r;if(!(t=t+1|0))break}break A}}g=0}return s=c+32|0,g}function NA(A,r,e,i){var Q=0,f=0,B=0,t=0,b=0;if((0|e)<=0)return 0;A:{if(e>>>0>=4){for(;Q=(f=(f=B)+(B=a[A>>2])|0)>>>0<B>>>0?Q+1|0:Q,B=Zi(a[r>>2],0,i,0),Q=N+Q|0,Q=(f=B+f|0)>>>0<B>>>0?Q+1|0:Q,a[A>>2]=f,B=(f=a[A+4>>2])+(t=Zi(a[r+4>>2],0,i,0))|0,f=N,f=B>>>0<t>>>0?f+1|0:f,t=Q,b=Q+B|0,Q=f,f=Q=b>>>0<t>>>0?Q+1|0:Q,a[A+4>>2]=b,B=(Q=a[A+8>>2])+(t=Zi(a[r+8>>2],0,i,0))|0,Q=N,Q=B>>>0<t>>>0?Q+1|0:Q,Q=(B=f+B|0)>>>0<f>>>0?Q+1|0:Q,a[A+8>>2]=B,B=(f=a[A+12>>2])+(t=Zi(a[r+12>>2],0,i,0))|0,f=N,f=B>>>0<t>>>0?f+1|0:f,t=Q,b=Q+B|0,Q=f,a[A+12>>2]=b,B=b>>>0<t>>>0?Q+1|0:Q,Q=0,A=A+16|0,r=r+16|0,(e=e-4|0)>>>0>=4;);if(!e)break A}if(b=i,1&e?(Q=(i=(f=a[A>>2])+B|0)>>>0<f>>>0?Q+1|0:Q,f=Zi(a[r>>2],0,b,0),Q=N+Q|0,Q=(i=f+i|0)>>>0<f>>>0?Q+1|0:Q,a[A>>2]=i,B=Q,A=A+4|0,r=r+4|0,i=e-1|0):i=e,1!=(0|e))for(;f=0,f=(e=B+a[A>>2]|0)>>>0<B>>>0?f+1|0:f,e=(B=Zi(a[r>>2],0,b,0))+e|0,Q=N+f|0,a[A>>2]=e,e=e>>>0<B>>>0?Q+1|0:Q,Q=0,Q=(e=(f=e)+a[A+4>>2]|0)>>>0<f>>>0?Q+1|0:Q,B=Zi(a[r+4>>2],0,b,0),f=N+Q|0,f=(e=B+e|0)>>>0<B>>>0?f+1|0:f,a[A+4>>2]=e,B=f,A=A+8|0,r=r+8|0,i=i-2|0;);}return B}function GA(A,r){var e=0,i=0,Q=0,f=0,B=0,t=0;a[A+92>>2]=0,Q=0|Ga[a[A+8>>2]](r),a[A+56>>2]=a[A+56>>2]+1,i=a[A>>2],(e=(Q>>>0)%b[A+24>>2]|0)>>>0<b[A+20>>2]&&(e=(Q>>>0)%b[A+16>>2]|0);A:{r:if(e=a[(i=(e<<2)+i|0)>>2]){for(f=a[A+4>>2];;){if(a[A+88>>2]=a[A+88>>2]+1,(0|Q)!=a[e+8>>2]||(a[A+60>>2]=a[A+60>>2]+1,0|Ga[0|f](a[e>>2],r))){if(i=e+4|0,e=a[e+4>>2])continue;break r}break}if(r=a[i>>2])break A}return a[A+76>>2]=a[A+76>>2]+1,0}if(a[i>>2]=a[r+4>>2],Q=a[r>>2],SQ(r,31289,0),a[A+72>>2]=a[A+72>>2]+1,r=a[A+36>>2]-1|0,a[A+36>>2]=r,!((e=a[A+12>>2])>>>0<17|b[A+32>>2]<(r<<8>>>0)/(e>>>0)>>>0)){if(i=(r=a[A+20>>2])-1|0,B=a[A+24>>2],t=a[A>>2]+(i+B<<2)|0,f=a[t>>2],a[t>>2]=0,r)a[A+20>>2]=i,r=a[A>>2];else{if(!(r=K(a[A>>2],B<<2,31289,0)))return a[A+92>>2]=a[A+92>>2]+1,Q;a[A>>2]=r,a[A+52>>2]=a[A+52>>2]+1,a[A+16>>2]=a[A+16>>2]>>>1,e=a[A+24>>2]>>>1|0,a[A+24>>2]=e,i=e-1|0,a[A+20>>2]=i,e=a[A+12>>2]}if(a[A+12>>2]=e-1,a[A+48>>2]=a[A+48>>2]+1,!(e=a[(A=(i<<2)+r|0)>>2]))return a[A>>2]=f,Q;for(;e=a[(A=e)+4>>2];);a[A+4>>2]=f}return Q}function UA(A,r,e,i){var Q,f=0,B=0,t=0,b=0,k=0,n=0;if(a[((e<<3)+A|0)-4>>2]=0,a[A>>2]=0,Q=e<<1,!((0|e)<2)&&(f=r+4|0,b=e-1|0,a[(e<<2)+A>>2]=XA(A+4|0,f,b,a[r>>2]),2!=(0|e)))for(k=e-2|0,B=A+12|0;t=f+4|0,a[((b=b-1|0)<<2)+B>>2]=NA(B,t,b,a[f>>2]),B=B+8|0,n=k>>>0>1,f=t,k=k-1|0,n;);oA(A,A,A,Q),f=i,t=0;A:if(!((0|e)<=0)){if(e>>>0>=4){for(;B=a[r>>2],a[f>>2]=Zi(B,0,B,0),a[f+4>>2]=N,B=a[r+4>>2],a[f+8>>2]=Zi(B,0,B,0),a[f+12>>2]=N,B=a[r+8>>2],a[f+16>>2]=Zi(B,0,B,0),a[f+20>>2]=N,B=a[r+12>>2],a[f+24>>2]=Zi(B,0,B,0),a[f+28>>2]=N,f=f+32|0,r=r+16|0,(e=e-4|0)>>>0>=4;);if(!e)break A}if(B=e-1|0,b=3&e)for(;k=a[r>>2],a[f>>2]=Zi(k,0,k,0),a[f+4>>2]=N,e=e-1|0,f=f+8|0,r=r+4|0,(0|b)!=(0|(t=t+1|0)););if(!(B>>>0<3))for(;t=a[r>>2],a[f>>2]=Zi(t,0,t,0),a[f+4>>2]=N,t=a[r+4>>2],a[f+8>>2]=Zi(t,0,t,0),a[f+12>>2]=N,t=a[r+8>>2],a[f+16>>2]=Zi(t,0,t,0),a[f+20>>2]=N,t=a[r+12>>2],a[f+24>>2]=Zi(t,0,t,0),a[f+28>>2]=N,f=f+32|0,r=r+16|0,e=e-4|0;);}oA(A,A,i,Q)}function FA(A,r,e,i,Q){var f=0,B=0,t=0,b=0,k=0,n=0;ne(Q);A:if(f=sA(Q)){r:if(e){if((0|r)==(0|e)){if(Jf(f,e,Q))break r;break A}if(!df(f,r,e,Q))break A}else f=r;ne(Q),r=sA(Q),A||(A=sA(Q)),n=sA(Q);r:{e:{i:if(e=sA(Q)){if((0|Fi(f,i))<0){xf(r),A=ni(A,f),Yr(Q),b=0!=(0|A);break r}if(t=B=(0|(B=Dr(f)))<(0|(t=a[i+40>>2]<<1))?t:B,(0|B)!=a[i+44>>2]&&(ne(Q),t=-1,(k=sA(Q))&&ai(k,B)&&(t=H(i+20|0,0,k,i,Q)?B:-1),Yr(Q),a[i+44>>2]=t),-1!=(0|t)&&lr(n,f,a[i+40>>2])&&df(e,n,i+20|0,Q)&&lr(r,e,B-a[i+40>>2]|0)&&(a[r+12>>2]=0,df(e,i,r,Q)&&Er(A,f,e))){if(a[A+12>>2]=0,!((0|Fi(A,i))<0)){if(!Er(A,A,i))break i;if(!Be(r,1))break i;if(!((0|Fi(A,i))<0)){if(!Er(A,A,i))break i;if(!Be(r,1))break i;if(!((0|Fi(A,i))<0)){if(!Er(A,A,i))break i;if(!Be(r,1))break i;if((0|Fi(A,i))>=0)break e}}}e=ka(A)?0:a[f+12>>2],a[A+12>>2]=e,a[r+12>>2]=a[i+12>>2]^a[f+12>>2],b=1}}Yr(Q);break r}Ve(3,130,101),Yr(Q),b=0}}return Yr(Q),b}function ZA(A,r,e,Q){var f,t=0,b=0,k=0;s=f=s-304|0,b=a[A>>2];A:{if(Q){if((0|Q)!=(0|b)&&(b=0,!r|(0|e)<0))break A;a[A>>2]=Q}else if(!(Q=b)){b=0;break A}if(b=0,!(2&a[Q+12>>2])){r:{if(r){if((0|(t=a[Q+36>>2]))>144)break A;if((0|e)>(0|t)){if(!Vr(a[A+4>>2],Q))break A;if(!nf(a[A+4>>2],r,e))break A;if(!Ri(a[A+4>>2],f,f+156|0))break A;e=a[f+156>>2]}else{if((0|e)<0)break A;a[yQ(f,r,e)+156>>2]=e}for(144!=(0|e)&&IQ(e+f|0,0,144-e|0),e=0;i[(b=f+160|0)+e|0]=54^B[e+f|0],i[(t=1|e)+b|0]=54^B[f+t|0],i[(t=2|e)+b|0]=54^B[f+t|0],i[b+(t=3|e)|0]=54^B[f+t|0],144!=(0|(e=e+4|0)););if(!Vr(a[A+8>>2],Q)){b=0;break r}if(b=0,!nf(a[A+8>>2],f+160|0,a[Q+36>>2]))break r;for(e=0;i[(t=f+160|0)+e|0]=92^B[e+f|0],i[(k=1|e)+t|0]=92^B[f+k|0],i[(k=2|e)+t|0]=92^B[f+k|0],i[t+(k=3|e)|0]=92^B[f+k|0],144!=(0|(e=e+4|0)););if(!Vr(a[A+12>>2],Q))break r;if(!nf(a[A+12>>2],f+160|0,a[Q+36>>2]))break r}if(b=0!=(0|ur(a[A+4>>2],a[A+8>>2])),!r)break A}Of(f,144),Of(f+160|0,144)}}return s=f+304|0,b}function dA(A,r,e){var Q=0,f=0,t=0,b=0;A:if(A&&(Q=a[r+16>>2],t=i[0|r],a[A>>2]||!t)){Q&&(f=a[Q+16>>2])||(f=0);r:{e:{i:{Q:{f:switch(0|t){case 1:case 6:break r;case 4:break e;case 2:break i;case 5:break Q;case 0:break f;default:break A}if(f=a[r+8>>2])return void ii(A,f)}return void wr(A,r,e)}if(f&&2==(0|Ga[0|f](2,A,r,0)))break A;if((0|(Q=Wf(A,r)))<0|(0|Q)>=a[r+12>>2]||ii(Lf(A,Q=a[r+8>>2]+n(Q,20)|0),Q),f&&Ga[0|f](3,A,r,0),e)break A;return SQ(a[A>>2],31289,0),void(a[A>>2]=0)}if(!Q)break A;if(!(e=a[Q+8>>2]))break A;return void Ga[0|e](A,r)}if(!(te(A,-1,r)||f&&2==(0|Ga[0|f](2,A,r,0)))){if(A&&(t=a[A>>2])&&(!(Q=a[r+16>>2])|!(2&B[Q+4|0])||(Q=t+a[Q+20>>2]|0,SQ(a[Q>>2],31289,0),a[Q+8>>2]=1,a[Q>>2]=0,a[Q+4>>2]=0)),(0|(Q=a[r+12>>2]))>0)for(Q=a[r+8>>2]+n(Q,20)|0,t=0;(b=Lr(A,Q=Q-20|0,0))&&ii(Lf(A,b),b),(0|(t=t+1|0))<a[r+12>>2];);f&&Ga[0|f](3,A,r,0),e||(SQ(a[A>>2],31289,0),a[A>>2]=0)}}}function WA(A){var r,e=0,i=0,Q=0,f=0,t=0;if(s=r=s-16|0,e=0,A){A:{r:if(A)if(2==(-257&(t=a[A+4>>2]))){if((i=a[A>>2])>>>0>=9)Ve(13,225,223);else if(Q=a[A+8>>2]){t&=256;e:{i:{if(i){if(A=0,e=B[0|Q],1!=(0|i)&&(A=e>>>24|0,e=B[Q+1|0]|e<<8,2!=(0|i)&&(A=A<<8|e>>>24,e=B[Q+2|0]|e<<8,3!=(0|i)&&(A=A<<8|e>>>24,e=B[Q+3|0]|e<<8,4!=(0|i)&&(A=A<<8|e>>>24,e=B[Q+4|0]|e<<8,5!=(0|i)&&(A=A<<8|e>>>24,e=B[Q+5|0]|e<<8,6!=(0|i)&&(A=A<<8|e>>>24,e=B[Q+6|0]|e<<8,7!=(0|i)&&(A=A<<8|e>>>24,e=B[Q+7|0]|e<<8))))))),f=A,t){if((0|f)>0|(0|f)>=0)break e;if(!e&-2147483648==(0|f)){a[r+8>>2]=0,a[r+12>>2]=-2147483648,A=1;break A}Ve(13,224,224);break r}if((0|f)>0|(0|f)>=0)break i;Ve(13,224,223);break r}if(t)break e}a[r+8>>2]=e,a[r+12>>2]=f,A=1;break A}a[r+8>>2]=0-e,a[r+12>>2]=0-((0!=(0|e))+f|0),A=1;break A}}else Ve(13,227,225);else Ve(13,227,67);A=0}e=-1,A&&(A=a[r+8>>2],e=-1!=(a[r+12>>2]-(A>>>0<2147483648)|0)?-1:A)}return s=r+16|0,e}function VA(A,r,e,i){var Q,f=0,B=0,t=0,k=0,c=0,C=0;if(s=Q=s-208|0,a[Q+8>>2]=1,a[Q+12>>2]=0,k=n(r,e)){for(a[Q+16>>2]=e,a[Q+20>>2]=e,c=0-e|0,f=r=e,t=2;B=r,r=(e+f|0)+r|0,a[(Q+16|0)+(t<<2)>>2]=r,t=t+1|0,f=B,r>>>0<k>>>0;);if((B=(A+k|0)+c|0)>>>0<=A>>>0)t=0,r=1,B=0;else{for(t=1,r=1;3!=(3&t)?(b[(Q+16|0)+((f=r-1|0)<<2)>>2]>=B-A>>>0?mr(A,e,i,Q+8|0,r,0,Q+16|0):ye(A,e,i,r,Q+16|0),1!=(0|r)?(EQ(Q+8|0,f),r=1):(EQ(Q+8|0,1),r=0)):(ye(A,e,i,r,Q+16|0),cQ(Q+8|0,2),r=r+2|0),t=1|(f=a[Q+8>>2]),a[Q+8>>2]=t,B>>>0>(A=A+e|0)>>>0;);t=f>>>0>1,B=0!=a[Q+12>>2]}if(mr(A,e,i,Q+8|0,r,0,Q+16|0),B||1!=(0|r)|t)for(;(0|r)<=1?(cQ(f=Q+8|0,B=qQ(f)),t=a[Q+8>>2],r=r+B|0):(EQ(f=Q+8|0,2),a[Q+8>>2]=7^a[Q+8>>2],cQ(f,1),mr((C=A+c|0)-a[(k=Q+16|0)+((B=r-2|0)<<2)>>2]|0,e,i,f,r-1|0,1,k),EQ(f,1),t=1|a[Q+8>>2],a[Q+8>>2]=t,mr(C,e,i,f,B,1,k),r=B),A=A+c|0,a[Q+12>>2]|1!=(0|t)|1!=(0|r););}s=Q+208|0}function YA(A,r,e,Q){var f,t=0,b=0,k=0,n=0;f=a[Q>>2];A:{r:{e:{i:if(!A||(n=Pr(A))){Q:{if(!(!r|!e)){b=0!=(0|(t=e-1|0));f:{a:{B:{t:if(!(!(3&(A=r))|!t))for(;;){if(!B[0|A])break B;if(b=0!=(0|(t=t-1|0)),!(3&(A=A+1|0)))break t;if(!t)break}if(!b)break a}B:{if(!(!B[0|A]|t>>>0<4))for(;;){if((-1^(b=a[A>>2]))&b-16843009&-2139062144)break B;if(A=A+4|0,!((t=t-4|0)>>>0>3))break}if(!t)break a}for(;;){if(!B[0|A])break f;if(A=A+1|0,!(t=t-1|0))break}}A=0}if(A)break Q;if(b=0,r){A=r;f:if(e){for(t=r+e|0;;){if(!B[0|A])break f;if(A=A+1|0,!(e=e-1|0))break}A=t}(A=iQ((e=A-r|0)+1|0,31289,0))&&(b=yQ(A,r,e),i[e+b|0]=0)}if(!b)break Q}if(!(k=iQ(12,31289,0))){k=0,t=n;break i}if(!f&&(A=kf(),a[Q>>2]=A,!A)){Ve(34,174,65),t=n;break e}if(a[k+8>>2]=b,a[k+4>>2]=n,a[k>>2]=0,t=n,ir(a[Q>>2],k))break A;break i}t=n,b=0}if(Ve(34,174,65),f)break r}Df(a[Q>>2]),a[Q>>2]=0}SQ(k,31289,0),SQ(t,31289,0),SQ(b,31289,0)}}function RA(A,r,e,i,Q){var f=0;f=rr(A,r,e,i);A:if(Q)if(i=A+(A=i<<2)|0,(0|Q)<0){if(r=a[(A=A+e|0)>>2],a[i>>2]=0-(r+f|0),f=r?1:f,-1==(0|Q))break A;for(;;){if(r=a[A+4>>2],a[i+4>>2]=0-(r+f|0),-2==(0|Q))break A;if(r=r?1:f,e=a[A+8>>2],a[i+8>>2]=0-(r+e|0),Q>>>0>4294967292)break A;if(r=e?1:r,e=a[A+12>>2],a[i+12>>2]=0-(r+e|0),-4==(0|Q))break A;if(r=e?1:r,e=a[A+16>>2],a[i+16>>2]=0-(r+e|0),f=e?1:r,i=i+16|0,A=A+16|0,-1==(0|(Q=Q+4|0)))break}}else{if(A=A+r|0,f)for(;;){if(r=a[A>>2],a[i>>2]=r-f,(0|Q)<2)break A;if(e=a[A+4>>2],r=r?0:f,a[i+4>>2]=e-r,2==(0|Q))break A;if(f=a[A+8>>2],r=e?0:r,a[i+8>>2]=f-r,f=f?0:r,Q>>>0<4)break A;if(r=a[A+12>>2],a[i+12>>2]=r-f,4==(0|Q))return;if(i=i+16|0,A=A+16|0,Q=Q-4|0,!(r=r?0:f))break}if(a[i>>2]=a[A>>2],1!=(0|Q))for(;;){if(a[i+4>>2]=a[A+4>>2],2==(0|Q))break A;if(a[i+8>>2]=a[A+8>>2],Q>>>0<4)break A;if(a[i+12>>2]=a[A+12>>2],4==(0|Q))break A;if(a[i+16>>2]=a[A+16>>2],i=i+16|0,A=A+16|0,!((Q=Q-4|0)>>>0>1))break}}}function yA(A,r){var e,Q=0,f=0;return e=a[r+92>>2],i[e+(Q=r+28|0)|0]=128,(f=e+1|0)>>>0>=57&&(IQ(Q+f|0,0,63-e|0),U(r,Q,1),f=0),IQ(Q+f|0,0,56-f|0),f=(f=a[r+24>>2])<<24|f<<8&16711680|f>>>8&65280|f>>>24,i[r+84|0]=f,i[r+85|0]=f>>>8,i[r+86|0]=f>>>16,i[r+87|0]=f>>>24,f=(f=a[r+20>>2])<<24|f<<8&16711680|f>>>8&65280|f>>>24,i[r+88|0]=f,i[r+89|0]=f>>>8,i[r+90|0]=f>>>16,i[r+91|0]=f>>>24,U(r,Q,1),a[r+92>>2]=0,Of(Q,64),Q=(Q=a[r>>2])<<24|Q<<8&16711680|Q>>>8&65280|Q>>>24,i[0|A]=Q,i[A+1|0]=Q>>>8,i[A+2|0]=Q>>>16,i[A+3|0]=Q>>>24,Q=(Q=a[r+4>>2])<<24|Q<<8&16711680|Q>>>8&65280|Q>>>24,i[A+4|0]=Q,i[A+5|0]=Q>>>8,i[A+6|0]=Q>>>16,i[A+7|0]=Q>>>24,Q=(Q=a[r+8>>2])<<24|Q<<8&16711680|Q>>>8&65280|Q>>>24,i[A+8|0]=Q,i[A+9|0]=Q>>>8,i[A+10|0]=Q>>>16,i[A+11|0]=Q>>>24,Q=(Q=a[r+12>>2])<<24|Q<<8&16711680|Q>>>8&65280|Q>>>24,i[A+12|0]=Q,i[A+13|0]=Q>>>8,i[A+14|0]=Q>>>16,i[A+15|0]=Q>>>24,r=(r=a[r+16>>2])<<24|r<<8&16711680|r>>>8&65280|r>>>24,i[A+16|0]=r,i[A+17|0]=r>>>8,i[A+18|0]=r>>>16,i[A+19|0]=r>>>24,1}function hA(A,r,e){var Q=0,f=0,t=0,b=0,k=0,n=0;if(!e){if(!(Q=Ki(20,31289)))return Ve(3,113,65),0;a[Q+16>>2]=1,e=Q}A:{r:{if((0|r)>0)for(;;){if(B[0|A])break A;if(A=A+1|0,f=(0|r)>1,r=r-1|0,!f)break r}if(r)break A}return a[e+4>>2]=0,e}f=(t=(b=r-1|0)>>>2|0)+1|0;A:{if(!((0|t)<a[e+8>>2]||ke(e,f))){if(e=0,!Q)break A;if(2&(r=a[Q+16>>2])||(A=a[Q>>2],8&r?x(A,a[Q+8>>2]<<2):SQ(A,31289,0)),!(1&i[Q+16|0]))break A;return SQ(Q,31289,0),0}if(t=0,a[e+12>>2]=0,a[e+4>>2]=f,k=1&r,Q=3&b,1!=(0|r))for(n=-2&r,b=0;r=B[0|A]|t<<8,Q?Q=Q-1|0:(f=f-1|0,a[a[e>>2]+(f<<2)>>2]=r,r=0,Q=3),t=B[A+1|0]|r<<8,Q?Q=Q-1|0:(f=f-1|0,a[a[e>>2]+(f<<2)>>2]=t,t=0,Q=3),A=A+2|0,(0|n)!=(0|(b=b+2|0)););if(!k|Q||(a[(a[e>>2]+(f<<2)|0)-4>>2]=B[0|A]|t<<8),(0|(A=a[e+4>>2]))>0){r=a[e>>2]+(A<<2)|0;r:{for(;;){if(a[(r=r-4|0)>>2])break r;if(Q=(0|A)>1,A=A-1|0,!Q)break}A=0}a[e+4>>2]=A}A||(a[e+12>>2]=0)}return e}function JA(){var A,r,e,i=0,Q=0,f=0,B=0,t=0,b=0,k=0,n=0;if(!(A=Ki(96,31289)))return Ve(4,106,65),0;if(a[A+60>>2]=1,i=uf(),a[A+92>>2]=i,!i)return Ve(4,106,65),SQ(A,31289,0),0;i=a[22760],a[A+8>>2]=i,a[A+64>>2]=-1025&a[i+36>>2],r=A+56|0,s=e=s-48|0;A:if(k=If(92036,60),n=0,a[23010]?k:n){if(a[23008]){if(a[r>>2]=0,f=0,!(t=(0|(B=Vf(a[23001])))<=0)&&(i=e,!(B>>>0>=10)||(f=0,i=iQ(B<<2,31289,0)))){for(;a[(Q<<2)+i>>2]=UQ(a[23001],Q),(0|B)!=(0|(Q=Q+1|0)););f=i}if(f||t){if(Q=0,(0|B)>0)for(;!(i=a[(t=(Q<<2)+f|0)>>2])|!a[i+8>>2]||(i=0,(b=a[r>>2])&&((0|Vf(b))<=(0|Q)||(i=UQ(a[r>>2],Q))),b=i,i=a[t>>2],Ga[a[i+8>>2]](A,b,r,Q,a[i>>2],a[i+4>>2])),(0|B)!=(0|(Q=Q+1|0)););if(Q=1,(0|f)==(0|e))break A;SQ(f,31289,0)}else Q=0,Ve(15,112,65)}}else Ve(15,113,65);if(s=e+48|0,Q){if(!(i=a[a[A+8>>2]+28>>2]))return A;if(0|Ga[0|i](A))return A;Ve(4,106,70)}return be(A),0}function TA(A){var r=0,e=0;if(er(8)){if(s=r=s-16|0,A&&!(!If(92256,110)|!a[23065])){a[r>>2]=1,a[r+8>>2]=A;A:if(A=Kr(a[23068],r)){if(a[A+4>>2]){if(a[r+8>>2]=a[A+12>>2],!(A=Kr(a[23068],r)))break A;if(a[A+4>>2]){if(a[r+8>>2]=a[A+12>>2],!(A=Kr(a[23068],r)))break A;if(a[A+4>>2]){if(a[r+8>>2]=a[A+12>>2],!(A=Kr(a[23068],r)))break A;if(a[A+4>>2]){if(a[r+8>>2]=a[A+12>>2],!(A=Kr(a[23068],r)))break A;if(a[A+4>>2]){if(a[r+8>>2]=a[A+12>>2],!(A=Kr(a[23068],r)))break A;if(a[A+4>>2]){if(a[r+8>>2]=a[A+12>>2],!(A=Kr(a[23068],r)))break A;if(a[A+4>>2]){if(a[r+8>>2]=a[A+12>>2],!(A=Kr(a[23068],r)))break A;if(a[A+4>>2]){if(a[r+8>>2]=a[A+12>>2],!(A=Kr(a[23068],r)))break A;if(a[A+4>>2]){if(a[r+8>>2]=a[A+12>>2],!(A=Kr(a[23068],r)))break A;if(a[A+4>>2]&&(a[r+8>>2]=a[A+12>>2],!(A=Kr(a[23068],r))|a[A+4>>2]))break A}}}}}}}}}e=a[A+12>>2]}}s=r+16|0}return e}function mA(A,r){var e=0,Q=0,f=0,t=0,b=0;A:if((e=a[A>>2])>>>0>=r>>>0){if(!(Q=a[A+4>>2]))break A;IQ(r+Q|0,0,e-r|0)}else{if(!((f=a[A+8>>2])>>>0>=r>>>0)){r:if(!(r>>>0>=1610612733)){t=(r+3>>>0)/3<<2;e:{i:{if(1&i[A+12|0]){if(e=j(t),!(Q=a[A+4>>2])|!e)break i;yQ(e,Q,a[A>>2]),x(a[A+4>>2],a[A>>2]);break e}Q:{f:{if(!(b=a[A+4>>2])){if(!(!(e=a[22755])|104==(0|e))){e=0|Ga[0|e](t,31289,0);break Q}if(!t)break f;B[92200]||(i[92200]=1),e=G(t);break Q}a:{if(!t){if(f&&Of(b,f),!(e=a[22757])|106==(0|e))break a;Ga[0|e](b,31289,0),e=0;break Q}if(t>>>0<f>>>0){Of(t+b|0,f-t|0),e=b;break Q}if(!(e=a[22755])|104==(0|e)?(B[92200]||(i[92200]=1),Q=G(t)):Q=0|Ga[0|e](t,31289,0),e=0,!Q)break Q;if(e=yQ(Q,b,f),f&&Of(b,f),!(!(f=a[22757])|106==(0|f))){Ga[0|f](b,31289,0);break Q}}m(b)}e=Q}}if(!e)break r}a[A+8>>2]=t,a[A+4>>2]=e,IQ((Q=e)+(e=a[A>>2])|0,0,r-e|0);break A}return Ve(7,105,65),0}IQ(e+a[A+4>>2]|0,0,r-e|0)}return a[A>>2]=r,r}function LA(A,r,e){var Q,f=0,t=0,b=0,k=0;s=Q=s-32|0;A:if(!(!r|(0|e)<=0)&&(f=a[r>>2])&&i[(e+f|0)-1|0]>=0)if(a[Q+24>>2]=f,a[Q+16>>2]=0,a[Q+28>>2]=0,a[Q+20>>2]=e,b=gr(Q+8|0))f=Pe(b),A&&(oi(a[A>>2]),a[A>>2]=f),a[r>>2]=a[r>>2]+e;else{for(;;){if(!(128!=B[0|f]|(i[f-1|0]<0?t:0))){f=0,Ve(13,196,216);break A}if(f=f+1|0,(0|(t=t+1|0))==(0|e))break}if(!(A&&(f=a[A>>2])&&1&i[f+20|0])){if(f=0,!(b=Ki(24,31289))){Ve(13,123,65);break A}a[b+20>>2]=1,f=b}if(t=a[f+16>>2],k=a[r>>2],a[f+16>>2]=0,!(a[f+12>>2]>=(0|e)&&t)){if(a[f+12>>2]=0,SQ(t,31289,0),!(t=iQ(e,31289,0))){Ve(13,196,65),a[A>>2]==(0|f)&&A||oi(f),f=0;break A}a[f+20>>2]=8|a[f+20>>2]}b=yQ(t,k,e),4&B[f+20|0]&&(SQ(a[f>>2],31289,0),SQ(a[f+4>>2],31289,0),a[f+20>>2]=-5&a[f+20>>2]),a[f+12>>2]=e,a[f+16>>2]=b,a[f>>2]=0,a[f+4>>2]=0,A&&(a[A>>2]=f),a[r>>2]=e+k}else f=0,Ve(13,196,216);return s=Q+32|0,f}function XA(A,r,e,i){var Q=0,f=0,B=0,t=0,b=0,k=0;if((0|e)<=0)return 0;A:{if(e>>>0>=4){for(;B=Zi(a[r>>2],0,i,0)+f|0,Q=Q+N|0,Q=f>>>0>B>>>0?Q+1|0:Q,a[A>>2]=B,f=Q,t=Zi(a[r+4>>2],0,i,0)+Q|0,Q=N,Q=f>>>0>t>>>0?Q+1|0:Q,a[A+4>>2]=t,t=Zi(a[r+8>>2],0,i,0)+Q|0,B=N,B=Q>>>0>t>>>0?B+1|0:B,a[A+8>>2]=t,Q=B,t=Zi(a[r+12>>2],0,i,0)+Q|0,f=N,f=Q>>>0>t>>>0?f+1|0:f,a[A+12>>2]=t,Q=0,A=A+16|0,r=r+16|0,(e=e-4|0)>>>0>=4;);if(!e)break A}if(k=e-1|0,t=i,b=3&e)for(i=0;B=Zi(a[r>>2],0,t,0)+f|0,Q=N,Q=f>>>0>B>>>0?Q+1|0:Q,a[A>>2]=B,f=Q,e=e-1|0,A=A+4|0,r=r+4|0,(0|b)!=(0|(i=i+1|0)););if(!(k>>>0<3))for(;i=Zi(a[r>>2],0,t,0)+f|0,Q=N,Q=i>>>0<f>>>0?Q+1|0:Q,a[A>>2]=i,B=Zi(a[r+4>>2],0,t,0)+Q|0,f=N,f=Q>>>0>B>>>0?f+1|0:f,a[A+4>>2]=B,B=Zi(a[r+8>>2],0,t,0)+f|0,Q=N,Q=f>>>0>B>>>0?Q+1|0:Q,a[A+8>>2]=B,b=Zi(a[r+12>>2],0,t,0)+Q|0,B=N,B=Q>>>0>b>>>0?B+1|0:B,a[A+12>>2]=b,f=B,A=A+16|0,r=r+16|0,e=e-4|0;);}return f}function SA(A,r,e,i,Q){var f,B=0,t=0,b=0,k=0,n=0,c=0,C=0,E=0,o=0,l=0,w=0,g=0;if(!Gf(A,r))return 0;f=1<<Q;A:if((0|Q)<=3){if((0|r)<=0)break A;for(t=-1^i,n=2147483646&(Q=(0|f)>1?f:1),C=1&Q,E=a[A>>2],o=(0|f)<2;;){if(Q=0,B=0,b=0,!o)for(;B=(t&((c=1|Q)^i)-1)>>31&a[(c<<2)+e>>2]|a[(Q<<2)+e>>2]&(t&(i^Q)-1)>>31|B,Q=Q+2|0,(0|n)!=(0|(b=b+2|0)););if(b=E+(k<<2)|0,C&&(B=a[(Q<<2)+e>>2]&(t&(i^Q)-1)>>31|B),a[b>>2]=B,e=(f<<2)+e|0,(0|(k=k+1|0))==(0|r))break}}else if(!((0|r)<=0))if(b=a[A>>2],31==(0|(Q=Q-2|0)))IQ(b,0,r<<2);else for(C=-1^(n=(t=1<<Q)-1&i),E=3<<Q,o=2<<Q,c=(0|t)>1?t:1,l=((Q=-1^(i>>=Q))&(2^i)-1)>>31,w=(Q&(1^i)-1)>>31,g=(Q&(3^i)-1)>>31;;){for(B=0,Q=0;B=(a[(Q+E<<2)+e>>2]&g|a[(Q+o<<2)+e>>2]&l|a[(Q+t<<2)+e>>2]&w|(i?0:a[(Q<<2)+e>>2]))&(C&(Q^n)-1)>>31|B,(0|c)!=(0|(Q=Q+1|0)););if(a[b+(k<<2)>>2]=B,e=(f<<2)+e|0,(0|(k=k+1|0))==(0|r))break}return a[A+4>>2]=r,1}function HA(A,r,e){var i=0,Q=0,f=0,a=0,B=0,t=0,b=0,k=0,c=0;A:{r:{e:{i:{Q:{f:{a:{B:{t:{if(r){if(!e)break t;break B}return D=(r=A)-n(A=(A>>>0)/(e>>>0)|0,e)|0,I=0,N=0,A}if(!A)break a;break f}if(!((i=e-1|0)&e))break Q;f=0-(a=(C(e)+33|0)-C(r)|0)|0;break e}return D=0,I=r-n(A=(r>>>0)/0|0,0)|0,N=0,A}if((i=32-C(r)|0)>>>0<31)break i;break r}if(D=A&i,I=0,1==(0|e))break A;return e=31&(i=e?31-C(e-1^e)|0:32),(63&i)>>>0>=32?(i=0,A=r>>>e|0):(i=r>>>e|0,A=((1<<e)-1&r)<<32-e|A>>>e),N=i,A}a=i+1|0,f=63-i|0}if(Q=31&(i=63&a),i>>>0>=32?(i=0,B=r>>>Q|0):(i=r>>>Q|0,B=((1<<Q)-1&r)<<32-Q|A>>>Q),Q=i,f=31&(i=63&f),i>>>0>=32?(i=A<<f,A=0):(i=(1<<f)-1&A>>>32-f|r<<f,A<<=f),r=i,a)for(k=-1!=(0|(f=e-1|0))?0:-1;B=(i=B<<1|r>>>31)-(b=e&(t=k-((Q=Q<<1|B>>>31)+(i>>>0>f>>>0)|0)>>31))|0,Q=Q-(i>>>0<b>>>0)|0,r=r<<1|A>>>31,A=c|A<<1,c=t&=1,a=a-1|0;);return D=B,I=Q,N=r<<1|A>>>31,t|A<<1}D=A,I=r,A=0,r=0}return N=r,A}function pA(A,r,e,Q){var f,t=0,b=0,k=0,n=0,c=0,C=0;if(Q){b=-128&(f=B[0|e]),r&&(a[r>>2]=b);A:{if(1==(0|Q)){if(!A)break A;return r=B[0|e],b?void(i[0|A]=0-r):void(i[0|A]=r)}r:{if(k=B[0|e]){if(r=0,255!=(0|k))break r;if(k=3&(r=Q-1|0),Q-2>>>0<3)r=1;else for(C=-4&r,r=1;t=t|B[0|(t=r+e|0)]|B[t+1|0]|B[t+2|0]|B[t+3|0],r=r+4|0,(0|C)!=(0|(n=n+4|0)););if(k)for(;t=B[r+e|0]|t,r=r+1|0,(0|(c=c+1|0))!=(0|k););if(!t){r=0;break r}}if(r=-1,(0|b)==(128&B[e+1|0]))return void Ve(13,226,221)}if(A){if(!(b=r+Q|0))return;if(k=f<<24>>31,r=A+b|0,t=e+Q|0,A=(128&f)>>>7|0,1&b?(r=r-1|0,A=(B[0|(t=t-1|0)]^255&k)+A|0,i[0|r]=A,A=A>>>8|0,e=b-1|0):e=b,1!=(0|b))for(;A=(255&(k^B[t-1|0]))+A|0,i[r-1|0]=A,r=r-2|0,A=(255&(k^B[0|(t=t-2|0)]))+(A>>>8|0)|0,i[0|r]=A,A=A>>>8|0,e=e-2|0;);}}}else Ve(13,226,222)}function vA(A,r,e,Q,f,t,b,k,n,c,C){var E,o,l=0,w=0,g=0;s=E=s-16|0,o=a[t>>2],a[E>>2]=o;A:{r:{e:{if(C){if(B[0|C]){l=a[C+4>>2],a[E+4>>2]=a[C+8>>2],a[E+8>>2]=a[C+16>>2],a[E+12>>2]=a[C+12>>2],a[E>>2]=a[C+20>>2]+o;break e}if(l=MA(E,E+4|0,E+12|0,E+8|0,b),a[C+4>>2]=l,g=a[E+4>>2],a[C+8>>2]=g,a[C+16>>2]=a[E+8>>2],a[C+12>>2]=a[E+12>>2],w=a[E>>2],i[0|C]=1,w=w-o|0,a[C+20>>2]=w,129&l|(w+g|0)<=(0|b))break e;Ve(13,104,155),i[0|C]=0,A=0;break A}if(!(128&(l=MA(E,E+4|0,E+12|0,E+8|0,b))))break r;Ve(13,104,102),A=0;break A}if(128&l){Ve(13,104,102),i[0|C]=0,A=0;break A}}if(!((0|k)<0)){if(!(a[E+12>>2]==(0|k)&a[E+8>>2]==(0|n))){if(A=-1,c)break A;C&&(i[0|C]=0),Ve(13,104,168),A=0;break A}C&&(i[0|C]=0)}(k=1&l)&&(a[E+4>>2]=(b+o|0)-a[E>>2]),Q&&(i[0|Q]=k),f&&(i[0|f]=32&l),A&&(a[A>>2]=a[E+4>>2]),e&&(i[0|e]=a[E+8>>2]),r&&(a[r>>2]=a[E+12>>2]),a[t>>2]=a[E>>2],A=1}return s=E+16|0,A}function zA(A,r,e){var i,Q=0,f=0,B=0,t=0,b=0,k=0,n=0;s=i=s-48|0;A:if(A>>>0>=16)Ve(15,113,7);else if(k=If(92036,60),n=0,a[23010]?k:n){if(a[23008]){r:if(!((0|(B=Vf(a[(t=91968+(A<<2)|0)>>2])))<=0)){if(Q=i,!(B>>>0>=10)||(Q=iQ(B<<2,31289,0))){for(A=0;a[(A<<2)+Q>>2]=UQ(a[t>>2],A),(0|B)!=(0|(A=A+1|0)););if(A=0,(0|B)>0)for(;!(f=a[(A<<2)+Q>>2])|!a[f+12>>2]||(b=0,(t=a[e>>2])&&((0|Vf(t))<=(0|A)||(b=UQ(a[e>>2],A))),Ga[a[f+12>>2]](r,b,e,A,a[f>>2],a[f+4>>2])),(0|B)!=(0|(A=A+1|0)););if((0|Q)!=(0|i))break r;break A}for(Q=0;!(f=UQ(a[t>>2],Q))|!a[f+12>>2]||(b=0,(A=a[e>>2])&&((0|Vf(A))<=(0|Q)||(b=UQ(a[e>>2],Q))),Ga[a[f+12>>2]](r,b,e,Q,a[f>>2],a[f+4>>2])),(0|B)!=(0|(Q=Q+1|0)););Q=0}SQ(Q,31289,0)}}else Ve(15,113,65);Df(a[e>>2]),a[e>>2]=0,s=i+48|0}function jA(A,r,e,Q,f){var B,t=0,b=0,k=0;if(f=192&f|(0!=(0|r))<<5,B=a[A>>2],(0|Q)<=30)i[0|B]=f|31&Q,t=B+1|0;else{for(i[0|B]=31|f,f=Q;t=b,b=b+1|0,k=f>>>0<128,f=f>>>7|0,!k;);if(1&t?(k=b,f=t):(i[b+B|0]=127&Q,Q>>=7,k=t,f=t-1|0),t)for(;i[k+B|0]=127&Q|((0|b)!=(0|k))<<7,i[f+B|0]=Q>>>7&127|((0|f)!=(0|b))<<7,Q>>=14,k=f-1|0,t=(0|f)>1,f=f-2|0,t;);t=(B+1|0)+b|0}if(2==(0|r))return i[0|t]=128,void(a[A>>2]=t+1);if((0|e)>127){for(b=0,f=e;r=b,b=b+1|0,Q=f>>>0>255,f=f>>>8|0,Q;);if(i[0|t]=128|b,k=3&b)for(Q=0,f=b;i[f+t|0]=e,e>>=8,f=f-1|0,(0|k)!=(0|(Q=Q+1|0)););else f=b;if(r>>>0>=3)for(;Q=e<<8&16711680|e<<24|e>>>8&65280|e>>>24,i[0|(r=(f+t|0)-3|0)]=Q,i[r+1|0]=Q>>>8,i[r+2|0]=Q>>>16,i[r+3|0]=Q>>>24,e>>=31,r=(0|f)>4,f=f-4|0,r;);a[A>>2]=1+(b+t|0)}else i[0|t]=e,a[A>>2]=t+1}function xA(A,r,e,i){var Q=0,f=0,B=0,t=0,b=0,k=0,n=0,c=0;if(!(B=a[r+4>>2])||!(t=a[e+4>>2]))return xf(A),1;ne(i);A:if((0|A)!=(0|r)&&(0|(Q=A))!=(0|e)||(Q=sA(i))){r:{e:if(8!=(0|B)|8!=(0|t))if(n=B+t|0,(0|B)<16|(0|t)<16||(f=B-t|0)+1>>>0>2){if(!Gf(Q,n))break A;a[Q+4>>2]=n,ee(a[Q>>2],a[r>>2],B,a[e>>2],t)}else{if(f=pi((0|f)<0?t:B),!(k=sA(i)))break r;if(!((0|(f=1<<(b=f-1|0)))>=(0|B)&(0|f)>=(0|t))){if(!Gf(k,b=8<<b))break r;if(!Gf(Q,b))break r;bA(a[Q>>2],a[r>>2],a[e>>2],f,B-f|0,t-f|0,a[k>>2]),a[Q+4>>2]=n;break e}if(!Gf(k,b=4<<b))break r;if(!Gf(Q,b))break r;aA(a[Q>>2],a[r>>2],a[e>>2],f,B-f|0,t-f|0,a[k>>2]),a[Q+4>>2]=n}else{if(!Gf(Q,16))break A;a[Q+4>>2]=16,Z(a[Q>>2],a[r>>2],a[e>>2])}if(a[Q+12>>2]=a[e+12>>2]^a[r+12>>2],(0|A)!=(0|Q)&&!ni(A,Q))break A;c=1}return Yr(i),c}return Yr(i),0}function KA(A,r,e,Q){var f,t=0,b=0,k=0,n=0,c=0,C=0;if(s=f=s-80|0,e)if(t=ra(e),tQ(A,Q,128))if(ka(e))a[f+64>>2]=r,t=(0|Xe(A,31182,f- -64|0))>0;else if(b=t?30163:31289,(0|Dr(e))<=32)e=a[a[e>>2]>>2],a[f+16>>2]=e,a[f+12>>2]=b,a[f+8>>2]=e,a[f+4>>2]=b,a[f>>2]=r,t=(0|Xe(A,31188,f))>0;else{t=0;A:if((k=iQ(n=1+((Dr(e)+7|0)/8|0)|0,31289,0))&&(i[0|k]=0,a[f+48>>2]=r,a[f+52>>2]=45==B[0|b]?30790:31289,!((0|Xe(A,31149,f+48|0))<=0))){t=1;r:{if(r=(r=ve(r=e,e=k+1|0))+((b=B[k+1|0])>>>7|0)|0)for(b=b<<24>>24>=0?e:k,Q=Q+4|0,c=r-1|0,e=0;;){if(!((e>>>0)%15|0)){if(e&&(0|Qr(A,31288))<=0)break r;if(!tQ(A,Q,128))break r}if(C=B[e+b|0],a[f+36>>2]=(0|e)==(0|c)?31289:24622,a[f+32>>2]=C,(0|Xe(A,6663,f+32|0))<=0)break r;if((0|r)==(0|(e=e+1|0)))break}if((0|Ar(A,31288,1))>0)break A}t=0}pQ(k,n)}else t=0;else t=1;return s=f+80|0,t}function OA(A,r,e){A|=0,e|=0;var i,Q=0,f=0,B=0,t=0,b=0,k=0,n=0;if((0|(i=a[(r|=0)+12>>2]))!=a[e+12>>2]){if(Q=a[r+4>>2]<a[e+4>>2],f=a[(B=Q?r:e)+4>>2],!Gf(A,(b=a[(e=Q?e:r)+4>>2])+1|0))return a[A+12>>2]=i,0;if(a[A+4>>2]=b,r=(Q=a[A>>2])+(t=f<<2)|0,Q=oA(Q,e=a[e>>2],a[B>>2],f),B=b-f|0){if(e=e+t|0,k=3&B)for(t=0;n=a[e>>2]+Q|0,a[r>>2]=n,r=r+4|0,e=e+4|0,B=B-1|0,Q&=!n,(0|k)!=(0|(t=t+1|0)););if(!((-1^f)+b>>>0<3))for(;f=a[e>>2]+Q|0,a[r>>2]=f,f=(Q&=!f)+a[e+4>>2]|0,a[r+4>>2]=f,f=(Q&=!f)+a[e+8>>2]|0,a[r+8>>2]=f,f=(Q&=!f)+a[e+12>>2]|0,a[r+12>>2]=f,Q&=!f,r=r+16|0,e=e+16|0,B=B-4|0;);}return a[r>>2]=Q,a[A+4>>2]=a[A+4>>2]+Q,a[A+12>>2]=i,1}return(0|(Q=Fi(r,e)))>0?(Q=a[r+12>>2],r=Er(A,r,e),a[A+12>>2]=Q,0|r):(0|Q)<0?(Q=a[e+12>>2],r=Er(A,e,r),a[A+12>>2]=!Q,0|r):(xf(A),a[A+12>>2]=0,1)}function PA(A,r,e){A|=0,e|=0;var i,Q=0,f=0,B=0,t=0,b=0,k=0,n=0;if((0|(i=a[(r|=0)+12>>2]))==a[e+12>>2]){if(Q=a[r+4>>2]<a[e+4>>2],f=a[(B=Q?r:e)+4>>2],!Gf(A,(b=a[(e=Q?e:r)+4>>2])+1|0))return a[A+12>>2]=i,0;if(a[A+4>>2]=b,r=(Q=a[A>>2])+(t=f<<2)|0,Q=oA(Q,e=a[e>>2],a[B>>2],f),B=b-f|0){if(e=e+t|0,k=3&B)for(t=0;n=a[e>>2]+Q|0,a[r>>2]=n,r=r+4|0,e=e+4|0,B=B-1|0,Q&=!n,(0|k)!=(0|(t=t+1|0)););if(!((-1^f)+b>>>0<3))for(;f=a[e>>2]+Q|0,a[r>>2]=f,f=(Q&=!f)+a[e+4>>2]|0,a[r+4>>2]=f,f=(Q&=!f)+a[e+8>>2]|0,a[r+8>>2]=f,f=(Q&=!f)+a[e+12>>2]|0,a[r+12>>2]=f,Q&=!f,r=r+16|0,e=e+16|0,B=B-4|0;);}return a[r>>2]=Q,a[A+4>>2]=a[A+4>>2]+Q,a[A+12>>2]=i,1}return(0|(Q=Fi(r,e)))>0?(Q=a[r+12>>2],r=Er(A,r,e),a[A+12>>2]=Q,0|r):(0|Q)<0?(Q=a[e+12>>2],r=Er(A,e,r),a[A+12>>2]=Q,0|r):(xf(A),a[A+12>>2]=0,1)}function qA(A,r,e,i){var Q,f,B=0,t=0,b=0,k=0;Q=(0|e)/2|0;A:switch(e-4|0){case 0:return void tA(A,r);case 4:return void d(A,r);default:break A}if((0|e)<=15)UA(A,r,e,i);else{f=(Q<<2)+r|0;A:if(Q)if((0|(b=a[(B=(Q<<2)-4|0)+r>>2]))==(0|(B=a[B+f>>2]))){if(!((0|Q)<2)){for(B=Q-2|0;;){if((0|(k=a[(b=B<<2)+r>>2]))==(0|(b=a[b+f>>2]))){if(b=(0|B)<=0,B=B-1|0,!b)continue;break A}break}t=b>>>0<k>>>0?1:-1}}else t=B>>>0<b>>>0?1:-1;A:{r:{if((0|t)>0)rr(i,r,f,Q);else{if((0|t)>=0)break r;rr(i,f,r,Q)}qA((e<<2)+i|0,i,Q,t=(e<<3)+i|0);break A}IQ((t=e<<2)+i|0,0,t),t=(e<<3)+i|0}if(qA(A,r,Q,t),qA(B=(r=e<<2)+A|0,f,Q,t),r=r+i|0,(r=(B=oA(i,A,B,e)-rr(r,i,r,e)|0)+oA(i=(Q<<2)+A|0,i,r,e)|0)&&(A=(i=r)+(r=a[(e=(e+Q<<2)+A|0)>>2])|0,a[e>>2]=A,!(A>>>0>=r>>>0)))for(;A=a[e+4>>2]+1|0,a[e+4>>2]=A,e=e+4|0,!A;);}}function _A(A,r,e){var i,Q,f,a=0,B=0,t=0,b=0;if(!e)return-1;b=A-(A>>>0<e>>>0?0:e)|0,32!=(0|(A=pi(e)))&&(b=b<<(B=32-A|0)|r>>>A,e<<=B,r<<=B),B=65535,(0|(i=e>>>16|0))!=(b>>>16|0)&&(B=(b>>>0)/(i>>>0)|0);A:if(!((t=b-(a=n(B,i))|0)>>>0>65535|(A=n(Q=65535&e,B))>>>0<=(t<<16|r>>>16)>>>0))for(;;){if(A=A-Q|0,B=B-1|0,(t=b-(a=a-i|0)|0)>>>0>65535)break A;if(!((t<<16|r>>>16)>>>0<A>>>0))break}f=B-(t=(a=(a=(A>>>16|0)+a|0)+((A<<=16)>>>0>r>>>0)|0)>>>0>b>>>0)|0,B=65535,((r=(a=(b-a|0)+(t?e:0)<<16)|(e=r-A|0)>>>16)>>>16|0)!=(0|i)&&(B=(r>>>0)/(i>>>0)|0),b=f<<16;A:if(!((t=r-(a=n(B,i))|0)>>>0>65535|(A=n(B,Q))>>>0<=(t<<16|(e<<=16)>>>16)>>>0))for(;;){if(A=A-Q|0,B=B-1|0,(t=r-(a=a-i|0)|0)>>>0>65535)break A;if(!((t<<16|e>>>16)>>>0<A>>>0))break}return b|B-(r>>>0<((A>>>16|0)+a|0)+(e>>>0<A<<16>>>0)>>>0)}function $A(A,r,e,i,Q){var f,t,b=0,k=0,n=0,c=0,C=0;s=f=s-32|0,b=a[r>>2],a[f+12>>2]=b;A:{if((t=1&i)||A){r:{if((0|e)>0)for(c=Q+1|0,C=(0|Q)<5;;){if(i=b,!(B[0|b]|1==(0|e)|B[b+1|0])){if(b=i+2|0,a[f+12>>2]=b,t)break r;Ve(13,106,159),b=0;break A}a[f+16>>2]=i;e:{i:{if(!(128&(k=MA(f+16|0,f+20|0,f+28|0,f+24|0,e)))){if(!(n=1&k))break i;Q=(e+i|0)-(b=a[f+16>>2])|0;break e}Ve(13,104,102),Ve(13,106,58),b=0;break A}b=a[f+16>>2],Q=a[f+20>>2]}if(a[f+12>>2]=b,32&k){if(!C){Ve(13,106,197),b=0;break A}if(b=0,!$A(A,f+12|0,Q,n,c))break A;b=a[f+12>>2]}else if(Q){if(A){if(!mA(A,(k=a[A>>2])+Q|0)){Ve(13,140,65),b=0;break A}yQ(k+a[A+4>>2]|0,b,Q)}b=Q+b|0,a[f+12>>2]=b}if(!((0|(e=(e+i|0)-b|0))>0))break}if(t){Ve(13,106,137),b=0;break A}}a[r>>2]=b}else a[r>>2]=e+b;b=1}return s=f+32|0,b}function Ar(A,r,e){var i,Q=0,f=0,B=0,t=0,b=0;if(s=i=s-16|0,(0|e)>=0){A:if(A)if(B=i+12|0,Q=a[A>>2],a[Q+8>>2]&&Q){Q=a[A+8>>2];r:{e:{i:{Q:{f:{if(!(f=a[A+4>>2])){if(Q)break f;break e}if(!Q)break Q}Q=0|Ga[0|Q](A,3,r,e,0,0,1,0);break i}if(Q=-1,(0|e)<0)break A;Q=0|Ga[0|f](A,3,r,e,0,1)}if((0|Q)<=0)break r}if(!a[A+16>>2]){Ve(32,128,120),Q=-2;break A}(0|(f=0|Ga[a[a[A>>2]+8>>2]](A,r,e,B)))>0&&(b=a[A+68>>2],t=(Q=a[B>>2])+a[A+64>>2]|0,a[A+64>>2]=t,a[A+68>>2]=Q>>>0>t>>>0?b+1|0:b),Q=a[A+8>>2];e:{i:{if(!(t=a[A+4>>2])){if(Q)break i;Q=f;break A}if(!Q)break e}Q=0|Ga[0|Q](A,131,r,e,0,0,f,B);break A}Q=-1,(0|e)<0||((0|f)>0&&(0|(f=a[B>>2]))<0||(0|(Q=0|Ga[0|t](A,131,r,e,0,f)))<=0||(a[B>>2]=Q,Q=1))}}else Ve(32,128,121),Q=-2;Q=(0|Q)>0?a[i+12>>2]:Q}return s=i+16|0,Q}function rr(A,r,e,i){var Q=0,f=0,B=0,t=0;if((0|i)<=0)return 0;A:{if(i>>>0>=4){for(;Q=a[r>>2],B=a[e>>2],a[A>>2]=Q-(B+f|0),t=a[r+4>>2],B=(0|Q)==(0|B)?f:Q>>>0<B>>>0,Q=a[e+4>>2],a[A+4>>2]=t-(B+Q|0),f=a[r+8>>2],t=(0|Q)==(0|t)?B:Q>>>0>t>>>0,Q=a[e+8>>2],a[A+8>>2]=f-(t+Q|0),B=a[r+12>>2],f=(0|Q)==(0|f)?t:Q>>>0>f>>>0,Q=a[e+12>>2],a[A+12>>2]=B-(f+Q|0),f=(0|Q)==(0|B)?f:Q>>>0>B>>>0,A=A+16|0,e=e+16|0,r=r+16|0,(i=i-4|0)>>>0>=4;);if(!i)break A}if(1&i?(Q=a[r>>2],B=a[e>>2],a[A>>2]=Q-(B+f|0),f=(0|Q)==(0|B)?f:Q>>>0<B>>>0,A=A+4|0,e=e+4|0,r=r+4|0,Q=i-1|0):Q=i,1!=(0|i))for(;i=a[r>>2],B=a[e>>2],a[A>>2]=i-(B+f|0),t=a[r+4>>2],f=(0|i)==(0|B)?f:i>>>0<B>>>0,i=a[e+4>>2],a[A+4>>2]=t-(f+i|0),f=(0|i)==(0|t)?f:i>>>0>t>>>0,A=A+8|0,e=e+8|0,r=r+8|0,Q=Q-2|0;);}return f}function er(A){var r=0,e=0;A:{if(B[92116]){if(262144&A)break A;return Ve(15,116,70),0}if(!(!If(92132,76)|!a[23034])&&(r=1,!(262144&A))){r:{if(524288&A){if(r=0,!If(92140,81))break A;if(a[23036])break r;break A}if(r=0,!If(92140,77)|!a[23036])break A}!If(92148,78)|!(1&i[92152])||1&A&&!If(92156,82)|!(1&i[92160])||2&A&&!If(92156,83)|!(1&i[92160])||16&A&&!If(92164,84)|!(1&i[92168])||4&A&&!If(92164,85)|!(1&i[92168])||32&A&&!If(92172,86)|!(1&i[92176])||8&A&&!If(92172,87)|!(1&i[92176])||131072&A||128&A&&!If(92180,88)|!a[23046]||64&A&&(a[23047]=0,e=If(92180,89),a[23047]=0,!e|a[23046]<=0)||256&A&&!If(92192,90)|!a[23049]||(r=1)}}return r}function ir(A,r){var e,i=0,Q=0,f=0,B=0;if(!A)return-1;e=a[A>>2];A:if(A&&!((0|(Q=a[A>>2]))>1073741822)){if(Q=(0|Q)>3?Q:3,f=a[A+4>>2]){if(!((0|Q)<(0|(i=a[A+12>>2])))){if((0|i)>1073741822)break A;r:{e:{if(Q>>>0>=1073741823)for(;;){if((0|i)>715827881)break A;if(!((0|Q)>=(0|(i=((0|i)/2|0)+i|0))))break e}for(;;){if((0|i)>715827881){i=1073741823;break r}if(!((0|Q)>=(0|(i=((0|i)/2|0)+i|0))))break}}if(!i)break A}if(!(f=K(f,i<<2,31289,0)))break A;a[A+12>>2]=i,a[A+4>>2]=f}}else{if(f=Ki((B=Q+1|0)<<2,31289),a[A+4>>2]=f,!f){Ve(15,129,65),B=0;break A}a[A+12>>2]=B}(0|e)<(0|(i=a[A>>2]))&(0|e)>=0?(eQ((B=(Q=e<<2)+f|0)+4|0,B,i-e<<2),a[Q+a[A+4>>2]>>2]=r,i=a[A>>2]):a[(i<<2)+f>>2]=r,a[A+8>>2]=0,B=i+1|0,a[A>>2]=B}return B}function Qr(A,r){var e,i=0,Q=0,f=0,B=0;s=e=s-16|0,a[e+12>>2]=0;A:if(A&&(i=a[A>>2])&&a[i+24>>2]){i=a[A+8>>2];r:{e:{i:{Q:{if(!(Q=a[A+4>>2])){if(i)break Q;break r}if(!i)break i}i=0|Ga[0|i](A,4,r,0,0,0,1,0);break e}i=0|Ga[0|Q](A,4,r,0,0,1)}if((0|i)<=0)break A}if(a[A+16>>2]){(0|(i=0|Ga[a[a[A>>2]+24>>2]](A,r)))<=0?Q=0:(Q=a[A+68>>2],Q=(f=a[A+64>>2]+i|0)>>>0<i>>>0?Q+1|0:Q,a[A+64>>2]=f,a[A+68>>2]=Q,a[e+12>>2]=i,Q=i,i=1),f=a[A+8>>2];r:{e:{i:{if(!(B=a[A+4>>2])){if(f)break i;break e}if(!f)break r}i=0|Ga[0|f](A,132,r,0,0,0,i,e+12|0)}if((0|i)<=0)break A;if((0|(i=a[e+12>>2]))>=0)break A;Ve(32,110,102),i=-1;break A}i=0|Ga[0|B](A,132,r,0,0,(0|i)>0?Q:i)}else Ve(32,110,120),i=-2}else Ve(32,110,121),i=-2;return s=e+16|0,i}function fr(){var A=0,r=0,e=0,Q=0,f=0,t=0;A:if((A=Ur())&&(0|(r=a[A+384>>2]))!=(0|(e=a[A+388>>2]))){for(Q=A+128|0;;){if(2&B[0|(t=(f=r<<2)+A|0)])1&i[t+192|0]&&(SQ(a[Q+f>>2],31289,0),r=a[A+384>>2],a[Q+(r<<2)>>2]=0),a[192+((r<<2)+A|0)>>2]=0,a[(a[A+384>>2]<<2)+A>>2]=0,r=(a[A+384>>2]<<2)+A|0,a[r+320>>2]=-1,a[r+256>>2]=0,a[r- -64>>2]=0,r=(0|(r=a[A+384>>2]))>0?r-1|0:15,a[A+384>>2]=r;else{if(!(2&B[0|(t=(f=(e=(e+1|0)%16|0)<<2)+A|0)])){Q=a[((r<<2)+A|0)- -64>>2];break A}a[A+388>>2]=e,1&i[t+192|0]&&(SQ(a[Q+f>>2],31289,0),e=a[A+388>>2],a[Q+(e<<2)>>2]=0),a[192+((e<<2)+A|0)>>2]=0,a[(a[A+388>>2]<<2)+A>>2]=0,r=(a[A+388>>2]<<2)+A|0,a[r+320>>2]=-1,a[r+256>>2]=0,a[r- -64>>2]=0,r=a[A+384>>2]}if((0|(e=a[A+388>>2]))==(0|r))break}return 0}return Q}function ar(A,r,e,Q){var f=0,t=0,b=0;if(!a[e+4>>2]||!a[e>>2])return Ve(3,100,107),0;if(-1!=a[e+20>>2]){A:if(a[e>>2]&&(f=a[e+4>>2])){r:{if(-1!=(0|(t=a[e+20>>2]))){if(t=t+1|0,a[e+20>>2]=t,!(2&B[e+24|0]|!a[e+8>>2]|32!=(0|t))){if(t=0,Y(e,0,0,Q,0,0))break r;break A}}else a[e+20>>2]=1;if(!(1&i[e+24|0])){if(b=a[e+28>>2]){if(t=0,!Gi(f,f,f,b,Q))break A;if(Gi(f=a[e>>2],f,f,a[e+28>>2],Q))break r;break A}if(t=0,!ki(f,f,f,a[e+12>>2],Q))break A;if(!ki(f=a[e>>2],f,f,a[e+12>>2],Q))break A}}t=1}else Ve(3,103,107);if(32==a[e+20>>2]&&(a[e+20>>2]=0),!t)return 0}else a[e+20>>2]=0;return r&&!ni(r,a[e+4>>2])?0:(r=a[e>>2],(f=a[e+28>>2])?Zf(A,A,r,f,Q):ki(A,A,r,a[e+12>>2],Q))}function Br(A,r,e){var i,Q=0,f=0,B=0,t=0;if(s=i=s-32|0,!ka(r)){ne(e);A:if((Q=sA(e))&&ni(B=A+24|0,r)){ia(r)&&jf(B,4),a[A+36>>2]=0,rf(i+8|0),a[i+16>>2]=2,a[i+20>>2]=0,a[i+8>>2]=i,ia(r)&&jf(i+8|0,4),a[A>>2]=(Dr(r)+31|0)/32<<5,xf(f=A+4|0);r:if(ai(f,32)){if(r=a[a[r>>2]>>2],a[i>>2]=r,a[i+4>>2]=0,a[i+12>>2]=0!=(0|r),Qf(i+8|0))xf(Q);else if(!hi(Q,f,i+8|0,e))break r;if($Q(Q,Q,32)){e:{if(!ka(Q)){if(pr(Q,1))break e;break r}if(!MQ(Q,-1))break r}if(H(Q,0,Q,i+8|0,e)){if(a[A+64>>2]=a[Q+4>>2]>0?a[a[Q>>2]>>2]:0,a[A+68>>2]=0,xf(f),!ai(f,a[A>>2]<<1))break A;if(!H(0,f,f,B,e))break A;(0|(r=a[A+28>>2]))>(0|(Q=a[A+8>>2]))&&IQ(a[f>>2]+(Q<<2)|0,0,r-Q<<2),a[A+8>>2]=r,t=1}}}}Yr(e)}return s=i+32|0,t}function tr(A,r,e){var i,Q=0,f=0,t=0;s=i=s-80|0,f=a[A+16>>2],Q=a[f>>2];A:if(4&B[Q+4|0]){if(!r){Q=0|Ga[a[Q+64>>2]](f,0,e,A);break A}if(2&B[A+9|0]){Q=0|Ga[a[Q+64>>2]](f,r,e,A);break A}if(!(f=ue(f))){Q=0;break A}Q=0|Ga[a[a[f>>2]+64>>2]](f,r,e,A),bQ(f)}else{t=a[Q+64>>2];r:{if(r){if(Q=0,a[i+12>>2]=0,2&B[A+9|0]){if(t){Q=0|Ga[0|t](f,r,e,A);break A}Q=Ri(A,i+16|0,i+12|0)}else{if(!(f=ta()))break A;if(!ur(f,A)){Qa(f);break A}t?(Q=a[f+16>>2],Q=0|Ga[a[a[Q>>2]+64>>2]](Q,r,e,f)):Q=Ri(f,i+16|0,i+12|0),Qa(f)}if(!Q|t)break A;if(Q=0,(0|We(a[A+16>>2],r,e,i+16|0,a[i+12>>2]))>0)break r;break A}if(t){if(Q=0,(0|Ga[0|t](f,0,e,A))>0)break r;break A}if(Q=0,(0|(A=wf(a[A>>2])))<0)break A;if((0|We(f,0,e,0,A))<=0)break A}Q=1}return s=i+80|0,Q}function br(A,r){var e,Q,f=0,t=0,b=0,k=0,n=0,c=0,C=0,E=0;if(s=e=s-16|0,a[e+12>>2]=r,Q=A,A=iQ(81,31289,0))A:{if(i[0|A]=0,(0|Q)>0)for(c=80;;){if((0|(n=ci(b=(f=a[r>>2])||24336)+n|0))>(0|c)){if(!(f=K(A,n+21|0,31289,0))){SQ(A,31289,0);break A}c=n+20|0,A=f}r=r+4|0,f=A,k=0,C=0;r:if(t=c+1|0){e:{for(;;){if(!B[0|f])break e;if(f=f+1|0,k=k+1|0,!(t=t-1|0))break}break r}e:if(!(t>>>0<2))for(t=t-1|0;;){if(!(k=B[0|b]))break e;if(i[0|f]=k,f=f+1|0,b=b+1|0,(0|t)==(0|(C=C+1|0)))break}i[0|f]=0}if(ci(b),(0|(E=E+1|0))==(0|Q))break}(r=Ur())?(r=r+(a[r+384>>2]<<2)|0,1&i[r+192|0]&&(SQ(a[r+128>>2],31289,0),a[r+128>>2]=0),a[r+128>>2]=A,a[r+192>>2]=3):SQ(A,31289,0)}s=e+16|0}function kr(A,r,e,i,Q){r|=0,e|=0,i|=0,Q|=0;var f,t,b=0,k=0;s=t=s-16|0,f=a[(A|=0)+20>>2];A:{r:{if(k=a[f+24>>2]){b=-1;e:switch(a[f+20>>2]-1|0){case 4:if(!(k=a[f+40>>2])&&(k=iQ(sQ(a[A+8>>2]),31289,0),a[f+40>>2]=k,!k)){Ve(4,167,65);break A}if((0|(A=of(Q,i,k,a[a[A+8>>2]+24>>2],5)))<=0){b=0;break A}if(B[(b=A-1|0)+a[f+40>>2]|0]!=(0|DQ(a[a[f+24>>2]>>2]))){b=0,Ve(4,141,100);break A}if((0|wf(a[f+24>>2]))!=(0|b)){b=0,Ve(4,141,143);break A}if(!r)break r;yQ(r,a[f+40>>2],b);break r;case 0:break e;default:break A}if(b=0,(0|v(a[k>>2],0,0,r,t+12|0,i,Q,a[a[A+8>>2]+24>>2]))<=0)break A;b=a[t+12>>2]}else b=of(Q,i,r,a[a[A+8>>2]+24>>2],a[f+20>>2]);if((0|b)<0)break A}a[e>>2]=b,b=1}return s=t+16|0,0|b}function nr(A,r){var e,i=0,Q=0,f=0,B=0,t=0,b=0,k=0;s=e=s-16|0,a[e+12>>2]=r,f=-1;A:if(A&&(i=a[A>>2]))if(Q=a[A+16>>2]){if(a[A+8>>2]||((0|i)>=2&&VA(a[A+4>>2],i,4,Q),a[A+8>>2]=1),r){if(r=0,i=a[A>>2]){B=e+12|0,Q=a[A+4>>2],t=a[A+16>>2];r:{e:if(!((0|i)<=0)){for(;;){if(r=(i+b|0)/2|0,(0|(k=0|Ga[0|t](B,Q+(r<<2)|0)))>=0){if(!k)break e;b=r+1|0}else i=r;if(!((0|i)>(0|b)))break}r=0;break r}for(;(0|(i=r))>0&&(r=i-1|0,!(0|Ga[0|t](B,Q+(r<<2)|0))););r=Q+(i<<2)|0}}r&&(f=r-a[A+4>>2]>>2)}}else{if((0|i)<=0)break A;for(Q=a[A+4>>2],A=0;;){if(a[Q+(A<<2)>>2]==(0|r)){f=A;break A}if((0|i)==(0|(A=A+1|0)))break}}return s=e+16|0,f}function cr(A,r,e){var Q,f=0,B=0,t=0,b=0;if((0|e)<0)return-1;s=Q=s-32|0,f=(Dr(A)+7|0)/8|0;A:{if(-1!=(0|e)){if(!((0|e)>=(0|f))){if(a[Q+24>>2]=a[A+16>>2],f=a[A+12>>2],a[Q+16>>2]=a[A+8>>2],a[Q+20>>2]=f,f=a[A+4>>2],a[Q+8>>2]=a[A>>2],a[Q+12>>2]=f,(0|(f=a[Q+12>>2]))>0){B=a[Q+8>>2]+(f<<2)|0;r:{for(;;){if(a[(B=B-4|0)>>2])break r;if(t=(0|f)>1,f=f-1|0,!t)break}f=0}a[Q+12>>2]=f}if(f||(a[Q+20>>2]=0),f=-1,((Dr(Q+8|0)+7|0)/8|0)>(0|e))break A}}else e=f;if(B=a[A+8>>2]){if(f=0,!e)break A;for(t=1-(B<<2)|0,b=0-(a[A+4>>2]<<2)|0,B=0,r=r+e|0,f=0;i[0|(r=r-1|0)]=B+b>>31&a[a[A>>2]+(-4&f)>>2]>>>(f<<3),f=(f+t>>>31|0)+f|0,(0|(B=B+1|0))!=(0|e););}else Of(r,e);f=e}return s=Q+32|0,f}function Cr(A,r,e,i){var Q=0,f=0,B=0,t=0,b=0,k=0,n=0,c=0;if((0|r)!=(0|e)&&(f=(A=((A-1&(-1^A))>>>31|0)-1|0)&((Q=a[r+4>>2])^a[e+4>>2]),a[r+4>>2]=f^Q,a[e+4>>2]=f^a[e+4>>2],f=A&((Q=a[r+12>>2])^a[e+12>>2]),a[r+12>>2]=f^Q,a[e+12>>2]=f^a[e+12>>2],f=A&((Q=a[r+16>>2])^a[e+16>>2])&4,a[r+16>>2]=f^Q,a[e+16>>2]=f^a[e+16>>2],!((0|i)<=0))){if(f=a[e>>2],r=a[r>>2],e=0,1!=(0|i))for(c=-2&i,Q=0;k=t=(B=e<<2)+r|0,n=A&((b=a[t>>2])^a[(t=f+B|0)>>2]),a[k>>2]=b^n,a[t>>2]=a[t>>2]^n,k=B=a[(t=(b=4|B)+r|0)>>2],B=A&(B^a[(b=f+b|0)>>2]),a[t>>2]=k^B,a[b>>2]=B^a[b>>2],e=e+2|0,(0|c)!=(0|(Q=Q+2|0)););1&i&&(B=r=a[(i=(e<<=2)+r|0)>>2],r=(t=A)&(r^a[(A=e+f|0)>>2]),a[i>>2]=B^r,a[A>>2]=r^a[A>>2])}}function Er(A,r,e){var i=0,Q=0,f=0,B=0,t=0,b=0,k=0;if((0|(t=(B=a[r+4>>2])-(f=a[e+4>>2])|0))<0)return Ve(3,115,100),0;if(!Gf(A,B))return 0;if(Q=(i=a[A>>2])+(b=f<<2)|0,r=rr(k=i,i=a[r>>2],a[e>>2],f),t){if(e=i+b|0,b=3&t)for(i=0;k=a[e>>2],a[Q>>2]=k-r,Q=Q+4|0,e=e+4|0,t=t-1|0,r&=!k,(0|b)!=(0|(i=i+1|0)););if(!((-1^f)+B>>>0<=2))for(;f=a[e>>2],a[Q>>2]=f-r,i=a[e+4>>2],r&=!f,a[Q+4>>2]=i-r,f=a[e+8>>2],r&=!i,a[Q+8>>2]=f-r,i=a[e+12>>2],r&=!f,a[Q+12>>2]=i-r,Q=Q+16|0,e=e+16|0,r&=!i,t=t-4|0;);}r=0;A:if(B){for(;;){if(r=B,a[(Q=Q-4|0)>>2])break A;if(!(B=B-1|0))break}r=0}return a[A+12>>2]=0,a[A+4>>2]=r,1}function or(A,r){var e=0,i=0,Q=0,f=0,B=0,t=0,b=0;if(!(e=r)&&!(e=Pi()))return 0;ne(e);A:if(sA(e)){r:if(!(i=a[A+20>>2])){if((Q=a[A+24>>2])&&(t=a[A+28>>2])&&(b=a[A+32>>2]))if(ne(e),f=sA(e),B=sA(e),(i=sA(e))&&OA(B,t,32936)&&OA(i,b,32936)&&df(f,B,i,e)){if(i=hi(0,Q,f,e),Yr(e),i)break r}else Yr(e);i=0,Ve(4,136,140),f=0;break A}(Q=KQ(1))?(nQ(Q,a[A+16>>2]),f=0,B=Y(0,i,Q,e,a[a[A+8>>2]+24>>2],a[A+68>>2]),xi(Q),B?(a[B+16>>2]=0,f=B):Ve(4,136,3)):(f=0,Ve(4,136,65))}else Ve(4,136,65);return Yr(e),(0|r)!=(0|e)&&Gr(e),a[A+20>>2]!=(0|i)&&xi(i),f}function lr(A,r,e){var i=0,Q=0,f=0,B=0,t=0,b=0,k=0,n=0,c=0,C=0,E=0,o=0,l=0,w=0;if((0|e)<0)return Ve(3,146,119),0;A:{if((0|(f=a[r+4>>2]))<=(0|(i=e>>>5|0)))xf(A);else{if(B=f-i|0,(0|A)!=(0|r)&&!Gf(A,B))break A;if(t=31&e,b=a[r>>2]+(i<<2)|0,Q=a[b>>2],k=a[A>>2],!((0|B)<2)){if(c=(e=0-(n=0-e&31)|0)>>>8|e,l=1&(C=B-1|0),(0|i)!=(f-2|0))for(w=-2&C,e=0,i=0;f=a[(o=4|(E=e<<2))+b>>2],a[k+E>>2]=f<<n&c|Q>>>t,Q=a[((e=e+2|0)<<2)+b>>2],a[k+o>>2]=Q<<n&c|f>>>t,(0|w)!=(0|(i=i+2|0)););else e=0;l&&(i=Q>>>t|0,Q=a[4+((e<<=2)+b|0)>>2],a[e+k>>2]=Q<<n&c|i)}a[(C<<2)+k>>2]=Q>>>t,r=a[r+12>>2],a[A+4>>2]=B,a[A+12>>2]=r}Q=1}return Hi(A),Q}function wr(A,r,e){var i=0,Q=0;A:{r:{e:{i:{Q:{f:{a:{if(r){if(i=a[r+16>>2],e){if(!i)break a;if(!(i=a[i+16>>2]))break a;return void Ga[0|i](A,r)}if(!i)break a;if(!(i=a[i+12>>2]))break a;return void Ga[0|i](A,r)}if(Q=a[A>>2],!(i=a[Q+4>>2]))break A;A=Q+4|0,Q=a[Q>>2];break f}if(5==B[0|r]){if(!(i=a[A>>2]))break A;break e}if(1==(0|(Q=a[r+4>>2])))break Q;if(!(i=a[A>>2]))break A}f:switch(Q+4|0){case 10:oi(i);break r;case 5:break f;case 9:break r;case 0:break i;default:break e}if(!r)return void(a[A>>2]=-1)}return void(a[A>>2]=a[r+20>>2])}wr(A,0,0),SQ(a[A>>2],31289,0);break r}i&&(16&B[i+12|0]||SQ(a[i+8>>2],31289,0),e||SQ(i,31289,0))}a[A>>2]=0}}function gr(A){var r,e=0,i=0,Q=0,f=0,B=0,t=0,b=0;s=r=s-16|0;A:if(A&&!(i=a[A+8>>2])&&(i=0,B=a[A+12>>2])){r:{if(e=a[23070]){if(a[r+12>>2]=A,a[r+8>>2]=0,e=Kr(e,r+8|0))break r;B=a[A+12>>2]}e=1071;e:{if(!B)for(;;){if(t=a[67168+((A=(e+f|0)/2|0)<<2)>>2],!(Q=a[n(t,24)+38492>>2]))break e;if(!((0|(f=(Q=(0|Q)>0)?f:A+1|0))<(0|(e=Q?A:e))))break A}for(;;){if(t=a[67168+((Q=(e+f|0)/2|0)<<2)>>2],b=n(t,24),(i=B-a[b+38492>>2]|0)||(i=$i(a[A+16>>2],a[b+38496>>2],B)),(0|i)<0)e=Q;else{if(!i)break e;f=Q+1|0}if(!((0|e)>(0|f)))break}i=0;break A}i=a[n(t,24)+38488>>2];break A}i=a[a[e+4>>2]+8>>2]}return s=r+16|0,i}function ur(A,r){var e=0,i=0;if(!r||!(i=a[r>>2]))return Ve(6,110,111),0;(0|i)==a[A>>2]&&(e=a[A+12>>2],Kf(A,4)),Qi(A),i=a[r+20>>2],a[A+16>>2]=a[r+16>>2],a[A+20>>2]=i,i=a[r+12>>2],a[A+8>>2]=a[r+8>>2],a[A+12>>2]=i,i=a[r+4>>2],a[A>>2]=a[r>>2],a[A+4>>2]=i,mf(A,1024),a[A+12>>2]=0,a[A+16>>2]=0;A:{if(a[r+12>>2]&&(i=a[a[A>>2]+40>>2])){if(e)a[A+12>>2]=e;else{if(e=iQ(i,31289,0),a[A+12>>2]=e,!e)break A;i=a[a[A>>2]+40>>2]}yQ(e,a[r+12>>2],i)}return a[A+20>>2]=a[r+20>>2],(e=a[r+16>>2])&&(e=ue(e),a[A+16>>2]=e,!e)?(Qi(A),0):(e=a[a[A>>2]+28>>2])?0|Ga[0|e](A,r):1}return Ve(6,110,65),0}function Mr(A,r,e,i){var Q,f=0,B=0,t=0,b=0,k=0,n=0,c=0,C=0;if(!r&&!(r=a[e+4>>2]))return Ve(3,101,107),0;if(Q=a[e+28>>2]){if((0|(t=a[r+4>>2]))<=a[A+8>>2]){if(b=a[A+4>>2],t){if(k=a[A>>2],e=0,t-1>>>0>=3)for(n=-4&t;a[(f=(e<<2)+k|0)>>2]=a[f>>2]&e-b>>31,a[(B=((f=1|e)<<2)+k|0)>>2]=a[B>>2]&f-b>>31,a[(B=((f=2|e)<<2)+k|0)>>2]=a[B>>2]&f-b>>31,a[(B=((f=3|e)<<2)+k|0)>>2]=a[B>>2]&f-b>>31,e=e+4|0,(0|n)!=(0|(c=c+4|0)););if(n=3&t)for(;a[(f=(e<<2)+k|0)>>2]=a[f>>2]&e-b>>31,e=e+1|0,(0|n)!=(0|(C=C+1|0)););}a[A+4>>2]=(t-b|0)<0?b:t}return Zf(A,A,r,Q,i)}return ki(A,A,r,a[e+12>>2],i)}function sr(A,r,e){var i=0;if(!r)return 0;A:if(i=a[r+16>>2]){if(e){if(!(i=a[i+16>>2]))break A;return Ga[0|i](A,r),1}if(i=a[i+8>>2])return 0|Ga[0|i](A,r)}i=-1;A:{r:if(5!=B[0|r]){e:{i:{Q:{f:switch((i=a[r+4>>2])+4|0){case 0:break e;case 9:break i;case 5:break Q;case 10:break f;default:break r}return a[A>>2]=Pe(0),1}return a[A>>2]=a[r+20>>2],1}return a[A>>2]=1,1}if(!(r=iQ(8,31289,0)))return Ve(13,119,65),0;a[r>>2]=-1,a[r+4>>2]=0,a[A>>2]=r;break A}e?(e=a[A>>2],a[e>>2]=0,a[e+4>>2]=0,a[e+8>>2]=0,a[e+12>>2]=128,a[e+4>>2]=i):(e=xQ(i),a[A>>2]=e),!e|5!=B[0|r]||(a[e+12>>2]=64|a[e+12>>2])}return 0!=a[A>>2]}function Dr(A){var r,e=0,i=0,Q=0,f=0,t=0,b=0,k=0,n=0,c=0,C=0;if(r=(i=a[A+4>>2])-1|0,4&B[A+16|0]){if((0|(t=a[A+8>>2]))>0)for(k=0-i|0,n=a[A>>2],A=0;c=e,b|=e=((A^r)-1&k)>>31,C=e,f=(Q=(e=a[(A<<2)+n>>2])>>>0>65535)<<4|0!=(0|e),f|=(Q=(e=Q?e>>>16|0:e)>>>0>255)<<3,e=(c+(32&(-1^b))|0)+(C&((f|=(Q=(e=Q?e>>>8|0:e)>>>0>15)<<2)|(Q=(e=Q?e>>>4|0:e)>>>0>3)<<1)+((Q?e>>>2|0:e)>>>0>1))|0,(0|t)!=(0|(A=A+1|0)););return i?e:0}return i?(e=r<<5|0!=(0|(A=a[a[A>>2]+(r<<2)>>2]))|(i=A>>>0>65535)<<4,e|=(i=(A=i?A>>>16|0:A)>>>0>255)<<3,((e|=(i=(A=i?A>>>8|0:A)>>>0>15)<<2)|(i=(A=i?A>>>4|0:A)>>>0>3)<<1)+((i?A>>>2|0:A)>>>0>1)|0):0}function Ir(A,r,e){var i,Q=0,f=0,B=0,t=0,b=0,k=0;if(Q=a[r+4>>2]<a[e+4>>2],f=a[(B=Q?r:e)+4>>2],!Gf(A,(i=a[(r=Q?e:r)+4>>2])+1|0))return 0;if(a[A+4>>2]=i,e=(Q=a[A>>2])+(t=f<<2)|0,Q=oA(Q,r=a[r>>2],a[B>>2],f),B=i-f|0){if(r=r+t|0,b=3&B)for(t=0;k=a[r>>2]+Q|0,a[e>>2]=k,e=e+4|0,r=r+4|0,B=B-1|0,Q&=!k,(0|b)!=(0|(t=t+1|0)););if(!((-1^f)+i>>>0<3))for(;f=a[r>>2]+Q|0,a[e>>2]=f,f=(Q&=!f)+a[r+4>>2]|0,a[e+4>>2]=f,f=(Q&=!f)+a[r+8>>2]|0,a[e+8>>2]=f,f=(Q&=!f)+a[r+12>>2]|0,a[e+12>>2]=f,Q&=!f,e=e+16|0,r=r+16|0,B=B-4|0;);}return a[e>>2]=Q,a[A+12>>2]=0,a[A+4>>2]=a[A+4>>2]+Q,1}function Nr(A,r){var e,i=0,Q=0,f=0,B=0;s=e=s-16|0,a[e+12>>2]=0;A:if(A&&(i=a[A>>2])&&a[i+28>>2]){Q=a[A+8>>2];r:{e:{i:{Q:{if(!(i=a[A+4>>2])){if(Q)break Q;break r}if(!Q)break i}i=0|Ga[0|Q](A,5,r,511,0,0,1,0);break e}i=0|Ga[0|i](A,5,r,511,0,1)}if((0|i)<=0)break A}if(a[A+16>>2]){(0|(Q=0|Ga[a[a[A>>2]+28>>2]](A,r,511)))<=0?i=0:(a[e+12>>2]=Q,i=Q,Q=1),f=a[A+8>>2];r:{e:{if(!(B=a[A+4>>2])){if(f)break e;break A}if(!f)break r}Ga[0|f](A,133,r,511,0,0,Q,e+12|0);break A}Ga[0|B](A,133,r,511,0,(0|Q)>0?i:Q)}else Ve(32,104,120)}else Ve(32,104,121);s=e+16|0}function Gr(A){var r=0;if(A){if(SQ(a[A+20>>2],31289,0),a[A+20>>2]=0,r=a[A>>2])for(;a[r>>2]&&yi(r),a[r+20>>2]&&yi(r+20|0),a[r+40>>2]&&yi(r+40|0),a[r+60>>2]&&yi(r+60|0),a[r+80>>2]&&yi(r+80|0),a[r+100>>2]&&yi(r+100|0),a[r+120>>2]&&yi(r+120|0),a[r+140>>2]&&yi(r+140|0),a[r+160>>2]&&yi(r+160|0),a[r+180>>2]&&yi(r+180|0),a[r+200>>2]&&yi(r+200|0),a[r+220>>2]&&yi(r+220|0),a[r+240>>2]&&yi(r+240|0),a[r+260>>2]&&yi(r+260|0),a[r+280>>2]&&yi(r+280|0),a[r+300>>2]&&yi(r+300|0),r=a[A>>2],a[A+4>>2]=a[r+324>>2],SQ(r,31289,0),r=a[A+4>>2],a[A>>2]=r,r;);SQ(A,31289,0)}}function Ur(){var A,r=0,e=0,Q=0;A=a[23332];A:{r:if(er(262144)&&!(!If(91948,44)|!a[22988])){e:{i:switch((r=Af(91940))+1|0){case 0:break r;case 1:break i;default:break e}if(!_Q(91940,-1))break r;if(!(r=Ki(392,31289)))break A;i:if(B[92116])Ve(15,116,70);else Q:if(!(!If(92132,76)|!a[23034]||!If(92140,77)|!a[23036]||!If(92148,78)|!(1&i[92152]))){if(!(e=Af(91016))){if(!(e=Ki(12,31289)))break Q;if(!_Q(91016,e)){SQ(e,31289,0);break i}}a[e+4>>2]=1,Q=1}if(!Q||!_Q(91940,r)){DA(r);break A}er(2)}a[23332]=A,Q=r}return Q}return _Q(91940,0),0}function Fr(A,r,e){var i,Q=0,f=0;s=i=s-16|0;A:{r:if(Q=HQ(89720)){if(20!=(0|e)){if(f=xQ(2),a[Q+8>>2]=f,!f)break r;if(!Ee(f,e))break r}if(!(!A|64==a[A>>2])){if(e=HQ(32744),a[Q>>2]=e,!e)break r;wi(e,A)}if(a[i+12>>2]=0,a[Q+4>>2]=0,!(A=r||A))break A;if(64!=a[A>>2]){e=0;e:{if(64!=a[A>>2]){if(!(e=HQ(32744))){e=0;break e}wi(e,A)}se(e,32744,i+12|0)&&(r=HQ(32744),a[Q+4>>2]=r,r&&(Ei(r,Pe(911),16,a[i+12>>2]),a[i+12>>2]=0))}if(wQ(a[i+12>>2]),la(e),!a[Q+4>>2])break r}if(64==a[A>>2])break A;if(r=HQ(32744),a[Q+16>>2]=r,r){wi(r,A);break A}}Ea(Q),Q=0}return s=i+16|0,Q}function Zr(A,r,e){var i,Q,f=0,B=0,t=0,b=0,k=0;if(s=Q=s-128|0,(0|(i=a[r+4>>2]))<=0)a[A+12>>2]=0,a[A+4>>2]=0,b=1;else{ne(e),(0|(f=A))==(0|r)&&(f=sA(e));A:if(!(!f|!(B=sA(e)))&&Gf(f,t=i<<1)){r:{e:switch(i-4|0){case 0:tA(a[f>>2],a[r>>2]);break r;case 4:d(a[f>>2],a[r>>2]);break r;default:break e}if(i>>>0<=15)UA(a[f>>2],a[r>>2],i,Q);else if(1<<(k=pi(i)-1|0)!=(0|i)){if(!Gf(B,t))break A;UA(a[f>>2],a[r>>2],i,a[B>>2])}else{if(!Gf(B,4<<k))break A;qA(a[f>>2],a[r>>2],i,a[B>>2])}}a[f+4>>2]=t,a[f+12>>2]=0,((0|A)==(0|f)||ni(A,f))&&(b=1)}Yr(e)}return s=Q+128|0,b}function dr(A,r){var e,i,Q;return e=B[r+8|0]|B[r+9|0]<<8|B[r+10|0]<<16|B[r+11|0]<<24,i=B[r+12|0]|B[r+13|0]<<8|B[r+14|0]<<16|B[r+15|0]<<24,Q=B[0|r]|B[r+1|0]<<8|B[r+2|0]<<16|B[r+3|0]<<24,r=B[r+4|0]|B[r+5|0]<<8|B[r+6|0]<<16|B[r+7|0]<<24,a[A>>2]=0,a[A+4>>2]=0,a[A+40>>2]=0,a[A+48>>2]=2,a[A+52>>2]=4,a[A+8>>2]=1886610805^Q,a[A+12>>2]=1936682341^r,a[A+16>>2]=1852075885^e,a[A+20>>2]=1685025377^i,a[A+24>>2]=1852142177^Q,a[A+28>>2]=1819895653^r,a[A+32>>2]=2037671283^e,a[A+36>>2]=1952801890^i,r=(r=a[A+44>>2])||16,a[A+44>>2]=r,16==(0|r)&&(a[A+16>>2]=1852075907^e,a[A+20>>2]=1685025377^i),1}function Wr(A,r,e,i,Q,f,t){var b,k=0,n=0;s=b=s-16|0;A:if(A){k=a[i>>2],a[b+8>>2]=a[r>>2];r:{e:{if(16&k){n=58;i:switch((Q=vA(b+12|0,0,0,b+7|0,b+6|0,b+8|0,e,a[i+4>>2],192&k,Q,f))+1|0){case 0:break A;case 1:break r;default:break i}if(!B[b+6|0]){n=120;break r}if(e=a[b+8>>2],!kA(A,b+8|0,a[b+12>>2],i,0,f,t))break r;f=a[b+8>>2],A=a[b+12>>2]+(e-f|0)|0,a[b+12>>2]=A;i:{if(B[b+7|0]){if(i=137,B[f+1|0]|B[0|f]|(0|A)<2)break i;f=f+2|0;break e}if(i=119,!A)break e}Q=0,Ve(13,132,i);break A}Q=kA(A,r,e,i,Q,f,t);break A}a[r>>2]=f,Q=1;break A}Ve(13,132,n),Q=0}else Q=0;return s=b+16|0,Q}function Vr(A,r){var e=0;return mf(A,2),(0|(e=a[A>>2]))==(0|r)||(e&&(a[e+32>>2]&&($f(A,2)||Ga[a[a[A>>2]+32>>2]](A)),!a[A+12>>2]|a[a[A>>2]+40>>2]<=0||($f(A,4),pQ(a[A+12>>2],a[a[A>>2]+40>>2]),a[A+12>>2]=0)),a[A>>2]=r,1&i[A+9|0]||!(e=a[r+40>>2])||(a[A+20>>2]=a[r+20>>2],r=Ki(e,31289),a[A+12>>2]=r,r))?((!(e=a[A+16>>2])||(r=0,(0|(e=Ie(e,-1,248,7,0,A)))>0||-2==(0|e)))&&(r=1,1&i[A+9|0]||(r=0|Ga[a[a[A>>2]+16>>2]](A))),r):(Ve(6,128,65),0)}function Yr(A){var r=0,e=0,i=0,Q=0,f=0,B=0;if(A){if(r=a[A+36>>2])return void(a[A+36>>2]=r-1);if(r=a[A+24>>2]-1|0,a[A+24>>2]=r,!((Q=a[a[A+20>>2]+(r<<2)>>2])>>>0>=(f=a[A+32>>2])>>>0)&&(r=a[A+12>>2],e=f-Q|0,a[A+12>>2]=r-e,e&&(B=Q+1|0,i=r=r-1&15,1&e&&(e=e-1|0,i=r-1|0,r||(a[A+4>>2]=a[a[A+4>>2]+320>>2],i=15)),r=i,(0|f)!=(0|B))))for(;e=e-2|0,r?r=r-1|0:(a[A+4>>2]=a[a[A+4>>2]+320>>2],r=15),r?r=r-1|0:(a[A+4>>2]=a[a[A+4>>2]+320>>2],r=15),e;);a[A+40>>2]=0,a[A+32>>2]=Q}}function Rr(A,r){var e=0,i=0,Q=0;A:if(a[r+12>>2]||ka(r))Ve(3,138,115);else if(1!=(0|(e=Dr(r)))){r:{if(!gQ(r,e-2|0)&&!gQ(r,e-3|0)){for(e=e+1|0,i=100;;){if(Q=0,!xr(A,e,-1,0))break A;if(!((0|Ye(A,r))<0)){if(!OA(A,A,r))break A;if(!((0|Ye(A,r))<0||OA(A,A,r)))break A}if(!(i=i-1|0))break r;if(Q=1,!((0|Ye(A,r))>=0))break}break A}for(i=100;;){if(Q=0,!xr(A,e,-1,0))break A;if(!(i=i-1|0))break r;if(Q=1,!((0|Ye(A,r))>=0))break}break A}Ve(3,138,113)}else xf(A),Q=1;return Q}function yr(A,r,e,Q,f){var a,B,t=0,b=0,k=0,n=0;s=a=s-80|0;A:if(B=ta()){if((0|(n=wf(f)))<0)t=-1;else if(!((0|r)<=0)){for(;;){if(b=t<<8&16711680|t<<24|t>>>8&65280|t>>>24,i[a+76|0]=b,i[a+77|0]=b>>>8,i[a+78|0]=b>>>16,i[a+79|0]=b>>>24,!Vr(B,f)){t=-1;break A}if(!nf(B,e,Q)){t=-1;break A}if(!nf(B,a+76|0,4)){t=-1;break A}r:{e:{if((0|(b=k+n|0))<=(0|r)){if(Ri(B,A+k|0,0))break e;t=-1;break A}if(t=-1,!Ri(B,a,0))break A;yQ(A+k|0,a,r-k|0);break r}if(t=t+1|0,k=b,(0|b)<(0|r))continue}break}t=0}}else t=-1;return Of(a,64),Qa(B),s=a+80|0,t}function hr(A,r){var e,i,Q=0,f=0;if(s=e=s-16|0,i=e+4|0,f=e+8|0,(Q=e+12|0)&&(a[Q>>2]=a[r>>2]),i&&((Q=a[r+4>>2])?(a[i>>2]=a[Q>>2],f&&(a[f>>2]=a[Q+4>>2])):a[i>>2]=-1),f=1,912==(0|gr(a[e+12>>2]))&&(f=1,Q=a[e+4>>2]+1|0))if(17==(0|Q)){f=A,A=0;A:if(r=zi(89720,a[r+4>>2])){r:{if(Q=a[r+4>>2]){if(911!=(0|gr(a[Q>>2]))){a[r+16>>2]=0;break r}if(Q=zi(32744,a[Q+4>>2]),a[r+16>>2]=Q,!Q)break r}A=r;break A}Ea(r),A=0}Q=A,a[f+52>>2]=Q,f=0!=(0|Q)}else Ve(4,164,149),f=0;return s=e+16|0,f}function Jr(A,r,e,Q,f,a,B){var t,b,k=0,c=0,C=0,E=0;if(s=t=s-80|0,k=(B>>>0)/((b=wf(A))>>>0)|0,a&&!((E=k+((0|n(k,b))!=(0|B))|0)>>>0>255)&&(k=_r())){C=0;A:if(ZA(k,r,e,A)){if(E)for(A=1;;){i[t+15|0]=A;r:{e:{if(A>>>0>=2){if(!ZA(k,0,0,0))break e;if(!tf(k,t+16|0,b))break e}if(tf(k,Q,f)&&(tf(k,t+15|0,1)&&Ci(k,t+16|0,0)))break r}C=0;break A}if(yQ(a+c|0,t+16|0,r=c+b>>>0>B>>>0?B-c|0:b),c=r+c|0,!(E>>>0>=(A=A+1|0)>>>0))break}C=a}c=C,Of(t+16|0,64),CQ(k)}return s=t+80|0,c}function Tr(A){var r=0,e=0,i=0,Q=0,f=0,t=0,k=0;A:{r:{if((Q=a[23056])>>>0>A>>>0)break A;if((f=a[23057])+Q>>>0<=A>>>0)break A;if(e=(t=a[23059])-1|0,!((r=a[23060])>>>0>(i=(A+f|0)-Q|0)>>>0)){for(r=(i>>>0)/(r>>>0)|0,i=a[23061];;){if(B[(r>>>3|0)+i|0]>>>(7&r)&1)break r;if(1&r)break;if(e=e-1|0,k=r>>>0<2,r=r>>>1|0,k)break r}break A}i=a[23061]}if((0|e)<(0|t)&(0|e)>=0&&!((A=A-Q|0)&(r=f>>>e|0)-1||!(A=((A>>>0)/(r>>>0)|0)+(1<<e)|0)|A>>>0>=b[23063]|!(B[(A>>>3|0)+i|0]>>>(7&A)&1)))return r}Ia(),E()}function mr(A,r,e,i,Q,f,B){var t,b=0,k=0,n=0,c=0;s=t=s-240|0,b=a[i>>2],a[t+232>>2]=b,i=a[i+4>>2],a[t>>2]=A,a[t+236>>2]=i,n=1;A:{r:{e:{if(i|1!=(0|b)){for(c=0-r|0,b=A;;){if(i=b-a[(k=(Q<<2)+B|0)>>2]|0,(0|Ga[0|e](i,A))<=0){i=b;break e}i:{if(!((0|Q)<2|f)){if(f=a[k-8>>2],k=b+c|0,(0|Ga[0|e](k,i))>=0)break i;if((0|Ga[0|e](k-f|0,i))>=0)break i}if(a[(n<<2)+t>>2]=i,cQ(b=t+232|0,f=qQ(b)),n=n+1|0,Q=Q+f|0,f=0,b=i,a[t+236>>2]|1!=a[t+232>>2])continue;break r}break}i=b;break r}i=A}if(f)break A}Te(r,t,n),ye(i,r,e,Q,B)}s=t+240|0}function Lr(A,r,e){var i,Q=0,f=0,B=0;s=i=s-16|0;A:if(768&(f=a[r>>2])){Q=a[r+16>>2];r:{if(!(A=a[a[A>>2]+a[Q+4>>2]>>2])){if(!(r=a[Q+24>>2]))break r;break A}if(A=256&f?gr(A):WA(A),a[i+12>>2]=A,(A=a[Q+8>>2])&&!(0|Ga[0|A](i+12|0))){r=0,Ve(13,110,164);break A}if((0|(f=a[Q+16>>2]))>0)for(r=a[Q+12>>2],A=0,B=a[i+12>>2];;){if(a[r>>2]==(0|B)){r=r+4|0;break A}if(r=r+24|0,(0|f)==(0|(A=A+1|0)))break}if(r=a[Q+20>>2])break A}r=0,e&&Ve(13,110,164)}return s=i+16|0,r}function Xr(A,r,e){var i,Q,f=0,B=0,t=0,b=0,k=0,n=0,c=0,C=0,E=0,o=0;if(!Gf(A,(Q=(i=(0|e)/32|0)+1|0)+a[r+4>>2]|0))return 0;if(f=a[r+4>>2]){if(c=a[r>>2],B=a[(c+(f<<=2)|0)-4>>2],t=f+(n=(k=a[A>>2])+(i<<2)|0)|0,E=(b=0-(C=31&(f=0-e|0))|0)>>>8|b,a[t>>2]=E&B>>>f,o=31&e,(0|(f=a[r+4>>2]))>=2)for(;b=f-1|0,t=B<<o,B=a[((f<<2)+c|0)-8>>2],a[(b<<2)+n>>2]=t|B>>>C&E,t=f>>>0>2,f=b,t;);a[n>>2]=B<<o}else k=a[A>>2],a[k+(i<<2)>>2]=0;return e+31>>>0>=63&&IQ(k,0,i<<2),a[A+12>>2]=a[r+12>>2],a[A+4>>2]=a[r+4>>2]+Q,1}function Sr(A){var r,e=0,Q=0;if(!A)return 0;e=a[A+16>>2],r=Ki(20,31289);A:{r:{if(8&e){if(e=9,r)break r;break A}if(e=1,!r)break A}if(a[r+16>>2]=e,(0|A)!=(0|r)){if(!((0|(Q=a[(4&B[A+16|0]?8:4)+A>>2]))>a[r+8>>2])||ke(r,Q))return(0|(e=a[A+4>>2]))>0&&(yQ(a[r>>2],a[A>>2],Q<<2),e=a[A+4>>2]),A=a[A+12>>2],a[r+4>>2]=e,a[r+12>>2]=A,r;2&(e=a[r+16>>2])||(A=a[r>>2],8&e?x(A,a[r+8>>2]<<2):SQ(A,31289,0)),A=0,1&i[r+16|0]&&SQ(r,31289,0)}return A}return Ve(3,113,65),0}function Hr(A,r){var e=0,i=0;A:{r:{e:{i:{Q:{f:{for(;;){if(774&t[r>>1])break A;if(e=a[r+16>>2],i=B[0|e]){a:switch((i<<24>>24)-1|0){case 3:break a;case 0:case 1:case 5:break Q;case 4:break f;default:break i}if((r=a[e+16>>2])&&(r=a[r+12>>2]))return void Ga[0|r](A,e);break A}if(!(r=a[e+8>>2]))break}if(r=a[e+16>>2])break e;break r}if(r=a[e+16>>2])break e;break r}a[A>>2]=0}return}if(r=a[r+16>>2])return void Ga[0|r](A,e);break A}if(!(5==(0|i)|1!=a[e+4>>2]))return void(a[A>>2]=a[e+20>>2])}a[A>>2]=0}function pr(A,r){var e=0,i=0,Q=0,f=0;if(!r)return 1;if(ka(A)){if(!(i=MQ(A,r)))return 0;a[A+12>>2]=!!a[A+4>>2]}else{if(a[A+12>>2])return a[A+12>>2]=0,r=Be(A,r),a[A+12>>2]=1,r;if(Q=a[A>>2],e=a[Q>>2],!(1!=a[A+4>>2]|e>>>0>=r>>>0))return a[Q>>2]=r-e,a[A+12>>2]=1,1;if(r>>>0<=e>>>0)i=Q;else if(a[Q>>2]=e-r,i=Q+4|0,f=1,e=a[Q+4>>2])r=1;else for(;a[i>>2]=-1,r=1,!(e=a[(i=((f=f+1|0)<<2)+Q|0)>>2]););if(a[i>>2]=e-r,i=1,!((0|r)!=(0|e)|(a[A+4>>2]-1|0)!=(0|f)))return a[A+4>>2]=f,1}return i}function vr(A,r){var e,i,Q=0,f=0,B=0,t=0;if(ka(r))return xf(A),1;if(e=a[r>>2],f=a[r+4>>2],(0|A)!=(0|r)){if(!Gf(A,f))return 0;a[A+12>>2]=a[r+12>>2]}if(a[A+4>>2]=f,B=a[(Q=(r=f-1|0)<<2)+e>>2],i=a[A>>2],a[Q+i>>2]=B>>>1,Q=a[A+4>>2]-(1==(0|B))|0,a[A+4>>2]=Q,(0|f)>=2){if(1&r&&(t=B<<31,B=a[(Q=(r=f-2|0)<<2)+e>>2],a[Q+i>>2]=B>>>1|t),2!=(0|f))for(;f=a[(Q=(t=r-1|0)<<2)+e>>2],a[Q+i>>2]=B<<31|f>>>1,B=a[(Q=(r=r-2|0)<<2)+e>>2],a[Q+i>>2]=f<<31|B>>>1,t>>>0>1;);Q=a[A+4>>2]}return Q||(a[A+12>>2]=0),1}function zr(A,r,e,Q,f){var B=0,t=0,b=0;A:if(a[A>>2]|r&&!((B=a[e>>2])>>>0>(t=a[Q>>2])>>>0)){if(!(!r|(0|B)!=(0|t))){if(B>>>0>2147482623)break A;if(B=B+1024|0,a[Q>>2]=B,t=a[r>>2]){if(!(B=K(t,B,31289,0)))break A;a[r>>2]=B}else{if(B=iQ(B,31289,0),a[r>>2]=B,!B)return Ve(32,150,65),0;if(t=a[e>>2]){if(!(b=a[A>>2]))return 0;yQ(B,b,t)}a[A>>2]=0}}if(b=1,!((B=a[Q>>2])>>>0<=(Q=a[e>>2])>>>0)){if(A=a[A>>2])return a[e>>2]=Q+1,i[A+Q|0]=f,1;A=a[r>>2],a[e>>2]=Q+1,i[A+Q|0]=f}}return b}function jr(A,r,e,i){var Q=0,f=0;Q=e-1|0;A:if(!((0|i)>=0)){for(f=i;;){if(!a[(Q-f<<2)+r>>2]){if(f=f+1|0)continue;break A}break}return-1}A:if(!((0|i)<=0)){for(;;){if(!a[(i+Q<<2)+A>>2]){if(f=(0|i)<2,i=i-1|0,!f)continue;break A}break}return 1}f=0;A:if(e){if((0|(Q=a[(i=Q<<2)+A>>2]))!=(0|(i=a[r+i>>2])))return i>>>0<Q>>>0?1:-1;if(!((0|e)<2)){for(i=e-2|0;;){if((0|(Q=a[(e=i<<2)+A>>2]))==(0|(e=a[r+e>>2]))){if(e=(0|i)<=0,i=i-1|0,!e)continue;break A}break}f=e>>>0<Q>>>0?1:-1}}return f}function xr(A,r,e,Q){var f,a=0,t=0,b=0;s=f=s-16|0;A:{r:{e:{if(!r){if(-1!=(0|e)|Q)break e;xf(A),b=1;break A}if(!(1==(0|r)&(0|e)>0|(0|r)<0)){if(!(a=iQ(t=(r=r+7|0)>>>3|0,31289,0))){Ve(3,127,65);break r}fQ(a,t),b=510<<(r&=7);i:if((0|e)<0)r=B[0|a];else{if(e){if(!r){r=1,i[0|a]=1,i[a+1|0]=128|B[a+1|0];break i}r=B[0|a]|3<<r-1}else r=B[0|a]|1<<r;i[0|a]=r}i[0|a]=(-1^b)&r,Q&&(i[0|(r=(a+t|0)-1|0)]=1|B[0|r]),b=0!=(0|hA(a,t,A));break r}}Ve(3,127,118);break A}pQ(a,t)}return s=f+16|0,b}function Kr(A,r){var e,i=0,Q=0,f=0;a[A+92>>2]=0,e=0|Ga[a[A+8>>2]](r),a[A+56>>2]=a[A+56>>2]+1,Q=a[A>>2],(i=(e>>>0)%b[A+24>>2]|0)>>>0<b[A+20>>2]&&(i=(e>>>0)%b[A+16>>2]|0);A:{r:{e:if(i=a[(Q=(i<<2)+Q|0)>>2]){for(f=a[A+4>>2];;){if(a[A+88>>2]=a[A+88>>2]+1,a[i+8>>2]!=(0|e)||(a[A+60>>2]=a[A+60>>2]+1,0|Ga[0|f](a[i>>2],r))){if(Q=i+4|0,i=a[i+4>>2])continue;break e}break}if(r=a[Q>>2])break r}i=A+84|0,A=0;break A}i=A+80|0,A=a[r>>2]}return a[i>>2]=a[i>>2]+1,A}function Or(A,r){var e,Q=0,f=0,t=0;s=e=s-16|0;A:if(r&&!(1&i[r+5|0]&&(Q=-1,f=1,1!=(0|Ar(A,30163,1))))){r:{if(Q=a[r>>2]){if((0|Q)>0)break r;Q=f;break A}Q=2==(0|Ar(A,29984,2))?2|f:-1;break A}for(Q=f;;){if(!(!t|(t>>>0)%35)){if(2!=(0|Ar(A,31179,2))){Q=-1;break A}Q=Q+2|0}if(f=a[r+8>>2]+t|0,i[e+14|0]=B[21439+(B[0|f]>>>4|0)|0],i[e+15|0]=B[21439+(15&B[0|f])|0],2!=(0|Ar(A,e+14|0,2))){Q=-1;break A}if(Q=Q+2|0,!((0|(t=t+1|0))<a[r>>2]))break}}return s=e+16|0,Q}function Pr(A){var r=0,e=0,Q=0;if(A&&(Q=iQ(ci(A)+1|0,31289,0))){A:{if(3&((e=Q)^A))r=B[0|A];else{if(3&A)for(;;){if(r=B[0|A],i[0|e]=r,!r)break A;if(e=e+1|0,!(3&(A=A+1|0)))break}if(!((-1^(r=a[A>>2]))&r-16843009&-2139062144))for(;a[e>>2]=r,r=a[A+4>>2],e=e+4|0,A=A+4|0,!(r-16843009&(-1^r)&-2139062144););}if(i[0|e]=r,255&r)for(;r=B[A+1|0],i[e+1|0]=r,e=e+1|0,A=A+1|0,r;);}e=Q}return e}function qr(A,r,e,i,Q,f,B){var t,b=0;return s=t=s-80|0,!(!(b=_r())||!ZA(b,r|e?r:37988,e,A)|!a[b>>2]||!nf(a[b+4>>2],i,Q)|!a[b>>2])&&Ri(a[b+4>>2],t,t+76|0)&&ur(a[b+4>>2],a[b+12>>2])&&nf(a[b+4>>2],t,a[t+76>>2])&&(e=f||92048,Ri(a[b+4>>2],e,B))?(Qi(a[b+8>>2]),Qi(a[b+12>>2]),Qi(a[b+4>>2]),a[b>>2]=0,Qa(a[b+8>>2]),Qa(a[b+12>>2]),Qa(a[b+4>>2]),SQ(b,37988,0)):(CQ(b),e=0),s=t+80|0,e}function _r(){var A,r=0,e=0;if(A=Ki(16,37988)){Qi(a[A+8>>2]),Qi(a[A+12>>2]),Qi(a[A+4>>2]),a[A>>2]=0;A:{if((a[A+8>>2]||(r=ta(),a[A+8>>2]=r,r))&&(a[A+12>>2]||(r=ta(),a[A+12>>2]=r,r))){if(r=1,a[A+4>>2])break A;if(e=ta(),a[A+4>>2]=e,e)break A}Qi(a[A+8>>2]),Qi(a[A+12>>2]),Qi(a[A+4>>2]),r=0,a[A>>2]=0}if(r)return A;Qi(a[A+8>>2]),Qi(a[A+12>>2]),Qi(a[A+4>>2]),a[A>>2]=0,Qa(a[A+8>>2]),Qa(a[A+12>>2]),Qa(a[A+4>>2]),SQ(A,37988,0)}return 0}function $r(){var A=0,r=0,e=0,Q=0,f=0,B=0;A:if((r=Ur())&&(0|(A=a[r+384>>2]))!=a[r+388>>2]){for(Q=r+128|0;;){if(!(1&(B=a[(e=(f=A<<2)+r|0)>>2]))){if(1&i[e+192|0]&&(SQ(a[Q+f>>2],31289,0),A=a[r+384>>2],a[(A<<2)+Q>>2]=0),a[192+((A<<2)+r|0)>>2]=0,a[(a[r+384>>2]<<2)+r>>2]=0,A=(a[r+384>>2]<<2)+r|0,a[A+320>>2]=-1,a[A+256>>2]=0,a[A- -64>>2]=0,A=(0|(A=a[r+384>>2]))>0?A-1|0:15,a[r+384>>2]=A,a[r+388>>2]!=(0|A))continue;break A}break}a[e>>2]=-2&B}}function Ae(A,r){var e=0,i=0,Q=0,f=0;if(A&&!((0|(Q=(i=a[A+12>>2])-1|0))<0)){if(1&i){if(e=a[a[A>>2]+(Q<<2)>>2])for(;f=a[e+4>>2],Ga[0|r](a[e>>2]),e=f;);Q=i-2|0}if(1!=(0|i))for(;;){if(e=a[(i=Q<<2)+a[A>>2]>>2])for(;f=a[e+4>>2],Ga[0|r](a[e>>2]),e=f;);if(e=a[(i+a[A>>2]|0)-4>>2])for(;f=a[e+4>>2],Ga[0|r](a[e>>2]),e=f;);if(!((0|(Q=Q-2|0))>=0))break}}}function re(A,r){var e=0,Q=0,f=0;A:{r:if(!((e=a[A>>2])>>>0>=r>>>0)){if(b[A+8>>2]>=r>>>0){if(!(Q=a[A+4>>2]))break r;return IQ(e+Q|0,0,r-e|0),a[A>>2]=r,r}if(r>>>0>=1610612733)break A;Q=(r+3>>>0)/3<<2;e:{i:{if(1&i[A+12|0]){if(e=j(Q),!(f=a[A+4>>2])|!e)break i;yQ(e,f,a[A>>2]),x(a[A+4>>2],a[A>>2]);break e}e=K(a[A+4>>2],Q,31289,0)}if(!e)break A}a[A+8>>2]=Q,a[A+4>>2]=e,IQ((f=e)+(e=a[A>>2])|0,0,r-e|0)}return a[A>>2]=r,r}return Ve(7,100,65),0}function ee(A,r,e,i,Q){var f=0,B=0;if((0|e)>=(0|Q)?(f=e,B=i,e=Q,i=r):(f=Q,B=r),(0|e)<=0)XA(A,i,f,0);else{a[(Q=(f<<2)+A|0)>>2]=XA(A,i,f,a[B>>2]);A:if(1!=(0|e))for(;;){if(a[Q+4>>2]=NA(A+4|0,i,f,a[B+4>>2]),2==(0|e))break A;if(a[Q+8>>2]=NA(A+8|0,i,f,a[B+8>>2]),e>>>0<4)break A;if(a[Q+12>>2]=NA(A+12|0,i,f,a[B+12>>2]),4==(0|e))break A;if(A=A+16|0,a[Q+16>>2]=NA(A,i,f,a[B+16>>2]),Q=Q+16|0,B=B+16|0,!((e=e-4|0)>>>0>1))break}}}function ie(A,r){var e=0,i=0,Q=0,f=0,B=0,t=0,b=0;if(i=-1,r){if(!a[A+4>>2])return 0;if($Q(A,A,Q=32-pi(r)|0)){if((0|(e=a[A+4>>2]))<=0)r=0;else{for(b=r<<Q,t=a[A>>2],r=0;f=_A(f=r,r=a[(B=(i=e-1|0)<<2)+t>>2],b),t=a[A>>2],a[B+t>>2]=f,r=r-n(b,f)|0,B=e>>>0>1,e=i,B;);if(!((0|(e=a[A+4>>2]))<=0)){if(e=e-1|0,a[a[A>>2]+(e<<2)>>2])return r>>>Q|0;a[A+4>>2]=e}}i=r>>>Q|0,e||(a[A+12>>2]=0)}}return i}function Qe(A,r,e,i){var Q=0;if(A){A:{if(Q=a[A>>2]){if(Q=TA(pe(gr(a[Q>>2])))){a[r>>2]=Q;break A}return Ve(4,156,166),a[r>>2]=0,0}a[r>>2]=37664}A:{if(r=a[A+16>>2]){if(r=TA(pe(gr(a[r>>2])))){a[e>>2]=r;break A}return Ve(4,156,166),a[e>>2]=0,0}a[e>>2]=37664}A:{if(r=a[A+8>>2]){if(r=WA(r),a[i>>2]=r,(0|r)>=0)break A;return Ve(4,151,150),0}a[i>>2]=20}if(!(A=a[A+12>>2]))return 1;Q=1,1!=(0|WA(A))&&(Q=0,Ve(4,151,139))}return Q}function fe(A,r,e,i,Q,f){var B,t=0;s=B=s-16|0,a[B+12>>2]=0,a[B+8>>2]=0;A:if(t=a[a[f+8>>2]+44>>2])A=0|Ga[0|t](A,r,e,i,Q,f);else{r:{e:{i:{if(114==(0|A)){if(t=36,36==(0|e))break i;A=0,Ve(4,117,131);break A}if(A=Re(B+8|0,B+12|0,A,r,e),t=a[B+12>>2],e=a[B+8>>2],A){r=e;break e}A=0;break r}a[B+12>>2]=36,e=0}(hf(f)-11|0)<(0|t)?(A=0,Ve(4,117,112)):(A=1,(0|(r=cf(t,r,i,f,1)))<=0?A=0:a[Q>>2]=r)}pQ(e,t)}return s=B+16|0,A}function ae(A,r){var e=0,i=0;A:{if(i=255&r){if(3&A)for(;;){if(!(e=B[0|A])|(0|e)==(255&r))break A;if(!(3&(A=A+1|0)))break}r:if(!((-1^(e=a[A>>2]))&e-16843009&-2139062144))for(i=n(i,16843009);;){if((-1^(e^=i))&e-16843009&-2139062144)break r;if(e=a[A+4>>2],A=A+4|0,e-16843009&(-1^e)&-2139062144)break}for(;(i=B[0|(e=A)])&&(A=e+1|0,(0|i)!=(255&r)););return e}return ci(A)+A|0}return A}function Be(A,r){var e=0,i=0,Q=0,f=0;Q=1;A:if(r){if(ka(A))return MQ(A,r);r:{if(!a[A+12>>2]){for(;;){if((0|(e=a[A+4>>2]))<=(0|i))break r;if(e=r,r=a[A>>2]+(i<<2)|0,e=e+(f=a[r>>2])|0,a[r>>2]=e,r=1,i=i+1|0,!(e>>>0<f>>>0))break}break A}if(a[A+12>>2]=0,Q=pr(A,r),ka(A))break A;return a[A+12>>2]=!a[A+12>>2],Q}if((0|i)==(0|e)){if(!Gf(A,i+1|0))return 0;a[A+4>>2]=a[A+4>>2]+1,a[a[A>>2]+(i<<2)>>2]=r}}return Q}function te(A,r,e){var Q=0,f=0;A:{r:switch(B[0|e]-1|0){case 0:case 5:break r;default:break A}if(!(!(e=a[e+16>>2])|!(1&i[e+4|0]))){f=(A=a[A>>2])+a[e+12>>2]|0,A=A+a[e+8>>2]|0,Q=-1;r:switch(r+1|0){case 1:if(Q=1,a[A>>2]=1,A=uf(),a[f>>2]=A,A)break A;return Ve(13,233,65),-1;case 2:return r=A,A=a[A>>2]+1|0,a[r>>2]=A,A;case 0:break r;default:break A}Q=a[A>>2]-1|0,a[A>>2]=Q,Q||(Yf(a[f>>2]),Q=0,a[f>>2]=0)}}return Q}function be(A){var r=0;A&&(r=a[A+60>>2],a[A+60>>2]=r-1,1!=(0|r)&(0|r)>1||((r=a[A+8>>2])&&(r=a[r+32>>2])&&Ga[0|r](A),zA(9,A,A+56|0),Yf(a[A+92>>2]),xi(a[A+16>>2]),xi(a[A+20>>2]),yi(a[A+24>>2]),yi(a[A+28>>2]),yi(a[A+32>>2]),yi(a[A+36>>2]),yi(a[A+40>>2]),yi(a[A+44>>2]),Ea(a[A+52>>2]),Ti(a[A+48>>2],140),LQ(a[A+84>>2]),LQ(a[A+88>>2]),SQ(a[A+80>>2],31289,0),SQ(A,31289,0)))}function ke(A,r){var e=0,i=0,Q=0;if(a[A+8>>2]<(0|r)){if((0|r)>=16777216)return Ve(3,120,114),0;if(2&(e=a[A+16>>2]))return Ve(3,120,105),0;if(i=r<<2,!(i=8&e&&B[92204]?j(i):Ki(i,31289)))return Ve(3,120,65),0;(0|(e=a[A+4>>2]))>0&&yQ(i,a[A>>2],e<<2),(e=a[A>>2])&&(Q=a[A+8>>2]<<2,8&B[A+16|0]?x(e,Q):pQ(e,Q)),a[A+8>>2]=r,a[A>>2]=i}return A}function ne(A){var r,e=0,i=0,Q=0;if(e=a[A+36>>2],a[A+40>>2]||e)a[A+36>>2]=e+1;else{r=a[A+32>>2];A:{if((0|(e=a[A+24>>2]))==a[A+28>>2]){if(!(i=iQ((e=e?n(e,3)>>>1|0:32)<<2,31289,0)))break A;(Q=a[A+24>>2])&&yQ(i,a[A+20>>2],Q<<2),SQ(a[A+20>>2],31289,0),a[A+28>>2]=e,a[A+20>>2]=i,e=a[A+24>>2]}else i=a[A+20>>2];return a[A+24>>2]=e+1,void(a[(e<<2)+i>>2]=r)}Ve(3,148,65),Ve(3,129,109),a[A+36>>2]=a[A+36>>2]+1}}function ce(A,r,e){var i=0,Q=0,f=0;A:{if(e){if(Q=(i=a[A+20>>2])+(e<<3)|0,a[A+20>>2]=Q,a[A+24>>2]=a[A+24>>2]+(i>>>0>Q>>>0)+(e>>>29),i=a[A+92>>2]){if(f=(Q=A+28|0)+i|0,e+i>>>0<64&e>>>0<=63)break A;yQ(f,r,i=64-i|0),U(A,Q,1),IQ(Q,0,68),e=e-i|0,r=r+i|0}e>>>0>=64&&(U(A,r,e>>>6|0),r=(-64&e)+r|0,e&=63),e&&(a[A+92>>2]=e,yQ(A+28|0,r,e))}return 1}return yQ(f,r,e),a[A+92>>2]=a[A+92>>2]+e,1}function Ce(A,r,e){var i=0,Q=0,f=0;A:{if(e){if(Q=(i=a[A+32>>2])+(e<<3)|0,a[A+32>>2]=Q,a[A+36>>2]=a[A+36>>2]+(i>>>0>Q>>>0)+(e>>>29),i=a[A+104>>2]){if(f=(Q=A+40|0)+i|0,e+i>>>0<64&e>>>0<=63)break A;yQ(f,r,i=64-i|0),F(A,Q,1),IQ(Q,0,68),e=e-i|0,r=r+i|0}e>>>0>=64&&(F(A,r,e>>>6|0),r=(-64&e)+r|0,e&=63),e&&(a[A+104>>2]=e,yQ(A+40|0,r,e))}return}yQ(f,r,e),a[A+104>>2]=a[A+104>>2]+e}function Ee(A,r){var e,Q=0,f=0,B=0;if(s=e=s-16|0,Q=r,f=r>>31,(0|r)>=0){for(r=8;i[(r=r-1|0)+(e+8|0)|0]=Q,B=!f&Q>>>0>255|0!=(0|f),Q=(255&f)<<24|Q>>>8,f=f>>>8|0,B;);Q=2}else{for(Q=0-Q|0,f=0-(f+(0!=(0|r))|0)|0,r=8;i[(r=r-1|0)+(e+8|0)|0]=Q,B=!f&Q>>>0<256,Q=(255&f)<<24|Q>>>8,f=f>>>8|0,!B;);Q=258}return a[A+4>>2]=Q,A=ze(A,(e+8|0)+r|0,8-r|0),s=e+16|0,A}function oe(A,r,e,i,Q){var f,B,t=0,b=0,k=0,n=0;return s=f=s-16|0,a[f+12>>2]=a[e+4>>2],-1!=(0|(B=z(A,0,f+12|0,e)))&&(k=a[f+12>>2],i=-1==(0|i)?k:i,b=(t=-2==(0|B))?0:B,t<<=1,r&&(1<<(n=k+3|0)&1572865&&n>>>0<=20||jA(r,t,b,i,Q),z(A,a[r>>2],f+12|0,e),-2!=(0|B)?a[r>>2]=a[r>>2]+b:bf(r)),1<<(A=k+3|0)&1572865&&A>>>0<=20||(b=Ke(t,b,i))),s=f+16|0,b}function le(A,r,e){var Q=0,f=0,B=0,t=0;A:if(!(f=a[A>>2])){if(!(Q=iQ(76,31289,0)))return Ve(3,149,65),0;if(a[Q>>2]=0,rf(f=Q+4|0),rf(B=Q+24|0),rf(t=Q+44|0),a[Q+72>>2]=1,a[Q+64>>2]=0,a[Q+68>>2]=0,!Br(Q,r,e)){if(yi(f),yi(B),yi(t),f=0,!(1&i[Q+72|0]))break A;return SQ(Q,31289,0),0}a[A>>2]?(yi(f),yi(B),yi(t),1&i[Q+72|0]&&SQ(Q,31289,0),A=a[A>>2]):(a[A>>2]=Q,A=Q),f=A}return f}function we(A,r,e){var i=0,Q=0,f=0,a=0;return Q=31&(f=a=63&e),f=f>>>0>=32?-1>>>Q|0:(i=-1>>>Q|0)|(1<<Q)-1<<32-Q,f&=A,i&=r,Q=31&a,a>>>0>=32?(i=f<<Q,a=0):(i=(1<<Q)-1&f>>>32-Q|i<<Q,a=f<<Q),f=i,i=31&(Q=0-e&63),Q>>>0>=32?(i=-1<<i,e=0):i=(e=-1<<i)|(1<<i)-1&-1>>>32-i,A&=e,r&=i,i=31&Q,Q>>>0>=32?(e=0,A=r>>>i|0):(e=r>>>i|0,A=((1<<i)-1&r)<<32-i|A>>>i),N=e|f,A|=a}function ge(A,r,e){var i,Q=0;s=i=s-16|0,Q=0;A:if(A){a[i+12>>2]=0,a[A+24>>2]&&(Q=a[A+12>>2])&&(Q=a[Q+88>>2])&&(Ga[0|Q](A),a[A+24>>2]=0);r:{if(!a[A+12>>2]||a[A+4>>2]!=(0|r)){if(!(Q=Je(i+12|0,r)))break r;a[A+12>>2]=Q,Q=a[Q>>2],a[A+4>>2]=r,a[A>>2]=Q,a[A+16>>2]=a[i+12>>2]}a[A+24>>2]=e,Q=0!=(0|e);break A}Ve(6,158,156),Q=0}return s=i+16|0,Q}function ue(A){var r=0,e=0,i=0;if(!(!(r=a[A>>2])|!a[r+12>>2])){if(!(r=iQ(40,31289,0)))return Ve(6,156,65),0;if(a[r>>2]=a[A>>2],(e=a[A+8>>2])?(_f(e),e=a[A+8>>2]):e=0,a[r+8>>2]=e,(e=a[A+12>>2])&&(_f(e),i=a[A+12>>2]),a[r+20>>2]=0,a[r+24>>2]=0,a[r+12>>2]=i,a[r+16>>2]=a[A+16>>2],(0|Ga[a[a[A>>2]+12>>2]](r,A))>0)return r;a[r>>2]=0,Ii(a[r+8>>2]),Ii(a[r+12>>2]),SQ(r,31289,0)}return 0}function Me(A,r,e,i){var Q=0,f=0,B=0,t=0;if(!i||!(B=TA(i)))return Ve(6,168,152),0;i=-2,f=147;A:{r:if(A&&(Q=a[A>>2])&&(t=a[Q+100>>2])){if(!a[Q+128>>2]){if(!(Q=a[A+16>>2])){i=-1,f=149;break r}if(i=-1,-1!=(0|r)&&(f=148,!(r&Q)))break r}if(i=-2,f=147,-2!=(0|(A=0|Ga[0|t](A,e,0,B))))break A}Ve(6,137,f),A=i}return A}function se(A,r,e){var i=0;if(!(e&&(i=a[e>>2])||((i=Ki(16,31289))?a[i+4>>2]=4:Ve(13,130,65),i)))return Ve(13,198,65),0;SQ(a[i+8>>2],31289,0),a[i+8>>2]=0,A=xe(A,i+8|0,r),a[i>>2]=A;A:{if(A){if(a[i+8>>2]){a[e>>2]|!e||(a[e>>2]=i),r=i;break A}r=65}else r=112;if(Ve(13,198,r),!e||(r=0,!a[e>>2]))return wQ(i),0}return r}function De(A){var r=0,e=0;if(A){r=a[A+48>>2],a[A+48>>2]=r-1;A:if(!(1!=(0|r)&(0|r)>1)){r=a[A+8>>2];r:{e:{i:{Q:{if(!(e=a[A+4>>2])){if(r)break Q;break r}if(!r)break i}r=0|Ga[0|r](A,1,0,0,0,0,1,0);break e}r=0|Ga[0|e](A,1,0,0,0,1)}if((0|r)<=0)break A}(r=a[A>>2])&&(r=a[r+40>>2])&&Ga[0|r](A),zA(12,A,A+72|0),Yf(a[A+76>>2]),SQ(A,31289,0)}}}function Ie(A,r,e,i,Q,f){var B=0,t=0,b=0,k=0,n=0;B=-2,t=147;A:{r:if(A&&(b=a[A>>2])&&(n=a[b+100>>2])){if(k=-1,-1!=(0|r)&a[b>>2]!=(0|r))break A;if(!a[b+128>>2]){if(!(r=a[A+16>>2])){B=-1,t=149;break r}if(B=-1,-1!=(0|e)&&(t=148,!(r&e)))break r}if(B=-2,t=147,-2!=(0|(k=0|Ga[0|n](A,i,Q,f))))break A}Ve(6,137,t),k=B}return k}function Ne(A,r){var e=0,i=0,Q=0,f=0,B=0;A:{r:{if((0|A)!=(0|r)){if(a[A+12>>2]=a[r+12>>2],Gf(A,a[r+4>>2]+1|0))break r;return 0}if(Gf(A,a[A+4>>2]+1|0))break A;return 0}a[A+4>>2]=a[r+4>>2]}if(i=a[A>>2],a[r+4>>2]>0)for(Q=a[r>>2];B=e,e=a[Q>>2],a[i>>2]=B|e<<1,i=i+4|0,Q=Q+4|0,e=e>>>31|0,(0|(f=f+1|0))<a[r+4>>2];);return a[i>>2]=e,a[A+4>>2]=a[A+4>>2]+e,1}function Ge(A){A|=0;var r=0,e=0,i=0;if(A=a[22981]){if(a[22982]){for(;;){if(r=a[22981]+n(e,12)|0,SQ(a[r>>2],31289,0),a[r+8>>2])for(A=0;SQ(a[(i=A<<3)+a[r+4>>2]>>2],31289,0),SQ(a[4+(a[r+4>>2]+i|0)>>2],31289,0),(A=A+1|0)>>>0<b[r+8>>2];);if(SQ(a[r+4>>2],31289,0),!((e=e+1|0)>>>0<b[22982]))break}A=a[22981]}SQ(A,31289,0),a[22982]=0,a[22981]=0}}function Ue(A,r){var e=0,i=0,Q=0;A:{r:{e:{if(!((e=a[23058])>>>0>A>>>0|e+(a[23059]<<2)>>>0<=A>>>0)){if((i=a[23056])>>>0>r>>>0)break e;if((Q=a[23057]+i|0)>>>0<=r>>>0)break e;if(e=a[A>>2],a[r>>2]=e,!e)return a[r+4>>2]=A,void(a[A>>2]=r);if(e>>>0<i>>>0|e>>>0>=Q>>>0)break r;if(a[r+4>>2]=A,a[e+4>>2]!=(0|A))break A;return a[e+4>>2]=r,void(a[A>>2]=r)}Ia(),E()}Ia(),E()}Ia(),E()}Ia(),E()}function Fe(A){var r=0;if(A&&(r=a[A+8>>2],a[A+8>>2]=r-1,!(1!=(0|r)&(0|r)>1))){if(!(4&B[A+12|0])&&(r=a[a[A>>2]+8>>2])&&!(0|Ga[0|r](A)))return void Ve(37,111,107);if((r=a[a[A>>2]+32>>2])&&!(0|Ga[0|r](A)))return void Ve(37,111,102);Df(a[A+4>>2]),SQ(a[A+28>>2],31289,0),SQ(a[A+32>>2],31289,0),Yf(a[A+36>>2]),SQ(A,31289,0)}}function Ze(A,r){var e,Q=0,f=0,t=0,b=0;if(e=iQ(ci(A)>>>1|0,31289,0)){for(f=e;t=A,Q=B[0|A];)if(A=t+1|0,58!=(0|Q)){if(!(A=B[t+1|0]))return Ve(15,118,103),SQ(e,31289,0),0;if(((b=jQ(A))|(Q=jQ(Q)))<0)return SQ(e,31289,0),Ve(15,118,102),0;A=t+2|0,i[0|f]=Q<<4|b,f=f+1|0}return r&&(a[r>>2]=f-e),e}return Ve(15,118,65),0}function de(A,r){var e=0,i=0;i=1;A:if(A){e=141,A=a[A>>2];r:{e:switch(r-3|0){case 2:if(-1!=(0|DQ(A)))break A;e=142;break r;case 0:break r;default:break e}e=157;e:{if((0|A)<=256){if((r=A-95|0)>>>0>22|!(1<<r&4718593))break e;break A}if(A-672>>>0<4|A-1096>>>0<4|257==(0|A))break A;break r}if(64==(0|A)|A-3>>>0<2)break A}i=0,Ve(4,140,e)}return i}function We(A,r,e,i,Q){var f=0,t=0;if(!(A&&(t=a[A>>2])&&(f=a[t+40>>2])))return Ve(6,140,150),-2;if(8!=a[A+16>>2])return Ve(6,140,151),-1;A:{if(2&B[t+4|0]){if(!(f=sQ(a[A+8>>2])))return Ve(6,140,163),0;if(!r)return a[e>>2]=f,1;if(f>>>0>b[e>>2])break A;f=a[a[A>>2]+40>>2]}return 0|Ga[0|f](A,r,e,i,Q)}return Ve(6,140,155),0}function Ve(A,r,e){var Q,f=0,B=0,t=0;(Q=Ur())&&(f=(a[Q+384>>2]+1|0)%16|0,a[Q+384>>2]=f,a[Q+388>>2]==(0|f)&&(a[Q+388>>2]=(16777216+(f<<24)>>24)%16),a[(B=(t=f<<2)+Q|0)>>2]=0,a[B+256>>2]=0,a[B+320>>2]=0,a[B- -64>>2]=4095&e|r<<12&16773120|A<<24,1&i[B+192|0]&&(SQ(a[(A=Q+128|0)+t>>2],31289,0),f=a[Q+384>>2],a[A+(f<<2)>>2]=0),a[192+((f<<2)+Q|0)>>2]=0)}function Ye(A,r){var e,i=0,Q=0,f=0,B=0;if(!r||!A)return A?-1:0!=(0|r);f=(e=a[A+12>>2])?-1:1;A:if(a[r+12>>2]==(0|e)&&!((0|(Q=a[A+4>>2]))>(0|(i=a[r+4>>2])))){if((0|i)<=(0|Q))for(;;){if((0|Q)<=0)return 0;if((B=a[(i=(Q=Q-1|0)<<2)+a[A>>2]>>2])>>>0>(i=a[i+a[r>>2]>>2])>>>0)break A;if(!(i>>>0<=B>>>0))break}f=e?1:-1}return f}function Re(A,r,e,i,Q){var f,B=0;return s=f=s-48|0,a[f+4>>2]=0,a[f+40>>2]=f+32,e=Pe(e),a[f+32>>2]=e,e?e&&a[e+12>>2]?(a[f+24>>2]=5,a[f+28>>2]=0,a[f+16>>2]=i,a[f+8>>2]=Q,a[f+36>>2]=f+24,a[f+44>>2]=f+8,(0|(e=xe(f+40|0,f+4|0,32904)))<0||(a[A>>2]=a[f+4>>2],a[r>>2]=e,B=1)):Ve(4,146,116):Ve(4,146,117),s=f+48|0,B}function ye(A,r,e,i,Q){var f,B=0,t=0,b=0,k=0,n=0,c=0;s=f=s-240|0,a[f>>2]=A,b=1;A:if(!((0|i)<2))for(n=0-r|0,B=A;;){if(t=(B=B+n|0)-a[((k=i-2|0)<<2)+Q>>2]|0,(0|Ga[0|e](A,t))>=0&&(0|Ga[0|e](A,B))>=0)break A;if(c=t,B=(t=(0|Ga[0|e](t,B))>=0)?c:B,a[(b<<2)+f>>2]=B,b=b+1|0,!((0|(i=t?i-1|0:k))>1))break}Te(r,f,b),s=f+240|0}function he(A,r,e,i){var Q=0,f=0;A:{if(Q=Ki(24,31289)){if(Kf(Q,1),f=0,Vr(Q,i)&&(!r||(f=0,0|Ga[a[Q+20>>2]](Q,A,r)))){if(A=a[Q>>2],a[A+8>>2]>=65)break A;A=0|Ga[a[A+24>>2]](Q,e),f=a[Q>>2],(r=a[f+32>>2])&&(Ga[0|r](Q),Kf(Q,2),f=a[Q>>2]),Of(a[Q+12>>2],a[f+40>>2]),f=0!=(0|A)}Qi(Q),SQ(Q,31289,0)}return f}Ia(),E()}function Je(A,r){var e;s=e=s-160|0;A:{for(;;){r:{if(a[e+16>>2]=r,a[e+12>>2]=e+16,!(r=a[22972])||(0|(r=nr(r,e+16|0)))<0){if(!(r=Yi(e+12|0,90784,5,4,4)))break r;r=a[r>>2]}else r=UQ(a[22972],r);if(r){if(!(1&i[r+8|0]))break A;r=a[r+4>>2];continue}}break}r=0}return A&&(a[A>>2]=0),s=e+160|0,r}function Te(A,r,e){var i,Q=0,f=0,B=0,t=0;if(s=i=s-256|0,!((0|e)<2)&&(a[(t=(e<<2)+r|0)>>2]=i,A))for(;;){for(f=A>>>0<256?A:256,yQ(a[t>>2],a[r>>2],f),Q=0;B=(Q<<2)+r|0,Q=Q+1|0,yQ(a[B>>2],a[(Q<<2)+r>>2],f),a[B>>2]=a[B>>2]+f,(0|e)!=(0|Q););if(!(A=A-f|0))break}s=i+256|0}function me(A,r){var e,i,Q=0,f=0,B=0;if(!r)return-1;if((0|(Q=a[A+4>>2]))<=0)return 0;if(i=1&Q,e=a[A>>2],1==(0|Q))A=0;else{for(B=-2&Q,A=0;f=Sf(0|a[(e+(Q<<2)|0)-4>>2],f,r),f=Sf(0|a[e+((Q=Q-2|0)<<2)>>2],f,r),(0|B)!=(0|(A=A+2|0)););B=0,A=f}return i&&(f=Sf(a[(e+(Q<<2)|0)-4>>2]|B,A,r)),f}function Le(){var A=0,r=0;if(A=Af(91880),_Q(91880,0),A&&((r=a[A+240>>2])&&Ga[a[r+12>>2]](A),Yf(a[A>>2]),zA(15,A,A+96|0),a[A+8>>2]?x(A,260):pQ(A,260)),A=Af(91876),_Q(91876,0),A){if((r=a[A+240>>2])&&Ga[a[r+12>>2]](A),Yf(a[A>>2]),zA(15,A,A+96|0),a[A+8>>2])return void x(A,260);pQ(A,260)}}function Xe(A,r,e){var i,Q;return s=Q=s-16|0,a[Q+12>>2]=e,s=i=s-2080|0,a[i+12>>2]=i+16,a[i+8>>2]=2048,a[i+4>>2]=0,e=h(i+12|0,i+4|0,i+8|0,i+2076|0,i,r,e),r=a[i+4>>2],e?(e=a[i+2076>>2],r?(A=Ar(A,r,e),SQ(a[i+4>>2],31289,0)):A=Ar(A,i+16|0,e)):(SQ(r,31289,0),A=-1),s=i+2080|0,s=Q+16|0,A}function Se(A,r){var e=0,i=0,Q=0,f=0;A:{r:{if((i=kf())&&(e=iQ(12,31289,0))){if(Q=iQ(f=ci(r)+1|0,31289,0),a[e>>2]=Q,!Q)break r;if(yQ(Q,r,f),a[e+8>>2]=i,a[e+4>>2]=0,uA(a[A+8>>2],e)|a[a[A+8>>2]+92>>2]>0)break r;return e}Df(i),e=0;break A}Df(i),SQ(a[e>>2],31289,0)}return SQ(e,31289,0),0}function He(A,r,e){var i,Q=0;s=i=s-16|0;A:if(e)if(A){if(r){if(a[i>>2]=r,a[i+4>>2]=e,Q=Kr(a[A+8>>2],i)){Q=a[Q+8>>2];break A}if(!mi(r,19438)&&(Q=ei(e)))break A}if(a[i+4>>2]=e,a[i>>2]=2905,Q=0,!(A=Kr(a[A+8>>2],i)))break A;Q=a[A+8>>2]}else Q=ei(e);return s=i+16|0,Q}function pe(A){var r,e=0,i=0;s=r=s-32|0;A:if(A>>>0<=1194){if(!(a[n(A,24)+38488>>2]|!A)){Ve(8,104,101),e=0;break A}e=a[n(A,24)+38480>>2]}else e=0,(i=a[23070])&&(a[r+24>>2]=3,a[r+8>>2]=A,a[r+28>>2]=r,(A=Kr(i,r+24|0))?e=a[a[A+4>>2]>>2]:(Ve(8,104,101),e=0));return s=r+32|0,A=e}function ve(A,r){var e,Q,f,B=0,t=0;if(e=((B=Dr(A))+7|0)/8|0,!(t=a[A+8>>2]))return Of(r,e),e;if(B+14>>>0<15)return 0;for(Q=1-(t<<2)|0,f=0-(a[A+4>>2]<<2)|0,t=r+e|0,B=0,r=0;i[0|(t=t-1|0)]=B+f>>31&a[a[A>>2]+(-4&r)>>2]>>>(r<<3),r=(r+Q>>>31|0)+r|0,(0|e)!=(0|(B=B+1|0)););return e}function ze(A,r,e){var Q=0,f=0;if((0|e)<0){if(!r)return 0;e=ci(r)}if(e>>>0>=2147483647)return Ve(13,0,223),0;Q=a[A+8>>2];A:{if(e>>>0<b[A>>2]){if(Q)break A;Q=0}if(f=Q,Q=K(Q,e+1|0,31289,0),a[A+8>>2]=Q,!Q)return Ve(13,186,65),a[A+8>>2]=f,0}return a[A>>2]=e,r&&(yQ(Q,r,e),i[a[A+8>>2]+e|0]=0),1}function je(A){var r,e,i,Q;return A?(r=Ki(16,31289))?(a[r+4>>2]=4,a[r+4>>2]=a[A+4>>2],Q=ze(r,a[A+8>>2],a[A>>2]),e=128&(i=a[r+12>>2]),Q?(a[r+12>>2]=e,a[r+12>>2]=-129&a[A+12>>2]|e,r):(16&i||SQ(a[r+8>>2],31289,0),e||SQ(r,31289,0),0)):(Ve(13,130,65),0):0}function xe(A,r,e){var i,Q=0;return s=i=s-16|0,a[i+12>>2]=A,r?a[r>>2]?A=QA(i+12|0,r,e,-1,0):(0|(A=QA(i+12|0,0,e,-1,0)))<=0||((Q=iQ(A,31289,0))?(a[i+8>>2]=Q,QA(i+12|0,i+8|0,e,-1,0),a[r>>2]=Q):(Ve(13,118,65),A=-1)):A=QA(i+12|0,0,e,-1,0),s=i+16|0,A}function Ke(A,r,e){var i=0,Q=0;if((0|r)<0)return-1;if(i=1,(0|e)>=31)for(;i=i+1|0,Q=(0|e)>127,e>>=7,Q;);if(2!=(0|A)){if(i=i+1|0,!((0|r)<128))for(e=r;i=i+1|0,A=e>>>0>255,e=e>>>8|0,A;);}else i=i+3|0;return(2147483647-r|0)>(0|i)?r+i|0:-1}function Oe(A,r,e){var Q=0;A:{r:{e:{if(!((0|r)<0|a[23059]<=(0|r))){if((A=A-a[23056]|0)&(Q=a[23057]>>>r|0)-1)break e;if(!(A=((A>>>0)/(Q>>>0)|0)+(1<<r)|0)|A>>>0>=b[23063])break r;if(!((r=1<<(7&A))&(e=B[0|(A=(A>>>3|0)+e|0)])))break A;return void(i[0|A]=e&(-1^r))}Ia(),E()}Ia(),E()}Ia(),E()}Ia(),E()}function Pe(A){var r,e=0,i=0;s=r=s-32|0;A:if(A>>>0<=1194){if(!(a[n(A,24)+38488>>2]|!A)){Ve(8,103,101);break A}e=n(A,24)+38480|0}else(i=a[23070])?(a[r+24>>2]=3,a[r+8>>2]=A,a[r+28>>2]=r,(A=Kr(i,r+24|0))?e=a[A+4>>2]:Ve(8,103,101)):Ve(8,103,101);return s=r+32|0,e}function qe(A){for(var r=0,e=0,Q=0,f=0;A=(r=A)+1|0,pf(i[0|r]););A:{r:{e:switch((e=i[0|r])-43|0){case 0:break r;case 2:break e;default:break A}f=1}e=i[0|A],r=A}if(aa(e))for(;Q=48+(n(Q,10)-i[0|r]|0)|0,A=i[r+1|0],r=r+1|0,aa(A););return f?Q:0-Q|0}function _e(A,r,e){var Q=0;A:{r:{e:{if(!((0|r)<0|a[23059]<=(0|r))){if((A=A-a[23056]|0)&(Q=a[23057]>>>r|0)-1)break e;if(!(A=((A>>>0)/(Q>>>0)|0)+(1<<r)|0)|A>>>0>=b[23063])break r;if((r=1<<(7&A))&(e=B[0|(A=(A>>>3|0)+e|0)]))break A;return void(i[0|A]=r|e)}Ia(),E()}Ia(),E()}Ia(),E()}Ia(),E()}function $e(A,r){var e=0,Q=0,f=0;for(e=r;;){if(f=255&(Q=i[0|e]),!((0|Q)<0)&&(Q=a[A+4>>2],8&B[Q+(f<<1)|0])){for(;A=e,!((e=e-1|0)>>>0<r>>>0||(0|(f=i[0|e]))<0)&&16&B[Q+((255&f)<<1)|0];);return void(i[0|A]=0)}e=e+1|0}}function Ai(A,r,e,i){var Q=0;if(a[22978]||(Q=kf(),a[22978]=Q,Q)){if(!(Q=Ki(24,31289)))return Ve(14,122,65),0;if(a[Q>>2]=A,A=Pr(r),a[Q+12>>2]=i,a[Q+8>>2]=e,a[Q+4>>2]=A,!A)return SQ(Q,31289,0),0;if(ir(a[22978],Q))return Q;SQ(a[Q+4>>2],31289,0),SQ(Q,31289,0)}return 0}function ri(A,r){var e=0,Q=0;A:if(!((0|(e=i[0|r]))<0))for(A=a[A+4>>2];;){r:if(32&(e=t[A+((255&e)<<1)>>1])){if(Q=r+1|0,!((0|(e=i[r+1|0]))<0|!(8&t[A+((255&e)<<1)>>1])))break r;Q=r+2|0}else{if(!(775&e))break A;Q=r+1|0}if(!((0|(e=i[0|(r=Q)]))>=0))break}return r}function ei(A){var r=0,e=0,i=0,Q=0;if((0|(r=ae(A,61)))!=(0|A))A:if(!B[(i=r-A|0)+A|0]&&(e=a[23333])&&(r=a[e>>2])){for(;;){if(si(A,r,i)||(r=a[e>>2]+i|0,61!=B[0|r])){if(r=a[e+4>>2],e=e+4|0,r)continue;break A}break}Q=r+1|0}return Q}function ii(A,r){var e,i,Q=0,f=0;if(s=e=s-16|0,(i=4096&(Q=a[r>>2]))&&(a[e+12>>2]=A,A=e+12|0),6&Q){if((0|Vf(Q=a[A>>2]))>0)for(;a[e+8>>2]=UQ(Q,f),dA(e+8|0,a[r+16>>2],i),f=f+1|0,(0|Vf(Q))>(0|f););Df(Q),a[A>>2]=0}else dA(A,a[r+16>>2],i);s=e+16|0}function Qi(A){var r=0;A&&((r=a[A>>2])&&(a[r+32>>2]&&($f(A,2)||Ga[a[a[A>>2]+32>>2]](A)),!a[A+12>>2]|a[a[A>>2]+40>>2]<=0||$f(A,4)||(pQ(a[A+12>>2],a[a[A>>2]+40>>2]),a[A+12>>2]=0)),$f(A,1024)||bQ(a[A+16>>2]),Of(A,24))}function fi(){var A=0,r=0,e=0;if((0|Vf(a[22979]))>0)for(;(A=NQ(a[22979]))&&(r=a[A>>2],(e=a[r+12>>2])&&(Ga[0|e](A),r=a[A>>2]),a[r+16>>2]=a[r+16>>2]-1,SQ(a[A+4>>2],31289,0),SQ(a[A+8>>2],31289,0),SQ(A,31289,0)),(0|Vf(a[22979]))>0;);Df(a[22979]),a[22979]=0}function ai(A,r){var e=0,i=0,Q=0,f=0;A:if(!((0|r)<0)){if((0|(e=a[A+4>>2]))<=(0|(i=r>>>5|0))){if(Q=i+1|0,a[A+8>>2]<=(0|i)){if(!ke(A,Q))break A;e=a[A+4>>2]}(0|e)<=(0|i)&&IQ(a[A>>2]+(e<<2)|0,0,Q-e<<2),a[A+4>>2]=Q}f=1,A=a[A>>2]+(i<<2)|0,a[A>>2]=a[A>>2]|1<<r}return f}function Bi(A){var r=0,e=0,i=0,Q=0;if(!(A|=0))return 0;if(r=B[0|A]){for(Q=256;i=(i<<(e=15&(r^(r=r<<24>>24)>>>2))|((63&(e=32-e|0))>>>0>=32?0:i>>>e|0))^n(r|=Q,r),Q=Q+256|0,r=B[A+1|0],A=A+1|0,r;);A=i>>>16^i}else A=0;return 0|A}function ti(){var A=0,r=0;if(r=Af(91900)){if((A=a[r>>2])&&(A=NQ(A)))for(;SQ(a[A+8>>2],31289,0),SQ(A,31289,0),A=NQ(a[r>>2]););Df(a[r>>2]),SQ(r,31289,0),_Q(91900,0)}r=Af(91896),_Q(91896,0)&&SQ(r,31289,0)}function bi(){var A,r=0,e=0;if(A=Ur()){for(;1&i[(r=(e<<2)+A|0)+192|0]&&(SQ(a[r+128>>2],31289,0),a[r+128>>2]=0),a[r+192>>2]=0,a[r>>2]=0,a[r- -64>>2]=0,a[r+256>>2]=0,a[r+320>>2]=-1,16!=(0|(e=e+1|0)););a[A+384>>2]=0,a[A+388>>2]=0}}function ki(A,r,e,i,Q){var f,B=0;ne(Q);A:if(f=sA(Q)){r:{if((0|r)==(0|e)){if(Jf(f,r,Q))break r;break A}if(!df(f,r,e,Q))break A}if(H(0,A,f,i,Q)){if(B=1,!a[A+12>>2])break A;if(0|Ga[0|(a[i+12>>2]?26:27)](A,A,i))break A}B=0}return Yr(Q),B}function ni(A,r){var e=0,i=0;return(0|A)==(0|r)?r:(0|(e=a[(4&B[r+16|0]?8:4)+r>>2]))<=a[A+8>>2]||ke(A,e)?((0|(i=a[r+4>>2]))>0&&(yQ(a[A>>2],a[r>>2],e<<2),i=a[r+4>>2]),r=a[r+12>>2],a[A+4>>2]=i,a[A+12>>2]=r,A):0}function ci(A){var r=0,e=0,i=0;A:{if(3&(r=A))for(;;){if(!B[0|r])break A;if(!(3&(r=r+1|0)))break}for(;e=r,r=r+4|0,!((-1^(i=a[e>>2]))&i-16843009&-2139062144););for(;e=(r=e)+1|0,B[0|r];);}return r-A|0}function Ci(A,r,e){var i,Q=0;return s=i=s-80|0,a[A>>2]&&Ri(a[A+4>>2],i,i+76|0)&&ur(a[A+4>>2],a[A+12>>2])&&nf(a[A+4>>2],i,a[i+76>>2])&&(Q=1,Ri(a[A+4>>2],r,e))||(Q=0),s=i+80|0,A=Q}function Ei(A,r,e,i){var Q=0,f=0;if(A){if(-1==(0|e))return oi(a[A>>2]),a[A>>2]=r,oa(a[A+4>>2]),a[A+4>>2]=0,1;(a[A+4>>2]||(Q=HQ(32672),a[A+4>>2]=Q,Q))&&(oi(a[A>>2]),a[A>>2]=r,f=1,e&&gi(a[A+4>>2],e,i))}return f}function oi(A){var r=0;A&&(4&(r=a[A+20>>2])&&(SQ(a[A>>2],31289,0),SQ(a[A+4>>2],31289,0),a[A>>2]=0,a[A+4>>2]=0,r=a[A+20>>2]),8&r&&(SQ(a[A+16>>2],31289,0),a[A+12>>2]=0,a[A+16>>2]=0,r=a[A+20>>2]),1&r&&SQ(A,31289,0))}function li(A){var r=0,e=0,i=0;if(A){if(r=a[A+12>>2])for(;;){if(e=a[a[A>>2]+(i<<2)>>2]){for(;r=a[e+4>>2],SQ(e,31289,0),e=r;);r=a[A+12>>2]}if(!((i=i+1|0)>>>0<r>>>0))break}SQ(a[A>>2],31289,0),SQ(A,31289,0)}}function wi(A,r){var e=0,i=0;if(e=a[r+12>>2],r=Pe(a[r>>2]),A){if(e&=8)return oi(a[A>>2]),a[A>>2]=r,oa(a[A+4>>2]),void(a[A+4>>2]=0);(a[A+4>>2]||(i=HQ(32672),a[A+4>>2]=i,i))&&(oi(a[A>>2]),a[A>>2]=r,gi(a[A+4>>2],e?-1:5,0))}}function gi(A,r,e){var i;s=i=s-16|0,a[i+12>>2]=A;A:{r:switch(a[A>>2]-1|0){case 0:case 4:break A;default:break r}a[A+4>>2]&&(wr(i+12|0,0,0),A=a[i+12>>2])}a[A>>2]=r,a[A+4>>2]=1!=(0|r)?e:e?255:0,s=i+16|0}function ui(A,r){var e=0,i=0;A:{r:switch((r=r||16)-8|0){case 0:case 8:break r;default:break A}e=(e=a[A+44>>2])||16,a[A+44>>2]=e,i=1,(0|r)!=(0|e)&&(a[A+44>>2]=r,r=a[A+20>>2],a[A+16>>2]=238^a[A+16>>2],a[A+20>>2]=r)}return i}function Mi(A,r){var e,i=0;if(e=Ki(96,31289)){if(i=Ki(64,31289),a[e>>2]=i,i)return a[e+32>>2]=256,a[e+24>>2]=8,a[e+28>>2]=512,a[e+12>>2]=8,a[e+16>>2]=16,a[e+8>>2]=A||102,a[e+4>>2]=r||103,e;SQ(0,31289,0),SQ(e,31289,0)}return 0}function si(A,r,e){var i=0,Q=0,f=0;if(!e)return 0;A:if(i=B[0|A]){for(;;){if((Q=B[0|r])&&!(!(e=e-1|0)|(0|i)!=(0|Q))){if(r=r+1|0,i=B[A+1|0],A=A+1|0,i)continue;break A}break}f=i}return(255&f)-B[0|r]|0}function Di(A,r,e){var i=0,Q=0,f=0;if(A&&!((0|(i=a[A+12>>2]-1|0))<0))for(;;){if(Q=a[a[A>>2]+(i<<2)>>2])for(;f=a[Q+4>>2],Ga[0|r](a[Q>>2],e),Q=f;);if(!((0|(i=i-1|0))>=0))break}}function Ii(A){var r=0;A&&(r=a[A+8>>2],a[A+8>>2]=r-1,1!=(0|r)&(0|r)>1||((r=a[A+12>>2])&&(r=a[r+88>>2])&&(Ga[0|r](A),a[A+24>>2]=0),Yf(a[A+36>>2]),Ti(a[A+32>>2],55),SQ(A,31289,0)))}function Ni(A,r){var e,i=0;return A?r?(s=e=s-16|0,i=0,r&&A&&(a[e>>2]=r,a[e+4>>2]=0,i=0,(A=Kr(a[A+8>>2],e))&&(i=a[A+8>>2])),s=e+16|0,A=i):(Ve(14,108,107),0):(Ve(14,108,105),0)}function Gi(A,r,e,i,Q){var f=0,B=0;if((a[e+4>>2]+a[r+4>>2]|0)<=a[i+28>>2]<<1){ne(Q);A:if(f=sA(Q)){r:{if((0|r)==(0|e)){if(Zr(f,r,Q))break r;break A}if(!xA(f,r,e,Q))break A}B=0!=(0|cA(A,f,i))}Yr(Q)}return B}function Ui(A){IQ(A+32|0,0,76),a[A+108>>2]=32,a[A+24>>2]=528734635,a[A+28>>2]=1541459225,a[A+16>>2]=1359893119,a[A+20>>2]=-1694144372,a[A+8>>2]=1013904242,a[A+12>>2]=-1521486534,a[A>>2]=1779033703,a[A+4>>2]=-1150833019}function Fi(A,r){var e=0,i=0,Q=0;if(!(i=(e=a[A+4>>2])-a[r+4>>2]|0)){for(Q=a[r>>2],i=a[A>>2];;){if((0|e)<=0)return 0;if((0|(r=a[(A=(e=e-1|0)<<2)+i>>2]))!=(0|(A=a[A+Q>>2])))break}i=A>>>0<r>>>0?1:-1}return i}function Zi(A,r,e,i){var Q,f,a,B,t=0,b=0;return B=n(t=e>>>16|0,b=A>>>16|0),t=(65535&(b=((a=n(Q=65535&e,f=65535&A))>>>16|0)+n(b,Q)|0))+n(t,f)|0,N=(n(r,e)+B|0)+n(A,i)+(b>>>16)+(t>>>16)|0,65535&a|t<<16}function di(A,r,e){var i=0;return(i=a[A+84>>2])||(i=or(A,e),a[A+84>>2]=i,i)?a[i+16>>2]?(a[r>>2]=0,(i=a[A+88>>2])||(i=or(A,e),a[A+88>>2]=i)):a[r>>2]=1:i=0,i}function Wi(A,r){var e=0,i=0;if(e=a[A+4>>2]){if(!r)return xf(A),1;if(r=XA(i=a[A>>2],i,e,r)){if(!Gf(A,a[A+4>>2]+1|0))return 0;e=a[A+4>>2],a[A+4>>2]=e+1,a[a[A>>2]+(e<<2)>>2]=r}}return 1}function Vi(A){var r=0,e=0;if(r=a[23068]){if(a[23069]=A,e=a[r+32>>2],ca(a[23068],0),Ae(a[23068],111),r=a[23068],(0|A)<0)return li(r),Ti(a[23067],112),Yf(a[23066]),a[23067]=0,a[23068]=0,void(a[23066]=0);ca(r,e)}}function Yi(A,r,e,i,Q){var f=0,a=0,B=0,t=0;A:{if((0|e)>0)for(;;){if(a=n(f=(e+B|0)/2|0,i)+r|0,(0|(t=0|Ga[0|Q](A,a)))<0)e=f;else{if(!t)break A;B=f+1|0}if(!((0|e)>(0|B)))break}a=0}return a}function Ri(A,r,e){var i=0;if(i=a[A>>2],a[i+8>>2]<65)return i=0|Ga[a[i+24>>2]](A,r),r=a[A>>2],e&&(a[e>>2]=a[r+8>>2]),(e=a[r+32>>2])&&(Ga[0|e](A),Kf(A,2),r=a[A>>2]),Of(a[A+12>>2],a[r+40>>2]),i;Ia(),E()}function yi(A){var r=0,e=0,Q=0;A&&((r=a[A>>2])&&(2&(e=a[A+16>>2])||(Q=a[A+8>>2]<<2,8&e?x(r,Q):pQ(r,Q))),1&i[A+16|0]&&(Of(A,20),SQ(A,31289,0)))}function hi(A,r,e,i){var Q,f=0;return s=Q=s-16|0,a[Q+12>>2]=0,i||(f=i=Pi(),i)?(i=L(A,r,e,i,Q+12|0),a[Q+12>>2]&&Ve(3,110,108),Gr(f)):(i=0,Ve(3,110,65)),s=Q+16|0,i}function Ji(A,r,e){var i,Q;return s=i=s-16|0,(Q=He(A,r,e))||(A?(Ve(14,109,108),a[i+12>>2]=e,a[i+8>>2]=24394,a[i+4>>2]=r,a[i>>2]=24353,br(4,i)):Ve(14,109,106)),s=i+16|0,Q}function Ti(A,r){var e=0,i=0,Q=0;if(A){if((0|(e=a[A>>2]))>0)for(;(Q=a[a[A+4>>2]+(i<<2)>>2])&&(Ga[0|r](Q),e=a[A>>2]),(0|e)>(0|(i=i+1|0)););SQ(a[A+4>>2],31289,0),SQ(A,31289,0)}}function mi(A,r){r|=0;var e=0,i=0;A:if(!(!(e=B[0|(A|=0)])|(0|(i=B[0|r]))!=(0|e)))for(;;){if(i=B[r+1|0],!(e=B[A+1|0]))break A;if(r=r+1|0,A=A+1|0,(0|e)!=(0|i))break}return e-i|0}function Li(A,r,e){var Q=0,f=0;A:{r:if(e>>>0>=2)for(e=e-1|0;;){if(!(Q=B[0|r]))break r;if(i[0|A]=Q,A=A+1|0,r=r+1|0,(0|e)==(0|(f=f+1|0)))break}else if(!e)break A;i[0|A]=0}ci(r)}function Xi(A,r,e){var i=0;if(!A)return 1;A:switch(a[A>>2]-1|0){case 0:return(i=a[A+8>>2])?(Ga[0|i](r,e,a[A+4>>2]),1):1;case 1:i=0|Ga[a[A+8>>2]](r,e,A);break;default:break A}return i}function Si(A,r){r|=0;var e,i;return e=a[(A|=0)+24>>2],i=a[e>>2],r=a[r+24>>2],(A=i-a[r>>2]|0)||i&&(A=$i(a[e+8>>2],a[r+8>>2],i))||(A=a[e+4>>2]-a[r+4>>2]|0),0|!A}function Hi(A){var r=0,e=0,i=0;if((0|(r=a[A+4>>2]))>0){e=a[A>>2]+(r<<2)|0;A:{for(;;){if(a[(e=e-4|0)>>2])break A;if(i=(0|r)>1,r=r-1|0,!i)break}r=0}a[A+4>>2]=r}r||(a[A+12>>2]=0)}function pi(A){var r,e=0,i=0;return i=(i=A>>>0>65535)<<4|0!=(0|A)|(r=(e=i?A>>>16|0:A)>>>0>255)<<3,((i|=(e=(A=r?e>>>8|0:e)>>>0>15)<<2)|(e=(A=e?A>>>4|0:A)>>>0>3)<<1)+((e?A>>>2|0:A)>>>0>1)|0}function vi(){var A,r;return(A=Ki(40,31289))?(a[A+28>>2]=1,a[A+8>>2]=1,a[A>>2]=0,a[A+4>>2]=0,r=uf(),a[A+36>>2]=r,r?A:(Ve(6,106,65),SQ(A,31289,0),0)):(Ve(6,106,65),0)}function zi(A,r){var e=0,i=0;return!r|16!=a[r>>2]||(r=a[r+4>>2])&&(s=e=s-16|0,a[e+12>>2]=a[r+8>>2],(i=AQ(0,e+12|0,a[r>>2],A))||Ve(13,199,110),s=e+16|0),i}function ji(A,r,e,i){var Q;return s=Q=s-32|0,a[Q+12>>2]=i,a[Q+28>>2]=A,a[Q+24>>2]=r,A=h(Q+28|0,0,Q+24|0,Q+20|0,Q+16|0,e,i),s=Q+32|0,r=a[Q+20>>2],A?a[Q+16>>2]?-1:(0|r)>=0?r:-1:-1}function xi(A){var r=0,e=0;A&&(2&(r=a[A+16>>2])||(e=a[A>>2],8&r?x(e,a[A+8>>2]<<2):SQ(e,31289,0)),1&i[A+16|0]&&SQ(A,31289,0))}function Ki(A,r){var e,Q=0;A:{if(!(e=a[22755])|104==(0|e)){if(!A)break A;B[92200]||(i[92200]=1),r=G(A)}else r=0|Ga[0|e](A,r,0);r&&(Q=IQ(r,0,A))}return Q}function Oi(A){var r,i;return(A=(r=a[22776])+(i=A+3&-4)|0)>>>0<=r>>>0&&i||A>>>0>(e.byteLength/65536|0)<<16>>>0&&!(0|l(0|A))?(a[23332]=48,-1):(a[22776]=A,r)}function Pi(){var A;return(A=Ki(48,31289))?(a[A>>2]=0,a[A+4>>2]=0,a[A+24>>2]=0,a[A+28>>2]=0,a[A+16>>2]=0,a[A+20>>2]=0,a[A+8>>2]=0,a[A+12>>2]=0,A):(Ve(3,106,65),A)}function qi(A,r,e,i,Q){var f=0,B=0;return A&&(f=a[A>>2])&&912!=(0|(f=a[f>>2]))&&(B=-1,6!=(0|f))||(B=Ie(A,-1,r,e,i,Q)),B}function _i(A){var r,e,i=0;s=r=s-16|0,(e=Ur())&&(i=a[e+384>>2],A=(A-1&(-1^A))>>31,a[r+8>>2]=A,a[r+12>>2]=-1^A,a[(A=(i<<2)+e|0)>>2]=a[A>>2]|2&a[r+12>>2]),s=r+16|0}function $i(A,r,e){var i=0,Q=0,f=0;A:if(e){for(;;){if((0|(i=B[0|A]))==(0|(Q=B[0|r]))){if(r=r+1|0,A=A+1|0,e=e-1|0)continue;break A}break}f=i-Q|0}return f}function AQ(A,r,e,Q){var f,B=0;return s=f=s-32|0,i[f+8|0]=0,a[f+4>>2]=0,(0|T(A=A||f+4|0,r,e,Q,-1,0,0,f+8|0,0))<=0?ga(A,Q):B=a[A>>2],s=f+32|0,B}function rQ(A,r){var e=0,i=0;e=67;A:{if(!(!A|!r)&&(e=108,i=a[a[A>>2]+12>>2])){if(A=0|Ga[0|i](A,r))break A;e=106}A=0,Ve(37,108,e)}return A}function eQ(A,r,e){var Q=0;if(A>>>0<r>>>0)return yQ(A,r,e);if(e)for(Q=A+e|0,r=r+e|0;r=r-1|0,i[0|(Q=Q-1|0)]=B[0|r],e=e-1|0;);return A}function iQ(A,r,e){A|=0,r|=0,e|=0;var Q;return!(Q=a[22755])|104==(0|Q)?(A?(B[92200]||(i[92200]=1),A=G(A)):A=0,0|A):0|Ga[0|Q](A,r,e)}function QQ(){var A;return(A=iQ(76,31289,0))?(a[A>>2]=0,rf(A+4|0),rf(A+24|0),rf(A+44|0),a[A+72>>2]=1,a[A+64>>2]=0,a[A+68>>2]=0,A):(Ve(3,149,65),A)}function fQ(A,r){for(var e=0,Q=0,f=0;(0|r)!=(0|Q);)s=e=s-16|0,i[e+15|0]=0,f=0|o(91108,e+15|0,0),s=e+16|0,i[A+Q|0]=f,Q=Q+1|0;return 1}function aQ(A,r,e){var i;return s=i=s-16|0,(e=Ze(e,i+12|0))?(A=0|Ga[a[a[A>>2]+100>>2]](A,r,a[i+12>>2],e),SQ(e,31289,0)):A=0,s=i+16|0,A}function BQ(A){return IQ(A+20|0,0,76),a[A+16>>2]=-1009589776,a[A+8>>2]=-1732584194,a[A+12>>2]=271733878,a[A>>2]=1732584193,a[A+4>>2]=-271733879,1}function tQ(A,r,e){r=(0|(r=(0|r)>0?r:0))<(0|e)?r:e,e=1;A:{for(;;){if(!r)break A;if(r=r-1|0,1!=(0|Qr(A,31112)))break}e=0}return e}function bQ(A){var r=0;A&&((r=a[A>>2])&&(r=a[r+16>>2])&&Ga[0|r](A),Ii(a[A+8>>2]),Ii(a[A+12>>2]),SQ(A,31289,0))}function kQ(A,r){var e,i=0;for(i=ci(A)+1|0,e=255&r;r=0,i&&(0|e)!=B[0|(r=(i=i-1|0)+A|0)];);return r}function nQ(A,r){a[A>>2]=a[r>>2],a[A+4>>2]=a[r+4>>2],a[A+8>>2]=a[r+8>>2],a[A+12>>2]=a[r+12>>2],a[A+16>>2]=6|(-4&a[r+16>>2]|1&a[A+16>>2])}function cQ(A,r){var e=0,i=0;r>>>0<=31?(i=a[A+4>>2],e=A):(r=r-32|0,e=A+4|0),e=a[e>>2],a[A+4>>2]=i>>>r,a[A>>2]=i<<32-r|e>>>r}function CQ(A){A&&(Qi(a[A+8>>2]),Qi(a[A+12>>2]),Qi(a[A+4>>2]),a[A>>2]=0,Qa(a[A+8>>2]),Qa(a[A+12>>2]),Qa(a[A+4>>2]),SQ(A,37988,0))}function EQ(A,r){var e=0,i=0;r>>>0<=31?(i=a[A>>2],e=A+4|0):(r=r-32|0,e=A),e=a[e>>2],a[A>>2]=i<<r,a[A+4>>2]=e<<r|i>>>32-r}function oQ(A){var r=0;A&&(r=a[A+8>>2])&&(ca(r,0),Di(r=a[A+8>>2],30,r),Ae(a[A+8>>2],31),li(a[A+8>>2]))}function lQ(A){var r=0,e=0;A&&((r=a[A+4>>2])&&(e=a[A+8>>2],1&i[A+12|0]?x(r,e):pQ(r,e)),SQ(A,31289,0))}function wQ(A){var r=0,e=0;A&&(e=128&(r=a[A+12>>2]),16&r||SQ(a[A+8>>2],31289,0),e||SQ(A,31289,0))}function gQ(A,r){var e=0,i=0;return(0|r)<0||(0|(e=r>>>5|0))>=a[A+4>>2]||(i=a[a[A>>2]+(e<<2)>>2]>>>r&1),i}function uQ(){var A=0,r=0;(A=Ur())&&(0|(r=a[A+384>>2]))!=a[A+388>>2]&&(a[(A=A+(r<<2)|0)>>2]=1|a[A>>2])}function MQ(A,r){var e=0;return a[A+8>>2]<=0&&!ke(A,1)||(a[A+12>>2]=0,a[a[A>>2]>>2]=r,a[A+4>>2]=0!=(0|r),e=1),e}function sQ(A){var r=0,e=0;return A&&(r=a[A+12>>2])&&(r=a[r+48>>2])&&(e=0|Ga[0|r](A)),e}function DQ(A){A:switch(A-672|0){case 0:return 52;case 1:return 54;case 2:return 53;default:break A}return 64!=(0|A)?-1:51}function IQ(A,r,e){A|=0,r|=0;var Q=0;if(e|=0)for(Q=A;i[0|Q]=r,Q=Q+1|0,e=e-1|0;);return 0|A}function NQ(A){var r=0,e=0;return A&&(r=a[A>>2])&&(r=r-1|0,e=a[a[A+4>>2]+(r<<2)>>2],a[A>>2]=r),e}function GQ(A,r){var e;return 1==(0|(e=a[A+4>>2]))&a[a[A>>2]>>2]==(0|r)?r?!a[A+12>>2]:1:!(r|e)}function UQ(A,r){var e=0;return e=0,!A|(0|r)<0||(e=0,a[A>>2]<=(0|r)||(e=a[a[A+4>>2]+(r<<2)>>2])),e}function FQ(A,r,e,i){var Q,f=0;return ne(i),(Q=sA(i))&&ni(Q,r)&&(f=cA(A,Q,e)),Yr(i),Hi(A),f}function ZQ(A,r,e,i){return(0|r)<(0|i)?(Ve(4,107,110),0):(0|r)>(0|i)?(Ve(4,107,122),0):(yQ(A,e,i),1)}function dQ(A,r,e,i){if(H(0,A,r,e,i)){if(!a[A+12>>2])return 1;A=0|Ga[0|(a[e+12>>2]?26:27)](A,A,e)}else A=0;return A}function WQ(A){var r,e=0;return e=-1,(0|(r=a[A+4>>2]))>1||(e=0,1==(0|r)&&(e=a[a[A>>2]>>2])),e}function VQ(A,r){var e=0;if(!(!A|r>>>0>2147483646)){if(!(e=iQ(r,31289,0)))return Ve(15,115,65),0;yQ(e,A,r)}return e}function YQ(A,r,e,i,Q,f){var B;return(B=a[a[f+8>>2]+48>>2])?0|Ga[0|B](A,r,e,i,Q,f):v(A,r,e,0,0,i,Q,f)}function RQ(A){var r;return(r=a[23075])>>>0<=255?(a[23075]=r+1,a[A>>2]=r,a[92304+(r<<2)>>2]=0,A=1):A=0,A}function yQ(A,r,e){var Q=0;if(e)for(Q=A;i[0|Q]=B[0|r],Q=Q+1|0,r=r+1|0,e=e-1|0;);return A}function hQ(A,r,e,i){var Q,f=0;return ne(i),(Q=sA(i))&&ni(Q,r)&&(f=cA(A,Q,e)),Yr(i),f}function JQ(A,r){var e,i=0;return s=e=s-16|0,!A|!r||(a[e>>2]=r,a[e+4>>2]=0,i=Kr(a[A+8>>2],e)),s=e+16|0,i}function TQ(A){A&&(yi(A+4|0),yi(A+24|0),yi(A+44|0),1&i[A+72|0]&&SQ(A,31289,0))}function mQ(A){var r=0;return r=2,(0|A)<1024||(r=3,A>>>0<4096||(r=A>>>0<8192?4:5)),r}function LQ(A){A&&(xi(a[A>>2]),xi(a[A+4>>2]),xi(a[A+8>>2]),xi(a[A+12>>2]),Yf(a[A+36>>2]),SQ(A,31289,0))}function XQ(A,r){return 1062!=a[A>>2]?(Ve(6,172,175),0):(A=a[A+24>>2],a[r>>2]=a[A>>2],a[A+8>>2])}function SQ(A,r,e){A|=0,r|=0,e|=0;var i;!(i=a[22757])|106==(0|i)?m(A):Ga[0|i](A,r,e)}function HQ(A){var r;return s=r=s-16|0,a[r+12>>2]=0,A=eA(r+12|0,A,0),s=r+16|0,(0|A)>0?a[r+12>>2]:0}function pQ(A,r){if(A){if(r&&Of(A,r),!(!(r=a[22757])|106==(0|r)))return void Ga[0|r](A,31289,0);m(A)}}function vQ(A,r,e){var i;return(0|(i=ci(e)))<0?-1:0|Ga[a[a[A>>2]+100>>2]](A,r,i,e)}function zQ(A,r){return ni(A,r)?(xf(A+20|0),r=Dr(r),a[A+44>>2]=0,a[A+40>>2]=r,1):0}function jQ(A){return A=(255&(A=A-48|0))>>>0<=54?a[38256+(A<<24>>24<<2)>>2]:-1}function xQ(A){var r;return(r=Ki(16,31289))?(a[r+4>>2]=A,r):(Ve(13,130,65),r)}function KQ(A){var r;return(r=Ki(20,31289))?(a[r+16>>2]=A,r):(Ve(3,113,65),r)}function OQ(A,r){var e=0;return(-1>>>(e=31&r)&A)<<e|((e=A)&-1<<(A=0-r&31))>>>A}function PQ(A){var r=0;(A=a[(A|=0)+24>>2])&&((r=a[A+8>>2])&&Of(r,a[A>>2]),wQ(A))}function qQ(A){var r=0;return(r=gf(a[A>>2]-1|0))||(r=(A=gf(a[A+4>>2]))?A+32|0:0),r}function _Q(A,r){return(A=a[A>>2])>>>0<=255?(a[92304+(A<<2)>>2]=r,A=1):A=0,A}function $Q(A,r,e){return(0|e)<0?(Ve(3,145,119),0):(r=Xr(A,r,e),Hi(A),r)}function Af(A){return A=(A=a[A>>2])>>>0<=255?a[92304+(A<<2)>>2]:0}function rf(A){a[A>>2]=0,a[A+4>>2]=0,a[A+16>>2]=0,a[A+8>>2]=0,a[A+12>>2]=0}function ef(A,r){var e;s=e=s-16|0,a[e+12>>2]=A,dA(e+12|0,r,0),s=e+16|0}function Qf(A){return 1!=a[A+4>>2]|1!=a[a[A>>2]>>2]?0:!a[A+12>>2]}function ff(A,r,e,i){return ar(r,e||0,A,i)}function af(A,r){return r|=0,a[a[(A|=0)>>2]>>2]-a[a[r>>2]>>2]|0}function Bf(){var A;return(A=Ki(16,31289))||Ve(7,101,65),A}function tf(A,r,e){return a[A>>2]?nf(a[A+4>>2],r,e):0}function bf(A){var r;r=a[A>>2],i[0|r]=0,i[r+1|0]=0,a[A>>2]=r+2}function kf(){var A;return(A=Ki(20,31289))&&(a[A+16>>2]=0),A}function nf(A,r,e){return e?0|Ga[a[A+20>>2]](A,r,e):1}function cf(A,r,e,i,Q){return 0|Ga[a[a[i+8>>2]+12>>2]](A,r,e,i,Q)}function Cf(A,r,e,i,Q){return 0|Ga[a[a[i+8>>2]+16>>2]](A,r,e,i,Q)}function Ef(A,r,e,i,Q){return 0|Ga[a[a[i+8>>2]+4>>2]](A,r,e,i,Q)}function of(A,r,e,i,Q){return 0|Ga[a[a[i+8>>2]+8>>2]](A,r,e,i,Q)}function lf(A){return a[A+4>>2]<=0?0:1&a[a[A>>2]>>2]}function wf(A){return A?a[A+8>>2]:(Ve(6,162,159),-1)}function gf(A){return i[90752+(n(0-A&A,124511785)>>>27|0)|0]}function uf(){var A;return(A=Ki(4,31289))&&(a[A>>2]=1),A}function Mf(A,r){return 0!=(t[37360+(A<<1)>>1]&r)&A>>>0<=127}function sf(){var A;(A=Af(91940))&&(_Q(91940,0),DA(A))}function Df(A){A&&(SQ(a[A+4>>2],31289,0),SQ(A,31289,0))}function If(A,r){return a[A>>2]||(Ga[0|r](),a[A>>2]=1),1}function Nf(A){return A?a[a[A+8>>2]+36>>2]:0}function Gf(A,r){return a[A+8>>2]<(0|r)&&(A=ke(A,r)),A}function Uf(A,r){return A=ae(A,r),B[0|A]==(255&r)?A:0}function Ff(A){return 1==a[A+4>>2]&1==a[a[A>>2]>>2]&1}function Zf(A,r,e,i,Q){return r=Gi(A,r,e,i,Q),Hi(A),r}function df(A,r,e,i){return r=xA(A,r,e,i),Hi(A),r}function Wf(A,r){return a[a[A>>2]+a[r+4>>2]>>2]}function Vf(A){return A?a[A>>2]:-1}function Yf(A){A&&(a[A>>2]=0,SQ(A,31289,0))}function Rf(A,r,e,i){return Zf(A,r,e+4|0,e,i)}function yf(A,r,e,i){return Gi(A,r,e+4|0,e,i)}function hf(A){return(Dr(a[A+16>>2])+7|0)/8|0}function Jf(A,r,e){return r=Zr(A,r,e),Hi(A),r}function Tf(A,r,e){a[a[A>>2]+a[e+4>>2]>>2]=r}function mf(A,r){a[A+8>>2]=a[A+8>>2]&(-1^r)}function Lf(A,r){return a[A>>2]+a[r+8>>2]|0}function Xf(A,r,e){return AQ(A,r,e,89488)}function Sf(A,r,e){return HA(A,r,e),N=I,D}function Hf(A,r,e){return AQ(A,r,e,89584)}function pf(A){return 32==(0|A)|A-9>>>0<5}function vf(A){a[A+36>>2]=0,a[A+32>>2]=0}function zf(A){return A-65>>>0<26?32|A:A}function jf(A,r){a[A+16>>2]=a[A+16>>2]|r}function xf(A){a[A+4>>2]=0,a[A+12>>2]=0}function Kf(A,r){a[A+8>>2]=a[A+8>>2]|r}function Of(A,r){Ga[a[22758]](A,0,r)}function Pf(A,r){return xe(A,r,89488)}function qf(A,r){return xe(A,r,32560)}function _f(A){a[A+8>>2]=a[A+8>>2]+1}function $f(A,r){return a[A+8>>2]&r}function Aa(A,r,e){return ze(A,r,e)}function ra(A){return 0!=a[A+12>>2]}function ea(A,r){return xr(A,r,1,1)}function ia(A){return 4&a[A+16>>2]}function Qa(A){Qi(A),SQ(A,31289,0)}function fa(A){SQ(A|=0,31289,0)}function aa(A){return A-48>>>0<10}function Ba(A,r){return eA(A,r,0)}function ta(){return Ki(24,31289)}function ba(A){return wA(A,0,0,0)}function ka(A){return!a[A+4>>2]}function na(A,r,e){Ai(0,A,r,e)}function ca(A,r){a[A+32>>2]=r}function Ca(A,r){a[A+20>>2]=r}function Ea(A){ef(A,89720)}function oa(A){ef(A,32672)}function la(A){ef(A,32744)}function wa(A){a[A>>2]=257}function ga(A,r){dA(A,r,0)}function ua(){i[92160]=1}function Ma(){i[92168]=1}function sa(){i[92176]=1}function Da(){a[23036]=1}function Ia(){M(),E()}Q(r=B,1024,"aWQtYWNhLWFjY2Vzc0lkZW50aXR5AGlkLWFjYS1jaGFyZ2luZ0lkZW50aXR5AGFjLWF1ZGl0RW50aXR5AHNlY3VyaXR5AFNlY3VyaXR5AENNQyBSZWdpc3RyYXRpb24gQXV0aG9yaXR5AENNQyBDZXJ0aWZpY2F0ZSBBdXRob3JpdHkAc3VidHJlZU1heGltdW1RdWFsaXR5AHN1YnRyZWVNaW5pbXVtUXVhbGl0eQBzaW5nbGVMZXZlbFF1YWxpdHkAZFNBUXVhbGl0eQBmcmllbmRseUNvdW50cnkAaWQtc21pbWUtYWEtbWxFeHBhbmRIaXN0b3J5AGNhUmVwb3NpdG9yeQBDQSBSZXBvc2l0b3J5AGRpcmVjdG9yeQBEaXJlY3RvcnkAYnVzaW5lc3NDYXRlZ29yeQBpZC1zbWltZS1jdGktZXRzLXByb29mT2ZEZWxpdmVyeQBzZWNyZXRhcnkAa3gtYW55AGF1dGgtYW55AHNldGV4dC1waW5BbnkAc2V0Y3QtUEFOT25seQBoZXhrZXkAcHVibGljX2tleQBpZC1yZWdDdHJsLXByb3RvY29sRW5jcktleQBSU0FQcml2YXRlS2V5AGlkLWVjUHVibGljS2V5AFJTQVB1YmxpY0tleQBtZW1iZXItYm9keQBJU08gVVMgTWVtYmVyIEJvZHkASVNPIE1lbWJlciBCb2R5AElTTyBDTiBNZW1iZXIgQm9keQBzZXQtcG9saWN5AGFueVBvbGljeQBpbmhpYml0QW55UG9saWN5AHNldC1hZGRQb2xpY3kAWDUwOXYzIEluaGliaXQgQW55IFBvbGljeQBYNTA5djMgQW55IFBvbGljeQBsYXN0TW9kaWZpZWRCeQBqYW5ldE1haWxib3gAb3RoZXJNYWlsYm94AHJmYzgyMk1haWxib3gAcG9zdE9mZmljZUJveABpQTVTdHJpbmdTeW50YXgAY2FzZUlnbm9yZUlBNVN0cmluZ1N5bnRheABwaWxvdEF0dHJpYnV0ZVN5bnRheABtYXgAJXMgU2FsdCBMZW5ndGg6IDB4AFRyYWlsZXIgRmllbGQ6IDB4AHNldGV4dC1jdgAuJWx1ACUwOXUAJXUAc2V0QXR0ci1UMmNsZWFydHh0AHN1cHBvcnRlZEFwcGxpY2F0aW9uQ29udGV4dABzZXQtY2VydEV4dABuc0NlcnRFeHQAc2V0Q2V4dC1zZXRFeHQAc2V0LW1zZ0V4dABzZXRjdC1IT0RJbnB1dABob3N0AGt4LWdvc3QAYXNzZXJ0aW9uIGZhaWxlZDogKGNoYXIgKiopdGVtcC0+bmV4dC0+cF9uZXh0ID09IGxpc3QAYXV0aG9yaXR5UmV2b2NhdGlvbkxpc3QAY2VydGlmaWNhdGVSZXZvY2F0aW9uTGlzdABkZWx0YVJldm9jYXRpb25MaXN0AGlkLXNtaW1lLWFhLXJlY2VpcHRSZXF1ZXN0AGlkLWl0LXN1YnNjcmlwdGlvblJlcXVlc3QAaWQtY21jLXJldm9rZVJlcXVlc3QATWljcm9zb2Z0IEV4dGVuc2lvbiBSZXF1ZXN0AGlkLXRjMjYtc2lnbndpdGhkaWdlc3QAaWQtdGMyNi1kaWdlc3QAaWQtc21pbWUtYWEtbXNnU2lnRGlnZXN0AG1lc3NhZ2VEaWdlc3QAaWQtdGMyNi1nb3N0LTM0MTAtMjAxMi01MTItcGFyYW1TZXRUZXN0AGlkLW1vZC1hdHRyaWJ1dGUtY2VydABpZC1jbWMtZ2V0Q2VydABpZC1zbWltZS1jdC1wdWJsaXNoQ2VydABpZC1zbWltZS1hYS1ldHMtb3RoZXJTaWdDZXJ0AGlkLWl0LWNhUHJvdEVuY0NlcnQAc2V0QXR0ci1DZXJ0AHJGQzgyMmxvY2FsUGFydABpZC1xdABpZC1zY3J5cHQAc2V0ZXh0LWdlbkNyeXB0AGlkLXNtaW1lLWN0LXJlY2VpcHQAaWQtc21pbWUtY3RpLWV0cy1wcm9vZk9mUmVjZWlwdABzZXQtcG9saWN5LXJvb3QAdHJ1c3RSb290AHNldENleHQtaGFzaGVkUm9vdABUcnVzdCBSb290AHBpbG90AGFjY291bnQAaXNzdWluZ0Rpc3RyaWJ1dGlvblBvaW50AFg1MDl2MyBJc3N1aW5nIERpc3RyaWJ1dGlvbiBQb2ludABpZC1zbWltZS1hYS1jb250ZW50SGludABkb21haW5Db21wb25lbnQAZG9jdW1lbnQAbnNDb21tZW50AE5ldHNjYXBlIENvbW1lbnQATWFuYWdlbWVudABpZC10YzI2LWFncmVlbWVudABkaEtleUFncmVlbWVudABzZWN1cmVTaGVsbENsaWVudABTU0ggQ2xpZW50AGlkLXBwbC1pbmRlcGVuZGVudABJbmRlcGVuZGVudABnb3N0ODktY250AHJvbGVPY2N1cGFudABtZ210AE9wZW5TU0wgZGVmYXVsdABoZXhzYWx0AGlkLXBraW5pdABPUEVOU1NMX2luaXQASE1BQyBHT1NUIDM0LjExLTIwMTIgMjU2IGJpdABITUFDIEdPU1QgMzQuMTEtMjAxMiA1MTIgYml0AGlkLWl0AGlkLXNldABNaWNyb3NvZnQgTG9jYWwgS2V5IHNldABHT1NUIFIgMzQuMTAtMjAxMiAoNTEyIGJpdCkgdGVzdGluZyBwYXJhbWV0ZXIgc2V0AEdPU1QgMjgxNDctODkgVEMyNiBwYXJhbWV0ZXIgc2V0AGhleHNlY3JldABzdHJlZXQATG9jYWxLZXlTZXQAaWQtR29zdDI4MTQ3LTg5LVRlc3RQYXJhbVNldABpZC1Hb3N0UjM0MTEtOTQtVGVzdFBhcmFtU2V0AGlkLUdvc3RSMzQxMC05NC1UZXN0UGFyYW1TZXQAaWQtR29zdFIzNDEwLTIwMDEtVGVzdFBhcmFtU2V0AGlkLUdvc3RSMzQxMS05NC1DcnlwdG9Qcm9QYXJhbVNldABpZC1Hb3N0MjgxNDctODktQ3J5cHRvUHJvLUQtUGFyYW1TZXQAaWQtR29zdFIzNDEwLTk0LUNyeXB0b1Byby1ELVBhcmFtU2V0AGlkLUdvc3RSMzQxMC05NC1DcnlwdG9Qcm8tWGNoQy1QYXJhbVNldABpZC1Hb3N0MjgxNDctODktQ3J5cHRvUHJvLUMtUGFyYW1TZXQAaWQtR29zdFIzNDEwLTk0LUNyeXB0b1Byby1DLVBhcmFtU2V0AGlkLUdvc3RSMzQxMC0yMDAxLUNyeXB0b1Byby1DLVBhcmFtU2V0AGlkLUdvc3RSMzQxMC05NC1DcnlwdG9Qcm8tWGNoQi1QYXJhbVNldABpZC1Hb3N0UjM0MTAtMjAwMS1DcnlwdG9Qcm8tWGNoQi1QYXJhbVNldABpZC1Hb3N0MjgxNDctODktQ3J5cHRvUHJvLUItUGFyYW1TZXQAaWQtR29zdFIzNDEwLTk0LUNyeXB0b1Byby1CLVBhcmFtU2V0AGlkLUdvc3RSMzQxMC0yMDAxLUNyeXB0b1Byby1CLVBhcmFtU2V0AGlkLUdvc3RSMzQxMC05NC1DcnlwdG9Qcm8tWGNoQS1QYXJhbVNldABpZC1Hb3N0UjM0MTAtMjAwMS1DcnlwdG9Qcm8tWGNoQS1QYXJhbVNldABpZC1Hb3N0MjgxNDctODktQ3J5cHRvUHJvLUEtUGFyYW1TZXQAaWQtR29zdFIzNDEwLTk0LUNyeXB0b1Byby1BLVBhcmFtU2V0AGlkLUdvc3RSMzQxMC0yMDAxLUNyeXB0b1Byby1BLVBhcmFtU2V0AGlkLUdvc3QyODE0Ny04OS1DcnlwdG9Qcm8tUklDLTEtUGFyYW1TZXQAaWQtR29zdDI4MTQ3LTg5LUNyeXB0b1Byby1Pc2Nhci0xLTEtUGFyYW1TZXQAaWQtR29zdDI4MTQ3LTg5LUNyeXB0b1Byby1Pc2Nhci0xLTAtUGFyYW1TZXQAR09TVCAyODE0Ny04OSBDcnlwdG9jb20gUGFyYW1TZXQAZElUUmVkaXJlY3QAaG9sZEluc3RydWN0aW9uUmVqZWN0AEhvbGQgSW5zdHJ1Y3Rpb24gUmVqZWN0AFNpZ25pbmcgVG9vbCBvZiBTdWJqZWN0AGRjb2JqZWN0AHNpbXBsZVNlY3VyaXR5T2JqZWN0AHBpbG90T2JqZWN0AGRvbWFpblJlbGF0ZWRPYmplY3QAZGNPYmplY3QAaWQtY2N0AGlkLXNtaW1lLWN0AGpvaW50LWlzby1pdHUtdABzZXQtYnJhbmQtTm92dXMAb3JnYW5pemF0aW9uYWxTdGF0dXMAZXh0ZW5kZWRTdGF0dXMARXh0ZW5kZWQgT0NTUCBTdGF0dXMAR09TVCBSIDM0LjEwLTIwMTIgd2l0aCAyNTYgYml0IG1vZHVsdXMAR09TVCBSIDM0LjEwLTIwMTIgd2l0aCA1MTIgYml0IG1vZHVsdXMAYWVzLTEyOC14dHMAYWVzLTI1Ni14dHMAaWQtc21pbWUtYWEtc21pbWVFbmNyeXB0Q2VydHMAY3JsRGlzdHJpYnV0aW9uUG9pbnRzAFg1MDl2MyBDUkwgRGlzdHJpYnV0aW9uIFBvaW50cwBwb2xpY3lDb25zdHJhaW50cwBuYW1lQ29uc3RyYWludHMAYmFzaWNDb25zdHJhaW50cwBYNTA5djMgUG9saWN5IENvbnN0cmFpbnRzAFg1MDl2MyBOYW1lIENvbnN0cmFpbnRzAFg1MDl2MyBCYXNpYyBDb25zdHJhaW50cwBxY1N0YXRlbWVudHMAaWQtdGMyNi1kaWdlc3QtY29uc3RhbnRzAGlkLXRjMjYtY2lwaGVyLWNvbnN0YW50cwBpZC10YzI2LXNpZ24tY29uc3RhbnRzAGlkLXRjMjYtZ29zdC0yODE0Ny1jb25zdGFudHMAaWQtdGMyNi1nb3N0LTM0MTAtMjAxMi0yNTYtY29uc3RhbnRzAGlkLXRjMjYtY29uc3RhbnRzAGlkLXRjMjYtZ29zdC0zNDEwLTIwMTItNTEyLWNvbnN0YW50cwByc2Ffa2V5Z2VuX2JpdHMAY3RfcHJlY2VydF9zY3RzAGN0X2NlcnRfc2N0cwBwc3MAc2V0LWJyYW5kLUFtZXJpY2FuRXhwcmVzcwBzdHJlZXRBZGRyZXNzAHByZXNlbnRhdGlvbkFkZHJlc3MAZW1haWxBZGRyZXNzAHBvc3RhbEFkZHJlc3MAaG9tZVBvc3RhbEFkZHJlc3MAdW5zdHJ1Y3R1cmVkQWRkcmVzcwByZWdpc3RlcmVkQWRkcmVzcwB0ZXh0RW5jb2RlZE9SQWRkcmVzcwB4MTIxQWRkcmVzcwBpZC1jbWMtcG9wTGlua1dpdG5lc3MAaWQtY21jLWxyYVBPUFdpdG5lc3MAYXV0aG9yaXR5SW5mb0FjY2VzcwBzdWJqZWN0SW5mb0FjY2VzcwBBdXRob3JpdHkgSW5mb3JtYXRpb24gQWNjZXNzAFN1YmplY3QgSW5mb3JtYXRpb24gQWNjZXNzAEN0cmwvcHJvdmlzaW9uIFdBUCBBY2Nlc3MAaWQtc21pbWUtbW9kLWVzcwBhdXRoLWRzcwBwaWxvdE9iamVjdENsYXNzAHVzZXJDbGFzcwByc2Fzc2FQc3MAaWQtYWNhLWVuY0F0dHJzAGlkLXJlZ0luZm8tdXRmOFBhaXJzAGNhSXNzdWVycwBDQSBJc3N1ZXJzAHNldC1icmFuZC1EaW5lcnMAaWQtY2N0LWNycwBwaWxvdEdyb3VwcwBpZC1xdC1jcHMAcHJpbWVfaW5mb3MAaWQtcmVnQ3RybC1wa2lBcmNoaXZlT3B0aW9ucwBTZWN1cmUgRWxlY3Ryb25pYyBUcmFuc2FjdGlvbnMAaW50ZXJuYXRpb25hbC1vcmdhbml6YXRpb25zAEludGVybmF0aW9uYWwgT3JnYW5pemF0aW9ucwBjZXJ0aWZpY2F0ZSBleHRlbnNpb25zAG1lc3NhZ2UgZXh0ZW5zaW9ucwBpZC1jbWMtYWRkRXh0ZW5zaW9ucwBYNTAwYWxnb3JpdGhtcwBpZC10YzI2LWFsZ29yaXRobXMAZGlyZWN0b3J5IHNlcnZpY2VzIC0gYWxnb3JpdGhtcwBzdXBwb3J0ZWRBbGdvcml0aG1zAGlkLXNtaW1lLW1vZC1jbXMAYWFDb250cm9scwBpZC1zbWltZS1hYS1lcXVpdmFsZW50TGFiZWxzAGluaXRpYWxzAGlkLWNoYXJhY3RlcmlzdGljLXR3by1iYXNpcwB0cEJhc2lzAHBwQmFzaXMAb25CYXNpcwBpZC1Hb3N0UjM0MTAtOTQtYkJpcwBpZC1Hb3N0UjM0MTAtOTQtYUJpcwBtaW1lLW1ocwBwb2xpY3lNYXBwaW5ncwBYNTA5djMgUG9saWN5IE1hcHBpbmdzAG1pbWUtbWhzLWhlYWRpbmdzAGZsYWdzAGlkLWl0LXN1cHBMYW5nVGFncwBpZC1zbWltZS1hYS1ldHMtUmV2b2NhdGlvblJlZnMAaWQtc21pbWUtYWEtZXRzLUNlcnRpZmljYXRlUmVmcwB5ZXMAaWQtc21pbWUtYWEtZXRzLWNlcnRWYWx1ZXMAaWQtc21pbWUtYWEtZXRzLXJldm9jYXRpb25WYWx1ZXMAc3ViamVjdERpcmVjdG9yeUF0dHJpYnV0ZXMAZXh0ZW5kZWRDZXJ0aWZpY2F0ZUF0dHJpYnV0ZXMAWDUwOXYzIFN1YmplY3QgRGlyZWN0b3J5IEF0dHJpYnV0ZXMAYWNjZXB0YWJsZVJlc3BvbnNlcwBBY2NlcHRhYmxlIE9DU1AgUmVzcG9uc2VzAGVudGVycHJpc2VzAEVudGVycHJpc2VzAHNlbGVjdGVkLWF0dHJpYnV0ZS10eXBlcwBjb250ZW50IHR5cGVzAGlkLWl0LXNpZ25LZXlQYWlyVHlwZXMAaWQtaXQtZW5jS2V5UGFpclR5cGVzAFNlbGVjdGVkIEF0dHJpYnV0ZSBUeXBlcwByc2Ffa2V5Z2VuX3ByaW1lcwBzZXRDZXh0LVBHV1ljYXBhYmlsaXRpZXMAcGF5bWVudCBnYXRld2F5IGNhcGFiaWxpdGllcwBpc3N1ZXIgY2FwYWJpbGl0aWVzAHNldENleHQtSXNzdWVyQ2FwYWJpbGl0aWVzAFMvTUlNRSBDYXBhYmlsaXRpZXMAZG9jdW1lbnRTZXJpZXMAbWltZS1taHMtYm9kaWVzAGNlcnRpZmljYXRlUG9saWNpZXMAWDUwOXYzIENlcnRpZmljYXRlIFBvbGljaWVzAGlkLW1vZC1kdmNzAGFkIGR2Y3MAaWQtcWNzAHBrY3MAQ1QgUHJlY2VydGlmaWNhdGUgU0NUcwBDVCBDZXJ0aWZpY2F0ZSBTQ1RzAGlkLWl0LXVuc3VwcG9ydGVkT0lEcwAlMDJ4JXMAJXMlcyVzAHNldC1hdHRyAGlkLXNtaW1lLWFhLWV0cy1zaWduZXJBdHRyAGFzc2VydGlvbiBmYWlsZWQ6IHNoLmZyZWVsaXN0W2xpc3RdID09IHB0cgBncmFzc2hvcHBlci1jdHIAbWFnbWEtY3RyAGdvc3Q4OS1jdHIAYWVzLTEyOC1jdHIAYXJpYS0xMjgtY3RyAGNhbWVsbGlhLTEyOC1jdHIAYWVzLTI1Ni1jdHIAYXJpYS0yNTYtY3RyAGNhbWVsbGlhLTI1Ni1jdHIAc200LWN0cgBhZXMtMTkyLWN0cgBhcmlhLTE5Mi1jdHIAY2FtZWxsaWEtMTkyLWN0cgBzZXJ2aWNlTG9jYXRvcgBPQ1NQIFNlcnZpY2UgTG9jYXRvcgBpZC1yZWdDdHJsLWF1dGhlbnRpY2F0b3IAZGVzdGluYXRpb25JbmRpY2F0b3IAWDUwOXYzIERlbHRhIENSTCBJbmRpY2F0b3IAZG9jdW1lbnRBdXRob3IAYWxnb3IAY3Jvc3NDZXJ0aWZpY2F0ZVBhaXIAc2VjdXJlU2hlbGxTZXJ2ZXIAU1NIIFNlcnZlcgBob2xkSW5zdHJ1Y3Rpb25DYWxsSXNzdWVyAGNlcnRpZmljYXRlSXNzdWVyAEhvbGQgSW5zdHJ1Y3Rpb24gQ2FsbCBJc3N1ZXIAU2lnbmluZyBUb29sIG9mIElzc3VlcgBYNTA5djMgQ2VydGlmaWNhdGUgSXNzdWVyAHNlbmRSb3V0ZXIAc2VuZFByb3hpZWRSb3V0ZXIAU2VuZCBSb3V0ZXIAU2VuZCBQcm94aWVkIFJvdXRlcgBwYXJhbWV0ZXIAaXBzZWNVc2VyAElQU2VjIFVzZXIAb3duZXIAc2VuZE93bmVyAHNlbmRQcm94aWVkT3duZXIAU2VuZCBPd25lcgBTZW5kIFByb3hpZWQgT3duZXIAY3RfcHJlY2VydF9zaWduZXIAQ1QgUHJlY2VydGlmaWNhdGUgU2lnbmVyAGF1dGhvcml0eUtleUlkZW50aWZpZXIAc3ViamVjdEtleUlkZW50aWZpZXIAaWQtc21pbWUtYWEtY29udGVudElkZW50aWZpZXIAaWQtb24tcGVybWFuZW50SWRlbnRpZmllcgBkb2N1bWVudElkZW50aWZpZXIAc2JncC1yb3V0ZXJJZGVudGlmaWVyAG9yZ2FuaXphdGlvbklkZW50aWZpZXIAc2V0Q2V4dC1Ub2tlbklkZW50aWZpZXIAdGVsZXRleFRlcm1pbmFsSWRlbnRpZmllcgB1bmlxdWVJZGVudGlmaWVyAHg1MDBVbmlxdWVJZGVudGlmaWVyAGhvdXNlSWRlbnRpZmllcgBYNTA5djMgQXV0aG9yaXR5IEtleSBJZGVudGlmaWVyAFg1MDl2MyBTdWJqZWN0IEtleSBJZGVudGlmaWVyAFBlcm1hbmVudCBJZGVudGlmaWVyAGdlbmVyYXRpb25RdWFsaWZpZXIAZG5RdWFsaWZpZXIAZG9jdW1lbnRQdWJsaXNoZXIAaWQtdGMyNi1jaXBoZXIAbWFuYWdlcgBpZC1wZGEtZ2VuZGVyAGlkLXNtaW1lLWN0aS1ldHMtcHJvb2ZPZlNlbmRlcgBkaHB1YmxpY251bWJlcgB0ZWxleE51bWJlcgByb29tTnVtYmVyAGNybE51bWJlcgBzZXJpYWxOdW1iZXIAdGVsZXBob25lTnVtYmVyAHBhZ2VyVGVsZXBob25lTnVtYmVyAGhvbWVUZWxlcGhvbmVOdW1iZXIAZmFjc2ltaWxlVGVsZXBob25lTnVtYmVyAG1vYmlsZVRlbGVwaG9uZU51bWJlcgBpbnRlcm5hdGlvbmFsaVNETk51bWJlcgBYNTA5djMgQ1JMIE51bWJlcgBtZW1iZXIAdW5pcXVlTWVtYmVyAGlkLXNtaW1lLXNwcQBzZXRjdC1DYXBUb2tlblNlcQBleHRSZXEAbXNFeHRSZXEAaWQtcmVnSW5mby1jZXJ0UmVxAGlkLWl0LWtleVBhaXJQYXJhbVJlcQByc2Ffa2V5Z2VuX3B1YmV4cABpZC1hY2EtZ3JvdXAASUVFRSBTZWN1cml0eSBpbiBTdG9yYWdlIFdvcmtpbmcgR3JvdXAAaWQtbW9kLW9jc3AAa3gtc3JwAGF1dGgtc3JwAGlkLWFsZy1kaC1wb3AAaXFtcABhc3NlcnRpb24gZmFpbGVkOiBzaC5mcmVlbGlzdFtzbGlzdF0gPT0gdGVtcABpZC1tb2QtY21wAGlkLXNtaW1lLWFhLWV0cy1jb250ZW50VGltZXN0YW1wAGlkLXNtaW1lLWFhLWV0cy1jZXJ0Q1JMVGltZXN0YW1wAGlkLXNtaW1lLWFhLWV0cy1hcmNoaXZlVGltZVN0YW1wAGlkLXNtaW1lLWFhLWV0cy1lc2NUaW1lU3RhbXAAaWQta3AAaWQtcGtpcABpZC1wZGEtY291bnRyeU9mQ2l0aXplbnNoaXAAb2FlcAByc2Flc09hZXAAaWQtaXQta2V5UGFpclBhcmFtUmVwAHdhcABpZC1zbWltZS1hbGctQ01TM0RFU3dyYXAAaWQtc21pbWUtYWxnLTNERVN3cmFwAGlkLXNtaW1lLWFsZy1DTVNSQzJ3cmFwAGlkLXNtaW1lLWFsZy1SQzJ3cmFwAGlkLWFlczEyOC13cmFwAGlkLWNhbWVsbGlhMTI4LXdyYXAAZHN0dTI4MTQ3LXdyYXAAaWQtYWVzMjU2LXdyYXAAaWQtY2FtZWxsaWEyNTYtd3JhcABpZC10YzI2LXdyYXAAaWQtYWVzMTkyLXdyYXAAaWQtY2FtZWxsaWExOTItd3JhcABEU1RVIEdvc3QgMjgxNDctMjAwOSBrZXkgd3JhcABvZWFwAGlkLXNtaW1lLWNkLWxkYXAAc2V0QXR0ci1QR1dZY2FwAHNldEF0dHItSXNzQ2FwAGF1dG8ATWljcm9zb2Z0IFNlcnZlciBHYXRlZCBDcnlwdG8ATmV0c2NhcGUgU2VydmVyIEdhdGVkIENyeXB0bwBwaG90bwBzZWVBbHNvAGlzbwBjcnlwdG9wcm8Abm8AYXVkaW8AaGV4aW5mbwBwcm94eUNlcnRJbmZvAGlkLXNtaW1lLWN0LWNvbnRlbnRJbmZvAGlkLWNtYy1zdGF0dXNJbmZvAGlkLWFjYS1hdXRoZW50aWNhdGlvbkluZm8AaWQtcmVnQ3RybC1wa2lQdWJsaWNhdGlvbkluZm8AaWQtcmVnSW5mbwBpZC1jbWMtcmVnSW5mbwBpZC1pdC1jYUtleVVwZGF0ZUluZm8AaWQtY21jLXJlc3BvbnNlSW5mbwBiaW9tZXRyaWNJbmZvAGlkLXNtaW1lLWN0LVRTVEluZm8AaWQtc21pbWUtY3QtVERUSW5mbwBCaW9tZXRyaWMgSW5mbwBpZC1jbWMtZGF0YVJldHVybgBwaWxvdFBlcnNvbgBjdF9wcmVjZXJ0X3BvaXNvbgBDVCBQcmVjZXJ0aWZpY2F0ZSBQb2lzb24AQ1JMUmVhc29uAHJzYUVuY3J5cHRpb24AZHNhRW5jcnlwdGlvbgBzaGFXaXRoUlNBRW5jcnlwdGlvbgBzaGEyNTZXaXRoUlNBRW5jcnlwdGlvbgBzaGE1MTItMjU2V2l0aFJTQUVuY3J5cHRpb24AbWQ1V2l0aFJTQUVuY3J5cHRpb24AbWQ0V2l0aFJTQUVuY3J5cHRpb24Ac2hhMzg0V2l0aFJTQUVuY3J5cHRpb24Ac2hhMjI0V2l0aFJTQUVuY3J5cHRpb24Ac2hhNTEyLTIyNFdpdGhSU0FFbmNyeXB0aW9uAHNtM1dpdGhSU0FFbmNyeXB0aW9uAG1kMldpdGhSU0FFbmNyeXB0aW9uAHNoYTUxMldpdGhSU0FFbmNyeXB0aW9uAHNoYTFXaXRoUlNBRW5jcnlwdGlvbgBkZXNjcmlwdGlvbgBtYWlsUHJlZmVyZW5jZU9wdGlvbgBlbWFpbFByb3RlY3Rpb24ARS1tYWlsIFByb3RlY3Rpb24Ac3RibF9zZWN0aW9uAGFsZ19zZWN0aW9uAG9pZF9zZWN0aW9uAGlkLXNtaW1lLWN0LWNvbnRlbnRDb2xsZWN0aW9uAGlkZW50aWZpZWQtb3JnYW5pemF0aW9uAHBpbG90T3JnYW5pemF0aW9uAEN0cmwvUHJvdmlzaW9uIFdBUCBUZXJtaW5hdGlvbgB0YXJnZXRJbmZvcm1hdGlvbgBwcm90b2NvbEluZm9ybWF0aW9uAFByb3h5IENlcnRpZmljYXRlIEluZm9ybWF0aW9uAGlkLXNtaW1lLWN0aS1ldHMtcHJvb2ZPZkNyZWF0aW9uAGRvY3VtZW50TG9jYXRpb24AaWQtc21pbWUtYWEtZXRzLXNpZ25lckxvY2F0aW9uAFRMUyBXZWIgQ2xpZW50IEF1dGhlbnRpY2F0aW9uAFRMUyBXZWIgU2VydmVyIEF1dGhlbnRpY2F0aW9uAGlkLWNtYy1pZGVudGlmaWNhdGlvbgBhZGRpdGlvbmFsIHZlcmlmaWNhdGlvbgB4NTA5RXh0QWRtaXNzaW9uAFByb2Zlc3Npb25hbCBJbmZvcm1hdGlvbiBvciBiYXNpcyBmb3IgQWRtaXNzaW9uAHpsaWIgY29tcHJlc3Npb24AdmVyc2lvbgBkb2N1bWVudFZlcnNpb24ATmV0c2NhcGUgQ2VydGlmaWNhdGUgRXh0ZW5zaW9uAGlkLW9uAG1pbgBtc1NtYXJ0Y2FyZExvZ2luAE1pY3Jvc29mdCBTbWFydGNhcmQgTG9naW4AaWQtc21pbWUtY3RpLWV0cy1wcm9vZk9mT3JpZ2luAGRvbWFpbgBhc3NvY2lhdGVkRG9tYWluAGROU0RvbWFpbgBpZC10YzI2LXNpZ24AbXNDVExTaWduAHJzYV9wc3Nfc2FsdGxlbgByc2FfcHNzX2tleWdlbl9zYWx0bGVuAGlkLXNtaW1lLWFhLXRpbWVTdGFtcFRva2VuAGlkLXJlZ0N0cmwtcmVnVG9rZW4Ac2V0Y3QtUEFOVG9rZW4ARFNUVSA0MTQ1LTIwMDIgYmlnIGVuZGlhbgBEU1RVIDQxNDUtMjAwMiBsaXR0bGUgZW5kaWFuAGNvdW50cnlDb2RlM24AcHNldWRvbnltAE1pbmltdW0Ac2JncC1hdXRvbm9tb3VzU3lzTnVtAGlkLWl0LWltcGxpY2l0Q29uZmlybQBzZXRBdHRyLUdlbkNyeXB0Z3JtAHJvb20AaWQtY21jLXBvcExpbmtSYW5kb20AY3J5cHRvY29tAEdPU1QgUiAzNDEwLTIwMDEgUGFyYW1ldGVyIFNldCBDcnlwdG9jb20AR09TVCAzNC4xMC05NCBDcnlwdG9jb20AR09TVCBSIDM0LjExLTk0IHdpdGggR09TVCBSIDM0LjEwLTk0IENyeXB0b2NvbQBHT1NUIDM0LjEwLTIwMDEgQ3J5cHRvY29tAEdPU1QgUiAzNC4xMS05NCB3aXRoIEdPU1QgUiAzNC4xMC0yMDAxIENyeXB0b2NvbQBtc0NvZGVDb20AaWQtdGMyNi1jaXBoZXItZ29zdHIzNDEyLTIwMTUta3V6bnllY2hpay1jdHJhY3BrbQBpZC10YzI2LWNpcGhlci1nb3N0cjM0MTItMjAxNS1tYWdtYS1jdHJhY3BrbQBhbGdvcml0aG0AbWFza0dlbkFsZ29yaXRobQBoYXNoQWxnb3JpdGhtAGlwc2VjRW5kU3lzdGVtAE1pY3Jvc29mdCBFbmNyeXB0ZWQgRmlsZSBTeXN0ZW0ASVBTZWMgRW5kIFN5c3RlbQBhZXMtMTI4LWdjbQBhcmlhLTEyOC1nY20AY2FtZWxsaWEtMTI4LWdjbQBhZXMtMjU2LWdjbQBhcmlhLTI1Ni1nY20AY2FtZWxsaWEtMjU2LWdjbQBhZXMtMTkyLWdjbQBhcmlhLTE5Mi1nY20AY2FtZWxsaWEtMTkyLWdjbQBhZXMtMTI4LWNjbQBhcmlhLTEyOC1jY20AY2FtZWxsaWEtMTI4LWNjbQBhZXMtMjU2LWNjbQBhcmlhLTI1Ni1jY20AY2FtZWxsaWEtMjU2LWNjbQBhZXMtMTkyLWNjbQBhcmlhLTE5Mi1jY20AY2FtZWxsaWEtMTkyLWNjbQBYOWNtAGdlbmVyYXRlIGNyeXB0b2dyYW0AZ2VuZXJpYyBjcnlwdG9ncmFtAC91c3IvbG9jYWwvc3NsAGlkLXJlZ0N0cmwAbnNDYVBvbGljeVVybABuc1Jldm9jYXRpb25VcmwAbnNDYVJldm9jYXRpb25VcmwAbnNSZW5ld2FsVXJsAG5zQmFzZVVybABOZXRzY2FwZSBDQSBQb2xpY3kgVXJsAE5ldHNjYXBlIFJldm9jYXRpb24gVXJsAE5ldHNjYXBlIENBIFJldm9jYXRpb24gVXJsAE5ldHNjYXBlIFJlbmV3YWwgVXJsAE5ldHNjYXBlIEJhc2UgVXJsAHg1MDlDcmwAaWQtcHBsAHdoaXJscG9vbABzdWJqZWN0U2lnblRvb2wAaXNzdWVyU2lnblRvb2wAaWQtbW9kLXRpbWVzdGFtcC1wcm90b2NvbABpZC1jdC14bWwAYXV0aC1udWxsAEluaGVyaXQgYWxsAGlkLXBwbC1pbmhlcml0QWxsAG5vUmV2QXZhaWwAbWFpbABNYWlsAGlwc2VjVHVubmVsAElQU2VjIFR1bm5lbAByc2Ffb2FlcF9sYWJlbABpZC1zbWltZS1hYS1zZWN1cml0eUxhYmVsAHVjbABpZC1zbWltZS1jdGktZXRzLXByb29mT2ZBcHByb3ZhbABleHBlcmltZW50YWwARXhwZXJpbWVudGFsAGt4LXBzawBhdXRoLXBzawBreC1lY2RoZS1wc2sAa3gtZGhlLXBzawBreC1yc2EtcHNrAG5vbWFzawBmYXZvdXJpdGVEcmluawBpZC10YzI2LWNpcGhlci1nb3N0cjM0MTItMjAxNS1rdXpueWVjaGlrAGlkLXRjMjYtd3JhcC1nb3N0cjM0MTItMjAxNS1rdXpueWVjaGlrAHNiZ3AtaXBBZGRyQmxvY2sAbm9DaGVjawBPQ1NQIE5vIENoZWNrAGlkLXNtaW1lLWN0aQByc2Fkc2kAaWQtc21pbWUtc3BxLWV0cy1zcXQtdXJpAHVhLXBraQBtZXJjaGFudCBpbml0aWF0ZWQgYXV0aABjbGllbnRBdXRoAHBrSW5pdENsaWVudEF1dGgAc2VydmVyQXV0aABzZXRleHQtbWlBdXRoAFBLSU5JVCBDbGllbnQgQXV0aABpZC1wZGEtZGF0ZU9mQmlydGgAaWQtcGRhLXBsYWNlT2ZCaXJ0aABzYWx0TGVuZ3RoAHBhdGgAT1BFTlNTTF9maW5pc2gAc2lwaGFzaABHT1NUIFIgMzQuMTEtMjAxMiB3aXRoIDI1NiBiaXQgaGFzaABHT1NUIFIgMzQuMTEtMjAxMiB3aXRoIDUxMiBiaXQgaGFzaABTaXBIYXNoAGllZWUtc2lzd2cAd2FwLXdzZwBzZXRjdC1BY3FDYXJkQ29kZU1zZwBvcmcAYWMtcHJveHlpbmcAYWMtdGFyZ2V0aW5nAFg1MDl2MyBBQyBUYXJnZXRpbmcAR2VuZXJhbFN0cmluZwBVVEY4U3RyaW5nAGFkX3RpbWVzdGFtcGluZwB0aW1lU3RhbXBpbmcAQUQgVGltZSBTdGFtcGluZwBjb2RlU2lnbmluZwBPQ1NQU2lnbmluZwBNaWNyb3NvZnQgVHJ1c3QgTGlzdCBTaWduaW5nAE1pY3Jvc29mdCBJbmRpdmlkdWFsIENvZGUgU2lnbmluZwBNaWNyb3NvZnQgQ29tbWVyY2lhbCBDb2RlIFNpZ25pbmcAT0NTUCBTaWduaW5nAHNldENleHQtdHVubmVsaW5nAGlkLUdvc3QyODE0Ny04OS1DcnlwdG9Qcm8tS2V5TWVzaGluZwBpZC1Hb3N0MjgxNDctODktTm9uZS1LZXlNZXNoaW5nAGlkLWNtYy1xdWVyeVBlbmRpbmcAaWQtc21pbWUtYWxnAGlkLWFsZwBpZC1pdC1wcmVmZXJyZWRTeW1tQWxnAHNldEF0dHItVG9rSUNDc2lnAHNldEF0dHItU2VjRGV2U2lnAHNldEF0dHItSXNzQ2FwLVNpZwBrZXlCYWcAcGtjczhTaHJvdWRlZEtleUJhZwBjZXJ0QmFnAHNlY3JldEJhZwBzYWZlQ29udGVudHNCYWcAY3JsQmFnAHRsczEtcHJmAGlkLWNtYy1pZGVudGl0eVByb29mAG9wZW5zc2xfY29uZgBvcGVuc3NsLmNuZgBpZC1tb2QtY3JtZgBkZXMtY2RtZgBzZXRDZXh0LXNldFF1YWxmAGFyY2hpdmVDdXRvZmYAT0NTUCBBcmNoaXZlIEN1dG9mZgBpZC1zbWltZS1hYS1lbmNyeXBLZXlQcmVmADAxMjM0NTY3ODlhYmNkZWYAaGtkZgBkaC1jb2ZhY3Rvci1rZGYAZGgtc3RkLWtkZgBkaWdlc3RzaXplAGFzc2VydGlvbiBmYWlsZWQ6IGxpc3QgPj0gMCAmJiBsaXN0IDwgc2guZnJlZWxpc3Rfc2l6ZQBhc3NlcnRpb24gZmFpbGVkOiBiaXQgPiAwICYmIGJpdCA8IHNoLmJpdHRhYmxlX3NpemUAdHJ1ZQBpZC1zbWltZS1hYS1tYWNWYWx1ZQBwcml2YXRlAFByaXZhdGUAdXNlckNlcnRpZmljYXRlAHNkc2lDZXJ0aWZpY2F0ZQBpZC1zbWltZS1hYS1zaWduaW5nQ2VydGlmaWNhdGUAY0FDZXJ0aWZpY2F0ZQB4NTA5Q2VydGlmaWNhdGUAaW52YWxpZGl0eURhdGUASW52YWxpZGl0eSBEYXRlAGlkLWl0LXN1YnNjcmlwdGlvblJlc3BvbnNlAGJhc2ljT0NTUFJlc3BvbnNlAGlkLWNjdC1QS0lSZXNwb25zZQBCYXNpYyBPQ1NQIFJlc3BvbnNlAFNpZ25pbmcgS0RDIFJlc3BvbnNlAGZhbHNlAGlkLWl0LXJldlBhc3NwaHJhc2UAY291bnRlcnNpZ25hdHVyZQBJQ0Mgb3IgdG9rZW4gc2lnbmF0dXJlAHNlY3VyZSBkZXZpY2Ugc2lnbmF0dXJlAGlkLWFsZy1ub1NpZ25hdHVyZQBwZXJzb25hbFNpZ25hdHVyZQByc2FTaWduYXR1cmUAdGxzZmVhdHVyZQBUTFMgRmVhdHVyZQBzZXRleHQtcGluU2VjdXJlAHNldC1jdHlwZQBzZXRDZXh0LWNlcnRUeXBlAG5zQ2VydFR5cGUAY29udGVudFR5cGUAaWQtc21pbWUtYWEtZW5jYXBDb250ZW50VHlwZQBpZC1zbWltZS1hYS1ldHMtY29tbWl0bWVudFR5cGUAc2V0Q2V4dC1Ub2tlblR5cGUAc2V0QXR0ci1Ub2tlblR5cGUAcGlsb3RBdHRyaWJ1dGVUeXBlAGlkLXNtaW1lLWFhLXNpZ25hdHVyZVR5cGUAbnNEYXRhVHlwZQBOZXRzY2FwZSBDZXJ0IFR5cGUATmV0c2NhcGUgRGF0YSBUeXBlAE5ldHNjYXBlAGlkLXBlAG5vbmUAaG9sZEluc3RydWN0aW9uTm9uZQBIb2xkIEluc3RydWN0aW9uIE5vbmUAc2V0QXR0ci1Ub2tlbi1CMFByaW1lAGlkLWl0LWNvbmZpcm1XYWl0VGltZQBzaWduaW5nVGltZQBsYXN0TW9kaWZpZWRUaW1lAHNtLXNjaGVtZQBkaFNpbmdsZVBhc3MtY29mYWN0b3JESC1zaGEyNTZrZGYtc2NoZW1lAGRoU2luZ2xlUGFzcy1zdGRESC1zaGEyNTZrZGYtc2NoZW1lAGRoU2luZ2xlUGFzcy1jb2ZhY3RvckRILXNoYTM4NGtkZi1zY2hlbWUAZGhTaW5nbGVQYXNzLXN0ZERILXNoYTM4NGtkZi1zY2hlbWUAZGhTaW5nbGVQYXNzLWNvZmFjdG9yREgtc2hhMjI0a2RmLXNjaGVtZQBkaFNpbmdsZVBhc3Mtc3RkREgtc2hhMjI0a2RmLXNjaGVtZQBkaFNpbmdsZVBhc3MtY29mYWN0b3JESC1zaGE1MTJrZGYtc2NoZW1lAGRoU2luZ2xlUGFzcy1zdGRESC1zaGE1MTJrZGYtc2NoZW1lAGRoU2luZ2xlUGFzcy1jb2ZhY3RvckRILXNoYTFrZGYtc2NoZW1lAGRoU2luZ2xlUGFzcy1zdGRESC1zaGExa2RmLXNjaGVtZQBzdXJuYW1lAGxvY2FsaXR5TmFtZQBqdXJpc2RpY3Rpb25Mb2NhbGl0eU5hbWUAY291bnRyeU5hbWUAZnJpZW5kbHlDb3VudHJ5TmFtZQBqdXJpc2RpY3Rpb25Db3VudHJ5TmFtZQBmcmllbmRseU5hbWUAc3ViamVjdEFsdE5hbWUAaXNzdWVyQWx0TmFtZQBvcmdhbml6YXRpb25hbFVuaXROYW1lAGRuc05hbWUAbnNTc2xTZXJ2ZXJOYW1lAGNvbW1vbk5hbWUAb3JnYW5pemF0aW9uTmFtZQBnaXZlbk5hbWUAYnVpbGRpbmdOYW1lAHN0YXRlT3JQcm92aW5jZU5hbWUAanVyaXNkaWN0aW9uU3RhdGVPclByb3ZpbmNlTmFtZQBwaHlzaWNhbERlbGl2ZXJ5T2ZmaWNlTmFtZQBkbWROYW1lAGFzc29jaWF0ZWROYW1lAHVuc3RydWN0dXJlZE5hbWUAZGlzdGluZ3Vpc2hlZE5hbWUAQ1NQTmFtZQBOZXRzY2FwZSBTU0wgU2VydmVyIE5hbWUATWljcm9zb2Z0IFVzZXIgUHJpbmNpcGFsIE5hbWUAWDUwOXYzIFN1YmplY3QgQWx0ZXJuYXRpdmUgTmFtZQBYNTA5djMgSXNzdWVyIEFsdGVybmF0aXZlIE5hbWUATWljcm9zb2Z0IENTUCBOYW1lAHRpdGxlAGRvY3VtZW50VGl0bGUAcGVyc29uYWxUaXRsZQBpZC1hY2Etcm9sZQBYNTA5djMgTm8gUmV2b2NhdGlvbiBBdmFpbGFibGUAZHN0dTQxNDVsZQBreC1lY2RoZQBreC1kaGUAaXBzZWMgSW50ZXJuZXQgS2V5IEV4Y2hhbmdlAEFueSBsYW5ndWFnZQBpZC1wcGwtYW55TGFuZ3VhZ2UAaWQtaGV4LW11bHRpcGFydC1tZXNzYWdlAGlkLWhleC1wYXJ0aWFsLW1lc3NhZ2UAaWQtaXQtb3JpZ1BLSU1lc3NhZ2UAa2V5VXNhZ2UAZXh0ZW5kZWRLZXlVc2FnZQBhbnlFeHRlbmRlZEtleVVzYWdlAEFueSBFeHRlbmRlZCBLZXkgVXNhZ2UAWDUwOXYzIEV4dGVuZGVkIEtleSBVc2FnZQBYNTA5djMgS2V5IFVzYWdlAHNldGN0LUF1dGhSZXZSZXNCYWdnYWdlAHNldGN0LUF1dGhSZXNCYWdnYWdlAHNldGN0LUF1dGhSZXZSZXFCYWdnYWdlAGllZWUALmluY2x1ZGUAZmlwc19tb2RlAHJzYV9wYWRkaW5nX21vZGUARFNUVSBHb3N0IDI4MTQ3LTIwMDkgT0ZCIG1vZGUARFNUVSBHb3N0IDI4MTQ3LTIwMDkgQ0ZCIG1vZGUAaG9sZEluc3RydWN0aW9uQ29kZQBwb3N0YWxDb2RlAFg1MDl2MyBDUkwgUmVhc29uIENvZGUASG9sZCBJbnN0cnVjdGlvbiBDb2RlAHNlYXJjaEd1aWRlAGVuaGFuY2VkU2VhcmNoR3VpZGUAZGVzLWVkZQBpZC1jbWMtcmVjaXBpZW50Tm9uY2UAaWQtY21jLXNlbmRlck5vbmNlAE9DU1AgTm9uY2UAbnNDZXJ0U2VxdWVuY2UATmV0c2NhcGUgQ2VydGlmaWNhdGUgU2VxdWVuY2UAaWQtc21pbWUtYWEtY29udGVudFJlZmVyZW5jZQBpZC1wZGEtY291bnRyeU9mUmVzaWRlbmNlAGlkLWNtYy1jb25maXJtQ2VydEFjY2VwdGFuY2UAY2xlYXJhbmNlAGlkLXNtaW1lLXNwcS1ldHMtc3F0LXVub3RpY2UAaWQtcXQtdW5vdGljZQB0ZXh0Tm90aWNlAFBvbGljeSBRdWFsaWZpZXIgVXNlciBOb3RpY2UAaWQtY2UAZHN0dTQxNDViZQB1c2VyUGFzc3dvcmQAY2hhbGxlbmdlUGFzc3dvcmQAYVJlY29yZABtWFJlY29yZABuU1JlY29yZABjTkFNRVJlY29yZABzT0FSZWNvcmQAc2V0LWJyYW5kLU1hc3RlckNhcmQAaWQtcGtpeC1tb2QAaWQtc21pbWUtbW9kAHByaXZhdGVLZXlVc2FnZVBlcmlvZABYNTA5djMgUHJpdmF0ZSBLZXkgVXNhZ2UgUGVyaW9kAE5VTEwgc2hhcmVkIGxpYnJhcnkgbWV0aG9kAE9wZW5TU0wgUlNBLVBTUyBtZXRob2QAT3BlblNTTCBTSVBIQVNIIG1ldGhvZABPcGVuU1NMIEhNQUMgbWV0aG9kAE9wZW5TU0wgUlNBIG1ldGhvZABwcmVmZXJyZWREZWxpdmVyeU1ldGhvZABkb2QAc2V0LWJyYW5kAG1zQ29kZUluZAByc2Ffb2FlcF9tZAByc2FfcHNzX2tleWdlbl9tZAByc2FfcHNzX2tleWdlbl9tZ2YxX21kAHJzYV9tZ2YxX21kAGRzYUVuY3J5cHRpb24tb2xkAERTQS1vbGQAZHNhV2l0aFNIQTEtb2xkAERTQS1TSEExLW9sZABjaGFyYWN0ZXJpc3RpYy10d28tZmllbGQAcHJpbWUtZmllbGQAdHJhaWxlckZpZWxkACVsZAB1aWQAaWQtc21pbWUtbW9kLW9pZAB2YWxpZABzZXRDZXh0LWNDZXJ0UmVxdWlyZWQAdW5kZWZpbmVkAHNldGN0LVBJRGF0YVVuc2lnbmVkAHBTcGVjaWZpZWQAZWNkc2Etd2l0aC1TcGVjaWZpZWQAaGV4c2VlZABlY2RzYS13aXRoLVJlY29tbWVuZGVkAGlkLXNtaW1lLWNkAGlkLWFlczEyOC13cmFwLXBhZABpZC1hZXMyNTYtd3JhcC1wYWQAaWQtYWVzMTkyLXdyYXAtcGFkAGlkLWFkAGlkLXNtaW1lLWFhLWV0cy1zaWdQb2xpY3lJZAB1c2VySWQAaWQtY21jLXRyYW5zYWN0aW9uSWQAJS04ZABpZC1zbWltZS1hYS1kdmNzLWR2YwBjZXJ0aWNvbS1hcmMAc2V0QXR0ci1UMkVuYwBpZC1tb2QtY21jAGlkLWNtYwBnb3N0OTRjYwBnb3N0MjAwMWNjAGlkLUdvc3RSMzQxMC0yMDAxLVBhcmFtU2V0LWNjAGlkLUdvc3QyODE0Ny04OS1jYwBpZC1Hb3N0UjM0MTEtOTQtd2l0aC1Hb3N0UjM0MTAtOTQtY2MAaWQtR29zdFIzNDExLTk0LXdpdGgtR29zdFIzNDEwLTIwMDEtY2MAZGVzeC1jYmMAZGVzLWNiYwBncmFzc2hvcHBlci1jYmMAYmYtY2JjAGRlcy1lZGUtY2JjAHNlZWQtY2JjAG1hZ21hLWNiYwBpZGVhLWNiYwBnb3N0ODktY2JjAGFlcy0xMjgtY2JjAGFyaWEtMTI4LWNiYwBjYW1lbGxpYS0xMjgtY2JjAGFlcy0yNTYtY2JjAGFyaWEtMjU2LWNiYwBjYW1lbGxpYS0yNTYtY2JjAGNhc3Q1LWNiYwByYzUtY2JjAHNtNC1jYmMAcmMyLTY0LWNiYwBkZXMtZWRlMy1jYmMAcmMyLWNiYwBhZXMtMTkyLWNiYwBhcmlhLTE5Mi1jYmMAY2FtZWxsaWEtMTkyLWNiYwByYzItNDAtY2JjAGlkLXRjMjYtY2lwaGVyLWdvc3RyMzQxMi0yMDE1LWt1em55ZWNoaWstY3RyYWNwa20tb21hYwBpZC10YzI2LWNpcGhlci1nb3N0cjM0MTItMjAxNS1tYWdtYS1jdHJhY3BrbS1vbWFjAGhtYWMAY2FtZWxsaWEtMTI4LWNtYWMAY2FtZWxsaWEtMjU2LWNtYWMAY2FtZWxsaWEtMTkyLWNtYWMAZ29zdC1tYWMAZ3Jhc3Nob3BwZXItbWFjAG1hZ21hLW1hYwBpZC10YzI2LW1hYwBpZC1ESEJhc2VkTWFjAGNvdW50cnlDb2RlM2MAJTAyeCVjAHJiAHNldC1yb290S2V5VGh1bWIAZGVzLW9mYgBncmFzc2hvcHBlci1vZmIAYmYtb2ZiAGRlcy1lZGUtb2ZiAHNlZWQtb2ZiAG1hZ21hLW9mYgBpZGVhLW9mYgBhZXMtMTI4LW9mYgBhcmlhLTEyOC1vZmIAY2FtZWxsaWEtMTI4LW9mYgBkc3R1MjgxNDctb2ZiAGFlcy0yNTYtb2ZiAGFyaWEtMjU2LW9mYgBjYW1lbGxpYS0yNTYtb2ZiAGNhc3Q1LW9mYgByYzUtb2ZiAHNtNC1vZmIAZGVzLWVkZTMtb2ZiAHJjMi1vZmIAYWVzLTE5Mi1vZmIAYXJpYS0xOTItb2ZiAGNhbWVsbGlhLTE5Mi1vZmIAZGVzLWNmYgBncmFzc2hvcHBlci1jZmIAYmYtY2ZiAGRlcy1lZGUtY2ZiAHNlZWQtY2ZiAG1hZ21hLWNmYgBpZGVhLWNmYgBhZXMtMTI4LWNmYgBhcmlhLTEyOC1jZmIAY2FtZWxsaWEtMTI4LWNmYgBkc3R1MjgxNDctY2ZiAGFlcy0yNTYtY2ZiAGFyaWEtMjU2LWNmYgBjYW1lbGxpYS0yNTYtY2ZiAGNhc3Q1LWNmYgByYzUtY2ZiAHNtNC1jZmIAZGVzLWVkZTMtY2ZiAHJjMi1jZmIAYWVzLTE5Mi1jZmIAYXJpYS0xOTItY2ZiAGNhbWVsbGlhLTE5Mi1jZmIAYWVzLTEyOC1vY2IAYWVzLTI1Ni1vY2IAYWVzLTE5Mi1vY2IAZGVzLWVjYgBncmFzc2hvcHBlci1lY2IAYmYtZWNiAHNlZWQtZWNiAG1hZ21hLWVjYgBpZGVhLWVjYgBnb3N0ODktZWNiAGFlcy0xMjgtZWNiAGFyaWEtMTI4LWVjYgBjYW1lbGxpYS0xMjgtZWNiAGFlcy0yNTYtZWNiAGFyaWEtMjU2LWVjYgBjYW1lbGxpYS0yNTYtZWNiAGNhc3Q1LWVjYgByYzUtZWNiAHNtNC1lY2IAcmMyLWVjYgBhZXMtMTkyLWVjYgBhcmlhLTE5Mi1lY2IAY2FtZWxsaWEtMTkyLWVjYgBpZC1Hb3N0UjM0MTAtOTQtYgBwa2NzNy1kYXRhAGlkLXNtaW1lLWN0LURWQ1NSZXF1ZXN0RGF0YQBwa2NzNy1kaWdlc3REYXRhAHNldGN0LUNhcFJldlJlc0RhdGEAc2V0Y3QtQXV0aFJldlJlc0RhdGEAc2V0Y3QtQ3JlZFJldlJlc0RhdGEAc2V0Y3QtQ2VydFJlc0RhdGEAc2V0Y3QtUEluaXRSZXNEYXRhAHNldGN0LUNhcFJlc0RhdGEAc2V0Y3QtQmF0Y2hBZG1pblJlc0RhdGEAc2V0Y3QtQ3JlZFJlc0RhdGEAc2V0Y3QtUFJlc0RhdGEAc2V0Y3QtUENlcnRSZXFEYXRhAHNldGN0LUNlcnRSZXFEYXRhAHNldGN0LUJhdGNoQWRtaW5SZXFEYXRhAHNldGN0LUNhcFRva2VuRGF0YQBpZC1vbi1wZXJzb25hbERhdGEAaWQtc21pbWUtY3QtYXV0aERhdGEAc2V0Q2V4dC1tZXJjaERhdGEAaWQtc21pbWUtY3QtRFZDU1Jlc3BvbnNlRGF0YQBwa2NzNy1lbmNyeXB0ZWREYXRhAGlkLXNtaW1lLWN0LWNvbXByZXNzZWREYXRhAHBrY3M3LWVudmVsb3BlZERhdGEAaWQtc21pbWUtY3QtYXV0aEVudmVsb3BlZERhdGEAcGtjczctc2lnbmVkQW5kRW52ZWxvcGVkRGF0YQBwa2NzNy1zaWduZWREYXRhAHF1YWxpdHlMYWJlbGxlZERhdGEAc2V0Y3QtUEFORGF0YQBzZXRjdC1QSURhdGEAc2V0Y3QtT0lEYXRhAGlkLWNjdC1QS0lEYXRhAHNldENleHQtVHJhY2syRGF0YQBreC1yc2EAYXV0aC1yc2EAa2lzYQBzZXQtYnJhbmQtVmlzYQBhdXRoLWVjZHNhAGlhbmEAaWQtdGMyNi1jaXBoZXItZ29zdHIzNDEyLTIwMTUtbWFnbWEAaWQtdGMyNi13cmFwLWdvc3RyMzQxMi0yMDE1LW1hZ21hAHNoYQBpZC1wZGEAb3NjY2EAaWQtYWNhAGlkLXNtaW1lLWFhAGlkLUdvc3RSMzQxMC05NC1hAGFzc2VydGlvbiBmYWlsZWQ6IHRlbXAgIT0gc2guZnJlZWxpc3Rbc2xpc3RdAGlkLXRjMjYtZ29zdC0yODE0Ny1wYXJhbS1aAEt4QU5ZAEF1dGhBTlkAQVNOMV9BTlkARVhUUkFDVF9PTkxZAEVYUEFORF9PTkxZAFg1MDlfUFVCS0VZAHNldGN0LUF1dGhSZXNUQlNYAHNldGN0LUNhcFJldlJlcVRCU1gAc2V0Y3QtQ3JlZFJldlJlcVRCU1gAc2V0Y3QtQ2FwUmVxVEJTWABzZXRjdC1DcmVkUmVxVEJTWABQS0lYAHNldGN0LUF1dGhSZXNUQkVYAHNldGN0LUNhcFJldlJlcVRCRVgAc2V0Y3QtQ3JlZFJldlJlcVRCRVgAc2V0Y3QtQ2VydFJlcVRCRVgAc2V0Y3QtQ2FwUmVxVEJFWABzZXRjdC1DcmVkUmVxVEJFWABzZXRjdC1DYXBUb2tlblRCRVgARU5WAHNldEF0dHItVG9rZW4tRU1WAFVOSVYAT1UAanVyaXNkaWN0aW9uU1QAS3hHT1NUAElOVABFWFBMSUNJVABJTVBMSUNJVAByc2FPQUVQRW5jcnlwdGlvblNFVABPQ1QAQVNOMV9PQkpFQ1QARk9STUFUAEpPSU5ULUlTTy1JVFUtVABJU08tVVMAQUVTLTEyOC1YVFMAQUVTLTI1Ni1YVFMAUlNBU1NBLVBTUwBSU0EtUFNTAEF1dGhEU1MAUG9saWN5IFF1YWxpZmllciBDUFMAU01JTUUtQ0FQUwBSU0FfUFNTX1BBUkFNUwBTTklMUwBNSU1FIE1IUwBtc0VGUwBZRVMAaWQtc21pbWUtYWxnLUVTREh3aXRoM0RFUwBQQkUtU0hBMS0zREVTAFBCRS1TSEExLTJERVMAUEJFLU1ENS1ERVMAUEJFLU1EMi1ERVMAUEJFLVNIQTEtREVTAEFEX0RWQ1MAUlNBIERhdGEgU2VjdXJpdHksIEluYy4gUEtDUwBzZXRjdC1BdXRoUmV2UmVzVEJTAHNldGN0LVBDZXJ0UmVzVEJTAHNldGN0LU1lQXFDSW5pdFJlc1RCUwBzZXRjdC1DYXJkQ0luaXRSZXNUQlMAc2V0Y3QtQ1JMTm90aWZpY2F0aW9uUmVzVEJTAHNldGN0LVJlZ0Zvcm1SZXNUQlMAc2V0Y3QtQXV0aFJlc1RCUwBzZXRjdC1FcnJvclRCUwBzZXRjdC1DYXBSZXZSZXFUQlMAc2V0Y3QtQXV0aFJldlJlcVRCUwBzZXRjdC1DcmVkUmV2UmVxVEJTAHNldGN0LUNlcnRSZXFUQlMAc2V0Y3QtQ2VydElucVJlcVRCUwBzZXRjdC1DYXBSZXFUQlMAc2V0Y3QtQXV0aFJlcVRCUwBzZXRjdC1DcmVkUmVxVEJTAHNldGN0LUJDSURpc3RyaWJ1dGlvblRCUwBzZXRjdC1DUkxOb3RpZmljYXRpb25UQlMAc2V0Y3QtQ2FwVG9rZW5UQlMAc2V0Y3QtQXV0aFRva2VuVEJTAHNldGN0LVBJLVRCUwBCSVRTVFIAR0VOU1RSAEFFUy0xMjgtQ1RSAEFSSUEtMTI4LUNUUgBDQU1FTExJQS0xMjgtQ1RSAEFFUy0yNTYtQ1RSAEFSSUEtMjU2LUNUUgBDQU1FTExJQS0yNTYtQ1RSAFNNNC1DVFIAQUVTLTE5Mi1DVFIAQVJJQS0xOTItQ1RSAENBTUVMTElBLTE5Mi1DVFIAWDUwOV9BTEdPUgBESVIAQVNOMV9JTlRFR0VSAFNFUQBFWFAAY2Fwd2FwV1RQAE9DU1AAS3hTUlAAQXV0aFNSUABpZC1jbWMtZW5jcnlwdGVkUE9QAGlkLWNtYy1kZWNyeXB0ZWRQT1AASU1QAEJNUABSU0FFUy1PQUVQAEJJVFdSQVAAU0VUV1JBUABPQ1RXUkFQAFNFUVdSQVAASVNPAE5PAFJTQV9QUklNRV9JTkZPAFNOAE9HUk4AbXNVUE4ASU5OAEdOAElTTy1DTgBCT09MRUFOAHNldEF0dHItSXNzQ2FwLUNWTQBDQklHTlVNAEVOVU0ARk9STQBpZC1hZXMxMjgtR0NNAEFSSUEtMTI4LUdDTQBDQU1FTExJQS0xMjgtR0NNAGlkLWFlczI1Ni1HQ00AQVJJQS0yNTYtR0NNAENBTUVMTElBLTI1Ni1HQ00AaWQtYWVzMTkyLUdDTQBBUklBLTE5Mi1HQ00AQ0FNRUxMSUEtMTkyLUdDTQBpZC1hZXMxMjgtQ0NNAEFSSUEtMTI4LUNDTQBDQU1FTExJQS0xMjgtQ0NNAGlkLWFlczI1Ni1DQ00AQVJJQS0yNTYtQ0NNAENBTUVMTElBLTI1Ni1DQ00AaWQtYWVzMTkyLUNDTQBBUklBLTE5Mi1DQ00AQ0FNRUxMSUEtMTkyLUNDTQBqdXJpc2RpY3Rpb25MAGZyZXNoZXN0Q1JMAGlkLWl0LWN1cnJlbnRDUkwAaWQtY21jLWdldENSTABkZWx0YUNSTABYNTA5djMgRnJlc2hlc3QgQ1JMAEJPT0wAQXV0aE5VTEwAS3hQU0sAQXV0aFBTSwBLeFJTQV9QU0sAS3hESEUtUFNLAEt4RUNESEUtUFNLAGlkLWFsZy1QV1JJLUtFSwBzZXRjdC1QSQBTSVBIQVNIAGlkLXNtaW1lLWFsZy1FU0RIAGlkLUdvc3RSMzQxMC05NERIAGlkLUdvc3RSMzQxMC0yMDAxREgAR09TVCBSIDM0LjEwLTk0IERIAFg5LjQyIERIAEdPU1QgUiAzNC4xMC0yMDAxIERIAE9SRwBBU04xX0JJVF9TVFJJTkcAQVNOMV9PQ1RFVF9TVFJJTkcAVEVMRVRFWFNUUklORwBCSVRTVFJJTkcAT0NURVRTVFJJTkcAQk1QU1RSSU5HAFVOSVZFUlNBTFNUUklORwBWSVNJQkxFU1RSSU5HAFBSSU5UQUJMRVNUUklORwBOVU1FUklDU1RSSU5HAElBNVNUUklORwBUNjFTVFJJTkcAWDUwOV9TSUcAVExTMS1QUkYAR09TVCBSIDM0LjExLTk0IFBSRgBPUEVOU1NMX0NPTkYAREVTLUNETUYAaWQtY3QtYXNjaWlUZXh0V2l0aENSTEYAVU5ERUYAMDEyMzQ1Njc4OUFCQ0RFRgBIS0RGAGFzc2VydGlvbiBmYWlsZWQ6IGN0eC0+ZGlnZXN0LT5tZF9zaXplIDw9IEVWUF9NQVhfTURfU0laRQBUUlVFAFg1MDlfQVRUUklCVVRFAEZBTFNFAEdFTlRJTUUAR0VORVJBTElaRURUSU1FAFVUQ1RJTUUAU01JTUUAUy9NSU1FAFZJU0lCTEUAUFJJTlRBQkxFAGlwc2VjSUtFAEt4REhFAEt4RUNESEUAREVTLUVERQBTRVFVRU5DRQBzZXRjdC1DYXBSZXZSZXNUQkUAc2V0Y3QtQXV0aFJldlJlc1RCRQBzZXRjdC1DcmVkUmV2UmVzVEJFAHNldGN0LUNlcnRSZXNUQkUAc2V0Y3QtQ2FwUmVzVEJFAHNldGN0LUJhdGNoQWRtaW5SZXNUQkUAc2V0Y3QtQXV0aFJlc1RCRQBzZXRjdC1DcmVkUmVzVEJFAHNldGN0LUNhcFJldlJlcVRCRQBzZXRjdC1BdXRoUmV2UmVxVEJFAHNldGN0LUNyZWRSZXZSZXFUQkUAc2V0Y3QtQ2VydFJlcVRCRQBzZXRjdC1DYXBSZXFUQkUAc2V0Y3QtQmF0Y2hBZG1pblJlcVRCRQBzZXRjdC1SZWdGb3JtUmVxVEJFAHNldGN0LUF1dGhSZXFUQkUAc2V0Y3QtQ3JlZFJlcVRCRQBzZXRjdC1DYXBUb2tlblRCRQBzZXRjdC1BdXRoVG9rZW5UQkUAc2V0Y3QtQWNxQ2FyZENvZGVNc2dUQkUAc2V0Y3QtUElVbnNpZ25lZFRCRQBzZXRjdC1QSUR1YWxTaWduZWRUQkUAaWQtdGMyNi1nb3N0LTM0MTAtMjAxMi0yNTYtcGFyYW1TZXREAERPRABFWFRSQUNUX0FORF9FWFBBTkQAbG9jYWxLZXlJRABpZC1yZWdDdHJsLW9sZENlcnRJRABTWE5ldElEAENybElEAFVJRABPSUQASU5WQUxJRABTdHJvbmcgRXh0cmFuZXQgSUQAT0NTUCBDUkwgSUQARU5VTUVSQVRFRABQU1BFQ0lGSUVEAEdPU1QgUiAzNC4xMC0yMDEyICgyNTYgYml0KSBQYXJhbVNldCBEAGlkLXRjMjYtZ29zdC0zNDEwLTIwMTItMjU2LXBhcmFtU2V0QwBpZC10YzI2LWdvc3QtMzQxMC0yMDEyLTUxMi1wYXJhbVNldEMAanVyaXNkaWN0aW9uQwBVVEMATlVNRVJJQwBuc1NHQwBtc1NHQwBwa0luaXRLREMAcGJlV2l0aE1ENUFuZENhc3Q1Q0JDAERFU1gtQ0JDAHBiZVdpdGhTSEExQW5kMy1LZXlUcmlwbGVERVMtQ0JDAHBiZVdpdGhTSEExQW5kMi1LZXlUcmlwbGVERVMtQ0JDAHBiZVdpdGhNRDVBbmRERVMtQ0JDAHBiZVdpdGhNRDJBbmRERVMtQ0JDAHBiZVdpdGhTSEExQW5kREVTLUNCQwBCRi1DQkMAREVTLUVERS1DQkMAU0VFRC1DQkMASURFQS1DQkMAQUVTLTEyOC1DQkMAQVJJQS0xMjgtQ0JDAENBTUVMTElBLTEyOC1DQkMAQUVTLTI1Ni1DQkMAQVJJQS0yNTYtQ0JDAENBTUVMTElBLTI1Ni1DQkMAQ0FTVDUtQ0JDAFJDNS1DQkMAU000LUNCQwBSQzItNjQtQ0JDAERFUy1FREUzLUNCQwBwYmVXaXRoU0hBMUFuZDEyOEJpdFJDMi1DQkMAcGJlV2l0aFNIQTFBbmQ0MEJpdFJDMi1DQkMAcGJlV2l0aE1ENUFuZFJDMi1DQkMAcGJlV2l0aE1EMkFuZFJDMi1DQkMAcGJlV2l0aFNIQTFBbmRSQzItQ0JDAEFFUy0xOTItQ0JDAEFSSUEtMTkyLUNCQwBDQU1FTExJQS0xOTItQ0JDAFJDMi00MC1DQkMAY2Fwd2FwQUMAaWQtUGFzc3dvcmRCYXNlZE1BQwBITUFDAENBTUVMTElBLTEyOC1DTUFDAENBTUVMTElBLTI1Ni1DTUFDAENBTUVMTElBLTE5Mi1DTUFDAERpZmZpZS1IZWxsbWFuIGJhc2VkIE1BQwBwYXNzd29yZCBiYXNlZCBNQUMAR09TVCAyODE0Ny04OSBNQUMAR09TVCBSIDM0LjEwLTIwMTIgKDI1NiBiaXQpIFBhcmFtU2V0IEMAR09TVCBSIDM0LjEwLTIwMTIgKDUxMiBiaXQpIFBhcmFtU2V0IEMAaWQtdGMyNi1nb3N0LTM0MTAtMjAxMi0yNTYtcGFyYW1TZXRCAGlkLXRjMjYtZ29zdC0zNDEwLTIwMTItNTEyLXBhcmFtU2V0QgBaTElCAERFUy1PRkIAQkYtT0ZCAERFUy1FREUtT0ZCAFNFRUQtT0ZCAElERUEtT0ZCAEFFUy0xMjgtT0ZCAEFSSUEtMTI4LU9GQgBDQU1FTExJQS0xMjgtT0ZCAEFFUy0yNTYtT0ZCAEFSSUEtMjU2LU9GQgBDQU1FTExJQS0yNTYtT0ZCAENBU1Q1LU9GQgBSQzUtT0ZCAFNNNC1PRkIAREVTLUVERTMtT0ZCAFJDMi1PRkIAQUVTLTE5Mi1PRkIAQVJJQS0xOTItT0ZCAENBTUVMTElBLTE5Mi1PRkIAREVTLUNGQgBCRi1DRkIAREVTLUVERS1DRkIAU0VFRC1DRkIASURFQS1DRkIAQUVTLTEyOC1DRkIAQVJJQS0xMjgtQ0ZCAENBTUVMTElBLTEyOC1DRkIAQUVTLTI1Ni1DRkIAQVJJQS0yNTYtQ0ZCAENBTUVMTElBLTI1Ni1DRkIAQ0FTVDUtQ0ZCAFJDNS1DRkIAU000LUNGQgBERVMtRURFMy1DRkIAUkMyLUNGQgBBRVMtMTkyLUNGQgBBUklBLTE5Mi1DRkIAQ0FNRUxMSUEtMTkyLUNGQgBzZXRjdC1BdXRoUmV2UmVzVEJFQgBBRVMtMTI4LU9DQgBBRVMtMjU2LU9DQgBBRVMtMTkyLU9DQgBzZXQtYnJhbmQtSkNCAERFUy1FQ0IAQkYtRUNCAFNFRUQtRUNCAElERUEtRUNCAEFFUy0xMjgtRUNCAEFSSUEtMTI4LUVDQgBDQU1FTExJQS0xMjgtRUNCAEFFUy0yNTYtRUNCAEFSSUEtMjU2LUVDQgBDQU1FTExJQS0yNTYtRUNCAENBU1Q1LUVDQgBSQzUtRUNCAFNNNC1FQ0IAUkMyLUVDQgBBRVMtMTkyLUVDQgBBUklBLTE5Mi1FQ0IAQ0FNRUxMSUEtMTkyLUVDQgBHT1NUIFIgMzQuMTAtMjAxMiAoMjU2IGJpdCkgUGFyYW1TZXQgQgBHT1NUIFIgMzQuMTAtMjAxMiAoNTEyIGJpdCkgUGFyYW1TZXQgQgBpZC10YzI2LWdvc3QtMzQxMC0yMDEyLTI1Ni1wYXJhbVNldEEAaWQtdGMyNi1nb3N0LTM0MTAtMjAxMi01MTItcGFyYW1TZXRBAElTTy1VQQBzZXQtYnJhbmQtSUFUQS1BVEEAS3hSU0EAQXV0aFJTQQBtZDVXaXRoUlNBAG1kYzJXaXRoUlNBAHNoYTFXaXRoUlNBAHJpcGVtZDE2MFdpdGhSU0EAT3BlblNTTCBQS0NTIzEgUlNBAEtJU0EAcGlsb3REU0EAQXV0aEVDRFNBAGNtY1JBAElBTkEAZHNhV2l0aFNIQQBSU0EtU0hBAERTQS1TSEEAY21jQ0EAR09TVCBSIDM0LjEwLTIwMTIgKDI1NiBiaXQpIFBhcmFtU2V0IEEAR09TVCBSIDM0LjEwLTIwMTIgKDUxMiBiaXQpIFBhcmFtU2V0IEEAWDkuNTcgQ00gPwA8TlVMTD4APElOVkFMSUQ+AGdyb3VwPQBzZWN0aW9uPQAsIHBhdGg9ACwgdmFsdWU9ACwgVHlwZT0AIG5hbWU9AG1vZHVsZT0ALCByZXRjb2RlPQBmaWVsZD0ARmllbGQ9AHByaXZhdGVFeHBvbmVudDoAcHVibGljRXhwb25lbnQ6AGNvZWZmaWNpZW50OgBtb2R1bHVzOgBNb2R1bHVzOgBQU1MgcGFyYW1ldGVyIHJlc3RyaWN0aW9uczoAc2VjdGlvbjoALHZhbHVlOgAsbmFtZToAZXhwb25lbnQlZDoAY29lZmZpY2llbnQlZDoAcHJpbWUlZDoAZXhwb25lbnQyOgBwcmltZTI6AGV4cG9uZW50MToAcHJpbWUxOgB3YXAtd3NnLWlkbS1lY2lkLXd0bHM5AHBrY3M5AHVhY3VydmU5AGdvc3Q4OQAwMTIzNDU2Nzg5AEdPU1QgMjgxNDctODkAWDI1NTE5AEVEMjU1MTkAWDUwOQBEU1RVIEdvc3QgMjgxNDctMjAwOQBEU1RVIGN1cnZlIDkAd2FwLXdzZy1pZG0tZWNpZC13dGxzOAB1YWN1cnZlOABkZXMtY2ZiOABhZXMtMTI4LWNmYjgAYXJpYS0xMjgtY2ZiOABjYW1lbGxpYS0xMjgtY2ZiOABhZXMtMjU2LWNmYjgAYXJpYS0yNTYtY2ZiOABjYW1lbGxpYS0yNTYtY2ZiOABzbTQtY2ZiOABkZXMtZWRlMy1jZmI4AGFlcy0xOTItY2ZiOABhcmlhLTE5Mi1jZmI4AGNhbWVsbGlhLTE5Mi1jZmI4AFVURjgAREVTLUNGQjgAQUVTLTEyOC1DRkI4AEFSSUEtMTI4LUNGQjgAQ0FNRUxMSUEtMTI4LUNGQjgAQUVTLTI1Ni1DRkI4AEFSSUEtMjU2LUNGQjgAQ0FNRUxMSUEtMjU2LUNGQjgAU000LUNGQjgAREVTLUVERTMtQ0ZCOABBRVMtMTkyLUNGQjgAQVJJQS0xOTItQ0ZCOABDQU1FTExJQS0xOTItQ0ZCOABpZC1zbWltZS1tb2QtZXRzLWVTaWdQb2xpY3ktODgAaWQtbW9kLXF1YWxpZmllZC1jZXJ0LTg4AGlkLXBraXgxLWV4cGxpY2l0LTg4AGlkLXBraXgxLWltcGxpY2l0LTg4AGlkLXNtaW1lLW1vZC1ldHMtZVNpZ25hdHVyZS04OABpZC1tb2Qta2VhLXByb2ZpbGUtODgAWDQ0OABFRDQ0OABmZmRoZTIwNDgAc2hha2UxMjgAU0hBS0UxMjgAUEJFLVNIQTEtUkM0LTEyOABQQkUtU0hBMS1SQzItMTI4AERTVFUgY3VydmUgOAB3YXAtd3NnLWlkbS1lY2lkLXd0bHM3AHBrY3M3AHVhY3VydmU3AGlkLXNtaW1lLW1vZC1ldHMtZVNpZ1BvbGljeS05NwBpZC1zbWltZS1tb2QtZXRzLWVTaWduYXR1cmUtOTcAWDkuNTcAWDktNTcAZHN0dTI4MTQ3AHBpbG90QXR0cmlidXRlVHlwZTI3AERTVFUgY3VydmUgNwB3YXAtd3NnLWlkbS1lY2lkLXd0bHM2AHVhY3VydmU2AGZmZGhlNDA5NgBibGFrZTJzMjU2AEJMQUtFMnMyNTYAc2hha2UyNTYAYWVzLTEyOC1jYmMtaG1hYy1zaGEyNTYAYWVzLTI1Ni1jYmMtaG1hYy1zaGEyNTYAYWVzLTE5Mi1jYmMtaG1hYy1zaGEyNTYAbWRfZ29zdDEyXzI1NgBnb3N0MjAxMl8yNTYAU0hBS0UyNTYAaG1hY1dpdGhTSEEyNTYAZHNhX3dpdGhfU0hBMjU2AGVjZHNhLXdpdGgtU0hBMjU2AEFFUy0xMjgtQ0JDLUhNQUMtU0hBMjU2AEFFUy0yNTYtQ0JDLUhNQUMtU0hBMjU2AEFFUy0xOTItQ0JDLUhNQUMtU0hBMjU2AFJTQS1TSEEyNTYAUlNBLVNIQTUxMi8yNTYAaWQtZWNkc2Etd2l0aC1zaGEzLTI1NgBpZC1kc2Etd2l0aC1zaGEzLTI1NgBpZC1yc2Fzc2EtcGtjczEtdjFfNS13aXRoLXNoYTMtMjU2AGhtYWMtc2hhMy0yNTYAaWQtaG1hY1dpdGhTSEEzLTI1NgBlY2RzYV93aXRoX1NIQTMtMjU2AFJTQS1TSEEzLTI1NgBzaGE1MTItMjU2AGhtYWNXaXRoU0hBNTEyLTI1NgBpZC10YzI2LWhtYWMtZ29zdC0zNDExLTIwMTItMjU2AGlkLXRjMjYtc2lnbndpdGhkaWdlc3QtZ29zdDM0MTAtMjAxMi0yNTYAaWQtdGMyNi1hZ3JlZW1lbnQtZ29zdC0zNDEwLTIwMTItMjU2AGlkLXRjMjYARFNUVSBjdXJ2ZSA2AGMyb25iMjM5djUAYzJvbmIxOTF2NQB3YXAtd3NnLWlkbS1lY2lkLXd0bHM1AHBrY3M1AHVhY3VydmU1AHJjNC1obWFjLW1kNQBobWFjV2l0aE1ENQBSU0EtTlAtTUQ1AFJDNC1ITUFDLU1ENQBSU0EtTUQ1AElBNQBITUFDIERTVFUgR29zdCAzNDMxMS05NQBpZC10YzI2LXdyYXAtZ29zdHIzNDEyLTIwMTUta3V6bnllY2hpay1rZXhwMTUAaWQtdGMyNi13cmFwLWdvc3RyMzQxMi0yMDE1LW1hZ21hLWtleHAxNQBjaGFjaGEyMC1wb2x5MTMwNQBDaGFDaGEyMC1Qb2x5MTMwNQBEU1RVIGN1cnZlIDUAYzJvbmIyMzl2NABjMm9uYjE5MXY0AHdhcC13c2ctaWRtLWVjaWQtd3RsczQAdWFjdXJ2ZTQAbWQ0AHJjNABpcHNlYzQAUlNBLU1ENABwYmVXaXRoU0hBMUFuZDEyOEJpdFJDNABwYmVXaXRoU0hBMUFuZDQwQml0UkM0AG1kX2dvc3Q5NABwcmYtZ29zdHIzNDExLTk0AGlkLUhNQUNHb3N0UjM0MTEtOTQASE1BQyBHT1NUIDM0LjExLTk0AEdPU1QgUiAzNC4xMS05NABpZC1Hb3N0UjM0MTEtOTQtd2l0aC1Hb3N0UjM0MTAtOTQAR09TVCBSIDM0LjExLTk0IHdpdGggR09TVCBSIDM0LjEwLTk0AGlkLWRzYS13aXRoLXNoYTM4NABobWFjV2l0aFNIQTM4NABkc2Ffd2l0aF9TSEEzODQAZWNkc2Etd2l0aC1TSEEzODQAUlNBLVNIQTM4NABpZC1lY2RzYS13aXRoLXNoYTMtMzg0AGlkLWRzYS13aXRoLXNoYTMtMzg0AGlkLXJzYXNzYS1wa2NzMS12MV81LXdpdGgtc2hhMy0zODQAaG1hYy1zaGEzLTM4NABpZC1obWFjV2l0aFNIQTMtMzg0AGVjZHNhX3dpdGhfU0hBMy0zODQAUlNBLVNIQTMtMzg0AFBCRS1NRDUtUkMyLTY0AFBCRS1NRDItUkMyLTY0AFBCRS1TSEExLVJDMi02NABmZmRoZTYxNDQAc2hhMjI0AGhtYWNXaXRoU0hBMjI0AGRzYV93aXRoX1NIQTIyNABlY2RzYS13aXRoLVNIQTIyNABSU0EtU0hBMjI0AFJTQS1TSEE1MTIvMjI0AGlkLWVjZHNhLXdpdGgtc2hhMy0yMjQAaWQtZHNhLXdpdGgtc2hhMy0yMjQAaWQtcnNhc3NhLXBrY3MxLXYxXzUtd2l0aC1zaGEzLTIyNABobWFjLXNoYTMtMjI0AGlkLWhtYWNXaXRoU0hBMy0yMjQAZWNkc2Ffd2l0aF9TSEEzLTIyNABSU0EtU0hBMy0yMjQAc2hhNTEyLTIyNABobWFjV2l0aFNIQTUxMi0yMjQAT2FrbGV5LUVDMk4tNABEU1RVIGN1cnZlIDQAcHJpbWUyMzl2MwBjMnRuYjIzOXYzAGMycG5iMTYzdjMAcHJpbWUxOTJ2MwBjMnRuYjE5MXYzAGlkLXNtaW1lLW1vZC1tc2ctdjMAd2FwLXdzZy1pZG0tZWNpZC13dGxzMwBwa2NzMwBuMwBzbTMAdWFjdXJ2ZTMAZGVzLWVkZTMAaXBzZWMzAFJTQS1TTTMAREVTLUVERTMAaWQtbW9kLXF1YWxpZmllZC1jZXJ0LTkzAGlkLXBraXgxLWV4cGxpY2l0LTkzAGlkLXBraXgxLWltcGxpY2l0LTkzAGlkLW1vZC1rZWEtcHJvZmlsZS05MwBzc2x2MjMAT2FrbGV5LUVDMk4tMwBEU1RVIGN1cnZlIDMAc25tcHYyAFNOTVB2MgBwcmltZTIzOXYyAGMydG5iMjM5djIAYzJwbmIxNjN2MgBwcmltZTE5MnYyAGMydG5iMTkxdjIAc2VjcDEyOHIyAHNlY3QxOTNyMgBzZWN0MTYzcjIAc2VjdDExM3IyAHNlY3AxMTJyMgBzZWN0MTMxcjIAc2VjcDE2MHIyAHNtMgBzZXRleHQtdHJhY2syAHVhY3VydmUyAG1kMgBtZGMyAGlkLXNtaW1lLWFhLXNpZ25pbmdDZXJ0aWZpY2F0ZVYyAHNldEF0dHItSXNzQ2FwLVQyAFBCRVMyAFNNMgBQQktERjIAUlNBLU1EMgBpZC1zbWltZS1hbGctRVNESHdpdGhSQzIAUlNBLU1EQzIAZmZkaGU4MTkyAGZmZGhlMzA3MgBBTlNJIFg5LjYyAGFuc2ktWDktNjIASU5UMzIAYXV0aC1nb3N0MTIAd2FwLXdzZy1pZG0tZWNpZC13dGxzMTIAQXV0aEdPU1QxMgBibGFrZTJiNTEyAEJMQUtFMmI1MTIAaWQtZHNhLXdpdGgtc2hhNTEyAG1kX2dvc3QxMl81MTIAZ29zdDIwMTJfNTEyAGhtYWNXaXRoU0hBNTEyAGRzYV93aXRoX1NIQTUxMgBlY2RzYS13aXRoLVNIQTUxMgBSU0EtU0hBNTEyAGlkLWVjZHNhLXdpdGgtc2hhMy01MTIAaWQtZHNhLXdpdGgtc2hhMy01MTIAaWQtcnNhc3NhLXBrY3MxLXYxXzUtd2l0aC1zaGEzLTUxMgBobWFjLXNoYTMtNTEyAGlkLWhtYWNXaXRoU0hBMy01MTIAZWNkc2Ffd2l0aF9TSEEzLTUxMgBSU0EtU0hBMy01MTIAaWQtdGMyNi1obWFjLWdvc3QtMzQxMS0yMDEyLTUxMgBpZC10YzI2LXNpZ253aXRoZGlnZXN0LWdvc3QzNDEwLTIwMTItNTEyAGlkLXRjMjYtYWdyZWVtZW50LWdvc3QtMzQxMC0yMDEyLTUxMgBnb3N0ODktY250LTEyAGdvc3QtbWFjLTEyAFJTQS1TSEExLTIAY2xlYXJ0ZXh0IHRyYWNrIDIAZW5jcnlwdGVkIHRyYWNrIDIARFNUVSBjdXJ2ZSAyAGMycG5iMzY4dzEAYzJwbmIyMDh3MQBjMnBuYjMwNHcxAGMycG5iMjcydzEAYzJ0bmIzNTl2MQBwcmltZTIzOXYxAGMydG5iMjM5djEAYzJwbmIxNzZ2MQBwcmltZTI1NnYxAGMycG5iMTYzdjEAcHJpbWUxOTJ2MQBjMnRuYjE5MXYxAGlkLXFjcy1wa2l4UUNTeW50YXgtdjEAYnJhaW5wb29sUDI1NnQxAGJyYWlucG9vbFAzODR0MQBicmFpbnBvb2xQMjI0dDEAYnJhaW5wb29sUDE5MnQxAGJyYWlucG9vbFA1MTJ0MQBicmFpbnBvb2xQMTYwdDEAYnJhaW5wb29sUDMyMHQxAHdhcC13c2ctaWRtLWVjaWQtd3RsczEAcGtjczEAc2VjdDQwOXIxAHNlY3AxMjhyMQBicmFpbnBvb2xQMjU2cjEAc2VjcDM4NHIxAGJyYWlucG9vbFAzODRyMQBzZWNwMjI0cjEAYnJhaW5wb29sUDIyNHIxAHNlY3QxOTNyMQBzZWN0MjgzcjEAc2VjdDE2M3IxAHNlY3QyMzNyMQBzZWN0MTEzcjEAYnJhaW5wb29sUDE5MnIxAGJyYWlucG9vbFA1MTJyMQBzZWNwMTEycjEAc2VjdDU3MXIxAGMydG5iNDMxcjEAc2VjdDEzMXIxAHNlY3A1MjFyMQBzZWNwMTYwcjEAYnJhaW5wb29sUDE2MHIxAGJyYWlucG9vbFAzMjByMQBkbXExAGRtcDEAc2VjdDIzOWsxAHNlY3Q0MDlrMQBzZWNwMjU2azEAc2VjcDIyNGsxAHNlY3QyODNrMQBzZWN0MTYzazEAc2VjdDIzM2sxAHNlY3AxOTJrMQBzZWN0NTcxazEAc2VjcDE2MGsxAG1nZjEAdWFjdXJ2ZTEAZGVzLWNmYjEAYWVzLTEyOC1jZmIxAGFyaWEtMTI4LWNmYjEAY2FtZWxsaWEtMTI4LWNmYjEAYWVzLTI1Ni1jZmIxAGFyaWEtMjU2LWNmYjEAY2FtZWxsaWEtMjU2LWNmYjEAc200LWNmYjEAZGVzLWVkZTMtY2ZiMQBhZXMtMTkyLWNmYjEAYXJpYS0xOTItY2ZiMQBjYW1lbGxpYS0xOTItY2ZiMQBpZC1hbGctZGgtc2lnLWhtYWMtc2hhMQBhZXMtMTI4LWNiYy1obWFjLXNoYTEAYWVzLTI1Ni1jYmMtaG1hYy1zaGExAGFlcy0xOTItY2JjLWhtYWMtc2hhMQBtZDUtc2hhMQBNR0YxAFBCTUFDMQBERVMtQ0ZCMQBBRVMtMTI4LUNGQjEAQVJJQS0xMjgtQ0ZCMQBDQU1FTExJQS0xMjgtQ0ZCMQBBRVMtMjU2LUNGQjEAQVJJQS0yNTYtQ0ZCMQBDQU1FTExJQS0yNTYtQ0ZCMQBTTTQtQ0ZCMQBERVMtRURFMy1DRkIxAEFFUy0xOTItQ0ZCMQBBUklBLTE5Mi1DRkIxAENBTUVMTElBLTE5Mi1DRkIxAGhtYWNXaXRoU0hBMQBkc2FXaXRoU0hBMQBlY2RzYS13aXRoLVNIQTEAQUVTLTEyOC1DQkMtSE1BQy1TSEExAEFFUy0yNTYtQ0JDLUhNQUMtU0hBMQBBRVMtMTkyLUNCQy1ITUFDLVNIQTEAUlNBLVNIQTEARFNBLVNIQTEATUQ1LVNIQTEAVDYxAHg5MzEAd2FwLXdzZy1pZG0tZWNpZC13dGxzMTEAZHN0dTM0MzExAGhtYWNXaXRoRHN0dTM0MzExAGF1dGgtZ29zdDAxAEF1dGhHT1NUMDEAZ29zdDIwMDEAaWQtR29zdFIzNDExLTk0LXdpdGgtR29zdFIzNDEwLTIwMDEAR09TVCBSIDM0LjExLTk0IHdpdGggR09TVCBSIDM0LjEwLTIwMDEARFNUVSBjdXJ2ZSAxAHVhY3VydmUwAHJpcGVtZDE2MABSU0EtUklQRU1EMTYwAGlkLWFsZy1kZXM0MAByYzQtNDAAUEJFLVNIQTEtUkM0LTQwAFBCRS1TSEExLVJDMi00MABjaGFjaGEyMABDaGFDaGEyMAB3YXAtd3NnLWlkbS1lY2lkLXd0bHMxMABYNTAwAGlkLW1vZC1jbXAyMDAwAERTVFUgY3VydmUgMABhc3NlcnRpb24gZmFpbGVkOiAoYml0ICYgMSkgPT0gMABhc3NlcnRpb24gZmFpbGVkOiAoKHB0ciAtIHNoLmFyZW5hKSAmICgoc2guYXJlbmFfc2l6ZSA+PiBsaXN0KSAtIDEpKSA9PSAwAC8ATmV0c2NhcGUgQ29tbXVuaWNhdGlvbnMgQ29ycC4AUlNBIERhdGEgU2VjdXJpdHksIEluYy4ALQBhc3NlcnRpb24gZmFpbGVkOiBXSVRISU5fRlJFRUxJU1QodGVtcDItPnBfbmV4dCkgfHwgV0lUSElOX0FSRU5BKHRlbXAyLT5wX25leHQpAGFzc2VydGlvbiBmYWlsZWQ6IHRlbXAtPm5leHQgPT0gTlVMTCB8fCBXSVRISU5fQVJFTkEodGVtcC0+bmV4dCkAYXNzZXJ0aW9uIGZhaWxlZDogdGVtcC0oc2guYXJlbmFfc2l6ZSA+PiBzbGlzdCkgPT0gc2hfZmluZF9teV9idWRkeSh0ZW1wLCBzbGlzdCkAYXNzZXJ0aW9uIGZhaWxlZDogV0lUSElOX0ZSRUVMSVNUKGxpc3QpAGFzc2VydGlvbiBmYWlsZWQ6IHB0ciA9PSBzaF9maW5kX215X2J1ZGR5KGJ1ZGR5LCBsaXN0KQBCQyAoZGVmYXVsdCkAMTQgKGRlZmF1bHQpAG1nZjEgd2l0aCBzaGExIChkZWZhdWx0KQBHT1NUIFIgMzQuMTAtMjAxMiB3aXRoIEdPU1QgUiAzNC4xMS0yMDEyICgyNTYgYml0KQBHT1NUIFIgMzQuMTAtMjAxMiB3aXRoIEdPU1QgUiAzNC4xMS0yMDEyICg1MTIgYml0KQBhc3NlcnRpb24gZmFpbGVkOiAhVEVTVEJJVCh0YWJsZSwgYml0KQBhc3NlcnRpb24gZmFpbGVkOiBURVNUQklUKHRhYmxlLCBiaXQpAGFzc2VydGlv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"),Q(r,32445,"BAAAAAI="),Q(r,32461,"EAAAABAAAAAQAAAAEAAAABAAAAAgAAAAEAAAABAAAAAQAAAAAAEAAAAAAAEAAAACAAAABAAAAAgAAAAQAAAAAEAAAACAAAAgAAAAQAAAAIAAAAAAAQAAABAAAAAIAAAAEAAAAAAAAAQ="),Q(r,32584,"1VIAAAAAAAAC"),Q(r,32612,"sU8AAAAAAAAD"),Q(r,32640,"xVIAAAAAAAAG"),Q(r,32668,"UUwAAAAAAAD8////"),Q(r,32696,"2ko="),Q(r,32716,"2yoAAIR/AAABAAAAAAAAAAQAAAB6HAAAoH8AAAEAAAAQAAAAwH8AAAIAAAAAAAAACAAAAKJPAAAAAAAAAg=="),Q(r,32788,"tGIBAAAAAACMUAAAAAAAAAI="),Q(r,32816,"1GIBAAEAAACLUAAAAAAAAAI="),Q(r,32844,"9GIBAAIAAADibA=="),Q(r,32876,"ixsAAOh/"),Q(r,32892,"BAAAAMoIAAAwfwAAAQAAABAAAABggAAAAgAAAAAAAAAIAAAAZVMAAAEAAACkgAAAAQAAAAEAAAAAAAAAAgAAAAAAAAACAAMABQAHAAsADQARABMAFwAdAB8AJQApACsALwA1ADsAPQBDAEcASQBPAFMAWQBhAGUAZwBrAG0AcQB/AIMAiQCLAJUAlwCdAKMApwCtALMAtQC/AMEAxQDHANMA3wDjAOUA6QDvAPEA+wABAQcBDQEPARUBGQEbASUBMwE3ATkBPQFLAVEBWwFdAWEBZwFvAXUBewF/AYUBjQGRAZkBowGlAa8BsQG3AbsBwQHJAc0BzwHTAd8B5wHrAfMB9wH9AQkCCwIdAiMCLQIzAjkCOwJBAksCUQJXAlkCXwJlAmkCawJ3AoECgwKHAo0CkwKVAqECpQKrArMCvQLFAs8C1wLdAuMC5wLvAvUC+QIBAwUDEwMdAykDKwM1AzcDOwM9A0cDVQNZA1sDXwNtA3EDcwN3A4sDjwOXA6EDqQOtA7MDuQPHA8sD0QPXA98D5QPxA/UD+wP9AwcECQQPBBkEGwQlBCcELQQ/BEMERQRJBE8EVQRdBGMEaQR/BIEEiwSTBJ0EowSpBLEEvQTBBMcEzQTPBNUE4QTrBP0E/wQDBQkFCwURBRUFFwUbBScFKQUvBVEFVwVdBWUFdwWBBY8FkwWVBZkFnwWnBasFrQWzBb8FyQXLBc8F0QXVBdsF5wXzBfsFBwYNBhEGFwYfBiMGKwYvBj0GQQZHBkkGTQZTBlUGWwZlBnkGfwaDBoUGnQahBqMGrQa5BrsGxQbNBtMG2QbfBvEG9wb7Bv0GCQcTBx8HJwc3B0UHSwdPB1EHVQdXB2EHbQdzB3kHiweNB50Hnwe1B7sHwwfJB80HzwfTB9sH4QfrB+0H9wcFCA8IFQghCCMIJwgpCDMIPwhBCFEIUwhZCF0IXwhpCHEIgwibCJ8IpQitCL0IvwjDCMsI2wjdCOEI6QjvCPUI+QgFCQcJHQkjCSUJKwkvCTUJQwlJCU0JTwlVCVkJXwlrCXEJdwmFCYkJjwmbCaMJqQmtCccJ2QnjCesJ7wn1CfcJ/QkTCh8KIQoxCjkKPQpJClcKYQpjCmcKbwp1CnsKfwqBCoUKiwqTCpcKmQqfCqkKqwq1Cr0KwQrPCtkK5QrnCu0K8QrzCgMLEQsVCxsLIwspCy0L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GzshOyM7LTs5O0U7UztZO187cTt7O4E7iTubO587pTunO607tzu5O8M7yzvRO9c74TvjO/U7/zsBPA08ETwXPB88KTw1PEM8TzxTPFs8ZTxrPHE8hTyJPJc8pzy1PL88xzzRPN083zzxPPc8Az0NPRk9Gz0fPSE9LT0zPTc9Pz1DPW89cz11PXk9ez2FPZE9lz2dPas9rz21Pbs9wT3JPc898z0FPgk+Dz4RPh0+Iz4pPi8+Mz5BPlc+Yz5lPnc+gT6HPqE+uT69Pr8+wz7FPsk+1z7bPuE+5z7vPv8+Cz8NPzc/Oz89P0E/WT9fP2U/Zz95P30/iz+RP60/vz/NP9M/3T/pP+s/8T/9PxtAIUAlQCtAMUA/QENARUBdQGFAZ0BtQIdAkUCjQKlAsUC3QL1A20DfQOtA90D5QAlBC0ERQRVBIUEzQTVBO0E/QVlBZUFrQXdBe0GTQatBt0G9Qb9By0HnQe9B80H5QQVCB0IZQh9CI0IpQi9CQ0JTQlVCW0JhQnNCfUKDQoVCiUKRQpdCnUK1QsVCy0LTQt1C40LxQgdDD0MfQyVDJ0MzQzdDOUNPQ1dDaUOLQ41Dk0OlQ6lDr0O1Q71Dx0PPQ+FD50PrQ+1D8UP5QwlEC0QXRCNEKUQ7RD9ERURLRFFEU0RZRGVEb0SDRI9EoUSlRKtErUS9RL9EyUTXRNtE+UT7RAVFEUUTRStFMUVBRUlFU0VVRWFFd0V9RX9Fj0WjRa1Fr0W7RcdFMDEyMzQ1Njc4OUFCQ0RFRg=="),Q(r,37088,"CA=="),Q(r,37106,"EAAQAAAAAAAQ"),Q(r,37152,"EAAAAkAAgAAAAAACAAJAAAAAAAAAAgACAAIAAgACAAIBAAEAAQABAAEAAQABAAEAAQABAAAAAAIAAAAAAAAAAgACAgACAAIAAgACAAIAAgACAAIAAgACAAIAAgACAAIAAgACAAIAAgACAAIAAgACAAIAAgACAAAAIAAAAAACAAFAAAQABAAEAAQABAAEAAQABAAEAAQABAAEAAQABAAEAAQABAAEAAQABAAEAAQABAAEAAQABAAAAAACAAAAAgAA77u/"),Q(r,37360,"QABAAEAAQABAAEAAQABAAEAAaABIAEgASABIAEAAQABAAEAAQABAAEAAQABAAEAAQABAAEAAQABAAEAAQABAACgJgAOAA4ADgAOAA4ADgAuAC4ALgAOAD4ALgAuAC4APlA2UDZQNlA2UDZQNlA2UDZQNlA2AC4ADgAOAD4ADgAuAA5INkg2SDZINkg2SDYINgg2CDYINgg2CDYINgg2CDYINgg2CDYINgg2CDYINgg2CDYINgg2AA4ADgAOAA4ADgAORDZENkQ2RDZENkQ2BDYENgQ2BDYENgQ2BDYENgQ2BDYENgQ2BDYENgQ2BDYENgQ2BDYENgAOAA4ADgANAAAQAAAAIAAAAEAAAAAAAAAAvAAAAMAAAADE="),Q(r,37652,"QAAAAGAAAAAAAAAAQAAAAEEAAAAUAAAACAAAADIAAAAzAAAANA=="),Q(r,37700,"QAAAAGQAAAA1AAAAVwMAAFcDAAAAAAAAmlkAACQ/"),Q(r,37740,"PQ=="),Q(r,37760,"Pg=="),Q(r,37800,"PwAAAEA="),Q(r,37840,"QQAAAAAAAABCAAAAAAAAAFcDAAAAAAAARAAAAEUAAABG"),Q(r,37888,"Rw=="),Q(r,37916,"SAAAAEk="),Q(r,37956,"SgAAAEs="),Q(r,37992,"DAQAAAAAAABbAAAAAAAAAFw="),Q(r,38084,"XQAAAF4AAABfAAAAYA=="),Q(r,38124,"/QMAAAAAAABhAAAAAAAAAGI="),Q(r,38220,"YwAAAGQAAABl"),Q(r,38260,"AQAAAAIAAAADAAAABAAAAAUAAAAGAAAABwAAAAgAAAAJAAAA/////////////////////////////////////woAAAALAAAADAAAAA0AAAAOAAAADwAAAP//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////CgAAAAsAAAAMAAAADQAAAA4AAAAPAAAAAAAAALlTAABaQA=="),Q(r,38504,"JS8AALt1AAABAAAABgAAAEA8AQAAAAAAwRkAAFlNAAACAAAABwAAAEY8AQAAAAAAkmwAAEFsAAADAAAACAAAAE08AQAAAAAAlGYAAGhmAAAEAAAACAAAAFU8AQAAAAAAtGcAAHZnAAAFAAAACAAAAF08AQAAAAAAgCQAAIAkAAAGAAAACQAAAGU8AQAAAAAAjmwAAHAlAAAHAAAACQAAAG48AQAAAAAAkGYAAOUkAAAIAAAACQAAAHc8AQAAAAAAOE0AAPtXAAAJAAAACQAAAIA8AQAAAAAALE0AAOZXAAAKAAAACQAAAIk8AQAAAAAAD3UAAC55AAALAAAAAQAAAJI8AQAAAAAAhWAAAIVgAAAMAAAAAgAAAJM8AQAAAAAAbVAAAEE5AAANAAAAAwAAAJU8AQAAAAAAX1oAALA4AAAOAAAAAwAAAJg8AQAAAAAAB1IAAIo4AAAPAAAAAwAAAJs8AQAAAAAAH0wAAHQ5AAAQAAAAAwAAAJ48AQAAAAAAUlAAAEw5AAARAAAAAwAAAKE8AQAAAAAACUwAABI5AAASAAAAAwAAAKQ8AQAAAAAAb14AAOBJAAATAAAABAAAAKc8AQAAAAAAI2MAACNjAAAUAAAACAAAAKs8AQAAAAAAW0cAAFtHAAAVAAAACQAAALM8AQAAAAAAZUkAAGVJAAAWAAAACQAAALw8AQAAAAAAFkkAABZJAAAXAAAACQAAAMU8AQAAAAAASEkAAEhJAAAYAAAACQAAAM48AQAAAAAAgkcAAIJHAAAZAAAACQAAANc8AQAAAAAA50gAAOdIAAAaAAAACQAAAOA8AQAAAAAA6WoAAOlqAAAbAAAACAAAAOk8AQAAAAAA6QoAAOkKAAAcAAAACQAAAPE8AQAAAAAAq1wAAGNGAAAdAAAABQAAAPo8AQAAAAAAilsAAD1FAAAeAAAABQAAAP88AQAAAAAAHlgAABxCAAAfAAAABQAAAAQ9AQAAAAAAf1QAAP08AAAgAAAABQAAAAk9AQAAAAAAF2sAAP9qAAAh"),Q(r,39296,"QlgAAFpCAAAiAAAACwAAAA49AQAAAAAArlsAAHtFAAAj"),Q(r,39344,"w1wAAJVGAAAk"),Q(r,39368,"QFkAAPRCAAAlAAAACAAAABk9AQAAAAAAOl0AABdHAAAm"),Q(r,39416,"MlwAAA1GAAAn"),Q(r,39440,"WFsAAAtFAAAo"),Q(r,39464,"rV4AAE5KAAApAAAABQAAACE9AQAAAAAAoV4AAJwkAAAqAAAABQAAACY9AQAAAAAALVgAADtCAAAr"),Q(r,39536,"xFgAAOdCAAAsAAAACAAAACs9AQAAAAAAsFoAADtEAAAtAAAABQAAADM9AQAAAAAA1FoAAHlEAAAu"),Q(r,39608,"R2AAAEdgAAAvAAAACAAAADg9AQAAAAAAbBMAAGwTAAAwAAAACQAAAEA9AQAAAAAA2jkAANo5AAAxAAAACQAAAEk9AQAAAAAAqjUAAKo1AAAyAAAACQAAAFI9AQAAAAAA6ggAAOoIAAAzAAAACQAAAFs9AQAAAAAA3jYAAN42AAA0AAAACQAAAGQ9AQAAAAAA6jQAAOo0AAA1AAAACQAAAG09AQAAAAAANT4AADU+AAA2AAAACQAAAHY9AQAAAAAAmRMAAJkTAAA3AAAACQAAAH89AQAAAAAA1xcAANcXAAA4AAAACQAAAIg9AQAAAAAAdDYAAJ11AAA5AAAABwAAAJE9AQAAAAAAiQcAAOsnAAA6AAAACAAAAJg9AQAAAAAAQzYAAGE2AAA7AAAACAAAAKA9AQAAAAAAmVsAAFxFAAA8"),Q(r,39944,"JVwAAABGAAA9"),Q(r,39968,"v1oAAFpEAAA+"),Q(r,39992,"S1sAAP5EAAA/"),Q(r,40016,"1nMAAJ1yAABAAAAABQAAAKg9AQAAAAAAwHMAAJ0lAABBAAAACQAAAK09AQAAAAAAqV4AAJZeAABCAAAABQAAALY9AQAAAAAAzj8AALw/AABDAAAABQAAALs9AQAAAAAAUGkAADJZAABEAAAACQAAAMA9AQAAAAAAh2wAAIdsAABFAAAACQAAAMk9AQAAAAAA5j8AANY/AABGAAAABQAAANI9AQAAAAAAnzUAAE42AABHAAAACQAAANc9AQAAAAAAwywAACwtAABIAAAACQAAAOA9AQAAAAAAlCwAAOQsAABJAAAACQAAAOk9AQAAAAAApCwAAPwsAABKAAAACQAAAPI9AQAAAAAAtiwAABctAABLAAAACQAAAPs9AQAAAAAAhiwAAM0sAABMAAAACQAAAAQ+AQAAAAAAMTkAAAU6AABNAAAACQAAAA0+AQAAAAAAsQoAALsKAABOAAAACQAAABY+AQAAAAAAOT0AAEg9AABPAAAACQAAAB8+AQAAAAAAm1cAABNCAABQ"),Q(r,40424,"Fz4AABc+AABRAAAAAgAAACg+AQAAAAAAGh0AAC8eAABSAAAAAwAAACo+AQAAAAAAdzsAANY7AABTAAAAAwAAAC0+AQAAAAAApT4AALs+AABUAAAAAwAAADA+AQAAAAAA9TgAADw6AABVAAAAAwAAADM+AQAAAAAABDkAAFw6AABWAAAAAwAAADY+AQAAAAAA1BEAABcSAABXAAAAAwAAADk+AQAAAAAA/h4AAJUfAABYAAAAAwAAADw+AQAAAAAAdhkAAIoZAABZAAAAAwAAAD8+AQAAAAAAAx0AAA8eAABaAAAAAwAAAEI+AQAAAAAAJlgAADRCAABbAAAACQAAAEU+AQAAAAAAs1wAAHtGAABc"),Q(r,40712,"klsAAFVFAABd"),Q(r,40736,"uFoAAFNEAABe"),Q(r,40760,"s2wAAEVsAABfAAAABAAAAE4+AQAAAAAAr2wAADdeAABgAAAABAAAAFI+AQAAAAAAznQAAL50AABh"),Q(r,40832,"clkAACZDAABi"),Q(r,40856,"ZlAAAF05AABjAAAAAwAAAFY+AQAAAAAAVFAAAII4AABkAAAAAwAAAFk+AQAAAAAAfBYAAHwWAABlAAAAAwAAAFw+AQAAAAAAKUAAANkdAABmAAAACgAAAF8+AQAAAAAAfREAAJMRAABnAAAAAwAAAGk+AQAAAAAAeGYAACxeAABoAAAABQAAAGw+AQAAAAAACB8AAAgfAABpAAAAAwAAAHE+AQAAAAAAjjoAAI46AABqAAAAAwAAAHQ+AQAAAAAAsyUAALMlAABrAAAAAwAAAHc+AQAAAAAAn1gAAMJCAABsAAAACQAAAHo+AQAAAAAAIF0AAP1GAABt"),Q(r,41120,"C1wAAOZFAABu"),Q(r,41144,"MVsAAOREAABv"),Q(r,41168,"hVcAAIVXAABwAAAACQAAAIM+AQAAAAAAyXMAAGJzAABxAAAABwAAAIw+AQAAAAAA0nMAAJlyAABy"),Q(r,41240,"sW4AAENeAABzAAAABQAAAJM+AQAAAAAAh14AAI4kAAB0AAAABwAAAJg+AQAAAAAAp3QAAJl0AAB1AAAABQAAAJ8+AQ=="),Q(r,41336,"o3QAAE9eAAB3AAAABgAAAKQ+AQAAAAAAqVgAAMxCAAB4AAAACAAAAKo+AQAAAAAAKl0AAAdHAAB5"),Q(r,41408,"FVwAAPBFAAB6"),Q(r,41432,"O1sAAO5EAAB7"),Q(r,41480,"q1oAAMInAAB9AAAACwAAALI+AQAAAAAAgDsAALw7AAB+AAAAAwAAAL0+AQAAAAAAZksAAGZLAAB/AAAABgAAAMA+AQAAAAAAQCEAAEAhAACAAAAABwAAAMY+AQAAAAAAgC8AADUnAACBAAAACAAAAM0+AQAAAAAAZC8AABcnAACCAAAACAAAANU+AQAAAAAA6jAAAFYxAACDAAAACAAAAN0+AQAAAAAA1CUAAOQlAACEAAAACAAAAOU+AQAAAAAAzDAAANwwAACFAAAACAAAAO0+AQAAAAAAcT8AAB8xAACGAAAACgAAAPU+AQAAAAAAcioAAEExAACHAAAACgAAAP8+AQAAAAAAjSgAAAIxAACIAAAACgAAAAk/AQAAAAAAdVcAAMwiAACJAAAACgAAABM/AQAAAAAA7EwAABMrAACKAAAACgAAAB0/AQAAAAAAb1cAAOoiAACLAAAACQAAACc/AQAAAAAA3lEAAGEbAACMAAAAAwAAADA/AQAAAAAAdiQAALA8AACNAAAAAwAAADM/AQAAAAAASDQAAFc0AACOAAAAAwAAADY/AQAAAAAAkVYAAK9WAACPAAAABQAAADk/AQAAAAAA3WIAAIlnAACQAAAACgAAAD4/AQAAAAAAxXQAAKFnAACRAAAACgAAAEg/AQAAAAAAEE0AAKRXAACSAAAACgAAAFI/AQAAAAAAHk0AAMVXAACTAAAACgAAAFw/AQAAAAAA7mIAANFYAACUAAAACgAAAGY/AQAAAAAA1XQAAO1YAACVAAAACgAAAHA/AQAAAAAAPTIAAD0yAACWAAAACwAAAHo/AQAAAAAARDIAAEQyAACXAAAACwAAAIU/AQAAAAAAWDIAAFgyAACYAAAACwAAAJA/AQAAAAAAejIAAHoyAACZAAAACwAAAJs/AQAAAAAAYDIAAGAyAACaAAAACwAAAKY/AQAAAAAAajIAAGoyAACbAAAACwAAALE/AQAAAAAA6DgAAOg4AACcAAAACQAAALw/AQAAAAAAcVYAAHFWAACdAAAACQAAAMU/AQAAAAAAODQAADg0AACeAAAACgAAAM4/AQAAAAAA+zMAAPszAACfAAAACgAAANg/AQAAAAAAPi0AAD4tAACgAAAACgAAAOI/AQAAAAAAfWwAAH1sAAChAAAACQAAAOw/AQAAAAAAp3IAAKdyAACiAAAACQAAAPU/AQAAAAAAVXMAAFVzAACjAAAACAAAAP4/AQAAAAAAJBUAAK5MAACkAAAACAAAAAZAAQAAAAAA4T0AAPo9AAClAAAACAAAAA5AAQAAAAAAuVgAANxCAACm"),Q(r,42488,"w0wAAEMZAACnAAAACQAAABZAAQAAAAAAQWkAAB1ZAACoAAAACQAAAB9AAQAAAAAAMmkAAAhZAACpAAAACQAAAChAAQAAAAAARE0AABBYAACqAAAACQAAADFAAQAAAAAA4R8AAI8IAACrAAAACgAAADpAAQAAAAAA2h8AAJkIAACsAAAACQAAAERAAQAAAAAAhTgAAIU4AACtAAAAAwAAAE1AAQAAAAAAdh4AAHYeAACuAAAAAwAAAFBAAQAAAAAAfTYAAH02AACvAAAABwAAAFNAAQAAAAAA/UAAAP1AAACwAAAABwAAAFpAAQAAAAAACxQAADEUAACxAAAACAAAAGFAAQAAAAAA0E8AANBPAACyAAAACAAAAGlAAQAAAAAA5xQAAPEUAACzAAAACAAAAHFAAQAAAAAA9jAAAGMxAAC0AAAACAAAAHlAAQAAAAAAPlAAABUjAAC1"),Q(r,42848,"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"),Q(r,46904,"qx4AAKseAABfAQAACAAAAHNGAQAAAAAATiEAAE4hAABgAQAACAAAAHtGAQAAAAAAgz0AAIM9AABhAQAACAAAAINGAQAAAAAAbCMAAGwjAABiAQAACAAAAItGAQAAAAAAAAQAAAAEAABjAQAACAAAAJNGAQAAAAAAFgQAABYEAABkAQAACAAAAJtGAQAAAAAAJSAAACUgAABlAQAACAAAAKNGAQAAAAAAsDoAALA6AABmAQAACAAAAKtGAQAAAAAAcW8AAHFvAABnAQAACAAAALNGAQAAAAAADRUAAA0VAABoAQAACAAAALtGAQAAAAAAskkAALJJAABpAQAACAAAAMNGAQAAAAAAlDQAAJQ0AABqAQAACAAAAMtGAQAAAAAAvDAAANkwAABrAQAACAAAANNGAQAAAAAAUU0AALIZAABsAQAACAAAANtGAQAAAAAAgjQAAKc0AABtAQAACQAAAONGAQAAAAAAMz0AAC49AABuAQAACQAAAOxGAQAAAAAAmVYAAMJWAABvAQAACQAAAPVGAQAAAAAAGRgAAC0YAABwAQAACQAAAP5GAQAAAAAAAi8AAAovAABxAQAACQAAAAdHAQAAAAAA3jIAAOwyAAByAQAACQAAABBHAQAAAAAADxsAAB4bAABzAQAACQAAABlHAQAAAAAA1RAAAOQQAAB0AQAACQAAACJHAQAAAAAAPkAAAD5AAAB1AQAACQAAACtHAQAAAAAA3i8AAN4vAAB2AQAACQAAADRHAQAAAAAADwoAACwKAAB3AQAACQAAAD1HAQAAAAAA2yoAANsqAAB4AQAABAAAAEZHAQAAAAAATzUAAE81AAB5AQAABQAAAEpHAQAAAAAA7RUAAA8WAAB6AQAAAgAAAE9HAQAAAAAAwVIAAHIwAAB7AQAAAQAAAFFHAQAAAAAAWlYAAGM/AAB8AQAAAgAAAFJHAQAAAAAAkV4AAANKAAB9AQAAAwAAAFRHAQAAAAAAFQUAAB8FAAB+AQAABAAAAFdHAQAAAAAATAsAAMwKAAB/AQAABAAAAFtHAQAAAAAAPi4AAEsuAACAAQAABAAAAF9HAQAAAAAA2zMAAOMzAACBAQAABAAAAGNHAQAAAAAAPQQAAEYEAACCAQAABAAAAGdHAQAAAAAAm2sAAKJrAACDAQAABAAAAGtHAQAAAAAA0i0AANItAACEAQAABAAAAG9HAQAAAAAARxgAAFMYAACFAQAABQAAAHNHAQAAAAAARhAAAIQQAACGAQAACQAAAHhHAQAAAAAAglcAAJgKAACHAQAACgAAAIFHAQAAAAAAXigAAHkoAACIAQAACgAAAItHAQAAAAAABFIAAARSAACJAQ=="),Q(r,47936,"XxgAALMYAACKAQAAAwAAAJVHAQAAAAAAuj0AALo9AACLAQAABAAAAJhHAQAAAAAAgWcAAPokAACMAQAACQAAAJxHAQAAAAAAdjAAAHYwAACNAQAACAAAAKVHAQAAAAAAHxQAAE4UAACOAQAACAAAAK1HAQAAAAAAwhQAAMIUAACPAQAACAAAALVHAQAAAAAAtzoAALc6AACQAQAAAwAAAL1HAQAAAAAAshEAAOURAACRAQAAAwAAAMBHAQAAAAAAgiYAAI8wAACSAQAAAwAAAMNHAQAAAAAAwi0AALw6AACTAQAAAwAAAMZHAQAAAAAABFIAAARSAACUAQ=="),Q(r,48200,"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"),Q(r,50768,"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yUsAAMlLAABLAgAABAAAAJRMAQAAAAAAFFUAABRVAABMAgAABAAAAJhMAQAAAAAATFUAAExVAABNAgAABAAAAJxMAQAAAAAAkUsAAJFLAABOAgAABAAAAKBMAQAAAAAAt1QAALdUAABPAgAABAAAAKRMAQAAAAAAgVUAAIFVAABQAgAABAAAAKhMAQAAAAAA7FQAAOxUAABRAgAABAAAAKxMAQAAAAAAmFUAAJhVAABSAgAABAAAALBMAQAAAAAAYFUAAGBVAABTAgAABAAAALRMAQAAAAAApksAAKZLAABUAgAABAAAALhMAQAAAAAAy1QAAMtUAABVAgAABAAAALxMAQAAAAAAw04AAMNOAABWAgAABAAAAMBMAQAAAAAAyE0AAMhNAABXAgAABAAAAMRMAQAAAAAAqk4AAKpOAABYAgAABAAAAMhMAQAAAAAAuwkAAFksAABZAgAABAAAAMxMAQAAAAAAiy8AAEwvAABaAgAABAAAANBMAQAAAAAAczUAAHM1AABbAgAABAAAANRMAQAAAAAAdQUAAHUFAABcAgAABAAAANhMAQAAAAAAKmwAACpsAABdAgAABAAAANxMAQAAAAAANwcAAGknAABeAgAABAAAAOBMAQAAAAAA/wkAAP8JAABfAgAABAAAAORMAQAAAAAAGQoAABkKAABgAgAABAAAAOhMAQAAAAAAjjUAAI41AABhAgAABAAAAOxMAQAAAAAAuEgAALhIAABiAgAABAAAAPBMAQAAAAAAREAAAERAAABjAgAABAAAAPRMAQAAAAAAcDEAAHAxAABkAgAABAAAAPhMAQAAAAAAkwcAAJMHAABlAgAABAAAAPxMAQAAAAAAzTIAAM0yAABmAgAABAAAAABNAQAAAAAA3hgAAN4YAABnAgAABAAAAARNAQAAAAAApx0AAKcdAABoAgAABAAAAAhNAQAAAAAAwUkAAMFJAABpAgAABAAAAAxNAQAAAAAA8jUAAPI1AABqAgAABAAAABBNAQAAAAAAKBkAACgZAABrAgAABAAAABRNAQAAAAAAjgkAAI4JAABsAgAABAAAABhNAQAAAAAAqCIAAPcYAABtAgAABAAAABxNAQAAAAAABDYAAAQ2AABuAgAABAAAACBNAQAAAAAAuCIAABQZAABvAgAABAAAACRNAQAAAAAAKkQAACpEAABwAgAABQAAAChNAQAAAAAAUAYAAFAGAABxAgAABQAAAC1NAQAAAAAA8ksAAPJLAAByAgAABQAAADJNAQAAAAAAsjYAALI2AABzAgAABQAAADdNAQAAAAAAeFAAAHhQAAB0AgAABQAAADxNAQAAAAAAa2wAAGtsAAB1AgAABQAAAEFNAQAAAAAAKjIAACoyAAB2AgAABQAAAEZNAQAAAAAAfykAAEUsAAB3AgAABgAAAEtNAQAAAAAAYkEAAM5uAAB4AgAABgAAAFFNAQAAAAAATgcAALxuAAB5AgAABgAAAFdNAQAAAAAABjIAAPs0AAB6AgAABgAAAF1NAQAAAAAAGDIAABI1AAB7AgAABgAAAGNNAQAAAAAAC14AAAteAAB8AgAABAAAAGlNAQAAAAAA/BQAAPwUAAB9AgAABAAAAG1NAQAAAAAAMBMAADATAAB+AgAABAAAAHFNAQAAAAAAnVwAAJ1cAAB/AgAABAAAAHVNAQAAAAAA6UkAAOlJAACAAgAABAAAAHlNAQAAAAAAdz4AAHc+AACBAgAABAAAAH1NAQAAAAAAsBAAALAQAACCAgAABQAAAIFNAQAAAAAAmFMAAMQyAACDAgAACAAAAIZNAQAAAAAAOEwAADhMAACEAgAACQAAAI5NAQAAAAAAbkwAAKoQAACFAg=="),Q(r,53984,"ZEwAAKAQAACGAg=="),Q(r,54008,"dhUAAJIVAACHAgAAAQAAAJdNAQAAAAAAFCgAACUoAACIAgAACgAAAJhNAQAAAAAAXFAAAB46AACJAgAACgAAAKJNAQAAAAAAt3IAAKFxAACKAg=="),Q(r,54104,"KHMAABJyAACLAg=="),Q(r,54128,"5HIAAM5xAACMAg=="),Q(r,54152,"gWEAANVgAACNAg=="),Q(r,54176,"8mEAAEZhAACOAg=="),Q(r,54200,"rmEAAAJhAACPAg=="),Q(r,54224,"rnIAAJhxAACQAg=="),Q(r,54248,"eGEAAMxgAACRAg=="),Q(r,54272,"GnMAAARyAACSAg=="),Q(r,54296,"5GEAADhhAACTAg=="),Q(r,54320,"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"),Q(r,56480,"bGoAAHpnAADuAg=="),Q(r,56504,"ZFgAAIdCAADvAgAACwAAAMxPAQAAAAAAYVkAABVDAADwAgAACwAAANdPAQAAAAAAjlgAALFCAADxAgAACwAAAOJPAQAAAAAA5VwAAMJGAADyAgAACAAAAO1PAQAAAAAAW10AADhHAADzAgAACAAAAPVPAQAAAAAAD10AAOxGAAD0AgAACAAAAP1PAQAAAAAA0FsAAJ1FAAD1AgAACAAAAAVQAQAAAAAAU1wAAC5GAAD2AgAACAAAAA1QAQAAAAAA+lsAANVFAAD3AgAACAAAABVQAQAAAAAA0nIAALxxAAD4Ag=="),Q(r,56744,"Q3MAAC1yAAD5Ag=="),Q(r,56768,"/3IAAOlxAAD6Ag=="),Q(r,56792,"nGEAAPBgAAD7Ag=="),Q(r,56816,"DWIAAGFhAAD8Ag=="),Q(r,56840,"yWEAAB1hAAD9Ag=="),Q(r,56864,"9loAAJtEAAD+AgAACAAAAB1QAQAAAAAAeVsAACxFAAD/AgAACAAAACVQAQAAAAAAIFsAANNEAAAAAwAACAAAAC1QAQAAAAAAvBcAAPUXAAABAwAAAwAAADVQAQAAAAAARQoAAF4KAAACAwAAAwAAADhQAQAAAAAA3RsAACMcAAADAwAAAwAAADtQAQ=="),Q(r,57032,"c14AAORJAAAFAwAABgAAAD5QAQ=="),Q(r,57104,"ulwAAIJGAAAIAwAACAAAAERQAQAAAAAAOVgAAEdCAAAJAwAACAAAAExQAQAAAAAAy1oAAGZEAAAKAwAACAAAAFRQAQAAAAAApVsAAGhFAAALAwAACAAAAFxQAQAAAAAAh2YAAGNmAAAMAwAACAAAAGRQAQAAAAAAtnMAAI9yAAANAwAACAAAAGxQAQAAAAAAhlkAAO5ZAAAOAwAACQAAAHRQAQAAAAAABEQAANVZAAAPAwAACQAAAH1QAQAAAAAAKhcAACoXAAAQAwAACAAAAIZQAQAAAAAA+gQAAAcFAAARAwAACAAAAI5QAQAAAAAA+0gAAPtIAAASAwAACwAAAJZQAQAAAAAAoVMAAKFTAAATAwAACwAAAKFQAQAAAAAA7yEAAO8hAAAUAwAACQAAAKxQAQAAAAAAUSIAAFEiAAAVAwAACQAAALVQAQAAAAAAISIAACEiAAAWAwAACQAAAL5QAQAAAAAAoUAAAKFAAAAXAwAABwAAAMdQAQAAAAAAhEAAAIRAAAAYAwAABwAAAM5QAQAAAAAAkGkAAJBpAAAZAwAACAAAANVQAQAAAAAAfGQAAHxkAAAaAwAACAAAAN1QAQAAAAAAg2gAAINoAAAbAwAACAAAAOVQAQAAAAAAem0AAHptAAAcAwAACAAAAO1QAQAAAAAAbGYAAGxmAAAdAwAACAAAAPVQAQAAAAAAcWkAAHFpAAAeAwAACAAAAP1QAQAAAAAAXWQAAF1kAAAfAwAACAAAAAVRAQAAAAAAZGgAAGRoAAAgAwAACAAAAA1RAQAAAAAAW20AAFttAAAhAwAACAAAABVRAQAAAAAAgGkAAIBpAAAiAwAACQAAAB1RAQAAAAAAbGQAAGxkAAAjAwAACQAAACZRAQAAAAAATS0AAE0tAAAkAwAABgAAAC9RAQAAAAAAGSMAABkjAAAlAwAABQAAADVRAQAAAAAArSkAAK0pAAAmAwAABQAAADpRAQAAAAAAOHQAAFx0AAAnAwAABgAAAD9RAQAAAAAACmgAACxoAAAoAwAABgAAAEVRAQAAAAAAuGcAAPpnAAApAwAABgAAAEtRAQAAAAAA02cAAOdnAAAqAwAABgAAAFFRAQAAAAAAL3QAAHF0AAArAwAABgAAAFdRAQAAAAAAu2cAAEFoAAAsAwAABgAAAF1RAQAAAAAAVmAAAGhgAAAtAwAABgAAAGNRAQAAAAAANAsAADQLAAAuAw=="),Q(r,58040,"1UMAAAFaAAAvAwAABgAAAGlRAQAAAAAAwmcAAHdTAAAwAwAABgAAAG9RAQAAAAAAfFIAAKxSAAAxAwAABgAAAHVRAQAAAAAAalIAAJBSAAAyAwAABgAAAHtRAQAAAAAAgjEAAIIxAAAzAwAABwAAAIFRAQAAAAAApzEAAKcxAAA0AwAABwAAAIhRAQAAAAAAbAwAAGwMAAA1AwAABwAAAI9RAQAAAAAAxQwAAMUMAAA2AwAABwAAAJZRAQAAAAAATwwAAE8MAAA3AwAABwAAAJ1RAQAAAAAA3w4AAN8OAAA4AwAABwAAAKRRAQAAAAAAHA4AABwOAAA5AwAABwAAAKtRAQAAAAAAWQ0AAFkNAAA6AwAABwAAALJRAQAAAAAA5wwAAOcMAAA7AwAABwAAALlRAQAAAAAAeQ8AAHkPAAA8AwAABwAAAMBRAQAAAAAApg8AAKYPAAA9AwAABwAAAMdRAQAAAAAAUA8AAFAPAAA+AwAABwAAAM5RAQAAAAAAiQwAAIkMAAA/AwAABwAAANVRAQAAAAAABA8AAAQPAABAAwAABwAAANxRAQAAAAAAQQ4AAEEOAABBAwAABwAAAONRAQAAAAAAfg0AAH4NAABCAwAABwAAAOpRAQAAAAAADA0AAAwNAABDAwAABwAAAPFRAQAAAAAAjQ4AAI0OAABEAwAABwAAAPhRAQAAAAAAyg0AAMoNAABFAwAABwAAAP9RAQAAAAAAMQ0AADENAABGAwAABwAAAAZSAQAAAAAApgwAAKYMAABHAwAABwAAAA1SAQAAAAAAKQ8AACkPAABIAwAABwAAABRSAQAAAAAAZg4AAGYOAABJAwAABwAAABtSAQAAAAAAow0AAKMNAABKAwAABwAAACJSAQAAAAAAtQ4AALUOAABLAwAABwAAAClSAQAAAAAA8g0AAPINAABMAwAABwAAADBSAQAAAAAAckoAAHJKAABNAwAABwAAADdSAQAAAAAAzhYAAM4WAABOAwAABwAAAD5SAQAAAAAASUcAAElHAABPAwAABwAAAEVSAQAAAAAAuRYAALkWAABQAwAABwAAAExSAQAAAAAAtEEAANMPAABRAwAACAAAAFNSAQAAAAAAgkEAAOApAABSAwAACAAAAFtSAQAAAAAAi0EAACcqAABTAwAACAAAAGNSAQAAAAAAx0EAAPgpAABUAwAACAAAAGtSAQAAAAAA7EEAAEEqAABVAwAACAAAAHNSAQAAAAAAlkEAALcpAABWAwAACAAAAHtSAQAAAAAAmlkAAJpDAABXAw=="),Q(r,59024,"QwwAAMcLAABYAwAACQAAAINSAQAAAAAAs1EAAOdRAABZAwAAAwAAAIxSAQAAAAAATR0AAE0eAABaAwAACAAAAI9SAQAAAAAA3TwAAN08AABbAwAAAwAAAJdSAQAAAAAAKQUAACkFAABcAwAAAwAAAJpSAQAAAAAAeRMAAHkTAABdAwAAAwAAAJ1SAQAAAAAAwQYAAMEGAABeAwAAAwAAAKBSAQAAAAAAqDkAAKg5AABfAwAAAwAAAKNSAQAAAAAAFR8AABUfAABgAwAAAwAAAKZSAQAAAAAA5x4AAOceAABhAwAAAwAAAKlSAQAAAAAAvx0AAL8dAABiAwAAAwAAAKxSAQAAAAAATh8AAE4fAABjAwAAAwAAAK9SAQAAAAAA1BMAANQTAABkAwAAAwAAALJSAQAAAAAAfR8AAH0fAABlAwAAAwAAALVSAQAAAAAArRMAAK0TAABmAwAAAwAAALhSAQAAAAAATBsAAEwbAABnAwAAAwAAALtSAQAAAAAASz8AAEs/AABoAwAAAwAAAL5SAQAAAAAAWBMAAFgTAABpAwAAAwAAAMFSAQAAAAAAYQcAAGEHAABqAwAAAwAAAMRSAQAAAAAApx8AAKcfAABrAwAAAwAAAMdSAQAAAAAAmRwAAJkcAABsAwAAAwAAAMpSAQAAAAAAPwsAAD8LAABtAwAAAwAAAM1SAQAAAAAADSMAAA0jAABuAwAAAwAAANBSAQAAAAAAKD4AACg+AABvAwAAAwAAANNSAQAAAAAA6zMAAOszAABwAwAAAwAAANZSAQAAAAAAKjQAACo0AABxAwAAAwAAANlSAQAAAAAA/wcAAP8HAAByAwAAAwAAANxSAQAAAAAAFwgAABcIAABzAwAAAwAAAN9SAQAAAAAAkRsAAJEbAAB0AwAAAwAAAOJSAQAAAAAA6TwAAOk8AAB1AwAAAwAAAOVSAQAAAAAAlCYAAJQmAAB2AwAAAwAAAOhSAQAAAAAA6zkAAOs5AAB3AwAAAwAAAOtSAQAAAAAArh8AAK4fAAB4AwAAAwAAAO5SAQAAAAAA/x0AAP8dAAB5AwAAAwAAAPFSAQAAAAAALxYAAC8WAAB6AwAAAwAAAPRSAQAAAAAAMQgAADEIAAB7AwAAAwAAAPdSAQAAAAAAwzkAAMM5AAB8AwAAAwAAAPpSAQAAAAAAN1IAADdSAAB9AwAACwAAAP1SAQAAAAAA0FkAANBDAAB+Aw=="),Q(r,59960,"nVAAAEQrAAB/AwAACQAAAAhTAQAAAAAAIVEAAMIrAACAAwAACQAAABFTAQAAAAAAxEAAAMRAAACBAwAACQAAABpTAQAAAAAA9VAAAJgrAACCAwAACQAAACNTAQAAAAAAeVEAABYsAACDAwAACQAAACxTAQAAAAAA6kAAAOpAAACEAwAACQAAADVTAQAAAAAAyVAAAG4rAACFAwAACQAAAD5TAQAAAAAATVEAAOwrAACGAwAACQAAAEdTAQAAAAAA10AAANdAAACHAwAACQAAAFBTAQAAAAAAHE8AAIkaAACIAw=="),Q(r,60200,"eE8AAOUaAACJAw=="),Q(r,60224,"Rk8AALMaAACKAw=="),Q(r,60248,"/iEAAP4hAACLAwAACwAAAFlTAQAAAAAAYCIAAGAiAACMAwAACwAAAGRTAQAAAAAAMCIAADAiAACNAwAACwAAAG9TAQAAAAAAkTsAAKU7AACOAwAABAAAAHpTAQAAAAAAonIAAIpxAACPAwAACQAAAH5TAQAAAAAAk0wAALgUAACQAwAACQAAAIdTAQAAAAAAe0wAAEcRAACRAwAACAAAAJBTAQAAAAAAh0wAAFMRAACSAwAACAAAAJhTAQAAAAAAg2YAAF9mAACTAw=="),Q(r,60464,"fnMAAFdyAACUAw=="),Q(r,60488,"qnMAAINyAACVAw=="),Q(r,60512,"lHMAAG1yAACWAw=="),Q(r,60536,"E1AAAG8hAACXAwAACQAAAKBTAQAAAAAA2B4AAKNSAACYAwAABwAAAKlTAQAAAAAA/HAAAPxwAACZAwAACQAAALBTAQAAAAAA2G8AANhvAACaAwAACQAAALlTAQAAAAAAn3AAAJ9wAACbAwAACQAAAMJTAQAAAAAAuG8AALhvAACcAwAACQAAAMtTAQAAAAAAXXAAAF1wAACdAwAACQAAANRTAQAAAAAAqG8AAKhvAACeAwAACQAAAN1TAQAAAAAAKXAAAClwAACfAwAACQAAAOZTAQAAAAAAiG8AAIhvAACgAwAACQAAAO9TAQAAAAAADHEAAAxxAAChAwAACQAAAPhTAQAAAAAA6G8AAOhvAACiAwAACQAAAAFUAQAAAAAAQ3AAAENwAACjAwAACQAAAApUAQAAAAAAmG8AAJhvAACkAwAACQAAABNUAQAAAAAAr3AAAK9wAAClAwAACQAAABxUAQAAAAAAyG8AAMhvAACmAwAACQAAACVUAQAAAAAA2VYAAHlAAACnAwAACQAAAC5UAQAAAAAAYDgAAGA4AACoAwAACQAAADdUAQAAAAAAyDcAAMg3AACpAwAABgAAAEBUAQAAAAAALjcAAC43AACqAwAABgAAAEZUAQAAAAAAezcAAHs3AACrAwAABgAAAExUAQAAAAAAFTgAABU4AACsAwAABgAAAFJUAQAAAAAAOTgAADk4AACtAwAACQAAAFhUAQAAAAAAnzcAAJ83AACuAwAABgAAAGFUAQAAAAAABTcAAAU3AACvAwAABgAAAGdUAQAAAAAAUjcAAFI3AACwAwAABgAAAG1UAQAAAAAA7DcAAOw3AACxAwAABgAAAHNUAQAAAAAAQDMAAEAzAACyAw=="),Q(r,61208,"MDMAADAzAACzAw=="),Q(r,61232,"jmQAAPFjAAC0Aw=="),Q(r,61256,"vmQAACFkAAC1Aw=="),Q(r,61280,"pmQAAAlkAAC2Aw=="),Q(r,61304,"DxMAAMYZAAC3AwAACgAAAHlUAQAAAAAASyQAAF0kAAC4AwAACgAAAINUAQAAAAAA2BwAAOocAAC5AwAACgAAAI1UAQAAAAAAHxMAAN0ZAAC6AwAACgAAAJdUAQAAAAAApVEAAJc4AAC7AwAACwAAAKFUAQAAAAAADEwAAIg5AAC8AwAACwAAAKxUAQAAAAAAVVcAANA4AAC9AwAACwAAALdUAQAAAAAAeVwAAD9GAAC+Aw=="),Q(r,61496,"kVwAAFdGAAC/Aw=="),Q(r,61520,"hVwAAEtGAADAAw=="),Q(r,61544,"uFAAAF0rAADBAwAACAAAAMJUAQAAAAAAPFEAANsrAADCAwAACAAAAMpUAQAAAAAANU8AAKIaAADDAwAACAAAANJUAQAAAAAAn1kAAJ9DAADEAwAACAAAANpUAQAAAAAAEFEAALErAADFAwAACAAAAOJUAQAAAAAAlFEAAC8sAADGAwAACAAAAOpUAQAAAAAAkU8AAP4aAADHAwAACAAAAPJUAQAAAAAAw1kAAMNDAADIAwAACAAAAPpUAQAAAAAA5FAAAIcrAADJAwAACAAAAAJVAQAAAAAAaFEAAAUsAADKAwAACAAAAApVAQAAAAAAX08AAMwaAADLAwAACAAAABJVAQAAAAAAsVkAALFDAADMAwAACAAAABpVAQAAAAAAsQkAALQJAADNAwAACQAAACJVAQAAAAAADmYAAA5mAADOAwAABQAAACtVAQAAAAAAl24AAJduAADPAw=="),Q(r,61904,"pW4AAKVuAADQAw=="),Q(r,61928,"/BUAAPwVAADRAwAABgAAADBVAQAAAAAAgCgAAIAoAADSAwAABwAAADZVAQAAAAAAR2QAAPkQAADTAwAACAAAAD1VAQAAAAAATm0AACARAADUAwAACAAAAEVVAQAAAAAAwggAAMIIAADVAwAABwAAAE1VAQAAAAAAOWQAAPovAADWAwAACAAAAFRVAQAAAAAAQG0AAB4wAADXAwAACAAAAFxVAQAAAAAAqwgAAKsIAADYAwAABwAAAGRVAQAAAAAAwGUAAEl3AADZAwAACAAAAGtVAQAAAAAASW4AAHx3AADaAwAACAAAAHNVAQAAAAAA+EMAAPhDAADbAwAABwAAAHtVAQAAAAAAoGUAAIALAADcAwAACAAAAIJVAQAAAAAAKW4AAJ0LAADdAwAACAAAAIpVAQAAAAAAlB4AAJQeAADeAwAABwAAAJJVAQAAAAAA1woAANcKAADfAwAABwAAAJlVAQAAAAAA6WUAAOllAADgAwAACAAAAKBVAQAAAAAAcm4AAHJuAADhAwAACAAAAKhVAQAAAAAAyBIAAMgSAADiAwAABgAAALBVAQAAAAAAbxIAAG8SAADjAwAABwAAALZVAQAAAAAA2hIAANoSAADkAwAACAAAAL1VAQAAAAAA+AgAAN8LAADlAwAACQAAAMVVAQAAAAAA310AAN5eAADmAwAACQAAAM5VAQAAAAAAhloAAJNdAADnAwAACQAAANdVAQAAAAAAPRIAAD0SAADoAwAABwAAAOBVAQAAAAAAVhIAAFYSAADpAwAABwAAAOdVAQAAAAAAhhIAAIYSAADqAwAACAAAAO5VAQAAAAAAsUoAABEMAADrAwAACQAAAPZVAQAAAAAAYlAAAGJQAADsAwAACAAAAP9VAQAAAAAAV1AAAFdQAADtAwAABQAAAAdWAQAAAAAA3UwAAN1MAADuAwAABQAAAAxWAQAAAAAAVy0AAC4QAADvAwAABQAAABFWAQAAAAAAZy0AAAwcAADwAwAABQAAABZWAQAAAAAAY0IAAGNCAADxAw=="),Q(r,62720,"nkYAAJ5GAADyAw=="),Q(r,62744,"fhoAAH4aAADzAw=="),Q(r,62768,"a0YAAGtGAAD0Aw=="),Q(r,62792,"ZBoAAGQaAAD1Aw=="),Q(r,62816,"Q0QAAENEAAD2Aw=="),Q(r,62840,"JEIAACRCAAD3Aw=="),Q(r,62864,"RUUAAEVFAAD4Aw=="),Q(r,62888,"3kMAAN5DAAD5Aw=="),Q(r,62912,"HWcAAAtnAAD6Aw=="),Q(r,62936,"7nQAAOV0AAD7Aw=="),Q(r,62960,"XDUAAGc1AAD8AwAACAAAABtWAQAAAAAAblMAAIEyAAD9Aw=="),Q(r,63008,"aFQAAPY6AAD+AwAACAAAACNWAQAAAAAAfVkAAGkUAAD/AwAACAAAACtWAQAAAAAAxk8AAGMmAAAABAAACAAAADNWAQAAAAAA+AoAAAoLAAABBAAACAAAADtWAQAAAAAAphsAALgbAAACBAAACAAAAENWAQAAAAAAPRwAAFocAAADBAAACAAAAEtWAQAAAAAASBwAAGYcAAAEBAAACAAAAFNWAQAAAAAAnxwAALocAAAFBAAACAAAAFtWAQAAAAAAqRwAAMUcAAAGBAAACAAAAGNWAQAAAAAAaQsAAGkLAAAHBAAABgAAAGtWAQAAAAAAby8AAJkvAAAIBAAABwAAAHFWAQAAAAAAe1cAALs0AAAJBAAABwAAAHhWAQAAAAAAdmAAAHZgAAAKBAAAAwAAAH9WAQAAAAAAtmIAALZiAAALBAAAAwAAAIJWAQAAAAAA0FMAACszAAAMBA=="),Q(r,63368,"Hl4AANRJAAANBA=="),Q(r,63392,"d1QAAOY6AAAOBA=="),Q(r,63416,"cVQAAO86AAAPBA=="),Q(r,63440,"K1IAAGguAAAQBA=="),Q(r,63464,"IVIAAHUuAAARBA=="),Q(r,63488,"F1IAAIAuAAASBA=="),Q(r,63512,"CVIAAFguAAATBA=="),Q(r,63536,"1U8AAGUgAAAUBA=="),Q(r,63560,"G0wAAMEHAAAVBA=="),Q(r,63584,"JF4AANtJAAAWBA=="),Q(r,63608,"gV4AAPhJAAAXBA=="),Q(r,63632,"D1IAAF8uAAAYBA=="),Q(r,63656,"pkwAAJQUAAAZBA=="),Q(r,63680,"JHQAABh0AAAaBA=="),Q(r,63704,"DG0AAOhsAAAbBA=="),Q(r,63728,"208AAGwgAAAcBA=="),Q(r,63753,"UgAAmi0AAB0E"),Q(r,63824,"Im0AABdtAAAgBAAACwAAAIVWAQAAAAAA3WMAANJjAAAhBAAACwAAAJBWAQAAAAAAGyYAABsmAAAiBAAACwAAAJtWAQAAAAAAKkkAACpJAAAjBAAACwAAAKZWAQAAAAAAkC0AAJAtAAAkBAAACwAAALFWAQAAAAAAJmcAABRnAAAlBA=="),Q(r,63968,"QjAAAPIvAAAmBA=="),Q(r,63992,"zEoAAGUFAAAnBA=="),Q(r,64016,"0koAAGwFAAAoBA=="),Q(r,64040,"2FwAALVGAAApBAAACQAAALxWAQAAAAAAV1gAAHpCAAAqBAAACQAAAMVWAQAAAAAAw1sAAJBFAAArBAAACQAAAM5WAQAAAAAA6VoAAI5EAAAsBAAACQAAANdWAQAAAAAAKE8AAJUaAAAtBAAACQAAAOBWAQAAAAAATl0AACtHAAAuBAAACQAAAOlWAQAAAAAAVFkAAAhDAAAvBAAACQAAAPJWAQAAAAAARlwAACFGAAAwBAAACQAAAPtWAQAAAAAAbFsAAB9FAAAxBAAACQAAAARXAQAAAAAAhE8AAPEaAAAyBAAACQAAAA1XAQAAAAAAAl0AAN9GAAAzBAAACQAAABZXAQAAAAAAgVgAAKRCAAA0BAAACQAAAB9XAQAAAAAA7VsAAMhFAAA1BAAACQAAAChXAQAAAAAAE1sAAMZEAAA2BAAACQAAADFXAQAAAAAAUk8AAL8aAAA3BAAACQAAADpXAQAAAAAAxHIAAK5xAAA4BA=="),Q(r,64424,"NXMAAB9yAAA5BA=="),Q(r,64448,"8XIAANtxAAA6BA=="),Q(r,64472,"jmEAAOJgAAA7BA=="),Q(r,64496,"/2EAAFNhAAA8BA=="),Q(r,64520,"u2EAAA9hAAA9BA=="),Q(r,64544,"SmwAAEpsAAA+BAAACwAAAENXAQAAAAAAfWAAAH1gAAA/BAAAAwAAAE5XAQAAAAAAu2IAALtiAABABAAAAwAAAFFXAQAAAAAAkB0AAJAdAABBBAAAAwAAAFRXAQAAAAAADGsAABJEAABCBAAAAwAAAFdXAQAAAAAA72oAADMpAABDBAAAAwAAAFpXAQAAAAAAKTkAACk5AABEBAAAAwAAAF1XAQAAAAAAgScAAJInAABFBAAABQAAAGBXAQAAAAAAYWoAAE5qAABGBAAACQAAAGVXAQAAAAAAlWUAAIJlAABHBAAACQAAAG5XAQAAAAAARWoAABBqAABIBAAACQAAAHdXAQAAAAAAeWUAAERlAABJBAAACQAAAIBXAQAAAAAAKWkAAPRoAABKBAAACQAAAIlXAQAAAAAAIG4AAOttAABLBAAACQAAAJJXAQAAAAAA1GIAAMtiAABMBAAACQAAAJtXAQAAAAAAVGQAAOhjAABNBAAACQAAAKRXAQAAAAAAGWoAAAtqAABOBAAACQAAAK1XAQAAAAAATWUAAD9lAABPBAAACQAAALZXAQAAAAAA/WgAAO9oAABQBAAACQAAAL9XAQAAAAAA9G0AAOZtAABRBAAACQAAAMhXAQAAAAAAUWgAAHNoAABSBAAACQAAANFXAQAAAAAALW0AAGptAABTBAAACQAAANpXAQAAAAAA02kAAC9qAABUBAAACQAAAONXAQAAAAAAB2UAAGNlAABVBAAACQAAAOxXAQAAAAAAt2gAABNpAABWBAAACQAAAPVXAQAAAAAArm0AAApuAABXBAAACQAAAP5XAQAAAAAAvGkAAC1qAABYBAAACQAAAAdYAQAAAAAA8GQAAGFlAABZBAAACQAAABBYAQAAAAAAoGgAABFpAABaBAAACQAAABlYAQAAAAAAl20AAAhuAABbBAAACQAAACJYAQAAAAAA6GkAAEFqAABcBAAACQAAACtYAQAAAAAAHGUAAHVlAABdBAAACQAAADRYAQAAAAAAzGgAACVpAABeBAAACQAAAD1YAQAAAAAAw20AABxuAABfBAAACQAAAEZYAQAAAAAAL1EAAM4rAABgBAAACQAAAE9YAQAAAAAAh1EAACIsAABhBAAACQAAAFhYAQAAAAAAW1EAAPgrAABiBAAACQAAAGFYAQAAAAAAq1AAAFArAABjBAAACQAAAGpYAQAAAAAAA1EAAKQrAABkBAAACQAAAHNYAQAAAAAA11AAAHorAABlBAAACQAAAHxYAQAAAAAAwWIAAMFiAABmBA=="),Q(r,65528,"wmwAAMJsAABnBA=="),Q(r,65552,"yGMAAMhjAABoBA=="),Q(r,65576,"YGkAAGBpAABpBA=="),Q(r,65600,"uGwAALhsAABqBA=="),Q(r,65624,"sV4AAGoEAABrBAAACAAAAIVYAQAAAAAAi14AAE8EAABsBAAACAAAAI1YAQAAAAAAMl0AAA9HAABtBAAACAAAAJVYAQAAAAAAsVgAANRCAABuBAAACAAAAJ1YAQAAAAAAQ1sAAPZEAABvBAAACAAAAKVYAQAAAAAAEXMAAPtxAABwBAAACAAAAK1YAQAAAAAAHVwAAPhFAABxBAAACAAAALVYAQAAAAAA22EAAC9hAAByBAAACAAAAL1YAQAAAAAAcE8AAN0aAABzBAAACAAAAMVYAQAAAAAAaVAAABcGAAB0BAAAAwAAAM1YAQAAAAAAWUoAAFlKAAB1BAAABQAAANBYAQAAAAAA+zYAAPs2AAB2BAAABgAAANVYAQAAAAAAE2sAAPJqAAB3BAAACAAAANtYAQAAAAAAD2sAAFslAAB4BAAACAAAAONYAQAAAAAArWkAAD8lAAB5BAAACQAAAOtYAQAAAAAA4WQAAMkkAAB6BAAACQAAAPRYAQAAAAAAoxIAAKMSAAB7BAAACAAAAP1YAQAAAAAAul0AALdeAAB8BAAACQAAAAVZAQAAAAAAMFcAADpaAAB9BAAACQAAAA5ZAQAAAAAABF4AAAReAAB+BAAAAwAAABdZAQAAAAAARS8AAEUvAAB/BAAABwAAABpZAQAAAAAAfGMAAIpgAACABAAACgAAACFZAQAAAAAArEQAAFU8AACBBAAACwAAACtZAQAAAAAArkUAAHM8AACCBAAACwAAADZZAQAAAAAAEiIAAHQiAACDBAAACwAAAEFZAQAAAAAABnQAAJxmAACEBAAACgAAAExZAQAAAAAA/HMAAKFmAACFBAAACgAAAFZZAQAAAAAA2zoAABYpAACGBAAACwAAAGBZAQAAAAAAHT4AAPwoAACHBAAADQAAAGtZAQAAAAAAkHQAACN1AACIBAAADQAAAHhZAQAAAAAAj3EAAIN0AACJBAAADQAAAIVZAQAAAAAAOGwAAOBuAACKBAAADQAAAJJZAQAAAAAA9moAAI5rAACLBAAADQAAAJ9ZAQAAAAAAaWcAAHpqAACMBAAADQAAAKxZAQAAAAAAVmYAAC9nAACNBAAADQAAALlZAQAAAAAAv2MAABZmAACOBAAADQAAAMZZAQAAAAAAKWMAAJtjAACPBAAADQAAANNZAQAAAAAAw2AAAP9iAACQBAAADQAAAOBZAQAAAAAATWAAAJ9gAACRBAAADQAAAO1ZAQAAAAAALDwAACw8AACSBAAAAgAAAPpZAQAAAAAASjAAADIgAACTBAAABQAAAPxZAQAAAAAAg2wAACZsAACUBAAACAAAAAFaAQAAAAAACEoAAAhKAACVBAAACAAAAAlaAQAAAAAArioAAK4qAACWBAAACQAAABFaAQAAAAAAaEMAAGhDAACXBAAACQAAABpaAQAAAAAAoS4AAKEuAACYBAAACAAAACNaAQAAAAAAfCoAAHwqAACZBAAACQAAACtaAQAAAAAAMUMAADFDAACaBAAACQAAADRaAQAAAAAARCIAAEQiAACbBAAABwAAAD1aAQAAAAAALEoAACxKAACcBAAACAAAAERaAQAAAAAA4mYAAOJmAACdBAAACQAAAExaAQAAAAAAyi4AAMouAACeBAAACAAAAFVaAQAAAAAAtGYAALRmAACfBAAACQAAAF1aAQAAAAAAYVoAAGxdAACgBAAACQAAAGZaAQAAAAAAC1cAABNaAAChBAAACQAAAG9aAQAAAAAANVYAAORWAACiBAAACQAAAHhaAQAAAAAAi0YAAItGAACjBA=="),Q(r,66992,"dBoAAHQaAACkBA=="),Q(r,67016,"b0QAAG9EAAClBA=="),Q(r,67040,"UEIAAFBCAACmBA=="),Q(r,67064,"cUUAAHFFAACnBA=="),Q(r,67088,"7kMAAO5DAACoBA=="),Q(r,67112,"WWoAAFlqAACpBAAACAAAAIFaAQAAAAAAjWUAAI1lAACqBAAACAAAAIlaAQ=="),Q(r,67172,"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"),Q(r,76208,"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"),Q(r,80960,"KoZIhvcNKoZIhvcNASqGSIb3DQICKoZIhvcNAgUqhkiG9w0DBCqGSIb3DQEBASqGSIb3DQEBAiqGSIb3DQEBBCqGSIb3DQEFASqGSIb3DQEFA1VVBFUEA1UEBlUEB1UECFUEClUEC1UIAQEqhkiG9w0BByqGSIb3DQEHASqGSIb3DQEHAiqGSIb3DQEHAyqGSIb3DQEHBCqGSIb3DQEHBSqGSIb3DQEHBiqGSIb3DQEDKoZIhvcNAQMBKw4DAgYrDgMCCSsOAwIHKw4DAhErBgEEAYE8BwEBAiqGSIb3DQMCKw4DAhIrDgMCDyqGSIb3DQMHKw4DAggqhkiG9w0BCSqGSIb3DQEJASqGSIb3DQEJAiqGSIb3DQEJAyqGSIb3DQEJBCqGSIb3DQEJBSqGSIb3DQEJBiqGSIb3DQEJByqGSIb3DQEJCCqGSIb3DQEJCWCGSAGG+EJghkgBhvhCAWCGSAGG+EICKw4DAhoqhkiG9w0BAQUrDgMCDSsOAwIMKoZIhvcNAQULKoZIhvcNAQUMKw4DAhtghkgBhvhCAQFghkgBhvhCAQJghkgBhvhCAQNghkgBhvhCAQRghkgBhvhCAQdghkgBhvhCAQhghkgBhvhCAQxghkgBhvhCAQ1ghkgBhvhCAgVVHVUdDlUdD1UdEFUdEVUdElUdE1UdFFUdIFUdIysGAQQBl1UBAlUIA2VVCANkVQQqVQQEVQQrCZImiZPyLGQBLFUdHysOAwIDVQQFVQQMVQQNKoZIhvZ9B0IKKoZIhvZ9B0IMKoZIzjgEAysOAwIdKoZIzjgEASskAwIBKyQDAwECKoZIhvcNAwgqhkiG9w0BCRADCFUdJSsGAQUFBysGAQUFBwMrBgEFBQcDASsGAQUFBwMCKwYBBQUHAwMrBgEFBQcDBCsGAQUFBwMIKwYBBAGCNwIBFSsGAQQBgjcCARYrBgEEAYI3CgMBKwYBBAGCNwoDAysGAQQBgjcKAwRghkgBhvhCBAFVHRtVHRVVHRgrZQEEASqGSIb3DQEMAQEqhkiG9w0BDAECKoZIhvcNAQwBAyqGSIb3DQEMAQQqhkiG9w0BDAEFKoZIhvcNAQwBBiqGSIb3DQEMCgEBKoZIhvcNAQwKAQIqhkiG9w0BDAoBAyqGSIb3DQEMCgEEKoZIhvcNAQwKAQUqhkiG9w0BDAoBBiqGSIb3DQEJFCqGSIb3DQEJFSqGSIb3DQEJFgEqhkiG9w0BCRYCKoZIhvcNAQkXASqGSIb3DQEFDSqGSIb3DQEFDiqGSIb3DQIHKwYBBQUHAgErBgEFBQcCAiqGSIb3DQEJDyqGSIb3DQEFBCqGSIb3DQEFBiqGSIb3DQEFCisGAQQBgjcCAQ4qhkiG9w0BCQ5VBClVBC4rBgEFBQcBKwYBBQUHMCsGAQUFBwEBKwYBBQUHMAErBgEFBQcwAisGAQUFBwMJKiqGSCqGSM44KoZIzjgEKoZIhvcNAQEqhkiG9w0BBSqGSIb3DQEJECqGSIb3DQEJEAAqhkiG9w0BCRABKoZIhvcNAQkQAiqGSIb3DQEJEAMqhkiG9w0BCRAEKoZIhvcNAQkQBSqGSIb3DQEJEAYqhkiG9w0BCRAAASqGSIb3DQEJEAACKoZIhvcNAQkQAAMqhkiG9w0BCRAABCqGSIb3DQEJEAAFKoZIhvcNAQkQAAYqhkiG9w0BCRAAByqGSIb3DQEJEAAIKoZIhvcNAQkQAQEqhkiG9w0BCRABAiqGSIb3DQEJEAEDKoZIhvcNAQkQAQQqhkiG9w0BCRABBSqGSIb3DQEJEAEGKoZIhvcNAQkQAQcqhkiG9w0BCRABCCqGSIb3DQEJEAIBKoZIhvcNAQkQAgIqhkiG9w0BCRACAyqGSIb3DQEJEAIEKoZIhvcNAQkQAgUqhkiG9w0BCRACBiqGSIb3DQEJEAIHKoZIhvcNAQkQAggqhkiG9w0BCRACCSqGSIb3DQEJEAIKKoZIhvcNAQkQAgsqhkiG9w0BCRACDCqGSIb3DQEJEAINKoZIhvcNAQkQAg4qhkiG9w0BCRACDyqGSIb3DQEJEAIQKoZIhvcNAQkQAhEqhkiG9w0BCRACEiqGSIb3DQEJEAITKoZIhvcNAQkQAhQqhkiG9w0BCRACFSqGSIb3DQEJEAIWKoZIhvcNAQkQAhcqhkiG9w0BCRACGCqGSIb3DQEJEAIZKoZIhvcNAQkQAhoqhkiG9w0BCRACGyqGSIb3DQEJEAIcKoZIhvcNAQkQAh0qhkiG9w0BCRADASqGSIb3DQEJEAMCKoZIhvcNAQkQAwMqhkiG9w0BCRADBCqGSIb3DQEJEAMFKoZIhvcNAQkQAwYqhkiG9w0BCRADByqGSIb3DQEJEAQBKoZIhvcNAQkQBQEqhkiG9w0BCRAFAiqGSIb3DQEJEAYBKoZIhvcNAQkQBgIqhkiG9w0BCRAGAyqGSIb3DQEJEAYEKoZIhvcNAQkQBgUqhkiG9w0BCRAGBiqGSIb3DQIEKwYBBQUHACsGAQUFBwIrBgEFBQcEKwYBBQUHBSsGAQUFBwYrBgEFBQcHKwYBBQUHCCsGAQUFBwkrBgEFBQcKKwYBBQUHCysGAQUFBwwrBgEFBQcAASsGAQUFBwACKwYBBQUHAAMrBgEFBQcABCsGAQUFBwAFKwYBBQUHAAYrBgEFBQcABysGAQUFBwAIKwYBBQUHAAkrBgEFBQcACisGAQUFBwALKwYBBQUHAAwrBgEFBQcADSsGAQUFBwAOKwYBBQUHAA8rBgEFBQcAECsGAQUFBwECKwYBBQUHAQMrBgEFBQcBBCsGAQUFBwEFKwYBBQUHAQYrBgEFBQcBBysGAQUFBwEIKwYBBQUHAQkrBgEFBQcCAysGAQUFBwMFKwYBBQUHAwYrBgEFBQcDBysGAQUFBwMKKwYBBQUHBAErBgEFBQcEAisGAQUFBwQDKwYBBQUHBAQrBgEFBQcEBSsGAQUFBwQGKwYBBQUHBAcrBgEFBQcECCsGAQUFBwQJKwYBBQUHBAorBgEFBQcECysGAQUFBwQMKwYBBQUHBA0rBgEFBQcEDisGAQUFBwQPKwYBBQUHBQErBgEFBQcFAisGAQUFBwUBASsGAQUFBwUBAisGAQUFBwUBAysGAQUFBwUBBCsGAQUFBwUBBSsGAQUFBwUBBisGAQUFBwUCASsGAQUFBwUCAisGAQUFBwYBKwYBBQUHBgIrBgEFBQcGAysGAQUFBwYEKwYBBQUHBwErBgEFBQcHAisGAQUFBwcDKwYBBQUHBwQrBgEFBQcHBSsGAQUFBwcGKwYBBQUHBwcrBgEFBQcHCCsGAQUFBwcJKwYBBQUHBworBgEFBQcHCysGAQUFBwcPKwYBBQUHBxArBgEFBQcHESsGAQUFBwcSKwYBBQUHBxMrBgEFBQcHFSsGAQUFBwcWKwYBBQUHBxcrBgEFBQcHGCsGAQUFBwgBKwYBBQUHCQErBgEFBQcJAisGAQUFBwkDKwYBBQUHCQQrBgEFBQcJBSsGAQUFBwoBKwYBBQUHCgIrBgEFBQcKAysGAQUFBwoEKwYBBQUHCgUrBgEFBQcLASsGAQUFBwwBKwYBBQUHDAIr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"),Q(r,88736,"BgAAAAYAAAAEAAAAb14AADg/AAB3AAAAeAAAAHkAAAB6AAAAewAAAHwAAAB9AAAAfgAAAH8AAACA"),Q(r,88820,"gQAAAIIAAACDAAAAhAAAAIUAAACGAAAAhwAAAIgAAACJ"),Q(r,88880,"EwAAAAYAAAAB"),Q(r,89024,"kAMAAJADAAAEAAAAnkwAAPY+AAB3AAAAeAAAAHkAAAB6AAAAewAAAHwAAAB9AAAAfgAAAH8AAACA"),Q(r,89108,"gQAAAIIAAACD"),Q(r,89128,"hgAAAIcAAAAAAAAAiQ=="),Q(r,89180,"uR8AACCA"),Q(r,89196,"BAAAAD5BAAAggA=="),Q(r,89216,"CAAAAK4QAAAggAAAAQAAABAAAABQXAEAAwAAAAAAAAAUAAAARVA="),Q(r,89265,"EAAAAAAAAAQAAADTJwAAPIA="),Q(r,89292,"EAAAAD8pAAAEgA=="),Q(r,89312,"FAAAACY+AAAEgA=="),Q(r,89332,"GAAAAD5BAAAggA=="),Q(r,89352,"HAAAAMUiAAAggA=="),Q(r,89372,"IAAAABEgAAAggA=="),Q(r,89392,"JAAAACFxAAAggA=="),Q(r,89412,"KAAAABxxAAAggA=="),Q(r,89432,"LAAAAIMgAAAggAAABQAAAAAAAAAwAAAALhUAAIxcAQ=="),Q(r,89480,"igAAAAAAAAABAAAAEAAAALBcAQAKAAAAeF0BAGAAAAC+BQ=="),Q(r,89528,"EAAAAD8pAAAEgA=="),Q(r,89548,"FAAAACY+AAAEgA=="),Q(r,89576,"igAAAAAAAAABAAAAEAAAALBdAQACAAAA2F0BAGAAAADbBQAAAAAAAJE="),Q(r,89628,"9ioAAOh/AACRAAAAAQAAAAQAAADlKgAA6H8AAJEAAAACAAAACAAAANMvAABMfwAAkQAAAAMAAAAMAAAAGEAAAEx/"),Q(r,89712,"iwAAAAAAAAABAAAAEAAAABBeAQAEAAAAYF4BABQAAADOTAAABgAAAAIAAACVAAAAlgAAAJc="),Q(r,89780,"mAAAAAAAAACZAAAAAAAAAJoAAAAAAAAAmw=="),Q(r,89828,"nAAAAAAAAACd"),Q(r,89848,"ngAAAJ8="),Q(r,89880,"kAMAAAIAAACVAAAAlgAAAJc="),Q(r,89912,"mAAAAKAAAACZAAAAoAAAAJo="),Q(r,89980,"ngAAAJ8="),Q(r,90032,"mC+KQpFEN3HP+8C1pdu16VvCVjnxEfFZpII/ktVeHKuYqgfYAVuDEr6FMSTDfQxVdF2+cv6x3oCnBtybdPGbwcFpm+SGR77vxp3BD8yhDCRvLOktqoR0StypsFzaiPl2UlE+mG3GMajIJwOwx39Zv/ML4MZHkafVUWPKBmcpKRSFCrcnOCEbLvxtLE0TDThTVHMKZbsKanYuycKBhSxykqHov6JLZhqocItLwqNRbMcZ6JLRJAaZ1oU1DvRwoGoQFsGkGQhsNx5Md0gntbywNLMMHDlKqthOT8qcW/NvLmjugo90b2OleBR4yIQIAseM+v++kOtsUKT3o/m+8nhxxiYEAAAmBAAAAAAAAFBSAAANPw=="),Q(r,90316,"oQ=="),Q(r,90336,"og=="),Q(r,90376,"owAAAKQ="),Q(r,90416,"pQAAAAAAAACmAAAAAAAAACYEAAAEAAAAqAAAAKkAAACq"),Q(r,90464,"qw=="),Q(r,90492,"rAAAAK0="),Q(r,90532,"rgAAAK8="),Q(r,90588,"SBAAAIR/AAACAAAAAAAAAAQAAAAuDAAAoH8AAAEAAAAQAAAA0GEBAAIAAAAAAAAACAAAABRU"),Q(r,90668,"ixsAAOh/"),Q(r,90684,"BAAAAJgFAABofw=="),Q(r,90712,"sAAAAAAAAAABAAAAEAAAACBiAQACAAAASGIBAAwAAAD8SgAAAAAAAAABFwIdGBMDHhsZCxQIBA0fFhwSGgoHDBURCQYQBQ8O"),Q(r,90784,"oFoBADBbAQBQkwAAwFsBALBgAQ=="),Q(r,90812,"DAAAAA0AAAAAAAAADgAAAA8AAAAQ"),Q(r,90844,"EQAAAA0AAAAAAAAAEgAAAA8AAAAQ"),Q(r,90876,"EwAAABQAAAAVAAAAFgAAABcAAAAYAAAAUQsAACEAAAAiAAAAIwAAACQAAAAlAAAAJgAAACcAAAAoAAAAKQAAANs+"),Q(r,90992,"lF4BAOCTAAAYXwEA7JQAAGiUAABAYQEA/////2gAAABpAAAAagAAAGsAAACrBAAApGMBAGBeAACOAAAAjwAAAJAAAACRAAAAkgAAAI0AAACTAAAAlAAAAAAE"),Q(r,91104,"kG6B");var Na,Ga=(Na=[null,af,function(A,r){return r|=0,a[(A|=0)>>2]-a[r>>2]|0},function(A){1&i[16+(A|=0)|0]&&SQ(A,31289,0)},af,function(A,r,e){e|=0;var i=0,Q=0;if(!(A|=0))return 0;A:{r:{e:{i:switch(1+(r|=0)|0){case 0:r=ci(A);default:for(i=31840;;){if(a[i+4>>2]==(0|r)&&!si(a[i>>2],A,r))break e;if(i=i+12|0,49==(0|(Q=Q+1|0)))break}return 0;case 4:}if(!si(A,20397,3)){i=10502;break r}for(i=31840;;){if(3==a[i+4>>2]&&!si(a[i>>2],A,3))break e;if(i=i+12|0,49==(0|(Q=Q+1|0)))break}return 0}if(Q=0,a[22973]=i,!(A=a[i+8>>2])|65536&A)break A;if(!(i=A>>>0<=30?a[32432+(A<<2)>>2]:0))break A}a[e>>2]=a[e>>2]|i,Q=1}return 0|Q},function(A,r){var e,Q=0,f=0,t=0,b=0,k=0,c=0,C=0,E=0,o=0,l=0;A:{r:if(e=Ni(r|=0,a[8+(A|=0)>>2])){if((0|Vf(e))<=0)return 1;for(;;){if(A=UQ(e,l),t=a[A+4>>2],C=kQ(A=a[A+8>>2],44)){if(k=171,!B[C+1|0])break r;for(r=C+1|0;r=(f=r)+1|0,Mf(i[0|f],8););for(;r=A,A=A+1|0,Mf(i[0|r],8););for(;;){if(Mf(i[0|(A=C-1|0)],8)){if(C=A,(0|A)!=(0|r))continue;break r}break}if(!(C=iQ(1+(A=C-r|0)|0,31289,0))){Ve(13,124,65);break r}r=yQ(C,r,A),i[A+r|0]=0}else C=t,f=A,r=0;Q=f,b=0,s=E=s-32|0;e:{i:{Q:{f:if(t){a[E+8>>2]=t,A=t;a:{B:{if(f=a[23070]){if(a[E>>2]=1,a[E+4>>2]=E+8,A=Kr(f,E))break B;A=a[E+8>>2]}for(f=1186;;){t:{if(c=a[76208+((k=(f+b|0)/2|0)<<2)>>2],(0|(o=mi(A,a[n(c,24)+38480>>2])))<0)f=k;else{if(!o)break t;b=k+1|0}if((0|f)>(0|b))continue;break f}break}A=n(c,24)+38488|0;break a}A=a[A+4>>2]+8|0}if(a[A>>2])break Q}if(!C)break i;a[E+12>>2]=C,A=C;f:{a:{if(f=a[23070]){if(a[E>>2]=2,a[E+4>>2]=E+8,A=Kr(f,E))break a;A=a[E+12>>2]}for(f=1186,b=0;;){B:{if(c=a[71456+((k=(f+b|0)/2|0)<<2)>>2],(0|(o=mi(A,a[n(c,24)+38484>>2])))<0)f=k;else{if(!o)break B;b=k+1|0}if((0|f)>(0|b))continue;break i}break}A=n(c,24)+38488|0;break f}A=a[A+4>>2]+8|0}if(!a[A>>2])break i}b=0,Ve(8,100,102);break e}if(A=0,s=b=s-32|0,!((0|(f=BA(0,0,Q)))<=0||(0|(c=Ke(0,f,6)))<0))if(k=iQ(c,31289,0)){a[b+8>>2]=k,jA(b+8|0,0,f,6,0),BA(a[b+8>>2],f,Q),a[b>>2]=k,s=Q=s-16|0,a[Q+12>>2]=a[b>>2],A=102;i:if(128&MA(Q+12|0,Q+8|0,Q+4|0,Q,c)||(A=116,6!=a[Q+4>>2]))f=0,Ve(13,147,A);else{if(!(f=LA(0,Q+12|0,a[Q+8>>2]))){f=0;break i}a[b>>2]=a[Q+12>>2]}s=Q+16|0,A=f,SQ(k,31289,0)}else Ve(8,108,65);if(s=b+32|0,A){if(gr(A))b=0,Ve(8,100,102);else{f=a[22759],a[22759]=f+1,a[A+4>>2]=C,a[A>>2]=t,a[A+8>>2]=f,k=0,f=0,b=0,C=0;i:{Q:{if(a[23070]||(t=Mi(116,117),a[23070]=t,t)){t=0;f:if(A)if(1&i[A+20|0]){if((t=Ki(24,31289))?a[t+20>>2]=1:Ve(13,123,65),t){a[t+20>>2]=13|a[A+20>>2];a:{if((0|(Q=a[A+12>>2]))>0){if(Q=VQ(a[A+16>>2],Q),a[t+16>>2]=Q,!Q)break a;Q=a[A+12>>2]}if(a[t+12>>2]=Q,a[t+8>>2]=a[A+8>>2],!(Q=a[A+4>>2])||(Q=Pr(Q),a[t+4>>2]=Q,Q)){if(!(Q=a[A>>2]))break f;if(Q=Pr(Q),a[t>>2]=Q,Q)break f}}oi(t),t=65}else t=13;Ve(8,101,t),t=0}else t=A;if(c=t){if(t=0,(b=iQ(8,31289,0))&&(Q=0,!a[c+12>>2]|!a[A+16>>2]||(t=0,Q=iQ(8,31289,0))))if(!a[c>>2]||(f=iQ(8,31289,0))){if(!a[c+4>>2])break Q;if(k=iQ(8,31289,0))break Q;C=f,t=Q}else t=Q;k=t,Ve(8,105,65)}SQ(k,31289,0),SQ(C,31289,0),SQ(0,31289,0),SQ(b,31289,0),oi(c)}b=0;break i}Q&&(a[Q+4>>2]=c,a[Q>>2]=0,SQ(uA(a[23070],Q),31289,0)),f&&(a[f+4>>2]=c,a[f>>2]=1,SQ(uA(a[23070],f),31289,0)),k&&(a[k+4>>2]=c,a[k>>2]=2,SQ(uA(a[23070],k),31289,0)),a[b+4>>2]=c,a[b>>2]=3,SQ(uA(a[23070],b),31289,0),a[c+20>>2]=-14&a[c+20>>2],b=a[c+8>>2]}a[A>>2]=0,a[A+4>>2]=0}oi(A)}else b=0}if(s=E+32|0,SQ(r,31289,0),!b)break;if(l=l+1|0,A=1,(0|Vf(e))<=(0|l))break A}k=171}else k=172;Ve(13,174,k),A=0}return 0|A},function(A){},function(A,r){var e,Q=0,f=0,t=0,b=0,k=0,c=0,C=0,E=0,o=0,l=0,w=0,g=0;s=e=s-48|0;A:{r:{if(w=Ni(r|=0,a[8+(A|=0)>>2])){if((0|Vf(w))>0)break r;r=1;break A}r=0,Ve(13,223,172);break A}r:{e:{for(;;){i:{A=UQ(w,g),l=a[A+8>>2],f=a[A+4>>2],a[e+40>>2]=0,C=0,s=t=s-32|0,r=f,a[t+8>>2]=r;Q:{f:{if(A=a[23070]){if(a[t>>2]=1,a[t+4>>2]=t+8,A=Kr(A,t))break f;r=a[t+8>>2]}Q=1186;a:{for(;;){if(c=a[76208+((A=(Q+C|0)/2|0)<<2)>>2],(0|(b=mi(r,a[n(c,24)+38480>>2])))<0)Q=A;else{if(!b)break a;C=A+1|0}if(!((0|Q)>(0|C)))break}C=0;break Q}C=a[n(c,24)+38488>>2];break Q}C=a[a[A+4>>2]+8>>2]}if(s=t+32|0,!C){C=0,s=t=s-32|0,r=f,a[t+12>>2]=r;Q:{f:{if(A=a[23070]){if(a[t>>2]=2,a[t+4>>2]=t+8,A=Kr(A,t))break f;r=a[t+12>>2]}Q=1186;a:{for(;;){if(c=a[71456+((A=(Q+C|0)/2|0)<<2)>>2],(0|(b=mi(r,a[n(c,24)+38484>>2])))<0)Q=A;else{if(!b)break a;C=A+1|0}if(!((0|Q)>(0|C)))break}C=0;break Q}C=a[n(c,24)+38488>>2];break Q}C=a[a[A+4>>2]+8>>2]}if(s=t+32|0,!C)break i}r=0,s=k=s-16|0,a[k+12>>2]=0;Q:{f:{if(t=Pr(l)){for(o=1,b=A=t;!(1<<(Q=B[0|b])&9217&&Q>>>0<=13);){a:{B:switch(o-1|0){case 0:if(o=1,Q=Q-44|0){if(14!=(0|Q))break a;if(i[0|b]=0,E=108,r=A,!(A=B[0|A]))break f;for(;;){if(Mf(A<<24>>24,8)){if(A=B[r+1|0],r=r+1|0,A)continue;break f}break}if(!B[0|r])break f;A=ci(r)+r|0;t:{for(;;){if(Q=A,(0|r)==(0|(A=A-1|0)))break t;if(!Mf(i[0|A],8))break}i[0|Q]=0}if(!B[0|r])break f;A=b=b+1|0,o=2;continue}if(i[0|b]=0,E=108,r=A,!(A=B[0|A]))break f;for(;;){if(Mf(A<<24>>24,8)){if(A=B[r+1|0],r=r+1|0,A)continue;break f}break}if(!B[0|r])break f;A=ci(r)+r|0;t:{for(;;){if(Q=A,(0|r)==(0|(A=A-1|0)))break t;if(!Mf(i[0|A],8))break}i[0|Q]=0}if(!B[0|r])break f;YA(r,0,0,k+12|0),A=b=b+1|0;continue;case 1:break B;default:break a}if(o=2,44==(0|Q)){if(i[0|b]=0,E=109,!(Q=B[0|A]))break f;for(;;){if(Mf(Q<<24>>24,8)){if(Q=B[A+1|0],A=A+1|0,Q)continue;break f}break}if(!B[0|A])break f;Q=ci(A)+A|0;B:{for(;;){if(c=Q,(0|(Q=Q-1|0))==(0|A))break B;if(!Mf(i[0|Q],8))break}i[0|c]=0}if(!B[0|A])break f;YA(r,A,ci(A),k+12|0),o=1,A=b+1|0,r=0}}b=b+1|0}if(Q=B[0|A],2!=(0|o)){if(E=108,!(255&Q))break f;for(;;){if(Mf(Q<<24>>24,8)){if(Q=B[A+1|0],A=A+1|0,Q)continue;break f}break}if(!B[0|A])break f;r=ci(A)+A|0;a:{for(;;){if(Q=r,(0|(r=r-1|0))==(0|A))break a;if(!Mf(i[0|r],8))break}i[0|Q]=0}if(!B[0|A])break f;YA(A,0,0,k+12|0)}else{if(E=109,!(255&Q))break f;for(;;){if(Mf(Q<<24>>24,8)){if(Q=B[A+1|0],A=A+1|0,Q)continue;break f}break}if(!B[0|A])break f;Q=ci(A)+A|0;a:{for(;;){if(c=Q,(0|(Q=Q-1|0))==(0|A))break a;if(!Mf(i[0|Q],8))break}i[0|c]=0}if(!B[0|A])break f;YA(r,A,ci(A),k+12|0)}SQ(t,31289,0),A=a[k+12>>2];break Q}E=65}Ve(34,109,E),SQ(t,31289,0),Ti(a[k+12>>2],10),A=0}if(s=k+16|0,A){if(o=-1,b=0,r=0,Q=-1,(0|Vf(A))>0){for(;;){E=UQ(A,r);Q:if(mi(f=a[E+4>>2],10256)){if(mi(f,1806)){if(!mi(f,11917)){if(k=a[E+8>>2],a[(l=e+40|0)>>2]=0,k)for(;;){t=0;f:if(f=B[0|k]){for(;;){if(!pf(255&(t=f)))break f;if(f=B[k+1|0],k=k+1|0,!f)break}t=0}if(c=Uf(k,124),255&t&&(0|c)!=(0|k)){for((f=c)||(f=ci(k)+k|0);pf(B[0|(f=f-1|0)]););f=0|Ga[5](k,1+(f-k|0)|0,l)}else f=0|Ga[5](0,0,l);if((0|f)<=0||(f=1,k=c+1|0,!c))break}else Ve(14,119,115),f=0;if(!f)break e;if(a[e+40>>2])break Q;break e}if(mi(f,5924))break e;if(mi(f=a[E+8>>2],11915)){if(b=1,mi(f,13955))break e}else b=2}else if(o=lA(a[E+8>>2],e+44|0),B[a[e+44>>2]])break e}else if(Q=lA(a[E+8>>2],e+44|0),B[a[e+44>>2]])break e;if(r=r+1|0,!((0|Vf(A))>(0|r)))break}r=a[e+40>>2]}f=Q,t=r,s=c=s-32|0;Q:{f:{a:{if((r=a[22971])||((r=Ki(20,31289))&&(a[r+16>>2]=1),a[22971]=r,r)){if(a[c+8>>2]=C,!(!(Q=(0|(r=nr(r,c+8|0)))>=0?UQ(a[22971],r):Yi(c+8|0,31296,27,20,2))|!(1&i[Q+16|0]))){r=Q;break f}if(r=Ki(20,31289)){if(ir(a[22971],r))break a;SQ(r,31289,0)}else Ve(13,138,65)}Q=0,Ve(13,129,65);break Q}Q?(a[r>>2]=a[Q>>2],a[r+4>>2]=a[Q+4>>2],a[r+8>>2]=a[Q+8>>2],a[r+12>>2]=a[Q+12>>2],Q=1|a[Q+16>>2]):(a[r+4>>2]=-1,a[r+8>>2]=-1,a[r>>2]=C,Q=1),a[r+16>>2]=Q}(0|f)>=0&&(a[r+4>>2]=f),(0|o)>=0&&(a[r+8>>2]=o),t&&(a[r+12>>2]=t),Q=1,b&&(a[r+16>>2]=1|b)}if(s=c+32|0,!Q){Ve(13,222,65);break r}if(Ti(A,10),r=1,g=g+1|0,(0|Vf(w))>(0|g))continue;break A}}break}A=0,Ve(13,222,218),a[e+12>>2]=l,a[e+8>>2]=24377,a[e+4>>2]=f,a[e>>2]=24395,br(4,e);break r}Ve(13,222,218),r=a[E+4>>2],a[e+28>>2]=a[E+8>>2],a[e+24>>2]=24377,a[e+20>>2]=r,a[e+16>>2]=24420,br(4,e+16|0)}Ti(A,10),r=0,Ve(13,223,219)}return s=e+48|0,0|r},function(A){A|=0,(A=a[22971])&&(a[22971]=0,Ti(A,3))},function(A){(A|=0)&&(SQ(a[A+4>>2],31289,0),SQ(a[A+8>>2],31289,0),SQ(a[A>>2],31289,0),SQ(A,31289,0))},function(A,r){return r|=0,0|($i(a[(A|=0)>>2],a[r>>2],(0|(A=a[A+4>>2]))<(0|(r=a[r+4>>2]))?A:r)||A-r|0)},function(A,r){return r|=0,r=A|=0,A=KQ(1),a[r>>2]=A,0!=(0|A)|0},function(A,r){var e;r|=0,(e=a[(A|=0)>>2])&&(1&i[r+20|0]?yi(e):xi(e),a[A>>2]=0)},function(A,r,e,Q,f,B){if(r|=0,e|=0,Q|=0,B|=0,!(Q=a[(A|=0)>>2])&&(Q=KQ(1),a[A>>2]=Q,!Q))return 0;if(hA(r,e,Q))A=1;else{if(!(r=a[A>>2]))return 0;1&i[B+20|0]?yi(r):xi(r),a[A>>2]=0,A=0}return 0|A},function(A,r,e,Q){return r|=0,e|=0,Q|=0,(A=a[(A|=0)>>2])?(Q=!(e=7&Dr(A)),r&&(e||(i[0|r]=0,r=r+1|0),ve(A,r)),((Dr(A)+7|0)/8|0)+Q|0):-1},function(A,r,e,i,Q){A|=0,e|=0,i|=0,Q|=0;var f=0,b=0,k=0;i=0,Q=a[(r|=0)>>2];A:if(!(a[Q+12>>2]&&1!=(0|Ar(A,30163,1))||ka(Q)&&1!=(0|Ar(A,30105,1))||(i=1,(0|(r=a[Q+4>>2]))<=0)))for(;;){r:{e:{i:{Q:{f:{if(!f&(i=a[(b=(e=r-1|0)<<2)+a[Q>>2]>>2])>>>0<268435456){if(k=0,i>>>0<16777216)break f;i=i>>>24|0}else{if(1!=(0|Ar(A,37056+(i>>>28|0)|0,1)))break e;i=15&B[3+(b+a[Q>>2]|0)|0]}if(1!=(0|Ar(A,i+37056|0,1)))break e;i=a[b+a[Q>>2]>>2],k=1}if((f=i>>>20&15)||k){if(1!=(0|Ar(A,f+37056|0,1)))break e;f=15&t[2+(b+a[Q>>2]|0)>>1]}else if(!(f=i>>>16&15)){k=0;break Q}if(1!=(0|Ar(A,f+37056|0,1)))break e;i=a[b+a[Q>>2]>>2],k=1}if((f=i>>>12&15)||k){if(1!=(0|Ar(A,f+37056|0,1)))break e;f=a[b+a[Q>>2]>>2]>>>8&15}else if(!(f=i>>>8&15)){k=0;break i}if(1!=(0|Ar(A,f+37056|0,1)))break e;i=a[b+a[Q>>2]>>2],k=1}if((f=i>>>4&15)||k){if(1!=(0|Ar(A,f+37056|0,1)))break e;i=15&a[b+a[Q>>2]>>2]}else if(!(i&=15)){f=0;break r}if(f=1,1==(0|Ar(A,i+37056|0,1)))break r}i=0;break A}if(i=1,b=r>>>0>1,r=e,!b)break}return i?(0|Qr(A,31288))>0|0:0},function(A,r){return r|=0,r=A|=0,A=KQ(9),a[r>>2]=A,0!=(0|A)|0},function(A,r,e,Q,f,B){return r|=0,e|=0,Q|=0,B|=0,(Q=a[(A|=0)>>2])||(Q=KQ(9),a[A>>2]=Q,Q)?(e=hA(r,e,Q),r=a[A>>2],e?(jf(r,4),1):r?(1&i[B+20|0]?yi(r):xi(r),a[A>>2]=0,0):0):0},function(A,r){return A|=0,r|=0,r=Ki(4,31289),a[A>>2]=r,r?A=1:(Ve(13,139,65),A=0),0|A},function(A,r){SQ(a[(A|=0)>>2],31289,0),a[A>>2]=0},function(A,r){a[a[(A|=0)>>2]>>2]=0},function(A,r,e,Q,f,t){A|=0,r|=0,e|=0,Q|=0,f|=0,t|=0;var b=0,k=0,n=0,c=0,C=0,E=0,o=0,l=0,w=0,g=0,u=0;s=Q=s-32|0,a[Q+28>>2]=r,a[Q+16>>2]=0,a[Q+20>>2]=0,a[Q+12>>2]=0;A:if((r=a[A>>2])||(r=Ki(8,31289),a[A>>2]=r,r)){r:if(e){l=Q+12|0,f=0,s=E=s-16|0;e:if(e){n=a[Q+28>>2],A=1;i:{if(1!=(0|e)){Q:{if(c=B[0|n]){if(A=0,255!=(0|c))break Q;if(C=3&(A=e-1|0),e-2>>>0<3)A=1;else for(w=-4&A,A=1;f=(o=f)|B[0|(f=A+n|0)]|B[f+1|0]|B[f+2|0]|B[f+3|0],A=A+4|0,(0|w)!=(0|(g=g+4|0)););if(C)for(;f=B[A+n|0]|f,A=A+1|0,(0|C)!=(0|(u=u+1|0)););if(!f){A=0;break Q}}if(A=0,(B[n+1|0]^c)<<24>>24>=0)break i;A=-1}if(!(A=A+e|0)){A=0;break e}if(!(A>>>0<9)){A=0,Ve(13,101,223);break e}}if(pA(E+8|0,l,n,e),c=3&A,f=0,A-1>>>0<3)A=0;else for(l=-4&A,A=0,C=0;w=B[(n=E+8|0)+(3|A)|0],k=e=k<<16|b>>>16,o=b<<=16,e=(b=B[A+n|0])>>>24|0,e|=k,b=(k=(b=o|b<<8)|B[n+(1|A)|0])<<16,k=e<<16|k>>>16,o=b,e=(b=B[n+(2|A)|0])>>>24|0,b=o|b<<8|w,k|=e,A=A+4|0,(0|l)!=(0|(C=C+4|0)););if(c)for(;k=k<<8|b>>>24,b=B[(E+8|0)+A|0]|b<<8,A=A+1|0,(0|c)!=(0|(f=f+1|0)););a[Q+16>>2]=b,a[Q+20>>2]=k,A=1;break e}Ve(13,226,221)}else A=0,Ve(13,226,222);if(s=E+16|0,!A){e=0;break A}if(!((A=2&a[t+20>>2])|!(e=a[Q+12>>2]))){e=0,Ve(13,105,226);break A}e:{if(e){if(A=a[Q+16>>2],!a[Q+20>>2]&A>>>0<2147483649)break e;e=0,Ve(13,105,224);break A}if(e=0,!((!(k=a[Q+20>>2])&(b=a[Q+16>>2])>>>0>2147483647|(f=0!=(0|k)))&0!=(0|A)|f&!A))break r;Ve(13,105,223);break A}b=0-A|0}i[0|r]=b,i[r+1|0]=b>>>8,i[r+2|0]=b>>>16,i[r+3|0]=b>>>24,e=1}else e=0,Ve(13,141,65);return s=Q+32|0,0|e},function(A,r,e,Q){r|=0,e|=0,Q|=0;var f=0,t=0,b=0,k=0,n=0,c=0,C=0,E=0,o=0;if(A=a[(A|=0)>>2],e=B[0|A]|B[A+1|0]<<8|B[A+2|0]<<16|B[A+3|0]<<24,!(1&(t=a[Q+20>>2]))||(A=-1,e)){for(Q=r,t=(A=t>>>1&(0|e)<0)?0-e|0:e,e=0,c=A,r=0,k=s-16|0,A=8;C=A,f=t,i[0|(E=(A=A-1|0)+(k+8|0)|0)]=f,n=r,r=r+1|0,b=!e&f>>>0>255|0!=(0|e),t=(255&e)<<24|f>>>8,e=e>>>8|0,b;);A:r:{e:{i:{if(9!=(0|C)){if(b=9-C|0,c){if(A=1,e=255,!((255&f)>>>0>128)&&(A=0,e=255,128==(255&f))){if(b>>>0>=2){if(e=3&n,n-1>>>0<3)A=1,r=0;else for(t=-4&n,r=0,A=1,f=0;r=(c=r)|B[0|(r=A+E|0)]|B[r+1|0]|B[r+2|0]|B[r+3|0],A=A+4|0,(0|t)!=(0|(f=f+4|0)););if(e)for(;r=B[A+E|0]|r,A=A+1|0,(0|(o=o+1|0))!=(0|e););A=0!=(0|r)}e=0-A|0}}else A=f<<24>>24<0,e=0;if(t=A+b|0,!Q)break r;if(i[0|Q]=e,f=1&e,A=(A+Q|0)+b|0,1&b)break i;r=k+16|0;break e}if(t=1,!Q)break r;i[0|Q]=0,t=1;break A}A=A-1|0,r=(B[k+15|0]^255&e)+f|0,i[0|A]=r,b=8-C|0,f=r>>>8|0,r=k+15|0}if(n)for(;Q=(255&(e^B[r-1|0]))+f|0,i[A-1|0]=Q,A=A-2|0,Q=(255&(e^B[0|(r=r-2|0)]))+(Q>>>8|0)|0,i[0|A]=Q,f=Q>>>8|0,b=b-2|0;);}A=t}return 0|A},function(A,r,e,i,Q){return A|=0,r|=0,i|=0,s=i=s-16|0,e=a[20+(e|=0)>>2],a[i>>2]=a[a[r>>2]>>2],A=Xe(A,2&e?31167:31114,i),s=i+16|0,0|A},function(A,r,e){return 0|Ar(e|=0,A|=0,r|=0)},OA,PA,function(A){var r;return r=Bi(a[(A|=0)>>2]),Bi(a[A+4>>2])^r<<2},function(A,r){A|=0;var e=0,i=0;A:{if((0|(e=a[(r|=0)>>2]))==(0|(i=a[A>>2]))||!(e=mi(i,e))){if(r=a[r+4>>2],A=a[A+4>>2]){if(r)break A;r=0,e=1}else e=-1;e=(0|A)==(0|r)?0:e}return 0|e}return 0|mi(A,r)},function(A,r){r|=0,a[4+(A|=0)>>2]&&GA(r,A)},function(A){var r=0,e=0,i=0,Q=0;if(!a[4+(A|=0)>>2]){if((0|(e=Vf(i=a[A+8>>2])))>0)for(;r=UQ(i,Q=e-1|0),SQ(a[r+8>>2],31289,0),SQ(a[r+4>>2],31289,0),SQ(r,31289,0),r=e>>>0>1,e=Q,r;);Df(i),SQ(a[A>>2],31289,0),SQ(A,31289,0)}},function(A,r){var e,i,Q;r|=0,s=e=s-32|0,i=a[(A|=0)>>2],(Q=a[A+4>>2])?(a[e+24>>2]=a[A+8>>2],a[e+20>>2]=Q,a[e+16>>2]=i,Xe(r,31155,e+16|0)):(a[e>>2]=i,Xe(r,31171,e)),s=e+32|0},function(A){var r;if(A|=0,r=iQ(12,31289,0)){if(0|Ga[a[A+8>>2]](r))return 0|r;SQ(r,31289,0)}return 0},function(A){return(A|=0)?(a[A+8>>2]=0,a[A+4>>2]=37088,a[A>>2]=90900,1):0},function(A){return(A|=0)?(oQ(A),SQ(A,31289,0),1):0},function(A){return(A|=0)?(oQ(A),1):0},function(A,r,e){A|=0,r|=0,e|=0;var Q,f,b,k=0,n=0,c=0,C=0,E=0,o=0,l=0,w=0,g=0,u=0,M=0,D=0,I=0;s=Q=s-48|0,a[Q+28>>2]=0,b=a[A+8>>2];A:{r:if(f=Bf())if(c=Pr(2905),a[Q+28>>2]=c,c)if(A&&(a[A+8>>2]||(k=Mi(28,29),a[A+8>>2]=k,k))&&(n=1),n){e:{if(!(M=Se(A,c))){Ve(14,121,103);break r}if(re(f,512)){k=1;i:for(;;){for(E=0,C=0;;){o=a[f+4>>2]+E|0,i[0|o]=0,c=o+3|0;Q:{f:{a:{if(!(1&C)){for(;;){if(Nr(r,o),i[o+511|0]=0,!(!k|(0|(C=ci(o)))<3)){if($i(o,37344,3)){l=1;break f}i[(C=C-3|0)+eQ(o,c,C)|0]=0}if(C){l=1;break a}if(!(n=NQ(0)))break;De(r),k=0,r=n}lQ(f),SQ(a[Q+28>>2],31289,0),Df(0),A=1;break A}Nr(r,o),i[o+511|0]=0,!k|(0|(C=ci(o)))<3||$i(o,37344,3)||(i[(C=C-3|0)+eQ(o,c,C)|0]=0),l=0!=(0|C)}if((0|(k=C))<=0)break Q}for(k=C;;){f:switch(B[(n=k-1|0)+o|0]-10|0){case 0:case 3:break f;default:break Q}if(c=k>>>0>1,k=n,!c)break}k=0}(w=(0|k)==(0|C)&l)||(i[k+o|0]=0,I=I+1|0);Q:{f:{if(!((0|(E=k+E|0))<=0)&&(C=(n=E-1|0)+a[f+4>>2]|0,!((0|(k=i[0|C]))<0)&&(c=a[A+4>>2],32&B[c+((255&k)<<1)|0]))){if(l=0,E>>>0<2){E=n;break Q}if((0|(k=i[C-1|0]))<0){E=n;break Q}if(!(w|!(k=32&B[c+((255&k)<<1)|0])))break f;E=k?E:n;break Q}if(l=0,w)break Q}C=a[f+4>>2],l=o=i[0|C],k=C;f:{a:{B:if(!((0|o)<0))for(E=a[A+4>>2],l=o,c=C;;){if(2048&(n=t[E+((255&l)<<1)>>1]))break a;if(!(16&n)){k=c;break B}if(l=i[c+1|0],c=k=c+1|0,!((0|l)>=0))break}for(;;){B:{if(l<<24>>24>=0){if(g=a[A+4>>2],128&(n=t[g+((u=255&l)<<1)>>1])){c=k;break a}if(1024&n){t:{for(;;){if(E=k,c=k+1|0,n=(k=B[k+1|0])<<24>>24,8&B[g+(k<<1)|0]&&(0|n)>=0)break t;if(k=c,(255&n)==(0|u)&&(k=E+2|0,B[E+2|0]!=(0|u)))break}n=l}k=(255&n)==(0|u)?E+2|0:c;break B}if(64&n){t:{for(;;)if(c=k,k=k+1|0,(0|(n=(E=B[c+1|0])<<24>>24))>=0){if(8&(E=t[g+(E<<1)>>1])|(255&n)==(0|u))break t;if(!(32&E))continue;if(k=c+2|0,(0|(n=i[c+2|0]))<0)continue;if(8&B[g+((255&n)<<1)|0])break B}else if((255&n)==(0|u))break;n=l}l=B[0|(k=(255&n)==(0|u)?c+2|0:k)];continue}if(32&n){if(!((0|(n=i[k+1|0]))<0|!(8&t[g+((255&n)<<1)>>1]))){l=B[0|(k=k+1|0)];continue}l=B[0|(k=k+2|0)];continue}if(8&n)break f}l=B[0|(k=k+1|0)];continue}l=B[0|k]}}i[0|c]=0,o=B[0|C]}l=!w;f:if(!(o<<24>>24<0)){for(w=a[A+4>>2];;){if(16==(24&(n=t[w+((k=255&o)<<1)>>1]))){if(o=i[C+1|0],C=C+1|0,(0|o)>=0)continue;break f}break}if(E=0,8&n)break Q;if(91==(0|k)){o=c=C+1|0;a:if(!((0|(C=i[C+1|0]))<0))for(;;){if(16!=(24&t[w+((255&C)<<1)>>1])){o=c;break a}if(C=i[c+1|0],c=o=c+1|0,!((0|C)>=0))break}l=o,n=o;a:{for(;;){c=l;B:if(!(C<<24>>24<0))for(;;){t:if(32&(n=t[(k=w+((255&C)<<1)|0)>>1])){if(!((0|(n=i[c+1|0]))<0|!(8&t[w+((255&n)<<1)>>1]))){c=c+1|0;break t}c=c+2|0}else{if(!(775&n)){if(E=c,16!=(24&t[k>>1])){n=c;break B}for(;;){if(n=E+1|0,(0|(C=(k=B[E+1|0])<<24>>24))<0)break B;if(E=n,16!=(24&t[w+(k<<1)>>1]))break}break B}c=c+1|0}if(!((0|(C=i[0|(n=c)]))>=0))break}if(k=255&C){if(93==(0|k))break a;if(k=(0|n)!=(0|l),l=n,k)continue}break}Ve(14,121,100);break r}if(i[0|c]=0,!O(A,0,Q+28|0,o))break r;if(!(M=JQ(A,n=a[Q+28>>2]))&&!(M=Se(A,n))){Ve(14,121,103);break r}if(k=0,!re(f,512))break e;continue i}}E=ri(A,C),58!=(0|(k=B[0|E]))|58!=B[E+1|0]?o=a[Q+28>>2]:(i[0|E]=0,E=ri(A,n=E+2|0),k=B[0|E],o=C,C=n);f:if(k<<24>>24<0)n=E;else for(w=a[A+4>>2],c=E;;){if(16!=(24&t[w+((255&k)<<1)>>1])){n=c;break f}if(k=i[c+1|0],c=n=c+1|0,!((0|k)>=0))break}f:{a:{B:{if(!si(C,15409,8)){if((C+8|0)==(0|n)&61!=(255&k))break B;a[Q+32>>2]=0;t:if(61==B[0|n])if(k=n+1|0,(0|(c=i[n+1|0]))<0)n=k;else for(E=a[A+4>>2];;){if(16!=(24&t[E+((255&c)<<1)>>1])){n=k;break t}if(c=i[k+1|0],k=n=k+1|0,!((0|c)>=0))break}if($e(A,n),!O(A,o,Q+32|0,n))break f;if(SQ(a[Q+32>>2],31289,0),k=0,!re(f,512))break e;continue i}if(61==(255&k))break a}Ve(14,121,101);break r}i[0|E]=0,k=n+1|0;a:if((0|(c=i[n+1|0]))<0)n=k;else for(E=a[A+4>>2];;){if(16!=(24&t[E+((255&c)<<1)>>1])){n=k;break a}if(c=i[k+1|0],k=n=k+1|0,!((0|c)>=0))break}if($e(A,n),!(w=iQ(12,31289,0))){Ve(14,121,65);break r}if(k=Pr(C),a[w+8>>2]=0,a[w+4>>2]=k,k){if(O(A,o,w+8|0,n))if(c=M,!mi(o,a[Q+28>>2])||(c=JQ(A,o))||(c=Se(A,o))){if(E=0,n=a[c+8>>2],a[w>>2]=a[c>>2],k=0,ir(n,w)){if(g=uA(a[A+8>>2],w)){if((0|(o=a[n>>2]))>0)a:for(C=a[n+4>>2];;){if((0|g)==a[(c=C+(k<<2)|0)>>2]){(o-1|0)!=(0|k)&&(eQ(c,4+(C+(k<<2)|0)|0,o+(-1^k)<<2),k=a[n>>2]-1|0),a[n>>2]=k;break a}if((0|o)==(0|(k=k+1|0)))break}SQ(a[g+4>>2],31289,0),SQ(a[g+8>>2],31289,0),SQ(g,31289,0)}k=1}if(k)break Q;Ve(14,121,65)}else Ve(14,121,103)}else Ve(14,121,65);D=w;break r}break r}if(C=1^l,k=0,!re(f,E+512|0))break}break}}}Ve(14,121,7)}else Ve(14,121,65);else Ve(14,121,65);else Ve(14,121,7);if(lQ(f),SQ(a[Q+28>>2],31289,0),(0|Vf(0))>0)for(;n=NQ(0),De(r),r=n,(0|Vf(0))>0;);Df(0),e&&(a[e>>2]=I),a[Q+16>>2]=I,ji(r=Q+32|0,13,16421,Q+16|0),a[Q>>2]=31060,a[Q+4>>2]=r,br(2,Q),(0|(r=a[A+8>>2]))!=(0|b)&&(s=n=s-16|0,(e=a[22977])||(a[22977]=90900,e=90900),Ga[a[e+8>>2]](n),a[n+8>>2]=r,Ga[a[a[n>>2]+16>>2]](n),s=n+16|0,a[A+8>>2]=0),A=0,D&&(SQ(a[D+4>>2],31289,0),SQ(a[D+8>>2],31289,0),SQ(D,31289,0),A=0)}return s=Q+48|0,0|A},function(A,r){return r|=0,Di(a[8+(A|=0)>>2],32,r),1},function(A,r){return r|=0,1&f[a[4+(A|=0)>>2]+((255&r)<<1)>>1]&(0|r)>=0},function(A,r){return(r|=0)-48|0},function(A,r,e){return 128==(4095&fr())?(Ve(14,120,114),0):(Ve(14,120,2),0)},function(A,r){var e,i=0,Q=0,f=0,B=0,t=0,k=0,c=0,C=0,E=0;s=e=s-32|0;A:{r:if((0|Vf(f=Ni(r|=0,A=a[8+(A|=0)>>2])))<=0)Ve(14,123,f?119:120),a[e+4>>2]=A,a[e>>2]=24360,br(2,e);else if(Ge(A=Vf(f)),i=Ki(n(A,12),31289),a[22981]=i,i){if(a[22982]=A,!A){A=1;break A}for(;;){if(A=a[22981],i=UQ(f,B),(0|Vf(t=Ni(r,a[i+8>>2])))<=0){Ve(14,123,t?117:118),A=a[i+4>>2],a[e+28>>2]=a[i+8>>2],a[e+24>>2]=24377,a[e+20>>2]=A,a[e+16>>2]=24395,br(4,e+16|0);break r}if(A=A+n(B,12)|0,i=Pr(a[i+4>>2]),a[A>>2]=i,!i)break r;if(i=Ki((k=Vf(t))<<3,31289),a[A+4>>2]=i,!i)break r;if(a[A+8>>2]=k,k)for(i=A,A=0;;){if(Q=UQ(t,A),c=a[i+4>>2]+(A<<3)|0,E=Uf(C=a[Q+4>>2],46),a[c>>2]=Pr(E?E+1|0:C),Q=Pr(a[Q+8>>2]),a[c+4>>2]=Q,!a[c>>2]|!Q)break r;if((0|k)==(0|(A=A+1|0)))break}if(A=1,!((B=B+1|0)>>>0<b[22982]))break}break A}Ge(A),A=0}return s=e+32|0,0|A},Ge,function(){i[91936]=1,a[22988]=RQ(91940)},function(A,r){var e,i=0,Q=0,f=0,B=0;s=e=s-32|0;A:{r:{if(f=Ni(r|=0,a[8+(A|=0)>>2])){if(r=0,(0|Vf(f))>0)break r;A=1;break A}A=0,Ve(6,177,165);break A}for(;;){Q=UQ(f,r);r:{if(!mi(a[Q+4>>2],15418)){s=i=s-32|0;e:{if(A=a[Q+8>>2]){if(!(mi(A,21519)&&mi(A,13249)&&mi(A,19206)&&mi(A,1687)&&mi(A,19698)&&mi(A,6012))){a[e+28>>2]=255,A=1;break e}if(!(mi(A,21539)&&mi(A,13520)&&mi(A,20598)&&mi(A,10559)&&mi(A,20546)&&mi(A,8995))){a[e+28>>2]=0,A=1;break e}}Ve(34,110,104),A=a[Q>>2],B=a[Q+4>>2],a[i+20>>2]=a[Q+8>>2],a[i+16>>2]=24535,a[i+12>>2]=B,a[i+8>>2]=24543,a[i+4>>2]=A,a[i>>2]=24526,br(6,i),A=0}if(s=i+32|0,!A){A=0,Ve(6,177,168);break A}if(a[e+28>>2]<=0)break r;A=0,Ve(6,177,167);break A}Ve(6,177,169),A=a[Q+4>>2],a[e+12>>2]=a[Q+8>>2],a[e+8>>2]=24377,a[e+4>>2]=A,a[e>>2]=24395,br(4,e)}if(A=1,r=r+1|0,!((0|Vf(f))>(0|r)))break}}return s=e+32|0,0|A},fa,function(A){return IQ((A=a[12+(A|=0)>>2])+16|0,0,76),a[A+8>>2]=-1732584194,a[A+12>>2]=271733878,a[A>>2]=1732584193,a[A+4>>2]=-271733879,1},function(A,r,e){r|=0,e|=0;var i=0,Q=0,f=0;A=a[12+(A|=0)>>2];A:if(e){Q=(i=a[A+16>>2])+(e<<3)|0,a[A+16>>2]=Q,a[A+20>>2]=a[A+20>>2]+(i>>>0>Q>>>0)+(e>>>29);r:{if(i=a[A+88>>2]){if(f=(Q=A+24|0)+i|0,e+i>>>0<64&e>>>0<=63)break r;yQ(f,r,i=64-i|0),W(A,Q,1),IQ(Q,0,68),e=e-i|0,r=r+i|0}if(e>>>0>=64&&(W(A,r,e>>>6|0),r=(-64&e)+r|0,e&=63),!e)break A;a[A+88>>2]=e,yQ(A+24|0,r,e);break A}yQ(f,r,e),a[A+88>>2]=a[A+88>>2]+e}return 1},function(A,r){r|=0;var e,Q,f=0;return Q=(e=a[12+(A|=0)>>2])+24|0,f=a[e+88>>2],i[Q+f|0]=128,(A=f+1|0)>>>0>=57&&(IQ(A+Q|0,0,63-f|0),W(e,Q,1),A=0),IQ(A+Q|0,0,56-A|0),A=a[e+20>>2],f=a[e+16>>2],i[e+80|0]=f,i[e+81|0]=f>>>8,i[e+82|0]=f>>>16,i[e+83|0]=f>>>24,i[e+84|0]=A,i[e+85|0]=A>>>8,i[e+86|0]=A>>>16,i[e+87|0]=A>>>24,W(e,Q,1),a[e+88>>2]=0,Of(Q,64),A=a[e>>2],i[0|r]=A,i[r+1|0]=A>>>8,i[r+2|0]=A>>>16,i[r+3|0]=A>>>24,A=a[e+4>>2],i[r+4|0]=A,i[r+5|0]=A>>>8,i[r+6|0]=A>>>16,i[r+7|0]=A>>>24,A=a[e+8>>2],i[r+8|0]=A,i[r+9|0]=A>>>8,i[r+10|0]=A>>>16,i[r+11|0]=A>>>24,A=a[e+12>>2],i[r+12|0]=A,i[r+13|0]=A>>>8,i[r+14|0]=A>>>16,i[r+15|0]=A>>>24,1},function(A){return 0|BQ(a[12+(A|=0)>>2])},function(A,r,e){return r|=0,e|=0,0|ce(a[12+(A|=0)>>2],r,e)},function(A,r){return 0|yA(r|=0,a[12+(A|=0)>>2])},function(A,r,e,i){A|=0,e|=0,i|=0;var Q,f=0;return s=Q=s-80|0,f=-2,29==(0|(r|=0))&&(f=0,!A|48!=(0|e)||(0|ce(A=a[A+12>>2],i,48))<=0||(a[(r=Q- -64|0)>>2]=909522486,a[r+4>>2]=909522486,a[Q+56>>2]=909522486,a[Q+60>>2]=909522486,a[Q+48>>2]=909522486,a[Q+52>>2]=909522486,a[Q+40>>2]=909522486,a[Q+44>>2]=909522486,a[Q+32>>2]=909522486,a[Q+36>>2]=909522486,ce(A,Q+32|0,40)&&yA(Q,A)&&BQ(A)&&((0|ce(A,i,48))<=0||(a[(r=Q- -64|0)>>2]=1549556828,a[r+4>>2]=1549556828,a[Q+56>>2]=1549556828,a[Q+60>>2]=1549556828,a[Q+48>>2]=1549556828,a[Q+52>>2]=1549556828,a[Q+40>>2]=1549556828,a[Q+44>>2]=1549556828,a[Q+32>>2]=1549556828,a[Q+36>>2]=1549556828,ce(A,Q+32|0,40)&&ce(A,Q,20)&&(Of(Q,20),f=1))))),s=Q+80|0,0|f},function(A,r,e){return Ve(6,173,177),0},function(A){ef(A|=0,90616)},function(A,r,e){var i;return A|=0,r|=0,e=a[4+(e|=0)>>2],i=a[e+32>>2],a[i+4>>2]=r,a[i>>2]=A,0|Ga[a[e+28>>2]](e)},af,function(A){!(A|=0)|!(1&i[A+4|0])||SQ(A,31289,0)},fa,function(){var A=0;er(0)?(A=uf(),a[23008]=A,A=0!=(0|A)):A=0,a[23010]=A},Si,function(A){return 64},PQ,function(A,r,e,i){return A|=0,i|=0,3==(0|(r|=0))?(a[i>>2]=672,A=1):A=-2,0|A},function(A,r,e){r|=0,e|=0;var i=0,Q=0;if(!a[24+(A|=0)>>2]&&(i=xQ(4))){if(!Aa(i,r,e))return wQ(i),0;a[A+24>>2]=i,Q=1}return 0|Q},function(A,r,e){r|=0,e|=0;var i=0;A=a[24+(A|=0)>>2];A:{if(r){if(!A|b[A>>2]>b[e>>2])break A;a[e>>2]=a[A>>2],yQ(r,a[A+8>>2],a[e>>2])}else a[e>>2]=a[A>>2];i=1}return 0|i},function(A,r,e){return r|=0,e|=0,0!=(0|tf(a[a[a[16+(A|=0)>>2]+20>>2]+20>>2],r,e))|0},function(A){var r,e;return A|=0,(r=Ki(24,31289))?(a[r+8>>2]=4,e=_r(),a[r+20>>2]=e,e?(a[A+36>>2]=0,a[A+20>>2]=r,1):(SQ(r,31289,0),0)):(Ve(15,123,65),0)},function(A,r){A|=0,r|=0;var e,i=0,Q=0,f=0;if(!(i=Ki(24,31289)))return Ve(15,123,65),0;if(a[i+8>>2]=4,f=_r(),a[i+20>>2]=f,!f)return SQ(i,31289,0),0;a[A+36>>2]=0,a[A+20>>2]=i,e=a[A+20>>2],f=a[r+20>>2],a[e>>2]=a[f>>2],i=a[f+20>>2],r=a[e+20>>2],(a[r+8>>2]||(Q=ta(),a[r+8>>2]=Q,Q))&&(a[r+12>>2]||(Q=ta(),a[r+12>>2]=Q,Q))&&(a[r+4>>2]||(Q=ta(),a[r+4>>2]=Q,Q))&&ur(a[r+8>>2],a[i+8>>2])&&ur(a[r+12>>2],a[i+12>>2])&&ur(a[r+4>>2],a[i+4>>2])?(a[r>>2]=a[i>>2],r=1):(Qi(a[r+8>>2]),Qi(a[r+12>>2]),Qi(a[r+4>>2]),a[r>>2]=0,r=0);A:{if(r){if(r=1,!(i=a[f+12>>2]))break A;if(Aa(e+4|0,i,a[f+4>>2]))break A}r=0,(i=a[A+20>>2])&&(CQ(a[i+20>>2]),pQ(a[i+12>>2],a[i+4>>2]),SQ(i,31289,0),Ca(A,0))}return 0|r},function(A){var r;(r=a[20+(A|=0)>>2])&&(CQ(a[r+20>>2]),pQ(a[r+12>>2],a[r+4>>2]),SQ(r,31289,0),Ca(A,0))},function(A,r){r|=0;var e=0;return A=a[20+(A|=0)>>2],a[A+12>>2]&&(A=je(A+4|0))&&(ge(r,855,A),e=1),0|e},function(A,r){var e;return r|=0,A=a[a[20+(A|=0)>>2]+20>>2],e=$f(r,-257),Kf(a[A+8>>2],e),Kf(a[A+12>>2],e),Kf(a[A+4>>2],e),Kf(r,256),Ca(r,67),1},function(A,r,e,i){var Q,f;r|=0,e|=0,i|=0,s=Q=s-16|0,f=a[20+(A|=0)>>2],A=0;A:if(!((0|(i=wf(i?a[i>>2]:0)))<0)){if(a[e>>2]=i,r){if(!Ci(a[f+20>>2],r,Q+12|0))break A;a[e>>2]=a[Q+12>>2]}A=1}return s=Q+16|0,0|A},function(A,r,e,i){r|=0,e|=0,i|=0;var Q,f=0;Q=a[20+(A|=0)>>2],f=-2;A:{r:{e:switch(r-1|0){case 5:if(f=0,!i&(0|e)>0|(0|e)<-1)break A;if(Aa(Q+4|0,i,e))break r;break A;case 6:if(r=a[a[A+8>>2]+24>>2],ZA(a[Q+20>>2],a[r+8>>2],a[r>>2],a[Q>>2]))break r;return 0;case 0:break e;default:break A}a[Q>>2]=i}f=1}return 0|f},function(A,r,e){return A|=0,(e|=0)?mi(r|=0,1439)?0|(A=mi(r,1425)?-2:aQ(A,6,e)):0|vQ(A,6,e):0},function(){var A,r=0;s=A=s-16|0,r=0,RQ(A+12|0)&&(r=uf(),a[23031]=r,r?(i[92112]=1,a[22754]=a[A+12>>2],r=1):(Yf(0),a[23031]=0,wa(A+12|0),r=0)),a[23034]=r,s=A+16|0},Da,function(){i[92152]=1},function(A){(A|=0)&&(a[A>>2]&&ti(),a[A+4>>2]&&sf(),a[A+8>>2]&&Le(),SQ(A,31289,0))},function(){var A,r=0,e=0,Q=0,f=0,t=0;if(s=A=s-16|0,!(B[92116]|!B[92112])){if(i[92116]=1,r=Af(91016),_Q(91016,0),r&&(a[r>>2]&&ti(),a[r+4>>2]&&sf(),a[r+8>>2]&&Le(),SQ(r,31289,0)),r=a[23030])for(;Ga[a[r>>2]](),e=a[r+4>>2],SQ(r,31289,0),r=e;);if(a[23030]=0,Yf(a[23031]),a[23031]=0,B[92128]&&(wa(91896),wa(91900)),r=a[22754],a[22754]=-1,a[A+12>>2]=r,wa(A+12|0),(e=a[22968])&&((r=a[e+240>>2])&&Ga[a[r+12>>2]](e),Yf(a[e>>2]),zA(15,e,e+96|0),a[e+8>>2]?x(e,260):pQ(e,260),a[22968]=0,wa(91876),wa(91880)),fi(),fi(),(0|(r=Vf(a[22978])))>0)for(e=r-1|0;r=e,f=UQ(a[22978],r),!(e=a[22978])|(0|r)<0||(0|(t=a[e>>2]))<=(0|r)||(Q=a[e+4>>2]+(r<<2)|0,(0|r)!=(t-1|0)?(eQ(Q,Q+4|0,t+(-1^r)<<2),Q=a[e>>2]-1|0):Q=r,a[e>>2]=Q),Fe(a[f>>2]),SQ(a[f+4>>2],31289,0),SQ(f,31289,0),e=r-1|0,r;);if(Vf(a[22978])||(Df(a[22978]),a[22978]=0),li(a[23074]),a[23074]=0,Yf(a[23073]),a[23073]=0,Ti(a[22992],59),a[22992]=0,Ti(a[22993],59),a[22993]=0,Ti(a[22994],59),a[22994]=0,Ti(a[22995],59),a[22995]=0,Ti(a[22996],59),a[22996]=0,Ti(a[22997],59),a[22997]=0,Ti(a[22998],59),a[22998]=0,Ti(a[22999],59),a[22999]=0,Ti(a[23e3],59),a[23e3]=0,Ti(a[23001],59),a[23001]=0,Ti(a[23002],59),a[23002]=0,Ti(a[23003],59),a[23003]=0,Ti(a[23004],59),a[23004]=0,Ti(a[23005],59),a[23005]=0,Ti(a[23006],59),a[23006]=0,Ti(a[23007],59),a[23007]=0,Yf(a[23008]),a[23008]=0,Yf(a[22976]),a[22976]=0,Vi(2),Vi(1),Vi(-1),Ti(a[22989],46),a[22989]=0,Ti(a[23071],118),a[23071]=0,Df(a[23072]),a[23072]=0,(r=a[22990])&&Ti(r,58),(r=a[23070])&&(ca(r,0),Ae(a[23070],113),Ae(a[23070],114),Ae(a[23070],115),li(a[23070]),a[23070]=0),B[91936]&&wa(91940),Yf(a[22986]),a[22986]=0,!a[23053]){if(SQ(a[23058],31289,0),SQ(a[23061],31289,0),SQ(a[23062],31289,0),-1!=(0|(t=a[23054]))&&(e=a[23055])){s=f=s-16|0,a[f+12>>2]=0,Q=f+12|0;A:{if(r=a[23335])for(;;){if((0|t)==a[r>>2])break A;if(Q&&(a[Q>>2]=r),!(r=a[r+36>>2]))break}r=0}Q=r,r&&e&&(0|e)==a[Q+4>>2]?(r=a[f+12>>2],a[(r?r+36|0:93340)>>2]=a[Q+36>>2],(32&(r=a[Q+16>>2])||!(r=0|w(0|t,0|e,a[Q+32>>2],0|r,a[Q+12>>2],a[Q+24>>2])))&&(a[Q+8>>2]&&m(a[Q>>2]),r=0,32&B[Q+16|0]||m(Q))):r=-28,s=f+16|0,r>>>0>=4294963201&&(a[23332]=0-r)}a[23062]=0,a[23063]=0,a[23060]=0,a[23061]=0,a[23058]=0,a[23059]=0,a[23056]=0,a[23057]=0,a[23054]=0,a[23055]=0,i[92204]=0,Yf(a[23052]),a[23052]=0}i[92112]=0}s=A+16|0},Da,ua,ua,Ma,Ma,sa,sa,function(){i[91920]=1,a[23046]=1},function(){var A=0,r=0,e=0,Q=0,f=0,t=0,b=0,k=0,n=0,c=0,C=0,E=0,o=0,l=0,w=0,g=0;if(A=a[23047],b=1,!B[91920]){A?(b=a[A+8>>2],f=a[A+4>>2],C=a[A>>2]):(b=50,C=0),na(9743,6,7),na(9718,8,9),na(9731,45,0),na(12963,42,43),bi(),(t=0|Ga[a[22726]](90900))||Ve(14,111,65);A:{r:{e:if(t){if(!(k=C)&&(s=Q=s-16|0,(A=ei(21387))?k=Pr(A):(k=0,(A=iQ(r=ci(11372)+13|0,31289,0))&&(a[Q+8>>2]=12972,a[Q+4>>2]=30107,a[Q>>2]=11372,ji(A,r,6670,Q),k=A)),s=Q+16|0,!k)){k=0;break r}if(t?A=0|Ga[a[a[t>>2]+36>>2]](t,k,0):(Ve(14,113,105),A=0),(0|A)<=0){if(!(16&b))break e;if(114!=(4095&fr()))break e;bi(),e=1}else{s=Q=s-96|0,e=1;i:if(t){Q:{f:{if(f){if(f=Ji(t,0,f))break Q;if(!(32&b))break f}if(f=Ji(t,0,12959))break Q}bi();break i}if(c=Ni(t,f)){if(!((0|Vf(c))<=0))for(w=1&b,l=4&b,g=8&b;;){A=UQ(c,E),o=a[A+8>>2],A=(A=kQ(f=a[A+4>>2],46))?A-f|0:ci(f),e=0;Q:{f:{a:{if((0|Vf(a[22978]))>0)for(;;){if(r=UQ(a[22978],e),!si(a[r+4>>2],f,A))break a;if(e=e+1|0,!((0|Vf(a[22978]))>(0|e)))break}if(!g){(e=Ji(t,o,12254))||(bi(),e=f),a[22983]||(a[22983]=90940);B:if(A=Ki(40,31289)){if(r=kf(),a[A+4>>2]=r,r){if(r=a[22983],a[A+8>>2]=1,a[A>>2]=r,r=uf(),a[A+36>>2]=r,r){if(!(r=a[a[A>>2]+28>>2]))break B;if(0|Ga[0|r](A))break B;Fe(A),A=0;break B}Ve(37,113,65),Df(a[A+4>>2])}else Ve(37,113,65);SQ(A,31289,0),A=0}else Ve(37,113,65),A=0;B:if(A){a[A+12>>2]=0,r=110;t:if(!a[A+28>>2]&&(r=111,e)){b:{if(a[A+32>>2])r=110;else{if(r=Pr(e))break b;r=65}Ve(37,129,r),r=112;break t}if(SQ(a[A+28>>2],31289,0),a[A+28>>2]=r,r=108,n=a[a[A>>2]+4>>2]){if(0|Ga[0|n](A))break B;r=103}}Ve(37,112,r),Fe(A),A=0}else Ve(37,112,65),A=0;if(r=110,A&&(r=112,n=rQ(A,2931))){if(r=Ai(A,f,n,rQ(A,12259)))break a;r=0}Fe(A),Ve(14,117,r),a[Q+76>>2]=e,a[Q+72>>2]=24369,a[Q+68>>2]=f,a[Q+64>>2]=24401,br(4,Q- -64|0)}if(l)break f;Ve(14,118,113),a[Q+52>>2]=f,a[Q+48>>2]=24401,br(2,Q+48|0);break f}if(A=iQ(20,31289,0)){if(a[A>>2]=r,a[A+4>>2]=Pr(f),e=Pr(o),a[A+16>>2]=0,a[A+8>>2]=e,!(!a[A+4>>2]|!e)){a:{if(e=a[r+8>>2]){if(n=1,(0|Ga[0|e](A,t))<=0)break a}else n=0;if(((e=a[22979])||(e=kf(),a[22979]=e,e))&&ir(e,A)){a[r+16>>2]=a[r+16>>2]+1;break Q}Ve(14,115,65)}n&&(e=a[r+12>>2])&&Ga[0|e](A)}SQ(a[A+4>>2],31289,0),SQ(a[A+8>>2],31289,0),SQ(A,31289,0)}l||(Ve(14,118,109),a[Q+32>>2]=-1,ji(A=Q+83|0,13,16699,Q+32|0),a[Q+16>>2]=24409,a[Q+20>>2]=A,a[Q+12>>2]=o,a[Q+8>>2]=24377,a[Q+4>>2]=f,a[Q>>2]=24401,br(6,Q))}if(!w){e=-1;break i}}if(e=1,E=E+1|0,!((0|Vf(c))>(0|E)))break}}else e=0}s=Q+96|0}}if(C)break A}SQ(k,31289,0)}t&&Ga[a[a[t>>2]+12>>2]](t),i[91920]=1,b=2&b?1:e}a[23046]=b},function(){var A=0;A=0,RQ(91896)&&(A=1,RQ(91900)||(wa(91896),A=0)),A?(i[92128]=1,A=1):A=0,a[23049]=A},function(A){var r;return A|=0,(r=Ki(1052,31289))?(a[A+20>>2]=r,1):(Ve(52,108,65),0)},function(A){A=a[20+(A|=0)>>2],pQ(a[A+8>>2],a[A+12>>2]),pQ(a[A+16>>2],a[A+20>>2]),Of(A+24|0,a[A+1048>>2]),SQ(A,31289,0)},function(A){return A=a[20+(A|=0)>>2],pQ(a[A+16>>2],a[A+20>>2]),pQ(a[A+8>>2],a[A+12>>2]),Of(A+24|0,a[A+1048>>2]),IQ(A,0,1052),1},function(A,r,e){r|=0,e|=0;var i,Q,f=0,B=0,t=0;s=i=s-80|0,A=a[20+(A|=0)>>2];A:if(Q=a[A+4>>2])if(f=a[A+16>>2]){r:switch(a[A>>2]){case 0:if(e=a[e>>2],t=a[A+1048>>2],!qr(Q,a[A+8>>2],a[A+12>>2],f,a[A+20>>2],i,i+76|0))break A;A=Jr(Q,i,a[i+76>>2],A+24|0,t,r,e),Of(i,64),B=0!=(0|A);break A;case 1:if(!r){a[e>>2]=wf(Q),B=1;break A}(A=qr(Q,a[A+8>>2],a[A+12>>2],f,a[A+20>>2],r,i))&&(a[e>>2]=a[i>>2]),B=0!=(0|A);break A;case 2:break r;default:break A}B=0!=(0|Jr(Q,f,a[A+20>>2],A+24|0,a[A+1048>>2],r,a[e>>2]))}else Ve(52,102,104);else Ve(52,102,105);return s=i+80|0,0|B},function(A,r,e,i){var Q;r|=0,e|=0,i|=0,Q=a[20+(A|=0)>>2],A=-2;A:{r:{e:switch(r-4099|0){case 0:if(!i)return 0;a[Q+4>>2]=i;break r;case 4:a[Q>>2]=e;break r;case 1:if(A=1,!e|!i)break A;if(A=0,(0|e)<0)break A;if((A=a[Q+8>>2])&&pQ(A,a[Q+12>>2]),A=0,r=VQ(i,e),a[Q+8>>2]=r,!r)break A;a[Q+12>>2]=e;break r;case 2:if(A=0,(0|e)<0)break A;if((A=a[Q+16>>2])&&pQ(A,a[Q+20>>2]),A=0,r=VQ(i,e),a[Q+16>>2]=r,!r)break A;a[Q+20>>2]=e;break r;case 3:break e;default:break A}if(A=1,!e|!i)break A;if(A=0,(0|e)<0)break A;if((1024-(r=a[Q+1048>>2])|0)<(0|e))break A;yQ(24+(r+Q|0)|0,i,e),a[Q+1048>>2]=a[Q+1048>>2]+e}A=1}return 0|A},function(A,r,e){A|=0,e|=0;A:{if(!mi(r|=0,15500)){if(r=0,mi(e,22110)&&(r=1,mi(e,19171))){if(r=0,mi(e,19184))break A;r=2}return 0|Ie(A,-1,1024,4103,r,0)}if(!mi(r,16313))return 0|Me(A,1024,4099,e);if(!mi(r,2916))return 0|vQ(A,4100,e);if(!mi(r,2913))return 0|aQ(A,4100,e);if(!mi(r,1439))return 0|vQ(A,4101,e);if(!mi(r,1425))return 0|aQ(A,4101,e);if(!mi(r,9007))return 0|vQ(A,4102,e);if(!mi(r,9004))return 0|aQ(A,4102,e);Ve(52,103,103),r=-2}return 0|r},function(A){var r;return A|=0,(r=Ki(1040,31289))?(a[A+20>>2]=r,1):(Ve(52,110,65),0)},function(A){A=a[20+(A|=0)>>2],pQ(a[A+4>>2],a[A+8>>2]),Of(A+12|0,a[A+1036>>2]),SQ(A,31289,0)},function(A,r,e){r|=0,e|=0;var Q,f=0,t=0,b=0,k=0,n=0;if(A=a[20+(A|=0)>>2],!(t=a[A>>2]))return Ve(52,101,105),0;if(!(b=a[A+4>>2]))return Ve(52,101,107),0;if(!(k=a[A+1036>>2]))return Ve(52,101,106),0;f=A+12|0,Q=a[e>>2],A=a[A+8>>2];A:{if(114==a[t>>2]){if(!iA(37616,b,e=(e=1&A)+(A=A>>>1|0)|0,f,k,r,Q))return 0;if(!(t=iQ(Q,31289,0)))return Ve(52,111,65),0;if(!iA(37664,A+b|0,e,f,k,t,Q))break A;if(Q){if(e=0,A=0,Q-1>>>0>=3)for(k=-4&Q,b=0;i[0|(f=A+r|0)]=B[0|f]^B[A+t|0],i[0|(n=(f=1|A)+r|0)]=B[0|n]^B[t+f|0],i[0|(n=(f=2|A)+r|0)]=B[0|n]^B[t+f|0],i[0|(n=(f=3|A)+r|0)]=B[0|n]^B[t+f|0],A=A+4|0,(0|k)!=(0|(b=b+4|0)););if(b=3&Q)for(;i[0|(k=A+r|0)]=B[0|k]^B[A+t|0],A=A+1|0,(0|b)!=(0|(e=e+1|0)););}return pQ(t,Q),1}return 0!=(0|iA(t,b,A,f,k,r,Q))|0}return pQ(t,Q),0},function(A,r,e,i){var Q;r|=0,e|=0,i|=0,Q=a[20+(A|=0)>>2],A=-2;A:{r:{e:switch(r-4096|0){case 0:a[Q>>2]=i;break r;case 1:if(A=0,(0|e)<0)break A;if((A=a[Q+4>>2])&&pQ(A,a[Q+8>>2]),Of(Q+12|0,a[Q+1036>>2]),A=0,a[Q+1036>>2]=0,r=VQ(i,e),a[Q+4>>2]=r,!r)break A;a[Q+8>>2]=e;break r;case 2:break e;default:break A}if(A=1,!e|!i)break A;if(A=0,(0|e)<0)break A;if((1024-(r=a[Q+1036>>2])|0)<(0|e))break A;yQ(12+(r+Q|0)|0,i,e),a[Q+1036>>2]=a[Q+1036>>2]+e}A=1}return 0|A},function(A,r,e){return A|=0,r|=0,(e|=0)?mi(r,16313)?mi(r,3125)?mi(r,3122)?mi(r,16540)?mi(r,16537)?(Ve(52,100,103),-2):0|aQ(A,4098,e):0|vQ(A,4098,e):0|aQ(A,4097,e):0|vQ(A,4097,e):(A=a[A+20>>2],(r=TA(e))?(a[A>>2]=r,1):(Ve(52,100,100),0)):(Ve(52,100,102),0)},Bi,mi,iQ,K,SQ,IQ,function(A){A|=0;var r=0,e=0,i=0,Q=0,f=0;if(!(r=a[23067])||(0|(e=Vf(r)))<=(0|(r=a[A>>2]))){if(r=0,i=a[A+8>>2])if(e=B[0|i]){for(f=256;r=(r<<(Q=15&((e=(e=e<<24>>24)-91>>>0<=4294967269|e>>>0>127?e:32^e)>>>2^e))|((63&(Q=32-Q|0))>>>0>=32?0:r>>>Q|0))^n(e|=f,e),f=f+256|0,e=B[i+1|0],i=i+1|0,e;);r^=r>>>16}else r=0}else r=UQ(a[23067],r),r=0|Ga[a[r>>2]](a[A+8>>2]);return r^a[A>>2]},function(A,r){r|=0;var e=0,i=0,Q=0;if(!(e=a[(A|=0)>>2]-a[r>>2]|0)){if((e=a[23067])&&!((0|(i=Vf(e)))<=(0|(e=a[A>>2]))))return e=UQ(a[23067],e),0|Ga[a[e+4>>2]](a[A+8>>2],a[r+8>>2]);i=a[r+8>>2],r=0,Q=a[A+8>>2];A:if(A=B[0|Q])for(;;){if(!(e=B[0|i])){r=A;break A}if((0|A)!=(0|e)&&(0|zf(A))!=(0|zf(B[0|i]))){r=B[0|Q];break A}if(i=i+1|0,A=B[Q+1|0],Q=Q+1|0,!A)break}e=zf(255&r)-zf(B[0|i])|0}return 0|e},function(){var A=0;a[23068]=0,A=uf(),a[23066]=A,A?(A=Mi(108,109),a[23068]=A):A=a[23068],A||(Yf(a[23066]),a[23066]=0),a[23065]=0!=a[23068]&0!=a[23066]},function(A){var r=0,e=0;A:if(A|=0){if(e=a[A>>2],(0|(r=a[23069]))<0)r=e;else if((0|r)!=(0|e))break A;A=a[A+8>>2],s=e=s-16|0,!If(92256,110)|!a[23065]||(a[e>>2]=-32769&r,a[e+8>>2]=A,(A=GA(a[23068],e))&&((r=a[23067])&&((0|Vf(r))<=(0|(r=a[A>>2]))||(r=UQ(a[23067],r),Ga[a[r+8>>2]](a[A+8>>2],a[A>>2],a[A+12>>2]))),SQ(A,31289,0))),s=e+16|0}},fa,function(A){A=a[4+(A|=0)>>2],a[A+8>>2]=0,a[A+20>>2]=13|a[A+20>>2]},function(A){A=a[4+(A|=0)>>2],a[A+8>>2]=a[A+8>>2]+1},function(A){var r,e;r=a[4+(A|=0)>>2],e=a[r+8>>2]-1|0,a[r+8>>2]=e,e||oi(r),SQ(A,31289,0)},function(A){var r=0,e=0,i=0,Q=0,f=0,t=0;i=a[4+(A|=0)>>2];A:{r:{e:switch(a[A>>2]){case 0:if(e=(r=a[i+12>>2])<<20,(0|r)<=0)break r;if(i=a[i+16>>2],f=1&r,1!=(0|r))for(t=-2&r,r=0;e=B[r+i|0]<<(n(r,3)>>>0)%24^e^B[(e=1|r)+i|0]<<(n(e,3)>>>0)%24,r=r+2|0,(0|t)!=(0|(Q=Q+2|0)););else r=0;if(!f)break r;e=B[r+i|0]<<(n(r,3)>>>0)%24^e;break r;case 1:e=Bi(a[i>>2]);break r;case 2:e=Bi(a[i+4>>2]);break r;case 3:break e;default:break A}e=a[i+8>>2]}r=1073741823&e|a[A>>2]<<30}return 0|r},function(A,r){r|=0;var e=0,i=0;A:if(!(e=(i=a[(A|=0)>>2])-a[r>>2]|0)){r=a[r+4>>2],A=a[A+4>>2],e=0;r:switch(0|i){case 0:if(e=(i=a[A+12>>2])-a[r+12>>2]|0)break A;return 0|$i(a[A+16>>2],a[r+16>>2],i);case 1:return(A=a[A>>2])?(r=a[r>>2])?0|mi(A,r):1:-1;case 2:return(A=a[A+4>>2])?(r=a[r+4>>2])?0|mi(A,r):1:-1;case 3:break r;default:break A}e=a[A+8>>2]-a[r+8>>2]|0}return 0|e},fa,function(A,r){A|=0,r|=0;var e,i,Q,f=0;return s=e=s-16|0,i=e+4|0,(Q=e+12|0)&&(a[Q>>2]=a[a[r+4>>2]+8>>2],a[e+8>>2]=a[a[r+4>>2]>>2]),i&&(a[i>>2]=a[r>>2]),(r=Hf(0,e+12|0,a[e+8>>2]))?hr(r,a[e+4>>2])&&(f=1,ge(A,a[a[A+12>>2]>>2],r))||(be(r),f=0):Ve(4,139,4),s=e+16|0,0|f},function(A,r){A|=0,r|=0;var e,i=0,Q=0,f=0,B=0,t=0;s=e=s-16|0,a[e+12>>2]=0,i=a[r+24>>2],a[e+8>>2]=0,Q=5;A:{if(912==a[a[r+12>>2]>>2]&&(Q=-1,f=a[i+52>>2])){if(!se(f,89720,e+8|0))break A;i=a[r+24>>2],Q=16}(0|(f=xe(i,e+12|0,89584)))<=0||(t=1,i=Pe(a[a[r+12>>2]>>2]),r=a[e+12>>2],B=0,Ei(a[A>>2],i,Q,a[e+8>>2])&&(r&&(SQ(a[a[A+4>>2]+8>>2],31289,0),A=a[A+4>>2],a[A>>2]=f,a[A+8>>2]=r,a[A+12>>2]=-16&a[A+12>>2]|8),B=1),B||(t=0,SQ(a[e+12>>2],31289,0)))}return s=e+16|0,0|t},function(A,r){r|=0;var e=0;return e=1,1&Nf(a[24+(A|=0)>>2])||(e=1,1&Nf(a[r+24>>2])||(e=0,Ye(a[a[r+24>>2]+16>>2],a[a[A+24>>2]+16>>2])||(e=!Ye(a[a[r+24>>2]+20>>2],a[a[A+24>>2]+20>>2])))),0|e},function(A,r,e,i){return 0|CA(A|=0,r|=0,e|=0,0)},function(A,r){A|=0,r|=0;var e,i,Q,f=0;return s=e=s-16|0,i=e+4|0,(Q=e+12|0)&&(a[Q>>2]=a[a[r+8>>2]+8>>2],a[e+8>>2]=a[a[r+8>>2]>>2]),i&&(a[i>>2]=a[r+4>>2]),(r=Xf(0,e+12|0,a[e+8>>2]))?hr(r,a[e+4>>2])?(ge(A,a[a[A+12>>2]>>2],r),f=1):be(r):Ve(4,150,4),s=e+16|0,0|f},function(A,r){A|=0,r|=0;var e,i=0,Q=0,f=0,B=0,t=0,b=0;s=e=s-16|0,a[e+12>>2]=0,i=a[r+24>>2],a[e+8>>2]=0,Q=5;A:{if(912==a[a[r+12>>2]>>2]&&(Q=-1,f=a[i+52>>2])){if(!se(f,89720,e+8|0))break A;i=a[r+24>>2],Q=16}B=1,(0|(i=Pf(i,e+12|0)))>0&&(f=Pe(a[a[r+12>>2]>>2]),t=a[e+8>>2],r=a[e+12>>2],Ee(a[A>>2],0)&&Ei(a[A+4>>2],f,Q,t)&&(b=1,r&&(A=a[A+8>>2],SQ(a[A+8>>2],31289,0),a[A>>2]=i,a[A+8>>2]=r)),b)||(B=0,Ve(4,138,65),wQ(a[e+8>>2]))}return s=e+16|0,0|B},function(A,r,e,i){return 0|CA(A|=0,r|=0,e|=0,1)},function(A){return 0|hf(a[24+(A|=0)>>2])},function(A){return 0|Dr(a[a[24+(A|=0)>>2]+16>>2])},function(A){var r,e=0;A=a[24+(A|=0)>>2],r=Dr(a[A+16>>2]);A:{if(1==a[A+4>>2]){if((0|(A=Vf(a[A+48>>2])))<=0)break A;if((0|mQ(r))<(A+2|0))break A}e=256,((0|r)>15359||(e=192,(0|r)>7679||(e=128,(0|r)>3071||(e=112,(0|r)>2047||(e=80,A=0,!((0|r)<1024))))))&&(A=e),e=A}return 0|e},function(A,r,e,i,Q){A|=0,e|=0,i|=0,Q|=0;var f=0,t=0,b=0;A:{r:{e:{i:{if(912==(0|gr(a[(r|=0)>>2]))){if(Q=0,!(r=zi(89720,a[r+4>>2])))break e;if(Q=a[r+4>>2]){if(911!=(0|gr(a[Q>>2]))){a[r+16>>2]=0;break i}if(Q=zi(32744,a[Q+4>>2]),a[r+16>>2]=Q,!Q)break i}Q=r;break e}if(e)break r;return(0|Qr(A,31288))>0|0}Ea(r),Q=0}if(r=gA(A,0,Q,i),Ea(Q),!r)break A;if(!e)return 1}r=0,s=Q=s-16|0,t=(0|(f=a[e>>2]))>0?f:0,b=a[e+8>>2];r:{e:{for(;;){if((0|r)==(0|t))break e;if(!((r>>>0)%18|0)){if(e=0,(0|Ar(A,31288,1))<=0)break r;if((0|tQ(A,i,i))<=0)break r}if(a[Q>>2]=B[r+b|0],r=r+1|0,a[Q+4>>2]=(0|f)==(0|r)?31289:24622,!((0|Xe(A,6663,Q))>0))break}e=0;break r}e=1==(0|Ar(A,31288,1))}s=Q+16|0,f=e}return 0|f},function(A){be(a[24+(A|=0)>>2])},function(A,r,e,i){A|=0,r|=0,e|=0,i|=0;var Q,f=0;s=Q=s-16|0,a[Q+12>>2]=0,f=-2;A:{r:{e:{i:switch(r-1|0){case 0:if(e)break e;(A=Q+12|0)&&(a[A>>2]=a[i+16>>2]);break e;case 2:break r;case 1:break i;default:break A}if(912==a[a[A+12>>2]>>2])break A;e||(A=Q+12|0)&&(a[A>>2]=a[i+8>>2])}if(f=1,!(A=a[Q+12>>2]))break A;Ei(A,Pe(6),5,0);break A}if(A=a[a[A+24>>2]+52>>2]){if(!Qe(A,Q+8|0,Q+4|0,Q)){f=0,Ve(4,0,68);break A}a[i>>2]=a[a[Q+8>>2]>>2],f=2}else a[i>>2]=672,f=1}return s=Q+16|0,0|f},function(A,r,e){return A|=0,(r=Xf(0,r|=0,e|=0))?(ge(A,a[a[A+12>>2]>>2],r),1):(Ve(4,147,4),0)},function(A,r){return r|=0,0|Pf(a[24+(A|=0)>>2],r)},function(A,r,e,i,Q,f){A|=0,r|=0,e|=0,Q|=0,f|=0,s=Q=s-32|0;A:if(912==(0|gr(a[(i|=0)>>2])))if(e=0,a[Q+28>>2]=0,a[Q+20>>2]=0,a[Q+16>>2]=0,912==(0|gr(a[i>>2]))){r:if(r=zi(89720,a[i+4>>2])){e:{if(i=a[r+4>>2]){if(911!=(0|gr(a[i>>2]))){a[r+16>>2]=0;break e}if(i=zi(32744,a[i+4>>2]),a[r+16>>2]=i,!i)break e}e=r;break r}Ea(r)}r:{e:if(Qe(e,Q+16|0,Q+20|0,Q+24|0)){if(f){if(!rA(A,Q+28|0,a[Q+16>>2],f,1))break e;A=a[Q+28>>2]}else{if((0|Ie(0,-1,248,13,0,Q+12|0))<=0)break e;if(a[a[Q+16>>2]>>2]!=a[a[Q+12>>2]>>2]){Ve(4,155,158);break e}A=0}if(!((0|qi(A,-1,4097,6,0))<=0)&&!((0|qi(a[Q+28>>2],24,4098,a[Q+24>>2],0))<=0)&&(0|qi(a[Q+28>>2],1016,4101,0,a[Q+20>>2]))>0)break r}else Ve(4,155,149);Ea(e),A=-1;break A}Ea(e),A=2}else Ve(4,155,155),A=-1;else Ve(4,148,155),A=-1;return s=Q+32|0,0|A},function(A,r,e,i,Q,f){r|=0,e|=0,i|=0,Q|=0,f|=0;var B=0;s=r=s-16|0,e=0;A:if(!((0|qi(A=a[16+(A|=0)>>2],-1,4102,0,r))<=0)&&(e=2,6==a[r>>2]&&(f=a[A+8>>2],e=0,!((0|Ie(A,-1,248,13,0,r+12|0))<=0)&&!((0|qi(A,1016,4104,0,r+8|0))<=0)&&qi(A,24,4103,0,r+4|0)))){if(-1!=(0|(A=a[r+4>>2]))){if(!(A+3>>>0>1)&&(a[r+4>>2]=(sQ(f)-wf(a[r+12>>2])|0)-2,A=0,f&&(B=a[f+12>>2])&&(B=a[B+52>>2])&&(A=0|Ga[0|B](f)),f=A,A=a[r+4>>2],1==(7&f)&&(A=A-1|0,a[r+4>>2]=A),(0|A)<0))break A}else A=wf(a[r+12>>2]),a[r+4>>2]=A;if((f=Fr(a[r+12>>2],a[r+8>>2],A))&&(A=se(f,89720,0),Ea(f),A)){if(Q){if(!(f=je(A))){wQ(A);break A}Ei(Q,Pe(912),16,f)}Ei(i,Pe(912),16,A),e=3}}return s=r+16|0,0|e},function(A,r,e){A|=0,r|=0,e|=0;var i=0,Q=0,f=0;if(s=e=s-16|0,a[e+8>>2]=0,a[e+4>>2]=0,912==(0|gr(a[r>>2]))){i=0;A:if(r=zi(89720,a[r+4>>2])){r:{if(i=a[r+4>>2]){if(911!=(0|gr(a[i>>2]))){a[r+16>>2]=0;break r}if(i=zi(32744,a[i+4>>2]),a[r+16>>2]=i,!i)break r}i=r;break A}Ea(r),i=0}Qe(i,e+4|0,e+8|0,e+12|0)&&(r=a[e+4>>2],((Q=a[r>>2])-672>>>0>2|(0|Q)!=a[a[e+8>>2]>>2]||(f=2,a[e+12>>2]!=(0|wf(r))))&&(f=0),r=wf(r)<<2,a[A+12>>2]=f,a[A+8>>2]=r,a[A+4>>2]=912,a[A>>2]=Q,Q=1),Ea(i)}return s=e+16|0,0|Q},function(A){var r,e=0,i=0,Q=0,f=0,B=0,t=0,b=0,k=0,n=0;if(r=a[24+(A|=0)>>2],!a[r+24>>2]||!a[r+28>>2]|!a[r+32>>2]|!a[r+16>>2]|!a[r+20>>2])Ve(4,160,147),A=0;else if(1!=a[r+4>>2]||(0|(f=Vf(a[r+48>>2])))>0&&(0|mQ(Dr(a[r+16>>2])))>=(f+2|0)){e=KQ(1),B=KQ(1),t=KQ(1),b=KQ(1),k=KQ(1);A:if(!(Q=Pi())||!k|!e|!B|!t|!b)Ve(4,160,65),A=-1;else{if(Qf(a[r+20>>2])?(Ve(4,160,101),A=0):A=1,lf(a[r+20>>2])||(Ve(4,160,101),A=0),1!=(0|ba(a[r+28>>2]))&&(Ve(4,160,128),A=0),1!=(0|ba(a[r+32>>2]))&&(Ve(4,160,129),A=0),f)for(;1!=(0|ba(a[UQ(a[r+48>>2],i)>>2]))&&(Ve(4,160,170),A=0),(0|f)!=(0|(i=i+1|0)););if(df(e,a[r+28>>2],a[r+32>>2],Q)){r:if(f){for(i=0;;){if(df(e,e,a[UQ(a[r+48>>2],i)>>2],Q)){if((0|f)!=(0|(i=i+1|0)))continue;break r}break}A=-1;break A}if(Ye(e,a[r+16>>2])&&(Ve(4,160,f?172:127),A=0),OA(e,a[r+28>>2],32936))if(OA(B,a[r+32>>2],32936))if(df(b,e,B,Q))if(q(k,e,B,Q)){r:if(f){for(i=0;;){if(!OA(t,a[UQ(a[r+48>>2],i)>>2],32936)){A=-1;break A}if(!df(b,b,t,Q)){A=-1;break A}if(!q(k,k,t,Q))break;if((0|f)==(0|(i=i+1|0)))break r}A=-1;break A}if(H(t,0,b,k,Q))if(ki(e,a[r+24>>2],a[r+20>>2],t,Q)){if(Qf(e)||(Ve(4,160,123),A=0),!(!a[r+44>>2]|!a[r+36>>2]|!a[r+40>>2])){if(!OA(e,a[r+28>>2],32936)){A=-1;break A}if(!H(0,B,a[r+24>>2],e,Q)){A=-1;break A}if(Ye(B,a[r+36>>2])&&(Ve(4,160,124),A=0),!OA(e,a[r+32>>2],32936)){A=-1;break A}if(!H(0,B,a[r+24>>2],e,Q)){A=-1;break A}if(Ye(B,a[r+40>>2])&&(Ve(4,160,125),A=0),!hi(e,a[r+32>>2],a[r+28>>2],Q)){A=-1;break A}Ye(e,a[r+44>>2])&&(A=0,Ve(4,160,126))}if(f)for(;;){if(i=UQ(a[r+48>>2],n),!OA(e,a[i>>2],32936)){A=-1;break A}if(!H(0,B,a[r+24>>2],e,Q)){A=-1;break A}if(Ye(B,a[i+4>>2])&&(Ve(4,160,169),A=0),!hi(e,a[i+12>>2],a[i>>2],Q)){A=-1;break A}if(Ye(e,a[i+8>>2])&&(Ve(4,160,168),A=0),(0|(n=n+1|0))==(0|f))break}}else A=-1;else A=-1}else A=-1;else A=-1;else A=-1;else A=-1}else A=-1}xi(e),xi(B),xi(t),xi(b),xi(k),Gr(Q)}else Ve(4,160,167),A=0;return 0|A},function(A,r,e,i){r|=0,e|=0,i|=0;var Q=0,f=0,B=0,t=0,b=0;e=1;A:{r:switch(0|(A|=0)){case 0:return A=JA(),a[r>>2]=A,(0!=(0|A))<<1;case 2:return be(a[r>>2]),a[r>>2]=0,2;case 5:break r;default:break A}if(A=a[r>>2],1==a[A+4>>2]){i=0;r:{e:{if(!((0|(f=Vf(a[A+48>>2])))<=0)&&(Q=Pi())){for(B=A+28|0,e=A+32|0;;){if(t=a[e>>2],b=a[B>>2],B=(e=UQ(a[A+48>>2],i))+12|0,!(r=a[e+12>>2])&&(r=KQ(9),a[e+12>>2]=r,!r))break e;if(!df(r,b,t,Q))break e;if(r=1,(0|f)==(0|(i=i+1|0)))break}break r}Q=0}r=0}Gr(Q),e=(1==(0|r))<<1}}return 0|e},function(A,r,e,i){return r|=0,2==(0|(A|=0))&&la(a[a[r>>2]+16>>2]),1},function(A){yi(a[(A|=0)>>2]),yi(a[A+4>>2]),yi(a[A+8>>2]),yi(a[A+12>>2]),SQ(A,31289,0)},P,function(A,r,e,Q,f){A|=0,r|=0,e|=0,f|=0;var t=0,b=0,k=0,n=0,c=0,C=0,E=0,o=0,l=0;if((0|Dr(a[16+(Q|=0)>>2]))>=16385)return Ve(4,104,105),-1;if(!((0|Fi(a[Q+16>>2],a[Q+20>>2]))<=0)&&((0|Dr(a[Q+16>>2]))<3073||(0|Dr(a[Q+20>>2]))<65)){l=-1;A:if(c=Pi())if(ne(c),E=sA(c),o=sA(c),(b=iQ(n=(Dr(a[Q+16>>2])+7|0)/8|0,31289,0))&&o){r:{e:switch(f-1|0){case 3:A=nA(b,n,r,A,0,0,0,0);break r;case 1:i:if((0|(k=n-11|0))<(0|A))Ve(4,110,110),t=0;else if(i[0|b]=0,i[b+1|0]=2,t=0,!((0|fQ(f=b+2|0,k=k-A|0))<=0)){if((0|k)>0)for(;;){if(!B[0|f])for(;;){if(t=0,(0|fQ(f,1))<=0)break i;if(B[0|f])break}if(f=f+1|0,(0|k)==(0|(C=C+1|0)))break}i[f+8|0]=0,i[0|f]=3,i[f+1|0]=3,i[f+2|0]=3,i[f+3|0]=3,i[f+4|0]=3,i[f+5|0]=3,i[f+6|0]=3,i[f+7|0]=3,yQ(f+9|0,r,A),t=1}A=t;break r;case 2:A=ZQ(b,n,r,A);break r;default:Ve(4,104,118);break A;case 0:}e:if((n-11|0)<(0|A))Ve(4,109,110),t=0;else if(i[0|b]=0,i[b+1|0]=2,t=0,!((0|fQ(f=b+2|0,k=(n-A|0)-3|0))<=0)){if((0|k)>0)for(;;){if(!B[0|f])for(;;){if(t=0,(0|fQ(f,1))<=0)break e;if(B[0|f])break}if(f=f+1|0,(0|k)==(0|(C=C+1|0)))break}i[0|f]=0,yQ(f+1|0,r,A),t=1}A=t}(0|A)<=0||hA(b,n,E)&&((0|Fi(E,a[Q+16>>2]))>=0?Ve(4,104,132):2&B[Q+64|0]&&!le(Q+68|0,a[Q+16>>2],c)||0|Ga[a[a[Q+8>>2]+24>>2]](o,E,a[Q+20>>2],a[Q+16>>2],c,a[Q+68>>2])&&(l=cr(o,e,n)))}else Ve(4,104,65);return Yr(c),Gr(c),pQ(b,n),0|l}return Ve(4,104,101),-1},function(A,r,e,i,Q){A|=0,r|=0,e|=0,Q|=0;var f=0,t=0,b=0,k=0,n=0,c=0,C=0;if((0|Dr(a[16+(i|=0)>>2]))>=16385)return Ve(4,103,105),-1;if(!((0|Fi(a[i+16>>2],a[i+20>>2]))<=0)&&((0|Dr(a[i+16>>2]))<3073||(0|Dr(a[i+20>>2]))<65)){A:if(b=Pi()){ne(b),C=sA(b),n=-1,c=65;r:if(!(!(k=sA(b))|!(t=iQ(f=(Dr(a[i+16>>2])+7|0)/8|0,31289,0))||(c=108,(0|A)>(0|f)))){if(!hA(r,A,C))break A;if(c=132,!((0|Fi(C,a[i+16>>2]))>=0)){if(2&B[i+64|0]&&!le(i+68|0,a[i+16>>2],b))break A;if(!(0|Ga[a[a[i+8>>2]+24>>2]](k,C,a[i+20>>2],a[i+16>>2],b,a[i+68>>2])))break A;e:if(5!=(0|Q)){c=118,i=cr(k,t,f);i:switch(Q-1|0){case 0:A=t,r=-1;Q:{f:{if((0|f)>=11){if((0|i)==(0|f)){if(B[0|A]){Ve(4,112,138);break Q}i=f-1|0,A=A+1|0}if(!((i+1|0)==(0|f)&1==B[0|A])){Ve(4,112,106);break Q}Q=i-1|0;a:{B:{t:{if((0|i)>=2){for(r=0;;){if(255!=(0|(i=B[A+1|0]))){if(!i)break t;Ve(4,112,102),r=-1;break Q}if(A=A+1|0,(0|Q)==(0|(r=r+1|0)))break}break f}if(Q)break B;break f}if((0|r)==(0|Q))break f;if(r>>>0>7)break a}Ve(4,112,103),r=-1;break Q}if((0|f)<(0|(r=Q+(-1^r)|0))){Ve(4,112,109),r=-1;break Q}yQ(e,A+2|0,r)}break Q}Ve(4,112,113),r=-1}i=r;break e;case 2:break i;default:break r}yQ(e,t,i)}else{if(12!=(15&a[a[k>>2]>>2])&&!OA(k,a[i+16>>2],k))break A;Q=e;i:if((0|cr(k,t,f))==(0|f)&&106==(254&(A=B[0|t]))){r=t+1|0;Q:{if(107!=(0|A))A=r,i=f-2|0;else{if((0|f)<4)break Q;i=f-3|0,n=(f+t|0)-2|0,e=0;f:{for(;;){if(A=r+1|0,187!=(0|(r=B[0|r]))){if(186==(0|r))break f;break Q}if(r=A,(0|i)==(0|(e=e+1|0)))break}e=i,A=n}if(!e)break Q;i=i-e|0}if(204!=B[A+i|0]){Ve(4,128,139),i=-1;break i}yQ(Q,A,i);break i}Ve(4,128,138),i=-1}else Ve(4,128,137),i=-1}if((0|i)>=0){n=i;break A}c=114,n=i}}Ve(4,103,c)}else n=-1;return Yr(b),Gr(b),pQ(t,f),0|n}return Ve(4,103,101),-1},function(A,r,e,Q,f){A|=0,r|=0,e|=0,Q|=0,f|=0;var B,t,b=0,k=0,n=0,c=0,C=0,E=0,o=0,l=0;s=t=s-16|0,a[t+12>>2]=0,l=-1;A:if(B=Pi())if(ne(B),c=sA(B),k=sA(B),(E=iQ(C=(Dr(a[Q+16>>2])+7|0)/8|0,31289,0))&&k){r:{e:switch(f-1|0){case 4:b=E,(0|(n=C-A|0))<=1?(Ve(4,127,110),A=-1):(2!=(0|n)?(i[0|b]=107,b=b+1|0,n>>>0>=4&&(b=IQ(b,187,b=n-3|0)+b|0),i[0|b]=186):i[0|b]=106,i[yQ(b+1|0,r,A)+A|0]=204,A=1);break r;case 2:A=ZQ(E,C,r,A);break r;default:Ve(4,102,118);break A;case 0:}(C-11|0)<(0|A)?(Ve(4,108,110),A=0):(i[0|E]=0,i[E+1|0]=1,b=IQ(E+2|0,255,b=(C-A|0)-3|0)+b|0,i[0|b]=0,yQ(b+1|0,r,A),A=1)}if(!((0|A)<=0)&&hA(E,C,c))if((0|Fi(c,a[Q+16>>2]))>=0)Ve(4,102,132);else{if(2&(r=a[Q+64>>2])){if(!le(Q+68|0,a[Q+16>>2],B))break A;r=a[Q+64>>2]}if(A=0,!(b=128&r)){if(!(A=di(Q,t+12|0,B))){Ve(4,102,68);break A}if(!a[t+12>>2]&&!(o=sA(B))){Ve(4,102,65);break A}if(!ff(A,c,o,B))break A;r=a[Q+64>>2]}r:{if(!(!(32&r|1==a[Q+4>>2])&(!a[Q+44>>2]|!a[Q+28>>2]|!a[Q+32>>2]|!a[Q+36>>2]|!a[Q+40>>2]))){if(0|Ga[a[a[Q+8>>2]+20>>2]](k,c,Q,B))break r;break A}if(!(r=KQ(1))){Ve(4,102,65);break A}if(!(n=a[Q+24>>2])){Ve(4,102,179),xi(r);break A}if(nQ(r,n),n=0|Ga[a[a[Q+8>>2]+24>>2]](k,c,r,a[Q+16>>2],B,a[Q+68>>2]),xi(r),!n)break A}if(b||Mr(k,o,A,B)){if(5==(0|f)){if(!OA(c,a[Q+16>>2],k))break A;k=(0|Ye(k,c))>0?c:k}l=cr(k,e,C)}}}else Ve(4,102,65);return Yr(B),Gr(B),pQ(E,C),s=t+16|0,0|l},function(A,r,e,Q,f){A|=0,r|=0,e|=0,Q|=0,f|=0;var t,b,k=0,n=0,c=0,C=0,E=0,o=0,l=0,w=0,g=0,u=0,M=0,D=0,I=0,N=0,G=0,U=0,F=0,Z=0,d=0,W=0,V=0,Y=0,R=0,y=0;s=b=s-16|0,a[b+12>>2]=0,k=-1;A:if(t=Pi())if(ne(t),E=sA(t),o=sA(t),(M=iQ(c=(Dr(a[Q+16>>2])+7|0)/8|0,31289,0))&&o){if((0|A)>(0|c))Ve(4,101,108);else if(hA(r,A,E))if(A=0,(0|Fi(E,a[Q+16>>2]))>=0)Ve(4,101,132);else{if(r=0,!(l=128&(n=a[Q+64>>2]))){if(!(A=di(Q,b+12|0,t))){Ve(4,101,68);break A}if(!a[b+12>>2]&&!(r=sA(t))){Ve(4,101,65);break A}if(!ff(A,E,r,t))break A;n=a[Q+64>>2]}r:{if(!(!(32&n|1==a[Q+4>>2])&(!a[Q+44>>2]|!a[Q+28>>2]|!a[Q+32>>2]|!a[Q+36>>2]|!a[Q+40>>2]))){if(0|Ga[a[a[Q+8>>2]+20>>2]](o,E,Q,t))break r;break A}if(!(n=KQ(1))){Ve(4,101,65);break A}if(!(w=a[Q+24>>2])){Ve(4,101,179),xi(n);break A}if(nQ(n,w),2&B[Q+64|0]&&!le(Q+68|0,a[Q+16>>2],t)){xi(n);break A}if(Q=0|Ga[a[a[Q+8>>2]+24>>2]](o,E,n,a[Q+16>>2],t,a[Q+68>>2]),xi(n),!Q)break A}if(l||Mr(o,r,A,t)){k=cr(o,M,c);r:{e:switch(f-1|0){case 3:k=X(e,c,M,k,c,0,0,0,0);break r;case 1:if(s=r=s-16|0,A=-1,!((0|c)<=0|(0|k)<=0))if((0|k)<=(0|c)&(0|c)>10)if(Q=iQ(c,31289,0)){if(f=k+M|0,A=Q+c|0,1!=(0|c))for(E=-2&c;f=f+(n=(k-1&(-1^k))>>31^-1)|0,i[A-1|0]=B[0|f]&n,f=f+(n=((k=k+n|0)-1&(-1^k))>>31^-1)|0,i[0|(A=A-2|0)]=B[0|f]&n,k=k+n|0,(0|E)!=(0|(C=C+2|0)););if(1&c&&(n=A-1|0,A=(k-1&(-1^k))>>31^-1,i[0|n]=B[A+f|0]&A),k=2,C=((2^B[Q+1|0])-1&B[0|Q]-1)>>31,a[r+12>>2]=C,E=-1^C,a[r+12>>2]=E,I=107&a[r+12>>2],n=0,(0|c)<3)f=0;else for(f=0,A=0;w=(l=B[0|(o=Q+k|0)]-1>>31)&(-1^A),a[r+12>>2]=w,N=a[r+12>>2],a[r+12>>2]=-1^w,n=a[r+12>>2]&n|k&N,f=((A|=l)|(3^B[0|o])-1>>31)&(1&(-1^A))+f,(0|c)!=(0|(k=k+1|0)););if(n=-1^(k=((A=-1^n)&n-10)>>31),a[r+12>>2]=n|E,N=a[r+12>>2],a[r+12>>2]=k&C,G=a[r+12>>2],f=(k=(f-8&(-1^f))>>31)|-1^(C&=n),a[r+12>>2]=f,d=a[r+12>>2],a[r+12>>2]=-1^f,g=a[r+12>>2],u=-1^(A=(((f=A+c|0)^c-f|f^c)^c)>>31),l=k&C,a[r+12>>2]=u|-1^l,U=a[r+12>>2],a[r+12>>2]=A&l,F=a[r+12>>2],A=((n=c-11|0)^(n-c^c|c^n))>>31,a[r+12>>2]=A,k=a[r+12>>2],a[r+12>>2]=-1^A,w=a[r+12>>2]&c|k&n,(0|c)>=13)for(R=n-f|0,A=1;;){if(!((c-A|0)<12)){if(W=1&(C=n-A|0),o=-1^(E=((A&R)-1|0)<0?0:255),k=11,1!=(0|C))for(y=-2&C,C=0;Z=B[0|(D=Q+k|0)],V=B[(A+k|0)+Q|0],a[r+12>>2]=E,Y=a[r+12>>2],a[r+12>>2]=o,i[0|D]=Z&a[r+12>>2]|V&Y,V=B[0|(Z=(D=k+1|0)+Q|0)],D=B[(A+D|0)+Q|0],a[r+12>>2]=E,Y=a[r+12>>2],a[r+12>>2]=o,i[0|Z]=V&a[r+12>>2]|D&Y,k=k+2|0,(0|y)!=(0|(C=C+2|0)););W&&(W=B[0|(C=Q+k|0)],k=B[(A+k|0)+Q|0],a[r+12>>2]=E,E=a[r+12>>2],a[r+12>>2]=o,i[0|C]=W&a[r+12>>2]|k&E)}if(!((0|n)>(0|(A<<=1))))break}if(A=l&u,(0|w)>0)for(C=255&A,k=0;E=B[0|(n=e+k|0)],o=B[11+(Q+k|0)|0],l=C&(k-f|f)>>31,a[r+12>>2]=l,u=a[r+12>>2],a[r+12>>2]=-1^l,i[0|n]=E&a[r+12>>2]|o&u,(0|w)!=(0|(k=k+1|0)););pQ(Q,c),Ve(4,114,U&(115&g|d&(113&G|I&N))|109&F),_i(1&A),a[r+12>>2]=A,e=a[r+12>>2],a[r+12>>2]=-1^A,A=a[r+12>>2]|e&f}else Ve(4,114,65);else Ve(4,114,111);s=r+16|0,k=A;break r;case 2:yQ(e,M,k);break r;default:Ve(4,101,118),k=-1;break A;case 0:}if(s=r=s-16|0,A=-1,!((0|c)<=0|(0|k)<=0))if((0|k)<=(0|c)&(0|c)>10)if(Q=iQ(c,31289,0)){if(f=k+M|0,A=Q+c|0,1!=(0|c))for(E=-2&c;f=f+(n=(k-1&(-1^k))>>31^-1)|0,i[A-1|0]=B[0|f]&n,f=f+(n=((k=k+n|0)-1&(-1^k))>>31^-1)|0,i[0|(A=A-2|0)]=B[0|f]&n,k=k+n|0,(0|E)!=(0|(C=C+2|0)););if(1&c&&(n=A-1|0,A=(k-1&(-1^k))>>31^-1,i[0|n]=B[A+f|0]&A),k=2,C=(2^B[Q+1|0])-1&B[0|Q]-1,f=0,(0|c)>=3)for(A=0;E=(n=B[Q+k|0]-1>>31)&(-1^A),a[r+12>>2]=E,o=a[r+12>>2],a[r+12>>2]=-1^E,f=a[r+12>>2]&f|k&o,A|=n,(0|c)!=(0|(k=k+1|0)););if(A=(((n=c-11|0)-c^c|c^n)^n)>>31,a[r+12>>2]=A,k=a[r+12>>2],a[r+12>>2]=-1^A,w=(((A=-1^f)&f-10)>>31^-1)&C>>31,I=((c-(f=A+c|0)^f|f^c)^c)<0,l=a[r+12>>2]&c|k&n,(0|c)>=13)for(N=n-f|0,A=1;;){if(!((c-A|0)<12)){if(G=1&(C=n-A|0),o=-1^(E=((A&N)-1|0)<0?0:255),k=11,1!=(0|C))for(d=-2&C,C=0;u=B[0|(g=Q+k|0)],U=B[Q+(A+k|0)|0],a[r+12>>2]=E,F=a[r+12>>2],a[r+12>>2]=o,i[0|g]=u&a[r+12>>2]|U&F,U=B[0|(u=(g=k+1|0)+Q|0)],g=B[Q+(A+g|0)|0],a[r+12>>2]=E,F=a[r+12>>2],a[r+12>>2]=o,i[0|u]=U&a[r+12>>2]|g&F,k=k+2|0,(0|d)!=(0|(C=C+2|0)););G&&(G=B[0|(C=Q+k|0)],k=B[Q+(A+k|0)|0],a[r+12>>2]=E,E=a[r+12>>2],a[r+12>>2]=o,i[0|C]=G&a[r+12>>2]|k&E)}if(!((0|n)>(0|(A<<=1))))break}if(A=I?0:w,(0|l)>0)for(C=255&A,k=0;E=B[0|(n=e+k|0)],o=B[11+(Q+k|0)|0],w=C&(f|k-f)>>31,a[r+12>>2]=w,I=a[r+12>>2],a[r+12>>2]=-1^w,i[0|n]=E&a[r+12>>2]|o&I,(0|l)!=(0|(k=k+1|0)););pQ(Q,c),Ve(4,113,159),_i(1&A),a[r+12>>2]=A,e=a[r+12>>2],a[r+12>>2]=-1^A,A=a[r+12>>2]|e&f}else Ve(4,113,65);else Ve(4,113,159);s=r+16|0,k=A}Ve(4,101,114),_i((-1^k)>>>31|0)}}}else Ve(4,101,65);return Yr(t),Gr(t),pQ(M,c),s=b+16|0,0|k},function(A,r,e,i){A|=0,r|=0,e|=0;var Q,f,B=0,t=0,b=0,k=0,n=0,c=0,C=0,E=0,o=0,l=0,w=0,g=0,u=0,M=0,D=0,I=0,N=0,G=0;s=f=s-16|0,ne(i|=0),k=sA(i),n=sA(i),c=sA(i);A:if((Q=sA(i))&&!(1==a[e+4>>2]&&(t=Vf(a[e+48>>2]))-4>>>0<4294967293)){if(4&(b=a[e+64>>2])){if(!(B=KQ(1)))break A;nQ(B,a[e+28>>2]);r:{if(!le(e+72|0,B,i)||(nQ(B,a[e+32>>2]),!le(e+76|0,B,i))){xi(B);break A}if(t){e:{for(;;){if(b=UQ(a[e+48>>2],w),nQ(B,a[b>>2]),le(b+16|0,B,i)){if((0|(w=w+1|0))!=(0|t))continue;break e}break}xi(B),w=0;break A}xi(B),B=0}else{if(xi(B),B=0,141!=a[a[e+8>>2]+24>>2])break r;B=(0|Dr(a[e+32>>2]))==(0|Dr(a[e+28>>2]))}}b=a[e+64>>2]}if(2&b&&!le(e+68|0,a[e+16>>2],i))w=0;else{r:{if(B){if(w=0,!hQ(c,r,a[e+76>>2],i))break A;if(!yf(c,c,a[e+76>>2],i))break A;if(!R(c,c,a[e+40>>2],a[e+32>>2],i,a[e+76>>2]))break A;if(!hQ(k,r,a[e+72>>2],i))break A;if(!yf(k,k,a[e+72>>2],i))break A;if(!R(k,k,a[e+36>>2],a[e+28>>2],i,a[e+72>>2]))break A;if(t=0,o=a[e+28>>2],B=0,Gf(k,b=a[o+4>>2])){if(b){for(B=a[c>>2],C=a[k>>2],D=B||C,g=(B=a[k>>2])||C,B=0;u=E+(n=a[g+(B<<2)>>2]&t-a[k+4>>2]>>31)|0,E=a[D+(l<<2)>>2]&t-a[c+4>>2]>>31,a[C+(t<<2)>>2]=u-E,E=0-(M=(0|E)==(0|n)?M:E>>>0>n>>>0)|0,l=((t=t+1|0)-a[c+8>>2]>>>31|0)+l|0,B=(t-a[k+8>>2]>>>31|0)+B|0,(0|t)!=(0|b););if(b){if(l=a[o>>2],D=1&b,g=b-1|0)for(I=-2&b,B=0,t=0,n=0;N=u=(o=B<<2)+C|0,u=(t=(G=a[l+o>>2]&E)+t|0)+a[u>>2]|0,a[N>>2]=u,N=(o|=4)+C|0,u=(t=(o=a[l+o>>2]&E)+((t>>>0<G>>>0)+(t>>>0>u>>>0)|0)|0)+a[N>>2]|0,a[N>>2]=u,t=(t>>>0<o>>>0)+(t>>>0>u>>>0)|0,B=B+2|0,(0|I)!=(0|(n=n+2|0)););else B=0,t=0;if(D&&(n=(B<<=2)+C|0,t=(B=t+(E=a[B+l>>2]&E)|0)+a[n>>2]|0,a[n>>2]=t,t=(B>>>0<E>>>0)+(B>>>0>t>>>0)|0),b){if(E=t-M|0,o=1&b,g)for(D=-2&b,B=0,t=0,n=0;u=g=(M=B<<2)+C|0,g=(t=(I=E&a[l+M>>2])+t|0)+a[g>>2]|0,a[u>>2]=g,u=(M|=4)+C|0,g=(t=(M=E&a[l+M>>2])+((t>>>0<I>>>0)+(t>>>0>g>>>0)|0)|0)+a[u>>2]|0,a[u>>2]=g,t=(t>>>0<M>>>0)+(t>>>0>g>>>0)|0,B=B+2|0,(0|D)!=(0|(n=n+2|0)););else B=0,t=0;o&&(a[(C=(B<<=2)+C|0)>>2]=a[C>>2]+((E&a[B+l>>2])+t|0))}}}a[k+12>>2]=0,a[k+4>>2]=b,B=1}if(!B)break A;if(!yf(k,k,a[e+72>>2],i))break A;if(!Gi(k,k,a[e+44>>2],a[e+72>>2],i))break A;if(!xA(A,k,a[e+32>>2],i))break A;if(t=0,l=0,C=0,E=0,s=k=s-128|0,n=a[e+16>>2],Gf(A,b=a[n+4>>2])&&(B=k,!(b>>>0>=33)||(B=iQ(b<<2,31289,0)))){if(b)for(M=(t=a[c>>2])||B,o=(t=a[A>>2])||B,t=0;I=(D=a[M+(E<<2)>>2]&t-a[c+4>>2]>>31)+(C=C+(g=a[o+(l<<2)>>2]&t-a[A+4>>2]>>31)|0)|0,a[(t<<2)+B>>2]=I,C=(C>>>0<g>>>0)+(D>>>0>I>>>0)|0,E=((t=t+1|0)-a[c+8>>2]>>>31|0)+E|0,l=(t-a[A+8>>2]>>>31|0)+l|0,(0|t)!=(0|b););if(t=rr(c=a[A>>2],B,a[n>>2],b),b){if(E=-1^(l=C-t|0),t=0,1!=(0|b))for(M=-2&b,C=0;g=o=(n=t<<2)+c|0,u=E&a[o>>2],o=B+n|0,a[g>>2]=u|l&a[o>>2],a[o>>2]=0,o=(n|=4)+c|0,n=B+n|0,a[o>>2]=E&a[o>>2]|l&a[n>>2],a[n>>2]=0,t=t+2|0,(0|M)!=(0|(C=C+2|0)););1&b&&(c=(t<<=2)+c|0,t=B+t|0,a[c>>2]=E&a[c>>2]|l&a[t>>2],a[t>>2]=0)}a[A+12>>2]=0,a[A+4>>2]=b,t=1,(0|B)!=(0|k)&&SQ(B,31289,0)}if(s=k+128|0,t)break r;break A}if(!(B=KQ(1))){w=0;break A}if(nQ(B,r),w=0,!H(0,k,B,a[e+32>>2],i)){xi(B);break A}if(!(b=KQ(1))){xi(B);break A}if(nQ(b,a[e+40>>2]),!(0|Ga[a[a[e+8>>2]+24>>2]](c,k,b,a[e+32>>2],i,a[e+76>>2]))){xi(B),xi(b);break A}if(xi(b),b=H(0,k,B,a[e+28>>2],i),xi(B),!b)break A;if(!(B=KQ(1)))break A;if(nQ(B,a[e+36>>2]),b=0|Ga[a[a[e+8>>2]+24>>2]](A,k,B,a[e+28>>2],i,a[e+72>>2]),xi(B),!b)break A;if((0|t)>0){if(B=KQ(1),!(b=KQ(1))|!B){xi(b),xi(B);break A}e:{for(;;){if(E=sA(i),a[(f+4|0)+(l<<2)>>2]=E,E&&(C=UQ(a[e+48>>2],l),nQ(b,r),nQ(B,a[C+4>>2]),H(0,k,b,a[C>>2],i)&&0|Ga[a[a[e+8>>2]+24>>2]](E,k,B,a[C>>2],i,a[C+16>>2]))){if((0|(l=l+1|0))!=(0|t))continue;break e}break}xi(b),xi(B);break A}xi(b),xi(B)}if(!OA(A,A,c))break A;if(ra(A)&&!PA(A,A,a[e+28>>2]))break A;if(!df(k,A,a[e+44>>2],i))break A;if(!(B=KQ(1)))break A;if(nQ(B,k),b=H(0,A,B,a[e+28>>2],i),xi(B),!b)break A;if(ra(A)&&!PA(A,A,a[e+28>>2]))break A;if(!df(k,A,a[e+32>>2],i))break A;if(!PA(A,k,c))break A;if(!((0|t)<=0)){if(!(c=KQ(1)))break A;B=0;e:{for(;;){if(w=UQ(a[e+48>>2],B),OA(k,a[(f+4|0)+(B<<2)>>2],A)&&df(n,k,a[w+8>>2],i)&&(nQ(c,n),H(0,k,c,a[w>>2],i)&&(!ra(k)||PA(k,k,a[w>>2]))&&df(k,k,a[w+12>>2],i)&&PA(A,A,k))){if((0|t)!=(0|(B=B+1|0)))continue;break e}break}xi(c),w=0;break A}xi(c)}}if((B=a[e+20>>2])&&(k=a[e+16>>2])){r:{if(141==a[a[e+8>>2]+24>>2]){if(P(Q,A,B,k,i,a[e+68>>2]))break r;w=0;break A}if(Hi(A),!(0|Ga[a[a[e+8>>2]+24>>2]](Q,A,a[e+20>>2],a[e+16>>2],i,a[e+68>>2]))){w=0;break A}}if(!OA(Q,Q,r)){w=0;break A}if(!ka(Q)){if(w=0,!H(0,Q,Q,a[e+16>>2],i))break A;if(ra(Q)&&!PA(Q,Q,a[e+16>>2]))break A;if(!ka(Q)){if(!(B=KQ(1)))break A;if(nQ(B,a[e+24>>2]),r=0|Ga[a[a[e+8>>2]+24>>2]](A,r,B,a[e+16>>2],i,a[e+68>>2]),xi(B),!r)break A}}}Hi(A),w=1}}return Yr(i),s=f+16|0,0|w},function(A){return a[64+(A|=0)>>2]=6|a[A+64>>2],1},function(A){var r=0;if(TQ(a[68+(A|=0)>>2]),TQ(a[A+72>>2]),TQ(a[A+76>>2]),(0|Vf(a[A+48>>2]))>0)for(;TQ(a[UQ(a[A+48>>2],r)+16>>2]),r=r+1|0,(0|Vf(a[A+48>>2]))>(0|r););return 1},function(A){A|=0;var r,e=0;return(r=Ki(52,31289))?(a[r+8>>2]=2,a[r>>2]=2048,e=a[a[A>>2]>>2],a[r+32>>2]=-2,a[r+36>>2]=-1,a[r+20>>2]=912==(0|e)?6:1,a[A+36>>2]=2,a[A+32>>2]=r+12,a[A+20>>2]=r,A=1):A=0,0|A},function(A,r){A|=0,r|=0;var e,i=0,Q=0;A:if((e=Ki(52,31289))&&(a[e+8>>2]=2,a[e>>2]=2048,i=a[a[A>>2]>>2],a[e+32>>2]=-2,a[e+36>>2]=-1,a[e+20>>2]=912==(0|i)?6:1,a[A+36>>2]=2,a[A+32>>2]=e+12,a[A+20>>2]=e,A=a[r+20>>2],a[e>>2]=a[A>>2],!(r=a[A+4>>2])||(r=Sr(r),a[e+4>>2]=r,r))){if(a[e+20>>2]=a[A+20>>2],a[e+24>>2]=a[A+24>>2],a[e+28>>2]=a[A+28>>2],a[A+44>>2]){if(SQ(a[e+44>>2],31289,0),r=VQ(a[A+44>>2],a[A+48>>2]),a[e+44>>2]=r,!r)break A;a[e+48>>2]=a[A+48>>2]}Q=1}return 0|Q},function(A){(A=a[20+(A|=0)>>2])&&(xi(a[A+4>>2]),SQ(a[A+40>>2],31289,0),SQ(a[A+44>>2],31289,0),SQ(A,31289,0))},function(A,r){r|=0;var e=0,i=0,Q=0,f=0,B=0,t=0;A:{Q=a[20+(A|=0)>>2];r:{if(!a[Q+4>>2]){if(i=KQ(1),a[Q+4>>2]=i,!i)break r;if(!MQ(i,65537))break r}if(!(i=JA()))return 0;if(a[A+28>>2]){if((e=iQ(12,31289,0))||Ve(3,143,65),!e)break A;a[e+8>>2]=56,a[e+4>>2]=A,a[e>>2]=2}if(f=V(i,a[Q>>2],a[Q+8>>2],a[Q+4>>2],e),e&&SQ(e,31289,0),(0|f)>0){if(912==(0|(e=a[a[A>>2]>>2]))&&(e=a[A+20>>2],Q=a[e+32>>2],(B=a[e+28>>2])|(t=a[e+24>>2])||(B=0,e=912,-2!=(0|Q)))){if(e=Fr(t,B,-2==(0|Q)?0:Q),a[i+52>>2]=e,!e)break A;e=a[a[A>>2]>>2]}return ge(r,e,i),0|f}be(i)}return 0|f}return be(i),0},function(A,r,e,Q,f){r|=0,e|=0,Q|=0,f|=0;var t,b,k,n=0,c=0,C=0,E=0,o=0,l=0,w=0,g=0,u=0,M=0;s=k=s-16|0,b=a[a[8+(A|=0)>>2]+24>>2],t=a[A+20>>2];A:{r:if(c=a[t+24>>2]){if((0|wf(c))!=(0|f)){Ve(4,142,143),c=-1;break A}if(95==a[a[t+24>>2]>>2]){if(c=-1,1!=a[t+20>>2])break A;if(s=A=s-32|0,a[A+20>>2]=4,a[A+24>>2]=Q,a[A+16>>2]=f,f=0,(0|(c=qf(A+16|0,0)))>((Q=hf(b))-11|0)?Ve(4,118,112):(Q=iQ(C=Q+1|0,31289,0))?(a[A+12>>2]=Q,qf(A+16|0,A+12|0),(0|(r=cf(c,Q,r,b,1)))>0&&(a[k+12>>2]=r,f=1),pQ(Q,C)):Ve(4,118,65),s=A+32|0,c=f,(0|f)<=0)break A;c=a[k+12>>2];break r}c=-1;e:switch(a[t+20>>2]-1|0){case 4:if(n=f+1|0,sQ(a[A+8>>2])>>>0<n>>>0){Ve(4,142,120);break A}if(!(C=a[t+40>>2])&&(C=iQ(sQ(a[A+8>>2]),31289,0),a[t+40>>2]=C,!C)){Ve(4,167,65),Ve(4,142,65);break A}yQ(C,Q,f),A=DQ(a[a[t+24>>2]>>2]),i[a[t+40>>2]+f|0]=A,c=cf(n,a[t+40>>2],r,b,5);break r;case 0:if((0|(c=fe(a[a[t+24>>2]>>2],Q,f,r,k+8|0,b)))<=0)break A;c=a[k+8>>2];break r;case 5:break e;default:break A}if(!(f=a[t+40>>2])&&(f=iQ(sQ(a[A+8>>2]),31289,0),a[t+40>>2]=f,!f)){Ve(4,167,65);break A}C=f,n=Q,w=a[t+28>>2],Q=a[t+32>>2];e:{i:if(!((0|(f=wf(g=a[t+24>>2])))<0)){A=f;Q:{f:switch(Q+2|0){default:if((0|Q)>-4){A=Q;break Q}Ve(4,152,136);break i;case 1:break Q;case 0:}A=-3}if(Q=Dr(a[b+16>>2]),l=hf(b),(u=Q-1&7)||(i[0|C]=0,l=l-1|0,C=C+1|0),(f+2|0)>(0|l))Ve(4,152,110),Q=A;else{Q=(l-f|0)-2|0;Q:{if(-3!=(0|A)){if((0|A)>(0|Q))break Q;Q=A}if(!((0|Q)<=0)){if(!(E=iQ(Q,31289,0))){Ve(4,152,65);break i}if(!((0|fQ(E,Q))>0)){A=0;break e}}if(!(A=ta())){A=0;break e}if(!Vr(A,g))break e;if(!nf(A,90012,8))break e;if(!nf(A,n,f))break e;if(Q&&!nf(A,E,Q))break e;if(!Ri(A,M=(n=(-1^f)+l|0)+C|0,0))break e;if(yr(C,n,M,f,w||g))break e;if(i[0|(n=((l-(Q+f|0)|0)+C|0)-2|0)]=1^B[0|n],!((0|Q)<=0)){if(w=0,Q-1>>>0>=3)for(g=-4&Q,f=0;i[n+1|0]=B[n+1|0]^B[o+E|0],i[n+2|0]=B[n+2|0]^B[(1|o)+E|0],i[n+3|0]=B[n+3|0]^B[(2|o)+E|0],i[n+4|0]=B[n+4|0]^B[(3|o)+E|0],o=o+4|0,n=n+4|0,(0|g)!=(0|(f=f+4|0)););if(f=3&Q)for(;i[n+1|0]=B[n+1|0]^B[o+E|0],o=o+1|0,n=n+1|0,(0|f)!=(0|(w=w+1|0)););}u&&(i[0|C]=B[0|C]&255>>>8-u),i[(C+l|0)-1|0]=188,o=1;break e}Ve(4,152,110),Q=A}}E=0,A=0}if(Qa(A),pQ(E,Q),!o)break A;c=cf(hf(b),a[t+40>>2],r,b,3)}else c=cf(f,Q,r,b,a[t+20>>2]);(0|c)<0||(a[e>>2]=c,c=1)}return s=k+16|0,0|c},function(A,r,e,Q,f){r|=0,e|=0,Q|=0,f|=0;var t,b=0,k=0,n=0,c=0,C=0,E=0,o=0,l=0,w=0,g=0,u=0,M=0;s=t=s-16|0,b=a[8+(A|=0)>>2],n=a[b+24>>2];A:{r:{k=a[A+20>>2];e:{if(C=a[k+24>>2]){if(1==a[k+20>>2]){b=YQ(a[C>>2],Q,f,r,e,n);break A}if((0|wf(C))!=(0|f)){Ve(4,149,143),b=-1;break A}b=-1;i:switch(a[k+20>>2]-5|0){case 0:if(b=0,(0|kr(A,0,t+12|0,r,e))<=0)break A;A=a[t+12>>2];break r;case 1:break i;default:break A}if(!(f=a[k+40>>2])&&(f=iQ(sQ(a[A+8>>2]),31289,0),a[k+40>>2]=f,!f)){Ve(4,167,65);break A}if((0|of(e,r,f,n,3))<=0)break e;M=a[k+24>>2],g=a[k+28>>2],o=a[k+40>>2],e=a[k+32>>2],r=0,s=k=s+-64|0;i:if((l=ta())&&!((0|(f=wf(M)))<0))if(A=f,-1==(0|e)||(A=e,(0|e)>-4))if(e=Dr(a[n+16>>2]),C=hf(n),u=e-1&7,B[0|o]>>>u|0)Ve(4,126,133);else if((0|(n=C-(b=!u)|0))<(f+2|0))Ve(4,126,109);else{if(e=(n-f|0)-2|0,-3!=(0|A)){if(!((0|A)<=(0|e))){Ve(4,126,109);break i}}else A=e;if(188==B[((w=b+o|0)+n|0)-1|0])if(c=iQ(E=n+(-1^f)|0,31289,0)){if(!((0|yr(c,E,n=E+w|0,f,g||M))<0)){if(!((0|E)<=0)){if(g=0,((C+(u?0:-1)|0)-f|0)-2>>>0>=3)for(C=-4&E,o=0;i[0|(e=r+c|0)]=B[0|e]^B[r+w|0],i[0|(e=(b=1|r)+c|0)]=B[0|e]^B[b+w|0],i[0|(e=(b=2|r)+c|0)]=B[0|e]^B[b+w|0],i[0|(e=(b=3|r)+c|0)]=B[0|e]^B[b+w|0],r=r+4|0,(0|C)!=(0|(o=o+4|0)););if(b=3&E)for(;i[0|(e=r+c|0)]=B[0|e]^B[r+w|0],r=r+1|0,(0|b)!=(0|(g=g+1|0)););}for(u&&(i[0|c]=B[0|c]&255>>>8-u),o=E-1|0,r=0;e=r+1|0,!(C=B[r+c|0])&&(b=(0|r)<(0|o),r=e,b););1==(0|C)?(b=-2==(0|A))|(0|(r=A))==(0|(A=E-e|0))?(r=0,Vr(l,M)&&nf(l,90012,8)&&nf(l,Q,f)&&((0|e)==(0|E)||nf(l,e+c|0,A))&&Ri(l,k,0)&&($i(k,n,f)?Ve(4,126,104):r=1)):(r=0,Ve(4,126,136)):(r=0,Ve(4,126,135))}}else Ve(4,126,65),c=0;else Ve(4,126,134)}else Ve(4,126,136);SQ(c,31289,0),Qa(l),s=k- -64|0,b=(0|r)>0;break A}if(!(A=a[k+40>>2])&&(A=iQ(sQ(b),31289,0),a[k+40>>2]=A,!A)){Ve(4,167,65),b=-1;break A}if(A=of(e,r,A,n,a[k+20>>2]))break r}b=0;break A}b=0,(0|A)==(0|f)&&(b=!$i(Q,a[k+40>>2],f))}return s=t+16|0,0|b},kr,function(A,r,e,i,Q){r|=0,e|=0,i|=0,Q|=0;var f,B=0,t=0;if(B=a[a[8+(A|=0)>>2]+24>>2],f=a[A+20>>2],4!=(0|(t=a[f+20>>2])))A=Ef(Q,i,r,B,t);else{if(t=hf(B),!(B=a[f+40>>2])&&(B=iQ(sQ(a[A+8>>2]),31289,0),a[f+40>>2]=B,!B))return Ve(4,167,65),-1;if(!nA(B,t,i,Q,a[f+44>>2],a[f+48>>2],a[f+24>>2],a[f+28>>2]))return-1;A=Ef(t,a[f+40>>2],r,a[a[A+8>>2]+24>>2],3)}return(0|A)>=0&&(a[e>>2]=A,A=1),0|A},function(A,r,e,i,Q){r|=0,e|=0,i|=0,Q|=0;var f,B=0,t=0;s=f=s-16|0,B=a[20+(A|=0)>>2];A:{if(4!=(0|(t=a[B+20>>2])))A=Cf(Q,i,r,a[a[A+8>>2]+24>>2],t);else{if(!(t=a[B+40>>2])&&(t=iQ(sQ(a[A+8>>2]),31289,0),a[B+40>>2]=t,!t)){Ve(4,167,65),A=-1;break A}if((0|(A=Cf(Q,i,t,a[a[A+8>>2]+24>>2],3)))<=0)break A;A=X(r,A,a[B+40>>2],A,A,a[B+44>>2],a[B+48>>2],a[B+24>>2],a[B+28>>2])}i=a[e>>2],r=A>>31,a[f+12>>2]=r,Q=a[f+12>>2],B=-1^r,a[f+12>>2]=B,a[e>>2]=A&a[f+12>>2]|i&Q,a[f+12>>2]=r,r=a[f+12>>2],a[f+12>>2]=B,A=1&a[f+12>>2]|A&r}return s=f+16|0,0|A},function(A,r,e,i){r|=0,e|=0,i|=0;var Q,f=0;Q=a[20+(A|=0)>>2],f=1;A:{r:{e:{i:{Q:{f:{a:{B:{t:{b:{k:{n:switch(r-4097|0){default:switch(r-1|0){case 0:break k;case 12:break r;case 4:case 6:break i;case 1:break f;case 2:case 3:break a;default:break Q}case 0:c:if(!(e-1>>>0>5)){if(!de(a[Q+24>>2],e))return 0;C:{E:{if(6==(0|e)){if(!(24&B[A+16|0]))break c;if(!a[Q+24>>2])break E;break C}if(912==a[a[A>>2]>>2])break c;if(4!=(0|e))break C;if(!(3&B[A+17|0]))break c;if(a[Q+24>>2])break C}a[Q+24>>2]=37664}a[Q+20>>2]=e;break A}Ve(4,143,144);break Q;case 5:a[i>>2]=a[Q+20>>2];break A;case 1:case 6:if(6!=a[Q+20>>2]){Ve(4,143,146);break Q}if(4103==(0|r)){a[i>>2]=a[Q+32>>2];break A}if((0|e)<-3)break Q;c:if(-1!=(0|(r=a[Q+36>>2]))){C:switch(e+2|0){case 0:if(16!=a[A+16>>2])break c;Ve(4,143,146);break Q;default:if((0|e)<0|(0|r)<=(0|e))break c;break e;case 1:}if((0|wf(a[Q+24>>2]))<(0|r))break e}a[Q+32>>2]=e;break A;case 2:if((0|e)<=511){Ve(4,143,120);break Q}a[Q>>2]=e;break A;case 3:if(!i||!lf(i)||Qf(i)){Ve(4,143,101);break Q}xi(a[Q+4>>2]),a[Q+4>>2]=i;break A;case 12:if(e-6>>>0<=4294967291){Ve(4,143,165);break Q}a[Q+8>>2]=e;break A;case 4:case 7:break b;case 8:case 10:break n;case 11:break B;case 9:break t}if(4!=a[Q+20>>2]){Ve(4,143,141);break Q}if(4107==(0|r))break r;a[Q+24>>2]=i;break A}if(!de(i,a[Q+20>>2]))return 0;if(-1!=a[Q+36>>2]){if(a[a[Q+24>>2]>>2]==a[i>>2])break i;return Ve(4,143,145),0}a[Q+24>>2]=i;break A}b:switch(a[Q+20>>2]-4|0){default:Ve(4,143,156);break Q;case 0:case 2:}if(4104==(0|r)){if(A=a[Q+28>>2]){a[i>>2]=A;break A}break r}if(-1!=a[Q+36>>2]){if(a[a[Q+28>>2]>>2]==a[i>>2])break i;return Ve(4,143,152),0}a[Q+28>>2]=i;break A}if(4!=a[Q+20>>2]){Ve(4,143,141);break Q}if(SQ(a[Q+44>>2],31289,0),!(!i|(0|e)<=0)){a[Q+48>>2]=e,a[Q+44>>2]=i;break A}a[Q+44>>2]=0,a[Q+48>>2]=0;break A}if(4!=a[Q+20>>2]){Ve(4,143,141);break Q}return a[i>>2]=a[Q+44>>2],a[Q+48>>2]}if(912!=a[a[A>>2]>>2])break i}Ve(4,143,148)}f=-2}return 0|f}return Ve(4,143,164),0}a[i>>2]=a[Q+24>>2]}return 1},function(A,r,e){A|=0,r|=0;var Q,f=0,t=0,b=0,k=0,c=0,C=0,E=0,o=0,l=0,w=0;s=Q=s-16|0;A:if(e|=0){r:{if(!mi(r,15428)){if(mi(e,28687))if(mi(e,27513))if(mi(e,13955)){if(r=4,mi(e,8850)&&mi(e,8554))if(mi(e,29663)){if(mi(e,4908))break r;r=6}else r=5}else r=3;else r=2;else r=1;k=qi(A,-1,4097,r,0);break A}if(!mi(r,10391)){r=-1,mi(e,2250)&&(r=-3,mi(e,1806)&&(r=-2,mi(e,8903)&&(r=qe(e)))),k=qi(A,24,4098,r,0);break A}if(!mi(r,4863)){k=qi(A,4,4099,qe(e),0);break A}if(!mi(r,8211)){a[Q+12>>2]=0,o=Q+12|0;e:{i:{if(f=(45==B[0|e])+e|0,!(48!=B[0|f]|120!=(32|B[f+1|0]))){Q:{f:if(b=f+2|0){if(!(t=B[0|b]))break Q;for(45==(0|t)&&(l=1,b=b+1|0);;){if(Mf(i[b+c|0],16)){if(536870912!=(0|(c=c+1|0)))continue;break f}break}if(c){if(t=c+l|0,!o)break Q;a:{if(!(f=a[o>>2])){if(f=KQ(1))break a;break f}xf(f)}if(!(c>>>0>536870904)&&(!((0|(r=31+(c<<2)>>>5|0))>a[f+8>>2])||ke(f,r))){for(;;){for(r=(0|c)<8?c:8,E=0;E=(w=E<<4)|((0|(E=jQ(B[(c-r|0)+b|0])))>0?E:0),w=r>>>0>1,r=r-1|0,w;);if(a[a[f>>2]+(C<<2)>>2]=E,C=C+1|0,r=(0|c)>8,c=c-8|0,!r)break}if(a[f+4>>2]=C,Hi(f),a[o>>2]=f,!a[f+4>>2])break Q;a[f+12>>2]=l;break Q}a[o>>2]||xi(f)}}t=0}if(t)break i;r=0;break e}r=0;Q:if(f)f:if(r=B[0|f]){45==(0|r)&&(l=1,f=f+1|0);a:{for(;;){if(Mf(i[f+C|0],4)){if(536870912!=(0|(C=C+1|0)))continue;break a}break}if(C){if(r=C+l|0,!o)break f;B:{if(!(t=a[o>>2])){if(t=KQ(1))break B;r=0;break Q}xf(t)}if(!(C>>>0>536870904)&&(!((0|(b=31+(C<<2)>>>5|0))>a[t+8>>2])||ke(t,b))){for(E=(b=(C>>>0)%9|0)?9-b|0:0;;){if(b=(i[0|f]+n(c,10)|0)-48|0,9==(0|(E=E+1|0))){if(!Wi(t,1e9))break a;if(c=0,E=0,!Be(t,b))break a}else c=b;if(f=f+1|0,b=C>>>0>1,C=C-1|0,!b)break}if(Hi(t),a[o>>2]=t,!a[t+4>>2])break f;a[t+12>>2]=l;break f}}}r=0,a[o>>2]||xi(t)}if(!r){r=0;break e}}45==B[0|e]&&(r=a[o>>2],a[r+4>>2]&&(a[r+12>>2]=1)),r=1}if(!r)break A;if((0|(k=qi(A,4,4100,0,a[Q+12>>2])))>0)break A;xi(a[Q+12>>2]);break A}if(!mi(r,6348)){k=qi(A,4,4109,qe(e),0);break A}if(!mi(r,16304)){k=Me(A,1016,4101,e);break A}if(912==a[a[A>>2]>>2]){if(!mi(r,16281)){k=Me(A,4,4101,e);break A}if(!mi(r,16263)){k=Me(A,4,1,e);break A}if(!mi(r,10407)){k=Ie(A,912,4,4098,qe(e),0);break A}}if(!mi(r,16251)){k=Me(A,768,4105,e);break A}if(k=-2,mi(r,11760))break A;if(!(r=Ze(e,Q+8|0))){k=0;break A}if((0|(k=Ie(A,6,768,4106,a[Q+8>>2],r)))>0)break A;SQ(r,31289,0);break A}Ve(4,144,118),k=-2}else Ve(4,144,147);return s=Q+16|0,0|k},function(A){var r,e=0,i=0,Q=0;return s=r=s-16|0,912==a[a[(A|=0)>>2]>>2]&&(e=a[a[A+8>>2]+24>>2],(i=a[e+52>>2])?(A=a[A+20>>2],Qe(i,r+12|0,r+8|0,r+4|0)&&(Q=1,(0|(i=(hf(e)-wf(a[r+12>>2])|0)-(1==(7&Dr(a[e+16>>2])))|0))<(0|(e=a[r+4>>2]))?(Q=0,Ve(4,165,150)):(a[A+36>>2]=e,a[A+24>>2]=a[r+12>>2],i=a[r+8>>2],a[A+32>>2]=e,a[A+28>>2]=i))):Q=1),s=r+16|0,0|Q},Si,function(A){return 16},PQ,function(A,r,e,i){return-2},function(A,r,e){r|=0,e|=0;var i=0;if(!(a[24+(A|=0)>>2]|16!=(0|e))&&(e=xQ(4))){if(!Aa(e,r,16))return wQ(e),0;a[A+24>>2]=e,i=1}return 0|i},function(A,r,e){A|=0,e|=0;var i=0;A:{if(r|=0){if(!(A=a[A+24>>2])|b[e>>2]<16)break A;yQ(r,a[A+8>>2],a[A>>2])}a[e>>2]=16,i=1}return 0|i},function(A,r,e){r|=0,e|=0;var i,Q=0,f=0,t=0,b=0,k=0,n=0,c=0,C=0,E=0,o=0,l=0,w=0,g=0,u=0,M=0;i=a[a[16+(A|=0)>>2]+20>>2]+16|0,A=a[i+4>>2],A=(Q=e+a[i>>2]|0)>>>0<e>>>0?A+1|0:A,a[i>>2]=Q,a[i+4>>2]=A,f=a[i+32>>2],k=a[i+36>>2],A=a[i+24>>2],n=a[i+28>>2],c=a[i+16>>2],C=a[i+20>>2],b=a[i+8>>2],Q=a[i+12>>2];A:{if(t=a[i+40>>2]){if((o=8-t|0)>>>0>e>>>0){yQ(56+(i+t|0)|0,r,e),a[i+40>>2]=a[i+40>>2]+e;break A}if(yQ(t+(i+56|0)|0,r,o),f^=w=a[i+56>>2],k^=g=a[i+60>>2],(0|(u=a[i+48>>2]))>0)for(;t=A,E=A+f|0,A=k+n|0,t=t>>>0>(n=E)>>>0?A+1|0:A,A=Q+C|0,A=(b=b+c|0)>>>0<c>>>0?A+1|0:A,C=we(c,C,13)^b,Q=A,A=(E=A^N)+t|0,A=(c=C+n|0)>>>0<C>>>0?A+1|0:A,M=c,c=we(C,E,17)^c,C=N^A,f=we(f,k,16)^n,k=t^=N,t=we(f,t,21),n=N,E=t,t=f,f=we(b,Q,32),Q=N+k|0,Q=f>>>0>(t=t+f|0)>>>0?Q+1|0:Q,b=t,f=E^t,k=Q^n,A=we(M,A,32),n=N,(0|u)!=(0|(l=l+1|0)););e=e-o|0,b^=w,Q^=g,r=r+o|0}r:if((0|(o=(r+e|0)-(t=7&e)|0))!=(0|r)){if((0|(w=a[i+48>>2]))>0)for(;;){for(f^=g=B[0|r]|B[r+1|0]<<8|B[r+2|0]<<16|B[r+3|0]<<24,k^=u=B[r+4|0]|B[r+5|0]<<8|B[r+6|0]<<16|B[r+7|0]<<24,l=0;e=A,E=A+f|0,A=k+n|0,e=e>>>0>(n=E)>>>0?A+1|0:A,A=Q+C|0,A=(b=b+c|0)>>>0<c>>>0?A+1|0:A,C=we(c,C,13)^b,Q=A,A=(E=A^N)+e|0,A=(c=C+n|0)>>>0<C>>>0?A+1|0:A,M=c,c=we(C,E,17)^c,C=N^A,f=we(f,k,16)^n,k=e^=N,e=we(f,e,21),n=N,E=e,e=f,f=we(b,Q,32),Q=N+k|0,Q=(e=e+f|0)>>>0<f>>>0?Q+1|0:Q,b=e,f=E^e,k=Q^n,A=we(M,A,32),n=N,(0|w)!=(0|(l=l+1|0)););if(b^=g,Q^=u,(0|o)==(0|(r=r+8|0)))break r}for(;b^=e=B[0|r]|B[r+1|0]<<8|B[r+2|0]<<16|B[r+3|0]<<24,Q^=l=B[r+4|0]|B[r+5|0]<<8|B[r+6|0]<<16|B[r+7|0]<<24,f^=e,k^=l,(0|o)!=(0|(r=r+8|0)););}t&&yQ(i+56|0,o,t),a[i+32>>2]=f,a[i+36>>2]=k,a[i+24>>2]=A,a[i+28>>2]=n,a[i+16>>2]=c,a[i+20>>2]=C,a[i+8>>2]=b,a[i+12>>2]=Q,a[i+40>>2]=t}return 1},function(A){var r;return A|=0,(r=Ki(80,31289))?(a[r+4>>2]=4,Ca(A,r),vf(A),1):(Ve(15,125,65),0)},function(A,r){A|=0,r|=0;var e,i=0,Q=0;return(i=Ki(80,31289))?(a[i+4>>2]=4,Ca(A,i),vf(A),i=a[A+20>>2],r=a[r+20>>2],a[r+8>>2]&&(r&&(a[i+4>>2]=a[r+4>>2],ze(i,a[r+8>>2],a[r>>2])&&(Q=128&a[i+12>>2],a[i+12>>2]=Q,a[i+12>>2]=Q|-129&a[r+12>>2],Q=1)),!Q)?(r=a[A+20>>2])?(pQ(a[r+8>>2],a[r>>2]),pQ(r,80),Ca(A,0),0):0:(A=a[r+20>>2],a[i+16>>2]=a[r+16>>2],a[i+20>>2]=A,A=a[r+76>>2],a[i+72>>2]=a[r+72>>2],a[i+76>>2]=A,e=a[4+(Q=r- -64|0)>>2],a[(A=i- -64|0)>>2]=a[Q>>2],a[A+4>>2]=e,A=a[r+60>>2],a[i+56>>2]=a[r+56>>2],a[i+60>>2]=A,A=a[r+52>>2],a[i+48>>2]=a[r+48>>2],a[i+52>>2]=A,A=a[r+44>>2],a[i+40>>2]=a[r+40>>2],a[i+44>>2]=A,A=a[r+36>>2],a[i+32>>2]=a[r+32>>2],a[i+36>>2]=A,A=a[r+28>>2],a[i+24>>2]=a[r+24>>2],a[i+28>>2]=A,1)):(Ve(15,125,65),0)},function(A){var r;(r=a[20+(A|=0)>>2])&&(pQ(a[r+8>>2],a[r>>2]),pQ(r,80),Ca(A,0))},function(A,r){r|=0;var e=0;return A=a[20+(A|=0)>>2],a[A+8>>2]&&(A=je(A))&&(e=ge(r,1062,A)),0|e},function(A,r){r|=0;var e,i,Q=0;return s=e=s-16|0,i=a[20+(A|=0)>>2],!(A=XQ(a[A+8>>2],e+12|0))|16!=a[e+12>>2]||(Kf(r,256),Ca(r,167),Q=dr(i+16|0,A)),s=e+16|0,0|Q},function(A,r,e,Q){r|=0,e|=0,Q|=0;var f,t,b=0,k=0,n=0,c=0,C=0,E=0,o=0,l=0,w=0,g=0,u=0,M=0,s=0,D=0;if(f=a[20+(A|=0)>>2]+16|0,t=a[f+44>>2],a[e>>2]=t,!r)return 1;if(Q=0,(0|t)==a[f+44>>2]){n=a[f>>2]<<24,E=a[f+32>>2],c=a[f+36>>2];A:switch(a[f+40>>2]-1|0){case 6:n|=B[f+62|0]<<16;case 5:n|=B[f+61|0]<<8;case 4:n|=B[f+60|0];case 3:u|=(A=B[f+59|0])<<24,n|=e=A>>>8|0;case 2:u|=(e=B[f+58|0])<<16,n|=A=e>>>16|0;case 1:u|=(A=B[f+57|0])<<8,n|=e=A>>>24|0;case 0:u=B[f+56|0]|u}if(C=a[f+24>>2],k=a[f+28>>2],l=a[f+16>>2],w=a[f+20>>2],b=a[f+8>>2],e=a[f+12>>2],E^=u,g=n^c,(0|(M=a[f+48>>2]))>0)for(;A=k+g|0,D=c=E+C|0,c=A=c>>>0<E>>>0?A+1|0:A,e=e+w|0,e=(k=b+l|0)>>>0<b>>>0?e+1|0:e,s=k,C=we(l,w,13)^k,A=(b=N^e)+A|0,A=(k=C)>>>0>(o=k+D|0)>>>0?A+1|0:A,k=o,l=we(C,b,17)^k,w=N^A,C=we(E,g,16)^D,E=b=c^N,b=we(C,b,21),c=N,o=b,b=we(s,e,32),e=N+E|0,e=b>>>0>(C=b+C|0)>>>0?e+1|0:e,E=o^(b=C),g=e^c,C=we(k,A,32),k=N,(0|M)!=(0|(Q=Q+1|0)););if(b^=u,e^=n,C^=16==(0|t)?238:255,(0|(s=a[f+52>>2]))>0)for(Q=0;A=k+g|0,o=c=E+C|0,c=A=c>>>0<E>>>0?A+1|0:A,e=e+w|0,e=(n=b+l|0)>>>0<b>>>0?e+1|0:e,C=n,k=we(l,w,13)^n,A=(b=N^e)+A|0,A=k>>>0>(M=k+o|0)>>>0?A+1|0:A,n=M,l=we(k,b,17)^n,w=N^A,k=we(E,g,16)^o,E=b=c^N,b=we(k,b,21),c=N,o=b,b=we(C,e,32),e=N+E|0,e=b>>>0>(k=b+k|0)>>>0?e+1|0:e,E=o^(b=k),g=e^c,C=we(n,A,32),k=N,(0|s)!=(0|(Q=Q+1|0)););if(A=E^C^b^l,i[0|r]=A,i[r+1|0]=A>>>8,i[r+2|0]=A>>>16,i[r+3|0]=A>>>24,A=g^k^e^w,i[r+4|0]=A,i[r+5|0]=A>>>8,i[r+6|0]=A>>>16,i[r+7|0]=A>>>24,M=1,8!=a[f+44>>2]){if(l^=221,(0|(s=a[f+52>>2]))>0)for(Q=0;A=k+g|0,D=c=E+C|0,c=A=c>>>0<E>>>0?A+1|0:A,A=e+w|0,A=(n=b+l|0)>>>0<b>>>0?A+1|0:A,C=n,k=we(l,w,13)^n,b=A,e=(A^=N)+c|0,e=k>>>0>(o=k+D|0)>>>0?e+1|0:e,n=o,l=we(k,A,17)^n,w=(A=e)^N,k=we(E,g,16)^D,E=e=c^N,e=we(k,e,21),c=N,o=e,b=we(C,b,32),e=N+E|0,e=b>>>0>(k=b+k|0)>>>0?e+1|0:e,E=o^(b=k),g=e^c,C=we(n,A,32),k=N,(0|s)!=(0|(Q=Q+1|0)););A=E^C^b^l,i[r+8|0]=A,i[r+9|0]=A>>>8,i[r+10|0]=A>>>16,i[r+11|0]=A>>>24,A=g^k^e^w,i[r+12|0]=A,i[r+13|0]=A>>>8,i[r+14|0]=A>>>16,i[r+15|0]=A>>>24}}return 0|M},function(A,r,e,i){r|=0,e|=0,i|=0;var Q,f,B=0;s=Q=s-16|0,f=a[20+(A|=0)>>2],B=-2;A:{r:{e:{i:switch(r-1|0){case 13:B=ui(f+16|0,e);break A;case 5:a[Q+12>>2]=e;break e;case 0:break r;case 6:break i;default:break A}i=XQ(a[A+8>>2],Q+12|0),e=a[Q+12>>2]}if(B=0,!i|16!=(0|e))break A;if(!Aa(f,i,16))break A;B=dr(f+16|0,a[f+8>>2]);break A}B=1}return s=Q+16|0,0|B},function(A,r,e){return A|=0,(e|=0)?mi(r|=0,13131)?mi(r,1439)?0|(A=mi(r,1425)?-2:aQ(A,6,e)):0|vQ(A,6,e):(r=qe(e),0|ui(a[A+20>>2]+16|0,r)):0},function(A,r,e,i){r|=0,e|=0,i|=0;var Q=0,f=0;e=1;A:{r:{e:switch((A|=0)-3|0){case 0:Ii(a[a[r>>2]+8>>2]);break r;case 2:break e;default:break A}i=a[r>>2],Ii(a[i+8>>2]),e=0,a[i+8>>2]=0,uQ();e:if(A=vi()){i:{f=gr(a[a[i>>2]>>2]),s=r=s-16|0,a[r+12>>2]=0;Q:{f:{if(A){if(a[A+24>>2]&&(Q=a[A+12>>2])&&(Q=a[Q+88>>2])&&(Ga[0|Q](A),a[A+24>>2]=0),(0|f)==a[A+4>>2]&&(Q=1,a[A+12>>2]))break Q;if(!(Q=Je(r+12|0,f)))break f;a[A+12>>2]=Q,Q=a[Q>>2],a[A+4>>2]=f,a[A>>2]=Q,a[A+16>>2]=a[r+12>>2],Q=1;break Q}if(Q=1,Je(r+12|0,f))break Q}Ve(6,158,156),Q=0}if(s=r+16|0,r=111,Q&&(r=124,f=a[a[A+12>>2]+20>>2])){if(0|Ga[0|f](A,i))break i;r=125}Ve(11,148,r),Ii(A),A=0;break e}a[i+8>>2]=A,A=1}else Ve(11,148,65),A=-1;if(-1==(0|A))break A;$r()}e=1}return 0|e}],Na.set=function(A,r){this[A]=r},Na.get=function(A){return this[A]},Na);return{h:function(){var A,r=0;s=A=s-16|0,0|u(A+12|0,A+8|0)||(r=G(4+(a[A+12>>2]<<2)|0),a[23333]=r,r&&(!(r=G(a[A+8>>2]))||(a[a[23333]+(a[A+12>>2]<<2)>>2]=0,0|g(a[23333],0|r)))&&(a[23333]=0)),s=A+16|0},i:Ga,j:m,k:G,l:function(){var A;s=A=s-16|0,i[A+15|0]=0,o(91144,A+15|0,0),s=A+16|0},m:function(){return 450},n:function(){return 1700},o:function(){return 256},p:function(A,r){A|=0,r|=0;var e,i,Q,f=0,B=0,t=0;if(s=i=s-16|0,a[i+8>>2]=r,a[i+12>>2]=A,Q=KQ(1),e=JA(),Be(Q,65537),r=1,1==(0|(A=(A=a[a[e+8>>2]+52>>2])?0|Ga[0|A](e,2048,Q,0):V(e,2048,2,Q,0)))){if(t=i+12|0,e)if(A=vi()){s=r=s-16|0;A:if(A){a[r+12>>2]=0,a[A+24>>2]&&(f=a[A+12>>2])&&(f=a[f+88>>2])&&(Ga[0|f](A),a[A+24>>2]=0);r:{if(!a[A+12>>2]||6!=a[A+4>>2]){if(!(f=Je(r+12|0,6)))break r;a[A+12>>2]=f,f=a[f>>2],a[A+4>>2]=6,a[A>>2]=f,a[A+16>>2]=a[r+12>>2]}if(a[A+24>>2]=e,!e)break A;a[e+60>>2]=a[e+60>>2]+1;break A}Ve(6,158,156)}s=r+16|0;A:if(A){r:{if(r=HQ(90720)){if(B=111,(f=a[A+12>>2])&&(B=124,f=a[f+24>>2])){if(0|Ga[0|f](r,A))break r;B=126}Ve(11,120,B)}ef(r,90720);break A}ef(0,90720),a[r+8>>2]=A,_f(A),xe(r,t,90720),ef(r,90720)}Ii(A)}else Ve(13,165,65);Pf(e,i+8|0),be(e),xi(Q),r=0}return s=i+16|0,0|r},q:function(A,r,e,i,Q){var f;return A|=0,r|=0,e|=0,i|=0,Q|=0,s=f=s-160|0,a[f+156>>2]=i,a[f+152>>2]=JA(),Xf(f+152|0,f+156|0,Q)?(Ui(f),Ce(f,r,e),_(r=f+112|0,f),i=fe(672,r,32,A,0,a[f+152>>2]),be(a[f+152>>2])):i=-1,s=f+160|0,0|i},r:function(A,r,e,i,Q){var f;return A|=0,r|=0,e|=0,i|=0,Q|=0,s=f=s-160|0,a[f+156>>2]=i,a[f+152>>2]=JA(),Hf(f+152|0,f+156|0,Q)?(Ui(f),Ce(f,r,e),_(r=f+112|0,f),i=YQ(672,r,32,A,256,a[f+152>>2]),be(a[f+152>>2])):i=-1,s=f+160|0,0|i}}}(A)}(asmLibraryArg)},instantiate:function(A,r){return{then:function(r){var e=new WebAssembly.Module(A);r({instance:new WebAssembly.Instance(e)})}}},RuntimeError:Error},wasmMemory;wasmBinary=[],"object"!=_typeof(WebAssembly)&&abort("no native wasm support detected");var ABORT=!1,EXITSTATUS;function assert(A,r){A||abort(r)}var UTF8Decoder="undefined"!=typeof TextDecoder?new TextDecoder("utf8"):void 0,buffer,HEAP8,HEAPU8,HEAP16,HEAPU16,HEAP32,HEAPU32,HEAPF32,HEAPF64;function UTF8ArrayToString(A,r,e){for(var i=r+e,Q=r;A[Q]&&!(Q>=i);)++Q;if(Q-r>16&&A.buffer&&UTF8Decoder)return UTF8Decoder.decode(A.subarray(r,Q));for(var f="";r<Q;){var a=A[r++];if(128&a){var B=63&A[r++];if(192!=(224&a)){var t=63&A[r++];if((a=224==(240&a)?(15&a)<<12|B<<6|t:(7&a)<<18|B<<12|t<<6|63&A[r++])<65536)f+=String.fromCharCode(a);else{var b=a-65536;f+=String.fromCharCode(55296|b>>10,56320|1023&b)}}else f+=String.fromCharCode((31&a)<<6|B)}else f+=String.fromCharCode(a)}return f}function UTF8ToString(A,r){return A?UTF8ArrayToString(HEAPU8,A,r):""}function writeArrayToMemory(A,r){HEAP8.set(A,r)}function writeAsciiToMemory(A,r,e){for(var i=0;i<A.length;++i)HEAP8[r++>>0]=A.charCodeAt(i);e||(HEAP8[r>>0]=0)}function updateGlobalBufferAndViews(A){buffer=A,Module.HEAP8=HEAP8=new Int8Array(A),Module.HEAP16=HEAP16=new Int16Array(A),Module.HEAP32=HEAP32=new Int32Array(A),Module.HEAPU8=HEAPU8=new Uint8Array(A),Module.HEAPU16=HEAPU16=new Uint16Array(A),Module.HEAPU32=HEAPU32=new Uint32Array(A),Module.HEAPF32=HEAPF32=new Float32Array(A),Module.HEAPF64=HEAPF64=new Float64Array(A)}var INITIAL_MEMORY=Module.INITIAL_MEMORY||16777216,wasmTable;wasmMemory=Module.wasmMemory?Module.wasmMemory:new WebAssembly.Memory({initial:INITIAL_MEMORY/65536,maximum:INITIAL_MEMORY/65536}),wasmMemory&&(buffer=wasmMemory.buffer),INITIAL_MEMORY=buffer.byteLength,updateGlobalBufferAndViews(buffer);var __ATPRERUN__=[],__ATINIT__=[],__ATPOSTRUN__=[],runtimeInitialized=!1;function keepRuntimeAlive(){return noExitRuntime}function preRun(){if(Module.preRun)for("function"==typeof Module.preRun&&(Module.preRun=[Module.preRun]);Module.preRun.length;)addOnPreRun(Module.preRun.shift());callRuntimeCallbacks(__ATPRERUN__)}function initRuntime(){runtimeInitialized=!0,callRuntimeCallbacks(__ATINIT__)}function postRun(){if(Module.postRun)for("function"==typeof Module.postRun&&(Module.postRun=[Module.postRun]);Module.postRun.length;)addOnPostRun(Module.postRun.shift());callRuntimeCallbacks(__ATPOSTRUN__)}function addOnPreRun(A){__ATPRERUN__.unshift(A)}function addOnInit(A){__ATINIT__.unshift(A)}function addOnPostRun(A){__ATPOSTRUN__.unshift(A)}var runDependencies=0,runDependencyWatcher=null,dependenciesFulfilled=null;function addRunDependency(A){runDependencies++,Module.monitorRunDependencies&&Module.monitorRunDependencies(runDependencies)}function removeRunDependency(A){if(runDependencies--,Module.monitorRunDependencies&&Module.monitorRunDependencies(runDependencies),0==runDependencies&&(null!==runDependencyWatcher&&(clearInterval(runDependencyWatcher),runDependencyWatcher=null),dependenciesFulfilled)){var r=dependenciesFulfilled;dependenciesFulfilled=null,r()}}function abort(A){throw Module.onAbort&&Module.onAbort(A),err(A="Aborted("+A+")"),ABORT=!0,EXITSTATUS=1,A+=". Build with -sASSERTIONS for more info.",new WebAssembly.RuntimeError(A)}var dataURIPrefix="data:application/octet-stream;base64,",wasmBinaryFile;function isDataURI(A){return A.startsWith(dataURIPrefix)}function isFileURI(A){return A.startsWith("file://")}function getBinary(A){try{if(A==wasmBinaryFile&&wasmBinary)return new Uint8Array(wasmBinary);var r=tryParseAsDataURI(A);if(r)return r;if(readBinary)return readBinary(A);throw"both async and sync fetching of the wasm failed"}catch(A){abort(A)}}function getBinaryPromise(){if(!wasmBinary&&(ENVIRONMENT_IS_WEB||ENVIRONMENT_IS_WORKER)){if("function"==typeof fetch&&!isFileURI(wasmBinaryFile))return fetch(wasmBinaryFile,{credentials:"same-origin"}).then((function(A){if(!A.ok)throw"failed to load wasm binary file at '"+wasmBinaryFile+"'";return A.arrayBuffer()})).catch((function(){return getBinary(wasmBinaryFile)}));if(readAsync)return new Promise((function(A,r){readAsync(wasmBinaryFile,(function(r){A(new Uint8Array(r))}),r)}))}return Promise.resolve().then((function(){return getBinary(wasmBinaryFile)}))}function createWasm(){var A={a:asmLibraryArg};function r(A,r){var e=A.exports;Module.asm=e,wasmTable=Module.asm.i,addOnInit(Module.asm.h),removeRunDependency("wasm-instantiate")}function e(A){r(A.instance)}function i(r){return getBinaryPromise().then((function(r){return WebAssembly.instantiate(r,A)})).then((function(A){return A})).then(r,(function(A){err("failed to asynchronously prepare wasm: "+A),abort(A)}))}if(addRunDependency("wasm-instantiate"),Module.instantiateWasm)try{return Module.instantiateWasm(A,r)}catch(A){return err("Module.instantiateWasm callback failed with error: "+A),!1}return wasmBinary||"function"!=typeof WebAssembly.instantiateStreaming||isDataURI(wasmBinaryFile)||isFileURI(wasmBinaryFile)||ENVIRONMENT_IS_NODE||"function"!=typeof fetch?i(e):fetch(wasmBinaryFile,{credentials:"same-origin"}).then((function(r){return WebAssembly.instantiateStreaming(r,A).then(e,(function(A){return err("wasm streaming compile failed: "+A),err("falling back to ArrayBuffer instantiation"),i(e)}))})),{}}wasmBinaryFile="<<< WASM_BINARY_FILE >>>",isDataURI(wasmBinaryFile)||(wasmBinaryFile=locateFile(wasmBinaryFile));var ASM_CONSTS={91108:function(){return Module.getRandomValue()},91144:function _(){if(void 0===Module.getRandomValue)try{var window_="object"===("undefined"==typeof window?"undefined":_typeof(window))?window:self,crypto_=void 0!==window_.crypto?window_.crypto:window_.msCrypto,randomValuesStandard=function(){var A=new Uint32Array(1);return crypto_.getRandomValues(A),A[0]>>>0};randomValuesStandard(),Module.getRandomValue=randomValuesStandard}catch(e){try{var crypto=eval("require")("crypto"),randomValueNodeJS=function(){var A=crypto.randomBytes(4);return(A[0]<<24|A[1]<<16|A[2]<<8|A[3])>>>0};randomValueNodeJS(),Module.getRandomValue=randomValueNodeJS}catch(A){throw"No secure random number generator found"}}}};function callRuntimeCallbacks(A){for(;A.length>0;)A.shift()(Module)}var SYSCALLS={varargs:void 0,get:function(){return SYSCALLS.varargs+=4,HEAP32[SYSCALLS.varargs-4>>2]},getStr:function(A){return UTF8ToString(A)}};function __munmap_js(A,r,e,i,Q,f){}function _abort(){abort("")}var readAsmConstArgsArray=[];function readAsmConstArgs(A,r){var e;for(readAsmConstArgsArray.length=0,r>>=2;e=HEAPU8[A++];)r+=105!=e&r,readAsmConstArgsArray.push(105==e?HEAP32[r]:HEAPF64[r++>>1]),++r;return readAsmConstArgsArray}function _emscripten_asm_const_int(A,r,e){var i=readAsmConstArgs(r,e);return ASM_CONSTS[A].apply(null,i)}function abortOnCannotGrowMemory(A){abort("OOM")}function _emscripten_resize_heap(A){HEAPU8.length;abortOnCannotGrowMemory(A>>>=0)}var ENV={};function getExecutableName(){return thisProgram||"./this.program"}function getEnvStrings(){if(!getEnvStrings.strings){var A={USER:"web_user",LOGNAME:"web_user",PATH:"/",PWD:"/",HOME:"/home/web_user",LANG:("object"==("undefined"==typeof navigator?"undefined":_typeof(navigator))&&navigator.languages&&navigator.languages[0]||"C").replace("-","_")+".UTF-8",_:getExecutableName()};for(var r in ENV)void 0===ENV[r]?delete A[r]:A[r]=ENV[r];var e=[];for(var r in A)e.push(r+"="+A[r]);getEnvStrings.strings=e}return getEnvStrings.strings}function _environ_get(A,r){var e=0;return getEnvStrings().forEach((function(i,Q){var f=r+e;HEAPU32[A+4*Q>>2]=f,writeAsciiToMemory(i,f),e+=i.length+1})),0}function _environ_sizes_get(A,r){var e=getEnvStrings();HEAPU32[A>>2]=e.length;var i=0;return e.forEach((function(A){i+=A.length+1})),HEAPU32[r>>2]=i,0}var ASSERTIONS=!1;function intArrayToString(A){for(var r=[],e=0;e<A.length;e++){var i=A[e];i>255&&(ASSERTIONS&&assert(!1,"Character code "+i+" ("+String.fromCharCode(i)+")  at offset "+e+" not in 0x00-0xFF."),i&=255),r.push(String.fromCharCode(i))}return r.join("")}var decodeBase64="function"==typeof atob?atob:function(A){var r,e,i,Q,f,a,B="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=",t="",b=0;A=A.replace(/[^A-Za-z0-9\+\/\=]/g,"");do{r=B.indexOf(A.charAt(b++))<<2|(Q=B.indexOf(A.charAt(b++)))>>4,e=(15&Q)<<4|(f=B.indexOf(A.charAt(b++)))>>2,i=(3&f)<<6|(a=B.indexOf(A.charAt(b++))),t+=String.fromCharCode(r),64!==f&&(t+=String.fromCharCode(e)),64!==a&&(t+=String.fromCharCode(i))}while(b<A.length);return t};function intArrayFromBase64(A){if("boolean"==typeof ENVIRONMENT_IS_NODE&&ENVIRONMENT_IS_NODE){var r=Buffer.from(A,"base64");return new Uint8Array(r.buffer,r.byteOffset,r.byteLength)}try{for(var e=decodeBase64(A),i=new Uint8Array(e.length),Q=0;Q<e.length;++Q)i[Q]=e.charCodeAt(Q);return i}catch(A){throw new Error("Converting base64 string to bytes failed.")}}function tryParseAsDataURI(A){if(isDataURI(A))return intArrayFromBase64(A.slice(dataURIPrefix.length))}var asmLibraryArg={d:__munmap_js,g:_abort,b:_emscripten_asm_const_int,c:_emscripten_resize_heap,e:_environ_get,f:_environ_sizes_get,a:wasmMemory},asm=createWasm(),___wasm_call_ctors=Module.___wasm_call_ctors=function(){return(___wasm_call_ctors=Module.___wasm_call_ctors=Module.asm.h).apply(null,arguments)},_free=Module._free=function(){return(_free=Module._free=Module.asm.j).apply(null,arguments)},_malloc=Module._malloc=function(){return(_malloc=Module._malloc=Module.asm.k).apply(null,arguments)},_rsasignjs_init=Module._rsasignjs_init=function(){return(_rsasignjs_init=Module._rsasignjs_init=Module.asm.l).apply(null,arguments)},_rsasignjs_public_key_bytes=Module._rsasignjs_public_key_bytes=function(){return(_rsasignjs_public_key_bytes=Module._rsasignjs_public_key_bytes=Module.asm.m).apply(null,arguments)},_rsasignjs_secret_key_bytes=Module._rsasignjs_secret_key_bytes=function(){return(_rsasignjs_secret_key_bytes=Module._rsasignjs_secret_key_bytes=Module.asm.n).apply(null,arguments)},_rsasignjs_signature_bytes=Module._rsasignjs_signature_bytes=function(){return(_rsasignjs_signature_bytes=Module._rsasignjs_signature_bytes=Module.asm.o).apply(null,arguments)},_rsasignjs_keypair=Module._rsasignjs_keypair=function(){return(_rsasignjs_keypair=Module._rsasignjs_keypair=Module.asm.p).apply(null,arguments)},_rsasignjs_sign=Module._rsasignjs_sign=function(){return(_rsasignjs_sign=Module._rsasignjs_sign=Module.asm.q).apply(null,arguments)},_rsasignjs_verify=Module._rsasignjs_verify=function(){return(_rsasignjs_verify=Module._rsasignjs_verify=Module.asm.r).apply(null,arguments)},calledRun;function ExitStatus(A){this.name="ExitStatus",this.message="Program terminated with exit("+A+")",this.status=A}function run(A){function r(){calledRun||(calledRun=!0,Module.calledRun=!0,ABORT||(initRuntime(),Module.onRuntimeInitialized&&Module.onRuntimeInitialized(),postRun()))}A=A||arguments_,runDependencies>0||(preRun(),runDependencies>0||(Module.setStatus?(Module.setStatus("Running..."),setTimeout((function(){setTimeout((function(){Module.setStatus("")}),1),r()}),1)):r()))}if(Module.writeArrayToMemory=writeArrayToMemory,dependenciesFulfilled=function A(){calledRun||run(),calledRun||(dependenciesFulfilled=A)},Module.run=run,Module.preInit)for("function"==typeof Module.preInit&&(Module.preInit=[Module.preInit]);Module.preInit.length>0;)Module.preInit.pop()();run()}function dataResult(A,r){return new Uint8Array(new Uint8Array(Module.HEAPU8.buffer,A,r))}function dataFree(A){try{Module._free(A)}catch(A){setTimeout((function(){throw A}),0)}}function importJWK(A,r){return Promise.resolve().then((function(){var e=A.indexOf(0),i=JSON.parse(sodiumUtil.to_string(e>-1?new Uint8Array(A.buffer,A.byteOffset,e):A));return Promise.resolve().then((function(){return crypto.subtle.importKey("jwk",i,algorithm,!1,[r])})).catch((function(){return pemJwk.jwk2pem(i)}))}))}function exportJWK(A,r){return Promise.resolve().then((function(){return"string"==typeof A?pemJwk.pem2jwk(A):crypto.subtle.exportKey("jwk",A,algorithm.name)})).then((function(A){var e=sodiumUtil.from_string(JSON.stringify(A)),i=new Uint8Array(r);return i.set(e),sodiumUtil.memzero(e),i}))}function exportKeyPair(A){return Promise.all([exportJWK(A.publicKey,rsaSign.publicKeyBytes),exportJWK(A.privateKey,rsaSign.privateKeyBytes)]).then((function(A){return{publicKey:A[0],privateKey:A[1]}}))}[Float32Array,Float64Array,Int8Array,Int16Array,Int32Array,Uint8Array,Uint16Array,Uint32Array,Uint8ClampedArray].forEach((function(A){A.prototype.indexOf||(A.prototype.indexOf=function(A){for(var r=0;r<this.length;++r)if(this[r]===A)return r;return-1})}));var algorithm=isNode?"RSA-SHA256":{name:"RSASSA-PKCS1-v1_5",hash:{name:"SHA-256"},modulusLength:2048,publicExponent:new Uint8Array([1,0,1])},rsaSign={publicKeyBytes:450,privateKeyBytes:1700,bytes:256,keyPair:function(){return Promise.resolve().then((function(){if(isNode){var A=generateRSAKeypair();return{publicKey:A.public,privateKey:A.private}}return crypto.subtle.generateKey(algorithm,!0,["sign","verify"])})).then(exportKeyPair).catch((function(){return initiated.then((function(){var A=Module._malloc(rsaSign.publicKeyBytes),r=Module._malloc(rsaSign.privateKeyBytes);try{var e=Module._rsasignjs_keypair(A,r);if(0!==e)throw new Error("RSA Sign error: keyPair failed ("+e+")");return exportKeyPair({publicKey:"-----BEGIN PUBLIC KEY-----\n"+sodiumUtil.to_base64(dataResult(A,rsaSign.publicKeyBytes))+"\n-----END PUBLIC KEY-----",privateKey:"-----BEGIN RSA PRIVATE KEY-----\n"+sodiumUtil.to_base64(dataResult(r,rsaSign.privateKeyBytes))+"\n-----END RSA PRIVATE KEY-----"})}finally{dataFree(A,rsaSign.publicKeyBytes),dataFree(r,rsaSign.privateKeyBytes)}}))}))},sign:function(A,r){return rsaSign.signDetached(A,r).then((function(r){var e=new Uint8Array(rsaSign.bytes+A.length);return e.set(r),e.set(A,rsaSign.bytes),e}))},signDetached:function(A,r){return importJWK(r,"sign").then((function(r){return Promise.resolve().then((function(){if(isNode){var e=Buffer.from(A),i=nodeCrypto.createSign(algorithm);i.write(e),i.end();var Q=i.sign(r);return sodiumUtil.memzero(e),Q}return crypto.subtle.sign(algorithm,r,A)})).catch((function(){return initiated.then((function(){r=sodiumUtil.from_base64(r.split("-----")[2]);var e=Module._malloc(rsaSign.bytes),i=Module._malloc(A.length),Q=Module._malloc(r.length);Module.writeArrayToMemory(A,i),Module.writeArrayToMemory(r,Q);try{var f=Module._rsasignjs_sign(e,i,A.length,Q,r.length);if(1!==f)throw new Error("RSA Sign error: sign failed ("+f+")");return dataResult(e,rsaSign.bytes)}finally{dataFree(e),dataFree(i),dataFree(Q)}}))})).then((function(A){return sodiumUtil.memzero(r),new Uint8Array(A)}))}))},open:function(A,r){return Promise.resolve().then((function(){var e=new Uint8Array(A.buffer,A.byteOffset,rsaSign.bytes),i=new Uint8Array(A.buffer,A.byteOffset+rsaSign.bytes);return rsaSign.verifyDetached(e,i,r).then((function(A){if(A)return i;throw new Error("Failed to open RSA signed message.")}))}))},verifyDetached:function(A,r,e){return importJWK(e,"verify").then((function(e){return Promise.resolve().then((function(){if(isNode){var i=nodeCrypto.createVerify(algorithm);return i.update(Buffer.from(r)),i.verify(e,A)}return crypto.subtle.verify(algorithm,e,A,r)})).catch((function(){return initiated.then((function(){e=sodiumUtil.from_base64(e.split("-----")[2]);var i=Module._malloc(rsaSign.bytes),Q=Module._malloc(r.length),f=Module._malloc(e.length);Module.writeArrayToMemory(A,i),Module.writeArrayToMemory(r,Q),Module.writeArrayToMemory(e,f);try{return 1===Module._rsasignjs_verify(i,Q,r.length,f,e.length)}finally{dataFree(i),dataFree(Q),dataFree(f)}}))})).then((function(A){return sodiumUtil.memzero(e),A}))}))}};rsaSign.rsaSign=rsaSign,module.exports=rsaSign;

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