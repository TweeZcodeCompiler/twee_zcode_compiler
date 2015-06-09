
WORDS::


GLOBAL::
.GVAR PASSAGE_PTR=0

OBJECT::


IMPURE::
VOCAB::

ENDLOD::

.FUNCT main
START::
    call_vs start -> PASSAGE_PTR
JUMP_TABLE_START:
    jeq 0 PASSAGE_PTR ?L_start
    jeq 1 PASSAGE_PTR ?L_passage1
    jeq 2 PASSAGE_PTR ?L_passage2
L_start:
    call_vs start -> PASSAGE_PTR
    jump JUMP_TABLE_END
L_passage1:
    call_vs passage1 -> PASSAGE_PTR
    jump JUMP_TABLE_END
L_passage2:
    call_vs passage2 -> PASSAGE_PTR
    jump JUMP_TABLE_END
JUMP_TABLE_END:
    jg PASSAGE_PTR -1 ?JUMP_TABLE_START
    quit

; passages
.FUNCT start
    print "Dies ist ein Test"
    new_line
    print "print 1 to exit"
    read_char 1 -> sp
    jeq 49 sp ?~L_2
    ret 2
L_2
    ret 1

.FUNCT passage1
    print "correct"
    new_line
    ret 2

.FUNCT passage2
    print "wrong"
    new_line
    ret -1

.END

