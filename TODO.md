# Multi-bytes quotes @multi-bytes-quotes
Could be useful, for instance to match C-style (`/* ... */`) comments.

It could be implemented by having `quotes` as provided through `-q`
to be a list of quotes separated by a special character, whose default
value would match IFS's, and which could be configurable through a new
`[-r qs]` option.

# Default option sets @default-opts-sets
Could be interesting to provide some default
set of options to cover common use cases. For
instance, indenting a list of markdown URLs such
as:

    [example]: https://example.com
    [google]:  https://www.google.com

Or a list of variable declaration/initialization, with
potential comments as last field such as:

	static char    *ifs    = "[ \t,;:]\\+"; /* input field separator regex(3) */
	static regex_t ifsre;
	static char    *ofs    = "␜";           /* output field separator         */
	static char    *quotes = "\"\"''[]()";  /* default quotes                 */
	static int     kq      = 0;             /* keep quotes in output          */
	static int     ki      = 0;             /* keep line indentation          */
	static uint    maxf    = 0;             /* maximum number of field if > 0 */
	static int     strict  = 0;             /* exits if OFS is found on input */

Perhaps the best way to provide them would be as
small scripts `lcol` (link columnize) and `vcol`
(variable columnize) instead of polluting qsplit.

# In quotes alignement @in-quotes-alignement
In the following case:

	static int     ki      = 0;             /* keep line indentation          */
	static uint    maxf    = 0;             /* maximum number of field if > 0 */

Closing comment indicator `*/` must be indented by hand. We
have no way of saying something like:

  - `/*` is a field on its own;
  - whatever follows until `*/` is also a field;
  - and so is the `*/` itself.

This could be implemented by having a second way to specify
quotes such as `[-p quotes]`, which would parse `quotes` as
`-p` (see @multi-bytes-quotes), but where the quotes would
be parsed so as to provide the previously described field
separation.

# Indentation as a field @indent-as-a-field @no-indent
Indentation is a weird feature for qsplit to have; it's there
because of one peculiar use case, so as to avoid user to have
to re-indent the code.

We could either remove indentation management and/or have
an option to consider it as a field.

Indentation management could be performed in a secondary
script; given the current use case, we could have a `iqcol`
that would manage indentation and call `qcol`.
