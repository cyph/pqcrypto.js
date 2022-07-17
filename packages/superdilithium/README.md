# superdilithium

## Overview

SuperDilithium combines the post-quantum [Dilithium](https://pq-crystals.org/dilithium) with
the more conventional elliptic-curve (ECC) [Ed25519](https://ed25519.cr.yp.to) as a single
signing scheme. Dilithium is provided by
[dilithium-crystals](https://github.com/cyph/pqcrypto.js/tree/master/packages/dilithium-crystals)
and Ed25519 signing is performed using [libsodium.js](https://github.com/jedisct1/libsodium.js).

Before signing, a SHA-512 hash is performed, using the current platform's native implementation
where available or an efficient JavaScript implementation from
[TweetNaCl.js](https://github.com/dchest/tweetnacl-js) otherwise.

## Example Usage

	import {superDilithium} from 'superdilithium';

	const keyPair /*: {privateKey: Uint8Array; publicKey: Uint8Array} */ =
		await superDilithium.keyPair()
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
		await superDilithium.sign(message, keyPair.privateKey, additionalData)
	;

	const verified /*: Uint8Array */ =
		await superDilithium.open(signed, keyPair.publicKey, additionalData) // same as message
	;

	/* Detached signatures */

	const signature /*: Uint8Array */ =
		await superDilithium.signDetached(message, keyPair.privateKey, additionalData)
	;

	const isValid /*: boolean */ =
		await superDilithium.verifyDetached(
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
		await superDilithium.exportKeys(keyPair, 'secret passphrase')
	;

	if (typeof localStorage === 'undefined') {
		localStorage	= {};
	}

	// May now save exported keys to disk (or whatever)
	localStorage.superDilithiumPrivateKey = keyData.private.combined;
	localStorage.dilithiumPrivateKey      = keyData.private.postQuantum;
	localStorage.eccPrivateKey            = keyData.private.classical;
	localStorage.superDilithiumPublicKey  = keyData.public.combined;
	localStorage.dilithiumPublicKey       = keyData.public.postQuantum;
	localStorage.eccPublicKey             = keyData.public.classical;


	/* Reconstruct an exported key using either the superDilithium
		value or any pair of valid dilithium and ecc values */

	const keyPair1 = await superDilithium.importKeys({
		public: {
			classical: localStorage.eccPublicKey,
			postQuantum: localStorage.dilithiumPublicKey
		}
	});

	// May now use keyPair1.publicKey as in the above examples
	console.log('Import #1:');
	console.log(keyPair1);

	const keyPair2 = await superDilithium.importKeys(
		{
			private: {
				combined: localStorage.superDilithiumPrivateKey
			}
		},
		'secret passphrase'
	);

	// May now use keyPair2 as in the above examples
	console.log('Import #2:');
	console.log(keyPair2);

	// Constructing an entirely new SuperDilithium key pair from
	// the original Dilithium key pair and a new ECC key pair
	const keyPair3 = await superDilithium.importKeys(
		{
			private: {
				classical: (
					await superDilithium.exportKeys(
						await superDilithium.keyPair(),
						'hunter2'
					)
				).private.classical,
				postQuantum: localStorage.dilithiumPrivateKey
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

2.0.0:

* Standardized method signatures across packages.
