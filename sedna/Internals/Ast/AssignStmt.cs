using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Sedna.Core.Internals.Ast
{
    public class AssignStmt : IAst
    {
        public string Name { get; set; }        public IAst Value { get; set; }

        //very bad will need fixing
        public override bool IsValid(Token raw) => (bool)raw.Raw?.Trim().Contains("=");

        public override IAst Parse(Token raw)
        {
            var re = new AssignStmt();
            //txt = "Hello world"
            re.Name = raw.Raw.Split(' ')[0];

            if (raw.Raw.Contains('='))
            {
                re.Value = IAst.ParseToken(raw.Raw.Split('=').Last().Trim())[0];
            }

            return re;
        }
    }
}