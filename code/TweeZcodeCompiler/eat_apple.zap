.FUNCT main
print "Hier ist ein Apfel willst du ihn essen?"
new_line
print "1...Essen"
new_line
print "0...Nicht essen"
new_line
print "Was willst du tun "
read_char 1 -> sp
je 49 sp ?ESSEN
print "Ich hab noch immer hunger"
quit
ESSEN:
print "Du isst den Apfel"
new_line
print "Mhm lecker"
quit
