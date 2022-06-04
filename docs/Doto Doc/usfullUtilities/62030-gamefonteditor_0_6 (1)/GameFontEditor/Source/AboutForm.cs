using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace GameFontEditor
{
    public partial class AboutForm : Form
    {
        public AboutForm()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            Close();
        }

        private void InitLink(LinkLabel link)
        {
            if (!link.LinkArea.IsEmpty)
            {
                string text = link.Text.Substring(link.LinkArea.Start, link.LinkArea.Length);
                link.Links.Add(new LinkLabel.Link(link.LinkArea.Start, link.LinkArea.Length, text));
            }
        }

        private void AboutForm_Load(object sender, EventArgs e)
        {
            InitLink(linkAsafAccount);
            InitLink(linkLicense);
            InitLink(linkFreeImage);
            InitLink(linkIcons);
            InitLink(linkThread);

            lbVersion.Text = "Version: " + Application.ProductVersion + " (Beta)";
        }

        private void LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            System.Diagnostics.Process.Start(e.Link.LinkData.ToString()); 
        }

        private void link_Enter(object sender, EventArgs e)
        {
            btClose.Focus();
        }
    }
}