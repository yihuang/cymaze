from libc.stdlib cimport malloc
cdef extern from "c_maze.h":
    ctypedef int (*fn_randint) (int)
    void set_randint(fn_randint fn)

    ctypedef struct MazeDefine:
        unsigned char* g_Maze
        int NumCells
        int g_PtX
        int g_PtY

    MazeDefine* newMaze(int numCells)
    void freeMaze(MazeDefine *m)
    void GenerateMaze(MazeDefine *m)
    void RenderMaze( MazeDefine *m, unsigned char* img, int ImageSize )
    void SaveBMP( const char* FileName, const void* RawBGRImage, int Width, int Height )

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
    cdef MazeDefine* m

    def __cinit__(self, int numCells):
        self.m = newMaze(numCells)

    def __dealloc__(self):
        freeMaze(self.m)
        self.m = NULL

    property maze:
        def __get__(self):
            cdef int size = self.m.NumCells*self.m.NumCells
            cdef bytearray ba = bytearray(size)
            cdef int i
            for i in range(size):
                ba[i] = self.m.g_Maze[i] & 0x0f
            return ba

    property size:
        def __get__(self):
            return self.m.NumCells

    property pos:
        def __get__(self):
            return (self.m.g_PtX, self.m.g_PtY)

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
