const targets = [
	{name: 'local build', ntru: require('.')},
	{name: 'stable release', ntru: require('ntru')}
];

const toHex = bytes => Buffer.from(bytes).toString('hex');

for (const {name, ntru} of targets) {
	test(`${name} key pair generation`, async () => ntru.keyPair());
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
		const keyPair = await keyPairTarget.ntru.keyPair();

		const {cyphertext, secret} = await encryptTarget.ntru.encrypt(
			keyPair.publicKey
		);

		const decrypted = await decryptTarget.ntru.decrypt(
			cyphertext,
			keyPair.privateKey
		);

		expect(toHex(decrypted)).toBe(toHex(secret));
	});
}
