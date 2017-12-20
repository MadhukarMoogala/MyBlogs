using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Autodesk.AutoCAD.Runtime;
using Autodesk.AutoCAD.EditorInput;
using Autodesk.AutoCAD.ApplicationServices;
using System.Runtime.InteropServices;

namespace GetCommandList
{
    public class MyCommands
    {
        [DllImport("D:\\Arxprojects\\ListOfCommands\\extractCommands_ARX\\x64\\Debug\\extractCommands.arx",
            CallingConvention = CallingConvention.Cdecl,
            CharSet = CharSet.Unicode,
            EntryPoint = "cmdExtract")]

        private static extern ErrorStatus cmdExtract([Out] IntPtr[] cmdNames);

        [DllImport("D:\\Arxprojects\\ListOfCommands\\extractCommands_ARX\\x64\\Debug\\extractCommands.arx",
          CallingConvention = CallingConvention.Cdecl,
          CharSet = CharSet.Unicode,
          EntryPoint = "getSizeOfCmds")]

        private static extern int getSizeOfCmds();


        [CommandMethod("ExtCmd")]
        public static void cmd()
        {
            int count = getSizeOfCmds();
            IntPtr[] cmdNames = new IntPtr[count];
            ErrorStatus es = cmdExtract(cmdNames);
            string[] names = new string[count];
            for (int i = 0; i < count; i++)
            {
                names[i] = Marshal.PtrToStringBSTR(cmdNames[i]);
                Marshal.FreeBSTR(cmdNames[i]);
            }
        }
    }
}
