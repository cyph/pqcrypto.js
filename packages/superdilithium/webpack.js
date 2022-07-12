module.exports = {
	entry: {
		app: './index.js'
	},
	mode: 'none',
	output: {
		filename: 'index.js',
		library: 'superDilithium',
		libraryTarget: 'var'
	},
	resolve: {
		fallback: {
			crypto: false,
			'dilithium-crystals': require.resolve('../dilithium-crystals'),
			path: false,
			stream: false,
			vm: require.resolve('vm-browserify')
		}
	},
	target: ['web', 'es5']
};
