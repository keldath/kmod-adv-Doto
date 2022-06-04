using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Drawing;
using System.Drawing.Imaging;
using System.Windows.Forms;
using FreeImageAPI;

namespace GameFontEditor
{
    public class GameFontImage
    {
        public class GameFontFileType
        {
            public string filename;
            public int ImageWidth;
            public int ImageHeight;
            public int IconHeight;
            public int IconDefaultWidth;
            public int MarkerDefaultHeightBelowSeperator;
            public string OverlayStarFileName;
        }

        public const int NumActualFontRows = 4;

        private int m_nActualFontBottomSeperator;

        public int ActualFontBottomSeperator
        {
            get { return m_nActualFontBottomSeperator; }
        }

        private int m_nAllowedNumNonSepPixels = 50;

        private bool m_bAllowCellSplit = true;

        public bool AllowCellSplit
        {
            get { return m_bAllowCellSplit; }
            set { m_bAllowCellSplit = value; }
        }

        public event EventHandler ImageGotDirty;

        private Bitmap m_starOverlay;

        public Bitmap StarOverlay
        {
            get { return m_starOverlay; }
        }

        static List<GameFontFileType> s_types = null;

        static List<GameFontFileType> Types
        {
            get
            {
                if (s_types == null)
                {
                    s_types = new List<GameFontFileType>();

                    GameFontFileType small = new GameFontFileType();
                    small.filename = "GameFont_75.tga";
                    small.ImageWidth = 512;
                    small.ImageHeight = 256;
                    small.IconHeight = 16;
                    small.IconDefaultWidth = 16;
                    small.MarkerDefaultHeightBelowSeperator = 14;
                    small.OverlayStarFileName = "GameFont_star75.tga";
                    s_types.Add(small);

                    GameFontFileType large = new GameFontFileType();
                    large.filename = "GameFont.tga";
                    large.ImageWidth = 640;
                    large.ImageHeight = 320;
                    large.IconHeight = 20;
                    large.IconDefaultWidth = 21;
                    large.MarkerDefaultHeightBelowSeperator = 17;
                    large.OverlayStarFileName = "GameFont_star.tga";
                    s_types.Add(large);
                }
                return s_types;
            }
        }

        GameFontFileType FindFileType(int width, int height, string filename)
        {
            foreach (GameFontFileType type in Types)
            {
                if (type.ImageWidth == width && type.ImageHeight == height)
                    return type;
            }
            // Try finding by file name
            foreach (GameFontFileType type in Types)
            {
                if (filename.ToLower().Equals(type.filename.ToLower()))
                    return type;
            }

            // Non generic code, but oh well
            if (filename.Contains("75"))
                return Types[0];
            else
                return Types[1];

            //return null;
        }

        Color SeperatorColor
        {
            get { return Color.FromArgb(0, SeperatorColorOpaque); }
        }

        Color SeperatorColorOpaque
        {
            get { return Color.FromArgb(255, 0, 255); }
        }

        Color MarkerColor
        {
            get { return Color.FromArgb(0, MarkerColorOpaque); }
        }

        Color MarkerColorOpaque
        {
            get { return Color.FromArgb(0, 255, 255); }
        }

        public class FontCell
        {
            public Rectangle Position;
            public short? MarkerRelativePosition;
            public Bitmap Image;
            public int RowNumber;
            public int SlotNumber;
            public bool ModifiedExternally = false;

            public int Width
            {
                get { return Position.Width; }
            }

            public int Height
            {
                get { return Position.Height; }
            }
            
            public override string ToString()
            {
                return string.Format("{0}x{1}{5}Row {2} Slot {3}{5}Marker: {4}",
                    Position.Width, Position.Height, RowNumber, SlotNumber, MarkerRelativePosition, Environment.NewLine);
            }
        }

        public class FontRow
        {
            public int Index;
            public int TopSeperatorPosition;
            public int BottomSeperatorPosition;

            public int InnerTop
            {
                get { return TopSeperatorPosition + 1; }
            }
            
            public int InnerBottom
            {
                get { return BottomSeperatorPosition - 1; }
            }

            public int Height
            {
                get { return BottomSeperatorPosition - InnerTop; }
            }

            List<FontCell> m_cells;
            public List<FontCell> Cells
            {
                get 
                {
                    if (m_cells == null)
                        m_cells = new List<FontCell>();

                    return m_cells;
                }
            }
        }

        List<FontRow> m_listRows;

        public FontCell GetCellByCoords(int x, int y)
        {
            foreach (FontRow row in m_listRows)
            {
                if (y >= row.InnerTop && y <= row.InnerBottom)
                {
                    foreach (FontCell cell in row.Cells)
                    {
                        if (cell.Position.Left <= x && cell.Position.Right >= x)
                            return cell;
                    }
                }
            }
            return null;
        }

        private void FixCorruptedSeperatorLines(List<int> seperatorLines)
        {
            Program.Log("Fixing corrupted seperator lines...");
            foreach (int nLine in seperatorLines)
            {
                for (int col = 0; col < m_data.Width; ++col)
                {
                    m_data.SetPixel(col, nLine, SeperatorColor);
                }
            }

            UpdateImage();
        }

        // If split was successful - add previous cells to row, and modify given cell accordingly
        private void TrySplitCorruptedCell(FontRow row, FontCell cell)
        {
            bool bFixed = false;

            int nStart = cell.Position.Left;
            int nEnd = cell.Position.Right;

            const int nAvgThresholdPerPixel = 50;

            for (int col = nStart; col < nEnd; ++col)
            {
                int nColDistance = 0;
                List<int> pixelDistances = new List<int>();

                for (int y = cell.Position.Top; y < cell.Position.Bottom; ++y)
                {
                    Color color = m_data.GetPixel(col, y);
                    int nCurDistance = ImageUtils.CalcColorDistance(color, SeperatorColor);
                    pixelDistances.Add(nCurDistance);
                    nColDistance += nCurDistance;
                }

                int nBestDistance = nColDistance;

                short? markerPos = null;

                // See how much a marker in each position improves the score
                for (int y = cell.Position.Top; y < cell.Position.Bottom; ++y)
                {
                    Color color = m_data.GetPixel(col, y);
                    int nMarkerDistance = ImageUtils.CalcColorDistance(color, MarkerColor);
                    int nOrigDistance = pixelDistances[y - cell.Position.Top];
                    if (nBestDistance > nColDistance - nOrigDistance + nMarkerDistance)
                    {
                        nBestDistance = nColDistance - nOrigDistance + nMarkerDistance;
                        markerPos = Convert.ToInt16(cell.Position.Bottom - y);
                    }
                }

                if (nBestDistance <= nAvgThresholdPerPixel * cell.Height)
                {
                    // Found a seperating column - split cell and fix
                    FontCell newCell = new FontCell();
                    newCell.Position.X = cell.Position.X;
                    newCell.Position.Y = cell.Position.Y;
                    newCell.Position.Width = col - newCell.Position.X;
                    newCell.Position.Height = row.Height;
                    newCell.MarkerRelativePosition = markerPos;
                    newCell.Image = ImageUtils.BuildSubImage(m_data, newCell.Position);
                    newCell.SlotNumber = row.Cells.Count + 1;
                    newCell.RowNumber = row.Index + 1;

                    // Add new cell to row
                    row.Cells.Add(newCell);

                    // Update existing cell
                    cell.Position.X = col + 1;
                    cell.Position.Width -= (newCell.Width + 1);
                    cell.Image = ImageUtils.BuildSubImage(m_data, cell.Position);
                    cell.SlotNumber = row.Cells.Count + 1;

                    // Fix column
                    for (int y = cell.Position.Top; y < cell.Position.Bottom; ++y)
                    {
                        m_data.SetPixel(col, y, SeperatorColor);
                    }

                    if (markerPos != null)
                        m_data.SetPixel(col, cell.Position.Bottom - (int)markerPos, MarkerColor);

                    // And skip next column
                    ++col;

                    bFixed = true;
                    Program.Log(String.Format("Split corrupted cell {0} in row {1}", newCell.SlotNumber, row.Index + 1));
                }
            }

            if (bFixed)
                UpdateImage();
        }

        private void AnalyzeImageBits()
        {
            bool bFixSeperatorLines = false;

            List<int> seperatorLines = new List<int>();
            int nPixelIndex = 0;
            Color color;
            Color opaque;
            int numBadAlphaPixelSepCols = 0;

            ImageUtils.ImageData imgData = m_data;

            for (int nLine = 0; nLine < imgData.Height; ++nLine)
            {
                int numNonSeperatorPixels = 0;
                int numBadAlphaPixelSepLineCurLine = 0;
                nPixelIndex = nLine * imgData.Width;
                bool isSeparator = true;

                for (int nCol = 0; nCol < imgData.Width; ++nCol)
                {
                    color = Color.FromArgb(imgData.Data[nPixelIndex]);
                    opaque = Color.FromArgb(255, color);
                    ++nPixelIndex;
                    if (opaque != SeperatorColorOpaque)
                    {
                        numNonSeperatorPixels++;
                        //isSeparator = false;
                        //break;
                    }

                    if (color.A != 0)
                        ++numBadAlphaPixelSepLineCurLine;

                    if (numNonSeperatorPixels > m_nAllowedNumNonSepPixels)
                    {
                        isSeparator = false;
                        break;
                    }
                }

                if (isSeparator)
                {
                    seperatorLines.Add(nLine);

                    if (numNonSeperatorPixels > 0 || numBadAlphaPixelSepLineCurLine > 0)
                    {
                        bFixSeperatorLines = true;
                        Program.Log(String.Format("Found corrupted line @ y={0}. # bad pixels: {1} color, {2} alpha",
                            nLine, numNonSeperatorPixels, numBadAlphaPixelSepLineCurLine));
                    }
                }
            }
 
            // Find end position of actual font
            m_nActualFontBottomSeperator = -1;
            if (seperatorLines.Count >= NumActualFontRows)
                m_nActualFontBottomSeperator = seperatorLines[NumActualFontRows - 1];

            // Create rows
            m_listRows = new List<FontRow>();

            int prevSeperator = -1;
            for (int index = 0; index < seperatorLines.Count; ++index)
            {
                if (prevSeperator + 1 == seperatorLines[index])
                    // Two lines in a row - we've reached the end of the relevant data
                    break;

                FontRow row = new FontRow();
                row.TopSeperatorPosition = prevSeperator;
                row.BottomSeperatorPosition = seperatorLines[index];
                row.Index = m_listRows.Count;
                m_listRows.Add(row);

                prevSeperator = seperatorLines[index];
            }
            
            // Build cells data
            foreach (FontRow row in m_listRows)
            {
                int nPrevSepCol = -1;
                for (int column = 0; column < imgData.Width; ++column)
                {
                    bool bIsSepColumn = true;
                    bool bFoundMarker = false;
                    short markerPosition = 0;
                    // Scan columns, look for seperators and markers
                    for (int line = row.InnerTop; line <= row.InnerBottom; ++line)
                    {
                        int relativeLine = line - row.TopSeperatorPosition;
                        color = m_data.GetPixel(column, line);
                        opaque = Color.FromArgb(255, color);

                        if (!bFoundMarker && opaque == MarkerColorOpaque)
                        {
                            bFoundMarker = true;
                            markerPosition = Convert.ToInt16(relativeLine);
                        }
                        else if (opaque != SeperatorColorOpaque)
                        {
                            bIsSepColumn = false;
                            break;
                        }

                        if (color.A != 0)
                            ++numBadAlphaPixelSepCols;
                    }

                    if (bIsSepColumn)
                    {
                        if (nPrevSepCol + 1 == column)
                        {
                            // reached end of data
                            break;
                        }

                        FontCell cell = new FontCell();
                        cell.Position.X = nPrevSepCol + 1;
                        cell.Position.Y = row.InnerTop;
                        cell.Position.Width = column - cell.Position.X;
                        cell.Position.Height = row.Height;

                        // Inverse, so it would be counted from the bottom
                        if (bFoundMarker)
                            cell.MarkerRelativePosition = Convert.ToInt16(row.Height + 1 - markerPosition);
                        else
                            cell.MarkerRelativePosition = null;
                        cell.Image = ImageUtils.BuildSubImage(imgData, cell.Position);
                        cell.SlotNumber = row.Cells.Count + 1;
                        cell.RowNumber = row.Index + 1;

                        if (cell.Width > m_type.IconDefaultWidth)
                        {
                            if (m_bAllowCellSplit)
                            {
                                // Something is fishy here - look for a corrupted seperator column
                                TrySplitCorruptedCell(row, cell);
                            }
                            else
                            {
                                Program.Log(String.Format("Skipped suspicious cell @ ({0},{1})", cell.RowNumber, cell.SlotNumber));
                            }
                        }

                        row.Cells.Add(cell);
                        nPrevSepCol = column;
                    }
                }
            }

            Program.Log(String.Format("Image Analyzed: Found {0} rows", m_listRows.Count));

            if (bFixSeperatorLines)
                FixCorruptedSeperatorLines(seperatorLines);
        }

        private void SetDataPixel(int x, int y, Color c)
        {
            int index = m_data.Width * y + x;
            m_data.Data[index] = c.ToArgb();
        }

        public void SetCellMarker(FontCell cell, short? relativePos)
        {
            if (relativePos == cell.MarkerRelativePosition)
                return;

            int x = cell.Position.Right;

            if (cell.MarkerRelativePosition != null)
            {
                SetDataPixel(x, cell.Position.Bottom - (int)cell.MarkerRelativePosition, SeperatorColor);
            }

            cell.MarkerRelativePosition = relativePos;

            if (cell.MarkerRelativePosition != null)
            {
                SetDataPixel(x, cell.Position.Bottom - (int)cell.MarkerRelativePosition, MarkerColor);
            }

            UpdateImage();
        }

        Bitmap m_displayedImage;
        Bitmap m_analyzedImage;
        string m_filePath;
        ImageUtils.ImageData m_data;
        GameFontFileType m_type;

        public ImageUtils.ImageData Data
        {
            get { return m_data; }
        }

        public Image DisplayedImage
        {
            get { return m_displayedImage; }
        }

        public Image AnalyzedImage
        {
            get { return m_analyzedImage; }
        }
        
        public string Path
        {
            get { return m_filePath; }
        }

        public GameFontFileType FontFileType
        {
            get { return m_type; }
        }

        public GameFontImage()
        {
        }

        public void Load(string path, int nAllowedNumNonSepPixels)
        {
            m_filePath = path;
            m_nAllowedNumNonSepPixels = nAllowedNumNonSepPixels;
            FREE_IMAGE_FORMAT eFormat = FREE_IMAGE_FORMAT.FIF_TARGA;
            // Data must be with alpha channel
            m_analyzedImage = FreeImage.LoadBitmap(path, FREE_IMAGE_LOAD_FLAGS.DEFAULT, ref eFormat);
            // Display with pink
            m_displayedImage = FreeImage.LoadBitmap(path, FREE_IMAGE_LOAD_FLAGS.TARGA_LOAD_RGB888, ref eFormat);
            //m_displayedImage = m_analyzedImage;

            m_data = ImageUtils.GetDataFromImage(m_analyzedImage);
            m_type = FindFileType(m_data.Width, m_data.Height, System.IO.Path.GetFileName(path));

            string starFile = System.IO.Path.Combine(Application.StartupPath, m_type.OverlayStarFileName);
            m_starOverlay = FreeImage.LoadBitmap(starFile, FREE_IMAGE_LOAD_FLAGS.DEFAULT, ref eFormat);
            Program.Log(String.Format("Image size: {0}x{1}", m_data.Width, m_data.Height));
            AnalyzeImageBits();
        }

        public event EventHandler DisplayImageUpdated;

        public void UpdateImage()
        {
            UpdateImage(true);
        }

        public void UpdateImage(bool bSetDirty)
        {
            // Copy bits back to analyzed image
            Rectangle rect = new Rectangle(0, 0, m_analyzedImage.Width, m_analyzedImage.Height);
            BitmapData bmpData = m_analyzedImage.LockBits(rect, ImageLockMode.ReadWrite, PixelFormat.Format32bppArgb);
            System.Runtime.InteropServices.Marshal.Copy(m_data.Data, 0, bmpData.Scan0, m_data.Width * m_data.Height);
            m_analyzedImage.UnlockBits(bmpData);

            FIBITMAP tmp32 = FreeImage.CreateFromBitmap(m_analyzedImage);
            FIBITMAP tmp24 = FreeImage.ConvertTo24Bits(tmp32);
            m_displayedImage = FreeImage.GetBitmap(tmp24);

            if (bSetDirty)
            {
                if (ImageGotDirty != null)
                    ImageGotDirty(this, EventArgs.Empty);

                //m_bDirty = true;
            }

            if (DisplayImageUpdated != null)
                DisplayImageUpdated(this, EventArgs.Empty);
        }

        public bool Save(string path)
        {
            // Make sure everything's up to date
            UpdateImage(false);

            // Save analyzed image
            bool bSaved = FreeImage.SaveBitmap(m_analyzedImage, path, FREE_IMAGE_FORMAT.FIF_TARGA, FREE_IMAGE_SAVE_FLAGS.DEFAULT);
            if (bSaved)
            {
                m_filePath = path;
            }            
            return bSaved;
        }

        public void OnCellImageImported(FontCell cell)
        {
            ImageUtils.ImageData importedData = ImageUtils.GetDataFromImage(cell.Image);
            for (int line = 0; line < cell.Height; ++line)
            {
                for (int col = 0; col < cell.Width; ++col)
                {
                    int dstIndex = (line + cell.Position.Top) * m_data.Width + col + cell.Position.Left;
                    m_data.Data[dstIndex] = importedData.Data[line * importedData.Width + col];
                }
            }

            UpdateImage();
        }

        public void ClearCell(FontCell cell)
        {
            int emptyColor = Color.FromArgb(0, 255, 255, 255).ToArgb();
            for (int line = 0; line < cell.Height; ++line)
            {
                for (int col = 0; col < cell.Width; ++col)
                {
                    int dstIndex = (line + cell.Position.Top) * m_data.Width + col + cell.Position.Left;
                    m_data.Data[dstIndex] = emptyColor;
                }
            }

            cell.Image = ImageUtils.BuildSubImage(m_data, cell.Position);
            UpdateImage();
        }
    }
}
