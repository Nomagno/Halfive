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
#include <halfive/h5block.h>

#include <emscripten.h>

/*Xorshift variant*/
#include <stdint.h>
uint32_t prng(uint32_t x) {
	unsigned A = 5;
	unsigned B = 3;
	unsigned C = 21;

	x ^= x << A;
	x ^= x >> B;
	x ^= x << C;
	return x;
}

uint32_t shuffleList(unsigned *arr, unsigned n, uint32_t seed) {
	seed = prng(seed);
	for (signed i = n-1; i > 0; i--) {
		unsigned j = (unsigned)(((float)(seed % 1000)/(float)1000) * ((float)(i+1)));
		unsigned tmp = arr[i];
		arr[i] = arr[j];
		arr[j] = tmp;
	}
	return seed;
}

uint32_t H5Block_genShapes(H5Block_Game *game, uint32_t seed, _Bool initial) {
	if (initial) {
		for (int i = 0; i < LIST_SIZE; i++) {
			game->next_shapes[i] = i+1;
		}
		seed = shuffleList(game->next_shapes, LIST_SIZE, seed);

		for (int i = 0; i < LIST_SIZE; i++) {
			game->next_shapes[i+LIST_SIZE] = i+1;
		}
		seed = shuffleList((&game->next_shapes[0])+LIST_SIZE, LIST_SIZE, seed);

		return seed;
	} else {
		_Bool genNewBag = 1;
		for (int i = 0; i < LIST_SIZE; i++) {
			if (game->next_shapes[i+LIST_SIZE] != 0) {
				genNewBag = 0;
			}
		}
		if (genNewBag) {
			for (int i = 0; i < LIST_SIZE; i++) {
				game->next_shapes[i+LIST_SIZE] = i+1;
			}
			seed = shuffleList((&game->next_shapes[0])+LIST_SIZE, LIST_SIZE, seed);
		}
		return seed;
	}
}

H5Block_Shape H5Block_getRotatedShape(H5Block_Shape x, unsigned rot) {
	if (rot == 0) return x;

	H5Block_Shape retval = {0};
	retval.color = x.color;

	for (int i = 0; i < SHAPE_SIZE; i++){
		retval.blocks[i].exists = 1;
		if (rot == 2) {
			retval.blocks[i].horizontal = -x.blocks[i].horizontal;
			retval.blocks[i].vertical = -x.blocks[i].vertical;
		} else if (rot == 1) {
			retval.blocks[i].horizontal = x.blocks[i].vertical;
			retval.blocks[i].vertical = -x.blocks[i].horizontal;
		} else if (rot == 3) {
			retval.blocks[i].horizontal = -x.blocks[i].vertical;
			retval.blocks[i].vertical = x.blocks[i].horizontal;
		}
	}
	return retval;
}

_Bool H5Block_checkMove(H5Block_Playfield *p, H5Block_Shape a, signed x, signed y) {
	for (int i = 0; i < SHAPE_SIZE; i++) {
		signed finalX = a.blocks[i].horizontal + x;
		signed finalY = a.blocks[i].vertical + y;
		if (finalX < 0 || finalX >= PLAY_W ||
		  finalY < 0 || finalY >= PLAY_H) {
			return 0; /*Out of bounds move*/
		}
		if (p->data[finalY][finalX] != 0)  {
			return 0; /*There is already a block*/
		}
	}
	return 1;
}

void H5Block_removeRow(H5Block_Playfield *field, unsigned row){
	H5Block_Playfield newField = {0};
	for (unsigned i = 0; i < PLAY_H; i++) {
		if (i >= row) {
			if (i >= PLAY_H-1) {
				for (unsigned j = 0; j < PLAY_W; j++) {
					newField.data[i][j] = 0;
				}
			} else {
				for (unsigned j = 0; j < PLAY_W; j++) {
					newField.data[i][j] = field->data[i+1][j];
				}
			}
		} else {
			for (unsigned j = 0; j < PLAY_W; j++) {
				newField.data[i][j] = field->data[i][j];
			}
		}
	}
	*field = newField;
}

void H5Block_addRow(H5Block_Playfield *field, unsigned row,
	_Bool newRow[restrict PLAY_W]){
	H5Block_Playfield newField = {0};
	for (unsigned i = 0; i < PLAY_H; i++) {
		if (i > row) {
			for (unsigned j = 0; j < PLAY_W; j++) {
				newField.data[i][j] = field->data[i-1][j];
			}
		} else if (i == row) {
			for (unsigned j = 0; j < PLAY_W; j++) {
				newField.data[i][j] = newRow[j];
			}
		}
	}
	*field = newField;
}

unsigned H5Block_updatePlayfield(H5Block_Playfield *field){
	unsigned linesCleared = 0;
	for (signed i = PLAY_H-1; i >= 0; i--) {
		_Bool clearcheck = 1;
		for (unsigned j = 0; j < PLAY_W; j++) {
			clearcheck &= !!(field->data[i][j]);
		}
		if (clearcheck == 1) {
			/*Playfield to be shifted down at row i*/
			H5Block_removeRow(field, i);
			linesCleared += 1;
		}
	}
	return linesCleared;
}

unsigned H5Block_placeShape(H5Block_Playfield *p, H5Block_Shape a, signed x, signed y) {
	unsigned linesCleared = 0;
	for (int i = 0; i < SHAPE_SIZE; i++) {
		signed finalX = a.blocks[i].horizontal + x;
		signed finalY = a.blocks[i].vertical + y;
		if (finalX < 0 || finalX >= PLAY_W ||
		  finalY < 0 || finalY >= PLAY_H) {
			; /*Out of bounds placement*/
		} else {
			p->data[finalY][finalX] = a.color;
		}
	}
	linesCleared = H5Block_updatePlayfield(p);
	return linesCleared;
}


void H5Block_pullShape(H5Block_Game *game) {
	unsigned next_shapes_clone[LIST_SIZE*2];
	game->currShape = game->next_shapes[0];
	for (int i = 1; i < LIST_SIZE*2; i++) {
		next_shapes_clone[i-1] = game->next_shapes[i];
	}
	next_shapes_clone[(LIST_SIZE*2)-1] = 0;
	for (int i = 0; i < (LIST_SIZE*2)-1; i++) {
		game->next_shapes[i] = next_shapes_clone[i];
	}
	game->next_shapes[(LIST_SIZE*2)-1] = 0;

}

/*EDIT THIS IF MAKING PIECES THAT DON'T FIX IN A [HEIGHT 2, WIDTH 4] ARRAY WITH CENTER AT CENTER TWO TO THE RIGHT, BOTTOMMOST ROW*/
void H5Block_populatePiecefield(H5Block_Game *game) {
	for (int k = 0; k < LIST_SIZE; k++) {
		for (int i = 0; i < SHAPE_SIZE; i++) {
			unsigned blockX = game->list[k].blocks[i].horizontal + 2;
			unsigned blockY = game->list[k].blocks[i].vertical;
			game->piecefield[k].data[blockY][blockX] = 1;
		}
	}
}

uint32_t H5Block_doGameplayLoop(H5Block_Game *game, uint32_t seed, enum H5Block_Action action) {
	H5Block_Shape finalShape = H5Block_getRotatedShape(game->list[game->currShape-1], game->currRot);

	unsigned linesCleared = 0;
	unsigned newRot = 0;
	switch (action) {
	case Action_Left:
		if (H5Block_checkMove(&game->playfield, finalShape, game->currX-1, game->currY)) {
			game->currX -= 1;
		}
		break;
	case Action_Right:
		if (H5Block_checkMove(&game->playfield, finalShape, game->currX+1, game->currY)) {
			game->currX += 1;
		}
		break;
	case Action_Down:
		if (H5Block_checkMove(&game->playfield, finalShape, game->currX, game->currY-1)) {
			game->currY -= 1;
		} else {
			linesCleared = H5Block_placeShape(&game->playfield, finalShape, game->currX, game->currY);
			if (linesCleared == 0) {
				game->combo = 0;
			} else {
				;
			}
			game->placed_pieces += 1;
			game->holdConsumed = 0;
			game->currRot = 0;
			game->currX = DEFAULT_X;
			game->currY = DEFAULT_Y;
			H5Block_pullShape(game);
			_Bool gameOver = !H5Block_checkMove(&game->playfield, game->list[game->currShape-1], game->currX, game->currY);
			if (gameOver) {
				game->gameOver = 1;
			};
			seed = H5Block_genShapes(game, seed, 0);
		}
		break;
	case Action_RotateClockwise:
		newRot = (game->currRot == 3) ? 0 : (game->currRot+1);
		finalShape = H5Block_getRotatedShape(game->list[game->currShape-1], newRot);
		if (H5Block_checkMove(&game->playfield, finalShape, game->currX, game->currY)) {
			game->currRot = newRot;
		}
		break;
	case Action_RotateCounterClockwise:
		newRot = (game->currRot == 0) ? 3 : (game->currRot-1);
		finalShape = H5Block_getRotatedShape(game->list[game->currShape-1], newRot);
		if (H5Block_checkMove(&game->playfield, finalShape, game->currX, game->currY)) {
			game->currRot = newRot;
		}
		break;
	case Action_Hold: ;
		if (game->hold_slot != 0 && game->holdConsumed == 0) {
			game->holdConsumed = 1;
			unsigned tmp = game->hold_slot;
			game->hold_slot = game->currShape;
			game->currShape = tmp;
			game->currRot = 0;
			game->currX = DEFAULT_X;
			game->currY = DEFAULT_Y;
		} else if (game->holdConsumed == 0) {
			game->holdConsumed = 1;
			game->hold_slot = game->currShape;
			game->currRot = 0;
			game->currX = DEFAULT_X;
			game->currY = DEFAULT_Y;			
			H5Block_pullShape(game);
		}
		break;
	case Action_None:
	default:
		break;
	}

	switch (linesCleared) {
	case 0:
		break;
	case 1:
		game->score += game->lastLinesCleared*game->combo*5 + 10;
		game->combo += 1;
		break;
	case 2:
		game->score += game->lastLinesCleared*game->combo*10 + 20;
		game->combo += 1;
		break;
	case 3:
		game->score += game->lastLinesCleared*game->combo*20 + 60;
		game->combo += 1;
		break;
	case 4:
		if (game->lastLinesCleared == 4) {
			game->score += (game->combo-1)*160 + 1000;
		} else {
			game->score += game->lastLinesCleared*game->combo*30 + 240;
			game->combo += 1;
		}
		break;
	default:
		break;
	}
	return seed;
}

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

#include <halfive/code_setup.h>
#include <time.h>
#include <stdio.h>

char getShapeName(unsigned shape) {
	switch(shape) {
	case 1:
		return 'S';
	case 2:
		return 'Z';
	case 3:
		return 'J';
	case 4:
		return 'L';
	case 5:
		return 'I';
	case 6:
		return 'O';
	case 7:
		return 'T';
	default:
		return '_';
	}
}

void H5Block_printToCommandLine_auxiliaryData(H5Block_Game *game) {
		maybe_printf("COMBO: %u\n", game->combo);
		maybe_printf("SCORE: %lu\n", game->score);
		maybe_printf("CURRENT PIECE: %c\n", getShapeName(game->currShape));
		maybe_printf("HOLD SLOT: %c\n", getShapeName(game->hold_slot));
		maybe_printf("BAG:");
		for (int i = 0; i < LIST_SIZE; i++) {
			maybe_printf(" %c", getShapeName(game->next_shapes[i]));
		};
		maybe_putchar('\n');
}

void H5Block_getVisualRepresentationOfField(H5Block_Game *game, H5Block_playfieldVisual *pV){
	*pV = (H5Block_playfieldVisual){0};
	for (int i = 0; i < PLAY_H; i++) {
		for (int j = 0; j < PLAY_W; j++) {
			if (game->playfield.data[i][j] != 0) {
				pV->data[i][j] = game->playfield.data[i][j];
			} else {
				pV->data[i][j] = 0;
			}
		}
	}
	H5Block_Shape finalShape = H5Block_getRotatedShape(game->list[game->currShape-1], game->currRot);
	for (int i = 0; i < SHAPE_SIZE; i++) {
		unsigned coordX = finalShape.blocks[i].horizontal + game->currX;
		unsigned coordY = finalShape.blocks[i].vertical + game->currY;
		if (finalShape.blocks[i].exists) {
			pV->data[coordY][coordX] = finalShape.color;
		}
	}

}

#include <halfive/h5vi.h>

void H5Block_printPerformanceData(h5umax time_available, h5umax time_taken, _Bool is_paused, _Bool quit) {
	h5umax performance_percent = 100*(double)((double)time_available/(double)(time_taken));
	if (time_taken >= time_available) {
		maybe_printf("H5Vi: WARNING: Timing objective not met - Performance: %li%%\n",
		       performance_percent);
	} else {
		maybe_printf("H5Vi: Frame time: %li nanoseconds - Performance: %li%%\n",
		       (is_paused) ? 0 : time_taken, (is_paused) ? 100 : performance_percent);
	}
	if (quit) {
		maybe_printf("H5Vi: Quitting\n");
	}
}

#define WCONSTANT 256
#define HCONSTANT 256
#define OFFSET (HCONSTANT/3)

void H5Block_RenderAt(H5Render_PixelData surf, unsigned side, signed x, signed y, unsigned offset, h5uint colour) {
	VEC2(h5ulong) p1 = { offset+x*side, (surf.height-1)-(y*side) };
	VEC2(h5ulong) p2 = { p1.x+side, p1.y };
	VEC2(h5ulong) p3 = { p1.x+side, p1.y-side };
	VEC2(h5ulong) p4 = { p1.x, p1.y-side };
	H5Render_ulong_drawRectangle(surf, p1, p2, p3, p4, colour);
}

void H5Block_Render(H5Render_PixelData surf, H5Block_playfieldVisual pV, H5Block_Game *game) {
	unsigned side = WCONSTANT/(PLAY_H+7);
	unsigned offset_x = 1;
	unsigned offset_y = 1;
	unsigned overhead_y = 3;
	
	/*Render playfield*/
	for (unsigned i = 0; i < DEFAULT_Y+overhead_y; i++) {
		for (unsigned j = 0; j < PLAY_W; j++) {
			if(pV.data[i][j] != 0) {
				H5Block_RenderAt(surf, side, j+offset_x, i+offset_y, OFFSET, pV.data[i][j]);
			} else if (pV.data[i][j] == 0){
				H5Block_RenderAt(surf, side, j+offset_x, i+offset_y, OFFSET, 0xFFFF);
			}
		}
	}

	/*Render informative elimination line*/
	for (unsigned j = 0; j < PLAY_W; j++) {
		if (pV.data[DEFAULT_Y][j] == 0) {
			H5Block_RenderAt(surf, side, j+offset_x, DEFAULT_Y+offset_y, OFFSET, 0xF80D);
		}
	}

	/*Render playfield borders*/
	for (unsigned j = 0; j < PLAY_W+2; j++) {
		H5Block_RenderAt(surf, side, j, 0, OFFSET, 0x0001);
		H5Block_RenderAt(surf, side, j, DEFAULT_Y+offset_y+overhead_y, OFFSET, 0x0001);
	}
	for (unsigned i = 0; i < DEFAULT_Y+overhead_y+2; i++) {
		H5Block_RenderAt(surf, side, 0, i, OFFSET, 0x0001);
		H5Block_RenderAt(surf, side, PLAY_W+1, i, OFFSET, 0x0001);
	}

	unsigned offset_left_h_ABSOLUTE = 5*side;
	unsigned offset_left_y = DEFAULT_Y+overhead_y-3;
	unsigned offset_right_h = PLAY_W+3;
	unsigned piecefield_w = 4;
	unsigned piecefield_h = 2;
	/*Render hold slot to the left of playfield*/
	for (unsigned i = 0; i < piecefield_h; i++) {
		for (unsigned j = 0; j < piecefield_w; j++) {
			if(game->hold_slot > 0 && game->piecefield[game->hold_slot-1].data[i][j] != 0) {
				H5Block_RenderAt(surf, side, j, i+offset_left_y, OFFSET-offset_left_h_ABSOLUTE, game->list[game->hold_slot-1].color);
			} else if (game->hold_slot <= 0 || game->piecefield[game->hold_slot-1].data[i][j] == 0){
				H5Block_RenderAt(surf, side, j, i+offset_left_y, OFFSET-offset_left_h_ABSOLUTE, 0xFFFF);
			}
		}
	}

	/*Render next 4 pieces to the right of playfield*/
	for (signed k = 3; k >= 0; k--) {
		for (unsigned i = 0; i < piecefield_h; i++) {
			for (unsigned j = 0; j < piecefield_w; j++) {
				if (game->next_shapes[k] == 0 || game->piecefield[game->next_shapes[k]-1].data[i][j] == 0) {
					H5Block_RenderAt(surf, side, j+offset_right_h, i+(k*3), OFFSET, 0xFFFF);
				} else if(game->piecefield[game->next_shapes[k]-1].data[i][j] != 0) {
					H5Block_RenderAt(surf, side, j+offset_right_h, i+(k*3), OFFSET, game->list[game->next_shapes[k]-1].color);
				}
			}
		}
	}

}


#define INITIAL_KEY_FRAMES 8
/*Game loop for Halfive Block*/
int H5Block_simulateOneFrame(H5Coordinate_GraphicalEventData *opaque_handle) {
	clock_t start_t, end_t;
	start_t = clock(); /*Microseconds in XSI-compliant systems*/

	H5Coordinate_GraphicalEventData generalData = *opaque_handle;
	struct H5Block_EventData specificData = *(struct H5Block_EventData *)(generalData.userdata);
	H5VI_InputData input_keys = generalData.input_keys;
	H5Block_Game *game = specificData.game;
	H5Block_playfieldVisual *pV = specificData.pV;
	uint32_t *seed = specificData.seed;
	enum H5Block_Action action = Action_None;

	_Bool skip_gravity = 0;

	if (input_keys.keys[H5KEY_QUIT] == 1) {
		*specificData.quit = 1;
		goto GO_BACK;
	}
	if (input_keys.keys[H5KEY_PAUSE] == 1 && *specificData.is_paused == 0) {
		*specificData.is_paused = 1;
		goto GO_BACK;
	} else if (input_keys.keys[H5KEY_PAUSE] == 1 && *specificData.is_paused == 1) {
		*specificData.is_paused = 0;
		goto GO_BACK;
	}
	if (*specificData.is_paused == 1) {
		goto GO_BACK;
	}

	/*Works on initial keypress, then has INITIAL_KEY_FRAMES frames delay, then autorepeats every AUTOREPEAT_FRAMES*/
	if (H5VI_autoRepeat(&input_keys, H5KEY_DOWN, 1, INITIAL_KEY_FRAMES, 1)) {
		action = Action_Down;
		int oldY = game->currY;
		*seed = H5Block_doGameplayLoop(game, *seed, Action_Down);
		if (game->currY > oldY) {
			goto SKIP_DIRECTIONALS;
		}
	}

	if (input_keys.keys[H5KEY_LEFT] || input_keys.keys[H5KEY_RIGHT]) {
		skip_gravity = 1;
	}
	if (input_keys.keys[H5KEY_LEFT] && input_keys.keys[H5KEY_RIGHT]) {
		/*No press delay charging!*/
		((H5Coordinate_GraphicalEventData *)(opaque_handle))->input_keys.fetch_elapsed[H5KEY_LEFT] = 0;
		((H5Coordinate_GraphicalEventData *)(opaque_handle))->input_keys.fetch_elapsed[H5KEY_RIGHT] = 0;
		goto SKIP_DIRECTIONALS;
	}
	if (H5VI_autoRepeat(&input_keys, H5KEY_LEFT, 1, INITIAL_KEY_FRAMES, 1)) {
		action = Action_Left;
		*seed = H5Block_doGameplayLoop(game, *seed, Action_Left);
	}
	if (H5VI_autoRepeat(&input_keys, H5KEY_RIGHT, 1, INITIAL_KEY_FRAMES, 1)) {
		action = Action_Right;
		*seed = H5Block_doGameplayLoop(game, *seed, Action_Right);
	}

	SKIP_DIRECTIONALS: ;
	unsigned old_placed_pieces = game->placed_pieces;
	if (H5VI_isOnPress(&input_keys, H5KEY_UP)) {
		*seed = H5Block_doGameplayLoop(game, *seed, Action_Down);
		while (old_placed_pieces == game->placed_pieces && game->gameOver == 0) {
			action = Action_Down;
			*seed = H5Block_doGameplayLoop(game, *seed, Action_Down);
		}
	}
	if (H5VI_isOnPress(&input_keys, H5KEY_B1)) {
		action = Action_RotateCounterClockwise;
		*seed = H5Block_doGameplayLoop(game, *seed, Action_RotateCounterClockwise);
	}
	if (H5VI_isOnPress(&input_keys, H5KEY_B2)) {
		action = Action_RotateClockwise;
		*seed = H5Block_doGameplayLoop(game, *seed, Action_RotateClockwise);
	}
	if (H5VI_isOnPress(&input_keys, H5KEY_B4)) {
		action = Action_Hold;
		*seed = H5Block_doGameplayLoop(game, *seed, Action_Hold);
	}

	/*GRAVITY!*/
	if (skip_gravity == 0 && (generalData.frame_counter % (30/game->gravity_strength)) == 0 && input_keys.keys[H5KEY_DOWN] == 0) {
		*seed = H5Block_doGameplayLoop(game, *seed, Action_Down);
	}


	H5Block_getVisualRepresentationOfField(game, pV);
	H5Block_Render(generalData.rendered_output, *pV, game);

	goto GO_BACK;

GO_BACK:
	end_t = clock(); /*Microseconds in XSI-compliant systems*/
	*generalData.time_taken = ((end_t-start_t)*1000);
	return 0;
}

static h5uint array_main_buf[HCONSTANT][WCONSTANT] = {0};

/*WARNING: PROGRAM DOESN'T WORK CORRECTLY IF 'CLOCKS_PER_SEC' IS NOT XSI-MANDATED 1000000*/
H5VI_Reference main_ref;
/*Halfive Block game state*/
H5Block_Game game = {
	.list = POPULAR_SHAPES,
	.currX = DEFAULT_X,
	.currY = DEFAULT_Y,
	.gravity_strength = 2
};
/*Playfield outputted by the CLI renderer*/
H5Block_playfieldVisual pV = {0};
/*Screen buffer*/
H5Render_PixelData main_buf = {HCONSTANT, WCONSTANT, .data = &array_main_buf[0][0]};
uint32_t seed;
H5Coordinate_GraphicalEventData loop_data = {
	.ref = &main_ref,
	.time_available = 33333333, /*average frame length in nanoseconds --- FPS = (1/time_available_in_seconds)*/
	.delta_time = 0, /*To-do: update this per-run*/
	.time_taken = &(h5umax){0},
	.rendered_output = {HCONSTANT, WCONSTANT, .data = &array_main_buf[0][0]},
	.const_userdata = NULL,
	.input_keys = {0},
	.frame_counter = 0,
	.userdata = &(struct H5Block_EventData){
		.game = &game,
		.pV = &pV,
		.seed = &seed,
		.is_paused = &(_Bool){0},
		.quit = &(_Bool){0}
	}
};

void infiniteLoop(void *useless) {
	_Bool old_is_paused = *(((struct H5Block_EventData *)(loop_data.userdata))->is_paused);

	for (size_t i = 0; i < ELEMNUM(loop_data.input_keys.previous_keys); i++) {
		loop_data.input_keys.previous_keys[i] = loop_data.input_keys.keys[i];
	}

	H5VI_getInput(&main_ref, &loop_data.input_keys);
	H5VI_updateDelayData(&main_ref, &loop_data.input_keys);
	H5Block_simulateOneFrame((void *)&loop_data);
	loop_data.frame_counter += 1;

	_Bool new_is_paused = *(((struct H5Block_EventData *)(loop_data.userdata))->is_paused);
	_Bool quit = *(((struct H5Block_EventData *)(loop_data.userdata))->quit);

	H5Block_printPerformanceData(loop_data.time_available, *loop_data.time_taken, new_is_paused, quit);
	H5Block_printToCommandLine_auxiliaryData(&game);

	if (old_is_paused != new_is_paused) { /*If pause state changed, sleep 500ms instead of usual amount*/
		//nanosleep(&(struct timespec){0, 500000000 }, NULL);
	} else {
		/*Copy the buffer to the screen*/
		H5VI_setBuffer(&main_ref, &main_buf);

		h5smax sleep_time = loop_data.time_available - *loop_data.time_taken;

		/*How long do we sleep? Primitive calculation for now, better
		  not have a potato computer or it'll just drop the framerate and complain*/
		//nanosleep(&(struct timespec){0, (sleep_time > 0) ? sleep_time : 1 }, NULL);
	}

	if (quit || game.gameOver) {
		maybe_printf("GAME OVER!\n");
		emscripten_cancel_main_loop();
	}
}

int main(void) {
	H5Render_fill(main_buf, 0xFFFF);
	seed = clock() % 1100;

	if (H5VI_init(&main_ref, HCONSTANT, WCONSTANT)) {
		H5VI_destroy(&main_ref);
		goto EXIT;
	}
	H5VI_setBuffer(&main_ref, &main_buf);

	H5Block_populatePiecefield(&game);
	seed = H5Block_genShapes(&game, seed, 1);
	H5Block_pullShape(&game);

	emscripten_set_main_loop_arg(infiniteLoop, NULL, 30, 1);

	EXIT:
		H5VI_destroy(&main_ref);
}
