////////////////////////////////////////////////////////////////////////////
//	Created		: 7.12.2009
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef HYPERGRAPH_NODE_STYLE_H_INCLUDED
#define HYPERGRAPH_NODE_STYLE_H_INCLUDED

using namespace System;
using namespace System::Collections;
using namespace System::Drawing;

#include "hypergraph_defines.h"

namespace xray {
namespace editor {
namespace controls {
namespace hypergraph {

	ref class node;
	ref class node_style;

	public interface class nodes_style_manager
	{
	public:
		virtual		void	assign_style	(node^ n, System::String^ style_name);
	}; // interface class nodes_style_manager

	public ref class connection_point_style
	{
	public:
												connection_point_style();
		void									clone(connection_point_style^ style);

		property System::String^				name;
		property connection_type				dir;
		property point_align					al;
		property Color							active_color;
		property Color							highlight_color;
		property Color							inactive_color;
		property float							scale
		{
			void								set(float scl)						{set_scale_impl(scl);};
			float								get()								{return m_scale;};
		};
		property System::Drawing::Rectangle		base_rect
		{
			System::Drawing::Rectangle			get()								{return m_base_rect;};
			void								set(System::Drawing::Rectangle r)	{m_base_rect=r;};
		};
		property System::Drawing::Rectangle		rect
		{
			System::Drawing::Rectangle			get()								{return m_rect;};
			void								set(System::Drawing::Rectangle r)	{set_rect_impl(r);};
		};
	private:
		void									set_rect_impl(System::Drawing::Rectangle r);
		void									set_scale_impl(float scl);
		System::Drawing::Rectangle				m_base_rect;
		System::Drawing::Rectangle				m_rect;
		float									m_scale;
	}; // ref class connection_point_style

	public ref class node_style
	{
	public:
		enum class e_states : int{
			active		= 0,
			highlihted	= 1,
			inactive	= 2,
		}; // enum class e_states

		property System::String^				name;
		property System::Drawing::Font^			font;
		property System::Drawing::Font^			connection_points_font;
		property System::Drawing::Pen^			border_pen;
		property System::Drawing::SolidBrush^	brush;
		property view_mode						mode;
		property float							scale
		{
			float								get()						{return m_scale;};
			void								set(float scl)				{set_scale_impl(scl);};
		};
		property System::Drawing::Size^			full_size;
		property System::Drawing::Size^			brief_size;

		System::Drawing::StringFormat^			fmt;
										
										node_style();
		Color							get_node_color(e_states st);
		Color							get_link_color(e_states st);
		Color							get_point_color(System::String^ n, e_states st);
		void							set_brush_color(e_states st);
		System::Drawing::Size			get_max_point_size();
		System::Collections::ArrayList^	get_point_styles()					{return m_point_styles;};
		void							add_new_style(connection_point_style^ st);
		connection_point_style^			get_point_style(System::String^ style);

	protected:
		void							set_scale_impl(float scl);

		Color							m_color_node_active;
		Color							m_color_node_highlighted;
		Color							m_color_node_inactive;
		Color							m_color_link_active;
		Color							m_color_link_highlighted;
		Color							m_color_link_inactive;
		System::Drawing::Size			m_max_point_size;
		System::Collections::ArrayList^	m_point_styles;
		float							m_scale;

	private:
		bool							m_point_cached;
	}; // ref class node_style
} //namespace hypergraph
} //namespace controls
} //namespace editor
} //namespace xray

#endif // #ifndef HYPERGRAPH_NODE_STYLE_H_INCLUDED