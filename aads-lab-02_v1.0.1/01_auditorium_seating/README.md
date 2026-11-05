# Auditorium group reservation

## Problem

An auditorium has an oval floor plan, so its physical rows contain different
numbers of seats. A booking operator must be able to reserve seats for several
groups without reloading the room after every request.

The program loads the current seating plan once and then accepts reservation
commands until the operator ends the session. For every request, it must keep
the complete group together in one row, update the occupied seats, list the
assigned places clearly, and display the new seating map.

## Supplied files

```text
src/main.c                command loop, command parsing, and coordination
src/seating_plan.h        documented public operations
src/seating_plan.c        incomplete operation bodies marked with TODO
tests/test_seating_plan.c focused tests for the public operations
tests/support/            supplied memory-checking support; do not modify
data/example.txt          representative reservation and row lengths
data/odd_padding.txt      even and odd centering cases
data/unavailable.txt      room with no block for a group of three
data/single_row.txt       smallest row-count case
```

The prepared Run configuration initially uses `data/example.txt`. Consult the
Getting Started document for the procedures used to build, run, debug, and
test the program and to select another input.

The supplied `main.c` loads the auditorium, repeatedly asks for a command, and
handles invalid commands. It also prints the command menu and the reservation
summary. Do not duplicate that coordination logic inside the operations you
complete.

The supplied `main.c` also prints and releases the temporary centered map
returned by the public operations. The map-construction algorithm remains part
of the work to complete; do not duplicate output formatting inside it.

Do not change the declarations in `src/seating_plan.h`: the supplied program
and tests use those exact operations. Private helpers may be added to
`src/seating_plan.c`.

## Input file

The executable receives only the auditorium file:

```text
auditorium_seating <input_file>
```

The file begins with the number of physical rows. Each following row contains
its letter, its number of seats, and one state for every seat:

```text
row_count
row_letter seat_count state_1 state_2 ... state_n
...
```

Rows are listed in physical order from the front of the auditorium to the back.
State `0` means available and state `1` means occupied.

The complete `data/example.txt` file is:

```text
4
A 5 0 1 0 1 0
B 8 1 0 0 0 1 0 0 1
C 9 0 0 1 0 0 0 1 0 0
D 6 1 0 1 0 0 1
```

## Commands

After loading the file, the supplied program displays the command menu once:

```text
Available commands:
  reserve <group_size> reserve consecutive seats for a group
  stop                 end the session
```

It then prints `Command?` before each input and waits for the next command.

`reserve <group_size>` attempts one reservation using the current seating
state. A successful request changes those seats to occupied, so every later
command sees the updated room. An unsuccessful request leaves the room
unchanged. `stop` releases the loaded data and ends the program.

An unknown command or an invalid group size prints a concise diagnostic and
then asks for another command without changing the room.

## Seat numbering

Seat `1` is always the physical seat furthest to the left in its row. Numbers
increase towards the right and restart from `1` in every row.

Blank space used to center a short row on screen is not part of the physical
row and never changes its numbering. The first real seat shown in a row is
therefore always seat `1`, regardless of its horizontal position in the map.

## Required reservation behavior

The complete group must occupy one uninterrupted block of available seats in
the same physical row. A group cannot be divided between rows and cannot
continue from the right end of one row to the left end of the next.

Inspect rows in their file order and choose the first row containing a block
large enough for the complete group.

If that row contains several suitable blocks, choose the block whose center is
closest to the center of the physical row. If two blocks are equally close,
choose the one with the lower starting seat number.

For example, with `data/example.txt`, a request for two seats cannot be
satisfied by row A. Row B contains three suitable blocks; seats 3 and 4 form
the block closest to the row center and become occupied.

## Centered map representation

All printed rows must have the width of the longest physical row. Shorter rows
are centered by placing the positions that do not correspond to real seats on
their two sides.

Use these quantities:

```text
total_padding = maximum_seat_count - row_seat_count
left_padding  = total_padding / 2
right_padding = total_padding - left_padding
```

Integer division is used. If the padding is odd, the additional position is
placed on the right.

For a longest row containing nine seats:

```text
5 real seats: 2 empty positions + 5 seats + 2 empty positions
8 real seats: 0 empty positions + 8 seats + 1 empty position
6 real seats: 1 empty position  + 6 seats + 2 empty positions
```

Use the symbolic states declared in `seating_plan.h`: `NO_SEAT` for an empty
map position, `SEAT_AVAILABLE` for an available physical seat, and
`SEAT_OCCUPIED` for an occupied physical seat. Map construction does not print
anything and does not change the physical seating plan.

## Output

After loading, the program prints the initial seating map and the command menu.
A successful request prints every assigned seat on the same line:

```text
RESERVATION
Requested seats: 2
Assigned row: B
Assigned seats: B3 B4
```

With `data/unavailable.txt`, a request for three seats prints:

```text
RESERVATION
Requested seats: 3
Result: not available
```

The updated seating map is printed after either result. For the first request
of two seats using `data/example.txt`, it is:

```text
SEATING MAP
Legend: .=available X=occupied
A |          1.  2X  3.  4X  5.         |
B |  1X  2.  3X  4X  5X  6.  7.  8X     |
C |  1.  2.  3X  4.  5.  6.  7X  8.  9. |
D |      1X  2.  3X  4.  5.  6X         |
```

Each displayed row has nine positions. In row A, the first two and last two
positions are blank; `1.` remains physical seat A1.

The supplied printing code shows `.` for an available seat, `X` for an
occupied seat, and blanks for `NO_SEAT`. Every position uses the same width:
seat numbers are right-aligned before their state symbol, while a position
without a physical seat occupies the equivalent blank space.

## Memory constraint

The program must not store the auditorium in a fixed matrix sized for the
maximum possible number of rows and seats. Such a matrix would reserve memory
for seats that do not exist.

Memory retained for the seating plan must instead be proportional to the row
count and to the number of real seats declared in the file. The equal-width map
is created separately for display and released when it is no longer needed.

## Constraints and assumptions

- the auditorium contains from 1 to 26 rows;
- row letters are uppercase letters from `A` to `Z`;
- every row contains from 1 to 99 seats;
- every seat state is `0` or `1`;
- a reservation requests from 1 to 99 seats.

## Invalid input and errors

A file that cannot be opened, a missing or out-of-range value, a repeated row
letter, or an allocation failure terminates the program with
a non-zero status and a concise diagnostic on standard error. Input files are
written by hand, so a repeated row letter is detected and reported rather than
assumed away. No command is
requested and no partial map is printed. Every acquired resource must still be
released.

Invalid interactive commands do not terminate the program. Exact diagnostic
wording is not part of the required output.

## Tests

The supplied tests cover loading, cleanup after a partial read, row boundaries,
block selection, center and lower-number tie-breaking, seat reservation, even
and odd padding, and preservation of seat numbering after centering. One test
checks that a complete load, map-building and destruction cycle releases every
memory block it acquired.

Run them incrementally while completing the corresponding operations.
