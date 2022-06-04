using System;
using System.Collections.Generic;
using System.Text;
using System.Xml;
using System.Xml.Serialization;
using System.Drawing;
using System.IO;
using System.ComponentModel;

namespace GameFontEditor
{
    /// <summary>
    /// A settings class which can be written to a config file and read back.
    /// </summary>
    [XmlRoot("EditorConfig")]
    public class EditorConfig
    {
        public EditorConfig()
        {
            m_bShowSettings = false;
            m_bShowGrid = true;
            m_bAutoBackup = true;
            m_bDisableFontEdit = true;
            m_nMaxCorruptedPixels = 50;
            m_bAllowCellSplit = true;
            m_bIsMaximized = false;
            m_windowRect = new Rectangle(100, 100, 880, 640);
            m_nAlphaSlider = 20;
            m_imageBGColor = Color.FromArgb(30, 108, 182);
            string myDocs = Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments);
            m_editedFilesFolder = Path.Combine(myDocs, "Civ4GameFontEditedImages");
        }

        bool m_bShowSettings;
        bool m_bShowGrid;
        bool m_bAutoBackup;
        bool m_bDisableFontEdit;
        int m_nMaxCorruptedPixels;
        bool m_bAllowCellSplit;
        bool m_bIsMaximized;
        Rectangle m_windowRect;
        int m_nAlphaSlider;
        Color m_imageBGColor;
        string m_externalEditorPath;
        string m_editedFilesFolder;

        public string EditedFilesFolder
        {
            get { return m_editedFilesFolder; }
            set { m_editedFilesFolder = value; }
        }

        public string ExternalEditorPath
        {
            get { return m_externalEditorPath; }
            set { m_externalEditorPath = value; }
        }

        public bool ShowSettings
        {
            get { return m_bShowSettings; }
            set { m_bShowSettings = value; }
        }
        
        public bool ShowGrid
        {
            get { return m_bShowGrid; }
            set { m_bShowGrid = value; }
        }

        public bool AutoBackup
        {
            get { return m_bAutoBackup; }
            set { m_bAutoBackup = value; }
        }

        public bool DisableFontEdit
        {
            get { return m_bDisableFontEdit; }
            set { m_bDisableFontEdit = value; }
        }

        public int MaxCorruptedPixels
        {
            get { return m_nMaxCorruptedPixels; }
            set { m_nMaxCorruptedPixels = value; }
        }

        public bool AllowCellSplit
        {
            get { return m_bAllowCellSplit; }
            set { m_bAllowCellSplit = value; }
        }

        public bool IsMaximized
        {
            get { return m_bIsMaximized; }
            set { m_bIsMaximized = value; }
        }

        public Rectangle WindowRect
        {
            get { return m_windowRect; }
            set { m_windowRect = value; }
        }

        public int AlphaSlider
        {
            get { return m_nAlphaSlider; }
            set { m_nAlphaSlider = value; }
        }

        [XmlIgnore()]
        public Color ImageBGColor
        {
            get { return m_imageBGColor; }
            set { m_imageBGColor = value; }
        }

        [XmlElement()]
        public string ImageBGColorString
        {
            get
            {
                TypeConverter cc = TypeDescriptor.GetConverter(m_imageBGColor);
                return cc.ConvertTo(m_imageBGColor, typeof(string)) as string;
            }

            set
            {
                TypeConverter cc = TypeDescriptor.GetConverter(m_imageBGColor);
                m_imageBGColor = (Color)cc.ConvertFrom(value);
            }
        }

        public void Save(string path)
        {
            XmlSerializer ser = new XmlSerializer(GetType());
            FileStream stream = new FileStream(path, FileMode.Create);
            ser.Serialize(stream, this);
            stream.Close();
        }

        public static EditorConfig Load(string path)
        {
            if (!File.Exists(path))
                return new EditorConfig();

            XmlSerializer ser = new XmlSerializer(typeof(EditorConfig));
            FileStream stream = new FileStream(path, FileMode.Open);
            EditorConfig cfg = ser.Deserialize(stream) as EditorConfig;
            stream.Close();
            return cfg;
        }
    }
}
