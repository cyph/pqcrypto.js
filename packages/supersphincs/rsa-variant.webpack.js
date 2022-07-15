module.exports = {
	entry: {
		app: './dist/rsa-variant.js'
	},
	mode: 'none',
	output: {
		filename: 'rsa-variant.bundle.js',
		library: 'superSphincs',
		libraryTarget: 'var'
	},
	resolve: {
		fallback: {
			'fast-sha512': require.resolve('../fast-sha512'),
			rsasign: require.resolve('../rsasign'),
			sphincs: require.resolve('../sphincs'),
			vm: require.resolve('vm-browserify')
		}
	},
	target: ['web', 'es5']
};
