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

void init_maze(Maze *m, V2 maze_size) {
    memset(m, 0, sizeof(Maze));
    m->maze_size = maze_size;
    m->data = (unsigned char*)malloc( m->maze_size.x * m->maze_size.y );
}

Maze* new_maze(V2 maze_size) {
    Maze *m = (Maze*)malloc(sizeof(Maze));
    init_maze(m, maze_size);
    return m;
}

void free_maze(Maze *m) {
    if (m->data) {
        free(m->data);
    }
    free(m);
}

int cellidx(Maze *m, V2 p)
{
    return p.x + m->maze_size.x * p.y;
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

int is_valid_dir( Maze *m, eDirection dir )
{
    V2 newPos = {m->pos.x + Heading_X[ dir ], m->pos.y + Heading_Y[ dir ]};

	if ( !dir || newPos.x < 0 || newPos.y < 0 || newPos.x >= m->maze_size.x || newPos.y >= m->maze_size.y )
        return 0;

	return m->data[ cellidx(m, newPos) ] == 0;
}

eDirection get_direction(Maze *m)
{
	eDirection dir = (eDirection) (1 << randint(3));

	while ( 1 )
	{
		for ( int x = 0; x < 4; x++ )
		{
			if ( is_valid_dir( m, dir ) ) { return (eDirection)dir; }

			dir = (eDirection) (dir << 1);

			if ( dir > eDirection_Left ) { dir = eDirection_Up; }
		}

		dir = (eDirection) (( m->data[ cellidx(m, m->pos) ] & 0xf0 ) >> 4);

		// nowhere to go
		if ( !dir ) return eDirection_Invalid;

		m->pos.x += Heading_X[ dir ];
		m->pos.y += Heading_Y[ dir ];

		dir = (eDirection) (1 << randint(3));
	}
}

void gen_maze(Maze *m)
{
    memset(m->data, 0, m->maze_size.x * m->maze_size.y);
    m->pos.x = randint(m->maze_size.x-1);
    m->pos.y = randint(m->maze_size.y-1);

	for ( eDirection dir = get_direction(m); dir != eDirection_Invalid; dir = get_direction(m) )
	{
		m->data[ cellidx(m, m->pos) ] |= dir;

		m->pos.x += Heading_X[ dir ];
		m->pos.y += Heading_Y[ dir ];

		m->data[ cellidx(m, m->pos) ] = Mask[ dir ];
	}
}

void line( unsigned char* img, int x1, int y1, int x2, int y2, V2 image_size )
{
    y1 = image_size.y - y1 -1;
    y2 = image_size.y - y2 -1;
	if ( x1 == x2 )
	{
        if (x1 >= image_size.x)
            x1 = image_size.x - 1;
        if (y2 < 0)
            y2 = 0;

		// vertical line
		for ( int y = y1; y > y2; y-- )
		{
			if ( x1 >= image_size.x || y >= image_size.y )
                continue;
			int i = 3 * ( y * image_size.x + x1 );
			img[ i + 2 ] = img[ i + 1 ] = img[ i + 0 ] = 255;
		}
	}

	if ( y1 == y2 )
	{
        if (x2 >= image_size.x)
            x2 = image_size.x - 1;
        if (y1 < 0)
            y1 = 0;

		// horizontal line
		for ( int x = x1; x < x2; x++ )
		{
			int i = 3 * ( y1 * image_size.x + x );
			img[ i + 2 ] = img[ i + 1 ] = img[ i + 0 ] = 255;
		}
	}
}

void render_maze( Maze *m, unsigned char* img, V2 image_size )
{
    V2 cell_size = { image_size.x / m->maze_size.x, image_size.y / m->maze_size.y };
    if (cell_size.x <= 0 || cell_size.y <= 0) {
        return;
    }
	for ( int y = 0; y < m->maze_size.y; y++ )
	{
		for ( int x = 0; x < m->maze_size.x; x++ )
		{
            V2 p = {x, y};
			unsigned char v = m->data[ cellidx(m, p) ];

			int nx = x * cell_size.x;
			int ny = y * cell_size.y;

			if ( !( v & eDirection_Up    ) ) line( img, nx,               ny,               nx + cell_size.x + 1, ny                   , image_size);
			if ( !( v & eDirection_Right ) ) line( img, nx + cell_size.x, ny,               nx + cell_size.x,     ny + cell_size.y + 1 , image_size);
			if ( !( v & eDirection_Down  ) ) line( img, nx,               ny + cell_size.y, nx + cell_size.x + 1, ny + cell_size.y     , image_size);
			if ( !( v & eDirection_Left  ) ) line( img, nx,               ny,               nx,                   ny + cell_size.y + 1 , image_size);
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

void save_maze( Maze *m, const char* filename, V2 image_size )
{
	struct sBMPHeader header;

	int buf_size = image_size.x * image_size.y * 3;
    unsigned char* buf = (unsigned char*)malloc(buf_size);
    render_maze(m, buf, image_size);

	header.bfType = 0x4D * 256 + 0x42;
	header.bfSize = buf_size + sizeof( struct sBMPHeader );
	header.bfReserved1 = 0;
	header.bfReserved2 = 0;
	header.bfOffBits = 0x36;
	header.biSize = 40;
	header.biWidth = image_size.x;
	header.biHeight = image_size.y;
	header.biPlanes = 1;
	header.biBitCount = 24;
	header.biCompression = 0;
	header.biSizeImage = buf_size;
	header.biXPelsPerMeter = 6000;
	header.biYPelsPerMeter = 6000;
	header.biClrUsed = 0;
	header.biClrImportant = 0;

    FILE* fp = fopen(filename, "wb");
    fwrite(( const char* )&header, 1,  sizeof( header ), fp );
    fwrite(( const char* )buf, 1, buf_size, fp );
    fclose(fp);
}
