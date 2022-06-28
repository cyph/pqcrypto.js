declare module 'rlwe' {
	interface IRLWE {
		/** Length of shared secret. */
		bytes: Promise<number>;

		/** Private key length. */
		privateKeyBytes: Promise<number>;

		/** Public key length. */
		publicKeyBytes: Promise<number>;

		/** First step: generates key pair for Alice. */
		aliceKeyPair () : Promise<{privateKey: Uint8Array; publicKey: Uint8Array}>;

		/** Third step: uses Bob's public key to produce the same shared secret for Alice. */
		aliceSecret (publicKey: Uint8Array, privateKey: Uint8Array) : Promise<Uint8Array>;

		/** Second step: uses Alice's public key to generate public key and shared secret for Bob. */
		bobSecret (alicePublicKey: Uint8Array) : Promise<{publicKey: Uint8Array; secret: Uint8Array}>;
	}

	const rlwe: IRLWE;
}
