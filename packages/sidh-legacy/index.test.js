const targets = [
	{name: 'local build', sidh: require('.')},
	{name: 'stable release', sidh: require('sidh-legacy')}
];

const plaintext = new Uint8Array([98, 97, 108, 108, 115, 0]);

const toHex = bytes => Buffer.from(bytes).toString('hex');

for (const {name, sidh} of targets) {
	test(`${name} key pair generation`, async () => sidh.keyPair());
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
		const keyPair = await keyPairTarget.sidh.keyPair();

		const encrypted = await encryptTarget.sidh.encrypt(
			plaintext,
			keyPair.publicKey
		);

		const decrypted = await decryptTarget.sidh.decrypt(
			encrypted,
			keyPair.privateKey
		);

		expect(toHex(decrypted)).toBe(toHex(plaintext));
	});
}
