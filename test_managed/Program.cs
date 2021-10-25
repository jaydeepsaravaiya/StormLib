using System;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Threading;

using ManagedStormLib;
namespace ManagedStormLib_test
{
    class Program
    {
        static Random random = new Random();
        static void Main(string[] args)
        {
            try
            {
                var fileInfo = new FileInfo("war3patch.mpq");
                using var war3patch = new MPQArchive("war3patch.mpq", MPQOpenArchiveFlags.READ_ONLY);
                war3patch.OpenPatchArchive("map.w3x", null, MPQOpenArchiveFlags.READ_ONLY);
                war3patch.OpenFileEx("war3map.j", MpqFileSearchScope.FromMPQ, out MPQFile file);
                using (file)
                {
                    Console.WriteLine(file.Length);
                }
            }
            catch (Win32Exception e)
            {

                Console.WriteLine(e.Message);
            }
        }

        private static void Archive_Compacting(MPQUserData pvUserData, uint dwWorkType, ulong BytesProcessed, ulong TotalBytes)
        {
            Console.WriteLine("Compacting...");
        }
    }

}
