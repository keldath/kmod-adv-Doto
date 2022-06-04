namespace GameFontEditor
{
    partial class MainForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;
        private ImageEnhanced imageEnhanced1 = null;

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
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainForm));
            this.lbImageInfo = new System.Windows.Forms.Label();
            this.picCellHover = new System.Windows.Forms.PictureBox();
            this.txtCellHover = new System.Windows.Forms.TextBox();
            this.picCellSelected = new System.Windows.Forms.PictureBox();
            this.txtCellSelected = new System.Windows.Forms.TextBox();
            this.chkMarker = new System.Windows.Forms.CheckBox();
            this.lbMarkerPos = new System.Windows.Forms.Label();
            this.btMarkerPosPlus = new System.Windows.Forms.Button();
            this.btMarkerPosMinus = new System.Windows.Forms.Button();
            this.panelMarker = new System.Windows.Forms.Panel();
            this.btExport = new System.Windows.Forms.Button();
            this.btImport = new System.Windows.Forms.Button();
            this.txtPixel = new System.Windows.Forms.TextBox();
            this.txtLog = new System.Windows.Forms.TextBox();
            this.btClearCell = new System.Windows.Forms.Button();
            this.panelImage = new ScrollWorkaroundPanel();
            this.toolStrip1 = new System.Windows.Forms.ToolStrip();
            this.toolStripButtonOpen = new System.Windows.Forms.ToolStripButton();
            this.toolStripButtonSave = new System.Windows.Forms.ToolStripButton();
            this.toolStripButtonSaveAs = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.toolStripButtonCut = new System.Windows.Forms.ToolStripButton();
            this.toolStripButtonCopy = new System.Windows.Forms.ToolStripButton();
            this.toolStripButtonPaste = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator3 = new System.Windows.Forms.ToolStripSeparator();
            this.toolStripButtonUndo = new System.Windows.Forms.ToolStripButton();
            this.toolStripButtonRedo = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
            this.toolStripButtonSettings = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator4 = new System.Windows.Forms.ToolStripSeparator();
            this.toolStripButtonAbout = new System.Windows.Forms.ToolStripButton();
            this.label1 = new System.Windows.Forms.Label();
            this.chkShowGrid = new System.Windows.Forms.CheckBox();
            this.toolTip1 = new System.Windows.Forms.ToolTip(this.components);
            this.btStar = new System.Windows.Forms.Button();
            this.label2 = new System.Windows.Forms.Label();
            this.numericUpDownPixels = new System.Windows.Forms.NumericUpDown();
            this.chkAutoBackup = new System.Windows.Forms.CheckBox();
            this.chkDisableActualFont = new System.Windows.Forms.CheckBox();
            this.chkSplit = new System.Windows.Forms.CheckBox();
            this.btCalcAlpha = new System.Windows.Forms.Button();
            this.trackBarAlphaCalc = new System.Windows.Forms.TrackBar();
            this.label3 = new System.Windows.Forms.Label();
            this.panelSettings = new System.Windows.Forms.Panel();
            this.btExternalEditor = new System.Windows.Forms.Button();
            this.lbExternalEditor = new System.Windows.Forms.Label();
            this.btBGColor = new System.Windows.Forms.Button();
            this.panel1 = new System.Windows.Forms.Panel();
            this.label5 = new System.Windows.Forms.Label();
            this.panelMain = new System.Windows.Forms.Panel();
            this.btEdit = new System.Windows.Forms.Button();
            this.lbAlphaSens = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.colorDialog1 = new System.Windows.Forms.ColorDialog();
            this.btUpdate = new System.Windows.Forms.Button();
            this.imageEnhanced1 = new GameFontEditor.ImageEnhanced();
            ((System.ComponentModel.ISupportInitialize)(this.picCellHover)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.picCellSelected)).BeginInit();
            this.panelMarker.SuspendLayout();
            this.panelImage.SuspendLayout();
            this.toolStrip1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownPixels)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.trackBarAlphaCalc)).BeginInit();
            this.panelSettings.SuspendLayout();
            this.panelMain.SuspendLayout();
            this.SuspendLayout();
            // 
            // lbImageInfo
            // 
            this.lbImageInfo.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.lbImageInfo.AutoSize = true;
            this.lbImageInfo.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.lbImageInfo.Location = new System.Drawing.Point(119, 451);
            this.lbImageInfo.Name = "lbImageInfo";
            this.lbImageInfo.Size = new System.Drawing.Size(2, 15);
            this.lbImageInfo.TabIndex = 2;
            // 
            // picCellHover
            // 
            this.picCellHover.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.picCellHover.Location = new System.Drawing.Point(84, 53);
            this.picCellHover.Name = "picCellHover";
            this.picCellHover.Size = new System.Drawing.Size(16, 16);
            this.picCellHover.SizeMode = System.Windows.Forms.PictureBoxSizeMode.CenterImage;
            this.picCellHover.TabIndex = 3;
            this.picCellHover.TabStop = false;
            this.toolTip1.SetToolTip(this.picCellHover, "Highlighted slot\'s image");
            // 
            // txtCellHover
            // 
            this.txtCellHover.Location = new System.Drawing.Point(7, 75);
            this.txtCellHover.Multiline = true;
            this.txtCellHover.Name = "txtCellHover";
            this.txtCellHover.ReadOnly = true;
            this.txtCellHover.Size = new System.Drawing.Size(93, 68);
            this.txtCellHover.TabIndex = 4;
            this.toolTip1.SetToolTip(this.txtCellHover, "Highlighted slot\'s parameters");
            // 
            // picCellSelected
            // 
            this.picCellSelected.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.picCellSelected.Location = new System.Drawing.Point(7, 150);
            this.picCellSelected.Name = "picCellSelected";
            this.picCellSelected.Size = new System.Drawing.Size(32, 32);
            this.picCellSelected.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.picCellSelected.TabIndex = 5;
            this.picCellSelected.TabStop = false;
            this.toolTip1.SetToolTip(this.picCellSelected, "Selected slot\'s image");
            // 
            // txtCellSelected
            // 
            this.txtCellSelected.Location = new System.Drawing.Point(7, 188);
            this.txtCellSelected.Multiline = true;
            this.txtCellSelected.Name = "txtCellSelected";
            this.txtCellSelected.ReadOnly = true;
            this.txtCellSelected.Size = new System.Drawing.Size(93, 68);
            this.txtCellSelected.TabIndex = 6;
            this.toolTip1.SetToolTip(this.txtCellSelected, "Selected slot\'s parameters");
            // 
            // chkMarker
            // 
            this.chkMarker.AutoSize = true;
            this.chkMarker.Location = new System.Drawing.Point(10, 11);
            this.chkMarker.Name = "chkMarker";
            this.chkMarker.Size = new System.Drawing.Size(59, 17);
            this.chkMarker.TabIndex = 7;
            this.chkMarker.Text = "Marker";
            this.chkMarker.UseVisualStyleBackColor = true;
            this.chkMarker.CheckedChanged += new System.EventHandler(this.checkBox1_CheckedChanged);
            // 
            // lbMarkerPos
            // 
            this.lbMarkerPos.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.lbMarkerPos.Location = new System.Drawing.Point(29, 31);
            this.lbMarkerPos.Name = "lbMarkerPos";
            this.lbMarkerPos.Size = new System.Drawing.Size(35, 23);
            this.lbMarkerPos.TabIndex = 8;
            this.lbMarkerPos.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // btMarkerPosPlus
            // 
            this.btMarkerPosPlus.Location = new System.Drawing.Point(70, 33);
            this.btMarkerPosPlus.Name = "btMarkerPosPlus";
            this.btMarkerPosPlus.Size = new System.Drawing.Size(16, 21);
            this.btMarkerPosPlus.TabIndex = 9;
            this.btMarkerPosPlus.Text = "+";
            this.btMarkerPosPlus.UseVisualStyleBackColor = true;
            this.btMarkerPosPlus.Click += new System.EventHandler(this.btMarkerPosPlus_Click);
            // 
            // btMarkerPosMinus
            // 
            this.btMarkerPosMinus.Location = new System.Drawing.Point(7, 32);
            this.btMarkerPosMinus.Name = "btMarkerPosMinus";
            this.btMarkerPosMinus.Size = new System.Drawing.Size(16, 21);
            this.btMarkerPosMinus.TabIndex = 10;
            this.btMarkerPosMinus.Text = "-";
            this.btMarkerPosMinus.UseVisualStyleBackColor = true;
            this.btMarkerPosMinus.Click += new System.EventHandler(this.btMarkerPosMinus_Click);
            // 
            // panelMarker
            // 
            this.panelMarker.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.panelMarker.Controls.Add(this.chkMarker);
            this.panelMarker.Controls.Add(this.lbMarkerPos);
            this.panelMarker.Controls.Add(this.btMarkerPosPlus);
            this.panelMarker.Controls.Add(this.btMarkerPosMinus);
            this.panelMarker.Location = new System.Drawing.Point(781, 172);
            this.panelMarker.Name = "panelMarker";
            this.panelMarker.Size = new System.Drawing.Size(91, 68);
            this.panelMarker.TabIndex = 13;
            // 
            // btExport
            // 
            this.btExport.Location = new System.Drawing.Point(7, 260);
            this.btExport.Name = "btExport";
            this.btExport.Size = new System.Drawing.Size(50, 23);
            this.btExport.TabIndex = 14;
            this.btExport.Text = "Export";
            this.toolTip1.SetToolTip(this.btExport, "Export selected slot\'s image to file");
            this.btExport.UseVisualStyleBackColor = true;
            this.btExport.Click += new System.EventHandler(this.btExport_Click);
            // 
            // btImport
            // 
            this.btImport.Location = new System.Drawing.Point(7, 289);
            this.btImport.Name = "btImport";
            this.btImport.Size = new System.Drawing.Size(50, 23);
            this.btImport.TabIndex = 15;
            this.btImport.Text = "Import";
            this.toolTip1.SetToolTip(this.btImport, "Import file into selected slot\'s image");
            this.btImport.UseVisualStyleBackColor = true;
            this.btImport.Click += new System.EventHandler(this.btImport_Click);
            // 
            // txtPixel
            // 
            this.txtPixel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.txtPixel.Location = new System.Drawing.Point(809, 57);
            this.txtPixel.Multiline = true;
            this.txtPixel.Name = "txtPixel";
            this.txtPixel.ReadOnly = true;
            this.txtPixel.Size = new System.Drawing.Size(59, 100);
            this.txtPixel.TabIndex = 16;
            this.toolTip1.SetToolTip(this.txtPixel, "Hovered Pixel Information");
            // 
            // txtLog
            // 
            this.txtLog.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.txtLog.Location = new System.Drawing.Point(7, 469);
            this.txtLog.Multiline = true;
            this.txtLog.Name = "txtLog";
            this.txtLog.ReadOnly = true;
            this.txtLog.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.txtLog.Size = new System.Drawing.Size(853, 82);
            this.txtLog.TabIndex = 17;
            // 
            // btClearCell
            // 
            this.btClearCell.Location = new System.Drawing.Point(7, 317);
            this.btClearCell.Name = "btClearCell";
            this.btClearCell.Size = new System.Drawing.Size(50, 23);
            this.btClearCell.TabIndex = 18;
            this.btClearCell.Text = "Clear";
            this.toolTip1.SetToolTip(this.btClearCell, "Clear selected slot\'s image");
            this.btClearCell.UseVisualStyleBackColor = true;
            this.btClearCell.Click += new System.EventHandler(this.btClearCell_Click);
            // 
            // panelImage
            // 
            this.panelImage.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.panelImage.AutoScroll = true;
            this.panelImage.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.panelImage.Controls.Add(this.imageEnhanced1);
            this.panelImage.Location = new System.Drawing.Point(113, 55);
            this.panelImage.Name = "panelImage";
            this.panelImage.Size = new System.Drawing.Size(667, 386);
            this.panelImage.TabIndex = 19;
            // 
            // toolStrip1
            // 
            this.toolStrip1.ImageScalingSize = new System.Drawing.Size(32, 32);
            this.toolStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripButtonOpen,
            this.toolStripButtonSave,
            this.toolStripButtonSaveAs,
            this.toolStripSeparator1,
            this.toolStripButtonCut,
            this.toolStripButtonCopy,
            this.toolStripButtonPaste,
            this.toolStripSeparator3,
            this.toolStripButtonUndo,
            this.toolStripButtonRedo,
            this.toolStripSeparator2,
            this.toolStripButtonSettings,
            this.toolStripSeparator4,
            this.toolStripButtonAbout});
            this.toolStrip1.Location = new System.Drawing.Point(0, 0);
            this.toolStrip1.Name = "toolStrip1";
            this.toolStrip1.Size = new System.Drawing.Size(872, 39);
            this.toolStrip1.TabIndex = 20;
            this.toolStrip1.Text = "Settings";
            // 
            // toolStripButtonOpen
            // 
            this.toolStripButtonOpen.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolStripButtonOpen.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButtonOpen.Image")));
            this.toolStripButtonOpen.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButtonOpen.Name = "toolStripButtonOpen";
            this.toolStripButtonOpen.Size = new System.Drawing.Size(36, 36);
            this.toolStripButtonOpen.Text = "Open Image";
            this.toolStripButtonOpen.ToolTipText = "Open Image (Ctrl-O)";
            this.toolStripButtonOpen.Click += new System.EventHandler(this.toolStripButtonOpen_Click);
            // 
            // toolStripButtonSave
            // 
            this.toolStripButtonSave.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolStripButtonSave.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButtonSave.Image")));
            this.toolStripButtonSave.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButtonSave.Name = "toolStripButtonSave";
            this.toolStripButtonSave.Size = new System.Drawing.Size(36, 36);
            this.toolStripButtonSave.Text = "Save Image";
            this.toolStripButtonSave.ToolTipText = "Save Image (Ctrl-S)";
            this.toolStripButtonSave.Click += new System.EventHandler(this.toolStripButtonSave_Click);
            // 
            // toolStripButtonSaveAs
            // 
            this.toolStripButtonSaveAs.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolStripButtonSaveAs.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButtonSaveAs.Image")));
            this.toolStripButtonSaveAs.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButtonSaveAs.Name = "toolStripButtonSaveAs";
            this.toolStripButtonSaveAs.Size = new System.Drawing.Size(36, 36);
            this.toolStripButtonSaveAs.Text = "Save Image As...";
            this.toolStripButtonSaveAs.Click += new System.EventHandler(this.toolStripButtonSaveAs_Click);
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(6, 39);
            // 
            // toolStripButtonCut
            // 
            this.toolStripButtonCut.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolStripButtonCut.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButtonCut.Image")));
            this.toolStripButtonCut.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButtonCut.Name = "toolStripButtonCut";
            this.toolStripButtonCut.Size = new System.Drawing.Size(36, 36);
            this.toolStripButtonCut.ToolTipText = "Cut (Ctrl-X)";
            this.toolStripButtonCut.Click += new System.EventHandler(this.toolStripButtonCut_Click);
            // 
            // toolStripButtonCopy
            // 
            this.toolStripButtonCopy.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolStripButtonCopy.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButtonCopy.Image")));
            this.toolStripButtonCopy.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButtonCopy.Name = "toolStripButtonCopy";
            this.toolStripButtonCopy.Size = new System.Drawing.Size(36, 36);
            this.toolStripButtonCopy.ToolTipText = "Copy (Ctrl-C)";
            this.toolStripButtonCopy.Click += new System.EventHandler(this.toolStripButtonCopy_Click);
            // 
            // toolStripButtonPaste
            // 
            this.toolStripButtonPaste.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolStripButtonPaste.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButtonPaste.Image")));
            this.toolStripButtonPaste.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButtonPaste.Name = "toolStripButtonPaste";
            this.toolStripButtonPaste.Size = new System.Drawing.Size(36, 36);
            this.toolStripButtonPaste.ToolTipText = "Paste (Ctrl-V)";
            this.toolStripButtonPaste.Click += new System.EventHandler(this.toolStripButtonPaste_Click);
            // 
            // toolStripSeparator3
            // 
            this.toolStripSeparator3.Name = "toolStripSeparator3";
            this.toolStripSeparator3.Size = new System.Drawing.Size(6, 39);
            // 
            // toolStripButtonUndo
            // 
            this.toolStripButtonUndo.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolStripButtonUndo.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButtonUndo.Image")));
            this.toolStripButtonUndo.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButtonUndo.Name = "toolStripButtonUndo";
            this.toolStripButtonUndo.Size = new System.Drawing.Size(36, 36);
            this.toolStripButtonUndo.Text = "Undo";
            this.toolStripButtonUndo.ToolTipText = "Undo (Ctrl-Z)";
            this.toolStripButtonUndo.Click += new System.EventHandler(this.toolStripButtonUndo_Click);
            // 
            // toolStripButtonRedo
            // 
            this.toolStripButtonRedo.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolStripButtonRedo.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButtonRedo.Image")));
            this.toolStripButtonRedo.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButtonRedo.Name = "toolStripButtonRedo";
            this.toolStripButtonRedo.Size = new System.Drawing.Size(36, 36);
            this.toolStripButtonRedo.Text = "Redo";
            this.toolStripButtonRedo.ToolTipText = "Redo (Ctrl-Y)";
            this.toolStripButtonRedo.Click += new System.EventHandler(this.toolStripButtonRedo_Click);
            // 
            // toolStripSeparator2
            // 
            this.toolStripSeparator2.Name = "toolStripSeparator2";
            this.toolStripSeparator2.Size = new System.Drawing.Size(6, 39);
            // 
            // toolStripButtonSettings
            // 
            this.toolStripButtonSettings.CheckOnClick = true;
            this.toolStripButtonSettings.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.toolStripButtonSettings.Image = ((System.Drawing.Image)(resources.GetObject("toolStripButtonSettings.Image")));
            this.toolStripButtonSettings.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButtonSettings.Name = "toolStripButtonSettings";
            this.toolStripButtonSettings.Size = new System.Drawing.Size(50, 36);
            this.toolStripButtonSettings.Text = "Settings";
            this.toolStripButtonSettings.ToolTipText = "Toggle Settings Panel";
            this.toolStripButtonSettings.CheckedChanged += new System.EventHandler(this.toolStripButtonSettings_CheckedChanged);
            // 
            // toolStripSeparator4
            // 
            this.toolStripSeparator4.Name = "toolStripSeparator4";
            this.toolStripSeparator4.Size = new System.Drawing.Size(6, 39);
            // 
            // toolStripButtonAbout
            // 
            this.toolStripButtonAbout.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.toolStripButtonAbout.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolStripButtonAbout.Name = "toolStripButtonAbout";
            this.toolStripButtonAbout.Size = new System.Drawing.Size(40, 36);
            this.toolStripButtonAbout.Text = "About";
            this.toolStripButtonAbout.Click += new System.EventHandler(this.toolStripButtonAbout_Click);
            // 
            // label1
            // 
            this.label1.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(4, 453);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(28, 13);
            this.label1.TabIndex = 21;
            this.label1.Text = "Log:";
            // 
            // chkShowGrid
            // 
            this.chkShowGrid.AutoSize = true;
            this.chkShowGrid.Checked = true;
            this.chkShowGrid.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chkShowGrid.Location = new System.Drawing.Point(141, 7);
            this.chkShowGrid.Name = "chkShowGrid";
            this.chkShowGrid.Size = new System.Drawing.Size(75, 17);
            this.chkShowGrid.TabIndex = 22;
            this.chkShowGrid.Text = "Show Grid";
            this.toolTip1.SetToolTip(this.chkShowGrid, "Check to show the pink grid");
            this.chkShowGrid.UseVisualStyleBackColor = true;
            this.chkShowGrid.CheckedChanged += new System.EventHandler(this.chkShowGrid_CheckedChanged);
            // 
            // toolTip1
            // 
            this.toolTip1.AutomaticDelay = 0;
            this.toolTip1.AutoPopDelay = 10000;
            this.toolTip1.InitialDelay = 0;
            this.toolTip1.ReshowDelay = 100;
            // 
            // btStar
            // 
            this.btStar.Image = ((System.Drawing.Image)(resources.GetObject("btStar.Image")));
            this.btStar.ImageAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.btStar.Location = new System.Drawing.Point(58, 260);
            this.btStar.Name = "btStar";
            this.btStar.Size = new System.Drawing.Size(50, 23);
            this.btStar.TabIndex = 29;
            this.btStar.Text = "Star";
            this.btStar.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.toolTip1.SetToolTip(this.btStar, "Add a star overlay to the selected icon");
            this.btStar.UseVisualStyleBackColor = true;
            this.btStar.Click += new System.EventHandler(this.btStar_Click);
            // 
            // label2
            // 
            this.label2.Location = new System.Drawing.Point(139, 28);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(145, 13);
            this.label2.TabIndex = 23;
            this.label2.Text = "Max corrupted pixels in line:";
            this.toolTip1.SetToolTip(this.label2, "The maximum corrupted pixels allowed for a line to be considered a seperator line" +
                    ". Set to 0 for no auto-fix of lines. Only takes effect when opening an image.");
            // 
            // numericUpDownPixels
            // 
            this.numericUpDownPixels.Increment = new decimal(new int[] {
            5,
            0,
            0,
            0});
            this.numericUpDownPixels.Location = new System.Drawing.Point(290, 26);
            this.numericUpDownPixels.Maximum = new decimal(new int[] {
            2000,
            0,
            0,
            0});
            this.numericUpDownPixels.Name = "numericUpDownPixels";
            this.numericUpDownPixels.Size = new System.Drawing.Size(52, 20);
            this.numericUpDownPixels.TabIndex = 24;
            this.toolTip1.SetToolTip(this.numericUpDownPixels, "The maximum corrupted pixels allowed for a line to be considered a seperator line" +
                    ". Set to 0 for no auto-fix of lines. Only takes effect when opening an image.");
            this.numericUpDownPixels.Value = new decimal(new int[] {
            50,
            0,
            0,
            0});
            // 
            // chkAutoBackup
            // 
            this.chkAutoBackup.AutoSize = true;
            this.chkAutoBackup.Checked = true;
            this.chkAutoBackup.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chkAutoBackup.Location = new System.Drawing.Point(222, 7);
            this.chkAutoBackup.Name = "chkAutoBackup";
            this.chkAutoBackup.Size = new System.Drawing.Size(153, 17);
            this.chkAutoBackup.TabIndex = 26;
            this.chkAutoBackup.Text = "Auto Backup on First Save";
            this.toolTip1.SetToolTip(this.chkAutoBackup, "Check to create an automatic backup when saving the modified image for the first " +
                    "time");
            this.chkAutoBackup.UseVisualStyleBackColor = true;
            // 
            // chkDisableActualFont
            // 
            this.chkDisableActualFont.AutoSize = true;
            this.chkDisableActualFont.Checked = true;
            this.chkDisableActualFont.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chkDisableActualFont.Location = new System.Drawing.Point(381, 7);
            this.chkDisableActualFont.Name = "chkDisableActualFont";
            this.chkDisableActualFont.Size = new System.Drawing.Size(153, 17);
            this.chkDisableActualFont.TabIndex = 27;
            this.chkDisableActualFont.Text = "Disable Actual Font Editing";
            this.toolTip1.SetToolTip(this.chkDisableActualFont, "Uncheck to allow editing font characters");
            this.chkDisableActualFont.UseVisualStyleBackColor = true;
            this.chkDisableActualFont.CheckedChanged += new System.EventHandler(this.chkDisableActualFont_CheckedChanged);
            // 
            // chkSplit
            // 
            this.chkSplit.AutoSize = true;
            this.chkSplit.Checked = true;
            this.chkSplit.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chkSplit.Location = new System.Drawing.Point(381, 27);
            this.chkSplit.Name = "chkSplit";
            this.chkSplit.Size = new System.Drawing.Size(148, 17);
            this.chkSplit.TabIndex = 28;
            this.chkSplit.Text = "Allow Suspicious Cell Split";
            this.toolTip1.SetToolTip(this.chkSplit, "If a too-wide cell is found, attempt to split it and fix corrupted pixels. Only t" +
                    "akes effect when opening an image.");
            this.chkSplit.UseVisualStyleBackColor = true;
            // 
            // btCalcAlpha
            // 
            this.btCalcAlpha.Location = new System.Drawing.Point(58, 289);
            this.btCalcAlpha.Name = "btCalcAlpha";
            this.btCalcAlpha.Size = new System.Drawing.Size(50, 23);
            this.btCalcAlpha.TabIndex = 32;
            this.btCalcAlpha.Text = "Alpha!";
            this.toolTip1.SetToolTip(this.btCalcAlpha, "Calculate transparency for the selected slot");
            this.btCalcAlpha.UseVisualStyleBackColor = true;
            this.btCalcAlpha.Click += new System.EventHandler(this.btCalcAlpha_Click);
            // 
            // trackBarAlphaCalc
            // 
            this.trackBarAlphaCalc.Location = new System.Drawing.Point(5, 371);
            this.trackBarAlphaCalc.Maximum = 100;
            this.trackBarAlphaCalc.Name = "trackBarAlphaCalc";
            this.trackBarAlphaCalc.Size = new System.Drawing.Size(104, 45);
            this.trackBarAlphaCalc.TabIndex = 33;
            this.toolTip1.SetToolTip(this.trackBarAlphaCalc, "Set calculated transparency sensitivity");
            this.trackBarAlphaCalc.Value = 20;
            this.trackBarAlphaCalc.Scroll += new System.EventHandler(this.trackBarAlphaCalc_Scroll);
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(177)));
            this.label3.Location = new System.Drawing.Point(4, 28);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(129, 13);
            this.label3.TabIndex = 25;
            this.label3.Text = "Open Image Settings:";
            // 
            // panelSettings
            // 
            this.panelSettings.BackColor = System.Drawing.SystemColors.InactiveCaptionText;
            this.panelSettings.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.panelSettings.Controls.Add(this.btExternalEditor);
            this.panelSettings.Controls.Add(this.lbExternalEditor);
            this.panelSettings.Controls.Add(this.btBGColor);
            this.panelSettings.Controls.Add(this.panel1);
            this.panelSettings.Controls.Add(this.label5);
            this.panelSettings.Controls.Add(this.chkAutoBackup);
            this.panelSettings.Controls.Add(this.chkShowGrid);
            this.panelSettings.Controls.Add(this.chkSplit);
            this.panelSettings.Controls.Add(this.label2);
            this.panelSettings.Controls.Add(this.chkDisableActualFont);
            this.panelSettings.Controls.Add(this.numericUpDownPixels);
            this.panelSettings.Controls.Add(this.label3);
            this.panelSettings.Dock = System.Windows.Forms.DockStyle.Top;
            this.panelSettings.Location = new System.Drawing.Point(0, 39);
            this.panelSettings.Name = "panelSettings";
            this.panelSettings.Size = new System.Drawing.Size(872, 50);
            this.panelSettings.TabIndex = 30;
            // 
            // btExternalEditor
            // 
            this.btExternalEditor.Location = new System.Drawing.Point(714, 26);
            this.btExternalEditor.Name = "btExternalEditor";
            this.btExternalEditor.Size = new System.Drawing.Size(24, 19);
            this.btExternalEditor.TabIndex = 33;
            this.btExternalEditor.Text = "...";
            this.btExternalEditor.UseVisualStyleBackColor = true;
            this.btExternalEditor.Click += new System.EventHandler(this.btExternalEditor_Click);
            // 
            // lbExternalEditor
            // 
            this.lbExternalEditor.AutoSize = true;
            this.lbExternalEditor.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.lbExternalEditor.Location = new System.Drawing.Point(551, 27);
            this.lbExternalEditor.Name = "lbExternalEditor";
            this.lbExternalEditor.Size = new System.Drawing.Size(157, 15);
            this.lbExternalEditor.TabIndex = 32;
            this.lbExternalEditor.Text = "< No External Editor Selected >";
            this.lbExternalEditor.SizeChanged += new System.EventHandler(this.lbExternalEditor_SizeChanged);
            // 
            // btBGColor
            // 
            this.btBGColor.Location = new System.Drawing.Point(554, 1);
            this.btBGColor.Name = "btBGColor";
            this.btBGColor.Size = new System.Drawing.Size(139, 22);
            this.btBGColor.TabIndex = 31;
            this.btBGColor.Text = "Select Background Color";
            this.btBGColor.UseVisualStyleBackColor = true;
            this.btBGColor.Click += new System.EventHandler(this.btBGColor_Click);
            // 
            // panel1
            // 
            this.panel1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.panel1.BackColor = System.Drawing.SystemColors.Highlight;
            this.panel1.Location = new System.Drawing.Point(0, 24);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(868, 1);
            this.panel1.TabIndex = 30;
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(177)));
            this.label5.Location = new System.Drawing.Point(4, 8);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(118, 13);
            this.label5.TabIndex = 29;
            this.label5.Text = "Edit Mode Settings:";
            // 
            // panelMain
            // 
            this.panelMain.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.panelMain.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
            this.panelMain.Controls.Add(this.btUpdate);
            this.panelMain.Controls.Add(this.btEdit);
            this.panelMain.Controls.Add(this.lbAlphaSens);
            this.panelMain.Controls.Add(this.label6);
            this.panelMain.Controls.Add(this.trackBarAlphaCalc);
            this.panelMain.Controls.Add(this.btCalcAlpha);
            this.panelMain.Controls.Add(this.label4);
            this.panelMain.Controls.Add(this.panelImage);
            this.panelMain.Controls.Add(this.label1);
            this.panelMain.Controls.Add(this.lbImageInfo);
            this.panelMain.Controls.Add(this.btStar);
            this.panelMain.Controls.Add(this.panelMarker);
            this.panelMain.Controls.Add(this.txtLog);
            this.panelMain.Controls.Add(this.txtPixel);
            this.panelMain.Controls.Add(this.txtCellHover);
            this.panelMain.Controls.Add(this.picCellSelected);
            this.panelMain.Controls.Add(this.picCellHover);
            this.panelMain.Controls.Add(this.btClearCell);
            this.panelMain.Controls.Add(this.txtCellSelected);
            this.panelMain.Controls.Add(this.btExport);
            this.panelMain.Controls.Add(this.btImport);
            this.panelMain.Location = new System.Drawing.Point(0, 42);
            this.panelMain.Name = "panelMain";
            this.panelMain.Size = new System.Drawing.Size(872, 566);
            this.panelMain.TabIndex = 31;
            // 
            // btEdit
            // 
            this.btEdit.Location = new System.Drawing.Point(58, 317);
            this.btEdit.Name = "btEdit";
            this.btEdit.Size = new System.Drawing.Size(50, 23);
            this.btEdit.TabIndex = 36;
            this.btEdit.Text = "Edit";
            this.btEdit.UseVisualStyleBackColor = true;
            this.btEdit.Click += new System.EventHandler(this.btEdit_Click);
            // 
            // lbAlphaSens
            // 
            this.lbAlphaSens.AutoSize = true;
            this.lbAlphaSens.Location = new System.Drawing.Point(41, 403);
            this.lbAlphaSens.Name = "lbAlphaSens";
            this.lbAlphaSens.Size = new System.Drawing.Size(0, 13);
            this.lbAlphaSens.TabIndex = 35;
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(2, 350);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(108, 13);
            this.label6.TabIndex = 34;
            this.label6.Text = "Alpha calc sensitivity:";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(177)));
            this.label4.Location = new System.Drawing.Point(119, 24);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(279, 13);
            this.label4.TabIndex = 31;
            this.label4.Text = "< Click the Settings button to edit the settings >";
            // 
            // colorDialog1
            // 
            this.colorDialog1.FullOpen = true;
            this.colorDialog1.ShowHelp = true;
            this.colorDialog1.SolidColorOnly = true;
            // 
            // btUpdate
            // 
            this.btUpdate.Enabled = false;
            this.btUpdate.Location = new System.Drawing.Point(44, 150);
            this.btUpdate.Name = "btUpdate";
            this.btUpdate.Size = new System.Drawing.Size(56, 32);
            this.btUpdate.TabIndex = 37;
            this.btUpdate.Text = "Update";
            this.toolTip1.SetToolTip(this.btUpdate, "Update slot from the externally edited image");
            this.btUpdate.UseVisualStyleBackColor = true;
            this.btUpdate.Click += new System.EventHandler(this.btUpdate_Click);
            // 
            // imageEnhanced1
            // 
            this.imageEnhanced1.BackColor = System.Drawing.SystemColors.Desktop;
            this.imageEnhanced1.DisableActualFontEdit = false;
            this.imageEnhanced1.FontImage = null;
            this.imageEnhanced1.Location = new System.Drawing.Point(4, 3);
            this.imageEnhanced1.Name = "imageEnhanced1";
            this.imageEnhanced1.SelectedCell = null;
            this.imageEnhanced1.ShowGrid = false;
            this.imageEnhanced1.Size = new System.Drawing.Size(649, 303);
            this.imageEnhanced1.TabIndex = 0;
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.GradientInactiveCaption;
            this.ClientSize = new System.Drawing.Size(872, 606);
            this.Controls.Add(this.panelSettings);
            this.Controls.Add(this.panelMain);
            this.Controls.Add(this.toolStrip1);
            this.MinimumSize = new System.Drawing.Size(800, 640);
            this.Name = "MainForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
            this.Text = "Civilization IV Game Font Editor by Asaf";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.MainForm_FormClosing);
            ((System.ComponentModel.ISupportInitialize)(this.picCellHover)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.picCellSelected)).EndInit();
            this.panelMarker.ResumeLayout(false);
            this.panelMarker.PerformLayout();
            this.panelImage.ResumeLayout(false);
            this.toolStrip1.ResumeLayout(false);
            this.toolStrip1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDownPixels)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.trackBarAlphaCalc)).EndInit();
            this.panelSettings.ResumeLayout(false);
            this.panelSettings.PerformLayout();
            this.panelMain.ResumeLayout(false);
            this.panelMain.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label lbImageInfo;
        private System.Windows.Forms.PictureBox picCellHover;
        private System.Windows.Forms.TextBox txtCellHover;
        private System.Windows.Forms.PictureBox picCellSelected;
        private System.Windows.Forms.TextBox txtCellSelected;
        private System.Windows.Forms.CheckBox chkMarker;
        private System.Windows.Forms.Label lbMarkerPos;
        private System.Windows.Forms.Button btMarkerPosPlus;
        private System.Windows.Forms.Button btMarkerPosMinus;
        private System.Windows.Forms.Panel panelMarker;
        private System.Windows.Forms.Button btExport;
        private System.Windows.Forms.Button btImport;
        private System.Windows.Forms.TextBox txtPixel;
        private System.Windows.Forms.TextBox txtLog;
        private System.Windows.Forms.Button btClearCell;
        private ScrollWorkaroundPanel panelImage;
        private System.Windows.Forms.ToolStrip toolStrip1;
        private System.Windows.Forms.ToolStripButton toolStripButtonOpen;
        private System.Windows.Forms.ToolStripButton toolStripButtonSave;
        private System.Windows.Forms.ToolStripButton toolStripButtonSaveAs;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.CheckBox chkShowGrid;
        private System.Windows.Forms.ToolTip toolTip1;
        private System.Windows.Forms.ToolStripButton toolStripButtonAbout;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.NumericUpDown numericUpDownPixels;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.CheckBox chkAutoBackup;
        private System.Windows.Forms.ToolStripButton toolStripButtonUndo;
        private System.Windows.Forms.ToolStripButton toolStripButtonRedo;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator2;
        private System.Windows.Forms.CheckBox chkDisableActualFont;
        private System.Windows.Forms.CheckBox chkSplit;
        private System.Windows.Forms.Button btStar;
        private System.Windows.Forms.ToolStripButton toolStripButtonCut;
        private System.Windows.Forms.ToolStripButton toolStripButtonCopy;
        private System.Windows.Forms.ToolStripButton toolStripButtonPaste;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator3;
        private System.Windows.Forms.ToolStripButton toolStripButtonSettings;
        private System.Windows.Forms.Panel panelSettings;
        private System.Windows.Forms.Panel panelMain;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator4;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Button btCalcAlpha;
        private System.Windows.Forms.TrackBar trackBarAlphaCalc;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label lbAlphaSens;
        private System.Windows.Forms.Button btBGColor;
        private System.Windows.Forms.ColorDialog colorDialog1;
        private System.Windows.Forms.Button btEdit;
        private System.Windows.Forms.Button btExternalEditor;
        private System.Windows.Forms.Label lbExternalEditor;
        private System.Windows.Forms.Button btUpdate;
    }
}

