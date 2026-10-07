#include <stdio.h>

%%

.+"Ent_"		passthru_define();
.			;

%%

passthru_define()
{
	int	c;

	printf(yytext);

	/* scan input stream until a '\n' is found */
	for (;;) {
		if (!(c = input())) {
			sprintf(stderr, "Unexpected EOF\n");
			exit(-1);
		}
		putchar(c);
		if (c == '\n') return;
	}
}
