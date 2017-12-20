using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Text;

namespace BuildSystem
{
    public static class BuildSystemEngine
    {
        public static void Compile(string src, Options o)
        {
            var workingdir = Path.GetFullPath(src);

            var bin = Path.Combine(workingdir, o.BinDirectory);

            //clean out Bin
            if (Directory.Exists(bin)) Directory.Delete(bin, true);
            Directory.CreateDirectory(bin);

            Log("Compiling Source Code");

            //compile src
            var exclude = new List<string>() { o.BinDirectory, o.IncludesDirectory };
            IterateSrc(src, workingdir, o, exclude);

            var sb = new StringBuilder();

            foreach (var f in Directory.GetFiles(bin))
            {
                if (f.EndsWith(".o")) sb.Append($" \"{f}\"");
            }

            //link
            Log("Linking");
            if (RunLinker($"-m elf_i386 -T \"{Path.Combine(workingdir, o.LinkerFile)}\" -o {Path.Combine(bin, "kernel.bin")} {sb.ToString()}", workingdir))
            {
                Log($"Linking was a success");
            }
            else
            {
                Error($"Linking Failed");
                Environment.Exit(1);
            }

            //build iso
            Log("Building iso");
            if (File.Exists(Path.Combine(workingdir, "iso", "krnlld.bin"))) File.Delete(Path.Combine(workingdir, "iso", "krnlld.bin"));
            File.Copy(Path.Combine(bin, "kernel.bin"), Path.Combine(workingdir, "iso", "krnlld.bin"));
            if (RunMkisofs($"-o \"{bin}/myvaros.iso\" -b isolinux-debug.bin -c boot.cat -input-charset utf-8 -no-emul-boot -boot-load-size 4 -boot-info-table -quiet iso", workingdir))
            {
                Log($"Building iso was a success");
            }
            else
            {
                Error($"Building iso Failed");
                Environment.Exit(1);
            }

            //clean up
            Log("Cleaning up files");
            //delete all .o files in bin
            foreach (var f in Directory.GetFiles(bin))
            {
                if (f.EndsWith(".o")) File.Delete(f);
            }

            Log("starting Qemu");
            RunQemu($"-m 2G -cdrom \"{bin}/myvaros.iso\" -serial tcp:127.0.0.1:4444,server,nowait", workingdir);
        }

        public static void IterateSrc(string src, string workingdir, Options o, List<string> excludedDirectorys)
        {
            if (excludedDirectorys.Contains(src)) return;

            foreach (var f in Directory.GetFiles(src))
            {
                if (f.EndsWith(".c")) Gcc(f, workingdir, o);
                if (f.EndsWith(".asm")) Nasm(f, workingdir, o);
            }

            foreach (var d in Directory.GetDirectories(src))
            {
                IterateSrc(d, workingdir, o, excludedDirectorys);
            }
        }

        public static void Gcc(string file, string workingdir, Options o)
        {
            if (RunGcc($"{o.GccOpt} -I {o.IncludesDirectory} -c \"{file}\" -o \"{Path.Combine(workingdir, o.BinDirectory, new FileInfo(file).Name + Guid.NewGuid().ToString() + ".o")}\"", workingdir))
            {
                var fl = new FileInfo(file);
                Log($"Successfully Compiled {fl.Name}");
            }
            else
            {
                var fl = new FileInfo(file);
                Error($"Failed to Compile {fl.Name}");
                Environment.Exit(1);
            }
        }

        public static void Nasm(string file, string workingdir, Options o)
        {
            if (RunNasm($"-g -f elf \"{file}\" -o \"{Path.Combine(workingdir, o.BinDirectory, Guid.NewGuid().ToString() + ".o")}\"", workingdir))
            {
                var fl = new FileInfo(file);
                Log($"Successfully Compiled {fl.Name}");
            }
            else
            {
                var fl = new FileInfo(file);
                Error($"Failed to Compile {fl.Name}");
                Environment.Exit(1);
            }
        }

        public static bool RunGcc(string args, string workingdir) => StartProcess("gcc", workingdir, args);
        public static bool RunNasm(string args, string workingdir) => StartProcess("nasm", workingdir, args);
        public static bool RunLinker(string args, string workingdir) => StartProcess("ld", workingdir, args);
        public static bool RunMkisofs(string args, string workingdir) => StartProcess("mkisofs", workingdir, args);
        public static bool RunQemu(string args, string workingdir) => StartProcess("qemu-system-i386", workingdir, args, false, true);


        public static bool StartProcess(string pname, string workingdir, string args, bool WaitForExit = true, bool UseShellExecute = false)
        {
            var p = new Process();

            p.StartInfo.FileName = pname;
            p.StartInfo.WorkingDirectory = workingdir;
            p.StartInfo.Arguments = args;
            p.StartInfo.UseShellExecute = UseShellExecute;
            p.StartInfo.CreateNoWindow = !UseShellExecute;
            p.StartInfo.RedirectStandardError = !UseShellExecute;
            p.StartInfo.RedirectStandardOutput = !UseShellExecute;

            p.Start();
            if (WaitForExit)
            {
                p.WaitForExit();

                LogStream(p.StandardOutput);
                ErrorStream(p.StandardError);
            }
            else
            {
                return true;
            }

            return p.ExitCode == 0;
        }

        public static void LogStream(StreamReader s)
        {
            var ln = s.ReadLine();
            while (ln != null)
            {
                if (ln.ToLower().Contains("warn"))
                {
                    Warning(ln);
                }
                else
                {
                    Log(ln);
                }

                ln = s.ReadLine();
            }
        }

        public static void ErrorStream(StreamReader s)
        {
            var ln = s.ReadLine();
            while (ln != null)
            {
                if (ln.ToLower().Contains("warn"))
                {
                    Warning(ln);
                }
                else
                {
                    Error(ln);
                }
                ln = s.ReadLine();
            }
        }

        public static void Log(string msg)
        {
            Console.Write("[");
            Console.ForegroundColor = ConsoleColor.Green;
            Console.Write("LOG");
            Console.ResetColor();
            Console.Write("] ");
            Console.WriteLine(msg);
        }

        public static void Warning(string warn)
        {
            Console.Write("[");
            Console.ForegroundColor = ConsoleColor.Yellow;
            Console.Write("Warn");
            Console.ResetColor();
            Console.Write("] ");
            Console.WriteLine(warn);
        }

        public static void Error(string error)
        {
            Console.Write("[");
            Console.ForegroundColor = ConsoleColor.Red;
            Console.Write("Error");
            Console.ResetColor();
            Console.Write("] ");
            Console.WriteLine(error);
        }
    }
}