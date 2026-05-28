/*
===============================================
Pathos Engine - Created by Andrew Stephen "Overfloater" Lucas

Copyright 2016
All Rights Reserved.
===============================================
*/

#ifndef GAMEUIINVENTORYWINDOW_H
#define GAMEUIINVENTORYWINDOW_H

#include "gameuielements.h"

enum inventorybuttonevent_t
{
	INV_BUTTON_EXIT = 0,
	INV_BUTTON_DROP
};

/*
====================
CGameUIInventoryWindow

====================
*/
class CGameUIInventoryWindow : public CGameUIWindow
{
	struct button_t
	{
		button_t():
			pButton(nullptr)
			{
			}

		// Pointer to button
		CGameUIButton* pButton;
	};

public:
	// Window description file
	static const Char INVENTORYWINDOW_DESC_FILE[];
	// Window object name
	static const Char INVENTORYWINDOW_OBJ_NAME[];
	// Title text object name
	static const Char INVENTORYWINDOW_TITLE_TEXT_OBJ_NAME[];
	// Upper separator object name
	static const Char INVENTORYWINDOW_UPPER_SEPARATOR_OBJ_NAME[];
	// Lower separator object name
	static const Char INVENTORYWINDOW_LOWER_SEPARATOR_OBJ_NAME[];
	// Exit window button object name
	static const Char INVENTORYWINDOW_EXIT_BUTTON_OBJ_NAME[];

public:
	CGameUIInventoryWindow( Int32 flags, Int32 originX, Int32 originY, Uint32 width, Uint32 height );
	~CGameUIInventoryWindow();

public:
	// Initializes the window
	bool init( const ui_windowdescription_t* pWindowDesc, const ui_objectinfo_t* pWindowObject ) override;

	// Initializes the data
	bool initData( Uint32 horizontalRowCount, Uint32 verticalRowCount );
	// Returns the current window information
	void getInformation( void ) const;

	// Returns the type of the window
	virtual gameui_windows_t getWindowType( void ) const override { return GAMEUI_INVENTORYWINDOW; }

public:
	// Manages an event
	void ManageEvent( inventorybuttonevent_t event );

	// Creates instance of window
	static CGameUIInventoryWindow* CreateInstance( void );

private:
	// Exit button
	CGameUIButton* m_pExitButton;

	// Title text object
	CGameUIText* m_pTitleText;
};

/*
=================================
CGameUIInventoryWindowButtonCallbackEvent

=================================
*/
class CGameUIInventoryWindowButtonCallbackEvent : public CGameUICallbackEvent
{
public:
	CGameUIInventoryWindowButtonCallbackEvent( CGameUIInventoryWindow* pWindow, Uint32 type ):
		m_pWindow(pWindow),
		m_eventType((inventorybuttonevent_t)type)
	{ 
	};
	virtual ~CGameUIInventoryWindowButtonCallbackEvent( void ) { };

public:
	// Performs the action
	virtual void PerformAction( Float param ) override;
	// Handles a special key event
	virtual bool KeyEvent( Int32 button, Int16 mod, bool keyDown ) override { return false; }
	// Handles a mouse button event
	virtual bool MouseButtonEvent( Int32 mouseX, Int32 mouseY, Int32 button, bool keyDown ) override { return false; }

private:
	// Text window object
	CGameUIInventoryWindow* m_pWindow;
	// Event type
	inventorybuttonevent_t m_eventType;
};
#endif //GAMEUIINVENTORYWINDOW_H