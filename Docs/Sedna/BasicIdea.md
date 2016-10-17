Create a compiler that compiles sedna to some kinda of a byte code, 
that the operating system then runs in a vm using JIT, but to get started an interpiter should be fine.

System calls:
insted of using interupts for system calls, i will allow the vm to call something like kernel::putc for eg.
