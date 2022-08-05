;

function dataReturn (returnValue, result) {
	if (returnValue === 0) {
		return result;
	}
	else {
		throw new Error('Dilithium error: ' + returnValue);
	}
}

function dataResult (buffer, byteLength) {
	return new Uint8Array(
		new Uint8Array(Module.HEAPU8.buffer, buffer, byteLength)
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


var publicKeyBytes, privateKeyBytes, bytes;

var initiated	= Module.ready.then(function () {
	Module._dilithiumjs_init();

	publicKeyBytes	= Module._dilithiumjs_public_key_bytes();
	privateKeyBytes	= Module._dilithiumjs_secret_key_bytes();
	bytes			= Module._dilithiumjs_signature_bytes();
});


var dilithium	= {
	publicKeyBytes: initiated.then(function () { return publicKeyBytes; }),
	privateKeyBytes: initiated.then(function () { return privateKeyBytes; }),
	bytes: initiated.then(function () { return bytes; }),

	keyPair: function () { return initiated.then(function () {
		var publicKeyBuffer		= Module._malloc(publicKeyBytes);
		var privateKeyBuffer	= Module._malloc(privateKeyBytes);

		try {
			var returnValue	= Module._dilithiumjs_keypair(
				publicKeyBuffer,
				privateKeyBuffer
			);

			return dataReturn(returnValue, {
				publicKey: dataResult(publicKeyBuffer, publicKeyBytes),
				privateKey: dataResult(privateKeyBuffer, privateKeyBytes)
			});
		}
		finally {
			dataFree(publicKeyBuffer);
			dataFree(privateKeyBuffer);
		}
	}); },

	sign: function (message, privateKey) {
		return dilithium.signDetached(message, privateKey).then(function (signature) {
			var signed	= new Uint8Array(bytes + message.length);
			signed.set(signature);
			signed.set(message, bytes);
			return signed;
		});
	},

	signDetached: function (message, privateKey) { return initiated.then(function () {
		var signatureBuffer		= Module._malloc(bytes);
		var messageBuffer		= Module._malloc(message.length);
		var privateKeyBuffer	= Module._malloc(privateKeyBytes);

		Module.writeArrayToMemory(new Uint8Array(bytes), signatureBuffer);
		Module.writeArrayToMemory(message, messageBuffer);
		Module.writeArrayToMemory(privateKey, privateKeyBuffer);

		try {
			var returnValue	= Module._dilithiumjs_sign(
				signatureBuffer,
				messageBuffer,
				message.length,
				privateKeyBuffer
			);

			return dataReturn(returnValue, dataResult(signatureBuffer, bytes));
		}
		finally {
			dataFree(signatureBuffer);
			dataFree(messageBuffer);
			dataFree(privateKeyBuffer);
		}
	}); },

	open: function (signed, publicKey) { return initiated.then(function () {
		var signature	= new Uint8Array(signed.buffer, signed.byteOffset, bytes);
		var message		= new Uint8Array(
			signed.buffer,
			signed.byteOffset + bytes,
			signed.length - bytes
		);

		return dilithium.verifyDetached(signature, message, publicKey).then(function (isValid) {
			if (isValid) {
				return message;
			}
			else {
				throw new Error('Failed to open Dilithium signed message.');
			}
		});
	}); },

	verifyDetached: function (signature, message, publicKey) {
		return initiated.then(function () {
			var signatureBuffer	= Module._malloc(bytes);
			var messageBuffer	= Module._malloc(message.length);
			var publicKeyBuffer	= Module._malloc(publicKeyBytes);

			Module.writeArrayToMemory(signature, signatureBuffer);
			Module.writeArrayToMemory(message, messageBuffer);
			Module.writeArrayToMemory(publicKey, publicKeyBuffer);

			try {
				var returnValue	= Module._dilithiumjs_verify(
					messageBuffer,
					message.length,
					signatureBuffer,
					publicKeyBuffer
				);

				return returnValue === 0;
			}
			finally {
				dataFree(signatureBuffer);
				dataFree(messageBuffer);
				dataFree(publicKeyBuffer);
			}
		});
	}
};



return dilithium;

}());


if (typeof module !== 'undefined' && module.exports) {
	dilithium.dilithium	= dilithium;
	module.exports	= dilithium;
}
else {
	self.dilithium	= dilithium;
}
