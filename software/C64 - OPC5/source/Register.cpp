// ============================================================================
//        __
//   \\__/ o\    (C) 2012-2017  Robert Finch, Waterloo
//    \  __ /    All rights reserved.
//     \/_//     robfinch<remove>@finitron.ca
//       ||
//
// C64 - 'C' derived language compiler
//  - 64 bit CPU
//
// This source file is free software: you can redistribute it and/or modify 
// it under the terms of the GNU Lesser General Public License as published 
// by the Free Software Foundation, either version 3 of the License, or     
// (at your option) any later version.                                      
//                                                                          
// This source file is distributed in the hope that it will be useful,      
// but WITHOUT ANY WARRANTY; without even the implied warranty of           
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            
// GNU General Public License for more details.                             
//                                                                          
// You should have received a copy of the GNU General Public License        
// along with this program.  If not, see <http://www.gnu.org/licenses/>.    
//                                                                          
// ============================================================================
//
#include "stdafx.h"

extern void initFPRegStack();
extern void ReleaseTempFPRegister(AMODE *);
/*
int tmpregs[] = {3,4,5,6,7,8,9,10};
int regstack[8];
int rsp=7;
int regmask=0;

int tmpbregs[] = {3,4,5,6,7,8};
int bregstack[6];
int brsp=5;
*/
static short int next_reg;
static short int next_breg;
#define MAX_REG 4			/* max. scratch data	register (D2) */
#define	MAX_REG_STACK	30

// Only registers 5,6,7 and 8 are used for temporaries
static short int reg_in_use[256];	// 0 to 15
static short int save_reg_in_use[256];

static struct {
    enum e_am mode;
    short int reg;
	union {
		char isPushed;	/* flags if pushed or corresponding reg_alloc * number */
		char allocnum;
	} f;
} 
	reg_stack[MAX_REG_STACK + 1],
	reg_alloc[MAX_REG_STACK + 1],
	save_reg_alloc[MAX_REG_STACK + 1],
	stacked_regs[MAX_REG_STACK + 1];

static short int reg_stack_ptr;
static short int reg_alloc_ptr;
static short int save_reg_alloc_ptr;
static short int breg_stack_ptr;
static short int breg_alloc_ptr;

#define MAXTMPREG	(sym->AllowRegVars ? 3 : 5)
#define MINTMPREG	(sym->IsLeaf ? 0 : 1)
#define NTMPREG		2

char tmpregs[] = {1,5,6,7,3,4};
char regstack[18];
int rsp=17;
int regmask=0;

void initRegStack()
{
	int i;
	SYM *sym = currentFn;

	next_reg = MINTMPREG;
	//for (rsp=0; rsp < 3; rsp=rsp+1)
	//	regstack[rsp] = tmpregs[rsp];
	//rsp = 0;
	for (i = 0; i <= 255; i++) {
		reg_in_use[i] = -1;
	}
    reg_stack_ptr = 0;
    reg_alloc_ptr = 0;
//    act_scratch = 0;
    memset(reg_stack,0,sizeof(reg_stack));
    memset(reg_alloc,0,sizeof(reg_alloc));
    memset(stacked_regs,0,sizeof(stacked_regs));
    memset(save_reg_alloc,0,sizeof(save_reg_alloc));
}

void GenerateTempRegPush(int reg, int rmode, int number, int stkpos)
{
	AMODE *ap1;
    ap1 = allocAmode();
    ap1->preg = reg;
    ap1->mode = rmode;


	GenerateMonadic(op_push,0,ap1);
	TRACE(printf("pushing r%d\n", reg);)
    reg_stack[reg_stack_ptr].mode = (enum e_am)rmode;
    reg_stack[reg_stack_ptr].reg = reg;
    reg_stack[reg_stack_ptr].f.allocnum = number;
    if (reg_alloc[number].f.isPushed=='T')
		fatal("GenerateTempRegPush(): register already pushed");
    reg_alloc[number].f.isPushed = 'T';
	if (++reg_stack_ptr > MAX_REG_STACK)
		fatal("GenerateTempRegPush(): register stack overflow");
}

void GenerateTempRegPop(int reg, int rmode, int number, int stkpos)
{
	AMODE *ap1;
 
    if (reg_stack_ptr-- == -1)
		fatal("GenerateTempRegPop(): register stack underflow");
    /* check if the desired register really is on stack */
    if (reg_stack[reg_stack_ptr].f.allocnum != number)
		fatal("GenerateTempRegPop()/2");
	if (reg_in_use[reg] >= 0)
		fatal("GenerateTempRegPop():register still in use");
	TRACE(printf("popped r%d\n", reg);)
	reg_in_use[reg] = number;
	ap1 = allocAmode();
	ap1->preg = reg;
	ap1->mode = rmode;
	GenerateMonadic(op_pop,0,ap1);
    reg_alloc[number].f.isPushed = 'F';
}

void initstack()
{
	initRegStack();
	//initFPRegStack();
}

AMODE *GetTempRegister()
{
	int pushed;

	return (GetTempRegister2(&pushed));
}

AMODE *GetTempRegister2(int *pushed)
{
	AMODE *ap;
    SYM *sym = currentFn;

	*pushed = FALSE;
	if (reg_in_use[tmpregs[next_reg]] >= 0) {
		*pushed = TRUE;
		GenerateTempRegPush(tmpregs[next_reg], am_reg, reg_in_use[tmpregs[next_reg]],0);
	}
	TRACE(printf("GetTempRegister:r%d\n", next_reg);)
    reg_in_use[tmpregs[next_reg]] = reg_alloc_ptr;
    ap = allocAmode();
    ap->mode = am_reg;
    ap->preg = tmpregs[next_reg];
    ap->deep = reg_alloc_ptr;
    reg_alloc[reg_alloc_ptr].reg = tmpregs[next_reg];
    reg_alloc[reg_alloc_ptr].mode = am_reg;
    reg_alloc[reg_alloc_ptr].f.isPushed = 'F';
    if (next_reg++ >= MAXTMPREG)
		next_reg = MINTMPREG;		/* wrap around */
    if (reg_alloc_ptr++ == MAX_REG_STACK)
		fatal("GetTempRegister(): register stack overflow");
	return ap;
}

AMODE *GetTempFPRegister()
{
	AMODE *ap;
    SYM *sym = currentFn;
    
	if (reg_in_use[next_reg] >= 0) {
		GenerateTempRegPush(next_reg, am_reg, reg_in_use[next_reg],0);
	}
	TRACE(printf("GetTempRegister:r%d\n", next_reg);)
    reg_in_use[next_reg] = reg_alloc_ptr;
    ap = allocAmode();
    ap->mode = am_fpreg;
    ap->preg = next_reg;
    ap->deep = reg_alloc_ptr;
	ap->isFloat = TRUE;
    reg_alloc[reg_alloc_ptr].reg = next_reg;
    reg_alloc[reg_alloc_ptr].mode = am_reg;
    reg_alloc[reg_alloc_ptr].f.isPushed = 'F';
    if (next_reg++ >= 10)
    	next_reg = MINTMPREG;		/* wrap around */
    if (reg_alloc_ptr++ == MAX_REG_STACK)
		fatal("GetTempRegister(): register stack overflow");
	return ap;
}
/*
 * this routines checks if all allocated registers were freed
 */
void checkstack()
{
    int i;
    SYM *sym = currentFn;

    for (i=1; i<= MAXTMPREG; i++)
        if (reg_in_use[i] != -1)
            fatal("checkstack()/1");
	if (next_reg != sym->IsLeaf ? 1 : 3) {
		//printf("Nextreg: %d\n", next_reg);
        fatal("checkstack()/3");
	}
    if (reg_stack_ptr != 0)
        fatal("checkstack()/5");
    if (reg_alloc_ptr != 0)
        fatal("checkstack()/6");
}

static int IsTmpReg(int rg)
{
	int nn;
    SYM *sym = currentFn;

	for (nn = MINTMPREG; nn < MAXTMPREG+1; nn++)
		if (rg==tmpregs[nn])
			return (TRUE);
	return (FALSE);
}

/*
 * validate will make sure that if a register within an address mode has been
 * pushed onto the stack that it is popped back at this time.
 */
void validate(AMODE *ap)
{
    SYM *sym = currentFn;
	int frg = MINTMPREG;

	if (ap->amode2)
		validate(ap->amode2);
    switch (ap->mode) {
	case am_reg:
		if (IsTmpReg(ap->preg) && reg_alloc[ap->deep].f.isPushed == 'T' ) {
			GenerateTempRegPop(ap->preg, am_reg, (int) ap->deep, 0);
		}
		break;
    case am_indx2:
		if (IsTmpReg(ap->preg) && reg_alloc[ap->deep].f.isPushed == 'T') {
			GenerateTempRegPop(ap->preg, am_reg, (int) ap->deep, 0);
		}
		if (IsTmpReg(ap->sreg) && reg_alloc[ap->deep2].f.isPushed  == 'T') {
			GenerateTempRegPop(ap->sreg, am_reg, (int) ap->deep2, 0);
//			if (isThor)
//				GenerateTriadic(op_addui,0,makereg(regSP),makereg(regSP),make_immed(8));
		}
		break;
    case am_indx3:
		if (IsTmpReg(ap->sreg) && reg_alloc[ap->deep2].f.isPushed == 'T') {
			GenerateTempRegPop(ap->sreg, am_reg, (int) ap->deep2, 0);
//			if (isThor)
//				GenerateTriadic(op_addui,0,makereg(regSP),makereg(regSP),make_immed(8));
		}
		goto common;
    case am_ind:
    case am_indx:
    case am_ainc:
    case am_adec:
common:
		if (IsTmpReg(ap->preg) && reg_alloc[ap->deep].f.isPushed == 'T') {
			GenerateTempRegPop(ap->preg, am_reg, (int) ap->deep, 0);
		}
		break;
    }
}


/*
 * release any temporary registers used in an addressing mode.
 */
void ReleaseTempRegister(AMODE *ap)
{
    int number;
    SYM *sym = currentFn;
	int frg = MINTMPREG;

	TRACE(printf("ReleaseTempRegister:r%d r%d\n", ap->preg, ap->sreg);)

	if (ap->amode2)
		ReleaseTempRegister(ap->amode2);

	if (ap==NULL) {
		printf("DIAG - NULL pointer in ReleaseTempRegister\n");
		return;
	}

	validate(ap);
    switch (ap->mode) {
	case am_ind:
	case am_indx:
	case am_ainc:
	case am_adec:
	case am_reg:
common:
		if (IsTmpReg(ap->preg)) {
			if (reg_in_use[ap->preg]==-1)
				return;
			if (next_reg-- <= frg)
				next_reg = MAXTMPREG;
			number = reg_in_use[ap->preg];
			reg_in_use[ap->preg] = -1;
			if (reg_alloc_ptr-- == 0)
				fatal("ReleaseTempRegister(): no registers are allocated");
			if (reg_alloc[number].f.isPushed=='T')
				fatal("ReleaseTempRegister(): register on stack");
		}
		return;
    case am_indx2:
	case am_indx3:
		if (IsTmpReg(ap->sreg)) {
			if (reg_in_use[ap->sreg]==-1)
				return;
			if (next_reg-- <= frg)
				next_reg = MAXTMPREG;
			number = reg_in_use[ap->sreg];
			reg_in_use[ap->sreg] = -1;
			//break;
		}
		goto common;
    default:
		return;
    }
 //   /* some consistency checks */
	//if (number != ap->deep) {
	//	printf("number %d ap->deep %d\n", number, ap->deep);
	//	//fatal("ReleaseTempRegister()/1");
	//}
}

// The following is used to save temporary registers across function calls.
// Save the list of allocated registers and registers in use.
// Go through the allocated register list and generate a push instruction to
// put the register on the stack if it isn't already on the stack.

int TempInvalidate()
{
    int i;
	int sp;

	sp = 0;
	TRACE(printf("TempInvalidate()\n");)
	save_reg_alloc_ptr = reg_alloc_ptr;
	memcpy(save_reg_alloc, reg_alloc, sizeof(save_reg_alloc));
	memcpy(save_reg_in_use, reg_in_use, sizeof(save_reg_in_use));
	for (sp = i = 0; i < reg_alloc_ptr; i++) {
        if (reg_in_use[reg_alloc[i].reg] != -1) {
    		if (reg_alloc[i].f.isPushed == 'F') {
    			GenerateTempRegPush(reg_alloc[i].reg, reg_alloc[i].mode, i, sp);
    			stacked_regs[sp].reg = reg_alloc[i].reg;
    			stacked_regs[sp].mode = reg_alloc[i].mode;
    			stacked_regs[sp].f.allocnum = i;
    			sp++;
    			// mark the register void
    			reg_in_use[reg_alloc[i].reg] = -1;
    		}
        }
	}
	return sp;
}

// Pop back any temporary registers that were pushed before the function call.
// Restore the allocated and in use register lists.

void TempRevalidate(int sp)
{
	int nn;

	for (nn = sp-1; nn >= 0; nn--)
		GenerateTempRegPop(stacked_regs[nn].reg, stacked_regs[nn].mode, stacked_regs[nn].f.allocnum,sp-nn-1);
	reg_alloc_ptr = save_reg_alloc_ptr;
	memcpy(reg_alloc, save_reg_alloc, sizeof(reg_alloc));
	memcpy(reg_in_use, save_reg_in_use, sizeof(reg_in_use));
}

/*
void initRegStack()
{
	for (rsp=0; rsp < 8; rsp=rsp+1)
		regstack[rsp] = tmpregs[rsp];
	for (brsp = 0; brsp < 6; brsp++)
		bregstack[brsp] = tmpbregs[brsp];
	rsp = 0;
	brsp = 0;
}

void ReleaseTempRegister(AMODE *ap)
{
	if (ap==NULL) {
		printf("DIAG - NULL pointer in ReleaseTempRegister\n");
		return;
	}
	if( ap->mode == am_immed || ap->mode == am_direct )
        return;         // no registers used
	if (ap->mode == am_breg || ap->mode==am_brind) {
		if (ap->preg < 9 && ap->preg >= 3)
			PushOnBrstk(ap->preg);
		return;
	}
	if(ap->preg < 11 && ap->preg >= 3)
		PushOnRstk(ap->preg);
}
*/
void ReleaseTempFPRegister(AMODE *ap)
{
     ReleaseTempRegister(ap);
}

void ReleaseTempReg(AMODE *ap)
{
     if (ap->mode==am_fpreg)
         ReleaseTempFPRegister(ap);
     else {
		 if (ap->amode2)
			 ReleaseTempRegister(ap->amode2);
         ReleaseTempRegister(ap);
	 }
}

