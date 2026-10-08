# Cyclist training analysis

## Problem

During a training session, every cyclist in a professional group is timed on
each completed lap. Different cyclists may complete different numbers of laps.
The recorded data must remain available after loading so that the coach can ask
several questions without reading the file again.

The program loads the complete group, shows a compact team list, retrieves all
information about a cyclist by name, and identifies the athlete with the
smallest average lap time. Commands may be repeated in any order until the
coach ends the session.

## Supplied files

```text
src/main.c                  minimal entry point and supplied command reader
data/training.txt           representative four-cyclist training session
data/single_cyclist.txt     smallest valid group
data/equal_averages.txt     equal averages resolved by identifier
```

The prepared configuration initially uses `data/training.txt`. Consult the
Getting Started document to run it or select a different input file.

## Input file

The program receives the input-file path as its only command-line argument:

```text
cyclist_training <input_file>
```

The first file line contains the number of cyclists. For every cyclist, the
file then contains:

- name, numeric identifier, and number of completed laps;
- exactly that many lap times.

The complete `data/training.txt` file is:

```text
4
Rossi 100 3
1.30 1.38 1.29
Bianchi 101 5
1.46 1.43 1.42 1.51 1.28
Neri 117 2
1.26 1.34
Verdi 89 4
2.01 1.45 1.43 1.38
```

Names are case-sensitive single tokens. A lap time is a positive real value.

## Commands

After loading the file, the program repeatedly reads one of these commands
from standard input:

```text
list
details <name>
best
stop
```

The supplied program displays the command menu once after loading:

```text
Available commands:
  list
  details <name>
  best
  stop
```

It then displays the following prompt before every command:

```text
Command?
```

- `list` reports the number of cyclists and shows every name, identifier, and
  lap count in file order;
- `details <name>` searches for an exact name and shows that cyclist's
  identifier, lap count, complete sequence of lap times, and average;
- `best` shows the complete details of the cyclist with the smallest average
  lap time;
- `stop` releases all stored data and terminates the program.

If two cyclists have the same average, `best` selects the cyclist with the
smaller identifier. A missing name produces `Cyclist not found.` and does not
terminate the command loop. An unknown command produces `Unknown command.` and
leaves the loaded data unchanged.

## Dynamic data

The number of cyclists is declared by the file and must be retained after
loading. Every cyclist owns:

- a dynamically allocated copy of the name, using only the memory required by
  that string;
- a dynamically allocated array containing exactly the cyclist's lap times.

All names, all lap-time arrays, and the cyclist array itself must be released
when the program terminates or when loading fails partway through the file.

## Output

Lap times and averages are printed with two decimal places. Exact spacing
between fields is not significant, but section labels and values must make the
report unambiguous.

A session using `data/training.txt` may look like this. The command menu is
printed once, before the first prompt, and is omitted here because it is shown
above.

```text
Training loaded: 4 cyclists.
Command? list
CYCLISTS 4
Name | ID | Laps
Rossi | 100 | 3
Bianchi | 101 | 5
Neri | 117 | 2
Verdi | 89 | 4
Command? best
BEST CYCLIST
Name: Neri
ID: 117
Laps: 2
Times: 1.26 1.34
Average: 1.30
Command? details Bianchi
CYCLIST DETAILS
Name: Bianchi
ID: 101
Laps: 5
Times: 1.46 1.43 1.42 1.51 1.28
Average: 1.42
Command? details Blu
Cyclist not found.
Command? stop
Program ended.
```

## Constraints and assumptions

- the file declares at least one cyclist;
- names are unique, case-sensitive single tokens;
- identifiers are positive and unique;
- every cyclist completes at least one lap;
- lap times are positive;
- every valid input line fits in a 256-character buffer;
- the file and entered commands follow the documented format.

## Invalid input and errors

An incorrect argument count, a file that cannot be opened, a failed essential
file read, or an allocation failure terminates the program with a non-zero
status and a concise diagnostic on standard error. Every allocation completed
before the error must still be released. Exact diagnostic wording is not part
of the normal report.

## Starter code

The supplied `main.c` contains complete, documented functions that print the
command menu once and read both the command and the optional cyclist name. Its
initial `main` already repeats command acquisition until input ends and
recognizes `stop`, so the external interface of the program is in place: what
remains is loading the data and answering the `list`, `details` and `best`
commands. The keyboard-input functions do not need to be implemented or moved
to another module.

Choose the cyclist representation, file-loading operations, searches,
statistics, remaining functions, modules, and internal organization of the
program. Source files may be added or organized in subdirectories anywhere
below `src/` without changing `CMakeLists.txt`.
