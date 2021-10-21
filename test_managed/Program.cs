using System;
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
            var testData = new byte[1024];
            random.NextBytes(testData);
            File.Delete("test.mpq");
            MPQArchive.CreateArchive("test.mpq", CreateArchiveFlags.ARCHIVE_V1 | CreateArchiveFlags.LISTFILE, 32, out MPQArchive created);
            using (created)
            {
                created.CreateFile("test.txt", DateTime.Now, (uint)testData.Length, null, AddFileFlags.COMPRESS, out MPQFile createdFile);
                using (createdFile)
                {
                    createdFile.WriteFile(testData, CompressionType.ZLIB);
                    createdFile.CloseFile();
                }
                created.FlushArchive();
                created.CloseArchive();
            }
            using var opened = new MPQArchive("test.mpq"); ;
            opened.OpenFileEx("test.txt", MpqFileSearchScope.FromMPQ, out MPQFile openedFile);
            using (openedFile)
            {
                openedFile.ReadFile(out byte[] data, openedFile.GetFileSize(), out uint readByets, new NativeOverlapped());
                if (data != testData)
                {
                    Console.WriteLine("Test Succeed!");
                }
                else
                {
                    Console.WriteLine("Test Failed!");
                }
            }
        }
    }
}
