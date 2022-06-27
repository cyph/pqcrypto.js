# sphincs.js

## Overview

The [SPHINCS](https://sphincs.cr.yp.to) post-quantum cryptographic signing scheme
compiled to WebAssembly using [Emscripten](https://github.com/kripken/emscripten).
A simple JavaScript wrapper is provided to make SPHINCS easy to use in Web applications.

N.B. Unless interoperability with other SPHINCS implementations is a hard requirement,
it is recommended to use [supersphincs](https://github.com/cyph/supersphincs)
instead of this.

## Example Usage

	(async () => {
		const keyPair /*: {privateKey: Uint8Array; publicKey: Uint8Array} */ =
			await sphincs.keyPair()
		;

		const message /*: Uint8Array */ =
			new Uint8Array([104, 101, 108, 108, 111, 0]) // "hello"
		;

		/* Combined signatures */

		const signed /*: Uint8Array */ =
			await sphincs.sign(message, keyPair.privateKey)
		;

		const verified /*: Uint8Array */ =
			await sphincs.open(signed, keyPair.publicKey) // same as message
		;

		/* Detached signatures */
		
		const signature /*: Uint8Array */ =
			await sphincs.signDetached(message, keyPair.privateKey)
		;

		const isValid /*: boolean */ =
			await sphincs.verifyDetached(signature, message, keyPair.publicKey) // true
		;

		console.log(keyPair);
		console.log(message);
		console.log(signed);
		console.log(verified);
		console.log(signature);
		console.log(isValid);
	})();

## Changelog

Breaking changes in major versions:

2.0.0:

* As part of upgrading from asm.js to WebAssembly (with asm.js included as a fallback),
the API is fully asynchronous.
