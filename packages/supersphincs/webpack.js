module.exports = {
	entry: {
		app: './index.js'
	},
	mode: 'none',
	output: {
		filename: 'index.js',
		library: 'superSphincs',
		libraryTarget: 'var'
	},
	resolve: {
		fallback: {
			crypto: false,
			'fast-sha512': require.resolve('../fast-sha512'),
			path: false,
			sphincs: require.resolve('../sphincs'),
			stream: false,
			vm: require.resolve('vm-browserify')
		}
	},
	target: ['web', 'es5']
};
