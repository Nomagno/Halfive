/*
Copyright Nomagno 2023

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice (including the next
paragraph) shall be included in all copies or substantial portions of the
Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
IN THE SOFTWARE.
*/

#include <halfive/h5req.h>
#include <halfive/h5render.h>
#include <halfive/h5coord.h>

#ifndef H5BLOCK_H
#define H5BLOCK_H

#define SHAPE_SIZE 4
#define LIST_SIZE 7
#define PLAY_W 10
#define PLAY_H 25

#define DEFAULT_X 4
#define DEFAULT_Y 21

#define ELIMINATION_HEIGHT 21


typedef struct {
	_Bool exists;
	/*Positions relative to shape center*/
	signed horizontal; /*negative left, positive right*/
	signed vertical; /*negative down, positive up*/
} H5Block_Block;

typedef struct {
	h5uint color;
	H5Block_Block blocks[SHAPE_SIZE];
} H5Block_Shape;


typedef struct {
	h5uint data[PLAY_H][PLAY_W];
} H5Block_Playfield;

typedef struct {
	_Bool data[2][4];
} H5Block_Piecefield;

typedef struct {
	_Bool gameOver;
	unsigned level;
	unsigned gravity_strength;
	/*Purely informative. Handled by the platform's timing system*/
	unsigned combo;
	unsigned lastLinesCleared;
	unsigned placed_pieces;
	long unsigned score;
	H5Block_Shape list[LIST_SIZE];
	H5Block_Piecefield piecefield[LIST_SIZE];
	unsigned next_shapes[LIST_SIZE*2];

	_Bool holdConsumed;
	unsigned hold_slot;

	unsigned currShape;
	/*0 normal, 1 clockwise, 2 flipped, 3 counterclockwise*/
	unsigned currRot;
	signed currX;
	signed currY;
	H5Block_Playfield playfield;
} H5Block_Game;

enum H5Block_Action {
	Action_None,
	Action_Left,
	Action_Right,
	Action_Down,
	Action_RotateClockwise,
	Action_RotateCounterClockwise,
	Action_Hold,
};

typedef struct {
	h5uint data[PLAY_H][PLAY_W];
} H5Block_playfieldVisual;

/*userdata struct of H5Coordinate_GraphicalEventData*/
struct H5Block_EventData {
	H5Block_Game *game;
	H5Block_playfieldVisual *pV;
	uint32_t *seed;
	_Bool *is_paused;
	_Bool *quit;
};

int H5Block_simulateOneFrame(H5Coordinate_GraphicalEventData *opaque_handle);

#endif
