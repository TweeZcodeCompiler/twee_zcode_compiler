;one data set contains:
;	1 byte:		type of stored data
;	2 byte:		size of stored data
;	n bit:		data

;the types are:
;	0:	free
;	1:	reservated
;	2:	number
;	3:	bool
;	4:	string
;	5:	concatinated types

;--------------------------------------------------------------------
;function sys_malloc(mem, size):
;	mem:	byte-adress of an array (or some other memory space)
;	size:	size of the memory to allociate
;	return:	byte adress of the begin of the free memory

.FUNCT sys_malloc mem, size, currentPointer, currentSize, lastPointer
	
	; initial set currentPointer = lastPointer = mem
	; all pointers points to the begin of the memory space
	; size will be calculated with the 3 byte of storeage information
	store currentPointer mem
	store currentSize 0
	store lastPointer currentPointer
	add size 3 -> size 
	
	;increment the currentPointer until currentSize == size
	;or found allready allociated memory
SEARCH:
	loadb currentPointer 0 -> sp
	je sp 0 ?FREE_MEM
	;jump over the allociated memory
	inc currentPointer
	loadw currentPointer 0 -> sp
	add currentPointer sp -> currentPointer
	dec currentPointer
	store lastPointer
	store currentSize 0
	jump ?SEARCH
FREE_MEM:
	inc current Size
	inc currentPointer
	je currentSize size ?FOUND
	jump ?SEARCH
	
	;if the algorithm found a free memory, it will be allociated
FOUND:
	storeb lastPointer 0 1
	inc lastPointer
	storew lastPointer 0 size
	add lastPointer 2 -> lastPointer
	ret lastPointer
	
;--------------------------------------------------------------------
;function sys_free(pointer)
;	pointer:	byte adress

.FUNCT sys_free pointer, size, i

	;calculate the size of the pice of memory
	sub pointer 2 -> pointer
	loadw pointer 0 -> size
	dec pointer
	store i 0
	
	;fill with 0
FREE:
	je i size ?FINISH
	storeb pointer 0 0
	inc pointer
	inc i
	jump ?FREE
FINISH:
	rtrue

;--------------------------------------------------------------------
;function sys_number(mem, initialValue):
;	mem:		byte-adress of an array (or some other memory space)
;	initialValue:	initial value of number
;	return:		byte-adress of the number

.FUNCT sys_number mem, initialValue, pointer
	call sys_malloc mem 2 -> pointer
	sub pointer 3 -> pointer
	storeb pointer 0 2
	add pointer 3 -> pointer
	storew pointer 0 initialValue
	ret pointer
	
;--------------------------------------------------------------------
;function sys_bool(mem, initialValue):
;	mem:		byte-adress of an array (or some other memory space)
;	initialValue:	initial value of bool
;	return:		byte-adress of the bool

.FUNCT sys_bool mem, initialValue, pointer
	call sys_malloc mem 1 -> pointer
	sub pointer 3 -> pointer
	storeb pointer 0 3
	add pointer 3 -> pointer
	storeb pointer 0 initialValue
	ret pointer

;--------------------------------------------------------------------
;function sys_string(mem, initialValue):
;	mem:		byte-adress of an array (or some other memory space)
;	initialValue:	initial value of string (byte-adress)
;	return:		byte-adress of the string

.FUNCT sys_string mem, initialValue, pointer
	call sys_malloc mem 2 -> pointer
	sub pointer 3 -> pointer
	storeb pointer 0 4
	add pointer 3 -> pointer
	storew pointer 0 initialValue
	ret pointer

;--------------------------------------------------------------------
;function sys_concat(mem, head, tail):
;	mem:		byte-adress of an array (or some other memory space)
;	heat:		byte-adress of the first part
;	tail:		byte-adress of the second part
;	return:		byte-adress of the concatination

.FUNCT sys_concat mem, head, tail, pointer
	call sys_malloc mem 4 -> pointer
	sub pointer 3 -> pointer
	storeb pointer 0 5
	add pointer 3 -> pointer
	storew pointer 0 head
	storew pointer 1 tail
	ret pointer

;--------------------------------------------------------------------
;function sys_instanceof(pointer)
;	pointer:	byte-adress of the stored type
;	return:		type code (see top of page)

.FUNCT sys_instanceof pointer
	sub pointer 3 -> pointer
	loadb pointer 0 -> sp
	ret sp

;--------------------------------------------------------------------
;function sys_print(pointer)
;	pointer:	byte-adress of the stored type

.FUNCT sys_print pointer, type

	call sys_instanceof pointer -> type
	je type 2 ?NUMBER
	je type 3 ?BOOL
	je type 4 ?STRING
	je type 5 ?CONCAT
	ret
	
NUMBER:
	loadw pointer 0 -> sp
	print_num sp
	ret
	
BOOL:
	loadb pointer 0 -> sp
	je sp 0 ?FALSE
	print "true"
	ret
	FALSE:
	print "false"
	ret
	
STRING:
	loadw pointer 0 -> sp
	print_addr sp
	ret

CONCAT:
	loadw pointer 1 -> sp
	loadw pointer 0 -> sp
	call sys_print sp
	call sys_print sp
	ret 
