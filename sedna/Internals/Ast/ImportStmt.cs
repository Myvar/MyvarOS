using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Sedna.Core.Internals.Ast
{
    public class ImportStmt : IAst
    {
        public string ScopeName { get; set; }

        public override bool IsValid(Token raw) => (bool)raw.Raw?.Trim().StartsWith("imports", StringComparison.Ordinal);

        public override IAst Parse(Token raw) => new ImportStmt() { ScopeName = raw.Raw?.Trim().Split(' ')[1] };
    }
}
