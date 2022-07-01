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
			crypto: require.resolve('crypto-browserify'),
			path: require.resolve('path-browserify'),
			stream: require.resolve('stream-browserify'),
			vm: require.resolve('vm-browserify')
		}
	}
};
