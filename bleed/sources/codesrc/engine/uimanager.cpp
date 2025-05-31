/*
===============================================
Pathos Engine - Created by Andrew Stephen "Overfloater" Lucas

Copyright 2016
All Rights Reserved.
===============================================
*/

#include "includes.h"
#include "file.h"
#include "window.h"
#include "input.h"

#include "texturemanager.h"
#include "r_main.h"
#include "r_vbo.h"
#include "r_glsl.h"
#include "r_basicdraw.h"
#include "r_text.h"
#include "r_interface.h"
#include "r_interfacefuncs.h"
#include "cl_utils.h"
#include "cl_main.h"

#include "uielements.h"
#include "uimanager.h"
#include "uiconsolewindow.h"
#include "system.h"
#include "enginestate.h"
#include "enginefuncs.h"
#include "textschemas.h"

// Default font schema of the game UI
const Char CUIManager::DEFAULT_TEXT_SCHEMA[] = "uidefault";

// Class definition
CUIManager gUIManager;

//=============================================
// @brief Constructor
//
//=============================================
CUIManager::CUIManager( void ):
	m_pSchemaManager(nullptr),
	m_pFocusWindow(nullptr),
	m_currentFocusIndex(0),
	m_windowFilterFlags(CUIWindow::UIW_FL_NONE),
	m_pFontSet(nullptr)
{
}

//=============================================
// @brief Destructor
//
//=============================================
CUIManager::~CUIManager( void )
{
	Shutdown();
}

//=============================================
// @brief Initializes interfaces
//
//=============================================
void CUIManager::Init( void )
{
	m_pFontSet = gTextSchemas.GetResolutionSchemaFontSet(DEFAULT_TEXT_SCHEMA, gWindow.GetHeight());
	if(!m_pFontSet)
		m_pFontSet = gText.GetDefaultFont();

	// TODO: Get rid of this
	ui_engine_interface_t uiFuncs;

	// Set the interface
	uiFuncs.pfnGetEngineTime = Engine_GetEngineTime;
	uiFuncs.pfnCon_Printf = Con_Printf;
	uiFuncs.pfnCon_DPrintf = Con_DPrintf;
	uiFuncs.pfnCon_VPrintf = Con_VPrintf;
	uiFuncs.pfnCon_EPrintf = Con_EPrintf;
	uiFuncs.pfnGetMousePosition = CL_GetMousePosition;
	uiFuncs.pfnUILoadSchemaFile = CL_UILoadSchemaFile;
	uiFuncs.pfnBasicDrawIsActive = R_BasicDrawIsActive;
	uiFuncs.pfnEnableBasicDraw = R_EnableBasicDraw,
	uiFuncs.pfnDisableBasicDraw = R_DisableBasicDraw;
	uiFuncs.pfnBasicDrawEnableTextures = R_BasicDrawEnableTextures;
	uiFuncs.pfnBasicDrawDisableTextures = R_BasicDrawDisableTextures;
	uiFuncs.pfnBasicDrawBegin = R_BasicDrawBegin;
	uiFuncs.pfnBasicDrawEnd = R_BasicDrawEnd;
	uiFuncs.pfnBasicDrawColor4f = R_BasicDrawColor4f;
	uiFuncs.pfnBasicDrawColor4fv = R_BasicDrawColor4fv;
	uiFuncs.pfnBasicDrawTexCoord2f = R_BasicDrawTexCoord2f;
	uiFuncs.pfnBasicDrawTexCoord2fv = R_BasicDrawTexCoord2fv;
	uiFuncs.pfnBasicDrawVertex3f = R_BasicDrawVertex3f;
	uiFuncs.pfnBasicDrawVertex3fv = R_BasicDrawVertex3fv;
	uiFuncs.pfnBasicDrawSetProjection = R_BasicDrawSetProjection;
	uiFuncs.pfnBasicDrawSetModelView = R_BasicDrawSetModelView;
	uiFuncs.pfnBind2DTexture = R_Bind2DTexture;
	uiFuncs.pfnGetDefaultFontSet = R_GetDefaultFontSet;
	uiFuncs.pfnLoadFontSet = R_LoadFontSet;
	uiFuncs.pfnDrawSimpleString = R_DrawString;
	uiFuncs.pfnDrawStringBox = R_DrawStringBox;
	uiFuncs.pfnBeginTextRendering = R_BeginTextRendering;
	uiFuncs.pfnFinishTextRendering = R_FinishTextRendering;
	uiFuncs.pfnSetStringRectangle = R_SetStringRectangle;
	uiFuncs.pfnDrawCharacter = R_DrawCharacter;
	uiFuncs.pfnGetStringSize = R_GetStringSize;
	uiFuncs.pfnEstimateStringHeight = R_EstimateStringHeight;
	uiFuncs.pfnGetProjectionMatrix = R_GetProjectionMatrix;
	uiFuncs.pfnGetModelViewMatrix = R_GetModelViewMatrix;
	uiFuncs.pfnGetWindowSize = R_GetScreenSize;
	uiFuncs.pfnValidateBasicDraw = R_IF_ValidateBasicDraw;

	CUIObject::SetRenderInterface(uiFuncs);

	m_pSchemaManager = new CUISchemaManager(FL_GetInterface(), R_GetDummyTexture, R_LoadTexture, CL_GetSchemaFontSet, m_pFontSet);
	m_pSchemaManager->SetScreenResolution(gWindow.GetWidth(), gWindow.GetHeight());
}

//=============================================
// @brief Initializes interfaces
//
//=============================================
void CUIManager::OnGLInitialization( void )
{
	m_windowList.begin();
	while(!m_windowList.end())
	{
		CUIWindow* pWindow = m_windowList.get();
		pWindow->onGLInitialization();

		m_windowList.next();
	}
}

//=============================================
// @brief Destroys all active windows
//
//=============================================
void CUIManager::Shutdown( void )
{
	if(!m_windowList.empty())
	{
		m_windowList.begin();
		while(!m_windowList.end())
		{
			CUIObject* pfree = m_windowList.get();
			delete pfree;

			m_windowList.next();
		}
	}

	if(m_pSchemaManager)
	{
		m_pSchemaManager->Clear();
		delete m_pSchemaManager;
		m_pSchemaManager = nullptr;
	}
}

//=============================================
// @brief  Performs think functions
//
//=============================================
void CUIManager::Think( void )
{
	m_windowList.begin();
	while(!m_windowList.end())
	{
		CUIWindow* pWindow = m_windowList.get();
		pWindow->think();

		m_windowList.next();
	}
}

//=============================================
// @brief  Performs post-command think functions
//
//=============================================
void CUIManager::PostThink( void )
{
	m_windowList.begin();
	while(!m_windowList.end())
	{
		CUIWindow* pWindow = m_windowList.get();
		pWindow->postThink();

		// Destroy any flagged windows
		Int32 flags = pWindow->getWindowFlags();
		if(flags & CUIWindow::UIW_FL_KILLME)
			DestroyWindow(pWindow);

		m_windowList.next();
	}
}

//=============================================
// @brief Draws the active windows
//
//=============================================
bool CUIManager::Draw( void )
{
	// Set modelview
	rns.view.modelview.PushMatrix();
	rns.view.modelview.LoadIdentity();
	rns.view.modelview.Scale(1.0f/gWindow.GetWidth(), 1.0f/gWindow.GetHeight(), 1.0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Set it in the shader
	CBasicDraw* pDraw = CBasicDraw::GetInstance();
	pDraw->SetModelview(rns.view.modelview.GetMatrix());

	// Track render errors
	bool result = true;

	// Windows are sorted by focus index
	m_windowList.rbegin();
	while(!m_windowList.end())
	{
		CUIWindow* pWindow = m_windowList.get();

		if(m_windowFilterFlags && !(pWindow->getWindowFlags() & m_windowFilterFlags))
		{
			m_windowList.prev();
			continue;
		}

		if(pWindow->isVisible())
		{
			result = pWindow->draw();
			if(!result)
				break;
		}

		m_windowList.prev();
	}

	// Restore modelview
	rns.view.modelview.PopMatrix();

	glDisable(GL_BLEND);

	return result;
}

//=============================================
// @brief Draws the active windows
//
//=============================================
bool CUIManager::KeyEvent( Int32 button, Int16 mod, bool keyDown )
{
	if(!m_pFocusWindow)
		return false;

	if(m_pFocusWindow->keyEvent(button, mod, keyDown))
		return true;

	return false;
}

//=============================================
// @brief Draws the active windows
//
//=============================================
bool CUIManager::MouseButtonEvent( Int32 button, bool keyDown )
{
	Int32 cursorX, cursorY;
	gInput.GetMousePosition(cursorX, cursorY);

	m_windowList.begin();
	while(!m_windowList.end())
	{
		// See if we hit this window
		CUIWindow* pWindow = m_windowList.get();
		if(pWindow->isVisible()
			&& pWindow->isMouseOver(cursorX, cursorY) 
			&& pWindow->mouseButtonEvent(cursorX, cursorY, button, keyDown))
		{
			// Set as focus window if it's not already in focus
			if(keyDown && (!m_pFocusWindow || m_pFocusWindow != pWindow))
				SetFocusWindow(pWindow);

			// Release any press states
			if(!keyDown && m_pFocusWindow)
				m_pFocusWindow->releaseClickStates();

			return true;
		}

		m_windowList.next();
	}

	if(m_pFocusWindow)
	{
		// Release any clicked buttons
		if(button == SDL_BUTTON_LEFT && !keyDown)
			m_pFocusWindow->releaseClickStates();

		// We clicked away from any windows, so release focus
		if(button == SDL_BUTTON_LEFT && keyDown)
			SetFocusWindow(nullptr);
	}

	return false;
}

//=============================================
// @brief Draws the active windows
//
//=============================================
bool CUIManager::MouseWheelEvent( Int32 button, bool keyDown, Int32 scroll )
{
	Int32 cursorX, cursorY;
	gInput.GetMousePosition(cursorX, cursorY);

	m_windowList.begin();
	while(!m_windowList.end())
	{
		// See if we hit this window
		CUIWindow* pWindow = m_windowList.get();
		if(pWindow->isVisible()
			&& pWindow->isMouseOver(cursorX, cursorY) 
			&& pWindow->mouseWheelEvent(cursorX, cursorY, button, keyDown, scroll))
		{
			// Set as focus window if it's not already in focus
			if(keyDown && (!m_pFocusWindow || m_pFocusWindow != pWindow))
				SetFocusWindow(pWindow);

			// Release any press states
			if(!keyDown && m_pFocusWindow)
				m_pFocusWindow->releaseClickStates();

			return true;
		}

		m_windowList.next();
	}

	if(m_pFocusWindow)
	{
		// Release any clicked buttons
		if(button == SDL_BUTTON_LEFT && !keyDown)
			m_pFocusWindow->releaseClickStates();

		// We clicked away from any windows, so release focus
		if(button == SDL_BUTTON_LEFT && keyDown)
			SetFocusWindow(nullptr);
	}

	return false;
}


//=============================================
// @brief Sets the current focus window
//
//=============================================
void CUIManager::SetFocusWindow( CUIWindow* pWindow )
{
	if(m_pFocusWindow == pWindow)
		return;

	// Reset previous focus window
	if(m_pFocusWindow)
		m_pFocusWindow->setFocusState(false);

	// Raise focus index
	if(pWindow)
	{
		pWindow->setFocusIndex(m_currentFocusIndex);
		m_currentFocusIndex++;

		// Set window as current focus window
		m_pFocusWindow = pWindow;
		m_pFocusWindow->setFocusState(true);

		// Reorder the windows
		ReorderWindows();
	}
	else if(m_pFocusWindow)
	{
		// No focus on any window
		m_pFocusWindow = nullptr;
		m_currentFocusIndex++;
	}
}

//=============================================
// @brief Hides windows with a specific flag shows the rest
//
//=============================================
void CUIManager::HideWindows( Int32 windowFlags )
{
	if(m_windowList.empty())
		return;

	m_windowList.begin();
	while(!m_windowList.end())
	{
		// See if we hit this window
		CUIWindow* pWindow = m_windowList.get();
		if(pWindow->getWindowFlags() & windowFlags)
		{
			if(pWindow == m_pFocusWindow)
				SetFocusWindow(nullptr);

			// Hide the window
			pWindow->setVisible(false);
		}

		m_windowList.next();
	}
}

//=============================================
// @brief Show windows with a specific flag
//
//=============================================
void CUIManager::ShowWindows( Int32 windowFlags )
{
	if(m_windowList.empty())
		return;

	m_windowList.begin();
	while(!m_windowList.end())
	{
		// See if we hit this window
		CUIWindow* pWindow = m_windowList.get();
		if(pWindow->getWindowFlags() & windowFlags)
		{
			if(!m_pFocusWindow)
			{
				SetFocusWindow(pWindow);
				ShowWindows(windowFlags);
				return;
			}

			// Hide the window
			pWindow->setVisible(true);
		}

		m_windowList.next();
	}
}

//=============================================
// @brief Sorts the windows by focus indexes
//
//=============================================
void CUIManager::ReorderWindows( void )
{
	// Probably not the most efficient setup
	Uint32 currentFocusIdx = m_currentFocusIndex;

	// Temporary list to hold vars
	CLinkedList<CUIWindow*> tmpList;

	while(TRUE)
	{
		// Last nearest value to focus index
		Int32 lastNearest = -1;
		CUIWindow* pLastWindow = nullptr;

		m_windowList.begin();
		while(!m_windowList.end())
		{
			CUIWindow* pWindow = m_windowList.get();
			Uint32 focusIndex = pWindow->getFocusIndex();
			
			if(focusIndex < currentFocusIdx 
				&& lastNearest < static_cast<Int32>(focusIndex))
			{
				pLastWindow = pWindow;
				lastNearest = static_cast<Int32>(focusIndex);
			}

			m_windowList.next();
		}

		// Check if we reached the end
		if(lastNearest == -1)
			break;

		// Set current focus index
		currentFocusIdx = lastNearest;
		tmpList.add(pLastWindow);
	}

	// Shouldn't happen, but still
	assert(tmpList.size() == m_windowList.size());

	// Fill the list with the new values
	m_windowList.clear();

	tmpList.begin();
	while(!tmpList.end())
	{
		m_windowList.add(tmpList.get());
		tmpList.next();
	}
}

//=============================================
// @brief Sorts the windows by focus indexes
//
//=============================================
void CUIManager::RepositionWindows( void )
{
	if(m_windowList.empty())
		return;

	Uint32 winWidth = gWindow.GetWidth();
	Uint32 winHeight = gWindow.GetHeight();

	m_windowList.begin();
	while(!m_windowList.end())
	{
		CUIWindow* pWindow = m_windowList.get();

		Int32 originX, originY;
		pWindow->getAbsPosition(originX, originY);

		Uint32 width, height;
		pWindow->getSize(width, height);

		Int32 newOriginX = originX;
		if(originX + width > winWidth)
			newOriginX = winWidth - width;

		Int32 newOriginY = originY;
		if(originY + height > winHeight)
			newOriginY = winHeight - height;

		// Reposition to new location
		pWindow->setPosition(newOriginX, newOriginY);

		m_windowList.next();
	}

}

//=============================================
// @brief Adds a window to the list
//
//=============================================
void CUIManager::AddWindow( CUIWindow* pWindow )
{
	// Add it to the list
	m_windowList.add(pWindow);
	SetFocusWindow(pWindow);
}

//=============================================
// @brief Destroys a window and removes it from the list
//
//=============================================
void CUIManager::DestroyWindow( CUIWindow* pWindow )
{
	// Remove it from the list first
	m_windowList.remove(pWindow);

	// Remove it from focus
	if(m_pFocusWindow == pWindow)
	{
		// Find window with the highest focus
		Int32 lastHighest = -1;
		CUIWindow* pBestWindow = nullptr;

		m_windowList.begin();
		while(!m_windowList.end())
		{
			CUIWindow* pListWnd = m_windowList.get();
			if(pListWnd != pWindow)
			{
				Uint32 focusIndex = pWindow->getFocusIndex();
				if(static_cast<Int32>(focusIndex) > lastHighest || lastHighest == -1)
				{
					pBestWindow = pListWnd;
					lastHighest = static_cast<Int32>(focusIndex);
				}
			}

			m_windowList.next();
		}

		// Set this as the focus window
		if(pBestWindow)
			SetFocusWindow(pBestWindow);
		else
			m_pFocusWindow = nullptr;
	}

	// Delete this object
	delete pWindow;
}

//=============================================
// @brief
//
//=============================================
bool CUIManager::HasActiveWindows( void )
{
	if(m_windowList.empty())
		return false;

	m_windowList.begin();
	while(!m_windowList.end())
	{
		CUIWindow* pListWnd = m_windowList.get();
		if(pListWnd->isVisible())
			return true;

		m_windowList.next();
	}

	return false;
}

//=============================================
// @brief Loads in a schema file
//
// @param pstrFilename Name of the UI scheme file
// @return Pointer to scheme object
//=============================================
ui_schemeinfo_t* CUIManager::LoadSchemaFile( const Char* pstrFilename )
{
	ui_schemeinfo_t* presult = m_pSchemaManager->LoadSchemaFile(pstrFilename);
	if(!presult)
	{
		const CString& errorStr = m_pSchemaManager->GetErrorString();
		if(!errorStr.empty())
			Con_EPrintf("%s\n", errorStr.c_str());

		return nullptr;
	}
	else
	{
		Uint32 nbWarnings = m_pSchemaManager->GetNbWarnings();
		if(nbWarnings > 0)
		{
			for(Uint32 i = 0; i < nbWarnings; i++)
				Con_Printf("%s\n", m_pSchemaManager->GetWarning(i).c_str());

			m_pSchemaManager->ClearWarnings();
		}

		return presult;
	}
}

//=============================================
// @brief Loads in a schema file
//
// @param pstrFilename Name of the UI scheme file
// @return Pointer to scheme object
//=============================================
ui_windowdescription_t* CUIManager::LoadWindowDescriptionFile( const Char* pstrWindowName, const Char* pstrFilename )
{
	ui_windowdescription_t* presult = m_pSchemaManager->LoadWindowDescriptionFile(pstrWindowName, pstrFilename);
	if(!presult)
	{
		const CString& errorStr = m_pSchemaManager->GetErrorString();
		if(!errorStr.empty())
			Con_EPrintf(errorStr.c_str());

		return nullptr;
	}
	else
	{
		Uint32 nbWarnings = m_pSchemaManager->GetNbWarnings();
		if(nbWarnings > 0)
		{
			for(Uint32 i = 0; i < nbWarnings; i++)
				Con_Printf(m_pSchemaManager->GetWarning(i).c_str());

			m_pSchemaManager->ClearWarnings();
		}

		return presult;
	}
}

//=============================================
// @brief Sets a window flag for draw filtering
//
//=============================================
void CUIManager::SetDrawFilter( Int32 windowFlags )
{
	m_windowFilterFlags |= windowFlags;
}

//=============================================
// @brief Sets a window flag for draw filtering
//
//=============================================
void CUIManager::RemoveDrawFilter( Int32 windowFlags )
{
	m_windowFilterFlags &= ~windowFlags;
}

//=============================================
// @brief Tells if mouse is over any active window
//
//=============================================
bool CUIManager::IsMouseOverAnyWindow( void )
{
	if(m_windowList.empty())
		return false;

	Int32 cursorX, cursorY;
	gInput.GetMousePosition(cursorX, cursorY);

	m_windowList.begin();
	while(!m_windowList.end())
	{
		CUIWindow* pWindow = m_windowList.get();
		if(pWindow->isMouseOver(cursorX, cursorY))
			return true;

		m_windowList.next();
	}

	return false;
}