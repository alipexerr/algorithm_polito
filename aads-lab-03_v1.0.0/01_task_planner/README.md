# Task planner

## Problem

A small task planner keeps track of activities that still have to be completed.
Every task has a unique identifier, a due date, and a short label.

Tasks may be loaded from a file in any order, but the planner must always show
pending activities in chronological order. New tasks can be added while the
program is running, existing tasks can be searched or removed, and the complete
pending list can be displayed at any time.

An optional extension records completed activities and allows the user to undo
the most recent completion.

## Supplied files

```text
src/main.c                         application lifecycle and command dispatch
src/planner/planner.h              public structures and documented operations
src/planner/planner.c              incomplete list operations marked with TODO
src/planner/planner_io.h           supplied input and output interface
src/planner/planner_io.c           supplied loading, command parsing, and reports
tests/test_task_planner.c          focused tests for the required behavior
data/example.txt                   representative tasks in non-chronological order
data/equal_dates.txt               several tasks sharing the same due date
data/single_task.txt               smallest non-empty case
data/empty.txt                     empty initial planner
```

The prepared Run configuration initially uses `data/example.txt`. Consult the
Getting Started document for the procedures used to build, run, debug, and
test the program and to select another input.

## Starter code

This guided exercise introduces a separate I/O module. The supplied program is
organized into three roles:

- `main.c` owns the application lifecycle and dispatches each parsed command;
- `planner_io.h` and `planner_io.c` load tasks, parse commands, and format
  user-facing output;
- `planner.h` and `planner.c` define the planner data and the operations that
  modify or inspect it.

The control flow follows the same separation. `main.c` asks the I/O module to
load or read data, invokes the appropriate planner operation, and passes its
result back to the I/O module for presentation. During the initial load, the
I/O module also uses the public planner operations, so file input and
interactive insertion share the same domain logic.

Complete the TODO bodies in `src/planner/planner.c`. Keep input parsing and
output formatting in the supplied I/O module, do not change the public
declarations, and add only private helpers when useful. The optional
completion-history operations are already connected to the command loop, but
their bodies are not covered by the automatic tests.

## Input file

The executable receives the initial task file as its only command-line
argument:

```text
task_planner <input_file>
```

The file contains tasks until end of file. Every row has this form:

```text
identifier due_date label
```

`due_date` uses the format `YYYYMMDD`. `label` is a single non-whitespace token.

The complete `data/example.txt` file is:

```text
T12 20260909 send_material
T17 20260907 prepare_slides
T08 20260915 write_report
T04 20260909 correct_exercises
```

Records may appear in any order. The program must never rely on the input
already being sorted. The planner must present those tasks as:

```text
T17 20260907 prepare_slides
T04 20260909 correct_exercises
T12 20260909 send_material
T08 20260915 write_report
```

Tasks are ordered by increasing due date. When two tasks have the same due
date, their identifiers are ordered lexicographically.

The file may be empty.

## Commands

After loading the file, the supplied program displays this menu once:

```text
Available commands:
  list                             show all pending tasks
  find <identifier>                show one task
  add <identifier> <date> <label>  add one pending task
  remove <identifier>              remove one pending task
  complete <identifier>            complete one task (optional)
  undo                             undo the latest completion (optional)
  stop                             end the program
```

It then prints the following short prompt before each command:

```text
Command?
```

`list` shows every pending task in the required order.

`find <identifier>` searches for an exact identifier and prints the
corresponding task. If the identifier is absent, print:

```text
Task not found.
```

`add <identifier> <date> <label>` adds a new pending task while preserving the
required ordering. If the identifier already exists, print:

```text
Task already exists.
```

and leave the planner unchanged.

`remove <identifier>` removes the matching pending task. If the identifier is
absent, print `Task not found.` and leave the planner unchanged.

`stop` releases the loaded data and ends the program.

An unknown command prints `Unknown command.` and leaves the planner unchanged.

## Required behavior

The planner must preserve the ordering rule after every successful insertion
and removal. Loading must produce the same ordered planner for every possible
ordering of the input records.

The number of tasks is not known before the file is read. The program must not
reserve storage for an arbitrary maximum number of tasks.

A task label must be retained using only the memory required by its actual text
and the string terminator. Temporary input buffers may be larger, but unused
buffer capacity must not remain associated with every stored task.

Removing a task must release all resources that belong only to that task.
Stopping the program, or failing partway through the initial load, must release
everything acquired earlier.

## Output

A `list` command for `data/example.txt` prints:

```text
PENDING TASKS 4
ID | DUE DATE | LABEL
T17 | 20260907 | prepare_slides
T04 | 20260909 | correct_exercises
T12 | 20260909 | send_material
T08 | 20260915 | write_report
```

When no task is pending, print:

```text
PENDING TASKS 0
```

A successful `find T12` prints:

```text
TASK
ID: T12
Due date: 20260909
Label: send_material
```

After a successful `add` or `remove`, the supplied program prints a concise
confirmation and continues the command loop.

Exact spacing between fields is not significant, but identifiers, dates,
labels, ordering, and section labels must make the result unambiguous.

## Optional extension — completion history

Extend the planner with:

```text
complete <identifier>
undo
```

`complete <identifier>` marks one pending task as completed. The task disappears
from the pending collection, but enough information must be retained to allow
the completion to be undone later.

`undo` restores the task affected by the most recent successful `complete`
command. The restored task becomes pending again and must return to the correct
position according to the normal ordering rule.

Only completion operations are undoable. A task removed with `remove` is
deleted normally and cannot be restored by `undo`.

For example:

```text
complete T04
complete T12
undo
```

restores `T12`, because it is the task completed most recently. A second
`undo` restores `T04`.

If no completed task is available to restore, `undo` leaves the planner
unchanged and prints:

```text
Nothing to undo.
```

Reopening an arbitrary older completed task is outside the scope of this
extension: `undo` always applies to the most recent completion that has not
already been undone.

The number of completed tasks that may still be undone is not bounded in
advance. Their retained data must therefore grow according to the actual
completion history rather than an arbitrary fixed capacity.

The extension is intentionally not covered by the supplied tests. Its internal
organization is part of the additional work.

## Constraints and assumptions

- a task identifier contains from 1 to 15 non-whitespace characters;
- task identifiers are unique among pending tasks;
- a task label contains from 1 to 63 non-whitespace characters;
- dates use `YYYYMMDD` and may be assumed to represent valid calendar dates;
- every valid input line fits in a 256-character buffer;
- blank lines, in any position, are ignored while loading;
- interactive commands follow the documented format.

For the optional extension, an identifier restored by `undo` is again subject
to the normal uniqueness rule for pending tasks.

## Invalid input and errors

A file that cannot be opened, a malformed task record, a repeated task
identifier in the initial file, or an allocation failure terminates the
program with a non-zero status and a concise diagnostic on standard error.

No command is requested after a fatal loading error. Every resource acquired
before the error must still be released. Exact diagnostic wording is not part
of the required output.

Invalid interactive commands do not terminate the program.

## Tests

The supplied tests cover loading from arbitrarily ordered input, ordering by
date and identifier, lookup, duplicate identifiers, insertion at different
ordered positions, removal, empty and single-task planners, and cleanup after a
partial load. One test checks that a complete load-remove-destroy cycle releases
every memory block it acquired.

The optional completion-history extension is not tested automatically.

Run the required tests incrementally while completing the implementation.
