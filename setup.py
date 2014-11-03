#!/usr/bin/env python
from distutils.core import setup
from distutils.extension import Extension

try:
    from Cython.Distutils import build_ext
    have_cython = True
except ImportError:
    have_cython = False

if have_cython:
    cymaze_files = ["cymaze.pyx"]
    cmdclass = {'build_ext': build_ext}
else:
    cymaze_files = ["cymaze.c"]
    cmdclass = {}

ext_modules = [ Extension(
                    "cymaze",
                    cymaze_files + ['c_maze.c'],
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
