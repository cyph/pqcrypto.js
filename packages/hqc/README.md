# hqc

## Overview

The [HQC](https://pqc-hqc.org) post-quantum asymmetric
cypher compiled to WebAssembly using [Emscripten](https://github.com/kripken/emscripten).
A simple JavaScript wrapper is provided to make HQC easy to use in web applications.

The default parameter set is HQC-RMRS-256 (roughly 256-bit strength).

## Example Usage

	import {hqc} from 'hqc';

	const keyPair /*: {privateKey: Uint8Array; publicKey: Uint8Array} */ =
		await hqc.keyPair()
	;

	const {cyphertext, secret} /*: {cyphertext: Uint8Array; secret: Uint8Array} */ =
		await hqc.encrypt(keyPair.publicKey)
	;

	const decrypted /*: Uint8Array */ =
		await hqc.decrypt(cyphertext, keyPair.privateKey) // same as secret
	;

	console.log(keyPair);
	console.log(secret);
	console.log(cyphertext);
	console.log(decrypted);
