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

void initMaze(Maze *m, V2 cellSize) {
    memset(m, 0, sizeof(Maze));
    m->cellSize = cellSize;
    m->data = (unsigned char*)malloc( m->cellSize.x * m->cellSize.y );
}

Maze* new_maze(V2 cellSize) {
    Maze *m = (Maze*)malloc(sizeof(Maze));
    initMaze(m, cellSize);
    return m;
}

void free_maze(Maze *m) {
    if (m->data) {
        free(m->data);
    }
    free(m);
}

// return the current index in m->data
//int CellIdx(Maze *m)
//{
//	return m->pos.x + m->cellSize.x * m->pos.y;
//}

int CellIdx(Maze *m, V2 p)
{
    return p.x + m->cellSize.x * p.y;
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

int IsDirValid( Maze *m, eDirection Dir )
{
    V2 newPos = {m->pos.x + Heading_X[ Dir ], m->pos.y + Heading_Y[ Dir ]};

	if ( !Dir || newPos.x < 0 || newPos.y < 0 || newPos.x >= m->cellSize.x || newPos.y >= m->cellSize.y )
        return 0;

	return m->data[ CellIdx(m, newPos) ] == 0;
}

eDirection GetDirection(Maze *m)
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

		Dir = (eDirection) (( m->data[ CellIdx(m, m->pos) ] & 0xf0 ) >> 4);

		// nowhere to go
		if ( !Dir ) return eDirection_Invalid;

		m->pos.x += Heading_X[ Dir ];
		m->pos.y += Heading_Y[ Dir ];

		Dir = (eDirection) (1 << randint(3));
	}
}

void gen_maze(Maze *m)
{
    memset(m->data, 0, m->cellSize.x * m->cellSize.y);
    m->pos.x = randint(m->cellSize.x-1);
    m->pos.y = randint(m->cellSize.y-1);

	for ( eDirection Dir = GetDirection(m); Dir != eDirection_Invalid; Dir = GetDirection(m) )
	{
		m->data[ CellIdx(m, m->pos) ] |= Dir;

		m->pos.x += Heading_X[ Dir ];
		m->pos.y += Heading_Y[ Dir ];

		m->data[ CellIdx(m, m->pos) ] = Mask[ Dir ];
	}
}

void Line( unsigned char* img, int x1, int y1, int x2, int y2, V2 ImageSize )
{
    y1 = ImageSize.y - y1 -1;
    y2 = ImageSize.y - y2 -1;
	if ( x1 == x2 )
	{
		// vertical line
		for ( int y = y1; y > y2; y-- )
		{
			if ( x1 >= ImageSize.x || y >= ImageSize.y ) continue;
			int i = 3 * ( y * ImageSize.x + x1 );
			img[ i + 2 ] = img[ i + 1 ] = img[ i + 0 ] = 255;
		}
	}

	if ( y1 == y2 )
	{
		// horizontal line
		for ( int x = x1; x < x2; x++ )
		{
			if ( y1 >= ImageSize.y || x >= ImageSize.x ) continue;
			int i = 3 * ( y1 * ImageSize.x + x );
			img[ i + 2 ] = img[ i + 1 ] = img[ i + 0 ] = 255;
		}
	}
}

void render_maze( Maze *m, unsigned char* img, V2 ImageSize )
{
    V2 CellSize = { ImageSize.x / m->cellSize.x, ImageSize.y / m->cellSize.y };
	for ( int y = 0; y < m->cellSize.y; y++ )
	{
		for ( int x = 0; x < m->cellSize.x; x++ )
		{
			unsigned char v = m->data[ y * m->cellSize.x + x ];

			int nx = x * CellSize.x;
			int ny = y * CellSize.y;

			if ( !( v & eDirection_Up    ) ) Line( img, nx,              ny,              nx + CellSize.x + 1, ny                  , ImageSize);
			if ( !( v & eDirection_Right ) ) Line( img, nx + CellSize.x, ny,              nx + CellSize.x,     ny + CellSize.y + 1 , ImageSize);
			if ( !( v & eDirection_Down  ) ) Line( img, nx,              ny + CellSize.y, nx + CellSize.x + 1, ny + CellSize.y     , ImageSize);
			if ( !( v & eDirection_Left  ) ) Line( img, nx,              ny,              nx,                  ny + CellSize.y + 1 , ImageSize);
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

void save_maze( Maze *m, const char* FileName, V2 imageSize )
{
	struct sBMPHeader Header;

	int ImageSize = imageSize.x * imageSize.y * 3;
    unsigned char* RawBGRImage = (unsigned char*)malloc(ImageSize);
    render_maze(m, RawBGRImage, imageSize);

	Header.bfType = 0x4D * 256 + 0x42;
	Header.bfSize = ImageSize + sizeof( struct sBMPHeader );
	Header.bfReserved1 = 0;
	Header.bfReserved2 = 0;
	Header.bfOffBits = 0x36;
	Header.biSize = 40;
	Header.biWidth = imageSize.x;
	Header.biHeight = imageSize.y;
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
