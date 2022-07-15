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
			'fast-sha512': require.resolve('../fast-sha512'),
			rsasign: require.resolve('../rsasign'),
			'sphincs-legacy': require.resolve('../sphincs-legacy'),
			vm: require.resolve('vm-browserify')
		}
	},
	target: ['web', 'es5']
};
