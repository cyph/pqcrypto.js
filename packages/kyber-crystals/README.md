# kyber-crystals

## Overview

The [Kyber](https://pq-crystals.org/kyber) post-quantum asymmetric
cypher compiled to WebAssembly using [Emscripten](https://github.com/kripken/emscripten).
A simple JavaScript wrapper is provided to make Kyber easy to use in web applications.

The default parameter set is Kyber-1024 (roughly 256-bit strength).

## Example Usage

	import {kyber} from 'kyber-crystals';

	const keyPair /*: {privateKey: Uint8Array; publicKey: Uint8Array} */ =
		await kyber.keyPair()
	;

	const {cyphertext, secret} /*: {cyphertext: Uint8Array; secret: Uint8Array} */ =
		await kyber.encrypt(keyPair.publicKey)
	;

	const decrypted /*: Uint8Array */ =
		await kyber.decrypt(cyphertext, keyPair.privateKey) // same as secret
	;

	console.log(keyPair);
	console.log(secret);
	console.log(cyphertext);
	console.log(decrypted);
