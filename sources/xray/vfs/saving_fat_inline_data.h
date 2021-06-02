////////////////////////////////////////////////////////////////////////////
//	Created		: 26.04.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef VFS_SAVING_FAT_INLINE_DATA_H_INCLUDED
#define VFS_SAVING_FAT_INLINE_DATA_H_INCLUDED

namespace xray {
namespace vfs {

struct XRAY_VFS_API fat_inline_data
{
public:
	struct item
	{
							item		() :	max_size(0), current_size(0), compression_rate(0), 
												allow_compression_in_db(true) {}
		fixed_string<32>				extension;
		u32								max_size;
		u32								current_size;
		float							compression_rate; // 0 - no compression, 1 - compress all
		bool							allow_compression_in_db;
		enum							{ no_limit = u32(-1) };
	};

	typedef	buffer_vector<item>			container;

public:
	fat_inline_data						(container * items) : m_items(items), m_highest_compression_rate(0) {}

	void	push_back					(item const & item);

	bool	find_by_extension 			(item * * out_item, pcstr extension);
	bool	find_by_extension 			(item const * * out_item, pcstr extension) const;

	void	begin_fitting				();

	bool	try_fit_for_inlining		(pcstr file_name, u32 file_size, u32 compressed_size);
	u32		total_size_for_extensions_with_limited_size () const;

	float	highest_compression_rate	() const { return m_highest_compression_rate; }
	
private:
	float								m_highest_compression_rate;
	buffer_vector<item> *				m_items;
};

} // namespace vfs
} // namespace xray

#endif // #ifndef VFS_SAVING_FAT_INLINE_DATA_H_INCLUDED