////////////////////////////////////////////////////////////////////////////
//	Created		: 17.12.2010
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;

namespace xray.editor.wpf_controls.property_editors.attributes
{
	[AttributeUsage( AttributeTargets.Property )]
	public class vector3_attribute: Attribute
	{
		public vector3_attribute( )	
		{
			x_name			= "x";
			y_name			= "y";
			z_name			= "z";

			x_min			= Double.NaN;
			y_min			= Double.NaN;
			z_min			= Double.NaN;

			x_max			= Double.NaN;
			y_max			= Double.NaN;
			z_max			= Double.NaN;

			x_step_size		= 0.1;
			y_step_size		= 0.1;
			z_step_size		= 0.1;

			x_precision		= 3;
			y_precision		= 3;
			z_precision		= 3;
		}

		public String	x_name		{get;set;}
		public String	y_name		{get;set;}
		public String	z_name		{get;set;}

		public Double	x_min		{get;set;}
		public Double	y_min		{get;set;}
		public Double	z_min		{get;set;}

		public Double	x_max		{get;set;}
		public Double	y_max		{get;set;}
		public Double	z_max		{get;set;}

		public Double	x_step_size {get;set;}
		public Double	y_step_size {get;set;}
		public Double	z_step_size {get;set;}

		public Int32	x_precision	{get;set;}
		public Int32	y_precision	{get;set;}
		public Int32	z_precision	{get;set;}
	}
}
