# rlwe

## Note

This package is ***no longer recommended***. The underlying implementation is unmaintained,
meanwhile more robust alternatives have been developed. Instead prefer Kyber, a NIST PQC winner
that has been selected for standardization. An implementation of Kyber is provided by
[kyber-crystals](https://github.com/cyph/pqcrypto.js/tree/master/packages/kyber-crystals).

---

## Overview

The [RLWE](https://en.wikipedia.org/wiki/Ring_learning_with_errors_key_exchange) post-quantum asymmetric
cypher compiled to WebAssembly using [Emscripten](https://github.com/kripken/emscripten).
The specific implementation in use is [from Microsoft Research](https://research.microsoft.com/en-us/projects/latticecrypto).
A simple JavaScript wrapper is provided to make RLWE easy to use in web applications.

The parameters are configured to 128-bit strength. (More specifically, the security level is
128 quantum bits and 128 classical bits.)

## Example Usage

	import {rlwe} from 'rlwe';

	const aliceKeyPair /*: {privateKey: Uint8Array; publicKey: Uint8Array} */ =
		await rlwe.aliceKeyPair()
	;

	const bob /*: {publicKey: Uint8Array; secret: Uint8Array} */ =
		await rlwe.bobSecret(aliceKeyPair.publicKey)
	;

	const aliceSecret /*: Uint8Array */ =
		await rlwe.aliceSecret(bob.publicKey, aliceKeyPair.privateKey) // equal to bob.secret
	;

	console.log(aliceKeyPair);
	console.log(bob);
	console.log(aliceSecret);

Note: This library only handles generating shared secrets; you'll need to handle key derivation
and symmetric encryption from there.

## Changelog

Breaking changes in major versions:

3.0.0:

* As part of upgrading from asm.js to WebAssembly (with asm.js included as a fallback),
the API is fully asynchronous.

2.0.0:

* Removed some undocumented functions as part of minor API cleanup.
