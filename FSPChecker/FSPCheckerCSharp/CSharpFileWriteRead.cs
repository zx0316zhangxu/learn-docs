using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Threading;
using System.Text;
using System.Collections.Generic;

namespace FSPChecker
{
    class CSharpFileWriteRead
    {

        public static void Excute(string root_path)
        {
            string tmp_path = Path.Combine(root_path, AppConst.CSHARP_FILE_NAME);

            using (FileStream fs = new FileStream(tmp_path, FileMode.Open, FileAccess.Write))
            {
                byte[] buf = new byte[4096];
                long count = 0;
                Log.WriteLogBuffer("C#_FileWrite_"+ root_path + "_START");

                Random rd = new Random();
                while (count < (10*1024*1024))
                {
                    fs.Seek(rd.Next(1024 * 1024 * 100), SeekOrigin.Begin);
                    fs.Write(buf, 0, 4096);
                    count += 4096;
                }
                Log.WriteLogBuffer("       C#_FileWrite:Size=" + count.ToString() + "");


                fs.Close();
                Log.WriteLogBuffer("C#_FileWrite_" + root_path + "_END");

            }
            using (FileStream fs = new FileStream(tmp_path, FileMode.Open, FileAccess.Read))
            {
                int count = 0;
                byte[] buf = new byte[4096];
                Log.WriteLogBuffer("C#_FileRead_" + root_path + "_START");

                Random rd = new Random();
                while (count < (10 * 1024 * 1024))
                {
                    fs.Seek(rd.Next(1024 * 1024 * 100), SeekOrigin.Begin);
                    fs.Read(buf, 0, 4096);
                    count+=4096;
                }
                Log.WriteLogBuffer("       C#_FileRead:Size=" + count.ToString() + "");

                fs.Close();
                Log.WriteLogBuffer("C#_FileRead_" + root_path + "_END");

            }
        }


    }
}
