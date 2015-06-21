.GVAR USER_INPUT
.FUNCT main
print "Hier ist ein Apfel willst du ihn essen?"
new_line
print "1...Essen"
new_line
print "0...Nicht essen"
new_line
print "Was willst du tun "
read_char 1 -> USER_INPUT
jg 52 USER_INPUT ?L0
new_line
print "Deine Aktion wird ausgefuehrt ..."
new_line
quit
L0:
print "huhu"
quit
