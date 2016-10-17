using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Sedna.Core.Internals.Ast
{
    public class InvokeStmt : IAst
    {
        public string Path { get; set; }
        public List<IAst> Perams { get; set; } = new List<IAst>();

        public override bool IsValid(Token raw)
        {
            if(string.IsNullOrEmpty(raw.Raw))
            {
                return false;
            }
            if (char.IsDigit(raw.Raw[0]))
            {
                return false;
            }
            if (raw.Raw.Trim().EndsWith(")", StringComparison.Ordinal) && 
                !raw.Raw.Trim().StartsWith("(", StringComparison.Ordinal))
            {
                return true;
            }
            return false;
        }

        public override IAst Parse(Token raw)
        {
            var re = new InvokeStmt();

            re.Path = raw.Raw.Trim().Split('(')[0];
            foreach (var i in raw.Raw.Trim().Remove(raw.Raw.Trim().Length - 1, 1).Replace(re.Path, "").Remove(0,1).Split(','))
            {
                var x = IAst.ParseToken(new Token() { Raw = i.Trim() });
                if(x != null)
                {
                    re.Perams.AddRange(x);
                }
            }

            return re;
        }
    }
}
