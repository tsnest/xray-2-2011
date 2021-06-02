////////////////////////////////////////////////////////////////////////////
//	Created		: 09.07.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "project.h"
#include "object_inspector_tab.h"
#include "project_tab.h"
#include "level_editor.h"

namespace xray {
namespace editor {

void project::select_id(u32 id, enum_selection_method method)
{
	item_id_list^ tmp_vector	= gcnew item_id_list;
	
	tmp_vector->Add				( id );

	select_ids					( tmp_vector, method );

}

void project::select_ids(item_id_list^ objs, enum_selection_method method)
{
	project_items_list^ tmp_vector = gcnew project_items_list;
	
	for each( System::UInt32^ it in objs)
	{
		project_item_base^ item	= project_item_base::object_by_id( *it );
		
		if(item->get_selectable())
			tmp_vector->Add		(item);
	}

	select_objects			(tmp_vector, method);
}

void project::select_object( project_item_base^ item, enum_selection_method method )
{
	project_items_list^ tmp_vector = gcnew project_items_list;

	if(item != nullptr)
		tmp_vector->Add		(item);

	select_objects			(tmp_vector, method);
}

ref class objects_enumerator
{
public:
	objects_enumerator(System::Collections::IEnumerator^ en):m_enumerator(en),m_b_valid(true){}
	bool MoveNext	(){m_b_valid = m_enumerator->MoveNext(); return m_b_valid;}
	bool IsValid	(){return m_b_valid;}
	project_item_base^	current() {return safe_cast<project_item_base^>(m_enumerator->Current);}
private:
	bool			m_b_valid;
	System::Collections::IEnumerator^ m_enumerator;
};

ref class object_id_predicate:public System::Collections::Generic::IComparer<project_item_base^>
{
public:
	virtual int Compare( project_item_base^ first, project_item_base^ second )
	{
		u32 id1 = first->id();
		u32 id2 = second->id();
		if(id1==id2) return 0;
		else
			return (id1>id2)?1:-1;
	}
};

project_item_base^ get_locked_root( project_item_base^ itm )
{
	project_item_base^ p = itm->m_parent;
	while(p)
	{
		if ( p->GetType() == project_item_folder::typeid ){
			project_item_folder^ f = safe_cast<project_item_folder^>(p);
			if(f->locked)
				return f;
		}

		p = p->m_parent;
	}
	return itm;
}

void project::select_objects(project_items_list^ items, enum_selection_method const method)
{
	if( method == enum_selection_method_set )
	{
		project_items_list		sel(m_selection_list);

		for each (project_item_base^ it_up in sel)
		{
			if ( items->Contains( it_up ) )	
				continue;

			it_up->set_selected	(false);
		}

		m_selection_list->Clear();
	}

	if( items->Count== 0 )
	{
		object_inspector_tab^ inspector_tab	= m_level_editor->get_object_inspector_tab();
		if(inspector_tab && inspector_tab->Visible)
			inspector_tab->show_properties(m_selection_list);
		
		m_project_tab->set_selected_objects_count(0);

		m_selection_changed();
		
		return;
	}

	project_items_list		t(items);
	items->Clear			( );
	for each (project_item_base^ item in t)
	{
		ASSERT(item->id()!=0);

		if (!items->Contains(item))
			items->Add( item );
	}

	items->Sort					( gcnew object_id_predicate );

	project_items_list			tmp_vec;

	objects_enumerator^ it_old		= gcnew objects_enumerator(selection_list()->GetEnumerator());

	objects_enumerator^ it_new	= gcnew objects_enumerator(items->GetEnumerator());

	project_item_base^ control_object = nullptr;

	// Selective adding elements which are intersected between old selections list and new selection objects 
	// depending on select mode.
	it_old->MoveNext	();
	it_new->MoveNext	();

	while(it_old->IsValid() && it_new->IsValid())
	{
		if( it_old->current()->id() < it_new->current()->id() )
		{
			tmp_vec.Add			(it_old->current());
			it_old->MoveNext	();
		}else 
		if( it_old->current()->id() > it_new->current()->id() )
		{
			if( method == enum_selection_method_set || 
				method == enum_selection_method_add || 
				method == enum_selection_method_invert )
			{
				tmp_vec.Add		(it_new->current());

				//.it_new->current()->set_selected(true);
				control_object		= it_new->current();
			}

			it_new->MoveNext();
		}else
		{
			if( method == enum_selection_method_set || 
				method == enum_selection_method_add )
			{
				tmp_vec.Add		(it_old->current());
			}else
				it_new->current()->set_selected(false);

			it_old->MoveNext();
			it_new->MoveNext();
		}
	}

	// Add all elements from new select list if the selection mode is one of followings
	if( method == enum_selection_method_set || 
		method == enum_selection_method_add || 
		method == enum_selection_method_invert )
		
		while( it_new->IsValid() )
		{
			tmp_vec.Add				(it_new->current());

			//if( !it_new->current()->get_selected	( ) )
			//	it_new->current()->set_selected	(true);

			control_object			= it_new->current();

			it_new->MoveNext		( );
		}

	// Add all elements in the old selection list that aren't intersect with the new list
	while( it_old->IsValid() )
	{
		tmp_vec.Add					( it_old->current() );
		it_old->MoveNext				( );
	}

	m_selection_list->Clear();
	for each(project_item_base^ itm in tmp_vec)
	{
		project_item_base^ i = get_locked_root( itm );
		if(!m_selection_list->Contains(i))
		{
			m_selection_list->Add(i);
			if(!i->get_selected())
				i->set_selected(true);

		}
	}

//	m_selection_list				= %tmp_vec;

	object_inspector_tab^ inspector_tab	= m_level_editor->get_object_inspector_tab();

	if( inspector_tab && inspector_tab->Visible )
		inspector_tab->show_properties	( m_selection_list );

	m_project_tab->set_selected_objects_count	(m_selection_list->Count);
	m_selection_changed							( );
}

void project::pin_selection( )
{
	m_selection_pin = !get_pin_selection();
}

bool project::get_pin_selection( )
{
	return m_selection_pin;
}

bool project::is_object_in_ignore_list( u32 id )
{
	return m_ignore_list.Contains( id );
}

void project::set_object_ignore_list( u32 id, bool b_add )
{
	R_ASSERT(b_add == !m_ignore_list.Contains(id));

	if(b_add)
	{
		m_ignore_list.Add( id );
		select_object(nullptr, enum_selection_method_set);
	}else
	{
		m_ignore_list.Remove( id );
		select_id(id, enum_selection_method_add);
	}
	
}	

} // namespace editor
} // namespace xray