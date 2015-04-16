
typedef unsigned int uint;

typedef struct tagMSG align(32) {
	struct tagMSG *link;
	uint d1;
	uint d2;
	uint type;
} MSG;

typedef struct _tagJCB align(2048)
{
    struct _tagJCB *iof_next;
    struct _tagJCB *iof_prev;
    char UserName[32];
    char path[256];
    char exitRunFile[256];
    char commandLine[256];
    unsigned __int32 *pVidMem;
    unsigned __int32 *pVirtVidMem;
    unsigned __int16 VideoRows;
    unsigned __int16 VideoCols;
    unsigned __int16 CursorRow;
    unsigned __int16 CursorCol;
    unsigned __int32 NormAttr;
    __int8 KeybdHead;
    __int8 KeybdTail;
    unsigned __int16 KeybdBuffer[16];
    __int16 number;
} JCB;

struct tagMBX;

typedef struct _tagTCB align(512) {
	int regs[32];
	int isp;
	int dsp;
	int esp;
	int ipc;
	int dpc;
	int epc;
	int cr0;
	struct _tagTCB *next;
	struct _tagTCB *prev;
	struct _tagTCB *mbq_next;
	struct _tagTCB *mbq_prev;
	int *sys_stack;
	int *bios_stack;
	int *stack;
	__int64 timeout;
	JCB *hJob;
	int msgD1;
	int msgD2;
	MSG *MsgPtr;
	uint hWaitMbx;
	struct tagMBX *mailboxes;
	__int8 priority;
	__int8 status;
	__int8 affinity;
	__int16 number;
} TCB;

typedef struct tagMBX align(128) {
    struct tagMBX *link;
	TCB *tq_head;
	TCB *tq_tail;
	MSG *mq_head;
	MSG *mq_tail;
	uint tq_count;
	uint mq_size;
	uint mq_count;
	uint mq_missed;
	uint owner;		// hJcb of owner
	char mq_strategy;
	byte resv[7];
} MBX;

typedef struct tagALARM {
	struct tagALARM *next;
	struct tagALARM *prev;
	MBX *mbx;
	MSG *msg;
	uint BaseTimeout;
	uint timeout;
	uint repeat;
	byte resv[8];		// padding to 64 bytes
} ALARM;


TCB *GetRunningTCB();
JCB *GetJCBPtr();                   // get the JCB pointer of the running task
void set_vector(unsigned int, unsigned int);
int getCPU();
void outb(unsigned int, int);
void outc(unsigned int, int);
void outh(unsigned int, int);
void outw(unsigned int, int);


// The text screen memory can only handle half-word transfers, hence the use
// of memsetH, memcpyH.

extern int IOFocusNdx;

short int *GetScreenLocation()
{
      return GetJCBPtr()->pVidMem;
}

short int GetCurrAttr()
{
      return GetJCBPtr()->NormAttr;
}

void SetCurrAttr(short int attr)
{
     GetJCBPtr()->NormAttr = attr & 0xFFFFFC00;
}

void SetVideoReg(int regno, int val)
{
     asm {
         lw   r1,24[bp]
         lw   r2,32[bp]
         asl  r1,r1,#2
         sh   r2,$FFDA0000[r1]
     }
}

void SetCursorPos(int row, int col)
{
    JCB *j;

    j = GetJCBPtr();
    j->CursorCol = col;
    j->CursorRow = row;
    UpdateCursorPos();
}

void SetCursorCol(int col)
{
    JCB *j;

    j = GetJCBPtr();
    j->CursorCol = col;
    UpdateCursorPos();
}

int GetCursorPos()
{
    JCB *j;

    j = GetJCBPtr();
    return j->CursorCol | (j->CursorRow << 8);
}

int GetTextCols()
{
    return GetJCBPtr()->VideoCols;
}

int GetTextRows()
{
    return GetJCBPtr()->VideoRows;
}

char AsciiToScreen(char ch)
{
     if (ch==0x5B)
         return 0x1B;
     if (ch==0x5D)
         return 0x1D;
     ch &= 0xFF;
     ch |= 0x100;
     if (!(ch & 0x20))
         return ch;
     if (!(ch & 0x40))
         return ch;
     ch = ch & 0x19F;
     return ch;
}

char ScreenToAscii(char ch)
{
     ch &= 0xFF;
     if (ch==0x1B)
        return 0x5B;
     if (ch==0x1D)
        return 0x5D;
     if (ch < 27)
        ch += 0x60;
     return ch;
}
    

void UpdateCursorPos()
{
    JCB *j;
    int pos;

    j = GetJCBPtr();
//    if (j == IOFocusNdx) {
       pos = j->CursorRow * j->VideoCols + j->CursorCol;
       SetVideoReg(11,pos);
//    }
}

void HomeCursor()
{
    JCB *j;

    j = GetJCBPtr();
    j->CursorCol = 0;
    j->CursorRow = 0;
    UpdateCursorPos();
}

short int *CalcScreenLocation()
{
    JCB *j;
    int pos;

    j = GetJCBPtr();
    pos = j->CursorRow * j->VideoCols + j->CursorCol;
//    if (j == IOFocusNdx) {
       SetVideoReg(11,pos);
//    }
    return GetScreenLocation()+pos;
}

void ClearScreen()
{
     short int *p;
     int nn;
     int mx;
     JCB *j;
     short int vc;
     
     j = GetJCBPtr();
     p = GetScreenLocation();
     // Compiler did a byte multiply generating a single byte result first
     // before assigning it to mx. The (int) casts force the compiler to use
     // an int result.
     mx = (int)j->VideoRows * (int)j->VideoCols;
     vc = GetCurrAttr() | AsciiToScreen(' ');
     memsetH(p, vc, mx);
}

void ClearBmpScreen()
{
     memsetW(0x400000, 0, 0x80000);
}

void BlankLine(int row)
{
     short int *p;
     int nn;
     int mx;
     JCB *j;
     short int vc;
     
     j = GetJCBPtr();
     p = GetScreenLocation();
     p = p + (int)j->VideoCols * row;
     vc = GetCurrAttr() | AsciiToScreen(' ');
     memsetH(p, vc, j->VideoCols);
}

// ScrollUp will call BlankLine. Scrollup is written in assembler for
// performance reasons and is included as part of the video BIOS. Note the
// BIOS cannot be called with SYS #10 because the bios isn't re-entrant and
// the bios is already active from putch().
naked ScrollUp()
{
     asm {
         push  lr
         bsr   VBScrollUp
         rts
     }
}

void IncrementCursorRow()
{
     JCB *j;
     
     j = GetJCBPtr();
     j->CursorRow++;
     if (j->CursorRow < j->VideoRows) {
         UpdateCursorPos();
         return;
     }
     j->CursorRow--;
     UpdateCursorPos();
     ScrollUp();
}

void IncrementCursorPos()
{
     JCB *j;
     
     j = GetJCBPtr();
     j->CursorCol++;
     if (j->CursorCol < j->VideoCols) {
         UpdateCursorPos();
         return;
     }
     j->CursorCol = 0;
     IncrementCursorRow();
}

void DisplayChar(char ch)
{
     short int *p;
     int nn;
     JCB *j;

     j = GetJCBPtr();
     switch(ch) {
     case '\r':  j->CursorCol = 0; UpdateCursorPos(); break;
     case '\n':  IncrementCursorRow(); break;
     case 0x91:
          if (j->CursorCol < j->VideoCols-1) {
             j->CursorCol++;
             UpdateCursorPos();
          }
          break;
     case 0x90:
          if (j->CursorRow > 0) {
               j->CursorRow--;
               UpdateCursorPos();
          }
          break;
     case 0x93:
          if (j->CursorCol > 0) {
               j->CursorCol--;
               UpdateCursorPos();
          }
          break;
     case 0x92:
          if (j->CursorRow < j->VideoRows-1) {
             j->CursorRow++;
             UpdateCursorPos();
          }
          break;
     case 0x94:
          if (j->CursorCol==0)
             j->CursorRow = 0;
          j->CursorCol = 0;
          UpdateCursorPos();
          break;
     case 0x99:  // delete
          p = CalcScreenLocation();
          for (nn = j->CursorCol; nn < j->VideoCols-1; nn++) {
              p[nn-j->CursorCol] = p[nn+1-j->CursorCol];
          }
          p[nn-j->CursorCol] = GetCurrAttr() | AsciiToScreen(' ');
          break;
     case 0x08: // backspace
          if (j->CursorCol > 0) {
              j->CursorCol--;
              p = CalcScreenLocation();
              for (nn = j->CursorCol; nn < j->VideoCols-1; nn++) {
                  p[nn-j->CursorCol] = p[nn+1-j->CursorCol];
              }
              p[nn-j->CursorCol] = GetCurrAttr() | AsciiToScreen(' ');
          }
          break;
     case 0x0C:   // CTRL-L
          ClearScreen();
          HomeCursor();
          break;
     case '\t':
          DisplayChar(' ');
          DisplayChar(' ');
          DisplayChar(' ');
          DisplayChar(' ');
          break;
     default:
          p = CalcScreenLocation();
          *p = GetCurrAttr() | AsciiToScreen(ch);
          IncrementCursorPos();
          break;
     }
}

void CRLF()
{
     DisplayChar('\r');
     DisplayChar('\n');
}

void DisplayString(char *s)
{
     while (*s) { DisplayChar(*s); s++; }
}

void DisplayStringCRLF(char *s)
{
     DisplayString(s);
     CRLF();
}
