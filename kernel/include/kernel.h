
#pragma once
#include "types.h"

struct context_t;

typedef struct context_t Context;

extern Context kernel_ctx;

void set_kernel_mode();
void set_user_mode();

/* longjmp to context ctx (from a previous setjmp),
 */
__attribute__((noreturn))
void longjmp(Context * ctx, usize data);

/* longjmp to context ctx with an eret instruction (for use within exception handlers)
 */
__attribute__((noreturn))
void eret_longjmp(Context * ctx, usize data);

/* save current context in *ctx for a future longjmp,
 * return 0 when returning normally (the first time) and 1 when returning from lonjmp
 */
__attribute__((returns_twice))
isize setjmp(Context * ctx);

void kmain();

