![img.jpg](img.jpg)

<br>
developer : Han Chan Hee<br>
student id : 20182204<br>
date : 2021-06-22 ~ 2021-06-24<br>
<br>

> Shell command must be lowercase!<br>
> parameter should be hexadecimal<br>
> if you want alphabet to hexadecimal you can use both upper and lowercase<br>
> memory address is started from 0x0 to 0xFFFFF<br>
> Assembler can handle Upper Case file name<br>


**compile**<br>
    Enter `make`to compile<br>
<br>

**run**
    Enter the `./20182204.out`
    After using all the command that you want to use

**delete**
    Enter the `make clean`
    it will delete all the runnable files include obj files

**command**
    to input prompt `*sicsim>*` enter the command below
    - h[elp] : print all the command you can enter
    - d[ir] : print all the files in current directory
    - q[uit] : Exit the sicsim prompt
    - hi[story] : print all the command that you entered
    - du[mp][start,end] : print memory data from (start) to (end)
    - e[dit]address,value : change the memory (address) to (value)
    - f[ill] start,end,value : choose memory from (start)to (end) and change to (value)
    - reset : reset all memory value to 0
    - opcode mnemonic : print (mnemonic)'s opcode
    - opcodelist : print (opcode hash table) data
    - type file : print current directory's file data
    - assemble file : to assemble file. and create .obj files and .lst files.
    - symbol : print (symbol table) that latest assembled
    - progaddr {address} : when loader or run command is entered choose the start address
    - loader {object filename1} {object filename2} {...} : from filename1 include three .obj files to run linking loader
    - bp {address} : make breakpoint to address position
    - bp clear : delete all the breakpoint
    - bp : print all the breakpoint
    - run : run the program