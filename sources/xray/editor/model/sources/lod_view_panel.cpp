#include "pch.h"
#include "lod_view_panel.h"


namespace xray{
namespace model_editor {

using namespace xray::editor::wpf_controls::hypergraph;

void lod_view_panel::in_constructor( )
{
	xray::editor::wpf_controls::hypergraph::hypergraph_control^ hg = hypergraph_host1->hypergraph;
	hg->viewport_origin			= System::Windows::Vector( 0, 0 );
	hg->field->Width			= 1024;
	hg->field->Height			= 768;
	hg->field->is_fixed_size	= true;
	hg->is_nodes_moveable		= false;
	hg->is_selectable			= true;

}

} // namespace model_editor
} // namespace xray
