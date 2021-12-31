#include <stdint.h>
#include <stddef.h>

/*Half-Workd VIsual interface*/

/*Pixels are RGBA 16-bit 5551 format:
00000 00000 00000 0
RED   GREEN BLUE  ALPHA
*/
typedef struct {
	const size_t w;
	const size_t h;
	uint16_t **pix;
} hwvi_pixbuf;

enum hwvi_key {
	b1, b2, b3, 
	b4, b5, b6,
	b7, b8, b9,
	m1, m2, quit
};

typedef struct {
	enum hwvi_key keys[5];
	uint8_t axis[4];
	uint16_t cursor_x;
	uint16_t cursor_y;
} hwvi_input;

typedef struct {
	int length;
	char name[16];
} hwvi_sound;

typedef struct {
	/*
	Platform string examples:
	X11, WAYLAND, WIN32, 
	MACOS, ANDROID, SDL
	*/
	char platform[16];
	void *data;
} hwvi_ref;

extern int hwvi_init(hwvi_ref *ref, size_t h, size_t w, hwvi_pixbuf *inbuf);
extern int hwvi_setbuf(hwvi_ref *surf, const hwvi_pixbuf *inbuf);
extern int hwvi_playsound(hwvi_ref *stream, _Bool do_block, const hwvi_sound *sound);
extern int hwvi_getinput(hwvi_ref *tty, const hwvi_input *keys);
