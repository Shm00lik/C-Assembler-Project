L1: .data 7, -24
mcr m1
	bne L1(r4,r3)
	sub r1,r4
	bne END
endmcr

m1

END:stop
