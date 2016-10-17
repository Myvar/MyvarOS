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

        public void Emit(List<IAst> ast)
        {

            //write scope
            foreach(var i in ast)
            {
                if(i is ScopeStmt)
                {
                    var x = i as ScopeStmt;
                    WriteString(x.ScopeName); //Write ScopeName

                    break;
                }
            }

            var imports = new List<string>();
            //build imports index
            foreach(var i in ast)
            {
                if(i is ImportStmt)
                {
                    var x = i as ImportStmt;
                    imports.Add(x.ScopeName);
                    break;
                }
            }

            WriteInt(imports.Count);
            
            foreach(var i in imports)
            {
                WriteString(i);
            }

            var typeindex = new List<TypeStmt>();

             //build type index
            foreach(var i in ast)
            {
                if(i is TypeStmt)
                {
                    var x = i as TypeStmt;
                    typeindex.Add(x);
                    break;
                }
            }

            WriteInt(typeindex.Count);


            foreach(var i in typeindex)
            {
                WriteString(i.Name);
                WriteString(i.BaseType);

                var methodIndex = new List<FnStmt>();
                
                foreach(var method in i.Body)
                {
                    if(method is FnStmt)
                    {
                        methodIndex.Add(method as FnStmt);
                    }
                }

                WriteInt(methodIndex.Count);

                //first calulate opcodes count

                foreach(var method in methodIndex)
                {
                    WriteString(method.Name);

                    WriteInt(method.Parameters.Count);
                    foreach(var parm in method.Parameters)
                    {
                        WriteString(parm.Value);
                    }

                    //calualte bytecode count
                    AllowWrite = false;
                    int count = 0;

                    foreach(var stmts in method.Body)
                    {
                        Emit(stmts, ref count);
                    }

                    //Write actuale byte code
                    AllowWrite = true;                    

                    WriteInt(count);
                    count = 0;
                    foreach(var stmts in method.Body)
                    {
                        Emit(stmts, ref count);
                    }
                }

            }
        }

        public void Emit(IAst Opcode, ref int count)
        {
            if(Opcode is InvokeStmt)
            {
                var x = Opcode as InvokeStmt;
                foreach(var i in x.Perams)
                {
                    EmitParameter(i, ref count);
                }

                count++;

                WriteByte(0x30);//call opcode
                WriteString(x.Path);
            }
        }

        public void EmitParameter(IAst Opcode, ref int count)
        {
            if(Opcode is ValueStmt)
            {
                var x = Opcode as ValueStmt;
                if(x.Value.StartsWith("\""))
                {
                    count++;
                    WriteByte(0x20);//loadstr opcode
                    WriteString(x.Value.Trim().Trim('"'));
                    return;
                }
            }
        }

        public string ResolveCall(string s)
        {
            //hardcode for now
            return "[kernel]stdio::" + s;
        }

        public void Write(string file)
        {
            File.WriteAllBytes(file, _Buf.ToArray());
        }

        public bool AllowWrite { get; set; } = true;

        public void WriteString(string s)
        {
            if(!AllowWrite)
            {
                return;
            }

            WriteInt(Encoding.ASCII.GetByteCount(s));
            _Buf.AddRange(Encoding.ASCII.GetBytes(s));
        }

        public void WriteInt(int i)
        {
            if(!AllowWrite)
            {
                return;
            }

            _Buf.AddRange(BitConverter.GetBytes(i));
        }

        public void WriteByte(byte b)
        {
            if(!AllowWrite)
            {
                return;
            }

            _Buf.Add(b);
        }

    }
}
