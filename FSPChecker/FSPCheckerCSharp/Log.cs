using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using System.IO;
using System.Reflection;
using System.Security.Cryptography;
using System.Diagnostics;

namespace FSPChecker
{
    class Log
    {
#if false
        [DllImport("libchecker.dll", EntryPoint = "WriteLogBuffer", CharSet = CharSet.Unicode)]
        public static extern void WriteLogBuffer(string buff);
#else
        public static StreamWriter logFile;
        public static void WriteLogBuffer(string buff)
        {
            Console.WriteLine(string.Format("{0}  {1}", DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss fff"), buff));
            logFile.WriteLine(string.Format("{0}  {1}", DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss fff"), buff));
            
        }
#endif
    }
}
