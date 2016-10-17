using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Sedna.Core.Internals.Ast
{
    public class DecStmt : IAst
    {
        public string Name { get; set; }
        public string Type { get; set; }
        public IAst Value { get; set; }

        public override bool IsValid(Token raw) => (bool)raw.Raw?.Trim().StartsWith("dec", StringComparison.Ordinal);

        public override IAst Parse(Token raw)
        {
            var re = new DecStmt();
            //dec txt : str = "Hello world"
            re.Name = raw.Raw.Split(' ')[1];
            
            if(raw.Raw.Split('=')[0].Contains(':'))
            {
                re.Type = raw.Raw.Split('=')[0].Split(':').Last().Trim();
            }

            if (raw.Raw.Contains('='))
            {
                re.Value = IAst.ParseToken(raw.Raw.Split('=').Last().Trim())[0];
            }

            return re;
        }
    }
}
