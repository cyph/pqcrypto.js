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

	const plaintext /*: Uint8Array */ =
		new Uint8Array([104, 101, 108, 108, 111, 0]) // "hello"
	;

	const encrypted /*: Uint8Array */ =
		await kyber.encrypt(plaintext, keyPair.publicKey)
	;

	const decrypted /*: Uint8Array */ =
		await kyber.decrypt(encrypted, keyPair.privateKey) // same as plaintext
	;

	console.log(keyPair);
	console.log(plaintext);
	console.log(encrypted);
	console.log(decrypted);

Note: Kyber is a low-level cryptographic primitive, not a high-level construct like libsodium's
[crypto_box](https://download.libsodium.org/doc/public-key_cryptography/authenticated_encryption.html).
This module can be combined with a symmetric cypher and a MAC to provide such a construct, but you
should avoid using kyber directly for anything important if you lack the experience to do so.
