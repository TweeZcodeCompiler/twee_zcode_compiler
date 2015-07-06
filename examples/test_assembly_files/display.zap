.GVAR VAR
.FUNCT main
print "main routine"
read_char 1 -> VAR
je 49 VAR ?EinsCall
new_line
quit
EinsCall:
jump ?Fu
print "lala"
Fu:
print "fufu"
quit
