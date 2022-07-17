# supersphincs-legacy

## Note

The original SPHINCS algorithm is ***no longer recommended***. Instead prefer SPHINCS+, which
has been selected by NIST for standardization. An implementation with SPHINCS+ is provided by
[supersphincs](https://github.com/cyph/pqcrypto.js/tree/master/packages/supersphincs).

---

## Overview

SuperSPHINCS combines the post-quantum [SPHINCS](https://sphincs.cr.yp.to) with the more conventional
[RSASSA-PKCS1-v1_5](https://tools.ietf.org/html/rfc3447#section-8.2) as a single signing scheme.
SPHINCS is provided by [sphincs-legacy](https://github.com/cyph/pqcrypto.js/tree/master/packages/sphincs-legacy)
and RSA signing is performed using
[rsasign](https://github.com/cyph/pqcrypto.js/tree/master/packages/rsasign).

Before signing, a SHA-512 hash is performed, using the current platform's native implementation
where available or an efficient JavaScript implementation from
[TweetNaCl.js](https://github.com/dchest/tweetnacl-js) otherwise.

## Example Usage

	import {superSphincs} from 'supersphincs-legacy';

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
	localStorage.rsaPrivateKey          = keyData.private.classical;
	localStorage.superSphincsPublicKey  = keyData.public.combined;
	localStorage.sphincsPublicKey       = keyData.public.postQuantum;
	localStorage.rsaPublicKey           = keyData.public.classical;


	/* Reconstruct an exported key using either the superSphincs
		value or any pair of valid sphincs and rsa values */

	const keyPair1 = await superSphincs.importKeys({
		public: {
			classical: localStorage.rsaPublicKey,
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
	// the original SPHINCS key pair and a new RSA key pair
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

2.0.0:

* Standardized method signatures across packages.
