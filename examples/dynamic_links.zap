;; Disclaimer: This program is not stable. It is unable to handle invalid user input.
;; Furthermore, the approach to use a bitfield-like structure to list all links leads to
;; a different ordering of the links shown than the order of how the links appear in a passage.
;; For this reason, a different approach should be considered.

WORDS::

; each passage has an id. this id is used as an index to this table
PASSAGES:: .TABLE
    .BYTE 0
    .BYTE 0
    .BYTE 0
.ENDT

; this table is used to look up what passage the user input was referring to.
; the user input is used as an index into this table. each value holds the
; id number of the passage the input option referred to. e.g., when a link of the form:
;
;       2: somepassage
;
; is encountered and the user selects 2, 2-1=1 is used as an index into the table,
; which should then hold the id number of "somepassage" 
USERINPUT_LOOKUP:: .TABLE
    .BYTE 0
    .BYTE 0
    .BYTE 0
.ENDT

GLOBAL::
.GVAR PASSAGE_PTR=0
.GVAR USER_INPUT=0
.GVAR PASSAGES_COUNT=3 ; should be the same as the size of the upper two tables

OBJECT::
IMPURE::
VOCAB::
ENDLOD::

; display routines for each passage
.FUNCT start_display
print "start passage links to passage 1"
storeb PASSAGES 1 1 ; stores indicate links
storeb PASSAGES 2 1
new_line
ret 0

.FUNCT passage1_display
print "passage 1 includes passage 2 (e.g. due to display macro from twee source)"
new_line
call_vs passage_by_id 2 -> sp
pop
print "we're back in passage 1"
new_line
ret 0

.FUNCT passage2_display
print "passage 2 links back to start passage"
storeb PASSAGES 0 1
new_line
ret 0



; this routine displays all possible links to the user, interprets his input 
; and returns the id of the selected passage
.FUNCT display_links,i=0,selectcount=0
; iterate through passages-array and display input option for each linked passage
loop_start:
; BEGIN LOOP CONTENTS
loadb PASSAGES i -> sp
jz sp ?passage_not_set ; interpret anything other than 0 as set
storeb USERINPUT_LOOKUP selectcount i
add selectcount 1 -> sp
add selectcount 49 -> sp ; 49 is the offset from character code for 0-9 to the numbers themselves
print_char sp
print ": "
call_vs print_name_for_passage_id i
new_line
add selectcount 1 -> selectcount

passage_not_set:
; END LOOP CONTENTS

; loop condition
add i 1 -> i
jl i PASSAGES_COUNT ?loop_start

read_char 1 -> USER_INPUT
sub USER_INPUT 48 -> sp
sub sp 1 -> sp
; TODO: check if the input value is in range
loadb USERINPUT_LOOKUP sp -> sp 

ret sp


.FUNCT print_name_for_passage_id,id=-1
jeq id 0 ?start
jeq id 1 ?passage1
jeq id 2 ?passage2
print "invalid"
ret 0
start:
print "start"
ret 0
passage1:
print "passage1"
ret 0
passage2:
print "passage2"
ret 0


; calls a passage by its id number
.FUNCT passage_by_id,id=-1
jeq id 0 ?start
jeq id 1 ?passage1
jeq id 2 ?passage2
print "invalid id for passage "
print_num id
new_line
quit
ret 0
start:
call_vs start_display
ret 0
passage1:
call_vs passage1_display
ret 0
passage2:
call_vs passage2_display
ret 0


; tables have to be reset after each run through display_links
.FUNCT reset_tables,i=0
loop_start:
storeb PASSAGES i 0
storeb USERINPUT_LOOKUP i 0
add i 1 -> i
jl i PASSAGES_COUNT ?loop_start
ret 0



.FUNCT main
START::

; call start passage
call_vs passage_by_id 0 

main_loop:
call_vs display_links -> sp
call_vs reset_tables -> sp
pop
call_vs passage_by_id sp -> sp
pop
jump main_loop

quit


.END

