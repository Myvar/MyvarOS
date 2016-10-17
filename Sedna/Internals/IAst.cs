using Sedna.Core.Internals.Ast;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Sedna.Core.Internals
{
    public abstract class IAst
    {
        public static List<IAst> Buffer { get; set; } = new List<IAst>();

        public static List<IAst> AstIndex { get; set; } = new List<IAst>()
        {
            new ImportStmt(),
            new RetStmt(),
            new ScopeStmt(),
            new AttributeStmt(),
            new TypeStmt(),
            new FnStmt(),
            new DecStmt(),
            new LoopStmt(),
            new WhenStmt(),
            new InvokeStmt(),
            new ExpressionStmt(),

            /* Must always be last becuse isvalid logic is a littlbit fuzzy */
            
        };

        public static List<IAst> ParseToken(Token t)
        {
            Buffer.Clear();
            
            foreach (var i in AstIndex)
            {
                if (i.IsValid(t))
                {
                    var x = new List<IAst>() { i.Parse(t) };
                    x.AddRange(Buffer);
                    foreach(var z in x)
                    {
                        z.Raw = t;
                    }
                    return x;
                }
            }
            if (string.IsNullOrEmpty(t.Raw) || string.IsNullOrWhiteSpace(t.Raw))
            {
                return null;
            }

            return new List<IAst>() { new ValueStmt().Parse(t) };

        }

        public static List<IAst> ParseToken(string t)
        {
            var re = new List<IAst>() { };
            re.AddRange(ParseToken(new Token() { Raw = t }));
            return re;
        }

        public Token Raw { get; set; }

        public abstract bool IsValid(Token raw);
        public abstract IAst Parse(Token raw);
    }
}
