using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Sedna.Core.Internals;
using Sedna.Core.Internals.Ast;
using System.Collections;
using Newtonsoft.Json;

namespace Sedna.Core
{
    public class Compiler
    {
        private static CompilerScope _Scope = new CompilerScope();
       

        public Compiler()
        {

        }

        public void Compile(List<string> src, string Output, string target)
        {
            /*
                Steps:
                1. Tokenize
                2. Parse
                3. Astyfie
                4. Assemble
            */

           

            _Scope = new CompilerScope();

            var bc = new ByteCode();

            foreach (var i in src)
            {
                var x = Parser.Parse(File.ReadAllText(i));

                if (!CheckErrors()) return;
                bc.Emit(x);
            }

            bc.Write(Output);

        }

        public static bool CheckErrors()
        {
            if (Parser.Errors.Count == 0 && _Scope.Errors.Count == 0)
            {
                return true;
            }
            else
            {
                Console.WriteLine("Found Errors:");
                foreach (var i in Parser.Errors)
                {
                    Console.WriteLine(i);
                }

                foreach (var i in _Scope.Errors)
                {
                    Console.WriteLine(i);
                }
                return false;
            }
        }
    }

    public class Module
    {
        public string Name { get; set; }
        public string Description { get; set; }

        public List<Method> Methods { get; set; } = new List<Method>();
    }

    public class Method
    {
        public string NameSpace { get; set; }
        public string Class { get; set; }
        public string MethodName { get; set; }
        public List<string> Parameters { get; set; } = new List<string>();
        public string Returns { get; set; }
    }
}
