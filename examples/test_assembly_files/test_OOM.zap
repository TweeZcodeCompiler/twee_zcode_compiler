.BYTEARRAY MEM [300]
.STRING str "Hallo"

.FUNCT main pointer
call sys_malloc MEM 300 -> pointer
print "Should not been shown."
new_line
quit
