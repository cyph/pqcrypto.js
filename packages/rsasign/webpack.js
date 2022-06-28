module.exports = {
	entry: {
		app: './dist/index.module.js'
	},
	mode: 'none',
	output: {
		filename: 'index.js',
		library: 'rsaSign',
		libraryTarget: 'var'
	},
	resolve: {
		fallback: {
			vm: require.resolve('vm-browserify')
		}
	}
};
