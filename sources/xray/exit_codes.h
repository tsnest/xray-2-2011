////////////////////////////////////////////////////////////////////////////
//	Created		: 15.11.2010
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_EXIT_CODES_H_INCLUDED
#define XRAY_EXIT_CODES_H_INCLUDED

namespace xray {

enum {
	exit_code_success								=	0,

	exit_code_editor_level_test_project_not_loaded	=	100,
	exit_code_editor_level_not_all_objects_loaded,

	exit_code_last									=	200,

	exit_code_tests_failed							=	10000,
	exit_code_tests_finished_by_test_watcher,

	exit_code_called_by_terminate					=	100000,
}; 

} // namespace xray

#endif // #ifndef XRAY_EXIT_CODES_H_INCLUDED