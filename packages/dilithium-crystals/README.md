# dilithium-crystals

## Overview

The [Dilithium](https://pq-crystals.org/dilithium) post-quantum cryptographic signing scheme
compiled to WebAssembly using [Emscripten](https://github.com/kripken/emscripten).
A simple JavaScript wrapper is provided to make Dilithium easy to use in Web applications.

The default parameter set is Dilithium5 (roughly 256-bit strength).

N.B. Unless interoperability with other Dilithium implementations is a hard requirement,
it is recommended to use [superdilithium](https://github.com/cyph/pqcrypto.js/tree/master/packages/superdilithium)
instead.

## Example Usage

	import {dilithium} from 'dilithium-crystals';

	const keyPair /*: {privateKey: Uint8Array; publicKey: Uint8Array} */ =
		await dilithium.keyPair()
	;

	const message /*: Uint8Array */ =
		new Uint8Array([104, 101, 108, 108, 111, 0]) // "hello"
	;

	/* Combined signatures */

	const signed /*: Uint8Array */ =
		await dilithium.sign(message, keyPair.privateKey)
	;

	const verified /*: Uint8Array */ =
		await dilithium.open(signed, keyPair.publicKey) // same as message
	;

	/* Detached signatures */

	const signature /*: Uint8Array */ =
		await dilithium.signDetached(message, keyPair.privateKey)
	;

	const isValid /*: boolean */ =
		await dilithium.verifyDetached(signature, message, keyPair.publicKey) // true
	;

	console.log(keyPair);
	console.log(message);
	console.log(signed);
	console.log(verified);
	console.log(signature);
	console.log(isValid);
