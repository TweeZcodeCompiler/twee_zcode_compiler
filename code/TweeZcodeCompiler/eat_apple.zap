.FUNCT main
print "Hier ist ein Apfel willst du ihn essen?"
new_line
print "1...Essen"
new_line
print "0...Nicht essen"
new_line
print "Was willst du tun "
read_char 1 -> sp
je 49 ?ESSEN
new_line
print "Du isst den Apfel"
new_line
print "Mhm lecker"
new_line
quit
ESSEN:
new_line
print "Ich hab noch immer hunger"
new_line
quit
