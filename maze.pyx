from libc.stdlib cimport malloc
cdef extern from "c_maze.h":
    ctypedef int (*fn_randint) (int)
    void set_randint(fn_randint fn)

    ctypedef struct V2:
        int x,y

    ctypedef struct CMaze "Maze":
        V2 cellSize;
        V2 pos;
        unsigned char* data;

    CMaze* new_maze(V2 cellSize)
    void free_maze(CMaze *m)
    void gen_maze(CMaze *m)
    void render_maze( CMaze *m, unsigned char* img, V2 imageSize )
    void save_maze( CMaze *m, const char* FileName, V2 imageSize )

    cpdef enum eDirection:
        eDirection_Invalid
        eDirection_Up
        eDirection_Right
        eDirection_Down
        eDirection_Left

import random

cdef int randint(int limit):
    return random.randint(0, limit)

set_randint(<fn_randint>randint)

cdef class Maze(object):
    cdef CMaze* m

    def __cinit__(self, tuple size):
        cdef V2 csize
        csize.x = size[0]
        csize.y = size[1]

        self.m = new_maze(csize)

    def __dealloc__(self):
        free_maze(self.m)
        self.m = NULL

    property maze:
        def __get__(self):
            cdef int size = self.m.cellSize.x*self.m.cellSize.y
            cdef bytearray ba = bytearray(size)
            cdef int i
            for i in range(size):
                ba[i] = self.m.data[i] & 0x0f
            return ba

    property size:
        def __get__(self):
            return (self.m.cellSize.x, self.m.cellSize.y)

    property pos:
        def __get__(self):
            return (self.m.pos.x, self.m.pos.y)

    def generate(self):
        gen_maze(self.m)

    def render(self, unsigned char[:] buf, tuple size):
        cdef V2 csize
        csize.x = size[0]
        csize.y = size[1]
        assert len(buf) == csize.x*csize.y*3
        render_maze(self.m, &buf[0], csize)

    def save_bmp(self, bytes filename, tuple size):
        cdef V2 csize
        csize.x = size[0]
        csize.y = size[1]
        save_maze(self.m, filename, csize)
