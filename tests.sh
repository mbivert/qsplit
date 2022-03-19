#!/bin/sh

# NOTE: this is crude, but good enough to ensure
# decent behavior.

PATH=$PWD/:$PATH
export PATH

if [ ! -x ./qsplit ]; then
	echo './qsplit not found' 1>&2
	exit 1
fi

if ! which diff >/dev/null; then
	echo 'diff(1) not found in $PATH' 1>&2
	exit 1
fi

for x in tests/*; do
	set +e
	eval "./qsplit $(cat $x/args) $x/input" > /tmp/test.$$ 2>/tmp/test.err.$$;
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