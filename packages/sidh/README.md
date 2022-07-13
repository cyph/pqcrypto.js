# sidh

## Overview

The [SIDH](https://en.wikipedia.org/wiki/Supersingular_isogeny_key_exchange) post-quantum asymmetric
cypher compiled to WebAssembly using [Emscripten](https://github.com/kripken/emscripten).
The specific implementation in use is [from Microsoft Research](https://github.com/Microsoft/PQCrypto-SIDH).
A simple JavaScript wrapper is provided to make SIDH easy to use in web applications.

The default parameter set is SIKEp751 (roughly 256-bit strength).

## Example Usage

	import {sidh} from 'sidh';

	const keyPair /*: {privateKey: Uint8Array; publicKey: Uint8Array} */ =
		await sidh.keyPair()
	;

	const {cyphertext, secret} /*: {cyphertext: Uint8Array; secret: Uint8Array} */ =
		await sidh.encrypt(keyPair.publicKey)
	;

	const decrypted /*: Uint8Array */ =
		await sidh.decrypt(cyphertext, keyPair.privateKey) // same as secret
	;

	console.log(keyPair);
	console.log(secret);
	console.log(cyphertext);
	console.log(decrypted);

## Changelog

Breaking changes in major versions:

6.0.0:

* Upgraded to SIDH 3.5.1 with stronger parameters and a KEM API. For backwards compatibility
with previous versions of this package, use
[sidh-legacy](https://github.com/cyph/pqcrypto.js/tree/master/packages/sidh-legacy).

5.0.0:

* Upgraded to SIDH 3.0 and built new API in line with mceliece and ntru.

4.0.0:

* As part of upgrading from asm.js to WebAssembly (with asm.js included as a fallback),
the API is fully asynchronous.

3.0.0:

* Removed some undocumented functions as part of minor API cleanup.

2.0.0:

* Upgraded to SIDH 2.0.
