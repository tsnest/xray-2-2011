////////////////////////////////////////////////////////////////////////////
//	Created		: 13.09.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef OBJECT_COLLISION_GEOMETRY_SET_PLANE_ACTION_H_INCLUDED
#define OBJECT_COLLISION_GEOMETRY_SET_PLANE_ACTION_H_INCLUDED

using namespace System;

namespace xray
{
	namespace editor
	{
		ref class object_collision_geometry_mesh;

		ref class object_collision_geometry_set_plane_action: editor_base::action_single
		{
			typedef editor_base::action_single	super;
		public:

			object_collision_geometry_set_plane_action		( String^ name, editor_base::scene_view_panel^ view );

		private:
			editor_base::scene_view_panel^	m_window_view;
			object_collision_geometry_mesh^	m_object_collision_geometry_mesh;

		public:
			virtual bool		do_it							( ) override;
			void				set_collision_geometry_mesh		( object_collision_geometry_mesh^ maesh );


		}; // class object_collision_geometry_set_plane_action

	} // namespace editor
} // namespace xray

#endif // #ifndef OBJECT_COLLISION_GEOMETRY_SET_PLANE_ACTION_H_INCLUDED