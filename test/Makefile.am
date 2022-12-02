bin_PROGRAMS = vformat

vformat_SOURCE = testsuppt.c vformat.c 
vformat_LDADD = ../src/.libs/libvformat.a testsuppt.o 

EXTRA_DIST = testsuppt.c testsuppt.h tests/access/*.vcf tests/adhoc/*.vc* tests/spec/*.vcf tests/utf-8/*.vcf
TESTS = vformat