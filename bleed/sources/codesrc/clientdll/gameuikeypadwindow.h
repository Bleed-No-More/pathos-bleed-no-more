/*
===============================================
Pathos Engine - Created by Andrew Stephen "Overfloater" Lucas

Copyright 2016
All Rights Reserved.
===============================================
*/

#ifndef GAMEUIKEYPADWINDOW_H
#define GAMEUIKEYPADWINDOW_H

#include "gameuielements.h"

enum keypadbutton_t
{
	KEYPADWINDOW_BUTTON_CLEAR = 0,
	KEYPADWINDOW_BUTTON_DELETE,
	KEYPADWINDOW_BUTTON_CANCEL,
	KEYPADWINDOW_BUTTON_OK
};

/*
====================
CGameUIKeypadWindow

====================
*/
class CGameUIKeypadWindow : public CGameUIWindow
{
public:
	// Keypad window description filename
	static const Char KEYPADWINDOW_DESC_FILE[];
	// Keypad window description filename
	static const Char KEYPADWINDOW_OBJ_NAME[];
	// Title text object name
	static const Char KEYPADWINDOW_TITLE_TEXT_OBJ_NAME[];
	// Numerical display object name
	static const Char KEYPADWINDOW_NUMERICAL_DISPLAY_OBJ_NAME[];
	// Numerical display text object name
	static const Char KEYPADWINDOW_DISPLAY_TEXT_OBJ_NAME[];
	// Upper separator object name
	static const Char KEYPADWINDOW_UPPER_SEPARATOR_OBJ_NAME[];
	// Button tab object name
	static const Char KEYPADWINDOW_BUTTON_TAB_OBJ_NAME[];
	// Lower separator object name
	static const Char KEYPADWINDOW_LOWER_SEPARATOR_OBJ_NAME[];
	// Notes surface object name
	static const Char KEYPADWINDOW_NOTES_SURFACE_OBJ_NAME[];
	// Notes tab title text object name
	static const Char KEYPADWINDOW_NOTES_TITLE_TEXT_OBJ_NAME[];
	// Notes tab "Passcode" label text object name
	static const Char KEYPADWINDOW_NOTES_PASSCODE_LABEL_TEXT_OBJ_NAME[];
	// Notes tab "Passcode" text object name
	static const Char KEYPADWINDOW_NOTES_PASSCODE_TEXT_OBJ_NAME[];
	// Numerical button object generic name
	static const Char KEYPADWINDOW_NUMERICAL_BUTTON_OBJ_NAME[];
	// "Ok" button object name
	static const Char KEYPADWINDOW_OK_BUTTON_OBJ_NAME[];
	// "Cancel" button object name
	static const Char KEYPADWINDOW_CANCEL_BUTTON_OBJ_NAME[];
	// "Delete" button object name
	static const Char KEYPADWINDOW_DELETE_BUTTON_OBJ_NAME[];
	// "Clear" button object name
	static const Char KEYPADWINDOW_CLEAR_BUTTON_OBJ_NAME[];
	// Prompt text lifetime
	static const Double KEYPADWINDOW_PROMPT_LIFETIME;
	// Default text color
	static const color32_t KEYPADWINDOW_PROMPT_FAIL_TEXT_COLOR;
	// Default text color
	static const color32_t KEYPADWINDOW_PROMPT_SUCCESS_TEXT_COLOR;

public:
	CGameUIKeypadWindow( Int32 flags, Int32 originX, Int32 originY, Uint32 width, Uint32 height );
	~CGameUIKeypadWindow();

public:
	// Initializes the window
	bool init( const ui_windowdescription_t* pWindowDesc, const ui_objectinfo_t* pWindowObject ) override;
	// Initializes the data
	bool initData( const Char* pstrPasscode, const Char* pstrInput, bool stayTillNext );

	// Think function for window
	void think( void ) override;
	// Called when window is removed
	void onRemove( void ) override;
	// Mark for delayed removal
	void setDelayedRemoval( Double delay ) override;

public:
	// Returns the current window information
	void getInformation( CString& passcode, CString& input, bool& stayTillNext ) const;
	// Returns the type of the window
	virtual gameui_windows_t getWindowType( void ) const override { return GAMEUI_KEYPADWINDOW; }

public:
	// Manages a button event
	void ManageButtonEvent( keypadbutton_t event );
	// Manages a digit input event
	void ManageDigitButtonEvent( Char digit );

	// Creates instance of window
	static CGameUIKeypadWindow* CreateInstance( void );

private:
	// Text for the input
	CGameUIText* m_pKeypadDisplayText;
	// Default display text color
	color32_t m_keypadDisplayDefaultColor;
	// Lower separator object
	CGameUIHorizontalSeparator* m_pLowerSeparator;
	// Notes tab object
	CGameUISurface* m_pNotesSurface;
	// Text for passcode itself
	CGameUIText* m_pTextPasscode;

	// Passcode
	CString m_passcode;
	// Input code
	CString m_inputCode;
	// TRUE if login was successful
	bool m_codeAccepted;
	// Time until prompt text resets
	Double m_promptTextTime;
	// TRUE if we should stay till next window spawn
	bool m_stayTillNextWindow;
	// Message send time
	Double m_messageSendTime;
};

/*
=================================
CGameUIKeypadWindowButtonCallbackEvent

=================================
*/
class CGameUIKeypadWindowButtonCallbackEvent : public CGameUICallbackEvent
{
public:
	CGameUIKeypadWindowButtonCallbackEvent( CGameUIKeypadWindow* kKeypadWindow, keypadbutton_t button ):
		m_pKeypadWindow(kKeypadWindow),
		m_button(button)
	{ 
	};
	virtual ~CGameUIKeypadWindowButtonCallbackEvent( void ) { };

public:
	// Performs the action
	virtual void PerformAction( Float param ) override;
	// Handles a special key event
	virtual bool KeyEvent( Int32 button, Int16 mod, bool keyDown ) override { return false; }
	// Handles a mouse button event
	virtual bool MouseButtonEvent( Int32 mouseX, Int32 mouseY, Int32 button, bool keyDown ) override { return false; }

private:
	// Text window object
	CGameUIKeypadWindow* m_pKeypadWindow;
	// Event type
	keypadbutton_t m_button;
};

/*
=================================
CGameUIKeypadWindowDigitCallbackEvent

=================================
*/
class CGameUIKeypadWindowDigitCallbackEvent : public CGameUICallbackEvent
{
public:
	CGameUIKeypadWindowDigitCallbackEvent( CGameUIKeypadWindow* pKeypadWindow, Char digit ):
		m_pKeypadWindow(pKeypadWindow),
		m_digit(digit)
	{ 
	};
	virtual ~CGameUIKeypadWindowDigitCallbackEvent( void ) { };

public:
	// Performs the action
	virtual void PerformAction( Float param ) override;
	// Handles a special key event
	virtual bool KeyEvent( Int32 button, Int16 mod, bool keyDown ) override { return false; }
	// Handles a mouse button event
	virtual bool MouseButtonEvent( Int32 mouseX, Int32 mouseY, Int32 button, bool keyDown ) override { return false; }

private:
	// Text window object
	CGameUIKeypadWindow* m_pKeypadWindow;
	// Event type
	Char m_digit;
};

#endif //GAMEUIKEYPADWINDOW_H
