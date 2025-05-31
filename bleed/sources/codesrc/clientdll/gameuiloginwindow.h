/*
===============================================
Pathos Engine - Created by Andrew Stephen "Overfloater" Lucas

Copyright 2016
All Rights Reserved.
===============================================
*/

#ifndef GAMEUILOGINWINDOW_H
#define GAMEUILOGINWINDOW_H

#include "gameuielements.h"

enum loginwindowevent_t
{
	LOGINWINDOW_EVENT_USERNAMETAB = 0,
	LOGINWINDOW_EVENT_PASSWORDTAB,
	LOGINWINDOW_EVENT_LOGIN_BTN,
	LOGINWINDOW_EVENT_CANCEL_BTN
};

/*
====================
CGameUILoginWindow

====================
*/
class CGameUILoginWindow : public CGameUIWindow
{
public:
	// Login window description filename
	static const Char LOGINWINDOW_DESC_FILE[];
	// Login window object name
	static const Char LOGINWINDOW_OBJ_NAME[];
	// Login window title text filename
	static const Char LOGINWINDOW_TITLE_TEXT_OBJ_NAME[];
	// Login window upper separator
	static const Char LOGINWINDOW_UPPER_SEPARATOR_OBJ_NAME[];
	// Login window lower separator
	static const Char LOGINWINDOW_LOWER_SEPARATOR_OBJ_NAME[];
	// Login window username text object
	static const Char LOGINWINDOW_USERNAME_TEXT_OBJ_NAME[];
	// Login window username input tab object
	static const Char LOGINWINDOW_USERNAME_INPUT_TAB_OBJ_NAME[];
	// Login window password text object
	static const Char LOGINWINDOW_PASSWORD_TEXT_OBJ_NAME[];
	// Login window password input tab object
	static const Char LOGINWINDOW_PASSWORD_INPUT_TAB_OBJ_NAME[];
	// Login window prompt text object
	static const Char LOGINWINDOW_PROMPT_TEXT_OBJ_NAME[];
	// Login window login button object
	static const Char LOGINWINDOW_LOGIN_BUTTON_OBJ_NAME[];
	// Login window cancel button object
	static const Char LOGINWINDOW_CANCEL_BUTTON_OBJ_NAME[];
	// Login window "Notes" title text object
	static const Char LOGINWINDOW_NOTES_TITLE_TEXT_OBJ_NAME[];
	// Login window "Username" label text object
	static const Char LOGINWINDOW_NOTES_USERNAME_LABEL_OBJ_NAME[];
	// Login window "Username" data field object
	static const Char LOGINWINDOW_NOTES_USERNAME_DATA_FIELD_OBJ_NAME[];
	// Login window "Password" label text object
	static const Char LOGINWINDOW_NOTES_PASSWORD_LABEL_OBJ_NAME[];
	// Login window "Password" data field object
	static const Char LOGINWINDOW_NOTES_PASSWORD_DATA_FIELD_OBJ_NAME[];
	// Default text color
	static const color32_t LOGINWINDOW_PROMPT_FAIL_TEXT_COLOR;
	// Default text color
	static const color32_t LOGINWINDOW_PROMPT_SUCCESS_TEXT_COLOR;
	// Prompt text lifetime
	static const Double LOGINWINDOW_PROMPT_LIFETIME;

public:
	CGameUILoginWindow( Int32 flags, Int32 originX, Int32 originY, Uint32 width, Uint32 height );
	~CGameUILoginWindow();

public:
	// Initializes the window
	bool init( const ui_windowdescription_t* pWindowDesc, const ui_objectinfo_t* pWindowObject ) override;
	// Initializes the data
	bool initData( const Char* pstrUsername, const Char* pstrPassword, const Char* pstrUsernameInput, const Char* pstrPasswordInput, bool stayTillNext );

	// Think function for window
	void think( void ) override;
	// Called when window is removed
	void onRemove( void ) override;
	// Mark for delayed removal
	void setDelayedRemoval( Double delay ) override;

	// Manages a key event
	virtual bool keyEvent( Int32 button, Int16 mod, bool keyDown ) override;

public:
	// Manages an event
	void ManageEvent( loginwindowevent_t event );

	// Creates instance of window
	static CGameUILoginWindow* CreateInstance( void );

public:
	// Returns the current window information
	void getInformation( CString& username, CString& password, CString& usernameInput, CString& passwordInput, bool& stayTillNext );
	// Returns the type of the window
	virtual gameui_windows_t getWindowType( void ) const override { return GAMEUI_LOGINWINDOW; }

private:
	// Username field
	CGameUITextInputTab* m_pUsernameField;
	// Password field
	CGameUITextInputTab* m_pPasswordField;

	// Prompt text
	CGameUIText* m_pPromptText;
	// Prompt text disappear time
	Double m_promptTextTime;
	// Time at which window dies
	Double m_loginWindowTime;

	// All elements tied to the "Info" tab
	CArray<CGameUIObject*> m_pInfoTabObjectsArray;

	// Username text label
	CGameUIText* m_pTextUsername;
	// Username text label
	CGameUIText* m_pTextPassword;

	// Username
	CString m_username;
	// Password
	CString m_password;
	// TRUE if login was successful
	bool m_loginSuccessful;
	// TRUE if we should stay till next window spawn
	bool m_stayTillNextWindow;
	// Message send time
	Double m_messageSendTime;
};

/*
=================================
CGameUILoginWindowExitCallbackEvent

=================================
*/
class CGameUILoginWindowCallbackEvent : public CGameUICallbackEvent
{
public:
	CGameUILoginWindowCallbackEvent( CGameUILoginWindow* pLoginWindow, loginwindowevent_t type ):
		m_pLoginWindow(pLoginWindow),
		m_eventType(type)
	{ 
	};
	virtual ~CGameUILoginWindowCallbackEvent( void ) { };

public:
	// Performs the action
	virtual void PerformAction( Float param ) override;
	// Handles a special key event
	virtual bool KeyEvent( Int32 button, Int16 mod, bool keyDown ) override { return false; }
	// Handles a mouse button event
	virtual bool MouseButtonEvent( Int32 mouseX, Int32 mouseY, Int32 button, bool keyDown ) override { return false; }

private:
	// Text window object
	CGameUILoginWindow* m_pLoginWindow;
	// Event type
	loginwindowevent_t m_eventType;
};
#endif //GAMEUILOGINWINDOW_H