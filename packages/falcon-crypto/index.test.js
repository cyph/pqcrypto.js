const targets = [
	{name: 'local build', falcon: require('.')},
	{name: 'stable release', falcon: require('falcon-crypto')}
];

const message = new Uint8Array([98, 97, 108, 108, 115, 0]);

const toHex = bytes => Buffer.from(bytes).toString('hex');

for (const {name, falcon} of targets) {
	test(`${name} key pair generation`, async () => falcon.keyPair());
}

for (const [keyPairTarget, signTarget, verifyTarget] of targets.flatMap(a =>
	targets.flatMap(b =>
		a === b ?
			[[a, a, a]] :
			[
				[a, a, b],
				[a, b, b]
			]
	)
)) {
	test(`end-to-end test (${keyPairTarget.name} key pair, ${signTarget.name} signing, ${verifyTarget.name} verification)`, async () => {
		const keyPair = await keyPairTarget.falcon.keyPair();

		const signed = await signTarget.falcon.sign(
			message,
			keyPair.privateKey
		);
		const verified = await verifyTarget.falcon.open(
			signed,
			keyPair.publicKey
		);

		const signature = await signTarget.falcon.signDetached(
			message,
			keyPair.privateKey
		);
		const isValid = await verifyTarget.falcon.verifyDetached(
			signature,
			message,
			keyPair.publicKey
		);

		expect(toHex(verified)).toBe(toHex(message));
		expect(isValid).toBe(true);
	});
}
