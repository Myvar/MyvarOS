# Byte Code
this file will outline the bytecode, i will update it as i implment it.

# Data Types
Byte = unsigned byte;
SByte = signed byte;
Short = signed 2 bytes
UShort = unsigned 2 bytes
Int = signed int 4 bytes;
UInt = unsigned int 4 bytes;
Int64 = signed int64 8 bytes;
UInt64 = unsigned int64 8 bytes;
String = Int lenth followed by byte[]; //asci

# File Formant
{
  String Scope;// the namespace of this block
  Int imports; // count of imports
  Imports[]
  {
    String namespace;
  }
  
  Int Clases; //count of classes
  Class[]
  {
    String Name;
    String Bace;
    Opcode[]
    {
      //opcode format depends on the opcode it self
    }
  }
  
}
