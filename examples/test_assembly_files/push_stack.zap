.WORDARRAY MYSTACK [1024]
.FUNCT main
storew MYSTACK 0 1024
push_stack 1 MYSTACK ?success
loadw MYSTACK 0 -> sp
print_num sp
print "failure"
loadw MYSTACK 0 -> sp
print_num sp
quit
success:
print "success!"
loadw MYSTACK 0 -> sp
print_num sp
quit
