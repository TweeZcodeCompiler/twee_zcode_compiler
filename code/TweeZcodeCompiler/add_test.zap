.GVAR A
.GVAR B
.GVAR C
.FUNCT main
print "3+4="
store A 3
store B 4
add A B -> C
print_num C
new_line
print "3-4="
store A 3
store B 4
sub A B -> C
print_num C
new_line
print "3*4="
store A 3
store B 4
mul A B -> C
print_num C
new_line
print "20/4="
store A 20
store B 4
div A B -> C
print_num C
new_line
print "1 && 0="
store A 1
store B 0
and A B -> C
print_num C
new_line
print "0 || 1="
store A 0
store B 1
or A B -> C
print_num C
new_line
quit