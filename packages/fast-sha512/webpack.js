module.exports = {
	entry: {
		app: './index.js'
	},
	mode: 'none',
	output: {
		filename: 'index.js',
		library: 'fastSHA512',
		libraryTarget: 'var'
	},
	target: ['web', 'es5']
};
