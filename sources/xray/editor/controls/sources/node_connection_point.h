#ifndef NODE_CONNECTION_POINT_H_INCLUDED
#define NODE_CONNECTION_POINT_H_INCLUDED

#include "hypergraph_node.h"
#include "hypergraph_node_style.h"

namespace xray {
namespace editor {
namespace controls {
namespace hypergraph {

ref class connection_point_style;

public ref class connection_point
{
public:
								connection_point	(node^ o_owner, System::Type^ type, connection_point_style^ st);

		System::Drawing::Point			get_point		();
		System::Type^					m_data_type;
		System::Drawing::Rectangle		get_rect		()								{return m_style->rect;};
		void							set_rect		(System::Drawing::Rectangle r)	{m_style->rect=r;};
		System::Drawing::Rectangle		base_rect		()								{return m_style->base_rect;};
		void							set_scale		(float scl)	{m_style->scale=scl;};

		property bool					draw_text;

		property System::Type^ data_type{
			System::Type^			get()	{return m_data_type;}
		}

		property connection_type direction{
			connection_type			get			()					{return m_style->dir;}
			void					set			(connection_type d)	{m_style->dir=d;}
		}
		property String^ name{
			String^	get			()			{return m_style->name;}
			void	set			(String^ n)	{m_style->name=n;}
		}
		property node^ owner{
			node^	get			()			{return m_owner;};
			void	set			(node^ n)	{m_owner=n;};
		}
		
		property point_align	align{
			point_align		get			()							{return m_style->al;};
			void			set			(point_align a)				{m_style->al = a;};
		}

protected:
		node^					m_owner;
		System::String^			m_name;
		connection_point_style^ m_style;
};//class connection_point

public ref class link
{
public:
	property connection_point^		source_point;
	property connection_point^		destination_point;

	property node^ source_node{
		node^	get()			{return source_point->owner;}
		void	set(node^ n)	{source_point->owner=n;}
	}
	property node^ destination_node{
		node^	get()			{return destination_point->owner;}
		void	set(node^ n)	{destination_point->owner=n;}
	}

public:
						link			(connection_point^ src, connection_point^ dst);
	virtual	void		draw			(Drawing::Graphics^ g, Drawing::Pen^ pen, nodes_link_style link_style);	
			bool		pick			(Drawing::Point p);
			bool		intersect		(Drawing::Rectangle rectangle);
			bool		visible			();
			bool		endpoints_equal	(link^ lnk);
			bool		is_relative		(node^ node);
}; // class link

}; // namespace hypergraph
}; // namespace controls
}; // namespace editor
}; // namespace xray

#endif //#ifndef NODE_CONNECTION_POINT_H_INCLUDED