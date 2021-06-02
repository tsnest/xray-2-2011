////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PPMD_COMPRESSOR_H_INCLUDED
#define PPMD_COMPRESSOR_H_INCLUDED

#include "compressor.h"

class ppmd_compressor_impl;

namespace xray {

class XRAY_CORE_API ppmd_compressor : public compressor
{
public:
	 // Method of model restoration at memory insufficiency:
	 //     model_restoration_restart - restart model from scratch (default)
	 //     model_restoration_cut_off - cut off model (nearly twice slower)                   
	 //     model_restoration_freeze  - freeze context tree (dangerous)                    
	enum			model_restoration_enum	{	model_restoration_restart , 
												model_restoration_cut_off, 
												model_restoration_freeze	};

public:
					ppmd_compressor (memory::base_allocator* allocator, 
									 u32 sub_allocator_size_mb,
									 model_restoration_enum model_restoration = model_restoration_restart);

	virtual		   ~ppmd_compressor	();

	virtual signalling_bool	compress	(const_buffer src, mutable_buffer dest, u32& out_size);
	virtual signalling_bool	decompress	(const_buffer src, mutable_buffer dest, u32& out_size);

private:
	memory::base_allocator*			m_allocator;
	ppmd_compressor_impl*			m_impl;
	model_restoration_enum			m_model_restoration;
};

} // namespace xray

#endif // #ifndef PPMD_COMPRESSOR_H_INCLUDED