# Linux Programming Formative

Formative projects for the Linux Programming course. Each project folder
contains the source code, the exact commands used, and the output evidence
generated on my machine. Full written reports for all four projects are in
the accompanying submission document.

**Author:** Hauwa Muhammad Bello
**Environment:** Ubuntu 24.04 on WSL2, gcc 13.3, GNU binutils 2.42, strace 6.8, Python 3.12

## Repository structure

| Folder | Project | Summary |
|---|---|---|
| project1 | Investigating a Suspicious Binary | ELF analysis of a file sync tool using objdump and nm, with a forensic report |
| project2 | System Call Monitoring Tool | A backup program traced with strace, with a syscall summary table |
| project3 | Python Performance Extension | A CPython C extension computing statistics, benchmarked against pure Python (10.9x speedup) |
| project4 | Signal-Based Server Controller | A monitor service handling SIGINT, SIGUSR1 and SIGTERM via sigaction |

## Project 1: Investigating a Suspicious Binary

A file synchronization tool (`data_sync.c`) is compiled and then investigated
using only static ELF inspection, without executing it, to determine its
likely behavior from its entry point, sections, and imported symbols.

**Files:** `data_sync.c` (source), `data_sync` (binary under analysis),
`output_filetype.txt`, `output_entrypoint.txt`, `output_sections.txt`,
`output_imports.txt`, `output_symbols.txt` (analysis evidence).

**Reproduce:**
```bash
cd project1
gcc -Wall -Wextra -o data_sync data_sync.c
file data_sync
objdump -f data_sync      # entry point
objdump -h data_sync      # sections
nm -D --undefined-only data_sync   # imported functions
nm data_sync              # all symbols
```

## Project 2: System Call Monitoring Tool

A small backup program (`backup_monitor.c`) creates a data file, writes
timestamped log entries, and reads the file back. It is run under strace to
capture every system call, and the trace is interpreted and classified into
file-related and process-related calls.

**Files:** `backup_monitor.c` (source), `backup_monitor` (binary),
`strace_output.txt` (complete 48-line trace), `backup_data.txt` and
`backup.log` (files produced by the traced run).

**Reproduce:**
```bash
cd project2
gcc -Wall -Wextra -o backup_monitor backup_monitor.c
strace -o strace_output.txt ./backup_monitor
```

## Project 3: Python Performance Extension

The same descriptive statistics (count, mean, variance, standard deviation,
min, max) are computed over 5,000,000 floats twice: in pure Python
(`purestats.py`) and in a C extension written against the CPython API
(`faststats.c`). Both use the identical two-pass algorithm so the benchmark
isolates language overhead. The benchmark verifies both versions agree
before timing them. Result on my machine: pure Python 0.3507 s, C extension
0.0322 s, a 10.9x speedup.

**Files:** `purestats.py` (baseline), `faststats.c` (extension source),
`setup.py` (build script), `benchmark.py` (benchmark with correctness
check), `benchmark_results.txt` (results), `faststats.cpython-312-*.so`
(compiled extension), `build/` (build artifacts).

**Reproduce:**
```bash
cd project3
sudo apt install python3-dev python3-setuptools
python3 setup.py build_ext --inplace
python3 benchmark.py
```

## Project 4: Signal-Based Server Controller

A background service (`monitor_service.c`) prints a heartbeat every 5
seconds and responds to administrative signals registered with sigaction():
SIGINT triggers a graceful shutdown through a volatile flag checked by the
main loop, SIGUSR1 prints a status message and continues, and SIGTERM prints
an emergency message and exits immediately. Handlers use only
async-signal-safe functions (write, _exit).

**Files:** `monitor_service.c` (source), `monitor_service` (binary),
`demo_terminal1.txt` (service output during the demonstration),
`demo_terminal2.txt` (kill commands sent from a second terminal).

**Reproduce:**
```bash
cd project4
gcc -Wall -Wextra -o monitor_service monitor_service.c
./monitor_service          # note the printed PID
# from a second terminal:
kill -SIGUSR1 <pid>
kill -SIGTERM <pid>
# graceful shutdown: press Ctrl+C in the service terminal
```
