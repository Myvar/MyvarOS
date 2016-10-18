using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Sedna.Core.Internals.Ast
{
    public class RetStmt : IAst
    {
        public IAst RetValue { get; set; }

        public override bool IsValid(Token raw) => (bool)raw.Raw?.Trim().StartsWith("return", StringComparison.Ordinal);

        public override IAst Parse(Token raw)
        {
            var re = new RetStmt();
            var x = IAst.ParseToken(new Token() { Raw = raw.Raw?.Trim().Split(' ')[1] });
            if (x != null)
            {
                re.RetValue = x[0];
            }

            return re;
        }
    }
}
