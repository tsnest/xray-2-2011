////////////////////////////////////////////////////////////////////////////
//	Created		: 26.04.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/vfs/saving_fat_inline_data.h>
#include <xray/fs/path_string_utils.h>

namespace xray {
namespace vfs {

bool   fat_inline_data::find_by_extension (item const * * out_item, pcstr extension) const
{
	item * item	=	NULL;
	fat_inline_data * this_non_const	=	const_cast<fat_inline_data *>(this);
	if ( this_non_const->find_by_extension(& item, extension) )
	{
		* out_item						=	item;
		return								true;
	}

	return									false;
}

bool   fat_inline_data::find_by_extension (item * * out_item, pcstr extension)
{
	if ( !m_items )
		return								false;

	for ( container::iterator		it	=	m_items->begin(), 
								end_it	=	m_items->end();
									it	!=	end_it;
								  ++it	)
	{
		if ( it->extension == extension )
		{
			* out_item					=	& * it;
			return							true;			
		}
	}

	return									false;
}

u32   fat_inline_data::total_size_for_extensions_with_limited_size () const
{
	if ( !m_items )
		return								0;
	u32 total_size						=	0;
	for ( container::const_iterator	it	=	m_items->begin(), 
								end_it	=	m_items->end();
									it	!=	end_it;
								  ++it	)
	{
		if ( it->current_size != item::no_limit )
			total_size					+=	(it->current_size * 2) + (it->current_size < 10 ? 10 : 0);
	}

	return									total_size;
}

void   fat_inline_data::push_back (item const & item) 
{ 
	R_ASSERT								(m_items);
	m_items->push_back						(item); 
	m_highest_compression_rate			=	math::max(m_highest_compression_rate, item.compression_rate); 
}

void   fat_inline_data::begin_fitting ()
{
	R_ASSERT								(m_items);

	for ( container::iterator	it	=	m_items->begin(), 
								end_it	=	m_items->end();
								it	!=	end_it;
							  ++it	)
	{
		it->current_size			=	it->max_size;
	}
}

bool   fat_inline_data::try_fit_for_inlining (pcstr file_name, u32 const file_size, u32 const compressed_size)
{
	typedef item	item_type;
	item * item;
	if ( find_by_extension(& item, fs_new::extension_from_path(file_name)) )
	{
		if ( compressed_size )
		{
			float const rate		=	(float)compressed_size / (file_size ? file_size : 1);
			if ( rate > item->compression_rate )
				return					false;
		}

		if ( item->current_size == item_type::no_limit )
			return						true;

		u32 const size				=	compressed_size ? compressed_size : file_size;
		if ( item->current_size >= size )
		{
			item->current_size			-=	size;
			return						true;
		}
	}

	return								false;
}

} // namespace vfs
} // namespace xray
