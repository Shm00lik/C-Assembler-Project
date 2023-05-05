;illegal
prn #-5  j6
stop label

label:

mcr hello
	jmp end
	mov #7, #8
endmcr
hello

not #7
clr #5
jmp label(r1,r2)

r3: not r5
rts: not r6

jmp label(r1 r2)
jmp label(r1,r2  )
jmp label(	r1,r2)
jmp label(r1		,r2)

; hello
lea #56, r1
prn r9


