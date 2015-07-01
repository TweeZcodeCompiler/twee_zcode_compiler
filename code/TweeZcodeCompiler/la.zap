.GVAR PASSAGE_PTR
.GVAR USER_INPUT
.FUNCT main 
	call R_start -> PASSAGE_PTR
JUMP_TABLE_START:
	je 0 PASSAGE_PTR ?L_Start
	je 1 PASSAGE_PTR ?L_Garten
L_Start:
	call R_Start -> PASSAGE_PTR
	jump ?JUMP_TABLE_START
L_Garten:
	call R_Garten -> PASSAGE_PTR
	jump ?JUMP_TABLE_START
JUMP_TABLE_END:
	quit
.FUNCT R_Start 
	print "***** Start *****"
	new_line
	print "Welcome in my Twee Story"
	new_line
.GVAR var
	store var 1
	new_line
	new_line
	new_line
	print "Select one of the following options"
	new_line
READ_BEGIN:
	read_char 1 -> USER_INPUT
	jump ?READ_BEGIN
.FUNCT R_Garten 
	print "***** Garten *****"
	new_line
	store var 2
	print "	"
	new_line
	print "	"
	new_line
	new_line
	print "Select one of the following options"
	new_line
READ_BEGIN:
	read_char 1 -> USER_INPUT
	jump ?READ_BEGIN
