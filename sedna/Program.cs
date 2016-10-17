using System;
using System.Collections.Generic;

namespace Sedna.Core
{
    public class Program
    {
        public static void Main(string[] args)
        {
            var c =  new Compiler();
            c.Compile(new List<string>() { "main.sn" }, "main.sbc", "" );
        }
    }
}
