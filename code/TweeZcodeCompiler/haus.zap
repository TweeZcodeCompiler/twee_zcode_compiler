.FUNCT main test=49, bla, a=4, b=5
print_num 47

print "Du bist im Haus_"
print "_Willst du in den Garten? (ja..1/nein...2):"
print "Test"
new_line

je 9 1 0 ?ZERO
print "n"
jump ?WEITER

ZERO:
print "ist null"

WEITER:
print "Willst du in den Garten? (ja..1/nein...2): "
read_char -> test
print_char test
je 49 test ?GARTEN_CALL
new_line
new_line
print "Du bist im Haus geblieben"
new_line

je a b ?~EQUALS
print "nicht gleich"
quit

EQUALS:
print "gleich"
quit

GARTEN_CALL:
new_line
new_line
print "Wir gehen nun in den Garten!"
new_line
call_1n garten
new_line
print "du bist wieder im Haus"
new_line

jl 4 5 ?~LESS
print "nicht weniger"
quit

LESS:
print "weniger"
quit

.FUNCT garten
new_line
print "Du bist im Garten!"
new_line

jg 5 4 ?~GREATER
print "nicht groesser"
ret 0

GREATER:
print "groesser"
ret 0
