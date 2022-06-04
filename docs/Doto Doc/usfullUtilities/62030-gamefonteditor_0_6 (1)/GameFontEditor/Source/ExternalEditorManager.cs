using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Drawing;
using System.Windows.Forms;
using FreeImageAPI;
using System.Diagnostics;

namespace GameFontEditor
{
    class ExternalEditorManager
    {
        private string m_workingFolder;
        private FileSystemWatcher m_watcher;
        private string m_externalEditorPath;
        private Dictionary<string, GameFontImage.FontCell> m_mapFileToLocation;
        private Dictionary<GameFontImage.FontCell, string> m_mapLocationToFile;

        public delegate void ImageFileChangedHandler(GameFontImage.FontCell cell);
        public event ImageFileChangedHandler ImageFileChanged;

        public string WorkingFolder
        {
            get { return m_workingFolder; }
        }

        public string ExternalEditorPath
        {
            get { return m_externalEditorPath; }
            set { m_externalEditorPath = value; }
        }

        public ExternalEditorManager(string workingFolder)
        {
            m_mapFileToLocation = new Dictionary<string, GameFontImage.FontCell>();
            m_mapLocationToFile = new Dictionary<GameFontImage.FontCell, string>();
            m_workingFolder = workingFolder;
            if (!Directory.Exists(workingFolder))
                Directory.CreateDirectory(workingFolder);

            // TODO: Check if there are leftovers from previous sessions?
            m_watcher = new FileSystemWatcher(m_workingFolder);
            m_watcher.NotifyFilter = NotifyFilters.LastWrite | NotifyFilters.FileName;
            m_watcher.Changed += new FileSystemEventHandler(OnFileChanged);
            m_watcher.Renamed += new RenamedEventHandler(OnFileRenamed);

            m_watcher.EnableRaisingEvents = true;
        }

        void OnFileRenamed(object sender, RenamedEventArgs e)
        {
            OnFileChanged(sender, e);
        }

        public void Reset()
        {
            m_watcher.EnableRaisingEvents = false;

            DirectoryInfo dirInfo = new DirectoryInfo(WorkingFolder);
            FileInfo[] files = dirInfo.GetFiles();
            if (files.Length > 0)
            {
                string msg = String.Format(
                    "There are {0} file(s) in the working folder ({1}). Do you want to delete them?", 
                    files.Length, WorkingFolder);

                DialogResult dr = MessageBox.Show(msg, "Files exist", MessageBoxButtons.YesNo, MessageBoxIcon.Question);
                if (dr == DialogResult.Yes)
                {
                    foreach (FileInfo file in files)
                        file.Delete();
                }
            }

            m_watcher.EnableRaisingEvents = true;

            m_mapFileToLocation.Clear();
            m_mapLocationToFile.Clear();
        }

        void OnFileChanged(object sender, FileSystemEventArgs e)
        {
            if (m_mapFileToLocation.ContainsKey(e.Name))
            {
                if (ImageFileChanged != null)
                {
                    ImageFileChanged(m_mapFileToLocation[e.Name]);
                }
            }
        }

        public bool EditImage(GameFontImage.FontCell imageCell)
        {
            if (!File.Exists(m_externalEditorPath))
                return false;

            if (m_mapLocationToFile.ContainsKey(imageCell))
            {
                DialogResult dr = MessageBox.Show("Image was already opened for editing. Do you want to delete the file and reopen this image for editing?",
                    "Overwrite confirmation", MessageBoxButtons.YesNo, MessageBoxIcon.Warning);

                if (dr == DialogResult.No)
                    return false;

                string oldfilename = m_mapLocationToFile[imageCell];
                File.Delete(Path.Combine(WorkingFolder, oldfilename));
                m_mapLocationToFile.Remove(imageCell);
                m_mapFileToLocation.Remove(oldfilename);
            }

            m_watcher.EnableRaisingEvents = false;
            string filename = Path.GetRandomFileName() + ".png";
            string fullPath = Path.Combine(WorkingFolder, filename);
            FreeImage.SaveBitmap(imageCell.Image, fullPath);
            m_watcher.EnableRaisingEvents = true;

            m_mapLocationToFile.Add(imageCell, filename);
            m_mapFileToLocation.Add(filename, imageCell);

            Process editor = Process.Start(m_externalEditorPath, "\"" + fullPath + "\"");
            return true;
        }

        public string GetFilePathForCell(GameFontImage.FontCell imageCell)
        {
            if (m_mapLocationToFile.ContainsKey(imageCell))
                return Path.Combine(WorkingFolder, m_mapLocationToFile[imageCell]);

            return null;
        }
    }
}
