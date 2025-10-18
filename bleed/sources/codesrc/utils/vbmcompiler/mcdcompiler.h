/*
===============================================
Pathos Engine - Created by Andrew Stephen "Overfloater" Lucas

Copyright 2016
All Rights Reserved.
===============================================
*/

#ifndef MCDCOMPILER_H
#define MCDCOMPILER_H

#include <map>
#include <set>

#include "compiler_types.h"
#include "studiocompiler.h"

/*
=======================
CMCDCompiler

=======================
*/
class CMCDCompiler
{
public:
	// File buffer allocation size
	static const Uint32 MCD_FILEBUFFER_ALLOC_SIZE;

public:
	CMCDCompiler( CStudioModelCompiler& studioCompiler );
	~CMCDCompiler( void );

public:
	// Processes and writes the MCD file
	bool CreateMCDFile( void );
	// Clears any data used by the class
	void Clear( void );

	// Returns a texture skinref for a texture name
	Int32 GetTextureIndex( const Char* pstrTextureName );

private:
	// Creates clipping hull data from triangle data
	bool CreateSubmodelClippingHulls( void );

private:
	// Studiomodel compiler object
	CStudioModelCompiler& m_studioCompiler;

	// Bone transforms array
	CArray<smdl::bone_transforminfo_t> m_boneTransformInfoArray;
	// Array of submodels
	CArray<mcd::submodel_t*> m_pSubmodelsArray;
	// Bodyparts array
	CArray<mcd::bodypart_t*> m_pBodyPartsArray;
	// Array of textures
	CArray<CString> m_texturesArray;

	// Current submodel
	mcd::submodel_t* m_pSubModel;
	// Submodel windings array
	CArray<CWinding*> m_submodelWindingsArray;
	// Submodel vertex array holding vertices in final positions
	CArray<Vector> m_submodelVertexArray;

	// Clipping hull sizes
	Vector m_clipHullMins[MAX_MAP_HULLS][2];
	// Clipping hull sizes
	Vector m_clipHullMaxs[MAX_MAP_HULLS][2];

	// File buffer for writing the MCD file
	CBuffer* m_pFileBuffer;
};
#endif //MCDCOMPILER_H