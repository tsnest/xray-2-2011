#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace xray{
namespace editor {

	ref class tool_terrain;
	ref class editor_control_base;

	/// <summary>
	/// Summary for terrain_smooth_control_tab
	/// </summary>
	public ref class terrain_smooth_control_tab : public System::Windows::Forms::UserControl
	{
	public:
		terrain_smooth_control_tab(tool_terrain^ t):m_tool(t),m_in_sync(false)
		{
			InitializeComponent();
			in_constructor();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~terrain_smooth_control_tab()
		{
			if (components)
			{
				delete components;
			}
		}
	void	on_control_activated(editor_control_base^ c);
	void	on_control_deactivated(editor_control_base^ c);

	void	in_constructor		();
	tool_terrain^				m_tool;
	private: ::gTrackBar::gTrackBar^  radius_track_bar;
	private: ::gTrackBar::gTrackBar^  hardness_track_bar;
	private: ::gTrackBar::gTrackBar^  distortion_track_bar;
	private: ::gTrackBar::gTrackBar^  strength_track_bar;

	protected: 

	protected: 
		bool						m_in_sync;
	void	sync				(bool b_sync_ui);

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
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(terrain_smooth_control_tab::typeid));
			this->strength_track_bar = (gcnew ::gTrackBar::gTrackBar());
			this->distortion_track_bar = (gcnew ::gTrackBar::gTrackBar());
			this->hardness_track_bar = (gcnew ::gTrackBar::gTrackBar());
			this->radius_track_bar = (gcnew ::gTrackBar::gTrackBar());
			this->SuspendLayout();
			// 
			// strength_track_bar
			// 
			this->strength_track_bar->AButColorA = System::Drawing::Color::CornflowerBlue;
			this->strength_track_bar->AButColorB = System::Drawing::Color::Lavender;
			this->strength_track_bar->AButColorBorder = System::Drawing::Color::SteelBlue;
			this->strength_track_bar->ArrowColorDown = System::Drawing::Color::GhostWhite;
			this->strength_track_bar->ArrowColorHover = System::Drawing::Color::DarkBlue;
			this->strength_track_bar->ArrowColorUp = System::Drawing::Color::LightSteelBlue;
			this->strength_track_bar->BackColor = System::Drawing::SystemColors::Control;
			this->strength_track_bar->BorderColor = System::Drawing::Color::Black;
			this->strength_track_bar->BorderShow = false;
			this->strength_track_bar->BrushDirection = System::Drawing::Drawing2D::LinearGradientMode::Horizontal;
			this->strength_track_bar->BrushStyle = gTrackBar::gTrackBar::eBrushStyle::Path;
			this->strength_track_bar->ChangeLarge = 0.1F;
			this->strength_track_bar->ChangeSmall = 0.05F;
			this->strength_track_bar->ColorDown = System::Drawing::Color::CornflowerBlue;
			this->strength_track_bar->ColorDownBorder = System::Drawing::Color::DarkSlateBlue;
			this->strength_track_bar->ColorDownHiLt = System::Drawing::Color::AliceBlue;
			this->strength_track_bar->ColorHover = System::Drawing::Color::RoyalBlue;
			this->strength_track_bar->ColorHoverBorder = System::Drawing::Color::Blue;
			this->strength_track_bar->ColorHoverHiLt = System::Drawing::Color::White;
			this->strength_track_bar->ColorUp = System::Drawing::Color::MediumBlue;
			this->strength_track_bar->ColorUpBorder = System::Drawing::Color::DarkBlue;
			this->strength_track_bar->ColorUpHiLt = System::Drawing::Color::AliceBlue;
			this->strength_track_bar->Dock = System::Windows::Forms::DockStyle::Top;
			this->strength_track_bar->FloatValue = true;
			this->strength_track_bar->FloatValueFont = (gcnew System::Drawing::Font(L"Arial", 8, System::Drawing::FontStyle::Bold));
			this->strength_track_bar->FloatValueFontColor = System::Drawing::Color::MediumBlue;
			this->strength_track_bar->fltValue = 0;
			this->strength_track_bar->intValue = 0;
			this->strength_track_bar->is_integer = false;
			this->strength_track_bar->Label = L"Strength";
			this->strength_track_bar->LabelAlighnment = System::Drawing::StringAlignment::Near;
			this->strength_track_bar->LabelColor = System::Drawing::Color::Black;
			this->strength_track_bar->LabelFont = (gcnew System::Drawing::Font(L"Arial", 9));
			this->strength_track_bar->LabelPadding = System::Windows::Forms::Padding(3);
			this->strength_track_bar->LabelShow = true;
			this->strength_track_bar->Location = System::Drawing::Point(0, 135);
			this->strength_track_bar->Margin = System::Windows::Forms::Padding(2, 3, 2, 3);
			this->strength_track_bar->MaxValue = 1;
			this->strength_track_bar->MinValue = 0.3F;
			this->strength_track_bar->Name = L"strength_track_bar";
			this->strength_track_bar->Orientation = System::Windows::Forms::Orientation::Horizontal;
			this->strength_track_bar->ShowFocus = false;
			this->strength_track_bar->Size = System::Drawing::Size(239, 47);
			this->strength_track_bar->SliderCapEnd = System::Drawing::Drawing2D::LineCap::Round;
			this->strength_track_bar->SliderCapStart = System::Drawing::Drawing2D::LineCap::Round;
			this->strength_track_bar->SliderColorHigh = System::Drawing::Color::DarkGray;
			this->strength_track_bar->SliderColorLow = System::Drawing::Color::Red;
			this->strength_track_bar->SliderFocalPt = (cli::safe_cast<System::Drawing::PointF >(resources->GetObject(L"strength_track_bar.SliderFocalPt")));
			this->strength_track_bar->SliderHighlightPt = (cli::safe_cast<System::Drawing::PointF >(resources->GetObject(L"strength_track_bar.SliderHighlightPt")));
			this->strength_track_bar->SliderShape = gTrackBar::gTrackBar::eShape::Rectangle;
			this->strength_track_bar->SliderSize = System::Drawing::Size(20, 10);
			this->strength_track_bar->SliderWidth = 1;
			this->strength_track_bar->TabIndex = 10;
			this->strength_track_bar->TickColor = System::Drawing::Color::DarkGray;
			this->strength_track_bar->TickInterval = 0.1F;
			this->strength_track_bar->TickType = gTrackBar::gTrackBar::eTickType::Middle;
			this->strength_track_bar->TickWidth = 10;
			this->strength_track_bar->UpDownAutoWidth = true;
			this->strength_track_bar->UpDownWidth = 30;
			this->strength_track_bar->ValueBox = gTrackBar::gTrackBar::eValueBox::Left;
			this->strength_track_bar->ValueBoxBackColor = System::Drawing::Color::White;
			this->strength_track_bar->ValueBoxBorder = System::Drawing::SystemColors::ActiveBorder;
			this->strength_track_bar->ValueBoxFont = (gcnew System::Drawing::Font(L"Arial", 8.25F));
			this->strength_track_bar->ValueBoxFontColor = System::Drawing::SystemColors::ControlText;
			this->strength_track_bar->ValueBoxShape = gTrackBar::gTrackBar::eShape::Rectangle;
			this->strength_track_bar->ValueBoxSize = System::Drawing::Size(40, 20);
			this->strength_track_bar->ValueChanged += gcnew gTrackBar::gTrackBar::ValueChangedEventHandler(this, &terrain_smooth_control_tab::sync);
			// 
			// distortion_track_bar
			// 
			this->distortion_track_bar->AButColorA = System::Drawing::Color::CornflowerBlue;
			this->distortion_track_bar->AButColorB = System::Drawing::Color::Lavender;
			this->distortion_track_bar->AButColorBorder = System::Drawing::Color::SteelBlue;
			this->distortion_track_bar->ArrowColorDown = System::Drawing::Color::GhostWhite;
			this->distortion_track_bar->ArrowColorHover = System::Drawing::Color::DarkBlue;
			this->distortion_track_bar->ArrowColorUp = System::Drawing::Color::LightSteelBlue;
			this->distortion_track_bar->BackColor = System::Drawing::SystemColors::Control;
			this->distortion_track_bar->BorderColor = System::Drawing::Color::Black;
			this->distortion_track_bar->BorderShow = false;
			this->distortion_track_bar->BrushDirection = System::Drawing::Drawing2D::LinearGradientMode::Horizontal;
			this->distortion_track_bar->BrushStyle = gTrackBar::gTrackBar::eBrushStyle::Path;
			this->distortion_track_bar->ChangeLarge = 1;
			this->distortion_track_bar->ChangeSmall = 1;
			this->distortion_track_bar->ColorDown = System::Drawing::Color::CornflowerBlue;
			this->distortion_track_bar->ColorDownBorder = System::Drawing::Color::DarkSlateBlue;
			this->distortion_track_bar->ColorDownHiLt = System::Drawing::Color::AliceBlue;
			this->distortion_track_bar->ColorHover = System::Drawing::Color::RoyalBlue;
			this->distortion_track_bar->ColorHoverBorder = System::Drawing::Color::Blue;
			this->distortion_track_bar->ColorHoverHiLt = System::Drawing::Color::White;
			this->distortion_track_bar->ColorUp = System::Drawing::Color::MediumBlue;
			this->distortion_track_bar->ColorUpBorder = System::Drawing::Color::DarkBlue;
			this->distortion_track_bar->ColorUpHiLt = System::Drawing::Color::AliceBlue;
			this->distortion_track_bar->Dock = System::Windows::Forms::DockStyle::Top;
			this->distortion_track_bar->FloatValue = true;
			this->distortion_track_bar->FloatValueFont = (gcnew System::Drawing::Font(L"Arial", 8, System::Drawing::FontStyle::Bold));
			this->distortion_track_bar->FloatValueFontColor = System::Drawing::Color::MediumBlue;
			this->distortion_track_bar->fltValue = 0;
			this->distortion_track_bar->intValue = 0;
			this->distortion_track_bar->is_integer = false;
			this->distortion_track_bar->Label = L"Distortion";
			this->distortion_track_bar->LabelAlighnment = System::Drawing::StringAlignment::Near;
			this->distortion_track_bar->LabelColor = System::Drawing::Color::Black;
			this->distortion_track_bar->LabelFont = (gcnew System::Drawing::Font(L"Arial", 9));
			this->distortion_track_bar->LabelPadding = System::Windows::Forms::Padding(3);
			this->distortion_track_bar->LabelShow = true;
			this->distortion_track_bar->Location = System::Drawing::Point(0, 90);
			this->distortion_track_bar->Margin = System::Windows::Forms::Padding(2, 3, 2, 3);
			this->distortion_track_bar->MaxValue = 1;
			this->distortion_track_bar->MinValue = 0;
			this->distortion_track_bar->Name = L"distortion_track_bar";
			this->distortion_track_bar->Orientation = System::Windows::Forms::Orientation::Horizontal;
			this->distortion_track_bar->ShowFocus = false;
			this->distortion_track_bar->Size = System::Drawing::Size(239, 45);
			this->distortion_track_bar->SliderCapEnd = System::Drawing::Drawing2D::LineCap::Round;
			this->distortion_track_bar->SliderCapStart = System::Drawing::Drawing2D::LineCap::Round;
			this->distortion_track_bar->SliderColorHigh = System::Drawing::Color::DarkGray;
			this->distortion_track_bar->SliderColorLow = System::Drawing::Color::Red;
			this->distortion_track_bar->SliderFocalPt = (cli::safe_cast<System::Drawing::PointF >(resources->GetObject(L"distortion_track_bar.SliderFocalPt")));
			this->distortion_track_bar->SliderHighlightPt = (cli::safe_cast<System::Drawing::PointF >(resources->GetObject(L"distortion_track_bar.SliderHighlightPt")));
			this->distortion_track_bar->SliderShape = gTrackBar::gTrackBar::eShape::Rectangle;
			this->distortion_track_bar->SliderSize = System::Drawing::Size(20, 10);
			this->distortion_track_bar->SliderWidth = 1;
			this->distortion_track_bar->TabIndex = 9;
			this->distortion_track_bar->TickColor = System::Drawing::Color::DarkGray;
			this->distortion_track_bar->TickInterval = 0.1F;
			this->distortion_track_bar->TickType = gTrackBar::gTrackBar::eTickType::Middle;
			this->distortion_track_bar->TickWidth = 10;
			this->distortion_track_bar->UpDownAutoWidth = true;
			this->distortion_track_bar->UpDownWidth = 30;
			this->distortion_track_bar->ValueBox = gTrackBar::gTrackBar::eValueBox::Left;
			this->distortion_track_bar->ValueBoxBackColor = System::Drawing::Color::White;
			this->distortion_track_bar->ValueBoxBorder = System::Drawing::SystemColors::ActiveBorder;
			this->distortion_track_bar->ValueBoxFont = (gcnew System::Drawing::Font(L"Arial", 8.25F));
			this->distortion_track_bar->ValueBoxFontColor = System::Drawing::SystemColors::ControlText;
			this->distortion_track_bar->ValueBoxShape = gTrackBar::gTrackBar::eShape::Rectangle;
			this->distortion_track_bar->ValueBoxSize = System::Drawing::Size(40, 20);
			this->distortion_track_bar->ValueChanged += gcnew gTrackBar::gTrackBar::ValueChangedEventHandler(this, &terrain_smooth_control_tab::sync);
			// 
			// hardness_track_bar
			// 
			this->hardness_track_bar->AButColorA = System::Drawing::Color::CornflowerBlue;
			this->hardness_track_bar->AButColorB = System::Drawing::Color::Lavender;
			this->hardness_track_bar->AButColorBorder = System::Drawing::Color::SteelBlue;
			this->hardness_track_bar->ArrowColorDown = System::Drawing::Color::GhostWhite;
			this->hardness_track_bar->ArrowColorHover = System::Drawing::Color::DarkBlue;
			this->hardness_track_bar->ArrowColorUp = System::Drawing::Color::LightSteelBlue;
			this->hardness_track_bar->BackColor = System::Drawing::SystemColors::Control;
			this->hardness_track_bar->BorderColor = System::Drawing::Color::Black;
			this->hardness_track_bar->BorderShow = false;
			this->hardness_track_bar->BrushDirection = System::Drawing::Drawing2D::LinearGradientMode::Horizontal;
			this->hardness_track_bar->BrushStyle = gTrackBar::gTrackBar::eBrushStyle::Path;
			this->hardness_track_bar->ChangeLarge = 0.2F;
			this->hardness_track_bar->ChangeSmall = 0.1F;
			this->hardness_track_bar->ColorDown = System::Drawing::Color::CornflowerBlue;
			this->hardness_track_bar->ColorDownBorder = System::Drawing::Color::DarkSlateBlue;
			this->hardness_track_bar->ColorDownHiLt = System::Drawing::Color::AliceBlue;
			this->hardness_track_bar->ColorHover = System::Drawing::Color::RoyalBlue;
			this->hardness_track_bar->ColorHoverBorder = System::Drawing::Color::Blue;
			this->hardness_track_bar->ColorHoverHiLt = System::Drawing::Color::White;
			this->hardness_track_bar->ColorUp = System::Drawing::Color::MediumBlue;
			this->hardness_track_bar->ColorUpBorder = System::Drawing::Color::DarkBlue;
			this->hardness_track_bar->ColorUpHiLt = System::Drawing::Color::AliceBlue;
			this->hardness_track_bar->Dock = System::Windows::Forms::DockStyle::Top;
			this->hardness_track_bar->FloatValue = true;
			this->hardness_track_bar->FloatValueFont = (gcnew System::Drawing::Font(L"Arial", 8, System::Drawing::FontStyle::Bold));
			this->hardness_track_bar->FloatValueFontColor = System::Drawing::Color::MediumBlue;
			this->hardness_track_bar->fltValue = 0;
			this->hardness_track_bar->intValue = 0;
			this->hardness_track_bar->is_integer = false;
			this->hardness_track_bar->Label = L"Hardness";
			this->hardness_track_bar->LabelAlighnment = System::Drawing::StringAlignment::Near;
			this->hardness_track_bar->LabelColor = System::Drawing::Color::Black;
			this->hardness_track_bar->LabelFont = (gcnew System::Drawing::Font(L"Arial", 9));
			this->hardness_track_bar->LabelPadding = System::Windows::Forms::Padding(3);
			this->hardness_track_bar->LabelShow = true;
			this->hardness_track_bar->Location = System::Drawing::Point(0, 45);
			this->hardness_track_bar->Margin = System::Windows::Forms::Padding(2, 3, 2, 3);
			this->hardness_track_bar->MaxValue = 1;
			this->hardness_track_bar->MinValue = 0;
			this->hardness_track_bar->Name = L"hardness_track_bar";
			this->hardness_track_bar->Orientation = System::Windows::Forms::Orientation::Horizontal;
			this->hardness_track_bar->ShowFocus = false;
			this->hardness_track_bar->Size = System::Drawing::Size(239, 45);
			this->hardness_track_bar->SliderCapEnd = System::Drawing::Drawing2D::LineCap::Round;
			this->hardness_track_bar->SliderCapStart = System::Drawing::Drawing2D::LineCap::Round;
			this->hardness_track_bar->SliderColorHigh = System::Drawing::Color::DarkGray;
			this->hardness_track_bar->SliderColorLow = System::Drawing::Color::Red;
			this->hardness_track_bar->SliderFocalPt = (cli::safe_cast<System::Drawing::PointF >(resources->GetObject(L"hardness_track_bar.SliderFocalPt")));
			this->hardness_track_bar->SliderHighlightPt = (cli::safe_cast<System::Drawing::PointF >(resources->GetObject(L"hardness_track_bar.SliderHighlightPt")));
			this->hardness_track_bar->SliderShape = gTrackBar::gTrackBar::eShape::Rectangle;
			this->hardness_track_bar->SliderSize = System::Drawing::Size(20, 10);
			this->hardness_track_bar->SliderWidth = 1;
			this->hardness_track_bar->TabIndex = 8;
			this->hardness_track_bar->TickColor = System::Drawing::Color::DarkGray;
			this->hardness_track_bar->TickInterval = 0.1F;
			this->hardness_track_bar->TickType = gTrackBar::gTrackBar::eTickType::Middle;
			this->hardness_track_bar->TickWidth = 10;
			this->hardness_track_bar->UpDownAutoWidth = true;
			this->hardness_track_bar->UpDownWidth = 30;
			this->hardness_track_bar->ValueBox = gTrackBar::gTrackBar::eValueBox::Left;
			this->hardness_track_bar->ValueBoxBackColor = System::Drawing::Color::White;
			this->hardness_track_bar->ValueBoxBorder = System::Drawing::SystemColors::ActiveBorder;
			this->hardness_track_bar->ValueBoxFont = (gcnew System::Drawing::Font(L"Arial", 8.25F));
			this->hardness_track_bar->ValueBoxFontColor = System::Drawing::SystemColors::ControlText;
			this->hardness_track_bar->ValueBoxShape = gTrackBar::gTrackBar::eShape::Rectangle;
			this->hardness_track_bar->ValueBoxSize = System::Drawing::Size(40, 20);
			this->hardness_track_bar->ValueChanged += gcnew gTrackBar::gTrackBar::ValueChangedEventHandler(this, &terrain_smooth_control_tab::sync);
			// 
			// radius_track_bar
			// 
			this->radius_track_bar->AButColorA = System::Drawing::Color::CornflowerBlue;
			this->radius_track_bar->AButColorB = System::Drawing::Color::Lavender;
			this->radius_track_bar->AButColorBorder = System::Drawing::Color::SteelBlue;
			this->radius_track_bar->ArrowColorDown = System::Drawing::Color::GhostWhite;
			this->radius_track_bar->ArrowColorHover = System::Drawing::Color::DarkBlue;
			this->radius_track_bar->ArrowColorUp = System::Drawing::Color::LightSteelBlue;
			this->radius_track_bar->BackColor = System::Drawing::SystemColors::Control;
			this->radius_track_bar->BorderColor = System::Drawing::Color::Black;
			this->radius_track_bar->BorderShow = false;
			this->radius_track_bar->BrushDirection = System::Drawing::Drawing2D::LinearGradientMode::Horizontal;
			this->radius_track_bar->BrushStyle = gTrackBar::gTrackBar::eBrushStyle::Path;
			this->radius_track_bar->ChangeLarge = 10;
			this->radius_track_bar->ChangeSmall = 1;
			this->radius_track_bar->ColorDown = System::Drawing::Color::CornflowerBlue;
			this->radius_track_bar->ColorDownBorder = System::Drawing::Color::DarkSlateBlue;
			this->radius_track_bar->ColorDownHiLt = System::Drawing::Color::AliceBlue;
			this->radius_track_bar->ColorHover = System::Drawing::Color::RoyalBlue;
			this->radius_track_bar->ColorHoverBorder = System::Drawing::Color::Blue;
			this->radius_track_bar->ColorHoverHiLt = System::Drawing::Color::White;
			this->radius_track_bar->ColorUp = System::Drawing::Color::MediumBlue;
			this->radius_track_bar->ColorUpBorder = System::Drawing::Color::DarkBlue;
			this->radius_track_bar->ColorUpHiLt = System::Drawing::Color::AliceBlue;
			this->radius_track_bar->Dock = System::Windows::Forms::DockStyle::Top;
			this->radius_track_bar->FloatValue = true;
			this->radius_track_bar->FloatValueFont = (gcnew System::Drawing::Font(L"Arial", 8, System::Drawing::FontStyle::Bold));
			this->radius_track_bar->FloatValueFontColor = System::Drawing::Color::MediumBlue;
			this->radius_track_bar->fltValue = 0;
			this->radius_track_bar->intValue = 0;
			this->radius_track_bar->is_integer = false;
			this->radius_track_bar->Label = L"Radius";
			this->radius_track_bar->LabelAlighnment = System::Drawing::StringAlignment::Near;
			this->radius_track_bar->LabelColor = System::Drawing::Color::Black;
			this->radius_track_bar->LabelFont = (gcnew System::Drawing::Font(L"Arial", 9));
			this->radius_track_bar->LabelPadding = System::Windows::Forms::Padding(3);
			this->radius_track_bar->LabelShow = true;
			this->radius_track_bar->Location = System::Drawing::Point(0, 0);
			this->radius_track_bar->Margin = System::Windows::Forms::Padding(2, 3, 2, 3);
			this->radius_track_bar->MaxValue = 50;
			this->radius_track_bar->MinValue = 0;
			this->radius_track_bar->Name = L"radius_track_bar";
			this->radius_track_bar->Orientation = System::Windows::Forms::Orientation::Horizontal;
			this->radius_track_bar->ShowFocus = false;
			this->radius_track_bar->Size = System::Drawing::Size(239, 45);
			this->radius_track_bar->SliderCapEnd = System::Drawing::Drawing2D::LineCap::Round;
			this->radius_track_bar->SliderCapStart = System::Drawing::Drawing2D::LineCap::Round;
			this->radius_track_bar->SliderColorHigh = System::Drawing::Color::DarkGray;
			this->radius_track_bar->SliderColorLow = System::Drawing::Color::Red;
			this->radius_track_bar->SliderFocalPt = (cli::safe_cast<System::Drawing::PointF >(resources->GetObject(L"radius_track_bar.SliderFocalPt")));
			this->radius_track_bar->SliderHighlightPt = (cli::safe_cast<System::Drawing::PointF >(resources->GetObject(L"radius_track_bar.SliderHighlightPt")));
			this->radius_track_bar->SliderShape = gTrackBar::gTrackBar::eShape::Rectangle;
			this->radius_track_bar->SliderSize = System::Drawing::Size(20, 10);
			this->radius_track_bar->SliderWidth = 1;
			this->radius_track_bar->TabIndex = 7;
			this->radius_track_bar->TickColor = System::Drawing::Color::DarkGray;
			this->radius_track_bar->TickInterval = 10;
			this->radius_track_bar->TickType = gTrackBar::gTrackBar::eTickType::Middle;
			this->radius_track_bar->TickWidth = 10;
			this->radius_track_bar->UpDownAutoWidth = true;
			this->radius_track_bar->UpDownWidth = 30;
			this->radius_track_bar->ValueBox = gTrackBar::gTrackBar::eValueBox::Left;
			this->radius_track_bar->ValueBoxBackColor = System::Drawing::Color::White;
			this->radius_track_bar->ValueBoxBorder = System::Drawing::SystemColors::ActiveBorder;
			this->radius_track_bar->ValueBoxFont = (gcnew System::Drawing::Font(L"Arial", 8.25F));
			this->radius_track_bar->ValueBoxFontColor = System::Drawing::SystemColors::ControlText;
			this->radius_track_bar->ValueBoxShape = gTrackBar::gTrackBar::eShape::Rectangle;
			this->radius_track_bar->ValueBoxSize = System::Drawing::Size(40, 20);
			this->radius_track_bar->ValueChanged += gcnew gTrackBar::gTrackBar::ValueChangedEventHandler(this, &terrain_smooth_control_tab::sync);
			// 
			// terrain_smooth_control_tab
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->Controls->Add(this->strength_track_bar);
			this->Controls->Add(this->distortion_track_bar);
			this->Controls->Add(this->hardness_track_bar);
			this->Controls->Add(this->radius_track_bar);
			this->Name = L"terrain_smooth_control_tab";
			this->Size = System::Drawing::Size(239, 190);
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void sync(System::Object^  sender, System::EventArgs^  e);
};
} // namespace editor
} // namespace xray
