using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;

namespace Sedna.Core.Internals.Ast
{
    public class ExpressionStmt : IAst
    {

        public IAst Left { get; set; }
        public IAst Right { get; set; }

        public string Operator { get; set; }

        public LogicOperator LogicOperator { get; set; }

        public override bool IsValid(Token raw)
        {
            if (raw.Raw.Trim().Length == 1 || raw.Raw.Trim().Length == 2)
            {
                return false;
            }
            if (raw.Raw.Trim().StartsWith("\""))
            {
                return false;
            }
            return raw.Raw.Contains("==") ||
                    raw.Raw.Contains("!=") ||
                    raw.Raw.Contains("<") ||
                    raw.Raw.Contains(">") ||
                    raw.Raw.Contains(">=") ||
                    raw.Raw.Contains("<=");
        }
        public override IAst Parse(Token raw)
        {
            var re = new ExpressionStmt();

            if(raw.Raw.Contains("=="))
            {
                var x = Regex.Split(raw.Raw, "==");
                re.Left = IAst.ParseToken(x[0].Trim())[0];
                re.Right = IAst.ParseToken(x[1].Trim())[0];
                re.Operator = "beq";
            }
            else if (raw.Raw.Contains("!="))
            {
                var x = Regex.Split(raw.Raw, "!=");
                re.Left = IAst.ParseToken(x[0].Trim())[0];
                re.Right = IAst.ParseToken(x[1].Trim())[0];
                re.Operator = "Bne";
            }
            else if (raw.Raw.Contains("<="))
            {
                var x = Regex.Split(raw.Raw, "<=");
                re.Left = IAst.ParseToken(x[0].Trim())[0];
                re.Right = IAst.ParseToken(x[1].Trim())[0];
                re.Operator = "Ble";
            }
            else if (raw.Raw.Contains(">="))
            {
                var x = Regex.Split(raw.Raw, ">=");
                re.Left = IAst.ParseToken(x[0].Trim())[0];
                re.Right = IAst.ParseToken(x[1].Trim())[0];
                re.Operator = "Bge";
            }
            else if (raw.Raw.Contains(">"))
            {
                var x = Regex.Split(raw.Raw, ">");
                re.Left = IAst.ParseToken(x[0].Trim())[0];
                re.Right = IAst.ParseToken(x[1].Trim())[0];
                re.Operator = "Bgt";
            }
            else if (raw.Raw.Contains("<"))
            {
                var x = Regex.Split(raw.Raw, "<");
                re.Left = IAst.ParseToken(x[0].Trim())[0];
                re.Right = IAst.ParseToken(x[1].Trim())[0];
                re.Operator = "Blt";
            }

            return re;
        }
    }

    public enum LogicOperator
    {
        And,
        Or
    }
}
