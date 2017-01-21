using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using Newtonsoft.Json;
using Sedna.Core.Internals;
using Sedna.Core.Internals.Ast;

namespace Sedna.Core
{
    public class ByteCode
    {
        List<byte> _Buf = new List<byte>();
        int bc_count = 0;
        List<string> variables = new List<string>();
        private List<Module> Modules { get; set; } = new List<Module>();
        private List<string> Imports { get; set; } = new List<string>();
        private List<string> Methods { get; set; } = new List<string>();
        private Dictionary<string, string> Parameters { get; set; } = new Dictionary<string, string>();



        private void LoadModule(string file)
        {
            Modules.Add(JsonConvert.DeserializeObject<Module>(File.ReadAllText(file)));
        }

        public ByteCode()
        {
            string file = "";

            if (Directory.Exists("./Modules"))
            {
                file = "./Modules";
            }
            else
            {
                file = "../Modules";
            }


            foreach (var i in Directory.GetFiles(file))
            {
                if (i.EndsWith(".json"))
                {
                    LoadModule(i);
                }
            }
        }

        public void Emit(List<IAst> ast)
        {

            //write scope
            foreach (var i in ast)
            {
                if (i is ScopeStmt)
                {
                    var x = i as ScopeStmt;
                    WriteString(x.ScopeName); //Write ScopeName

                    break;
                }
            }


            foreach (var i in ast)
            {
                if (i is ImportStmt)
                {
                    var x = i as ImportStmt;
                    Imports.Add(x.ScopeName);
                }
            }

            WriteInt(Imports.Count);

            foreach (var i in Imports)
            {
                WriteString(i);
            }

            var typeindex = new List<TypeStmt>();

            //build type index
            foreach (var i in ast)
            {
                if (i is TypeStmt)
                {
                    var x = i as TypeStmt;
                    typeindex.Add(x);
                    break;
                }
            }

            WriteInt(typeindex.Count);


            foreach (var i in typeindex)
            {
                WriteString(i.Name);
                WriteString(i.BaseType);

                var methodIndex = new List<FnStmt>();

                foreach (var method in i.Body)
                {
                    if (method is FnStmt)
                    {
                        methodIndex.Add(method as FnStmt);
                        Methods.Add((method as FnStmt).Name);
                    }
                }

                WriteInt(methodIndex.Count);

                //first calulate opcodes count

                foreach (var method in methodIndex)
                {
                    Parameters = method.Parameters;
                    WriteString(method.Name);

                    WriteInt(method.Parameters.Count);
                    foreach (var parm in method.Parameters)
                    {
                        WriteString(parm.Value);
                    }

                    variables = new List<string>();
                    //calualte bytecode count
                    AllowWrite = false;
                    int count = 0;

                    int start_byte_count = bc_count;

                    foreach (var prm in Parameters.Reverse())
                    {
                        //load all the parameters into local variables
                        variables.Add(prm.Key);
                        //register variable in interpiter
                        count++;
                        WriteByte(0x40);
                        WriteInt(variables.IndexOf(prm.Key));                       

                        count++;
                        WriteByte(0x21);
                        WriteInt(variables.IndexOf(prm.Key));

                        //load the value into the local variable
                        count++;
                        WriteByte(0x41);
                    }

                    foreach (var stmts in method.Body)
                    {
                        Emit(stmts, ref count);
                    }

                    //Write actuale byte code
                    AllowWrite = true;

                    // Write the total bytes of "bytecode". This
                    // is to help with reading the module without
                    // parsing/interpreting the bytecode until 
                    // later. It also abstracts away the need for
                    // the loader to understand the bytecode.
                    // -kmcg
                    WriteInt(bc_count - start_byte_count);

                    WriteInt(count);
                    count = 0;

                    foreach (var prm in Parameters.Reverse())
                    {
                        //load all the parameters into local variables
                        variables.Add(prm.Key);
                        //register variable in interpiter
                        count++;
                        WriteByte(0x40);
                        WriteInt(variables.IndexOf(prm.Key));                       

                        count++;
                        WriteByte(0x21);
                        WriteInt(variables.IndexOf(prm.Key));

                        //load the value into the local variable
                        count++;
                        WriteByte(0x41);
                    }

                    foreach (var stmts in method.Body)
                    {
                        Emit(stmts, ref count);
                    }
                }

            }
        }

        public void Emit(IAst Opcode, ref int count)
        {
            if (Opcode is InvokeStmt)
            {
                var x = Opcode as InvokeStmt;
                foreach (var i in x.Perams)
                {
                    EmitParameter(i, ref count);
                }

                count++;

                WriteByte(0x30);//call opcode
                WriteString(ResolveCall(x.Path));
            }

            if (Opcode is DecStmt)
            {
                var x = Opcode as DecStmt;
                //register variable internaly
                variables.Add(x.Name);
                //register variable in interpiter
                count++;
                WriteByte(0x40);
                WriteInt(variables.IndexOf(x.Name));

                EmitParameter(x.Value, ref count);

                count++;
                WriteByte(0x21);
                WriteInt(variables.IndexOf(x.Name));

                //load the value into the local variable
                count++;
                WriteByte(0x41);
            }

            if (Opcode is AssignStmt)
            {
                var x = Opcode as AssignStmt;



                EmitParameter(x.Value, ref count);

                count++;
                WriteByte(0x21);//load int
                WriteInt(variables.IndexOf(x.Name));

                //load the value into the local variable
                count++;
                WriteByte(0x41);
            }
        }

        public void EmitParameter(IAst Opcode, ref int count)
        {
            if (Opcode is ResolveStmt)
            {
                var x = Opcode as ResolveStmt;

                EmitParameter(IAst.ParseToken(new Token() { Raw = x.Segments[0] }, true)[0], ref count);
                for (int i = 0; i < x.Segments.Count; i++)
                {
                    if (x.Segments[i] == "+")
                    {
                        //load a and b onto stack

                        EmitParameter(IAst.ParseToken(new Token() { Raw = x.Segments[i + 1] }, true)[0], ref count);
                        count++;
                        WriteByte(0x50);
                    }
                    if (x.Segments[i] == "-")
                    {
                        //load a and b onto stack                       
                        EmitParameter(IAst.ParseToken(new Token() { Raw = x.Segments[i + 1] }, true)[0], ref count);

                        count++;
                        WriteByte(0x51);
                    }
                    if (x.Segments[i] == "*")
                    {
                        //load a and b onto stack                       
                        EmitParameter(IAst.ParseToken(new Token() { Raw = x.Segments[i + 1] }, true)[0], ref count);

                        count++;
                        WriteByte(0x52);
                    }
                    if (x.Segments[i] == "/")
                    {
                        //load a and b onto stack                        
                        EmitParameter(IAst.ParseToken(new Token() { Raw = x.Segments[i + 1] }, true)[0], ref count);

                        count++;
                        WriteByte(0x53);
                    }
                }

            }
            else if (Opcode is ValueStmt)
            {
                var x = Opcode as ValueStmt;

                if (variables.Contains(x.Value))
                {
                    count++;
                    WriteByte(0x42);//Load the local variable
                    WriteInt(variables.IndexOf(x.Value));
                }
                else
                {
                    if (x.Value.StartsWith("\""))
                    {
                        count++;
                        WriteByte(0x20);//loadstr opcode
                        WriteString(EscapeLiternals(x.Value.Trim().Trim('"')));
                        return;
                    }
                    else if (x.Value.Trim().ToLower().StartsWith("0x"))
                    {
                        count++;
                        WriteByte(0x21);//loadstr opcode
                        WriteInt(Convert.ToInt32(x.Value.Trim(), 16));
                    }
                    else if (char.IsDigit(x.Value.Trim()[0]))
                    {
                        count++;
                        WriteByte(0x21);//loadstr opcode
                        WriteInt(int.Parse(x.Value.Trim()));
                    }
                }
            }
            else
            {
                Emit(Opcode, ref count);
            }
        }

        private string EscapeLiternals(string s)
        {
            return s.Replace("\\n", "\n");
        }

        public string ResolveCall(string s)
        {

            //cheack if it is a local method
            if (Methods.Contains(s))
            {
                return "[this]" + s;
            }

            //try finding using import
            foreach (var i in Modules)
            {
                if (Imports.Contains(i.Name))
                {
                    foreach (var m in i.Methods)
                    {
                        if ((m.NameSpace + "." + m.Class + "." + m.MethodName)
                        == i.Name + "." + s)
                        {
                            var re = "[" + i.Name + "]" +
                            (m.NameSpace.Remove(0, i.Name.Length).Length == 0 ?
                            "" : m.NameSpace.Remove(0, i.Name.Length) + ".") +
                            m.Class + "::" + m.MethodName;

                            return re;
                        }
                    }
                }
            }
            //hardcode for now
            return "[Kernel]stdio::" + s;
        }

        public void Write(string file)
        {
            File.WriteAllBytes(file, _Buf.ToArray());
        }

        public bool AllowWrite { get; set; } = true;

        public void WriteString(string s)
        {
            bc_count += 4 + Encoding.ASCII.GetByteCount(s);

            if (!AllowWrite)
            {
                return;
            }

            WriteInt(Encoding.ASCII.GetByteCount(s));
            _Buf.AddRange(Encoding.ASCII.GetBytes(s));
        }

        public void WriteInt(int i)
        {
            bc_count += 4;

            if (!AllowWrite)
            {
                return;
            }

            _Buf.AddRange(BitConverter.GetBytes(i));
        }

        public void WriteByte(byte b)
        {
            bc_count += 1;

            if (!AllowWrite)
            {
                return;
            }

            _Buf.Add(b);
        }

    }
}
