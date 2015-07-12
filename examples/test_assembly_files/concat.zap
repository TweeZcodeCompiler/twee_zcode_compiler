.BYTEARRAY buff [300]
.STRING str1 "Deine Eingabe: "
.STRING str2 "du"
.STRING str3 " "

.FUNCT main, a, b, c, d, diff
call sys_string buff str1 -> a
call sys_string buff str3 -> diff
print "Wetten, ich kann mir alle deine Eingaben merken?"
new_line
store d 0
LOOP:
je d 10 ?ENDE
print "Gibt was ein"
new_line
read_char 1 -> sp
call sys_char buff sp -> b
call sys_concat buff a b -> a
call sys_concat buff a diff -> a
add d 1 -> d
jump ?LOOP
ENDE:
call_vn sys_print a
new_line 
quit
