#include <halfive/h5req.h>

#if defined(PRINTF_AVAILABLE) && !defined(DISABLE_PRINTF)

#define maybe_printf printf
#define maybe_puts puts
#define maybe_putchar putchar

#else

#define maybe_printf printf_unavailable
#define maybe_puts puts_unavailable
#define maybe_putchar putchar_unavailable
static int printf_unavailable(char *str, ...) { return 0; }
static int puts_unavailable(char *str) { return 0; }
static int putchar_unavailable(char *c) { return 0; }

#endif
