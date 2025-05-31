/*
===============================================
Pathos Engine - Created by Andrew Stephen "Overfloater" Lucas

Copyright 2016
All Rights Reserved.
===============================================
*/

#include "includes.h"
#include "gameuielements.h"
#include "clientdll.h"
#include "fontset.h"
#include "gameuiwindows_shared.h"

// Pointer to game ui manager object
CGameUIManager* CGameUIObject::g_pGUIManager = nullptr;

//====================================
//
//====================================
CGameUIObject::CGameUIObject( Int32 flags, Int32 originX, Int32 originY, Uint32 width, Uint32 height ):
	m_flags(flags),
	m_drawFlags(GUIEL_DF_NONE),
	m_originX(originX),
	m_originY(originY),
	m_baseOriginX(originX),
	m_baseOriginY(originY),
	m_color(255, 255, 255, 255),
	m_width(width),
	m_height(height),
	m_pParent(nullptr),
	m_pLinkedObject(nullptr),
	m_isVisible(true),
	m_isInputFocusObject(false)
{
	if(!(m_flags & (UIEL_FL_ALIGN_L|UIEL_FL_ALIGN_R|UIEL_FL_ALIGN_CH)))
		m_flags |= UIEL_FL_ALIGN_L;

	if(!(m_flags & (UIEL_FL_ALIGN_T|UIEL_FL_ALIGN_B|UIEL_FL_ALIGN_CV)))
		m_flags |= UIEL_FL_ALIGN_T;
}

//====================================
//
//====================================
CGameUIObject::~CGameUIObject( void )
{
	if(!m_pChildrenArray.empty())
	{
		for(Uint32 i = 0; i < m_pChildrenArray.size(); i++)
			delete m_pChildrenArray[i];

		m_pChildrenArray.clear();
	}
}

//====================================
//
//====================================
void CGameUIObject::setParent( CGameUIObject* pparent )
{
	m_pParent = pparent;
	m_pParent->addChild(this);

	// Re-adjust position
	adjustPosition();

	// Make sure to call this
	m_pParent->childPostAdjustPosition(this);
}

//====================================
//
//====================================
CGameUIObject* CGameUIObject::getParent( void )
{
	return m_pParent;
}

//====================================
//
//====================================
void CGameUIObject::addChild( CGameUIObject* pchild )
{
	for(Uint32 i = 0; i < m_pChildrenArray.size(); i++)
	{
		if(m_pChildrenArray[i] == pchild)
		{
			cl_engfuncs.pfnCon_Printf("%s - Child already present.\n", __FUNCTION__);
			return;
		}
	}

	m_pChildrenArray.push_back(pchild);
}

//====================================
//
//====================================
void CGameUIObject::removeChild( CGameUIObject* pchild )
{
	for(Uint32 i = 0; i < m_pChildrenArray.size(); i++)
	{
		if(m_pChildrenArray[i] == pchild)
		{
			m_pChildrenArray.erase(i);
			return;
		}
	}
}

//====================================
//
//====================================
CGameUIObject* CGameUIObject::getParentWindow( void )
{
	CGameUIObject* pParent = getParent();
	while(pParent)
	{
		if(pParent->isWindow())
			return pParent;

		pParent = pParent->getParent();
	}

	return nullptr;
}

//====================================
//
//====================================
void CGameUIObject::setPosition( Int32 xcoord, Int32 ycoord )
{
	m_originX = xcoord;
	m_originY = ycoord;
}

//====================================
//
//====================================
void CGameUIObject::getPosition( Int32& xcoord, Int32& ycoord ) const
{
	xcoord = m_originX;
	ycoord = m_originY;
}

//====================================
//
//====================================
void CGameUIObject::getAbsolutePosition( Int32& xcoord, Int32& ycoord )
{
	if(m_pParent)
	{
		Int32 parentx = 0;
		Int32 parenty = 0;

		m_pParent->getAbsolutePosition(parentx, parenty);

		xcoord = parentx + m_originX;
		ycoord = parenty + m_originY;
	}
	else
	{
		xcoord = m_originX;
		ycoord = m_originY;
	}
}

//====================================
//
//====================================
void CGameUIObject::setSize( Uint32 width, Uint32 height )
{
	m_width = width;
	m_height = height;
}

//====================================
//
//====================================
void CGameUIObject::getSize( Uint32& width, Uint32& height ) const
{
	width = m_width;
	height = m_height;
}

//====================================
//
//====================================
void CGameUIObject::setWidth( Uint32 width )
{
	m_width = width;
}

//====================================
//
//====================================
Uint32 CGameUIObject::getWidth( void ) const
{
	return m_width;
}

//====================================
//
//====================================
void CGameUIObject::setHeight( Uint32 height )
{
	m_height = height;
}

//====================================
//
//====================================
Uint32 CGameUIObject::getHeight( void ) const
{
	return m_height;
}

//====================================
//
//====================================
void CGameUIObject::setVisible( bool visible )
{
	m_isVisible = visible;
}

//====================================
//
//====================================
bool CGameUIObject::isVisible( void ) const
{
	return m_isVisible;
}

//====================================
//
//====================================
bool CGameUIObject::isMouseOver( Int32 mousex, Int32 mousey )
{
	Int32 absX, absY;
	getAbsolutePosition(absX, absY);

	if(absX > mousex)
		return false;
	if(absX+(Int32)m_width < mousex)
		return false;
	if(absY > mousey)
		return false;
	if(absY+(Int32)m_height < mousey)
		return false;

	return true;
}

//====================================
//
//====================================
bool CGameUIObject::isMouseOver( void )
{
	Int32 absX, absY;
	getAbsolutePosition(absX, absY);

	Int32 mousex, mousey;
	cl_engfuncs.pfnGetMousePosition(mousex, mousey);

	if(absX > mousex)
		return false;
	if(absX+(Int32)m_width < mousex)
		return false;
	if(absY > mousey)
		return false;
	if(absY+(Int32)m_height < mousey)
		return false;

	return true;
}

//====================================
//
//====================================
void CGameUIObject::setInputFocusObject( CGameUIObject* pObject )
{
	if(!m_pParent)
		return;

	m_pParent->setInputFocusObject(pObject);
}

//====================================
//
//====================================
bool CGameUIObject::isInputFocusObject( void )
{
	return m_isInputFocusObject;
}

//====================================
//
//====================================
void CGameUIObject::setIsInputFocusObject( bool isFocusObject )
{
	m_isInputFocusObject = isFocusObject;

	// Set or remove flag for highlight
	if(m_isInputFocusObject)
		m_drawFlags |= GUIEL_DF_MARK_HIGHLIGHT;
	else
		m_drawFlags &= ~GUIEL_DF_MARK_HIGHLIGHT;
}

//====================================
//
//====================================
Int32 CGameUIObject::getFlags( void ) const
{
	return m_flags;
}

//====================================
//
//====================================
Int32 CGameUIObject::getDrawFlags( void ) const
{
	return m_drawFlags;
}

//====================================
//
//====================================
void CGameUIObject::setDrawFlags( Int32 flags )
{
	m_drawFlags |= flags;
}

//====================================
//
//====================================
void CGameUIObject::think( void )
{
	if(m_pChildrenArray.empty())
		return;

	for(Uint32 i = 0; i < m_pChildrenArray.size(); i++)
		m_pChildrenArray[i]->think();
}

//====================================
//
//====================================
bool CGameUIObject::draw( void )
{
	if(!m_isVisible)
		return true;

	if(m_pChildrenArray.empty())
		return true;

	for(Uint32 i = 0; i < m_pChildrenArray.size(); i++)
	{
		CGameUIObject* pObject = m_pChildrenArray[i];
		if(pObject->getFlags() & UIEL_FL_DRAW_LINKED_HIGHLIGHT_ONLY)
		{
			CGameUIObject* pLinkedObject = pObject->getLinkedObject();
			if(!pLinkedObject->isMouseOver())
				continue;
		}

		if(!pObject->isVisible())
			continue;

		if(!pObject->draw())
			return false;
	}

	return true;
}

//====================================
//
//====================================
void CGameUIObject::adjustPosition( void )
{
	if(!m_pChildrenArray.empty())
	{
		for(Uint32 i = 0; i < m_pChildrenArray.size(); i++)
			m_pChildrenArray[i]->adjustPosition();
	}
	
	if(!m_pParent)
		return;

	Uint32 parentwidth, parentheight;
	m_pParent->getSize(parentwidth, parentheight);

	if(m_flags & UIEL_FL_ALIGN_R)
		m_originX = parentwidth - m_width - m_baseOriginX;
	else if(m_flags & UIEL_FL_ALIGN_CH)
		m_originX = (parentwidth / 2) - (m_width/2);
	else
		m_originX = m_baseOriginX;

	if(m_flags & UIEL_FL_ALIGN_B)
		m_originY = parentheight - m_height - m_baseOriginY;
	else if(m_flags & UIEL_FL_ALIGN_CV)
		m_originY = (parentheight / 2) - (m_height/2);
	else
		m_originY = m_baseOriginY;
}

//====================================
//
//====================================
bool CGameUIObject::mouseWheelEvent( Int32 mouseX, Int32 mouseY, Int32 button, bool keyDown, Int32 scroll )
{
	if(!m_isVisible)
		return false;

	if(!isInteractive())
		return false;

	if(m_pChildrenArray.empty())
		return false;

	for(Uint32 i = 0; i < m_pChildrenArray.size(); i++)
	{
		CGameUIObject* pObject = m_pChildrenArray[i];
		if(pObject->getFlags() & UIEL_FL_DRAW_LINKED_HIGHLIGHT_ONLY)
		{
			CGameUIObject* pLinkedObject = pObject->getLinkedObject();
			if(!pLinkedObject->isMouseOver())
				continue;
		}

		if(pObject->mouseWheelEvent(mouseX, mouseY, button, keyDown, scroll))
			return true;
	}

	return false;
}

//====================================
//
//====================================
bool CGameUIObject::mouseButtonEvent( Int32 mouseX, Int32 mouseY, Int32 button, bool keyDown )
{
	if(!m_isVisible)
		return false;

	if(!isInteractive())
		return false;

	if(m_pChildrenArray.empty())
		return false;

	for(Uint32 i = 0; i < m_pChildrenArray.size(); i++)
	{
		CGameUIObject* pObject = m_pChildrenArray[i];
		if(pObject->getFlags() & UIEL_FL_DRAW_LINKED_HIGHLIGHT_ONLY)
		{
			CGameUIObject* pLinkedObject = pObject->getLinkedObject();
			if(!pLinkedObject->isMouseOver())
				continue;
		}

		if(pObject->mouseButtonEvent(mouseX, mouseY, button, keyDown))
			return true;
	}

	return false;
}

//====================================
//
//====================================
bool CGameUIObject::keyEvent( Int32 button, Int16 mod, bool keyDown )
{
	if(!m_isVisible)
		return false;

	if(!isInteractive())
		return false;

	if(m_pChildrenArray.empty())
		return false;

	for(Uint32 i = 0; i < m_pChildrenArray.size(); i++)
	{
		CGameUIObject* pObject = m_pChildrenArray[i];
		if(pObject->getFlags() & UIEL_FL_DRAW_LINKED_HIGHLIGHT_ONLY)
		{
			CGameUIObject* pLinkedObject = pObject->getLinkedObject();
			if(!pLinkedObject->isMouseOver())
				continue;
		}

		if(pObject->keyEvent(button, mod, keyDown))
			return true;
	}

	return false;
}

//====================================
//
//====================================
void CGameUIObject::setLinkedObject( CGameUIObject* pObject )
{
	m_pLinkedObject = pObject;
}

//====================================
//
//====================================
CGameUIObject* CGameUIObject::getLinkedObject( void )
{
	return m_pLinkedObject;
}

//====================================
//
//====================================
void CGameUIObject::setButtonsDisabled( bool isDisabled )
{
	if(m_pChildrenArray.empty())
		return;

	for(Uint32 i = 0; i < m_pChildrenArray.size(); i++)
	{
		CGameUIObject* pObject = m_pChildrenArray[i];
		
		if(pObject->isButton())
			pObject->setDisabled(isDisabled);
		else
			pObject->setButtonsDisabled(isDisabled);

	}
}

//====================================
//
//====================================
void CGameUIObject::setColor( Uint32 r, Uint32 g, Uint32 b, Int32 a, bool recursive )
{
	m_color.r = _max(255, r);
	m_color.g = _max(255, g);
	m_color.b = _max(255, b);
	
	if(a != -1)
		m_color.a = _max(255, a);

	if(recursive)
	{
		// Set also the children
		for(Uint32 i = 0; i < this->m_pChildrenArray.size(); i++)
			m_pChildrenArray[i]->setColor(r, g, b, a, true);
	}
}

//====================================
//
//====================================
void CGameUIObject::setAlpha( Int32 alpha, bool recursive )
{
	m_color.a = clamp(alpha, 0, 255);

	if(recursive)
	{
		// Set also the children
		for(Uint32 i = 0; i < m_pChildrenArray.size(); i++)
			m_pChildrenArray[i]->setAlpha(alpha, true);
	}
}

//====================================
//
//====================================
void CGameUIObject::SetGameUIManager( CGameUIManager* pGUIManager )
{
	g_pGUIManager = pGUIManager;
}

//====================================
//
//====================================
CGameUITexturedObject::CGameUITexturedObject( Int32 flags, Uint32 width, Uint32 height, Int32 originx, Int32 originy ):
	CGameUIObject(flags, originx, originy, width, height),
	m_pDefaultTexture(nullptr),
	m_pFocusTexture(nullptr),
	m_pClickedTexture(nullptr),
	m_pDisabledTexture(nullptr),
	m_pHighlightedTexture(nullptr),
	m_pNewItemTexture(nullptr)
{
}

//====================================
//
//====================================
CGameUITexturedObject::~CGameUITexturedObject( void )
{
}

//=============================================
// @brief Draw function for elements
//
//=============================================
bool CGameUITexturedObject::draw( void )
{
	if(!isVisible())
		return true;

	// Only draw if it's a rendered element
	if(m_pDefaultTexture != nullptr && m_height > 0 && m_width > 0)
	{
		Int32 xpos = 0;
		Int32 ypos = 0;

		// Positions are relative to parent
		if(m_pParent)
			m_pParent->getAbsolutePosition(xpos, ypos);

		xpos += m_originX;
		ypos += m_originY;

		if(!cl_renderfuncs.pfnBasicDrawEnableTextures())
			return false;

		// Determine texture to use
		const en_texture_t* ptexture = nullptr;
		if(m_pDisabledTexture && (isDisabled() || (m_pParent && m_pParent->isDisabled())))
		{
			ptexture = m_pDisabledTexture;
		}
		else if(m_pClickedTexture && (isClickedOn() || (m_pParent && m_pParent->isClickedOn())))
		{
			ptexture = m_pClickedTexture;
		}
		else if(m_pHighlightedTexture && (isMouseOver() 
			|| (m_pParent && m_pParent->isMouseOver())  
			|| (getDrawFlags() & GUIEL_DF_MARK_HIGHLIGHT) 
			|| (m_pParent->getDrawFlags() & GUIEL_DF_MARK_HIGHLIGHT)))
		{
			ptexture = m_pHighlightedTexture;
		}
		else if(m_pNewItemTexture && ((getDrawFlags() & GUIEL_DF_MARK_NEW)
			|| (m_pParent->getDrawFlags() & GUIEL_DF_MARK_NEW)))
		{
			ptexture = m_pNewItemTexture;
		}
		else if(m_pFocusTexture)
		{
			CGameUIObject* pWindow = getParentWindow();

			if(pWindow && pWindow->isInFocus())
			{
				Int32 mX, mY;
				cl_engfuncs.pfnGetMousePosition(mX, mY);

				if(isMouseOver(mX, mY))
					ptexture = m_pFocusTexture;
			}
		}

		// Use default otherwise
		if(!ptexture)
			ptexture = m_pDefaultTexture;

		cl_renderfuncs.pfnBind2DTexture(GL_TEXTURE0_ARB, ptexture->palloc->gl_index, false);

		cl_renderfuncs.pfnBasicDrawColor4f(static_cast<Float>(m_color.r)/255.0f,
			static_cast<Float>(m_color.g)/255.0f, 
			static_cast<Float>(m_color.b)/255.0f, 
			static_cast<Float>(m_color.a)/255.0f);

		// Textures are tiled based on size, for now
		Float tcmax_x = static_cast<Float>(m_width) / static_cast<Float>(ptexture->width);
		Float tcmax_y = static_cast<Float>(m_height) / static_cast<Float>(ptexture->height);

		cl_renderfuncs.pfnValidateBasicDraw();

		cl_renderfuncs.pfnBasicDrawBegin(CBasicDraw::DRAW_TRIANGLES);
		cl_renderfuncs.pfnBasicDrawTexCoord2f(0, tcmax_y);
		cl_renderfuncs.pfnBasicDrawVertex3f(static_cast<Float>(xpos), static_cast<Float>(ypos) + static_cast<Float>(m_height), -1);

		cl_renderfuncs.pfnBasicDrawTexCoord2f(0, 0);
		cl_renderfuncs.pfnBasicDrawVertex3f(static_cast<Float>(xpos), static_cast<Float>(ypos), -1);

		cl_renderfuncs.pfnBasicDrawTexCoord2f(tcmax_x, 0);
		cl_renderfuncs.pfnBasicDrawVertex3f(static_cast<Float>(xpos)+static_cast<Float>(m_width), static_cast<Float>(ypos), -1);

		cl_renderfuncs.pfnBasicDrawTexCoord2f(0, tcmax_y);
		cl_renderfuncs.pfnBasicDrawVertex3f(static_cast<Float>(xpos), static_cast<Float>(ypos)+static_cast<Float>(m_height), -1);

		cl_renderfuncs.pfnBasicDrawTexCoord2f(tcmax_x, 0);
		cl_renderfuncs.pfnBasicDrawVertex3f(static_cast<Float>(xpos)+static_cast<Float>(m_width), static_cast<Float>(ypos), -1);

		cl_renderfuncs.pfnBasicDrawTexCoord2f(tcmax_x, tcmax_y);
		cl_renderfuncs.pfnBasicDrawVertex3f(static_cast<Float>(xpos)+static_cast<Float>(m_width), static_cast<Float>(ypos)+static_cast<Float>(m_height), -1);
		cl_renderfuncs.pfnBasicDrawEnd();
	}

	// Call base class to render children
	return CGameUIObject::draw();
}

//====================================
//
//====================================
CGameUISolidSurface::CGameUISolidSurface( Int32 flags, const color32_t& color, Int32 originX, Int32 originY, Uint32 width, Uint32 height ):
	CGameUIObject(flags, originX, originY, width, height)
{
	setColor(color.r, color.g, color.b, color.a);
}

//====================================
//
//====================================
CGameUISolidSurface::~CGameUISolidSurface( void )
{
}

//====================================
//
//====================================
bool CGameUISolidSurface::draw( void )
{
	Int32 baseOriginX, baseOriginY;
	getAbsolutePosition(baseOriginX, baseOriginY);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Draw background
	if(!cl_renderfuncs.pfnBasicDrawDisableTextures())
		return false;

	cl_renderfuncs.pfnBasicDrawColor4f(static_cast<Float>(m_color.r)/255.0f,
		static_cast<Float>(m_color.g)/255.0f, 
		static_cast<Float>(m_color.b)/255.0f, 
		static_cast<Float>(m_color.a)/255.0f);

	Int32 xpos = 0;
	Int32 ypos = 0;

	// Positions are relative to parent
	if(m_pParent)
		m_pParent->getAbsolutePosition(xpos, ypos);

	xpos += m_originX;
	ypos += m_originY;

	cl_renderfuncs.pfnValidateBasicDraw();

	cl_renderfuncs.pfnBasicDrawBegin(CBasicDraw::DRAW_TRIANGLES);
	cl_renderfuncs.pfnBasicDrawVertex3f(static_cast<Float>(xpos), static_cast<Float>(ypos) + static_cast<Float>(m_height), -1);
	cl_renderfuncs.pfnBasicDrawVertex3f(static_cast<Float>(xpos), static_cast<Float>(ypos), -1);
	cl_renderfuncs.pfnBasicDrawVertex3f(static_cast<Float>(xpos)+static_cast<Float>(m_width), static_cast<Float>(ypos), -1);

	cl_renderfuncs.pfnBasicDrawVertex3f(static_cast<Float>(xpos), static_cast<Float>(ypos)+static_cast<Float>(m_height), -1);
	cl_renderfuncs.pfnBasicDrawVertex3f(static_cast<Float>(xpos)+static_cast<Float>(m_width), static_cast<Float>(ypos), -1);
	cl_renderfuncs.pfnBasicDrawVertex3f(static_cast<Float>(xpos)+static_cast<Float>(m_width), static_cast<Float>(ypos)+static_cast<Float>(m_height), -1);
	cl_renderfuncs.pfnBasicDrawEnd();

	// Call base class to render
	return CGameUIObject::draw();
}

//=============================================
// @brief Constructor
//
//=============================================
CGameUISurface::CGameUISurface( Int32 flags, Int32 originx, Int32 originy, Uint32 width, Uint32 height ):
	CGameUIObject( flags, originx, originy, width, height ),
	m_pBackground(nullptr),
	m_pBtmLeftCorner(nullptr),
	m_pBtmRightCorner(nullptr),
	m_pTopLeftCorner(nullptr),
	m_pTopRightCorner(nullptr),
	m_pLeftBorder(nullptr),
	m_pRightBorder(nullptr),
	m_pBottomBorder(nullptr),
	m_pTopBorder(nullptr),
	m_pScheme(nullptr)
{
}

//=============================================
// @brief Destructor
//
//=============================================
CGameUISurface::~CGameUISurface( void )
{
}

//=============================================
// @brief Creates a UI element from a schema object
//
//=============================================
CGameUITexturedObject* CGameUISurface::createObject( const ui_schemeinfo_t* pscheme, const Char* objectName )
{
	// Get it from the schema
	const ui_schemeobject_t* pobj = pscheme->getObject(objectName);
	if(!pobj)
	{
		cl_engfuncs.pfnCon_Printf("Error: Schema '%s' is missing the '%s' definition!\n", pscheme->schemeName.c_str(), objectName);
		return nullptr;
	}

	// Allocate the new element
	CGameUITexturedObject* pNew = new CGameUITexturedObject(UIEL_FL_NONE, pobj->width, pobj->height, 0, 0);

	// Set properties
	pNew->setDefaultTexture(pobj->defaultTexture);
	pNew->setFocusTexture(pobj->focusTexture);
	pNew->setClickedTexture(pobj->clickTexture);
	pNew->setDisabledTexture(pobj->disabledTexture);
	pNew->setHighlightedTexture(pobj->highlightTexture);
	pNew->setNewItemTexture(pobj->newItemTexture);
	pNew->setParent(this);

	return pNew;
}

//=============================================
// @brief Initializes the tab element with a schema
//
//=============================================
bool CGameUISurface::initSchema( const Char* pstrSchemaName )
{
	m_pScheme = g_pGUIManager->LoadSchemaFile(pstrSchemaName);
	if(!m_pScheme)
		return false;

	// Set background element
	m_pBackground = createObject(m_pScheme, "Body");
	if(!m_pBackground)
		return false;

	// Set bottom left corner element
	m_pLeftBorder = createObject(m_pScheme, "Left");
	if(!m_pLeftBorder)
		return false;

	// Set bottom right corner element
	m_pRightBorder = createObject(m_pScheme, "Right");
	if(!m_pRightBorder)
		return false;

	if(!(m_flags & UIEL_FL_NO_BOTTOM_BORDER))
	{
		// Init bottom elements
		if(!initBottomElements())
			return false;
	}

	// Init top elements
	if(!initTopElements())
		return false;

	return true;
}

//=============================================
// @brief Sets the bottom border elements
//
//=============================================
bool CGameUISurface::initBottomElements( void )
{
	// Set top left corner element
	m_pBottomBorder = createObject(m_pScheme, "Bottom");
	if(!m_pBottomBorder)
		return false;

	// Set bottom left corner element
	m_pBtmLeftCorner = createObject(m_pScheme, "BottomLeft");
	if(!m_pBtmLeftCorner)
		return false;

	// Set bottom right corner element
	m_pBtmRightCorner = createObject(m_pScheme, "BottomRight");
	if(!m_pBtmRightCorner)
		return false;

	return true;
}

//=============================================
// @brief Sets the bottom border elements
//
//=============================================
bool CGameUISurface::initTopElements( void )
{
	// Set top right corner element
	m_pTopBorder = createObject(m_pScheme, "Top");
	if(!m_pTopBorder)
		return false;

	// Set top left corner element
	m_pTopLeftCorner = createObject(m_pScheme, "TopLeft");
	if(!m_pTopLeftCorner)
		return false;

	// Set top right corner element
	m_pTopRightCorner = createObject(m_pScheme, "TopRight");
	if(!m_pTopRightCorner)
		return false;

	// Call to finalize
	adjustPosition();

	return true;
}

//=============================================
// @brief Sets the alpha value of the UI element
//
//=============================================
void CGameUISurface::setAlpha( Int32 alpha, bool recursive )
{
	// Call base class to manage basics
	CGameUIObject::setAlpha(alpha, recursive);

	// If not recursive, we need to set for the base elements
	if(!recursive)
	{
		m_pBackground->setAlpha(m_color.a);
		if(m_pTopLeftCorner)
			m_pTopLeftCorner->setAlpha(m_color.a);

		if(m_pTopRightCorner)
			m_pTopRightCorner->setAlpha(m_color.a);

		m_pLeftBorder->setAlpha(m_color.a);
		m_pRightBorder->setAlpha(m_color.a);

		if(m_pTopBorder)
			m_pTopBorder->setAlpha(m_color.a);

		if(m_pBtmLeftCorner)
			m_pBtmLeftCorner->setAlpha(m_color.a);
		if(m_pBtmRightCorner)
			m_pBtmRightCorner->setAlpha(m_color.a);
		if(m_pBottomBorder)
			m_pBottomBorder->setAlpha(m_color.a);
	}
}

//=============================================
// @brief Repositions the object after a parent's size is changed
//
//=============================================
void CGameUISurface::adjustPosition( void )
{
	CGameUIObject::adjustPosition();
	adjustBgElements();
}

//=============================================
// @brief Adjusts the top border element
//
//=============================================
void CGameUISurface::adjustTopBorder( void )
{
	if(!m_pTopLeftCorner && !m_pTopBorder
		&& !m_pTopRightCorner)
		return;

	// Set the top left corner element
	Int32 originX = 0;
	Int32 originY = 0;
	if(m_pTopLeftCorner)
		m_pTopLeftCorner->setPosition(originX, originY);

	// Set the top border element
	originY = 0;
	if(m_pTopLeftCorner)
		originX = m_pTopLeftCorner->getWidth();
	else
		originY = 0;

	if(m_pTopBorder)
	{
		Int32 width = m_width;
		if(m_pTopLeftCorner)
			width -= m_pTopLeftCorner->getWidth();
		if(m_pTopRightCorner)
			width -= m_pTopRightCorner->getWidth();

		m_pTopBorder->setWidth(width);
		m_pTopBorder->setPosition(originX, originY);
	}

	// Set the top right corner element
	if(m_pTopRightCorner)
	{
		originY = 0;
		originX = m_width - m_pTopRightCorner->getWidth();
		m_pTopRightCorner->setPosition(originX, originY);
	}
}

//=============================================
// @brief Adjusts the background elements
//
//=============================================
void CGameUISurface::adjustBgElements( void )
{
	if(!m_pBackground || !m_pLeftBorder
		|| !m_pRightBorder)
		return;

	// Done in a separate function to support tabs
	adjustTopBorder();

	// Set the right border
	Int32 originX = 0;
	Int32 originY;
	if(m_pTopLeftCorner)
		originY = m_pTopLeftCorner->getHeight();
	else
		originY = 0;

	m_pRightBorder->setPosition(originX, originY);

	Int32 height = m_height;
	if(m_pTopLeftCorner)
		height -= m_pTopLeftCorner->getHeight();

	if(m_pBtmLeftCorner)
		height -= m_pBtmLeftCorner->getHeight();
	m_pRightBorder->setHeight(height);

	if(m_pBtmLeftCorner)
	{
		// Set the right bottom corner
		originX = 0;
		originY = m_height - m_pBtmLeftCorner->getHeight();
		m_pBtmLeftCorner->setPosition(originX, originY);
	}

	Uint32 width = 0;
	if(m_pBottomBorder && m_pBtmLeftCorner && m_pBtmRightCorner)
	{
		// Set the bottom border
		originX = m_pBtmLeftCorner->getWidth();
		originY = m_height - m_pBottomBorder->getHeight();
		m_pBottomBorder->setPosition(originX, originY);

		width = m_width - m_pBtmLeftCorner->getWidth();
		width -= m_pBtmRightCorner->getWidth();
		m_pBottomBorder->setWidth(width);
	}

	if(m_pBtmRightCorner)
	{
		// Set the bottom right corner
		originX = m_width - m_pBtmRightCorner->getWidth();
		originY = m_height - m_pBtmRightCorner->getHeight();
		m_pBtmRightCorner->setPosition(originX, originY);
	}

	// Set the left border
	originX = m_width - m_pRightBorder->getWidth();
	if(m_pTopRightCorner)
		originY = m_pTopRightCorner->getHeight();
	else
		originY = 0;

	m_pLeftBorder->setPosition(originX, originY);

	if(m_pTopRightCorner)
		height = m_height - m_pTopRightCorner->getHeight();
	else
		height = m_height;

	if(m_pBtmLeftCorner)
		height -= m_pBtmLeftCorner->getHeight();
	m_pLeftBorder->setHeight(height);

	// Set background
	originX = m_pLeftBorder->getWidth();
	if(m_pTopBorder)
		originY = m_pTopBorder->getHeight();
	else
		originY = 0;

	width = m_width - originX;
	width -= m_pRightBorder->getWidth();

	height = m_height - originY;
	if(m_pBottomBorder)
		height -= m_pBottomBorder->getHeight();

	m_pBackground->setPosition(originX, originY);
	m_pBackground->setSize(width, height);
}

//=============================================
// @brief Draw function
//
//=============================================
bool CGameUISurface::draw( void )
{
	if(!m_isVisible)
		return true;

	// Call base class to draw children
	return CGameUIObject::draw();
}

//====================================
//
//====================================
CGameUIButton::CGameUIButton( Int32 flags, CGameUICallbackEvent* pEvent, Int32 originX, Int32 originY, Uint32 width, Uint32 height ):
	CGameUISurface(flags, originX, originY, width, height),
	m_isClickedOn(false),
	m_isDisabled(false),
	m_isHighlighted(false),
	m_pEvent(pEvent),
	m_pDisplayObject(nullptr)
{
}

//====================================
//
//====================================
CGameUIButton::CGameUIButton( Int32 flags, CGameUICallbackEvent* pEvent, SDL_Keycode keycode, Int32 originX, Int32 originY, Uint32 width, Uint32 height ):
	CGameUISurface(flags, originX, originY, width, height),
	m_isClickedOn(false),
	m_isDisabled(false),
	m_isHighlighted(false),
	m_pEvent(pEvent),
	m_pDisplayObject(nullptr)
{
	m_buttonKeysArray.push_back(keycode);
}

//====================================
//
//====================================
CGameUIButton::CGameUIButton( Int32 flags, CGameUICallbackEvent* pEvent, const CArray<SDL_Keycode>& keycodesArray, Int32 originX, Int32 originY, Uint32 width, Uint32 height ):
	CGameUISurface(flags, originX, originY, width, height),
	m_isClickedOn(false),
	m_isDisabled(false),
	m_isHighlighted(false),
	m_pEvent(pEvent),
	m_buttonKeysArray(keycodesArray),
	m_pDisplayObject(nullptr)
{
}

//====================================
//
//====================================
CGameUIButton::~CGameUIButton( void )
{
	if(m_pEvent)
	{
		delete m_pEvent;
		m_pEvent = nullptr;
	}
}

//====================================
//
//====================================
bool CGameUIButton::mouseButtonEvent( Int32 mouseX, Int32 mouseY, Int32 button, bool keyDown )
{
	// Only left mouse click affects draggers
	if(button != SDL_BUTTON_LEFT)
		return false;

	if(m_isDisabled)
		return false;

	if(isMouseOver(mouseX, mouseY))
	{
		if(keyDown)
		{
			if(!m_isClickedOn)
			{
				m_isClickedOn = true;
			}
		}
		else if(m_isClickedOn)
		{
			// Peform the action
			if(m_pEvent)
				m_pEvent->PerformAction(0);

			m_isClickedOn = false;
		}

		return true;
	}

	return false;
}

//====================================
//
//====================================
bool CGameUIButton::keyEvent( Int32 button, Int16 mod, bool keyDown )
{
	if(m_buttonKeysArray.empty())
		return false;

	for(Uint32 i = 0; i < m_buttonKeysArray.size(); i++)
	{
		SDL_Keycode buttonKey = m_buttonKeysArray[i];
		if(buttonKey == SDLK_UNKNOWN)
			return false;

		if(m_isDisabled)
			return false;

		SDL_Keycode sdlKeycode = SDL_GetKeyFromScancode((SDL_Scancode)button);
		if(buttonKey == sdlKeycode && keyDown)
		{
			m_isClickedOn = true;

			if(m_pEvent)
				m_pEvent->PerformAction(0);

			break;
		}
	}

	return false;
}

//====================================
//
//====================================
void CGameUIButton::think( void )
{
	if(m_isClickedOn)
	{
		if(!isMouseOver())
			m_isClickedOn = false;
	}

	// Call base to handle the rest
	CGameUIObject::think();
}

//====================================
//
//====================================
void CGameUIButton::setText( const Char* pstrText )
{
	if(m_pDisplayObject)
	{
		removeChild(m_pDisplayObject);
		delete m_pDisplayObject;
	}

	m_pDisplayObject = new CGameUIText(UIEL_FL_ALIGN_CV|UIEL_FL_ALIGN_CH, m_color, nullptr, pstrText, 0, 0);
	m_pDisplayObject->setParent(this);
}

//====================================
//
//====================================
void CGameUIButton::setSchemaObject( const ui_schemeobject_t* pScheme )
{
	if(m_pDisplayObject)
	{
		removeChild(m_pDisplayObject);
		delete m_pDisplayObject;
	}

	CGameUITexturedObject* pDisplayObject = new CGameUITexturedObject((UIEL_FL_ALIGN_CH|UIEL_FL_ALIGN_CV|UIEL_FL_ONTOP), pScheme->width, pScheme->height, 0, 0);
	pDisplayObject->setParent(this);
	m_pDisplayObject = pDisplayObject;

	pDisplayObject->setDefaultTexture(pScheme->defaultTexture);
	pDisplayObject->setFocusTexture(pScheme->focusTexture);
	pDisplayObject->setClickedTexture(pScheme->clickTexture);
}

//====================================
//
//====================================
void CGameUIButton::setDisabled( bool isDisabled )
{
	m_isDisabled = isDisabled;
}

//====================================
//
//====================================
bool CGameUIButton::isDisabled( void )
{
	return m_isDisabled;
}

//====================================
//
//====================================
CGameUIText::CGameUIText( Int32 flags, const color32_t& color, const font_set_t* pfontset, const Char* pstrText, Int32 originX, Int32 originY ):
	CGameUIObject(flags, originX, originY, 0, 0),
	m_color(color),
	m_text(pstrText),
	m_pFontSet(pfontset),
	m_textInset(0),
	m_maxWidth(0),
	m_maxHeight(0)
{
	if(!m_pFontSet)
		m_pFontSet = gGameUIManager.GetDefaultFontSet();

	// Call to adjust size
	adjustSize();
}

//====================================
//
//====================================
CGameUIText::CGameUIText( Int32 flags, const color32_t& color, const font_set_t* pfontset, Int32 originX, Int32 originY ):
	CGameUIObject(flags, originX, originY, 0, 0),
	m_color(color),
	m_pFontSet(pfontset),
	m_textInset(0),
	m_maxWidth(0),
	m_maxHeight(0)
{
	if(!m_pFontSet)
		m_pFontSet = gGameUIManager.GetDefaultFontSet();
}

//====================================
//
//====================================
CGameUIText::CGameUIText( Int32 flags, const color32_t& color, const font_set_t* pfontset, const Char* pstrText, Int32 originX, Int32 originY, Uint32 maxwidth, Uint32 maxheight, Uint32 textInset ):
	CGameUIObject(flags, originX, originY, 0, 0),
	m_color(color),
	m_text(pstrText),
	m_pFontSet(pfontset),
	m_textInset(textInset),
	m_maxWidth(maxwidth),
	m_maxHeight(maxheight)
{
	if(!m_pFontSet)
		m_pFontSet = gGameUIManager.GetDefaultFontSet();

	// Call to adjust size
	adjustSize();
}

//====================================
//
//====================================
CGameUIText::~CGameUIText( void )
{
}

//====================================
//
//====================================
bool CGameUIText::draw( void )
{
	// Not happy about so many state switches
	cl_renderfuncs.pfnDisableBasicDraw();

	Int32 xpos, ypos;
	getAbsolutePosition(xpos, ypos);

	if(!m_maxWidth || !m_maxHeight)
	{
		Uint32 height = cl_renderfuncs.pfnEstimateStringHeight(m_pFontSet, m_text.c_str(), 0);
		if(!cl_renderfuncs.pfnDrawSimpleString(m_color, xpos, ypos+height, m_text.c_str(), m_pFontSet))
			return false;
	}
	else
	{
		if(!cl_renderfuncs.pfnDrawStringBox(0, 0, 
			m_maxWidth, m_maxHeight, 
			m_textInset, m_textInset, 
			false, 
			m_color, 
			xpos, ypos, 
			m_text.c_str(),
			m_pFontSet,
			0, 
			m_pFontSet->fontsize,
			0))
			return false;
	}

	// Re-enable basic drawing
	if(!cl_renderfuncs.pfnEnableBasicDraw())
		return false;

	// Re-enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Call base to handle the rest
	return CGameUIObject::draw();
}

//====================================
//
//====================================
void CGameUIText::setText( const Char* pstrText )
{
	m_text = pstrText;
	adjustSize();
}

//====================================
//
//====================================
void CGameUIText::setFontSet( const font_set_t* pFontSet )
{
	m_pFontSet = pFontSet;
	adjustSize();
}

//====================================
//
//====================================
void CGameUIText::setColor( const color32_t& fontcolor )
{
	m_color = fontcolor;
}

//====================================
//
//====================================
void CGameUIText::adjustSize( void )
{
	// Calculate string size
	Uint32 stringwidth = 0;
	Uint32 stringheight = 0;
	
	const Char* pstr = m_text.c_str();
	while(*pstr != '\0')
	{
		stringwidth += m_pFontSet->glyphs[*pstr].advancex;
		if(stringheight < m_pFontSet->glyphs[*pstr].height)
			stringheight = m_pFontSet->glyphs[*pstr].height;

		pstr++;
	}

	// Assign final size
	m_width = stringwidth;
	m_height = stringheight;

	// Adjust position relative to parent
	adjustPosition();
}

//====================================
//
//====================================
CGameUITextTab::CGameUITextTab( Int32 flags, const font_set_t* pfontset, Uint32 textinset, const color32_t& textcolor, Int32 originX, Int32 originY, Uint32 width, Uint32 height ):
	CGameUISurface(flags, originX, originY, width, height),
	m_textInset(textinset),
	m_pFontSet(pfontset),
	m_textOffset(0),
	m_textColor(textcolor),
	m_pScroller(nullptr)
{
	if(!m_pFontSet)
		m_pFontSet = gGameUIManager.GetDefaultFontSet();
}

//====================================
//
//====================================
CGameUITextTab::~CGameUITextTab( void )
{
}

//====================================
//
//====================================
bool CGameUITextTab::draw( void )
{
	// Call base class to render
	if(!CGameUISurface::draw())
		return false;

	if(!m_displayText.empty())
	{
		// Not happy about so many state switches
		cl_renderfuncs.pfnDisableBasicDraw();

		Int32 originx, originy;
		getAbsolutePosition(originx, originy);

		// Set drawing rectangle
		if(!cl_renderfuncs.pfnDrawStringBox(0, 0, 
			m_width - m_pScroller->getWidth(), m_height, 
			m_textInset, m_textInset, 
			false, 
			m_textColor, 
			originx, originy, 
			m_displayText.c_str(), 
			m_pFontSet, 
			m_textOffset, 
			m_pFontSet->fontsize, 
			0))
			return false;

		// Reset HUD renderer
		if(!cl_renderfuncs.pfnEnableBasicDraw())
			return false;

		// Re-enable blending
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	return true;
}

//====================================
//
//====================================
bool CGameUITextTab::initSchema( const Char* pstrSchemaName, const Char* pstrButtonSchemaName )
{
	if(!CGameUISurface::initSchema(pstrSchemaName))
		return false;

	m_pScroller = new CGameUIScroller(UIEL_FL_NONE, UIEL_SCROLL_V, m_pFontSet->fontsize, 16, m_height, 0, 0);
	m_pScroller->setParent(this);

	if(!m_pScroller->initSchema(pstrButtonSchemaName))
	{
		cl_engfuncs.pfnCon_EPrintf("Could not init schema '%s' for 'CGameUIScroller'.\n", pstrButtonSchemaName);
		return false;
	}

	return true;
}

//====================================
//
//====================================
void CGameUITextTab::initData( const byte* pdata, Uint32 datasize )
{
	// Set the text to display
	m_displayText.assign(reinterpret_cast<const Char*>(pdata), datasize);
	updateRangeSize();
	
	// Reset to beginning
	m_pScroller->resetScrollerPosition();
}

//====================================
//
//====================================
void CGameUITextTab::setFontSet( const font_set_t* pfontset )
{
	m_pFontSet = pfontset;
	m_pScroller->setUnitSize(m_pFontSet->fontsize);
}

//====================================
//
//====================================
void CGameUITextTab::setTextColor( const color32_t& textcolor )
{
	m_textColor = textcolor;
}

//=============================================
// @brief Sets the offset value for the element
//
//=============================================
void CGameUITextTab::setOffsetValue( Float offset )
{
	if(m_displayText.empty())
		return;

	cl_renderfuncs.pfnSetStringRectangle(0, 0, m_width - m_pScroller->getWidth(), m_height, m_textInset, m_textInset);
	Float textHeight = cl_renderfuncs.pfnEstimateStringHeight(m_pFontSet, m_displayText.c_str(), m_pFontSet->fontsize);
	cl_renderfuncs.pfnSetStringRectangle(0, 0, 0, 0, 0, 0);

	if(textHeight < m_height)
	{
		m_textOffset = 0;
		return;
	}

	Uint32 totalHeight = textHeight-(m_height-m_pFontSet->fontsize-m_textInset*2);
	Uint32 nbElements = totalHeight/m_pFontSet->fontsize;
	m_textOffset = nbElements*offset;
}

//=============================================
// @brief Sets the source text array to render text from
//
//=============================================
void CGameUITextTab::updateRangeSize( void )
{
	if(m_displayText.empty())
	{
		m_pScroller->setFullRange(0);
		return;
	}

	// Re-estimate the height
	cl_renderfuncs.pfnSetStringRectangle(0, 0, m_width - m_pScroller->getWidth(), m_height, m_textInset, m_textInset);
	Int32 textHeight = cl_renderfuncs.pfnEstimateStringHeight(m_pFontSet, m_displayText.c_str(), m_pFontSet->fontsize);
	m_pScroller->setFullRange(textHeight);

	// Reset rectangle
	cl_renderfuncs.pfnSetStringRectangle(0, 0, 0, 0, 0, 0);
}

//=============================================
// @brief Sets the source text array to render text from
//
//=============================================
bool CGameUITextTab::mouseWheelEvent( Int32 mouseX, Int32 mouseY, Int32 button, bool keyDown, Int32 scroll )
{
	if(keyDown && isVisible() && m_pScroller->moveScroller(button, scroll))
		return true;
	else
		return false;
}

//=============================================
// @brief Sets the source text array to render text from
//
//=============================================
bool CGameUITextTab::keyEvent( Int32 button, Int16 mod, bool keyDown )
{
	if(!keyDown)
		return false;

	// Get SDL Keycode
	SDL_Keycode sdlKeycode = SDL_GetKeyFromScancode((SDL_Scancode)button);

	switch(sdlKeycode)
	{
	case SDLK_DOWN:
		m_pScroller->moveScroller(button, 1);
		return true;
	case SDLK_UP:
		m_pScroller->moveScroller(button, -1);
		return true;
	default:
		return false;
	}
}

//====================================
//
//====================================
void CGameUITextTab::think( void )
{
	// Call base class to manage this
	CGameUISurface::think();

	// Set us as the input focus if the mouse is over us
	if(isMouseOver())
		setInputFocusObject(this);
}

//====================================
//
//====================================
CGameUIWindow::CGameUIWindow( Int32 flags, Int32 originX, Int32 originY, Uint32 width, Uint32 height ):
	CGameUISurface(FL_NONE, originX, originY, width, height),
	m_windowFlags(flags),
	m_pInputFocusObject(nullptr),
	m_windowRemoveTime(0),
	m_isInFocus(false)
{
}

//====================================
//
//====================================
CGameUIWindow::~CGameUIWindow( void )
{
}

//====================================
//
//====================================
void CGameUIWindow::setInputFocusObject( CGameUIObject* pObject )
{
	if(m_pInputFocusObject)
		m_pInputFocusObject->setIsInputFocusObject(false);

	m_pInputFocusObject = pObject;
	m_pInputFocusObject->setIsInputFocusObject(true);
}

//====================================
//
//====================================
bool CGameUIWindow::mouseWheelEvent( Int32 mouseX, Int32 mouseY, Int32 button, bool keyDown, Int32 scroll )
{
	if(!m_isVisible)
		return false;

	if(!isInteractive())
		return false;

	if(m_pChildrenArray.empty())
		return false;

	// Allow input focus object to take any inputs first
	if(m_pInputFocusObject && m_pInputFocusObject->isVisible() 
		&& m_pInputFocusObject->mouseWheelEvent(mouseX, mouseY, button, keyDown, scroll))
		return true;

	// If not, allow others to take the input
	for(Uint32 i = 0; i < m_pChildrenArray.size(); i++)
	{
		if(m_pChildrenArray[i]->mouseWheelEvent(mouseX, mouseY, button, keyDown, scroll))
			return true;
	}

	return false;
}

//====================================
//
//====================================
bool CGameUIWindow::mouseButtonEvent( Int32 mouseX, Int32 mouseY, Int32 button, bool keyDown )
{
	if(!m_isVisible)
		return false;

	if(!isInteractive())
		return false;

	if(m_pChildrenArray.empty())
		return false;

	// Allow input focus object to take any inputs first
	if(m_pInputFocusObject && m_pInputFocusObject->mouseButtonEvent(mouseX, mouseY, button, keyDown))
		return true;

	// If not, allow others to take the input
	for(Uint32 i = 0; i < m_pChildrenArray.size(); i++)
	{
		if(m_pChildrenArray[i]->mouseButtonEvent(mouseX, mouseY, button, keyDown))
			return true;
	}

	return false;
}

//====================================
//
//====================================
bool CGameUIWindow::keyEvent( Int32 button, Int16 mod, bool keyDown )
{
	if(!m_isVisible)
		return false;

	if(!isInteractive())
		return false;

	if(m_pChildrenArray.empty())
		return false;

	// Allow input focus object to take any inputs first
	if(m_pInputFocusObject && m_pInputFocusObject->keyEvent(button, mod, keyDown))
		return true;

	// If not, allow others to take the input
	for(Uint32 i = 0; i < m_pChildrenArray.size(); i++)
	{
		if(m_pChildrenArray[i]->keyEvent(button, mod, keyDown))
			return true;
	}

	return false;
}

//====================================
//
//====================================
void CGameUIWindow::setDelayedRemoval( Double delay )
{
	if(!(m_windowFlags & FL_WINDOW_WAIT_TILL_NEXT))
	{
		m_windowRemoveTime = cl_engfuncs.pfnGetClientTime() + delay;
		m_windowFlags |= FL_WINDOW_DELAY_REMOVE;
	}

	setButtonsDisabled(true);
}

//====================================
//
//====================================
bool CGameUIWindow::isInFocus( void )
{
	return m_isInFocus;
}

//====================================
//
//====================================
void CGameUIWindow::setIsInFocus( bool inFocus )
{
	m_isInFocus = inFocus;
}

//=============================================
// @brief Constructor
//
//=============================================
CGameUIDragButton::CGameUIDragButton( Int32 flags, ui_scroller_align_t alignment, CGameUICallbackEvent* pEvent, Uint32 width, Uint32 height, Int32 originx, Int32 originy ):
	CGameUIButton( flags, pEvent, originx, originy, width, height ),
	m_alignment(alignment),
	m_lastMouseX(0),
	m_lastMouseY(0),
	m_startInset(0),
	m_endInset(0),
	m_position(0),
	m_lastParentLength(0)
{
	if(alignment == UIEL_SCROLL_V)
		m_flags |= UIEL_FL_ALIGN_R;
	else
		m_flags |= UIEL_FL_ALIGN_B;
}

//=============================================
// @brief Destructor
//
//=============================================
CGameUIDragButton::~CGameUIDragButton( void )
{
}

//=============================================
// @brief Sets the string to display
//
//=============================================
void CGameUIDragButton::setBounds( Int32 start, Int32 end )
{
	m_startInset = start;
	m_endInset = end;
}

//=============================================
// @brief Sets the string to display
//
//=============================================
void CGameUIDragButton::getBounds( Int32& start, Int32& end )
{
	start = m_startInset;
	end = m_endInset;
}

//=============================================
// @brief Sets the string to display
//
//=============================================
bool CGameUIDragButton::mouseButtonEvent( Int32 mouseX, Int32 mouseY, Int32 button, bool keyDown )
{
	// Only left mouse click affects draggers
	if(button != SDL_BUTTON_LEFT)
		return false;

	if(isMouseOver(mouseX, mouseY))
	{
		if(keyDown)
		{
			if(!m_isClickedOn)
			{
				m_lastMouseX = mouseX;
				m_lastMouseY = mouseY;
				m_isClickedOn = true;
			}

			return true;
		}
	}

	if(m_isClickedOn && !keyDown)
	{
		// Peform the action
		if(m_pEvent)
			m_pEvent->PerformAction(0);

		m_isClickedOn = false;
		m_lastMouseX = 0;
		m_lastMouseY = 0;
	}

	return false;
}

//=============================================
// @brief Performs think functions
//
//=============================================
void CGameUIDragButton::think( void )
{
	if(m_isClickedOn)
	{
		Int32 xPos, yPos;
		cl_engfuncs.pfnGetMousePosition(xPos, yPos);

		Int32 prevVal;
		Int32 curVal;
		if(m_alignment == UIEL_SCROLL_V)
		{
			prevVal = m_lastMouseY;
			curVal = yPos;
		}
		else
		{
			prevVal = m_lastMouseX;
			curVal = xPos;
		}

		// Peform the action
		if(curVal != prevVal)
		{
			Int32 delta = curVal - prevVal;
			if(adjPosition(delta, true))
			{
				m_lastMouseX = xPos;
				m_lastMouseY = yPos;
			}

			if(m_pEvent)
				m_pEvent->PerformAction(0);
		}
	}

	// Call base to handle the rest
	CGameUIObject::think();
}

//=============================================
// @brief Sets the length of the drag button
//
//=============================================
void CGameUIDragButton::setLength( Uint32 length )
{
	if(m_alignment == UIEL_SCROLL_V)
	{
		Int32 finalHeight = length - m_startInset - m_endInset;
		m_height = _min(0, finalHeight);
	}
	else
	{
		Int32 finalWidth = length - m_startInset - m_endInset;
		m_width = _min(0, finalWidth);
	}
}

//=============================================
// @brief Sets the position of the drag button
//
//=============================================
void CGameUIDragButton::setPosition( Float position )
{
	m_position = clamp(position, 0.0, 1.0);
}

//=============================================
// @brief Gets the position of the drag button
//
//=============================================
Float CGameUIDragButton::getPosition( void )
{
	return m_position;
}

//=============================================
// @brief Sets the position of the drag button
//
//=============================================
bool CGameUIDragButton::adjPosition( Int32 adjAmt, bool isMouseDrag, bool callEvent )
{
	if(m_position == 1.0 && adjAmt > 0)
		return false;
	else if(m_position == 0.0 && adjAmt < 0)
		return false;

	// Get parent length
	Uint32 parentLength;
	if(m_alignment == UIEL_SCROLL_V)
		parentLength = m_pParent->getHeight();
	else
		parentLength = m_pParent->getWidth();

	// Determine my own range
	Int32 myRange = parentLength-getLength()-m_startInset-m_endInset;
	
	// No range, just set to base
	if(!myRange)
	{
		// Set appropriate position
		if(m_alignment == UIEL_SCROLL_V)
			m_originY = m_startInset;
		else
			m_originX = m_startInset;

		return true;
	}

	Uint32 referenceRange;
	if(isMouseDrag)
		referenceRange = myRange;
	else
		referenceRange = m_pParent->getFullRange();

	// Make adjustments
	Float adjFrac = (Float)adjAmt/(Float)(referenceRange);

	m_position += adjFrac;
	m_position = clamp(m_position, 0.0, 1.0);

	// Set appropriate position
	if(m_alignment == UIEL_SCROLL_V)
		m_originY = m_startInset + m_position*myRange;
	else
		m_originX = m_startInset + m_position*myRange;

	// Call the action to reset the position
	if(callEvent && m_pEvent)
		m_pEvent->PerformAction(0);

	// Remember last parent length
	m_lastParentLength = parentLength;
	return true;
}

//=============================================
// @brief Gets the length of the drag button
//
//=============================================
Uint32 CGameUIDragButton::getLength( void )
{
	if(m_alignment == UIEL_SCROLL_V)
		return m_height;
	else
		return m_width;
}

//=============================================
// @brief Repositions the object after a parent's size is changed
//
//=============================================
void CGameUIDragButton::adjustPosition( void )
{
	// Call base class first
	CGameUIButton::adjustPosition();

	if(m_lastParentLength)
	{
		// Reset the position
		Uint32 parentLength;
		if(m_alignment == UIEL_SCROLL_V)
			parentLength = m_pParent->getHeight();
		else
			parentLength = m_pParent->getWidth();

		// Determine previous full position
		Uint32 prevRelativePosition = m_position*m_lastParentLength;
		m_lastParentLength = parentLength;

		// Determine current position based on this
		Float currentPosition = (Float)prevRelativePosition/(Float)parentLength;
		m_position = clamp(currentPosition, 0.0, 1.0);

		// Determine my own range
		Int32 myRange = parentLength-getLength()-m_startInset-m_endInset;

		// Set appropriate position
		Int32 offAdj = m_position*myRange;
		if(!offAdj)
			m_position = 0;

		if(m_alignment == UIEL_SCROLL_V)
			m_originY = m_startInset + offAdj;
		else
			m_originX = m_startInset + offAdj;

		// Call the action to reset the position
		if(m_pEvent)
			m_pEvent->PerformAction(0);
	}
}

//=============================================
// @brief Performs the close action for the window
//
//=============================================
void CGameUIScroller::CGameUIScrollerArrowBtnAction::PerformAction( Float param )
{
	assert(m_pScroller != nullptr);

	// Get the dragger button
	CGameUIDragButton* pButton = m_pScroller->getDragButton();
	Int32 adjAmt = m_jumpSize * m_direction;

	pButton->adjPosition(adjAmt, false);
}


//=============================================
// @brief Performs the close action for the window
//
//=============================================
void CGameUIScroller::CGameUIScrollerDragBtnAction::PerformAction( Float param )
{
	assert(m_pScroller != nullptr);

	// Get the dragger button
	CGameUIDragButton* pButton = m_pScroller->getDragButton();
	Float position = pButton->getPosition();

	// Get parent of the scroller button
	CGameUIObject* pParent = m_pScroller->getParent();
	if(pParent)
		pParent->setOffsetValue(position);
}

//=============================================
// @brief Constructor
//
//=============================================
CGameUIScroller::CGameUIScroller( Int32 flags, ui_scroller_align_t alignment, Uint32 unitSize, Uint32 width, Uint32 height, Int32 originx, Int32 originy ):
	CGameUITexturedObject( flags, width, height, originx, originy ),
	m_pButtonStart(nullptr),
	m_pButtonEnd(nullptr),
	m_pDragButton(nullptr),
	m_alignment(alignment),
	m_fullRangeSize(0),
	m_prevFullRangeSize(0),
	m_unitSize(unitSize),
	m_pScheme(nullptr)
{
	if(alignment == UIEL_SCROLL_V)
		m_flags |= UIEL_FL_ALIGN_R;
	else
		m_flags |= UIEL_FL_ALIGN_B;
}

//=============================================
// @brief Destructor
//
//=============================================
CGameUIScroller::~CGameUIScroller( void )
{
}

//=============================================
// @brief Tells if the parent size is valid for us
//
//=============================================
void CGameUIScroller::setFullRange( Int32 fullRangeSize )
{ 
	if(!m_prevFullRangeSize)
		m_prevFullRangeSize = fullRangeSize;

	m_fullRangeSize = fullRangeSize;
	readjustDragButton();
}

//=============================================
// @brief Tells if the parent size is valid for us
//
//=============================================
void CGameUIScroller::readjustDragButton( void )
{
	if(!m_pDragButton)
		return; // Not initialized yet

	Int32 insetStart, insetEnd;
	m_pDragButton->getBounds(insetStart, insetEnd);

	// Determine coverage
	Uint32 viewSize = 0;
	if(m_alignment == UIEL_SCROLL_V)
		viewSize = m_height;
	else
		viewSize = m_width;

	// Set to maximum if empty
	if(!m_fullRangeSize)
	{
		m_pDragButton->setLength(viewSize);
		m_pDragButton->setPosition(0);
	}

	// Determine coverage
	Float coverage = viewSize/(Float)m_fullRangeSize;
	Uint32 draggerLength = coverage*viewSize;
	if(draggerLength > viewSize)
		draggerLength = viewSize;

	m_pDragButton->setLength(draggerLength);

	if(m_flags & UIEL_FL_SCR_REVERSE)
	{
		// Always set to end
		m_pDragButton->setPosition(1.0);
	}
	else
	{
		Double prevPos = m_pDragButton->getPosition();
		Uint32 prevRealPos = m_prevFullRangeSize*prevPos;

		Double newPos = (Double)prevRealPos/(Double)m_fullRangeSize;
		m_pDragButton->setPosition((Float)newPos);
	}

	// Force a reset
	m_pDragButton->adjPosition(0, false);
	m_prevFullRangeSize = m_fullRangeSize;
}

//=============================================
// @brief Moves the scroller in the specified direction
//
//=============================================
bool CGameUIScroller::moveScroller( Int32 button, Int32 scrollAmount )
{
	if((button == MOUSE_WHEEL_UP || button == MOUSE_WHEEL_DOWN) && m_alignment != UIEL_SCROLL_V)
		return false;

	if((button == MOUSE_WHEEL_RIGHT || button == MOUSE_WHEEL_LEFT) && m_alignment != UIEL_SCROLL_H)
		return false;

	Int32 moveAmount = scrollAmount * m_unitSize;
	if(button == MOUSE_WHEEL_UP || button == MOUSE_WHEEL_RIGHT)
		moveAmount = -moveAmount;

	m_pDragButton->adjPosition(moveAmount, false);

	return true;
}

//=============================================
// @brief Resets the position of the scroller
//
//=============================================
void CGameUIScroller::resetScrollerPosition( void )
{
	m_pDragButton->setPosition(0);
	readjustDragButton();

	m_pParent->setOffsetValue(0);
}

//=============================================
// @brief Sets the source text array to render text from
//
//=============================================
void CGameUIScroller::setUnitSize( Uint32 unitsize )
{
	m_unitSize = unitsize;
}

//====================================
//
//====================================
bool CGameUIScroller::initSchema( const Char* pstrSchemaName )
{
	m_pScheme = g_pGUIManager->LoadSchemaFile(pstrSchemaName);
	if(!m_pScheme)
		return false;

	// Set background for this element
	const ui_schemeobject_t* pObject = m_pScheme->getObject("ScrollerBase");
	if(!pObject)
	{
		cl_engfuncs.pfnCon_EPrintf("Schema file %s is missing 'ScrollerBase'.\n", pstrSchemaName);
		return false;
	}

	m_pDefaultTexture = pObject->defaultTexture;

	// Final height/width to set
	Int32 finalWidth = -1;
	Int32 finalHeight = -1;

	// Set based on alignment
	CString objectName;
	Int32 flags = UIEL_FL_NONE;
	if(m_alignment == UIEL_SCROLL_V)
	{
		flags |= UIEL_FL_ALIGN_T;
		objectName = "ArrowUp";
		finalWidth = pObject->width;
	}
	else
	{
		flags |= UIEL_FL_ALIGN_L;
		objectName = "ArrowLeft";
		finalHeight = pObject->height;
	}

	pObject = m_pScheme->getObject(objectName.c_str());
	if(!pObject)
	{
		cl_engfuncs.pfnCon_EPrintf("Schema file %s is missing 'ScrollerBase'.\n", pstrSchemaName);
		return false;
	}

	// Create the start button
	CGameUIScrollerArrowBtnAction* pAction = new CGameUIScrollerArrowBtnAction(this, -1, m_unitSize);
	m_pButtonStart = new CGameUIButton(flags, pAction, 0, 0, pObject->width, pObject->height);
	m_pButtonStart->setParent(this);
	
	if(!m_pButtonStart->initSchema(pstrSchemaName))
	{
		cl_engfuncs.pfnCon_EPrintf("Failed to initialize button for scroller with schema file '%s'.\n", pstrSchemaName);
		return false;
	}

	// Set draw icon
	m_pButtonStart->setSchemaObject(pObject);

	// Set based on alignment
	flags = UIEL_FL_NONE;
	if(m_alignment == UIEL_SCROLL_V)
	{
		flags |= UIEL_FL_ALIGN_B;
		objectName = "ArrowDown";

		if(static_cast<Int32>(pObject->width) > finalWidth)
			finalWidth = pObject->width;
	}
	else
	{
		flags |= UIEL_FL_ALIGN_R;
		objectName = "ArrowRight";

		if(static_cast<Int32>(pObject->height) > finalHeight)
			finalHeight = pObject->height;
	}

	pObject = m_pScheme->getObject(objectName.c_str());
	if(!pObject)
	{
		cl_engfuncs.pfnCon_EPrintf("Schema file %s is missing 'ScrollerBase'.\n", pstrSchemaName);
		return false;
	}

	// Create the start button
	pAction = new CGameUIScrollerArrowBtnAction(this, 1, m_unitSize);
	m_pButtonEnd = new CGameUIButton(flags, pAction, 0, 0, pObject->width, pObject->height);
	m_pButtonEnd->setParent(this);

	if(!m_pButtonEnd->initSchema(pstrSchemaName))
	{
		cl_engfuncs.pfnCon_EPrintf("Failed to initialize button for scroller with schema file '%s'.\n", pstrSchemaName);
		return false;
	}

	// Set draw icon
	m_pButtonEnd->setSchemaObject(pObject);

	// Set if needed
	if(finalWidth != -1)
		m_width = finalWidth;

	if(finalHeight != -1)
		m_height = finalHeight;

	// Adjust positions
	m_pButtonStart->adjustPosition();
	m_pButtonEnd->adjustPosition();

	// Middle button is a bit more complicated
	Uint32 dragBtnWidth = 0;
	Uint32 dragBtnHeight = 0;
	Int32 dragBtnOriginX = 0;
	Int32 dragBtnOriginY = 0;

	if(m_alignment == UIEL_SCROLL_V)
	{
		flags |= UIEL_FL_FIXED_W;
		dragBtnWidth = finalWidth;

		dragBtnOriginY = m_pButtonStart->getHeight();
		dragBtnHeight = m_height - dragBtnOriginY - m_pButtonEnd->getHeight();
	}
	else
	{
		flags |= UIEL_FL_FIXED_H;
		dragBtnHeight = finalWidth;

		dragBtnOriginX = m_pButtonStart->getWidth();
		dragBtnWidth = m_width - dragBtnOriginX - m_pButtonEnd->getWidth();
	}

	// Create middle button
	CGameUIScrollerDragBtnAction* pDragAction = new CGameUIScrollerDragBtnAction(this);
	m_pDragButton = new CGameUIDragButton(flags, m_alignment, pDragAction, dragBtnWidth, dragBtnHeight, dragBtnOriginX, dragBtnOriginY);
	m_pDragButton->setParent(this);

	// Set bounds for the scroller
	if(m_alignment == UIEL_SCROLL_V)
		m_pDragButton->setBounds(m_pButtonStart->getHeight(), m_pButtonEnd->getHeight());
	else
		m_pDragButton->setBounds(m_pButtonStart->getWidth(), m_pButtonEnd->getWidth());

	if(!m_pDragButton->initSchema(pstrSchemaName))
	{
		cl_engfuncs.pfnCon_EPrintf("Failed to initialize button for scroller with schema file '%s'.\n", pstrSchemaName);
		return false;
	}
	
	adjustPosition();
	return true;
}

//=============================================
// @brief Constructor
//
//=============================================
CGameUITextInputTab::CGameUITextInputTab( Int32 flags, CGameUICallbackEvent* pAction, Uint32 textinset, const font_set_t* pFont, Int32 originx, Int32 originy, Uint32 width, Uint32 height ):
	CGameUISurface( flags, originx, originy, width, height ),
	m_pFont(pFont),
	m_nextBlinkTime(0),
	m_drawMarker(true),
	m_inset(textinset),
	m_inputPosition(0),
	m_pAction(pAction)
{
	// Clear this
	m_szBuffer[0] = '\0';
}

//=============================================
// @brief Destructor
//
//=============================================
CGameUITextInputTab::~CGameUITextInputTab( void )
{
	if(m_pAction)
		delete m_pAction;
}

//=============================================
// @brief Destructor
//
//=============================================
bool CGameUITextInputTab::mouseButtonEvent( Int32 mouseX, Int32 mouseY, Int32 button, bool keyDown )
{
	// Only left mouse click affects this
	if(button != SDL_BUTTON_LEFT)
		return false;

	if(isMouseOver(mouseX, mouseY))
	{
		// Set us as the input focus
		setInputFocusObject(this);
		return true;
	}

	return false;
}

//=============================================
// @brief Destructor
//
//=============================================
bool CGameUITextInputTab::keyEvent( Int32 button, Int16 mod, bool keyDown )
{
	if(!keyDown)
		return true;

	// Get SDL Keycode
	SDL_Keycode sdlKeycode = SDL_GetKeyFromScancode((SDL_Scancode)button);
	
	switch(sdlKeycode)
	{
    case SDLK_KP_1:
		sdlKeycode = SDLK_1;
		break;
    case SDLK_KP_2:
		sdlKeycode = SDLK_2;
		break;
    case SDLK_KP_3:
		sdlKeycode = SDLK_3;
		break;
    case SDLK_KP_4:
		sdlKeycode = SDLK_4;
		break;
    case SDLK_KP_5:
		sdlKeycode = SDLK_5;
		break;
    case SDLK_KP_6:
		sdlKeycode = SDLK_6;
		break;
    case SDLK_KP_7:
		sdlKeycode = SDLK_7;
		break;
    case SDLK_KP_8:
		sdlKeycode = SDLK_8;
		break;
    case SDLK_KP_9:
		sdlKeycode = SDLK_9;
		break;
	case SDLK_KP_0:
		sdlKeycode = SDLK_0;
		break;
	case SDLK_KP_PLUS:
		sdlKeycode = SDLK_PLUS;
		break;
	case SDLK_KP_MINUS:
		sdlKeycode = SDLK_MINUS;
		break;
	case SDLK_KP_MULTIPLY:
		sdlKeycode = SDLK_ASTERISK;
		break;
	case SDLK_KP_DIVIDE:
		sdlKeycode = SDLK_SLASH;
		break;
	}

	if(sdlKeycode == SDLK_RETURN || sdlKeycode == SDLK_KP_ENTER)
		return false;

	// See if it's a valid text input character
	if(sdlKeycode >= SDLK_SPACE && sdlKeycode <= SDLK_z)
	{
		// Avoid buffer over-indexing
		if(m_inputPosition == GAMEUI_MAX_INPUT_LENGTH)
			return true;

		// Shift if needed
		Char inputChar = (Char)sdlKeycode;
		if(mod & (KMOD_SHIFT|KMOD_CAPS))
			inputChar = Common::GetShiftedChar(inputChar);

		// Shift elements if needed
		if(m_szBuffer[m_inputPosition] != '\0')
		{
			// Insert it inbetween
			Uint32 bufLength = qstrlen(m_szBuffer);
			Char *psrc = m_szBuffer + bufLength;
			Char *pdst = m_szBuffer + m_inputPosition - 1;

			while(psrc != pdst)
			{
				Char *pcdst = psrc+1;
				*pcdst = *psrc;

				psrc--;
			}

			m_szBuffer[m_inputPosition] = inputChar;
		}
		else
		{
			// Set the input in the array
			m_szBuffer[m_inputPosition] = inputChar;
			m_szBuffer[m_inputPosition+1] = '\0';
		}

		// Always draw after input is added
		m_nextBlinkTime = cl_engfuncs.pfnGetClientTime() + 0.5;
		m_drawMarker = true;

		m_inputPosition++;
	}
	else if(sdlKeycode == SDLK_LEFT)
	{
		if(m_inputPosition > 0)
		{
			// Always draw after input is added
			m_nextBlinkTime = cl_engfuncs.pfnGetClientTime() + 0.5;
			m_drawMarker = true;

			m_inputPosition--;
		}
	}
	else if(sdlKeycode == SDLK_RIGHT)
	{
		if(m_szBuffer[m_inputPosition] != '\0')
		{
			// Always draw after input is added
			m_nextBlinkTime = cl_engfuncs.pfnGetClientTime() + 0.5;
			m_drawMarker = true;

			m_inputPosition++;
		}
	}
	else if(sdlKeycode == SDLK_BACKSPACE)
	{
		if(m_inputPosition)
		{
			if(m_szBuffer[m_inputPosition] != '\0')
			{
				// Remove inbetween
				Uint32 bufLength = qstrlen(m_szBuffer);
				Char *psrc = m_szBuffer + m_inputPosition - 1;
				Char *pdst = m_szBuffer + bufLength;

				while(psrc != pdst)
				{
					Char *pcsrc = psrc+1;
					*psrc = *pcsrc;

					psrc++;
				}
			}
			else
			{
				Char* pstr = m_szBuffer+m_inputPosition;
				*(pstr-1) = *pstr;
			}

			m_inputPosition--;
		}
	}

	// Sent a key event to the handler
	if(m_pAction)
		m_pAction->KeyEvent(button, mod, keyDown);

	return true;
}

//=============================================
// @brief Destructor
//
//=============================================
void CGameUITextInputTab::clearText( void )
{
	m_szBuffer[0] = '\0';
	m_inputPosition = 0;
}

//=============================================
// @brief Sets the contents of the text buffer
//
//=============================================
void CGameUITextInputTab::setText( const Char* pstrText )
{
	if(!pstrText)
	{
		m_inputPosition = 0;
		m_szBuffer[0] = '\0';
		return;
	}

	qstrcpy(m_szBuffer, pstrText);
	m_inputPosition = qstrlen(pstrText);
}

//=============================================
// @brief Peforms think functions
//
//=============================================
void CGameUITextInputTab::think( void )
{
	// Call base class to handle basics
	CGameUIObject::think();

	// Time input market
	if(cl_engfuncs.pfnGetClientTime() >= m_nextBlinkTime)
	{
		m_nextBlinkTime = cl_engfuncs.pfnGetClientTime() + 0.5;
		m_drawMarker = !m_drawMarker;
	}
}

//=============================================
// @brief Destructor
//
//=============================================
bool CGameUITextInputTab::draw( void )
{
	// Call base class to handle rendering
	if(!CGameUISurface::draw())
		return false;

	// Two pixels for now
	Int32 markerWidth = 2;

	// For the marker
	Char* pstr = nullptr;
	Uint32 numChars = 0;
	Uint32 stringWidth = 0;

	if(m_szBuffer[0] != '\0')
	{
		// Not happy about so many state switches
		cl_renderfuncs.pfnDisableBasicDraw();

		// Determine position to draw in
		Uint32 textHeight = cl_renderfuncs.pfnEstimateStringHeight(m_pFont, m_szBuffer, 0);

		Int32 baseOriginX, baseOriginY;
		getAbsolutePosition(baseOriginX, baseOriginY);

		// Draw in the vertical center
		Uint32 yOrg = baseOriginY + m_height/2 + textHeight/2;

		// Draw only the parts that fit in
		numChars = qstrlen(m_szBuffer)-1;
		pstr = m_szBuffer+numChars;
		while(numChars > 0)
		{
			Uint32 charWidth = m_pFont->glyphs[*pstr].advancex;
			if(m_inset+charWidth+stringWidth+markerWidth >= (m_width-m_inset))
			{
				pstr++;
				break;
			}

			stringWidth += charWidth;

			numChars--;
			pstr--;
		}

		// Draw the required chars
		color32_t color(255, 255, 255, 255);
		if(!cl_renderfuncs.pfnDrawSimpleString(color, baseOriginX+m_inset, yOrg, pstr, m_pFont))
			return false;

		// Re-enable basic drawing
		if(!cl_renderfuncs.pfnEnableBasicDraw())
			return false;

		// Re-enable blending
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	// Draw marker if needed
	if(m_drawMarker && isInputFocusObject())
	{
		Int32 baseOriginX, baseOriginY;
		getAbsolutePosition(baseOriginX, baseOriginY);

		Int32 xOrg = baseOriginX + m_inset;
		Int32 yOrg = baseOriginY + m_height/2 - m_pFont->fontsize/2;
		Int32 markerHeight = m_pFont->fontsize;

		// Determine offset
		if(pstr != nullptr)
		{
			if(m_inputPosition < numChars)
				return true;

			Uint32 nbChars = m_inputPosition - numChars;
			for(Uint32 i = 0; i < nbChars; i++)
				xOrg += m_pFont->glyphs[pstr[i]].advancex;

			xOrg += m_pFont->outlineradius;
		}

		if(!cl_renderfuncs.pfnBasicDrawDisableTextures())
			return false;

		cl_renderfuncs.pfnBasicDrawColor4f(1.0, 1.0, 1.0, 1.0);

		cl_renderfuncs.pfnValidateBasicDraw();

		cl_renderfuncs.pfnBasicDrawBegin(CBasicDraw::DRAW_TRIANGLES);
		cl_renderfuncs.pfnBasicDrawVertex3f(static_cast<Float>(xOrg), static_cast<Float>(yOrg) + static_cast<Float>(markerHeight), -1);
		cl_renderfuncs.pfnBasicDrawVertex3f(static_cast<Float>(xOrg), static_cast<Float>(yOrg), -1);
		cl_renderfuncs.pfnBasicDrawVertex3f(static_cast<Float>(xOrg)+static_cast<Float>(markerWidth), static_cast<Float>(yOrg), -1);

		cl_renderfuncs.pfnBasicDrawVertex3f(static_cast<Float>(xOrg), static_cast<Float>(yOrg)+static_cast<Float>(markerHeight), -1);
		cl_renderfuncs.pfnBasicDrawVertex3f(static_cast<Float>(xOrg)+static_cast<Float>(markerWidth), static_cast<Float>(yOrg), -1);
		cl_renderfuncs.pfnBasicDrawVertex3f(static_cast<Float>(xOrg)+static_cast<Float>(markerWidth), static_cast<Float>(yOrg)+static_cast<Float>(markerHeight), -1);
		cl_renderfuncs.pfnBasicDrawEnd();

		if(!cl_renderfuncs.pfnBasicDrawEnableTextures())
			return false;
	}

	return true;
}

//=============================================
// @brief Constructor
//
//=============================================
CGameUIScrollableSurface::CGameUIScrollableSurface( Int32 flags, const font_set_t* pFont, Int32 originx, Int32 originy, Uint32 width, Uint32 height, Uint32 postspacing ) :
	CGameUIObject(flags, originx, originy, width, height),
	m_pSurface(nullptr),
	m_pScroller(nullptr),
	m_scrollOffset(0),
	m_baseYOffset(0),
	m_borderEdgeThickness(0),
	m_baseHeight(height),
	m_postElementSpacing(postspacing),
	m_pFont(pFont)
{
}

//=============================================
// @brief Destructor
//
//=============================================
CGameUIScrollableSurface::~CGameUIScrollableSurface(void)
{
}

//=============================================
// @brief Sets the source text array to render text from
//
//=============================================
bool CGameUIScrollableSurface::initSchema( const Char* pstrBgSchemaName, const Char* pstrButtonSchemaName )
{
	// The surface is a seperate object so it isn't affected by the scroller
	m_pSurface = new CGameUISurface(m_flags, 0, 0, m_width, m_height);
	m_pSurface->setParent(this);

	if(!m_pSurface->initSchema(pstrBgSchemaName))
	{
		cl_engfuncs.pfnCon_EPrintf("Failed to set schema file '%s' for 'CGameUIScrollableSurface'.\n", pstrBgSchemaName);
		return false;
	}

	m_pSurface->setAlpha(250);

	// Create the scroller
	Int32 flags = UIEL_FL_NONE;
	if(m_flags & UIEL_FL_SCR_REVERSE)
		flags |= UIEL_FL_SCR_REVERSE;

	m_pScroller = new CGameUIScroller(flags, UIEL_SCROLL_V, 16, 0, m_height, 0, 0);
	m_pScroller->setParent(this);

	if(!m_pScroller->initSchema(pstrButtonSchemaName))
	{
		cl_engfuncs.pfnCon_EPrintf("Failed to initialize CGameUIScroller with schema file '%s' for 'CGameUIScrollableSurface'.\n", pstrButtonSchemaName);
		return false;
	}

	return true;
}

//=============================================
// @brief Sets the offset value for the element
//
//=============================================
void CGameUIScrollableSurface::setOffsetValue( Float offset )
{
	m_scrollOffset = offset;
}

//====================================
//
//====================================
bool CGameUIScrollableSurface::mouseWheelEvent( Int32 mouseX, Int32 mouseY, Int32 button, bool keyDown, Int32 scroll )
{
	if (keyDown && isVisible() && m_pScroller->moveScroller(button, scroll))
		return true;
	else
		return false;
}

//====================================
//
//====================================
bool CGameUIScrollableSurface::mouseButtonEvent( Int32 mouseX, Int32 mouseY, Int32 button, bool keyDown )
{
	if(!m_isVisible)
		return false;

	if(!isInteractive())
		return false;

	if(keyDown && !isMouseOver(mouseX, mouseY))
		return false;

	if(m_pChildrenArray.empty())
		return false;

	// Offset in modelview by the offset
	Int32 minusCoverage = m_height - m_baseHeight + m_baseYOffset;
	if (minusCoverage < 0)
		minusCoverage = 0;

	Int32 offsetAmount = minusCoverage * m_scrollOffset;

	// Prioritize interactives first
	for(Uint32 i = 0; i < m_pChildrenArray.size(); i++)
	{
		CGameUIObject* pObject = m_pChildrenArray[i];
		if (!pObject->isInteractive())
			continue;

		if(pObject->getFlags() & UIEL_FL_DRAW_LINKED_HIGHLIGHT_ONLY)
		{
			CGameUIObject* pLinkedObject = pObject->getLinkedObject();
			if(!pLinkedObject->isMouseOver())
				continue;
		}

		Int32 savedOriginX = 0;
		Int32 savedOriginY = 0;
		if (shouldShiftChild(pObject))
		{
			if (!isChildVisible(pObject, offsetAmount))
				continue;

			// Get current origin
			pObject->getPosition(savedOriginX, savedOriginY);
			// Set new origin and test
			pObject->setPosition(savedOriginX, savedOriginY + m_baseYOffset - offsetAmount);
		}

		bool bResult = pObject->mouseButtonEvent(mouseX, mouseY, button, keyDown);

		// Restore it
		if (shouldShiftChild(pObject))
			pObject->setPosition(savedOriginX, savedOriginY);

		if (bResult)
			return false;
	}

	// Now handle non-interactives
	for (Uint32 i = 0; i < m_pChildrenArray.size(); i++)
	{
		CGameUIObject* pObject = m_pChildrenArray[i];
		if (pObject->isInteractive())
			continue;

		if (pObject->getFlags() & UIEL_FL_DRAW_LINKED_HIGHLIGHT_ONLY)
		{
			CGameUIObject* pLinkedObject = pObject->getLinkedObject();
			if (!pLinkedObject->isMouseOver())
				continue;
		}

		Int32 savedOriginX = 0;
		Int32 savedOriginY = 0;
		if (shouldShiftChild(pObject))
		{
			if (!isChildVisible(pObject, offsetAmount))
				continue;

			// Get current origin
			pObject->getPosition(savedOriginX, savedOriginY);
			// Set new origin and test
			pObject->setPosition(savedOriginX, savedOriginY + m_baseYOffset - offsetAmount);
		}

		bool bResult = pObject->mouseButtonEvent(mouseX, mouseY, button, keyDown);

		// Restore it
		if (shouldShiftChild(pObject))
			pObject->setPosition(savedOriginX, savedOriginY);

		if (bResult)
			return false;
	}

	return false;
}

//====================================
//
//====================================
void CGameUIScrollableSurface::think( void )
{
	if(m_pChildrenArray.empty())
		return;

	// Set us as the input focus if the mouse is over us
	if(isMouseOver())
		setInputFocusObject(this);

	// Offset in modelview by the offset
	Int32 minusCoverage = m_height - m_baseHeight + m_baseYOffset;
	if (minusCoverage < 0)
		minusCoverage = 0;

	Int32 offsetAmount = minusCoverage * m_scrollOffset;

	for (Uint32 i = 0; i < m_pChildrenArray.size(); i++)
	{
		CGameUIObject* pObject = m_pChildrenArray[i];

		Int32 savedOriginX = 0;
		Int32 savedOriginY = 0;

		if (pObject->isInteractive() && shouldShiftChild(pObject))
		{
			if (!isChildVisible(pObject, offsetAmount))
				continue;

			pObject->getPosition(savedOriginX, savedOriginY);
			pObject->setPosition(savedOriginX, savedOriginY + m_baseYOffset - offsetAmount);
		}

		pObject->think();

		if (pObject->isInteractive() && shouldShiftChild(pObject))
			pObject->setPosition(savedOriginX, savedOriginY);
	}
}

//====================================
//
//====================================
bool CGameUIScrollableSurface::draw( void )
{
	if(!m_isVisible)
		return true;

	if(m_pChildrenArray.empty())
		return true;

	// Draw non-scrolling elements first
	for(Uint32 i = 0; i < m_pChildrenArray.size(); i++)
	{
		CGameUIObject* pObject = m_pChildrenArray[i];
		if (shouldShiftChild(pObject))
			continue;

		if(pObject->getFlags() & UIEL_FL_DRAW_LINKED_HIGHLIGHT_ONLY)
		{
			CGameUIObject* pLinkedObject = pObject->getLinkedObject();
			if(!pLinkedObject->isMouseOver())
				continue;
		}

		if(!pObject->isVisible())
			continue;

		if(!pObject->draw())
			return false;
	}

	// Get the parent's size to set up the scissor
	Int32 absX, absY;
	m_pSurface->getAbsolutePosition(absX, absY);

	Uint32 width, height;
	m_pSurface->getSize(width, height);

	Uint32 scrwidth, scrheight;
	cl_renderfuncs.pfnGetScreenSize(scrwidth, scrheight);

	Int32 originY = static_cast<Int32>(scrheight) - absY - height;

	glEnable(GL_SCISSOR_TEST);
	glScissor(absX + 2, originY + 2, width - 4, height - 4);

	// Offset in modelview by the offset
	Int32 minusCoverage = m_height - m_baseHeight + m_baseYOffset;
	if (minusCoverage < 0)
		minusCoverage = 0;

	Int32 offsetAmount = minusCoverage * m_scrollOffset;

	// Draw scrolling elements now
	for (Uint32 i = 0; i < m_pChildrenArray.size(); i++)
	{
		CGameUIObject* pObject = m_pChildrenArray[i];
		if (!shouldShiftChild(pObject))
			continue;

		if (pObject->getFlags() & UIEL_FL_DRAW_LINKED_HIGHLIGHT_ONLY)
		{
			CGameUIObject* pLinkedObject = pObject->getLinkedObject();
			if (!pLinkedObject->isMouseOver())
				continue;
		}

		if (!pObject->isVisible())
			continue;
		
		Int32 savedOriginX, savedOriginY;
		pObject->getPosition(savedOriginX, savedOriginY);

		pObject->setPosition(savedOriginX, savedOriginY + m_baseYOffset - offsetAmount);
		if (!pObject->draw())
			return false;

		pObject->setPosition(savedOriginX, savedOriginY);
	}

	glDisable(GL_SCISSOR_TEST);

	return true;
}

//====================================
//
//====================================
bool CGameUIScrollableSurface::shouldShiftChild( CGameUIObject* pChild )
{
	if (pChild == m_pScroller
		|| pChild == m_pSurface)
		return false;

	return true;
}

//====================================
//
//====================================
bool CGameUIScrollableSurface::isChildVisible( CGameUIObject* pChild, Int32 offsetAmount )
{
	Int32 xPos, yPos;
	pChild->getPosition(xPos, yPos);
	yPos -= offsetAmount;
	yPos += m_baseYOffset;

	Uint32 width, height;
	pChild->getSize(width, height);

	// Get the size and location of the background
	Int32 surfX, surfY;
	m_pSurface->getPosition(surfX, surfY);

	Uint32 surfWidth, surfHeight;
	m_pSurface->getSize(surfWidth, surfHeight);

	if (xPos >= surfX + static_cast<Int32>(surfWidth))
		return false;
	else if (xPos + static_cast<Int32>(width) < surfX)
		return false;
	else if (yPos >= surfY + static_cast<Int32>(surfHeight))
		return false;
	else if (yPos + static_cast<Int32>(height) < surfY)
		return false;

	return true;
}

//====================================
//
//====================================
bool CGameUIScrollableSurface::isMouseOver( Int32 mousex, Int32 mousey )
{
	Uint32 surfWidth, surfHeight;
	m_pSurface->getSize(surfWidth, surfHeight);

	Int32 absX, absY;
	getAbsolutePosition(absX, absY);

	if (absX > mousex)
		return false;
	if (absX + static_cast<Int32>(surfWidth) < mousex)
		return false;
	if (absY > mousey)
		return false;
	if (absY + static_cast<Int32>(surfHeight) + static_cast<Int32>(m_baseYOffset) < mousey)
		return false;

	return true;
}

//====================================
//
//====================================
bool CGameUIScrollableSurface::isMouseOver( void )
{
	Uint32 surfWidth, surfHeight;
	m_pSurface->getSize(surfWidth, surfHeight);

	Int32 absX, absY;
	getAbsolutePosition(absX, absY);

	Int32 mousex, mousey;
	cl_engfuncs.pfnGetMousePosition(mousex, mousey);

	if (absX > mousex)
		return false;
	if (absX + static_cast<Int32>(surfWidth) < mousex)
		return false;
	if (absY > mousey)
		return false;
	if (absY + static_cast<Int32>(surfHeight) + static_cast<Int32>(m_baseYOffset) < mousey)
		return false;

	return true;
}

//====================================
//
//====================================
bool CGameUIScrollableSurface::isMouseOverChild( CGameUIObject* pChild )
{
	if (!isMouseOver())
		return false;

	Uint32 i = 0;
	for (; m_pChildrenArray.size(); i++)
	{
		if (m_pChildrenArray[i] == pChild)
			break;
	}

	if (i == m_pChildrenArray.size())
		return false;

	// Offset in modelview by the offset
	Int32 minusCoverage = m_height - m_baseHeight + m_baseYOffset;
	if (minusCoverage < 0)
		minusCoverage = 0;

	Int32 offsetAmount = minusCoverage * m_scrollOffset;

	Int32 savedOriginX = 0;
	Int32 savedOriginY = 0;

	if (pChild->isInteractive() && shouldShiftChild(pChild))
	{
		if (!isChildVisible(pChild, offsetAmount))
			return false;

		pChild->getPosition(savedOriginX, savedOriginY);
		pChild->setPosition(savedOriginX, savedOriginY + m_baseYOffset - offsetAmount);
	}

	bool result = pChild->isMouseOver();

	if (pChild->isInteractive() && shouldShiftChild(pChild))
		pChild->setPosition(savedOriginX, savedOriginY);

	return result;
}

//====================================
//
//====================================
void CGameUIScrollableSurface::childPostAdjustPosition( CGameUIObject* pchild )
{
	// See if we need to be expanded
	Int32 xPos, yPos;
	pchild->getPosition(xPos, yPos);

	Int32 adjustH = 0;
	Int32 endH = yPos + pchild->getHeight();
	if (endH > (Int32)m_height)
	{
		adjustH = endH - m_height;
		adjustH += m_postElementSpacing;
	}

	m_height += adjustH;

	if (m_pScroller)
		m_pScroller->setFullRange(m_height + m_baseYOffset);
}

//====================================
//
//====================================
CGameUIHorizontalSeparator::CGameUIHorizontalSeparator( Int32 flags, Uint32 width, Uint32 height, Int32 originx, Int32 originy ):
	CGameUITexturedObject(flags, width, height, originx, originy)
{
}

//====================================
//
//====================================
CGameUIHorizontalSeparator::~CGameUIHorizontalSeparator( void )
{
}

//=============================================
// @brief Initialize schema for the object
//
//=============================================
bool CGameUIHorizontalSeparator::initSchema( const Char* pstrSchemaName )
{
	ui_schemeinfo_t* pscheme = g_pGUIManager->LoadSchemaFile(pstrSchemaName);
	if(!pscheme)
		return false;

	// Get it from the schema
	const ui_schemeobject_t* pobj = pscheme->getObject("HorizontalSeparator");
	if(!pobj)
	{
		cl_engfuncs.pfnCon_Printf("Error: Schema '%s' is missing the 'HorizontalSeparator' definition!\n", pscheme->schemeName.c_str());
		return false;
	}

	m_pDefaultTexture = pobj->defaultTexture;
	if(m_pDefaultTexture->height > m_height)
	{
		m_height = m_pDefaultTexture->height;
		adjustPosition();
	}

	return true;
}

//====================================
//
//====================================
CGameUIVerticalSeparator::CGameUIVerticalSeparator( Int32 flags, Uint32 width, Uint32 height, Int32 originx, Int32 originy ):
	CGameUITexturedObject(flags, width, height, originx, originy)
{
}

//====================================
//
//====================================
CGameUIVerticalSeparator::~CGameUIVerticalSeparator( void )
{
}

//=============================================
// @brief Initialize schema for the object
//
//=============================================
bool CGameUIVerticalSeparator::initSchema( const Char* pstrSchemaName )
{
	ui_schemeinfo_t* pscheme = g_pGUIManager->LoadSchemaFile(pstrSchemaName);
	if(!pscheme)
		return false;

	// Get it from the schema
	const ui_schemeobject_t* pobj = pscheme->getObject("VerticalSeparator");
	if(!pobj)
	{
		cl_engfuncs.pfnCon_Printf("Error: Schema '%s' is missing the 'VerticalSeparator' definition!\n", pscheme->schemeName.c_str());
		return false;
	}

	m_pDefaultTexture = pobj->defaultTexture;
	if(m_pDefaultTexture->width > m_width)
	{
		m_width = m_pDefaultTexture->width;
		adjustPosition();
	}

	return true;
}