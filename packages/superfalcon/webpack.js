module.exports = {
	entry: {
		app: './index.js'
	},
	mode: 'none',
	output: {
		filename: 'index.js',
		library: 'superFalcon',
		libraryTarget: 'var'
	},
	resolve: {
		fallback: {
			crypto: false,
			'falcon-crypto': require.resolve('../falcon-crypto'),
			'fast-sha512': require.resolve('../fast-sha512'),
			path: false,
			stream: false,
			vm: require.resolve('vm-browserify')
		}
	},
	target: ['web', 'es5']
};
