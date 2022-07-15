declare module 'fast-sha512' {
	interface IFastSHA512 {
		/** Hash length. */
		bytes: number;

		/** SHA-512 hash. */
		baseHash (message: Uint8Array, shouldClearMessage: boolean) : Promise<Uint8Array>;

		/** SHA-512 hash. */
		hash (message: Uint8Array|string, onlyBinary: true) : Promise<Uint8Array>;

		/** SHA-512 hash. */
		hash (message: Uint8Array|string, onlyBinary?: false) : Promise<{
			binary: Uint8Array;
			hex: string;
		}>;
	}

	const fastSHA512: IFastSHA512;
}
