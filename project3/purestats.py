"""
purestats.py
Pure Python implementation of descriptive statistics on a large array.
Used as the slow baseline for comparison against the C extension.
"""


def compute_stats(data):
    """Compute count, mean, variance, std dev, min and max of a list
    of floats using plain Python loops. Two passes over the data:
    one for the mean, one for the variance."""
    n = len(data)
    if n == 0:
        raise ValueError("empty data")

    total = 0.0
    minimum = data[0]
    maximum = data[0]
    for x in data:
        total += x
        if x < minimum:
            minimum = x
        if x > maximum:
            maximum = x
    mean = total / n

    sq_diff_sum = 0.0
    for x in data:
        diff = x - mean
        sq_diff_sum += diff * diff
    variance = sq_diff_sum / n
    std_dev = variance ** 0.5

    return {
        "count": n,
        "mean": mean,
        "variance": variance,
        "std_dev": std_dev,
        "min": minimum,
        "max": maximum,
    }
