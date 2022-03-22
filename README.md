# LC-2K (Little Computer 2000)
### About
This implementation of the LC-2K was created by Tyler Hooper for Computer Architecture in Spring 2021
### Assembler
The assembler program is located at [assembler.c](assembler.c). 

This program takes `<assembly-code-file>` as input, converts it to machine code, and writes to the `<machine-code-file>`
##### Usage
`assembler.exe <assembly-code-file> <machine-code-file>`

Output is directed to `<machine-code-file>`

### Simulator
The simulator program is located at [simulator.c](simulator.c). 

This program takes `<machine-code-file>` as input and simulates the program, printing each state to `output`.

##### Usage
`simulator.exe <machine-code-file> > output`

Results are directed to the `output` file.