using System;
using System.IO;
using EntryPoint;
using Newtonsoft.Json;

namespace BuildSystem
{
    class Program
    {
        static void Main(string[] args)
        {
            var arguments = Cli.Parse<CliArguments>(args);
            if (!String.IsNullOrEmpty(arguments.SourceCodeDirectory)) {
                
                var o = new Options();

                if(File.Exists(arguments.OptionsFile))
                {
                    o = JsonConvert.DeserializeObject<Options>(File.ReadAllText(arguments.OptionsFile));
                }
                else
                {
                    File.WriteAllText("BuildConf.json", JsonConvert.SerializeObject(o, Formatting.Indented));
                }

                var path = Path.GetFullPath(arguments.SourceCodeDirectory);

                if(!Directory.Exists(path))
                {
                    Console.WriteLine("Please provide src Directory");
                    return;
                }

                BuildSystemEngine.Compile(path, o);
            }
            else
            {
                Console.WriteLine("Please provide src Directory");
            }
        }
    }
}
