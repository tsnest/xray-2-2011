////////////////////////////////////////////////////////////////////////////
//	Created		: 13.02.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef BLENDER_H_INCLUDED
#define BLENDER_H_INCLUDED

#include "manager_common_inline.h"
#include "blender_defines.h"
namespace xray {
namespace render {


class blender_compiler;
void jitter(blender_compiler& compiler);


#pragma pack(push, 4)


enum	xrProperties
{
	xrPID_MARKER	= 0,
	xrPID_MATRIX,		// really only name(stringZ) is written into stream
	xrPID_CONSTANT,		// really only name(stringZ) is written into stream
	xrPID_TEXTURE,		// really only name(stringZ) is written into stream
	xrPID_INTEGER,
	xrPID_FLOAT,
	xrPID_BOOL,
	xrPID_TOKEN,
	xrPID_CLSID,
	xrPID_OBJECT,		// really only name(stringZ) is written into stream
	xrPID_STRING,		// really only name(stringZ) is written into stream
	xrPID_MARKER_TEMPLATE,
	xrPID_FORCEDWORD=u32(-1)
};

struct	xrP_Integer
{
	int					value;
	int					min;
	int					max;

	xrP_Integer() : value(0), min(0), max(255)	{}
};

struct	xrP_Float
{
	float				value;
	float				min;
	float				max;

	xrP_Float()	: value(0), min(0), max(1)		{}
};

struct	xrP_BOOL
{
	BOOL				value;
	xrP_BOOL() : value(FALSE)					{}
};

struct	xrP_TOKEN
{
	struct Item {
		u32			ID;
		string64	str;
	};

	u32				IDselected;
	u32				Count;

	//--- elements:		(ID,string64)

	xrP_TOKEN()	: IDselected(0), Count(0)		{}
};

struct	xrP_CLSID
{
	class_id	Selected;
	u32			Count;
	//--- elements:		(...)

	xrP_CLSID()	: Selected(0), Count(0)			{}
};

struct	xrP_Template
{
	u32				Type;
	u32				Limit;
};

#pragma pack(pop)

u32	xrPREAD(memory::reader& mem_reader);
void xrPREAD_MARKER(memory::reader& mem_reader);

template<typename T>
void xrPREAD_PROP(memory::reader& mem_reader, u32 ID, T& data)
{
	R_ASSERT			(ID==xrPREAD(mem_reader));
	mem_reader.r		(&data,sizeof(data),sizeof(data));
	switch (ID)
	{
	case xrPID_TOKEN:	mem_reader.advance(((xrP_TOKEN*)&data)->Count * sizeof(xrP_TOKEN::Item));
		break;
	case xrPID_CLSID:	mem_reader.advance(((xrP_CLSID*)&data)->Count * sizeof(class_id));
		break;
	}
}

enum property_id_enum
{
	pid_material_name = 0,
	pid_use_bump,
	pid_use_detail,
	pid_use_steep_paralax,
	pid_lmap_name,
	pid_common_count
};

struct blender_compilation_options
{
	blender_compilation_options():use_detail_diffuse(false),use_detail_bump(false),use_steep_paralax(false),detail_texture(NULL){}
	sh_list	tex_list;
	//sh_list				L_constants;
	//sh_list				L_matrices;

	LPCSTR			detail_texture;
	//constant_setup*	detail_scaler;

	//BOOL			bEditor;
	//bool			bDetail;
	bool			use_detail_diffuse;//bDetail_Diffuse;
	bool			use_detail_bump;//bDetail_Bump;
	bool			use_steep_paralax;//bUseSteepParallax;
	//int					iElement;
};

#pragma pack(push, 4)

class blender_desc
{
public:
	class_id			m_cls;
	char				m_name[128];
	char				m_computer[32];
	u32					m_time;
	u16					m_version;

	blender_desc(): m_cls(class_id(0)), m_time(0), m_version(0)
	{
	}

	void setup(LPCSTR name);
};

#pragma  pack(pop)

class blender_compiler;

class blender: public utils::property_container<property_id_enum>
{
public:
	//protected:
	//	u32							BC				(BOOL v)		{ return v?0xff:0; }
	//	BOOL						c_XForm			();
	//public:
	//	static			IBlender*	Create			(CLASS_ID cls);
	//	static			void		Destroy			(IBlender*& B);
	//	static			void		CreatePalette	(xr_vector<IBlender*> & palette);
	//
	//	CBlender_DESC&				getDescription	()	{return description;}
	//	virtual 		LPCSTR		getName			()	{return description.cName;}
	//	virtual			LPCSTR		getComment		()	= 0;
	//
	//	virtual			BOOL		canBeDetailed	()	{ return FALSE; }
	//	virtual			BOOL		canBeLMAPped	()	= 0;
	//	virtual			BOOL		canUseSteepParallax	()	{ return FALSE; }
	//
	virtual	void load(memory::reader& mem_reader);
	//
	virtual	void compile(blender_compiler& compiler, const blender_compilation_options& options);
	//virtual	res_shader*	compile(/*blender_compiler& compiler, */const blender_compilation_options& options) = 0;
	//
	//	virtual			Flags32		GetOptions		() {Flags32 Flags; Flags.assign(0); return Flags;}
	//	virtual			Flags32		GetConfig		() {Flags32 Flags; Flags.assign(0); return Flags;}
	//

	blender() {}

	virtual ~blender() {}

protected:
	blender_desc	m_desc;			//description;
	int				m_priority;
	bool			m_strict_sort;
	char			m_ot_name[64];
	char			m_ot_xform[64];

}; // class blender

//enum BlenderOptionsEnum
//{
//	BO_USE_THM			=	(1<<0),
//	BO_USE_SKINNING		=	(1<<1),
//	BO_USE_LMH			=	(1<<2),
//	BO_USE_MSAA			=	(1<<3)
//};
//
//enum BlenderConfigEnum
//{
//	BC_FLAT		= (1 << 0),
//	BC_FLAT_D	= (1 << 1),
//	BC_BUMP		= (1 << 2),
//	BC_BUMP_D	= (1 << 3),
//	BC_BUMP_DB	= (1 << 4),
//	BC_STEEP	= (1 << 5),
//	BC_STEEP_D	= (1 << 6),
//	BC_STEEP_DB	= (1 << 7)
//};
//
//class ECORE_API IBlender	: public CPropertyBase
//{
//	friend class				CBlender_Compile;
//protected:
//	CBlender_DESC				description;
//	xrP_Integer					oPriority;
//	xrP_BOOL					oStrictSorting;
//	string64					oT_Name;
//	string64					oT_xform;
//protected:
//	u32							BC				(BOOL v)		{ return v?0xff:0; }
//	BOOL						c_XForm			();
//public:
//	static			IBlender*	Create			(CLASS_ID cls);
//	static			void		Destroy			(IBlender*& B);
//	static			void		CreatePalette	(xr_vector<IBlender*> & palette);
//
//	CBlender_DESC&				getDescription	()	{return description;}
//	virtual 		LPCSTR		getName			()	{return description.cName;}
//	virtual			LPCSTR		getComment		()	= 0;
//
//	virtual			BOOL		canBeDetailed	()	{ return FALSE; }
//	virtual			BOOL		canBeLMAPped	()	= 0;
//	virtual			BOOL		canUseSteepParallax	()	{ return FALSE; }
//
//	virtual			void		Save			(IWriter& fs);
//	virtual			void		Load			(IReader& fs, u16 version);
//
//	virtual			void		Compile			(CBlender_Compile& C);
//
//	virtual			Flags32		GetOptions		() {Flags32 Flags; Flags.assign(0); return Flags;}
//	virtual			Flags32		GetConfig		() {Flags32 Flags; Flags.assign(0); return Flags;}
//
//	IBlender();
//	virtual ~IBlender();
//};

} // namespace render 
} // namespace xray 


#endif // #ifndef BLENDER_H_INCLUDED