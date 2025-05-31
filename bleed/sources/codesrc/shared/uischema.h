/*
===============================================
Pathos Engine - Created by Andrew Stephen "Overfloater" Lucas

Copyright 2016
All Rights Reserved.
===============================================
*/

#ifndef UISCHEMA_H
#define UISCHEMA_H

#include "file_interface.h"
#include "fontset.h"

struct en_texture_t;

struct ui_schemeobject_t
{
	ui_schemeobject_t():
		width(0),
		height(0),
		defaultTexture(nullptr),
		focusTexture(nullptr),
		clickTexture(nullptr),
		disabledTexture(nullptr),
		highlightTexture(nullptr),
		newItemTexture(nullptr)
	{}

	CString typeName;

	Uint32 width;
	Uint32 height;

	en_texture_t* defaultTexture;
	en_texture_t* focusTexture;
	en_texture_t* clickTexture;
	en_texture_t* disabledTexture;
	en_texture_t* highlightTexture;
	en_texture_t* newItemTexture;
};

struct ui_schemeinfo_t
{
	inline const ui_schemeobject_t* getObject( const Char* pstrName ) const
	{
		for(Uint32 i = 0; i < tabObjects.size(); i++)
		{
			if(!qstrcmp(tabObjects[i].typeName, pstrName))
				return &tabObjects[i];
		}

		return nullptr;
	}

	CString	schemeName;
	CArray<ui_schemeobject_t> tabObjects;
};

enum ui_object_type_t
{
	UI_OBJECT_UNDEFINED = -1,
	UI_OBJECT_SEPARATOR_H,
	UI_OBJECT_SEPARATOR_V,
	UI_OBJECT_TEXT,
	UI_OBJECT_TEXT_TAB,
	UI_OBJECT_BUTTON,
	UI_OBJECT_WINDOW,
	UI_OBJECT_TAB,
	UI_OBJECT_LIST,
	UI_OBJECT_TICKBOX,
	UI_OBJECT_SLIDER
};

enum ui_elem_flags_t
{
	UIEL_FL_NONE						= 0,		// No special flags
	UIEL_FL_FIXED_W						= (1<<0),	// Width of the element is fixed
	UIEL_FL_FIXED_H						= (1<<1),	// Height of the element is fixed
	UIEL_FL_ALIGN_L						= (1<<2),	// Align to the left edge of the parent
	UIEL_FL_ALIGN_R						= (1<<3),	// Align to the right edge of the parent
	UIEL_FL_ALIGN_T						= (1<<4),	// Align to the top edge of the parent
	UIEL_FL_ALIGN_B						= (1<<5),	// Align to the bottom edge of the parent
	UIEL_FL_ALIGN_CH					= (1<<6),	// Align to center of parent horizontally
	UIEL_FL_ALIGN_CV					= (1<<7),	// Align to center of parent vertically
	UIEL_FL_WRAP_WORD					= (1<<8),	// Breaks long lines into multiple lines
	UIEL_FL_FIXED_XPOS					= (1<<9),	// Relative X position is fixed in offset
	UIEL_FL_FIXED_YPOS					= (1<<10),	// Relative Y position is fixed in offset
	UIEL_FL_ONTOP						= (1<<11),	// Always draw last
	UIEL_FL_SCR_REVERSE					= (1<<12),  // Always auto-position scroller to the end when contents are changed
	UIEL_FL_EXPAND_W					= (1<<13),  // Width can be expanded by child elements
	UIEL_FL_EXPAND_H					= (1<<14),  // Height can be expanded by child elements
	UIEL_FL_NO_HEADER					= (1<<15),  // CUIList - No header used
	UIEL_FL_HOVER_HIGHLIGHT				= (1<<16),	// CUIList - Highlight elements the mouse hovers over
	UIEL_FL_DRAW_LINKED_HIGHLIGHT_ONLY	= (1<<17),	// Draw linked elements only if highlighted by mouse
	UIEL_FL_NO_BOTTOM_BORDER			= (1<<18),	// Do not draw bottom border part
	UIEL_FL_RESOLUTION_RELATIVE			= (1<<19),	// Sizes and origins are relative to screen resolution
};

enum ui_scroller_align_t
{
	UIEL_SCROLL_V = 0,
	UIEL_SCROLL_H,
};

struct ui_objectinfo_t
{
	ui_objectinfo_t():
		type(UI_OBJECT_UNDEFINED),
		originx(0),
		originy(0),
		width(0),
		height(0),
		insetx(0),
		insety(0),
		title_insetx(0),
		title_insety(0),
		text_inset(0),
		listrowheight(0),
		listpostspacing(0),
		alpha(0),
		flags(0),
		pfont(nullptr),
		minvalue(0),
		maxvalue(0),
		markerdistance(0),
		screenresolution(0),
		basescreenresolution(0),
		basescreenwidth(0),
		basescreenheight(0),
		resizable(false),
		dragger(false)
		{}

	const Char* getName( void ) const { return objectName.c_str(); }
	ui_object_type_t getType( void ) const { return type; }

	Uint32 getXOrigin( void ) const 
	{ 
		Uint32 outValue = originx;
		if((flags & UIEL_FL_RESOLUTION_RELATIVE) && screenresolution > 0 && basescreenresolution > 0)
			outValue *= (static_cast<Float>(screenresolution) / static_cast<Float>(basescreenresolution));

		return outValue; 
	}

	Uint32 getYOrigin( void ) const 
	{ 
		Uint32 outValue = originy;
		if((flags & UIEL_FL_RESOLUTION_RELATIVE) && screenresolution > 0 && basescreenresolution > 0)
			outValue *= (static_cast<Float>(screenresolution) / static_cast<Float>(basescreenresolution));

		return outValue; 
	}

	Uint32 getWidth( void ) const 
	{ 
		Uint32 outValue = width;
		if((flags & UIEL_FL_RESOLUTION_RELATIVE) && screenresolution > 0 && basescreenresolution > 0)
			outValue *= (static_cast<Float>(screenresolution) / static_cast<Float>(basescreenresolution));

		return outValue; 
	}

	Uint32 getHeight( void ) const 
	{ 
		Uint32 outValue = height;
		if((flags & UIEL_FL_RESOLUTION_RELATIVE) && screenresolution > 0 && basescreenresolution > 0)
			outValue *= (static_cast<Float>(screenresolution) / static_cast<Float>(basescreenresolution));

		return outValue;  
	}

	Uint32 getXInset( void ) const 
	{
		Uint32 outValue = insetx;
		if((flags & UIEL_FL_RESOLUTION_RELATIVE) && screenresolution > 0 && basescreenresolution > 0)
			outValue *= (static_cast<Float>(screenresolution) / static_cast<Float>(basescreenresolution));

		return outValue;  
	}

	Uint32 getYInset( void ) const 
	{ 
		Uint32 outValue = insety;
		if((flags & UIEL_FL_RESOLUTION_RELATIVE) && screenresolution > 0 && basescreenresolution > 0)
			outValue *= (static_cast<Float>(screenresolution) / static_cast<Float>(basescreenresolution));

		return outValue; 
	}

	Uint32 getTitleXInset( void ) const 
	{ 
		Uint32 outValue = title_insetx;
		if((flags & UIEL_FL_RESOLUTION_RELATIVE) && screenresolution > 0 && basescreenresolution > 0)
			outValue *= (static_cast<Float>(screenresolution) / static_cast<Float>(basescreenresolution));

		return outValue; 
	}

	Uint32 getTitleYInset( void ) const 
	{ 
		Uint32 outValue = title_insety;
		if((flags & UIEL_FL_RESOLUTION_RELATIVE) && screenresolution > 0 && basescreenresolution > 0)
			outValue *= (static_cast<Float>(screenresolution) / static_cast<Float>(basescreenresolution));

		return outValue;
	}

	Uint32 getTextInset( void ) const 
	{ 
		Uint32 outValue = text_inset;
		if((flags & UIEL_FL_RESOLUTION_RELATIVE) && screenresolution > 0 && basescreenresolution > 0)
			outValue *= (static_cast<Float>(screenresolution) / static_cast<Float>(basescreenresolution));

		return outValue;
	}

	Uint32 getListRowHeight( void ) const 
	{ 
		Uint32 outValue = listrowheight;
		if((flags & UIEL_FL_RESOLUTION_RELATIVE) && screenresolution > 0 && basescreenresolution > 0)
			outValue *= (static_cast<Float>(screenresolution) / static_cast<Float>(basescreenresolution));

		return outValue;
	}
	Uint32 getListPostSpacing( void ) const 
	{ 
		Uint32 outValue = listpostspacing;
		if((flags & UIEL_FL_RESOLUTION_RELATIVE) && screenresolution > 0 && basescreenresolution > 0)
			outValue *= (static_cast<Float>(screenresolution) / static_cast<Float>(basescreenresolution));

		return outValue;
	}

	Float getAlpha( void ) const { return alpha; }
	Int32 getFlags( void ) const { return flags; }

	Float getMinValue( void ) const { return minvalue; }
	Float getMaxValue( void ) const { return maxvalue; }
	Float getMarkerDistance( void ) const { return markerdistance; }

	const CString& getText( void ) const { return text; }
	const CString& getTitle( void ) const { return title; }
	const CString& getSchema( void ) const { return schema; }
	const CString& getButtonSchema( void ) const { return buttonschema; }
	const CString& getTextSchemaName( void ) const { return textschemaname; }
	const color32_t& getTextColor( void ) const { return textcolor; }

	bool isResizable( void ) const { return resizable; }
	bool hasDragger( void ) const { return dragger; }
	const font_set_t* getFont( void ) const { return pfont; }

	void setScreenResolution( Uint32 screenwidth, Uint32 screenheight ) 
	{ 
		Uint32 resolution;
		Uint32 baseresolution;
		if(screenwidth > screenheight)
		{
			resolution = screenheight;
			baseresolution = basescreenheight;
		}
		else
		{
			resolution = screenwidth;
			baseresolution = basescreenwidth;
		}

		screenresolution = resolution;
		basescreenresolution = baseresolution;
	}

	CString objectName;
	ui_object_type_t type;

	Int32 originx;
	Int32 originy;

	Uint32 width;
	Uint32 height;

	Uint32 insetx;
	Uint32 insety;

	Uint32 title_insetx;
	Uint32 title_insety;

	Uint32 text_inset;

	Uint32 listrowheight;
	Uint32 listpostspacing;

	Float alpha;

	Int32 flags;

	CString text;
	CString title;
	CString schema;
	CString buttonschema;
	CString textschemaname;

	const font_set_t* pfont;

	Float minvalue;
	Float maxvalue;
	Float markerdistance;

	color32_t textcolor;

	Uint32 screenresolution;
	Uint32 basescreenresolution;

	Uint32 basescreenwidth;
	Uint32 basescreenheight;

	bool resizable;
	bool dragger;
};

struct ui_windowdescription_t
{
	ui_windowdescription_t()
		{}

	inline const ui_objectinfo_t* getObject( ui_object_type_t type, const Char* pstrName ) const
	{
		for(Uint32 i = 0; i < objectsArray.size(); i++)
		{
			if(objectsArray[i].type != type)
				continue;

			if(!qstrcmp(objectsArray[i].objectName, pstrName))
				return &objectsArray[i];
		}

		return nullptr;
	}

	CString windowName;
	CArray<ui_objectinfo_t> objectsArray;
};

/*
=================================
CUISchemaManager

=================================
*/
class CUISchemaManager
{
public:
	typedef en_texture_t*		(*pfnGetDummyTexture_t)( void );
	typedef en_texture_t*		(*pfnLoadTexture_t)( const Char* pstrFilename, rs_level_t level, Int32 flags, const GLint* pborder );
	typedef const font_set_t*	(*pfnGetSchemaFontSet_t)( const Char* schemaFileName );

public:
	CUISchemaManager( const file_interface_t& fileInterface, pfnGetDummyTexture_t pfnGetDummyTexture, pfnLoadTexture_t pfnLoadTexture, pfnGetSchemaFontSet_t pfnGetSchemaFontSet, const font_set_t* pDefaultFontSet );
	~CUISchemaManager( void );

public:
	// Clears all data
	void Clear( void );
	// Loads in a schema file
	ui_schemeinfo_t* LoadSchemaFile( const Char* pstrFilename );
	// Loads in a window schema file
	ui_windowdescription_t* LoadWindowDescriptionFile( const Char* pstrWindowName, const Char* pstrFilename );

	// Get nb of warnings
	Uint32 GetNbWarnings( void ) const;
	// Get warning
	CString GetWarning( Uint32 index ) const;
	// Clear warnings
	void ClearWarnings( void );

	// Get error MSG if any
	const CString& GetErrorString( void ) const;

	// Set screen resolution
	void SetScreenResolution( Uint32 screenWidth, Uint32 screenHeight );

private:
	// Array of schemas loaded
	CArray<ui_schemeinfo_t*> m_tabSchemeArray;
	// Array of window descriptions
	CArray<ui_windowdescription_t*> m_windowDescriptionArray;
	// Warnings array
	CArray<CString> m_warningStringArray;
	// Error string
	CString m_errorString;

	// Get dummy texture fn
	pfnGetDummyTexture_t m_pfnGetDummyTexture;
	// Load texture fn ptr
	pfnLoadTexture_t m_pfnLoadTexture;
	// Get resolution schema fn ptr
	pfnGetSchemaFontSet_t m_pfnGetSchemaFontSet;

	// Default font set used
	const font_set_t* m_pDefaultFontSet;

	// Screen width
	Uint32 m_screenWidth;
	// Screen width
	Uint32 m_screenHeight;

	// File interface
	const file_interface_t& m_fileInterface;
};
#endif //UISCHEMA_H