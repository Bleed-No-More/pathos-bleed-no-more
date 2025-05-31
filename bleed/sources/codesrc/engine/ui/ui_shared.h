/*
===============================================
Pathos Engine - Created by Andrew Stephen "Overfloater" Lucas

Copyright 2016
All Rights Reserved.
===============================================
*/

#ifndef UI_SHARED_H
#define UI_SHARED_H

#include "r_basicdraw.h"
#include "uischema.h"

struct en_texture_t;
struct font_set_t;

class CMatrix;

enum tab_flags_t
{
	TAB_FL_NONE			= 0,
	TAB_FL_RESIZABLE	= (1<<1),
	TAB_FL_MOVABLE		= (1<<2),
	TAB_FL_CLAMP_S		= (1<<3),
	TAB_FL_CLAMP_T		= (1<<4)
};

struct ui_engine_interface_t
{
	ui_engine_interface_t():
		pfnGetEngineTime(nullptr),
		pfnCon_Printf(nullptr),
		pfnCon_DPrintf(nullptr),
		pfnCon_VPrintf(nullptr),
		pfnCon_EPrintf(nullptr),
		pfnGetMousePosition(nullptr),
		pfnUILoadSchemaFile(nullptr),
		pfnBasicDrawIsActive(nullptr),
		pfnEnableBasicDraw(nullptr),
		pfnDisableBasicDraw(nullptr),
		pfnBasicDrawEnableTextures(nullptr),
		pfnBasicDrawDisableTextures(nullptr),
		pfnBasicDrawBegin(nullptr),
		pfnBasicDrawEnd(nullptr),
		pfnBasicDrawColor4f(nullptr),
		pfnBasicDrawColor4fv(nullptr),
		pfnBasicDrawTexCoord2f(nullptr),
		pfnBasicDrawTexCoord2fv(nullptr),
		pfnBasicDrawVertex3f(nullptr),
		pfnBasicDrawVertex3fv(nullptr),
		pfnBasicDrawSetProjection(nullptr),
		pfnBasicDrawSetModelView(nullptr),
		pfnBind2DTexture(nullptr),
		pfnGetDefaultFontSet(nullptr),
		pfnLoadFontSet(nullptr),
		pfnDrawSimpleString(nullptr),
		pfnDrawStringBox(nullptr),
		pfnBeginTextRendering(nullptr),
		pfnFinishTextRendering(nullptr),
		pfnDrawCharacter(nullptr),
		pfnGetStringSize(nullptr),
		pfnEstimateStringHeight(nullptr),
		pfnSetStringRectangle(nullptr),
		pfnGetProjectionMatrix(nullptr),
		pfnGetModelViewMatrix(nullptr),
		pfnGetWindowSize(nullptr),
		pfnValidateBasicDraw(nullptr)
		{
		}

	// Returns the engine time
	Double				(*pfnGetEngineTime)( void );

	void				(*pfnCon_Printf)( const Char *fmt, ... );
	void				(*pfnCon_DPrintf)( const Char *fmt, ... );
	void				(*pfnCon_VPrintf)( const Char *fmt, ... );
	void				(*pfnCon_EPrintf)( const Char *fmt, ... );

	// Returns the mouse position
	void				(*pfnGetMousePosition)( Int32& x, Int32& y );

	// Loads a window schema file
	ui_schemeinfo_t*	(*pfnUILoadSchemaFile)( const Char* pstrFilename );

	// Tells if basic draw is active
	bool				(*pfnBasicDrawIsActive)( void );
	// Enable basic draw
	bool				(*pfnEnableBasicDraw)( void );
	// Disable basic draw
	void				(*pfnDisableBasicDraw)( void );
	// Enable textures in basic draw
	bool				(*pfnBasicDrawEnableTextures)( void );
	// Disable textures in basic draw
	bool				(*pfnBasicDrawDisableTextures)( void );
	// Resets states for primitive rendering
	void				(*pfnBasicDrawBegin)( CBasicDraw::primitivetype_t primitiveType );
	// Renders the primitives
	void				(*pfnBasicDrawEnd)( void );
	// Sets a vertex's color
	void				(*pfnBasicDrawColor4f)( Float r, Float g, Float b, Float a );
	void				(*pfnBasicDrawColor4fv)( const Float* pfc );
	// Sets a vertex's texcoords
	void				(*pfnBasicDrawTexCoord2f)( Float u, Float v );
	void				(*pfnBasicDrawTexCoord2fv)( const Float* ptc );
	// Sets a vertex's origin
	void				(*pfnBasicDrawVertex3f)( Float x, Float y, Float z );
	void				(*pfnBasicDrawVertex3fv)( const Float* pfv );
	// Sets the projection matrix
	void				(*pfnBasicDrawSetProjection)( const Float* pMatrix );
	// Sets the modelview matrix
	void				(*pfnBasicDrawSetModelView)( const Float* pMatrix );

	// Binds a texture to a texture unit
	void				(*pfnBind2DTexture)( Int32 texture, Uint32 id, bool force );

	// Gets the default font set
	const font_set_t*	(*pfnGetDefaultFontSet)( void );
	// Loads a font set
	const font_set_t*	(*pfnLoadFontSet)( const Char *pstrFilename, Int32 fontSize );
	// Draws a simple string on the screen
	bool				(*pfnDrawSimpleString)( color32_t color, Int32 x, Int32 y, const Char* pstrString, const font_set_t* pfont );
	// Draws a string box on the screen
	bool				(*pfnDrawStringBox)( Int16 minx, Int16 miny, Int16 maxx, Int16 maxy, Int16 insetx, Int16 insety, bool reverse, color32_t color, Int32 x, Int32 y, const Char* pstrString, const font_set_t* pfont, Uint32 offset, Uint32 minlineheight, Uint32 xoffset );
	// Sets up for text rendering
	bool				(*pfnBeginTextRendering)( const font_set_t* pfontset );
	// Ends text rendering
	void				(*pfnFinishTextRendering)( void );
	// Draws a single character
	bool				(*pfnDrawCharacter)( Int32 x, Int32 y, Char character, Uint32 r, Uint32 g, Uint32 b, Uint32 a );
	// Returns the size of a string in pixels
	void				(*pfnGetStringSize)( const font_set_t *pset, const Char *pstring, Uint32 *width, Uint32 *height, Int32 *ymin );
	// Estimates height of a string
	Int32				(*pfnEstimateStringHeight)( const font_set_t *pset, const Char *pstrString, Uint32 minlineheight );
	// Sets the string drawing rectangle
	void				(*pfnSetStringRectangle)( Int16 minx, Int16 miny, Int16 maxx, Int16 maxy, Int32 insetx, Int32 insety );

	// Retreives the current projection matrix
	CMatrix&			(*pfnGetProjectionMatrix)( void );
	// Retreives the current modelview matrix
	CMatrix&			(*pfnGetModelViewMatrix)( void );

	// Gets the window sizes
	void				(*pfnGetWindowSize)( Uint32& width, Uint32& height );
	// Validates basic draw
	void				(*pfnValidateBasicDraw)( void );
};

#endif //UI_SHARED