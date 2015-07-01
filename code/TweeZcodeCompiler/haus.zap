.GVAR PASSAGE_PTR
.FUNCT main
	call R_start -> PASSAGE_PTR
JUMP_TABLE_START:
	je 0 PASSAGE_PTR ?L_start
	je 1 PASSAGE_PTR ?L_land_of_the_iguanas
	je 2 PASSAGE_PTR ?L_kinds
	je 3 PASSAGE_PTR ?L_favouriteFood
	je 4 PASSAGE_PTR ?L_juveniles
	je 5 PASSAGE_PTR ?L_Disclaimer
L_start:
	call R_start -> PASSAGE_PTR
	jump ?JUMP_TABLE_START
L_land_of_the_iguanas:
	call R_land_of_the_iguanas -> PASSAGE_PTR
	jump ?JUMP_TABLE_START
L_kinds:
	call R_kinds -> PASSAGE_PTR
	jump ?JUMP_TABLE_START
L_favouriteFood:
	call R_favouriteFood -> PASSAGE_PTR
	jump ?JUMP_TABLE_START
L_juveniles:
	call R_juveniles -> PASSAGE_PTR
	jump ?JUMP_TABLE_START
L_Disclaimer:
	call R_Disclaimer -> PASSAGE_PTR
	jump ?JUMP_TABLE_START
JUMP_TABLE_END:
	quit
.FUNCT R_start USER_INPUT
	print "***** start *****"
	new_line
	print "Welcome Adventurer!"
	print "Welcome "
	print "to"
	set_text_style r
	print "to"
	set_text_style r
	print " the "
	print " - a place full of myths and adventure!"
	print "Let's not waste any time and get going! "
	new_line
	print "Select one of the following options"
	new_line
	print "    1) land of the iguanas"
	new_line
	print "    2) land of the iguanas"
	new_line
READ_BEGIN:
	read_char 1 -> USER_INPUT
	je 49 USER_INPUT ?L0
	je 50 USER_INPUT ?L1
	jump ?READ_BEGIN
L0:
	ret 1
L1:
	ret 1
.FUNCT R_land_of_the_iguanas USER_INPUT
	print "***** land of the iguanas *****"
	new_line
	print "The land of the iguanas ... a old and forgotten world inhabited by the mighty"
	print "iguanas. There are many "
	print " of iguanas - huge yet peaceful lizards."
	print "They primarily live of all kinds of fruits, vegetables and other greens."
	print "But when they are "
	print " they also eat insects."
	new_line
	print "Select one of the following options"
	new_line
	print "    1) kinds"
	new_line
	print "    2) juveniles"
	new_line
READ_BEGIN:
	read_char 1 -> USER_INPUT
	je 49 USER_INPUT ?L0
	je 50 USER_INPUT ?L1
	jump ?READ_BEGIN
L0:
	ret 2
L1:
	ret 4
.FUNCT R_kinds USER_INPUT
	print "***** kinds *****"
	new_line
	print "There are green iguanas, rhino iguanas and many more"
	new_line
	print "Select one of the following options"
	new_line
	print "    1) land of the iguanas"
	new_line
READ_BEGIN:
	read_char 1 -> USER_INPUT
	je 49 USER_INPUT ?L0
	jump ?READ_BEGIN
L0:
	ret 1
.FUNCT R_favouriteFood USER_INPUT
	print "***** favouriteFood *****"
	new_line
	print "Iguanas love bananas - it's like chocolate to them."
	new_line
	print "Select one of the following options"
	new_line
	print "    1) land of the iguanas"
	new_line
READ_BEGIN:
	read_char 1 -> USER_INPUT
	je 49 USER_INPUT ?L0
	jump ?READ_BEGIN
L0:
	ret 1
.FUNCT R_juveniles USER_INPUT
	print "***** juveniles *****"
	new_line
	print "Juvenile iguanas will eventually hit puberty and just as human juveniles they will get moody ..."
	new_line
	print "Select one of the following options"
	new_line
	print "    1) land of the iguanas"
	new_line
READ_BEGIN:
	read_char 1 -> USER_INPUT
	je 49 USER_INPUT ?L0
	jump ?READ_BEGIN
L0:
	ret 1
.FUNCT R_Disclaimer USER_INPUT
	print "***** Disclaimer *****"
	new_line
	print "The story is made up ... sort off - I really like iguanas."
	print "Great reptiles."
	print "I'll try to remain in terms of details close to reality but"
	print "might add some spice."
	new_line
	print "Select one of the following options"
	new_line
	print "    1) land of the iguanas"
	new_line
READ_BEGIN:
	read_char 1 -> USER_INPUT
	je 49 USER_INPUT ?L0
	jump ?READ_BEGIN
L0:
	ret 1
