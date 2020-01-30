## Marking Guide for Mini Project 1
**Out:** January 20, 2020    
**Due:** February 3, 2020

This document describes the marking scheme for Mini Project 1.

1. Accurate makefile and no error in compilation (2 marks)
    * In OSC VM, the TA will use `make` command to compile and make the project; if no errors reported and shell prompt 
    shows up properly you will get the 2 marks for this part.

2. Simple shell commands (2 marks)
    * At this step, the TA may run any simple shell command in your shell. It could be w/wo parameters and **is including but not
    limited to** `ls -la`, `cd /etc/bin/`, `pwd`, `exit`, `top &` etc. When running `top &`, the TA will check if
    a top process is running in the background. The TA will use the same set of commands for all students.

3. History feature (2 marks)
    * Here the TA uses `!!` to run the last command. If there is no last command (i.e., right at the beginning), 
    your shell should provide a proper error message.  
    
4. Redirection input and output (2 marks)
    * To test this, the TA issues `ls > out.txt` command. the `out.txt` file should be created in the current 
    directory. Then, `sort < out.txt` will be executed; as a result, a sorted list of files/folders should be printed 
    in the screen.d
    
5. Communication via pipe (2 marks)
    * For this part, the TA will execute `ls | sort` or/and `ps -ef | grep systemd` command; expected results should be printed
    out in the screen.     
