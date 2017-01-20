using System.Collections.Generic;

namespace Sedna.Core.Internals.Ast
{
    public class ResolveStmt : IAst
    {
        public List<string> Segments { get; set; } = new List<string>();

        public override bool IsValid(Token raw)
        {
            if (raw.Raw.Contains("+") || raw.Raw.Contains("-")
            || raw.Raw.Contains("*") || raw.Raw.Contains("/"))
            {
                //this is bad and we need to properly parse this to make sure there
                //is not just a oprator in the string
                return true;
            }
            else
            {
                return false;
            }
        }

        public override IAst Parse(Token raw)
        {
            var re = new ResolveStmt();

            var s = raw.Raw;

            var tmp = "";

            for (int i = 0; i < s.Length; i++)
            {
                if (s[i] == '+' || s[i] == '-' || s[i] == '*' || s[i] == '/')
                {
                    re.Segments.Add(tmp.Trim());
                    tmp = "";
                    re.Segments.Add(s[i].ToString().Trim());
                }
                else
                {
                    tmp += s[i];
                }
            }

            re.Segments.Add(tmp.Trim());

            return re;
        }

    }
}