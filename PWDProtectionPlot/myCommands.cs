// (C) Copyright 2015 by Microsoft 
//
using System;
using Autodesk.AutoCAD.Runtime;
using Autodesk.AutoCAD.ApplicationServices;
using Autodesk.AutoCAD.DatabaseServices;
using Autodesk.AutoCAD.Geometry;
using Autodesk.AutoCAD.EditorInput;
using Autodesk.AutoCAD.PlottingServices;
using System.Windows;
using System.Text;
using Autodesk.AutoCAD.Publishing;
using System.Collections.Specialized;


// This line is not mandatory, but improves loading performances
[assembly: CommandClass(typeof(PWDProtectionPlot.MyCommands))]

namespace PWDProtectionPlot
{

    // This class is instantiated by AutoCAD for each document when
    // a command is called by the user the first time in the context
    // of a given document. In other words, non static data in this class
    // is implicitly per-document!

    public class MyCommands
    {

        public static PasswordWindow pwdWindow;

        [CommandMethod("PublishViews2MultiSheet")]
        static public void PublishViews2MultiSheet()
        {
            pwdWindow = new PasswordWindow();
            pwdWindow.ShowDialog();
            Document doc = Autodesk.AutoCAD.ApplicationServices.Core.Application.DocumentManager.MdiActiveDocument;
            Database db = doc.Database;
            Editor ed = doc.Editor;
            StringCollection viewsToPlot = new StringCollection();
            viewsToPlot.Add("Test1");
            viewsToPlot.Add("Test2");
            using (Transaction Tx = db.TransactionManager.StartTransaction())
            {
                ObjectId layoutId = LayoutManager.Current.GetLayoutId(LayoutManager.Current.CurrentLayout);
                Layout layout = Tx.GetObject(layoutId, OpenMode.ForWrite) as Layout;
                foreach (String viewName in viewsToPlot)
                {
                    PlotSettings plotSettings = new PlotSettings(layout.ModelType);
                    plotSettings.CopyFrom(layout);
                    PlotSettingsValidator psv = PlotSettingsValidator.Current;
                    psv.SetPlotConfigurationName(plotSettings, "DWF6 ePlot.pc3", "ANSI_A_(8.50_x_11.00_Inches)");
                    psv.RefreshLists(plotSettings);
                    psv.SetPlotViewName(plotSettings, viewName);
                    psv.SetPlotType(plotSettings, Autodesk.AutoCAD.DatabaseServices.PlotType.View);
                    psv.SetUseStandardScale(plotSettings, true);
                    psv.SetStdScaleType(plotSettings, StdScaleType.ScaleToFit);
                    psv.SetPlotCentered(plotSettings, true);
                    psv.SetPlotRotation(plotSettings, PlotRotation.Degrees000);
                    psv.SetPlotPaperUnits(plotSettings, PlotPaperUnit.Inches);
                    plotSettings.PlotSettingsName = String.Format("{0}{1}", viewName, "PS");
                    plotSettings.PrintLineweights = true;
                    plotSettings.AddToPlotSettingsDictionary(db);
                    Tx.AddNewlyCreatedDBObject(plotSettings, true);
                    psv.RefreshLists(plotSettings);
                    layout.CopyFrom(plotSettings);
                }
                Tx.Commit();
            }
            short bgPlot = (short)Autodesk.AutoCAD.ApplicationServices.Core.Application.GetSystemVariable("BACKGROUNDPLOT");
            Autodesk.AutoCAD.ApplicationServices.Core.Application.SetSystemVariable("BACKGROUNDPLOT", 0);
            string dwgFileName = Autodesk.AutoCAD.ApplicationServices.Core.Application.GetSystemVariable("DWGNAME") as string;
            string dwgPath = Autodesk.AutoCAD.ApplicationServices.Core.Application.GetSystemVariable("DWGPREFIX") as string;
            using (Transaction Tx = db.TransactionManager.StartTransaction())
            {
                DsdEntryCollection collection = new DsdEntryCollection();
                ObjectId activeLayoutId = LayoutManager.Current.GetLayoutId(LayoutManager.Current.CurrentLayout);
                foreach (String viewName in viewsToPlot)
                {
                    Layout layout = Tx.GetObject(activeLayoutId, OpenMode.ForRead) as Layout;
                    DsdEntry entry = new DsdEntry();
                    entry.DwgName = dwgPath + dwgFileName;
                    entry.Layout = layout.LayoutName;
                    entry.Title = viewName;
                    entry.NpsSourceDwg = entry.DwgName;
                    entry.Nps = String.Format("{0}{1}", viewName, "PS");
                    collection.Add(entry);
                }
                dwgFileName = dwgFileName.Substring(0, dwgFileName.Length - 4);
                DsdData dsdData = new DsdData();
                dsdData.SheetType = SheetType.MultiDwf;
                dsdData.ProjectPath = dwgPath;
                dsdData.DestinationName = dsdData.ProjectPath + dwgFileName + ".dwf";
                /*Get password from user*/
                dsdData.Password = pwdWindow.passwordBox.Password;
                if (System.IO.File.Exists(dsdData.DestinationName)) System.IO.File.Delete(dsdData.DestinationName);
                dsdData.SetDsdEntryCollection(collection);

                /*DsdFile */
                string dsdFile = dsdData.ProjectPath + dwgFileName + ".dsd";
                dsdData.WriteDsd(dsdFile);
                System.IO.StreamReader sr = new System.IO.StreamReader(dsdFile);
                string str = sr.ReadToEnd();
                sr.Close();
                str = str.Replace("PromptForDwfName=TRUE",
                                   "PromptForDwfName=FALSE");
                /*Prompts User to Enter Password and Reconfirms*/
                //str = str.Replace("PromptForPwd=FALSE",
                //                   "PromptForPwd=TRUE");
                //str = str.Replace("PwdProtectPublishedDWF=FALSE", 
                //                   "PwdProtectPublishedDWF=TRUE");
                int occ = 0;
                int index = str.IndexOf("Setup=");
                int startIndex = 0;
                StringBuilder dsdText = new StringBuilder();
                while (index != -1)
                {
                    String str1 = str.Substring(startIndex, index + 6 - startIndex);
                    dsdText.Append(str1);
                    dsdText.Append(String.Format("{0}{1}", viewsToPlot[occ], "PS"));
                    startIndex = index + 6;
                    index = str.IndexOf("Setup=", index + 6);
                    if (index == -1)
                    {
                        dsdText.Append(str.Substring(startIndex, str.Length - startIndex));
                    }
                    occ++;
                }
                System.IO.StreamWriter sw = new System.IO.StreamWriter(dsdFile);
                sw.Write(dsdText.ToString());
                sw.Close();
                dsdData.ReadDsd(dsdFile);
                System.IO.File.Delete(dsdFile);
                PlotConfig plotConfig = PlotConfigManager.SetCurrentConfig("DWF6 ePlot.pc3");
                Publisher publisher = Autodesk.AutoCAD.ApplicationServices.Core.Application.Publisher;
                publisher.PublishExecute(dsdData, plotConfig);
                Tx.Commit();
            }
            Autodesk.AutoCAD.ApplicationServices.Core.Application.SetSystemVariable("BACKGROUNDPLOT", bgPlot);
        }
    }
}

