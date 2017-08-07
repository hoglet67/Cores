	code
_BinaryMergeTest:
	# 	return a + b + 1;
	      	add  	r8,r9,0
	      	add  	r8,r0,1
	      	mov  	r1,r8,0
BinaryMergeTest_4:
	      	mov  	r15,r13,0
	rodata
	extern	_BinaryMergeTest
#	code
_ZBBAA_ClassTest:
	      	   push 	r12
	      	   mov  	r12,r14
	      	   dec  	r14,2
	      	   push 	r2
	      	   mov  	r1,r12,-1
	      	   mov  	r2,r1
	# 	a.a = a.a + b.a;
	      	   ld   	r1,r2,1
	      	   mov  	r2,r12,-2
	      	   ld   	r2,r2,1
	      	   add  	r1,r2
	      	   sto  	r1,r2,1
	      	   pop  	r2
	      	   mov  	r14,r12
	      	   pop  	r12
	      	   mov  	r15,r13




_ZBBAAMBAA_AddQAAAQAAA:
	      	   push 	r13
	      	   push 	r12
	      	   mov  	r12,r14
	# 	return a + b + c + d;
	      	   ld   	r5,r11,1
	      	   ld   	r6,r11,2
	      	   add  	r5,r6
	      	   ld   	r6,r12,2
	      	   add  	r5,r6
	      	   ld   	r6,r12,3
	      	   add  	r1,r6
	      	   mov  	r14,r12
	      	   pop  	r13
	      	   pop  	r12
	      	   mov  	r15,r13
#	rodata
#	global	_ClassTest
#	extern	_Add
	bss
_nums:
	fill.w	15,0x00

	code
_main:
	      	   push 	r13
	      	   push 	r12
	      	   mov  	r12,r14
	      	   sub  	r14,r0,4
	# 	c1 = 0;
	      	   sto  	r0,r12,-2
	# 	c2 = 1;
	      	   mov  	r5,r0,1
	      	   sto  	r5,r12,-3
	# 	for (n = 0; n < 23; n = n + 1) {
	      	   sto  	r0,r12,-4
Fibonnaci_4:
	      	   ld   	r5,r12,-4
	      	   cmp  	r5,r0,23
	      	pl.mov  	r15,r0,Fibonnaci_5
	# 		if (n < 1) {
	      	   ld   	r5,r12,-4
	      	   cmp  	r5,r0,1
	      	pl.inc  	r15,Fibonnaci_8-PC
	# 			nums[0] = 1;
	      	   mov  	r5,r0,1
	      	   sto  	r5,r0,_nums
	# 			c = 1;
	      	   mov  	r5,r0,1
	      	   sto  	r5,r12,-1
	      	   mov  	r15,r0,Fibonnaci_9
Fibonnaci_8:
	# 			nums[n] = c;
	      	   ld   	r5,r12,-4
	      	   ld   	r6,r12,-1
	      	   sto  	r6,r5,_nums
	# 			c = c1 + c2;
	      	   ld   	r5,r12,-2
	      	   ld   	r6,r12,-3
	      	   add  	r5,r6
	      	   sto  	r5,r12,-1
	# 			c1 = c2;
	      	   ld   	r5,r12,-3
	      	   sto  	r5,r12,-2
	# 			c2 = c;
	      	   ld   	r5,r12,-1
	      	   sto  	r5,r12,-3
Fibonnaci_9:
	      	   ld   	r5,r12,-4
	      	   add  	r5,r0,1
	      	   sto  	r5,r12,-4
	      	   mov  	r15,r0,Fibonnaci_4
Fibonnaci_5:
	      	   mov  	r14,r12
	      	   pop  	r13
	      	   pop  	r12
	      	   mov  	r15,r13




	rodata
;	global	_main
;	global	_nums
#	code
_main:
	      	   push 	r13
	      	   push 	r12
	      	   mov  	r12,r14
	      	   dec  	r14,1
	# 	for (x = 0; x < 10; x++)  {
	      	   sto  	r0,r12,-1
Test1_8:
	      	   ld   	r5,r12,-1
	      	   cmp  	r5,r0,10
	      	mi.inc  	r15,Test1_9-PC
	# 		printf("Hello World!");
	      	   mov  	r5,r0,Test1_1
	      	   push 	r5
	      	   jsr  	r13,r0,_printf
	      	   inc  	r14,1
	      	   ld   	r5,r12,-1
	      	   inc  	r5,1
	      	   sto  	r5,r12,-1
	      	   mov  	r15,r0,Test1_8
Test1_9:
	# 	switch(argc) {
	      	   ld   	r5,r12,2
	# 	case 1:	printf("One"); break;
	      	   cmp  	r6,r5,1
	      	 z.inc  	r15,Test1_16-PC
	# 	case 2:	printf("Two"); break;
	      	   cmp  	r6,r5,2
	      	 z.mov  	r15,r0,Test1_17
	# 	case 3:	printf("Three"); break;
	      	   cmp  	r6,r5,3
	      	 z.mov  	r15,r0,Test1_18
	      	   mov  	r15,r0,Test1_11
Test1_16:
	      	   mov  	r5,r0,Test1_2
	      	   push 	r5
	      	   jsr  	r13,r0,_printf
	      	   inc  	r14,1
	      	   mov  	r15,r0,Test1_11
Test1_17:
	      	   mov  	r5,r0,Test1_3
	      	   push 	r5
	      	   jsr  	r13,r0,_printf
	      	   inc  	r14,1
	      	   inc  	r15,Test1_11-PC
Test1_18:
	      	   mov  	r5,r0,Test1_4
	      	   push 	r5
	      	   jsr  	r13,r0,_printf
	      	   inc  	r14,1
Test1_11:
	# 	exit(0);
	      	   push 	r0
	      	   jsr  	r13,r0,_exit
	      	   inc  	r14,1
	      	   mov  	r14,r12
	      	   pop  	r13
	      	   pop  	r12
	      	   mov  	r15,r13
#	rodata
	align	2
Test1_4:	# Three
	word	84,104,114,101,101,0
Test1_3:	# Two
	word	84,119,111,0
Test1_2:	# One
	word	79,110,101,0
Test1_1:	# Hello World!
	word	72,101,108,108,111,32,87,111
	word	114,108,100,33,0
#	extern	_main
#	extern	_exit
#	extern	_printf
#	bss
_ary:
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0
	word	0

#	code
_main:
	      	   push 	r13
	      	   push 	r12
	      	   mov  	r12,r14
	      	   dec  	r14,3
	# 	for (y = 0; y < argc; y++) {
	      	   sto  	r0,r12,-2
Test2_8:
	      	   ld   	r5,r12,-2
	      	   ld   	r6,r12,2
	      	   cmp  	r5,r6
	      	mi.mov  	r15,r0,Test2_9
	# 		for (z = 0; z < 45; z++)
	      	   sto  	r0,r12,-3
Test2_11:
	      	   ld   	r5,r12,-3
	      	   cmp  	r5,r0,45
	      	mi.mov  	r15,r0,Test2_12
	# 			ary[y][z] = rand();
	      	   ld   	r5,r12,-3
	      	   ld   	r6,r12,-2
	      	   mov  	r7,r0,45
	      	   mov  	r1,r6
	      	   mov  	r2,r7
	      	   jsr  	r13,r0,_mulu
	      	   mov  	r6,r1
	      	   inc  	r6,_ary
	      	   mov  	r7,r5
	      	   add  	r5,r6
	      	   push 	r5
	      	   jsr  	r13,r0,_rand
	      	   pop  	r5
	      	   sto  	r1,r5,0
	      	   ld   	r5,r12,-3
	      	   inc  	r5,1
	      	   sto  	r5,r12,-3
	      	   mov  	r15,r0,Test2_11
Test2_12:
	      	   ld   	r5,r12,-2
	      	   inc  	r5,1
	      	   sto  	r5,r12,-2
	      	   mov  	r15,r0,Test2_8
Test2_9:
	# 	for (x = 0; x < 10; x++)  {
	      	   sto  	r0,r12,-1
Test2_14:
	      	   ld   	r5,r12,-1
	      	   cmp  	r5,r0,10
	      	mi.inc  	r15,Test2_15-PC
	# 		printf("Hello World!");
	      	   mov  	r5,r0,Test2_1
	      	   push 	r5
	      	   jsr  	r13,r0,_printf
	      	   inc  	r14,1
	      	   ld   	r5,r12,-1
	      	   inc  	r5,1
	      	   sto  	r5,r12,-1
	      	   mov  	r15,r0,Test2_14
Test2_15:
	# 	naked switch(argc) {
	      	   ld   	r5,r12,2
	# 	case 1:	printf("One"); break;
	      	   cmp  	r6,r5,1
	      	 z.inc  	r15,Test2_22-PC
	# 	case 2:	printf("Two"); break;
	      	   cmp  	r6,r5,2
	      	 z.mov  	r15,r0,Test2_23
	# 	case 3:	printf("Three"); break;
	      	   cmp  	r6,r5,3
	      	 z.mov  	r15,r0,Test2_24
	      	   mov  	r15,r0,Test2_17
Test2_22:
	      	   mov  	r5,r0,Test2_2
	      	   push 	r5
	      	   jsr  	r13,r0,_printf
	      	   inc  	r14,1
	      	   mov  	r15,r0,Test2_17
Test2_23:
	      	   mov  	r5,r0,Test2_3
	      	   push 	r5
	      	   jsr  	r13,r0,_printf
	      	   inc  	r14,1
	      	   inc  	r15,Test2_17-PC
Test2_24:
	      	   mov  	r5,r0,Test2_4
	      	   push 	r5
	      	   jsr  	r13,r0,_printf
	      	   inc  	r14,1
Test2_17:
	# 	exit(0);
	      	   push 	r0
	      	   jsr  	r13,r0,_exit
	      	   inc  	r14,1
	      	   mov  	r14,r12
	      	   pop  	r13
	      	   pop  	r12
	      	   mov  	r15,r13
#	rodata
	align	2
Test2_4:	# Three
	word	84,104,114,101,101,0
Test2_3:	# Two
	word	84,119,111,0
Test2_2:	# One
	word	79,110,101,0
Test2_1:	# Hello World!
	word	72,101,108,108,111,32,87,111
	word	114,108,100,33,0
#	extern	_main
#	extern	_rand
#	extern	_exit
#	global	_ary
#	extern	_printf
#	code
_abs:
	# 	return a < 0 ? -a : a;
	      	   cmp  	r8,r0
	      	mi.inc  	r15,TestAbs_4-PC
	      	   not  	r1,r8,-1
	      	   inc  	r15,TestAbs_5-PC
TestAbs_4:
	      	   mov  	r1,r8
TestAbs_5:
	      	   mov  	r15,r13
_min:
	# 	return a < b ? a : b;
	      	   cmp  	r8,r9
	      	mi.inc  	r15,TestAbs_10-PC
	      	   mov  	r1,r8
	      	   inc  	r15,TestAbs_11-PC
TestAbs_10:
	      	   mov  	r1,r9
TestAbs_11:
	      	   mov  	r15,r13
_max:
	# 	return a > b ? a : b;
	      	   cmp  	r8,r9
	      	 z.inc  	r15,TestAbs_18-PC
	      	   cmp  	r8,r9
	      	pl.inc  	r15,TestAbs_16-PC
TestAbs_18:
	      	   mov  	r1,r8
	      	   inc  	r15,TestAbs_17-PC
TestAbs_16:
	      	   mov  	r1,r9
TestAbs_17:
	      	   mov  	r15,r13
_minu:
	# 	return a < b ? a : b;
	      	   cmp  	r8,r9
	      	 c.inc  	r15,TestAbs_23-PC
	      	   mov  	r1,r8
	      	   inc  	r15,TestAbs_24-PC
TestAbs_23:
	      	   mov  	r1,r9
TestAbs_24:
	      	   mov  	r15,r13
#	rodata
#	extern	_minu
#	extern	_abs
#	extern	_min
#	extern	_max
#	code
_TestArrayAssign:
	      	   push 	r12
	      	   mov  	r12,r14
	      	   sub  	r14,r0,53
	# 	y[0] = 1;
	      	   mov  	r1,r0,1
	      	   sto  	r1,r12,-23
	# 	y[1] = 2;
	      	   mov  	r1,r12,-23
	      	   mov  	r2,r0,2
	      	   sto  	r2,r1,1
	# 	y[2] = 2;
	      	   mov  	r1,r12,-23
	      	   mov  	r2,r0,2
	      	   sto  	r2,r1,2
	# 	y[3] = 2;
	      	   mov  	r1,r12,-23
	      	   mov  	r2,r0,2
	      	   sto  	r2,r1,3
	# 	y[4] = 2;
	      	   mov  	r1,r12,-23
	      	   mov  	r2,r0,2
	      	   sto  	r2,r1,4
	# 	y[5] = 2;
	      	   mov  	r1,r12,-23
	      	   mov  	r2,r0,2
	      	   sto  	r2,r1,5
	# 	y[6] = 2;
	      	   mov  	r1,r12,-23
	      	   mov  	r2,r0,2
	      	   sto  	r2,r1,6
	# 	y[7] = 2;
	      	   mov  	r1,r12,-23
	      	   mov  	r2,r0,2
	      	   sto  	r2,r1,7
	# 	y = {1,2,3,4,5,6,7,8};
	      	   mov  	r1,r12,-23
	# 	x[0][0] = 1;
	      	   mov  	r1,r0,1
	      	   sto  	r1,r12,-15
	# 	x[0][1] = 2;
	      	   mov  	r1,r12,-15
	      	   mov  	r2,r0,2
	      	   sto  	r2,r1,1
	# 	x[0][2] = 2;
	      	   mov  	r1,r12,-15
	      	   mov  	r2,r0,2
	      	   sto  	r2,r1,2
	# 	x[0][3] = 2;
	      	   mov  	r1,r12,-15
	      	   mov  	r2,r0,2
	      	   sto  	r2,r1,3
	# 	x[0][4] = 2;
	      	   mov  	r1,r12,-15
	      	   mov  	r2,r0,2
	      	   sto  	r2,r1,4
	# 	x[1][0] = 3;
	      	   mov  	r1,r12,-15
	      	   mov  	r2,r0,3
	      	   sto  	r2,r1,5
	# 	x[1][1] = 3;
	      	   mov  	r1,r12,-15
	      	   mov  	r2,r0,3
	      	   sto  	r2,r1,6
	# 	x[1][2] = 3;
	      	   mov  	r1,r12,-15
	      	   mov  	r2,r0,3
	      	   sto  	r2,r1,7
	# 	x[1][3] = 3;
	      	   mov  	r1,r12,-15
	      	   mov  	r2,r0,3
	      	   sto  	r2,r1,8
	# 	x[1][4] = 5;
	      	   mov  	r1,r12,-15
	      	   mov  	r2,r0,5
	      	   sto  	r2,r1,9
	# 	x[2][0] = 5;
	      	   mov  	r1,r12,-15
	      	   mov  	r2,r0,5
	      	   sto  	r2,r1,10
	# 	x[2][1] = 5;
	      	   mov  	r1,r12,-15
	      	   mov  	r2,r0,5
	      	   sto  	r2,r1,11
	# 	x[2][2] = 5;
	      	   mov  	r1,r12,-15
	      	   mov  	r2,r0,5
	      	   sto  	r2,r1,12
	# 	x[2][3] = 5;
	      	   mov  	r1,r12,-15
	      	   mov  	r2,r0,5
	      	   sto  	r2,r1,13
	# 	x[2][4] = 5;
	      	   mov  	r1,r12,-15
	      	   mov  	r2,r0,5
	      	   sto  	r2,r1,14
	# 	z[0][0][0] = 1;
	      	   mov  	r1,r0,1
	      	   sto  	r1,r12,-53
	# 	z[0][0][1] = 1;
	      	   mov  	r1,r12,-53
	      	   mov  	r2,r0,1
	      	   sto  	r2,r1,1
	# 	z[0][0][2] = 1;
	      	   mov  	r1,r12,-53
	      	   mov  	r2,r0,1
	      	   sto  	r2,r1,2
	# 	z[0][0][3] = 1;
	      	   mov  	r1,r12,-53
	      	   mov  	r2,r0,1
	      	   sto  	r2,r1,3
	# 	z[0][0][4] = 1;
	      	   mov  	r1,r12,-53
	      	   mov  	r2,r0,1
	      	   sto  	r2,r1,4
	# 	z[0][1][0] = 2;
	      	   mov  	r1,r12,-53
	      	   mov  	r2,r0,2
	      	   sto  	r2,r1,5
	# 	z[0][1][1] = 2;
	      	   mov  	r1,r12,-53
	      	   mov  	r2,r0,2
	      	   sto  	r2,r1,6
	# 	z[0][1][2] = 2;
	      	   mov  	r1,r12,-53
	      	   mov  	r2,r0,2
	      	   sto  	r2,r1,7
	# 	z[0][1][3] = 2;
	      	   mov  	r1,r12,-53
	      	   mov  	r2,r0,2
	      	   sto  	r2,r1,8
	# 	z[0][1][4] = 2;
	      	   mov  	r1,r12,-53
	      	   mov  	r2,r0,2
	      	   sto  	r2,r1,9
	# 	z[0][2][0] = 2;
	      	   mov  	r1,r12,-53
	      	   mov  	r2,r0,2
	      	   sto  	r2,r1,10
	      	   mov  	r14,r12
	      	   pop  	r12
	      	   mov  	r15,r13




_TestArrayAssign2:
	      	   push 	r13
	      	   push 	r12
	      	   mov  	r12,r14
	      	   dec  	r14,15
	# 	x[2] = {10,9,8,7,6};
	      	   mov  	r5,r0,10
	      	   mov  	r6,r12,-15
	      	   add  	r5,r6
	      	   mov  	r6,r5
	      	   mov  	r7,r0,10
	      	   sto  	r7,r6,10
	      	   mov  	r7,r0,9
	      	   sto  	r7,r6,11
	      	   mov  	r7,r0,8
	      	   sto  	r7,r6,12
	      	   mov  	r7,r0,7
	      	   sto  	r7,r6,13
	      	   mov  	r7,r0,6
	      	   sto  	r7,r6,14
	      	   mov  	r14,r12
	      	   pop  	r13
	      	   pop  	r12
	      	   mov  	r15,r13




_TestArrayAssign3:
	      	   push 	r13
	      	   push 	r12
	      	   mov  	r12,r14
	      	   sub  	r14,r0,63
	# 	for (m = 0; m < 3; m++) {
	      	   sto  	r0,r12,-3
TestArrayAssign_12:
	      	   ld   	r5,r12,-3
	      	   cmp  	r5,r0,3
	      	mi.mov  	r15,r0,TestArrayAssign_13
	# 		for (j = 0; j < 4; j++) {
	      	   sto  	r0,r12,-1
TestArrayAssign_15:
	      	   ld   	r5,r12,-1
	      	   cmp  	r5,r0,4
	      	mi.mov  	r15,r0,TestArrayAssign_16
	# 			for (k = 0; k < 5; k++)
	      	   sto  	r0,r12,-2
TestArrayAssign_18:
	      	   ld   	r5,r12,-2
	      	   cmp  	r5,r0,5
	      	mi.mov  	r15,r0,TestArrayAssign_19
	# 				x[m][j][k] = rand();
	      	   ld   	r5,r12,-2
	      	   ld   	r6,r12,-1
	      	   mov  	r7,r0,5
	      	   mov  	r1,r6
	      	   mov  	r2,r7
	      	   jsr  	r13,r0,_mulu
	      	   mov  	r6,r1
	      	   ld   	r7,r12,-3
	      	   push 	r5
	      	   mov  	r5,r0,20
	      	   mov  	r1,r7
	      	   mov  	r2,r5
	      	   jsr  	r13,r0,_mulu
	      	   mov  	r7,r1
	      	   mov  	r5,r12,-63
	      	   add  	r7,r5
	      	   add  	r6,r7
	      	   mov  	r7,r5
	      	   add  	r5,r6
	      	   jsr  	r13,r0,_rand
	      	   sto  	r1,r5,0
	      	   pop  	r5
	      	   ld   	r5,r12,-2
	      	   inc  	r5,1
	      	   sto  	r5,r12,-2
	      	   mov  	r15,r0,TestArrayAssign_18
TestArrayAssign_19:
	      	   ld   	r5,r12,-1
	      	   inc  	r5,1
	      	   sto  	r5,r12,-1
	      	   mov  	r15,r0,TestArrayAssign_15
TestArrayAssign_16:
	      	   ld   	r5,r12,-3
	      	   inc  	r5,1
	      	   sto  	r5,r12,-3
	      	   mov  	r15,r0,TestArrayAssign_12
TestArrayAssign_13:
	      	   mov  	r14,r12
	      	   pop  	r13
	      	   pop  	r12
	      	   mov  	r15,r13




#	rodata
#	extern	_rand
#	global	_TestArrayAssign
#	global	_TestArrayAssign2
#	global	_TestArrayAssign3
#	code
_TestArrayY:
	      	   push 	r13
	      	   push 	r12
	      	   mov  	r12,r14
	      	   dec  	r14,8
	# 	y = {1,2,3,4,5,6,7,8};
	      	   mov  	r5,r0
	      	   add  	r5,r12,-8
	      	   mov  	r6,r5
	      	   mov  	r7,r0,1
	      	   sto  	r7,r6,0
	      	   mov  	r7,r0,2
	      	   sto  	r7,r6,1
	      	   mov  	r7,r0,3
	      	   sto  	r7,r6,2
	      	   mov  	r7,r0,4
	      	   sto  	r7,r6,3
	      	   mov  	r7,r0,5
	      	   sto  	r7,r6,4
	      	   mov  	r7,r0,6
	      	   sto  	r7,r6,5
	      	   mov  	r7,r0,7
	      	   sto  	r7,r6,6
	      	   mov  	r7,r0,8
	      	   sto  	r7,r6,7
	      	   mov  	r14,r12
	      	   pop  	r13
	      	   pop  	r12
	      	   mov  	r15,r13
#	rodata
#	extern	_TestArrayY
#	code
_TestDivmod:
	      	   push 	r12
	      	   mov  	r12,r14
	# 	return a / b;
	      	   ld   	r1,r12,1
	      	   ld   	r2,r12,2
	      	   mov  	r13,r15,2
	      	   mov  	r15,r0,_div
	      	   mov  	r14,r12
	      	   pop  	r12
	      	   mov  	r15,r13
_TestDivmod2:
	      	   push 	r12
	      	   mov  	r12,r14
	# 	return a / 10;
	      	   ld   	r1,r12,1
	      	   mov  	r2,r0,10
	      	   mov  	r13,r15,2
	      	   mov  	r15,r0,_div
	      	   mov  	r14,r12
	      	   pop  	r12
	      	   mov  	r15,r13
_TestMod:
	      	   push 	r12
	      	   mov  	r12,r14
	# 	return a % b;
	      	   ld   	r1,r12,1
	      	   ld   	r2,r12,2
	      	   mov  	r13,r15,2
	      	   mov  	r15,r0,_mod
	      	   mov  	r14,r12
	      	   pop  	r12
	      	   mov  	r15,r13
_TestDivmod3:
	      	   push 	r12
	      	   mov  	r12,r14
	# 	a /= b;
	      	   ld   	r1,r12,1
	      	   ld   	r2,r12,2
	      	   mov  	r13,r15,2
	      	   mov  	r15,r0,_div
	      	   sto  	r1,r12,1
	# 	return a;
	      	   ld   	r1,r12,1
TestDivmod_16:
	      	   nop  
	      	   mov  	r14,r12
	      	   pop  	r12
	      	   mov  	r15,r13
_TestModu:
	# 	return a % b;
	      	   mov  	r1,r8,0
	      	   mov  	r2,r9,0
	      	   mov  	r13,r15,2
	      	   mov  	r15,r0,_modu
	      	   mov  	r15,r13
#	rodata
#	extern	_TestMod
#	extern	_TestDivmod
#	extern	_TestDivmod2
#	extern	_TestModu
#	extern	_TestDivmod3
#	code
_TestFor:
	      	   push 	r13
	      	   push 	r12
	      	   mov  	r12,r14
	      	   dec  	r14,3
	# 	for (x = 1; x < 100; x++) {
	      	   mov  	r5,r0,1
	      	   sto  	r5,r12,-1
TestFor_4:
	      	   ld   	r5,r12,-1
	      	   cmp  	r5,r0,100
	      	mi.inc  	r15,TestFor_5-PC
	# 		putch('a');
	      	   mov  	r5,r0,97
	      	   push 	r5
	      	   jsr  	r13,r0,_putch
	      	   inc  	r14,1
	      	   ld   	r5,r12,-1
	      	   inc  	r5,1
	      	   sto  	r5,r12,-1
	      	   mov  	r15,r0,TestFor_4
TestFor_5:
	# 	y = 50;
	      	   mov  	r5,r0,50
	      	   sto  	r5,r12,-2
TestFor_7:
	      	   ld   	r5,r12,-2
	      	   cmp  	r5,r0
	      	 z.inc  	r15,TestFor_10-PC
	      	   cmp  	r5,r0
	      	pl.inc  	r15,TestFor_8-PC
TestFor_10:
	# 		putch('b');
	      	   mov  	r5,r0,98
	      	   push 	r5
	      	   jsr  	r13,r0,_putch
	      	   inc  	r14,1
	# 		--y;
	      	   ld   	r5,r12,-2
	      	   dec  	r5,1
	      	   sto  	r5,r12,-2
	      	   mov  	r15,r0,TestFor_7
TestFor_8:
	# 	for (z = 1; z < 10; ) ;
	      	   mov  	r5,r0,1
	      	   sto  	r5,r12,-3
TestFor_11:
	      	   ld   	r5,r12,-3
	      	   cmp  	r5,r0,10
	      	pl.dec  	r15,TestFor_11-PC
	      	   mov  	r14,r12
	      	   pop  	r13
	      	   pop  	r12
	      	   mov  	r15,r13




#	rodata
#	global	_TestFor
#	extern	_putch
#	code
_SomeFunc:
	      	   push 	r12
	      	   mov  	r12,r14
	# 	return a + b - c;
	      	   ld   	r1,r12,1
	      	   ld   	r2,r12,2
	      	   add  	r1,r2
	      	   sub  	r1,r8
TestFunccall_4:
	      	   nop  
	      	   mov  	r14,r12
	      	   pop  	r12
	      	   mov  	r15,r13
_main:
	# 	SomeFunc(10,20,30);
	      	   push 	r8
	      	   mov  	r5,r0,30
	      	   mov  	r8,r5
	      	   mov  	r6,r0,20
	      	   push 	r6
	      	   mov  	r7,r0,10
	      	   push 	r7
	      	   jsr  	r13,r0,_SomeFunc
	      	   inc  	r14,2
	      	   pop  	r8
	      	   mov  	r15,r13




#	rodata
#	global	_main
#	extern	_SomeFunc
#	code
_myint:
	# void interrupt myint()
	      	   push 	r1
	      	   push 	r2
	      	   push 	r3
	      	   push 	r4
	      	   push 	r5
	      	   push 	r6
	      	   push 	r7
	      	   push 	r8
	      	   push 	r9
	      	   push 	r10
	      	   push 	r11
	      	   push 	r12
	      	   push 	r13
	      	   push 	r13
	      	   push 	r12
	      	   mov  	r12,r14
	# 	printf("Hello again.");
	      	   mov  	r5,r0,TestInt_1
	      	   push 	r5
	      	   jsr  	r13,r0,_printf
	      	   inc  	r14,1
	      	   mov  	r14,r12
	      	   pop  	r13
	      	   pop  	r12
	      	   ld   	r1,r14,0
	      	   ld   	r2,r14,1
	      	   ld   	r3,r14,2
	      	   ld   	r4,r14,3
	      	   ld   	r5,r14,4
	      	   ld   	r6,r14,5
	      	   ld   	r7,r14,6
	      	   ld   	r8,r14,7
	      	   ld   	r9,r14,8
	      	   ld   	r10,r14,9
	      	   ld   	r11,r14,10
	      	   ld   	r12,r14,11
	      	   ld   	r13,r14,12
	      	   add  	r14,r0,13
	      	   rti  




_BIOScall:
	# void interrupt myint()
	      	   push 	r1
	      	   push 	r2
	      	   push 	r3
	      	   push 	r4
	      	   push 	r5
	      	   push 	r6
	      	   push 	r7
	      	   push 	r8
	      	   push 	r9
	      	   push 	r10
	      	   push 	r11
	      	   push 	r12
	      	   push 	r13
	      	   push 	r12
	      	   mov  	r12,r14
	      	   mov  	r1,r0,-1
	      	   mov  	r14,r12
	      	   pop  	r12
	      	   ld   	r1,r14,0
	      	   ld   	r2,r14,1
	      	   ld   	r3,r14,2
	      	   ld   	r4,r14,3
	      	   ld   	r5,r14,4
	      	   ld   	r6,r14,5
	      	   ld   	r7,r14,6
	      	   ld   	r8,r14,7
	      	   ld   	r9,r14,8
	      	   ld   	r10,r14,9
	      	   ld   	r11,r14,10
	      	   ld   	r12,r14,11
	      	   ld   	r13,r14,12
	      	   add  	r14,r0,13
	      	   rti  




#	rodata
	align	2
TestInt_1:	# Hello again.
	word	72,101,108,108,111,32,97,103
	word	97,105,110,46,0
#	global	_BIOScall
#	global	_myint
#	extern	_printf
#	code
_TestLValue:
	      	   push 	r12
	      	   mov  	r12,r14
	      	   dec  	r14,3
	      	   push 	r2
	      	   mov  	r1,r12,-2
	      	   mov  	r2,r1
	# 	x = y + z;
	      	   ld   	r1,r2,0
	      	   ld   	r2,r12,-3
	      	   add  	r1,r2
	      	   sto  	r1,r12,-1
	# 	x = &y + 20;
	      	   mov  	r1,r0,20
	      	   add  	r2,r1
	      	   sto  	r2,r12,-1
	# 	x = y + &x;
	      	   ld   	r1,r2,0
	      	   mov  	r2,r12,-1
	      	   add  	r1,r2
	      	   sto  	r1,r12,-1
	# 	&x = y + z;	// should give an LValue error
	      	   mov  	r1,r12,-1
	      	   pop  	r2
	      	   mov  	r14,r12
	      	   pop  	r12
	      	   mov  	r15,r13




#	rodata
#	global	_TestLValue
#	code
_TestPreload:
	      	   push 	r12
	      	   mov  	r12,r14
	      	   dec  	r14,1
	# 	int x = a < 10;
	      	   mov  	r1,r0,1
	      	   ld   	r2,r12,1
	      	   cmp  	r2,r0,10
	      	pl.mov  	r1,r0
	      	   sto  	r1,r12,-1
	# 	return(x);
	      	   ld   	r1,r12,-1
TestPreload_6:
	      	   nop  
	      	   mov  	r14,r12
	      	   pop  	r12
	      	   mov  	r15,r13
#	rodata
#	extern	_TestPreload
#	code
_TestRotate:
	      	   push 	r12
	      	   mov  	r12,r14
	# 	return ((a << b) | (a >> (16-b)));
	      	   ld   	r1,r12,1
	      	   ld   	r2,r12,2
TestRotate_4:
	      	   add  	r1,r0,0
	      	   sub  	r2,r0,1
	      	nz.dec  	r15,TestRotate_4-PC
	      	   ld   	r2,r12,1
	      	   mov  	r5,r0,16
	      	   ld   	r6,r12,2
	      	   sub  	r5,r6
TestRotate_5:
	      	   add  	r2,r0,0
	      	pl.add  	r0,r0,0
	      	mi.sub  	r0,r0,1
	      	   ror  	r2,r0,0
	      	   sub  	r5,r0,1
	      	nz.dec  	r15,TestRotate_5-PC
	      	   or   	r1,r2
TestRotate_6:
	      	   nop  
	      	   mov  	r14,r12
	      	   pop  	r12
	      	   mov  	r15,r13
_TestRotate2:
TestRotate_10:
	      	   add  	r8,r0,0
	      	   sub  	r9,r0,1
	      	nz.dec  	r15,TestRotate_10-PC
	      	   mov  	r1,r0,16
	      	   sub  	r1,r9
TestRotate_11:
	      	   add  	r8,r0,0
	      	pl.add  	r0,r0,0
	      	mi.sub  	r0,r0,1
	      	   ror  	r8,r0,0
	      	   sub  	r1,r0,1
	      	nz.dec  	r15,TestRotate_11-PC
	      	   or   	r1,r8
	      	   mov  	r15,r13
#	rodata
#	extern	_TestRotate
#	extern	_TestRotate2
#	code
_TestShiftLeft:
TestShift_4:
	      	   add  	r8,r0,0
	      	   sub  	r9,r0,1
	      	nz.dec  	r15,TestShift_4-PC
	      	   mov  	r1,r8
	      	   mov  	r15,r13
_TestShiftRight:
	      	   push 	r12
	      	   mov  	r12,r14
	# 	return a >> b;
	      	   ld   	r1,r12,1
	      	   ld   	r2,r12,2
TestShift_9:
	      	   add  	r1,r0,0
	      	pl.add  	r0,r0,0
	      	mi.sub  	r0,r0,1
	      	   ror  	r1,r0,0
	      	   sub  	r2,r0,1
	      	nz.dec  	r15,TestShift_9-PC
	      	   mov  	r14,r12
	      	   pop  	r12
	      	   mov  	r15,r13
_TestShiftLeftI1:
	      	   push 	r12
	      	   mov  	r12,r14
	# 	return a << 1;
	      	   ld   	r1,r12,1
	      	   add  	r1,r0,0
TestShift_14:
	      	   nop  
	      	   mov  	r14,r12
	      	   pop  	r12
	      	   mov  	r15,r13
_TestShiftLeftI5:
	      	   push 	r12
	      	   mov  	r12,r14
	# 	return a << 1;
	      	   ld   	r1,r12,1
	      	   add  	r1,r0,0
TestShift_18:
	      	   nop  
	      	   mov  	r14,r12
	      	   pop  	r12
	      	   mov  	r15,r13
_TestShiftRight:
	      	   push 	r12
	      	   mov  	r12,r14
	# 	return a >> b;
	      	   ld   	r1,r12,1
	      	   ld   	r2,r12,2
TestShift_22:
	      	   add  	r1,r0,0
	      	pl.add  	r0,r0,0
	      	mi.sub  	r0,r0,1
	      	   ror  	r1,r0,0
	      	   sub  	r2,r0,1
	      	nz.dec  	r15,TestShift_22-PC
	      	   mov  	r14,r12
	      	   pop  	r12
	      	   mov  	r15,r13
#	rodata
#	extern	_TestShiftLeftI1
#	extern	_TestShiftLeftI5
#	extern	_TestShiftLeft
#	extern	_TestShiftRight
#	extern	_TestShiftRight
#	code
_TestStruct:
	      	   push 	r13
	      	   push 	r12
	      	   mov  	r12,r14
	      	   dec  	r14,3
	      	   push 	r2
	      	   mov  	r5,r12,-3
	      	   mov  	r2,r5
	# 	ts.a = 1;
	      	   mov  	r5,r0,1
	      	   sto  	r5,r2,0
	# 	ts.b = 2;
	      	   mov  	r5,r0,2
	      	   sto  	r5,r2,1
	# 	ts.c = 3;
	      	   mov  	r5,r0,3
	      	   ld   	r6,r2,2
	      	   and  	r5,r0,16383
	      	   and  	r6,r0,-16384
	      	   or   	r6,r5,0
	      	   sto  	r6,r2,2
	# 	ts.d = 1;
	      	   mov  	r5,r0,1
	      	   ld   	r6,r2,2
	      	   and  	r5,r0,3
	      	   ror  	r6,r0,0
	      	   ror  	r6,r0,0
	      	   ror  	r6,r0,0
	      	   ror  	r6,r0,0
	      	   ror  	r6,r0,0
	      	   ror  	r6,r0,0
	      	   ror  	r6,r0,0
	      	   ror  	r6,r0,0
	      	   ror  	r6,r0,0
	      	   ror  	r6,r0,0
	      	   ror  	r6,r0,0
	      	   ror  	r6,r0,0
	      	   ror  	r6,r0,0
	      	   ror  	r6,r0,0
	      	   and  	r6,r0,-4
	      	   or   	r6,r5,0
	      	   adc  	r6,r0,14
	      	   adc  	r6,r0,14
	      	   adc  	r6,r0,14
	      	   adc  	r6,r0,14
	      	   adc  	r6,r0,14
	      	   adc  	r6,r0,14
	      	   adc  	r6,r0,14
	      	   adc  	r6,r0,14
	      	   adc  	r6,r0,14
	      	   adc  	r6,r0,14
	      	   adc  	r6,r0,14
	      	   adc  	r6,r0,14
	      	   adc  	r6,r0,14
	      	   adc  	r6,r0,14
	      	   sto  	r6,r2,2
	      	   mov  	r5,r6,0
	      	   and  	r5,r0,16383
	      	   mov  	r6,r0,-8192
	      	   add  	r5,r0,r6
	      	   xor  	r5,r0,r6
	      	   ld   	r7,r2,2
	      	   mov  	r6,r7,0
	      	   ror  	r6,r0,0
	      	   ror  	r6,r0,0
	      	   ror  	r6,r0,0
	      	   ror  	r6,r0,0
	      	   ror  	r6,r0,0
	      	   ror  	r6,r0,0
	      	   ror  	r6,r0,0
	      	   ror  	r6,r0,0
	      	   ror  	r6,r0,0
	      	   ror  	r6,r0,0
	      	   ror  	r6,r0,0
	      	   ror  	r6,r0,0
	      	   ror  	r6,r0,0
	      	   ror  	r6,r0,0
	      	   and  	r6,r0,3
	      	   mov  	r7,r0,-2
	      	   add  	r6,r0,r7
	      	   xor  	r6,r0,r7
	      	   add  	r1,r6
	      	   pop  	r2
	      	   mov  	r14,r12
	      	   pop  	r13
	      	   pop  	r12
	      	   mov  	r15,r13
_TestStruct2:
	      	   push 	r13
	      	   push 	r12
	      	   mov  	r12,r14
	      	   dec  	r14,3
	# 	b.a = a;
	      	   sto  	r8,r12,-3
	# 	return (b);
	      	   mov  	r5,r12,-3
	      	   ld   	r7,r12,2
	      	   ld   	r6,r5
	      	   sto  	r6,r7
	      	   ld   	r6,r5,1
	      	   sto  	r6,r7,1
	      	   ld   	r6,r5,2
	      	   sto  	r6,r7,2
	      	   mov  	r14,r12
	      	   pop  	r13
	      	   pop  	r12
	      	   mov  	r15,r13
#	rodata
#	extern	_TestStruct2
