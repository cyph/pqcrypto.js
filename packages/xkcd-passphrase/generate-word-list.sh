#!/bin/bash


{
	echo 'abcdefghijklmnopqrstuvwxyz0123456789' | perl -pe 's/(.)/\1\n/g';
	curl -s https://www.eff.org/files/2016/07/18/eff_large_wordlist.txt | awk '{print $2}';
	curl -s https://www.eff.org/files/2016/09/08/eff_short_wordlist_1.txt | awk '{print $2}';
	curl -s https://www.eff.org/files/2016/09/08/eff_short_wordlist_2_0.txt | awk '{print $2}';
	curl -s https://deron.meranda.us/data/census-derived-all-first.txt | head -n1500 | awk '{print $1}';
	curl -s https://www.state.gov/misc/list/ | grep -P '^\s*<a href="http://www\.state\.gov/p' | perl -pe 's/\s*<//g' | grep -oP '>[A-Za-z- ]+' | sed 's|>||g' | sort | uniq | grep -v ' ';
	node -e '(async () => console.log(require("xlsx").read(await require("node-fetch")("https://www2.census.gov/topics/genealogy/2000surnames/Top1000.xls").then(o => o.buffer())).Strings.slice(11, -1).map(o => o.t && o.t.toLowerCase().trim()).filter(s => !s.match(/[^a-z-]/)).slice(0, 600).join("\n")))()';
	{
		curl -s https://en.wikipedia.org/wiki/Wikipedia:Multiyear_ranking_of_most_viewed_pages;
		curl -s https://simple.wikipedia.org/wiki/List_of_United_States_cities_by_population;
		curl -s https://en.wikipedia.org/wiki/List_of_companies_named_after_people;
		curl -s https://en.wikipedia.org/wiki/List_of_S%26P_500_companies;
	} |
		sed 's|, Incorporated||g' |
		sed 's| Incorporated||g' |
		sed 's|, Inc\.||g' |
		sed 's| Inc\.||g' |
		sed 's|, Inc||g' |
		sed 's| Inc||g' |
		sed 's| Corporation||g' |
		sed 's| Company||g' |
		sed 's| Corp||g' |
		sed 's| Co\.||g' |
		sed 's| College||g' |
		sed 's| University||g' |
		sed 's|"The |"|g' |
		sed 's|"A |"|g' |
		grep -oP 'title="[A-Za-z0-9-]+"' |
		perl -pe 's/title="(.*?)"/\1/g' \
	;
	cat misc.txt;
} |
	awk '{print $1}' |
	tr '[:upper:]' '[:lower:]' |
	grep -P . |
	grep -vP '([^a-z0-9-]|^-$)' |
	sort |
	uniq \
> word-list.txt
