////////////////////////////////////////////////////////////////////////////
//	Created		: 16.09.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef MATERIAL_COOKER_H_INCLUDED
#define MATERIAL_COOKER_H_INCLUDED

#include <xray/resources_cook_classes.h>

namespace xray {
namespace render {

struct material_cook : public resources::translate_query_cook {
	material_cook								  ();
	virtual	void translate_query				  (resources::query_result_for_cook& parent);
	virtual void delete_resource				  (resources::resource_base* resource);
			void on_material_config_loaded		  (xray::resources::queries_result& result);
private:
			void on_material_binary_config_loaded	( resources::query_result_for_cook* parent, configs::binary_config* cfg );
			void on_fs_iterator_ready				(vfs::vfs_locked_iterator const & it, resources::query_result_for_cook * parent);
};

} // namespace render
} // namespace xray

#endif // #ifndef MATERIAL_COOKER_H_INCLUDED