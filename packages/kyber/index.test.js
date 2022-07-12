const targets = [
	{name: 'local build', kyber: require('.')},
	{name: 'stable release', kyber: require('kyber')}
];

const plaintext = new Uint8Array([98, 97, 108, 108, 115, 0]);

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

		const encrypted = await encryptTarget.kyber.encrypt(
			plaintext,
			keyPair.publicKey
		);

		const decrypted = await decryptTarget.kyber.decrypt(
			encrypted,
			keyPair.privateKey
		);

		expect(toHex(decrypted)).toBe(toHex(plaintext));
	});
}
