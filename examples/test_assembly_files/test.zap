.GVAR PASSAGE_PTR
.FUNCT main
	call R_start -> PASSAGE_PTR
	quit
.FUNCT R_start USER_INPUT
READ_BEGIN:
	read_char 1 -> USER_INPUT
	je 49 USER_INPUT ?L0
	je 50 USER_INPUT ?L1
	jump ?READ_BEGIN
L0:
	ret 1
L1:
	ret 1
print "Fehler"
quit
