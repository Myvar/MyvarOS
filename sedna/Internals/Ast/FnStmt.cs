using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;

namespace Sedna.Core.Internals.Ast
{
    public class FnStmt : IAst
    {
        public string Name { get; set; }
        public string ReturnType { get; set; }
        public List<IAst> Body { get; set; } = new List<IAst>();
        public List<string> Attributes { get; set; } = new List<string>();
        public Dictionary<string, string> Parameters { get; set; } = new Dictionary<string, string>();

        public override bool IsValid(Token raw) => (bool)raw.RawHeader?.Trim().StartsWith("fn", StringComparison.Ordinal);

        public override IAst Parse(Token raw)
        {
            var re = new FnStmt();

            re.Name = raw.RawHeader.Trim().Split(' ')[1].Split('(')[0];

            if (raw.RawHeader.Split(')').Last().Contains(":"))
            {
                re.ReturnType = raw.RawHeader.Split(':').Last().Trim();
            }


            var s1 = Regex.Split(raw.RawHeader.Replace(" ", ""), re.Name + "\\(")[1];
            var s2 = "";
            if (!string.IsNullOrEmpty(re.ReturnType))
            {
                 s2 = s1.Replace(":" + re.ReturnType, "").Trim().TrimEnd(')');
            }
            else
            {
                s2 = s1.Trim().TrimEnd(')');
            }

            if (s2 != "")
            {
                var prm = s2.Split(',');
                foreach(var i in prm)
                {
                    re.Parameters.Add(i.Split(':')[0], i.Split(':')[1]);
                }
            }




            foreach (var i in raw.Body)
            {
                re.Body.AddRange(IAst.ParseToken(i));
            }

            return re;
        }
    }
}
