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

#include "gameuiinventorywindow.h"
#include "gameuiwindows_shared.h"
#include "snd_shared.h"
#include "gameui_shared.h"
#include "gameuitextwindow.h"

// Window description file
const Char CGameUIInventoryWindow::INVENTORYWINDOW_DESC_FILE[] = "gameuiinventoryswindow.txt";
// Window object name
const Char CGameUIInventoryWindow::INVENTORYWINDOW_OBJ_NAME[] = "GameUIInventoryWindow";
// Title text object name
const Char CGameUIInventoryWindow::INVENTORYWINDOW_TITLE_TEXT_OBJ_NAME[] = "GameUIInventoryWindowTitleText";
// Exit window button object name
const Char CGameUIInventoryWindow::INVENTORYWINDOW_EXIT_BUTTON_OBJ_NAME[] = "GameUIInventoryWindowExitButton";

// Inventory background surface object name
const Char CGameUIInventoryWindow::INVENTORYWINDOW_BG_SURFACE_OBJ_NAME[] = "GameUIInventoryBackgroundSurface";
// Inventory cell object name
const Char CGameUIInventoryWindow::INVENTORYWINDOW_CELL_OBJ_NAME[] = "GameUIInventoryCell";

//====================================
//
//====================================
CGameUIInventoryWindow::CGameUIInventoryWindow( Int32 flags, Int32 originX, Int32 originY, Uint32 width, Uint32 height ):
	CGameUIWindow(flags, originX, originY, width, height),
	m_pExitButton(nullptr),
	m_horizontalRowCount(0),
	m_verticalRowCount(0),
	m_pCellBackgroundSurface(nullptr)
{
}

//====================================
//
//====================================
CGameUIInventoryWindow::~CGameUIInventoryWindow( void )
{
}

//====================================
//
//====================================
bool CGameUIInventoryWindow::init( const ui_windowdescription_t* pWindowDesc, const ui_objectinfo_t* pWindowObject )
{
	// Initialize schema for surface object
	if(!CGameUISurface::initSchema(pWindowObject->getSchema().c_str()))
		return false;

	// We'll need this later for cells
	m_pWindowDescription = pWindowDesc;
	m_pWindowObjectInfo = pWindowObject;

	//
	// Create title text object
	//
	const ui_objectinfo_t* pTitleTextObject = m_pWindowDescription->getObject(UI_OBJECT_TEXT, INVENTORYWINDOW_TITLE_TEXT_OBJ_NAME);
	if(!pTitleTextObject)
	{
		cl_engfuncs.pfnCon_EPrintf("Window description file '%s' has no definition for '%s'.\n", INVENTORYWINDOW_DESC_FILE, INVENTORYWINDOW_TITLE_TEXT_OBJ_NAME);
		return false;
	}

	const font_set_t* pFontSet = g_pGUIManager->GetSchemaResolutionFont(pTitleTextObject->getTextSchemaName().c_str());

	CGameUIText* pWindowTitleText = new CGameUIText(
		pTitleTextObject->getFlags(), 
		pTitleTextObject->getTextColor(), 
		pFontSet, 
		m_pWindowObjectInfo->getXInset() + pTitleTextObject->getXOrigin(), 
		m_pWindowObjectInfo->getYInset() + pTitleTextObject->getYOrigin());

	pWindowTitleText->setParent(this);
	pWindowTitleText->setText(pTitleTextObject->getText().c_str());

	//
	// Create the exit button
	//

	const ui_objectinfo_t* pExitButtonObject = m_pWindowDescription->getObject(UI_OBJECT_BUTTON, INVENTORYWINDOW_EXIT_BUTTON_OBJ_NAME);
	if(!pExitButtonObject)
	{
		cl_engfuncs.pfnCon_EPrintf("Window description file '%s' has no definition for '%s'.\n", INVENTORYWINDOW_DESC_FILE, INVENTORYWINDOW_EXIT_BUTTON_OBJ_NAME);
		return false;
	}

	// Create UI object
	CGameUIInventoryWindowButtonCallbackEvent* pExitEvent = new CGameUIInventoryWindowButtonCallbackEvent(this,  INV_BUTTON_EXIT);
	CGameUIButton* pExitButton = new CGameUIButton(pExitButtonObject->getFlags(), 
		pExitEvent, 
		SDLK_RETURN,
		m_pWindowObjectInfo->getXInset() + pExitButtonObject->getXOrigin(),
		m_pWindowObjectInfo->getYInset() + pExitButtonObject->getYOrigin(),
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
bool CGameUIInventoryWindow::initData( Uint32 horizontalRowCount, Uint32 verticalRowCount )
{
	// Set basic data
	m_horizontalRowCount = horizontalRowCount;
	m_verticalRowCount = verticalRowCount;

	//
	// Get data for individual cells
	//
	const ui_objectinfo_t* pCellObject = m_pWindowDescription->getObject(UI_OBJECT_TAB, INVENTORYWINDOW_CELL_OBJ_NAME);
	if(!pCellObject)
	{
		cl_engfuncs.pfnCon_EPrintf("Window description file '%s' has no definition for '%s'.\n", INVENTORYWINDOW_DESC_FILE, INVENTORYWINDOW_CELL_OBJ_NAME);
		return false;
	}

	//
	// Get data for background
	//
	const ui_objectinfo_t* pCellBackgroundObject = m_pWindowDescription->getObject(UI_OBJECT_TAB, INVENTORYWINDOW_BG_SURFACE_OBJ_NAME);
	if(!pCellObject)
	{
		cl_engfuncs.pfnCon_EPrintf("Window description file '%s' has no definition for '%s'.\n", INVENTORYWINDOW_DESC_FILE, INVENTORYWINDOW_BG_SURFACE_OBJ_NAME);
		return false;
	}

	Uint32 backgroundWidth = (pCellObject->width * m_horizontalRowCount) + pCellBackgroundObject->insetx * 2;
	Uint32 backgroundHeight = (pCellObject->height * m_verticalRowCount) + pCellBackgroundObject->insety * 2;

	m_pCellBackgroundSurface = new CGameUISurface(pCellBackgroundObject->getFlags(), 
		m_pWindowObjectInfo->getXInset() + pCellBackgroundObject->getXOrigin(),
		m_pWindowObjectInfo->getYInset() + pCellBackgroundObject->getYOrigin(),
		backgroundWidth,
		backgroundHeight);
	m_pCellBackgroundSurface->setParent(this);

	if(!m_pCellBackgroundSurface->initSchema(pCellBackgroundObject->getSchema().c_str()))
	{
		cl_engfuncs.pfnCon_EPrintf("Failed to initialize 'CGameUITextTab'.\n");
		return false;
	}

	Uint32 cellCount = m_horizontalRowCount * m_verticalRowCount;
	m_pCellsArray.resize(cellCount);

	for(Uint32 i = 0; i < cellCount; i++)
	{
		Uint32 xOrigin = pCellBackgroundObject->getXInset() + (i % m_verticalRowCount) * pCellObject->width;
		Uint32 yOrigin = pCellBackgroundObject->getYInset() + (i / m_horizontalRowCount) * pCellObject->height;

		CGameUISurface* pCell = new CGameUISurface(pCellObject->getFlags(), xOrigin, yOrigin, backgroundWidth, backgroundHeight);
		m_pCellsArray[i] = pCell;

		pCell->setParent(m_pCellBackgroundSurface);
	}

	return true;
}

//====================================
//
//====================================
void CGameUIInventoryWindow::getInformation( Uint32& horizontalRowCount, Uint32& verticalRowCount ) const
{
	horizontalRowCount = m_horizontalRowCount;
	verticalRowCount = m_verticalRowCount;
}

//====================================
//
//====================================
void CGameUIInventoryWindow::ManageEvent( inventorybuttonevent_t event )
{
	if(event == INV_BUTTON_EXIT)
	{
		// Remove window
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
CGameUIInventoryWindow* CGameUIInventoryWindow::CreateInstance( void )
{
	// Load the schema file
	ui_windowdescription_t* pWinDesc = g_pGUIManager->LoadWindowDescriptionFile(INVENTORYWINDOW_OBJ_NAME, INVENTORYWINDOW_DESC_FILE);
 	if(!pWinDesc)
	{
		cl_engfuncs.pfnCon_EPrintf("Failed to load window description '%s' for '%s'.\n", INVENTORYWINDOW_DESC_FILE, INVENTORYWINDOW_OBJ_NAME);
		return nullptr;
	}

	const ui_objectinfo_t* pWindowObject = pWinDesc->getObject(UI_OBJECT_WINDOW, INVENTORYWINDOW_OBJ_NAME);
	if(!pWindowObject)
	{
		cl_engfuncs.pfnCon_EPrintf("Window description file '%s' has no definition for '%s'.\n", INVENTORYWINDOW_DESC_FILE, INVENTORYWINDOW_OBJ_NAME);
		return nullptr;
	}

	Uint32 screenWidth, screenHeight;
	cl_renderfuncs.pfnGetScreenSize(screenWidth, screenHeight);

	Uint32 relativeWidth = gHUDDraw.ScaleY(pWindowObject->getWidth());
	Uint32 relativeHeight = gHUDDraw.ScaleY(pWindowObject->getHeight());

	Int32 xPosition = (screenWidth / 2) - (relativeWidth / 2);
	Int32 yPosition = (screenHeight / 2) - (relativeHeight / 2);

	CGameUIInventoryWindow* pNewWindow = new CGameUIInventoryWindow(CGameUIWindow::FL_WINDOW_NONE, xPosition, yPosition, relativeWidth, relativeHeight);
	if(!pNewWindow->init(pWinDesc, pWindowObject))
	{
		cl_engfuncs.pfnCon_EPrintf("%s - Failed to initialize 'CGameUIInventoryWindow'.\n", __FUNCTION__);
		delete pNewWindow;
		return nullptr;
	}

	return pNewWindow;
}

//====================================
//
//====================================
void CGameUIInventoryWindowButtonCallbackEvent::PerformAction( Float param )
{
	if(!m_pWindow)
		return;

	m_pWindow->ManageEvent(m_eventType);
}
