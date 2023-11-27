#include "raylib.h"

typedef struct Board {
    int w;
    int h;
    int **arr;
} Board;

typedef struct Point {
    int x;
    int y;
} Point;

typedef struct Shape {
    Color color;
    int size;
    int i_origin;
    Point *points;
    bool is_alive;
} Shape;

typedef enum Direction {
    LEFT,
    RIGHT,
    DOWN,
    UP
} Direction;

typedef enum Rotation {
    CLOCKWISE = -1,
    COUNTERCLOCKWISE = 1
} Rotation;