# superfalcon

## Overview

SuperFalcon combines the post-quantum [Falcon](https://falcon-sign.info) with the more conventional
elliptic-curve (ECC) [Ed25519](https://ed25519.cr.yp.to) as a single signing scheme. Falcon is
provided by [falcon-crypto](https://github.com/cyph/pqcrypto.js/tree/master/packages/falcon-crypto)
and Ed25519 signing is performed using [libsodium.js](https://github.com/jedisct1/libsodium.js).

Before signing, a SHA-512 hash is performed, using the current platform's native implementation
where available or an efficient JavaScript implementation from
[TweetNaCl.js](https://github.com/dchest/tweetnacl-js) otherwise.

## Example Usage

	import {superFalcon} from 'superfalcon';

	const keyPair /*: {privateKey: Uint8Array; publicKey: Uint8Array} */ =
		await superFalcon.keyPair()
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
		await superFalcon.sign(message, keyPair.privateKey, additionalData)
	;

	const verified /*: Uint8Array */ =
		await superFalcon.open(signed, keyPair.publicKey, additionalData) // same as message
	;

	/* Detached signatures */

	const signature /*: Uint8Array */ =
		await superFalcon.signDetached(message, keyPair.privateKey, additionalData)
	;

	const isValid /*: boolean */ =
		await superFalcon.verifyDetached(
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
		await superFalcon.exportKeys(keyPair, 'secret passphrase')
	;

	if (typeof localStorage === 'undefined') {
		localStorage	= {};
	}

	// May now save exported keys to disk (or whatever)
	localStorage.superFalconPrivateKey = keyData.private.combined;
	localStorage.falconPrivateKey      = keyData.private.postQuantum;
	localStorage.eccPrivateKey         = keyData.private.classical;
	localStorage.superFalconPublicKey  = keyData.public.combined;
	localStorage.falconPublicKey       = keyData.public.postQuantum;
	localStorage.eccPublicKey          = keyData.public.classical;


	/* Reconstruct an exported key using either the superFalcon
		value or any pair of valid falcon and ecc values */

	const keyPair1 = await superFalcon.importKeys({
		public: {
			classical: localStorage.eccPublicKey,
			postQuantum: localStorage.falconPublicKey
		}
	});

	// May now use keyPair1.publicKey as in the above examples
	console.log('Import #1:');
	console.log(keyPair1);

	const keyPair2 = await superFalcon.importKeys(
		{
			private: {
				combined: localStorage.superFalconPrivateKey
			}
		},
		'secret passphrase'
	);

	// May now use keyPair2 as in the above examples
	console.log('Import #2:');
	console.log(keyPair2);

	// Constructing an entirely new SuperFalcon key pair from
	// the original Falcon key pair and a new ECC key pair
	const keyPair3 = await superFalcon.importKeys(
		{
			private: {
				classical: (
					await superFalcon.exportKeys(
						await superFalcon.keyPair(),
						'hunter2'
					)
				).private.classical,
				postQuantum: localStorage.falconPrivateKey
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
