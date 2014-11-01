from libc.stdlib cimport malloc
cdef extern from "c_maze.h":
    ctypedef int (*fn_randint) (int)
    void set_randint(fn_randint fn)

    ctypedef struct MazeDefine
    MazeDefine* newMaze(int numCells)
    void freeMaze(MazeDefine *m)
    void GenerateMaze(MazeDefine *m)
    void RenderMaze( MazeDefine *m, unsigned char* img, int ImageSize )
    void SaveBMP( const char* FileName, const void* RawBGRImage, int Width, int Height )

cdef extern void test()

import random

cdef int randint(int limit):
    return random.randint(0, limit)

set_randint(<fn_randint>randint)

cdef class Maze(object):
    cdef MazeDefine* m

    def __cinit__(self, int numCells):
        self.m = newMaze(numCells)
        test()

    def __dealloc__(self):
        freeMaze(self.m)
        self.m = NULL

    def generate(self):
        GenerateMaze(self.m)

    def render(self, unsigned char[:] buf, int imageSize):
        assert len(buf) == imageSize*imageSize*3
        RenderMaze(self.m, &buf[0], imageSize)

    def save_bmp(self, bytes filename, int imageSize):
        cdef int size = 3*imageSize*imageSize
        cdef unsigned char* buf = <unsigned char*>malloc(size)
        RenderMaze(self.m, buf, imageSize)
        SaveBMP(filename, buf, imageSize, imageSize)
