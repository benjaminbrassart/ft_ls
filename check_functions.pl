#!/usr/bin/env perl

use 5.14.0;

use strict;
use warnings;

my %functions = map { $_ => 0} qw(write opendir readdir closedir stat lstat getpwuid getgrgid listxattr getxattr time ctime readlink malloc free perror strerror exit);
my $nm_output = `nm -u ft_ls`;

exit $? >> 8 unless $? == 0;

my @lines = split /^/, $nm_output;
my $status = 0;

foreach my $line (@lines) {
    chomp $line;

    next unless $line =~ s/\s+U (?!_)(\w+).*/$1/;

    next if exists $functions{$line};

    print "$line\n";
    $status = 1;
}

exit $status;
