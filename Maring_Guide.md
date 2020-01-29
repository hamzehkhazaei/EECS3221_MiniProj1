## Marking Guide for Mini Project 1
**Out:** January 20, 2020    
**Due:** February 3, 2020

This document describes the marking scheme for Mini Project 1.

1. Accurate makefile and no error in compilation 
    * The TA will use `mysh:~$ make` command to compile and make the project; if no errors reported and shell prompt 
    shows up properly you will get the 2 mark for first part.

2. Simple shell commands
    * At this step the TA runs multiple simple commands. It could be any simple command w/wo parameters **including but not
    limited to** `ls -la`, `cd /etc/bin/`, `pwd`, `exit`, `top &` etc. When running `top &`, the TA will check if
    a top process is running in background. The TA will use the same set of commands for all students.

3. History feature
    * Here the TA uses `!!` to run the last command. If there is no last command, it should provide proper error message.
    
4. Redirection input and output
    * In order to test this, the TA issues `ls > out.txt` command. the `out.txt` file should be created in the current 
    directory. Then, `sort < out.txt` will be executed; as the result the list of files should be printed in order 
    (i.e., sorted) in the screen.
    
5. Communication via pipe
    * For this part, the TA will execute `ls | sort` or `ps -ef | grep systemd` command; expected results should be printed
    out in the screen.     
