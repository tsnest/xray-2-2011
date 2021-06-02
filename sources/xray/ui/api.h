#ifndef XRAY_UI_API_H_INCLUDED
#define XRAY_UI_API_H_INCLUDED

#include <xray/render/engine/base_classes.h>

#ifndef XRAY_UI_API
#	ifdef XRAY_STATIC_LIBRARIES
#		define XRAY_UI_API
#	else // #ifdef XRAY_STATIC_LIBRARIES
#		ifdef XRAY_UI_BUILDING
#			define XRAY_UI_API				XRAY_DLL_EXPORT
#		else // #ifdef XRAY_UI_BUILDING
#			ifndef XRAY_ENGINE_BUILDING
#				define XRAY_UI_API			XRAY_DLL_IMPORT
#			else // #ifndef XRAY_ENGINE_BUILDING
#				define XRAY_UI_API			XRAY_DLL_EXPORT
#			endif // #ifndef XRAY_ENGINE_BUILDING
#		endif // #ifdef XRAY_UI_BUILDING
#	endif // #ifdef XRAY_STATIC_LIBRARIES
#endif // #ifndef XRAY_UI_API


namespace xray {

namespace input {
	struct world;
} // namespace input

namespace render {
	class world;

	namespace ui {
		class renderer;
	} // namespace ui
} // namespace input

namespace ui {

struct engine;
struct world;

typedef memory::doug_lea_allocator_type	allocator_type;

XRAY_UI_API	world*	create_world		(
						engine& engine,
						render::ui::renderer& renderer,
						memory::base_allocator& allocator,
						render::scene_view_ptr const& scene_view
					);
XRAY_UI_API	void	destroy_world		( world*& world);

} // namespace ui
} // namespace xray

#endif // #ifndef XRAY_UI_API_H_INCLUDED