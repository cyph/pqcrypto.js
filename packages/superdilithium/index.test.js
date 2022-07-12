const targets = [
	{name: 'local build', superDilithium: require('.')},
	{name: 'stable release', superDilithium: require('superdilithium')}
];

const message = new Uint8Array([98, 97, 108, 108, 115, 0]);

const toHex = bytes => Buffer.from(bytes).toString('hex');

for (const {name, superDilithium} of targets) {
	test(`${name} key pair generation`, async () => superDilithium.keyPair());
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
		const keyPair = await keyPairTarget.superDilithium.keyPair();

		const signed = await signTarget.superDilithium.sign(
			message,
			keyPair.privateKey
		);
		const verified = await verifyTarget.superDilithium.open(
			signed,
			keyPair.publicKey
		);

		const signature = await signTarget.superDilithium.signDetached(
			message,
			keyPair.privateKey
		);
		const isValid = await verifyTarget.superDilithium.verifyDetached(
			signature,
			message,
			keyPair.publicKey
		);

		expect(toHex(verified)).toBe(toHex(message));
		expect(isValid).toBe(true);
	});
}
