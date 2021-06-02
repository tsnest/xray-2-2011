////////////////////////////////////////////////////////////////////////////
//	Created 	: 28.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright ( C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/engine/world.h>
#include <xray/render/core/world.h>
#include <xray/render/core/resource_manager.h>
#include <xray/particle/api.h>
#include "system_renderer.h"
#include "terrain.h"
#include "material_manager.h"
#include "environment.h"
#include "stage_particles.h"
#include "engine_options.h"
#include "model_cooker.h"
#include "animated_model_instance_cook.h"
#include "render_model_cooker.h"
#include "terrain_cook.h"
#include "scene_manager.h"
#include "material_cook.h"
#include "scene_cook.h"
#include "scene_view_cook.h"
#include "render_output_window_cook.h"
#include "scene.h"
#include "scene_view.h"
#include "renderer.h"
#include <xray/render/core/backend.h>
#include <xray/render/core/effect_manager.h>
#include "renderer_context.h"
#include "render_model_user.h"
#include "render_model_skeleton.h"
#include <xray/render/core/effect_constant_storage.h>
#include <xray/render/core/custom_config.h>
#include <xray/render/core/res_effect.h>
#include <xray/particle/world.h>
#include <xray/render/facade/render_stage_types.h>
#include "scene.h"
#include <xray/render/core/res_geometry.h>
#include <xray/render/core/render_target.h>
#include <xray/render/engine/api.h>
#include "render_output_window.h"
#include "speedtree.h"
#include "speedtree_cook.h"
#include "speedtree_forest.h"
#include <xray/render/facade/ui_renderer.h>
#include <xray/render/facade/material_effects_instance_cook_data.h>
#include "material_effects_instance_cook.h"
#include "model_converter.h"
#include "decal_instance.h"

#include <xray/fs_watcher.h>

#include <xray/console_command.h>
#include <xray/console_command_processor.h>
#include <xray/render/core/options.h>

//#include "renderer_cook.h"

#pragma comment( lib,"dxgi.lib")

#if !USE_DX10
#	pragma comment( lib,"d3d11.lib")
#	pragma comment( lib,"d3dx11.lib")
#else
#	pragma comment( lib,"d3d10.lib")
#	pragma comment( lib,"d3dx10.lib")
#endif

#pragma comment( lib,"d3dx9.lib")

#pragma comment( lib,"d3dcompiler.lib")

namespace xray {
namespace render {

void register_texture_cook	( );
void unregister_texture_cook( );

static void register_cooks				( )
{
	using resources::register_cook;

	static terrain_mesh_cook			terrain_mesh_cooker;
	register_cook						( &terrain_mesh_cooker );

	static terrain_instance_cook		terrain_instance_cooker;
	register_cook						( &terrain_instance_cooker );

	static terrain_model_cook			terrain_model_cooker;
	register_cook						( &terrain_model_cooker );

	static user_mesh_cook				user_mesh_cooker;
	register_cook						( &user_mesh_cooker );


	
	static static_model_instance_cook	static_model_instance_cooker;
	register_cook						( &static_model_instance_cooker );

	static skeleton_model_instance_cook	skeleton_model_instance_cooker;
	register_cook						( &skeleton_model_instance_cooker );


	static render_model_cook			skeleton_mesh_instance_cooker( resources::skeleton_render_model_class );
	register_cook						( &skeleton_mesh_instance_cooker );
	
	static render_model_cook			render_model_class_cooker( resources::static_render_model_class );
	register_cook						( &render_model_class_cooker );


	static static_render_model_instance_cook static_render_model_instance_cooker;
	register_cook						( &static_render_model_instance_cooker );

	static skeleton_render_model_instance_cook	skeleton_render_model_instance_cooker;
	register_cook						( &skeleton_render_model_instance_cooker );
	
	static converted_model_cook	converted_model_cooker;
	register_cook						( &converted_model_cooker );

	static composite_render_model_instance_cook composite_render_model_instance_cooker;
	register_cook						( &composite_render_model_instance_cooker );

	static composite_render_model_cook composite_render_model_cooker;
	register_cook						( &composite_render_model_cooker );

	static material_cook				material_cook;
	register_cook						( &material_cook );

	static material_effects_instance_cook material_effects_instance_cooker;
	resources::register_cook(&material_effects_instance_cooker);

	register_texture_cook				( );

	particle::initialize				( );
	
	static scene_cook					scene_cook;
	register_cook						( &scene_cook );
	
	static scene_view_cook				scene_view_cook;
	register_cook						( &scene_view_cook );

 	static render_output_window_cook	render_output_window_cook;
 	register_cook						( &render_output_window_cook );

	static animated_model_instance_cook	animated_model_cook;
	register_cook						( &animated_model_cook );
}

static void unregister_cooks				( )
{
	xray::particle::finalize				( );
	xray::render::unregister_texture_cook	( );
}

} // namespace render
} // namespace xray

static void initialize_options		( )
{
	using namespace xray::render;
}

struct singletons_on_preinitialize {
	xray::render::resource_manager			resource_manager;
	xray::render::device					device;
	xray::render::backend					backend;
	xray::render::scene_manager				scene_manager;
	xray::render::shader_macros				shader_macros;
	xray::render::effect_manager			effect_manager;
	xray::render::effect_constant_storage	effect_constant_storage;
}; // struct singletons_on_preinitialize

struct singletons_on_initialize {
	xray::render::renderer_context						renderer_context;
	xray::render::material_manager						material_manager;
	xray::render::environment							environment;
	xray::render::particle_shader_constants				particle_shader_constants;
	xray::render::decal_shader_constants_and_geometry	decal_shader_constants_and_geometry;
}; //struct singletons

static xray::uninitialized_reference< xray::render::options >			s_options;

static xray::uninitialized_reference< singletons_on_preinitialize >		s_singletons_on_preinitialize;
static xray::uninitialized_reference< singletons_on_initialize >		s_singletons_on_initialize;
//static xray::uninitialized_reference< xray::render::renderer >			s_renderer;
static xray::uninitialized_reference< xray::render::system_renderer >	s_system_renderer;

namespace xray {
namespace render {

untyped_buffer_ptr		g_quad_ib;
untyped_buffer * create_quad_ib();

// Modified helper function from DirectX SDK
static HRESULT get_dx_version_via_dxdiag	(
		DWORD& major_version,
        DWORD& minor_version
	)
{
    HRESULT result;
    bool should_cleanup_COM = false;

    bool does_major_version_obtained = false;
    bool does_minor_version_obtained = false;

    // Init COM.  COM may fail if its already been inited with a different 
    // concurrency model.  And if it fails you shouldn't release it.
    result = CoInitialize( NULL );
    should_cleanup_COM = SUCCEEDED( result );

    // Get an IDxDiagProvider
    bool does_dx_version_obtained = false;
    IDxDiagProvider* dxdiag_provider = NULL;
    result = CoCreateInstance( CLSID_DxDiagProvider,
                           NULL,
                           CLSCTX_INPROC_SERVER,
                           IID_IDxDiagProvider,
                           ( LPVOID* )&dxdiag_provider );
    if( SUCCEEDED( result ) )
    {
        // Fill out a DXDIAG_INIT_PARAMS struct
        DXDIAG_INIT_PARAMS dxDiagInitParam;
        ZeroMemory( &dxDiagInitParam, sizeof( DXDIAG_INIT_PARAMS ) );
        dxDiagInitParam.dwSize = sizeof( DXDIAG_INIT_PARAMS );
        dxDiagInitParam.dwDxDiagHeaderVersion = DXDIAG_DX9_SDK_VERSION;
        dxDiagInitParam.bAllowWHQLChecks = false;
        dxDiagInitParam.pReserved = NULL;

        // Init the m_pDxDiagProvider
        result = dxdiag_provider->Initialize( &dxDiagInitParam );
        if( SUCCEEDED( result ) )
        {
            IDxDiagContainer* pDxDiagRoot = NULL;
            IDxDiagContainer* pDxDiagSystemInfo = NULL;

            // Get the DxDiag root container
            result = dxdiag_provider->GetRootContainer( &pDxDiagRoot );
            if( SUCCEEDED( result ) )
            {
                // Get the object called DxDiag_SystemInfo
                result = pDxDiagRoot->GetChildContainer( L"DxDiag_SystemInfo", &pDxDiagSystemInfo );
                if( SUCCEEDED( result ) )
                {
                    VARIANT var;
                    VariantInit( &var );

                    // Get the "dwDirectXVersionMajor" property
                    result = pDxDiagSystemInfo->GetProp( L"dwDirectXVersionMajor", &var );
                    if( SUCCEEDED( result ) && var.vt == VT_UI4 )
                    {
						major_version = var.ulVal;
                        does_major_version_obtained = true;
                    }
                    VariantClear( &var );

                    // Get the "dwDirectXVersionMinor" property
                    result = pDxDiagSystemInfo->GetProp( L"dwDirectXVersionMinor", &var );
                    if( SUCCEEDED( result ) && var.vt == VT_UI4 )
                    {
						minor_version = var.ulVal;
                        does_minor_version_obtained = true;
                    }
                    VariantClear( &var );

                    // If it all worked right, then mark it down
                    if( does_major_version_obtained && does_minor_version_obtained )
                        does_dx_version_obtained = true;

                    pDxDiagSystemInfo->Release();
                }

                pDxDiagRoot->Release();
            }
        }

        dxdiag_provider->Release();
    }

    if( should_cleanup_COM )
        CoUninitialize();

    if( does_dx_version_obtained )
        return S_OK;
    else
        return E_FAIL;
}


static bool does_os_support_dx11	( )
{
	OSVERSIONINFOEX OsVersionInfo;

	ZeroMemory( &OsVersionInfo , sizeof( OSVERSIONINFOEX ) );
	OsVersionInfo.dwOSVersionInfoSize = sizeof( OSVERSIONINFOEX );

	if ( GetVersionEx( (LPOSVERSIONINFO) &OsVersionInfo ) == 0 )
		return true; // failed to get OS version information for some reason

	if ( OsVersionInfo.dwMajorVersion > 6 )
		return true; // unknown OS greater than Win7

	if ( OsVersionInfo.dwMajorVersion == 6 )
	{
		 if ( OsVersionInfo.dwMinorVersion >= 1 )
			 return true; // Win7 + optional update
		 // Vista here
		 if ( OsVersionInfo.wServicePackMajor >= 2 )
		 {
			// SP2 or greater, check for DX11 ( KB971512 + KB971644 )
			DWORD major_version, minor_version;
			if ( SUCCEEDED( get_dx_version_via_dxdiag( major_version, minor_version ) ) )
			{
				if ( major_version >= 10 )
					return true;
			}
			else
				return true; // failed to get DX version for some reason
		 }			 
	}
	return false;
}

void engine::world::apply_render_options_changes( )
{
	reset_renderer								( false );
#ifndef MASTER_GOLD
	options::ref().save_current_configuration	( );
#endif // #ifndef MASTER_GOLD
}

engine::world::world						( ) :
	m_frame_id						( 0 ),
	m_initialized					( false ),
	m_enable_terrain_debug_mode		( false ),
	m_renderer						( NULL )
{
	CHECK_OR_EXIT							(
		does_os_support_dx11(),
		"Your operating system doesn't support DirectX 11.\r\n"
		"Please upgrade your OS to Windows Vista Service Pack 2 + Platform Update or later."
	);

	render::core::initialize		( );

	XRAY_CONSTRUCT_REFERENCE		( s_options, options )( );
	
	initialize_options				( );

	initialize_speedtree			( );

	register_cooks					( );

	XRAY_CONSTRUCT_REFERENCE		( s_singletons_on_preinitialize, singletons_on_preinitialize )( );
	
	// TODO
	material::initialize_nomaterial_material( );
}

void engine::world::set_renderer_configuration( fs_new::virtual_path_string const& config_name, bool async_effects )
{
	fs_new::virtual_path_string						path;
	path.assignf						("resources/render/%s.cfg", config_name.c_str());
	
	query_resource( 
		path.c_str(), 
		resources::binary_config_class, 
		boost::bind( &engine::world::on_renderer_config_loaded, this, async_effects, _1), 
		g_allocator
	);
}

void engine::world::on_renderer_config_loaded(bool async_effects, resources::queries_result& data)
{
	if (data.is_successful())
	{
		options::ref().load_from_config(
			xray::static_cast_resource_ptr<xray::configs::binary_config_ptr>(
				data[0].get_unmanaged_resource()
			)->get_root()["options"]
		);
	}
	reset_renderer					( async_effects );
}

void engine::world::reset_renderer( bool async_effects )
{
	LOG_INFO						("Renderer creating started...");
	
	if (m_renderer)
 		DELETE						(m_renderer);
 	
 	effect_manager::ref().force_sync = !async_effects;
 	
 	m_renderer						 = NEW( render::renderer )( &s_singletons_on_initialize->renderer_context );
 	
 	effect_manager::ref().force_sync = false;
	
	LOG_INFO						("Renderer creating finished.");
}

void engine::world::on_console_commands_config_loaded( bool load_renderer_options, resources::queries_result& data )
{
	if( !data.is_successful( ) )
	{
		LOG_ERROR("config file loading FAILED");
		if(load_renderer_options)
			set_renderer_configuration	( "", true );
		return;
	}
	
	resources::pinned_ptr_const<u8> pinned_data	(data[ 0 ].get_managed_resource( ));
	memory::reader				F( pinned_data.c_ptr( ), pinned_data.size( ) );
	xray::console_commands::load		( F, xray::console_commands::execution_filter_all );
	
	if (load_renderer_options)
		set_renderer_configuration	( options::ref().get_current_configuration(), true );
}

void engine::world::load_console_commands_config_query( pcstr cfg_name, bool load_renderer_options )
{
	resources::query_resource	(
		cfg_name,
		xray::resources::raw_data_class,
		boost::bind(&engine::world::on_console_commands_config_loaded, this, load_renderer_options, _1),
		g_allocator, 
		NULL, 
		NULL, 
		assert_on_fail_false
	);
}

void engine::world::initialize				( )
{
// #ifndef MASTER_GOLD
// 	xray::resources::subscribe_watcher("resources/render/", fastdelegate::FastDelegate<void ( xray::vfs::vfs_notification const &) >( this, &engine::world::on_render_config_changed ));
// #endif // #ifndef MASTER_GOLD
	
	// model cooker uses material_manager
	XRAY_CONSTRUCT_REFERENCE		( s_singletons_on_initialize, singletons_on_initialize )( );
	
	while ( !s_singletons_on_preinitialize->scene_manager.scene_count() ) {
		resources::dispatch_callbacks	( );
		threading::yield				( 10 );
	}
	
	R_ASSERT						( !m_initialized );
	m_initialized					= true;
	
	XRAY_CONSTRUCT_REFERENCE		( s_system_renderer, system_renderer ) ( &s_singletons_on_initialize->renderer_context );
	
	//set_renderer_configuration		( "", true );
	
	static xray::console_commands::cc_delegate cfg_load_cc_0( "cfg_load", boost::bind(&engine::world::load_console_commands_config_query, this, _1, false ), true );
	static xray::console_commands::cc_delegate cfg_load_cc_1( "cfg_load", boost::bind(&engine::world::load_console_commands_config_query, this, _1, true ), true );
	
	cfg_load_cc_0.execute				("resources/startup.cfg");
	cfg_load_cc_1.execute				("user_data/user.cfg");
}
	
engine::world::~world						( )
{
//	xray::resources::unsubscribe_watcher("resources/render/",fastdelegate::FastDelegate<void ( xray::vfs::vfs_notification const &) >( this, &engine::world::on_render_config_changed ));

	material::finalize_nomaterial_material	( );
	finalize_speedtree						( );
	
	XRAY_DESTROY_REFERENCE			( s_system_renderer );
	//XRAY_DESTROY_REFERENCE			( s_renderer );
	//XRAY_DESTROY_REFERENCE			( m_renderer );
	DELETE							( m_renderer );
	XRAY_DESTROY_REFERENCE			( s_singletons_on_initialize );
	XRAY_DESTROY_REFERENCE			( s_singletons_on_preinitialize );
	XRAY_DESTROY_REFERENCE			( s_options );
	
	unregister_cooks				( );
	
	particle::finalize				( );
	render::core::finalize			( );
}

void engine::world::reload_shaders		( )
{
	resource_manager::ref().reload_shader_sources	( true );
}

void engine::world::reload_modified_textures		( )
{
	resource_manager::ref().reload_modified_textures( );
}

void engine::world::clear_resources						()
{
	// Not sure if we need this
	//model_manager::ref().clear_resources		( );
}

void engine::world::set_view_matrix					( scene_view_ptr const& scene_view, float4x4 const& view_and_culling_matrix )
{
	static_cast_checked< render::scene_view* >( scene_view.c_ptr() )->camera_set_view		( view_and_culling_matrix );
}

#ifndef MASTER_GOLD
void engine::world::set_view_matrix_only			( scene_view_ptr const& scene_view, float4x4 const& view_matrix )
{
	static_cast_checked< render::scene_view* >( scene_view.c_ptr() )->camera_set_view_only	( view_matrix );
}
#endif // #ifndef MASTER_GOLD

void engine::world::set_projection_matrix			( scene_view_ptr const& scene_view, float4x4 const& projection_matrix )
{
	static_cast_checked< render::scene_view* >(scene_view.c_ptr())->camera_set_projection	( projection_matrix );
}

void engine::world::draw_text( pcstr text, xray::math::float2 const& position, xray::ui::font* const in_font, xray::math::color const& in_color )
{
	XRAY_UNREFERENCED_PARAMETERS(text, &position, in_font, &in_font, &in_color);
	NOT_IMPLEMENTED();
}

void engine::world::draw_scene						(
		scene_ptr const& scene,
		scene_view_ptr const& view,
		render_output_window_ptr const& output_window,
		viewport_type const& viewport,
		boost::function< void ( bool ) > const& on_draw_scene
	)
{
	float4x4 identity;
	identity.identity			( );
	system_renderer::ref().set_w( identity );
	
	if (m_renderer)
		m_renderer->render	( scene, view, output_window, viewport, on_draw_scene, m_enable_terrain_debug_mode );
	else {
		static_cast_checked<xray::render::scene*>(scene.c_ptr())->flush	( on_draw_scene );
	}
}

void engine::world::draw_debug_lines			( colored_vertices_type  const& vertices, colored_indices_type  const& indices)
{
	system_renderer::ref().set_w( float4x4().identity());
	render::system_renderer::ref().draw_lines	(
		&*vertices.begin(),
		&*vertices.end(),
		&*indices.begin(),
		&*indices.end()
	);
}

void engine::world::draw_debug_triangles		( colored_vertices_type const& vertices, colored_indices_type  const& indices)
{
	system_renderer::ref().set_w( float4x4().identity());
	render::system_renderer::ref().draw_triangles(
		&*vertices.begin(),
		&*vertices.end(),
		&*indices.begin(),
		&*indices.end()
	);
}

void engine::world::end_frame( )
{
//	if ( m_renderer )
		++m_frame_id;
}

u32 engine::world::frame_id			()
{
	return				( m_frame_id);
}

pcstr engine::world::type			()
{
	return				( "dx11");
}

void engine::world::setup_view_and_output		( scene_view_ptr const& view_ptr, render_output_window_ptr const& output_window, viewport_type const& viewport)
{
	render::system_renderer::ref().setup_scene_view( view_ptr );
	render::system_renderer::ref().setup_render_output_window( output_window, viewport );
}

void engine::world::draw_ui_vertices	( ui_vertices_type vertices, u32 const & count, int prim_type, int point_type)
{
	render::system_renderer::ref().draw_ui_vertices( (vertex_formats::TL*)vertices, count, prim_type, point_type );
}

void engine::world::clear_zbuffer		( float z_value)
{
	backend::ref().clear_depth_stencil( D3D_CLEAR_DEPTH /*| D3D11_CLEAR_STENCIL */, z_value, 0);
}

void engine::world::draw_editor_lines		( colored_vertices_type const& vertices, colored_indices_type  const& indices)
{
	system_renderer::ref().set_w( float4x4().identity());
	render::system_renderer::ref().draw_lines(
		&*vertices.begin(),
		&*vertices.end(),
		&*indices.begin(),
		&*indices.end()
	);
}

void engine::world::draw_editor_triangles	( colored_vertices_type const& vertices, colored_indices_type  const& indices)
{
	system_renderer::ref().set_w( float4x4().identity());
	render::system_renderer::ref().draw_triangles(
		&*vertices.begin(),
		&*vertices.end(),
		&*indices.begin(),
		&*indices.end()
	);
}

void engine::world::draw_screen_lines		( xray::render::scene_ptr const& scene, xray::math::float2 const* points, u32 count, math::color const& color, float width, u32 pattern)
{
	XRAY_UNREFERENCED_PARAMETER	( scene );

	float3* const points_3d		= pointer_cast< float3* >( ALLOCA( sizeof(xray::math::float3) * count ) );

	// normalize coordinates
	for ( u32 i = 0; i < count; ++i)
		points_3d[i]			= float3(
			+	 points[i].elements[0] / (float)xray::render::backend::ref().target_width()  *2.f - 1.f,
			-(	(points[i].elements[1] / (float)xray::render::backend::ref().target_height())*2.f - 1.f),
			0.f
		);

	render::system_renderer::ref().draw_screen_lines( points_3d, count, color, width, pattern, false, true);
}

void engine::world::draw_3D_screen_lines	( xray::render::scene_ptr const& scene, xray::math::float3 const* points, u32 count, math::color const& color, float width, u32 pattern, bool use_depth)
{
	XRAY_UNREFERENCED_PARAMETER	( scene );

	render::system_renderer::ref().draw_screen_lines( points, count, color, width, pattern, use_depth, false);
}

void engine::world::draw_3D_screen_point	( xray::render::scene_ptr const& scene, float3 const& position, math::color color, float width, bool use_depth)
{
	XRAY_UNREFERENCED_PARAMETER	( scene );

	render::system_renderer::ref().draw_3D_point( position, width, color, use_depth);
}

void engine::world::setup_grid_render_mode	( u32 grid_density)
{
	XRAY_UNREFERENCED_PARAMETER	( grid_density);
	render::system_renderer::ref().setup_grid_render_mode( grid_density);
	
}
void engine::world::remove_grid_render_mode	()
{
	render::system_renderer::ref().remove_grid_render_mode();
}

//xray::math::float4x4 const&	engine::world::get_view_matrix			() const
//{
//	return system_renderer::ref().get_v();
//}
//
//xray::math::float4x4 const&	engine::world::get_projection_matrix	() const
//{
//	return system_renderer::ref().get_p();
//}
//xray::render::viewport_type engine::world::get_viewport	() const
//{
//	D3D_VIEWPORT viewport;
//	backend::ref().get_viewport( viewport);
//	return viewport_type( xray::math::int2( math::floor(viewport.TopLeftX), math::floor(viewport.TopLeftY)), xray::math::int2( math::floor(viewport.Width), math::floor(viewport.Height) ));
//}

void engine::world::add_speedtree_instance( xray::render::scene_ptr const& in_scene, render::speedtree_instance_ptr const& v, float4x4 const& transform, bool populate_forest )
{
	xray::render::scene* scene = static_cast_checked<xray::render::scene*>(in_scene.c_ptr());
	
	scene->add_speedtree_instance(v, transform, populate_forest);
}

void engine::world::remove_speedtree_instance( xray::render::scene_ptr const& in_scene, render::speedtree_instance_ptr const& v, bool populate_forest )
{
	xray::render::scene* scene = static_cast_checked<xray::render::scene*>(in_scene.c_ptr());
	
	scene->remove_speedtree_instance(v, populate_forest);
}

void engine::world::update_speedtree_instance( xray::render::scene_ptr const& in_scene, render::speedtree_instance_ptr const& v, float4x4 const& transform, bool populate_forest )
{
	xray::render::scene* scene = static_cast_checked<xray::render::scene*>(in_scene.c_ptr());
	
	scene->set_speedtree_instance_transform(v, transform, populate_forest);
}

void engine::world::populate_speedtree_forest( xray::render::scene_ptr const& in_scene )
{
	xray::render::scene* scene = static_cast_checked<xray::render::scene*>(in_scene.c_ptr());
	
	scene->populate_speedtree_forest( );
}

void engine::world::add_model( xray::render::scene_ptr const& in_scene, render::render_model_instance_ptr const& v, float4x4 const& transform, bool apply_transform )
{
	xray::render::scene* scene = static_cast_checked<xray::render::scene*>(in_scene.c_ptr());

	render_model_instance_impl_ptr model = static_cast_resource_ptr<render_model_instance_impl_ptr>(v);
	
	if (apply_transform)
		model->set_transform( transform );
	
	scene->add_model( model);
}

void engine::world::update_model( xray::render::scene_ptr const& in_scene, render_model_instance_ptr const& v, xray::math::float4x4 const& transform)
{
	xray::render::scene* scene = static_cast_checked<xray::render::scene*>(in_scene.c_ptr());

	render_model_instance_impl_ptr model = static_cast_resource_ptr<render_model_instance_impl_ptr>(v);
	model->set_transform				( transform );
	scene->modify_model					( model );
}

void engine::world::remove_model( xray::render::scene_ptr const& in_scene, render_model_instance_ptr const& v)
{
	xray::render::scene* scene = static_cast_checked<xray::render::scene*>(in_scene.c_ptr());

	render_model_instance_impl_ptr model = static_cast_resource_ptr<render_model_instance_impl_ptr>(v);
	scene->remove_model( model);
}

static enum_vertex_input_type speedtree_subsurface_name_to_vertex_input_type(fs_new::virtual_path_string const& subsurface_name)
{
	if (subsurface_name=="branch")			return speedtree_branch_vertex_input_type;
	else if (subsurface_name=="frond")		return speedtree_frond_vertex_input_type;
	else if (subsurface_name=="leafmesh")	return speedtree_leafmesh_vertex_input_type;
	else if (subsurface_name=="leafcard")	return speedtree_leafcard_vertex_input_type;
	else if (subsurface_name=="billboard")	return speedtree_billboard_vertex_input_type;
	else									return null_vertex_input_type;
}

static speedtree_tree::component_type vertex_input_type_to_speedtree_component_type(enum_vertex_input_type type)
{
	switch (type)
	{
		case speedtree_branch_vertex_input_type:	return speedtree_tree::branch;
		case speedtree_frond_vertex_input_type:		return speedtree_tree::frond;
		case speedtree_leafmesh_vertex_input_type:	return speedtree_tree::leafmesh;
		case speedtree_leafcard_vertex_input_type:	return speedtree_tree::leafcard;
		case speedtree_billboard_vertex_input_type: return speedtree_tree::billboard;
		default:									NODEFAULT(return speedtree_tree::branch);
	};
}

static void on_speedtree_material_effects_instance_ready(resources::queries_result& data, speedtree_tree* tree, enum_vertex_input_type vertex_input_type)
{
	if (data[0].is_successful())
		tree->set_material_effects(
			xray::static_cast_resource_ptr<material_effects_instance_ptr>(data[0].get_unmanaged_resource()),
			vertex_input_type_to_speedtree_component_type(vertex_input_type),
			data[0].get_requested_path()
		);
}

void engine::world::set_speedtree_instance_material (render::speedtree_instance_ptr const& v, 
													 fs_new::virtual_path_string const& subsurface_name, 
													 resources::unmanaged_resource_ptr in_mtl_ptr)
{
	speedtree_tree*		tree				= static_cast_checked<speedtree_tree*>(v->m_speedtree_tree_ptr.c_ptr());
	material_ptr		mtl_ptr				= static_cast_resource_ptr<material_ptr>(in_mtl_ptr);
	
	if (!mtl_ptr)
		return;
	
	resources::user_data_variant			data_variant;
	
	data_variant.set						(NEW(material_effects_instance_cook_data)(
		speedtree_subsurface_name_to_vertex_input_type(subsurface_name), 
		static_cast_resource_ptr<resources::unmanaged_resource_ptr>(mtl_ptr))
	);
	
	// Must be query_resource_and_wait!
	resources::query_resource_and_wait		(
		mtl_ptr->get_material_name(),
		resources::material_effects_instance_class,
		boost::bind(
			&on_speedtree_material_effects_instance_ready, 
			_1, 
			tree, 
			speedtree_subsurface_name_to_vertex_input_type(subsurface_name)
		),
		xray::render::g_allocator,
		&data_variant
	);
}

static void on_model_material_effects_instance_ready(resources::queries_result& in_data, 
													 render_surface*			in_render_surface)
{
	if (!in_render_surface)
		return;
 	
 	if (in_data[0].is_successful())
	{
 		in_render_surface->set_material_effects(
 			xray::static_cast_resource_ptr<material_effects_instance_ptr>(in_data[0].get_unmanaged_resource()),
			in_data[0].get_requested_path()
 		);
	}
}

void engine::world::set_model_material( render::render_model_instance_ptr const& v, 
										fs_new::virtual_path_string const& subsurface_name, 
										resources::unmanaged_resource_ptr m )
{
	render_model_instance_impl_ptr model = static_cast_resource_ptr<render_model_instance_impl_ptr>(v);
	
	render_surface_instances		list;
	model->get_surfaces				( list, false );
	
	bool check_name = (subsurface_name.length()!=0);
	
	material_ptr mtl_ptr = static_cast_resource_ptr<material_ptr>(m);
	
	for (render_surface_instances::iterator it = list.begin(); it != list.end(); ++it)
	{
		render_surface_instance& inst = *(*it);
		
		bool const apply = !check_name || (inst.m_render_surface->m_render_geometry.shading_group_name == subsurface_name.c_str());
		
		if (!apply)
			continue;
		
		if (!mtl_ptr)
		{
			inst.m_render_surface->m_materail_effects_instance = 0;
			continue;
		}
		
		resources::user_data_variant data_variant;
		
		data_variant.set						(NEW(material_effects_instance_cook_data)(
			inst.m_render_surface->get_vertex_input_type(), 
			static_cast_resource_ptr<resources::unmanaged_resource_ptr>(mtl_ptr))
		);
		
		// Must be query_resource_and_wait!
		resources::query_resource_and_wait		(
			mtl_ptr->get_material_name(),
			resources::material_effects_instance_class,
			boost::bind(
				&on_model_material_effects_instance_ready, 
				_1, 
				inst.m_render_surface
			),
			xray::render::g_allocator,
			&data_variant
		);
	}
#if 0
	render_model_instance_impl_ptr model = static_cast_resource_ptr<render_model_instance_impl_ptr>(v);
	
	render_surface_instances		list;
	model->get_surfaces	( list );
	
	bool check_name = (subsurface_name.length()!=0);
	
	material_ptr mtl_ptr = static_cast_resource_ptr<material_ptr>(m);

	for (render_surface_instances::iterator it=list.begin(); it!=list.end(); ++it)
	{
		// TODO: Set only render_model_instance m_material?
		// TODO: Compare sg_name with subsurface_name.
		render_surface_instance& inst = *(*it);

		bool apply = !check_name || (inst.m_render_surface->m_render_geometry.shading_group_name == subsurface_name);

		if( apply )
		{
			if(mtl_ptr.c_ptr())
				inst.m_render_surface->set_material			( mtl_ptr );
			else
				inst.m_render_surface->set_default_material	( );
		}
	}
#endif // #if 0
}

void engine::world::set_model_visible( render::render_model_instance_ptr const& v, fs_new::virtual_path_string const& subsurface_name, bool value )
{
	render_model_instance_impl_ptr model = static_cast_resource_ptr<render_model_instance_impl_ptr>(v);
	
	render_surface_instances		list;
	model->get_surfaces	( list, false );
	
	bool check_name = (subsurface_name.length()!=0);

	for (render_surface_instances::iterator it=list.begin(); it!=list.end(); ++it)
	{
		render_surface_instance& inst = *(*it);
		bool apply = !check_name || (inst.m_render_surface->m_render_geometry.shading_group_name == subsurface_name.c_str());

		if( apply )
			inst.m_visible = value;
	}
}

void engine::world::update_system_model( render::render_model_instance_ptr const& v, xray::math::float4x4 const& transform)
{
	render_model_instance_impl_ptr model = static_cast_resource_ptr<render_model_instance_impl_ptr>(v);
	model->set_transform				( transform );
}


void engine::world::set_model_ghost_mode( render::render_model_instance_ptr const& v, bool value)
{
	XRAY_UNREFERENCED_PARAMETERS( v, value );
	//render_model_instance_impl_ptr model = static_cast_resource_ptr<render_model_instance_impl_ptr>(v);
	//polymorph_vector_base<render_model_instance> const* render_models = model->get_models_();
	//system_renderer::ref().set_model_ghost_mode( *render_models, value);
}

void engine::world::draw_terrain_debug( )
{
	//system_renderer::ref().draw_debug_terrain();
}

void engine::world::update_model_vertex_buffer( render::render_model_instance_ptr const& v, xray::vectora<xray::render::buffer_fragment> const& fragments)
{
	user_render_model_instance* model = static_cast_checked<user_render_model_instance*>(v.c_ptr());

	ASSERT( fragments.size() > 0 );

	user_render_surface_editable* surface = static_cast_checked<user_render_surface_editable*>(model->m_surface);
	u8* lock_data	= (u8*)surface->m_vb->map( D3D_MAP_WRITE_DISCARD);


	vectora<buffer_fragment>::const_iterator	it	= fragments.begin();
	vectora<buffer_fragment>::const_iterator	end	= fragments.end();

	for( ; it != end; ++it)
		memory::copy		( lock_data + it->start, it->size, it->buffer, it->size );

	surface->m_vb->unmap	( );
}

void engine::world::update_model_index_buffer( render::render_model_instance_ptr const& v, xray::vectora<xray::render::buffer_fragment> const& fragments)
{
	XRAY_UNREFERENCED_PARAMETERS	( v,fragments);
	NOT_IMPLEMENTED();
}

//void engine::world::change_model_shader( render::render_model_instance_ptr const& v, char const* shader, char const* texture)
//{
//	NOT_IMPLEMENTED();
//	//render_model* r_model = static_cast_checked<render_model*>(&(*v));
//	//r_model->set_shader(shader, texture);
//}

void engine::world::add_light( xray::render::scene_ptr const& in_scene, u32 id, render::light_props const& props)
{
	xray::render::scene* scene = static_cast_checked<xray::render::scene*>(in_scene.c_ptr());

	scene->add_light( id, props);
}

void engine::world::update_light( xray::render::scene_ptr const& in_scene, u32 id, render::light_props const& props)
{
	xray::render::scene* scene = static_cast_checked<xray::render::scene*>(in_scene.c_ptr());

	scene->update_light	( id, props);
}

void engine::world::remove_light( xray::render::scene_ptr const& in_scene, u32 id)
{
	xray::render::scene* scene = static_cast_checked<xray::render::scene*>(in_scene.c_ptr());

	scene->remove_light	( id);
}

void engine::world::add_decal(xray::render::scene_ptr const& in_scene, u32 id, render::decal_properties const& properties)
{
	xray::render::scene* scene = static_cast_checked<xray::render::scene*>(in_scene.c_ptr());

	scene->add_decal(id, properties);
}

void engine::world::update_decal( xray::render::scene_ptr const& in_scene, u32 id, render::decal_properties const& properties)
{
	xray::render::scene* scene = static_cast_checked<xray::render::scene*>(in_scene.c_ptr());

	scene->update_decal	( id, properties);
}

void engine::world::remove_decal( xray::render::scene_ptr const& in_scene, u32 id)
{
	xray::render::scene* scene = static_cast_checked<xray::render::scene*>(in_scene.c_ptr());

	scene->remove_decal	( id);
}

void engine::world::terrain_add_cell( xray::render::scene_ptr const& in_scene,  render::render_model_instance_ptr const& v )
{
	xray::render::scene* scene			 = static_cast_checked<xray::render::scene*>(in_scene.c_ptr());
	terrain_render_model_instance_ptr t	 = static_cast_resource_ptr<terrain_render_model_instance_ptr>(v);

	ASSERT( scene->terrain());
	if( scene->terrain())
		scene->terrain()->add_cell		( t);
}

void engine::world::terrain_remove_cell( xray::render::scene_ptr const& in_scene, render::render_model_instance_ptr const& v )
{
	xray::render::scene* scene = static_cast_checked<xray::render::scene*>(in_scene.c_ptr());

	terrain_render_model_instance_ptr t	= static_cast_resource_ptr<terrain_render_model_instance_ptr>(v);

	ASSERT( scene->terrain());
	if( scene->terrain())
		scene->terrain()->remove_cell		( t);
}

void engine::world::terrain_update_cell_buffer( xray::render::scene_ptr const& in_scene, render::render_model_instance_ptr const& v, xray::vectora<xray::render::terrain_buffer_fragment> const& fragments, float4x4 const& transform)
{
	xray::render::scene* scene = static_cast_checked<xray::render::scene*>(in_scene.c_ptr());

	terrain_render_model_instance_ptr t	= static_cast_resource_ptr<terrain_render_model_instance_ptr>(v);
	
	ASSERT( scene->terrain());
	if( scene->terrain())
		scene->terrain()->update_cell_buffer( t, fragments, transform);
}

void engine::world::terrain_update_cell_aabb( xray::render::scene_ptr const& in_scene, render::render_model_instance_ptr const& v, math::aabb const & aabb)
{
	xray::render::scene* scene = static_cast_checked<xray::render::scene*>(in_scene.c_ptr());

	terrain_render_model_instance_ptr t	= static_cast_resource_ptr<terrain_render_model_instance_ptr>(v);
	
	ASSERT( scene->terrain());
	if( scene->terrain())
		scene->terrain()->update_cell_aabb	( t, aabb);
}

void engine::world::terrain_add_cell_texture(  xray::render::scene_ptr const& in_scene, render::render_model_instance_ptr const& v, fs_new::virtual_path_string const & texture, u32 user_tex_id)
{
	xray::render::scene* scene = static_cast_checked<xray::render::scene*>(in_scene.c_ptr());

	terrain_render_model_instance_ptr t	= static_cast_resource_ptr<terrain_render_model_instance_ptr>(v);
	
	ASSERT( scene->terrain());
	if( scene->terrain())
		scene->terrain()->add_cell_texture	( t, texture, user_tex_id);
}

void engine::world::terrain_remove_cell_texture( xray::render::scene_ptr const& in_scene, render::render_model_instance_ptr const& v, u32 user_tex_id)
{
	xray::render::scene* scene = static_cast_checked<xray::render::scene*>(in_scene.c_ptr());

	terrain_render_model_instance_ptr t	= static_cast_resource_ptr<terrain_render_model_instance_ptr>(v);
	
	ASSERT( scene->terrain());
	if( scene->terrain())
		scene->terrain()->remove_cell_texture	( t, user_tex_id);
}

void engine::world::terrain_exchange_texture( xray::render::scene_ptr const& in_scene, fs_new::virtual_path_string const & old_texture, fs_new::virtual_path_string const & new_texture)
{
	xray::render::scene* scene = static_cast_checked<xray::render::scene*>(in_scene.c_ptr());

	ASSERT( scene->terrain());
	if( scene->terrain())
		scene->terrain()->exchange_texture( old_texture, new_texture );
}

void engine::world::setup_rotation_control_modes( bool color_write )
{
	render::system_renderer::ref().setup_rotation_control_modes( color_write);
}

void engine::world::update_skeleton( render::render_model_instance_ptr const& v, math::float4x4* matrices, u32 count )
{
	skeleton_render_model_instance* skeleton	= static_cast<skeleton_render_model_instance*>(v.c_ptr());
	skeleton->update_render_matrices	( matrices, count );
}

untyped_buffer * create_quad_ib()
{
	if( g_quad_ib)
		return &*g_quad_ib;

	const u32 quad_count = 4*1024;
	const u32 idx_count  = quad_count*2*3;

	// 	if ( device::ref().get_caps().geometry.software)
	// 	{
	// 		ASSERT( 0);
	// 		usage |= D3DUSAGE_SOFTWAREPROCESSING;
	// 	}

	u16	indices[idx_count];

	int		vertex_id = 0;
	int		idx= 0;
	for ( int i=0; i<quad_count; ++i)
	{
		indices[idx++]=u16( vertex_id+0);
		indices[idx++]=u16( vertex_id+1);
		indices[idx++]=u16( vertex_id+2);

		indices[idx++]=u16( vertex_id+3);
		indices[idx++]=u16( vertex_id+2);
		indices[idx++]=u16( vertex_id+1);

		vertex_id+=4;
	}

	g_quad_ib = resource_manager::ref().create_buffer( idx_count*sizeof(u16), indices, enum_buffer_type_index, false);
	return &*g_quad_ib;
}

void engine::world::play_particle_system			( xray::render::scene_ptr const& in_scene, particle::particle_system_instance_ptr in_instance, bool use_transform, bool always_looping, math::float4x4 const& transform )
{
	xray::render::scene* scene = static_cast_checked<xray::render::scene*>(in_scene.c_ptr());

	scene->particle_world()->play(in_instance, transform, use_transform, always_looping);
}

void engine::world::stop_particle_system			( xray::render::scene_ptr const& in_scene, particle::particle_system_instance_ptr in_instance )
{
	// TODO: time
	xray::render::scene* scene = static_cast_checked<xray::render::scene*>(in_scene.c_ptr());

	scene->particle_world()->stop(in_instance, 0.0f);
}

void engine::world::remove_particle_system_instance	( particle::particle_system_instance_ptr particle_system_instance, xray::render::scene_ptr const& in_scene )
{
	xray::render::scene* scene = static_cast_checked<xray::render::scene*>(in_scene.c_ptr());

	R_ASSERT						( particle_system_instance );
	scene->particle_world()->remove	( particle_system_instance );
}

void engine::world::update_particle_system_instance	( particle::particle_system_instance_ptr particle_system_instance, xray::render::scene_ptr const& in_scene, xray::math::float4x4 const& transform, bool visible, bool paused)
{
	xray::render::scene* scene = static_cast_checked<xray::render::scene*>(in_scene.c_ptr());

	particle::world& world			= *scene->particle_world();
	world.set_transform				( particle_system_instance, transform );
	world.set_visible				( particle_system_instance, visible );
	world.set_paused				( particle_system_instance, paused );
}

#ifndef MASTER_GOLD

void engine::world::update_preview_particle_system	( xray::particle::particle_system** preview_ps, xray::render::scene_ptr const& in_scene, xray::configs::lua_config_value config_value )
{
	xray::render::scene* scene = static_cast_checked<xray::render::scene*>(in_scene.c_ptr());

	scene->particle_world()->update_preview	( preview_ps, config_value );
}

void engine::world::update_preview_particle_system_transform	( xray::particle::particle_system** preview_ps, xray::render::scene_ptr const& in_scene, math::float4x4 const& transform )
{
	xray::render::scene* scene = static_cast_checked<xray::render::scene*>(in_scene.c_ptr());

	scene->particle_world()->set_transform	( preview_ps, transform );
}

void engine::world::set_looping_preview_particle_system	( xray::particle::particle_system** preview_ps, xray::render::scene_ptr const& in_scene, bool is_looped )
{
	xray::render::scene* scene = static_cast_checked<xray::render::scene*>(in_scene.c_ptr());

	scene->particle_world()->set_looped	( preview_ps, is_looped );
}

void engine::world::add_preview_particle_system		( xray::particle::particle_system** in_ps_out_ptr, xray::render::scene_ptr const& in_scene, xray::configs::lua_config_value init_values, math::float4x4 const& transform )
{
	xray::render::scene* scene = static_cast_checked<xray::render::scene*>(in_scene.c_ptr());

	*in_ps_out_ptr = scene->particle_world()->add( init_values, transform );
}

void engine::world::remove_preview_particle_system	( xray::particle::particle_system** in_ps_out_ptr, xray::render::scene_ptr const& in_scene )
{
	xray::render::scene* scene = static_cast_checked<xray::render::scene*>(in_scene.c_ptr());

	R_ASSERT						( in_ps_out_ptr );
	scene->particle_world()->remove	( in_ps_out_ptr );
}

void engine::world::restart_preview_particle_system	( xray::particle::particle_system** in_ps_out_ptr, xray::render::scene_ptr const& in_scene )
{
	xray::render::scene* scene = static_cast_checked<xray::render::scene*>(in_scene.c_ptr());

	scene->particle_world()->restart( in_ps_out_ptr );
}

void engine::world::show_preview_particle_system	( xray::particle::particle_system** in_ps_out_ptr, xray::render::scene_ptr const& in_scene )
{
	xray::render::scene* scene = static_cast_checked<xray::render::scene*>(in_scene.c_ptr());

	scene->particle_world()->show	( in_ps_out_ptr );
}

u32 engine::world::get_num_preview_particle_system_emitters(xray::particle::particle_system** in_ps_out_ptr, xray::render::scene_ptr const& in_scene ) const
{
	xray::render::scene* scene = static_cast_checked<xray::render::scene*>(in_scene.c_ptr());

	return	scene->particle_world()->get_num_preview_particle_system_emitters( in_ps_out_ptr );
}

void engine::world::gather_preview_particle_system_statistics	( xray::particle::particle_system** in_ps_out_ptr, xray::render::scene_ptr const& in_scene, xray::particle::preview_particle_emitter_info* out_info )
{
	xray::render::scene* scene = static_cast_checked<xray::render::scene*>(in_scene.c_ptr());

	return	scene->particle_world()->gather_stats( in_ps_out_ptr, out_info );
}

void engine::world::draw_render_statistics	( xray::ui::world* ui_world )
{
	statistics::ref().render(*ui_world, 5, 5);
}

void engine::world::select_particle_system_instance	( scene_ptr const& scene, particle::particle_system_instance_ptr const& instance, bool const is_selected )
{
	static_cast_checked< render::scene& >( *scene.c_ptr() ).select_particle_system_instance( instance, is_selected );
}

void engine::world::select_speedtree_instance		( scene_ptr const& scene, render::speedtree_instance_ptr const& instance, bool const is_selected )
{
	static_cast_checked< render::scene& >( *scene.c_ptr() ).select_speedtree_instance( instance, is_selected );
}

void engine::world::select_model( scene_ptr const& scene, render_model_instance_ptr const& instance, bool const is_selected )
{
	render_model_instance_impl_ptr impl = static_cast_resource_ptr<render_model_instance_impl_ptr>(instance);
	static_cast_checked< render::scene& >( *scene.c_ptr() ).select_model( impl, is_selected );
}

void engine::world::enable_terrain_debug_mode		( bool const is_enabled )
{
	m_enable_terrain_debug_mode						= is_enabled;
}

void engine::world::apply_material_changes( fs_new::virtual_path_string const& material_name, xray::configs::lua_config_value const& config_value )
{
	material_manager::ref().update_material_from_config(material_name, config_value);
}

#endif // #ifndef MASTER_GOLD

void	engine::world::set_selection_parameters	( xray::math::float4 const& selection_color, float selection_rate )
{
	system_renderer::ref().set_selection_parameters( selection_color, selection_rate );
}

static xray::uninitialized_reference< engine::world > s_world;

engine::world* engine::create_world	( )
{
	R_ASSERT					( !s_world.initialized() );
	XRAY_CONSTRUCT_REFERENCE	( s_world, engine::world ) ( );
	return						s_world.c_ptr( );
}

void engine::destroy			( engine::world*& engine_world )
{
	R_ASSERT					( s_world.initialized() );
	R_ASSERT					( s_world.c_ptr() == engine_world );
	XRAY_DESTROY_REFERENCE		( s_world );
	engine_world				= 0;
}

void engine::world::set_slomo	( xray::render::scene_ptr const& scene, float time_multiplier )
{
	render::scene* const scene_ptr	= static_cast_checked< render::scene* >( scene.c_ptr() );
	scene_ptr->set_slomo			( time_multiplier );
}

void engine::world::toggle_render_stage	( enum_render_stage_type stage_type, bool toggle )
{
	//render::renderer::ref().toggle_render_stage(stage_type, toggle);
	if (m_renderer)
		m_renderer->toggle_render_stage(stage_type, toggle);
}

void engine::world::set_view_mode	( scene_view_ptr view_ptr, scene_view_mode view_mode )
{
	scene_view* view = static_cast_checked<scene_view*>(view_ptr.c_ptr());
	view->set_view_mode(view_mode);
}


void engine::world::set_particles_render_mode		( scene_view_ptr view_ptr, xray::particle::enum_particle_render_mode render_mode )
{
	scene_view* view = static_cast_checked<scene_view*>(view_ptr.c_ptr());
	view->set_particles_render_mode(render_mode);	
}

void engine::world::set_sky_material				( scene_ptr const& in_scene, resources::unmanaged_resource_ptr mtl_ptr)
{
	xray::render::scene* scn	= static_cast_checked<xray::render::scene*>(in_scene.c_ptr());
	scn->set_sky_material		(static_cast_resource_ptr<material_effects_instance_ptr>(mtl_ptr));
}

#if 0

static void fix_specular_in_stage( configs::lua_config_value& material_config, pcstr stage_name )
{
	if (!material_config.value_exists(stage_name))
		return;
	
	configs::lua_config_value& stage_config	= material_config[stage_name]["effect"];
	
	if (stage_config.value_exists("texture_specular_intensity"))
	{
		if (stage_config.value_exists("texture_specular_power"))
		{
			stage_config["texture_specular_power"].assign_lua_value(stage_config["texture_specular_intensity"]);
		}
	}
}

static void on_material_loaded( pstr request_path, resources::queries_result& data )
{
	if( data.is_successful() )
	{
		configs::lua_config_ptr material_config		= static_cast_checked<configs::lua_config*>(data[0].get_unmanaged_resource().c_ptr());
		fix_specular_in_stage						(material_config->get_root()["material"], "g_stage");
		fix_specular_in_stage						(material_config->get_root()["material"], "forward");
		fix_specular_in_stage						(material_config->get_root()["material"], "lighting");
		
		fs_new::virtual_path_string			physical_path;
		if (resources::query_result::convert_logical_to_disk_path(&physical_path, request_path))
		{
			material_config->get_root().save_as		(physical_path.c_str());
		}
	}
	FREE(request_path);
}

static void on_fs_iterator_materials_ready( pcstr materials_path, xray::resources::fs_iterator fs_it )
{
	resources::fs_iterator it				= fs_it.children_begin();
	resources::fs_iterator it_e				= fs_it.children_end();
	
	for ( ; it!=it_e; ++it )
	{
		pcstr mname							= it.get_name();
		
		if(it.is_folder())
		{
			fs_new::virtual_path_string new_materials_path;
			new_materials_path.assignf("%s/%s", materials_path, mname);
			on_fs_iterator_materials_ready(new_materials_path.c_str(), it);
			continue;
		}
		
		if (strstr(mname, ".orig"))
			continue;
		
		fs_new::virtual_path_string request_path;
		request_path.assignf				("%s/%s", materials_path, mname);
		
		pstr			request_path_buffer = ALLOC(char, 256);
		memory::zero						(request_path_buffer, 256);
		memory::copy						(request_path_buffer, request_path.length(), request_path.c_str(), request_path.length());
		
 		resources::query_resource(
 			request_path.c_str(),
			resources::lua_config_class,
 			boost::bind(&on_material_loaded, request_path_buffer, _1),
 			xray::render::g_allocator
 		);
	}
}

static void fix_materials( pcstr materials_path )
{
	resources::query_fs_iterator(
		materials_path,
		boost::bind(&on_fs_iterator_materials_ready, materials_path, _1),
		g_allocator,
		resources::recursive_true
	);
}
#endif // #if 0

#if 0
static bool support_srgb_format( pcstr name )
{
	return	strings::equal(name, "A8")		||
			strings::equal(name, "DXT1")	||
			strings::equal(name, "DXT1a")	||
			strings::equal(name, "DXT3")	||
			strings::equal(name, "DXT5");
}

static void on_texture_options_loaded( pstr request_path, resources::queries_result& data )
{
	if( data.is_successful() )
	{
		configs::lua_config_ptr options_config			= static_cast_checked<configs::lua_config*>(data[0].get_unmanaged_resource().c_ptr());
		configs::lua_config_value& options_config_value	= options_config->get_root()["options"];
		
		if (!options_config_value.value_exists("srgb"))
			options_config_value["srgb"]				= (u32)support_srgb_format((pcstr)options_config_value["format"]);
		
		fs_new::virtual_path_string			physical_path;
		if (resources::query_result::convert_logical_to_disk_path(&physical_path, request_path))
		{
			options_config->get_root().save_as		(physical_path.c_str());
		}
	}
	DEBUG_FREE(request_path);
}

static void on_fs_iterator_texture_options_ready( pcstr options_path, xray::resources::fs_iterator fs_it )
{
	resources::fs_iterator it				= fs_it.children_begin();
	resources::fs_iterator it_e				= fs_it.children_end();
	
	for ( ; it!=it_e; ++it )
	{
		pcstr mname							= it.get_name();
		
		if(it.is_folder())
		{
			fs_new::virtual_path_string new_options_path;
			new_options_path.assignf("%s/%s", options_path, mname);
			on_fs_iterator_texture_options_ready(new_options_path.c_str(), it);
			continue;
		}
		
		if (strstr(mname, ".orig") || !strstr(mname, ".options"))
			continue;
		
		fs_new::virtual_path_string request_path;
		request_path.assignf				("%s/%s", options_path, mname);
		
		pstr			request_path_buffer = DEBUG_ALLOC(char, 256);
		memory::zero						(request_path_buffer, 256);
		memory::copy						(request_path_buffer, request_path.length(), request_path.c_str(), request_path.length());
		
 		resources::query_resource(
 			request_path.c_str(),
			resources::lua_config_class,
 			boost::bind(&on_texture_options_loaded, request_path_buffer, _1),
 			&xray::debug::g_mt_allocator
 		);
	}
}

static void fix_texture_options( pcstr path )
{
	resources::query_fs_iterator(
		 path,
		boost::bind(&on_fs_iterator_texture_options_ready,  path, _1),
		&xray::debug::g_mt_allocator,
		resources::recursive_true
	);
}
#endif // #if 0

void engine::world::enable_post_process				( scene_view_ptr view_ptr, bool enable )
{
	scene_view* view = static_cast_checked<scene_view*>(view_ptr.c_ptr());
	view->set_use_post_process(enable);
	
	//fix_texture_options("resources/textures");
	//fix_materials("resources/materials");
	//fix_materials("resources/material_instances");
}

void engine::world::set_post_process( scene_view_ptr view_ptr, resources::unmanaged_resource_ptr post_process_resource)
{
	scene_view* view = static_cast_checked<scene_view*>(view_ptr.c_ptr());
	
	material_ptr mtl = static_cast_checked<xray::render::material*>(post_process_resource.c_ptr());
	
	xray::render::material_manager::ref().initialize_post_process_parameters(&view->m_post_process_parameters, mtl, false);
}

particle::world& engine::world::particle_world	( scene_ptr const& scene )
{
	return	*static_cast_resource_ptr< resources::resource_ptr<xray::render::scene, resources::unmanaged_resource> >( scene )->particle_world();
}

math::uint2 engine::world::window_client_size	( render::render_output_window_ptr const& render_output_window )
{
	return	static_cast_checked< render::render_output_window* >( render_output_window.c_ptr() )->get_window_client_size( );
}

void engine::world::draw_lines					( scene_ptr const& scene, debug_vertices_type const& vertices, debug_indices_type const& indices )
{
	static_cast_resource_ptr< resources::resource_ptr<xray::render::scene, resources::unmanaged_resource> >( scene )->draw_lines	( vertices, indices );
}

void engine::world::draw_triangles				( scene_ptr const& scene, debug_vertices_type const& vertices, debug_indices_type const& indices )
{
	static_cast_resource_ptr< resources::resource_ptr<xray::render::scene, resources::unmanaged_resource> >( scene )->draw_triangles( vertices, indices );
}

void make_ui_vertices(
	xray::vectora<xray::render::ui::vertex>& out_vertices,
	pcstr in_text, 
	xray::ui::font const& in_font,
	xray::math::float2 const& in_position,
	xray::math::color const& in_color,
	xray::math::color const& in_selection_color,
	u32 max_line_width,
	bool is_multiline,
	u32 start_selection_index,
	u32 end_selection_index
);

void engine::world::draw_text					(
	vectora< xray::render::ui::vertex >& output,
		pcstr const& text,
		xray::ui::font const& font,
		float2 const& position,
		math::color const& text_color,
		math::color const& selection_color,
		u32 const max_line_width,
		bool const is_multiline,
		u32 const start_selection,
		u32 const end_selection
	)
{
	make_ui_vertices(
		output,
		text,
		font,
		position,
		text_color,
		selection_color,
		max_line_width,
		is_multiline,
		start_selection,
		end_selection
	);
}

} // namespace render
} // namespace xray
