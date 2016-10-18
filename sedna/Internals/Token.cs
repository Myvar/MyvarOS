using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Sedna.Core.Internals
{
    public class Token
    {
        public string RawHeader { get; set; } = "";
        public string Raw { get; set; } = "";
        public int Position { get; set; }
        public List<Token> Body { get; set; } = new List<Token>();
    }
}
