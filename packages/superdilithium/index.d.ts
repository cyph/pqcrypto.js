declare module 'superdilithium' {
	interface ISuperDilithium {
		/** Signature length. */
		bytes: Promise<number>;

		/** Hash length. */
		hashBytes: Promise<number>;

		/** Private key length. */
		privateKeyBytes: Promise<number>;

		/** Public key length. */
		publicKeyBytes: Promise<number>;

		/** Serializes key pair. */
		exportKeys (keyPair: {publicKey: Uint8Array}) : Promise<{
			private: {
				classical: null;
				combined: null;
				postQuantum: null;
			};
			public: {
				classical: string;
				combined: string;
				postQuantum: string;
			};
		}>;

		/**
		 * Serializes key pair with optional encryption.
		 * Using encryption (a password) requires native crypto support.
		 */
		exportKeys (
			keyPair: {
				privateKey: Uint8Array;
				publicKey: Uint8Array;
			},
			password?: string
		) : Promise<{
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
		}>;

		/** SHA-512 hash. */
		hash (message: Uint8Array|string, onlyBinary: true) : Promise<Uint8Array>;

		/** SHA-512 hash. */
		hash (message: Uint8Array|string, onlyBinary?: false) : Promise<{
			binary: Uint8Array;
			hex: string;
		}>;

		/** Imports exported keys and creates key pair object. */
		importKeys (keyData: {
			public: {classical: string; postQuantum: string}|{combined: string}
		}) : Promise<{
			privateKey: null;
			publicKey: Uint8Array;
		}>;

		/**
		 * Imports exported keys and creates key pair object.
		 * Using a password requires native crypto support.
		 */
		importKeys (
			keyData: {
				private: {classical: string; postQuantum: string}|{combined: string};
				public?: any;
			},
			password?: string
		) : Promise<{
			privateKey: Uint8Array;
			publicKey: Uint8Array;
		}>;

		/** Generates key pair. */
		keyPair () : Promise<{privateKey: Uint8Array; publicKey: Uint8Array}>;

		/** Verifies signed message against publicKey and returns it. */
		open (
			signed: Uint8Array|string,
			publicKey: Uint8Array|{
				public: {classical: string; postQuantum: string}|{combined: string}
			},
			additionalData?: Uint8Array|string,
			knownGoodHash?: Uint8Array|string
		) : Promise<Uint8Array>;

		/** Verifies signed message against publicKey and returns it. */
		open (
			signed: Uint8Array|string,
			publicKey: Uint8Array|{
				public: {classical: string; postQuantum: string}|{combined: string}
			},
			additionalData?: Uint8Array|string,
			knownGoodHash?: Uint8Array|string,
			includeHash: true
		) : Promise<{
			hash: Uint8Array;
			message: Uint8Array;
		}>;

		/** Verifies signed message against publicKey and returns it decoded to a string. */
		openString (
			signed: Uint8Array|string,
			publicKey: Uint8Array|{
				public: {classical: string; postQuantum: string}|{combined: string}
			},
			additionalData?: Uint8Array|string,
			knownGoodHash?: Uint8Array|string
		) : Promise<string>;

		/** Verifies signed message against publicKey and returns it decoded to a string. */
		openString (
			signed: Uint8Array|string,
			publicKey: Uint8Array|{
				public: {classical: string; postQuantum: string}|{combined: string}
			},
			additionalData?: Uint8Array|string,
			knownGoodHash?: Uint8Array|string,
			includeHash: true
		) : Promise<{
			hash: string;
			message: Uint8Array;
		}>;

		/** Signs message with privateKey and returns combined message. */
		sign (
			message: Uint8Array|string,
			privateKey: Uint8Array,
			additionalData?: Uint8Array|string
		) : Promise<Uint8Array>;

		/** Signs message with privateKey and returns combined message encoded as base64. */
		signBase64 (
			message: Uint8Array|string,
			privateKey: Uint8Array,
			additionalData?: Uint8Array|string
		) : Promise<string>;

		/** Signs message with privateKey and returns signature. */
		signDetached (
			message: Uint8Array|string,
			privateKey: Uint8Array,
			additionalData?: Uint8Array|string,
			preHashed?: boolean
		) : Promise<Uint8Array>;

		/** Signs message with privateKey and returns signature encoded as base64. */
		signDetachedBase64 (
			message: Uint8Array|string,
			privateKey: Uint8Array,
			additionalData?: Uint8Array|string,
			preHashed?: boolean
		) : Promise<string>;

		/** Verifies detached signature against publicKey. */
		verifyDetached (
			signature: Uint8Array|string,
			message: Uint8Array|string,
			publicKey: Uint8Array|{
				public: {classical: string; postQuantum: string}|{combined: string}
			},
			additionalData?: Uint8Array|string,
			knownGoodHash?: Uint8Array|string
		) : Promise<boolean>;

		/** Verifies detached signature against publicKey. */
		verifyDetached (
			signature: Uint8Array|string,
			message: Uint8Array|string,
			publicKey: Uint8Array|{
				public: {classical: string; postQuantum: string}|{combined: string}
			},
			additionalData?: Uint8Array|string,
			knownGoodHash?: Uint8Array|string,
			includeHash: true
		) : Promise<{
			hash: Uint8Array;
			valid: boolean;
		}>;
	}

	const superDilithium: ISuperDilithium;
}
