using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.IO;
using System.Drawing;
using System.Drawing.Imaging;
using System.Text;
using System.Windows.Forms;
using FreeImageAPI;
using System.Reflection;

namespace GameFontEditor
{
    public partial class MainForm : Form
    {
        GameFontImage m_fontImage;
        string m_filePath;
        UndoRedoStack m_undoRedo;
        uint m_nLastSavedActionId = BaseEditAction.BaseId;
        bool m_bCanUndoToSave = true;
        bool m_bImageDirty = false;
        ExternalEditorManager m_externalEditorManager;

        public MainForm()
        {
            InitializeComponent();
            imageEnhanced1.HighlightedCellChanged += new ImageEnhanced.CellChangedDelegate(imageEnhanced1_HighlightedCellChanged);
            imageEnhanced1.SelectedCellChanged += new ImageEnhanced.CellChangedDelegate(imageEnhanced1_SelectedCellChanged);
            imageEnhanced1.PointedPixelChanged += new ImageEnhanced.PointedPixelChangedDelegate(imageEnhanced1_PointedPixelChanged);
            ResetCellDisplay();
            m_undoRedo = new UndoRedoStack();
            UpdateLayout();

            panelSettings.BackColor = Color.FromArgb(216, 228, 248);
            imageEnhanced1.BackColor = Color.FromArgb(30, 108, 182);
        }

        public void UpdateFromConfig(EditorConfig cfg)
        {
            this.Bounds = cfg.WindowRect;

            if (cfg.IsMaximized)
                this.WindowState = FormWindowState.Maximized;

            toolStripButtonSettings.Checked = cfg.ShowSettings;
            chkShowGrid.Checked = cfg.ShowGrid;
            chkSplit.Checked = cfg.AllowCellSplit;
            chkAutoBackup.Checked = cfg.AutoBackup;
            chkDisableActualFont.Checked = cfg.DisableFontEdit;
            numericUpDownPixels.Value = cfg.MaxCorruptedPixels;
            trackBarAlphaCalc.Value = cfg.AlphaSlider;
            imageEnhanced1.BackColor = cfg.ImageBGColor;
            m_externalEditorManager = new ExternalEditorManager(cfg.EditedFilesFolder);
            m_externalEditorManager.ExternalEditorPath = cfg.ExternalEditorPath;
            m_externalEditorManager.ImageFileChanged += new ExternalEditorManager.ImageFileChangedHandler(m_externalEditorManager_ImageFileChanged);
            UpdateLayout();
        }

        void m_externalEditorManager_ImageFileChanged(GameFontImage.FontCell cell)
        {
            if (m_fontImage == null)
                return;

            if (cell != null)
            {
                cell.ModifiedExternally = true;
                if (cell == imageEnhanced1.SelectedCell)
                {
                    SetControlPropertyThreadSafe(btUpdate, "Enabled", true);
                }
            }
        }

        public void UpdateToConfig(EditorConfig cfg)
        {
            cfg.IsMaximized = this.WindowState == FormWindowState.Maximized;
            if (cfg.IsMaximized)
                cfg.WindowRect = this.RestoreBounds;
            else
                cfg.WindowRect = this.Bounds;

            cfg.ShowSettings = toolStripButtonSettings.Checked;
            cfg.ShowGrid = chkShowGrid.Checked;
            cfg.AllowCellSplit = chkSplit.Checked;
            cfg.AutoBackup = chkAutoBackup.Checked;
            cfg.DisableFontEdit = chkDisableActualFont.Checked;
            cfg.MaxCorruptedPixels = (int)numericUpDownPixels.Value;
            cfg.AlphaSlider = trackBarAlphaCalc.Value;
            cfg.ImageBGColor = imageEnhanced1.BackColor;
            cfg.EditedFilesFolder = m_externalEditorManager.WorkingFolder;
            cfg.ExternalEditorPath = m_externalEditorManager.ExternalEditorPath;
        }

        void UpdateExternalEditor()
        {
            if (m_externalEditorManager == null ||
                m_externalEditorManager.ExternalEditorPath == null || 
                m_externalEditorManager.ExternalEditorPath.Length == 0)
            {
                lbExternalEditor.Text = "< No External Editor Selected >";
                toolTip1.SetToolTip(lbExternalEditor, "");
            }
            else
            {
                lbExternalEditor.Text = Path.GetFileNameWithoutExtension(m_externalEditorManager.ExternalEditorPath);
                toolTip1.SetToolTip(lbExternalEditor, m_externalEditorManager.ExternalEditorPath);
            }
        }

        void UpdateTitleBar()
        {
            string baseTitle = "Civilization IV Game Font Editor by Asaf";

            if (m_fontImage != null)
            {
                this.Text = (m_bImageDirty ? "*" : "") + Path.GetFileName(m_fontImage.Path) + " - " + baseTitle;
            }
            else
                this.Text = baseTitle;
        }

        void imageEnhanced1_PointedPixelChanged(int x, int y, Color c)
        {
            txtPixel.Text = String.Format("({1},{2}){0}R:{3}{0}G:{4}{0}B:{5}{0}A:{6}", Environment.NewLine,
                x, y, c.R, c.G, c.B, c.A);
        }

        void DisplayCellInfo(GameFontImage.FontCell cell, PictureBox pic, TextBox txt)
        {
            if (cell != null)
            {
                pic.Image = cell.Image;
                txt.Text = cell.ToString();
            }
            else
            {
                pic.Image = null;
                txt.Text = "";
            }
        }

        void imageEnhanced1_SelectedCellChanged(GameFontImage.FontCell cell)
        {
            DisplayCellInfo(cell, picCellSelected, txtCellSelected);
            UpdateMarkerControlsFromCell(cell);
            SetCellButtonsEnabled(cell != null);
            btUpdate.Enabled = (cell != null) && cell.ModifiedExternally;
        }

        void imageEnhanced1_HighlightedCellChanged(GameFontImage.FontCell cell)
        {
            DisplayCellInfo(cell, picCellHover, txtCellHover);
        }

        private void OpenImage()
        {
            if (!CanCloseImage())
                return;

            OpenFileDialog dlg = new OpenFileDialog();
            dlg.Filter = "Targa files (*.tga)|*.tga";
            DialogResult dr = dlg.ShowDialog();
            if (dr == DialogResult.OK)
            {
                CloseImage();
                LoadImage(dlg.FileName);
            }
        }

        void SetCellButtonsEnabled(bool bEnable)
        {
            btExport.Enabled = bEnable;
            btImport.Enabled = bEnable;
            btClearCell.Enabled = bEnable;
            btStar.Enabled = bEnable;
            btCalcAlpha.Enabled = bEnable;
            btEdit.Enabled = bEnable;
        }

        void ResetCellDisplay()
        {
            DisplayCellInfo(null, picCellSelected, txtCellSelected);
            DisplayCellInfo(null, picCellHover, txtCellHover);
            UpdateMarkerControlsFromCell(null);
            UpdateTitleBar();

            SetCellButtonsEnabled(false);
        }

        private void LoadImage(string path)
        {
#if !DEBUG
            try
#endif
            {
                Log(String.Format("Loading image file {0}", path));
                m_fontImage = new GameFontImage();
                m_fontImage.ImageGotDirty += new EventHandler(m_fontImage_ImageGotDirty);
                m_fontImage.AllowCellSplit = chkSplit.Checked;
                m_fontImage.Load(path, (int)numericUpDownPixels.Value);
                imageEnhanced1.DisableActualFontEdit = chkDisableActualFont.Checked;
                imageEnhanced1.ShowGrid = chkShowGrid.Checked;
                imageEnhanced1.FontImage = m_fontImage;
                lbImageInfo.Text = String.Format("Image size: {0}x{1} ({2})", m_fontImage.DisplayedImage.Width, m_fontImage.DisplayedImage.Height, path);
                imageEnhanced1.Size = new Size(m_fontImage.DisplayedImage.Width, m_fontImage.DisplayedImage.Height);
                picCellHover.Width = m_fontImage.FontFileType.IconDefaultWidth;
                picCellHover.Height = m_fontImage.FontFileType.IconHeight;
                m_filePath = path;
                ResetCellDisplay();
                Log("Image loaded successfully");
                UpdateTitleBar();
            }
#if !DEBUG
            catch (Exception ex)
            {
                string msg = String.Format("Failed loading image: {0}", ex.Message);
                MessageBox.Show(msg, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                Application.Exit();
            }
#endif
        }

        void m_fontImage_ImageGotDirty(object sender, EventArgs e)
        {
            if (!m_undoRedo.InAction)
            {
                // Image got dirty, but its undoable!
                m_bCanUndoToSave = false;
            }
            m_bImageDirty = true;
            UpdateTitleBar();
        }

        bool m_bInternalCheckUpdate = false;

        void UpdateMarkerControlsFromCell(GameFontImage.FontCell cell)
        {
            bool bMarkerExists = false;
            int nMarkerRelativePos = 0;
            int nMaxPos = 0;
            int nMinPos = 1;

            if (cell != null)
            {
                bMarkerExists = (cell.MarkerRelativePosition != null);
                if (bMarkerExists)
                {
                    nMarkerRelativePos = (int)cell.MarkerRelativePosition;
                    nMaxPos = cell.Height;
                }
            }

            chkMarker.Enabled = (cell != null);
            m_bInternalCheckUpdate = true;
            chkMarker.Checked = bMarkerExists;
            m_bInternalCheckUpdate = false;
            lbMarkerPos.Text = nMarkerRelativePos.ToString();
            btMarkerPosMinus.Enabled = nMarkerRelativePos > nMinPos;
            btMarkerPosPlus.Enabled = nMarkerRelativePos < nMaxPos;
            UpdateTitleBar();
        }

        private void AddCellMarkerAction(short? newPosition)
        {
            CellMarkerChangeAction action = new CellMarkerChangeAction(imageEnhanced1.SelectedCell, m_fontImage, newPosition);
            action.ActionDone += new BaseEditAction.ActionHandler(CellImageChanged);
            action.ActionUndone += new BaseEditAction.ActionHandler(CellImageChanged);
            m_undoRedo.Do(action);
        }

        private void btMarkerPosMinus_Click(object sender, EventArgs e)
        {
            AddCellMarkerAction((short)(imageEnhanced1.SelectedCell.MarkerRelativePosition - 1));
        }

        private void btMarkerPosPlus_Click(object sender, EventArgs e)
        {
            AddCellMarkerAction((short)(imageEnhanced1.SelectedCell.MarkerRelativePosition + 1));
        }

        private void checkBox1_CheckedChanged(object sender, EventArgs e)
        {
            if (imageEnhanced1.SelectedCell == null)
                return;

            if (m_bInternalCheckUpdate)
                return;

            short? newPos = null;
            if (imageEnhanced1.SelectedCell.MarkerRelativePosition == null)
                newPos = (short)(imageEnhanced1.SelectedCell.Height + 1 - m_fontImage.FontFileType.MarkerDefaultHeightBelowSeperator);

            AddCellMarkerAction(newPos);
        }

        private void Save()
        {
            if (m_fontImage == null)
                return;

            Cursor = Cursors.WaitCursor;
            if (chkAutoBackup.Checked)
            {
                BackupImage();
            }
            if (m_fontImage.Save(m_filePath))
            {
                m_bImageDirty = false;
                m_bCanUndoToSave = true;
                Log("Image Saved");
                if (m_undoRedo.PeekUndo != null)
                    m_nLastSavedActionId = (m_undoRedo.PeekUndo as BaseEditAction).Id;
                else
                    m_nLastSavedActionId = BaseEditAction.BaseId;
            }
            Cursor = Cursors.Default;
            UpdateTitleBar();
        }

        private string m_backupImagePath = null;

        private void BackupImage()
        {
            if (m_backupImagePath == null)
            {
                int i = 1;
                bool bFileExists = true;
                string filePath = "";
                while (bFileExists)
                {
                    string fileName = string.Format("{0}.{1:000}.bak", Path.GetFileName(m_filePath), i);
                    filePath = Path.Combine(Path.GetDirectoryName(m_filePath), fileName);
                    bFileExists = File.Exists(filePath);
                    ++i;
                }
                m_backupImagePath = filePath;

                // Only backup if the original file existed before the first save
                if (File.Exists(m_filePath))
                {
                    File.Copy(m_filePath, m_backupImagePath);
                }
            }

        }

        private void SaveAs()
        {
            if (m_fontImage == null)
                return;

            SaveFileDialog dlg = new SaveFileDialog();
            dlg.Filter = "Targa files (*.tga)|*.tga";
            dlg.OverwritePrompt = true;
            DialogResult dr = dlg.ShowDialog();
            if (dr == DialogResult.OK)
            {
                m_filePath = dlg.FileName;
                Save();
                lbImageInfo.Text = String.Format("Image size: {0}x{1} ({2})", m_fontImage.DisplayedImage.Width, m_fontImage.DisplayedImage.Height, m_filePath);
            }
        }

        private void btExport_Click(object sender, EventArgs e)
        {
            if (imageEnhanced1.SelectedCell == null)
                return;

            SaveFileDialog dlg = new SaveFileDialog();
            dlg.Filter = "Targa files (*.tga)|*.tga|PNG Files (*.png)|*.png";
            dlg.OverwritePrompt = true;
            DialogResult dr = dlg.ShowDialog();
            if (dr == DialogResult.OK)
            {
                FreeImage.SaveBitmap(imageEnhanced1.SelectedCell.Image, dlg.FileName);
            }
        }

        private void btImport_Click(object sender, EventArgs e)
        {
            if (imageEnhanced1.SelectedCell == null)
                return;

            OpenFileDialog dlg = new OpenFileDialog();
            dlg.Filter = "Targa files (*.tga)|*.tga|DDS files (*.dds)|*.dds|PNG Files (*.png)|*.png";
            DialogResult dr = dlg.ShowDialog();
            if (dr == DialogResult.Cancel)
                return;

            ImportImageToCell(dlg.FileName);
        }

        private void ImportImageToCell(string filename)
        {
            FREE_IMAGE_FORMAT format = FREE_IMAGE_FORMAT.FIF_UNKNOWN;
            Bitmap newImage = FreeImage.LoadBitmap(filename, FREE_IMAGE_LOAD_FLAGS.DEFAULT, ref format);
            if (newImage == null)
            {
                MessageBox.Show(String.Format("Failed loading {0}", filename), "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            if (SetCellImage(newImage))
                Log(String.Format("Imported image {0} to row {1} slot {2}", filename, imageEnhanced1.SelectedCell.RowNumber, imageEnhanced1.SelectedCell.SlotNumber));
        }

        bool SetCellImage(Bitmap newImage)
        {
            if (newImage.Width != imageEnhanced1.SelectedCell.Image.Width ||
                newImage.Height != imageEnhanced1.SelectedCell.Image.Height)
            {
                string msg = String.Format("Loaded image size ({0}x{1}) is different than this slot's size ({2}x{3})." +
                "Do you want to use it anyway (image will be resized)?",
                    newImage.Width, newImage.Height, imageEnhanced1.SelectedCell.Image.Width,
                    imageEnhanced1.SelectedCell.Image.Height);
                DialogResult dr = MessageBox.Show(msg, "Confirmation Required", MessageBoxButtons.YesNo, MessageBoxIcon.Warning);
                if (dr == DialogResult.No)
                    return false;

                FIBITMAP tmp = FreeImage.CreateFromBitmap(newImage);
                FIBITMAP resized = FreeImage.Rescale(tmp, imageEnhanced1.SelectedCell.Image.Width,
                    imageEnhanced1.SelectedCell.Image.Height, FREE_IMAGE_FILTER.FILTER_BILINEAR);
                newImage = FreeImage.GetBitmap(resized);
            }

            SetCellImageAction action = new SetCellImageAction(imageEnhanced1.SelectedCell, m_fontImage, newImage);
            action.ActionDone += new BaseEditAction.ActionHandler(CellImageChanged);
            action.ActionUndone += new BaseEditAction.ActionHandler(CellImageChanged);

            m_undoRedo.Do(action);

            return true;
        }

        public void Log(string msg)
        {
            txtLog.Text += String.Format("{0}{1}", msg, Environment.NewLine);
            txtLog.Select(txtLog.Text.Length - 2, 1);
            txtLog.ScrollToCaret();
        }

        private void btClearCell_Click(object sender, EventArgs e)
        {
            if (imageEnhanced1.SelectedCell == null || m_fontImage == null)
                return;

            ClearCell();
        }

        void ClearCell()
        {
            ClearCellAction action = new ClearCellAction(imageEnhanced1.SelectedCell, m_fontImage);
            action.ActionDone += new BaseEditAction.ActionHandler(CellImageChanged);
            action.ActionUndone += new BaseEditAction.ActionHandler(CellImageChanged);

            m_undoRedo.Do(action);

            Log(String.Format("Cleared row {0} slot {1}", imageEnhanced1.SelectedCell.RowNumber, imageEnhanced1.SelectedCell.SlotNumber));
        }

        void CellImageChanged(BaseEditAction action)
        {
            CellEditAction cellAction = action as CellEditAction;
            if (cellAction == null)
                return;

            if (imageEnhanced1.SelectedCell != cellAction.Cell)
                // Cell changed will be called internally
                imageEnhanced1.SelectedCell = cellAction.Cell;
            else
                imageEnhanced1_SelectedCellChanged(imageEnhanced1.SelectedCell);
            UpdateTitleBar();
        }

        private bool CanCloseImage()
        {
            if (m_fontImage == null)
                // Already closed
                return true;

            if (m_bImageDirty)
            {
                DialogResult dr = MessageBox.Show("You have modified the image. Would you like to save it?", "Confirmation", MessageBoxButtons.YesNoCancel, MessageBoxIcon.Warning);
                if (dr == DialogResult.Cancel)
                    return false;

                if (dr == DialogResult.Yes)
                    Save();
            }

            return true;
        }

        private void CloseImage()
        {
            m_externalEditorManager.Reset();
            m_fontImage = null;
            // Reset backup path
            m_backupImagePath = null;
            m_bImageDirty = false;
            m_bCanUndoToSave = true;
            ResetCellDisplay();
            m_undoRedo.Clear();
        }

        private void MainForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (!CanCloseImage())
                e.Cancel = true;
            else
                CloseImage();
        }

        private void toolStripButtonOpen_Click(object sender, EventArgs e)
        {
            OpenImage();
        }

        private void toolStripButtonSave_Click(object sender, EventArgs e)
        {
            Save();
        }

        private void toolStripButtonSaveAs_Click(object sender, EventArgs e)
        {
            SaveAs();
        }

        private void chkShowGrid_CheckedChanged(object sender, EventArgs e)
        {
            imageEnhanced1.ShowGrid = chkShowGrid.Checked;
        }

        private void toolStripButtonAbout_Click(object sender, EventArgs e)
        {
            AboutForm about = new AboutForm();
            about.ShowDialog();
        }

        private void toolStripButtonUndo_Click(object sender, EventArgs e)
        {
            DoUndo();
        }

        private void toolStripButtonRedo_Click(object sender, EventArgs e)
        {
            DoRedo();
        }

        private void DoUndo()
        {
            m_undoRedo.Undo();
            UpdateDirty();
        }

        private void DoRedo()
        {
            m_undoRedo.Redo();
            UpdateDirty();
        }

        void UpdateDirty()
        {
            if (m_bCanUndoToSave)
            {
                if (m_undoRedo.PeekUndo != null)
                {
                    m_bImageDirty = ((m_undoRedo.PeekUndo as BaseEditAction).Id != m_nLastSavedActionId);
                }
                else
                {
                    m_bImageDirty = (m_nLastSavedActionId != BaseEditAction.BaseId);
                }
            }
            UpdateTitleBar();
        }

        // the CreateBitmapFromDib function was taken from 
        // http://www.generoso.info/blog/wpf-handling-drag-and-drop-of-image-from-almost-every-source.html
        private System.Drawing.Bitmap CreateBitmapFromDib(Stream dib)
        {
            // We create a new Bitmap File in memory. 
            // This is the easiest way to convert a DIB to Bitmap. 
            // No PInvoke needed. 
            BinaryReader reader = new BinaryReader(dib);

            int headerSize = reader.ReadInt32();
            int pixelSize = (int)dib.Length - headerSize;
            int fileSize = 14 + headerSize + pixelSize;

            MemoryStream bmp = new MemoryStream(fileSize);
            BinaryWriter writer = new BinaryWriter(bmp);

            // 1. Write Bitmap File Header:              
            writer.Write((byte)'B');
            writer.Write((byte)'M');
            writer.Write(fileSize);
            writer.Write((int)0);
            writer.Write(14 + headerSize);

            // 2. Copy the DIB  
            dib.Position = 0;
            byte[] data = new byte[(int)dib.Length];
            dib.Read(data, 0, (int)dib.Length);
            writer.Write(data, 0, (int)data.Length);

            // 3. Create a new Bitmap from our new stream: 
            bmp.Position = 0;
            return new System.Drawing.Bitmap(bmp);
        }

        void Copy()
        {
            if (imageEnhanced1.SelectedCell != null && m_fontImage != null)
            {
                Clipboard.Clear();

                using (MemoryStream pngStream = new MemoryStream())
                {
                    DataObject data = new DataObject();

                    imageEnhanced1.SelectedCell.Image.Save(pngStream, ImageFormat.Png);
                    data.SetData("PNG", pngStream);
                    data.SetData("System.Drawing.Bitmap", imageEnhanced1.SelectedCell.Image);
                    Clipboard.SetDataObject(data, true);
                }
            }
        }

        void Cut()
        {
            if (imageEnhanced1.SelectedCell != null && m_fontImage != null)
            {
                Copy();
                ClearCell();
            }
        }

        void Paste()
        {
            if (imageEnhanced1.SelectedCell == null || m_fontImage == null)
                return;

            Bitmap bmp = null;
            if (Clipboard.GetDataObject().GetDataPresent("System.Drawing.Bitmap"))
            {
                object obj = Clipboard.GetDataObject().GetData("System.Drawing.Bitmap");
                bmp = obj as Bitmap;
            }
            else if (Clipboard.GetDataObject().GetDataPresent(DataFormats.Dib))
            {
                object obj = Clipboard.GetDataObject().GetData(DataFormats.Dib);
                bmp = CreateBitmapFromDib(obj as Stream);
            }

            if (bmp != null)
            {
                SetCellImage(bmp);
            }
        }

        private void chkDisableActualFont_CheckedChanged(object sender, EventArgs e)
        {
            imageEnhanced1.DisableActualFontEdit = chkDisableActualFont.Checked;
            if (imageEnhanced1.SelectedCell != null &&
                imageEnhanced1.SelectedCell.RowNumber <= GameFontImage.NumActualFontRows)
            {
                imageEnhanced1.SelectedCell = null;
            }
        }
         
        private void btStar_Click(object sender, EventArgs e)
        {
            if (imageEnhanced1.SelectedCell == null)
                return;

            if (m_fontImage == null || m_fontImage.StarOverlay == null)
                return;

            Bitmap orig = imageEnhanced1.SelectedCell.Image;
            Bitmap star = m_fontImage.StarOverlay;
            if (orig.Width != star.Width || orig.Height != star.Height)
            {
                FreeImageBitmap fi_star = new FreeImageBitmap(star);
                fi_star.Rescale(orig.Width, orig.Height, FREE_IMAGE_FILTER.FILTER_BILINEAR);
                star = fi_star.ToBitmap();
            }

            Bitmap newImage = ImageUtils.PasteImage(star, orig);

            SetCellImageAction action = new SetCellImageAction(imageEnhanced1.SelectedCell, m_fontImage, newImage);
            action.ActionDone += new BaseEditAction.ActionHandler(CellImageChanged);
            action.ActionUndone += new BaseEditAction.ActionHandler(CellImageChanged);

            m_undoRedo.Do(action);
        }

        const int WM_KEYDOWN = 0x100;

        protected override bool ProcessCmdKey(ref Message msg, Keys keyData)
        {
            if (msg.Msg == WM_KEYDOWN)
            {
                switch (keyData)
                {
                    case Keys.Control | Keys.Z:
                        DoUndo();
                        break;
                    case Keys.Control | Keys.Y:
                        DoRedo();
                        break;
                    case Keys.Control | Keys.S:
                        Save();
                        break;
                    case Keys.Control | Keys.O:
                        OpenImage();
                        break;
                    case Keys.Control | Keys.X:
                        Cut();
                        break;
                    case Keys.Control | Keys.C:
                        Copy();
                        break;
                    case Keys.Control | Keys.V:
                        Paste();
                        break;
                }
            }
            return base.ProcessCmdKey(ref msg, keyData);
        }

        private void toolStripButtonCut_Click(object sender, EventArgs e)
        {
            Cut();
        }

        private void toolStripButtonCopy_Click(object sender, EventArgs e)
        {
            Copy();
        }

        private void toolStripButtonPaste_Click(object sender, EventArgs e)
        {
            Paste();
        }

        void UpdateLayout()
        {
            if (toolStripButtonSettings.Checked)
            {
                panelSettings.Visible = true;
            }
            else
            {
                panelSettings.Visible = false;
            }
            panelMain.Top = toolStrip1.Bottom;
            panelMain.Height = this.ClientRectangle.Bottom - panelMain.Top;
            lbAlphaSens.Text = trackBarAlphaCalc.Value.ToString();
            UpdateExternalEditor();
        }

        private void toolStripButtonSettings_CheckedChanged(object sender, EventArgs e)
        {
            UpdateLayout();
        }

        private void btCalcAlpha_Click(object sender, EventArgs e)
        {
            if (imageEnhanced1.SelectedCell == null)
                return;

            if (m_fontImage == null)
                return;
            
            Bitmap orig = imageEnhanced1.SelectedCell.Image;
            Bitmap newImage;

            bool bSuccess = ImageUtils.CreateImageWithTransparency(orig, out newImage, trackBarAlphaCalc.Value);
            if (!bSuccess)
            {
                MessageBox.Show("Failed applying transparency. See log for details", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            SetCellImageAction action = new SetCellImageAction(imageEnhanced1.SelectedCell, m_fontImage, newImage);
            action.ActionDone += new BaseEditAction.ActionHandler(CellImageChanged);
            action.ActionUndone += new BaseEditAction.ActionHandler(CellImageChanged);

            m_undoRedo.Do(action);
        }

        private void trackBarAlphaCalc_Scroll(object sender, EventArgs e)
        {
            lbAlphaSens.Text = trackBarAlphaCalc.Value.ToString();
        }

        private void btBGColor_Click(object sender, EventArgs e)
        {
            colorDialog1.Color = imageEnhanced1.BackColor;

            DialogResult dr = colorDialog1.ShowDialog();
            if (dr == DialogResult.OK)
                imageEnhanced1.BackColor = colorDialog1.Color;
        }

        private void btEdit_Click(object sender, EventArgs e)
        {
            if (imageEnhanced1.SelectedCell == null)
                return;

            if (m_fontImage == null)
                return;

            m_externalEditorManager.EditImage(imageEnhanced1.SelectedCell);
        }

        private void btExternalEditor_Click(object sender, EventArgs e)
        {
            OpenFileDialog d = new OpenFileDialog();
            d.Filter = "Executable files (*.exe)|*.exe";
            if (m_externalEditorManager.ExternalEditorPath == null || m_externalEditorManager.ExternalEditorPath.Length == 0)
            {
                d.InitialDirectory = Environment.GetFolderPath(Environment.SpecialFolder.ProgramFiles);
            }
            else
            {
                d.InitialDirectory = Path.GetDirectoryName(m_externalEditorManager.ExternalEditorPath);
            }

            DialogResult dr = d.ShowDialog();
            if (dr != DialogResult.OK)
                return;

            m_externalEditorManager.ExternalEditorPath = d.FileName;
            UpdateExternalEditor();
        }

        private void lbExternalEditor_SizeChanged(object sender, EventArgs e)
        {
            btExternalEditor.Left = lbExternalEditor.Right;
        }

        private void btUpdate_Click(object sender, EventArgs e)
        {
            if (imageEnhanced1.SelectedCell == null)
                return;

            if (m_fontImage == null)
                return;

            string filename = m_externalEditorManager.GetFilePathForCell(imageEnhanced1.SelectedCell);

            if (filename != null)
                ImportImageToCell(filename);

            imageEnhanced1.SelectedCell.ModifiedExternally = false;
            btUpdate.Enabled = false;
        }

        /***********************************************************************************/
        // Taken as-is from the selected answer at:
        // http://stackoverflow.com/questions/661561/how-to-update-gui-from-another-thread-in-c
        private delegate void SetControlPropertyThreadSafeDelegate(Control control, string propertyName, object propertyValue);

        public static void SetControlPropertyThreadSafe(Control control, string propertyName, object propertyValue)
        {
            if (control.InvokeRequired)
            {
                control.Invoke(new SetControlPropertyThreadSafeDelegate(SetControlPropertyThreadSafe), new object[] { control, propertyName, propertyValue });
            }
            else
            {
                control.GetType().InvokeMember(propertyName, BindingFlags.SetProperty, null, control, new object[] { propertyValue });
            }
        }
        /***********************************************************************************/

    }
}