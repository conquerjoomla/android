/****************************************************************************
*                                                                           *
* Project 64 - A Nintendo 64 emulator.                                      *
* http://www.pj64-emu.com/                                                  *
* Copyright (C) 2012 Project64. All rights reserved.                        *
*                                                                           *
* License:                                                                  *
* GNU/GPLv2 http://www.gnu.org/licenses/gpl-2.0.html                        *
*                                                                           *
****************************************************************************/
#pragma once
#include "Plugin Base.h"

typedef union
{
	uint32_t Value;
	struct
	{
		unsigned R_DPAD : 1;
		unsigned L_DPAD : 1;
		unsigned D_DPAD : 1;
		unsigned U_DPAD : 1;
		unsigned START_BUTTON : 1;
		unsigned Z_TRIG : 1;
		unsigned B_BUTTON : 1;
		unsigned A_BUTTON : 1;

		unsigned R_CBUTTON : 1;
		unsigned L_CBUTTON : 1;
		unsigned D_CBUTTON : 1;
		unsigned U_CBUTTON : 1;
		unsigned R_TRIG : 1;
		unsigned L_TRIG : 1;
		unsigned Reserved1 : 1;
		unsigned Reserved2 : 1;

		signed   Y_AXIS : 8;

		signed   X_AXIS : 8;
	};
} BUTTONS;

typedef struct
{
	uint32_t Present;
	uint32_t RawData;
	int   Plugin;
} CONTROL;

enum PluginType
{
	PLUGIN_NONE = 1,
	PLUGIN_MEMPAK = 2,
	PLUGIN_RUMBLE_PAK = 3,
	PLUGIN_TANSFER_PAK = 4, // not implemeted for non raw data
	PLUGIN_RAW = 5, // the controller plugin is passed in raw data
};

class CControl_Plugin;

class CCONTROL
{
public:
	CCONTROL(uint32_t &Present, uint32_t &RawData, int32_t &PlugType);
	inline bool  Present(void) const { return m_Present != 0; }
	inline uint32_t Buttons(void) const { return m_Buttons.Value; }
	inline PluginType Plugin(void) const { return static_cast<PluginType>(m_PlugType); }
private:
	friend CControl_Plugin; //controller plugin class has full access

	uint32_t & m_Present;
	uint32_t & m_RawData;
	int      & m_PlugType;
	BUTTONS    m_Buttons;

	CCONTROL(void);                         // Disable default constructor
	CCONTROL(const CCONTROL&);              // Disable copy constructor
	CCONTROL& operator=(const CCONTROL&);   // Disable assignment
};

class CControl_Plugin : public CPlugin
{
public:
	CControl_Plugin(void);
	~CControl_Plugin();

	bool Initiate(CN64System * System, RenderWindow * Window);
	void SetControl(CControl_Plugin const * const Plugin);
	void UpdateKeys(void);

	void(__cdecl *WM_KeyDown)       (uint32_t wParam, uint32_t lParam);
	void(__cdecl *WM_KeyUp)         (uint32_t wParam, uint32_t lParam);
	void(__cdecl *RumbleCommand)	(int Control, int bRumble);
	void(__cdecl *GetKeys)          (int Control, BUTTONS * Keys);
	void(__cdecl *ReadController)   (int Control, uint8_t * Command);
	void(__cdecl *ControllerCommand)(int Control, uint8_t * Command);

	inline CCONTROL const * Controller(int control) { return m_Controllers[control]; }
	inline CONTROL * PluginControllers(void) { return m_PluginControllers; }

private:
	CControl_Plugin(const CControl_Plugin&);			// Disable copy constructor
	CControl_Plugin& operator=(const CControl_Plugin&);	// Disable assignment

	virtual int GetDefaultSettingStartRange() const { return FirstCtrlDefaultSet; }
	virtual int GetSettingStartRange() const { return FirstCtrlSettings; }
	PLUGIN_TYPE type() { return PLUGIN_TYPE_CONTROLLER; }
	bool LoadFunctions(void);
	void UnloadPluginDetails(void);

	bool   m_AllocatedControllers;

	// What the different controls are set up as
	CONTROL m_PluginControllers[4];
	CCONTROL * m_Controllers[4];
};
