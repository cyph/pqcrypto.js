# supersphincs

## Overview

SuperSPHINCS combines the post-quantum [SPHINCS+](https://sphincs.org) with the more conventional
elliptic-curve (ECC) [Ed25519](https://ed25519.cr.yp.to) as a single signing scheme. SPHINCS+ is
provided by [sphincs](https://github.com/cyph/pqcrypto.js/tree/master/packages/sphincs) and
Ed25519 signing is performed using [libsodium.js](https://github.com/jedisct1/libsodium.js).

Before signing, a SHA-512 hash is performed, using the current platform's native implementation
where available or an efficient JavaScript implementation from
[TweetNaCl.js](https://github.com/dchest/tweetnacl-js) otherwise.

## Example Usage

	import {superSphincs} from 'supersphincs';

	const keyPair /*: {privateKey: Uint8Array; publicKey: Uint8Array} */ =
		await superSphincs.keyPair()
	;

	const message /*: Uint8Array */ =
		new Uint8Array([104, 101, 108, 108, 111]) // "hello"
	;

	// Optional additional data argument, similar conceptually to what AEAD cyphers support.
	// If specified, must be the same when signing and verifying. For more information and
	// usage advice, see: https://download.libsodium.org/doc/secret-key_cryptography/aead.html
	const additionalData /*: Uint8Array */ =
		new Uint8Array([119, 111, 114, 108, 100]) // "world"
	;

	/* Combined signatures */

	const signed /*: Uint8Array */ =
		await superSphincs.sign(message, keyPair.privateKey, additionalData)
	;

	const verified /*: Uint8Array */ =
		await superSphincs.open(signed, keyPair.publicKey, additionalData) // same as message
	;

	/* Detached signatures */

	const signature /*: Uint8Array */ =
		await superSphincs.signDetached(message, keyPair.privateKey, additionalData)
	;

	const isValid /*: boolean */ =
		await superSphincs.verifyDetached(
			signature,
			message,
			keyPair.publicKey,
			additionalData
		) // true
	;

	/* Export and optionally encrypt keys */

	const keyData /*: {
		private: {
			classical: string;
			combined: string;
			postQuantum: string;
		};
		public: {
			classical: string;
			combined: string;
			postQuantum: string;
		};
	} */ =
		await superSphincs.exportKeys(keyPair, 'secret passphrase')
	;

	if (typeof localStorage === 'undefined') {
		localStorage	= {};
	}

	// May now save exported keys to disk (or whatever)
	localStorage.superSphincsPrivateKey = keyData.private.combined;
	localStorage.sphincsPrivateKey      = keyData.private.postQuantum;
	localStorage.eccPrivateKey          = keyData.private.classical;
	localStorage.superSphincsPublicKey  = keyData.public.combined;
	localStorage.sphincsPublicKey       = keyData.public.postQuantum;
	localStorage.eccPublicKey           = keyData.public.classical;


	/* Reconstruct an exported key using either the superSphincs
		value or any pair of valid sphincs and ecc values */

	const keyPair1 = await superSphincs.importKeys({
		public: {
			classical: localStorage.eccPublicKey,
			postQuantum: localStorage.sphincsPublicKey
		}
	});

	// May now use keyPair1.publicKey as in the above examples
	console.log('Import #1:');
	console.log(keyPair1);

	const keyPair2 = await superSphincs.importKeys(
		{
			private: {
				combined: localStorage.superSphincsPrivateKey
			}
		},
		'secret passphrase'
	);

	// May now use keyPair2 as in the above examples
	console.log('Import #2:');
	console.log(keyPair2);

	// Constructing an entirely new SuperSPHINCS key pair from
	// the original SPHINCS+ key pair and a new ECC key pair
	const keyPair3 = await superSphincs.importKeys(
		{
			private: {
				classical: (
					await superSphincs.exportKeys(
						await superSphincs.keyPair(),
						'hunter2'
					)
				).private.classical,
				postQuantum: localStorage.sphincsPrivateKey
			}
		},
		{
			classical: 'hunter2',
			postQuantum: 'secret passphrase'
		}
	);

	// May now use keyPair3 as in the above examples
	console.log('Import #3:');
	console.log(keyPair3);

## Changelog

Breaking changes in major versions:

8.0.0:

* Standardized method signatures across packages.

7.0.0:

* Upgraded from SPHINCS and RSA to SPHINCS+ and ECC. For backwards compatibility with
previous versions of this package, use
[supersphincs-legacy](https://github.com/cyph/pqcrypto.js/tree/master/packages/supersphincs-legacy).

6.0.0:

* Additional data optional with default value of `new Uint8Array(0)`.

5.0.0:

* Additional data format change.

4.0.0:

* As part of upgrading from asm.js to WebAssembly (with asm.js included as a fallback),
the API is fully asynchronous.

3.0.0:

* General API cleanup.

2.0.0:

* Split into module (supersphincs.js) and standalone pre-bundled version (dist/supersphincs.js).
