////////////////////////////////////////////////////////////////////////////
//	Created		: 23.11.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////
#ifndef SCENE_GRAPH_NODE_H_INCLUDED
#define SCENE_GRAPH_NODE_H_INCLUDED

using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace xray {
namespace editor {

	ref class object_base;
	/// <summary>
	/// Summary for scene_graph_node
	/// </summary>
	public ref class scene_graph_node : public controls::hypergraph::node
	{
		typedef controls::hypergraph::node	super;
	public:
		scene_graph_node(void):m_object(nullptr)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}
		property object_base^ object{
			object_base^	get()					{return m_object;}
			void			set(object_base^ o)		{set_object_impl(o);}
		}
	public:
		virtual System::String^	caption					() override;
		virtual void	on_destroyed			() override;

	protected:
		object_base^		m_object;
		void				set_object_impl	(object_base^ o);
		void				init_internal	();
	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~scene_graph_node()
		{
			if (components)
			{
				delete components;
			}
		}

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
		}
#pragma endregion
	};
}; //namespace editor
}; //namespace xray

#endif // #ifndef SCENE_GRAPH_NODE_H_INCLUDED
