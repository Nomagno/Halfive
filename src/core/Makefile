#Copyright Nomagno 2021, 2022, 2023
#
#Permission is hereby granted, free of charge, to any person obtaining
#a copy of this software and associated documentation files (the "Software"),
#to deal in the Software without restriction, including without limitation
#the rights to use, copy, modify, merge, publish, distribute, sublicense,
#and/or sell copies of the Software, and to permit persons to whom the
#Software is furnished to do so, subject to the following conditions:
#
#The above copyright notice and this permission notice (including the next
#paragraph) shall be included in all copies or substantial portions of the
#Software.
#
#THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
#THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
#FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
#IN THE SOFTWARE.




# Main available macros (not setting any of these makes for a headless/nolibc build):
# H5VI_GSERV_IMPL_FOO - Enable H5VI graphics server implementation foo
# H5VI_AUDIOSERV_IMPL_FOO - Enable H5VI audio server implementation foo
# H5VI_STDINPUT_IMPL_FOO - Enable H5VI input server implementation foo
# H5ASSEMBLY - Enable entry point for assembler
# H5PHY_VM_SIMULATION - Most memory consuming option, enable doing per-player H5VM simulation.
# FLOATS_SUPPORTED - Are floats permissible on the target platform?

INCLROOT="../../include/"

FLAGS=-O3 -fsanitize=address,undefined -g
# FLAGS=-O3 -lm

PARAMS=-DH5VERSION=0 -DH5PHY_VM_SIMULATION -DH5VI_GSERV_IMPL_SDL2 -DH5VI_STDINPUT_IMPL_SDL2 -DTOUCH_SUPPORT

main: warn libhalfive.a
block_emscripten_bin: warn block_emscripten
block_sdl2_bin: warn block_sdl2
sim_sdl2_bin: warn sim_sdl2
gear_sdl2_bin: warn gear_sdl2
pamview_bin: warn pamviewer
texttest_bin: warn texttester
h5asm_bin: warn h5assembler
h5asm_debug_bin: warn h5assembler_debug
remain: clean main
reblock_emscripten_bin: clean block_emscripten
reblock_sdl2_bin: clean block_sdl2
resim_sdl2_bin: clean sim_sdl2
regear_sdl2_bin: clean gear_sdl2
repamview_bin: clean pamviewer
retexttest_bin: clean texttester
reh5asm: clean h5assembler
reh5asm_debug: clean h5assembler_debug

cleand:
	rm -f *.gcov *.gcno *.gcda
clean:
	rm -f *.o *.a h5asm h5asm_debug pamview texttest gear sim block halfive_shell.html index.js index.wasm
clean_visual_shell:
	read -p "Delete path '../../docs/public_misc/*' (TAKE NOTE OF REGEX!)? ENTER TO CONFIRM! " hehe && rm -rf ../../docs/public_misc/*


warn:
	echo "This program makes extensive used of C preprocessor macros. You may use the variable PARAMS=\"-DMYVAR1 -DMYVAR2\" like so to set them"

libhalfive.a : h5stdlib.o h5asm.o h5vm.o h5doc.o h5math.o h5rat.o h5render.o h5pix.o
	ar rcs libhalfive.a h5stdlib.o h5asm.o h5vm.o h5doc.o h5math.o h5rat.o h5render.o h5pix.o

block_emscripten:
	emcc $(FLAGS) $(PARAMS) -I$(INCLROOT) -DTOUCH_SUPPORT -DIMPL_EMSCRIPTEN -s WASM=1 -s USE_SDL=2 --embed-file ../../assets/sprites/ -o index.js h5block_frontend.c h5block.c h5vi.c h5math.c h5render.c h5pix.c h5stdlib.c; mkdir -p ../../docs/public_misc/block; cp ../../docs/emscripten_template/halfive_shell.html index.js index.wasm ../../docs/public_misc/block/
block_sdl2 : h5render.o h5stdlib.o h5math.o h5vi.o h5block.o h5pix.o
	cc $(FLAGS) $(PARAMS) -I$(INCLROOT) -o block h5block_frontend.c h5block.o h5vi.o h5math.o h5render.o h5pix.o h5stdlib.o -lSDL2 -lm
sim_sdl2 : h5render.o h5stdlib.o h5math.o
	cc $(FLAGS) $(PARAMS) -I$(INCLROOT) -DH5VI_GSERV_IMPL_SDL2 -DH5VI_STDINPUT_IMPL_SDL2 -o sim h5sim.c h5vi.c h5render.o h5math.o h5stdlib.o -lSDL2 -lm
gear_sdl2 : h5render.o h5stdlib.o h5vm_gear.o h5math.o h5asm_gear.o
	cc $(FLAGS) $(PARAMS) -I$(INCLROOT) -DH5VI_GSERV_IMPL_SDL2 -DH5VI_STDINPUT_IMPL_SDL2 -o gear h5gear.c h5vi.o h5render.o h5math.o h5stdlib.o h5vm_gear.o h5asm_gear.o -lSDL2 -lm
pamviewer : h5pix.o h5render.o h5math.o h5stdlib.o
	cc $(FLAGS) $(PARAMS) -I$(INCLROOT) -DH5VI_IMAGE_VIEWER -o pamview h5vi.c h5pix.o h5render.o h5math.o h5stdlib.o -lSDL2 -lm
texttester : h5pix.o h5render.o h5math.o h5stdlib.o
	cc $(FLAGS) $(PARAMS) -I$(INCLROOT) -DH5VI_STRING_VIEWER -o texttest h5vi.c h5pix.o h5render.o h5math.o h5stdlib.o -lSDL2 -lm
h5assembler : h5assembler.o h5vm.o h5stdlib.o
	cc $(FLAGS) $(PARAMS) -I$(INCLROOT) h5assembler.o h5vm.o h5stdlib.o -o h5asm
h5assembler.o : h5vm.o
	cc $(FLAGS) $(PARAMS) -I$(INCLROOT) -DH5ASSEMBLY -c h5vm/h5asm.c -o h5assembler.o 

h5assembler_debug : h5assembler_debug.o h5vm.o h5stdlib.o
	cc $(FLAGS) $(PARAMS) -I$(INCLROOT) -o h5asm_debug h5assembler_debug.o h5vm.o h5stdlib.o 
h5assembler_debug.o : h5vm.o
	cc $(FLAGS) $(PARAMS) -I$(INCLROOT) -DH5ASSEMBLY -DH5ASM_VERBOSE -c -o h5assembler_debug.o h5vm/h5asm.c 

h5vi.o : h5render.o h5pix.o
	cc $(FLAGS) $(PARAMS) -I$(INCLROOT) -DH5VI_GSERV_IMPL_SDL2 -DH5VI_STDINPUT_IMPL_SDL2 -c h5vi.c -o h5vi.o
h5block.o : h5render.o
	cc $(FLAGS) $(PARAMS) -I$(INCLROOT) -c h5block.c -o h5block.o 
h5pix.o : h5render.o h5math.o
	cc $(FLAGS) $(PARAMS) -I$(INCLROOT) -c h5pix.c -o h5pix.o 
h5render.o : h5math.o
	cc $(FLAGS) $(PARAMS) -I$(INCLROOT) -c h5render.c -o h5render.o 
h5asm_gear.o : h5stdlib.o
	cc $(FLAGS) $(PARAMS) -I$(INCLROOT) -c h5vm/h5asm_gear.c -o h5asm_gear.o 
h5asm.o : h5vm.o
	cc $(FLAGS) $(PARAMS) -I$(INCLROOT) -c h5vm/h5asm.c -o h5asm.o 
h5vm_gear.o : h5stdlib.o
	cc $(FLAGS) $(PARAMS) -I$(INCLROOT) -c h5vm/h5vm_gear.c -o h5vm_gear.o 
h5vm.o : h5stdlib.o
	cc $(FLAGS) $(PARAMS) -I$(INCLROOT) -c h5vm/h5vm.c -o h5vm.o 
h5doc.o : h5stdlib.o
	cc $(FLAGS) $(PARAMS) -I$(INCLROOT) -c h5doc/h5doc.c -o h5doc.o 
h5stdlib.o :
	cc $(FLAGS) $(PARAMS) -I$(INCLROOT) -c h5stdlib.c -o h5stdlib.o 
h5math.o :
	cc $(FLAGS) $(PARAMS) -I$(INCLROOT) -c h5math.c -o h5math.o 
h5rat.o :
	cc $(FLAGS) $(PARAMS) -I$(INCLROOT) -c h5rat.c -o h5rat.o 
