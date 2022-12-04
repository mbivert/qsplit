.PHONY: all
all: qsplit qsplit.1 qcol
# e.g. make install group=wheel
group ?= root

CCOPTS := -O3 -ansi -pedantic -Wall

clean:
	@echo Removing compiled binaries...
	@rm qsplit

qsplit: qsplit.c
	@echo Compiling $?
	@cc ${CCOPTS} -o $@ $?

.PHONY: tests
tests: qsplit tests.sh
	@echo Running tests...
	@sh tests.sh qsplit | ./qcol -i ' '

.PHONY: install
install: qsplit qsplit.1 qcol
	@echo Installing qsplit, qsplit.1, qcol...
	@install -o root -g ${group} -m 755 qsplit   /bin/qsplit
	@install -o root -g ${group} -m 644 qsplit.1 /usr/share/man/man1/qsplit.1
	@install -o root -g ${group} -m 755 qcol     /bin/qcol

.PHONY: uninstall
uninstall:
	@echo Removing qsplit, qsplit.1, qcol...
	@rm -f /bin/qsplit
	@rm -f /usr/share/man/man1/qsplit.1
	@rm -f /bin/qcol


