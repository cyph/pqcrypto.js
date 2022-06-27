# rsasign.js

## Overview

[RSASSA-PKCS1-v1_5](https://tools.ietf.org/html/rfc3447#section-8.2) with key length 2048 and 
hash function [SHA-256](https://en.wikipedia.org/wiki/SHA-2) wrapped for usage in JavaScript.

The platform native RSA implementation (via
[SubtleCrypto](https://developer.mozilla.org/en-US/docs/Web/API/SubtleCrypto) or the
[Node.js Crypto API](https://nodejs.org/api/crypto.html)) is preferred where available.
Otherwise, the OpenSSL implementation compiled to pure JavaScript with
[Emscripten](https://github.com/kripken/emscripten) is used as a fallback.

## Example Usage

	(async () => {
		const keyPair /*: {privateKey: Uint8Array; publicKey: Uint8Array} */ =
			await rsaSign.keyPair()
		;

		const message /*: Uint8Array */ =
			new Uint8Array([104, 101, 108, 108, 111, 0]) // "hello"
		;

		/* Combined signatures */

		const signed /*: Uint8Array */ =
			await rsaSign.sign(message, keyPair.privateKey)
		;

		const verified /*: Uint8Array */ =
			await rsaSign.open(signed, keyPair.publicKey) // same as message
		;

		/* Detached signatures */
		
		const signature /*: Uint8Array */ =
			await rsaSign.signDetached(message, keyPair.privateKey)
		;

		const isValid /*: boolean */ =
			await rsaSign.verifyDetached(signature, message, keyPair.publicKey) // true
		;

		console.log(keyPair);
		console.log(message);
		console.log(signed);
		console.log(verified);
		console.log(signature);
		console.log(isValid);
	})();
