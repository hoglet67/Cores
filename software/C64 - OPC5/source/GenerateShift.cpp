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


AMODE *GenerateShift(ENODE *node,int flags, int size)
{
	AMODE *ap1, *ap2;
	int nn;
	int lab1;

    ap1 = GenerateExpression(node->p[0],F_REG,size);
    ap2 = GenerateExpression(node->p[1],F_REG | F_IMMED,sizeOfWord);
	switch (node->nodetype) {
	case en_shl:
	case en_shlu:
		switch(ap2->mode) {
		case am_immed:
			for (nn = 0; nn < ap2->offset->i && nn < 33; nn++) {
				GenerateDiadic(op_add,0,ap1,makereg(regZero));
				if (size==2)
					GenerateDiadic(op_adc,0,ap1->amode2,makereg(regZero));
			}
			ReleaseTempRegister(ap2);
			MakeLegalAmode(ap1,flags,size);
			return ap1;
		case am_reg:
			lab1 = nextlabel++;
			GenerateLabel(lab1);
			GenerateDiadic(op_add,0,ap1,makereg(regZero));
			if (size==2)
				GenerateDiadic(op_adc,0,ap1->amode2,makereg(regZero));
			GenerateTriadic(op_sub,0,ap2,makereg(regZero),make_immed(1));
			GeneratePredicatedTriadic(pop_nz,op_mov,0,makereg(regPC),makereg(regZero),make_clabel(lab1));
			ReleaseTempRegister(ap2);
			MakeLegalAmode(ap1,flags,size);
			return ap1;
		}
		break;
	case en_shru:
	case en_shr:
		switch(ap2->mode) {
		case am_immed:
			for (nn = 0; nn < ap2->offset->i && nn < 33; nn++) {
				GenerateDiadic(op_add,0,makereg(0),makereg(0));	// clear carry
				if (size==2)
					GenerateDiadic(op_ror,0,ap1->amode2,makereg(regZero));
				GenerateDiadic(op_ror,0,ap1,makereg(regZero));
			}
			ReleaseTempRegister(ap2);
			MakeLegalAmode(ap1,flags,size);
			return ap1;
		case am_reg:
			lab1 = nextlabel++;
			GenerateLabel(lab1);
			GenerateDiadic(op_add,0,makereg(0),makereg(0));	// clear carry
			if (size==2)
				GenerateDiadic(op_ror,0,ap1->amode2,makereg(regZero));
			GenerateDiadic(op_ror,0,ap1,makereg(regZero));
			GenerateTriadic(op_sub,0,ap2,makereg(regZero),make_immed(1));
			GeneratePredicatedTriadic(pop_nz,op_mov,0,makereg(regPC),makereg(regZero),make_clabel(lab1));
			ReleaseTempRegister(ap2);
			MakeLegalAmode(ap1,flags,size);
			return ap1;
		}
		break;
	case en_asr:
		switch(ap2->mode) {
		case am_immed:
			for (nn = 0; nn < ap2->offset->i && nn < 33; nn++) {
				if (size==2) {
					GenerateDiadic(op_asr,0,ap1->amode2,makereg(regZero));
					GenerateDiadic(op_ror,0,ap1,makereg(regZero));
				}
				else
					GenerateDiadic(op_asr,0,ap1,makereg(regZero));
			}
			ReleaseTempRegister(ap2);
			MakeLegalAmode(ap1,flags,size);
			return ap1;
		case am_reg:
			lab1 = nextlabel++;
			GenerateLabel(lab1);
			if (size==2) {
				GenerateDiadic(op_asr,0,ap1->amode2,makereg(regZero));
				GenerateDiadic(op_ror,0,ap1,makereg(regZero));
			}
			else
				GenerateDiadic(op_asr,0,ap1,makereg(regZero));
			GenerateTriadic(op_sub,0,ap2,makereg(regZero),make_immed(1));
			GeneratePredicatedTriadic(pop_nz,op_mov,0,makereg(regPC),makereg(regZero),make_clabel(lab1));
			ReleaseTempRegister(ap2);
			MakeLegalAmode(ap1,flags,size);
			return ap1;
		}
		break;
	}
/*
	GenerateTriadic(op,size,ap3,ap1,ap2);
	// Shifts automatically sign extend
	// Don't actually need to zero extend on a shift right, but the other shifts
	// need it.
	if (ap2->isUnsigned)
		switch(size) {
		case 1:	GenerateTriadic(op_and,0,ap3,ap3,make_immed(0xFF)); break;	// shorter
		case 2:	Generate4adic(op_bfextu,0,ap3,ap3,make_immed(0),make_immed(15)); break;
		case 4:	Generate4adic(op_bfextu,0,ap3,ap3,make_immed(0),make_immed(31)); break;
		default:	;
		}
	ReleaseTempRegister(ap2);
	ReleaseTempRegister(ap1);
    MakeLegalAmode(ap3,flags,size);
    return ap3;
*/
	ReleaseTempRegister(ap2);
	ReleaseTempRegister(ap1);
	ap1 = makereg(1);
	if (size==2)
		ap1->amode2 = makereg(2);
	return (ap1);
}


//
// Generate shift equals ( <<=, >>=) operators.
//
AMODE *GenerateAssignShift(ENODE *node,int flags,int size)
{
	AMODE *ap1, *ap2, *ap3;
	int lab1, nn;

	ap1 = GetTempRegister();
	//size = GetNaturalSize(node->p[0]);
    ap3 = GenerateExpression(node->p[0],F_ALL,size);
    ap2 = GenerateExpression(node->p[1],F_REG | F_IMMED,size);
	if (ap3->mode==am_reg)
		GenerateTriadic(op_mov,0,ap1,ap3,make_immed(0));
	else if (ap3->mode == am_immed) {
		error(ERR_LVALUE);
	    GenLdi(ap1,ap3);
	}
	else
        GenLoad(ap1,ap3,size,size);
	//MaskShift(op, ap1, size);
	switch (node->nodetype) {
	case en_shl:
		switch(ap2->mode) {
		case am_immed:
			for (nn = 0; nn < ap2->offset->i && nn < 33; nn++) {
				GenerateTriadic(op_add,0,ap1,makereg(regZero),make_immed(0));
				if (size==2)
					GenerateTriadic(op_adc,0,ap1->amode2,makereg(regZero),make_immed(0));
			}
			ReleaseTempRegister(ap2);
			MakeLegalAmode(ap1,flags,size);
			return ap1;
		case am_reg:
			lab1 = nextlabel++;
			GenerateLabel(lab1);
			GenerateTriadic(op_add,0,ap1,makereg(regZero),make_immed(0));
			if (size==2)
				GenerateTriadic(op_adc,0,ap1->amode2,makereg(regZero),make_immed(0));
			GenerateTriadic(op_sub,0,ap2,makereg(regZero),make_immed(1));
			GeneratePredicatedTriadic(pop_nz,op_mov,0,makereg(regPC),makereg(regZero),make_clabel(lab1));
			ReleaseTempRegister(ap2);
			MakeLegalAmode(ap1,flags,size);
			return ap1;
		}
		break;

	case en_shru:
	case en_shr:
		switch(ap2->mode) {
		case am_immed:
			for (nn = 0; nn < ap2->offset->i && nn < 33; nn++) {
				GenerateTriadic(op_add,0,makereg(0),makereg(0),make_immed(0));	// clear carry
				if (size==2)
					GenerateTriadic(op_ror,0,ap1->amode2,makereg(regZero),make_immed(0));
				GenerateTriadic(op_ror,0,ap1,makereg(regZero),make_immed(0));
			}
			ReleaseTempRegister(ap2);
			MakeLegalAmode(ap1,flags,size);
			return ap1;
		case am_reg:
			lab1 = nextlabel++;
			GenerateLabel(lab1);
			GenerateTriadic(op_add,0,makereg(0),makereg(0),make_immed(0));	// clear carry
			if (size==2)
				GenerateTriadic(op_ror,0,ap1->amode2,makereg(regZero),make_immed(0));
			GenerateTriadic(op_ror,0,ap1,makereg(regZero),make_immed(0));
			GenerateTriadic(op_sub,0,ap2,makereg(regZero),make_immed(1));
			GeneratePredicatedTriadic(pop_nz,op_mov,0,makereg(regPC),makereg(regZero),make_clabel(lab1));
			ReleaseTempRegister(ap2);
			MakeLegalAmode(ap1,flags,size);
			return ap1;
		}
		break;

	case en_asr:
		switch(ap2->mode) {
		case am_immed:
			for (nn = 0; nn < ap2->offset->i && nn < 33; nn++) {
				if (size==2) {
					GenerateDiadic(op_asr,0,ap1->amode2,makereg(regZero));
					GenerateDiadic(op_ror,0,ap1,makereg(regZero));
				}
				else
					GenerateDiadic(op_asr,0,ap1,makereg(regZero));
			}
			ReleaseTempRegister(ap2);
			MakeLegalAmode(ap1,flags,size);
			return ap1;
		case am_reg:
			lab1 = nextlabel++;
			GenerateLabel(lab1);
			if (size==2) {
				GenerateDiadic(op_asr,0,ap1->amode2,makereg(regZero));
				GenerateDiadic(op_ror,0,ap1,makereg(regZero));
			}
			else
				GenerateDiadic(op_asr,0,ap1,makereg(regZero));
			GenerateTriadic(op_sub,0,ap2,makereg(regZero),make_immed(1));
			GeneratePredicatedTriadic(pop_nz,op_mov,0,makereg(regPC),makereg(regZero),make_clabel(lab1));
			ReleaseTempRegister(ap2);
			MakeLegalAmode(ap1,flags,size);
			return ap1;
		}
		break;
	}
	if (ap3->mode != am_reg)
        GenStore(ap1,ap3,size);
    ReleaseTempRegister(ap2);
    ReleaseTempRegister(ap3);
    MakeLegalAmode(ap1,flags,size);
    return ap1;
}

