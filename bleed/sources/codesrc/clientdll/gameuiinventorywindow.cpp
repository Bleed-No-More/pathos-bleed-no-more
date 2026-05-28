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
const Char CGameUIInventoryWindow::INVENTORYWINDOW_DESC_FILE[] = "gameuidocumentswindow.txt";
// Window object name
const Char CGameUIInventoryWindow::INVENTORYWINDOW_OBJ_NAME[] = "GameUIInventoryWindow";
// Title text object name
const Char CGameUIInventoryWindow::INVENTORYWINDOW_TITLE_TEXT_OBJ_NAME[] = "GameUIInventoryWindowTitleText";
// Upper separator object name
const Char CGameUIInventoryWindow::INVENTORYWINDOW_UPPER_SEPARATOR_OBJ_NAME[] = "GameUIInventoryWindowUpperSeparator";
// Lower separator object name
const Char CGameUIInventoryWindow::INVENTORYWINDOW_LOWER_SEPARATOR_OBJ_NAME[] = "GameUIInventoryWindowLowerSeparator";
// Exit window button object name
const Char CGameUIInventoryWindow::INVENTORYWINDOW_EXIT_BUTTON_OBJ_NAME[] = "GameUIInventoryWindowExitWindow";


//====================================
//
//====================================
CGameUIInventoryWindow::CGameUIInventoryWindow( Int32 flags, Int32 originX, Int32 originY, Uint32 width, Uint32 height ):
	CGameUIWindow(flags, originX, originY, width, height),
	m_pExitButton(nullptr)
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

	//
	// Create title text object
	//
	const ui_objectinfo_t* pTitleTextObject = pWindowDesc->getObject(UI_OBJECT_TEXT, INVENTORYWINDOW_TITLE_TEXT_OBJ_NAME);
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
		pWindowObject->getXInset() + pTitleTextObject->getXOrigin(), 
		pWindowObject->getYInset() + pTitleTextObject->getYOrigin());

	pWindowTitleText->setParent(this);
	pWindowTitleText->setText(pTitleTextObject->getText().c_str());

	//
	// Create upper separator
	//
	const ui_objectinfo_t* pUpperSeparatorObject = pWindowDesc->getObject(UI_OBJECT_SEPARATOR_H, INVENTORYWINDOW_UPPER_SEPARATOR_OBJ_NAME);
	if(!pUpperSeparatorObject)
	{
		cl_engfuncs.pfnCon_EPrintf("Window description file '%s' has no definition for '%s'.\n", INVENTORYWINDOW_DESC_FILE, INVENTORYWINDOW_UPPER_SEPARATOR_OBJ_NAME);
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
		cl_engfuncs.pfnCon_EPrintf("Failed to initialize 'CGameUIHorizontalSeparator' object named '%s'.\n", INVENTORYWINDOW_UPPER_SEPARATOR_OBJ_NAME);
		return false;
	}

	//
	// Create lower separator
	//
	const ui_objectinfo_t* pLowerSeparatorObject = pWindowDesc->getObject(UI_OBJECT_SEPARATOR_H, INVENTORYWINDOW_LOWER_SEPARATOR_OBJ_NAME);
	if(!pLowerSeparatorObject)
	{
		cl_engfuncs.pfnCon_EPrintf("Window description file '%s' has no definition for '%s'.\n", INVENTORYWINDOW_DESC_FILE, INVENTORYWINDOW_LOWER_SEPARATOR_OBJ_NAME);
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
		cl_engfuncs.pfnCon_EPrintf("Failed to initialize 'CGameUIHorizontalSeparator' object named '%s'.\n", INVENTORYWINDOW_LOWER_SEPARATOR_OBJ_NAME);
		return false;
	}

	//
	// Create the exit button
	//

	const ui_objectinfo_t* pExitButtonObject = pWindowDesc->getObject(UI_OBJECT_BUTTON, INVENTORYWINDOW_EXIT_BUTTON_OBJ_NAME);
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
bool CGameUIInventoryWindow::initData( Uint32 horizontalRowCount, Uint32 verticalRowCount )
{
	return true;
}

//====================================
//
//====================================
void CGameUIInventoryWindow::getInformation( void ) const
{
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
