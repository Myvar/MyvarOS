# Byte Code
this file will outline the bytecode, i will update it as i implment it.

# Virtual machien
The language Is Stack baced so prameters are on stack

Create a VM for every Scope, and load modules into it so when call opcode:

for eg.

[NotePad]Program::Main
will make a call to the kernel NotePad VM at module Program it will call the method main.

Note the for eg. [Kernel]Terminal::puts
will call the Kernel vm witch will not be a VM but rather a plug and this is how system calls will be executed.

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
```
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
    
    Int Methods;
    Methods[]
    {    
      String Name;
      Int Parameters;
      Parameters[] { String type }
      Int Opcodes;
      Opcode[]
      {
        //opcode format depends on the opcode it self
      }
    }
  }
  
}
```

# Opcodes

### LoadStr

Loads a string on the stack

ID: 0x20

Parameter 1: Stiring

### LoadInt

Loads an int onto the stack

ID: 0x21

Parameter 1: Int;

### Call

Loads a string on the stack

ID: 0x30

Parameter 1: Stiring

### Dec Variable

ID: 0x40

declears a variable

Parameter 1: int

### SetLocalVariable

ID: 0x41

Will pop last value on stack and save it in local variable

### LoadLocalVariable

ID: 0x42

Will take parm 1 as var index and loads its value onto the stack


### Add

ID: 0x50

adds the two items on the stack together

### Sub

ID: 0x51

adds the two items on the stack together

### Mul

ID: 0x52

adds the two items on the stack together

### Dev

ID: 0x53

adds the two items on the stack together