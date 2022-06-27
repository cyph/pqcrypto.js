;
return Module;
};

var initiated, Module, publicKeyBytes, privateKeyBytes, bytes;
function resetSphincsModule () {
	Module		= getSphincsModule();
	initiated	= Module.ready.then(function () {
		Module._sphincsjs_init();

		publicKeyBytes	= Module._sphincsjs_public_key_bytes();
		privateKeyBytes	= Module._sphincsjs_secret_key_bytes();
		bytes			= Module._sphincsjs_signature_bytes();
	});
}
resetSphincsModule();


function dataReturn (returnValue, result) {
	if (returnValue === 0) {
		return result;
	}
	else {
		throw new Error('SPHINCS error: ' + returnValue);
	}
}

function dataResult (buffer, bytes) {
	return new Uint8Array(
		new Uint8Array(Module.HEAPU8.buffer, buffer, bytes)
	);
}

function dataFree (buffer) {
	try {
		if (typeof Module._free === 'function') {
			Module._free(buffer);
		}
	}
	catch (err) {
		resetSphincsModule();
		console.error('Re-initializing SPHINCS due to _free failure:', err);
	}
}


var sphincs	= {
	publicKeyBytes: initiated.then(function () { return publicKeyBytes; }),
	privateKeyBytes: initiated.then(function () { return privateKeyBytes; }),
	bytes: initiated.then(function () { return bytes; }),

	keyPair: function (retries) { return initiated.then(function () {
		var publicKeyBuffer, privateKeyBuffer;

		try {
			var publicKeyBuffer		= Module._malloc(publicKeyBytes);
			var privateKeyBuffer	= Module._malloc(privateKeyBytes);

			var returnValue	= Module._sphincsjs_keypair(
				publicKeyBuffer,
				privateKeyBuffer
			);

			return dataReturn(returnValue, {
				publicKey: dataResult(publicKeyBuffer, publicKeyBytes),
				privateKey: dataResult(privateKeyBuffer, privateKeyBytes)
			});
		}
		catch (err) {
			retries = retries || 0;
			if (retries >= 2) {
				throw err;
			}

			console.error('Ignoring error and retrying sphincs.keyPair:', err);
			resetSphincsModule();
			return sphincs.keyPair(retries + 1);
		}
		finally {
			dataFree(publicKeyBuffer);
			dataFree(privateKeyBuffer);
		}
	}); },

	sign: function (message, privateKey, retries) { return initiated.then(function () {
		var signedBuffer, signedLengthBuffer, messageBuffer, privateKeyBuffer;

		try {
			var signedBytes	= message.length + bytes;

			signedBuffer		= Module._malloc(signedBytes);
			signedLengthBuffer	= Module._malloc(8);
			messageBuffer		= Module._malloc(message.length);
			privateKeyBuffer	= Module._malloc(privateKeyBytes);

			Module.writeArrayToMemory(message, messageBuffer);
			Module.writeArrayToMemory(privateKey, privateKeyBuffer);

			var returnValue	= Module._sphincsjs_sign(
				signedBuffer,
				signedLengthBuffer,
				messageBuffer,
				message.length,
				privateKeyBuffer
			);

			return dataReturn(returnValue, dataResult(signedBuffer, signedBytes));
		}
		catch (err) {
			retries = retries || 0;
			if (retries >= 2) {
				throw err;
			}

			console.error('Ignoring error and retrying sphincs.sign:', err);
			resetSphincsModule();
			return sphincs.sign(message, privateKey, retries + 1);
		}
		finally {
			dataFree(signedBuffer);
			dataFree(signedLengthBuffer);
			dataFree(messageBuffer);
			dataFree(privateKeyBuffer);
		}
	}); },

	signDetached: function (message, privateKey) {
		return sphincs.sign(message, privateKey).then(function (signed) {
			return new Uint8Array(
				signed.buffer,
				0,
				bytes
			);
		});
	},

	open: function (signed, publicKey, retries) { return initiated.then(function () {
		var openedBuffer, openedLengthBuffer, signedBuffer, publicKeyBuffer;

		try {
			openedBuffer		= Module._malloc(signed.length + bytes);
			openedLengthBuffer	= Module._malloc(8);
			signedBuffer		= Module._malloc(signed.length);
			publicKeyBuffer		= Module._malloc(publicKeyBytes);

			Module.writeArrayToMemory(signed, signedBuffer);
			Module.writeArrayToMemory(publicKey, publicKeyBuffer);

			var returnValue	= Module._sphincsjs_open(
				openedBuffer,
				openedLengthBuffer,
				signedBuffer,
				signed.length,
				publicKeyBuffer
			);

			return dataReturn(returnValue, dataResult(openedBuffer, signed.length - bytes));
		}
		catch (err) {
			retries = retries || 0;
			if (retries >= 2) {
				throw err;
			}

			console.error('Ignoring error and retrying sphincs.open:', err);
			resetSphincsModule();
			return sphincs.open(signed, publicKey, retries + 1);
		}
		finally {
			dataFree(openedBuffer);
			dataFree(openedLengthBuffer);
			dataFree(signedBuffer);
			dataFree(publicKeyBuffer);
		}
	}); },

	verifyDetached: function (signature, message, publicKey) {
		return initiated.then(function () {
			var signed	= new Uint8Array(bytes + message.length);
			signed.set(signature);
			signed.set(message, bytes);

			return sphincs.open(
				signed,
				publicKey
			).catch(function () {}).then(function (opened) {
				try {
					return opened !== undefined;
				}
				finally {
					var arrs	= opened ? [signed, opened] : [signed];
					for (var i = 0 ; i < arrs.length ; ++i) {
						var arr	= arrs[i];
						for (var j = 0 ; j < arr.length ; ++j) {
							arr[j]	= 0;
						}
					}
				}
			});
		});
	}
};



return sphincs;

}());


if (typeof module !== 'undefined' && module.exports) {
	sphincs.sphincs	= sphincs;
	module.exports	= sphincs;
}
else {
	self.sphincs	= sphincs;
}
