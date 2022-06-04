using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace GameFontEditor
{
    public partial class DisplayException : Form
    {
        public DisplayException()
        {
            InitializeComponent();
        }

        private Exception m_exception;

        public Exception Exception
        {
            get { return m_exception; }
            set 
            {
                m_exception = value; 
                Exception cur = m_exception;

                StringBuilder sb = new StringBuilder();

                while (cur != null)
                {
                    sb.AppendLine(m_exception.Message);
                    sb.AppendLine(m_exception.StackTrace);
                    sb.AppendLine("========================================");
                    cur = cur.InnerException;
                }

                textBox1.Text = sb.ToString();
            }
        }


    }
}