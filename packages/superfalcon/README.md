# superfalcon

## Overview

SuperFALCON combines the post-quantum [FALCON](https://falcon.cr.yp.to) with the more conventional
[RSASSA-PKCS1-v1_5](https://tools.ietf.org/html/rfc3447#section-8.2) as a single signing scheme.
FALCON is provided by [falcon](https://github.com/cyph/pqcrypto.js/tree/master/packages/falcon)
and RSA signing is performed using
[rsasign](https://github.com/cyph/pqcrypto.js/tree/master/packages/rsasign).

Before signing, a SHA-512 hash is performed, using the current platform's native implementation
where available or an efficient JavaScript implementation from
[TweetNaCl.js](https://github.com/dchest/tweetnacl-js) otherwise.

## Example Usage

	(async () => {
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
				rsa: string;
				falcon: string;
				superFalcon: string;
			};
			public: {
				rsa: string;
				falcon: string;
				superFalcon: string;
			};
		} */ =
			await superFalcon.exportKeys(keyPair, 'secret passphrase')
		;

		if (typeof localStorage === 'undefined') {
			localStorage	= {};
		}

		// May now save exported keys to disk (or whatever)
		localStorage.superFalconPrivateKey = keyData.private.superFalcon;
		localStorage.falconPrivateKey      = keyData.private.falcon;
		localStorage.rsaPrivateKey          = keyData.private.rsa;
		localStorage.superFalconPublicKey  = keyData.public.superFalcon;
		localStorage.falconPublicKey       = keyData.public.falcon;
		localStorage.rsaPublicKey           = keyData.public.rsa;


		/* Reconstruct an exported key using either the superFalcon
			value or any pair of valid falcon and rsa values */

		const keyPair1 = await superFalcon.importKeys({
			public: {
				rsa: localStorage.rsaPublicKey,
				falcon: localStorage.falconPublicKey
			}
		});

		// May now use keyPair1.publicKey as in the above examples
		console.log('Import #1:');
		console.log(keyPair1);

		const keyPair2 = await superFalcon.importKeys(
			{
				private: {
					superFalcon: localStorage.superFalconPrivateKey
				}
			},
			'secret passphrase'
		);

		// May now use keyPair2 as in the above examples
		console.log('Import #2:');
		console.log(keyPair2);

		// Constructing an entirely new SuperFALCON key pair from
		// the original FALCON key pair and a new RSA key pair
		const keyPair3 = await superFalcon.importKeys(
			{
				private: {
					rsa: (
						await superFalcon.exportKeys(
							await superFalcon.keyPair(),
							'hunter2'
						)
					).private.rsa,
					falcon: localStorage.falconPrivateKey
				}
			},
			{
				rsa: 'hunter2',
				falcon: 'secret passphrase'
			}
		);

		// May now use keyPair3 as in the above examples
		console.log('Import #3:');
		console.log(keyPair3);
	})();

## Changelog

Breaking changes in major versions:

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

* Split into module (superfalcon.js) and standalone pre-bundled version (dist/superfalcon.js).
