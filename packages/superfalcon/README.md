# superfalcon

## Overview

SuperFALCON combines the post-quantum [FALCON](https://falcon.cr.yp.to) with the more conventional
[ECCSSA-PKCS1-v1_5](https://tools.ietf.org/html/rfc3447#section-8.2) as a single signing scheme.
FALCON is provided by [falcon](https://github.com/cyph/pqcrypto.js/tree/master/packages/falcon)
and ECC signing is performed using
[eccsign](https://github.com/cyph/pqcrypto.js/tree/master/packages/eccsign).

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
				ecc: string;
				falcon: string;
				superFalcon: string;
			};
			public: {
				ecc: string;
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
		localStorage.eccPrivateKey          = keyData.private.ecc;
		localStorage.superFalconPublicKey  = keyData.public.superFalcon;
		localStorage.falconPublicKey       = keyData.public.falcon;
		localStorage.eccPublicKey           = keyData.public.ecc;


		/* Reconstruct an exported key using either the superFalcon
			value or any pair of valid falcon and ecc values */

		const keyPair1 = await superFalcon.importKeys({
			public: {
				ecc: localStorage.eccPublicKey,
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
		// the original FALCON key pair and a new ECC key pair
		const keyPair3 = await superFalcon.importKeys(
			{
				private: {
					ecc: (
						await superFalcon.exportKeys(
							await superFalcon.keyPair(),
							'hunter2'
						)
					).private.ecc,
					falcon: localStorage.falconPrivateKey
				}
			},
			{
				ecc: 'hunter2',
				falcon: 'secret passphrase'
			}
		);

		// May now use keyPair3 as in the above examples
		console.log('Import #3:');
		console.log(keyPair3);
	})();
