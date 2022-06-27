#!/usr/bin/env node

require('./dist/xkcd-passphrase').generate(
	parseInt(process.argv[2], 10),
	process.argv[3] && require('fs').readFileSync(process.argv[3]).toString().trim().split('\n')
).then(s => console.log(s));
