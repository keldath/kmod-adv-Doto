namespace GameFontEditor
{
    partial class AboutForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.label1 = new System.Windows.Forms.Label();
            this.btClose = new System.Windows.Forms.Button();
            this.label2 = new System.Windows.Forms.Label();
            this.linkAsafAccount = new System.Windows.Forms.LinkLabel();
            this.label3 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.linkLicense = new System.Windows.Forms.LinkLabel();
            this.linkFreeImage = new System.Windows.Forms.LinkLabel();
            this.lbVersion = new System.Windows.Forms.Label();
            this.label8 = new System.Windows.Forms.Label();
            this.linkIcons = new System.Windows.Forms.LinkLabel();
            this.label9 = new System.Windows.Forms.Label();
            this.linkThread = new System.Windows.Forms.LinkLabel();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(177)));
            this.label1.Location = new System.Drawing.Point(13, 22);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(186, 13);
            this.label1.TabIndex = 0;
            this.label1.Text = "Civilization IV Game Font Editor";
            // 
            // btClose
            // 
            this.btClose.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.btClose.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.btClose.Location = new System.Drawing.Point(245, 291);
            this.btClose.Name = "btClose";
            this.btClose.Size = new System.Drawing.Size(57, 24);
            this.btClose.TabIndex = 1;
            this.btClose.Text = "Close";
            this.btClose.UseVisualStyleBackColor = true;
            this.btClose.Click += new System.EventHandler(this.button1_Click);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(25, 47);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(159, 13);
            this.label2.TabIndex = 2;
            this.label2.Text = "Created By Asaf of Civ Fanatics:";
            // 
            // linkAsafAccount
            // 
            this.linkAsafAccount.AutoSize = true;
            this.linkAsafAccount.Location = new System.Drawing.Point(183, 47);
            this.linkAsafAccount.Name = "linkAsafAccount";
            this.linkAsafAccount.Size = new System.Drawing.Size(263, 13);
            this.linkAsafAccount.TabIndex = 3;
            this.linkAsafAccount.TabStop = true;
            this.linkAsafAccount.Text = "http://forums.civfanatics.com/member.php?u=179974";
            this.linkAsafAccount.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.LinkClicked);
            this.linkAsafAccount.Enter += new System.EventHandler(this.link_Enter);
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(25, 150);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(292, 13);
            this.label3.TabIndex = 4;
            this.label3.Text = "This software uses the FreeImage open source image library.";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(25, 190);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(483, 13);
            this.label4.TabIndex = 6;
            this.label4.Text = "FreeImage is used under the GNU GPL, version 2. See license-gpl.txt which comes w" +
                "ith this program.";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(25, 97);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(478, 13);
            this.label5.TabIndex = 7;
            this.label5.Text = "This software is used under GNU GPL, version 2. See license-gpl.txt which comes w" +
                "ith this program.";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(25, 117);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(50, 13);
            this.label6.TabIndex = 9;
            this.label6.Text = "Also see:";
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(25, 170);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(78, 13);
            this.label7.TabIndex = 10;
            this.label7.Text = "For details, see";
            // 
            // linkLicense
            // 
            this.linkLicense.AutoSize = true;
            this.linkLicense.Location = new System.Drawing.Point(81, 117);
            this.linkLicense.Name = "linkLicense";
            this.linkLicense.Size = new System.Drawing.Size(240, 13);
            this.linkLicense.TabIndex = 11;
            this.linkLicense.TabStop = true;
            this.linkLicense.Text = "http://www.opensource.org/licenses/gpl-2.0.php";
            this.linkLicense.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.LinkClicked);
            this.linkLicense.Enter += new System.EventHandler(this.link_Enter);
            // 
            // linkFreeImage
            // 
            this.linkFreeImage.AutoSize = true;
            this.linkFreeImage.Location = new System.Drawing.Point(109, 170);
            this.linkFreeImage.Name = "linkFreeImage";
            this.linkFreeImage.Size = new System.Drawing.Size(161, 13);
            this.linkFreeImage.TabIndex = 12;
            this.linkFreeImage.TabStop = true;
            this.linkFreeImage.Text = "http://freeimage.sourceforge.net";
            this.linkFreeImage.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.LinkClicked);
            this.linkFreeImage.Enter += new System.EventHandler(this.link_Enter);
            // 
            // lbVersion
            // 
            this.lbVersion.AutoSize = true;
            this.lbVersion.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(177)));
            this.lbVersion.Location = new System.Drawing.Point(207, 22);
            this.lbVersion.Name = "lbVersion";
            this.lbVersion.Size = new System.Drawing.Size(53, 13);
            this.lbVersion.TabIndex = 13;
            this.lbVersion.Text = "Version:";
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(25, 228);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(104, 13);
            this.label8.TabIndex = 14;
            this.label8.Text = "Icons are taken from";
            // 
            // linkIcons
            // 
            this.linkIcons.AutoSize = true;
            this.linkIcons.Location = new System.Drawing.Point(135, 228);
            this.linkIcons.Name = "linkIcons";
            this.linkIcons.Size = new System.Drawing.Size(124, 13);
            this.linkIcons.TabIndex = 15;
            this.linkIcons.TabStop = true;
            this.linkIcons.Text = "http://www.iconza.com/";
            this.linkIcons.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.LinkClicked);
            this.linkIcons.Enter += new System.EventHandler(this.link_Enter);
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(25, 69);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(162, 13);
            this.label9.TabIndex = 16;
            this.label9.Text = "Discussion thread in CFC forums:";
            // 
            // linkThread
            // 
            this.linkThread.AutoSize = true;
            this.linkThread.Location = new System.Drawing.Point(193, 69);
            this.linkThread.Name = "linkThread";
            this.linkThread.Size = new System.Drawing.Size(278, 13);
            this.linkThread.TabIndex = 17;
            this.linkThread.TabStop = true;
            this.linkThread.Text = "http://forums.civfanatics.com/showthread.php?t=429541";
            this.linkThread.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.LinkClicked);
            this.linkThread.Enter += new System.EventHandler(this.link_Enter);
            // 
            // AboutForm
            // 
            this.AcceptButton = this.btClose;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.btClose;
            this.ClientSize = new System.Drawing.Size(541, 327);
            this.ControlBox = false;
            this.Controls.Add(this.linkThread);
            this.Controls.Add(this.label9);
            this.Controls.Add(this.linkIcons);
            this.Controls.Add(this.label8);
            this.Controls.Add(this.lbVersion);
            this.Controls.Add(this.linkFreeImage);
            this.Controls.Add(this.linkLicense);
            this.Controls.Add(this.label7);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.linkAsafAccount);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.btClose);
            this.Controls.Add(this.label1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.Fixed3D;
            this.Name = "AboutForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "About Game Font Editor";
            this.Load += new System.EventHandler(this.AboutForm_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button btClose;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.LinkLabel linkAsafAccount;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.LinkLabel linkLicense;
        private System.Windows.Forms.LinkLabel linkFreeImage;
        private System.Windows.Forms.Label lbVersion;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.LinkLabel linkIcons;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.LinkLabel linkThread;
    }
}