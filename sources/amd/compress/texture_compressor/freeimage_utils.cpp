////////////////////////////////////////////////////////////////////////////
//	Created		: 06.04.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "utils.h"

FIBITMAP* get_freeimage_from_memory(u8 const* const tga_file_data, u32 const tga_file_data_size)
{
	FreeImage_SetPluginEnabled		(FIF_TARGA, true);
	FreeImage_SetPluginEnabled		(FIF_TIFF, true);
	
	FIMEMORYHEADER					fh;
	fh.curpos						= 0;
	fh.data							= (void*)(tga_file_data);
	fh.datalen						= tga_file_data_size;
	fh.delete_me					= false;
	fh.filelen						= tga_file_data_size;
	FIMEMORY fm;
	fm.data							= (void*)&fh;
	
	FIBITMAP* original_fb			= 0;
	
	// TODO: Select format.
	
	original_fb			= FreeImage_LoadFromMemory(FIF_TIFF,&fm);

	if (!original_fb)
	{
		fh.curpos		= 0;
		original_fb		= FreeImage_LoadFromMemory(FIF_TARGA,&fm);
	}
	
	return original_fb;
}


