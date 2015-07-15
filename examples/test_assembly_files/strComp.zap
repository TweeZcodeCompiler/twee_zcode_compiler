.STRING a "Hal12"
.STRING b "Hal"
.STRING c "lo"
.BYTEARRAY mem [1000]

.FUNCT main p1, p2, str1, num
call sys_string mem b -> str1
call sys_number mem 12 -> num
call sys_concat mem str1 num -> p1
call sys_string mem a -> p2
call sys_comp mem p1 a -> sp
new_line
print_num sp
new_line
quit
