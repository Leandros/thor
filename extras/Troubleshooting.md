# Troubleshooting


### Perl can't find cpan

You can go into the CPAN shell via this command:

    perl -MCPAN -e shell

The first time executing it will configure it, the second time go into shell.


### Perl can't find libraries

After installation Strawberry Perl can't find any libraries, a message similar
to the following is emitted:

    Can't locate Archive/Extract.pm in @INC (you may need to install the Archive::Extract module) (@INC contains: /usr/lib/perl5/site_perl /usr/share/perl5/site_perl /usr/lib/perl5/vendor_perl /usr/share/perl5/vendor_perl /usr/lib/perl5/core_perl /usr/share/perl5/core_perl .) at download.pl line 10.
    BEGIN failed--compilation aborted at download.pl line 10.

Go to 'Control Panel > System and Security > System > Advanced System Settings'
and add these path

- `<path to strawberry perl>/perl/lib`
- `<path to strawberry perl>/perl/site/lib`
- `<path to strawberry perl>/perl/vendor/lib`

to `PERL5LIB`.
