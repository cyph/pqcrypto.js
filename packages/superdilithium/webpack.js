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
			falcon: require.resolve('../falcon'),
			path: false,
			stream: false,
			vm: require.resolve('vm-browserify')
		}
	},
	target: ['web', 'es5']
};
