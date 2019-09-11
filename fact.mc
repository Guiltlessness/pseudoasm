.fact
	popi icx
	pushir icx
	pushi 0
	je fact_end
	pushir icx
	pushir icx
	pushi 1
	subi
	call fact
	muli
	ret
fact_end:
	pushi 1
	ret
.main
	ini
	call fact
	outi
	oendl
	chee
	ret
	exit

