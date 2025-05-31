/*
===============================================
Pathos Engine - Created by Andrew Stephen "Overfloater" Lucas

Copyright 2016
All Rights Reserved.
===============================================
*/

#ifndef GAMEUIDOCUMENTSWINDOW_H
#define GAMEUIDOCUMENTSWINDOW_H

#include "gameuielements.h"

enum documentsbuttonevent_t
{
	DOC_BUTTON_EXIT = 0,
	DOC_BUTTON_BACK,
	DOC_BUTTON_DOCUMENT_0
};

/*
====================
CGameUIDocumentsWindow

====================
*/
class CGameUIDocumentsWindow : public CGameUIWindow
{
public:
	struct textdocumentinfo_t
	{
		textdocumentinfo_t():
			titleColorWasSet(false),
			textColorWasSet(false)
		{
		}

		// File path
		CString filePath;
		// Document file path and optionally the code
		CString documentEntry;

		// Document title
		CString documentTitle;
		// Text file contents
		CString documentText;

		// Font set name
		CString titleTextSchema;
		// Text color
		color32_t titleTextColor;
		// TRUE if title color was set
		bool titleColorWasSet;
		// Font set name
		CString textSchema;
		// Text color
		color32_t textColor;
		// TRUE if text color was set
		bool textColorWasSet;
	};

	struct button_t
	{
		button_t():
			pButton(nullptr),
			pDocumentInfo(nullptr)
			{
			}

		// Pointer to button
		CGameUIButton* pButton;

		// Info about the document
		textdocumentinfo_t* pDocumentInfo;
	};

public:
	// Window description file
	static const Char DOCUMENTSWINDOW_DESC_FILE[];
	// Window object name
	static const Char DOCUMENTSWINDOW_OBJ_NAME[];
	// Title text object name
	static const Char DOCUMENTSWINDOW_TITLE_TEXT_OBJ_NAME[];
	// Upper separator object name
	static const Char DOCUMENTSWINDOW_UPPER_SEPARATOR_OBJ_NAME[];
	// Lower separator object name
	static const Char DOCUMENTSWINDOW_LOWER_SEPARATOR_OBJ_NAME[];
	// Document buttons tab object name
	static const Char DOCUMENTSWINDOW_BUTTON_TAB_OBJ_NAME[];
	// Exit button object name
	static const Char DOCUMENTSWINDOW_EXIT_BUTTON_OBJ_NAME[];
	// Reader title text object name
	static const Char DOCUMENTSWINDOW_READER_TITLE_TEXT_OBJ_NAME[];
	// Reader text tab object name
	static const Char DOCUMENTSWINDOW_READER_TEXT_TAB_OBJ_NAME[];
	// Reader back button object name
	static const Char DOCUMENTSWINDOW_READER_BACK_BTN_OBJ_NAME[];
	// Document button object name
	static const Char DOCUMENTSWINDOW_DOCUMENT_BUTTON_OBJ_NAME[];

public:
	CGameUIDocumentsWindow( Int32 flags, Int32 originX, Int32 originY, Uint32 width, Uint32 height );
	~CGameUIDocumentsWindow();

public:
	// Initializes the window
	bool init( const ui_windowdescription_t* pWindowDesc, const ui_objectinfo_t* pWindowObject ) override;

	// Initializes the data
	bool initData( const CArray<CString>& textFilesArray, const Char* pstrActiveFileName );
	// Returns the current window information
	void getInformation( CArray<CString>& textFilesArray, CString& pstrActiveFileName ) const;

	// Returns the type of the window
	virtual gameui_windows_t getWindowType( void ) const override { return GAMEUI_DOCUMENTSWINDOW; }

public:
	// Manages an event
	void ManageEvent( documentsbuttonevent_t event );
	// Sets the current active document
	void SetActiveDocument( textdocumentinfo_t* pDocument );

	// Creates instance of window
	static CGameUIDocumentsWindow* CreateInstance( void );

private:
	// Exit button
	CGameUIButton* m_pExitButton;
	// Array of buttons
	CArray<button_t> m_buttonsArray;

	// Title text object
	CGameUIText* m_pReaderTitleText;
	// Text tab object
	CGameUITextTab* m_pReaderTextTab;
	// Buttons tab object
	CGameUIScrollableSurface* m_pButtonsTab;

	// Array containing listing objects
	CArray<CGameUIObject*> m_pListingObjectsArray;
	// Array containing text reader objects
	CArray<CGameUIObject*> m_pTextReaderObjectsArray;

	// Array of text documents
	CArray<textdocumentinfo_t*> m_documentsArray;
	// Current document being read
	textdocumentinfo_t* m_pCurrentDocument;

	// Default title font set
	const font_set_t* m_pDefaultTitleFont;
	// Defautl title font color
	color32_t m_defaultTitleColor;

	// Default text font set
	const font_set_t* m_pDefaultTextFont;
	// Defautl text font color
	color32_t m_defaultTextColor;

	// Font set used
	const font_set_t* m_pFontSet;
};

/*
=================================
CGameUIDocumentsWindowButtonCallbackEvent

=================================
*/
class CGameUIDocumentsWindowButtonCallbackEvent : public CGameUICallbackEvent
{
public:
	CGameUIDocumentsWindowButtonCallbackEvent( CGameUIDocumentsWindow* pWindow, Uint32 type ):
		m_pWindow(pWindow),
		m_eventType((documentsbuttonevent_t)type)
	{ 
	};
	virtual ~CGameUIDocumentsWindowButtonCallbackEvent( void ) { };

public:
	// Performs the action
	virtual void PerformAction( Float param ) override;
	// Handles a special key event
	virtual bool KeyEvent( Int32 button, Int16 mod, bool keyDown ) override { return false; }
	// Handles a mouse button event
	virtual bool MouseButtonEvent( Int32 mouseX, Int32 mouseY, Int32 button, bool keyDown ) override { return false; }

private:
	// Text window object
	CGameUIDocumentsWindow* m_pWindow;
	// Event type
	documentsbuttonevent_t m_eventType;
};
#endif //GAMEUIDOCUMENTSWINDOW_H