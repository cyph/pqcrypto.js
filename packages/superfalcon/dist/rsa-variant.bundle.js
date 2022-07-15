var superFalcon;
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


var falcon		= __webpack_require__(1);
var fastSHA512	= __webpack_require__(2);
var rsaSign		= __webpack_require__(5);
var sodiumUtil	= __webpack_require__(4);


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

	publicKeyBytes	= rsaSign.publicKeyBytes + falconBytes.publicKeyBytes;
	privateKeyBytes	= rsaSign.privateKeyBytes + falconBytes.privateKeyBytes;
	bytes			= rsaSign.bytes + falconBytes.bytes;
});


var superFalcon	= {
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
			falcon.keyPair()
		]).then(function (results) {
			var rsaKeyPair		= results[0];
			var falconKeyPair	= results[1];

			var keyPair	= {
				keyType: 'superfalcon',
				publicKey: new Uint8Array(publicKeyBytes),
				privateKey: new Uint8Array(privateKeyBytes)
			};

			keyPair.publicKey.set(rsaKeyPair.publicKey);
			keyPair.privateKey.set(rsaKeyPair.privateKey);
			keyPair.publicKey.set(falconKeyPair.publicKey, rsaSign.publicKeyBytes);
			keyPair.privateKey.set(falconKeyPair.privateKey, rsaSign.privateKeyBytes);

			sodiumUtil.memzero(falconKeyPair.privateKey);
			sodiumUtil.memzero(rsaKeyPair.privateKey);
			sodiumUtil.memzero(falconKeyPair.publicKey);
			sodiumUtil.memzero(rsaKeyPair.publicKey);

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
				rsaSign.signDetached(
					hash,
					new Uint8Array(
						privateKey.buffer,
						privateKey.byteOffset,
						rsaSign.privateKeyBytes
					)
				),
				falcon.signDetached(
					hash,
					new Uint8Array(
						privateKey.buffer,
						privateKey.byteOffset + rsaSign.privateKeyBytes
					)
				)
			]);
		}).then(function (results) {
			var hash			= results[0];
			var rsaSignature	= results[1];
			var falconSignature	= results[2];

			var signature	= new Uint8Array(bytes);

			signature.set(rsaSignature);
			signature.set(falconSignature, rsaSign.bytes);

			sodiumUtil.memzero(hash);
			sodiumUtil.memzero(falconSignature);
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
				throw new Error('Failed to open SuperFalcon signed message.');
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
					return rsaSign.verifyDetached(
						new Uint8Array(signature.buffer, signature.byteOffset, rsaSign.bytes),
						hash,
						new Uint8Array(pk.buffer, pk.byteOffset, rsaSign.publicKeyBytes)
					);
				}),
				hashAlreadyVerified || publicKeyPromise.then(function (pk) {
					return falcon.verifyDetached(
						new Uint8Array(
							signature.buffer,
							signature.byteOffset + rsaSign.bytes,
							falconBytes.bytes
						),
						hash,
						new Uint8Array(pk.buffer, pk.byteOffset + rsaSign.publicKeyBytes)
					);
				})
			]);
		}).then(function (results) {
			var hash			= results[0];
			var rsaIsValid		= results[1];
			var falconIsValid	= results[2];
			var valid			= rsaIsValid && falconIsValid;

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

			var falconPrivateKey		= new Uint8Array(
				falconBytes.publicKeyBytes +
				falconBytes.privateKeyBytes
			);

			var superFalconPrivateKey	= new Uint8Array(
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

			falconPrivateKey.set(new Uint8Array(
				keyPair.publicKey.buffer,
				keyPair.publicKey.byteOffset + rsaSign.publicKeyBytes
			));
			falconPrivateKey.set(
				new Uint8Array(
					keyPair.privateKey.buffer,
					keyPair.privateKey.byteOffset + rsaSign.privateKeyBytes
				),
				falconBytes.publicKeyBytes
			);

			superFalconPrivateKey.set(keyPair.publicKey);
			superFalconPrivateKey.set(keyPair.privateKey, publicKeyBytes);

			if (password != null && password.length > 0) {
				return Promise.all([
					encrypt(rsaPrivateKey, password),
					encrypt(falconPrivateKey, password),
					encrypt(superFalconPrivateKey, password)
				]).then(function (results) {
					sodiumUtil.memzero(superFalconPrivateKey);
					sodiumUtil.memzero(falconPrivateKey);
					sodiumUtil.memzero(rsaPrivateKey);

					return results;
				});
			}
			else {
				return [
					rsaPrivateKey,
					falconPrivateKey,
					superFalconPrivateKey
				];
			}
		}).then(function (results) {
			if (!results) {
				return {
					rsa: null,
					falcon: null,
					superFalcon: null
				};
			}

			var rsaPrivateKey			= results[0];
			var falconPrivateKey		= results[1];
			var superFalconPrivateKey	= results[2];

			var privateKeyData	= {
				rsa: sodiumUtil.to_base64(rsaPrivateKey),
				falcon: sodiumUtil.to_base64(falconPrivateKey),
				superFalcon: sodiumUtil.to_base64(superFalconPrivateKey)
			};

			sodiumUtil.memzero(superFalconPrivateKey);
			sodiumUtil.memzero(falconPrivateKey);
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
					falcon: sodiumUtil.to_base64(new Uint8Array(
						keyPair.publicKey.buffer,
						keyPair.publicKey.byteOffset + rsaSign.publicKeyBytes
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

				if (password != null && password.length > 0) {
					return Promise.all([decrypt(superFalconPrivateKey, password)]);
				}
				else {
					return [superFalconPrivateKey];
				}
			}
			else if (
				keyData.private &&
				typeof keyData.private.rsa === 'string' &&
				typeof keyData.private.falcon === 'string'
			) {
				var rsaPrivateKey		= sodiumUtil.from_base64(keyData.private.rsa);
				var falconPrivateKey	= sodiumUtil.from_base64(keyData.private.falcon);

				if (password == null || password.length < 1) {
					return [rsaPrivateKey, falconPrivateKey];
				}

				return Promise.all([
					decrypt(
						rsaPrivateKey,
						typeof password === 'string' ? password : password.rsa
					),
					decrypt(
						falconPrivateKey,
						typeof password === 'string' ? password : password.falcon
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
				var rsaPrivateKey		= results[0];
				var falconPrivateKey	= results[1];

				keyPair.publicKey.set(
					new Uint8Array(
						rsaPrivateKey.buffer,
						rsaPrivateKey.byteOffset,
						rsaSign.publicKeyBytes
					)
				);
				keyPair.publicKey.set(
					new Uint8Array(
						falconPrivateKey.buffer,
						falconPrivateKey.byteOffset,
						falconBytes.publicKeyBytes
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
						falconPrivateKey.buffer,
						falconPrivateKey.byteOffset + falconBytes.publicKeyBytes
					),
					rsaSign.privateKeyBytes
				);
			}

			return keyPair;
		}).then(function (keyPair) {
			if (!keyPair.privateKey) {
				if (keyData.public.superFalcon) {
					keyPair.publicKey.set(sodiumUtil.from_base64(keyData.public.superFalcon));
				}
				else if (keyData.public.rsa && keyData.public.falcon) {
					keyPair.publicKey.set(sodiumUtil.from_base64(keyData.public.rsa));
					keyPair.publicKey.set(
						sodiumUtil.from_base64(keyData.public.falcon),
						rsaSign.publicKeyBytes
					);
				}
			}

			return keyPair;
		});
	}
};



superFalcon.superFalcon	= superFalcon;
module.exports			= superFalcon;


/***/ }),
/* 1 */
/***/ (function(module) {

var __dirname = "/";
var falcon=function(){var A,I,g,B={};function Q(A,I){if(0===A)return I;throw new Error("Falcon error: "+A)}function C(A,I){return new Uint8Array(new Uint8Array(B.HEAPU8.buffer,A,I))}function E(A){try{B._free(A)}catch(A){setTimeout((function(){throw A}),0)}}B.ready=new Promise((function(A,I){function g(A){return g="function"==typeof Symbol&&"symbol"==typeof Symbol.iterator?function(A){return typeof A}:function(A){return A&&"function"==typeof Symbol&&A.constructor===Symbol&&A!==Symbol.prototype?"symbol":typeof A},g(A)}(D={}).onAbort=I,D.onRuntimeInitialized=function(){try{D._falconjs_public_key_bytes(),A(D)}catch(A){I(A)}};var B,Q,C,E,i,f,D=void 0!==D?D:{},o=Object.assign({},D),n=[],r=function(A,I){throw I},a="object"==("undefined"==typeof window?"undefined":g(window)),w="function"==typeof importScripts,e="object"==("undefined"==typeof process?"undefined":g(process))&&"object"==g(process.versions)&&"string"==typeof process.versions.node,F="";e?(F=w?eval("require")("path").dirname(F)+"/":__dirname+"/",f=function(){i||(E=eval("require")("fs"),i=eval("require")("path"))},B=function(A,I){var g=W(A);return g?I?g:g.toString():(f(),A=i.normalize(A),E.readFileSync(A,I?void 0:"utf8"))},C=function(A){var I=B(A,!0);return I.buffer||(I=new Uint8Array(I)),I},Q=function(A,I,g){var B=W(A);B&&I(B),f(),A=i.normalize(A),E.readFile(A,(function(A,B){A?g(A):I(B.buffer)}))},process.argv.length>1&&process.argv[1].replace(/\\/g,"/"),n=process.argv.slice(2), true&&(module.exports=D),process.on("uncaughtException",(function(A){if(!(A instanceof m))throw A})),process.on("unhandledRejection",(function(A){throw A})),r=function(A,I){if(J())throw process.exitCode=A,I;var g;(g=I)instanceof m||t("exiting due to exception: "+g),process.exit(A)},D.inspect=function(){return"[Emscripten Module object]"}):(a||w)&&(w?F=self.location.href:"undefined"!=typeof document&&document.currentScript&&(F=document.currentScript.src),F=0!==F.indexOf("blob:")?F.substr(0,F.replace(/[?#].*/,"").lastIndexOf("/")+1):"",B=function(A){try{var I=new XMLHttpRequest;return I.open("GET",A,!1),I.send(null),I.responseText}catch(I){var g=W(A);if(g)return function(A){for(var I=[],g=0;g<A.length;g++){var B=A[g];B>255&&(B&=255),I.push(String.fromCharCode(B))}return I.join("")}(g);throw I}},w&&(C=function(A){try{var I=new XMLHttpRequest;return I.open("GET",A,!1),I.responseType="arraybuffer",I.send(null),new Uint8Array(I.response)}catch(I){var g=W(A);if(g)return g;throw I}}),Q=function(A,I,g){var B=new XMLHttpRequest;B.open("GET",A,!0),B.responseType="arraybuffer",B.onload=function(){if(200==B.status||0==B.status&&B.response)I(B.response);else{var Q=W(A);Q?I(Q.buffer):g()}},B.onerror=g,B.send(null)});D.print||console.log.bind(console);var c,t=D.printErr||console.warn.bind(console);Object.assign(D,o),o=null,D.arguments&&(n=D.arguments),D.thisProgram&&D.thisProgram,D.quit&&(r=D.quit),D.wasmBinary&&(c=D.wasmBinary);var h,P=D.noExitRuntime||!0;"object"!=("undefined"==typeof WebAssembly?"undefined":g(WebAssembly))&&L("no native wasm support detected");var y=!1;var s,S,G,U,R="undefined"!=typeof TextDecoder?new TextDecoder("utf8"):void 0;function k(A,I){return A?function(A,I,g){for(var B=I+g,Q=I;A[Q]&&!(Q>=B);)++Q;if(Q-I>16&&A.buffer&&R)return R.decode(A.subarray(I,Q));for(var C="";I<Q;){var E=A[I++];if(128&E){var i=63&A[I++];if(192!=(224&E)){var f=63&A[I++];if((E=224==(240&E)?(15&E)<<12|i<<6|f:(7&E)<<18|i<<12|f<<6|63&A[I++])<65536)C+=String.fromCharCode(E);else{var D=E-65536;C+=String.fromCharCode(55296|D>>10,56320|1023&D)}}else C+=String.fromCharCode((31&E)<<6|i)}else C+=String.fromCharCode(E)}return C}(S,A,I):""}D.INITIAL_MEMORY;var N=[],H=[],u=[];function J(){return P}var d=0,v=null,Y=null;function L(A){throw D.onAbort&&D.onAbort(A),t(A="Aborted("+A+")"),y=!0,1,A+=". Build with -sASSERTIONS for more info.",new WebAssembly.RuntimeError(A)}var j,M,x="data:application/octet-stream;base64,";function b(A){return A.startsWith(x)}function q(A){return A.startsWith("file://")}function z(A){try{if(A==j&&c)return new Uint8Array(c);var I=W(A);if(I)return I;if(C)return C(A);throw"both async and sync fetching of the wasm failed"}catch(A){L(A)}}b(j="data:application/octet-stream;base64,AGFzbQEAAAABiwEUYAF/AGADf39/AGACf38AYAR/f39/AX9gAX8Bf2AEf39/fwBgAX4BfmADf39/AX9gAn5+AX5gBn9/f39/fwBgAn9/AX9gBX9/f39/AGAAAX9gAn9+AGAAAGACfn4Bf2AIf39/f39/f38AYAN/fn4Bf2ALf39/f39/f39/f38AYAl/f39/f35+fn4AAh8FAWEBYQAHAWEBYgAEAWEBYwABAWEBZAAAAWEBZQAFAz8+CAgGCAcCCQcGCQEJAgQGAQEPAAEKAAsLAAUBDQACAAMDABAABAYFAQ0OAAQKERIDEwsCBQACAQMDCgwMDA4EBQFwAQEBBQYBAYACgAIGCQF/AUGw+4EECwcxDAFmAgABZwAuAWgBAAFpADABagAvAWsAQgFsAEEBbQBAAW4APwFvAD4BcAA9AXEAPAqI2wM+qQICBH4EfyAAIAGFQoCAgICAgICAgH+DIAFCGYhC////P4NCgICAwACEIgIgAEIZiEL///8/g0KAgIDAAIQiA34gAiAAQv///w+DIgR+IgJCGYinIAMgAUL///8PgyIFfiIDQhmIp2ogBCAFfiIEQhmIpyADp0H///8PcWogAqdB////D3FqIgZBGXZqrXwiAkI3iCIDpyAAQjSIp0H/D3EiByABQjSIp0H/D3EiCGpqQYAIayIJQR92rUIBfUIAIAhB/w9qIAdB/w9qcUELdq19g0IAIAN9IAIgBiAEp3JB////D3FB////D2pBGXathCIAQgGDIAJCAYiEIACFgyAAhYMiAEICiIQgCUEAIABCNoina3GtQjSGfEHIASAAp0EHcXZBAXGtfAv7AwICfgd/IABCACAAQv///////////wCDIAFC////////////AIN9IgJCAFetIABCP4iDIAJCP4iEfSAAIAGFgyIAhSICQjSIpyIGQf8PcSIEQf8PakELdq1CN4YgAkIDhkL4////////P4OEIAQgACABhSIAQjSIpyIHQf8PcSIIayIFQRp0QR91rCIBQoGAgIAQg0IBhSAFQR9xrSIChkIBfSIDQgAgBUE8a0Efdq19IABCA4ZC+P///////z+DIAhB/w9qQQt2rUI3hoSDIgCDIAN8IACEIgBCIIggAIUgAYMgAIUgAogiAEIAIAZBC3YiBSAHQQt2c619IABCAYaDfXwiACAAQiCGIABC/////w9WIgYbIgAgAEIQhiAAQv///////z9WIgcbIgAgAEIIhiAAQv//////////AFYiCBsiACAAQgSGIABC//////////8PViIJGyIAIABCAoYgAEL//////////z9WIgobIgBCAYYgAIUgAEI/iCIBQgF9gyAAhSIAQv8Dg0L/A3wgAIRCCYggAacgBCAGQQV0aiAHQQR0aiAIQQN0aiAJQQJ0aiAKQQF0ampBOGsiBEEfdq1C/////////z98gyIAQgKIIAWtQj+GhCAEQQAgAEI2iKdrca1CNIZ8QcgBIACnQQdxdkEBca18C40CAgJ+Bn8gAEKAgICAgICAgIB/g0IAIABCP4cgAIUgAEI/iHwiACAAQiCGIABC/////w9WIgMbIgEgAUIQhiABQv///////z9WIgQbIgEgAUIIhiABQv//////////AFYiBRsiASABQgSGIAFC//////////8PViIGGyIBIAFCAoYgAUL//////////z9WIgcbIgFCAYYgAYUgAUI/iCICQgF9gyABhSIBQv8Dg0L/A3wgAYRCCYggAFAiCBsiAEICiIRCACAAQjaIfUK0CCAErUIEhkKeCEL+ByADG3wgBa1CA4Z8IAatQgKGfCAHrUIBhnwgAoQgCBuDQjSGfEHIASAAp0EHcXZBAXGtfAsUACAAIAFCgICAgICAgICAf4UQBguABAEDfyACQYAETwRAIAAgASACEAIgAA8LIAAgAmohAwJAIAAgAXNBA3FFBEACQCAAQQNxRQRAIAAhAgwBCyACRQRAIAAhAgwBCyAAIQIDQCACIAEtAAA6AAAgAUEBaiEBIAJBAWoiAkEDcUUNASACIANJDQALCwJAIANBfHEiBEHAAEkNACACIARBQGoiBUsNAANAIAIgASgCADYCACACIAEoAgQ2AgQgAiABKAIINgIIIAIgASgCDDYCDCACIAEoAhA2AhAgAiABKAIUNgIUIAIgASgCGDYCGCACIAEoAhw2AhwgAiABKAIgNgIgIAIgASgCJDYCJCACIAEoAig2AiggAiABKAIsNgIsIAIgASgCMDYCMCACIAEoAjQ2AjQgAiABKAI4NgI4IAIgASgCPDYCPCABQUBrIQEgAkFAayICIAVNDQALCyACIARPDQEDQCACIAEoAgA2AgAgAUEEaiEBIAJBBGoiAiAESQ0ACwwBCyADQQRJBEAgACECDAELIAAgA0EEayIESwRAIAAhAgwBCyAAIQIDQCACIAEtAAA6AAAgAiABLQABOgABIAIgAS0AAjoAAiACIAEtAAM6AAMgAUEEaiEBIAJBBGoiAiAETQ0ACwsgAiADSQRAA0AgAiABLQAAOgAAIAFBAWohASACQQFqIgIgA0cNAAsLIAAL2wICDn8IfkECIQQgAUECTwRAQQEhCkEBIAF0QQF2IgshBQNAIAVBAXYhBiAEBEAgBEEBdiICQQEgAkEBSxshDEEAIQcgBiEIQQAhAgNAIAIgBmogAksEQCAEIAdqQQR0QeAKaiIDKQMAIRQgAykDCCEVIAIhAwNAIAAgAyALakEDdGoiDSkDACERIAAgA0EDdGoiDikDACESIAAgAyAGaiIJIAtqQQN0aiIPKQMAIRAgACAJQQN0aiIJKQMAIhMgFBAFIBAgFRAFEAghFiATIBUQBSAQIBQQBRAGIRAgEiAWEAYhEyARIBAQBiEXIA4gEzcDACANIBc3AwAgEiAWEAghEiARIBAQCCERIAkgEjcDACAPIBE3AwAgA0EBaiIDIAhHDQALCyAFIAhqIQggAiAFaiECIAdBAWoiByAMRw0ACwsgBEEBdCEEIAYhBSAKQQFqIgogAUcNAAsLC8MDAg5/BH4gAwRAIAStIRYgBa0hF0EBIQlBASADdCIQIQgDQCAIIg5BAk8EQCAJIgpBAXQhCSAOQQF2IQggCgRAIAEgCmwhESAIQQEgCEEBSxshEkEAIQtBACEMA0AgACABIAtsQQJ0aiIFIBFBAnRqIQcgAiAIIAxqQQJ0ajUCACEVQQAhDwNAIAUgBygCACIGIAUoAgAiE2oiDSANIARrIg0gDUEASBs2AgAgByATIAZrIgZBH3UgBHEgBmqtIBV+IhQgF35C/////weDIBZ+IBR8Qh+IpyIGIAYgBGsiBiAGQQBIGzYCACAHIAFBAnQiBmohByAFIAZqIQUgD0EBaiIPIApHDQALIAkgC2ohCyAMQQFqIgwgEkcNAAsLIA5BBE8NAQsLIBBBfnEhAkGAgICAeCADdq0hFUEAIQcDQCAAIAA1AgAgFX4iFCAXfkL/////B4MgFn4gFHxCH4inIgMgAyAEayIDIANBAEgbNgIAIAAgAUECdCIDaiIAIAA1AgAgFX4iFCAXfkL/////B4MgFn4gFHxCH4inIgUgBSAEayIFIAVBAEgbNgIAIAAgA2ohACAHQQJqIgcgAkcNAAsLC+gCAQJ/AkAgACABRg0AIAEgACACaiIEa0EAIAJBAXRrTQRAIAAgASACEAkPCyAAIAFzQQNxIQMCQAJAIAAgAUkEQCADBEAgACEDDAMLIABBA3FFBEAgACEDDAILIAAhAwNAIAJFDQQgAyABLQAAOgAAIAFBAWohASACQQFrIQIgA0EBaiIDQQNxDQALDAELAkAgAw0AIARBA3EEQANAIAJFDQUgACACQQFrIgJqIgMgASACai0AADoAACADQQNxDQALCyACQQNNDQADQCAAIAJBBGsiAmogASACaigCADYCACACQQNLDQALCyACRQ0CA0AgACACQQFrIgJqIAEgAmotAAA6AAAgAg0ACwwCCyACQQNNDQADQCADIAEoAgA2AgAgAUEEaiEBIANBBGohAyACQQRrIgJBA0sNAAsLIAJFDQADQCADIAEtAAA6AAAgA0EBaiEDIAFBAWohASACQQFrIgINAAsLIAAL4AECA34DfyAAQhmIQv///z+DQoCAgMAAhCIBIAF+IAEgAEL///8PgyICfiIBp0EBdEH+//8fcSACIAJ+IgJCGYinaiIEQRl2IAFCGIinQf7///8BcWqtfCIBQjeIIgOnIABCNIinQf8PcSIFQQF0akGACGsiBkEAIAZBH3atQgF9QgAgBUH/D2pBC3atfYNCACADfSABIAQgAqdyQf///w9xQf///w9qQRl2rYQiAEIBgyABQgGIhCAAhYMgAIWDIgBCNoina3GtQjSGIABCAoh8QcgBIACnQQdxdkEBca18C/8GAgV+A39BACAErSIIQQAgBEEBdGsgBEF9bCILIAtBAEgbrSIHIAd+IgYgBa0iB35C/////weDfiAGfEIfiKciBSAFIARrIgUgBUEASButIgYgBn4iBiAHfkL/////B4MgCH4gBnxCH4inIgUgBSAEayIFIAVBAEgbrSIGIAZ+IgYgB35C/////weDIAh+IAZ8Qh+IpyIFIAUgBGsiBSAFQQBIG60iBiAGfiIGIAd+Qv////8HgyAIfiAGfEIfiKciBSAFIARrIgUgBUEASButIgYgBn4iBiAHfkL/////B4MgCH4gBnxCH4inIgUgBSAEayIFIAVBAEgbIgVBAXFrIARxIAVqQQF2rSIKIAOtfiIGIAd+Qv////8HgyAIfiAGfEIfiKciAyADIARrIgMgA0EASBshBQJAIAJBCUsNACACQQFxBH8gBa0iBiAGfiIGIAd+Qv////8HgyAIfiAGfEIfiKciAyADIARrIgMgA0EASBshBSACQQFqBSACCyEDIAJBCUYNAANAIAWtIgYgBn4iBiAHfkL/////B4MgCH4gBnxCH4inIgUgBSAEayIFIAVBAEgbrSIGIAZ+IgYgB35C/////weDIAh+IAZ8Qh+IpyIFIAUgBGsiBSAFQQBIGyEFIANBAmoiA0EKRw0ACwsgBEECayENIAWtIQZBHiELQYCAgIB4IARrIgUhAwNAQQAgDSALIgx2QQFxayADrSIJIAl+IgkgB35C/////weDIAh+IAl8Qh+IpyIDIAMgBGsiAyADQQBIGyIDrSAGfiIJIAd+Qv////8HgyAIfiAJfEIfiKciCyALIARrIgsgC0EASBsgA3NxIANzIQMgDEEBayELIAwNAAsgCiADrSIKIAd+Qv////8HgyAIfiAKfEIfiKciAyADIARrIgMgA0EASButfiIKIAd+Qv////8HgyAIfiAKfEIfiKciAyADIARrIgMgA0EASButIQpBCiACayEMQQAhAyAFIQsDQCAAIAMgDHRBAXRB4L8Bai8BAEECdCINaiALNgIAIAEgDWogBTYCACAFrSAKfiIJIAd+Qv////8HgyAIfiAJfEIfiKciBSAFIARrIgUgBUEASBshBSALrSAGfiIJIAd+Qv////8HgyAIfiAJfEIfiKciCyALIARrIgsgC0EASBshCyADQQFqIgMgAnZFDQALC5UBAgV+BX8gAgRAQQEgAnRBAXYiCUEBIAlBAUsbIQpBACECA0AgASACIAlqQQN0IghqKQMAIQMgACAIaiIIKQMAIQQgACACQQN0IgtqIgwpAwAiBSABIAtqKQMAIgYQBSAEIAMQBRAIIQcgBSADEAUgBCAGEAUQBiEDIAwgBzcDACAIIAM3AwAgAkEBaiICIApHDQALCwujCwIMfwZ+IAVBgbD//wc2AgAgAUECTwRAQQEhCgNAIApBDGwiCEGgjQFqKAIAIgmtIRMgAwRAQQAhCyAJQQBBACAJQQF0ayAJQX1sIgYgBkEASButIhIgEn4iEkECQQJBAkECIAlrIgYgCWxrIAZsIgYgCWxrIAZsIgYgCWxrIAZsIgYgCWxB/v///wdqIAZsQf////8Hca0iFH5C/////weDIBN+IBJ8Qh+IpyIGIAYgCWsiBiAGQQBIG60iEiASfiISIBR+Qv////8HgyATfiASfEIfiKciBiAGIAlrIgYgBkEASButIhIgEn4iEiAUfkL/////B4MgE34gEnxCH4inIgYgBiAJayIGIAZBAEgbrSISIBJ+IhIgFH5C/////weDIBN+IBJ8Qh+IpyIGIAYgCWsiBiAGQQBIG60iEiASfiISIBR+Qv////8HgyATfiASfEIfiKciBiAGIAlrIgYgBkEASBsiBkEBcWtxIAZqQQF2rSEWIApBfnEhDiAKQQFxIRAgCEGojQFqNQIAIRcgACEGA0AgBiAKQQJ0aiIRKAIAIQxBACEHIAohCANAIAetIBZ+IhIgFH5C/////weDIBN+IBJ8Qh+IpyIHIAcgCWsiByAHQQBIGyAGIAhBAWsiCEECdGooAgAiByAHIAlrIgcgB0EASBtqIgcgByAJayIHIAdBAEgbIQcgCA0AC0EAIQggDCAHayIHQR91IAlxIAdqrSAXfiISIBR+Qv////8HgyATfiASfEIfiKciByAHIAlrIgcgB0EASButIRVCACESQQAhDCANBEADQCAGIAhBAnQiB2oiDyAPNQIAIBJC/////w+DfCAFIAdqNQIAIBV+fCISp0H/////B3E2AgAgBiAHQQRyIgdqIg8gDzUCACASQh+IQv////8Pg3wgBSAHajUCACAVfnwiEqdB/////wdxNgIAIBJCH4ghEiAIQQJqIQggDEECaiIMIA5HDQALCyARIBAEfiAGIAhBAnQiCGoiByAHNQIAIBJC/////w+DfCAFIAhqNQIAIBV+fCISp0H/////B3E2AgAgEkIfiAUgEgs+AgAgBiACQQJ0aiEGIAtBAWoiCyADRw0ACwsgCkEBcSEGAkAgDUUEQEIAIRJBACEJDAELIApBfnEhB0IAIRJBACEJQQAhCANAIAUgCUECdCILaiIMIAw1AgAgE34gEkL/////D4N8IhKnQf////8HcTYCACAFIAtBBHJqIgsgCzUCACATfiASQh+IQv////8Pg3wiEqdB/////wdxNgIAIBJCH4ghEiAJQQJqIQkgCEECaiIIIAdHDQALCyAFIApBAnRqIAYEfiAFIAlBAnRq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rTcDyAEgACABaiEBIAQgAGsiBA0ACwsL2AIBAn8CQCABRQ0AIABBADoAACAAIAFqIgJBAWtBADoAACABQQNJDQAgAEEAOgACIABBADoAASACQQNrQQA6AAAgAkECa0EAOgAAIAFBB0kNACAAQQA6AAMgAkEEa0EAOgAAIAFBCUkNACAAQQAgAGtBA3EiA2oiAkEANgIAIAIgASADa0F8cSIDaiIBQQRrQQA2AgAgA0EJSQ0AIAJBADYCCCACQQA2AgQgAUEIa0EANgIAIAFBDGtBADYCACADQRlJDQAgAkEANgIYIAJBADYCFCACQQA2AhAgAkEANgIMIAFBEGtBADYCACABQRRrQQA2AgAgAUEYa0EANgIAIAFBHGtBADYCACADIAJBBHFBGHIiA2siAUEgSQ0AIAIgA2ohAgNAIAJCADcDGCACQgA3AxAgAkIANwMIIAJCADcDACACQSBqIQIgAUEgayIBQR9LDQALCyAAC00CAX4BfyAAKAIAIgAgACkDyAEiAadBeHFqIgIgAikDAEIfIAFCA4aGhTcDACAAQgA3A8gBIAAgACkDgAFCgICAgICAgICAf4U3A4ABC+ocAgV+JX8gACACQQJ0IhJBpL4BaigCACIZQQEgAXQiFUEBdiIkbEECdCICaiInIAJqIAAgEkGgvgFqKAIAIhJBCCABdGwQDCIoIBIgAXRBAnQiAmoiKSACaiIaIBVBAnQiAmoiGyACaiETICRBASAkQQFLGyErIBJBASASQQFLGyEfIBVBfHEhLCAVQQNxIS0gFUEBayEgIANFIAFBAWsiLiABSXEhDQNAIBogGyABICVBDGwiAkGkjQFqKAIAIAJBoI0BaigCACIPIA9BAiAPQQIgD0ECIA9BAiAPayICbGsgAmwiAmxrIAJsIgJsayACbCICbEH+////B2ogAmxB/////wdxIiEQDkEAIA+tIgZBACAPQQF0ayAPQX1sIgIgAkEASButIgUgBX4iBSAhrSIHfkL/////B4N+IAV8Qh+IpyICIAIgD2siAiACQQBIG60iBSAFfiIFIAd+Qv////8HgyAGfiAFfEIfiKciAiACIA9rIgIgAkEASButIgUgBX4iBSAHfkL/////B4MgBn4gBXxCH4inIgIgAiAPayICIAJBAEgbrSIFIAV+IgUgB35C/////weDIAZ+IAV8Qh+I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QYHgAGsgAUGAgISAA0kbIgEgAWwiAUH/3wBsQf//A3FBgeAAbCABaiIBQRB2IgIgAkGB4ABrIAFBgICEgANJGyAAbCIAQf/fAGxB//8DcUGB4ABsIABqIgBBEHYiASABQYHgAGsgAEGAgISAA0kbbCIAQf/fAGxB//8DcUGB4ABsIABqIgBBEHYiASABQYHgAGsgAEGAgISAA0kbC7YQAgh+Bn8gASABQj+HIgZBPSABQjSIp2siC0EadEEfdawgBiABQgqGQoD4////////P4NCgICAgICAgIDAAISFIAFCP4h8IgZCIIcgBoWDIAaFIAtBH3GthyABQoCAgICAgIDw/wCDQoCAgICAgIDwP1QbIghCIIZCIIcQBxAIIQkgAhANQoCAgICAgIAIfSIBQjSIQv8Pg0IBfEILiEIBfSABgyEKIAIgACkDkAYQBSEGA0AgACgCgAQiC0H3A08EQCAAEChBACELCyAAIAtBCGo2AoAEIAAgC2opAAAiAUIwiKcgABApQRB0ciEPIAGnQf///wdxIRAgAUIYiKdB////B3EhDEEAIQ5BACENA0AgDSAPIA5BAnQiC0HgzwFqKAIAayAMIAtB5M8BaigCAGsgECALQejPAWooAgBrQR91akEfdWpBH3ZqIQ0gDkEzSSELIA5BA2ohDiALDQALQoCAgICAgICAgH9CgJD9////////AEKA6IqAgICAgMAAQoD+h6zVqtWqFUKAnpyo1arVqgVC0N+Z8JCixIgBQuqf7IKD2+AWQuOt3ueBjaADQoTfvazPgTRCxcD3nNnjBUKKqPzl/ckAQob4s6TlBiAAEClBAXEiC0EBdEEBayANbCALaiIPrBAHIAkQCBANIAoQBSANIA1srRAHQsL3joy88dPhPxAFEAgiASABQv6Frqn2qMX7PxAFIgJCP4dBPSACQjSIpyIMayILQRp0QR91rCACQgqGQoD4////////P4NCgICAgICAgIDAAIQiAUIgiCABhYMgAYUgC0Efca2IQgAgDEH+D3FB/QdLG4UgAkI/iHwiB0IghkIghxAHQu/z6PevyIvzPxAFEAgiAkI0iKdB/w9xIgtBPmpBAEIAIAtB/w9qQQt2rX0gAkIChkL8////////H4NCgICAgICAgCCEgyIBQjaIp2txrUI0hiACQoCAgICAgICAgH+DIAFCAoiEfCICQgqGQoD4////////P4NCgICAgICAgIDAAIQiAUIgiCABhUE9IAJCNIinIgxrIgtBGnRBH3WsgyABhSALQR9xrYhCACAMQf4PcUH9B0sbIAJCP4eFIAJCP4h8QgGGIgFCIIgiBEKjh8agB34iAkIgiCAEQgKGfCABQv7///8PgyIFQqOHxqAHfkIgiCAFQgKGfCIBQiCIfCABQv////8PgyACQv////8Pg3xCIIh8fSIBQv////8PgyIDIAR+IgJCIIggAUIgiCIBIAR+fCABIAV+IAMgBX5CIIh8IgFCIIh8IAFC/////w+DIAJC/////w+DfEIgiHx9IgFC/////w+DIgMgBH4iAkIgiCABQiCIIgEgBH58IAEgBX4gAyAFfkIgiHwiAUIgiHwgAUL/////D4MgAkL/////D4N8QiCIfH0iAUL/////D4MiAyAEfiICQiCIIAFCIIgiASAEfnwgASAFfiADIAV+QiCIfCIBQiCIfCABQv////8PgyACQv////8Pg3xCIIh8fSIBQv////8PgyIDIAR+IgJCIIggAUIgiCIBIAR+fCABIAV+IAMgBX5CIIh8IgFCIIh8IAFC/////w+DIAJC/////w+DfEIgiHx9IgFC/////w+DIgMgBH4iAkIgiCABQiCIIgEgBH58IAEgBX4gAyAFfkIgiHwiAUIgiHwgAUL/////D4MgAkL/////D4N8QiCIfH0iAUL/////D4MiAyAEfiICQiCIIAFCIIgiASAEfnwgASAFfiADIAV+QiCIfCIBQiCIfCABQv////8PgyACQv////8Pg3xCIIh8fSIBQv////8PgyIDIAR+IgJCIIggAUIgiCIBIAR+fCABIAV+IAMgBX5CIIh8IgFCIIh8IAFC/////w+DIAJC/////w+DfEIgiHx9IgFC/////w+DIgMgBH4iAkIgiCABQiCIIgEgBH58IAEgBX4gAyAFfkIgiHwiAUIgiHwgAUL/////D4MgAkL/////D4N8QiCIfH0iAUL/////D4MiAyAEfiICQiCIIAFCIIgiASAEfnwgASAFfiADIAV+QiCIfCIBQiCIfCABQv////8PgyACQv////8Pg3xCIIh8fSIBQv////8PgyIDIAR+IgJCIIggAUIgiCIBIAR+fCABIAV+IAMgBX5CIIh8IgFCIIh8IAFC/////w+DIAJC/////w+DfEIgiHx9IgFC/////w+DIgMgBH4iAkIgiCAEIAFCIIgiAX58IAEgBX4gAyAFfkIgiHwiAUIgiHwgAUL/////D4MgAkL/////D4N8QiCIfH0iAkL/////D4MiBCAGQjSIp0H/D3EiC0E+akEAQgAgC0H/D2pBC3atfSAGQgKGQvz///////8fg0KAgICAgICAIISDIgFCNoina3GtQjSGIAZCgICAgICAgICAf4MgAUICiIR8IgNCCoZCgPj///////8/g0KAgICAgICAgMAAhCIBQiCIIAGFQT0gA0I0iKciDGsiC0EadEEfdayDIAGFIAtBH3GtiEIAIAxB/g9xQf0HSxsgA0I/h4UgA0I/iHxCAYYiBUIgiCIBfiIDQiCIIAEgAkIgiCICfnwgBCAFQv7///8PgyIBfkIgiCABIAJ+fCIBQiCIfCABQv////8PgyADQv////8Pg3xCIIh8QgGGQgF9IAdC/////w+DQj8gB6dBQGpBAEgbiCEDQsAAIQEDQCAAECkiECADIAFCCH0iAoinQf8BcSIMRgRAIAFCCFYhCyACIQEgCw0BCwsgECAMa0EATg0ACyAPIAinaguZEAIbfwV+IApBASAJdCITQQJ0IgxqIh8gDGoiFSAEQQFqIhEgCXRBAnRqIRYgEUUEQCAVIBEgESATQQEgFhAQDwsgE0F4cSEiIBNBB3EhIyATQX5xISQgE0EBayEgIARBAWshGyAFQQJ0ISUDQEEBIQVBgICAgHggGkEMbCIPQaCNAWooAgAiDWshDCANQQAgDa0iKEEAIA1BAXRrIA1BfWwiCyALQQBIG60iJyAnfiImIA1BAiANQQIgDUECIA1BAiANayILbGsgC2wiC2xrIAtsIgtsayALbCILbEH+////B2ogC2xB/////wdxIiGtIid+Qv////8Hg34gJnxCH4inIgsgCyANayILIAtBAEgbrSImICZ+IiYgJ35C/////weDICh+ICZ8Qh+IpyILIAsgDWsiCyALQQBIG60iJiAmfiImICd+Qv////8HgyAofiAmfEIfiKciCyALIA1rIgsgC0EASButIiYgJn4iJiAnfkL/////B4MgKH4gJnxCH4inIgsgCyANayILIAtBAEgbrSImICZ+IiYgJ35C/////weDICh+ICZ8Qh+IpyILIAsgDWsi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GyEPICVBASAlQQFLGyEeQQIgDHQhLSAHQQFrIR0DQEGAgICAeCAaQQxsQaCNAWooAgAiDmshDUEBIQQgDkEAIA6tIjJBACAOQQF0ayAOQX1sIgAgAEEASButIi4gLn4iLiAOQQIgDkECIA5BAiAOQQIgDmsiAGxrIABsIgBsayAAbCIAbGsgAGwiAGxB/v///wdqIABsQf////8Hca0iM35C/////weDfiAufEIfiKciACAAIA5rIgAgAEEASButIi4gLn4iLiAzfkL/////B4MgMn4gLnxCH4inIgAgACAOayIAIABBAEgbrSIuIC5+Ii4gM35C/////weDIDJ+IC58Qh+IpyIAIAAgDmsiACAAQQBIG60iLiAufiIuIDN+Qv////8HgyAyfiAufEIfiKciACAAIA5rIgAgAEEASButIi4gLn4iLiAzfkL/////B4MgMn4gLnxCH4inIgAgACAOayIAIABBAEgbIgBBAXFrcSAAakEBdiIFIQBBACEBIB0EQANAAkAgBCAdcUUEQCAArSEvDAELIACtIi8gDa1+Ii4gM35C/////weDIDJ+IC58Qh+IpyIAIAAgDmsiACAAQQBIGyENCyAvIC9+Ii4gM35C/////weDIDJ+IC58Qh+IpyIAIAAgDmsiACAAQQBIGyEAQQIgAXQhBCABQQFqIQEgBCAdTQ0ACwsgAkEKRiIfRQRAIAMgGkECdCIAaiEQIAAgGGohFiAFrSEvQQAhCCAVIQUgEyEGA0BBACEBIAchBANAIAGtIC9+Ii4gM35C/////weDIDJ+IC58Qh+IpyIAIAAgDmsiACAAQQBIGyAFIARBAWsiBEECdGooAgAiACAAIA5rIgAgAEEASBtqIgAgACAOayIAIABBAEgbIQEgBA0AC0EAIQAgECABIA1BACAFIB1BAnQiEmooAgBBHnZrcWsiAUEfdSAOcSABajYCACAHIQQDQCAArSAvfiIuIDN+Qv////8HgyAyfiAufEIfiKciACAAIA5rIgAgAEEASBsgBiAEQQFrIgRBAnRqKAIAIgAgACAOayIAIABBAEgbaiIAIAAgDmsiACAAQQBIGyEAIAQNAAsgFiAAIA1BACAGIBJqKAIAQR52a3FrIgBBH3UgDnEgAGo2AgAgFiARQQJ0IgBqIRYgACAQaiEQIAYgB0ECdCIAaiEGIAAgBWohBSAIQQFqIgggHkcNAAsLIBpBAWoiGiAPRw0ACyAVIBRBAnQiAGoiFyAAaiIjIABqIiQgAGoiGSAlQQJ0aiEbIBFBASARQQFLGyEnIBRBfnEhKSALQQFrIShBASAMQQFrIip0IRMgJUEBICVBAUsbIiBB/v///wdxISsgIEEBcSEsIBFBA3QhIQNAICJBDGwiAUGgjQFqKAIAIglBACAJrSIwQQAgCUEBdGsgCUF9bCIAIABBAEgbrSIuIC5+Ii4gCUECIAlBAiAJQQIgCUECIAlrIgBsayAAbCIAbGsgAGwiAGxrIABsIgBsQf7///8HaiAAbEH/////B3EiHK0iMX5C/////weDfiAufEIfiKciACAAIAlrIgAgAEEASButIi4gLn4iLiAxfkL/////B4MgMH4gLnxCH4inIgAgACAJayIAIABBAEgbrSIuIC5+Ii4gMX5C/////weDIDB+IC58Qh+IpyIAIAAgCWsiACAAQQBIG60iLiAufiIuIDF+Qv////8HgyAwfiAufEIfiKciACAAIAlrIgAgAEEASButIi4gLn4iLiAxfkL/////B4MgMH4gLnxCH4inIgAgACAJayIAIABBAEgbIgBBAXFrcSAAaiEAIAsgIkYEQCAmIAsgCyAUQQEgFRAQIAogCyALIBRBASAVEBALIABBAXYhHiAVIBcgDCABQaSNAWooAgAgCSAcEA4CQAJAIAsgIksEQEEAIQEgJiAiQQJ0IgVqIhIhACAFIApqIgghBUEAIQYgH0UEQANAICMgAUECdCIEaiAAKAIANgIAIAQgJGogBSgCADYCACAjIARBBHIiB2ogACALQQJ0Ig9qIgQoAgA2AgAgByAkaiAFIA9qIgAoAgA2AgAgACAPaiEFIAQgD2ohACABQQJqIQEgBkECaiIGIClHDQALCyACQQpGBEAgIyABQQJ0IgFqIAAoAgA2AgAgASAkaiAFKAIANgIACyASIAsgFyAMIAkgHBALIAggCyAXIAwgCSAcEAsMAQtBgICAgHggCWshCCAoBEBBACEEQQEhASAeIQADQAJAIAEgKHFFBEAgAK0hLwwBCyAArSIvIAitfiIuIDF+Qv////8HgyAwfiAufEIfiKciACAAIAlrIgAgAEEASBshCAsgLyAvfiIuIDF+Qv////8HgyAwfiAufEIfiKciACAAIAlrIgAg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aiIKIAxHDQALCyAFpyEKIA0EQCAAIARBAnQiBGooAgAgC2ogAiAEaigCAGshCQtBACEEQQAgCmtBAXYhC0EAIAlBf3NBH3YgCnJrIQkgDgRAIANBfnEhD0EAIQ0DQCAAIARBAnQiDGoiECAQKAIAIAogAiAMaigCACALcyAJcWprIgpB/////wdxNgIAIAAgDEEEciIMaiIQIBAoAgAgAiAMaigCACALcyAJcSAKQR92amsiCkH/////B3E2AgAgCkEfdiEKIARBAmohBCANQQJqIg0gD0cNAAsLIANBAXEEQCAAIARBAnQiBGoiACAAKAIAIAogAiAEaigCACALcyAJcWprQf////8HcTYCAAsgA0EBcSELIBJCP4ghBQJAIA5FBEBBACEAQQAhCgwBCyADQX5xIQ1BACEAQQAhCkEAIQkDQCABIABBAnQiBEEEciIMaigCACABIARqKAIAIApqIAIgBGooAgBrQR91aiACIAxqKAIAayIEQR91IQogAEECaiEAIAlBAmoiCSANRw0ACwsgBachCSALBEAgASAAQQJ0IgBqKAIAIApqIAAgAmooAgBrIQQLQQAhAEEAIAlrQQF2IQpBACAEQX9zQR92IAlyayEEIA4EQCADQX5xIQ1BACELA0AgASAAQQJ0Ig5qIgwgDCgCACAJIAIgDmooAgAgCnMgBHFqayIJQf////8HcTYCACABIA5BBHIiDmoiDCAMKAIAIAIgDmooAgAgCnMgBHEgCUEfdmprIglB/////wdxNgIAIAlBH3YhCSAAQQJqIQAgC0ECaiILIA1HDQALCyADQQFxRQ0AIAEgAEECdCIAaiIBIAEoAgAgCSAAIAJqKAIAIApzIARxamtB/////wdxNgIACwuiBgIKfwJ+IABBgCBqIQwDQCAAIAVBAnQiBmogASAFaiwAACIHQR91QYGw//8HcSAHajYCACAGIAxqIAIgBWosAAAiBkEfdUGBsP//B3EgBmo2AgAgBUEBaiIFQYAIRw0ACwJAAkAgAw0AIARFDQAgDEGAIGoiDSANQYAgakEKQcXa2rYBQYGw//8HQf+v/80HEA5BASEBQYAIIQIDQCACIgdBAXYhAgJAIAFFDQAgB0ECSQ0AIAJBASACQQFLGyEIQQAhBEEAIQMDQCAAIARBAnRqIgUgAkECdGohBiANIAEgA2pBAnRq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EAUaIEYgCEEDdGogUjcDACAEQQhqIQQgCEEBaiIIQYAERw0ACyAHICBBgCAQDCECIEEgPEGAwAAQDCELQQAhBCACIQVBACEIA0BCAEEAIAUoAgAiBkEedmsiA0EBdiAGcyADQQFxaiIGQf////8HcSAGQQF0IANxa6wQB0KAgICAgICA+D8QBRAGIVFCgICAgICAgPg/QoCAgICAgIDwwQAQBRogQyAIQQN0aiBRNwMAIAVBBGohBSAIQQFqIghBgARHDQALICYhBQNAQgBBACAFKAIAIgZBHnZrIgNBAXYgBnMgA0EBcWoiBkH/////B3EgBkEBdCADcWusEAdCgICAgICAgPg/EAUQBiFRQoCAgICAgID4P0KAgICAgICA8MEAEAUaIEQgBEEDdGogUTcDACAFQQRqIQUgBEEBaiIEQYAERw0ACyACIAtBgIABEAwiBkEJEAogFUEJEAogIEEJEAogKUEJEApBACECA0AgFSACQYACakEDdCIDaikDACFRICkgAkEDdCIEaikDACFSIAQgFWopAwAhUyADIClqKQMAIVUgAyAGaikDACFUIAMgIGopAwBCgICAgICAgICAf4UhViAEIAZqKQMAIlcgBCAgaikDACJYEAUgVCBWEAUQCCFZIFcgVhAFIFQgWBAFEAYhVCBTIFIQBSBRIFVCgICAgICAgICAf4UiVhAFEAghVSBTIFYQBSBRIFIQBRAGIVEgBCAkaiBZIFUQBjcDACADICRqIFQgURAGNwMAIAJBAWoiAkGAAkcNAAsgPiAgIClBCRArICQgPkEJEB8gJEEJEBFBACEEA0AgJCAEQQN0aiICKQMAIlFCgICAgICAgPDDABAWRQ0CQoCAgICAgIDwQyBREBZFDQIgAiBREBMQBzcDACAEQQFqIgRBgARHDQALICRBCRAKICAgJEEJEA8gKSAkQQkQDyAGICBBCRAsIBUgKUEJECwgBkEJEBEgFUEJEBEgCyAGQYDAABAMIQJBACEEA0AgBiAEQQJ0IgNqIAIgBEEDdCIFaikDABATPgIAIAMgJmogBSBCaikDABATPgIAIARBAWoiBEGABEcNAAsgLiA4QQpBxdratgFBgbD//wdB/6//zQcQDkEAIQQDQCAGIARBAnQiAmoiAyADKAIAIgNBAXRBgICAgHhxIANyIgNBH3VBgbD//wdxIANq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gACAAoABgANAAEACQAFAA8AAwALAAcADIAAgAiABIAOgAKACoAGgA2AAYAJgAWAD4ADgAuAB4AMQABACEAEQA5AAkAKQAZADUABQAlABUAPQANAC0AHQAzAAMAIwATADsACwArABsANwAHACcAFwA/AA8ALwAfADCAAIAggBCAOIAIgCiAGIA0gASAJIAUgDyADIAsgByAMoACgCKAEoA6gAqAKoAagDaABoAmgBaAPoAOgC6AHoAxgAGAIYARgDmACYApgBmANYAFgCWAFYA9gA2ALYAdgDOAA4AjgBOAO4ALgCuAG4A3gAeAJ4AXgD+AD4AvgB+AMEAAQCBAEEA4QAhAKEAYQDRABEAkQBRAPEAMQCxAHEAyQAJAIkASQDpACkAqQBpANkAGQCZAFkA+QA5ALkAeQDFAAUAhQBFAOUAJQClAGUA1QAVAJUAVQD1ADUAtQB1AM0ADQCNAE0A7QAtAK0AbQDdAB0AnQBdAP0APQC9AH0AwwADAIMAQwDjACMAowBjANMAEwCTAFMA8wAzALMAcwDLAAsAiwBLAOsAKwCrAGsA2wAbAJsAWwD7ADsAuwB7AMcABwCHAEcA5wAnAKcAZwDXABcAlwBXAPcANwC3AHcAzwAPAI8ATwDvAC8ArwBvAN8AHwCfAF8A/wA/AL8AfwDAgACAgIBAgOCAIICggGCA0IAQgJCAUIDwgDCAsIBwgMiACICIgEiA6IAogKiAaIDYgBiAmIBYgPiAOIC4gHiAxIAEgISARIDkgCSApIBkgNSAFICUgFSA9IA0gLSAdIDMgAyAjIBMgOyALICsgGyA3IAcgJyAXID8gDyAvIB8gMKAAoCCgEKA4oAigKKAYoDSgBKAkoBSgPKAMoCygHKAyoAKgIqASoDqgCqAqoBqgNqAGoCagFqA+oA6gLqAeoDGgAaAhoBGgOaAJoCmgGaA1oAWgJaAVoD2gDaAtoB2gM6ADoCOgE6A7oAugK6AboDegB6AnoBegP6APoC+gH6AwYABgIGAQYDhgCGAoYBhgNGAEYCRgFGA8YAxgLGAcYDJgAmAiYBJgOmAKYCpgGmA2YAZgJmAWYD5gDmAuYB5gMWABYCFgEWA5YAlgKWAZYDVgBWAlYBVgPWANYC1gHWAzYANgI2ATYDtgC2ArYBtgN2AHYCdgF2A/YA9gL2AfYDDgAOAg4BDgOOAI4CjgGOA04ATgJOAU4DzgDOAs4BzgMuAC4CLgEuA64ArgKuAa4DbgBuAm4BbgPuAO4C7gHuAx4AHgIeAR4DngCeAp4BngNeAF4CXgFeA94A3gLeAd4DPgA+Aj4BPgO+AL4CvgG+A34AfgJ+AX4D/gD+Av4B/gMBAAECAQEBA4EAgQKBAYEDQQBBAkEBQQPBAMECwQHBAyEAIQIhASEDoQChAqEBoQNhAGECYQFhA+EA4QLhAeEDEQARAhEBEQORAJECkQGRA1EAUQJRAVED0QDRAtEB0QMxADECMQExA7EAsQKxAbEDcQBxAnEBcQPxAPEC8QHxAwkACQIJAQkDiQCJAokBiQNJAEkCSQFJA8kAyQLJAckDKQApAikBKQOpAKkCqQGpA2kAaQJpAWkD6QDpAukB6QMZABkCGQEZA5kAmQKZAZkDWQBZAlkBWQPZANkC2QHZAzkAOQI5ATkDuQC5ArkBuQN5AHkCeQF5A/kA+QL5AfkDBQAFAgUBBQOFAIUChQGFA0UARQJFAUUDxQDFAsUBxQMlACUCJQElA6UApQKlAaUDZQBlAmUBZQPlAOUC5QHlAxUAFQIVARUDlQCVApUBlQNVAFUCVQFVA9UA1QLVAdUDNQA1AjUBNQO1ALUCtQG1A3UAdQJ1AXUD9QD1AvUB9QMNAA0CDQENA40AjQKNAY0DTQBNAk0BTQPNAM0CzQHNAy0ALQItAS0DrQCtAq0BrQNtAG0CbQFtA+0A7QLtAe0DHQAdAh0BHQOdAJ0CnQGdA10AXQJdAV0D3QDdAt0B3QM9AD0CPQE9A70AvQK9Ab0DfQB9An0BfQP9AP0C/QH9AwMAAwIDAQMDgwCDAoMBgwNDAEMCQwFDA8MAwwLDAcMDIwAjAiMBIwOjAKMCowGjA2MAYwJjAWMD4wDjAuMB4wMTABMCEwETA5MAkwKTAZMDUwBTAlMBUwPTANMC0wHTAzMAMwIzATMDswCzArMBswNzAHMCcwFzA/MA8wLzAfMDCwALAgsBCwOLAIsCiwGLA0sASwJLAUsDywDLAssBywMrACsCKwErA6sAqwKrAasDawBrAmsBawPrAOsC6wHrAxsAGwIbARsDmwCbApsBmwNbAFsCWwFbA9sA2wLbAdsDOwA7AjsBOwO7ALsCuwG7A3sAewJ7AXsD+wD7AvsB+wMHAAcCBwEHA4cAhwKHAYcD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")||(M=j,j=D.locateFile?D.locateFile(M,F):F+M);var T={30916:function(){return D.getRandomValue()},30952:function(){if(void 0===D.getRandomValue)try{var A="object"===("undefined"==typeof window?"undefined":g(window))?window:self,I=void 0!==A.crypto?A.crypto:A.msCrypto,B=function(){var A=new Uint32Array(1);return I.getRandomValues(A),A[0]>>>0};B(),D.getRandomValue=B}catch(A){try{var Q=eval("require")("crypto"),C=function(){var A=Q.randomBytes(4);return(A[0]<<24|A[1]<<16|A[2]<<8|A[3])>>>0};C(),D.getRandomValue=C}catch(A){throw"No secure random number generator found"}}}};function K(A){for(;A.length>0;)A.shift()(D)}var l=[];var p="function"==typeof atob?atob:function(A){var I,g,B,Q,C,E,i="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=",f="",D=0;A=A.replace(/[^A-Za-z0-9\+\/\=]/g,"");do{I=i.indexOf(A.charAt(D++))<<2|(Q=i.indexOf(A.charAt(D++)))>>4,g=(15&Q)<<4|(C=i.indexOf(A.charAt(D++)))>>2,B=(3&C)<<6|(E=i.indexOf(A.charAt(D++))),f+=String.fromCharCode(I),64!==C&&(f+=String.fromCharCode(g)),64!==E&&(f+=String.fromCharCode(B))}while(D<A.length);return f};function W(A){if(b(A))return function(A){if("boolean"==typeof e&&e){var I=Buffer.from(A,"base64");return new Uint8Array(I.buffer,I.byteOffset,I.byteLength)}try{for(var g=p(A),B=new Uint8Array(g.length),Q=0;Q<g.length;++Q)B[Q]=g.charCodeAt(Q);return B}catch(A){throw new Error("Converting base64 string to bytes failed.")}}(A.slice(x.length))}var X,V={e:function(A,I,g,B){L("Assertion failed: "+k(A)+", at: "+[I?k(I):"unknown filename",g,B?k(B):"unknown function"])},a:function(A,I,g){var B=function(A,I){var g;for(l.length=0,I>>=2;g=S[A++];)I+=105!=g&I,l.push(105==g?G[I]:U[I++>>1]),++I;return l}(I,g);return T[A].apply(null,B)},c:function(A,I,g){S.copyWithin(A,I,I+g)},b:function(A){S.length,L("OOM")},d:function(A){!function(A,I){A,function(A){A,J()||(D.onExit&&D.onExit(A),y=!0);r(A,new m(A))}(A)}(A)}};(function(){var A={a:V};function I(A,I){var g,B,Q=A.exports;D.asm=Q,h=D.asm.f,g=h.buffer,g,D.HEAP8=s=new Int8Array(g),D.HEAP16=new Int16Array(g),D.HEAP32=G=new Int32Array(g),D.HEAPU8=S=new Uint8Array(g),D.HEAPU16=new Uint16Array(g),D.HEAPU32=new Uint32Array(g),D.HEAPF32=new Float32Array(g),D.HEAPF64=U=new Float64Array(g),D.asm.h,B=D.asm.g,H.unshift(B),function(A){if(d--,D.monitorRunDependencies&&D.monitorRunDependencies(d),0==d&&(null!==v&&(clearInterval(v),v=null),Y)){var I=Y;Y=null,I()}}()}function g(A){I(A.instance)}function B(I){return function(){if(!c&&(a||w)){if("function"==typeof fetch&&!q(j))return fetch(j,{credentials:"same-origin"}).then((function(A){if(!A.ok)throw"failed to load wasm binary file at '"+j+"'";return A.arrayBuffer()})).catch((function(){return z(j)}));if(Q)return new Promise((function(A,I){Q(j,(function(I){A(new Uint8Array(I))}),I)}))}return Promise.resolve().then((function(){return z(j)}))}().then((function(I){return WebAssembly.instantiate(I,A)})).then((function(A){return A})).then(I,(function(A){t("failed to asynchronously prepare wasm: "+A),L(A)}))}if(d++,D.monitorRunDependencies&&D.monitorRunDependencies(d),D.instantiateWasm)try{return D.instantiateWasm(A,I)}catch(A){return t("Module.instantiateWasm callback failed with error: "+A),!1}c||"function"!=typeof WebAssembly.instantiateStreaming||b(j)||q(j)||e||"function"!=typeof fetch?B(g):fetch(j,{credentials:"same-origin"}).then((function(I){return WebAssembly.instantiateStreaming(I,A).then(g,(function(A){return t("wasm streaming compile failed: "+A),t("falling back to ArrayBuffer instantiation"),B(g)}))}))})(),D.___wasm_call_ctors=function(){return(D.___wasm_call_ctors=D.asm.g).apply(null,arguments)},D._malloc=function(){return(D._malloc=D.asm.i).apply(null,arguments)},D._free=function(){return(D._free=D.asm.j).apply(null,arguments)},D._falconjs_init=function(){return(D._falconjs_init=D.asm.k).apply(null,arguments)},D._falconjs_public_key_bytes=function(){return(D._falconjs_public_key_bytes=D.asm.l).apply(null,arguments)},D._falconjs_secret_key_bytes=function(){return(D._falconjs_secret_key_bytes=D.asm.m).apply(null,arguments)},D._falconjs_signature_bytes=function(){return(D._falconjs_signature_bytes=D.asm.n).apply(null,arguments)},D._falconjs_keypair=function(){return(D._falconjs_keypair=D.asm.o).apply(null,arguments)},D._falconjs_sign=function(){return(D._falconjs_sign=D.asm.p).apply(null,arguments)},D._falconjs_verify=function(){return(D._falconjs_verify=D.asm.q).apply(null,arguments)};function m(A){this.name="ExitStatus",this.message="Program terminated with exit("+A+")",this.status=A}function O(A){function I(){X||(X=!0,D.calledRun=!0,y||(!0,K(H),D.onRuntimeInitialized&&D.onRuntimeInitialized(),function(){if(D.postRun)for("function"==typeof D.postRun&&(D.postRun=[D.postRun]);D.postRun.length;)A=D.postRun.shift(),u.unshift(A);var A;K(u)}()))}A=A||n,d>0||(!function(){if(D.preRun)for("function"==typeof D.preRun&&(D.preRun=[D.preRun]);D.preRun.length;)A=D.preRun.shift(),N.unshift(A);var A;K(N)}(),d>0||(D.setStatus?(D.setStatus("Running..."),setTimeout((function(){setTimeout((function(){D.setStatus("")}),1),I()}),1)):I()))}if(D.writeArrayToMemory=function(A,I){s.set(A,I)},Y=function A(){X||O(),X||(Y=A)},D.run=O,D.preInit)for("function"==typeof D.preInit&&(D.preInit=[D.preInit]);D.preInit.length>0;)D.preInit.pop()();O()})).catch((function(){function A(I){return A="function"==typeof Symbol&&"symbol"==typeof Symbol.iterator?function(A){return typeof A}:function(A){return A&&"function"==typeof Symbol&&A.constructor===Symbol&&A!==Symbol.prototype?"symbol":typeof A},A(I)}var I,g,B,Q,C,E,i=void 0!==(i={})?i:{},f=Object.assign({},i),D=[],o=function(A,I){throw I},n="object"==("undefined"==typeof window?"undefined":A(window)),r="function"==typeof importScripts,a="object"==("undefined"==typeof process?"undefined":A(process))&&"object"==A(process.versions)&&"string"==typeof process.versions.node,w="";a?(w=r?eval("require")("path").dirname(w)+"/":__dirname+"/",E=function(){C||(Q=eval("require")("fs"),C=eval("require")("path"))},I=function(A,I){var g=V(A);return g?I?g:g.toString():(E(),A=C.normalize(A),Q.readFileSync(A,I?void 0:"utf8"))},B=function(A){var g=I(A,!0);return g.buffer||(g=new Uint8Array(g)),g},g=function(A,I,g){var B=V(A);B&&I(B),E(),A=C.normalize(A),Q.readFile(A,(function(A,B){A?g(A):I(B.buffer)}))},process.argv.length>1&&process.argv[1].replace(/\\/g,"/"),D=process.argv.slice(2), true&&(module.exports=i),process.on("uncaughtException",(function(A){if(!(A instanceof Z))throw A})),process.on("unhandledRejection",(function(A){throw A})),o=function(A,I){if(v())throw process.exitCode=A,I;var g;(g=I)instanceof Z||F("exiting due to exception: "+g),process.exit(A)},i.inspect=function(){return"[Emscripten Module object]"}):(n||r)&&(r?w=self.location.href:"undefined"!=typeof document&&document.currentScript&&(w=document.currentScript.src),w=0!==w.indexOf("blob:")?w.substr(0,w.replace(/[?#].*/,"").lastIndexOf("/")+1):"",I=function(A){try{var I=new XMLHttpRequest;return I.open("GET",A,!1),I.send(null),I.responseText}catch(I){var g=V(A);if(g)return function(A){for(var I=[],g=0;g<A.length;g++){var B=A[g];B>255&&(B&=255),I.push(String.fromCharCode(B))}return I.join("")}(g);throw I}},r&&(B=function(A){try{var I=new XMLHttpRequest;return I.open("GET",A,!1),I.responseType="arraybuffer",I.send(null),new Uint8Array(I.response)}catch(I){var g=V(A);if(g)return g;throw I}}),g=function(A,I,g){var B=new XMLHttpRequest;B.open("GET",A,!0),B.responseType="arraybuffer",B.onload=function(){if(200==B.status||0==B.status&&B.response)I(B.response);else{var Q=V(A);Q?I(Q.buffer):g()}},B.onerror=g,B.send(null)});i.print||console.log.bind(console);var e,F=i.printErr||console.warn.bind(console);Object.assign(i,f),f=null,i.arguments&&(D=i.arguments),i.thisProgram&&i.thisProgram,i.quit&&(o=i.quit),i.wasmBinary&&(e=i.wasmBinary);var c,t=i.noExitRuntime||!0,h={Memory:function(A){this.buffer=new ArrayBuffer(65536*A.initial)},Module:function(A){},Instance:function(A,I){this.exports=function(A){for(var I,g=new Uint8Array(123),B=25;B>=0;--B)g[48+B]=52+B,g[65+B]=B,g[97+B]=26+B;function Q(A,I,B){for(var Q,C,E=0,i=I,f=B.length,D=I+(3*f>>2)-("="==B[f-2])-("="==B[f-1]);E<f;E+=4)Q=g[B.charCodeAt(E+1)],C=g[B.charCodeAt(E+2)],A[i++]=g[B.charCodeAt(E)]<<2|Q>>4,i<D&&(A[i++]=Q<<4|C>>2),i<D&&(A[i++]=C<<6|g[B.charCodeAt(E+3)])}return g[43]=62,g[47]=63,function(A){var g=A.a.buffer,B=new Int8Array(g),C=new Int16Array(g),E=new Int32Array(g),i=new Uint8Array(g),f=new Uint16Array(g),D=new Uint32Array(g),o=(new Float32Array(g),new Float64Array(g),Math.imul),n=(Math.fround,Math.abs,Math.clz32,Math.min,Math.max,Math.floor,Math.ceil,Math.trunc,Math.sqrt,A.abort),r=A.b,a=A.c,w=A.d,e=8420912,F=0;function c(A){var I,g=0,B=0,Q=0,C=0,f=0,o=0,n=0,r=0,a=0,w=0;e=I=e-16|0;A:{I:{g:{B:{Q:{C:{E:{i:{f:{D:{o:{if((A|=0)>>>0<=244){if(3&(g=(C=E[7952])>>>(B=(n=A>>>0<11?16:A+11&-8)>>>3|0)|0)){g=(A=(B=B+(1&(-1^g))|0)<<3)+31848|0,Q=E[A+31856>>2],(0|g)!=(0|(A=E[Q+8>>2]))?(E[A+12>>2]=g,E[g+8>>2]=A):E[7952]=qA(-2,B)&C,A=Q+8|0,g=B<<3,E[Q+4>>2]=3|g,E[(g=g+Q|0)+4>>2]=1|E[g+4>>2];break A}if((w=E[7954])>>>0>=n>>>0)break o;if(g){B=A=(g=(0-(A=(0-(A=2<<B)|A)&g<<B)&A)-1|0)>>>12&16,B|=A=(g=g>>>A|0)>>>5&8,B|=A=(g=g>>>A|0)>>>2&4,g=(A=(Q=((B|=A=(g=g>>>A|0)>>>1&2)|(A=(g=g>>>A|0)>>>1&1))+(g>>>A|0)|0)<<3)+31848|0,o=E[A+31856>>2],(0|g)!=(0|(A=E[o+8>>2]))?(E[A+12>>2]=g,E[g+8>>2]=A):(C=qA(-2,Q)&C,E[7952]=C),E[o+4>>2]=3|n,Q=(A=Q<<3)-n|0,E[(B=o+n|0)+4>>2]=1|Q,E[A+o>>2]=Q,w&&(g=31848+(-8&w)|0,f=E[7957],(A=1<<(w>>>3))&C?A=E[g+8>>2]:(E[7952]=A|C,A=g),E[g+8>>2]=f,E[A+12>>2]=f,E[f+12>>2]=g,E[f+8>>2]=A),A=o+8|0,E[7957]=B,E[7954]=Q;break A}if(!(a=E[7953]))break o;for(B=A=(g=(0-a&a)-1|0)>>>12&16,B|=A=(g=g>>>A|0)>>>5&8,B|=A=(g=g>>>A|0)>>>2&4,B=E[32112+(((B|=A=(g=g>>>A|0)>>>1&2)|(A=(g=g>>>A|0)>>>1&1))+(g>>>A|0)<<2)>>2],f=(-8&E[B+4>>2])-n|0,g=B;(A=E[g+16>>2])||(A=E[g+20>>2]);)f=(Q=(g=(-8&E[A+4>>2])-n|0)>>>0<f>>>0)?g:f,B=Q?A:B,g=A;if(r=E[B+24>>2],(0|(Q=E[B+12>>2]))!=(0|B)){A=E[B+8>>2],E[A+12>>2]=Q,E[Q+8>>2]=A;break I}if(!(A=E[(g=B+20|0)>>2])){if(!(A=E[B+16>>2]))break D;g=B+16|0}for(;o=g,Q=A,(A=E[(g=A+20|0)>>2])||(g=Q+16|0,A=E[Q+16>>2]););E[o>>2]=0;break I}if(n=-1,!(A>>>0>4294967231)&&(n=-8&(A=A+11|0),a=E[7953])){f=0-n|0,C=0,n>>>0<256||(C=31,n>>>0>16777215||(A=A>>>8|0,A<<=o=A+1048320>>>16&8,C=28+((A=((A<<=B=A+520192>>>16&4)<<(g=A+245760>>>16&2)>>>15|0)-(g|B|o)|0)<<1|n>>>A+21&1)|0));n:{r:{if(g=E[32112+(C<<2)>>2])for(A=0,B=n<<(31==(0|C)?0:25-(C>>>1|0)|0);;){if(!((o=(-8&E[g+4>>2])-n|0)>>>0>=f>>>0||(Q=g,f=o))){f=0,A=g;break r}if(o=E[g+20>>2],g=E[16+((B>>>29&4)+g|0)>>2],A=o?(0|o)==(0|g)?A:o:A,B<<=1,!g)break}else A=0;if(!(A|Q)){if(Q=0,!(A=(0-(A=2<<C)|A)&a))break o;B=A=(g=(A&0-A)-1|0)>>>12&16,B|=A=(g=g>>>A|0)>>>5&8,B|=A=(g=g>>>A|0)>>>2&4,A=E[32112+(((B|=A=(g=g>>>A|0)>>>1&2)|(A=(g=g>>>A|0)>>>1&1))+(g>>>A|0)<<2)>>2]}if(!A)break n}for(;f=(B=(g=(-8&E[A+4>>2])-n|0)>>>0<f>>>0)?g:f,Q=B?A:Q,A=(g=E[A+16>>2])||E[A+20>>2];);}if(!(!Q|E[7954]-n>>>0<=f>>>0)){if(C=E[Q+24>>2],(0|Q)!=(0|(B=E[Q+12>>2]))){A=E[Q+8>>2],E[A+12>>2]=B,E[B+8>>2]=A;break g}if(!(A=E[(g=Q+20|0)>>2])){if(!(A=E[Q+16>>2]))break f;g=Q+16|0}for(;o=g,B=A,(A=E[(g=A+20|0)>>2])||(g=B+16|0,A=E[B+16>>2]););E[o>>2]=0;break g}}}if((B=E[7954])>>>0>=n>>>0){Q=E[7957],(g=B-n|0)>>>0>=16?(E[7954]=g,A=Q+n|0,E[7957]=A,E[A+4>>2]=1|g,E[B+Q>>2]=g,E[Q+4>>2]=3|n):(E[7957]=0,E[7954]=0,E[Q+4>>2]=3|B,E[(A=B+Q|0)+4>>2]=1|E[A+4>>2]),A=Q+8|0;break A}if((r=E[7955])>>>0>n>>>0){g=r-n|0,E[7955]=g,A=(B=E[7958])+n|0,E[7958]=A,E[A+4>>2]=1|g,E[B+4>>2]=3|n,A=B+8|0;break A}if(A=0,a=n+47|0,E[8070]?B=E[8072]:(E[8073]=-1,E[8074]=-1,E[8071]=4096,E[8072]=4096,E[8070]=I+12&-16^1431655768,E[8075]=0,E[8063]=0,B=4096),(g=(o=a+B|0)&(f=0-B|0))>>>0<=n>>>0)break A;if((Q=E[8062])&&Q>>>0<(C=(B=E[8060])+g|0)>>>0|B>>>0>=C>>>0)break A;if(4&i[32252])break C;o:{n:{if(Q=E[7958])for(A=32256;;){if((B=E[A>>2])>>>0<=Q>>>0&Q>>>0<B+E[A+4>>2]>>>0)break n;if(!(A=E[A+8>>2]))break}if(-1==(0|(B=rA(0))))break E;if(C=g,(A=(Q=E[8071])-1|0)&B&&(C=(g-B|0)+(A+B&0-Q)|0),C>>>0<=n>>>0|C>>>0>2147483646)break E;if((Q=E[8062])&&Q>>>0<(f=(A=E[8060])+C|0)>>>0|A>>>0>=f>>>0)break E;if((0|B)!=(0|(A=rA(C))))break o;break Q}if((C=f&o-r)>>>0>2147483646)break E;if((0|(B=rA(C)))==(E[A>>2]+E[A+4>>2]|0))break i;A=B}if(!(-1==(0|A)|n+48>>>0<=C>>>0)){if((B=(B=E[8072])+(a-C|0)&0-B)>>>0>2147483646){B=A;break Q}if(-1!=(0|rA(B))){C=B+C|0,B=A;break Q}rA(0-C|0);break E}if(B=A,-1!=(0|A))break Q;break E}Q=0;break I}B=0;break g}if(-1!=(0|B))break Q}E[8063]=4|E[8063]}if(g>>>0>2147483646)break B;if(-1==(0|(B=rA(g)))|-1==(0|(A=rA(0)))|A>>>0<=B>>>0)break B;if((C=A-B|0)>>>0<=n+40>>>0)break B}A=E[8060]+C|0,E[8060]=A,A>>>0>D[8061]&&(E[8061]=A);Q:{C:{E:{if(o=E[7958]){for(A=32256;;){if(((Q=E[A>>2])+(g=E[A+4>>2])|0)==(0|B))break E;if(!(A=E[A+8>>2]))break}break C}for((A=E[7956])>>>0<=B>>>0&&A||(E[7956]=B),A=0,E[8065]=C,E[8064]=B,E[7960]=-1,E[7961]=E[8070],E[8067]=0;g=(Q=A<<3)+31848|0,E[Q+31856>>2]=g,E[Q+31860>>2]=g,32!=(0|(A=A+1|0)););g=(Q=C-40|0)-(A=B+8&7?-8-B&7:0)|0,E[7955]=g,A=A+B|0,E[7958]=A,E[A+4>>2]=1|g,E[4+(B+Q|0)>>2]=40,E[7959]=E[8074];break Q}if(!(8&i[A+12|0]|Q>>>0>o>>>0|B>>>0<=o>>>0)){E[A+4>>2]=g+C,B=(A=o+8&7?-8-o&7:0)+o|0,E[7958]=B,A=(g=E[7955]+C|0)-A|0,E[7955]=A,E[B+4>>2]=1|A,E[4+(g+o|0)>>2]=40,E[7959]=E[8074];break Q}}D[7956]>B>>>0&&(E[7956]=B),g=B+C|0,A=32256;C:{E:{i:{f:{D:{o:{for(;;){if((0|g)!=E[A>>2]){if(A=E[A+8>>2])continue;break o}break}if(!(8&i[A+12|0]))break D}for(A=32256;;){if((g=E[A>>2])>>>0<=o>>>0&&(f=g+E[A+4>>2]|0)>>>0>o>>>0)break f;A=E[A+8>>2]}}if(E[A>>2]=B,E[A+4>>2]=E[A+4>>2]+C,E[(a=(B+8&7?-8-B&7:0)+B|0)+4>>2]=3|n,A=(C=g+(g+8&7?-8-g&7:0)|0)-(r=n+a|0)|0,(0|C)==(0|o)){E[7958]=r,A=E[7955]+A|0,E[7955]=A,E[r+4>>2]=1|A;break E}if(E[7957]==(0|C)){E[7957]=r,A=E[7954]+A|0,E[7954]=A,E[r+4>>2]=1|A,E[A+r>>2]=A;break E}if(1==(3&(f=E[C+4>>2]))){o=-8&f;D:if(f>>>0<=255){if(Q=E[C+8>>2],g=f>>>3|0,(0|(B=E[C+12>>2]))==(0|Q)){E[7952]=E[7952]&qA(-2,g);break D}E[Q+12>>2]=B,E[B+8>>2]=Q}else{if(n=E[C+24>>2],(0|C)==(0|(B=E[C+12>>2])))if((g=E[(f=C+20|0)>>2])||(g=E[(f=C+16|0)>>2])){for(;Q=f,B=g,(g=E[(f=g+20|0)>>2])||(f=B+16|0,g=E[B+16>>2]););E[Q>>2]=0}else B=0;else g=E[C+8>>2],E[g+12>>2]=B,E[B+8>>2]=g;if(n){Q=E[C+28>>2];o:{if(E[(g=32112+(Q<<2)|0)>>2]==(0|C)){if(E[g>>2]=B,B)break o;E[7953]=E[7953]&qA(-2,Q);break D}if(E[n+(E[n+16>>2]==(0|C)?16:20)>>2]=B,!B)break D}E[B+24>>2]=n,(g=E[C+16>>2])&&(E[B+16>>2]=g,E[g+24>>2]=B),(g=E[C+20>>2])&&(E[B+20>>2]=g,E[g+24>>2]=B)}}f=E[(C=C+o|0)+4>>2],A=A+o|0}if(E[C+4>>2]=-2&f,E[r+4>>2]=1|A,E[A+r>>2]=A,A>>>0<=255){g=31848+(-8&A)|0,(B=E[7952])&(A=1<<(A>>>3))?A=E[g+8>>2]:(E[7952]=A|B,A=g),E[g+8>>2]=r,E[A+12>>2]=r,E[r+12>>2]=g,E[r+8>>2]=A;break E}if(f=31,A>>>0<=16777215&&(g=A>>>8|0,g<<=f=g+1048320>>>16&8,f=28+((g=((g<<=Q=g+520192>>>16&4)<<(B=g+245760>>>16&2)>>>15|0)-(B|Q|f)|0)<<1|A>>>g+21&1)|0),E[r+28>>2]=f,E[r+16>>2]=0,E[r+20>>2]=0,g=32112+(f<<2)|0,(Q=E[7953])&(B=1<<f)){for(f=A<<(31==(0|f)?0:25-(f>>>1|0)|0),B=E[g>>2];;){if((-8&E[(g=B)+4>>2])==(0|A))break i;if(B=f>>>29|0,f<<=1,!(B=E[(Q=(4&B)+g|0)+16>>2]))break}E[Q+16>>2]=r}else E[7953]=B|Q,E[g>>2]=r;E[r+24>>2]=g,E[r+12>>2]=r,E[r+8>>2]=r;break E}for(g=(Q=C-40|0)-(A=B+8&7?-8-B&7:0)|0,E[7955]=g,A=A+B|0,E[7958]=A,E[A+4>>2]=1|g,E[4+(B+Q|0)>>2]=40,E[7959]=E[8074],E[(Q=(A=(f+(f-39&7?39-f&7:0)|0)-47|0)>>>0<o+16>>>0?o:A)+4>>2]=27,A=E[8067],E[Q+16>>2]=E[8066],E[Q+20>>2]=A,A=E[8065],E[Q+8>>2]=E[8064],E[Q+12>>2]=A,E[8066]=Q+8,E[8065]=C,E[8064]=B,E[8067]=0,A=Q+24|0;E[A+4>>2]=7,g=A+8|0,A=A+4|0,g>>>0<f>>>0;);if((0|Q)==(0|o))break Q;if(E[Q+4>>2]=-2&E[Q+4>>2],f=Q-o|0,E[o+4>>2]=1|f,E[Q>>2]=f,f>>>0<=255){g=31848+(-8&f)|0,(B=E[7952])&(A=1<<(f>>>3))?A=E[g+8>>2]:(E[7952]=A|B,A=g),E[g+8>>2]=o,E[A+12>>2]=o,E[o+12>>2]=g,E[o+8>>2]=A;break Q}if(A=31,f>>>0<=16777215&&(A=f>>>8|0,A<<=Q=A+1048320>>>16&8,A=28+((A=((A<<=B=A+520192>>>16&4)<<(g=A+245760>>>16&2)>>>15|0)-(g|B|Q)|0)<<1|f>>>A+21&1)|0),E[o+28>>2]=A,E[o+16>>2]=0,E[o+20>>2]=0,g=32112+(A<<2)|0,(Q=E[7953])&(B=1<<A)){for(A=f<<(31==(0|A)?0:25-(A>>>1|0)|0),Q=E[g>>2];;){if((0|f)==(-8&E[(g=Q)+4>>2]))break C;if(B=A>>>29|0,A<<=1,!(Q=E[(B=(4&B)+g|0)+16>>2]))break}E[B+16>>2]=o}else E[7953]=B|Q,E[g>>2]=o;E[o+24>>2]=g,E[o+12>>2]=o,E[o+8>>2]=o;break Q}A=E[g+8>>2],E[A+12>>2]=r,E[g+8>>2]=r,E[r+24>>2]=0,E[r+12>>2]=g,E[r+8>>2]=A}A=a+8|0;break A}A=E[g+8>>2],E[A+12>>2]=o,E[g+8>>2]=o,E[o+24>>2]=0,E[o+12>>2]=g,E[o+8>>2]=A}if(!((A=E[7955])>>>0<=n>>>0)){g=A-n|0,E[7955]=g,A=(B=E[7958])+n|0,E[7958]=A,E[A+4>>2]=1|g,E[B+4>>2]=3|n,A=B+8|0;break A}}E[7951]=48,A=0;break A}g:if(C){g=E[Q+28>>2];B:{if(E[(A=32112+(g<<2)|0)>>2]==(0|Q)){if(E[A>>2]=B,B)break B;a=qA(-2,g)&a,E[7953]=a;break g}if(E[C+(E[C+16>>2]==(0|Q)?16:20)>>2]=B,!B)break g}E[B+24>>2]=C,(A=E[Q+16>>2])&&(E[B+16>>2]=A,E[A+24>>2]=B),(A=E[Q+20>>2])&&(E[B+20>>2]=A,E[A+24>>2]=B)}g:if(f>>>0<=15)A=f+n|0,E[Q+4>>2]=3|A,E[(A=A+Q|0)+4>>2]=1|E[A+4>>2];else if(E[Q+4>>2]=3|n,E[(C=Q+n|0)+4>>2]=1|f,E[C+f>>2]=f,f>>>0<=255)g=31848+(-8&f)|0,(B=E[7952])&(A=1<<(f>>>3))?A=E[g+8>>2]:(E[7952]=A|B,A=g),E[g+8>>2]=C,E[A+12>>2]=C,E[C+12>>2]=g,E[C+8>>2]=A;else{A=31,f>>>0<=16777215&&(A=f>>>8|0,A<<=o=A+1048320>>>16&8,A=28+((A=((A<<=B=A+520192>>>16&4)<<(g=A+245760>>>16&2)>>>15|0)-(g|B|o)|0)<<1|f>>>A+21&1)|0),E[C+28>>2]=A,E[C+16>>2]=0,E[C+20>>2]=0,g=32112+(A<<2)|0;B:{if((B=1<<A)&a){for(A=f<<(31==(0|A)?0:25-(A>>>1|0)|0),n=E[g>>2];;){if((-8&E[(g=n)+4>>2])==(0|f))break B;if(B=A>>>29|0,A<<=1,!(n=E[(B=(4&B)+g|0)+16>>2]))break}E[B+16>>2]=C}else E[7953]=B|a,E[g>>2]=C;E[C+24>>2]=g,E[C+12>>2]=C,E[C+8>>2]=C;break g}A=E[g+8>>2],E[A+12>>2]=C,E[g+8>>2]=C,E[C+24>>2]=0,E[C+12>>2]=g,E[C+8>>2]=A}A=Q+8|0;break A}I:if(r){g=E[B+28>>2];g:{if(E[(A=32112+(g<<2)|0)>>2]==(0|B)){if(E[A>>2]=Q,Q)break g;E[7953]=qA(-2,g)&a;break I}if(E[r+(E[r+16>>2]==(0|B)?16:20)>>2]=Q,!Q)break I}E[Q+24>>2]=r,(A=E[B+16>>2])&&(E[Q+16>>2]=A,E[A+24>>2]=Q),(A=E[B+20>>2])&&(E[Q+20>>2]=A,E[A+24>>2]=Q)}f>>>0<=15?(A=f+n|0,E[B+4>>2]=3|A,E[(A=A+B|0)+4>>2]=1|E[A+4>>2]):(E[B+4>>2]=3|n,E[(Q=B+n|0)+4>>2]=1|f,E[Q+f>>2]=f,w&&(g=31848+(-8&w)|0,o=E[7957],(A=1<<(w>>>3))&C?A=E[g+8>>2]:(E[7952]=A|C,A=g),E[g+8>>2]=o,E[A+12>>2]=o,E[o+12>>2]=g,E[o+8>>2]=A),E[7957]=Q,E[7954]=f),A=B+8|0}return e=I+16|0,0|A}function t(A){var I=0,g=0,B=0,Q=0,C=0,i=0,f=0,D=0,o=0,n=0,r=0,a=0,w=0,e=0,c=0,t=0,h=0,P=0,y=0,s=0,S=0,G=0,U=0,R=0,k=0,N=0,H=0,u=0,J=0,d=0,v=0,Y=0,L=0,j=0,M=0,x=0,b=0,q=0,z=0,T=0,K=0,l=0,p=0,W=0,V=0,m=0,O=0,Z=0,_=0,$=0,AA=0,IA=0,gA=0,BA=0,QA=0,CA=0,EA=0,iA=0,fA=0,DA=0,oA=0,nA=0,rA=0,aA=0,wA=0,eA=0,FA=0,cA=0,tA=0,hA=0,PA=0,yA=0,sA=0,SA=0,GA=0,UA=0,RA=0,kA=0,NA=0,HA=0,uA=0,JA=0,dA=0,vA=0,YA=0,LA=0,jA=0,MA=0,xA=0,bA=0,qA=0,zA=0,TA=0,KA=0,lA=0;for(Z=E[A+192>>2],x=E[A+196>>2],_=E[A+184>>2],$=E[A+188>>2],P=E[A+176>>2],z=E[A+180>>2],y=E[A+168>>2],T=E[A+172>>2],K=E[A+160>>2],s=E[A+164>>2],S=E[A+152>>2],i=E[A+156>>2],G=E[A+144>>2],U=E[A+148>>2],R=E[A+136>>2],l=E[A+140>>2],AA=E[A+128>>2],IA=E[A+132>>2],k=E[A+120>>2],p=E[A+124>>2],N=E[A+112>>2],o=E[A+116>>2],f=E[A+104>>2],n=E[A+108>>2],W=E[A+96>>2],H=E[A+100>>2],g=E[A+88>>2],B=E[A+92>>2],u=E[A+80>>2],V=E[A+84>>2],D=E[A+72>>2],r=E[A+76>>2],c=E[A+64>>2],a=E[A+68>>2],t=E[A+56>>2],Q=E[A+60>>2],h=E[A+48>>2],J=E[A+52>>2],C=E[A+40>>2],d=E[A+44>>2],fA=E[A+32>>2],gA=E[A+36>>2],v=E[A+24>>2],Y=E[A+28>>2],w=E[A+16>>2],L=E[A+20>>2],I=E[A+8>>2],e=E[A+12>>2],b=E[A>>2],j=E[A+4>>2];FA>>>0<=23;)BA=m=j^d^V^s^p,EA=v^c^f^G^_,m=X(O=b^C^u^k^K,m,1)^EA,M=X(fA^m,gA^(iA=(rA=Y^a^n^U^$)^F),27),QA=F,DA=q=L^Q^H^l^z,O=X(aA=w^t^W^R^P,q,1)^O,q=X(g^O,B^(BA^=F),10),CA=F,wA=B=e^J^B^T^IA,oA=fA^D^N^S^Z,g=X(eA=I^h^g^y^AA,B,1)^oA,C=X(C^g,d^(B=(gA=gA^r^o^i^x)^F),36),cA=F,d=X(I^O,e^BA,1),fA=F,e=f,f=X(oA,gA,1)^aA,gA=X(e^f,(nA=n)^(n=F^DA),25),I=F,nA=t,t=X(EA,rA,1)^eA,e=X(nA^t,(DA=Q)^(Q=F^wA),6),rA=F,v=X(f^v,n^Y,28),Y=F,u=X(g^u,B^V,3),V=F,EA=X(D^m,r^iA,20),DA=D=F,r=X(w^t,Q^L,62),w=F,N=X(N^m,o^iA,39),o=F,L=X(f^c,a^n,55),aA=c=F,oA=E[(a=(tA=FA<<3)+1120|0)>>2],a=E[a+4>>2],W=X(t^W,Q^H,43),H=F,h=X(h^O,J^BA,44),a=D=(hA=(wA=B^j)^(-1^(J=F))&H^a)^(PA=(-1^c)&o^w)^(yA=(-1^D)&V^Y)^(sA=(-1^cA)&CA^QA)^(SA=(-1^rA)&I^fA),D=X(c=(GA=(nA=oA)^(oA=g^b)^(-1^h)&W)^(UA=(-1^L)&N^r)^(RA=(-1^EA)&u^v)^(kA=(-1^C)&q^M)^(NA=(-1^e)&gA^d),D,1),b=F,R=X(R^t,Q^l,15),l=F,_=X(f^_,n^$,56),eA=j=F,$=X(f^G,n^U,21),G=F,Z=X(Z^m,x^iA,14),f=F,x=X(g^k,B^p,41),U=F,y=X(y^O,T^BA,2),k=F,T=X(S^m,i^iA,8),S=F,K=X(g^K,B^s,18),p=F,s=X(O^AA,IA^BA,45),i=F,HA=(-1^_)&M^R,uA=(-1^Z)&oA^$,AA=(-1^y)&r^x,JA=(-1^K)&d^T,P=X(t^P,Q^z,61),b=X((D^=n=HA^uA^AA^JA^(IA=(-1^P)&v^s))^(g=(-1^r)&L^y),(r=(jA=(dA=(-1^j)&QA^l)^(vA=(-1^f)&wA^G)^(B=(-1^k)&w^U)^(YA=(-1^p)&fA^S)^(LA=(-1^(z=F))&Y^i))^b)^(Q=(-1^w)&aA^k),14),j=F,c=X(y=(MA=gA^(-1^T)&K)^(w=W^(-1^$)&Z)^(xA=(-1^x)&y^N)^(bA=u^(-1^s)&P)^(qA=(-1^R)&_^q),k=(zA=I^(-1^S)&p)^(TA=H^(-1^G)&f)^(KA=(-1^U)&k^o)^(lA=V^(-1^i)&z)^(nA=(-1^l)&eA^CA),1)^c,m=X(c^(s=(-1^u)&s^EA),(a^=F)^(i=(-1^V)&i^DA),44),iA=F,U=aA^(-1^o)&U,u=rA^(-1^I)&S,I=(-1^H)&G^J,t=(Z=(M=_^(-1^M)&C)^(EA=P^(-1^v)&EA)^(G=K^(-1^d)&e)^g^(o=(-1^oA)&h^Z))^X(_=(x=L^(-1^N)&x)^(H=e^(-1^gA)&T)^(q=C^(-1^q)&R)^(g=(-1^W)&$^h)^s,S=U^u^(CA=cA^(-1^CA)&l)^I^i,1),BA=P=(Q=($=(C=eA^(-1^QA)&cA)^(d=z^(-1^Y)&DA)^(R=p^(-1^fA)&rA)^Q^(N=(-1^wA)&J^f))^F)^hA,fA=(-1^(O=t^GA))&m^b,gA=(-1^P)&iA^j,P=X(c^x,a^U,2),z=F,v=X(Z,$,1)^y,K=X(IA^v,LA^(Y=k^F),55),s=F,DA=w,w=X(n,jA,1)^_,Z=(-1^(S=X(DA^w,(L=S^F)^TA,62)))&K^P,x=(-1^(i=x=F))&s^z,_=(y=X(t^kA,Q^sA,41))^(-1^P)&S,$=(T=F)^(-1^z)&i,P=(G=X(D^G,r^R,39))^(-1^y)&P,z=(U=F)^(-1^T)&z,y=K^(-1^G)&y,T=s^(-1^U)&T,K=S^(-1^K)&G,s=i^(-1^s)&U,R=X(v^AA,B^Y,56),l=F,k=X(t^RA,Q^yA,36),p=F,N=X(o^D,r^N,27),o=i=F,S=(-1^N)&k^R,i=(-1^i)&p^l,G=(AA=X(w^qA,L^nA,15))^(-1^R)&N,U=(IA=F)^(-1^l)&o,R=(f=X(H^c,a^u,10))^(-1^AA)&R,l=(n=F)^(-1^IA)&l,AA=k^(-1^f)&AA,IA=p^(-1^n)&IA,k=(-1^k)&f^N,p=(-1^p)&n^o,W=X(t^UA,Q^PA,18),H=F,u=X(w^bA,L^lA,6),V=F,h=X(g^c,I^a,1),J=o=F,N=(-1^h)&u^W,o=(-1^o)&V^H,f=(g=X(D^M,C^r,8))^(-1^W)&h,n=(B=F)^(-1^H)&J,W=(C=X(v^JA,Y^YA,25))^(-1^g)&W,H=(I=F)^(-1^B)&H,g=u^(-1^C)&g,B=V^(-1^I)&B,u=(-1^u)&C^h,V=(-1^V)&I^J,h=X(w^xA,L^KA,61),J=F,C=X(D^EA,r^d,20),d=F,I=X(v^uA,Y^vA,28),e=r=F,D=(-1^I)&C^h,r=(-1^r)&d^J,c=(M=X(c^q,a^CA,45))^(-1^h)&I,a=(QA=F)^(-1^J)&e,t=(q=X(t^NA,Q^SA,3))^(-1^M)&h,Q=(CA=F)^(-1^QA)&J,h=(-1^q)&M^C,J=(-1^CA)&QA^d,C=I^(-1^C)&q,d=e^(-1^d)&CA,v=(I=X(v^HA,Y^dA,21))^(-1^b)&O,Y=(e=F)^(-1^j)&BA,w=(M=X(w^MA,L^zA,43))^(-1^I)&b,L=(b=QA=F)^(-1^e)&j,I=(-1^M)&I^m,e=(-1^b)&e^iA,b=(-1^m)&M^E[(j=1120+(8|tA)|0)>>2]^O,j=(-1^iA)&QA^E[j+4>>2]^BA,FA=FA+2|0;E[A+192>>2]=Z,E[A+196>>2]=x,E[A+184>>2]=_,E[A+188>>2]=$,E[A+176>>2]=P,E[A+180>>2]=z,E[A+168>>2]=y,E[A+172>>2]=T,E[A+160>>2]=K,E[A+164>>2]=s,E[A+152>>2]=S,E[A+156>>2]=i,E[A+144>>2]=G,E[A+148>>2]=U,E[A+136>>2]=R,E[A+140>>2]=l,E[A+128>>2]=AA,E[A+132>>2]=IA,E[A+120>>2]=k,E[A+124>>2]=p,E[A+112>>2]=N,E[A+116>>2]=o,E[A+104>>2]=f,E[A+108>>2]=n,E[A+96>>2]=W,E[A+100>>2]=H,E[A+88>>2]=g,E[A+92>>2]=B,E[A+80>>2]=u,E[A+84>>2]=V,E[A+72>>2]=D,E[A+76>>2]=r,E[A+64>>2]=c,E[A+68>>2]=a,E[A+56>>2]=t,E[A+60>>2]=Q,E[A+48>>2]=h,E[A+52>>2]=J,E[A+40>>2]=C,E[A+44>>2]=d,E[A+32>>2]=fA,E[A+36>>2]=gA,E[A+24>>2]=v,E[A+28>>2]=Y,E[A+16>>2]=w,E[A+20>>2]=L,E[A+8>>2]=I,E[A+12>>2]=e,E[A>>2]=b,E[A+4>>2]=j}function h(A){var I=0,g=0,B=0,Q=0,C=0,i=0,f=0;A:if(A|=0){C=(B=A-8|0)+(A=-8&(I=E[A-4>>2]))|0;I:if(!(1&I)){if(!(3&I))break A;if((B=B-(I=E[B>>2])|0)>>>0<D[7956])break A;if(A=A+I|0,E[7957]==(0|B)){if(3==(3&(I=E[C+4>>2])))return E[7954]=A,E[C+4>>2]=-2&I,E[B+4>>2]=1|A,void(E[A+B>>2]=A)}else{if(I>>>0<=255){if(Q=E[B+8>>2],I=I>>>3|0,(0|(g=E[B+12>>2]))==(0|Q)){E[7952]=E[7952]&qA(-2,I);break I}E[Q+12>>2]=g,E[g+8>>2]=Q;break I}if(f=E[B+24>>2],(0|B)==(0|(I=E[B+12>>2])))if((g=E[(Q=B+20|0)>>2])||(g=E[(Q=B+16|0)>>2])){for(;i=Q,(g=E[(Q=(I=g)+20|0)>>2])||(Q=I+16|0,g=E[I+16>>2]););E[i>>2]=0}else I=0;else g=E[B+8>>2],E[g+12>>2]=I,E[I+8>>2]=g;if(!f)break I;Q=E[B+28>>2];g:{if(E[(g=32112+(Q<<2)|0)>>2]==(0|B)){if(E[g>>2]=I,I)break g;E[7953]=E[7953]&qA(-2,Q);break I}if(E[f+(E[f+16>>2]==(0|B)?16:20)>>2]=I,!I)break I}if(E[I+24>>2]=f,(g=E[B+16>>2])&&(E[I+16>>2]=g,E[g+24>>2]=I),!(g=E[B+20>>2]))break I;E[I+20>>2]=g,E[g+24>>2]=I}}if(!(B>>>0>=C>>>0)&&1&(I=E[C+4>>2])){I:{if(!(2&I)){if(E[7958]==(0|C)){if(E[7958]=B,A=E[7955]+A|0,E[7955]=A,E[B+4>>2]=1|A,E[7957]!=(0|B))break A;return E[7954]=0,void(E[7957]=0)}if(E[7957]==(0|C))return E[7957]=B,A=E[7954]+A|0,E[7954]=A,E[B+4>>2]=1|A,void(E[A+B>>2]=A);A=(-8&I)+A|0;g:if(I>>>0<=255){if(Q=E[C+8>>2],I=I>>>3|0,(0|(g=E[C+12>>2]))==(0|Q)){E[7952]=E[7952]&qA(-2,I);break g}E[Q+12>>2]=g,E[g+8>>2]=Q}else{if(f=E[C+24>>2],(0|C)==(0|(I=E[C+12>>2])))if((g=E[(Q=C+20|0)>>2])||(g=E[(Q=C+16|0)>>2])){for(;i=Q,(g=E[(Q=(I=g)+20|0)>>2])||(Q=I+16|0,g=E[I+16>>2]););E[i>>2]=0}else I=0;else g=E[C+8>>2],E[g+12>>2]=I,E[I+8>>2]=g;if(f){Q=E[C+28>>2];B:{if(E[(g=32112+(Q<<2)|0)>>2]==(0|C)){if(E[g>>2]=I,I)break B;E[7953]=E[7953]&qA(-2,Q);break g}if(E[f+(E[f+16>>2]==(0|C)?16:20)>>2]=I,!I)break g}E[I+24>>2]=f,(g=E[C+16>>2])&&(E[I+16>>2]=g,E[g+24>>2]=I),(g=E[C+20>>2])&&(E[I+20>>2]=g,E[g+24>>2]=I)}}if(E[B+4>>2]=1|A,E[A+B>>2]=A,E[7957]!=(0|B))break I;return void(E[7954]=A)}E[C+4>>2]=-2&I,E[B+4>>2]=1|A,E[A+B>>2]=A}if(A>>>0<=255)return I=31848+(-8&A)|0,(g=E[7952])&(A=1<<(A>>>3))?A=E[I+8>>2]:(E[7952]=A|g,A=I),E[I+8>>2]=B,E[A+12>>2]=B,E[B+12>>2]=I,void(E[B+8>>2]=A);Q=31,A>>>0<=16777215&&(I=A>>>8|0,I<<=i=I+1048320>>>16&8,Q=28+((I=((I<<=Q=I+520192>>>16&4)<<(g=I+245760>>>16&2)>>>15|0)-(g|Q|i)|0)<<1|A>>>I+21&1)|0),E[B+28>>2]=Q,E[B+16>>2]=0,E[B+20>>2]=0,i=32112+(Q<<2)|0;I:{g:{if((g=E[7953])&(I=1<<Q)){for(Q=A<<(31==(0|Q)?0:25-(Q>>>1|0)|0),I=E[i>>2];;){if(g=I,(-8&E[I+4>>2])==(0|A))break g;if(I=Q>>>29|0,Q<<=1,!(I=E[(i=g+(4&I)|0)+16>>2]))break}E[i+16>>2]=B,E[B+24>>2]=g}else E[7953]=I|g,E[i>>2]=B,E[B+24>>2]=i;E[B+12>>2]=B,E[B+8>>2]=B;break I}A=E[g+8>>2],E[A+12>>2]=B,E[g+8>>2]=B,E[B+24>>2]=0,E[B+12>>2]=g,E[B+8>>2]=A}A=E[7960]-1|0,E[7960]=A||-1}}}function P(A,I,g,B,Q){var C,f,D,n,r,a,w=0,e=0,c=0,t=0,h=0,P=0,s=0,S=0,G=0,U=0,R=0,k=0,N=0;for(G=w=g>>31,w=(e=w)^(1073741823&(w=g<<10|I>>>22)|1073741824),P=((w=(t=(h=g>>>31|0)+(e^I<<10&-1024)|0)>>>0<h>>>0?w+1|0:w)^t)&(e=0-((h=61-(g>>>20|0)&63)>>>5|0)|0)^t,t=(w^w>>31)&(U=e>>31)^w,w=31&h,C=lA(I,g,M(e=G=(2145386496&g)>>>0<1071644672?G:(63&w)>>>0>=32?t>>w:((1<<w)-1&t)<<32-w|P>>>w,w=e>>31),F),f=F,D=SA(OA(B,Q),F),n=F,r=j(B,Q,E[A+784>>2],E[A+788>>2]),a=F;;){for(B=0,(c=E[A+512>>2])>>>0>=503&&(y(A),c=0),E[A+512>>2]=c+8,g=i[(I=A+c|0)+4|0]|i[I+5|0]<<8|i[I+6|0]<<16|i[I+7|0]<<24,I=i[0|I]|i[I+1|0]<<8|i[I+2|0]<<16|i[I+3|0]<<24,Q=MA(A)<<16|g>>>16,w=16777215&I,g=16777215&((16777215&g)<<8|I>>>24),c=0;c>>>0<=53;)B=((Q-E[(I=c<<2)+26720>>2]|0)+((g-E[I+26724>>2]|0)+(w-E[I+26728>>2]>>31)>>31)>>>31|0)+B|0,c=c+3|0;for(I=1&MA(A),I=lA(j(OA(lA(M(I=k=o(B,(I<<1)-1|0)+I|0,I>>31),F,C,f),F),F,D,n),F,j(M(o(B,B),0),F,-1048331326,1069764491),F),t=BA(j(I,g=F,1697350398,1073157447),F),h=(I=BA(j(lA(I,g,j(M(B=t,w=B>>31),F,-17155601,1072049730),F),F,0,1138753536),F))<<1&-2,U=e=F<<1|I>>>31,Q=0,I=1947304867,g=4,c=1;13!=(0|c);)s=E[(B=1376+(c<<3)|0)>>2],N=E[B+4>>2],S=g,e=iA(g,0,U,Q),R=F,B=I,g=iA(I,w=0,U,Q),P=(I=e)+(e=F)|0,I=R,e=I=e>>>0>P>>>0?I+1|0:I,I=iA(h,Q,S,w),S=F,R=P,iA(B,w,h,Q),w=S,w=(I=(P=F)+I|0)>>>0<P>>>0?w+1|0:w,P=I,S=w,B=R+w|0,w=e,I=w=B>>>0<S>>>0?w+1|0:w,e=0,B=(g=(e=g+P>>>0<g>>>0?e+1|0:e)+B|0)>>>0<e>>>0?I+1|0:I,I=s-g|0,g=N-((g>>>0>s>>>0)+B|0)|0,c=c+1|0;for(s=(B=BA(j(r,a,0,1138753536),F))<<1,e=I,Q=iA(c=F<<1|B>>>31,B=0,I,0),B=(w=F)+(h=iA(I=c,B,c=g,g=0))|0,I=(w=0)+F|0,I=B>>>0<h>>>0?I+1|0:I,S=B,iA(s&=-2,0,e,w),g=(e=F)+(w=iA(c,g,s,B=0))|0,e=F+B|0,B=g,w=(g=S+(e=g>>>0<w>>>0?e+1|0:e)|0)>>>0<e>>>0?I+1|0:I,I=0,I=B+Q>>>0<Q>>>0?I+1|0:I,B=g,g=I,B=(I=B+I|0)>>>0<g>>>0?w+1|0:w,g=I,I=B<<1|I>>>31,B=(g<<=1)-1|0,g=I-!g|0,I=31&(Q=(t+-64|0)<0?t:63),(63&Q)>>>0>=32?(w=0,t=g>>>I|0):(w=g>>>I|0,t=((1<<I)-1&g)<<32-I|B>>>I),I=64,g=0;Q=g-(I>>>0<8)|0,e=31&(B=I-8|0),(0|(s=MA(A)))==(0|(e=255&((63&B)>>>0>=32?w>>>e|0:((1<<e)-1&w)<<32-e|t>>>e)))&&(c=!g&I>>>0>8|0!=(0|g),I=B,g=Q,c););if(s-e>>>31|0)break}return G+k|0}function y(A){var I,g,Q,C=0,i=0,f=0,D=0,o=0,n=0,r=0,a=0,w=0,F=0,c=0,t=0,h=0,P=0,y=0,s=0,S=0,G=0,U=0,R=0,k=0,N=0,H=0,u=0,J=0,d=0,v=0;e=I=e+-64|0,g=A+520|0,G=E[A+568>>2],U=E[A+572>>2],Q=I+16|0;A:for(;;){if(8!=(0|N))for(LA(Q,g,48),r=E[I+60>>2]^U,y=E[I+56>>2]^G,o=0,a=1634760805,f=857760878,t=2036477234,h=1797285236,w=E[I+44>>2],C=E[I+28>>2],F=E[I+40>>2],i=E[I+24>>2],R=E[I+36>>2],k=E[I+52>>2],D=E[I+20>>2],s=E[I+32>>2],S=E[I+48>>2],c=E[I+16>>2];;){if(10==(0|o)){for(E[I+48>>2]=S,E[I>>2]=a,E[I+16>>2]=c,E[I+32>>2]=s,E[I+20>>2]=D,E[I+52>>2]=k,E[I+4>>2]=f,E[I+36>>2]=R,E[I+24>>2]=i,E[I+56>>2]=y,E[I+8>>2]=t,E[I+40>>2]=F,E[I+28>>2]=C,E[I+60>>2]=r,E[I+12>>2]=h,E[I+44>>2]=w,C=0;o=4,4!=(0|C);)E[(i=(o=C<<2)+I|0)>>2]=E[i>>2]+E[o+26704>>2],C=C+1|0;for(;14!=(0|o);)E[(i=(C=o<<2)+I|0)>>2]=E[i>>2]+E[(C+g|0)-16>>2],o=o+1|0;for(E[I+56>>2]=E[I+56>>2]+(E[A+560>>2]^G),E[I+60>>2]=E[I+60>>2]+(E[A+564>>2]^U),D=N<<2,o=0;16!=(0|o);)C=D+(o<<5)|0,i=E[(o<<2)+I>>2],B[C+A|0]=i,B[(1|C)+A|0]=i>>>8,B[(2|C)+A|0]=i>>>16,B[(3|C)+A|0]=i>>>24,o=o+1|0;N=N+1|0,U=(G=G+1|0)?U:U+1|0;continue A}n=C,r=qA((C=C+h|0)^r,16),H=(P=qA(n^(w=r+w|0),12))+C|0,n=c,c=qA((C=c+a|0)^S,16),h=qA(n^(a=c+s|0),12),n=a,S=qA(c^(a=C+h|0),8),C=H+(h=qA(h^(s=n+S|0),7))|0,n=i,c=qA((i=i+t|0)^y,16),u=qA(n^(F=c+F|0),12),J=qA((i=u+i|0)^c,8),c=qA(C^J,16),n=D,y=qA((D=D+f|0)^k,16),d=qA(n^(f=y+R|0),12),n=C,C=qA((D=d+D|0)^y,8),t=qA((f=c+(v=C+f|0)|0)^h,12),y=qA(c^(h=n+t|0),8),c=qA((R=y+f|0)^t,7),n=i,r=qA(r^H,8),f=qA((i=r+w|0)^P,7),w=qA((t=n+f|0)^C,16),f=qA((C=w+s|0)^f,12),k=qA(w^(t=f+t|0),8),C=qA((s=C+k|0)^f,7),n=D,f=qA((D=F+J|0)^u,7),F=qA((w=n+f|0)^S,16),P=qA((i=F+i|0)^f,12),S=qA(F^(f=P+w|0),8),i=qA((w=i+S|0)^P,7),n=r,r=qA(d^v,7),F=qA(n^(a=r+a|0),16),P=qA((D=F+D|0)^r,12),r=qA(F^(a=P+a|0),8),D=qA((F=D+r|0)^P,7),o=o+1|0}break}E[A+512>>2]=0,E[A+568>>2]=G,E[A+572>>2]=U,e=I- -64|0}function s(A,I,g,B,Q,C,i,f){var D=0,o=0,n=0,r=0,a=0,w=0,e=0,c=0,t=0,h=0,y=0,S=0,U=0,R=0,k=0,N=0,H=0,u=0,d=0,Y=0;if(!i){for(Q=0,i=1048575&(C=E[B+4>>2])|1048576,C=((B=E[B>>2])&0-(f=1&(S=(y=C>>>20&2047)-1023|0)))+B|0,f=i+(0-(0!=(0|f))&i)|0,f=B>>>0>C>>>0?f+1|0:f,e=C,B=2097152,i=0;;){if(C=f<<1|(n=e)>>>31,f=n<<1,54==(0|Q)){Q=0-(B=y+2047>>>11|0)|0,r=0-(0!=(0|B))|0,B=QA(0,(S>>1)-54|0,((f=0!=(C|f))|(B=D)<<1)&Q,(C=i<<1|B>>>31)&r),Q=F;break}o=C,C=B+a|0,e=(t=f)-(R=(n=r+c|0)&(U=(f=o-((C=n>>>0<r>>>0?C+1|0:C)+(f>>>0<n>>>0)|0)>>>31|0)-1|0))|0,f=o-((C&(n=0-!f|0))+(t>>>0<R>>>0)|0)|0,C=a+((C=B<<1|(o=c)>>>31)&n)|0,C=(o=r+(o<<1&U)|0)>>>0<r>>>0?C+1|0:C,r=o,a=C,Q=Q+1|0,C=i+(B&n)|0,C=(o=D+(c&U)|0)>>>0<D>>>0?C+1|0:C,D=o,i=C,c=(1&B)<<31|c>>>1,B=B>>>1|0}return B=j(B,Q,-1066729002,1064818311),Q=F,C=P(A,E[I>>2],E[I+4>>2],B,Q),E[I>>2]=M(C,C>>31),E[I+4>>2]=F,A=P(A,E[g>>2],E[g+4>>2],B,Q),E[g>>2]=M(A,A>>31),void(E[g+4>>2]=F)}for(U=1<<i>>>1|0;(0|r)!=(0|U);)H=E[(D=y=(t=r+U<<3)+C|0)>>2],u=E[D+4>>2],d=E[(D=S=(w=r<<3)+C|0)>>2],Y=E[D+4>>2],n=E[(D=R=Q+t|0)>>2],o=E[D+4>>2],D=E[(a=N=Q+w|0)>>2],a=E[a+4>>2],t=E[(c=B+t|0)>>2],c=E[c+4>>2],w=E[(e=B+w|0)+4>>2],e=j(e=E[e>>2],w,w=K(G(k=OA(e,w),F,OA(t,c),F),F),h=F),k=F,c=j(mA(t,c),F,w,h),h=F,t=lA(j(D,a,e,k),F,j(n,o,c,h),F),w=F,c=G(j(D,a,c,h),F,j(n,o,e,k),F),e=F,n=mA(n,o),o=F,k=lA(j(t,w,D,a),F,j(c,e,n,o),F),h=F,D=G(j(t,w,n,o),F,j(c,e,D,a),F),a=F,n=lA(d,Y,k,h),o=F,D=lA(H,u,D,a),E[S>>2]=n,E[S+4>>2]=o,E[y>>2]=D,E[y+4>>2]=F,E[N>>2]=t,E[N+4>>2]=w,E[R>>2]=mA(c,e),E[R+4>>2]=F,r=r+1|0;J(f,D=(r=(o=1<<i)<<2&-8)+f|0,B,i),n=LA(B,f,a=8<<i),J(f,D,C,i),C=LA(C,f,a),B=LA(f,Q,a),t=LA(Q,n,r),w=LA(t+r|0,C,r),J(Q=(o<<=3)+B|0,f=Q+r|0,g,i),s(A,Q,f,C,C+r|0,e=w,w=i-1|0,Q+o|0),v(C=B+(2<<i<<3)|0,Q,f,i),eA(Q=LA(Q,g,a),C,i),LA(g,C,a),V(B,Q,i),FA(I,B,i),J(B,D,I,i),s(A,B,D,n,n+r|0,t,w,Q),v(I,B,D,i)}function S(A,I,g,B,Q){var C,i,f,D,n,r,a,w,e,F,c=0,t=0,h=0,P=0,y=0,s=0,S=0,G=0,U=0,k=0,N=0;for(i=E[(g<<=2)+24468>>2],y=(F=(P=o(i,r=(n=1<<I)>>>1|0)<<2)+A|0)+P|0,P=E[g+24464>>2],C=(D=(f=(w=(a=hA(y,A,o(P,8<<I)))+(g=P<<I<<2)|0)+g|0)+(g=n<<2)|0)+g|0,e=I-1|0;(0|P)!=(0|s);){for(g=o(s,12),t=E[g+18192>>2],l(f,D,I,g=E[g+18196>>2],t,h=JA(t)),c=0,g=U=(y=s<<2)+a|0;(0|c)!=(0|n);)E[(c<<2)+C>>2]=E[g>>2],c=c+1|0,g=(P<<2)+g|0;for(S=RA(t,h),B||p(C,1,f,I,t,h),c=0,g=k=A+y|0;(0|c)!=(0|r);)G=(c<<3)+C|0,E[g>>2]=aA(aA(E[G>>2],E[G+4>>2],t,h),S,t,h),c=c+1|0,g=(i<<2)+g|0;for(B&&q(U,P,D,I,t,h),c=0,g=U=y+w|0;(0|c)!=(0|n);)E[(c<<2)+C>>2]=E[g>>2],c=c+1|0,g=(P<<2)+g|0;for(B||p(C,1,f,I,t,h),c=0,g=y=y+F|0;(0|c)!=(0|r);)G=(c<<3)+C|0,E[g>>2]=aA(aA(E[G>>2],E[G+4>>2],t,h),S,t,h),c=c+1|0,g=(i<<2)+g|0;B&&q(U,P,D,I,t,h),Q||(q(k,i,D,e,t,h),q(y,i,D,e,t,h)),s=s+1|0}R(a,P,P,n,1,f),R(w,P,P,n,1,f),U=P>>>0>i>>>0?P:i,B=P;A:for(;;){if((0|B)!=(0|U)){for(g=o(B,12),S=yA(P,t=E[g+18192>>2],h=JA(t),s=RA(t,h)),l(f,D,I,E[g+18196>>2],t,h),c=0,g=a;(0|c)!=(0|n);)E[(c<<2)+C>>2]=xA(g,P,t,h,s,S),c=c+1|0,g=(P<<2)+g|0;for(p(C,1,f,I,t,h),c=0,y=G=(k=B<<2)+A|0,g=0;;){if((0|g)==(0|r)){for(g=w;(0|c)!=(0|n);)E[(c<<2)+C>>2]=xA(g,P,t,h,s,S),c=c+1|0,g=(P<<2)+g|0;for(p(C,1,f,I,t,h),c=0,g=y=k+F|0;(0|c)!=(0|r);)S=(c<<3)+C|0,E[g>>2]=aA(aA(E[S>>2],E[S+4>>2],t,h),s,t,h),c=c+1|0,g=(i<<2)+g|0;Q||(q(G,i,D,e,t,h),q(y,i,D,e,t,h)),B=B+1|0;continue A}N=(g<<3)+C|0,E[y>>2]=aA(aA(E[N>>2],E[N+4>>2],t,h),s,t,h),g=g+1|0,y=(i<<2)+y|0}}break}}function G(A,I,g,B){var Q,C,E,i,f=0,D=0,o=0,n=0,r=0,a=0;return f=(D=A)^(A=0-(o=((0|A)==(0|g)&(0|(f=(2147483647&I)-((2147483647&B)+(A>>>0<g>>>0)|0)|0))<=0|(0|f)<0)&I>>>31|f>>>31)&(A^g)),o=I^=n=0-((0|n)+(0!=(0|o))|0)&(I^B),Q=(D=I>>>20|0)>>>11|0,B^=n,n=A^g,A=(g=DA(1,0,C=63&(D=(I=2047&D)-(r=2047&(a=B>>>20|0))|0)))-1|0,E=0-(D=D-60>>>31|0)|0,i=0-(0!=(0|D))|0,n=B<<3|(D=n)>>>29,B=D<<3&-8,D=8388607&n,n=B,g=((g=F-!g|0)&(B=((B=r+2047>>>11<<23)|D)&i))+g|0,A=wA((n=A+((r=n&E)&A)|0)|r,B|(A>>>0>n>>>0?g+1|0:g),C),a=0-(0!=(0|(B=Q^a>>>11)))|0,D=A-(B=A<<1&(D=0-B|0))|0,B=(n=g=F)-(((g=g<<1|A>>>31)&a)+(A>>>0<B>>>0)|0)|0,f=8388607&(g=o<<3|(A=f)>>>29),o=A=A<<3&-8,g=A+D|0,A=(I+2047>>>11<<23|f)+B|0,o=((f=0!=(0|(A=g>>>0<o>>>0?A+1|0:A)))<<5)+I|0,I=f?g:0,f=(B=(A=f?A:g)>>>0>65535)?I:I<<16,o=((g=(A=B?A:A<<16|I>>>16)>>>0>16777215)<<3)+(o=(B<<4)+o|0)|0,I=A,B=A<<8|f>>>24,A=g?f:f<<8,B=(g=(I=g?I:B)>>>0>268435455)?A:A<<4,o=f=(g<<2)+o|0,f=(A=g?I:I<<4|A>>>28)>>>0>1073741823,g=A,A=A<<2|(I=B)>>>30,I=f?I:I<<2,f=((g=(A=f?g:A)>>>31|0)+(o=o+(f<<1)|0)|0)-1132|0,o=0-!g|0,B=I^(B=g-1|0)&(I^I<<1),A^=(A^(g=A<<1|I>>>31))&o,I=0,A=QA(Q,f,(511&(A|=I=(g=511+(511&B)|0)>>>0<511?I+1|0:I))<<23|(I=g|B)>>>9,A>>>9|0)}function U(A,I,g,B,Q,C,i,f,D,n,r){var a,w,e,F,c,t=0,h=0,P=0,y=0,s=0,S=0,G=0,U=0,k=0;for(e=(s=(F=(P=(w=1<<n)<<2)+r|0)+P|0)+((a=Q+1|0)<<n<<2)|0,c=C<<2;(0|a)!=(0|S);){for(C=o(S,12),k=yA(Q,t=E[C+18192>>2],y=JA(t),G=RA(t,y)),l(r,F,n,E[C+18196>>2],t,y),h=0;(0|h)!=(0|w);)E[(C=h<<2)+e>>2]=_A(E[C+i>>2],t),h=h+1|0;for(p(e,1,r,n,t,y),h=0,C=B,U=P=(S<<2)+s|0;(0|h)!=(0|w);)E[U>>2]=xA(C,Q,t,y,G,k),h=h+1|0,U=(a<<2)+U|0,C=C+c|0;for(p(P,a,r,n,t,y),h=0,C=P;(0|h)!=(0|w);)E[C>>2]=aA(aA(E[(h<<2)+e>>2],E[C>>2],t,y),G,t,y),h=h+1|0,C=(a<<2)+C|0;q(P,a,F,n,t,y),S=S+1|0}for(R(s,a,a,w,1,e),C=0,r=g<<2;(0|C)!=(0|w);){B=A,i=0,n=0;A:if(a)for(P=I>>>0<f>>>0?f:I,h=31-D|0,Q=0-(E[((a<<2)+s|0)-4>>2]>>>30|0)>>>1|0,A=f;;){if((0|A)==(0|P))break A;g=Q,(t=A-f|0)>>>0<a>>>0&&(g=E[(t<<2)+s>>2]),i=(E[(t=B+(A<<2)|0)>>2]-(g<<D&2147483647|n)|0)+i|0,E[t>>2]=2147483647&i,i>>=31,A=A+1|0,n=g>>>h|0}C=C+1|0,s=(a<<2)+s|0,A=B+r|0}}function R(A,I,g,B,Q,C){var i,f=0,D=0,n=0,r=0,a=0,w=0,e=0,c=0,t=0,h=0,P=0,y=0,s=0,S=0;for(E[C>>2]=2147473409,i=I>>>0>1?I:1,D=1;(0|D)!=(0|i);){for(f=o(D,12),y=E[f+18200>>2],c=0,s=RA(a=E[f+18192>>2],t=JA(a)),r=A;(0|B)!=(0|c);){for(f=0,S=aA(y,VA(E[(D<<2)+r>>2],UA(r,D,a,t,s),a),a,t),w=0;(0|D)!=(0|w);)h=n=(e=w<<2)+r|0,P=0,P=(n=f+E[n>>2]|0)>>>0<f>>>0?1:P,f=(e=iA(E[C+e>>2],0,S,0))+n|0,n=F+P|0,n=f>>>0<e>>>0?n+1|0:n,E[h>>2]=2147483647&f,f=(2147483647&n)<<1|f>>>31,w=w+1|0;E[(D<<2)+r>>2]=f,c=c+1|0,r=(g<<2)+r|0}E[(D<<2)+C>>2]=CA(C,D,a),D=D+1|0}A:if(Q)for(r=0,a=g<<2;;){if((0|B)==(0|r))break A;for(g=0,D=0,Q=I;Q;)g=(n=g)|(h=(1&g)-1|0)&(0-(g=(D<<30|(f=E[(g=(Q=Q-1|0)<<2)+C>>2])>>>1)-E[A+g>>2]|0)>>>31|g>>31),D=1&f;for(Q=0,D=0-(g>>>31|0)|0,g=0;(0|I)!=(0|g);)f=(n=Q)+((Q=E[(w=(f=g<<2)+A|0)>>2])-E[C+f>>2]|0)|0,E[w>>2]=Q^D&(Q^2147483647&f),Q=f>>31,g=g+1|0;r=r+1|0,A=A+a|0}}function k(A,I){var g,B,Q=0,C=0,i=0,f=0,D=0,o=0,n=0,r=0,a=0,w=0,e=0,c=0,t=0,h=0,P=0,y=0,s=0,S=0,U=0,R=0,k=0,N=0,H=0,u=0,J=0,d=0,v=0;for(B=(g=1<<I)>>>1|0,t=g,o=1,h=I;h>>>0>=2;){for(a=o<<1,t=t>>>1|0,f=0,P=0;f>>>0<B>>>0;){for(u=f+o|0,y=E[(Q=1488+(t+P<<4)|0)>>2],s=E[Q+4>>2],S=mA(E[Q+8>>2],E[Q+12>>2]),U=F,Q=f;Q>>>0<u>>>0;)n=E[(C=i=(Q+B<<3)+A|0)>>2],r=E[C+4>>2],e=E[(w=((C=Q+o|0)+B<<3)+A|0)>>2],c=E[w+4>>2],J=G(R=E[(D=(Q<<3)+A|0)>>2],k=E[D+4>>2],N=E[(C=(C<<3)+A|0)>>2],H=E[C+4>>2]),d=F,v=G(n,r,e,c),E[D>>2]=J,E[D+4>>2]=d,E[i>>2]=v,E[i+4>>2]=F,i=lA(R,k,N,H),D=F,n=lA(n,r,e,c),r=F,e=lA(j(i,D,y,s),F,j(n,r,S,U),F),c=F,i=G(j(i,D,S,U),F,j(n,r,y,s),F),E[C>>2]=e,E[C+4>>2]=c,E[w>>2]=i,E[w+4>>2]=F,Q=Q+1|0;f=f+a|0,P=P+1|0}h=h-1|0,o=a}A:if(I)for(o=E[(I=17872+(I<<3)|0)>>2],a=E[I+4>>2],Q=0;;){if((0|Q)==(0|g))break A;f=E[(I=(Q<<3)+A|0)>>2],E[I>>2]=j(f,E[I+4>>2],o,a),E[I+4>>2]=F,Q=Q+1|0}}function N(A,I,g){var B,Q,E,D=0,o=0,n=0,r=0,a=0,w=0,F=0,c=0;for(e=B=e-144|0,r=1,E=(Q=f[682])+1024|0;;){if((0|D)==(0|E)){for(;;){if(w=0,D=0,r>>>0>Q>>>0){e=B+144|0;break}for(;(0|D)!=(0|E);)A=(D<<1)+I|0,D>>>0<1024||(A=(D-1024<<1)+g|0,D>>>0<2048||(A=((D<<1)+B|0)-4080|0)),F=((n=f[A>>1])>>>15|0)-1|0,D>>>0>=r>>>0&&(c=A,(o=D-r|0)>>>0<1024?a=(o<<1)+I|0:(a=(o-1024<<1)+g|0,o>>>0<2048||(a=((o<<1)+B|0)-4080|0)),o=0-(511+(D-w&r)>>>9|0)&F&(n^(A=f[a>>1])),C[c>>1]=n^o,C[a>>1]=A^o),w=w-F|0,D=D+1|0;r<<=1}break}L(B+14|0,2,A),n=(o=(o=(n=i[B+15|0]|i[B+14|0]<<8)-((n-24578>>>31|0)-1&24578)|0)-((o-24578>>>31|0)-1&24578)|0)-((o-12289>>>31|0)-1&12289)|(n-61445>>>31|0)-1,D>>>0<1024?C[(D<<1)+I>>1]=n:D>>>0<2048?C[(D-1024<<1)+g>>1]=n:C[((D<<1)+B|0)-4080>>1]=n,D=D+1|0}}function H(A,I){var g,B,Q=0,C=0,i=0,f=0,D=0,o=0,n=0,r=0,a=0,w=0,e=0,c=0,t=0,h=0,P=0,y=0,s=0,S=0,U=0,R=0,k=0,N=0,H=0,u=0,J=0,d=0,v=0;for(B=I>>>0>1?I:1,o=2,t=g=1<<I>>>1|0,h=1;(0|h)!=(0|B);){for(J=o>>>1|0,P=t>>>1|0,n=0,r=0;(0|r)!=(0|J);){for(d=n+P|0,y=E[(I=1488+(o+r<<4)|0)>>2],s=E[I+4>>2],S=E[I+8>>2],U=E[I+12>>2],I=n;I>>>0<d>>>0;)a=E[(Q=C=(I+g<<3)+A|0)>>2],R=E[Q+4>>2],k=E[(Q=i=(I<<3)+A|0)>>2],N=E[Q+4>>2],f=E[(w=((Q=I+P|0)+g<<3)+A|0)>>2],D=E[w+4>>2],H=lA(j(e=E[(Q=(Q<<3)+A|0)>>2],c=E[Q+4>>2],y,s),F,j(f,D,S,U),F),u=F,f=G(j(e,c,S,U),F,j(f,D,y,s),F),D=F,e=G(k,N,H,u),c=F,v=G(a,R,f,D),E[i>>2]=e,E[i+4>>2]=c,E[C>>2]=v,E[C+4>>2]=F,C=lA(k,N,H,u),i=F,a=lA(a,R,f,D),E[Q>>2]=C,E[Q+4>>2]=i,E[w>>2]=a,E[w+4>>2]=F,I=I+1|0;n=n+t|0,r=r+1|0}o<<=1,h=h+1|0,t=P}}function u(A,I,g){var B,Q=0,C=0,f=0,D=0,o=0,n=0,r=0,a=0;for(B=E[A>>2];;){A:{if(f=0,A=0,Q=o=E[B+204>>2],!(!(Q=(D=g+(C=E[B+200>>2])|0)>>>0<g>>>0?Q+1|0:Q)&D>>>0<136)){for(;!A&f>>>0<(Q=136-C|0)>>>0;)a=E[(Q=(-8&(D=(Q=C)+f|0))+B|0)+4>>2],o=i[I+f|0],D=31&(C=D<<3),(63&C)>>>0>=32?(C=o<<D,o=0):(C=(1<<D)-1&o>>>32-D,o<<=D),E[Q>>2]=o^E[Q>>2],E[Q+4>>2]=C^a,A=(C=f+1|0)?A:A+1|0,f=C,C=E[B+200>>2],o=E[B+204>>2];E[B+200>>2]=0,E[B+204>>2]=0,t(B),I=I+Q|0,g=g-Q|0;continue}for(;;){if(!n&(0|g)==(0|r))break A;f=A=r+E[B+200>>2]|0,D=E[(A=(-8&A)+B|0)+4>>2],Q=i[I+r|0],C=31&(f<<=3),(63&f)>>>0>=32?(f=Q<<C,Q=0):(f=(1<<C)-1&Q>>>32-C,Q<<=C),E[A>>2]=Q^E[A>>2],E[A+4>>2]=D^f,A=n,r=n=r+1|0,n=A=n?A:A+1|0}}break}A=E[B+204>>2],A=(I=g+E[B+200>>2]|0)>>>0<g>>>0?A+1|0:A,E[B+200>>2]=I,E[B+204>>2]=A}function J(A,I,g,B){var Q=0,C=0,i=0,f=0,D=0,o=0,n=0,r=0,a=0,w=0,e=0,c=0,t=0,h=0,P=0,y=0;for(n=E[g+4>>2],E[A>>2]=E[g>>2],E[A+4>>2]=n,e=E[(Q=g+((n=(B=1<<B)>>>1|0)<<3)|0)+4>>2],E[I>>2]=E[Q>>2],E[I+4>>2]=e,e=B>>>2|0,B=0;(0|B)!=(0|e);)Q=E[(f=((C=B<<1)+n<<3)+g|0)>>2],f=E[f+4>>2],C=E[(D=((i=1|C)+n<<3)+g|0)>>2],D=E[D+4>>2],c=G(a=E[(o=(B<<4)+g|0)>>2],o=E[o+4>>2],w=E[(i=(i<<3)+g|0)>>2],i=E[i+4>>2]),r=F,t=G(Q,f,C,D),y=F,E[(P=(h=B<<3)+A|0)>>2]=SA(c,r),E[P+4>>2]=F,E[(r=(c=B+e<<3)+A|0)>>2]=SA(t,y),E[r+4>>2]=F,i=lA(a,o,w,i),o=F,f=lA(Q,f,C,D),C=F,D=mA(E[(Q=1488+(B+n<<4)|0)+8>>2],E[Q+12>>2]),a=F,r=lA(j(i,o,w=E[Q>>2],Q=E[Q+4>>2]),F,j(f,C,D,a),F),t=F,Q=G(j(i,o,D,a),F,j(f,C,w,Q),F),f=F,E[(C=I+h|0)>>2]=SA(r,t),E[C+4>>2]=F,E[(C=I+c|0)>>2]=SA(Q,f),E[C+4>>2]=F,B=B+1|0}function d(A,I,g,B,Q,C,i,f,D,n,r,a,w){var e,c,t=0,h=0,P=0,y=0,s=0,S=0,G=0,U=0,R=0,k=0;for(t=E[I>>2],G=E[A>>2],e=2147483647&o(o(t,a)+o(n,G)|0,Q),c=2147483647&o(o(f,t)+o(C,G)|0,Q),Q=0;(0|B)!=(0|Q);)h=iA(U=E[(G=Q<<2)+A>>2],0,n,r)+s|0,t=P+F|0,t=s>>>0>h>>>0?t+1|0:t,s=iA(R=E[I+G>>2],0,a,w),t=F+t|0,t=s>>>0>(P=s+h|0)>>>0?t+1|0:t,s=(h=iA(k=E[g+G>>2],0,e,0))+P|0,P=F+t|0,P=s>>>0<h>>>0?P+1|0:P,h=iA(C,i,U,0)+S|0,t=y+F|0,t=S>>>0>h>>>0?t+1|0:t,S=iA(f,D,R,0),t=F+t|0,t=S>>>0>(y=S+h|0)>>>0?t+1|0:t,S=(h=iA(c,0,k,0))+y|0,y=F+t|0,y=S>>>0<h>>>0?y+1|0:y,Q&&(E[(t=G-4|0)+A>>2]=2147483647&S,E[I+t>>2]=2147483647&s),Q=Q+1|0,s=(2147483647&P)<<1|s>>>31,P>>=31,S=(2147483647&y)<<1|S>>>31,y>>=31;E[(Q=(B<<2)-4|0)+A>>2]=S,E[I+Q>>2]=s,AA(A,B,g,y>>>31|0),AA(I,B,g,P>>>31|0)}function v(A,I,g,B){var Q=0,C=0,i=0,f=0,D=0,o=0,n=0,r=0,a=0,w=0,e=0,c=0,t=0,h=0,P=0,y=0;for(n=E[I+4>>2],E[A>>2]=E[I>>2],E[A+4>>2]=n,e=E[g+4>>2],E[(Q=((n=(B=1<<B)>>>1|0)<<3)+A|0)>>2]=E[g>>2],E[Q+4>>2]=e,e=B>>>2|0,B=0;(0|B)!=(0|e);)c=E[(C=(Q=B+e<<3)+I|0)>>2],C=E[C+4>>2],t=E[(i=(f=B<<3)+I|0)>>2],i=E[i+4>>2],r=E[(Q=g+Q|0)>>2],a=E[Q+4>>2],f=j(D=E[(Q=g+f|0)>>2],o=E[Q+4>>2],h=E[(Q=1488+(B+n<<4)|0)>>2],w=E[Q+4>>2]),P=E[Q+8>>2],y=E[Q+12>>2],Q=lA(f,F,j(r,a,P,y),F),f=F,r=G(j(D,o,P,y),F,j(r,a,h,w),F),a=F,D=G(t,i,Q,f),o=F,h=G(c,C,r,a),E[(w=(B<<4)+A|0)>>2]=D,E[w+4>>2]=o,E[(o=((D=B<<1)+n<<3)+A|0)>>2]=h,E[o+4>>2]=F,Q=lA(t,i,Q,f),t=F,c=lA(c,C,r,a),E[(i=((C=1|D)<<3)+A|0)>>2]=Q,E[i+4>>2]=t,E[(Q=(C+n<<3)+A|0)>>2]=c,E[Q+4>>2]=F,B=B+1|0}function Y(A,I,g,B,Q,C,i,f,D,n){var r,a,w,e=0,c=0,t=0,h=0,P=0,y=0,s=0,S=0,G=0,U=0,R=0,k=0,N=0,H=0,u=0;for(a=(r=1<<n)-1|0,w=C<<2;(0|P)!=(0|r);){for(S=(o(g,P)<<2)+A|0,n=0,C=0-E[(P<<2)+i>>2]|0,h=B;(0|n)!=(0|r);){G=h,y=C,c=0,t=0;A:if(Q)for(U=I>>>0<f>>>0?f:I,R=31-D|0,k=0-(E[((Q<<2)+G|0)-4>>2]>>>30|0)>>>1|0,N=y>>31,C=f;;){if((0|C)==(0|U))break A;h=k,(e=C-f|0)>>>0<Q>>>0&&(h=E[(e<<2)+G>>2]),H=e=(C<<2)+S|0,u=E[e>>2],s=(e=c)>>31,s=e>>>0>(c=u+e|0)>>>0?s+1|0:s,e=iA(h<<D&2147483647|t,0,y,N),t=F+s|0,t=e>>>0>(c=e+c|0)>>>0?t+1|0:t,e=c,E[H>>2]=2147483647&e,c=(2147483647&t)<<1|e>>>31,C=C+1|0,t=h>>>R|0}S=(C=(n+P|0)==(0|a))?A:(g<<2)+S|0,C=C?0-y|0:y,n=n+1|0,h=G+w|0}P=P+1|0}}function L(A,I,g){var Q,C=0,i=0,f=0,D=0,o=0,n=0,r=0;Q=E[g>>2],D=I;A:{for(;;){if(C=E[Q+200>>2],i=E[Q+204>>2],!o&(0|f)==(0|D)){g=I;break A}if(!((0|o)==(0|i)&C>>>0>(g=f)>>>0|o>>>0<i>>>0))break;r=A+g|0,n=E[(C=(-8&(i=n=f+(136-C|0)|0))+Q|0)>>2],C=E[C+4>>2],g=31&(i<<=3),B[0|r]=(63&i)>>>0>=32?C>>>g|0:((1<<g)-1&C)<<32-g|n>>>g,o=(g=f+1|0)?o:o+1|0,f=g}D=f}for(f=C-D|0,o=i-((C>>>0<D>>>0)+0|0)|0,I=I-g|0,A=A+g|0;E[Q+200>>2]=f,E[Q+204>>2]=o,I;){for(t(Q),f=I>>>0<136?I:136,g=0;(0|g)!=(0|f);)C=E[(D=(-8&g)+Q|0)>>2],o=E[D+4>>2],D=31&(i=g<<3&56),B[A+g|0]=(63&i)>>>0>=32?o>>>D|0:((1<<D)-1&o)<<32-D|C>>>D,g=g+1|0;I=I-f|0,A=A+f|0,f=136-f|0,o=0}}function j(A,I,g,B){var Q,C,E,i=0,f=0,D=0,o=0,n=0;return Q=(I^B)>>>31|0,n=iA(i=134217727&((33554431&B)<<7|g>>>25)|134217728,0,D=134217727&((33554431&I)<<7|A>>>25)|134217728,0),C=F,f=iA(i,f,A&=33554431,0),E=F,i=iA(i=D,o,D=33554431&g,0),g=F,o=iA(A,o,D,o),A=n,D=((33554431&E)<<7|f>>>25)+((33554431&g)<<7|i>>>25)+((n=((33554431&i)+((33554431&F)<<7|o>>>25)|0)+(33554431&f)|0)>>>25)|0,f=C,f=(i=A+D|0)>>>0<D>>>0?f+1|0:f,D=i,g=((f=(i=f)>>>23|0)+((A=I>>>20&2047)+(I=B>>>20&2047)|0)|0)-2100|0,I=0-(A=(I+2047&A+2047)>>>11|0)|0,B=0-(0!=(0|A))|0,A=QA(Q,g,(0-f&((1&(A=33554431+(33554431&(o|n))>>>25|D)|(1&i)<<31|D>>>1)^A)^A)&I,(i^0-(0!=(0|f))&(i^i>>>1))&B)}function M(A,I){var g,B=0,Q=0,C=0,E=0,i=0,f=0;return i=(g=I>>>31|0)+(A^(B=I>>31))|0,A=I^B,C=(E=0!=(0|(A=i>>>0<g>>>0?A+1|0:A)))<<5,I=A,Q=(A=E?A:i)<<16|(B=E?i:0)>>>16,B=(E=A>>>0>65535)?B:B<<16,E=((f=(A=E?A:Q)>>>0>16777215)<<3)+(C=(E<<4)+C|0)|0,C=B,B=A<<8|B>>>24,B=(Q=E)+((E=(A=f?A:B)>>>0>268435455)<<2)|0,Q=A<<4|(C=f?C:C<<8)>>>28,C=E?C:C<<4,f=((Q=(A=E?A:Q)>>>0>1073741823)<<1)+B|0,B=Q?C:C<<2,C=A=Q?A:A<<2|C>>>30,Q=((A=A>>>31|0)+f|0)-54|0,E=(f=!(I|i))?0:Q,I=0-!A|0,i=A-1|0,I=(A=C)^(A^(Q=A<<1|B>>>31))&I,Q=0,Q=(A=511+(511&(i=B^i&(B^B<<1)))|0)>>>0<511?Q+1|0:Q,A|=i,Q=(I|=Q)>>>9|0,A=QA(g,E,f?0:(511&I)<<23|A>>>9,f?0:Q)}function x(A,I){for(var g=0,Q=0,C=0,i=0,f=0,D=0,o=0,n=0,r=0,a=0,w=0,e=0;1024!=(0|o);){A:{for(;;){for(n=0,Q=0;!n;){for(g=nA(A),i=C=F,C=C>>>31|0,a=nA(A),w=2147483647&F,i=(2147483647&i)-((g>>>0<770878296)+298923992|0)>>>31|0,g=1,f=0;27!=(0|g);)e=f,f=E[(D=17968+(g<<3)|0)>>2],f=e|0-((D=!(w-(E[D+4>>2]+(f>>>0>a>>>0)|0)>>>31|0))&(1^i))&g,g=g+1|0,i|=D;Q=(Q+C|0)+(0-C^f)|0,n=n+1|0}if(!(Q-128>>>0<4294967041)){if(g=1&Q,1023==(0|o)){if((0|g)==(0|r))continue;break A}break}}r^=g}B[I+o|0]=Q,o=o+1|0}}function b(A){var I=0,g=0,B=0,Q=0,E=0,i=0,D=0,o=0,n=0,r=0,a=0,w=0;B=1024,g=1;A:for(;;){if(B>>>0<2){for(g=4091,I=1024;;){if(I>>>0<2){I:for(I=0;;){if(1024==(0|I))break I;C[(Q=(I<<1)+A|0)>>1]=jA(f[Q>>1],g),I=I+1|0}break}I=I>>>1|0,g=(0-(1&g)&12289)+g>>>1|0}return}for(Q=g<<1,B=B>>>1|0,i=0,D=0;;){if((0|B)==(0|D)){g=Q;continue A}for(r=g+i|0,a=f[28992+(B+D<<1)>>1],I=i;I>>>0<r>>>0;)o=f[(E=(I<<1)+A|0)>>1],w=E,n=f[(E=(I+g<<1)+A|0)>>1],C[w>>1]=KA(o,n),C[E>>1]=jA(XA(o,n),a),I=I+1|0;i=Q+i|0,D=D+1|0}}}function q(A,I,g,B,Q,C){var i=0,f=0,D=0,n=0,r=0,a=0,w=0,e=0,F=0,c=0,t=0,h=0,P=0;A:if(B){f=F=1<<B,n=1;I:for(;;){if(f>>>0>=2)for(h=o(I,n),c=n<<1,f=f>>>1|0,r=0,w=0;;){if((0|f)==(0|r)){n=c;continue I}for(a=(i=(o(I,w)<<2)+A|0)+(h<<2)|0,P=E[(f+r<<2)+g>>2],e=0;(0|n)!=(0|e);)D=E[i>>2],t=E[a>>2],E[i>>2]=pA(D,t,Q),E[a>>2]=aA(VA(D,t,Q),P,Q,C),e=e+1|0,a=(D=I<<2)+a|0,i=i+D|0;w=w+c|0,r=r+1|0}break}for(g=-2147483648>>>B|0,i=0,I<<=2;;){if((0|i)==(0|F))break A;E[A>>2]=aA(E[A>>2],g,Q,C),i=i+1|0,A=A+I|0}}}function z(A,I,g,B,Q){var C,i=0,f=0,D=0,o=0,n=0,r=0,a=0,w=0,e=0,c=0;C=1<<Q;A:{if(!g)for(Q=0;;){if((0|Q)==(0|C))break A;E[(I=(Q<<3)+A|0)>>2]=0,E[I+4>>2]=0,Q=Q+1|0}for(w=g-1<<2,e=B<<2;;){if((0|D)==(0|C))break A;for(Q=0,a=1&(o=0-(E[I+w>>2]>>>30|0)|0),c=o>>>1|0,B=0,n=1072693248,i=0,r=0;(0|g)!=(0|Q);)f=Q<<2,Q=Q+1|0,a=(f=(E[I+f>>2]^c)+a|0)>>>31|0,i=G(i,r,j(M(i=(2147483647&f)-(o&f<<1)|0,i>>31),F,B,n),F),r=F,B=j(B,n,0,1105199104),n=F;E[(B=(D<<3)+A|0)>>2]=i,E[B+4>>2]=r,D=D+1|0,I=I+e|0}}}function T(A,I,g,Q,C){var i,f=0,D=0;for(i=A+4096|0;1024!=(0|f);)E[(D=f<<2)+A>>2]=_A(B[I+f|0],2147473409),E[i+D>>2]=_A(B[g+f|0],2147473409),f=f+1|0;if(!(!C|Q))return l(I=i+4096|0,I+4096|0,10,383167813,2147473409,g=JA(2147473409)),p(A,1,I,10,2147473409,g),void p(i,1,I,10,2147473409,g);if(Q){if(1==(0|Q))return void S(A,10,0,0,C);for(S(A,10,0,0,1),I=Q-1|0,f=1;(0|I)!=(0|f);)S(A,10-f|0,f,1,1),f=f+1|0;S(A,11-Q|0,I,1,C)}}function K(A,I){var g,B=0,Q=0,C=0,E=0,i=0,f=0,D=0,o=0;for(g=1048575&I|1048576,C=1048576;55!=(0|f);)f=f+1|0,D=0-!(B=C-((A>>>0>Q>>>0)+g|0)>>>31|0)|0,o=(B=o|1&(E=B-1|0))<<1,i=i<<1|B>>>31,B=(B=(B=C)-((D&g)+((C=A&E)>>>0>Q>>>0)|0)|0)<<1|(Q=Q-C|0)>>>31,Q<<=1,C=B;return A=QA(f=(1072693248^I)>>>31|0,968+((E=i>>>23|0)-(I>>>20&2047)|0)|0,(A=B=0-E|0)&((Q=(B=0!=(Q|C))|o)^(B|(1&i)<<31|o>>>1))^Q,i^(i>>>1^i)&(D=0-(0!=(0|E))|0))}function l(A,I,g,B,Q,C){var i,D,o,n=0,r=0,a=0,w=0;for(n=g>>>0>10?g:10,r=aA(B,w=RA(Q,C),Q,C),B=g;(0|B)!=(0|n);)B=B+1|0,r=aA(r,r,Q,C);for(D=10-g|0,B=0,i=r,n=r=$A(Q),o=Q-2|0,a=30;(0|a)>=0;)n=aA(n,n,Q,C),n=0-(o>>>a&1)&(aA(n,i,Q,C)^n)^n,a=a-1|0;for(a=aA(w,aA(n,1,Q,C),Q,C),n=r;!(B>>>g|0);)w=f[24656+(B<<D<<1)>>1]<<2,E[w+A>>2]=r,E[I+w>>2]=n,B=B+1|0,r=aA(r,i,Q,C),n=aA(n,a,Q,C)}function p(A,I,g,B,Q,C){var i=0,f=0,D=0,n=0,r=0,a=0,w=0,e=0,F=0,c=0,t=0,h=0;A:if(B)for(i=1,r=F=1<<B;;){if(i>>>0>=F>>>0)break A;for(t=o(a=r>>>1|0,I),D=0,w=0;(0|i)!=(0|D);){for(n=(B=(o(I,w)<<2)+A|0)+(t<<2)|0,h=E[(i+D<<2)+g>>2],e=0;(0|e)!=(0|a);)f=E[B>>2],c=aA(E[n>>2],h,Q,C),E[B>>2]=pA(f,c,Q),E[n>>2]=VA(f,c,Q),e=e+1|0,n=(f=I<<2)+n|0,B=B+f|0;w=r+w|0,D=D+1|0}i<<=1,r=a}}function W(A,I,g,Q){var C=0,E=0,f=0,D=0,o=0;D=-1^(E=-1<<Q-1);A:{for(;;){if(1024!=(0|C)){if((0|E)>=(0|(o=B[g+C|0])))break A;if(C=C+1|0,(0|D)>=(0|o))continue;break A}break}if(E=7+(Q<<10)>>>3|0,A){if(I>>>0<E>>>0)break A;for(D=-1<<Q^-1,I=0,C=0;1024!=(0|f);){for(I=D&i[g+f|0]|I<<Q,C=Q+C|0;C>>>0>=8;)C=C-8|0,B[0|A]=I>>>C,A=A+1|0;f=f+1|0}C&&(B[0|A]=I<<8-C)}f=E}return f}function X(A,I,g){var B=0,Q=0,C=0,E=0;return Q=31&(C=E=63&g),C=C>>>0>=32?-1>>>Q|0:(B=-1>>>Q|0)|(1<<Q)-1<<32-Q,C&=A,B&=I,Q=31&E,E>>>0>=32?(B=C<<Q,E=0):(B=(1<<Q)-1&C>>>32-Q|B<<Q,E=C<<Q),C=B,B=31&(Q=0-g&63),Q>>>0>=32?(B=-1<<B,g=0):B=(g=-1<<B)|(1<<B)-1&-1>>>32-B,A&=g,I&=B,B=31&Q,Q>>>0>=32?(g=0,A=I>>>B|0):(g=I>>>B|0,A=((1<<B)-1&I)<<32-B|A>>>B),F=g|C,A|=E}function V(A,I,g){var B,Q=0,C=0,i=0,f=0,D=0,o=0,n=0,r=0,a=0,w=0,e=0,c=0;for(B=1<<g>>>1|0,g=0;(0|g)!=(0|B);)f=E[(i=(C=g+B<<3)+I|0)>>2],i=E[i+4>>2],o=E[(C=A+C|0)>>2],n=E[C+4>>2],e=lA(j(r=E[(D=(Q=g<<3)+A|0)>>2],a=E[D+4>>2],w=E[(Q=I+Q|0)>>2],Q=E[Q+4>>2]),F,j(o,n,f,i),F),c=F,f=G(j(r,a,f,i),F,j(o,n,w,Q),F),E[D>>2]=e,E[D+4>>2]=c,E[C>>2]=f,E[C+4>>2]=F,g=g+1|0}function m(A){var I=0,g=0,B=0,Q=0,E=0,i=0,D=0,o=0,n=0,r=0,a=0,w=0;for(I=1,i=1024;I>>>0<1024;){for(D=i>>>1|0,Q=0,E=0;(0|I)!=(0|E);){for(r=Q+D|0,a=f[26944+(I+E<<1)>>1],g=Q;g>>>0<r>>>0;)o=f[(B=(g<<1)+A|0)>>1],w=B,n=jA(f[(B=(g+D<<1)+A|0)>>1],a),C[w>>1]=KA(o,n),C[B>>1]=XA(o,n),g=g+1|0;Q=Q+i|0,E=E+1|0}I<<=1,i=D}}function O(A,I){for(var g=0,B=0,Q=0,C=0,i=0,f=0,D=0,o=0,n=0,r=0,a=0,w=0,e=0;512!=(0|C);)f=E[(i=(g=C+512<<3)+A|0)>>2],D=E[i+4>>2],o=E[(Q=(B=C<<3)+I|0)>>2],Q=E[Q+4>>2],n=E[(B=A+B|0)>>2],r=E[B+4>>2],g=mA(E[(g=I+g|0)>>2],E[g+4>>2]),a=F,w=lA(j(n,r,o,Q),F,j(f,D,g,a),F),e=F,f=G(j(n,r,g,a),F,j(f,D,o,Q),F),E[B>>2]=w,E[B+4>>2]=e,E[i>>2]=f,E[i+4>>2]=F,C=C+1|0}function Z(A,I,g,Q){var C=0,E=0,f=0,D=0,o=0,n=0,r=0,a=0;A:if(!((f=7+(I<<10)>>>3|0)>>>0>Q>>>0))for(o=-1<<I^-1,n=0-(D=1<<I-1)|0,Q=0;;){I:{if(C>>>0<1024)for(Q=Q+8|0,r=g+1|0,E=i[0|g]|E<<8;;){if(I>>>0>Q>>>0|C>>>0>=1024)break I;if(g=E>>>(Q=Q-I|0)&o,(0|(g|=0-(g&D)))==(0|n))break A;B[A+C|0]=g,C=C+1|0}a=(-1<<Q^-1)&E?0:f;break A}g=r}return a}function _(A,I,g,B){var Q,C=0,i=0,f=0,D=0,o=0,n=0,r=0,a=0;for(Q=1<<B>>>1|0,B=0;(0|B)!=(0|Q);)o=E[(C=(i=B+Q<<3)+g|0)>>2],n=E[C+4>>2],r=E[(f=(C=B<<3)+g|0)>>2],f=E[f+4>>2],a=E[(i=I+i|0)>>2],i=E[i+4>>2],D=A+C|0,C=I+C|0,E[D>>2]=K(G(G(OA(E[C>>2],E[C+4>>2]),F,OA(a,i),F),F,G(OA(r,f),F,OA(o,n),F),F),F),E[D+4>>2]=F,B=B+1|0}function $(A,I){var g,B=0,Q=0,C=0;return B=I<<10|A>>>22,C=A<<10&-1024,B=DA(C=(Q=(2046&(A=I>>>20|0))>>>0>1021)?C:0,Q=Q?1073741823&B|1073741824:0,63^(g=61-A&63)),A=F,Q=wA(C,Q,g),C=200>>>(A>>>29|(0-(B|536870911&A)|B)>>>31)&1,A=F,A=(B=Q+C|0)>>>0<C>>>0?A+1|0:A,C=B,A^=B=I>>31,F=A=(I=(Q=I>>>31|0)+(C^B)|0)>>>0<Q>>>0?A+1|0:A,I}function AA(A,I,g,B){for(var Q=0,C=0,i=0,f=0,D=0;(0|I)!=(0|Q);)i=E[(C=Q<<2)+A>>2]-(E[g+C>>2]+i|0)>>>31|0,Q=Q+1|0;for(Q=0,C=0-B>>>1|0,i=0-(!i|B)|0;(0|I)!=(0|Q);)B=E[(D=(f=Q<<2)+A|0)>>2]-((i&(C^E[g+f>>2]))+B|0)|0,E[D>>2]=2147483647&B,B=B>>>31|0,Q=Q+1|0}function IA(A){var I,g,B=0,Q=0;I=E[A>>2],B=E[I+200>>2],g=E[(A=(-8&B)+I|0)+4>>2],Q=31&(B<<=3),(63&B)>>>0>=32?(B=31<<Q,Q=0):(B=(1<<Q)-1&31>>>32-Q,Q=31<<Q),E[A>>2]=Q^E[A>>2],E[A+4>>2]=B^g,A=-2147483648^E[I+132>>2],E[I+128>>2]=E[I+128>>2],E[I+132>>2]=A,E[I+200>>2]=0,E[I+204>>2]=0}function gA(A,I,g){var B,Q=0,C=0,i=0;for(B=1<<g>>>1|0,g=0;(0|g)!=(0|B);)i=E[(Q=(C=g<<3)+A|0)>>2],C=I+C|0,E[Q>>2]=j(i,E[Q+4>>2],E[C>>2],E[C+4>>2]),E[Q+4>>2]=F,i=E[(Q=(g+B<<3)+A|0)>>2],E[Q>>2]=j(i,E[Q+4>>2],E[C>>2],E[C+4>>2]),E[Q+4>>2]=F,g=g+1|0}function BA(A,I){var g,B,Q,C=0,E=0;return g=I<<10|A>>>22,B=A<<10&-1024,E=A=I>>31,C=(2046&(A=I>>>20|0))>>>0>1021,A=E^(Q=wA(B,1073741823&g|1073741824,61-A&63),0,C?Q:0),E^=C?F:0,I=(C=I>>>31|0)+A|0,A=E,F=A=I>>>0<C>>>0?A+1|0:A,I}function QA(A,I,g,B){var Q,C=0;return C=A<<31,I=g&(A=(Q=I+1076|0)>>>31|0)-1,C|=A=(g=B&0-!A)>>>2|0,A=(A=(0-(g>>>22|0)&Q)<<20)+C|0,A=(g=(g=B=(3&g)<<30|I>>>2)+(B=0)|0)>>>0<B>>>0?A+1|0:A,F=A=(I=(B=g)+(g=200>>>(7&I)&1)|0)>>>0<g>>>0?A+1|0:A,I}function CA(A,I,g){var B,Q=0,C=0,i=0,f=0;for(B=g,g=0;(0|I)!=(0|g);)f=Q=(g<<2)+A|0,i=iA(E[Q>>2],0,B,0),Q=F,Q=(C=i+C|0)>>>0<i>>>0?Q+1|0:Q,E[f>>2]=2147483647&C,g=g+1|0,C=(2147483647&Q)<<1|C>>>31;return C}function EA(A){for(var I=0,g=0,B=0,Q=0,C=0;512!=(0|g);)Q=E[(I=B=(g+512<<3)+A|0)>>2],C=E[I+4>>2],E[(I=(g<<3)+A|0)>>2]=G(OA(E[I>>2],E[I+4>>2]),F,OA(Q,C),F),E[I+4>>2]=F,E[B>>2]=0,E[B+4>>2]=0,g=g+1|0}function iA(A,I,g,B){var Q,C,E,i,f=0,D=0;return i=o(f=g>>>16|0,D=A>>>16|0),f=(65535&(D=((E=o(Q=65535&g,C=65535&A))>>>16|0)+o(D,Q)|0))+o(f,C)|0,F=(o(I,g)+i|0)+o(A,B)+(D>>>16)+(f>>>16)|0,65535&E|f<<16}function fA(A,I,g){var Q,C=0,E=0,i=0;for(Q=0-g|0;;){if(C>>>10|0)i=1;else if(!((0|Q)>(0|(E=WA((C<<2)+I|0)))|(0|g)<(0|E))){B[A+C|0]=E,C=C+1|0;continue}break}return i}function DA(A,I,g){var B,Q,C=0;return B=(C=0-(g>>5)|0)>>31,Q=C,C=I^(I^(C=A))&B,I=A^=Q&A,(63&(A=g&=31))>>>0>=32?(g=I<<A,A=0):(g=(1<<A)-1&I>>>32-A|C<<A,A=I<<A),F=g,A}function oA(A){var I=0,g=0;for(I=A,A=c(208),E[I>>2]=A,A||(w(111),n());25!=(0|g);)E[(I=A+(g<<3)|0)>>2]=0,E[I+4>>2]=0,g=g+1|0;E[A+200>>2]=0,E[A+204>>2]=0}function nA(A){var I;return e=I=e-16|0,L(I+8|0,8,A),e=I+16|0,F=i[I+12|0]|i[I+13|0]<<8|i[I+14|0]<<16|i[I+15|0]<<24,i[I+8|0]|i[I+9|0]<<8|i[I+10|0]<<16|i[I+11|0]<<24}function rA(A){var I,B;return(A=(I=E[7760])+(B=A+3&-4)|0)>>>0<=I>>>0&&B||A>>>0>(g.byteLength/65536|0)<<16>>>0&&!(0|a(0|A))?(E[7951]=48,-1):(E[7760]=A,I)}function aA(A,I,g,B){return A=iA(I,0,A,0),B=iA(g,0,2147483647&iA(A,I=F,B,0),0),I=I+F|0,(0|(A=(I=(2147483647&(I=A>>>0>(B=A+B|0)>>>0?I+1|0:I))<<1|(A=B)>>>31)-g|0))<0?I:A}function wA(A,I,g){var B=0;return A^=(A^I)&(B=0-(g>>5)|0),I^=I&B>>31,B=A,(63&(A=g&=31))>>>0>=32?(g=0,A=I>>>A|0):(g=I>>>A|0,A=((1<<A)-1&I)<<32-A|B>>>A),F=g,A}function eA(A,I,g){for(var B=0,Q=0,C=0;!(C>>>g|0);)Q=(B=C<<3)+A|0,B=I+B|0,E[Q>>2]=lA(E[Q>>2],E[Q+4>>2],E[B>>2],E[B+4>>2]),E[Q+4>>2]=F,C=C+1|0}function FA(A,I,g){for(var B=0,Q=0,C=0;!(C>>>g|0);)Q=(B=C<<3)+A|0,B=I+B|0,E[Q>>2]=G(E[Q>>2],E[Q+4>>2],E[B>>2],E[B+4>>2]),E[Q+4>>2]=F,C=C+1|0}function cA(A,I){var g=0,B=0;return I=jA(I,10952),B=jA(g=AI(I),I),g=AI(AI(AI(AI(AI(jA(B,g)))))),B=jA(g,B),g=jA(B,g),jA(jA(AI(jA(AI(AI(jA(AI(AI(g)),B))),g)),I),A)}function tA(A,I,g){var B,Q=0,C=0;for(B=0-g>>>1|0;(0|I)!=(0|Q);)g=(E[(C=(Q<<2)+A|0)>>2]^B)+g|0,E[C>>2]=2147483647&g,Q=Q+1|0,g=g>>>31|0}function hA(A,I,g){var Q=0;if(A>>>0<I>>>0)return LA(A,I,g);if(g)for(Q=A+g|0,I=I+g|0;I=I-1|0,B[0|(Q=Q-1|0)]=i[0|I],g=g-1|0;);return A}function PA(A,I,g,B,Q){var C,i=0;for(C=I-1|0,I=0;!(I>>>C|0);)i=(I<<3)+A|0,E[(I<<2)+A>>2]=aA(aA(E[i>>2],E[i+4>>2],g,B),Q,g,B),I=I+1|0}function yA(A,I,g,B){var Q,C=0,E=0;for(Q=A-1|0,A=0,C=$A(I);Q>>>0>=(E=1<<A)>>>0;)Q&E&&(C=aA(C,B,I,g)),A=A+1|0,B=aA(B,B,I,g);return C}function sA(A,I){var g,B=0;for(I=(g=1<<I)>>>1|0;(0|I)!=(0|g);)E[(B=(I<<3)+A|0)>>2]=mA(E[B>>2],E[B+4>>2]),E[B+4>>2]=F,I=I+1|0}function SA(A,I){var g,B,Q=0;return g=Q=I+-1048576|0,I=0,B=Q=1+(Q>>>20&2047)|0,F=g&(Q=(I=Q?I:I+1|0)>>>11|0)-!(I=(2047&I)<<21|B>>>11),A&=I-1}function GA(A,I){for(var g=0,Q=0,C=0;(0|I)!=(0|Q);)e=g=e-16|0,B[g+15|0]=0,C=0|r(31044,g+15|0,0),e=g+16|0,B[A+Q|0]=C,Q=Q+1|0}function UA(A,I,g,B,Q){for(var C=0,i=0;I;)C=pA(aA(C,Q,g,B),(0|(C=(i=E[((I=I-1|0)<<2)+A>>2])-g|0))<0?i:C,g);return C}function RA(A,I){var g,B=0;return g=A,B=aA(B=pA(B=$A(A),B,A),B,A,I),B=aA(B,B,A,I),B=aA(B,B,A,I),B=aA(B,B,A,I),(g&0-(1&(A=aA(B,B,A,I))))+A>>>1|0}function kA(A,I,g,B){var Q=0;return Q=g,((0|A)!=(0|(Q=(g=(I^B)<0)?0:Q))|(0|I)!=(0|(g=g?0:B)))&(I&B)>>>31^(A>>>0<Q>>>0&(0|I)<=(0|g)|(0|I)<(0|g))}function NA(A,I){for(var g=0,Q=0,C=0;!(g>>>10|0);)Q=(g<<3)+A|0,C=B[I+g|0],E[Q>>2]=M(C,C>>31),E[Q+4>>2]=F,g=g+1|0}function HA(A,I,g){for(var B=0,Q=0;!(Q>>>10|0);)E[(B=(Q<<3)+A|0)>>2]=j(E[B>>2],E[B+4>>2],I,g),E[B+4>>2]=F,Q=Q+1|0}function uA(A){for(var I=0,g=0,Q=0,C=0;!(g>>>10|0);)C=I,I=B[A+g|0],Q|=I=C+o(I,I)|0,g=g+1|0;return Q>>31|I}function JA(A){var I;return A=o(2-o(I=A,A=2-A|0)|0,A),A=o(2-o(I,A)|0,A),A=o(2-o(I,A)|0,A),2147483647&o(o(I,A)+2147483646|0,A)}function dA(A){for(var I=0,g=0;!(g>>>10|0);)E[(I=(g<<3)+A|0)>>2]=mA(E[I>>2],E[I+4>>2]),E[I+4>>2]=F,g=g+1|0}function vA(A,I){for(var g=0,B=0,Q=0;!(g>>>10|0);)C[(Q=(B=g<<1)+A|0)>>1]=jA(f[Q>>1],f[I+B>>1]),g=g+1|0}function YA(A){for(var I=0,g=0;!(I>>>10|0);)C[(g=(I<<1)+A|0)>>1]=jA(f[g>>1],10952),I=I+1|0}function LA(A,I,g){var Q=0;if(g)for(Q=A;B[0|Q]=i[0|I],Q=Q+1|0,I=I+1|0,g=g-1|0;);return A}function jA(A,I){return A=o(A,I),I=(A=o(65535&o(A,12287),12289)+A|0)>>>16|0,A>>>0<805371904?I:I-12289|0}function MA(A){var I,g=0;return I=(g=E[A+512>>2])+1|0,E[A+512>>2]=I,g=i[A+g|0],512==(0|I)&&y(A),g}function xA(A,I,g,B,Q,C){return I?VA(UA(A,I,g,B,Q),0-(E[((I<<2)+A|0)-4>>2]>>>30|0)&C,g):0}function bA(A,I){var g=0;if(I)for(g=A;B[0|g]=0,g=g+1|0,I=I-1|0;);return A}function qA(A,I){var g=0;return(-1>>>(g=31&I)&A)<<g|((g=A)&-1<<(A=0-I&31))>>>A}function zA(A,I){return A+(((I=7&(A=I-A|0))?8-I|0:0)+A|0)|0}function TA(A){return A-((A-1073736705>>>31|0)-1&2147473409)|0}function KA(A,I){return(0|(A=(I=A+I|0)-12289|0))<0?I:A}function lA(A,I,g,B){return A=G(A,I,g,-2147483648^B)}function pA(A,I,g){return(0|(A=(I=A+I|0)-g|0))<0?I:A}function WA(A){return(A=E[A>>2])<<1&-2147483648|A}function XA(A,I){return((A=A-I|0)>>31&12289)+A|0}function VA(A,I,g){return((A=A-I|0)>>31&g)+A|0}function mA(A,I){return F=-2147483648^I,A}function OA(A,I){return A=j(A,I,A,I)}function ZA(A){return(A>>31&12289)+A|0}function _A(A,I){return(A>>31&I)+A|0}function $A(A){return-2147483648-A|0}function AI(A){return jA(A,A)}function II(A){h(E[A>>2])}Q(I=i,1024,"cmFuZG9tYnl0ZXMAbGlic29kaXVtL3NyYy9saWJzb2RpdW0vcmFuZG9tYnl0ZXMvcmFuZG9tYnl0ZXMuYwBidWZfbGVuIDw9IFNJWkVfTUFY"),Q(I,1120,"AQAAAAAAAACCgAAAAAAAAIqAAAAAAACAAIAAgAAAAICLgAAAAAAAAAEAAIAAAAAAgYAAgAAAAIAJgAAAAAAAgIoAAAAAAAAAiAAAAAAAAAAJgACAAAAAAAoAAIAAAAAAi4AAgAAAAACLAAAAAAAAgImAAAAAAACAA4AAAAAAAIACgAAAAAAAgIAAAAAAAACACoAAAAAAAAAKAACAAAAAgIGAAIAAAACAgIAAAAAAAIABAACAAAAAAAiAAIAAAACAAAgICAgIBwcGBgUACAgICAgICAgICA=="),Q(I,1346,"QQBDAEcATQBWAGQAegCaAM0AHwE="),Q(I,1376,"o4MRdAQAAAAG/IxUNgAAAAoUv9xPAgAAReCdkx0XAACEb4/1DNAAAOOW9xxogAYA6g9bMNiCLQDQbwYOERERAQAPB1VVVVUFAP+BVVVVVRUAtAIAAAAAQABI//////9/AAAAAAAAAIA="),Q(I,1511,"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"),Q(I,18192,"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"),Q(I,24464,"AQAAAAEAAAACAAAAAgAAAAQAAAAHAAAADgAAABsAAAA1AAAAagAAANEAAAAAAAAAAgAAAAIAAAAFAAAABwAAAAwAAAAVAAAAKAAAAE4AAACdAAAANAE="),Q(I,24560,"BAAAAAAAAAALAAAAAQAAABgAAAABAAAAMgAAAAEAAABmAAAAAQAAAMoAAAACAAAAkQEAAAQAAAAaAwAABQAAACkGAAAIAAAAQgwAAA0AAACkGAAAGQ=="),Q(I,24659,"AgABAAOAAIACgAGAA0AAQAJAAUADwADAAsABwAMgACACIAEgA6AAoAKgAaADYABgAmABYAPgAOAC4AHgAxAAEAIQARADkACQApABkANQAFACUAFQA9AA0ALQAdADMAAwAjABMAOwALACsAGwA3AAcAJwAXAD8ADwAvAB8AMIAAgCCAEIA4gAiAKIAYgDSABIAkgBSAPIAMgCyAHIAygAKAIoASgDqACoAqgBqANoAGgCaAFoA+gA6ALoAegDGAAYAhgBGAOYAJgCmAGYA1gAWAJYAVgD2ADYAtgB2AM4ADgCOAE4A7gAuAK4AbgDeAB4AngBeAP4APgC+AH4AwQABAIEAQQDhACEAoQBhANEAEQCRAFEA8QAxALEAcQDJAAkAiQBJAOkAKQCpAGkA2QAZAJkAWQD5ADkAuQB5AMUABQCFAEUA5QAlAKUAZQDVABUAlQBVAPUANQC1AHUAzQANAI0ATQDtAC0ArQBtAN0AHQCdAF0A/QA9AL0AfQDDAAMAgwBDAOMAIwCjAGMA0wATAJMAUwDzADMAswBzAMsACwCLAEsA6wArAKsAawDbABsAmwBbAPsAOwC7AHsAxwAHAIcARwDnACcApwBnANcAFwCXAFcA9wA3ALcAdwDPAA8AjwBPAO8ALwCvAG8A3wAfAJ8AXwD/AD8AvwB/AMCAAICAgECA4IAggKCAYIDQgBCAkIBQgPCAMICwgHCAyIAIgIiASIDogCiAqIBogNiAGICYgFiA+IA4gLiAeIDEgASAhIBEgOSAJICkgGSA1IAUgJSAVID0gDSAtIB0gMyADICMgEyA7IAsgKyAbIDcgByAnIBcgPyAPIC8gHyAwoACgIKAQoDigCKAooBigNKAEoCSgFKA8oAygLKAcoDKgAqAioBKgOqAKoCqgGqA2oAagJqAWoD6gDqAuoB6gMaABoCGgEaA5oAmgKaAZoDWgBaAloBWgPaANoC2gHaAzoAOgI6AToDugC6AroBugN6AHoCegF6A/oA+gL6AfoDBgAGAgYBBgOGAIYChgGGA0YARgJGAUYDxgDGAsYBxgMmACYCJgEmA6YApgKmAaYDZgBmAmYBZgPmAOYC5gHmAxYAFgIWARYDlgCWApYBlgNWAFYCVgFWA9YA1gLWAdYDNgA2AjYBNgO2ALYCtgG2A3YAdgJ2AXYD9gD2AvYB9gMOAA4CDgEOA44AjgKOAY4DTgBOAk4BTgPOAM4CzgHOAy4ALgIuAS4DrgCuAq4BrgNuAG4CbgFuA+4A7gLuAe4DHgAeAh4BHgOeAJ4CngGeA14AXgJeAV4D3gDeAt4B3gM+AD4CPgE+A74AvgK+Ab4DfgB+An4BfgP+AP4C/gH+AwEAAQIBAQEDgQCBAoEBgQNBAEECQQFBA8EAwQLBAcEDIQAhAiEBIQOhAKECoQGhA2EAYQJhAWED4QDhAuEB4QMRABECEQERA5EAkQKRAZEDUQBRAlEBUQPRANEC0QHRAzEAMQIxATEDsQCxArEBsQNxAHECcQFxA/EA8QLxAfEDCQAJAgkBCQOJAIkCiQGJA0kASQJJAUkDyQDJAskByQMpACkCKQEpA6kAqQKpAakDaQBpAmkBaQPpAOkC6QHpAxkAGQIZARkDmQCZApkBmQNZAFkCWQFZA9kA2QLZAdkDOQA5AjkBOQO5ALkCuQG5A3kAeQJ5AXkD+QD5AvkB+QMFAAUCBQEFA4UAhQKFAYUDRQBFAkUBRQPFAMUCxQHFAyUAJQIlASUDpQClAqUBpQNlAGUCZQFlA+UA5QLlAeUDFQAVAhUBFQOVAJUClQGVA1UAVQJVAVUD1QDVAtUB1QM1ADUCNQE1A7UAtQK1AbUDdQB1AnUBdQP1APUC9QH1Aw0ADQINAQ0DjQCNAo0BjQNNAE0CTQFNA80AzQLNAc0DLQAtAi0BLQOtAK0CrQGtA20AbQJtAW0D7QDtAu0B7QMdAB0CHQEdA50AnQKdAZ0DXQBdAl0BXQPdAN0C3QHdAz0APQI9AT0DvQC9Ar0BvQN9AH0CfQF9A/0A/QL9Af0DAwADAgMBAwODAIMCgwGDA0MAQwJDAUMDwwDDAsMBwwMjACMCIwEjA6MAowKjAaMDYwBjAmMBYwPjAOMC4wHjAxMAEwITARMDkwCTApMBkwNTAFMCUwFTA9MA0wLTAdMDMwAzAjMBMwOzALMCswGzA3MAcwJzAXMD8wDzAvMB8wMLAAsCCwELA4sAiwKLAYsDSwBLAksBSwPLAMsCywHLAysAKwIrASsDqwCrAqsBqwNrAGsCawFrA+sA6wLrAesDGwAbAhsBGwObAJsCmwGbA1sAWwJbAVsD2wDbAtsB2wM7ADsCOwE7A7sAuwK7AbsDewB7AnsBewP7APsC+wH7AwcABwIHAQcDhwCHAocBhwNHAEcCRwFHA8cAxwLHAccDJwAnAicBJwOnAKcCpwGnA2cAZwJnAWcD5wDnAucB5wMXABcCFwEXA5cAlwKXAZcDVwBXAlcBVwPXANcC1wHXAzcANwI3ATcDtwC3ArcBtwN3AHcCdwF3A/cA9wL3AfcDDwAPAg8BDwOPAI8CjwGPA08ATwJPAU8DzwDPAs8BzwMvAC8CLwEvA68ArwKvAa8DbwBvAm8BbwPvAO8C7wHvAx8AHwIfAR8DnwCfAp8BnwNfAF8CXwFfA98A3wLfAd8DPwA/Aj8BPwO/AL8CvwG/A38AfwJ/AX8D/wD/Av8B/wNleHBhbmQgMzItYnl0ZSBr9PejAKzTLgACGDkAK9NUAD8fGACC230AzX0iAEiT0AD/wSkAddEKAMd3QwDkSpkAhJUCAPOubABvHz8ASncAAO1UxwBfvXQAJBAAACtU3QDkancAoQEAAGXc/wDaY60AHwAAAIrYgAAoZHsAAQAAALL9wwBpDAQAAAAAACTPEgD7MdAAAAAAAJ+UAAAfCYsAAAAAAGYDAACYqV0AAAAAAA4AAAC7br8="),Q(I,26896,"fl0v"),Q(I,26908,"mHA="),Q(I,26920,"xg=="),Q(I,26932,"AQ=="),Q(I,26944,"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"),Q(I,31040,"MH6A");var gI,BI=((gI=[]).set=function(A,I){this[A]=I},gI.get=function(A){return this[A]},gI);return{f:function(){},g:BI,h:c,i:h,j:function(){var A;e=A=e-16|0,B[A+15|0]=0,r(31080,A+15|0,0),e=A+16|0},k:function(){return 1793},l:function(){return 2305},m:function(){return 1332},n:function(A,I){A|=0,I|=0;var g,Q,D,n,r,a,w,c,t,h,P,y,s,S,N=0,J=0,v=0,L=0,X=0,O=0,Z=0,AA=0,BA=0,QA=0,EA=0,DA=0,nA=0,rA=0,wA=0,SA=0,UA=0,dA=0,vA=0,YA=0,jA=0,MA=0,qA=0,KA=0,XA=0,$A=0,AI=0,gI=0,BI=0,QI=0,CI=0,EI=0,iI=0,fI=0,DI=0,oI=0,nI=0,rI=0,aI=0,wI=0,eI=0,FI=0,cI=0,tI=0;e=D=e-34880|0,GA(N=D+16|0,48),oA(a=D+8|0),u(a,N,48),IA(a),Q=D+5184|0,n=D+4160|0,h=D+3136|0,P=D+2112|0,c=(r=(g=D+6208|0)- -8192|0)- -8192|0,w=(N=D- -64|0)||g,t=((N?0:1024)<<1)+g|0,S=31==(0|(y=i[1322]-1|0)),s=-1<<i[1333]-1^-1;A:for(;;){for(x(a,Q),x(a,n),v=0-(L=1<<y)|0,J=0;;){if(1024!=(0|J)){if((0|(N=B[J+Q|0]))>=(0|L)|(0|N)<=(0|v))continue A;if((0|(N=B[J+n|0]))>=(0|L))continue A;if(J=J+1|0,(0|N)>(0|v))continue;continue A}break}if(!S&&!((((J=uA(Q))|(N=uA(n)))>>31|N+J)>>>0>16822)){for(NA(g,Q),NA(r,n),H(g,10),H(r,10),_(c,g,r,10),sA(g,10),sA(r,10),HA(g,0,1086849152),HA(r,0,1086849152),gA(g,c,10),gA(r,c,10),k(g,10),k(r,10),J=0,rA=0,AA=0;1024!=(0|J);)L=G(rA,AA,OA(E[(N=(v=J<<3)+g|0)>>2],E[N+4>>2]),F),rA=G(L,F,OA(E[(N=v+r|0)>>2],E[N+4>>2]),F),AA=F,J=J+1|0;if(kA(rA,AA,1608035756,1087401370)){for(J=0;1024!=(0|J);)C[(N=J<<1)+t>>1]=ZA(B[J+Q|0]),C[N+w>>1]=ZA(B[J+n|0]),J=J+1|0;m(w),m(t),J=0;I:{for(;;){if(1024!=(0|J)){if(v=f[(N=J<<1)+t>>1]){C[(N=N+w|0)>>1]=cA(f[N>>1],v),J=J+1|0;continue}v=0;break I}break}b(w),v=1}if(v){if(FI=0,iI=0,T(nI=(N=(YA=E[6126])<<2)+(eI=N+g|0)|0,Q,n,10,0),R(nI,YA,YA,2,0,J=(aI=N+nI|0)+N|0),YA){for(QI=JA(E[nI>>2]),CI=JA(E[aI>>2]),DI=LA((N=(v=YA<<2)+J|0)+v|0,nI,v),wI=LA(DI+v|0,aI,v),E[eI>>2]=1,bA(eI+4|0,v-4|0),$A=bA(g,v),gI=LA(J,aI,v),cI=LA(N,nI,v),E[cI>>2]=E[cI>>2]-1,oI=o(YA,62)+30|0;wA=0,O=0,AA=0,UA=0,N=-1,v=-1,L=YA,oI>>>0>=30;){for(;L;)wA^=((X=E[(J=(L=L-1|0)<<2)+wI>>2])^wA)&N,O^=(X^O)&v,AA^=((J=E[J+DI>>2])^AA)&N,UA^=(J^UA)&v,v=(N=v)&(2147483647+(J|X)>>>31|0)-1;for(v=(J=(X=-1^N)&O)>>>1|0,Z=BA=(J<<=31)+(N&O|wA)|0,EI=J>>>0>BA>>>0?v+1|0:v,v=N&UA|AA,L=(N=X&UA)>>>1|0,QA=J=v+(N<<=31)|0,fI=N>>>0>J>>>0?L+1|0:L,N=E[wI>>2],O=E[DI>>2],v=0,X=0,UA=0,SA=1,J=0,rA=0,AA=0,BA=1,wA=0;31!=(0|v);)N=((N=N-((tI=(EA=O)-(0-(O=(DA=(qA=O>>>v&1)&N>>>v)&(L=((fI^(L=EI-((Z>>>0<QA>>>0)+fI|0)|0))&(EI^fI)^L)>>>31|0))&N)|0)&0-(L=DA&(-1^L)))|0)&0-(BI=O|!qA))+N|0,L=((L=wA-(((MA=UA-(((dA=0-(0!=(0|O))|0)&wA)+((DA=X)>>>0<(KA=(rI=0-O|0)&BA)>>>0)|0)|0)&(XA=0-(0!=(0|L))|0))+((qA=BA)>>>0<(AI=(EA=X-KA|0)&(vA=0-L|0))>>>0)|0)|0)&(KA=0-(0!=(0|(X=BI)))|0))+L|0,L=(BA=BA-AI|0)>>>0>(nA=(BA&(jA=0-X|0))+BA|0)>>>0?L+1|0:L,BA=nA,wA=L,O=rA-(L=vA&(qA=SA-(UA=rA&rI)|0))|0,L=(KA&(J=AA-((XA&(AI=J-((AA&dA)+(SA>>>0<UA>>>0)|0)|0))+(L>>>0>rA>>>0)|0)|0))+J|0,rA=DA=(jA&O)+O|0,AA=O>>>0>DA>>>0?L+1|0:L,nA=X-1|0,SA=0-!X|0,X=EI-(((DA=fI-((dA&EI)+((J=Z&rI)>>>0>QA>>>0)|0)|0)&XA)+((J=vA&(rI=QA-J|0))>>>0>Z>>>0)|0)|0,Z=(J=Z-J|0)^(J^((1&X)<<31|J>>>1))&nA,EI=X^(X^X>>>1)&(O=SA),O=tI+(tI&BI-1)|0,L=MA+(SA&MA)|0,UA=L=(J=EA)>>>0>(X=J+(J&nA)|0)>>>0?L+1|0:L,J=AI+(SA&AI)|0,J=(L=qA)>>>0>(QA=L+(nA&L)|0)>>>0?J+1|0:J,SA=QA,L=(QA=DA)>>>1|0,QA=rI^jA&(rI^((1&QA)<<31|rI>>>1)),fI=DA^KA&(L^DA),v=v+1|0;for(N=0,DA=0,v=0,QA=0,L=0;(0|N)!=(0|YA);)Z=iA(qA=E[(BI=N<<2)+DI>>2],0,rA,AA),O=F+v|0,O=Z>>>0>(DA=Z+DA|0)>>>0?O+1|0:O,EA=(v=DA)+(Z=iA(DA=E[BI+wI>>2],0,BA,wA))|0,v=F+O|0,v=Z>>>0>EA>>>0?v+1|0:v,O=QA,QA=iA(qA,0,SA,J)+O|0,L=L+F|0,L=O>>>0>QA>>>0?L+1|0:L,Z=(O=QA)+(QA=iA(DA,0,X,UA))|0,O=F+L|0,O=Z>>>0<QA>>>0?O+1|0:O,N&&(E[(L=BI-4|0)+DI>>2]=2147483647&Z,E[L+wI>>2]=2147483647&EA),N=N+1|0,L=v,v>>=31,DA=(2147483647&L)<<1|EA>>>31,L=O>>31,QA=(2147483647&O)<<1|Z>>>31;E[(N=(YA<<2)-4|0)+DI>>2]=QA,E[N+wI>>2]=DA,tA(DI,YA,L=L>>>31|0),tA(wI,YA,N=v>>>31|0),N=1&(O=L|N<<1),L=UA,QA=0-(0!=(0|(O=O>>>1|0)))|0,DA=0-O|0,UA=J-(((O=J<<1|(v=SA)>>>31)&(Z=0-N|0))+(v>>>0<(SA=v<<1&(qA=0-N|0))>>>0)|0)|0,L=L-((Z&(L<<1|X>>>31))+((J=qA&X<<1)>>>0>X>>>0)|0)|0,SA=v-SA|0,O=X-J|0,N=AA-(((v=AA<<1|(J=rA)>>>31)&QA)+((X=J<<1&DA)>>>0>J>>>0)|0)|0,d(eI,gI,aI,YA,CI,SA,UA,O,L,AA=J-X|0,N,X=BA-(J=DA&BA<<1)|0,J=wA-((QA&(wA<<1|BA>>>31))+(J>>>0>BA>>>0)|0)|0),d($A,cI,nI,YA,QI,SA,UA,O,L,AA,N,X,J),oI=oI-30|0}for(N=1^E[DI>>2],v=1;(0|v)!=(0|YA);)N=E[DI+(v<<2)>>2]|N,v=v+1|0;v=E[aI>>2]&E[nI>>2]&!N}else v=0;v&&(CA(g,YA,12289)||(iI=!CA(eI,YA,12289)));I:if(iI){for(iI=10;;){if(iI>>>0>=3){for(QI=0,L=g,BI=E[(X=(iI=iI-1|0)<<2)+24468>>2],T(J=(L+(v=o(BI,$A=(MA=1<<(vA=10-iI|0))>>>1|0)<<2)|0)+v|0,Q,n,iI,1),XA=(N=(QA=E[X+24512>>2])<<vA<<2)+L|0,jA=E[X+24464>>2],UA=hA(XA+N|0,J,o(jA,8<<vA)),X=(BA=hA((N=jA<<vA<<2)+(SA=UA+N|0)|0,L,o(BI,4<<vA&-8)))+v|0,v=0;;){if((0|v)==(0|QA)){EI=(YA=(KA=(gI=(N=MA<<2)+(CI=N+BA|0)|0)+N|0)+N|0)+($A<<2)|0,fI=vA-1|0,X=QA<<3;break}for(Z=(N=v<<2)+XA|0,O=N+L|0,N=0,rA=yA(BI,EA=E[o(v,12)+18192>>2],qA=JA(EA),DA=RA(EA,qA)),J=X,wA=BA;(0|N)!=(0|$A);)E[O>>2]=xA(wA,BI,EA,qA,DA,rA),E[Z>>2]=xA(J,BI,EA,qA,DA,rA),N=N+1|0,Z=(AA=QA<<2)+Z|0,O=O+AA|0,J=(AA=BI<<2)+J|0,wA=AA+wA|0;v=v+1|0}for(;;){g:{B:{if((0|QA)!=(0|QI)){if(N=o(QI,12),AI=RA(nA=E[N+18192>>2],dA=JA(nA)),(0|jA)==(0|QI)&&(R(UA,jA,jA,MA,1,BA),R(SA,jA,jA,MA,1,BA)),l(BA,CI,vA,E[N+18196>>2],nA,dA),jA>>>0>QI>>>0){for(Z=0,O=AA=(N=QI<<2)+SA|0,J=v=N+UA|0;(0|Z)!=(0|MA);)E[(N=Z<<2)+gI>>2]=E[J>>2],E[N+KA>>2]=E[O>>2],Z=Z+1|0,O=(N=jA<<2)+O|0,J=N+J|0;q(v,jA,CI,vA,nA,dA),q(AA,jA,CI,vA,nA,dA)}else{for(Z=0,v=yA(jA,nA,dA,AI),O=SA,J=UA;(0|Z)!=(0|MA);)E[(N=Z<<2)+gI>>2]=xA(J,jA,nA,dA,AI,v),E[N+KA>>2]=xA(O,jA,nA,dA,AI,v),Z=Z+1|0,O=(N=jA<<2)+O|0,J=N+J|0;p(gI,1,BA,vA,nA,dA),p(KA,1,BA,vA,nA,dA)}for(Z=0,O=v=(N=QI<<2)+XA|0,J=N=N+L|0;(0|Z)!=(0|$A);)E[(AA=Z<<2)+YA>>2]=E[J>>2],E[AA+EI>>2]=E[O>>2],Z=Z+1|0,O=(AA=QA<<2)+O|0,J=J+AA|0;for(p(YA,1,BA,fI,nA,dA),p(EI,1,BA,fI,nA,dA),Z=0,O=v,J=N;;){if((0|Z)==(0|$A))break B;qA=E[(AA=Z<<3)+gI>>2],rA=E[(DA=4|AA)+gI>>2],wA=E[AA+KA>>2],BI=aA(E[(AA=Z<<2)+YA>>2],AI,nA,dA),EA=aA(E[AA+EI>>2],AI,nA,dA),E[J>>2]=aA(E[DA+KA>>2],BI,nA,dA),E[(AA=QA<<2)+J>>2]=aA(wA,BI,nA,dA),E[O>>2]=aA(rA,EA,nA,dA),E[O+AA>>2]=aA(qA,EA,nA,dA),Z=Z+1|0,O=X+O|0,J=J+X|0}}R(L,QA,QA,MA,1,BA),R(XA,QA,QA,MA,1,BA),KA=zA(L,BA),v=zA(L,fI=(YA=(((N=3&(J=(AI=(EI=(X=MA<<3)+(gI=KA+X|0)|0)+($A<<3)|0)-L|0))?4-N|0:0)+J|0)+L|0)+(MA<<2)|0),z(KA,((J=jA<<2)+UA|0)-(N=(BA=jA>>>0<10?jA:10)<<2)|0,BA,jA,vA),z(gI,(J+SA|0)-N|0,BA,jA,vA),H(KA,vA),H(gI,vA),_(EI,KA,gI,vA),sA(KA,vA),sA(gI,vA),$A=(N=X+AI|0)>>>0>v>>>0?N:v,Z=((v=o(QA,31))-(J=E[(N=iI<<3)+24560>>2])|0)+(N=o(E[N+24564>>2],6))|0,BI=N+J|0,EA=2<<vA,qA=jA-BA|0,DA=iI>>>0>4,J=QA;Q:{for(;;){for(z(AI,((X=J<<2)+L|0)-(N=(BA=J>>>0<10?J:10)<<2)|0,BA,QA,vA),z($A,(X+XA|0)-N|0,BA,QA,vA),H(AI,vA),H($A,vA),V(AI,KA,vA),V($A,gI,vA),FA($A,AI,vA),gA($A,EI,vA),k($A,vA),N=((N=(X=o(qA+(BA-J|0)|0,31)+Z|0)>>31)^X)-N|0,rA=0&(X=(0|X)<0),AA=X?1073741824:1071644672,BA=0,wA=1072693248;;){if(!N){C:for(N=0;;){if((0|N)==(0|MA))break C;if(O=0,!kA(-4194304,-1042284545,AA=j(E[(X=$A+(N<<3)|0)>>2],E[X+4>>2],BA,wA),X=F))break Q;if(!kA(AA,X,-4194304,1105199103))break Q;E[YA+(N<<2)>>2]=$(AA,X),N=N+1|0}break}1&N&&(BA=j(BA,wA,rA,AA),wA=F),N=N>>>1|0,rA=OA(rA,AA),AA=F}if(N=Z-o(X=(0|Z)/31|0,31)|0,DA?(Y(L,J,QA,UA,jA,jA,YA,X,N,vA),Y(XA,J,QA,SA,jA,jA,YA,X,N,vA)):(U(L,J,QA,UA,jA,jA,YA,X,N,vA,fI),U(XA,J,QA,SA,jA,jA,YA,X,N,vA,fI)),(0|(N=(X=Z+BI|0)+10|0))>=(0|v)?N=v:J=J-((X+41|0)<=(0|o(J,31)))|0,!((0|Z)>0))break;Z=((0|Z)>25?Z:25)-25|0,v=N}C:if(!(J>>>0>=jA>>>0))for(BA=J-1<<2,v=L,O=0;;){if((0|O)==(0|MA))break C;for(X=0-(E[v+BA>>2]>>>30|0)>>>1|0,N=J;(0|N)!=(0|jA);)E[(N<<2)+v>>2]=X,N=N+1|0;for(X=0-(E[BA+XA>>2]>>>30|0)>>>1|0,N=J;(0|N)!=(0|jA);)E[(N<<2)+XA>>2]=X,N=N+1|0;O=O+1|0,XA=(N=QA<<2)+XA|0,v=N+v|0}for(v=jA<<2,N=0,O=1,Z=L;;){if((0|N)==(0|EA))break Q;J=hA(Z,L,v),N=N+1|0,L=(QA<<2)+L|0,Z=J+v|0}}break g}q(N,QA,CI,vA,nA,dA),q(v,QA,CI,vA,nA,dA),QI=QI+1|0;continue}break}if(O)continue;break I}break}for(oI=0,wA=0,O=(UA=(X=g+2048|0)+2048|0)+4096|0;2!=(0|oI);){for(v=(N=oI<<2)+O|0,N=N+UA|0,Z=0,BA=yA(2,rA=E[o(oI,12)+18192>>2],SA=JA(rA),AA=RA(rA,SA)),L=g,J=X;256!=(0|Z);)E[N>>2]=xA(L,2,rA,SA,AA,BA),E[v>>2]=xA(J,2,rA,SA,AA,BA),v=v+8|0,N=N+8|0,J=J+8|0,L=L+8|0,Z=Z+1|0;oI=oI+1|0}for(XA=hA(g,UA,4096),CI=(QI=(BI=(MA=(L=(AA=(fI=hA(XA+4096|0,O,4096))+4096|0)+2048|0)+2048|0)+4096|0)+2048|0)+4096|0,EI=(YA=(DA=(qA=(EA=MA+2048|0)+2048|0)+2048|0)+2048|0)+1024|0;2!=(0|wA);){for(N=o(wA,12),nA=E[N+18192>>2],l(MA,BI,10,v=E[N+18196>>2],nA,dA=JA(nA)),N=0;1024!=(0|N);)E[(J=N<<2)+QI>>2]=_A(B[N+Q|0],nA),E[J+CI>>2]=_A(B[N+n|0],nA),N=N+1|0;for($A=RA(nA,dA),p(QI,1,MA,10,nA,dA),p(CI,1,MA,10,nA,dA),N=10;N>>>0>9;)PA(QI,N,nA,dA,$A),PA(CI,N,nA,dA,$A),N=N-1|0;for(gI=hA(EA,BI,2048),KA=hA(qA,QI,2048),AI=hA(DA,CI,2048),N=0,J=BA=(v=wA<<2)+fI|0,v=X=v+XA|0;256!=(0|N);)E[(O=N<<2)+YA>>2]=E[v>>2],E[O+EI>>2]=E[J>>2],J=J+8|0,v=v+8|0,N=N+1|0;for(p(YA,1,MA,8,nA,dA),p(EI,1,MA,8,nA,dA),N=0,J=BA,v=X;256!=(0|N);)QA=E[(O=N<<3)+KA>>2],SA=E[(rA=4|O)+KA>>2],UA=E[O+AI>>2],Z=aA(E[(O=N<<2)+YA>>2],$A,nA,dA),O=aA(E[O+EI>>2],$A,nA,dA),E[v>>2]=aA(E[rA+AI>>2],Z,nA,dA),E[v+8>>2]=aA(UA,Z,nA,dA),E[J>>2]=aA(SA,O,nA,dA),E[J+8>>2]=aA(QA,O,nA,dA),J=J+16|0,v=v+16|0,N=N+1|0;q(X,2,gI,9,nA,dA),q(BA,2,gI,9,nA,dA);g:if(!wA)for(q(KA,1,gI,9,nA,dA),q(AI,1,gI,9,nA,dA),Z=0,v=L,N=AA;;){if(512==(0|Z))break g;J=Z<<2,E[N>>2]=E[J+KA>>2],E[v>>2]=E[J+AI>>2],v=v+4|0,N=N+4|0,Z=Z+1|0}wA=wA+1|0}for(R(XA,2,2,1024,1,MA),R(AA,1,1,1024,1,MA),z(N=zA(XA,MA),XA,2,2,9),z(N+4096|0,fI,2,2,9),J=hA(XA,AA,4096),z(N=(wA=(YA=hA(zA(J,(AI=J+2048|0)+2048|0),N,8192))+4096|0)+4096|0,J,1,1,9),z(N+4096|0,AI,1,1,9),H(XA=hA(J,YA,16384),9),H($A=XA+4096|0,9),H(QI=$A+4096|0,9),H(CI=QI+4096|0,9),MA=CI+4096|0,v=0;256!=(0|v);)EI=E[(N=(gI=v+256<<3)+$A|0)>>2],fI=E[N+4>>2],BI=E[(N=(KA=v<<3)+CI|0)>>2],EA=E[N+4>>2],qA=E[(N=$A+KA|0)>>2],DA=E[N+4>>2],L=E[(N=gI+CI|0)>>2],J=E[N+4>>2],Z=E[(N=XA+gI|0)>>2],QA=E[N+4>>2],rA=E[(N=KA+QI|0)>>2],O=E[N+4>>2],AA=E[(N=XA+KA|0)>>2],BA=E[N+4>>2],X=mA(E[(N=gI+QI|0)>>2],E[N+4>>2]),N=F,SA=lA(j(AA,BA,rA,O),F,j(Z,QA,X,N),F),UA=F,O=G(j(AA,BA,X,N),F,j(Z,QA,rA,O),F),AA=F,J=mA(L,J),N=F,BA=lA(j(qA,DA,BI,EA),F,j(EI,fI,J,N),F),X=F,L=G(j(qA,DA,J,N),F,j(EI,fI,BI,EA),F),J=F,E[(N=MA+KA|0)>>2]=G(SA,UA,BA,X),E[N+4>>2]=F,E[(N=MA+gI|0)>>2]=G(O,AA,L,J),E[N+4>>2]=F,v=v+1|0;_(N=MA+4096|0,QI,CI,9),gA(MA,N,9),k(MA,9),Z=0;g:{for(;512!=(0|Z);){if(v=0,!kA(J=E[(L=MA+(Z<<3)|0)>>2],N=E[L+4>>2],0,1138753536))break g;if(!kA(0,-1008730112,J,N))break g;E[L>>2]=M($(J,N),F),E[L+4>>2]=F,Z=Z+1|0}for(H(MA,9),V(QI,MA,9),V(CI,MA,9),eA(XA,QI,9),eA($A,CI,9),k(XA,9),k($A,9),X=hA(YA,XA,8192),Z=0,v=1;;){if(512==(0|Z))break g;N=(J=Z<<3)+X|0,E[(L=Z<<2)+XA>>2]=$(E[N>>2],E[N+4>>2]),N=J+wA|0,E[L+AI>>2]=$(E[N>>2],E[N+4>>2]),Z=Z+1|0}}if(v){for(J=0,l(L=(DA=(qA=(QA=g+2048|0)+2048|0)+4096|0)+4096|0,rA=L+4096|0,10,383167813,2147473409,EA=JA(2147473409));512!=(0|J);)E[(N=(v=J<<2)+g|0)>>2]=_A(WA(N),2147473409),E[(N=v+QA|0)>>2]=_A(WA(N),2147473409),J=J+1|0;for(p(g,1,L,9,2147473409,EA),p(QA,1,L,9,2147473409,EA),J=0;1024!=(0|J);)E[(N=J<<2)+qA>>2]=_A(B[J+Q|0],2147473409),E[N+DA>>2]=_A(B[J+n|0],2147473409),J=J+1|0;for(Z=RA(2147473409,EA),p(qA,1,L,10,2147473409,EA),p(DA,1,L,10,2147473409,EA),J=0;J>>>0<1024;)O=E[(UA=(N=J<<2)+qA|0)>>2],X=E[(BA=(AA=4|N)+qA|0)>>2],v=E[(L=N+DA|0)>>2],wA=aA(E[(N=J<<1)+g>>2],Z,2147473409,EA),SA=aA(E[N+QA>>2],Z,2147473409,EA),N=AA+DA|0,E[UA>>2]=aA(E[N>>2],wA,2147473409,EA),E[BA>>2]=aA(v,wA,2147473409,EA),E[L>>2]=aA(X,SA,2147473409,EA),E[N>>2]=aA(O,SA,2147473409,EA),J=J+2|0;for(q(qA,1,rA,10,2147473409,EA),q(DA,1,rA,10,2147473409,EA),l(QA=(BA=(UA=hA(g,qA,8192))+4096|0)+4096|0,rA=QA+4096|0,10,383167813,2147473409,EA),p(UA,1,QA,10,2147473409,EA),p(BA,1,QA,10,2147473409,EA),wA=(DA=(SA=rA+4096|0)+4096|0)+4096|0,N=_A(B[0|Q],2147473409),E[wA>>2]=N,E[DA>>2]=N,J=1;1024!=(0|J);)N=J+Q|0,E[DA+(J<<2)>>2]=_A(B[0|N],2147473409),E[wA+(1024-J<<2)>>2]=_A(0-B[0|N]|0,2147473409),J=J+1|0;for(p(DA,1,QA,10,2147473409,EA),p(wA,1,QA,10,2147473409,EA),J=0;1024!=(0|J);)N=aA(E[(v=J<<2)+wA>>2],Z,2147473409,EA),E[v+rA>>2]=aA(N,E[v+UA>>2],2147473409,EA),E[v+SA>>2]=aA(N,E[v+DA>>2],2147473409,EA),J=J+1|0;for(N=_A(B[0|n],2147473409),E[wA>>2]=N,E[DA>>2]=N,J=1;1024!=(0|J);)N=J+n|0,E[DA+(J<<2)>>2]=_A(B[0|N],2147473409),E[wA+(1024-J<<2)>>2]=_A(0-B[0|N]|0,2147473409),J=J+1|0;for(p(DA,1,QA,10,2147473409,EA),p(wA,1,QA,10,2147473409,EA),J=0;1024!=(0|J);)N=E[(v=(X=J<<2)+rA|0)>>2],L=aA(E[X+wA>>2],Z,2147473409,EA),E[v>>2]=pA(N,aA(L,E[X+BA>>2],2147473409,EA),2147473409),N=E[(v=X+SA|0)>>2],E[v>>2]=pA(N,aA(L,E[X+DA>>2],2147473409,EA),2147473409),J=J+1|0;for(l(QA,DA,10,383167813,2147473409,EA),q(rA,1,DA,10,2147473409,EA),q(SA,1,DA,10,2147473409,EA),J=0;1024!=(0|J);)N=(v=J<<2)+rA|0,E[v+QA>>2]=TA(E[N>>2]),E[N>>2]=TA(E[v+SA>>2]),J=J+1|0;for(J=0,L=zA(UA,SA);1024!=(0|J);)v=L+(J<<3)|0,N=E[rA+(J<<2)>>2],E[v>>2]=M(N,N>>31),E[v+4>>2]=F,J=J+1|0;for(H(L,10),AA=(N=hA(zA(UA,rA),L,4096))+4096|0,J=0;1024!=(0|J);)L=AA+(J<<3)|0,v=E[QA+(J<<2)>>2],E[L>>2]=M(v,v>>31),E[L+4>>2]=F,J=J+1|0;for(H(AA,10),O=0;512!=(0|O);)L=K(E[(J=(v=O<<3)+N|0)>>2],E[J+4>>2]),J=E[(X=v+AA|0)>>2],v=F,E[X>>2]=j(J,E[X+4>>2],L,v),E[X+4>>2]=F,J=E[(X=AA+(O+512<<3)|0)>>2],E[X>>2]=j(J,E[X+4>>2],L,v),E[X+4>>2]=F,O=O+1|0;for(k(AA,10),J=0;1024!=(0|J);)N=AA+(J<<3)|0,E[QA+(J<<2)>>2]=_A($(E[N>>2],E[N+4>>2]),2147473409),J=J+1|0;for(l(rA,SA,10,383167813,2147473409,EA),J=0;1024!=(0|J);)E[(N=J<<2)+DA>>2]=_A(B[J+Q|0],2147473409),E[N+wA>>2]=_A(B[J+n|0],2147473409),J=J+1|0;for(p(QA,1,rA,10,2147473409,EA),p(DA,1,rA,10,2147473409,EA),p(wA,1,rA,10,2147473409,EA),J=0;1024!=(0|J);)N=E[(v=(X=J<<2)+UA|0)>>2],L=aA(E[X+QA>>2],Z,2147473409,EA),E[v>>2]=VA(N,aA(L,E[X+DA>>2],2147473409,EA),2147473409),N=E[(v=X+BA|0)>>2],E[v>>2]=VA(N,aA(L,E[X+wA>>2],2147473409,EA),2147473409),J=J+1|0;for(q(UA,1,SA,10,2147473409,EA),q(BA,1,SA,10,2147473409,EA),J=0;1024!=(0|J);)E[(N=(v=J<<2)+UA|0)>>2]=TA(E[N>>2]),E[(N=v+BA|0)>>2]=TA(E[N>>2]),J=J+1|0;if(fA(h,g,s)&&fA(N=P||g- -8192|0,BA=g+4096|0,s)){for(l(X=(v=(L=BA+4096|0)+4096|0)+4096|0,g,10,383167813,2147473409,AA=JA(2147473409)),iI=0,J=0;;){if(1024==(0|J)){for(;1024!=(0|iI);)E[(N=iI<<2)+BA>>2]=_A(B[iI+Q|0],2147473409),E[N+L>>2]=_A(B[iI+n|0],2147473409),E[N+v>>2]=_A(B[iI+h|0],2147473409),iI=iI+1|0;for(FI=1,p(BA,1,X,10,2147473409,AA),p(L,1,X,10,2147473409,AA),p(v,1,X,10,2147473409,AA),p(g,1,X,10,2147473409,AA),J=0,N=aA(12289,1,2147473409,AA);;){if(1024==(0|J))break I;if(X=J<<2,J=J+1|0,(0|VA(aA(E[X+BA>>2],E[X+g>>2],2147473409,AA),aA(E[L+X>>2],E[v+X>>2],2147473409,AA),2147473409))!=(0|N))break}break}E[(J<<2)+g>>2]=_A(B[N+J|0],2147473409),J=J+1|0}FI=0}}}if(FI)break}}}}if(II(a),B[0|I]=90,L=-1,(v=W(I+1|0,2304,Q,N=i[1322]))&&(N=W((J=v+1|0)+I|0,2304-v|0,D+4160|0,N))&&!(!(I=W((N=N+J|0)+I|0,2305-N|0,D+3136|0,i[1333]))|2305!=(I+N|0))){B[0|A]=10,I=A+1|0,v=D- -64|0,A=0,L=0;A:{I:{for(;;){if(1024!=(0|A)){if(N=A<<1,A=A+1|0,f[N+v>>1]<=12288)continue;break I}break}if(I){for(J=0,A=0;1024!=(0|J);){for(L=f[v+(J<<1)>>1]|L<<14,A=A+14|0;(0|A)>=8;)A=A-8|0,B[0|I]=L>>>A,I=I+1|0;J=J+1|0}(0|A)<=0||(B[0|I]=L<<8-A)}A=1792;break A}A=0}L=1792!=(0|A)?-1:0}return e=D+34880|0,0|L},o:function(A,I,g,Q){A|=0,I|=0,g|=0,Q|=0;var D,n,r,a,w,c=0,t=0,h=0,P=0,S=0,G=0,U=0,R=0,J=0,d=0,v=0,Y=0,j=0,x=0;if(e=n=(e=r=e-16|0)-16|0,E[n+12>>2]=1287,R=(w=A+2|0)+1|0,a=w+41|0,e=D=e-79936|0,h=-1,90==i[0|Q]&&(t=Z(D+5184|0,S=i[1322],Q+1|0,2304))&&(t=Z(D+4160|0,P=S,(S=t+1|0)+Q|0,2304-t|0))&&(P=Q,Q=t+S|0,!(!(t=Z(D+3136|0,i[1333],P+Q|0,2305-Q|0))|2305!=(Q+t|0)))){for(S=D+2112|0,P=D+5184|0,G=D+4160|0,d=D+3136|0,t=(Q=D+6208|0)+2048|0;1024!=(0|c);)C[(J=c<<1)+Q>>1]=ZA(B[c+G|0]),C[t+J>>1]=ZA(B[c+d|0]),c=c+1|0;for(m(Q),m(t),YA(Q),vA(Q,t),c=0;1024!=(0|c);)C[t+(c<<1)>>1]=ZA(B[c+P|0]),c=c+1|0;m(t),c=0;A:{I:{for(;1024!=(0|c);){if(!(G=f[(P=c<<1)+t>>1]))break I;C[(P=Q+P|0)>>1]=cA(f[P>>1],G),c=c+1|0}for(b(Q),c=0;;){if(P=1,1024==(0|c))break A;if((t=((t=f[Q+(c<<1)>>1])>>>0<6144?0:-12289)+t|0)-128>>>0<4294967041)break I;B[c+S|0]=t,c=c+1|0}}P=0}if(P){for(GA(R,40),oA(Q=D+8|0),u(Q,R,40),u(Q,I,g),IA(Q),N(Q,D- -64|0,D+6208|0),II(Q),GA(I=D+16|0,48),oA(Q),u(Q,I,48),IA(Q);;){for(R=D- -64|0,x=D+8|0,d=D+5184|0,J=D+4160|0,v=D+3136|0,Y=D+2112|0,g=D+6208|0,e=S=e-800|0;;){for(E[S+792>>2]=794350117,E[S+796>>2]=1073020125,e=Q=e+-64|0,L(Q,56,x),c=(G=S+8|0)+520|0,I=0;14!=(0|I);)E[(h=I<<2)+c>>2]=E[Q+h>>2],I=I+1|0;for(y(G),e=Q- -64|0,NA(c=g- -8192|0,d),NA(g,J),NA(t=(h=c- -8192|0)- -8192|0,v),NA(h,Y),H(c,10),H(g,10),H(t,10),H(h,10),dA(c),dA(t),EA(I=LA(t- -8192|0,c,8192)),O(Q=LA(I- -8192|0,g,8192),h),EA(g),FA(g,I,10),P=LA(I,c,8192),O(c,t),FA(c,Q,10),EA(h),EA(Q=LA(Q,t,8192)),FA(h,Q,10),I=0;1024!=(0|I);)E[(U=Q+(I<<3)|0)>>2]=M(f[R+(I<<1)>>1],0),E[U+4>>2]=F,I=I+1|0;for(H(Q,10),V(U=LA(Q- -8192|0,Q,8192),P,10),HA(U,mA(-1869109374,1058362595),F),V(Q,t,10),HA(Q,-1869109374,1058362595),s(G,I=LA(t,Q,16384),P,g,c,h,10,Q),P=hA(P,I,16384),NA(c,d),NA(g,J),NA(I,v),NA(h,Y),H(c,10),H(g,10),H(I,10),H(h,10),dA(c),dA(I),t=LA(U,P,8192),G=LA(t- -8192|0,Q,8192),V(t,g,10),V(G,h,10),FA(t,G,10),V(h=LA(G,P,8192),c,10),G=LA(P,t,8192),V(Q,I,10),FA(Q,h,10),k(G,10),k(Q,10),h=0,c=0,I=0;1024!=(0|I);)P=G+(I<<3)|0,P=f[(U=I<<1)+R>>1]-$(E[P>>2],E[P+4>>2])|0,C[t+U>>1]=P,c|=h=o(P,P)+h|0,I=I+1|0;for(c>>=31,I=0;1024!=(0|I);)P=Q+(I<<3)|0,C[g+(I<<1)>>1]=0-$(E[P>>2],E[P+4>>2]),I=I+1|0;for(I=0,c=(Q=c|h)>>31;!(I>>>10|0);)P=Q,Q=C[g+(I<<1)>>1],c|=Q=P+o(Q,Q)|0,I=I+1|0;if(I=0,(c>>31|Q)>>>0>=87067565||(LA(R,g,2048),LA(g,t,2048),I=1),I)break}e=S+800|0,t=E[n+12>>2],h=0,I=0,c=0;A:{I:{for(;;){if(1024!=(0|h)){if(g=h<<1,h=h+1|0,(f[g+R>>1]-2048&65535)>>>0>=61441)continue;break I}g=0,h=0;break}for(;;){g:{if(1024!=(0|c))for(I=(127&(P=((Q=f[R+(c<<1)>>1])^(S=Q<<16>>31))-S|0)|Q>>>8&128|I<<8)<<(S=1+((65408&P)>>>7|0)|0)|1,h=8+(h+S|0)|0;;){if(h>>>0<8)break g;if(h=h-8|0,a){if(g>>>0>=t>>>0)break I;B[g+a|0]=I>>>h}g=g+1|0}if(!h)break A;B:{if(a){if(Q=0,g>>>0>=t>>>0)break B;B[g+a|0]=I<<8-h}Q=g+1|0}g=Q;break A}c=c+1|0}}g=0}if(g)break}II(D+8|0),E[n+12>>2]=g,h=0}}return e=D+79936|0,(0|h)<0?j=-1:(B[0|w]=58,E[r+12>>2]=E[n+12>>2]+41),e=n+16|0,j||(C[A>>1]=E[r+12>>2]),e=r+16|0,0|j},p:function(A,I,g,B){A|=0,I|=0,B|=0;var Q=0,E=0,D=0,n=0,r=0,a=0,w=0,F=0,c=0,t=0,h=0,P=0;if(n=(g|=0)+2|0,h=-1,g=f[g>>1],!(58!=i[0|n]|g>>>0<41)){if(D=n+1|0,P=n+41|0,t=g-41|0,e=r=e-8208|0,10==i[0|B]){a=r+4112|0,w=B+1|0,B=0;A:{for(;;){n=E>>>10|0;I:{for(;;){if(g=B,n)break I;if(B=g+8|0,F=i[0|w]|F<<8,w=w+1|0,!((0|g)<6))break}if((g=F>>>(B=g-6|0)&16383)>>>0>12288)break A;C[a+(E<<1)>>1]=g,E=E+1|0;continue}break}c=(-1<<g^-1)&F?0:1792}if(1792==(0|c)&&(m(g=r+4112|0),YA(g),t)){n=r+16|0,E=0,F=0,w=0;A:{for(;;){if(Q>>>10|0)break A;I:if(!(E>>>0>=t>>>0)){for(c=127&(g=(F=i[E+P|0]|F<<8)>>>w|0),B=128&g,E=E+1|0;;){if(!w){if(E>>>0>=t>>>0)break I;F=i[E+P|0]|F<<8,E=E+1|0,w=8}if(F>>>(w=w-1|0)&1)break;if(g=c>>>0>1919,c=c+128|0,g)break I}C[n+(Q<<1)>>1]=B?0-c|0:c,Q=Q+1|0;continue}break}E=0}if((0|E)==(0|t)){for(oA(B=r+8|0),u(B,D,40),u(B,A,I),IA(B),N(B,g=r+2064|0,a=r+6160|0),II(B),n=r+16|0,I=r+4112|0,Q=0;1024!=(0|Q);)B=C[(A=Q<<1)+n>>1],C[A+a>>1]=B+(B>>>15&12289),Q=Q+1|0;for(m(a),vA(a,I),b(a),D=0;!(D>>>10|0);)C[(A=(I=D<<1)+a|0)>>1]=XA(f[A>>1],f[I+g>>1]),D=D+1|0;for(Q=0;1024!=(0|Q);)I=f[(A=a+(Q<<1)|0)>>1],C[A>>1]=I+(I>>>0>6144?-12289:0),Q=Q+1|0;for(D=0,Q=0,B=0;!(B>>>10|0);)A=C[(I=B<<1)+a>>1],g=o(A,A)+D|0,A=C[I+n>>1],Q|=(D=g+o(A,A)|0)|g,B=B+1|0;h=(Q>>31|D)>>>0<87067565?0:-1}}}e=r+8208|0}return 0|h}}}(A)}(O)},instantiate:function(A,I){return{then:function(I){var g=new h.Module(A);I({instance:new h.Instance(g)})}}},RuntimeError:Error};e=[],"object"!=A(h)&&M("no native wasm support detected");var P=!1;var y,s,S,G,U,R="undefined"!=typeof TextDecoder?new TextDecoder("utf8"):void 0;function k(A,I){return A?function(A,I,g){for(var B=I+g,Q=I;A[Q]&&!(Q>=B);)++Q;if(Q-I>16&&A.buffer&&R)return R.decode(A.subarray(I,Q));for(var C="";I<Q;){var E=A[I++];if(128&E){var i=63&A[I++];if(192!=(224&E)){var f=63&A[I++];if((E=224==(240&E)?(15&E)<<12|i<<6|f:(7&E)<<18|i<<12|f<<6|63&A[I++])<65536)C+=String.fromCharCode(E);else{var D=E-65536;C+=String.fromCharCode(55296|D>>10,56320|1023&D)}}else C+=String.fromCharCode((31&E)<<6|i)}else C+=String.fromCharCode(E)}return C}(S,A,I):""}var N,H=i.INITIAL_MEMORY||16777216;(c=i.wasmMemory?i.wasmMemory:new h.Memory({initial:H/65536,maximum:H/65536}))&&(y=c.buffer),H=y.byteLength,y=N=y,i.HEAP8=s=new Int8Array(N),i.HEAP16=new Int16Array(N),i.HEAP32=G=new Int32Array(N),i.HEAPU8=S=new Uint8Array(N),i.HEAPU16=new Uint16Array(N),i.HEAPU32=new Uint32Array(N),i.HEAPF32=new Float32Array(N),i.HEAPF64=U=new Float64Array(N);var u=[],J=[],d=[];function v(){return t}var Y=0,L=null,j=null;function M(A){throw i.onAbort&&i.onAbort(A),F(A="Aborted("+A+")"),P=!0,1,A+=". Build with -sASSERTIONS for more info.",new h.RuntimeError(A)}var x,b,q="data:application/octet-stream;base64,";function z(A){return A.startsWith(q)}function T(A){return A.startsWith("file://")}function K(A){try{if(A==x&&e)return new Uint8Array(e);var I=V(A);if(I)return I;if(B)return B(A);throw"both async and sync fetching of the wasm failed"}catch(A){M(A)}}z(x="<<< WASM_BINARY_FILE >>>")||(b=x,x=i.locateFile?i.locateFile(b,w):w+b);var l={31044:function(){return i.getRandomValue()},31080:function(){if(void 0===i.getRandomValue)try{var I="object"===("undefined"==typeof window?"undefined":A(window))?window:self,g=void 0!==I.crypto?I.crypto:I.msCrypto,B=function(){var A=new Uint32Array(1);return g.getRandomValues(A),A[0]>>>0};B(),i.getRandomValue=B}catch(A){try{var Q=eval("require")("crypto"),C=function(){var A=Q.randomBytes(4);return(A[0]<<24|A[1]<<16|A[2]<<8|A[3])>>>0};C(),i.getRandomValue=C}catch(A){throw"No secure random number generator found"}}}};function p(A){for(;A.length>0;)A.shift()(i)}var W=[];var X="function"==typeof atob?atob:function(A){var I,g,B,Q,C,E,i="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=",f="",D=0;A=A.replace(/[^A-Za-z0-9\+\/\=]/g,"");do{I=i.indexOf(A.charAt(D++))<<2|(Q=i.indexOf(A.charAt(D++)))>>4,g=(15&Q)<<4|(C=i.indexOf(A.charAt(D++)))>>2,B=(3&C)<<6|(E=i.indexOf(A.charAt(D++))),f+=String.fromCharCode(I),64!==C&&(f+=String.fromCharCode(g)),64!==E&&(f+=String.fromCharCode(B))}while(D<A.length);return f};function V(A){if(z(A))return function(A){if("boolean"==typeof a&&a){var I=Buffer.from(A,"base64");return new Uint8Array(I.buffer,I.byteOffset,I.byteLength)}try{for(var g=X(A),B=new Uint8Array(g.length),Q=0;Q<g.length;++Q)B[Q]=g.charCodeAt(Q);return B}catch(A){throw new Error("Converting base64 string to bytes failed.")}}(A.slice(q.length))}var m,O={e:function(A,I,g,B){M("Assertion failed: "+k(A)+", at: "+[I?k(I):"unknown filename",g,B?k(B):"unknown function"])},b:function(A,I,g){var B=function(A,I){var g;for(W.length=0,I>>=2;g=S[A++];)I+=105!=g&I,W.push(105==g?G[I]:U[I++>>1]),++I;return W}(I,g);return l[A].apply(null,B)},c:function(A){S.length,M("OOM")},d:function(A){!function(A,I){A,function(A){A,v()||(i.onExit&&i.onExit(A),P=!0);o(A,new Z(A))}(A)}(A)},a:c};(function(){var A={a:O};function I(A,I){var g,B=A.exports;i.asm=B,i.asm.g,g=i.asm.f,J.unshift(g),function(A){if(Y--,i.monitorRunDependencies&&i.monitorRunDependencies(Y),0==Y&&(null!==L&&(clearInterval(L),L=null),j)){var I=j;j=null,I()}}()}function B(A){I(A.instance)}function Q(I){return function(){if(!e&&(n||r)){if("function"==typeof fetch&&!T(x))return fetch(x,{credentials:"same-origin"}).then((function(A){if(!A.ok)throw"failed to load wasm binary file at '"+x+"'";return A.arrayBuffer()})).catch((function(){return K(x)}));if(g)return new Promise((function(A,I){g(x,(function(I){A(new Uint8Array(I))}),I)}))}return Promise.resolve().then((function(){return K(x)}))}().then((function(I){return h.instantiate(I,A)})).then((function(A){return A})).then(I,(function(A){F("failed to asynchronously prepare wasm: "+A),M(A)}))}if(Y++,i.monitorRunDependencies&&i.monitorRunDependencies(Y),i.instantiateWasm)try{return i.instantiateWasm(A,I)}catch(A){return F("Module.instantiateWasm callback failed with error: "+A),!1}e||"function"!=typeof h.instantiateStreaming||z(x)||T(x)||a||"function"!=typeof fetch?Q(B):fetch(x,{credentials:"same-origin"}).then((function(I){return h.instantiateStreaming(I,A).then(B,(function(A){return F("wasm streaming compile failed: "+A),F("falling back to ArrayBuffer instantiation"),Q(B)}))}))})(),i.___wasm_call_ctors=function(){return(i.___wasm_call_ctors=i.asm.f).apply(null,arguments)},i._malloc=function(){return(i._malloc=i.asm.h).apply(null,arguments)},i._free=function(){return(i._free=i.asm.i).apply(null,arguments)},i._falconjs_init=function(){return(i._falconjs_init=i.asm.j).apply(null,arguments)},i._falconjs_public_key_bytes=function(){return(i._falconjs_public_key_bytes=i.asm.k).apply(null,arguments)},i._falconjs_secret_key_bytes=function(){return(i._falconjs_secret_key_bytes=i.asm.l).apply(null,arguments)},i._falconjs_signature_bytes=function(){return(i._falconjs_signature_bytes=i.asm.m).apply(null,arguments)},i._falconjs_keypair=function(){return(i._falconjs_keypair=i.asm.n).apply(null,arguments)},i._falconjs_sign=function(){return(i._falconjs_sign=i.asm.o).apply(null,arguments)},i._falconjs_verify=function(){return(i._falconjs_verify=i.asm.p).apply(null,arguments)};function Z(A){this.name="ExitStatus",this.message="Program terminated with exit("+A+")",this.status=A}function _(A){function I(){m||(m=!0,i.calledRun=!0,P||(!0,p(J),i.onRuntimeInitialized&&i.onRuntimeInitialized(),function(){if(i.postRun)for("function"==typeof i.postRun&&(i.postRun=[i.postRun]);i.postRun.length;)A=i.postRun.shift(),d.unshift(A);var A;p(d)}()))}A=A||D,Y>0||(!function(){if(i.preRun)for("function"==typeof i.preRun&&(i.preRun=[i.preRun]);i.preRun.length;)A=i.preRun.shift(),u.unshift(A);var A;p(u)}(),Y>0||(i.setStatus?(i.setStatus("Running..."),setTimeout((function(){setTimeout((function(){i.setStatus("")}),1),I()}),1)):I()))}if(i.writeArrayToMemory=function(A,I){s.set(A,I)},j=function A(){m||_(),m||(j=A)},i.run=_,i.preInit)for("function"==typeof i.preInit&&(i.preInit=[i.preInit]);i.preInit.length>0;)i.preInit.pop()();return _(),new Promise((function(A,I){i.onAbort=I,i.onRuntimeInitialized=function(){A(i)}}))})).then((function(A){Object.keys(A).forEach((function(I){B[I]=A[I]}))}));var i=B.ready.then((function(){B._falconjs_init(),A=B._falconjs_public_key_bytes(),I=B._falconjs_secret_key_bytes(),g=B._falconjs_signature_bytes()})),f={publicKeyBytes:i.then((function(){return A})),privateKeyBytes:i.then((function(){return I})),bytes:i.then((function(){return g})),keyPair:function(){return i.then((function(){var g=B._malloc(A),i=B._malloc(I);try{return Q(B._falconjs_keypair(g,i),{publicKey:C(g,A),privateKey:C(i,I)})}finally{E(g),E(i)}}))},sign:function(A,I){return f.signDetached(A,I).then((function(I){var B=new Uint8Array(g+A.length);return B.set(I),B.set(A,g),B}))},signDetached:function(A,f){return i.then((function(){var i=B._malloc(g),D=B._malloc(A.length),o=B._malloc(I);B.writeArrayToMemory(new Uint8Array(g),i),B.writeArrayToMemory(A,D),B.writeArrayToMemory(f,o);try{return Q(B._falconjs_sign(i,D,A.length,o),C(i,g))}finally{E(i),E(D),E(o)}}))},open:function(A,I){return i.then((function(){var B=new Uint8Array(A.buffer,A.byteOffset,g),Q=new Uint8Array(A.buffer,A.byteOffset+g);return f.verifyDetached(B,Q,I).then((function(A){if(A)return Q;throw new Error("Failed to open Falcon signed message.")}))}))},verifyDetached:function(I,Q,C){return i.then((function(){var i=B._malloc(g),f=B._malloc(Q.length),D=B._malloc(A);B.writeArrayToMemory(I,i),B.writeArrayToMemory(Q,f),B.writeArrayToMemory(C,D);try{return 0===B._falconjs_verify(f,Q.length,i,D)}finally{E(i),E(f),E(D)}}))}};return f}(); true&&module.exports?(falcon.falcon=falcon,module.exports=falcon):self.falcon=falcon;

/***/ }),
/* 2 */
/***/ (function(module, __unused_webpack_exports, __webpack_require__) {

var isNode	=
	typeof process === 'object' &&
	"function" === 'function' &&
	typeof window !== 'object' &&
	typeof importScripts !== 'function'
;


var naclSHA512	= __webpack_require__(3);
var sodiumUtil	= __webpack_require__(4);


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
/* 3 */
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
/* 4 */
/***/ (function(module) {

var sodiumUtil=function(){function r(e){if("function"==typeof TextDecoder)return new TextDecoder("utf-8",{fatal:!0}).decode(e);var n=8192,t=Math.ceil(e.length/n);if(t<=1)try{return decodeURIComponent(escape(String.fromCharCode.apply(null,e)))}catch(r){throw new TypeError("The encoded data was not valid.")}for(var o="",i=0,f=0;f<t;f++){var a=Array.prototype.slice.call(e,f*n+i,(f+1)*n+i);if(0!=a.length){var u,c=a.length,l=0;do{var s=a[--c];s>=240?(l=4,u=!0):s>=224?(l=3,u=!0):s>=192?(l=2,u=!0):s<128&&(l=1,u=!0)}while(!u);for(var d=l-(a.length-c),p=0;p<d;p++)i--,a.pop();o+=r(a)}}return o}function e(r){for(var e,n,t,o="",i=0;i<r.length;i++)t=87+(n=15&r[i])+(n-10>>8&-39)<<8|87+(e=r[i]>>>4)+(e-10>>8&-39),o+=String.fromCharCode(255&t)+String.fromCharCode(t>>>8);return o}function n(r,e){function n(r){return r<26?r+65:r<52?r+71:r<62?r-4:62===r?43:63===r?47:65}if(void 0===e&&(e=!0),"string"==typeof r)throw new Error("input has to be an array");for(var t=2,o="",i=r.length,f=0,a=0;a<i;a++)t=a%3,a>0&&4*a/3%76==0&&!e&&(o+="\r\n"),f|=r[a]<<(16>>>t&24),2!==t&&r.length-a!=1||(o+=String.fromCharCode(n(f>>>18&63),n(f>>>12&63),n(f>>>6&63),n(63&f)),f=0);return o.substr(0,o.length-2+t)+(2===t?"":1===t?"=":"==")}return{from_base64:function(r){return"string"==typeof r?function(r,e){for(var n,t,o,i=r.replace(/[^A-Za-z0-9\+\/]/g,""),f=i.length,a=e?Math.ceil((3*f+1>>2)/e)*e:3*f+1>>2,u=new Uint8Array(a),c=0,l=0,s=0;s<f;s++)if(t=3&s,c|=((o=i.charCodeAt(s))>64&&o<91?o-65:o>96&&o<123?o-71:o>47&&o<58?o+4:43===o?62:47===o?63:0)<<18-6*t,3===t||f-s==1){for(n=0;n<3&&l<a;n++,l++)u[l]=c>>>(16>>>n&24)&255;c=0}return u}(r):r},from_base64url:function(r){return"string"==typeof r?this.from_base64(r.replace(/-/g,"+").replace(/_/g,"/")):r},from_hex:function(r){return"string"==typeof r?function(r){if(!function(r){return"string"==typeof r&&/^[0-9a-f]+$/i.test(r)&&r.length%2==0}(r))throw new TypeError("The provided string doesn't look like hex data");for(var e=new Uint8Array(r.length/2),n=0;n<r.length;n+=2)e[n>>>1]=parseInt(r.substr(n,2),16);return e}(r):r},from_string:function(r){return"string"==typeof r?function(r){if("function"==typeof TextEncoder)return new TextEncoder("utf-8").encode(r);r=unescape(encodeURIComponent(r));for(var e=new Uint8Array(r.length),n=0;n<r.length;n++)e[n]=r.charCodeAt(n);return e}(r):r},memcmp:function(r,e){if(!(r instanceof Uint8Array&&e instanceof Uint8Array))throw new TypeError("Only Uint8Array instances can be compared");if(r.length!==e.length)throw new TypeError("Only instances of identical length can be compared");for(var n=0,t=0,o=r.length;t<o;t++)n|=r[t]^e[t];return 0===n},memzero:function(r){r instanceof Uint8Array?function(r){if(!r instanceof Uint8Array)throw new TypeError("Only Uint8Array instances can be wiped");for(var e=0,n=r.length;e<n;e++)r[e]=0}(r):"undefined"!=typeof Buffer&&r instanceof Buffer&&r.fill(0)},to_base64:function(r){return"string"==typeof r?r:n(r).replace(/\s+/g,"")},to_base64url:function(r){return this.to_base64(r).replace(/\+/g,"-").replace(/\//g,"_")},to_hex:function(r){return"string"==typeof r?r:e(r).replace(/\s+/g,"")},to_string:function(e){return"string"==typeof e?e:r(e)}}}(); true&&module.exports?(sodiumUtil.sodiumUtil=sodiumUtil,module.exports=sodiumUtil):self.sodiumUtil=sodiumUtil;

/***/ }),
/* 5 */
/***/ (function(module, __unused_webpack_exports, __webpack_require__) {

var __dirname = "/";
var Module={},isNode="object"==typeof process&&"function"=="function"&&"object"!=typeof window&&"function"!=typeof importScripts,pemJwk=__webpack_require__(6),sodiumUtil=__webpack_require__(29),initiated,nodeCrypto,generateRSAKeypair;if(isNode)initiated=Promise.resolve(),nodeCrypto=eval("require")("crypto"),generateRSAKeypair=eval("require")("generate-rsa-keypair");else{initiated=new Promise((function(A,e){Module.onAbort=e,Module.onRuntimeInitialized=function(){try{Module._rsasignjs_init(),A()}catch(A){e(A)}}}));var Module=void 0!==Module?Module:{},moduleOverrides={},key;for(key in Module)Module.hasOwnProperty(key)&&(moduleOverrides[key]=Module[key]);var arguments_=[],thisProgram="./this.program",quit_=function(A,e){throw e},ENVIRONMENT_IS_WEB=!1,ENVIRONMENT_IS_WORKER=!1,ENVIRONMENT_IS_NODE=!1,ENVIRONMENT_IS_SHELL=!1;ENVIRONMENT_IS_WEB="object"==typeof window,ENVIRONMENT_IS_WORKER="function"==typeof importScripts,ENVIRONMENT_IS_NODE="object"==typeof process&&"object"==typeof process.versions&&"string"==typeof process.versions.node,ENVIRONMENT_IS_SHELL=!ENVIRONMENT_IS_WEB&&!ENVIRONMENT_IS_NODE&&!ENVIRONMENT_IS_WORKER;var scriptDirectory="",read_,readAsync,readBinary,setWindowTitle,nodeFS,nodePath;function locateFile(A){return Module.locateFile?Module.locateFile(A,scriptDirectory):scriptDirectory+A}ENVIRONMENT_IS_NODE?(scriptDirectory=ENVIRONMENT_IS_WORKER?eval("require")("path").dirname(scriptDirectory)+"/":__dirname+"/",read_=function shell_read(filename,binary){var ret=tryParseAsDataURI(filename);return ret?binary?ret:ret.toString():(nodeFS||(nodeFS=eval("require")("fs")),nodePath||(nodePath=eval("require")("path")),filename=nodePath.normalize(filename),nodeFS.readFileSync(filename,binary?null:"utf8"))},readBinary=function(A){var e=read_(A,!0);return e.buffer||(e=new Uint8Array(e)),assert(e.buffer),e},process.argv.length>1&&(thisProgram=process.argv[1].replace(/\\/g,"/")),arguments_=process.argv.slice(2), true&&(module.exports=Module),process.on("uncaughtException",(function(A){if(!(A instanceof ExitStatus))throw A})),process.on("unhandledRejection",abort),quit_=function(A){process.exit(A)},Module.inspect=function(){return"[Emscripten Module object]"}):ENVIRONMENT_IS_SHELL?("undefined"!=typeof read&&(read_=function(A){var e=tryParseAsDataURI(A);return e?intArrayToString(e):read(A)}),readBinary=function(A){var e;return(e=tryParseAsDataURI(A))?e:"function"==typeof readbuffer?new Uint8Array(readbuffer(A)):(assert("object"==typeof(e=read(A,"binary"))),e)},"undefined"!=typeof scriptArgs?arguments_=scriptArgs:"undefined"!=typeof arguments&&(arguments_=arguments),"function"==typeof quit&&(quit_=function(A){quit(A)}),"undefined"!=typeof print&&("undefined"==typeof console&&(console={}),console.log=print,console.warn=console.error="undefined"!=typeof printErr?printErr:print)):(ENVIRONMENT_IS_WEB||ENVIRONMENT_IS_WORKER)&&(ENVIRONMENT_IS_WORKER?scriptDirectory=self.location.href:document.currentScript&&(scriptDirectory=document.currentScript.src),scriptDirectory=0!==scriptDirectory.indexOf("blob:")?scriptDirectory.substr(0,scriptDirectory.lastIndexOf("/")+1):"",read_=function(A){try{var e=new XMLHttpRequest;return e.open("GET",A,!1),e.send(null),e.responseText}catch(e){var r=tryParseAsDataURI(A);if(r)return intArrayToString(r);throw e}},ENVIRONMENT_IS_WORKER&&(readBinary=function(A){try{var e=new XMLHttpRequest;return e.open("GET",A,!1),e.responseType="arraybuffer",e.send(null),new Uint8Array(e.response)}catch(e){var r=tryParseAsDataURI(A);if(r)return r;throw e}}),readAsync=function(A,e,r){var i=new XMLHttpRequest;i.open("GET",A,!0),i.responseType="arraybuffer",i.onload=function(){if(200==i.status||0==i.status&&i.response)e(i.response);else{var n=tryParseAsDataURI(A);n?e(n.buffer):r()}},i.onerror=r,i.send(null)},setWindowTitle=function(A){document.title=A});var out=Module.print||console.log.bind(console),err=Module.printErr||console.warn.bind(console);for(key in moduleOverrides)moduleOverrides.hasOwnProperty(key)&&(Module[key]=moduleOverrides[key]);moduleOverrides=null,Module.arguments&&(arguments_=Module.arguments),Module.thisProgram&&(thisProgram=Module.thisProgram),Module.quit&&(quit_=Module.quit);var STACK_ALIGN=16;function dynamicAlloc(A){var e=HEAP32[DYNAMICTOP_PTR>>2],r=e+A+15&-16;return HEAP32[DYNAMICTOP_PTR>>2]=r,e}function getNativeTypeSize(A){switch(A){case"i1":case"i8":return 1;case"i16":return 2;case"i32":return 4;case"i64":return 8;case"float":return 4;case"double":return 8;default:if("*"===A[A.length-1])return 4;if("i"===A[0]){var e=Number(A.substr(1));return assert(e%8==0,"getNativeTypeSize invalid bits "+e+", type "+A),e/8}return 0}}function warnOnce(A){warnOnce.shown||(warnOnce.shown={}),warnOnce.shown[A]||(warnOnce.shown[A]=1,err(A))}var jsCallStartIndex=1,functionPointers=new Array(0),funcWrappers={};function dynCall(A,e,r){return r&&r.length?Module["dynCall_"+A].apply(null,[e].concat(r)):Module["dynCall_"+A].call(null,e)}var tempRet0=0,setTempRet0=function(A){tempRet0=A},getTempRet0=function(){return tempRet0},GLOBAL_BASE=8,wasmBinary,noExitRuntime;function setValue(A,e,r,i){switch("*"===(r=r||"i8").charAt(r.length-1)&&(r="i32"),r){case"i1":case"i8":HEAP8[A>>0]=e;break;case"i16":HEAP16[A>>1]=e;break;case"i32":HEAP32[A>>2]=e;break;case"i64":tempI64=[e>>>0,(tempDouble=e,+Math_abs(tempDouble)>=1?tempDouble>0?(0|Math_min(+Math_floor(tempDouble/4294967296),4294967295))>>>0:~~+Math_ceil((tempDouble-+(~~tempDouble>>>0))/4294967296)>>>0:0)],HEAP32[A>>2]=tempI64[0],HEAP32[A+4>>2]=tempI64[1];break;case"float":HEAPF32[A>>2]=e;break;case"double":HEAPF64[A>>3]=e;break;default:abort("invalid type for setValue: "+r)}}Module.wasmBinary&&(wasmBinary=Module.wasmBinary),Module.noExitRuntime&&(noExitRuntime=Module.noExitRuntime);var ABORT=!1,EXITSTATUS=0;function assert(A,e){A||abort("Assertion failed: "+e)}function getCFunc(A){var e=Module["_"+A];return assert(e,"Cannot call unknown function "+A+", make sure it is exported"),e}function ccall(A,e,r,i,n){var t={string:function(A){var e=0;if(null!=A&&0!==A){var r=1+(A.length<<2);stringToUTF8(A,e=stackAlloc(r),r)}return e},array:function(A){var e=stackAlloc(A.length);return writeArrayToMemory(A,e),e}};var f=getCFunc(A),a=[],B=0;if(i)for(var o=0;o<i.length;o++){var u=t[r[o]];u?(0===B&&(B=stackSave()),a[o]=u(i[o])):a[o]=i[o]}var l=f.apply(null,a);return l=function(A){return"string"===e?UTF8ToString(A):"boolean"===e?Boolean(A):A}(l),0!==B&&stackRestore(B),l}var ALLOC_NONE=3;function getMemory(A){return runtimeInitialized?_malloc(A):dynamicAlloc(A)}var UTF8Decoder="undefined"!=typeof TextDecoder?new TextDecoder("utf8"):void 0;function UTF8ArrayToString(A,e,r){for(var i=e+r,n=e;A[n]&&!(n>=i);)++n;if(n-e>16&&A.subarray&&UTF8Decoder)return UTF8Decoder.decode(A.subarray(e,n));for(var t="";e<n;){var f=A[e++];if(128&f){var a=63&A[e++];if(192!=(224&f)){var B=63&A[e++];if((f=224==(240&f)?(15&f)<<12|a<<6|B:(7&f)<<18|a<<12|B<<6|63&A[e++])<65536)t+=String.fromCharCode(f);else{var o=f-65536;t+=String.fromCharCode(55296|o>>10,56320|1023&o)}}else t+=String.fromCharCode((31&f)<<6|a)}else t+=String.fromCharCode(f)}return t}function UTF8ToString(A,e){return A?UTF8ArrayToString(HEAPU8,A,e):""}function stringToUTF8Array(A,e,r,i){if(!(i>0))return 0;for(var n=r,t=r+i-1,f=0;f<A.length;++f){var a=A.charCodeAt(f);if(a>=55296&&a<=57343)a=65536+((1023&a)<<10)|1023&A.charCodeAt(++f);if(a<=127){if(r>=t)break;e[r++]=a}else if(a<=2047){if(r+1>=t)break;e[r++]=192|a>>6,e[r++]=128|63&a}else if(a<=65535){if(r+2>=t)break;e[r++]=224|a>>12,e[r++]=128|a>>6&63,e[r++]=128|63&a}else{if(r+3>=t)break;e[r++]=240|a>>18,e[r++]=128|a>>12&63,e[r++]=128|a>>6&63,e[r++]=128|63&a}}return e[r]=0,r-n}function stringToUTF8(A,e,r){return stringToUTF8Array(A,HEAPU8,e,r)}function lengthBytesUTF8(A){for(var e=0,r=0;r<A.length;++r){var i=A.charCodeAt(r);i>=55296&&i<=57343&&(i=65536+((1023&i)<<10)|1023&A.charCodeAt(++r)),i<=127?++e:e+=i<=2047?2:i<=65535?3:4}return e}var UTF16Decoder="undefined"!=typeof TextDecoder?new TextDecoder("utf-16le"):void 0,buffer,HEAP8,HEAPU8,HEAP16,HEAPU16,HEAP32,HEAPU32,HEAPF32,HEAPF64;function allocateUTF8(A){var e=lengthBytesUTF8(A)+1,r=_malloc(e);return r&&stringToUTF8Array(A,HEAP8,r,e),r}function writeArrayToMemory(A,e){HEAP8.set(A,e)}function writeAsciiToMemory(A,e,r){for(var i=0;i<A.length;++i)HEAP8[e++>>0]=A.charCodeAt(i);r||(HEAP8[e>>0]=0)}function updateGlobalBufferAndViews(A){buffer=A,Module.HEAP8=HEAP8=new Int8Array(A),Module.HEAP16=HEAP16=new Int16Array(A),Module.HEAP32=HEAP32=new Int32Array(A),Module.HEAPU8=HEAPU8=new Uint8Array(A),Module.HEAPU16=HEAPU16=new Uint16Array(A),Module.HEAPU32=HEAPU32=new Uint32Array(A),Module.HEAPF32=HEAPF32=new Float32Array(A),Module.HEAPF64=HEAPF64=new Float64Array(A)}var STACK_BASE=83008,DYNAMIC_BASE=8471616,DYNAMICTOP_PTR=82816,INITIAL_INITIAL_MEMORY=Module.INITIAL_MEMORY||16777216;function callRuntimeCallbacks(A){for(;A.length>0;){var e=A.shift();if("function"!=typeof e){var r=e.func;"number"==typeof r?void 0===e.arg?Module.dynCall_v(r):Module.dynCall_vi(r,e.arg):r(void 0===e.arg?null:e.arg)}else e(Module)}}buffer=Module.buffer?Module.buffer:new ArrayBuffer(INITIAL_INITIAL_MEMORY),INITIAL_INITIAL_MEMORY=buffer.byteLength,updateGlobalBufferAndViews(buffer),HEAP32[DYNAMICTOP_PTR>>2]=DYNAMIC_BASE;var __ATPRERUN__=[],__ATINIT__=[],__ATMAIN__=[],__ATEXIT__=[],__ATPOSTRUN__=[],runtimeInitialized=!1,runtimeExited=!1;function preRun(){if(Module.preRun)for("function"==typeof Module.preRun&&(Module.preRun=[Module.preRun]);Module.preRun.length;)addOnPreRun(Module.preRun.shift());callRuntimeCallbacks(__ATPRERUN__)}function initRuntime(){runtimeInitialized=!0,callRuntimeCallbacks(__ATINIT__)}function preMain(){callRuntimeCallbacks(__ATMAIN__)}function exitRuntime(){runtimeExited=!0}function postRun(){if(Module.postRun)for("function"==typeof Module.postRun&&(Module.postRun=[Module.postRun]);Module.postRun.length;)addOnPostRun(Module.postRun.shift());callRuntimeCallbacks(__ATPOSTRUN__)}function addOnPreRun(A){__ATPRERUN__.unshift(A)}function addOnPostRun(A){__ATPOSTRUN__.unshift(A)}var Math_abs=Math.abs,Math_ceil=Math.ceil,Math_floor=Math.floor,Math_min=Math.min,runDependencies=0,runDependencyWatcher=null,dependenciesFulfilled=null;function addRunDependency(A){runDependencies++,Module.monitorRunDependencies&&Module.monitorRunDependencies(runDependencies)}function removeRunDependency(A){if(runDependencies--,Module.monitorRunDependencies&&Module.monitorRunDependencies(runDependencies),0==runDependencies&&(null!==runDependencyWatcher&&(clearInterval(runDependencyWatcher),runDependencyWatcher=null),dependenciesFulfilled)){var e=dependenciesFulfilled;dependenciesFulfilled=null,e()}}function abort(A){throw Module.onAbort&&Module.onAbort(A),out(A+=""),err(A),ABORT=!0,EXITSTATUS=1,A="abort("+A+"). Build with -s ASSERTIONS=1 for more info."}Module.preloadedImages={},Module.preloadedAudios={};var memoryInitializer=null;function hasPrefix(A,e){return String.prototype.startsWith?A.startsWith(e):0===A.indexOf(e)}var dataURIPrefix="data:application/octet-stream;base64,";function isDataURI(A){return hasPrefix(A,dataURIPrefix)}var fileURIPrefix="file://",tempDouble,tempI64,ASM_CONSTS=[function(){return Module.getRandomValue()},function(){if(void 0===Module.getRandomValue)try{var window_="object"==typeof window?window:self,crypto_=void 0!==window_.crypto?window_.crypto:window_.msCrypto,randomValuesStandard=function(){var A=new Uint32Array(1);return crypto_.getRandomValues(A),A[0]>>>0};randomValuesStandard(),Module.getRandomValue=randomValuesStandard}catch(e){try{var crypto=eval("require")("crypto"),randomValueNodeJS=function(){var A=crypto.randomBytes(4);return(A[0]<<24|A[1]<<16|A[2]<<8|A[3])>>>0};randomValueNodeJS(),Module.getRandomValue=randomValueNodeJS}catch(A){throw"No secure random number generator found"}}}];function _emscripten_asm_const_i(A){return ASM_CONSTS[A]()}__ATINIT__.push({func:function(){___emscripten_environ_constructor()}}),memoryInitializer="data:application/octet-stream;base64,AAAAAAAAAAAAAAAAAAAAAAAAAAB2OgEApMsAAAAAAAAAAAAABAAAAOrSAAAMywAAAAAAAAAAAAANAAAAAQAAAEAAAAAGKAAAAAAAAA4AAAACAAAAAgAAAAIAAAACAAAADwAAAAEAAACAAAAABigAAAAAAAAQAAAAAQAAAIAAAAAGKAAAAAAAABEAAAABAAAAQAAAAAYoAAAAAAAAEgAAAAEAAABAAAAABigAAAAAAAAwAAAAAQAAAIAAAAAQAAAAAgAAADEAAAABAAAA/////xYoAAAAAAAANgAAAAEAAAD/////FigAAAAAAAA3AAAAAQAAAP////8GKAAAAAAAAGMAAAABAAAAAIAAAAYoAAAAAAAAZAAAAAEAAAAAgAAABigAAAAAAABlAAAAAQAAAACAAAAGKAAAAAAAAGkAAAABAAAAQAAAAAIAAAACAAAAnAAAAP//////////AAgAAAIAAACtAAAAAQAAAACAAAAGKAAAAAAAAK4AAAD//////////wIAAAACAAAAhwEAAAEAAAD/////EAAAAAIAAAChAQAA//////////8ACAAAAgAAAMwBAAABAAAAAAEAABAAAAACAAAAvQMAAAIAAAACAAAAAgAAAAIAAADsAwAAAQAAAAwAAAABAAAAAgAAAO0DAAABAAAADQAAAAEAAAACAAAA7gMAAAEAAAALAAAAAQAAAAIAAABE0wAABAAAAAEAAABJ0wAABwAAAAEAAAAm+gAABAAAAAUAAABR0wAAAwAAAAIAAABV0wAABwAAAAIAAABd0wAABAAAAAoAAABi0wAACgAAAAoAAABt0wAAAwAAAAYAAABx0wAABgAAAAYAAAB40wAABwAAABcAAACA0wAAAwAAABcAAACE0wAADwAAABgAAACU0wAABwAAABgAAACc0wAAAwAAAAQAAACg0wAACwAAAAQAAACs0wAABgAAAAMAAACz0wAACQAAAAMAAAC90wAADwAAABwAAADN0wAABAAAABwAAADS0wAAAwAAABYAAADW0wAACQAAABYAAAA/0wAABAAAAAwAAADg0wAACgAAAAwAAADr0wAAAwAAAB4AAADv0wAACQAAAB4AAAD50wAADQAAABoAAAAH1AAABwAAABoAAAAP1AAADwAAABMAAAAf1AAACQAAABMAAAAp1AAAAwAAABQAAAAt1AAA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qQIAAKoCAACrAgAAoQEAAFgDAACGAQAAWwAAAM0DAAA7AQAAPAEAAD0BAAA+AQAAPwEAAEABAABBAQAAQgEAAG0BAABuAQAAbwEAAHABAABxAQAAcgEAAHMBAAB0AQAAdQEAAHYBAAB3AQAAmQMAAJoDAACbAwAAnAMAAJ0DAACeAwAAnwMAAKADAAChAwAAogMAAKMDAACkAwAApQMAAKYDAACoAwAArQMAAKIBAACjAQAApAEAAKUBAAAUAwAAfwMAAIADAACBAwAApgEAAKcBAACoAQAAqQEAABUDAACCAwAAgwMAAIQDAACqAQAAqwEAAKwBAACtAQAAFgMAAIUDAACGAwAAhwMAAKACAAChAgAAogIAAKMCAAAiAwAAIwMAAEcAAABIAAAASQAAAEoAAABLAAAATAAAAE0AAABOAAAATwAAAIsAAADKAQAAywEAAMwBAADNAQAAzgEAAM8BAADQAQAA0QEAANIBAADTAQAA1AEAANUBAADWAQAA1wEAANgBAADZAQAA2gEAANsBAADcAQAA3QEAAIcBAADeAQAA3wEAAOABAADhAQAA4gEAAOMBAADkAQAA5QEAAOYBAADnAQAA6AEAAOkBAADqAQAAZgAAAOsBAADsAQAA7QEAAO4BAADvAQAA8AEAAPEBAADyAQAA8wEAAPQBAAD1AQAA9gEAALoBAAC7AQAAvAEAAL0BAAC+AQAAvwEAAMABAADBAQAAiAEAAMIBAADDAQAAxAEAAMUBAADGAQAAxwEAAMgBAADJAQAAvQAAAL4AAAC/AAAAwAAAAMEAAADCAAAAwwAAAJ4AAACfAAAAoAAAAJAAAACRAAAAkgAAAJMAAACUAAAAlQAAAKsAAACGAAAAhwAAAIgAAACJAAAAigAAAIgCAACJAgAAtwMAALgDAAC5AwAAugMAAO8CAADwAgAA8QIAAIsDAACMAwAAjQMAAMQAAADFAAAAxgAAAMcAAADIAAAAyQAAAMoAAADLAAAAzAAAAM0AAADOAAAAzwAAANAAAADRAAAA0gAAANMAAAASAwAAIgQAACMEAAATAwAAJAQAANQAAADVAAAA1gAAANcAAADYAAAA2QAAANoAAADbAAAA3AAAAN0AAADeAAAA3wAAAOAAAADhAAAA4gAAAOMAAADkAAAA5QAAAOYAAADnAAAA6AAAAOkAAADqAAAA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ABAAEAAQABAAEAAQABAAEAAQABAAEAAQABAAEAAQABAAEAAQ/////////////////////////////////////////////////////////////////wABAgMEBQYHCAn/////////CgsMDQ4PEBESExQVFhcYGRobHB0eHyAhIiP///////8KCwwNDg8QERITFBUWFxgZGhscHR4fICEiI/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////8AAAAAAAAAAAAAAAAAAAAMBAAAAAAAAAEAAAAAAAAAAQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABAAAAAQAAAAIAAAD9AwAAAAAAAAIAAAAAAAAAAgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAADAAAAAgAAAAQAAAAAAAAAAgAAAAAAAAAAAAAAlMoAAAAAAADS0gAAAAAAAAAAAAABAAAAAQAAAAAAAAABAAAAAwAAAAEAAAAAAAAAAgAAAAAAAAAAAAAA0MoAAAEAAADZ0gAAAAAAAAAAAAACAAAAAQAAAAAAAAACAAAAAwAAAAEAAAABAAAAEAAAABAAAAACAAAAAAAAAAgAAADh0gAAAAAAAAQAAAAAAAAAAAAAAAAAAAAAAAAA+9IAAAAAAAACAAAAAAAAAAAAAAAAAAAAAAAAAA3TAAAAAAAAAwAAAAAAAAAAAAAAAAAAAAAAAAAa0wAAAAAAAAYAAAAAAAAAAAAAAAAAAAAAAAAAKtMAAAAAAAD8////AAAAAAAAAAAAAAAAAAAAADbTAABABQAAsAUAAGjMAAABAAAAEAAAAPAEAAACAAAAAAAAAAgAAAAY1QAAAAAAAAIAAAAAAAAAAAAAANzLAAD///9/M9UAAAAAAAAAAAAAAwAAAAIAAAACAAAAAwAAAAQAAAACAAAAVwMAAAAAAAADAAAABAAAAAMAAAAAAAAAAAAAAAAAAAAFAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABgAAAAUAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAYAAAAFAAAAVwMAAFcDAAAAAAAA3yABAHXVAAAAAAAAAAAAAAcAAAAAAAAAAAAAAAAAAAAAAAAABAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAQAAAAHAAAABgAAAAgAAAAAAAAAAAAAAEvWAAADAAAABAAAAAUAAAAGAAAACAAAAAQAAAAFAAAABgAAAAAEAAAAAAAAAAAAAAAAAAAAAAAAAQAAABAAAAAgBgAACQAAACzNAABYAAAAStoAAAAAAAAAAAAAAAAAAAAAAAAMAAAAAAAAAAEAAAAQAAAA4AYAAAIAAABgzQAAWAAAAEraAAAAAAAAAAAAAAAAAAAAAAAADAAAAAAAAAABAAAAEAAAABAHAAAEAAAAAAAAABAAAADd1wAABgAAAAIAAAAKAAAADwAAAAYAAAAAAAAAAAAAAAAAAAAQAAAAAAAAAAgAAAAAAAAACQAAAAAAAAAKAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAALAAAAAAAAAAwAAAAAAAAAAAAAAA0AAAAIAAAAqNgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAlBAAACQAAAAEAAAAQAAAAILUAAAIAAAAAAAAACAAAAEU6AQABAAAAEAAAAFC1AAACAAAAbM4AAAwAAABqOgEAAAAAAAAAAAAAAAAAAAAAAA4AAAAAAAAABAAAAAgAAAAQAAAAAAAAAAsAAAAKAAAAEQAAAAAAAAAAAAAAQAAAAGAAAAAAAAAAQAAAAEEAAAAUAAAACAAAAAwAAAALAAAAEgAAAAAAAAAAAAAAQAAAAGQAAAAPAAAA+M4AAAEAAAABAAAAAAAAAAIAAAABAAAAAQAAAP////+rOwEADQAAAA4AAAAPAAAAEAAAAAwAAAATAAAAFAAAABUAAAANAAAAInsgcmV0dXJuIE1vZHVsZS5nZXRSYW5kb21WYWx1ZSgpOyB9IgB7IGlmIChNb2R1bGUuZ2V0UmFuZG9tVmFsdWUgPT09IHVuZGVmaW5lZCkgeyB0cnkgeyB2YXIgd2luZG93XyA9ICdvYmplY3QnID09PSB0eXBlb2Ygd2luZG93ID8gd2luZG93IDogc2VsZjsgdmFyIGNyeXB0b18gPSB0eXBlb2Ygd2luZG93Xy5jcnlwdG8gIT09ICd1bmRlZmluZWQnID8gd2luZG93Xy5jcnlwdG8gOiB3aW5kb3dfLm1zQ3J5cHRvOyB2YXIgcmFuZG9tVmFsdWVzU3RhbmRhcmQgPSBmdW5jdGlvbigpIHsgdmFyIGJ1ZiA9IG5ldyBVaW50MzJBcnJheSgxKTsgY3J5cHRvXy5nZXRSYW5kb21WYWx1ZXMoYnVmKTsgcmV0dXJuIGJ1ZlswXSA+Pj4gMDsgfTsgcmFuZG9tVmFsdWVzU3RhbmRhcmQoKTsgTW9kdWxlLmdldFJhbmRvbVZhbHVlID0gcmFuZG9tVmFsdWVzU3RhbmRhcmQ7IH0gY2F0Y2ggKGUpIHsgdHJ5IHsgdmFyIGNyeXB0byA9IHJlcXVpcmUoJ2NyeXB0bycpOyB2YXIgcmFuZG9tVmFsdWVOb2RlSlMgPSBmdW5jdGlvbigpIHsgdmFyIGJ1ZiA9IGNyeXB0b1sncmFuZG9tQnl0ZXMnXSg0KTsgcmV0dXJuIChidWZbMF0gPDwgMjQgfCBidWZbMV0gPDwgMTYgfCBidWZbMl0gPDwgOCB8IGJ1ZlszXSkgPj4+IDA7IH07IHJhbmRvbVZhbHVlTm9kZUpTKCk7IE1vZHVsZS5nZXRSYW5kb21WYWx1ZSA9IHJhbmRvbVZhbHVlTm9kZUpTOyB9IGNhdGNoIChlKSB7IHRocm93ICdObyBzZWN1cmUgcmFuZG9tIG51bWJlciBnZW5lcmF0b3IgZm91bmQnOyB9IH0gfSB9AHNhbHQAaGV4c2FsdABoZXhpbmZvAG1kAHNlY3JldABoZXhzZWNyZXQAc2VlZABoZXhzZWVkAGFzc2VydGlvbiBmYWlsZWQ6IGNodW5rID49IDAAYXNzZXJ0aW9uIGZhaWxlZDogKih1bnNpZ25lZCBpbnQgKilsb2NrID09IDEAc3RibF9zZWN0aW9uAG1pbgBtYXgAbWFzawBmbGFncwBub21hc2sAZmllbGQ9AEJJR05VTQBDQklHTlVNAFg1MDlfU0lHAGRpZ2VzdAA8SU5WQUxJRD4AQVNOMV9PQ1RFVF9TVFJJTkcAQVNOMV9JTlRFR0VSAEFTTjFfQklUX1NUUklORwBBU04xX09CSkVDVABBU04xX0FOWQBVVEY4AEJPT0wAQk9PTEVBTgBJTlQASU5URUdFUgBFTlVNAEVOVU1FUkFURUQAT0lEAE9CSkVDVABVVENUSU1FAFVUQwBHRU5FUkFMSVpFRFRJTUUAR0VOVElNRQBPQ1QAT0NURVRTVFJJTkcAQklUU1RSAEJJVFNUUklORwBVTklWRVJTQUxTVFJJTkcAVU5JVgBJQTUASUE1U1RSSU5HAFVURjhTdHJpbmcAQk1QAEJNUFNUUklORwBWSVNJQkxFU1RSSU5HAFZJU0lCTEUAUFJJTlRBQkxFU1RSSU5HAFBSSU5UQUJMRQBUNjEAVDYxU1RSSU5HAFRFTEVURVhTVFJJTkcAR2VuZXJhbFN0cmluZwBHRU5TVFIATlVNRVJJQwBOVU1FUklDU1RSSU5HAFNFUVVFTkNFAFNFUQBTRVQARVhQAEVYUExJQ0lUAElNUABJTVBMSUNJVABPQ1RXUkFQAFNFUVdSQVAAU0VUV1JBUABCSVRXUkFQAEZPUk0ARk9STUFUAERJUgAlcyAwCgAlcyAlcyVsdSAoJXMweCVseCkKACAoTmVnYXRpdmUpACVzJXMKAG9pZF9zZWN0aW9uAEZpZWxkPQAsIFR5cGU9AFR5cGU9AFg1MDlfQUxHT1IAcGFyYW1ldGVyADAwAFwKAExPTkcAJWxkCgBrZXkAaGV4a2V5AGFzc2VydGlvbiBmYWlsZWQ6IGogPD0gKGludClzaXplb2YoY3R4LT5rZXkpAE9wZW5TU0wgSE1BQyBtZXRob2QAJTA0eCAtIAAlYwAgICAAJTAyeCVjACAgACAAPE5VTEw+ADAxMjM0NTY3ODkAMHgAMDEyMzQ1Njc4OWFiY2RlZgBhc3NlcnRpb24gZmFpbGVkOiAqc2J1ZmZlciAhPSBOVUxMIHx8IGJ1ZmZlciAhPSBOVUxMAGFzc2VydGlvbiBmYWlsZWQ6ICpjdXJybGVuIDw9ICptYXhsZW4AYXNzZXJ0aW9uIGZhaWxlZDogKnNidWZmZXIgIT0gTlVMTABOQQBPcGVuU1NMIFBLQ1MjMSBSU0EAT3BlblNTTCBSU0EgbWV0aG9kACAoSU5WQUxJRCBQU1MgUEFSQU1FVEVSUykKAEhhc2ggQWxnb3JpdGhtOiAAc2hhMSAoZGVmYXVsdCkATWFzayBBbGdvcml0aG06IAAgd2l0aCAASU5WQUxJRABtZ2YxIHdpdGggc2hhMSAoZGVmYXVsdCkAU2FsdCBMZW5ndGg6IDB4ADE0IChkZWZhdWx0KQBUcmFpbGVyIEZpZWxkOiAweABCQyAoZGVmYXVsdCkAUHJpdmF0ZS1LZXk6ICglZCBiaXQpCgBQdWJsaWMtS2V5OiAoJWQgYml0KQoAbW9kdWx1czoATW9kdWx1czoAcHVibGljRXhwb25lbnQ6AEV4cG9uZW50OgBwcml2YXRlRXhwb25lbnQ6AHByaW1lMToAcHJpbWUyOgBleHBvbmVudDE6AGV4cG9uZW50MjoAY29lZmZpY2llbnQ6AHZlcnNpb24AZQBkAHAAcQBkbXAxAGRtcTEAaXFtcABSU0FfUFNTX1BBUkFNUwBoYXNoQWxnb3JpdGhtAG1hc2tHZW5BbGdvcml0aG0Ac2FsdExlbmd0aAB0cmFpbGVyRmllbGQAcnNhX3BhZGRpbmdfbW9kZQBzc2x2MjMAbm9uZQBvZWFwAG9hZXAAeDkzMQByc2FfcHNzX3NhbHRsZW4AcnNhX2tleWdlbl9iaXRzAHJzYV9rZXlnZW5fcHViZXhwAHJzYV9tZ2YxX21kAHJzYV9vYWVwX21kAHJzYV9vYWVwX2xhYmVsAE5VTEwgc2hhcmVkIGxpYnJhcnkgbWV0aG9kAFVOREVGAHVuZGVmaW5lZAByc2Fkc2kAUlNBIERhdGEgU2VjdXJpdHksIEluYy4AcGtjcwBSU0EgRGF0YSBTZWN1cml0eSwgSW5jLiBQS0NTAE1EMgBtZDIATUQ1AG1kNQBSQzQAcmM0AHJzYUVuY3J5cHRpb24AUlNBLU1EMgBtZDJXaXRoUlNBRW5jcnlwdGlvbgBSU0EtTUQ1AG1kNVdpdGhSU0FFbmNyeXB0aW9uAFBCRS1NRDItREVTAHBiZVdpdGhNRDJBbmRERVMtQ0JDAFBCRS1NRDUtREVTAHBiZVdpdGhNRDVBbmRERVMtQ0JDAFg1MDAAZGlyZWN0b3J5IHNlcnZpY2VzIChYLjUwMCkAWDUwOQBDTgBjb21tb25OYW1lAEMAY291bnRyeU5hbWUATABsb2NhbGl0eU5hbWUAU1QAc3RhdGVPclByb3ZpbmNlTmFtZQBPAG9yZ2FuaXphdGlvbk5hbWUAT1UAb3JnYW5pemF0aW9uYWxVbml0TmFtZQBSU0EAcnNhAHBrY3M3AHBrY3M3LWRhdGEAcGtjczctc2lnbmVkRGF0YQBwa2NzNy1lbnZlbG9wZWREYXRhAHBrY3M3LXNpZ25lZEFuZEVudmVsb3BlZERhdGEAcGtjczctZGlnZXN0RGF0YQBwa2NzNy1lbmNyeXB0ZWREYXRhAHBrY3MzAGRoS2V5QWdyZWVtZW50AERFUy1FQ0IAZGVzLWVjYgBERVMtQ0ZCAGRlcy1jZmIAREVTLUNCQwBkZXMtY2JjAERFUy1FREUAZGVzLWVkZQBERVMtRURFMwBkZXMtZWRlMwBJREVBLUNCQwBpZGVhLWNiYwBJREVBLUNGQgBpZGVhLWNmYgBJREVBLUVDQgBpZGVhLWVjYgBSQzItQ0JDAHJjMi1jYmMAUkMyLUVDQgByYzItZWNiAFJDMi1DRkIAcmMyLWNmYgBSQzItT0ZCAHJjMi1vZmIAU0hBAHNoYQBSU0EtU0hBAHNoYVdpdGhSU0FFbmNyeXB0aW9uAERFUy1FREUtQ0JDAGRlcy1lZGUtY2JjAERFUy1FREUzLUNCQwBkZXMtZWRlMy1jYmMAREVTLU9GQgBkZXMtb2ZiAElERUEtT0ZCAGlkZWEtb2ZiAHBrY3M5AGVtYWlsQWRkcmVzcwB1bnN0cnVjdHVyZWROYW1lAGNvbnRlbnRUeXBlAG1lc3NhZ2VEaWdlc3QAc2lnbmluZ1RpbWUAY291bnRlcnNpZ25hdHVyZQBjaGFsbGVuZ2VQYXNzd29yZAB1bnN0cnVjdHVyZWRBZGRyZXNzAGV4dGVuZGVkQ2VydGlmaWNhdGVBdHRyaWJ1dGVzAE5ldHNjYXBlAE5ldHNjYXBlIENvbW11bmljYXRpb25zIENvcnAuAG5zQ2VydEV4dABOZXRzY2FwZSBDZXJ0aWZpY2F0ZSBFeHRlbnNpb24AbnNEYXRhVHlwZQBOZXRzY2FwZSBEYXRhIFR5cGUAREVTLUVERS1DRkIAZGVzLWVkZS1jZmIAREVTLUVERTMtQ0ZCAGRlcy1lZGUzLWNmYgBERVMtRURFLU9GQgBkZXMtZWRlLW9mYgBERVMtRURFMy1PRkIAZGVzLWVkZTMtb2ZiAFNIQTEAc2hhMQBSU0EtU0hBMQBzaGExV2l0aFJTQUVuY3J5cHRpb24ARFNBLVNIQQBkc2FXaXRoU0hBAERTQS1vbGQAZHNhRW5jcnlwdGlvbi1vbGQAUEJFLVNIQTEtUkMyLTY0AHBiZVdpdGhTSEExQW5kUkMyLUNCQwBQQktERjIARFNBLVNIQTEtb2xkAGRzYVdpdGhTSEExLW9sZABuc0NlcnRUeXBlAE5ldHNjYXBlIENlcnQgVHlwZQBuc0Jhc2VVcmwATmV0c2NhcGUgQmFzZSBVcmwAbnNSZXZvY2F0aW9uVXJsAE5ldHNjYXBlIFJldm9jYXRpb24gVXJsAG5zQ2FSZXZvY2F0aW9uVXJsAE5ldHNjYXBlIENBIFJldm9jYXRpb24gVXJsAG5zUmVuZXdhbFVybABOZXRzY2FwZSBSZW5ld2FsIFVybABuc0NhUG9saWN5VXJsAE5ldHNjYXBlIENBIFBvbGljeSBVcmwAbnNTc2xTZXJ2ZXJOYW1lAE5ldHNjYXBlIFNTTCBTZXJ2ZXIgTmFtZQBuc0NvbW1lbnQATmV0c2NhcGUgQ29tbWVudABuc0NlcnRTZXF1ZW5jZQBOZXRzY2FwZSBDZXJ0aWZpY2F0ZSBTZXF1ZW5jZQBERVNYLUNCQwBkZXN4LWNiYwBpZC1jZQBzdWJqZWN0S2V5SWRlbnRpZmllcgBYNTA5djMgU3ViamVjdCBLZXkgSWRlbnRpZmllcgBrZXlVc2FnZQBYNTA5djMgS2V5IFVzYWdlAHByaXZhdGVLZXlVc2FnZVBlcmlvZABYNTA5djMgUHJpdmF0ZSBLZXkgVXNhZ2UgUGVyaW9kAHN1YmplY3RBbHROYW1lAFg1MDl2MyBTdWJqZWN0IEFsdGVybmF0aXZlIE5hbWUAaXNzdWVyQWx0TmFtZQBYNTA5djMgSXNzdWVyIEFsdGVybmF0aXZlIE5hbWUAYmFzaWNDb25zdHJhaW50cwBYNTA5djMgQmFzaWMgQ29uc3RyYWludHMAY3JsTnVtYmVyAFg1MDl2MyBDUkwgTnVtYmVyAGNlcnRpZmljYXRlUG9saWNpZXMAWDUwOXYzIENlcnRpZmljYXRlIFBvbGljaWVzAGF1dGhvcml0eUtleUlkZW50aWZpZXIAWDUwOXYzIEF1dGhvcml0eSBLZXkgSWRlbnRpZmllcgBCRi1DQkMAYmYtY2JjAEJGLUVDQgBiZi1lY2IAQkYtQ0ZCAGJmLWNmYgBCRi1PRkIAYmYtb2ZiAE1EQzIAbWRjMgBSU0EtTURDMgBtZGMyV2l0aFJTQQBSQzQtNDAAcmM0LTQwAFJDMi00MC1DQkMAcmMyLTQwLWNiYwBHTgBnaXZlbk5hbWUAU04Ac3VybmFtZQBpbml0aWFscwB1aWQAdW5pcXVlSWRlbnRpZmllcgBjcmxEaXN0cmlidXRpb25Qb2ludHMAWDUwOXYzIENSTCBEaXN0cmlidXRpb24gUG9pbnRzAFJTQS1OUC1NRDUAbWQ1V2l0aFJTQQBzZXJpYWxOdW1iZXIAdGl0bGUAZGVzY3JpcHRpb24AQ0FTVDUtQ0JDAGNhc3Q1LWNiYwBDQVNUNS1FQ0IAY2FzdDUtZWNiAENBU1Q1LUNGQgBjYXN0NS1jZmIAQ0FTVDUtT0ZCAGNhc3Q1LW9mYgBwYmVXaXRoTUQ1QW5kQ2FzdDVDQkMARFNBLVNIQTEAZHNhV2l0aFNIQTEATUQ1LVNIQTEAbWQ1LXNoYTEAUlNBLVNIQTEtMgBzaGExV2l0aFJTQQBEU0EAZHNhRW5jcnlwdGlvbgBSSVBFTUQxNjAAcmlwZW1kMTYwAFJTQS1SSVBFTUQxNjAAcmlwZW1kMTYwV2l0aFJTQQBSQzUtQ0JDAHJjNS1jYmMAUkM1LUVDQgByYzUtZWNiAFJDNS1DRkIAcmM1LWNmYgBSQzUtT0ZCAHJjNS1vZmIAWkxJQgB6bGliIGNvbXByZXNzaW9uAGV4dGVuZGVkS2V5VXNhZ2UAWDUwOXYzIEV4dGVuZGVkIEtleSBVc2FnZQBQS0lYAGlkLWtwAHNlcnZlckF1dGgAVExTIFdlYiBTZXJ2ZXIgQXV0aGVudGljYXRpb24AY2xpZW50QXV0aABUTFMgV2ViIENsaWVudCBBdXRoZW50aWNhdGlvbgBjb2RlU2lnbmluZwBDb2RlIFNpZ25pbmcAZW1haWxQcm90ZWN0aW9uAEUtbWFpbCBQcm90ZWN0aW9uAHRpbWVTdGFtcGluZwBUaW1lIFN0YW1waW5nAG1zQ29kZUluZABNaWNyb3NvZnQgSW5kaXZpZHVhbCBDb2RlIFNpZ25pbmcAbXNDb2RlQ29tAE1pY3Jvc29mdCBDb21tZXJjaWFsIENvZGUgU2lnbmluZwBtc0NUTFNpZ24ATWljcm9zb2Z0IFRydXN0IExpc3QgU2lnbmluZwBtc1NHQwBNaWNyb3NvZnQgU2VydmVyIEdhdGVkIENyeXB0bwBtc0VGUwBNaWNyb3NvZnQgRW5jcnlwdGVkIEZpbGUgU3lzdGVtAG5zU0dDAE5ldHNjYXBlIFNlcnZlciBHYXRlZCBDcnlwdG8AZGVsdGFDUkwAWDUwOXYzIERlbHRhIENSTCBJbmRpY2F0b3IAQ1JMUmVhc29uAFg1MDl2MyBDUkwgUmVhc29uIENvZGUAaW52YWxpZGl0eURhdGUASW52YWxpZGl0eSBEYXRlAFNYTmV0SUQAU3Ryb25nIEV4dHJhbmV0IElEAFBCRS1TSEExLVJDNC0xMjgAcGJlV2l0aFNIQTFBbmQxMjhCaXRSQzQAUEJFLVNIQTEtUkM0LTQwAHBiZVdpdGhTSEExQW5kNDBCaXRSQzQAUEJFLVNIQTEtM0RFUwBwYmVXaXRoU0hBMUFuZDMtS2V5VHJpcGxlREVTLUNCQwBQQkUtU0hBMS0yREVTAHBiZVdpdGhTSEExQW5kMi1LZXlUcmlwbGVERVMtQ0JDAFBCRS1TSEExLVJDMi0xMjgAcGJlV2l0aFNIQTFBbmQxMjhCaXRSQzItQ0JDAFBCRS1TSEExLVJDMi00MABwYmVXaXRoU0hBMUFuZDQwQml0UkMyLUNCQwBrZXlCYWcAcGtjczhTaHJvdWRlZEtleUJhZwBjZXJ0QmFnAGNybEJhZwBzZWNyZXRCYWcAc2FmZUNvbnRlbnRzQmFnAGZyaWVuZGx5TmFtZQBsb2NhbEtleUlEAHg1MDlDZXJ0aWZpY2F0ZQBzZHNpQ2VydGlmaWNhdGUAeDUwOUNybABQQkVTMgBQQk1BQzEAaG1hY1dpdGhTSEExAGlkLXF0LWNwcwBQb2xpY3kgUXVhbGlmaWVyIENQUwBpZC1xdC11bm90aWNlAFBvbGljeSBRdWFsaWZpZXIgVXNlciBOb3RpY2UAUkMyLTY0LUNCQwByYzItNjQtY2JjAFNNSU1FLUNBUFMAUy9NSU1FIENhcGFiaWxpdGllcwBQQkUtTUQyLVJDMi02NABwYmVXaXRoTUQyQW5kUkMyLUNCQwBQQkUtTUQ1LVJDMi02NABwYmVXaXRoTUQ1QW5kUkMyLUNCQwBQQkUtU0hBMS1ERVMAcGJlV2l0aFNIQTFBbmRERVMtQ0JDAG1zRXh0UmVxAE1pY3Jvc29mdCBFeHRlbnNpb24gUmVxdWVzdABleHRSZXEARXh0ZW5zaW9uIFJlcXVlc3QAbmFtZQBkblF1YWxpZmllcgBpZC1wZQBpZC1hZABhdXRob3JpdHlJbmZvQWNjZXNzAEF1dGhvcml0eSBJbmZvcm1hdGlvbiBBY2Nlc3MAT0NTUABjYUlzc3VlcnMAQ0EgSXNzdWVycwBPQ1NQU2lnbmluZwBPQ1NQIFNpZ25pbmcASVNPAGlzbwBtZW1iZXItYm9keQBJU08gTWVtYmVyIEJvZHkASVNPLVVTAElTTyBVUyBNZW1iZXIgQm9keQBYOS01NwBYOS41NwBYOWNtAFg5LjU3IENNID8AcGtjczEAcGtjczUAU01JTUUAUy9NSU1FAGlkLXNtaW1lLW1vZABpZC1zbWltZS1jdABpZC1zbWltZS1hYQBpZC1zbWltZS1hbGcAaWQtc21pbWUtY2QAaWQtc21pbWUtc3BxAGlkLXNtaW1lLWN0aQBpZC1zbWltZS1tb2QtY21zAGlkLXNtaW1lLW1vZC1lc3MAaWQtc21pbWUtbW9kLW9pZABpZC1zbWltZS1tb2QtbXNnLXYzAGlkLXNtaW1lLW1vZC1ldHMtZVNpZ25hdHVyZS04OABpZC1zbWltZS1tb2QtZXRzLWVTaWduYXR1cmUtOTcAaWQtc21pbWUtbW9kLWV0cy1lU2lnUG9saWN5LTg4AGlkLXNtaW1lLW1vZC1ldHMtZVNpZ1BvbGljeS05NwBpZC1zbWltZS1jdC1yZWNlaXB0AGlkLXNtaW1lLWN0LWF1dGhEYXRhAGlkLXNtaW1lLWN0LXB1Ymxpc2hDZXJ0AGlkLXNtaW1lLWN0LVRTVEluZm8AaWQtc21pbWUtY3QtVERUSW5mbwBpZC1zbWltZS1jdC1jb250ZW50SW5mbwBpZC1zbWltZS1jdC1EVkNTUmVxdWVzdERhdGEAaWQtc21pbWUtY3QtRFZDU1Jlc3BvbnNlRGF0YQBpZC1zbWltZS1hYS1yZWNlaXB0UmVxdWVzdABpZC1zbWltZS1hYS1zZWN1cml0eUxhYmVsAGlkLXNtaW1lLWFhLW1sRXhwYW5kSGlzdG9yeQBpZC1zbWltZS1hYS1jb250ZW50SGludABpZC1zbWltZS1hYS1tc2dTaWdEaWdlc3QAaWQtc21pbWUtYWEtZW5jYXBDb250ZW50VHlwZQBpZC1zbWltZS1hYS1jb250ZW50SWRlbnRpZmllcgBpZC1zbWltZS1hYS1tYWNWYWx1ZQBpZC1zbWltZS1hYS1lcXVpdmFsZW50TGFiZWxzAGlkLXNtaW1lLWFhLWNvbnRlbnRSZWZlcmVuY2UAaWQtc21pbWUtYWEtZW5jcnlwS2V5UHJlZgBpZC1zbWltZS1hYS1zaWduaW5nQ2VydGlmaWNhdGUAaWQtc21pbWUtYWEtc21pbWVFbmNyeXB0Q2VydHMAaWQtc21pbWUtYWEtdGltZVN0YW1wVG9rZW4AaWQtc21pbWUtYWEtZXRzLXNpZ1BvbGljeUlkAGlkLXNtaW1lLWFhLWV0cy1jb21taXRtZW50VHlwZQBpZC1zbWltZS1hYS1ldHMtc2lnbmVyTG9jYXRpb24AaWQtc21pbWUtYWEtZXRzLXNpZ25lckF0dHIAaWQtc21pbWUtYWEtZXRzLW90aGVyU2lnQ2VydABpZC1zbWltZS1hYS1ldHMtY29udGVudFRpbWVzdGFtcABpZC1zbWltZS1hYS1ldHMtQ2VydGlmaWNhdGVSZWZzAGlkLXNtaW1lLWFhLWV0cy1SZXZvY2F0aW9uUmVmcwBpZC1zbWltZS1hYS1ldHMtY2VydFZhbHVlcwBpZC1zbWltZS1hYS1ldHMtcmV2b2NhdGlvblZhbHVlcwBpZC1zbWltZS1hYS1ldHMtZXNjVGltZVN0YW1wAGlkLXNtaW1lLWFhLWV0cy1jZXJ0Q1JMVGltZXN0YW1wAGlkLXNtaW1lLWFhLWV0cy1hcmNoaXZlVGltZVN0YW1wAGlkLXNtaW1lLWFhLXNpZ25hdHVyZVR5cGUAaWQtc21pbWUtYWEtZHZjcy1kdmMAaWQtc21pbWUtYWxnLUVTREh3aXRoM0RFUwBpZC1zbWltZS1hbGctRVNESHdpdGhSQzIAaWQtc21pbWUtYWxnLTNERVN3cmFwAGlkLXNtaW1lLWFsZy1SQzJ3cmFwAGlkLXNtaW1lLWFsZy1FU0RIAGlkLXNtaW1lLWFsZy1DTVMzREVTd3JhcABpZC1zbWltZS1hbGctQ01TUkMyd3JhcABpZC1zbWltZS1jZC1sZGFwAGlkLXNtaW1lLXNwcS1ldHMtc3F0LXVyaQBpZC1zbWltZS1zcHEtZXRzLXNxdC11bm90aWNlAGlkLXNtaW1lLWN0aS1ldHMtcHJvb2ZPZk9yaWdpbgBpZC1zbWltZS1jdGktZXRzLXByb29mT2ZSZWNlaXB0AGlkLXNtaW1lLWN0aS1ldHMtcHJvb2ZPZkRlbGl2ZXJ5AGlkLXNtaW1lLWN0aS1ldHMtcHJvb2ZPZlNlbmRlcgBpZC1zbWltZS1jdGktZXRzLXByb29mT2ZBcHByb3ZhbABpZC1zbWltZS1jdGktZXRzLXByb29mT2ZDcmVhdGlvbgBNRDQAbWQ0AGlkLXBraXgtbW9kAGlkLXF0AGlkLWl0AGlkLXBraXAAaWQtYWxnAGlkLWNtYwBpZC1vbgBpZC1wZGEAaWQtYWNhAGlkLXFjcwBpZC1jY3QAaWQtcGtpeDEtZXhwbGljaXQtODgAaWQtcGtpeDEtaW1wbGljaXQtODgAaWQtcGtpeDEtZXhwbGljaXQtOTMAaWQtcGtpeDEtaW1wbGljaXQtOTMAaWQtbW9kLWNybWYAaWQtbW9kLWNtYwBpZC1tb2Qta2VhLXByb2ZpbGUtODgAaWQtbW9kLWtlYS1wcm9maWxlLTkzAGlkLW1vZC1jbXAAaWQtbW9kLXF1YWxpZmllZC1jZXJ0LTg4AGlkLW1vZC1xdWFsaWZpZWQtY2VydC05MwBpZC1tb2QtYXR0cmlidXRlLWNlcnQAaWQtbW9kLXRpbWVzdGFtcC1wcm90b2NvbABpZC1tb2Qtb2NzcABpZC1tb2QtZHZjcwBpZC1tb2QtY21wMjAwMABiaW9tZXRyaWNJbmZvAEJpb21ldHJpYyBJbmZvAHFjU3RhdGVtZW50cwBhYy1hdWRpdEVudGl0eQBhYy10YXJnZXRpbmcAYWFDb250cm9scwBzYmdwLWlwQWRkckJsb2NrAHNiZ3AtYXV0b25vbW91c1N5c051bQBzYmdwLXJvdXRlcklkZW50aWZpZXIAdGV4dE5vdGljZQBpcHNlY0VuZFN5c3RlbQBJUFNlYyBFbmQgU3lzdGVtAGlwc2VjVHVubmVsAElQU2VjIFR1bm5lbABpcHNlY1VzZXIASVBTZWMgVXNlcgBEVkNTAGR2Y3MAaWQtaXQtY2FQcm90RW5jQ2VydABpZC1pdC1zaWduS2V5UGFpclR5cGVzAGlkLWl0LWVuY0tleVBhaXJUeXBlcwBpZC1pdC1wcmVmZXJyZWRTeW1tQWxnAGlkLWl0LWNhS2V5VXBkYXRlSW5mbwBpZC1pdC1jdXJyZW50Q1JMAGlkLWl0LXVuc3VwcG9ydGVkT0lEcwBpZC1pdC1zdWJzY3JpcHRpb25SZXF1ZXN0AGlkLWl0LXN1YnNjcmlwdGlvblJlc3BvbnNlAGlkLWl0LWtleVBhaXJQYXJhbVJlcQBpZC1pdC1rZXlQYWlyUGFyYW1SZXAAaWQtaXQtcmV2UGFzc3BocmFzZQBpZC1pdC1pbXBsaWNpdENvbmZpcm0AaWQtaXQtY29uZmlybVdhaXRUaW1lAGlkLWl0LW9yaWdQS0lNZXNzYWdlAGlkLXJlZ0N0cmwAaWQtcmVnSW5mbwBpZC1yZWdDdHJsLXJlZ1Rva2VuAGlkLXJlZ0N0cmwtYXV0aGVudGljYXRvcgBpZC1yZWdDdHJsLXBraVB1YmxpY2F0aW9uSW5mbwBpZC1yZWdDdHJsLXBraUFyY2hpdmVPcHRpb25zAGlkLXJlZ0N0cmwtb2xkQ2VydElEAGlkLXJlZ0N0cmwtcHJvdG9jb2xFbmNyS2V5AGlkLXJlZ0luZm8tdXRmOFBhaXJzAGlkLXJlZ0luZm8tY2VydFJlcQBpZC1hbGctZGVzNDAAaWQtYWxnLW5vU2lnbmF0dXJlAGlkLWFsZy1kaC1zaWctaG1hYy1zaGExAGlkLWFsZy1kaC1wb3AAaWQtY21jLXN0YXR1c0luZm8AaWQtY21jLWlkZW50aWZpY2F0aW9uAGlkLWNtYy1pZGVudGl0eVByb29mAGlkLWNtYy1kYXRhUmV0dXJuAGlkLWNtYy10cmFuc2FjdGlvbklkAGlkLWNtYy1zZW5kZXJOb25jZQBpZC1jbWMtcmVjaXBpZW50Tm9uY2UAaWQtY21jLWFkZEV4dGVuc2lvbnMAaWQtY21jLWVuY3J5cHRlZFBPUABpZC1jbWMtZGVjcnlwdGVkUE9QAGlkLWNtYy1scmFQT1BXaXRuZXNzAGlkLWNtYy1nZXRDZXJ0AGlkLWNtYy1nZXRDUkwAaWQtY21jLXJldm9rZVJlcXVlc3QAaWQtY21jLXJlZ0luZm8AaWQtY21jLXJlc3BvbnNlSW5mbwBpZC1jbWMtcXVlcnlQZW5kaW5nAGlkLWNtYy1wb3BMaW5rUmFuZG9tAGlkLWNtYy1wb3BMaW5rV2l0bmVzcwBpZC1jbWMtY29uZmlybUNlcnRBY2NlcHRhbmNlAGlkLW9uLXBlcnNvbmFsRGF0YQBpZC1wZGEtZGF0ZU9mQmlydGgAaWQtcGRhLXBsYWNlT2ZCaXJ0aABpZC1wZGEtZ2VuZGVyAGlkLXBkYS1jb3VudHJ5T2ZDaXRpemVuc2hpcABpZC1wZGEtY291bnRyeU9mUmVzaWRlbmNlAGlkLWFjYS1hdXRoZW50aWNhdGlvbkluZm8AaWQtYWNhLWFjY2Vzc0lkZW50aXR5AGlkLWFjYS1jaGFyZ2luZ0lkZW50aXR5AGlkLWFjYS1ncm91cABpZC1hY2Etcm9sZQBpZC1xY3MtcGtpeFFDU3ludGF4LXYxAGlkLWNjdC1jcnMAaWQtY2N0LVBLSURhdGEAaWQtY2N0LVBLSVJlc3BvbnNlAGFkX3RpbWVzdGFtcGluZwBBRCBUaW1lIFN0YW1waW5nAEFEX0RWQ1MAYWQgZHZjcwBiYXNpY09DU1BSZXNwb25zZQBCYXNpYyBPQ1NQIFJlc3BvbnNlAE5vbmNlAE9DU1AgTm9uY2UAQ3JsSUQAT0NTUCBDUkwgSUQAYWNjZXB0YWJsZVJlc3BvbnNlcwBBY2NlcHRhYmxlIE9DU1AgUmVzcG9uc2VzAG5vQ2hlY2sAT0NTUCBObyBDaGVjawBhcmNoaXZlQ3V0b2ZmAE9DU1AgQXJjaGl2ZSBDdXRvZmYAc2VydmljZUxvY2F0b3IAT0NTUCBTZXJ2aWNlIExvY2F0b3IAZXh0ZW5kZWRTdGF0dXMARXh0ZW5kZWQgT0NTUCBTdGF0dXMAdmFsaWQAdHJ1c3RSb290AFRydXN0IFJvb3QAYWxnb3JpdGhtAHJzYVNpZ25hdHVyZQBYNTAwYWxnb3JpdGhtcwBkaXJlY3Rvcnkgc2VydmljZXMgLSBhbGdvcml0aG1zAE9SRwBvcmcARE9EAGRvZABJQU5BAGlhbmEAZGlyZWN0b3J5AERpcmVjdG9yeQBtZ210AE1hbmFnZW1lbnQAZXhwZXJpbWVudGFsAEV4cGVyaW1lbnRhbABwcml2YXRlAFByaXZhdGUAc2VjdXJpdHkAU2VjdXJpdHkAc25tcHYyAFNOTVB2MgBNYWlsAGVudGVycHJpc2VzAEVudGVycHJpc2VzAGRjb2JqZWN0AGRjT2JqZWN0AERDAGRvbWFpbkNvbXBvbmVudABkb21haW4ARG9tYWluAE5VTEwAc2VsZWN0ZWQtYXR0cmlidXRlLXR5cGVzAFNlbGVjdGVkIEF0dHJpYnV0ZSBUeXBlcwBjbGVhcmFuY2UAUlNBLU1ENABtZDRXaXRoUlNBRW5jcnlwdGlvbgBhYy1wcm94eWluZwBzdWJqZWN0SW5mb0FjY2VzcwBTdWJqZWN0IEluZm9ybWF0aW9uIEFjY2VzcwBpZC1hY2EtZW5jQXR0cnMAcm9sZQBwb2xpY3lDb25zdHJhaW50cwBYNTA5djMgUG9saWN5IENvbnN0cmFpbnRzAHRhcmdldEluZm9ybWF0aW9uAFg1MDl2MyBBQyBUYXJnZXRpbmcAbm9SZXZBdmFpbABYNTA5djMgTm8gUmV2b2NhdGlvbiBBdmFpbGFibGUAYW5zaS1YOS02MgBBTlNJIFg5LjYyAHByaW1lLWZpZWxkAGNoYXJhY3RlcmlzdGljLXR3by1maWVsZABpZC1lY1B1YmxpY0tleQBwcmltZTE5MnYxAHByaW1lMTkydjIAcHJpbWUxOTJ2MwBwcmltZTIzOXYxAHByaW1lMjM5djIAcHJpbWUyMzl2MwBwcmltZTI1NnYxAGVjZHNhLXdpdGgtU0hBMQBDU1BOYW1lAE1pY3Jvc29mdCBDU1AgTmFtZQBBRVMtMTI4LUVDQgBhZXMtMTI4LWVjYgBBRVMtMTI4LUNCQwBhZXMtMTI4LWNiYwBBRVMtMTI4LU9GQgBhZXMtMTI4LW9mYgBBRVMtMTI4LUNGQgBhZXMtMTI4LWNmYgBBRVMtMTkyLUVDQgBhZXMtMTkyLWVjYgBBRVMtMTkyLUNCQwBhZXMtMTkyLWNiYwBBRVMtMTkyLU9GQgBhZXMtMTkyLW9mYgBBRVMtMTkyLUNGQgBhZXMtMTkyLWNmYgBBRVMtMjU2LUVDQgBhZXMtMjU2LWVjYgBBRVMtMjU2LUNCQwBhZXMtMjU2LWNiYwBBRVMtMjU2LU9GQgBhZXMtMjU2LW9mYgBBRVMtMjU2LUNGQgBhZXMtMjU2LWNmYgBob2xkSW5zdHJ1Y3Rpb25Db2RlAEhvbGQgSW5zdHJ1Y3Rpb24gQ29kZQBob2xkSW5zdHJ1Y3Rpb25Ob25lAEhvbGQgSW5zdHJ1Y3Rpb24gTm9uZQBob2xkSW5zdHJ1Y3Rpb25DYWxsSXNzdWVyAEhvbGQgSW5zdHJ1Y3Rpb24gQ2FsbCBJc3N1ZXIAaG9sZEluc3RydWN0aW9uUmVqZWN0AEhvbGQgSW5zdHJ1Y3Rpb24gUmVqZWN0AGRhdGEAcHNzAHVjbABwaWxvdABwaWxvdEF0dHJpYnV0ZVR5cGUAcGlsb3RBdHRyaWJ1dGVTeW50YXgAcGlsb3RPYmplY3RDbGFzcwBwaWxvdEdyb3VwcwBpQTVTdHJpbmdTeW50YXgAY2FzZUlnbm9yZUlBNVN0cmluZ1N5bnRheABwaWxvdE9iamVjdABwaWxvdFBlcnNvbgBhY2NvdW50AGRvY3VtZW50AHJvb20AZG9jdW1lbnRTZXJpZXMAckZDODIybG9jYWxQYXJ0AGROU0RvbWFpbgBkb21haW5SZWxhdGVkT2JqZWN0AGZyaWVuZGx5Q291bnRyeQBzaW1wbGVTZWN1cml0eU9iamVjdABwaWxvdE9yZ2FuaXphdGlvbgBwaWxvdERTQQBxdWFsaXR5TGFiZWxsZWREYXRhAFVJRAB1c2VySWQAdGV4dEVuY29kZWRPUkFkZHJlc3MAbWFpbAByZmM4MjJNYWlsYm94AGluZm8AZmF2b3VyaXRlRHJpbmsAcm9vbU51bWJlcgBwaG90bwB1c2VyQ2xhc3MAaG9zdABtYW5hZ2VyAGRvY3VtZW50SWRlbnRpZmllcgBkb2N1bWVudFRpdGxlAGRvY3VtZW50VmVyc2lvbgBkb2N1bWVudEF1dGhvcgBkb2N1bWVudExvY2F0aW9uAGhvbWVUZWxlcGhvbmVOdW1iZXIAc2VjcmV0YXJ5AG90aGVyTWFpbGJveABsYXN0TW9kaWZpZWRUaW1lAGxhc3RNb2RpZmllZEJ5AGFSZWNvcmQAcGlsb3RBdHRyaWJ1dGVUeXBlMjcAbVhSZWNvcmQAblNSZWNvcmQAc09BUmVjb3JkAGNOQU1FUmVjb3JkAGFzc29jaWF0ZWREb21haW4AYXNzb2NpYXRlZE5hbWUAaG9tZVBvc3RhbEFkZHJlc3MAcGVyc29uYWxUaXRsZQBtb2JpbGVUZWxlcGhvbmVOdW1iZXIAcGFnZXJUZWxlcGhvbmVOdW1iZXIAZnJpZW5kbHlDb3VudHJ5TmFtZQBvcmdhbml6YXRpb25hbFN0YXR1cwBqYW5ldE1haWxib3gAbWFpbFByZWZlcmVuY2VPcHRpb24AYnVpbGRpbmdOYW1lAGRTQVF1YWxpdHkAc2luZ2xlTGV2ZWxRdWFsaXR5AHN1YnRyZWVNaW5pbXVtUXVhbGl0eQBzdWJ0cmVlTWF4aW11bVF1YWxpdHkAcGVyc29uYWxTaWduYXR1cmUAZElUUmVkaXJlY3QAYXVkaW8AZG9jdW1lbnRQdWJsaXNoZXIAeDUwMFVuaXF1ZUlkZW50aWZpZXIAbWltZS1taHMATUlNRSBNSFMAbWltZS1taHMtaGVhZGluZ3MAbWltZS1taHMtYm9kaWVzAGlkLWhleC1wYXJ0aWFsLW1lc3NhZ2UAaWQtaGV4LW11bHRpcGFydC1tZXNzYWdlAGdlbmVyYXRpb25RdWFsaWZpZXIAcHNldWRvbnltAGlkLXNldABTZWN1cmUgRWxlY3Ryb25pYyBUcmFuc2FjdGlvbnMAc2V0LWN0eXBlAGNvbnRlbnQgdHlwZXMAc2V0LW1zZ0V4dABtZXNzYWdlIGV4dGVuc2lvbnMAc2V0LWF0dHIAc2V0LXBvbGljeQBzZXQtY2VydEV4dABjZXJ0aWZpY2F0ZSBleHRlbnNpb25zAHNldC1icmFuZABzZXRjdC1QQU5EYXRhAHNldGN0LVBBTlRva2VuAHNldGN0LVBBTk9ubHkAc2V0Y3QtT0lEYXRhAHNldGN0LVBJAHNldGN0LVBJRGF0YQBzZXRjdC1QSURhdGFVbnNpZ25lZABzZXRjdC1IT0RJbnB1dABzZXRjdC1BdXRoUmVzQmFnZ2FnZQBzZXRjdC1BdXRoUmV2UmVxQmFnZ2FnZQBzZXRjdC1BdXRoUmV2UmVzQmFnZ2FnZQBzZXRjdC1DYXBUb2tlblNlcQBzZXRjdC1QSW5pdFJlc0RhdGEAc2V0Y3QtUEktVEJTAHNldGN0LVBSZXNEYXRhAHNldGN0LUF1dGhSZXFUQlMAc2V0Y3QtQXV0aFJlc1RCUwBzZXRjdC1BdXRoUmVzVEJTWABzZXRjdC1BdXRoVG9rZW5UQlMAc2V0Y3QtQ2FwVG9rZW5EYXRhAHNldGN0LUNhcFRva2VuVEJTAHNldGN0LUFjcUNhcmRDb2RlTXNnAHNldGN0LUF1dGhSZXZSZXFUQlMAc2V0Y3QtQXV0aFJldlJlc0RhdGEAc2V0Y3QtQXV0aFJldlJlc1RCUwBzZXRjdC1DYXBSZXFUQlMAc2V0Y3QtQ2FwUmVxVEJTWABzZXRjdC1DYXBSZXNEYXRhAHNldGN0LUNhcFJldlJlcVRCUwBzZXRjdC1DYXBSZXZSZXFUQlNYAHNldGN0LUNhcFJldlJlc0RhdGEAc2V0Y3QtQ3JlZFJlcVRCUwBzZXRjdC1DcmVkUmVxVEJTWABzZXRjdC1DcmVkUmVzRGF0YQBzZXRjdC1DcmVkUmV2UmVxVEJTAHNldGN0LUNyZWRSZXZSZXFUQlNYAHNldGN0LUNyZWRSZXZSZXNEYXRhAHNldGN0LVBDZXJ0UmVxRGF0YQBzZXRjdC1QQ2VydFJlc1RCUwBzZXRjdC1CYXRjaEFkbWluUmVxRGF0YQBzZXRjdC1CYXRjaEFkbWluUmVzRGF0YQBzZXRjdC1DYXJkQ0luaXRSZXNUQlMAc2V0Y3QtTWVBcUNJbml0UmVzVEJTAHNldGN0LVJlZ0Zvcm1SZXNUQlMAc2V0Y3QtQ2VydFJlcURhdGEAc2V0Y3QtQ2VydFJlcVRCUwBzZXRjdC1DZXJ0UmVzRGF0YQBzZXRjdC1DZXJ0SW5xUmVxVEJTAHNldGN0LUVycm9yVEJTAHNldGN0LVBJRHVhbFNpZ25lZFRCRQBzZXRjdC1QSVVuc2lnbmVkVEJFAHNldGN0LUF1dGhSZXFUQkUAc2V0Y3QtQXV0aFJlc1RCRQBzZXRjdC1BdXRoUmVzVEJFWABzZXRjdC1BdXRoVG9rZW5UQkUAc2V0Y3QtQ2FwVG9rZW5UQkUAc2V0Y3QtQ2FwVG9rZW5UQkVYAHNldGN0LUFjcUNhcmRDb2RlTXNnVEJFAHNldGN0LUF1dGhSZXZSZXFUQkUAc2V0Y3QtQXV0aFJldlJlc1RCRQBzZXRjdC1BdXRoUmV2UmVzVEJFQgBzZXRjdC1DYXBSZXFUQkUAc2V0Y3QtQ2FwUmVxVEJFWABzZXRjdC1DYXBSZXNUQkUAc2V0Y3QtQ2FwUmV2UmVxVEJFAHNldGN0LUNhcFJldlJlcVRCRVgAc2V0Y3QtQ2FwUmV2UmVzVEJFAHNldGN0LUNyZWRSZXFUQkUAc2V0Y3QtQ3JlZFJlcVRCRVgAc2V0Y3QtQ3JlZFJlc1RCRQBzZXRjdC1DcmVkUmV2UmVxVEJFAHNldGN0LUNyZWRSZXZSZXFUQkVYAHNldGN0LUNyZWRSZXZSZXNUQkUAc2V0Y3QtQmF0Y2hBZG1pblJlcVRCRQBzZXRjdC1CYXRjaEFkbWluUmVzVEJFAHNldGN0LVJlZ0Zvcm1SZXFUQkUAc2V0Y3QtQ2VydFJlcVRCRQBzZXRjdC1DZXJ0UmVxVEJFWABzZXRjdC1DZXJ0UmVzVEJFAHNldGN0LUNSTE5vdGlmaWNhdGlvblRCUwBzZXRjdC1DUkxOb3RpZmljYXRpb25SZXNUQlMAc2V0Y3QtQkNJRGlzdHJpYnV0aW9uVEJTAHNldGV4dC1nZW5DcnlwdABnZW5lcmljIGNyeXB0b2dyYW0Ac2V0ZXh0LW1pQXV0aABtZXJjaGFudCBpbml0aWF0ZWQgYXV0aABzZXRleHQtcGluU2VjdXJlAHNldGV4dC1waW5BbnkAc2V0ZXh0LXRyYWNrMgBzZXRleHQtY3YAYWRkaXRpb25hbCB2ZXJpZmljYXRpb24Ac2V0LXBvbGljeS1yb290AHNldENleHQtaGFzaGVkUm9vdABzZXRDZXh0LWNlcnRUeXBlAHNldENleHQtbWVyY2hEYXRhAHNldENleHQtY0NlcnRSZXF1aXJlZABzZXRDZXh0LXR1bm5lbGluZwBzZXRDZXh0LXNldEV4dABzZXRDZXh0LXNldFF1YWxmAHNldENleHQtUEdXWWNhcGFiaWxpdGllcwBzZXRDZXh0LVRva2VuSWRlbnRpZmllcgBzZXRDZXh0LVRyYWNrMkRhdGEAc2V0Q2V4dC1Ub2tlblR5cGUAc2V0Q2V4dC1Jc3N1ZXJDYXBhYmlsaXRpZXMAc2V0QXR0ci1DZXJ0AHNldEF0dHItUEdXWWNhcABwYXltZW50IGdhdGV3YXkgY2FwYWJpbGl0aWVzAHNldEF0dHItVG9rZW5UeXBlAHNldEF0dHItSXNzQ2FwAGlzc3VlciBjYXBhYmlsaXRpZXMAc2V0LXJvb3RLZXlUaHVtYgBzZXQtYWRkUG9saWN5AHNldEF0dHItVG9rZW4tRU1WAHNldEF0dHItVG9rZW4tQjBQcmltZQBzZXRBdHRyLUlzc0NhcC1DVk0Ac2V0QXR0ci1Jc3NDYXAtVDIAc2V0QXR0ci1Jc3NDYXAtU2lnAHNldEF0dHItR2VuQ3J5cHRncm0AZ2VuZXJhdGUgY3J5cHRvZ3JhbQBzZXRBdHRyLVQyRW5jAGVuY3J5cHRlZCB0cmFjayAyAHNldEF0dHItVDJjbGVhcnR4dABjbGVhcnRleHQgdHJhY2sgMgBzZXRBdHRyLVRva0lDQ3NpZwBJQ0Mgb3IgdG9rZW4gc2lnbmF0dXJlAHNldEF0dHItU2VjRGV2U2lnAHNlY3VyZSBkZXZpY2Ugc2lnbmF0dXJlAHNldC1icmFuZC1JQVRBLUFUQQBzZXQtYnJhbmQtRGluZXJzAHNldC1icmFuZC1BbWVyaWNhbkV4cHJlc3MAc2V0LWJyYW5kLUpDQgBzZXQtYnJhbmQtVmlzYQBzZXQtYnJhbmQtTWFzdGVyQ2FyZABzZXQtYnJhbmQtTm92dXMAREVTLUNETUYAZGVzLWNkbWYAcnNhT0FFUEVuY3J5cHRpb25TRVQASVRVLVQAaXR1LXQASk9JTlQtSVNPLUlUVS1UAGpvaW50LWlzby1pdHUtdABpbnRlcm5hdGlvbmFsLW9yZ2FuaXphdGlvbnMASW50ZXJuYXRpb25hbCBPcmdhbml6YXRpb25zAG1zU21hcnRjYXJkTG9naW4ATWljcm9zb2Z0IFNtYXJ0Y2FyZGxvZ2luAG1zVVBOAE1pY3Jvc29mdCBVbml2ZXJzYWwgUHJpbmNpcGFsIE5hbWUAQUVTLTEyOC1DRkIxAGFlcy0xMjgtY2ZiMQBBRVMtMTkyLUNGQjEAYWVzLTE5Mi1jZmIxAEFFUy0yNTYtQ0ZCMQBhZXMtMjU2LWNmYjEAQUVTLTEyOC1DRkI4AGFlcy0xMjgtY2ZiOABBRVMtMTkyLUNGQjgAYWVzLTE5Mi1jZmI4AEFFUy0yNTYtQ0ZCOABhZXMtMjU2LWNmYjgAREVTLUNGQjEAZGVzLWNmYjEAREVTLUNGQjgAZGVzLWNmYjgAREVTLUVERTMtQ0ZCMQBkZXMtZWRlMy1jZmIxAERFUy1FREUzLUNGQjgAZGVzLWVkZTMtY2ZiOABzdHJlZXQAc3RyZWV0QWRkcmVzcwBwb3N0YWxDb2RlAGlkLXBwbABwcm94eUNlcnRJbmZvAFByb3h5IENlcnRpZmljYXRlIEluZm9ybWF0aW9uAGlkLXBwbC1hbnlMYW5ndWFnZQBBbnkgbGFuZ3VhZ2UAaWQtcHBsLWluaGVyaXRBbGwASW5oZXJpdCBhbGwAbmFtZUNvbnN0cmFpbnRzAFg1MDl2MyBOYW1lIENvbnN0cmFpbnRzAGlkLXBwbC1pbmRlcGVuZGVudABJbmRlcGVuZGVudABSU0EtU0hBMjU2AHNoYTI1NldpdGhSU0FFbmNyeXB0aW9uAFJTQS1TSEEzODQAc2hhMzg0V2l0aFJTQUVuY3J5cHRpb24AUlNBLVNIQTUxMgBzaGE1MTJXaXRoUlNBRW5jcnlwdGlvbgBSU0EtU0hBMjI0AHNoYTIyNFdpdGhSU0FFbmNyeXB0aW9uAFNIQTI1NgBzaGEyNTYAU0hBMzg0AHNoYTM4NABTSEE1MTIAc2hhNTEyAFNIQTIyNABzaGEyMjQAaWRlbnRpZmllZC1vcmdhbml6YXRpb24AY2VydGljb20tYXJjAHdhcAB3YXAtd3NnAGlkLWNoYXJhY3RlcmlzdGljLXR3by1iYXNpcwBvbkJhc2lzAHRwQmFzaXMAcHBCYXNpcwBjMnBuYjE2M3YxAGMycG5iMTYzdjIAYzJwbmIxNjN2MwBjMnBuYjE3NnYxAGMydG5iMTkxdjEAYzJ0bmIxOTF2MgBjMnRuYjE5MXYzAGMyb25iMTkxdjQAYzJvbmIxOTF2NQBjMnBuYjIwOHcxAGMydG5iMjM5djEAYzJ0bmIyMzl2MgBjMnRuYjIzOXYzAGMyb25iMjM5djQAYzJvbmIyMzl2NQBjMnBuYjI3MncxAGMycG5iMzA0dzEAYzJ0bmIzNTl2MQBjMnBuYjM2OHcxAGMydG5iNDMxcjEAc2VjcDExMnIxAHNlY3AxMTJyMgBzZWNwMTI4cjEAc2VjcDEyOHIyAHNlY3AxNjBrMQBzZWNwMTYwcjEAc2VjcDE2MHIyAHNlY3AxOTJrMQBzZWNwMjI0azEAc2VjcDIyNHIxAHNlY3AyNTZrMQBzZWNwMzg0cjEAc2VjcDUyMXIxAHNlY3QxMTNyMQBzZWN0MTEzcjIAc2VjdDEzMXIxAHNlY3QxMzFyMgBzZWN0MTYzazEAc2VjdDE2M3IxAHNlY3QxNjNyMgBzZWN0MTkzcjEAc2VjdDE5M3IyAHNlY3QyMzNrMQBzZWN0MjMzcjEAc2VjdDIzOWsxAHNlY3QyODNrMQBzZWN0MjgzcjEAc2VjdDQwOWsxAHNlY3Q0MDlyMQBzZWN0NTcxazEAc2VjdDU3MXIxAHdhcC13c2ctaWRtLWVjaWQtd3RsczEAd2FwLXdzZy1pZG0tZWNpZC13dGxzMwB3YXAtd3NnLWlkbS1lY2lkLXd0bHM0AHdhcC13c2ctaWRtLWVjaWQtd3RsczUAd2FwLXdzZy1pZG0tZWNpZC13dGxzNgB3YXAtd3NnLWlkbS1lY2lkLXd0bHM3AHdhcC13c2ctaWRtLWVjaWQtd3RsczgAd2FwLXdzZy1pZG0tZWNpZC13dGxzOQB3YXAtd3NnLWlkbS1lY2lkLXd0bHMxMAB3YXAtd3NnLWlkbS1lY2lkLXd0bHMxMQB3YXAtd3NnLWlkbS1lY2lkLXd0bHMxMgBhbnlQb2xpY3kAWDUwOXYzIEFueSBQb2xpY3kAcG9saWN5TWFwcGluZ3MAWDUwOXYzIFBvbGljeSBNYXBwaW5ncwBpbmhpYml0QW55UG9saWN5AFg1MDl2MyBJbmhpYml0IEFueSBQb2xpY3kAT2FrbGV5LUVDMk4tMwBpcHNlYzMAT2FrbGV5LUVDMk4tNABpcHNlYzQAQ0FNRUxMSUEtMTI4LUNCQwBjYW1lbGxpYS0xMjgtY2JjAENBTUVMTElBLTE5Mi1DQkMAY2FtZWxsaWEtMTkyLWNiYwBDQU1FTExJQS0yNTYtQ0JDAGNhbWVsbGlhLTI1Ni1jYmMAQ0FNRUxMSUEtMTI4LUVDQgBjYW1lbGxpYS0xMjgtZWNiAENBTUVMTElBLTE5Mi1FQ0IAY2FtZWxsaWEtMTkyLWVjYgBDQU1FTExJQS0yNTYtRUNCAGNhbWVsbGlhLTI1Ni1lY2IAQ0FNRUxMSUEtMTI4LUNGQgBjYW1lbGxpYS0xMjgtY2ZiAENBTUVMTElBLTE5Mi1DRkIAY2FtZWxsaWEtMTkyLWNmYgBDQU1FTExJQS0yNTYtQ0ZCAGNhbWVsbGlhLTI1Ni1jZmIAQ0FNRUxMSUEtMTI4LUNGQjEAY2FtZWxsaWEtMTI4LWNmYjEAQ0FNRUxMSUEtMTkyLUNGQjEAY2FtZWxsaWEtMTkyLWNmYjEAQ0FNRUxMSUEtMjU2LUNGQjEAY2FtZWxsaWEtMjU2LWNmYjEAQ0FNRUxMSUEtMTI4LUNGQjgAY2FtZWxsaWEtMTI4LWNmYjgAQ0FNRUxMSUEtMTkyLUNGQjgAY2FtZWxsaWEtMTkyLWNmYjgAQ0FNRUxMSUEtMjU2LUNGQjgAY2FtZWxsaWEtMjU2LWNmYjgAQ0FNRUxMSUEtMTI4LU9GQgBjYW1lbGxpYS0xMjgtb2ZiAENBTUVMTElBLTE5Mi1PRkIAY2FtZWxsaWEtMTkyLW9mYgBDQU1FTExJQS0yNTYtT0ZCAGNhbWVsbGlhLTI1Ni1vZmIAc3ViamVjdERpcmVjdG9yeUF0dHJpYnV0ZXMAWDUwOXYzIFN1YmplY3QgRGlyZWN0b3J5IEF0dHJpYnV0ZXMAaXNzdWluZ0Rpc3RyaWJ1dGlvblBvaW50AFg1MDl2MyBJc3N1aW5nIERpc3RyaWJ1dGlvbiBQb2ludABjZXJ0aWZpY2F0ZUlzc3VlcgBYNTA5djMgQ2VydGlmaWNhdGUgSXNzdWVyAEtJU0EAa2lzYQBTRUVELUVDQgBzZWVkLWVjYgBTRUVELUNCQwBzZWVkLWNiYwBTRUVELU9GQgBzZWVkLW9mYgBTRUVELUNGQgBzZWVkLWNmYgBITUFDLU1ENQBobWFjLW1kNQBITUFDLVNIQTEAaG1hYy1zaGExAGlkLVBhc3N3b3JkQmFzZWRNQUMAcGFzc3dvcmQgYmFzZWQgTUFDAGlkLURIQmFzZWRNYWMARGlmZmllLUhlbGxtYW4gYmFzZWQgTUFDAGlkLWl0LXN1cHBMYW5nVGFncwBjYVJlcG9zaXRvcnkAQ0EgUmVwb3NpdG9yeQBpZC1zbWltZS1jdC1jb21wcmVzc2VkRGF0YQBpZC1jdC1hc2NpaVRleHRXaXRoQ1JMRgBpZC1hZXMxMjgtd3JhcABpZC1hZXMxOTItd3JhcABpZC1hZXMyNTYtd3JhcABlY2RzYS13aXRoLVJlY29tbWVuZGVkAGVjZHNhLXdpdGgtU3BlY2lmaWVkAGVjZHNhLXdpdGgtU0hBMjI0AGVjZHNhLXdpdGgtU0hBMjU2AGVjZHNhLXdpdGgtU0hBMzg0AGVjZHNhLXdpdGgtU0hBNTEyAGhtYWNXaXRoTUQ1AGhtYWNXaXRoU0hBMjI0AGhtYWNXaXRoU0hBMjU2AGhtYWNXaXRoU0hBMzg0AGhtYWNXaXRoU0hBNTEyAGRzYV93aXRoX1NIQTIyNABkc2Ffd2l0aF9TSEEyNTYAd2hpcmxwb29sAGNyeXB0b3BybwBjcnlwdG9jb20AaWQtR29zdFIzNDExLTk0LXdpdGgtR29zdFIzNDEwLTIwMDEAR09TVCBSIDM0LjExLTk0IHdpdGggR09TVCBSIDM0LjEwLTIwMDEAaWQtR29zdFIzNDExLTk0LXdpdGgtR29zdFIzNDEwLTk0AEdPU1QgUiAzNC4xMS05NCB3aXRoIEdPU1QgUiAzNC4xMC05NABtZF9nb3N0OTQAR09TVCBSIDM0LjExLTk0AGlkLUhNQUNHb3N0UjM0MTEtOTQASE1BQyBHT1NUIDM0LjExLTk0AGdvc3QyMDAxAEdPU1QgUiAzNC4xMC0yMDAxAGdvc3Q5NABHT1NUIFIgMzQuMTAtOTQAZ29zdDg5AEdPU1QgMjgxNDctODkAZ29zdDg5LWNudABnb3N0LW1hYwBHT1NUIDI4MTQ3LTg5IE1BQwBwcmYtZ29zdHIzNDExLTk0AEdPU1QgUiAzNC4xMS05NCBQUkYAaWQtR29zdFIzNDEwLTIwMDFESABHT1NUIFIgMzQuMTAtMjAwMSBESABpZC1Hb3N0UjM0MTAtOTRESABHT1NUIFIgMzQuMTAtOTQgREgAaWQtR29zdDI4MTQ3LTg5LUNyeXB0b1Byby1LZXlNZXNoaW5nAGlkLUdvc3QyODE0Ny04OS1Ob25lLUtleU1lc2hpbmcAaWQtR29zdFIzNDExLTk0LVRlc3RQYXJhbVNldABpZC1Hb3N0UjM0MTEtOTQtQ3J5cHRvUHJvUGFyYW1TZXQAaWQtR29zdDI4MTQ3LTg5LVRlc3RQYXJhbVNldABpZC1Hb3N0MjgxNDctODktQ3J5cHRvUHJvLUEtUGFyYW1TZXQAaWQtR29zdDI4MTQ3LTg5LUNyeXB0b1Byby1CLVBhcmFtU2V0AGlkLUdvc3QyODE0Ny04OS1DcnlwdG9Qcm8tQy1QYXJhbVNldABpZC1Hb3N0MjgxNDctODktQ3J5cHRvUHJvLUQtUGFyYW1TZXQAaWQtR29zdDI4MTQ3LTg5LUNyeXB0b1Byby1Pc2Nhci0xLTEtUGFyYW1TZXQAaWQtR29zdDI4MTQ3LTg5LUNyeXB0b1Byby1Pc2Nhci0xLTAtUGFyYW1TZXQAaWQtR29zdDI4MTQ3LTg5LUNyeXB0b1Byby1SSUMtMS1QYXJhbVNldABpZC1Hb3N0UjM0MTAtOTQtVGVzdFBhcmFtU2V0AGlkLUdvc3RSMzQxMC05NC1DcnlwdG9Qcm8tQS1QYXJhbVNldABpZC1Hb3N0UjM0MTAtOTQtQ3J5cHRvUHJvLUItUGFyYW1TZXQAaWQtR29zdFIzNDEwLTk0LUNyeXB0b1Byby1DLVBhcmFtU2V0AGlkLUdvc3RSMzQxMC05NC1DcnlwdG9Qcm8tRC1QYXJhbVNldABpZC1Hb3N0UjM0MTAtOTQtQ3J5cHRvUHJvLVhjaEEtUGFyYW1TZXQAaWQtR29zdFIzNDEwLTk0LUNyeXB0b1Byby1YY2hCLVBhcmFtU2V0AGlkLUdvc3RSMzQxMC05NC1DcnlwdG9Qcm8tWGNoQy1QYXJhbVNldABpZC1Hb3N0UjM0MTAtMjAwMS1UZXN0UGFyYW1TZXQAaWQtR29zdFIzNDEwLTIwMDEtQ3J5cHRvUHJvLUEtUGFyYW1TZXQAaWQtR29zdFIzNDEwLTIwMDEtQ3J5cHRvUHJvLUItUGFyYW1TZXQAaWQtR29zdFIzNDEwLTIwMDEtQ3J5cHRvUHJvLUMtUGFyYW1TZXQAaWQtR29zdFIzNDEwLTIwMDEtQ3J5cHRvUHJvLVhjaEEtUGFyYW1TZXQAaWQtR29zdFIzNDEwLTIwMDEtQ3J5cHRvUHJvLVhjaEItUGFyYW1TZXQAaWQtR29zdFIzNDEwLTk0LWEAaWQtR29zdFIzNDEwLTk0LWFCaXMAaWQtR29zdFIzNDEwLTk0LWIAaWQtR29zdFIzNDEwLTk0LWJCaXMAaWQtR29zdDI4MTQ3LTg5LWNjAEdPU1QgMjgxNDctODkgQ3J5cHRvY29tIFBhcmFtU2V0AGdvc3Q5NGNjAEdPU1QgMzQuMTAtOTQgQ3J5cHRvY29tAGdvc3QyMDAxY2MAR09TVCAzNC4xMC0yMDAxIENyeXB0b2NvbQBpZC1Hb3N0UjM0MTEtOTQtd2l0aC1Hb3N0UjM0MTAtOTQtY2MAR09TVCBSIDM0LjExLTk0IHdpdGggR09TVCBSIDM0LjEwLTk0IENyeXB0b2NvbQBpZC1Hb3N0UjM0MTEtOTQtd2l0aC1Hb3N0UjM0MTAtMjAwMS1jYwBHT1NUIFIgMzQuMTEtOTQgd2l0aCBHT1NUIFIgMzQuMTAtMjAwMSBDcnlwdG9jb20AaWQtR29zdFIzNDEwLTIwMDEtUGFyYW1TZXQtY2MAR09TVCBSIDM0MTAtMjAwMSBQYXJhbWV0ZXIgU2V0IENyeXB0b2NvbQBITUFDAGhtYWMATG9jYWxLZXlTZXQATWljcm9zb2Z0IExvY2FsIEtleSBzZXQAZnJlc2hlc3RDUkwAWDUwOXYzIEZyZXNoZXN0IENSTABpZC1vbi1wZXJtYW5lbnRJZGVudGlmaWVyAFBlcm1hbmVudCBJZGVudGlmaWVyAHNlYXJjaEd1aWRlAGJ1c2luZXNzQ2F0ZWdvcnkAcG9zdGFsQWRkcmVzcwBwb3N0T2ZmaWNlQm94AHBoeXNpY2FsRGVsaXZlcnlPZmZpY2VOYW1lAHRlbGVwaG9uZU51bWJlcgB0ZWxleE51bWJlcgB0ZWxldGV4VGVybWluYWxJZGVudGlmaWVyAGZhY3NpbWlsZVRlbGVwaG9uZU51bWJlcgB4MTIxQWRkcmVzcwBpbnRlcm5hdGlvbmFsaVNETk51bWJlcgByZWdpc3RlcmVkQWRkcmVzcwBkZXN0aW5hdGlvbkluZGljYXRvcgBwcmVmZXJyZWREZWxpdmVyeU1ldGhvZABwcmVzZW50YXRpb25BZGRyZXNzAHN1cHBvcnRlZEFwcGxpY2F0aW9uQ29udGV4dABtZW1iZXIAb3duZXIAcm9sZU9jY3VwYW50AHNlZUFsc28AdXNlclBhc3N3b3JkAHVzZXJDZXJ0aWZpY2F0ZQBjQUNlcnRpZmljYXRlAGF1dGhvcml0eVJldm9jYXRpb25MaXN0AGNlcnRpZmljYXRlUmV2b2NhdGlvbkxpc3QAY3Jvc3NDZXJ0aWZpY2F0ZVBhaXIAZW5oYW5jZWRTZWFyY2hHdWlkZQBwcm90b2NvbEluZm9ybWF0aW9uAGRpc3Rpbmd1aXNoZWROYW1lAHVuaXF1ZU1lbWJlcgBob3VzZUlkZW50aWZpZXIAc3VwcG9ydGVkQWxnb3JpdGhtcwBkZWx0YVJldm9jYXRpb25MaXN0AGRtZE5hbWUAaWQtYWxnLVBXUkktS0VLAENNQUMAY21hYwBpZC1hZXMxMjgtR0NNAGFlcy0xMjgtZ2NtAGlkLWFlczEyOC1DQ00AYWVzLTEyOC1jY20AaWQtYWVzMTI4LXdyYXAtcGFkAGlkLWFlczE5Mi1HQ00AYWVzLTE5Mi1nY20AaWQtYWVzMTkyLUNDTQBhZXMtMTkyLWNjbQBpZC1hZXMxOTItd3JhcC1wYWQAaWQtYWVzMjU2LUdDTQBhZXMtMjU2LWdjbQBpZC1hZXMyNTYtQ0NNAGFlcy0yNTYtY2NtAGlkLWFlczI1Ni13cmFwLXBhZABBRVMtMTI4LUNUUgBhZXMtMTI4LWN0cgBBRVMtMTkyLUNUUgBhZXMtMTkyLWN0cgBBRVMtMjU2LUNUUgBhZXMtMjU2LWN0cgBpZC1jYW1lbGxpYTEyOC13cmFwAGlkLWNhbWVsbGlhMTkyLXdyYXAAaWQtY2FtZWxsaWEyNTYtd3JhcABhbnlFeHRlbmRlZEtleVVzYWdlAEFueSBFeHRlbmRlZCBLZXkgVXNhZ2UATUdGMQBtZ2YxAFJTQVNTQS1QU1MAcnNhc3NhUHNzAEFFUy0xMjgtWFRTAGFlcy0xMjgteHRzAEFFUy0yNTYtWFRTAGFlcy0yNTYteHRzAFJDNC1ITUFDLU1ENQByYzQtaG1hYy1tZDUAQUVTLTEyOC1DQkMtSE1BQy1TSEExAGFlcy0xMjgtY2JjLWhtYWMtc2hhMQBBRVMtMTkyLUNCQy1ITUFDLVNIQTEAYWVzLTE5Mi1jYmMtaG1hYy1zaGExAEFFUy0yNTYtQ0JDLUhNQUMtU0hBMQBhZXMtMjU2LWNiYy1obWFjLXNoYTEAUlNBRVMtT0FFUAByc2Flc09hZXAAZGhwdWJsaWNudW1iZXIAWDkuNDIgREgAYnJhaW5wb29sUDE2MHIxAGJyYWlucG9vbFAxNjB0MQBicmFpbnBvb2xQMTkycjEAYnJhaW5wb29sUDE5MnQxAGJyYWlucG9vbFAyMjRyMQBicmFpbnBvb2xQMjI0dDEAYnJhaW5wb29sUDI1NnIxAGJyYWlucG9vbFAyNTZ0MQBicmFpbnBvb2xQMzIwcjEAYnJhaW5wb29sUDMyMHQxAGJyYWlucG9vbFAzODRyMQBicmFpbnBvb2xQMzg0dDEAYnJhaW5wb29sUDUxMnIxAGJyYWlucG9vbFA1MTJ0MQBQU1BFQ0lGSUVEAHBTcGVjaWZpZWQAZGhTaW5nbGVQYXNzLXN0ZERILXNoYTFrZGYtc2NoZW1lAGRoU2luZ2xlUGFzcy1zdGRESC1zaGEyMjRrZGYtc2NoZW1lAGRoU2luZ2xlUGFzcy1zdGRESC1zaGEyNTZrZGYtc2NoZW1lAGRoU2luZ2xlUGFzcy1zdGRESC1zaGEzODRrZGYtc2NoZW1lAGRoU2luZ2xlUGFzcy1zdGRESC1zaGE1MTJrZGYtc2NoZW1lAGRoU2luZ2xlUGFzcy1jb2ZhY3RvckRILXNoYTFrZGYtc2NoZW1lAGRoU2luZ2xlUGFzcy1jb2ZhY3RvckRILXNoYTIyNGtkZi1zY2hlbWUAZGhTaW5nbGVQYXNzLWNvZmFjdG9yREgtc2hhMjU2a2RmLXNjaGVtZQBkaFNpbmdsZVBhc3MtY29mYWN0b3JESC1zaGEzODRrZGYtc2NoZW1lAGRoU2luZ2xlUGFzcy1jb2ZhY3RvckRILXNoYTUxMmtkZi1zY2hlbWUAZGgtc3RkLWtkZgBkaC1jb2ZhY3Rvci1rZGYAQUVTLTEyOC1DQkMtSE1BQy1TSEEyNTYAYWVzLTEyOC1jYmMtaG1hYy1zaGEyNTYAQUVTLTE5Mi1DQkMtSE1BQy1TSEEyNTYAYWVzLTE5Mi1jYmMtaG1hYy1zaGEyNTYAQUVTLTI1Ni1DQkMtSE1BQy1TSEEyNTYAYWVzLTI1Ni1jYmMtaG1hYy1zaGEyNTYAY3RfcHJlY2VydF9zY3RzAENUIFByZWNlcnRpZmljYXRlIFNDVHMAY3RfcHJlY2VydF9wb2lzb24AQ1QgUHJlY2VydGlmaWNhdGUgUG9pc29uAGN0X3ByZWNlcnRfc2lnbmVyAENUIFByZWNlcnRpZmljYXRlIFNpZ25lcgBjdF9jZXJ0X3NjdHMAQ1QgQ2VydGlmaWNhdGUgU0NUcwBqdXJpc2RpY3Rpb25MAGp1cmlzZGljdGlvbkxvY2FsaXR5TmFtZQBqdXJpc2RpY3Rpb25TVABqdXJpc2RpY3Rpb25TdGF0ZU9yUHJvdmluY2VOYW1lAGp1cmlzZGljdGlvbkMAanVyaXNkaWN0aW9uQ291bnRyeU5hbWUAQUVTLTEyOC1PQ0IAYWVzLTEyOC1vY2IAQUVTLTE5Mi1PQ0IAYWVzLTE5Mi1vY2IAQUVTLTI1Ni1PQ0IAYWVzLTI1Ni1vY2IAQ0FNRUxMSUEtMTI4LUdDTQBjYW1lbGxpYS0xMjgtZ2NtAENBTUVMTElBLTEyOC1DQ00AY2FtZWxsaWEtMTI4LWNjbQBDQU1FTExJQS0xMjgtQ1RSAGNhbWVsbGlhLTEyOC1jdHIAQ0FNRUxMSUEtMTI4LUNNQUMAY2FtZWxsaWEtMTI4LWNtYWMAQ0FNRUxMSUEtMTkyLUdDTQBjYW1lbGxpYS0xOTItZ2NtAENBTUVMTElBLTE5Mi1DQ00AY2FtZWxsaWEtMTkyLWNjbQBDQU1FTExJQS0xOTItQ1RSAGNhbWVsbGlhLTE5Mi1jdHIAQ0FNRUxMSUEtMTkyLUNNQUMAY2FtZWxsaWEtMTkyLWNtYWMAQ0FNRUxMSUEtMjU2LUdDTQBjYW1lbGxpYS0yNTYtZ2NtAENBTUVMTElBLTI1Ni1DQ00AY2FtZWxsaWEtMjU2LWNjbQBDQU1FTExJQS0yNTYtQ1RSAGNhbWVsbGlhLTI1Ni1jdHIAQ0FNRUxMSUEtMjU2LUNNQUMAY2FtZWxsaWEtMjU2LWNtYWMAaWQtc2NyeXB0AGlkLXRjMjYAZ29zdDg5LWNudC0xMgBnb3N0LW1hYy0xMgBpZC10YzI2LWFsZ29yaXRobXMAaWQtdGMyNi1zaWduAGdvc3QyMDEyXzI1NgBHT1NUIFIgMzQuMTAtMjAxMiB3aXRoIDI1NiBiaXQgbW9kdWx1cwBnb3N0MjAxMl81MTIAR09TVCBSIDM0LjEwLTIwMTIgd2l0aCA1MTIgYml0IG1vZHVsdXMAaWQtdGMyNi1kaWdlc3QAbWRfZ29zdDEyXzI1NgBHT1NUIFIgMzQuMTEtMjAxMiB3aXRoIDI1NiBiaXQgaGFzaABtZF9nb3N0MTJfNTEyAEdPU1QgUiAzNC4xMS0yMDEyIHdpdGggNTEyIGJpdCBoYXNoAGlkLXRjMjYtc2lnbndpdGhkaWdlc3QAaWQtdGMyNi1zaWdud2l0aGRpZ2VzdC1nb3N0MzQxMC0yMDEyLTI1NgBHT1NUIFIgMzQuMTAtMjAxMiB3aXRoIEdPU1QgUiAzNC4xMS0yMDEyICgyNTYgYml0KQBpZC10YzI2LXNpZ253aXRoZGlnZXN0LWdvc3QzNDEwLTIwMTItNTEyAEdPU1QgUiAzNC4xMC0yMDEyIHdpdGggR09TVCBSIDM0LjExLTIwMTIgKDUxMiBiaXQpAGlkLXRjMjYtbWFjAGlkLXRjMjYtaG1hYy1nb3N0LTM0MTEtMjAxMi0yNTYASE1BQyBHT1NUIDM0LjExLTIwMTIgMjU2IGJpdABpZC10YzI2LWhtYWMtZ29zdC0zNDExLTIwMTItNTEyAEhNQUMgR09TVCAzNC4xMS0yMDEyIDUxMiBiaXQAaWQtdGMyNi1jaXBoZXIAaWQtdGMyNi1hZ3JlZW1lbnQAaWQtdGMyNi1hZ3JlZW1lbnQtZ29zdC0zNDEwLTIwMTItMjU2AGlkLXRjMjYtYWdyZWVtZW50LWdvc3QtMzQxMC0yMDEyLTUxMgBpZC10YzI2LWNvbnN0YW50cwBpZC10YzI2LXNpZ24tY29uc3RhbnRzAGlkLXRjMjYtZ29zdC0zNDEwLTIwMTItNTEyLWNvbnN0YW50cwBpZC10YzI2LWdvc3QtMzQxMC0yMDEyLTUxMi1wYXJhbVNldFRlc3QAR09TVCBSIDM0LjEwLTIwMTIgKDUxMiBiaXQpIHRlc3RpbmcgcGFyYW1ldGVyIHNldABpZC10YzI2LWdvc3QtMzQxMC0yMDEyLTUxMi1wYXJhbVNldEEAR09TVCBSIDM0LjEwLTIwMTIgKDUxMiBiaXQpIFBhcmFtU2V0IEEAaWQtdGMyNi1nb3N0LTM0MTAtMjAxMi01MTItcGFyYW1TZXRCAEdPU1QgUiAzNC4xMC0yMDEyICg1MTIgYml0KSBQYXJhbVNldCBCAGlkLXRjMjYtZGlnZXN0LWNvbnN0YW50cwBpZC10YzI2LWNpcGhlci1jb25zdGFudHMAaWQtdGMyNi1nb3N0LTI4MTQ3LWNvbnN0YW50cwBpZC10YzI2LWdvc3QtMjgxNDctcGFyYW0tWgBHT1NUIDI4MTQ3LTg5IFRDMjYgcGFyYW1ldGVyIHNldABJTk4AT0dSTgBTTklMUwBzdWJqZWN0U2lnblRvb2wAU2lnbmluZyBUb29sIG9mIFN1YmplY3QAaXNzdWVyU2lnblRvb2wAU2lnbmluZyBUb29sIG9mIElzc3VlcgBnb3N0ODktY2JjAGdvc3Q4OS1lY2IAZ29zdDg5LWN0cgBncmFzc2hvcHBlci1lY2IAZ3Jhc3Nob3BwZXItY3RyAGdyYXNzaG9wcGVyLW9mYgBncmFzc2hvcHBlci1jYmMAZ3Jhc3Nob3BwZXItY2ZiAGdyYXNzaG9wcGVyLW1hYwBDaGFDaGEyMC1Qb2x5MTMwNQBjaGFjaGEyMC1wb2x5MTMwNQBDaGFDaGEyMABjaGFjaGEyMAB0bHNmZWF0dXJlAFRMUyBGZWF0dXJlAFRMUzEtUFJGAHRsczEtcHJmAGlwc2VjSUtFAGlwc2VjIEludGVybmV0IEtleSBFeGNoYW5nZQBjYXB3YXBBQwBDdHJsL3Byb3Zpc2lvbiBXQVAgQWNjZXNzAGNhcHdhcFdUUABDdHJsL1Byb3Zpc2lvbiBXQVAgVGVybWluYXRpb24Ac2VjdXJlU2hlbGxDbGllbnQAU1NIIENsaWVudABzZWN1cmVTaGVsbFNlcnZlcgBTU0ggU2VydmVyAHNlbmRSb3V0ZXIAU2VuZCBSb3V0ZXIAc2VuZFByb3hpZWRSb3V0ZXIAU2VuZCBQcm94aWVkIFJvdXRlcgBzZW5kT3duZXIAU2VuZCBPd25lcgBzZW5kUHJveGllZE93bmVyAFNlbmQgUHJveGllZCBPd25lcgBpZC1wa2luaXQAcGtJbml0Q2xpZW50QXV0aABQS0lOSVQgQ2xpZW50IEF1dGgAcGtJbml0S0RDAFNpZ25pbmcgS0RDIFJlc3BvbnNlAFgyNTUxOQBYNDQ4AEhLREYAaGtkZgBLeFJTQQBreC1yc2EAS3hFQ0RIRQBreC1lY2RoZQBLeERIRQBreC1kaGUAS3hFQ0RIRS1QU0sAa3gtZWNkaGUtcHNrAEt4REhFLVBTSwBreC1kaGUtcHNrAEt4UlNBX1BTSwBreC1yc2EtcHNrAEt4UFNLAGt4LXBzawBLeFNSUABreC1zcnAAS3hHT1NUAGt4LWdvc3QAQXV0aFJTQQBhdXRoLXJzYQBBdXRoRUNEU0EAYXV0aC1lY2RzYQBBdXRoUFNLAGF1dGgtcHNrAEF1dGhEU1MAYXV0aC1kc3MAQXV0aEdPU1QwMQBhdXRoLWdvc3QwMQBBdXRoR09TVDEyAGF1dGgtZ29zdDEyAEF1dGhTUlAAYXV0aC1zcnAAQXV0aE5VTEwAYXV0aC1udWxsAEJMQUtFMmI1MTIAYmxha2UyYjUxMgBCTEFLRTJzMjU2AGJsYWtlMnMyNTYAaWQtc21pbWUtY3QtY29udGVudENvbGxlY3Rpb24AaWQtc21pbWUtY3QtYXV0aEVudmVsb3BlZERhdGEAaWQtY3QteG1sAC4lbHUAYXNzZXJ0aW9uIGZhaWxlZDogV0lUSElOX0ZSRUVMSVNUKGxpc3QpAGFzc2VydGlvbiBmYWlsZWQ6IFdJVEhJTl9BUkVOQShwdHIpAGFzc2VydGlvbiBmYWlsZWQ6IHRlbXAtPm5leHQgPT0gTlVMTCB8fCBXSVRISU5fQVJFTkEodGVtcC0+bmV4dCkAYXNzZXJ0aW9uIGZhaWxlZDogKGNoYXIgKiopdGVtcC0+bmV4dC0+cF9uZXh0ID09IGxpc3QAYXNzZXJ0aW9uIGZhaWxlZDogbGlzdCA+PSAwICYmIGxpc3QgPCBzaC5mcmVlbGlzdF9zaXplAGFzc2VydGlvbiBmYWlsZWQ6ICgocHRyIC0gc2guYXJlbmEpICYgKChzaC5hcmVuYV9zaXplID4+IGxpc3QpIC0gMSkpID09IDAAYXNzZXJ0aW9uIGZhaWxlZDogYml0ID4gMCAmJiBiaXQgPCBzaC5iaXR0YWJsZV9zaXplAGFzc2VydGlvbiBmYWlsZWQ6ICFURVNUQklUKHRhYmxlLCBiaXQpAGFzc2VydGlvbiBmYWlsZWQ6ICFzaF90ZXN0Yml0KHRlbXAsIHNsaXN0LCBzaC5iaXRtYWxsb2MpAGFzc2VydGlvbiBmYWlsZWQ6IFdJVEhJTl9GUkVFTElTVCh0ZW1wMi0+cF9uZXh0KSB8fCBXSVRISU5fQVJFTkEodGVtcDItPnBfbmV4dCkAYXNzZXJ0aW9uIGZhaWxlZDogdGVtcCAhPSBzaC5mcmVlbGlzdFtzbGlzdF0AYXNzZXJ0aW9uIGZhaWxlZDogc2guZnJlZWxpc3Rbc2xpc3RdID09IHRlbXAAYXNzZXJ0aW9uIGZhaWxlZDogdGVtcC0oc2guYXJlbmFfc2l6ZSA+PiBzbGlzdCkgPT0gc2hfZmluZF9teV9idWRkeSh0ZW1wLCBzbGlzdCkAYXNzZXJ0aW9uIGZhaWxlZDogc2hfdGVzdGJpdChjaHVuaywgbGlzdCwgc2guYml0dGFibGUpAGFzc2VydGlvbiBmYWlsZWQ6IFdJVEhJTl9BUkVOQShjaHVuaykAYXNzZXJ0aW9uIGZhaWxlZDogKGJpdCAmIDEpID09IDAAYXNzZXJ0aW9uIGZhaWxlZDogc2hfdGVzdGJpdChwdHIsIGxpc3QsIHNoLmJpdHRhYmxlKQBhc3NlcnRpb24gZmFpbGVkOiBURVNUQklUKHRhYmxlLCBiaXQpAGFzc2VydGlvbiBmYWlsZWQ6IHB0ciA9PSBzaF9maW5kX215X2J1ZGR5KGJ1ZGR5LCBsaXN0KQBhc3NlcnRpb24gZmFpbGVkOiBwdHIgIT0gTlVMTABhc3NlcnRpb24gZmFpbGVkOiAhc2hfdGVzdGJpdChwdHIsIGxpc3QsIHNoLmJpdG1hbGxvYykAYXNzZXJ0aW9uIGZhaWxlZDogc2guZnJlZWxpc3RbbGlzdF0gPT0gcHRyAEZBTFNFAFRSVUUAdHJ1ZQBZAHkAWUVTAHllcwBmYWxzZQBOAG4ATk8Abm8Ac2VjdGlvbjoALG5hbWU6ACx2YWx1ZToAWDUwOV9BVFRSSUJVVEUAb2JqZWN0AHNldAAKADoAJTAyeCVzAFg1MDlfUFVCS0VZAGFsZ29yAHB1YmxpY19rZXkAL3Vzci9sb2NhbC9zc2wAYXNzZXJ0aW9uIGZhaWxlZDogY3R4LT5kaWdlc3QtPm1kX3NpemUgPD0gRVZQX01BWF9NRF9TSVpFAGFsZ19zZWN0aW9uAGZpcHNfbW9kZQAwACV1ACUwOXUALQBzc2xfY29uZgBzZWN0aW9uPQBuYW1lPQBFTlYAYXNzZXJ0aW9uIGZhaWxlZDogdnYgPT0gTlVMTABncm91cD0AIG5hbWU9AG9wZW5zc2xfY29uZgBwYXRoAE9QRU5TU0xfaW5pdABPUEVOU1NMX2ZpbmlzaABtb2R1bGU9ACwgcGF0aD0AJS04ZAAsIHZhbHVlPQAsIHJldGNvZGU9AE9QRU5TU0xfQ09ORgAvAG9wZW5zc2wuY25mAE9wZW5TU0wgZGVmYXVsdAByYgBbJXNdICVzPSVzCgBbWyVzXV0KAGRlZmF1bHQAJWxkAGxpbmUgAAABAgQHAwYF";var tempDoublePtr=82992;function demangle(A){return A}function demangleAll(A){return A.replace(/\b__Z[\w\d_]+/g,(function(A){var e=demangle(A);return A===e?A:e+" ["+A+"]"}))}function jsStackTrace(){var A=new Error;if(!A.stack){try{throw new Error}catch(e){A=e}if(!A.stack)return"(no stack trace available)"}return A.stack.toString()}var ENV={};function __getExecutableName(){return thisProgram||"./this.program"}function ___buildEnvironment(A){var e,r;___buildEnvironment.called?(r=HEAP32[A>>2],e=HEAP32[r>>2]):(___buildEnvironment.called=!0,ENV.USER="web_user",ENV.LOGNAME="web_user",ENV.PATH="/",ENV.PWD="/",ENV.HOME="/home/web_user",ENV.LANG=("object"==typeof navigator&&navigator.languages&&navigator.languages[0]||"C").replace("-","_")+".UTF-8",ENV._=__getExecutableName(),e=getMemory(1024),r=getMemory(256),HEAP32[r>>2]=e,HEAP32[A>>2]=r);var i=[],n=0;for(var t in ENV)if("string"==typeof ENV[t]){var f=t+"="+ENV[t];i.push(f),n+=f.length}if(n>1024)throw new Error("Environment size exceeded TOTAL_ENV_SIZE!");for(var a=0;a<i.length;a++){writeAsciiToMemory(f=i[a],e),HEAP32[r+4*a>>2]=e,e+=f.length+1}HEAP32[r+4*i.length>>2]=0}var PATH={splitPath:function(A){return/^(\/?|)([\s\S]*?)((?:\.{1,2}|[^\/]+?|)(\.[^.\/]*|))(?:[\/]*)$/.exec(A).slice(1)},normalizeArray:function(A,e){for(var r=0,i=A.length-1;i>=0;i--){var n=A[i];"."===n?A.splice(i,1):".."===n?(A.splice(i,1),r++):r&&(A.splice(i,1),r--)}if(e)for(;r;r--)A.unshift("..");return A},normalize:function(A){var e="/"===A.charAt(0),r="/"===A.substr(-1);return(A=PATH.normalizeArray(A.split("/").filter((function(A){return!!A})),!e).join("/"))||e||(A="."),A&&r&&(A+="/"),(e?"/":"")+A},dirname:function(A){var e=PATH.splitPath(A),r=e[0],i=e[1];return r||i?(i&&(i=i.substr(0,i.length-1)),r+i):"."},basename:function(A){if("/"===A)return"/";var e=A.lastIndexOf("/");return-1===e?A:A.substr(e+1)},extname:function(A){return PATH.splitPath(A)[3]},join:function(){var A=Array.prototype.slice.call(arguments,0);return PATH.normalize(A.join("/"))},join2:function(A,e){return PATH.normalize(A+"/"+e)}},SYSCALLS={mappings:{},buffers:[null,[],[]],printChar:function(A,e){var r=SYSCALLS.buffers[A];0===e||10===e?((1===A?out:err)(UTF8ArrayToString(r,0)),r.length=0):r.push(e)},varargs:void 0,get:function(){return SYSCALLS.varargs+=4,HEAP32[SYSCALLS.varargs-4>>2]},getStr:function(A){return UTF8ToString(A)},get64:function(A,e){return A}};function ___sys_getegid32(){return 0}function ___sys_getuid32(){return ___sys_getegid32()}function ___syscall199(){return ___sys_getuid32()}function ___sys_getgid32(){return ___sys_getegid32()}function ___syscall200(){return ___sys_getgid32()}function ___sys_geteuid32(){return ___sys_getegid32()}function ___syscall201(){return ___sys_geteuid32()}function ___syscall202(){return ___sys_getegid32()}function _abort(){abort()}function _atexit(A,e){__ATEXIT__.unshift({func:A,arg:e})}function _emscripten_asm_const_int(){}function _emscripten_get_heap_size(){return HEAPU8.length}function abortOnCannotGrowMemory(A){abort("OOM")}function _emscripten_resize_heap(A){abortOnCannotGrowMemory(A>>>=0)}function _getenv(A){return 0===A?0:(A=UTF8ToString(A),ENV.hasOwnProperty(A)?(_getenv.ret&&_free(_getenv.ret),_getenv.ret=allocateUTF8(ENV[A]),_getenv.ret):0)}function _emscripten_memcpy_big(A,e,r){HEAPU8.copyWithin(A,e,e+r)}function _time(A){var e=Date.now()/1e3|0;return A&&(HEAP32[A>>2]=e),e}var __readAsmConstArgsArray=[],ASSERTIONS=!1;function intArrayToString(A){for(var e=[],r=0;r<A.length;r++){var i=A[r];i>255&&(ASSERTIONS&&assert(!1,"Character code "+i+" ("+String.fromCharCode(i)+")  at offset "+r+" not in 0x00-0xFF."),i&=255),e.push(String.fromCharCode(i))}return e.join("")}var decodeBase64="function"==typeof atob?atob:function(A){var e,r,i,n,t,f,a="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=",B="",o=0;A=A.replace(/[^A-Za-z0-9\+\/\=]/g,"");do{e=a.indexOf(A.charAt(o++))<<2|(n=a.indexOf(A.charAt(o++)))>>4,r=(15&n)<<4|(t=a.indexOf(A.charAt(o++)))>>2,i=(3&t)<<6|(f=a.indexOf(A.charAt(o++))),B+=String.fromCharCode(e),64!==t&&(B+=String.fromCharCode(r)),64!==f&&(B+=String.fromCharCode(i))}while(o<A.length);return B};function intArrayFromBase64(A){if("boolean"==typeof ENVIRONMENT_IS_NODE&&ENVIRONMENT_IS_NODE){var e;try{e=Buffer.from(A,"base64")}catch(r){e=new Buffer(A,"base64")}return new Uint8Array(e.buffer,e.byteOffset,e.byteLength)}try{for(var r=decodeBase64(A),i=new Uint8Array(r.length),n=0;n<r.length;++n)i[n]=r.charCodeAt(n);return i}catch(A){throw new Error("Converting base64 string to bytes failed.")}}function tryParseAsDataURI(A){if(isDataURI(A))return intArrayFromBase64(A.slice(dataURIPrefix.length))}var asmGlobalArg={Math:Math,Int8Array:Int8Array,Int16Array:Int16Array,Int32Array:Int32Array,Uint8Array:Uint8Array,Uint16Array:Uint16Array,Float32Array:Float32Array,Float64Array:Float64Array},asmLibraryArg={a:abort,b:setTempRet0,c:getTempRet0,d:___buildEnvironment,e:___sys_getegid32,f:___sys_geteuid32,g:___sys_getgid32,h:___sys_getuid32,i:___syscall199,j:___syscall200,k:___syscall201,l:___syscall202,m:__getExecutableName,n:_abort,o:_atexit,p:_emscripten_asm_const_i,q:_emscripten_asm_const_int,r:_emscripten_get_heap_size,s:_emscripten_memcpy_big,t:_emscripten_resize_heap,u:_getenv,v:_time,w:tempDoublePtr},asm=function(A,e,r){var i=new A.Int8Array(r),n=new A.Int16Array(r),t=new A.Int32Array(r),f=new A.Uint8Array(r),a=new A.Uint16Array(r),B=(new A.Float32Array(r),new A.Float64Array(r)),o=(e.w,A.Math.imul),u=A.Math.clz32,l=e.a,c=e.b,E=e.c,Q=e.d,w=(e.e,e.f,e.g,e.h,e.i),b=e.j,k=e.k,s=e.l,C=(e.m,e.n),d=e.o,I=e.p,g=(e.q,e.r),D=e.s,M=e.t,h=e.u,U=e.v,G=83008;function Z(){I(1)}function N(A,e){A|=0,e|=0;var r=0;for(r=0;(0|r)!=(0|e);)i[A+r>>0]=0|I(0),r=r+1|0}function F(A,e,r,i,n,f,a){e|=0,r|=0,i|=0,n|=0,f|=0,a|=0;var B,o,u,l,c,E=0,Q=0;c=G,G=G+80|0,(0|(B=0|pi(A|=0)))<=-1&&oi(53850,82796,0),o=0|di(),u=0|di(),l=0|di();A:do{if(0==(0|o)|0==(0|u)|0==(0|l)||(ji(l,8),E=0|$i(855,0,e,r),0==(0|E)))E=0,f=0;else if(0!=(0|mi(l,0,A,0,E))&&0!=(0|hi(o,l))){if(0|i&&0==(0|Di(o,i,n))){f=0;break}if(0!=(0|Ji(o,c,c+68|0))&&0!=(0|hi(o,l))){e:do{if(0|i)for(;;){if(!(0|Di(o,c,0|t[c+68>>2]))){f=0;break A}if((A=a>>>0>B>>>0)&&0==(0|hi(u,o))){f=0;break A}if(!(0|Di(o,i,n))){f=0;break A}if(!A)break e;if(!(0|Ji(o,f,c+64|0))){Q=27;break e}if(A=0|t[c+64>>2],!(0|Ji(u,c,c+68|0))){Q=27;break e}if(!(0|hi(o,l))){f=0;break A}a=a-A|0,f=f+A|0}else for(A=a;;){if(!(0|Di(o,c,0|t[c+68>>2]))){f=0;break A}if(A>>>0<=B>>>0){a=A;break e}if(!(0|hi(u,o))){f=0;break A}if(!(0|Ji(o,f,c+64|0))){Q=27;break e}if(a=0|t[c+64>>2],!(0|Ji(u,c,c+68|0))){Q=27;break e}if(!(0|hi(o,l))){f=0;break A}A=A-a|0,f=f+a|0}}while(0);if(27==(0|Q)){f=0;break}0|Ji(o,c,c+68|0)?(If(0|f,0|c,0|a),f=1):f=0}else f=0}else f=0}while(0);return yi(E),Ii(o),Ii(u),Ii(l),$r(c,64),G=c,0|f}function W(){var A=0;return(A=0|Sn(4,82796,0))?t[A>>2]=1:A=0,0|A}function Y(A){return 1==(0|t[(A|=0)>>2])?1:(oi(53879,82796,0),0)}function R(A){return 1==(0|t[(A|=0)>>2])?1:(oi(53879,82796,0),0)}function y(A){return 1==(0|t[(A|=0)>>2])?1:(oi(53879,82796,0),0)}function V(A){(A|=0)&&(t[A>>2]=0,Xn(A,82796,0))}function m(A,e){return e|=0,0|t[(A|=0)>>2]||(ZB[15&e](),t[A>>2]=1),1}function H(A,e){return A|=0,e|=0,(e=0|t[20508])>>>0>255?0|(A=0):(t[20508]=e+1,t[A>>2]=e,t[80880+(e<<2)>>2]=0,0|(A=1))}function v(A){return(A=0|t[(A|=0)>>2])>>>0>255?0|(A=0):0|(A=0|t[80880+(A<<2)>>2])}function J(A,e){return e|=0,(A=0|t[(A|=0)>>2])>>>0>255?0|(e=0):(t[80880+(A<<2)>>2]=e,0|(e=1))}function X(A){return t[(A|=0)>>2]=257,1}function T(A,e,r,i){return e|=0,r|=0,i|=0,i=(0|t[(A|=0)>>2])+e|0,t[A>>2]=i,t[r>>2]=i,1}function L(A,e,r){A|=0,e|=0,r|=0;var i;return i=G,G=G+16|0,t[i>>2]=A,0|t[A+4>>2]&&(x(i,0,0),A=0|t[i>>2]),t[A>>2]=e,1==(0|e)?(t[A+4>>2]=0==(0|r)?0:255,void(G=i)):(t[A+4>>2]=r,void(G=i))}function S(A,e){A|=0;var r=0;return 0|(r=0!=(0|(e|=0))&&16==(0|t[e>>2])&&0!=(0|(r=0|t[e+4>>2]))?0|function(A,e){A|=0,e|=0;var r;if(r=G,G=G+16|0,t[r>>2]=t[A+8>>2],0|(e=0|hA(0,r,0|t[A>>2],e)))return G=r,0|e;return Xe(13,199,110,0,0),G=r,0|e}(r,A):0)}function p(A,e,r){A|=0,e|=0;var i,n=0;if((0==(0|(r|=0))||0==(0|(n=0|t[r>>2])))&&!(n=0|function(){var A=0;return(A=0|Sn(16,82796,0))?(t[A+4>>2]=4,0|A):(Xe(13,130,65,0,0),0|(A=0))}()))return Xe(13,198,65,0,0),0|(r=0);Xn(0|t[(i=n+8|0)>>2],82796,0),t[i>>2]=0,A=0|TA(A,i,e),t[n>>2]=A;do{if(A){if(!(0|t[i>>2])){Xe(13,198,65,0,0);break}return r?(0|t[r>>2]||(t[r>>2]=n),0|(r=n)):0|(r=n)}Xe(13,198,112,0,0)}while(0);return 0|r&&0|t[r>>2]||cA(n),0|(r=0)}function z(A,e){A|=0,e|=0;var r;r=G,G=G+16|0,t[r>>2]=A,K(r,e,0),G=r}function K(A,e,r){A|=0,r|=0;var n=0,f=0,a=0,B=0,o=0;if(f=0|t[(e|=0)+16>>2],A&&!((a=0|i[e>>0])<<24>>24&&0==(0|t[A>>2])))switch(0!=(0|f)&&0!=(0|(n=0|t[f+16>>2]))||(n=0),a<<24>>24|0){case 0:return(n=0|t[e+8>>2])?void j(A,n):void x(A,e,r);case 5:return void x(A,e,r);case 2:if((a=0!=(0|n))&&2==(0|MB[31&n](2,A,e,0)))return;if((0|(f=0|qA(A,e)))>-1&&(0|f)<(0|t[e+12>>2])&&j(0|ne(A,o=(0|t[e+8>>2])+(20*f|0)|0),o),a&&MB[31&n](3,A,e,0),0|r)return;return Xn(0|t[A>>2],82796,0),void(t[A>>2]=0);case 4:if(!f)return;if(!(n=0|t[f+8>>2]))return;return void FB[7&n](A,e);case 1:case 6:if(0|Ae(A,-1,e))return;if((o=0!=(0|n))&&2==(0|MB[31&n](2,A,e,0)))return;if(function(A,e){e|=0;var r;if(!(A|=0))return;if(!(r=0|t[A>>2]))return;if(!(A=0|t[e+16>>2]))return;if(!(2&t[A+4>>2]))return;if(!(A=r+(0|t[A+20>>2])|0))return;Xn(0|t[A>>2],82796,0),t[A>>2]=0,t[A+4>>2]=0,t[A+8>>2]=1}(A,e),(0|(f=0|t[e+12>>2]))>0){B=0,a=(0|t[e+8>>2])+(20*f|0)|0;do{0|(f=0|te(A,a=a+-20|0,0))&&j(0|ne(A,f),f),B=B+1|0}while((0|B)<(0|t[e+12>>2]))}if(o&&MB[31&n](3,A,e,0),0|r)return;return Xn(0|t[A>>2],82796,0),void(t[A>>2]=0);default:return}}function j(A,e){A|=0;var r,i,n,f=0;if(n=G,G=G+16|0,4096&(i=0|t[(e|=0)>>2])&&(t[n+4>>2]=A,A=n+4|0),!(6&i))return K(A,0|t[e+16>>2],4096&i),void(G=n);if((0|Hn(r=0|t[A>>2]))>0){f=0;do{t[n>>2]=0|vn(r,f),K(n,0|t[e+16>>2],4096&i),f=f+1|0}while((0|f)<(0|Hn(r)))}Fn(r),t[A>>2]=0,G=n}function x(A,e,r){A|=0,e|=0,r|=0;var n=0,f=0,a=0;do{if(0|e){if(n=0|t[e+16>>2],r){if(0|n&&0|(f=0|t[n+16>>2]))return void FB[7&f](A,e)}else if(0|n&&0|(a=0|t[n+12>>2]))return void FB[7&a](A,e);if(5==(0|i[e>>0])){if(0|t[A>>2]){f=20;break}return}if(1!=(0|(n=0|t[e+4>>2]))){if(!(0|t[A>>2]))return;f=14}else f=17}else{if(n=0|t[A>>2],!(0|t[n+4>>2]))return;A=n+4|0,n=0|t[n>>2],f=14}}while(0);A:do{if(14==(0|f))switch(0|n){case 5:break A;case 6:$(0|t[A>>2]);break A;case 1:if(0|e){f=17;break A}return void(t[A>>2]=-1);case-4:x(A,0,0),Xn(0|t[A>>2],82796,0);break A;default:f=20;break A}}while(0);17!=(0|f)?(20==(0|f)&&function(A,e){if(e|=0,!(A|=0))return;16&t[A+12>>2]||Xn(0|t[A+8>>2],82796,0);if(0|e)return;Xn(A,82796,0)}(0|t[A>>2],r),t[A>>2]=0):t[A>>2]=t[e+20>>2]}function O(A,e){K(A|=0,e|=0,0)}function P(A,e,r,n){A|=0,e|=0,r|=0;var t,f,a=0,B=0,o=0,u=0,l=0,c=0,E=0,Q=0,w=0,b=0,k=0;switch(f=G,G=G+32|0,0|(n|=0)){case-1:a=0|mt(r);break;case 0:return G=f,0|(k=n);default:a=n}t=0|i[r>>0];A:do{if((t+-48&255)<3){if((0|a)<2){Xe(13,100,138,0,0),n=0;break}do{if(2!=(0|a)){n=0,B=0|i[r+1>>0],w=0,o=a+-2|0,r=r+2|0,a=f,l=24;e:for(;;){switch(0|B){case 32:case 46:break;default:k=11;break e}B=0,E=o,b=r,r=0;r:for(;;){switch(Q=E+-1|0,c=b,b=b+1|0,(c=0|i[c>>0])<<24>>24){case 32:case 46:break r}if((c+-48&255)>9){k=14;break e}if((o=0==(0|r))&B>>>0>429496720){if(!n&&!(n=0|qf())){n=0,k=52;break e}if(!(0|ta(n,B))){k=52;break e}r=1,k=20}else o?(B=(c<<24>>24)-48+(10*B|0)|0,r=0):k=20;if(20==(0|k)){if(k=0,!(0|QB(n,10))){k=52;break e}if(!(0|cB(n,(c<<24>>24)-48|0))){k=52;break e}}if((0|E)<2)break;E=Q}do{if(!w){if(t<<24>>24<50&B>>>0>39){k=26;break e}if(r){if(0|cB(n,(40*(t<<24>>24)|0)-1920|0)){k=32;break}k=52;break e}r=B+((40*(t<<24>>24)|0)-1920)|0,k=31;break}r?k=32:(r=B,k=31)}while(0);if(31==(0|k))for(k=0,o=0;;){if(B=o+1|0,i[a+o>>0]=127&r,!(r>>>=7)){u=a;break}o=B}else if(32==(0|k)){if(k=0,(0|((r=0|Kf(n))+6|0)/7)>(0|l)){if((0|a)!=(0|f)&&Xn(a,82796,0),!(a=0|Ln(32+((r+6|0)/7|0)|0,82796,0))){a=0;break}l=32+((r+6|0)/7|0)|0}if((r+12|0)>>>0<13)B=0,u=a;else for(o=(r+6|0)/7|0,u=0;;){if(o=o+-1|0,-1==(0|(r=0|lB(n,128)))){k=52;break e}if(B=u+1|0,i[a+u>>0]=r,!o){u=a;break}u=B}}if(a=B+w|0,A){if((0|a)>(0|e)){k=43;break}if((0|B)>1)for(r=w;w=B,B=B+-1|0,a=r+1|0,i[A+r>>0]=-128|i[u+B>>0],!((0|w)<=2);)r=a;else a=w;i[A+a>>0]=0|i[u>>0],a=a+1|0}if((0|E)<2){k=49;break}B=c<<24>>24,w=a,o=Q,r=b,a=u}if(11==(0|k))Xe(13,100,131,0,0),k=52;else if(14==(0|k))Xe(13,100,130,0,0),k=52;else if(26==(0|k))Xe(13,100,147,0,0),k=52;else if(43==(0|k))Xe(13,100,107,0,0),a=u,k=52;else if(49==(0|k)){if((0|u)==(0|f))break;Xn(u,82796,0);break}if(52==(0|k)&&(0|a)==(0|f))break A;Xn(a,82796,0);break A}n=0,a=0}while(0);return Pf(n),G=f,0|(k=a)}Xe(13,100,122,0,0),n=0}while(0);return Pf(n),G=f,0|(k=0)}function _(A,e){A|=0,e|=0;var r,i=0,n=0,f=0;r=G,G=G+80|0;do{if(0!=(0|e)&&0!=(0|t[e+16>>2])){if((0|(i=0|Or(r,80,e,0)))<=79){if((0|i)<1){i=0|se(A,54001,9),i=(0|be(A,0|t[e+16>>2],0|t[e+12>>2]))+i|0;break}se(A,r,i);break}if(!(n=0|Ln(i+1|0,82796,0))){i=-1;break}if(Or(n,i+1|0,e,0),se(A,n,i),(0|n)==(0|r))break;Xn(n,82796,0);break}f=3}while(0);return 3==(0|f)&&(i=0|se(A,64038,4)),G=r,0|i}function q(A,e,r){A|=0;var n,f=0,a=0,B=0,o=0,u=0,l=0;if(n=G,G=G+32|0,!(0==(0|(e|=0))|(0|(r|=0))<1)&&0|(B=0|t[e>>2])&&(0|i[B+(r+-1)>>0])>=0){if(t[n+8>>2]=0,t[n+16>>2]=B,t[n+12>>2]=r,t[n+20>>2]=0,0|(a=0|zr(n)))return f=0|Sr(a),0|A&&($(0|t[A>>2]),t[A>>2]=f),t[e>>2]=(0|t[e>>2])+r,G=n,0|(e=f);for(o=0,a=B;;){if(-128==(0|i[a>>0])){if(!o){l=13;break}if((0|i[a+-1>>0])>=0){l=13;break}}if((0|(o=o+1|0))>=(0|r))break;a=a+1|0}if(13==(0|l))return Xe(13,196,216,0,0),G=n,0|(e=0);0!=(0|A)&&0!=(0|(f=0|t[A>>2]))&&0!=(1&t[f+20>>2]|0)||(l=18);do{if(18==(0|l)){if(0|(f=0|Sn(24,82796,0))){t[f+20>>2]=1;break}return Xe(13,123,65,0,0),G=n,0|(e=0)}}while(0);o=0|t[e>>2],a=0|t[(u=f+16|0)>>2],t[u>>2]=0,B=f+12|0,0!=(0|a)&&(0|t[B>>2])>=(0|r)||(l=23);do{if(23==(0|l)){if(t[B>>2]=0,Xn(a,82796,0),0|(a=0|Ln(r,82796,0))){t[(l=f+20|0)>>2]=8|t[l>>2];break}return Xe(13,196,65,0,0),0|A&&(0|t[A>>2])==(0|f)?(G=n,0|(e=0)):($(f),G=n,0|(e=0))}}while(0);return If(0|a,0|o,0|r),t[u>>2]=a,t[B>>2]=r,t[f>>2]=0,t[f+4>>2]=0,0|A&&(t[A>>2]=f),t[e>>2]=o+r,G=n,0|(e=f)}return Xe(13,196,216,0,0),G=n,0|(e=0)}function $(A){var e=0;(A|=0)&&(4&(e=0|t[A+20>>2])&&(Xn(0|t[A>>2],82796,0),Xn(0|t[A+4>>2],82796,0),t[A+4>>2]=0,t[A>>2]=0,e=0|t[A+20>>2]),8&e&&(Xn(0|t[A+16>>2],82796,0),t[A+16>>2]=0,t[A+12>>2]=0,e=0|t[A+20>>2]),1&e&&Xn(A,82796,0))}function AA(A,e){return 0|TA(A|=0,e|=0,51980)}function eA(A){cA(A|=0)}function rA(){return 0|EA(2)}function iA(){return 0|pA(52092)}function nA(A){z(A|=0,52092)}function tA(A,e,r,n,a){e|=0,r|=0,n|=0,a|=0;var B,o=0,u=0,l=0,c=0,E=0;B=0|t[(A|=0)>>2];A:do{if(0|a){if(31==(31&(E=0|f[B>>0])|0)){if(!(a+-1|0))break;if((o=0|i[B+1>>0])<<24>>24<0)for(c=0==(a+-2|0),u=a+-2|0,l=B+2|0,o&=127;;){if(c|(0|o)>16777215)break A;if(c=0|i[l>>0],l=l+1|0,o=o<<7|127&c,u=u+-1|0,!(c<<24>>24<0)){c=0==(0|u);break}c=0==(0|u)}else c=0==(a+-2|0),u=a+-2|0,l=B+2|0,o&=127;if(c)break;c=u,u=l}else{if(!(a+-1|0))break;c=a+-1|0,u=B+1|0,o=31&E}if(t[r>>2]=o,t[n>>2]=192&E,(0|c)>=1){if((l=0|i[u>>0])<<24>>24==-128){if(t[e>>2]=0,!(32&E))break;o=u+1|0,u=0,c=1}else{u=u+1|0;e:do{if(128&l){if((1+(127&l)|0)>=(0|c))break A;if(127&l){for(o=127&l,n=u;!((l=0|i[n>>0])<<24>>24);){if(u=n+1|0,!(o=o+-1|0)){o=u,u=0;break e}n=u}if(o>>>0>4)break A;if(u=o+-1|0){c=u,u=255&l,l=n;do{c=c+-1|0,u=u<<8|0|f[(l=l+1|0)>>0]}while(0!=(0|c))}else u=255&l;if((0|u)<0)break A;o=n+o|0}else o=u,u=0}else o=u,u=127&l}while(0);t[e>>2]=u,c=0}return(0|u)>(B+a-(l=o)|0)?(Xe(13,114,155,0,0),o=32&E|128):o=32&E,t[A>>2]=l,0|(A=o|c)}}}while(0);return Xe(13,114,123,0,0),0|(A=128)}function fA(A,e,r,n,f){e|=0,r|=0,n|=0,f|=0;var a,B=0,o=0,u=0;if(a=0|t[(A|=0)>>2],f=192&f|(0==(0|e)?0:32),(0|n)<31)i[a>>0]=f|31&n,o=a+1|0;else{for(i[a>>0]=31|f,f=0,B=n;o=f+1|0,B>>>=7;)f=o;for(B=o;u=127&n,i[a+B>>0]=(0|B)==(0|o)?u:-128|u,(0|f)>0;)B=f,f=f+-1|0,n>>=7;o=a+1+o|0}if(2==(0|e))return i[o>>0]=-128,u=o+1|0,void(t[A>>2]=u);if((0|r)<128)return i[o>>0]=r,u=o+1|0,void(t[A>>2]=u);B=0,f=r;do{f>>>=8,B=B+1|0}while(0!=(0|f));for(i[o>>0]=128|B,n=B,f=r;i[o+n>>0]=f,(0|n)>1;)n=n+-1|0,f>>=8;u=o+1+B|0,t[A>>2]=u}function aA(A){var e;return e=0|t[(A|=0)>>2],i[e>>0]=0,i[e+1>>0]=0,t[A>>2]=e+2,2}function BA(A,e,r){A|=0;var i=0;if((0|(e|=0))<0)return-1;if((0|(r|=0))>30){i=1;do{r>>>=7,i=i+1|0}while(0!=(0|r))}else i=1;if(2!=(0|A)){if(i=i+1|0,(0|e)>127){r=e;do{r>>>=8,i=i+1|0}while(0!=(0|r))}}else i=i+3|0;return 0|((0|i)<(2147483647-e|0)?i+e|0:-1)}function oA(A,e){var r=0,n=0,f=0,a=0;if(!(e|=0))return 0|(e=0);t[(A|=0)+4>>2]=t[e+4>>2],a=0|t[e+8>>2],r=0|t[e>>2];do{if((0|r)<0){if(a){r=0|mt(a);break}return 0|(e=0)}}while(0);if(n=0|t[A+8>>2],(0|t[A>>2])>(0|r)?n||(n=0,f=7):f=7,7==(0|f)){if(f=0|Tn(n,r+1|0,82796,0),t[A+8>>2]=f,!f)return Xe(13,186,65,0,0),t[A+8>>2]=n,0|(e=0);n=f}return t[A>>2]=r,0|a&&(If(0|n,0|a,0|r),i[(0|t[A+8>>2])+r>>0]=0),a=128&t[A+12>>2],t[A+12>>2]=a,t[A+12>>2]=-129&t[e+12>>2]|a,0|(e=1)}function uA(A,e,r){A|=0,e|=0,r|=0;var n=0,f=0;do{if((0|r)<0){if(e){r=0|mt(e);break}return 0|(A=0)}}while(0);if(n=0|t[A+8>>2],(0|t[A>>2])>(0|r)?n||(n=0,f=6):f=6,6==(0|f)){if(f=0|Tn(n,r+1|0,82796,0),t[A+8>>2]=f,!f)return Xe(13,186,65,0,0),t[A+8>>2]=n,0|(A=0);n=f}return t[A>>2]=r,e?(If(0|n,0|e,0|r),i[(0|t[A+8>>2])+r>>0]=0,0|(A=1)):0|(A=1)}function lA(A){var e=0;return(A|=0)?(e=0|Sn(16,82796,0))?(t[e+4>>2]=4,0|oA(e,A)?0|e:(16&(A=0|t[e+12>>2])||Xn(0|t[e+8>>2],82796,0),128&A|0||Xn(e,82796,0),0|(e=0))):(Xe(13,130,65,0,0),0|(e=0)):0|(e=0)}function cA(A){var e;(A|=0)&&(16&(e=0|t[A+12>>2])||Xn(0|t[A+8>>2],82796,0),128&e|0||Xn(A,82796,0))}function EA(A){A|=0;var e;return(e=0|Sn(16,82796,0))?(t[e+4>>2]=A,0|(A=e)):(Xe(13,130,65,0,0),0|(A=0))}function QA(A,e,r,i,n){A|=0,e|=0,r|=0,i|=0,n|=0;var f,a=0,B=0,o=0,u=0;f=G,G=G+32|0,(a=0|t[20509])?u=3:(a=0|function(A){A|=0;var e,r=0;if(0|(e=0|Sn(20,82796,0))?(r=0|Sn(16,82796,0),t[e+4>>2]=r,0|r):0)return t[e+16>>2]=A,t[e+12>>2]=4,0|(r=e);return Xn(e,82796,0),0|(r=0)}(23),t[20509]=a,0|a&&(u=3));A:do{if(3==(0|u)){t[f>>2]=A,(o=0!=(0|(B=(0|(a=0|Rn(a,f)))>-1?0|vn(0|t[20509],a):0|Pr(f,64,24,20,24))))&&0!=(1&t[B+16>>2]|0)?a=B:u=8;do{if(8==(0|u)){if(!(a=0|Sn(20,82796,0)))break A;if(!(0|yn(0|t[20509],a))){Xn(a,82796,0);break A}if(o){t[a>>2]=t[B>>2],t[a+4>>2]=t[B+4>>2],t[a+8>>2]=t[B+8>>2],t[a+12>>2]=t[B+12>>2],t[a+16>>2]=1|t[B+16>>2];break}t[a>>2]=A,t[a+4>>2]=-1,t[a+8>>2]=-1,t[a+16>>2]=1;break}}while(0);return(0|e)>-1&&(t[a+4>>2]=e),(0|r)>-1&&(t[a+8>>2]=r),0|i&&(t[a+12>>2]=i),n?(t[a+16>>2]=1|n,G=f,0|(e=1)):(G=f,0|(e=1))}}while(0);return Xe(13,129,65,0,0),G=f,0|(e=0)}function wA(A,e){e|=0;var r,n=0,a=0,B=0,o=0,u=0,l=0,c=0;if(0!=(0|(r=0|t[(A|=0)+8>>2]))&0!=(0|(B=0|t[A>>2]))){if(n=0|i[r>>0],A=0==(256&t[A+4>>2]|0))n=A&n<<24>>24<0&1,A=0;else if((255&n)<=128)if(n<<24>>24==-128){if(B>>>0>1){A=1,n=0;do{n=0|n|f[r+A>>0],A=A+1|0}while((0|A)!=(0|B));A=(0!=(0|n))<<31>>31}else A=0;n=1&A}else n=0,A=-1;else n=1,A=-1;c=B,a=n,l=n+B|0}else c=0,a=0,A=0,l=1;if(!e)return 0|l;if(!(n=0|t[e>>2]))return 0|l;if(i[n>>0]=A,0|c)for(u=1&A,B=n+a+c|0,o=c,a=r+c|0;o=o+-1|0,n=u+(255&(i[(a=a+-1|0)>>0]^A))|0,i[(B=B+-1|0)>>0]=n,o;)u=n>>>8;return t[e>>2]=(0|t[e>>2])+l,0|l}function bA(A,e,r){A|=0,r|=0;var n,a,B=0,o=0,u=0,l=0,c=0;if(a=G,G=G+16|0,o=0|t[(e|=0)>>2],!r)return Xe(13,226,222,0,0),G=a,0|(e=0);if(n=0|i[o>>0],1!=(0|r)){switch(n<<24>>24){case 0:c=8;break;case-1:u=1,l=0;do{l=0|l|f[o+u>>0],u=u+1|0}while((0|u)!=(0|r));l?c=8:o=0;break;default:o=0}if(8==(0|c)){if((i[o+1>>0]^n)<<24>>24>-1)return Xe(13,226,221,0,0),G=a,0|(e=0);o=1}if(!(o=r-o|0))return G=a,0|(e=0)}else o=1;0!=(0|A)&&0!=(0|(B=0|t[A>>2]))||(c=13);do{if(13==(0|c)){if(B=0|rA()){t[B+4>>2]=2;break}return G=a,0|(e=0)}}while(0);return 0|uA(B,0,o)?(function(A,e,r,n){A|=0,e|=0,r|=0;var a=0,B=0,o=0,u=0;if(!(n|=0))return Xe(13,226,222,0,0),0|(u=0);u=0|i[r>>0],0|e&&(t[e>>2]=-128&u&255);if(1==(0|n))return A?(e=0|i[r>>0],(-128&u)<<24>>24?(i[A>>0]=1+(255&~e),0|(u=1)):(i[A>>0]=e,0|(u=1))):0|(u=1);switch(0|i[r>>0]){case 0:B=14;break;case-1:e=1,a=0;do{a|=f[r+e>>0],e=e+1|0}while((0|e)!=(0|n));a?B=14:e=0;break;default:e=0}if(14==(0|B)){if((-128&u)<<24>>24==(-128&i[r+1>>0])<<24>>24)return Xe(13,226,221,0,0),0|(u=0);e=1}if(e=n-e|0,!A)return 0|(u=e);if(!e)return 0|(u=0);a=u<<24>>24>>7&1,A=A+e|0,o=e,B=r+n|0;for(;o=o+-1|0,a=a+(255&(i[(B=B+-1|0)>>0]^u<<24>>24>>7))|0,i[(A=A+-1|0)>>0]=a,o;)a>>>=8}(0|t[B+8>>2],a,0|t[e>>2],r),0|t[a>>2]&&(t[(c=B+4|0)>>2]=256|t[c>>2]),t[e>>2]=(0|t[e>>2])+r,A?(t[A>>2]=B,G=a,0|(e=B)):(G=a,0|(e=B))):(Xe(13,194,65,0,0),0|A&&(0|t[A>>2])==(0|B)?(G=a,0|(e=0)):(function(A){cA(A|=0)}(B),G=a,0|(e=0)))}function kA(A,e,r){A|=0,r|=0;var i=0,n=0,a=0,B=0;if(!(e|=0))return Xe(13,227,67,0,0),0|(A=0);if((-257&(B=0|t[e+4>>2])|0)!=(0|r))return Xe(13,227,225,0,0),0|(A=0);if(a=0|t[e+8>>2],(e=0|t[e>>2])>>>0>8)return Xe(13,225,223,0,0),0|(A=0);if(!a)return 0|(A=0);if(e){r=0,i=0,n=0;do{r=0|Cf(0|r,0|i,8),i=0|E(),r=0|r|f[a+n>>0],n=n+1|0}while((0|n)!=(0|e))}else i=0,r=0;return e=(0|i)>-1|-1==(0|i)&r>>>0>4294967295,256&B?e?(a=0|Ef(0,0,0|r,0|i),B=0|E(),t[A>>2]=a,t[A+4>>2]=B,0|(A=1)):0==(0|r)&-2147483648==(0|i)?(t[A>>2]=0,t[A+4>>2]=-2147483648,0|(A=1)):(Xe(13,224,224,0,0),0|(A=0)):e?(t[A>>2]=r,t[A+4>>2]=i,0|(A=1)):(Xe(13,224,223,0,0),0|(A=0))}function sA(A,e){e|=0;var r,n=0,f=0;if(r=G,G=G+16|0,t[(A|=0)+4>>2]=2,(0|e)<0){n=0|Ef(0,0,0|e,((0|e)<0)<<31>>31|0),f=0|E(),e=8;do{i[r+(e=e+-1|0)>>0]=n,n=0|sf(0|n,0|f,8),f=0|E()}while(!(0==(0|n)&0==(0|f)));n=258}else{f=e,n=((0|e)<0)<<31>>31,e=8;do{i[r+(e=e+-1|0)>>0]=f,f=0|sf(0|f,0|n,8),n=0|E()}while(!(0==(0|f)&0==(0|n)));n=2}return t[A+4>>2]=n,A=0|uA(A,r+e|0,8-e|0),G=r,0|A}function CA(A){var e,r,i;return e=G,G=G+16|0,(A|=0)?0|kA(e,A,2)?(r=0|cf(0|(A=0|t[e>>2]),0|t[e+4>>2],-2147483648,0),i=0|E(),G=e,0|(A=i>>>0>0|0==(0|i)&r>>>0>4294967295?-1:A)):(G=e,0|(A=-1)):(G=e,0|(A=0))}function dA(A,e){return A|=0,t[(e|=0)>>2]=0,0|ct(A,124,1,14,e)}function IA(A,e,r,n,a){A|=0,e|=0,n|=0,a|=0;var B,o,u,l=0,c=0;if(u=G,G=G+48|0,!(r|=0))return G=u,0|(c=1);if(n=(n=0==(0|Ia(r)))?82796:80624,!(0|Ie(A,a,128)))return G=u,0|(c=0);if(0|jf(r))return t[u>>2]=e,c=(0|ge(A,54475,u))>0&1,G=u,0|c;if((0|Kf(r))<33)return a=0|t[(0|En(r))>>2],c=0|t[(0|En(r))>>2],t[u+8>>2]=e,t[u+8+4>>2]=n,t[u+8+8>>2]=a,t[u+8+12>>2]=n,t[u+8+16>>2]=c,c=(0|ge(A,54481,u+8|0))>0&1,G=u,0|c;o=0|Ln(B=1+((7+(0|Kf(r))|0)/8|0)|0,82796,0);do{if(0!=(0|o)&&(i[o>>0]=0,l=45==(0|i[n>>0])?54501:82796,t[u+32>>2]=e,t[u+32+4>>2]=l,(0|ge(A,54513,u+32|0))>=1)){r=0|ua(r,o+1|0),l=(n=0|i[o+1>>0])<<24>>24<0?o:o+1|0;A:do{if(r+((255&n)>>>7&255)|0)for(e=0;;){if(!((e>>>0)%15|0)){if(0|e&&(0|Ce(A,80479))<1)break A;if(!(0|Ie(A,a+4|0,128)))break A}if(t[u+40>>2]=f[l+e>>0],t[u+40+4>>2]=(0|e)==(r+((255&n)>>>7&255)+-1|0)?82796:80481,e=e+1|0,(0|ge(A,80483,u+40|0))<1)break A;if(e>>>0>=(r+((255&n)>>>7&255)|0)>>>0){c=17;break}}else c=17}while(0);if(17==(0|c)&&(0|se(A,80479,1))>=1){n=1;break}n=0}else n=0}while(0);return pn(o,B,82796,0),G=u,0|(c=n)}function gA(A,e){A|=0,e|=0;var r,i=0;for(r=G,G=G+128|0;;){if(t[r>>2]=r+8,t[r+8>>2]=e,!(e=0|Pr(r,52120,3,4,26))){i=6;break}if(!(e=0|t[e>>2])){e=0;break}if(!(1&t[e+8>>2]))break;e=0|t[e+4>>2]}return 6==(0|i)&&(e=0),A?(t[A>>2]=0,G=r,0|e):(G=r,0|e)}function DA(A,e){return 0|function(A,e){e|=0;var r,i=0;return 0|(r=(i=0|t[(A|=0)>>2])-(0|t[e>>2])|0)?0|(e=r):0|(i=0|Ht(0|t[A+8>>2],0|t[e+8>>2],i))?0|(e=i):0|(e=(0|t[A+4>>2])-(0|t[e+4>>2])|0)}(A|=0,e|=0)}function MA(A,e,r){return 0|uA(A|=0,e|=0,r|=0)}function hA(A,e,r,n){A|=0,e|=0,r|=0,n|=0;var f;return f=G,G=G+32|0,t[f>>2]=0,A=0==(0|A)?f:A,i[f+8>>0]=0,(0|UA(A,e,r,n,-1,0,0,f+8|0,0))<1?(O(A,n),G=f,0|(r=0)):(r=0|t[A>>2],G=f,0|r)}function UA(A,e,r,n,f,a,B,o,u){A|=0,e|=0,r|=0,n|=0,f|=0,a|=0,B|=0,o|=0,u|=0;var l,c=0,E=0,Q=0,w=0,b=0,k=0,s=0,C=0;if(l=G,G=G+48|0,t[l+28>>2]=r,E=0|t[n+16>>2],t[l+32>>2]=0,!A)return G=l,0|(n=0);C=0|E&&0!=(0|(c=0|t[E+16>>2]))?c:0;A:do{if(!((0|u)>29)){e:do{switch(0|i[n>>0]){case 0:if(!(c=0|t[n+8>>2]))return n=0|ZA(A,e,r,n,f,a,B,o),G=l,0|n;if(!(-1==(0|f)&B<<24>>24==0)){Xe(13,120,170,0,0);break A}return n=0|GA(A,e,r,c,0,o,u+1|0),G=l,0|n;case 5:if(t[l+32>>2]=t[e>>2],!(0|NA(0,l+24|0,l+38|0,0,0,l+32|0,r,-1,0,1,o))){Xe(13,120,58,0,0);break A}if(0|i[l+38>>0]){if(B<<24>>24)return G=l,0|(n=-1);Xe(13,120,139,0,0);break A}if(E=(c=0|t[l+24>>2])>>>0>30?0:0|t[1136+(c<<2)>>2],t[n+4>>2]&E|0)return n=0|ZA(A,e,0|t[l+28>>2],n,c,0,0,o),G=l,0|n;if(B<<24>>24)return G=l,0|(n=-1);Xe(13,120,140,0,0);break A;case 4:return n=0|GB[0&t[E+16>>2]](A,e,r,n,f,a,B,o),G=l,0|n;case 2:if((Q=0!=(0|C))&&0==(0|MB[31&C](4,A,n,0))){s=100;break e}if(0|t[A>>2])(0|(c=0|qA(A,n)))>-1&&(0|c)<(0|t[n+12>>2])&&(j(0|ne(A,k=(0|t[n+8>>2])+(20*c|0)|0),k),$A(A,-1,n));else if(!(0|xA(A,n))){Xe(13,120,58,0,0);break A}t[l+32>>2]=t[e>>2],E=0|t[n+12>>2];r:do{if((0|E)>0){if(-1==(0|(a=0|GA(E=0|ne(A,c=0|t[n+8>>2]),l+32|0,r,c,1,o,u+1|0))))for(E=0;;){if((0|(f=E+1|0))>=(0|(E=0|t[n+12>>2]))){c=f;break r}if(c=c+20|0,a=0|t[l+28>>2],-1!=(0|(a=0|GA(E=0|ne(A,c),l+32|0,a,c,1,o,u+1|0))))break;E=f}else f=0;if((0|a)<=0){if(j(E,c),Xe(13,120,58,0,0),c)break e;break A}E=0|t[n+12>>2],c=f;break}c=0}while(0);if((0|c)==(0|E)){if(!(B<<24>>24)){Xe(13,120,143,0,0);break A}return O(A,n),G=l,0|(n=-1)}if($A(A,c,n),Q&&0==(0|MB[31&C](5,A,n,0))){s=100;break e}return t[e>>2]=t[l+32>>2],G=l,0|(n=1);case 1:case 6:switch(t[l+32>>2]=t[e>>2],0|(c=0|NA(l+28|0,0,0,l+37|0,l+36|0,l+32|0,r,-1==(0|f)?16:f,-1==(0|f)?0:a,B,o))){case 0:Xe(13,120,58,0,0);break A;case-1:return G=l,0|(n=c);default:if(0|E&&0!=(4&t[E+4>>2]|0)?(t[l+28>>2]=r-(0|t[l+32>>2])+(0|t[e>>2]),w=1):w=0|i[l+37>>0],!(0|i[l+36>>0])){Xe(13,120,149,0,0);break A}if(0==(0|t[A>>2])&&0==(0|xA(A,n))){Xe(13,120,58,0,0);break A}if((k=0!=(0|C))&&0==(0|MB[31&C](4,A,n,0))){s=100;break e}E=0|t[n+8>>2];r:do{if((0|t[n+12>>2])>0){for(a=0;768&t[E>>2]|0&&0|(Q=0|te(A,E,0))&&j(0|ne(A,Q),Q),!((0|(a=a+1|0))>=(0|(c=0|t[n+12>>2])));)E=E+20|0;if(E=0|t[n+8>>2],(0|c)>0){for(a=0;;){if(!(c=0|te(A,E,1)))break A;if(r=0|ne(A,c),!(f=0|t[l+28>>2])){s=82;break r}if(Q=0|t[l+32>>2],(0|f)>=2&&0==(0|i[Q>>0])&&0==(0|i[Q+1>>0]))break;switch(0|GA(r,l+32|0,f,c,B=(0|a)==((0|t[n+12>>2])-1|0)?0:1&t[c>>2],o,u+1|0)){case 0:break e;case-1:j(r,c);break;default:t[l+28>>2]=Q-(0|t[l+32>>2])+(0|t[l+28>>2])}if(E=E+20|0,(0|(a=a+1|0))>=(0|t[n+12>>2])){s=82;break r}}if(t[l+32>>2]=Q+2,0|i[l+37>>0]){t[l+28>>2]=f+Q-(Q+2),i[l+37>>0]=0,c=f+Q-(Q+2)|0;break}Xe(13,120,159,0,0);break A}a=0,s=82}else a=0,s=82}while(0);do{if(82==(0|s)&&(c=0|t[l+28>>2],0|i[l+37>>0])){if((0|c)>=2&&(b=0|t[l+32>>2],0==(0|i[b>>0]))&&0==(0|i[b+1>>0])){t[l+32>>2]=b+2;break}Xe(13,120,137,0,0);break A}}while(0);if(w<<24>>24==0&0!=(0|c)){Xe(13,120,148,0,0);break A}r:do{if((0|a)<(0|t[n+12>>2])){for(;;){if(!(c=0|te(A,E,1)))break A;if(!(1&t[c>>2]))break;if(j(0|ne(A,c),c),(0|(a=a+1|0))>=(0|t[n+12>>2]))break r;E=E+20|0}Xe(13,120,121,0,0);break e}}while(0);if(!(0|re(A,s=0|t[e>>2],(c=0|t[l+32>>2])-s|0,n))){s=100;break e}if(k&&0==(0|MB[31&C](5,A,n,0))){s=100;break e}return t[e>>2]=c,G=l,0|(n=1)}default:return G=l,0|(n=0)}}while(0);if(100==(0|s)){Xe(13,120,100,0,0);break}return A=0|t[c+12>>2],n=0|t[n+24>>2],t[l>>2]=54531,t[l+4>>2]=A,t[l+8>>2]=54538,t[l+12>>2]=n,Ke(4,l),G=l,0|(n=0)}Xe(13,120,201,0,0)}while(0);return n=0|t[n+24>>2],t[l+16>>2]=54546,t[l+16+4>>2]=n,Ke(2,l+16|0),G=l,0|(n=0)}function GA(A,e,r,n,f,a,B){e|=0,r|=0,f|=0,a|=0,B|=0;var o,u=0;if(o=G,G=G+16|0,!(A|=0))return G=o,0|(e=0);if(u=0|t[(n|=0)>>2],t[o>>2]=t[e>>2],!(16&u))return e=0|WA(A,e,r,n,f,a,B),G=o,0|e;u=0|NA(o+4|0,0,0,o+9|0,o+8|0,o,r,0|t[n+4>>2],192&u,f,a),r=0|t[o>>2];A:do{switch(0|u){case 0:Xe(13,132,58,0,0),u=0;break;case-1:break;default:if(!(0|i[o+8>>0])){Xe(13,132,120,0,0),u=0;break A}if(!(0|WA(A,o,0|t[o+4>>2],n,0,a,B))){Xe(13,132,58,0,0),u=0;break A}u=0|t[o>>2],r=(0|t[o+4>>2])+(r-u)|0,t[o+4>>2]=r;do{if(0|i[o+9>>0]){if((0|r)>=2&&0==(0|i[u>>0])&&0==(0|i[u+1>>0])){t[o>>2]=u+2,u=u+2|0,r=19;break}Xe(13,132,137,0,0),r=20}else r?(Xe(13,132,119,0,0),r=20):r=19}while(0);if(19==(0|r))return t[e>>2]=u,G=o,0|(e=1);if(20==(0|r))return G=o,0|(e=0)}}while(0);return G=o,0|(e=u)}function ZA(A,e,r,n,a,B,o,u){A|=0,e|=0,r|=0,n|=0,a|=0,B|=0,o|=0,u|=0;var l,c=0,E=0,Q=0,w=0,b=0,k=0,s=0;if(l=G,G=G+48|0,i[l+44>>0]=0,t[l>>2]=0,t[l+4>>2]=0,t[l+8>>2]=0,t[l+12>>2]=0,!A)return Xe(13,108,125,0,0),G=l,0|(s=0);if(5==(0|i[n>>0])?(c=a,a=-1):c=0|t[n+4>>2],t[l+24>>2]=c,-4==(0|c)){if((0|a)>-1)return Xe(13,108,127,0,0),G=l,0|(s=0);if(o<<24>>24)return Xe(13,108,126,0,0),G=l,0|(s=0);if(t[l+16>>2]=t[e>>2],!(0|NA(0,l+24|0,l+40|0,0,0,l+16|0,r,-1,0,0,u)))return Xe(13,108,58,0,0),G=l,0|(s=0);0|i[l+40>>0]?(t[l+24>>2]=-3,c=-3):c=0|t[l+24>>2],b=l+16|0,k=e}else b=l+16|0,k=e;switch(Q=-1==(0|a),t[b>>2]=t[k>>2],0|(a=0|NA(l+20|0,0,0,l+45|0,l+46|0,l+16|0,r,Q?c:a,Q?0:B,o,u))){case 0:return Xe(13,108,58,0,0),G=l,0|(s=0);case-1:return G=l,0|(s=a);default:a=0|t[l+24>>2];A:do{switch(0|a){case-3:u?(i[u>>0]=0,s=24):s=24;break;case 17:case 16:if(!(0|i[l+46>>0]))return Xe(13,108,156,0,0),G=l,0|(s=0);s=24;break;default:if(!(0|i[l+46>>0])){e=0|t[l+16>>2],a=0|t[l+20>>2],t[l+16>>2]=e+a,s=60;break A}switch(0|a){case 1:case 2:case 5:case 6:case 10:return Xe(13,108,195,0,0),G=l,0|(s=0)}if(i[l+44>>0]=1,0|FA(l,l+16|0,0|t[l+20>>2],0|i[l+45>>0],0)){if(0|Gt(l,(a=0|t[l>>2])+1|0)){i[(0|t[l+4>>2])+a>>0]=0,e=0|t[l+4>>2],s=60;break A}Xe(13,108,65,0,0),a=0;break A}a=0}}while(0);A:do{if(24==(0|s)){if(Q=0|t[e>>2],!((a=0|i[l+45>>0])<<24>>24)){s=0|t[l+16>>2],a=0|t[l+20>>2],t[l+16>>2]=s+a,e=Q,a=s-Q+a|0,s=60;break}c=0|t[l+20>>2];e:do{if((0|c)>0){B=1,o=a,a=0|t[b>>2],u=0;r:for(;;){for(e=B,E=a;;){if(1==(0|c)){c=1;break}if(0|i[(B=E)>>0])break;if(0|i[B+1>>0])break;if(!(a=e+-1|0)){s=49;break r}if(!((0|c)>2))break e;e=a,c=c+-2|0,E=B+2|0}if(t[l+28>>2]=E,128&(r=0|tA(l+28|0,l+32|0,l+40|0,l+36|0,c))?(Xe(13,104,102,0,0),a=E,r=u,B=1):(1&r?(B=c+E-(a=0|t[l+28>>2])|0,t[l+32>>2]=B):(a=0|t[l+28>>2],B=0|t[l+32>>2]),o=1&r,r=B,B=0),B){s=40;break}if(o<<24>>24){if(-1==(0|e)){s=43;break}B=e+1|0}else B=e,a=a+r|0;if((0|(c=c+E-a|0))<=0){s=47;break}u=r}if(40==(0|s)){Xe(13,190,58,0,0),a=0;break A}if(43==(0|s)){Xe(13,190,58,0,0),a=0;break A}if(47==(0|s)){if(B)break}else 49==(0|s)&&(a=B+2|0);t[b>>2]=a,e=Q,a=a-Q|0,s=60;break A}}while(0);Xe(13,190,137,0,0),a=0}}while(0);A:do{if(60==(0|s)){if(o=0|t[l+24>>2],t[l+40>>2]=e,u=e,0!=(0|(c=0|t[n+16>>2]))&&0!=(0|(w=0|t[c+20>>2]))){if(!(0|UB[7&w](A,e,a,o,l+44|0,n))){a=0;break}}else s=62;do{if(62==(0|s)){e:do{if(-4==(0|t[n+4>>2])){c=0|t[A>>2];do{if(!c){if(c=0|iA()){t[A>>2]=c;break}nA(0);break e}}while(0);(0|t[c>>2])!=(0|o)&&L(c,o,0),r=A,A=c+4|0,s=71}else r=0,c=0,s=71}while(0);if(71==(0|s)){e:do{switch(0|o){case 6:0|q(A,l+40|0,a)&&(s=99);break;case 5:if(a){Xe(13,204,144,0,0);break e}t[A>>2]=1,s=95;break e;case 1:if(1==(0|a)){t[A>>2]=f[e>>0],s=99;break e}Xe(13,204,106,0,0);break e;case 3:0|SA(A,l+40|0,a)&&(s=99);break;case 10:case 2:0|bA(A,l+40|0,a)&&(s=4+(0|t[A>>2])|0,t[s>>2]=256&t[s>>2]|o,s=95);break;case 30:1&a?Xe(13,204,214,0,0):s=86;break;case 28:3&a?Xe(13,204,215,0,0):s=86;break;default:s=86}}while(0);e:do{if(86==(0|s)){B=0|t[A>>2];do{if(!B){if(B=0|EA(o)){t[A>>2]=B;break}Xe(13,204,65,0,0);break e}t[B+4>>2]=o}while(0);if(0|i[l+44>>0]){Xn(0|t[(s=B+8|0)>>2],82796,0),t[s>>2]=u,t[B>>2]=a,i[l+44>>0]=0,s=99;break}0|uA(B,e,a)?s=99:(Xe(13,204,65,0,0),cA(B),t[A>>2]=0)}else 95==(0|s)&&(5==(0|o)&0!=(0|c)?(t[c+4>>2]=0,s=99):s=99)}while(0);if(99==(0|s))break;nA(c),0|r&&(t[r>>2]=0)}a=0;break A}}while(0);t[k>>2]=t[b>>2],a=1}}while(0);return 0|i[l+44>>0]?(Xn(0|t[l+4>>2],82796,0),G=l,0|(s=a)):(G=l,0|(s=a))}return 0}function NA(A,e,r,n,f,a,B,o,u,l,c){A|=0,e|=0,r|=0,n|=0,f|=0,B|=0,o|=0,u|=0,l|=0,c|=0;var E,Q,w=0,b=0,k=0;Q=G,G=G+16|0,E=0|t[(a|=0)>>2],t[Q>>2]=E;do{if(0|c){if(0|i[c>>0]){b=0|t[c+4>>2],t[Q+4>>2]=t[c+8>>2],t[Q+8>>2]=t[c+16>>2],t[Q+12>>2]=t[c+12>>2],t[Q>>2]=E+(0|t[c+20>>2]);break}if(w=0|tA(Q,Q+4|0,Q+12|0,Q+8|0,B),t[c+4>>2]=w,b=0|t[Q+4>>2],t[c+8>>2]=b,t[c+16>>2]=t[Q+8>>2],t[c+12>>2]=t[Q+12>>2],k=(0|t[Q>>2])-E|0,t[c+20>>2]=k,i[c>>0]=1,0==(129&w|0)&(k+b|0)>(0|B))return Xe(13,104,155,0,0),i[c>>0]=0,G=Q,0|(k=0);b=w}else b=0|tA(Q,Q+4|0,Q+12|0,Q+8|0,B)}while(0);if(128&b|0)return Xe(13,104,102,0,0),c?(i[c>>0]=0,G=Q,0|(k=0)):(G=Q,0|(k=0));do{if((0|o)>-1){if((0|t[Q+12>>2])==(0|o)&&(0|t[Q+8>>2])==(0|u)){if(!c)break;i[c>>0]=0;break}return l<<24>>24?(G=Q,0|(k=-1)):(0|c&&(i[c>>0]=0),Xe(13,104,168,0,0),G=Q,0|(k=0))}}while(0);return 0|(w=1&b)&&(t[Q+4>>2]=E+B-(0|t[Q>>2])),0|n&&(i[n>>0]=w),0|f&&(i[f>>0]=32&b),0|A&&(t[A>>2]=t[Q+4>>2]),0|r&&(i[r>>0]=t[Q+8>>2]),0|e&&(t[e>>2]=t[Q+12>>2]),t[a>>2]=t[Q>>2],G=Q,0|(k=1)}function FA(A,e,r,n,f){A|=0,r|=0,n|=0,f|=0;var a,B=0,o=0,u=0,l=0,c=0,E=0,Q=0,w=0;if(a=G,G=G+32|0,B=0|t[(e|=0)>>2],t[a>>2]=B,!(0!=(0|A)|0!=(1&n)))return t[e>>2]=B+r,G=a,0|(e=1);A:do{if((0|r)>0){e:do{if((0|f)>4){for(Q=B,u=B,E=r,o=0;;){if(1!=(0|E)&&0==(0|i[B>>0])&&0==(0|i[B+1>>0])){r=27;break e}if(t[a+4>>2]=B,128&(l=0|tA(a+4|0,a+8|0,a+16|0,a+12|0,E))?(Xe(13,104,102,0,0),c=B,B=u,r=Q,u=0,l=o,o=1):(1&l?(o=E+B-(r=0|t[a+4>>2])|0,t[a+8>>2]=o):(o=0|t[a+8>>2],r=0|t[a+4>>2]),t[a>>2]=r,c=r,B=r,u=32&l,l=o,o=0),o){r=36;break e}if(u<<24>>24)break;if(l){if(0|A){if(!(0|Gt(A,(B=0|t[A>>2])+l|0))){r=44;break e}If((0|t[A+4>>2])+B|0,0|c,0|l)}B=c+l|0,t[a>>2]=B,r=B}if((0|(E=Q+E-r|0))<=0){r=48;break A}Q=r,u=B,B=r,o=l}return Xe(13,106,197,0,0),G=a,0|(e=0)}for(E=B,w=B,l=0,c=0,Q=r,o=0;;){if(1!=(0|Q)&&0==(0|i[E>>0])&&0==(0|i[E+1>>0])){B=E,r=27;break e}if(t[a+4>>2]=E,128&(u=0|tA(a+4|0,a+8|0,a+16|0,a+12|0,Q))?(Xe(13,104,102,0,0),r=w,u=o,o=1):(1&u?(o=Q+E-(r=0|t[a+4>>2])|0,t[a+8>>2]=o):(r=0|t[a+4>>2],o=0|t[a+8>>2]),t[a>>2]=r,E=r,B=r,l=32&u,c=1&u,u=o,o=0),o){r=36;break e}if(l<<24>>24){if(!(0|FA(A,a,u,c,f+1|0))){B=0;break}r=B=0|t[a>>2]}else if(u){if(0|A){if(!(0|Gt(A,(B=0|t[A>>2])+u|0))){r=44;break e}If((0|t[A+4>>2])+B|0,0|E,0|u)}B=E+u|0,t[a>>2]=B,r=B}if((0|(Q=w+Q-r|0))<=0){r=48;break A}E=r,w=r,o=u}return G=a,0|B}while(0);if(27==(0|r)){if(B=B+2|0,t[a>>2]=B,1&n)break;return Xe(13,106,159,0,0),G=a,0|(e=0)}if(36==(0|r))return Xe(13,106,58,0,0),G=a,0|(e=0);if(44==(0|r))return Xe(13,140,65,0,0),G=a,0|(e=0)}else r=48}while(0);return 48==(0|r)&&1&n?(Xe(13,106,137,0,0),G=a,0|(e=0)):(t[e>>2]=B,G=a,0|(e=1))}function WA(A,e,r,n,f,a,B){A|=0,e|=0,r|=0,n|=0,f|=0,a|=0,B|=0;var o,u,l=0,c=0,E=0;if(u=G,G=G+32|0,t[u+8>>2]=r,!A)return G=u,0|(e=0);o=0|t[n>>2],t[u+4>>2]=t[e>>2],4096&o?(t[u+12>>2]=A,c=u+12|0):c=A;A:do{if(6&o){switch(8&o?(l=192&o,A=0|t[n+4>>2]):(l=0,A=o>>>1&1|16),0|(A=0|NA(u+8|0,0,0,u+16|0,0,u+4|0,r,A,l,f,a))){case 0:Xe(13,131,58,0,0),A=0;break;case-1:break;default:if(A=0|t[c>>2]){if((0|Hn(A))>0)do{t[u>>2]=0|Vn(A),O(u,0|t[n+16>>2])}while((0|Hn(A))>0);A=0|t[c>>2]}else A=0|Wn(),t[c>>2]=A;do{if(A){A=0|t[u+8>>2];e:do{if((0|A)>0){r:for(;;){l=0|t[u+4>>2];do{if(1!=(0|A)&&0==(0|i[l>>0])&&0==(0|i[l+1>>0])){if(t[u+4>>2]=l+2,0|i[u+16>>0]){t[u+8>>2]=A+l-(l+2),i[u+16>>0]=0,A=6;break}Xe(13,131,159,0,0),A=4;break}E=26}while(0);do{if(26==(0|E)){if(E=0,t[u>>2]=0,!(0|UA(u,u+4|0,A,0|t[n+16>>2],-1,0,0,a,B))){Xe(13,131,58,0,0),z(0|t[u>>2],0|t[n+16>>2]),A=4;break}t[u+8>>2]=l-(0|t[u+4>>2])+(0|t[u+8>>2]),0|yn(0|t[c>>2],0|t[u>>2])?A=0:(Xe(13,131,65,0,0),z(0|t[u>>2],0|t[n+16>>2]),A=4)}}while(0);switch(7&A){case 6:break e;case 0:break;default:break r}if((0|(A=0|t[u+8>>2]))<=0)break e}if(!A)break A;return G=u,0|(A=0)}}while(0);if(0|i[u+16>>0]){Xe(13,131,137,0,0);break}break A}Xe(13,131,65,0,0)}while(0);return G=u,0|(e=0)}return G=u,0|(e=A)}if(A=0|t[n+16>>2],8&o){switch(0|(A=0|UA(c,u+4|0,r,A,0|t[n+4>>2],192&o,f,a,B))){case 0:break;case-1:return G=u,0|(e=A);default:break A}return Xe(13,131,58,0,0),G=u,0|(e=0)}switch(0|(A=0|UA(c,u+4|0,r,A,-1,0,f,a,B))){case 0:break;case-1:return G=u,0|(e=A);default:break A}return Xe(13,131,58,0,0),G=u,0|(e=0)}while(0);return t[e>>2]=t[u+4>>2],G=u,0|(e=1)}function YA(){return 0|pA(52132)}function RA(A){z(A|=0,52132)}function yA(A,e,r,i){e|=0,i|=0;var n=0;return(A|=0)?-1!=(0|(r|=0))&&0==(0|t[A+4>>2])&&(n=0|iA(),t[A+4>>2]=n,0==(0|n))?0|(n=0):($(0|t[A>>2]),t[A>>2]=e,r?(e=0|t[A+4>>2],-1==(0|r)?(nA(e),t[A+4>>2]=0,0|(n=1)):(L(e,r,i),0|(n=1))):0|(n=1)):0|(i=0)}function VA(A,e){A|=0;var r,i,n=0;if(r=(i=0==(8&t[(e|=0)+12>>2]|0))?5:-1,e=0|Sr(0|Si(e)),A&&(!i||0!=(0|t[A+4>>2])||(n=0|iA(),t[A+4>>2]=n,0!=(0|n))))return $(0|t[A>>2]),t[A>>2]=e,e=0|t[A+4>>2],i?void L(e,r,0):(nA(e),void(t[A+4>>2]=0))}function mA(A,e,r,i){A|=0,e|=0,r|=0,i|=0;var n,f=0;return n=G,G=G+16|0,t[n>>2]=A,e?0|t[e>>2]?(e=0|HA(n,e,r,-1,i),G=n,0|e):((0|(A=0|HA(n,0,r,-1,i)))>=1&&((f=0|Ln(A,82796,0))?(t[n+4>>2]=f,HA(n,n+4|0,r,-1,i),t[e>>2]=f):A=-1),G=n,0|(e=A)):(e=0|HA(n,0,r,-1,i),G=n,0|e)}function HA(A,e,r,n,f){A|=0,e|=0,n|=0,f|=0;var a,B=0,o=0,u=0,l=0,c=0,E=0,Q=0;a=G,G=G+16|0,B=0|t[(r|=0)+16>>2],(o=0|i[r>>0])<<24>>24!=0&&0==(0|t[A>>2])?B=0:u=3;A:do{if(3==(0|u)){switch(Q=B?0|t[B+16>>2]:0,o<<24>>24|0){case 0:if(B=0|t[r+8>>2]){B=0|vA(A,e,B,n,f);break A}B=0|JA(A,e,r,n,f);break A;case 5:B=0|JA(A,e,r,-1,f);break A;case 2:if((o=0!=(0|Q))&&0==(0|MB[31&Q](6,A,r,0))){B=0;break A}if((0|(B=0|qA(A,r)))>-1&&(0|B)<(0|t[r+12>>2])){B=0|vA(0|ne(A,B=(0|t[r+8>>2])+(20*B|0)|0),e,B,-1,f);break A}if(!o){B=0;break A}MB[31&Q](7,A,r,0),B=0;break A;case 4:B=0|hB[15&t[B+20>>2]](A,e,r,n,f);break A;case 6:E=1+(f>>>11&1)|0;break;case 1:E=o<<24>>24;break;default:B=0;break A}if((0|(B=0|ie(a,e,A,r)))>=0){if(0|B){B=0|t[a>>2];break}if(t[a>>2]=0,c=-1==(0|n)?-193&f:f,f=-1==(0|n)?16:n,(l=0!=(0|Q))&&0==(0|MB[31&Q](6,A,r,0))){B=0;break}if((0|t[r+12>>2])>0)for(u=0,n=0|t[r+8>>2];;){if(!(B=0|te(A,n,1))){B=0;break A}if(-1==(0|(o=0|vA(0|ne(A,B),0,B,-1,c)))){B=-1;break A}if((0|o)>(2147483647-(B=0|t[a>>2])|0)){B=-1;break A}if(t[a>>2]=B+o,(0|(u=u+1|0))>=(0|t[r+12>>2])){B=B+o|0;break}n=n+20|0}else B=0|t[a>>2];if(!(0==(0|e)|-1==(0|(B=0|BA(E,B,f))))){if(fA(e,E,0|t[a>>2],f,c),(0|t[r+12>>2])>0)for(u=0,n=0|t[r+8>>2];;){if(!(o=0|te(A,n,1))){B=0;break A}if(vA(0|ne(A,o),e,o,-1,c),(0|(u=u+1|0))>=(0|t[r+12>>2]))break;n=n+20|0}if(2==(0|E)&&aA(e),l&&0==(0|MB[31&Q](7,A,r,0))){B=0;break}}}else B=0}}while(0);return G=a,0|B}function vA(A,e,r,i,n){A|=0,e|=0,i|=0,n|=0;var f,a,B,o=0,u=0,l=0,c=0,E=0,Q=0;B=G,G=G+16|0,4096&(a=0|t[(r|=0)>>2])&&(t[B+4>>2]=A,A=B+4|0);do{if(24&a){if(-1!=(0|i))return G=B,0|(e=-1);E=192&a,c=0|t[r+4>>2];break}E=-1!=(0|i)?192&n:0,c=i}while(0);if(!(6&a))return i=0|t[r+16>>2],16&a?(o=0|HA(A,0,i,-1,-193&n))?0!=(0|e)&-1!=(0|(i=0|BA(1+((a&n)>>>11&1)|0,o,c)))?(fA(e,1+((a&n)>>>11&1)|0,o,c,E),HA(A,e,0|t[r+16>>2],-1,-193&n),2!=(1+((a&n)>>>11&1)|0)?(G=B,0|(e=i)):(aA(e),G=B,0|(e=i))):(G=B,0|(e=i)):(G=B,0|(e=0)):(e=0|HA(A,e,i,c,E|-193&n),G=B,0|e);f=0|t[A>>2];A:do{if(f){if(l=(u=0==(16&a|0)&-1!=(0|c))?c:(2&a)>>>1|16,u=u?E:0,(0|Hn(f))>0)for(o=0,A=0;;){if(t[B>>2]=0|vn(f,o),-1==(0|(i=0|HA(B,0,0|t[r+16>>2],-1,-193&n)))|(0|A)>(2147483647-i|0)){A=-1;break A}if(A=i+A|0,(0|(o=o+1|0))>=(0|Hn(f))){o=A;break}}else o=0;if(-1!=(0|(i=0|BA(1+((a&n)>>>11&1)|0,o,l)))){if(!(0==(0|e)|-1==(0|(A=16&a|0?0|BA(1+((a&n)>>>11&1)|0,i,c):i)))){16&a|0&&fA(e,1+((a&n)>>>11&1)|0,i,c,E),fA(e,1+((a&n)>>>11&1)|0,o,l,u),E=0|t[r+16>>2],t[B+8>>2]=0;do{if(0!=(2&a|0)&&(0|Hn(f))>=2){if(0|(c=0|Ln(12*(0|Hn(f))|0,82796,0))){if(!(l=0|Ln(o,82796,0))){Xn(c,82796,0);break}if(t[B+8>>2]=l,(0|Hn(f))>0)for(i=0,o=c;;){if(t[B+12>>2]=0|vn(f,i),t[o>>2]=t[B+8>>2],t[o+4>>2]=0|HA(B+12|0,B+8|0,E,-1,-193&n),t[o+8>>2]=t[B+12>>2],(0|(i=i+1|0))>=(0|Hn(f))){i=B+8|0;break}o=o+12|0}else i=B+8|0;if(pt(c,0|Hn(f),12,27),t[i>>2]=t[e>>2],r=(0|Hn(f))>0,i=0|t[B+8>>2],r)for(o=0,u=c;r=u+4|0,If(0|i,0|t[u>>2],0|t[r>>2]),t[B+8>>2]=(0|t[B+8>>2])+(0|t[r>>2]),r=(0|(o=o+1|0))<(0|Hn(f)),i=0|t[B+8>>2],r;)u=u+12|0;if(t[e>>2]=i,4&a|0&&(0|Hn(f))>0)for(i=0,o=c;Jn(f,i,0|t[o+8>>2]),!((0|(i=i+1|0))>=(0|Hn(f)));)o=o+12|0;Xn(c,82796,0),Xn(l,82796,0)}}else Q=24}while(0);if(24==(0|Q)&&(0|Hn(f))>0){i=0;do{t[B+12>>2]=0|vn(f,i),HA(B+12|0,e,E,-1,-193&n),i=i+1|0}while((0|i)<(0|Hn(f)))}2==(1+((a&n)>>>11&1)|0)&&(aA(e),16&a|0)&&aA(e)}}else A=-1}else A=0}while(0);return G=B,0|(e=A)}function JA(A,e,r,i,n){A|=0,e|=0,r|=0,i|=0,n|=0;var f,a=0,B=0,o=0,u=0;switch(f=G,G=G+16|0,t[f>>2]=t[r+4>>2],a=0|XA(A,0,f,r),0|(B=0|t[f>>2])){case 16:case 17:case-3:u=0;break;default:u=1}switch(0|a){case-2:a=0,o=2;break;case-1:return G=f,0|(u=0);default:o=0}B=-1==(0|i)?B:i;do{if(0|e){if(0|u&&fA(e,o,a,B,n),XA(A,0|t[e>>2],f,r),o){aA(e);break}t[e>>2]=(0|t[e>>2])+a;break}}while(0);return u?(u=0|BA(o,a,B),G=f,0|u):(G=f,0|(u=a))}function XA(A,e,r,n){A|=0,e|=0,r|=0,n|=0;var f,a=0,B=0,o=0;if(f=G,G=G+16|0,t[f>>2]=e,0|(a=0|t[n+16>>2])&&0|(B=0|t[a+24>>2]))return e=0|MB[31&B](A,e,r,n),G=f,0|e;o=(a=0|i[n>>0])<<24>>24==0&&1==(0|t[n+4>>2])?11:6;do{if(6==(0|o)){if(!(B=0|t[A>>2]))return G=f,0|(e=-1);if(a<<24>>24==5){a=0|t[B+4>>2],t[r>>2]=a,B=A;break}-4==(0|t[n+4>>2])?(B=0|t[A>>2],a=0|t[B>>2],t[r>>2]=a,B=B+4|0):o=11}}while(0);switch(11==(0|o)&&(B=A,a=0|t[r>>2]),0|a){case 6:if(B=0|t[B>>2],0==(0|(a=0|t[B+16>>2]))|0==(0|(B=0|t[B+12>>2])))return G=f,0|(e=-1);break;case 1:if(-1==(0|(B=0|t[B>>2])))return G=f,0|(e=-1);do{if(-4!=(0|t[n+4>>2])){if(a=0|t[n+20>>2],B){if(!((0|a)>0))break;return G=f,0|(a=-1)}if(a)break;return G=f,0|(a=-1)}}while(0);i[f+4>>0]=B,a=f+4|0,B=1;break;case 3:return e=0|LA(0|t[B>>2],0==(0|e)?0:f),G=f,0|e;case 10:case 2:return e=0|wA(0|t[B>>2],0==(0|e)?0:f),G=f,0|e;case 5:return G=f,0|(e=0);default:if(B=0|t[B>>2],2048==(0|t[n+20>>2])&&16&t[B+12>>2]|0)return e?(t[B+8>>2]=e,t[B>>2]=0,G=f,0|(e=-2)):(G=f,0|(e=-2));a=0|t[B+8>>2],B=0|t[B>>2]}return 0!=(0|e)&0!=(0|B)?(If(0|e,0|a,0|B),G=f,0|(e=B)):(G=f,0|(e=B))}function TA(A,e,r){return 0|mA(A|=0,e|=0,r|=0,0)}function LA(A,e){e|=0;var r=0,n=0,a=0,B=0;if(!(A|=0))return 0|(e=0);n=0|t[A>>2];do{if((0|n)>0){if(8&(r=0|t[A+12>>2])|0){B=7&r;break}for(a=0|t[A+8>>2],r=n;;){if(0|i[a+(n=r+-1|0)>>0]){n=r;break}if(!((0|r)>1))break;r=n}B=1&(r=0|f[a+(n+-1)>>0])?0:2&r?1:4&r?2:8&r?3:16&r?4:32&r?5:0==(64&r|0)?0==(128&r|0)?0:7:6}else B=0}while(0);return a=n+1|0,e?(r=0|t[e>>2],i[r>>0]=B,(0|n)>0?(If(r+1|0,0|t[A+8>>2],0|n),i[(r=r+1+n|0)+-1>>0]=255<<B&f[r+-1>>0]):r=r+1|0,t[e>>2]=r,0|(e=a)):0|(e=a)}function SA(A,e,r){A|=0,e|=0,r|=0;var n=0,a=0,B=0,o=0;do{if((0|r)>=1){if((0==(0|A)||0==(0|(n=0|t[A>>2])))&&!(n=0|EA(3)))return 0|(A=0);if(B=0|t[e>>2],!((255&(o=0|i[B>>0]))>7)){if(t[(a=n+12|0)>>2]=-16&t[a>>2]|255&o|8,(0|r)>1){if(!(a=0|Ln(r+-1|0,82796,0))){a=65;break}If(0|a,B+1|0,r+-1|0),i[a+(r+-2)>>0]=255<<(255&o)&(0|f[a+(r+-2)>>0]),B=B+r|0}else B=B+1|0,a=0;return t[n>>2]=r+-1,Xn(0|t[(r=n+8|0)>>2],82796,0),t[r>>2]=a,t[n+4>>2]=3,0|A&&(t[A>>2]=n),t[e>>2]=B,0|(A=n)}a=220}else a=152,n=0}while(0);return Xe(13,189,a,0,0),0|A&&(0|t[A>>2])==(0|n)||function(A){cA(A|=0)}(n),0|(A=0)}function pA(A){A|=0;var e;return e=G,G=G+16|0,t[e>>2]=0,A=(0|zA(e,A,0))>0,G=e,0|(A?0|t[e>>2]:0)}function zA(A,e,r){A|=0,r|=0;var n,f=0,a=0,B=0,o=0,u=0,l=0,c=0,E=0,Q=0;n=G,G=G+16|0,0!=(0|(a=0|t[(e|=0)+16>>2]))&&0!=(0|(f=0|t[a+16>>2]))||(f=0);A:do{switch(0|i[e>>0]){case 4:if(!a)return G=n,0|(e=1);if(!(f=0|t[a+4>>2]))return G=n,0|(e=1);if(0|gB[63&f](A,e))return G=n,0|(e=1);Q=56;break;case 0:if(!(B=0|t[e+8>>2])){if(!(0|jA(A,e,r))){Q=56;break A}return G=n,0|(f=1)}f=0|t[B+16>>2],4096&(a=0|t[B>>2])&&(t[n>>2]=A,A=n);do{if(!(1&a)){if(768&a|0){t[A>>2]=0;break}if(!(6&a)){if(!(0|zA(A,f,4096&a))){Q=56;break A}return G=n,0|(f=1)}if(f=0|Wn()){t[A>>2]=f;break}Xe(13,133,65,0,0),Q=56;break A}KA(A,B)}while(0);return G=n,0|(e=1);case 5:if(0|jA(A,e,r))return G=n,0|(e=1);Q=56;break;case 2:a=0!=(0|f);e:do{if(a){switch(0|MB[31&f](0,A,e,0)){case 0:break A;case 2:f=1;break;default:break e}return G=n,0|f}}while(0);if(r)Df(0|t[A>>2],0,0|t[e+20>>2]);else if(Q=0|Sn(0|t[e+20>>2],82796,0),t[A>>2]=Q,!Q){Q=56;break A}if($A(A,-1,e),!a)return G=n,0|(e=1);if(0|MB[31&f](1,A,e,0))return G=n,0|(e=1);Q=57;break;case 1:case 6:c=0!=(0|f);e:do{if(c){switch(0|MB[31&f](0,A,e,0)){case 0:break A;case 2:f=1;break;default:break e}return G=n,0|f}}while(0);if(0|r)Df(0|t[A>>2],0,0|t[e+20>>2]);else if(E=0|Sn(0|t[e+20>>2],82796,0),t[A>>2]=E,!E){Q=56;break A}if((0|Ae(A,0,e))<0){if(0|r){Q=56;break A}Xn(0|t[A>>2],82796,0),t[A>>2]=0,Q=56;break A}ee(A,e);e:do{if((0|t[e+12>>2])>0){l=0,E=0|t[e+8>>2];r:for(;;){a=0|ne(A,E),B=0|t[E+16>>2],4096&(o=0|t[E>>2])?(t[n>>2]=a,u=n):u=a;do{if(1&o)KA(u,E),Q=50;else{if(768&o|0){t[u>>2]=0,Q=50;break}if(!(6&o)){if(0|zA(u,B,4096&o))break;break r}if(!(a=0|Wn())){Q=48;break r}t[u>>2]=a,Q=50}}while(0);if(50==(0|Q)&&(Q=0),(0|(l=l+1|0))>=(0|t[e+12>>2]))break e;E=E+20|0}48==(0|Q)&&Xe(13,133,65,0,0),K(A,e,r),Q=56;break A}}while(0);if(!c)return G=n,0|(e=1);if(0|MB[31&f](1,A,e,0))return G=n,0|(e=1);Q=57;break;default:return G=n,0|(e=1)}}while(0);return 56==(0|Q)?(Xe(13,121,65,0,0),G=n,0|(e=0)):(57==(0|Q)&&K(A,e,r),Xe(13,121,100,0,0),G=n,0|(e=0))}function KA(A,e){A|=0,e|=0;var r=0,n=0,f=0;A:do{if(!(774&t[e>>2])){e:for(;;){switch(f=0|t[e+16>>2],(r=0|i[f>>0])<<24>>24|0){case 4:e=6;break e;case 5:e=19;break e;case 6:case 1:case 2:e=28;break e;case 0:break;default:e=29;break e}if(!(e=0|t[f+8>>2])){e=11;break}if(774&t[e>>2]|0)break A}if(6==(0|e))return 0|(e=0|t[f+16>>2])&&0|(n=0|t[e+12>>2])?void FB[7&n](A,f):void(t[A>>2]=0);if(11==(0|e))return 0|(e=0|t[f+16>>2])?(e=0|t[e+16>>2])?void FB[7&e](A,f):void(t[A>>2]=0):r<<24>>24!=5&&1==(0|t[f+4>>2])?void(t[A>>2]=t[f+20>>2]):void(t[A>>2]=0);if(19==(0|e)){do{if(0|f){if(!(e=0|t[f+16>>2])){if(r<<24>>24==5)break;if(1!=(0|t[f+4>>2]))break;return void(t[A>>2]=t[f+20>>2])}return(e=0|t[e+16>>2])?void FB[7&e](A,f):void(t[A>>2]=0)}}while(0);return void(t[A>>2]=0)}if(28==(0|e))return void(t[A>>2]=0);if(29==(0|e))return}}while(0);t[A>>2]=0}function jA(A,e,r){A|=0,r|=0;var n=0,f=0;if(!(e|=0))return 0|(A=0);n=0|t[e+16>>2];do{if(0|n){if(r){if(!(n=0|t[n+16>>2]))break;return FB[7&n](A,e),0|(A=1)}if(!(n=0|t[n+8>>2]))break;return 0|(A=0|gB[63&n](A,e))}}while(0);A:do{if(5==(0|i[e>>0]))n=-1,f=15;else switch(0|(n=0|t[e+4>>2])){case 6:return t[A>>2]=0|Sr(0),0|(A=1);case 1:return t[A>>2]=t[e+20>>2],0|(A=1);case 5:return t[A>>2]=1,0|(A=1);case-4:if(n=0|Ln(8,82796,0)){t[n+4>>2]=0,t[n>>2]=-1,t[A>>2]=n;break A}return 0|(A=0);default:f=15;break A}}while(0);return 15==(0|f)&&(r?(f=0|t[A>>2],t[f>>2]=0,t[f+4>>2]=0,t[f+8>>2]=0,t[f+4>>2]=n,t[f+12>>2]=128,n=f):(n=0|EA(n),t[A>>2]=n),0!=(0|n)&5==(0|i[e>>0])&&(t[(f=n+12|0)>>2]=64|t[f>>2])),0|(A=0!=(0|t[A>>2])&1)}function xA(A,e){return 0|zA(A|=0,e|=0,0)}function OA(A,e,r,i,n,f,a){return A|=0,e|=0,i|=0,n|=0,f|=0,a|=0,(0|(r|=0))>-1&&0==(0|sA(0|t[A>>2],r))?0|(r=0):0|yA(0|t[A+4>>2],e,i,n)?f?(function(A,e,r){e|=0,r|=0,Xn(0|t[(A|=0)+8>>2],82796,0),t[A+8>>2]=e,t[A>>2]=r}(0|t[A+8>>2],f,a),0|(r=1)):0|(r=1):0|(r=0)}function PA(A,e,r,i,n){return e|=0,r|=0,i|=0,n|=0,0|(A|=0)&&(t[A>>2]=t[t[n+4>>2]>>2]),0|e&&(t[e>>2]=0|function(A){return 0|t[(A|=0)+8>>2]}(0|t[n+8>>2]),t[r>>2]=0|function(A){return 0|t[(A|=0)>>2]}(0|t[n+8>>2])),i?(t[i>>2]=t[n+4>>2],1):1}function _A(A,e){A|=0;var r,n=0,a=0,B=0;if(r=G,G=G+16|0,!(e|=0))return G=r,0|(A=0);if(256&t[e+4>>2]){if(1!=(0|se(A,80624,1)))return G=r,0|(A=-1);n=1}else n=0;if(!(a=0|t[e>>2]))return A=2==(0|se(A,54573,2)),G=r,0|(A=A?2|n:-1);if((0|a)<=0)return G=r,0|(A=n);for(a=0;;){if(0!=(0|a)&0==(0|(a>>>0)%35)){if(2!=(0|se(A,54576,2))){n=-1,a=13;break}n=n+2|0}if(B=(0|t[e+8>>2])+a|0,i[r>>0]=0|i[1312+((0|f[B>>0])>>>4)>>0],i[r+1>>0]=0|i[1312+(15&i[B>>0])>>0],2!=(0|se(A,r,2))){n=-1,a=13;break}if(n=n+2|0,(0|(a=a+1|0))>=(0|t[e>>2])){a=13;break}}return 13==(0|a)?(G=r,0|n):0}function qA(A,e){return e|=0,0|t[(0|t[(A|=0)>>2])+(0|t[e+4>>2])>>2]}function $A(A,e,r){return e|=0,r|=0,r=(0|t[(A|=0)>>2])+(0|t[r+4>>2])|0,A=0|t[r>>2],t[r>>2]=e,0|A}function Ae(A,e,r){A|=0,e|=0,r|=0;var n,f=0;n=G,G=G+16|0;A:do{switch(0|i[r>>0]){case 6:case 1:if(0!=(0|(r=0|t[r+16>>2]))&&0!=(1&t[r+4>>2]|0)){if(A=(f=0|t[A>>2])+(0|t[r+8>>2])|0,f=f+(0|t[r+12>>2])|0,!e){if(t[A>>2]=1,e=0|W(),t[f>>2]=e,0|e){r=1;break A}Xe(13,233,65,0,0),r=-1;break A}0|T(A,e,n,0|t[f>>2])?(r=0|t[n>>2])||(V(0|t[f>>2]),t[f>>2]=0,r=0|t[n>>2]):r=-1}else r=0;break;default:r=0}}while(0);return G=n,0|r}function ee(A,e){e|=0;var r;(A|=0)&&(r=0|t[A>>2])&&(A=0|t[e+16>>2])&&2&t[A+4>>2]&&(A=r+(0|t[A+20>>2])|0)&&(t[A>>2]=0,t[A+4>>2]=0,t[A+8>>2]=1)}function re(A,e,r,i){e|=0,r|=0,i|=0;var n=0;return(A|=0)&&(n=0|t[A>>2])&&(A=0|t[i+16>>2])&&2&t[A+4>>2]&&(A=n+(0|t[A+20>>2])|0)?(Xn(0|t[A>>2],82796,0),n=0|Ln(r,82796,0),t[A>>2]=n,n?(If(0|n,0|e,0|r),t[A+4>>2]=r,t[A+8>>2]=0,0|(r=1)):0|(r=0)):0|(r=1)}function ie(A,e,r,i){A|=0,e|=0,i|=0;var n;return(r|=0)&&(n=0|t[r>>2])&&(r=0|t[i+16>>2])&&2&t[r+4>>2]&&(r=n+(0|t[r+20>>2])|0)?0|t[r+8>>2]?0|(e=0):(0|e&&(If(0|t[e>>2],0|t[r>>2],0|t[r+4>>2]),t[e>>2]=(0|t[e>>2])+(0|t[r+4>>2])),A?(t[A>>2]=t[r+4>>2],0|(e=1)):0|(e=1)):0|(e=0)}function ne(A,e){return e|=0,(0|t[(A|=0)>>2])+(0|t[e+8>>2])|0}function te(A,e,r){A|=0,r|=0;var i,n,f=0,a=0;if(n=G,G=G+16|0,!(768&(f=0|t[(e|=0)>>2])))return G=n,0|(r=e);if(i=0|t[e+16>>2],e=0|t[(0|t[A>>2])+(0|t[i+4>>2])>>2]){if(e=256&f?0|zr(e):0|CA(e),t[n>>2]=e,0|(e=0|t[i+8>>2])&&0==(0|IB[31&e](n)))return Xe(13,110,164,0,0),G=n,0|(r=0);e=0|t[i+16>>2];A:do{if((0|e)>0){for(A=0|t[n>>2],f=0|t[i+12>>2],a=0;(0|t[f>>2])!=(0|A);){if((0|(a=a+1|0))>=(0|e))break A;f=f+24|0}return G=n,0|(r=f+4|0)}}while(0);if(0|(e=0|t[i+20>>2]))return G=n,0|(r=e)}else if(0|(e=0|t[i+24>>2]))return G=n,0|(r=e);return r?(Xe(13,110,164,0,0),G=n,0|(r=0)):(G=n,0|(r=0))}function fe(A,e,r,n,f){e|=0,r|=0,n|=0,f|=0;var a,B=0,o=0,u=0;if(a=G,G=G+128|0,B=0|t[(A|=0)>>2],n){if((0|B)!=(0|n)&&0==(0|e)|(0|r)<0)return G=a,0|(f=0);t[A>>2]=n,B=n,n=1}else{if(!B)return G=a,0|(f=0);n=0}A:do{if(e){(0|(n=0|Li(B)))>=129&&oi(54600,82796,0);do{if((0|n)>=(0|r)){if(r>>>0>128)return G=a,0|(f=0);If(A+20|0,0|e,0|r),t[A+16>>2]=r;break}if(!(0|gi(0|t[A+4>>2],B,f)))break A;if(!(0|Di(0|t[A+4>>2],e,r)))break A;if(!(0|Mi(0|t[A+4>>2],A+20|0,A+16|0)))break A;r=0|t[A+16>>2]}while(0);128==(0|r)||Df(A+20+r|0,0,128-r|0),o=19}else n?o=19:(u=A+8|0,o=28)}while(0);if(19==(0|o)){r=0;do{i[a+r>>0]=54^i[A+20+r>>0],r=r+1|0}while(128!=(0|r));if(0|gi(0|t[A+8>>2],B,f)&&0|Di(e=0|t[A+8>>2],a,0|Li(B))){r=0;do{i[a+r>>0]=92^i[A+20+r>>0],r=r+1|0}while(128!=(0|r));0|gi(0|t[A+12>>2],B,f)&&0|Di(f=0|t[A+12>>2],a,0|Li(B))&&(u=A+8|0,o=28)}}return 28==(0|o)&&0|hi(0|t[A+4>>2],0|t[u>>2])?(G=a,0|(f=1)):(G=a,0|(f=0))}function ae(A,e,r){return e|=0,r|=0,0|t[(A|=0)>>2]?0|(r=0|Di(0|t[A+4>>2],e,r)):0|(r=0)}function Be(A,e,r){e|=0,r|=0;var i;return i=G,G=G+80|0,0|t[(A|=0)>>2]&&0|Mi(0|t[A+4>>2],i,i+64|0)&&0|hi(0|t[A+4>>2],0|t[A+12>>2])&&0|Di(0|t[A+4>>2],i,0|t[i+64>>2])&&0|Mi(0|t[A+4>>2],e,r)?(G=i,0|(e=1)):(G=i,0|(e=0))}function oe(){var A=0;return 0!=(0|(A=0|Sn(148,82796,0)))&&0==(0|ue(A))&&(le(A),A=0),0|A}function ue(A){var e=0,r=0;Ci(0|t[(A|=0)+8>>2]),Ci(0|t[A+12>>2]),Ci(0|t[A+4>>2]),t[A>>2]=0,t[A+16>>2]=0,$r(A+20|0,128),(0!=(0|t[A+8>>2])||(r=0|di(),t[A+8>>2]=r,0|r))&&(e=3);do{if(3==(0|e)){if(0==(0|t[A+12>>2])&&(r=0|di(),t[A+12>>2]=r,0==(0|r)))break;if(0==(0|t[A+4>>2])&&(r=0|di(),t[A+4>>2]=r,0==(0|r)))break;return t[A>>2]=0,0|(r=1)}}while(0);return Ci(0|t[A+8>>2]),Ci(0|t[A+12>>2]),Ci(0|t[A+4>>2]),t[A>>2]=0,t[A+16>>2]=0,$r(A+20|0,128),0|(r=0)}function le(A){(A|=0)&&(Ci(0|t[A+8>>2]),Ci(0|t[A+12>>2]),Ci(0|t[A+4>>2]),t[A>>2]=0,t[A+16>>2]=0,$r(A+20|0,128),Ii(0|t[A+8>>2]),Ii(0|t[A+12>>2]),Ii(0|t[A+4>>2]),Xn(A,82796,0))}function ce(A,e,r){e|=0;var n=0,f=0,a=0;if(!(r|=0))return 1;f=0|t[(A|=0)+16>>2],n=0|t[A+20>>2],(f+(r<<3)|0)>>>0<f>>>0&&(t[A+20>>2]=n+1,n=n+1|0),t[A+20>>2]=n+(r>>>29),t[A+16>>2]=f+(r<<3),a=0|t[A+88>>2];do{if(a){if((a+r|r)>>>0>63){If(A+24+a|0,0|e,64-a|0),Ee(A,A+24|0,1),f=(n=A+24|0)+68|0;do{i[n>>0]=0,n=n+1|0}while((0|n)<(0|f));n=e+(64-a)|0,r=r-(64-a)|0;break}return If(A+24+a|0,0|e,0|r),t[A+88>>2]=(0|t[A+88>>2])+r,1}n=e}while(0);return a=-64&r,(f=r>>>6)&&(Ee(A,n,f),n=n+a|0,r=r-a|0),r?(t[A+88>>2]=r,If(A+24|0,0|n,0|r),1):1}function Ee(A,e,r){A|=0,e|=0;var i=0,n=0,a=0,B=0,o=0,u=0,l=0,c=0,E=0,Q=0,w=0,b=0,k=0,s=0,C=0,d=0,I=0,g=0,D=0,M=0,h=0,U=0,G=0,Z=0,N=0;if(r|=0)for(i=0|t[A>>2],n=0|t[A+4>>2],a=0|t[A+8>>2],B=0|t[A+12>>2];r=r+-1|0,C=(0|f[e+1>>0])<<8|0|f[e>>0]|(0|f[e+2>>0])<<16|(0|f[e+3>>0])<<24,g=(0|f[e+5>>0])<<8|0|f[e+4>>0]|(0|f[e+6>>0])<<16|(0|f[e+7>>0])<<24,b=((b=i+-680876936+((a^B)&n^B)+C|0)<<7|b>>>25)+n|0,l=(0|f[e+9>>0])<<8|0|f[e+8>>0]|(0|f[e+10>>0])<<16|(0|f[e+11>>0])<<24,Z=B+-389564586+g+(b&(n^a)^a)|0,M=(0|f[e+13>>0])<<8|0|f[e+12>>0]|(0|f[e+14>>0])<<16|(0|f[e+15>>0])<<24,s=((s=a+606105819+l+((Z<<12|Z>>>20)+b&(b^n)^n)|0)<<17|s>>>15)+((Z<<12|Z>>>20)+b)|0,o=(0|f[e+17>>0])<<8|0|f[e+16>>0]|(0|f[e+18>>0])<<16|(0|f[e+19>>0])<<24,Q=n+-1044525330+M+(s&((Z<<12|Z>>>20)+b^b)^b)|0,U=(0|f[e+21>>0])<<8|0|f[e+20>>0]|(0|f[e+22>>0])<<16|(0|f[e+23>>0])<<24,d=((d=b+-176418897+o+((Q<<22|Q>>>10)+s&(s^(Z<<12|Z>>>20)+b)^(Z<<12|Z>>>20)+b)|0)<<7|d>>>25)+((Q<<22|Q>>>10)+s)|0,c=(0|f[e+25>>0])<<8|0|f[e+24>>0]|(0|f[e+26>>0])<<16|(0|f[e+27>>0])<<24,b=(Z<<12|Z>>>20)+b+1200080426+U+(d&((Q<<22|Q>>>10)+s^s)^s)|0,Z=(0|f[e+29>>0])<<8|0|f[e+28>>0]|(0|f[e+30>>0])<<16|(0|f[e+31>>0])<<24,I=((I=s+-1473231341+c+((b<<12|b>>>20)+d&(d^(Q<<22|Q>>>10)+s)^(Q<<22|Q>>>10)+s)|0)<<17|I>>>15)+((b<<12|b>>>20)+d)|0,u=(0|f[e+33>>0])<<8|0|f[e+32>>0]|(0|f[e+34>>0])<<16|(0|f[e+35>>0])<<24,s=(Q<<22|Q>>>10)+s+-45705983+Z+(I&((b<<12|b>>>20)+d^d)^d)|0,Q=(0|f[e+37>>0])<<8|0|f[e+36>>0]|(0|f[e+38>>0])<<16|(0|f[e+39>>0])<<24,N=((N=u+1770035416+d+((s<<22|s>>>10)+I&(I^(b<<12|b>>>20)+d)^(b<<12|b>>>20)+d)|0)<<7|N>>>25)+((s<<22|s>>>10)+I)|0,E=(0|f[e+41>>0])<<8|0|f[e+40>>0]|(0|f[e+42>>0])<<16|(0|f[e+43>>0])<<24,d=Q+-1958414417+((b<<12|b>>>20)+d)+(N&((s<<22|s>>>10)+I^I)^I)|0,b=(0|f[e+45>>0])<<8|0|f[e+44>>0]|(0|f[e+46>>0])<<16|(0|f[e+47>>0])<<24,G=((G=E+-42063+I+((d<<12|d>>>20)+N&(N^(s<<22|s>>>10)+I)^(s<<22|s>>>10)+I)|0)<<17|G>>>15)+((d<<12|d>>>20)+N)|0,w=(0|f[e+49>>0])<<8|0|f[e+48>>0]|(0|f[e+50>>0])<<16|(0|f[e+51>>0])<<24,I=b+-1990404162+((s<<22|s>>>10)+I)+(G&((d<<12|d>>>20)+N^N)^N)|0,N=(s=(0|f[e+53>>0])<<8|0|f[e+52>>0]|(0|f[e+54>>0])<<16|(0|f[e+55>>0])<<24)+-40341101+((d<<12|d>>>20)+N)+((h=((h=w+1804603682+N+((I<<22|I>>>10)+G&(G^(d<<12|d>>>20)+N)^(d<<12|d>>>20)+N)|0)<<7|h>>>25)+((I<<22|I>>>10)+G)|0)&((I<<22|I>>>10)+G^G)^G)|0,c=Q+-343485551+(((E=s+1309151649+(((k=g+-2054922799+(((I=U+-57434055+(((h=l+-995338651+(((N=c+76029189+(((D=E+-1094730640+(((G=(k=(0|f[e+57>>0])<<8|0|f[e+56>>0]|(0|f[e+58>>0])<<16|(0|f[e+59>>0])<<24)+-35309556+(((I=w+-1926607734+(((h=u+1163531501+(((N=o+-405537848+(((D=C+-373897302+(((G=(d=(0|f[e+61>>0])<<8|0|f[e+60>>0]|(0|f[e+62>>0])<<16|(0|f[e+63>>0])<<24)+1236535329+((I<<22|I>>>10)+G)+((D=((D=k+-1502002290+G+((N<<12|N>>>20)+h&(h^(I<<22|I>>>10)+G)^(I<<22|I>>>10)+G)|0)<<17|D>>>15)+((N<<12|N>>>20)+h)|0)&((N<<12|N>>>20)+h^h)^h)|0)<<22|G>>>10)+D)+(((N=((N=b+643717713+D+((((h=c+-1069501632+((N<<12|N>>>20)+h)+(((I=((I=g+-165796510+h+(((G<<22|G>>>10)+D^D)&(N<<12|N>>>20)+h^D)|0)<<5|I>>>27)+((G<<22|G>>>10)+D)|0)^(G<<22|G>>>10)+D)&D^(G<<22|G>>>10)+D)|0)<<9|h>>>23)+I^I)&(G<<22|G>>>10)+D^I)|0)<<14|N>>>18)+((h<<9|h>>>23)+I)|0)^(h<<9|h>>>23)+I)&I^(h<<9|h>>>23)+I)|0)<<20|D>>>12)+N)+(((h=((h=d+-660478335+N+((((I=E+38016083+((h<<9|h>>>23)+I)+(((G=((G=U+-701558691+I+(((D<<20|D>>>12)+N^N)&(h<<9|h>>>23)+I^N)|0)<<5|G>>>27)+((D<<20|D>>>12)+N)|0)^(D<<20|D>>>12)+N)&N^(D<<20|D>>>12)+N)|0)<<9|I>>>23)+G^G)&(D<<20|D>>>12)+N^G)|0)<<14|h>>>18)+((I<<9|I>>>23)+G)|0)^(I<<9|I>>>23)+G)&G^(I<<9|I>>>23)+G)|0)<<20|N>>>12)+h)+(((I=((I=M+-187363961+h+((((G=k+-1019803690+((I<<9|I>>>23)+G)+(((D=((D=Q+568446438+G+(((N<<20|N>>>12)+h^h)&(I<<9|I>>>23)+G^h)|0)<<5|D>>>27)+((N<<20|N>>>12)+h)|0)^(N<<20|N>>>12)+h)&h^(N<<20|N>>>12)+h)|0)<<9|G>>>23)+D^D)&(N<<20|N>>>12)+h^D)|0)<<14|I>>>18)+((G<<9|G>>>23)+D)|0)^(G<<9|G>>>23)+D)&D^(G<<9|G>>>23)+D)|0)<<20|h>>>12)+I)+(((G=((G=Z+1735328473+I+((((D=l+-51403784+((G<<9|G>>>23)+D)+(((N=((N=s+-1444681467+D+(((h<<20|h>>>12)+I^I)&(G<<9|G>>>23)+D^I)|0)<<5|N>>>27)+((h<<20|h>>>12)+I)|0)^(h<<20|h>>>12)+I)&I^(h<<20|h>>>12)+I)|0)<<9|D>>>23)+N^N)&(h<<20|h>>>12)+I^N)|0)<<14|G>>>18)+((D<<9|D>>>23)+N)|0)^(D<<9|D>>>23)+N)&N^(D<<9|D>>>23)+N)|0)<<20|I>>>12)+G)+(((N=u+-2022574463+((D<<9|D>>>23)+N)+((I<<20|I>>>12)+G^G^(h=((h=U+-378558+N+(G^(D<<9|D>>>23)+N^(I<<20|I>>>12)+G)|0)<<4|h>>>28)+((I<<20|I>>>12)+G)|0))|0)<<11|N>>>21)+h^h^(D=((D=b+1839030562+G+(h^(I<<20|I>>>12)+G^(N<<11|N>>>21)+h)|0)<<16|D>>>16)+((N<<11|N>>>21)+h)|0))|0)<<23|G>>>9)+D)+(((h=o+1272893353+((N<<11|N>>>21)+h)+((G<<23|G>>>9)+D^D^(I=((I=g+-1530992060+h+(D^(N<<11|N>>>21)+h^(G<<23|G>>>9)+D)|0)<<4|I>>>28)+((G<<23|G>>>9)+D)|0))|0)<<11|h>>>21)+I^I^(N=((N=Z+-155497632+D+(I^(G<<23|G>>>9)+D^(h<<11|h>>>21)+I)|0)<<16|N>>>16)+((h<<11|h>>>21)+I)|0))|0)<<23|D>>>9)+N)+(((I=C+-358537222+((h<<11|h>>>21)+I)+((D<<23|D>>>9)+N^N^(G=((G=s+681279174+I+(N^(h<<11|h>>>21)+I^(D<<23|D>>>9)+N)|0)<<4|G>>>28)+((D<<23|D>>>9)+N)|0))|0)<<11|I>>>21)+G^G^(h=((h=M+-722521979+N+(G^(D<<23|D>>>9)+N^(I<<11|I>>>21)+G)|0)<<16|h>>>16)+((I<<11|I>>>21)+G)|0))|0)<<23|N>>>9)+h)+(((G=w+-421815835+((I<<11|I>>>21)+G)+((N<<23|N>>>9)+h^h^(D=((D=Q+-640364487+G+(h^(I<<11|I>>>21)+G^(N<<23|N>>>9)+h)|0)<<4|D>>>28)+((N<<23|N>>>9)+h)|0))|0)<<11|G>>>21)+D^D^(I=((I=d+530742520+h+(D^(N<<23|N>>>9)+h^(G<<11|G>>>21)+D)|0)<<16|I>>>16)+((G<<11|G>>>21)+D)|0))|0)<<23|h>>>9)+I)+(((k=((k=k+-1416354905+I+((((D=Z+1126891415+((G<<11|G>>>21)+D)+(((C=((C=C+-198630844+D+(((h<<23|h>>>9)+I|~((G<<11|G>>>21)+D))^I)|0)<<6|C>>>26)+((h<<23|h>>>9)+I)|0)|~I)^(h<<23|h>>>9)+I)|0)<<10|D>>>22)+C|~((h<<23|h>>>9)+I))^C)|0)<<15|k>>>17)+((D<<10|D>>>22)+C)|0)|~C)^(D<<10|D>>>22)+C)|0)<<21|I>>>11)+k)+(((E=((E=E+-1051523+k+((((C=M+-1894986606+((D<<10|D>>>22)+C)+(((w=((w=w+1700485571+C+(((I<<21|I>>>11)+k|~((D<<10|D>>>22)+C))^k)|0)<<6|w>>>26)+((I<<21|I>>>11)+k)|0)|~k)^(I<<21|I>>>11)+k)|0)<<10|C>>>22)+w|~((I<<21|I>>>11)+k))^w)|0)<<15|E>>>17)+((C<<10|C>>>22)+w)|0)|~w)^(C<<10|C>>>22)+w)|0)<<21|k>>>11)+E)+(((c=((c=c+-1560198380+E+((((w=d+-30611744+((C<<10|C>>>22)+w)+(((u=((u=u+1873313359+w+(((k<<21|k>>>11)+E|~((C<<10|C>>>22)+w))^E)|0)<<6|u>>>26)+((k<<21|k>>>11)+E)|0)|~E)^(k<<21|k>>>11)+E)|0)<<10|w>>>22)+u|~((k<<21|k>>>11)+E))^u)|0)<<15|c>>>17)+((w<<10|w>>>22)+u)|0)|~u)^(w<<10|w>>>22)+u)|0)<<21|E>>>11)+c)+(((l=((l=l+718787259+c+((((u=b+-1120210379+((w<<10|w>>>22)+u)+(((o=((o=o+-145523070+u+(((E<<21|E>>>11)+c|~((w<<10|w>>>22)+u))^c)|0)<<6|o>>>26)+((E<<21|E>>>11)+c)|0)|~c)^(E<<21|E>>>11)+c)|0)<<10|u>>>22)+o|~((E<<21|E>>>11)+c))^o)|0)<<15|l>>>17)+((u<<10|u>>>22)+o)|0)|~o)^(u<<10|u>>>22)+o)|0,i=o+(0|t[A>>2])|0,t[A>>2]=i,n=l+(0|t[A+4>>2])+(c<<21|c>>>11)|0,t[A+4>>2]=n,a=l+(0|t[A+8>>2])|0,t[A+8>>2]=a,B=(u<<10|u>>>22)+o+(0|t[A+12>>2])|0,t[A+12>>2]=B,r;)e=e+64|0}function Qe(A,e){A|=0;var r=0;return r=0|t[(e|=0)+88>>2],i[e+24+r>>0]=-128,(r+1|0)>>>0>56?(Df(e+24+(r+1)|0,0,63-r|0),Ee(e,e+24|0,1),r=0):r=r+1|0,Df(e+24+r|0,0,56-r|0),r=0|t[e+16>>2],i[e+80>>0]=r,i[e+80+1>>0]=r>>>8,i[e+80+2>>0]=r>>>16,i[e+80+3>>0]=r>>>24,r=0|t[e+20>>2],i[e+84>>0]=r,i[e+84+1>>0]=r>>>8,i[e+84+2>>0]=r>>>16,i[e+84+3>>0]=r>>>24,Ee(e,e+24|0,1),t[e+88>>2]=0,$r(e+24|0,64),r=0|t[e>>2],i[A>>0]=r,i[A+1>>0]=r>>>8,i[A+2>>0]=r>>>16,i[A+3>>0]=r>>>24,r=0|t[e+4>>2],i[A+4>>0]=r,i[A+5>>0]=r>>>8,i[A+6>>0]=r>>>16,i[A+7>>0]=r>>>24,r=0|t[e+8>>2],i[A+8>>0]=r,i[A+9>>0]=r>>>8,i[A+10>>0]=r>>>16,i[A+11>>0]=r>>>24,e=0|t[e+12>>2],i[A+12>>0]=e,i[A+13>>0]=e>>>8,i[A+14>>0]=e>>>16,i[A+15>>0]=e>>>24,1}function we(A,e,r,n,a){A|=0,e|=0,r|=0,n|=0;var B,u,l,c=0,E=0,Q=0,w=0;if(l=G,G=G+512|0,c=(0|(E=(0|(a|=0))>0?a:0))<128?E:128,(0|a)>=1?(Df(0|l,32,0|c),i[l+c>>0]=0,(0|c)<6?(c=E,Q=4):a=6):(i[l>>0]=0,c=0,Q=4),4==(0|Q)&&(a=c),(0|(B=((0|n)/(0|(u=16-((c+3-a|0)/4|0)|0))|0)+((0|o(u,(0|n)/(0|u)|0))<(0|n)&1)|0))<=0)return G=l,0|(e=0);if((0|u)<=0){a=0,c=0;do{dt(l+176|0,l,289),t[l+496>>2]=0|o(a,u),Ge(l+144|0,20,54665,l+496|0),It(l+176|0,l+144|0,289),It(l+176|0,54687,289),It(l+176|0,80479,289),r=0|mt(l+176|0),c=(0|DB[31&A](l+176|0,r,e))+c|0,a=a+1|0}while((0|a)!=(0|B));return G=l,0|c}Q=0,c=0;do{dt(l+176|0,l,289),E=0|o(Q,u),t[l+472>>2]=E,Ge(l+144|0,20,54665,l+472|0),It(l+176|0,l+144|0,289),a=0;do{(a+E|0)<(0|n)?(t[l+488>>2]=f[r+E+a>>0],t[l+488+4>>2]=7==(0|a)?45:32,Ge(l+144|0,20,54680,l+488|0),It(l+176|0,l+144|0,289)):It(l+176|0,54676,289),a=a+1|0}while((0|a)!=(0|u));if(It(l+176|0,54687,289),(0|E)<(0|n)){a=0;do{w=0|i[r+E+a>>0],t[l+480>>2]=(w+-32&255)<95?255&w:46,Ge(l+144|0,20,54673,l+480|0),It(l+176|0,l+144|0,289),a=a+1|0}while((0|a)<(0|u)&(a+E|0)<(0|n))}It(l+176|0,80479,289),w=0|mt(l+176|0),c=(0|DB[31&A](l+176|0,w,e))+c|0,Q=Q+1|0}while((0|Q)!=(0|B));return G=l,0|c}function be(A,e,r){return 0|we(16,A|=0,e|=0,r|=0,0)}function ke(A){var e,r=0,i=0,n=0;return e=G,G=G+16|0,(A|=0)?(0|T(A+44|0,-1,e,0|t[A+68>>2]))<1?(G=e,0|(A=0)):(0|t[e>>2])>0?(G=e,0|(A=1)):0|(r=0|t[A+4>>2])&&(i=0|UB[7&r](A,1,0,0,0,1),t[e>>2]=i,(0|i)<1)?(G=e,0|(A=i)):(0|(r=0|t[A>>2])&&0|(n=0|t[r+32>>2])&&IB[31&n](A),bt(12,A,A+64|0),V(0|t[A+68>>2]),Xn(A,82796,0),G=e,0|(A=1)):(G=e,0|(A=0))}function se(A,e,r){e|=0,r|=0;var i,n=0,f=0,a=0;return(A|=0)?(i=0|t[A+4>>2],0|(f=0|t[A>>2])&&0|t[f+8>>2]?0|i&&(0|(n=0|UB[7&i](A,3,e,r,0,1)))<1?0|(f=n):0|t[A+12>>2]?((0|(n=0|DB[31&t[8+(0|t[A>>2])>>2]](A,e,r)))>0&&(a=0|cf(0|t[A+56>>2],0|t[A+56+4>>2],0|n,((0|n)<0)<<31>>31|0),f=0|E(),t[A+56>>2]=a,t[A+56+4>>2]=f),i?0|(a=0|UB[7&i](A,131,e,r,0,n)):0|(a=n)):(Xe(32,113,120,0,0),0|(f=-2)):(Xe(32,113,121,0,0),0|(a=-2))):0|(r=0)}function Ce(A,e){e|=0;var r=0,i=0,n=0,f=0;return 0|(A|=0)&&0|(r=0|t[A>>2])&&0|t[r+16>>2]?0|(i=0|t[A+4>>2])&&(0|(n=0|UB[7&i](A,4,e,0,0,1)))<1?0|(e=n):0|t[A+12>>2]?((0|(r=0|gB[63&t[16+(0|t[A>>2])>>2]](A,e)))>0&&(f=0|cf(0|t[A+56>>2],0|t[A+56+4>>2],0|r,((0|r)<0)<<31>>31|0),n=0|E(),t[A+56>>2]=f,t[A+56+4>>2]=n),i?0|(f=0|UB[7&i](A,132,e,0,0,r)):0|(f=r)):(Xe(32,110,120,0,0),0|(e=-2)):(Xe(32,110,121,0,0),0|(f=-2))}function de(A,e,r){e|=0,r|=0;var i=0,n=0,f=0;return 0|(A|=0)&&0|(i=0|t[A>>2])&&0|t[i+20>>2]?0|(n=0|t[A+4>>2])&&(0|(f=0|UB[7&n](A,5,e,r,0,1)))<1?0|(r=f):0|t[A+12>>2]?(i=0|DB[31&t[20+(0|t[A>>2])>>2]](A,e,r),n?0|(r=0|UB[7&n](A,133,e,r,0,i)):0|(r=i)):(Xe(32,104,120,0,0),0|(r=-2)):(Xe(32,104,121,0,0),0|(r=-2))}function Ie(A,e,r){if(A|=0,!(e=(0|(e=(0|(e|=0))>0?e:0))>(0|(r|=0))?r:e))return 0|(A=1);for(;;){if(e=e+-1|0,1!=(0|Ce(A,54690))){e=0,r=5;break}if(!e){e=1,r=5;break}}return 5==(0|r)?0|e:0}function ge(A,e,r){A|=0,e|=0,r|=0;var i;return i=G,G=G+16|0,t[i>>2]=r,r=0|function(A,e,r){A|=0,e|=0,r|=0;var i;if(i=G,G=G+2080|0,t[i+2060>>2]=i,t[i+2056>>2]=2048,t[i+2052>>2]=0,e=0==(0|De(i+2060|0,i+2052|0,i+2056|0,i+2064|0,i+2048|0,e,r)),r=0|t[i+2052>>2],e)return Xn(r,82796,0),G=i,0|(A=-1);return e=0|t[i+2064>>2],r?(A=0|se(A,r,e),Xn(0|t[i+2052>>2],82796,0),G=i,0|A):(A=0|se(A,i,e),G=i,0|A)}(A,e,i),G=i,0|r}function De(A,e,r,f,a,o,u){A|=0,e|=0,r|=0,f|=0,a|=0,o|=0,u|=0;var l,c=0,E=0,Q=0,w=0,b=0,k=0,s=0,C=0,d=0,I=0;l=G,G=G+16|0,t[l>>2]=0,b=0,k=0|i[o>>0],c=0,o=o+1|0,w=-1,s=0,E=0;A:for(;;){Q=k<<24>>24;e:do{if(k<<24>>24){if(0==(0|e)&&(0|t[l>>2])>>>0>=(0|t[r>>2])>>>0){Q=s,E=7;break}switch(0|E){case 0:if(k<<24>>24!=37){if(!(0|Me(A,e,l,r,Q))){c=0,d=79;break A}E=0}else E=1;k=0|i[o>>0],o=o+1|0,Q=s;break e;case 1:switch(0|Q){case 45:k=0|i[o>>0],c|=1,o=o+1|0,Q=s,E=1;break e;case 43:k=0|i[o>>0],c|=2,o=o+1|0,Q=s,E=1;break e;case 32:k=0|i[o>>0],c|=4,o=o+1|0,Q=s,E=1;break e;case 35:k=0|i[o>>0],c|=8,o=o+1|0,Q=s,E=1;break e;case 48:k=0|i[o>>0],c|=16,o=o+1|0,Q=s,E=1;break e;default:Q=s,E=2;break e}case 2:if(((255&k)-48|0)>>>0<10){k=0|i[o>>0],o=o+1|0,Q=Q+-48+(10*s|0)|0,E=2;break e}if(k<<24>>24!=42){Q=s,E=3;break e}k=3+(0|t[u>>2])&-4,Q=0|t[k>>2],t[u>>2]=k+4,k=0|i[o>>0],o=o+1|0,E=3;break e;case 3:if(k<<24>>24!=46){Q=s,E=5;break e}k=0|i[o>>0],o=o+1|0,Q=s,E=4;break e;case 4:if(((255&k)-48|0)>>>0<10){k=0|i[o>>0],o=o+1|0,w=Q+-48+(10*((0|w)>0?w:0)|0)|0,Q=s,E=4;break e}if(k<<24>>24!=42){Q=s,E=5;break e}k=3+(0|t[u>>2])&-4,w=0|t[k>>2],t[u>>2]=k+4,k=0|i[o>>0],o=o+1|0,Q=s,E=5;break e;case 5:switch(0|Q){case 104:b=1,k=0|i[o>>0],o=o+1|0,Q=s,E=6;break e;case 108:o=(b=108==(0|i[o>>0]))?o+1|0:o,b=b?4:2,k=0|i[o>>0],o=o+1|0,Q=s,E=6;break e;case 113:b=4,k=0|i[o>>0],o=o+1|0,Q=s,E=6;break e;case 76:b=3,k=0|i[o>>0],o=o+1|0,Q=s,E=6;break e;default:Q=s,E=6;break e}case 6:r:do{switch(0|Q){case 105:case 100:switch(0|b){case 1:E=3+(0|t[u>>2])&-4,Q=0|t[E>>2],t[u>>2]=E+4,E=(65535&Q)<<16>>16,Q=(((65535&Q)<<16>>16|0)<0)<<31>>31;break;case 2:E=3+(0|t[u>>2])&-4,Q=0|t[E>>2],t[u>>2]=E+4,E=Q,Q=((0|Q)<0)<<31>>31;break;case 4:k=7+(0|t[u>>2])&-8,E=0|t[k>>2],Q=0|t[k+4>>2],t[u>>2]=k+8;break;default:E=3+(0|t[u>>2])&-4,Q=0|t[E>>2],t[u>>2]=E+4,E=Q,Q=((0|Q)<0)<<31>>31}if(!(0|he(A,e,l,r,E,Q,10,s,w,c))){c=0,d=79;break A}break;case 88:c|=32,d=38;break;case 117:case 111:case 120:d=38;break;case 102:if(k=7+(0|t[u>>2])&-8,I=+B[k>>3],t[u>>2]=k+8,!(0|Ue(A,e,l,r,I,s,w,c,0))){c=0,d=79;break A}break;case 69:c|=32,d=46;break;case 101:d=46;break;case 71:c|=32,d=48;break;case 103:d=48;break;case 99:if(k=3+(0|t[u>>2])&-4,s=0|t[k>>2],t[u>>2]=k+4,!(0|Me(A,e,l,r,s))){c=0,d=79;break A}break;case 115:if(E=3+(0|t[u>>2])&-4,Q=0|t[E>>2],t[u>>2]=E+4,s=((w=s-(b=0|st(Q=0==(0|Q)?54692:Q,(k=(0|(E=(E=(0|w)>-1)|0!=(0|e)?E?w:2147483647:0|t[r>>2]))>-1)?E:-1))|0)|s|0)<0?0:w,k=k?(2147483647-s|0)>(0|E)?s+E|0:2147483647:E,(0|(E=0==(1&c|0)?s:0-s|0))>0&0!=(0|k))for(c=0,w=E;;){if(!(0|Me(A,e,l,r,32))){c=0,d=79;break A}if(E=w+-1|0,!((0|w)>1&k>>>0>(c=c+1|0)>>>0))break;w=E}else c=0;if(0!=(0|b)&k>>>0>c>>>0)for(w=b;;){if(!(0|Me(A,e,l,r,0|i[Q>>0]))){c=0,d=79;break A}if(!(k>>>0>(c=c+1|0)>>>0&0!=(0|(w=w+-1|0))))break;Q=Q+1|0}if((0|E)<0&k>>>0>c>>>0)for(;;){if(!(0|Me(A,e,l,r,32))){c=0,d=79;break A}if(!((0|E)<-1&k>>>0>(c=c+1|0)>>>0))break;E=E+1|0}break;case 112:if(b=3+(0|t[u>>2])&-4,k=0|t[b>>2],t[u>>2]=b+4,!(0|he(A,e,l,r,k,0,16,s,w,8|c))){c=0,d=79;break A}break;case 110:switch(0|b){case 1:k=3+(0|t[u>>2])&-4,s=0|t[k>>2],t[u>>2]=k+4,n[s>>1]=t[l>>2];break r;case 2:k=3+(0|t[u>>2])&-4,s=0|t[k>>2],t[u>>2]=k+4,t[s>>2]=t[l>>2];break r;case 4:k=3+(0|t[u>>2])&-4,s=0|t[k>>2],t[u>>2]=k+4,t[s>>2]=t[l>>2],t[s+4>>2]=0;break r;default:k=3+(0|t[u>>2])&-4,s=0|t[k>>2],t[u>>2]=k+4,t[s>>2]=t[l>>2];break r}case 37:if(!(0|Me(A,e,l,r,37))){c=0,d=79;break A}break;case 119:o=o+1|0}}while(0);if(38==(0|d)){switch(d=0,Q=64|c,0|b){case 1:E=3+(0|t[u>>2])&-4,c=0|t[E>>2],t[u>>2]=E+4,c&=65535,E=0;break;case 2:E=3+(0|t[u>>2])&-4,c=0|t[E>>2],t[u>>2]=E+4,E=0;break;case 4:b=7+(0|t[u>>2])&-8,c=0|t[b>>2],E=0|t[b+4>>2],t[u>>2]=b+8;break;default:E=3+(0|t[u>>2])&-4,c=0|t[E>>2],t[u>>2]=E+4,E=0}if(!(0|he(A,e,l,r,c,E,k<<24>>24==111?8:k<<24>>24==117?10:16,s,w,Q))){c=0,d=79;break A}}else if(46==(0|d)){if(d=0,k=7+(0|t[u>>2])&-8,I=+B[k>>3],t[u>>2]=k+8,!(0|Ue(A,e,l,r,I,s,w,c,1))){c=0,d=79;break A}}else if(48==(0|d)&&(d=0,k=7+(0|t[u>>2])&-8,I=+B[k>>3],t[u>>2]=k+8,!(0|Ue(A,e,l,r,I,s,w,c,2)))){c=0,d=79;break A}b=0,k=0|i[o>>0],c=0,o=o+1|0,w=-1,Q=0,E=0;break e;default:Q=s;break e}}else k=0,Q=s,E=7}while(0);if(7==(0|E)){d=74;break}s=Q}return 74==(0|d)?(0==(0|e)&&(C=(0|t[r>>2])-1|0,d=(0|t[l>>2])>>>0>C>>>0,t[a>>2]=1&d,d)&&(t[l>>2]=C),0|Me(A,e,l,r,0)?(t[f>>2]=(0|t[l>>2])-1,G=l,0|(d=1)):(G=l,0|(d=0))):79==(0|d)?(G=l,0|c):0}function Me(A,e,r,n,f){r|=0,n|=0,f|=0;var a=0,B=0,o=0;0!=(0|(e|=0))|0!=(0|t[(A|=0)>>2])||oi(54730,82796,0),(B=0|t[r>>2])>>>0>(a=0|t[n>>2])>>>0&&oi(54783,82796,0);do{if(0!=(0|e)&(0|B)==(0|a)){if(B>>>0>2147482623)return 0|(r=0);if(t[n>>2]=B+1024,0|(a=0|t[e>>2])){if(a=0|Tn(a,B+1024|0,82796,0)){t[e>>2]=a;break}return 0|(r=0)}if(o=0|Ln(B+1024|0,82796,0),t[e>>2]=o,!o)return 0|(r=0);a=0|t[r>>2];do{if(0|a){if(B=0|t[A>>2]){If(0|o,0|B,0|a);break}oi(54821,82796,0)}}while(0);t[A>>2]=0}}while(0);return(B=0|t[r>>2])>>>0>=(0|t[n>>2])>>>0?0|(r=1):(a=0|t[A>>2])?(t[r>>2]=B+1,i[a+B>>0]=f,0|(r=1)):(e=0|t[e>>2],t[r>>2]=B+1,i[e+B>>0]=f,0|(r=1))}function he(A,e,r,n,t,f,a,B,o,u){A|=0,e|=0,r|=0,n|=0,t|=0,f|=0,a|=0,B|=0,u|=0;var l,c,Q=0,w=0,b=0,k=0,s=0,C=0,d=0;c=G,G=G+32|0,l=(0|(o|=0))>0?o:0;do{if(!(64&u)){if((0|f)<0){t=0|Ef(0,0,0|t,0|f),f=0|E(),k=45;break}k=0==(2&u|0)?u<<3&32:43;break}k=0}while(0);for(b=16==(0|a)?54710:8==(0|a)?80614:82796,w=0==(32&u|0)?54713:1312,Q=0;d=0|Ef(0|(s=t),0|(C=f),0|(d=0|lf(0|(t=0|bf(0|t,0|f,0|a,0)),0|(f=0|E()),0|a,0)),0|E()),E(),o=Q+1|0,i[c+Q>>0]=0|i[w+d>>0],(C>>>0>0|0==(0|C)&s>>>0>=a>>>0)&o>>>0<26;)Q=o;w=0==(8&u|0)?82796:b,i[c+(t=26==(0|o)?Q:o)>>0]=0,f=(l-t|0)>0?l-t|0:0,d=(0|(d=((b=0!=(0|k))<<31>>31)+B-((0|l)<(0|t)?t:l)-(0|mt(w))|0))>0?d:0,o=0==(16&u|0)?d:0,f=0==(16&u|0)?f:(0|f)<(0|d)?d:f,o=0==(1&u|0)?o:0-o|0;A:do{if((0|o)>0){for(Q=o;;){if(!(0|Me(A,e,r,n,32))){o=0;break}if(o=Q+-1|0,!((0|Q)>1))break A;Q=o}return G=c,0|o}}while(0);if(b&&0==(0|Me(A,e,r,n,k)))return G=c,0|(d=0);Q=0|i[w>>0];A:do{if(Q<<24>>24){for(;;){if(w=w+1|0,!(0|Me(A,e,r,n,Q<<24>>24))){o=0;break}if(!((Q=0|i[w>>0])<<24>>24))break A}return G=c,0|o}}while(0);A:do{if((0|f)>0){for(;;){if(!(0|Me(A,e,r,n,48))){o=0;break}if(!((0|f)>1))break A;f=f+-1|0}return G=c,0|o}}while(0);A:do{if(0|t){for(;;){if(f=t,!(0|Me(A,e,r,n,0|i[c+(t=t+-1|0)>>0]))){o=0;break}if((0|f)<=1)break A}return G=c,0|o}}while(0);if((0|o)>=0)return G=c,0|(d=1);for(;;){if(!(0|Me(A,e,r,n,32))){o=0,t=29;break}if(!((0|o)<-1)){o=1,t=29;break}o=o+1|0}return 29==(0|t)?(G=c,0|o):0}function Ue(A,e,r,n,t,f,a,B,o){A|=0,e|=0,r|=0,n|=0,f|=0,B|=0,o|=0;var u,l=0,c=0,E=0,Q=0,w=0,b=0,k=0,s=0,C=0,d=0,I=0;u=G,G=G+96|0,E=(0|(a|=0))<0?6:a,d=(t=+t)<0?45:0==(2&B|0)?B<<3&32:43;do{if(2==(0|o))if(0!=t)if(t<1e-4||t>=10&0==(0|E))l=1,I=13;else{if((0|E)>0){a=E,w=1;do{w*=10,a=a+-1|0}while(0!=(0|a));if(w<=t){l=1,I=12;break}}l=0,I=12}else l=0,I=13;else l=o,I=12}while(0);if(12==(0|I)&&(o?I=13:(C=0,w=t,a=E,k=l)),13==(0|I)){if(0!=t){if(t<1){a=0,w=t;do{w*=10,a=a+-1|0}while(w<1)}else a=0,w=t;if(w>10){do{w/=10,a=a+1|0}while(w>10);c=a}else c=a}else c=0,w=t;if(2==(0|o)){if(a=0==(0|E)?0:E+-1|0,!l&&(0|(a=a-c|0))<0)return G=u,0|(A=0)}else a=E;C=c,w=1==(0|l)?w:t,k=l}if((t=w<0?-w:w)>4294967295)return G=u,0|(A=0);if(b=(0|a)<9?a:9){l=b,w=1;do{w*=10,l=l+-1|0}while(0!=(0|l));c=(w-+(0|(c=~~w))>=.5&1)+c|0,l=b,w=1;do{w*=10,l=l+-1|0}while(0!=(0|l))}else c=1,w=1;for(Q=(l=(((w*=t-+(~~t>>>0>>>0))-+(0|~~w)>=.5&1)+~~w|0)>>>0<c>>>0)?0:c,l=(1&(1^l))+(~~t>>>0)|0,E=0;s=l,l=(l>>>0)/10|0,c=E+1|0,i[u+64+E>>0]=0|i[s-(10*l|0)+54699>>0],s>>>0>9&c>>>0<20;)E=c;l=(w-+(0|~~w)>=.5&1)+~~w-Q|0,i[u+64+(Q=20==(0|c)?E:c)>>0]=0;A:do{if((0|a)>0){if(2!=(0|o))for(c=0,a=l;;){if(s=a,a=(a>>>0)/10|0,l=c+1|0,i[u+32+c>>0]=0|i[s-(10*a|0)+54699>>0],(0|l)>=(0|b)){c=b;break A}c=l}for(a=l;;){for(l=0;(c=a-(10*(E=(a>>>0)/10|0)|0)|0)|l;){if(a=l+1|0,i[u+32+l>>0]=0|i[54699+c>>0],!((0|a)<(0|b))){l=a,c=b;break A}l=a,a=E}if((0|l)>=(0|(c=b+-1|0)))break A;if(!((0|b)>1)){l=0;break}a=(a>>>0)/10|0,b=c}}else l=0,c=b}while(0);if(i[u+32+(E=l+((20==(0|l))<<31>>31)|0)>>0]=0,s=1==(0|k)){for(a=0,b=(0|C)<0?0-C|0:C;k=b,b=(0|b)/10|0,l=a+1|0,i[u+a>>0]=0|i[k-(10*b|0)+54699>>0],(0|k)>9&l>>>0<20;)a=l;if((0|k)>9)return G=u,0|(A=0);a||(i[u+l>>0]=48,l=2)}else l=0;a=((b=0!=(0|d))<<31>>31)+f-Q-c+((o=(0|c)>0)<<31>>31)+(s?-2-l|0:0)|0,c=c-E|0,a=(0|a)>0?a:0,a=0==(1&B|0)?a:0-a|0;A:do{if(0!=(16&B|0)&(0|a)>0){do{if(b){if(0|Me(A,e,r,n,d)){if((0|a)>1){a=a+-1|0;break}a=0;break A}return G=u,0|(A=0)}}while(0);for(b=a;;){if(!(0|Me(A,e,r,n,48))){a=0;break}if(a=b+-1|0,!((0|b)>1)){k=0,I=55;break A}b=a}return G=u,0|a}k=d,I=55}while(0);if(55==(0|I)){A:do{if((0|a)>0){for(b=a;;){if(!(0|Me(A,e,r,n,32))){a=0;break}if(a=b+-1|0,!((0|b)>1))break A;b=a}return G=u,0|a}}while(0);if(0!=(0|k)&&0==(0|Me(A,e,r,n,k)))return G=u,0|(A=0)}A:do{if(0|Q){for(;;){if(b=Q,!(0|Me(A,e,r,n,0|i[u+64+(Q=Q+-1|0)>>0]))){a=0;break}if((0|b)<=1)break A}return G=u,0|a}}while(0);A:do{if(!(0==(8&B|0)&(1^o))){if(!(0|Me(A,e,r,n,46)))return G=u,0|(A=0);if((0|E)>0){for(;;){if(Q=E,!(0|Me(A,e,r,n,0|i[u+32+(E=E+-1|0)>>0]))){a=0;break}if((0|Q)<=1)break A}return G=u,0|a}}}while(0);A:do{if((0|c)>0){for(;;){if(!(0|Me(A,e,r,n,48))){a=0;break}if(!((0|c)>1))break A;c=c+-1|0}return G=u,0|a}}while(0);A:do{if(s){if(!(0|Me(A,e,r,n,32&B^101)))return G=u,0|(A=0);if((0|C)<0){if(!(0|Me(A,e,r,n,45)))return G=u,0|(A=0)}else if(!(0|Me(A,e,r,n,43)))return G=u,0|(A=0);if((0|l)>0){for(;;){if(c=l,!(0|Me(A,e,r,n,0|i[u+(l=l+-1|0)>>0]))){a=0;break}if((0|c)<=1)break A}return G=u,0|a}}}while(0);if((0|a)>=0)return G=u,0|(A=1);for(;;){if(!(0|Me(A,e,r,n,32))){a=0,I=88;break}if(!((0|a)<-1)){a=1,I=88;break}a=a+1|0}return 88==(0|I)?(G=u,0|a):0}function Ge(A,e,r,i){A|=0,e|=0,r|=0,i|=0;var n;return n=G,G=G+32|0,t[n>>2]=i,t[n+28>>2]=A,t[n+24>>2]=e,(i=0!=(0|De(n+28|0,0,n+24|0,n+20|0,n+16|0,r,n)))&0==(0|t[n+16>>2])?(i=0|t[n+20>>2],G=n,0|(i=(0|i)>-1?i:-1)):(G=n,0|(i=-1))}function Ze(A,e){A|=0,e|=0;var r=0,i=0;if(!(r=0|Sn(100,82796,0)))return 0|(r=0);i=0|Sn(64,82796,0),t[r>>2]=i;do{if(i){if(i=0|W(),t[r+12>>2]=i,!i){e=0|t[r>>2];break}return t[r+4>>2]=0==(0|e)?28:e,t[r+8>>2]=0==(0|A)?17:A,t[r+16>>2]=8,t[r+20>>2]=16,t[r+28>>2]=8,t[r+32>>2]=512,t[r+36>>2]=256,0|(i=r)}e=0}while(0);return Xn(e,82796,0),Xn(r,82796,0),0|(i=0)}function Ne(A){var e=0,r=0,n=0,t=0,f=0;if(!(A|=0))return 0|(n=0);if(!((e=0|i[A>>0])<<24>>24))return 0|(n=0);for(r=256,n=0;n=(n>>>(32-(15&((f=e<<24>>24)>>>2^f))|0)|n<<(15&(f>>>2^f)))^(0|o(t=r|f,t)),(e=0|i[(A=A+1|0)>>0])<<24>>24;)r=r+256|0;return 0|(f=n>>>16^n)}function Fe(A){var e=0,r=0,i=0;if(A|=0){if(0|(e=0|t[A+16>>2])){i=0;do{if(r=0|t[(0|t[A>>2])+(i<<2)>>2]){e=r;do{r=e,e=0|t[e+4>>2],Xn(r,82796,0)}while(0!=(0|e));e=0|t[A+16>>2]}i=i+1|0}while(i>>>0<e>>>0)}V(0|t[A+12>>2]),Xn(0|t[A>>2],82796,0),Xn(A,82796,0)}}function We(A,e){e|=0;var r,i=0,n=0,f=0,a=0,B=0,o=0,u=0;if(r=G,G=G+16|0,t[(A|=0)+96>>2]=0,i=0|t[A+16>>2],(0|t[A+32>>2])>>>0<=((t[A+40>>2]<<8>>>0)/(i>>>0)|0)>>>0){B=0|t[A+20>>2],o=0|t[A+24>>2],u=0|t[A+28>>2];do{if((o+1|0)>>>0>=u>>>0){if(i=0|Tn(0|t[A>>2],B<<3,82796,0)){t[A>>2]=i,Df(i+(B<<2)|0,0,B<<2|0),t[A+28>>2]=B,t[A+20>>2]=B<<1,t[A+48>>2]=1+(0|t[A+48>>2]),i=0|t[A+16>>2],n=0;break}return e=A+96|0,A=0,u=(u=0|t[e>>2])+1|0,t[e>>2]=u,G=r,0|A}n=o+1|0}while(0);if(t[A+24>>2]=n,t[A+16>>2]=i+1,t[A+44>>2]=1+(0|t[A+44>>2]),a=0|t[A>>2],t[a+(u+o<<2)>>2]=0,0|(i=0|t[a+(o<<2)>>2]))for(n=a+(o<<2)|0,f=i;i=f+4|0,(0|((0|t[f+8>>2])>>>0)%(B>>>0))!=(0|o)&&(t[n>>2]=t[i>>2],t[i>>2]=t[a+(u+o<<2)>>2],t[a+(u+o<<2)>>2]=f,i=n),f=0|t[i>>2];)n=i}return n=0|Ye(A,e,r),0|(i=0|t[n>>2])?(u=0|t[i>>2],t[i>>2]=e,e=A+72|0,A=u,u=(u=0|t[e>>2])+1|0,t[e>>2]=u,G=r,0|A):(i=0|Ln(12,82796,0))?(t[i>>2]=e,t[i+4>>2]=0,t[i+8>>2]=t[r>>2],t[n>>2]=i,t[A+68>>2]=1+(0|t[A+68>>2]),e=A+40|0,A=0,u=(u=0|t[e>>2])+1|0,t[e>>2]=u,G=r,0|A):(e=A+96|0,A=0,u=(u=0|t[e>>2])+1|0,t[e>>2]=u,G=r,0|A)}function Ye(A,e,r){e|=0,r|=0;var i,n,f,a=0;if(f=G,G=G+16|0,n=0|IB[31&t[(A|=0)+8>>2]](e),T(A+60|0,1,f,0|t[A+12>>2]),t[r>>2]=n,(r=(n>>>0)%((0|t[A+28>>2])>>>0)|0)>>>0<(0|t[A+24>>2])>>>0&&(r=(n>>>0)%((0|t[A+20>>2])>>>0)|0),i=0|t[A+4>>2],r=(0|t[A>>2])+(r<<2)|0,!(a=0|t[r>>2]))return G=f,0|(A=r);for(;;){if(T(A+92|0,1,f,0|t[A+12>>2]),(0|t[a+8>>2])==(0|n)&&(T(A+64|0,1,f,0|t[A+12>>2]),0==(0|gB[63&i](0|t[a>>2],e)))){a=8;break}if(!(a=0|t[(r=a+4|0)>>2])){a=8;break}}return 8==(0|a)?(G=f,0|r):0}function Re(A,e){e|=0;var r,i,n,f=0,a=0,B=0;if(n=G,G=G+16|0,t[(A|=0)+96>>2]=0,e=0|Ye(A,e,n),!(f=0|t[e>>2]))return t[A+80>>2]=1+(0|t[A+80>>2]),G=n,0|(B=0);if(t[e>>2]=t[f+4>>2],B=0|t[f>>2],Xn(f,82796,0),t[A+76>>2]=1+(0|t[A+76>>2]),e=(0|t[A+40>>2])-1|0,t[A+40>>2]=e,(f=0|t[A+16>>2])>>>0<=16)return G=n,0|B;if((0|t[A+36>>2])>>>0<((e<<8>>>0)/(f>>>0)|0)>>>0)return G=n,0|B;r=0|t[A+24>>2],e=0|t[A+28>>2],a=(0|t[A>>2])+(r+-1+e<<2)|0,i=0|t[a>>2],t[a>>2]=0;do{if(!r){if(0|(e=0|Tn(0|t[A>>2],e<<2,82796,0))){t[A+56>>2]=1+(0|t[A+56>>2]),t[A+20>>2]=(0|t[A+20>>2])>>>1,f=(0|t[A+28>>2])>>>1,t[A+28>>2]=f,t[A+24>>2]=f+-1,t[A>>2]=e,a=0|t[A+16>>2],f=f+-1|0;break}return t[A+96>>2]=1+(0|t[A+96>>2]),G=n,0|B}t[A+24>>2]=r+-1,a=f,e=0|t[A>>2],f=r+-1|0}while(0);if(t[A+16>>2]=a+-1,t[A+52>>2]=1+(0|t[A+52>>2]),!(e=0|t[(f=e+(f<<2)|0)>>2]))return t[f>>2]=i,G=n,0|B;for(;f=0|t[e+4>>2];)e=f;return t[e+4>>2]=i,G=n,0|B}function ye(A,e){e|=0;var r;return r=G,G=G+16|0,t[(A|=0)+96>>2]=0,(e=0|t[(0|Ye(A,e,r+4|0))>>2])?(e=0|t[e>>2],T(A+84|0,1,r,0|t[A+12>>2]),G=r,0|(A=e)):(T(A+88|0,1,r,0|t[A+12>>2]),G=r,0|(A=0))}function Ve(A,e){e|=0;var r=0,i=0,n=0;if((A|=0)&&!((0|(r=(0|t[A+16>>2])-1|0))<=-1))do{if(0|(i=0|t[(0|t[A>>2])+(r<<2)>>2]))do{n=i,i=0|t[i+4>>2],NB[31&e](0|t[n>>2])}while(0!=(0|i));r=r+-1|0}while((0|r)>-1)}function me(A,e,r){e|=0,r|=0;var i=0,n=0,f=0;if((A|=0)&&!((0|(i=(0|t[A+16>>2])-1|0))<=-1))do{if(0|(n=0|t[(0|t[A>>2])+(i<<2)>>2]))do{f=n,n=0|t[n+4>>2],FB[7&e](0|t[f>>2],r)}while(0!=(0|n));i=i+-1|0}while((0|i)>-1)}function He(A){var e=0,r=0,n=0,t=0,f=0;if(!(A|=0))return 0|(n=0);if(!((e=0|i[A>>0])<<24>>24))return 0|(n=0);for(r=256,n=0;n=(n>>>(32-(15&((f=0|Vt(e<<24>>24))>>>2^f))|0)|n<<(15&(f>>>2^f)))^(0|o(t=f|r,t)),(e=0|i[(A=A+1|0)>>0])<<24>>24;)r=r+256|0;return 0|(f=n>>>16^n)}function ve(A,e){e|=0,t[(A|=0)+36>>2]=e}function Je(A){return 0|t[(A|=0)+96>>2]}function Xe(A,e,r,i,n){A|=0,e|=0,r|=0,i|=0,n|=0;var f,a=0;(f=0|Te())&&(a=(1+(0|t[f+384>>2])|0)%16|0,t[f+384>>2]=a,(0|a)==(0|t[f+388>>2])&&(t[f+388>>2]=(a+1|0)%16|0),t[f+(a<<2)>>2]=0,a=0|t[f+384>>2],t[f+64+(a<<2)>>2]=e<<12&16773120|A<<24|4095&r,t[f+256+(a<<2)>>2]=i,t[f+320+(a<<2)>>2]=n,i=0|t[f+384>>2],1&t[f+192+(i<<2)>>2]?(Xn(0|t[f+128+(i<<2)>>2],82796,0),i=0|t[f+384>>2],t[f+128+(i<<2)>>2]=0,i=f+192+(i<<2)|0):i=f+192+(i<<2)|0,t[i>>2]=0)}function Te(){var A=0;if(!(0|Kn(262144,0,0)))return 0|(A=0);if(!((A=0!=(0|m(82056,1)))&0!=(0|t[20515])))return 0|(A=0);if(-1==(0|(A=0|v(82052))))return 0|(A=0);if(0|A)return 0|A;if(!(0|J(82052,-1)))return 0|(A=0);if(!(A=0|Sn(392,82796,0)))return J(82052,0),0|(A=0);do{if(0|zn(2,0)){if(!(0|J(82052,A))){Le(A);break}return Kn(2,0,0),0|A}Le(A)}while(0);return J(82052,0),0|(A=0)}function Le(A){(A|=0)&&(1&t[A+192>>2]|0&&(Xn(0|t[A+128>>2],82796,0),t[A+128>>2]=0),t[A+192>>2]=0,1&t[A+196>>2]|0&&(Xn(0|t[A+132>>2],82796,0),t[A+132>>2]=0),t[A+196>>2]=0,1&t[A+200>>2]|0&&(Xn(0|t[A+136>>2],82796,0),t[A+136>>2]=0),t[A+200>>2]=0,1&t[A+204>>2]|0&&(Xn(0|t[A+140>>2],82796,0),t[A+140>>2]=0),t[A+204>>2]=0,1&t[A+208>>2]|0&&(Xn(0|t[A+144>>2],82796,0),t[A+144>>2]=0),t[A+208>>2]=0,1&t[A+212>>2]|0&&(Xn(0|t[A+148>>2],82796,0),t[A+148>>2]=0),t[A+212>>2]=0,1&t[A+216>>2]|0&&(Xn(0|t[A+152>>2],82796,0),t[A+152>>2]=0),t[A+216>>2]=0,1&t[A+220>>2]|0&&(Xn(0|t[A+156>>2],82796,0),t[A+156>>2]=0),t[A+220>>2]=0,1&t[A+224>>2]|0&&(Xn(0|t[A+160>>2],82796,0),t[A+160>>2]=0),t[A+224>>2]=0,1&t[A+228>>2]|0&&(Xn(0|t[A+164>>2],82796,0),t[A+164>>2]=0),t[A+228>>2]=0,1&t[A+232>>2]|0&&(Xn(0|t[A+168>>2],82796,0),t[A+168>>2]=0),t[A+232>>2]=0,1&t[A+236>>2]|0&&(Xn(0|t[A+172>>2],82796,0),t[A+172>>2]=0),t[A+236>>2]=0,1&t[A+240>>2]|0&&(Xn(0|t[A+176>>2],82796,0),t[A+176>>2]=0),t[A+240>>2]=0,1&t[A+244>>2]|0&&(Xn(0|t[A+180>>2],82796,0),t[A+180>>2]=0),t[A+244>>2]=0,1&t[A+248>>2]|0&&(Xn(0|t[A+184>>2],82796,0),t[A+184>>2]=0),t[A+248>>2]=0,1&t[A+252>>2]|0&&(Xn(0|t[A+188>>2],82796,0),t[A+188>>2]=0),t[A+252>>2]=0,Xn(A,82796,0))}function Se(){var A,e=0,r=0,i=0;if(A=0|Te()){i=0;do{t[A+(i<<2)>>2]=0,t[A+64+(i<<2)>>2]=0,r=A+128+(i<<2)|0,1&t[(e=A+192+(i<<2)|0)>>2]|0&&(Xn(0|t[r>>2],82796,0),t[r>>2]=0),t[e>>2]=0,t[A+256+(i<<2)>>2]=0,t[A+320+(i<<2)>>2]=-1,i=i+1|0}while(16!=(0|i));t[A+388>>2]=0,t[A+384>>2]=0}}function pe(){return 0|function(A,e,r,i,n,f){A|=0,e|=0,r|=0,i|=0,n|=0,f|=0;var a,B=0,o=0,u=0,l=0;if(!(a=0|Te()))return 0|(f=0);if(0!=(0|A)&0!=(0|e))return 0|r&&(t[r>>2]=82796),0|i&&(t[i>>2]=0),0|n&&(t[n>>2]=82796),f?(t[f>>2]=0,0|(f=68)):0|(f=68);if((0|(B=0|t[a+388>>2]))==(0|(o=0|t[a+384>>2])))return 0|(f=0);for(;;){if(2&t[(u=a+(o<<2)|0)>>2])t[u>>2]=0,B=0|t[a+384>>2],t[a+64+(B<<2)>>2]=0,1&t[a+192+(B<<2)>>2]?(Xn(0|t[a+128+(B<<2)>>2],82796,0),B=0|t[a+384>>2],t[a+128+(B<<2)>>2]=0,B=a+192+(B<<2)|0):B=a+192+(B<<2)|0,t[B>>2]=0,o=0|t[a+384>>2],t[a+256+(o<<2)>>2]=0,t[a+320+(o<<2)>>2]=-1,o=(0|(o=0|t[a+384>>2]))>0?o+-1|0:15,t[a+384>>2]=o;else{if(!(2&t[a+((B=(B+1|0)%16|0)<<2)>>2]))break;t[a+388>>2]=B,t[a+(B<<2)>>2]=0,B=0|t[a+388>>2],t[a+64+(B<<2)>>2]=0,1&t[a+192+(B<<2)>>2]?(Xn(0|t[a+128+(B<<2)>>2],82796,0),B=0|t[a+388>>2],t[a+128+(B<<2)>>2]=0,B=a+192+(B<<2)|0):B=a+192+(B<<2)|0,t[B>>2]=0,o=0|t[a+388>>2],t[a+256+(o<<2)>>2]=0,t[a+320+(o<<2)>>2]=-1,o=0|t[a+384>>2]}if((0|(B=0|t[a+388>>2]))==(0|o)){B=0,l=39;break}}return 39==(0|l)?0|B:(o=0|t[a+64+((u=0|e?o:B)<<2)>>2],0|A&&(t[a+388>>2]=u,t[a+64+(u<<2)>>2]=0),0!=(0|r)&0!=(0|i)&&((B=0|t[a+256+(u<<2)>>2])?(t[r>>2]=B,B=0|t[a+320+(u<<2)>>2]):(t[r>>2]=54856,B=0),t[i>>2]=B),n?(B=0|t[a+128+(u<<2)>>2])?(t[n>>2]=B,f?(t[f>>2]=t[a+192+(u<<2)>>2],0|(f=o)):0|(f=o)):(t[n>>2]=82796,f?(t[f>>2]=0,0|(f=o)):0|(f=o)):A?(1&t[a+192+(u<<2)>>2]|0&&(Xn(0|t[a+128+(u<<2)>>2],82796,0),t[a+128+(u<<2)>>2]=0),t[a+192+(u<<2)>>2]=0,0|(f=o)):0|(f=o))}(0,1,0,0,0,0)}function ze(){var A;(A=0|v(82052))&&(J(82052,0),Le(A))}function Ke(A,e){A|=0,e|=0;var r;r=G,G=G+16|0,t[r>>2]=e,je(A,r),G=r}function je(A,e){A|=0,e|=0;var r=0,n=0,f=0,a=0,B=0,o=0;if(f=0|Ln(81,82796,0)){i[f>>0]=0;A:do{if((0|A)>0){for(o=0,n=0,r=80;;){if(a=3+(0|t[e>>2])&-4,B=0|t[a>>2],t[e>>2]=a+4,B){if((0|(n=(0|mt(B))+n|0))>(0|r)){if(!(r=0|Tn(f,n+21|0,82796,0)))break;a=n+20|0,f=r}else a=r;It(f,B,a+1|0),r=a}if((0|(o=o+1|0))>=(0|A))break A}return void Xn(f,82796,0)}}while(0);(n=0|Te())&&(r=0|t[n+384>>2],1&t[n+192+(r<<2)>>2]|0&&(Xn(0|t[n+128+(r<<2)>>2],82796,0),t[n+128+(r<<2)>>2]=0),t[n+128+(r<<2)>>2]=f,t[n+192+(r<<2)>>2]=3)}}function xe(){var A,e=0;return(e=0|Te())?(A=0|t[e+384>>2],(0|t[e+388>>2])==(0|A)?0|(e=0):(t[e+(A<<2)>>2]=1|t[e+(A<<2)>>2],0|(e=1))):0|(e=0)}function Oe(){var A,e=0,r=0,i=0;if(!(A=0|Te()))return 0|(i=0);if(e=0|t[A+384>>2],(0|t[A+388>>2])==(0|e))return 0|(i=0);for(;!(1&(e=0|t[(r=A+(e<<2)|0)>>2])|0);)if(t[r>>2]=0,e=0|t[A+384>>2],t[A+64+(e<<2)>>2]=0,1&t[A+192+(e<<2)>>2]?(Xn(0|t[A+128+(e<<2)>>2],82796,0),e=0|t[A+384>>2],t[A+128+(e<<2)>>2]=0,e=A+192+(e<<2)|0):e=A+192+(e<<2)|0,t[e>>2]=0,e=0|t[A+384>>2],t[A+256+(e<<2)>>2]=0,t[A+320+(e<<2)>>2]=-1,e=0==(0|(e=0|t[A+384>>2]))?15:e+-1|0,t[A+384>>2]=e,(0|t[A+388>>2])==(0|e)){e=0,i=9;break}return 9==(0|i)?0|e:(t[r>>2]=-2&e,0|(i=1))}function Pe(A){A|=0;var e,r,i;r=G,G=G+16|0,(e=0|Te())?(i=0|t[e+384>>2],t[r>>2]=(A+-1&(-2147483648^A))>>31,t[r>>2]=~((A+-1&(-2147483648^A))>>31),t[e+(i<<2)>>2]=t[e+(i<<2)>>2]|2&t[r>>2],G=r):G=r}function _e(A,e,r,n){return A|=0,r|=0,((e|=0)-(n|=0)|0)<2?(Xe(4,127,110,0,0),0|(r=-1)):(2==(e-n|0)?(i[A>>0]=106,e=A+1|0):(i[A>>0]=107,(e-n|0)>3?(Df(A+1|0,-69,e-n-3|0),e=A+1+(e-n-3)|0):e=A+1|0,i[e>>0]=-70,e=e+1|0),If(0|e,0|r,0|n),i[e+n>>0]=-52,0|(r=1))}function qe(A,e,r,n,t){A|=0,e|=0,r|=0;var f=0;if((0|(t|=0))==(0|(n|=0))&&(-2&(f=0|i[r>>0]))<<24>>24==106){A:do{if(f<<24>>24==107){do{if((0|t)>3){n=0,f=r+1|0;e:for(;;){switch(r=f,f=f+1|0,0|i[r>>0]){case-70:e=11;break e;case-69:break;default:e=8;break e}if((t+-3|0)<=(0|(n=n+1|0))){e=10;break}}if(8==(0|e))return Xe(4,128,138,0,0),0|(A=-1);if(10==(0|e)){n=t+-3-n|0;break A}if(11==(0|e)){if(n){n=t+-3-n|0;break A}break}}}while(0);return Xe(4,128,138,0,0),0|(A=-1)}n=t+-2|0,f=r+1|0}while(0);return-52==(0|i[f+n>>0])?(If(0|A,0|f,0|n),0|(A=n)):(Xe(4,128,139,0,0),0|(A=-1))}return Xe(4,128,137,0,0),0|(A=-1)}function $e(A){switch(0|(A|=0)){case 64:A=51;break;case 672:A=52;break;case 673:A=54;break;case 674:A=53;break;default:A=-1}return 0|A}function Ar(A,e,r,i){A|=0,r|=0,i|=0;var n,f=0,a=0;n=G,G=G+16|0,f=(f=0|t[(e|=0)+16>>2])?0|Kf(f):0;do{if(0|Ie(A,r,128)){if(0|i&&0!=(0|t[e+24>>2])){if(t[n>>2]=f,(0|ge(A,55073,n))<1){f=0;break}f=55136,a=55118}else{if(t[n+8>>2]=f,(0|ge(A,55096,n+8|0))<1){f=0;break}f=55152,a=55127}if(0!=(0|IA(A,a,0|t[e+16>>2],0,r))&&0!=(0|IA(A,f,0|t[e+20>>2],0,r))){if(0|i){if(!(0|IA(A,55162,0|t[e+24>>2],0,r))){f=0;break}if(!(0|IA(A,55179,0|t[e+28>>2],0,r))){f=0;break}if(!(0|IA(A,55187,0|t[e+32>>2],0,r))){f=0;break}if(!(0|IA(A,55195,0|t[e+36>>2],0,r))){f=0;break}if(!(0|IA(A,55206,0|t[e+40>>2],0,r))){f=0;break}if(!(0|IA(A,55217,0|t[e+44>>2],0,r))){f=0;break}}f=1}else f=0}else f=0}while(0);return G=n,0|f}function er(A,e,r,i){e|=0,r|=0,i|=0;var n=0,f=0,a=0,B=0,o=0,u=0;if(0|(n=0|t[52+(0|t[(A|=0)+8>>2])>>2]))return 0|(u=0|MB[31&n](A,e,r,i));if((0|e)>=16){f=0|eB();A:do{if(0!=(0|f)&&(iB(f),a=0|tB(f),B=0|tB(f),o=0|tB(f),0!=(0|tB(f)))){if(0==(0|t[A+16>>2])&&(n=0|qf(),t[A+16>>2]=n,0==(0|n))){u=56;break}if(0==(0|t[A+24>>2])&&(n=0|$f(),t[A+24>>2]=n,0==(0|n))){u=56;break}if(0==(0|t[A+20>>2])&&(n=0|qf(),t[A+20>>2]=n,0==(0|n))){u=56;break}if(0==(0|t[A+28>>2])&&(n=0|$f(),t[A+28>>2]=n,0==(0|n))){u=56;break}if(0==(0|t[A+32>>2])&&(n=0|$f(),t[A+32>>2]=n,0==(0|n))){u=56;break}if(0==(0|t[A+36>>2])&&(n=0|$f(),t[A+36>>2]=n,0==(0|n))){u=56;break}if(0==(0|t[A+40>>2])&&(n=0|$f(),t[A+40>>2]=n,0==(0|n))){u=56;break}if(0==(0|t[A+44>>2])&&(n=0|$f(),t[A+44>>2]=n,0==(0|n))){u=56;break}if(0!=(0|ra(0|t[A+20>>2],r))&&(ha(0|t[A+28>>2],4),ha(0|t[A+32>>2],4),ha(o,4),0!=(0|Ka(0|t[A+28>>2],(e+1|0)/2|0,0,0,0,i)))){for(n=0;;){if(!(0|cn(o,r=0|t[A+28>>2],52964))){u=56;break A}if(xe(),0|Hf(B,o,0|t[A+20>>2],f))break;if(50331756!=(-16773121&(0|pe())|0)){u=56;break A}if(Oe(),!(0|za(i,2,n))){u=56;break A}if(!(0|Ka(0|t[A+28>>2],(e+1|0)/2|0,0,0,0,i))){u=56;break A}n=n+1|0}if(0|za(i,3,0)){for(;;){do{if(!(0|Ka(0|t[A+32>>2],e-((e+1|0)/2|0)|0,0,0,0,i))){u=56;break A}}while(!(0|ca(0|t[A+28>>2],0|t[A+32>>2])));if(!(0|cn(o,r=0|t[A+32>>2],52964))){u=56;break A}if(xe(),0|Hf(B,o,0|t[A+20>>2],f))break;if(50331756!=(-16773121&(0|pe())|0)){u=56;break A}if(Oe(),!(0|za(i,2,n))){u=56;break A}n=n+1|0}if(0|za(i,3,1))if((0|ca(0|t[A+28>>2],0|t[A+32>>2]))<0?(r=0|t[A+28>>2],n=0|t[A+32>>2],t[A+28>>2]=n,t[A+32>>2]=r):(n=0|t[A+28>>2],r=0|t[A+32>>2]),0|Sf(0|t[A+16>>2],n,r,f)){if(!(0|cn(B,i=0|t[A+28>>2],52964))){u=56;break}if(!(0|cn(o,i=0|t[A+32>>2],52964))){u=56;break}if(!(0|Sf(a,B,o,f))){u=56;break}if(!(n=0|qf())){u=56;break}if(Da(n,a,4),a=0==(0|Hf(0|t[A+24>>2],0|t[A+20>>2],n,f)),Pf(n),a){u=56;break}if(!(n=0|qf())){u=56;break}Da(n,0|t[A+24>>2],4);do{if(0|wB(0,0|t[A+36>>2],n,B,f)){if(!(0|wB(0,0|t[A+40>>2],n,o,f)))break;if(Pf(n),!(n=0|qf())){u=56;break A}if(Da(n,0|t[A+28>>2],4),A=0==(0|Hf(0|t[A+44>>2],0|t[A+32>>2],n,f)),Pf(n),A){u=56;break A}n=1;break A}}while(0);Pf(n),u=56}else u=56;else u=56}else u=56}else u=56}else u=56}while(0);56==(0|u)&&(Xe(4,129,3,0,0),n=0),f?nB(f):f=0}else Xe(4,129,120,0,0),f=0,n=0;return rB(f),0|(u=n)}function rr(A,e,r,i,n,t){return 0|ir(A|=0,e|=0,r|=0,i|=0,n|=0,t|=0,0,0)}function ir(A,e,r,n,t,f,a,B){A|=0,r|=0,n|=0,t|=0,f|=0;var o,u,l=0;if(u=G,G=G+64|0,(a|=0)||(a=52916),l=0==(0|(B|=0))?a:B,((e|=0)+-1-((o=0|pi(a))<<1)|0)<=(0|n))return Xe(4,154,110,0,0),G=u,0|(A=0);if((1|o<<1)>=(0|e))return Xe(4,154,120,0,0),G=u,0|(A=0);i[A>>0]=0;do{if(0!=(0|Ui(t,f,A+o+1|0,0,a,0))&&(Df(A+o+1+o|0,0,e+-1-n-1-(o<<1)|0),i[A+o+1+(e+-1-n-1-o)>>0]=1,If(A+o+1+(e+-1)+(0-n)+(0-o)|0,0|r,0|n),(0|Nt(A+1|0,o))>=1)){if(!(n=0|Ln(a=e+-1-o|0,82796,0))){Xe(4,154,65,0,0),n=0,B=0;break}if((0|nr(n,a,A+1|0,o,l))>=0){if((0|a)>0){B=0;do{i[(e=A+o+1+B|0)>>0]=i[e>>0]^i[n+B>>0],B=B+1|0}while((0|B)!=(0|a))}if((0|nr(u,o,A+o+1|0,a,l))>=0)if((0|o)>0){B=0;do{i[(l=A+1+B|0)>>0]=i[l>>0]^i[u+B>>0],B=B+1|0}while((0|B)!=(0|o));B=1}else B=1;else B=0}else B=0}else n=0,a=0,B=0}while(0);return $r(u,64),pn(n,a,82796,0),G=u,0|(A=B)}function nr(A,e,r,n,t){A|=0,e|=0,r|=0,n|=0,t|=0;var f,a,B=0,o=0,u=0,l=0;a=G,G=G+80|0,f=0|di();A:do{if(0!=(0|f)&&(o=0|pi(t),(0|o)>=0))if((0|e)>0){for(u=0,l=0;;){if(i[a+64>>0]=u>>>24,i[a+64+1>>0]=u>>>16,i[a+64+2>>0]=u>>>8,i[a+64+3>>0]=u,!(0|gi(f,t,0))){B=-1;break A}if(!(0|Di(f,r,n))){B=-1;break A}if(!(0|Di(f,a+64|0,4))){B=-1;break A}if((0|(B=l+o|0))>(0|e))break;if(!(0|Mi(f,A+l|0,0))){B=-1;break A}if(!((0|B)<(0|e))){B=0;break A}u=u+1|0,l=B}0|Mi(f,a,0)?(If(A+l|0,0|a,e-l|0),B=0):B=-1}else B=0;else B=-1}while(0);return $r(a,64),Ii(f),G=a,0|B}function tr(A,e,r,i,n,t,f){return 0|fr(A|=0,e|=0,r|=0,i|=0,n|=0,t|=0,f|=0,0,0)}function fr(A,e,r,n,a,B,o,u,l){A|=0,e|=0,r|=0,n|=0,a|=0,B|=0,o|=0;var c,E,Q=0,w=0,b=0,k=0,s=0,C=0,d=0;if(E=G,G=G+144|0,(u|=0)||(u=52916),k=0==(0|(l|=0))?u:l,c=0|pi(u),(0|e)<1|(0|n)<1)return G=E,0|(a=-1);if((0|a)>=(0|n)&&(2+(c<<1)|0)<=(0|a)){s=0|Ln(C=a-c-1|0,82796,0);do{if(s){if(!(Q=0|Ln(a,82796,0))){Xe(4,153,65,0,0),Q=0,l=0,u=-1;break}if((0|a)>0)for(w=Q+a|0,b=n,l=r+n|0,n=0;l=l+(0-(1&(r=~((b+-1&(-2147483648^b))>>31))))|0,i[(w=w+-1|0)>>0]=(0|f[l>>0])&r,(0|(n=n+1|0))!=(0|a);)b=b-(1&r)|0;else Q=Q+a|0;if(l=(0|f[Q>>0])-1>>31,0|nr(E+64|0,c,(r=Q+1|0)+c|0,C,k))u=-1;else{if((0|c)>0){n=0;do{i[(b=E+64+n|0)>>0]=i[b>>0]^i[r+n>>0],n=n+1|0}while((0|n)!=(0|c))}if(0|nr(s,C,E+64|0,c,k))u=-1;else{if((a-c|0)>1){n=0;do{i[(k=s+n|0)>>0]=i[k>>0]^i[r+c+n>>0],n=n+1|0}while((0|n)<(0|C))}if(0|Ui(B,o,E,0,u,0)){if(u=0|ui(s,E,c),(0|C)>(0|c)){n=0,l&=(u+-1&(-2147483648^u))>>31,r=c,u=0;do{k=(1^(o=0|f[s+r>>0]))-1>>31&~n,t[E+128>>2]=k,B=t[E+128>>2]&r,t[E+128>>2]=~k,u=t[E+128>>2]&u|B,l&=(n|=(1^o)-1>>31)|o+-1>>31,r=r+1|0}while((0|r)!=(0|C))}else n=0,l&=(u+-1&(-2147483648^u))>>31,u=0;if(l=n&l&~(((e-(u=a-c-2-u|0)^u|u^e)^e)>>31),B=C-c-1|0,t[E+128>>2]=((B-e^e|B^e)^B)>>31,k=t[E+128>>2]&B,t[E+128>>2]=~(((B-e^e|B^e)^B)>>31),k=t[E+128>>2]&e|k,(C-c|0)>2){b=1;do{if(r=b&B-u,(c+1|0)<(C-b|0)){n=a+-1-(c+b)|0,w=c+1|0;do{e=s+w|0,o=0|f[s+(w+b)>>0],d=0|f[e>>0],t[E+128>>2]=(r+-1&(-2147483648^r))>>31&255^255,o=t[E+128>>2]&o,t[E+128>>2]=(r+-1&(-2147483648^r))>>31|-256,i[e>>0]=t[E+128>>2]&d|o,w=w+1|0}while((0|w)!=(0|n))}b<<=1}while((0|b)<(0|B))}if((0|k)>0){n=0;do{d=A+n|0,B=255&l&(n-u|u)>>31,e=0|f[s+((n=n+1|0)+c)>>0],o=0|f[d>>0],t[E+128>>2]=B,e=t[E+128>>2]&e,t[E+128>>2]=~B,i[d>>0]=t[E+128>>2]&o|e}while((0|n)!=(0|k))}Xe(4,153,121,0,0),Pe(1&l)}else u=-1}}}else Xe(4,153,65,0,0),Q=0,l=0,u=-1}while(0);return $r(E+64|0,64),pn(s,C,82796,0),pn(Q,a,82796,0),t[E+128>>2]=l,d=t[E+128>>2]&u,t[E+128>>2]=~l,d=t[E+128>>2]|d,G=E,0|d}return Xe(4,153,121,0,0),G=E,0|(d=-1)}function ar(A,e,r,i,n,f){A|=0,e|=0,r|=0,i|=0,n|=0,f|=0;var a,B=0,o=0;if(a=G,G=G+16|0,t[a+4>>2]=0,t[a>>2]=0,0|(B=0|t[44+(0|t[f+8>>2])>>2]))return o=0|UB[7&B](A,e,r,i,n,f),G=a,0|o;do{if(114==(0|A)){if(36==(0|r)){t[a+4>>2]=36,B=36,A=0,o=8;break}return Xe(4,117,131,0,0),G=a,0|(o=0)}A=0==(0|Br(a,a+4|0,A,e,r)),r=0|t[a>>2],B=0|t[a+4>>2],A?e=0:(A=r,e=r,o=8)}while(0);do{if(8==(0|o)){if((0|B)>((0|dr(f))-11|0)){Xe(4,117,112,0,0),r=A,e=0;break}(0|(e=0|gr(B,e,i,f,1)))<1?(r=A,e=0):(t[n>>2]=e,r=A,e=1)}}while(0);return pn(r,B,82796,0),G=a,0|(o=e)}function Br(A,e,r,i,n){A|=0,e|=0,r|=0,i|=0,n|=0;var f;return f=G,G=G+48|0,t[f>>2]=0,t[f+40>>2]=f+32,r=0|Sr(r),t[f+32>>2]=r,r?0|function(A){A=(A|=0)?0|t[A+12>>2]:0;return 0|A}(r)?(t[f+24>>2]=5,t[f+24+4>>2]=0,t[f+32+4>>2]=f+24,t[f+40+4>>2]=f+8,t[f+8+8>>2]=i,t[f+8>>2]=n,(0|(r=0|function(A,e){return 0|TA(A|=0,e|=0,51952)}(f+40|0,f)))<0?(G=f,0|(e=0)):(t[A>>2]=t[f>>2],t[e>>2]=r,G=f,0|(e=1))):(Xe(4,146,116,0,0),G=f,0|(e=0)):(Xe(4,146,117,0,0),G=f,0|(e=0))}function or(A,e,r,n,f,a,B,o){A|=0,e|=0,r|=0,n|=0,f|=0,a|=0,B|=0,o|=0;var u,l;if(l=G,G=G+16|0,t[l+4>>2]=0,t[l>>2]=0,(0|dr(o))!=(0|B))return Xe(4,145,119,0,0),G=l,0|(B=0);u=0|Ln(B,82796,0);A:do{if(u)if((0|(a=0|Mr(B,a,u,o,1)))>=1){if(114==(0|A)){if(36!=(0|a)){Xe(4,145,104,0,0),o=0,r=0;break}if(0|n){r=u,a=(o=n)+36|0;do{i[o>>0]=0|i[r>>0],o=o+1|0,r=r+1|0}while((0|o)<(0|a));t[f>>2]=36,o=0,r=1;break}if(36!=(0|r)){Xe(4,145,131,0,0),o=0,r=0;break}if(!(0|Ht(u,e,36))){o=0,r=1;break}Xe(4,145,104,0,0),o=0,r=0;break}if(95==(0|A)&18==(0|a)&&4==(0|i[u>>0])&&16==(0|i[u+1>>0])){if(0|n){r=u+2|0,a=(o=n)+16|0;do{i[o>>0]=0|i[r>>0],o=o+1|0,r=r+1|0}while((0|o)<(0|a));t[f>>2]=16,o=0,r=1;break}if(16!=(0|r)){Xe(4,145,131,0,0),o=0,r=0;break}if(!(0|Ht(e,u+2|0,16))){o=0,r=1;break}Xe(4,145,104,0,0),o=0,r=0;break}do{if(0|n){if(!(o=0|Xi(0|pr(A)))){Xe(4,145,117,0,0),o=0,r=0;break A}if((o=0|pi(o))>>>0>a>>>0){Xe(4,145,143,0,0),o=0,r=0;break A}e=u+a+(0-o)|0,r=o;break}}while(0);if(A=0==(0|Br(l,l+4|0,A,e,r)),o=0|t[l+4>>2],A)r=0;else{if((0|o)==(0|a)&&0==(0|Ht(0|t[l>>2],u,a))){if(!n){o=a,r=1;break}If(0|n,0|e,0|r),t[f>>2]=r,o=a,r=1;break}Xe(4,145,104,0,0),r=0}}else o=0,r=0;else Xe(4,145,65,0,0),o=0,r=0}while(0);return pn(0|t[l>>2],o,82796,0),pn(u,B,82796,0),G=l,0|(B=r)}function ur(A,e,r,i,n,f){A|=0,e|=0,r|=0,i|=0,n|=0;var a;return(a=0|t[48+(0|t[(f|=0)+8>>2])>>2])?0|(A=0|UB[7&a](A,e,r,i,n,f)):0|(A=0|or(A,e,r,0,0,i,n,f))}function lr(A,e,r,n){A|=0,r|=0;var t=0;if(((e|=0)+-11|0)<(0|(n|=0)))return Xe(4,110,110,0,0),0|(r=0);if(i[A>>0]=0,i[A+1>>0]=2,(0|Nt(A+2|0,e+-11-n|0))<1)return 0|(r=0);A:do{if((e+-11-n|0)>0){t=0,A=A+2|0;e:for(;;){if(!(0|i[A>>0]))do{if((0|Nt(A,1))<1){A=0;break e}}while(!(0|i[A>>0]));if(A=A+1|0,(0|(t=t+1|0))>=(e+-11-n|0))break A}return 0|A}A=A+2|0}while(0);return i[(e=A)>>0]=3,i[e+1>>0]=3,i[e+2>>0]=3,i[e+3>>0]=3,i[e+4>>0]=3,i[e+4+1>>0]=3,i[e+4+2>>0]=3,i[e+4+3>>0]=3,i[A+8>>0]=0,If(A+9|0,0|r,0|n),0|(r=1)}function cr(A,e,r,n,a){A|=0,r|=0;var B,o,u=0,l=0,c=0,E=0,Q=0,w=0,b=0,k=0,s=0;if(o=G,G=G+16|0,(0|(e|=0))<1|(0|(n|=0))<1)return G=o,0|(A=-1);if((0|n)>(0|(a|=0))|(0|a)<11)return Xe(4,114,111,0,0),G=o,0|(A=-1);if(!(B=0|Ln(a,82796,0)))return Xe(4,114,65,0,0),G=o,0|(A=-1);for(l=B+a|0,c=n,n=r+n|0,r=0;n=n+(0-(1&(u=~((c+-1&(-2147483648^c))>>31))))|0,i[(l=l+-1|0)>>0]=(0|f[n>>0])&u,(0|(r=r+1|0))!=(0|a);)c=c-(1&u)|0;if(E=((255&(2^i[B+1>>0]))-1&(0|f[B>>0])-1)>>31,t[o>>2]=E,t[o>>2]=~E,c=107&t[o>>2],(0|a)>2){u=0,l=2,n=0,r=0;do{k=(w=(0|f[(b=B+l|0)>>0])-1>>31)&~u,t[o>>2]=k,Q=t[o>>2]&l,t[o>>2]=~k,r=t[o>>2]&r|Q,u|=w,n=((255&(3^i[b>>0]))-1>>31|u)&(1&u^1)+n,l=l+1|0}while((0|l)!=(0|a))}else n=0,r=0;if(k=(r+-10&(-2147483648^r))>>31,t[o>>2]=~k|~E,Q=c&t[o>>2],t[o>>2]=k&E,Q=113&t[o>>2]|Q,b=E&~k&~(w=(n+-8&(-2147483648^n))>>31),t[o>>2]=~(E&~k)|~w,Q=t[o>>2]&Q,t[o>>2]=E&~k&w,Q=115&t[o>>2]|Q,E=~(((e-(w=a+-1-r|0)^w|w^e)^e)>>31),t[o>>2]=E|~b,Q=t[o>>2]&Q,t[o>>2]=((e-w^w|w^e)^e)>>31&b,Q=109&t[o>>2]|Q,k=((a+-11-e^e|a+-11^e)^a+-11)>>31,t[o>>2]=k,c=t[o>>2]&a+-11,t[o>>2]=~k,c=t[o>>2]&e|c,(0|a)>12){u=1;do{if(n=u&a+-11-w,(0|(l=a-u|0))>11){r=11;do{k=B+r|0,e=0|f[B+(r+u)>>0],s=0|f[k>>0],t[o>>2]=(n+-1&(-2147483648^n))>>31&255^255,e=t[o>>2]&e,t[o>>2]=(n+-1&(-2147483648^n))>>31|-256,i[k>>0]=t[o>>2]&s|e,r=r+1|0}while((0|r)!=(0|l))}u<<=1}while((0|u)<(a+-11|0))}if((0|c)>0){n=0;do{s=A+n|0,l=b&E&255&(n-w|w)>>31,k=0|f[B+(n+11)>>0],e=0|f[s>>0],t[o>>2]=l,k=t[o>>2]&k,t[o>>2]=~l,i[s>>0]=t[o>>2]&e|k,n=n+1|0}while((0|n)!=(0|c))}return pn(B,a,82796,0),Xe(4,114,Q,0,0),Pe(b&E&1),t[o>>2]=b&E,s=t[o>>2]&w,t[o>>2]=~(b&E),s=t[o>>2]|s,G=o,0|s}function Er(A){z(A|=0,52600)}function Qr(A,e,r){return 0|hA(A|=0,e|=0,r|=0,52496)}function wr(A,e){return 0|TA(A|=0,e|=0,52496)}function br(A,e,r){return 0|hA(A|=0,e|=0,r|=0,52548)}function kr(){return 0|function(A){A|=0;var e=0;if(!(e=0|Sn(88,82796,0)))return Xe(4,106,65,0,0),0|(e=0);if(t[e+52>>2]=1,A=0|W(),t[e+84>>2]=A,!A)return Xe(4,106,65,0,0),Xn(e,82796,0),0|(e=0);if(A=52440,t[e+8>>2]=A,t[e+56>>2]=-1025&t[A+36>>2],!(0|function(A,e,r){e|=0,r|=0;var i,n=0,f=0,a=0,B=0;if(i=G,G=G+48|0,(A=A|0)>>>0>13)return Xe(15,113,7,0,0),G=i,0|(e=0);if(!((B=0!=(0|m(82276,14)))&0!=(0|t[20570])))return Xe(15,113,65,0,0),G=i,0|(e=0);if(!(n=0|t[20568]))return G=i,0|(e=0);R(n),t[r>>2]=0,B=0|Hn(0|t[81968+(A<<2)>>2]);do{if((0|B)>0){if((0|B)>=10){if(!(n=0|Ln(B<<2,82796,0))){n=0;break}}else n=i;f=0;do{t[n+(f<<2)>>2]=0|vn(0|t[81968+(A<<2)>>2],f),f=f+1|0}while((0|f)!=(0|B))}else n=0}while(0);if(y(0|t[20568]),(0|B)>0&0==(0|n))return Xe(15,112,65,0,0),G=i,0|(e=0);if((0|B)>0){a=0;do{0|(f=0|t[(A=n+(a<<2)|0)>>2])&&0|t[f+8>>2]&&(f=0!=(0|(f=0|t[r>>2]))&&(0|Hn(f))>(0|a)?0|vn(0|t[r>>2],a):0,A=0|t[A>>2],YB[0&t[A+8>>2]](e,f,r,a,0|t[A>>2],0|t[A+4>>2])),a=a+1|0}while((0|a)!=(0|B))}if((0|n)==(0|i))return G=i,0|(e=1);return Xn(n,82796,0),G=i,0|(e=1)}(9,e,e+48|0)))return sr(e),0|(e=0);return(A=0|t[28+(0|t[e+8>>2])>>2])?0|IB[31&A](e)?0|e:(Xe(4,106,70,0,0),sr(e),0|(e=0)):0|e}(0)}function sr(A){var e,r,i=0;r=G,G=G+16|0,(A|=0)?(T(A+52|0,-1,r,0|t[A+84>>2]),(0|t[r>>2])>0||(0|(e=0|t[A+8>>2])&&0|(i=0|t[e+32>>2])&&IB[31&i](A),bt(9,A,A+48|0),V(0|t[A+84>>2]),xf(0|t[A+16>>2]),xf(0|t[A+20>>2]),xf(0|t[A+24>>2]),xf(0|t[A+28>>2]),xf(0|t[A+32>>2]),xf(0|t[A+36>>2]),xf(0|t[A+40>>2]),xf(0|t[A+44>>2]),wn(0|t[A+76>>2]),wn(0|t[A+80>>2]),Xn(0|t[A+72>>2],82796,0),Xn(A,82796,0)),G=r):G=r}function Cr(A,e,r,i){A|=0,e|=0,r|=0,i|=0;do{if((0|i)<=(0|e)){if((0|i)<(0|e)){Xe(4,107,122,0,0),i=0;break}If(0|A,0|r,0|i),i=1;break}Xe(4,107,110,0,0),i=0}while(0);return 0|i}function dr(A){return 0|(7+(0|Kf(0|t[(A|=0)+16>>2]))|0)/8}function Ir(A,e,r,i,n){return A|=0,e|=0,r|=0,n|=0,0|hB[15&t[4+(0|t[(i|=0)+8>>2])>>2]](A,e,r,i,n)}function gr(A,e,r,i,n){return A|=0,e|=0,r|=0,n|=0,0|hB[15&t[12+(0|t[(i|=0)+8>>2])>>2]](A,e,r,i,n)}function Dr(A,e,r,i,n){return A|=0,e|=0,r|=0,n|=0,0|hB[15&t[16+(0|t[(i|=0)+8>>2])>>2]](A,e,r,i,n)}function Mr(A,e,r,i,n){return A|=0,e|=0,r|=0,n|=0,0|hB[15&t[8+(0|t[(i|=0)+8>>2])>>2]](A,e,r,i,n)}function hr(A,e){A|=0;var r=0,i=0,n=0,f=0,a=0,B=0,o=0;if(e|=0)r=e;else if(!(r=0|eB()))return 0|(A=0);iB(r);A:do{if(0|tB(r)){i=0|t[A+20>>2];e:do{if(!i){i=0|t[A+24>>2],n=0|t[A+28>>2],f=0|t[A+32>>2];do{if(!(0==(0|i)|0==(0|n)|0==(0|f))){if(iB(r),a=0|tB(r),B=0|tB(r),0|(o=0|tB(r))&&0|cn(B,n,52964)&&0|cn(o,f,52964)&&0|Sf(a,B,o,r)){if(i=0|Hf(0,i,a,r),nB(r),i)break e;break}nB(r)}}while(0);Xe(4,136,140,0,0),i=0,n=0;break A}}while(0);if(!(f=0|qf())){Xe(4,136,65,0,0),n=0;break}if(Da(f,0|t[A+16>>2],4),n=0|bn(0,i,f,r,0|t[24+(0|t[A+8>>2])>>2],0|t[A+60>>2]),Pf(f),n){Qn(n);break}Xe(4,136,3,0,0),n=0;break}Xe(4,136,65,0,0),i=0,n=0}while(0);return nB(r),(0|r)!=(0|e)&&rB(r),(0|i)==(0|t[A+20>>2])||Pf(i),0|(A=n)}function Ur(A,e,r,n,a,B){A|=0,e|=0,r|=0,n|=0,a|=0,B|=0;var o,u,l=0,c=0,E=0,Q=0,w=0;u=G,G=G+64|0,o=0|di();A:do{if(0!=(0|o)&&(E=0==(0|n)?r:n,w=0|pi(r),(0|w)>=0)){switch(0|B){case-1:B=w;break;case-2:B=-2;break;default:if((0|B)<-2){Xe(4,126,136,0,0),n=0,B=0;break A}}if(l=7+(0|Kf(0|t[A+16>>2]))&7,n=0|dr(A),255<<l&f[a>>0]|0){Xe(4,126,133,0,0),n=0,B=0;break}if(c=0==(0|l)?a+1|0:a,(n+((0==(0|l))<<31>>31)|0)<(w+2|0)){Xe(4,126,109,0,0),n=0,B=0;break}if((0|B)>((Q=n+((0==(0|l))<<31>>31)-w|0)+-2|0)){Xe(4,126,109,0,0),n=0,B=0;break}if(-68!=(0|i[c+(n+((0==(0|l))<<31>>31)+-1)>>0])){Xe(4,126,134,0,0),n=0,B=0;break}if(!(n=0|Ln(Q+-1|0,82796,0))){Xe(4,126,65,0,0),n=0,B=0;break}if((0|nr(n,Q+-1|0,c+(Q+-1)|0,w,E))>=0){if((0|Q)>1){A=0;do{i[(E=n+A|0)>>0]=i[E>>0]^i[c+A>>0],A=A+1|0}while((0|A)<(Q+-1|0))}0|l&&(i[n>>0]=255>>>(8-l|0)&f[n>>0]),a=0;do{A=0|i[n+a>>0],E=a,a=a+1|0}while((0|E)<(Q+-2|0)&A<<24>>24==0);if(A<<24>>24!=1){Xe(4,126,135,0,0),B=0;break}if(!((0|B)<0|(0|(A=Q+-1-a|0))==(0|B))){Xe(4,126,136,0,0),B=0;break}if(0!=(0|gi(o,r,0))&&0!=(0|Di(o,82788,8))&&0!=(0|Di(o,e,w))){if(0|A&&0==(0|Di(o,n+a|0,A))){B=0;break}0|Mi(o,u,0)?0|Ht(u,c+(Q+-1)|0,w)?(Xe(4,126,104,0,0),B=0):B=1:B=0}else B=0}else B=0}else n=0,B=0}while(0);return Xn(n,82796,0),Ii(o),G=u,0|B}function Gr(A,e,r,n,a,B){A|=0,e|=0,r|=0,n|=0,B|=0;var o,u,l=0,c=0,E=0,Q=0,w=0;u=0==(0|(a|=0))?n:a,o=0|pi(n);A:do{if((0|o)>=0){switch(0|B){case-1:B=o;break;case-2:B=-2;break;default:if((0|B)<-2){Xe(4,152,136,0,0),A=0,e=0,a=0;break A}}if(w=7+(0|Kf(0|t[A+16>>2]))&7,a=0|dr(A),w?(Q=e,E=a):(i[e>>0]=0,Q=e+1|0,E=a+-1|0),(0|E)<(o+2|0)){Xe(4,152,110,0,0),A=0,e=0,a=0;break}if(l=E-o|0,-2!=(0|B)){if((0|B)>(l+-2|0)){Xe(4,152,110,0,0),A=0,e=0,a=0;break}}else B=l+-2|0;e=(0|B)>0;do{if(e){if(a=0|Ln(B,82796,0)){if((0|Nt(a,B))<1){A=0,e=0;break A}break}Xe(4,152,65,0,0),A=0,e=0,a=0;break A}a=0}while(0);if(A=Q+(l+-1)|0,c=0|di())if(0!=(0|gi(c,n,0))&&0!=(0|Di(c,82788,8))&&0!=(0|Di(c,r,o))){if(0|B&&0==(0|Di(c,a,B))){A=c,e=0;break}if(0!=(0|Mi(c,A,0))&&0==(0|nr(Q,l+-1|0,A,o,u))){if(i[(A=Q+(-2-o+E-B)|0)>>0]=1^i[A>>0],e){e=0;do{i[(A=A+1|0)>>0]=i[A>>0]^i[a+e>>0],e=e+1|0}while((0|e)!=(0|B))}0|w&&(i[Q>>0]=255>>>(8-w|0)&(0|f[Q>>0])),i[Q+(E+-1)>>0]=-68,A=c,e=1}else A=c,e=0}else A=c,e=0;else A=0,e=0}else A=0,e=0,a=0}while(0);return Ii(A),pn(a,B,82796,0),0|e}function Zr(A,e,r,n){return A|=0,r|=0,((e|=0)+-11|0)<(0|(n|=0))?(Xe(4,108,110,0,0),0|(A=0)):(i[A>>0]=0,i[A+1>>0]=1,Df(A+2|0,-1,e+-3-n|0),i[(A=A+2+(e+-3-n)|0)>>0]=0,If(A+1|0,0|r,0|n),0|(A=1))}function Nr(A,e,r,n,t){A|=0,e|=0,r|=0,n|=0;var f=0,a=0,B=0,o=0;if((0|(t|=0))<11)return 0|(A=-1);do{if((0|t)==(0|n)){if(!(0|i[r>>0])){n=t+-1|0,r=r+1|0;break}return Xe(4,112,138,0,0),0|(A=-1)}}while(0);if((n+1|0)==(0|t)&&1==(0|i[r>>0])){a=n+-1|0,f=r+1|0;A:do{if((0|n)>1){t=0;e:for(;;){switch(0|i[f>>0]){case 0:B=12;break e;case-1:break;default:break e}if(r=f+1|0,!((0|(t=t+1|0))<(0|a)))break A;o=f,f=r,r=o}if(12==(0|B)){r=r+2|0;break}return Xe(4,112,102,0,0),0|(o=-1)}t=0,r=f}while(0);return(0|t)==(0|a)?(Xe(4,112,113,0,0),0|(o=-1)):t>>>0<8?(Xe(4,112,103,0,0),0|(o=-1)):(0|(n=n+-2-t|0))>(0|e)?(Xe(4,112,109,0,0),0|(o=-1)):(If(0|A,0|r,0|n),0|(o=n))}return Xe(4,112,106,0,0),0|(o=-1)}function Fr(A,e,r,n){A|=0,r|=0;var t=0;if(((e|=0)+-11|0)<(0|(n|=0)))return Xe(4,109,110,0,0),0|(r=0);if(i[A>>0]=0,i[A+1>>0]=2,(0|Nt(A+2|0,e+-3-n|0))<1)return 0|(r=0);A:do{if((e+-3-n|0)>0){t=0,A=A+2|0;e:for(;;){if(!(0|i[A>>0]))do{if((0|Nt(A,1))<1){A=0;break e}}while(!(0|i[A>>0]));if(A=A+1|0,(0|(t=t+1|0))>=(e+-3-n|0))break A}return 0|A}A=A+2|0}while(0);return i[A>>0]=0,If(A+1|0,0|r,0|n),0|(r=1)}function Wr(A,e,r,n,a){A|=0,r|=0;var B,o,u=0,l=0,c=0,E=0,Q=0,w=0,b=0;if(o=G,G=G+16|0,((n|=0)|(e|=0)|0)<0)return G=o,0|(A=-1);if((0|n)>(0|(a|=0))|(0|a)<11)return Xe(4,113,159,0,0),G=o,0|(A=-1);if(!(B=0|Ln(a,82796,0)))return Xe(4,113,65,0,0),G=o,0|(A=-1);for(l=B+a|0,c=n,n=r+n|0,r=0;n=n+(0-(1&(u=~((c+-1&(-2147483648^c))>>31))))|0,i[(l=l+-1|0)>>0]=(0|f[n>>0])&u,(0|(r=r+1|0))!=(0|a);)c=c-(1&u)|0;if(c=((255&(2^i[B+1>>0]))-1&(0|f[B>>0])-1)>>31,(0|a)>2)for(r=0,u=2,n=0;E=(l=(0|f[B+u>>0])-1>>31)&~r,t[o>>2]=E,Q=t[o>>2]&u,t[o>>2]=~E,n=t[o>>2]&n|Q,(0|(u=u+1|0))!=(0|a);)r|=l;else n=0;if(E=c&~((n+-10&(-2147483648^n))>>31)&~(((e-(Q=a+-1-n|0)^Q|Q^e)^e)>>31),l=((a+-11-e^e|a+-11^e)^a+-11)>>31,t[o>>2]=l,c=t[o>>2]&a+-11,t[o>>2]=~l,c=t[o>>2]&e|c,(0|a)>12){u=1;do{if(n=u&a+-11-Q,(0|(l=a-u|0))>11){r=11;do{e=B+r|0,w=0|f[B+(r+u)>>0],b=0|f[e>>0],t[o>>2]=(n+-1&(-2147483648^n))>>31&255^255,w=t[o>>2]&w,t[o>>2]=(n+-1&(-2147483648^n))>>31|-256,i[e>>0]=t[o>>2]&b|w,r=r+1|0}while((0|r)!=(0|l))}u<<=1}while((0|u)<(a+-11|0))}if((0|c)>0){n=0;do{b=A+n|0,l=255&E&(n-Q|Q)>>31,w=0|f[B+(n+11)>>0],e=0|f[b>>0],t[o>>2]=l,w=t[o>>2]&w,t[o>>2]=~l,i[b>>0]=t[o>>2]&e|w,n=n+1|0}while((0|n)!=(0|c))}return pn(B,a,82796,0),Xe(4,113,159,0,0),Pe(1&E),t[o>>2]=E,b=t[o>>2]&Q,t[o>>2]=~E,b=t[o>>2]|b,G=o,0|b}function Yr(A,e,r,i,n){e|=0,r|=0,i|=0,n|=0;var a,B,o=0,u=0;B=G,G=G+16|0,a=0|t[(A|=0)+20>>2],o=0|t[a+20>>2];A:do{if(o)switch(0|t[a+16>>2]){case 5:if(!(o=0|t[a+32>>2])&&(o=0|Ln(0|Zi(0|t[A+8>>2]),82796,0),t[a+32>>2]=o,!o))return G=B,0|(u=-1);if((0|(o=0|Mr(n,i,o,0|t[24+(0|t[A+8>>2])>>2],5)))<1)return G=B,0|(u=0);if(n=0|f[(0|t[a+32>>2])+(o+-1)>>0],(0|$e(0|Si(0|t[a+20>>2])))!=(0|n))return Xe(4,141,100,0,0),G=B,0|(u=0);if((o+-1|0)==(0|pi(0|t[a+20>>2]))){if(!e){o=o+-1|0;break A}If(0|e,0|t[a+32>>2],o+-1|0),o=o+-1|0;break A}return Xe(4,141,143,0,0),G=B,0|(u=0);case 1:if(!((0|or(0|Si(o),0,0,e,B,i,n,0|t[24+(0|t[A+8>>2])>>2]))<1)){o=0|t[B>>2],u=14;break A}return G=B,0|(o=0);default:return G=B,0|(u=-1)}else o=0|Mr(n,i,e,0|t[24+(0|t[A+8>>2])>>2],0|t[a+16>>2]),u=14}while(0);return 14==(0|u)&&(0|o)<0?(G=B,0|(u=o)):(t[r>>2]=o,G=B,0|(u=1))}function Rr(A,e){A|=0,e|=0;A:do{if(A)switch(A=0|Si(A),0|e){case 3:Xe(4,140,141,0,0),A=0;break A;case 5:if(-1!=(0|$e(A))){A=1;break A}Xe(4,140,142,0,0),A=0;break A;default:switch(0|A){case 117:case 95:case 257:case 3:case 114:case 4:case 674:case 673:case 672:case 675:case 64:A=1;break A}Xe(4,140,157,0,0),A=0;break A}else A=1}while(0);return 0|A}function yr(A,e,r,i,n,f){A|=0,e|=0,r|=0,i|=0,n|=0,f|=0;var a;return a=G,G=G+32|0,t[a+8+4>>2]=4,t[a+8>>2]=r,t[a+8+8>>2]=e,(0|(A=0|AA(a+8|0,0)))>((e=0|dr(f))+-11|0)?(Xe(4,118,112,0,0),G=a,0|(n=0)):(r=0|Ln(e+1|0,82796,0))?(t[a>>2]=r,AA(a+8|0,a),(0|(A=0|gr(A,r,i,f,1)))<1?A=0:(t[n>>2]=A,A=1),pn(r,e+1|0,82796,0),G=a,0|(n=A)):(Xe(4,118,65,0,0),G=a,0|(n=0))}function Vr(){var A,e=0,r=0;if(0|(A=0|v(82068))){if(0!=(0|(r=0|t[A>>2]))&&0!=(0|(e=0|Vn(r)))){do{Xn(0|t[e+8>>2],82796,0),Xn(e,82796,0),e=0|Vn(0|t[A>>2])}while(0!=(0|e));e=A}else e=A;Fn(0|t[e>>2]),Xn(A,82796,0),J(82068,0)}e=0|v(82064),0|J(82064,0)&&Xn(e,82796,0)}function mr(){var A=0,e=0;return 0|t[20518]||(t[20518]=52736),(e=0|Sn(40,82796,0))?(A=0|Wn(),t[e+4>>2]=A,A?(t[e>>2]=t[20518],t[e+8>>2]=1,A=0|W(),t[e+36>>2]=A,A?(A=0|t[28+(0|t[e>>2])>>2])?0|IB[31&A](e)?0|e:(Hr(e),0|(e=0)):0|e:(Xe(37,113,65,0,0),Fn(0|t[e+4>>2]),Xn(e,82796,0),0|(e=0))):(Xe(37,113,65,0,0),Xn(e,82796,0),0|(e=0))):(Xe(37,113,65,0,0),0|(e=0))}function Hr(A){var e,r=0;return e=G,G=G+16|0,(A|=0)?(0|T(A+8|0,-1,e,0|t[A+36>>2]))<1?(G=e,0|(A=0)):(0|t[e>>2])>0?(G=e,0|(A=1)):0==(4&t[A+12>>2]|0)&&0|(r=0|t[8+(0|t[A>>2])>>2])&&0==(0|IB[31&r](A))?(Xe(37,111,107,0,0),G=e,0|(A=0)):0|(r=0|t[32+(0|t[A>>2])>>2])&&0==(0|IB[31&r](A))?(Xe(37,111,102,0,0),G=e,0|(A=0)):(Fn(0|t[A+4>>2]),Xn(0|t[A+28>>2],82796,0),Xn(0|t[A+32>>2],82796,0),V(0|t[A+36>>2]),Xn(A,82796,0),G=e,0|(A=1)):(G=e,0|(A=1))}function vr(A,e,r,i){A|=0,e|=0,r|=0,i|=0;do{if(!A){if(0|(r=0|mr())){t[r+12>>2]=i,i=1,A=r;break}return Xe(37,112,65,0,0),0|(e=0)}i=0}while(0);r=A+28|0;do{if(0|t[r>>2])Xe(37,112,110,0,0);else{if(0|e){if(!(0|Jr(A,e))){Xe(37,112,112,0,0);break}if(0|t[r>>2]){if(!(r=0|t[4+(0|t[A>>2])>>2])){Xe(37,112,108,0,0);break}if(0|IB[31&r](A))return 0|(e=A);Xe(37,112,103,0,0);break}}Xe(37,112,111,0,0)}}while(0);return i?(Hr(A),0|(e=0)):0|(e=0)}function Jr(A,e){return 0==(0|(A|=0))|0==(0|(e|=0))?(Xe(37,129,67,0,0),0|(A=0)):0|t[A+32>>2]?(Xe(37,129,110,0,0),0|(A=0)):(e=0|kt(e,82796,0))?(Xn(0|t[A+28>>2],82796,0),t[A+28>>2]=e,0|(A=1)):(Xe(37,129,65,0,0),0|(A=0))}function Xr(A,e){var r=0;return 0==(0|(A|=0))|0==(0|(e|=0))?(Xe(37,108,67,0,0),0|(e=0)):(r=0|t[12+(0|t[A>>2])>>2])?0|(r=0|gB[63&r](A,e))?0|(e=r):(Xe(37,108,106,0,0),0|(e=0)):(Xe(37,108,108,0,0),0|(e=0))}function Tr(A){A|=0;var e,r=0;if(r=0|t[20522])return t[20525]=A,e=0|function(A){return 0|t[(A|=0)+36>>2]}(r),ve(0|t[20522],0),Ve(0|t[20522],12),r=0|t[20522],(0|A)<0?(Fe(r),mn(0|t[20524],13),V(0|t[20523]),t[20522]=0,void(t[20523]=0)):void ve(r,e)}function Lr(A){A|=0;var e=0,r=0,i=0,n=0,f=0,a=0,B=0;if(0==(0|t[20526])&&(B=0|Ze(19,30),t[20526]=B,0==(0|B)))return 0|(B=0);if(B=0|function(A){var e=0,r=0,i=0;if(!(A|=0))return 0|(i=0);if(!(1&t[A+20>>2]))return 0|(i=A);if(!(i=0|function(){var A=0;return(A=0|Sn(24,82796,0))?(t[A+20>>2]=1,0|A):(Xe(13,123,65,0,0),0|(A=0))}()))return Xe(8,101,13,0,0),0|(i=0);t[i+20>>2]=13|t[A+20>>2],(0|(e=0|t[A+12>>2]))>0?(e=0|Ct(0|t[A+16>>2],e,82796,0),t[i+16>>2]=e,0|e&&(e=0|t[A+12>>2],r=8)):r=8;do{if(8==(0|r)){if(t[i+12>>2]=e,t[i+8>>2]=t[A+8>>2],0|(e=0|t[A+4>>2])&&(r=0|kt(e,82796,0),t[i+4>>2]=r,0==(0|r)))break;if(!(e=0|t[A>>2]))return 0|i;if(A=0|kt(e,82796,0),t[i>>2]=A,0|A)return 0|i}}while(0);return $(i),Xe(8,101,65,0,0),0|(i=0)}(A)){e=0|Ln(8,82796,0);do{if(e){if(0!=(0|t[B+12>>2])&&0!=(0|t[A+16>>2])){if(!(A=0|Ln(8,82796,0))){r=0,A=0;break}a=A}else a=0,A=0;if(0|t[B>>2]){if(!(i=0|Ln(8,82796,0))){r=0;break}r=i,f=i}else r=0,f=0;if(0|t[B+4>>2]){if(!(i=0|Ln(8,82796,0)))break;n=i}else n=0,i=0;return 0|a&&(t[a>>2]=0,t[a+4>>2]=B,Xn(0|We(0|t[20526],A),82796,0)),0|f&&(t[f>>2]=1,t[f+4>>2]=B,Xn(0|We(0|t[20526],r),82796,0)),0|n&&(t[n>>2]=2,t[n+4>>2]=B,Xn(0|We(0|t[20526],i),82796,0)),t[e>>2]=3,t[e+4>>2]=B,Xn(0|We(0|t[20526],e),82796,0),t[B+20>>2]=-14&t[B+20>>2],0|(B=0|t[B+8>>2])}r=0,A=0}while(0);Xe(8,105,65,0,0)}else r=0,e=0,A=0;return Xn(A,82796,0),Xn(r,82796,0),Xn(0,82796,0),Xn(e,82796,0),$(B),0|(B=0)}function Sr(A){A|=0;var e,r=0;e=G,G=G+32|0;do{if(A>>>0>=1061){if(r=0|t[20526]){if(t[e+24>>2]=3,t[e+24+4>>2]=e,t[e+8>>2]=A,r=0|ye(r,e+24|0)){r=0|t[r+4>>2];break}Xe(8,103,101,0,0),r=0;break}r=0}else{if(0|A&&0==(0|t[1888+(24*A|0)+8>>2])){Xe(8,103,101,0,0),r=0;break}r=1888+(24*A|0)|0}}while(0);return G=e,0|r}function pr(A){A|=0;var e,r=0;e=G,G=G+32|0;do{if(A>>>0>=1061){if(r=0|t[20526]){if(t[e+24>>2]=3,t[e+24+4>>2]=e,t[e+8>>2]=A,r=0|ye(r,e+24|0)){r=0|t[t[r+4>>2]>>2];break}Xe(8,104,101,0,0),r=0;break}r=0}else{if(0|A&&0==(0|t[1888+(24*A|0)+8>>2])){Xe(8,104,101,0,0),r=0;break}r=0|t[1888+(24*A|0)>>2]}}while(0);return G=e,0|r}function zr(A){var e,r=0,i=0,n=0,f=0,a=0,B=0,o=0;if(e=G,G=G+16|0,!(A|=0))return G=e,0|(A=0);if(0|(r=0|t[A+8>>2]))return G=e,0|(A=r);if(!(r=0|t[A+12>>2]))return G=e,0|(A=0);i=0|t[20526];A:do{if(i){if(t[e>>2]=0,t[e+4>>2]=A,0|(r=0|ye(i,e)))return A=0|t[8+(0|t[r+4>>2])>>2],G=e,0|A;if(!(r=0|t[A+12>>2])){for(f=956,a=0;;){if(r=0|t[34128+((n=(a+f|0)/2|0)<<2)>>2],!(i=0|t[1888+(24*r|0)+12>>2]))break A;if((0|(a=(0|i)>0?a:n+1|0))>=(0|(f=(0|i)>0?n:f))){r=0;break}}return G=e,0|r}B=r,o=11}else B=r,o=11}while(0);A:do{if(11==(0|o)){for(i=956,a=0;;){if(r=0|t[34128+((f=(a+i|0)/2|0)<<2)>>2],(n=B-(0|t[1888+(24*r|0)+12>>2])|0)||(n=0|Ht(0|t[A+16>>2],0|t[1888+(24*r|0)+16>>2],B)),(0|n)<0)i=f,r=a;else{if(!n)break A;r=f+1|0}if(!((0|r)<(0|i))){r=0;break}a=r}return G=e,0|r}}while(0);return A=0|t[1888+(24*r|0)+8>>2],G=e,0|A}function Kr(A,e){A|=0,e|=0;var r,i=0,n=0,f=0,a=0,B=0;r=G,G=G+64|0;A:do{if(!e){t[r+32>>2]=A,e=0|t[20526];do{if(e){if(t[r+8>>2]=1,t[r+8+4>>2]=r+32,e=0|ye(e,r+8|0)){e=8+(0|t[e+4>>2])|0,n=13;break}B=0|t[r+32>>2],n=6;break}B=A,n=6}while(0);do{if(6==(0|n)){for(i=1052,e=0;;){if(f=0|t[37952+((a=(e+i|0)/2|0)<<2)>>2],(0|(n=0|Wt(B,0|t[1888+(24*f|0)>>2])))<0)i=a;else{if(!n){n=12;break}e=a+1|0}if((0|e)>=(0|i)){n=11;break}}if(11==(0|n)){n=14;break}if(12==(0|n)){e=1888+(24*f|0)+8|0,n=13;break}}}while(0);if(13==(0|n)&&((i=0|t[e>>2])||(n=14)),14==(0|n)){t[r+32+4>>2]=A,e=0|t[20526];do{if(e){if(t[r+8>>2]=2,t[r+8+4>>2]=r+32,e=0|ye(e,r+8|0)){e=8+(0|t[e+4>>2])|0;break}B=0|t[r+32+4>>2],n=18;break}B=A,n=18}while(0);do{if(18==(0|n)){for(i=1052,e=0;;){if(f=0|t[42160+((a=(e+i|0)/2|0)<<2)>>2],(0|(n=0|Wt(B,0|t[1888+(24*f|0)+4>>2])))<0)i=a;else{if(!n){n=24;break}e=a+1|0}if((0|e)>=(0|i)){n=23;break}}if(23==(0|n))break A;if(24==(0|n)){e=1888+(24*f|0)+8|0;break}}}while(0);if(!(e=0|t[e>>2]))break;i=e}do{if(i>>>0<1061){if(0|t[1888+(24*i|0)+8>>2]){e=1888+(24*i|0)|0;break}Xe(8,103,101,0,0),e=0;break}if(e=0|t[20526]){if(t[r+32>>2]=3,t[r+32+4>>2]=r+8,t[r+8+8>>2]=i,e=0|ye(e,r+32|0)){e=0|t[e+4>>2];break}Xe(8,103,101,0,0),e=0;break}e=0}while(0);return G=r,0|(A=e)}}while(0);return(0|(n=0|P(0,0,A,-1)))<1||(0|(e=0|BA(0,n,6)))<0?(G=r,0|(A=0)):(i=0|Ln(e,82796,0))?(t[r+4>>2]=i,fA(r+4|0,0,n,6,0),P(0|t[r+4>>2],n,A,-1),t[r>>2]=i,A=0|function(A,e,r){var i;if(A|=0,e|=0,r|=0,i=G,G=G+16|0,t[i+12>>2]=t[e>>2],128&(0|tA(i+12|0,i+8|0,i+4|0,i,r)))A=102;else{if(6==(0|t[i+4>>2]))return(A=0|q(A,i+12|0,0|t[i+8>>2]))?(t[e>>2]=t[i+12>>2],G=i,0|(e=A)):(G=i,0|(e=0));A=116}return Xe(13,147,A,0,0),G=i,0|(e=0)}(0,r,e),Xn(i,82796,0),G=r,0|A):(G=r,0|(A=0))}function jr(A){A|=0;var e,r=0,i=0,n=0,f=0,a=0,B=0;e=G,G=G+32|0,t[e+8>>2]=A,r=0|t[20526];do{if(r){if(t[e>>2]=1,t[e+4>>2]=e+8,!(r=0|ye(r,e))){A=0|t[e+8>>2];break}return B=0|t[8+(0|t[r+4>>2])>>2],G=e,0|B}}while(0);for(i=1052,r=0;;){if(n=0|t[37952+((a=(r+i|0)/2|0)<<2)>>2],(0|(f=0|Wt(A,0|t[1888+(24*n|0)>>2])))<0)i=a;else{if(!f)break;r=a+1|0}if((0|r)>=(0|i)){r=0,B=11;break}}return 11==(0|B)?(G=e,0|r):(B=0|t[1888+(24*n|0)+8>>2],G=e,0|B)}function xr(A){A|=0;var e,r=0,i=0,n=0,f=0,a=0,B=0;e=G,G=G+32|0,t[e+8+4>>2]=A,r=0|t[20526];do{if(r){if(t[e>>2]=2,t[e+4>>2]=e+8,!(r=0|ye(r,e))){A=0|t[e+8+4>>2];break}return B=0|t[8+(0|t[r+4>>2])>>2],G=e,0|B}}while(0);for(i=1052,r=0;;){if(n=0|t[42160+((a=(r+i|0)/2|0)<<2)>>2],(0|(f=0|Wt(A,0|t[1888+(24*n|0)+4>>2])))<0)i=a;else{if(!f)break;r=a+1|0}if((0|r)>=(0|i)){r=0,B=11;break}}return 11==(0|B)?(G=e,0|r):(B=0|t[1888+(24*n|0)+8>>2],G=e,0|B)}function Or(A,e,r,n){r|=0,n|=0;var f,a=0,B=0,u=0,l=0,c=0,E=0,Q=0,w=0,b=0,k=0;if(f=G,G=G+80|0,0!=(0|(A|=0))&(0|(e|=0))>0&&(i[A>>0]=0),!r)return G=f,0|(k=0);if(!(0|t[r+16>>2]))return G=f,0|(k=0);A:do{if(0==(0|n)&&(a=0|zr(r),0|a)){do{if(a>>>0<1061){if(0|t[1888+(24*a|0)+8>>2]){n=1888+(24*a|0)+4|0,k=16;break}Xe(8,102,101,0,0),k=15;break}if(n=0|t[20526]){if(t[f+64>>2]=3,t[f+64+4>>2]=f+40,t[f+40+8>>2]=a,n=0|ye(n,f+64|0)){n=4+(0|t[n+4>>2])|0,k=16;break}Xe(8,102,101,0,0),k=15;break}k=15}while(0);15==(0|k)?k=17:16==(0|k)&&((n=0|t[n>>2])||(k=17));do{if(17==(0|k)){do{if(a>>>0<1061){if(0|t[1888+(24*a|0)+8>>2]){n=1888+(24*a|0)|0,k=26;break}Xe(8,104,101,0,0),k=25;break}if(n=0|t[20526]){if(t[f+64>>2]=3,t[f+64+4>>2]=f+40,t[f+40+8>>2]=a,n=0|ye(n,f+64|0)){n=0|t[n+4>>2],k=26;break}Xe(8,104,101,0,0),k=25;break}k=25}while(0);if(25==(0|k))break A;if(26==(0|k)){if(n=0|t[n>>2])break;break A}}}while(0);return 0|A&&dt(A,n,e),k=0|mt(n),G=f,0|k}}while(0);a=0|t[r+12>>2];A:do{if((0|a)>0){n=0,Q=1,B=a,a=0,r=0|t[r+16>>2];e:for(;!(0==(0|(B=B+-1|0))&(u=0|i[r>>0])<<24>>24<0);){for(c=u,w=B,B=0,b=r,l=u<<24>>24<0,r=0;;){if(b=b+1|0,u=127&c,E=0==(0|r))B|=u;else if(!(0|cB(n,u)))break e;if(!l)break;if(E&B>>>0>33554431){if(!n&&!(n=0|qf())){n=0;break e}if(!(0|ta(n,B)))break e;r=1,k=43}else E?(B<<=7,r=0):k=43;if(43==(0|k)&&(k=0,!(0|rn(n,n,7))))break e;if(0==(0|(u=w+-1|0))&(l=(c=0|i[b>>0])<<24>>24<0))break e;w=u}if(Q){do{if(B>>>0>79){if(E){u=2,B=B+-80|0;break}if(0|EB(n,80)){u=2;break}break e}u=Q=(B>>>0)/40|0,B=(0|o(Q,-40))+B|0}while(0);r=A+1|0,(0|e)>1&0!=(0|A)&&(i[A>>0]=u+48,i[r>>0]=0,A=r,e=e+-1|0),a=a+1|0}if(E){t[f+32>>2]=B,Ge(f,26,79257,f+32|0),B=0|mt(f);do{if((0|e)>0&0!=(0|A)){if(dt(A,f,e),(0|e)<(0|B)){A=A+e|0,e=0;break}A=A+B|0,e=e-B|0;break}}while(0);a=B+a|0}else{if(!(r=0|tn(n)))break;u=0|mt(r);do{if(A){if(B=A+1|0,(0|e)>1&&(i[A>>0]=46,i[B>>0]=0,A=B,e=e+-1|0),dt(A,r,e),(0|e)<(0|u)){A=A+e|0,e=0;break}A=A+u|0,e=e-u|0;break}A=0}while(0);Xn(r,82796,0),a=a+1+u|0}if(!((0|w)>0))break A;Q=0,B=w,r=b}return Pf(n),G=f,0|(k=-1)}n=0,a=0}while(0);return Pf(n),G=f,0|(k=a)}function Pr(A,e,r,i,n){A|=0,e|=0,i|=0,n|=0;var t=0,f=0,a=0,B=0;if((0|(r|=0))<=0)return 0|(i=0);for(t=r,B=0;;){if(r=e+(0|o(a=(B+t|0)/2|0,i))|0,(0|(f=0|gB[63&n](A,r)))<0)t=a,r=B;else{if(!f){t=7;break}r=a+1|0}if(!((0|r)<(0|t))){r=0,t=7;break}B=r}return 7==(0|t)?0|r:0}function _r(A,e,r,i,n,t){A|=0,e|=0,i|=0,n|=0,t|=0;var f=0,a=0,B=0,u=0;if(!(r|=0))return 0|(i=0);A:do{if((0|r)>0){for(u=r,B=0;;){if(f=e+(0|o(r=(u+B|0)/2|0,i))|0,(0|(a=0|gB[63&n](A,f)))<0)a=r;else{if(!a){a=0;break A}a=u,B=r+1|0}if(!((0|B)<(0|a)))break;u=a}if(!(1&t))return 0|(i=0);a=1}else a=0,r=0,f=0}while(0);if(0==(2&t|0)|a)return 0|(i=f);A:do{if((0|r)>0)for(;;){if(t=e+(0|o(f=r+-1|0,i))|0,0|gB[63&n](A,t))break A;if(!((0|r)>1)){r=f;break}r=f}}while(0);return 0|(i=e+(0|o(r,i))|0)}function qr(A,e,r){A|=0,e|=0,r|=0;var i,n=0,f=0,a=0,B=0,o=0,u=0;i=G,G=G+32|0;A:do{if(e){t[i+8>>2]=e,n=0|t[20526];do{if(n){if(t[i>>2]=1,t[i+4>>2]=i+8,n=0|ye(n,i)){n=8+(0|t[n+4>>2])|0;break}u=0|t[i+8>>2],f=6;break}u=e,f=6}while(0);do{if(6==(0|f)){for(f=1052,n=0;;){if(B=0|t[37952+((o=(n+f|0)/2|0)<<2)>>2],(0|(a=0|Wt(u,0|t[1888+(24*B|0)>>2])))<0)f=o;else{if(!a){f=12;break}n=o+1|0}if((0|n)>=(0|f)){f=11;break}}if(11==(0|f)){f=14;break A}if(12==(0|f)){n=1888+(24*B|0)+8|0;break}}}while(0);0|t[n>>2]||(f=14)}else f=14}while(0);A:do{if(14==(0|f)){e:do{if(0|r){t[i+8+4>>2]=r,n=0|t[20526];do{if(n){if(t[i>>2]=2,t[i+4>>2]=i+8,n=0|ye(n,i)){n=8+(0|t[n+4>>2])|0;break}u=0|t[i+8+4>>2],f=19;break}u=r,f=19}while(0);do{if(19==(0|f)){for(f=1052,n=0;;){if(B=0|t[42160+((o=(n+f|0)/2|0)<<2)>>2],(0|(a=0|Wt(u,0|t[1888+(24*B|0)+4>>2])))<0)f=o;else{if(!a){f=25;break}n=o+1|0}if((0|n)>=(0|f)){f=24;break}}if(24==(0|f))break e;if(25==(0|f)){n=1888+(24*B|0)+8|0;break}}}while(0);if(0|t[n>>2])break A}}while(0);return(f=0|Kr(A,1))?(0|zr(f)?(Xe(8,100,102,0,0),n=0):(n=0|t[13195],t[13195]=n+1,t[f+8>>2]=n,t[f>>2]=e,t[f+4>>2]=r,n=0|Lr(f),t[f>>2]=0,t[f+4>>2]=0),$(f),G=i,0|(e=n)):(G=i,0|(e=0))}}while(0);return Xe(8,100,102,0,0),G=i,0|(e=0)}function $r(A,e){A|=0,e|=0,DB[31&t[13196]](A,0,e)}function Ai(A,e,r){return 0|(0|Ln(A|=0,e|=0,r|=0))}function ei(A,e,r){e|=0,r|=0;(A|=0)&&Xn(A,e,r)}function ri(A,e,r,i){e|=0,r|=0,i|=0;(A|=0)&&($r(A,e),Xn(A,r,i))}function ii(A,e,r){A|=0,e|=0;var i,n=0,f=0,a=0;i=0==(0|t[(r|=0)>>2]),A?(A=0|kt(A,82796,0))?a=3:(f=0,A=0,e=0,a=11):(A=0,a=3);do{if(3==(0|a)){if(e){if(!(e=0|kt(e,82796,0))){f=0,e=0,a=11;break}}else e=0;if(f=0|Ln(12,82796,0)){if(i&&(a=0|Wn(),t[r>>2]=a,0==(0|a))){Xe(34,105,65,0,0),n=r,a=13;break}if(n=r,t[f>>2]=0,t[f+4>>2]=A,t[f+8>>2]=e,0|yn(0|t[n>>2],f))return 0|(r=1);a=11}else f=0,a=11}}while(0);return 11==(0|a)&&(Xe(34,105,65,0,0),i&&(n=r,a=13)),13==(0|a)&&(Fn(0|t[n>>2]),t[r>>2]=0),Xn(f,82796,0),Xn(A,82796,0),Xn(e,82796,0),0|(r=0)}function ni(A,e){e|=0;var r,i=0;r=G,G=G+32|0,i=0|t[(A|=0)+8>>2];do{if(0|i){if(0|Wt(i,80391)&&0|Wt(i,80396)&&0|Wt(i,80401)&&0|Wt(i,80403)&&0|Wt(i,80405)&&0|Wt(i,80409)){if(0|Wt(i,80385)&&0|Wt(i,80413)&&0|Wt(i,80419)&&0|Wt(i,80421)&&0|Wt(i,80423)&&0|Wt(i,80426))break;return t[e>>2]=0,G=r,0|(A=1)}return t[e>>2]=255,G=r,0|(A=1)}}while(0);return Xe(34,110,104,0,0),i=0|t[A>>2],e=0|t[A+4>>2],A=0|t[A+8>>2],t[r>>2]=80429,t[r+4>>2]=i,t[r+8>>2]=80438,t[r+12>>2]=e,t[r+16>>2]=80445,t[r+20>>2]=A,Ke(6,r),G=r,0|(A=0)}function ti(A){A|=0;var e,r=0,n=0,a=0,B=0,o=0,u=0,l=0,c=0;e=G,G=G+16|0,t[e>>2]=0,c=0|kt(A,82796,0);A:do{if(c){u=0,o=c,a=c,n=1;e:for(;;){switch((A=0|i[o>>0])<<24>>24){case 10:case 13:case 0:l=45;break e}r:do{switch(0|n){case 1:switch(A<<24>>24){case 58:if(i[o>>0]=0,!((A=0|i[a>>0])<<24>>24)){l=17;break e}for(r=a;0|Yt(255&A);)if(!((A=0|i[(r=r+1|0)>>0])<<24>>24)){l=17;break e}if(((n=r+(0|mt(r))|0)+-1|0)!=(0|r)){for(a=n,B=n+-1|0;;){if(!(0|Yt(0|f[B>>0]))){l=15;break}if((0|(n=B+-1|0))==(0|r))break;a=B,B=n}if(15==(0|l)&&(l=0,i[a>>0]=0,A=0|i[r>>0]),0==(0|r)|A<<24>>24==0){l=17;break e}}A=o+1|0,n=2;break r;case 44:if(i[o>>0]=0,!((A=0|i[a>>0])<<24>>24)){l=29;break e}for(r=a;0|Yt(255&A);)if(!((A=0|i[(r=r+1|0)>>0])<<24>>24)){l=29;break e}if(((n=r+(0|mt(r))|0)+-1|0)!=(0|r)){for(a=n,B=n+-1|0;;){if(!(0|Yt(0|f[B>>0]))){l=27;break}if((0|(n=B+-1|0))==(0|r))break;a=B,B=n}if(27==(0|l)&&(l=0,i[a>>0]=0,A=0|i[r>>0]),0==(0|r)|A<<24>>24==0){l=29;break e}}ii(r,0,e),A=o+1|0,n=1;break r;default:r=u,A=a,n=1;break r}case 2:if(A<<24>>24==44){if(i[o>>0]=0,!((A=0|i[a>>0])<<24>>24)){l=42;break e}for(r=a;0|Yt(255&A);)if(!((A=0|i[(r=r+1|0)>>0])<<24>>24)){l=42;break e}if(((n=r+(0|mt(r))|0)+-1|0)!=(0|r)){for(a=n,B=n+-1|0;;){if(!(0|Yt(0|f[B>>0]))){l=40;break}if((0|(n=B+-1|0))==(0|r))break;a=B,B=n}if(40==(0|l)&&(l=0,i[a>>0]=0,A=0|i[r>>0]),0==(0|r)|A<<24>>24==0){l=42;break e}}ii(u,r,e),r=0,A=o+1|0,n=1}else r=u,A=a,n=2;break;default:r=u,A=a}}while(0);u=r,o=o+1|0,a=A}if(17==(0|l)){Xe(34,109,108,0,0);break}if(29==(0|l)){Xe(34,109,108,0,0);break}if(42==(0|l)){Xe(34,109,109,0,0);break}if(45==(0|l)){o=2==(0|n),A=0|i[a>>0];e:do{if(A<<24>>24){for(r=a;0|Yt(255&A);)if(!((A=0|i[(r=r+1|0)>>0])<<24>>24)){A=0;break e}n=r+(0|mt(r))|0;r:do{if((n+-1|0)!=(0|r)){for(a=n,B=n+-1|0;0|Yt(0|f[B>>0]);){if((0|(n=B+-1|0))==(0|r))break r;a=B,B=n}i[a>>0]=0,A=0|i[r>>0]}}while(0);A=A<<24>>24==0?0:r}else A=0}while(0);r=0==(0|A);do{if(o){if(r){Xe(34,109,109,0,0);break A}ii(u,A,e);break}if(r){Xe(34,109,108,0,0);break A}ii(A,0,e);break}while(0);return Xn(c,82796,0),c=0|t[e>>2],G=e,0|c}}else Xe(34,109,65,0,0)}while(0);return Xn(c,82796,0),mn(0|t[e>>2],8),G=e,0|(c=0)}function fi(A,e,r){A|=0,r|=0;var i,n,a=0;n=G,G=G+16|0,i=0|t[(e|=0)>>2],e=0|t[e+8>>2];A:do{if((0|i)>0){for(a=0;;){if(!((a>>>0)%18|0)){if((0|se(A,80479,1))<1){e=0,a=9;break}if((0|Ie(A,r,r))<1){e=0,a=9;break}}if(t[n>>2]=f[e+a>>0],a=a+1|0,t[n+4>>2]=(0|a)==(0|i)?82796:80481,(0|ge(A,80483,n))<1){e=0,a=9;break}if((0|a)>=(0|i))break A}if(9==(0|a))return G=n,0|e}}while(0);return A=1==(0|se(A,80479,1))&1,G=n,0|A}function ai(A,e){e|=0;var r,i=0;return r=G,G=G+16|0,(A|=0)?(i=0|Fi())?(function(A,e){e|=0;var r,i=0,n=0,f=0;if(r=G,G=G+16|0,!(A=A|0))return G=r,0|(A=0);t[r>>2]=0,(0|t[A+24>>2]&&0|(i=0|t[A+12>>2])?0|(n=0|t[i+88>>2]):0)&&(NB[31&n](A),t[A+24>>2]=0);6==(0|t[A+4>>2])&&0!=(0|t[A+12>>2])||(f=8);do{if(8==(0|f)){if(0|(i=0|gA(r,6))){t[A+12>>2]=i,t[A+16>>2]=t[r>>2],t[A>>2]=t[i>>2],t[A+4>>2]=6;break}return Xe(6,158,156,0,0),G=r,0|(A=0)}}while(0);if(t[A+24>>2]=e,!e)return G=r,0|(A=0);(function(A){var e;e=G,G=G+16|0,A=(0|T((A|=0)+52|0,1,e,0|t[A+84>>2]))>0,G=e,t[e>>2]})(e),G=r,A=1}(i,A),t[r>>2]=0,0|function(A,e){e|=0;var r,i=0;if(!(A|=0))return 0|(A=0);r=0|pA(52816);do{if(0|r){if(!(i=0|t[e+12>>2])){Xe(11,120,111,0,0);break}if(!(i=0|t[i+24>>2])){Xe(11,120,124,0,0);break}if(!(0|gB[63&i](r,e))){Xe(11,120,126,0,0);break}return z(0|t[A>>2],52816),t[A>>2]=r,t[r+8>>2]=e,Wi(e),0|(A=1)}}while(0);return z(r,52816),0|(A=0)}(r,i)?(A=0|TA(0|t[r>>2],e,52816),z(0|t[r>>2],52816)):A=0,yi(i),G=r,0|(i=A)):(Xe(13,165,65,0,0),G=r,0|(i=0)):(G=r,0|(i=0))}function Bi(A,e,r,i,n){return e|=0,r|=0,i|=0,n|=0,0|(A|=0)&&(t[A>>2]=t[t[n>>2]>>2]),0|e&&(t[e>>2]=t[8+(0|t[n+4>>2])>>2],t[r>>2]=t[t[n+4>>2]>>2]),i?(t[i>>2]=t[n>>2],1):1}function oi(A,e,r){C()}function ui(A,e,r){A|=0,e|=0,r|=0;var n,f,a;if(a=G,G=G+16|0,t[a+4>>2]=A,t[a>>2]=e,n=0|t[a+4>>2],f=0|t[a>>2],!r)return r=0,G=a,0|(r&=255);e=0,A=0;do{A=i[f+e>>0]^i[n+e>>0]|A,e=e+1|0}while((0|e)!=(0|r));return G=a,0|(r=255&A)}function li(A,e,r){A|=0,e|=0,r|=0;var i,n=0,f=0;i=G,G=G+112|0;do{if(-1==(0|r)){if(!A)return G=i,0|(f=0);if(r=0|t[A+12>>2]){r=0|t[r>>2];break}return G=i,0|(f=0)}}while(0);t[i>>2]=i+4,t[i+4>>2]=r,n=8;do{if(8==(0|n)){if(r=0|Pr(i,46464,4,4,31)){f=0|t[r>>2],n=11;break}break}}while(0);return 11==(0|n)&&0|f?(n=0|Sn(40,82796,0))?(t[n+4>>2]=e,t[n>>2]=f,t[n+16>>2]=0,t[n+8>>2]=A,0|A&&Wi(A),(r=0|t[f+8>>2])?(0|IB[31&r](n))>=1?(G=i,0|(f=n)):(t[n>>2]=0,yi(0|t[n+8>>2]),yi(0|t[n+12>>2]),Xn(n,82796,0),G=i,0|(f=0)):(G=i,0|(f=n))):(Xe(6,157,65,0,0),G=i,0|(f=0)):(Xe(6,157,156,0,0),G=i,0|(f=0))}function ci(A){var e,r=0;return(r=0|t[(A|=0)>>2])&&0|t[r+12>>2]?(e=0|Ln(40,82796,0))?(t[e>>2]=t[A>>2],(r=0|t[A+8>>2])&&(Wi(r),r=0|t[A+8>>2]),t[e+8>>2]=r,(r=0|t[A+12>>2])&&(Wi(r),r=0|t[A+12>>2]),t[e+12>>2]=r,t[e+20>>2]=0,t[e+24>>2]=0,t[e+16>>2]=t[A+16>>2],(0|gB[63&t[12+(0|t[A>>2])>>2]](e,A))>0?0|(A=e):(t[e>>2]=0,yi(0|t[e+8>>2]),yi(0|t[e+12>>2]),Xn(e,82796,0),0|(A=0))):(Xe(6,156,65,0,0),0|(A=0)):0|(A=0)}function Ei(A){var e,r=0;(A|=0)&&(0|(e=0|t[A>>2])&&0|(r=0|t[e+16>>2])&&NB[31&r](A),yi(0|t[A+8>>2]),yi(0|t[A+12>>2]),Xn(A,82796,0))}function Qi(A,e,r,i,n,f){e|=0,r|=0,i|=0,n|=0,f|=0;var a=0,B=0;return 0|(A|=0)&&0|(a=0|t[A>>2])&&0|(B=0|t[a+100>>2])?-1!=(0|e)&&(0|t[a>>2])!=(0|e)?0|(f=-1):(a=0|t[A+16>>2])?-1!=(0|r)&0==(a&r|0)?(Xe(6,137,148,0,0),0|(f=-1)):-2!=(0|(a=0|MB[31&B](A,i,n,f)))?0|(f=a):(Xe(6,137,147,0,0),0|(f=-2)):(Xe(6,137,149,0,0),0|(f=-1)):(Xe(6,137,147,0,0),0|(f=-2))}function wi(A,e,r){A|=0,e|=0;var i;return(0|(i=0|mt(r|=0)))<0?0|(r=-1):0|(r=0|MB[31&t[100+(0|t[A>>2])>>2]](A,e,i,r))}function bi(A,e,r){A|=0,e|=0;var i;return i=G,G=G+16|0,(r=0|Dt(r|=0,i))?(A=0|MB[31&t[100+(0|t[A>>2])>>2]](A,e,0|t[i>>2],r),Xn(r,82796,0),G=i,0|A):(G=i,0|(A=0))}function ki(A,e){e|=0,t[(A|=0)+20>>2]=e}function si(A){return 0|t[(A|=0)+20>>2]}function Ci(A){var e=0;return(A|=0)?(0|(e=0|t[A>>2])&&(0|t[e+32>>2]&&0==(0|xi(A,2))&&IB[31&t[32+(0|t[A>>2])>>2]](A),0|(e=0|t[A>>2])&&0|t[e+40>>2]&&0|t[A+12>>2]&&0==(0|xi(A,4))&&pn(0|t[A+12>>2],0|t[40+(0|t[A>>2])>>2],82796,0)),Ei(0|t[A+16>>2]),$r(A,24),1):1}function di(){return 0|Sn(24,82796,0)}function Ii(A){Ci(A|=0),Xn(A,82796,0)}function gi(A,e,r){e|=0,r|=0;var i=0,n=0;return function(A,e){e|=0,t[(A|=0)+8>>2]=t[A+8>>2]&~e}(A|=0,2),(0|(r=0|t[A>>2]))!=(0|e)&&(0|r&&0|(i=0|t[r+40>>2])&&(pn(0|t[A+12>>2],i,82796,0),t[A+12>>2]=0),t[A>>2]=e,0==(256&t[A+8>>2]|0)&&0|(n=0|t[e+40>>2])&&(t[A+20>>2]=t[e+20>>2],e=0|Sn(n,82796,0),t[A+12>>2]=e,0==(0|e)))?(Xe(6,128,65,0,0),0|(A=0)):0|(r=0|t[A+16>>2])&&!(-2==(0|(e=0|Qi(r,-1,248,7,0,A)))|(0|e)>0)?0|(A=0):256&t[A+8>>2]|0?0|(A=1):0|(A=0|IB[31&t[16+(0|t[A>>2])>>2]](A))}function Di(A,e,r){return e|=0,r|=0,0|DB[31&t[(A|=0)+20>>2]](A,e,r)}function Mi(A,e,r){e|=0,r|=0;var i,n=0;return n=0|t[(A|=0)>>2],(0|t[n+8>>2])>=65&&oi(),i=0|gB[63&t[n+24>>2]](A,e),n=0|t[A>>2],0|r&&(t[r>>2]=t[n+8>>2]),(e=0|t[n+32>>2])&&(IB[31&e](A),ji(A,2),n=0|t[A>>2]),$r(0|t[A+12>>2],0|t[n+40>>2]),0|i}function hi(A,e){A|=0;var r=0,i=0;if(0|(e|=0)&&0|(r=0|t[e>>2])){if((0|t[A>>2])==(0|r)?(r=0|t[A+12>>2],ji(A,4)):r=0,Ci(A),t[A>>2]=t[e>>2],t[A+4>>2]=t[e+4>>2],t[A+8>>2]=t[e+8>>2],t[A+12>>2]=t[e+12>>2],t[A+16>>2]=t[e+16>>2],t[A+20>>2]=t[e+20>>2],t[A+12>>2]=0,t[A+16>>2]=0,0|t[e+12>>2]&&0|(i=0|t[40+(0|t[A>>2])>>2])){do{if(!r){if(r=0|Ln(i,82796,0),t[A+12>>2]=r,0|r){i=0|t[40+(0|t[A>>2])>>2];break}return Xe(6,110,65,0,0),0|(A=0)}t[A+12>>2]=r}while(0);If(0|r,0|t[e+12>>2],0|i)}return t[A+20>>2]=t[e+20>>2],0|(r=0|t[e+16>>2])&&(i=0|ci(r),t[A+16>>2]=i,0==(0|i))?(Ci(A),0|(A=0)):(r=0|t[28+(0|t[A>>2])>>2])?0|(A=0|gB[63&r](A,e)):0|(A=1)}return Xe(6,110,111,0,0),0|(A=0)}function Ui(A,e,r,i,n,f){A|=0,e|=0,r|=0,i|=0,n|=0,f|=0;var a=0;return(a=0|Sn(24,82796,0))?(ji(a,1),0!=(0|gi(a,n,0))&&0!=(0|DB[31&t[a+20>>2]](a,A,e))?(f=0|t[a>>2],(0|t[f+8>>2])>=65&&oi(),A=0|gB[63&t[f+24>>2]](a,r),f=0|t[a>>2],0|i&&(t[i>>2]=t[f+8>>2]),(e=0|t[f+32>>2])&&(IB[31&e](a),ji(a,2),f=0|t[a>>2]),$r(0|t[a+12>>2],0|t[f+40>>2]),f=0!=(0|A)&1):f=0,Ci(a),Xn(a,82796,0),0|(a=f)):0|(a=0)}function Gi(A){var e=0,r=0;return 0|(e=0!=(0|(A|=0))&&0!=(0|(e=0|t[A+12>>2]))&&0!=(0|(r=0|t[e+52>>2]))?0|IB[31&r](A):0)}function Zi(A){var e=0,r=0;return 0|(e=0!=(0|(A|=0))&&0!=(0|(e=0|t[A+12>>2]))&&0!=(0|(r=0|t[e+48>>2]))?0|IB[31&r](A):0)}function Ni(A,e){A|=0,e|=0;var r,i=0,n=0;return r=G,G=G+16|0,t[r>>2]=0,0|A&&(0|t[A+24>>2]&&0|(i=0|t[A+12>>2])&&0|(n=0|t[i+88>>2])&&(NB[31&n](A),t[A+24>>2]=0),(0|t[A+4>>2])==(0|e)&&0|t[A+12>>2])?(G=r,0|(e=1)):(i=0|gA(r,e))?A?(t[A+12>>2]=i,t[A+16>>2]=t[r>>2],t[A>>2]=t[i>>2],t[A+4>>2]=e,G=r,0|(e=1)):(G=r,0|(e=1)):(Xe(6,158,156,0,0),G=r,0|(e=0))}function Fi(){var A=0,e=0;return(A=0|Sn(40,82796,0))?(t[A>>2]=0,t[A+4>>2]=0,t[A+8>>2]=1,t[A+28>>2]=1,e=0|W(),t[A+36>>2]=e,0|e?0|(e=A):(Xe(6,106,65,0,0),Xn(A,82796,0),0|(e=0))):(Xe(6,106,65,0,0),0|(A=0))}function Wi(A){var e;return e=G,G=G+16|0,A=(0|T((A|=0)+8|0,1,e,0|t[A+36>>2]))>0,G=e,A&(0|t[e>>2])>1&1|0}function Yi(A,e,r){e|=0,r|=0;var i,n=0,f=0,a=0;if(i=G,G=G+16|0,!(A|=0))return G=i,0|(e=0);t[i>>2]=0,0|t[A+24>>2]&&0|(n=0|t[A+12>>2])&&0|(f=0|t[n+88>>2])&&(NB[31&f](A),t[A+24>>2]=0),(0|t[A+4>>2])==(0|e)&&0!=(0|t[A+12>>2])||(a=8);do{if(8==(0|a)){if(0|(n=0|gA(i,e))){t[A+12>>2]=n,t[A+16>>2]=t[i>>2],t[A>>2]=t[n>>2],t[A+4>>2]=e;break}return Xe(6,158,156,0,0),G=i,0|(e=0)}}while(0);return t[A+24>>2]=r,G=i,0|(e=0!=(0|r)&1)}function Ri(A){return 0|t[(A|=0)+24>>2]}function yi(A){var e,r,i=0;r=G,G=G+16|0,(A|=0)?(T(A+8|0,-1,r,0|t[A+36>>2]),(0|t[r>>2])>0||(0|(e=0|t[A+12>>2])&&0|(i=0|t[e+88>>2])&&(NB[31&i](A),t[A+24>>2]=0),V(0|t[A+36>>2]),mn(0|t[A+32>>2],18),Xn(A,82796,0)),G=r):G=r}function Vi(A,e){e|=0;var r=0;return(r=0|t[(A|=0)+12>>2])&&(r=0|t[r+92>>2])?0|(e=0|MB[31&r](A,3,0,e)):0|(e=-2)}function mi(A,e,r,i,n){return 0|Hi(A|=0,e|=0,r|=0,i|=0,n|=0,0)}function Hi(A,e,r,i,n,f){e|=0,r|=0,i|=0,n|=0,f|=0;var a,B=0,o=0;if(a=G,G=G+16|0,!(B=0|t[(A|=0)+16>>2])&&(B=0|function(A,e){return 0|li(A|=0,e|=0,-1)}(n,i),t[A+16>>2]=B,!B))return G=a,0|(e=0);o=0|t[B>>2];do{if(0==(0|r)&&0==(4&t[o+4>>2]|0)){if((0|Vi(n,a))>0&&0|(B=0|Xi(0|pr(0|t[a>>2])))){n=0|t[A+16>>2],o=0|t[n>>2],r=B;break}return Xe(6,161,158,0,0),G=a,0|(e=0)}n=B}while(0);do{if(f){if(!(B=0|t[o+68>>2])){if(!((0|_i(n))<1))break;return G=a,0|(B=0)}if((0|gB[63&B](n,A))<1)return G=a,0|(e=0);t[16+(0|t[A+16>>2])>>2]=128;break}if(!(B=0|t[o+60>>2])){if(!((0|Oi(n))<1))break;return G=a,0|(B=0)}if((0|gB[63&B](n,A))<1)return G=a,0|(e=0);t[16+(0|t[A+16>>2])>>2]=64;break}while(0);return(0|Qi(0|t[A+16>>2],-1,248,1,0,r))<1?(G=a,0|(e=0)):(0|e&&(t[e>>2]=t[A+16>>2]),4&t[4+(0|t[t[A+16>>2]>>2])>>2]|0?(G=a,0|(e=1)):(e=0!=(0|gi(A,r,i))&1,G=a,0|e))}function vi(A,e,r,i,n){return 0|Hi(A|=0,e|=0,r|=0,i|=0,n|=0,1)}function Ji(A,e,r){e|=0,r|=0;var i,n,f=0,a=0,B=0;if(n=G,G=G+80|0,a=0|t[(A|=0)+16>>2],f=0|t[a>>2],4&t[f+4>>2]|0)return e?512&t[A+8>>2]|0?(r=0|MB[31&t[f+64>>2]](a,e,r,A),G=n,0|r):(f=0|ci(a))?(r=0|MB[31&t[64+(0|t[f>>2])>>2]](f,e,r,A),Ei(f),G=n,0|r):(G=n,0|(r=0)):(r=0|MB[31&t[f+64>>2]](a,0,r,A),G=n,0|r);i=0|t[f+64>>2];do{if(e){t[n+64>>2]=0;do{if(512&t[A+8>>2]){if(i){f=0|MB[31&i](a,e,r,A);break}f=0|Mi(A,n,n+64|0),B=20;break}if(a=0|di()){if(!(0|hi(a,A))){Ii(a),f=0;break}i?(f=0|t[a+16>>2],f=0|MB[31&t[64+(0|t[f>>2])>>2]](f,e,r,a)):f=0|Mi(a,n,n+64|0),Ii(a),B=20}else f=0}while(0);if(20==(0|B)&&0==(0|i)&0!=(0|f)){if(!((0|Pi(0|t[A+16>>2],e,r,n,0|t[n+64>>2]))<1))break;f=0}return G=n,0|(r=f)}if(0|i){if(!((0|MB[31&i](a,0,r,A))<1))break;return G=n,0|(f=0)}if((0|(f=0|pi(0|t[A>>2])))<0)return G=n,0|(r=0);if((0|Pi(a,0,r,0,f))<1)return G=n,0|(r=0)}while(0);return G=n,0|(r=1)}function Xi(A){return A|=0,0|Kn(8,0,0)?0|(A=0|function(A,e){var r;if(e|=0,r=G,G=G+16|0,!(A|=0))return G=r,0|(e=0);if(0==(0|m(82080,2))|0==(0|t[20521]))return G=r,0|0;Y(0|t[20523]),t[r+8>>2]=A,t[r>>2]=-32769&e,A=0|ye(0|t[20522],r);A:do{if(A){e:do{if(!(32768&e))for(e=0;;){if(!(0|t[A+4>>2]))break e;if(e>>>0>9){A=0;break A}if(t[r+8>>2]=t[A+12>>2],!(A=0|ye(0|t[20522],r))){A=0;break A}e=e+1|0}}while(0);A=0|t[A+12>>2]}else A=0}while(0);return y(0|t[20523]),G=r,0|A}(A,1)):0|(A=0)}function Ti(){Tr(2),Tr(1),Tr(-1),mn(0|t[20539],17),mn(0|t[20519],11),Fn(0|t[20519])}function Li(A){return 0|t[(A|=0)+36>>2]}function Si(A){return 0|t[(A|=0)>>2]}function pi(A){return(A|=0)?0|(A=0|t[A+8>>2]):(Xe(6,162,159,0,0),0|(A=-1))}function zi(A){return 0|t[(A|=0)+16>>2]}function Ki(A){return 0|t[(A|=0)+12>>2]}function ji(A,e){e|=0,t[(A|=0)+8>>2]=t[A+8>>2]|e}function xi(A,e){return e|=0,t[(A|=0)+8>>2]&e|0}function Oi(A){var e=0;return 0|(A|=0)&&0|(e=0|t[A>>2])&&0|t[e+40>>2]?(t[A+16>>2]=8,(e=0|t[e+36>>2])?((0|(e=0|IB[31&e](A)))>=1||(t[A+16>>2]=0),0|(A=e)):0|(A=1)):(Xe(6,141,150,0,0),0|(A=-2))}function Pi(A,e,r,i,n){e|=0,r|=0,i|=0,n|=0;var f=0,a=0;if(0|(A|=0)&&0|(a=0|t[A>>2])&&0|(f=0|t[a+40>>2])){if(8!=(0|t[A+16>>2]))return Xe(6,140,151,0,0),0|(n=-1);do{if(2&t[a+4>>2]){if(!(f=0|Zi(0|t[A+8>>2])))return Xe(6,140,163,0,0),0|(n=0);if(!e)return t[r>>2]=f,0|(n=1);if((0|t[r>>2])>>>0>=f>>>0){f=0|t[40+(0|t[A>>2])>>2];break}return Xe(6,140,155,0,0),0|(n=0)}}while(0);return 0|(n=0|hB[15&f](A,e,r,i,n))}return Xe(6,140,150,0,0),0|(n=-2)}function _i(A){var e=0;return 0|(A|=0)&&0|(e=0|t[A>>2])&&0|t[e+48>>2]?(t[A+16>>2]=16,(e=0|t[e+44>>2])?((0|(e=0|IB[31&e](A)))>=1||(t[A+16>>2]=0),0|(A=e)):0|(A=1)):(Xe(6,143,150,0,0),0|(A=-2))}function qi(A,e){!function(A,e,r){e|=0,r|=0,t[(A|=0)>>2]=2,t[A+4>>2]=r,t[A+8>>2]=e}(A|=0,17,e|=0)}function $i(A,e,r,i){r|=0,i|=0;var n=0;if(!(A=0|function(A,e){return 0|li(0,e|=0,A|=0)}(A|=0,e|=0)))return 0|(n=0);e=0|t[A>>2];do{if(0!=(0|e)&&0!=(0|t[e+32>>2])){if(t[A+16>>2]=4,0|(e=0|t[e+28>>2])&&(0|IB[31&e](A))<1){t[A+16>>2]=0,e=0;break}if((0|Qi(A,-1,4,6,i,r))<1)e=0;else{if(0|(i=0|t[A>>2])&&0|t[i+32>>2]){if(4!=(0|t[A+16>>2])){Xe(6,146,151,0,0),e=0;break}if(!(e=0|Fi())){e=0;break}if((0|gB[63&t[32+(0|t[A>>2])>>2]](A,e))>=1)break;yi(e),e=0;break}Xe(6,146,150,0,0),e=0}}else n=4}while(0);return 4==(0|n)&&(Xe(6,147,150,0,0),e=0),Ei(A),0|(n=e)}function An(A,e){A|=0,e|=0;var r=0,i=0,n=0,f=0,a=0,B=0;do{if((0|A)==(0|e)){if(0|ia(A,1+(0|t[A+4>>2])|0)){f=e+4|0;break}return 0|(f=0)}if(t[A+12>>2]=t[e+12>>2],0|ia(A,1+(0|t[e+4>>2])|0)){t[A+4>>2]=t[e+4>>2],f=e+4|0;break}return 0|(f=0)}while(0);if((0|t[f>>2])<=0)return 0|(f=1);for(e=0|t[e>>2],r=0,i=0,n=0|t[A>>2];a=0|t[e>>2],B=n,n=n+4|0,t[B>>2]=a<<1|r,r=a>>>31,!((0|(i=i+1|0))>=(0|t[f>>2]));)e=e+4|0;return r?(t[n>>2]=1,t[A+4>>2]=1+(0|t[A+4>>2]),0|(B=1)):0|(B=1)}function en(A,e){A|=0;var r,i,n,f,a=0,B=0,o=0;if(0|jf(e|=0))return ba(A),0|(A=1);i=0|t[e+4>>2],n=0|t[e>>2],f=0|t[n+(i+-1<<2)>>2];do{if((0|e)!=(0|A)){if(0|ia(A,i-(1==(0|f)&1)|0)){t[A+12>>2]=t[e+12>>2],e=0|t[n+(i+-1<<2)>>2];break}return 0|(A=0)}e=f}while(0);if(r=0|t[A>>2],0|(a=e>>>1)&&(t[r+(i+-1<<2)>>2]=a),(0|i)>1){a=i+-1|0;do{B=a,o=e,e=0|t[n+((a=a+-1|0)<<2)>>2],t[r+(a<<2)>>2]=e>>>1|o<<31}while((0|B)>1)}return t[A+4>>2]=i-(1==(0|f)&1),i-(1==(0|f)&1)|0||(t[A+12>>2]=0),0|(o=1)}function rn(A,e,r){A|=0,e|=0;var i,n,f=0,a=0,B=0;if((0|(r|=0))<0)return Xe(3,145,119,0,0),0|(A=0);if(!(0|ia(A,1+(r>>>5)+(0|t[e+4>>2])|0)))return 0|(A=0);if(t[A+12>>2]=t[e+12>>2],i=0|t[e>>2],n=0|t[A>>2],t[n+((0|t[e+4>>2])+(r>>>5)<<2)>>2]=0,f=0|t[e+4>>2],31&r){if((0|f)>0)for(f=f+-1|0;B=0|t[i+(f<<2)>>2],t[n+((a=f+(r>>>5)|0)+1<<2)>>2]=t[n+(a+1<<2)>>2]|B>>>(32-(31&r)|0),t[n+(a<<2)>>2]=B<<(31&r),(0|f)>0;)f=f+-1|0}else if((0|f)>0)for(f=f+-1|0;t[n+(f+(r>>>5)<<2)>>2]=t[i+(f<<2)>>2],(0|f)>0;)f=f+-1|0;return Df(0|n,0,r>>>5<<2|0),t[A+4>>2]=1+(r>>>5)+(0|t[e+4>>2]),aa(A),0|(B=1)}function nn(A,e,r){A|=0,e|=0;var i=0,n=0,f=0,a=0,B=0,o=0;if((0|(r|=0))<0)return Xe(3,146,119,0,0),0|(A=0);if((r>>>5|0)>=(0|(B=0|t[e+4>>2]))|0==(0|B))return ba(A),0|(A=1);i=(31-r+(0|Kf(e))|0)/32|0;do{if((0|A)!=(0|e)){if(0|ia(A,i)){t[A+12>>2]=t[e+12>>2];break}return 0|(A=0)}if(!r)return 0|(A=1)}while(0);if(f=(0|t[e>>2])+(r>>>5<<2)|0,B=0|t[A>>2],e=(0|t[e+4>>2])-(r>>>5)|0,t[A+4>>2]=i,31&r){if(n=(0|t[f>>2])>>>(31&r),e+-1|0){for(a=e+-1|0,i=B;o=0|t[(f=f+4|0)>>2],t[i>>2]=o<<32-(31&r)|n,n=o>>>(31&r),a=a+-1|0;)i=i+4|0;i=B+(e+-1<<2)|0}else i=B;n?(t[i>>2]=n,n=19):n=19}else if(e)for(n=f,i=B;;){if(t[i>>2]=t[n>>2],!(e=e+-1|0)){n=19;break}n=n+4|0,i=i+4|0}return 19==(0|n)&&(i=0|t[A+4>>2]),0|i||(t[A+12>>2]=0),0|(o=1)}function tn(A){var e,r,n,f=0,a=0,B=0,o=0,u=0;n=G,G=G+16|0,e=0|Ln(4+((a=(((0|(o=3*(0|Kf(A|=0))|0))/10|0)+((0|o)/1e3|0)+2|0)/9|0)<<2)|0,82796,0),r=0|Ln(o=((0|o)/10|0)+((0|o)/1e3|0)+5|0,82796,0);A:do{if(!(0==(0|e)|0==(0|r))){if(!(B=0|ea(A))){Xn(e,82796,0),Pf(0);break}if(0|jf(B))i[r>>0]=48,i[r+1>>0]=0;else{0|Ia(B)?(i[r>>0]=45,A=r+1|0):A=r;e:do{if(0|jf(B))f=e;else{for(f=e;;){if((f-e>>2|0)>(0|a)){a=22;break}if(u=0|lB(B,1e9),t[f>>2]=u,-1==(0|u)){a=23;break}if(f=f+4|0,0|jf(B))break e}if(22==(0|a)){Xn(e,82796,0),Pf(B);break A}if(23==(0|a)){Xn(e,82796,0),Pf(B);break A}}}while(0);for(f=f+-4|0,t[n>>2]=t[f>>2],Ge(A,o+r-A|0,80616,n);0|i[A>>0];)A=A+1|0;if((0|f)!=(0|e))do{for(f=f+-4|0,t[n+8>>2]=t[f>>2],Ge(A,o+r-A|0,80619,n+8|0);0|i[A>>0];)A=A+1|0}while((0|f)!=(0|e))}return Xn(e,82796,0),Pf(B),G=n,0|(u=r)}Xe(3,104,65,0,0),Xn(e,82796,0),Pf(0)}while(0);return Xn(r,82796,0),G=n,0|(u=0)}function fn(A,e){A|=0;var r=0,n=0,a=0,B=0,o=0,u=0,l=0,c=0,E=0;if(!(e|=0))return 0|(A=0);switch(0|i[e>>0]){case 45:e=e+1|0,c=1;break;case 0:return 0|(A=0);default:c=0}for(r=0;;){if(!(0|Rt(0|f[e+r>>0]))){n=7;break}if((r=r+1|0)>>>0>=536870912){e=0;break}}do{if(7==(0|n))if((r+-1|0)>>>0<=536870910){if(l=r+c|0,!A)return 0|(A=l);if(n=0|t[A>>2])ba(n);else if(!(n=0|qf()))return 0|(A=0);if(!(r>>>0>536870904)){if((0|(a=(31+(r<<2)|0)>>>5))>(0|t[n+8>>2])&&0==(0|Aa(n,a))){e=n;break}for(o=1+((r+((o=~r)>>>0>4294967287?o:-9)+8|0)>>>3)|0,u=0;;){for(a=0,B=(0|r)<8?r:8;a=((0|(E=0|gt(0|i[e+(r-B)>>0])))>0?E:0)|a<<4,!((0|B)<2);)B=B+-1|0;if(t[(0|t[n>>2])+(u<<2)>>2]=a,(0|(u=u+1|0))==(0|o))break;r=r+-8|0}return t[(E=n+4|0)>>2]=o,aa(n),t[A>>2]=n,0|t[E>>2]?(t[n+12>>2]=c,0|(E=l)):0|(E=l)}e=n}else e=0}while(0);return 0|t[A>>2]||Pf(e),0|(E=0)}function an(A,e){A|=0;var r=0,n=0,a=0,B=0,o=0,u=0,l=0;if(!(e|=0))return 0|(A=0);switch(0|i[e>>0]){case 45:e=e+1|0,l=1;break;case 0:return 0|(A=0);default:l=0}for(r=0;;){if(((0|f[e+r>>0])-48|0)>>>0>=10){n=7;break}if((r=r+1|0)>>>0>=536870912){e=0;break}}A:do{if(7==(0|n))if((r+-1|0)>>>0<=536870910){if(u=r+l|0,!A)return 0|(A=u);if(n=0|t[A>>2])ba(n);else if(!(n=0|qf()))return 0|(A=0);if(!(r>>>0>536870904)){if((0|(a=(31+(r<<2)|0)>>>5))>(0|t[n+8>>2])&&0==(0|Aa(n,a))){e=n;break}a=0==(0|(a=(r>>>0)%9|0))?0:9-a|0,B=0;do{if(o=r,r=r+-1|0,B=(10*B|0)-48+(0|i[e>>0])|0,e=e+1|0,9==(0|(a=a+1|0))){if(!(0|QB(n,1e9))){e=n;break A}if(!(0|cB(n,B))){e=n;break A}a=0,B=0}}while((0|o)>1);return aa(n),t[A>>2]=n,0|t[n+4>>2]?(t[n+12>>2]=l,0|(A=u)):0|(A=u)}e=n}else e=0}while(0);return 0|t[A>>2]||Pf(e),0|(A=0)}function Bn(A,e){A|=0;var r,n=0;r=45==(0|i[(e|=0)>>0])?e+1|0:e;A:do{if(48==(0|i[r>>0])){switch(0|i[r+1>>0]){case 120:case 88:break;default:n=4;break A}if(!(0|fn(A,r+2|0)))return 0|(n=0)}else n=4}while(0);return 4==(0|n)&&0==(0|an(A,r))?0|(n=0):45!=(0|i[e>>0])?0|(n=1):(e=0|t[A>>2],0|t[e+4>>2]?(t[e+12>>2]=1,0|(n=1)):0|(n=1))}function on(A,e,r){A|=0,r|=0;var i=0,n=0,f=0,a=0,B=0,o=0;if((0|(o=0|t[(e|=0)+12>>2]))!=(0|t[r+12>>2]))return i=0==(0|o)?r:e,(0|la(e=0==(0|o)?e:r,i))<0?0|un(A,i,e)?(t[A+12>>2]=1,0|(o=1)):0|(o=0):0|un(A,e,i)?(t[A+12>>2]=0,0|(o=1)):0|(o=0);if(i=(B=(0|t[e+4>>2])<(0|t[r+4>>2]))?e:r,B=0|t[(e=B?r:e)+4>>2],f=0|t[i+4>>2],0|ia(A,B+1|0)){if(t[A+4>>2]=B,e=0|t[e>>2],r=0|hn(a=0|t[A>>2],e,0|t[i>>2],f),B-f|0){for(n=e+(f<<2)|0,i=B-f|0,e=a+(f<<2)|0;i=i+-1|0,f=(0|t[n>>2])+r|0,t[e>>2]=f,r&=0==(0|f),i;)n=n+4|0,e=e+4|0;e=a+(B<<2)|0}else e=a+(f<<2)|0;t[e>>2]=r,t[A+4>>2]=(0|t[A+4>>2])+r,t[A+12>>2]=0,e=A+12|0,r=1}else e=A+12|0,r=0;return t[e>>2]=o,0|(o=r)}function un(A,e,r){A|=0,r|=0;var i,n=0,f=0,a=0,B=0;if(((n=0|t[(e|=0)+4>>2])-(f=0|t[r+4>>2])|0)<0)return Xe(3,115,100,0,0),0|(A=0);if(!(0|ia(A,n)))return 0|(A=0);if(e=0|t[e>>2],r=0|Un(i=0|t[A>>2],e,0|t[r>>2],f),n-f|0){for(B=e+(f<<2)|0,a=n-f|0,f=i+(f<<2)|0;a=a+-1|0,e=0|t[B>>2],t[f>>2]=e-r,a;)B=B+4|0,r&=0==(0|e),f=f+4|0;e=i+(n<<2)|0}else e=i+(f<<2)|0;A:do{if(n)for(;;){if(0|t[(e=e+-4|0)>>2])break A;if(!(n=n+-1|0)){n=0;break}}else n=0}while(0);return t[A+4>>2]=n,t[A+12>>2]=0,0|(A=1)}function ln(A,e,r){A|=0,r|=0;var i,n=0,f=0,a=0,B=0;if(n=(B=(0|t[(e|=0)+4>>2])<(0|t[r+4>>2]))?e:r,B=0|t[(e=B?r:e)+4>>2],a=0|t[n+4>>2],!(0|ia(A,B+1|0)))return 0|(A=0);if(t[A+4>>2]=B,e=0|t[e>>2],r=0|hn(i=0|t[A>>2],e,0|t[n>>2],a),B-a|0){for(f=e+(a<<2)|0,n=B-a|0,e=i+(a<<2)|0;n=n+-1|0,a=(0|t[f>>2])+r|0,t[e>>2]=a,r&=0==(0|a),n;)f=f+4|0,e=e+4|0;e=i+(B<<2)|0}else e=i+(a<<2)|0;return t[e>>2]=r,t[A+4>>2]=(0|t[A+4>>2])+r,t[A+12>>2]=0,0|(A=1)}function cn(A,e,r){A|=0,e|=0;var i=0,n=0,f=0,a=0,B=0,o=0;if(i=0!=(0|t[(r|=0)+12>>2]),0|t[e+12>>2]?i?i=r:(o=1&(1^i),n=4):i?(o=0,n=4):(i=e,e=r),4==(0|n)){if(i=(B=(0|t[e+4>>2])<(0|t[r+4>>2]))?e:r,B=0|t[(e=B?r:e)+4>>2],f=0|t[i+4>>2],!(0|ia(A,B+1|0)))return 0|(A=0);if(t[A+4>>2]=B,e=0|t[e>>2],i=0|hn(a=0|t[A>>2],e,0|t[i>>2],f),B-f|0){for(n=e+(f<<2)|0,r=B-f|0,e=a+(f<<2)|0;r=r+-1|0,f=(0|t[n>>2])+i|0,t[e>>2]=f,i&=0==(0|f),r;)n=n+4|0,e=e+4|0;e=a+(B<<2)|0}else e=a+(f<<2)|0;return t[e>>2]=i,t[A+4>>2]=(0|t[A+4>>2])+i,t[A+12>>2]=o,0|(A=1)}return 0|ia(A,(0|(B=0|t[i+4>>2]))>(0|(o=0|t[e+4>>2]))?B:o)?(0|la(i,e))<0?0|un(A,e,i)?(t[A+12>>2]=1,0|(A=1)):0|(A=0):0|un(A,i,e)?(t[A+12>>2]=0,0|(A=1)):0|(A=0):0|(A=0)}function En(A){return 0|t[(A|=0)>>2]}function Qn(A){t[(A|=0)+16>>2]=0}function wn(A){(A|=0)&&(Pf(0|t[A>>2]),Pf(0|t[A+4>>2]),Pf(0|t[A+8>>2]),Pf(0|t[A+12>>2]),V(0|t[A+36>>2]),Xn(A,82796,0))}function bn(A,e,r,i,n,f){e|=0,r|=0,i|=0,n|=0,f|=0;var a,B=0,o=0,u=0,l=0,c=0,E=0;if(a=G,G=G+16|0,A|=0)r=A;else if(!(r=0|function(A,e,r){A|=0,e|=0,r|=0;var i,n=0,f=0;if(!(i=0|Sn(40,82796,0)))return Xe(3,102,65,0,0),0|(n=0);if(f=0|W(),t[i+36>>2]=f,!f)return Xe(3,102,65,0,0),Xn(i,82796,0),0|(f=0);t[i+16>>2]=0,(0==(0|A)||(f=0|ea(A),t[i>>2]=f,0|f))&&(n=7);do{if(7==(0|n)){if(0|e&&(f=0|ea(e),t[i+4>>2]=f,0==(0|f)))break;if(f=0|ea(r),t[i+12>>2]=f,0|f)return 0|Of(r,4)&&ha(0|t[i+12>>2],4),t[i+20>>2]=-1,0|(f=i)}}while(0);return Pf(0|t[i>>2]),Pf(0|t[i+4>>2]),Pf(0|t[i+8>>2]),Pf(0|t[i+12>>2]),V(0|t[i+36>>2]),Xn(i,82796,0),0|(f=0)}(0,0,r)))return G=a,0|(E=0);E=0==(0|t[r>>2])&&(E=0|qf(),t[r>>2]=E,0==(0|E))?30:5;A:do{if(5==(0|E)){if(0==(0|t[(u=r+4|0)>>2])&&(o=0|qf(),t[u>>2]=o,0==(0|o))){E=30;break}if(B=0|t[(o=r+8|0)>>2],e&&(Pf(B),B=0|ea(e),t[o>>2]=B),B){0|n&&(t[r+32>>2]=n),0|f&&(t[r+28>>2]=f),B=r+12|0;e:do{if(0|Ya(0|t[r>>2],0|t[B>>2])){for(e=32;!(0|vf(0|t[u>>2],0|t[r>>2],0|t[B>>2],i,a));){if(!(0|t[a>>2]))break e;if(!e){E=19;break}if(!(0|Ya(0|t[r>>2],0|t[B>>2])))break e;e=e+-1|0}if(19==(0|E)){Xe(3,128,113,0,0);break}if(0!=(0|(e=0|t[r+32>>2]))&&0!=(0|(c=0|t[(l=r+28|0)>>2]))){if(f=0|t[r>>2],!(0|UB[7&e](f,f,0|t[o>>2],0|t[B>>2],i,c))){E=30;break A}}else{if(!(0|Oa(c=0|t[r>>2],c,0|t[o>>2],0|t[B>>2],i))){E=30;break A}l=r+28|0}if(!(e=0|t[l>>2]))return G=a,0|(E=r);if(!(0|Xa(c=0|t[u>>2],c,e,i))){E=30;break A}if(!(0!=(0|Xa(c=0|t[r>>2],c,0|t[l>>2],i))|0==(0|A)^1))break A;return G=a,0|r}}while(0);E=30}else E=30}}while(0);do{if(30==(0|E)){if(A)return G=a,0|(E=r);u=r+4|0,o=r+8|0,B=r+12|0;break}}while(0);return Pf(0|t[r>>2]),Pf(0|t[u>>2]),Pf(0|t[o>>2]),Pf(0|t[B>>2]),V(0|t[r+36>>2]),Xn(r,82796,0),G=a,0|(E=0)}function kn(A,e,r,i){A|=0,e|=0,i|=0;var n=0;if(0|t[(r|=0)>>2]&&0|t[r+4>>2]){if(-1!=(0|t[r+20>>2])){if(!(0|function(A,e){A|=0,e|=0;var r=0,i=0,n=0;A:do{if(0!=(0|t[A>>2])&&(i=0|t[A+4>>2],0!=(0|i))){if(-1!=(0|(r=0|t[A+20>>2])))if(t[A+20>>2]=r+1,32==(r+1|0)&&0!=(0|t[A+8>>2])&&0==(2&t[A+24>>2]|0)){if(!(0|bn(A,0,0,e,0,0))){r=A+20|0,i=0;break}}else n=10;else t[A+20>>2]=1,n=10;do{if(10==(0|n)&&0==(1&t[A+24>>2]|0)){if(r=0|t[A+28>>2]){if(!(0|ma(i,i,i,r,e))){r=A+20|0,i=0;break A}if(0|ma(i=0|t[A>>2],i,i,0|t[A+28>>2],e))break;r=A+20|0,i=0;break A}if(!(0|AB(i,i,i,0|t[A+12>>2],e))){r=A+20|0,i=0;break A}if(0|AB(i=0|t[A>>2],i,i,0|t[A+12>>2],e))break;r=A+20|0,i=0;break A}}while(0);r=A+20|0,i=1}else n=3}while(0);return 3==(0|n)&&(Xe(3,103,107,0,0),r=A+20|0,i=0),32!=(0|t[r>>2])||(t[r>>2]=0),0|i}(r,i)))return 0|(A=0)}else t[r+20>>2]=0;return 0|e&&0==(0|ra(e,0|t[r+4>>2]))?0|(A=0):(e=0|t[r+28>>2],n=0|t[r>>2],e?0|(A=0|Va(A,A,n,e,i)):0|(A=0|AB(A,A,n,0|t[r+12>>2],i)))}return Xe(3,100,107,0,0),0|(A=0)}function sn(A,e,r,i){A|=0,r|=0,i|=0;var n,f=0,a=0,B=0,o=0;if(!(e|=0)&&!(e=0|t[r+4>>2]))return Xe(3,101,107,0,0),0|(A=0);if(!(n=0|t[r+28>>2]))return 0|(A=0|AB(A,A,e,0|t[r+12>>2],i));if(r=0|t[e+4>>2],(0|t[A+8>>2])>=(0|r)){if(f=0|t[A+4>>2],0|r){a=0|t[A>>2],B=0;do{t[(o=a+(B<<2)|0)>>2]=t[o>>2]&B-f>>31,B=B+1|0}while((0|B)!=(0|r))}t[A+4>>2]=r&~(r-f>>31)|r-f>>31&f}return 0|(o=0|Va(A,A,e,n,i))}function Cn(A){return 0,0|function(A,e){return(0|(A|=0))==(0|(e|=0))|0}(0,0|t[(A|=0)+16>>2])}function dn(A){return 0|R(0|t[(A|=0)+36>>2])}function In(A){return 0|y(0|t[(A|=0)+36>>2])}function gn(A,e,r,i){A|=0,e|=0,i|=0;var n=0,f=0,a=0,B=0,o=0,u=0,l=0,c=0;if((0|(r|=0))<1)return 0|(i=0);if(r>>>0>3){for(f=e,n=0,a=r,B=A;u=0|lf(0|t[f>>2],0,0|i,0),c=0|E(),l=0|cf(0|t[B>>2],0,0|n,0),c=0|cf(0|l,0|E(),0|u,0|c),u=0|E(),t[B>>2]=c,l=0|cf(0|(c=0|lf(0|t[f+4>>2],0,0|i,0)),0|(l=0|E()),0|t[(o=B+4|0)>>2],0),u=0|cf(0|l,0|E(),0|u,0),l=0|E(),t[o>>2]=u,u=0|cf(0|(o=0|lf(0|t[f+8>>2],0,0|i,0)),0|(u=0|E()),0|t[(c=B+8|0)>>2],0),l=0|cf(0|u,0|E(),0|l,0),u=0|E(),t[c>>2]=l,l=0|cf(0|(c=0|lf(0|t[f+12>>2],0,0|i,0)),0|(l=0|E()),0|t[(o=B+12|0)>>2],0),u=0|cf(0|l,0|E(),0|u,0),n=0|E(),t[o>>2]=u,!((a=a+-4|0)>>>0<=3);)f=f+16|0,B=B+16|0;if(!(f=r+-4-(r+-4&-4)|0))return 0|(c=n);e=e+(4+(r+-4&-4)<<2)|0,A=A+(4+(r+-4&-4)<<2)|0}else n=0,f=r;for(;l=0|lf(0|t[e>>2],0,0|i,0),c=0|E(),n=0|cf(0|t[A>>2],0,0|n,0),c=0|cf(0|n,0|E(),0|l,0|c),n=0|E(),t[A>>2]=c,f=f+-1|0;)e=e+4|0,A=A+4|0;return 0|n}function Dn(A,e,r,i){A|=0,e|=0,i|=0;var n=0,f=0,a=0,B=0,o=0,u=0;if((0|(r|=0))<1)return 0|(i=0);if(r>>>0>3){for(f=e,n=0,a=r,B=A;u=0|cf(0|(u=0|lf(0|t[f>>2],0,0|i,0)),0|E(),0|n,0),o=0|E(),t[B>>2]=u,o=0|cf(0|(u=0|lf(0|t[f+4>>2],0,0|i,0)),0|E(),0|o,0),u=0|E(),t[B+4>>2]=o,u=0|cf(0|(o=0|lf(0|t[f+8>>2],0,0|i,0)),0|E(),0|u,0),o=0|E(),t[B+8>>2]=u,o=0|cf(0|(u=0|lf(0|t[f+12>>2],0,0|i,0)),0|E(),0|o,0),n=0|E(),t[B+12>>2]=o,!((a=a+-4|0)>>>0<=3);)f=f+16|0,B=B+16|0;if(!(f=r+-4-(r+-4&-4)|0))return 0|(u=n);e=e+(4+(r+-4&-4)<<2)|0,A=A+(4+(r+-4&-4)<<2)|0}else n=0,f=r;for(;u=0|cf(0|(u=0|lf(0|t[e>>2],0,0|i,0)),0|E(),0|n,0),n=0|E(),t[A>>2]=u,f=f+-1|0;)e=e+4|0,A=A+4|0;return 0|n}function Mn(A,e,r){e|=0;var i,n,t=0,f=0,a=0,B=0,u=0,l=0,c=0;if(!(r|=0))return 0|(r=-1);for(c=(A|=0)-(A>>>0<r>>>0?0:r)|0,i=r<<32-(n=0|zf(r))>>>16,A=2,c=0==(32-n|0)?c:c<<32-n|e>>>n,u=e<<32-n,l=0;;){a=0|o(t=(c>>>16|0)==(0|i)?65535:(c>>>0)/(i>>>0)|0,i),e=0|o(t,r<<32-n&65535),f=c-a|0;A:do{if(f>>>0>65535)f=a;else for(B=u>>>16;;){if(e>>>0<=(f<<16|B)>>>0){f=a;break A}if(t=t+-1|0,e=e-(r<<32-n&65535)|0,(f=c-(a=a-i|0)|0)>>>0>65535){f=a;break}}}while(0);if(a=(e>>>16)+f+(u>>>0<(B=e<<16)>>>0&1)|0,f=u-B|0,t=t+((e=c>>>0<a>>>0)<<31>>31)|0,!(A=A+-1|0))break;c=c-a+(e?r<<32-n:0)<<16|f>>>16,u=f<<16,l=t<<16}return 0|(r=t|l)}function hn(A,e,r,i){A|=0,e|=0,r|=0;var n=0,f=0,a=0,B=0,o=0,u=0,l=0,c=0;if((0|(i|=0))<1)return 0|(l=0);if(i>>>0>3){for(l=4+(i+-4&-4)|0,f=0,n=0,a=e,B=r,o=i,u=A;c=0|t[B>>2],n=0|cf(0|f,0|n,0|t[a>>2],0),c=0|cf(0|n,0|E(),0|c,0),n=0|E(),t[u>>2]=c,c=0|cf(0|t[B+4>>2],0,0|t[a+4>>2],0),n=0|cf(0|c,0|E(),0|n,0),c=0|E(),t[u+4>>2]=n,n=0|cf(0|t[B+8>>2],0,0|t[a+8>>2],0),c=0|cf(0|n,0|E(),0|c,0),n=0|E(),t[u+8>>2]=c,c=0|cf(0|t[B+12>>2],0,0|t[a+12>>2],0),n=0|cf(0|c,0|E(),0|n,0),f=0|E(),t[u+12>>2]=n,!((o=o+-4|0)>>>0<=3);)n=0,a=a+16|0,B=B+16|0,u=u+16|0;if(!(i=i+-4-(i+-4&-4)|0))return 0|(c=f);n=0,e=e+(l<<2)|0,r=r+(l<<2)|0,A=A+(l<<2)|0}else f=0,n=0;for(;c=0|t[r>>2],l=0|cf(0|f,0|n,0|t[e>>2],0),c=0|cf(0|l,0|E(),0|c,0),f=0|E(),t[A>>2]=c,i=i+-1|0;)n=0,e=e+4|0,r=r+4|0,A=A+4|0;return 0|(c=f)}function Un(A,e,r,i){A|=0,e|=0,r|=0;var n=0,f=0,a=0,B=0,o=0,u=0,l=0,c=0,E=0;if((0|(i|=0))<1)return 0|(u=0);if(i>>>0>3){for(u=4+(i+-4&-4)|0,f=e,a=r,n=0,B=i,o=A;c=0|t[f>>2],E=0|t[a>>2],t[o>>2]=c-n-E,E=(0|c)==(0|E)?n:c>>>0<E>>>0&1,c=0|t[f+4>>2],l=0|t[a+4>>2],t[o+4>>2]=c-l-E,l=(0|c)==(0|l)?E:c>>>0<l>>>0&1,c=0|t[f+8>>2],E=0|t[a+8>>2],t[o+8>>2]=c-E-l,E=(0|c)==(0|E)?l:c>>>0<E>>>0&1,c=0|t[f+12>>2],l=0|t[a+12>>2],t[o+12>>2]=c-l-E,n=(0|c)==(0|l)?E:c>>>0<l>>>0&1,!((B=B+-4|0)>>>0<=3);)f=f+16|0,a=a+16|0,o=o+16|0;if(!(i=i+-4-(i+-4&-4)|0))return 0|(E=n);f=e+(u<<2)|0,r=r+(u<<2)|0,A=A+(u<<2)|0}else f=e,n=0;for(;c=0|t[f>>2],E=0|t[r>>2],t[A>>2]=c-n-E,n=(0|c)==(0|E)?n:c>>>0<E>>>0&1,i=i+-1|0;)f=f+4|0,r=r+4|0,A=A+4|0;return 0|n}function Gn(A,e,r){A|=0,e|=0;var i,n=0,a=0,B=0,o=0,u=0,l=0,c=0,E=0,Q=0,w=0,b=0,k=0,s=0,C=0,d=0,I=0,g=0,D=0,M=0,h=0,U=0,Z=0,N=0,F=0,W=0,Y=0,R=0,y=0,V=0;if(i=G,G=G+64|0,r|=0){for(w=0|t[A+8>>2],b=0|t[A+12>>2],k=0|t[A+16>>2],s=0|t[A+20>>2],C=0|t[A+24>>2],d=0|t[A+28>>2],I=0|t[A>>2],g=0|t[A+4>>2];;){D=(0|f[e+1>>0])<<16|(0|f[e>>0])<<24|(0|f[e+2>>0])<<8|0|f[e+3>>0],t[i>>2]=D,B=(D=d+1116352408+((k<<26|k>>>6)^(k<<21|k>>>11)^(k<<7|k>>>25))+(C&~k^s&k)+D|0)+b|0,D=(w&I^(l=g&I)^w&g)+((I<<30|I>>>2)^(I<<19|I>>>13)^(I<<10|I>>>22))+D|0,c=(0|f[e+5>>0])<<16|(0|f[e+4>>0])<<24|(0|f[e+6>>0])<<8|0|f[e+7>>0],t[i+4>>2]=c,a=(c=C+1899447441+(B&k^s&~B)+c+((B<<26|B>>>6)^(B<<21|B>>>11)^(B<<7|B>>>25))|0)+w|0,c=((D<<30|D>>>2)^(D<<19|D>>>13)^(D<<10|D>>>22))+(D&g^l^(u=D&I))+c|0,l=(0|f[e+9>>0])<<16|(0|f[e+8>>0])<<24|(0|f[e+10>>0])<<8|0|f[e+11>>0],t[i+8>>2]=l,Q=(l=s+-1245643825+l+(a&B^k&~a)+((a<<26|a>>>6)^(a<<21|a>>>11)^(a<<7|a>>>25))|0)+g|0,l=((c<<30|c>>>2)^(c<<19|c>>>13)^(c<<10|c>>>22))+(c&I^u^c&D)+l|0,u=(0|f[e+13>>0])<<16|(0|f[e+12>>0])<<24|(0|f[e+14>>0])<<8|0|f[e+15>>0],t[i+12>>2]=u,E=(u=k+-373957723+u+(Q&a^B&~Q)+((Q<<26|Q>>>6)^(Q<<21|Q>>>11)^(Q<<7|Q>>>25))|0)+I|0,u=((l<<30|l>>>2)^(l<<19|l>>>13)^(l<<10|l>>>22))+(l&D^c&D^l&c)+u|0,o=(0|f[e+17>>0])<<16|(0|f[e+16>>0])<<24|(0|f[e+18>>0])<<8|0|f[e+19>>0],t[i+16>>2]=o,D=B+961987163+o+(E&Q^a&~E)+((E<<26|E>>>6)^(E<<21|E>>>11)^(E<<7|E>>>25))+D|0,o=((u<<30|u>>>2)^(u<<19|u>>>13)^(u<<10|u>>>22))+(u&c^l&c^u&l)+(B+961987163+o+(E&Q^a&~E)+((E<<26|E>>>6)^(E<<21|E>>>11)^(E<<7|E>>>25)))|0,B=(0|f[e+21>>0])<<16|(0|f[e+20>>0])<<24|(0|f[e+22>>0])<<8|0|f[e+23>>0],t[i+20>>2]=B,c=a+1508970993+B+(D&E^Q&~D)+((D<<26|D>>>6)^(D<<21|D>>>11)^(D<<7|D>>>25))+c|0,B=((o<<30|o>>>2)^(o<<19|o>>>13)^(o<<10|o>>>22))+(o&l^u&l^o&u)+(a+1508970993+B+(D&E^Q&~D)+((D<<26|D>>>6)^(D<<21|D>>>11)^(D<<7|D>>>25)))|0,a=(0|f[e+25>>0])<<16|(0|f[e+24>>0])<<24|(0|f[e+26>>0])<<8|0|f[e+27>>0],t[i+24>>2]=a,l=Q+-1841331548+a+(c&D^E&~c)+((c<<26|c>>>6)^(c<<21|c>>>11)^(c<<7|c>>>25))+l|0,a=((B<<30|B>>>2)^(B<<19|B>>>13)^(B<<10|B>>>22))+(B&u^o&u^B&o)+(Q+-1841331548+a+(c&D^E&~c)+((c<<26|c>>>6)^(c<<21|c>>>11)^(c<<7|c>>>25)))|0,Q=(0|f[e+29>>0])<<16|(0|f[e+28>>0])<<24|(0|f[e+30>>0])<<8|0|f[e+31>>0],t[i+28>>2]=Q,u=E+-1424204075+Q+(l&c^D&~l)+((l<<26|l>>>6)^(l<<21|l>>>11)^(l<<7|l>>>25))+u|0,Q=((a<<30|a>>>2)^(a<<19|a>>>13)^(a<<10|a>>>22))+(a&o^B&o^a&B)+(E+-1424204075+Q+(l&c^D&~l)+((l<<26|l>>>6)^(l<<21|l>>>11)^(l<<7|l>>>25)))|0,E=(0|f[e+33>>0])<<16|(0|f[e+32>>0])<<24|(0|f[e+34>>0])<<8|0|f[e+35>>0],t[i+32>>2]=E,o=D+-670586216+E+(u&l^c&~u)+((u<<26|u>>>6)^(u<<21|u>>>11)^(u<<7|u>>>25))+o|0,E=((Q<<30|Q>>>2)^(Q<<19|Q>>>13)^(Q<<10|Q>>>22))+(Q&B^a&B^Q&a)+(D+-670586216+E+(u&l^c&~u)+((u<<26|u>>>6)^(u<<21|u>>>11)^(u<<7|u>>>25)))|0,D=(0|f[e+37>>0])<<16|(0|f[e+36>>0])<<24|(0|f[e+38>>0])<<8|0|f[e+39>>0],t[i+36>>2]=D,B=D+310598401+c+(o&u^l&~o)+((o<<26|o>>>6)^(o<<21|o>>>11)^(o<<7|o>>>25))+B|0,c=((E<<30|E>>>2)^(E<<19|E>>>13)^(E<<10|E>>>22))+(E&a^Q&a^E&Q)+(D+310598401+c+(o&u^l&~o)+((o<<26|o>>>6)^(o<<21|o>>>11)^(o<<7|o>>>25)))|0,D=(0|f[e+41>>0])<<16|(0|f[e+40>>0])<<24|(0|f[e+42>>0])<<8|0|f[e+43>>0],t[i+40>>2]=D,a=D+607225278+l+(B&o^u&~B)+((B<<26|B>>>6)^(B<<21|B>>>11)^(B<<7|B>>>25))+a|0,l=((c<<30|c>>>2)^(c<<19|c>>>13)^(c<<10|c>>>22))+(c&Q^E&Q^c&E)+(D+607225278+l+(B&o^u&~B)+((B<<26|B>>>6)^(B<<21|B>>>11)^(B<<7|B>>>25)))|0,D=(0|f[e+45>>0])<<16|(0|f[e+44>>0])<<24|(0|f[e+46>>0])<<8|0|f[e+47>>0],t[i+44>>2]=D,Q=D+1426881987+u+(a&B^o&~a)+((a<<26|a>>>6)^(a<<21|a>>>11)^(a<<7|a>>>25))+Q|0,u=((l<<30|l>>>2)^(l<<19|l>>>13)^(l<<10|l>>>22))+(l&E^c&E^l&c)+(D+1426881987+u+(a&B^o&~a)+((a<<26|a>>>6)^(a<<21|a>>>11)^(a<<7|a>>>25)))|0,D=(0|f[e+49>>0])<<16|(0|f[e+48>>0])<<24|(0|f[e+50>>0])<<8|0|f[e+51>>0],t[i+48>>2]=D,E=D+1925078388+o+(Q&a^B&~Q)+((Q<<26|Q>>>6)^(Q<<21|Q>>>11)^(Q<<7|Q>>>25))+E|0,o=((u<<30|u>>>2)^(u<<19|u>>>13)^(u<<10|u>>>22))+(u&c^l&c^u&l)+(D+1925078388+o+(Q&a^B&~Q)+((Q<<26|Q>>>6)^(Q<<21|Q>>>11)^(Q<<7|Q>>>25)))|0,D=(0|f[e+53>>0])<<16|(0|f[e+52>>0])<<24|(0|f[e+54>>0])<<8|0|f[e+55>>0],t[i+52>>2]=D,c=D+-2132889090+B+(E&Q^a&~E)+((E<<26|E>>>6)^(E<<21|E>>>11)^(E<<7|E>>>25))+c|0,B=((o<<30|o>>>2)^(o<<19|o>>>13)^(o<<10|o>>>22))+(o&l^u&l^o&u)+(D+-2132889090+B+(E&Q^a&~E)+((E<<26|E>>>6)^(E<<21|E>>>11)^(E<<7|E>>>25)))|0,D=(0|f[e+57>>0])<<16|(0|f[e+56>>0])<<24|(0|f[e+58>>0])<<8|0|f[e+59>>0],t[i+56>>2]=D,l=D+-1680079193+a+(c&E^Q&~c)+((c<<26|c>>>6)^(c<<21|c>>>11)^(c<<7|c>>>25))+l|0,a=((B<<30|B>>>2)^(B<<19|B>>>13)^(B<<10|B>>>22))+(B&u^o&u^B&o)+(D+-1680079193+a+(c&E^Q&~c)+((c<<26|c>>>6)^(c<<21|c>>>11)^(c<<7|c>>>25)))|0,D=(0|f[e+61>>0])<<16|(0|f[e+60>>0])<<24|(0|f[e+62>>0])<<8|0|f[e+63>>0],t[i+60>>2]=D,n=((a<<30|a>>>2)^(a<<19|a>>>13)^(a<<10|a>>>22))+(a&(B^o)^B&o)+(D+-1046744716+Q+(l&c^E&~l)+((l<<26|l>>>6)^(l<<21|l>>>11)^(l<<7|l>>>25)))|0,u=D+-1046744716+Q+(l&c^E&~l)+((l<<26|l>>>6)^(l<<21|l>>>11)^(l<<7|l>>>25))+u|0,Q=16;do{y=0|t[i+((9&(U=1|Q))<<2)>>2],R=0|t[i+((Q+14&14)<<2)>>2],R=(0|t[i+((W=8&Q)<<2)>>2])+(0|t[i+((Q+9&9)<<2)>>2])+((y<<14|y>>>18)^y>>>3^(y<<25|y>>>7))+((R<<13|R>>>19)^R>>>10^(R<<15|R>>>17))|0,t[i+(W<<2)>>2]=R,F=(R=((u<<26|u>>>6)^(u<<21|u>>>11)^(u<<7|u>>>25))+E+(c&~u^l&u)+(0|t[50576+(Q<<2)>>2])+R|0)+o|0,R=((n<<30|n>>>2)^(n<<19|n>>>13)^(n<<10|n>>>22))+((n^a)&B^(M=n&a))+R|0,N=0|t[i+((U+1&10)<<2)>>2],Z=0|t[i+((U+14&15)<<2)>>2],Z=(0|t[i+((U+9&10)<<2)>>2])+y+((N<<14|N>>>18)^N>>>3^(N<<25|N>>>7))+((Z<<13|Z>>>19)^Z>>>10^(Z<<15|Z>>>17))|0,t[i+((9&U)<<2)>>2]=Z,N=(Z=(0|t[50576+(U<<2)>>2])+c+(F&u^l&~F)+((F<<26|F>>>6)^(F<<21|F>>>11)^(F<<7|F>>>25))+Z|0)+B|0,Z=((R<<30|R>>>2)^(R<<19|R>>>13)^(R<<10|R>>>22))+(R&a^M^(U=R&n))+Z|0,M=2|Q,y=0|t[i+((3|W)<<2)>>2],D=0|t[i+((M+14&8)<<2)>>2],D=(0|t[i+((10&M)<<2)>>2])+(0|t[i+((M+9&11)<<2)>>2])+((y<<14|y>>>18)^y>>>3^(y<<25|y>>>7))+((D<<13|D>>>19)^D>>>10^(D<<15|D>>>17))|0,t[i+((10&M)<<2)>>2]=D,M=(D=(0|t[50576+(M<<2)>>2])+l+D+(N&F^u&~N)+((N<<26|N>>>6)^(N<<21|N>>>11)^(N<<7|N>>>25))|0)+a|0,D=((Z<<30|Z>>>2)^(Z<<19|Z>>>13)^(Z<<10|Z>>>22))+(Z&n^U^Z&R)+D|0,y=0|t[i+(((U=3|Q)+1&12)<<2)>>2],h=0|t[i+((U+14&9)<<2)>>2],h=(0|t[i+((11&U)<<2)>>2])+(0|t[i+((U+9&12)<<2)>>2])+((y<<14|y>>>18)^y>>>3^(y<<25|y>>>7))+((h<<13|h>>>19)^h>>>10^(h<<15|h>>>17))|0,t[i+((11&U)<<2)>>2]=h,U=(h=(0|t[50576+(U<<2)>>2])+u+h+(M&N^F&~M)+((M<<26|M>>>6)^(M<<21|M>>>11)^(M<<7|M>>>25))|0)+n|0,h=((D<<30|D>>>2)^(D<<19|D>>>13)^(D<<10|D>>>22))+(D&R^Z&R^D&Z)+h|0,y=4|Q,V=0|t[i+((5|W)<<2)>>2],Y=0|t[i+((y+14&10)<<2)>>2],Y=(0|t[i+((12&y)<<2)>>2])+(0|t[i+((y+9&13)<<2)>>2])+((V<<14|V>>>18)^V>>>3^(V<<25|V>>>7))+((Y<<13|Y>>>19)^Y>>>10^(Y<<15|Y>>>17))|0,t[i+((12&y)<<2)>>2]=Y,E=(Y=(0|t[50576+(y<<2)>>2])+F+Y+(U&M^N&~U)+((U<<26|U>>>6)^(U<<21|U>>>11)^(U<<7|U>>>25))|0)+R|0,o=((h<<30|h>>>2)^(h<<19|h>>>13)^(h<<10|h>>>22))+(h&Z^D&Z^h&D)+Y|0,R=0|t[i+(((Y=5|Q)+1&14)<<2)>>2],F=0|t[i+((Y+14&11)<<2)>>2],F=(0|t[i+((13&Y)<<2)>>2])+(0|t[i+((Y+9&14)<<2)>>2])+((R<<14|R>>>18)^R>>>3^(R<<25|R>>>7))+((F<<13|F>>>19)^F>>>10^(F<<15|F>>>17))|0,t[i+((13&Y)<<2)>>2]=F,c=(F=(0|t[50576+(Y<<2)>>2])+N+F+(E&U^M&~E)+((E<<26|E>>>6)^(E<<21|E>>>11)^(E<<7|E>>>25))|0)+Z|0,B=((o<<30|o>>>2)^(o<<19|o>>>13)^(o<<10|o>>>22))+(o&D^h&D^(N=o&h))+F|0,F=6|Q,W=0|t[i+((7|W)<<2)>>2],Z=0|t[i+((F+14&12)<<2)>>2],Z=(0|t[i+((14&F)<<2)>>2])+(0|t[i+((F+9&15)<<2)>>2])+((W<<14|W>>>18)^W>>>3^(W<<25|W>>>7))+((Z<<13|Z>>>19)^Z>>>10^(Z<<15|Z>>>17))|0,t[i+((14&F)<<2)>>2]=Z,l=(Z=(0|t[50576+(F<<2)>>2])+M+Z+(c&E^U&~c)+((c<<26|c>>>6)^(c<<21|c>>>11)^(c<<7|c>>>25))|0)+D|0,a=((B<<30|B>>>2)^(B<<19|B>>>13)^(B<<10|B>>>22))+(B&h^N^(M=B&o))+Z|0,N=0|t[i+(((Z=7|Q)+1&8)<<2)>>2],D=0|t[i+((Z+14&13)<<2)>>2],D=(0|t[i+((15&Z)<<2)>>2])+(0|t[i+((Z+9&8)<<2)>>2])+((N<<14|N>>>18)^N>>>3^(N<<25|N>>>7))+((D<<13|D>>>19)^D>>>10^(D<<15|D>>>17))|0,t[i+((15&Z)<<2)>>2]=D,u=(D=(0|t[50576+(Z<<2)>>2])+U+D+(l&c^E&~l)+((l<<26|l>>>6)^(l<<21|l>>>11)^(l<<7|l>>>25))|0)+h|0,n=((a<<30|a>>>2)^(a<<19|a>>>13)^(a<<10|a>>>22))+(a&(B^o)^M)+D|0,Q=Q+8|0}while(Q>>>0<64);if(r=r+-1|0,I=n+I|0,t[A>>2]=I,g=a+g|0,t[A+4>>2]=g,w=B+w|0,t[A+8>>2]=w,b=o+b|0,t[A+12>>2]=b,k=u+k|0,t[A+16>>2]=k,s=l+s|0,t[A+20>>2]=s,C=c+C|0,t[A+24>>2]=C,d=E+d|0,t[A+28>>2]=d,!r)break;e=e+64|0}G=i}else G=i}function Zn(A,e){A|=0;var r=0,n=0;switch(r=0|t[(e|=0)+104>>2],i[e+40+r>>0]=-128,(r+1|0)>>>0>56?(Df(e+40+(r+1)|0,0,63-r|0),Gn(e,e+40|0,1),r=0):r=r+1|0,Df(e+40+r|0,0,56-r|0),r=0|t[e+36>>2],i[e+96>>0]=r>>>24,i[e+96+1>>0]=r>>>16,i[e+96+2>>0]=r>>>8,i[e+96+3>>0]=r,r=0|t[e+32>>2],i[e+100>>0]=r>>>24,i[e+100+1>>0]=r>>>16,i[e+100+2>>0]=r>>>8,i[e+100+3>>0]=r,Gn(e,e+40|0,1),t[e+104>>2]=0,$r(e+40|0,64),0|(r=0|t[e+108>>2])){case 28:return r=0|t[e>>2],i[A>>0]=r>>>24,i[A+1>>0]=r>>>16,i[A+2>>0]=r>>>8,i[A+3>>0]=r,r=0|t[e+4>>2],i[A+4>>0]=r>>>24,i[A+5>>0]=r>>>16,i[A+6>>0]=r>>>8,i[A+7>>0]=r,r=0|t[e+8>>2],i[A+8>>0]=r>>>24,i[A+9>>0]=r>>>16,i[A+10>>0]=r>>>8,i[A+11>>0]=r,r=0|t[e+12>>2],i[A+12>>0]=r>>>24,i[A+13>>0]=r>>>16,i[A+14>>0]=r>>>8,i[A+15>>0]=r,r=0|t[e+16>>2],i[A+16>>0]=r>>>24,i[A+17>>0]=r>>>16,i[A+18>>0]=r>>>8,i[A+19>>0]=r,r=0|t[e+20>>2],i[A+20>>0]=r>>>24,i[A+21>>0]=r>>>16,i[A+22>>0]=r>>>8,i[A+23>>0]=r,e=0|t[e+24>>2],i[A+24>>0]=e>>>24,i[A+25>>0]=e>>>16,i[A+26>>0]=e>>>8,i[A+27>>0]=e,0|(e=1);case 32:return r=0|t[e>>2],i[A>>0]=r>>>24,i[A+1>>0]=r>>>16,i[A+2>>0]=r>>>8,i[A+3>>0]=r,r=0|t[e+4>>2],i[A+4>>0]=r>>>24,i[A+5>>0]=r>>>16,i[A+6>>0]=r>>>8,i[A+7>>0]=r,r=0|t[e+8>>2],i[A+8>>0]=r>>>24,i[A+9>>0]=r>>>16,i[A+10>>0]=r>>>8,i[A+11>>0]=r,r=0|t[e+12>>2],i[A+12>>0]=r>>>24,i[A+13>>0]=r>>>16,i[A+14>>0]=r>>>8,i[A+15>>0]=r,r=0|t[e+16>>2],i[A+16>>0]=r>>>24,i[A+17>>0]=r>>>16,i[A+18>>0]=r>>>8,i[A+19>>0]=r,r=0|t[e+20>>2],i[A+20>>0]=r>>>24,i[A+21>>0]=r>>>16,i[A+22>>0]=r>>>8,i[A+23>>0]=r,r=0|t[e+24>>2],i[A+24>>0]=r>>>24,i[A+25>>0]=r>>>16,i[A+26>>0]=r>>>8,i[A+27>>0]=r,e=0|t[e+28>>2],i[A+28>>0]=e>>>24,i[A+29>>0]=e>>>16,i[A+30>>0]=e>>>8,i[A+31>>0]=e,0|(e=1);default:if(r>>>0>32)return 0|(e=0);if(r>>>0<=3)return 0|(e=1);for(r=A,A=0;;){if(n=0|t[e+(A<<2)>>2],i[r>>0]=n>>>24,i[r+1>>0]=n>>>16,i[r+2>>0]=n>>>8,i[r+3>>0]=n,(A=A+1|0)>>>0>=(0|t[e+108>>2])>>>2>>>0){r=1;break}r=r+4|0}return 0|r}return 0}function Nn(A,e,r){e|=0;var n=0,f=0,a=0;if(!(r|=0))return 1;f=0|t[(A|=0)+32>>2],n=0|t[A+36>>2],(f+(r<<3)|0)>>>0<f>>>0&&(t[A+36>>2]=n+1,n=n+1|0),t[A+36>>2]=n+(r>>>29),t[A+32>>2]=f+(r<<3),a=0|t[A+104>>2];do{if(a){if((a+r|r)>>>0>63){If(A+40+a|0,0|e,64-a|0),Gn(A,A+40|0,1),f=(n=A+40|0)+68|0;do{i[n>>0]=0,n=n+1|0}while((0|n)<(0|f));n=e+(64-a)|0,r=r-(64-a)|0;break}return If(A+40+a|0,0|e,0|r),t[A+104>>2]=(0|t[A+104>>2])+r,1}n=e}while(0);return a=-64&r,(f=r>>>6)&&(Gn(A,n,f),n=n+a|0,r=r-a|0),r?(t[A+104>>2]=r,If(A+40|0,0|n,0|r),1):1}function Fn(A){(A|=0)&&(Xn(0|t[A+4>>2],82796,0),Xn(A,82796,0))}function Wn(){var A,e=0;return 0|(A=0|Sn(20,82796,0))&&(e=0|Sn(16,82796,0),t[A+4>>2]=e,0|e)?(t[A+16>>2]=0,t[A+12>>2]=4,0|(e=A)):(Xn(A,82796,0),0|(e=0))}function Yn(A,e){var r,i,n=0;return 0==(0|(A|=0))|(0|(e|=0))<0||(0|(n=0|t[A>>2]))<=(0|e)?0|(A=0):(r=0|t[A+4>>2],i=0|t[r+(e<<2)>>2],(n+-1|0)==(0|e)?n=n+-1|0:(gf(r+(e<<2)|0,r+(e+1<<2)|0,(n-e<<2)-4|0),n=(0|t[A>>2])-1|0),t[A>>2]=n,0|(A=i))}function Rn(A,e){A|=0,e|=0;var r,i=0,n=0,f=0;r=G,G=G+16|0,t[r>>2]=e;A:do{if(A){if(!(i=0|t[A+16>>2])){if((0|(f=0|t[A>>2]))<=0){i=-1;break}for(n=0|t[A+4>>2],i=0;;){if((0|t[n+(i<<2)>>2])==(0|e))break A;if((0|(i=i+1|0))>=(0|f)){i=-1;break A}}}0|t[A+8>>2]||(pt(0|t[A+4>>2],0|t[A>>2],4,i),t[A+8>>2]=1),i=0!=(0|e)&&0!=(0|(n=0|_r(r,0|t[A+4>>2],0|t[A>>2],4,0|t[A+16>>2],2)))?n-(0|t[A+4>>2])>>2:-1}else i=-1}while(0);return G=r,0|i}function yn(A,e){e|=0;var r,i=0,n=0,f=0;if(r=0|t[(A|=0)>>2],!A)return 0|(f=0);if((0|r)<0|2147483647==(0|r))return 0|(f=0);if((i=0|t[A+12>>2])>>>0<=(r+1|0)>>>0){if(i<<1>>>0<i>>>0|i<<1>>>0>1073741823)return 0|(f=0);if(!(n=0|Tn(0|t[A+4>>2],i<<3,82796,0)))return 0|(f=0);t[A+4>>2]=n,t[A+12>>2]=i<<1,(0|(i=0|t[A>>2]))>(0|r)?(gf(n+(r+1<<2)|0,n+(r<<2)|0,i-r<<2|0),t[(0|t[A+4>>2])+(r<<2)>>2]=e,i=0|t[A>>2]):f=8}else n=0|t[A+4>>2],i=r,f=8;return 8==(0|f)&&(t[n+(i<<2)>>2]=e),f=i+1|0,t[A>>2]=f,t[A+8>>2]=0,0|f}function Vn(A){var e,r;return(A|=0)?(0|(e=0|t[A>>2]))<1?0|(A=0):(r=0|t[(0|t[A+4>>2])+(e+-1<<2)>>2],t[A>>2]=e+-1,0|(A=r)):0|(A=0)}function mn(A,e){e|=0;var r=0,i=0,n=0;if(A|=0){if((0|(r=0|t[A>>2]))>0){n=0;do{(i=0|t[(0|t[A+4>>2])+(n<<2)>>2])&&(NB[31&e](i),r=0|t[A>>2]),n=n+1|0}while((0|n)<(0|r))}Xn(0|t[A+4>>2],82796,0),Xn(A,82796,0)}}function Hn(A){return 0|(A=(A|=0)?0|t[A>>2]:-1)}function vn(A,e){return 0==(0|(A|=0))|(0|(e|=0))<0||(0|t[A>>2])<=(0|e)?0|(A=0):0|(A=0|t[(0|t[A+4>>2])+(e<<2)>>2])}function Jn(A,e,r){return r|=0,0==(0|(A|=0))|(0|(e|=0))<0||(0|t[A>>2])<=(0|e)?0|(A=0):(t[(0|t[A+4>>2])+(e<<2)>>2]=r,0|(A=0|t[(0|t[A+4>>2])+(e<<2)>>2]))}function Xn(A,e,r){af(A|=0)}function Tn(A,e,r,i){A|=0,e|=0,r|=0;do{if(0|A){if(0|e){r=0|Bf(A,e);break}af(A),r=0;break}e?(0|t[13247]&&(t[13247]=0),r=0|ff(e)):r=0}while(0);return 0|r}function Ln(A,e,r){return 0,0,(A|=0)?(0|t[13247]&&(t[13247]=0),0|(A=0|ff(A))):0|(A=0)}function Sn(A,e,r){return e|=0,0,(A|=0)?(0|t[13247]&&(t[13247]=0),(e=0|ff(A))?(Df(0|e,0,0|A),0|(A=e)):0|(A=0)):0|(A=0)}function pn(A,e,r,i){if(e|=0,0,0,A|=0)return e?($r(A,e),void af(A)):void af(A)}function zn(A,e){return A|=0,e|=0,0|t[20540]?(Xe(15,116,70,0,0),0|(A=0)):(e=0!=(0|m(82164,3)))&0!=(0|t[20542])&&(e=0!=(0|m(82172,4)))&0!=(0|t[20544])?(e=0|v(52992))||0==(0|(e=0|Sn(8,82796)))||0!=(0|J(52992,e))?e?(0==(1&A|0)&!0||(t[e>>2]=1),0==(2&A|0)&!0||(t[e+4>>2]=1),0|(A=1)):0|(A=0):(Xn(e),0|(A=0)):0|(A=0)}function Kn(A,e,r){return A|=0,e|=0,r|=0,0|t[20540]?0==(262144&A|0)&!0?(Xe(15,116,70,0,0),0|(A=0)):0|(A=0):(e=0!=(0|m(82164,3)))&0!=(0|t[20542])?0==(262144&A|0)&!0&&!((e=0!=(0|m(82172,4)))&0!=(0|t[20544]))?0|(A=0):(0==(1&A|0)&!0||(e=0!=(0|m(82192,6)))&0!=(0|t[20549]))&&(0==(2&A|0)&!0||(e=0!=(0|m(82192,7)))&0!=(0|t[20550]))&&(0==(16&A|0)&!0||(e=0!=(0|m(82204,8)))&0!=(0|t[20552]))&&(0==(4&A|0)&!0||(e=0!=(0|m(82204,9)))&0!=(0|t[20553]))&&(0==(32&A|0)&!0||(e=0!=(0|m(82216,8)))&0!=(0|t[20552]))&&(0==(8&A|0)&!0||(e=0!=(0|m(82216,10)))&0!=(0|t[20555]))&&(0==(128&A|0)&!0||(e=0!=(0|m(82224,11)))&0!=(0|t[20557]))&&(0==(64&A|0)&!0||(R(0|t[20546]),e=r?0|t[r>>2]:0,t[20558]=e,e=0!=(0|m(82224,12)),r=0|t[20559],y(0|t[20546]),e&0!=(0|r)))&&(0==(256&A|0)&!0||(A=0!=(0|m(82240,13)))&0!=(0|t[20561]))?0|(A=1):0|(A=0):0|(A=0)}function jn(A){A|=0;var e=0,r=0,i=0,n=0;if(A=0|t[20562]){if(0|t[20563]){for(e=0;;){if(Xn(0|t[A+(12*e|0)>>2]),n=A+(12*e|0)+8|0,A=0|t[(i=A+(12*e|0)+4|0)>>2],0|t[n>>2]){r=0;do{Xn(0|t[A+(r<<3)>>2]),Xn(0|t[(0|t[i>>2])+(r<<3)+4>>2]),r=r+1|0,A=0|t[i>>2]}while(r>>>0<(0|t[n>>2])>>>0)}if(Xn(A),(e=e+1|0)>>>0>=(0|t[20563])>>>0)break;A=0|t[20562]}A=0|t[20562]}Xn(A),t[20562]=0,t[20563]=0}}function xn(A,e){var r;return r=G,G=G+16|0,0==(0|(A|=0))|0==(0|(e|=0))?(G=r,0|(e=0)):(t[r+4>>2]=0,t[r>>2]=e,e=0|ye(0|t[A+8>>2],r),G=r,0|e)}function On(A,e){var r;return r=G,G=G+16|0,0==(0|(A|=0))|0==(0|(e|=0))?(G=r,0|(e=0)):(t[r+4>>2]=0,t[r>>2]=e,(A=0|ye(0|t[A+8>>2],r))?(e=0|t[A+8>>2],G=r,0|e):(G=r,0|(e=0)))}function Pn(A,e,r){A|=0,r|=0;var i=0;return i=0|t[(e|=0)+8>>2],t[r>>2]=t[e>>2],0|yn(i,r)?(A=0|We(0|t[A+8>>2],r))?(function(A,e){e|=0;var r,i,n=0,f=0,a=0;if((0|(r=0|t[(A|=0)>>2]))<=0)return 0|(a=0);for(i=0|t[A+4>>2],n=0;(0|t[(f=i+(n<<2)|0)>>2])!=(0|e);)if((0|r)<=(0|(n=n+1|0))){n=0,a=8;break}8==(0|a)||((r+-1|0)!=(0|n)&&(gf(0|f,i+(n+1<<2)|0,(r-n<<2)-4|0),n=(0|t[A>>2])-1|0),t[A>>2]=n,a=e)}(i,A),Xn(0|t[A+4>>2]),Xn(0|t[A+8>>2]),Xn(A),0|(i=1)):0|(i=1):0|(i=0)}function _n(A,e,r){A|=0,e|=0,r|=0;var i,n=0,f=0;i=G,G=G+16|0;do{if(r){if(!A){n=0|wt(r);break}if(e){if(t[i+4>>2]=r,t[i>>2]=e,0|(f=0|ye(0|t[A+8>>2],i))){n=0|t[f+8>>2];break}if(0==(0|Wt(e,80650))&&0!=(0|(n=0|wt(r))))break;n=A+8|0,f=i+4|0,A=i}else n=A+8|0,f=i+4|0,A=i;t[A>>2]=80850,t[f>>2]=r,n=(n=0|ye(0|t[n>>2],i))?0|t[n+8>>2]:0}else n=0}while(0);return G=i,0|n}function qn(A){A|=0;var e=0;do{if(A){if(0==(0|t[A+8>>2])&&(e=0|Ze(20,34),t[A+8>>2]=e,0==(0|e))){A=0;break}A=1}else A=0}while(0);return 0|A}function $n(A){var e=0;(A|=0)&&(e=0|t[A+8>>2])&&(ve(e,0),me(e=0|t[A+8>>2],3,e),Ve(0|t[A+8>>2],21),Fe(0|t[A+8>>2]))}function At(A,e){A|=0,e|=0;var r,i=0,n=0,f=0;r=0|Wn();do{if(r){if(!(i=0|Ln(12))){Fn(r),i=0;break}if(f=0|Ln(n=1+(0|mt(e))|0),t[i>>2]=f,0|f&&(If(0|f,0|e,0|n),t[i+4>>2]=0,t[i+8>>2]=r,0|We(0|t[A+8>>2],i)&&oi(),(0|Je(0|t[A+8>>2]))<=0))return 0|(e=i);Fn(r),Xn(0|t[i>>2])}else Fn(0),i=0}while(0);return Xn(i),0|(e=0)}function et(A,e){A|=0,e|=0;do{if(A){if(e){A=0|On(A,e);break}Xe(14,108,107,0,0),A=0;break}Xe(14,108,105,0,0),A=0}while(0);return 0|A}function rt(A,e,r){var i,n;return n=G,G=G+16|0,0|(i=0|_n(A|=0,e|=0,r|=0))?(G=n,0|(r=i)):A?(Xe(14,109,108,0,0),t[n>>2]=80683,t[n+4>>2]=e,t[n+8>>2]=80690,t[n+12>>2]=r,Ke(4,n),G=n,0|(r=0)):(Xe(14,109,106,0,0),G=n,0|(r=0))}function it(A){return(A|=0)||(A=52996),0|(A=0|IB[31&t[A+4>>2]](A))?0|A:(Xe(14,111,65,0,0),0|(A=0))}function nt(A){(A|=0)&&IB[31&t[12+(0|t[A>>2])>>2]](A)}function tt(A,e,r){return e|=0,r|=0,(A|=0)?0|(e=0|DB[31&t[36+(0|t[A>>2])>>2]](A,e,r)):(Xe(14,113,105,0,0),0|(e=0))}function ft(A,e,r){r|=0;var i,n,f=0,a=0,B=0,o=0,u=0,l=0,c=0;if(n=G,G=G+80|0,!(A|=0))return G=n,0|(r=1);if(0!=(0|(e|=0))&&0==(32&r|0)|0!=(0|(f=0|rt(A,0,e)))||(f=0|rt(A,0,80697)),!f)return Se(),G=n,0|(r=1);if(!(i=0|et(A,f)))return G=n,0|(r=0);if((0|Hn(i))<=0)return G=n,0|(r=1);for(c=0;;){u=0|vn(i,c),l=0|t[u+4>>2],u=0|t[u+8>>2],a=(f=0|ef(l,46))?f-l|0:0|mt(l);A:do{if((0|Hn(0|t[20565]))>0){for(e=0;f=0|vn(0|t[20565],e),e=e+1|0,0|Af(0|t[f+4>>2],l,a);)if((0|e)>=(0|Hn(0|t[20565]))){f=0;break A}}else f=0}while(0);e=0==(0|f);do{if(0==(8&r|0)&e){if((f=0|rt(A,u,80710))||(Se(),f=l),a=0|vr(0,f,0,0))if(e=0|Xr(a,80715)){if(e=0|at(a,l,e,0|Xr(a,80728))){f=e,o=28;break}e=0}else e=112;else e=110;Hr(a),Xe(14,117,e,0,0),t[n>>2]=80743,t[n+4>>2]=l,t[n+8>>2]=80751,t[n+12>>2]=f,Ke(4,n),o=26}else o=e?26:28}while(0);A:do{if(26==(0|o))4&r||(Xe(14,118,113,0,0),t[n+16>>2]=80743,t[n+16+4>>2]=l,Ke(2,n+16|0)),o=43;else if(28==(0|o)){if(o=0,0|(B=0|Ln(20))){if(t[B>>2]=f,t[B+4>>2]=0|kt(l,82796,0),a=0|kt(u,82796,0),t[B+8>>2]=a,t[B+16>>2]=0,!(0==(0|a)|0==(0|t[B+4>>2]))){(e=0|t[f+8>>2])?(0|gB[63&e](B,A))<1?e=1:(a=1,o=32):(a=0,o=32);do{if(32==(0|o)){if(o=0,!(e=0|t[20566])&&(e=0|Wn(),t[20566]=e,!e)){Xe(14,115,65,0,0),e=a;break}if(0|yn(e,B)){t[(l=f+16|0)>>2]=1+(0|t[l>>2]);break A}Xe(14,115,65,0,0),e=a;break}}while(0);0!=(0|e)&0!=(0|(f=0|t[f+12>>2]))&&NB[31&f](B)}Xn(0|t[B+4>>2]),Xn(0|t[B+8>>2]),Xn(B)}4&r||(Xe(14,118,109,0,0),t[n+24>>2]=-1,Ge(n+56|0,13,80759,n+24|0),t[n+32>>2]=80743,t[n+32+4>>2]=l,t[n+32+8>>2]=80764,t[n+32+12>>2]=u,t[n+32+16>>2]=80773,t[n+32+20>>2]=n+56,Ke(6,n+32|0)),o=43}}while(0);if(43==(0|o)&&0==(1&r|0)){f=-1,o=45;break}if((0|(c=c+1|0))>=(0|Hn(i))){f=1,o=45;break}}return 45==(0|o)?(G=n,0|f):0}function at(A,e,r,i){A|=0,e|=0,r|=0,i|=0;var n=0;return 0==(0|t[20565])&&(n=0|Wn(),t[20565]=n,0==(0|n))?0|(e=0):(n=0|Sn(24,82796))?(t[n>>2]=A,e=0|kt(e,82796,0),t[n+4>>2]=e,t[n+8>>2]=r,t[n+12>>2]=i,e?0|yn(0|t[20565],n)?0|(e=n):(Xn(0|t[n+4>>2]),Xn(n),0|(e=0)):(Xn(n),0|(e=0))):0|(e=0)}function Bt(){var A=0,e=0;return 0|(A=0|wt(80784))?0|(e=0|kt(A,82796,0)):(e=0|Ln(A=13+(0|mt(80519))|0))?(dt(e,80519,A),It(e,80797,A),It(e,80799,A),0|e):0|(e=0)}function ot(){var A=0,e=0,r=0;if(r=(0|Hn(0|t[20566]))>0,A=0|t[20566],!r)return Fn(r=A),void(t[20566]=0);do{0|(r=0|Vn(A))&&(A=0|t[r>>2],(e=0|t[A+12>>2])&&(NB[31&e](r),A=0|t[r>>2]),t[(e=A+16|0)>>2]=(0|t[e>>2])-1,Xn(0|t[r+4>>2]),Xn(0|t[r+8>>2]),Xn(r)),r=(0|Hn(0|t[20566]))>0,A=0|t[20566]}while(r);Fn(A),t[20566]=0}function ut(A,e,r){return 0!=(0|at(0,A|=0,e|=0,r|=0))|0}function lt(A){return 0|t[(A|=0)+8>>2]}function ct(A,e,r,n,t){e|=0,r|=0,n|=0,t|=0;var a=0,B=0;if(!(A|=0))return Xe(14,119,115,0,0),0|(e=0);if(!r){for(a=A;;){if((0|(B=0|qt(a,e)))!=(0|a)&&0!=(0|i[a>>0])?(r=B||a+(0|mt(a))|0,r=0|DB[31&n](a,1-a+(r+-1)|0,t)):r=0|DB[31&n](0,0,t),(0|r)<1){a=27;break}if(!B){r=1,a=27;break}a=B+1|0}if(27==(0|a))return 0|r}for(;;){r=0|i[A>>0];A:do{if(r<<24>>24)for(;;){if(!(0|Yt(255&r)))break A;if(!((r=0|i[(A=A+1|0)>>0])<<24>>24)){r=0;break}}else r=0}while(0);if((0|(a=0|qt(A,e)))==(0|A)|r<<24>>24==0)r=0|DB[31&n](0,0,t);else{r=a||A+(0|mt(A))|0;do{r=r+-1|0}while(0!=(0|Yt(0|f[r>>0])));r=0|DB[31&n](A,1-A+r|0,t)}if((0|r)<1){a=27;break}if(!a){r=1,a=27;break}A=a+1|0}return 27==(0|a)?0|r:0}function Et(A,e,r){A|=0,e|=0,r|=0;var B,o,u,l=0,c=0,E=0,Q=0,w=0,b=0,k=0,s=0,C=0,d=0,I=0,g=0;u=G,G=G+48|0,t[u+16>>2]=0,o=0|t[A+8>>2],B=0|Mt();A:do{if(B){if(l=0|kt(80850,82796,0),t[u+16>>2]=l,!l){Xe(14,121,65,0,0),E=0,l=0,c=0;break}if(!(0|qn(A))){Xe(14,121,65,0,0),E=0,l=0,c=0;break}if(!(c=0|At(A,l))){Xe(14,121,103,0,0),E=0,l=0,c=0;break}e:do{if(0|Ut(B,512)){l=0;r:for(;;){for(Q=0,E=0;;){if(b=(0|t[B+4>>2])+E|0,i[b>>0]=0,de(e,b,511),i[b+511>>0]=0,!((k=0|mt(b))|Q)){g=113;break r}i:do{if((0|k)>0)for(Q=k;;){switch(0|i[b+(w=Q+-1|0)>>0]){case 10:case 13:break;default:w=Q;break i}if(!((0|Q)>1))break i;Q=w}else w=k}while(0);if(0!=(0|k)&(0|w)==(0|k)?Q=1:(i[b+w>>0]=0,Q=0,l=l+1|0),(0|(E=w+E|0))>0&&(I=(0|t[B+4>>2])+(E+-1)|0,d=0|t[A+4>>2],0!=(32&n[d+(f[I>>0]<<1)>>1]))?(0|E)>=2&&0!=(32&n[d+(f[I+-1>>0]<<1)>>1])?g=24:(Q=1,E=E+-1|0):g=24,24==(0|g))if(g=0,Q)Q=1;else{s=0|t[B+4>>2],C=0|t[A+4>>2],E=0|i[s>>0],Q=0|n[C+((255&E)<<1)>>1];i:do{if(2048&Q)E=s,g=51;else{for(k=Q,w=E,b=65535&Q,E=s;16&b;){if(w=0|i[(E=E+1|0)>>0],2048&(b=65535&(Q=0|n[C+((255&w)<<1)>>1]))|0){g=51;break i}k=Q}if(128&(Q=65535&k))g=51;else for(;;){n:do{if(1024&Q){Q=0|i[(b=E+1|0)>>0];t:do{if(!(8&n[C+((255&Q)<<1)>>1]))for(k=E;;){if(Q<<24>>24==w<<24>>24){if((0|i[(E=k+2|0)>>0])!=w<<24>>24){Q=w,E=k;break t}}else E=b;if(Q=0|i[(b=E+1|0)>>0],8&n[C+((255&Q)<<1)>>1])break;k=E}}while(0);E=Q<<24>>24==w<<24>>24?E+2|0:b}else{if(!(64&Q)){if(32&Q|0){E=0==(8&n[C+(f[(k=E+1|0)>>0]<<1)>>1])?E+2|0:k;break}if(8&Q|0)break i;E=E+1|0;break}if(Q=0|i[(k=E+1|0)>>0],b=0|a[C+((255&Q)<<1)>>1],(Q=Q<<24>>24==w<<24>>24)|0!=(8&b|0))w=k;else for(;;){if(32&b){if(8&n[C+(f[(E=E+2|0)>>0]<<1)>>1])break n}else E=k;if(Q=0|i[(k=E+1|0)>>0],b=0|a[C+((255&Q)<<1)>>1],(Q=Q<<24>>24==w<<24>>24)|0!=(8&b|0)){w=k;break}}E=Q?E+2|0:w}}while(0);if(w=0|i[E>>0],128&(Q=0|a[C+((255&w)<<1)>>1])|0){g=51;break}}}}while(0);for(51==(0|g)&&(g=0,i[E>>0]=0),b=0|t[A+4>>2];w=0|i[s>>0],Q=s+1|0,16==(24&(E=0|n[b+((255&w)<<1)>>1]));)s=Q;if(8&E)Q=0,E=0;else{if(w<<24>>24==91)break;for(Q=s;;){if(32&(E&=65535))Q=0==(8&n[b+(f[(C=Q+1|0)>>0]<<1)>>1])?Q+2|0:C;else{if(!(775&E))break;Q=Q+1|0}w=0|i[Q>>0],E=0|n[b+((255&w)<<1)>>1]}i:do{if(w<<24>>24==58){if(58!=(0|i[Q+1>>0])){w=58,k=s,C=0;break}for(i[Q>>0]=0,k=Q+2|0,b=0|t[A+4>>2],E=k;;)if(w=0|i[E>>0],32&(Q=0|a[b+((255&w)<<1)>>1]))E=0==(8&n[b+(f[(C=E+1|0)>>0]<<1)>>1])?E+2|0:C;else{if(!(775&Q)){Q=E,C=s;break i}E=E+1|0}}else k=s,C=0}while(0);if(E=Q+1|0,16==(24&n[b+((255&w)<<1)>>1]))do{w=0|i[E>>0],E=E+1|0}while(16==(24&n[b+((255&w)<<1)>>1]));if(w<<24>>24!=61){g=93;break r}for(i[Q>>0]=0,w=0|t[A+4>>2],s=E;16==(24&n[w+(f[s>>0]<<1)>>1]);)s=s+1|0;for(;!(8&n[w+(f[E>>0]<<1)>>1]);)E=E+1|0;Q=E+-1|0;i:do{if((0|Q)!=(0|s))for(;;){if(!(16&n[w+(f[Q>>0]<<1)>>1]))break i;if((0|(E=Q+-1|0))==(0|s)){E=Q;break}b=Q,Q=E,E=b}}while(0);if(i[E>>0]=0,!(b=0|Ln(12))){g=103;break r}if(Q=0|t[u+16>>2],w=0==(0|C)?Q:C,E=0|Ln(1+(0|mt(k))|0),t[b+4>>2]=E,t[b+8>>2]=0,!E){g=105;break r}if(dt(E,k,1+(0|mt(k))|0),!(0|Qt(A,w,b+8|0,s))){g=114;break r}do{if(0|Wt(w,Q)){if(0|(E=0|xn(A,w)))break;if(!(E=0|At(A,w))){g=110;break r}}else E=c}while(0);if(!(0|Pn(A,E,b))){g=112;break r}Q=0,E=0}}if(!(0|Ut(B,E+512|0)))break e}for(;c=0|i[Q>>0],16==(24&n[b+((255&c)<<1)>>1]);)Q=Q+1|0;k=Q;i:for(;;){for(E=k;;){if(32&(c=0|a[b+((255&c)<<1)>>1]))E=0==(8&n[b+(f[(C=E+1|0)>>0]<<1)>>1])?E+2|0:C;else{if(!(775&c))break;E=E+1|0}c=0|i[E>>0]}for(w=E;c=0|i[w>>0],16==(24&n[b+((255&c)<<1)>>1]);)w=w+1|0;switch(c<<24>>24){case 0:g=70;break r;case 93:break i}if((0|k)==(0|w)){g=70;break r}k=w}if(i[E>>0]=0,!(0|Qt(A,0,u+16|0,Q))){E=0,c=0;break A}if(!(c=0|xn(A,E=0|t[u+16>>2]))&&!(c=0|At(A,E))){g=74;break}if(!(0|Ut(B,512)))break e}if(70==(0|g)){Xe(14,121,100,0,0),E=0,c=0;break A}if(74==(0|g)){Xe(14,121,103,0,0),E=0,c=0;break A}if(93==(0|g)){Xe(14,121,101,0,0),E=0,c=0;break A}if(103==(0|g)){Xe(14,121,65,0,0),E=0,c=0;break A}if(105==(0|g)){Xe(14,121,65,0,0),E=b,c=b;break A}if(110==(0|g)){Xe(14,121,103,0,0),E=b,c=b;break A}if(112==(0|g)){Xe(14,121,65,0,0),E=b,c=b;break A}if(113==(0|g))return ht(B),Xn(0|t[u+16>>2]),G=u,0|(A=1);if(114==(0|g)){E=b,c=b;break A}}else l=0}while(0);Xe(14,121,7,0,0),E=0,c=0}else Xe(14,121,7,0,0),E=0,l=0,c=0}while(0);return ht(B),Xn(0|t[u+16>>2]),0|r&&(t[r>>2]=l),t[u>>2]=l,Ge(u+20|0,13,80858,u),t[u+8>>2]=80862,t[u+8+4>>2]=u+20,Ke(2,u+8|0),(0|o)!=(0|(l=0|t[A+8>>2]))&&(!function(A){A|=0;var e,r=0;e=G,G=G+16|0,(r=0|t[20564])||(r=52996,t[20564]=r),IB[31&t[r+8>>2]](e),t[e+8>>2]=A,IB[31&t[16+(0|t[e>>2])>>2]](e),G=e}(l),t[A+8>>2]=0),c?(Xn(0|t[c+4>>2]),Xn(0|t[c+8>>2]),Xn(E),G=u,0|(A=0)):(G=u,0|(A=0))}function Qt(A,e,r,f){A|=0,e|=0,r|=0,f|=0;var B,o=0,u=0,l=0,c=0,E=0,Q=0,w=0,b=0,k=0,s=0,C=0,d=0,I=0;if(!(B=0|Mt()))return 0|(r=0);do{if(0|Ut(B,1+(0|mt(f))|0)){I=0,o=0;A:for(;;){d=f;e:for(;;){c=0|t[A+4>>2],w=0|i[d>>0],f=0|a[c+((255&w)<<1)>>1];do{if(64&f){u=0|i[(l=d+1|0)>>0],f=0|a[c+((255&u)<<1)>>1];r:do{if(8&f|0||u<<24>>24==w<<24>>24)f=o;else for(Q=c,E=f,c=d;;){if(32&E){if(f=0|i[(l=c+2|0)>>0],8&n[Q+((255&f)<<1)>>1]){u=f,f=o;break r}u=f,c=l}else c=l;if(f=o+1|0,i[(0|t[B+4>>2])+o>>0]=u,l=c+1|0,Q=0|t[A+4>>2],u=0|i[l>>0],8&(E=0|a[Q+((255&u)<<1)>>1])|0||u<<24>>24==w<<24>>24)break;o=f}}while(0);u=u<<24>>24==w<<24>>24?l+1|0:l}else{if(1024&f|0){f=0|i[(l=d+1|0)>>0];r:do{if(8&n[c+((255&f)<<1)>>1])E=f,c=d,u=l,f=o;else for(u=f,c=d;;){if(u<<24>>24==w<<24>>24){if((0|i[(f=c+2|0)>>0])!=w<<24>>24){E=w,u=l,f=o;break r}u=w,c=f}else c=l;if(f=o+1|0,i[(0|t[B+4>>2])+o>>0]=u,o=0|i[(l=c+1|0)>>0],8&n[(0|t[A+4>>2])+((255&o)<<1)>>1]){E=o,u=l;break}u=o,o=f}}while(0);u=E<<24>>24==w<<24>>24?c+2|0:u;break}if(!(32&f)){if(8&f|0){f=53;break A}if(u=d+1|0,w<<24>>24==36)break e;i[(0|t[B+4>>2])+o>>0]=w,f=o+1|0;break}if(u=d+2|0,f=0|i[d+1>>0],8&n[c+((255&f)<<1)>>1]){f=53;break A}switch(((f+-98&255)>>>1|f+-98<<24>>24<<7&255)<<24>>24){case 8:f=13;break;case 6:f=10;break;case 0:f=8;break;case 9:f=9}i[(0|t[B+4>>2])+o>>0]=f,f=o+1|0}}while(0);d=u,o=f}for(b=Q=0|(k=(k=0|i[u>>0])<<24>>24==123?125:k<<24>>24==40?41:0)?d+2|0:u;w=0|i[b>>0],f=b+1|0,263&n[c+((255&w)<<1)>>1];)b=f;if(w<<24>>24==58)if(58==(0|i[f>>0]))for(i[b>>0]=0,c=b+2|0,f=0|t[A+4>>2],l=c;;){if(u=0|i[l>>0],!(263&n[f+((255&u)<<1)>>1])){C=u,E=Q,s=l,Q=w<<24>>24,u=b;break}l=l+1|0}else C=58,E=e,s=b,c=Q,Q=I,u=0;else C=w,E=e,s=b,c=Q,Q=I,u=0;if(i[s>>0]=0,0|k){if((0|k)!=(C<<24>>24|0)){f=38;break}f=s+1|0}else f=s;if(c=0|_n(A,E,c),0|u&&(i[u>>0]=Q),i[s>>0]=C,!c){f=43;break}if((u=(u=0|mt(c))+(d-f)+(0|t[B>>2])|0)>>>0>65536){f=45;break}if(!(0|Gt(B,u))){f=47;break}if((u=0|i[c>>0])<<24>>24)for(l=o;c=c+1|0,o=l+1|0,i[(0|t[B+4>>2])+l>>0]=u,(u=0|i[c>>0])<<24>>24;)l=o;i[s>>0]=C,I=Q}if(38==(0|f)){Xe(14,101,102,0,0);break}if(43==(0|f)){Xe(14,101,104,0,0);break}if(45==(0|f)){Xe(14,101,116,0,0);break}if(47==(0|f)){Xe(14,101,65,0,0);break}if(53==(0|f))return i[(0|t[B+4>>2])+o>>0]=0,Xn(0|t[r>>2]),t[r>>2]=t[B+4>>2],Xn(B),0|(r=1)}}while(0);return ht(B),0|(r=0)}function wt(A){return A|=0,0|function(){if((0|(0|function(){return 0|w()}()))!=(0|function(){return 0|k()}()))return 0|1;return 0|(0|(0|function(){return 0|b()}()))!=(0|function(){return 0|s()}())&1}()?0|(A=0):0|(A=0|h(0|A))}function bt(A,e,r){e|=0,r|=0;var i,n=0,f=0,a=0,B=0;if(i=G,G=G+48|0,(A|=0)>>>0>13)return Xe(15,113,7,0,0),Fn(e=0|t[r>>2]),t[r>>2]=0,void(G=i);if(!((B=0!=(0|m(82276,14)))&0!=(0|t[20570])))return Xe(15,113,65,0,0),Fn(e=0|t[r>>2]),t[r>>2]=0,void(G=i);if(!(n=0|t[20568]))return Fn(e=0|t[r>>2]),t[r>>2]=0,void(G=i);R(n),B=0|Hn(0|t[81968+(A<<2)>>2]);A:do{if((0|B)>0){if((0|B)>=10){if(!(n=0|Ln(B<<2)))for(y(0|t[20568]),a=0;;)if(R(0|t[20568]),f=0|vn(0|t[81968+(A<<2)>>2],a),y(0|t[20568]),0|f&&0|t[f+12>>2]&&(n=0!=(0|(n=0|t[r>>2]))&&(0|Hn(n))>(0|a)?0|vn(0|t[r>>2],a):0,YB[0&t[f+12>>2]](e,n,r,a,0|t[f>>2],0|t[f+4>>2])),(0|(a=a+1|0))==(0|B)){n=0;break A}}else n=i;f=0;do{t[n+(f<<2)>>2]=0|vn(0|t[81968+(A<<2)>>2],f),f=f+1|0}while((0|f)!=(0|B));if(y(0|t[20568]),(0|B)>0){A=0;do{0|(a=0|t[n+(A<<2)>>2])&&0|t[a+12>>2]&&(f=0!=(0|(f=0|t[r>>2]))&&(0|Hn(f))>(0|A)?0|vn(0|t[r>>2],A):0,YB[0&t[a+12>>2]](e,f,r,A,0|t[a>>2],0|t[a+4>>2])),A=A+1|0}while((0|A)!=(0|B))}if((0|n)==(0|i))return Fn(e=0|t[r>>2]),t[r>>2]=0,void(G=i)}else y(0|t[20568]),n=0}while(0);Xn(n),Fn(e=0|t[r>>2]),t[r>>2]=0,G=i}function kt(A,e,r){var i=0,n=0;return 0!=(0|(A|=0))&&0!=(0|(i=0|Ln(n=1+(0|mt(A))|0)))?If(0|i,0|A,0|n):i=0,0|i}function st(A,e){A|=0,e|=0;var r=0;A:do{if(e)for(r=A;;){if(e=e+-1|0,!(0|i[r>>0])){e=r;break A}if(r=r+1|0,!e){e=r;break}}else e=A}while(0);return e-A|0}function Ct(A,e,r,i){return r|=0,0,0==(0|(A|=0))|(e|=0)>>>0>2147483646?0|(e=0):(r=0|Ln(e))?(If(0|r,0|A,0|e),0|(e=r)):(Xe(15,115,65,0,0),0|(e=0))}function dt(A,e,r){A|=0,e|=0,r|=0;var n=0,t=0,f=0,a=0;A:do{if(r>>>0>1)for(n=0;;){if(!((t=0|i[e>>0])<<24>>24))break A;if(e=e+1|0,f=A+1|0,i[A>>0]=t,n=n+1|0,(r=r+-1|0)>>>0<=1){A=f,r=1,a=5;break}A=f}else n=0,a=5}while(0);return 5!=(0|a)||r?(i[A>>0]=0,a=n,0|(a=(f=0|mt(f=e))+a|0)):(a=n,0|(a=(f=0|mt(f=e))+a|0))}function It(A,e,r){A|=0,e|=0,r|=0;var n=0,t=0,f=0,a=0;A:do{if(r){for(n=0;0|i[A>>0];){if(n=n+1|0,!(r=r+-1|0)){t=0,r=n,A=e;break A}A=A+1|0}e:do{if(r>>>0>1)for(t=0;;){if(!((f=0|i[e>>0])<<24>>24))break e;if(e=e+1|0,a=A+1|0,i[A>>0]=f,t=t+1|0,(r=r+-1|0)>>>0<=1){A=a;break}A=a}else t=0}while(0);i[A>>0]=0,r=n,A=e}else t=0,r=0,A=e}while(0);return t+r+(0|mt(A))|0}function gt(A){switch((A|=0)<<24>>24){case 48:A=0;break;case 49:A=1;break;case 50:A=2;break;case 51:A=3;break;case 52:A=4;break;case 53:A=5;break;case 54:A=6;break;case 55:A=7;break;case 56:A=8;break;case 57:A=9;break;case 65:case 97:A=10;break;case 66:case 98:A=11;break;case 67:case 99:A=12;break;case 68:case 100:A=13;break;case 69:case 101:A=14;break;case 70:case 102:A=15;break;default:A=-1}return 0|A}function Dt(A,e){e|=0;var r,n=0,f=0,a=0,B=0;if(!(r=0|Ln((0|mt(A|=0))>>>1)))return Xe(15,118,65,0,0),0|(e=0);n=0|i[A>>0];A:do{if(n<<24>>24){for(B=r;;){for(;f=A+1|0,n<<24>>24==58;){if(!((n=0|i[f>>0])<<24>>24)){n=B;break A}A=f}if(A=A+2|0,!((f=0|i[f>>0])<<24>>24)){A=9;break}if(f=0|gt(f),((n=0|gt(n))|f|0)<0){A=11;break}if(a=B+1|0,i[B>>0]=n<<4|f,!((n=0|i[A>>0])<<24>>24)){n=a;break A}B=a}if(9==(0|A))return Xe(15,118,103,0,0),Xn(r),0|(e=0);if(11==(0|A))return Xn(r),Xe(15,118,102,0,0),0|(e=0)}else n=r}while(0);return e?(t[e>>2]=n-r,0|(e=r)):0|(e=r)}function Mt(){var A=0;return(A=0|Sn(16,82796))?0|A:(Xe(7,101,65,0,0),0|(A=0))}function ht(A){var e,r=0;if(A|=0){e=0|t[A+4>>2];do{if(0|e){if(r=0|t[A+8>>2],1&t[A+12>>2]){ri(e,r,82796,0);break}pn(e,r);break}}while(0);Xn(A)}}function Ut(A,e){e|=0;var r=0,i=0,n=0,f=0;if((i=0|t[(A|=0)>>2])>>>0>=e>>>0)return t[A>>2]=e,0|(A=e);if((0|t[A+8>>2])>>>0>=e>>>0)return 0|(r=0|t[A+4>>2])&&Df(r+i|0,0,e-i|0),t[A>>2]=e,0|(A=e);if(e>>>0>1610612732)return Xe(7,100,65,0,0),0|(A=0);1&t[A+12>>2]?0!=(0|(i=0|Ai((((e+3|0)>>>0)/3|0)<<2,82796,0)))&0!=(0|(r=0|t[A+4>>2]))?(If(0|i,0|r,0|t[A>>2]),ri(0|t[A+4>>2],0|t[A>>2],82796,0),t[A+4>>2]=0,n=A+4|0,r=i):(r=i,f=13):(r=0|Tn(0|t[A+4>>2],(((e+3|0)>>>0)/3|0)<<2,82796),f=13);do{if(13==(0|f)){if(0|r){n=A+4|0;break}return Xe(7,100,65,0,0),0|(A=0)}}while(0);return t[n>>2]=r,t[A+8>>2]=(((e+3|0)>>>0)/3|0)<<2,Df(r+(f=0|t[A>>2])|0,0,e-f|0),t[A>>2]=e,0|(A=e)}function Gt(A,e){e|=0;var r=0,i=0,n=0,f=0;if((i=0|t[(A|=0)>>2])>>>0>=e>>>0)return 0|(r=0|t[A+4>>2])&&Df(r+e|0,0,i-e|0),t[A>>2]=e,0|(A=e);if((r=0|t[A+8>>2])>>>0>=e>>>0)return Df((0|t[A+4>>2])+i|0,0,e-i|0),t[A>>2]=e,0|(A=e);if(e>>>0>1610612732)return Xe(7,105,65,0,0),0|(A=0);1&t[A+12>>2]?0!=(0|(i=0|Ai((((e+3|0)>>>0)/3|0)<<2,82796,0)))&0!=(0|(r=0|t[A+4>>2]))?(If(0|i,0|r,0|t[A>>2]),ri(0|t[A+4>>2],0|t[A>>2],82796,0),t[A+4>>2]=0,n=A+4|0,r=i):(r=i,f=13):(r=0|function(A,e,r,i,n){return e|=0,r|=0,0,0,(A|=0)?r?e>>>0>r>>>0?($r(A+r|0,e-r|0),0|A):(0|t[13247]&&(t[13247]=0),(r=0|ff(r))?(If(0|r,0|A,0|e),e?($r(A,e),af(A),0|(A=r)):(af(A),0|(A=r))):0|(A=0)):e?($r(A,e),af(A),0|(A=0)):(af(A),0|(A=0)):r?(0|t[13247]&&(t[13247]=0),0|(A=0|ff(r))):0|(A=0)}(0|t[A+4>>2],r,(((e+3|0)>>>0)/3|0)<<2),f=13);do{if(13==(0|f)){if(0|r){n=A+4|0;break}return Xe(7,105,65,0,0),0|(A=0)}}while(0);return t[n>>2]=r,t[A+8>>2]=(((e+3|0)>>>0)/3|0)<<2,Df(r+(f=0|t[A>>2])|0,0,e-f|0),t[A>>2]=e,0|(A=e)}function Zt(A,e,r){r=+r,Z()}function Nt(A,e){return N(A|=0,e|=0),1}function Ft(){return 82284}function Wt(A,e){e|=0;var r=0,n=0;if(r=0|i[(A|=0)>>0],n=0|i[e>>0],r<<24>>24!=0&&r<<24>>24==n<<24>>24)do{e=e+1|0,r=0|i[(A=A+1|0)>>0],n=0|i[e>>0]}while(r<<24>>24!=0&&r<<24>>24==n<<24>>24);return(255&r)-(255&n)|0}function Yt(A){return 1&(32==(0|(A|=0))|(A+-9|0)>>>0<5)|0}function Rt(A){return 1&(((32|(A|=0))-97|0)>>>0<6|0!=(0|yt(A)))|0}function yt(A){return((A|=0)+-48|0)>>>0<10|0}function Vt(A){return 0|(0==(0|function(A){return((A|=0)+-65|0)>>>0<26|0}(A|=0))?A:32|A)}function mt(A){A|=0;var e=0,r=0,n=0;A:do{if(3&A)for(e=A,r=A;;){if(!(0|i[r>>0]))break A;if(!(3&(e=r=r+1|0))){e=r,n=5;break}}else e=A,n=5}while(0);if(5==(0|n)){for(;!((-2139062144&(r=0|t[e>>2])^-2139062144)&r+-16843009);)e=e+4|0;if((255&r)<<24>>24)do{e=e+1|0}while(0!=(0|i[e>>0]))}return e-A|0}function Ht(A,e,r){A|=0,e|=0,r|=0;var n=0,t=0;A:do{if(r){for(;(n=0|i[A>>0])<<24>>24==(t=0|i[e>>0])<<24>>24;){if(!(r=r+-1|0)){A=0;break A}A=A+1|0,e=e+1|0}A=(255&n)-(255&t)|0}else A=0}while(0);return 0|A}function vt(A,e,r,i,n){A|=0,e|=0,r|=0,i|=0,n|=0;var f;return f=G,G=G+144|0,t[f>>2]=0,t[f+4>>2]=A,t[f+44>>2]=A,t[f+8>>2]=(0|A)<0?-1:A+2147483647|0,t[f+76>>2]=-1,Jt(f,0,0),i=0|Xt(f,r,1,i,n),n=0|E(),0|e&&(t[e>>2]=A+((0|t[f+4>>2])+(0|t[f+120>>2])-(0|t[f+8>>2]))),c(0|n),G=f,0|i}function Jt(A,e,r){e|=0,r|=0;var i,n;t[(A|=0)+112>>2]=e,t[A+112+4>>2]=r,i=0|t[A+8>>2],n=0|t[A+4>>2],t[A+120>>2]=i-n,t[A+120+4>>2]=((i-n|0)<0)<<31>>31,t[A+104>>2]=(0!=(0|e)|0!=(0|r))&((((i-n|0)<0)<<31>>31|0)>(0|r)|((i-n|0)>>>0>e>>>0?(((i-n|0)<0)<<31>>31|0)==(0|r):0))?n+e:i}function Xt(A,e,r,n,a){A|=0,e|=0,r|=0,n|=0,a|=0;var B=0,u=0,l=0,Q=0,w=0,b=0,k=0,s=0;A:do{if(e>>>0>36)t[20571]=28,a=0,n=0;else{do{(B=0|t[A+4>>2])>>>0<(0|t[A+104>>2])>>>0?(t[A+4>>2]=B+1,B=0|f[B>>0]):B=0|Tt(A)}while(0!=(0|Yt(B)));e:do{switch(0|B){case 43:case 45:if(u=(45==(0|B))<<31>>31,(B=0|t[A+4>>2])>>>0<(0|t[A+104>>2])>>>0){t[A+4>>2]=B+1,B=0|f[B>>0];break e}B=0|Tt(A);break e;default:u=0}}while(0);do{if(16==(16|e)&48==(0|B)){if((B=0|t[A+4>>2])>>>0<(0|t[A+104>>2])>>>0?(t[A+4>>2]=B+1,B=0|f[B>>0]):B=0|Tt(A),120!=(32|B)){if(e){k=32;break}e=8,k=47;break}if((B=0|t[A+4>>2])>>>0<(0|t[A+104>>2])>>>0?(t[A+4>>2]=B+1,B=0|f[B>>0]):B=0|Tt(A),(0|f[51345+B>>0])>15){if((n=0==(0|t[A+104>>2]))||(t[A+4>>2]=(0|t[A+4>>2])-1),!r){Jt(A,0,0),a=0,n=0;break A}if(n){a=0,n=0;break A}t[A+4>>2]=(0|t[A+4>>2])-1,a=0,n=0;break A}e=16,k=47}else{if(!((e=0==(0|e)?10:e)>>>0>(0|f[51345+B>>0])>>>0)){0|t[A+104>>2]&&(t[A+4>>2]=(0|t[A+4>>2])-1),Jt(A,0,0),t[20571]=28,a=0,n=0;break A}k=32}}while(0);e:do{if(32==(0|k))if(10==(0|e))if((B=B+-48|0)>>>0<10){l=B,e=0;do{e=(10*e|0)+l|0,(B=0|t[A+4>>2])>>>0<(0|t[A+104>>2])>>>0?(t[A+4>>2]=B+1,B=0|f[B>>0]):B=0|Tt(A),l=B+-48|0}while(l>>>0<10&e>>>0<429496729);if(l>>>0<10){r=0;do{if(Q=0|lf(0|e,0|r,10,0),(w=0|E())>>>0>~(b=((0|l)<0)<<31>>31)>>>0|(0|w)==(0|~b)&Q>>>0>~l>>>0){Q=e,e=10,k=76;break e}e=0|cf(0|Q,0|w,0|l,0|b),r=0|E(),(B=0|t[A+4>>2])>>>0<(0|t[A+104>>2])>>>0?(t[A+4>>2]=B+1,B=0|f[B>>0]):B=0|Tt(A),l=B+-48|0}while(l>>>0<10&(r>>>0<429496729|429496729==(0|r)&e>>>0<2576980378));l>>>0>9?B=u:(Q=e,e=10,k=76)}else r=0,B=u}else r=0,e=0,B=u;else k=47}while(0);e:do{if(47==(0|k)){if(!(e+-1&e)){if(s=0|i[80868+((23*e|0)>>>5&7)>>0],e>>>0>(255&(l=0|i[51345+B>>0]))>>>0){l&=255,Q=0;do{Q=l|Q<<s,(B=0|t[A+4>>2])>>>0<(0|t[A+104>>2])>>>0?(t[A+4>>2]=B+1,B=0|f[B>>0]):B=0|Tt(A),l=255&(r=0|i[51345+B>>0])}while(Q>>>0<134217728&e>>>0>l>>>0);k=0}else r=l,k=0,Q=0,l&=255;if(w=0|sf(-1,-1,0|s),e>>>0<=l>>>0|(b=0|E())>>>0<k>>>0|(0|b)==(0|k)&w>>>0<Q>>>0){r=k,k=76;break}for(B=k;;){if(Q=0|Cf(0|Q,0|B,0|s),l=0|E(),Q|=255&r,(B=0|t[A+4>>2])>>>0<(0|t[A+104>>2])>>>0?(t[A+4>>2]=B+1,B=0|f[B>>0]):B=0|Tt(A),e>>>0<=(255&(r=0|i[51345+B>>0]))>>>0|l>>>0>b>>>0|(0|l)==(0|b)&Q>>>0>w>>>0){r=l,k=76;break e}B=l}}if(e>>>0>(255&(l=0|i[51345+B>>0]))>>>0){l&=255,Q=0;do{Q=l+(0|o(Q,e))|0,(B=0|t[A+4>>2])>>>0<(0|t[A+104>>2])>>>0?(t[A+4>>2]=B+1,B=0|f[B>>0]):B=0|Tt(A),l=255&(r=0|i[51345+B>>0])}while(Q>>>0<119304647&e>>>0>l>>>0);w=0}else r=l,Q=0,w=0,l&=255;if(e>>>0>l>>>0)for(k=0|bf(-1,-1,0|e,0),s=0|E(),l=w;;){if(l>>>0>s>>>0|(0|l)==(0|s)&Q>>>0>k>>>0){r=l,k=76;break e}if(w=0|lf(0|Q,0|l,0|e,0),(b=0|E())>>>0>4294967295|-1==(0|b)&w>>>0>~(r&=255)>>>0){r=l,k=76;break e}if(Q=0|cf(0|w,0|b,0|r,0),l=0|E(),(B=0|t[A+4>>2])>>>0<(0|t[A+104>>2])>>>0?(t[A+4>>2]=B+1,B=0|f[B>>0]):B=0|Tt(A),e>>>0<=(255&(r=0|i[51345+B>>0]))>>>0){r=l,k=76;break}}else r=w,k=76}}while(0);if(76==(0|k))if(e>>>0>(0|f[51345+B>>0])>>>0){do{(B=0|t[A+4>>2])>>>0<(0|t[A+104>>2])>>>0?(t[A+4>>2]=B+1,B=0|f[B>>0]):B=0|Tt(A)}while(e>>>0>(0|f[51345+B>>0])>>>0);t[20571]=68,r=a,e=n,B=0==(1&n|0)&!0?u:0}else e=Q,B=u;if(0|t[A+104>>2]&&(t[A+4>>2]=(0|t[A+4>>2])-1),!(r>>>0<a>>>0|(0|r)==(0|a)&e>>>0<n>>>0)){if(!(0!=(1&n|0)|!1|0!=(0|B))){t[20571]=68,n=0|cf(0|n,0|a,-1,-1),a=0|E();break}if(r>>>0>a>>>0|(0|r)==(0|a)&e>>>0>n>>>0){t[20571]=68;break}}n=0|Ef(e^B|0,r^(n=((0|B)<0)<<31>>31)|0,0|B,0|n),a=0|E()}}while(0);return c(0|a),0|n}function Tt(A){var e=0,r=0,n=0,a=0,B=0,o=0,u=0;return(0==(0|(e=0|t[(A|=0)+112>>2]))&0==(0|(r=0|t[A+112+4>>2]))||(0|(a=0|t[A+120+4>>2]))<(0|r)|((0|a)==(0|r)?(0|t[A+120>>2])>>>0<e>>>0:0))&&(0|(e=0|function(A){var e;e=G,G=G+16|0,A=(0==(0|Lt(A=A|0))?1==(0|DB[31&t[A+32>>2]](A,e,1)):0)?0|f[e>>0]:-1;return G=e,0|A}(A)))>=0?(n=0|t[A+112>>2],a=0|t[A+112+4>>2],r=0|t[A+8>>2],0==(0|n)&0==(0|a)||(B=0|t[A+4>>2],o=0|Ef(0|n,0|a,0|t[A+120>>2],0|t[A+120+4>>2]),(0|(a=0|E()))>(((r-B|0)<0)<<31>>31|0)|(o>>>0>(r-B|0)>>>0?(0|a)==(((r-B|0)<0)<<31>>31|0):0))?(n=r,u=9):t[A+104>>2]=B+(o+-1),9==(0|u)&&(t[A+104>>2]=r,r=n),r?(B=r+1-(o=0|t[A+4>>2])|0,B=0|cf(0|t[A+120>>2],0|t[A+120+4>>2],0|B,((0|B)<0)<<31>>31|0),r=0|E(),t[A+120>>2]=B,t[A+120+4>>2]=r,r=o):r=0|t[A+4>>2],(0|e)!=(0|f[(r=r+-1|0)>>0])&&(i[r>>0]=e)):u=4,4==(0|u)&&(t[A+104>>2]=0,e=-1),0|e}function Lt(A){var e=0,r=0;return e=0|i[(A|=0)+74>>0],i[A+74>>0]=e+255|e,(0|t[A+20>>2])>>>0>(0|t[A+28>>2])>>>0&&DB[31&t[A+36>>2]](A,0,0),t[A+16>>2]=0,t[A+28>>2]=0,t[A+20>>2]=0,4&(e=0|t[A>>2])?(t[A>>2]=32|e,e=-1):(r=(0|t[A+44>>2])+(0|t[A+48>>2])|0,t[A+8>>2]=r,t[A+4>>2]=r,e=e<<27>>31),0|e}function St(A,e,r){return A=0|vt(A|=0,e|=0,r|=0,-1,0),E(),0|A}function pt(A,e,r,i){A|=0,i|=0;var n,f,a=0,B=0,u=0,l=0;f=G,G=G+208|0,n=0|o(r|=0,e|=0),t[f+192>>2]=1,t[f+192+4>>2]=0;A:do{if(0|n){for(t[f+4>>2]=r,t[f>>2]=r,e=r,a=r,B=2;e=e+r+a|0,t[f+(B<<2)>>2]=e,e>>>0<n>>>0;)l=a,a=e,B=B+1|0,e=l;if((A+n+(0-r)|0)>>>0>A>>>0){e=1,a=A,B=1;do{do{if(3!=(3&e|0)){if((0|t[f+((e=B+-1|0)<<2)>>2])>>>0<(A+n+(0-r)-a|0)>>>0?zt(a,r,i,B,f):jt(a,r,i,f+192|0,B,0,f),1==(0|B)){xt(f+192|0,1),B=0;break}xt(f+192|0,e),B=1;break}zt(a,r,i,B,f),Kt(f+192|0,2),B=B+2|0}while(0);e=1|t[f+192>>2],t[f+192>>2]=e,a=a+r|0}while(a>>>0<(A+n+(0-r)|0)>>>0)}else e=1,a=A,B=1;for(jt(a,r,i,f+192|0,B,0,f);;){if(1==(0|B)&1==(0|e)){if(!(0|t[f+192+4>>2]))break A;u=19}else(0|B)<2?u=19:(xt(f+192|0,2),l=B+-2|0,t[f+192>>2]=7^t[f+192>>2],Kt(f+192|0,1),jt(a+(0-(0|t[f+(l<<2)>>2]))+(0-r)|0,r,i,f+192|0,B+-1|0,1,f),xt(f+192|0,1),e=1|t[f+192>>2],t[f+192>>2]=e,jt(a=a+(0-r)|0,r,i,f+192|0,l,1,f),B=l);19==(0|u)&&(u=0,Kt(f+192|0,l=0|Ot(f+192|0)),e=0|t[f+192>>2],a=a+(0-r)|0,B=l+B|0)}}}while(0);G=f}function zt(A,e,r,i,n){A|=0,e|=0,r|=0,i|=0,n|=0;var f,a=0,B=0,o=0,u=0;f=G,G=G+240|0,t[f>>2]=A;A:do{if((0|i)>1)for(a=A,B=A,A=1;;){if(o=(B=B+(0-e)|0)+(0-(0|t[n+((u=i+-2|0)<<2)>>2]))|0,(0|gB[63&r](a,o))>-1&&(0|gB[63&r](a,B))>-1)break A;if(a=f+(A<<2)|0,(0|gB[63&r](o,B))>-1?(t[a>>2]=o,B=o,i=i+-1|0):(t[a>>2]=B,i=u),A=A+1|0,(0|i)<=1)break A;a=0|t[f>>2]}else A=1}while(0);_t(e,f,A),G=f}function Kt(A,e){A|=0;var r=0,i=0;(e|=0)>>>0>31?(r=0|t[A+4>>2],t[A>>2]=r,t[A+4>>2]=0,i=0,e=e+-32|0):(r=0|t[A>>2],i=0|t[A+4>>2]),t[A>>2]=i<<32-e|r>>>e,t[A+4>>2]=i>>>e}function jt(A,e,r,i,n,f,a){A|=0,e|=0,r|=0,n|=0,f|=0,a|=0;var B,o=0,u=0,l=0;B=G,G=G+240|0,u=0|t[(i|=0)>>2],t[B+232>>2]=u,l=0|t[i+4>>2],t[B+232+4>>2]=l,t[B>>2]=A;A:do{if(1!=(0|u)|0!=(0|l)&&(o=A+(0-(0|t[a+(n<<2)>>2]))|0,(0|gB[63&r](o,A))>=1))for(u=A,l=1,i=0==(0|f);;){if(i&(0|n)>1){if(A=u+(0-e)|0,i=0|t[a+(n+-2<<2)>>2],(0|gB[63&r](A,o))>-1){o=u,i=l,A=10;break A}if((0|gB[63&r](A+(0-i)|0,o))>-1){o=u,i=l,A=10;break A}}if(i=l+1|0,t[B+(l<<2)>>2]=o,Kt(B+232|0,A=0|Ot(B+232|0)),A=A+n|0,1==(0|t[B+232>>2])&&0==(0|t[B+232+4>>2])){n=A,A=10;break A}if(n=o+(0-(0|t[a+(A<<2)>>2]))|0,(0|gB[63&r](n,0|t[B>>2]))<1){u=i,n=A,i=0,A=9;break}u=o,o=n,l=i,n=A,i=1}else o=A,u=1,i=f,A=9}while(0);9==(0|A)&&0==(0|i)&&(i=u,A=10),10==(0|A)&&(_t(e,B,i),zt(o,e,r,n,a)),G=B}function xt(A,e){A|=0;var r=0,i=0;(e|=0)>>>0>31?(r=0|t[A>>2],t[A+4>>2]=r,t[A>>2]=0,i=0,e=e+-32|0):(r=0|t[A+4>>2],i=0|t[A>>2]),t[A+4>>2]=i>>>(32-e|0)|r<<e,t[A>>2]=i<<e}function Ot(A){var e;return(e=0|Pt((0|t[(A|=0)>>2])-1|0))?0|e:0|(0==(0|(A=0|Pt(0|t[A+4>>2])))?0:A+32|0)}function Pt(A){var e=0;if(A|=0)if(1&A)e=0;else for(e=0;e=e+1|0,!(2&A);)A>>>=1;else e=32;return 0|e}function _t(A,e,r){A|=0,e|=0,r|=0;var i,n=0,f=0,a=0;i=G,G=G+256|0;A:do{if((0|r)>=2&&(t[e+(r<<2)>>2]=i,0|A))for(n=i;;){f=A>>>0<256?A:256,If(0|n,0|t[e>>2],0|f),n=0;do{a=e+(n<<2)|0,n=n+1|0,If(0|t[a>>2],0|t[e+(n<<2)>>2],0|f),t[a>>2]=(0|t[a>>2])+f}while((0|n)!=(0|r));if(!(A=A-f|0))break A;n=0|t[e+(r<<2)>>2]}}while(0);G=i}function qt(A,e){return A=0|$t(A|=0,e|=0),0|((0|i[A>>0])==(255&e)<<24>>24?A:0)}function $t(A,e){A|=0,e|=0;var r=0,n=0;A:do{if(255&e){if(3&A)do{if((n=0|i[A>>0])<<24>>24==0||n<<24>>24==(255&e)<<24>>24)break A;A=A+1|0}while(0!=(3&A|0));n=0|o(255&e,16843009),r=0|t[A>>2];e:do{if(!((-2139062144&r^-2139062144)&r+-16843009))do{if((-2139062144&(r^=n)^-2139062144)&r+-16843009|0)break e;r=0|t[(A=A+4|0)>>2]}while(!((-2139062144&r^-2139062144)&r+-16843009|0))}while(0);for(;(n=0|i[A>>0])<<24>>24!=0&&n<<24>>24!=(255&e)<<24>>24;)A=A+1|0}else A=A+(0|mt(A))|0}while(0);return 0|A}function Af(A,e,r){A|=0,e|=0;var n=0,t=0;if(r|=0){n=0|i[A>>0];A:do{if(n<<24>>24)for(;;){if(r=r+-1|0,!(n<<24>>24==(t=0|i[e>>0])<<24>>24&0!=(0|r)&t<<24>>24!=0))break A;if(e=e+1|0,!((n=0|i[(A=A+1|0)>>0])<<24>>24)){n=0;break}}else n=0}while(0);e=(255&n)-(0|f[e>>0])|0}else e=0;return 0|e}function ef(A,e){return 0|rf(A|=0,e|=0,1+(0|mt(A))|0)}function rf(A,e,r){A|=0,e|=0,r|=0;A:do{if(r){for(;(0|i[A+(r=r+-1|0)>>0])!=(255&e)<<24>>24;)if(!r){r=0;break A}r=A+r|0}else r=0}while(0);return 0|r}function nf(A,e){e|=0;var r=0,n=0,t=0;r=0|i[(A|=0)>>0];A:do{if(r<<24>>24){for(t=r,n=255&r;;){if(!((r=0|i[e>>0])<<24>>24)){r=t;break A}if(t<<24>>24!=r<<24>>24&&(0|(t=0|Vt(n)))!=(0|Vt(0|f[e>>0])))break;if(e=e+1|0,!((r=0|i[(A=A+1|0)>>0])<<24>>24)){r=0;break A}t=r,n=255&r}r=0|i[A>>0]}else r=0}while(0);return(t=0|Vt(255&r))-(0|Vt(0|f[e>>0]))|0}function tf(A){A|=0;for(var e=0,r=0,n=0,t=0;r=A+1|0,0|Yt(0|i[A>>0]);)A=r;switch(0|(e=0|i[A>>0])){case 45:A=1,n=5;break;case 43:A=0,n=5;break;default:t=0,r=A}if(5==(0|n)&&(e=0|i[r>>0],t=A),0|yt(e)){A=0,e=r;do{A=48+(10*A|0)-(0|i[e>>0])|0,e=e+1|0}while(0!=(0|yt(0|i[e>>0])))}else A=0;return 0|(0==(0|t)?0-A|0:A)}function ff(A){A|=0;var e,r=0,i=0,n=0,f=0,a=0,B=0,o=0,u=0,l=0,c=0,E=0,Q=0,w=0,b=0,k=0,s=0,C=0,d=0,I=0;e=G,G=G+16|0;do{if(A>>>0<245){if(Q=A>>>0<11?16:A+11&-8,(c=0|t[20573])>>>(Q>>>3)&3|0)return A=0|t[(i=82332+((c>>>(Q>>>3)&1^1)+(Q>>>3)<<1<<2)|0)+8>>2],(0|(r=0|t[A+8>>2]))==(0|i)?t[20573]=c&~(1<<(c>>>(Q>>>3)&1^1)+(Q>>>3)):(t[r+12>>2]=i,t[i+8>>2]=r),I=(c>>>(Q>>>3)&1^1)+(Q>>>3)<<3,t[A+4>>2]=3|I,t[A+I+4>>2]=1|t[A+I+4>>2],G=e,0|(I=A+8|0);if(Q>>>0>(E=0|t[20575])>>>0){if(c>>>(Q>>>3)|0)return n=0|t[82332+((f=((n=(((A=c>>>(Q>>>3)<<(Q>>>3)&(2<<(Q>>>3)|0-(2<<(Q>>>3))))&0-A)-1|0)>>>(((A&0-A)-1|0)>>>12&16))>>>5&8|((A&0-A)-1|0)>>>12&16|n>>>(n>>>5&8)>>>2&4|(f=n>>>(n>>>5&8)>>>(n>>>(n>>>5&8)>>>2&4))>>>1&2|f>>>(f>>>1&2)>>>1&1)+(f>>>(f>>>1&2)>>>(f>>>(f>>>1&2)>>>1&1))|0)<<1<<2)+8>>2],(0|(A=0|t[n+8>>2]))==(82332+(f<<1<<2)|0)?(t[20573]=c&~(1<<f),A=c&~(1<<f)):(t[A+12>>2]=82332+(f<<1<<2),t[82332+(f<<1<<2)+8>>2]=A,A=c),t[n+4>>2]=3|Q,t[n+Q+4>>2]=(f<<3)-Q|1,t[n+(f<<3)>>2]=(f<<3)-Q,0|E&&(i=0|t[20578],A&1<<(E>>>3)?(A=82332+(E>>>3<<1<<2)+8|0,r=0|t[82332+(E>>>3<<1<<2)+8>>2]):(t[20573]=A|1<<(E>>>3),A=82332+(E>>>3<<1<<2)+8|0,r=82332+(E>>>3<<1<<2)|0),t[A>>2]=i,t[r+12>>2]=i,t[i+8>>2]=r,t[i+12>>2]=82332+(E>>>3<<1<<2)),t[20575]=(f<<3)-Q,t[20578]=n+Q,G=e,0|(I=n+8|0);if(B=0|t[20574]){for(l=0|t[82596+(((u=((B&0-B)-1|0)>>>(((B&0-B)-1|0)>>>12&16))>>>5&8|((B&0-B)-1|0)>>>12&16|u>>>(u>>>5&8)>>>2&4|(l=u>>>(u>>>5&8)>>>(u>>>(u>>>5&8)>>>2&4))>>>1&2|l>>>(l>>>1&2)>>>1&1)+(l>>>(l>>>1&2)>>>(l>>>(l>>>1&2)>>>1&1))<<2)>>2],u=(-8&t[l+4>>2])-Q|0,r=l;(A=0|t[r+16>>2])||(A=0|t[r+20>>2]);)u=(o=(r=(-8&t[A+4>>2])-Q|0)>>>0<u>>>0)?r:u,r=A,l=o?A:l;if((o=l+Q|0)>>>0>l>>>0){f=0|t[l+24>>2],A=0|t[l+12>>2];do{if((0|A)==(0|l)){if(!(A=0|t[(r=l+20|0)>>2])&&!(A=0|t[(r=l+16|0)>>2])){r=0;break}for(;;)if(i=0|t[(n=A+20|0)>>2])A=i,r=n;else{if(!(i=0|t[(n=A+16|0)>>2]))break;A=i,r=n}t[r>>2]=0,r=A}else r=0|t[l+8>>2],t[r+12>>2]=A,t[A+8>>2]=r,r=A}while(0);do{if(0|f){if(A=0|t[l+28>>2],(0|l)==(0|t[82596+(A<<2)>>2])){if(t[82596+(A<<2)>>2]=r,!r){t[20574]=B&~(1<<A);break}}else if(t[((0|t[f+16>>2])==(0|l)?f+16|0:f+20|0)>>2]=r,!r)break;t[r+24>>2]=f,0|(A=0|t[l+16>>2])&&(t[r+16>>2]=A,t[A+24>>2]=r),0|(A=0|t[l+20>>2])&&(t[r+20>>2]=A,t[A+24>>2]=r)}}while(0);return u>>>0<16?(I=u+Q|0,t[l+4>>2]=3|I,t[(I=l+I+4|0)>>2]=1|t[I>>2]):(t[l+4>>2]=3|Q,t[o+4>>2]=1|u,t[o+u>>2]=u,0|E&&(i=0|t[20578],1<<(E>>>3)&c?(A=82332+(E>>>3<<1<<2)+8|0,r=0|t[82332+(E>>>3<<1<<2)+8>>2]):(t[20573]=1<<(E>>>3)|c,A=82332+(E>>>3<<1<<2)+8|0,r=82332+(E>>>3<<1<<2)|0),t[A>>2]=i,t[r+12>>2]=i,t[i+8>>2]=r,t[i+12>>2]=82332+(E>>>3<<1<<2)),t[20575]=u,t[20578]=o),G=e,0|(I=l+8|0)}}}}else if(A>>>0<=4294967231){if(Q=A+11&-8,n=0|t[20574]){u=(A+11|0)>>>8?Q>>>0>16777215?31:Q>>>((u=14-(((u=(A+11|0)>>>8<<((1048320+((A+11|0)>>>8)|0)>>>16&8))+520192|0)>>>16&4|(1048320+((A+11|0)>>>8)|0)>>>16&8|(245760+(u<<((u+520192|0)>>>16&4))|0)>>>16&2)+(u<<((u+520192|0)>>>16&4)<<((245760+(u<<((u+520192|0)>>>16&4))|0)>>>16&2)>>>15)|0)+7|0)&1|u<<1:0,A=0|t[82596+(u<<2)>>2];A:do{if(A)for(i=0-Q|0,r=0,o=Q<<(31==(0|u)?0:25-(u>>>1)|0),B=0;;){if((f=(-8&t[A+4>>2])-Q|0)>>>0<i>>>0){if(!f){i=0,r=A,b=65;break A}i=f,B=A}if(r=0==(0|(b=0|t[A+20>>2]))|(0|b)==(0|(A=0|t[A+16+(o>>>31<<2)>>2]))?r:b,!A){A=B,b=61;break}o<<=1}else i=0-Q|0,r=0,A=0,b=61}while(0);if(61==(0|b)){if(0==(0|r)&0==(0|A)){if(!(((A=2<<u)|0-A)&n))break;r=0|t[82596+(((E=((A|0-A)&n&0-((A|0-A)&n))-1|0)>>>(E>>>12&16)>>>5&8|E>>>12&16|(A=E>>>(E>>>12&16)>>>(E>>>(E>>>12&16)>>>5&8))>>>2&4|A>>>(A>>>2&4)>>>1&2|(r=A>>>(A>>>2&4)>>>(A>>>(A>>>2&4)>>>1&2))>>>1&1)+(r>>>(r>>>1&1))<<2)>>2],A=0}r?b=65:(B=i,o=A)}if(65==(0|b))for(;;){if(i=(f=(E=(-8&t[r+4>>2])-Q|0)>>>0<i>>>0)?E:i,f=f?r:A,(A=0|t[r+16>>2])||(A=0|t[r+20>>2]),!A){B=i,o=f;break}r=A,A=f}if(0!=(0|o)&&B>>>0<((0|t[20575])-Q|0)>>>0&&(l=o+Q|0)>>>0>o>>>0){a=0|t[o+24>>2],A=0|t[o+12>>2];do{if((0|A)==(0|o)){if(!(A=0|t[(r=o+20|0)>>2])&&!(A=0|t[(r=o+16|0)>>2])){A=0;break}for(;;)if(i=0|t[(f=A+20|0)>>2])A=i,r=f;else{if(!(i=0|t[(f=A+16|0)>>2]))break;A=i,r=f}t[r>>2]=0}else I=0|t[o+8>>2],t[I+12>>2]=A,t[A+8>>2]=I}while(0);do{if(a){if(r=0|t[o+28>>2],(0|o)==(0|t[82596+(r<<2)>>2])){if(t[82596+(r<<2)>>2]=A,!A){t[20574]=n&~(1<<r),n&=~(1<<r);break}}else if(t[((0|t[a+16>>2])==(0|o)?a+16|0:a+20|0)>>2]=A,!A)break;t[A+24>>2]=a,0|(r=0|t[o+16>>2])&&(t[A+16>>2]=r,t[r+24>>2]=A),(r=0|t[o+20>>2])&&(t[A+20>>2]=r,t[r+24>>2]=A)}}while(0);A:do{if(B>>>0<16)I=B+Q|0,t[o+4>>2]=3|I,t[(I=o+I+4|0)>>2]=1|t[I>>2];else{if(t[o+4>>2]=3|Q,t[l+4>>2]=1|B,t[l+B>>2]=B,i=B>>>3,B>>>0<256){(A=0|t[20573])&1<<i?(A=82332+(i<<1<<2)+8|0,r=0|t[82332+(i<<1<<2)+8>>2]):(t[20573]=A|1<<i,A=82332+(i<<1<<2)+8|0,r=82332+(i<<1<<2)|0),t[A>>2]=l,t[r+12>>2]=l,t[l+8>>2]=r,t[l+12>>2]=82332+(i<<1<<2);break}if(A=82596+((i=(A=B>>>8)?B>>>0>16777215?31:B>>>((i=14-((520192+(A<<((A+1048320|0)>>>16&8))|0)>>>16&4|(A+1048320|0)>>>16&8|((i=A<<((A+1048320|0)>>>16&8)<<((520192+(A<<((A+1048320|0)>>>16&8))|0)>>>16&4))+245760|0)>>>16&2)+(i<<((i+245760|0)>>>16&2)>>>15)|0)+7|0)&1|i<<1:0)<<2)|0,t[l+28>>2]=i,t[l+16+4>>2]=0,t[l+16>>2]=0,!(n&(r=1<<i))){t[20574]=n|r,t[A>>2]=l,t[l+24>>2]=A,t[l+12>>2]=l,t[l+8>>2]=l;break}A=0|t[A>>2];e:do{if((-8&t[A+4>>2]|0)!=(0|B)){for(n=B<<(31==(0|i)?0:25-(i>>>1)|0);r=0|t[(i=A+16+(n>>>31<<2)|0)>>2];){if((-8&t[r+4>>2]|0)==(0|B)){A=r;break e}n<<=1,A=r}t[i>>2]=l,t[l+24>>2]=A,t[l+12>>2]=l,t[l+8>>2]=l;break A}}while(0);I=0|t[(d=A+8|0)>>2],t[I+12>>2]=l,t[d>>2]=l,t[l+8>>2]=I,t[l+12>>2]=A,t[l+24>>2]=0}}while(0);return G=e,0|(I=o+8|0)}}}else Q=-1}while(0);if((i=0|t[20575])>>>0>=Q>>>0)return r=i-Q|0,A=0|t[20578],r>>>0>15?(I=A+Q|0,t[20578]=I,t[20575]=r,t[I+4>>2]=1|r,t[A+i>>2]=r,t[A+4>>2]=3|Q):(t[20575]=0,t[20578]=0,t[A+4>>2]=3|i,t[A+i+4>>2]=1|t[A+i+4>>2]),G=e,0|(I=A+8|0);if((f=0|t[20576])>>>0>Q>>>0)return C=f-Q|0,t[20576]=C,d=(I=0|t[20579])+Q|0,t[20579]=d,t[d+4>>2]=1|C,t[I+4>>2]=3|Q,G=e,0|(I=I+8|0);if(0|t[20691]?A=0|t[20693]:(t[20693]=4096,t[20692]=4096,t[20694]=-1,t[20695]=-1,t[20696]=0,t[20684]=0,t[20691]=-16&e^1431655768,A=4096),B=Q+48|0,((l=A+(o=Q+47|0)|0)&(u=0-A|0))>>>0<=Q>>>0)return G=e,0|(I=0);if(0|(A=0|t[20683])&&(((E=0|t[20681])+(l&u)|0)>>>0<=E>>>0||(E+(l&u)|0)>>>0>A>>>0))return G=e,0|(I=0);A:do{if(4&t[20684])A=0,b=143;else{r=0|t[20579];e:do{if(r){for(i=82740;!((E=0|t[i>>2])>>>0<=r>>>0&&(E+(0|t[i+4>>2])|0)>>>0>r>>>0);){if(!(A=0|t[i+8>>2])){b=128;break e}i=A}if((l-f&u)>>>0<2147483647)if((0|(A=0|uf(l-f&u)))==((0|t[i>>2])+(0|t[i+4>>2])|0)){if(-1!=(0|A)){B=A,a=l-f&u,b=145;break A}A=l-f&u}else n=A,i=l-f&u,b=136;else A=0}else b=128}while(0);do{if(128==(0|b))if(-1!=(0|(r=0|uf(0)))&&(a=(0==((a=0|t[20692])+-1&r|0)?0:(a+-1+r&0-a)-r|0)+(l&u)|0,w=0|t[20681],a>>>0>Q>>>0&a>>>0<2147483647)){if(0|(E=0|t[20683])&&(a+w|0)>>>0<=w>>>0|(a+w|0)>>>0>E>>>0){A=0;break}if((0|(A=0|uf(a)))==(0|r)){B=r,b=145;break A}n=A,i=a,b=136}else A=0}while(0);do{if(136==(0|b)){if(r=0-i|0,!(B>>>0>i>>>0&i>>>0<2147483647&-1!=(0|n))){if(-1==(0|n)){A=0;break}B=n,a=i,b=145;break A}if((A=o-i+(A=0|t[20693])&0-A)>>>0>=2147483647){B=n,a=i,b=145;break A}if(-1==(0|uf(A))){uf(r),A=0;break}B=n,a=A+i|0,b=145;break A}}while(0);t[20684]=4|t[20684],b=143}}while(0);if(143==(0|b)&&(l&u)>>>0<2147483647&&!(-1==(0|(k=0|uf(l&u)))|1^(C=((s=0|uf(0))-k|0)>>>0>(Q+40|0)>>>0)|k>>>0<s>>>0&-1!=(0|k)&-1!=(0|s)^1)&&(B=k,a=C?s-k|0:A,b=145),145==(0|b)){A=(0|t[20681])+a|0,t[20681]=A,A>>>0>(0|t[20682])>>>0&&(t[20682]=A),u=0|t[20579];A:do{if(u){for(n=82740;;){if((0|B)==((A=0|t[n>>2])+(r=0|t[n+4>>2])|0)){b=154;break}if(!(i=0|t[n+8>>2]))break;n=i}if(154==(0|b)&&(d=n+4|0,0==(8&t[n+12>>2]|0))&&B>>>0>u>>>0&A>>>0<=u>>>0){t[d>>2]=r+a,I=(0|t[20576])+a|0,d=0==(u+8&7|0)?0:0-(u+8)&7,t[20579]=u+d,t[20576]=I-d,t[u+d+4>>2]=I-d|1,t[u+I+4>>2]=40,t[20580]=t[20695];break}for(B>>>0<(0|t[20577])>>>0&&(t[20577]=B),i=B+a|0,r=82740;;){if((0|t[r>>2])==(0|i)){b=162;break}if(!(A=0|t[r+8>>2]))break;r=A}if(162==(0|b)&&0==(8&t[r+12>>2]|0)){t[r>>2]=B,t[(c=r+4|0)>>2]=(0|t[c>>2])+a,l=(c=B+(0==(7&(c=B+8|0)|0)?0:0-c&7)|0)+Q|0,o=(A=i+(0==(i+8&7|0)?0:0-(i+8)&7)|0)-c-Q|0,t[c+4>>2]=3|Q;e:do{if((0|u)==(0|A))I=(0|t[20576])+o|0,t[20576]=I,t[20579]=l,t[l+4>>2]=1|I;else{if((0|t[20578])==(0|A)){I=(0|t[20575])+o|0,t[20575]=I,t[20578]=l,t[l+4>>2]=1|I,t[l+I>>2]=I;break}if(1==(3&(B=0|t[A+4>>2])|0)){r:do{if(B>>>0<256){if(r=0|t[A+8>>2],(0|(i=0|t[A+12>>2]))==(0|r)){t[20573]=t[20573]&~(1<<(B>>>3));break}t[r+12>>2]=i,t[i+8>>2]=r;break}a=0|t[A+24>>2],r=0|t[A+12>>2];do{if((0|r)==(0|A)){if(r=0|t[A+16+4>>2])i=A+16+4|0;else{if(!(r=0|t[A+16>>2])){r=0;break}i=A+16|0}for(;;)if(n=0|t[(f=r+20|0)>>2])r=n,i=f;else{if(!(n=0|t[(f=r+16|0)>>2]))break;r=n,i=f}t[i>>2]=0}else I=0|t[A+8>>2],t[I+12>>2]=r,t[r+8>>2]=I}while(0);if(!a)break;i=0|t[A+28>>2];do{if((0|t[82596+(i<<2)>>2])==(0|A)){if(t[82596+(i<<2)>>2]=r,0|r)break;t[20574]=t[20574]&~(1<<i);break r}if(t[((0|t[a+16>>2])==(0|A)?a+16|0:a+20|0)>>2]=r,!r)break r}while(0);if(t[r+24>>2]=a,0|(i=0|t[A+16>>2])&&(t[r+16>>2]=i,t[i+24>>2]=r),!(i=0|t[A+16+4>>2]))break;t[r+20>>2]=i,t[i+24>>2]=r}while(0);A=A+(-8&B)|0,f=(-8&B)+o|0}else f=o;if(t[(i=A+4|0)>>2]=-2&t[i>>2],t[l+4>>2]=1|f,t[l+f>>2]=f,i=f>>>3,f>>>0<256){(A=0|t[20573])&1<<i?(A=82332+(i<<1<<2)+8|0,r=0|t[82332+(i<<1<<2)+8>>2]):(t[20573]=A|1<<i,A=82332+(i<<1<<2)+8|0,r=82332+(i<<1<<2)|0),t[A>>2]=l,t[r+12>>2]=l,t[l+8>>2]=r,t[l+12>>2]=82332+(i<<1<<2);break}A=f>>>8;do{if(A){if(f>>>0>16777215){n=31;break}n=f>>>((n=14-((520192+(A<<((A+1048320|0)>>>16&8))|0)>>>16&4|(A+1048320|0)>>>16&8|((n=A<<((A+1048320|0)>>>16&8)<<((520192+(A<<((A+1048320|0)>>>16&8))|0)>>>16&4))+245760|0)>>>16&2)+(n<<((n+245760|0)>>>16&2)>>>15)|0)+7|0)&1|n<<1}else n=0}while(0);if(r=82596+(n<<2)|0,t[l+28>>2]=n,t[l+16+4>>2]=0,t[l+16>>2]=0,!((A=0|t[20574])&(i=1<<n))){t[20574]=A|i,t[r>>2]=l,t[l+24>>2]=r,t[l+12>>2]=l,t[l+8>>2]=l;break}A=0|t[r>>2];r:do{if((-8&t[A+4>>2]|0)!=(0|f)){for(n=f<<(31==(0|n)?0:25-(n>>>1)|0);r=0|t[(i=A+16+(n>>>31<<2)|0)>>2];){if((-8&t[r+4>>2]|0)==(0|f)){A=r;break r}n<<=1,A=r}t[i>>2]=l,t[l+24>>2]=A,t[l+12>>2]=l,t[l+8>>2]=l;break e}}while(0);I=0|t[(d=A+8|0)>>2],t[I+12>>2]=l,t[d>>2]=l,t[l+8>>2]=I,t[l+12>>2]=A,t[l+24>>2]=0}}while(0);return G=e,0|(I=c+8|0)}for(r=82740;!((A=0|t[r>>2])>>>0<=u>>>0&&(I=A+(0|t[r+4>>2])|0)>>>0>u>>>0);)r=0|t[r+8>>2];f=(f=I+-47+(0==(I+-47+8&7|0)?0:0-(I+-47+8)&7)|0)>>>0<(u+16|0)>>>0?u:f,A=a+-40|0,d=B+(C=0==(7&(C=B+8|0)|0)?0:0-C&7)|0,t[20579]=d,t[20576]=A-C,t[d+4>>2]=A-C|1,t[B+A+4>>2]=40,t[20580]=t[20695],t[f+4>>2]=27,t[f+8>>2]=t[20685],t[f+8+4>>2]=t[20686],t[f+8+8>>2]=t[20687],t[f+8+12>>2]=t[20688],t[20685]=B,t[20686]=a,t[20688]=0,t[20687]=f+8,A=f+24|0;do{d=A,t[(A=A+4|0)>>2]=7}while((d+8|0)>>>0<I>>>0);if((0|f)!=(0|u)){if(t[f+4>>2]=-2&t[f+4>>2],t[u+4>>2]=f-u|1,t[f>>2]=f-u,(f-u|0)>>>0<256){i=82332+((f-u|0)>>>3<<1<<2)|0,(A=0|t[20573])&1<<((f-u|0)>>>3)?(A=i+8|0,r=0|t[i+8>>2]):(t[20573]=A|1<<((f-u|0)>>>3),A=i+8|0,r=i),t[A>>2]=u,t[r+12>>2]=u,t[u+8>>2]=r,t[u+12>>2]=i;break}if(r=82596+((n=(f-u|0)>>>8?(f-u|0)>>>0>16777215?31:(f-u|0)>>>((n=14-(((n=(f-u|0)>>>8<<((1048320+((f-u|0)>>>8)|0)>>>16&8))+520192|0)>>>16&4|(1048320+((f-u|0)>>>8)|0)>>>16&8|(245760+(n<<((n+520192|0)>>>16&4))|0)>>>16&2)+(n<<((n+520192|0)>>>16&4)<<((245760+(n<<((n+520192|0)>>>16&4))|0)>>>16&2)>>>15)|0)+7|0)&1|n<<1:0)<<2)|0,t[u+28>>2]=n,t[u+20>>2]=0,t[u+16>>2]=0,!((A=0|t[20574])&(i=1<<n))){t[20574]=A|i,t[r>>2]=u,t[u+24>>2]=r,t[u+12>>2]=u,t[u+8>>2]=u;break}A=0|t[r>>2];e:do{if((-8&t[A+4>>2]|0)!=(f-u|0)){for(n=f-u<<(31==(0|n)?0:25-(n>>>1)|0);r=0|t[(i=A+16+(n>>>31<<2)|0)>>2];){if((-8&t[r+4>>2]|0)==(f-u|0)){A=r;break e}n<<=1,A=r}t[i>>2]=u,t[u+24>>2]=A,t[u+12>>2]=u,t[u+8>>2]=u;break A}}while(0);I=0|t[(d=A+8|0)>>2],t[I+12>>2]=u,t[d>>2]=u,t[u+8>>2]=I,t[u+12>>2]=A,t[u+24>>2]=0}}else 0==(0|(I=0|t[20577]))|B>>>0<I>>>0&&(t[20577]=B),t[20685]=B,t[20686]=a,t[20688]=0,t[20582]=t[20691],t[20581]=-1,t[20586]=82332,t[20585]=82332,t[20588]=82340,t[20587]=82340,t[20590]=82348,t[20589]=82348,t[20592]=82356,t[20591]=82356,t[20594]=82364,t[20593]=82364,t[20596]=82372,t[20595]=82372,t[20598]=82380,t[20597]=82380,t[20600]=82388,t[20599]=82388,t[20602]=82396,t[20601]=82396,t[20604]=82404,t[20603]=82404,t[20606]=82412,t[20605]=82412,t[20608]=82420,t[20607]=82420,t[20610]=82428,t[20609]=82428,t[20612]=82436,t[20611]=82436,t[20614]=82444,t[20613]=82444,t[20616]=82452,t[20615]=82452,t[20618]=82460,t[20617]=82460,t[20620]=82468,t[20619]=82468,t[20622]=82476,t[20621]=82476,t[20624]=82484,t[20623]=82484,t[20626]=82492,t[20625]=82492,t[20628]=82500,t[20627]=82500,t[20630]=82508,t[20629]=82508,t[20632]=82516,t[20631]=82516,t[20634]=82524,t[20633]=82524,t[20636]=82532,t[20635]=82532,t[20638]=82540,t[20637]=82540,t[20640]=82548,t[20639]=82548,t[20642]=82556,t[20641]=82556,t[20644]=82564,t[20643]=82564,t[20646]=82572,t[20645]=82572,t[20648]=82580,t[20647]=82580,I=a+-40|0,d=B+(C=0==(7&(C=B+8|0)|0)?0:0-C&7)|0,t[20579]=d,t[20576]=I-C,t[d+4>>2]=I-C|1,t[B+I+4>>2]=40,t[20580]=t[20695]}while(0);if((A=0|t[20576])>>>0>Q>>>0)return C=A-Q|0,t[20576]=C,d=(I=0|t[20579])+Q|0,t[20579]=d,t[d+4>>2]=1|C,t[I+4>>2]=3|Q,G=e,0|(I=I+8|0)}return t[20571]=48,G=e,0|(I=0)}function af(A){var e=0,r=0,i=0,n=0,f=0,a=0,B=0,o=0,u=0;if(A|=0){e=0|t[20577],u=A+-8+(-8&(r=0|t[A+-4>>2]))|0;do{if(1&r)B=A+-8|0,o=A+-8|0,A=-8&r;else{if(i=0|t[A+-8>>2],!(3&r))return;if(a=i+(-8&r)|0,(f=A+-8+(0-i)|0)>>>0<e>>>0)return;if((0|t[20578])==(0|f)){if(3!=(3&(A=0|t[u+4>>2])|0)){B=f,o=f,A=a;break}return t[20575]=a,t[u+4>>2]=-2&A,t[f+4>>2]=1|a,void(t[f+a>>2]=a)}if(i>>>0<256){if(A=0|t[f+8>>2],(0|(e=0|t[f+12>>2]))==(0|A)){t[20573]=t[20573]&~(1<<(i>>>3)),B=f,o=f,A=a;break}t[A+12>>2]=e,t[e+8>>2]=A,B=f,o=f,A=a;break}n=0|t[f+24>>2],A=0|t[f+12>>2];do{if((0|A)==(0|f)){if(A=0|t[f+16+4>>2])e=f+16+4|0;else{if(!(A=0|t[f+16>>2])){A=0;break}e=f+16|0}for(;;)if(r=0|t[(i=A+20|0)>>2])A=r,e=i;else{if(!(r=0|t[(i=A+16|0)>>2]))break;A=r,e=i}t[e>>2]=0}else o=0|t[f+8>>2],t[o+12>>2]=A,t[A+8>>2]=o}while(0);if(n){if(e=0|t[f+28>>2],(0|t[82596+(e<<2)>>2])==(0|f)){if(t[82596+(e<<2)>>2]=A,!A){t[20574]=t[20574]&~(1<<e),B=f,o=f,A=a;break}}else if(t[((0|t[n+16>>2])==(0|f)?n+16|0:n+20|0)>>2]=A,!A){B=f,o=f,A=a;break}t[A+24>>2]=n,0|(e=0|t[f+16>>2])&&(t[A+16>>2]=e,t[e+24>>2]=A),(e=0|t[f+16+4>>2])?(t[A+20>>2]=e,t[e+24>>2]=A,B=f,o=f,A=a):(B=f,o=f,A=a)}else B=f,o=f,A=a}}while(0);if(!(B>>>0>=u>>>0)&&1&(r=0|t[u+4>>2])){if(2&r)t[u+4>>2]=-2&r,t[o+4>>2]=1|A,t[B+A>>2]=A,n=A;else{if((0|t[20579])==(0|u)){if(u=(0|t[20576])+A|0,t[20576]=u,t[20579]=o,t[o+4>>2]=1|u,(0|o)!=(0|t[20578]))return;return t[20578]=0,void(t[20575]=0)}if((0|t[20578])==(0|u))return u=(0|t[20575])+A|0,t[20575]=u,t[20578]=B,t[o+4>>2]=1|u,void(t[B+u>>2]=u);n=(-8&r)+A|0;do{if(r>>>0<256){if(e=0|t[u+8>>2],(0|(A=0|t[u+12>>2]))==(0|e)){t[20573]=t[20573]&~(1<<(r>>>3));break}t[e+12>>2]=A,t[A+8>>2]=e;break}f=0|t[u+24>>2],A=0|t[u+12>>2];do{if((0|A)==(0|u)){if(A=0|t[u+16+4>>2])e=u+16+4|0;else{if(!(A=0|t[u+16>>2])){e=0;break}e=u+16|0}for(;;)if(r=0|t[(i=A+20|0)>>2])A=r,e=i;else{if(!(r=0|t[(i=A+16|0)>>2]))break;A=r,e=i}t[e>>2]=0,e=A}else e=0|t[u+8>>2],t[e+12>>2]=A,t[A+8>>2]=e,e=A}while(0);if(0|f){if(A=0|t[u+28>>2],(0|t[82596+(A<<2)>>2])==(0|u)){if(t[82596+(A<<2)>>2]=e,!e){t[20574]=t[20574]&~(1<<A);break}}else if(t[((0|t[f+16>>2])==(0|u)?f+16|0:f+20|0)>>2]=e,!e)break;t[e+24>>2]=f,0|(A=0|t[u+16>>2])&&(t[e+16>>2]=A,t[A+24>>2]=e),0|(A=0|t[u+16+4>>2])&&(t[e+20>>2]=A,t[A+24>>2]=e)}}while(0);if(t[o+4>>2]=1|n,t[B+n>>2]=n,(0|o)==(0|t[20578]))return void(t[20575]=n)}if(r=n>>>3,n>>>0<256)return(A=0|t[20573])&1<<r?(A=82332+(r<<1<<2)+8|0,e=0|t[82332+(r<<1<<2)+8>>2]):(t[20573]=A|1<<r,A=82332+(r<<1<<2)+8|0,e=82332+(r<<1<<2)|0),t[A>>2]=o,t[e+12>>2]=o,t[o+8>>2]=e,void(t[o+12>>2]=82332+(r<<1<<2));e=82596+((i=(A=n>>>8)?n>>>0>16777215?31:n>>>((i=14-((520192+(A<<((A+1048320|0)>>>16&8))|0)>>>16&4|(A+1048320|0)>>>16&8|((i=A<<((A+1048320|0)>>>16&8)<<((520192+(A<<((A+1048320|0)>>>16&8))|0)>>>16&4))+245760|0)>>>16&2)+(i<<((i+245760|0)>>>16&2)>>>15)|0)+7|0)&1|i<<1:0)<<2)|0,t[o+28>>2]=i,t[o+20>>2]=0,t[o+16>>2]=0,A=0|t[20574],r=1<<i;A:do{if(A&r){A=0|t[e>>2];e:do{if((-8&t[A+4>>2]|0)!=(0|n)){for(i=n<<(31==(0|i)?0:25-(i>>>1)|0);e=0|t[(r=A+16+(i>>>31<<2)|0)>>2];){if((-8&t[e+4>>2]|0)==(0|n)){A=e;break e}i<<=1,A=e}t[r>>2]=o,t[o+24>>2]=A,t[o+12>>2]=o,t[o+8>>2]=o;break A}}while(0);u=0|t[(B=A+8|0)>>2],t[u+12>>2]=o,t[B>>2]=o,t[o+8>>2]=u,t[o+12>>2]=A,t[o+24>>2]=0}else t[20574]=A|r,t[e>>2]=o,t[o+24>>2]=e,t[o+12>>2]=o,t[o+8>>2]=o}while(0);if(u=(0|t[20581])-1|0,t[20581]=u,!(0|u)){for(A=82748;A=0|t[A>>2];)A=A+8|0;t[20581]=-1}}}}function Bf(A,e){e|=0;var r=0,i=0;return(A|=0)?e>>>0>4294967231?(t[20571]=48,0|(A=0)):0|(r=0|function(A,e){e|=0;var r,i,n,f,a=0,B=0,o=0,u=0,l=0;if(f=0|t[(A=A|0)+4>>2],r=A+(-8&f)|0,!(3&f))return e>>>0<256?0|(A=0):(-8&f)>>>0>=(e+4|0)>>>0&&((-8&f)-e|0)>>>0<=t[20693]<<1>>>0?0|A:0|(A=0);if((-8&f)>>>0>=e>>>0)return((-8&f)-e|0)>>>0<=15||(t[A+4>>2]=1&f|e|2,t[A+e+4>>2]=(-8&f)-e|3,t[r+4>>2]=1|t[r+4>>2],of(A+e|0,(-8&f)-e|0)),0|A;if((0|t[20579])==(0|r))return(a=(0|t[20576])+(-8&f)|0)>>>0<=e>>>0?0|(A=0):(t[A+4>>2]=1&f|e|2,t[A+e+4>>2]=a-e|1,t[20579]=A+e,t[20576]=a-e,0|A);if((0|t[20578])==(0|r))return(a=(0|t[20575])+(-8&f)|0)>>>0<e>>>0?0|(A=0):((a-e|0)>>>0>15?(t[A+4>>2]=1&f|e|2,t[A+e+4>>2]=a-e|1,t[A+a>>2]=a-e,t[A+a+4>>2]=-2&t[A+a+4>>2],B=A+e|0,a=a-e|0):(t[A+4>>2]=1&f|a|2,t[A+a+4>>2]=1|t[A+a+4>>2],B=0,a=0),t[20575]=a,t[20578]=B,0|A);if(2&(i=0|t[r+4>>2])|0)return 0|(A=0);if(((-8&i)+(-8&f)|0)>>>0<e>>>0)return 0|(A=0);n=(-8&i)+(-8&f)-e|0;do{if(i>>>0<256){if(B=0|t[r+8>>2],(0|(a=0|t[r+12>>2]))==(0|B)){t[20573]=t[20573]&~(1<<(i>>>3));break}t[B+12>>2]=a,t[a+8>>2]=B;break}l=0|t[r+24>>2],a=0|t[r+12>>2];do{if((0|a)==(0|r)){if(a=0|t[r+16+4>>2])B=r+16+4|0;else{if(!(a=0|t[r+16>>2])){B=0;break}B=r+16|0}for(;;)if(o=0|t[(u=a+20|0)>>2])a=o,B=u;else{if(!(o=0|t[(u=a+16|0)>>2]))break;a=o,B=u}t[B>>2]=0,B=a}else B=0|t[r+8>>2],t[B+12>>2]=a,t[a+8>>2]=B,B=a}while(0);if(0|l){if(a=0|t[r+28>>2],(0|t[82596+(a<<2)>>2])==(0|r)){if(t[82596+(a<<2)>>2]=B,!B){t[20574]=t[20574]&~(1<<a);break}}else if(t[((0|t[l+16>>2])==(0|r)?l+16|0:l+20|0)>>2]=B,!B)break;t[B+24>>2]=l,0|(a=0|t[r+16>>2])&&(t[B+16>>2]=a,t[a+24>>2]=B),0|(a=0|t[r+16+4>>2])&&(t[B+20>>2]=a,t[a+24>>2]=B)}}while(0);return n>>>0<16?(t[A+4>>2]=1&f|(-8&i)+(-8&f)|2,t[A+((-8&i)+(-8&f))+4>>2]=1|t[A+((-8&i)+(-8&f))+4>>2],0|A):(t[A+4>>2]=1&f|e|2,t[A+e+4>>2]=3|n,t[A+((-8&i)+(-8&f))+4>>2]=1|t[A+((-8&i)+(-8&f))+4>>2],of(A+e|0,n),0|A)}(A+-8|0,e>>>0<11?16:e+11&-8))?0|(A=r+8|0):(r=0|ff(e))?(If(0|r,0|A,0|((i=(-8&(i=0|t[A+-4>>2]))-(0==(3&i|0)?8:4)|0)>>>0<e>>>0?i:e)),af(A),0|(A=r)):0|(A=0):0|(A=0|ff(e))}function of(A,e){e|=0;var r=0,i=0,n=0,f=0,a=0,B=0,o=0;r=0|t[(A|=0)+4>>2];do{if(1&r)o=A,r=e;else{if(B=0|t[A>>2],!(3&r))return;if((0|t[20578])==(A+(0-B)|0)){if(3!=(3&(r=0|t[A+e+4>>2])|0)){o=A+(0-B)|0,r=B+e|0;break}return t[20575]=B+e,t[A+e+4>>2]=-2&r,t[A+(0-B)+4>>2]=B+e|1,void(t[A+e>>2]=B+e)}if(B>>>0<256){if(r=0|t[A+(0-B)+8>>2],(0|(i=0|t[A+(0-B)+12>>2]))==(0|r)){t[20573]=t[20573]&~(1<<(B>>>3)),o=A+(0-B)|0,r=B+e|0;break}t[r+12>>2]=i,t[i+8>>2]=r,o=A+(0-B)|0,r=B+e|0;break}a=0|t[A+(0-B)+24>>2],r=0|t[A+(0-B)+12>>2];do{if((0|r)==(A+(0-B)|0)){if(r=0|t[(i=A+(0-B)+16|0)+4>>2])i=i+4|0;else if(!(r=0|t[i>>2])){i=0;break}for(;;)if(n=0|t[(f=r+20|0)>>2])r=n,i=f;else{if(!(n=0|t[(f=r+16|0)>>2]))break;r=n,i=f}t[i>>2]=0,i=r}else i=0|t[A+(0-B)+8>>2],t[i+12>>2]=r,t[r+8>>2]=i,i=r}while(0);if(a){if(r=0|t[A+(0-B)+28>>2],(0|t[82596+(r<<2)>>2])==(A+(0-B)|0)){if(t[82596+(r<<2)>>2]=i,!i){t[20574]=t[20574]&~(1<<r),o=A+(0-B)|0,r=B+e|0;break}}else if(t[((0|t[a+16>>2])==(A+(0-B)|0)?a+16|0:a+20|0)>>2]=i,!i){o=A+(0-B)|0,r=B+e|0;break}t[i+24>>2]=a,0|(r=0|t[A+(0-B)+16>>2])&&(t[i+16>>2]=r,t[r+24>>2]=i),(r=0|t[A+(0-B)+16+4>>2])?(t[i+20>>2]=r,t[r+24>>2]=i,o=A+(0-B)|0,r=B+e|0):(o=A+(0-B)|0,r=B+e|0)}else o=A+(0-B)|0,r=B+e|0}}while(0);if(2&(n=0|t[A+e+4>>2]))t[A+e+4>>2]=-2&n,t[o+4>>2]=1|r,t[o+r>>2]=r,a=r;else{if((0|t[20579])==(A+e|0)){if(e=(0|t[20576])+r|0,t[20576]=e,t[20579]=o,t[o+4>>2]=1|e,(0|o)!=(0|t[20578]))return;return t[20578]=0,void(t[20575]=0)}if((0|t[20578])==(A+e|0))return e=(0|t[20575])+r|0,t[20575]=e,t[20578]=o,t[o+4>>2]=1|e,void(t[o+e>>2]=e);a=(-8&n)+r|0;do{if(n>>>0<256){if(i=0|t[A+e+8>>2],(0|(r=0|t[A+e+12>>2]))==(0|i)){t[20573]=t[20573]&~(1<<(n>>>3));break}t[i+12>>2]=r,t[r+8>>2]=i;break}B=0|t[A+e+24>>2],r=0|t[A+e+12>>2];do{if((0|r)==(A+e|0)){if(r=0|t[A+e+16+4>>2])i=A+e+16+4|0;else{if(!(r=0|t[A+e+16>>2])){i=0;break}i=A+e+16|0}for(;;)if(n=0|t[(f=r+20|0)>>2])r=n,i=f;else{if(!(n=0|t[(f=r+16|0)>>2]))break;r=n,i=f}t[i>>2]=0,i=r}else i=0|t[A+e+8>>2],t[i+12>>2]=r,t[r+8>>2]=i,i=r}while(0);if(0|B){if(r=0|t[A+e+28>>2],(0|t[82596+(r<<2)>>2])==(A+e|0)){if(t[82596+(r<<2)>>2]=i,!i){t[20574]=t[20574]&~(1<<r);break}}else if(t[((0|t[B+16>>2])==(A+e|0)?B+16|0:B+20|0)>>2]=i,!i)break;t[i+24>>2]=B,0|(r=0|t[A+e+16>>2])&&(t[i+16>>2]=r,t[r+24>>2]=i),0|(r=0|t[A+e+16+4>>2])&&(t[i+20>>2]=r,t[r+24>>2]=i)}}while(0);if(t[o+4>>2]=1|a,t[o+a>>2]=a,(0|o)==(0|t[20578]))return void(t[20575]=a)}if(n=a>>>3,a>>>0<256)return(r=0|t[20573])&1<<n?(r=82332+(n<<1<<2)+8|0,i=0|t[82332+(n<<1<<2)+8>>2]):(t[20573]=r|1<<n,r=82332+(n<<1<<2)+8|0,i=82332+(n<<1<<2)|0),t[r>>2]=o,t[i+12>>2]=o,t[o+8>>2]=i,void(t[o+12>>2]=82332+(n<<1<<2));if(i=82596+((f=(r=a>>>8)?a>>>0>16777215?31:a>>>((f=14-((520192+(r<<((r+1048320|0)>>>16&8))|0)>>>16&4|(r+1048320|0)>>>16&8|((f=r<<((r+1048320|0)>>>16&8)<<((520192+(r<<((r+1048320|0)>>>16&8))|0)>>>16&4))+245760|0)>>>16&2)+(f<<((f+245760|0)>>>16&2)>>>15)|0)+7|0)&1|f<<1:0)<<2)|0,t[o+28>>2]=f,t[o+20>>2]=0,t[o+16>>2]=0,!((r=0|t[20574])&(n=1<<f)))return t[20574]=r|n,t[i>>2]=o,t[o+24>>2]=i,t[o+12>>2]=o,void(t[o+8>>2]=o);r=0|t[i>>2];A:do{if((-8&t[r+4>>2]|0)!=(0|a)){for(f=a<<(31==(0|f)?0:25-(f>>>1)|0);i=0|t[(n=r+16+(f>>>31<<2)|0)>>2];){if((-8&t[i+4>>2]|0)==(0|a)){r=i;break A}f<<=1,r=i}return t[n>>2]=o,t[o+24>>2]=r,t[o+12>>2]=o,void(t[o+8>>2]=o)}}while(0);e=0|t[(A=r+8|0)>>2],t[e+12>>2]=o,t[A>>2]=o,t[o+8>>2]=e,t[o+12>>2]=r,t[o+24>>2]=0}function uf(A){A|=0;var e,r;r=(e=0|t[20704])+(A+3&-4)|0;do{if((A+3&-4|0)<1|r>>>0>e>>>0){if(r>>>0>(0|g())>>>0&&0==(0|M(0|r)))break;return t[20704]=r,0|(A=e)}}while(0);return t[20571]=48,0|(A=-1)}function lf(A,e,r,i){e|=0,i|=0;var n,t;return n=0|function(A,e){var r,i,n;return n=((r=0|o(65535&(e|=0),65535&(A|=0)))>>>16)+(0|o(65535&e,A>>>16))|0,i=0|o(e>>>16,65535&A),0|(c((n>>>16)+(0|o(e>>>16,A>>>16))+(((65535&n)+i|0)>>>16)|0),n+i<<16|65535&r|0)}(A|=0,r|=0),t=0|E(),0|(c((0|o(e,r))+(0|o(i,A))+t|0&t|0),0|n)}function cf(A,e,r,i){return 0|(c((e|=0)+(i|=0)+((A|=0)+(r|=0)>>>0>>>0<A>>>0|0)>>>0|0),A+r>>>0|0)}function Ef(A,e,r,i){return 0|(c(0|(i=(e|=0)-(i|=0)-((r|=0)>>>0>(A|=0)>>>0|0)>>>0)),A-r>>>0|0)}function Qf(A){return 0|((A|=0)?31-(0|u(A^A-1))|0:32)}function wf(A,e,r,i,n){A|=0,r|=0,i|=0,n|=0;var f=0,a=0,B=0,o=0,l=0,Q=0,w=0,b=0,k=0,s=0;if(!(e|=0))return i?n?(t[n>>2]=0|A,t[n+4>>2]=0&e,n=0,0|(c(0|(i=0)),n)):(n=0,0|(c(0|(i=0)),n)):(0|n&&(t[n>>2]=(A>>>0)%(r>>>0),t[n+4>>2]=0),n=(A>>>0)/(r>>>0)>>>0,0|(c(0|(i=0)),n));do{if(r){if(0|i){if((a=(0|u(0|i))-(0|u(0|e))|0)>>>0<=31){b=a+1|0,B=A>>>((a+1|0)>>>0)&a-31>>31|e<<31-a,w=e>>>((a+1|0)>>>0)&a-31>>31,f=0,a=A<<31-a;break}return n?(t[n>>2]=0|A,t[n+4>>2]=e|0&e,n=0,0|(c(0|(i=0)),n)):(n=0,0|(c(0|(i=0)),n))}if(r-1&r|0){b=a=33+(0|u(0|r))-(0|u(0|e))|0,B=32-a-1>>31&e>>>((a-32|0)>>>0)|(e<<32-a|A>>>(a>>>0))&a-32>>31,w=a-32>>31&e>>>(a>>>0),f=A<<64-a&32-a>>31,a=(e<<64-a|A>>>((a-32|0)>>>0))&32-a>>31|A<<32-a&a-33>>31;break}return 0|n&&(t[n>>2]=r-1&A,t[n+4>>2]=0),1==(0|r)?(n=0|A,0|(c(0|(i=e|0&e)),n)):(i=e>>>((n=0|Qf(0|r))>>>0)|0,n=e<<32-n|A>>>(n>>>0)|0,0|(c(0|i),n))}if(!i)return 0|n&&(t[n>>2]=(e>>>0)%(r>>>0),t[n+4>>2]=0),n=(e>>>0)/(r>>>0)>>>0,0|(c(0|(i=0)),n);if(!A)return 0|n&&(t[n>>2]=0,t[n+4>>2]=(e>>>0)%(i>>>0)),n=(e>>>0)/(i>>>0)>>>0,0|(c(0|(r=0)),n);if(!(i-1&i))return 0|n&&(t[n>>2]=0|A,t[n+4>>2]=i-1&e|0&e),r=0,n=e>>>((0|Qf(0|i))>>>0),0|(c(0|r),n);if((a=(0|u(0|i))-(0|u(0|e))|0)>>>0<=30){b=a+1|0,B=e<<31-a|A>>>((a+1|0)>>>0),w=e>>>((a+1|0)>>>0),f=0,a=A<<31-a;break}return n?(t[n>>2]=0|A,t[n+4>>2]=e|0&e,n=0,0|(c(0|(i=0)),n)):(n=0,0|(c(0|(i=0)),n))}while(0);if(b){l=0|cf(0|r,i|0&i|0,-1,-1),Q=0|E(),o=a,e=w,A=b,a=0;do{s=o,o=f>>>31|o<<1,f=a|f<<1,Ef(0|l,0|Q,0|(s=B<<1|s>>>31|0),0|(k=B>>>31|e<<1|0)),a=1&(w=(b=0|E())>>31|((0|b)<0?-1:0)<<1),B=0|Ef(0|s,0|k,w&(0|r)|0,(((0|b)<0?-1:0)>>31|((0|b)<0?-1:0)<<1)&(i|0&i)|0),e=0|E(),A=A-1|0}while(0!=(0|A));A=0}else o=a,e=w,A=0,a=0;return 0|n&&(t[n>>2]=B,t[n+4>>2]=e),s=-2&(f<<1|0)|a,0|(c(0|(k=(0|f)>>>31|o<<1|0&(0|f>>>31)|A)),s)}function bf(A,e,r,i){return 0|wf(A|=0,e|=0,r|=0,i|=0,0)}function kf(A,e,r,i){var n;return n=G,G=G+16|0,wf(A|=0,e|=0,r|=0,i|=0,0|n),G=n,0|(c(0|t[n+4>>2]),0|t[n>>2])}function sf(A,e,r){return A|=0,e|=0,(0|(r|=0))<32?(c(e>>>r|0),A>>>r|(e&(1<<r)-1)<<32-r):(c(0),e>>>r-32|0)}function Cf(A,e,r){return A|=0,e|=0,(0|(r|=0))<32?(c(e<<r|(A&(1<<r)-1<<32-r)>>>32-r|0),A<<r):(c(A<<r-32|0),0)}function df(){return 82816}function If(A,e,r){A|=0,e|=0;var n,f,a=0;if((0|(r|=0))>=512)return D(0|A,0|e,0|r),0|A;if(f=0|A,n=A+r|0,(3&A)==(3&e)){for(;3&A;){if(!r)return 0|f;i[A>>0]=0|i[e>>0],A=A+1|0,e=e+1|0,r=r-1|0}for(a=(r=-4&n|0)-64|0;(0|A)<=(0|a);)t[A>>2]=t[e>>2],t[A+4>>2]=t[e+4>>2],t[A+8>>2]=t[e+8>>2],t[A+12>>2]=t[e+12>>2],t[A+16>>2]=t[e+16>>2],t[A+20>>2]=t[e+20>>2],t[A+24>>2]=t[e+24>>2],t[A+28>>2]=t[e+28>>2],t[A+32>>2]=t[e+32>>2],t[A+36>>2]=t[e+36>>2],t[A+40>>2]=t[e+40>>2],t[A+44>>2]=t[e+44>>2],t[A+48>>2]=t[e+48>>2],t[A+52>>2]=t[e+52>>2],t[A+56>>2]=t[e+56>>2],t[A+60>>2]=t[e+60>>2],A=A+64|0,e=e+64|0;for(;(0|A)<(0|r);)t[A>>2]=t[e>>2],A=A+4|0,e=e+4|0}else for(r=n-4|0;(0|A)<(0|r);)i[A>>0]=0|i[e>>0],i[A+1>>0]=0|i[e+1>>0],i[A+2>>0]=0|i[e+2>>0],i[A+3>>0]=0|i[e+3>>0],A=A+4|0,e=e+4|0;for(;(0|A)<(0|n);)i[A>>0]=0|i[e>>0],A=A+1|0,e=e+1|0;return 0|f}function gf(A,e,r){var n=0;if((0|(e|=0))<(0|(A|=0))&(0|A)<(e+(r|=0)|0)){for(n=A,e=e+r|0,A=A+r|0;(0|r)>0;)e=e-1|0,r=r-1|0,i[(A=A-1|0)>>0]=0|i[e>>0];A=n}else If(A,e,r);return 0|A}function Df(A,e,r){e|=0;var n,f=0;if(n=(A|=0)+(r|=0)|0,e&=255,(0|r)>=67){for(;3&A;)i[A>>0]=e,A=A+1|0;for(f=e|e<<8|e<<16|e<<24;(0|A)<=((-4&n)-64|0);)t[A>>2]=f,t[A+4>>2]=f,t[A+8>>2]=f,t[A+12>>2]=f,t[A+16>>2]=f,t[A+20>>2]=f,t[A+24>>2]=f,t[A+28>>2]=f,t[A+32>>2]=f,t[A+36>>2]=f,t[A+40>>2]=f,t[A+44>>2]=f,t[A+48>>2]=f,t[A+52>>2]=f,t[A+56>>2]=f,t[A+60>>2]=f,A=A+64|0;for(;(0|A)<(-4&n|0);)t[A>>2]=f,A=A+4|0}for(;(0|A)<(0|n);)i[A>>0]=e,A=A+1|0;return n-r|0}function Mf(A){return 0,l(0),0}function hf(A,e){return 0,0,l(1),0}function Uf(A,e,r){return 0,0,0,l(2),0}function Gf(A,e,r,i){return 0,0,0,0,l(3),0}function Zf(A,e,r,i,n){return 0,0,0,0,0,l(4),0}function Nf(A,e,r,i,n,t){return 0,0,0,0,0,0,l(5),0}function Ff(){l(7)}function Wf(A){l(8)}function Yf(A,e){l(9)}function Rf(A,e,r){A|=0,e|=0;var i,n,f=0,a=0,B=0,o=0,u=0,l=0,c=0,Q=0,w=0,b=0;w=0|lf(0|t[(r|=0)>>2],0,0|t[e>>2],0),a=0|E(),t[A>>2]=w,a=0|cf(0|(w=0|lf(0|t[r+4>>2],0,0|t[e>>2],0)),0|E(),0|a,0),w=0|E(),a=0|cf(0|(f=0|lf(0|t[r>>2],0,0|t[e+4>>2],0)),0|E(),0|a,0),f=0|E(),t[A+4>>2]=a,a=0|cf(0|(a=0|lf(0|t[r>>2],0,0|t[e+8>>2],0)),0|E(),f+w|0,0),Q=((b=0|E())+((f+w|0)>>>0<f>>>0&1)|0)>>>0<b>>>0,B=0|cf(0|a,0,0|(B=0|lf(0|t[r+4>>2],0,0|t[e+4>>2],0)),0|E()),f=b+((f+w|0)>>>0<f>>>0&1)+(a=0|E())|0,w=0|cf(0|B,0,0|(w=0|lf(0|t[r+8>>2],0,0|t[e>>2],0)),0|E()),B=0|E(),t[A+8>>2]=w,w=0|cf(0|(w=0|lf(0|t[r+12>>2],0,0|t[e>>2],0)),0|E(),f+B|0,0),B=(f>>>0<a>>>0?Q?2:1:1&Q)+((f+B|0)>>>0<B>>>0&1)+(b=0|E())|0,f=0|cf(0|w,0,0|(f=0|lf(0|t[r+8>>2],0,0|t[e+4>>2],0)),0|E()),w=0|E(),Q=0|cf(0|f,0,0|(Q=0|lf(0|t[r+4>>2],0,0|t[e+8>>2],0)),0|E()),f=0|E(),a=0|cf(0|Q,0,0|(a=0|lf(0|t[r>>2],0,0|t[e+12>>2],0)),0|E()),Q=0|E(),t[A+12>>2]=a,a=0|cf(0|(a=0|lf(0|t[r>>2],0,0|t[e+16>>2],0)),0|E(),B+w+f+Q|0,0),Q=((B+w|0)>>>0<w>>>0?B>>>0<b>>>0?2:1:B>>>0<b>>>0&1)+((B+w+f|0)>>>0<f>>>0&1)+((B+w+f+Q|0)>>>0<Q>>>0&1)+(l=0|E())|0,f=0|cf(0|a,0,0|(f=0|lf(0|t[r+4>>2],0,0|t[e+12>>2],0)),0|E()),a=0|E(),w=0|cf(0|f,0,0|(w=0|lf(0|t[r+8>>2],0,0|t[e+8>>2],0)),0|E()),f=0|E(),B=0|cf(0|w,0,0|(B=0|lf(0|t[r+12>>2],0,0|t[e+4>>2],0)),0|E()),w=0|E(),b=0|cf(0|B,0,0|(b=0|lf(0|t[r+16>>2],0,0|t[e>>2],0)),0|E()),B=0|E(),t[A+16>>2]=b,b=0|cf(0|(b=0|lf(0|t[r+20>>2],0,0|t[e>>2],0)),0|E(),Q+a+f+w+B|0,0),B=((Q+a|0)>>>0<a>>>0?Q>>>0<l>>>0?2:1:Q>>>0<l>>>0&1)+((Q+a+f|0)>>>0<f>>>0&1)+((Q+a+f+w|0)>>>0<w>>>0&1)+((Q+a+f+w+B|0)>>>0<B>>>0&1)+(c=0|E())|0,w=0|cf(0|b,0,0|(w=0|lf(0|t[r+16>>2],0,0|t[e+4>>2],0)),0|E()),b=0|E(),f=0|cf(0|w,0,0|(f=0|lf(0|t[r+12>>2],0,0|t[e+8>>2],0)),0|E()),w=0|E(),a=0|cf(0|f,0,0|(a=0|lf(0|t[r+8>>2],0,0|t[e+12>>2],0)),0|E()),f=0|E(),Q=0|cf(0|a,0,0|(Q=0|lf(0|t[r+4>>2],0,0|t[e+16>>2],0)),0|E()),a=0|E(),l=0|cf(0|Q,0,0|(l=0|lf(0|t[r>>2],0,0|t[e+20>>2],0)),0|E()),o=B+b+w+f+a+(Q=0|E())|0,t[A+20>>2]=l,l=0|cf(0|(l=0|lf(0|t[r>>2],0,0|t[e+24>>2],0)),0|E(),0|o,0),Q=((B+b|0)>>>0<b>>>0?B>>>0<c>>>0?2:1:B>>>0<c>>>0&1)+((B+b+w|0)>>>0<w>>>0&1)+((B+b+w+f|0)>>>0<f>>>0&1)+((B+b+w+f+a|0)>>>0<a>>>0&1)+(o>>>0<Q>>>0&1)+(u=0|E())|0,o=0|cf(0|l,0,0|(o=0|lf(0|t[r+4>>2],0,0|t[e+20>>2],0)),0|E()),l=0|E(),a=0|cf(0|o,0,0|(a=0|lf(0|t[r+8>>2],0,0|t[e+16>>2],0)),0|E()),o=0|E(),f=0|cf(0|a,0,0|(f=0|lf(0|t[r+12>>2],0,0|t[e+12>>2],0)),0|E()),a=0|E(),w=0|cf(0|f,0,0|(w=0|lf(0|t[r+16>>2],0,0|t[e+8>>2],0)),0|E()),f=0|E(),b=0|cf(0|w,0,0|(b=0|lf(0|t[r+20>>2],0,0|t[e+4>>2],0)),0|E()),B=Q+l+o+a+f+(w=0|E())|0,c=0|cf(0|b,0,0|(c=0|lf(0|t[r+24>>2],0,0|t[e>>2],0)),0|E()),b=0|E(),t[A+24>>2]=c,c=0|cf(0|(c=0|lf(0|t[r+28>>2],0,0|t[e>>2],0)),0|E(),B+b|0,0),b=((Q+l|0)>>>0<l>>>0?Q>>>0<u>>>0?2:1:Q>>>0<u>>>0&1)+((Q+l+o|0)>>>0<o>>>0&1)+((Q+l+o+a|0)>>>0<a>>>0&1)+((Q+l+o+a+f|0)>>>0<f>>>0&1)+(B>>>0<w>>>0&1)+((B+b|0)>>>0<b>>>0&1)+(n=0|E())|0,B=0|cf(0|c,0,0|(B=0|lf(0|t[r+24>>2],0,0|t[e+4>>2],0)),0|E()),c=0|E(),w=0|cf(0|B,0,0|(w=0|lf(0|t[r+20>>2],0,0|t[e+8>>2],0)),0|E()),B=0|E(),f=0|cf(0|w,0,0|(f=0|lf(0|t[r+16>>2],0,0|t[e+12>>2],0)),0|E()),w=0|E(),a=0|cf(0|f,0,0|(a=0|lf(0|t[r+12>>2],0,0|t[e+16>>2],0)),0|E()),f=0|E(),o=0|cf(0|a,0,0|(o=0|lf(0|t[r+8>>2],0,0|t[e+20>>2],0)),0|E()),l=b+c+B+w+f+(a=0|E())|0,Q=0|cf(0|o,0,0|(Q=0|lf(0|t[r+4>>2],0,0|t[e+24>>2],0)),0|E()),o=0|E(),u=0|cf(0|Q,0,0|(u=0|lf(0|t[r>>2],0,0|t[e+28>>2],0)),0|E()),Q=0|E(),t[A+28>>2]=u,u=0|cf(0|(u=0|lf(0|t[r+4>>2],0,0|t[e+28>>2],0)),0|E(),l+o+Q|0,0),Q=((b+c|0)>>>0<c>>>0?b>>>0<n>>>0?2:1:b>>>0<n>>>0&1)+((b+c+B|0)>>>0<B>>>0&1)+((b+c+B+w|0)>>>0<w>>>0&1)+((b+c+B+w+f|0)>>>0<f>>>0&1)+(l>>>0<a>>>0&1)+((l+o|0)>>>0<o>>>0&1)+((l+o+Q|0)>>>0<Q>>>0&1)+(i=0|E())|0,o=0|cf(0|u,0,0|(o=0|lf(0|t[r+8>>2],0,0|t[e+24>>2],0)),0|E()),u=0|E(),l=0|cf(0|o,0,0|(l=0|lf(0|t[r+12>>2],0,0|t[e+20>>2],0)),0|E()),o=0|E(),a=0|cf(0|l,0,0|(a=0|lf(0|t[r+16>>2],0,0|t[e+16>>2],0)),0|E()),l=0|E(),f=0|cf(0|a,0,0|(f=0|lf(0|t[r+20>>2],0,0|t[e+12>>2],0)),0|E()),a=0|E(),w=0|cf(0|f,0,0|(w=0|lf(0|t[r+24>>2],0,0|t[e+8>>2],0)),0|E()),B=Q+u+o+l+a+(f=0|E())|0,c=0|cf(0|w,0,0|(c=0|lf(0|t[r+28>>2],0,0|t[e+4>>2],0)),0|E()),w=0|E(),t[A+32>>2]=c,c=0|cf(0|(c=0|lf(0|t[r+28>>2],0,0|t[e+8>>2],0)),0|E(),B+w|0,0),w=((Q+u|0)>>>0<u>>>0?Q>>>0<i>>>0?2:1:Q>>>0<i>>>0&1)+((Q+u+o|0)>>>0<o>>>0&1)+((Q+u+o+l|0)>>>0<l>>>0&1)+((Q+u+o+l+a|0)>>>0<a>>>0&1)+(B>>>0<f>>>0&1)+((B+w|0)>>>0<w>>>0&1)+(b=0|E())|0,B=0|cf(0|c,0,0|(B=0|lf(0|t[r+24>>2],0,0|t[e+12>>2],0)),0|E()),c=0|E(),f=0|cf(0|B,0,0|(f=0|lf(0|t[r+20>>2],0,0|t[e+16>>2],0)),0|E()),B=0|E(),a=0|cf(0|f,0,0|(a=0|lf(0|t[r+16>>2],0,0|t[e+20>>2],0)),0|E()),f=0|E(),l=0|cf(0|a,0,0|(l=0|lf(0|t[r+12>>2],0,0|t[e+24>>2],0)),0|E()),a=0|E(),o=0|cf(0|l,0,0|(o=0|lf(0|t[r+8>>2],0,0|t[e+28>>2],0)),0|E()),u=w+c+B+f+a+(l=0|E())|0,t[A+36>>2]=o,o=0|cf(0|(o=0|lf(0|t[r+12>>2],0,0|t[e+28>>2],0)),0|E(),0|u,0),l=((w+c|0)>>>0<c>>>0?w>>>0<b>>>0?2:1:w>>>0<b>>>0&1)+((w+c+B|0)>>>0<B>>>0&1)+((w+c+B+f|0)>>>0<f>>>0&1)+((w+c+B+f+a|0)>>>0<a>>>0&1)+(u>>>0<l>>>0&1)+(Q=0|E())|0,u=0|cf(0|o,0,0|(u=0|lf(0|t[r+16>>2],0,0|t[e+24>>2],0)),0|E()),o=0|E(),a=0|cf(0|u,0,0|(a=0|lf(0|t[r+20>>2],0,0|t[e+20>>2],0)),0|E()),u=0|E(),f=0|cf(0|a,0,0|(f=0|lf(0|t[r+24>>2],0,0|t[e+16>>2],0)),0|E()),a=0|E(),B=0|cf(0|f,0,0|(B=0|lf(0|t[r+28>>2],0,0|t[e+12>>2],0)),0|E()),f=0|E(),t[A+40>>2]=B,B=0|cf(0|(B=0|lf(0|t[r+28>>2],0,0|t[e+16>>2],0)),0|E(),l+o+u+a+f|0,0),f=((l+o|0)>>>0<o>>>0?l>>>0<Q>>>0?2:1:l>>>0<Q>>>0&1)+((l+o+u|0)>>>0<u>>>0&1)+((l+o+u+a|0)>>>0<a>>>0&1)+((l+o+u+a+f|0)>>>0<f>>>0&1)+(c=0|E())|0,a=0|cf(0|B,0,0|(a=0|lf(0|t[r+24>>2],0,0|t[e+20>>2],0)),0|E()),B=0|E(),u=0|cf(0|a,0,0|(u=0|lf(0|t[r+20>>2],0,0|t[e+24>>2],0)),0|E()),a=0|E(),o=0|cf(0|u,0,0|(o=0|lf(0|t[r+16>>2],0,0|t[e+28>>2],0)),0|E()),u=0|E(),t[A+44>>2]=o,o=0|cf(0|(o=0|lf(0|t[r+20>>2],0,0|t[e+28>>2],0)),0|E(),f+B+a+u|0,0),u=((f+B|0)>>>0<B>>>0?f>>>0<c>>>0?2:1:f>>>0<c>>>0&1)+((f+B+a|0)>>>0<a>>>0&1)+((f+B+a+u|0)>>>0<u>>>0&1)+(l=0|E())|0,a=0|cf(0|o,0,0|(a=0|lf(0|t[r+24>>2],0,0|t[e+24>>2],0)),0|E()),o=0|E(),B=0|cf(0|a,0,0|(B=0|lf(0|t[r+28>>2],0,0|t[e+20>>2],0)),0|E()),a=0|E(),t[A+48>>2]=B,B=0|cf(0|(B=0|lf(0|t[r+28>>2],0,0|t[e+24>>2],0)),0|E(),u+o+a|0,0),a=((u+o|0)>>>0<o>>>0?u>>>0<l>>>0?2:1:u>>>0<l>>>0&1)+((u+o+a|0)>>>0<a>>>0&1)+(f=0|E())|0,o=0|cf(0|B,0,0|(o=0|lf(0|t[r+24>>2],0,0|t[e+28>>2],0)),0|E()),B=0|E(),t[A+52>>2]=o,e=0|cf(0|(e=0|lf(0|t[r+28>>2],0,0|t[e+28>>2],0)),0|E(),a+B|0,0),r=((a+B|0)>>>0<B>>>0?a>>>0<f>>>0?2:1:a>>>0<f>>>0&1)+(0|E())|0,t[A+56>>2]=e,t[A+60>>2]=r}function yf(A,e,r){A|=0,e|=0;var i,n=0,f=0,a=0,B=0,o=0,u=0;a=0|lf(0|t[(r|=0)>>2],0,0|t[e>>2],0),B=0|E(),t[A>>2]=a,B=0|cf(0|(a=0|lf(0|t[r+4>>2],0,0|t[e>>2],0)),0|E(),0|B,0),a=0|E(),B=0|cf(0|(f=0|lf(0|t[r>>2],0,0|t[e+4>>2],0)),0|E(),0|B,0),f=0|E(),t[A+4>>2]=B,B=0|cf(0|(B=0|lf(0|t[r>>2],0,0|t[e+8>>2],0)),0|E(),f+a|0,0),o=((u=0|E())+((f+a|0)>>>0<f>>>0&1)|0)>>>0<u>>>0,n=0|cf(0|B,0,0|(n=0|lf(0|t[r+4>>2],0,0|t[e+4>>2],0)),0|E()),f=u+((f+a|0)>>>0<f>>>0&1)+(B=0|E())|0,a=0|cf(0|n,0,0|(a=0|lf(0|t[r+8>>2],0,0|t[e>>2],0)),0|E()),n=0|E(),t[A+8>>2]=a,a=0|cf(0|(a=0|lf(0|t[r+12>>2],0,0|t[e>>2],0)),0|E(),f+n|0,0),n=(f>>>0<B>>>0?o?2:1:1&o)+((f+n|0)>>>0<n>>>0&1)+(u=0|E())|0,f=0|cf(0|a,0,0|(f=0|lf(0|t[r+8>>2],0,0|t[e+4>>2],0)),0|E()),a=0|E(),o=0|cf(0|f,0,0|(o=0|lf(0|t[r+4>>2],0,0|t[e+8>>2],0)),0|E()),f=0|E(),B=0|cf(0|o,0,0|(B=0|lf(0|t[r>>2],0,0|t[e+12>>2],0)),0|E()),o=0|E(),t[A+12>>2]=B,B=0|cf(0|(B=0|lf(0|t[r+4>>2],0,0|t[e+12>>2],0)),0|E(),n+a+f+o|0,0),o=((n+a|0)>>>0<a>>>0?n>>>0<u>>>0?2:1:n>>>0<u>>>0&1)+((n+a+f|0)>>>0<f>>>0&1)+((n+a+f+o|0)>>>0<o>>>0&1)+(i=0|E())|0,f=0|cf(0|B,0,0|(f=0|lf(0|t[r+8>>2],0,0|t[e+8>>2],0)),0|E()),B=0|E(),a=0|cf(0|f,0,0|(a=0|lf(0|t[r+12>>2],0,0|t[e+4>>2],0)),0|E()),f=0|E(),t[A+16>>2]=a,a=0|cf(0|(a=0|lf(0|t[r+12>>2],0,0|t[e+8>>2],0)),0|E(),o+B+f|0,0),f=((o+B|0)>>>0<B>>>0?o>>>0<i>>>0?2:1:o>>>0<i>>>0&1)+((o+B+f|0)>>>0<f>>>0&1)+(n=0|E())|0,B=0|cf(0|a,0,0|(B=0|lf(0|t[r+8>>2],0,0|t[e+12>>2],0)),0|E()),a=0|E(),t[A+20>>2]=B,e=0|cf(0|(e=0|lf(0|t[r+12>>2],0,0|t[e+12>>2],0)),0|E(),f+a|0,0),r=((f+a|0)>>>0<a>>>0?f>>>0<n>>>0?2:1:f>>>0<n>>>0&1)+(0|E())|0,t[A+24>>2]=e,t[A+28>>2]=r}function Vf(A,e){A|=0;var r=0,i=0,n=0,f=0,a=0,B=0,o=0,u=0,l=0,c=0,Q=0,w=0,b=0;f=0|lf(0|(f=0|t[(e|=0)>>2]),0,0|f,0),l=0|E(),t[A>>2]=f,l=0|cf(0|(f=0|lf(0|t[e>>2],0,0|t[e+4>>2],0)),0|(a=0|E()),0|l,0),n=0|E(),a=0|cf(0|l,0,0|f,0|a),f=0|E(),t[A+4>>2]=a,a=0|cf(0|(a=0|lf(0|(a=0|t[e+4>>2]),0,0|a,0)),0|E(),f+n|0,0),Q=((l=0|E())+((f+n|0)>>>0<f>>>0&1)|0)>>>0<l>>>0,a=0|cf(0|a,0,0|(b=0|lf(0|t[e>>2],0,0|t[e+8>>2],0)),0|(c=0|E())),f=l+((f+n|0)>>>0<f>>>0&1)+(w=0|E())|0,c=0|cf(0|a,0,0|b,0|c),b=0|E(),t[A+8>>2]=c,n=0|cf(0|(c=0|lf(0|t[e>>2],0,0|t[e+12>>2],0)),0|(a=0|E()),f+b|0,0),b=(f>>>0<w>>>0?Q?2:1:1&Q)+((f+b|0)>>>0<b>>>0&1)+(l=0|E())|0,a=0|cf(0|n,0,0|c,0|a),c=0|E(),a=0|cf(0|a,0,0|(n=0|lf(0|t[e+4>>2],0,0|t[e+8>>2],0)),0|(f=0|E())),Q=0|E(),f=0|cf(0|a,0,0|n,0|f),n=0|E(),t[A+12>>2]=f,f=0|cf(0|(f=0|lf(0|(f=0|t[e+8>>2]),0,0|f,0)),0|E(),b+c+Q+n|0,0),n=((b+c|0)>>>0<c>>>0?b>>>0<l>>>0?2:1:b>>>0<l>>>0&1)+((b+c+Q|0)>>>0<Q>>>0&1)+((b+c+Q+n|0)>>>0<n>>>0&1)+(a=0|E())|0,f=0|cf(0|f,0,0|(Q=0|lf(0|t[e+4>>2],0,0|t[e+12>>2],0)),0|(c=0|E())),b=0|E(),c=0|cf(0|f,0,0|Q,0|c),Q=0|E(),c=0|cf(0|c,0,0|(f=0|lf(0|t[e>>2],0,0|t[e+16>>2],0)),0|(l=0|E())),w=0|E(),l=0|cf(0|c,0,0|f,0|l),f=0|E(),t[A+16>>2]=l,B=0|cf(0|(l=0|lf(0|t[e>>2],0,0|t[e+20>>2],0)),0|(c=0|E()),n+b+Q+w+f|0,0),f=((n+b|0)>>>0<b>>>0?n>>>0<a>>>0?2:1:n>>>0<a>>>0&1)+((n+b+Q|0)>>>0<Q>>>0&1)+((n+b+Q+w|0)>>>0<w>>>0&1)+((n+b+Q+w+f|0)>>>0<f>>>0&1)+(r=0|E())|0,c=0|cf(0|B,0,0|l,0|c),l=0|E(),c=0|cf(0|c,0,0|(B=0|lf(0|t[e+4>>2],0,0|t[e+16>>2],0)),0|(w=0|E())),Q=0|E(),w=0|cf(0|c,0,0|B,0|w),B=0|E(),w=0|cf(0|w,0,0|(c=0|lf(0|t[e+8>>2],0,0|t[e+12>>2],0)),0|(b=0|E())),n=0|E(),b=0|cf(0|w,0,0|c,0|b),w=f+l+Q+B+n+(c=0|E())|0,t[A+20>>2]=b,b=0|cf(0|(b=0|lf(0|(b=0|t[e+12>>2]),0,0|b,0)),0|E(),0|w,0),c=((f+l|0)>>>0<l>>>0?f>>>0<r>>>0?2:1:f>>>0<r>>>0&1)+((f+l+Q|0)>>>0<Q>>>0&1)+((f+l+Q+B|0)>>>0<B>>>0&1)+((f+l+Q+B+n|0)>>>0<n>>>0&1)+(w>>>0<c>>>0&1)+(a=0|E())|0,b=0|cf(0|b,0,0|(w=0|lf(0|t[e+8>>2],0,0|t[e+16>>2],0)),0|(n=0|E())),B=0|E(),n=0|cf(0|b,0,0|w,0|n),w=0|E(),n=0|cf(0|n,0,0|(b=0|lf(0|t[e+4>>2],0,0|t[e+20>>2],0)),0|(Q=0|E())),l=0|E(),Q=0|cf(0|n,0,0|b,0|Q),b=0|E(),Q=0|cf(0|Q,0,0|(n=0|lf(0|t[e>>2],0,0|t[e+24>>2],0)),0|(f=0|E())),o=c+B+w+l+b+(r=0|E())|0,f=0|cf(0|Q,0,0|n,0|f),n=0|E(),t[A+24>>2]=f,u=0|cf(0|(f=0|lf(0|t[e>>2],0,0|t[e+28>>2],0)),0|(Q=0|E()),o+n|0,0),n=((c+B|0)>>>0<B>>>0?c>>>0<a>>>0?2:1:c>>>0<a>>>0&1)+((c+B+w|0)>>>0<w>>>0&1)+((c+B+w+l|0)>>>0<l>>>0&1)+((c+B+w+l+b|0)>>>0<b>>>0&1)+(o>>>0<r>>>0&1)+((o+n|0)>>>0<n>>>0&1)+(i=0|E())|0,Q=0|cf(0|u,0,0|f,0|Q),f=0|E(),Q=0|cf(0|Q,0,0|(u=0|lf(0|t[e+4>>2],0,0|t[e+24>>2],0)),0|(o=0|E())),r=0|E(),o=0|cf(0|Q,0,0|u,0|o),u=0|E(),o=0|cf(0|o,0,0|(Q=0|lf(0|t[e+8>>2],0,0|t[e+20>>2],0)),0|(b=0|E())),l=0|E(),b=0|cf(0|o,0,0|Q,0|b),o=n+f+r+u+l+(Q=0|E())|0,b=0|cf(0|b,0,0|(w=0|lf(0|t[e+12>>2],0,0|t[e+16>>2],0)),0|(B=0|E())),c=0|E(),B=0|cf(0|b,0,0|w,0|B),w=0|E(),t[A+28>>2]=B,B=0|cf(0|(B=0|lf(0|(B=0|t[e+16>>2]),0,0|B,0)),0|E(),o+c+w|0,0),w=((n+f|0)>>>0<f>>>0?n>>>0<i>>>0?2:1:n>>>0<i>>>0&1)+((n+f+r|0)>>>0<r>>>0&1)+((n+f+r+u|0)>>>0<u>>>0&1)+((n+f+r+u+l|0)>>>0<l>>>0&1)+(o>>>0<Q>>>0&1)+((o+c|0)>>>0<c>>>0&1)+((o+c+w|0)>>>0<w>>>0&1)+(b=0|E())|0,B=0|cf(0|B,0,0|(c=0|lf(0|t[e+12>>2],0,0|t[e+20>>2],0)),0|(o=0|E())),Q=0|E(),o=0|cf(0|B,0,0|c,0|o),c=0|E(),o=0|cf(0|o,0,0|(B=0|lf(0|t[e+8>>2],0,0|t[e+24>>2],0)),0|(l=0|E())),u=0|E(),l=0|cf(0|o,0,0|B,0|l),B=0|E(),l=0|cf(0|l,0,0|(o=0|lf(0|t[e+4>>2],0,0|t[e+28>>2],0)),0|(r=0|E())),n=w+Q+c+u+B+(f=0|E())|0,r=0|cf(0|l,0,0|o,0|r),o=0|E(),t[A+32>>2]=r,i=0|cf(0|(r=0|lf(0|t[e+8>>2],0,0|t[e+28>>2],0)),0|(l=0|E()),n+o|0,0),o=((w+Q|0)>>>0<Q>>>0?w>>>0<b>>>0?2:1:w>>>0<b>>>0&1)+((w+Q+c|0)>>>0<c>>>0&1)+((w+Q+c+u|0)>>>0<u>>>0&1)+((w+Q+c+u+B|0)>>>0<B>>>0&1)+(n>>>0<f>>>0&1)+((n+o|0)>>>0<o>>>0&1)+(a=0|E())|0,l=0|cf(0|i,0,0|r,0|l),r=0|E(),l=0|cf(0|l,0,0|(i=0|lf(0|t[e+12>>2],0,0|t[e+24>>2],0)),0|(n=0|E())),f=0|E(),n=0|cf(0|l,0,0|i,0|n),i=0|E(),n=0|cf(0|n,0,0|(l=0|lf(0|t[e+16>>2],0,0|t[e+20>>2],0)),0|(B=0|E())),u=0|E(),B=0|cf(0|n,0,0|l,0|B),n=o+r+f+i+u+(l=0|E())|0,t[A+36>>2]=B,B=0|cf(0|(B=0|lf(0|(B=0|t[e+20>>2]),0,0|B,0)),0|E(),0|n,0),l=((o+r|0)>>>0<r>>>0?o>>>0<a>>>0?2:1:o>>>0<a>>>0&1)+((o+r+f|0)>>>0<f>>>0&1)+((o+r+f+i|0)>>>0<i>>>0&1)+((o+r+f+i+u|0)>>>0<u>>>0&1)+(n>>>0<l>>>0&1)+(c=0|E())|0,B=0|cf(0|B,0,0|(n=0|lf(0|t[e+16>>2],0,0|t[e+24>>2],0)),0|(u=0|E())),i=0|E(),u=0|cf(0|B,0,0|n,0|u),n=0|E(),u=0|cf(0|u,0,0|(B=0|lf(0|t[e+12>>2],0,0|t[e+28>>2],0)),0|(f=0|E())),r=0|E(),f=0|cf(0|u,0,0|B,0|f),B=0|E(),t[A+40>>2]=f,o=0|cf(0|(f=0|lf(0|t[e+16>>2],0,0|t[e+28>>2],0)),0|(u=0|E()),l+i+n+r+B|0,0),B=((l+i|0)>>>0<i>>>0?l>>>0<c>>>0?2:1:l>>>0<c>>>0&1)+((l+i+n|0)>>>0<n>>>0&1)+((l+i+n+r|0)>>>0<r>>>0&1)+((l+i+n+r+B|0)>>>0<B>>>0&1)+(a=0|E())|0,u=0|cf(0|o,0,0|f,0|u),f=0|E(),u=0|cf(0|u,0,0|(o=0|lf(0|t[e+20>>2],0,0|t[e+24>>2],0)),0|(r=0|E())),n=0|E(),r=0|cf(0|u,0,0|o,0|r),o=0|E(),t[A+44>>2]=r,r=0|cf(0|(r=0|lf(0|(r=0|t[e+24>>2]),0,0|r,0)),0|E(),B+f+n+o|0,0),o=((B+f|0)>>>0<f>>>0?B>>>0<a>>>0?2:1:B>>>0<a>>>0&1)+((B+f+n|0)>>>0<n>>>0&1)+((B+f+n+o|0)>>>0<o>>>0&1)+(u=0|E())|0,r=0|cf(0|r,0,0|(n=0|lf(0|t[e+20>>2],0,0|t[e+28>>2],0)),0|(f=0|E())),B=0|E(),f=0|cf(0|r,0,0|n,0|f),n=0|E(),t[A+48>>2]=f,a=0|cf(0|(f=0|lf(0|t[e+24>>2],0,0|t[e+28>>2],0)),0|(r=0|E()),o+B+n|0,0),n=((o+B|0)>>>0<B>>>0?o>>>0<u>>>0?2:1:o>>>0<u>>>0&1)+((o+B+n|0)>>>0<n>>>0&1)+(i=0|E())|0,r=0|cf(0|a,0,0|f,0|r),f=0|E(),t[A+52>>2]=r,r=0|cf(0|(r=0|lf(0|(r=0|t[e+28>>2]),0,0|r,0)),0|E(),n+f|0,0),e=((n+f|0)>>>0<f>>>0?n>>>0<i>>>0?2:1:n>>>0<i>>>0&1)+(0|E())|0,t[A+56>>2]=r,t[A+60>>2]=e}function mf(A,e){A|=0;var r=0,i=0,n=0,f=0,a=0,B=0,o=0,u=0;r=0|lf(0|(r=0|t[(e|=0)>>2]),0,0|r,0),a=0|E(),t[A>>2]=r,a=0|cf(0|(r=0|lf(0|t[e>>2],0,0|t[e+4>>2],0)),0|(u=0|E()),0|a,0),o=0|E(),u=0|cf(0|a,0,0|r,0|u),r=0|E(),t[A+4>>2]=u,u=0|cf(0|(u=0|lf(0|(u=0|t[e+4>>2]),0,0|u,0)),0|E(),r+o|0,0),n=((a=0|E())+((r+o|0)>>>0<r>>>0&1)|0)>>>0<a>>>0,u=0|cf(0|u,0,0|(B=0|lf(0|t[e>>2],0,0|t[e+8>>2],0)),0|(f=0|E())),r=a+((r+o|0)>>>0<r>>>0&1)+(i=0|E())|0,f=0|cf(0|u,0,0|B,0|f),B=0|E(),t[A+8>>2]=f,o=0|cf(0|(f=0|lf(0|t[e>>2],0,0|t[e+12>>2],0)),0|(u=0|E()),r+B|0,0),B=(r>>>0<i>>>0?n?2:1:1&n)+((r+B|0)>>>0<B>>>0&1)+(a=0|E())|0,u=0|cf(0|o,0,0|f,0|u),f=0|E(),u=0|cf(0|u,0,0|(o=0|lf(0|t[e+4>>2],0,0|t[e+8>>2],0)),0|(r=0|E())),n=0|E(),r=0|cf(0|u,0,0|o,0|r),o=0|E(),t[A+12>>2]=r,r=0|cf(0|(r=0|lf(0|(r=0|t[e+8>>2]),0,0|r,0)),0|E(),B+f+n+o|0,0),o=((B+f|0)>>>0<f>>>0?B>>>0<a>>>0?2:1:B>>>0<a>>>0&1)+((B+f+n|0)>>>0<n>>>0&1)+((B+f+n+o|0)>>>0<o>>>0&1)+(u=0|E())|0,r=0|cf(0|r,0,0|(n=0|lf(0|t[e+4>>2],0,0|t[e+12>>2],0)),0|(f=0|E())),B=0|E(),f=0|cf(0|r,0,0|n,0|f),n=0|E(),t[A+16>>2]=f,a=0|cf(0|(f=0|lf(0|t[e+8>>2],0,0|t[e+12>>2],0)),0|(r=0|E()),o+B+n|0,0),n=((o+B|0)>>>0<B>>>0?o>>>0<u>>>0?2:1:o>>>0<u>>>0&1)+((o+B+n|0)>>>0<n>>>0&1)+(i=0|E())|0,r=0|cf(0|a,0,0|f,0|r),f=0|E(),t[A+20>>2]=r,r=0|cf(0|(r=0|lf(0|(r=0|t[e+12>>2]),0,0|r,0)),0|E(),n+f|0,0),e=((n+f|0)>>>0<f>>>0?n>>>0<i>>>0?2:1:n>>>0<i>>>0&1)+(0|E())|0,t[A+24>>2]=r,t[A+28>>2]=e}function Hf(A,e,r,i){var n;return n=G,G=G+16|0,e=0|vf(A|=0,e|=0,r|=0,i|=0,n),0|t[n>>2]?(Xe(3,110,108,0,0),G=n,0|e):(G=n,0|e)}function vf(A,e,r,i,n){A|=0,e|=0,i|=0,n|=0;var f,a=0,B=0,o=0,u=0,l=0,c=0,E=0,Q=0,w=0,b=0,k=0,s=0;if(f=G,G=G+32|0,0==(0|ka(r|=0,1))&&0==(0|jf(r))){if(0|n&&(t[n>>2]=0),0==(0|Of(e,4))&&0==(0|Of(r,4))){iB(i),u=0|tB(i),B=0|tB(i),a=0|tB(i),E=0|tB(i),l=0|tB(i),w=0|tB(i),Q=0|tB(i);A:do{if(Q){if(A)b=A;else{if(!(o=0|qf())){B=0,a=0;break}b=o}if(ta(a,1),ba(w),0!=(0|ra(B,e))&&0!=(0|ra(u,r))){if(t[u+12>>2]=0,0==(0|t[B+12>>2])&&(0|la(B,u))<=-1||(k=42),42==(0|k)&&0==(0|$a(B,B,u,i))){B=b,a=0;break}if(0!=(0|da(r))&&(0|Kf(r))<2049)if(0|jf(B))B=u,a=w,k=97;else for(;;){if(!(0|Qa(B,0))){o=0;do{if(o=o+1|0,0|da(a)&&0==(0|ln(a,a,r))){B=b,a=0;break A}if(!(0|en(a,a))){B=b,a=0;break A}}while(!(0|Qa(B,o)));if(!(0|nn(B,B,o))){B=b,a=0;break A}}if(!(0|Qa(u,0))){o=0;do{if(o=o+1|0,0|da(w)&&0==(0|ln(w,w,r))){B=b,a=0;break A}if(!(0|en(w,w))){B=b,a=0;break A}}while(!(0|Qa(u,o)));if(!(0|nn(u,u,o))){B=b,a=0;break A}}if((0|la(B,u))>-1){if(!(0|ln(a,a,w))){B=b,a=0;break A}if(!(0|un(B,B,u))){B=b,a=0;break A}}else{if(!(0|ln(w,w,a))){B=b,a=0;break A}if(!(0|un(u,u,B))){B=b,a=0;break A}}if(0|jf(B)){B=u,a=w,k=97;break}}else if(0|jf(B))B=u,a=w,k=97;else{for(c=u,o=w,u=-1;;){e=0|Kf(c);do{if((0|e)==(0|Kf(B))){if(!(0|ta(E,1))){B=b,a=0;break A}if(!(0|cn(l,c,B))){B=b,a=0;break A}}else{if((0|(e=0|Kf(c)))!=(1+(0|Kf(B))|0)){if(0|wB(E,l,c,B,i))break;B=b,a=0;break A}if(!(0|An(Q,B))){B=b,a=0;break A}if((0|la(c,Q))<0){if(!(0|ta(E,1))){B=b,a=0;break A}if(0|cn(l,c,B))break;B=b,a=0;break A}if(!(0|cn(l,c,Q))){B=b,a=0;break A}if(!(0|on(E,Q,B))){B=b,a=0;break A}if((0|la(c,E))<0){if(0|ta(E,2))break;B=b,a=0;break A}if(!(0|ta(E,3))){B=b,a=0;break A}if(!(0|cn(l,l,B))){B=b,a=0;break A}}}while(0);if(0|sa(E)){if(!(0|on(c,a,o))){B=b,a=0;break A}}else{do{if(0|Ca(E,2)){if(!(0|An(c,a))){B=b,a=0;break A}}else{if(0|Ca(E,4)){if(0|rn(c,a,2))break;B=b,a=0;break A}if(1!=(0|t[E+4>>2])){if(0|Sf(c,E,a,i))break;B=b,a=0;break A}if(!(0|ra(c,a))){B=b,a=0;break A}if(!(0|QB(c,0|t[t[E>>2]>>2]))){B=b,a=0;break A}}}while(0);if(!(0|on(c,c,o))){B=b,a=0;break A}}if(0|jf(l))break;s=o,w=l,e=B,u=0-u|0,o=a,a=c,l=s,B=w,c=e}(0|u)>0&&(k=97)}if(97==(0|k)&&!(0|cn(a,r,a))){B=b,a=0;break}if(!(0|sa(B))){if(!n){B=b,a=0;break}t[n>>2]=1,B=b,a=0;break}if(0==(0|t[a+12>>2])&&(0|la(a,r))<0){if(!(0|ra(b,a))){B=b,a=0;break}}else k=102;if(102==(0|k)&&0==(0|$a(b,a,r,i))){B=b,a=0;break}B=b,a=b}else B=b,a=0}else B=0,a=0}while(0);return 0==(0|A)&0==(0|a)&&Pf(B),nB(i),G=f,0|(s=a)}iB(i),l=0|tB(i),B=0|tB(i),a=0|tB(i),Q=0|tB(i),c=0|tB(i),o=0|tB(i);A:do{if(0|tB(i)){if(A)u=A;else if(!(u=0|qf())){B=0,a=0;break}if(ta(a,1),ba(o),0!=(0|ra(B,e))&&0!=(0|ra(l,r))){if(t[l+12>>2]=0,0==(0|t[B+12>>2])&&(0|la(B,l))<=-1||(k=16),16==(0|k)&&(_f(f),Da(f,B,4),0==(0|$a(B,f,l,i)))){B=u,a=0;break}if(0|jf(B))B=l,a=o,k=24;else{for(E=l,l=-1;;){if(_f(f),Da(f,E,4),!(0|wB(Q,c,f,B,i))){B=u,a=0;break A}if(!(0|Sf(E,Q,a,i))){B=u,a=0;break A}if(!(0|on(E,E,o))){B=u,a=0;break A}if(0|jf(c))break;b=o,n=c,s=B,l=0-l|0,o=a,a=E,c=b,B=n,E=s}(0|l)>0&&(k=24)}if(24==(0|k)&&!(0|cn(a,r,a))){B=u,a=0;break}if(!(0|sa(B))){Xe(3,139,108,0,0),B=u,a=0;break}if(0==(0|t[a+12>>2])&&(0|la(a,r))<0){if(!(0|ra(u,a))){B=u,a=0;break}}else k=29;if(29==(0|k)&&0==(0|$a(u,a,r,i))){B=u,a=0;break}B=u,a=u}else B=u,a=0}else B=0,a=0}while(0);return 0==(0|A)&0==(0|a)&&Pf(B),nB(i),G=f,0|(s=a)}return n?(t[n>>2]=1,G=f,0|(s=0)):(G=f,0|(s=0))}function Jf(A,e,r,i,n){n|=0;var f=0,a=0,B=0,o=0,u=0,l=0,c=0,E=0,Q=0;if(o=0|Un(A|=0,e|=0,r|=0,i|=0),!n)return 0|(Q=o);if((0|n)<0){if(u=0|t[r+(i<<2)>>2],t[A+(i<<2)>>2]=0-(u+o),u=0==(0|u)?o:1,-1==(0|n))return 0|(Q=u);for(l=r+(i<<2)|0,c=n,E=A+(i<<2)|0,r=u;;){if(u=0|t[l+4>>2],t[E+4>>2]=0-(u+r),r=0==(0|u)?r:1,-2==(0|c)){c=r,r=23;break}if(u=0|t[l+8>>2],t[E+8>>2]=0-(u+r),r=0==(0|u)?r:1,(0|c)>-4){c=r,r=23;break}if(u=0|t[l+12>>2],t[E+12>>2]=0-(u+r),r=0==(0|u)?r:1,-4==(0|c)){c=r,r=23;break}if(E=E+16|0,u=0|t[(l=l+16|0)>>2],t[E>>2]=0-(u+r),r=0==(0|u)?r:1,(0|c)>-6){c=r,r=23;break}c=c+4|0}if(23==(0|r))return 0|c}do{if(o){for(l=e+(i<<2)|0,u=A+(i<<2)|0;;){if(r=0|t[l>>2],t[u>>2]=r-o,r=0==(0|r)?o:0,(0|n)<2){c=r,r=23;break}if(A=0|t[l+4>>2],t[u+4>>2]=A-r,r=0==(0|A)?r:0,2==(0|n)){c=r,r=23;break}if(o=0|t[l+8>>2],t[u+8>>2]=o-r,o=0==(0|o)?r:0,(0|n)<4){c=o,r=23;break}if(r=0|t[l+12>>2],t[u+12>>2]=r-o,r=0==(0|r)?o:0,Q=n+-4|0,4==(0|n)){c=r,r=23;break}if(l=l+16|0,u=u+16|0,!r){r=16;break}n=Q}if(16==(0|r)){if((0|n)>4){B=l,a=Q,f=u;break}return 0|(c=0)}if(23==(0|r))return 0|c}else B=e+(i<<2)|0,a=n,f=A+(i<<2)|0}while(0);if(t[f>>2]=t[B>>2],1==(0|a))return 0|(Q=0);for(;;){if(t[f+4>>2]=t[B+4>>2],2==(0|a)){c=0,r=23;break}if(t[f+8>>2]=t[B+8>>2],(0|a)<4){c=0,r=23;break}if(t[f+12>>2]=t[B+12>>2],4==(0|a)){c=0,r=23;break}if(B=B+16|0,t[(f=f+16|0)>>2]=t[B>>2],(0|a)<6){c=0,r=23;break}a=a+-4|0}return 23==(0|r)?0|c:0}function Xf(A,e,r,i,n,f,a){A|=0,e|=0,r|=0,a|=0;var B,o,u=0,l=0,c=0;if(u=((0|(i|=0))/2|0)+(n|=0)|0,l=((0|i)/2|0)+(f|=0)|0,8==(0|i)&0==(f|n|0))Rf(A,e,r);else if((0|i)<16){if(Tf(A,e,n+i|0,r,f+i|0),(f+n|0)>=0)return;Df(A+(f+n+(i<<1)<<2)|0,0,0-(f+n<<2)|0)}else{switch((3*(c=0|wa(e,B=e+(((0|i)/2|0)<<2)|0,u,0-n|0))|0)+(0|wa(o=r+(((0|i)/2|0)<<2)|0,r,l,f))|0){case-4:Jf(a,B,e,u,n),Jf(a+(((0|i)/2|0)<<2)|0,r,o,l,0-f|0),c=0,u=0;break;case-3:c=0,u=1;break;case-2:Jf(a,B,e,u,n),Jf(a+(((0|i)/2|0)<<2)|0,o,r,l,f),c=1,u=0;break;case 1:case 0:case-1:c=0,u=1;break;case 2:Jf(a,e,B,u,0-n|0),Jf(a+(((0|i)/2|0)<<2)|0,r,o,l,0-f|0),c=1,u=0;break;case 3:c=0,u=1;break;case 4:Jf(a,e,B,u,0-n|0),Jf(a+(((0|i)/2|0)<<2)|0,o,r,l,f),c=0,u=0;break;default:c=0,u=0}do{if(!(8==(-2&i|0)&0==(f|n|0))){if(16==(-2&i|0)&0==(f|n|0)){if(u){l=(u=a+(i<<2)|0)+64|0;do{t[u>>2]=0,u=u+4|0}while((0|u)<(0|l))}else Rf(a+(i<<2)|0,a,a+(((0|i)/2|0)<<2)|0);Rf(A,e,r),Rf(A+(i<<2)|0,B,o),u=A+(i<<2)|0,l=a+(i<<2)|0;break}u?Df(a+(i<<2)|0,0,i<<2|0):Xf(a+(i<<2)|0,a,a+(((0|i)/2|0)<<2)|0,(0|i)/2|0,0,0,a+(i<<1<<2)|0),Xf(A,e,r,(0|i)/2|0,0,0,a+(i<<1<<2)|0),Xf(A+(i<<2)|0,B,o,(0|i)/2|0,n,f,a+(i<<1<<2)|0),u=A+(i<<2)|0,l=a+(i<<2)|0;break}u?(t[a+(i<<2)>>2]=0,t[a+(i<<2)+4>>2]=0,t[a+(i<<2)+8>>2]=0,t[a+(i<<2)+12>>2]=0,t[a+(i<<2)+16>>2]=0,t[a+(i<<2)+20>>2]=0,t[a+(i<<2)+24>>2]=0,t[a+(i<<2)+28>>2]=0):yf(a+(i<<2)|0,a,a+(((0|i)/2|0)<<2)|0),yf(A,e,r),yf(A+(i<<2)|0,B,o),u=A+(i<<2)|0,l=a+(i<<2)|0}while(0);if(u=0|hn(a,A,u,i),u=c?u-(0|Un(l,a,l,i))|0:(0|hn(l,l,a,i))+u|0,(l=(0|hn(A+(((0|i)/2|0)<<2)|0,A+(((0|i)/2|0)<<2)|0,l,i))+u|0)&&(A=(0|t[(u=A+(((0|i)/2|0)+i<<2)|0)>>2])+l|0,t[u>>2]=A,!(A>>>0>=l>>>0)))do{A=1+(0|t[(u=u+4|0)>>2])|0,t[u>>2]=A}while(!(0|A))}}function Tf(A,e,r,i,n){A|=0,e|=0,i|=0;var f,a=0,B=0;if(f=(0|(r|=0))<(0|(n|=0))?n:r,B=(0|r)<(0|n)?e:i,a=(0|r)<(0|n)?r:n,r=(0|r)<(0|n)?i:e,(0|a)<1)Dn(A,r,f,0);else if(t[A+(f<<2)>>2]=0|Dn(A,r,f,0|t[B>>2]),1!=(0|a))for(i=A,e=A+(f<<2)|0;;){if(t[e+4>>2]=0|gn(i+4|0,r,f,0|t[B+4>>2]),2==(0|a)){e=9;break}if(t[e+8>>2]=0|gn(i+8|0,r,f,0|t[B+8>>2]),(0|a)<4){e=9;break}if(t[e+12>>2]=0|gn(i+12|0,r,f,0|t[B+12>>2]),4==(0|a)){e=9;break}if(i=i+16|0,B=B+16|0,t[(e=e+16|0)>>2]=0|gn(i,r,f,0|t[B>>2]),(0|a)<6){e=9;break}a=a+-4|0}}function Lf(A,e,r,i,n,f,a){A|=0,e|=0,r|=0,n|=0,f|=0,a|=0;var B=0,o=0,u=0;if((0|(i|=0))<8)Tf(A,e,n+i|0,r,f+i|0);else{switch((3*(u=0|wa(e,e+(i<<2)|0,n,i-n|0))|0)+(0|wa(r+(i<<2)|0,r,f,f-i|0))|0){case-4:Jf(a,e+(i<<2)|0,e,n,n-i|0),Jf(a+(i<<2)|0,r,r+(i<<2)|0,f,i-f|0),u=0;break;case-2:case-3:Jf(a,e+(i<<2)|0,e,n,n-i|0),Jf(a+(i<<2)|0,r+(i<<2)|0,r,f,f-i|0),u=1;break;case 2:case 1:case 0:case-1:Jf(a,e,e+(i<<2)|0,n,i-n|0),Jf(a+(i<<2)|0,r,r+(i<<2)|0,f,i-f|0),u=1;break;case 4:case 3:Jf(a,e,e+(i<<2)|0,n,i-n|0),Jf(a+(i<<2)|0,r+(i<<2)|0,r,f,f-i|0),u=0;break;default:u=0}A:do{if(8==(0|i))Rf(a+(i<<1<<2)|0,a,a+32|0),Rf(A,e,r),Tf(A+(i<<1<<2)|0,e+(i<<2)|0,n,r+(i<<2)|0,f),Df(A+((i<<1)+n+f<<2)|0,0,(i<<1)-n-f<<2|0),B=A+(i<<1<<2)|0,o=a+(i<<1<<2)|0;else{if(Xf(a+(i<<1<<2)|0,a,a+(i<<2)|0,i,0,0,a+(i<<2<<2)|0),Xf(A,e,r,i,0,0,a+(i<<2<<2)|0),!(o=((0|n)>(0|f)?n:f)-(i>>>1)|0)){Xf(A+(i<<1<<2)|0,e+(i<<2)|0,r+(i<<2)|0,i>>>1,n-(i>>>1)|0,f-(i>>>1)|0,a+(i<<2<<2)|0),Df(A+((i<<1)+(-2&i)<<2)|0,0,(i<<1)-(-2&i)<<2|0),B=A+(i<<1<<2)|0,o=a+(i<<1<<2)|0;break}if(B=A+(i<<1<<2)|0,(0|o)>0){Lf(B,e+(i<<2)|0,r+(i<<2)|0,i>>>1,n-(i>>>1)|0,f-(i>>>1)|0,a+(i<<2<<2)|0),Df(A+((i<<1)+n+f<<2)|0,0,(i<<1)-n-f<<2|0),o=a+(i<<1<<2)|0;break}if(Df(0|B,0,i<<3|0),(0|n)<16&(0|f)<16){Tf(B,e+(i<<2)|0,n,r+(i<<2)|0,f),o=a+(i<<1<<2)|0;break}e:do{if(!((i>>>2|0)<(0|n)|(i>>>2|0)<(0|f))){for(o=i>>>2;!((0|o)==(0|n)|(0|o)==(0|f));)if((0|(o=(0|o)/2|0))<(0|n)|(0|o)<(0|f))break e;Xf(B,e+(i<<2)|0,r+(i<<2)|0,o,n-o|0,f-o|0,a+(i<<2<<2)|0),o=a+(i<<1<<2)|0;break A}o=i>>>2}while(0);Lf(B,e+(i<<2)|0,r+(i<<2)|0,o,n-o|0,f-o|0,a+(i<<2<<2)|0),o=a+(i<<1<<2)|0}}while(0);if(B=0|hn(a,A,B,i<<1),B=u?B-(0|Un(o,a,o,i<<1))|0:(0|hn(o,o,a,i<<1))+B|0,(B=(0|hn(A+(i<<2)|0,A+(i<<2)|0,o,i<<1))+B|0)&&(a=(0|t[A+(3*i<<2)>>2])+B|0,t[A+(3*i<<2)>>2]=a,!(a>>>0>=B>>>0))){B=A+(3*i<<2)|0;do{A=1+(0|t[(B=B+4|0)>>2])|0,t[B>>2]=A}while(!(0|A))}}}function Sf(A,e,r,i){return i=0|pf(A|=0,e|=0,r|=0,i|=0),aa(A),0|i}function pf(A,e,r,i){A|=0,r|=0,i|=0;var n,f,a=0,B=0,o=0;if(0==(0|(n=0|t[(e|=0)+4>>2]))|0==(0|(f=0|t[r+4>>2])))return ba(A),0|(i=1);iB(i),(0|A)==(0|e)|(0|A)==(0|r)?(a=0|tB(i))?B=5:a=0:(a=A,B=5);A:do{if(5==(0|B)){do{if(8==(0|n)&0==(n-f|0)){if(!(0|ia(a,16))){a=0;break A}t[a+4>>2]=16,Rf(0|t[a>>2],0|t[e>>2],0|t[r>>2])}else{if(!((0|n)>15&(0|f)>15&(n-f+1|0)>>>0<3)){if(!(0|ia(a,f+n|0))){a=0;break A}t[a+4>>2]=f+n,Tf(0|t[a>>2],0|t[e>>2],n,0|t[r>>2],f);break}if(o=1<<(0|zf((n-f|0)>-1?n:f))-1,!(B=0|tB(i))){a=0;break A}if((0|n)>(0|o)|(0|f)>(0|o)){if(!(0|ia(B,o<<3))){a=0;break A}if(!(0|ia(a,o<<3))){a=0;break A}Lf(0|t[a>>2],0|t[e>>2],0|t[r>>2],o,n-o|0,f-o|0,0|t[B>>2])}else{if(!(0|ia(B,o<<2))){a=0;break A}if(!(0|ia(a,o<<2))){a=0;break A}Xf(0|t[a>>2],0|t[e>>2],0|t[r>>2],o,n-o|0,f-o|0,0|t[B>>2])}t[a+4>>2]=f+n}}while(0);if(t[a+12>>2]=t[r+12>>2]^t[e+12>>2],(0|a)!=(0|A)&&0==(0|ra(A,a))){a=0;break}a=1}}while(0);return nB(i),0|(i=a)}function zf(A){var e,r,i;return((0-((((e=((r=((i=((A|=0)>>>16^A)&0-(A>>>16)>>31^A)>>>8^i)&0-(i>>>8)>>31^i)>>>4^r)&0-(r>>>4)>>31^r)>>>2^e)&0-(e>>>2)>>31^e)>>>1)|0)>>>31)+(0-(A>>>16)>>31&16|0!=(0|A)|0-(i>>>8)>>31&8|0-(r>>>4)>>31&4|0-(e>>>2)>>31&2)|0}function Kf(A){var e=0,r=0,i=0,n=0,f=0,a=0,B=0,o=0,u=0,l=0;if(f=0|t[(A|=0)+4>>2],!(4&t[A+16>>2]))return f?0|(f=(0-((e=0|t[(0|t[A>>2])+(f+-1<<2)>>2])>>>16)>>31&16|0!=(0|e)|0-((r=(e>>>16^e)&0-(e>>>16)>>31^e)>>>8)>>31&8|0-((i=(r>>>8^r)&0-(r>>>8)>>31^r)>>>4)>>31&4|0-((n=(i>>>4^i)&0-(i>>>4)>>31^i)>>>2)>>31&2|f+-1<<5)+((0-(((n>>>2^n)&0-(n>>>2)>>31^n)>>>1)|0)>>>31)|0):0|(f=0);if((0|(n=0|t[A+8>>2]))>0){e=0|t[A>>2],r=0,i=0,A=0;do{A=(32&(i|=a=((r^f+-1)-1&f+2147483647)>>31)^32)+A+(((0-((((B=((o=((u=((l=0|t[e+(r<<2)>>2])>>>16^l)&0-(l>>>16)>>31^l)>>>8^u)&0-(u>>>8)>>31^u)>>>4^o)&0-(o>>>4)>>31^o)>>>2^B)&0-(B>>>2)>>31^B)>>>1)|0)>>>31)+(0-(l>>>16)>>31&16|0!=(0|l)|0-(u>>>8)>>31&8|0-(o>>>4)>>31&4|0-(B>>>2)>>31&2)&a)|0,r=r+1|0}while((0|r)!=(0|n))}else A=0;return 0|(l=A&~((-2147483648-f&(-2147483648^f))>>31))}function jf(A){return 0==(0|t[(A|=0)+4>>2])|0}function xf(A){var e=0,r=0;if(A|=0){e=0|t[A>>2];do{if(0!=(0|e)&&($r(e,t[A+8>>2]<<2),0==(2&(r=0|t[A+16>>2])|0))){if(e=0|t[A>>2],8&r){ei(e,82796,0),e=A+16|0;break}Xn(e),e=A+16|0;break}e=A+16|0}while(0);r=1&t[e>>2],$r(A,20),r&&Xn(A)}}function Of(A,e){return e|=0,t[(A|=0)+16>>2]&e|0}function Pf(A){var e,r=0;if(A|=0){e=0|t[A+16>>2];do{if(!(2&e)){if(r=0|t[A>>2],8&e){ei(r,82796,0);break}Xn(r);break}}while(0);return 1&t[A+16>>2]?void Xn(A):void(t[A>>2]=0)}}function _f(A){t[(A|=0)>>2]=0,t[A+4>>2]=0,t[A+8>>2]=0,t[A+12>>2]=0,t[A+16>>2]=0}function qf(){var A=0;return(A=0|Sn(20,82796))?(t[A+16>>2]=1,0|A):(Xe(3,113,65,0,0),0|(A=0))}function $f(){var A=0;return(A=0|Sn(20,82796))?(t[A+16>>2]=9,0|A):(Xe(3,113,65,0,0),0|(A=0))}function Aa(A,e){e|=0;var r,i=0,n=0,f=0,a=0,B=0,o=0,u=0,l=0,c=0;if((0|t[(A|=0)+8>>2])>=(0|e))return 0|(o=A);if((0|e)>16777215)return Xe(3,120,114,0,0),0|(o=0);if(2&(i=0|t[A+16>>2])|0)return Xe(3,120,105,0,0),0|(o=0);if(!(B=8&i?0|function(A,e,r){return 0|(A=0|Sn(A|=0,e|=0,r|=0))}(e<<2,82796,0):0|Sn(e<<2,82796)))return Xe(3,120,65,0,0),0|(o=0);r=0|t[A>>2];do{if(r){if((0|(i=0|t[A+4>>2]))>3){for(a=(0==(i>>>2|0)?i>>>2^1073741823:1073741822)+(i>>>2)<<2,n=B,f=r,i>>>=2;c=0|t[f+4>>2],l=0|t[f+8>>2],u=0|t[f+12>>2],t[n>>2]=t[f>>2],t[n+4>>2]=c,t[n+8>>2]=l,t[n+12>>2]=u,n=n+16|0,!((0|i)<=1);)f=f+16|0,i=i+-1|0;f=0|t[A+4>>2],i=r+(a+8<<2)|0}else f=i,n=B,i=r;switch(3&f){case 3:t[n+8>>2]=t[i+8>>2],o=18;break;case 2:o=18;break;case 1:o=19}if(18==(0|o)&&(t[n+4>>2]=t[i+4>>2],o=19),19==(0|o)&&(t[n>>2]=t[i>>2]),r){if($r(r,t[A+8>>2]<<2),i=0|t[A>>2],8&t[A+16>>2]){ei(i,82796,0),i=A;break}Xn(i),i=A;break}o=22}else o=22}while(0);return 22==(0|o)&&(i=A),t[i>>2]=B,t[A+8>>2]=e,0|(c=A)}function ea(A){var e=0,r=0,i=0,n=0,f=0,a=0,B=0,o=0,u=0;if(!(A|=0))return 0|(a=0);if(i=0==(8&t[A+16>>2]|0),a=0|Sn(20,82796),i){if(!a)return Xe(3,113,65,0,0),0|(a=0);e=1}else{if(!a)return Xe(3,113,65,0,0),0|(a=0);e=9}if(t[a+16>>2]=e,(0|a)==(0|A))return 0|(a=A);e=0|t[A+4>>2];do{if((0|t[a+8>>2])<(0|e)){if(0|Aa(a,e)){e=0|t[A+4>>2];break}e=0|t[a+16>>2];do{if(!(2&e)){if(r=0|t[a>>2],8&e){ei(r,82796,0);break}Xn(r);break}}while(0);return 1&t[a+16>>2]?(Xn(a),0|(a=0)):(t[a>>2]=0,0|(a=0))}}while(0);if(i=0|t[a>>2],r=0|t[A>>2],(0|e)>3){for(e>>>=2;u=0|t[r+4>>2],o=0|t[r+8>>2],B=0|t[r+12>>2],t[i>>2]=t[r>>2],t[i+4>>2]=u,t[i+8>>2]=o,t[i+12>>2]=B,i=i+16|0,r=r+16|0,!((0|e)<=1);)e=e+-1|0;e=0|t[A+4>>2]}switch(3&e){case 3:t[i+8>>2]=t[r+8>>2],f=17;break;case 2:f=17;break;case 1:f=18;break;case 0:n=e}return 17==(0|f)&&(t[i+4>>2]=t[r+4>>2],f=18),18==(0|f)&&(t[i>>2]=t[r>>2],n=0|t[A+4>>2]),t[a+12>>2]=t[A+12>>2],t[a+4>>2]=n,0|(u=a)}function ra(A,e){var r=0,i=0,n=0,f=0,a=0,B=0,o=0,u=0;if((0|(A|=0))==(0|(e|=0)))return 0|(a=A);r=0|t[e+4>>2];do{if((0|t[A+8>>2])<(0|r)){if(0|Aa(A,r)){r=0|t[e+4>>2];break}return 0|(a=0)}}while(0);if(n=0|t[A>>2],i=0|t[e>>2],(0|r)>3){for(r>>>=2;u=0|t[i+4>>2],o=0|t[i+8>>2],B=0|t[i+12>>2],t[n>>2]=t[i>>2],t[n+4>>2]=u,t[n+8>>2]=o,t[n+12>>2]=B,n=n+16|0,i=i+16|0,!((0|r)<=1);)r=r+-1|0;r=0|t[e+4>>2]}switch(3&r){case 3:t[n+8>>2]=t[i+8>>2],a=11;break;case 2:a=11;break;case 1:a=12;break;case 0:f=r}return 11==(0|a)&&(t[n+4>>2]=t[i+4>>2],a=12),12==(0|a)&&(t[n>>2]=t[i>>2],f=0|t[e+4>>2]),t[A+12>>2]=t[e+12>>2],t[A+4>>2]=f,0|(u=A)}function ia(A,e){return e|=0,(0|t[(A|=0)+8>>2])>=(0|e)?0|(e=A):0|(e=0|Aa(A,e))}function na(A){var e=0;return 0|(e=(0|(e=0|t[(A|=0)+4>>2]))<=1?1==(0|e)?0|t[t[A>>2]>>2]:0:-1)}function ta(A,e){return e|=0,(0|t[(A|=0)+8>>2])<=0&&0==(0|Aa(A,1))?0|(e=0):(t[A+12>>2]=0,t[t[A>>2]>>2]=e,t[A+4>>2]=0!=(0|e)&1,0|(e=1))}function fa(A,e,r){A|=0,e|=0,r|=0;var n,a,B=0,o=0,u=0,l=0,c=0;do{if(!r){if(0|(r=0|Sn(20,82796))){t[r+16>>2]=1,B=r,o=r,c=r;break}return Xe(3,113,65,0,0),0|(c=0)}B=0,o=0,c=r}while(0);A:do{if((0|e)>0)for(;;){if(0|i[A>>0]){r=e;break A}if(A=A+1|0,r=e+-1|0,!((0|e)>1)){l=9;break}e=r}else r=e,l=9}while(0);if(9==(0|l)&&!r)return t[c+4>>2]=0,0|c;if(e=r+-1|0,(0|t[c+8>>2])<=(e>>>2|0)&&0==(0|Aa(c,1+(e>>>2)|0))){if(!o)return 0|(c=0);r=0|t[(e=o+16|0)>>2];do{if(!(2&r)){if(A=0|t[o>>2],8&r){ei(A,82796,0);break}Xn(A);break}}while(0);return 1&t[e>>2]?(Xn(B),0|(c=0)):(t[o>>2]=0,0|(c=0))}if(t[(n=c+4|0)>>2]=1+(e>>>2),t[(a=c+12|0)>>2]=0,r){for(u=1+(e>>>2)|0,B=0,o=3&e;r=r+-1|0,B=B<<8|f[A>>0],e=u+-1|0,o?(e=u,o=o+-1|0):(t[(0|t[c>>2])+(e<<2)>>2]=B,B=0,o=3),r;)u=e,A=A+1|0;(0|(r=0|t[n>>2]))>0&&(l=27)}else r=1+(e>>>2)|0,l=27;if(27==(0|l)){for(e=(0|t[c>>2])+(r<<2)|0;!(0|t[(e=e+-4|0)>>2]);){if(A=r+-1|0,(0|r)<=1){r=A;break}r=A}t[n>>2]=r}return 0|r||(t[a>>2]=0),0|c}function aa(A){var e=0,r=0,i=0;if((0|(e=0|t[(A|=0)+4>>2]))>0){for(i=(0|t[A>>2])+(e<<2)|0;!(0|t[(i=i+-4|0)>>2]);){if(r=e+-1|0,(0|e)<=1){e=r;break}e=r}t[A+4>>2]=e}0|e||(t[A+12>>2]=0)}function Ba(A,e,r){return A|=0,e|=0,0|(A=(0|(r|=0))<0?-1:0|oa(A,e,r,0))}function oa(A,e,r,n){e|=0,r|=0,n|=0;var f,a,B,u=0,l=0,c=0;if(B=G,G=G+32|0,u=(7+(0|Kf(A|=0))|0)/8|0,-1!=(0|r))if((0|u)>(0|r)){if(t[B>>2]=t[A>>2],t[B+4>>2]=t[A+4>>2],t[B+8>>2]=t[A+8>>2],t[B+12>>2]=t[A+12>>2],t[B+16>>2]=t[A+16>>2],(0|(u=0|t[B+4>>2]))>0){for(c=(0|t[B>>2])+(u<<2)|0;!(0|t[(c=c+-4|0)>>2]);){if(l=u+-1|0,(0|u)<=1){u=l;break}u=l}t[B+4>>2]=u}if(u||(t[B+12>>2]=0),(0|(7+(0|Kf(B))|0)/8)>(0|r))return G=B,0|(A=-1);u=r}else u=r;if(!(l=0|t[A+8>>2]))return $r(e,u),G=B,0|(A=u);if(a=t[A+4>>2]<<2,!u)return G=B,0|(A=0);if(f=1|o(l,-4),n){for(c=0,r=0,l=e;i[l>>0]=(0|t[(0|t[A>>2])+(c>>>2<<2)>>2])>>>(c<<3&24)&r-a>>31,(0|(r=r+1|0))!=(0|u);)c=((f+c|0)>>>31)+c|0,l=l+1|0;return G=B,0|u}for(c=0,r=0,l=e+u|0;i[(l=l+-1|0)>>0]=(0|t[(0|t[A>>2])+(c>>>2<<2)>>2])>>>(c<<3&24)&r-a>>31,(0|(r=r+1|0))!=(0|u);)c=((f+c|0)>>>31)+c|0;return G=B,0|u}function ua(A,e){e|=0;var r,n,f=0,a=0,B=0,u=0;if(n=0|Kf(A|=0),!(f=0|t[A+8>>2]))return $r(e,(n+7|0)/8|0),0|(u=(n+7|0)/8|0);if(u=t[A+4>>2]<<2,(n+14|0)>>>0<15)return 0|(u=0);for(r=1|o(f,-4),a=0,B=0,f=e+((n+7|0)/8|0)|0;;){if(i[(f=f+-1|0)>>0]=(0|t[(0|t[A>>2])+(a>>>2<<2)>>2])>>>(a<<3&24)&B-u>>31,(0|(B=B+1|0))==(0|(n+7|0)/8)){f=(n+7|0)/8|0;break}a=((a+r|0)>>>31)+a|0}return 0|f}function la(A,e){e|=0;var r,i,n=0,f=0,a=0;if(0|(n=(f=0|t[(A|=0)+4>>2])-(0|t[e+4>>2])|0))return 0|(a=n);if(i=0|t[A>>2],r=0|t[e>>2],(0|f)<=0)return 0|(a=0);for(;e=f,(0|(A=0|t[i+((f=f+-1|0)<<2)>>2]))==(0|(n=0|t[r+(f<<2)>>2]));)if((0|e)<=1){A=0,a=7;break}return 7==(0|a)?0|A:0|(a=A>>>0>n>>>0?1:-1)}function ca(A,e){var r,i,n=0,f=0,a=0,B=0;if(0==(0|(A|=0))|0==(0|(e|=0)))return 0|(0==(0|A)?1&(0==(0|e)^1):-1);if(n=0==(0|(f=0|t[A+12>>2]))?1:-1,(0|f)!=(0|t[e+12>>2]))return 0|(B=n);if(B=0==(0|f)?-1:1,(0|(a=0|t[A+4>>2]))>(0|(f=0|t[e+4>>2])))return 0|(B=n);if((0|a)<(0|f))return 0|B;if((0|a)<=0)return 0|(B=0);for(i=0|t[A>>2],r=0|t[e>>2];;){if(e=a,(f=0|t[i+((a=a+-1|0)<<2)>>2])>>>0>(A=0|t[r+(a<<2)>>2])>>>0){f=11;break}if(f>>>0<A>>>0){n=B,f=11;break}if((0|e)<=1){n=0,f=11;break}}return 11==(0|f)?0|n:0}function Ea(A,e){var r=0;if((0|(e|=0))<0)return 0|(e=0);if((0|(r=0|t[(A|=0)+4>>2]))<=(e>>>5|0)){do{if((0|t[A+8>>2])<=(e>>>5|0)){if(0|Aa(A,1+(e>>>5)|0)){r=0|t[A+4>>2];break}return 0|(e=0)}}while(0);(0|r)<=(e>>>5|0)&&Df((0|t[A>>2])+(r<<2)|0,0,1+(e>>>5)-r<<2|0),t[A+4>>2]=1+(e>>>5)}return A=(0|t[A>>2])+(e>>>5<<2)|0,t[A>>2]=t[A>>2]|1<<(31&e),0|(e=1)}function Qa(A,e){return(0|(e|=0))<0||(0|t[(A|=0)+4>>2])<=(e>>>5|0)?0|(e=0):0|(e=(0|t[(0|t[A>>2])+(e>>>5<<2)>>2])>>>(31&e)&1)}function wa(A,e,r,i){A|=0,e|=0,r|=0,i|=0;var n=0,f=0;A:do{if((0|i)<0){for(n=i;;){if(0|t[e+(r+-1-n<<2)>>2]){i=-1;break}if(!((0|n)<-1))break A;n=n+1|0}return 0|i}}while(0);A:do{if((0|i)>0){for(;;){if(0|t[A+(i+(r+-1)<<2)>>2]){i=1;break}if(!((0|i)>1))break A;i=i+-1|0}return 0|i}}while(0);if(!r)return 0|(f=0);if((0|(i=0|t[A+(r+-1<<2)>>2]))!=(0|(n=0|t[e+(r+-1<<2)>>2])))return 0|(f=i>>>0>n>>>0?1:-1);if((0|r)<=1)return 0|(f=0);for(r=r+-2|0;(0|(i=0|t[A+(r<<2)>>2]))==(0|(n=0|t[e+(r<<2)>>2]));){if(!((0|r)>0)){i=0,f=17;break}r=r+-1|0}return 17==(0|f)?0|i:0|(f=i>>>0>n>>>0?1:-1)}function ba(A){t[(A|=0)+12>>2]=0,t[A+4>>2]=0}function ka(A,e){e|=0;var r;return 1==(0|(r=0|t[(A|=0)+4>>2]))&&(0|t[t[A>>2]>>2])==(0|e)?1:0==(r|e|0)|0}function sa(A){return 1!=(0|t[(A|=0)+4>>2])||1!=(0|t[t[A>>2]>>2])?0|(A=0):0|(A=0==(0|t[A+12>>2])&1)}function Ca(A,e){e|=0;var r;if(1==(0|(r=0|t[(A|=0)+4>>2]))){if((0|t[t[A>>2]>>2])!=(0|e))return 0|(e=0)}else if(r|e|0)return 0|(e=0);return e?0|(e=0==(0|t[A+12>>2])&1):0|(e=1)}function da(A){return(0|t[(A|=0)+4>>2])<=0?0|(A=0):0|(A=1&t[t[A>>2]>>2])}function Ia(A){return 0!=(0|t[(A|=0)+12>>2])|0}function ga(A,e,r,i){return 0|Va(A|=0,e|=0,(r|=0)+4|0,r,i|=0)}function Da(A,e,r){e|=0,r|=0,t[(A|=0)>>2]=t[e>>2],t[A+4>>2]=t[e+4>>2],t[A+8>>2]=t[e+8>>2],t[A+12>>2]=t[e+12>>2],t[A+16>>2]=1&t[A+16>>2]|r|-4&t[e+16>>2]|2}function Ma(){var A=0;return(A=0|Ln(12))?0|A:(Xe(3,143,65,0,0),0|(A=0))}function ha(A,e){e|=0,t[(A|=0)+16>>2]=t[A+16>>2]|e}function Ua(A){var e,r=0;e=(r=A|=0)+52|0;do{t[r>>2]=0,r=r+4|0}while((0|r)<(0|e));_f(A),_f(A+20|0)}function Ga(A,e,r){return 0,0|ra(A|=0,e|=0)?(ba(A+20|0),t[A+40>>2]=0|Kf(e),t[A+44>>2]=0,0|(A=1)):0|(A=0)}function Za(A,e,r,i,n){A|=0,e|=0,r|=0,i|=0;var t=0;iB(n|=0),t=0|tB(n);A:do{if(t){do{if(r){if((0|e)==(0|r)){if(0|fB(t,r,n))break;t=0;break A}if(0|Sf(t,e,r,n))break;t=0;break A}t=e}while(0);t=0|Na(0,A,t,i,n)}else t=0}while(0);return nB(n),0|t}function Na(A,e,r,i,n){A|=0,e|=0,r|=0,i|=0;var f,a,B=0,o=0,u=0;iB(n|=0),f=0|tB(n),a=0|tB(n),A||(A=0|tB(n)),o=e||0|tB(n);A:do{if(0==(0|f)|0==(0|a)|0==(0|A)|0==(0|o))A=0;else{if((0|la(r,i))<0)return ba(A),i=0!=(0|ra(o,r)),nB(n),0|(n=1&i);if(B=0|Kf(r),(0|(B=(0|(e=t[i+40>>2]<<1))>(0|B)?e:B))==(0|t[i+44>>2])?e=B:(iB(n),e=0!=(0|(e=0|tB(n)))&&0!=(0|Ea(e,B))?(e=0==(0|wB(i+20|0,0,e,i,n)))?-1:B:-1,nB(n),t[i+44>>2]=e),-1!=(0|e)&&0!=(0|nn(f,r,0|t[i+40>>2]))&&0!=(0|Sf(a,f,i+20|0,n))&&0!=(0|nn(A,a,B-(0|t[i+40>>2])|0))&&(t[(u=A+12|0)>>2]=0,0!=(0|Sf(a,i,A,n)))&&0!=(0|un(o,r,a))){t[(B=o+12|0)>>2]=0;e:do{if((0|la(o,i))>-1){for(e=0;!(e>>>0>2);){if(!(0|un(o,o,i))){A=0;break A}if(!(0|cB(A,1))){A=0;break A}if(!((0|la(o,i))>-1))break e;e=e+1|0}Xe(3,130,101,0,0),A=0;break A}}while(0);A=0|jf(o)?0:0|t[r+12>>2],t[B>>2]=A,t[u>>2]=t[i+12>>2]^t[r+12>>2],A=1}else A=0}}while(0);return nB(n),0|(n=A)}function Fa(A,e,r,i){return 0|Wa(0,A|=0,e|=0,r|=0,i|=0)}function Wa(A,e,r,n,t){A|=0,e|=0,n|=0,t|=0;var a,B=0,o=0,u=0;if(a=G,G=G+16|0,r|=0){if((0|r)>=0&&!(1==(0|r)&(0|n)>0)){u=0|Ln((r+7|0)/8|0);A:do{if(u)if(U(0|a),Zt(0,0,0),(0|Nt(u,(r+7|0)/8|0))<1)A=0;else{do{if(2==(0|A)){for(B=0;;){if((0|Nt(a+4|0,1))<1){o=19;break}if(0!=(0|B)&(A=0|i[a+4>>0])<<24>>24<0?(A=0|i[u+(B+-1)>>0],o=17):(255&A)>=42?(255&A)<84&&(A=-1,o=17):(A=0,o=17),17==(0|o)&&(o=0,i[u+B>>0]=A),(0|(B=B+1|0))>=(0|(r+7|0)/8)){o=11;break}}if(11==(0|o))break;if(19==(0|o)){A=0;break A}}}while(0);do{if((0|n)>-1){if(!n){A=255&(1<<((r+-1|0)%8|0)|0|f[u>>0]),i[u>>0]=A;break}if((r+-1|0)%8|0){A=255&(3<<((r+-1|0)%8|0)-1|0|f[u>>0]),i[u>>0]=A;break}i[u>>0]=1,i[u+1>>0]=-128|i[u+1>>0],A=1;break}A=0|i[u>>0]}while(0);i[u>>0]=255&A&(255<<1+((r+-1|0)%8|0)^255),0|t&&(i[u+(((r+7|0)/8|0)-1)>>0]=1|i[u+(((r+7|0)/8|0)-1)>>0]),A=0!=(0|fa(u,(r+7|0)/8|0,e))&1}else Xe(3,127,65,0,0),A=0}while(0);return pn(u,(r+7|0)/8|0),G=a,0|(u=A)}}else if(!(-1!=(0|n)|0!=(0|t)))return ba(e),G=a,0|(u=1);return Xe(3,127,118,0,0),G=a,0|(u=0)}function Ya(A,e){return 0|Ra(0,A|=0,e|=0)}function Ra(A,e,r){e|=0;var i,n=0,f=0;if(i=0==(0|(A|=0))?16:17,0==(0|t[(r|=0)+12>>2])&&0==(0|jf(r))){if(1==(0|(n=0|Kf(r))))return ba(e),0|(f=1);if(0==(0|Qa(r,n+-2|0))&&0==(0|Qa(r,n+-3|0))){for(A=100;;){if(!(0|MB[31&i](e,n+1|0,-1,0))){f=0,A=22;break}if((0|ca(e,r))>-1){if(!(0|cn(e,e,r))){f=0,A=22;break}if((0|ca(e,r))>-1&&0==(0|cn(e,e,r))){f=0,A=22;break}}if(!(A=A+-1|0)){A=16;break}if((0|ca(e,r))<=-1){f=1,A=22;break}}if(16==(0|A))return Xe(3,122,113,0,0),0|(f=0);if(22==(0|A))return 0|f}for(A=100;;){if(!(0|MB[31&i](e,n,-1,0))){f=0,A=22;break}if(!(A=A+-1|0)){A=20;break}if((0|ca(e,r))<=-1){f=1,A=22;break}}if(20==(0|A))return Xe(3,122,113,0,0),0|(f=0);if(22==(0|A))return 0|f}return Xe(3,122,115,0,0),0|(f=0)}function ya(A,e){return 0|Ra(1,A|=0,e|=0)}function Va(A,e,r,i,n){return i=0|ma(A|=0,e|=0,r|=0,i|=0,n|=0),aa(A),0|i}function ma(A,e,r,i,n){A|=0,e|=0,i|=0,n|=0;var f;if(((0|t[(r|=0)+4>>2])+(0|t[e+4>>2])|0)>(t[i+28>>2]<<1|0))return 0|(n=0);iB(n),f=0|tB(n);do{if(f){if((0|e)==(0|r)){if(!(0|aB(f,e,n))){e=0;break}}else if(!(0|pf(f,e,r,n))){e=0;break}e=0!=(0|Ha(A,f,i))&1}else e=0}while(0);return nB(n),0|(n=e)}function Ha(A,e,r){A|=0,e|=0;var i,n=0,f=0,a=0,B=0,u=0,l=0,c=0;if(!(u=0|t[(r|=0)+28>>2]))return t[A+4>>2]=0,0|(u=1);if(!(0|ia(e,u<<1)))return 0|(u=0);if(t[e+12>>2]=t[e+12>>2]^t[r+36>>2],i=0|t[r+24>>2],B=0|t[e>>2],n=0|t[e+4>>2],(0|u)>0){f=0;do{t[(a=B+(f<<2)|0)>>2]=t[a>>2]&f-n>>31,f=f+1|0}while((0|f)<(u<<1|0));if(t[e+4>>2]=u<<1,r=0|t[r+64>>2],(0|u)>0)for(n=0,a=0,f=B;n=(n|0!=(0|(c=(0|gn(f,i,u,0|o(0|t[f>>2],r)))+n|0)))&(c+(l=0|t[(B=f+(u<<2)|0)>>2])|0)>>>0<=l>>>0,t[B>>2]=c+l,(0|(a=a+1|0))!=(0|u);)f=f+4|0;else n=0}else t[e+4>>2]=u<<1,n=0;if(!(0|ia(A,u)))return 0|(c=0);if(t[A+4>>2]=u,t[A+12>>2]=t[e+12>>2],f=n-(0|Un(a=0|t[A>>2],r=(0|t[e>>2])+(u<<2)|0,i,u))|0,(0|u)<=0)return 0|(c=1);n=0;do{c=r+(n<<2)|0,t[(l=a+(n<<2)|0)>>2]=t[l>>2]&~f|t[c>>2]&f,t[c>>2]=0,n=n+1|0}while((0|n)!=(0|u));return 0|(n=1)}function va(A,e,r,i){A|=0,e|=0,r|=0;var n;return iB(i|=0),e=0!=(0|(n=0|tB(i)))&&0!=(0|ra(n,e))?0|Ha(A,n,r):0,nB(i),aa(A),0|e}function Ja(A,e,r,i){A|=0,e|=0,r|=0;var n;return iB(i|=0),e=0!=(0|(n=0|tB(i)))&&0!=(0|ra(n,e))?0|Ha(A,n,r):0,nB(i),0|e}function Xa(A,e,r,i){return 0|ma(A|=0,e|=0,(r|=0)+4|0,r,i|=0)}function Ta(){var A=0;return(A=0|Ln(76))?(t[A>>2]=0,_f(A+4|0),_f(A+24|0),_f(A+44|0),t[A+68>>2]=0,t[A+64>>2]=0,t[A+72>>2]=1,0|A):0|(A=0)}function La(A){(A|=0)&&(xf(A+4|0),xf(A+24|0),xf(A+44|0),1&t[A+72>>2]&&Xn(A))}function Sa(A,e,r){A|=0,r|=0;var i,n=0;if(i=G,G=G+32|0,0|jf(e|=0))return G=i,0|(r=0);iB(r),n=0|tB(r);A:do{if(0!=(0|n)&&0!=(0|ra(A+24|0,e))){0|Of(e,4)&&ha(A+24|0,4),t[A+36>>2]=0,_f(i+8|0),t[i+8>>2]=i,t[i+8+8>>2]=2,t[i+8+12>>2]=0,0|Of(e,4)&&ha(i+8|0,4),t[A>>2]=((31+(0|Kf(e))|0)/32|0)<<5,ba(A+4|0);do{if(0|Ea(A+4|0,32)){if(e=0|t[t[e>>2]>>2],t[i>>2]=e,t[i+4>>2]=0,t[i+8+4>>2]=0!=(0|e)&1,0|sa(i+8|0))ba(n);else if(!(0|Hf(n,A+4|0,i+8|0,r)))break;if(0|rn(n,n,32)){if(0|jf(n)){if(!(0|ta(n,-1)))break}else if(!(0|EB(n,1)))break;if(0|wB(n,0,n,i+8|0,r)){if(n=(0|t[n+4>>2])>0?0|t[t[n>>2]>>2]:0,t[A+64>>2]=n,t[A+68>>2]=0,ba(A+4|0),!(0|Ea(A+4|0,t[A>>2]<<1))){n=0;break A}if(!(0|wB(0,A+4|0,A+4|0,A+24|0,r))){n=0;break A}n=0|t[A+8>>2],(0|(e=0|t[A+28>>2]))>(0|n)&&Df((0|t[A+4>>2])+(n<<2)|0,0,e-n<<2|0),t[A+8>>2]=e,n=1;break A}}}}while(0);n=0}else n=0}while(0);return nB(r),G=i,0|(r=n)}function pa(A,e,r,i){A|=0,r|=0,i|=0;var n=0;return Y(e|=0),n=0|t[A>>2],y(e),0|n?0|(e=n):(n=0|Ln(76))?(t[n>>2]=0,_f(n+4|0),_f(n+24|0),_f(n+44|0),t[n+68>>2]=0,t[n+64>>2]=0,t[n+72>>2]=1,0|Sa(n,r,i)?(R(e),0|t[A>>2]?(xf(n+4|0),xf(n+24|0),xf(n+44|0),1&t[n+72>>2]|0&&Xn(n),n=0|t[A>>2]):t[A>>2]=n,y(e),0|(e=n)):(xf(n+4|0),xf(n+24|0),xf(n+44|0),1&t[n+72>>2]?(Xn(n),0|(e=0)):0|(e=0))):0|(e=0)}function za(A,e,r){A|=0,e|=0,r|=0;var i=0;A:do{if(A){switch(0|t[A>>2]){case 1:break;case 2:i=0|DB[31&t[A+8>>2]](e,r,A);break A;default:i=0;break A}(i=0|t[A+8>>2])?(WB[1&i](e,r,0|t[A+4>>2]),i=1):i=1}else i=1}while(0);return 0|i}function Ka(A,e,r,i,f,B){A|=0,r|=0,i|=0,f|=0,B|=0;var o,u=0,l=0,c=0,E=0,Q=0,w=0,b=0,k=0,s=0,C=0;if((0|(e|=0))<=3746)if((0|e)<=1344)if((0|e)<=475)if((0|e)<=399)if((0|e)<=346)if(u=(0|e)>54?27:34,(0|e)<=307){if((0|e)<2)return Xe(3,141,118,0,0),0|(C=0);if(2==(0|e)&0!=(0|r))return Xe(3,141,118,0,0),0|(C=0);s=0!=(0|r)}else u=8,C=7;else u=7,C=7;else u=6,C=7;else u=5,C=7;else u=4,C=7;else u=3,C=7;if(7==(0|C)&&(s=0!=(0|r)),0!=(0|(o=0|Sn(4096,82796)))&&0!=(0|(l=0|eB()))){if(iB(l),k=0|tB(l)){b=0;A:for(;;){e:do{if(i){if(!s){if(0|ja(A,e,i,f,l))break;C=90;break A}if(iB(l),r=0|tB(l),Q=0|tB(l),!(w=0|tB(l))){C=64;break A}if(!(0|en(w,i))){C=64;break A}if(!(0|Fa(Q,e+-1|0,0,1))){C=64;break A}if(!(0|wB(0,r,Q,w,l))){C=64;break A}if(!(0|cn(Q,Q,r))){C=64;break A}if(f){if(!(0|en(r,f))){C=64;break A}if(!(0|on(Q,Q,r))){C=64;break A}}else if(!(0|cB(Q,1))){C=64;break A}if(!(0|An(A,Q))){C=64;break A}if(!(0|cB(A,1))){C=64;break A}for(r=1;;){if(-1==(0|(c=0|uB(A,E=0|a[46480+(r<<1)>>1])))|-1==(0|(E=0|uB(Q,E)))){C=64;break A}if(r=r+1|0,0==(0|c)|0==(0|E)){if(!(0|on(A,A,i))){C=64;break A}if(!(0|on(Q,Q,w))){C=64;break A}r=1}else if(r>>>0>=2048)break}nB(l)}else{if(!(0|Fa(A,e,1,1))){C=90;break A}if((0|e)>=33)for(r=1;;){if(-1==(0|(c=0|uB(A,0|a[46480+(r<<1)>>1])))){u=0;break A}if(n[o+(r<<1)>>1]=c,(r=r+1|0)>>>0>=2048){r=0;r:do{for(c=1;!((((r+(0|a[o+(c<<1)>>1])|0)>>>0)%((0|a[46480+(c<<1)>>1])>>>0)|0)>>>0<2);)if((c=c+1|0)>>>0>=2048){C=43;break r}r=r+2|0}while(r>>>0<=4294949432);if(43==(0|C)){if(C=0,!(0|cB(A,r))){u=0;break A}if((0|Kf(A))==(0|e))break e}if(!(0|Fa(A,e,1,1))){u=0;break A}r=1}}for(w=-17864;;){c=1;do{if(-1==(0|(r=0|uB(A,0|a[46480+(c<<1)>>1])))){u=0;break A}n[o+(c<<1)>>1]=r,c=c+1|0}while(c>>>0<2048);r=0|na(A),w=(r=32==(0|e)?~r:(1<<e)-1-r|0)>>>0<w>>>0?r:w,r=0;r:do{for(c=0|na(A),Q=1;;){if(c>>>0<=(E=0|a[46480+(Q<<1)>>1])>>>0){C=33;break r}if(!(((r+(0|a[o+(Q<<1)>>1])|0)>>>0)%(E>>>0)|0))break;if((Q=Q+1|0)>>>0>=2048){C=33;break r}}r=r+2|0}while(r>>>0<=w>>>0);if(33==(0|C)){if(C=0,!(0|cB(A,r))){u=0;break A}if((0|Kf(A))==(0|e))break e}if(!(0|Fa(A,e,1,1))){u=0;break A}}}}while(0);E=b+1|0;e:do{if(0|B){switch(0|t[B>>2]){case 1:break;case 2:if(0|DB[31&t[B+8>>2]](0,b,B))break e;C=90;break A;default:C=90;break A}0|(r=0|t[B+8>>2])&&WB[1&r](0,b,0|t[B+4>>2])}}while(0);e:do{if(s){if(!(0|en(k,A))){C=90;break A}if(!B)for(r=0;;){switch(0|xa(A,1,l,0,0)){case-1:u=0;break A;case 0:break e}switch(0|xa(k,1,l,0,0)){case-1:u=0;break A;case 0:break e}if((r=r+1|0)>>>0>=u>>>0){u=1;break A}}for(c=0;;){switch(0|xa(A,1,l,0,B)){case-1:u=0;break A;case 0:break e}switch(0|xa(k,1,l,0,B)){case-1:u=0;break A;case 0:break e}switch(0|t[B>>2]){case 1:0|(r=0|t[B+8>>2])&&WB[1&r](2,b,0|t[B+4>>2]);break;case 2:if(!(0|DB[31&t[B+8>>2]](2,b,B))){u=0;break A}break;default:u=0;break A}if((c=c+1|0)>>>0>=u>>>0){u=1;break A}}}else switch(0|xa(A,u,l,0,B)){case-1:C=90;break A;case 0:break;default:u=1;break A}}while(0);b=E}64==(0|C)?(nB(l),u=0):90==(0|C)&&(u=0),Xn(o)}else Xn(o),u=0;nB(l)}else Xn(o),l=0,u=0;return rB(l),0|(C=u)}function ja(A,e,r,i,n){A|=0,e|=0,r|=0,i|=0;var t=0;iB(n|=0),t=0|tB(n);A:do{if(0!=(0|t)&&0!=(0|Fa(A,e,0,1))&&0!=(0|wB(0,t,A,r,n))&&0!=(0|cn(A,A,t))){if(i){if(!(0|on(A,A,i))){e=0;break}}else if(!(0|cB(A,1))){e=0;break}for(e=1;;){if(-1==(0|(t=0|uB(A,0|a[46480+(e<<1)>>1])))){e=0;break A}if(e=e+1|0,t>>>0<2){if(!(0|on(A,A,r))){e=0;break A}e=1}else if(e>>>0>=2048){e=1;break A}}}else e=0}while(0);return nB(n),0|e}function xa(A,e,r,i,n){e|=0,r|=0,i|=0,n|=0;var f=0,B=0,o=0,u=0,l=0,c=0,E=0;if((0|ca(A|=0,52964))<1)return 0|(r=0);if(E=e||((0|Kf(A))<=3746?(0|Kf(A))<=1344?(0|Kf(A))<=475?(0|Kf(A))<=399?(0|Kf(A))<=346?(0|Kf(A))>307?8:(E=(0|Kf(A))>54)?27:34:7:6:5:4:3),!(0|da(A)))return 0|(r=0|Ca(A,2));A:do{if(i){e=1;e:do{switch(7&(-1==(0|(c=0|uB(A,0|a[46480+(e<<1)>>1])))?5:0==(0|c)&1)){case 5:f=0,e=-1;break A;case 0:break;default:e=0,B=67;break e}e=e+1|0}while(e>>>0<2048);if(67==(0|B))return 0|e;if(n){switch(0|t[n>>2]){case 1:break;case 2:if(0|DB[31&t[n+8>>2]](1,-1,n)){B=21;break A}f=0,e=-1;break A;default:f=0,e=-1;break A}(e=0|t[n+8>>2])?(WB[1&e](1,-1,0|t[n+4>>2]),B=21):B=21}else B=21}else B=21}while(0);do{if(21==(0|B)){if(r)c=r;else{if(!(e=0|eB())){f=0,e=-1;break}c=e}if(iB(c),o=0|tB(c),u=0|tB(c),0!=(0|(l=0|tB(c)))&&0!=(0|ra(o,A))&&0!=(0|EB(o,1)))if(0|jf(o))f=0,e=0;else{for(e=1;!(0|Qa(o,e));)e=e+1|0;A:do{if(0!=(0|nn(u,o,e))&&(f=0|Ta(),0!=(0|f)))if(0|Sa(f,A,c))if((0|E)>0){if(!(i=e+-1|0))for(i=0;;){if(!(0|ya(l,o))){e=-1;break A}if(!(0|cB(l,1))){e=-1;break A}if(!(0|Pa(l,l,u,A,c,f))){e=-1;break A}if(0==(0|sa(l))&&0|ca(l,o)){e=0;break A}e:do{if(0|n){switch(0|t[n>>2]){case 1:break;case 2:if(0|DB[31&t[n+8>>2]](1,i,n))break e;e=-1;break A;default:e=-1;break A}if(!(e=0|t[n+8>>2]))break;WB[1&e](1,i,0|t[n+4>>2])}}while(0);if((0|(i=i+1|0))>=(0|E)){e=1;break A}}for(B=0;;){if(!(0|ya(l,o))){e=-1;break A}if(!(0|cB(l,1))){e=-1;break A}if(!(0|Pa(l,l,u,A,c,f))){e=-1;break A}e:do{if(!(0|sa(l))){if(!(0|ca(l,o)))break;for(e=i;;){if(!(0|AB(l,l,l,A,c))){e=-1;break A}if(0|sa(l)){e=0;break A}if(!(0|ca(l,o)))break e;if(!(e=e+-1|0)){e=0;break A}}}}while(0);e:do{if(0|n){switch(0|t[n>>2]){case 1:break;case 2:if(0|DB[31&t[n+8>>2]](1,B,n))break e;e=-1;break A;default:e=-1;break A}if(!(e=0|t[n+8>>2]))break;WB[1&e](1,B,0|t[n+4>>2])}}while(0);if((0|(B=B+1|0))>=(0|E)){e=1;break}}}else e=1;else e=-1;else f=0,e=-1}while(0);if(!c)break}else f=0,e=-1;nB(c),r||rB(c)}}while(0);return La(f),0|(r=e)}function Oa(A,e,r,i,n){return A|=0,e|=0,r|=0,n|=0,0|da(i|=0)?1==(0|t[e+4>>2])&&0==(0|t[e+12>>2])&&0==(0|Of(r,4))&&0==(0|Of(e,4))&&0==(0|Of(i,4))?0|(A=0|function(A,e,r,i,n,f){A|=0,e|=0,i|=0,n|=0,f|=0;var a=0,B=0,u=0,l=0,c=0,E=0,Q=0,w=0,b=0,k=0;if(0==(0|Of(r|=0,4))&&0==(0|Of(i,4))){if(!(0|da(i)))return Xe(3,117,102,0,0),0|(k=0);if(1==(0|t[i+4>>2])&&(e=(e>>>0)%((0|t[t[i>>2]>>2])>>>0)|0),!(B=0|Kf(r)))return 0|ka(i,1)?(ba(A),0|(k=1)):0|(k=0|ta(A,1));if(!e)return ba(A),0|(k=1);iB(n),w=0|tB(n),u=0|tB(n),l=0|tB(n);A:do{if(0==(0|w)|0==(0|u)|0==(0|l))a=0,e=0,k=46;else{if(f)a=f;else{if(!(a=0|Ta())){a=0,e=0,k=47;break}if(!(0|Sa(a,i,n))){e=0,k=47;break}}if((0|B)>1)for(w=B+-2|0,c=1,Q=e;;){B=0|o(Q,Q),E=0==(0|c);do{if((0|(B>>>0)/(Q>>>0))!=(0|Q)){if(E){if(!(0|QB(u,Q))){e=0,k=46;break A}if(0|wB(0,l,u,i,n)){B=1,c=l,k=27;break}e=0,k=46;break A}if(!(0|ta(u,Q))){e=0,k=46;break A}if(0|ga(u,u,a,n)){B=1,c=u,u=l,k=27;break}e=0,k=46;break A}E?(c=u,u=l,k=27):(Q=u,u=1)}while(0);if(27==(0|k)){if(k=0,!(0|Va(c,c,c,a,n))){e=0,k=46;break A}Q=c,c=0,l=u,u=0}do{if(0|Qa(r,w)){if((0|((E=0|o(B,e))>>>0)/(e>>>0))!=(0|B)){if(u){if(!(0|ta(Q,B))){e=0,k=46;break A}if(0|ga(Q,Q,a,n)){u=Q,c=0,B=e;break}e=0,k=46;break A}if(!(0|QB(Q,B))){e=0,k=46;break A}if(0|wB(0,l,Q,i,n)){u=l,l=Q,B=e;break}e=0,k=46;break A}u=Q,B=E}else u=Q}while(0);if(!((0|w)>0)){e=B;break}w=w+-1|0,Q=B}else c=1;B=0==(0|c);do{if(1!=(0|e)){if(B){if(!(0|QB(u,e))){e=0,k=46;break A}if(0|wB(0,l,u,i,n)){b=l,k=44;break}e=0,k=46;break A}if(!(0|ta(u,e))){e=0,k=46;break A}if(0|ga(u,u,a,n)){b=u,k=44;break}e=0,k=46;break A}if(B)b=u,k=44;else if(!(0|ta(A,1))){e=0,k=46;break A}}while(0);if(44==(0|k)&&0==(0|va(A,b,a,n))){e=0,k=46;break}e=1,k=46}}while(0);return 46==(0|k)&&(f||(k=47)),47==(0|k)&&La(a),nB(n),0|(k=e)}return Xe(3,117,66,0,0),0|(k=0)}(A,0|t[t[e>>2]>>2],r,i,n,0)):0|(A=0|Pa(A,e,r,i,n,0)):0|(A=0|function(A,e,r,i,n){A|=0,e|=0,i|=0,n|=0;var f,a=0,B=0,o=0,u=0,l=0,c=0;if(f=G,G=G+192|0,0==(0|Of(r=r|0,4))&&0==(0|Of(e,4))?0==(0|Of(i,4)):0){if(!(l=0|Kf(r)))return 0|ka(i,1)?(ba(A),G=f,0|(n=1)):(n=0|ta(A,1),G=f,0|n);iB(n),u=0|tB(n),a=0|tB(n),t[f>>2]=a;A:do{if(0==(0|u)|0==(0|a))e=0;else{if(Ua(f+128|0),0|t[i+12>>2]){if(!(0|ra(u,i))){e=0;break}if(t[u+12>>2]=0,(0|Ga(f+128|0,u))<1){e=0;break}}else if((0|Ga(f+128|0,i))<1){e=0;break}if(0|$a(a,e,i,n)){if(0|jf(a)){ba(A),e=1;break}if((0|l)<=671?(0|l)<=239?(0|l)<=79?(0|l)>23?(B=3,o=21):B=1:(B=4,o=21):(B=5,o=21):(B=6,o=21),21==(0|o)){if(!(0|Za(u,a,a,f+128|0,n))){e=0;break}if((0|(a=1<<B+-1))>1){i=1;do{if(e=0|tB(n),t[f+(i<<2)>>2]=e,!e){e=0;break A}if(!(0|Za(e,0|t[f+(i+-1<<2)>>2],u,f+128|0,n))){e=0;break A}i=i+1|0}while((0|i)<(0|a))}}if(0|ta(A,1)){if(B>>>0<=1)for(i=0|t[f>>2],a=1,e=l+-1|0;;){e:do{if(!(0|Qa(r,e))){if(0|a)for(;;){if(!e){e=1;break A}if(0|Qa(r,e=e+-1|0))break e}do{if(!(0|Za(A,A,A,f+128|0,n))){e=0;break A}if(!e){e=1;break A}e=e+-1|0}while(0==(0|Qa(r,e)))}}while(0);if(0==(0|a)&&0==(0|Za(A,A,A,f+128|0,n))){e=0;break A}if(!(0|Za(A,A,i,f+128|0,n))){e=0;break A}if((0|(e=e+-1|0))<0){e=1;break A}a=0}for(c=1,e=l+-1|0;;){e:do{if(!(0|Qa(r,e))){if(0|c)for(;;){if(!e){e=1;break A}if(0|Qa(r,e=e+-1|0))break e}do{if(!(0|Za(A,A,A,f+128|0,n))){e=0;break A}if(!e){e=1;break A}e=e+-1|0}while(0==(0|Qa(r,e)))}}while(0);for(u=1,o=0,a=1;!((0|(i=e-u|0))<0);){if(l=0==(0|Qa(r,i)),i=l?o:u,a=l?a:a<<u-o|1,(u=u+1|0)>>>0>=B>>>0){o=i;break}o=i}if(!(0!=(0|c)|(0|o)<0))for(i=0;;){if(!(0|Za(A,A,A,f+128|0,n))){e=0;break A}if(!((0|i)<(0|o)))break;i=i+1|0}if(!(0|Za(A,A,0|t[f+(a>>1<<2)>>2],f+128|0,n))){e=0;break A}if((0|(e=e+-1-o|0))<0){e=1;break}c=0}}else e=0}else e=0}}while(0);return nB(n),function(A){(A|=0)&&(Pf(A),Pf(A+20|0),1&t[A+48>>2]&&Xn(A))}(f+128|0),G=f,0|(n=e)}return Xe(3,125,66,0,0),G=f,0|(n=0)}(A,e,r,i,n))}function Pa(A,e,r,i,n,f){A|=0,e|=0,i|=0,n|=0,f|=0;var a,B=0,o=0,u=0,l=0,c=0,E=0,Q=0,w=0;if(a=G,G=G+128|0,0==(0|Of(r|=0,4))&&0==(0|Of(e,4))&&0==(0|Of(i,4))){if(!(0|da(i)))return Xe(3,109,102,0,0),G=a,0|(n=0);if(!(E=0|Kf(r)))return 0|ka(i,1)?(ba(A),G=a,0|(n=1)):(n=0|ta(A,1),G=a,0|n);iB(n),c=0|tB(n),w=0|tB(n),o=0|tB(n),t[a>>2]=o;A:do{if(0==(0|c)|0==(0|w)|0==(0|o))o=0,e=0,u=73;else{if(f)B=f;else{if(!(B=0|Ta())){B=0,e=0,u=74;break}if(!(0|Sa(B,i,n))){e=0,u=74;break}}if(!(0==(0|t[e+12>>2])&&(0|la(e,i))<=-1)){if(!(0|$a(o,e,i,n))){o=B,e=0,u=73;break}e=o}if(0|jf(e)){ba(A),o=B,e=1,u=73;break}if(0|Xa(o,e,B,n)){if((0|E)<=671?(0|E)<=239?(0|E)<=79?(0|E)>23?(l=3,u=25):Q=1:(l=4,u=25):(l=5,u=25):(l=6,u=25),25==(0|u)){if(!(0|ma(c,o,o,B,n))){o=B,e=0,u=73;break}if((0|(u=1<<l+-1))>1)for(o=1;;){if(e=0|tB(n),t[a+(o<<2)>>2]=e,!e){o=B,e=0,u=73;break A}if(!(0|ma(e,0|t[a+(o+-1<<2)>>2],c,B,n))){o=B,e=0,u=73;break A}if((0|(o=o+1|0))>=(0|u)){Q=l;break}}else Q=l}if(l=0|t[i+4>>2],(0|t[(0|t[i>>2])+(l+-1<<2)>>2])>=0){if(!(0|Xa(w,52964,B,n))){o=B,e=0,u=73;break}}else{if(!(0|ia(w,l))){o=B,e=0,u=73;break}if(e=0|t[i>>2],o=0|t[w>>2],t[o>>2]=0-(0|t[e>>2]),(0|l)>1){u=1;do{t[o+(u<<2)>>2]=~t[e+(u<<2)>>2],u=u+1|0}while((0|u)!=(0|l))}t[w+4>>2]=l}e:do{if(Q>>>0>1)for(i=1,e=E+-1|0;;){r:do{if(!(0|Qa(r,e))){if(0|i)for(;;){if(!e)break e;if(0|Qa(r,e=e+-1|0))break r}do{if(!(0|ma(w,w,w,B,n))){o=B,e=0,u=73;break A}if(!e)break e;e=e+-1|0}while(0==(0|Qa(r,e)))}}while(0);for(c=1,l=0,u=1;!((0|(o=e-c|0))<0);){if(o=(E=0==(0|Qa(r,o)))?l:c,u=E?u:u<<c-l|1,(c=c+1|0)>>>0>=Q>>>0){l=o;break}l=o}if(!(0!=(0|i)|(0|l)<0))for(o=0;;){if(!(0|ma(w,w,w,B,n))){o=B,e=0,u=73;break A}if(!((0|o)<(0|l)))break;o=o+1|0}if(!(0|ma(w,w,0|t[a+(u>>1<<2)>>2],B,n))){o=B,e=0,u=73;break A}if((0|(e=e+-1-l|0))<0)break;i=0}else for(o=0|t[a>>2],u=1,e=E+-1|0;;){r:do{if(!(0|Qa(r,e))){if(0|u)for(;;){if(!e)break e;if(0|Qa(r,e=e+-1|0))break r}do{if(!(0|ma(w,w,w,B,n))){o=B,e=0,u=73;break A}if(!e)break e;e=e+-1|0}while(0==(0|Qa(r,e)))}}while(0);if(0==(0|u)&&0==(0|ma(w,w,w,B,n))){o=B,e=0,u=73;break A}if(!(0|ma(w,w,o,B,n))){o=B,e=0,u=73;break A}if((0|(e=e+-1|0))<0)break;u=0}}while(0);o=B,e=0!=(0|va(A,w,B,n))&1,u=73}else o=B,e=0,u=73}}while(0);return 73==(0|u)&&(f||(B=o,u=74)),74==(0|u)&&La(B),nB(n),G=a,0|(n=e)}return n=0|_a(A,e,r,i,n,f),G=a,0|n}function _a(A,e,r,i,n,f){A|=0,e|=0,r|=0,n|=0,f|=0;var a,B,o,u=0,l=0,c=0,E=0,Q=0,w=0,b=0,k=0,s=0,C=0,d=0;if(o=G,G=G+48|0,!(0|da(i|=0)))return Xe(3,124,102,0,0),G=o,0|(n=0);if(B=0|t[i+4>>2],!(a=0|t[r+4>>2]))return 0|ka(i,1)?(ba(A),G=o,0|(n=1)):(n=0|ta(A,1),G=o,0|n);if(iB(n),f?(Q=f,d=10):(u=0|Ta())?0|Sa(u,i,n)?(Q=u,d=10):(Q=u,c=0,E=0,l=0,u=0,d=59):(Q=0,c=0,E=0,l=0,u=0,d=59),10==(0|d)){(0|(l=((B<<1|0)>(0|(s=1<<(C=(0|a)<=29?(0|a)>9?5:(0|a)>2?4:(0|a)>0?3:1:6)))?B<<1:s)+(B<<C)<<2))>=3072?(u=0|Ln(l+64|0))?d=16:(c=0,E=0,u=0):(u=G,G=G+(15+(1*(l+64|0)|0)&-16)|0,d=16);A:do{if(16==(0|d)){if(Df(0|(c=u+(64-(63&u))|0),0,0|l),E=(0|l)<3072?0:u,w=c+(B<<2<<C)|0,t[o+20>>2]=w,t[o>>2]=w+(B<<2),t[o+4>>2]=0,t[(k=o+20+4|0)>>2]=0,t[o+8>>2]=B,t[o+20+8>>2]=B,t[o+12>>2]=0,t[o+20+12>>2]=0,t[o+16>>2]=2,t[o+20+16>>2]=2,u=0|t[i>>2],(0|t[u+(B+-1<<2)>>2])>=0){if(!(0|Xa(o+20|0,52964,Q,n))){u=0;break}}else{if(t[w>>2]=0-(0|t[u>>2]),(0|B)>1){b=1;do{t[w+(b<<2)>>2]=~t[u+(b<<2)>>2],b=b+1|0}while((0|b)!=(0|B))}t[k>>2]=B}if(0==(0|t[e+12>>2])&&(0|la(e,i))<=-1){if(!(0|Xa(o,e,Q,n))){u=0;break}}else{if(!(0|wB(0,o,e,i,n))){u=0;break}if(!(0|Xa(o,o,Q,n))){u=0;break}}if(e=0|t[o+20>>2],(0|(b=(0|(b=0|t[k>>2]))<(0|B)?b:B))>0)for(u=0,w=0;t[c+(w<<2)>>2]=t[e+(u<<2)>>2],!((0|(u=u+1|0))>=(0|b));)w=w+s|0;if(u=0|t[o>>2],(0|(e=(0|(e=0|t[o+4>>2]))<(0|B)?e:B))>0)for(w=0,b=1;t[c+(b<<2)>>2]=t[u+(w<<2)>>2],!((0|(w=w+1|0))>=(0|e));)b=b+s|0;if(C>>>0>1){if(!(0|ma(o+20|0,o,o,Q,n))){u=0;break}if(e=0|t[o+20>>2],(0|(b=(0|(b=0|t[k>>2]))<(0|B)?b:B))>0)for(u=0,w=2;t[c+(w<<2)>>2]=t[e+(u<<2)>>2],!((0|(u=u+1|0))>=(0|b));)w=w+s|0;if((0|s)>3){w=3;do{if(!(0|ma(o+20|0,o,o+20|0,Q,n))){u=0;break A}if(i=0|t[o+20>>2],(0|(e=(0|(e=0|t[k>>2]))<(0|B)?e:B))>0)for(u=0,b=w;t[c+(b<<2)>>2]=t[i+(u<<2)>>2],!((0|(u=u+1|0))>=(0|e));)b=b+s|0;w=w+1|0}while((0|w)<(0|s))}}if((0|(u=((a<<5)-1|0)%(0|C)|0))>-1)for(w=(a<<5)-1|0,b=u,u=0;u=(0|Qa(r,w))+(u<<1)|0,w=w+-1|0,(0|b)>0;)b=b+-1|0;else w=(a<<5)-1|0,u=0;if(0|qa(o+20|0,B,c,u,C)){e:do{if((0|w)>-1)for(b=0,u=0;;){if(!(0|ma(o+20|0,o+20|0,o+20|0,Q,n))){u=0;break A}if(u=(0|Qa(r,w))+(u<<1)|0,e=w+-1|0,(b=b+1|0)>>>0>=C>>>0){if(!(0|qa(o,B,c,u,C))){u=0;break A}if(!(0|ma(o+20|0,o+20|0,o,Q,n))){u=0;break A}if(!((0|w)>0))break e;b=0,u=0}w=e}}while(0);u=0!=(0|va(A,o+20|0,Q,n))&1}else u=0}}while(0);f||(d=59)}return 59==(0|d)&&La(Q),0|c&&($r(c,l),Xn(E)),nB(n),G=o,0|(n=u)}function qa(A,e,r,i,n){r|=0,i|=0,n|=0;var f=0,a=0,B=0,o=0,u=0;if(!(0|ia(A|=0,e|=0)))return 0|(e=0);A:do{if((0|n)<4){if((0|e)>0)for(B=0;;){f=0,a=0;do{f=t[r+(a<<2)>>2]&((a^i)-1&(-2147483648^i))>>31|f,a=a+1|0}while((0|a)<(1<<n|0));if(t[(0|t[A>>2])+(B<<2)>>2]=f,(0|(B=B+1|0))==(0|e))break;r=r+(1<<n<<2)|0}}else if(o=1<<n+-2,u=i>>n+-2,(0|e)>0){if(31==(n+-2|0))for(f=0|t[A>>2],a=0;;)if(t[f+(a<<2)>>2]=0,(0|(a=a+1|0))==(0|e))break A;for(B=0;;){f=0,a=0;do{f=(t[r+(a+o<<2)>>2]&((1^u)-1&(-2147483648^u))>>31|t[r+(a<<2)>>2]&(u+-1&(-2147483648^u))>>31|t[r+(a+(o<<1)<<2)>>2]&((2^u)-1&(-2147483648^u))>>31|t[r+(a+(3<<n+-2)<<2)>>2]&((3^u)-1&(-2147483648^u))>>31)&((a^o+-1&i)-1&(o+-1&i^-2147483648))>>31|f,a=a+1|0}while((0|a)<(0|o));if(t[(0|t[A>>2])+(B<<2)>>2]=f,(0|(B=B+1|0))==(0|e))break;r=r+(1<<n<<2)|0}}}while(0);return t[A+4>>2]=e,0|(e=1)}function $a(A,e,r,i){return 0|wB(0,A|=0,e|=0,r|=0,i|=0)?0|t[A+12>>2]?0|(A=0|DB[31&(0==(0|t[r+12>>2])?18:19)](A,A,r)):0|(A=1):0|(A=0)}function AB(A,e,r,i,n){A|=0,e|=0,r|=0,i|=0;var f,a=0,B=0;iB(n|=0),f=0|tB(n);do{if(f){if((0|e)==(0|r)){if(!(0|fB(f,e,n))){e=0;break}if(0|wB(0,A,f,i,n)){if(!(0|t[A+12>>2])){e=1;break}a=0|DB[31&(0==(0|t[i+12>>2])?18:19)](A,A,i),B=11}}else{if(!(0|Sf(f,e,r,n))){e=0;break}if(0|wB(0,A,f,i,n)){if(!(0|t[A+12>>2])){e=1;break}a=0|DB[31&(0==(0|t[i+12>>2])?18:19)](A,A,i),B=11}}if(11==(0|B)&&0|a){e=1;break}e=0}else e=0}while(0);return nB(n),0|e}function eB(){var A=0;return(A=0|Sn(48,82796))?(t[A>>2]=0,t[A+4>>2]=0,t[A+8>>2]=0,t[A+12>>2]=0,t[A+16>>2]=0,t[A+20>>2]=0,t[A+24>>2]=0,t[A+28>>2]=0,0|A):(Xe(3,106,65,0,0),0|(A=0))}function rB(A){var e=0,r=0;if(A|=0){if(Xn(0|t[A+20>>2]),t[A+20>>2]=0,0|(e=0|t[A>>2]))do{0|t[e>>2]&&xf(e),0|t[(r=e+20|0)>>2]&&xf(r),0|t[(r=e+40|0)>>2]&&xf(r),0|t[(r=e+60|0)>>2]&&xf(r),0|t[(r=e+80|0)>>2]&&xf(r),0|t[(r=e+100|0)>>2]&&xf(r),0|t[(r=e+120|0)>>2]&&xf(r),0|t[(r=e+140|0)>>2]&&xf(r),0|t[(r=e+160|0)>>2]&&xf(r),0|t[(r=e+180|0)>>2]&&xf(r),0|t[(r=e+200|0)>>2]&&xf(r),0|t[(r=e+220|0)>>2]&&xf(r),0|t[(r=e+240|0)>>2]&&xf(r),0|t[(r=e+260|0)>>2]&&xf(r),0|t[(r=e+280|0)>>2]&&xf(r),0|t[(e=e+300|0)>>2]&&xf(e),e=0|t[A>>2],t[A+4>>2]=t[e+324>>2],Xn(e),e=0|t[A+4>>2],t[A>>2]=e}while(0!=(0|e));Xn(A)}}function iB(A){var e=0,r=0,i=0,n=0;if(0==(0|(e=0|t[(A|=0)+36>>2]))&&0==(0|t[A+40>>2])){if(n=0|t[A+32>>2],(0|(e=0|t[A+24>>2]))==(0|t[A+28>>2])){if(!(r=0|Ln((i=0==(0|e)?32:(3*e|0)>>>1)<<2)))return Xe(3,129,109,0,0),void(t[A+36>>2]=1+(0|t[A+36>>2]));(e=0|t[A+24>>2])?(If(0|r,0|t[A+20>>2],e<<2|0),e=A+20|0):e=A+20|0,Xn(0|t[e>>2]),t[e>>2]=r,t[A+28>>2]=i,e=0|t[A+24>>2]}else r=0|t[A+20>>2];return t[A+24>>2]=e+1,void(t[r+(e<<2)>>2]=n)}t[A+36>>2]=e+1}function nB(A){var e=0,r=0,i=0,n=0;if(0|(e=0|t[(A|=0)+36>>2]))t[A+36>>2]=e+-1;else{if(e=0|t[A+20>>2],n=(0|t[A+24>>2])-1|0,t[A+24>>2]=n,n=0|t[e+(n<<2)>>2],(e=0|t[A+32>>2])>>>0>n>>>0&&(r=0|t[A+12>>2],t[A+12>>2]=r-(e-n),e-n|0)){i=e-n|0,e=r+15&15;do{i=i+-1|0,e?e=e+-1|0:(t[A+4>>2]=t[320+(0|t[A+4>>2])>>2],e=15)}while(0!=(0|i))}t[A+32>>2]=n,t[A+40>>2]=0}}function tB(A){var e,r=0,i=0,n=0;return 0|t[(A|=0)+36>>2]||0|t[A+40>>2]?0|(A=0):(i=0|t[A+44>>2],(0|(e=0|t[A+12>>2]))==(0|t[A+16>>2])?(r=0|Ln(328))?(_f(r),8&i?(ha(r,8),_f(r+20|0),ha(r+20|0,8),_f(r+40|0),ha(r+40|0,8),_f(r+60|0),ha(r+60|0,8),_f(r+80|0),ha(r+80|0,8),_f(r+100|0),ha(r+100|0,8),_f(r+120|0),ha(r+120|0,8),_f(r+140|0),ha(r+140|0,8),_f(r+160|0),ha(r+160|0,8),_f(r+180|0),ha(r+180|0,8),_f(r+200|0),ha(r+200|0,8),_f(r+220|0),ha(r+220|0,8),_f(r+240|0),ha(r+240|0,8),_f(r+260|0),ha(r+260|0,8),_f(r+280|0),ha(r+280|0,8),_f(r+300|0),ha(r+300|0,8)):(_f(r+20|0),_f(r+40|0),_f(r+60|0),_f(r+80|0),_f(r+100|0),_f(r+120|0),_f(r+140|0),_f(r+160|0),_f(r+180|0),_f(r+200|0),_f(r+220|0),_f(r+240|0),_f(r+260|0),_f(r+280|0),_f(r+300|0)),i=0|t[A+8>>2],t[r+320>>2]=i,t[r+324>>2]=0,0|t[A>>2]?(t[i+324>>2]=r,t[A+8>>2]=r,i=A+4|0):(t[A+8>>2]=r,t[A+4>>2]=r,i=A),t[i>>2]=r,t[A+16>>2]=16+(0|t[A+16>>2]),t[A+12>>2]=1+(0|t[A+12>>2]),n=19):n=18:(e?(r=0|t[A+4>>2],15&e?i=15&e:(r=r+324|0,i=A+4|0,n=16)):(r=A,i=A+4|0,n=16),16==(0|n)&&(r=0|t[r>>2],t[i>>2]=r,i=0),t[A+12>>2]=e+1,n=(r=r+(20*i|0)|0)?19:18),18==(0|n)?(t[A+40>>2]=1,Xe(3,116,109,0,0),0|(A=0)):19==(0|n)?(ba(r),t[(n=r+16|0)>>2]=-5&t[n>>2],t[A+32>>2]=1+(0|t[A+32>>2]),0|(A=r)):0)}function fB(A,e,r){return r=0|aB(A|=0,e|=0,r|=0),aa(A),0|r}function aB(A,e,r){A|=0,r|=0;var i,n,f=0,a=0;if(n=G,G=G+128|0,(0|(f=0|t[(e|=0)+4>>2]))<1)return t[A+4>>2]=0,t[A+12>>2]=0,G=n,0|(r=1);iB(r),a=(0|e)==(0|A)?0|tB(r):A,i=0|tB(r);A:do{if(0!=(0|a)&0!=(0|i)&&0!=(0|ia(a,f<<1))){e:do{switch(0|f){case 4:mf(0|t[a>>2],0|t[e>>2]);break;case 8:Vf(0|t[a>>2],0|t[e>>2]);break;default:if((0|f)<16){BB(0|t[a>>2],0|t[e>>2],f,n);break e}if((0|f)==(1<<(0|zf(f))-1|0)){if(!(0|ia(i,f<<2))){f=0;break A}oB(0|t[a>>2],0|t[e>>2],f,0|t[i>>2]);break e}if(!(0|ia(i,f<<1))){f=0;break A}BB(0|t[a>>2],0|t[e>>2],f,0|t[i>>2]);break e}}while(0);if(t[a+12>>2]=0,t[a+4>>2]=f<<1,(0|a)!=(0|A)&&0==(0|ra(A,a))){f=0;break}f=1}else f=0}while(0);return nB(r),G=n,0|(r=f)}function BB(A,e,r,i){e|=0,i|=0;var n=0,f=0,a=0,B=0,o=0;if(t[(A|=0)+(((r|=0)<<1)-1<<2)>>2]=0,t[A>>2]=0,(0|r)>1&&(t[A+(r<<2)>>2]=0|Dn(A+4|0,e+4|0,r+-1|0,0|t[e>>2]),2!=(0|r)))for(n=e+4|0,f=r+-2|0,a=r+-1|0,B=A+12|0;o=n,n=n+4|0,t[B+((a=a+-1|0)<<2)>>2]=0|gn(B,n,a,0|t[o>>2]),!((0|f)<=1);)f=f+-1|0,B=B+8|0;hn(A,A,A,r<<1),function(A,e,r){A|=0,e|=0;var i=0,n=0,f=0,a=0,B=0;if(!((0|(r|=0))<1)){if(r>>>0>3){for(i=e,n=r,f=A;a=0|lf(0|(a=0|t[i>>2]),0,0|a,0),B=0|E(),t[f>>2]=a,t[f+4>>2]=B,B=0|lf(0|(B=0|t[i+4>>2]),0,0|B,0),a=0|E(),t[f+8>>2]=B,t[f+12>>2]=a,a=0|lf(0|(a=0|t[i+8>>2]),0,0|a,0),B=0|E(),t[f+16>>2]=a,t[f+20>>2]=B,B=0|lf(0|(B=0|t[i+12>>2]),0,0|B,0),a=0|E(),t[f+24>>2]=B,t[f+28>>2]=a,!((n=n+-4|0)>>>0<=3);)i=i+16|0,f=f+32|0;if(!(i=r+-4-(r+-4&-4)|0))return;e=e+(4+(r+-4&-4)<<2)|0,A=A+(8+((r+-4|0)>>>2<<3)<<2)|0}else i=r;for(;a=0|lf(0|(a=0|t[e>>2]),0,0|a,0),B=0|E(),t[A>>2]=a,t[A+4>>2]=B,i=i+-1|0;)e=e+4|0,A=A+8|0}}(i,e,r),hn(A,A,i,r<<1)}function oB(A,e,r,i){A|=0,e|=0,i|=0;var n=0,f=0,a=0,B=0;switch(0|(r|=0)){case 4:return void mf(A,e);case 8:return void Vf(A,e);default:if((0|r)<16)return void BB(A,e,r,i);n=0|function(A,e,r){e|=0;var i=0,n=0,f=0;if(!(r|=0))return 0|(f=0);if((0|(i=0|t[(A|=0)+(r+-1<<2)>>2]))!=(0|(n=0|t[e+(r+-1<<2)>>2])))return 0|(f=i>>>0>n>>>0?1:-1);if((0|r)<=1)return 0|(f=0);for(r=r+-2|0;(0|(i=0|t[A+(r<<2)>>2]))==(0|(n=0|t[e+(r<<2)>>2]));){if(!((0|r)>0)){i=0,f=9;break}r=r+-1|0}return 9==(0|f)?0|i:0|(f=i>>>0>n>>>0?1:-1)}(e,a=e+(((0|r)/2|0)<<2)|0,(0|r)/2|0);do{if((0|n)<=0){if((0|n)<0){Un(i,a,e,(0|r)/2|0),B=10;break}Df(i+(r<<2)|0,0,r<<2|0),n=i+(r<<1<<2)|0,f=i+(r<<2)|0;break}Un(i,e,a,(0|r)/2|0),B=10}while(0);if(10==(0|B)&&(oB(i+(r<<2)|0,i,(0|r)/2|0,i+(r<<1<<2)|0),n=i+(r<<1<<2)|0,f=i+(r<<2)|0),oB(A,e,(0|r)/2|0,n),oB(A+(r<<2)|0,a,(0|r)/2|0,n),!(f=(0|hn(i,A,A+(r<<2)|0,r))-(0|Un(f,i,f,r))+(0|hn(A+(((0|r)/2|0)<<2)|0,A+(((0|r)/2|0)<<2)|0,f,r))|0))return;if(A=(0|t[(n=A+(((0|r)/2|0)+r<<2)|0)>>2])+f|0,t[n>>2]=A,A>>>0>=f>>>0)return;do{A=1+(0|t[(n=n+4|0)>>2])|0,t[n>>2]=A}while(!(0|A));return}}function uB(A,e){var r,i=0,n=0;if(!(e|=0))return 0|(e=-1);if((0|(i=0|t[(A|=0)+4>>2]))<=0)return 0|(e=0);r=0|t[A>>2],n=0,A=i;do{i=A,n=0|kf(0|t[r+((A=A+-1|0)<<2)>>2],0|n,0|e,0),E()}while((0|i)>1);return 0|(e=n)}function lB(A,e){A|=0;var r,i=0,n=0,f=0,a=0,B=0,u=0;if(!(e|=0))return 0|(A=-1);if(!(0|t[A+4>>2]))return 0|(A=0);if(!(0|rn(A,A,r=32-(0|zf(e))|0)))return 0|(A=-1);i=0|t[A+4>>2];do{if((0|i)>0){n=0|t[A>>2],f=0;do{a=i,B=0|Mn(f,u=0|t[n+((i=i+-1|0)<<2)>>2],e<<r),f=u-(0|o(B,e<<r))|0,n=0|t[A>>2],t[n+(i<<2)>>2]=B}while((0|a)>1);if((0|(i=0|t[A+4>>2]))>0){if(!(0|t[(0|t[A>>2])+(i+-1<<2)>>2])){t[A+4>>2]=i+-1,n=i+-1|0,i=f;break}return 0|(u=f>>>r)}n=i,i=f}else n=i,i=0}while(0);return i>>>=r,0|n||(t[A+12>>2]=0),0|(u=i)}function cB(A,e){var r=0,i=0,n=0,f=0;if(!(e|=0))return 0|(n=1);if(0|jf(A|=0))return 0|(n=0|ta(A,e));if(0|t[A+12>>2])return t[A+12>>2]=0,e=0|EB(A,e),0|jf(A)||(t[A+12>>2]=0==(0|t[A+12>>2])&1),0|(n=e);for(i=0;!((0|i)>=(0|(r=0|t[A+4>>2])));){if(f=(0|t[A>>2])+(i<<2)|0,r=(0|t[f>>2])+e|0,t[f>>2]=r,!(e>>>0>r>>>0)){e=1,n=13;break}i=i+1|0,e=1}return 13==(0|n)?0|e:(0|i)!=(0|r)?0|(f=1):0|ia(A,i+1|0)?(t[A+4>>2]=1+(0|t[A+4>>2]),t[(0|t[A>>2])+(i<<2)>>2]=e,0|(f=1)):0|(f=0)}function EB(A,e){A|=0;var r=0,i=0,n=0;if(!(e|=0))return 0|(A=1);if(0|jf(A))return(r=0|ta(A,e))?(function(A,e){A|=0,e=0!=(0|(e|=0))&&0!=(0|t[A+4>>2])?1:0,t[A+12>>2]=e}(A,1),0|(A=r)):0|(A=0);if(0|t[A+12>>2])return t[A+12>>2]=0,i=0|cB(A,e),t[A+12>>2]=1,0|(A=i);if(i=0|t[A>>2],(r=0|t[i>>2])>>>0<e>>>0&&1==(0|t[A+4>>2]))return t[i>>2]=e-r,t[A+12>>2]=1,0|(A=1);if(t[i>>2]=r-e,r>>>0<e>>>0)for(r=1;;){if(e=0|t[(n=i+(r<<2)|0)>>2],t[n>>2]=e+-1,e){i=r,r=e+-1|0;break}r=r+1|0}else i=0,r=r-e|0;return 0|r||(0|i)!=((0|t[A+4>>2])-1|0)||(t[A+4>>2]=i),0|(n=1)}function QB(A,e){e|=0;var r=0,i=0;return(r=0|t[(A|=0)+4>>2])?e?(r=0|Dn(i=0|t[A>>2],i,r,e))?0|ia(A,1+(0|t[A+4>>2])|0)?(e=0|t[A>>2],i=0|t[A+4>>2],t[A+4>>2]=i+1,t[e+(i<<2)>>2]=r,0|(i=1)):0|(i=0):0|(i=1):(ba(A),0|(A=1)):0|(A=1)}function wB(A,e,r,i,n){A|=0,e|=0,i|=0,n|=0;var f,a=0,B=0,u=0,l=0,c=0,Q=0,w=0,b=0,k=0,s=0,C=0,d=0,I=0,g=0,D=0,M=0,h=0,U=0;f=G,G=G+32|0,(0|(U=0|t[(r|=0)+4>>2]))>0&&!(0|t[(0|t[r>>2])+(U+-1<<2)>>2])||(w=3);do{if(3==(0|w)){if((0|(U=0|t[i+4>>2]))>0&&0==(0|t[(0|t[i>>2])+(U+-1<<2)>>2]))break;if(u=0==(0|Of(r,4))&&0==(0|Of(i,4))?0:1,0|jf(i))return Xe(3,107,103,0,0),G=f,0|(U=0);if(!(U=0!=(0|u))&&(0|la(r,i))<0)return 0|e&&0==(0|ra(e,r))?(G=f,0|(U=0)):A?(ba(A),G=f,0|(U=1)):(G=f,0|(U=1));iB(n),I=0|tB(n),M=0|tB(n),g=0|tB(n),D=A||0|tB(n);do{if(!(0==(0|M)|0==(0|I)|0==(0|g)|0==(0|D))&&0|rn(g,i,h=32-((0|Kf(i))%32|0)|0)&&(t[g+12>>2]=0,0|rn(M,r,h+32|0))){if(t[M+12>>2]=0,A=0|t[M+4>>2],U){if((0|A)<=((a=0|t[g+4>>2])+1|0)){if(!(0|ia(M,a+2|0)))break;if((0|(a=0|t[M+4>>2]))<(0|(A=2+(0|t[g+4>>2])|0))){B=0|t[M>>2];do{t[B+(a<<2)>>2]=0,a=a+1|0,A=2+(0|t[g+4>>2])|0}while((0|a)<(0|A))}}else{if(!(0|ia(M,A+1|0)))break;t[(0|t[M>>2])+(t[M+4>>2]<<2)>>2]=0,A=1+(0|t[M+4>>2])|0}t[M+4>>2]=A}if(d=A-(s=0|t[g+4>>2])|0,t[f+12>>2]=0,t[f>>2]=(0|t[M>>2])+(d<<2),t[f+4>>2]=s,t[f+16>>2]=2,t[f+8>>2]=(0|t[M+8>>2])-d,a=0|t[g>>2],C=0|t[a+(s+-1<<2)>>2],1==(0|s)?(b=0,k=0):(b=0|t[a+(s+-2<<2)>>2],k=0),a=(0|t[M>>2])+(A+-1<<2)|0,0|ia(D,d+1|0)&&(t[(Q=D+12|0)>>2]=t[i+12>>2]^t[r+12>>2],t[(l=D+4|0)>>2]=d-u,c=0|t[D>>2],0|ia(I,s+1|0))){do{if(!U){if((0|la(f,g))>-1){Un(w=0|t[f>>2],w,0|t[g>>2],s),t[c+(d+-1<<2)>>2]=1,w=39;break}A=(0|t[l>>2])-1|0,t[l>>2]=A;break}w=39}while(0);if(39==(0|w)&&(A=0|t[l>>2]),A?A=c+(d<<2)+-4|0:(t[Q>>2]=0,A=c+(d<<2)|0),(0|d)>1){w=0,Q=a;do{a=0|t[Q>>2],c=Q,B=0|t[(Q=Q+-4|0)>>2];A:do{if((0|a)==(0|C))a=-1;else{if(a=0|Mn(a,B,C),u=B-(0|o(a,C))|0,B=0|lf(0|b,0|k,0|a,0),!((i=0|E())>>>0>u>>>0|(0|i)==(0|u)&B>>>0>(l=0|t[c+-8>>2])>>>0))break;do{if(a=a+-1|0,u=u+C|0,B=0|Ef(0|B,0|i,0|b,0|k),i=0|E(),u>>>0<C>>>0)break A}while(i>>>0>u>>>0|(0|i)==(0|u)&B>>>0>l>>>0)}}while(0);l=0|Dn(0|t[I>>2],0|t[g>>2],s,a),t[(0|t[I>>2])+(s<<2)>>2]=l,l=(0|t[f>>2])-4|0,t[f>>2]=l;do{if(0|Un(l,l,0|t[I>>2],s+1|0)){if(a=a+-1|0,!(0|hn(l=0|t[f>>2],l,0|t[g>>2],s)))break;t[c>>2]=1+(0|t[c>>2])}}while(0);t[(A=A+-4|0)>>2]=a,w=w+1|0}while((0|w)<(d+-1|0))}aa(M);do{if(0|e){if(A=0|t[r+12>>2],nn(e,M,h+32|0),0|jf(e))break;t[e+12>>2]=A}}while(0);return U&&aa(D),nB(n),G=f,0|(U=1)}}}while(0);return nB(n),G=f,0|(U=0)}}while(0);return Xe(3,107,107,0,0),G=f,0|(U=0)}function bB(A,e,r){e|=0;var n=0,f=0,a=0;if(!(r|=0))return 1;f=0|t[(A|=0)+20>>2],n=0|t[A+24>>2],(f+(r<<3)|0)>>>0<f>>>0&&(t[A+24>>2]=n+1,n=n+1|0),t[A+24>>2]=n+(r>>>29),t[A+20>>2]=f+(r<<3),a=0|t[A+92>>2];do{if(a){if((a+r|r)>>>0>63){If(A+28+a|0,0|e,64-a|0),kB(A,A+28|0,1),f=(n=A+28|0)+68|0;do{i[n>>0]=0,n=n+1|0}while((0|n)<(0|f));n=e+(64-a)|0,r=r-(64-a)|0;break}return If(A+28+a|0,0|e,0|r),t[A+92>>2]=(0|t[A+92>>2])+r,1}n=e}while(0);return a=-64&r,(f=r>>>6)&&(kB(A,n,f),n=n+a|0,r=r-a|0),r?(t[A+92>>2]=r,If(A+28|0,0|n,0|r),1):1}function kB(A,e,r){e|=0,r|=0;var i=0,n=0,a=0,B=0,o=0,u=0,l=0,c=0,E=0,Q=0,w=0,b=0,k=0,s=0,C=0,d=0,I=0,g=0,D=0,M=0,h=0,U=0,G=0,Z=0,N=0,F=0;for(i=0|t[(A|=0)>>2],n=0|t[A+4>>2],a=0|t[A+8>>2],B=0|t[A+12>>2],o=0|t[A+16>>2];I=(0|f[e+1>>0])<<16|(0|f[e>>0])<<24|(0|f[e+2>>0])<<8|0|f[e+3>>0],k=(0|f[e+5>>0])<<16|(0|f[e+4>>0])<<24|(0|f[e+6>>0])<<8|0|f[e+7>>0],u=o+1518500249+((a^B)&n^B)+(i<<5|i>>>27)+I|0,E=n<<30|n>>>2,D=(0|f[e+9>>0])<<16|(0|f[e+8>>0])<<24|(0|f[e+10>>0])<<8|0|f[e+11>>0],l=B+1518500249+((E^a)&i^a)+k+(u<<5|u>>>27)|0,Q=i<<30|i>>>2,F=(0|f[e+13>>0])<<16|(0|f[e+12>>0])<<24|(0|f[e+14>>0])<<8|0|f[e+15>>0],c=a+1518500249+(u&(Q^E)^E)+D+(l<<5|l>>>27)|0,N=(0|f[e+17>>0])<<16|(0|f[e+16>>0])<<24|(0|f[e+18>>0])<<8|0|f[e+19>>0],E=E+1518500249+(l&((u<<30|u>>>2)^Q)^Q)+F+(c<<5|c>>>27)|0,U=(0|f[e+21>>0])<<16|(0|f[e+20>>0])<<24|(0|f[e+22>>0])<<8|0|f[e+23>>0],Q=Q+1518500249+(c&((l<<30|l>>>2)^(u<<30|u>>>2))^(u<<30|u>>>2))+N+(E<<5|E>>>27)|0,d=(0|f[e+25>>0])<<16|(0|f[e+24>>0])<<24|(0|f[e+26>>0])<<8|0|f[e+27>>0],u=1518500249+(u<<30|u>>>2)+(E&((c<<30|c>>>2)^(l<<30|l>>>2))^(l<<30|l>>>2))+U+(Q<<5|Q>>>27)|0,b=(0|f[e+29>>0])<<16|(0|f[e+28>>0])<<24|(0|f[e+30>>0])<<8|0|f[e+31>>0],l=1518500249+(l<<30|l>>>2)+(Q&((E<<30|E>>>2)^(c<<30|c>>>2))^(c<<30|c>>>2))+d+(u<<5|u>>>27)|0,C=(0|f[e+33>>0])<<16|(0|f[e+32>>0])<<24|(0|f[e+34>>0])<<8|0|f[e+35>>0],c=1518500249+(c<<30|c>>>2)+(u&((Q<<30|Q>>>2)^(E<<30|E>>>2))^(E<<30|E>>>2))+b+(l<<5|l>>>27)|0,Z=(0|f[e+37>>0])<<16|(0|f[e+36>>0])<<24|(0|f[e+38>>0])<<8|0|f[e+39>>0],E=1518500249+(E<<30|E>>>2)+(l&((u<<30|u>>>2)^(Q<<30|Q>>>2))^(Q<<30|Q>>>2))+C+(c<<5|c>>>27)|0,M=(0|f[e+41>>0])<<16|(0|f[e+40>>0])<<24|(0|f[e+42>>0])<<8|0|f[e+43>>0],Q=1518500249+(Q<<30|Q>>>2)+(c&((l<<30|l>>>2)^(u<<30|u>>>2))^(u<<30|u>>>2))+Z+(E<<5|E>>>27)|0,w=(0|f[e+45>>0])<<16|(0|f[e+44>>0])<<24|(0|f[e+46>>0])<<8|0|f[e+47>>0],u=1518500249+(u<<30|u>>>2)+(E&((c<<30|c>>>2)^(l<<30|l>>>2))^(l<<30|l>>>2))+M+(Q<<5|Q>>>27)|0,h=(0|f[e+49>>0])<<16|(0|f[e+48>>0])<<24|(0|f[e+50>>0])<<8|0|f[e+51>>0],l=1518500249+(l<<30|l>>>2)+(Q&((E<<30|E>>>2)^(c<<30|c>>>2))^(c<<30|c>>>2))+w+(u<<5|u>>>27)|0,G=(0|f[e+53>>0])<<16|(0|f[e+52>>0])<<24|(0|f[e+54>>0])<<8|0|f[e+55>>0],c=1518500249+(c<<30|c>>>2)+(u&((Q<<30|Q>>>2)^(E<<30|E>>>2))^(E<<30|E>>>2))+h+(l<<5|l>>>27)|0,g=(0|f[e+57>>0])<<16|(0|f[e+56>>0])<<24|(0|f[e+58>>0])<<8|0|f[e+59>>0],E=1518500249+(E<<30|E>>>2)+(l&((u<<30|u>>>2)^(Q<<30|Q>>>2))^(Q<<30|Q>>>2))+G+(c<<5|c>>>27)|0,I=(D^I^C^G)<<1|(D^I^C^G)>>>31,D=(N^D^M^(s=(0|f[e+61>>0])<<16|(0|f[e+60>>0])<<24|(0|f[e+62>>0])<<8|0|f[e+63>>0]))<<1|(N^D^M^s)>>>31,N=(d^N^h^(k=(F^k^Z^g)<<1|(F^k^Z^g)>>>31))<<1|(d^N^h^k)>>>31,c=(d=(C^d^g^(F=(U^F^w^I)<<1|(U^F^w^I)>>>31))<<1|(C^d^g^F)>>>31)+1859775393+((c=1518500249+(c<<30|c>>>2)+k+((u=1518500249+(u<<30|u>>>2)+(E&((c<<30|c>>>2)^(l<<30|l>>>2))^(l<<30|l>>>2))+s+((Q=1518500249+(Q<<30|Q>>>2)+(c&((l<<30|l>>>2)^(u<<30|u>>>2))^(u<<30|u>>>2))+g+(E<<5|E>>>27)|0)<<5|Q>>>27)|0)&((Q<<30|Q>>>2)^(E<<30|E>>>2))^(E<<30|E>>>2))+((l=1518500249+(l<<30|l>>>2)+I+(Q&((E<<30|E>>>2)^(c<<30|c>>>2))^(c<<30|c>>>2))+(u<<5|u>>>27)|0)<<5|l>>>27)|0)<<30|c>>>2)+(((Q=F+1518500249+(Q<<30|Q>>>2)+(c&((l<<30|l>>>2)^(u<<30|u>>>2))^(u<<30|u>>>2))+((E=1518500249+(E<<30|E>>>2)+D+(l&((u<<30|u>>>2)^(Q<<30|Q>>>2))^(Q<<30|Q>>>2))+(c<<5|c>>>27)|0)<<5|E>>>27)|0)<<30|Q>>>2)^(E<<30|E>>>2)^(u=N+1859775393+(u<<30|u>>>2)+((c<<30|c>>>2)^(l<<30|l>>>2)^E)+(Q<<5|Q>>>27)|0))+((l=(U=(b^U^G^D)<<1|(b^U^G^D)>>>31)+1859775393+(l<<30|l>>>2)+((E<<30|E>>>2)^(c<<30|c>>>2)^Q)+(u<<5|u>>>27)|0)<<5|l>>>27)|0,C=(M^C^I^U)<<1|(M^C^I^U)>>>31,M=(h^M^D^(b=(Z^b^s^N)<<1|(Z^b^s^N)>>>31))<<1|(h^M^D^b)>>>31,h=(g^h^N^(Z=(w^Z^k^d)<<1|(w^Z^k^d)>>>31))<<1|(g^h^N^Z)>>>31,g=(g^I^d^(w=(G^w^F^C)<<1|(G^w^F^C)>>>31))<<1|(g^I^d^w)>>>31,I=(D^I^C^(G=(s^G^U^M)<<1|(s^G^U^M)>>>31))<<1|(D^I^C^G)>>>31,D=(D^N^M^(s=(s^k^b^h)<<1|(s^k^b^h)>>>31))<<1|(D^N^M^s)>>>31,N=(N^d^h^(k=(k^F^Z^g)<<1|(k^F^Z^g)>>>31))<<1|(N^d^h^k)>>>31,E=(d=(C^d^g^(F=(U^F^w^I)<<1|(U^F^w^I)>>>31))<<1|(C^d^g^F)>>>31)+1859775393+((E=k+1859775393+((E=h+1859775393+((E=b+1859775393+(E<<30|E>>>2)+((u<<30|u>>>2)^(Q<<30|Q>>>2)^l)+(c<<5|c>>>27)|0)<<30|E>>>2)+(((u=Z+1859775393+(u<<30|u>>>2)+((c<<30|c>>>2)^(l<<30|l>>>2)^E)+((Q=C+1859775393+(Q<<30|Q>>>2)+((l<<30|l>>>2)^(u<<30|u>>>2)^c)+(E<<5|E>>>27)|0)<<5|Q>>>27)|0)<<30|u>>>2)^(Q<<30|Q>>>2)^(l=M+1859775393+(l<<30|l>>>2)+((E<<30|E>>>2)^(c<<30|c>>>2)^Q)+(u<<5|u>>>27)|0))+((c=w+1859775393+(c<<30|c>>>2)+((Q<<30|Q>>>2)^(E<<30|E>>>2)^u)+(l<<5|l>>>27)|0)<<5|c>>>27)|0)<<30|E>>>2)+(((u=g+1859775393+(u<<30|u>>>2)+((c<<30|c>>>2)^(l<<30|l>>>2)^E)+((Q=G+1859775393+(Q<<30|Q>>>2)+((l<<30|l>>>2)^(u<<30|u>>>2)^c)+(E<<5|E>>>27)|0)<<5|Q>>>27)|0)<<30|u>>>2)^(Q<<30|Q>>>2)^(l=s+1859775393+(l<<30|l>>>2)+((E<<30|E>>>2)^(c<<30|c>>>2)^Q)+(u<<5|u>>>27)|0))+((c=I+1859775393+(c<<30|c>>>2)+((Q<<30|Q>>>2)^(E<<30|E>>>2)^u)+(l<<5|l>>>27)|0)<<5|c>>>27)|0)<<30|E>>>2)+(((u=F+1859775393+(u<<30|u>>>2)+((c<<30|c>>>2)^(l<<30|l>>>2)^E)+((Q=D+1859775393+(Q<<30|Q>>>2)+((l<<30|l>>>2)^(u<<30|u>>>2)^c)+(E<<5|E>>>27)|0)<<5|Q>>>27)|0)<<30|u>>>2)^(Q<<30|Q>>>2)^(l=N+1859775393+(l<<30|l>>>2)+((E<<30|E>>>2)^(c<<30|c>>>2)^Q)+(u<<5|u>>>27)|0))+((c=(U=(U^b^G^D)<<1|(U^b^G^D)>>>31)+1859775393+(c<<30|c>>>2)+((Q<<30|Q>>>2)^(E<<30|E>>>2)^u)+(l<<5|l>>>27)|0)<<5|c>>>27)|0,C=(C^M^I^U)<<1|(C^M^I^U)>>>31,M=(M^h^D^(b=(b^Z^s^N)<<1|(b^Z^s^N)>>>31))<<1|(M^h^D^b)>>>31,h=(g^h^N^(Z=(w^Z^k^d)<<1|(w^Z^k^d)>>>31))<<1|(g^h^N^Z)>>>31,g=(g^I^d^(w=(w^G^F^C)<<1|(w^G^F^C)>>>31))<<1|(g^I^d^w)>>>31,I=(I^D^C^(G=(G^s^U^M)<<1|(G^s^U^M)>>>31))<<1|(I^D^C^G)>>>31,D=(N^D^M^(s=(k^s^b^h)<<1|(k^s^b^h)>>>31))<<1|(N^D^M^s)>>>31,N=(N^d^h^(k=(k^F^Z^g)<<1|(k^F^Z^g)>>>31))<<1|(N^d^h^k)>>>31,Q=(d=(d^C^g^(F=(F^U^w^I)<<1|(F^U^w^I)>>>31))<<1|(d^C^g^F)>>>31)+-1894007588+((Q=k+-1894007588+((Q=h+-1894007588+((Q=b+1859775393+(Q<<30|Q>>>2)+((l<<30|l>>>2)^(u<<30|u>>>2)^c)+(E<<5|E>>>27)|0)<<30|Q>>>2)+(((c=M+-1894007588+(c<<30|c>>>2)+(((u=C+-1894007588+(u<<30|u>>>2)+((E|c<<30|c>>>2)&(l<<30|l>>>2)|E&(c<<30|c>>>2))+(Q<<5|Q>>>27)|0)|Q<<30|Q>>>2)&(E<<30|E>>>2)|u&(Q<<30|Q>>>2))+((l=Z+-1894007588+(l<<30|l>>>2)+((Q|E<<30|E>>>2)&(c<<30|c>>>2)|Q&(E<<30|E>>>2))+(u<<5|u>>>27)|0)<<5|l>>>27)|0)|l<<30|l>>>2)&(u<<30|u>>>2)|c&(l<<30|l>>>2))+((E=w+-1894007588+(E<<30|E>>>2)+((l|u<<30|u>>>2)&(Q<<30|Q>>>2)|l&(u<<30|u>>>2))+(c<<5|c>>>27)|0)<<5|E>>>27)|0)<<30|Q>>>2)+(((c=s+-1894007588+(c<<30|c>>>2)+(((u=G+-1894007588+(u<<30|u>>>2)+((E|c<<30|c>>>2)&(l<<30|l>>>2)|E&(c<<30|c>>>2))+(Q<<5|Q>>>27)|0)|Q<<30|Q>>>2)&(E<<30|E>>>2)|u&(Q<<30|Q>>>2))+((l=g+-1894007588+(l<<30|l>>>2)+((Q|E<<30|E>>>2)&(c<<30|c>>>2)|Q&(E<<30|E>>>2))+(u<<5|u>>>27)|0)<<5|l>>>27)|0)|l<<30|l>>>2)&(u<<30|u>>>2)|c&(l<<30|l>>>2))+((E=I+-1894007588+(E<<30|E>>>2)+((l|u<<30|u>>>2)&(Q<<30|Q>>>2)|l&(u<<30|u>>>2))+(c<<5|c>>>27)|0)<<5|E>>>27)|0)<<30|Q>>>2)+(((c=N+-1894007588+(c<<30|c>>>2)+(((u=D+-1894007588+(u<<30|u>>>2)+((E|c<<30|c>>>2)&(l<<30|l>>>2)|E&(c<<30|c>>>2))+(Q<<5|Q>>>27)|0)|Q<<30|Q>>>2)&(E<<30|E>>>2)|u&(Q<<30|Q>>>2))+((l=F+-1894007588+(l<<30|l>>>2)+((Q|E<<30|E>>>2)&(c<<30|c>>>2)|Q&(E<<30|E>>>2))+(u<<5|u>>>27)|0)<<5|l>>>27)|0)|l<<30|l>>>2)&(u<<30|u>>>2)|c&(l<<30|l>>>2))+((E=(U=(b^U^G^D)<<1|(b^U^G^D)>>>31)+-1894007588+(E<<30|E>>>2)+((l|u<<30|u>>>2)&(Q<<30|Q>>>2)|l&(u<<30|u>>>2))+(c<<5|c>>>27)|0)<<5|E>>>27)|0,C=(M^C^I^U)<<1|(M^C^I^U)>>>31,M=(M^h^D^(b=(b^Z^s^N)<<1|(b^Z^s^N)>>>31))<<1|(M^h^D^b)>>>31,h=(h^g^N^(Z=(Z^w^k^d)<<1|(Z^w^k^d)>>>31))<<1|(h^g^N^Z)>>>31,g=(I^g^d^(w=(G^w^F^C)<<1|(G^w^F^C)>>>31))<<1|(I^g^d^w)>>>31,I=(I^D^C^(G=(G^s^U^M)<<1|(G^s^U^M)>>>31))<<1|(I^D^C^G)>>>31,D=(D^N^M^(s=(s^k^b^h)<<1|(s^k^b^h)>>>31))<<1|(D^N^M^s)>>>31,N=(d^N^h^(k=(F^k^Z^g)<<1|(F^k^Z^g)>>>31))<<1|(d^N^h^k)>>>31,E=((Z^w^k^(d=(d^C^g^(F=(F^U^w^I)<<1|(F^U^w^I)>>>31))<<1|(d^C^g^F)>>>31))<<1|(Z^w^k^d)>>>31)-899497514+((E=N+-899497514+((E=s+-899497514+((E=M+-1894007588+(E<<30|E>>>2)+(((l=C+-1894007588+(l<<30|l>>>2)+((Q|E<<30|E>>>2)&(c<<30|c>>>2)|Q&(E<<30|E>>>2))+((u=b+-1894007588+(u<<30|u>>>2)+((E|c<<30|c>>>2)&(l<<30|l>>>2)|E&(c<<30|c>>>2))+(Q<<5|Q>>>27)|0)<<5|u>>>27)|0)|u<<30|u>>>2)&(Q<<30|Q>>>2)|l&(u<<30|u>>>2))+((c=Z+-1894007588+(c<<30|c>>>2)+((u|Q<<30|Q>>>2)&(E<<30|E>>>2)|u&(Q<<30|Q>>>2))+(l<<5|l>>>27)|0)<<5|c>>>27)|0)<<30|E>>>2)+(((u=h+-899497514+(u<<30|u>>>2)+((c<<30|c>>>2)^(l<<30|l>>>2)^E)+((Q=w+-1894007588+(Q<<30|Q>>>2)+((c|l<<30|l>>>2)&(u<<30|u>>>2)|c&(l<<30|l>>>2))+(E<<5|E>>>27)|0)<<5|Q>>>27)|0)<<30|u>>>2)^(Q<<30|Q>>>2)^(l=G+-899497514+(l<<30|l>>>2)+((E<<30|E>>>2)^(c<<30|c>>>2)^Q)+(u<<5|u>>>27)|0))+((c=g+-899497514+(c<<30|c>>>2)+((Q<<30|Q>>>2)^(E<<30|E>>>2)^u)+(l<<5|l>>>27)|0)<<5|c>>>27)|0)<<30|E>>>2)+(((u=k+-899497514+(u<<30|u>>>2)+((c<<30|c>>>2)^(l<<30|l>>>2)^E)+((Q=I+-899497514+(Q<<30|Q>>>2)+((l<<30|l>>>2)^(u<<30|u>>>2)^c)+(E<<5|E>>>27)|0)<<5|Q>>>27)|0)<<30|u>>>2)^(Q<<30|Q>>>2)^(l=D+-899497514+(l<<30|l>>>2)+((E<<30|E>>>2)^(c<<30|c>>>2)^Q)+(u<<5|u>>>27)|0))+((c=F+-899497514+(c<<30|c>>>2)+((Q<<30|Q>>>2)^(E<<30|E>>>2)^u)+(l<<5|l>>>27)|0)<<5|c>>>27)|0)<<30|E>>>2)+(((u=d+-899497514+(u<<30|u>>>2)+((c<<30|c>>>2)^(l<<30|l>>>2)^E)+((Q=(U=(U^b^G^D)<<1|(U^b^G^D)>>>31)+-899497514+(Q<<30|Q>>>2)+((l<<30|l>>>2)^(u<<30|u>>>2)^c)+(E<<5|E>>>27)|0)<<5|Q>>>27)|0)<<30|u>>>2)^(Q<<30|Q>>>2)^(l=(b=(Z^b^s^N)<<1|(Z^b^s^N)>>>31)+-899497514+(l<<30|l>>>2)+((E<<30|E>>>2)^(c<<30|c>>>2)^Q)+(u<<5|u>>>27)|0))+((c=((C^M^I^U)<<1|(C^M^I^U)>>>31)-899497514+(c<<30|c>>>2)+((Q<<30|Q>>>2)^(E<<30|E>>>2)^u)+(l<<5|l>>>27)|0)<<5|c>>>27)|0,C=w^G^F^((C^M^I^U)<<1|(C^M^I^U)>>>31),i=i+-899497514+((s^k^b^((w=h^g^N^((Z^w^k^d)<<1|(Z^w^k^d)>>>31))<<1|w>>>31))<<1|(s^k^b^(w<<1|w>>>31))>>>31)+((Q=((h^M^D^b)<<1|(h^M^D^b)>>>31)-899497514+(Q<<30|Q>>>2)+((l<<30|l>>>2)^(u<<30|u>>>2)^c)+(E<<5|E>>>27)|0)<<30|Q>>>2)+(((l=(w<<1|w>>>31)-899497514+(l<<30|l>>>2)+((E<<30|E>>>2)^(c<<30|c>>>2)^Q)+((u=(C<<1|C>>>31)-899497514+(u<<30|u>>>2)+((c<<30|c>>>2)^(l<<30|l>>>2)^E)+(Q<<5|Q>>>27)|0)<<5|u>>>27)|0)<<30|l>>>2)^(u<<30|u>>>2)^(c=((D=s^G^U^((h^M^D^b)<<1|(h^M^D^b)>>>31))<<1|D>>>31)-899497514+(c<<30|c>>>2)+((Q<<30|Q>>>2)^(E<<30|E>>>2)^u)+(l<<5|l>>>27)|0))+((E=((g^I^d^(C<<1|C>>>31))<<1|(g^I^d^(C<<1|C>>>31))>>>31)-899497514+(E<<30|E>>>2)+((u<<30|u>>>2)^(Q<<30|Q>>>2)^l)+(c<<5|c>>>27)|0)<<5|E>>>27)|0,t[A>>2]=i,n=E+n|0,t[A+4>>2]=n,a=(c<<30|c>>>2)+a|0,t[A+8>>2]=a,B=(l<<30|l>>>2)+B|0,t[A+12>>2]=B,o=(u<<30|u>>>2)+(0|t[A+16>>2])|0,t[A+16>>2]=o,r=r+-1|0;)e=e+64|0}function sB(A,e){A|=0;var r=0;return r=0|t[(e|=0)+92>>2],i[e+28+r>>0]=-128,(r+1|0)>>>0>56?(Df(e+28+(r+1)|0,0,63-r|0),kB(e,e+28|0,1),r=0):r=r+1|0,Df(e+28+r|0,0,56-r|0),r=0|t[e+24>>2],i[e+84>>0]=r>>>24,i[e+84+1>>0]=r>>>16,i[e+84+2>>0]=r>>>8,i[e+84+3>>0]=r,r=0|t[e+20>>2],i[e+88>>0]=r>>>24,i[e+88+1>>0]=r>>>16,i[e+88+2>>0]=r>>>8,i[e+88+3>>0]=r,kB(e,e+28|0,1),t[e+92>>2]=0,$r(e+28|0,64),r=0|t[e>>2],i[A>>0]=r>>>24,i[A+1>>0]=r>>>16,i[A+2>>0]=r>>>8,i[A+3>>0]=r,r=0|t[e+4>>2],i[A+4>>0]=r>>>24,i[A+5>>0]=r>>>16,i[A+6>>0]=r>>>8,i[A+7>>0]=r,r=0|t[e+8>>2],i[A+8>>0]=r>>>24,i[A+9>>0]=r>>>16,i[A+10>>0]=r>>>8,i[A+11>>0]=r,r=0|t[e+12>>2],i[A+12>>0]=r>>>24,i[A+13>>0]=r>>>16,i[A+14>>0]=r>>>8,i[A+15>>0]=r,e=0|t[e+16>>2],i[A+16>>0]=e>>>24,i[A+17>>0]=e>>>16,i[A+18>>0]=e>>>8,i[A+19>>0]=e,1}function CB(A){var e,r=0;e=(r=(A|=0)+20|0)+76|0;do{t[r>>2]=0,r=r+4|0}while((0|r)<(0|e));return t[A>>2]=1732584193,t[A+4>>2]=-271733879,t[A+8>>2]=-1732584194,t[A+12>>2]=271733878,t[A+16>>2]=-1009589776,1}function dB(A){var e,r=0;e=(r=(A|=0)+32|0)+76|0;do{t[r>>2]=0,r=r+4|0}while((0|r)<(0|e));return t[A>>2]=1779033703,t[A+4>>2]=-1150833019,t[A+8>>2]=1013904242,t[A+12>>2]=-1521486534,t[A+16>>2]=1359893119,t[A+20>>2]=-1694144372,t[A+24>>2]=528734635,t[A+28>>2]=1541459225,t[A+108>>2]=32,1}var IB=[Mf,function(A){var e;return A|=0,(e=0|Sn(1048,82796,0))?(t[A+20>>2]=e,0|(A=1)):0|(A=0)},function(A){var e;return A|=0,(e=0|Sn(1040,82796,0))?(t[A+20>>2]=e,0|(A=1)):0|(A=0)},function(A){A|=0;var e,r=0;return(e=0|Sn(24,82796,0))?(t[e+8>>2]=4,r=0|oe(),t[e+20>>2]=r,r?(t[A+20>>2]=e,t[A+36>>2]=0,0|(r=1)):(Xn(e,82796,0),0|(r=0))):0|(A=0)},function(A){return 0,64},function(A){return t[(A|=0)+56>>2]=6|t[A+56>>2],1},function(A){return La(0|t[(A|=0)+60>>2]),La(0|t[A+64>>2]),La(0|t[A+68>>2]),1},function(A){return 0|dr(0|t[(A|=0)+24>>2])},function(A){return 0|Kf(0|t[16+(0|t[(A|=0)+24>>2])>>2])},function(A){return 0|function(A){return 0|function(A,e){if(e|=0,(0|(A|=0))<=15359)if((0|A)<=7679)if((0|A)<=3071)if((0|A)<=2047){if(!((0|A)>1023))return 0|(e=0);A=80}else A=112;else A=128;else A=192;else A=256;return-1==(0|e)?0|(e=A):0|((0|e)<160?0:(0|(0|e)/2)<(0|A)?(0|e)/2|0:A)}(0|Kf(0|t[(A|=0)+16>>2]),-1)}(0|t[(A|=0)+24>>2])},function(A){var e;return A|=0,(e=0|Sn(44,82796,0))?(t[e>>2]=2048,t[e+16>>2]=1,t[e+28>>2]=-2,t[A+20>>2]=e,t[A+32>>2]=e+8,t[A+36>>2]=2,0|(A=1)):0|(A=0)},function(A){return 0|function(A){var e,r=0;e=(r=(A|=0)+16|0)+76|0;do{t[r>>2]=0,r=r+4|0}while((0|r)<(0|e));return t[A>>2]=1732584193,t[A+4>>2]=-271733879,t[A+8>>2]=-1732584194,t[A+12>>2]=271733878,1}(0|Ki(A|=0))},function(A){return 0|CB(0|Ki(A|=0))},function(A){var e;return A|=0,(e=0|Ln(12))?0|IB[31&t[A+8>>2]](e)?0|(A=e):(Xn(e),0|(A=0)):0|(A=e)},function(A){return(A|=0)?(t[A>>2]=52996,t[A+4>>2]=50832,t[A+8>>2]=0,0|(A=1)):0|(A=0)},function(A){return(A|=0)?($n(A),Xn(A),A=1):A=0,0|A},function(A){return(A|=0)?($n(A),A=1):A=0,0|A},Ne,function(A){var e=0,r=0;return e=A|=0,r=0|He(0|t[A+8>>2]),0|(r^=A=0|t[(A=e)>>2])},function(A){var e=0,r=0,i=0,n=0;switch(e=0|t[(A|=0)+4>>2],0|t[A>>2]){case 0:if(n=0|t[e+12>>2],r=0|t[e+16>>2],(0|n)>0){i=0,e=n<<20;do{e=(0|f[r+i>>0])<<(((3*i|0)>>>0)%24|0)^e,i=i+1|0}while((0|i)!=(0|n))}else e=n<<20;break;case 1:e=0|Ne(0|t[e>>2]);break;case 2:e=0|Ne(0|t[e+4>>2]);break;case 3:e=0|t[e+8>>2];break;default:return 0|(A=0)}return 0|(A=t[A>>2]<<30|1073741823&e)},function(A){var e;return e=(0|Ne(0|t[(A|=0)>>2]))<<2,(0|Ne(0|t[A+4>>2]))^e|0},Mf,Mf,Mf,Mf,Mf,Mf,Mf,Mf,Mf,Mf,Mf],gB=[hf,function(A,e){return A|=0,e|=0,e=0|qf(),t[A>>2]=e,0!=(0|e)|0},function(A,e){return A|=0,e|=0,e=0|$f(),t[A>>2]=e,0!=(0|e)|0},function(A,e){return e|=0,t[(A|=0)>>2]=t[e+20>>2],1},function(A,e){A|=0,e|=0;var r=0,i=0;if(!(r=0|Sn(24,82796,0)))return 0|(A=0);if(t[r+8>>2]=4,i=0|oe(),t[r+20>>2]=i,!i)return Xn(r,82796,0),0|(A=0);if(t[A+20>>2]=r,t[A+36>>2]=0,e=0|si(e),i=0|si(A),t[i>>2]=t[e>>2],0|function(A,e){e|=0;var r=0,i=0,n=0;if(0|ue(A|=0)&&0|hi(0|t[A+8>>2],0|t[e+8>>2])&&0|hi(0|t[A+12>>2],0|t[e+12>>2])&&0|hi(0|t[A+4>>2],0|t[e+4>>2])){i=e+20|0,n=(r=A+20|0)+128|0;do{t[r>>2]=t[i>>2],r=r+4|0,i=i+4|0}while((0|r)<(0|n));return t[A+16>>2]=t[e+16>>2],t[A>>2]=t[e>>2],0|(n=1)}return Ci(0|t[A+8>>2]),Ci(0|t[A+12>>2]),Ci(0|t[A+4>>2]),t[A>>2]=0,t[A+16>>2]=0,$r(A+20|0,128),0|(n=0)}(0|t[i+20>>2],0|t[e+20>>2])){if(!(r=0|t[e+12>>2]))return 0|(A=1);if(0|MA(i+4|0,r,0|t[e+4>>2]))return 0|(A=1)}return(r=0|si(A))?(le(0|t[r+20>>2]),pn(0|t[r+12>>2],0|t[r+4>>2],82796,0),Xn(r,82796,0),ki(A,0),0|(A=0)):0|(A=0)},function(A,e){return e|=0,A=0|t[(A|=0)+20>>2],0|t[A+12>>2]&&(A=0|function(A){return 0|lA(A|=0)}(A+4|0))?(Yi(e,855,A),0|(e=1)):0|(e=0)},function(A,e){return e|=0,function(A,e){e|=0,ji(0|t[(A|=0)+8>>2],e),ji(0|t[A+12>>2],e),ji(0|t[A+4>>2],e)}(A=0|t[20+(0|t[(A|=0)+20>>2])>>2],0|xi(e,-257)),ji(e,256),function(A,e){e|=0,t[(A|=0)+20>>2]=e}(e,15),1},function(A,e){return e|=0,0|DA(A=0|Ri(A|=0),0|Ri(e))},function(A,e){e|=0;var r=0;A=0|Ri(A|=0);do{if(e){if(0|(r=0|t[e>>2])){If(0|r,0|t[A+8>>2],0|t[A>>2]),t[e>>2]=(0|t[e>>2])+(0|t[A>>2]);break}if(r=0|Ln(0|t[A>>2],82796,0),t[e>>2]=r,r){If(0|r,0|t[A+8>>2],0|t[A>>2]);break}return 0|(e=-1)}}while(0);return 0|(e=0|t[A>>2])},function(A,e){var r;A|=0,e|=0,r=G,G=G+16|0;do{if(0|Bi(0,r+4|0,r,0,e)){if(e=0|br(0,r+4|0,0|t[r>>2])){Yi(A,6,e),e=1;break}Xe(4,139,4,0,0),e=0;break}e=0}while(0);return G=r,0|e},function(A,e){var r;return A|=0,e|=0,r=G,G=G+16|0,t[r>>2]=0,(0|(e=0|function(A,e){return 0|TA(A|=0,e|=0,52548)}(0|t[e+24>>2],r)))<1?(G=r,0|(A=0)):0|function(A,e,r,i,n,f){if(e|=0,r|=0,i|=0,n|=0,f|=0,!(0|yA(0|t[(A|=0)>>2],e,r,i)))return 0|(i=0);if(!n)return 0|(i=1);return Xn(0|t[8+(0|t[A+4>>2])>>2],82796,0),i=0|t[A+4>>2],t[i+8>>2]=n,t[i>>2]=f,t[i+12>>2]=-16&t[i+12>>2]|8,0|(i=1)}(A,0|Sr(6),5,0,0|t[r>>2],e)?(G=r,0|1):(Xn(0|t[r>>2],82796,0),G=r,0|0)},function(A,e){return A|=0,0|ca(0|t[16+(0|t[(e|=0)+24>>2])>>2],0|t[16+(0|t[A+24>>2])>>2])?0:0==(0|ca(0|t[20+(0|t[e+24>>2])>>2],0|t[20+(0|t[A+24>>2])>>2]))|0},function(A,e){var r;A|=0,e|=0,r=G,G=G+16|0;do{if(0|PA(0,r+4|0,r,0,e)){if(e=0|Qr(0,r+4|0,0|t[r>>2])){Yi(A,6,e),e=1;break}Xe(4,147,4,0,0),e=0;break}e=0}while(0);return G=r,0|e},function(A,e){var r;return A|=0,e|=0,r=G,G=G+16|0,t[r>>2]=0,(0|(e=0|wr(0|t[e+24>>2],r)))<1?(Xe(4,138,65,0,0),G=r,0|(A=0)):0|OA(A,0|Sr(6),0,5,0,0|t[r>>2],e)?(G=r,0|1):(Xe(4,138,65,0,0),G=r,0|0)},function(A,e){return e|=0,0|wr(0|t[(A|=0)+24>>2],e)},function(A,e){A|=0,e|=0;var r=0;return(r=0|Sn(44,82796,0))?(t[r>>2]=2048,t[r+16>>2]=1,t[r+28>>2]=-2,t[A+20>>2]=r,t[A+32>>2]=r+8,t[A+36>>2]=2,A=0|t[e+20>>2],t[r>>2]=t[A>>2],0|(e=0|t[A+4>>2])&&(e=0|ea(e),t[r+4>>2]=e,0==(0|e))?0|(r=0):(t[r+16>>2]=t[A+16>>2],t[r+20>>2]=t[A+20>>2],t[r+24>>2]=t[A+24>>2],0|t[A+36>>2]?(Xn(0|t[r+36>>2],82796,0),e=0|Ct(0|t[A+36>>2],0|t[A+40>>2],82796,0),t[r+36>>2]=e,e?(t[r+40>>2]=t[A+40>>2],0|(r=1)):0|(r=0)):0|(r=1))):0|(r=0)},function(A,e){e|=0;var r,i,n=0;if(r=0|t[(A|=0)+20>>2],!(0|t[r+4>>2])){if(n=0|qf(),t[r+4>>2]=n,!n)return 0|(e=0);if(!(0|ta(n,65537)))return 0|(e=0)}if(!(i=0|kr()))return 0|(e=0);do{if(0|t[A+28>>2]){if(0|(n=0|Ma())){qi(n,A),A=n;break}return sr(i),0|(e=0)}A=0}while(0);return n=0|er(i,0|t[r>>2],0|t[r+4>>2],A),function(A){if(!(A|=0))return;Xn(A,82796,0)}(A),(0|n)>0?(Yi(e,6,i),0|(e=n)):(sr(i),0|(e=n))},function(A,e){return 0|Qe(e|=0,0|Ki(A|=0))},function(A,e){return 0|sB(e|=0,0|Ki(A|=0))},function(A,e){return e|=0,me(0|t[(A|=0)+8>>2],4,e),1},function(A,e){return e|=0,1&n[(0|t[(A|=0)+4>>2])+((255&e)<<1)>>1]|0},function(A,e){return 0,((e|=0)<<24>>24)-48|0},function(A,e){var r,n,f=0,a=0,B=0,o=0,u=0,l=0,c=0,E=0,Q=0;if(n=G,G=G+48|0,!(r=0|et(e|=0,0|lt(A|=0))))return Xe(13,223,172,0,0),G=n,0|(Q=0);if((0|Hn(r))<=0)return G=n,0|(Q=1);for(Q=0;;){E=0|vn(r,Q),c=0|t[E+8>>2],E=0|t[E+4>>2],t[n+32>>2]=0,l=(e=0|jr(E))||(e=0|xr(E))?7:21;A:do{if(7==(0|l))if(l=0,A=0|ti(c)){do{if((0|Hn(A))>0){l=0,B=0,a=-1,f=-1;e:for(;;){u=0|vn(A,l),o=0|t[u+4>>2];do{if(0|Wt(o,53937)){if(!(0|Wt(o,53941))){if(a=0|St(0|t[u+8>>2],n+36|0,0),0|i[t[n+36>>2]>>0]){l=20;break e}break}if(!(0|Wt(o,53945))){if((o=0!=(0|dA(0|t[u+8>>2],n+32|0)))&0!=(0|t[n+32>>2]))break;l=20;break e}if(0|Wt(o,53950)){l=22;break e}if(0|Wt(B=0|t[u+8>>2],53956)){if(0|Wt(B,55355)){l=20;break e}B=1}else B=2}else if(f=0|St(0|t[u+8>>2],n+36|0,0),0|i[t[n+36>>2]>>0]){l=20;break e}}while(0);if((0|(l=l+1|0))>=(0|Hn(A))){l=25;break}}if(20==(0|l))l=0,Xe(13,222,218,0,0);else if(22==(0|l)){if(l=0,Xe(13,222,218,0,0),!u){l=24;break A}}else if(25==(0|l)){l=0,o=0|t[n+32>>2];break}E=0|t[u+4>>2],e=0|t[u+8>>2],t[n>>2]=53963,t[n+4>>2]=E,t[n+8>>2]=80764,t[n+12>>2]=e,Ke(4,n),e=0;break A}o=0,B=0,a=-1,f=-1}while(0);(e=0|QA(e,f,a,o,B))||(Xe(13,222,65,0,0),e=0)}else l=21}while(0);if(21==(0|l)&&(Xe(13,222,218,0,0),A=0,l=24),24==(0|l)&&(l=0,t[n+16>>2]=80644,t[n+16+4>>2]=E,t[n+16+8>>2]=80764,t[n+16+12>>2]=c,Ke(4,n+16|0),e=0),mn(A,8),!e)break;if((0|(Q=Q+1|0))>=(0|Hn(r))){e=1,l=31;break}}return 31==(0|l)?(G=n,0|e):(Xe(13,223,219,0,0),G=n,0|(Q=0))},function(A,e){return e|=0,(0|t[t[(A|=0)>>2]>>2])-(0|t[t[e>>2]>>2])|0},function(A,e){return e|=0,(0|t[(A|=0)>>2])-(0|t[e>>2])|0},function(A,e){var r,n=0,a=0,B=0,o=0,u=0;if(!(r=0|et(e|=0,0|lt(A|=0))))return Xe(13,174,172,0,0),0|(u=0);if((0|Hn(r))<=0)return 0|(u=1);o=0;A:for(;;){if(a=0|vn(r,o),B=0|t[a+8>>2],a=0|t[a+4>>2],0|(e=0|ef(B,44))){if(!((A=0|i[e+1>>0])<<24>>24))break;if(0|Yt(255&A))for(A=e+2|0;;){if(!(0|Yt(0|f[A>>0]))){n=0;break}A=A+1|0}else n=0,A=e+1|0}else n=a,A=B;if(0==(0|(n=0|qr(A,a,n)))|0!=(0|e)^1){if(!n)break}else{for(;0|Yt(0|f[B>>0]);)B=B+1|0;if(0|Yt(0|f[e+-1>>0]))for(e=e+-1|0;;){if((0|e)==(0|B))break A;if(!(0|Yt(0|f[(A=e+-1|0)>>0])))break;e=A}if(!(e=0|Ln((A=e-B|0)+1|0,82796,0)))break;If(0|e,0|B,0|A),i[e+A>>0]=0,t[4+(0|Sr(n))>>2]=e}if((0|(o=o+1|0))>=(0|Hn(r))){e=1,u=22;break}}return 22==(0|u)?0|e:(Xe(13,174,171,0,0),0|(u=0))},function(A,e){return e|=0,(0|t[t[(A|=0)>>2]>>2])-(0|t[t[e>>2]>>2])|0},function(A,e){var r,i;return e|=0,i=0|t[(A|=0)+4>>2],r=0|t[e+4>>2],0|(0==(0|(e=0|Ht(0|t[A>>2],0|t[e>>2],(0|i)<(0|r)?i:r)))?i-r|0:e)},Wt,function(A,e){e|=0;var r=0;return 0|(r=(0|t[(A|=0)>>2])-(0|t[e>>2])|0)?0|r:0|(r=0|nf(0|t[A+8>>2],0|t[e+8>>2]))},function(A,e){e|=0;var r,i=0,n=0;i=(r=0|t[(A|=0)>>2])-(0|t[e>>2])|0;A:do{if(!i)switch(n=0|t[A+4>>2],e=0|t[e+4>>2],0|r){case 0:if(0|(i=(A=0|t[n+12>>2])-(0|t[e+12>>2])|0))break A;i=0|Ht(0|t[n+16>>2],0|t[e+16>>2],A);break A;case 1:if(!(A=0|t[n>>2])){i=-1;break A}if(!(i=0|t[e>>2])){i=1;break A}i=0|Wt(A,i);break A;case 2:if(!(A=0|t[n+4>>2])){i=-1;break A}if(!(i=0|t[e+4>>2])){i=1;break A}i=0|Wt(A,i);break A;case 3:i=(0|t[n+8>>2])-(0|t[e+8>>2])|0;break A;default:i=0;break A}}while(0);return 0|i},function(A,e){return e|=0,(0|t[t[(A|=0)>>2]>>2])-(0|t[t[e>>2]>>2])|0},function(A,e){var r,i=0,n=0;if(r=G,G=G+32|0,!(A=0|et(e|=0,0|lt(A|=0))))return Xe(6,177,165,0,0),G=r,0|(i=0);if((0|Hn(A))<=0)return G=r,0|(i=1);for(i=0;;){if(e=0|vn(A,i),0|Wt(0|t[e+4>>2],80604))Xe(6,177,169,0,0),n=0|t[e+4>>2],e=0|t[e+8>>2],t[r>>2]=80644,t[r+4>>2]=n,t[r+8>>2]=80764,t[r+12>>2]=e,Ke(4,r);else{if(!(0|ni(e,r+16|0))){A=7;break}if((0|t[r+16>>2])>0){A=9;break}}if((0|(i=i+1|0))>=(0|Hn(A))){e=1,A=13;break}}return 7==(0|A)?(Xe(6,177,168,0,0),G=r,0|(n=0)):9==(0|A)?(Xe(6,177,167,0,0),G=r,0|(n=0)):13==(0|A)?(G=r,0|e):0},function(A,e){var r,i,n=0,f=0,a=0,B=0,o=0,u=0,l=0,c=0,E=0,Q=0;i=G,G=G+32|0,r=0|et(e|=0,n=0|lt(A|=0));A:do{if((0|Hn(r))<1)Xe(14,123,r?119:120,0,0),t[i>>2]=80635,t[i+4>>2]=n,Ke(2,i);else{if(n=0|Sn(12*(o=0|Hn(r))|0,82796),t[20562]=n,t[20563]=o,!o)return G=i,0|(A=1);a=0|vn(r,0),f=0|et(e,0|t[a+8>>2]);e:do{if(!((0|Hn(f))<1)){for(o=0;;){if(B=0|kt(0|t[a+4>>2],82796,0),t[n+(12*o|0)>>2]=B,!B)break A;if(u=0|Sn((a=0|Hn(f))<<3,82796),t[(B=n+(12*o|0)+4|0)>>2]=u,!u)break A;if(t[n+(12*o|0)+8>>2]=a,0|a){n=0;do{if(l=0|vn(f,n),c=0|t[B>>2],E=0|qt(Q=0|t[l+4>>2],46),t[(u=c+(n<<3)|0)>>2]=0|kt(0==(0|E)?Q:E+1|0,82796,0),l=0|kt(0|t[l+8>>2],82796,0),t[c+(n<<3)+4>>2]=l,n=n+1|0,!(0!=(0|l)&0!=(0|t[u>>2])))break A}while(n>>>0<a>>>0)}if((o=o+1|0)>>>0>=(0|t[20563])>>>0){n=1;break}if(n=0|t[20562],a=0|vn(r,o),(0|Hn(f=0|et(e,0|t[a+8>>2])))<1){B=a,n=a+8|0;break e}}return G=i,0|n}B=a,n=a+8|0}while(0);Xe(14,123,f?117:118,0,0),E=0|t[B+4>>2],Q=0|t[n>>2],t[i+8>>2]=80644,t[i+8+4>>2]=E,t[i+8+8>>2]=80764,t[i+8+12>>2]=Q,Ke(4,i+8|0)}}while(0);return jn(A),G=i,0|(Q=0)},function(A,e){e|=0;var r=0,i=0,n=0;if((0|(r=0|t[(A|=0)>>2]))!=(0|(i=0|t[e>>2]))&&0|(n=0|Wt(r,i)))return 0|(e=n);if(i=0|t[A+4>>2],r=0|t[e+4>>2],i){if(r)return 0|(e=0|Wt(i,r));r=0}return 0|((0|i)==(0|r)?0:0==(0|i)?-1:1)},hf,hf,hf,hf,hf,hf,hf,hf,hf,hf,hf,hf,hf,hf,hf,hf,hf,hf,hf,hf,hf,hf,hf,hf,hf,hf,hf,hf,hf],DB=[Uf,function(A,e,r){e|=0,r|=0;var n,f,a,B,u=0,l=0,c=0,E=0;if(B=G,G=G+144|0,f=0|t[(A|=0)+20>>2],!(u=0|t[f>>2]))return G=B,0|(E=0);if(!(A=0|t[f+12>>2]))return G=B,0|(E=0);if(a=0|t[f+1044>>2],n=0|t[r>>2],0|function(A,e,r,i,n,f,a){A|=0,e|=0,r|=0,i|=0,n|=0,a|=0;var B,o;o=G,G=G+80|0,B=0==(0|(f|=0))?81904:f,f=0|Sn(148,82796,0);do{if(f){if(!(0|ue(f))){le(f),f=0;break}if(0!=(0|(A=0==(0|e)&0==(0|r)?0|fe(f,82796,0,A,0):0|fe(f,e,r,A,0)))&&0!=(0|t[f>>2])&&0!=(0|Di(0|t[f+4>>2],i,n))&&0|t[f>>2]&&0|Mi(0|t[f+4>>2],o,o+64|0)&&0|hi(0|t[f+4>>2],0|t[f+12>>2])&&0|Di(0|t[f+4>>2],o,0|t[o+64>>2])&&0|Mi(0|t[f+4>>2],B,a))return Ci(0|t[f+8>>2]),Ci(0|t[f+12>>2]),Ci(0|t[f+4>>2]),t[f>>2]=0,t[f+16>>2]=0,$r(f+20|0,128),Ii(0|t[f+8>>2]),Ii(0|t[f+12>>2]),Ii(0|t[f+4>>2]),Xn(f,82796,0),G=o,0|(n=B)}else f=0}while(0);return le(f),G=o,0|(n=0)}(u,0|t[f+4>>2],0|t[f+8>>2],A,0|t[f+16>>2],B,B+64|0))if(A=0|t[B+64>>2],(c=((n>>>0)/((l=0|pi(u))>>>0)|0)+(0!=(n-(0|o((n>>>0)/(l>>>0)|0,l))|0)&1)|0)>>>0<=255&&0!=(0|(E=0|oe()))){A:do{if(0|fe(E,B,A,u,0)){if(c){for(r=0,u=1;;){if(i[B+128>>0]=u,u>>>0>1){if(!(0|fe(E,0,0,0,0)))break;if(!(0|ae(E,B+64|0,l)))break}if(!(0|ae(E,f+20|0,a)))break;if(!(0|ae(E,B+128|0,1)))break;if(A=(r+l|0)>>>0>n>>>0?n-r|0:l,0==(0|Be(E,B+64|0,0)))break;if(If(e+r|0,B+64|0,0|A),(u=u+1|0)>>>0>c>>>0)break A;r=A+r|0}e=0}}else e=0}while(0);$r(B+64|0,64),le(E)}else e=0;else e=0;return G=B,0|0!=(0|e)&1},function(A,e,r){return A|=0,r|=0,0|Wt(e|=0,53817)?0|Wt(e,53796)?0|Wt(e,53801)?0|Wt(e,54589)?0|Wt(e,54593)?0|Wt(e,65373)?0|Wt(e,53809)?0|(r=-2):0|(r=0|bi(A,4102,r)):0|(r=0|wi(A,4102,r)):0|(r=0|bi(A,4101,r)):0|(r=0|wi(A,4101,r)):0|(r=0|bi(A,4100,r)):0|(r=0|wi(A,4100,r)):0|(r=0|Qi(A,-1,1024,4099,0,0|Xi(r)))},function(A,e,r){e|=0,r|=0;var n,f=0,a=0,B=0,o=0;if(n=0|t[(A|=0)+20>>2],0|(A=0|t[n>>2])&&0|(B=0|t[n+4>>2])&&0|(o=0|t[n+1036>>2])){if(f=0|t[n+8>>2],a=0|t[r>>2],114!=(0|Si(A)))return 0|(e=0!=(0|F(A,B,f,n+12|0,o,e,a))&1);if(!(0|F(52868,B,(f>>>1)+(1&f)|0,n+12|0,o,e,a)))return 0|(e=0);if(!(r=0|Ln(a,82796,0)))return 0|(e=0);if(!(0|F(52916,B+(f>>>1)|0,(f>>>1)+(1&f)|0,n+12|0,o,r,a)))return pn(r,a,82796,0),0|(e=0);if(0|a){A=0;do{i[(o=e+A|0)>>0]=i[o>>0]^i[r+A>>0],A=A+1|0}while((0|A)!=(0|a))}return pn(r,a,82796,0),0|(e=1)}return Xe(52,101,101,0,0),0|(e=0)},function(A,e,r){return A|=0,e|=0,(r|=0)?0|Wt(e,53817)?0|Wt(e,53820)?0|Wt(e,53827)?0|Wt(e,53837)?0|Wt(e,53842)?0|(r=-2):0|(r=0|bi(A,4098,r)):0|(r=0|wi(A,4098,r)):0|(r=0|bi(A,4097,r)):0|(r=0|wi(A,4097,r)):(e=0|t[A+20>>2],(A=0|Xi(r))?(t[e>>2]=A,0|(r=1)):(Xe(52,100,100,0,0),0|(r=0))):(Xe(52,100,102,0,0),0|(r=0))},function(A,e,r){A|=0,e|=0,r|=0;do{if(r){if(!(0|Wt(e,54589))){A=0|wi(A,6,r);break}A=0|Wt(e,54593)?-2:0|bi(A,6,r)}else A=0}while(0);return 0|A},function(A,e,r){var i;return A|=0,e|=0,r|=0,0|(i=0|EA(4))&&0|MA(i,0|t[e>>2],r)&&0|Yi(A,855,i)?0|(A=1):(eA(i),0|(A=0))},function(A,e,r){return A|=0,(r=0|Qr(0,e|=0,r|=0))?(Yi(A,6,r),0|(A=1)):(Xe(4,147,4,0,0),0|(A=0))},function(A,e,r){var i;if(A|=0,e|=0,i=G,G=G+16|0,!(r|=0))return Xe(4,144,147,0,0),G=i,0|(A=0);if(!(0|Wt(e,55331))){if(0|Wt(r,59750))if(0|Wt(r,55348))if(0|Wt(r,55355))if(0!=(0|Wt(r,55360))&&0!=(0|Wt(r,55365)))if(0|Wt(r,55370)){if(0|Wt(r,65006))return Xe(4,144,118,0,0),G=i,0|(A=-2);e=6}else e=5;else e=4;else e=3;else e=2;else e=1;return A=0|Qi(A,6,-1,4097,e,0),G=i,0|A}return 0|Wt(e,55375)?0|Wt(e,55391)?0|Wt(e,55407)?0|Wt(e,55425)?0|Wt(e,55437)?0|Wt(e,55449)?(G=i,0|(A=-2)):((r=0|Dt(r,i))?(0|(e=0|Qi(A,6,768,4106,0|t[i>>2],r)))<1&&Xn(r,82796,0):e=0,G=i,0|(A=e)):(e=0|Xi(r))?(A=0|Qi(A,6,768,4105,0,e),G=i,0|A):(Xe(4,144,157,0,0),G=i,0|(A=0)):(e=0|Xi(r))?(A=0|Qi(A,6,1016,4101,0,e),G=i,0|A):(Xe(4,144,157,0,0),G=i,0|(A=0)):(t[i>>2]=0,0|Bn(i,r)?(0|(e=0|Qi(A,6,4,4100,0,0|t[i>>2])))<1&&Pf(0|t[i>>2]):e=0,G=i,0|(A=e)):(A=0|Qi(A,6,4,4099,0|tf(r),0),G=i,0|A):(A=0|Qi(A,6,24,4098,0|tf(r),0),G=i,0|A)},function(A,e,r){return 0|Df(0|(A|=0),0|(e|=0),0|(r|=0))},function(A,e,r){return e|=0,r|=0,0|ce(0|Ki(A|=0),e,r)},function(A,e,r){return e|=0,r|=0,0|bB(0|Ki(A|=0),e,r)},Et,function(A,e,r){return A|=0,e|=0,r|=0,0|(e=0)?(r=0|Et(A,e,r),ke(e),0|r):128==(4095&(0|pe())|0)?(Xe(14,120,114,0,0),0|(r=0)):(Xe(14,120,2,0,0),0|(r=0))},function(A,e,r){e|=0,r|=0;var i=0,n=0,f=0;if(!(A|=0))return 0|(r=0);A:do{switch(0|e){case 3:if(!(0|Af(A,54471,3)))return t[r>>2]=10502|t[r>>2],0|(r=1);for(i=0,e=544;;){if(3==(0|t[e+4>>2])&&0==(0|Af(0|t[e>>2],A,3)))break A;if(i=i+1|0,e=e+12|0,t[20510]=e,i>>>0>=49){e=0;break}}return 0|e;case-1:n=0|mt(A),f=7;break;default:n=e,f=7}}while(0);A:do{if(7==(0|f)){for(i=0,e=544;;){if((0|n)==(0|t[e+4>>2])&&0==(0|Af(0|t[e>>2],A,n)))break A;if(i=i+1|0,e=e+12|0,t[20510]=e,i>>>0>=49){e=0;break}}return 0|e}}while(0);return 0!=(0|(e=0|t[e+8>>2]))&0==(65536&e|0)&&(e=0|function(A){A=(A|=0)>>>0>30?0:0|t[1136+(A<<2)>>2];return 0|A}(e))?(t[r>>2]=t[r>>2]|e,0|(r=1)):0|(r=0)},function(A,e,r){return e|=0,r|=0,0!=(0|ae(0|t[20+(0|t[20+(0|zi(A|=0))>>2])>>2],e,r))|0},function(A,e,r){return 0|se(r|=0,A|=0,e|=0)},function(A,e,r){var i;return A|=0,e|=0,r=0|function(A){return 0|t[(A|=0)+4>>2]}(r|=0),i=0|t[r+32>>2],t[i>>2]=A,t[i+4>>2]=e,0|IB[31&t[r+28>>2]](r)},on,cn,Ln,Uf,Uf,Uf,Uf,Uf,Uf,Uf,Uf,Uf,Uf,Uf],MB=[Gf,function(A,e,r,i){e|=0,r|=0,i|=0;var n,f=0;switch(n=0|t[(A|=0)+20>>2],0|e){case 4099:i?(t[n>>2]=i,A=1):A=0;break;case 4100:0==(0|r)|0==(0|i)?A=1:(0|r)>=0?(0|(A=0|t[n+4>>2])&&pn(A,0|t[n+8>>2],82796,0),i=0|Ct(i,r,82796,0),t[n+4>>2]=i,i?(t[n+8>>2]=r,A=1):A=0):A=0;break;case 4101:(0|r)>=0?(0|(A=0|t[n+12>>2])&&pn(A,0|t[n+16>>2],82796,0),i=0|Ct(i,r,82796,0),t[n+12>>2]=i,i?(t[n+16>>2]=r,A=1):A=0):A=0;break;case 4102:0==(0|r)|0==(0|i)?A=1:(0|r)>=0&&(1024-(f=0|t[n+1044>>2])|0)>=(0|r)?(If(n+20+f|0,0|i,0|r),t[n+1044>>2]=(0|t[n+1044>>2])+r,A=1):A=0;break;default:A=-2}return 0|A},function(A,e,r,i){var n;switch(e|=0,r|=0,i|=0,n=0|t[(A|=0)+20>>2],0|e){case 4096:return t[n>>2]=i,0|(i=1);case 4097:return(0|r)<0?0|(i=0):(0|(A=0|t[n+4>>2])&&pn(A,0|t[n+8>>2],82796,0),$r(n+12|0,0|t[n+1036>>2]),t[n+1036>>2]=0,i=0|Ct(i,r,82796,0),t[n+4>>2]=i,i?(t[n+8>>2]=r,0|(i=1)):0|(i=0));case 4098:return 0==(0|r)|0==(0|i)?0|(i=1):(0|r)<0||(1024-(A=0|t[n+1036>>2])|0)<(0|r)?0|(i=0):(If(n+12+A|0,0|i,0|r),t[n+1036>>2]=(0|t[n+1036>>2])+r,0|(i=1));default:return 0|(i=-2)}return 0},function(A,e,r,n){return e|=0,r|=0,n|=0,(n=0|t[(A|=0)>>2])?(r=0==(7&(0|Kf(n))|0),0|e&&(r&&(i[e>>0]=0,e=e+1|0),ua(n,e)),0|(A=((7+(0|Kf(n))|0)/8|0)+(1&r)|0)):0|(A=-1)},function(A,e,r,n){e|=0,r|=0,n|=0;var a,B,o=0;if((0|(o=f[(A|=0)>>0]|f[A+1>>0]<<8|f[A+2>>0]<<16|f[A+3>>0]<<24))==(0|t[n+20>>2]))return 0|(o=-1);if(B=0==(((r=o>>31^o)>>>31)+(r>>>0>1&1)+(0!=(0|r)&1)+(r>>>0>3&1)+(r>>>0>7&1)+(r>>>0>15&1)+(r>>>0>31&1)+(r>>>0>63&1)+(r>>>0>127&1)+(r>>>0>255&1)+(r>>>0>511&1)+(r>>>0>1023&1)+(r>>>0>2047&1)+(r>>>0>4095&1)+(r>>>0>8191&1)+(r>>>0>16383&1)+(r>>>0>32767&1)+(r>>>0>65535&1)+(r>>>0>131071&1)+(r>>>0>262143&1)+(r>>>0>524287&1)+(r>>>0>1048575&1)+(r>>>0>2097151&1)+(r>>>0>4194303&1)+(r>>>0>8388607&1)+(r>>>0>16777215&1)+(r>>>0>33554431&1)+(r>>>0>67108863&1)+(r>>>0>134217727&1)+(r>>>0>268435455&1)+(r>>>0>536870911&1)+(r>>>0>1073741823&1)&7|0),a=(r>>>31)+(r>>>0>1&1)+(0!=(0|r)&1)+(r>>>0>3&1)+(r>>>0>7&1)+(r>>>0>15&1)+(r>>>0>31&1)+(r>>>0>63&1)+(r>>>0>127&1)+(r>>>0>255&1)+(r>>>0>511&1)+(r>>>0>1023&1)+(r>>>0>2047&1)+(r>>>0>4095&1)+(r>>>0>8191&1)+(r>>>0>16383&1)+(r>>>0>32767&1)+(r>>>0>65535&1)+(r>>>0>131071&1)+(r>>>0>262143&1)+(r>>>0>524287&1)+(r>>>0>1048575&1)+(r>>>0>2097151&1)+(r>>>0>4194303&1)+(r>>>0>8388607&1)+(r>>>0>16777215&1)+(r>>>0>33554431&1)+(r>>>0>67108863&1)+(r>>>0>134217727&1)+(r>>>0>268435455&1)+(r>>>0>536870911&1)+(r>>>0>1073741823&1)+7>>3,0|e&&(B?(i[e>>0]=o>>31,A=e+1|0):A=e,((r>>>31)+(r>>>0>1&1)+(0!=(0|r)&1)+(r>>>0>3&1)+(r>>>0>7&1)+(r>>>0>15&1)+(r>>>0>31&1)+(r>>>0>63&1)+(r>>>0>127&1)+(r>>>0>255&1)+(r>>>0>511&1)+(r>>>0>1023&1)+(r>>>0>2047&1)+(r>>>0>4095&1)+(r>>>0>8191&1)+(r>>>0>16383&1)+(r>>>0>32767&1)+(r>>>0>65535&1)+(r>>>0>131071&1)+(r>>>0>262143&1)+(r>>>0>524287&1)+(r>>>0>1048575&1)+(r>>>0>2097151&1)+(r>>>0>4194303&1)+(r>>>0>8388607&1)+(r>>>0>16777215&1)+(r>>>0>33554431&1)+(r>>>0>67108863&1)+(r>>>0>134217727&1)+(r>>>0>268435455&1)+(r>>>0>536870911&1)+(r>>>0>1073741823&1)|0)>0))for(n=a;e=n,i[A+(n=n+-1|0)>>0]=r^o>>31,!((0|e)<=1);)r>>>=8;return 0|(o=a+(1&B)|0)},function(A,e,r,i){var n,f;return e|=0,r|=0,i|=0,f=G,G=G+16|0,n=0|t[(A|=0)+20>>2],(0|(A=0|pi(0|function(A){A=(A|=0)?0|t[A>>2]:0;return 0|A}(i))))>=0?(t[r>>2]=A,e?0|Be(0|t[n+20>>2],e,f)?(t[r>>2]=t[f>>2],A=1):A=0:A=1):A=0,G=f,0|A},function(A,e,r,i){var n;switch(e|=0,r|=0,i|=0,n=0|t[(A|=0)+20>>2],0|e){case 6:if((0|r)<-1|(0|r)>0&0==(0|i))return 0|(e=0);if(!(0|MA(n+4|0,i,r)))return 0|(e=0);break;case 1:t[n>>2]=i;break;case 7:if(e=0|t[24+(0|t[A+8>>2])>>2],!(0|fe(0|t[n+20>>2],0|t[e+8>>2],0|t[e>>2],0|t[n>>2],0|t[A+4>>2])))return 0|(e=0);break;default:return 0|(e=-2)}return 0|(e=1)},function(A,e,r,i){return 0,r|=0,i|=0,3==(0|(e|=0))?(t[i>>2]=672,r=1):r=-2,0|r},function(A,e,r,i){A|=0,e|=0,r|=0;var n,f,a,B=0,o=0,u=0;if(iB(i|=0),n=0|tB(i),f=0|tB(i),!(a=0|tB(i)))return r=0,nB(i),0|r;B=0|t[r+56>>2];do{if(4&B){if(!(B=0|qf()))return r=0,nB(i),0|r;if(Da(B,0|t[r+28>>2],4),0|pa(r+64|0,0|t[r+84>>2],B,i)&&(Da(B,0|t[r+32>>2],4),0|pa(r+68|0,0|t[r+84>>2],B,i))){Pf(B),o=4==(0|t[24+(0|t[r+8>>2])>>2])?(0|(o=0|Kf(0|t[r+32>>2])))==(0|Kf(0|t[r+28>>2])):0,B=0|t[r+56>>2],o&=1;break}return Pf(B),r=0,nB(i),0|r}o=0}while(0);if(2&B|0&&0==(0|pa(r+60|0,0|t[r+84>>2],0|t[r+16>>2],i)))return r=0,nB(i),0|r;if(o){if(!(0|Ja(f,e,0|t[r+68>>2],i)))return r=0,nB(i),0|r;if(!(0|Xa(f,f,0|t[r+68>>2],i)))return r=0,nB(i),0|r;if(!(0|_a(f,f,0|t[r+40>>2],0|t[r+32>>2],i,0|t[r+68>>2])))return r=0,nB(i),0|r;if(!(0|Ja(n,e,0|t[r+64>>2],i)))return r=0,nB(i),0|r;if(!(0|Xa(n,n,0|t[r+64>>2],i)))return r=0,nB(i),0|r;if(!(0|_a(n,n,0|t[r+36>>2],0|t[r+28>>2],i,0|t[r+64>>2])))return r=0,nB(i),0|r;if(!(0|function(A,e,r,i){A|=0,e|=0,r|=0;var n,f,a=0,B=0,o=0,u=0,l=0,c=0,E=0,Q=0,w=0;if(n=0|t[(i=i|0)+4>>2],!(0|ia(A,n)))return 0|(A=0);if(f=0|t[A>>2],B=0==(0|(B=0|t[e>>2]))?f:B,l=0==(0|(l=0|t[r>>2]))?f:l,0|n){for(o=0,u=0,c=0,a=0,E=0;w=a-(0|t[e+4>>2])>>31&t[B+(o<<2)>>2],Q=a-(0|t[r+4>>2])>>31&t[l+(u<<2)>>2],t[f+(a<<2)>>2]=w+E-Q,E=0-(c=(0|w)==(0|Q)?c:w>>>0<Q>>>0&1)|0,(0|(a=a+1|0))!=(0|n);)o=((a-(0|t[e+8>>2])|0)>>>31)+o|0,u=((a-(0|t[r+8>>2])|0)>>>31)+u|0;if(u=0|t[i>>2],0|n){a=0,B=0;do{w=(t[u+(B<<2)>>2]&E)+a|0,Q=w+(0|t[(i=f+(B<<2)|0)>>2])|0,t[i>>2]=Q,a=(Q>>>0<w>>>0&1)+(w>>>0<a>>>0&1)|0,B=B+1|0}while((0|B)!=(0|n));if(r=a-c|0,0|n)for(e=0,o=0;a=(t[u+(o<<2)>>2]&r)+e|0,B=a+(0|t[(w=f+(o<<2)|0)>>2])|0,t[w>>2]=B,(0|(o=o+1|0))!=(0|n);)e=(B>>>0<a>>>0&1)+(a>>>0<e>>>0&1)|0}}return t[A+4>>2]=n,t[A+12>>2]=0,0|(w=1)}(n,n,f,0|t[r+28>>2])))return r=0,nB(i),0|r;if(!(0|Xa(n,n,0|t[r+64>>2],i)))return r=0,nB(i),0|r;if(!(0|ma(n,n,0|t[r+44>>2],0|t[r+64>>2],i)))return r=0,nB(i),0|r;if(!(0|pf(A,n,0|t[r+32>>2],i)))return r=0,nB(i),0|r;if(!(0|function(A,e,r,i){A|=0,e|=0,r|=0;var n,f,a=0,B=0,o=0,u=0,l=0,c=0,E=0,Q=0,w=0;if(f=G,G=G+128|0,n=0|t[(i=i|0)+4>>2],!(0|ia(A,n)))return G=f,0|(A=0);if(n>>>0>32){if(!(a=0|Ln(n<<2,82796,0)))return G=f,0|(A=0)}else a=f;if(E=0==(0|(E=0|t[e>>2]))?a:E,c=0==(0|(c=0|t[r>>2]))?a:c,n)for(u=0,l=0,B=0,o=0;Q=(o-(0|t[e+4>>2])>>31&t[E+(u<<2)>>2])+B|0,w=(o-(0|t[r+4>>2])>>31&t[c+(l<<2)>>2])+Q|0,t[a+(o<<2)>>2]=w,B=(w>>>0<Q>>>0&1)+(Q>>>0<B>>>0&1)|0,(0|(o=o+1|0))!=(0|n);)u=((o-(0|t[e+8>>2])|0)>>>31)+u|0,l=((o-(0|t[r+8>>2])|0)>>>31)+l|0;else B=0;if(u=0|t[A>>2],o=B-(0|Un(u,a,0|t[i>>2],n))|0,0|n){B=0;do{w=a+(B<<2)|0,t[(Q=u+(B<<2)|0)>>2]=t[Q>>2]&~o|t[w>>2]&o,t[w>>2]=0,B=B+1|0}while((0|B)!=(0|n))}if(t[A+4>>2]=n,t[A+12>>2]=0,(0|a)==(0|f))return G=f,0|(w=1);return Xn(a,82796,0),G=f,0|(w=1)}(A,A,f,0|t[r+16>>2])))return r=0,nB(i),0|r}else{if(!(B=0|qf()))return r=0,nB(i),0|r;if(Da(B,e,4),!(0|wB(0,n,B,0|t[r+32>>2],i)))return Pf(B),r=0,nB(i),0|r;if(!(o=0|qf()))return Pf(B),r=0,nB(i),0|r;if(Da(o,0|t[r+40>>2],4),!(0|UB[7&t[24+(0|t[r+8>>2])>>2]](f,n,o,0|t[r+32>>2],i,0|t[r+68>>2])))return Pf(B),Pf(o),r=0,nB(i),0|r;if(Pf(o),o=0==(0|wB(0,n,B,0|t[r+28>>2],i)),Pf(B),o)return r=0,nB(i),0|r;if(!(B=0|qf()))return r=0,nB(i),0|r;if(Da(B,0|t[r+36>>2],4),o=0==(0|UB[7&t[24+(0|t[r+8>>2])>>2]](A,n,B,0|t[r+28>>2],i,0|t[r+64>>2])),Pf(B),o)return r=0,nB(i),0|r;if(!(0|cn(A,A,f)))return r=0,nB(i),0|r;if(0|Ia(A)&&0==(0|on(A,A,0|t[r+28>>2])))return r=0,nB(i),0|r;if(!(0|Sf(n,A,0|t[r+44>>2],i)))return r=0,nB(i),0|r;if(!(B=0|qf()))return r=0,nB(i),0|r;if(Da(B,n,4),o=0==(0|wB(0,A,B,0|t[r+28>>2],i)),Pf(B),o)return r=0,nB(i),0|r;if(0|Ia(A)&&0==(0|on(A,A,0|t[r+28>>2])))return r=0,nB(i),0|r;if(!(0|Sf(n,A,0|t[r+32>>2],i)))return r=0,nB(i),0|r;if(!(0|on(A,n,f)))return r=0,nB(i),0|r}if(0|(B=0|t[r+20>>2])&&0|(u=0|t[r+16>>2])){if(4==(0|t[24+(0|t[r+8>>2])>>2])){if(!(0|Pa(a,A,B,u,i,0|t[r+60>>2])))return r=0,nB(i),0|r}else if(aa(A),!(0|UB[7&t[24+(0|t[r+8>>2])>>2]](a,A,0|t[r+20>>2],0|t[r+16>>2],i,0|t[r+60>>2])))return r=0,nB(i),0|r;if(!(0|cn(a,a,e)))return r=0,nB(i),0|r;if(0|jf(a))return aa(A),r=1,nB(i),0|r;if(!(0|wB(0,a,a,0|t[r+16>>2],i)))return r=0,nB(i),0|r;if(0|Ia(a)&&0==(0|on(a,a,0|t[r+16>>2])))return r=0,nB(i),0|r;if(!(0|jf(a))){if(!(B=0|qf()))return r=0,nB(i),0|r;if(Da(B,0|t[r+24>>2],4),r=0==(0|UB[7&t[24+(0|t[r+8>>2])>>2]](A,e,B,0|t[r+16>>2],i,0|t[r+60>>2])),Pf(B),r)return r=0,nB(i),0|r}}return aa(A),r=1,nB(i),0|r},function(A,e,r,i){return r|=0,0,0|Ar(A|=0,0|t[(e|=0)+24>>2],r,0)},function(A,e,r,i){return r|=0,0,0|Ar(A|=0,0|t[(e|=0)+24>>2],r,1)},function(A,e,r,i){A|=0,e|=0,r|=0,i|=0;var n,f=0;switch(n=G,G=G+16|0,t[n>>2]=0,0|e){case 1:r?A=1:(!function(A,e,r,i){A|=0,r|=0,i|=0,0|(e|=0)&&(t[e>>2]=t[A+28>>2]);0|r&&(t[r>>2]=t[A+8>>2]);if(!i)return;t[i>>2]=t[A+16>>2]}(i,0,0,n),f=7);break;case 2:r?A=1:(!function(A,e){if(A|=0,!(e|=0))return;t[e>>2]=t[A+8>>2]}(i,n),f=7);break;case 3:t[i>>2]=672,A=1;break;default:A=-2}return 7==(0|f)&&((A=0|t[n>>2])?(yA(A,0|Sr(6),5,0),A=1):A=1),G=n,0|A},function(A,e,r,i){switch(e|=0,0,i|=0,0|(A|=0)){case 0:i=0|kr(),t[e>>2]=i,i=0==(0|i)?0:2;break;case 2:sr(0|t[e>>2]),t[e>>2]=0,i=2;break;default:i=1}return 0|i},function(A,e,r,i){var n;e|=0,r|=0,i|=0,n=0|t[(A|=0)+20>>2];A:do{switch(0|e){case 4097:e:do{if((r+-1|0)>>>0<6){if(!(0|Rr(0|t[n+20>>2],r))){A=0;break A}switch(0|r){case 6:if(!(24&t[A+16>>2]))break e;0|t[n+20>>2]||(t[n+20>>2]=52916);break;case 4:if(!(768&t[A+16>>2]))break e;0|t[n+20>>2]||(t[n+20>>2]=52916)}t[n+16>>2]=r,A=1;break A}}while(0);Xe(4,143,144,0,0),A=-2;break;case 4102:t[i>>2]=t[n+16>>2],A=1;break;case 4103:case 4098:if(6!=(0|t[n+16>>2])){Xe(4,143,146,0,0),A=-2;break A}if(4103==(0|e)){t[i>>2]=t[n+28>>2],A=1;break A}(0|r)<-2?A=-2:(t[n+28>>2]=r,A=1);break;case 4099:if((0|r)<512){Xe(4,143,120,0,0),A=-2;break A}t[n>>2]=r,A=1;break A;case 4100:if(0|i&&0|da(i)&&0==(0|sa(i))){Pf(0|t[n+4>>2]),t[n+4>>2]=i,A=1;break A}Xe(4,143,101,0,0),A=-2;break;case 4107:case 4105:if(4!=(0|t[n+16>>2])){Xe(4,143,141,0,0),A=-2;break A}if(4107==(0|e)){t[i>>2]=t[n+20>>2],A=1;break A}t[n+20>>2]=i,A=1;break A;case 1:0|Rr(i,0|t[n+16>>2])?(t[n+20>>2]=i,A=1):A=0;break;case 13:t[i>>2]=t[n+20>>2],A=1;break;case 4104:case 4101:switch(0|t[n+16>>2]){case 4:case 6:break;default:Xe(4,143,156,0,0),A=-2;break A}if(4104!=(0|e)){t[n+24>>2]=i,A=1;break A}if(A=0|t[n+24>>2]){t[i>>2]=A,A=1;break A}t[i>>2]=t[n+20>>2],A=1;break A;case 4106:if(4!=(0|t[n+16>>2])){Xe(4,143,141,0,0),A=-2;break A}if(Xn(0|t[n+36>>2],82796,0),(0|r)>0&0!=(0|i)){t[n+36>>2]=i,t[n+40>>2]=r,A=1;break A}t[n+36>>2]=0,t[n+40>>2]=0,A=1;break A;case 4108:if(4==(0|t[n+16>>2])){t[i>>2]=t[n+36>>2],A=0|t[n+40>>2];break A}Xe(4,143,141,0,0),A=-2;break A;case 5:case 4:case 3:case 7:A=1;break;case 2:Xe(4,143,148,0,0),A=-2;break;default:A=-2}}while(0);return 0|A},function(A,e,r,i){switch(e|=0,0,0,0|(A|=0)){case 3:return yi(0|t[8+(0|t[e>>2])>>2]),0|(e=1);case 5:return e=0|t[e>>2],yi(0|t[e+8>>2]),t[e+8>>2]=0,xe(),-1==(0|function(A,e){A|=0,e|=0;var r,i=0;if(!(r=0|Fi()))return Xe(11,148,65,0,0),0|(A=-1);do{if(0|Ni(r,0|zr(0|t[t[e>>2]>>2]))){if(!(i=0|t[20+(0|t[r+12>>2])>>2])){Xe(11,148,124,0,0);break}if(!(0|gB[63&i](r,e))){Xe(11,148,125,0,0);break}return t[A>>2]=r,0|(A=1)}Xe(11,148,111,0,0)}while(0);return yi(r),0|(A=0)}(e+8|0,e))?0|(e=0):(Oe(),0|(e=1));default:return 0|(e=1)}return 0},function(A,e,r,i){A|=0,r|=0,i|=0;var n,f=0;if(n=G,G=G+80|0,29==(0|(e|=0)))if(0!=(0|A)&&(f=0|Ki(A),48==(0|r))&&(0|bB(f,i,48))>=1){A=(e=n+32|0)+40|0;do{t[e>>2]=909522486,e=e+4|0}while((0|e)<(0|A));if(0!=(0|bB(f,n+32|0,40))&&0!=(0|sB(n,f))&&0!=(0|CB(f))&&(0|bB(f,i,48))>=1){A=(e=n+32|0)+40|0;do{t[e>>2]=1549556828,e=e+4|0}while((0|e)<(0|A));0!=(0|bB(f,n+32|0,40))&&0!=(0|bB(f,n,20))?($r(n,20),e=1):e=0}else e=0}else e=0;else e=-2;return G=n,0|e},Fa,function(A,e,r,i){return 0|Wa(1,A|=0,e|=0,r|=0,i|=0)},Tn,Gf,Gf,Gf,Gf,Gf,Gf,Gf,Gf,Gf,Gf,Gf,Gf,Gf],hB=[Zf,function(A,e,r,i,n){return 0,0,0,0|function(A,e){A|=0;var r=0,i=0,n=0,f=0,a=0;if(0|t[12+(e|=0)>>2]&&1!=(0|se(A,80624,1)))return 0|(A=0);if(0|jf(e)&&1!=(0|se(A,80614,1)))return 0|(A=0);if((0|(r=0|t[e+4>>2]))<=0)return 0|(A=1);i=0;for(;;){if(a=r,r=r+-1|0,(n=0|t[(0|t[e>>2])+(r<<2)>>2])>>>28|i){if(1!=(0|se(A,1312+(n>>>28)|0,1))){r=0,i=11;break}n=0|t[(0|t[e>>2])+(r<<2)>>2],i=1}if((f=n>>>24&15)|i){if(1!=(0|se(A,1312+f|0,1))){r=0,i=11;break}n=0|t[(0|t[e>>2])+(r<<2)>>2],i=1}if((f=n>>>20&15)|i){if(1!=(0|se(A,1312+f|0,1))){r=0,i=11;break}n=0|t[(0|t[e>>2])+(r<<2)>>2],i=1}if((f=n>>>16&15)|i){if(1!=(0|se(A,1312+f|0,1))){r=0,i=11;break}n=0|t[(0|t[e>>2])+(r<<2)>>2],i=1}if((f=n>>>12&15)|i){if(1!=(0|se(A,1312+f|0,1))){r=0,i=11;break}n=0|t[(0|t[e>>2])+(r<<2)>>2],i=1}if((f=n>>>8&15)|i){if(1!=(0|se(A,1312+f|0,1))){r=0,i=11;break}n=0|t[(0|t[e>>2])+(r<<2)>>2],i=1}if((f=n>>>4&15)|i){if(1!=(0|se(A,1312+f|0,1))){r=0,i=11;break}n=0|t[(0|t[e>>2])+(r<<2)>>2],i=1}if((n&=15)|i){if(1!=(0|se(A,1312+n|0,1))){r=0,i=11;break}i=1}if((0|a)<=1){r=1,i=11;break}}return 11==(0|i)?0|r:0}(A|=0,0|t[(e|=0)>>2])?0|(e=(0|Ce(A,80479))>0&1):0|(e=0)},function(A,e,r,i,n){return A|=0,e|=0,0,0,n|=0,n=G,G=G+16|0,t[n>>2]=t[e>>2],e=0|ge(A,54584,n),G=n,0|e},function(A,e,r,i,n){A|=0,e|=0,r|=0,n|=0;var f=0,a=0,B=0,o=0,u=0;if((0|Kf(0|t[(i|=0)+16>>2]))>16384)return Xe(4,104,105,0,0),0|(u=-1);if((0|la(0|t[i+16>>2],0|t[i+20>>2]))<1)return Xe(4,104,101,0,0),0|(u=-1);if((0|Kf(0|t[i+16>>2]))>3072&&(0|Kf(0|t[i+20>>2]))>64)return Xe(4,104,101,0,0),0|(u=-1);if(u=0|eB()){iB(u),B=0|tB(u),o=0|tB(u),a=0|Ln(f=(7+(0|Kf(0|t[i+16>>2]))|0)/8|0,82796,0);A:do{if(0==(0|B)|0==(0|o)|0==(0|a))Xe(4,104,65,0,0),A=-1;else{switch(0|n){case 1:A=0|Fr(a,f,e,A);break;case 4:A=0|rr(a,f,e,A,0,0);break;case 2:A=0|lr(a,f,e,A);break;case 3:A=0|Cr(a,f,e,A);break;default:Xe(4,104,118,0,0),A=-1;break A}if((0|A)>=1&&0!=(0|fa(a,f,B))){if((0|la(B,0|t[i+16>>2]))>-1){Xe(4,104,132,0,0),A=-1;break}if(2&t[i+56>>2]|0&&0==(0|pa(i+60|0,0|t[i+84>>2],0|t[i+16>>2],u))){A=-1;break}A=0|UB[7&t[24+(0|t[i+8>>2])>>2]](o,B,0|t[i+20>>2],0|t[i+16>>2],u,0|t[i+60>>2])?0|Ba(o,r,f):-1}else A=-1}}while(0);nB(u)}else a=0,f=0,A=-1;return rB(u),pn(a,f,82796,0),0|(u=A)},function(A,e,r,i,n){A|=0,e|=0,r|=0,n|=0;var f=0,a=0,B=0,o=0,u=0;if((0|Kf(0|t[(i|=0)+16>>2]))>16384)return Xe(4,103,105,0,0),0|(u=-1);if((0|la(0|t[i+16>>2],0|t[i+20>>2]))<1)return Xe(4,103,101,0,0),0|(u=-1);if((0|Kf(0|t[i+16>>2]))>3072&&(0|Kf(0|t[i+20>>2]))>64)return Xe(4,103,101,0,0),0|(u=-1);if(u=0|eB()){iB(u),f=0|tB(u),o=0|tB(u),B=0|Ln(a=(7+(0|Kf(0|t[i+16>>2]))|0)/8|0,82796,0);A:do{if(0==(0|f)|0==(0|o)|0==(0|B))Xe(4,103,65,0,0),f=-1;else{if((0|a)<(0|A)){Xe(4,103,108,0,0),f=-1;break}if(0|fa(e,A,f)){if((0|la(f,0|t[i+16>>2]))>-1){Xe(4,103,132,0,0),f=-1;break}if(2&t[i+56>>2]|0&&0==(0|pa(i+60|0,0|t[i+84>>2],0|t[i+16>>2],u))){f=-1;break}if(0|UB[7&t[24+(0|t[i+8>>2])>>2]](o,f,0|t[i+20>>2],0|t[i+16>>2],u,0|t[i+60>>2])){e:do{if(5==(0|n)){if(12!=(15&t[(0|En(o))>>2]|0)&&0==(0|cn(o,0|t[i+16>>2],o))){f=-1;break A}f=0|qe(r,a,B,0|Ba(o,B,a),a)}else switch(f=0|Ba(o,B,a),0|n){case 1:f=0|Nr(r,a,B,f,a);break e;case 3:If(0|r,0|B,0|f);break e;default:Xe(4,103,118,0,0),f=-1;break A}}while(0);(0|f)<0&&Xe(4,103,114,0,0)}else f=-1}else f=-1}}while(0);nB(u)}else B=0,a=0,f=-1;return rB(u),pn(B,a,82796,0),0|(u=f)},function(A,e,r,i,n){A|=0,e|=0,r|=0,i|=0,n|=0;var f,a,B=0,o=0,u=0,l=0,c=0,E=0,Q=0;if(!(a=0|eB()))return r=0,c=0,E=-1,rB(a),pn(r,c,82796,0),0|E;iB(a),f=0|tB(a),B=0|tB(a),c=0|Ln(E=(7+(0|Kf(0|t[i+16>>2]))|0)/8|0,82796,0);A:do{if(0==(0|f)|0==(0|B)|0==(0|c))Xe(4,102,65,0,0),B=-1;else{switch(0|n){case 1:A=0|Zr(c,E,e,A);break;case 5:A=0|_e(c,E,e,A);break;case 3:A=0|Cr(c,E,e,A);break;default:Xe(4,102,118,0,0),B=-1;break A}if((0|A)>=1&&0!=(0|fa(c,E,f))){if((0|la(f,0|t[i+16>>2]))>-1){Xe(4,102,132,0,0),B=-1;break}if(2&(A=0|t[i+56>>2])){if(!(0|pa(i+60|0,0|t[i+84>>2],0|t[i+16>>2],a))){B=-1;break}A=0|t[i+56>>2]}e:do{if(!(128&A)){R(0|t[i+84>>2]),(A=0|t[i+76>>2])?(e=A,l=20):(A=0|hr(i,a),t[i+76>>2]=A,A?(e=A,l=20):y(0|t[i+84>>2]));r:do{if(20==(0|l)){do{if(!(0|Cn(e))){if(A=0|t[i+80>>2])y(0|t[i+84>>2]);else if(A=0|hr(i,a),t[i+80>>2]=A,y(0|t[i+84>>2]),!A)break r;if(e=0|tB(a)){dn(A),o=0|kn(f,e,A,a),In(A);break}Xe(4,102,65,0,0),B=-1;break A}y(0|t[i+84>>2]),A=e,o=0|kn(f,0,e,a),e=0}while(0);if(!o){B=-1;break A}o=0|t[i+56>>2],u=1;break e}}while(0);Xe(4,102,68,0,0),B=-1;break A}o=A,A=0,u=0,e=0}while(0);do{if(!(32&o)){if(0|t[i+28>>2]&&0|t[i+32>>2]&&0|t[i+36>>2]&&0|t[i+40>>2]&&0|t[i+44>>2]){l=37;break}if(o=0|qf()){if(Da(o,0|t[i+24>>2],4),Q=0==(0|UB[7&t[24+(0|t[i+8>>2])>>2]](B,f,o,0|t[i+16>>2],a,0|t[i+60>>2])),Pf(o),Q){B=-1;break A}break}Xe(4,102,65,0,0),B=-1;break A}l=37}while(0);if(37==(0|l)&&0==(0|MB[31&t[20+(0|t[i+8>>2])>>2]](B,f,i,a))){B=-1;break}if(u&&0==(0|sn(B,e,A,a))){B=-1;break}if(5==(0|n)){if(!(0|cn(f,0|t[i+16>>2],B))){B=-1;break}B=(Q=(0|ca(B,f))>0)?f:B}B=0|Ba(B,r,E)}else B=-1}}while(0);return nB(a),Q=B,rB(a),pn(c,E,82796,0),0|Q},function(A,e,r,i,n){A|=0,e|=0,r|=0,i|=0,n|=0;var f,a,B,o=0,u=0,l=0,c=0,E=0,Q=0;if(!(B=0|eB()))return r=0,c=0,E=-1,rB(B),pn(r,c,82796,0),0|E;iB(B),f=0|tB(B),a=0|tB(B),c=0|Ln(E=(7+(0|Kf(0|t[i+16>>2]))|0)/8|0,82796,0);A:do{if(0==(0|f)|0==(0|a)|0==(0|c))Xe(4,101,65,0,0),A=-1;else{if((0|E)<(0|A)){Xe(4,101,108,0,0),A=-1;break}if(0|fa(e,A,f)){if((0|la(f,0|t[i+16>>2]))>-1){Xe(4,101,132,0,0),A=-1;break}A=0|t[i+56>>2];e:do{if(!(128&A)){R(0|t[i+84>>2]),(A=0|t[i+76>>2])?l=13:(A=0|hr(i,B),t[i+76>>2]=A,A?l=13:y(0|t[i+84>>2]));r:do{if(13==(0|l)){do{if(!(0|Cn(A))){if(A=0|t[i+80>>2])y(0|t[i+84>>2]);else if(A=0|hr(i,B),t[i+80>>2]=A,y(0|t[i+84>>2]),!A)break r;if(e=0|tB(B)){dn(A),u=0|kn(f,e,A,B),In(A),o=A,A=u;break}Xe(4,101,65,0,0),A=-1;break A}y(0|t[i+84>>2]),o=A,A=0|kn(f,0,A,B),e=0}while(0);if(!A){A=-1;break A}A=0|t[i+56>>2],u=1;break e}}while(0);Xe(4,101,68,0,0),A=-1;break A}o=0,u=0,e=0}while(0);do{if(32&A)l=30;else{if(0|t[i+28>>2]&&0|t[i+32>>2]&&0|t[i+36>>2]&&0|t[i+40>>2]&&0|t[i+44>>2]){l=30;break}if(!(A=0|qf())){Xe(4,101,65,0,0),A=-1;break A}if(Da(A,0|t[i+24>>2],4),2&t[i+56>>2]|0&&0==(0|pa(i+60|0,0|t[i+84>>2],0|t[i+16>>2],B))){Pf(A),A=-1;break A}if(Q=0==(0|UB[7&t[24+(0|t[i+8>>2])>>2]](a,f,A,0|t[i+16>>2],B,0|t[i+60>>2])),Pf(A),Q){A=-1;break A}}}while(0);if(30==(0|l)&&0==(0|MB[31&t[20+(0|t[i+8>>2])>>2]](a,f,i,B))){A=-1;break}if(u&&0==(0|sn(a,e,o,B))){A=-1;break}switch(A=0|Ba(a,c,E),0|n){case 1:A=0|Wr(r,E,c,A,E);break;case 4:A=0|tr(r,E,c,A,E,0,0);break;case 2:A=0|cr(r,E,c,A,E);break;case 3:If(0|r,0|c,0|A);break;default:Xe(4,101,118,0,0),A=-1;break A}Xe(4,101,114,0,0),Pe(A>>>31^1)}else A=-1}}while(0);return nB(B),Q=A,rB(B),pn(c,E,82796,0),0|Q},function(A,e,r,i,n){A|=0,r|=0,i|=0,n|=0;var f=0;if(912==(0|zr(0|t[(e|=0)>>2]))){if(f=0|S(52600,0|t[e+4>>2])){if(e=0!=(0|(n=0|t[f+4>>2]))&&911==(0|zr(0|t[n>>2]))?0|S(52132,0|t[n+4>>2]):0,(0|Ce(A,80479))<1)return Er(f),RA(e),0|(r=0);if(!(0|Ie(A,i,128)))return Er(f),RA(e),0|(r=0);if((0|Ce(A,54924))<1)return Er(f),RA(e),0|(r=0);if(n=0|t[f>>2]){if((0|_(A,0|t[n>>2]))<1)return Er(f),RA(e),0|(r=0)}else if((0|Ce(A,54941))<1)return Er(f),RA(e),0|(r=0);if((0|Ce(A,80479))<1)return Er(f),RA(e),0|(r=0);if(!(0|Ie(A,i,128)))return Er(f),RA(e),0|(r=0);if((0|Ce(A,54956))<1)return Er(f),RA(e),0|(r=0);n=0|t[f+4>>2];do{if(n){if((0|_(A,0|t[n>>2]))<1)return Er(f),RA(e),0|(r=0);if((0|Ce(A,54973))<1)return Er(f),RA(e),0|(r=0);if(e){if((0|_(A,0|t[e>>2]))>=1)break;return Er(f),RA(e),0|(r=0)}if((0|Ce(A,54980))>=1)break;return Er(f),RA(0),0|(r=0)}if((0|Ce(A,54988))<1)return Er(f),RA(e),0|(r=0)}while(0);if(Ce(A,80479),!(0|Ie(A,i,128)))return Er(f),RA(e),0|(r=0);if((0|Ce(A,55013))<1)return Er(f),RA(e),0|(r=0);if(n=0|t[f+8>>2]){if((0|_A(A,n))<1)return Er(f),RA(e),0|(r=0)}else if((0|Ce(A,55029))<1)return Er(f),RA(e),0|(r=0);if(Ce(A,80479),!(0|Ie(A,i,128)))return Er(f),RA(e),0|(r=0);if((0|Ce(A,55042))<1)return Er(f),RA(e),0|(r=0);n=0|t[f+12>>2];do{if(n){if((0|_A(A,n))>=1)break;return Er(f),RA(e),0|(r=0)}if((0|Ce(A,55060))>=1)break;return Er(f),RA(e),0|(r=0)}while(0);Ce(A,80479),Er(f),RA(e)}else if(f=(0|Ce(A,54897))<1,Er(0),RA(0),f)return 0|(r=0);if(!r)return 0|(r=1)}else if(!r)return(0|Ce(A,80479))>0|0;return 0|(r=0|fi(A,r,i))},function(A,e,r,n,f){e|=0,r|=0,n|=0,f|=0;var a,B,o,u=0;o=G,G=G+16|0,a=0|t[(A|=0)+20>>2],B=0|t[24+(0|t[A+8>>2])>>2],u=0|t[a+20>>2];A:do{if(u){if((0|pi(u))!=(0|f))return Xe(4,142,143,0,0),G=o,0|(r=-1);if(95==(0|Si(0|t[a+20>>2]))){if(1==(0|t[a+16>>2])){if((0|(u=0|yr(0,n,f,e,o,B)))>=1){u=0|t[o>>2];break}return G=o,0|u}return G=o,0|(r=-1)}switch(0|t[a+16>>2]){case 5:if((0|Zi(0|t[A+8>>2]))>>>0<(f+1|0)>>>0)return Xe(4,142,120,0,0),G=o,0|(r=-1);if(!(u=0|t[a+32>>2])&&(u=0|Ln(0|Zi(0|t[A+8>>2]),82796,0),t[a+32>>2]=u,!u))return Xe(4,142,65,0,0),G=o,0|(r=-1);If(0|u,0|n,0|f),u=255&(0|$e(0|Si(0|t[a+20>>2]))),i[(0|t[a+32>>2])+f>>0]=u,u=0|gr(f+1|0,0|t[a+32>>2],e,B,5);break A;case 1:if((0|(u=0|ar(0|Si(0|t[a+20>>2]),n,f,e,o,B)))>=1){u=0|t[o>>2];break A}return G=o,0|u;case 6:if(!(u=0|t[a+32>>2])&&(u=0|Ln(0|Zi(0|t[A+8>>2]),82796,0),t[a+32>>2]=u,!u))return G=o,0|(r=-1);if(0|Gr(B,u,n,0|t[a+20>>2],0|t[a+24>>2],0|t[a+28>>2])){u=0|gr(u=0|dr(B),0|t[a+32>>2],e,B,3);break A}return G=o,0|(r=-1);default:return G=o,0|(r=-1)}}else u=0|gr(f,n,e,B,0|t[a+16>>2])}while(0);return(0|u)<0?(G=o,0|(r=u)):(t[r>>2]=u,G=o,0|(r=1))},function(A,e,r,i,n){e|=0,r|=0,i|=0,n|=0;var f,a,B,o,u=0;o=G,G=G+16|0,B=0|t[(A|=0)+20>>2],f=0|t[A+8>>2],a=0|t[f+24>>2],u=0|t[B+20>>2];A:do{if(u){if(1==(0|t[B+16>>2]))return i=0|ur(0|Si(u),i,n,e,r,a),G=o,0|i;if((0|pi(u))!=(0|n))return Xe(4,149,143,0,0),G=o,0|(i=-1);switch(0|t[B+16>>2]){case 5:if((0|Yr(A,0,o,e,r))<1)return G=o,0|(i=0);u=0|t[o>>2];break A;case 6:return(u=0|t[B+32>>2])||(u=0|Ln(0|Zi(0|t[A+8>>2]),82796,0),t[B+32>>2]=u,u)?(0|Mr(r,e,u,a,3))<1?(G=o,0|(i=0)):(i=(0|Ur(a,i,0|t[B+20>>2],0|t[B+24>>2],0|t[B+32>>2],0|t[B+28>>2]))>0&1,G=o,0|i):(G=o,0|(i=-1));default:return G=o,0|(i=-1)}}else{if(!(u=0|t[B+32>>2])&&(u=0|Ln(0|Zi(f),82796,0),t[B+32>>2]=u,!u))return G=o,0|(i=-1);if(u=0|Mr(r,e,u,a,0|t[B+16>>2]),t[o>>2]=u,!u)return G=o,0|(i=0)}}while(0);return(0|u)!=(0|n)?(G=o,0|(i=0)):(i=0==(0|Ht(i,0|t[B+32>>2],n))&1,G=o,0|i)},Yr,function(A,e,r,i,n){e|=0,r|=0,i|=0,n|=0;var f,a=0,B=0;f=0|t[(A|=0)+20>>2],a=0|t[f+16>>2],B=0|t[24+(0|t[A+8>>2])>>2];do{if(4==(0|a)){if(B=0|dr(B),!(a=0|t[f+32>>2])&&(a=0|Ln(0|Zi(0|t[A+8>>2]),82796,0),t[f+32>>2]=a,!a))return 0|(r=-1);if(0|ir(a,B,i,n,0|t[f+36>>2],0|t[f+40>>2],0|t[f+20>>2],0|t[f+24>>2])){a=0|Ir(B,0|t[f+32>>2],e,0|t[24+(0|t[A+8>>2])>>2],3);break}return 0|(r=-1)}a=0|Ir(n,i,e,B,a)}while(0);return(0|a)<0?0|(r=a):(t[r>>2]=a,0|(r=1))},function(A,e,r,i,n){e|=0,r|=0,i|=0,n|=0;var f,a=0;f=0|t[(A|=0)+20>>2],a=0|t[f+16>>2];do{if(4==(0|a)){if(!(a=0|t[f+32>>2])&&(a=0|Ln(0|Zi(0|t[A+8>>2]),82796,0),t[f+32>>2]=a,!a))return 0|(r=-1);if((0|(a=0|Dr(n,i,a,0|t[24+(0|t[A+8>>2])>>2],3)))<1)return 0|(r=a);a=0|fr(e,a,0|t[f+32>>2],a,a,0|t[f+36>>2],0|t[f+40>>2],0|t[f+20>>2],0|t[f+24>>2]);break}a=0|Dr(n,i,e,0|t[24+(0|t[A+8>>2])>>2],a)}while(0);return(0|a)<0?0|(r=a):(t[r>>2]=a,0|(r=1))},Zf,Zf,Zf],UB=[Nf,function(A,e,r,i,n,f){return e|=0,r|=0,0,n|=0,f|=0,(n=0|t[(A|=0)>>2])||(n=0|qf(),t[A>>2]=n,n)?0|fa(e,r,n)?0|(A=1):(n=0|t[A>>2])?(1&t[f+20>>2]?xf(n):Pf(n),t[A>>2]=0,0|(A=0)):0|(A=0):0|(A=0)},function(A,e,r,i,n,f){e|=0,r|=0,n|=0,f|=0,n=0|t[(A|=0)>>2];do{if(!n){if(n=0|$f(),t[A>>2]=n,n)break;return 0|(A=0)}}while(0);return 0|fa(e,r,n)?(ha(0|t[A>>2],4),0|(A=1)):(n=0|t[A>>2])?(1&t[f+20>>2]?xf(n):Pf(n),t[A>>2]=0,0|(A=0)):0|(A=0)},function(A,e,r,n,a,B){A|=0,e|=0,n|=0,a|=0,B|=0;var o=0,u=0;if((0|(r|=0))>1){switch(0|i[e>>0]){case-1:e=e+1|0,a=r+-1|0,n=255;break;case 0:e=e+1|0,a=r+-1|0,n=0;break;default:a=r,n=256}if((0|a)>4)return Xe(13,166,128,0,0),0|(A=0);if(256!=(0|n)){if(!(128&(n^f[e>>0])))return Xe(13,166,221,0,0),0|(A=0);o=e,r=n,u=14}else u=9}else r?(a=r,u=9):(a=0,u=10);if(9==(0|u)&&((0|i[e>>0])<0?(n=255,u=13):u=10),10==(0|u)&&(n=0,u=13),13==(0|u)&&((0|a)>0?(o=e,r=n,u=14):(a=n,e=0)),14==(0|u)){n=0,e=0;do{e=r^f[o+n>>0]|e<<8,n=n+1|0}while((0|n)!=(0|a));if((0|e)<0)return Xe(13,166,128,0,0),0|(A=0);a=r}return(0|(e^=(0!=(0|a))<<31>>31))==(0|t[B+20>>2])?(Xe(13,166,128,0,0),0|(A=0)):(i[A>>0]=e,i[A+1>>0]=e>>8,i[A+2>>0]=e>>16,i[A+3>>0]=e>>24,0|(A=1))},Pa,function(A,e,r,i,n,f){A|=0,e|=0,r|=0,n|=0,f|=0;var a,B=0,o=0;if(a=G,G=G+16|0,912!=(0|zr(0|t[(i|=0)>>2])))return Xe(4,148,155,0,0),G=a,-1;if(t[a+4>>2]=0,912!=(0|zr(0|t[i>>2])))return Xe(4,155,155,0,0),G=a,-1;r=0|S(52600,0|t[i+4>>2]);A:do{if(r){if(e=0|t[r+4>>2])if(n=911==(0|zr(0|t[e>>2]))?0|S(52132,0|t[e+4>>2]):0,e=0|t[r+4>>2]){if(911!=(0|zr(0|t[e>>2]))){Xe(4,157,153,0,0),e=-1;break}if(!n){Xe(4,157,154,0,0),n=0,e=-1;break}if(!(e=0|Xi(0|pr(0|zr(0|t[n>>2]))))){Xe(4,157,151,0,0),e=-1;break}o=e}else B=17;else n=0,B=17;if(17==(0|B)){if(!(e=52916)){e=-1;break}o=e}if(e=0|t[r>>2]){if(!(e=0|Xi(0|pr(0|zr(0|t[e>>2]))))){Xe(4,156,166,0,0),e=-1;break}}else if(!(e=52916)){e=-1;break}if(i=0|t[r+8>>2]){if((0|(i=0|CA(i)))<0){Xe(4,155,150,0,0),e=-1;break}}else i=20;if(0|(B=0|t[r+12>>2])&&1!=(0|CA(B))){Xe(4,155,139,0,0),e=-1;break}e:do{if(!f){do{if((0|Qi(0,-1,248,13,0,a))>=1){if((0|(f=0|Si(e)))==(0|Si(0|t[a>>2])))break e;Xe(4,155,158,0,0);break}}while(0);e=-1;break A}if(!(0|vi(A,a+4|0,e,0,f))){e=-1;break A}}while(0);e=(0|Qi(0|t[a+4>>2],6,-1,4097,6,0))>=1&&(0|Qi(0|t[a+4>>2],6,24,4098,i,0))>=1?(e=(0|Qi(0|t[a+4>>2],6,1016,4101,0,o))<1)?-1:2:-1}else Xe(4,155,149,0,0),n=0,r=0,e=-1}while(0);return Er(r),RA(n),G=a,0|e},function(A,e,r,i,n,f){e|=0,r|=0,i|=0,n|=0,f|=0;var a,B=0,o=0;if(a=G,G=G+32|0,(0|Qi(r=0|zi(A|=0),6,-1,4102,0,a))<1)return G=a,0|(n=0);if(6!=(0|t[a>>2]))return G=a,0|(n=2);t[a+8>>2]=0,e=0|function(A){return 0|t[(A|=0)+8>>2]}(r);do{if((0|Qi(r,-1,248,13,0,a+16|0))>=1&&(0|Qi(r,6,1016,4104,0,a+12|0))>=1&&0!=(0|Qi(r,6,24,4103,0,a+4|0))){switch(0|t[a+4>>2]){case-1:t[a+4>>2]=0|pi(0|t[a+16>>2]);break;case-2:t[a+4>>2]=(0|Zi(e))-2-(0|pi(0|t[a+16>>2])),7+(0|Gi(e))&7||(t[a+4>>2]=(0|t[a+4>>2])-1)}if(r=0|pA(52600)){if(20!=(0|t[a+4>>2])){if(e=0|rA(),t[r+8>>2]=e,!e){o=26;break}if(!(0|sA(e,0|t[a+4>>2]))){o=26;break}}if(64!=(0|Si(e=0|t[a+16>>2]))){if(f=0|YA(),t[r>>2]=f,!f){o=26;break}VA(f,e)}if(f=0|t[a+12>>2],t[a+20>>2]=0,t[r+4>>2]=0,64!=(0|Si(f))&&(64!=(0|Si(f))?(e=0|YA())?(VA(e,f),o=22):e=0:(e=0,o=22),22==(0|o)&&0!=(0|p(e,52132,a+20|0))&&(B=0|YA(),t[r+4>>2]=B,0!=(0|B))&&(yA(B,A=0|Sr(911),16,0|t[a+20>>2]),t[a+20>>2]=0),cA(0|t[a+20>>2]),RA(e),!(0|t[r+4>>2]))){o=26;break}B=0==(0|p(r,52600,a+8|0)),Er(r),r=0|t[a+8>>2],B&&(o=28)}else r=0,o=26}else r=0,o=26}while(0);if(26==(0|o)&&(Er(r),r=0|t[a+8>>2],o=28),28==(0|o)&&(cA(r),r=0),!r)return G=a,0|(n=0);do{if(0|n){if(0|(e=0|lA(r))){yA(n,0|Sr(912),16,e);break}return cA(r),G=a,0|(n=0)}}while(0);return yA(i,0|Sr(912),16,r),G=a,0|(n=3)},Nf],GB=[function(A,e,r,i,n,t,f,a){return 0,0,0,0,0,0,0,0,l(6),0}],ZB=[Ff,function(){t[20512]=1,t[20515]=0|H(82052,0)},function(){t[20522]=0|Ze(18,29),t[20523]=0|W(),t[20521]=0!=(0|t[20522])&0!=(0|t[20523])&1},function(){var A,e=0;A=G,G=G+16|0;A:do{if(0|H(A,19)){e=0|W(),t[20546]=e;do{if(e){if(0|d(5)){e=0|t[20546];break}t[13248]=t[A>>2],t[20545]=1,e=1;break A}e=0}while(0);V(e),t[20546]=0,X(A),e=0}else e=0}while(0);t[20542]=e,G=A},function(){var A,e=0;return A=G,G=G+16|0,0|function(A){return A|=0,0|Kn(262144,0,0)?0!=(0|m(82056,1))&0!=(0|t[20515])?(t[A>>2]=0|v(82052),0|0!=(0|J(82052,-1))&1):0|0:0|(A=0)}(A)?(Hr(0|function(A,e){A|=0,e|=0;var r,i,n=0;return(n=0|t[20518])||(n=52736),(n=0|t[n+36>>2])?(0|(r=0|DB[31&n](A,0,0)))<0?0|(e=0):((i=0|Ln(r,82796,0))?((n=0|t[20518])||(n=52736),(n=0|t[n+36>>2])?n=0|DB[31&n](A,i,r):(Xe(37,105,108,0,0),n=-1),n=(0|n)==(0|r)?0|vr(0,i,0,e):0):n=0,Xn(i,82796,0),0|(e=n)):(Xe(37,105,108,0,0),0|(e=0))}(82180,4)),function(A){-1!=(0|(A|=0))&&J(82052,A)}(0|t[A>>2]),e=1,t[20544]=e,void(G=A)):(e=0,t[20544]=e,void(G=A))},function(){var A,e=0;A=G,G=G+16|0,0!=(0|t[20545])&0==(0|t[20540])?(t[20540]=1,e=0|v(52992),J(52992,0),0|e&&(0|t[e>>2]&&Vr(),0|t[e+4>>2]&&ze(),Xn(e)),V(0|t[20546]),0|t[20547]&&(X(82064),X(82068)),t[A>>2]=t[13248],t[13248]=-1,X(A),ot(),function(A){A|=0;var e,r=0;ot(),e=0|Hn(0|t[20565]),r=0|t[20565];A:do{if((0|e)>0){if(0|A)for(A=e+-1|0;;){if(r=0|vn(r,A),Yn(0|t[20565],A),Hr(0|t[r>>2]),Xn(0|t[r+4>>2]),Xn(r),r=0|t[20565],!((0|A)>0))break A;A=A+-1|0}for(A=e+-1|0;r=0|vn(r,A),(0|t[r+16>>2])<=0&&0|t[r>>2]&&(Yn(0|t[20565],A),Hr(0|t[r>>2]),Xn(0|t[r+4>>2]),Xn(r)),r=0|t[20565],(0|A)>0;)A=A+-1|0}}while(0);0|Hn(r)||(Fn(0|t[20565]),t[20565]=0)}(1),mn(0|t[20492],22),t[20492]=0,mn(0|t[20493],22),t[20493]=0,mn(0|t[20494],22),t[20494]=0,mn(0|t[20495],22),t[20495]=0,mn(0|t[20496],22),t[20496]=0,mn(0|t[20497],22),t[20497]=0,mn(0|t[20498],22),t[20498]=0,mn(0|t[20499],22),t[20499]=0,mn(0|t[20500],22),t[20500]=0,mn(0|t[20501],22),t[20501]=0,mn(0|t[20502],22),t[20502]=0,mn(0|t[20503],22),t[20503]=0,mn(0|t[20504],22),t[20504]=0,mn(0|t[20505],22),t[20505]=0,V(0|t[20568]),t[20568]=0,V(0),Ti(),function(){var A;(A=0|t[20526])&&(ve(A,0),Ve(0|t[20526],14),Ve(0|t[20526],15),Ve(0|t[20526],16),Fe(0|t[20526]),t[20526]=0)}(),0|t[20512]&&X(82052),V(0),t[20545]=0,G=A):G=A},function(){t[20549]=1},function(){t[20550]=1},function(){t[20552]=1},function(){t[20553]=1},function(){t[20555]=1},function(){t[20567]=1,t[20557]=1},function(){!function(A){if(A|=0,0|t[20567])return;(function(){(function(){ut(54519,25,10)})(),function(){ut(53924,22,7)}(),function(){ut(80592,32,0)}(),function(){ut(80626,33,20)}()})(),Se(),function(A,e,r){A|=0,e|=0,r|=0;var i,n=0,t=0,f=0;i=0|it(0);do{if(i){if(A)n=A;else if(!(n=0|Bt())){t=0,n=0;break}if((0|tt(i,n,0))>=1){t=n,n=0|ft(i,e,r),f=9;break}0!=(16&r|0)&&114==(4095&(0|pe())|0)?(Se(),t=n,n=1,f=9):(t=n,n=0,f=9)}else t=0,n=0,f=9}while(0);if(9==(0|f)&&A)return f=n,nt(i),0|f;Xn(t),f=n,nt(i)}(0,A,48),t[20567]=1}(0|t[20558]),t[20559]=1},function(){var A=0;0|function(){var A=0;return 0|H(82064,0)?0|H(82068,0)?A=1:(X(82064),A=0):A=0,0|A}()?(t[20547]=1,A=1):A=0,t[20561]=A},function(){var A=0;if(!(0|Kn(0,0,0)))return A=0,void(t[20570]=A);A=0|W(),t[20568]=A,A=0!=(0|A)&1,t[20570]=A},Ff],NB=[Wf,function(A){A=0|t[(A|=0)+20>>2],pn(0|t[A+4>>2],0|t[A+8>>2],82796,0),pn(0|t[A+12>>2],0|t[A+16>>2],82796,0),$r(A+20|0,0|t[A+1044>>2]),Xn(A,82796,0)},function(A){A=0|t[(A|=0)+20>>2],pn(0|t[A+4>>2],0|t[A+8>>2],82796,0),$r(A+12|0,0|t[A+1036>>2]),Xn(A,82796,0)},function(A){var e;(e=0|si(A|=0))&&(le(0|t[e+20>>2]),pn(0|t[e+12>>2],0|t[e+4>>2],82796,0),Xn(e,82796,0),ki(A,0))},function(A){var e;(e=0|Ri(A|=0))&&(0|(A=0|t[e+8>>2])&&$r(A,0|t[e>>2]),eA(e))},function(A){sr(0|t[(A|=0)+24>>2])},function(A){(A=0|t[(A|=0)+20>>2])&&(Pf(0|t[A+4>>2]),Xn(0|t[A+32>>2],82796,0),Xn(0|t[A+36>>2],82796,0),Xn(A,82796,0))},function(A){!function(){var A;if(!(A=0|t[20509]))return;t[20509]=0,mn(A,9)}()},function(A){(A|=0)&&(Xn(0|t[A+4>>2],82796,0),Xn(0|t[A+8>>2],82796,0),Xn(0|t[A>>2],82796,0),Xn(A,82796,0))},function(A){1&t[(A|=0)+16>>2]&&Xn(A,82796,0)},function(A){0},function(A){Xn(A|=0,82796,0)},function(A){var e,r=0;if(A|=0){if(e=0|t[20525],r=0|t[A>>2],(0|e)>=0){if((0|e)!=(0|r))return;r=e}!function(A,e){var r;A|=0,e|=0,r=G,G=G+16|0,0==(0|m(82080,2))|0==(0|t[20521])?G=r:(R(0|t[20523]),t[r+8>>2]=A,t[r>>2]=-32769&e,(A=0|Re(0|t[20522],r))?(Xn(A,82796,0),A=1):A=0,y(0|t[20523]),G=r)}(0|t[A+8>>2],r)}},function(A){Xn(A|=0,82796,0)},function(A){A=0|t[(A|=0)+4>>2],t[A+8>>2]=0,t[A+20>>2]=13|t[A+20>>2]},function(A){A=8+(0|t[(A|=0)+4>>2])|0,t[A>>2]=1+(0|t[A>>2])},function(A){var e,r;e=0|t[(A|=0)+4>>2],r=(0|t[e+8>>2])-1|0,t[e+8>>2]=r,0|r||$(e),Xn(A,82796,0)},function(A){Xn(A|=0,82796,0)},function(A){z(A|=0,52788)},function(A){(A|=0)&&(0|t[A>>2]&&Vr(),0|t[A+4>>2]&&ze(),Xn(A))},jn,function(A){var e,r=0,i=0,n=0;if(!(0|t[(A|=0)+4>>2])){if((0|(r=0|Hn(e=0|t[A+8>>2])))>0)do{i=r,n=0|vn(e,r=r+-1|0),Xn(0|t[n+8>>2]),Xn(0|t[n+4>>2]),Xn(n)}while((0|i)>1);Fn(e),Xn(0|t[A>>2]),Xn(A)}},function(A){Xn(A|=0)},Wf,Wf,Wf,Wf,Wf,Wf,Wf,Wf,Wf],FB=[Yf,function(A,e){var r;e|=0,(r=0|t[(A|=0)>>2])&&(1&t[e+20>>2]?xf(r):Pf(r),t[A>>2]=0)},function(A,e){e|=0,t[(A|=0)>>2]=t[e+20>>2]},function(A,e){e|=0,0|t[(A|=0)+4>>2]&&Re(e,A)},function(A,e){var r,i,n;return e|=0,n=G,G=G+32|0,r=0|t[(A|=0)+4>>2],i=0|t[A>>2],r?(A=0|t[A+8>>2],t[n>>2]=i,t[n+4>>2]=r,t[n+8>>2]=A,ge(e,80830,n),void(G=n)):(t[n+16>>2]=i,ge(e,80842,n+16|0),void(G=n))},Yf,Yf,Yf],WB=[function(A,e,r){l(10)},Xn],YB=[function(A,e,r,i,n,t){l(11)}];return{___emscripten_environ_constructor:function(){Q(82288)},___errno_location:Ft,___muldi3:lf,___udivdi3:bf,___uremdi3:kf,__get_environ:function(){return 82288},_bitshift64Lshr:sf,_bitshift64Shl:Cf,_emscripten_get_sbrk_ptr:df,_free:af,_i64Add:cf,_i64Subtract:Ef,_malloc:ff,_memalign:function(A,e){return e|=0,(A|=0)>>>0<9?0|(e=0|ff(e)):0|(e=0|function(A,e){e|=0;var r=0,i=0,n=0,f=0;if((r=(A=A|0)>>>0>16?A:16)+-1&r)for(A=16;A>>>0<r>>>0;)A<<=1;else A=r;if((-64-A|0)>>>0<=e>>>0)return t[20571]=48,0|(f=0);if(!(n=0|ff((f=e>>>0<11?16:e+11&-8)+12+A|0)))return 0|(f=0);do{if(A+-1&n){if(r=(e=((e=(n+A+-1&0-A)-8|0)-(n+-8)|0)>>>0>15?e:e+A|0)-(n+-8)|0,3&(A=0|t[n+-4>>2])){t[e+4>>2]=(-8&A)-r|1&t[e+4>>2]|2,t[e+((-8&A)-r)+4>>2]=1|t[e+((-8&A)-r)+4>>2],t[n+-4>>2]=r|1&t[n+-4>>2]|2,t[e+4>>2]=1|t[e+4>>2],of(n+-8|0,r),i=e,A=e;break}t[e>>2]=(0|t[n+-8>>2])+r,t[e+4>>2]=(-8&A)-r,i=e,A=e;break}i=n+-8|0,A=n+-8|0}while(0);(3&(A=0|t[(r=A+4|0)>>2])|0?(-8&A)>>>0>(f+16|0)>>>0:0)&&(n=i+f|0,t[r>>2]=f|1&A|2,t[n+4>>2]=(-8&A)-f|3,t[(e=i+(-8&A)+4|0)>>2]=1|t[e>>2],of(n,(-8&A)-f|0));return 0|(f=i+8|0)}(A,e))},_memcpy:If,_memmove:gf,_memset:Df,_rsasignjs_init:function(){Z()},_rsasignjs_keypair:function(A,e){var r;return A|=0,e|=0,r=G,G=G+16|0,t[r+4>>2]=A,t[r>>2]=e,e=0|qf(),A=0|kr(),cB(e,65537),1==(0|er(A,2048,e,0))?(ai(A,r+4|0),wr(A,r),sr(A),Pf(e),e=0):e=1,G=r,0|e},_rsasignjs_public_key_bytes:function(){return 450},_rsasignjs_secret_key_bytes:function(){return 1700},_rsasignjs_sign:function(A,e,r,i,n){var f;return A|=0,e|=0,r|=0,i|=0,n|=0,f=G,G=G+160|0,t[f+144>>2]=i,t[f+148>>2]=0|kr(),0|Qr(f+148|0,f+144|0,n)?(dB(f+32|0),Nn(f+32|0,e,r),Zn(f,f+32|0),e=0|ar(672,f,32,A,0,0|t[f+148>>2]),sr(0|t[f+148>>2])):e=-1,G=f,0|e},_rsasignjs_signature_bytes:function(){return 256},_rsasignjs_verify:function(A,e,r,i,n){var f;return A|=0,e|=0,r|=0,i|=0,n|=0,f=G,G=G+160|0,t[f+144>>2]=i,t[f+148>>2]=0|kr(),0|br(f+148|0,f+144|0,n)?(dB(f+32|0),Nn(f+32|0,e,r),Zn(f,f+32|0),e=0|ur(672,f,32,A,256,0|t[f+148>>2]),sr(0|t[f+148>>2])):e=-1,G=f,0|e},dynCall_ii:function(A,e){return e|=0,0|IB[31&(A|=0)](0|e)},dynCall_iii:function(A,e,r){return e|=0,r|=0,0|gB[63&(A|=0)](0|e,0|r)},dynCall_iiii:function(A,e,r,i){return e|=0,r|=0,i|=0,0|DB[31&(A|=0)](0|e,0|r,0|i)},dynCall_iiiii:function(A,e,r,i,n){return e|=0,r|=0,i|=0,n|=0,0|MB[31&(A|=0)](0|e,0|r,0|i,0|n)},dynCall_iiiiii:function(A,e,r,i,n,t){return e|=0,r|=0,i|=0,n|=0,t|=0,0|hB[15&(A|=0)](0|e,0|r,0|i,0|n,0|t)},dynCall_iiiiiii:function(A,e,r,i,n,t,f){return e|=0,r|=0,i|=0,n|=0,t|=0,f|=0,0|UB[7&(A|=0)](0|e,0|r,0|i,0|n,0|t,0|f)},dynCall_iiiiiiiii:function(A,e,r,i,n,t,f,a,B){return e|=0,r|=0,i|=0,n|=0,t|=0,f|=0,a|=0,B|=0,0|GB[0&(A|=0)](0|e,0|r,0|i,0|n,0|t,0|f,0|a,0|B)},dynCall_v:function(A){ZB[15&(A|=0)]()},dynCall_vi:function(A,e){e|=0,NB[31&(A|=0)](0|e)},dynCall_vii:function(A,e,r){e|=0,r|=0,FB[7&(A|=0)](0|e,0|r)},dynCall_viii:function(A,e,r,i){e|=0,r|=0,i|=0,WB[1&(A|=0)](0|e,0|r,0|i)},dynCall_viiiiii:function(A,e,r,i,n,t,f){e|=0,r|=0,i|=0,n|=0,t|=0,f|=0,YB[0&(A|=0)](0|e,0|r,0|i,0|n,0|t,0|f)},stackAlloc:function(A){var e;return e=G,G=(G=G+(A|=0)|0)+15&-16,0|e},stackRestore:function(A){G=A|=0},stackSave:function(){return 0|G}}}(asmGlobalArg,asmLibraryArg,buffer),___emscripten_environ_constructor=Module.___emscripten_environ_constructor=asm.___emscripten_environ_constructor,___errno_location=Module.___errno_location=asm.___errno_location,___muldi3=Module.___muldi3=asm.___muldi3,___udivdi3=Module.___udivdi3=asm.___udivdi3,___uremdi3=Module.___uremdi3=asm.___uremdi3,__get_environ=Module.__get_environ=asm.__get_environ,_bitshift64Lshr=Module._bitshift64Lshr=asm._bitshift64Lshr,_bitshift64Shl=Module._bitshift64Shl=asm._bitshift64Shl,_emscripten_get_sbrk_ptr=Module._emscripten_get_sbrk_ptr=asm._emscripten_get_sbrk_ptr,_free=Module._free=asm._free,_i64Add=Module._i64Add=asm._i64Add,_i64Subtract=Module._i64Subtract=asm._i64Subtract,_malloc=Module._malloc=asm._malloc,_memalign=Module._memalign=asm._memalign,_memcpy=Module._memcpy=asm._memcpy,_memmove=Module._memmove=asm._memmove,_memset=Module._memset=asm._memset,_rsasignjs_init=Module._rsasignjs_init=asm._rsasignjs_init,_rsasignjs_keypair=Module._rsasignjs_keypair=asm._rsasignjs_keypair,_rsasignjs_public_key_bytes=Module._rsasignjs_public_key_bytes=asm._rsasignjs_public_key_bytes,_rsasignjs_secret_key_bytes=Module._rsasignjs_secret_key_bytes=asm._rsasignjs_secret_key_bytes,_rsasignjs_sign=Module._rsasignjs_sign=asm._rsasignjs_sign,_rsasignjs_signature_bytes=Module._rsasignjs_signature_bytes=asm._rsasignjs_signature_bytes,_rsasignjs_verify=Module._rsasignjs_verify=asm._rsasignjs_verify,stackAlloc=Module.stackAlloc=asm.stackAlloc,stackRestore=Module.stackRestore=asm.stackRestore,stackSave=Module.stackSave=asm.stackSave,dynCall_ii=Module.dynCall_ii=asm.dynCall_ii,dynCall_iii=Module.dynCall_iii=asm.dynCall_iii,dynCall_iiii=Module.dynCall_iiii=asm.dynCall_iiii,dynCall_iiiii=Module.dynCall_iiiii=asm.dynCall_iiiii,dynCall_iiiiii=Module.dynCall_iiiiii=asm.dynCall_iiiiii,dynCall_iiiiiii=Module.dynCall_iiiiiii=asm.dynCall_iiiiiii,dynCall_iiiiiiiii=Module.dynCall_iiiiiiiii=asm.dynCall_iiiiiiiii,dynCall_v=Module.dynCall_v=asm.dynCall_v,dynCall_vi=Module.dynCall_vi=asm.dynCall_vi,dynCall_vii=Module.dynCall_vii=asm.dynCall_vii,dynCall_viii=Module.dynCall_viii=asm.dynCall_viii,dynCall_viiiiii=Module.dynCall_viiiiii=asm.dynCall_viiiiii,calledRun;if(Module.asm=asm,Module.writeArrayToMemory=writeArrayToMemory,memoryInitializer)if(isDataURI(memoryInitializer)||(memoryInitializer=locateFile(memoryInitializer)),ENVIRONMENT_IS_NODE||ENVIRONMENT_IS_SHELL){var data=readBinary(memoryInitializer);HEAPU8.set(data,GLOBAL_BASE)}else{addRunDependency("memory initializer");var applyMemoryInitializer=function(A){A.byteLength&&(A=new Uint8Array(A)),HEAPU8.set(A,GLOBAL_BASE),Module.memoryInitializerRequest&&delete Module.memoryInitializerRequest.response,removeRunDependency("memory initializer")},doBrowserLoad=function(){readAsync(memoryInitializer,applyMemoryInitializer,(function(){throw new Error("could not load memory initializer "+memoryInitializer)}))},memoryInitializerBytes=tryParseAsDataURI(memoryInitializer);if(memoryInitializerBytes)applyMemoryInitializer(memoryInitializerBytes.buffer);else if(Module.memoryInitializerRequest){var useRequest=function(){var A=Module.memoryInitializerRequest,e=A.response;if(200!==A.status&&0!==A.status){var r=tryParseAsDataURI(Module.memoryInitializerRequestURL);if(!r)return console.warn("a problem seems to have happened with Module.memoryInitializerRequest, status: "+A.status+", retrying "+memoryInitializer),void doBrowserLoad();e=r.buffer}applyMemoryInitializer(e)};Module.memoryInitializerRequest.response?setTimeout(useRequest,0):Module.memoryInitializerRequest.addEventListener("load",useRequest)}else doBrowserLoad()}function ExitStatus(A){this.name="ExitStatus",this.message="Program terminated with exit("+A+")",this.status=A}function run(A){function e(){calledRun||(calledRun=!0,Module.calledRun=!0,ABORT||(initRuntime(),preMain(),Module.onRuntimeInitialized&&Module.onRuntimeInitialized(),postRun()))}A=A||arguments_,runDependencies>0||(preRun(),runDependencies>0||(Module.setStatus?(Module.setStatus("Running..."),setTimeout((function(){setTimeout((function(){Module.setStatus("")}),1),e()}),1)):e()))}if(dependenciesFulfilled=function A(){calledRun||run(),calledRun||(dependenciesFulfilled=A)},Module.run=run,Module.preInit)for("function"==typeof Module.preInit&&(Module.preInit=[Module.preInit]);Module.preInit.length>0;)Module.preInit.pop()();noExitRuntime=!0,run()}function dataResult(A,e){return new Uint8Array(new Uint8Array(Module.HEAPU8.buffer,A,e))}function dataFree(A){try{Module._free(A)}catch(A){setTimeout((function(){throw A}),0)}}function importJWK(A,e){return Promise.resolve().then((function(){var r=A.indexOf(0),i=JSON.parse(sodiumUtil.to_string(r>-1?new Uint8Array(A.buffer,A.byteOffset,r):A));return Promise.resolve().then((function(){return crypto.subtle.importKey("jwk",i,algorithm,!1,[e])})).catch((function(){return pemJwk.jwk2pem(i)}))}))}function exportJWK(A,e){return Promise.resolve().then((function(){return"string"==typeof A?pemJwk.pem2jwk(A):crypto.subtle.exportKey("jwk",A,algorithm.name)})).then((function(A){var r=sodiumUtil.from_string(JSON.stringify(A)),i=new Uint8Array(e);return i.set(r),sodiumUtil.memzero(r),i}))}function exportKeyPair(A){return Promise.all([exportJWK(A.publicKey,rsaSign.publicKeyBytes),exportJWK(A.privateKey,rsaSign.privateKeyBytes)]).then((function(A){return{publicKey:A[0],privateKey:A[1]}}))}[Float32Array,Float64Array,Int8Array,Int16Array,Int32Array,Uint8Array,Uint16Array,Uint32Array,Uint8ClampedArray].forEach((function(A){A.prototype.indexOf||(A.prototype.indexOf=function(A){for(var e=0;e<this.length;++e)if(this[e]===A)return e;return-1})}));var algorithm=isNode?"RSA-SHA256":{name:"RSASSA-PKCS1-v1_5",hash:{name:"SHA-256"},modulusLength:2048,publicExponent:new Uint8Array([1,0,1])},rsaSign={publicKeyBytes:450,privateKeyBytes:1700,bytes:256,keyPair:function(){return Promise.resolve().then((function(){if(isNode){var A=generateRSAKeypair();return{publicKey:A.public,privateKey:A.private}}return crypto.subtle.generateKey(algorithm,!0,["sign","verify"])})).then(exportKeyPair).catch((function(){return initiated.then((function(){var A=Module._malloc(rsaSign.publicKeyBytes),e=Module._malloc(rsaSign.privateKeyBytes);try{var r=Module._rsasignjs_keypair(A,e);if(0!==r)throw new Error("RSA Sign error: keyPair failed ("+r+")");return exportKeyPair({publicKey:"-----BEGIN PUBLIC KEY-----\n"+sodiumUtil.to_base64(dataResult(A,rsaSign.publicKeyBytes))+"\n-----END PUBLIC KEY-----",privateKey:"-----BEGIN RSA PRIVATE KEY-----\n"+sodiumUtil.to_base64(dataResult(e,rsaSign.privateKeyBytes))+"\n-----END RSA PRIVATE KEY-----"})}finally{dataFree(A,rsaSign.publicKeyBytes),dataFree(e,rsaSign.privateKeyBytes)}}))}))},sign:function(A,e){return rsaSign.signDetached(A,e).then((function(e){var r=new Uint8Array(rsaSign.bytes+A.length);return r.set(e),r.set(A,rsaSign.bytes),r}))},signDetached:function(A,e){return importJWK(e,"sign").then((function(e){return Promise.resolve().then((function(){if(isNode){var r=Buffer.from(A),i=nodeCrypto.createSign(algorithm);i.write(r),i.end();var n=i.sign(e);return sodiumUtil.memzero(r),n}return crypto.subtle.sign(algorithm,e,A)})).catch((function(){return initiated.then((function(){e=sodiumUtil.from_base64(e.split("-----")[2]);var r=Module._malloc(rsaSign.bytes),i=Module._malloc(A.length),n=Module._malloc(e.length);Module.writeArrayToMemory(A,i),Module.writeArrayToMemory(e,n);try{var t=Module._rsasignjs_sign(r,i,A.length,n,e.length);if(1!==t)throw new Error("RSA Sign error: sign failed ("+t+")");return dataResult(r,rsaSign.bytes)}finally{dataFree(r),dataFree(i),dataFree(n)}}))})).then((function(A){return sodiumUtil.memzero(e),new Uint8Array(A)}))}))},open:function(A,e){return Promise.resolve().then((function(){var r=new Uint8Array(A.buffer,A.byteOffset,rsaSign.bytes),i=new Uint8Array(A.buffer,A.byteOffset+rsaSign.bytes);return rsaSign.verifyDetached(r,i,e).then((function(A){if(A)return i;throw new Error("Failed to open RSA signed message.")}))}))},verifyDetached:function(A,e,r){return importJWK(r,"verify").then((function(r){return Promise.resolve().then((function(){if(isNode){var i=nodeCrypto.createVerify(algorithm);return i.update(Buffer.from(e)),i.verify(r,A)}return crypto.subtle.verify(algorithm,r,A,e)})).catch((function(){return initiated.then((function(){r=sodiumUtil.from_base64(r.split("-----")[2]);var i=Module._malloc(rsaSign.bytes),n=Module._malloc(e.length),t=Module._malloc(r.length);Module.writeArrayToMemory(A,i),Module.writeArrayToMemory(e,n),Module.writeArrayToMemory(r,t);try{return 1===Module._rsasignjs_verify(i,n,e.length,t,r.length)}finally{dataFree(i),dataFree(n),dataFree(t)}}))})).then((function(A){return sodiumUtil.memzero(r),A}))}))}};rsaSign.rsaSign=rsaSign,module.exports=rsaSign;


/***/ }),
/* 6 */
/***/ (function(module, __unused_webpack_exports, __webpack_require__) {

var asn = __webpack_require__(7)
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
/* 7 */
/***/ (function(__unused_webpack_module, exports, __webpack_require__) {

var asn1 = exports;

asn1.bignum = __webpack_require__(8);

asn1.define = (__webpack_require__(10).define);
asn1.base = __webpack_require__(13);
asn1.constants = __webpack_require__(21);
asn1.decoders = __webpack_require__(23);
asn1.encoders = __webpack_require__(26);


/***/ }),
/* 8 */
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
      Buffer = (__webpack_require__(9).Buffer);
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
/* 9 */
/***/ (function() {

/* (ignored) */

/***/ }),
/* 10 */
/***/ (function(__unused_webpack_module, exports, __webpack_require__) {

var asn1 = __webpack_require__(7);
var inherits = __webpack_require__(11);

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
    named = (__webpack_require__(12).runInThisContext)(
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
/* 11 */
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
/* 12 */
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
/* 13 */
/***/ (function(__unused_webpack_module, exports, __webpack_require__) {

var base = exports;

base.Reporter = (__webpack_require__(14).Reporter);
base.DecoderBuffer = (__webpack_require__(15).DecoderBuffer);
base.EncoderBuffer = (__webpack_require__(15).EncoderBuffer);
base.Node = __webpack_require__(19);


/***/ }),
/* 14 */
/***/ (function(__unused_webpack_module, exports, __webpack_require__) {

var inherits = __webpack_require__(11);

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
/* 15 */
/***/ (function(__unused_webpack_module, exports, __webpack_require__) {

var inherits = __webpack_require__(11);
var Reporter = (__webpack_require__(13).Reporter);
var Buffer = (__webpack_require__(16).Buffer);

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
/* 16 */
/***/ (function(__unused_webpack_module, exports, __webpack_require__) {

"use strict";
/*!
 * The buffer module from node.js, for the browser.
 *
 * @author   Feross Aboukhadijeh <https://feross.org>
 * @license  MIT
 */
/* eslint-disable no-proto */



var base64 = __webpack_require__(17)
var ieee754 = __webpack_require__(18)
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
/* 17 */
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
/* 18 */
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
/* 19 */
/***/ (function(module, __unused_webpack_exports, __webpack_require__) {

var Reporter = (__webpack_require__(13).Reporter);
var EncoderBuffer = (__webpack_require__(13).EncoderBuffer);
var DecoderBuffer = (__webpack_require__(13).DecoderBuffer);
var assert = __webpack_require__(20);

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
/* 20 */
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
/* 21 */
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

constants.der = __webpack_require__(22);


/***/ }),
/* 22 */
/***/ (function(__unused_webpack_module, exports, __webpack_require__) {

var constants = __webpack_require__(21);

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
/* 23 */
/***/ (function(__unused_webpack_module, exports, __webpack_require__) {

var decoders = exports;

decoders.der = __webpack_require__(24);
decoders.pem = __webpack_require__(25);


/***/ }),
/* 24 */
/***/ (function(module, __unused_webpack_exports, __webpack_require__) {

var inherits = __webpack_require__(11);

var asn1 = __webpack_require__(7);
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
/* 25 */
/***/ (function(module, __unused_webpack_exports, __webpack_require__) {

var inherits = __webpack_require__(11);
var Buffer = (__webpack_require__(16).Buffer);

var DERDecoder = __webpack_require__(24);

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
/* 26 */
/***/ (function(__unused_webpack_module, exports, __webpack_require__) {

var encoders = exports;

encoders.der = __webpack_require__(27);
encoders.pem = __webpack_require__(28);


/***/ }),
/* 27 */
/***/ (function(module, __unused_webpack_exports, __webpack_require__) {

var inherits = __webpack_require__(11);
var Buffer = (__webpack_require__(16).Buffer);

var asn1 = __webpack_require__(7);
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
/* 28 */
/***/ (function(module, __unused_webpack_exports, __webpack_require__) {

var inherits = __webpack_require__(11);

var DEREncoder = __webpack_require__(27);

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
/* 29 */
/***/ (function(module) {

var sodiumUtil=function(){function r(e){if("function"==typeof TextDecoder)return new TextDecoder("utf-8",{fatal:!0}).decode(e);var n=8192,t=Math.ceil(e.length/n);if(t<=1)try{return decodeURIComponent(escape(String.fromCharCode.apply(null,e)))}catch(r){throw new TypeError("The encoded data was not valid.")}for(var o="",i=0,f=0;f<t;f++){var a=Array.prototype.slice.call(e,f*n+i,(f+1)*n+i);if(0!=a.length){var u,c=a.length,l=0;do{var s=a[--c];s>=240?(l=4,u=!0):s>=224?(l=3,u=!0):s>=192?(l=2,u=!0):s<128&&(l=1,u=!0)}while(!u);for(var d=l-(a.length-c),p=0;p<d;p++)i--,a.pop();o+=r(a)}}return o}function e(r){for(var e,n,t,o="",i=0;i<r.length;i++)t=87+(n=15&r[i])+(n-10>>8&-39)<<8|87+(e=r[i]>>>4)+(e-10>>8&-39),o+=String.fromCharCode(255&t)+String.fromCharCode(t>>>8);return o}function n(r,e){function n(r){return r<26?r+65:r<52?r+71:r<62?r-4:62===r?43:63===r?47:65}if(void 0===e&&(e=!0),"string"==typeof r)throw new Error("input has to be an array");for(var t=2,o="",i=r.length,f=0,a=0;a<i;a++)t=a%3,a>0&&4*a/3%76==0&&!e&&(o+="\r\n"),f|=r[a]<<(16>>>t&24),2!==t&&r.length-a!=1||(o+=String.fromCharCode(n(f>>>18&63),n(f>>>12&63),n(f>>>6&63),n(63&f)),f=0);return o.substr(0,o.length-2+t)+(2===t?"":1===t?"=":"==")}return{from_base64:function(r){return"string"==typeof r?function(r,e){for(var n,t,o,i=r.replace(/[^A-Za-z0-9\+\/]/g,""),f=i.length,a=e?Math.ceil((3*f+1>>2)/e)*e:3*f+1>>2,u=new Uint8Array(a),c=0,l=0,s=0;s<f;s++)if(t=3&s,c|=((o=i.charCodeAt(s))>64&&o<91?o-65:o>96&&o<123?o-71:o>47&&o<58?o+4:43===o?62:47===o?63:0)<<18-6*t,3===t||f-s==1){for(n=0;n<3&&l<a;n++,l++)u[l]=c>>>(16>>>n&24)&255;c=0}return u}(r):r},from_base64url:function(r){return"string"==typeof r?this.from_base64(r.replace(/-/g,"+").replace(/_/g,"/")):r},from_hex:function(r){return"string"==typeof r?function(r){if(!function(r){return"string"==typeof r&&/^[0-9a-f]+$/i.test(r)&&r.length%2==0}(r))throw new TypeError("The provided string doesn't look like hex data");for(var e=new Uint8Array(r.length/2),n=0;n<r.length;n+=2)e[n>>>1]=parseInt(r.substr(n,2),16);return e}(r):r},from_string:function(r){return"string"==typeof r?function(r){if("function"==typeof TextEncoder)return new TextEncoder("utf-8").encode(r);r=unescape(encodeURIComponent(r));for(var e=new Uint8Array(r.length),n=0;n<r.length;n++)e[n]=r.charCodeAt(n);return e}(r):r},memcmp:function(r,e){if(!(r instanceof Uint8Array&&e instanceof Uint8Array))throw new TypeError("Only Uint8Array instances can be compared");if(r.length!==e.length)throw new TypeError("Only instances of identical length can be compared");for(var n=0,t=0,o=r.length;t<o;t++)n|=r[t]^e[t];return 0===n},memzero:function(r){r instanceof Uint8Array?function(r){if(!r instanceof Uint8Array)throw new TypeError("Only Uint8Array instances can be wiped");for(var e=0,n=r.length;e<n;e++)r[e]=0}(r):"undefined"!=typeof Buffer&&r instanceof Buffer&&r.fill(0)},to_base64:function(r){return"string"==typeof r?r:n(r).replace(/\s+/g,"")},to_base64url:function(r){return this.to_base64(r).replace(/\+/g,"-").replace(/\//g,"_")},to_hex:function(r){return"string"==typeof r?r:e(r).replace(/\s+/g,"")},to_string:function(e){return"string"==typeof e?e:r(e)}}}(); true&&module.exports?(sodiumUtil.sodiumUtil=sodiumUtil,module.exports=sodiumUtil):self.sodiumUtil=sodiumUtil;

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
/******/ 	superFalcon = __webpack_exports__;
/******/ 	
/******/ })()
;