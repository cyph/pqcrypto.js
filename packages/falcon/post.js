;
return Module;
};

var initiated, Module, publicKeyBytes, privateKeyBytes, bytes;
function resetFalconModule () {
	Module		= getFalconModule();
	initiated	= Module.ready.then(function () {
		Module._falconjs_init();

		publicKeyBytes	= Module._falconjs_public_key_bytes();
		privateKeyBytes	= Module._falconjs_secret_key_bytes();
		bytes			= Module._falconjs_signature_bytes();
	});
}
resetFalconModule();


function dataReturn (returnValue, result) {
	if (returnValue === 0) {
		return result;
	}
	else {
		throw new Error('FALCON error: ' + returnValue);
	}
}

function dataResult (buffer, byteLength) {
	return new Uint8Array(
		new Uint8Array(Module.HEAPU8.buffer, buffer, byteLength)
	);
}

function dataFree (buffer) {
	try {
		if (typeof Module._free === 'function') {
			Module._free(buffer);
		}
	}
	catch (err) {
		resetFalconModule();
		console.error('Re-initializing FALCON due to _free failure:', err);
	}
}


var falcon	= {
	publicKeyBytes: initiated.then(function () { return publicKeyBytes; }),
	privateKeyBytes: initiated.then(function () { return privateKeyBytes; }),
	bytes: initiated.then(function () { return bytes; }),

	keyPair: function (retries) { return initiated.then(function () {
		var publicKeyBuffer, privateKeyBuffer;

		try {
			var publicKeyBuffer		= Module._malloc(publicKeyBytes);
			var privateKeyBuffer	= Module._malloc(privateKeyBytes);

			var returnValue	= Module._falconjs_keypair(
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

			console.error('Ignoring error and retrying falcon.keyPair:', err);
			resetFalconModule();
			return falcon.keyPair(retries + 1);
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

			var returnValue	= Module._falconjs_sign(
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

			console.error('Ignoring error and retrying falcon.sign:', err);
			resetFalconModule();
			return falcon.sign(message, privateKey, retries + 1);
		}
		finally {
			dataFree(signedBuffer);
			dataFree(signedLengthBuffer);
			dataFree(messageBuffer);
			dataFree(privateKeyBuffer);
		}
	}); },

	signDetached: function (message, privateKey) {
		return falcon.sign(message, privateKey).then(function (signed) {
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

			var returnValue	= Module._falconjs_open(
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

			console.error('Ignoring error and retrying falcon.open:', err);
			resetFalconModule();
			return falcon.open(signed, publicKey, retries + 1);
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

			return falcon.open(
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



return falcon;

}());


if (typeof module !== 'undefined' && module.exports) {
	falcon.falcon	= falcon;
	module.exports	= falcon;
}
else {
	self.falcon	= falcon;
}
