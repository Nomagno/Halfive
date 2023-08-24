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

/*These are the one-sided tetrominoes,
  seven mathematical four-block shapes used
  for block games with relative ubiquity*/
#define POPULAR_SHAPES { \
	/*left right; down up*/ \
	{0x7701, {{1, -1, 0}, {1, 0, 0}, {1, 0, 1}, {1, 1, 1}}}, /*S*/ \
	{0xF80D, {{1, -1, 1}, {1, 0, 1}, {1, 0, 0}, {1, 1, 0}}}, /*Z*/ \
	{0x42EF, {{1, -1, 1}, {1, -1, 0}, {1, 0, 0}, {1, 1, 0}}}, /*J*/ \
	{0xFC01, {{1, -1, 0}, {1, 0, 0}, {1, 1, 0}, {1, 1, 1}}}, /*L*/ \
	{0x363F, {{1, -2, 0}, {1, -1, 0}, {1, 0, 0}, {1, 1, 0}}}, /*I*/ \
	{0xFE01, {{1, -1, 1}, {1, -1, 0}, {1, 0, 0}, {1, 0, 1}}}, /*O*/ \
	{0x8AF9, {{1, -1, 0}, {1, 0, 0}, {1, 0, 1}, {1, 1, 0}}}, /*T*/ \
}


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

uint32_t H5Block_genShapes(H5Block_Game *game, uint32_t seed, _Bool initial);
H5Block_Shape H5Block_getRotatedShape(H5Block_Shape x, unsigned rot);
_Bool H5Block_checkMove(H5Block_Playfield *p, H5Block_Shape a, signed x, signed y);
void H5Block_removeRow(H5Block_Playfield *field, unsigned row);
void H5Block_addRow(H5Block_Playfield *field, unsigned row, _Bool newRow[restrict PLAY_W]);
unsigned H5Block_updatePlayfield(H5Block_Playfield *field);
unsigned H5Block_placeShape(H5Block_Playfield *p, H5Block_Shape a, signed x, signed y);
void H5Block_pullShape(H5Block_Game *game);
void H5Block_populatePiecefield(H5Block_Game *game);
uint32_t H5Block_doGameplayLoop(H5Block_Game *game, uint32_t seed, enum H5Block_Action action);
void H5Block_getVisualRepresentationOfField(H5Block_Game *game, H5Block_playfieldVisual *pV);
int H5Block_simulateOneFrame(H5Coordinate_GraphicalEventData *opaque_handle);

#endif
