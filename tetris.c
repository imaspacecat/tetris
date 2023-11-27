#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include "tetris.h"
#include "raylib.h"

void init_board(Board *board) {
    for (int i = 0; i < board->h; ++i) {
        for (int j = 0; j < board->w; ++j) {
            board->arr[i][j] = 0;
        }
    }
}

Board *create_board(int width, int height) {
    Board *board = malloc(sizeof(Board));
    board->arr = malloc(height * sizeof(int *));

    for (int i = 0; i < height; ++i) {
        board->arr[i] = malloc(width * sizeof(int));
    }

    board->w = width;
    board->h = height;

    init_board(board);
    return board;
}

void free_board(Board *board) {
    free(board->arr);
    free(board);
}

void print_board(Board *board) {
    for (int i = 0; i < board->h; ++i) {
        for (int j = 0; j < board->w; ++j) {
            printf("%d", board->arr[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

Shape *create_shape(int size, Color color, int i_origin, Point *points) {
    Shape *shape = malloc(sizeof(Shape));
    shape->points = malloc(size * sizeof(Point));
    shape->size = size;
    shape->color = color;
    shape->i_origin = i_origin;
    shape->is_alive = true;

    for (int i = 0; i < size; ++i) {
        shape->points[i] = points[i];
    }

    return shape;
}

void free_shape(Shape *shape) {
    free(shape->points);
    free(shape);
}

void draw_shape(Shape *shape, Board *board, int c) {
    for (int i = 0; i < shape->size; i++) {
        board->arr[shape->points[i].y][shape->points[i].x] = c;
    }
}

void erase_shape(Shape *shape, Board *board) {
    draw_shape(shape, board, 0);
}

void kill_shape(Shape *shape, Board *board) {
    draw_shape(shape, board, 2);
}

void print_shape(Shape *shape) {
    for (int i = 0; i < shape->size; ++i) {
        printf("(%d, %d)\n", shape->points[i].x, shape->points[i].y);
    }
}


Point o_points[] = {
        {0, 0},
        {1, 0},
        {0, 1},
        {1, 1}
};
Shape o_shape = {YELLOW, 4, -1, o_points};

Point i_points[] = {
        {0, 0},
        {0, 1},
        {0, 2},
        {0, 3}
};

Shape i_shape = {SKYBLUE, 4, 1, i_points};

Point s_points[] = {
        {1, 0},
        {2, 0},
        {0, 1},
        {1, 1}
};
Shape s_shape = {RED, 4, 3, s_points};

Point z_points[] = {
        {0, 0},
        {1, 0},
        {1, 1},
        {2, 1}
};
Shape z_shape = {GREEN, 4, 2, z_points};

Point l_points[] = {
        {0, 0},
        {0, 1},
        {0, 2},
        {1, 2}
};
Shape l_shape = {ORANGE, 4, 1, l_points};

Point j_points[] = {
        {1, 0},
        {1, 1},
        {1, 2},
        {0, 2}
};
Shape j_shape = {PINK, 4, 1, j_points};

Point t_points[] = {
        {0, 0},
        {1, 0},
        {2, 0},
        {1, 1}
};
Shape t_shape = {PURPLE, 4, 1, t_points};


const Shape *shapes[] = {&o_shape, &i_shape, &s_shape, &z_shape, &l_shape, &j_shape, &t_shape};

Shape *rand_shape() {
    int i = (rand() % 7);
    printf("%d\n", i);
//    i = 1;
    Shape *shape = create_shape(shapes[i]->size, shapes[i]->color, shapes[i]->i_origin, shapes[i]->points);
    return shape;
}


bool is_legal_move(Shape *shape, Board *board) {
    for (int i = 0; i < shape->size; i++) {
        if (shape->points[i].x < 0 || shape->points[i].x >= board->w ||
            shape->points[i].y >= board->h || shape->points[i].y < 0 ||
            board->arr[shape->points[i].y][shape->points[i].x] == 2) {
            printf("illegal move\n");
            return false;
        }
    }

    return true;
}

void rotate_shape(Shape *shape, Rotation rotation) {
    // scuff fix to not rotate o shape
    if (shape->i_origin == -1) {
        return;
    }
    for (int i = 0; i < shape->size; i++) {
        int origin_x = shape->points[shape->i_origin].x;
        int origin_y = shape->points[shape->i_origin].y;
        Point translated = {
                shape->points[i].x - origin_x,
                shape->points[i].y - origin_y
        };

        translated.y *= -1;

        shape->points[i].x = -translated.y * rotation;
        shape->points[i].y = translated.x * rotation;

        shape->points[i].y *= -1;
        shape->points[i].x += origin_x;
        shape->points[i].y += origin_y;
    }
}

void move_shape(Shape *shape, Direction direction) {
    if (direction == LEFT) {
        for (int i = 0; i < shape->size; i++) {
            shape->points[i].x--;
        }
    } else if (direction == RIGHT) {
        for (int i = 0; i < shape->size; i++) {
            shape->points[i].x++;
        }
    } else if (direction == DOWN) {
        for (int i = 0; i < shape->size; i++) {
            shape->points[i].y++;
        }
    } else {
        for (int i = 0; i < shape->size; i++) {
            shape->points[i].y--;
        }
    }
}

void update_shape(Shape *shape, Board *board, Direction direction, int c) {
    erase_shape(shape, board);
    move_shape(shape, direction);
    draw_shape(shape, board, c);
}

void rotate_clockwise(Shape *shape, Board *board) {
    erase_shape(shape, board);
    rotate_shape(shape, CLOCKWISE);
    printf("rotated clockwise shape: \n");
    print_shape(shape);
    if (!is_legal_move(shape, board)) {
        rotate_shape(shape, COUNTERCLOCKWISE);
    }
    draw_shape(shape, board, 1);
}

void rotate_counterclockwise(Shape *shape, Board *board) {
    erase_shape(shape, board);
    rotate_shape(shape, COUNTERCLOCKWISE);
    if (!is_legal_move(shape, board)) {
        rotate_shape(shape, CLOCKWISE);
    }
    draw_shape(shape, board, 1);
}

void left(Shape *shape, Board *board) {
    erase_shape(shape, board);
    move_shape(shape, LEFT);
    if (!is_legal_move(shape, board)) {
        move_shape(shape, RIGHT);
    }
    draw_shape(shape, board, 1);
}

void right(Shape *shape, Board *board) {
    erase_shape(shape, board);
    move_shape(shape, RIGHT);
    if (!is_legal_move(shape, board)) {
        move_shape(shape, LEFT);
    }
    draw_shape(shape, board, 1);
}

void down(Shape *shape, Board *board) {
    erase_shape(shape, board);
    move_shape(shape, DOWN);
    if (!is_legal_move(shape, board)) {
        move_shape(shape, UP);
        draw_shape(shape, board, 2);
        shape->is_alive = false;
        return;
    }
    draw_shape(shape, board, 1);
}

// for debugging purposes
void up(Shape *shape, Board *board) {
    erase_shape(shape, board);
    move_shape(shape, UP);
    if (!is_legal_move(shape, board)) {
        move_shape(shape, DOWN);
        draw_shape(shape, board, 2);
        return;
    }
    draw_shape(shape, board, 1);
}

//Shape *create_shadow_shape(Shape *shape, Board *board) {
//    Shape *shadow = create_shape(shape->size, LIGHTGRAY, shape->i_origin, shape->points);
//    move_shape(shadow, DOWN);
//    while (is_legal_move(shadow, board)) {
//        move_shape(shadow, DOWN);
//    }
//    move_shape(shadow, UP);
//
//    return shadow;
//}

void copy_row(int *src, int *dst, int len) {
    for (int i = 0; i < len; i++) {
        dst[i] = src[i];
    }
}

void remove_row(int i_row, Board *board) {
    for (int i = i_row; i > 0; --i) {
        copy_row(board->arr[i-1], board->arr[i], board->w);
    }

    for (int i = 0; i < board->w; i++) {
        board->arr[0][i] = 0;
    }
}

// empty: 0
// shape: 1
// dead:  2
// shadow: 3

int main() {
    // rng bs idk what im doing
    time_t t;
    srand((unsigned) time(&t));

    const int board_width = 10;
    const int board_height = 20;

    int rows_completed = 0;
    int tetrises = 0;

    Board *board = create_board(board_width, board_height);

    Shape *current_shape = rand_shape();

    draw_shape(current_shape, board, 1);

//    print_board(board);
//
//    copy_row(board->arr[0], board->arr[3], board->w);
//    print_board(board);
//    remove_row(3, board);
//    print_board(board);
//    remove_row(5, board);
//    print_board(board);

    // =============================================================================================

    const int screenWidth = 1000;
    const int screenHeight = 1000;

    const int rect_size = screenWidth / board_height;

    const float side_move_rate = 100;
    const float down_move_rate = 50;
    const float gravity_rate = 500;
    const float initial_movement_delay = 300;

    int frame_count = 0;
    double key_down_frame_time_sum = 0;
    double movement_frame_time_sum = 0;

    double gravity_frame_time_sum = 0;

    InitWindow(screenWidth, screenHeight, "tetris");

    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT | FLAG_WINDOW_HIGHDPI);
    SetTargetFPS(60);

    // 10 x 20
    double initial_press_time_left = 0;
    double subsequent_press_time_left = 0;
    double initial_press_time_right = 0;
    double subsequent_press_time_right = 0;


//    Shape *shadow = create_shadow_shape(current_shape, board) ;

    while (!WindowShouldClose()) {

        movement_frame_time_sum += GetFrameTime();
        initial_press_time_left += GetFrameTime();
        subsequent_press_time_left += GetFrameTime();
        initial_press_time_right += GetFrameTime();
        subsequent_press_time_right += GetFrameTime();

        // spawn new shape once current shape dies
        if (!current_shape->is_alive) {
            current_shape = rand_shape();
//            shadow = create_shadow_shape(current_shape, board);

        }// else {
//            erase_shape(shadow, board);
//            shadow = create_shadow_shape(current_shape, board);
//            draw_shape(shadow, board, 3);
//        }

        if (IsKeyPressed(KEY_LEFT)) {
            left(current_shape, board);
        }

        if (IsKeyDown(KEY_LEFT)) {
            if (initial_press_time_left * 1000 > initial_movement_delay) {
                if (subsequent_press_time_left * 1000 > side_move_rate) {
                    subsequent_press_time_left = 0;
                    left(current_shape, board);
                }
            }
        }

        if (IsKeyUp(KEY_LEFT)) {
            initial_press_time_left = 0;
            subsequent_press_time_left = 0;
        }

        if (IsKeyPressed(KEY_RIGHT)) {
            right(current_shape, board);
        }

        if (IsKeyDown(KEY_RIGHT)) {
            if (initial_press_time_right * 1000 > initial_movement_delay) {
                if (subsequent_press_time_right * 1000 > side_move_rate) {
                    subsequent_press_time_right = 0;
                    right(current_shape, board);
                }
            }
        }

        if (IsKeyUp(KEY_RIGHT)) {
            initial_press_time_right = 0;
            subsequent_press_time_right = 0;
        }

        if (IsKeyDown(KEY_DOWN)) {
            if ((movement_frame_time_sum * 1000) > down_move_rate) {
                movement_frame_time_sum = 0;
                down(current_shape, board);
            }
        }

        if (IsKeyPressed(KEY_UP)) {
            rotate_clockwise(current_shape, board);
        }

        gravity_frame_time_sum += GetFrameTime();
        if ((gravity_frame_time_sum * 1000) > gravity_rate) {
            print_shape(current_shape);
            gravity_frame_time_sum = 0;
            down(current_shape, board);
        }


        int row_count = 0;
        // draw board
        for (int i = 0; i < board->h; i++) {
            for (int j = 0; j < board->w; j++) {

                Rectangle rect = {(float) (j * rect_size), (float) (i * rect_size), (float) (rect_size),
                                  (float) (rect_size)};
                if (board->arr[i][j] == 1) { // alive
                    DrawRectangleRec(rect, current_shape->color);
                } else if (board->arr[i][j] == 0) { // empty
                    DrawRectangleRec(rect, BLACK);
                } else if (board->arr[i][j] == 2) { // dead
                    DrawRectangleRec(rect, GRAY);
                    row_count++;
                } else if (board->arr[i][j] == 3) { // shadow
                    DrawRectangleRec(rect, LIGHTGRAY);
                }
                DrawRectangleLinesEx(rect, 0.5f, WHITE);
            }

            if (row_count == board->w) {
                remove_row(i, board);
                printf("removed row %d\n", i);
            }
            row_count = 0;
        }
        BeginDrawing();

        ClearBackground(BLACK);

        DrawText("welcome to tetris !!", screenWidth / 2 + 20, 200, 40, WHITE);

        EndDrawing();
    }

    CloseWindow();

    // =============================================================================================

    free_board(board);
    free_shape(current_shape);

    return 0;
}
