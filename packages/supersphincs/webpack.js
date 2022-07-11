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
			rsasign: require.resolve('../rsasign'),
			sphincs: require.resolve('../sphincs'),
			vm: require.resolve('vm-browserify')
		}
	},
	target: ['web', 'es5']
};
