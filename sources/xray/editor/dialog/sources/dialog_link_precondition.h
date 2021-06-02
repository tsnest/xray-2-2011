////////////////////////////////////////////////////////////////////////////
//	Created		: 05.02.2010
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef DIALOG_LINK_PRECONDITION_H_INCLUDED
#define DIALOG_LINK_PRECONDITION_H_INCLUDED

using namespace System;
using namespace System::Collections;
using namespace System::ComponentModel;

namespace xray {
namespace dialog_editor {
	public ref class dialog_link_precondition_attribute : public Attribute
	{
	};

	class dialog_expression;
	enum e_precondition_functions;

	public ref class dialog_link_precondition : public Windows::Forms::TreeNode
	{
		typedef Windows::Forms::TreeNode super;
		typedef Generic::List<dialog_link_precondition^ >	preconditions;
	public:
		enum class e_func_names : int
		{
			no_func		= 0,
			has_info	= 1,
			has_item	= 2,
		};

		enum class e_type : int
		{
			Precondition	= 0,
			And				= 1,
			Or				= 2,
			Not				= 3
		};

		dialog_link_precondition();
		dialog_link_precondition(dialog_expression* expr);
		~dialog_link_precondition();

		dialog_expression*	expression_copy();

		[CategoryAttribute("Precondition properties"), DisplayNameAttribute("Name"), 
			DescriptionAttribute("Name of action to be called"), ReadOnlyAttribute(true)]
		property String^ name
		{
			String^		get					()	{return Name;}
		};

		[CategoryAttribute("Precondition properties"), DisplayNameAttribute("Function"), 
			DescriptionAttribute("Name of precondition to be called")]
		property e_func_names func_names
		{
			e_func_names				get	();
			void						set	(e_func_names nv);
		};

		[CategoryAttribute("Precondition properties"), DisplayNameAttribute("Parameter"), 
			DescriptionAttribute("Parameter of precondition to be called")]
		property String^ param1
		{
			String^						get	();
			void						set	(String^ nv);
		};

		[CategoryAttribute("Precondition properties"), DisplayNameAttribute("Node type"), 
			DescriptionAttribute("Type of node"), dialog_link_precondition_attribute()]
		property e_type	type
		{
			e_type						get	()				{return m_type;};
			void						set	(e_type nv);
		};

	private:
		dialog_expression*	m_expression;
		e_type				m_type;
	}; // ref class dialog_link_precondition
} // namespace dialog_editor
} // namespace xray

#endif // #ifndef DIALOG_LINK_PRECONDITION_H_INCLUDED