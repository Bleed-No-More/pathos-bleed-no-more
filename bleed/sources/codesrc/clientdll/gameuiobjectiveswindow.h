/*
===============================================
Pathos Engine - Created by Andrew Stephen "Overfloater" Lucas

Copyright 2016
All Rights Reserved.
===============================================
*/

#ifndef GAMEUIOBJECTIVESWINDOW_H
#define GAMEUIOBJECTIVESWINDOW_H

#include "gameuielements.h"

enum objectivesbuttonevent_t
{
	OBJ_BUTTON_EXIT = 0,
	OBJ_BUTTON_0,
};

/*
====================
CGameUIObjectivesWindow

====================
*/
class CGameUIObjectivesWindow : public CGameUIWindow
{
public:
	// Max number of objective buttons
	static const Uint32 MAX_NB_OBJECTIVE_BUTTONS;

public:
	struct button_t
	{
		button_t():
			pButton(nullptr),
			pDescription(nullptr)
			{
			}

		// Objective name
		CString objectiveName;

		// Pointer to button
		CGameUIButton* pButton;
		// Pointer to description
		CGameUITextTab* pDescription;
	};

public:
	// Title text object name
	static const Char OBJECTIVESWINDOW_OBJ_NAME[];
	// Title text object name
	static const Char OBJECTIVESWINDOW_TITLE_TEXT_OBJ_NAME[];
	// Button tab object name
	static const Char OBJECTIVESWINDOW_BUTTON_TAB_OBJ_NAME[];
	// Info tab object name
	static const Char OBJECTIVESWINDOW_INFO_TAB_OBJ_NAME[];
	// Upper separator object name
	static const Char OBJECTIVESWINDOW_UPPER_SEPARATOR_OBJ_NAME[];
	// Lower separator object name
	static const Char OBJECTIVESWINDOW_LOWER_SEPARATOR_OBJ_NAME[];
	// Objective button object name
	static const Char OBJECTIVESWINDOW_OBJECTIVE_BUTTON_OBJ_NAME[];
	// Objective text tab object name
	static const Char OBJECTIVESWINDOW_OBJECTIVE_TEXT_TAB_OBJ_NAME[];
	// Default description text tab object name
	static const Char OBJECTIVESWINDOW_DEFAULT_DESCRIPTION_TEXT_TAB_OBJ_NAME[];
	// Exit button object name
	static const Char OBJECTIVESWINDOW_EXIT_BUTTON_OBJ_NAME[];
	// Window description file
	static const Char OBJECTIVESWINDOW_DESC_FILE[];
	// Script subfolder name
	static const Char OBJECTIVESWINDOW_SCRIPT_SUBFOLDER_NAME[];
	// Base script name
	static const Char OBJECTIVESWINDOW_SCRIPT_NAME[];

public:
	CGameUIObjectivesWindow( Int32 flags, Int32 originX, Int32 originY, Uint32 width, Uint32 height );
	~CGameUIObjectivesWindow();

public:
	// Initializes the window
	bool init( const ui_windowdescription_t* pWindowDesc, const ui_objectinfo_t* pWindowObject ) override;
	// Think function for window
	void think( void ) override;

	// Initializes the data
	bool initData( const CArray<CString>& objectivesArray, const Char* pstrActiveObjectiveName, Int32& newObjectivesBitflags );
	// Returns the current window information
	void getInformation( CArray<CString>& objectivesArray, CString& activeObjectiveName, Int32& newObjectivesBitflags ) const;
	// Returns the type of the window
	virtual gameui_windows_t getWindowType( void ) const override { return GAMEUI_OBJECTIVESWINDOW; }

public:
	// Manages an event
	void ManageEvent( objectivesbuttonevent_t event );

	// Creates instance of window
	static CGameUIObjectivesWindow* CreateInstance( void );

private:
	// Title text
	CGameUIText* m_pWindowTitleText;
	// Default description
	CGameUIText* m_pDefaultDescription;
	// Array of buttons
	CArray<button_t> m_buttonsArray;
	// Currently selected objective button
	button_t* m_pCurrentObjective;
	// Bitflags marking which entries are new
	Int32 m_newObjectivesBitflags;

	// Button tab
	CGameUIScrollableSurface* m_pButtonsTab;
	// Info tab
	CGameUISurface* m_pInfoTab;

	// Font set used
	const font_set_t* m_pFontSet;
};

/*
=================================
CGameUIObjectivesWindowExitCallbackEvent

=================================
*/
class CGameUIObjectivesWindowCallbackEvent : public CGameUICallbackEvent
{
public:
	CGameUIObjectivesWindowCallbackEvent( CGameUIObjectivesWindow* pWindow, Uint32 type ):
		m_pWindow(pWindow),
		m_eventType((objectivesbuttonevent_t)type)
	{ 
	};
	virtual ~CGameUIObjectivesWindowCallbackEvent( void ) { };

public:
	// Performs the action
	virtual void PerformAction( Float param ) override;
	// Handles a special key event
	virtual bool KeyEvent( Int32 button, Int16 mod, bool keyDown ) override { return false; }
	// Handles a mouse button event
	virtual bool MouseButtonEvent( Int32 mouseX, Int32 mouseY, Int32 button, bool keyDown ) override { return false; }

private:
	// Text window object
	CGameUIObjectivesWindow* m_pWindow;
	// Event type
	objectivesbuttonevent_t m_eventType;
};

#endif //GAMEUIOBJECTIVESWINDOW_H
