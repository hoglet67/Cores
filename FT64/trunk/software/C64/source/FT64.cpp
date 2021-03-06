// ============================================================================
// (C) 2017 Robert Finch
// All Rights Reserved.
// robfinch<remove>@finitron.ca
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

extern int lastsph;
extern char *semaphores[20];
extern int throwlab;
extern int breaklab;
extern int contlab;
extern int retlab;

extern TYP              stdfunc;

extern void DumpCSETable();
extern void scan(Statement *);
extern int GetReturnBlockSize();
void GenerateReturn(Statement *stmt);
extern void GenerateComment(char *);
int TempFPInvalidate();
int TempInvalidate();
void TempRevalidate(int);
void TempFPRevalidate(int);
void ReleaseTempRegister(AMODE *ap);
AMODE *GetTempRegister();
void GenLdi(AMODE *, AMODE *);
extern AMODE *copy_addr(AMODE *ap);
extern void GenLoad(AMODE *ap1, AMODE *ap3, int ssize, int size);

//
// Returns the desirability of optimization for a subexpression.
//
static int OptimizationDesireability(CSE *csp)
{
	if( csp->voidf || (csp->exp->nodetype == en_icon &&
                       csp->exp->i < 32768 && csp->exp->i >= -32768))
        return 0;
    if (csp->exp->nodetype==en_cnacon)
        return 0;
	if (csp->exp->isVolatile)
		return 0;
    if( IsLValue(csp->exp) )
	    return 2 * csp->uses;
    return csp->uses;
}

static int CSECmp(const void *a, const void *b)
{
	CSE *csp1, *csp2;
	int aa,bb;

	csp1 = (CSE *)a;
	csp2 = (CSE *)b;
	aa = OptimizationDesireability(csp1);
	bb = OptimizationDesireability(csp2);
	if (aa < bb)
		return (1);
	else if (aa == bb)
		return (0);
	else
		return (-1);
}

static int AllocateRegisters1()
{
	int nn,csecnt,reg;
	CSE *csp;

	reg = regFirstRegvar;
	for (nn = 0; nn < 3; nn++) {
		for (csecnt = 0; csecnt < csendx; csecnt++)	{
			csp = &CSETable[csecnt];
			if (csp->reg==-1) {
				if( OptimizationDesireability(csp) >= 4-nn ) {
					if (csp->exp->etype!=bt_vector) {
    					if(( csp->duses > csp->uses / (8 << nn)) && reg < regLastRegvar )
    						csp->reg = reg++;
    					else
    						csp->reg = -1;
					}
				}
			}
		}
	}
	return reg;
}

static int FinalAllocateRegisters(int reg)
{
	int csecnt;
	CSE *csp;

	for (csecnt = 0; csecnt < csendx; csecnt++)	{
		csp = &CSETable[csecnt];
		if (!csp->voidf && csp->reg==-1) {
			if (csp->exp->etype!=bt_vector) {
    			if(( csp->uses > 3) && reg < regLastRegvar )
    				csp->reg = reg++;
    			else
    				csp->reg = -1;
			}
		}
	}
	return reg;
}

static int AllocateVectorRegisters1()
{
	int nn,csecnt,vreg;
	CSE *csp;

	vreg = 11;
	for (nn = 0; nn < 3; nn++) {
		for (csecnt = 0; csecnt < csendx; csecnt++)	{
			csp = &CSETable[csecnt];
			if (csp->reg==-1) {
				if( OptimizationDesireability(csp) >= 4-nn ) {
					if (csp->exp->etype==bt_vector) {
    					if(( csp->duses > csp->uses / (8 << nn)) && vreg < 18 )
    						csp->reg = vreg++;
    					else
    						csp->reg = -1;
					}
				}
			}
		}
	}
	return vreg;
}

static int FinalAllocateVectorRegisters(int vreg)
{
	int csecnt;
	CSE *csp;

	vreg = 11;
	for (csecnt = 0; csecnt < csendx; csecnt++)	{
		csp = &CSETable[csecnt];
		if (!csp->voidf && csp->reg==-1) {
			if (csp->exp->etype==bt_vector) {
    			if(( csp->uses > 3) && vreg < 18 )
    				csp->reg = vreg++;
    			else
    				csp->reg = -1;
			}
		}
	}
	return vreg;
}

// ----------------------------------------------------------------------------
// AllocateRegisterVars will allocate registers for the expressions that have
// a high enough desirability.
// ----------------------------------------------------------------------------

int AllocateRegisterVars()
{
	CSE *csp;
    ENODE *exptr;
    int reg, vreg;
	uint64_t mask, rmask;
    uint64_t fpmask, fprmask;
	uint64_t vmask, vrmask;
    AMODE *ap, *ap2;
	int64_t nn;
	int cnt;
	int size;
	int csecnt;

	reg = regFirstRegvar;
	vreg = 11;
    mask = 0;
	rmask = 0;
	fpmask = 0;
	fprmask = 0;
	vmask = 0;
	vrmask = 0;

	// Sort the CSE table according to desirability of allocating
	// a register.
	qsort(CSETable,csendx,sizeof(CSE),CSECmp);

	// Initialize to no allocated registers
	for (csecnt = 0; csecnt < csendx; csecnt++)
		CSETable[csecnt].reg = -1;

	// Make multiple passes over the CSE table in order to use
	// up all temporary registers. Allocates on the progressively
	// less desirable.
	reg = AllocateRegisters1();
	vreg = AllocateVectorRegisters1();
	if (reg < regLastRegvar)
		reg = FinalAllocateRegisters(reg);
	if (vreg < 18)
		vreg = FinalAllocateVectorRegisters(vreg);

	// Generate bit masks of allocated registers
	for (csecnt = 0; csecnt < csendx; csecnt++) {
		csp = &CSETable[csecnt];
		if (csp->exp->etype==bt_vector) {
			if( csp->reg != -1 )
    		{
    			vrmask = vrmask | (1LL << (63 - csp->reg));
    			vmask = vmask | (1LL << csp->reg);
    		}
		}
		else {
			if( csp->reg != -1 )
    		{
    			rmask = rmask | (1LL << (63 - csp->reg));
    			mask = mask | (1LL << csp->reg);
    		}
		}
	}

	DumpCSETable();

	// Push temporaries on the stack.
	if( mask != 0 ) {
		cnt = 0;
		if (!cpu.SupportsPush)
			GenerateTriadic(op_sub,0,makereg(regSP),makereg(regSP),make_immed(popcnt(mask)*8));
		for (nn = 0; nn < 64; nn++) {
			if (rmask & (0x8000000000000000ULL >> nn)) {
				if (!cpu.SupportsPush)
					GenerateDiadic(op_sw,0,makereg(nn),make_indexed(cnt,regSP));
				else
					GenerateMonadic(op_push,0,makereg(nn&31));
				cnt+=sizeOfWord;
			}
		}
	}

    save_mask = mask;
    fpsave_mask = fpmask;
    csp = olist;

	// Initialize temporaries
	for (csecnt = 0; csecnt < csendx; csecnt++) {
		csp = &CSETable[csecnt];
        if( csp->reg != -1 )
        {               // see if preload needed
            exptr = csp->exp;
            if( !IsLValue(exptr) || (exptr->p[0]->i > 0) || (exptr->nodetype==en_struct_ref))
            {
                initstack();
				{
                    ap = GenerateExpression(exptr,F_REG|F_IMMED|F_MEM,sizeOfWord);
    				ap2 = makereg(csp->reg);
    				if (ap->mode==am_immed)
                        GenLdi(ap2,ap);
    				else if (ap->mode==am_reg)
    					GenerateDiadic(op_mov,0,ap2,ap);
    				else {
    					size = GetNaturalSize(exptr);
    					ap->isUnsigned = exptr->isUnsigned;
    					GenLoad(ap2,ap,size,size);
    				}
                }
                ReleaseTempReg(ap);
            }
        }
    }
	return (popcnt(mask));
}


AMODE *GenExpr(ENODE *node)
{
	AMODE *ap1,*ap2,*ap3,*ap4;
	int lab0, lab1;
	int size;
	int op;

    lab0 = nextlabel++;
    lab1 = nextlabel++;

	switch(node->nodetype) {
	case en_eq:		op = op_seq;	break;
	case en_ne:		op = op_sne;	break;
	case en_lt:		op = op_slt;	break;
	case en_ult:	op = op_sltu;	break;
	case en_le:		op = op_sle;	break;
	case en_ule:	op = op_sleu;	break;
	case en_gt:		op = op_sgt;	break;
	case en_ugt:	op = op_sgtu;	break;
	case en_ge:		op = op_sge;	break;
	case en_uge:	op = op_sgeu;	break;
	case en_veq:
		size = GetNaturalSize(node);
		ap3 = GetTempVectorRegister();         
		ap1 = GenerateExpression(node->p[0],F_REG,size);
		ap2 = GenerateExpression(node->p[1],F_REG,size);
		GenerateTriadic(op_vseq,0,ap3,ap1,ap2);
		ReleaseTempReg(ap2);
		ReleaseTempReg(ap1);
		return (ap3);
	case en_vne:
		size = GetNaturalSize(node);
		ap3 = GetTempVectorRegister();         
		ap1 = GenerateExpression(node->p[0],F_REG,size);
		ap2 = GenerateExpression(node->p[1],F_REG,size);
		GenerateTriadic(op_vsne,0,ap3,ap1,ap2);
		ReleaseTempReg(ap2);
		ReleaseTempReg(ap1);
		return (ap3);
	case en_vlt:
		size = GetNaturalSize(node);
		ap3 = GetTempVectorRegister();         
		ap1 = GenerateExpression(node->p[0],F_REG,size);
		ap2 = GenerateExpression(node->p[1],F_REG,size);
		GenerateTriadic(op_vslt,0,ap3,ap1,ap2);
		ReleaseTempReg(ap2);
		ReleaseTempReg(ap1);
		return (ap3);
	case en_vle:
		size = GetNaturalSize(node);
		ap3 = GetTempVectorRegister();         
		ap1 = GenerateExpression(node->p[0],F_REG,size);
		ap2 = GenerateExpression(node->p[1],F_REG,size);
		GenerateTriadic(op_vsle,0,ap3,ap1,ap2);
		ReleaseTempReg(ap2);
		ReleaseTempReg(ap1);
		return (ap3);
	case en_vgt:
		size = GetNaturalSize(node);
		ap3 = GetTempVectorRegister();         
		ap1 = GenerateExpression(node->p[0],F_REG,size);
		ap2 = GenerateExpression(node->p[1],F_REG,size);
		GenerateTriadic(op_vsgt,0,ap3,ap1,ap2);
		ReleaseTempReg(ap2);
		ReleaseTempReg(ap1);
		return (ap3);
	case en_vge:
		size = GetNaturalSize(node);
		ap3 = GetTempVectorRegister();         
		ap1 = GenerateExpression(node->p[0],F_REG,size);
		ap2 = GenerateExpression(node->p[1],F_REG,size);
		GenerateTriadic(op_vsge,0,ap3,ap1,ap2);
		ReleaseTempReg(ap2);
		ReleaseTempReg(ap1);
		return (ap3);
	default:	// en_land, en_lor
		//ap1 = GetTempRegister();
		//ap2 = GenerateExpression(node,F_REG,8);
		//GenerateDiadic(op_redor,0,ap1,ap2);
		//ReleaseTempReg(ap2);
		GenerateFalseJump(node,lab0,0);
		ap1 = GetTempRegister();
		GenerateDiadic(op_ldi,0,ap1,make_immed(1));
		GenerateMonadic(op_bra,0,make_label(lab1));
		GenerateLabel(lab0);
		GenerateDiadic(op_ldi,0,ap1,make_immed(0));
		GenerateLabel(lab1);
		return ap1;
	}

	switch(node->nodetype) {
	case en_eq:
		size = GetNaturalSize(node);
		ap3 = GetTempRegister();         
		ap1 = GenerateExpression(node->p[0],F_REG,size);
		ap2 = GenerateExpression(node->p[1],F_REG|F_IMMED,size);
		GenerateTriadic(op_cmp,0,ap3,ap1,ap2);
		ReleaseTempRegister(ap2);
		ReleaseTempRegister(ap1);
		GenerateDiadic(op_not,0,ap3,ap3);
		return (ap3);
	case en_ne:
		size = GetNaturalSize(node);
		ap3 = GetTempRegister();         
		ap1 = GenerateExpression(node->p[0],F_REG,size);
		ap2 = GenerateExpression(node->p[1],F_REG|F_IMMED,size);
		GenerateTriadic(op_cmp,0,ap3,ap1,ap2);
		ReleaseTempRegister(ap2);
		ReleaseTempRegister(ap1);
		return (ap3);
	case en_lt:
		size = GetNaturalSize(node);
		ap3 = GetTempRegister();         
		ap1 = GenerateExpression(node->p[0],F_REG,size);
		ap2 = GenerateExpression(node->p[1],F_REG|F_IMMED,size);
		GenerateTriadic(op_cmp,0,ap3,ap1,ap2);
		ReleaseTempRegister(ap2);
		ReleaseTempRegister(ap1);
		GenerateDiadic(op_slt,0,ap3,ap3);
		return (ap3);
	case en_le:
		size = GetNaturalSize(node);
		ap3 = GetTempRegister();         
		ap1 = GenerateExpression(node->p[0],F_REG,size);
		ap2 = GenerateExpression(node->p[1],F_REG|F_IMMED,size);
		GenerateTriadic(op_cmp,0,ap3,ap1,ap2);
		ReleaseTempRegister(ap2);
		ReleaseTempRegister(ap1);
		GenerateDiadic(op_sle,0,ap3,ap3);
		return (ap3);
	case en_gt:
		size = GetNaturalSize(node);
		ap3 = GetTempRegister();         
		ap1 = GenerateExpression(node->p[0],F_REG,size);
		ap2 = GenerateExpression(node->p[1],F_REG|F_IMMED,size);
		GenerateTriadic(op_cmp,0,ap3,ap1,ap2);
		ReleaseTempRegister(ap2);
		ReleaseTempRegister(ap1);
		GenerateDiadic(op_sgt,0,ap3,ap3);
		return (ap3);
	case en_ge:
		size = GetNaturalSize(node);
		ap3 = GetTempRegister();         
		ap1 = GenerateExpression(node->p[0],F_REG,size);
		ap2 = GenerateExpression(node->p[1],F_REG|F_IMMED,size);
		GenerateTriadic(op_cmp,0,ap3,ap1,ap2);
		ReleaseTempRegister(ap2);
		ReleaseTempRegister(ap1);
		GenerateDiadic(op_sge,0,ap3,ap3);
		return (ap3);
	case en_ult:
		size = GetNaturalSize(node);
		ap3 = GetTempRegister();         
		ap1 = GenerateExpression(node->p[0],F_REG,size);
		ap2 = GenerateExpression(node->p[1],F_REG|F_IMMED,size);
		GenerateTriadic(op_cmpu,0,ap3,ap1,ap2);
		ReleaseTempRegister(ap2);
		ReleaseTempRegister(ap1);
		GenerateDiadic(op_slt,0,ap3,ap3);
		return (ap3);
	case en_ule:
		size = GetNaturalSize(node);
		ap3 = GetTempRegister();         
		ap1 = GenerateExpression(node->p[0],F_REG,size);
		ap2 = GenerateExpression(node->p[1],F_REG|F_IMMED,size);
		GenerateTriadic(op_cmpu,0,ap3,ap1,ap2);
		ReleaseTempRegister(ap2);
		ReleaseTempRegister(ap1);
		GenerateDiadic(op_sle,0,ap3,ap3);
		return (ap3);
	case en_ugt:
		size = GetNaturalSize(node);
		ap3 = GetTempRegister();         
		ap1 = GenerateExpression(node->p[0],F_REG,size);
		ap2 = GenerateExpression(node->p[1],F_REG|F_IMMED,size);
		GenerateTriadic(op_cmpu,0,ap3,ap1,ap2);
		ReleaseTempRegister(ap2);
		ReleaseTempRegister(ap1);
		GenerateDiadic(op_sgt,0,ap3,ap3);
		return (ap3);
	case en_uge:
		size = GetNaturalSize(node);
		ap3 = GetTempRegister();         
		ap1 = GenerateExpression(node->p[0],F_REG,size);
		ap2 = GenerateExpression(node->p[1],F_REG|F_IMMED,size);
		GenerateTriadic(op_cmpu,0,ap3,ap1,ap2);
		ReleaseTempRegister(ap2);
		ReleaseTempRegister(ap1);
		GenerateDiadic(op_sge,0,ap3,ap3);
		return (ap3);
/*
	case en_ne:
	case en_lt:
	case en_ult:
	case en_gt:
	case en_ugt:
	case en_le:
	case en_ule:
	case en_ge:
	case en_uge:
		size = GetNaturalSize(node);
		ap1 = GenerateExpression(node->p[0],F_REG, size);
		ap2 = GenerateExpression(node->p[1],F_REG|F_IMMED,size);
		GenerateTriadic(op,0,ap1,ap1,ap2);
		ReleaseTempRegister(ap2);
		return ap1;
*/
	case en_chk:
		size = GetNaturalSize(node);
        ap4 = GetTempRegister();         
		ap1 = GenerateExpression(node->p[0],F_REG,size);
		ap2 = GenerateExpression(node->p[1],F_REG,size);
		ap3 = GenerateExpression(node->p[2],F_REG|F_IMM0,size);
		if (ap3->mode == am_immed) {  // must be a zero
		   ap3->mode = am_reg;
		   ap3->preg = 0;
        }
   		Generate4adic(op_chk,0,ap4,ap1,ap2,ap3);
        ReleaseTempRegister(ap3);
        ReleaseTempRegister(ap2);
        ReleaseTempRegister(ap1);
        return ap4;
	}
	size = GetNaturalSize(node);
    ap3 = GetTempRegister();         
	ap1 = GenerateExpression(node->p[0],F_REG,size);
	ap2 = GenerateExpression(node->p[1],F_REG|F_IMMED,size);
	GenerateTriadic(op,0,ap3,ap1,ap2);
    ReleaseTempRegister(ap2);
    ReleaseTempRegister(ap1);
    return ap3;
	/*
    GenerateFalseJump(node,lab0,0);
    ap1 = GetTempRegister();
    GenerateDiadic(op_ld,0,ap1,make_immed(1));
    GenerateMonadic(op_bra,0,make_label(lab1));
    GenerateLabel(lab0);
    GenerateDiadic(op_ld,0,ap1,make_immed(0));
    GenerateLabel(lab1);
    return ap1;
	*/
}

void GenCompareI(AMODE *ap3, AMODE *ap1, AMODE *ap2, int su)
{
	AMODE *ap4;

	if (ap2->offset->i < -32768LL || ap2->offset->i > 32767LL) {
		ap4 = GetTempRegister();
		GenerateTriadic(op_or,0,ap4,makereg(regZero),make_immed(ap2->offset->i & 0xFFFFLL));
		if (ap2->offset->i & 0xFFFF0000LL)
			GenerateDiadic(op_orq1,0,ap4,make_immed((ap2->offset->i >> 16) & 0xFFFFLL));
		if (ap2->offset->i & 0xFFFF00000000LL)
			GenerateDiadic(op_orq2,0,ap4,make_immed((ap2->offset->i >> 32) & 0xFFFFLL));
		if (ap2->offset->i & 0xFFFF000000000000LL)
			GenerateDiadic(op_orq3,0,ap4,make_immed((ap2->offset->i >> 48) & 0xFFFFLL));
		GenerateTriadic(op_cmp,0,ap3,ap1,ap4);
		ReleaseTempReg(ap4);
	}
	else
		GenerateTriadic(su ? op_cmp : op_cmpu,0,ap3,ap1,ap2);
}

void GenerateCmp(ENODE *node, int op, int label, int predreg, unsigned int prediction)
{
	int size, sz;
	AMODE *ap1, *ap2, *ap3;

	size = GetNaturalSize(node);
    if (op==op_flt || op==op_fle || op==op_fgt || op==op_fge || op==op_feq || op==op_fne) {
    	ap1 = GenerateExpression(node->p[0],F_REG,size);
	    ap2 = GenerateExpression(node->p[1],F_REG|F_IMM0,size);
    }
    else {
    	ap1 = GenerateExpression(node->p[0],F_REG, size);
	    ap2 = GenerateExpression(node->p[1],F_REG|F_IMMED,size);
    }
	/*
	// Optimize CMP to zero and branch into plain branch, this works only for
	// signed relational compares.
	if (ap2->mode == am_immed && ap2->offset->i==0 && (op==op_eq || op==op_ne || op==op_lt || op==op_le || op==op_gt || op==op_ge)) {
    	switch(op)
    	{
    	case op_eq:	op = op_beq; break;
    	case op_ne:	op = op_bne; break;
    	case op_lt: op = op_blt; break;
    	case op_le: op = op_ble; break;
    	case op_gt: op = op_bgt; break;
    	case op_ge: op = op_bge; break;
    	}
    	ReleaseTempReg(ap3);
		ReleaseTempReg(ap2);
		ReleaseTempReg(ap1);
		GenerateDiadic(op,0,ap1,make_clabel(label));
		return;
	}
	*/
	/*
	if (op==op_ltu || op==op_leu || op==op_gtu || op==op_geu)
 	    GenerateTriadic(op_cmpu,0,ap3,ap1,ap2);
    else if (op==op_flt || op==op_fle || op==op_fgt || op==op_fge || op==op_feq || op==op_fne)
        GenerateTriadic(op_fdcmp,0,ap3,ap1,ap2);
	else 
 	    GenerateTriadic(op_cmp,0,ap3,ap1,ap2);
	*/
	sz = 0;
	switch(op)
	{
	case op_eq:	op = op_beq; break;
	case op_ne:	op = op_bne; break;
	case op_lt: op = op_blt; break;
	case op_le: op = op_ble; break;
	case op_gt: op = op_bgt; break;
	case op_ge: op = op_bge; break;
	case op_ltu: op = op_bltu; break;
	case op_leu: op = op_bleu; break;
	case op_gtu: op = op_bgtu; break;
	case op_geu: op = op_bgeu; break;
	case op_feq:	op = op_fbeq; sz = 'd'; break;
	case op_fne:	op = op_fbne; sz = 'd'; break;
	case op_flt:	op = op_fblt; sz = 'd'; break;
	case op_fle:	op = op_fble; sz = 'd'; break;
	case op_fgt:	op = op_fbgt; sz = 'd'; break;
	case op_fge:	op = op_fbge; sz = 'd'; break;
	/*
		GenerateTriadic(op_fcmp,'q',ap3,ap1,ap2);
		GenerateTriadic(op_bbs,0,ap3,make_immed(0),make_clabel(label));
		goto xit;
	case op_fne:
		GenerateTriadic(op_fcmp,'q',ap3,ap1,ap2);
		GenerateTriadic(op_bbc,0,ap3,make_immed(0),make_clabel(label));
		goto xit;
	case op_flt:
		GenerateTriadic(op_fcmp,'q',ap3,ap1,ap2);
		GenerateTriadic(op_bbs,0,ap3,make_immed(1),make_clabel(label));
		goto xit;
	case op_fle:
		GenerateTriadic(op_fcmp,'q',ap3,ap1,ap2);
		GenerateTriadic(op_bbs,0,ap3,make_immed(2),make_clabel(label));
		goto xit;
	case op_fgt:
		GenerateTriadic(op_fcmp,'q',ap3,ap1,ap2);
		GenerateTriadic(op_bbc,0,ap3,make_immed(2),make_clabel(label));
		goto xit;
	case op_fge:
		GenerateTriadic(op_fcmp,'q',ap3,ap1,ap2);
		GenerateTriadic(op_bbc,0,ap3,make_immed(1),make_clabel(label));
		goto xit;
	*/
	}
	if (op==op_fbne || op==op_fbeq || op==op_fblt || op==op_fble || op==op_fbgt || op==op_fbge) {
		switch(op) {
		case op_fbne:
			if (ap2->mode==am_immed) {
				ap3 = GetTempRegister();
				GenerateDiadic(op_ldi,0,ap3,ap2);
				ReleaseTempRegister(ap3);
				GenerateTriadic(op_fbne,sz,ap1,ap3,make_clabel(label));
			}
			else
				GenerateTriadic(op_fbne,sz,ap1,ap2,make_clabel(label));
			break;
		case op_fbeq:
			if (ap2->mode==am_immed) {
				ap3 = GetTempRegister();
				GenerateDiadic(op_ldi,0,ap3,ap2);
				ReleaseTempRegister(ap3);
				GenerateTriadic(op_fbeq,sz,ap1,ap3,make_clabel(label));
			}
			else
				GenerateTriadic(op_fbeq,sz,ap1,ap2,make_clabel(label));
			break;
		case op_fblt:
			if (ap2->mode==am_immed) {
				ap3 = GetTempRegister();
				GenerateDiadic(op_ldi,0,ap3,ap2);
				ReleaseTempRegister(ap3);
				GenerateTriadic(op_fblt,sz,ap1,ap3,make_clabel(label));
			}
			else
				GenerateTriadic(op_fblt,sz,ap1,ap2,make_clabel(label));
			break;
		case op_fble:
			if (ap2->mode==am_immed) {
				ap3 = GetTempRegister();
				GenerateDiadic(op_ldi,0,ap3,ap2);
				ReleaseTempRegister(ap3);
				GenerateTriadic(op_fbge,sz,ap3,ap1,make_clabel(label));
			}
			else
				GenerateTriadic(op_fbge,sz,ap2,ap1,make_clabel(label));
			break;
		case op_fbgt:
			if (ap2->mode==am_immed) {
				ap3 = GetTempRegister();
				GenerateDiadic(op_ldi,0,ap3,ap2);
				ReleaseTempRegister(ap3);
				GenerateTriadic(op_fblt,sz,ap3,ap1,make_clabel(label));
			}
			else
				GenerateTriadic(op_fblt,sz,ap2,ap1,make_clabel(label));
			break;
		case op_fbge:
			if (ap2->mode==am_immed) {
				ap3 = GetTempRegister();
				GenerateDiadic(op_ldi,0,ap3,ap2);
				ReleaseTempRegister(ap3);
				GenerateTriadic(op_fbge,sz,ap1,ap3,make_clabel(label));
			}
			else
				GenerateTriadic(op_fbge,sz,ap1,ap2,make_clabel(label));
			break;
		}
	}
	else {
		switch(op) {
		case op_beq:
			if (ap2->mode==am_immed && ap2->offset->nodetype==en_icon && ap2->offset->i >= -256 && ap2->offset->i <=255) {
				GenerateTriadic(op_beqi,0,ap1,ap2,make_clabel(label));
			}
			else if (ap2->mode==am_immed) {
				ap3 = GetTempRegister();
				GenCompareI(ap3,ap1,ap2,1);
				ReleaseTempRegister(ap3);
				Generate4adic(op_beq,0,ap3,makereg(0),make_clabel(label),make_immed(prediction));
			}
			else
				Generate4adic(op_beq,0,ap1,ap2,make_clabel(label), make_immed(prediction));
			break;
		case op_bne:
			if (ap2->mode==am_immed) {
				if (ap2->offset->i == 0)
					GenerateTriadic(op_bne,0,ap1,makereg(0),make_clabel(label));
				else {
					ap3 = GetTempRegister();
					GenCompareI(ap3,ap1,ap2,1);
					ReleaseTempRegister(ap3);
					Generate4adic(op_bne,0,ap3,makereg(0),make_clabel(label), make_immed(prediction));
				}
			}
			else {
				Generate4adic(op_bne,0,ap1,ap2,make_clabel(label), make_immed(prediction));
			}
			break;
		case op_blt:
			if (ap2->mode==am_immed) {
				if (ap2->offset->i == 0)
					GenerateTriadic(op_blt,0,ap1,makereg(0),make_clabel(label));
				else {
					ap3 = GetTempRegister();
					GenCompareI(ap3,ap1,ap2,1);
					ReleaseTempRegister(ap3);
					Generate4adic(op_blt,0,ap3,makereg(0),make_clabel(label), make_immed(prediction));
				}
			}
			else
				Generate4adic(op_blt,0,ap1,ap2,make_clabel(label), make_immed(prediction));
			break;
		case op_ble:
			if (ap2->mode==am_immed) {
				if (ap2->offset->i == 0)
					GenerateTriadic(op_bge,0,makereg(0),ap1,make_clabel(label));
				else {
					ap3 = GetTempRegister();
					GenCompareI(ap3,ap1,ap2,1);
					ReleaseTempRegister(ap3);
					Generate4adic(op_bge,0,makereg(0),ap3,make_clabel(label), make_immed(prediction));
				}
			}
			else
				Generate4adic(op_bge,0,ap2,ap1,make_clabel(label), make_immed(prediction));
			break;
		case op_bgt:
			if (ap2->mode==am_immed) {
				if (ap2->offset->i == 0)
					GenerateTriadic(op_blt,0,makereg(0),ap1,make_clabel(label));
				else {
					ap3 = GetTempRegister();
					GenCompareI(ap3,ap1,ap2,1);
					ReleaseTempRegister(ap3);
					Generate4adic(op_blt,0,makereg(0),ap3,make_clabel(label), make_immed(prediction));
				}
			}
			else
				Generate4adic(op_blt,0,ap2,ap1,make_clabel(label), make_immed(prediction));
			break;
		case op_bge:
			if (ap2->mode==am_immed) {
				if (ap2->offset->i==0) {
					GenerateTriadic(op_bge,0,ap1,makereg(0),make_clabel(label));
				}
				else {
					ap3 = GetTempRegister();
					GenCompareI(ap3,ap1,ap2,1);
					ReleaseTempRegister(ap3);
					Generate4adic(op_bge,0,ap3,makereg(0),make_clabel(label), make_immed(prediction));
				}
			}
			else
				Generate4adic(op_bge,0,ap1,ap2,make_clabel(label), make_immed(prediction));
			break;
		case op_bltu:
			if (ap2->mode==am_immed) {
				// Don't generate any code if testing against unsigned zero.
				// An unsigned number can't be less than zero so the branch will
				// always be false. Spit out a warning, its probably coded wrong.
				if (ap2->offset->i == 0)
					error(ERR_UBLTZ);	//GenerateTriadic(op_bltu,0,ap1,makereg(0),make_clabel(label));
				else {
					ap3 = GetTempRegister();
					GenCompareI(ap3,ap1,ap2,0);
					ReleaseTempRegister(ap3);
					Generate4adic(op_blt,0,ap3,makereg(0),make_clabel(label), make_immed(prediction));
				}
			}
			else
				Generate4adic(op_bltu,0,ap1,ap2,make_clabel(label), make_immed(prediction));
			break;
		case op_bleu:
			if (ap2->mode==am_immed) {
				if (ap2->offset->i == 0)
					GenerateTriadic(op_bgeu,0,makereg(0),ap1,make_clabel(label));
				else {
					ap3 = GetTempRegister();
					GenCompareI(ap3,ap1,ap2,0);
					ReleaseTempRegister(ap3);
					Generate4adic(op_bge,0,makereg(0),ap3,make_clabel(label), make_immed(prediction));
				}
			}
			else
				Generate4adic(op_bgeu,0,ap2,ap1,make_clabel(label), make_immed(prediction));
			break;
		case op_bgtu:
			if (ap2->mode==am_immed) {
				if (ap2->offset->i == 0)
					GenerateTriadic(op_bltu,0,makereg(0),ap1,make_clabel(label));
				else {
					ap3 = GetTempRegister();
					GenCompareI(ap3,ap1,ap2,0);
					ReleaseTempRegister(ap3);
					Generate4adic(op_blt,0,makereg(0),ap3,make_clabel(label), make_immed(prediction));
				}
			}
			else
				Generate4adic(op_bltu,0,ap2,ap1,make_clabel(label), make_immed(prediction));
			break;
		case op_bgeu:
			if (ap2->mode==am_immed) {
				if (ap2->offset->i == 0) {
					// This branch is always true
					error(ERR_UBGEQ);
					Generate4adic(op_bgeu,0,ap1,makereg(0),make_clabel(label), make_immed(prediction));
				}
				else {
					ap3 = GetTempRegister();
					GenCompareI(ap3,ap1,ap2,0);
					ReleaseTempRegister(ap3);
					Generate4adic(op_bge,0,ap3,makereg(0),make_clabel(label), make_immed(prediction));
				}
			}
			else
				GenerateTriadic(op_bgeu,0,ap1,ap2,make_clabel(label));
			break;
		}
		//GenerateTriadic(op,sz,ap1,ap2,make_clabel(label));
	}
   	ReleaseTempReg(ap2);
   	ReleaseTempReg(ap1);
}


static void GenerateDefaultCatch(SYM *sym)
{
	GenerateLabel(throwlab);
	if (sym->IsLeaf){
		if (sym->DoesThrow) {
			GenerateDiadic(op_lw,0,makereg(regLR),make_indexed(sizeOfWord,regBP));		// load throw return address from stack into LR
			GenerateDiadic(op_sw,0,makereg(regLR),make_indexed(sizeOfWord*2,regBP));		// and store it back (so it can be loaded with the lm)
			GenerateDiadic(op_bra,0,make_label(retlab),NULL);				// goto regular return cleanup code
		}
	}
	else {
		GenerateDiadic(op_lw,0,makereg(regLR),make_indexed(sizeOfWord,regBP));		// load throw return address from stack into LR
		GenerateDiadic(op_sw,0,makereg(regLR),make_indexed(sizeOfWord*2,regBP));		// and store it back (so it can be loaded with the lm)
		GenerateDiadic(op_bra,0,make_label(retlab),NULL);				// goto regular return cleanup code
	}
}

// Generate a function body.
//
void GenerateFunction(SYM *sym)
{
	AMODE *ap;
    int defcatch;
	int nn;
	Statement *stmt = sym->stmt;
	int lab0;
	int o_throwlab, o_retlab, o_contlab, o_breaklab;

	o_throwlab = throwlab;
	o_retlab = retlab;
	o_contlab = contlab;
	o_breaklab = breaklab;

	throwlab = retlab = contlab = breaklab = -1;
	lastsph = 0;
	memset(semaphores,0,sizeof(semaphores));
	throwlab = nextlabel++;
	defcatch = nextlabel++;
	lab0 = nextlabel++;
	//GenerateZeradic(op_calltgt);
	while( lc_auto & 1 )	/* round frame size to word */
		++lc_auto;
	if (sym->IsInterrupt) {
       if (sym->stkname)
           GenerateDiadic(op_lea,0,makereg(SP),make_string(sym->stkname));
	   for (nn = 1 + (sym->tp->GetBtp()->type!=bt_void ? 1 : 0); nn < 31; nn++)
		   GenerateMonadic(op_push,0,makereg(nn));
	}
	// The prolog code can't be optimized because it'll run *before* any variables
	// assigned to registers are available. About all we can do here is constant
	// optimizations.
	if (sym->prolog) {
		scan(sym->prolog);
	    sym->prolog->Generate();
	}
	if (!sym->IsNocall) {
		/*
		// For a leaf routine don't bother to store the link register.
		if (sym->IsLeaf) {
    		//GenerateTriadic(op_addi,0,makereg(regSP),makereg(regSP),make_immed(-4));
			if (exceptions)
				GenerateMonadic(op_push, 0, makereg(regXLR));
			GenerateMonadic(op_push,0,makereg(regBP));
        }
		else
		*/
		{
			if (exceptions) {
				if (!sym->IsLeaf || sym->DoesThrow)
					GenerateMonadic(op_push, 0, makereg(regXLR));
			}
			//GenerateMonadic(op_push, 0, makereg(regBP));
			ap = make_label(throwlab);
			ap->mode = am_immed;
			if (sym->IsLeaf && !sym->DoesThrow)
				;
			else if (exceptions)
				GenLdi(makereg(regXLR),ap);
		}
		// The stack doesn't need to be linked if there is no stack space in use and there
		// are no parameters passed to the function. Since function parameters are
		// referenced to the BP register the stack needs to be linked if there are any.
		// Stack link/unlink is optimized away by the peephole optimizer if they aren't
		// needed. So they are just always spit out here.
//			snprintf(buf, sizeof(buf), "#-%sSTKSIZE_-8",sym->mangledName->c_str());
		if (cpu.SupportsLink)
			GenerateDiadic(op_link,0,makereg(regBP),make_immed(sym->stkspace));//make_string(my_strdup(buf)));
		else {
			// Alternate code for lacking a link instruction
			GenerateMonadic(op_push,0,makereg(regBP));
			GenerateDiadic(op_mov,0,makereg(regBP),makereg(regSP));
			GenerateTriadic(op_sub,0,makereg(regSP),makereg(regSP),make_immed(sym->stkspace));
		}
	}
	if (optimize)
		opt1(stmt);
    stmt->Generate();
    GenerateReturn(nullptr);
	if (exceptions && sym->IsInline)
		GenerateMonadic(op_bra,0,make_label(lab0));
	// Generate code for the hidden default catch
	if (exceptions)
		GenerateDefaultCatch(sym);
	if (exceptions && sym->IsInline)
		GenerateLabel(lab0);

	throwlab = o_throwlab;
	retlab = o_retlab;
	contlab = o_contlab;
	breaklab = o_breaklab;
}


// Unlink the stack
// For a leaf routine the link register and exception link register doesn't need to be saved/restored.

static void UnlinkStack(SYM * sym)
{
	// Alternate code for when there's no link/unlink instructions
	if (!cpu.SupportsUnlink) {
		GenerateDiadic(op_mov,0,makereg(regSP),makereg(regBP));
		GenerateMonadic(op_pop,0,makereg(regBP));
	}
	else
		GenerateMonadic(op_unlk,0,makereg(regBP));
	if (exceptions) {
		if (!sym->IsLeaf || sym->DoesThrow)
			GenerateMonadic(op_pop,0,makereg(regXLR));
	}
}


// Restore registers used as register variables.

static void RestoreRegisterVars()
{
	int cnt2, cnt;
	int nn;

	if( save_mask != 0 ) {
		cnt2 = cnt = bitsset(save_mask)*sizeOfWord;
		for (nn = 31; nn >=1 ; nn--) {
			if (save_mask & (1LL << nn)) {
				GenerateMonadic(op_pop,0,makereg(nn));
				cnt -= sizeOfWord;
			}
		}
	}
}

// Generate a return statement.
//
void GenerateReturn(Statement *stmt)
{
	AMODE *ap;
	int nn;
	int cnt,cnt2;
	int toAdd;
	SYM *sym = currentFn;
	SYM *p;

  // Generate the return expression and force the result into r1.
  if( stmt != NULL && stmt->exp != NULL )
  {
		initstack();
		if (sym->tp->GetBtp() && sym->tp->GetBtp()->IsFloatType())
			ap = GenerateExpression(stmt->exp,F_REG,sizeOfFP);
		else
			ap = GenerateExpression(stmt->exp,F_REG|F_IMMED,sizeOfWord);
		GenerateMonadic(op_hint,0,make_immed(2));
		if (ap->mode == am_immed)
		    GenLdi(makereg(1),ap);
		else if (ap->mode == am_reg) {
            if (sym->tp->GetBtp() && (sym->tp->GetBtp()->type==bt_struct || sym->tp->GetBtp()->type==bt_union)) {
				p = sym->params.Find("_pHiddenStructPtr",false);
				if (p) {
					if (p->IsRegister)
						GenerateDiadic(op_mov,0,makereg(1),makereg(p->reg));
					else
						GenerateDiadic(op_lw,0,makereg(1),make_indexed(p->value.i,regBP));
					GenerateMonadic(op_push,0,make_immed(sym->tp->GetBtp()->size));
					GenerateMonadic(op_push,0,ap);
					GenerateMonadic(op_push,0,makereg(1));
					GenerateMonadic(op_call,0,make_string("_memcpy"));
					GenerateTriadic(op_add,0,makereg(regSP),makereg(regSP),make_immed(sizeOfWord*3));
				}
				else {
					// ToDo compiler error
				}
            }
            else {
				if (sym->tp->GetBtp()->IsVectorType())
					GenerateDiadic(op_mov, 0, makevreg(1),ap);
				else
					GenerateDiadic(op_mov, 0, makereg(1),ap);
			}
        }
		else if (ap->mode == am_fpreg)
			GenerateDiadic(op_mov, 0, makereg(1),ap);
		else if (ap->type==stddouble.GetIndex()) {
			GenerateDiadic(op_lw,0,makereg(1),ap);
		}
		else {
			if (sym->tp->GetBtp()->IsVectorType())
				GenLoad(makevreg(1),ap,sizeOfWord,sizeOfWord);
			else
				GenLoad(makereg(1),ap,sizeOfWord,sizeOfWord);
		}
		ReleaseTempRegister(ap);
	}

	// Generate the return code only once. Branch to the return code for all returns.
	if (retlab != -1) {
		GenerateMonadic(op_bra,0,make_label(retlab));
		return;
	}
	retlab = nextlabel++;
	GenerateLabel(retlab);
	// Unlock any semaphores that may have been set
	for (nn = lastsph - 1; nn >= 0; nn--)
		GenerateDiadic(op_sb,0,makereg(0),make_string(semaphores[nn]));
		
	// Restore fp registers used as register variables.
	if( fpsave_mask != 0 ) {
		cnt2 = cnt = (bitsset(fpsave_mask)-1)*sizeOfFP;
		for (nn = 31; nn >=1 ; nn--) {
			if (fpsave_mask & (1LL << nn)) {
				GenerateDiadic(op_lw,0,makereg(nn),make_indexed(cnt2-cnt,regSP));
				cnt -= sizeOfWord;
			}
		}
		GenerateTriadic(op_add,0,makereg(regSP),makereg(regSP),make_immed(cnt2+sizeOfFP));
	}
	RestoreRegisterVars();
    if (sym->IsNocall) {
		if (sym->epilog) {
			sym->epilog->Generate();
			return;
		}
		return;
    }
	UnlinkStack(sym);
	toAdd = sizeOfWord;

	if (sym->epilog) {
		sym->epilog->Generate();
		return;
	}
        
	// Generate the return instruction. For the Pascal calling convention pop the parameters
	// from the stack.
	if (sym->IsInterrupt) {
		for (nn = 30; nn > (sym->tp->GetBtp()->type!=bt_void ? 1 : 0); nn--)
			GenerateMonadic(op_pop,0,makereg(nn));
		GenerateZeradic(op_rti);
		return;
	}

	// If Pascal calling convention remove parameters from stack by adding to stack pointer
	// based on the number of parameters. However if a non-auto register parameter is
	// present, then don't add to the stack pointer for it. (Remove the previous add effect).
	if (sym->IsPascal) {
		TypeArray *ta;
		int nn;
		ta = sym->GetProtoTypes();
		for (nn = 0; nn < ta->length; nn++) {
			switch(ta->types[nn]) {
			case bt_float:
			case bt_quad:
				if (ta->preg[nn] && (ta->preg[nn] & 0x8000)==0)
					;
				else
					toAdd += sizeOfFPQ;
				break;
			case bt_double:
				if (ta->preg[nn] && (ta->preg[nn] & 0x8000)==0)
					;
				else
					toAdd += sizeOfFPD;
				break;
			case bt_triple:
				if (ta->preg[nn] && (ta->preg[nn] & 0x8000)==0)
					;
				else
					toAdd += sizeOfFPT;
				break;
			default:
				if (ta->preg[nn] && (ta->preg[nn] & 0x8000)==0)
					;
				else
					toAdd += sizeOfWord;
			}
		}
	}
	if (!sym->IsInline) {
		if (toAdd != 8)
			GenerateMonadic(op_ret,0,make_immed(toAdd));
		else
			GenerateZeradic(op_ret);
	}
}

static int round4(int n)
{
    while(n & 3) n++;
    return (n);
};

static void SaveTemporaries(SYM *sym, int *sp, int *fsp)
{
	if (sym) {
		if (sym->UsesTemps) {
			*sp = TempInvalidate();
			//*fsp = TempFPInvalidate();
		}
	}
	else {
		*sp = TempInvalidate();
		//*fsp = TempFPInvalidate();
	}
}

static void RestoreTemporaries(SYM *sym, int sp, int fsp)
{
	if (sym) {
		if (sym->UsesTemps) {
			//TempFPRevalidate(fsp);
			TempRevalidate(sp);
		}
	}
	else {
		//TempFPRevalidate(fsp);
		TempRevalidate(sp);
	}
}

// Saves any registers used as parameters in the calling function.

static void SaveRegisterParameters(SYM *sym)
{
	TypeArray *ta;

	if (sym == nullptr)
		return;
	ta = sym->GetProtoTypes();
	if (ta) {
		int nn;
		for (nn = 0; nn < ta->length; nn++) {
			if (ta->preg[nn]) {
				switch(ta->types[nn]) {
				case bt_quad:	GenerateMonadic(op_push,0,makereg(ta->preg[nn]& 0x7fff)); break;
				case bt_float:	GenerateMonadic(op_push,0,makereg(ta->preg[nn]& 0x7fff)); break;
				case bt_double:	GenerateMonadic(op_push,0,makereg(ta->preg[nn]& 0x7fff)); break;
				case bt_triple:	GenerateMonadic(op_push,0,makereg(ta->preg[nn]& 0x7fff)); break;
				default:	GenerateMonadic(op_push,0,makereg(ta->preg[nn]& 0x7fff)); break;
				}
			}
		}
	}
}

static void RestoreRegisterParameters(SYM *sym)
{
	TypeArray *ta;

	if (sym == nullptr)
		return;
	ta = sym->GetProtoTypes();
	if (ta) {
		int nn;
		for (nn = ta->length - 1; nn >= 0; nn--) {
			if (ta->preg[nn]) {
				switch(ta->types[nn]) {
				case bt_quad:	GenerateMonadic(op_pop,0,makereg(ta->preg[nn]& 0x7fff)); break;
				case bt_float:	GenerateMonadic(op_pop,0,makereg(ta->preg[nn]& 0x7fff)); break;
				case bt_double:	GenerateMonadic(op_pop,0,makereg(ta->preg[nn]& 0x7fff)); break;
				case bt_triple:	GenerateMonadic(op_pop,0,makereg(ta->preg[nn]& 0x7fff)); break;
				default:	GenerateMonadic(op_pop,0,makereg(ta->preg[nn]& 0x7fff)); break;
				}
			}
		}
	}
}

// push the operand expression onto the stack.
// Structure variables are represented as an address in a register and arrive
// here as autocon nodes if on the stack. If the variable size is greater than
// 8 we assume a structure variable and we assume we have the address in a reg.
// Returns: number of stack words pushed.
//
static int GeneratePushParameter(ENODE *ep, int regno, int stkoffs)
{    
	AMODE *ap;
	int nn = 0;
	int sz;
	
	switch(ep->etype) {
	case bt_quad:	sz = sizeOfFPD; break;
	case bt_triple:	sz = sizeOfFPT; break;
	case bt_double:	sz = sizeOfFPD; break;
	case bt_float:	sz = sizeOfFPD; break;
	default:	sz = sizeOfWord; break;
	}
	if (ep->tp) {
		if (ep->tp->IsFloatType())
			ap = GenerateExpression(ep,F_REG,sizeOfFP);
		else
			ap = GenerateExpression(ep,F_REG|F_IMM0,sizeOfWord);
	}
	else if (ep->etype==bt_quad)
		ap = GenerateExpression(ep,F_REG,sz);
	else if (ep->etype==bt_double)
		ap = GenerateExpression(ep,F_REG,sz);
	else if (ep->etype==bt_triple)
		ap = GenerateExpression(ep,F_REG,sz);
	else if (ep->etype==bt_float)
		ap = GenerateExpression(ep,F_REG,sz);
	else
		ap = GenerateExpression(ep,F_REG|F_IMM0,sz);
	switch(ap->mode) {
    case am_reg:
    case am_fpreg:
    case am_immed:
/*
        nn = round8(ep->esize); 
        if (nn > 8) {// && (ep->tp->type==bt_struct || ep->tp->type==bt_union)) {           // structure or array ?
            ap2 = GetTempRegister();
            GenerateTriadic(op_subui,0,makereg(regSP),makereg(regSP),make_immed(nn));
            GenerateDiadic(op_mov, 0, ap2, makereg(regSP));
            GenerateMonadic(op_push,0,make_immed(ep->esize));
            GenerateMonadic(op_push,0,ap);
            GenerateMonadic(op_push,0,ap2);
            GenerateMonadic(op_bsr,0,make_string("memcpy_"));
            GenerateTriadic(op_addui,0,makereg(regSP),makereg(regSP),make_immed(24));
          	GenerateMonadic(op_push,0,ap2);
            ReleaseTempReg(ap2);
            nn = nn >> 3;
        }
        else {
*/
			if (regno) {
				GenerateMonadic(op_hint,0,make_immed(1));
				if (ap->mode==am_immed) {
					GenerateDiadic(op_ldi,0,makereg(regno & 0x7fff), ap);
					if (regno & 0x8000) {
						GenerateTriadic(op_sub,0,makereg(regSP),makereg(regSP),make_immed(sizeOfWord));
						nn = 1;
					}
				}
				else if (ap->mode==am_fpreg) {
					GenerateDiadic(op_mov,0,makereg(regno & 0x7fff), ap);
					if (regno & 0x8000) {
						GenerateTriadic(op_sub,0,makereg(regSP),makereg(regSP),make_immed(sz));
						nn = sz/sizeOfWord;
					}
				}
				else {
					GenerateDiadic(op_mov,0,makereg(regno & 0x7fff), ap);
					if (regno & 0x8000) {
						GenerateTriadic(op_sub,0,makereg(regSP),makereg(regSP),make_immed(sizeOfWord));
						nn = 1;
					}
				}
			}
			else {
				if (cpu.SupportsPush) {
					if (ap->mode==am_immed) {	// must have been a zero
         				GenerateMonadic(op_push,0,makereg(0));
						nn = 1;
					}
					else {
						if (ap->type=stddouble.GetIndex()) {
							GenerateMonadic(op_push,ap->FloatSize,ap);
							nn = sz/sizeOfWord;
						}
						else {
          					GenerateMonadic(op_push,0,ap);
							nn = 1;
						}
					}
				}
				else {
					if (ap->mode==am_immed) {	// must have been a zero
         				GenerateDiadic(op_sw,0,makereg(0),make_indexed(stkoffs,regSP));
						nn = 1;
					}
					else {
						if (ap->type=stddouble.GetIndex()) {
							GenerateDiadic(op_sw,0,ap,make_indexed(stkoffs,regSP));
							nn = sz/sizeOfWord;
						}
						else {
          					GenerateDiadic(op_sw,0,ap,make_indexed(stkoffs,regSP));
							nn = 1;
						}
					}
				}
			}
//        }
    	break;
    }
//	ReleaseTempReg(ap);
	return nn;
}

// push entire parameter list onto stack
//
static int GeneratePushParameterList(SYM *sym, ENODE *plist)
{
	TypeArray *ta = nullptr;
	int i,sum;
	struct ocode *ip;

	sum = 0;
	if (sym)
		ta = sym->GetProtoTypes();

	ip = peep_tail;
	if (!cpu.SupportsPush) {
		GenerateTriadic(op_sub,0,makereg(regSP),makereg(regSP),make_immed(0));
	}
	for(i = 0; plist != NULL; i++ )
    {
		sum += GeneratePushParameter(plist->p[0],ta ? ta->preg[ta->length - i - 1] : 0,sum*8);
		plist = plist->p[1];
    }
	if (!cpu.SupportsPush) {
		if (sum==0)
			MarkRemove(ip->fwd);
		else
			ip->fwd->oper3 = make_immed(sum*8);
	}
	if (ta)
		delete ta;
    return sum;
}

AMODE *GenerateFunctionCall(ENODE *node, int flags)
{ 
	AMODE *ap;
	SYM *sym;
	SYM *o_fn;
    int             i;
	int sp = 0;
	int fsp = 0;
	TypeArray *ta = nullptr;
	int64_t mask,fmask;

	sym = nullptr;

	// Call the function
	if( node->p[0]->nodetype == en_nacon || node->p[0]->nodetype == en_cnacon ) {
 		sym = gsearch(*node->p[0]->sp);
        i = 0;
		SaveTemporaries(sym, &sp, &fsp);
  /*
    	if ((sym->tp->GetBtp()->type==bt_struct || sym->tp->GetBtp()->type==bt_union) && sym->tp->GetBtp()->size > 8) {
            nn = tmpAlloc(sym->tp->GetBtp()->size) + lc_auto + round8(sym->tp->GetBtp()->size);
            GenerateMonadic(op_pea,0,make_indexed(-nn,regBP));
            i = 1;
        }
*/
		SaveRegisterParameters(sym);
        i = i + GeneratePushParameterList(sym,node->p[1]);
//		ReleaseTempRegister(ap);
		if (sym && sym->IsInline) {
			o_fn = currentFn;
			mask = save_mask;
			fmask = fpsave_mask;
			currentFn = sym;
			GenerateFunction(sym);
			currentFn = o_fn;
			fpsave_mask = fmask;
			save_mask = mask;
		}
		else
			GenerateMonadic(op_call,0,make_offset(node->p[0]));
	}
    else
    {
        i = 0;
    /*
    	if ((node->p[0]->tp->GetBtp()->type==bt_struct || node->p[0]->tp->GetBtp()->type==bt_union) && node->p[0]->tp->GetBtp()->size > 8) {
            nn = tmpAlloc(node->p[0]->tp->GetBtp()->size) + lc_auto + round8(node->p[0]->tp->GetBtp()->size);
            GenerateMonadic(op_pea,0,make_indexed(-nn,regBP));
            i = 1;
        }
     */
		ap = GenerateExpression(node->p[0],F_REG,sizeOfWord);
		if (ap->offset)
			sym = ap->offset->sym;
		SaveTemporaries(sym, &sp, &fsp);
		SaveRegisterParameters(sym);
        i = i + GeneratePushParameterList(sym,node->p[1]);
		ap->mode = am_ind;
		ap->offset = 0;
		if (sym && sym->IsInline) {
			o_fn = currentFn;
			mask = save_mask;
			fmask = fpsave_mask;
			currentFn = sym;
			GenerateFunction(sym);
			currentFn = o_fn;
			fpsave_mask = fmask;
			save_mask = mask;
		}
		else
			GenerateMonadic(op_call,0,ap);
		ReleaseTempRegister(ap);
    }
	// Pop parameters off the stack
	if (i!=0) {
		if (sym) {
			if (!sym->IsPascal)
				GenerateTriadic(op_add,0,makereg(regSP),makereg(regSP),make_immed(i * sizeOfWord));
		}
		else
			GenerateTriadic(op_add,0,makereg(regSP),makereg(regSP),make_immed(i * sizeOfWord));
	}
	RestoreRegisterParameters(sym);
	RestoreTemporaries(sym, sp, fsp);
	/*
	if (sym) {
	   if (sym->tp->type==bt_double)
           result = GetTempFPRegister();
	   else
           result = GetTempRegister();
    }
    else {
        if (node->etype==bt_double)
            result = GetTempFPRegister();
        else
            result = GetTempRegister();
    }
	*/
	if (sym && sym->tp && sym->tp->GetBtp()->IsFloatType() && (flags & F_FPREG))
		return (makereg(1));
	if (sym && sym->tp->IsVectorType())
		return (makevreg(1));
	return (makereg(1));
	/*
	else {
		if( result->preg != 1 || (flags & F_REG) == 0 ) {
			if (sym) {
				if (sym->tp->GetBtp()->type==bt_void)
					;
				else {
                    if (sym->tp->type==bt_double)
					    GenerateDiadic(op_fdmov,0,result,makefpreg(1));
                    else
					    GenerateDiadic(op_mov,0,result,makereg(1));
                }
			}
			else {
                if (node->etype==bt_double)
      				GenerateDiadic(op_fdmov,0,result,makereg(1));
                else
		     		GenerateDiadic(op_mov,0,result,makereg(1));
            }
		}
	}
    return result;
	*/
}

void GenLdi(AMODE *ap1, AMODE *ap2)
{
	GenerateDiadic(op_ldi,0,ap1,ap2);
  return;
}

