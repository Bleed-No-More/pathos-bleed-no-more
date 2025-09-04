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
#include "hud.h"

#include "gameuiobjectiveswindow.h"
#include "gameuiwindows_shared.h"
#include "snd_shared.h"
#include "gameui_shared.h"

// Title text object name
const Char CGameUIObjectivesWindow::OBJECTIVESWINDOW_OBJ_NAME[] = "GameUIObjectivesWindow";
// Title text object name
const Char CGameUIObjectivesWindow::OBJECTIVESWINDOW_TITLE_TEXT_OBJ_NAME[] = "GameUIObjectivesWindowTitleText";
// Button tab object name
const Char CGameUIObjectivesWindow::OBJECTIVESWINDOW_BUTTON_TAB_OBJ_NAME[] = "GameUIObjectivesWindowButtonTab";
// Info tab object name
const Char CGameUIObjectivesWindow::OBJECTIVESWINDOW_INFO_TAB_OBJ_NAME[] = "GameUIObjectivesWindowInfoTab";
// Upper separator object name
const Char CGameUIObjectivesWindow::OBJECTIVESWINDOW_UPPER_SEPARATOR_OBJ_NAME[] = "GameUIObjectivesWindowUpperSeparator";
// Lower separator object name
const Char CGameUIObjectivesWindow::OBJECTIVESWINDOW_LOWER_SEPARATOR_OBJ_NAME[] = "GameUIObjectivesWindowLowerSeparator";
// Objective button object name
const Char CGameUIObjectivesWindow::OBJECTIVESWINDOW_OBJECTIVE_BUTTON_OBJ_NAME[] = "GameUIObjectivesWindowObjectiveButton";
// Objective text tab object name
const Char CGameUIObjectivesWindow::OBJECTIVESWINDOW_OBJECTIVE_TEXT_TAB_OBJ_NAME[] = "GameUIObjectivesWindowObjectiveTextTab";
// Default description default description text object name
const Char CGameUIObjectivesWindow::OBJECTIVESWINDOW_DEFAULT_DESCRIPTION_TEXT_TAB_OBJ_NAME[] = "GameUIObjectivesWindowDefaultDescriptionText";
// Exit button object name
const Char CGameUIObjectivesWindow::OBJECTIVESWINDOW_EXIT_BUTTON_OBJ_NAME[] = "GameUIObjectivesWindowExitButton";
// Window description file
const Char CGameUIObjectivesWindow::OBJECTIVESWINDOW_DESC_FILE[] = "gameuiobjectiveswindow.txt";
// Script subfolder name
const Char CGameUIObjectivesWindow::OBJECTIVESWINDOW_SCRIPT_SUBFOLDER_NAME[] = "objectives";
// Base script name
const Char CGameUIObjectivesWindow::OBJECTIVESWINDOW_SCRIPT_NAME[] = "defaults.txt";

//====================================
//
//====================================
CGameUIObjectivesWindow::CGameUIObjectivesWindow( Int32 flags, Int32 originX, Int32 originY, Uint32 width, Uint32 height ):
	CGameUIWindow(flags, originX, originY, width, height),
	m_pWindowTitleText(nullptr),
	m_pDefaultDescription(nullptr),
	m_pCurrentObjective(nullptr),
	m_newObjectivesBitflags(0),
	m_pButtonsTab(nullptr),
	m_pInfoTab(nullptr),
	m_pFontSet(nullptr)
{
}

//====================================
//
//====================================
CGameUIObjectivesWindow::~CGameUIObjectivesWindow( void )
{
}

//====================================
//
//====================================
bool CGameUIObjectivesWindow::init( const ui_windowdescription_t* pWindowDesc, const ui_objectinfo_t* pWindowObject )
{
	// Initialize schema for surface object
	if(!CGameUISurface::initSchema(pWindowObject->getSchema().c_str()))
		return false;

	//
	// Create the title text object
	//
	const ui_objectinfo_t* pTitleTextObject = pWindowDesc->getObject(UI_OBJECT_TEXT, OBJECTIVESWINDOW_TITLE_TEXT_OBJ_NAME);
	if(!pTitleTextObject)
	{
		cl_engfuncs.pfnCon_EPrintf("Window description file '%s' has no definition for '%s'.\n", OBJECTIVESWINDOW_DESC_FILE, OBJECTIVESWINDOW_TITLE_TEXT_OBJ_NAME);
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
	// Create upper separator
	//
	const ui_objectinfo_t* pUpperSeparatorObject = pWindowDesc->getObject(UI_OBJECT_SEPARATOR_H, OBJECTIVESWINDOW_UPPER_SEPARATOR_OBJ_NAME);
	if(!pUpperSeparatorObject)
	{
		cl_engfuncs.pfnCon_EPrintf("Window description file '%s' has no definition for '%s'.\n", OBJECTIVESWINDOW_DESC_FILE, OBJECTIVESWINDOW_UPPER_SEPARATOR_OBJ_NAME);
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
		cl_engfuncs.pfnCon_EPrintf("Failed to initialize 'CGameUIHorizontalSeparator' object named '%s'.\n", OBJECTIVESWINDOW_UPPER_SEPARATOR_OBJ_NAME);
		return false;
	}

	//
	// Create the tab object
	//
	const ui_objectinfo_t* pButtonTabObject = pWindowDesc->getObject(UI_OBJECT_TAB, OBJECTIVESWINDOW_BUTTON_TAB_OBJ_NAME);
	if(!pButtonTabObject)
	{
		cl_engfuncs.pfnCon_EPrintf("Window description file '%s' has no definition for '%s'.\n", OBJECTIVESWINDOW_DESC_FILE, OBJECTIVESWINDOW_BUTTON_TAB_OBJ_NAME);
		return false;
	}

	m_pButtonsTab = new CGameUIScrollableSurface(pButtonTabObject->getFlags(), 
		pButtonTabObject->getFont(),
		pWindowObject->getXInset() + pButtonTabObject->getXOrigin(),
		pWindowObject->getYInset() + pButtonTabObject->getYOrigin(),
		pButtonTabObject->getWidth(),
		pButtonTabObject->getHeight(),
		pButtonTabObject->getListPostSpacing());
	m_pButtonsTab->setParent(this);

	if(!m_pButtonsTab->initSchema(pButtonTabObject->getSchema().c_str(), pButtonTabObject->getButtonSchema().c_str()))
	{
		cl_engfuncs.pfnCon_EPrintf("Failed to initialize 'CGameUITextTab'.\n");
		return false;
	}

	//
	// Create the info tab object
	//
	const ui_objectinfo_t* pInfoTabObject = pWindowDesc->getObject(UI_OBJECT_TAB, OBJECTIVESWINDOW_INFO_TAB_OBJ_NAME);
	if(!pInfoTabObject)
	{
		cl_engfuncs.pfnCon_EPrintf("Window description file '%s' has no definition for '%s'.\n", OBJECTIVESWINDOW_DESC_FILE, OBJECTIVESWINDOW_INFO_TAB_OBJ_NAME);
		return false;
	}

	m_pInfoTab = new CGameUISurface(pInfoTabObject->getFlags(),
		pWindowObject->getXInset() + pInfoTabObject->getXOrigin(),
		pWindowObject->getYInset() + pInfoTabObject->getYOrigin(),
		pInfoTabObject->getWidth(),
		pInfoTabObject->getHeight());
	m_pInfoTab->setParent(this);

	if(!m_pInfoTab->initSchema(pInfoTabObject->getSchema().c_str()))
	{
		cl_engfuncs.pfnCon_EPrintf("Failed to initialize 'CGameUITextTab'.\n");
		return false;
	}

	//
	// Create the title text object
	//
	const ui_objectinfo_t* pDefaultDescriptionObject = pWindowDesc->getObject(UI_OBJECT_TEXT, OBJECTIVESWINDOW_DEFAULT_DESCRIPTION_TEXT_TAB_OBJ_NAME);
	if(!pDefaultDescriptionObject)
	{
		cl_engfuncs.pfnCon_EPrintf("Window description file '%s' has no definition for '%s'.\n", OBJECTIVESWINDOW_DESC_FILE, OBJECTIVESWINDOW_DEFAULT_DESCRIPTION_TEXT_TAB_OBJ_NAME);
		return false;
	}

	const font_set_t* pDescriptionFont = g_pGUIManager->GetSchemaResolutionFont(pDefaultDescriptionObject->getTextSchemaName().c_str());
	if(!pDescriptionFont)
		pDescriptionFont = gGameUIManager.GetDefaultFontSet();

	m_pDefaultDescription = new CGameUIText(
		pDefaultDescriptionObject->getFlags(), 
		pDefaultDescriptionObject->getTextColor(), 
		pDescriptionFont, 
		pWindowObject->getXInset() + pDefaultDescriptionObject->getXOrigin(), 
		pWindowObject->getYInset() + pDefaultDescriptionObject->getYOrigin());

	m_pDefaultDescription->setParent(this);
	m_pDefaultDescription->setText(pDefaultDescriptionObject->getText().c_str());

	//
	// Create lower separator
	//
	const ui_objectinfo_t* pLowerSeparatorObject = pWindowDesc->getObject(UI_OBJECT_SEPARATOR_H, OBJECTIVESWINDOW_LOWER_SEPARATOR_OBJ_NAME);
	if(!pLowerSeparatorObject)
	{
		cl_engfuncs.pfnCon_EPrintf("Window description file '%s' has no definition for '%s'.\n", OBJECTIVESWINDOW_DESC_FILE, OBJECTIVESWINDOW_LOWER_SEPARATOR_OBJ_NAME);
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
		cl_engfuncs.pfnCon_EPrintf("Failed to initialize 'CGameUIHorizontalSeparator' object named '%s'.\n", OBJECTIVESWINDOW_LOWER_SEPARATOR_OBJ_NAME);
		return false;
	}

	//
	// Create the "Exit" button
	//
	const ui_objectinfo_t* pExitButtonObject = pWindowDesc->getObject(UI_OBJECT_BUTTON, OBJECTIVESWINDOW_EXIT_BUTTON_OBJ_NAME);
	if(!pExitButtonObject)
	{
		cl_engfuncs.pfnCon_EPrintf("Window description file '%s' has no definition for '%s'.\n", OBJECTIVESWINDOW_DESC_FILE, OBJECTIVESWINDOW_EXIT_BUTTON_OBJ_NAME);
		return false;
	}

	CGameUIObjectivesWindowCallbackEvent* pExitEvent = new CGameUIObjectivesWindowCallbackEvent(this, OBJ_BUTTON_EXIT);
	CGameUIButton* pExitButton = new CGameUIButton(pExitButtonObject->getFlags(), 
		pExitEvent, 
		SDLK_RETURN,
		pWindowObject->getXInset() + pExitButtonObject->getXOrigin(),
		pWindowObject->getYInset() + pExitButtonObject->getYOrigin(),
		pExitButtonObject->getWidth(),
		pExitButtonObject->getHeight());

	if(!pExitButton->initSchema(pExitButtonObject->getSchema().c_str()))
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
void CGameUIObjectivesWindow::think( void )
{
	if(!m_pCurrentObjective)
	{
		bool hasVisible = false;

		for (Uint32 i = 0; i < m_buttonsArray.size(); i++)
		{
			if (!m_buttonsArray[i].pButton->isVisible())
				continue;

			if (m_pButtonsTab->isMouseOverChild(m_buttonsArray[i].pButton))
			{
				if (!m_buttonsArray[i].pDescription->isVisible())
					m_buttonsArray[i].pDescription->setVisible(true);

				hasVisible = true;
			}
			else
			{
				if (m_buttonsArray[i].pDescription->isVisible())
					m_buttonsArray[i].pDescription->setVisible(false);
			}
		}

		if (!hasVisible && !m_pDefaultDescription->isVisible())
			m_pDefaultDescription->setVisible(true);
		else if (hasVisible && m_pDefaultDescription->isVisible())
			m_pDefaultDescription->setVisible(false);

	}

	// Call base class to manage think functions
	CGameUIWindow::think();
}

//====================================
//
//====================================
bool CGameUIObjectivesWindow::initData( const CArray<CString>& objectivesArray, const Char* pstrActiveObjectiveName, Int32& newObjectivesBitflags )
{
	// Get schema for the buttons and the documents tab
	ui_windowdescription_t* pWindowDesc = g_pGUIManager->LoadWindowDescriptionFile(OBJECTIVESWINDOW_OBJ_NAME, OBJECTIVESWINDOW_DESC_FILE);
 	if(!pWindowDesc)
	{
		cl_engfuncs.pfnCon_EPrintf("Failed to load window description '%s' for '%s'.\n", OBJECTIVESWINDOW_DESC_FILE, OBJECTIVESWINDOW_OBJ_NAME);
		return nullptr;
	}

	const ui_objectinfo_t* pInfoTabObject = pWindowDesc->getObject(UI_OBJECT_TAB, OBJECTIVESWINDOW_INFO_TAB_OBJ_NAME);
	if(!pInfoTabObject)
	{
		cl_engfuncs.pfnCon_EPrintf("Window description file '%s' has no definition for '%s'.\n", OBJECTIVESWINDOW_DESC_FILE, OBJECTIVESWINDOW_INFO_TAB_OBJ_NAME);
		return false;
	}

	const ui_objectinfo_t* pButtonTabObject = pWindowDesc->getObject(UI_OBJECT_TAB, OBJECTIVESWINDOW_BUTTON_TAB_OBJ_NAME);
	if(!pButtonTabObject)
	{
		cl_engfuncs.pfnCon_EPrintf("Window description file '%s' has no definition for '%s'.\n", OBJECTIVESWINDOW_DESC_FILE, OBJECTIVESWINDOW_BUTTON_TAB_OBJ_NAME);
		return false;
	}

	//
	// Get the objective button object definition
	//
	const ui_objectinfo_t* pObjectiveButtonObject = pWindowDesc->getObject(UI_OBJECT_BUTTON, OBJECTIVESWINDOW_OBJECTIVE_BUTTON_OBJ_NAME);
	if(!pObjectiveButtonObject)
	{
		cl_engfuncs.pfnCon_EPrintf("Window description file '%s' has no definition for '%s'.\n", OBJECTIVESWINDOW_DESC_FILE, OBJECTIVESWINDOW_OBJECTIVE_BUTTON_OBJ_NAME);
		return false;
	}

	//
	// Get the objective button object definition
	//
	const ui_objectinfo_t* pObjectiveTextTabObject = pWindowDesc->getObject(UI_OBJECT_TEXT_TAB, OBJECTIVESWINDOW_OBJECTIVE_TEXT_TAB_OBJ_NAME);
	if(!pObjectiveTextTabObject)
	{
		cl_engfuncs.pfnCon_EPrintf("Window description file '%s' has no definition for '%s'.\n", OBJECTIVESWINDOW_DESC_FILE, OBJECTIVESWINDOW_OBJECTIVE_TEXT_TAB_OBJ_NAME);
		return false;
	}

	// Load the base script
	CString scriptFilePath;
	scriptFilePath << GAMEUI_SCRIPT_BASE_PATH << PATH_SLASH_CHAR << OBJECTIVESWINDOW_SCRIPT_SUBFOLDER_NAME << PATH_SLASH_CHAR << OBJECTIVESWINDOW_SCRIPT_NAME;

	const byte* pfile = cl_filefuncs.pfnLoadFile(scriptFilePath.c_str(), nullptr);
	if(!pfile)
	{
		cl_engfuncs.pfnCon_EPrintf("%s - Failed to load '%s'.\n", __FUNCTION__, scriptFilePath.c_str());
		return false;
	}

	CString windowTitle;
	CString defaultDescription;

	CString token;
	const Char* pstr = reinterpret_cast<const Char*>(pfile);
	while(pstr)
	{
		// Read the token in
		pstr = Common::Parse(pstr, token);
		if(!pstr || token.empty())
			break;

		if(!qstrcmp(token, "$title"))
		{
			// Read in the token
			pstr = Common::Parse(pstr, token);
			windowTitle = token;
			continue;
		}
		else if(!qstrcmp(token, "$defaultdescription"))
		{
			// Next token should be an openig bracket
			pstr = Common::Parse(pstr, token);
			if(!pstr)
			{
				cl_engfuncs.pfnCon_EPrintf("%s - Unexpected EOF while reading $button definition in '%s'.\n", __FUNCTION__, scriptFilePath.c_str());
				cl_filefuncs.pfnFreeFile(pfile);
				return false;
			}

			if(qstrcmp(token, "{"))
			{
				cl_engfuncs.pfnCon_EPrintf("%s - Expected '{', got %s instead in definition in '%s'.\n", __FUNCTION__, token.c_str(), scriptFilePath.c_str());
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
				cl_engfuncs.pfnCon_EPrintf("%s - Unexpected EOF while reading $button definition in '%s'.\n", __FUNCTION__, scriptFilePath.c_str());
				cl_filefuncs.pfnFreeFile(pfile);
				return false;
			}

			// Assign contents
			Uint32 length = pstrend - pstr;
			defaultDescription.assign(pstr, length);

			// Erase any tabulations
			while(true)
			{
				Uint32 tabpos = defaultDescription.find(0, "\t");
				if(tabpos == CString::CSTRING_NO_POSITION)
					break;

				defaultDescription.erase(tabpos, 1);
			}
			
			// Set pointer
			pstr = pstrend+1;
		}
		else
		{
			cl_engfuncs.pfnCon_EPrintf("%s - Unknown token '$s' in '%s'.\n", __FUNCTION__, token.c_str(), scriptFilePath.c_str());
			cl_filefuncs.pfnFreeFile(pfile);
			return false;
		}
	}
	cl_filefuncs.pfnFreeFile(pfile);

	// Check for errors
	if(windowTitle.empty())
	{
		cl_engfuncs.pfnCon_EPrintf("%s - '$title' was not specified in '%s'.\n", __FUNCTION__, scriptFilePath.c_str());
		windowTitle = "NULL";
	}

	// Check for errors
	if(defaultDescription.empty())
	{
		cl_engfuncs.pfnCon_EPrintf("%s - '$defaultdescription' was not specified in '%s'.\n", __FUNCTION__, scriptFilePath.c_str());
		defaultDescription = "NULL";
	}

	m_pWindowTitleText->setText(windowTitle.c_str());
	m_pDefaultDescription->setText(defaultDescription.c_str());
	m_newObjectivesBitflags = newObjectivesBitflags;

	Int32 buttonYPos = pButtonTabObject->getListPostSpacing();

	// Track separately to avoid issues if a file is missing/bad
	Uint32 nbAdded = 0;
	for(Uint32 i = 0; i < objectivesArray.size(); i++)
	{
		scriptFilePath.clear();;
		scriptFilePath << GAMEUI_SCRIPT_BASE_PATH << PATH_SLASH_CHAR << "objectives" << PATH_SLASH_CHAR << objectivesArray[i] << ".txt";

		const byte* pobjfile = cl_filefuncs.pfnLoadFile(scriptFilePath.c_str(), nullptr);
		if(!pobjfile)
		{
			cl_engfuncs.pfnCon_EPrintf("%s - Failed to load '%s'.\n", __FUNCTION__, scriptFilePath.c_str());
			continue;
		}

		pstr = reinterpret_cast<const Char*>(pobjfile);

		// Read the token in
		pstr = Common::Parse(pstr, token);

		// First token should be "$title"
		if(qstrcmp(token, "$title"))
		{
			cl_engfuncs.pfnCon_EPrintf("%s - Unknown token '$s' in '%s', expected '$title'.\n", __FUNCTION__, token.c_str(), scriptFilePath.c_str());
			cl_filefuncs.pfnFreeFile(pobjfile);
			continue;
		}

		// Read in the objective's title
		pstr = Common::Parse(pstr, token);
		if(!pstr)
		{
			cl_engfuncs.pfnCon_EPrintf("%s - Unexpected EOF while reading objective definition in '%s'.\n", __FUNCTION__, scriptFilePath.c_str());
			cl_filefuncs.pfnFreeFile(pobjfile);
			continue;
		}

		// Save description
		CString objectiveTitle = token;

		// Next token should be an openig bracket
		pstr = Common::Parse(pstr, token);
		if(!pstr)
		{
			cl_engfuncs.pfnCon_EPrintf("%s - Unexpected EOF while reading $button definition in '%s'.\n", __FUNCTION__, scriptFilePath.c_str());
			cl_filefuncs.pfnFreeFile(pobjfile);
			continue;
		}

		if(qstrcmp(token, "{"))
		{
			cl_engfuncs.pfnCon_EPrintf("%s - Expected '{', got %s instead in definition in '%s'.\n", __FUNCTION__, token.c_str(), scriptFilePath.c_str());
			cl_filefuncs.pfnFreeFile(pobjfile);
			continue;
		}

		// Skip whitespaces to first line
		while(*pstr && SDL_isspace(*pstr))
			pstr++;
					
		// Find the ending bracket
		const Char* pstrend = qstrstr(pstr, "}");
		if(!pstrend)
		{
			cl_engfuncs.pfnCon_EPrintf("%s - Unexpected EOF while reading $button definition in '%s'.\n", __FUNCTION__, scriptFilePath.c_str());
			cl_filefuncs.pfnFreeFile(pobjfile);
			continue;
		}

		// Assign contents
		CString objectiveDescription;
		Uint32 length = pstrend - pstr;
		objectiveDescription.assign(pstr, length);

		// Erase any tabulations
		while(true)
		{
			Uint32 tabpos = objectiveDescription.find(0, "\t");
			if(tabpos == CString::CSTRING_NO_POSITION)
				break;

			objectiveDescription.erase(tabpos, 1);
		}

		// Assign button contents and such
		Uint32 addIndex = nbAdded;
		m_buttonsArray.resize(nbAdded + 1);
		nbAdded++;

		button_t& newButton = m_buttonsArray[addIndex];

		CGameUIObjectivesWindowCallbackEvent* pEvent = new CGameUIObjectivesWindowCallbackEvent(this, OBJ_BUTTON_0 + addIndex);
		newButton.pButton = new CGameUIButton(pObjectiveButtonObject->getFlags(),
			pEvent,
			pButtonTabObject->getXInset() + pObjectiveButtonObject->getXOrigin(),
			pButtonTabObject->getYInset() + pObjectiveButtonObject->getYOrigin() + buttonYPos,
			pObjectiveButtonObject->getWidth(),
			pObjectiveButtonObject->getHeight());
		newButton.pButton->setParent(m_pButtonsTab);
		newButton.pButton->setVisible(false);

		if(!newButton.pButton->initSchema(pObjectiveButtonObject->getSchema().c_str()))
		{
			cl_engfuncs.pfnCon_EPrintf("Failed to initialize 'CGameUIHorizontalSeparator' object named '%s'.\n", OBJECTIVESWINDOW_UPPER_SEPARATOR_OBJ_NAME);
			return false;
		}

		newButton.pDescription = new CGameUITextTab(pObjectiveTextTabObject->getFlags(),
			pObjectiveTextTabObject->getFont(),
			pObjectiveTextTabObject->getTextInset(),
			pObjectiveTextTabObject->getTextColor(),
			pInfoTabObject->getXInset() + pObjectiveTextTabObject->getXOrigin(),
			pInfoTabObject->getYInset() + pObjectiveTextTabObject->getYOrigin(),
			pObjectiveTextTabObject->getWidth(),
			pObjectiveTextTabObject->getHeight());

		newButton.pDescription->setParent(m_pInfoTab);
		newButton.pDescription->setVisible(false);

		if(!newButton.pDescription->initSchema(pObjectiveTextTabObject->getSchema().c_str(), pObjectiveTextTabObject->getButtonSchema().c_str()))
		{
			cl_engfuncs.pfnCon_EPrintf("Failed to initialize 'CGameUIHorizontalSeparator' object named '%s'.\n", OBJECTIVESWINDOW_UPPER_SEPARATOR_OBJ_NAME);
			return false;
		}

		// Set button properties
		buttonYPos += pObjectiveButtonObject->getHeight() + pButtonTabObject->getListPostSpacing();

		newButton.objectiveName = objectivesArray[i];
		newButton.pButton->setText(objectiveTitle.c_str());
		newButton.pDescription->initData((const byte*)objectiveDescription.c_str(), objectiveDescription.length());
		newButton.pButton->setVisible(true);

		// Restore currently selected objective
		if(pstrActiveObjectiveName && !qstrcmp(objectivesArray[i], pstrActiveObjectiveName))
			m_pCurrentObjective = &newButton;
		else if(m_newObjectivesBitflags & (1<<i))
			newButton.pButton->setDrawFlags(GUIEL_DF_MARK_NEW);

		// Set pointer
		pstr = pstrend+1;

		// There should be nothing else here
		pstr = Common::Parse(pstr, token);
		if(pstr)
		{
			pstr = Common::Parse(pstr, token);
			cl_engfuncs.pfnCon_EPrintf("%s - Unexpected %s after end of definition in '%s'.\n", __FUNCTION__, token.c_str(), scriptFilePath.c_str());
		}

		// Close file
		cl_filefuncs.pfnFreeFile(pobjfile);
	}

	// Clear "new objective" flag
	gHUD.SetNewObjective(false);

	return true;
}

//====================================
//
//====================================
void CGameUIObjectivesWindow::getInformation( CArray<CString>& objectivesArray, CString& activeObjectiveName, Int32& newObjectivesBitflags ) const
{
	if(m_pCurrentObjective)
		activeObjectiveName = m_pCurrentObjective->objectiveName;

	for(Uint32 i = 0; i < m_buttonsArray.size(); i++)
		objectivesArray.push_back(m_buttonsArray[i].objectiveName);

	newObjectivesBitflags = m_newObjectivesBitflags;
}

//====================================
//
//====================================
void CGameUIObjectivesWindow::ManageEvent( objectivesbuttonevent_t event )
{
	if (event >= OBJ_BUTTON_0)
	{
		Int32 buttonIndex = event - OBJ_BUTTON_0;
		button_t& button = m_buttonsArray[buttonIndex];
		if (!button.pButton->isVisible())
			return;

		if (m_pCurrentObjective)
		{
			m_pCurrentObjective->pButton->removeDrawFlags(GUIEL_DF_MARK_HIGHLIGHT);
			m_pCurrentObjective->pDescription->setVisible(false);
		}

		if (m_pCurrentObjective == &button)
		{
			m_pCurrentObjective = nullptr;
			m_pDefaultDescription->setVisible(true);
		}
		else
		{
			m_pCurrentObjective = &button;
			m_pCurrentObjective->pDescription->setVisible(true);
			m_pCurrentObjective->pButton->setDrawFlags(GUIEL_DF_MARK_HIGHLIGHT);

			if (m_newObjectivesBitflags & (1 << buttonIndex))
			{
				m_newObjectivesBitflags &= ~(1 << buttonIndex);
				m_pCurrentObjective->pButton->removeDrawFlags(GUIEL_DF_MARK_NEW);

				Uint32 msgid = g_pGUIManager->GetServerUIMessageId();
				if (!msgid)
				{
					cl_engfuncs.pfnCon_Printf("%s - Message 'GameUIMessage' not registered on client.\n", __FUNCTION__);
					return;
				}

				cl_engfuncs.pfnClientUserMessageBegin(msgid);
				cl_engfuncs.pfnMsgWriteByte(GAMEUIEVENT_READ_OBJECTIVE);
				cl_engfuncs.pfnMsgWriteString(m_pCurrentObjective->objectiveName.c_str());
				cl_engfuncs.pfnClientUserMessageEnd();
			}
		}
	}
	else if(event == OBJ_BUTTON_EXIT)
	{
		m_windowFlags |= CGameUIWindow::FL_WINDOW_KILLME;
	}
	else
	{
		cl_engfuncs.pfnCon_Printf("%s - Unknown button event %d.\n", __FUNCTION__, event);
	}
}

//====================================
//
//====================================
CGameUIObjectivesWindow* CGameUIObjectivesWindow::CreateInstance( void )
{
	// Load the schema file
	ui_windowdescription_t* pWinDesc = g_pGUIManager->LoadWindowDescriptionFile(OBJECTIVESWINDOW_OBJ_NAME, OBJECTIVESWINDOW_DESC_FILE);
 	if(!pWinDesc)
	{
		cl_engfuncs.pfnCon_EPrintf("Failed to load window description '%s' for '%s'.\n", OBJECTIVESWINDOW_DESC_FILE, OBJECTIVESWINDOW_OBJ_NAME);
		return nullptr;
	}

	const ui_objectinfo_t* pWindowObject = pWinDesc->getObject(UI_OBJECT_WINDOW, OBJECTIVESWINDOW_OBJ_NAME);
	if(!pWindowObject)
	{
		cl_engfuncs.pfnCon_EPrintf("Window description file '%s' has no definition for '%s'.\n", OBJECTIVESWINDOW_DESC_FILE, OBJECTIVESWINDOW_OBJ_NAME);
		return nullptr;
	}

	Uint32 screenWidth, screenHeight;
	cl_renderfuncs.pfnGetScreenSize(screenWidth, screenHeight);

	Uint32 relativeWidth = gHUDDraw.ScaleY(pWindowObject->getWidth());
	Uint32 relativeHeight = gHUDDraw.ScaleY(pWindowObject->getHeight());

	Int32 xPosition = (screenWidth / 2) - (relativeWidth / 2);
	Int32 yPosition = (screenHeight / 2) - (relativeHeight / 2);

	CGameUIObjectivesWindow* pNewWindow = new CGameUIObjectivesWindow(CGameUIWindow::FL_WINDOW_NONE, xPosition, yPosition, relativeWidth, relativeHeight);
	if(!pNewWindow->init(pWinDesc, pWindowObject))
	{
		cl_engfuncs.pfnCon_EPrintf("%s - Failed to initialize 'CGameUIObjectivesWindow'.\n", __FUNCTION__);
		delete pNewWindow;
		return nullptr;
	}

	return pNewWindow;
}

//====================================
//
//====================================
void CGameUIObjectivesWindowCallbackEvent::PerformAction( Float param )
{
	if(!m_pWindow)
		return;

	m_pWindow->ManageEvent(m_eventType);
}
