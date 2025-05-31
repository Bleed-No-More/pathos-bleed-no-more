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

#include "gameuidocumentswindow.h"
#include "gameuiwindows_shared.h"
#include "snd_shared.h"
#include "gameui_shared.h"
#include "gameuitextwindow.h"

// Window description file
const Char CGameUIDocumentsWindow::DOCUMENTSWINDOW_DESC_FILE[] = "gameuidocumentswindow.txt";
// Window object name
const Char CGameUIDocumentsWindow::DOCUMENTSWINDOW_OBJ_NAME[] = "GameUIDocumentsWindow";
// Title text object name
const Char CGameUIDocumentsWindow::DOCUMENTSWINDOW_TITLE_TEXT_OBJ_NAME[] = "GameUIDocumentsWindowTitleText";
// Upper separator object name
const Char CGameUIDocumentsWindow::DOCUMENTSWINDOW_UPPER_SEPARATOR_OBJ_NAME[] = "GameUIDocumentsWindowUpperSeparator";
// Lower separator object name
const Char CGameUIDocumentsWindow::DOCUMENTSWINDOW_LOWER_SEPARATOR_OBJ_NAME[] = "GameUIDocumentsWindowLowerSeparator";
// Document buttons tab object name
const Char CGameUIDocumentsWindow::DOCUMENTSWINDOW_BUTTON_TAB_OBJ_NAME[] = "GameUIDocumentsWindowButtonsTab";
// Exit button object name
const Char CGameUIDocumentsWindow::DOCUMENTSWINDOW_EXIT_BUTTON_OBJ_NAME[] = "GameUIDocumentsWindowExitButton";
// Reader title text object name
const Char CGameUIDocumentsWindow::DOCUMENTSWINDOW_READER_TITLE_TEXT_OBJ_NAME[] = "GameUIDocumentsWindowReaderTitleText";
// Reader text tab object name
const Char CGameUIDocumentsWindow::DOCUMENTSWINDOW_READER_TEXT_TAB_OBJ_NAME[] = "GameUIDocumentsWindowReaderTextTab";
// Reader back button object name
const Char CGameUIDocumentsWindow::DOCUMENTSWINDOW_READER_BACK_BTN_OBJ_NAME[] = "GameUIDocumentsWindowReaderBackButton";
// Document button object name
const Char CGameUIDocumentsWindow::DOCUMENTSWINDOW_DOCUMENT_BUTTON_OBJ_NAME[] = "GameUIDocumentsWindowDocumentButton";

//====================================
//
//====================================
CGameUIDocumentsWindow::CGameUIDocumentsWindow( Int32 flags, Int32 originX, Int32 originY, Uint32 width, Uint32 height ):
	CGameUIWindow(flags, originX, originY, width, height),
	m_pExitButton(nullptr),
	m_pCurrentDocument(nullptr),
	m_pDefaultTitleFont(nullptr),
	m_pDefaultTextFont(nullptr),
	m_pReaderTitleText(nullptr),
	m_pButtonsTab(nullptr),
	m_pReaderTextTab(nullptr),
	m_pFontSet(nullptr)
{
}

//====================================
//
//====================================
CGameUIDocumentsWindow::~CGameUIDocumentsWindow( void )
{
	if(!m_documentsArray.empty())
	{
		for(Uint32 i = 0; i < m_documentsArray.size(); i++)
			delete m_documentsArray[i];

		m_documentsArray.clear();
	}
}

//====================================
//
//====================================
bool CGameUIDocumentsWindow::init( const ui_windowdescription_t* pWindowDesc, const ui_objectinfo_t* pWindowObject )
{
	// Initialize schema for surface object
	if(!CGameUISurface::initSchema(pWindowObject->getSchema().c_str()))
		return false;

	//
	// Create title text object
	//
	const ui_objectinfo_t* pTitleTextObject = pWindowDesc->getObject(UI_OBJECT_TEXT, DOCUMENTSWINDOW_TITLE_TEXT_OBJ_NAME);
	if(!pTitleTextObject)
	{
		cl_engfuncs.pfnCon_EPrintf("Window description file '%s' has no definition for '%s'.\n", DOCUMENTSWINDOW_DESC_FILE, DOCUMENTSWINDOW_TITLE_TEXT_OBJ_NAME);
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
	m_pListingObjectsArray.push_back(pWindowTitleText);

	//
	// Create upper separator
	//
	const ui_objectinfo_t* pUpperSeparatorObject = pWindowDesc->getObject(UI_OBJECT_SEPARATOR_H, DOCUMENTSWINDOW_UPPER_SEPARATOR_OBJ_NAME);
	if(!pUpperSeparatorObject)
	{
		cl_engfuncs.pfnCon_EPrintf("Window description file '%s' has no definition for '%s'.\n", DOCUMENTSWINDOW_DESC_FILE, DOCUMENTSWINDOW_UPPER_SEPARATOR_OBJ_NAME);
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
		cl_engfuncs.pfnCon_EPrintf("Failed to initialize 'CGameUIHorizontalSeparator' object named '%s'.\n", DOCUMENTSWINDOW_UPPER_SEPARATOR_OBJ_NAME);
		return false;
	}

	//
	// Create buttons tab object
	//
	const ui_objectinfo_t* pDocumentButtonTabObject = pWindowDesc->getObject(UI_OBJECT_TAB, DOCUMENTSWINDOW_BUTTON_TAB_OBJ_NAME);
	if(!pDocumentButtonTabObject)
	{
		cl_engfuncs.pfnCon_EPrintf("Window description file '%s' has no definition for '%s'.\n", DOCUMENTSWINDOW_DESC_FILE, DOCUMENTSWINDOW_BUTTON_TAB_OBJ_NAME);
		return false;
	}

	pFontSet = g_pGUIManager->GetSchemaResolutionFont(pDocumentButtonTabObject->getTextSchemaName().c_str());

	// Create the buttons tab
	m_pButtonsTab = new CGameUIScrollableSurface(
		pDocumentButtonTabObject->getFlags(),
		pFontSet,
		pWindowObject->getXInset() + pDocumentButtonTabObject->getXOrigin(),
		pWindowObject->getYInset() + pDocumentButtonTabObject->getYOrigin(),
		pDocumentButtonTabObject->getWidth(),
		pDocumentButtonTabObject->getHeight(),
		pDocumentButtonTabObject->getListPostSpacing());
	m_pButtonsTab->setParent(this);

	if(!m_pButtonsTab->initSchema(pDocumentButtonTabObject->getSchema().c_str(), pDocumentButtonTabObject->getButtonSchema().c_str()))
	{
		cl_engfuncs.pfnCon_EPrintf("Failed to initialize 'CGameUIScrollableSurface' object named '%s'.\n", DOCUMENTSWINDOW_BUTTON_TAB_OBJ_NAME);
		return false;
	}

	m_pListingObjectsArray.push_back(m_pButtonsTab);
	
	//
	// Create lower separator
	//
	const ui_objectinfo_t* pLowerSeparatorObject = pWindowDesc->getObject(UI_OBJECT_SEPARATOR_H, DOCUMENTSWINDOW_LOWER_SEPARATOR_OBJ_NAME);
	if(!pLowerSeparatorObject)
	{
		cl_engfuncs.pfnCon_EPrintf("Window description file '%s' has no definition for '%s'.\n", DOCUMENTSWINDOW_DESC_FILE, DOCUMENTSWINDOW_LOWER_SEPARATOR_OBJ_NAME);
		return false;
	}

	CGameUIHorizontalSeparator* pLowerSeparator = new CGameUIHorizontalSeparator(
		pLowerSeparatorObject->getFlags(), 
		pLowerSeparatorObject->getWidth(), 
		pLowerSeparatorObject->getHeight(), 
		pWindowObject->getXInset() + pLowerSeparatorObject->getXOrigin(), 
		pWindowObject->getYInset() + pLowerSeparatorObject->getYOrigin());
	pLowerSeparator->setParent(this);

	if(!pLowerSeparator->initSchema(pUpperSeparatorObject->getSchema().c_str()))
	{
		cl_engfuncs.pfnCon_EPrintf("Failed to initialize 'CGameUIHorizontalSeparator' object named '%s'.\n", DOCUMENTSWINDOW_LOWER_SEPARATOR_OBJ_NAME);
		return false;
	}

	//
	// Create the exit button
	//

	const ui_objectinfo_t* pExitButtonObject = pWindowDesc->getObject(UI_OBJECT_BUTTON, DOCUMENTSWINDOW_EXIT_BUTTON_OBJ_NAME);
	if(!pExitButtonObject)
	{
		cl_engfuncs.pfnCon_EPrintf("Window description file '%s' has no definition for '%s'.\n", DOCUMENTSWINDOW_DESC_FILE, DOCUMENTSWINDOW_EXIT_BUTTON_OBJ_NAME);
		return false;
	}

	// Create UI object
	CGameUIDocumentsWindowButtonCallbackEvent* pExitEvent = new CGameUIDocumentsWindowButtonCallbackEvent(this,  DOC_BUTTON_EXIT);
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
	m_pListingObjectsArray.push_back(pExitButton);

	//
	// Create text object for reader title
	//
 	const ui_objectinfo_t* pReaderTitleTextObject = pWindowDesc->getObject(UI_OBJECT_TEXT, DOCUMENTSWINDOW_READER_TITLE_TEXT_OBJ_NAME);
	if(!pReaderTitleTextObject)
	{
		cl_engfuncs.pfnCon_EPrintf("Window description file '%s' has no definition for '%s'.\n", DOCUMENTSWINDOW_DESC_FILE, DOCUMENTSWINDOW_READER_TITLE_TEXT_OBJ_NAME);
		return false;
	}

	pFontSet = g_pGUIManager->GetSchemaResolutionFont(pReaderTitleTextObject->getTextSchemaName().c_str());

	m_pReaderTitleText = new CGameUIText(
		pReaderTitleTextObject->getFlags(), 
		pReaderTitleTextObject->getTextColor(), 
		pFontSet, 
		pWindowObject->getXInset() + pReaderTitleTextObject->getXOrigin(), 
		pWindowObject->getYInset() + pReaderTitleTextObject->getYOrigin());

	m_pReaderTitleText->setParent(this);
	m_pReaderTitleText->setText(pReaderTitleTextObject->getText().c_str());
	m_pTextReaderObjectsArray.push_back(m_pReaderTitleText);

	// Remember defaults for this
	m_pDefaultTitleFont = pFontSet;
	m_defaultTitleColor = pReaderTitleTextObject->getTextColor();

	//
	// Create the text tab object
	//
	const ui_objectinfo_t* pTextTabObject = pWindowDesc->getObject(UI_OBJECT_TEXT_TAB, DOCUMENTSWINDOW_READER_TEXT_TAB_OBJ_NAME);
	if(!pTextTabObject)
	{
		cl_engfuncs.pfnCon_EPrintf("Window description file '%s' has no definition for '%s'.\n", DOCUMENTSWINDOW_DESC_FILE, DOCUMENTSWINDOW_READER_TEXT_TAB_OBJ_NAME);
		return false;
	}

	pFontSet = g_pGUIManager->GetSchemaResolutionFont(pTextTabObject->getTextSchemaName().c_str());

	m_pReaderTextTab = new CGameUITextTab(pTextTabObject->getFlags(), 
		pFontSet,
		pTextTabObject->getTextInset(),
		pTextTabObject->getTextColor(),
		pWindowObject->getXInset() + pTextTabObject->getXOrigin(),
		pWindowObject->getYInset() + pTextTabObject->getYOrigin(),
		pTextTabObject->getWidth(),
		pTextTabObject->getHeight());
	m_pReaderTextTab->setParent(this);

	if(!m_pReaderTextTab->initSchema(pTextTabObject->getSchema().c_str(), pTextTabObject->getButtonSchema().c_str()))
	{
		cl_engfuncs.pfnCon_EPrintf("Failed to initialize 'CGameUITextTab'.\n");
		return false;
	}

	m_pTextReaderObjectsArray.push_back(m_pReaderTextTab);

	// Remember defaults for this
	m_pDefaultTextFont = pFontSet;
	m_defaultTextColor = pTextTabObject->getTextColor();

	//
	// Create the "Back" button
	//

	const ui_objectinfo_t* pBackButtonObject = pWindowDesc->getObject(UI_OBJECT_BUTTON, DOCUMENTSWINDOW_READER_BACK_BTN_OBJ_NAME);
	if(!pBackButtonObject)
	{
		cl_engfuncs.pfnCon_EPrintf("Window description file '%s' has no definition for '%s'.\n", DOCUMENTSWINDOW_DESC_FILE, DOCUMENTSWINDOW_READER_BACK_BTN_OBJ_NAME);
		return false;
	}

	// Create UI object
	CGameUIDocumentsWindowButtonCallbackEvent* pBackEvent = new CGameUIDocumentsWindowButtonCallbackEvent(this,  DOC_BUTTON_BACK);
	CGameUIButton* pBackButton = new CGameUIButton(pBackButtonObject->getFlags(), 
		pBackEvent, 
		SDLK_BACKSPACE,
		pWindowObject->getXInset() + pBackButtonObject->getXOrigin(),
		pWindowObject->getYInset() + pBackButtonObject->getYOrigin(),
		pBackButtonObject->getWidth(),
		pBackButtonObject->getHeight());

	if(!pBackButton->initSchema(pBackButtonObject->getSchema().c_str()))
	{
		cl_engfuncs.pfnCon_EPrintf("Failed to initialize 'CGameUIButton'.\n");
		return false;
	}

	pBackButton->setText(pBackButtonObject->getText().c_str());
	pBackButton->setParent(this);
	m_pTextReaderObjectsArray.push_back(pBackButton);

	return true;
}

//====================================
//
//====================================
bool CGameUIDocumentsWindow::initData( const CArray<CString>& textFilesArray, const Char* pstrActiveFileName )
{
	// Keep track of this
	Uint32 buttonYOffset = 0;

	// Get schema for the buttons and the documents tab
	ui_windowdescription_t* pWindowDesc = g_pGUIManager->LoadWindowDescriptionFile(DOCUMENTSWINDOW_OBJ_NAME, DOCUMENTSWINDOW_DESC_FILE);
 	if(!pWindowDesc)
	{
		cl_engfuncs.pfnCon_EPrintf("Failed to load window description '%s' for '%s'.\n", DOCUMENTSWINDOW_DESC_FILE, DOCUMENTSWINDOW_OBJ_NAME);
		return nullptr;
	}

	// Get button object
	const ui_objectinfo_t* pDocumentButtonObject = pWindowDesc->getObject(UI_OBJECT_BUTTON, DOCUMENTSWINDOW_DOCUMENT_BUTTON_OBJ_NAME);
	if(!pDocumentButtonObject)
	{
		cl_engfuncs.pfnCon_EPrintf("Window description file '%s' has no definition for '%s'.\n", DOCUMENTSWINDOW_DESC_FILE, DOCUMENTSWINDOW_DOCUMENT_BUTTON_OBJ_NAME);
		return false;
	}

	// Get documents tab info
	const ui_objectinfo_t* pDocumentButtonTabObject = pWindowDesc->getObject(UI_OBJECT_TAB, DOCUMENTSWINDOW_BUTTON_TAB_OBJ_NAME);
	if(!pDocumentButtonTabObject)
	{
		cl_engfuncs.pfnCon_EPrintf("Window description file '%s' has no definition for '%s'.\n", DOCUMENTSWINDOW_DESC_FILE, DOCUMENTSWINDOW_BUTTON_TAB_OBJ_NAME);
		return false;
	}

	// Track separately to avoid issues if a file is missing/bad
	Uint32 nbAdded = 0;
	for(Uint32 i = 0; i < textFilesArray.size(); i++)
	{
		CString textFileEntry = textFilesArray[i];

		CString textFilePath;
		CString textFileCode;

		// Check for the code
		Int32 semicolonPosition = textFileEntry.find(0, ";");
		if(semicolonPosition != CString::CSTRING_NO_POSITION)
		{
			Uint32 codeLength = textFileEntry.length() - semicolonPosition - 1;
			textFileCode.assign(textFileEntry.c_str() + semicolonPosition + 1, codeLength);
			textFilePath.assign(textFileEntry.c_str(), semicolonPosition);
		}
		else
		{
			// Just straight up use the entry
			textFilePath = textFileEntry;
		}

		// See if we already have this loaded
		textdocumentinfo_t* pDocument = nullptr;
		for(Uint32 j = 0; j < m_documentsArray.size(); j++)
		{
			textdocumentinfo_t* pCheck = m_documentsArray[j];
			if(!qstrcmp(pCheck->filePath, textFilePath))
			{
				pDocument = pCheck;
				break;
			}
		}

		// If not, load it in
		if(!pDocument)
		{
			// Title text
			CString titletext;
			// Font set name
			CString titletextschema;
			// Text color
			color32_t titletextcolor;
			// TRUE if text color was set
			bool titleTextColorWasSet = false;
			// Font set name
			CString textschema;
			// Text color
			color32_t textcolor;
			// TRUE if text color was set
			bool textColorWasSet = false;
			// Contents of the document
			CString documentContents;

			// Now load and process the file
			if(!CGameUITextWindow::ProcessTextFile(textFilePath.c_str(), textFileCode, titletext, titletextschema, titletextcolor, titleTextColorWasSet, textschema, textcolor, textColorWasSet, documentContents))
			{
				cl_engfuncs.pfnCon_Printf("%s - Failed to process document '%s'.\n", __FUNCTION__, textFilePath.c_str());
				continue;
			}

			pDocument = new textdocumentinfo_t;
			m_documentsArray.push_back(pDocument);

			// Set file path and code
			pDocument->documentEntry = textFileEntry;
			pDocument->filePath = textFilePath;

			// Set document specifics
			pDocument->documentTitle = titletext;
			pDocument->titleTextSchema = titletextschema;
			pDocument->titleTextColor = titletextcolor;
			pDocument->titleColorWasSet = titleTextColorWasSet;
			pDocument->textSchema = textschema;
			pDocument->textColor = textcolor;
			pDocument->textColorWasSet = textColorWasSet;
			pDocument->documentText = documentContents;
		}

		// Assign button contents and such
		Uint32 addIndex = nbAdded;
		m_buttonsArray.resize(nbAdded + 1);
		nbAdded++;

		// Assign data read from the document
		button_t& newButton = m_buttonsArray[addIndex];
		newButton.pDocumentInfo = pDocument;

		// Create the button object
		CGameUIDocumentsWindowButtonCallbackEvent* pEvent = new CGameUIDocumentsWindowButtonCallbackEvent(this, DOC_BUTTON_DOCUMENT_0 + addIndex);
		newButton.pButton = new CGameUIButton(pDocumentButtonTabObject->getFlags(),
			pEvent,
			pDocumentButtonTabObject->getXInset() + pDocumentButtonObject->getXOrigin(),
			pDocumentButtonTabObject->getYInset() + buttonYOffset + pDocumentButtonObject->getYOrigin(),
			pDocumentButtonObject->getWidth(),
			pDocumentButtonObject->getHeight());
		newButton.pButton->setParent(m_pButtonsTab);
		newButton.pButton->setVisible(false);
		m_pListingObjectsArray.push_back(newButton.pButton);

		if(!newButton.pButton->initSchema(pDocumentButtonObject->getSchema().c_str()))
		{
			cl_engfuncs.pfnCon_EPrintf("Failed to initialize 'CGameUIButton'.\n");
			return false;
		}

		// Set button properties
		buttonYOffset += pDocumentButtonObject->getHeight() + pDocumentButtonTabObject->getListPostSpacing();
		newButton.pButton->setText(pDocument->documentTitle.c_str());
	}

	if(pstrActiveFileName)
	{
		for(Uint32 i = 0; i < m_documentsArray.size(); i++)
		{
			textdocumentinfo_t* pInfo = m_documentsArray[i];
			if(!qstrcmp(pInfo->filePath, pstrActiveFileName))
			{
				SetActiveDocument(pInfo);
				break;
			}
		}
	}

	// Enable the front layer if nothing got selected
	if(!m_pCurrentDocument)
		SetActiveDocument(nullptr);

	// Clear "new objective" flag
	gHUD.SetNewObjective(false);

	return true;
}

//====================================
//
//====================================
void CGameUIDocumentsWindow::getInformation( CArray<CString>& textFilesArray, CString& pstrActiveFileName ) const
{
	// The buttons array will contain all of the original document list
	for(Uint32 i = 0; i < m_buttonsArray.size(); i++)
		textFilesArray.push_back(m_buttonsArray[i].pDocumentInfo->documentEntry);

	if(m_pCurrentDocument)
		pstrActiveFileName = m_pCurrentDocument->filePath;
}

//====================================
//
//====================================
void CGameUIDocumentsWindow::ManageEvent( documentsbuttonevent_t event )
{
	if (event >= DOC_BUTTON_DOCUMENT_0)
	{
		button_t& button = m_buttonsArray[event - DOC_BUTTON_DOCUMENT_0];
		if (!button.pButton->isVisible())
			return;

		// Set this as the active document
		SetActiveDocument(button.pDocumentInfo);
	}
	else if(event == DOC_BUTTON_EXIT)
	{
		// Remove window
		m_windowFlags |= CGameUIWindow::FL_WINDOW_KILLME;
	}
	else if(event == DOC_BUTTON_BACK)
	{
		// Close current document and unhide the main tab
		SetActiveDocument(nullptr);
	}
	else
	{
		cl_engfuncs.pfnCon_Printf("%s - Unknown button event %d.\n", __FUNCTION__, event);
	}
}

//====================================
//
//====================================
void CGameUIDocumentsWindow::SetActiveDocument( textdocumentinfo_t* pDocument )
{
	if(!pDocument)
	{
		for(Uint32 i = 0; i < m_pListingObjectsArray.size(); i++)
		{
			m_pListingObjectsArray[i]->setVisible(true);
			m_pListingObjectsArray[i]->setDisabled(false);
		}

		for(Uint32 i = 0; i < m_pTextReaderObjectsArray.size(); i++)
		{
			m_pTextReaderObjectsArray[i]->setVisible(false);
			m_pTextReaderObjectsArray[i]->setDisabled(true);
		}
	}
	else
	{
		for(Uint32 i = 0; i < m_pListingObjectsArray.size(); i++)
		{
			m_pListingObjectsArray[i]->setVisible(false);
			m_pListingObjectsArray[i]->setDisabled(true);
		}

		for(Uint32 i = 0; i < m_pTextReaderObjectsArray.size(); i++)
		{
			m_pTextReaderObjectsArray[i]->setVisible(true);
			m_pTextReaderObjectsArray[i]->setDisabled(false);
		}

		CString textschema = pDocument->textSchema;
		CString titletextschema = pDocument->titleTextSchema;

		Uint32 screenWidth, screenHeight;
		cl_renderfuncs.pfnGetScreenSize(screenWidth, screenHeight);

		// Load font if not default
		const font_set_t* pfontset = nullptr;
		if(!textschema.empty())
			pfontset = cl_engfuncs.pfnGetResolutionSchemaFontSet(textschema.c_str(), screenHeight);

		if(!pfontset)
		{
			pfontset = m_pDefaultTextFont;

			if(!pfontset)
				pfontset = gGameUIManager.GetDefaultFontSet();
		}

		const font_set_t* ptitlefontset = nullptr;
		if(!titletextschema.empty())
			ptitlefontset = cl_engfuncs.pfnGetResolutionSchemaFontSet(titletextschema.c_str(), screenHeight);

		if(!ptitlefontset)
		{
			ptitlefontset = m_pDefaultTitleFont;

			if(!ptitlefontset)
				ptitlefontset = gGameUIManager.GetDefaultFontSet();
		}

		// Assign the contents
		if(pDocument->textColorWasSet)
			m_pReaderTextTab->setTextColor(pDocument->textColor);
		else
			m_pReaderTextTab->setTextColor(m_defaultTextColor);

		m_pReaderTextTab->setFontSet(pfontset);
		m_pReaderTextTab->initData(reinterpret_cast<const byte*>(pDocument->documentText.c_str()), pDocument->documentText.length());

		if(!pDocument->documentTitle.empty())
		{
			if(pDocument->titleColorWasSet)
				m_pReaderTitleText->setColor(pDocument->titleTextColor);
			else
				m_pReaderTitleText->setColor(m_defaultTitleColor);

			m_pReaderTitleText->setFontSet(ptitlefontset);
			m_pReaderTitleText->setText(pDocument->documentTitle.c_str());
		}
	}

	// Remember this
	m_pCurrentDocument = pDocument;
}

//====================================
//
//====================================
CGameUIDocumentsWindow* CGameUIDocumentsWindow::CreateInstance( void )
{
	// Load the schema file
	ui_windowdescription_t* pWinDesc = g_pGUIManager->LoadWindowDescriptionFile(DOCUMENTSWINDOW_OBJ_NAME, DOCUMENTSWINDOW_DESC_FILE);
 	if(!pWinDesc)
	{
		cl_engfuncs.pfnCon_EPrintf("Failed to load window description '%s' for '%s'.\n", DOCUMENTSWINDOW_DESC_FILE, DOCUMENTSWINDOW_OBJ_NAME);
		return nullptr;
	}

	const ui_objectinfo_t* pWindowObject = pWinDesc->getObject(UI_OBJECT_WINDOW, DOCUMENTSWINDOW_OBJ_NAME);
	if(!pWindowObject)
	{
		cl_engfuncs.pfnCon_EPrintf("Window description file '%s' has no definition for '%s'.\n", DOCUMENTSWINDOW_DESC_FILE, DOCUMENTSWINDOW_OBJ_NAME);
		return nullptr;
	}

	Uint32 screenWidth, screenHeight;
	cl_renderfuncs.pfnGetScreenSize(screenWidth, screenHeight);

	Uint32 relativeWidth = gHUDDraw.ScaleY(pWindowObject->getWidth());
	Uint32 relativeHeight = gHUDDraw.ScaleY(pWindowObject->getHeight());

	Int32 xPosition = (screenWidth / 2) - (relativeWidth / 2);
	Int32 yPosition = (screenHeight / 2) - (relativeHeight / 2);

	CGameUIDocumentsWindow* pNewWindow = new CGameUIDocumentsWindow(CGameUIWindow::FL_WINDOW_NONE, xPosition, yPosition, relativeWidth, relativeHeight);
	if(!pNewWindow->init(pWinDesc, pWindowObject))
	{
		cl_engfuncs.pfnCon_EPrintf("%s - Failed to initialize 'CGameUIDocumentsWindow'.\n", __FUNCTION__);
		delete pNewWindow;
		return nullptr;
	}

	return pNewWindow;
}

//====================================
//
//====================================
void CGameUIDocumentsWindowButtonCallbackEvent::PerformAction( Float param )
{
	if(!m_pWindow)
		return;

	m_pWindow->ManageEvent(m_eventType);
}
