//
//////////////////////////////////////////////////////////////////////////////
//
//  Copyright 2016 Autodesk, Inc.  All rights reserved.
//
//  Use of this software is subject to the terms of the Autodesk license 
//  agreement provided at the time of installation or download, or which 
//  otherwise accompanies this software in either electronic or hard copy form.   
//
//Madhukar Moogala @ Autodesk 
//////////////////////////////////////////////////////////////////////////////


using System;
using System.Collections;
using System.Reflection;
using Autodesk.AutoCAD.Runtime;
using Autodesk.AutoCAD.DatabaseServices;
using Autodesk.AutoCAD.Geometry;
using Autodesk.AutoCAD.EditorInput;
using Autodesk.AutoCAD.ApplicationServices;
using System.IO;
using System.Text;

[assembly: ExtensionApplication(null)]
[assembly: CommandClass(typeof(filer.TestFiler))]

namespace filer
{
    ///<Summary>
    /// How to update Parameters on entities  of in-memory database
    ///</Summary>
public class TestFiler
{
    ///<Summary>
    /// How to update Parameters on entities  of in-memory database
    ///</Summary>
    ///
[CommandMethod("DIMUPDATE")]
public static void testDimUpdate()
{
Editor ed = Application.DocumentManager.MdiActiveDocument.Editor;
try
{
    string drawingFile = getDrawingLocation();
    using (Database db = new Database(false, true))
    {
        db.ReadDwgFile(drawingFile,
                        FileOpenMode.OpenForReadAndWriteNoShare,
                        false, null);
        db.CloseInput(true);
        using (Transaction tr = db.TransactionManager.StartTransaction())
        {
            BlockTableRecord btr = 
            (BlockTableRecord)tr.GetObject(SymbolUtilityServices.GetBlockModelSpaceId(db),
                                            OpenMode.ForRead);
            /*Width*/
            ObjectId wActionId = GetVariableByName(btr.ObjectId, "Width", false);
            string name = string.Empty;
            ResultBuffer rbuf = null;
            string expression = string.Empty;
            GetVariableValue(wActionId, ref name, ref rbuf, ref expression);
            expression = "30";//Width Value
            SetVariableValue(wActionId, null, expression);
                /*Check  if our values Got reflected in DB*/
            GetVariableValue(wActionId, ref name, ref rbuf, ref expression);
            if (String.Compare(expression, "30"/*hardCoded For Width*/).Equals(0))
                Application.ShowAlertDialog("Hey Success!");
            else //Quit processing further
                return;
            /*Length*/
            ObjectId lActionId = GetVariableByName(btr.ObjectId,
                                                    "Length", false);
            rbuf = null;
            GetVariableValue(lActionId, ref name, ref rbuf, ref expression);
            expression = "40";//height Value
            SetVariableValue(lActionId, null, expression);
            tr.Commit();
        }

        bool isEvaluationSuccess = AssocManager.EvaluateTopLevelNetwork(db, null, 0);
        if (isEvaluationSuccess)
        {
            //Now Check if the Values Updated in Entity
            using (Transaction tr = db.TransactionManager.StartTransaction())
            {
                BlockTableRecord btr = 
                (BlockTableRecord)tr.GetObject(SymbolUtilityServices.GetBlockModelSpaceId(db),
                                                OpenMode.ForRead);
                foreach (ObjectId id in btr)
                {
                    Entity ent = (Entity)tr.GetObject(id, OpenMode.ForRead);
                    Polyline pline = ent as Polyline;
                    if (pline != null)
                    {
                        //stream the points to Editor Console
                        for (int i = 0; i < pline.NumberOfVertices; i++)
                            ed.WriteMessage("\n" + 
                                            pline.GetPoint3dAt(i).ToString());
                    }


                }
                tr.Commit();
            }
        }
        else return; //We will quit

        //Now save the in-memory DB to Disk.
        db.SaveAs(new StringBuilder(drawingFile).Replace(".dwg",
                                                        "_new.dwg").ToString(),
                                                        DwgVersion.Current);


    }
}
catch (System.Exception ex)
{
    ed.WriteMessage(ex.ToString());
}

}

private static string getDrawingLocation()
{
    /*
    SolutionFolder
    │   
    ├───bin
    └───Debug
    │
    assembly.dll

    */
    var assemblyLoc = Assembly.GetExecutingAssembly().Location;
    var debugFolder = Path.GetDirectoryName(assemblyLoc);
    var binFolder = Path.GetDirectoryName(debugFolder);
    var solutionFolder = Path.GetDirectoryName(binFolder);
    var drawingLocationFilePath = solutionFolder + 
                                "\\TestParameters.dwg";
    return Path.GetFullPath(drawingLocationFilePath);
}

///<Summary>
/// Get Variable Name for Given variableId
///</Summary>
///
private static ObjectId GetVariableByName(ObjectId btrId,
string name,
bool createIfDoesNotExist)
{
    if (name.Length == 0)
        throw new Autodesk.AutoCAD.Runtime.Exception(
        ErrorStatus.InvalidInput);

    ObjectId networkId = AssocNetwork.GetInstanceFromObject(
    btrId,
    createIfDoesNotExist,
    true,
    "ACAD_ASSOCNETWORK");

    if (networkId == ObjectId.Null)
        return ObjectId.Null;

    using (Transaction Tx =
    btrId.Database.TransactionManager.StartTransaction())
    {
        using (AssocNetwork network =
        Tx.GetObject(networkId, OpenMode.ForRead, false)
        as AssocNetwork)
        {
            foreach (ObjectId actionId in network.GetActions)
            {
                if (actionId == ObjectId.Null)
                    continue;

                if (actionId.ObjectClass.IsDerivedFrom(
                RXObject.GetClass(typeof(AssocVariable))))
                {
                    //Check if we found our guy
                    AssocVariable var = Tx.GetObject(actionId, 
                                                    OpenMode.ForRead,
                                                    false)
                    as AssocVariable;

                    if (var.Name == name)
                        return actionId;
                }
            }
        }
    }

    //If we don't want to create a new variable, returns an error
    if (!createIfDoesNotExist)
        return ObjectId.Null;

    return ObjectId.Null;//We didnt find what we are looking
}
private static void GetVariableValue(ObjectId variableId,
ref string name,
ref ResultBuffer value,
ref string expression)

{
    using (Transaction Tx =
    variableId.Database.TransactionManager.StartTransaction())
    {
        AssocVariable var =
        Tx.GetObject(variableId, OpenMode.ForRead)
        as AssocVariable;

        name = var.Name;

        var.EvaluateExpression(ref value);

        expression = var.Expression;

        Tx.Commit();
    }
}
///<Summary>
/// Set Expression for Given variableId
///</Summary>
///
private static void SetVariableValue(ObjectId variableId,
ResultBuffer value,
string expression)
{
    using (Transaction Tx =
    variableId.Database.TransactionManager.StartTransaction())
    {
        AssocVariable var =
        Tx.GetObject(variableId, OpenMode.ForWrite)
        as AssocVariable;

        if (expression != string.Empty)
        {
            string errMsg = string.Empty;

            var.ValidateNameAndExpression(
            var.Name,
            expression,
            ref errMsg);

            var.SetExpression(
            expression, "", true, true, ref errMsg, true);

            ResultBuffer evaluatedExpressionValue = null;

            var.EvaluateExpression(ref evaluatedExpressionValue);

            var.Value = evaluatedExpressionValue;
        }
        else
        {
            var.Value = value;
        }

        Tx.Commit();
    }
}
}
}
