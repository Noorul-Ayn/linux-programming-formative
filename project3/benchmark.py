"""
benchmark.py
Compares the pure Python statistics implementation against the
faststats C extension on the same data, verifies both produce the
same results, and reports timings.
"""

import random
import time

import purestats
import faststats

ARRAY_SIZE = 5_000_000
RUNS = 3


def time_function(func, data, runs):
    """Run func(data) several times and return the best wall time."""
    best = None
    result = None
    for _ in range(runs):
        start = time.perf_counter()
        result = func(data)
        elapsed = time.perf_counter() - start
        if best is None or elapsed < best:
            best = elapsed
    return best, result


def main():
    print(f"Generating {ARRAY_SIZE:,} random floats...")
    random.seed(42)
    data = [random.uniform(0.0, 1000.0) for _ in range(ARRAY_SIZE)]

    print(f"Timing pure Python version ({RUNS} runs, best taken)...")
    py_time, py_result = time_function(purestats.compute_stats, data, RUNS)

    print(f"Timing C extension version ({RUNS} runs, best taken)...")
    c_time, c_result = time_function(faststats.compute_stats, data, RUNS)

    # Correctness check: both versions must agree
    for key in py_result:
        py_val = py_result[key]
        c_val = c_result[key]
        if isinstance(py_val, float):
            assert abs(py_val - c_val) < 1e-6, f"mismatch on {key}"
        else:
            assert py_val == c_val, f"mismatch on {key}"
    print("Correctness check passed: both versions agree.\n")

    print(f"{'Metric':<12}{'Value':>20}")
    for key, val in c_result.items():
        if isinstance(val, float):
            print(f"{key:<12}{val:>20.6f}")
        else:
            print(f"{key:<12}{val:>20,}")

    speedup = py_time / c_time
    print(f"\n{'Version':<20}{'Best time (s)':>15}")
    print(f"{'Pure Python':<20}{py_time:>15.4f}")
    print(f"{'C extension':<20}{c_time:>15.4f}")

    print(f"\nSpeedup: C extension is {speedup:.1f}x faster.")


if __name__ == "__main__":
    main()
