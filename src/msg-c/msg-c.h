
/*
BASED ON A CODE SNIPPET RELEASED UNDER THIS LICENSE:

Copyright (c) 2015, jason-deng
Copyright (c) 2021, Nomagno
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of C99FunctionOverload nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/*

USAGE OF Message-C, a small 200 lines Object-Oriented macro library for C

SEE msg-c.c SECTION 'EXAMPLE OF HOW TO EXTEND THE PRIMITIVE OBJECT' FOR A LESS
BORING USAGE EXAMPLE

ADDING AND REFERENCING OBJECT:

maddclass(Name, Parent, ...)
-Syntantic sugar for a typedef'ed struct
-The struct naming scheme is MsgStruct_<Name>, replacing Name of course
-The type naming scheme is MsgObj_<Name>, replacing Name of course
-The 'Parent' argument is used to store an empty object of this type under the
'super' member of the struct. -It takes up to 8 optional members, that can be
public member groups, private member groups, or a custom variable/struct (See
'ADDING MEMBERS', and 'TYPES OF MEMBERS')

mrefclass(Name)
-Reference an external Class by name (applies the naming scheme 'MsgObj_<Name>')

mselfref(Name)
-Reference a Class from within its declaration (applies the naming scheme
'struct MsgStruct_<Name>')


ADDING MEMBERS:

maddpub(ClassName, param1, ...)
-Syntactic sugar for an anonymous struct that has a known naming scheme
-The naming scheme is Pub<ClassName>, replacing ClassName of course
-It takes up to 7 optional parameters, where each parameter is a member (See
'TYPES OF MEMBERS') -If a parameter/member has type 'volatile const <type>
*const var', it is effectively read-only (Can be modified trough any means OTHER
than the pointer).
- See 'ACCESSING MEMBERS'

maddpriv(ClassName, Suffix, param1, ...)
-Syntactic sugar for an anonymous struct that has a known naming scheme, but
members with a secret naming scheme -The naming scheme of the struct is
Priv<ClassName>, replacing ClassName of course -The naming scheme of the
individual private members is param1<Suffix>, replacing Suffix of course -This
naming scheme should make it hard to access private members on accident, because
they are by definition not meant to be relied upon -It takes up to 6 optional
parameters, where each optional parameter is a member (See 'TYPES OF MEMBERS')
- See 'ACCESSING MEMBERS'

ACCESSING MEMBERS:
mgetpub(Instance, Name, Member)
-Accesses a public member <Member> of an instance <Instance> of object type
<Name> -Syntactic sugar for Instance.Pub<Name>.member

mgetpriv(Instance, Name, Suffix, Member)
-Accesses a private member <Member> of an instance <Instance> of Class type
<Name>, with quote and quote '''password''' <Suffix> -Syntactic sugar for
Instance.Priv<Name>.member<Suffix>

TYPES OF MEMBERS, AND WAYS OF DECLARING THEM:
You can use, of course, maddpub() and maddpriv() to add public/private
member groups. However, since this is all syntactic sugar for structs, you can
also nest these undefinitely, OR a METHOD.

METHOD:

maddinter(ReturnType, Call, Params)
-ReturnType is, well, the return type of the method (function).
-Call is the name of the method.
-Params is the type signature of the method/function, enclosed in parentheses
like if you were declaring a function normally
-EXAMPLE: -MsgAddInterface(void,
do_stuff, (int just_a_number, MsgSelfRef(Primitive) *self)) -In the example, we
add an interface for a method do_stuff() with return type void, with arguments
'int just_a_number', and 'MsgSelfRef(Primitive) *self'. This last one is
syntactic sugar for a pointer to an instance of the object it is part of. For
example, you could make an object Primitive, and implement this method as a
functions that returns the addition of just_a_number and a private member of the
instance.
-In more OOP terms: This is how you declare an interface you can then
implement.
-You can make a function with this type signature, and assign it to
an instance of Primitive.


ADDITIONAL FUNCTIONS:

yield(var, val)
-Where var is global, you can use a switch statement on it
-to barely simulate a coroutine with no callstack (or stack)

yclear(var)
-Resets var

makefuncptr(type, name)
-Typedef a function pointer with 'type' return type,
-and any number of non-variadic parameters, to 'name'

MACRO CONSTANTS:

TOP: macro for 'void *', intended to indicate a class has no parent
EMPTY: Equivalent to TOP, intended to indicate other things

ARRUNIT: Just a utility thing

*/

/*THE MACROS*/
#define FUNC_CHOOSER_DEF(_f1, _f2, _f3, _f4, _f5, _f6, _f7, _f8, _f9, _f10,    \
			 ...)                                                  \
	_f10

/*VARIADIC MsgNewClass DEFINITIONS, START (SEARCH FOR 'END' TO SKIP)*/
#define MsgNewClass_0() Need_two_arguments abort;
#define MsgNewClass_1(a) Need_two_arguments abort;

/*
TOP super -> void *super
ONLY SPECIFY THE TYPE OF THE THINGY IT INHERITS FROM
*/
#define MsgNewClass_2(name, inherits)                                          \
	typedef struct MsgStruct_##name {                                      \
		inherits super;                                                \
	} MsgObj_##name

#define MsgNewClass_3(name, inherits, field1)                                  \
	typedef struct MsgStruct_##name {                                      \
		inherits super;                                                \
		field1;                                                        \
	} MsgObj_##name

#define MsgNewClass_4(name, inherits, field1, field2)                          \
	typedef struct MsgStruct_##name {                                      \
		inherits super;                                                \
		field1;                                                        \
		field2;                                                        \
	} MsgObj_##name

#define MsgNewClass_5(name, inherits, field1, field2, field3)                  \
	typedef struct MsgStruct_##name {                                      \
		inherits super;                                                \
		field1;                                                        \
		field2;                                                        \
		field3;                                                        \
	} MsgObj_##name

#define MsgNewClass_6(name, inherits, field1, field2, field3, field4)          \
	typedef struct MsgStruct_##name {                                      \
		inherits super;                                                \
		field1;                                                        \
		field2;                                                        \
		field3;                                                        \
		field4;                                                        \
	} MsgObj_##name

#define MsgNewClass_7(name, inherits, field1, field2, field3, field4, field5)  \
	typedef struct MsgStruct_##name {                                      \
		inherits super;                                                \
		field1;                                                        \
		field2;                                                        \
		field3;                                                        \
		field4;                                                        \
	} MsgObj_##name

#define MsgNewClass_8(name, inherits, field1, field2, field3, field4, field5,  \
		      field6)                                                  \
	typedef struct MsgStruct_##name {                                      \
		inherits super;                                                \
		field1;                                                        \
		field2;                                                        \
		field3;                                                        \
		field4;                                                        \
	} MsgObj_##name

#define MsgNewClass_9(name, inherits, field1, field2, field3, field4, field5,  \
		      field6, field7)                                          \
	typedef struct MsgStruct_##name {                                      \
		inherits super;                                                \
		field1;                                                        \
		field2;                                                        \
		field3;                                                        \
		field4;                                                        \
	} MsgObj_##name

#define MsgNewClass_MACRO_CHOOSER(...)                                         \
	MsgNewClass_CHOOSE_FROM_ARG_COUNT(NO_ARG_EXPANDER __VA_ARGS__())
#define MsgNewClass_FUNC_RECOMPOSER(argsWithParentheses)                       \
	FUNC_CHOOSER_DEF argsWithParentheses
#define MsgNewClass_CHOOSE_FROM_ARG_COUNT(...)                                 \
	MsgNewClass_FUNC_RECOMPOSER(                                           \
	    (__VA_ARGS__, MsgNewClass_9, MsgNewClass_8, MsgNewClass_7,         \
	     MsgNewClass_6, MsgNewClass_5, MsgNewClass_4, MsgNewClass_3,       \
	     MsgNewClass_2, MsgNewClass_1, MsgNewClass_0))
#define MsgNewClass_NO_ARG_EXPANDER() , , , , , , , , , MsgNewClass_0
#define maddclass(...) MsgNewClass_MACRO_CHOOSER(__VA_ARGS__)(__VA_ARGS__)
/*VARIADIC MsgNewClass DEFINITIONS, END*/

/*VARIADIC MsgAddPublic DEFINITIONS, START (SEARCH FOR 'END' TO SKIP)*/
#define MsgAddPublic_0() Need_two_arguments abort;
#define MsgAddPublic_1(a) Need_two_arguments abort;

#define MsgAddPublic_2(strname, par1)                                          \
	struct {                                                               \
		par1;                                                          \
	} Pub##strname

#define MsgAddPublic_3(strname, par1, par2)                                    \
	struct {                                                               \
		par1;                                                          \
		par2;                                                          \
	} Pub##strname

#define MsgAddPublic_4(strname, par1, par2, par3)                              \
	struct {                                                               \
		par1;                                                          \
		par2;                                                          \
		par3;                                                          \
	} Pub##strname

#define MsgAddPublic_5(strname, par1, par2, par3, par4)                        \
	struct {                                                               \
		par1;                                                          \
		par2;                                                          \
		par3;                                                          \
		par4;                                                          \
	} Pub##strname

#define MsgAddPublic_6(strname, par1, par2, par3, par4, par5)                  \
	struct {                                                               \
		par1;                                                          \
		par2;                                                          \
		par3;                                                          \
		par4;                                                          \
		par5;                                                          \
	} Pub##strname

#define MsgAddPublic_7(strname, par1, par2, par3, par4, par5, par6)            \
	struct {                                                               \
		par1;                                                          \
		par2;                                                          \
		par3;                                                          \
		par4;                                                          \
		par5;                                                          \
		par6;                                                          \
	} Pub##strname

#define MsgAddPublic_8(strname, par1, par2, par3, par4, par5, par6, par7)      \
	struct {                                                               \
		par1;                                                          \
		par2;                                                          \
		par3;                                                          \
		par4;                                                          \
		par5;                                                          \
		par6;                                                          \
		par7;                                                          \
	} Pub##strname

#define MsgAddPublic_9(strname, par1, par2, par3, par4, par5, par6, par7,      \
		       par8)                                                   \
	struct {                                                               \
		par1;                                                          \
		par2;                                                          \
		par3;                                                          \
		par4;                                                          \
		par5;                                                          \
		par6;                                                          \
		par7;                                                          \
		par8;                                                          \
	} Pub##strname

#define MsgAddPublic_MACRO_CHOOSER(...)                                        \
	MsgAddPublic_CHOOSE_FROM_ARG_COUNT(NO_ARG_EXPANDER __VA_ARGS__())
#define MsgAddPublic_FUNC_RECOMPOSER(argsWithParentheses)                      \
	FUNC_CHOOSER_DEF argsWithParentheses
#define MsgAddPublic_CHOOSE_FROM_ARG_COUNT(...)                                \
	MsgAddPublic_FUNC_RECOMPOSER(                                          \
	    (__VA_ARGS__, MsgAddPublic_9, MsgAddPublic_8, MsgAddPublic_7,      \
	     MsgAddPublic_6, MsgAddPublic_5, MsgAddPublic_4, MsgAddPublic_3,   \
	     MsgAddPublic_2, MsgAddPublic_1, MsgAddPublic_0))
#define MsgAddPublic_NO_ARG_EXPANDER() , , , , , , , , , MsgAddPublic_0
#define maddpub(...) MsgAddPublic_MACRO_CHOOSER(__VA_ARGS__)(__VA_ARGS__)
/*VARIADIC MsgAddPublic DEFINITIONS, END*/

/*VARIADIC MsgAddPrivate DEFINITIONS, START (SEARCH FOR 'END' TO SKIP)*/
#define MsgAddPrivate_0() Need_three_arguments abort;
#define MsgAddPrivate_1(a) Need_three_arguments abort;
#define MsgAddPrivate_2(a, b) Need_three_arguments abort;

#define MsgAddPrivate_3(strname, suffix, par1)                                 \
	struct {                                                               \
		par1##suffix;                                                  \
	} Priv##strname

#define MsgAddPrivate_4(strname, suffix, par1, par2)                           \
	struct {                                                               \
		par1##suffix;                                                  \
		par2##suffix;                                                  \
	} Priv##strname

#define MsgAddPrivate_5(strname, suffix, par1, par2, par3)                     \
	struct {                                                               \
		par1##suffix;                                                  \
		par2##suffix;                                                  \
		par3##suffix;                                                  \
	} Priv##strname;

#define MsgAddPrivate_6(strname, suffix, par1, par2, par3, par4)               \
	struct {                                                               \
		par1##suffix;                                                  \
		par2##suffix;                                                  \
		par3##suffix;                                                  \
		par4##suffix;                                                  \
	} Priv##strname

#define MsgAddPrivate7(strname, suffix, par1, par2, par3, par4, par5)          \
	struct {                                                               \
		par1##suffix;                                                  \
		par2##suffix;                                                  \
		par3##suffix;                                                  \
		par4##suffix;                                                  \
		par5##suffix;                                                  \
	} Priv##strname

#define MsgAddPrivate_8(strname, suffix, par1, par2, par3, par4, par5, par6)   \
	struct {                                                               \
		par1##suffix;                                                  \
		par2##suffix;                                                  \
		par3##suffix;                                                  \
		par4##suffix;                                                  \
		par5##suffix;                                                  \
		par6##suffix;                                                  \
	} Priv##strname

#define MsgAddPrivate_9(strname, suffix, par1, par2, par3, par4, par5, par6,   \
			par7)                                                  \
	struct {                                                               \
		par1##suffix;                                                  \
		par2##suffix;                                                  \
		par3##suffix;                                                  \
		par4##suffix;                                                  \
		par5##suffix;                                                  \
		par6##suffix;                                                  \
		par7##suffix;                                                  \
	} Priv##strname

#define MsgAddPrivate_MACRO_CHOOSER(...)                                       \
	MsgAddPrivate_CHOOSE_FROM_ARG_COUNT(NO_ARG_EXPANDER __VA_ARGS__())
#define MsgAddPrivate_FUNC_RECOMPOSER(argsWithParentheses)                     \
	FUNC_CHOOSER_DEF argsWithParentheses
#define MsgAddPrivate_CHOOSE_FROM_ARG_COUNT(...)                               \
	MsgAddPrivate_FUNC_RECOMPOSER(                                         \
	    (__VA_ARGS__, MsgAddPrivate_9, MsgAddPrivate_8, MsgAddPrivate_7,   \
	     MsgAddPrivate_6, MsgAddPrivate_5, MsgAddPrivate_4,                \
	     MsgAddPrivate_3, MsgAddPrivate_2, MsgAddPrivate_1,                \
	     MsgAddPrivate_0))
#define MsgAddPrivate_NO_ARG_EXPANDER() , , , , , , , , , MsgAddPrivate_0
#define maddpriv(...) MsgAddPrivate_MACRO_CHOOSER(__VA_ARGS__)(__VA_ARGS__)
/*VARIADIC MsgAddPrivate DEFINITIONS, END*/

/*Reference external Class*/
#define mrefclass(Class) MsgObj_##Class

/*Self reference*/
#define mselfref(Class) struct MsgStruct_##Class

/*Add interface (method)*/
#define maddinter(functype, fieldname, funcparams)                             \
	functype(*fieldname) funcparams

/*Reference public member of an instance of a Class*/
#define mgetpub(Instance, type, member) Instance.Pub##type.member

/*Reference private member of an instance of an object*/
#define mgetpriv(Instance, type, suffix, member)                               \
	Instance.Priv##type.member##suffix

/*For classes without inheritance*/
#define TOP void *

/*For arguments that are left empty*/
#define EMPTY void *

/*
  Where var is global, you can use a switch statement
  on it to barely simulate a coroutine with no callstack
*/
#define yield(var, val)                                                        \
	var += 1;                                                              \
	return val

/*Resets var*/
#define yclear(var) var = 0

#define ARRUNIT 50

/*
  Typedef a function pointer with 'type' return type,
  and any number of non-variadic parameters, to 'name'
*/
#define makefuncptr(type, name) typedef type (*name)()

maddclass(Primitive, TOP,
	  maddpub(Primitive, _Bool isInit,
		  maddinter(int, init, (mselfref(Primitive) * self))
		  /*You could have a constructor instead of an initializer*/
		  ));

/*INITIALIZATION OF PRIMITIVE*/
extern int __PrimitiveInit(mrefclass(Primitive) * self);
