.FUNCT main
print "Hier ist ein Apfel willst du ihn essen?"
new_line
print "0...Essen"
new_line
print "1...Nicht essen"
new_line
print "Was willst du tun "
read_char 1 -> sp
je 49 sp ?ESSEN
new_line
new_line
print "Ich hab noch immer Hunger"
new_line
new_line
quit
ESSEN:
new_line
new_line
print "Du isst den Apfel"
new_line
print "Mhm lecker"
new_line
new_line
quit
