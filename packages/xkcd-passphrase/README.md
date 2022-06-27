# xkcd-passphrase

## Overview

A passphrase generator inspired by [xkcd #936](https://xkcd.com/936), designed
to provide a high level of security and memorability. By default, passwords are
generated with strength equivalent to a random 128-bit key.

Usage caveats:

* If end consumers of these generated passphrases are given the ability to easily
regenerate and cycle through them until they find ones they like, the level of
security provided drops _substantially_.

* While the architecture has been vetted by [Cure53](https://cure53.de), the code
itself has not yet been audited. Use at your own risk.

## Example Usage

	(async () => {
		console.log(await xkcdPassphrase.generate());
		/* collision pest numerous baboon tarnish aimee airgas swivel navigate */

		console.log(await xkcdPassphrase.generate(256));
		/*
		 * provolone email darkish symptom unending bridges bianca carport culminate vacancy
		 * rehydrate disjoin rotten cornball mousiness cephalon appear buddhism vanity
		 */

		console.log(await xkcdPassphrase.generate(512));
		/*
		 * minneapolis detonate headsman jacob lumber custodian glimmer silt lipton carded
		 * avalanche shady launder issueless freebee maude unedited spearhead nickname fleshed
		 * dissuade rudolph spouse lupe babolat severity chapman liquefy skunk humongous chatroom
		 * eatable kay cypress olson found emergency tree
		 */ 

		console.log(await xkcdPassphrase.generate(32, [
			'my',
			'awful',
			'custom',
			'word',
			'list',
			'that',
			'I',
			'created',
			'while',
			'drunk'
		]));
		/* that awful that custom list word list that custom custom */ 

		console.log(await xkcdPassphrase.generateWithWordCount(4));
		/* oscar jury email tugboat */
	})();

## Changelog

Breaking changes in major versions:

3.0.0:

* As part of upgrading from asm.js to WebAssembly (with asm.js included as a fallback),
the API is fully asynchronous.

2.0.0:

* Module bundling support.
