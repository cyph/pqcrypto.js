# ntru-legacy

## Note

The variant of NTRU used in this package is ***no longer recommended***. Instead prefer
NTRU Prime, which is a more robust alternative that was a NIST PQC Round 3 alternate
candidate. An implementation of NTRU Prime is provided by
[ntru](https://github.com/cyph/pqcrypto.js/tree/master/packages/ntru).

That being said, unless you specifically require NTRU, Kyber should be preferred. Kyber is
a NIST PQC winner that has been selected for standardization, and works similarly to NTRU
(both being lattice-based). An implementation of Kyber is provided by
[kyber-crystals](https://github.com/cyph/pqcrypto.js/tree/master/packages/kyber-crystals).

---

## Overview

The [NTRU](https://ntru.org) post-quantum asymmetric
cypher compiled to WebAssembly using [Emscripten](https://github.com/kripken/emscripten).
A simple JavaScript wrapper is provided to make NTRU easy to use in web applications.

The default parameter set is EES743EP1 (roughly 256-bit strength, as per
[NTRU's documentation](https://github.com/buu700/NTRUEncrypt-Archive/blob/master/doc/UserNotes-NTRUEncrypt.pdf)).

## Example Usage

	import {ntru} from 'ntru-legacy';

	const keyPair /*: {privateKey: Uint8Array; publicKey: Uint8Array} */ =
		await ntru.keyPair()
	;

	const plaintext /*: Uint8Array */ =
		new Uint8Array([104, 101, 108, 108, 111, 0]) // "hello"
	;

	const encrypted /*: Uint8Array */ =
		await ntru.encrypt(plaintext, keyPair.publicKey)
	;

	const decrypted /*: Uint8Array */ =
		await ntru.decrypt(encrypted, keyPair.privateKey) // same as plaintext
	;

	console.log(keyPair);
	console.log(plaintext);
	console.log(encrypted);
	console.log(decrypted);

Note: NTRU is a low-level cryptographic primitive, not a high-level construct like libsodium's
[crypto_box](https://download.libsodium.org/doc/public-key_cryptography/authenticated_encryption.html).
This module can be combined with a symmetric cypher and a MAC to provide such a construct, but you
should avoid using ntru directly for anything important if you lack the experience to do so.
