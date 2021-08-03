![img.jpg](img.jpg)

<br>
developer : Han Chan Hee<br>
student id : 20182204<br>
date : 2021-06-22 ~ 2021-06-24<br>
<br>

> Shell command must be lowercase!<br>
> Parameter should be hexadecimal<br>
> If you want alphabet to hexadecimal you can use both upper and lowercase<br>
> Memory address is started from 0x0 to 0xFFFFF<br>
> Assembler can handle Upper Case file name<br>


**compile**<br>
    Enter `make`to compile<br>
<br>

**run**<br>
    Enter the `./20182204.out`<br>
    After using all the command that you want to use<br>
<br>

**delete**<br>
    Enter the `make clean`<br>
    it will delete all the runnable files include obj files<br>
<br>

**command**<br>
    to input prompt `*sicsim>*` enter the command below<br>
    - h[elp] : print all the command you can enter<br>
    - d[ir] : print all the files in current directory<br>
    - q[uit] : Exit the sicsim prompt<br>
    - hi[story] : print all the command that you entered<br>
    - du[mp][start,end] : print memory data from (start) to (end)<br>
    - e[dit]address,value : change the memory (address) to (value)<br>
    - f[ill] start,end,value : choose memory from (start)to (end) and change to (value)<br>
    - reset : reset all memory value to 0<br>
    - opcode mnemonic : print (mnemonic)'s opcode<br>
    - opcodelist : print (opcode hash table) data<br>
    - type file : print current directory's file data<br>
    - assemble file : to assemble file. and create .obj files and .lst files.<br>
    - symbol : print (symbol table) that latest assembled<br>
    - progaddr {address} : when loader or run command is entered choose the start address<br>
    - loader {object filename1} {object filename2} {...} : from filename1 include three .obj files to run linking loader<br>
    - bp {address} : make breakpoint to address position<br>
    - bp clear : delete all the breakpoint<br>
    - bp : print all the breakpoint<br>
    - run : run the program<br>
    <br>
    <br>