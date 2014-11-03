typedef int(*fn_randint)(int);

typedef struct {
    int x, y;
} V2;

typedef struct {
    V2 maze_size;

    // current traversing position
    V2 pos;

    unsigned char* data;
} Maze;

typedef enum
{
	eDirection_Invalid = 0,
	eDirection_Up      = 1,
	eDirection_Right   = 2,
	eDirection_Down    = 4,
	eDirection_Left    = 8
}eDirection;

void set_randint(fn_randint fn);
Maze* new_maze(V2 maze_size);
void free_maze(Maze *m);
void gen_maze(Maze *m);
void render_maze( Maze *m, unsigned char* img, V2 image_size );
void save_maze( Maze *m, const char* FileName, V2 image_size );
