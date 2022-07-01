declare module 'sodiumutil' {
	interface ISodiumUtil {
		/** Converts base64 string into binary byte array. */
		from_base64 (s: string) : Uint8Array;

		/** Converts base64url string into binary byte array. */
		from_base64url (s: string) : Uint8Array;

		/** Converts hex string into binary byte array. */
		from_hex (s: string) : Uint8Array;

		/** Converts ASCII/Unicode string into binary byte array. */
		from_string (s: string) : Uint8Array;

		/** Indicates whether two blocks of memory contain the same data. */
		memcmp (a: Uint8Array, b: Uint8Array) : boolean;

		/** Zeroes out memory. */
		memzero (a: Uint8Array) : void;

		/** Converts binary into base64 string. */
		to_base64 (a: Uint8Array) : string;

		/** Converts binary into base64url string. */
		to_base64url (a: Uint8Array) : string;

		/** Converts binary into hex string. */
		to_hex (a: Uint8Array) : string;

		/** Converts binary into ASCII/Unicode string. */
		to_string (a: Uint8Array) : string;
	}

	const sodiumUtil: ISodiumUtil;
}
