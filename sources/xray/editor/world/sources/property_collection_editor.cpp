////////////////////////////////////////////////////////////////////////////
//	Created 	: 24.12.2007
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "property_collection_editor.h"
#include "property_container.h"
#include "property_collection.h"
#include "window_ide.h"
#include "window_view.h"
#include "editor_world.h"

using System::Type;
using System::String;
using System::Object;
using System::ComponentModel::PropertyDescriptor;
using xray::editor::property_holder_collection;
using Flobbster::Windows::Forms::PropertyBag;
using System::ComponentModel::Design::CollectionEditor;

typedef PropertyBag::PropertySpecDescriptor	PropertySpecDescriptor;

property_collection_editor::property_collection_editor				( xray::editor::editor_world& world, Type^ type) :
	inherited					( type ),
	m_world						( world )
{
}

Type^ property_collection_editor::CreateCollectionItemType			()
{
	return						(property_container::typeid);
}

Object^ property_collection_editor::CreateInstance					(Type^ type)
{
	XRAY_UNREFERENCED_PARAMETER	( type );

	property_container^			container = safe_cast<property_container^>(Context->Instance);
	PropertySpecDescriptor^		descriptor = safe_cast<PropertySpecDescriptor^>(Context->PropertyDescriptor);
	xray::editor::controls::property_value^	raw_value = container->value(descriptor->item);
	property_collection^		collection = safe_cast<property_collection^>(raw_value);
	return						(collection->create());
}

String^ property_collection_editor::GetDisplayText					(Object^ value)
{
	property_container^			container = safe_cast<property_container^>(value);

	property_holder_collection*	collection = container->holder().collection();
	if (!collection)
		return					(container->holder().display_name());

	int							index = collection->index(&container->holder());
	if (index < 0)
		return					(container->holder().display_name());

	ASSERT						((index < (int)collection->size()));
	char						buffer[256];
	collection->display_name	((u32)index, buffer, sizeof(buffer));

	return						(gcnew System::String(buffer));
}

void property_collection_editor::on_move							(Object^ sender, EventArgs^ e)
{
	XRAY_UNREFERENCED_PARAMETERS( sender, e );

	m_world.ide()->view().Invalidate	();
}

property_collection_editor::CollectionForm^ property_collection_editor::CreateCollectionForm	()
{
//	ASSERT						(!m_collection_form);
	m_collection_form			= inherited::CreateCollectionForm();
	m_collection_form->Move		+= gcnew System::EventHandler(this, &property_collection_editor::on_move);
	return						(m_collection_form);
}

Object^	property_collection_editor::EditValue						(
		ITypeDescriptorContext^ context,
		IServiceProvider^ provider,
		Object^ value
	)
{
	if (!m_collection_form || !m_collection_form->Visible)
		return					(inherited::EditValue(context, provider, value));

	property_collection_editor^	editor = gcnew property_collection_editor( m_world, CollectionType);
	return						(editor->EditValue(context, provider, value));
}
