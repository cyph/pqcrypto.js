# rlwe

## Overview

The [RLWE](https://en.wikipedia.org/wiki/Ring_learning_with_errors_key_exchange) post-quantum asymmetric
cypher compiled to WebAssembly using [Emscripten](https://github.com/kripken/emscripten).
The specific implementation in use is [from Microsoft Research](https://research.microsoft.com/en-us/projects/latticecrypto).
A simple JavaScript wrapper is provided to make RLWE easy to use in web applications.

The parameters are configured to 128-bit strength. (More specifically, the security level is
128 quantum bits and 128 classical bits.)

## Example Usage

	(async () => {
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
	})();

Note: This library only handles generating shared secrets; you'll need to handle key derivation
and symmetric encryption from there.

## Changelog

Breaking changes in major versions:

3.0.0:

* As part of upgrading from asm.js to WebAssembly (with asm.js included as a fallback),
the API is fully asynchronous.

2.0.0:

* Removed some undocumented functions as part of minor API cleanup.
