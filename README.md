# Introduction
[qsplit(1)][qsplit-1] is a (single-byte) quote-aware line splitting tool,
written in C (POSIX.1, e.g. [getopt(3)][getopt-3], [regex(3)][regex-3]),
typically useful as an [awk(1)][awk-1] front-end, thus allowing to
use [awk(1)][awk-1] on CSVs or log files (e.g. [httpd(8)][httpd-8]'s or
[sshd(8)][sshd-8]).

As an example, ``qcol(1)`` is a rough, [column(1)][column-1]-like tool,
indenting its input as a table, relying on [qsplit(1)][qsplit-1] to perform the
field separation and [awk(1)][awk-1] for the indentation.

    $ sed 3q /etc/passwd | qcol -x '  ' -i:
    root    x  0  0  root    /root  /bin/bash
    bin     x  1  1  bin     /bin   /usr/bin/nologin
    daemon  x  2  2  daemon  /      /usr/bin/nologin

    $ sed 3q /etc/passwd | column -s: -t
    root    x  0  0  root    /root  /bin/bash
    bin     x  1  1  bin     /bin   /usr/bin/nologin
    daemon  x  2  2  daemon  /      /usr/bin/nologin

Another common use case, is to create tables from pieces of code, e.g.

    $ cat << EOF | qcol -n -k -q '""' -i '[ 	]\+' -m 5
        static char  *ifs    = "[ \t,;:]\\+"; // input field separator regex(3)
        static regex_t ifsre;
        static char    *quotes = "\"\"''[]()";  // default quotes
        static int    kq    = 0;  // keep quotes in output
        static int   ki    = 0;  // keep line indentation
        static uint   maxf    = 0;  // maximum number of field if > 0
        static int   strict  = 0;       // exits if OFS is found on input
    EOF
        static char    *ifs    = "[ \t,;:]\\+"; // input field separator regex(3)
        static regex_t ifsre;
        static char    *quotes = "\"\"''[]()";  // default quotes
        static int     kq      = 0;             // keep quotes in output
        static int     ki      = 0;             // keep line indentation
        static uint    maxf    = 0;             // maximum number of field if > 0
        static int     strict  = 0;             // exits if OFS is found on input

``tests.sh`` run the tests found in the ``tests/`` directory.

    # The Makefile actually uses qcol(1) to format its output
    % make tests
    Running tests...
    01-opts-httpd-one-line:        OK
    02-keep-quotes-httpd-one-line: OK
    03-quoted-last-field:          OK
    04-var-decl:                   OK
    05-missing-quote:              OK
    06-default-empty:              OK
    07-var-decl-no-kq:             OK
    08-edge-cases:                 OK
    09-keep-indent:                OK
    10-no-keep-indent:             OK

# History
[qsplit(1)][qsplit-1] first originated as an improvement over a previous, poorly
written tool (input is limited by a [pipe(7)][pipe-7]'s capacity)
`ucol` (see [ucol.awk][gh-mb-utils-ucol-awk] and [ucol.c][gh-mb-utils-ucol-c]).

A previous implementation in [Go][golang] is available in
``old/go/qsplit.go``.

[golang]:               https://go.dev/
[awk-1]:                https://man.openbsd.org/awk.1
[column-1]:             https://man.openbsd.org/column.1
[getopt-3]:             https://man.openbsd.org/getopt.3
[regex-3]:              https://man.openbsd.org/regex.3
[httpd-8]:              https://man.openbsd.org/httpd.8
[sshd-8]:               https://man.openbsd.org/sshd.8
[getopt-3]:             https://man.openbsd.org/getopt.3
[regex-3]:              https://man.openbsd.org/regex.3
[pipe-7]:              https://man.openbsd.org/regex.7

[qsplit-1]:             https://github.com/mbivert/qsplit/blob/master/qsplit.1

[gh-mb-utils-ucol-awk]: https://github.com/mbivert/utils/blob/master/ucol.awk
[gh-mb-utils-ucol-c]:   https://github.com/mbivert/utils/blob/master/ucol.c

