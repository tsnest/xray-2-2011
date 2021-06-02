////////////////////////////////////////////////////////////////////////////
//	Created		: 01.07.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#ifndef SOUND_OBJECT_COLLISION_H_INCLUDED
#define SOUND_OBJECT_COLLISION_H_INCLUDED

#pragma managed(push,off)
#include <xray/collision/common_types.h>
#pragma managed(pop)

namespace xray {
namespace collision { 
	class geometry_instance; 
	struct space_partitioning_tree;
} // namespace collision
namespace sound_editor {

	class sound_object_collision_dynamic;
	ref class sound_object_instance;

	public ref struct sound_object_collision
	{
	public:
							sound_object_collision	(collision::space_partitioning_tree* tree);
		void				create_from_geometry	(bool user_geom, sound_object_instance^ o, collision::geometry_instance* geom, xray::collision::object_type t);
		aabb				get_aabb				();
		void				destroy					(memory::base_allocator* allocator);
		void				insert					();
		void				remove					();
		void				set_matrix				(float4x4 const* m);
		bool				initialized				();

		collision::object*	get_collision_object	();

	public:
		collision::space_partitioning_tree* m_collision_tree;

	private:
		collision::geometry_instance*		m_user_geometry;
		sound_object_collision_dynamic*		m_collision_obj;
		bool								m_b_active;
	};
} // namespace sound_editor
} // namespace xray
#endif // #ifndef SOUND_OBJECT_COLLISION_H_INCLUDED