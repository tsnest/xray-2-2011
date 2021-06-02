////////////////////////////////////////////////////////////////////////////
//	Created		: 12.02.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright ( C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "shader_manager.h"

#include "manager_common_inline.h"
#include "blender.h"
#include "blender_compiler.h"
#include "manager_common_inline.h"

#include "blender_deffer.h"
#include "blender_deffer_materials.h"
#include "blender_deffer_aref.h"
#include "blender_tree.h"
#include "blender_terrain.h"
#include "blender_forward_system.h"

namespace xray {
namespace render {


bool cmp_tl( const std::pair<u32,ref_texture>& lhs, const std::pair<u32,ref_texture>& rhs)
{
	return lhs.first < rhs.first;
}

shader_manager::shader_manager(): m_loading_incomplete( false)
{
	//load_blenders();
	init_blenders();
}

shader_manager::~shader_manager()
{
	map_blenders_it it  = m_blenders.begin(),
		end = m_blenders.end();

	for( ; it != end; ++it)
	{
		DELETE( it->second);
	}
	m_blenders.clear();
}

void shader_manager::init_blenders()
{
	m_blenders.insert( mk_pair( fixed_string<128>( "def_shaders\\def_aref"),			NEW( blender_deffer_aref)( true)));
	m_blenders.insert( mk_pair( fixed_string<128>( "def_shaders\\def_aref_v"),			NEW( blender_deffer_aref)( false)));
	m_blenders.insert( mk_pair( fixed_string<128>( "def_shaders\\def_objects_lod"),		NEW( blender_deffer_materials)(3)));//NEW( blender_tree)));
	m_blenders.insert( mk_pair( fixed_string<128>( "def_shaders\\def_trans"),			NEW( blender_deffer_aref)( true)));
	m_blenders.insert( mk_pair( fixed_string<128>( "def_shaders\\def_trans_v"),			NEW( blender_deffer_aref)( false)));
	m_blenders.insert( mk_pair( fixed_string<128>( "def_shaders\\def_trans_v_tuchi"),	NEW( blender_deffer_aref)( false)));
	m_blenders.insert( mk_pair( fixed_string<128>( "def_shaders\\def_vertex"),			NEW( blender_deffer_materials)(3)));
	m_blenders.insert( mk_pair( fixed_string<128>( "def_shaders\\lod_old"),				NEW( blender_deffer_aref)( false)));
	m_blenders.insert( mk_pair( fixed_string<128>( "default"),							NEW( blender_deffer_materials)(3)));
	m_blenders.insert( mk_pair( fixed_string<128>( "effects\\lightplanes"),				NEW( blender_deffer_materials)(3)));
	m_blenders.insert( mk_pair( fixed_string<128>( "flora\\leaf_wave"),					NEW( blender_deffer_materials)(3)));//NEW( blender_tree)));
	m_blenders.insert( mk_pair( fixed_string<128>( "flora\\trunk_wave"),				NEW( blender_deffer_materials)(3)));//NEW( blender_tree)));
	m_blenders.insert( mk_pair( fixed_string<128>( "selflight"),						NEW( blender_deffer_materials)(3)));
	m_blenders.insert( mk_pair( fixed_string<128>( "#terrain"),							NEW( blender_terrain)));
	m_blenders.insert( mk_pair( fixed_string<128>( "#forward_system"),					NEW( blender_forward_system)));//NEW( blender_tree)));

	// Blenders by texture 
	m_blenders_by_texture.insert( mk_pair( fixed_string<128>( "grnd/grnd_zemlya"),		NEW( blender_deffer_materials)(1)));
	m_blenders_by_texture.insert( mk_pair( fixed_string<128>( "briks/briks_br2"),		NEW( blender_deffer_materials)(1)));
	m_blenders_by_texture.insert( mk_pair( fixed_string<128>( "veh/veh_traktor_br"),	NEW( blender_deffer_materials)(1)));
	m_blenders_by_texture.insert( mk_pair( fixed_string<128>( "veh/veh_tanchik"),		NEW( blender_deffer_materials)(1)));
	m_blenders_by_texture.insert( mk_pair( fixed_string<128>( "veh/veh_zil"),			NEW( blender_deffer_materials)(1)));
	m_blenders_by_texture.insert( mk_pair( fixed_string<128>( "veh/veh_zaz968"),		NEW( blender_deffer_materials)(1)));
	
}

res_texture_list* shader_manager::create_texture_list( res_texture_list& tex_list)
{
	std::sort( tex_list.begin(), tex_list.end(), cmp_tl);
	for( u32 it = 0; it < m_texture_lists.size(); it++)
	{
		res_texture_list* base = m_texture_lists[it];
		if( tex_list.equal( *base))
			return base;
	}

	res_texture_list* lst = NEW( res_texture_list)( tex_list);
	lst->make_registered();
	m_texture_lists.push_back( lst);
	lst->set_id( m_texture_lists.size());
	return lst;
}

void shader_manager::delete_texture_list( const res_texture_list* tex_list)
{
	if( !tex_list->is_registered())
		return;

	if( reclaim_with_id( m_texture_lists, tex_list))
		return;

	LOG_ERROR( "!ERROR: Failed to find compiled list of textures");
}

res_pass* shader_manager::create_pass( const res_pass& pass)
{
	for( u32 it=0; it<m_passes.size(); it++)
		if( m_passes[it]->equal( pass))
			return m_passes[it];

	res_pass*	new_pass = NEW( res_pass)( pass.m_vs, pass.m_ps, pass.m_constants, pass.m_state, pass.m_tex_list);
	new_pass->make_registered();

	m_passes.push_back			( new_pass);
	return m_passes.back();
}

void shader_manager::delete_pass( const res_pass* pass)
{
	if( !pass->is_registered())
		return;

	if( reclaim( m_passes, pass))
		return;

	LOG_ERROR( "!ERROR: Failed to find compiled pass");
}

void fill_options( blender_compilation_options& opts, LPCSTR textures)
{
	if( !textures)
		return;

	u32 const length	= strings::length( textures );
	pstr const token	= ( pstr)ALLOCA( ( length+1)*sizeof( char) );
	pcstr next_token	= textures;
	while( next_token )  
	{
		next_token	= strings::get_token( next_token, token, length+1, ',' );
		opts.tex_list.push_back	( token );
	} 
}

blender* shader_manager::find_blender( LPCSTR name, LPCSTR texture /* "texture" fortesting only */ )
{
	// First find by texture name. This is temporary; fortesting only.
	map_blenders_it	it = m_blenders_by_texture.find( texture);

	if( it != m_blenders_by_texture.end())
		return it->second;


	// map_blenders_it
		it = m_blenders.find( name);

	if( it != m_blenders.end())
		return it->second;

	return 0;
}

res_shader*	shader_manager::create_shader( LPCSTR shader, LPCSTR textures)
{
	if( blender* b = find_blender( shader, textures))
	{
		return create_shader( b, shader, textures);
	}

	return 0;
}

res_shader*	shader_manager::create_shader( blender* desc, LPCSTR shader_name, LPCSTR textures)
{
	XRAY_UNREFERENCED_PARAMETER	( shader_name );
	
	blender_compilation_options	opts;
	fill_options( opts, textures);

	res_shader			shader;
	blender_compiler	compiler( shader);
	desc->compile		( compiler, opts);

	desc->reset();
//	CBlender_Compile	C;
//	Shader				S;
//
//	//.
//	// if( strstr( s_shader,"transparent"))	__asm int 3;
//
//	// Access to template
//	C.BT				= B;
//	C.bEditor			= FALSE;
//	C.bDetail			= FALSE;
//#ifdef _EDITOR
//	if( !C.BT)			{ ELog.Msg( mtError,"Can't find shader '%s'",s_shader); return 0; }
//	C.bEditor			= TRUE;
//#endif
//
//	// Parse names
//	_ParseList			( C.L_textures,	s_textures	);
//	_ParseList			( C.L_constants,	s_constants	);
//	_ParseList			( C.L_matrices,	s_matrices	);
//
//	// Compile element	( LOD0 - HQ)
//	{
//		C.iElement			= 0;
//		C.bDetail			= m_textures_description.GetDetailTexture( C.L_textures[0],C.detail_texture,C.detail_scaler);
//		//.		C.bDetail			= _GetDetailTexture( *C.L_textures[0],C.detail_texture,C.detail_scaler);
//		ShaderElement		E;
//		C._cpp_Compile		( &E);
//		S.E[0]				= _CreateElement	( E);
//	}
//
//	// Compile element	( LOD1)
//	{
//		C.iElement			= 1;
//		//.		C.bDetail			= _GetDetailTexture( *C.L_textures[0],C.detail_texture,C.detail_scaler);
//		C.bDetail			= m_textures_description.GetDetailTexture( C.L_textures[0],C.detail_texture,C.detail_scaler);
//		ShaderElement		E;
//		C._cpp_Compile		( &E);
//		S.E[1]				= _CreateElement	( E);
//	}
//
//	// Compile element
//	{
//		C.iElement			= 2;
//		C.bDetail			= FALSE;
//		ShaderElement		E;
//		C._cpp_Compile		( &E);
//		S.E[2]				= _CreateElement	( E);
//	}
//
//	// Compile element
//	{
//		C.iElement			= 3;
//		C.bDetail			= FALSE;
//		ShaderElement		E;
//		C._cpp_Compile		( &E);
//		S.E[3]				= _CreateElement	( E);
//	}
//
//	// Compile element
//	{
//		C.iElement			= 4;
//		C.bDetail			= TRUE;	//.$$$ HACK :)
//		ShaderElement		E;
//		C._cpp_Compile		( &E);
//		S.E[4]				= _CreateElement	( E);
//	}
//
//	// Compile element
//	{
//		C.iElement			= 5;
//		C.bDetail			= FALSE;
//		ShaderElement		E;
//		C._cpp_Compile		( &E);
//		S.E[5]				= _CreateElement	( E);
//	}
//
	// Search equal in shaders array
	for( u32 it = 0; it < m_shaders.size(); ++it)
		if( shader.equal( m_shaders[it]))
			return m_shaders[it];

	// Create _new_ entry
	res_shader*		new_shader = NEW( res_shader);//( shader);

	new_shader->m_cur_technique = shader.m_cur_technique;
	new_shader->m_sh_techniques = shader.m_sh_techniques;

	new_shader->make_registered();
	m_shaders.push_back( new_shader);

	return new_shader;
}

void shader_manager::delete_shader( res_shader* shader)
{
	if( !shader->is_registered())
		return;

	if( reclaim( m_shaders, shader))
		return;

	LOG_ERROR( "!ERROR: Failed to find complete shader");

}

res_shader_technique* shader_manager::create_shader_technique( const res_shader_technique& element)
{
	if( element.m_passes.empty())
		return 0;

	// Search equal in shaders array
	for( u32 it = 0; it < m_sh_techniques.size(); ++it)
		if( element.equal( m_sh_techniques[it]))
			return m_sh_techniques[it];

	// Create _new_ entry
	res_shader_technique*	new_technique = NEW( res_shader_technique); //( element);

	new_technique->m_flags = element.m_flags;
	new_technique->m_passes = element.m_passes;

	new_technique->make_registered();
	m_sh_techniques.push_back( new_technique);
	return new_technique;
}

void shader_manager::delete_shader_technique( const res_shader_technique* technique)
{
	if( !technique->is_registered())
		return;

	if( reclaim( m_sh_techniques, technique))
		return;

	LOG_ERROR( "!ERROR: Failed to find complete shader");
}

template<class T>
class pred_equal_ptr
{
	T*	m_ptr;
public:
	pred_equal_ptr( T* value): m_ptr( value) {}
	bool operator()( const T* value) {return m_ptr->equal( *value);}
};

res_geometry* shader_manager::create_geometry( DWORD fvf, IDirect3DVertexBuffer9* vb, IDirect3DIndexBuffer9* ib)
{
	ref_declaration dcl	= resource_manager::get_ref().create_decl( fvf);
	//u32 vb_stride		= D3DXGetDeclVertexSize	( decl,0);

	return create_geometry( dcl, vb, ib);
}

res_geometry* shader_manager::create_geometry( D3DVERTEXELEMENT9* decl, IDirect3DVertexBuffer9* vb, IDirect3DIndexBuffer9* ib)
{
	R_ASSERT( decl);

	ref_declaration dcl	= resource_manager::get_ref().create_decl( decl);
	//u32 vb_stride		= D3DXGetDeclVertexSize	( decl,0);

	return create_geometry( dcl, vb, ib);
}

res_geometry* shader_manager::create_geometry( ref_declaration dcl, IDirect3DVertexBuffer9* vb, IDirect3DIndexBuffer9* ib)
{
	R_ASSERT( vb);

	u32 vb_stride = dcl->get_stride( 0);
	res_geometry g( vb, ib, dcl, vb_stride);

	vec_geoms_it	it = std::find_if( m_geometries.begin(), m_geometries.end(),
									  pred_equal_ptr<res_geometry>( &g));
	vec_geoms_it	end = m_geometries.end();

	if( it != end)
	{
		return *it;
	}

	res_geometry* geom = NEW( res_geometry)( vb, ib, dcl, vb_stride);
	geom->make_registered();
	m_geometries.push_back( geom);
	
	return	geom;
}

void shader_manager::delete_geometry( res_geometry* geom)
{
	if( !geom->is_registered())
		return;

	if( reclaim( m_geometries, geom))
		return;

	LOG_ERROR( "!ERROR: Failed to find complete shader");
}


blender* shader_manager::make_blender( u64 cls)
{
	switch ( cls)
	{
	case B_DEFAULT:
	case B_VERT:
		return NEW( blender_deffer);
	case B_DEFAULT_AREF:
		return NEW( blender_deffer_aref)( true);
	case B_VERT_AREF:
		return NEW( blender_deffer_aref)( false);
	case B_TREE:
		return NEW( blender_tree);	
	case B_TERRAIN:
		return NEW( blender_terrain);	

	}

	return 0;
}


void shader_manager::load_blenders()
{
	resources::query_resource_and_wait( "resources/shaders.xr", 
		resources::raw_data_class, 
		boost::bind(& shader_manager::load_raw_file, this, _1),
		::xray::render::g_allocator);
}

void shader_manager::load_raw_file( resources::queries_result& data)
{
	ASSERT( data.is_successful());

	resources::pinned_ptr_const<u8>	ptr(data[0].get_managed_resource());
	memory::chunk_reader	base_reader( ptr.c_ptr(), ptr.size(), memory::chunk_reader::chunk_type_sequential);

	if( base_reader.chunk_exists( 2))
	{
		memory::chunk_reader ch_it = base_reader.open_chunk_reader( 2);
		u32 chunk_id = 0;

		while ( ch_it.chunk_exists( chunk_id))
		{
			memory::reader in = ch_it.open_reader( chunk_id);

			blender_desc	desc;
			in.r			( &desc, sizeof( desc), sizeof( desc));

			blender* effect_code = make_blender( desc.m_cls);

			if	( effect_code == 0)
			{
				//LOG_INFO( "!Renderer doesn't support blender '%s'", desc.m_name);
			}
			else
			{

				char buff[9];

				//*( ( class_id*)&buff) = desc.m_cls;
				buff[0] = ( ( char*)&( desc.m_cls))[7];
				buff[1] = ( ( char*)&( desc.m_cls))[6];
				buff[2] = ( ( char*)&( desc.m_cls))[5];
				buff[3] = ( ( char*)&( desc.m_cls))[4];
				buff[4] = ( ( char*)&( desc.m_cls))[3];
				buff[5] = ( ( char*)&( desc.m_cls))[2];
				buff[6] = ( ( char*)&( desc.m_cls))[1];
				buff[7] = ( ( char*)&( desc.m_cls))[0];
				buff[8] = 0;

				LOG_INFO( "Blender pair - > %s, %s", buff, desc.m_name);
				//if	( B->getDescription().version != desc.version)
				//{
				//	Msg( "! Version conflict in shader '%s'",desc.cName);
				//}

				in.rewind();
				effect_code->load( in);

				std::pair<map_blenders_it, bool> I =  m_blenders.insert( mk_pair( fixed_string<128>( desc.m_name), effect_code));
				R_ASSERT( I.second && "shader.xr - found duplicate name!!!");
			}

			++chunk_id;
		}
	}

	m_loading_incomplete = false;
}



} // namespace render 
} // namespace xray 
