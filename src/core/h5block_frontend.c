#include <halfive/h5block.h>

#include <halfive/code_setup.h>
#include <halfive/h5stdlib.h>
#include <halfive/h5vi.h>
#include <halfive/h5pix.h>
#include <time.h>
#include <stdio.h>

#ifdef IMPL_EMSCRIPTEN
	#include <emscripten.h>
#endif

#define BACKGROUND_COLOUR 0xCBB7

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

/*retval must be a buffer at least 32 characters long*/
void utoa(h5ulong x, char *retval)
{
	char buf[30];
	unsigned counter = 0;

	if (x == 0) {
		buf[counter] = '0';
		counter += 1;
	}

	for ( ; x != 0; x = x/10) {
		buf[counter] = '0'+(x%10);
		counter += 1;
	}

	for (unsigned i = 0; i < counter; ++i)
	 	retval[i] = buf[counter - i - 1];

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

#define FONT_SCALE 2

#define WCONSTANT 640
#define HCONSTANT 360

void H5Block_RenderAt(H5Render_PixelData surf, unsigned side, signed x, signed y, unsigned offset, h5uint colour) {
	/*(surf.height-1)-(y*side) -> this takes care of the fact that in
	  H5Block the Y axis is bottom-to-top, while in H5Render it is top-to-bottom*/
	VEC2(h5ulong) p1 = { offset+x*side, (surf.height-1)-(y*side) };
	VEC2(h5ulong) p2 = { p1.x+side, p1.y };
	VEC2(h5ulong) p3 = { p1.x+side, p1.y-side };
	VEC2(h5ulong) p4 = { p1.x, p1.y-side };
	H5Render_ulong_drawRectangle(surf, p1, p2, p3, p4, colour);
}

/*
#define TEXT_SIZE(number_of_chars, scale)
#define SAFE_REDUCE_SIZE(scale)
#define RENDER_TEXT(string, x, y, scale)
*/

void H5Block_RenderTextAt(H5Render_PixelData surf, char *string, unsigned x, unsigned y) {
	unsigned i;
	char tmpstr[32] = {0};
	for (i = 0; 1; i++) {
		tmpstr[i] = string[i];
		if (string[i] == '\0') break;
	}
	while (TEXT_SIZE(i, FONT_SCALE) > surf.width) {
		if (i == 0) {
			return;
		} else {
			i -= 1;
		}
	}
	tmpstr[i] = '\0';
	RENDER_TEXT(tmpstr, x, y, FONT_SCALE, surf);
}

void H5Block_Render(H5Render_PixelData surf, H5Block_playfieldVisual pV, H5Block_Game *game) {

	/*The length of a block's side in pixels*/
	unsigned block_side = surf.height/(PLAY_H+7);

	/*How many pixels to leave on the left, this is meant to more or less center the playfield on the screen*/
	unsigned rendering_offset_horizontal = surf.width/3;

	/*This offset is fundamentally to leave space after the rendering offset, and on the bottom of the screen so that the black border can be drawn later*/
	unsigned offset_x = 1;
	unsigned offset_y = 1;

	/*The overhead is how many blocks to render above the vertical elimination and spawn height (DEFAULT_Y)*/
	unsigned overhead_y = 3;
	
	/*Render playfield*/
	for (unsigned i = 0; i < DEFAULT_Y+overhead_y; i++) {
		for (unsigned j = 0; j < PLAY_W; j++) {
			if(pV.data[i][j] != 0) {
				H5Block_RenderAt(surf, block_side, j+offset_x, i+offset_y, rendering_offset_horizontal, pV.data[i][j]);
			} else if (pV.data[i][j] == 0){
				H5Block_RenderAt(surf, block_side, j+offset_x, i+offset_y, rendering_offset_horizontal, BACKGROUND_COLOUR);
			}
		}
	}

	/*Render informative elimination line*/
	for (unsigned j = 0; j < PLAY_W; j++) {
		if (pV.data[DEFAULT_Y][j] == 0) {
			H5Block_RenderAt(surf, block_side, j+offset_x, DEFAULT_Y+offset_y, rendering_offset_horizontal, 0xB00D);
		}
	}

	/*Render playfield black borders*/
	for (unsigned j = 0; j < PLAY_W+2; j++) {
		H5Block_RenderAt(surf, block_side, j, 0, rendering_offset_horizontal, 0x0001);
		H5Block_RenderAt(surf, block_side, j, DEFAULT_Y+offset_y+overhead_y, rendering_offset_horizontal, 0x0001);
	}
	for (unsigned i = 0; i < DEFAULT_Y+overhead_y+2; i++) {
		H5Block_RenderAt(surf, block_side, 0, i, rendering_offset_horizontal, 0x0001);
		H5Block_RenderAt(surf, block_side, PLAY_W+1, i, rendering_offset_horizontal, 0x0001);
	}

	/*Rather than using rendering_offset_horizontal, we change it to be able to
	  render the hold slot piecefield at an off-center location to the left*/
	unsigned offset_hold_slot_horizontal_ABSOLUTE = 5*block_side;

	/*The height in blocks at which to render the hold slot*/
	unsigned offset_left_y = DEFAULT_Y+overhead_y-3;

	/*A piecefield is a section that holds the render of a single piece, outside the playfield*/
	unsigned piecefield_w = 4;
	unsigned piecefield_h = 2;

	/*Render hold slot to the left of playfield*/
	for (unsigned i = 0; i < piecefield_h; i++) {
		for (unsigned j = 0; j < piecefield_w; j++) {
			if(game->hold_slot > 0 && game->piecefield[game->hold_slot-1].data[i][j] != 0) {
				H5Block_RenderAt(surf, block_side, j, i+offset_left_y, rendering_offset_horizontal-offset_hold_slot_horizontal_ABSOLUTE, game->list[game->hold_slot-1].colour);
			} else if (game->hold_slot <= 0 || game->piecefield[game->hold_slot-1].data[i][j] == 0){
				H5Block_RenderAt(surf, block_side, j, i+offset_left_y, rendering_offset_horizontal-offset_hold_slot_horizontal_ABSOLUTE, BACKGROUND_COLOUR);
			}
		}
	}

	/*We can calculate this in relative blocks with no real problems. This will
	   cause out-of-bounds pixel matrix access if the aspect ratio is insane
	   though (something like 1:10 or more skewed in the height's favour).
	 It is the horizontal offset for the next 4 pieces that will come out*/
	unsigned offset_next_pieces_horizontal = PLAY_W+3;
	unsigned offset_next_pieces_vertical = DEFAULT_Y+overhead_y-(4*2)-1;

	/*Render next 4 pieces to the right of playfield, same process as for the hold slot*/
	for (signed k = 0; k < 4; k++) {
		for (unsigned i = 0; i < piecefield_h; i++) {
			for (unsigned j = 0; j < piecefield_w; j++) {
				if (game->next_shapes[3-k] == 0 || game->piecefield[game->next_shapes[3-k]-1].data[i][j] == 0) {
					H5Block_RenderAt(surf, block_side, j+offset_next_pieces_horizontal, offset_next_pieces_vertical+i+(k*3), rendering_offset_horizontal, BACKGROUND_COLOUR);
				} else if(game->piecefield[game->next_shapes[3-k]-1].data[i][j] != 0) {
					H5Block_RenderAt(surf, block_side, j+offset_next_pieces_horizontal, offset_next_pieces_vertical+i+(k*3), rendering_offset_horizontal, game->list[game->next_shapes[3-k]-1].colour);
				}
			}
		}
	}

	H5Block_RenderTextAt(surf, "score", rendering_offset_horizontal, 4*block_side);

	char storageOfScore[32] = {0};
	utoa(game->score, storageOfScore);
	H5Block_RenderTextAt(surf, storageOfScore, TEXT_SIZE(7, FONT_SCALE)+rendering_offset_horizontal, 4*block_side);

	H5Block_RenderTextAt(surf, "halfive block", rendering_offset_horizontal, 1*block_side);

}


#define INITIAL_KEY_FRAMES 4
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
		/*skip_gravity = 1;*/
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
	#ifdef TOUCH_SUPPORT
		H5VI_renderVirtualButtons(generalData.rendered_output, &input_keys, 0x4325);
	#endif

	goto GO_BACK;

GO_BACK:
	action = action;
	end_t = clock(); /*Microseconds in XSI-compliant systems*/
	*generalData.time_taken = ((end_t-start_t)*1000);
	return 0;
}

uint32_t seed = 43823754;
static h5uint array_main_buf[HCONSTANT][WCONSTANT] = {0};
H5VI_Reference main_ref;
H5Render_PixelData main_buf = {HCONSTANT, WCONSTANT, .data = &array_main_buf[0][0]};
/*Halfive Glock game state*/
H5Block_Game game = {
	.list = POPULAR_SHAPES,
	.currX = DEFAULT_X,
	.currY = DEFAULT_Y,
	.gravity_strength = 2
};
H5Block_playfieldVisual pV = {0};

H5Coordinate_GraphicalEventData loop_data = {
	.ref = &main_ref,
	.time_available = 33333333, /*average frame length in nanoseconds --- FPS = (1/time_available_in_seconds)*/
	.delta_time = 0, /*To-do: update this per-run*/
	.time_taken = &(h5umax){0},
	.rendered_output = {HCONSTANT, WCONSTANT, .data = &array_main_buf[0][0]},
	.const_userdata = NULL,
	.input_keys = {.view_height = HCONSTANT, .view_width = WCONSTANT},
	.frame_counter = 0,
	.userdata = &(struct H5Block_EventData){
		.game = &game,
		.pV = &pV,
		.seed = &seed,
		.is_paused = &(_Bool){0},
		.quit = &(_Bool){0}
	}
};

_Bool quitForGood = 0;

void mainGameLoop(void *useless) {
	_Bool old_is_paused = *(((struct H5Block_EventData *)(loop_data.userdata))->is_paused);

	H5VI_getInput(&main_ref, &loop_data.input_keys);
	H5Block_simulateOneFrame((void *)&loop_data);
	loop_data.frame_counter += 1;

	_Bool new_is_paused = *(((struct H5Block_EventData *)(loop_data.userdata))->is_paused);
	_Bool quit = *(((struct H5Block_EventData *)(loop_data.userdata))->quit);

	/*Emscripten has unplayable performance degradation over time if there is
	  even one printf per frame. That issue took way too long to figure out...*/
	#if !defined(IMPL_EMSCRIPTEN)
		H5Block_printPerformanceData(loop_data.time_available, *loop_data.time_taken, new_is_paused, quit);
		H5Block_printToCommandLine_auxiliaryData(&game);
	#endif

	if (old_is_paused != new_is_paused) { /*If pause state changed, sleep 500ms instead of usual amount*/
		nanosleep(&(struct timespec){0, 500000000 }, NULL);
	} else {
		/*Copy the buffer to the screen*/
		H5VI_setBuffer(&main_ref, &main_buf);

	/*Emscripten handles the timing automatically*/
	#if !defined(IMPL_EMSCRIPTEN)
		h5smax sleep_time = loop_data.time_available - *loop_data.time_taken;

		/*How long do we sleep? Primitive calculation for now, better
		  not have a potato computer or it'll just drop the framerate and complain*/
		nanosleep(&(struct timespec){0, (sleep_time > 0) ? sleep_time : 1 }, NULL);
	#endif
	}

	if (quit || game.gameOver) {
		maybe_printf("GAME OVER!\n");
		#ifdef IMPL_EMSCRIPTEN
			emscripten_cancel_main_loop();
		#else
			quitForGood = 1;
			return;
		#endif
	}
}

#define PREPEND_PATH "../../assets/sprites/fonts/"
int main(void) {
	/*WARNING: PROGRAM DOESN'T WORK CORRECTLY IF 'CLOCKS_PER_SEC' IS NOT XSI-MANDATED 1000000*/
	INIT_FONT_MAIN("8x8", FONT_SCALE);
	global_padding = 2;
	
	H5Render_fill(main_buf, BACKGROUND_COLOUR);

	if (H5VI_init(&main_ref, HCONSTANT, WCONSTANT)) {
		H5VI_destroy(&main_ref);
		goto EXIT;
	}
	H5VI_setBuffer(&main_ref, &main_buf);

	#ifdef TOUCH_SUPPORT
		H5VI_defaultVirtualLayout(&main_ref, &loop_data.input_keys);
	#endif

	seed = clock() % 1100;

	H5Block_populatePiecefield(&game);
	seed = H5Block_genShapes(&game, seed, 1);
	H5Block_pullShape(&game);

	/*NOTE: this is a basic reference for how to implement H5Coord's main-loop
	  scheduler that makes use of an event queue and can adapt to variable performance*/
	/* To learn how and where you should hack if you wish to run H5Coord and H5VI
	   in a cooperative multitasking environment, see the emscripten ifdefs and 
	   take note of how the main game loop is its own function that works fine so long
	   as it has the right approximate timing.
	   Minor TODO: Make the timing for the native version be outside mainGameLoop, rather than inside.*/
	#ifdef IMPL_EMSCRIPTEN
		emscripten_set_main_loop_arg(mainGameLoop, NULL, 30, 1);
	#else
		while (1) {
			mainGameLoop(NULL);
			if (quitForGood == 1) {
				goto EXIT;
			}
		}
	#endif


	EXIT:
		H5VI_destroy(&main_ref);
}
