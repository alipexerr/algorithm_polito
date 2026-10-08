# Word-frequency index

## Problem

A text-analysis program receives a list of words that must be monitored and a
second file containing a text. The program reports how many times every listed
word occurs in the text. Letter case does not matter: `Memory`, `memory`, and
`MEMORY` represent the same word. Words that are not in the index are simply
ignored.

The program must be completed in two versions. They read the same files and
produce the same report, but represent an indexed word differently:

- the array version stores the word directly in a character array inside its
  record;
- the pointer version stores the word through a pointer to an independently
  allocated string.

## Supplied files

```text
src/array_version/         program, documented header, and incomplete array implementation
src/pointer_version/       program, documented header, and incomplete pointer implementation
tests/array_version/       focused tests for the array representation
tests/pointer_version/     focused tests for the pointer representation
data/words.txt             representative word list
data/text.txt              representative text used by both prepared programs
data/mixed_case_text.txt   text emphasizing case-insensitive matching
data/empty_text.txt        empty text
```

Four prepared configurations are available: one program and one test suite for
each representation. Both program configurations initially use
`data/words.txt` and `data/text.txt`. Consult the Getting Started document to
run a prepared configuration or select different files.

## Input

The two executable programs receive the word-list path and the text path:

```text
word_index_array <word_list_file> <text_file>
word_index_pointer <word_list_file> <text_file>
```

The word-list file begins with the number of indexed words. The words follow,
one per line. For example, `data/words.txt` contains:

```text
5
array
code
data
memory
pointer
```

The text file contains whitespace-separated words on any number of lines. The
complete `data/text.txt` file is:

```text
Data moves through an array
memory stores data and code
ARRAY data MEMORY pointer
```

Every word is a single token containing from 1 to 30 alphabetic characters.
The indexed words are distinct when compared without regard to case. The text
may be empty and may contain words that are absent from the index.

## Required behavior

Both versions load the indexed words in their original order and report how
often each one occurs in the complete text. Matching is case-insensitive, and
words that do not belong to the index are ignored. The final report preserves
the order of the word-list file, and all dynamically allocated memory must be
released.

In the pointer version, every record owns its word string. Every string must be
released before the record array. The array version has no separate allocation
for an individual word.

The required record declarations and public operations are documented in the
two `word_index.h` files. Their signatures must not be changed because the
supplied programs and tests call those operations directly. Additional private
helpers may be added to the corresponding `word_index.c` file.

## Output

For the supplied representative files, both versions print:

```text
WORD OCCURRENCES
array 2
code 1
data 3
memory 2
pointer 1
```

## Invalid input and errors

An incorrect number of arguments, a file that cannot be opened, an unreadable
word list, or an allocation failure terminates the program with a non-zero
status and a concise message on standard error. All memory allocated before an
error must still be released. Exact diagnostic wording is not part of the
required output.

Apart from I/O and allocation failures, the provided input files may be assumed to
satisfy the documented format and constraints.

## Starter code

The two `main.c` files already handle arguments, open and close files through a
dedicated operation, and print the report. The public function bodies in each
`word_index.c` are intentionally incomplete and marked with TODO comments.

Complete the array version and pointer version independently. Do not combine
them with conditional compilation or replace the two required record
representations with a common generic type.

## Tests

Each representation has a focused automatic suite. The tests check loading and
initialization, case-insensitive lookup, repeated and unknown words, empty
text, and the ownership behavior specific to the pointer version. One test
checks that a complete load-and-destroy cycle releases every memory block it
acquired.

Run the tests incrementally while completing the corresponding implementation.
Consult the Getting Started document for the prepared test procedure.
