#!/usr/bin/env python
from distutils.core import setup
from distutils.extension import Extension

try:
    from Cython.Distutils import build_ext
    have_cython = True
except ImportError:
    have_cython = False

if have_cython:
    pymaze_files = ["maze.pyx"]
    cmdclass = {'build_ext': build_ext}
else:
    pymaze_files = ["maze.c"]
    cmdclass = {}

ext_modules = [ Extension(
                    "cymaze",
                    pymaze_files + ['c_maze.c'],
                    include_dirs=["."],
                    extra_compile_args=['--std=c99'],
                )
              ]

setup(
    name = 'cymaze',
    version = '1.0.0',
    ext_modules = ext_modules,
    package_data={'cymaze': [
        "c_maze.h",
        "README.rst",
    ]},
    author='huangyi',
    author_email='yi.codeplayer@gmail.com',
    url = 'https://github.com/yihuang/cymaze',
    cmdclass=cmdclass,
)
