using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Sedna.Core.Internals.Ast
{
    public class TypeStmt : IAst
    {
        public string Name { get; set; }
        public string BaseType { get; set; } = "";
        public List<IAst> Body { get; set; } = new List<IAst>();
        public List<string> Attributes { get; set; } = new List<string>();

        public override bool IsValid(Token raw) => (bool)raw.RawHeader?.Trim().StartsWith("type", StringComparison.Ordinal);

        public override IAst Parse(Token raw)
        {
            var re = new TypeStmt();

            re.Name = raw.RawHeader.Trim().Split(' ')[1];

            foreach(var i in raw.Body)
            {
                re.Body.AddRange(IAst.ParseToken(i));
            }

            if(raw.RawHeader.Split(')').Last().Contains(":"))
            {
                re.BaseType = raw.RawHeader.Split(':').Last().Trim();
            }

            return re;
        }
    }
}
