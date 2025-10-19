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

#include "gameuisubwaywindow.h"
#include "gameuiwindows_shared.h"
#include "snd_shared.h"
#include "gameui_shared.h"

// Number of destination buttons
const Uint32 CGameUISubwayWindow::NB_DESTINATION_BUTTONS = 4;
// Title text object name
const Char CGameUISubwayWindow::SUBWAYWINDOW_OBJ_NAME[] = "GameUISubwayWindow";
// Window description file
const Char CGameUISubwayWindow::SUBWAYWINDOW_DESC_FILE[] = "gameuisubwaywindow.txt";
// Title text object name
const Char CGameUISubwayWindow::SUBWAYWINDOW_TITLE_TEXT_OBJ_NAME[] = "GameUISubwayWindowTitleText";
// Upper separator object name
const Char CGameUISubwayWindow::SUBWAYWINDOW_UPPER_SEPARATOR_OBJ_NAME[] = "GameUISubwayWindowUpperSeparator";
// Middle separator object name
const Char CGameUISubwayWindow::SUBWAYWINDOW_MIDDLE_SEPARATOR_OBJ_NAME[] = "GameUISubwayWindowMiddleSeparator";
// Lower separator object name
const Char CGameUISubwayWindow::SUBWAYWINDOW_LOWER_SEPARATOR_OBJ_NAME[] = "GameUISubwayWindowLowerSeparator";
// Destination button object name
const Char CGameUISubwayWindow::SUBWAYWINDOW_DESTINATION_BUTTON_OBJ_NAME[] = "GameUISubwayWindowDestinationButton";
// Destination text object name
const Char CGameUISubwayWindow::SUBWAYWINDOW_DESTINATION_TEXT_OBJ_NAME[] = "GameUISubwayWindowDestinationText";
// Default text object name
const Char CGameUISubwayWindow::SUBWAYWINDOW_DEFAULT_TEXT_OBJ_NAME[] = "GameUISubwayWindowDefaultText";
// Exit button object name
const Char CGameUISubwayWindow::SUBWAYWINDOW_EXIT_BUTTON_OBJ_NAME[] = "GameUISubwayWindowExitButton";

//====================================
//
//====================================
CGameUISubwayWindow::CGameUISubwayWindow( Int32 flags, Int32 originX, Int32 originY, Uint32 width, Uint32 height ):
	CGameUIWindow(flags, originX, originY, width, height),
	m_pWindowTitleText(nullptr),
	m_pDefaultDescription(nullptr),
	m_pExitButton(nullptr),
	m_subwayFlags(0),
	m_subwayLineIndex(0)
{
}

//====================================
//
//====================================
CGameUISubwayWindow::~CGameUISubwayWindow( void )
{
}

//====================================
//
//====================================
bool CGameUISubwayWindow::init( const ui_windowdescription_t* pWindowDesc, const ui_objectinfo_t* pWindowObject )
{
	// Initialize schema for surface object
	if(!CGameUISurface::initSchema(pWindowObject->getSchema().c_str()))
		return false;

	//
	// Create the title text object
	//
	const ui_objectinfo_t* pTitleTextObject = pWindowDesc->getObject(UI_OBJECT_TEXT, SUBWAYWINDOW_TITLE_TEXT_OBJ_NAME);
	if(!pTitleTextObject)
	{
		cl_engfuncs.pfnCon_EPrintf("Window description file '%s' has no definition for '%s'.\n", SUBWAYWINDOW_DESC_FILE, SUBWAYWINDOW_TITLE_TEXT_OBJ_NAME);
		return false;
	}

	const font_set_t* pTitleFont = g_pGUIManager->GetSchemaResolutionFont(pTitleTextObject->getTextSchemaName().c_str());
	if(!pTitleFont)
		pTitleFont = gGameUIManager.GetDefaultFontSet();

	m_pWindowTitleText = new CGameUIText(
		pTitleTextObject->getFlags(), 
		pTitleTextObject->getTextColor(), 
		pTitleFont, 
		pWindowObject->getXInset() + pTitleTextObject->getXOrigin(), 
		pWindowObject->getYInset() + pTitleTextObject->getYOrigin());

	m_pWindowTitleText->setParent(this);
	m_pWindowTitleText->setText(pTitleTextObject->getText().c_str());

	//
	// Create separator objects
	//

	// Create upper separator
	const ui_objectinfo_t* pUpperSeparatorObject = pWindowDesc->getObject(UI_OBJECT_SEPARATOR_H, SUBWAYWINDOW_UPPER_SEPARATOR_OBJ_NAME);
	if(!pUpperSeparatorObject)
	{
		cl_engfuncs.pfnCon_EPrintf("Window description file '%s' has no definition for '%s'.\n", SUBWAYWINDOW_DESC_FILE, SUBWAYWINDOW_UPPER_SEPARATOR_OBJ_NAME);
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
		cl_engfuncs.pfnCon_EPrintf("Failed to initialize 'CGameUIHorizontalSeparator' object named '%s'.\n", SUBWAYWINDOW_UPPER_SEPARATOR_OBJ_NAME);
		return false;
	}

	// Create middle separator
	const ui_objectinfo_t* pMiddleSeparatorObject = pWindowDesc->getObject(UI_OBJECT_SEPARATOR_H, SUBWAYWINDOW_MIDDLE_SEPARATOR_OBJ_NAME);
	if(!pMiddleSeparatorObject)
	{
		cl_engfuncs.pfnCon_EPrintf("Window description file '%s' has no definition for '%s'.\n", SUBWAYWINDOW_DESC_FILE, SUBWAYWINDOW_MIDDLE_SEPARATOR_OBJ_NAME);
		return false;
	}

	CGameUIHorizontalSeparator* pMiddleSeparator = new CGameUIHorizontalSeparator(
		pMiddleSeparatorObject->getFlags(), 
		pMiddleSeparatorObject->getWidth(), 
		pMiddleSeparatorObject->getHeight(), 
		pWindowObject->getXInset() + pMiddleSeparatorObject->getXOrigin(), 
		pWindowObject->getYInset() + pMiddleSeparatorObject->getYOrigin());
	pMiddleSeparator->setParent(this);

	if(!pMiddleSeparator->initSchema(pMiddleSeparatorObject->getSchema().c_str()))
	{
		cl_engfuncs.pfnCon_EPrintf("Failed to initialize 'CGameUIHorizontalSeparator' object named '%s'.\n", SUBWAYWINDOW_MIDDLE_SEPARATOR_OBJ_NAME);
		return false;
	}

	// Create lower separator
	const ui_objectinfo_t* pLowerSeparatorObject = pWindowDesc->getObject(UI_OBJECT_SEPARATOR_H, SUBWAYWINDOW_LOWER_SEPARATOR_OBJ_NAME);
	if(!pMiddleSeparatorObject)
	{
		cl_engfuncs.pfnCon_EPrintf("Window description file '%s' has no definition for '%s'.\n", SUBWAYWINDOW_DESC_FILE, SUBWAYWINDOW_LOWER_SEPARATOR_OBJ_NAME);
		return false;
	}

	CGameUIHorizontalSeparator* pLowerSeparator = new CGameUIHorizontalSeparator(
		pLowerSeparatorObject->getFlags(), 
		pLowerSeparatorObject->getWidth(), 
		pLowerSeparatorObject->getHeight(), 
		pWindowObject->getXInset() + pLowerSeparatorObject->getXOrigin(), 
		pWindowObject->getYInset() + pLowerSeparatorObject->getYOrigin());
	pLowerSeparator->setParent(this);

	if(!pLowerSeparator->initSchema(pLowerSeparatorObject->getSchema().c_str()))
	{
		cl_engfuncs.pfnCon_EPrintf("Failed to initialize 'CGameUIHorizontalSeparator' object named '%s'.\n", SUBWAYWINDOW_LOWER_SEPARATOR_OBJ_NAME);
		return false;
	}

	//
	// Create buttobns and their descriptions
	//
	const ui_objectinfo_t* pDestinationButtonObject = pWindowDesc->getObject(UI_OBJECT_BUTTON, SUBWAYWINDOW_DESTINATION_BUTTON_OBJ_NAME);
	if(!pDestinationButtonObject)
	{
		cl_engfuncs.pfnCon_EPrintf("Window description file '%s' has no definition for '%s'.\n", SUBWAYWINDOW_DESC_FILE, SUBWAYWINDOW_DESTINATION_BUTTON_OBJ_NAME);
		return false;
	}

	const ui_objectinfo_t* pDestinationTextObject = pWindowDesc->getObject(UI_OBJECT_TEXT, SUBWAYWINDOW_DESTINATION_TEXT_OBJ_NAME);
	if(!pDestinationTextObject)
	{
		cl_engfuncs.pfnCon_EPrintf("Window description file '%s' has no definition for '%s'.\n", SUBWAYWINDOW_DESC_FILE, SUBWAYWINDOW_DESTINATION_TEXT_OBJ_NAME);
		return false;
	}

	// Allocate buttons
	m_buttonsArray.resize(NB_DESTINATION_BUTTONS);

	Int32 yOffset = 0;
	for(Uint32 i = 0; i < NB_DESTINATION_BUTTONS; i++)
	{
		CGameUISubwayWindowCallbackEvent* pEvent = new CGameUISubwayWindowCallbackEvent(this, SUBWAY_BUTTON_1+i);
		CGameUIButton* pDestButton = new CGameUIButton(pDestinationButtonObject->getFlags(), 
			pEvent, 
			pWindowObject->getXInset() + pDestinationButtonObject->getXOrigin(),
			pWindowObject->getYInset() + pDestinationButtonObject->getYOrigin() + yOffset,
			pDestinationButtonObject->getWidth(),
			pDestinationButtonObject->getHeight());

		if(!pDestButton->initSchema(pDestinationButtonObject->getSchema().c_str()))
		{
			cl_engfuncs.pfnCon_EPrintf("Failed to initialize 'CGameUIButton'.\n");
			return false;
		}

		pDestButton->setText(pDestinationButtonObject->getText().c_str());
		pDestButton->setParent(this);
		m_buttonsArray[i].pButton = pDestButton;

		yOffset += pDestButton->getHeight() + pDestinationButtonObject->getListPostSpacing();

		CGameUIText* pDescriptionText = new CGameUIText(
			pDestinationTextObject->getFlags(), 
			pDestinationTextObject->getTextColor(), 
			pDestinationTextObject->getFont(),
			pDestinationTextObject->getText().c_str(),
			pWindowObject->getXInset() + pDestinationTextObject->getXOrigin(), 
			pWindowObject->getYInset() + pDestinationTextObject->getYOrigin(),
			pDestinationTextObject->getWidth(),
			pDestinationTextObject->getHeight(),
			pDestinationTextObject->getTextInset());

		pDescriptionText->setParent(this);
		pDescriptionText->setText(pDestinationTextObject->getText().c_str());
		m_buttonsArray[i].pDescription = pDescriptionText;
	}

	// Create default description
	const ui_objectinfo_t* pDefaultTextObject = pWindowDesc->getObject(UI_OBJECT_TEXT, SUBWAYWINDOW_DEFAULT_TEXT_OBJ_NAME);
	if(!pDefaultTextObject)
	{
		cl_engfuncs.pfnCon_EPrintf("Window description file '%s' has no definition for '%s'.\n", SUBWAYWINDOW_DESC_FILE, SUBWAYWINDOW_DEFAULT_TEXT_OBJ_NAME);
		return false;
	}

	m_pDefaultDescription = new CGameUIText(
		pDefaultTextObject->getFlags(), 
		pDefaultTextObject->getTextColor(), 
		pDefaultTextObject->getFont(),
		pDefaultTextObject->getText().c_str(),
		pWindowObject->getXInset() + pDefaultTextObject->getXOrigin(), 
		pWindowObject->getYInset() + pDefaultTextObject->getYOrigin(),
		pDefaultTextObject->getWidth(),
		pDefaultTextObject->getHeight(),
		pDefaultTextObject->getTextInset());

	m_pDefaultDescription->setParent(this);
	m_pDefaultDescription->setText(pDestinationTextObject->getText().c_str());

	// Create the exit button
	const ui_objectinfo_t* pExitButtonObject = pWindowDesc->getObject(UI_OBJECT_BUTTON, SUBWAYWINDOW_EXIT_BUTTON_OBJ_NAME);
	if(!pExitButtonObject)
	{
		cl_engfuncs.pfnCon_EPrintf("Window description file '%s' has no definition for '%s'.\n", SUBWAYWINDOW_DESC_FILE, SUBWAYWINDOW_EXIT_BUTTON_OBJ_NAME);
		return false;
	}

	CGameUISubwayWindowCallbackEvent* pClearEvent = new CGameUISubwayWindowCallbackEvent(this, SUBWAY_BUTTON_CANCEL);
	CGameUIButton* pExitButton = new CGameUIButton(pExitButtonObject->getFlags(),
		pClearEvent,
		SDLK_ESCAPE,
		pWindowObject->getXInset() + pExitButtonObject->getXOrigin(),
		pWindowObject->getYInset() + pExitButtonObject->getYOrigin(),
		pExitButtonObject->getWidth(),
		pExitButtonObject->getHeight());

	if(!pExitButton->initSchema(pDestinationButtonObject->getSchema().c_str()))
	{
		cl_engfuncs.pfnCon_EPrintf("Failed to initialize 'CGameUIButton'.\n");
		return false;
	}

	pExitButton->setText(pExitButtonObject->getText().c_str());
	pExitButton->setParent(this);
	return true;
}


//====================================
//
//====================================
void CGameUISubwayWindow::think( void )
{
	bool hasVisible = false;
	for(Uint32 i = 0; i < m_buttonsArray.size(); i++)
	{
		if(m_buttonsArray[i].pButton->isMouseOver())
		{
			if(!m_buttonsArray[i].pDescription->isVisible())
				m_buttonsArray[i].pDescription->setVisible(true);

			hasVisible = true;
		}
		else
		{
			if(m_buttonsArray[i].pDescription->isVisible())
				m_buttonsArray[i].pDescription->setVisible(false);
		}
	}

	if(!hasVisible && !m_pDefaultDescription->isVisible())
		m_pDefaultDescription->setVisible(true);
	else if(hasVisible && m_pDefaultDescription->isVisible())
		m_pDefaultDescription->setVisible(false);

	// Call base class to manage think functions
	CGameUIWindow::think();
}

//====================================
//
//====================================
bool CGameUISubwayWindow::initData( const Char* pstrScriptFile, Int32 flags, Int32 subwayLineIndex )
{
	const byte* pfile = cl_filefuncs.pfnLoadFile(pstrScriptFile, nullptr);
	if(!pfile)
	{
		cl_engfuncs.pfnCon_EPrintf("%s - Failed to load '%s'.\n", __FUNCTION__, pstrScriptFile);
		return false;
	}

	CArray<subwaybuttonschema_t> buttonschemas;
	buttonschemas.resize(NB_SUBWAY_BUTTON_TYPES);

	CString token;
	const Char* pstr = reinterpret_cast<const Char*>(pfile);
	while(pstr)
	{
		// Read the token in
		pstr = Common::Parse(pstr, token);

		if(!qstrcmp(token, "$title"))
		{
			// Read in the token
			pstr = Common::Parse(pstr, token);
			m_pWindowTitleText->setText(token.c_str());
			continue;
		}
		else if(!qstrcmp(token, "$button"))
		{
			// Read in the destination
			pstr = Common::Parse(pstr, token);
			if(!pstr)
			{
				cl_engfuncs.pfnCon_EPrintf("%s - Unexpected EOF while reading $button definition in '%s'.\n", __FUNCTION__, pstrScriptFile);
				cl_filefuncs.pfnFreeFile(pfile);
				return false;
			}

			subwaybuttonschematype_t type;
			if(!qstrcmp(token, "destination1"))
				type = SUBWAYWINDOW_BUTTON_DESTINATION_1;
			else if(!qstrcmp(token, "destination2"))
				type = SUBWAYWINDOW_BUTTON_DESTINATION_2;
			else if(!qstrcmp(token, "destination3"))
				type = SUBWAYWINDOW_BUTTON_DESTINATION_3;
			else if(!qstrcmp(token, "destination4"))
				type = SUBWAYWINDOW_BUTTON_DESTINATION_4;
			else if(!qstrcmp(token, "unavailable"))
				type = SUBWAYWINDOW_BUTTON_UNAVAILABLE;
			else if(!qstrcmp(token, "disabled"))
				type = SUBWAYWINDOW_BUTTON_DISABLED;
			else
			{
				cl_engfuncs.pfnCon_EPrintf("%s - Unknown $button type '%s'.\n", __FUNCTION__, token.c_str());
				cl_filefuncs.pfnFreeFile(pfile);
				return false;
			}

			// Next token should be an openig bracket
			pstr = Common::Parse(pstr, token);
			if(!pstr)
			{
				cl_engfuncs.pfnCon_EPrintf("%s - Unexpected EOF while reading $button definition in '%s'.\n", __FUNCTION__, pstrScriptFile);
				cl_filefuncs.pfnFreeFile(pfile);
				return false;
			}

			if(qstrcmp(token, "{"))
			{
				cl_engfuncs.pfnCon_EPrintf("%s - Expected '{', got %s instead in definition in '%s'.\n", __FUNCTION__, token.c_str(), pstrScriptFile);
				cl_filefuncs.pfnFreeFile(pfile);
				return false;
			}

			// Get ptr to schema
			subwaybuttonschema_t& schema = buttonschemas[type];

			while(true)
			{
				// Read the next token
				pstr = Common::Parse(pstr, token);
				if(!pstr)
					break;
				// End of definition
				if(!qstrcmp(token, "}"))
					break;

				if(!qstrcmp(token, "$buttontext"))
				{
					// Read in the button text
					pstr = Common::Parse(pstr, token);
					if(!pstr)
					{
						cl_engfuncs.pfnCon_EPrintf("%s - Unexpected EOF while reading $button definition in '%s'.\n", __FUNCTION__, pstrScriptFile);
						cl_filefuncs.pfnFreeFile(pfile);
						return false;
					}

					schema.buttontext = token;
				}
				else if(!qstrcmp(token, "$destinationid"))
				{
					// Read in the button text
					pstr = Common::Parse(pstr, token);
					if(!pstr)
					{
						cl_engfuncs.pfnCon_EPrintf("%s - Unexpected EOF while reading $button definition in '%s'.\n", __FUNCTION__, pstrScriptFile);
						cl_filefuncs.pfnFreeFile(pfile);
						return false;
					}

					schema.destinationid = token;
				}
				else if(!qstrcmp(token, "$description"))
				{
					// Next token should be an openig bracket
					pstr = Common::Parse(pstr, token);
					if(!pstr)
					{
						cl_engfuncs.pfnCon_EPrintf("%s - Unexpected EOF while reading $button definition in '%s'.\n", __FUNCTION__, pstrScriptFile);
						cl_filefuncs.pfnFreeFile(pfile);
						return false;
					}

					if(qstrcmp(token, "{"))
					{
						cl_engfuncs.pfnCon_EPrintf("%s - Expected '{', got %s instead in definition in '%s'.\n", __FUNCTION__, token.c_str(), pstrScriptFile);
						cl_filefuncs.pfnFreeFile(pfile);
						return false;
					}

					// Skip whitespaces to first line
					while(*pstr && SDL_isspace(*pstr))
						pstr++;
					
					// Find the ending bracket
					const Char* pstrend = qstrstr(pstr, "}");
					if(!pstrend)
					{
						cl_engfuncs.pfnCon_EPrintf("%s - Unexpected EOF while reading $button definition in '%s'.\n", __FUNCTION__, pstrScriptFile);
						cl_filefuncs.pfnFreeFile(pfile);
						return false;
					}

					// Assign contents
					Uint32 length = pstrend - pstr;
					schema.description.assign(pstr, length);

					// Erase any tabulations
					while(true)
					{
						Uint32 tabpos = schema.description.find(0, "\t");
						if(tabpos == CString::CSTRING_NO_POSITION)
							break;

						schema.description.erase(tabpos, 1);
					}

					// Set pointer
					pstr = pstrend+1;
				}
				else
				{
					cl_engfuncs.pfnCon_EPrintf("%s - Unknown token '$s' in '%s'.\n", __FUNCTION__, token.c_str(), pstrScriptFile);
					cl_filefuncs.pfnFreeFile(pfile);
					return false;
				}
			}
		}
		else if(!qstrcmp(token, "$defaultdescription"))
		{
			// Next token should be an openig bracket
			pstr = Common::Parse(pstr, token);
			if(!pstr)
			{
				cl_engfuncs.pfnCon_EPrintf("%s - Unexpected EOF while reading $button definition in '%s'.\n", __FUNCTION__, pstrScriptFile);
				cl_filefuncs.pfnFreeFile(pfile);
				return false;
			}

			if(qstrcmp(token, "{"))
			{
				cl_engfuncs.pfnCon_EPrintf("%s - Expected '{', got %s instead in definition in '%s'.\n", __FUNCTION__, token.c_str(), pstrScriptFile);
				cl_filefuncs.pfnFreeFile(pfile);
				return false;
			}

			// Skip whitespaces to first line
			while(*pstr && SDL_isspace(*pstr))
				pstr++;
					
			// Find the ending bracket
			const Char* pstrend = qstrstr(pstr, "}");
			if(!pstrend)
			{
				cl_engfuncs.pfnCon_EPrintf("%s - Unexpected EOF while reading $button definition in '%s'.\n", __FUNCTION__, pstrScriptFile);
				cl_filefuncs.pfnFreeFile(pfile);
				return false;
			}

			// Assign contents
			Uint32 length = pstrend - pstr;
			CString defaultdescription;
			defaultdescription.assign(pstr, length);

			// Erase any tabulations
			while(true)
			{
				Uint32 tabpos = defaultdescription.find(0, "\t");
				if(tabpos == CString::CSTRING_NO_POSITION)
					break;

				defaultdescription.erase(tabpos, 1);
			}

			m_pDefaultDescription->setText(defaultdescription.c_str());
			
			// Set pointer
			pstr = pstrend+1;
		}
		else
		{
			cl_engfuncs.pfnCon_EPrintf("%s - Unknown token '$s' in '%s'.\n", __FUNCTION__, token.c_str(), pstrScriptFile);
			cl_filefuncs.pfnFreeFile(pfile);
			return false;
		}
	}
	cl_filefuncs.pfnFreeFile(pfile);

	// Assign the button descriptions
	for(Uint32 i = 0; i < NB_DESTINATION_BUTTONS; i++)
	{
		Int32 j = 0;

		subwaybuttonschema_t* pschema = nullptr;
		if(flags & FL_SUBWAY_DISABLED)
		{
			// Disabled by admin
			pschema = &buttonschemas[SUBWAYWINDOW_BUTTON_DISABLED];
			// set index
			j = i;
		}
		else
		{
			bool isAvailable = false;

			switch(subwayLineIndex)
			{
			case SUBWAYLINE_BERGEN_ECKHART:
				{
					isAvailable = (flags & (1<<i)) ? true : false;

					// Stupid old hack
					if(i == 2)
						j = 3;
					else if(i == 3)
						j = 2;
					else
						j = i;
				}
				break;
			case SUBWAYLINE_KASSAR_STILLWELL:
				{
					// Handle this specially
					if(i == 0)
					{
						if(flags & FL_SUBWAY_GOT_KASSARST)
							isAvailable = true;
						else
							isAvailable = false;
					}
					else if(i == 2)
					{
						if(flags & FL_SUBWAY_GOT_MARSHALLST)
							isAvailable = true;
						else
							isAvailable = false;
					}
					else
					{
						// Not available
						isAvailable = false;
					}

					// Set index
					j = i;
				}
				break;
			case SUBWAYLINE_MARSHALL_LYNE:
				{
					// Handle this specially
					if(i == 0)
					{
						if(flags & FL_SUBWAY_GOT_MARSHALLST)
							isAvailable = true;
						else
							isAvailable = false;
					}
					else if(i == 2)
					{
						if(flags & FL_SUBWAY_GOT_AIELLOST)
							isAvailable = true;
						else
							isAvailable = false;
					}
					else
					{
						// Not available
						isAvailable = false;
					}

					// Set index
					j = i;
				}
				break;
			}

			if(!isAvailable)
				pschema = &buttonschemas[SUBWAYWINDOW_BUTTON_UNAVAILABLE];
			else
				pschema = &buttonschemas[SUBWAYWINDOW_BUTTON_DESTINATION_1+j];
		}

		m_buttonsArray[j].pButton->setText(pschema->buttontext.c_str());
		m_buttonsArray[j].pDescription->setText(pschema->description.c_str());
		m_buttonsArray[j].destinationid = pschema->destinationid;
	}

	// Set this information
	m_scriptFilePath = pstrScriptFile;
	m_subwayFlags = flags;

	return true;
}

//====================================
//
//====================================
void CGameUISubwayWindow::getInformation( CString& scriptfile, Int32& flags, Int32& subwayLineIndex ) const
{
	scriptfile = m_scriptFilePath;
	flags = m_subwayFlags;
	subwayLineIndex = m_subwayLineIndex;
}

//====================================
//
//====================================
void CGameUISubwayWindow::ManageEvent( subwaybuttonevent_t event )
{
	switch(event)
	{
		case SUBWAY_BUTTON_1:
		case SUBWAY_BUTTON_2:
		case SUBWAY_BUTTON_3:
		case SUBWAY_BUTTON_4:
			{
				subwaybutton_t& button = m_buttonsArray[event];
				if(button.destinationid.empty())
				{
					cl_engfuncs.pfnPlayAmbientSound(0, ZERO_VECTOR, SND_CHAN_AUTO, GAMEUI_FAIL_SOUND, VOL_NORM, ATTN_NORM, PITCH_NORM, SND_FL_2D, 0);
					return;
				}

				// Send message to server
				Uint32 msgid = g_pGUIManager->GetServerUIMessageId();
				if(!msgid)
				{
					cl_engfuncs.pfnCon_Printf("%s - Message 'GameUIMessage' not registered on client.\n", __FUNCTION__);
					return;
				}

				// Just tell them which window sent it
				cl_engfuncs.pfnClientUserMessageBegin(msgid);
					cl_engfuncs.pfnMsgWriteByte(GAMEUIEVENT_SUBWAY_SELECTION);
					cl_engfuncs.pfnMsgWriteString(button.destinationid.c_str());
				cl_engfuncs.pfnClientUserMessageEnd();

				cl_engfuncs.pfnPlayAmbientSound(0, ZERO_VECTOR, SND_CHAN_AUTO, GAMEUI_OK_SOUND, VOL_NORM, ATTN_NORM, PITCH_NORM, SND_FL_2D, 0);
				m_windowFlags |= CGameUIWindow::FL_WINDOW_KILLME;
			}
			break;
		case SUBWAY_BUTTON_CANCEL:
			m_windowFlags |= CGameUIWindow::FL_WINDOW_KILLME;
			break;
	}
}

//====================================
//
//====================================
CGameUISubwayWindow* CGameUISubwayWindow::CreateInstance( void )
{
	// Load the schema file
	ui_windowdescription_t* pWinDesc = g_pGUIManager->LoadWindowDescriptionFile(SUBWAYWINDOW_OBJ_NAME, SUBWAYWINDOW_DESC_FILE);
 	if(!pWinDesc)
	{
		cl_engfuncs.pfnCon_EPrintf("Failed to load window description '%s' for '%s'.\n", SUBWAYWINDOW_DESC_FILE, SUBWAYWINDOW_OBJ_NAME);
		return nullptr;
	}

	const ui_objectinfo_t* pWindowObject = pWinDesc->getObject(UI_OBJECT_WINDOW, SUBWAYWINDOW_OBJ_NAME);
	if(!pWindowObject)
	{
		cl_engfuncs.pfnCon_EPrintf("Window description file '%s' has no definition for '%s'.\n", SUBWAYWINDOW_DESC_FILE, SUBWAYWINDOW_OBJ_NAME);
		return nullptr;
	}

	Uint32 screenWidth, screenHeight;
	cl_renderfuncs.pfnGetScreenSize(screenWidth, screenHeight);

	Uint32 relativeWidth = gHUDDraw.ScaleY(pWindowObject->getWidth());
	Uint32 relativeHeight = gHUDDraw.ScaleY(pWindowObject->getHeight());

	Int32 xPosition = (screenWidth / 2) - (relativeWidth / 2);
	Int32 yPosition = (screenHeight / 2) - (relativeHeight / 2);

	CGameUISubwayWindow* pNewWindow = new CGameUISubwayWindow(CGameUIWindow::FL_WINDOW_NONE, xPosition, yPosition, relativeWidth, relativeHeight);
	if(!pNewWindow->init(pWinDesc, pWindowObject))
	{
		cl_engfuncs.pfnCon_EPrintf("%s - Failed to initialize 'CGameUISubwayWindow'.\n", __FUNCTION__);
		delete pNewWindow;
		return nullptr;
	}

	return pNewWindow;
}

//====================================
//
//====================================
void CGameUISubwayWindowCallbackEvent::PerformAction( Float param )
{
	if(!m_pSubwayWindow)
		return;

	m_pSubwayWindow->ManageEvent(m_eventType);
}
