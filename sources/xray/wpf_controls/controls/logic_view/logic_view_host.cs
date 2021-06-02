////////////////////////////////////////////////////////////////////////////
//	Created		: 09.12.2010
//	Author		: Alexander Plichko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

using System;
using System.ComponentModel.Design;
using System.Diagnostics;
using System.Windows.Forms.Integration;

namespace xray
{
    namespace editor
    {
        namespace wpf_controls
        {
            namespace logic_view
            {
             public class logic_view_host : ElementHost
                {
                 public logic_view_host( )
                    {
                        in_constructor( );
                    }


                    private logic_view m_logic_view;
                    public new String Child;


                    private void in_constructor()
                    {
                        if (!is_design_mode())
                        {
                            m_logic_view = new logic_view();
                            base.Child = m_logic_view;
                        }
                    }
                    private bool is_design_mode()
                    {
                        if (Site != null)
                            return Site.DesignMode;

                        var stack_trace = new StackTrace();
                        var frame_count = stack_trace.FrameCount - 1;

                        for (int frame = 0; frame < frame_count; frame++)
                        {
                            Type type = stack_trace.GetFrame(frame).GetMethod().DeclaringType;
                            if (typeof(IDesignerHost).IsAssignableFrom(type))
                                return true;
                        }
                        return false;
                    }

                    public logic_view logic_view
                    {
                        get { return m_logic_view; }
                    }

                }
            }
        }
    }
}
