.WORDARRAY MYSTACK [1024]
.FUNCT main

; init with size-1 (one word needed for size itself)
storew MYSTACK 0 1023

; push a few values
push_stack 1 MYSTACK ?~failure
call_vs top MYSTACK -> sp
print "top: "
print_num sp
new_line

push_stack 2 MYSTACK ?~failure
call_vs top MYSTACK -> sp
print "top: "
print_num sp
new_line

push_stack 3 MYSTACK ?~failure
call_vs top MYSTACK -> sp
print "top: "
print_num sp
new_line

push_stack 4 MYSTACK ?~failure
call_vs top MYSTACK -> sp
print "top: "
print_num sp
new_line

; pop a few elements, but not all
pop_stack 3 MYSTACK
call_vs top MYSTACK -> sp
print "top: "
print_num sp
new_line

; pop last element
pop_stack 1 MYSTACK

quit


failure:
print "fail.."
quit

.FUNCT top stack
loadw stack 0 -> sp
add sp 1 -> sp
loadw stack sp -> sp
ret sp