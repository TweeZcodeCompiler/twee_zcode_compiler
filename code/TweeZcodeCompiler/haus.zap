.FUNCT main test=2, bla, x=300, y=-1, var2
new_line
print "Du bist im Haus"
new_line
print "Willst du in den Garten? (ja..1/nein...2): "
read_char -> test
je 1 test ?GARTEN_CALL
new_line
new_line
print "Du bist im Haus geblieben"
new_line
quit
GARTEN_CALL:
new_line
new_line
print "Wir gehen nun in den Garten!"
new_line
call garten
new_line
print "du bist wieder im Haus"
new_line
quit

.FUNCT garten
new_line
print "Du bist im Garten!"
new_line
ret
