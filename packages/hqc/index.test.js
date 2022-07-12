const targets = [
	{name: 'local build', kyber: require('.')},
	{name: 'stable release', kyber: require('kyber-crystals')}
];

const toHex = bytes => Buffer.from(bytes).toString('hex');

for (const {name, kyber} of targets) {
	test(`${name} key pair generation`, async () => kyber.keyPair());
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
		const keyPair = await keyPairTarget.kyber.keyPair();

		const {cyphertext, secret} = await encryptTarget.kyber.encrypt(
			keyPair.publicKey
		);

		const decrypted = await decryptTarget.kyber.decrypt(
			cyphertext,
			keyPair.privateKey
		);

		expect(toHex(decrypted)).toBe(toHex(secret));
	});
}
