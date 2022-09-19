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
#H5VI_GSERV_IMPL_FOO - Enable H5VI graphics server implementation foo
#H5VI_AUDIOSERV_IMPL_FOO - Enable H5VI audio server implementation foo
#H5VI_STDINPUT_IMPL_FOO - Enable H5VI input server implementation foo
#H5ASSEMBLY - Enable entry point for assembler
#H5PHY_VM_SIMULATION - Most memory consuming option, enable doing per-player H5VM simulation.
#FLOATS_SUPPORTED - Are floats permissible on the target platform?

INCLROOT="../../include/"

PARAMS="-DH5VERSION=0 -DH5PHY_VM_SIMULATION -DH5VI_GSERV_IMPL_SDL2 -DH5VI_AUDIOSERV_IMPL_SDL2 -DH5VI_STDINPUT_IMPL_PORTABLE"

main: warn libhalfive.a
h5asm: warn h5assembler
remain: clean main
reh5asm: clean h5assembler
clean:
	rm -f *.o *.a h5assembler

warn:
	echo "This program makes extensive used of C preprocessor macros. You may use the variable PARAMS=\"-DMYVAR1 -DMYVAR2\" like so to set them"
libhalfive.a : h5stdlib.o h5asm.o h5vm.o h5doc.o h5math.o h5rat.o
	ar rcs libhalfive.a h5stdlib.o h5asm.o h5vm.o h5doc.o h5math.o h5rat.o
h5assembler : h5assembler.o h5vm.o h5stdlib.o
	cc $(PARAMS) -o h5assembler h5assembler.o h5vm.o h5stdlib.o
h5assembler.o : h5vm.o
	cc $(PARAMS) -I$(INCLROOT) -DH5ASSEMBLY -c -o h5assembler.o h5vm/h5asm.c
h5asm.o : h5vm.o
	cc $(PARAMS) -I$(INCLROOT) -c -o h5asm.o h5vm/h5asm.c
h5vm.o : h5stdlib.o
	cc $(PARAMS) -I$(INCLROOT) -c -o h5vm.o h5vm/h5vm.c
h5doc.o : h5stdlib.o
	cc $(PARAMS) -I$(INCLROOT) -c -o h5doc.o h5doc/h5doc.c
h5stdlib.o :
	cc $(PARAMS) -I$(INCLROOT) -c -o h5stdlib.o h5stdlib.c
h5math.o :
	cc $(PARAMS) -I$(INCLROOT) -c -o h5math.o h5math.c
h5rat.o :
	cc $(PARAMS) -I$(INCLROOT) -c -o h5rat.o h5rat.c
