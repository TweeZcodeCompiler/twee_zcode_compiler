.BYTEARRAY MEM [300]
.STRING str "Hallo"

.FUNCT main pointer
call sys_number MEM 24 -> pointer
call sys_instanceof pointer 2 -> sp
print_num sp
new_line
call sys_string MEM str -> pointer
call sys_instanceof pointer 2 -> sp
print_num sp
new_line
quit
