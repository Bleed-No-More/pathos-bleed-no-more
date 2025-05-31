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

#include "gameuitextwindow.h"
#include "gameuiwindows_shared.h"
#include "gameuimanager.h"

// Window description file
const Char CGameUITextWindow::TEXTWINDOW_DESC_FILE[] = "gameuitextwindow.txt";
// Window description file
const Char CGameUITextWindow::TEXTWINDOW_OBJ_NAME[] = "GameUITextWindow";
// Window title text object name
const Char CGameUITextWindow::TEXTWINDOW_TITLE_TEXT_OBJ_NAME[] = "GameUITextWindowTitleText";
// Window title text tab object name
const Char CGameUITextWindow::TEXTWINDOW_TITLE_TEXT_TAB_OBJ_NAME[] = "GameUITextWindowTextTab";
// Window exit button object name
const Char CGameUITextWindow::TEXTWINDOW_EXIT_BUTTON_OBJ_NAME[] = "GameUITextWindowExitButton";

//====================================
//
//====================================
CGameUITextWindow::CGameUITextWindow( Int32 flags, Int32 originX, Int32 originY, Uint32 width, Uint32 height ):
	CGameUIWindow(flags, originX, originY, width, height),
	m_pTitleText(nullptr),
	m_pTextTab(nullptr)
{
}

//====================================
//
//====================================
CGameUITextWindow::~CGameUITextWindow( void )
{
}

//====================================
//
//====================================
void CGameUITextWindow::onRemove( void )
{
	// Unpause the game
	cl_engfuncs.pfnSetPaused(false, false);

	// Call base class to manage rest
	CGameUIWindow::onRemove();
}

//====================================
//
//====================================
bool CGameUITextWindow::init( const ui_windowdescription_t* pWindowDesc, const ui_objectinfo_t* pWindowObject )
{
	// Initialize schema for surface object
	if(!CGameUISurface::initSchema(pWindowObject->getSchema().c_str()))
		return false;

	//
	// Create title text object
	//
 	const ui_objectinfo_t* pTitleTextObject = pWindowDesc->getObject(UI_OBJECT_TEXT, TEXTWINDOW_TITLE_TEXT_OBJ_NAME);
	if(!pTitleTextObject)
	{
		cl_engfuncs.pfnCon_EPrintf("Window description file '%s' has no definition for '%s'.\n", TEXTWINDOW_DESC_FILE, TEXTWINDOW_TITLE_TEXT_OBJ_NAME);
		return false;
	}

	const font_set_t* pFontSet = g_pGUIManager->GetSchemaResolutionFont(pTitleTextObject->getTextSchemaName().c_str());

	m_pTitleText = new CGameUIText(
		pTitleTextObject->getFlags(), 
		pTitleTextObject->getTextColor(), 
		pFontSet, 
		pWindowObject->getXInset() + pTitleTextObject->getXOrigin(), 
		pWindowObject->getYInset() + pTitleTextObject->getYOrigin());

	m_pTitleText->setParent(this);
	m_pTitleText->setText(pTitleTextObject->getText().c_str());

	//
	// Create the text tab object
	//
	const ui_objectinfo_t* pTextTabObject = pWindowDesc->getObject(UI_OBJECT_TEXT_TAB, TEXTWINDOW_TITLE_TEXT_TAB_OBJ_NAME);
	if(!pTextTabObject)
	{
		cl_engfuncs.pfnCon_EPrintf("Window description file '%s' has no definition for '%s'.\n", TEXTWINDOW_DESC_FILE, TEXTWINDOW_TITLE_TEXT_TAB_OBJ_NAME);
		return false;
	}

	pFontSet = g_pGUIManager->GetSchemaResolutionFont(pTextTabObject->getTextSchemaName().c_str());

	m_pTextTab = new CGameUITextTab(pTextTabObject->getFlags(), 
		pFontSet,
		pTextTabObject->getTextInset(),
		pTextTabObject->getTextColor(),
		pWindowObject->getXInset() + pTextTabObject->getXOrigin(),
		pWindowObject->getYInset() + pTextTabObject->getYOrigin(),
		pTextTabObject->getWidth(),
		pTextTabObject->getHeight());
	m_pTextTab->setParent(this);

	if(!m_pTextTab->initSchema(pTextTabObject->getSchema().c_str(), pTextTabObject->getButtonSchema().c_str()))
	{
		cl_engfuncs.pfnCon_EPrintf("Failed to initialize 'CGameUITextTab'.\n");
		return false;
	}

	//
	// Create the "exit" button
	//
	const ui_objectinfo_t* pExitButtonObject = pWindowDesc->getObject(UI_OBJECT_BUTTON, TEXTWINDOW_EXIT_BUTTON_OBJ_NAME);
	if(!pExitButtonObject)
	{
		cl_engfuncs.pfnCon_EPrintf("Window description file '%s' has no definition for '%s'.\n", TEXTWINDOW_DESC_FILE, TEXTWINDOW_EXIT_BUTTON_OBJ_NAME);
		return false;
	}

	CGameUITextWindowExitCallbackEvent* pEvent = new CGameUITextWindowExitCallbackEvent(this);
	CGameUIButton* pButton = new CGameUIButton(pExitButtonObject->getFlags(), 
		pEvent, 
		SDLK_RETURN,
		pWindowObject->getXInset() + pExitButtonObject->getXOrigin(),
		pWindowObject->getYInset() + pExitButtonObject->getYOrigin(),
		pExitButtonObject->getWidth(),
		pExitButtonObject->getHeight());

	if(!pButton->initSchema(pExitButtonObject->getSchema().c_str()))
	{
		cl_engfuncs.pfnCon_EPrintf("Failed to initialize 'CGameUIButton'.\n");
		return false;
	}

	pButton->setText(pExitButtonObject->getText().c_str());
	pButton->setParent(this);

	return true;
}

//====================================
//
//====================================
bool CGameUITextWindow::initData( const Char* pstrtextfilepath, const Char* pstrPassCode )
{
	// Title text
	CString titletext;
	// Font set name
	CString titletextschema;
	// Text color
	color32_t titletextcolor = GAMEUIWINDOW_DEFAULT_TEXT_COLOR;
	// TRUE if text color got set
	bool titleColorWasSet = false;
	// Font set name
	CString textschema;
	// Text color
	color32_t textcolor = GAMEUIWINDOW_DEFAULT_TEXT_COLOR;
	// TRUE if text color got set
	bool textColorWasSet = false;

	// Process the document to be used
	CString textcontents;
	if(!ProcessTextFile(pstrtextfilepath, pstrPassCode, titletext, titletextschema, titletextcolor, titleColorWasSet, textschema, textcolor, textColorWasSet, textcontents))
		return false;

	Uint32 screenWidth, screenHeight;
	cl_renderfuncs.pfnGetScreenSize(screenWidth, screenHeight);

	// Load font if not default
	const font_set_t* pfontset = nullptr;
	if(!textschema.empty())
		pfontset = cl_engfuncs.pfnGetResolutionSchemaFontSet(textschema.c_str(), screenHeight);

	const font_set_t* ptitlefontset = nullptr;
	if(!titletextschema.empty())
		ptitlefontset = cl_engfuncs.pfnGetResolutionSchemaFontSet(titletextschema.c_str(), screenHeight);

	if(pfontset)
		m_pTextTab->setFontSet(pfontset);

	if(textColorWasSet)
		m_pTextTab->setTextColor(textcolor);

	m_pTextTab->initData(reinterpret_cast<const byte*>(textcontents.c_str()), textcontents.length());

	if(ptitlefontset)
		m_pTitleText->setFontSet(ptitlefontset);

	if(titleColorWasSet)
		m_pTitleText->setColor(titletextcolor);

	// Set title if specified
	if(!titletext.empty())
		m_pTitleText->setText(titletext.c_str());

	// Set these
	m_textFilePath = pstrtextfilepath;
	m_passcode = pstrPassCode;

	// Pause the game
	cl_engfuncs.pfnSetPaused(true, true);

	return true;
}

//====================================
//
//====================================
void CGameUITextWindow::getInformation( CString& textfilepath, CString& passcode ) const
{
	textfilepath = m_textFilePath;
	passcode = m_passcode;
}

//====================================
//
//====================================
void CGameUITextWindow::CloseWindow( void )
{
	// Flag for removal
	m_windowFlags |= CGameUIWindow::FL_WINDOW_KILLME;

	// Unpause the game
	cl_engfuncs.pfnSetPaused(false, false);
}

//====================================
//
//====================================
bool CGameUITextWindow::ProcessTextFile( const Char* pstrFilePath, const CString passCode, CString& titleText, CString& titleTextSchema, color32_t& titleTextColor, bool& titleColorWasSet, CString& textSchema, color32_t& textColor, bool& textColorWasSet, CString& documentText )
{
	const byte* pfile = cl_filefuncs.pfnLoadFile(pstrFilePath, nullptr);
	if(!pfile)
	{
		cl_engfuncs.pfnCon_Printf("%s - Could not load file '%s'.\n", __FUNCTION__, pstrFilePath);
		return false;
	}

	// First get all options
	CString token;
	const Char* pstr = reinterpret_cast<const Char*>(pfile);
	while(pstr)
	{
		// Parse the token
		pstr = Common::Parse(pstr, token);
		if(!pstr)
		{
			cl_engfuncs.pfnCon_Printf("%s - Unexpected end of file in '%s'.\n", __FUNCTION__, pstrFilePath);
			cl_filefuncs.pfnFreeFile(pfile);
			return false;
		}

		if(token[0] == '$')
		{
			// Read in the next parameter
			CString paramvalue;
			pstr = Common::Parse(pstr, paramvalue);

			// Need to handle a parameter
			if(!qstrcmp(token, "$title"))
			{
				// Set title
				titleText = paramvalue;
			}
			else if(!qstrcmp(token, "$textschema"))
			{
				// Set the font set name
				textSchema = paramvalue;
			}
			else if(!qstrcmp(token, "$titletextschema"))
			{
				// Set the font set name
				titleTextSchema = paramvalue;
			}
			else if(!qstrcmp(token, "$color") || !qstrcmp(token, "$titlecolor"))
			{
				if(!Common::IsNumber(paramvalue.c_str()))
				{
					cl_engfuncs.pfnCon_Printf("%s - Expected a numerical value for '%s' in '%s', got '%s' instead.\n", __FUNCTION__, token.c_str(), pstrFilePath, paramvalue.c_str());
					cl_filefuncs.pfnFreeFile(pfile);
					return false;
				}

				// Read in the g component
				CString gcomponent;
				pstr = Common::Parse(pstr, gcomponent);
				if(!pstr)
				{
					cl_engfuncs.pfnCon_Printf("%s - Unexpected end of file in '%s'.\n", __FUNCTION__, pstrFilePath);
					cl_filefuncs.pfnFreeFile(pfile);
					return false;
				}
				if(!Common::IsNumber(gcomponent.c_str()))
				{
					cl_engfuncs.pfnCon_Printf("%s - Expected a numerical value for '%s' in '%s', got '%s' instead.\n", __FUNCTION__, token.c_str(), pstrFilePath, paramvalue.c_str());
					cl_filefuncs.pfnFreeFile(pfile);
					return false;
				}

				// Read in the b component
				CString bcomponent;
				pstr = Common::Parse(pstr, bcomponent);
				if(!pstr)
				{
					cl_engfuncs.pfnCon_Printf("%s - Unexpected end of file in '%s'.\n", __FUNCTION__, pstrFilePath);
					cl_filefuncs.pfnFreeFile(pfile);
					return false;
				}
				if(!Common::IsNumber(bcomponent.c_str()))
				{
					cl_engfuncs.pfnCon_Printf("%s - Expected a numerical value for '%s' in '%s', got '%s' instead.\n", __FUNCTION__, token.c_str(), pstrFilePath, paramvalue.c_str());
					cl_filefuncs.pfnFreeFile(pfile);
					return false;
				}

				// Read in the a component
				CString acomponent;
				pstr = Common::Parse(pstr, acomponent);
				if(!pstr)
				{
					cl_engfuncs.pfnCon_Printf("%s - Unexpected end of file in '%s'.\n", __FUNCTION__, pstrFilePath);
					cl_filefuncs.pfnFreeFile(pfile);
					return false;
				}
				if(!Common::IsNumber(acomponent.c_str()))
				{
					cl_engfuncs.pfnCon_Printf("%s - Expected a numerical value for '%s' in '%s', got '%s' instead.\n", __FUNCTION__, token.c_str(), pstrFilePath, paramvalue.c_str());
					cl_filefuncs.pfnFreeFile(pfile);
					return false;
				}

				// Assign the color components
				if(!qstrcmp(token, "$titlecolor"))
				{
					titleTextColor.r = SDL_atoi(paramvalue.c_str());
					titleTextColor.g = SDL_atoi(gcomponent.c_str());
					titleTextColor.b = SDL_atoi(bcomponent.c_str());
					titleTextColor.a = SDL_atoi(acomponent.c_str());
					titleColorWasSet = true;
				}
				else
				{
					textColor.r = SDL_atoi(paramvalue.c_str());
					textColor.g = SDL_atoi(gcomponent.c_str());
					textColor.b = SDL_atoi(bcomponent.c_str());
					textColor.a = SDL_atoi(acomponent.c_str());
					textColorWasSet = true;
				}
			}
			else
			{
				cl_engfuncs.pfnCon_Printf("%s - Unknown option '%s' in '%s'.\n", __FUNCTION__, token.c_str(), pstrFilePath);
				continue;
			}
		}
		else if(!qstrcmp(token, "{"))
		{
			// Time to read text contents
			break;
		}
		else
		{
			cl_engfuncs.pfnCon_Printf("%s - Expected '{' or option starting with '$', got '%s' instead in '%s'.\n", __FUNCTION__, token.c_str(), pstrFilePath);
			cl_filefuncs.pfnFreeFile(pfile);
			return false;
		}
	}

	// Safeguard twice
	if(qstrcmp(token, "{"))
	{
		cl_engfuncs.pfnCon_Printf("%s - Expected '{', got '%s' instead in '%s'.\n", __FUNCTION__, token.c_str(), pstrFilePath);
		cl_filefuncs.pfnFreeFile(pfile);
		return false;
	}

	// Find the ending bracket
	const Char* pstrend = qstrstr(pstr, "}");
	if(!pstrend)
	{
		cl_engfuncs.pfnCon_Printf("%s - Text file '%s' is missing ending bracket '}'.\n", __FUNCTION__, pstrFilePath);
		cl_filefuncs.pfnFreeFile(pfile);
		return false;
	}

	Uint32 datasize = (pstrend - pstr);
	documentText.assign(pstr, datasize);

	// Release the file
	cl_filefuncs.pfnFreeFile(pfile);

	// Replace any passcode tokens
	while(true)
	{
		Int32 tokenoffset = documentText.find(0, "%passcode%");
		if(tokenoffset == CString::CSTRING_NO_POSITION)
			break;

		documentText.erase(tokenoffset, 10);
		documentText.insert(tokenoffset, passCode.c_str());
	}

	return true;
}

//====================================
//
//====================================
CGameUITextWindow* CGameUITextWindow::CreateInstance( void )
{
	// Load the schema file
	ui_windowdescription_t* pWinDesc = g_pGUIManager->LoadWindowDescriptionFile(TEXTWINDOW_OBJ_NAME, TEXTWINDOW_DESC_FILE);
	if(!pWinDesc)
	{
		cl_engfuncs.pfnCon_EPrintf("Failed to load window description '%s' for '%s'.\n", TEXTWINDOW_DESC_FILE, TEXTWINDOW_OBJ_NAME);
		return nullptr;
	}

	const ui_objectinfo_t* pWindowObject = pWinDesc->getObject(UI_OBJECT_WINDOW, TEXTWINDOW_OBJ_NAME);
	if(!pWindowObject)
	{
		cl_engfuncs.pfnCon_EPrintf("Window description file '%s' has no definition for '%s'.\n", TEXTWINDOW_DESC_FILE, TEXTWINDOW_OBJ_NAME);
		return nullptr;
	}

	Uint32 screenWidth, screenHeight;
	cl_renderfuncs.pfnGetScreenSize(screenWidth, screenHeight);

	Int32 xPosition = (screenWidth / 2) - (pWindowObject->getWidth() / 2);
	Int32 yPosition = (screenHeight / 2) - (pWindowObject->getHeight() / 2);

	CGameUITextWindow* pNewWindow = new CGameUITextWindow(CGameUIWindow::FL_WINDOW_NONE, xPosition, yPosition, pWindowObject->getWidth(), pWindowObject->getHeight());
	if(!pNewWindow->init(pWinDesc, pWindowObject))
	{
		cl_engfuncs.pfnCon_EPrintf("%s - Failed to initialize 'CGameUITextWindow'.\n", __FUNCTION__);
		delete pNewWindow;
		return nullptr;
	}

	return pNewWindow;
}

//====================================
//
//====================================
void CGameUITextWindowExitCallbackEvent::PerformAction( Float param )
{
	if(!m_pTextWindow)
		return;

	m_pTextWindow->CloseWindow();
}

