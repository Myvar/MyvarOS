using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Sedna.Core.Internals.Ast
{
    public class LoopStmt : IAst
    {
        public LoopType LoopType { get; set; }

        public IAst CountedLoop { get; set; }

        public string VarName { get; set; }
        public IAst CollectionName { get; set; }

        public List<IAst> Body { get; set; } = new List<IAst>();

        public override bool IsValid(Token raw) => raw.RawHeader.Trim().StartsWith("loop", StringComparison.Ordinal);

        public override IAst Parse(Token raw)
        {
            var re = new LoopStmt();

            if (raw.RawHeader.Split(' ').Length == 2)//counted
            {
                if (char.IsDigit(raw.RawHeader.Split(' ').Last()[0]))
                {
                    re.LoopType = LoopType.CountedLoop;
                    re.CountedLoop = IAst.ParseToken(raw.RawHeader.Split(' ').Last())[0];
                }
            }
            else
            {

                if (raw.RawHeader.Trim() == "loop")//infinitloop
                {
                    re.LoopType = LoopType.InfinitLoop;
                }
                else
                {
                    if (raw.RawHeader.Trim().Split(' ')[2] == "to")//ForLoop
                    {
                        re.LoopType = LoopType.ForLoop;
                        var parts = raw.RawHeader.Trim().Split(' ');
                        re.VarName = parts[1];
                        re.CountedLoop = IAst.ParseToken(parts[3])[0];
                        //todo: step
                    }
                    else
                    {
                        if (raw.RawHeader.Trim().Split(' ')[2] == "in")//ForLoop
                        {
                            re.LoopType = LoopType.Foreachloop;
                            var parts = raw.RawHeader.Trim().Split(' ');
                            re.VarName = parts[1];
                            re.CollectionName = IAst.ParseToken(parts[3])[0];
                            
                        }
                    }
                }
            }
           


            foreach (var i in raw.Body)
            {
                re.Body.AddRange(IAst.ParseToken(i));
            }

            return re;
        }
    }

    public enum LoopType
    {        
        InfinitLoop,//loop
        CountedLoop,// loop 5
        ForLoop,//loop i to 5 step 2 or loop i to 5
        WhileLoop,//loop x == 10
        Foreachloop,//loop x in Animals
    }
}
