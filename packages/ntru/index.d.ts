declare module 'ntru' {
	interface INTRU {
		/** Secret length. */
		bytes: Promise<number>;

		/** Cyphertext length. */
		cyphertextBytes: Promise<number>;

		/** Private key length. */
		privateKeyBytes: Promise<number>;

		/** Public key length. */
		publicKeyBytes: Promise<number>;

		/** Decrypts secret with privateKey. */
		decrypt (cyphertext: Uint8Array|string, privateKey: Uint8Array) : Promise<Uint8Array>;

		/** Encrypts secret with publicKey. */
		encrypt (publicKey: Uint8Array) : Promise<{cyphertext: Uint8Array; secret: Uint8Array}>;

		/** Generates key pair. */
		keyPair () : Promise<{privateKey: Uint8Array; publicKey: Uint8Array}>;
	}

	const ntru: INTRU;
}
