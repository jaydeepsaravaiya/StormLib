using ManagedStormLib;



using System;
using System.Collections;
using System.ComponentModel;
using System.Diagnostics;
using System.IO;
using System.IO.Compression;
using System.Linq;
using System.Threading;
using System.Windows;

namespace ManagedStormLib_test
{
    class Program
    {
        static void Main(string[] args)
        {

            var buff = new VariousBuffers();
            Console.ReadLine();
        }

        static (long usedMemory, long time) BenchmarkFunction(Action action)
        {
            var process = Process.GetCurrentProcess();
            var timer = new Stopwatch();
            long totalMemory = 0, totalTime = 0;
            var currentMemory = process.PrivateMemorySize64;
            timer.Start();
            action.Invoke();
            timer.Stop();
            totalTime += timer.ElapsedTicks;
            timer.Reset();
            totalMemory += process.PrivateMemorySize64 - currentMemory;
            return (totalMemory / 1, totalTime / 1);
        }
    }
    public class VariousBuffers : IDisposable
    {
        BinaryReader reader;
        MPQArchive patch;
        MPQFile file;
        public VariousBuffers()
        {
            try
            {
                patch = new MPQArchive(@"D:\Games\wow\Data\enUS\backup-enUS.MPQ", MPQOpenArchiveFlags.READ_ONLY);
                patch.OpenFileEx(@"Data\enUS\Interface\Cinematics\WOW_FotLK_1024.avi", MpqFileSearchScope.FromMPQ, out file);
                var x = patch.GetAttributes();
                patch.ExtractFile(@"Data\enUS\Interface\Cinematics\WOW_FotLK_1024.avi", "a.avi", MpqFileSearchScope.FromMPQ);

                Console.WriteLine(file.pFileEntry.md5);
                
            }
            catch (Win32Exception e)
            {
                Console.WriteLine(e.Message);
                throw;
            }
        }
        public void NativeFullRead()
        {
            file.ReadFile(out byte[] x, (uint)file.Length, out _, null);
        }
        public void NativeByByteRead()
        {
            while (file.Position < file.Length)
            {
                file.ReadFile(out byte[] x, 1, out _, null);
            }
        }
        public void NativeByFourByteRead()
        {
            while (file.Position < file.Length)
            {
                file.ReadFile(out byte[] x, (uint)Math.Min(4, file.Length - file.Position), out _, null);
            }
        }
        public void NativeByEightByteRead()
        {
            while (file.Position < file.Length)
            {
                file.ReadFile(out byte[] x, (uint)Math.Min(8, file.Length - file.Position), out _, null);
            }
        }
        public void NativeBy1KByteRead()
        {
            while (file.Position < file.Length)
            {
                file.ReadFile(out byte[] x, (uint)Math.Min(1024, file.Length - file.Position), out _, null);
            }
        }
        public void OneByteAtATime()
        {
            reader.BaseStream.Position = 0;
            while (reader.BaseStream.Position < reader.BaseStream.Length)
            {
                reader.ReadByte();
            }
        }

        public void FourByteAtATime()
        {
            reader.BaseStream.Position = 0;
            while (reader.BaseStream.Position < reader.BaseStream.Length)
            {
                reader.ReadBytes(4);
            }
        }

        public void EightByteAtATime()
        {
            reader.BaseStream.Position = 0;
            while (reader.BaseStream.Position < reader.BaseStream.Length)
            {
                reader.ReadBytes(8);
            }
        }

        public void OneKByteAtATime()
        {
            reader.BaseStream.Position = 0;
            while (reader.BaseStream.Position < reader.BaseStream.Length)
            {
                reader.ReadBytes(1024);
            }
        }

        public void ReadWholeAtTime()
        {
            reader.BaseStream.Position = 0;
            reader.ReadBytes((int)reader.BaseStream.Length);
        }

        public void Dispose()
        {
            patch.Dispose();
            reader.Dispose();

        }
    }

}
