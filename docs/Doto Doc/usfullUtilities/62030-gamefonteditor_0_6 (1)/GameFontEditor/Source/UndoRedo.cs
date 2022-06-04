using System;
using System.Collections.Generic;

namespace GameFontEditor
{
    public interface IEditAction
    {
        // Returns true if was done
        bool Do();

        // Returns true if can be undone (otherwise will not be added to stack)
        bool CanUndo();

        // Undo the action (might be redone)
        void Undo();
    }

    public class UndoRedoStack
    {
        private Stack<IEditAction> m_undoStack;
        private Stack<IEditAction> m_redoStack;

        public UndoRedoStack()
        {
            m_undoStack = new Stack<IEditAction>();
            m_redoStack = new Stack<IEditAction>();
        }

        public bool UndoAvailable
        {
            get { return m_undoStack.Count > 0; }
        }

        public bool RedoAvailable
        {
            get { return m_redoStack.Count > 0; }
        }

        public IEditAction PeekUndo
        {
            get
            {
                return UndoAvailable ? m_undoStack.Peek() : null;
            }
        }

        public void Clear()
        {
            m_undoStack.Clear();
            m_redoStack.Clear();
        }

        private bool m_bInAction = false;

        public bool InAction
        {
            get { return m_bInAction; }
        }

        // Returns true if action was done
        public bool Do(IEditAction action)
        {
            try
            {
                m_bInAction = true;
                if (action.Do())
                {
                    if (action.CanUndo())
                    {
                        m_undoStack.Push(action);
                        m_redoStack.Clear();
                    }

                    return true;
                }

                return false;
            }
            finally
            {
                m_bInAction = false;
            }
        }

        public void Undo()
        {
            try
            {
                m_bInAction = true;
                if (m_undoStack.Count > 0)
                {
                    IEditAction action = m_undoStack.Pop();
                    action.Undo();
                    m_redoStack.Push(action);
                }
            }
            finally
            {
                m_bInAction = false;
            }
        }

        public void Redo()
        {
            try
            {
                m_bInAction = true;
                if (m_redoStack.Count > 0)
                {
                    IEditAction action = m_redoStack.Pop();

                    // Don't call UndoRedoStack.Do() since we don't want to clear the redo stack
                    if (action.Do())
                        m_undoStack.Push(action);
                }
            }
            finally
            {
                m_bInAction = false;
            }
        }
    }
}
