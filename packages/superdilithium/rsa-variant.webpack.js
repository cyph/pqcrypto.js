module.exports = {
	entry: {
		app: './dist/rsa-variant.js'
	},
	mode: 'none',
	output: {
		filename: 'rsa-variant.bundle.js',
		library: 'superDilithium',
		libraryTarget: 'var'
	},
	resolve: {
		fallback: {
			'dilithium-crystals': require.resolve('../dilithium-crystals'),
			'fast-sha512': require.resolve('../fast-sha512'),
			rsasign: require.resolve('../rsasign'),
			vm: require.resolve('vm-browserify')
		}
	},
	target: ['web', 'es5']
};
