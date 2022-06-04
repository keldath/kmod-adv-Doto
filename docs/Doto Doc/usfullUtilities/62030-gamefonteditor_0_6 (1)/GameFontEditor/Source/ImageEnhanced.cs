using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace GameFontEditor
{
    public partial class ImageEnhanced : UserControl
    {
        Rectangle m_drawnRect;
        Rectangle m_drawnSelectedRect;
        Rectangle m_disableRect;
        GameFontImage.FontCell m_curHighlightedCell;
        GameFontImage.FontCell m_curSelectedCell;

        public GameFontImage.FontCell HighlightedCell
        {
            get { return m_curHighlightedCell; }
        }

        public GameFontImage.FontCell SelectedCell
        {
            get { return m_curSelectedCell; }
            set
            {
                if (m_curSelectedCell != value)
                {
                    m_curSelectedCell = value;
                    OnSelectedChanged();
                }
            }
        }

        private bool m_bDisableActualFontEdit = false;

        public bool DisableActualFontEdit
        {
            get { return m_bDisableActualFontEdit; }
            set { m_bDisableActualFontEdit = value; }
        }


        public delegate void CellChangedDelegate(GameFontImage.FontCell cell);
        public event CellChangedDelegate HighlightedCellChanged;
        public event CellChangedDelegate SelectedCellChanged;

        public delegate void PointedPixelChangedDelegate(int x, int y, Color c);
        public event PointedPixelChangedDelegate PointedPixelChanged;

        Brush m_highlightBrush;
        Brush m_selectedBrush;
        GameFontImage m_gameFontImage;
        bool m_bImageReplaced;
        bool m_bShowGrid;
        bool m_bDraw;
        int m_nFontAreaHeight;

        public ImageEnhanced()
        {
            InitializeComponent();
            m_disableRect = new Rectangle();
            m_bImageReplaced = false;
        }
        
        public bool ShowGrid
        {
            get { return m_bShowGrid; }
            set { m_bShowGrid = value; UpdateDisplayedImage(); }
        }
        
        public GameFontImage FontImage
        {
            get { return m_gameFontImage; }
            set 
            {
                if (m_gameFontImage != null)
                    m_gameFontImage.DisplayImageUpdated -= new EventHandler(m_gameFontImage_DisplayImageUpdated);
                m_gameFontImage = value;
                OnGameFontImageUpdated();
            }
        }

        void m_gameFontImage_DisplayImageUpdated(object sender, EventArgs e)
        {
            UpdateDisplayedImage();
        }

        private void UpdateDisplayedImage()
        {
            if (m_gameFontImage != null)
            {
                if (m_bShowGrid)
                    BackgroundImage = m_gameFontImage.DisplayedImage;
                else
                    BackgroundImage = m_gameFontImage.AnalyzedImage;

                Refresh();
            }
        }

        void OnGameFontImageUpdated()
        {
            if (m_gameFontImage == null)
            {
                m_bDraw = false;
                return;
            }

            m_gameFontImage.DisplayImageUpdated += new EventHandler(m_gameFontImage_DisplayImageUpdated);
            
            m_bImageReplaced = true;
            m_disableRect.Y = 0;
            m_disableRect.X = 0;
            m_disableRect.Width = m_gameFontImage.Data.Width;
            m_disableRect.Height = m_gameFontImage.ActualFontBottomSeperator;

            Color semiTransparentColor1 = Color.FromArgb(90, Color.Gray);
            m_highlightBrush = new SolidBrush(semiTransparentColor1);

            Color semiTransparentColor2 = Color.FromArgb(150, Color.Gray);
            m_selectedBrush = new SolidBrush(semiTransparentColor2);

            m_drawnSelectedRect = new Rectangle(0, 0, 0, 0);

            UpdateDisplayedImage();

            m_nFontAreaHeight = m_gameFontImage.ActualFontBottomSeperator + 1;
        }

        private void UserControl1_MouseMove(object sender, MouseEventArgs e)
        {
            GameFontImage.FontCell prevCell = m_curHighlightedCell;
            Rectangle newDrawnRect = new Rectangle();

            if (m_gameFontImage == null)
                return;

            if (PointedPixelChanged != null)
                PointedPixelChanged(e.X, e.Y, m_gameFontImage.Data.GetPixel(e.X, e.Y));

            m_bImageReplaced = false;

            if (m_bDisableActualFontEdit && e.Y < m_gameFontImage.ActualFontBottomSeperator)
            {
                newDrawnRect = m_disableRect;
                m_curHighlightedCell = null;
                m_bDraw = true;
            }
            else
            {
                m_curHighlightedCell = m_gameFontImage.GetCellByCoords(e.X, e.Y);
                if (m_curHighlightedCell != null)
                {
                    newDrawnRect = m_curHighlightedCell.Position;
                    m_bDraw = true;
                }
                else
                {
                    newDrawnRect.Width = 0;
                    newDrawnRect.Height = 0;
                    m_bDraw = false;
                }
            }

            UpdateDrawnRect(newDrawnRect);

            if (prevCell != m_curHighlightedCell)
            {
                if (HighlightedCellChanged != null)
                    HighlightedCellChanged(m_curHighlightedCell);
            }
        }

        private void UpdateDrawnRect(Rectangle newDrawnRect)
        {
            if (m_drawnRect != newDrawnRect)
            {
                if (!m_drawnRect.IsEmpty)
                    Invalidate(m_drawnRect);
                if (!newDrawnRect.IsEmpty)
                    Invalidate(newDrawnRect);
            }

            m_drawnRect = newDrawnRect;
        }

        private void UserControl1_Paint(object sender, PaintEventArgs e)
        {
            if (m_gameFontImage == null)
                return;

            if (m_bImageReplaced)
                return;

            if (!m_bDraw)
                return;

            if (m_highlightBrush != null)
            {
                if (!m_drawnRect.IsEmpty)
                    e.Graphics.FillRectangle(m_highlightBrush, m_drawnRect);
            }

            if (m_selectedBrush != null)
            {
                if (!m_drawnSelectedRect.IsEmpty)
                    e.Graphics.FillRectangle(m_selectedBrush, m_drawnSelectedRect);
            }
        }

        private void ImageEnhanced_Click(object sender, EventArgs e)
        {
            if (m_curSelectedCell != m_curHighlightedCell)
            {
                m_curSelectedCell = m_curHighlightedCell;
                OnSelectedChanged();
            }
        }

        private void OnSelectedChanged()
        {
            if (SelectedCellChanged != null)
                SelectedCellChanged(m_curSelectedCell);

            if (!m_drawnSelectedRect.IsEmpty)
                Invalidate(m_drawnSelectedRect);
            m_drawnSelectedRect = m_curSelectedCell != null ? m_curSelectedCell.Position : new Rectangle(0, 0, 0, 0);
            if (!m_drawnSelectedRect.IsEmpty)
                Invalidate(m_drawnSelectedRect);
        }

        private void ImageEnhanced_MouseLeave(object sender, EventArgs e)
        {
            m_curHighlightedCell = null;
            HighlightedCellChanged(m_curHighlightedCell);

            UpdateDrawnRect(Rectangle.Empty);
        }
    }
}
