using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Sedna.Core.Internals.Ast
{
    public class ValueStmt : IAst
    {
        public string Value { get; set; } = "";

        public override bool IsValid(Token raw) => true;

        public override IAst Parse(Token raw) => new ValueStmt() { Value = raw.Raw };
    }
}
