#include <stdint.h>

long asm_test () {
	return 0;
}

long generate (
	char password[],
	long numWords,
	uint32_t randomValues[],
	char* wordList[],
	long wordLengths[],
	long wordListLength,
	long maxWordLength
) {
	long passwordLength	= 0;

	for (long i = 0 ; i < numWords ; ++i) {
		long index		= randomValues[i] / 4294967296.0 * wordListLength;
		char* word		= wordList[index];
		long wordLength	= wordLengths[index];

		for (long j = 0 ; j < maxWordLength ; ++j) {
			passwordLength += 2;

			if (j < wordLength) {
				password[passwordLength - 2]	= word[j];
				passwordLength -= 1;
			}
			else {
				password[passwordLength - 1]	= word[0];
				passwordLength -= 2;
			}
		}

		/* Append ascii space */
		password[passwordLength++]	= 32;
	}

	return passwordLength - 1;
}
