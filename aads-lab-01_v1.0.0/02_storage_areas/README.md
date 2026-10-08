# Warehouse storage areas

## Problem

A warehouse floor is divided into rectangular storage areas. Each area is
identified by a short code and is defined on the floor plan by two opposite
corners.

The measurements were collected while walking through the warehouse. The file
therefore follows acquisition order rather than grouping measurements by area:
the two corners belonging to the same code may be separated by measurements of
several other areas. The file also does not state how many distinct areas it
contains.

The program must reconstruct every area and produce two reports for the
warehouse planner: one ordered by usable surface and one ordered by perimeter.

## Supplied files

```text
src/main.c                minimal program entry point
data/example.txt          five areas, with interleaved corner measurements
data/tied_rankings.txt    measurements producing an ordering tie
data/many_areas.txt       twenty areas requiring repeated array growth
```

The prepared configuration initially reads `data/example.txt` and writes
`areas_by_surface.txt` and `areas_by_perimeter.txt` in the project directory.
Consult the Getting Started document to run the configuration or change its
arguments.

## Input

The program receives three command-line arguments:

```text
storage_areas <input_file> <surface_output_file> <perimeter_output_file>
```

The input file contains measurements until end of file. Every row has this
form:

```text
area_code x y
```

`area_code` identifies the storage area, while `x` and `y` are the coordinates
of one measured corner. Each code occurs exactly twice; the two coordinate
pairs are opposite corners of the corresponding rectangle.

The complete `data/example.txt` file is:

```text
A12 1.0 1.0
B07 0.0 0.0
C03 -1.0 2.0
D15 2.0 -1.0
E20 -2.0 -2.0
A12 4.0 3.0
D15 7.0 2.0
B07 2.0 5.0
E20 4.0 1.0
C03 3.0 6.0
```

For example, area `A12` has opposite corners `(1.0, 1.0)` and `(4.0, 3.0)`.
Its width is `3.0`, its height is `2.0`, its surface is `6.0`, and its
perimeter is `10.0`.

## Required behavior

Read the measurement file once and store one record for each distinct area.
When a code is encountered for the first time, retain its first corner. When
the same code is encountered again, complete the area using its second corner.

The number of distinct areas is unknown before reading. The dynamically
allocated record array must therefore be enlarged when its available space is
exhausted, without losing areas reconstructed earlier.

For corners `(x1, y1)` and `(x2, y2)`, calculate:

```text
width     = absolute value of (x2 - x1)
height    = absolute value of (y2 - y1)
surface   = width * height
perimeter = 2 * (width + height)
```

The program must then write all areas to both requested output files.

## Output

The surface report begins with `CODE SURFACE` and orders areas by decreasing
surface. The perimeter report begins with `CODE PERIMETER` and orders areas by
decreasing perimeter. When two values are equal, order the corresponding codes
lexicographically. Numeric values use two decimal places.

For `data/example.txt`, the surface report is:

```text
CODE SURFACE
E20 18.00
C03 16.00
D15 15.00
B07 10.00
A12 6.00
```

The perimeter report is:

```text
CODE PERIMETER
E20 18.00
C03 16.00
D15 16.00
B07 14.00
A12 10.00
```

## Constraints and assumptions

- the file contains at least one complete area;
- a code contains from 1 to 7 non-whitespace characters;
- each code occurs exactly twice;
- coordinates are real values;
- the two corners of an area define positive width and height.

## Invalid input and errors

An incorrect number of arguments, a file that cannot be opened, a failed
essential read, or an allocation failure terminates the program with a
non-zero status and a concise diagnostic on standard error. Allocated memory
and open files must be released before termination. Exact diagnostic wording is
not part of the output contract.

## Starter code

Only a minimal `main.c` is supplied. Choose the record representation,
functions, modules, and internal organization of the program. Source files may
be added or organized in subdirectories anywhere below `src/` without changing
`CMakeLists.txt`.
