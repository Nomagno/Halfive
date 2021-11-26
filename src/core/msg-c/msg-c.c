#include <stdio.h>
#include "msg-c.h"
/*
Redistributions, modified or unmodified, in whole or in part, must retain
applicable notices of copyright or other legal privilege, these conditions, and
the following license terms and disclaimer.  Subject to these conditions, each
holder of copyright or other legal privileges, author or assembler, and
contributor of this work, henceforth "licensor", hereby grants to any person
who obtains a copy of this work in any form:

1. Permission to reproduce, modify, distribute, publish, sell, sublicense, use,
and/or otherwise deal in the licensed material without restriction.

2. A perpetual, worldwide, non-exclusive, royalty-free, gratis, irrevocable
patent license to make, have made, provide, transfer, import, use, and/or
otherwise deal in the licensed material without restriction, for any and all
patents held by such licensor and necessarily infringed by the form of the work
upon distribution of that licensor's contribution to the work under the terms
of this license.

NO WARRANTY OF ANY KIND IS IMPLIED BY, OR SHOULD BE INFERRED FROM, THIS LICENSE
OR THE ACT OF DISTRIBUTION UNDER THE TERMS OF THIS LICENSE, INCLUDING BUT NOT
LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE,
AND NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS, ASSEMBLERS, OR HOLDERS OF
COPYRIGHT OR OTHER LEGAL PRIVILEGE BE LIABLE FOR ANY CLAIM, DAMAGES, OR OTHER
LIABILITY, WHETHER IN ACTION OF CONTRACT, TORT, OR OTHERWISE ARISING FROM, OUT
OF, OR IN CONNECTION WITH THE WORK OR THE USE OF OR OTHER DEALINGS IN THE WORK.*/
/*
'Primitive' only includes a single convenience boolean value: isInit, and an interface to a constructor that may be filled with a stub.
*/


/*INITIALIZATION OF PRIMITIVE*/
int __PrimitiveInit(mrefclass(Primitive) *self) {
	mgetpub((*self), Primitive, isInit) = 1;
  return 2;
}


/*
SEE msg-c.h FOR FORMAL DOCUMENTATION



EXAMPLE OF HOW TO EXTEND THE PRIMITIVE OBJECT:
*/


//DOWNSTREAM, HAS THREE PUBLIC VALUES, A SECRET VALUE, AND INHERITS FROM PRIMITIVE
/*
MsgNewClass(
	Downstream,
	MsgRefClass(Primitive), 

		//These fields aren't really 'private' if you try hard enough, but you shouldn't try to poke around with them as they are not part of the class' stable API.
		MsgAddPrivate(
			Downstream,
			priv,
			int secretvalue
		),
	MsgAddPublic(
		Downstream,
		int val, 
		char val2, 
		float val3, 
		volatile const int * readonly,
		MsgAddInterface(int, example_privateinterface, (int d, int b, MsgSelfRef(Downstream) *self)),
		MsgAddInterface(int, example_publicinterface, (int d, int b, MsgSelfRef(Downstream) *self)),
		MsgAddInterface(void, setsecretvalue, (int val, MsgSelfRef(Downstream) *self)),
		MsgAddInterface(void, init, (MsgSelfRef(Downstream) *self))
		)
);
*/

//ADD IMPLEMENTATIONS FOR THE INTERFACES DEFINED IN DOWNSTREAM
/*
//DO SIMPLE ARITHMETIC WITH PUBLIC FIELDS
int myimpl_public_arithmetic(int d, int b, MsgRefClass(Downstream) *self) {
	return MsgAccessPublic((*self), Downstream, val) + d * b;
}
*/
/*
//DO SIMPLE ARITHMETIC WITH PRIVATE FIELDS
int myimpl_private_arithmetic(int d, int b, MsgRefClass(Downstream) *self) {
	return MsgAccessPrivate((*self), Downstream, priv, secretvalue) + 1 + d*b;
}
*/
/*
//MANIPULATION OF PRIVATE FIELDS WITH PUBLIC INTERFACE
void myimpl_manipulate_secrets(int val, MsgRefClass(Downstream) *self) {
	MsgAccessPrivate((*self), Downstream, priv, secretvalue) = val;
}
*/
/*
//INITIALIZATION OF DOWNSTREAM
void myimpl_init_downstream(MsgRefClass(Downstream) *self) {
	MsgAccessPublic((*self), Downstream, val) = 0;
	MsgAccessPublic((*self), Downstream, val2) = 0;
	MsgAccessPublic((*self), Downstream, val3) = 0;
	MsgAccessPublic((*self), Downstream, readonly) = &MsgAccessPrivate((*self), Downstream, priv, secretvalue);
	MsgAccessPrivate((*self), Downstream, priv, secretvalue) = 0;

	MsgAccessPublic((*self).super, Primitive, init) = Primitive_init;	//INITIALIZE PARENT
	MsgAccessPublic((*self).super, Primitive, init(&self->super));	//INITIALIZE PARENT
}
*/
/*
void test() {
	MsgRefClass(Downstream) myobj;
	MsgAccessPublic(myobj.super, Primitive, x) = 90;


	MsgAccessPublic(myobj, Downstream, example_publicinterface) = myimpl_public_arithmetic;
	MsgAccessPublic(myobj, Downstream, example_privateinterface) = myimpl_private_arithmetic;
	MsgAccessPublic(myobj, Downstream, setsecretvalue) = myimpl_manipulate_secrets;

	MsgAccessPublic(myobj, Downstream, init) = myimpl_init_downstream;
	MsgAccessPublic(myobj.super, Primitive, init) = Primitive_init;

	MsgAccessPublic(myobj, Downstream, init(&myobj)); //INITIALIZE SELF (THIS INITIALIZES PARENT)

	myobj.PubDownstream.val2 = 'a';
	myobj.PubDownstream.val = 12;
	MsgAccessPublic(myobj, Downstream, setsecretvalue(19, &myobj));	//INITIALIZE PARENT

	printf(
	"5 * 78 + first public field (12): %i\n"
	"Second public value as char: %c\n"
	"7 * 2 + 1 + private field: %i\n", 
	MsgAccessPublic(myobj, Downstream, example_publicinterface(5, 78, &myobj)), 
	MsgAccessPublic(myobj, Downstream, val2), 
	MsgAccessPublic(myobj, Downstream, example_privateinterface(7, 2, &myobj))
	);
}


*/
