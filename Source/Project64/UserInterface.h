/****************************************************************************
*                                                                           *
* Project64 - A Nintendo 64 emulator.                                      *
* http://www.pj64-emu.com/                                                  *
* Copyright (C) 2012 Project64. All rights reserved.                        *
*                                                                           *
* License:                                                                  *
* GNU/GPLv2 http://www.gnu.org/licenses/gpl-2.0.html                        *
*                                                                           *
****************************************************************************/
#pragma once

#pragma warning(disable:4786)
#include "Support.h"

#include <Project64-core\Multilanguage.h>
#include <Project64-core\Settings\Settings Class.h>

#ifdef tofix
typedef unsigned char    BYTE;
typedef unsigned short   WORD;
typedef unsigned long    DWORD;
typedef unsigned __int64 QWORD;
typedef void *           HANDLE;
typedef const char *     LPCSTR;

#define CALLBACK    __stdcall

class CN64System;
#endif

#include "WTLApp.h"
#include "UserInterface/MenuShortCuts.h"

#include "UserInterface/Rom Browser.h"
#include "UserInterface/Gui Class.h"
#include "UserInterface/Menu Class.h"
#include "UserInterface/Main Menu Class.h"
#include "UserInterface/Notification Class.h"
#include <Project64-core/N64System/Frame Per Second Class.h>
#include "UserInterface/resource.h"
#include "UserInterface/Settings Config.h"
#include "UserInterface/Cheat Class UI.h"