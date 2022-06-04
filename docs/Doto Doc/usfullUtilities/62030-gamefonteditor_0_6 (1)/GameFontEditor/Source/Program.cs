using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.IO;

namespace GameFontEditor
{
    static class Program
    {
        static MainForm m_main;
        static EditorConfig m_config;
        static string m_configPath;

        public static void Log(string msg)
        {
            m_main.Log(msg);
        }

        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
#if !DEBUG
            try
#endif
            {
                Application.EnableVisualStyles();
                Application.SetCompatibleTextRenderingDefault(false);

                string myDocs = Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments);
                m_configPath = Path.Combine(myDocs, "Civ4GameFontEditor.cfg");

                m_config = EditorConfig.Load(m_configPath);

                m_main = new MainForm();
                m_main.UpdateFromConfig(m_config);
                Application.Run(m_main);
                m_main.UpdateToConfig(m_config);
                m_config.Save(m_configPath);
            }
#if !DEBUG
            catch (Exception ex)
            {
                DisplayException disp = new DisplayException();
                disp.Exception = ex;
                disp.ShowDialog();
            }
#endif
        }
    }
}