////////////////////////////////////////////////////////////////////////////
//	Created		: 13.09.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "game_resman_test_utils.h"
#include "game_resman_test_resource_with_quality.h"

namespace xray {
namespace resources {

test_resource_with_quality::test_resource_with_quality (pcstr name) 
:	test_resource						(u32(-1), name, 0, max_quality_levels_count),
	m_increasing_quality				(false),
	m_parent_query						(NULL)
{
	for ( u32 i=0; i<max_quality_levels_count; ++i )
		m_qualities_types[i]			=	s_class_ids[s_random.random(array_size(s_class_ids))];

}

test_resource_with_quality::~test_resource_with_quality ()
{
	R_ASSERT							(!m_increasing_quality);
}

bool   test_resource_with_quality::add_quality (test_resource_ptr child, u32 quality)
{
	for ( u32 i=0; i<array_size(m_qualities); ++i )
	{
		if ( !m_qualities[i] )
		{
			m_qualities[i].initialize_as_quality(this, child.c_ptr(), quality);
			return						true;
		}
	}

	return								false;
}

void   test_resource_with_quality::decrease_quality (u32 new_best_quality)
{
	for ( u32 i=0; i<array_size(m_qualities); ++i )
	{
		u32 const child_quality		=	this->child_quality(m_qualities[i].c_ptr());
		if ( child_quality != u32(-1) && child_quality < new_best_quality )
			m_qualities[i].set_zero		();
	}
}

void   test_resource_with_quality::on_quality_loaded (queries_result & results)
{
	R_ASSERT							(m_increasing_quality);
	u32 const target_quality		=	target_quality_level();

	bool const increasing			=	!m_parent_query;

	bool some_quality_loaded		=	false;
	bool some_qualities_failed		=	false;
	for ( int i=results.size()-1; i>=0; i-- )
	{
		u32 const current_result_quality	=	target_quality + i;
		if ( results[i].is_successful() )
		{
			R_ASSERT					(!some_qualities_failed);
		}
		else
		{
			LOGI_INFO					("grm", "%s %s failed", 
												increasing ? "increasing" : "loading",
												results[i].get_requested_path());
			some_qualities_failed	=	true;
			continue;
		}

		some_quality_loaded			=	true;

		test_resource_ptr quality_child =	static_cast_resource_ptr<test_resource_ptr>(results[i].get_unmanaged_resource());
		add_quality						(quality_child, current_result_quality);
	}


	LOGI_INFO							("grm", "%s qualities(->%d) for %s %s", 
										increasing ? "increasing" : "loading",
										current_quality_level(),
										resources::log_string(this).c_str(),
										some_quality_loaded ? "successful" : "failed");
	if ( m_parent_query )
	{
		m_parent_query->finish_query	(some_quality_loaded ? 
										 cook_base::result_success : cook_base::result_error);
		m_parent_query				=	NULL;
	}
	m_increasing_quality			=	false;
}

void   test_resource_with_quality::increase_quality_to_target (query_result_for_cook * parent_query) 
{ 
	m_parent_query					=	parent_query;

	u32 const target_quality		=	target_quality_level();
	pstr unused_buffer				=	NULL;
	STR_DUPLICATEA						(unused_buffer, "dont read/write this stack data");
	fixed_vector< creation_request, max_quality_levels_count >	all_requests;
	fixed_string<64>					names[max_quality_levels_count];

	float satisfactions[max_quality_levels_count];
	u32	quality_indexes[max_quality_levels_count];

	for ( u32 i=0; i<max_quality_levels_count; ++i )
	{
		quality_indexes[i]			=	i;
		u32 const size				=	u32( max_resource_size - ((float)i / (max_quality_levels_count - 1)) * 
																 (max_resource_size - min_resource_size));

		names[i].assignf				("quality %d%s for %d", 
										 i, 
										 test_allocator_name( m_qualities_types[i] ), 
										 int_name());

		all_requests.push_back			( creation_request( names[i].c_str(), const_buffer( unused_buffer, size ), m_qualities_types[i] ) );
		satisfactions[i]			=	m_parent_query ? 
										m_parent_query->satisfaction(i) : satisfaction(i);
	}
	
	creation_request * requests		=	&*all_requests.begin() + target_quality;
	u32 const current_quality		=	current_quality_level();
	u32 const requests_count		=	(current_quality == u32(-1)) ? 
										quality_levels_count() : current_quality - target_quality;
	
	query_resource_params				params (NULL, 
												requests, 
												requests_count, 
												boost::bind(& test_resource_with_quality::on_quality_loaded, this, _1),
												& memory::g_mt_allocator, 
												satisfactions, 
												NULL, NULL, parent_query, NULL, 
												quality_indexes);

	m_increasing_quality			=	true;
	query_resources						(params);
}

bool   test_resource_with_quality::is_increasing_quality	() const 
{ 
	return								m_increasing_quality; 
}

} // namespace resources
} // namespace xray
