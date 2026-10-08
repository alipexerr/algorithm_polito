# Monitoring grid

## Problem

A rectangular monitoring grid uses `X` to mark an anomalous cell and `.` to
mark a normal cell. Adjacent anomalous cells may form a horizontal line, a
vertical line, or one of the two downward diagonals.

The program must load a grid, find every aligned anomalous sequence that reaches
a required minimum length, and report its endpoints, direction, and length.

Only maximal sequences count. A sequence is maximal when it cannot be extended
in the same direction at either end. If a line contains five consecutive `X`
cells and the minimum length is three, the length-five line is reported once;
its length-three and length-four subsegments are not separate results.

Different maximal sequences may intersect. The same anomalous cell may
therefore belong to horizontal, vertical, or diagonal sequences at the same
time.

## Supplied files

```text
src/main.c                  minimal program entry point
data/example.txt            representative grid containing all four directions
data/border_sequences.txt   sequences that reach the borders of the grid
data/intersections.txt      horizontal and vertical sequences that intersect
data/no_sequences.txt       grid without a sequence long enough to report
```

The prepared program configuration initially uses `data/example.txt`. Consult
the Getting Started document for the prepared build, run, and debugging
procedures, and to select another supplied file or pass a file of your own.

## Input

The program receives exactly one command-line argument: the path of the input
file.

```text
monitoring_grid <input_file>
```

The first line contains three integers:

```text
rows columns minimum_length
```

- `rows` is the number of rows in the grid;
- `columns` is the number of cells in each row;
- `minimum_length` is the minimum number of aligned anomalous cells required
  for a sequence to appear in the report.

Exactly `rows` grid lines follow. Each line contains exactly `columns`
characters and uses only `.` and `X`.

The complete `data/example.txt` file is:

```text
5 6 3
XXX...
..X.X.
..XXX.
..X.X.
..X..X
```

The input must satisfy all of these conditions:

- `rows` and `columns` are between 1 and 50, and at least one of them is 2 or
  greater;
- `minimum_length` is at least 2 and no greater than the larger grid dimension;
- exactly `rows` grid strings are present;
- each grid string has exactly `columns` characters;
- only `.` and `X` occur in the grid.

A sequence represents a segment of at least two adjacent anomalous cells. An
isolated `X` is an anomalous occurrence, but it is not a sequence.

## Directions and coordinates

Sequences may follow these four directions:

| Direction | Row change | Column change |
| --- | ---: | ---: |
| `HORIZONTAL` | 0 | +1 |
| `VERTICAL` | +1 | 0 |
| `DIAGONAL_DOWN_RIGHT` | +1 | +1 |
| `DIAGONAL_DOWN_LEFT` | +1 | -1 |

Coordinates in the report are one-based: the top-left cell is row 1, column 1.
The start is the first cell encountered when following the row and column
changes shown above; the end is the last cell of the same maximal sequence.

## Required behavior

Report every maximal aligned sequence of anomalous cells whose length is at
least `minimum_length`. Report each maximal sequence exactly once and never
report one of its proper subsegments as a separate result. Each result contains
the one-based coordinates of both endpoints, its direction, and its length.

The complete search must run in `O(rows * columns)` time. The internal program
decomposition and the method used to meet this bound are part of the solution
design. To meet this bound, avoid restarting the traversal of the same sequence
from each of its cells.

The final results must be ordered by:

1. start row in ascending order;
2. start column in ascending order;
3. direction in this fixed order: `HORIZONTAL`, `VERTICAL`,
   `DIAGONAL_DOWN_RIGHT`, `DIAGONAL_DOWN_LEFT`.

The report also summarizes the grid dimensions, the required minimum length,
and the number of sequences found.

## Output

The report contains a table of maximal sequences followed by a statistics
section. For `data/example.txt`, the complete output is:

```text
ANOMALY SEQUENCES
Start row | Start column | End row | End column | Direction | Length
1 | 1 | 1 | 3 | HORIZONTAL | 3
1 | 2 | 5 | 6 | DIAGONAL_DOWN_RIGHT | 5
1 | 3 | 5 | 3 | VERTICAL | 5
2 | 5 | 4 | 5 | VERTICAL | 3
2 | 5 | 4 | 3 | DIAGONAL_DOWN_LEFT | 3
3 | 3 | 3 | 5 | HORIZONTAL | 3

STATS
Grid size: 5 rows x 6 columns
Minimum sequence length: 3
Sequences found: 6
```

When no sequence reaches the minimum length, the table contains no data rows
and the final statistic reports zero:

```text
ANOMALY SEQUENCES
Start row | Start column | End row | End column | Direction | Length

STATS
Grid size: 3 rows x 3 columns
Minimum sequence length: 3
Sequences found: 0
```

## Invalid input and errors

If the number of command-line arguments is incorrect, print this diagnostic to
standard error and terminate with a non-zero status:

```text
Usage: monitoring_grid <input_file>
```

If the file cannot be opened, print:

```text
Cannot open input file.
```

If its contents are invalid, print:

```text
Invalid monitoring grid.
```

No partial report is printed after an error.
