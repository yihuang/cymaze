#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "c_maze.h"

int c_randint(int limit) {
    // return a random number between 0 and limit inclusive.

    int divisor = RAND_MAX/(limit+1);
    int retval;

    do { 
        retval = rand() / divisor;
    } while (retval > limit);

    return retval;
}

static fn_randint randint = c_randint;

void set_randint(fn_randint fn) {
    randint = fn;
}

void initMaze(MazeDefine *m, int numCells) {
    memset(m, 0, sizeof(MazeDefine));
    m->NumCells = numCells;
    m->g_Maze = (unsigned char*)malloc( m->NumCells* m->NumCells );
}

MazeDefine* newMaze(int numCells) {
    MazeDefine *m = (MazeDefine*)malloc(sizeof(MazeDefine));
    initMaze(m, numCells);
    return m;
}

void freeMaze(MazeDefine *m) {
    if (m->g_Maze) {
        free(m->g_Maze);
    }
    free(m);
}

// return the current index in m->g_Maze
int CellIdx(MazeDefine *m)
{
	return m->g_PtX + m->NumCells * m->g_PtY;
}

//                   0  1  2  3  4  5  6  7  8
//                      U  R     D           L
int Heading_X[9] = { 0, 0,+1, 0, 0, 0, 0, 0,-1 };
int Heading_Y[9] = { 0,-1, 0, 0,+1, 0, 0, 0, 0 };
int Mask[9]      = {
							0,
							eDirection_Down | eDirection_Down << 4,
							eDirection_Left | eDirection_Left << 4,
							0,
							eDirection_Up | eDirection_Up << 4,
							0,
							0,
							0,
							eDirection_Right | eDirection_Right << 4
						};


////////////////////////////////////////////////////////////////////////////

int IsDirValid( MazeDefine *m, eDirection Dir )
{
	int NewX = m->g_PtX + Heading_X[ Dir ];
	int NewY = m->g_PtY + Heading_Y[ Dir ];

	if ( !Dir || NewX < 0 || NewY < 0 || NewX >= m->NumCells || NewY >= m->NumCells ) return 0;

	return m->g_Maze[ NewX + m->NumCells * NewY ] == 0;
}

eDirection GetDirection(MazeDefine *m)
{
	eDirection Dir = (eDirection) (1 << randint(3));

	while ( 1 )
	{
		for ( int x = 0; x < 4; x++ )
		{
			if ( IsDirValid( m, Dir ) ) { return (eDirection)Dir; }

			Dir = (eDirection) (Dir << 1);

			if ( Dir > eDirection_Left ) { Dir = eDirection_Up; }
		}

		Dir = (eDirection) (( m->g_Maze[ CellIdx(m) ] & 0xf0 ) >> 4);

		// nowhere to go
		if ( !Dir ) return eDirection_Invalid;

		m->g_PtX += Heading_X[ Dir ];
		m->g_PtY += Heading_Y[ Dir ];

		Dir = (eDirection) (1 << randint(3));
	}
}

void GenerateMaze(MazeDefine *m)
{
    memset(m->g_Maze, 0, m->NumCells* m->NumCells);
    m->g_PtX = randint(m->NumCells-1);
    m->g_PtY = randint(m->NumCells-1);
    printf("new starting point %d %d\n", m->g_PtX, m->g_PtY);

	int Cells = 0;
	for ( eDirection Dir = GetDirection(m); Dir != eDirection_Invalid; Dir = GetDirection(m) )
	{
		if ( ++Cells % 1000 == 0 ) {
            printf(".");
            fflush(stdout);
        }

		m->g_Maze[ CellIdx(m) ] |= Dir;

		m->g_PtX += Heading_X[ Dir ];
		m->g_PtY += Heading_Y[ Dir ];

		m->g_Maze[ CellIdx(m) ] = Mask[ Dir ];
	}
    printf("\n");
}

void Line( unsigned char* img, int x1, int y1, int x2, int y2, int ImageSize )
{
	if ( x1 == x2 )
	{
		// vertical line
		for ( int y = y1; y < y2; y++ )
		{
			if ( x1 >= ImageSize || y >= ImageSize ) continue;
			int i = 3 * ( y * ImageSize + x1 );
			img[ i + 2 ] = img[ i + 1 ] = img[ i + 0 ] = 255;
		}
	}

	if ( y1 == y2 )
	{
		// horizontal line
		for ( int x = x1; x < x2; x++ )
		{
			if ( y1 >= ImageSize || x >= ImageSize ) continue;
			int i = 3 * ( y1 * ImageSize + x );
			img[ i + 2 ] = img[ i + 1 ] = img[ i + 0 ] = 255;
		}
	}
}

void RenderMaze( MazeDefine *m, unsigned char* img, int ImageSize )
{
    int CellSize = ImageSize / m->NumCells;
	for ( int y = 0; y < m->NumCells; y++ )
	{
		for ( int x = 0; x < m->NumCells; x++ )
		{
			unsigned char v = m->g_Maze[ y * m->NumCells + x ];

			int nx = x * CellSize;
			int ny = y * CellSize;

			if ( !( v & eDirection_Up    ) ) Line( img, nx,            ny,            nx + CellSize + 1, ny                , ImageSize);
			if ( !( v & eDirection_Right ) ) Line( img, nx + CellSize, ny,            nx + CellSize,     ny + CellSize + 1 , ImageSize);
			if ( !( v & eDirection_Down  ) ) Line( img, nx,            ny + CellSize, nx + CellSize + 1, ny + CellSize     , ImageSize);
			if ( !( v & eDirection_Left  ) ) Line( img, nx,            ny,            nx,                ny + CellSize + 1 , ImageSize);
		}
	}
}

#if defined( __GNUC__ )
# define GCC_PACK(n) __attribute__((packed,aligned(n)))
#else
# define GCC_PACK(n) __declspec(align(n))
#endif // __GNUC__

#pragma pack(push, 1)
struct GCC_PACK( 1 ) sBMPHeader
{
	// BITMAPFILEHEADER
	unsigned short bfType;
	uint32_t bfSize;
	unsigned short bfReserved1;
	unsigned short bfReserved2;
	uint32_t bfOffBits;
	// BITMAPINFOHEADER
	uint32_t biSize;
	uint32_t biWidth;
	uint32_t biHeight;
	unsigned short biPlanes;
	unsigned short biBitCount;
	uint32_t biCompression;
	uint32_t biSizeImage;
	uint32_t biXPelsPerMeter;
	uint32_t biYPelsPerMeter;
	uint32_t biClrUsed;
	uint32_t biClrImportant;
};
#pragma pack(pop)

void SaveBMP( const char* FileName, const void* RawBGRImage, int Width, int Height )
{
	struct sBMPHeader Header;

	int ImageSize = Width * Height * 3;

	Header.bfType = 0x4D * 256 + 0x42;
	Header.bfSize = ImageSize + sizeof( struct sBMPHeader );
	Header.bfReserved1 = 0;
	Header.bfReserved2 = 0;
	Header.bfOffBits = 0x36;
	Header.biSize = 40;
	Header.biWidth = Width;
	Header.biHeight = Height;
	Header.biPlanes = 1;
	Header.biBitCount = 24;
	Header.biCompression = 0;
	Header.biSizeImage = ImageSize;
	Header.biXPelsPerMeter = 6000;
	Header.biYPelsPerMeter = 6000;
	Header.biClrUsed = 0;
	Header.biClrImportant = 0;

    FILE* fp = fopen(FileName, "wb");
    fwrite(( const char* )&Header, 1,  sizeof( Header ), fp );
    fwrite(( const char* )RawBGRImage, 1, ImageSize, fp );
    fclose(fp);
}

void test() {
    for(int i=0; i<1000; i++) {
        printf("%d\n", randint(3));
    }
}

static int main() {
    srand(time(NULL));

    MazeDefine *m = newMaze(63);
    GenerateMaze(m);

    int ImageSize = 512;
    unsigned char* img = (unsigned char*)malloc(3*ImageSize*ImageSize);
    memset(img, 0, 3*ImageSize*ImageSize);
    RenderMaze(m, img, ImageSize);

	SaveBMP( "Maze.bmp", img, ImageSize, ImageSize );
    free(img);

    freeMaze(m);
}
