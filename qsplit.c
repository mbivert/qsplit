#define _DEFAULT_SOURCE /* getopt(3), optind(3), optarg(3) */
#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>

typedef unsigned int uint;

static char    *ifs    = "[ \t]\\+";    /* input field separator regex(3) */
static regex_t ifsre;
static char    *ofs    = "␜";           /* output field separator         */
static char    *quotes = "\"\"''";      /* default quotes                 */
static int     kq      = 0;             /* keep quotes in output          */
static int     ki      = 0;             /* keep line indentation          */
static uint    maxf    = 0;             /* maximum number of field if > 0 */
static int     strict  = 0;             /* exits if OFS is found on input */

void
help(char *argv0, int c)
{
	fprintf(stderr, "%s [-kns] [-m max] [-i ifs] [-o ofs] [-q quotes]\n", argv0);
	exit(c);
}

char
isquote(char c)
{
	size_t i;

	for (i = 0; i < strlen(quotes); i += 2)
		if (quotes[i] == c)
			return quotes[i+1];

	return '\0';
}

int
matchifs(char *p, regmatch_t *m)
{
	return !regexec(&ifsre, p, 1, m, 0);
}

char *
eatstring(char *p, char c)
{
	for (p = p+1; *p != '\0'; p++) {
		if (*p == '\\' && *(p+1) == c)
			p++;
		else if (*p == c)
			break;
	}

	return p;
}

void
flush(char *p, char *q)
{
	char c; c = *q; *q = '\0'; printf("%s", p); *q = c;
}

/*
 * This should be good enough for most purposes; maybe
 * we could have an option to insert an OFS to make two
 * fields out of "foo""bar".
 *
 * TODO assert() that p ends with at least with \n\0 somewhere.
 */
int
split(char *p)
{
	regmatch_t m;
	char *q, c;
	uint n;

	/* Indentation is part of the first field */
	if (ki) while (isspace(*p))
		putchar(*p++);

	for (n = 0;;) {
		if (maxf > 0 && n == maxf)
			break;

		if ((c = isquote(*p)) != '\0') {
			q = eatstring(p, c);

			/* unterminated quote */
			if (*q == '\0') {
				fprintf(stderr, "missing end quote: '%c'\n", c);
				return 1;
			}
			if (!kq) p++; else q++;
			flush(p, q);
			p = kq ? q : q+1;
		}

		else if (matchifs(p, &m)) {
			/* IFS doesn't immediately follows */
			if (m.rm_so > 0)
				flush(p, p+m.rm_so);

			/* s/IFS/OFS/ */
			n++;
			printf("%s", ofs);
			p += m.rm_eo;
		}

		else break;
	}

	/* Everything else is one field */
	printf("%s", p);
	return 0;
}

int
qsplit(char *fn, FILE *f)
{
	char b[BUFSIZ], *p;

	for (;;) {
		errno = 0;
		if ((p = fgets(b, sizeof(b), f)) == NULL) {
			/* feof(3) doesn't fail, nor set errno(3) */
			if (feof(f)) return 0;
			perror(fn);
			return 1;
		}

		if (strstr(b, ofs)) {
			fprintf(stderr, "OFS (%s) on input (%s)\n", ofs, b);
			if (strict)
				return 2;
		}

		if (split(b))
			/* error is already on stderr */
			return 42;
	}

	return 0;
}

int
main(int argc, char *argv[])
{
	char err[BUFSIZ];
	int i, x;
	FILE *f;

	while ((x = getopt(argc, argv, "knsm:i:o:q:h")) != -1) {
		switch(x) {
		case 'k':
			kq = 1;
			break;
		case 'n':
			ki = 1;
			break;
		case 's':
			strict = 1;
			break;
		case 'm':
			while (*optarg >= '0' && *optarg <= '9')
				maxf = maxf*10+(*optarg++ - '0');
			break;
		case 'i':
			ifs    = optarg;
			break;
		case 'o':
			ofs    = optarg;
			break;
		case 'q':
			quotes = optarg;
			break;
		case 'h':
			help(argv[0], 0);
			break;
		case '?':
			help(argv[0], 1);
			break;
		}
	}

	if ((x = regcomp(&ifsre, ifs, 0))) {
		regerror(x, &ifsre, err, sizeof(err));
		fprintf(stderr, "regcomp(3) failed '%s': %s\n", ifs, err);
		return 2;
	}

	if (strlen(quotes) % 2) {
		fprintf(stderr, "unpaired (uni-byte) quotes: '%s'\n", quotes);
		return 3;
	}

	x = 0;
	if (optind == argc) qsplit("", stdin);
	else for (i = optind; i < argc; i++) {
		if ((f = fopen(argv[i], "r")) == NULL) {
			perror(argv[i]);
			x = 4;
			break;
		}
		if ((x = qsplit(argv[i], f))) {
			fclose(f);
			break;
		}
		fclose(f);
	}

	regfree(&ifsre);

	return x;
}
