using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using System.Drawing.Imaging;
using System.Windows.Forms;
using FreeImageAPI;

namespace GameFontEditor
{
    public abstract class BaseEditAction : IEditAction
    {
        public delegate void ActionHandler(BaseEditAction action);

        public event ActionHandler ActionDone;
        public event ActionHandler ActionUndone;

        private uint m_id;
        public uint Id
        {
            get { return m_id; }
        }

        public static uint BaseId
        {
            get { return 0; }
        }

        private static uint s_curId = BaseId;

        public static uint CurrentId
        {
            get { return s_curId; }
        }

        protected BaseEditAction()
        {
            m_id = ++s_curId;
        }

        // Returns true if was done
        public bool Do()
        {
            if (DoInternal())
            {
                if (ActionUndone != null)
                    ActionUndone(this);

                return true;
            }

            return false;
        }

        // Returns true if can be undone (otherwise will not be added to stack)
        public abstract bool CanUndo();

        // Undo the action (might be redone)
        public void Undo()
        {
            if (!CanUndo())
                return;

            if (UndoInternal())
            {
                if (ActionDone != null)
                    ActionDone(this);
            }
        }

        protected abstract bool DoInternal();

        // Some classes might not implement this
        protected abstract bool UndoInternal();
    }

    public abstract class CellEditAction : BaseEditAction
    {
        protected GameFontImage.FontCell m_cell;

        public GameFontImage.FontCell Cell
        {
            get { return m_cell; }
        }
    }

    public class ClearCellAction : CellEditAction
    {
        public ClearCellAction(GameFontImage.FontCell cell, GameFontImage fontImage)
        {
            m_cell = cell;
            m_fontImage = fontImage;
            m_originalImage = cell.Image.Clone() as Bitmap;
        }

        private GameFontImage m_fontImage;
        private Bitmap m_originalImage;
        
        public override bool CanUndo()
        {
            return true;
        }

        protected override bool DoInternal()
        {
            m_fontImage.ClearCell(m_cell);
            Program.Log(String.Format("Cleared row {0} slot {1}", m_cell.RowNumber, m_cell.SlotNumber));
            return true;
        }

        protected override bool UndoInternal()
        {
            m_cell.Image = m_originalImage.Clone() as Bitmap;
            m_fontImage.OnCellImageImported(m_cell);
            return true;
        }
    }
    
    public class SetCellImageAction : CellEditAction
    {
        public SetCellImageAction(GameFontImage.FontCell cell, GameFontImage fontImage, Bitmap newImage)
        {
            m_cell = cell;
            m_fontImage = fontImage;
            m_originalImage = cell.Image.Clone() as Bitmap;
            m_NewImage = newImage;
        }

        private GameFontImage m_fontImage;
        private Bitmap m_originalImage;
        private Bitmap m_NewImage;

        public override bool CanUndo()
        {
            return true;
        }

        protected override bool DoInternal()
        {
            m_cell.Image = m_NewImage.Clone() as Bitmap;
            m_fontImage.OnCellImageImported(m_cell);
            return true;        
        }

        protected override bool UndoInternal()
        {
            m_cell.Image = m_originalImage.Clone() as Bitmap;
            m_fontImage.OnCellImageImported(m_cell);
            return true;
        }
    }

    public class CellMarkerChangeAction : CellEditAction
    {
        public CellMarkerChangeAction(GameFontImage.FontCell cell, GameFontImage fontImage, short? newPosition)
        {
            m_cell = cell;
            m_fontImage = fontImage;
            m_oldPosition = m_cell.MarkerRelativePosition;
            m_newPosition = newPosition;
        }

        private GameFontImage m_fontImage;
        private short? m_oldPosition;
        private short? m_newPosition;

        public override bool CanUndo()
        {
            return true;
        }

        protected override bool DoInternal()
        {
            m_fontImage.SetCellMarker(m_cell, m_newPosition);
            return true;
        }

        protected override bool UndoInternal()
        {
            m_fontImage.SetCellMarker(m_cell, m_oldPosition);
            return true;
        }
    }


}
