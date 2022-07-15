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
			'fast-sha512': require.resolve('../fast-sha512'),
			rsasign: require.resolve('../rsasign'),
			'sphincs-legacy': require.resolve('../sphincs-legacy'),
			vm: require.resolve('vm-browserify')
		}
	},
	target: ['web', 'es5']
};
