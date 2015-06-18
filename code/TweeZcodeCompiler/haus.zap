.FUNCT main
new_line
set_text_style b
print "Du bist im Haus"
set_text_style r
new_line
print "Willst du in den Garten? (ja..1/nein...2) "
read_char 1 -> user_input
je 1 user_input ?GARTEN_CALL
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
