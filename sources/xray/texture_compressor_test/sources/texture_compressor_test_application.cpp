////////////////////////////////////////////////////////////////////////////
//	Created		: 19.03.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "texture_compressor_test_application.h"
#include "texture_compressor_test_memory.h"
#include <xray/core/core.h>
#include <xray/core/simple_engine.h>
#include <xray/os_include.h>		// for GetCommandLine
#include <io.h>

#include <conio.h>

#include <amd/compress/ATI_Compress.h>
#include <amd/compress/texture_compressor/api.h>

using xray::texture_compressor_test::application;

xray::memory::doug_lea_allocator_type	xray::texture_compressor_test::g_allocator;

class texture_compressor_test_core_engine : public xray::core::simple_engine
{
public:
	virtual	pcstr	get_mounts_path		( ) const	{ return "../../mounts"; }
	virtual	pcstr	get_resources_xray_path	( ) const			{ return "../../resources/sources"; }
};

typedef texture_compressor_test_core_engine									core_engine_type;
static xray::uninitialized_reference< texture_compressor_test_core_engine >	s_core_engine;

void application::initialize( )
{
	m_exit_code				= 0;

	XRAY_CONSTRUCT_REFERENCE( s_core_engine, core_engine_type );

	core::preinitialize		(
		&*s_core_engine,
		core::create_log, 
		GetCommandLine(), 
		command_line::contains_application_true,
		"texture_compressor_test",
		__DATE__
	);

	g_allocator.do_register	( 16*1024*1024, "texture_compressor_test" );

	memory::allocate_region	( );

	core::initialize		( "../../resources/sources/", "texture_compressor_test", core::perform_debug_initialization );
}

void application::finalize	( )
{
	core::finalize			( );

	XRAY_DESTROY_REFERENCE	( s_core_engine );
}

//#include <nvidia/DDSUtilities/inc/dxtlib/dxtlib.h>
//
//#ifdef WIN64
//#	pragma comment (lib, "../../../nvidia/DDSUtilities/lib/nvDXTlibMTDLL.vc8.x64.lib" )
//#else // #ifdef WIN64
//#	ifdef _DLL
//#		pragma comment (lib, "../../../nvidia/DDSUtilities/lib/nvDXTlibMTDLL.vc8.lib" )
//#	else // #ifdef _DLL
//#		pragma comment (lib, "../../../nvidia/DDSUtilities/lib/nvDXTlibMT.vc8.lib" )
//#	endif // #ifdef _DLL
//#endif // #ifdef WIN64

static xray::command_line::key
	s_target_format(
		"target_format",
		"tf",
		"texture compressor",
		""
	);

static xray::command_line::key
	s_input_file(
		"input_file",
		"i",
		"texture compressor",
		""
	);

static xray::command_line::key
	s_output_file(
		"output_file",
		"o",
		"texture compressor",
		""
	);

static xray::command_line::key
	s_compression_speed(
		"compression_speed",
		"cs",
		"texture compressor",
		""
	);

static xray::command_line::key
	s_enable_multithreading(
		"enable_multithreading",
		"em",
		"texture compressor",
		""
	);

pvoid texture_compressor_allocate	( u32 size, bool temp_data )
{
	XRAY_UNREFERENCED_PARAMETER		( temp_data );
	return							MALLOC( size, "texture_compressor" );
}

void texture_compressor_deallocate	( pvoid buffer )
{
	FREE							( buffer );
}

//static NV_ERROR_CODE nv_callback(const void * Data, size_t NumBytes, const MIPMapData * MipMapData, void* UserData )
//{
//	(void)&Data;
//	(void)&NumBytes;
//	(void)&MipMapData;
//	(void)&UserData;
//
//	//if (!MipMapData)
//	//{
//	//	return NV_FAIL;
//	//}
//	//int width = MipMapData->width, 
//	//	height = MipMapData->height,
//	//	mip_index = MipMapData->mipLevel;
//
//	return NV_OK;
//}

#pragma pack(push,1)
struct TgaHeader {
	u8	id_length;
	u8	colormap_type;
	u8	image_type;
	u16	colormap_index;
	u16	colormap_length;
	u8	colormap_size;
	u16	x_origin;
	u16	y_origin;
	u16	width;
	u16	height;
	u8	pixel_size;
	u8	flags;

	enum { Size = 18 };		//const static int SIZE = 18;
};
#pragma pack(pop)


void convert_to_32bpp(pcstr filename )
{
	FIBITMAP* fb			= FreeImage_Load(FIF_TARGA, filename );
	int bpp					= FreeImage_GetBPP(fb);
	if(bpp!=32)
	{
		FIBITMAP* fb32		= FreeImage_ConvertTo32Bits( fb );
		FreeImage_Save		(FIF_TARGA, fb32, filename, TARGA_DEFAULT);
		FreeImage_Unload	(fb32);
	}
	FreeImage_Unload		(fb);
}

struct dds_buffer_to_store
{
	u8*		data;
	u32		size;
	u32		current;
};

//NV_ERROR_CODE nv_write_callback(const void * buffer, 
//								size_t count, 
//								const MIPMapData * mipMapData, // if nz, this is MIP data
//								void* userData)
//{
//	dds_buffer_to_store* ud = (dds_buffer_to_store*)userData;
//	memcpy					(ud->data+ud->current, buffer, count);
//	ud->current				+= count;
//	return					NV_OK;
//}

void make_dds(pcstr filename )
{
//	FIBITMAP* fb				= FreeImage_Load(FIF_TARGA, filename );
//	FreeImage_FlipVertical		( fb );
//	BYTE* pdata					= FreeImage_GetBits( fb );
//	u32 width					= FreeImage_GetWidth( fb );
//	u32 height					= FreeImage_GetHeight( fb );
//	u32 bpp						= FreeImage_GetBPP( fb );
//	const u32 pitch				= FreeImage_GetPitch( fb );
//
//	nvCompressionOptions		nvOptions;
//
//	bool generate_mip_maps		= true;
//	if(generate_mip_maps)
//		nvOptions.GenerateMIPMaps		(0);
//	else
//		nvOptions.DoNotGenerateMIPMaps	( );
//
//	nvOptions.mipFilterType		= kMipFilterKaiser;
//	nvOptions.textureType		= kTextureTypeTexture2D; //kTextureTypeCubeMap
//	nvOptions.textureFormat		= kDXT1;
////	nvOptions.bForceDXT1FourColors = true;
//	nvOptions.quality			= kQualityHighest;
//	nvOptions.bRGBE				= false;
//
//	dds_buffer_to_store			buffer;
//	buffer.current				= 0;
//	buffer.size					= 10	*1024*1024;// 10mb
//	buffer.data					= (u8*)MALLOC( buffer.size,"dds file");
//	nvOptions.user_data			= &buffer;
//
//	//RGBAMipMappedCubeMap	pcubemap;
//	//pcubemap.resize			(height, height, 1);
//
//	//for(int i=0; i<6; ++i)
//	//{
//	//	RGBAMipMappedImage& face= pcubemap[i];
//	//	RGBAImage&	face_mip	= face[0];
//	//	face_mip.SetRGBE		(false);
//	//	rgba_t* pixels			= face_mip.pixels();
//	//	u32 face_width			= height;
//	//	int offset_x			= i*face_width;
//
//	//	FIBITMAP* cube_face	= FreeImage_Copy(fb, 
//	//										offset_x, 
//	//										0, 
//	//										offset_x+face_width, 
//	//										height);
//
//	//	BYTE* face_bytes		= FreeImage_GetBits(cube_face);
//	//	//memcpy				( pixels, face_bytes, face_width*height*(bpp/8) );
//	//	u8* pixel				= face_bytes;
//	//	for (u32 k=0; k<face_width*height; k++,pixel+=3)
//	//	{
//	//		pixels[k].b = pixel[0];
//	//		pixels[k].g = pixel[1];
//	//		pixels[k].r = pixel[2];
//	//		pixels[k].a = 0;
//	//	}
//
//	//	FreeImage_Unload		( cube_face );
//	//}
//	//
//	//HRESULT hr	= nvDDS::nvDXTcompress(	pcubemap, 
//	//								&nvOptions,
//	//								nv_write_callback );
//
//	HRESULT hr	= nvDDS::nvDXTcompress(	pdata, 
//									width, 
//									height, 
//									pitch,
//									(bpp==32) ? nvBGRA : nvBGR,
//									&nvOptions,
//									nv_write_callback );
//
//	pstr temp						= 0;
//	STR_JOINA		( temp, filename, ".dds" );
//	FILE* f			= fopen( temp, "wb");
//	u32 written		= fwrite(buffer.data, 1, buffer.current, f);
//	fclose			( f );
//	FREE			( buffer.data );
}

void process_folder(LPCSTR folder_name);
u32 counter = 0;

bool ends_with( pcstr string, pcstr tail )
{
	int string_len = strlen(string);
	int tail_len = strlen(tail);
	return strstr(string,tail) == (string+string_len-tail_len);
}

void process_fdata(LPCSTR folder_name, _finddata32_t& fd)
{
	if(0==_stricmp(fd.name, ".") || 0==_stricmp(fd.name, "..") )
		return;

	char						path[1024];
	strcpy_s					(path, sizeof(path), folder_name);
	strcat_s					(path, sizeof(path), "\\");
	strcat_s					(path, sizeof(path), fd.name);

	if(fd.attrib & _A_SUBDIR)
	{
		process_folder(path);
	}else
	{
		pcstr skip		= "_nmap.tga";
		if(ends_with(path,skip))
		{
			LOG_INFO("skip nmap: %s\n", path);
			return;
		}else
//		if(ends_with(path,".tga"))
		if(ends_with(path,"grass_dry.tga"))
//		if(ends_with(path,"baked_keg_nmap.tga"))
		{
			LOG_INFO("convert :%s\n", path);
//			convert_to_32bpp(path);
			make_dds(path);
		}else
			LOG_INFO("skip: %s\n", path);
	}
}

void process_folder(LPCSTR folder_name)
{
	_finddata32_t				finddata;
	char						path[1024];
	
	strcpy_s					(path, sizeof(path), folder_name);
	strcat_s					(path, sizeof(path), "\\*.*");

	intptr_t fres				= _findfirst32(path, &finddata);
	if(fres!=-1)
	{
		process_fdata(folder_name, finddata);

		while(_findnext32(fres, &finddata) == 0)
			process_fdata(folder_name, finddata);

		_findclose( fres );
	}
}

void application::execute	( )
{
//	process_folder		("e:\\stalker2\\resources\\sources\\textures");


/*
	fixed_string<512>				format_string;
	
	s_target_format.initialize("DXT1");
	s_input_file.initialize("D:/test.tga");
	s_output_file.initialize("D:/out_test.dds");
	//typedef enum
	//{
	//	ATI_TC_Speed_Normal,                      ///< Highest quality mode
	//	ATI_TC_Speed_Fast,                        ///< Slightly lower quality but much faster compression mode - DXTn & ATInN only
	//	ATI_TC_Speed_SuperFast,                   ///< Slightly lower quality but much, much faster compression mode - DXTn & ATInN only
	//} ATI_TC_Speed;
	s_compression_speed.initialize("1");

	if ( !s_target_format.is_set_as_string(&format_string) ) {
		LOG_ERROR					( "please specify texture format" );
		m_exit_code					= u32(-1);
		return;
	}

	ATI_TC_FORMAT const target_format	= texture_compressor::parse_format( format_string.c_str() );
	if ( target_format == ATI_TC_FORMAT_Unknown ) {
		LOG_ERROR					( "unknown texture format" );
		m_exit_code					= u32(-2);
		return;
	}
	
	fixed_string<512>				input_file;
	if ( !s_input_file.is_set_as_string(&input_file) ) {
		LOG_ERROR					( "please specify input file" );
		m_exit_code					= u32(-3);
		return;
	}

	fixed_string<512>				output_file;
	if ( !s_output_file.is_set_as_string(&output_file) ) {
		LOG_ERROR					( "please specify output file" );
		m_exit_code					= u32(-4);
		return;
	}

	float compression_speed			= -1.f;
	if ( !s_compression_speed.is_set_as_number(&compression_speed) ) {
		LOG_ERROR					( "please specify compression speed" );
		m_exit_code					= u32(-5);
		return;
	}

	FILE* file;
	errno_t error					= fopen_s( &file, input_file.c_str(), "rb" );
	if ( error ) {
		m_exit_code					= u32(-6);
		return;
	}

	u32 const buffer_size			= _filelength( _fileno(file) );
	u8* buffer						= (u8*)MALLOC(buffer_size,"tga_source");
	u32 const read_bytes			= (u32)fread_s( buffer, buffer_size, 1, buffer_size, file );
	if ( read_bytes != buffer_size ) {
		m_exit_code					= u32(-7);
		return;
	}

	fclose							( file );



	ATI_TC_Texture ati_tex;
	xray::texture_compressor::load_tga(
		&texture_compressor_allocate,
		&texture_compressor_deallocate,
		buffer, 
		ati_tex
		);

	nvCompressionOptions nvOptions;
	nvOptions.mipMapGeneration	= kGenerateMipMaps;
	nvOptions.numMipMapsToWrite = 0;
	nvOptions.mipFilterType		= kMipFilterKaiser;
	nvOptions.textureType		= kTextureTypeTexture2D;
	nvOptions.textureFormat		= kDXT1;
	nvOptions.quality			= kQualityFastest;
	nvOptions.bRGBE				= false;
	nvOptions.user_data			= 0;

	DWORD time0 = timeGetTime();
	nvDDS::nvDXTcompress(ati_tex.pData,1024,1024,0,nvBGRA,&nvOptions,nv_callback);
	printf("\nnv:%d",timeGetTime()-time0);

	//TODO

	time0 = timeGetTime();
	u32 new_buffer_size				= 0;
	u8* destination_buffer			=
		create_texture(
			new_buffer_size,
			&texture_compressor_allocate,
			&texture_compressor_deallocate,
			buffer,
			target_format,
			(texture_compressor::compression_speed_fast),
			true,//s_enable_multithreading.is_set(),
			buffer_size,
			true,
			xray::texture_compressor::texture_2D
		);
	printf("\nati:%d",timeGetTime() - time0);
	FREE							( buffer );
	_getch();

	error							= fopen_s( &file, output_file.c_str(), "wb" );
	if ( error ) {
		m_exit_code					= u32(-8);
		return;
	}

	fwrite							( destination_buffer, 1, new_buffer_size, file );
	fclose							( file );

	FREE							( destination_buffer );

	m_exit_code						= s_core_engine->get_exit_code();
*/
/*
	source_texture.pData,
	source_texture.dwWidth,
	source_texture.dwHeight,
	source_texture.dwPitch,
*/
}