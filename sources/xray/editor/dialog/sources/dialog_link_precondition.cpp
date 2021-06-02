////////////////////////////////////////////////////////////////////////////
//	Created		: 05.02.2010
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "dialog_link_precondition.h"
#include "dialog_expression.h"
#include "dialog_operation_and.h"
#include "dialog_operation_or.h"
#include "dialog_operation_not.h"
#include "dialog_precondition.h"
using xray::dialog_editor::dialog_link_precondition;
using xray::dialog_editor::dialog_expression;
using xray::dialog_editor::dialog_operation_and;
using xray::dialog_editor::dialog_operation_or;
using xray::dialog_editor::dialog_operation_not;
using xray::dialog_editor::dialog_precondition;
using xray::dialog_editor::e_precondition_functions;

dialog_link_precondition::dialog_link_precondition()
{
	m_expression = NEW(dialog_precondition)();
	m_type = e_type::Precondition;
}

dialog_link_precondition::dialog_link_precondition(dialog_expression* expr)
{
	if(dynamic_cast<dialog_operation_and*>(expr))
	{
		m_expression = NEW(dialog_operation_and)();
		m_type = e_type::And;
	}
	else if(dynamic_cast<dialog_operation_or*>(expr))
	{
		m_expression = NEW(dialog_operation_or)();
		m_type = e_type::Or;
	}
	else if(dynamic_cast<dialog_operation_not*>(expr))
	{
		m_expression = NEW(dialog_operation_not)();
		m_type = e_type::Not;
	}
	else if(dynamic_cast<dialog_precondition*>(expr))
	{
		dialog_precondition* n = NEW(dialog_precondition)();
		m_type = e_type::Precondition;
		n->m_function = dynamic_cast<dialog_precondition*>(expr)->m_function;
		if(dynamic_cast<dialog_precondition*>(expr)->m_param)
			n->m_param = strings::duplicate(g_allocator, dynamic_cast<dialog_precondition*>(expr)->m_param);

		m_expression = n;
	}
}

dialog_link_precondition::~dialog_link_precondition()
{
	DELETE(m_expression);
}

dialog_link_precondition::e_func_names dialog_link_precondition::func_names::get()
{
	if(m_type==e_type::Precondition)
		return e_func_names(dynamic_cast<dialog_precondition*>(m_expression)->m_function);
	else
		return e_func_names::no_func;
}

void dialog_link_precondition::func_names::set(e_func_names nv)
{
	if(m_type!=e_type::Precondition)
		return;

	dynamic_cast<dialog_precondition*>(m_expression)->m_function = e_precondition_functions(nv);
}

String^ dialog_link_precondition::param1::get()
{
	if(m_type==e_type::Precondition)
		return gcnew String(dynamic_cast<dialog_precondition*>(m_expression)->m_param);
	else
		return "";
}

void dialog_link_precondition::param1::set(String^ nv)
{
	if(m_type!=e_type::Precondition)
		return;

	dialog_precondition* prec = dynamic_cast<dialog_precondition*>(m_expression);
	if(prec->m_param)
		FREE(prec->m_param);

	prec->m_param = strings::duplicate(g_allocator, unmanaged_string(nv).c_str());
}

void dialog_link_precondition::type::set(e_type nv)
{
	if(m_type==nv)
		return;

	DELETE(m_expression);

	m_type = nv;
	switch(m_type)
	{
		case e_type::And: m_expression = NEW(dialog_operation_and)(); break;
		case e_type::Or: m_expression = NEW(dialog_operation_or)(); break;
		case e_type::Not: m_expression = NEW(dialog_operation_not)(); break;
		case e_type::Precondition: m_expression = NEW(dialog_precondition)(); break;
	}
}

dialog_expression* dialog_link_precondition::expression_copy()
{
	dialog_expression* new_expr = NULL;
	switch(m_type)
	{
	case e_type::And: new_expr = NEW(dialog_operation_and)(); break;
	case e_type::Or: new_expr = NEW(dialog_operation_or)(); break;
	case e_type::Not: new_expr = NEW(dialog_operation_not)(); break;
	case e_type::Precondition: 
		{
			new_expr = NEW(dialog_precondition)(); 
			dialog_precondition* n_e = dynamic_cast<dialog_precondition*>(new_expr);
			n_e->m_function = dynamic_cast<dialog_precondition*>(m_expression)->m_function;
			if(dynamic_cast<dialog_precondition*>(m_expression)->m_param)
				n_e->m_param = strings::duplicate(g_allocator, dynamic_cast<dialog_precondition*>(m_expression)->m_param);

			break;
		}
	}
	return new_expr;
}
