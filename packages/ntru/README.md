# ntru

## Overview

The [NTRU Prime](https://ntruprime.cr.yp.to) post-quantum asymmetric
cypher compiled to WebAssembly using [Emscripten](https://github.com/kripken/emscripten).
A simple JavaScript wrapper is provided to make NTRU easy to use in web applications.

The default parameter set is sntrup1277 (roughly 256-bit strength).

## Example Usage

	import {ntru} from 'ntru';

	const keyPair /*: {privateKey: Uint8Array; publicKey: Uint8Array} */ =
		await ntru.keyPair()
	;

	const {cyphertext, secret} /*: {cyphertext: Uint8Array; secret: Uint8Array} */ =
		await ntru.encrypt(keyPair.publicKey)
	;

	const decrypted /*: Uint8Array */ =
		await ntru.decrypt(cyphertext, keyPair.privateKey) // same as secret
	;

	console.log(keyPair);
	console.log(secret);
	console.log(cyphertext);
	console.log(decrypted);

## Changelog

Breaking changes in major versions:

4.0.0:

* Switched from legacy NTRUEncrypt to NTRU Prime. For backwards compatibility with previous versions
of this package, use
[ntru-legacy](https://github.com/cyph/pqcrypto.js/tree/master/packages/ntru-legacy).

3.0.0:

* As part of upgrading from asm.js to WebAssembly (with asm.js included as a fallback),
the API is fully asynchronous.

2.0.0:

* Removed some undocumented functions as part of minor API cleanup.
