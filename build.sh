#!/bin/bash

./mcc $1;
./allc_reg code.e > code.t;
/home/wusj/riscv/riscv < code.t > code.s;
/home/wusj/riscv/bin/riscv64-unknown-elf-gcc -o code.o -c code.s -lm;
/home/wusj/riscv/bin/riscv64-unknown-elf-gcc -o code code.o lib.o -lm;
/home/wusj/riscv/bin/spike pk code;

