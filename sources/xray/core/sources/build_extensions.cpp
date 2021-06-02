////////////////////////////////////////////////////////////////////////////
//	Created 	: 13.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "build_extensions.h"

static char s_print_build_id_string[]	=	"print_build_id";
static xray::command_line::key			s_print_build_id	(s_print_build_id_string, "", "", "prints build id to stdout");
static pcstr s_build_date				= __DATE__;

namespace xray			{
namespace command_line	{

bool	key_is_set	( pcstr key_raw );

} // namespace command_line
} // namespace xray

bool   xray::build::print_build_id_command_line ()
{
	static u32 s_out_result						=	u32(-1);
	if ( s_out_result == u32(-1) )
	{
		if ( xray::command_line::initialized() )
			s_out_result						=	s_print_build_id;
		else
			s_out_result						=	xray::command_line::key_is_set(s_print_build_id_string);
	}

	return											!!s_out_result;
}

struct date {
	u32	day;
	u32 month;
	u32 year;

	inline	date				( u32 const day_, u32 const month_, u32 const year_ ) :
		day						( day_ ),
		month					( month_ ),
		year					( year_ )
	{
	}
}; // struct date

static u32	day_count			( date const &date )
{
	u32 const years				= date.year;
	u32 const extra_day_count	= ((years % 400 == 0) || ((years % 4 == 0) && (years % 100 != 0))) ? 1 : 0;
	u32 const years_minus_1		= u32(years - 1);
	u32	result					= years_minus_1*365 + years_minus_1/4 - years_minus_1/100 + years_minus_1/400;

	u32 const months			= date.month;
	if ( months >  1 ) result	+= 31;
	if ( months >  2 ) result	+= 28 + extra_day_count;
	if ( months >  3 ) result	+= 31;
	if ( months >  4 ) result	+= 30;
	if ( months >  5 ) result	+= 31;
	if ( months >  6 ) result	+= 30;
	if ( months >  7 ) result	+= 31;
	if ( months >  8 ) result	+= 31;
	if ( months >  9 ) result	+= 30;
	if ( months > 10 ) result	+= 31;
	if ( months > 11 ) result	+= 30;

	return						( result + date.day - 1 );
}

static u32 build_id				( u32 const day, u32 const month, u32 const year, pcstr current_date )
{
	date						current(1,1,1);
	string16					month_string;
	string256					buffer;
	xray::strings::copy			( buffer, current_date );
	XRAY_SSCANF					( buffer, "%s %d %d", XRAY_SCANF_STRING(month_string, sizeof(month_string)), &current.day, &current.year );

	static pstr month_id[12]	= {
		"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"
	};

	current.month				= 0;
	for ( int i=0; i<12; i++ ) {
		if ( xray::strings::compare_insensitive( month_id[ i ], month_string ) )
			continue;

		current.month			= i + 1;
		break;
	}

	u32							start_days = day_count( date ( day, month, year ) );
	u32							stop_days = day_count( current );
	ASSERT						( stop_days >= start_days );
	return						( stop_days - start_days );
}

u32 xray::build::calculate_build_id ( pcstr current_date )
{
	// start development date (26.08.2008)
	return						build_id( 26, 8, 2008, current_date );
}

void xray::build::preinitialize	( pcstr const build_date )
{
	s_build_date				= build_date;
}

void xray::build::initialize	( core::engine *  )
{
	u32 const id	=	calculate_build_id ( s_build_date );
	LOG_INFO					(
		"%s build %d(internal id %d), %s",
		XRAY_ENGINE_ID,
		build_station_build_id(),
		id,
		s_build_date
	);

	if ( s_print_build_id )
	{
		LOG_FORCED			(logging::info,
							 logging::format_message, 
							 logging::log_to_console,
						     "%d", 
							 id);

		debug::terminate	("");
	}
}

pcstr xray::build::build_date	( )
{
	return					s_build_date;
}