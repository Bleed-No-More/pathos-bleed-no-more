/*
===============================================
Pathos Engine - Created by Andrew Stephen "Overfloater" Lucas

Copyright 2016
All Rights Reserved.
===============================================
*/

#ifndef GAMEUIELEMENTS_H
#define GAMEUIELEMENTS_H

#include "gameuimanager.h"
#include "gameui_shared.h"

class CGameUIManager;
class CGameUIScroller;

struct font_set_t;

// Max length of an input buffer
#define GAMEUI_MAX_INPUT_LENGTH 1024

/*
=================================
CGameUIObject

=================================
*/
class CGameUIObject
{
public:
	enum draw_flags_t
	{
		GUIEL_DF_NONE			= 0,
		GUIEL_DF_MARK_NEW		= (1<<0),
		GUIEL_DF_MARK_HIGHLIGHT	= (1<<1)
	};

public:
	CGameUIObject( Int32 flags, Int32 originX, Int32 originY, Uint32 width, Uint32 height );
	virtual ~CGameUIObject( void );

public:
	// Sets the parent of this object
	void setParent( CGameUIObject* pparent );
	// Retrieves the parent object
	CGameUIObject* getParent( void );

	// Sets the position of the object
	void setPosition( Int32 xcoord, Int32 ycoord );
	// Returns the position relative to parent
	void getPosition( Int32& xcoord, Int32& ycoord ) const;
	// Returns the absolute position
	void getAbsolutePosition( Int32& xcoord, Int32& ycoord );

	// Sets the size of the object
	void setSize( Uint32 width, Uint32 height );
	// Returns the size of the object
	void getSize( Uint32& width, Uint32& height ) const;
	// Sets the width of the object
	void setWidth( Uint32 width );
	// Retrieves the width of the object
	Uint32 getWidth( void ) const;
	// Sets the width of the object
	void setHeight( Uint32 width );
	// Retrieves the height of the object
	Uint32 getHeight( void ) const;

	// Sets visibility for the object
	void setVisible( bool visible );
	// Returns visibility state
	bool isVisible( void ) const;

	// Returns the object flags
	Int32 getFlags( void ) const;
	// Get Object flags
	Int32 getDrawFlags( void ) const;
	// Set draw flags
	void setDrawFlags( Int32 flags );
	// Set draw flags
	void removeDrawFlags( Int32 flags );

	// Adds a child object
	virtual void addChild( CGameUIObject* pchild );
	// Removes a child object
	virtual void removeChild( CGameUIObject* pchild );
	// Returns window owner of this element
	virtual CGameUIObject* getParentWindow( void );

	// Performs think functions
	virtual void think( void );
	// Performs rendering functions
	virtual bool draw( void );

	// Tells if the mouse is over the object
	virtual bool isMouseOver( Int32 mousex, Int32 mousey );
	// Tells if the mouse is over the object
	virtual bool isMouseOver( void );
	// Tell if the object is being clicked on
	virtual bool isClickedOn( void ) { return false; }

	// Manages a mouse wheel event
	virtual bool mouseWheelEvent( Int32 mouseX, Int32 mouseY, Int32 button, bool keyDown, Int32 scroll );
	// Manages a mouse button event
	virtual bool mouseButtonEvent( Int32 mouseX, Int32 mouseY, Int32 button, bool keyDown );
	// Manages a key event
	virtual bool keyEvent( Int32 button, Int16 mod, bool keyDown );

	// Re-adjusts the element's position
	virtual void adjustPosition( void );

	// Tells if the object is interactive
	virtual bool isInteractive( void ) { return true; }
	// Tells if object is a window
	virtual bool isWindow( void ) { return false; }
	// TRUE if window is in focus
	virtual bool isInFocus( void ) { return false; }
	// Set if object(window) is in focus
	virtual void setIsInFocus( bool inFocus ) { }

	// Sets offset for objects that use it
	virtual void setOffsetValue( Float offset ) { };
	// Returns the full range(for scrollers)
	virtual Uint32 getFullRange( void ) { return 0; }
	// Sets offset for objects that use it
	virtual void resetScrollerPosition( void ) { };

	// Sets the game ui object pointer
	static void SetGameUIManager( CGameUIManager* pGUIManager );

	// Sets the input focus object for parent window
	virtual void setInputFocusObject( CGameUIObject* pObject );
	// Tells if the object is the focus of inputs
	virtual bool isInputFocusObject( void );
	// Tells if the object is the focus of inputs
	virtual void setIsInputFocusObject( bool isFocusObject );

	// Sets the linked object
	virtual void setLinkedObject( CGameUIObject* pObject );
	// Returns the linked object
	virtual CGameUIObject* getLinkedObject( void );

	// Tells if the object is a button
	virtual bool isButton( void ) { return false; }
	// Disables the UI object
	virtual void setDisabled( bool isDisabled ) { }
	// Tells if the object is disabled
	virtual bool isDisabled( void ) { return false; }

	// Sets the color for the element
	virtual void setColor( Uint32 r, Uint32 g, Uint32 b, Int32 a = -1, bool recursive = false );
	// Set alpha value
	virtual void setAlpha( Int32 alpha, bool recursive = false );

	// Disables any buttons tied to this object
	virtual void setButtonsDisabled( bool isDisabled );

	// Called after child has adjusted their position
	virtual void childPostAdjustPosition( CGameUIObject* pchild ) { };

protected:
	// Object flags
	Int32 m_flags;
	// Draw flags
	Int32 m_drawFlags;
	// X coordinate of the object relative to parent
	Int32 m_originX;
	// Y coordinate of the object relative to parent
	Int32 m_originY;
	// Initial X coordinate of the object relative to parent
	Int32 m_baseOriginX;
	// Initial Y coordinate of the object relative to parent
	Int32 m_baseOriginY;
	// Color attribute
	color32_t m_color;

	// Width of the object
	Uint32 m_width;
	// Height of the object
	Uint32 m_height;

	// Parent of this object
	CGameUIObject* m_pParent;
	// Array of child objects
	CArray<CGameUIObject*> m_pChildrenArray;

	// Linked object
	CGameUIObject* m_pLinkedObject;

	// TRUE if visible
	bool m_isVisible;
	// TRUE if this is the input focus object of the parent window
	bool m_isInputFocusObject;

protected:
	// Pointer to game ui manager object
	static CGameUIManager* g_pGUIManager;
};

/*
=================================
CGameUITexturedObject

=================================
*/
class CGameUITexturedObject : public CGameUIObject
{
public:
	CGameUITexturedObject( Int32 flags, Uint32 width, Uint32 height, Int32 originx, Int32 originy );
	virtual ~CGameUITexturedObject( void );

	// Sets the default texture to use
	virtual void setDefaultTexture( const en_texture_t* ptexture ) { m_pDefaultTexture = ptexture; }
	// Sets the texture to use when the mouse is over the element
	virtual void setFocusTexture( const en_texture_t* ptexture ) { m_pFocusTexture = ptexture; }
	// Sets the texture to use when the element is clicked
	virtual void setClickedTexture( const en_texture_t* ptexture ) { m_pClickedTexture = ptexture; }
	// Sets the texture to use when the element is disabled
	virtual void setDisabledTexture( const en_texture_t* ptexture ) { m_pDisabledTexture = ptexture; }
	// Sets the texture to use when the element is highlighted
	virtual void setHighlightedTexture( const en_texture_t* ptexture ) { m_pHighlightedTexture = ptexture; }
	// Sets the texture to use when the element is a new highlighted element
	virtual void setNewItemTexture( const en_texture_t* ptexture ) { m_pNewItemTexture = ptexture; }

	// Draws the element and it's children
	virtual bool draw( void ) override;

protected:
	// Textures of the element
	const en_texture_t* m_pDefaultTexture;
	const en_texture_t* m_pFocusTexture;
	const en_texture_t* m_pClickedTexture;
	const en_texture_t* m_pDisabledTexture;
	const en_texture_t* m_pHighlightedTexture;
	const en_texture_t* m_pNewItemTexture;
};


/*
=================================
CGameUISolidSurface

=================================
*/
class CGameUISolidSurface : public CGameUIObject
{
public:
	CGameUISolidSurface( Int32 flags, const color32_t& color, Int32 originX, Int32 originY, Uint32 width, Uint32 height );
	virtual ~CGameUISolidSurface( void );

public:
	// Performs rendering functions
	bool draw( void ) override;
};

/*
=================================
CGameUISurface

=================================
*/
class CGameUISurface : public CGameUIObject
{
public:
	CGameUISurface( Int32 flags, Int32 originx, Int32 originy, Uint32 width, Uint32 height );
	virtual ~CGameUISurface( void );

public:
	// Loads the schema, and creates the sub-elements
	virtual bool initSchema( const Char* pstrSchemaName );

	// Sets the bottom border's elements
	virtual bool initBottomElements( void );
	// Sets the top border's elements
	virtual bool initTopElements( void );
	// Draws the surface, and all children
	virtual bool draw( void ) override;

	// Sets alpha value for the element
	virtual void setAlpha( Int32 alpha, bool recursive = false ) override;

	// Adjusts the surface elements
	virtual void adjustBgElements( void );
	// Adjusts the top border
	virtual void adjustTopBorder( void );
	// Repositions the element according to alignment after the parent is set
	virtual void adjustPosition( void ) override;

protected:
	// Creates a single schema sub-object
	virtual CGameUITexturedObject* createObject( const ui_schemeinfo_t* pscheme, const Char* objectName );

protected:
	// Background UI element
	CGameUITexturedObject* m_pBackground;

	// Bottom left corner element
	CGameUITexturedObject* m_pBtmLeftCorner;
	// Bottom right corner element
	CGameUITexturedObject* m_pBtmRightCorner;
	// Top left corner element
	CGameUITexturedObject* m_pTopLeftCorner;
	// Top right corner element
	CGameUITexturedObject* m_pTopRightCorner;

	// Left border element
	CGameUITexturedObject* m_pLeftBorder;
	// Right border element
	CGameUITexturedObject* m_pRightBorder;
	// Bottom border element
	CGameUITexturedObject* m_pBottomBorder;
	// Top border element
	CGameUITexturedObject* m_pTopBorder;

	// Scheme object
	const ui_schemeinfo_t* m_pScheme;
};

/*
=================================
CGameUICallbackEvent

=================================
*/
class CGameUICallbackEvent
{
public:
	CGameUICallbackEvent( void ) { };
	virtual ~CGameUICallbackEvent( void ) { };

public:
	// Performs the action
	virtual void PerformAction( Float param ) = 0;
	// Handles a special key event
	virtual bool KeyEvent( Int32 button, Int16 mod, bool keyDown ) { return false; }
	// Handles a mouse button event
	virtual bool MouseButtonEvent( Int32 mouseX, Int32 mouseY, Int32 button, bool keyDown ) { return false; }
};

/*
=================================
CGameUIText

=================================
*/
class CGameUIText : public CGameUIObject
{
public:
	CGameUIText( Int32 flags, const color32_t& color, const font_set_t* pfontset, const Char* pstrText, Int32 originX, Int32 originY );
	CGameUIText( Int32 flags, const color32_t& color, const font_set_t* pfontset, Int32 originX, Int32 originY );
	CGameUIText( Int32 flags, const color32_t& color, const font_set_t* pfontset, const Char* pstrText, Int32 originX, Int32 originY, Uint32 maxwidth, Uint32 maxheight, Uint32 textInset );
	virtual ~CGameUIText( void );

public:
	// Performs rendering functions
	virtual bool draw( void ) override;

	// Sets the text to use
	virtual void setText( const Char* pstrText );
	// Sets the text to use
	virtual void setFontSet( const font_set_t* pFontSet );
	// Sets the color to use
	virtual void setColor( const color32_t& fontcolor );

private:
	// Adjusts the size based on text contents
	void adjustSize( void );

private:
	// Render color
	color32_t m_color;
	// Text to display
	CString m_text;
	// Font set used
	const font_set_t* m_pFontSet;
	// Text inset
	Uint32 m_textInset;
	// Maximum width
	Uint32 m_maxWidth;
	// Maximum height
	Uint32 m_maxHeight;
};

/*
=================================
CGameUIButton

=================================
*/
class CGameUIButton : public CGameUISurface
{
public:
	CGameUIButton( Int32 flags, CGameUICallbackEvent* pEvent, Int32 originX, Int32 originY, Uint32 width, Uint32 height );
	CGameUIButton( Int32 flags, CGameUICallbackEvent* pEvent, SDL_Keycode keycode, Int32 originX, Int32 originY, Uint32 width, Uint32 height );
	CGameUIButton( Int32 flags, CGameUICallbackEvent* pEvent, const CArray<SDL_Keycode>& keycodesArray, Int32 originX, Int32 originY, Uint32 width, Uint32 height );
	virtual ~CGameUIButton( void );

public:
	// Manages a mouse button event
	virtual bool mouseButtonEvent( Int32 mouseX, Int32 mouseY, Int32 button, bool keyDown ) override;
	// Manages a key event
	virtual bool keyEvent( Int32 button, Int16 mod, bool keyDown ) override;

	// Performs think functions
	virtual void think( void ) override;
	// Sets text for the button
	virtual void setText( const Char* pstrText );
	// Set based on schema object
	virtual void setSchemaObject( const ui_schemeobject_t* pScheme );

	// Sets object enabled state
	virtual void setDisabled( bool isDisabled ) override;
	// Gets the object enabled state
	virtual bool isDisabled( void ) override;
	// Tells if the object is a button
	virtual bool isButton( void ) override { return true; }
	// Tell if the object is being clicked on
	virtual bool isClickedOn( void ) override { return m_isClickedOn; }

protected:
	// TRUE if the button is clicked on
	bool m_isClickedOn;
	// TRUE if the button is disabled
	bool m_isDisabled;
	// TRUE if button is highlighted
	bool m_isHighlighted;
	// Callback event on click
	CGameUICallbackEvent *m_pEvent;
	// Key tied to this button
	CArray<SDL_Keycode> m_buttonKeysArray;

	// Text to display if any
	CGameUIObject* m_pDisplayObject;
};

/*
=================================
CGameUITextTab

=================================
*/
class CGameUITextTab : public CGameUISurface
{
public:
	CGameUITextTab( Int32 flags, const font_set_t* pfontset, Uint32 textinset, const color32_t& textcolor, Int32 originX, Int32 originY, Uint32 width, Uint32 height );
	virtual ~CGameUITextTab( void );

public:
	// Performs think functions
	virtual void think( void ) override;
	// Performs rendering functions
	virtual bool draw( void ) override;
	// Init schema for this object
	virtual bool initSchema( const Char* pstrSchemaName, const Char* pstrButtonSchemaName );
	// Initializes the data
	virtual void initData( const byte* pdata, Uint32 datasize = 0 );

	// Sets offset for objects that use it
	virtual void setOffsetValue( Float offset ) override;
	// Updates range size
	virtual void updateRangeSize( void );

	// Sets the font set
	virtual void setFontSet( const font_set_t* pfontset );
	// Sets the text color
	virtual void setTextColor( const color32_t& textcolor );

	// Handles a mouse wheel event
	virtual bool mouseWheelEvent( Int32 mouseX, Int32 mouseY, Int32 button, bool keyDown, Int32 scroll ) override;
	// Manages a key event
	virtual bool keyEvent( Int32 button, Int16 mod, bool keyDown ) override;

private:
	// Text inset
	Uint32 m_textInset;
	// Text data
	CString m_displayText;
	// Font set used
	const font_set_t* m_pFontSet;
	// Text offset
	Uint32 m_textOffset;
	// Text color
	color32_t m_textColor;

	// Scroller object
	CGameUIScroller* m_pScroller;
};

/*
=================================
CGameUIWindow

=================================
*/
class CGameUIWindow : public CGameUISurface
{
public:
	enum gui_win_flags_t
	{
		FL_WINDOW_NONE				= 0,
		FL_WINDOW_KILLME			= (1<<0),
		FL_WINDOW_WAIT_TILL_NEXT	= (1<<1),
		FL_WINDOW_DELAY_REMOVE		= (1<<2),
		FL_WINDOW_NO_BGBORDERS		= (1<<3)
	};

public:
	CGameUIWindow( Int32 flags, Int32 originX, Int32 originY, Uint32 width, Uint32 height );
	virtual ~CGameUIWindow( void );

public:
	// Initializes the window
	virtual bool init( const ui_windowdescription_t* pWindowDesc, const ui_objectinfo_t* pWindowObject ) = 0;
	// Initializes the data
	virtual void initData( const byte* pdata, Uint32 datasize = 0 ) {};
	// Returns the window flags
	virtual Int32 getWindowFlags() { return m_windowFlags; }

	// Sets the input focus object
	virtual void setInputFocusObject( CGameUIObject* pObject ) override;

	// Manages a mouse wheel event
	virtual bool mouseWheelEvent( Int32 mouseX, Int32 mouseY, Int32 button, bool keyDown, Int32 scroll ) override;
	// Manages a mouse button event
	virtual bool mouseButtonEvent( Int32 mouseX, Int32 mouseY, Int32 button, bool keyDown ) override;
	// Manages a key event
	virtual bool keyEvent( Int32 button, Int16 mod, bool keyDown ) override;

	// Called when the window is removed
	virtual void onRemove( void ) {};
	// Returns the time at which the window is to be removed
	virtual Double getWindowRemoveTime() { return m_windowRemoveTime; }
	// Mark for delayed removal
	virtual void setDelayedRemoval( Double delay );
	// Returns the type of the window
	virtual gameui_windows_t getWindowType( void ) const { return GAMEUI_WINDOW_NONE; }
	// Tells if object is a window
	virtual bool isWindow( void ) override { return true; }
	// TRUE if window is in focus
	virtual bool isInFocus( void ) override;
	// Set if object(window) is in focus
	virtual void setIsInFocus( bool inFocus ) override;

protected:
	// Window flags
	Int32 m_windowFlags;
	// Input focus object
	CGameUIObject* m_pInputFocusObject;
	// Delay until window close
	Double m_windowRemoveTime;
	// TRUE if window is in focus
	bool m_isInFocus;
};

/*
=================================
CGameUIDragButton

=================================
*/
class CGameUIDragButton : public CGameUIButton
{
public:
	CGameUIDragButton( Int32 flags, ui_scroller_align_t alignment, CGameUICallbackEvent* pEvent, Uint32 width, Uint32 height, Int32 originx, Int32 originy );
	virtual ~CGameUIDragButton( void );

public:
	// Sets min/max offset from parent
	virtual void setBounds( Int32 start, Int32 end );
	// Sets min/max offset from parent
	virtual void getBounds( Int32& start, Int32& end );

	// Handles a mouse button event
	virtual bool mouseButtonEvent( Int32 mouseX, Int32 mouseY, Int32 button, bool keyDown ) override;
	// Performs think functions
	virtual void think( void ) override;

	// Sets the position of the button
	virtual void setPosition( Float position );
	// Gets the position of the button
	virtual Float getPosition( void );
	// Adjusts the position of the button
	virtual bool adjPosition( Int32 adjAmt, bool isMouseDrag, bool callEvent = true );

	// Sets the length of the button
	virtual void setLength( Uint32 length );
	// Gets the length of the button
	virtual Uint32 getLength( void );

	// Repositions the element according to alignment after the parent is set
	virtual void adjustPosition( void ) override;

protected:
	// Alignment of the scroller
	ui_scroller_align_t m_alignment;

	// Previous mouse x position while clicked on
	Int32 m_lastMouseX;
	// Previous mouse y position while clicked on
	Int32 m_lastMouseY;

	// Starting offset from parent
	Int32 m_startInset;
	// Offset from parent's other end
	Int32 m_endInset;

	// Current position of the slider
	Float m_position;
	// Last parent length the position was set on
	Uint32 m_lastParentLength;
};

/*
=================================
CGameUIScroller

=================================
*/
class CGameUIScroller : public CGameUITexturedObject
{
public:
	// Arrow button action object
	class CGameUIScrollerArrowBtnAction : public CGameUICallbackEvent
	{
		public:
			CGameUIScrollerArrowBtnAction( CGameUIScroller* pScroller, Int32 direction, Uint32 jumpSize ):
				m_pScroller(pScroller),
				m_direction(direction),
				m_jumpSize(jumpSize)
			{ };
			virtual ~CGameUIScrollerArrowBtnAction( void ) { };
			
		public:
			virtual void PerformAction( Float param ) override;

		private:
			// Window that created this
			CGameUIScroller* m_pScroller;
			// Direction sign
			Int32 m_direction;
			// Jump size
			Uint32 m_jumpSize;
	};

	// Dragger button action
	class CGameUIScrollerDragBtnAction : public CGameUICallbackEvent
	{
		public:
			explicit CGameUIScrollerDragBtnAction( CGameUIScroller* pScroller ):
				m_pScroller(pScroller)
			{ };
			virtual ~CGameUIScrollerDragBtnAction( void ) { };
			
		public:
			virtual void PerformAction( Float param ) override;

		private:
			// Window that created this
			CGameUIScroller* m_pScroller;
	};

public:
	CGameUIScroller( Int32 flags, ui_scroller_align_t alignment, Uint32 unitSize, Uint32 width, Uint32 height, Int32 originx, Int32 originy );
	virtual ~CGameUIScroller( void );

public:
	// Init schema for this object
	virtual bool initSchema( const Char* pstrSchemaName );

	// Queries vif the element is interactive
	virtual bool isInteractive( void ) override { return true; }
	// Sets offset for objects that use it
	virtual void resetScrollerPosition( void ) override;

	// Sets the full range's size
	virtual void setFullRange( Int32 fullRangeSize );
	// Returns the full range's size
	virtual Uint32 getFullRange( void ) override { return m_fullRangeSize; }
	// Readjusts the scroll drag button's position and size
	virtual void readjustDragButton( void );
	// Sets the unit size
	virtual void setUnitSize( Uint32 unitsize );

	// Returns the dragger button
	CGameUIDragButton* getDragButton( void ) { return m_pDragButton; }

	// Retreives the alignment
	virtual ui_scroller_align_t getAlignment( void ) { return m_alignment; }
	// Moves the scroller
	virtual bool moveScroller( Int32 button, Int32 scrollAmount );


protected:
	// Button at beginning of the scroller
	CGameUIButton* m_pButtonStart;
	// Button at end of the scroller
	CGameUIButton* m_pButtonEnd;
	// Drag button for the scroller
	CGameUIDragButton* m_pDragButton;

	// Alignment
	ui_scroller_align_t m_alignment;

	// Total area to consider
	Int32 m_fullRangeSize;
	// Previous range we considered
	Int32 m_prevFullRangeSize;
	// Size of a single jump unit
	Uint32 m_unitSize;

	// Scheme object
	const ui_schemeinfo_t* m_pScheme;
};

/*
=================================
CGameUITextInputTab

=================================
*/
class CGameUITextInputTab : public CGameUISurface
{
public:
	CGameUITextInputTab( Int32 flags, CGameUICallbackEvent* pAction, Uint32 textinset, const font_set_t* pFont, Int32 originx, Int32 originy, Uint32 width, Uint32 height );
	virtual ~CGameUITextInputTab( void );

public:
	// Handles a mouse button event
	virtual bool mouseButtonEvent( Int32 mouseX, Int32 mouseY, Int32 button, bool keyDown ) override;
	// Handles a keyboard input event
	virtual bool keyEvent( Int32 button, Int16 mod, bool keyDown ) override;
	// Draws the text on the UI object
	virtual bool draw( void ) override;
	// Peforms think functions
	virtual void think( void ) override;

	// Specifies the text inset
	virtual void setTextInset( Uint32 inset ) { m_inset = inset; }
	// Returns the buffer's contents
	virtual const Char* getText( void ) { return m_szBuffer; }
	// Sets the buffer's contents
	virtual void setText( const Char* pstrText );
	// Clears the buffer
	virtual void clearText( void );

protected:
	// Font set used
	const font_set_t* m_pFont;
	// Next marker blink time
	Float m_nextBlinkTime;
	// Boolean for market
	bool m_drawMarker;
	// Text inset on x axis
	Uint32 m_inset;

	// Current input string
	Char m_szBuffer[GAMEUI_MAX_INPUT_LENGTH+1];
	// Marker position
	Uint32 m_inputPosition;

	// Callback handler
	CGameUICallbackEvent* m_pAction;
};

/*
=================================
CGameUIScrollableSurface

=================================
*/
class CGameUIScrollableSurface : public CGameUIObject
{
public:
	CGameUIScrollableSurface( Int32 flags, const font_set_t* pFont, Int32 originx, Int32 originy, Uint32 width, Uint32 height, Uint32 postspacing );
	virtual ~CGameUIScrollableSurface( void );

public:
	// Loads the schema, and creates the sub-elements
	virtual bool initSchema( const Char* pstrBgSchemaName, const Char* pstrButtonSchemaName );

	// Tells if the object is interactive
	virtual bool isInteractive( void ) override { return true; }

	// Sets the scroller offset value(only for text tabs/lists
	virtual void setOffsetValue( Float offset ) override;

	// Manages a mouse wheel event
	virtual bool mouseWheelEvent( Int32 mouseX, Int32 mouseY, Int32 button, bool keyDown, Int32 scroll ) override;
	// Manages a mouse button event
	virtual bool mouseButtonEvent( Int32 mouseX, Int32 mouseY, Int32 button, bool keyDown ) override;

	// Performs think functions
	virtual void think( void ) override;
	// Performs rendering functions
	virtual bool draw( void ) override;

	// Tells if the mouse is over the object
	virtual bool isMouseOver( Int32 mousex, Int32 mousey ) override;
	// Tells if the mouse is over the object
	virtual bool isMouseOver( void ) override;

	// Called after child has adjusted their position
	virtual void childPostAdjustPosition( CGameUIObject* pchild ) override;

public:
	// Adjusts the element's size by x and y amount
	bool adjustSize( void );
	// TRUE if child should be shifted
	bool shouldShiftChild( CGameUIObject* pChild );
	// Tells if child is visible
	bool isChildVisible( CGameUIObject* pChild, Int32 offsetAmount );
	// Tells if the mouse is over the object
	bool isMouseOverChild( CGameUIObject* pChild );

public:
	// BG surface
	CGameUISurface* m_pSurface;
	// Scroller object
	CGameUIScroller* m_pScroller;
	// Offset value
	Float m_scrollOffset;
	// Base Y offset
	Uint32 m_baseYOffset;
	// Edge thickness
	Uint32 m_borderEdgeThickness;
	// Base height
	Uint32 m_baseHeight;
	// Element after-spacing
	Uint32 m_postElementSpacing;

	// Font set used
	const font_set_t* m_pFont;
};

/*
=================================
CGameUIHorizontalSeparator

=================================
*/
class CGameUIHorizontalSeparator : public CGameUITexturedObject
{
public:
	CGameUIHorizontalSeparator( Int32 flags, Uint32 width, Uint32 height, Int32 originx, Int32 originy );
	virtual ~CGameUIHorizontalSeparator( void );

public:
	// Loads the schema, and creates the sub-elements
	virtual bool initSchema( const Char* pstrSchemaName );
};

/*
=================================
CGameUIVerticalSeparator

=================================
*/
class CGameUIVerticalSeparator : public CGameUITexturedObject
{
public:
	CGameUIVerticalSeparator( Int32 flags, Uint32 width, Uint32 height, Int32 originx, Int32 originy );
	virtual ~CGameUIVerticalSeparator( void );

public:
	// Loads the schema, and creates the sub-elements
	virtual bool initSchema( const Char* pstrSchemaName );
};
#endif //GAMEUIELEMENTS_H