using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Reflection;

namespace FSPChecker
{
    class Program
    {
        static void Main(string[] args)
        {
            AssemblyName assemblyName = Assembly.GetExecutingAssembly().GetName();
#if WINXP

            // XP用のソース
            Assembly asm = Assembly.GetEntryAssembly();
            if (asm == null) asm = Assembly.GetCallingAssembly();
            AppConst.APP_ROOT_PATH = Path.GetDirectoryName(asm.Location) + @"/";
#else
            AppConst.APP_ROOT_PATH = Path.GetDirectoryName(assemblyName.CodeBase) + @"\";
#endif
            Log.logFile = new StreamWriter(Path.Combine(AppConst.APP_ROOT_PATH, AppConst.CSHARP_LOG_NAME));
            Log.logFile.AutoFlush = true;
            CSharpFileWriteRead.Excute(AppConst.FAT_ROOT_PATH);
            Sqlite3Insert.Excute(AppConst.FAT_ROOT_PATH);

            CSharpFileWriteRead.Excute(AppConst.NEXT_ROOT_PATH);
            Sqlite3Insert.Excute(AppConst.NEXT_ROOT_PATH);
            Log.logFile.Close();
        }
    }
}
