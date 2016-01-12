/****************************************************************************
*                                                                           *
* Project64 - A Nintendo 64 emulator.                                       *
* http://www.pj64-emu.com/                                                  *
* Copyright (C) 2016 Project64. All rights reserved.                        *
*                                                                           *
* License:                                                                  *
* GNU/GPLv2 http://www.gnu.org/licenses/gpl-2.0.html                        *
*                                                                           *
****************************************************************************/
#include "stdafx.h"

#include "alist.h"

enum { NAUDIO_COUNT = 0x170 }; /* ie 184 samples */
enum {
    NAUDIO_MAIN = 0x4f0,
    NAUDIO_MAIN2 = 0x660,
    NAUDIO_DRY_LEFT = 0x9d0,
    NAUDIO_DRY_RIGHT = 0xb40,
    NAUDIO_WET_LEFT = 0xcb0,
    NAUDIO_WET_RIGHT = 0xe20
};

/* audio commands definition */
static void UNKNOWN(CHle * hle, uint32_t w1, uint32_t w2)
{
    uint8_t acmd = (w1 >> 24);

    hle->WarnMessage("Unknown audio command %d: %08x %08x", acmd, w1, w2);
}

static void SPNOOP(struct hle_t* UNUSED(hle), uint32_t UNUSED(w1), uint32_t UNUSED(w2))
{
}

static void NAUDIO_0000(CHle * hle, uint32_t w1, uint32_t w2)
{
    /* ??? */
    UNKNOWN(hle, w1, w2);
}

static void NAUDIO_02B0(CHle * hle, uint32_t UNUSED(w1), uint32_t w2)
{
    /* emulate code at 0x12b0 (inside SETVOL), because PC always execute in IMEM */
    hle->alist_naudio().rate[1] &= ~0xffff;
    hle->alist_naudio().rate[1] |= (w2 & 0xffff);
}

static void SETVOL(CHle * hle, uint32_t w1, uint32_t w2)
{
    uint8_t flags = (w1 >> 16);

    if (flags & 0x4) {
        if (flags & 0x2) {
            hle->alist_naudio().vol[0] = w1;
            hle->alist_naudio().dry = (w2 >> 16);
            hle->alist_naudio().wet = w2;
        }
        else {
            hle->alist_naudio().target[1] = w1;
            hle->alist_naudio().rate[1] = w2;
        }
    }
    else
    {
        hle->alist_naudio().target[0] = w1;
        hle->alist_naudio().rate[0] = w2;
    }
}

static void ENVMIXER(CHle * hle, uint32_t w1, uint32_t w2)
{
    uint8_t  flags = (w1 >> 16);
    uint32_t address = (w2 & 0xffffff);

    hle->alist_naudio().vol[1] = w1;

    alist_envmix_lin(
        hle,
        flags & 0x1,
        NAUDIO_DRY_LEFT,
        NAUDIO_DRY_RIGHT,
        NAUDIO_WET_LEFT,
        NAUDIO_WET_RIGHT,
        NAUDIO_MAIN,
        NAUDIO_COUNT,
        hle->alist_naudio().dry,
        hle->alist_naudio().wet,
        hle->alist_naudio().vol,
        hle->alist_naudio().target,
        hle->alist_naudio().rate,
        address);
}

static void CLEARBUFF(CHle * hle, uint32_t w1, uint32_t w2)
{
    uint16_t dmem = w1 + NAUDIO_MAIN;
    uint16_t count = w2;

    alist_clear(hle, dmem, count);
}

static void MIXER(CHle * hle, uint32_t w1, uint32_t w2)
{
    int16_t  gain = w1;
    uint16_t dmemi = (w2 >> 16) + NAUDIO_MAIN;
    uint16_t dmemo = w2 + NAUDIO_MAIN;

    alist_mix(hle, dmemo, dmemi, NAUDIO_COUNT, gain);
}

static void LOADBUFF(CHle * hle, uint32_t w1, uint32_t w2)
{
    uint16_t count = (w1 >> 12) & 0xfff;
    uint16_t dmem = (w1 & 0xfff) + NAUDIO_MAIN;
    uint32_t address = (w2 & 0xffffff);

    alist_load(hle, dmem, address, count);
}

static void SAVEBUFF(CHle * hle, uint32_t w1, uint32_t w2)
{
    uint16_t count = (w1 >> 12) & 0xfff;
    uint16_t dmem = (w1 & 0xfff) + NAUDIO_MAIN;
    uint32_t address = (w2 & 0xffffff);

    alist_save(hle, dmem, address, count);
}

static void LOADADPCM(CHle * hle, uint32_t w1, uint32_t w2)
{
    uint16_t count = w1;
    uint32_t address = (w2 & 0xffffff);

    dram_load_u16(hle, (uint16_t*)hle->alist_naudio().table, address, count >> 1);
}

static void DMEMMOVE(CHle * hle, uint32_t w1, uint32_t w2)
{
    uint16_t dmemi = w1 + NAUDIO_MAIN;
    uint16_t dmemo = (w2 >> 16) + NAUDIO_MAIN;
    uint16_t count = w2;

    alist_move(hle, dmemo, dmemi, (count + 3) & ~3);
}

static void SETLOOP(CHle * hle, uint32_t UNUSED(w1), uint32_t w2)
{
    hle->alist_naudio().loop = (w2 & 0xffffff);
}

static void ADPCM(CHle * hle, uint32_t w1, uint32_t w2)
{
    uint32_t address = (w1 & 0xffffff);
    uint8_t  flags = (w2 >> 28);
    uint16_t count = (w2 >> 16) & 0xfff;
    uint16_t dmemi = ((w2 >> 12) & 0xf) + NAUDIO_MAIN;
    uint16_t dmemo = (w2 & 0xfff) + NAUDIO_MAIN;

    alist_adpcm(
        hle,
        flags & 0x1,
        flags & 0x2,
        false,          /* unsuported by this ucode */
        dmemo,
        dmemi,
        (count + 0x1f) & ~0x1f,
        hle->alist_naudio().table,
        hle->alist_naudio().loop,
        address);
}

static void RESAMPLE(CHle * hle, uint32_t w1, uint32_t w2)
{
    uint32_t address = (w1 & 0xffffff);
    uint8_t  flags = (w2 >> 30);
    uint16_t pitch = (w2 >> 14);
    uint16_t dmemi = ((w2 >> 2) & 0xfff) + NAUDIO_MAIN;
    uint16_t dmemo = (w2 & 0x3) ? NAUDIO_MAIN2 : NAUDIO_MAIN;

    alist_resample(
        hle,
        flags & 0x1,
        false,          /* TODO: check which ABI supports it */
        dmemo,
        dmemi,
        NAUDIO_COUNT,
        pitch << 1,
        address);
}

static void INTERLEAVE(CHle * hle, uint32_t UNUSED(w1), uint32_t UNUSED(w2))
{
    alist_interleave(hle, NAUDIO_MAIN, NAUDIO_DRY_LEFT, NAUDIO_DRY_RIGHT, NAUDIO_COUNT);
}

/* global functions */
void alist_process_naudio(CHle * hle)
{
    static const acmd_callback_t ABI[0x10] =
    {
        SPNOOP, ADPCM, CLEARBUFF, ENVMIXER,
        LOADBUFF, RESAMPLE, SAVEBUFF, NAUDIO_0000,
        NAUDIO_0000, SETVOL, DMEMMOVE, LOADADPCM,
        MIXER, INTERLEAVE, NAUDIO_02B0, SETLOOP
    };

    alist_process(hle, ABI, 0x10);
}