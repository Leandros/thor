#!/usr/bin/perl -w

use strict;
use warnings;

use File::Find;
use File::Basename;
use Parse::RecDescent;
use Getopt::Long;
Getopt::Long::Configure(qw{no_auto_abbrev no_ignore_case_always});

############
## Config ##
############
# No config, yet.

my $version_flag = 0;
my $help_flag = 0;
my $outfile = 'exports.def';
my $amend_flag = 0;

GetOptions(
    "version" => \$version_flag,
    "help|h" => \$help_flag,
    "out=s" => \$outfile,
    "amend" => \$amend_flag);

if ($version_flag) {
    print_version();
    exit 0;
}
if ($help_flag) {
    print_help();
    exit 0;
}

my $argc = $#ARGV + 1;
if ($argc < 2) {
    print "Error: To few arguments\n";
    exit 1;
}

# Create output file.
my $mode = $amend_flag ? '>>' : '>';
open(my $outfp, $mode, $outfile) or die "Error: Could not open $outfile";
if (!$amend_flag) {
    print $outfp "LIBRARY $ARGV[0]\n";
    print $outfp "EXPORTS\n";
}


# Main Loop (skip first element).
shift @ARGV;
foreach (@ARGV) {
    if (! -e $_) {
        print "Skipping $_: File does not exist\n";
        next;
    }
    if (-d $_) {
        process_dir($_);
    } elsif (-s $_) {
        process_file($_);
    }
}

close $outfp;

##############################################################################
## SUBROUTINES                                                              ##
##############################################################################
sub process_dir {
    my @files;
    my $dir = $_[0];
    find(sub { push @files, $File::Find::name unless -d; }, $dir);

    foreach (@files) {
        process_file($_);
    }
}

sub process_file {
    my $file = $_[0];
    chomp $file;
    my $valid = $file =~ m/^.+\.(h)$/;
    if ($valid) {
        print "$file\n";
    }
}

sub print_help {
    print "Usage: $0 [OPTIONS] DLLNAME FILES...\n";
    print "\n";
    print "    --version: print version and exit\n";
    print "    --help/-h: print help and exit\n";
    print "    --out: specify output file\n";
    print "    --amend: amend the output, instead of replacing\n";
}

sub print_version {
    print "makedef version 0.1\n";
    print "Copyright (c) 2016. Arvid Gerstmann.\n";
}

# ANSI C grammar taken from:
# https://www.lysator.liu.se/c/ANSI-C-grammar-y.html
# __GRAMMAR__
# %token IDENTIFIER CONSTANT STRING_LITERAL SIZEOF
# %token PTR_OP INC_OP DEC_OP LEFT_OP RIGHT_OP LE_OP GE_OP EQ_OP NE_OP
# %token AND_OP OR_OP MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN ADD_ASSIGN
# %token SUB_ASSIGN LEFT_ASSIGN RIGHT_ASSIGN AND_ASSIGN
# %token XOR_ASSIGN OR_ASSIGN TYPE_NAME

# %token TYPEDEF EXTERN STATIC AUTO REGISTER
# %token CHAR SHORT INT LONG SIGNED UNSIGNED FLOAT DOUBLE CONST VOLATILE VOID
# %token STRUCT UNION ENUM ELLIPSIS

# %token CASE DEFAULT IF ELSE SWITCH WHILE DO FOR GOTO CONTINUE BREAK RETURN

# %start translation_unit
# %%

# primary_expression
# 	: IDENTIFIER
# 	| CONSTANT
# 	| STRING_LITERAL
# 	| '(' expression ')'
# 	;

# postfix_expression
# 	: primary_expression
# 	| postfix_expression '[' expression ']'
# 	| postfix_expression '(' ')'
# 	| postfix_expression '(' argument_expression_list ')'
# 	| postfix_expression '.' IDENTIFIER
# 	| postfix_expression PTR_OP IDENTIFIER
# 	| postfix_expression INC_OP
# 	| postfix_expression DEC_OP
# 	;

# argument_expression_list
# 	: assignment_expression
# 	| argument_expression_list ',' assignment_expression
# 	;

# unary_expression
# 	: postfix_expression
# 	| INC_OP unary_expression
# 	| DEC_OP unary_expression
# 	| unary_operator cast_expression
# 	| SIZEOF unary_expression
# 	| SIZEOF '(' type_name ')'
# 	;

# unary_operator
# 	: '&'
# 	| '*'
# 	| '+'
# 	| '-'
# 	| '~'
# 	| '!'
# 	;

# cast_expression
# 	: unary_expression
# 	| '(' type_name ')' cast_expression
# 	;

# multiplicative_expression
# 	: cast_expression
# 	| multiplicative_expression '*' cast_expression
# 	| multiplicative_expression '/' cast_expression
# 	| multiplicative_expression '%' cast_expression
# 	;

# additive_expression
# 	: multiplicative_expression
# 	| additive_expression '+' multiplicative_expression
# 	| additive_expression '-' multiplicative_expression
# 	;

# shift_expression
# 	: additive_expression
# 	| shift_expression LEFT_OP additive_expression
# 	| shift_expression RIGHT_OP additive_expression
# 	;

# relational_expression
# 	: shift_expression
# 	| relational_expression '<' shift_expression
# 	| relational_expression '>' shift_expression
# 	| relational_expression LE_OP shift_expression
# 	| relational_expression GE_OP shift_expression
# 	;

# equality_expression
# 	: relational_expression
# 	| equality_expression EQ_OP relational_expression
# 	| equality_expression NE_OP relational_expression
# 	;

# and_expression
# 	: equality_expression
# 	| and_expression '&' equality_expression
# 	;

# exclusive_or_expression
# 	: and_expression
# 	| exclusive_or_expression '^' and_expression
# 	;

# inclusive_or_expression
# 	: exclusive_or_expression
# 	| inclusive_or_expression '|' exclusive_or_expression
# 	;

# logical_and_expression
# 	: inclusive_or_expression
# 	| logical_and_expression AND_OP inclusive_or_expression
# 	;

# logical_or_expression
# 	: logical_and_expression
# 	| logical_or_expression OR_OP logical_and_expression
# 	;

# conditional_expression
# 	: logical_or_expression
# 	| logical_or_expression '?' expression ':' conditional_expression
# 	;

# assignment_expression
# 	: conditional_expression
# 	| unary_expression assignment_operator assignment_expression
# 	;

# assignment_operator
# 	: '='
# 	| MUL_ASSIGN
# 	| DIV_ASSIGN
# 	| MOD_ASSIGN
# 	| ADD_ASSIGN
# 	| SUB_ASSIGN
# 	| LEFT_ASSIGN
# 	| RIGHT_ASSIGN
# 	| AND_ASSIGN
# 	| XOR_ASSIGN
# 	| OR_ASSIGN
# 	;

# expression
# 	: assignment_expression
# 	| expression ',' assignment_expression
# 	;

# constant_expression
# 	: conditional_expression
# 	;

# declaration
# 	: declaration_specifiers ';'
# 	| declaration_specifiers init_declarator_list ';'
# 	;

# declaration_specifiers
# 	: storage_class_specifier
# 	| storage_class_specifier declaration_specifiers
# 	| type_specifier
# 	| type_specifier declaration_specifiers
# 	| type_qualifier
# 	| type_qualifier declaration_specifiers
# 	;

# init_declarator_list
# 	: init_declarator
# 	| init_declarator_list ',' init_declarator
# 	;

# init_declarator
# 	: declarator
# 	| declarator '=' initializer
# 	;

# storage_class_specifier
# 	: TYPEDEF
# 	| EXTERN
# 	| STATIC
# 	| AUTO
# 	| REGISTER
# 	;

# type_specifier
# 	: VOID
# 	| CHAR
# 	| SHORT
# 	| INT
# 	| LONG
# 	| FLOAT
# 	| DOUBLE
# 	| SIGNED
# 	| UNSIGNED
# 	| struct_or_union_specifier
# 	| enum_specifier
# 	| TYPE_NAME
# 	;

# struct_or_union_specifier
# 	: struct_or_union IDENTIFIER '{' struct_declaration_list '}'
# 	| struct_or_union '{' struct_declaration_list '}'
# 	| struct_or_union IDENTIFIER
# 	;

# struct_or_union
# 	: STRUCT
# 	| UNION
# 	;

# struct_declaration_list
# 	: struct_declaration
# 	| struct_declaration_list struct_declaration
# 	;

# struct_declaration
# 	: specifier_qualifier_list struct_declarator_list ';'
# 	;

# specifier_qualifier_list
# 	: type_specifier specifier_qualifier_list
# 	| type_specifier
# 	| type_qualifier specifier_qualifier_list
# 	| type_qualifier
# 	;

# struct_declarator_list
# 	: struct_declarator
# 	| struct_declarator_list ',' struct_declarator
# 	;

# struct_declarator
# 	: declarator
# 	| ':' constant_expression
# 	| declarator ':' constant_expression
# 	;

# enum_specifier
# 	: ENUM '{' enumerator_list '}'
# 	| ENUM IDENTIFIER '{' enumerator_list '}'
# 	| ENUM IDENTIFIER
# 	;

# enumerator_list
# 	: enumerator
# 	| enumerator_list ',' enumerator
# 	;

# enumerator
# 	: IDENTIFIER
# 	| IDENTIFIER '=' constant_expression
# 	;

# type_qualifier
# 	: CONST
# 	| VOLATILE
# 	;

# declarator
# 	: pointer direct_declarator
# 	| direct_declarator
# 	;

# direct_declarator
# 	: IDENTIFIER
# 	| '(' declarator ')'
# 	| direct_declarator '[' constant_expression ']'
# 	| direct_declarator '[' ']'
# 	| direct_declarator '(' parameter_type_list ')'
# 	| direct_declarator '(' identifier_list ')'
# 	| direct_declarator '(' ')'
# 	;

# pointer
# 	: '*'
# 	| '*' type_qualifier_list
# 	| '*' pointer
# 	| '*' type_qualifier_list pointer
# 	;

# type_qualifier_list
# 	: type_qualifier
# 	| type_qualifier_list type_qualifier
# 	;


# parameter_type_list
# 	: parameter_list
# 	| parameter_list ',' ELLIPSIS
# 	;

# parameter_list
# 	: parameter_declaration
# 	| parameter_list ',' parameter_declaration
# 	;

# parameter_declaration
# 	: declaration_specifiers declarator
# 	| declaration_specifiers abstract_declarator
# 	| declaration_specifiers
# 	;

# identifier_list
# 	: IDENTIFIER
# 	| identifier_list ',' IDENTIFIER
# 	;

# type_name
# 	: specifier_qualifier_list
# 	| specifier_qualifier_list abstract_declarator
# 	;

# abstract_declarator
# 	: pointer
# 	| direct_abstract_declarator
# 	| pointer direct_abstract_declarator
# 	;

# direct_abstract_declarator
# 	: '(' abstract_declarator ')'
# 	| '[' ']'
# 	| '[' constant_expression ']'
# 	| direct_abstract_declarator '[' ']'
# 	| direct_abstract_declarator '[' constant_expression ']'
# 	| '(' ')'
# 	| '(' parameter_type_list ')'
# 	| direct_abstract_declarator '(' ')'
# 	| direct_abstract_declarator '(' parameter_type_list ')'
# 	;

# initializer
# 	: assignment_expression
# 	| '{' initializer_list '}'
# 	| '{' initializer_list ',' '}'
# 	;

# initializer_list
# 	: initializer
# 	| initializer_list ',' initializer
# 	;

# statement
# 	: labeled_statement
# 	| compound_statement
# 	| expression_statement
# 	| selection_statement
# 	| iteration_statement
# 	| jump_statement
# 	;

# labeled_statement
# 	: IDENTIFIER ':' statement
# 	| CASE constant_expression ':' statement
# 	| DEFAULT ':' statement
# 	;

# compound_statement
# 	: '{' '}'
# 	| '{' statement_list '}'
# 	| '{' declaration_list '}'
# 	| '{' declaration_list statement_list '}'
# 	;

# declaration_list
# 	: declaration
# 	| declaration_list declaration
# 	;

# statement_list
# 	: statement
# 	| statement_list statement
# 	;

# expression_statement
# 	: ';'
# 	| expression ';'
# 	;

# selection_statement
# 	: IF '(' expression ')' statement
# 	| IF '(' expression ')' statement ELSE statement
# 	| SWITCH '(' expression ')' statement
# 	;

# iteration_statement
# 	: WHILE '(' expression ')' statement
# 	| DO statement WHILE '(' expression ')' ';'
# 	| FOR '(' expression_statement expression_statement ')' statement
# 	| FOR '(' expression_statement expression_statement expression ')' statement
# 	;

# jump_statement
# 	: GOTO IDENTIFIER ';'
# 	| CONTINUE ';'
# 	| BREAK ';'
# 	| RETURN ';'
# 	| RETURN expression ';'
# 	;

# translation_unit
# 	: external_declaration
# 	| translation_unit external_declaration
# 	;

# external_declaration
# 	: function_definition
# 	| declaration
# 	;

# function_definition
# 	: declaration_specifiers declarator declaration_list compound_statement
# 	| declaration_specifiers declarator compound_statement
# 	| declarator declaration_list compound_statement
# 	| declarator compound_statement
# 	;

# %%
