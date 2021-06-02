////////////////////////////////////////////////////////////////////////////
//	Created		: 20.09.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef EFFECT_CONSTANT_STORAGE_H_INCLUDED
#define EFFECT_CONSTANT_STORAGE_H_INCLUDED

#include <xray/render/core/quasi_singleton.h>
#include <xray/render/core/shader_constant_defines.h>

namespace xray {
namespace render {

struct data_indexer
{
	data_indexer(u32* data, u32 type): data_ptr(data), class_id(type) {}
	u32*			data_ptr;
	u32				class_id;
}; // struct data_indexer

bool constant_data_predicate( data_indexer const& left, data_indexer const& right );

struct fixed_constants_data_buffer;

struct effect_constant_storage : public quasi_singleton<effect_constant_storage>
{
	typedef xray::render::vector<data_indexer> indexers_vector_type;
	
	effect_constant_storage();
	~effect_constant_storage();
	
	void clear();
	
	template<class T> T* store_constant(T const value)
	{
		COMPILE_ASSERT(	constant_type_traits<T>::size%4 == 0, The_type_must_be_multiple_to_4_bytes );
		
		u32 class_id = constant_type_traits<T>::class_id;
		
		data_indexer to_find((u32*)&value, class_id);
		
		indexers_vector_type::iterator found = std::lower_bound(m_indexers.begin(), m_indexers.end(), to_find, constant_data_predicate);
		
		while (found!=m_indexers.end())
		{
			if (is_equal(found->data_ptr, (u32*)&value, sizeof(value) / 4))
			{
				return (T*)found->data_ptr;
			}
			++found;
		}
		
		if (!m_constant_buffer)
			m_constant_buffer = NEW(fixed_constants_data_buffer);
		
		if (m_constant_buffer->is_overflowing(sizeof(T)))
		{
			fixed_constants_data_buffer* new_buffer = NEW(fixed_constants_data_buffer);
			new_buffer->next						= m_constant_buffer;
			m_constant_buffer						= new_buffer;
		}
		
		T* new_data = new(m_constant_buffer->place<T>())(T);
		*new_data = value;
		
		data_indexer to_insert((u32*)new_data,class_id);
		
		indexers_vector_type::iterator where_insert = std::lower_bound(m_indexers.begin(), m_indexers.end(), to_insert, constant_data_predicate);
		
		if (where_insert!=m_indexers.end())
		{
			m_indexers.insert(where_insert,1,to_insert);
		}
		else
			m_indexers.push_back(to_insert);
		
		return new_data;
	}
	template<> bool* store_constant(bool const value) { return (bool*)store_constant<u32>(u32(value)); }
	
private:
	bool is_equal(u32 const* a_ptr, u32 const* b_ptr, u32 const num_comparision);
	
	indexers_vector_type m_indexers;
	
	fixed_constants_data_buffer*	m_constant_buffer;
}; // struct effect_constant_storage


// Temporary class.
struct fixed_constants_data_buffer
{
	enum { buffer_size = 1 * Kb };
	
	fixed_constants_data_buffer(): m_pos(0), next(0) {}
	
	bool is_overflowing(u32 num_need_bytes) const { return m_pos + num_need_bytes > buffer_size; }
	
	template<class T> pbyte place()
	{ 
		pbyte mem	= &m_data[m_pos];
		
		m_pos		+=sizeof(T); 
		
		ASSERT( m_pos <= buffer_size );
		
		return mem;
	}
	
	fixed_constants_data_buffer* next;
	
private:
	byte m_data[buffer_size];
	u32  m_pos;
}; // struct fixed_constants_data_buffer


} // namespace render 
} // namespace xray 

#endif // #ifndef EFFECT_CONSTANTS_BUFFER_H_INCLUDED