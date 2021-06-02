using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace xray.editor.wpf_controls
{
    public class special_attribute: Attribute
    {
        public special_attribute(String value)
        {
            this.value = value;
        }

        public String value;
    }
}
