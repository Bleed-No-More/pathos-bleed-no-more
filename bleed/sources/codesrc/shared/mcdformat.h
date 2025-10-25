/*
===============================================
Pathos Engine - Created by Andrew Stephen "Overfloater" Lucas

Copyright 2016
All Rights Reserved.
===============================================
*/

#ifndef MCDFORMAT_H
#define MCDFORMAT_H

// MCD file id
static const Int32 MCD_FORMAT_HEADER = (('D'<<24)+('C'<<16)+('M'<<8)+'P');
// MCD file version
static const Int32 MCD_FORMAT_VERSION = 1;

enum mcdcollisiontype_t
{
	MCD_COLLISION_NULL = -1,
	MCD_COLLISION_TRIANGLES,		// The pure triangle data
	MCD_COLLISION_BVH,				// Bounding volume hierarchy information

	// Must be last
	NB_MCD_COLLISION_TYPES,
};

struct mcdbodypart_t
{
	mcdbodypart_t():
		base(0),
		submodelindex(0),
		numsubmodels(0)
	{
		memset(name, 0, sizeof(name));
	}

	Char name[128];

	Uint32 base;
	Uint32 submodelindex;
	Uint32 numsubmodels;
};

struct mcdsubmodel_t
{
	mcdsubmodel_t():
		collisiontypesoffset(0),
		numcollisiontypes(0)
	{
		memset(name, 0, sizeof(name));
	}

	Char name[128];

	Uint32 collisiontypesoffset;
	Uint32 numcollisiontypes;
};

struct mcdcollisiontypemodel_t
{
	mcdcollisiontypemodel_t():
		type(MCD_COLLISION_TRIANGLES),
		dataoffset(0)
	{}

	mcdcollisiontype_t type;
	Uint32 dataoffset;
};

struct mcdbone_t
{
	mcdbone_t():
		parentindex(NO_POSITION)
	{
		memset(name, 0, sizeof(name));
	}

	Char name[64];
	Vector position;
	Vector rotation;
	Int32 parentindex;
};

struct mcdvertex_t
{
	mcdvertex_t():
		boneindex(NO_POSITION)
	{}

	Vector origin;
	Int32 boneindex;
};

struct mcdtrimeshtriangle_t
{
	mcdtrimeshtriangle_t():
		skinref(NO_POSITION)
	{
		for(Uint32 i = 0; i < 3; i++)
			trivertexes[i] = 0;
	}

	Int32 skinref;
	Uint32 trivertexes[3];
};

struct mcdtrimeshtype_t
{
	mcdtrimeshtype_t():
		trianglesoffset(0),
		numtriangles(0),
		vertexoffset(0),
		numvertexes(0)
	{}

	Uint32 trianglesoffset;
	Uint32 numtriangles;

	Uint32 vertexoffset;
	Uint32 numvertexes;
};

struct mcdbvhtype_t
{
	mcdbvhtype_t():
		bvhnodeoffset(0),
		numbvhnodes(0)
	{}

	Uint32 bvhnodeoffset;
	Uint32 numbvhnodes;
};

struct mcdbvhnode_t
{
	mcdbvhnode_t():
		index(NO_POSITION),
		isleaf(false),
		triindexoffset(0),
		numtriangles(0)
	{
		for(Uint32 i = 0; i < 2; i++)
			children[i] = NO_POSITION;
	}

	Int32 index;
	Vector mins;
	Vector maxs;
	Int32 children[2];
	bool isleaf;

	Uint32 triindexoffset;
	Uint32 numtriangles;
};

struct mcdtexture_t
{
	mcdtexture_t():
		dataptr(0)
	{
		memset(name, 0, sizeof(name));
	}

	Char name[128];
	Uint64 dataptr;
};

struct mcdheader_t
{
	mcdheader_t():
		id(0),
		version(0),
		bodypartoffset(0),
		numbodyparts(0),
		textureoffset(0),
		numtextures(0),
		boneoffset(0),
		numbones(0),
		size(0)
	{
		memset(name, 0, sizeof(name));
	}

	Int32 id;
	Int32 version;

	Char name[256];

	Uint32 bodypartoffset;
	Uint32 numbodyparts;

	Uint32 textureoffset;
	Uint32 numtextures;

	Uint32 boneoffset;
	Uint32 numbones;

	Uint32 size;
};
#endif