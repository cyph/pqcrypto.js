const targets = [
	{name: 'local build', superSphincs: require('.')},
	{name: 'stable release', superSphincs: require('supersphincs')}
];

const message = new Uint8Array([98, 97, 108, 108, 115, 0]);

const toHex = bytes => Buffer.from(bytes).toString('hex');

for (const {name, superSphincs} of targets) {
	test(`${name} key pair generation`, async () => superSphincs.keyPair());
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
		const keyPair = await keyPairTarget.superSphincs.keyPair();

		const signed = await signTarget.superSphincs.sign(
			message,
			keyPair.privateKey
		);
		const verified = await verifyTarget.superSphincs.open(
			signed,
			keyPair.publicKey
		);

		const signature = await signTarget.superSphincs.signDetached(
			message,
			keyPair.privateKey
		);
		const isValid = await verifyTarget.superSphincs.verifyDetached(
			signature,
			message,
			keyPair.publicKey
		);

		expect(toHex(verified)).toBe(toHex(message));
		expect(isValid).toBe(true);
	});
}
