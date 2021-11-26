/*	
 * MIT License
 *
 * Copyright (c) 2010 Serge Zaitsev
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef JSMN_H
#define JSMN_H
#endif 
#include <stddef.h>

#define JSMN_API extern

/**
 * JSON type identifier. Basic types are:
 * 	o Object
 * 	o Array
 * 	o String
 * 	o Other primitive: number, boolean (true/false) or null
 */
typedef enum {
  JSMN_UNDEFINED = 0,
  JSMN_OBJECT = 1,
  JSMN_ARRAY = 2,
  JSMN_STRING = 3,
  JSMN_PRIMITIVE = 4
} jsmntype_t;

enum jsmnerr {
  /* Not enough tokens were provided */
  JSMN_ERROR_NOMEM = -1,
  /* Invalid character inside JSON string */
  JSMN_ERROR_INVAL = -2,
  /* The string is not a full JSON packet, more bytes expected */
  JSMN_ERROR_PART = -3
};

/**
 * JSON token description.
 * type		type (object, array, string etc.)
 * start	start position in JSON data string
 * end		end position in JSON data string
 */
typedef struct jsmntok {
  jsmntype_t type;
  unsigned int start;
  unsigned int end;
  unsigned int size;
#ifdef JSMN_PARENT_LINKS
  int parent;
#endif
} jsmntok_t;

/**
 * JSON parser state. Describes what kind of token or character is expected
 * next.
 */
typedef enum {
  JSMN_DELIMITER = 0x1,   /* Expecting colon if JSMN_KEY, comma otherwise */
  JSMN_CAN_CLOSE = 0x2,   /* Object or array can close here */

  JSMN_KEY = 0x10,    /* Expecting an object key or the delimiter following */
  JSMN_VALUE = 0x20,  /* Expecting an object value or the delimiter following */
  JSMN_IN_ARRAY = 0x40,   /* Expecting array item or the delimiter following */
  /* In an object if (state & JSMN_IN_OBJECT). Otherwise, in an array or at
   * the root. */
  JSMN_IN_OBJECT = JSMN_KEY | JSMN_VALUE,

  /* Actual values for parser->state */
  /* Root: not in any array or object */
  JSMN_STATE_ROOT = 0x0,
  /* Just saw object opening ({). Expecting key or close. */
  JSMN_STATE_OBJ_NEW = (JSMN_KEY | JSMN_CAN_CLOSE),
  /* Just saw a comma in an object. Expecting key. */
  JSMN_STATE_OBJ_KEY = (JSMN_KEY),
  /* Just saw a key in an object. Expecting colon. */
  JSMN_STATE_OBJ_COLON = (JSMN_KEY | JSMN_DELIMITER),
  /* Just saw a colon in an object. Expecting value. */
  JSMN_STATE_OBJ_VAL = (JSMN_VALUE),
  /* Just saw a value in an object. Expecting comma or close. */
  JSMN_STATE_OBJ_COMMA = (JSMN_VALUE | JSMN_DELIMITER | JSMN_CAN_CLOSE),
  /* Just saw an array opening ([). Expecting item or close. */
  JSMN_STATE_ARRAY_NEW = (JSMN_IN_ARRAY | JSMN_CAN_CLOSE),
  /* Just saw a comma in an array. Expecting item. */
  JSMN_STATE_ARRAY_ITEM = (JSMN_IN_ARRAY),
  /* Just saw an item in an array. Expecting comma or close. */
  JSMN_STATE_ARRAY_COMMA = (JSMN_DELIMITER | JSMN_IN_ARRAY | JSMN_CAN_CLOSE)
} jsmnstate_t;

/**
 * JSON parser. Contains an array of token blocks available. Also stores
 * the string being parsed now and current position in that string.
 */
typedef struct jsmn_parser {
  unsigned int pos;     /* offset in the JSON string */
  unsigned int toknext; /* next token to allocate */
  int toksuper;         /* superior token node, e.g. parent object or array */
  jsmnstate_t state;    /* parser state, from jsmnstate_t */
  int tokbefore;        /* token immediately preceding the first token in the
                           current JSON object */
} jsmn_parser;

/**
 * Create JSON parser over an array of tokens
 */
JSMN_API void jsmn_init(jsmn_parser *parser);

/**
 * Run JSON parser. It parses a JSON data string into and array of tokens, each
 * describing
 * a single JSON object.
 */
JSMN_API int jsmn_parse(jsmn_parser *parser, const char *js, const size_t len,
                        jsmntok_t *tokens, const unsigned int num_tokens);

/**
 * Parse JSON string and fill tokens.
 */
JSMN_API int jsmn_parse(jsmn_parser *parser, const char *js, const size_t len,
                        jsmntok_t *tokens, const unsigned int num_tokens);

/**
 * Creates a new parser based over a given buffer with an array of tokens
 * available.
 */
JSMN_API void jsmn_init(jsmn_parser *parser);
