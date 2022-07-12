declare module 'dilithium-crystals' {
	interface IDilithium {
		/** Signature length. */
		bytes: Promise<number>;

		/** Private key length. */
		privateKeyBytes: Promise<number>;

		/** Public key length. */
		publicKeyBytes: Promise<number>;

		/** Generates key pair. */
		keyPair () : Promise<{privateKey: Uint8Array; publicKey: Uint8Array}>;

		/** Verifies signed message against publicKey and returns it. */
		open (signed: Uint8Array, publicKey: Uint8Array) : Promise<Uint8Array>;

		/** Signs message with privateKey and returns combined message. */
		sign (message: Uint8Array, privateKey: Uint8Array) : Promise<Uint8Array>;

		/** Signs message with privateKey and returns signature. */
		signDetached (message: Uint8Array, privateKey: Uint8Array) : Promise<Uint8Array>;

		/** Verifies detached signature against publicKey. */
		verifyDetached (
			signature: Uint8Array,
			message: Uint8Array,
			publicKey: Uint8Array
		) : Promise<boolean>;
	}

	const dilithium: IDilithium;
}
