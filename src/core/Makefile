#Available macros (not setting any of these makes for a headless/nolibc build):
#HWVI_GSERV_IMPL_FOO - Enable HWVI graphics server implementation foo
#HWVI_AUDIOSERV_IMPL_FOO - Enable HWVI audio server implementation foo
#HWVI_STDINPUT_IMPL_FOO - Enable HWVI input server implementation foo
#HWASSEMBLY - Enable entry point for assembler
#HWVM_OU_HEX - Enable HWVM output register as hexadecimal
#HWVM_OU_RAW - Enable printing HWVM output register as raw characters
#HWVM_OU_DEBUG - Enable HWVM debug printing
#HALFIVE_VM_SIMULATION - Most memory consuming option, enable doing per-player HWVM simulation.

INCLROOT="../../include/"

PARAMS="-DHWVERSION=0 -DHWVM_OU_HEX -DHALFIVE_VM_SIMULATION -DHWVI_GSERV_IMPL_SDL2 -DHWVI_AUDIOSERV_IMPL_SDL2 -DHWVI_STDINPUT_IMPL_PORTABLE"

build: warn libhalfworld.a
rebuild: clean build
clean:
	rm *.o *.a

warn:
	echo "This program makes extensive used of C preprocessor macros. You may use the variable PARAMS=\"-DMYVAR1 -DMYVAR2\" like so to set them"
libhalfworld.a : hwstring.o hwasm.o hwvm.o hwdoc.o
	ar rcs libhalfworld.a hwstring.o hwasm.o hwvm.o hwdoc.o
hwasm.o : hwvm.o
	cc $(PARAMS) -I$(INCLROOT) -c -o hwasm.o hwvm/hwasm.c
hwvm.o : hwstring.o
	cc $(PARAMS) -I$(INCLROOT) -c -o hwvm.o hwvm/hwvm.c
hwdoc.o : hwstring.o
	cc $(PARAMS) -I$(INCLROOT) -c -o hwdoc.o hwdoc/hwdoc.c
hwstring.o :
	cc $(PARAMS) -I$(INCLROOT) -c -o hwstring.o hwstring.c
