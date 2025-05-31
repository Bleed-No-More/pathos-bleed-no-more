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
#include "common.h"

#include "gameuikeypadwindow.h"
#include "gameuiwindows_shared.h"
#include "snd_shared.h"
#include "r_common.h"

// Keypad window description filename
const Char CGameUIKeypadWindow::KEYPADWINDOW_DESC_FILE[] = "gameuikeypadwindow.txt";
// Keypad window description filename
const Char CGameUIKeypadWindow::KEYPADWINDOW_OBJ_NAME[] = "GameUIKeypadWindow";
// Title text object name
const Char CGameUIKeypadWindow::KEYPADWINDOW_TITLE_TEXT_OBJ_NAME[] = "GameUIKeypadWindowTitleText";
// Numerical display object name
const Char CGameUIKeypadWindow::KEYPADWINDOW_NUMERICAL_DISPLAY_OBJ_NAME[] = "GameUIKeypadWindowNumericalDisplay";
// Numerical display text object name
const Char CGameUIKeypadWindow::KEYPADWINDOW_DISPLAY_TEXT_OBJ_NAME[] = "GameUIKeypadWindowNumericalDisplayText";
// Upper separator object name
const Char CGameUIKeypadWindow::KEYPADWINDOW_UPPER_SEPARATOR_OBJ_NAME[] = "GameUIKeypadWindowUpperSeparator";
// Button tab object name
const Char CGameUIKeypadWindow::KEYPADWINDOW_BUTTON_TAB_OBJ_NAME[] = "GameUIKeypadWindowButtonTab";
// Lower separator object name
const Char CGameUIKeypadWindow::KEYPADWINDOW_LOWER_SEPARATOR_OBJ_NAME[] = "GameUIKeypadWindowLowerSeparator";
// Notes surface object name
const Char CGameUIKeypadWindow::KEYPADWINDOW_NOTES_SURFACE_OBJ_NAME[] = "GameUIKeypadWindowNotesSurface";
// Notes tab title text object name
const Char CGameUIKeypadWindow::KEYPADWINDOW_NOTES_TITLE_TEXT_OBJ_NAME[] = "GameUIKeypadWindowNotesTitleText";
// Notes tab "Passcode" label text object name
const Char CGameUIKeypadWindow::KEYPADWINDOW_NOTES_PASSCODE_LABEL_TEXT_OBJ_NAME[] = "GameUIKeypadWindowNotesPasscodeLabel";
// Notes tab "Passcode" text object name
const Char CGameUIKeypadWindow::KEYPADWINDOW_NOTES_PASSCODE_TEXT_OBJ_NAME[] = "GameUIKeypadWindowNotesPasscode";
// Button tab object name
const Char CGameUIKeypadWindow::KEYPADWINDOW_NUMERICAL_BUTTON_OBJ_NAME[] = "GameUIKeypadWindowButtonDigit";
// "Ok" button object name
const Char CGameUIKeypadWindow::KEYPADWINDOW_OK_BUTTON_OBJ_NAME[] = "GameUIKeypadWindowButtonOk";
// "Cancel" button object name
const Char CGameUIKeypadWindow::KEYPADWINDOW_CANCEL_BUTTON_OBJ_NAME[] = "GameUIKeypadWindowButtonCancel";
// "Delete" button object name
const Char CGameUIKeypadWindow::KEYPADWINDOW_DELETE_BUTTON_OBJ_NAME[] = "GameUIKeypadWindowButtonDelete";
// "Clear" button object name
const Char CGameUIKeypadWindow::KEYPADWINDOW_CLEAR_BUTTON_OBJ_NAME[] = "GameUIKeypadWindowButtonClear";
// Prompt text lifetime
const Double CGameUIKeypadWindow::KEYPADWINDOW_PROMPT_LIFETIME = 1;
// Default text color
const color32_t CGameUIKeypadWindow::KEYPADWINDOW_PROMPT_FAIL_TEXT_COLOR = color32_t(255, 30, 30, 255);
// Default text color
const color32_t CGameUIKeypadWindow::KEYPADWINDOW_PROMPT_SUCCESS_TEXT_COLOR = color32_t(30, 255, 30, 255);

//====================================
//
//====================================
CGameUIKeypadWindow::CGameUIKeypadWindow( Int32 flags, Int32 originX, Int32 originY, Uint32 width, Uint32 height ):
	CGameUIWindow(flags, originX, originY, width, height),
	m_pKeypadDisplayText(nullptr),
	m_pLowerSeparator(nullptr),
	m_pNotesSurface(nullptr),
	m_pTextPasscode(nullptr),
	m_codeAccepted(false),
	m_stayTillNextWindow(false),
	m_promptTextTime(0),
	m_messageSendTime(0)
{
}

//====================================
//
//====================================
CGameUIKeypadWindow::~CGameUIKeypadWindow( void )
{
}

//====================================
//
//====================================
bool CGameUIKeypadWindow::init( const ui_windowdescription_t* pWindowDesc, const ui_objectinfo_t* pWindowObject )
{
	// Initialize schema for surface object
	if(!CGameUISurface::initSchema(pWindowObject->getSchema().c_str()))
		return false;

	//
	// Create title text object
	//
	const ui_objectinfo_t* pTitleTextObject = pWindowDesc->getObject(UI_OBJECT_TEXT, KEYPADWINDOW_TITLE_TEXT_OBJ_NAME);
	if(!pTitleTextObject)
	{
		cl_engfuncs.pfnCon_EPrintf("Window description file '%s' has no definition for '%s'.\n", KEYPADWINDOW_DESC_FILE, KEYPADWINDOW_TITLE_TEXT_OBJ_NAME);
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
	// Create the numerical display tab
	//
	const ui_objectinfo_t* pKeypadNumericalDisplaySurfaceObject = pWindowDesc->getObject(UI_OBJECT_TAB, KEYPADWINDOW_NUMERICAL_DISPLAY_OBJ_NAME);
	if(!pKeypadNumericalDisplaySurfaceObject)
	{
		cl_engfuncs.pfnCon_EPrintf("Window description file '%s' has no definition for '%s'.\n", KEYPADWINDOW_DESC_FILE, KEYPADWINDOW_NUMERICAL_DISPLAY_OBJ_NAME);
		return false;
	}

	// Create the numerical display text tab
	CGameUISurface *pNumericalDisplaySurface = new CGameUISurface(
		pKeypadNumericalDisplaySurfaceObject->getFlags(),
		pWindowObject->getXInset() + pKeypadNumericalDisplaySurfaceObject->getXOrigin(),
		pWindowObject->getYInset() + pKeypadNumericalDisplaySurfaceObject->getYOrigin(),
		pKeypadNumericalDisplaySurfaceObject->getWidth(),
		pKeypadNumericalDisplaySurfaceObject->getHeight());
	pNumericalDisplaySurface->setParent(this);

	if(!pNumericalDisplaySurface->initSchema(pKeypadNumericalDisplaySurfaceObject->getSchema().c_str()))
	{
		cl_engfuncs.pfnCon_EPrintf("Failed to initialize 'CGameUIScrollableSurface' object named '%s'.\n", KEYPADWINDOW_NUMERICAL_DISPLAY_OBJ_NAME);
		return false;
	}

	//
	// Create display text object
	//
	const ui_objectinfo_t* pNumericalDisplayTextObject = pWindowDesc->getObject(UI_OBJECT_TEXT, KEYPADWINDOW_DISPLAY_TEXT_OBJ_NAME);
	if(!pNumericalDisplayTextObject)
	{
		cl_engfuncs.pfnCon_EPrintf("Window description file '%s' has no definition for '%s'.\n", KEYPADWINDOW_DESC_FILE, KEYPADWINDOW_DISPLAY_TEXT_OBJ_NAME);
		return false;
	}

	pFontSet = g_pGUIManager->GetSchemaResolutionFont(pNumericalDisplayTextObject->getTextSchemaName().c_str());

	m_pKeypadDisplayText = new CGameUIText(
		pNumericalDisplayTextObject->getFlags(), 
		pNumericalDisplayTextObject->getTextColor(), 
		pFontSet, 
		pWindowObject->getXInset() + pNumericalDisplayTextObject->getXOrigin(), 
		pWindowObject->getYInset() + pNumericalDisplayTextObject->getYOrigin());

	m_pKeypadDisplayText->setParent(pNumericalDisplaySurface);
	m_pKeypadDisplayText->setText(pNumericalDisplayTextObject->getText().c_str());

	m_keypadDisplayDefaultColor = pNumericalDisplayTextObject->getTextColor();

	//
	// Create upper separator
	//
	const ui_objectinfo_t* pUpperSeparatorObject = pWindowDesc->getObject(UI_OBJECT_SEPARATOR_H, KEYPADWINDOW_UPPER_SEPARATOR_OBJ_NAME);
	if(!pUpperSeparatorObject)
	{
		cl_engfuncs.pfnCon_EPrintf("Window description file '%s' has no definition for '%s'.\n", KEYPADWINDOW_DESC_FILE, KEYPADWINDOW_UPPER_SEPARATOR_OBJ_NAME);
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
		cl_engfuncs.pfnCon_EPrintf("Failed to initialize 'CGameUIHorizontalSeparator' object named '%s'.\n", KEYPADWINDOW_UPPER_SEPARATOR_OBJ_NAME);
		return false;
	}

	//
	// Create the button tab
	//
	const ui_objectinfo_t* pKeypadButtonSurfaceObject = pWindowDesc->getObject(UI_OBJECT_TAB, KEYPADWINDOW_BUTTON_TAB_OBJ_NAME);
	if(!pKeypadButtonSurfaceObject)
	{
		cl_engfuncs.pfnCon_EPrintf("Window description file '%s' has no definition for '%s'.\n", KEYPADWINDOW_DESC_FILE, KEYPADWINDOW_BUTTON_TAB_OBJ_NAME);
		return false;
	}

	// Create the numerical display text tab
	CGameUISurface *pButtonTab = new CGameUISurface(
		pKeypadButtonSurfaceObject->getFlags(),
		pWindowObject->getXInset() + pKeypadButtonSurfaceObject->getXOrigin(),
		pWindowObject->getYInset() + pKeypadButtonSurfaceObject->getYOrigin(),
		pKeypadButtonSurfaceObject->getWidth(),
		pKeypadButtonSurfaceObject->getHeight());
	pButtonTab->setParent(this);

	if(!pButtonTab->initSchema(pKeypadButtonSurfaceObject->getSchema().c_str()))
	{
		cl_engfuncs.pfnCon_EPrintf("Failed to initialize 'CGameUIScrollableSurface' object named '%s'.\n", KEYPADWINDOW_NUMERICAL_DISPLAY_OBJ_NAME);
		return false;
	}
	
	//
	// Create numerical buttons
	//

	for(Uint32 i = 0; i <= 9; i++)
	{
		// Replace token with digit
		CString buttonObjectName(KEYPADWINDOW_NUMERICAL_BUTTON_OBJ_NAME);
		buttonObjectName << i;

		const ui_objectinfo_t* pKeypadNumericalButtonObject = pWindowDesc->getObject(UI_OBJECT_BUTTON, buttonObjectName.c_str());
		if(!pKeypadNumericalButtonObject)
		{
			cl_engfuncs.pfnCon_EPrintf("Window description file '%s' has no definition for '%s'.\n", KEYPADWINDOW_DESC_FILE, buttonObjectName.c_str());
			return false;
		}

		CString buttonCharacter;
		buttonCharacter << (Int32)(i);

		CArray<SDL_Keycode> codesArray;
		codesArray.push_back(SDLK_0+i);
		codesArray.push_back(SDLK_KP_0+i);

		// Create UI object
		CGameUIKeypadWindowDigitCallbackEvent* pNumericalBtnEvent = new CGameUIKeypadWindowDigitCallbackEvent(this, buttonCharacter[0]);
		CGameUIButton* pNumericalButton = new CGameUIButton(pKeypadNumericalButtonObject->getFlags(), 
			pNumericalBtnEvent, 
			codesArray,
			pWindowObject->getXInset() + pKeypadNumericalButtonObject->getXOrigin(),
			pWindowObject->getYInset() + pKeypadNumericalButtonObject->getYOrigin(),
			pKeypadNumericalButtonObject->getWidth(),
			pKeypadNumericalButtonObject->getHeight());

		if(!pNumericalButton->initSchema(pKeypadNumericalButtonObject->getSchema().c_str()))
		{
			cl_engfuncs.pfnCon_EPrintf("Failed to initialize 'CGameUIButton'.\n");
			return false;
		}

		pNumericalButton->setText(buttonCharacter.c_str());
		pNumericalButton->setParent(pButtonTab);
	}

	//
	// Create other buttons
	//

	// Other button count
	const Uint32 OTHER_BTN_COUNT = 4;

	// UI schema object names
	CString buttonNamesArray[OTHER_BTN_COUNT] = {
		KEYPADWINDOW_OK_BUTTON_OBJ_NAME,
		KEYPADWINDOW_CANCEL_BUTTON_OBJ_NAME,
		KEYPADWINDOW_DELETE_BUTTON_OBJ_NAME,
		KEYPADWINDOW_CLEAR_BUTTON_OBJ_NAME
	};

	// Keys used by buttons
	Char otherBtnsKeysArray[OTHER_BTN_COUNT] = {
		SDLK_RETURN,
		SDLK_ESCAPE,
		SDLK_BACKSPACE,
		SDLK_DELETE
	};

	// Keypad button ids array
	keypadbutton_t keypabButtonTypesArray[OTHER_BTN_COUNT] = {
		KEYPADWINDOW_BUTTON_OK,
		KEYPADWINDOW_BUTTON_CANCEL,
		KEYPADWINDOW_BUTTON_DELETE,
		KEYPADWINDOW_BUTTON_CLEAR
	};

	for(Uint32 i = 0; i < OTHER_BTN_COUNT; i++)
	{
		CArray<SDL_Keycode> keysArray;
		keysArray.push_back(otherBtnsKeysArray[i]);

		switch(otherBtnsKeysArray[i])
		{
		case SDLK_RETURN:
			keysArray.push_back(SDLK_KP_ENTER);
			break;
		case SDLK_DELETE:
			keysArray.push_back(SDLK_KP_PERIOD);
			break;
		}

		const ui_objectinfo_t* pKeyButtonObject = pWindowDesc->getObject(UI_OBJECT_BUTTON, buttonNamesArray[i].c_str());
		if(!pKeyButtonObject)
		{
			cl_engfuncs.pfnCon_EPrintf("Window description file '%s' has no definition for '%s'.\n", KEYPADWINDOW_DESC_FILE, buttonNamesArray[i].c_str());
			return false;
		}

		// Create UI object
		CGameUIKeypadWindowButtonCallbackEvent* pNumericalBtnEvent = new CGameUIKeypadWindowButtonCallbackEvent(this, keypabButtonTypesArray[i]);
		CGameUIButton* pKeyButton = new CGameUIButton(pKeyButtonObject->getFlags(), 
			pNumericalBtnEvent, 
			keysArray,
			pWindowObject->getXInset() + pKeyButtonObject->getXOrigin(),
			pWindowObject->getYInset() + pKeyButtonObject->getYOrigin(),
			pKeyButtonObject->getWidth(),
			pKeyButtonObject->getHeight());

		if(!pKeyButton->initSchema(pKeyButtonObject->getSchema().c_str()))
		{
			cl_engfuncs.pfnCon_EPrintf("Failed to initialize 'CGameUIButton'.\n");
			return false;
		}

		pKeyButton->setText(pKeyButtonObject->getText().c_str());
		pKeyButton->setParent(pButtonTab);
	}

	//
	// Create lower separator
	//
	const ui_objectinfo_t* pLowerSeparatorObject = pWindowDesc->getObject(UI_OBJECT_SEPARATOR_H, KEYPADWINDOW_LOWER_SEPARATOR_OBJ_NAME);
	if(!pLowerSeparatorObject)
	{
		cl_engfuncs.pfnCon_EPrintf("Window description file '%s' has no definition for '%s'.\n", KEYPADWINDOW_DESC_FILE, KEYPADWINDOW_LOWER_SEPARATOR_OBJ_NAME);
		return false;
	}

	m_pLowerSeparator = new CGameUIHorizontalSeparator(
		pLowerSeparatorObject->getFlags(), 
		pLowerSeparatorObject->getWidth(), 
		pLowerSeparatorObject->getHeight(), 
		pWindowObject->getXInset() + pLowerSeparatorObject->getXOrigin(), 
		pWindowObject->getYInset() + pLowerSeparatorObject->getYOrigin());
	m_pLowerSeparator->setParent(this);

	if(!m_pLowerSeparator->initSchema(pUpperSeparatorObject->getSchema().c_str()))
	{
		cl_engfuncs.pfnCon_EPrintf("Failed to initialize 'CGameUIHorizontalSeparator' object named '%s'.\n", KEYPADWINDOW_UPPER_SEPARATOR_OBJ_NAME);
		return false;
	}

	// Set as initially invisible
	m_pLowerSeparator->setVisible(false);

	//
	// Create the Notes tab surface
	//
	const ui_objectinfo_t* pKeypadNotesSurfaceObject = pWindowDesc->getObject(UI_OBJECT_TAB, KEYPADWINDOW_NOTES_SURFACE_OBJ_NAME);
	if(!pKeypadNotesSurfaceObject)
	{
		cl_engfuncs.pfnCon_EPrintf("Window description file '%s' has no definition for '%s'.\n", KEYPADWINDOW_DESC_FILE, KEYPADWINDOW_NOTES_SURFACE_OBJ_NAME);
		return false;
	}

	// Create the numerical display text tab
	m_pNotesSurface = new CGameUISurface(
		pKeypadNotesSurfaceObject->getFlags(),
		pWindowObject->getXInset() + pKeypadNotesSurfaceObject->getXOrigin(),
		pWindowObject->getYInset() + pKeypadNotesSurfaceObject->getYOrigin(),
		pKeypadNotesSurfaceObject->getWidth(),
		pKeypadNotesSurfaceObject->getHeight());
	m_pNotesSurface->setParent(this);

	if(!m_pNotesSurface->initSchema(pKeypadNotesSurfaceObject->getSchema().c_str()))
	{
		cl_engfuncs.pfnCon_EPrintf("Failed to initialize 'CGameUIScrollableSurface' object named '%s'.\n", KEYPADWINDOW_NUMERICAL_DISPLAY_OBJ_NAME);
		return false;
	}

	// Set as initially invisible
	m_pNotesSurface->setVisible(false);

	//
	// Create "Notes" title text object
	//
	const ui_objectinfo_t* pNotesTitleTextObject = pWindowDesc->getObject(UI_OBJECT_TEXT, KEYPADWINDOW_NOTES_TITLE_TEXT_OBJ_NAME);
	if(!pNotesTitleTextObject)
	{
		cl_engfuncs.pfnCon_EPrintf("Window description file '%s' has no definition for '%s'.\n", KEYPADWINDOW_DESC_FILE, KEYPADWINDOW_NOTES_TITLE_TEXT_OBJ_NAME);
		return false;
	}

	pFontSet = g_pGUIManager->GetSchemaResolutionFont(pNotesTitleTextObject->getTextSchemaName().c_str());

	CGameUIText* pNotesTitleText = new CGameUIText(
		pNotesTitleTextObject->getFlags(), 
		pNotesTitleTextObject->getTextColor(), 
		pFontSet, 
		pWindowObject->getXInset() + pNotesTitleTextObject->getXOrigin(), 
		pWindowObject->getYInset() + pNotesTitleTextObject->getYOrigin());

	pNotesTitleText->setParent(m_pNotesSurface);
	pNotesTitleText->setText(pNotesTitleTextObject->getText().c_str());

	//
	// Create "Notes" passcode label text object
	//
	const ui_objectinfo_t* pNotesPasscodeLabelTextObject = pWindowDesc->getObject(UI_OBJECT_TEXT, KEYPADWINDOW_NOTES_PASSCODE_LABEL_TEXT_OBJ_NAME);
	if(!pNotesPasscodeLabelTextObject)
	{
		cl_engfuncs.pfnCon_EPrintf("Window description file '%s' has no definition for '%s'.\n", KEYPADWINDOW_DESC_FILE, KEYPADWINDOW_NOTES_PASSCODE_LABEL_TEXT_OBJ_NAME);
		return false;
	}

	pFontSet = g_pGUIManager->GetSchemaResolutionFont(pNotesPasscodeLabelTextObject->getTextSchemaName().c_str());

	CGameUIText* pNotesPasscodeLabelText = new CGameUIText(
		pNotesPasscodeLabelTextObject->getFlags(), 
		pNotesPasscodeLabelTextObject->getTextColor(), 
		pFontSet, 
		pWindowObject->getXInset() + pNotesPasscodeLabelTextObject->getXOrigin(), 
		pWindowObject->getYInset() + pNotesPasscodeLabelTextObject->getYOrigin());

	pNotesPasscodeLabelText->setParent(m_pNotesSurface);
	pNotesPasscodeLabelText->setText(pNotesPasscodeLabelTextObject->getText().c_str());

	//
	// Create "Notes" passcode text object
	//
	const ui_objectinfo_t* pNotesPasscodeTextObject = pWindowDesc->getObject(UI_OBJECT_TEXT, KEYPADWINDOW_NOTES_PASSCODE_TEXT_OBJ_NAME);
	if(!pNotesPasscodeTextObject)
	{
		cl_engfuncs.pfnCon_EPrintf("Window description file '%s' has no definition for '%s'.\n", KEYPADWINDOW_DESC_FILE, KEYPADWINDOW_NOTES_PASSCODE_TEXT_OBJ_NAME);
		return false;
	}

	pFontSet = g_pGUIManager->GetSchemaResolutionFont(pNotesPasscodeTextObject->getTextSchemaName().c_str());

	m_pTextPasscode = new CGameUIText(
		pNotesPasscodeTextObject->getFlags(), 
		pNotesPasscodeTextObject->getTextColor(), 
		pFontSet, 
		pWindowObject->getXInset() + pNotesPasscodeTextObject->getXOrigin(), 
		pWindowObject->getYInset() + pNotesPasscodeTextObject->getYOrigin());

	m_pTextPasscode->setParent(m_pNotesSurface);
	m_pTextPasscode->setText(pNotesPasscodeTextObject->getText().c_str());

	return true;
}

//====================================
//
//====================================
bool CGameUIKeypadWindow::initData( const Char* pstrPasscode, const Char* pstrInput, bool stayTillNext )
{
	if(pstrPasscode && qstrlen(pstrPasscode) > 0)
	{
		// Set the passcode
		m_passcode = pstrPasscode;

		// Verify that it's a valid digit
		if(Common::IsNumber(pstrPasscode))
		{
			m_pNotesSurface->setVisible(true);
			m_pLowerSeparator->setVisible(true);
			m_pTextPasscode->setText(pstrPasscode);
			m_pTextPasscode->setVisible(true);
		}
	}

	// If this is available, set input code
	if(pstrInput)
	{
		m_inputCode = pstrInput;
		m_pKeypadDisplayText->setText(pstrInput);
	}

	m_stayTillNextWindow = stayTillNext;
	if(m_stayTillNextWindow)
		m_windowFlags |= FL_WINDOW_WAIT_TILL_NEXT;

	return true;
}

//====================================
//
//====================================
void CGameUIKeypadWindow::getInformation( CString& passcode, CString& input, bool& stayTillNext ) const
{
	passcode = m_passcode;
	input = m_inputCode;
	stayTillNext = m_stayTillNextWindow;
}

//====================================
//
//====================================
void CGameUIKeypadWindow::ManageButtonEvent( keypadbutton_t event )
{
	switch(event)
	{
	case KEYPADWINDOW_BUTTON_CLEAR:
		{
			if(!m_inputCode.empty())
			{
				m_inputCode.clear();
				m_pKeypadDisplayText->setText("");
			}

			// Play the blip sound
			cl_engfuncs.pfnPlayAmbientSound(0, ZERO_VECTOR, SND_CHAN_AUTO, GAMEUI_BLIP_SOUND, VOL_NORM, ATTN_NORM, PITCH_NORM, SND_FL_2D, 0);
		}
		break;
	case KEYPADWINDOW_BUTTON_DELETE:
		{
			if(!m_inputCode.empty())
			{
				m_inputCode.erase(m_inputCode.length()-1, 1);
				m_pKeypadDisplayText->setText(m_inputCode.c_str());
			}

			// Play the blip sound
			cl_engfuncs.pfnPlayAmbientSound(0, ZERO_VECTOR, SND_CHAN_AUTO, GAMEUI_BLIP_SOUND, VOL_NORM, ATTN_NORM, PITCH_NORM, SND_FL_2D, 0);
		}
		break;
	case KEYPADWINDOW_BUTTON_CANCEL:
		{
			// Play the blip sound
			cl_engfuncs.pfnPlayAmbientSound(0, ZERO_VECTOR, SND_CHAN_AUTO, GAMEUI_BLIP_SOUND, VOL_NORM, ATTN_NORM, PITCH_NORM, SND_FL_2D, 0);
			m_windowFlags |= CGameUIWindow::FL_WINDOW_KILLME;
		}
		break;
	case KEYPADWINDOW_BUTTON_OK:
		{
			if(!m_passcode.empty() && !qstrcmp(m_inputCode, m_passcode))
			{
				cl_engfuncs.pfnPlayAmbientSound(0, ZERO_VECTOR, SND_CHAN_AUTO, GAMEUI_OK_SOUND, VOL_NORM, ATTN_NORM, PITCH_NORM, SND_FL_2D, 0);
				setDelayedRemoval(GAMEUIWINDOW_REMOVE_DELAY_TIME);

				m_pKeypadDisplayText->setText("Code accepted");
				m_pKeypadDisplayText->setColor(KEYPADWINDOW_PROMPT_SUCCESS_TEXT_COLOR);
				m_codeAccepted = true;
			}
			else
			{
				cl_engfuncs.pfnPlayAmbientSound(0, ZERO_VECTOR, SND_CHAN_AUTO, GAMEUI_FAIL_SOUND, VOL_NORM, ATTN_NORM, PITCH_NORM, SND_FL_2D, 0);
			
				m_pKeypadDisplayText->setText("Invalid code");
				m_pKeypadDisplayText->setColor(KEYPADWINDOW_PROMPT_FAIL_TEXT_COLOR);
				m_promptTextTime = cl_engfuncs.pfnGetClientTime() + KEYPADWINDOW_PROMPT_LIFETIME;

				m_inputCode.clear();
			}
		}
		break;
	}
}

//====================================
//
//====================================
void CGameUIKeypadWindow::ManageDigitButtonEvent( Char digit )
{
	// Reset text color
	if(m_promptTextTime)
	{
		m_promptTextTime = 0;
		m_pKeypadDisplayText->setColor(m_keypadDisplayDefaultColor);
	}

	// Set the digit
	m_inputCode << digit;
	m_pKeypadDisplayText->setText(m_inputCode.c_str());

	// Play the blip sound
	cl_engfuncs.pfnPlayAmbientSound(0, ZERO_VECTOR, SND_CHAN_AUTO, GAMEUI_BLIP_SOUND, VOL_NORM, ATTN_NORM, PITCH_NORM, SND_FL_2D, 0);
}

//====================================
//
//====================================
void CGameUIKeypadWindow::think( void )
{
	if(m_promptTextTime && m_promptTextTime <= cl_engfuncs.pfnGetClientTime())
	{
		m_pKeypadDisplayText->setText("");
		m_pKeypadDisplayText->setColor(m_keypadDisplayDefaultColor);
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
void CGameUIKeypadWindow::setDelayedRemoval( Double delay )
{
	if(m_windowFlags & FL_WINDOW_WAIT_TILL_NEXT)
		m_messageSendTime = cl_engfuncs.pfnGetClientTime() + delay;

	CGameUIWindow::setDelayedRemoval(delay);
}

//====================================
//
//====================================
void CGameUIKeypadWindow::onRemove( void ) 
{
	if(!m_codeAccepted)
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
		cl_engfuncs.pfnMsgWriteByte(GAMEUI_KEYPADWINDOW);
	cl_engfuncs.pfnClientUserMessageEnd();
}

//====================================
//
//====================================
CGameUIKeypadWindow* CGameUIKeypadWindow::CreateInstance( void )
{
	// Load the schema file
	ui_windowdescription_t* pWinDesc = g_pGUIManager->LoadWindowDescriptionFile(KEYPADWINDOW_OBJ_NAME, KEYPADWINDOW_DESC_FILE);
 	if(!pWinDesc)
	{
		cl_engfuncs.pfnCon_EPrintf("Failed to load window description '%s' for '%s'.\n", KEYPADWINDOW_DESC_FILE, KEYPADWINDOW_OBJ_NAME);
		return nullptr;
	}

	const ui_objectinfo_t* pWindowObject = pWinDesc->getObject(UI_OBJECT_WINDOW, KEYPADWINDOW_OBJ_NAME);
	if(!pWindowObject)
	{
		cl_engfuncs.pfnCon_EPrintf("Window description file '%s' has no definition for '%s'.\n", KEYPADWINDOW_DESC_FILE, KEYPADWINDOW_OBJ_NAME);
		return nullptr;
	}

	Uint32 screenWidth, screenHeight;
	cl_renderfuncs.pfnGetScreenSize(screenWidth, screenHeight);

	Uint32 relativeWidth = gHUDDraw.ScaleY(pWindowObject->getWidth());
	Uint32 relativeHeight = gHUDDraw.ScaleY(pWindowObject->getHeight());

	Int32 xPosition = (screenWidth / 2) - (relativeWidth / 2);
	Int32 yPosition = (screenHeight / 2) - (relativeHeight / 2);

	CGameUIKeypadWindow* pNewWindow = new CGameUIKeypadWindow(CGameUIWindow::FL_WINDOW_NONE, xPosition, yPosition, relativeWidth, relativeHeight);
	if(!pNewWindow->init(pWinDesc, pWindowObject))
	{
		cl_engfuncs.pfnCon_EPrintf("%s - Failed to initialize 'CGameUIKeypadWindow'.\n", __FUNCTION__);
		delete pNewWindow;
		return nullptr;
	}

	return pNewWindow;
}

//====================================
//
//====================================
void CGameUIKeypadWindowButtonCallbackEvent::PerformAction( Float param )
{
	if(!m_pKeypadWindow)
		return;

	m_pKeypadWindow->ManageButtonEvent(m_button);
}

//====================================
//
//====================================
void CGameUIKeypadWindowDigitCallbackEvent::PerformAction( Float param )
{
	if(!m_pKeypadWindow)
		return;

	m_pKeypadWindow->ManageDigitButtonEvent(m_digit);
}
