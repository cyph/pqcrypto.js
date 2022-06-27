;

function writeArrayToMemory (array, buffer) {
	Module.HEAP8.set(array, buffer);
}


var cachedWordLists	= {};

function dataFree (buffer, dataToClear) {
	try {
		if (typeof dataToClear === 'number') {
			sodiumUtil.memzero(new Uint8Array(Module.HEAPU8.buffer, buffer, dataToClear));
		}
		else if (dataToClear) {
			sodiumUtil.memzero(new Uint8Array(Module.HEAPU8.buffer, buffer, dataToClear.length));
			sodiumUtil.memzero(dataToClear);
		}

		Module._free(buffer);
	}
	catch (err) {
		setTimeout(function () { throw err; }, 0);
	}
}

function getRandomValues (n) {
	return isNode ?
		new Uint8Array(nodeCrypto.randomBytes(n).buffer) :
		crypto.getRandomValues(new Uint8Array(n))
	;
}

function processWordList (wordList) {
	if (!wordList) {
		wordList	= xkcdPassphrase.defaultWordList;
	}

	if (wordList.length < 2) {
		throw new Error('Cannot use empty or one-word word list.');
	}

	var k	= JSON.stringify(wordList);

	if (!cachedWordLists[k]) {
		cachedWordLists[k]	= {
			buffer: Module._malloc(wordList.length * 4),
			length: wordList.length,
			lengthsBuffer: Module._malloc(wordList.length * 4),
			maxWordLength: wordList.reduce(function (a, b) {
				return Math.max(a, b.length);
			}, 0)
		};

		writeArrayToMemory(
			new Uint8Array(
				new Uint32Array(wordList.map(function (s) {
					var buffer	= Module._malloc(s.length);
					Module.writeAsciiToMemory(s, buffer);
					return buffer;
				})).buffer
			),
			cachedWordLists[k].buffer
		);

		writeArrayToMemory(
			new Uint8Array(
				new Uint32Array(wordList.map(function (s) {
					return s.length;
				})).buffer
			),
			cachedWordLists[k].lengthsBuffer
		);
	}

	return cachedWordLists[k];
}


var initiated	= Module.ready;


var xkcdPassphrase	= {
	defaultBits: 128,
	defaultWordList: DEFAULT_WORD_LIST,

	generate: function (numBits, wordList) { return initiated.then(function () {
		if (!numBits) {
			numBits		= xkcdPassphrase.defaultBits;
		}

		return xkcdPassphrase.generateWithWordCount(
			Math.round(numBits / Math.log2(processWordList(wordList).length)),
			wordList
		);
	}); },

	generateWithWordCount: function (numWords, wordList) { return initiated.then(function () {
		if (!numWords) {
			throw new Error('Word count must be specified.');
		}

		var wordListData		= processWordList(wordList);

		var passwordLength		= numWords * (wordListData.maxWordLength + 1);
		var password			= Module._malloc(passwordLength);

		var randomValues		= getRandomValues(numWords * 4);
		var randomValuesBuffer	= Module._malloc(randomValues.length);

		writeArrayToMemory(randomValues, randomValuesBuffer);

		try {
			var returnValue	= Module._generate(
				password,
				numWords,
				randomValuesBuffer,
				wordListData.buffer,
				wordListData.lengthsBuffer,
				wordListData.length,
				wordListData.maxWordLength
			);

			if (returnValue > 0) {
				return Module.UTF8ToString(password, returnValue);
			}
			else {
				throw new Error('xkcd passphrase error: ' + returnValue);
			}
		}
		finally {
			dataFree(password, passwordLength);
			dataFree(randomValuesBuffer, randomValues);
		}
	}); }
};



return xkcdPassphrase;

}());


if (typeof module !== 'undefined' && module.exports) {
	xkcdPassphrase.xkcdPassphrase	= xkcdPassphrase;
	module.exports					= xkcdPassphrase;
}
else {
	self.xkcdPassphrase	= xkcdPassphrase;
}


}());
