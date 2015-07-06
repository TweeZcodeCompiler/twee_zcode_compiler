.GVAR USER_INPUT
.FUNCT main
print "Du bist in Berlin"
new_line
new_line
print "Moechtest du zum Pariser Platz gehen? (1=Ja 0=Nein):"
read_char 1 -> USER_INPUT
je 49 USER_INPUT ?GO_TO_PARISER_PLATZ 
new_line
new_line
quit	
GO_TO_PARISER_PLATZ:
call_1n pariserplatz
new_line
quit

.FUNCT pariserplatz
new_line
print "Du stehst vor dem Brandenburger Tor"
new_line 
img "img/Berlin.jpg"
new_line
new_line
new_line
quit
