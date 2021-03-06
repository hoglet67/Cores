#ifndef CGBLDEC_H
#define CGBLDEC_H

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
/*
 *	68000 C compiler
 *
 *	Copyright 1984, 1985, 1986 Matthew Brandt.
 *  all commercial rights reserved.
 *
 *	This compiler is intended as an instructive tool for personal use. Any
 *	use for profit without the written consent of the author is prohibited.
 *
 *	This compiler may be distributed freely for non-commercial use as long
 *	as this notice stays intact. Please forward any enhancements or questions
 *	to:
 *
 *		Matthew Brandt
 *		Box 920337
 *		Norcross, Ga 30092
 */

/*      global ParseSpecifierarations     */
//#define DOTRACE	1
#ifdef DOTRACE
#define TRACE(x)	x
#else
#define TRACE(x)
#endif

extern int maxPn;
extern int hook_predreg;
extern int gCpu;
extern int regGP;
extern int regSP;
extern int regBP;
extern int regLR;
extern int regXLR;
extern int regPC;
extern int regCLP;
extern int regZero;
extern int regFirstParm;
extern int regLastParm;
extern int farcode;
extern int wcharSupport;
extern int verbose;
extern int use_gp;
extern int address_bits;
extern std::ifstream *ifs;
extern txtoStream ofs;
extern txtoStream lfs;
extern txtoStream dfs;
extern int mangledNames;
extern int sizeOfWord;
extern int sizeOfFP;
extern int sizeOfFPT;
extern int sizeOfFPD;
extern int sizeOfFPQ;

extern GlobalDeclaration *gd;
extern bool firstLineOfFunc;
extern char last_rem[132];

/*
extern FILE             *input,
                        *list,
                        *output;
*/
extern FILE *outputG;
extern int incldepth;
extern int              lineno;
extern int              nextlabel;
extern int              lastch;
extern int              lastst;
extern char             lastid[128];
extern char             lastkw[128];
extern char             laststr[MAX_STLP1];
extern int64_t	ival;
extern double           rval;
extern char float_precision;
extern int parseEsc;
//extern FloatTriple      FAC1,FAC2;

extern TABLE            gsyms[257],
                        lsyms;
extern TABLE            tagtable;
extern SYM              *lasthead;
extern struct slit      *strtab;
extern struct clit		*casetab;
extern int              lc_static;
extern int              lc_auto;
extern int				lc_thread;
extern Statement     *bodyptr;       /* parse tree for function */
extern int              global_flag;
extern TABLE            defsyms;
extern int64_t          save_mask;      /* register save mask */
extern int64_t          fpsave_mask;
extern int				bsave_mask;
extern int uctran_off;
extern int isKernel;
extern int isPascal;
extern int isOscall;
extern int isInterrupt;
extern int isTask;
extern int isNocall;
extern bool isRegister;
extern int asmblock;
extern int optimize;
extern int opt_noregs;
extern int opt_nopeep;
extern int opt_noexpr;
extern int opt_nocgo;
extern int exceptions;
extern int mixedSource;
extern SYM *currentFn;
extern int iflevel;
extern int foreverlevel;
extern int looplevel;
extern int loopexit;
extern int regmask;
extern int bregmask;
extern Statement *currentStmt;
extern bool dogen;

extern TYP *stdint;
extern TYP *stduint;
extern TYP *stdlong;
extern TYP *stdulong;
extern TYP *stdshort;
extern TYP *stdushort;
extern TYP *stdchar;
extern TYP *stduchar;
extern TYP *stdbyte;
extern TYP *stdubyte;
extern TYP *stdstring;
extern TYP *stddbl;
extern TYP *stdtriple;
extern TYP *stdflt;
extern TYP *stddouble;
extern TYP *stdfunc;
extern TYP *stdexception;
extern TYP *stdconst;
extern TYP *stdquad;

extern std::string *declid;
extern Compiler compiler;

extern struct ocode *peep_head;
extern struct ocode *peep_tail;
extern int PeepCount(struct ocode *ip);

// Analyze.c
extern short int csendx;
extern CSEList CSETable;

extern int opt1(Statement *stmt);
// CMain.c
extern void closefiles();

extern void error(int n);
extern void needpunc(enum e_sym p,int);
// Memmgt.c
extern void *allocx(int);
extern char *xalloc(int);
extern SYM *allocSYM();
extern TYP *allocTYP();
extern AMODE *allocAmode();
extern ENODE *allocEnode();
extern CSE *allocCSE();
extern void ReleaseGlobalMemory();
extern void ReleaseLocalMemory();

// NextToken.c
extern void initsym();
extern void NextToken();
extern int getch();
extern int my_isspace(char c);
extern void getbase(int64_t);
extern void SkipSpaces();

// Stmt.c
extern Statement *ParseCompoundStatement();

extern void GenerateDiadic(int op, int len, struct amode *ap1,struct amode *ap2);
// Symbol.c
extern SYM *gsearch(std::string na);
extern SYM *search(std::string na,TABLE *thead);
extern void insert(SYM* sp, TABLE *table);

// ParseFunction.c
extern SYM *BuildParameterList(SYM *sp, int *);

extern char *my_strdup(char *);
// Decl.c
extern int imax(int i, int j);
extern TYP *maketype(int bt, int siz);
extern void dodecl(int defclass);
extern int ParseParameterDeclarations(int);
extern void ParseAutoDeclarations(SYM *sym, TABLE *table);
extern int ParseSpecifier(TABLE *table);
extern SYM* ParseDeclarationPrefix(char isUnion);
extern int ParseStructDeclaration(int);
extern void ParseEnumerationList(TABLE *table);
extern int ParseFunction(SYM *sp);
extern int declare(SYM *sym,TABLE *table,int al,int ilc,int ztype);
extern void initstack();
extern int getline(int listflag);
extern void compile();

// Init.c
extern void doinit(SYM *sp);
// Func.c
extern SYM *makeint(char *);
extern void funcbody(SYM *sp);
// Intexpr.c
extern int GetIntegerExpression(ENODE **p);
// Expr.c
extern SYM *makeStructPtr(std::string name);
extern ENODE *makenode(int nt, ENODE *v1, ENODE *v2);
extern ENODE *makeinode(int nt, int v1);
extern ENODE *makesnode(int nt, std::string *v1, std::string *v2, int i);
extern TYP *nameref(ENODE **node,int);
extern TYP *forcefit(ENODE **node1,TYP *tp1,ENODE **node2,TYP *tp2,bool);
extern TYP *expression(ENODE **node);
extern AMODE *GenerateExpression(ENODE *node, int flags, int size);
extern int GetNaturalSize(ENODE *node);
extern TYP *asnop(ENODE **node);
extern TYP *NonCommaExpression(ENODE **);
// Optimize.c
extern void opt_const(ENODE **node);
// GenerateStatement.c
//extern void GenerateFunction(Statement *stmt);
extern void GenerateIntoff(Statement *stmt);
extern void GenerateInton(Statement *stmt);
extern void GenerateStop(Statement *stmt);
extern void gen_regrestore();
extern AMODE *make_direct(int i);
extern AMODE *makereg(int r);
extern AMODE *makefpreg(int t);
extern AMODE *makebreg(int r);
extern AMODE *makepred(int r);
extern int bitsset(int64_t mask);
extern int popcnt(int64_t m);
// Outcode.c
extern void GenerateByte(int val);
extern void GenerateChar(int val);
extern void genhalf(int val);
extern void GenerateWord(int val);
extern void GenerateLong(int val);
extern void genstorage(int nbytes);
extern void GenerateReference(SYM *sp,int offset);
extern void GenerateLabelReference(int n);
extern void gen_strlab(char *s);
extern void dumplits();
extern int  stringlit(char *s);
extern void nl();
extern void seg(int sg, int algn);
extern void cseg();
extern void dseg();
extern void tseg();
//extern void put_code(int op, int len,AMODE *aps, AMODE *apd, AMODE *);
extern void put_code(struct ocode *);
extern char *put_label(int lab, char*, char*, char);
extern char *opstr(int op);
// Peepgen.c
extern AMODE *copy_addr(AMODE *ap);
extern void MarkRemoveRange(struct ocode *bp, struct ocode *ep);
extern void PeepRemove();
extern void PeepNop(struct ocode *ip1, struct ocode *ip2);
extern void flush_peep();
extern int equal_address(AMODE *ap1, AMODE *ap2);
extern int GeneratePreload();
extern void OverwritePreload(int handle, int op, int len, AMODE *ap1, AMODE *ap2, AMODE *ap3, AMODE *ap4);
extern void GenerateLabel(int labno);
extern void GenerateZeradic(int op);
extern void GenerateMonadic(int op, int len, AMODE *ap1);
extern void GenerateDiadic(int op, int len, AMODE *ap1, AMODE *ap2);
extern void GenerateTriadic(int op, int len, AMODE *ap1, AMODE *ap2, AMODE *ap3);
extern void Generate4adic(int op, int len, AMODE *ap1, AMODE *ap2, AMODE *ap3, AMODE *ap4);
extern void GeneratePredicatedMonadic(int pr, int pop, int op, int len, AMODE *ap1);
extern void GeneratePredicatedDiadic(int pop, int pr, int op, int len, AMODE *ap1, AMODE *ap2);
extern void GeneratePredicatedTriadic(int pop, int op, int len, AMODE *ap1, AMODE *ap2, AMODE *ap3);
// Gencode.c
extern void GenerateHint(int n);
extern void GenLdi(AMODE *, AMODE *);
extern AMODE *make_label(int lab);
extern AMODE *make_clabel(int lab);
extern AMODE *make_clabel2(int lab,char*);
extern AMODE *make_immed(int i);
extern AMODE *make_indirect(int i);
extern AMODE *make_offset(ENODE *node);
extern void swap_nodes(ENODE *node);
extern int isshort(ENODE *node);
// IdentifyKeyword.c
extern int IdentifyKeyword();
// Preproc.c
extern int preprocess();
// OPC6.cpp
extern void GenerateCmp(ENODE *node, int label, int predreg, unsigned int prediction, int type);
// CodeGenerator.c
extern AMODE *make_indirect(int i);
extern AMODE *make_indexed(int o, int i);
extern AMODE *make_indx(ENODE *node, int reg);
extern AMODE *make_string(char *s);
extern void GenerateFalseJump(ENODE *node,int label, unsigned int);
extern void GenerateTrueJump(ENODE *node,int label, unsigned int);
extern char *GetNamespace();
extern char nmspace[20][100];
extern AMODE *GenerateDereference(ENODE *, int, int, int);
extern void MakeLegalAmode(AMODE *ap,int flags, int size);
extern void GenLoad(AMODE *, AMODE *, int size, int);
extern void GenStore(AMODE *, AMODE *, int size);
// List.c
extern void ListTable(TABLE *t, int i);
// Register.c
extern AMODE *GetTempRegister();
extern AMODE *GetTempRegister2(int *pushed);
extern AMODE *GetTempRegisterPair();
extern AMODE *GetTempBrRegister();
extern AMODE *GetTempFPRegister();
extern void ReleaseTempRegister(AMODE *ap);
extern void ReleaseTempReg(AMODE *ap);
extern int TempInvalidate();
extern void TempRevalidate(int sp);
extern AMODE *GenerateFunctionCall(ENODE *node, int flags);

extern void GenerateFunction(SYM *sym);
extern void GenerateReturn(Statement *stmt);

extern AMODE *GenerateShift(ENODE *node,int flags, int size);
extern AMODE *GenerateAssignShift(ENODE *node,int flags,int size);
extern AMODE *GenerateBitfieldDereference(ENODE *node, int flags, int size);
extern AMODE *GenerateBitfieldAssign(ENODE *node, int flags, int size);
// err.c
extern void fatal(char *str);

extern int GetReturnBlockSize();

enum e_sg { noseg, codeseg, dataseg, stackseg, bssseg, idataseg, tlsseg, rodataseg };

#endif
