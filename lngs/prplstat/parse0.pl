#!/usr/bin/env perl

use warnings;
use strict;

while (<STDIN>) {
	if (/-+\s+(\w+\s+\w+).*Offline to.*\@\s+(\d+)\/(\d+)\/(\d+)\s+(\d+):(\d+):(\d+)\s+(.M)/) {
		print "(start ($1) ($4-$2-$3 $5-$6-$7 $8))\n";
	} elsif (/-+\s+(\w+).*Offline to.*\@\s+(\d+)\/(\d+)\/(\d+)\s+(\d+):(\d+):(\d+)\s+(.M)/) {
		print "(start ($1 one) ($4-$2-$3 $5-$6-$7 $8))\n";
	}
	if (/-+\s+(\w+\s+\w+).*to Offline.*\@\s+(\d+)\/(\d+)\/(\d+)\s+(\d+):(\d+):(\d+)\s+(.M)/) {
		print "(stop ($1) ($4-$2-$3 $5-$6-$7 $8))\n";
	} elsif (/-+\s+(\w+).*to Offline.*\@\s+(\d+)\/(\d+)\/(\d+)\s+(\d+):(\d+):(\d+)\s+(.M)/) {
		print "(stop ($1 one) ($4-$2-$3 $5-$6-$7 $8))\n";
	}
}
