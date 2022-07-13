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
			sphincs: require.resolve('../sphincs'),
			path: false,
			stream: false,
			vm: require.resolve('vm-browserify')
		}
	},
	target: ['web', 'es5']
};
