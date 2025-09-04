/*
===============================================
Pathos Engine - Created by Andrew Stephen "Overfloater" Lucas

Copyright 2016
All Rights Reserved.
===============================================
*/

#include "includes.h"
#include "huddraw.h"
#include "r_interface.h"
#include "clientdll.h"
#include "fontset.h"

#include "gameuiloginwindow.h"
#include "gameuiwindows_shared.h"
#include "snd_shared.h"

// Login window description filename
const Char CGameUILoginWindow::LOGINWINDOW_DESC_FILE[] = "gameuiloginwindow.txt";
// Login window description filename
const Char CGameUILoginWindow::LOGINWINDOW_OBJ_NAME[] = "GameUILoginWindow";
// Login window title text filename
const Char CGameUILoginWindow::LOGINWINDOW_TITLE_TEXT_OBJ_NAME[] = "GameUILoginWindowTitleText";
// Login window upper separator
const Char CGameUILoginWindow::LOGINWINDOW_UPPER_SEPARATOR_OBJ_NAME[] = "GameUILoginWindowUpperSeparator";
// Login window lower separator
const Char CGameUILoginWindow::LOGINWINDOW_LOWER_SEPARATOR_OBJ_NAME[] = "GameUILoginWindowLowerSeparator";
// Login window username text object
const Char CGameUILoginWindow::LOGINWINDOW_USERNAME_TEXT_OBJ_NAME[] = "GameUILoginWindowUsernameText";
// Login window username input tab object
const Char CGameUILoginWindow::LOGINWINDOW_USERNAME_INPUT_TAB_OBJ_NAME[] = "GameUILoginWindowUsernameInputTab";
// Login window password text object
const Char CGameUILoginWindow::LOGINWINDOW_PASSWORD_TEXT_OBJ_NAME[] = "GameUILoginWindowPasswordText";
// Login window password input tab object
const Char CGameUILoginWindow::LOGINWINDOW_PASSWORD_INPUT_TAB_OBJ_NAME[] = "GameUILoginWindowPasswordInputTab";
// Login window prompt text object
const Char CGameUILoginWindow::LOGINWINDOW_PROMPT_TEXT_OBJ_NAME[] = "GameUILoginWindowPromptText";
// Login window login button object
const Char CGameUILoginWindow::LOGINWINDOW_LOGIN_BUTTON_OBJ_NAME[] = "GameUILoginWindowLoginButton";
// Login window cancel button object
const Char CGameUILoginWindow::LOGINWINDOW_CANCEL_BUTTON_OBJ_NAME[] = "GameUILoginWindowCancelButton";
// Login window "Notes" title text object
const Char CGameUILoginWindow::LOGINWINDOW_NOTES_TITLE_TEXT_OBJ_NAME[] = "GameUILoginWindowNotesTitleText";
// Login window "Username" label text object
const Char CGameUILoginWindow::LOGINWINDOW_NOTES_USERNAME_LABEL_OBJ_NAME[] = "GameUILoginWindowUsernameLabel";
// Login window "Username" data field object
const Char CGameUILoginWindow::LOGINWINDOW_NOTES_USERNAME_DATA_FIELD_OBJ_NAME[] = "GameUILoginWindowUsernameDataField";
// Login window "Password" label text object
const Char CGameUILoginWindow::LOGINWINDOW_NOTES_PASSWORD_LABEL_OBJ_NAME[] = "GameUILoginWindowPasswordLabel";
// Login window "Password" data field object
const Char CGameUILoginWindow::LOGINWINDOW_NOTES_PASSWORD_DATA_FIELD_OBJ_NAME[] = "GameUILoginWindowPasswordDataField";
// Default text color
const color32_t CGameUILoginWindow::LOGINWINDOW_PROMPT_FAIL_TEXT_COLOR = color32_t(255, 30, 30, 255);
// Default text color
const color32_t CGameUILoginWindow::LOGINWINDOW_PROMPT_SUCCESS_TEXT_COLOR = color32_t(30, 255, 30, 255);
// Prompt text lifetime
const Double CGameUILoginWindow::LOGINWINDOW_PROMPT_LIFETIME = 3;

//====================================
//
//====================================
CGameUILoginWindow::CGameUILoginWindow( Int32 flags, Int32 originX, Int32 originY, Uint32 width, Uint32 height ):
	CGameUIWindow(flags, originX, originY, width, height),
	m_pUsernameField(nullptr),
	m_pPasswordField(nullptr),
	m_pPromptText(nullptr),
	m_promptTextTime(0),
	m_loginWindowTime(0),
	m_pTextUsername(nullptr),
	m_pTextPassword(nullptr),
	m_loginSuccessful(false),
	m_stayTillNextWindow(false),
	m_messageSendTime(0)
{
}

//====================================
//
//====================================
CGameUILoginWindow::~CGameUILoginWindow( void )
{
}

//====================================
//
//====================================
bool CGameUILoginWindow::init( const ui_windowdescription_t* pWindowDesc, const ui_objectinfo_t* pWindowObject )
{
	// Initialize schema for surface object
	if(!CGameUISurface::initSchema(pWindowObject->getSchema().c_str()))
		return false;

	//
	// Create the title text object
	//
	const ui_objectinfo_t* pTitleTextObject = pWindowDesc->getObject(UI_OBJECT_TEXT, LOGINWINDOW_TITLE_TEXT_OBJ_NAME);
	if(!pTitleTextObject)
	{
		cl_engfuncs.pfnCon_EPrintf("Window description file '%s' has no definition for '%s'.\n", LOGINWINDOW_DESC_FILE, LOGINWINDOW_TITLE_TEXT_OBJ_NAME);
		return false;
	}

	const font_set_t* pFontSet = g_pGUIManager->GetSchemaResolutionFont(pTitleTextObject->getTextSchemaName().c_str());

	CGameUIText* pWindowTitleText = new CGameUIText(
		pTitleTextObject->getFlags(), 
		pTitleTextObject->getTextColor(), 
		pFontSet, 
		pWindowObject->getXInset() + pTitleTextObject->getXOrigin(), 
		pWindowObject->getYInset() + pTitleTextObject->getYOrigin());

	pWindowTitleText->setParent(this);
	pWindowTitleText->setText(pTitleTextObject->getText().c_str());

	//
	// Create upper separator
	//
	const ui_objectinfo_t* pUpperSeparatorObject = pWindowDesc->getObject(UI_OBJECT_SEPARATOR_H, LOGINWINDOW_UPPER_SEPARATOR_OBJ_NAME);
	if(!pUpperSeparatorObject)
	{
		cl_engfuncs.pfnCon_EPrintf("Window description file '%s' has no definition for '%s'.\n", LOGINWINDOW_DESC_FILE, LOGINWINDOW_UPPER_SEPARATOR_OBJ_NAME);
		return false;
	}

	CGameUIHorizontalSeparator* pUpperSeparator = new CGameUIHorizontalSeparator(
		pUpperSeparatorObject->getFlags(), 
		pUpperSeparatorObject->getWidth(), 
		pUpperSeparatorObject->getHeight(), 
		pWindowObject->getXInset() + pUpperSeparatorObject->getXOrigin(), 
		pWindowObject->getYInset() + pUpperSeparatorObject->getYOrigin());
	pUpperSeparator->setParent(this);

	if(!pUpperSeparator->initSchema(pUpperSeparatorObject->getSchema().c_str()))
	{
		cl_engfuncs.pfnCon_EPrintf("Failed to initialize 'CGameUIHorizontalSeparator' object named '%s'.\n", LOGINWINDOW_UPPER_SEPARATOR_OBJ_NAME);
		return false;
	}

	//
	// Create the username tab text
	//
	const ui_objectinfo_t* pUsernameTextObject = pWindowDesc->getObject(UI_OBJECT_TEXT, LOGINWINDOW_USERNAME_TEXT_OBJ_NAME);
	if(!pUsernameTextObject)
	{
		cl_engfuncs.pfnCon_EPrintf("Window description file '%s' has no definition for '%s'.\n", LOGINWINDOW_DESC_FILE, LOGINWINDOW_USERNAME_TEXT_OBJ_NAME);
		return false;
	}

	pFontSet = g_pGUIManager->GetSchemaResolutionFont(pUsernameTextObject->getTextSchemaName().c_str());

	CGameUIText* pUsernameText = new CGameUIText(
		pUsernameTextObject->getFlags(), 
		pUsernameTextObject->getTextColor(), 
		pFontSet, 
		pWindowObject->getXInset() + pUsernameTextObject->getXOrigin(), 
		pWindowObject->getYInset() + pUsernameTextObject->getYOrigin());

	pUsernameText->setParent(this);
	pUsernameText->setText(pUsernameTextObject->getText().c_str());

	//
	// Create the username input tab
	//
	const ui_objectinfo_t* pUsernameInputTabObject = pWindowDesc->getObject(UI_OBJECT_TAB, LOGINWINDOW_USERNAME_INPUT_TAB_OBJ_NAME);
	if(!pUsernameInputTabObject)
	{
		cl_engfuncs.pfnCon_EPrintf("Window description file '%s' has no definition for '%s'.\n", LOGINWINDOW_DESC_FILE, LOGINWINDOW_USERNAME_INPUT_TAB_OBJ_NAME);
		return false;
	}

	CGameUILoginWindowCallbackEvent *pUsernameEvent = new CGameUILoginWindowCallbackEvent(this, LOGINWINDOW_EVENT_USERNAMETAB);
	m_pUsernameField = new CGameUITextInputTab(pUsernameInputTabObject->getFlags(), 
		pUsernameEvent,
		pUsernameInputTabObject->getTextInset(),
		pFontSet,
		pWindowObject->getXInset() + pUsernameInputTabObject->getXOrigin(), 
		pWindowObject->getYInset() + pUsernameInputTabObject->getYOrigin(),
		pUsernameInputTabObject->getWidth(),
		pUsernameInputTabObject->getHeight());
	m_pUsernameField->setParent(this);

	if(!m_pUsernameField->initSchema(pUsernameInputTabObject->getSchema().c_str()))
	{
		cl_engfuncs.pfnCon_EPrintf("Failed to initialize 'CGameUITextInputTab'.\n");
		return false;
	}

	//
	// Create the password tab text
	//
	const ui_objectinfo_t* pPasswordTextObject = pWindowDesc->getObject(UI_OBJECT_TEXT, LOGINWINDOW_PASSWORD_TEXT_OBJ_NAME);
	if(!pPasswordTextObject)
	{
		cl_engfuncs.pfnCon_EPrintf("Window description file '%s' has no definition for '%s'.\n", LOGINWINDOW_DESC_FILE, LOGINWINDOW_PASSWORD_TEXT_OBJ_NAME);
		return false;
	}

	pFontSet = g_pGUIManager->GetSchemaResolutionFont(pPasswordTextObject->getTextSchemaName().c_str());

	CGameUIText* pPasswordText = new CGameUIText(
		pPasswordTextObject->getFlags(), 
		pPasswordTextObject->getTextColor(), 
		pPasswordTextObject->getFont(), 
		pWindowObject->getXInset() + pPasswordTextObject->getXOrigin(), 
		pWindowObject->getYInset() + pPasswordTextObject->getYOrigin());

	pPasswordText->setParent(this);
	pPasswordText->setText(pPasswordTextObject->getText().c_str());

	//
	// Create the password input tab
	//
	const ui_objectinfo_t* pPasswordInputTabObject = pWindowDesc->getObject(UI_OBJECT_TAB, LOGINWINDOW_PASSWORD_INPUT_TAB_OBJ_NAME);
	if(!pPasswordInputTabObject)
	{
		cl_engfuncs.pfnCon_EPrintf("Window description file '%s' has no definition for '%s'.\n", LOGINWINDOW_DESC_FILE, LOGINWINDOW_PASSWORD_INPUT_TAB_OBJ_NAME);
		return false;
	}

	CGameUILoginWindowCallbackEvent *pPasswordEvent = new CGameUILoginWindowCallbackEvent(this, LOGINWINDOW_EVENT_PASSWORDTAB);
	m_pPasswordField = new CGameUITextInputTab(pPasswordInputTabObject->getFlags(), 
		pPasswordEvent,
		pPasswordInputTabObject->getTextInset(),
		pPasswordInputTabObject->getFont(),
		pWindowObject->getXInset() + pPasswordInputTabObject->getXOrigin(), 
		pWindowObject->getYInset() + pPasswordInputTabObject->getYOrigin(),
		pPasswordInputTabObject->getWidth(),
		pPasswordInputTabObject->getHeight());
	m_pPasswordField->setParent(this);

	if(!m_pPasswordField->initSchema(pPasswordInputTabObject->getSchema().c_str()))
	{
		cl_engfuncs.pfnCon_EPrintf("Failed to initialize 'CGameUITextInputTab'.\n");
		return false;
	}

	//
	// Create the prompt text object
	//
	const ui_objectinfo_t* pPromptTextObject = pWindowDesc->getObject(UI_OBJECT_TEXT, LOGINWINDOW_PROMPT_TEXT_OBJ_NAME);
	if(!pPromptTextObject)
	{
		cl_engfuncs.pfnCon_EPrintf("Window description file '%s' has no definition for '%s'.\n", LOGINWINDOW_DESC_FILE, LOGINWINDOW_PROMPT_TEXT_OBJ_NAME);
		return false;
	}

	m_pPromptText = new CGameUIText(pPromptTextObject->getFlags(), 
		pPromptTextObject->getTextColor(),
		pPromptTextObject->getFont(),
		pWindowObject->getXInset() + pPromptTextObject->getXOrigin(),
		pWindowObject->getYInset() + pPromptTextObject->getYOrigin());
	m_pPromptText->setParent(this);
	m_pPromptText->setVisible(false);
	m_pPromptText->setText(pPromptTextObject->getText().c_str());

	//
	// Create the "Login" button
	//
	const ui_objectinfo_t* pLoginButtonObject = pWindowDesc->getObject(UI_OBJECT_BUTTON, LOGINWINDOW_LOGIN_BUTTON_OBJ_NAME);
	if(!pLoginButtonObject)
	{
		cl_engfuncs.pfnCon_EPrintf("Window description file '%s' has no definition for '%s'.\n", LOGINWINDOW_DESC_FILE, LOGINWINDOW_LOGIN_BUTTON_OBJ_NAME);
		return false;
	}

	CGameUILoginWindowCallbackEvent* pLoginEvent = new CGameUILoginWindowCallbackEvent(this, LOGINWINDOW_EVENT_LOGIN_BTN);
	CGameUIButton* pLoginButton = new CGameUIButton(pLoginButtonObject->getFlags(), 
		pLoginEvent, 
		SDLK_RETURN,
		pWindowObject->getXInset() + pLoginButtonObject->getXOrigin(),
		pWindowObject->getYInset() + pLoginButtonObject->getYOrigin(),
		pLoginButtonObject->getWidth(),
		pLoginButtonObject->getHeight());

	if(!pLoginButton->initSchema(pLoginButtonObject->getSchema().c_str()))
	{
		cl_engfuncs.pfnCon_EPrintf("Failed to initialize 'CGameUIButton'.\n");
		return false;
	}

	pLoginButton->setText(pLoginButtonObject->getText().c_str());
	pLoginButton->setParent(this);

	//
	// Create the "Cancel" button
	//
	const ui_objectinfo_t* pCancelButtonObject = pWindowDesc->getObject(UI_OBJECT_BUTTON, LOGINWINDOW_CANCEL_BUTTON_OBJ_NAME);
	if(!pCancelButtonObject)
	{
		cl_engfuncs.pfnCon_EPrintf("Window description file '%s' has no definition for '%s'.\n", LOGINWINDOW_DESC_FILE, LOGINWINDOW_CANCEL_BUTTON_OBJ_NAME);
		return false;
	}

	CGameUILoginWindowCallbackEvent* pCancelEvent = new CGameUILoginWindowCallbackEvent(this, LOGINWINDOW_EVENT_CANCEL_BTN);
	CGameUIButton* pCancelButton = new CGameUIButton(pCancelButtonObject->getFlags(), 
		pCancelEvent, 
		SDLK_ESCAPE,
		pWindowObject->getXInset() + pCancelButtonObject->getXOrigin(),
		pWindowObject->getYInset() + pCancelButtonObject->getYOrigin(),
		pCancelButtonObject->getWidth(),
		pCancelButtonObject->getHeight());

	if(!pCancelButton->initSchema(pCancelButtonObject->getSchema().c_str()))
	{
		cl_engfuncs.pfnCon_EPrintf("Failed to initialize 'CGameUIButton'.\n");
		return false;
	}

	pCancelButton->setText(pCancelButtonObject->getText().c_str());
	pCancelButton->setParent(this);

	// Set the "Notes" tab objects

	//
	// Create lower separator
	//
	const ui_objectinfo_t* pLowerSeparatorObject = pWindowDesc->getObject(UI_OBJECT_SEPARATOR_H, LOGINWINDOW_LOWER_SEPARATOR_OBJ_NAME);
	if(!pLowerSeparatorObject)
	{
		cl_engfuncs.pfnCon_EPrintf("Window description file '%s' has no definition for '%s'.\n", LOGINWINDOW_DESC_FILE, LOGINWINDOW_LOWER_SEPARATOR_OBJ_NAME);
		return false;
	}

	CGameUIHorizontalSeparator* pLowerSeparator = new CGameUIHorizontalSeparator(
		pLowerSeparatorObject->getFlags(), 
		pLowerSeparatorObject->getWidth(), 
		pLowerSeparatorObject->getHeight(), 
		pWindowObject->getXInset() + pLowerSeparatorObject->getXOrigin(), 
		pWindowObject->getYInset() + pLowerSeparatorObject->getYOrigin());
	pLowerSeparator->setParent(this);
	pLowerSeparator->setVisible(false);
	m_pInfoTabObjectsArray.push_back(pLowerSeparator);

	if(!pLowerSeparator->initSchema(pLowerSeparatorObject->getSchema().c_str()))
	{
		cl_engfuncs.pfnCon_EPrintf("Failed to initialize 'CGameUIHorizontalSeparator' object named '%s'.\n", LOGINWINDOW_LOWER_SEPARATOR_OBJ_NAME);
		return false;
	}

	//
	// Create title text object
	//
	const ui_objectinfo_t* pNotesTitleTextObject = pWindowDesc->getObject(UI_OBJECT_TEXT, LOGINWINDOW_NOTES_TITLE_TEXT_OBJ_NAME);
	if(!pNotesTitleTextObject)
	{
		cl_engfuncs.pfnCon_EPrintf("Window description file '%s' has no definition for '%s'.\n", LOGINWINDOW_DESC_FILE, LOGINWINDOW_NOTES_TITLE_TEXT_OBJ_NAME);
		return false;
	}

	pFontSet = g_pGUIManager->GetSchemaResolutionFont(pNotesTitleTextObject->getTextSchemaName().c_str());

	CGameUIText* pNotesTitleText = new CGameUIText(
		pNotesTitleTextObject->getFlags(), 
		pNotesTitleTextObject->getTextColor(), 
		pNotesTitleTextObject->getFont(), 
		pWindowObject->getXInset() + pNotesTitleTextObject->getXOrigin(), 
		pWindowObject->getYInset() + pNotesTitleTextObject->getYOrigin());

	pNotesTitleText->setParent(this);
	pNotesTitleText->setText(pNotesTitleTextObject->getText().c_str());	
	pNotesTitleText->setVisible(false);
	m_pInfoTabObjectsArray.push_back(pNotesTitleText);

	//
	// Create the "username" notes field label
	//
	const ui_objectinfo_t* pInfoLabelUsernameObject = pWindowDesc->getObject(UI_OBJECT_TEXT, LOGINWINDOW_NOTES_USERNAME_LABEL_OBJ_NAME);
	if(!pInfoLabelUsernameObject)
	{
		cl_engfuncs.pfnCon_EPrintf("Window description file '%s' has no definition for '%s'.\n", LOGINWINDOW_DESC_FILE, LOGINWINDOW_NOTES_USERNAME_LABEL_OBJ_NAME);
		return false;
	}

	pFontSet = g_pGUIManager->GetSchemaResolutionFont(pInfoLabelUsernameObject->getTextSchemaName().c_str());

	CGameUIText* pInfoLabelUsername = new CGameUIText(
		pInfoLabelUsernameObject->getFlags(), 
		pInfoLabelUsernameObject->getTextColor(), 
		pInfoLabelUsernameObject->getFont(), 
		pWindowObject->getXInset() + pInfoLabelUsernameObject->getXOrigin(), 
		pWindowObject->getYInset() + pInfoLabelUsernameObject->getYOrigin());

	pInfoLabelUsername->setParent(this);
	pInfoLabelUsername->setText(pInfoLabelUsernameObject->getText().c_str());
	pInfoLabelUsername->setVisible(false);
	m_pInfoTabObjectsArray.push_back(pInfoLabelUsername);

	//
	// Create "username" data field
	//
	const ui_objectinfo_t* pInfoUsernameDataFieldObject = pWindowDesc->getObject(UI_OBJECT_TEXT, LOGINWINDOW_NOTES_USERNAME_DATA_FIELD_OBJ_NAME);
	if(!pInfoUsernameDataFieldObject)
	{
		cl_engfuncs.pfnCon_EPrintf("Window description file '%s' has no definition for '%s'.\n", LOGINWINDOW_DESC_FILE, LOGINWINDOW_NOTES_USERNAME_DATA_FIELD_OBJ_NAME);
		return false;
	}

	pFontSet = g_pGUIManager->GetSchemaResolutionFont(pInfoUsernameDataFieldObject->getTextSchemaName().c_str());

	m_pTextUsername = new CGameUIText(
		pInfoUsernameDataFieldObject->getFlags(), 
		pInfoUsernameDataFieldObject->getTextColor(), 
		pInfoUsernameDataFieldObject->getFont(), 
		pWindowObject->getXInset() + pInfoUsernameDataFieldObject->getXOrigin(), 
		pWindowObject->getYInset() + pInfoUsernameDataFieldObject->getYOrigin());

	m_pTextUsername->setParent(this);
	m_pTextUsername->setText(pInfoUsernameDataFieldObject->getText().c_str());
	m_pTextUsername->setVisible(false);
	m_pInfoTabObjectsArray.push_back(m_pTextUsername);

	//
	// Create the "password" label
	//
	const ui_objectinfo_t* pInfoLabelPasswordbject = pWindowDesc->getObject(UI_OBJECT_TEXT, LOGINWINDOW_NOTES_PASSWORD_LABEL_OBJ_NAME);
	if(!pInfoLabelPasswordbject)
	{
		cl_engfuncs.pfnCon_EPrintf("Window description file '%s' has no definition for '%s'.\n", LOGINWINDOW_DESC_FILE, LOGINWINDOW_NOTES_PASSWORD_LABEL_OBJ_NAME);
		return false;
	}

	pFontSet = g_pGUIManager->GetSchemaResolutionFont(pInfoLabelPasswordbject->getTextSchemaName().c_str());

	CGameUIText* pInfoLabelPassword = new CGameUIText(
		pInfoLabelPasswordbject->getFlags(), 
		pInfoLabelPasswordbject->getTextColor(), 
		pInfoLabelPasswordbject->getFont(), 
		pWindowObject->getXInset() + pInfoLabelPasswordbject->getXOrigin(), 
		pWindowObject->getYInset() + pInfoLabelPasswordbject->getYOrigin());

	pInfoLabelPassword->setParent(this);
	pInfoLabelPassword->setText(pInfoLabelPasswordbject->getText().c_str());
	pInfoLabelPassword->setVisible(false);
	m_pInfoTabObjectsArray.push_back(pInfoLabelPassword);

	//
	// Create "username" data field
	//
	const ui_objectinfo_t* pInfoPasswordDataFieldObject = pWindowDesc->getObject(UI_OBJECT_TEXT, LOGINWINDOW_NOTES_PASSWORD_DATA_FIELD_OBJ_NAME);
	if(!pInfoPasswordDataFieldObject)
	{
		cl_engfuncs.pfnCon_EPrintf("Window description file '%s' has no definition for '%s'.\n", LOGINWINDOW_DESC_FILE, LOGINWINDOW_NOTES_PASSWORD_DATA_FIELD_OBJ_NAME);
		return false;
	}

	pFontSet = g_pGUIManager->GetSchemaResolutionFont(pInfoPasswordDataFieldObject->getTextSchemaName().c_str());

	m_pTextPassword = new CGameUIText(
		pInfoPasswordDataFieldObject->getFlags(), 
		pInfoPasswordDataFieldObject->getTextColor(), 
		pInfoPasswordDataFieldObject->getFont(), 
		pWindowObject->getXInset() + pInfoPasswordDataFieldObject->getXOrigin(), 
		pWindowObject->getYInset() + pInfoPasswordDataFieldObject->getYOrigin());

	m_pTextPassword->setParent(this);
	m_pTextPassword->setText(pInfoPasswordDataFieldObject->getText().c_str());
	m_pTextPassword->setVisible(false);
	m_pInfoTabObjectsArray.push_back(m_pTextPassword);

	// Set input focus to username input tab
	setInputFocusObject(m_pUsernameField);

	return true;
}

//====================================
//
//====================================
bool CGameUILoginWindow::initData( const Char* pstrUsername, const Char* pstrPassword, const Char* pstrUsernameInput, const Char* pstrPasswordInput, bool stayTillNext )
{
	// Set the info labels to be visible
	if(pstrUsername && qstrlen(pstrUsername) > 0 && pstrPassword && qstrlen(pstrPassword) > 0)
	{
		for(Uint32 i = 0; i < m_pInfoTabObjectsArray.size(); i++)
			m_pInfoTabObjectsArray[i]->setVisible(true);

		m_pTextUsername->setText(pstrUsername);
		m_username = pstrUsername;

		m_pTextPassword->setText(pstrPassword);
		m_password = pstrPassword;
	}

	if(pstrUsernameInput)
		m_pUsernameField->setText(pstrUsernameInput);

	if(pstrPasswordInput)
		m_pPasswordField->setText(pstrPasswordInput);

	m_stayTillNextWindow = stayTillNext;
	if(m_stayTillNextWindow)
		m_windowFlags |= FL_WINDOW_WAIT_TILL_NEXT;

	return true;
}

//====================================
//
//====================================
void CGameUILoginWindow::getInformation( CString& username, CString& password, CString& usernameInput, CString& passwordInput, bool& stayTillNext )
{
	username = m_username;
	password = m_password;
	stayTillNext = m_stayTillNextWindow;

	usernameInput = m_pUsernameField->getText();
	passwordInput = m_pPasswordField->getText();
}

//====================================
//
//====================================
void CGameUILoginWindow::ManageEvent( loginwindowevent_t event )
{
	switch(event)
	{
	case LOGINWINDOW_EVENT_USERNAMETAB:
		break;
	case LOGINWINDOW_EVENT_PASSWORDTAB:
		break;
	case LOGINWINDOW_EVENT_LOGIN_BTN:
		{
			Double time = cl_engfuncs.pfnGetClientTime();
			const Char* pstrInputUsername = m_pUsernameField->getText();
			const Char* pstrInputPassword = m_pPasswordField->getText();

			if(!m_password.empty() && !qstrcmp(pstrInputUsername, m_username) && !qstrcmp(pstrInputPassword, m_password))
			{
				cl_engfuncs.pfnPlayAmbientSound(0, ZERO_VECTOR, SND_CHAN_AUTO, GAMEUI_OK_SOUND, VOL_NORM, ATTN_NORM, PITCH_NORM, SND_FL_2D, 0);

				m_pPromptText->setText("Login attempt successful.");
				m_pPromptText->setColor(LOGINWINDOW_PROMPT_SUCCESS_TEXT_COLOR);
				m_pPromptText->setVisible(true);
				m_promptTextTime = time + LOGINWINDOW_PROMPT_LIFETIME;

				setDelayedRemoval(GAMEUIWINDOW_REMOVE_DELAY_TIME);
				m_loginSuccessful = true;
				return;
			}
			else
			{
				cl_engfuncs.pfnPlayAmbientSound(0, ZERO_VECTOR, SND_CHAN_AUTO, GAMEUI_FAIL_SOUND, VOL_NORM, ATTN_NORM, PITCH_NORM, SND_FL_2D, 0);

				m_pPromptText->setText("Login attempt failed: Wrong username or password.");
				m_pPromptText->setColor(LOGINWINDOW_PROMPT_FAIL_TEXT_COLOR);
				m_pPromptText->setVisible(true);

				m_promptTextTime = time + LOGINWINDOW_PROMPT_LIFETIME;
				return;
			}
		}
		break;
	case LOGINWINDOW_EVENT_CANCEL_BTN:
		m_windowFlags |= CGameUIWindow::FL_WINDOW_KILLME;
		break;
	}
}

//====================================
//
//====================================
CGameUILoginWindow* CGameUILoginWindow::CreateInstance( void )
{
	// Load the schema file
	ui_windowdescription_t* pWinDesc = g_pGUIManager->LoadWindowDescriptionFile(LOGINWINDOW_OBJ_NAME, LOGINWINDOW_DESC_FILE);
 	if(!pWinDesc)
	{
		cl_engfuncs.pfnCon_EPrintf("Failed to load window description '%s' for '%s'.\n", LOGINWINDOW_DESC_FILE, LOGINWINDOW_OBJ_NAME);
		return nullptr;
	}

	const ui_objectinfo_t* pWindowObject = pWinDesc->getObject(UI_OBJECT_WINDOW, LOGINWINDOW_OBJ_NAME);
	if(!pWindowObject)
	{
		cl_engfuncs.pfnCon_EPrintf("Window description file '%s' has no definition for '%s'.\n", LOGINWINDOW_DESC_FILE, LOGINWINDOW_OBJ_NAME);
		return nullptr;
	}

	Uint32 screenWidth, screenHeight;
	cl_renderfuncs.pfnGetScreenSize(screenWidth, screenHeight);

	Uint32 relativeWidth = gHUDDraw.ScaleY(pWindowObject->getWidth());
	Uint32 relativeHeight = gHUDDraw.ScaleY(pWindowObject->getHeight());

	Int32 xPosition = (screenWidth / 2) - (relativeWidth / 2);
	Int32 yPosition = (screenHeight / 2) - (relativeHeight / 2);

	CGameUILoginWindow* pNewWindow = new CGameUILoginWindow(CGameUIWindow::FL_WINDOW_NONE, xPosition, yPosition, relativeWidth, relativeHeight);
	if(!pNewWindow->init(pWinDesc, pWindowObject))
	{
		cl_engfuncs.pfnCon_EPrintf("%s - Failed to initialize 'CGameUILoginWindow'.\n", __FUNCTION__);
		delete pNewWindow;
		return nullptr;
	}

	return pNewWindow;
}

//====================================
//
//====================================
void CGameUILoginWindow::think( void )
{
	if(m_promptTextTime && m_promptTextTime <= cl_engfuncs.pfnGetClientTime())
	{
		m_pPromptText->setVisible(false);
		m_promptTextTime = 0;
	}

	if(m_messageSendTime && m_messageSendTime <= cl_engfuncs.pfnGetClientTime())
	{
		onRemove();
		m_messageSendTime = 0;
	}

	// Call base class to manage think functions
	CGameUIWindow::think();
}

//====================================
//
//====================================
void CGameUILoginWindow::setDelayedRemoval( Double delay )
{
	if(m_windowFlags & FL_WINDOW_WAIT_TILL_NEXT)
		m_messageSendTime = cl_engfuncs.pfnGetClientTime() + delay;

	CGameUIWindow::setDelayedRemoval(delay);
}

//====================================
//
//====================================
void CGameUILoginWindow::onRemove( void ) 
{
	if(!m_loginSuccessful)
		return;

	// Send message to server
	Uint32 msgid = g_pGUIManager->GetServerUIMessageId();
	if(!msgid)
	{
		cl_engfuncs.pfnCon_Printf("%s - Message 'GameUIMessage' not registered on client.\n", __FUNCTION__);
		return;
	}

	// Just tell them which window sent it
	cl_engfuncs.pfnClientUserMessageBegin(msgid);
		cl_engfuncs.pfnMsgWriteByte(GAMEUIEVENT_CODE_MATCHES);
		cl_engfuncs.pfnMsgWriteByte(GAMEUI_LOGINWINDOW);
	cl_engfuncs.pfnClientUserMessageEnd();
}

//====================================
//
//====================================
bool CGameUILoginWindow::keyEvent( Int32 button, Int16 mod, bool keyDown )
{
	// Get SDL Keycode
	SDL_Keycode sdlKeycode = SDL_GetKeyFromScancode((SDL_Scancode)button);

	if(keyDown && sdlKeycode == SDLK_TAB)
	{
		if(m_pUsernameField->isInputFocusObject())
			setInputFocusObject(m_pPasswordField);
		else
			setInputFocusObject(m_pUsernameField);

		return true;
	}

	return CGameUIWindow::keyEvent(button, mod, keyDown);
}

//====================================
//
//====================================
void CGameUILoginWindowCallbackEvent::PerformAction( Float param )
{
	if(!m_pLoginWindow)
		return;

	m_pLoginWindow->ManageEvent(m_eventType);
}
