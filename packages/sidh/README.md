# sidh.js

## Overview

The [SIDH](https://en.wikipedia.org/wiki/Supersingular_isogeny_key_exchange) post-quantum asymmetric
cipher compiled to WebAssembly using [Emscripten](https://github.com/kripken/emscripten).
The specific implementation in use is [from Microsoft Research](https://www.microsoft.com/en-us/research/project/sidh-library).
A simple JavaScript wrapper is provided to make SIDH easy to use in web applications.

The parameters are configured to 128-bit strength (SIKEp503).

## Example Usage

	(async () => {
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
	})();

Note: SIDH is a low-level cryptographic primitive, not a high-level construct like libsodium's
[crypto_box](https://download.libsodium.org/doc/public-key_cryptography/authenticated_encryption.html).
This module can be combined with a symmetric cipher and a MAC to provide such a construct, but you
should avoid using sidh.js directly for anything important if you lack the experience to do so.

## Changelog

Breaking changes in major versions:

5.0.0:

* Upgraded to SIDH 3.0 and built new API in line with mceliece.js and ntru.js.

4.0.0:

* As part of upgrading from asm.js to WebAssembly (with asm.js included as a fallback),
the API is fully asynchronous.

3.0.0:

* Removed some undocumented functions as part of minor API cleanup.

2.0.0:

* Upgraded to SIDH 2.0.
