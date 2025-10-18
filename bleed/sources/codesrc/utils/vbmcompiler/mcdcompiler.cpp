/*
===============================================
Pathos Engine - Created by Andrew Stephen "Overfloater" Lucas

Copyright 2016
All Rights Reserved.
===============================================
*/

#include <SDL.h>
#ifdef WIN32
#include <Windows.h>
#endif

#include "includes.h"
#include "studiocompiler.h"
#include "mcdcompiler.h"
#include "options.h"
#include "main.h"
#include "compiler_math.h"
#include "refframesmdparser.h"
#include "filefuncs.h"
#include "collisionsmdparser.h"
#include "mcdformat.h"
#include "winding.h"

// TODO: Sanity check to make sure that a loaded collison smd has NO holes.
// Collision meshes CANNOT have triangles without a neighbor on every edge,
// that is considered invalid.

// File buffer allocation size
const Uint32 CMCDCompiler::MCD_FILEBUFFER_ALLOC_SIZE = 1024*1024;

//===============================================
// @brief Constructor for CMCDCompiler class
//
// @param studioCompiler reference to studiomodel compiler object
//===============================================
CMCDCompiler::CMCDCompiler( CStudioModelCompiler& studioCompiler ):
	m_studioCompiler(studioCompiler),
	m_pSubModel(nullptr),
	m_pFileBuffer(nullptr)
{
	// Set default hull sizes
	m_clipHullMins[HULL_POINT][0] = ZERO_VECTOR;
	m_clipHullMaxs[HULL_POINT][1] = ZERO_VECTOR;

	m_clipHullMins[HULL_HUMAN][0] = VEC_HULL_MIN;
	m_clipHullMaxs[HULL_HUMAN][1] = VEC_HULL_MAX;

	m_clipHullMins[HULL_LARGE][0] = Vector(-32, -32, -32);
	m_clipHullMaxs[HULL_LARGE][1] = Vector(32, 32, 32);

	m_clipHullMins[HULL_SMALL][0] = VEC_DUCK_HULL_MIN;
	m_clipHullMaxs[HULL_SMALL][1] = VEC_DUCK_HULL_MAX;
}

//===============================================
// @brief Destructor for CMCDCompiler class
//
//===============================================
CMCDCompiler::~CMCDCompiler( void )
{
	Clear();
}

//===============================================
// @brief Clears any data used by the class
//
//===============================================
void CMCDCompiler::Clear( void )
{
}

//===============================================
// @brief Processes and writes the MCD file
//
//===============================================
bool CMCDCompiler::CreateMCDFile( void )
{
	// Build up the bodyparts array exactly like in studiocompiler
	Uint32 nbbodyparts = m_studioCompiler.GetNbBodyParts();
	m_pBodyPartsArray.resize(nbbodyparts);

	for(Uint32 i = 0; i < nbbodyparts; i++)
	{
		const smdl::bodypart_t* psrcbodypart = m_studioCompiler.GetBodyPart(i);
		mcd::bodypart_t* pdestbodypart = new mcd::bodypart_t();
		m_pBodyPartsArray[i] = pdestbodypart;

		pdestbodypart->base = psrcbodypart->base;
		pdestbodypart->name = psrcbodypart->name;

		// Reserve the amount we'll need
		pdestbodypart->psubmodels.resize(psrcbodypart->psubmodels.size());
		for(Uint32 j = 0; j < psrcbodypart->psubmodels.size(); j++)
		{
			smdl::submodel_t* psrcsubmodel = psrcbodypart->psubmodels[j];
			mcd::submodel_t* pdestsubmodel = new mcd::submodel_t();

			m_pSubmodelsArray.push_back(pdestsubmodel);
			pdestbodypart->psubmodels.push_back(pdestsubmodel);

			if(psrcsubmodel->collisionsmdname.empty())
				continue;

			pdestsubmodel->name = psrcsubmodel->collisionsmdname;
			pdestsubmodel->reversetriangles = psrcsubmodel->reverseTriangles;

			// Parse the SMD in
			CCollisionSMDParser smdParser(m_studioCompiler, (*this), pdestsubmodel, pdestsubmodel->reversetriangles);
			if(!smdParser.ProcessFile(pdestsubmodel->name.c_str()))
				return false;
		}
	}

	// Set up bone transforms from the first frame of the first sequence
	m_boneTransformInfoArray.resize(m_studioCompiler.GetNbBones());

	const smdl::sequence_t* psequence = m_studioCompiler.GetSequence(0);
	const smdl::animation_t* panim = psequence->panims[0];
	for(Uint32 i = 0; i < panim->nodes.size(); i++)
	{
		smdl::bone_node_t& node = panim->nodes[i];
		Int32 boneindex = panim->bonemap[i];
		Int32 parentindex = panim->bonemap[node.parentindex];

		const Vector& posvalue = (*panim->pos_values[0])[0];
		const Vector& rotvalue = (*panim->rot_values[0])[0];

		CompilerMath::SetupBoneTransform(boneindex, parentindex, posvalue, rotvalue, m_boneTransformInfoArray);
	}

	// Process each submodel separately
	for(Uint32 i = 0; i < m_pSubmodelsArray.size(); i++)
	{
		m_pSubModel = m_pSubmodelsArray[i];
		if(!m_pSubModel->nbtriangles)
			continue;

		// Transform vertices into final positions
		if(m_submodelVertexArray.size() < m_pSubModel->vertexes.size())
			m_submodelVertexArray.resize(m_pSubModel->vertexes.size());

		for(Uint32 j = 0; j < m_pSubModel->vertexes.size(); j++)
		{
			const mcd::vertex_t& vertex = m_pSubModel->vertexes[j];
			smdl::bone_transforminfo_t& boneTransform = m_boneTransformInfoArray[vertex.boneindex];

			Math::VectorTransform(vertex.origin, boneTransform.matrix, m_submodelVertexArray[i]);
		}

		for(Uint32 j = 0; j < NB_MCD_COLLISION_TYPES; j++)
		{
			switch(j)
			{
			case MCD_COLLISION_CLIPPING_HULLS:
				{
					CreateSubmodelClippingHulls();
					break;
				}
			case MCD_COLLISION_TRIANGLES:
				{
					// Nothing to do here
					break;
				}
			}
		}
	}
}

//===============================================
// @brief Create windings for all submodels
//
//===============================================
bool CMCDCompiler::CreateSubmodelClippingHulls( void )
{
	return true;
}

//===============================================
// @brief Returns a texture skinref for a texture name
//
//===============================================
Int32 CMCDCompiler::GetTextureIndex( const Char* pstrTextureName )
{
	for(Uint32 i = 0; i < m_texturesArray.size(); i++)
	{
		if(!qstrcicmp(m_texturesArray[i], pstrTextureName))
			return i;
	}

	Int32 returnIndex = m_texturesArray.size();
	m_texturesArray.push_back(pstrTextureName);
	return returnIndex;
}