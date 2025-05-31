/*
===============================================
Pathos Engine - Created by Andrew Stephen "Overfloater" Lucas

Copyright 2016
All Rights Reserved.
===============================================
*/

#ifndef GAMEUITEXTWINDOW_H
#define GAMEUITEXTWINDOW_H

#include "gameuielements.h"

/*
====================
CGameUITextWindow

====================
*/
class CGameUITextWindow : public CGameUIWindow
{
public:
	// Window description file
	static const Char TEXTWINDOW_DESC_FILE[];
	// Window object name
	static const Char TEXTWINDOW_OBJ_NAME[];
	// Window title text object name
	static const Char TEXTWINDOW_TITLE_TEXT_OBJ_NAME[];
	// Window exit button object name
	static const Char TEXTWINDOW_EXIT_BUTTON_OBJ_NAME[];
	// Window title text tab object name
	static const Char TEXTWINDOW_TITLE_TEXT_TAB_OBJ_NAME[];

private:
	explicit CGameUITextWindow( Int32 flags, Int32 originX, Int32 originY, Uint32 width, Uint32 height );
	~CGameUITextWindow();

public:
	// Initializes the window
	bool init( const ui_windowdescription_t* pWindowDesc, const ui_objectinfo_t* pWindowObject ) override;
	// Initializes the data
	bool initData( const Char* pstrtextfilepath, const Char* pstrPassCode );
	// Returns the current window information
	void getInformation( CString& textfilepath, CString& passcode ) const;
	// Returns the type of the window
	virtual gameui_windows_t getWindowType( void ) const override { return GAMEUI_TEXTWINDOW; }

public:
	// Closes the window
	void CloseWindow( void );

public:
	// Called when the window is removed
	virtual void onRemove( void ) override;

public:
	// Processes a text document and returns it's data
	static bool ProcessTextFile( const Char* pstrFilePath, const CString passCode, CString& titleText, CString& titletextschema, color32_t& titletextcolor, bool& titleColorWasSet, CString& textschema, color32_t& textcolor, bool& textColorWasSet, CString& documentText );
	// Creates instance of window
	static CGameUITextWindow* CreateInstance( void );

private:
	// Title text object
	CGameUIText* m_pTitleText;
	// Text tab object
	CGameUITextTab* m_pTextTab;

	// Text file path
	CString m_textFilePath;
	// Passcode if any
	CString m_passcode;
};

/*
=================================
CGameUITextWindowExitCallbackEvent

=================================
*/
class CGameUITextWindowExitCallbackEvent : public CGameUICallbackEvent
{
public:
	explicit CGameUITextWindowExitCallbackEvent( CGameUITextWindow* pTextWindow ):
		m_pTextWindow(pTextWindow)
	{ 
	};
	virtual ~CGameUITextWindowExitCallbackEvent( void ) { };

public:
	// Performs the action
	virtual void PerformAction( Float param ) override;
	// Handles a special key event
	virtual bool KeyEvent( Int32 button, Int16 mod, bool keyDown ) override { return false; }
	// Handles a mouse button event
	virtual bool MouseButtonEvent( Int32 mouseX, Int32 mouseY, Int32 button, bool keyDown ) override { return false; }

private:
	// Text window object
	CGameUITextWindow* m_pTextWindow;
};

#endif //GAMEUITEXTWINDOW_H