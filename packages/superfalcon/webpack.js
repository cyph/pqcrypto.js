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
			vm: require.resolve('vm-browserify')
		}
	}
};
