# Exam session conflict detector

## Problem

A university schedules several exams in the same time slot. Each lecturer
exports the list of students registered for one exam, and the scheduling office
must check whether a student has been assigned to two or more simultaneous
exams.

The program receives the description of one exam session, reads all the
registration lists it references, and produces a conflict report. Every
conflicting student must appear once, followed by all the simultaneous exams
for which that student is registered.

Registration lists are already ordered by student identifier, and the conflict
report follows the same identifier order.

## Supplied files

```text
src/main.c                 minimal starting point
data/session.txt           representative two-way and three-way conflicts
data/no_conflicts.txt      simultaneous exams with no shared students
data/empty_exam.txt        one empty registration list and one conflict
data/{ads,databases,networks}.students
                           registration lists used by session.txt
data/{os,statistics,economics}.students
                           registration lists used by no_conflicts.txt
data/{empty,physics,chemistry}.students
                           registration lists used by empty_exam.txt
```

Each session file names its corresponding registration files in the `data`
directory. The prepared configuration initially uses `data/session.txt`; the
Getting Started document explains how to build, run, and debug the program and
how to select another input. The starter intentionally contains only a minimal
`main.c`; add source and header files as needed.

## Input files

The executable receives the path of one session file:

```text
exam_session_conflicts <session_file>
```

The session file begins with the date and starting time of the session, written
as `YYYYMMDDTHHMM`. The next line contains the number of simultaneous exams.
Each remaining line contains an exam code and the path of its registration
file:

```text
session_timestamp
exam_count
exam_code registration_file
...
```

For example:

```text
20261015T0900
3
ADS data/ads.students
DATABASES data/databases.students
NETWORKS data/networks.students
```

Paths are interpreted relative to the working directory from which the program
is started, which for the prepared configuration is the exercise directory.

Each registration file begins with its number of students and then contains
one student identifier per line:

```text
student_count
student_id
student_id
...
```

For example, `data/ads.students` could contain:

```text
4
s10234
s12580
s18421
s20987
```

Identifiers in the same file are unique and are already in ascending
lexicographic order. Different exams may contain the same identifier: those
repetitions are precisely the conflicts that the program must detect. An exam
may have no registered students.

## Required behavior

After loading all registration files, identify every student registered for at
least two exams in the session.

The report must satisfy these rules:

- each conflicting student is printed exactly once;
- students are printed in ascending identifier order;
- all exams involving that student are printed on the same line;
- exam codes on a conflict line retain their order from the session file;
- a registration belonging to only one exam is not a conflict;
- an empty registration file does not create a conflict.

For example, suppose the three registration files produce two conflicts. The
result is:

```text
SESSION 20261015T0900
CONFLICTS 2
s12580 | ADS DATABASES
s18421 | ADS DATABASES NETWORKS
```

The number after `CONFLICTS` is the number of conflicting students, not the
number of registrations involved. A student registered for three simultaneous
exams therefore still counts as one conflict.

With `data/no_conflicts.txt`, no student is registered for more than one exam,
so print:

```text
SESSION 20261016T1400
CONFLICTS 0
```

No additional conflict lines follow.

## Memory requirements

The program must not reserve a fixed table sized for the maximum possible
number of exams, students, or identifier characters. Memory retained after
loading must be proportional to the actual number of exams, the actual number
of registrations for each exam, and the actual length of every identifier.

The registration collection for each exam must be allocated independently and
contain exactly its actual number of registrations. An exam with no registered
students must not reserve room for possible future identifiers, and short
identifiers must not occupy buffers sized for the longest permitted one. Every
identifier read from a registration file is retained using only the space
required by its contents and the string terminator.

The choice of data structures, functions, and modules is part of the exercise.
Any additional memory used while producing the report must reflect the data
that are actually present rather than an arbitrary maximum.

## Constraints and assumptions

- the number of exams is not negative; a session with zero or one exam simply
  has no conflicts;
- exam codes are unique non-empty tokens;
- registration-file paths are non-empty tokens and contain no whitespace;
- the number of registered students for each exam is not negative;
- a student identifier is a non-empty token;
- identifiers in each registration file are unique and already in ascending
  lexicographic order;
- every valid input line fits in a 256-character buffer;
- the session timestamp and the ordering of each registration file may be
  assumed valid.

## Invalid input and errors

A session file that cannot be opened, an unavailable registration file, an
invalid or missing count, a missing required token, or an allocation failure
terminates the program with a non-zero status and a concise diagnostic on
standard error.

No partial conflict report is printed. Files already opened must be closed and
all memory acquired while loading earlier exams must still be released. In
other words, an error interrupts the loading process but does not transfer
ownership of any partially loaded data to the caller.
