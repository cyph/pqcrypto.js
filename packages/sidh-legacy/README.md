# sidh-legacy

## Note

The version of SIDH used in this package is ***no longer recommended***. Instead prefer the
latest version, which is currently being evaluated for standardization as a NIST PQC Round 4
candidate. An up-to-date implementation of SIDH is provided by
[sidh](https://github.com/cyph/pqcrypto.js/tree/master/packages/sidh).

[Either way, you probably shouldn't use this.](https://news.ycombinator.com/item?id=32317865)

---

## Overview

The [SIDH](https://en.wikipedia.org/wiki/Supersingular_isogeny_key_exchange) post-quantum asymmetric
cypher compiled to WebAssembly using [Emscripten](https://github.com/kripken/emscripten).
The specific implementation in use is [from Microsoft Research](https://github.com/Microsoft/PQCrypto-SIDH).
A simple JavaScript wrapper is provided to make SIDH easy to use in web applications.

The parameters are configured to 128-bit strength (SIKEp503).

## Example Usage

	import {sidh} from 'sidh-legacy';

	const keyPair /*: {privateKey: Uint8Array; publicKey: Uint8Array} */ =
		await sidh.keyPair()
	;

	const plaintext /*: Uint8Array */ =
		new Uint8Array([104, 101, 108, 108, 111, 0]) // "hello"
	;

	const encrypted /*: Uint8Array */ =
		await sidh.encrypt(plaintext, keyPair.publicKey)
	;

	const decrypted /*: Uint8Array */ =
		await sidh.decrypt(encrypted, keyPair.privateKey) // same as plaintext
	;

	console.log(keyPair);
	console.log(plaintext);
	console.log(encrypted);
	console.log(decrypted);

Note: SIDH is a low-level cryptographic primitive, not a high-level construct like libsodium's
[crypto_box](https://download.libsodium.org/doc/public-key_cryptography/authenticated_encryption.html).
This module can be combined with a symmetric cypher and a MAC to provide such a construct, but you
should avoid using sidh directly for anything important if you lack the experience to do so.
