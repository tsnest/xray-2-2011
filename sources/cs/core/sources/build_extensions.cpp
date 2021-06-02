////////////////////////////////////////////////////////////////////////////
//	Module 		: build_extensions.cpp
//	Created 	: 18.03.2007
//  Modified 	: 18.03.2007
//	Author		: Dmitriy Iassenev
//	Description : build extensions
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

using build::calculator;

struct date {
	u32	day;
	u32 month;
	u32 year;

	inline	date			( u32 const day, u32 const month, u32 const year ) :
		day					(day),
		month				(month),
		year				(year)
	{
	}
};

static u32	day_count		( date const& date )
{
	u32 const				days = date.day;
	u32 const				months = date.month;
	u32 const				years = date.year;
	ASSERT					(years > 0);
	ASSERT					(months > 0);
	ASSERT					(days > 0);
	u32						t1,t2,t3,t4;
	t1						= years/400;
	t2						= years/100;
	t3						= years/4;
	bool					a1,a2,a3;
	a1						= !(years%400);
	a2						= !(years%100);
	a3						= !(years%4);
	t4						= a3 && (!a2 || a1) ? 1 : 0;
	u32						result = u32(years - 1)*u32(365) + t1 - t2 + t3;

	if (months >  1) result	+= u32(31);
	if (months >  2) result	+= u32(28 + t4);
	if (months >  3) result	+= u32(31);
	if (months >  4) result	+= u32(30);
	if (months >  5) result	+= u32(31);
	if (months >  6) result	+= u32(30);
	if (months >  7) result	+= u32(31);
	if (months >  8) result	+= u32(31);
	if (months >  9) result	+= u32(30);
	if (months > 10) result	+= u32(31);
	if (months > 11) result	+= u32(30);
	result					+= u32(days - 1);

	return					result;
}

calculator::calculator		( u32 const day, u32 const month, u32 const year )
{
	date					current(1,1,1);
	string16				month_string;
	string256				buffer;
	sz_cpy					(buffer, __DATE__);
	CS_SSCANF				(buffer, "%s %d %d", CS_SCANF_STRING(month_string, sizeof(month_string)), &current.day, &current.year);

	static pstr month_id[12] = {
		"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"
	};

	current.month			= 0;
	for (int i=0; i<12; i++) {
		if (sz_icmp(month_id[i],month_string))
			continue;

		current.month		= i + 1;
		break;
	}

	u32						start_days = day_count(date(day, month, year));
	u32						stop_days = day_count(current);
	ASSERT					(stop_days >= start_days);
	m_build_id				= stop_days - start_days;
}

void build::initialize			()
{
	// start development date (02.03.2005)
	msg						(
		cs::message_initiator_core,
		cs::core::message_type_information,
		"%s build %d, %s\r\n","cs::core",
		calculator(2,3,2005).m_build_id,
		__DATE__
	);
}