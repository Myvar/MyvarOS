using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using Sedna.Core.Internals;
using Sedna.Core.Internals.Ast;

namespace Sedna.Core
{
    public class ByteCode
    {
        List<byte> _Buf = new List<byte>();
        int bc_count = 0;
        List<string> variables = new List<string>();

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

            var imports = new List<string>();
            //build imports index
            foreach (var i in ast)
            {
                if (i is ImportStmt)
                {
                    var x = i as ImportStmt;
                    imports.Add(x.ScopeName);
                }
            }

            WriteInt(imports.Count);

            foreach (var i in imports)
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
                    }
                }

                WriteInt(methodIndex.Count);

                //first calulate opcodes count

                foreach (var method in methodIndex)
                {
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

                EmitParameter(new ValueStmt() { Value = x.Segments[0] }, ref count);
                for (int i = 0; i < x.Segments.Count; i++)
                {
                    if (x.Segments[i] == "+")
                    {
                        //load a and b onto stack

                        EmitParameter(new ValueStmt() { Value = x.Segments[i + 1] }, ref count);

                        count++;
                        WriteByte(0x50);
                    }
                    if (x.Segments[i] == "-")
                    {
                        //load a and b onto stack                       
                        EmitParameter(new ValueStmt() { Value = x.Segments[i + 1] }, ref count);

                        count++;
                        WriteByte(0x51);
                    }
                    if (x.Segments[i] == "*")
                    {
                        //load a and b onto stack                       
                        EmitParameter(new ValueStmt() { Value = x.Segments[i + 1] }, ref count);

                        count++;
                        WriteByte(0x52);
                    }
                    if (x.Segments[i] == "/")
                    {
                        //load a and b onto stack                        
                        EmitParameter(new ValueStmt() { Value = x.Segments[i + 1] }, ref count);

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
                    else if(char.IsDigit(x.Value.Trim()[0]))
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
