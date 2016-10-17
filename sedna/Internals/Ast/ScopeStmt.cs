using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Sedna.Core.Internals.Ast
{
    public class ScopeStmt : IAst
    {
        public string ScopeName { get; set; }

        public override bool IsValid(Token raw)
        {
            if (((bool)raw.Raw?.Trim().StartsWith("scope", StringComparison.Ordinal)))
            {
                if (Parser.FoundScope)
                {
                    Parser.Errors.Add(new Error() { ErrorCode = "E0001", Message = "More than one Scope Found.", Position = raw.Position });
                }
                else
                {
                    Parser.FoundScope = true;
                    return true;
                }
                return false;
            }
            else
            {
                return false;
            }

        }

        public override IAst Parse(Token raw) => new ScopeStmt() { ScopeName = raw.Raw.Trim().Split(' ')[1] };
    }
}
