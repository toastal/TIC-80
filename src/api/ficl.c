// MIT License

// Copyright (c) 2026 toastal

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the “Software”), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
#include "api.h"
#include "core/core.h"
#include "script.h"
#include "tic.h"

#if defined(TIC_BUILD_WITH_FICL)

#include "ficl.h"

static const char* const FiclKeywords[] =
{
    // Control flow
    "IF", "ELSE", "THEN", "BEGIN", "UNTIL", "WHILE", "REPEAT", "DO", "LOOP",
    "+LOOP", "CASE", "OF", "ENDOF", "ENDCASE",
    // Definitions
    ":", ";", "IMMEDIATE", "CREATE", "ALLOT", "VARIABLE", "CONSTANT", "VALUE",
    "TO",
    // Stack
    "DUP", "DROP", "SWAP", "OVER", "ROT", ">R", "R>", "R@",
    // Memory
    "@", "!", "C@", "C!", "CELLS",
    // Arithmetic
    "+", "-", "*", "/", "MOD", "1+", "1-", "AND", "OR", "XOR", "INVERT",
    // Comparison
    "=", "<>", "<", ">", "<=", ">=", "0=", "0<", "0>",
    // Output
    ".", ".S", "EMIT", "CR",
    // Execution
    "'", "EXECUTE", "EXIT",
};

static void closeFicl(tic_mem* tic);

static inline s32 cell_to_int(CELL c)
{
    return (s32)c.i;
}

static inline int int_to_cell(s32 i)
{
    CELL c;
    c.i = i;
    return c.i;
}

static tic_core* getFiclCore(FICL_VM* vm)
{
    return (tic_core*)ficlGetUserPointer(vm);
}

static void ficl_cls(FICL_VM *vm)
{
    tic_core* core = getFiclCore(vm);
    if (!core) return;
    s32 color = cell_to_int(vmPop(vm));
    tic_api_cls((tic_mem*)core, color);
}

static void ficl_pix(FICL_VM *vm)
{
    tic_core* core = getFiclCore(vm);
    if (!core) return;
    s32 color = cell_to_int(vmPop(vm));
    s32 y = cell_to_int(vmPop(vm));
    s32 x = cell_to_int(vmPop(vm));
    tic_api_pix((tic_mem*)core, x, y, color, false);
}

static void ficl_line(FICL_VM *vm)
{
    tic_core* core = getFiclCore(vm);
    if (!core) return;
    s32 color = cell_to_int(vmPop(vm));
    s32 y1 = cell_to_int(vmPop(vm));
    s32 x1 = cell_to_int(vmPop(vm));
    s32 y0 = cell_to_int(vmPop(vm));
    s32 x0 = cell_to_int(vmPop(vm));
    tic_api_line((tic_mem*)core, x0, y0, x1, y1, color);
}

static void ficl_rect(FICL_VM *vm)
{
    tic_core* core = getFiclCore(vm);
    if (!core) return;
    s32 color = cell_to_int(vmPop(vm));
    s32 h = cell_to_int(vmPop(vm));
    s32 w = cell_to_int(vmPop(vm));
    s32 y = cell_to_int(vmPop(vm));
    s32 x = cell_to_int(vmPop(vm));
    tic_api_rect((tic_mem*)core, x, y, w, h, color);
}

static void ficl_rectb(FICL_VM *vm)
{
    tic_core* core = getFiclCore(vm);
    if (!core) return;
    s32 color = cell_to_int(vmPop(vm));
    s32 h = cell_to_int(vmPop(vm));
    s32 w = cell_to_int(vmPop(vm));
    s32 y = cell_to_int(vmPop(vm));
    s32 x = cell_to_int(vmPop(vm));
    tic_api_rectb((tic_mem*)core, x, y, w, h, color);
}

static void ficl_circ(FICL_VM *vm)
{
    tic_core* core = getFiclCore(vm);
    if (!core) return;
    s32 color = cell_to_int(vmPop(vm));
    s32 r = cell_to_int(vmPop(vm));
    s32 y = cell_to_int(vmPop(vm));
    s32 x = cell_to_int(vmPop(vm));
    tic_api_circ((tic_mem*)core, x, y, r, color);
}

static void ficl_circb(FICL_VM *vm)
{
    tic_core* core = getFiclCore(vm);
    if (!core) return;
    s32 color = cell_to_int(vmPop(vm));
    s32 r = cell_to_int(vmPop(vm));
    s32 y = cell_to_int(vmPop(vm));
    s32 x = cell_to_int(vmPop(vm));
    tic_api_circb((tic_mem*)core, x, y, r, color);
}

static void ficl_elli(FICL_VM *vm)
{
    tic_core* core = getFiclCore(vm);
    if (!core) return;
    s32 color = cell_to_int(vmPop(vm));
    s32 b = cell_to_int(vmPop(vm));
    s32 a = cell_to_int(vmPop(vm));
    s32 y = cell_to_int(vmPop(vm));
    s32 x = cell_to_int(vmPop(vm));
    tic_api_elli((tic_mem*)core, x, y, a, b, color);
}

static void ficl_ellib(FICL_VM *vm)
{
    tic_core* core = getFiclCore(vm);
    if (!core) return;
    s32 color = cell_to_int(vmPop(vm));
    s32 b = cell_to_int(vmPop(vm));
    s32 a = cell_to_int(vmPop(vm));
    s32 y = cell_to_int(vmPop(vm));
    s32 x = cell_to_int(vmPop(vm));
    tic_api_ellib((tic_mem*)core, x, y, a, b, color);
}

static void ficl_tri(FICL_VM *vm)
{
    tic_core* core = getFiclCore(vm);
    if (!core) return;
    s32 color = cell_to_int(vmPop(vm));
    s32 y2 = cell_to_int(vmPop(vm));
    s32 x2 = cell_to_int(vmPop(vm));
    s32 y1 = cell_to_int(vmPop(vm));
    s32 x1 = cell_to_int(vmPop(vm));
    s32 y0 = cell_to_int(vmPop(vm));
    s32 x0 = cell_to_int(vmPop(vm));
    tic_api_tri((tic_mem*)core, x0, y0, x1, y1, x2, y2, color);
}

static void ficl_trib(FICL_VM *vm)
{
    tic_core* core = getFiclCore(vm);
    if (!core) return;
    s32 color = cell_to_int(vmPop(vm));
    s32 y2 = cell_to_int(vmPop(vm));
    s32 x2 = cell_to_int(vmPop(vm));
    s32 y1 = cell_to_int(vmPop(vm));
    s32 x1 = cell_to_int(vmPop(vm));
    s32 y0 = cell_to_int(vmPop(vm));
    s32 x0 = cell_to_int(vmPop(vm));
    tic_api_trib((tic_mem*)core, x0, y0, x1, y1, x2, y2, color);
}

static void ficl_clip(FICL_VM *vm)
{
    tic_core* core = getFiclCore(vm);
    if (!core) return;
    s32 height = cell_to_int(vmPop(vm));
    s32 width = cell_to_int(vmPop(vm));
    s32 y = cell_to_int(vmPop(vm));
    s32 x = cell_to_int(vmPop(vm));
    tic_api_clip((tic_mem*)core, x, y, width, height);
}

static void ficl_paint(FICL_VM *vm)
{
    tic_core* core = getFiclCore(vm);
    if (!core) return;
    s32 border = cell_to_int(vmPop(vm));
    s32 color = cell_to_int(vmPop(vm));
    s32 y = cell_to_int(vmPop(vm));
    s32 x = cell_to_int(vmPop(vm));
    tic_api_paint((tic_mem*)core, x, y, color, border);
}

static void ficl_spr_star(FICL_VM* vm)
{
    tic_core* core = getFiclCore(vm);
    if (!core) return;

    s32 count = cell_to_int(vmPop(vm));
    const u8* colors = (const u8*)(intptr_t)vmPop(vm).i;
    s32 rotate = cell_to_int(vmPop(vm));
    s32 flip = cell_to_int(vmPop(vm));
    s32 scale = cell_to_int(vmPop(vm));
    s32 h = cell_to_int(vmPop(vm));
    s32 w = cell_to_int(vmPop(vm));
    s32 y = cell_to_int(vmPop(vm));
    s32 x = cell_to_int(vmPop(vm));
    s32 id = cell_to_int(vmPop(vm));

    if (count < 0) count = 0;
    if (count > TIC_PALETTE_SIZE) count = TIC_PALETTE_SIZE;

    tic_api_spr((tic_mem*)core, id, x, y, w, h, colors, count, scale, flip, rotate);
}

static void ficl_map_star(FICL_VM* vm)
{
    tic_core* core = getFiclCore(vm);
    if (!core) return;

    s32 count = cell_to_int(vmPop(vm));
    const u8* colors = (const u8*)(intptr_t)vmPop(vm).i;
    s32 scale = cell_to_int(vmPop(vm));
    s32 sy = cell_to_int(vmPop(vm));
    s32 sx = cell_to_int(vmPop(vm));
    s32 height = cell_to_int(vmPop(vm));
    s32 width = cell_to_int(vmPop(vm));
    s32 y = cell_to_int(vmPop(vm));
    s32 x = cell_to_int(vmPop(vm));

    if (count < 0) count = 0;
    if (count > TIC_PALETTE_SIZE) count = TIC_PALETTE_SIZE;

    tic_api_map((tic_mem*)core, x, y, width, height, sx, sy, colors, count, scale, NULL, NULL);
}

static void ficl_mget(FICL_VM* vm)
{
    tic_core* core = getFiclCore(vm);
    if (!core) return;
    s32 y = cell_to_int(vmPop(vm));
    s32 x = cell_to_int(vmPop(vm));
    u8 val = tic_api_mget((tic_mem*)core, x, y);
    vmPush(vm, int_to_cell(val));
}

static void ficl_mset(FICL_VM* vm)
{
    tic_core* core = getFiclCore(vm);
    if (!core) return;
    s32 y = cell_to_int(vmPop(vm));
    s32 x = cell_to_int(vmPop(vm));
    s32 v = cell_to_int(vmPop(vm));
    tic_api_mset((tic_mem*)core, x, y, v);
}

static void ficl_fget(FICL_VM* vm)
{
    tic_core* core = getFiclCore(vm);
    if (!core) return;
    s32 flag = cell_to_int(vmPop(vm));
    s32 id = cell_to_int(vmPop(vm));
    u8 val = tic_api_fget((tic_mem*)core, id, flag);
    vmPush(vm, int_to_cell(val));
}

static void ficl_fset(FICL_VM* vm)
{
    tic_core* core = getFiclCore(vm);
    if (!core) return;
    s32 flag = cell_to_int(vmPop(vm));
    s32 id = cell_to_int(vmPop(vm));
    s32 v = cell_to_int(vmPop(vm));
    tic_api_fset((tic_mem*)core, id, flag, v);
}

// Input API wrappers
static void ficl_btn(FICL_VM *vm)
{
    tic_core* core = getFiclCore(vm);
    if (!core) return;
    s32 id = cell_to_int(vmPop(vm));
    bool res = tic_api_btn((tic_mem*)core, id);
    vmPush(vm, int_to_cell(res ? 1 : 0));
}

static void ficl_btnp(FICL_VM *vm)
{
    tic_core* core = getFiclCore(vm);
    if (!core) return;
    s32 hold = cell_to_int(vmPop(vm));
    s32 period = cell_to_int(vmPop(vm));
    s32 id = cell_to_int(vmPop(vm));
    bool res = tic_api_btnp((tic_mem*)core, id, period, hold);
    vmPush(vm, int_to_cell(res ? 1 : 0));
}

static void ficl_key(FICL_VM *vm)
{
    tic_core* core = getFiclCore(vm);
    if (!core) return;
    s32 code = cell_to_int(vmPop(vm));
    bool res = tic_api_key((tic_mem*)core, code);
    vmPush(vm, int_to_cell(res ? 1 : 0));
}

static void ficl_keyp(FICL_VM *vm)
{
    tic_core* core = getFiclCore(vm);
    if (!core) return;
    s32 hold = cell_to_int(vmPop(vm));
    s32 period = cell_to_int(vmPop(vm));
    s32 code = cell_to_int(vmPop(vm));
    bool res = tic_api_keyp((tic_mem*)core, code, period, hold);
    vmPush(vm, int_to_cell(res ? 1 : 0));
}

static void ficl_mouse(FICL_VM *vm)
{
    tic_core* core = getFiclCore(vm);
    if (!core) return;
    tic_mem* tic = (tic_mem*)core;
    vmPush(vm, int_to_cell(tic->ram->input.mouse.y));
    vmPush(vm, int_to_cell(tic->ram->input.mouse.x));
    vmPush(vm, int_to_cell(tic->ram->input.mouse.buttons));
}

// Memory API wrappers
static void ficl_peek(FICL_VM *vm)
{
    tic_core* core = getFiclCore(vm);
    if (!core) return;
    s32 address = cell_to_int(vmPop(vm));
    u8 res = tic_api_peek((tic_mem*)core, 0, address);
    vmPush(vm, int_to_cell(res));
}

static void ficl_poke(FICL_VM *vm)
{
    tic_core* core = getFiclCore(vm);
    if (!core) return;
    s32 value = cell_to_int(vmPop(vm));
    s32 address = cell_to_int(vmPop(vm));
    tic_api_poke((tic_mem*)core, 0, address, value);
}

static void ficl_memcpy(FICL_VM *vm)
{
    tic_core* core = getFiclCore(vm);
    if (!core) return;
    s32 size = cell_to_int(vmPop(vm));
    s32 src = cell_to_int(vmPop(vm));
    s32 dst = cell_to_int(vmPop(vm));
    tic_api_memcpy((tic_mem*)core, dst, src, size);
}

static void ficl_memset(FICL_VM *vm)
{
    tic_core* core = getFiclCore(vm);
    if (!core) return;
    s32 size = cell_to_int(vmPop(vm));
    s32 val = cell_to_int(vmPop(vm));
    s32 dst = cell_to_int(vmPop(vm));
    tic_api_memset((tic_mem*)core, dst, val, size);
}

static void ficl_btnp(FICL_VM *vm)
{
    tic_mem* tic = (tic_mem*)ficlGetUserPointer(vm);
    if (!tic) return;
    s32 hold = cell_to_int(vmPop(vm));
    s32 period = cell_to_int(vmPop(vm));
    s32 id = cell_to_int(vmPop(vm));
    bool res = tic_api_btnp(tic, id, period, hold);
    vmPush(vm, int_to_cell(res ? 1 : 0));
}

// Memory API wrappers
static void ficl_peek(FICL_VM *vm)
{
    tic_mem* tic = (tic_mem*)ficlGetUserPointer(vm);
    if (!tic) return;
    s32 address = cell_to_int(vmPop(vm));
    u8 res = tic_api_peek(tic, 0, address);  // slot 0
    vmPush(vm, int_to_cell(res));
}

static void ficl_poke(FICL_VM *vm)
{
    tic_mem* tic = (tic_mem*)ficlGetUserPointer(vm);
    if (!tic) return;
    s32 value = cell_to_int(vmPop(vm));
    s32 address = cell_to_int(vmPop(vm));
    tic_api_poke(tic, 0, address, value);  // slot 0
}

// Math API wrappers
static void ficl_time(FICL_VM *vm)
{
    tic_core* core = getFiclCore(vm);
    if (!core) return;
    s32 res = tic_api_time((tic_mem*)core);
    vmPush(vm, int_to_cell(res));
}

static void ficl_tstamp(FICL_VM *vm)
{
    tic_core* core = getFiclCore(vm);
    if (!core) return;
    s32 res = core->api.tstamp((tic_mem*)core);
    vmPush(vm, int_to_cell(res));
}

// Sound API wrappers
static void ficl_music_star(FICL_VM *vm)
{
    tic_core* core = getFiclCore(vm);
    if (!core) return;
    s32 speed = cell_to_int(vmPop(vm));
    s32 tempo = cell_to_int(vmPop(vm));
    bool sustain = cell_to_int(vmPop(vm));
    bool loop = cell_to_int(vmPop(vm));
    s32 row = cell_to_int(vmPop(vm));
    s32 frame = cell_to_int(vmPop(vm));
    s32 index = cell_to_int(vmPop(vm));
    tic_api_music((tic_mem*)core, index, frame, row, loop, sustain, tempo, speed);
}

static void ficl_sfx_star(FICL_VM *vm)
{
    tic_core* core = getFiclCore(vm);
    if (!core) return;
    tic_mem* tic = (tic_mem*)core;

    s32 speed = SFX_DEF_SPEED;
    s32 right = MAX_VOLUME;
    s32 left = MAX_VOLUME;
    s32 channel = 0;
    s32 duration = -1;
    s32 octave = -1;
    s32 note = -1;

    s32 top = 0;
    FICL_STACK* stack = vm->pStack;
    if (stack) top = ficlStackDepth(stack);

    if (top >= 1)
    {
        s32 index = cell_to_int(vmPop(vm));

        if (index < SFX_COUNT && index >= 0)
        {
            tic_sample* effect = tic->ram->sfx.samples.data + index;
            note = effect->note;
            octave = effect->octave;
            speed = effect->speed;
        }

        if (top >= 2) note = cell_to_int(vmPop(vm));
        if (top >= 3) duration = cell_to_int(vmPop(vm));
        if (top >= 4) channel = cell_to_int(vmPop(vm));
        if (top >= 5)
        {
            left = cell_to_int(vmPop(vm));
            right = left;
        }
        if (top >= 6) speed = cell_to_int(vmPop(vm));

        tic_api_sfx(tic, index, note, octave, duration, channel, left, right, speed);
    }
}

// Other API wrappers
static void ficl_trace(FICL_VM *vm)
{
    tic_core* core = getFiclCore(vm);
    if (!core) return;
    s32 color = cell_to_int(vmPop(vm));
    char* text = (char*)(intptr_t)vmPop(vm).i;
    tic_api_trace((tic_mem*)core, text, color);
}

static void ficl_exit(FICL_VM *vm)
{
    (void)vm;
    tic_core* core = getFiclCore(vm);
    if (!core) return;
    tic_api_exit((tic_mem*)core);
}

static void ficl_reset(FICL_VM *vm)
{
    (void)vm;
    tic_core* core = getFiclCore(vm);
    if (!core) return;
    tic_api_reset((tic_mem*)core);
}

static void ficl_sync(FICL_VM *vm)
{
    tic_core* core = getFiclCore(vm);
    if (!core) return;
    bool toCart = cell_to_int(vmPop(vm));
    s32 bank = cell_to_int(vmPop(vm));
    s32 mask = cell_to_int(vmPop(vm));
    tic_api_sync((tic_mem*)core, mask, bank, toCart);
}

static void ficl_vbank(FICL_VM *vm)
{
    tic_core* core = getFiclCore(vm);
    if (!core) return;
    s32 bank = cell_to_int(vmPop(vm));
    tic_api_vbank((tic_mem*)core, bank);
}

static void ficl_pmem(FICL_VM *vm)
{
    tic_core* core = getFiclCore(vm);
    if (!core) return;
    tic_mem* tic = (tic_mem*)core;
    s32 index = cell_to_int(vmPop(vm));
    bool write = cell_to_int(vmPop(vm));

    if (write)
    {
        s32 val = cell_to_int(vmPop(vm));
        core->api.pmem(tic, index, val, true);
    }
    else
    {
        u32 res = core->api.pmem(tic, index, 0, false);
        vmPush(vm, int_to_cell(res));
    }
}

static void ficl_register_api(FICL_SYSTEM* sys)
{
    // Graphics
    ficlBuild(sys, "cls", ficl_cls, 0);
    ficlBuild(sys, "pix", ficl_pix, 0);
    ficlBuild(sys, "line", ficl_line, 0);
    ficlBuild(sys, "rect", ficl_rect, 0);
    ficlBuild(sys, "rectb", ficl_rectb, 0);
    ficlBuild(sys, "circ", ficl_circ, 0);
    ficlBuild(sys, "circb", ficl_circb, 0);
    ficlBuild(sys, "elli", ficl_elli, 0);
    ficlBuild(sys, "ellib", ficl_ellib, 0);
    ficlBuild(sys, "tri", ficl_tri, 0);
    ficlBuild(sys, "trib", ficl_trib, 0);
    ficlBuild(sys, "clip", ficl_clip, 0);
    ficlBuild(sys, "paint", ficl_paint, 0);
    ficlBuild(sys, "spr*", ficl_spr_star, 0);
    ficlBuild(sys, "map*", ficl_map_star, 0);
    ficlBuild(sys, "mget", ficl_mget, 0);
    ficlBuild(sys, "mset", ficl_mset, 0);
    ficlBuild(sys, "fget", ficl_fget, 0);
    ficlBuild(sys, "fset", ficl_fset, 0);
    // Input
    ficlBuild(sys, "btn", ficl_btn, 0);
    ficlBuild(sys, "btnp", ficl_btnp, 0);
    ficlBuild(sys, "key", ficl_key, 0);
    ficlBuild(sys, "keyp", ficl_keyp, 0);
    ficlBuild(sys, "mouse", ficl_mouse, 0);
    // Memory
    ficlBuild(sys, "peek", ficl_peek, 0);
    ficlBuild(sys, "poke", ficl_poke, 0);
    ficlBuild(sys, "memcpy", ficl_memcpy, 0);
    ficlBuild(sys, "memset", ficl_memset, 0);
    // Math
    ficlBuild(sys, "time", ficl_time, 0);
    ficlBuild(sys, "tstamp", ficl_tstamp, 0);
    // Sound
    ficlBuild(sys, "sfx*", ficl_sfx_star, 0);
    ficlBuild(sys, "music*", ficl_music_star, 0);
    // Other
    ficlBuild(sys, "trace", ficl_trace, 0);
    ficlBuild(sys, "exit", ficl_exit, 0);
    ficlBuild(sys, "reset", ficl_reset, 0);
    ficlBuild(sys, "sync", ficl_sync, 0);
    ficlBuild(sys, "vbank", ficl_vbank, 0);
    ficlBuild(sys, "pmem", ficl_pmem, 0);
}

// A small prelude with common functions & building on the C API
static const char* TIC80Prelude =
    "\\ TIC-80 Ficl Prelude\n"
    "0 CONSTANT NO-FLIP\n"
    "0 CONSTANT NO-ROT\n"
    "1 CONSTANT SCALE-1\n"
    "-1 CONSTANT ALL-COLORS\n"
    "0 CONSTANT NO-BORDER\n"
    ": 2dup ( a b -- a b a b ) over over ;\n"
    ": 2drop ( a b -- ) drop drop ;\n"
    ": nip ( a b -- b ) swap drop ;\n"
    ": tuck ( a b -- b a b ) swap over ;\n"
    ": spr ( id x y -- ) 1 1 SCALE-1 NO-FLIP NO-ROT 0 0 spr* ;\n"
    ": spr[] ( id x y w h -- ) SCALE-1 NO-FLIP NO-ROT 0 0 spr* ;\n"
    ": spr-scale ( id x y scale -- ) 1 1 swap NO-FLIP NO-ROT 0 0 spr* ;\n"
    ": spr-flip ( id x y flip -- ) 1 1 SCALE-1 swap NO-ROT 0 0 spr* ;\n"
    ": spr-rot ( id x y rot -- ) 1 1 SCALE-1 NO-FLIP swap 0 0 spr* ;\n"
    ": map ( x y -- ) 30 17 0 0 SCALE-1 0 0 map* ;\n"
    ": map[] ( x y w h sx sy -- ) SCALE-1 0 0 map* ;\n"
    ": map-scale ( x y scale -- ) 30 17 0 0 swap 0 0 map* ;\n"
    ": sfx ( index -- ) sfx* ;\n"
    ": sfx-note ( index note -- ) sfx* ;\n"
    ": sfx-play ( index note duration channel -- ) sfx* ;\n"
    ": music ( index -- ) music* ;\n"
    ": music-loop ( index -- ) -1 0 0 -1 0 music* ;\n"
    ": pressed? ( id -- f ) btn ;\n"
    ": pressedp? ( id -- f ) 0 0 btnp ;\n"
    ": cls0 ( -- ) 0 cls ;\n"
    ": cls-bg ( col -- ) cls ;\n"
    ": pixel ( x y col -- ) pix ;\n"
    ;

static bool initFicl(tic_mem* tic, const char* code)
{
    tic_core* core = (tic_core*)tic;

    // Close any existing VMs/systems
    closeFicl(tic);

    // Compute a best-guess dictionary size
    size_t cartridge_bytes = code ? strlen(code) : 0;
    size_t cell_size = sizeof(CELL);
    size_t dict_cells = (cartridge_bytes / cell_size) * 2;

    const size_t min_cells = 4096;
    const size_t max_cells = 32768;
    if (dict_cells < min_cells) {
        dict_cells = min_cells;
    }
    else if (dict_cells > max_cells)
    {
        dict_cells = max_cells;
    }

    // Create Ficl system
    FICL_SYSTEM* sys = ficlInitSystem((int)dict_cells);
    if (!sys)
    {
       core->data->error(core->data->data, "Ficl: failed to create system");
       return false;
    }
    else
    {
        core->ficlSystem = sys;
    }

    // Create Ficl VM
    FICL_VM* vm = ficlNewVM(sys);
    if (!vm)
    {
        core->data->error(core->data->data, "Ficl: failed to create VM");
        ficlTermSystem(sys);
        core->ficlSystem = NULL;
        return false;
    }
    else
    {
        core->currentVM = vm;
    }

    // Evaluate our prelude
    ficlEvaluate(vm, (char*)TIC80Prelude);

    // Attach TIC-80 memory to VM
    ficlSetUserPointer(vm, tic);

    // Register API
    ficl_register_api(sys);

    // Load cartridge
    if (code && *code)
    {
        int result = ficlEvaluate(vm, (char*)code);
        if (result != 0)
        {
            core->data->error(core->data->data, "Ficl: failed to load cartridge");
            closeFicl(tic);
            return false;
        }
    }

    return true;
}

static void closeFicl(tic_mem* tic)
{
    tic_core* core = (tic_core*)tic;

    if (core->currentVM)
    {
        ficlFreeVM(core->currentVM);
        core->currentVM = NULL;
    }

    if (core->ficlSystem)
    {
        ficlTermSystem(core->ficlSystem);
        core->ficlSystem = NULL;
    }
}

static void tickFicl(tic_mem* tic)
{
    tic_core* core = (tic_core*)tic;
    FICL_VM* vm = (FICL_VM*)core->currentVM;
    if (!vm) return;

    FICL_SYSTEM* sys = core->ficlSystem;
    if (!sys) return;

    FICL_WORD* tic_word = ficlLookup(sys, "TIC");
    if (tic_word)
    {
        vmExecute(vm, tic_word);
    }
}

static void bootFicl(tic_mem* tic)
{
    tic_core* core = (tic_core*)tic;
    FICL_VM* vm = (FICL_VM*)core->currentVM;
    if (!vm) return;

    FICL_SYSTEM* sys = core->ficlSystem;
    if (!sys) return;

    FICL_WORD* boot_word = ficlLookup(sys, "BOOT");
    if (boot_word)
    {
        vmExecute(vm, boot_word);
    }
}

static const tic_outline_item* getFiclOutline(const char* code, s32* size)
{
    *size = 0;
    return NULL;
}

static void evalFicl(tic_mem* tic, const char* code)
{
    tic_core* core = (tic_core*)tic;
    FICL_VM* vm = (FICL_VM*)core->currentVM;

    if (!vm || !code || !*code) return;

    int rc = ficlEvaluate(vm, (char*)code);
    if (rc != 0)
    {
        core->data->error(core->data->data, "Ficl: evaluation error");
    }
}

static void scanline_Ficl(tic_mem* memory, s32 row, void* data)
{
    (void)memory;
    (void)row;
    (void)data;
}

static void borderFicl(tic_mem* memory, s32 row, void* data)
{
    (void)memory;
    (void)row;
    (void)data;
}

static void menuFicl(tic_mem* memory, s32 index, void* data)
{
    (void)memory;
    (void)index;
    (void)data;
}

TIC_EXPORT const tic_script EXPORT_SCRIPT(Ficl) =
{
    .id                 = 21,
    .name               = "ficl",
    .fileExtension      = ".ficl",
    .projectComment     = "\\",
    {
        .init           = initFicl,
        .close          = closeFicl,
        .tick           = tickFicl,
        .boot           = bootFicl,

        .callback       =
        {
            .scanline   = scanline_Ficl,
            .border     = borderFicl,
            .menu       = menuFicl,
        },
    },

    .getOutline         = getFiclOutline,
    .eval               = evalFicl,

    .blockCommentStart  = NULL,
    .blockCommentEnd    = NULL,
    .blockCommentStart2 = NULL,
    .blockCommentEnd2   = NULL,
    .singleComment      = "\\",
    .blockStringStart   = "S\"",
    .blockStringEnd     = "\"",
    .stdStringStartEnd  = "\"",

    .keywords           = FiclKeywords,
    .keywordsCount      = COUNT_OF(FiclKeywords),

    .lang_isalnum       = NULL,
    .useStructuredEdition = false,
    .useBinarySection   = false,
};
