/*
===============================================
Pathos Engine - Created by Andrew Stephen "Overfloater" Lucas

Copyright 2016
All Rights Reserved.
===============================================
*/

#ifndef GAMEUISUBWAYWINDOW_H
#define GAMEUISUBWAYWINDOW_H

#include "gameuielements.h"

enum subwaybuttonschematype_t
{
	SUBWAYWINDOW_BUTTON_DESTINATION_1 = 0,
	SUBWAYWINDOW_BUTTON_DESTINATION_2,
	SUBWAYWINDOW_BUTTON_DESTINATION_3,
	SUBWAYWINDOW_BUTTON_DESTINATION_4,
	SUBWAYWINDOW_BUTTON_UNAVAILABLE,
	SUBWAYWINDOW_BUTTON_DISABLED,

	NB_SUBWAY_BUTTON_TYPES
};

enum subwaybuttonevent_t
{
	SUBWAY_BUTTON_1 = 0,
	SUBWAY_BUTTON_2,
	SUBWAY_BUTTON_3,
	SUBWAY_BUTTON_4,
	SUBWAY_BUTTON_CANCEL
};

/*
====================
CGameUISubwayWindow

====================
*/
class CGameUISubwayWindow : public CGameUIWindow
{
public:
	// Number of destination buttons
	static const Uint32 NB_DESTINATION_BUTTONS;

public:
	struct subwaybuttonschema_t
	{
		CString buttontext;
		CString description;
		CString destinationid;
	};
	struct subwaybutton_t
	{
		subwaybutton_t():
			pButton(nullptr),
			pDescription(nullptr)
			{
			}

		// Pointer to button
		CGameUIButton* pButton;
		// Pointer to description
		CGameUIText* pDescription;
		// Destination id
		CString destinationid;
	};

public:
	// Window description file
	static const Char SUBWAYWINDOW_DESC_FILE[];
	// Title text object name
	static const Char SUBWAYWINDOW_OBJ_NAME[];
	// Title text object name
	static const Char SUBWAYWINDOW_TITLE_TEXT_OBJ_NAME[];
	// Upper separator object name
	static const Char SUBWAYWINDOW_UPPER_SEPARATOR_OBJ_NAME[];
	// Middle separator object name
	static const Char SUBWAYWINDOW_MIDDLE_SEPARATOR_OBJ_NAME[];
	// Lower separator object name
	static const Char SUBWAYWINDOW_LOWER_SEPARATOR_OBJ_NAME[];
	// Destination button object name
	static const Char SUBWAYWINDOW_DESTINATION_BUTTON_OBJ_NAME[];
	// Destination text object name
	static const Char SUBWAYWINDOW_DESTINATION_TEXT_OBJ_NAME[];
	// Default text object name
	static const Char SUBWAYWINDOW_DEFAULT_TEXT_OBJ_NAME[];
	// Exit button object name
	static const Char SUBWAYWINDOW_EXIT_BUTTON_OBJ_NAME[];

public:
	CGameUISubwayWindow( Int32 flags, Int32 originX, Int32 originY, Uint32 width, Uint32 height );
	~CGameUISubwayWindow();

public:
	// Initializes the window
	bool init( const ui_windowdescription_t* pWindowDesc, const ui_objectinfo_t* pWindowObject ) override;
	// Think function for window
	void think( void ) override;

	// Initializes the data
	bool initData( const Char* pstrScriptFile, Int32 flags, Int32 subwayLineIndex );
	// Returns the current window information
	void getInformation( CString& scriptfile, Int32& flags, Int32& subwayLineIndex ) const;
	// Returns the type of the window
	virtual gameui_windows_t getWindowType( void ) const override { return GAMEUI_SUBWAYWINDOW; }

public:
	// Manages an event
	void ManageEvent( subwaybuttonevent_t event );

	// Creates instance of window
	static CGameUISubwayWindow* CreateInstance( void );

private:
	// Title text
	CGameUIText* m_pWindowTitleText;
	// Default description
	CGameUIText* m_pDefaultDescription;
	// Exit button
	CGameUIButton* m_pExitButton;
	// Array of buttons
	CArray<subwaybutton_t> m_buttonsArray;

	// Text file path
	CString m_scriptFilePath;
	// Flags
	Int32 m_subwayFlags;
	// Subway line index
	Int32 m_subwayLineIndex;
};

/*
=================================
CGameUISubwayWindowExitCallbackEvent

=================================
*/
class CGameUISubwayWindowCallbackEvent : public CGameUICallbackEvent
{
public:
	CGameUISubwayWindowCallbackEvent( CGameUISubwayWindow* pLoginWindow, Uint32 type ):
		m_pSubwayWindow(pLoginWindow),
		m_eventType((subwaybuttonevent_t)type)
	{ 
	};
	virtual ~CGameUISubwayWindowCallbackEvent( void ) { };

public:
	// Performs the action
	virtual void PerformAction( Float param ) override;
	// Handles a special key event
	virtual bool KeyEvent( Int32 button, Int16 mod, bool keyDown ) override { return false; }
	// Handles a mouse button event
	virtual bool MouseButtonEvent( Int32 mouseX, Int32 mouseY, Int32 button, bool keyDown ) override { return false; }

private:
	// Text window object
	CGameUISubwayWindow* m_pSubwayWindow;
	// Event type
	subwaybuttonevent_t m_eventType;
};

#endif //GAMEUISUBWAYWINDOW_H