////////////////////////////////////////////////////////////////////////////
//	Module 		: shared_string_manager.cpp
//	Created 	: 26.08.2006
//  Modified 	: 26.08.2006
//	Author		: Dmitriy Iassenev
//	Description : shared string manager
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <cs/core/os_include.h>

CS_CORE_API shared_string_manager*		g_shared_string_manager = 0;

shared_string_manager::~shared_string_manager	()
{
	collect_garbage						();
	if (m_storage.empty())
		return;

	OUTPUT_DEBUG_STRING					("shared strings leaked(");
	string16							temp;
	cs::sprintf							(temp, "%d", m_storage.size());
	R_ASSERT							(!m_storage.size(), "too many shared strings to convert to string");
	OUTPUT_DEBUG_STRING					(temp);
	OUTPUT_DEBUG_STRING					("):\r\n");
	STORAGE::const_iterator				I = m_storage.begin();
	STORAGE::const_iterator				E = m_storage.end();
	for ( ; I != E; ++I) {
		OUTPUT_DEBUG_STRING				("[");
		cs::sprintf						(temp, "%d", m_storage.size());
		R_ASSERT						(!m_storage.size(), "too many shared string references to convert to string");
		OUTPUT_DEBUG_STRING				(temp);
		OUTPUT_DEBUG_STRING				(" count] ");
		OUTPUT_DEBUG_STRING				((*I)->value());
		OUTPUT_DEBUG_STRING				("\r\n");
	}
}

shared_string_manager::profile_ptr shared_string_manager::shared_string	(pcstr value)
{
#pragma warning(push)
#pragma warning(disable:4815)
	shared_string_profile				query;
#pragma warning(pop)
	
	value								= value ? value : "";
	shared_string_profile::create_temp	(value,query);

	m_mutex.lock						();
	STORAGE::const_iterator				I = m_storage.find(&query);
	STORAGE::const_iterator				E = m_storage.end();
	for ( ; I != E; ++I) {
		if ((*I)->checksum() != query.checksum_no_check())
			break;

		if ((*I)->length() != query.length_no_check())
			continue;

		if (sz_cmp((*I)->value(),value))
			continue;

		m_mutex.unlock					();
		return							*I;
	}
	m_mutex.unlock						();

	profile_ptr							temp = shared_string_profile::create(value,query);

	m_mutex.lock						();
	m_storage.insert					(temp);
	m_mutex.unlock						();

	return								temp;
}

void shared_string_manager::check_consistency							()
{
	m_mutex.lock						();
	
	STORAGE::const_iterator				I = m_storage.begin();
	STORAGE::const_iterator				E = m_storage.end();
	for ( ; I != E; ++I) {
		printf("",(*I)->length());
		printf("",(*I)->checksum());
	}

	m_mutex.unlock						();
}

struct remove_predicate {
	inline	bool	operator()	(shared_string_profile* string) const
	{
		if (string->reference_count())
			return						false;

		shared_string_profile::destroy	(string);
		return							true;
	}
};

void shared_string_manager::collect_garbage								()
{
	m_mutex.lock						();

	m_storage.erase						(
		std::remove_if(
			m_storage.begin(),
			m_storage.end(),
			remove_predicate()
		),
		m_storage.end()
	);

	m_mutex.unlock						();
}

int	shared_string_manager::compute_stats								(STORAGE const& storage)
{
	int									result = 0;
	result								-= sizeof(*this) + 0;
	result								-= sizeof(STORAGE::allocator_type);
	const int							node_size = 20;
	STORAGE::const_iterator				I = storage.begin();
	STORAGE::const_iterator				E = storage.end();
	for ( ; I != E; ++I) {
		if (!(*I)->reference_count())
			continue;

		result							-= node_size; // for node in tree
		result							-= 16; // for string_profile itself
		result							+= ((*I)->reference_count() - 1)*((*I)->length() + 1); // for string_profile itself
	}

	return								result;
}

int	shared_string_manager::compute_stats								()
{
	m_mutex.lock						();
	int									result = compute_stats(m_storage);
	m_mutex.unlock						();
	return								result;
}

void shared_string_manager::dump										()
{
	m_mutex.lock						();
	STORAGE								storage = m_storage;
	m_mutex.unlock						();

	msg									(cs::message_initiator_core, cs::core::message_type_information,"shared string manager dump: (%d objects)",storage.size());
	msg									(cs::message_initiator_core, cs::core::message_type_information,"Refs. Length    CRC    Value");
	
	{
		STORAGE::const_iterator			I = storage.begin();
		STORAGE::const_iterator			E = storage.end();
		for ( ; I != E; ++I)
			msg							(cs::message_initiator_core, cs::core::message_type_information,"[%4d][%4d][%8x] \"%s\"",(*I)->reference_count(),(*I)->length(),(*I)->checksum(),(*I)->value());
	}

	msg									(cs::message_initiator_core, cs::core::message_type_information,"economy : %i",compute_stats(storage));
}
