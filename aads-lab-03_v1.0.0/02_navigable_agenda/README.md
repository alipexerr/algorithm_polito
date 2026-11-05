# Navigable agenda

## Problem

A personal agenda stores appointments only for dates that actually contain at
least one event. The number of such dates and the number of appointments on
each date are not known in advance.

Appointments may be loaded from a file in arbitrary order. The program must
organize them chronologically, allow appointments to be added and removed, and
let the user move both forward and backward through the stored days and through
the appointments of the currently selected day.

When the last appointment of a date is removed, that date disappears from the
agenda.

## Supplied files

```text
src/main.c                  minimal program entry point
data/example.txt            representative agenda with interleaved dates
data/single_day.txt         several appointments on one date
data/single_appointment.txt smallest non-empty agenda
data/empty.txt              empty initial agenda
```

The prepared configuration initially uses `data/example.txt`. Consult the
Getting Started document to run it or select a different input file.

## Input file

The executable receives the initial agenda file as its only command-line
argument:

```text
navigable_agenda <input_file>
```

The file contains appointments until end of file. Every row has this form:

```text
date time label
```

`date` uses `YYYYMMDD`, `time` uses `HHMM`, and `label` is a single
non-whitespace token.

The complete `data/example.txt` file is:

```text
20260905 1130 lunch
20260903 1845 train
20260912 1600 dentist
20260903 0930 meeting
20260905 0800 lecture
20260903 1400 review
```

Records may appear in any order. The program must never rely on the input
already being sorted. Grouped by day, and ordered by day and then by time, the
same appointments are:

```text
20260903
  0930 meeting
  1400 review
  1845 train

20260905
  0800 lecture
  1130 lunch

20260912
  1600 dentist
```

The input file may be empty.

Two appointments cannot use the same date and time.

## Commands

After loading the file, the program displays the available commands once:

```text
Available commands:
list
open <date>
next-day
previous-day
first
last
next
previous
add <date> <time> <label>
remove <date> <time>
stop
```

It then prints `Command?` before reading each command, without repeating the
complete menu.

`list` prints the complete agenda in chronological order.

`open <date>` selects an existing date and makes its first appointment current.

`next-day` and `previous-day` move the current selection to the following or
preceding stored date. After a successful day movement, the first appointment
of the newly selected date becomes current.

`first` and `last` select the earliest or latest appointment of the current
date.

`next` and `previous` move to the following or preceding appointment of the
current date.

`add <date> <time> <label>` inserts a new appointment in chronological order.
If the date is not already present, it becomes a new stored date in the agenda.

`remove <date> <time>` removes the matching appointment. If no other
appointment remains on that date, the date is removed as well.

`stop` releases all stored data and ends the program.

## Navigation behavior

At the first stored date, `previous-day` cannot move further backward. At the
last stored date, `next-day` cannot move further forward. In both cases the
current selection remains unchanged.

The same rule applies to `previous` at the first appointment of a date and to
`next` at its last appointment.

If no date is currently selected, navigation commands that require a current
date print:

```text
No day selected.
```

and leave the agenda unchanged.

After every successful `open`, `next-day`, `previous-day`, `first`, `last`,
`next`, or `previous`, print the selected appointment.

Moving once to the previous or next day, or once to the previous or next
appointment, must use the current position directly. The program must not
restart a search from the beginning of the corresponding collection merely to
reach an adjacent element.

## Changes to the current selection

Adding an appointment does not change the current date or current appointment.

Removing an appointment that is not currently selected also leaves the current
selection unchanged.

If the current appointment is removed, select the following appointment when
one exists. Otherwise select the preceding appointment.

If the removed appointment was the only one on its date, that date disappears.
In this case select the following stored date when one exists; otherwise select
the preceding stored date. The first appointment of the replacement date
becomes current.

If the last appointment in the entire agenda is removed, no date remains
selected.

## Required behavior

Dates must always be presented in increasing chronological order, and
appointments belonging to one date must always be presented in increasing time
order.

Loading must produce the same chronological agenda for every possible ordering
of the input records.

Only dates containing at least one appointment are retained. The number of
stored dates and the number of appointments on each date are not known before
loading and must not be limited by fixed preallocated tables.

An appointment label must be retained using only the memory required by its
actual text and the string terminator. Temporary input buffers may be larger,
but unused buffer capacity must not remain associated with every stored
appointment.

Removing an appointment or an empty date must release the resources that are no
longer needed. Stopping the program, or failing partway through loading, must
release everything acquired earlier.

## Output

A `list` command for `data/example.txt` prints:

```text
AGENDA
20260903
  0930 meeting
  1400 review
  1845 train
20260905
  0800 lecture
  1130 lunch
20260912
  1600 dentist
```

For an empty agenda, print:

```text
AGENDA
No appointments.
```

After:

```text
open 20260903
next
```

the selected appointment is:

```text
CURRENT
Date: 20260903
Time: 1400
Label: review
```

A missing date supplied to `open` produces:

```text
Day not found.
```

A missing appointment supplied to `remove` produces:

```text
Appointment not found.
```

Attempting to add an appointment at a date and time already present produces:

```text
Appointment already exists.
```

and leaves the agenda unchanged.

Exact spacing between fields is not significant, but ordering, values, and
section labels must make the state unambiguous.

## Constraints and assumptions

- dates use `YYYYMMDD` and may be assumed to represent valid calendar dates;
- times use `HHMM` and may be assumed to represent valid times;
- appointment labels contain from 1 to 63 non-whitespace characters;
- at most one appointment exists for the same date and time;
- every valid input line fits in a 256-character buffer;
- blank lines, in any position, are ignored while loading;
- interactive commands follow the documented format.

## Invalid input and errors

A file that cannot be opened, a malformed appointment record, a repeated
date-and-time pair in the input, or an allocation failure terminates the
program with a non-zero status and a concise diagnostic on standard error.

No command is requested after a fatal loading error. Files already opened must
be closed and every resource acquired before the error must still be released.

Invalid interactive commands do not terminate the program. Exact diagnostic
wording is not part of the normal output contract.

## Starter code

Only a minimal `main.c` is supplied.

Choose the record representation, functions, modules, command-processing
organization, and internal structure of the program. The prepared project
automatically discovers source files added or organized in subdirectories
anywhere below `src/`.

The required behavior, ordering rules, navigation semantics, and memory
constraints define the program contract.
