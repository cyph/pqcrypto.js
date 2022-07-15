module.exports = {
	entry: {
		app: './dist/rsa-variant.js'
	},
	mode: 'none',
	output: {
		filename: 'rsa-variant.bundle.js',
		library: 'superFalcon',
		libraryTarget: 'var'
	},
	resolve: {
		fallback: {
			'falcon-crypto': require.resolve('../falcon-crypto'),
			'fast-sha512': require.resolve('../fast-sha512'),
			rsasign: require.resolve('../rsasign'),
			vm: require.resolve('vm-browserify')
		}
	},
	target: ['web', 'es5']
};
