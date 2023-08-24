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

void H5Block_addRow(H5Block_Playfield *field, unsigned row, _Bool newRow[restrict PLAY_W]){
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
