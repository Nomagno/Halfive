#Copyright Nomagno 2021, 2022

#Redistributions, modified or unmodified, in whole or in part, must retain
#applicable notices of copyright or other legal privilege, these conditions, and
#the following license terms and disclaimer.  Subject to these conditions, each
#holder of copyright or other legal privileges, author or assembler, and
#contributor of this work, henceforth "licensor", hereby grants to any person
#who obtains a copy of this work in any form:

#1. Permission to reproduce, modify, distribute, publish, sell, sublicense, use,
#and/or otherwise deal in the licensed material without restriction.

#2. A perpetual, worldwide, non-exclusive, royalty-free, gratis, irrevocable
#patent license to make, have made, provide, transfer, import, use, and/or
#otherwise deal in the licensed material without restriction, for any and all
#patents held by such licensor and necessarily infringed by the form of the work
#upon distribution of that licensor's contribution to the work under the terms
#of this license.

#NO WARRANTY OF ANY KIND IS IMPLIED BY, OR SHOULD BE INFERRED FROM, THIS LICENSE
#OR THE ACT OF DISTRIBUTION UNDER THE TERMS OF THIS LICENSE, INCLUDING BUT NOT
#LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE,
#AND NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS, ASSEMBLERS, OR HOLDERS OF
#COPYRIGHT OR OTHER LEGAL PRIVILEGE BE LIABLE FOR ANY CLAIM, DAMAGES, OR OTHER
#LIABILITY, WHETHER IN ACTION OF CONTRACT, TORT, OR OTHERWISE ARISING FROM, OUT
#OF, OR IN CONNECTION WITH THE WORK OR THE USE OF OR OTHER DEALINGS IN THE
#WORK.



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
