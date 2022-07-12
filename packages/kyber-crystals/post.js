;

function dataReturn (returnValue, result) {
	if (returnValue === 0) {
		return result;
	}
	else {
		throw new Error('Kyber error: ' + returnValue);
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


var publicKeyBytes, privateKeyBytes, cyphertextBytes, plaintextBytes;

var initiated	= Module.ready.then(function () {
	Module._kyberjs_init();

	publicKeyBytes	= Module._kyberjs_public_key_bytes();
	privateKeyBytes	= Module._kyberjs_private_key_bytes();
	cyphertextBytes	= Module._kyberjs_encrypted_bytes();
	plaintextBytes	= Module._kyberjs_decrypted_bytes();
});


var kyber	= {
	publicKeyBytes: initiated.then(function () { return publicKeyBytes; }),
	privateKeyBytes: initiated.then(function () { return privateKeyBytes; }),
	cyphertextBytes: initiated.then(function () { return cyphertextBytes; }),
	plaintextBytes: initiated.then(function () { return plaintextBytes - 1; }),

	keyPair: function () { return initiated.then(function () {
		var publicKeyBuffer		= Module._malloc(publicKeyBytes);
		var privateKeyBuffer	= Module._malloc(privateKeyBytes);

		try {
			var returnValue	= Module._kyberjs_keypair(
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

	encrypt: function (message, publicKey) { return initiated.then(function () {
		if (message.length > (plaintextBytes - 1)) {
			throw new Error('Plaintext length exceeds kyber.plaintextBytes.');
		}

		var messageBuffer	= Module._calloc(plaintextBytes, 1);
		var publicKeyBuffer	= Module._malloc(publicKeyBytes);
		var encryptedBuffer	= Module._malloc(cyphertextBytes);

		Module.writeArrayToMemory(new Uint8Array([message.length]), messageBuffer);
		Module.writeArrayToMemory(message, messageBuffer + 1);
		Module.writeArrayToMemory(publicKey, publicKeyBuffer);

		try {
			var returnValue	= Module._kyberjs_encrypt(
				messageBuffer,
				message.length,
				publicKeyBuffer,
				encryptedBuffer
			);

			return dataReturn(
				returnValue,
				dataResult(encryptedBuffer, cyphertextBytes)
			);
		}
		finally {
			dataFree(messageBuffer);
			dataFree(publicKeyBuffer);
			dataFree(encryptedBuffer);
		}
	}); },

	decrypt: function (encrypted, privateKey) { return initiated.then(function () {
		var encryptedBuffer		= Module._malloc(cyphertextBytes);
		var privateKeyBuffer	= Module._malloc(privateKeyBytes);
		var decryptedBuffer		= Module._malloc(plaintextBytes);

		Module.writeArrayToMemory(encrypted, encryptedBuffer);
		Module.writeArrayToMemory(privateKey, privateKeyBuffer);

		try {
			var returnValue	= Module._kyberjs_decrypt(
				encryptedBuffer,
				privateKeyBuffer,
				decryptedBuffer
			);

			return dataReturn(
				returnValue,
				dataResult(
					decryptedBuffer + 1,
					dataResult(decryptedBuffer, 1)[0]
				)
			);
		}
		finally {
			dataFree(encryptedBuffer);
			dataFree(privateKeyBuffer);
			dataFree(decryptedBuffer);
		}
	}); }
};



return kyber;

}());


if (typeof module !== 'undefined' && module.exports) {
	kyber.kyber		= kyber;
	module.exports	= kyber;
}
else {
	self.kyber		= kyber;
}
