using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Data.SQLite;
using System.IO;

namespace FSPChecker
{
    class Sqlite3Insert
    {
        public static void Excute(string root_path)
        {
            
            string cs = @""+ root_path + AppConst.CSHARP_DB_NAME+"";
            if (File.Exists(cs))
            {
                File.Delete(cs);
            }
            SQLiteConnection.CreateFile(cs);
            var sql_con = new SQLiteConnection("Data Source="+cs+";Version=3;");

            sql_con.Open();

            var cmd = new SQLiteCommand(sql_con);

            cmd.CommandText = "DROP TABLE IF EXISTS tbTest";
            cmd.ExecuteNonQuery();

            cmd.CommandText = @"CREATE TABLE tbTest(id INTEGER PRIMARY KEY,
                    name TEXT, price INT)";
            cmd.ExecuteNonQuery();

            Log.WriteLogBuffer("C#_SqliteInsert_"+ root_path + "_START");
            for (var i = 0; i < 1000; i++)
            {
                using (var transaction = sql_con.BeginTransaction())
                {
                    // 100,000 inserts
                    for (var j = 0; j < 100; j++)
                    {
                        cmd.CommandText =
                            "INSERT INTO tbTest(name, price) VALUES('Audi'," + i*100+j + ");";
                        cmd.ExecuteNonQuery();
                    }

                    transaction.Commit();
                }
            }

            sql_con.Close();
            Log.WriteLogBuffer("C#_SqliteInsert_" + root_path + "_END");

        }
    }
}
