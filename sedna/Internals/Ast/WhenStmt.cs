using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Sedna.Core.Internals.Ast
{
    public class WhenStmt : IAst
    {
        public List<IAst> Expression { get; set; } = new List<IAst>();

        public List<IAst> Body { get; set; } = new List<IAst>();

        public override bool IsValid(Token raw) => raw.RawHeader.Trim().StartsWith("when", StringComparison.Ordinal);

        public override IAst Parse(Token raw)
        {
            var re = new WhenStmt();

            var exspretion = raw.RawHeader.Replace("when", ""/* Replace is fine becuse only one when is vaild in statment */).Trim().Remove(0, 1);
            exspretion = exspretion.Remove(exspretion.Length - 1, 1);

            re.Expression = Parser.ParseExpression(new Token() { Raw = exspretion, Position = raw.Position - "when".Length });

            foreach (var i in raw.Body)
            {
                re.Body.AddRange(IAst.ParseToken(i));
            }

            return re;
        }
    }
}
