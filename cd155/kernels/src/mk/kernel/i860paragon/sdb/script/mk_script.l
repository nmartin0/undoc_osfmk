#include <stdio.h>

%%

.*"typedef"		passthru_code();
.+"Debug".*\n		;
.+"SCRIPT"		passthru_code();
.+"LABELPATCHES"	passthru_code();
.+"INSTRUCTIONS"	passthru_code();
.+"PATCHES"		passthru_code();
.			;

%%

passthru_code()
{
	int	c;

	printf(yytext);

	/* scan input stream until a ';' is found */
	for (;;) {
		if (!(c = input())) {
			sprintf(stderr, "Unexpected EOF\n");
			exit(-1);
		}
		putchar(c);
		if (c == ';') return;
	}
}
