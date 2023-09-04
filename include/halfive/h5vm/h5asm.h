/*
Copyright Nomagno 2021, 2022, 2023

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

/*Halfive Virtual Machine Assembly language utilities*/

#ifndef H5ASM_H
#define H5ASM_H
#include <halfive/h5req.h>
#include <halfive/h5vm/h5vm.h>
h5uint H5ASM_parse(char *linestr, H5VM_ExecutableUnit *code);

#define STRARR(...) (char *[]){__VA_ARGS__}, ELEMNUM((char *[]){__VA_ARGS__})

#define H5ASM_ASSEMBLE(a, b, ...) H5ASM_run(STRARR(__VA_ARGS__), &context, a, b, ELEMNUM(b))
/*Use as ASM(arrayofints, arraytomap, $(machine code), ...) to execute all instructions (comma-separated) from the third argument, mapping the list of addresses from the first argument to the elements of the array in the second argument*/

#endif
