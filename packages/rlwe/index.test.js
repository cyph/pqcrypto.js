const targets = [
	{name: 'local build', rlwe: require('.')},
	{name: 'stable release', rlwe: require('rlwe')}
];

const toHex = bytes => Buffer.from(bytes).toString('hex');

for (const {name, rlwe} of targets) {
	test(`${name} key pair generation`, async () => rlwe.aliceKeyPair());
}

for (const [
	keyPairTarget,
	bobSecretTarget,
	aliceSecretTarget
] of targets.flatMap(a =>
	targets.flatMap(b =>
		a === b ?
			[[a, a, a]] :
			[
				[a, a, b],
				[a, b, b]
			]
	)
)) {
	test(`end-to-end test (${keyPairTarget.name} key pair, ${bobSecretTarget.name} Bob secret, ${aliceSecretTarget.name} Alice secret)`, async () => {
		const aliceKeyPair = await keyPairTarget.rlwe.aliceKeyPair();

		const bob = await bobSecretTarget.rlwe.bobSecret(
			aliceKeyPair.publicKey
		);

		const aliceSecret = await aliceSecretTarget.rlwe.aliceSecret(
			bob.publicKey,
			aliceKeyPair.privateKey
		);

		expect(toHex(bob.secret)).toBe(toHex(aliceSecret));
	});
}
