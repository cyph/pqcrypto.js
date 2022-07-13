const targets = [
	{name: 'local build', mceliece: require('.')},
	{name: 'stable release', mceliece: require('mceliece')}
];

const toHex = bytes => Buffer.from(bytes).toString('hex');

for (const {name, mceliece} of targets) {
	test(`${name} key pair generation`, async () => mceliece.keyPair());
}

for (const [keyPairTarget, encryptTarget, decryptTarget] of targets.flatMap(a =>
	targets.flatMap(b =>
		a === b ?
			[[a, a, a]] :
			[
				[a, a, b],
				[a, b, b]
			]
	)
)) {
	test(`end-to-end test (${keyPairTarget.name} key pair, ${encryptTarget.name} encryption, ${decryptTarget.name} decryption)`, async () => {
		const keyPair = await keyPairTarget.mceliece.keyPair();

		const {cyphertext, secret} = await encryptTarget.mceliece.encrypt(
			keyPair.publicKey
		);

		const decrypted = await decryptTarget.mceliece.decrypt(
			cyphertext,
			keyPair.privateKey
		);

		expect(toHex(decrypted)).toBe(toHex(secret));
	});
}
