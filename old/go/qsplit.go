package main

/*
 * Quote split: refined line splitter.
 *
 * Typically used as an awk(1) front-end to parse log files.
 *
 * E.g. zcat -f access.log* | qsplit | awk -F"\x1C" { ... }
 */

import (
	"log"
	"os"
	"bufio"
	"fmt"
	"strings"
	"flag"
	"regexp"
)

var ifs *regexp.Regexp
var ofs  string
var fns  []string
var kq   bool

// opening/closing quotes
var quotes map[rune]rune

func init() {
	ifsf := flag.String( "ifs",    "[ \t,;:]+",  "Input Field Separator")
	flag.StringVar(&ofs, "ofs",    "␜",          "Output Field Separator")
	qfs  := flag.String( "quotes", "\"\"''[]()", "Quotes")
	flag.BoolVar(&kq,    "kq",     true,         "Keep quotes on quoted fields")

	flag.Parse()

	fns = flag.Args()
	if len(fns) == 0 {
		fns = append(fns, "/dev/stdin")
	}

	// IFS will always be matched at potential start
	// of a field.
	ifs = regexp.MustCompile("^"+*ifsf)

	if len(*qfs) % 2 != 0 {
		log.Fatal("Odd number of quotes ("+*qfs+")")
	}

	var x = []rune(*qfs)
	quotes = make(map[rune]rune)
	for i := 0; i < len(x)-1; i += 2 {
		quotes[x[i]] = x[i+1]
	}
}

// This is C-ish, maybe there's a more Go-ish way
func qsplit(s string, kq bool) (xs []string) {
	var t []rune
	var q rune

	j := -1

	for i, r := range s {
		// Currently on a field separator, skipping
		if j > i {
			continue
		}

		// Quote when not quoted: start quoting
		if q == 0 && quotes[r] != 0 {
			q = r
			if (kq) {
				t = append(t, r)
			}

		// Not quoted
		} else if q == 0 {
			// Is this the beginning of a field separator?
			p := ifs.FindStringIndex(s[i:])

			// No: add to current field
			if p == nil {
				t = append(t, r)

			} else {
				// If we were on a field, start a new one
				if len(t) > 0 {
					xs = append(xs, string(t))
					t = nil
				}
				// Skip the field separator
				j = i+p[1]
			}

		// Matching quote when quoted: end of quote
		} else if q != 0 && r == quotes[q] {
			if (kq) {
				t = append(t, r)
			}
			xs = append(xs, string(t))
			t = nil
			q = 0

		// Quoting
		} else if q != 0 {
			t = append(t, r)
		}
	}

	return append(xs, string(t))
}

func main() {
	for _, fn := range fns {
		f, err := os.Open(fn)
		if err != nil {
			log.Fatal(fn, ": ", err);
		}
		defer f.Close()
		s := bufio.NewScanner(f)
		for s.Scan() {
			fmt.Println(strings.Join(qsplit(s.Text(), kq), ofs));
		}
		if s.Err() != nil {
			log.Fatal(fn, ": ", err);
		}
	}
}
