const targets = [
	{name: 'local build', dilithium: require('.')},
	{name: 'stable release', dilithium: require('dilithium-crystals')}
];

const message = new Uint8Array([98, 97, 108, 108, 115, 0]);

const toHex = bytes => Buffer.from(bytes).toString('hex');

for (const {name, dilithium} of targets) {
	test(`${name} key pair generation`, async () => dilithium.keyPair());
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
		const keyPair = await keyPairTarget.dilithium.keyPair();

		const signed = await signTarget.dilithium.sign(
			message,
			keyPair.privateKey
		);
		const verified = await verifyTarget.dilithium.open(
			signed,
			keyPair.publicKey
		);

		const signature = await signTarget.dilithium.signDetached(
			message,
			keyPair.privateKey
		);
		const isValid = await verifyTarget.dilithium.verifyDetached(
			signature,
			message,
			keyPair.publicKey
		);

		expect(toHex(verified)).toBe(toHex(message));
		expect(isValid).toBe(true);
	});
}
