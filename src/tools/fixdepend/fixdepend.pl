#!/usr/bin/perl -w
use strict;
use warnings;

my $argc = $#ARGV + 1;
if ($argc != 1) {
    print "Usage: $0 [FILE]\n";
    exit 1;
}

my $out = '';
my $file = $ARGV[0];
open(my $inputfh, $file) or die $!;

my $i = 0, my $j;
my $isEOL, my $isBOL, my $isRule, my $isFile;
my $linebuf;
my @chars;
while (my $line = <$inputfh>) {
    $i = 0;
    @chars = split('', $line);
    $linebuf = '';

    foreach my $c (@chars) {
        if ($c eq ' ') {
            # Case 1: Backwards, only spaces until newline. Do not escape.
            $j = $i;
            while ($chars[$j] eq ' ' && $j > 0) {
                $j--;
            }
            $isBOL = $j == 0 || $i == 0;

            # Case 2: Forwards, only spaces until newline. Do not escape.
            $j = $i;
            while ($chars[$j] eq ' ' && $j < ($#chars + 1)) {
                $j++;
            }
            $isEOL = $chars[$j] eq '\\'
                && (ord($chars[$j+1]) == 10 || ord($chars[$j+1]) == 13);

            # Case 3: Space preceded by single colon (:). Do not escape.
            $isRule = $i > 0 && $chars[$i-1] eq ':';

            # Case 4: Space preceded by filename (detected by checking if there
            # is one of our known fileendings (.h, .c, .cc, .hh, .m).
            $isFile = $i > 3 && hasFileending(substr($line, $i - 3, 3));

            if ($isBOL || $isEOL || $isRule || $isFile) {
                $linebuf .= ' ';
            } else {
                # Simple case (just replace. lol).
                $linebuf .= '\\ ';
            }
        } elsif ($c eq ':') {
            # Case 5: It's a rule for a source file, remove this line.
            if (isSourceFile($line)) {
                goto nextline;
            }
            $linebuf .= $c;
        } else {
            $linebuf .= $c;
        }

        $i++;
    }

    $out .= $linebuf;

nextline:;
}

sub hasFileending {
    my $str = $_[0];
    return $str =~ /\.(cc|mm|hh|c|m|h)(?!:)/g;
}

sub isSourceFile {
    my $str = $_[0];
    return $str =~ /\.(cc|mm|c|m):(?!\ \\)/g;
}

close($inputfh);
open(my $outputfh, ">$file") or die $!;
print $outputfh "$out";
close($outputfh);

