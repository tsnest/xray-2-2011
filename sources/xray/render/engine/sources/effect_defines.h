////////////////////////////////////////////////////////////////////////////
//	Created		: 08.02.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef BLENDER_DEFINES_H_INCLUDED
#define BLENDER_DEFINES_H_INCLUDED

namespace xray {
namespace render_dx10 {


// Main effects for level

enum enum_effects{
B_NO_NAME						= 0,
B_STANDART_BLENDERS_START		= 10,
B_DEFAULT,
B_DEFAULT_AREF,
B_VERT,			
B_VERT_AREF,		
B_LmBmmD,		
B_LaEmB,
B_LmEbB,
B_B,
B_BmmD,

B_DETAIL,
B_TREE,
B_TERRAIN,
B_TEST,

B_PARTICLE,

// Screen space effects
B_SCREEN_SPACE_BLENDERS_START	= 100,
B_SCREEN_SET,
B_SCREEN_GRAY,

B_SYSTEM_BLENDERS_START			= 300,
B_LIGHT,
B_BLUR,
B_SHADOW_TEX,
B_SHADOW_WORLD,

B_MODEL,
B_MODEL_EbB,

// Editor
B_EDITOR_BLENDERS_START			= 400,
B_EDITOR_WIRE,
B_EDITOR_SEL,

B_FORSE_DWORD_ = 0xFFFFFFFF
};

/*
#define		B_DEFAULT		MK_CLSID('L','M',' ',' ',' ',' ',' ',' ')
#define		B_DEFAULT_AREF	MK_CLSID('L','M','_','A','R','E','F',' ')
#define		B_VERT			MK_CLSID('V',' ',' ',' ',' ',' ',' ',' ')
#define		B_VERT_AREF		MK_CLSID('V','_','A','R','E','F',' ',' ')
#define		B_LmBmmD		MK_CLSID('L','m','B','m','m','D',' ',' ')
#define		B_LaEmB			MK_CLSID('L','a','E','m','B',' ',' ',' ')
#define		B_LmEbB			MK_CLSID('L','m','E','b','B',' ',' ',' ')
#define		B_B				MK_CLSID('B','m','m','D',' ',' ',' ',' ')
#define		B_BmmD			MK_CLSID('B','m','m','D','o','l','d',' ')

#define		B_DETAIL		MK_CLSID('D','_','S','T','I','L','L',' ')
#define		B_TREE			MK_CLSID('D','_','T','R','E','E',' ',' ')
#define		B_TERRAIN		MK_CLSID('D','_','T','E','R','R','A','I')
#define		B_TEST			MK_CLSID('D','_','T','E','S','T',' ',' ')

#define		B_PARTICLE		MK_CLSID('P','A','R','T','I','C','L','E')

// Screen space effects
#define		B_SCREEN_SET	MK_CLSID('S','_','S','E','T',' ',' ',' ')
#define		B_SCREEN_GRAY	MK_CLSID('S','_','G','R','A','Y',' ',' ')

#define		B_LIGHT			MK_CLSID('L','I','G','H','T',' ',' ',' ')
#define		B_BLUR			MK_CLSID('B','L','U','R',' ',' ',' ',' ')
#define		B_SHADOW_TEX	MK_CLSID('S','H','_','T','E','X',' ',' ')
#define		B_SHADOW_WORLD	MK_CLSID('S','H','_','W','O','R','L','D')

#define		B_MODEL			MK_CLSID('M','O','D','E','L',' ',' ',' ')
#define		B_MODEL_EbB		MK_CLSID('M','O','D','E','L','E','b','B')

// Editor
#define		B_EDITOR_WIRE	MK_CLSID('E','_','W','I','R','E',' ',' ')
#define		B_EDITOR_SEL	MK_CLSID('E','_','S','E','L',' ',' ',' ')
*/

// Editor visible effects list item
struct item_editor_effect
{
	class_id		id;
	char const*		name;
};

// Editor visible effects list
item_editor_effect enum_editor_effects[] = 
{
	{B_DEFAULT,			"B_DEFAULT"},
	{B_DEFAULT_AREF,	"B_DEFAULT_AREF"},
	{B_VERT,			"B_VERT"},
	{B_VERT_AREF,		"B_VERT_AREF"},
	{B_LmBmmD,			"B_LmBmmD"},	
	{B_LaEmB,			"B_LaEmB"},			
	{B_LmEbB,			"B_LmEbB"},			
	{B_B,				"B_B"},			
	{B_BmmD,			"B_BmmD"},
	{B_DETAIL,			"B_DETAIL"},
	{B_TREE,			"B_TREE"},
	{B_TERRAIN,			"B_TERRAIN"},
	{B_TEST,			"B_TEST"},

	{B_EDITOR_WIRE,		"B_EDITOR_WIRE"},
	{B_EDITOR_SEL,		"B_EDITOR_SEL"},
};



}//	namespace render 
}//	namespace xray 

#endif // #ifndef BLENDER_DEFINES_H_INCLUDED