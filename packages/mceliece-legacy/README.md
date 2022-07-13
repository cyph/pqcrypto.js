# mceliece-legacy

## Note

The variant of McEliece used in this package is ***no longer recommended***. Instead prefer
Classic McEliece, which is currently being evaluated for standardization as a NIST PQC Round 4
candidate. An implementation of Classic McEliece is provided by
[mceliece](https://github.com/cyph/pqcrypto.js/tree/master/packages/mceliece).

---

## Overview

The [McEliece](https://en.wikipedia.org/wiki/McEliece_cryptosystem) post-quantum asymmetric
cypher compiled to WebAssembly using [Emscripten](https://github.com/kripken/emscripten).
A simple JavaScript wrapper is provided to make McEliece easy to use in web applications.

The parameters are configured to slightly above 128-bit strength.

The underlying cypher implementation in use is [McBits](https://tungchou.github.io/mcbits).

## Example Usage

	import {mceliece} from 'mceliece-legacy';

	const keyPair /*: {privateKey: Uint8Array; publicKey: Uint8Array} */ =
		await mceliece.keyPair()
	;

	const plaintext /*: Uint8Array */ =
		new Uint8Array([104, 101, 108, 108, 111, 0]) // "hello"
	;

	const encrypted /*: Uint8Array */ =
		await mceliece.encrypt(plaintext, keyPair.publicKey)
	;

	const decrypted /*: Uint8Array */ =
		await mceliece.decrypt(encrypted, keyPair.privateKey) // same as plaintext
	;

	console.log(keyPair);
	console.log(plaintext);
	console.log(encrypted);
	console.log(decrypted);

Note: McEliece is a low-level cryptographic primitive, not a high-level construct like libsodium's
[crypto_box](https://download.libsodium.org/doc/public-key_cryptography/authenticated_encryption.html).
This module can be combined with a symmetric cypher and a MAC to provide such a construct, but you
should avoid using mceliece directly for anything important if you lack the experience to do so.
