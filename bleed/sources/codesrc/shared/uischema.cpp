/*
===============================================
Pathos Engine - Created by Andrew Stephen "Overfloater" Lucas

Copyright 2016
All Rights Reserved.
===============================================
*/

#include "includes.h"
#include "uischema.h"

#include "r_common.h"
#include "textures_shared.h"

//=============================================
// @brief Constructor
//
//=============================================
CUISchemaManager::CUISchemaManager( const file_interface_t& fileInterface, pfnGetDummyTexture_t pfnGetDummyTexture, pfnLoadTexture_t pfnLoadTexture, pfnGetSchemaFontSet_t pfnGetSchemaFontSet, const font_set_t* pDefaultFontSet ):
	m_fileInterface( fileInterface ),
	m_pfnGetDummyTexture(pfnGetDummyTexture),
	m_pfnLoadTexture(pfnLoadTexture),
	m_pfnGetSchemaFontSet(pfnGetSchemaFontSet),
	m_pDefaultFontSet(pDefaultFontSet),
	m_screenWidth(0),
	m_screenHeight(0)
{
}

//=============================================
// @brief Destructor
//
//=============================================
CUISchemaManager::~CUISchemaManager( void )
{
	Clear();
}

//=============================================
// @brief
//
//=============================================
void CUISchemaManager::Clear( void )
{
	if(m_tabSchemeArray.empty())
		return;

	for(Uint32 i = 0; i < m_tabSchemeArray.size(); i++)
		delete m_tabSchemeArray[i];

	if(!m_windowDescriptionArray.empty())
	{
		for(Uint32 i = 0; i < m_windowDescriptionArray.size(); i++)
			delete m_windowDescriptionArray[i];

		m_windowDescriptionArray.clear();
	}

	m_tabSchemeArray.clear();
}

//=============================================
// @brief
//
//=============================================
ui_schemeinfo_t* CUISchemaManager::LoadSchemaFile( const Char* pstrFilename )
{
	// Try to find it in the cache first
	for(Uint32 i = 0; i < m_tabSchemeArray.size(); i++)
	{
		if(!qstrcmp(m_tabSchemeArray[i]->schemeName, pstrFilename))
			return m_tabSchemeArray[i];
	}

	// Load in the file
	CString scriptPath;
	scriptPath << "scripts/ui/schemas/" << pstrFilename;

	Uint32 fileSize = 0;
	const Char* pfile = reinterpret_cast<const Char*>(m_fileInterface.pfnLoadFile(scriptPath.c_str(), &fileSize));
	if(!pfile)
	{
		m_errorString << "Failed to load UI schema script '" << scriptPath.c_str() << "'";
		return nullptr;
	}

	// Allocate new object
	ui_schemeinfo_t* pNew = new ui_schemeinfo_t;
	pNew->schemeName = pstrFilename;

	// Parse the contents
	CString token;
	CString line;

	const Char* pstr = pfile;
	while(pstr && *pstr && (pstr - pfile) < fileSize)
	{
		// Read in the first token
		CString objName;
		pstr = Common::Parse(pstr, objName);
		if(!pstr || objName.empty())
			break;

		// Scheme object we'll be processing
		ui_schemeobject_t newObject;
		newObject.typeName = objName;

		// Next token should be an opening bracket
		pstr = Common::Parse(pstr, token);
		if(!pstr || token.empty())
		{
			m_errorString << "Unexpected EOF on '" << scriptPath.c_str() << "'";
			m_fileInterface.pfnFreeFile(pfile);

			delete pNew;
			return nullptr;
		}

		// Make sure the script is valid
		if(qstrcmp(token, "{"))
		{
			m_errorString << "{ token expected '" << scriptPath.c_str() << "', got '" << token.c_str() << "' instead.";
			m_fileInterface.pfnFreeFile(pfile);

			delete pNew;
			return nullptr;
		}

		// Read in the fields, line by line
		while(pstr && *pstr && (pstr - pfile) < fileSize)
		{
			// Skip whitespaces
			while(*pstr && SDL_isspace(*pstr))
				pstr++;

			// Read in the entire line
			pstr = Common::ReadLine(pstr, line);
			if(line.empty())
				continue;

			// Read in the first token
			const Char* pstrl = Common::Parse(line.c_str(), token);
			if(token.empty())
			{
				m_errorString << "Unexpected EOF on '" << scriptPath.c_str() << "'";
				m_fileInterface.pfnFreeFile(pfile);

				delete pNew;
				return nullptr;
			}

			// Exit the loop
			if(!qstrcmp(token, "}"))
				break;
			
			if(!pstrl)
			{
				m_errorString << "Unexpected EOF on '" << scriptPath.c_str() << "'";
				m_fileInterface.pfnFreeFile(pfile);

				delete pNew;
				return nullptr;
			}

			// Read in the value
			CString value;
			pstrl = Common::Parse(pstrl, value);
			if(value.empty())
			{
				m_errorString << "Unexpected EOF on '" << scriptPath.c_str() << "'";
				m_fileInterface.pfnFreeFile(pfile);

				delete pNew;
				return nullptr;
			}

			// Determine field type
			CString textureName;
			if(!qstrcmp(token, "$default") 
				|| !qstrcmp(token, "$focus")
				|| !qstrcmp(token, "$clicked")
				|| !qstrcmp(token, "$disabled")
				|| !qstrcmp(token, "$newitem")
				|| !qstrcmp(token, "$highlighted"))
			{
				// If it's a texture resource, load it in
				CString texturePath;
				texturePath << "ui/" << value;

				// Load it in
				en_texture_t* ptexture = m_pfnLoadTexture(texturePath.c_str(), RS_WINDOW_LEVEL, TX_FL_NOMIPMAPS, nullptr);
				if(!ptexture)
					ptexture = m_pfnGetDummyTexture();

				// Assign it to the right place
				if(!qstrcmp(token, "$default"))
					newObject.defaultTexture = ptexture;
				else if(!qstrcmp(token, "$focus"))
					newObject.focusTexture = ptexture;
				else if(!qstrcmp(token, "$clicked"))
					newObject.clickTexture = ptexture;
				else if(!qstrcmp(token, "$disabled"))
					newObject.disabledTexture = ptexture;
				else if(!qstrcmp(token, "$newitem"))
					newObject.newItemTexture = ptexture;
				else if(!qstrcmp(token, "$highlighted"))
					newObject.highlightTexture = ptexture;

				if(!newObject.width)
					newObject.width = ptexture->width;
				if(!newObject.height)
					newObject.height = ptexture->height;
			}
			else if(!qstrcmp(token, "$width"))
				newObject.width = SDL_atoi(value.c_str());
			else if(!qstrcmp(token, "$height"))
				newObject.height = SDL_atoi(value.c_str());
			else
			{
				CString str;
				str << "Unknown field '" << token.c_str() << "' in '" << scriptPath.c_str() << "'";
				m_warningStringArray.push_back(str);
				break;
			}
		}

		// Add it to the object
		pNew->tabObjects.push_back(newObject);
	}

	// Add this scheme object to the array
	m_tabSchemeArray.push_back(pNew);
	m_fileInterface.pfnFreeFile(pfile);

	return pNew;
}

//=============================================
// @brief Loads in a schema file
//
// @param pstrFilename Name of the UI scheme file
// @return Pointer to scheme object
//=============================================
ui_windowdescription_t* CUISchemaManager::LoadWindowDescriptionFile( const Char* pstrWindowName, const Char* pstrFilename )
{
	// Try to find it in the cache first
	for(Uint32 i = 0; i < m_windowDescriptionArray.size(); i++)
	{
		if(!qstrcmp(m_windowDescriptionArray[i]->windowName, pstrWindowName))
			return m_windowDescriptionArray[i];
	}

	// Load in the file
	CString scriptPath;
	scriptPath << "scripts/ui/windows/" << pstrFilename;

	Uint32 fileSize = 0;
	const Char* pfile = reinterpret_cast<const Char*>(m_fileInterface.pfnLoadFile(scriptPath.c_str(), &fileSize));
	if(!pfile)
	{
		m_errorString << "Failed to load UI schema script %s.\n", scriptPath.c_str();
		return nullptr;
	}

	// Allocate new object
	ui_windowdescription_t* pNewWindowDesc = new ui_windowdescription_t;
	pNewWindowDesc->windowName = pstrWindowName;

	// Parse the contents
	CString token;
	CString line;

	const Char* pstr = pfile;
	while(pstr && *pstr && (pstr - pfile) < fileSize)
	{
		// Read in the object name
		CString objType;
		pstr = Common::Parse(pstr, objType);
		if(!pstr || objType.empty())
			break;

		ui_object_type_t type = UI_OBJECT_UNDEFINED;
		if(!qstrcmp(objType, "window"))
			type = UI_OBJECT_WINDOW;
		else if(!qstrcmp(objType, "button"))
			type = UI_OBJECT_BUTTON;
		else if(!qstrcmp(objType, "text"))
			type = UI_OBJECT_TEXT;
		else if(!qstrcmp(objType, "texttab"))
			type = UI_OBJECT_TEXT_TAB;
		else if(!qstrcmp(objType, "tab"))
			type = UI_OBJECT_TAB;
		else if(!qstrcmp(objType, "list"))
			type = UI_OBJECT_LIST;
		else if(!qstrcmp(objType, "tickbox"))
			type = UI_OBJECT_TICKBOX;
		else if(!qstrcmp(objType, "slider"))
			type = UI_OBJECT_SLIDER;
		else if(!qstrcmp(objType, "separator_h"))
			type = UI_OBJECT_SEPARATOR_H;
		else if(!qstrcmp(objType, "separator_v"))
			type = UI_OBJECT_SEPARATOR_V;
		else
		{
			m_errorString << "Unknown object type '" << objType << "' in '" << scriptPath << "'.";
			m_fileInterface.pfnFreeFile(pfile);

			delete pNewWindowDesc;
			return nullptr;
		}

		// Read in the object name
		CString objName;
		pstr = Common::Parse(pstr, objName);
		if(!pstr || objName.empty())
			break;

		// Read the bracket token
		pstr = Common::Parse(pstr, token);
		if(!pstr || token.empty())
		{
			m_errorString << "Unexpected EOF on '" << scriptPath << "'.";
			m_fileInterface.pfnFreeFile(pfile);

			delete pNewWindowDesc;
			return nullptr;
		}

		// Make sure the format is correct
		if(qstrcmp(token, "{"))
		{
			m_errorString << "Expected '{', got '" << token << "' in '" << scriptPath << "'.";
			m_fileInterface.pfnFreeFile(pfile);

			delete pNewWindowDesc;
			return nullptr;
		}
		
		// Create the new object
		ui_objectinfo_t newObject;
		newObject.objectName = objName;
		newObject.type = type;

		// Read in the parameters
		while(true)
		{
			// Skip whitespaces
			while(*pstr && SDL_isspace(*pstr))
				pstr++;

			// Read in the entire line
			pstr = Common::ReadLine(pstr, line);
			if(line.empty())
				continue;

			// Read in the first token
			const Char* pstrl = Common::Parse(line.c_str(), token);
			if(token.empty())
			{
				m_errorString << "Unexpected EOF on '" << scriptPath << "'.";
				m_fileInterface.pfnFreeFile(pfile);

				delete pNewWindowDesc;
				return nullptr;
			}

			// Exit the loop
			if(!qstrcmp(token, "}"))
				break;
			
			if(!pstrl)
			{
				m_errorString << "Unexpected EOF on '" << scriptPath << "'.";
				m_fileInterface.pfnFreeFile(pfile);

				delete pNewWindowDesc;
				return nullptr;
			}

			// $flags and $color values are handled specially
			if(!qstrcmp(token, "$textcolor"))
			{
				color32_t color;

				for(Uint32 i = 0; i < 4; i++)
				{
					// Read in the value
					CString value;
					pstrl = Common::Parse(pstrl, value);
					if(value.empty())
					{
						m_errorString << "Unexpected EOF on '" << scriptPath << "'.";
						m_fileInterface.pfnFreeFile(pfile);

						delete pNewWindowDesc;
						return nullptr;
					}

					if(!Common::IsNumber(value))
					{
						m_errorString << "Color field '" << token << "' read value '" << value << "' which is not a number.";
						m_fileInterface.pfnFreeFile(pfile);

						delete pNewWindowDesc;
						return nullptr;
					}

					color[i] = SDL_atoi(value.c_str());
				}

				newObject.textcolor = color;
			}
			else if(!qstrcmp(token, "$flags"))
			{
				newObject.flags = UIEL_FL_NONE;

				while(pstrl)
				{
					CString flag;
					pstrl = Common::Parse(pstrl, flag);
					
					if(!qstrcmp(flag, "fixed_width"))
						newObject.flags |= UIEL_FL_FIXED_W;
					else if(!qstrcmp(flag, "fixed_height"))
						newObject.flags |= UIEL_FL_FIXED_H;
					else if(!qstrcmp(flag, "align_left"))
						newObject.flags |= UIEL_FL_ALIGN_L;
					else if(!qstrcmp(flag, "align_right"))
						newObject.flags |= UIEL_FL_ALIGN_R;
					else if(!qstrcmp(flag, "align_top"))
						newObject.flags |= UIEL_FL_ALIGN_T;
					else if(!qstrcmp(flag, "align_bottom"))
						newObject.flags |= UIEL_FL_ALIGN_B;
					else if(!qstrcmp(flag, "align_center_horizontal"))
						newObject.flags |= UIEL_FL_ALIGN_CH;
					else if(!qstrcmp(flag, "align_center_vertical"))
						newObject.flags |= UIEL_FL_ALIGN_CV;
					else if(!qstrcmp(flag, "wrap_word"))
						newObject.flags |= UIEL_FL_WRAP_WORD;
					else if(!qstrcmp(flag, "fixed_xpos"))
						newObject.flags |= UIEL_FL_FIXED_XPOS;
					else if(!qstrcmp(flag, "fixed_ypos"))
						newObject.flags |= UIEL_FL_FIXED_YPOS;
					else if(!qstrcmp(flag, "ontop"))
						newObject.flags |= UIEL_FL_ONTOP;
					else if(!qstrcmp(flag, "scroller_reverse"))
						newObject.flags |= UIEL_FL_SCR_REVERSE;
					else if(!qstrcmp(flag, "expand_width"))
						newObject.flags |= UIEL_FL_EXPAND_W;
					else if(!qstrcmp(flag, "expand_height"))
						newObject.flags |= UIEL_FL_EXPAND_H;
					else if(!qstrcmp(flag, "noheader"))
						newObject.flags |= UIEL_FL_NO_HEADER;
					else if(!qstrcmp(flag, "hover_highlight"))
						newObject.flags |= UIEL_FL_HOVER_HIGHLIGHT;
					else if(!qstrcmp(flag, "no_bottom_border"))
						newObject.flags |= UIEL_FL_NO_BOTTOM_BORDER;
					else if(!qstrcmp(flag, "no_top_border"))
						newObject.flags |= UIEL_FL_NO_TOP_BORDER;
					else if(!qstrcmp(flag, "resolution_relative"))
						newObject.flags |= UIEL_FL_RESOLUTION_RELATIVE;
					else
						m_errorString << "Unknown flag '" << flag << "' in '" << scriptPath << "', discarding.";
				}
			}
			else
			{
				// Read in the value
				CString value;
				pstrl = Common::Parse(pstrl, value);
				if(value.empty())
				{
					m_errorString << "Unexpected EOF on '" << scriptPath << "'.";
					m_fileInterface.pfnFreeFile(pfile);

					delete pNewWindowDesc;
					return nullptr;
				}

				if(!qstrcmp(token, "$width"))
					newObject.width = SDL_atoi(value.c_str());
				else if(!qstrcmp(token, "$height"))
					newObject.height = SDL_atoi(value.c_str());
				else if(!qstrcmp(token, "$text"))
					newObject.text = value;
				else if(!qstrcmp(token, "$schema"))
					newObject.schema = value;
				else if(!qstrcmp(token, "$buttonschema"))
					newObject.buttonschema = value;
				else if(!qstrcmp(token, "$alpha"))
					newObject.alpha = SDL_atof(value.c_str());
				else if(!qstrcmp(token, "$insetx"))
					newObject.insetx = SDL_atoi(value.c_str());
				else if(!qstrcmp(token, "$insety"))
					newObject.insety = SDL_atoi(value.c_str());
				else if(!qstrcmp(token, "$originx"))
					newObject.originx = SDL_atoi(value.c_str());
				else if(!qstrcmp(token, "$originy"))
					newObject.originy = SDL_atoi(value.c_str());
				else if(!qstrcmp(token, "$titleinsetx"))
					newObject.title_insetx = SDL_atoi(value.c_str());
				else if(!qstrcmp(token, "$titleinsety"))
					newObject.title_insety = SDL_atoi(value.c_str());
				else if(!qstrcmp(token, "$textinset"))
					newObject.text_inset = SDL_atoi(value.c_str());
				else if(!qstrcmp(token, "$rowheight"))
					newObject.listrowheight = SDL_atoi(value.c_str());
				else if(!qstrcmp(token, "$postspacing"))
					newObject.listpostspacing = SDL_atoi(value.c_str());
				else if(!qstrcmp(token, "$title"))
					newObject.title = value;
				else if(!qstrcmp(token, "$dragger"))
					newObject.dragger = (!qstrcmp(value.c_str(), "true") ? true : false);
				else if(!qstrcmp(token, "$resizable"))
					newObject.resizable = (!qstrcmp(value.c_str(), "true") ? true : false);
				else if(!qstrcmp(token, "$textschema"))
					newObject.textschemaname = value;
				else if(!qstrcmp(token, "$minvalue"))
					newObject.minvalue = SDL_atof(value.c_str());
				else if(!qstrcmp(token, "$maxvalue"))
					newObject.maxvalue = SDL_atof(value.c_str());
				else if(!qstrcmp(token, "$markerdistance"))
					newObject.markerdistance = SDL_atof(value.c_str());
				else if(!qstrcmp(token, "$basescreenwidth"))
					newObject.basescreenwidth = SDL_atoi(value.c_str());
				else if(!qstrcmp(token, "$basescreenheight"))
					newObject.basescreenheight = SDL_atoi(value.c_str());
				else
				{
					CString str;
					str << "Unknown field '" << token << "' in '" << scriptPath << "'.";
					m_warningStringArray.push_back(str);
					continue;
				}
			}
		}

		// We need to set this for each object
		newObject.setScreenResolution(m_screenWidth, m_screenHeight);

		// Verify that we have valid sizes for non-text objects
		if(newObject.type != UI_OBJECT_TEXT && newObject.type != UI_OBJECT_TICKBOX)
		{
			if(newObject.type != UI_OBJECT_SEPARATOR_V && !newObject.width)
			{
				m_errorString << "Object '" << newObject.objectName << "' in '" << scriptPath << "' has no width, discarding.";
				continue;
			}
			if(newObject.type != UI_OBJECT_SEPARATOR_H && !newObject.height)
			{
				m_errorString << "Object '" << newObject.objectName << "' in '" << scriptPath << "' has no height, discarding.";
				continue;
			}
		}

		// Make sure sliders have the necessary values set
		if(newObject.type == UI_OBJECT_SLIDER)
		{
			if(!newObject.maxvalue)
			{
				m_errorString << "Object '" << newObject.objectName << "' in '" << scriptPath << "' has no maxvalue, discarding.";
				continue;
			}
			if(!newObject.markerdistance)
			{
				m_errorString << "Object '" << newObject.objectName << "' in '" << scriptPath << "' has no minvalue, discarding.";
				continue;
			}
		}

		// Check for valid row height
		if(newObject.type == UI_OBJECT_LIST)
		{
			if(!newObject.listrowheight)
			{
				m_errorString << "Object '" << newObject.objectName << "' in '" << scriptPath << "' has no row height set, discarding.";
				continue;
			}
		}

		// Load any custom fonts
		if(!newObject.textschemaname.empty())
		{
			newObject.pfont = m_pfnGetSchemaFontSet(newObject.textschemaname.c_str());
			if(!newObject.pfont)
			{
				CString str;
				str << "Object '" << newObject.objectName << "' in '" << scriptPath << "' - Text schema '" << newObject.textschemaname << "' not found.";
				m_warningStringArray.push_back(str);
				newObject.pfont = m_pDefaultFontSet;
			}
		}
		else
		{
			// Set default
			newObject.pfont = m_pDefaultFontSet;
		}

		if(!newObject.alpha)
			newObject.alpha = 255;
		else if(newObject.alpha > 255)
			newObject.alpha = 255;

		pNewWindowDesc->objectsArray.push_back(newObject);
	}

	// Add it to the list
	m_windowDescriptionArray.push_back(pNewWindowDesc);

	m_fileInterface.pfnFreeFile(pfile);
	return pNewWindowDesc;
}

//=============================================
// @brief
//
//=============================================
Uint32 CUISchemaManager::GetNbWarnings( void ) const
{
	return m_warningStringArray.size();
}

//=============================================
// @brief
//
//=============================================
CString CUISchemaManager::GetWarning( Uint32 index ) const
{
	if(index > m_warningStringArray.size())
		return CString();
	else
		return m_warningStringArray[index];
}

//=============================================
// @brief
//
//=============================================
void CUISchemaManager::ClearWarnings( void )
{
	if(m_warningStringArray.empty())
		return;

	m_warningStringArray.clear();
}

//=============================================
// @brief
//
//=============================================
const CString& CUISchemaManager::GetErrorString( void ) const
{
	return m_errorString;
}

//=============================================
// @brief
//
//=============================================
void CUISchemaManager::SetScreenResolution( Uint32 screenWidth, Uint32 screenHeight )
{
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// Go through schemas and set there as well
	for(Uint32 i = 0; i < m_windowDescriptionArray.size(); i++)
	{
		ui_windowdescription_t* pschema = m_windowDescriptionArray[i];

		for(Uint32 j = 0; j < pschema->objectsArray.size(); i++)
		{
			ui_objectinfo_t* pobject = &pschema->objectsArray[i];
			pobject->setScreenResolution(screenWidth, screenHeight);
		}
	}
}