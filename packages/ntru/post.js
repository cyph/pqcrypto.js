;

function dataReturn (returnValue, result) {
	if (returnValue === 0) {
		return result;
	}
	else {
		throw new Error('NTRU error: ' + returnValue);
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


var publicKeyBytes, privateKeyBytes, cyphertextBytes, bytes;

var initiated	= Module.ready.then(function () {
	Module._ntrujs_init();

	publicKeyBytes	= Module._ntrujs_public_key_bytes();
	privateKeyBytes	= Module._ntrujs_private_key_bytes();
	cyphertextBytes	= Module._ntrujs_cyphertext_bytes();
	bytes			= Module._ntrujs_secret_bytes();
});


var ntru	= {
	publicKeyBytes: initiated.then(function () { return publicKeyBytes; }),
	privateKeyBytes: initiated.then(function () { return privateKeyBytes; }),
	cyphertextBytes: initiated.then(function () { return cyphertextBytes; }),
	bytes: initiated.then(function () { return bytes; }),

	keyPair: function () { return initiated.then(function () {
		var publicKeyBuffer		= Module._malloc(publicKeyBytes);
		var privateKeyBuffer	= Module._malloc(privateKeyBytes);

		try {
			var returnValue	= Module._ntrujs_keypair(
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

	encrypt: function (publicKey) { return initiated.then(function () {
		var publicKeyBuffer		= Module._malloc(publicKeyBytes);
		var cyphertextBuffer	= Module._malloc(cyphertextBytes);
		var secretBuffer		= Module._malloc(bytes);

		Module.writeArrayToMemory(publicKey, publicKeyBuffer);

		try {
			var returnValue	= Module._ntrujs_encrypt(
				publicKeyBuffer,
				cyphertextBuffer,
				secretBuffer
			);

			return dataReturn(returnValue, {
				cyphertext: dataResult(cyphertextBuffer, cyphertextBytes),
				secret: dataResult(secretBuffer, bytes)
			});
		}
		finally {
			dataFree(secretBuffer);
			dataFree(publicKeyBuffer);
			dataFree(cyphertextBuffer);
		}
	}); },

	decrypt: function (cyphertext, privateKey) { return initiated.then(function () {
		var cyphertextBuffer	= Module._malloc(cyphertextBytes);
		var privateKeyBuffer	= Module._malloc(privateKeyBytes);
		var secretBuffer		= Module._malloc(bytes);

		Module.writeArrayToMemory(cyphertext, cyphertextBuffer);
		Module.writeArrayToMemory(privateKey, privateKeyBuffer);

		try {
			var returnValue	= Module._ntrujs_decrypt(
				cyphertextBuffer,
				privateKeyBuffer,
				secretBuffer
			);

			return dataReturn(
				returnValue,
				dataResult(secretBuffer, bytes)
			);
		}
		finally {
			dataFree(cyphertextBuffer);
			dataFree(privateKeyBuffer);
			dataFree(secretBuffer);
		}
	}); }
};



return ntru;

}());


if (typeof module !== 'undefined' && module.exports) {
	ntru.ntru		= ntru;
	module.exports	= ntru;
}
else {
	self.ntru		= ntru;
}
