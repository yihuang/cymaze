typedef int(*fn_randint)(int);

typedef struct {
    int NumCells;

    // current traversing position
    int g_PtX;
    int g_PtY;

    unsigned char* g_Maze;
} MazeDefine;

typedef enum
{
	eDirection_Invalid = 0,
	eDirection_Up      = 1,
	eDirection_Right   = 2,
	eDirection_Down    = 4,
	eDirection_Left    = 8
}eDirection;

void set_randint(fn_randint fn);
MazeDefine* newMaze(int numCells);
void freeMaze(MazeDefine *m);
void GenerateMaze(MazeDefine *m);
void RenderMaze( MazeDefine *m, unsigned char* img, int ImageSize );
void SaveBMP( const char* FileName, const void* RawBGRImage, int Width, int Height );
