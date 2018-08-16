
layout src
layout reg

set disassembly-flavor intel
set architecture i386

target remote localhost:1234

b kernel_main
