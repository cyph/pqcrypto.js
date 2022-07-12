const targets = [
	{name: 'local build', hqc: require('.')},
	{name: 'stable release', hqc: require('hqc')}
];

const toHex = bytes => Buffer.from(bytes).toString('hex');

for (const {name, hqc} of targets) {
	test(`${name} key pair generation`, async () => hqc.keyPair());
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
		const keyPair = await keyPairTarget.hqc.keyPair();

		const {cyphertext, secret} = await encryptTarget.hqc.encrypt(
			keyPair.publicKey
		);

		const decrypted = await decryptTarget.hqc.decrypt(
			cyphertext,
			keyPair.privateKey
		);

		expect(toHex(decrypted)).toBe(toHex(secret));
	});
}
