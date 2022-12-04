#!/bin/sh

# NOTE: this is crude, but good enough to ensure
# decent behavior.

PATH=$PWD/:$PATH
export PATH

if [ ! -x "$1" ]; then
	echo `basename $0` "<program> [tests/]" 1>&2
	exit 1
fi

d=tests; if [ -n "$2" ]; then d="$2"; fi

if ! which diff >/dev/null; then
	echo 'diff(1) not found in $PATH' 1>&2
	exit 1
fi

for x in $d/*; do
	set +e
	eval "$1 $(cat $x/args)" < $x/input > /tmp/test.$$ 2>/tmp/test.err.$$;
	y=$?
	set -e
	if [ "$y" == "0" ] && [ -e $x/output ]; then
		diff -u $x/output /tmp/test.$$
	elif [ "$y" != "0" ] && [ -e $x/err ]; then
		diff -u $x/err    /tmp/test.err.$$
	else
		echo "unexpected success/failure ($y)"
		exit 1
	fi
	rm -f /tmp/test.$$ /tmp/test.err.$$

	echo `basename $x`: OK
done