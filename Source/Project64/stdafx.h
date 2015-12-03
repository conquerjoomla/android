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

#pragma warning(disable:4247)
#pragma warning(disable:4786)

#include <common/MemTest.h>
#include <common/CriticalSection.h>
#include <shellapi.h>
#include <Project64-core\Multilanguage.h>
#include "UserInterface.h"
#include "N64System.h"
#include <Project64-core\Plugin.h>
#include "Support.h"
#include <Project64-core\Version.h>
#include <windows.h>
#include <mmsystem.h>
#include <Aclapi.h>

#include <Project64-core/3rdParty/Zip.h>
#include <Project64-core/3rdParty/7zip.h>
