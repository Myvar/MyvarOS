using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Sedna.Core
{
    public class Error
    {
        public string Message { get; set; }
        public string ErrorCode { get; set; }
        public int Position { get; set; }

        public static int CalulateLine(int pos)
        {
            int re = 1;

            bool inqout = false;

            var val = Parser.ErrorRawSrcRef;

            for (int i = 0; i < val.Length; i++)
            {
                var x = val[i];
                if (x == '"')
                {
                    if (inqout)
                    {
                        inqout = false;
                    }
                    else
                    {
                        inqout = true;
                    }
                }

                if (!inqout)
                {
                    if(i == pos)
                    {
                        break;
                    }
                    else
                    {
                        if(x == '\n')
                        {
                            re++;
                        }
                    }
                }
            }

            return re;
        }

        public static int CalulateLineOffset(int pos)
        {
            int re = 1;
            int realre = 0;
            bool inqout = false;
            var line = CalulateLine(pos);
            for (int i = 0; i < Parser.ErrorRawSrcRef.Length; i++)
            {
                var x = Parser.ErrorRawSrcRef[i];
                if (x == '"')
                {
                    if (inqout)
                    {
                        inqout = false;
                    }
                    else
                    {
                        inqout = true;
                    }
                }

                if (!inqout)
                {
                    if(re >= line)
                    {
                        realre++;
                    }
                    if (i == pos)
                    {
                        break;
                    }
                    else
                    {
                        if (x == '\n')
                        {
                            re++;
                        }
                    }
                }
            }

            return realre;
        }

        public override string ToString()
        {
            return ErrorCode + ": " + Message + " at line: " + CalulateLine(Position) + " Pos: " + CalulateLineOffset(Position);
        }
    }
}