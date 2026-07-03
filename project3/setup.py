"""
setup.py
Build script for the faststats C extension.
Build in place with:  python3 setup.py build_ext --inplace
"""

from setuptools import setup, Extension

setup(
    name="faststats",
    version="1.0",
    description="C extension computing statistics on large arrays",
    ext_modules=[Extension("faststats", sources=["faststats.c"])],
)
