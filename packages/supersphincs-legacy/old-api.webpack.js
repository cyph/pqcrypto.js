module.exports = {
	entry: {
		app: './dist/old-api.js'
	},
	mode: 'none',
	output: {
		filename: 'old-api.bundle.js',
		library: 'superSphincs',
		libraryTarget: 'var'
	},
	resolve: {
		fallback: {
			rsasign: require.resolve('../rsasign'),
			'sphincs-legacy': require.resolve('../sphincs-legacy'),
			vm: require.resolve('vm-browserify')
		}
	},
	target: ['web', 'es5']
};
