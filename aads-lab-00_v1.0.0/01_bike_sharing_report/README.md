# Bike-sharing station report

## Problem

A bike-sharing operator records the state of every station at the end of the
day. Each record contains the station identifier and name, the total number of
docks, the bicycles currently available, and the rentals completed during the
day.

The program must load one complete snapshot and produce a report that makes the
data easy to inspect. The report shows all stations ordered by daily activity,
summarizes the state of the network, identifies stations requiring attention,
and displays the result of a lookup requested in the input file.

The input order of the records must be preserved. A differently ordered view
must therefore be represented through station indices rather than by moving the
records in the original array.

## Supplied files

```text
src/main.c                  command-line handling and report printing
src/station_report.h        records and documented public operations
src/station_report.c        initial scaffold for the snapshot operations
data/example.txt            representative complete snapshot
data/no_bikes.txt           snapshot with multiple stations without bicycles
data/ties.txt               snapshot containing relevant ties
tests/test_station_report.c focused tests for the public operations
```

The function bodies in `src/station_report.c` are intentionally incomplete and
marked with TODO comments. Non-void functions contain only the neutral return
value needed to keep the initial project buildable.

The signatures declared in `src/station_report.h` must not be changed because
the supplied program and tests call those exact operations. Each declaration
has a documentation comment describing its purpose, input and output
parameters, return value, ordering rules, and relevant preconditions. Additional
private helper functions may be defined in `src/station_report.c` when useful.

The prepared program configuration initially uses `data/example.txt`. Consult
the Getting Started document for the prepared build, run, and debugging
procedures, to select another supplied file or pass a file of your own, and to
run the prepared tests.

## Input

The program receives exactly one command-line argument: the path of the input
file.

```text
bike_sharing_report <input_file>
```

The file has the following structure:

```text
N
station_id station_name docks available_bikes daily_rentals
...
query_station_id
```

The first line contains `N`, the number of stations in the snapshot. Exactly
`N` station records follow. The last line contains the identifier of the
station to search for; this identifier must be formally valid even when the corresponding
 station is not present.

For example:

```text
4
S03 Riverside 20 0 12
S01 Central 24 11 35
S04 University 18 7 35
S02 Museum 16 4 19
S04
```

Each station record contains, in order:

- `station_id`: an identifier unique within the snapshot;
- `station_name`: the station name without white spaces, different words of the name must be separated by _;
- `docks`: the total number of bicycle docks at the station;
- `available_bikes`: the bicycles present at the end of the day;
- `daily_rentals`: the rentals completed during the day.

Identifiers and names are single tokens separated by whitespace. Use an
underscore to represent a name containing more than one word.

The input must satisfy all of these conditions:

- `N` is between 1 and 100;
- station identifiers are unique;
- identifiers and names contain between 1 and 31 characters;
- `docks` is greater than zero;
- `available_bikes` is between zero and `docks`, inclusive;
- `daily_rentals` is not negative.

Every numeric input value and every aggregate total required by the report fit
in the range of the C `int` type.

## Required behavior

The complete snapshot must be read from the input file and validated before any part of the report
is printed. The stations must remain stored in the same order in which they appear
in the file.

The report requires the following information:

- the total number of stations;
- the sum of all docks in the network;
- the sum of all bicycles currently available;
- the sum of all rentals recorded during the day;
- the station with the greatest number of available bicycles;
- all stations with no available bicycles;
- the station requested by `query_station_id`, when present;
- a view of every station ordered by daily rentals.

If several stations have the same greatest number of available bicycles,
select the one with the lexicographically smallest identifier. Stations with no
available bicycles must be listed by identifier in ascending order.

The complete station view is ordered first by `daily_rentals` in descending
order and then, when two stations have the same value, by identifier in
ascending order. Build this view using an integer array of indices. Producing
the view must not reorder or otherwise modify the original station records.

The public operations used to obtain these results are declared and documented
in `src/station_report.h`.

## Output

The reporting code in `src/main.c` prints the sorted station view first, then
the network statistics, and finally the requested-station lookup.

For `data/example.txt`, the complete output is:

```text
STATIONS SORTED BY DAILY RENTALS
ID | Name | Docks | Available bikes | Daily rentals
S01 | Central | 24 | 11 | 35
S04 | University | 18 | 7 | 35
S02 | Museum | 16 | 4 | 19
S03 | Riverside | 20 | 0 | 12

STATS
Stations: 4
Total docks: 78
Available bikes: 22
Daily rentals: 101
Station with most available bikes: S01 (11 bikes)
Stations with no available bikes: 1 (S03)

QUERY RESULT
Requested station: S04
Name: University
Docks: 18
Available bikes: 7
Daily rentals: 35
```

When the requested identifier is absent, the last section has this form:

```text
QUERY RESULT
Requested station: missing
Station not found.
```

When every station has at least one bicycle, the corresponding statistic is:

```text
Stations with no available bikes: 0
```

## Invalid input and errors

If the number of command-line arguments is incorrect, print this diagnostic to
standard error and terminate with a non-zero status:

```text
Usage: bike_sharing_report <input_file>
```

If the file cannot be opened, print:

```text
Cannot open input file.
```

If its contents are invalid, print:

```text
Invalid station snapshot.
```

No partial report is printed after an error.

## Tests

The supplied suite contains at least one focused test for each public operation
declared in `src/station_report.h`. The tests check complete loading, rejection
of representative invalid snapshots, aggregate values, lookup, maximum
selection and its tie-breaking rule, stations without available bicycles, and
the sorted index view without modification of the original records.

The tests are intended to be run incrementally while the public operations are
implemented. Consult the Getting Started document for the procedure used to run
the prepared test configuration.
