using Sedna.Core.Internals;
using Sedna.Core.Internals.Ast;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Sedna.Core
{
    public static class Parser
    {
        public static bool FoundScope = false;
        public static List<Error> Errors = new List<Error>();
        public static string ErrorRawSrcRef { get; set; }

        public static List<IAst> Parse(string raw)
        {
            var re = new List<IAst>();
            FoundScope = false;
            var x = Tokenize(raw);

            ErrorRawSrcRef = raw;

            //loop through all the tokens and add it to final ast
            foreach (var i in x)
            {
                re.AddRange(Astyfie(i) /* Astyfie token */);
            }

            return re;
        }

        public static List<IAst> Astyfie(Token c)
        {
            var re = new List<IAst>();
            var x = IAst.ParseToken(c);
            if (x != null)
            {
                re.AddRange(x);
            }
            return re;
        }

        public static List<IAst> ParseExpression(Token c)
        {
            var re = new List<IAst>();

            string buffer = "";

            for (int i = 0; i < c.Raw.Length; i++)
            {
                var a = c.Raw[i];
                if (i < c.Raw.Length - 1)
                {
                    var b = c.Raw[i + 1];
                    if (a == '|' && b == '|')
                    {
                        i += 2;
                        var z = Astyfie(CreateToken(buffer.Trim(), c.Position + i));
                        (z[0] as ExpressionStmt).LogicOperator = LogicOperator.Or;
                        re.AddRange(z);
                        buffer = "";
                    }
                    else if (a == '&' && b == '&')
                    {
                        i += 2;
                        var z = Astyfie(CreateToken(buffer.Trim(), c.Position + i));
                        (z[0] as ExpressionStmt).LogicOperator = LogicOperator.And;
                        re.AddRange(z);
                        buffer = "";
                    }                    
                    else
                    {
                        buffer += a;
                    }
                }
                else
                {
                    buffer += a;
                }

            }

            re.AddRange(Astyfie(CreateToken(buffer.Trim(), c.Position)));
            buffer = "";

            return re;
        }

        public static List<Token> Tokenize(string raw)
        {
            var re = new List<Token>();

            Token CurentToken = new Token();

            string buffer = "";
            bool inblock = false;
            bool instring = false;
            bool incomment = false;
            bool inmulticomment = false;
            int bracketindex = 0;

            for (int i = 0; i < raw.Length; i++)
            {
                var currentchar = raw[i];




                if (currentchar == '"' && !incomment)
                {
                    if (raw[i - 1] != '\\')
                    {
                        if (instring)
                        {
                            instring = false;
                        }
                        else
                        {
                            instring = true;
                        }
                    }
                }

                if (currentchar == '#' && !inblock)
                {
                    incomment = true;
                }

                if (currentchar == '/' && !inblock && raw[i + 1] == '#')
                {
                    incomment = true;
                    inmulticomment = true;
                }

                if (!incomment)
                {
                    if (!instring && currentchar != '\0')
                    {

                        if (currentchar == '\r' || currentchar == '\t')
                        {
                            //egnor it
                        }
                        else
                        {

                            if (!inblock)
                            {
                                if (currentchar != '{' && currentchar != '\n' && currentchar != ';')
                                {
                                    buffer += currentchar;
                                }
                                else
                                {
                                    if (currentchar == '{')
                                    {

                                        if (bracketindex == 0)
                                        {
                                            inblock = true;
                                            CurentToken.RawHeader = buffer.Trim();
                                            CurentToken.Position = i - buffer.Length - 1;
                                            buffer = "";
                                           // buffer += currentchar;
                                        }
                                        bracketindex++;
                                    }
                                    else
                                    {
                                        if (!Seek('{', i, raw))
                                        {
                                            HandleToken(ref re, ref buffer, i);
                                        }
                                    }
                                }
                            }
                            else
                            {
                                if (currentchar == '{')
                                {
                                    bracketindex++;
                                }
                                if (currentchar == '}')
                                {
                                    buffer += currentchar;
                                    bracketindex--;
                                    if (bracketindex == 0)
                                    {
                                        bracketindex = 0;
                                        CurentToken.Body = Tokenize(buffer);
                                        bracketindex = 0;
                                        buffer = "";
                                        inblock = false;

                                        if ((!string.IsNullOrEmpty(CurentToken.Raw) && !string.IsNullOrEmpty(CurentToken.Raw) && CurentToken.Raw.Trim() != "}")
                                            || (!string.IsNullOrEmpty(CurentToken.RawHeader) && !string.IsNullOrEmpty(CurentToken.RawHeader) && CurentToken.RawHeader.Trim() != "}"))
                                        {

                                            re.Add(CurentToken);

                                        }
                                        CurentToken = new Token();
                                    }
                                }
                                else
                                {
                                    buffer += currentchar;
                                }
                            }
                        }
                    }
                    else
                    {

                        buffer += currentchar;
                    }
                }
                else
                {
                    if (currentchar == '\n' && !inmulticomment)
                    {
                        incomment = false;
                        buffer = "";
                    }
                    if (currentchar == '#' && raw[i + 1] == '/')
                    {
                        incomment = false;
                        buffer = "";
                        inmulticomment = false;
                        i += 1;
                    }
                }
            }


            HandleToken(ref re, ref buffer, raw.Count());


            return re;
        }

        private static bool Seek(char c, int start, string raw)
        {
            for (int i = start; i < raw.Length; i++)
            {
                var currentchar = raw[i];
                if ((byte)currentchar >= 32 && !string.IsNullOrWhiteSpace(currentchar.ToString()) && !string.IsNullOrEmpty(currentchar.ToString()) && (byte)currentchar <= 126) // makesure char is not like \r, \n, \t etc
                {
                    if (currentchar == c)
                    {
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }
            }
            return false;
        }

        private static void HandleToken(ref List<Token> re, ref string buffer, int pos)
        {
            if (!string.IsNullOrEmpty(buffer) && !string.IsNullOrWhiteSpace(buffer) && buffer.Trim() != "}")
            {
                re.Add(CreateToken(buffer, pos - buffer.Length - 1));
                buffer = "";
            }

        }

        public static Token CreateToken(string raw, int pos)
        {
            return new Token() { Raw = raw.Trim(), Position = pos };
        }
    }
}
