using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Sedna.Core.Internals.Ast
{
    public class AttributeStmt : IAst
    {
        public string Atrribute { get; set; }

        public override bool IsValid(Token raw) => (bool)raw.Raw?.Trim().StartsWith("@", StringComparison.Ordinal);

        public override IAst Parse(Token raw)
        {
            var atr = raw.Raw.Split(new char[] { '@' }, StringSplitOptions.RemoveEmptyEntries);

            for (int i = 1; i < atr.Length; i++)
            {
                var att = atr[i];
                IAst.Buffer.AddRange(IAst.ParseToken(new Token { Raw = "@" + att.Trim() }));
            }

            return new AttributeStmt() { Atrribute = atr[0].Trim() };
        }
    }
}
