# falcon

## Overview

The [FALCON](https://falcon-sign.info) post-quantum cryptographic signing scheme
compiled to WebAssembly using [Emscripten](https://github.com/kripken/emscripten).
A simple JavaScript wrapper is provided to make FALCON easy to use in Web applications.

N.B. Unless interoperability with other FALCON implementations is a hard requirement,
it is recommended to use [superfalcon](https://github.com/cyph/pqcrypto.js/tree/master/packages/superfalcon)
instead.

## Example Usage

	(async () => {
		const keyPair /*: {privateKey: Uint8Array; publicKey: Uint8Array} */ =
			await falcon.keyPair()
		;

		const message /*: Uint8Array */ =
			new Uint8Array([104, 101, 108, 108, 111, 0]) // "hello"
		;

		/* Combined signatures */

		const signed /*: Uint8Array */ =
			await falcon.sign(message, keyPair.privateKey)
		;

		const verified /*: Uint8Array */ =
			await falcon.open(signed, keyPair.publicKey) // same as message
		;

		/* Detached signatures */

		const signature /*: Uint8Array */ =
			await falcon.signDetached(message, keyPair.privateKey)
		;

		const isValid /*: boolean */ =
			await falcon.verifyDetached(signature, message, keyPair.publicKey) // true
		;

		console.log(keyPair);
		console.log(message);
		console.log(signed);
		console.log(verified);
		console.log(signature);
		console.log(isValid);
	})();
