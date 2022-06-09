/*
CSc 21000 - Assembly Programming
Project #1 - x86 Emulator
Fall 2021
Professor Michael Vulis
Name: Gazi Shahi
*/

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <bitset>
#include <stdio.h>
#include <vector>

using namespace std;

#define byte unsigned char
#define word unsigned short
byte mem[0x100000];

byte AL, AH, BL, BH, CL, CH, DL, DH;
word AX, BX, CX, DX, SP, BP, SI, DI;
word IP, FLAGS;
word ES, SS, CS, DS;

byte op = 0x00;
byte BG = 0x0;
byte FG = 0x0;
word imm16 = 0x0000;
vector<vector<char > > output;
vector<char> data;
int count = 0;

//Prints out flags
void printFlags(size_t const size, void const* const ptr)
{
	unsigned char* b = (unsigned char*)ptr;
	int i, j;

	for (i = size - 1; i >= 0; i--) {
		for (j = 7; j >= 0; j--) {
			byte test = (b[i] >> j) & 1;
			printf("%u", test);
		}
	}
	puts("");
}

//Prints the registers
void printReg() 
{
	printf("\n\nIP: %d %10s OPCODE: %02X\n", IP, " ", op);
	printf("AL=%02X \t AH=%02X \t AX=%04X \n", AL, AH, AX);
	printf("BL=%02X \t BH=%02X \t BX=%04X \n", BL, BH, BX);
	printf("CL=%02X \t CH=%02X \t CX=%04X \n", CL, CH, CX);
    printf("DL=%02X \t DH=%02X \t DX=%04X \n", DL, DH, DX);
	printf("SP=%04X \t BP=%04X \t SI=%04X \t DI=%04X \n", SP, BP, SI, DI);
	printf("FLAGS: "); 
    printFlags(sizeof(FLAGS), &FLAGS);
}

//Updates values of 8-bit registers
void UPDATE8(unsigned short AX, unsigned short CX, unsigned short DX, unsigned short BX, unsigned char& AL, unsigned char& AH, 
unsigned char& CL, unsigned char& CH, unsigned char& DL, unsigned char& DH, unsigned char& BL, unsigned char& BH)
{
    AL = AX & 0xff;
    AH = (AX & 0xff00) >> 8;
    CL = CX & 0xff;
    CH = (CX & 0xff00) >> 8;
    DL = DX & 0xff;
    DH = (DX & 0xff00) >> 8;
    BL = BX & 0xff;
    BH = (BX & 0xff00) >> 8;
}

//Add 8-bit to 8-bit
void ADD8(byte& a, byte& b) 
{
	if (a > 0xFF - b) FLAGS = FLAGS | (1 << 6);
	a += b;
	if (!a) FLAGS = FLAGS | (1 << 5);
}

//Add 16-bit to 16-bit
void ADD16(word& a, word& b) 
{
	if (a > 0xFFFF - b) FLAGS = FLAGS | (1 << 6);
	a += b;
	if (!a) FLAGS = FLAGS | (1 << 5);
}

//Add 8-bit to 16-bit
void ADD16and8(word& a, byte& b)
{
    if (a > 0xFFFF - b) FLAGS = FLAGS | (1 << 6);
	a += b;
	if (!a) FLAGS = FLAGS | (1 << 5);
}

//Subtract 8-bit and 8-bit
void SUB8(byte& a, byte& b)
{
    if (a > 0xFF - b) FLAGS = FLAGS | (1 << 6);
	a  -= b;
	if (!a) FLAGS = FLAGS | (1 << 5);
}

//Subtract 16-bit and 16-bit
void SUB16(word& a, word& b) {
	if (a > 0xFFFF - b) FLAGS = FLAGS | (1 << 6);
	a -= b;
	if (!a) FLAGS = FLAGS | (1 << 5);
}

//Subtract 8-bit from 16-bit
void SUB16and8(word& a, byte& b)
{
    if (a > 0xFFFF - b) FLAGS = FLAGS | (1 << 6);
	a -= b;
	if (!a) FLAGS = FLAGS | (1 << 5);
}

//Adds low and high 8-bit registers, essentially updates 16-bit registers
word join(byte L, byte H)
{
    word result = (H << 8) | L;
    return result;
}

//Function for instructions
void instructions(byte op)
{
    switch (op)
    {
		case 0xFE:
            op = mem[IP++];
            switch(op)
            {
                //Increment 8-bit Register
                case 0xC0:
                    AL++;
                    AX = join(AL, AH);
                    break;
                case 0xC1:
                    CL++;
                    CX = join(CL, CH);
                    break;
                case 0xC2:
                    DL++;
                    DX = join(DL, DH);
                    break;
                case 0xC3:
                    BL++;
                    BX = join(BL, BH);
                    break;
                case 0xC4:
                    AH++;
                    AX = join(AL, AH);
                    break;
                case 0xC5:
                    CH++;
                    CX = join(CL, CH);
                    break;
                case 0xC6:
                    DH++;
                    DX = join(DL, DH);
                    break;
                case 0xC7:
                    BH++;
                    BX = join(BL, BX);
                    break;

                //Decrement 8-bit Register
                case 0xC8:
                    AL--;
                    AX = join(AL, AH);
                    break;
                case 0xC9:
                    CL--;
                    CX = join(CL, CH);
                    break;
                case 0xCA:
                    DL--;
                    DX = join(DL, DH);
                    break;
                case 0xCB:
                    BL--;
                    BX = join(BL, BX);
                    break;
                case 0xCC:
                    AH--;
                    AX = join(AL, AH);
                    break;
                case 0xCD:
                    CH--;
                    CX = join(CL, CH);
                    break;
                case 0xCE:
                    DH--;
                    DX = join(DL, DH);
                    break;
                case 0xCF:
                    BH--;
                    BX = join(BL, BX);
                    break;
            }
            UPDATE8(AX,CX,DX,BX,AL,AH,CL,CH,DL,DH,BL,BH);
            break;

            //Increment 16-Bit Register
            case 0x40:
                AX++;
                UPDATE8(AX,CX,DX,BX,AL,AH,CL,CH,DL,DH,BL,BH);
                break;
            case 0x41:
                CX++;
                UPDATE8(AX,CX,DX,BX,AL,AH,CL,CH,DL,DH,BL,BH);
                break;
            case 0x42:
                DX++;
                UPDATE8(AX,CX,DX,BX,AL,AH,CL,CH,DL,DH,BL,BH);
                break;
            case 0x43:
                BX++;
                UPDATE8(AX,CX,DX,BX,AL,AH,CL,CH,DL,DH,BL,BH);
                break;
            case 0x44:
                SP++;
                UPDATE8(AX,CX,DX,BX,AL,AH,CL,CH,DL,DH,BL,BH);
                break;
            case 0x45:
                BP++;
                UPDATE8(AX,CX,DX,BX,AL,AH,CL,CH,DL,DH,BL,BH);
                break;
            case 0x46:
                SI++;
                UPDATE8(AX,CX,DX,BX,AL,AH,CL,CH,DL,DH,BL,BH);
                break;
            case 0x47:
                DI++;
                UPDATE8(AX,CX,DX,BX,AL,AH,CL,CH,DL,DH,BL,BH);
                break;

            //Decrement 16-Bit Register
            case 0x48:
                AX--;
                UPDATE8(AX,CX,DX,BX,AL,AH,CL,CH,DL,DH,BL,BH);
                break;
            case 0x49:
                CX--;
                UPDATE8(AX,CX,DX,BX,AL,AH,CL,CH,DL,DH,BL,BH);
                break;
            case 0x4A:
                DX--;
                UPDATE8(AX,CX,DX,BX,AL,AH,CL,CH,DL,DH,BL,BH);
                break;
            case 0x4B:
                BX--;
                UPDATE8(AX,CX,DX,BX,AL,AH,CL,CH,DL,DH,BL,BH);
                break;
            case 0x4C:
                SP--;
                UPDATE8(AX,CX,DX,BX,AL,AH,CL,CH,DL,DH,BL,BH);
                break;
            case 0x4D:
                BP--;
                UPDATE8(AX,CX,DX,BX,AL,AH,CL,CH,DL,DH,BL,BH);
                break;
            case 0x4E:
                SI--;
                UPDATE8(AX,CX,DX,BX,AL,AH,CL,CH,DL,DH,BL,BH);
                break;
            case 0x4F:
                DI--;
                UPDATE8(AX,CX,DX,BX,AL,AH,CL,CH,DL,DH,BL,BH);
                break;

        //ADD
        //add reg8, reg8
        case 0x02:
		    op = mem[IP++];
		    switch (op) {
            //add AL, reg8
		    case 0xC0: case 0xC1: case 0xC2: case 0xC3: case 0xC4: case 0xC5: case 0xC6: case 0xC7:
			    if(op == 0xC0)
				    ADD8(AL, AL);
			    if(op == 0xC1)
			    	ADD8(AL, CL);
			    if(op == 0xC2)
			    	ADD8(AL, DL);
			    if(op == 0xC3)
				    ADD8(AL, BL);
			    if(op == 0xC4)
				    ADD8(AL, AH);
			    if(op == 0xC5)
				    ADD8(AL, CH);
			    if(op == 0xC6)
				    ADD8(AL, DH);
			    if(op == 0xC7)
				    ADD8(AL, BH);
			    AX = join(AL, AH);
			    break;
            //add CL, reg8
            case 0xC8: case 0xC9: case 0xCA: case 0xCB: case 0xCC: case 0xCD: case 0xCE: case 0xCF:
                if(op == 0xC8)
                    ADD8(CL, AL);
                if(op == 0xC9)
                    ADD8(CL, CL);
                if(op == 0xCA)
                    ADD8(CL, DL);
                if(op == 0xCB)
                    ADD8(CL, BL);
                if(op == 0xCC)
                    ADD8(CL, AH);
                if(op == 0xCD)
                    ADD8(CL, CH);
                if(op == 0xCE)
                    ADD8(CL, DH);
                if(op == 0xCF)
                    ADD8(CL, BH);
                CX = join(CL, CH);
                break;
            //add DL, reg8
            case 0xD0: case 0xD1: case 0xD2: case 0xD3: case 0xD4: case 0xD5: case 0xD6: case 0xD7:
                if(op == 0xD0)
                    ADD8(DL, AL);
                if(op == 0xD1)
                    ADD8(DL, CL);
                if(op == 0xD2)
                    ADD8(DL, DL);
                if(op == 0xD3)
                    ADD8(DL, BL);
                if(op == 0xD4)
                    ADD8(DL, AH);
                if(op == 0xD5)
                    ADD8(DL, CH);
                if(op == 0xD6)
                    ADD8(DL, DH);
                if(op == 0xD7)
                    ADD8(DL, BH);
                DX = join(DL, DH);
                break;
            //add BL, reg8
            case 0xD8: case 0xD9: case 0xDA: case 0xDB: case 0xDC: case 0xDD: case 0xDE: case 0xDF:
                if(op == 0xD8)
                    ADD8(BL, AL);
                if(op == 0xD9)
                    ADD8(BL, CL);
                if(op == 0xDA)
                    ADD8(BL, DL);
                if(op == 0xDB)
                    ADD8(BL, BL);
                if(op == 0xDC)
                    ADD8(BL, AH);
                if(op == 0xDD)
                    ADD8(BL, CH);
                if(op == 0xDE)
                    ADD8(BL, DH);
                if(op == 0xDF)
                    ADD8(BL, BH);
                BX = join(BL, BH);
                break;
            //add AH, reg8
            case 0xE0: case 0xE1: case 0xE2: case 0xE3: case 0xE4: case 0xE5: case 0xE6: case 0xE7:
                if(op == 0xE0)
                    ADD8(AH, AL);
                if(op == 0xE1)
                    ADD8(AH, CL);
                if(op == 0xE2)
                    ADD8(AH, DL);
                if(op == 0xE3)
                    ADD8(AH, BL);
                if(op == 0xE4)
                    ADD8(AH, AH);
                if(op == 0xE5)
                    ADD8(AH, CH);
                if(op == 0xE6)
                    ADD8(AH, DH);
                if(op == 0xE7)
                    ADD8(AH, BH);
                AX = join(AL, AH);
                break;
            //add CH, reg8
            case 0xE8: case 0xE9: case 0xEA: case 0xEB: case 0xEC: case 0xED: case 0xEE: case 0xEF:
                if(op == 0xE8)
                    ADD8(CH, AL);
                if(op == 0xE9)
                    ADD8(CH, CL);
                if(op == 0xEA)
                    ADD8(CH, DL);
                if(op == 0xEB)
                    ADD8(CH, BL);
                if(op == 0xEC)
                    ADD8(CH, AH);
                if(op == 0xED)
                    ADD8(CH, CH);
                if(op == 0xEE)
                    ADD8(CH, DH);
                if(op == 0xEF)
                    ADD8(CH, BH);
                CX = join(CL, CH);
                break;
            //add DH, reg8
            case 0xF0: case 0xF1: case 0xF2: case 0xF3: case 0xF4: case 0xF5: case 0xF6: case 0xF7:
                if(op == 0xF0)
                    ADD8(DH, AL);
                if(op == 0xF1)
                    ADD8(DH, CL);
                if(op == 0xF2)
                    ADD8(DH, DL);
                if(op == 0xF3)
                    ADD8(DH, BL);
                if(op == 0xF4)
                    ADD8(DH, AH);
                if(op == 0xF5)
                    ADD8(DH, CH);
                if(op == 0xF6)
                    ADD8(DH, DH);
                if(op == 0xF7)
                    ADD8(DH, BH);
                DX = join(DL, DH);
                break;
            //add BH, reg8
            case 0xF8: case 0xF9: case 0xFA: case 0xFB: case 0xFC: case 0xFD: case 0xFE: case 0xFF:
                if(op == 0xF8)
                    ADD8(BH, AL);
                if(op == 0xF9)
                    ADD8(BH, CL);
                if(op == 0xFA)
                    ADD8(BH, DL);
                if(op == 0xFB)
                    ADD8(BH, BL);
                if(op == 0xFC)
                    ADD8(BH, AH);
                if(op == 0xFD)
                    ADD8(BH, CH);
                if(op == 0xFE)
                    ADD8(BH, DH);
                if(op == 0xFF)
                    ADD8(BH, BH);
                BX = join(BL, BH);
                break;
            break;
        }
        //add reg16, reg16
        case 0x03:
            op = mem[IP++];
            switch (op)
            {
                case 0xC0:
                    ADD16(AX, AX);
                    break;
                case 0xC1:
                    ADD16(AX, CX);
                    break;
                case 0xC2:
                    ADD16(AX, DX);
                    break;
                case 0xC3:
                    ADD16(AX, BX);
                    break;
                case 0xC4:
                    ADD16(AX, SP);
                    break;
                case 0xC5:
                    ADD16(AX, BP);
                    break;
                case 0xC6:
                    ADD16(AX, SI);
                    break;
                case 0xC7:
                    ADD16(AX, DI);
                    break;
                case 0xC8:
                    ADD16(CX, AX);
                    break;
                case 0xC9:
                    ADD16(CX, CX);
                    break;
                case 0xCA:
                    ADD16(CX, DX);
                    break;
                case 0xCB:
                    ADD16(CX, BX);
                    break;
                case 0xCC:
                    ADD16(CX, SP);
                    break;
                case 0xCD:
                    ADD16(CX, BP);
                    break;
                case 0xCE:
                    ADD16(CX, SI);
                    break;
                case 0xCF:
                    ADD16(CX, DI);
                    break;
                case 0xD0:
                    ADD16(DX, AX);
                    break;
                case 0xD1:
                    ADD16(DX, CX);
                    break;
                case 0xD2:
                    ADD16(DX, DX);
                    break;
                case 0xD3:
                    ADD16(DX, BX);
                    break;
                case 0xD4:
                    ADD16(DX, SP);
                    break;
                case 0xD5:
                    ADD16(DX, BP);
                    break;
                case 0xD6:
                    ADD16(DX, SI);
                    break;
                case 0xD7:
                    ADD16(DX, DI);
                    break;
                case 0xD8:
                    ADD16(BX, AX);
                    break;
                case 0xD9:
                    ADD16(BX, CX);
                    break;
                case 0xDA:
                    ADD16(BX, DX);
                    break;
                case 0xDB:
                    ADD16(BX, BX);
                    break;
                case 0xDC:
                    ADD16(BX, SP);
                    break;
                case 0xDD:
                    ADD16(BX, BP);
                    break;
                case 0xDE:
                    ADD16(BX, SI);
                    break;
                case 0xDF:
                    ADD16(BX, DI);
                    break;
                case 0xE0:
                    ADD16(SP, AX);
                    break;
                case 0xE1:
                    ADD16(SP, CX);
                    break;
                case 0xE2:
                    ADD16(SP, DX);
                    break;
                case 0xE3:
                    ADD16(SP, BX);
                    break;
                case 0xE4:
                    ADD16(SP, SP);
                    break;
                case 0xE5:
                    ADD16(SP, BP);
                    break;
                case 0xE6:
                    ADD16(SP, SI);
                    break;
                case 0xE7:
                    ADD16(SP, DI);
                    break;
                case 0xE8:
                    ADD16(BP, AX);
                    break;
                case 0xE9:
                    ADD16(BP, CX);
                    break;
                case 0xEA:
                    ADD16(BP, DX);
                    break;
                case 0xEB:
                    ADD16(BP, BX);
                    break;
                case 0xEC:
                    ADD16(BP, SP);
                    break;
                case 0xED:
                    ADD16(BP, BP);
                    break;
                case 0xEE:
                    ADD16(BP, SI);
                    break;
                case 0xEF:
                    ADD16(BP, DI);
                    break;
                case 0xF0:
                    ADD16(SI, AX);
                    break;
                case 0xF1:
                    ADD16(SI, CX);
                    break;
                case 0xF2:
                    ADD16(SI, DX);
                    break;
                case 0xF3:
                    ADD16(SI, BX);
                    break;
                case 0xF4:
                    ADD16(SI, SP);
                    break;
                case 0xF5:
                    ADD16(SI, BP);
                    break;
                case 0xF6:
                    ADD16(SI, SI);
                    break;
                case 0xF7:
                    ADD16(SI, DI);
                    break;
                case 0xF8:
                    ADD16(DI, AX);
                    break;
                case 0xF9:
                    ADD16(DI, CX);
                    break;
                case 0xFA:
                    ADD16(DI, DX);
                    break;
                case 0xFB:
                    ADD16(DI, BX);
                    break;
                case 0xFC:
                    ADD16(DI, SP);
                    break;
                case 0xFD:
                    ADD16(DI, BP);
                    break;
                case 0xFE:
                    ADD16(DI, SI);
                    break;
                case 0xFF:
                    ADD16(DI, DI);
            }
            UPDATE8(AX,CX,DX,BX,AL,AH,CL,CH,DL,DH,BL,BH);
            break;
        //add AL, imm8
        case 0x04:
            op = mem[IP++];
            ADD8(AL, op);
            AX = join(AL, AH);
            break;
        //add AX, imm16
        case 0x05:
            op = mem[IP++];
            AL = op;
            op = mem[IP++];
            AH = op;
            imm16 = join(AL, AH);
            ADD16(AX, imm16);
            if (!AX) FLAGS = FLAGS | (1 << 5);
            UPDATE8(AX,CX,DX,BX,AL,AH,CL,CH,DL,DH,BL,BH);
            break;
        case 0x80:
            op = mem[IP++];
            switch (op)
            {
                //add reg8, imm8
                case 0xC1: 
                    op = mem[IP++];
                    ADD8(CL, op);
                    CX = join(CL, CH);
                    break;
                case 0xC2: 
                    op = mem[IP++];
                    ADD8(DL, op);
                    DX = join(DL, DH);
                    break;
                case 0xC3: 
                    op = mem[IP++];
                    ADD8(BL, op);
                    BX = join(BL, BH);
                    break;
                case 0xC4: 
                    op = mem[IP++];
                    ADD8(AH, op);
                    AX = join(AL, AH);
                    break;
                case 0xC5: 
                    op = mem[IP++];
                    ADD8(CH, op);
                    CX = join(CL, CH);
                    break;
                case 0xC6: 
                    op = mem[IP++];
                    ADD8(DH, op);
                    DX = join(DL, DH);
                    break;
                case 0xC7: 
                    op = mem[IP++];
                    ADD8(BH, op);
                    BX = join(BL, BH);
                    break;
                
                //sub reg8, imm8
                case 0xE9: 
                    op = mem[IP++];
                    SUB8(CL, op);
                    CX = join(CL, CH);
                    break;
                case 0xEA: 
                    op = mem[IP++];
                    SUB8(DL, op);
                    DX = join(DL, DH);
                    break;
                case 0xEB: 
                    op = mem[IP++];
                    SUB8(BL, op);
                    BX = join(BL, BH);
                    break;
                case 0xEC: 
                    op = mem[IP++];
                    SUB8(AH, op);
                    AX = join(AL, AH);
                    break;
                case 0xED: 
                    op = mem[IP++];
                    SUB8(CH, op);
                    CX = join(CL, CH);
                    break;
                case 0xEE: 
                    op = mem[IP++];
                    SUB8(DH, op);
                    DX = join(DL, DH);
                    break;
                case 0xEF: 
                    op = mem[IP++];
                    SUB8(BH, op);
                    BX = join(BL, BH);
                    break;
            }
            break;
            
            /*case 0xC1: 
                    op = mem[IP++];
                    ADD8(CL, op);
                    CX = join(CL, CH);
                    break;
                case 0xC2: 
                    op = mem[IP++];
                    ADD8(DL, op);
                    DX = join(DL, DH);
                    break;
                case 0xC3: 
                    op = mem[IP++];
                    ADD8(BL, op);
                    BX = join(BL, BH);
                    break;
                case 0xC4: 
                    op = mem[IP++];
                    ADD8(AH, op);
                    AX = join(AL, AH);
                    break;
                case 0xC5: 
                    op = mem[IP++];
                    ADD8(CH, op);
                    CX = join(CL, CH);
                    break;
                case 0xC6: 
                    op = mem[IP++];
                    ADD8(DH, op);
                    DX = join(DL, DH);
                    break;
                case 0xC7:
                    op = mem[IP++];
                    switch(op)
                    {
                        case 0x80:
                            op = mem[IP++];
                            ADD8(BH, op);
                            BX = join(BL, BH);
                            break;
                        
                        case 0x83:
                            op = mem[IP++];
                            ADD16and8(DI, op);
                            cout << DI << endl;
                            break;
                    
                    }   
                    break;*/
                
            case 0xE9: 
                op = mem[IP++];
                SUB8(CL, op);
                CX = join(CL, CH);
                break;
            case 0xEA: 
                op = mem[IP++];
                SUB8(DL, op);
                DX = join(DL, DH);
                break;
            case 0xEB: 
                op = mem[IP++];
                SUB8(BL, op);
                BX = join(BL, BH);
                break;
            case 0xEC: 
                op = mem[IP++];
                SUB8(AH, op);
                AX = join(AL, AH);
                break;
            case 0xED: 
                op = mem[IP++];
                SUB8(CH, op);
                CX = join(CL, CH);
                break;
            case 0xEE: 
                op = mem[IP++];
                SUB8(DH, op);
                DX = join(DL, DH);
                break;
            case 0xEF: 
                op = mem[IP++];
                SUB8(BH, op);
                BX = join(BL, BH);
                break;
                
        case 0x81:
            op = mem[IP++];
            switch (op) 
            {
                //add reg16, imm16
                case 0xC1:
                    op = mem[IP++];
                    CL = op;
                    op = mem[IP++];
                    CH = op;
                    imm16 = join(CL, CH);
                    ADD16(CX, imm16);
                    break;
                case 0xC2:
                    op = mem[IP++];
                    DL = op;
                    op = mem[IP++];
                    DH = op;
                    imm16 = join(DL, DH);
                    ADD16(DX, imm16);
                    break;
                case 0xC3:
                    op = mem[IP++];
                    BL = op;
                    op = mem[IP++];
                    BH = op;
                    imm16 = join(BL, BH);
                    ADD16(BX, imm16);
                    break;
                case 0xC4:
                    op = mem[IP++];
                    imm16 = join(op, mem[IP]);
                    ADD16(SP, imm16);
                    break;
                case 0xC5:
                    op = mem[IP++];
                    imm16 = join(op, mem[IP]);
                    ADD16(BP, imm16);
                    break;
                case 0xC6:
                    op = mem[IP++];
                    imm16 = join(op, mem[IP]);
                    ADD16(SI, imm16);
                    break;
                case 0xC7:
                    op = mem[IP++];
                    imm16 = join(op, mem[IP]);
                    ADD16(DI, imm16);
                    break;

                //sub reg16, imm16
                case 0xE9:
                    op = mem[IP++];
                    CL = op;
                    op = mem[IP++];
                    CH = op;
                    imm16 = join(CL, CH);
                    SUB16(CX, imm16);
                    break;
                case 0xEA:
                    op = mem[IP++];
                    DL = op;
                    op = mem[IP++];
                    DH = op;
                    imm16 = join(DL, DH);
                    SUB16(DX, imm16);
                    break;
                case 0xEB:
                    op = mem[IP++];
                    BL = op;
                    op = mem[IP++];
                    BH = op;
                    imm16 = join(BL, BH);
                    SUB16(BX, imm16);
                    break;
                case 0xEC:
                    op = mem[IP++];
                    imm16 = join(op, mem[IP]);
                    SUB16(SP, imm16);
                    break;
                case 0xED:
                    op = mem[IP++];
                    imm16 = join(op, mem[IP]);
                    SUB16(BP, imm16);
                    break;
                case 0xEE:
                    op = mem[IP++];
                    imm16 = join(op, mem[IP]);
                    SUB16(SI, imm16);
                    break;
                case 0xEF:
                    op = mem[IP++];
                    imm16 = join(op, mem[IP]);
                    SUB16(DI, imm16);
                    break;
                
            }
            UPDATE8(AX,CX,DX,BX,AL,AH,CL,CH,DL,DH,BL,BH);
            break;
        case 0x83:
            op = mem[IP++];
            switch (op)
            {
                //add reg16, imm8
                case 0xC0:
                    op = mem[IP++];
                    ADD16and8(AX, op);
                case 0xC1:
                    op = mem[IP++];
                    ADD16and8(CX, op);
                    break;
                case 0xC2:
                    op = mem[IP++];
                    ADD16and8(DX, op);
                    break;
                case 0xC3:
                    op = mem[IP++];
                    ADD16and8(BX, op);
                    break;
                case 0xC4:
                    op = mem[IP++];
                    ADD16and8(SP, op);
                    break;
                case 0xC5:
                    op = mem[IP++];
                    ADD16and8(BP, op);
                    break;
                case 0xC6:
                    op = mem[IP++];
                    ADD16and8(SI, op);
                    break;
                case 0xC7:
                    op = mem[IP++];
                    ADD16and8(DI, op);
                    break;

                //sub reg16, imm8
                case 0xE8:
                    op = mem[IP++];
                    SUB16and8(AX, op);
                    break;
                case 0xE9:
                    op = mem[IP++];
                    SUB16and8(CX, op);
                    break;
                case 0xEA:
                    op = mem[IP++];
                    SUB16and8(DX, op);
                    break;
                case 0xEB:
                    op = mem[IP++];
                    SUB16and8(BX, op);
                    break;
                case 0xEC:
                    op = mem[IP++];
                    SUB16and8(SP, op);
                    break;
                case 0xED:
                    op = mem[IP++];
                    SUB16and8(BP, op);
                    break;
                case 0xEE:
                    op = mem[IP++];
                    SUB16and8(SI, op);
                    break;
                case 0xEF:
                    op = mem[IP++];
                    SUB16and8(DI, op);
                    break;
            }
            UPDATE8(AX,CX,DX,BX,AL,AH,CL,CH,DL,DH,BL,BH);
            break;

        //sub reg8, reg8
        case 0x2A:
		    op = mem[IP++];
		    switch (op) {
            //sub AL, reg8
                case 0xC0: case 0xC1: case 0xC2: case 0xC3: case 0xC4: case 0xC5: case 0xC6: case 0xC7:
                    if(op == 0xC0)
                        SUB8(AL, AL);
                    if(op == 0xC1)
                        SUB8(AL, CL);
                    if(op == 0xC2)
                        SUB8(AL, DL);
                    if(op == 0xC3)
                        SUB8(AL, BL);
                    if(op == 0xC4)
                        SUB8(AL, AH);
                    if(op == 0xC5)
                        SUB8(AL, CH);
                    if(op == 0xC6)
                        SUB8(AL, DH);
                    if(op == 0xC7)
                        SUB8(AL, BH);
                    AX = join(AL, AH);
                    break;
                //sub CL, reg8
                case 0xC8: case 0xC9: case 0xCA: case 0xCB: case 0xCC: case 0xCD: case 0xCE: case 0xCF:
                    if(op == 0xC8)
                        SUB8(CL, AL);
                    if(op == 0xC9)
                        SUB8(CL, CL);
                    if(op == 0xCA)
                        SUB8(CL, DL);
                    if(op == 0xCB)
                        SUB8(CL, BL);
                    if(op == 0xCC)
                        SUB8(CL, AH);
                    if(op == 0xCD)
                        SUB8(CL, CH);
                    if(op == 0xCE)
                        SUB8(CL, DH);
                    if(op == 0xCF)
                        SUB8(CL, BH);
                    CX = join(CL, CH);
                    break;
                //sub DL, reg8
                case 0xD0: case 0xD1: case 0xD2: case 0xD3: case 0xD4: case 0xD5: case 0xD6: case 0xD7:
                    if(op == 0xD0)
                        SUB8(DL, AL);
                    if(op == 0xD1)
                        SUB8(DL, CL);
                    if(op == 0xD2)
                        SUB8(DL, DL);
                    if(op == 0xD3)
                        SUB8(DL, BL);
                    if(op == 0xD4)
                        SUB8(DL, AH);
                    if(op == 0xD5)
                        SUB8(DL, CH);
                    if(op == 0xD6)
                        SUB8(DL, DH);
                    if(op == 0xD7)
                        SUB8(DL, BH);
                    DX = join(DL, DH);
                    break;
                //sub BL, reg8
                case 0xD8: case 0xD9: case 0xDA: case 0xDB: case 0xDC: case 0xDD: case 0xDE: case 0xDF:
                    if(op == 0xD8)
                        SUB8(BL, AL);
                    if(op == 0xD9)
                        SUB8(BL, CL);
                    if(op == 0xDA)
                        SUB8(BL, DL);
                    if(op == 0xDB)
                        SUB8(BL, BL);
                    if(op == 0xDC)
                        SUB8(BL, AH);
                    if(op == 0xDD)
                        SUB8(BL, CH);
                    if(op == 0xDE)
                        SUB8(BL, DH);
                    if(op == 0xDF)
                        SUB8(BL, BH);
                    BX = join(BL, BH);
                    break;
                //sub AH, reg8
                case 0xE0: case 0xE1: case 0xE2: case 0xE3: case 0xE4: case 0xE5: case 0xE6: case 0xE7:
                    if(op == 0xE0)
                        SUB8(AH, AL);
                    if(op == 0xE1)
                        SUB8(AH, CL);
                    if(op == 0xE2)
                        SUB8(AH, DL);
                    if(op == 0xE3)
                        SUB8(AH, BL);
                    if(op == 0xE4)
                        SUB8(AH, AH);
                    if(op == 0xE5)
                        SUB8(AH, CH);
                    if(op == 0xE6)
                        SUB8(AH, DH);
                    if(op == 0xE7)
                        SUB8(AH, BH);
                    AX = join(AL, AH);
                    break;
                //sub CH, reg8
                case 0xE8: case 0xE9: case 0xEA: case 0xEB: case 0xEC: case 0xED: case 0xEE: case 0xEF:
                    if(op == 0xE8)
                        SUB8(CH, AL);
                    if(op == 0xE9)
                        SUB8(CH, CL);
                    if(op == 0xEA)
                        SUB8(CH, DL);
                    if(op == 0xEB)
                        SUB8(CH, BL);
                    if(op == 0xEC)
                        SUB8(CH, AH);
                    if(op == 0xED)
                        SUB8(CH, CH);
                    if(op == 0xEE)
                        SUB8(CH, DH);
                    if(op == 0xEF)
                        SUB8(CH, BH);
                    CX = join(CL, CH);
                    break;
                //sub DH, reg8
                case 0xF0: case 0xF1: case 0xF2: case 0xF3: case 0xF4: case 0xF5: case 0xF6: case 0xF7:
                    if(op == 0xF0)
                        SUB8(DH, AL);
                    if(op == 0xF1)
                        SUB8(DH, CL);
                    if(op == 0xF2)
                        SUB8(DH, DL);
                    if(op == 0xF3)
                        SUB8(DH, BL);
                    if(op == 0xF4)
                        SUB8(DH, AH);
                    if(op == 0xF5)
                        SUB8(DH, CH);
                    if(op == 0xF6)
                        SUB8(DH, DH);
                    if(op == 0xF7)
                        SUB8(DH, BH);
                    DX = join(DL, DH);
                    break;
                //sub BH, reg8
                case 0xF8: case 0xF9: case 0xFA: case 0xFB: case 0xFC: case 0xFD: case 0xFE: case 0xFF:
                    if(op == 0xF8)
                        SUB8(BH, AL);
                    if(op == 0xF9)
                        SUB8(BH, CL);
                    if(op == 0xFA)
                        SUB8(BH, DL);
                    if(op == 0xFB)
                        SUB8(BH, BL);
                    if(op == 0xFC)
                        SUB8(BH, AH);
                    if(op == 0xFD)
                        SUB8(BH, CH);
                    if(op == 0xFE)
                        SUB8(BH, DH);
                    if(op == 0xFF)
                        SUB8(BH, BH);
                    BX = join(BL, BH);
                    break;
            }
            break;
        
        //sub reg16, reg16
        case 0x2B:
            op = mem[IP++];
            switch (op)
            {
                case 0xC0:
                    SUB16(AX, AX);
                    break;
                case 0xC1:
                    SUB16(AX, CX);
                    break;
                case 0xC2:
                    SUB16(AX, DX);
                    break;
                case 0xC3:
                    SUB16(AX, BX);
                    break;
                case 0xC4:
                    SUB16(AX, SP);
                    break;
                case 0xC5:
                    SUB16(AX, BP);
                    break;
                case 0xC6:
                    SUB16(AX, SI);
                    break;
                case 0xC7:
                    SUB16(AX, DI);
                    break;
                case 0xC8:
                    SUB16(CX, AX);
                    break;
                case 0xC9:
                    SUB16(CX, CX);
                    break;
                case 0xCA:
                    SUB16(CX, DX);
                    break;
                case 0xCB:
                    SUB16(CX, BX);
                    break;
                case 0xCC:
                    SUB16(CX, SP);
                    break;
                case 0xCD:
                    SUB16(CX, BP);
                    break;
                case 0xCE:
                    SUB16(CX, SI);
                    break;
                case 0xCF:
                    SUB16(CX, DI);
                    break;
                case 0xD0:
                    SUB16(DX, AX);
                    break;
                case 0xD1:
                    SUB16(DX, CX);
                    break;
                case 0xD2:
                    SUB16(DX, DX);
                    break;
                case 0xD3:
                    SUB16(DX, BX);
                    break;
                case 0xD4:
                    SUB16(DX, SP);
                    break;
                case 0xD5:
                    SUB16(DX, BP);
                    break;
                case 0xD6:
                    SUB16(DX, SI);
                    break;
                case 0xD7:
                    SUB16(DX, DI);
                    break;
                case 0xD8:
                    SUB16(BX, AX);
                    break;
                case 0xD9:
                    SUB16(BX, CX);
                    break;
                case 0xDA:
                    SUB16(BX, DX);
                    break;
                case 0xDB:
                    SUB16(BX, BX);
                    break;
                case 0xDC:
                    SUB16(BX, SP);
                    break;
                case 0xDD:
                    SUB16(BX, BP);
                    break;
                case 0xDE:
                    SUB16(BX, SI);
                    break;
                case 0xDF:
                    SUB16(BX, DI);
                    break;
                case 0xE0:
                    SUB16(SP, AX);
                    break;
                case 0xE1:
                    SUB16(SP, CX);
                    break;
                case 0xE2:
                    SUB16(SP, DX);
                    break;
                case 0xE3:
                    SUB16(SP, BX);
                    break;
                case 0xE4:
                    SUB16(SP, SP);
                    break;
                case 0xE5:
                    SUB16(SP, BP);
                    break;
                case 0xE6:
                    SUB16(SP, SI);
                    break;
                case 0xE7:
                    SUB16(SP, DI);
                    break;
                case 0xE8:
                    SUB16(BP, AX);
                    break;
                case 0xE9:
                    SUB16(BP, CX);
                    break;
                case 0xEA:
                    SUB16(BP, DX);
                    break;
                case 0xEB:
                    SUB16(BP, BX);
                    break;
                case 0xEC:
                    SUB16(BP, SP);
                    break;
                case 0xED:
                    SUB16(BP, BP);
                    break;
                case 0xEE:
                    SUB16(BP, SI);
                    break;
                case 0xEF:
                    SUB16(BP, DI);
                    break;
                case 0xF0:
                    SUB16(SI, AX);
                    break;
                case 0xF1:
                    SUB16(SI, CX);
                    break;
                case 0xF2:
                    SUB16(SI, DX);
                    break;
                case 0xF3:
                    SUB16(SI, BX);
                    break;
                case 0xF4:
                    SUB16(SI, SP);
                    break;
                case 0xF5:
                    SUB16(SI, BP);
                    break;
                case 0xF6:
                    SUB16(SI, SI);
                    break;
                case 0xF7:
                    SUB16(SI, DI);
                    break;
                case 0xF8:
                    SUB16(DI, AX);
                    break;
                case 0xF9:
                    SUB16(DI, CX);
                    break;
                case 0xFA:
                    SUB16(DI, DX);
                    break;
                case 0xFB:
                    SUB16(DI, BX);
                    break;
                case 0xFC:
                    SUB16(DI, SP);
                    break;
                case 0xFD:
                    SUB16(DI, BP);
                    break;
                case 0xFE:
                    SUB16(DI, SI);
                    break;
                case 0xFF:
                    SUB16(DI, DI);
                    break;
            }
            UPDATE8(AX,CX,DX,BX,AL,AH,CL,CH,DL,DH,BL,BH);
            break;

        //sub AL, imm8
        case 0x2C:
            op = mem[IP++];
            SUB8(AL, op);
            AX = join(AL, AH);
            break;
        //sub AX, imm16
        case 0x2D:
            op = mem[IP++];
            AL = op;
            op = mem[IP++];
            AH = op;
            imm16 = join(AL, AH);
            SUB16(AX, imm16);
            if (!AX) FLAGS = FLAGS | (1 << 5);
            UPDATE8(AX,CX,DX,BX,AL,AH,CL,CH,DL,DH,BL,BH);
            break;

        //mov reg8, imm8
        case 0xB0:
            op = mem[IP++];
            AL = op;
            AX = join(AL, AH);
            break;
	    case 0xB1:
            op = mem[IP++];
            CL = op;
            CX = join(CL, CH);
            break;
        case 0xB2:
            op = mem[IP++];
            DL = op;
            DX = join(DL, DH);
            break;
        case 0xB3:
            op = mem[IP++];
            BL = op;
            BX = join(BL, BH);
		    break;
        case 0xB4:
            op = mem[IP++];
            AH = op;
            AX = join(AL, AH);
            break;
        case 0xB5:
            op = mem[IP++];
            CH = op;
            CX = join(CL, CH);
            break;
	    case 0xB6:
            op = mem[IP++];
            DH = op;
            DX = join(DL, DH);
            break;
	    case 0xB7:
            op = mem[IP++];
            BH = op;
            BX = join(BL, BH);
            break;

        //mov reg16, imm16
	    case 0xB8:
            op = mem[IP++];
            AL = op;
            op = mem[IP++];
            AH = op;
            AX = join(AL, AH);
            UPDATE8(AX,CX,DX,BX,AL,AH,CL,CH,DL,DH,BL,BH);
            break;
        case 0xB9:
            op = mem[IP++];
		    CL = op;
		    op = mem[IP++];
		    CH = op;
		    CX = join(CL, CH);
            UPDATE8(AX,CX,DX,BX,AL,AH,CL,CH,DL,DH,BL,BH);
		    break;
        case 0xBA:
            op = mem[IP++];
            DL = op;
            op = mem[IP++];
            DH = op;
            DX = join(DL, DH);
            UPDATE8(AX,CX,DX,BX,AL,AH,CL,CH,DL,DH,BL,BH);
            break;
	    case 0xBB:
            op = mem[IP++];
            BL = op;
            op = mem[IP++];
            BH = op;
            BX = join(BL, BH);
            UPDATE8(AX,CX,DX,BX,AL,AH,CL,CH,DL,DH,BL,BH);
            break;
	    case 0xBC:
            op = mem[IP++];
            SP = join(op, mem[IP++]);
            break;
	    case 0xBD:
            op = mem[IP++];
            BP = join(op, mem[IP++]);
            break;
	    case 0xBE:
            op = mem[IP++];
            SI = join(op, mem[IP++]);
            break;
	    case 0xBF:
            op = mem[IP++];
            DI = join(op, mem[IP++]);
            break;

        //mov reg8, reg8
        case 0x8A:
            op = mem[IP++];
            switch (op)
            {
                //mov reg8 by itself
                case 0xC0: case 0xC9: case 0xD2: case 0xDB: case 0xE4: case 0xED: case 0xF6: case 0xFF:
                    break;
                //mov AL, reg8
                case 0xC1: case 0xC2: case 0xC3: case 0xC4: case 0xC5: case 0xC6: case 0xC7:
                    if (op == 0xC1)
                        AL = CL;
                    if (op == 0xC2)
                        AL = DL;
                    if (op == 0xC3)
                        AL = BL;
                    if (op == 0xC4)
                        AL = AH;
                    if (op == 0xC5)
                        AL = CH;
                    if (op == 0xC6)
                        AL = DH;
                    if (op == 0xC7)
                        AL = BH;
                    AX = join(AL, AH);
                    break;
                
                //mov CL, reg8
                case 0xC8: case 0xCA: case 0xCB: case 0xCC: case 0xCD: case 0xCE: case 0xCF:
                    if (op == 0xC8)
                        CL = AL;
                    if (op == 0xCA)
                        CL = DL;
                    if (op == 0xCB)
                        CL = BL;
                    if (op == 0xCC)
                        CL = AH;
                    if (op == 0xCD)
                        CL = CH;
                    if (op == 0xCE)
                        CL = DH;
                    if (op == 0xCF)
                        CL = BH;
                    CX = join(CL, CH);
                    break;

                //mov DL, reg8
                case 0xD0: case 0xD1: case 0xD3: case 0xD4: case 0xD5: case 0xD6: case 0xD7:
                    if (op == 0xD0)
                        DL = AL;
                    if (op == 0xD1)
                        DL = CL;
                    if (op == 0xD3)
                        DL = BL;
                    if (op == 0xD4)
                        DL = AH;
                    if (op == 0xD5)
                        DL = CH;
                    if (op == 0xD6)
                        DL = DH;
                    if (op == 0xD7)
                        DL = BH;
                    DX = join(DL, DH);
                    break;

                //mov BL, reg8
                case 0xD8: case 0xD9: case 0xDA: case 0xDC: case 0xDD: case 0xDE: case 0xDF:
                    if (op == 0xD8)
                        BL = AL;
                    if (op == 0xD9)
                        BL = CL;
                    if (op == 0xDA)
                        BL = DL;
                    if (op == 0xDC)
                        BL = AH;
                    if (op == 0xDD)
                        BL = CH;
                    if (op == 0xDE)
                        BL = DH;
                    if (op == 0xDF)
                        BL = BH;
                    BX = join(BL, BH);
                    break;

                //mov AH, reg8
                case 0xE0: case 0xE1: case 0xE2: case 0xE3: case 0xE5: case 0xE6: case 0xE7:
                    if (op == 0xE0)
                        AH = AL;
                    if (op == 0xE1)
                        AH = CL;
                    if (op == 0xE2)
                        AH = DL;
                    if (op == 0xE3)
                        AH = BL;
                    if (op == 0xE5)
                        AH = CH;
                    if (op == 0xE6)
                        AH = DH;
                    if (op == 0xE7)
                        AH = BH;
                    AX = join(AL, AH);
                    break;

                //mov CH, reg8
                case 0xE8: case 0xE9: case 0xEA: case 0xEB: case 0xEC: case 0xEE: case 0xEF:
                    if (op == 0xE8)
                        CH = AL;
                    if (op == 0xE9)
                        CH = CL;
                    if (op == 0xEA)
                        CH = DL;
                    if (op == 0xEB)
                        CH = BL;
                    if (op == 0xEC)
                        CH = AH;
                    if (op == 0xEE)
                        CH = DH;
                    if (op == 0xEF)
                        CH = BH;
                    CX = join(CL, CH);
                    break;

                //mov DH, reg8
                case 0xF0: case 0xF1: case 0xF2: case 0xF3: case 0xF4: case 0xF5: case 0xF7:
                    if (op == 0xF0)
                        DH = AL;
                    if (op == 0xF1)
                        DH = CL;
                    if (op == 0xF2)
                        DH = DL;
                    if (op == 0xF3)
                        DH = BL;
                    if (op == 0xF4)
                        DH = AH;
                    if (op == 0xF5)
                        DH = CH;
                    if (op == 0xF7)
                        DH = BH;
                    DX = join(DL, DH);
                    break; 

                //mov BH, reg8
                case 0xF8: case 0xF9: case 0xFA: case 0xFB: case 0xFC: case 0xFD: case 0xFE:
                    if (op == 0xF8)
                        BH = AL;
                    if (op == 0xF9)
                        BH = CL;
                    if (op == 0xFA)
                        BH = DL;
                    if (op == 0xFB)
                        BH = BL;
                    if (op == 0xFC)
                        BH = AH;
                    if (op == 0xFD)
                        BH = CH;
                    if (op == 0xFE)
                        BH = DH;
                    BX = join(BL, BH);
                    break;     
            }
            break;

        //mov reg16, reg16
        case 0x8B:
            op = mem[IP++];
            switch (op)
            {
                //mov reg16 by itself
                case 0xC0: case 0xC9: case 0xD2: case 0xDB: case 0xE4: case 0xED: case 0xF6: case 0xFF:
                    break;
                //mov AX, reg16
                case 0xC1: case 0xC2: case 0xC3: case 0xC4: case 0xC5: case 0xC6: case 0xC7:
                    if (op == 0xC1)
                        AX = CX;
                    if (op == 0xC2)
                        AX = DX;
                    if (op == 0xC3)
                        AX = BX;
                    if (op == 0xC4)
                        AX = SP;
                    if (op == 0xC5)
                        AX = BP;
                    if (op == 0xC6)
                        AX = SI;
                    if (op == 0xC7)
                        AX = DI;
                    break;
                
                //mov CX, reg16
                case 0xC8: case 0xCA: case 0xCB: case 0xCC: case 0xCD: case 0xCE: case 0xCF:
                    if (op == 0xC8)
                        CX = AX;
                    if (op == 0xCA)
                        CX = DX;
                    if (op == 0xCB)
                        CX = BX;
                    if (op == 0xCC)
                        CX = SP;
                    if (op == 0xCD)
                        CX = BP;
                    if (op == 0xCE)
                        CX = SI;
                    if (op == 0xCF)
                        CX = DI;
                    break;

                //mov DX, reg16
                case 0xD0: case 0xD1: case 0xD3: case 0xD4: case 0xD5: case 0xD6: case 0xD7:
                    if (op == 0xD0)
                        DX = AX;
                    if (op == 0xD1)
                        DX = CX;
                    if (op == 0xD3)
                        DX = BX;
                    if (op == 0xD4)
                        DX = SP;
                    if (op == 0xD5)
                        DX = BP;
                    if (op == 0xD6)
                        DX = SI;
                    if (op == 0xD7)
                        DX = DI;
                    break;

                //mov BX, reg16
                case 0xD8: case 0xD9: case 0xDA: case 0xDC: case 0xDD: case 0xDE: case 0xDF:
                    if (op == 0xD8)
                        BX = AX;
                    if (op == 0xD9)
                        BX = CX;
                    if (op == 0xDA)
                        BX = DX;
                    if (op == 0xDC)
                        BX = SP;
                    if (op == 0xDD)
                        BX = BP;
                    if (op == 0xDE)
                        BX = SI;
                    if (op == 0xDF)
                        BX = DI;
                    break;

                //mov SP, reg16
                case 0xE0: case 0xE1: case 0xE2: case 0xE3: case 0xE5: case 0xE6: case 0xE7:
                    if (op == 0xE0)
                        SP = AX;
                    if (op == 0xE1)
                        SP = CX;
                    if (op == 0xE2)
                        SP = DX;
                    if (op == 0xE3)
                        SP = BX;
                    if (op == 0xE5)
                        SP = BP;
                    if (op == 0xE6)
                        SP = SI;
                    if (op == 0xE7)
                        SP = DI;
                    break;

                //mov BP, reg16
                case 0xE8: case 0xE9: case 0xEA: case 0xEB: case 0xEC: case 0xEE: case 0xEF:
                    if (op == 0xE8)
                        BP = AX;
                    if (op == 0xE9)
                        BP = CX;
                    if (op == 0xEA)
                        BP = DX;
                    if (op == 0xEB)
                        BP = BX;
                    if (op == 0xEC)
                        BP = SP;
                    if (op == 0xEE)
                        BP = DH;
                    if (op == 0xEF)
                        BP = BH;
                    break;

                //mov SI, reg16
                case 0xF0: case 0xF1: case 0xF2: case 0xF3: case 0xF4: case 0xF5: case 0xF7:
                    if (op == 0xF0)
                        SI = AX;
                    if (op == 0xF1)
                        SI = CX;
                    if (op == 0xF2)
                        SI = DX;
                    if (op == 0xF3)
                        SI = BX;
                    if (op == 0xF4)
                        SI = SP;
                    if (op == 0xF5)
                        SI = BP;
                    if (op == 0xF7)
                        SI = DI;
                    break; 

                //mov DI, reg16
                case 0xF8: case 0xF9: case 0xFA: case 0xFB: case 0xFC: case 0xFD: case 0xFE:
                    if (op == 0xF8)
                        DI = AX;
                    if (op == 0xF9)
                        DI = CX;
                    if (op == 0xFA)
                        DI = DX;
                    if (op == 0xFB)
                        DI = BX;
                    if (op == 0xFC)
                        DI = SP;
                    if (op == 0xFD)
                        DI = BP;
                    if (op == 0xFE)
                        DI = SI;
                    break;     
            }
            UPDATE8 (AX,CX,DX,BX,AL,AH,CL,CH,DL,DH,BL,BH);
            break;

        //mov sreg, reg16
        case 0x8E:
            op = mem[IP++];
            switch(op)
            {
                //mov ES, reg16
                case 0xC0: case 0xC1: case 0xC2: case 0xC3: case 0xC4: case 0xC5: case 0xC6: case 0xC7:
                    if(op == 0xC0)
                        ES = AX;
                    if(op == 0xC1)
                        ES = CX;
                    if(op == 0xC2)
                        ES = DX;
                    if(op == 0xC3)
                        ES = BX;
                    if(op == 0xC4)
                        ES = SP;
                    if(op == 0xC5)
                        ES = BP;
                    if(op == 0xC6)
                        ES = SI;
                    if(op == 0xC7)
                        ES = DI;
                    break;

                //mov SS, reg16
                case 0xD0: case 0xD1: case 0xD2: case 0xD3: case 0xD4: case 0xD5: case 0xD6: case 0xD7:
                    if(op == 0xD0)
                        SS = AX;
                    if(op == 0xD1)
                        SS = CX;
                    if(op == 0xD2)
                        SS = DX;
                    if(op == 0xD3)
                        SS = BX;
                    if(op == 0xD4)
                        SS = SP;
                    if(op == 0xD5)
                        SS = BP;
                    if(op == 0xD6)
                        SS = SI;
                    if(op == 0xD7)
                        SS = DI;
                    break;

                //mov DS, reg16
                case 0xD8: case 0xD9: case 0xDA: case 0xDB: case 0xDC: case 0xDD: case 0xDE: case 0xDF:
                    if(op == 0xD8)
                        DS = AX;
                    if(op == 0xD9)
                        DS = CX;
                    if(op == 0xDA)
                        DS = DX;
                    if(op == 0xDB)
                        DS = BX;
                    if(op == 0xDC)
                        DS = SP;
                    if(op == 0xDD)
                        DS = BP;
                    if(op == 0xDE)
                        DS = SI;
                    if(op == 0xDF)
                        DS = DI;
                    break;
            }
            break;
        
        //xor reg8, reg8
        case 0x32:
            op = mem[IP++];
            switch(op)
            {
                //xor AL, reg8
                case 0xC0: case 0xC1: case 0xC2: case 0xC3: case 0xC4: case 0xC5: case 0xC6: case 0xC7:
                    if(op == 0xC0)
                        AL = AL ^ AL;
                    if(op == 0xC1)
                        AL = AL ^ CL;
                    if(op == 0xC2)
                        AL = AL ^ DL;
                    if(op == 0xC3)
                        AL = AL ^ BL;
                    if(op == 0xC4)
                        AL = AL ^ AH;
                    if(op == 0xC5)
                        AL = AL ^ CH;
                    if(op == 0xC6)
                        AL = AL ^ DH;
                    if(op == 0xC7)
                        AL = AL ^ BH;
                    AX = join(AL, AH);
                    break;

                //xor CL, reg8
                case 0xC8: case 0xC9: case 0xCA: case 0xCB: case 0xCC: case 0xCD: case 0xCE: case 0xCF:
                    if(op == 0xC8)
                        CL = CL ^ AL;
                    if(op == 0xC9)
                        CL = CL ^ CL;
                    if(op == 0xCA)
                        CL = CL ^ DL;
                    if(op == 0xCB)
                        CL = CL ^ BL;
                    if(op == 0xCC)
                        CL = CL ^ AH;
                    if(op == 0xCD)
                        CL = CL ^ CH;
                    if(op == 0xCE)
                        CL = CL ^ DH;
                    if(op == 0xCF)
                        CL = CL ^ BH;
                    CX = join(CL, CH);
                    break;
                
                //xor DL, reg8
                case 0xD0: case 0xD1: case 0xD2: case 0xD3: case 0xD4: case 0xD5: case 0xD6: case 0xD7:
                    if(op == 0xD0)
                        DL = DL ^ AL;
                    if(op == 0xD1)
                        DL = DL ^ CL;
                    if(op == 0xD2)
                        DL = DL ^ DL;
                    if(op == 0xD3)
                        DL = DL ^ BL;
                    if(op == 0xD4)
                        DL = DL ^ AH;
                    if(op == 0xD5)
                        DL = DL ^ CH;
                    if(op == 0xD6)
                        DL = DL ^ DH;
                    if(op == 0xD7)
                        DL = DL ^ BH;
                    DX = join(DL, DH);
                    break;

                //xor BL, reg8
                case 0xD8: case 0xD9: case 0xDA: case 0xDB: case 0xDC: case 0xDD: case 0xDE: case 0xDF:
                    if(op == 0xD8)
                        BL = BL ^ AL;
                    if(op == 0xD9)
                        BL = BL ^ CL;
                    if(op == 0xDA)
                        BL = BL ^ DL;
                    if(op == 0xDB)
                        BL = BL ^ BL;
                    if(op == 0xDC)
                        BL = BL ^ AH;
                    if(op == 0xDD)
                        BL = BL ^ CH;
                    if(op == 0xDE)
                        BL = BL ^ DH;
                    if(op == 0xDF)
                        BL = BL ^ BH;
                    BX = join(BL, BH);
                    break;

                //xor AH, reg8
                case 0xE0: case 0xE1: case 0xE2: case 0xE3: case 0xE4: case 0xE5: case 0xE6: case 0xE7:
                    if(op == 0xE0)
                        AH = AH ^ AL;
                    if(op == 0xE1)
                        AH = AH ^ CL;
                    if(op == 0xE2)
                        AH = AH ^ DL;
                    if(op == 0xE3)
                        AH = AH ^ BL;
                    if(op == 0xE4)
                        AH = AH ^ AH;
                    if(op == 0xE5)
                        AH = AH ^ CH;
                    if(op == 0xE6)
                        AH = AH ^ DH;
                    if(op == 0xE7)
                        AH = AH ^ BH;
                    AX = join(AL, AH);
                    break;

                //xor CH, reg8
                case 0xE8: case 0xE9: case 0xEA: case 0xEB: case 0xEC: case 0xED: case 0xEE: case 0xEF:
                    if(op == 0xC8)
                        CH = CH ^ AL;
                    if(op == 0xC9)
                        CH = CH ^ CL;
                    if(op == 0xCA)
                        CH = CH ^ DL;
                    if(op == 0xCB)
                        CH = CH ^ BL;
                    if(op == 0xCC)
                        CH = CH ^ AH;
                    if(op == 0xCD)
                        CH = CH ^ CH;
                    if(op == 0xCE)
                        CH = CH ^ DH;
                    if(op == 0xCF)
                        CH = CH ^ BH;
                    CX = join(CL, CH);
                    break;

                //xor DH, reg8
                case 0xF0: case 0xF1: case 0xF2: case 0xF3: case 0xF4: case 0xF5: case 0xF6: case 0xF7:
                    if(op == 0xF0)
                        DH = DH ^ AL;
                    if(op == 0xF1)
                        DH = DH ^ CL;
                    if(op == 0xF2)
                        DH = DH ^ DL;
                    if(op == 0xF3)
                        DH = DH ^ BL;
                    if(op == 0xF4)
                        DH = DH ^ AH;
                    if(op == 0xF5)
                        DH = DH ^ CH;
                    if(op == 0xF6)
                        DH = DH ^ DH;
                    if(op == 0xF7)
                        DH = DH ^ BH;
                    DX = join(DL, DH);
                    break;

                //xor BH, reg8
                case 0xF8: case 0xF9: case 0xFA: case 0xFB: case 0xFC: case 0xFD: case 0xFE: case 0xFF:
                    if(op == 0xF8)
                        BH = BH ^ AL;
                    if(op == 0xF9)
                        BH = BH ^ CL;
                    if(op == 0xFA)
                        BH = BH ^ DL;
                    if(op == 0xFB)
                        BH = BH ^ BL;
                    if(op == 0xFC)
                        BH = BH ^ AH;
                    if(op == 0xFD)
                        BH = BH ^ CH;
                    if(op == 0xFE)
                        BH = BH ^ DH;
                    if(op == 0xFF)
                        BH = BH ^ BH;
                    BX = join(BL, BH);
                    break;
            }
            break;

        //xor reg16, reg16
        case 0x33:
            op = mem[IP++];
            switch(op)
            {
                //xor AX, reg16
                case 0xC0: case 0xC1: case 0xC2: case 0xC3: case 0xC4: case 0xC5: case 0xC6: case 0xC7:
                    if(op == 0xC0)
                        AX = AX ^ AX;
                    if(op == 0xC1)
                        AX = AX ^ CX;
                    if(op == 0xC2)
                        AX = AX ^ DX;
                    if(op == 0xC3)
                        AX = AX ^ BX;
                    if(op == 0xC4)
                        AX = AX ^ SP;
                    if(op == 0xC5)
                        AX = AX ^ BP;
                    if(op == 0xC6)
                        AX = AX ^ SI;
                    if(op == 0xC7)
                        AX = AX ^ DI;
                    break;
                
                //xor CX, reg16
                case 0xC8: case 0xC9: case 0xCA: case 0xCB: case 0xCC: case 0xCD: case 0xCE: case 0xCF:
                    if(op == 0xC8)
                        CX = CX ^ AX;
                    if(op == 0xC9)
                        CX = CX ^ CX;
                    if(op == 0xCA)
                        CX = CX ^ DX;
                    if(op == 0xCB)
                        CX = CX ^ BX;
                    if(op == 0xCC)
                        CX = CX ^ SP;
                    if(op == 0xCD)
                        CX = CX ^ BP;
                    if(op == 0xCE)
                        CX = CX ^ SI;
                    if(op == 0xCF)
                        CX = CX ^ DI;
                    break;

                //xor DX, reg16
                case 0xD0: case 0xD1: case 0xD2: case 0xD3: case 0xD4: case 0xD5: case 0xD6: case 0xD7:
                    if(op == 0xD0)
                        DX = DX ^ AX;
                    if(op == 0xD1)
                        DX = DX ^ CX;
                    if(op == 0xD2)
                        DX = DX ^ DX;
                    if(op == 0xD3)
                        DX = DX ^ BX;
                    if(op == 0xD4)
                        DX = DX ^ SP;
                    if(op == 0xD5)
                        DX = DX ^ BP;
                    if(op == 0xD6)
                        DX = DX ^ SI;
                    if(op == 0xD7)
                        DX = DX ^ DI;
                    break;
                
                //xor BX, reg16
                case 0xD8: case 0xD9: case 0xDA: case 0xDB: case 0xDC: case 0xDD: case 0xDE: case 0xDF:
                    if(op == 0xD0)
                        BX = BX ^ AX;
                    if(op == 0xD1)
                        BX = BX ^ CX;
                    if(op == 0xD2)
                        BX = BX ^ DX;
                    if(op == 0xD3)
                        BX = BX ^ BX;
                    if(op == 0xD4)
                        BX = BX ^ SP;
                    if(op == 0xD5)
                        BX = BX ^ BP;
                    if(op == 0xD6)
                        BX = BX ^ SI;
                    if(op == 0xD7)
                        BX = BX ^ DI;
                    break;

                //xor SP, reg16
                case 0xE0: case 0xE1: case 0xE2: case 0xE3: case 0xE4: case 0xE5: case 0xE6: case 0xE7:
                    if(op == 0xE0)
                        SP = SP ^ AX;
                    if(op == 0xE1)
                        SP = SP ^ CX;
                    if(op == 0xE2)
                        SP = SP ^ DX;
                    if(op == 0xE3)
                        SP = SP ^ BX;
                    if(op == 0xE4)
                        SP = SP ^ SP;
                    if(op == 0xE5)
                        SP = SP ^ BP;
                    if(op == 0xE6)
                        SP = SP ^ SI;
                    if(op == 0xE7)
                        SP = SP ^ DI;
                    break;

                //xor BP, reg16
                case 0xE8: case 0xE9: case 0xEA: case 0xEB: case 0xEC: case 0xED: case 0xEE: case 0xEF:
                    if(op == 0xE0)
                        BP = BP ^ AX;
                    if(op == 0xE1)
                        BP = BP ^ CX;
                    if(op == 0xE2)
                        BP = BP ^ DX;
                    if(op == 0xE3)
                        BP = BP ^ BX;
                    if(op == 0xE4)
                        BP = BP ^ SP;
                    if(op == 0xE5)
                        BP = BP ^ BP;
                    if(op == 0xE6)
                        BP = BP ^ SI;
                    if(op == 0xE7)
                        BP = BP ^ DI;
                    break;

                //xor SI, reg16
                case 0xF0: case 0xF1: case 0xF2: case 0xF3: case 0xF4: case 0xF5: case 0xF6: case 0xF7:
                    if(op == 0xF0)
                        SI = SI ^ AX;
                    if(op == 0xF1)
                        SI = SI ^ CX;
                    if(op == 0xF2)
                        SI = SI ^ DX;
                    if(op == 0xF3)
                        SI = SI ^ BX;
                    if(op == 0xF4)
                        SI = SI ^ SP;
                    if(op == 0xF5)
                        SI = SI ^ BP;
                    if(op == 0xF6)
                        SI = SI ^ SI;
                    if(op == 0xF7)
                        SI = SI ^ DI;
                    break;

                //xor DI, reg16
                case 0xF8: case 0xF9: case 0xFA: case 0xFB: case 0xFC: case 0xFD: case 0xFE: case 0xFF:
                    if(op == 0xF8)
                        DI = DI ^ AX;
                    if(op == 0xF9)
                        DI = DI ^ CX;
                    if(op == 0xFA)
                        DI = DI ^ DX;
                    if(op == 0xFB)
                        DI = DI ^ BX;
                    if(op == 0xFC)
                        DI = DI ^ SP;
                    if(op == 0xFD)
                        DI = DI ^ BP;
                    if(op == 0xFE)
                        DI = DI ^ SI;
                    if(op == 0xFF)
                        DI = DI ^ DI;
                    break;
            }
            UPDATE8(AX,CX,DX,BX,AL,AH,CL,CH,DL,DH,BL,BH);
            break;

        case 0x26:
            op = mem[IP++];
            switch(op)
            {
                case 0x89:
                    op = mem[IP++];
                    switch(op)
                    {
                        case 0x05:
                            if (CX <= 2000)
                            {
                                BG = (AH & 0xF0) << 1;
                                FG = AH & 0xF;
                                if (AH <= 0xF)
                                {
                                    cout << "\033[40m";
                                    switch (AH)
                                    {
                                        case 0x0:
                                            cout << "\033[30m" << AL  << "\033[0m";
                                            break;
                                        case 0x1:
                                            cout << "\033[34m" << AL  << "\033[0m";
                                            break;
                                        case 0x2:
                                            cout << "\033[32m" << AL  << "\033[0m"; 
                                            break;
                                        case 0x3:
                                            cout << "\033[36m" << AL  << "\033[0m"; 
                                            break;
                                        case 0x4:
                                            cout << "\033[31m" << AL  << "\033[0m"; 
                                            break;
                                        case 0x5:
                                            cout << "\033[35m" << AL  << "\033[0m"; 
                                            break;
                                        case 0x6:
                                            cout << "\033[38;5;94m" << AL  << "\033[0m"; 
                                            break;
                                        case 0x7:
                                            cout << "\033[38;5;249m" << AL  << "\033[0m";
                                            break; 
                                        case 0x8:
                                            cout << "\033[38;5;240m" << AL  << "\033[0m";
                                            break; 
                                        case 0x9:
                                            cout << "\033[38;5;12m" << AL  << "\033[0m";
                                            break;
                                        case 0xA:
                                            cout << "\033[38;5;10m" << AL  << "\033[0m";
                                            break;
                                        case 0xB:
                                            cout << "\033[38;5;14m" << AL  << "\033[0m";
                                            break;
                                        case 0xC:
                                            cout << "\033[38;5;9m" << AL  << "\033[0m";
                                            break;
                                        case 0xD:
                                            cout << "\033[38;5;13m" << AL  << "\033[0m";
                                            break;
                                        case 0xE:
                                            cout << "\033[33m" << AL  << "\033[0m";
                                            break;
                                        case 0xF:
                                            cout << "\033[37m" << AL  << "\033[0m";
                                            break;
                                    }

                                }
                                if ((AH >= 0x10) && (AH <= 0x1F))
                                {
                                    cout << "\033[44m";
                                    switch (AH)
                                    {
                                        case 0x10:
                                            cout << "\033[30m" << AL  << "\033[0m";
                                            break;
                                        case 0x11:
                                            cout << "\033[34m" << AL  << "\033[0m";
                                            break;
                                        case 0x12:
                                            cout << "\033[32m" << AL  << "\033[0m"; 
                                            break;
                                        case 0x13:
                                            cout << "\033[36m" << AL  << "\033[0m"; 
                                            break;
                                        case 0x14:
                                            cout << "\033[31m" << AL  << "\033[0m"; 
                                            break;
                                        case 0x15:
                                            cout << "\033[35m" << AL  << "\033[0m"; 
                                            break;
                                        case 0x16:
                                            cout << "\033[38;5;94m" << AL  << "\033[0m"; 
                                            break;
                                        case 0x17:
                                            cout << "\033[38;5;249m" << AL  << "\033[0m";
                                            break; 
                                        case 0x18:
                                            cout << "\033[38;5;240m" << AL  << "\033[0m";
                                            break; 
                                        case 0x19:
                                            cout << "\033[38;5;12m" << AL  << "\033[0m";
                                            break;
                                        case 0x1A:
                                            cout << "\033[38;5;10m" << AL  << "\033[0m";
                                            break;
                                        case 0x1B:
                                            cout << "\033[38;5;14m" << AL  << "\033[0m";
                                            break;
                                        case 0x1C:
                                            cout << "\033[38;5;9m" << AL  << "\033[0m";
                                            break;
                                        case 0x1D:
                                            cout << "\033[38;5;13m" << AL  << "\033[0m";
                                            break;
                                        case 0x1E:
                                            cout << "\033[33m" << AL  << "\033[0m";
                                            break;
                                        case 0x1F:
                                            cout << "\033[37m" << AL  << "\033[0m";
                                            break;
                                    }
                                }
                                if ((AH >= 0x20) && (AH <= 0x2F))
                                {
                                    cout << "\033[42m";
                                    switch (AH)
                                    {
                                        case 0x20:
                                            cout << "\033[30m" << AL  << "\033[0m";
                                            break;
                                        case 0x21:
                                            cout << "\033[34m" << AL  << "\033[0m";
                                            break;
                                        case 0x22:
                                            cout << "\033[32m" << AL  << "\033[0m"; 
                                            break;
                                        case 0x23:
                                            cout << "\033[36m" << AL  << "\033[0m"; 
                                            break;
                                        case 0x24:
                                            cout << "\033[31m" << AL  << "\033[0m"; 
                                            break;
                                        case 0x25:
                                            cout << "\033[35m" << AL  << "\033[0m"; 
                                            break;
                                        case 0x26:
                                            cout << "\033[38;5;94m" << AL  << "\033[0m"; 
                                            break;
                                        case 0x27:
                                            cout << "\033[38;5;249m" << AL  << "\033[0m";
                                            break; 
                                        case 0x28:
                                            cout << "\033[38;5;240m" << AL  << "\033[0m";
                                            break; 
                                        case 0x29:
                                            cout << "\033[38;5;12m" << AL  << "\033[0m";
                                            break;
                                        case 0x2A:
                                            cout << "\033[38;5;10m" << AL  << "\033[0m";
                                            break;
                                        case 0x2B:
                                            cout << "\033[38;5;14m" << AL  << "\033[0m";
                                            break;
                                        case 0x2C:
                                            cout << "\033[38;5;9m" << AL  << "\033[0m";
                                            break;
                                        case 0x2D:
                                            cout << "\033[38;5;13m" << AL  << "\033[0m";
                                            break;
                                        case 0x2E:
                                            cout << "\033[33m" << AL  << "\033[0m";
                                            break;
                                        case 0x2F:
                                            cout << "\033[37m" << AL  << "\033[0m";
                                            break;
                                    }
                                }
                                if ((AH >= 0x30) && (AH <= 0x3F))
                                {
                                    cout << "\033[46m";
                                    switch (AH)
                                    {
                                        case 0x30:
                                            cout << "\033[30m" << AL  << "\033[0m";
                                            break;
                                        case 0x31:
                                            cout << "\033[34m" << AL  << "\033[0m";
                                            break;
                                        case 0x32:
                                            cout << "\033[32m" << AL  << "\033[0m"; 
                                            break;
                                        case 0x33:
                                            cout << "\033[36m" << AL  << "\033[0m"; 
                                            break;
                                        case 0x34:
                                            cout << "\033[31m" << AL  << "\033[0m"; 
                                            break;
                                        case 0x35:
                                            cout << "\033[35m" << AL  << "\033[0m"; 
                                            break;
                                        case 0x36:
                                            cout << "\033[38;5;94m" << AL  << "\033[0m"; 
                                            break;
                                        case 0x37:
                                            cout << "\033[38;5;249m" << AL  << "\033[0m";
                                            break; 
                                        case 0x38:
                                            cout << "\033[38;5;240m" << AL  << "\033[0m";
                                            break; 
                                        case 0x39:
                                            cout << "\033[38;5;12m" << AL  << "\033[0m";
                                            break;
                                        case 0x3A:
                                            cout << "\033[38;5;10m" << AL  << "\033[0m";
                                            break;
                                        case 0x3B:
                                            cout << "\033[38;5;14m" << AL  << "\033[0m";
                                            break;
                                        case 0x3C:
                                            cout << "\033[38;5;9m" << AL  << "\033[0m";
                                            break;
                                        case 0x3D:
                                            cout << "\033[38;5;13m" << AL  << "\033[0m";
                                            break;
                                        case 0x3E:
                                            cout << "\033[33m" << AL  << "\033[0m";
                                            break;
                                        case 0x3F:
                                            cout << "\033[37m" << AL  << "\033[0m";
                                            break;
                                    }
                                }
                                if ((AH >= 0x40) && (AH <= 0x4F))
                                {
                                    cout << "\033[41m";
                                    switch (AH)
                                    {
                                        case 0x40:
                                            cout << "\033[30m" << AL  << "\033[0m";
                                            break;
                                        case 0x41:
                                            cout << "\033[34m" << AL  << "\033[0m";
                                            break;
                                        case 0x42:
                                            cout << "\033[32m" << AL  << "\033[0m"; 
                                            break;
                                        case 0x43:
                                            cout << "\033[36m" << AL  << "\033[0m"; 
                                            break;
                                        case 0x44:
                                            cout << "\033[31m" << AL  << "\033[0m"; 
                                            break;
                                        case 0x45:
                                            cout << "\033[35m" << AL  << "\033[0m"; 
                                            break;
                                        case 0x46:
                                            cout << "\033[38;5;94m" << AL  << "\033[0m"; 
                                            break;
                                        case 0x47:
                                            cout << "\033[38;5;249m" << AL  << "\033[0m";
                                            break; 
                                        case 0x48:
                                            cout << "\033[38;5;240m" << AL  << "\033[0m";
                                            break; 
                                        case 0x49:
                                            cout << "\033[38;5;12m" << AL  << "\033[0m";
                                            break;
                                        case 0x4A:
                                            cout << "\033[38;5;10m" << AL  << "\033[0m";
                                            break;
                                        case 0x4B:
                                            cout << "\033[38;5;14m" << AL  << "\033[0m";
                                            break;
                                        case 0x4C:
                                            cout << "\033[38;5;9m" << AL  << "\033[0m";
                                            break;
                                        case 0x4D:
                                            cout << "\033[38;5;13m" << AL  << "\033[0m";
                                            break;
                                        case 0x4E:
                                            cout << "\033[33m" << AL  << "\033[0m";
                                            break;
                                        case 0x4F:
                                            cout << "\033[37m" << AL  << "\033[0m";
                                            break;
                                    }
                                }
                                if ((AH >= 0x50) && (AH <= 0x5F))
                                {
                                    cout << "\033[45m";
                                    switch (AH)
                                    {
                                        case 0x50:
                                            cout << "\033[30m" << AL  << "\033[0m";
                                            break;
                                        case 0x51:
                                            cout << "\033[34m" << AL  << "\033[0m";
                                            break;
                                        case 0x52:
                                            cout << "\033[32m" << AL  << "\033[0m"; 
                                            break;
                                        case 0x53:
                                            cout << "\033[36m" << AL  << "\033[0m"; 
                                            break;
                                        case 0x54:
                                            cout << "\033[31m" << AL  << "\033[0m"; 
                                            break;
                                        case 0x55:
                                            cout << "\033[35m" << AL  << "\033[0m"; 
                                            break;
                                        case 0x56:
                                            cout << "\033[38;5;94m" << AL  << "\033[0m"; 
                                            break;
                                        case 0x57:
                                            cout << "\033[38;5;249m" << AL  << "\033[0m";
                                            break; 
                                        case 0x58:
                                            cout << "\033[38;5;240m" << AL  << "\033[0m";
                                            break; 
                                        case 0x59:
                                            cout << "\033[38;5;12m" << AL  << "\033[0m";
                                            break;
                                        case 0x5A:
                                            cout << "\033[38;5;10m" << AL  << "\033[0m";
                                            break;
                                        case 0x5B:
                                            cout << "\033[38;5;14m" << AL  << "\033[0m";
                                            break;
                                        case 0x5C:
                                            cout << "\033[38;5;9m" << AL  << "\033[0m";
                                            break;
                                        case 0x5D:
                                            cout << "\033[38;5;13m" << AL  << "\033[0m";
                                            break;
                                        case 0x5E:
                                            cout << "\033[33m" << AL  << "\033[0m";
                                            break;
                                        case 0x5F:
                                            cout << "\033[37m" << AL  << "\033[0m";
                                            break;
                                    }
                                }
                                if ((AH >= 0x60) && (AH <= 0x6F))
                                {
                                    cout << "\033[48;5;94m";
                                    switch (AH)
                                    {
                                        case 0x60:
                                            cout << "\033[30m" << AL  << "\033[0m";
                                            break;
                                        case 0x61:
                                            cout << "\033[34m" << AL  << "\033[0m";
                                            break;
                                        case 0x62:
                                            cout << "\033[32m" << AL  << "\033[0m"; 
                                            break;
                                        case 0x63:
                                            cout << "\033[36m" << AL  << "\033[0m"; 
                                            break;
                                        case 0x64:
                                            cout << "\033[31m" << AL  << "\033[0m"; 
                                            break;
                                        case 0x65:
                                            cout << "\033[35m" << AL  << "\033[0m"; 
                                            break;
                                        case 0x66:
                                            cout << "\033[38;5;94m" << AL  << "\033[0m"; 
                                            break;
                                        case 0x67:
                                            cout << "\033[38;5;249m" << AL  << "\033[0m";
                                            break; 
                                        case 0x68:
                                            cout << "\033[38;5;240m" << AL  << "\033[0m";
                                            break; 
                                        case 0x69:
                                            cout << "\033[38;5;12m" << AL  << "\033[0m";
                                            break;
                                        case 0x6A:
                                            cout << "\033[38;5;10m" << AL  << "\033[0m";
                                            break;
                                        case 0x6B:
                                            cout << "\033[38;5;14m" << AL  << "\033[0m";
                                            break;
                                        case 0x6C:
                                            cout << "\033[38;5;9m" << AL  << "\033[0m";
                                            break;
                                        case 0x6D:
                                            cout << "\033[38;5;13m" << AL  << "\033[0m";
                                            break;
                                        case 0x6E:
                                            cout << "\033[33m" << AL  << "\033[0m";
                                            break;
                                        case 0x6F:
                                            cout << "\033[37m" << AL  << "\033[0m";
                                            break;
                                    }
                                }
                                if ((AH >= 0x70) && (AH <= 0x7F))
                                {
                                    cout << "\033[48;5;249m";
                                    switch (AH)
                                    {
                                        case 0x70:
                                            cout << "\033[30m" << AL  << "\033[0m";
                                            break;
                                        case 0x71:
                                            cout << "\033[34m" << AL  << "\033[0m";
                                            break;
                                        case 0x72:
                                            cout << "\033[32m" << AL  << "\033[0m"; 
                                            break;
                                        case 0x73:
                                            cout << "\033[36m" << AL  << "\033[0m"; 
                                            break;
                                        case 0x74:
                                            cout << "\033[31m" << AL  << "\033[0m"; 
                                            break;
                                        case 0x75:
                                            cout << "\033[35m" << AL  << "\033[0m"; 
                                            break;
                                        case 0x76:
                                            cout << "\033[38;5;94m" << AL  << "\033[0m"; 
                                            break;
                                        case 0x77:
                                            cout << "\033[38;5;249m" << AL  << "\033[0m";
                                            break; 
                                        case 0x78:
                                            cout << "\033[38;5;240m" << AL  << "\033[0m";
                                            break; 
                                        case 0x79:
                                            cout << "\033[38;5;12m" << AL  << "\033[0m";
                                            break;
                                        case 0x7A:
                                            cout << "\033[38;5;10m" << AL  << "\033[0m";
                                            break;
                                        case 0x7B:
                                            cout << "\033[38;5;14m" << AL  << "\033[0m";
                                            break;
                                        case 0x7C:
                                            cout << "\033[38;5;9m" << AL  << "\033[0m";
                                            break;
                                        case 0x7D:
                                            cout << "\033[38;5;13m" << AL  << "\033[0m";
                                            break;
                                        case 0x7E:
                                            cout << "\033[33m" << AL  << "\033[0m";
                                            break;
                                        case 0x7F:
                                            cout << "\033[37m" << AL  << "\033[0m";
                                            break;
                                    }
                                }
                                if ((AH >= 0x80) && (AH <= 0x8F))
                                {
                                    cout << "\033[48;5;240m";
                                    switch (AH)
                                    {
                                        case 0x20:
                                            cout << "\033[30m" << AL  << "\033[0m";
                                            break;
                                        case 0x21:
                                            cout << "\033[34m" << AL  << "\033[0m";
                                            break;
                                        case 0x22:
                                            cout << "\033[32m" << AL  << "\033[0m"; 
                                            break;
                                        case 0x23:
                                            cout << "\033[36m" << AL  << "\033[0m"; 
                                            break;
                                        case 0x24:
                                            cout << "\033[31m" << AL  << "\033[0m"; 
                                            break;
                                        case 0x25:
                                            cout << "\033[35m" << AL  << "\033[0m"; 
                                            break;
                                        case 0x26:
                                            cout << "\033[38;5;94m" << AL  << "\033[0m"; 
                                            break;
                                        case 0x27:
                                            cout << "\033[38;5;249m" << AL  << "\033[0m";
                                            break; 
                                        case 0x28:
                                            cout << "\033[38;5;240m" << AL  << "\033[0m";
                                            break; 
                                        case 0x29:
                                            cout << "\033[38;5;12m" << AL  << "\033[0m";
                                            break;
                                        case 0x2A:
                                            cout << "\033[38;5;10m" << AL  << "\033[0m";
                                            break;
                                        case 0x2B:
                                            cout << "\033[38;5;14m" << AL  << "\033[0m";
                                            break;
                                        case 0x2C:
                                            cout << "\033[38;5;9m" << AL  << "\033[0m";
                                            break;
                                        case 0x2D:
                                            cout << "\033[38;5;13m" << AL  << "\033[0m";
                                            break;
                                        case 0x2E:
                                            cout << "\033[33m" << AL  << "\033[0m";
                                            break;
                                        case 0x2F:
                                            cout << "\033[37m" << AL  << "\033[0m";
                                            break;
                                    }
                                }
                                if ((AH >= 0x90) && (AH <= 0x9F))
                                {
                                    cout << "\033[48;5;12m";
                                    switch (AH)
                                    {
                                        case 0x90:
                                            cout << "\033[30m" << AL  << "\033[0m";
                                            break;
                                        case 0x91:
                                            cout << "\033[34m" << AL  << "\033[0m";
                                            break;
                                        case 0x92:
                                            cout << "\033[32m" << AL  << "\033[0m"; 
                                            break;
                                        case 0x93:
                                            cout << "\033[36m" << AL  << "\033[0m"; 
                                            break;
                                        case 0x94:
                                            cout << "\033[31m" << AL  << "\033[0m"; 
                                            break;
                                        case 0x95:
                                            cout << "\033[35m" << AL  << "\033[0m"; 
                                            break;
                                        case 0x96:
                                            cout << "\033[38;5;94m" << AL  << "\033[0m"; 
                                            break;
                                        case 0x97:
                                            cout << "\033[38;5;249m" << AL  << "\033[0m";
                                            break; 
                                        case 0x98:
                                            cout << "\033[38;5;240m" << AL  << "\033[0m";
                                            break; 
                                        case 0x99:
                                            cout << "\033[38;5;12m" << AL  << "\033[0m";
                                            break;
                                        case 0x9A:
                                            cout << "\033[38;5;10m" << AL  << "\033[0m";
                                            break;
                                        case 0x9B:
                                            cout << "\033[38;5;14m" << AL  << "\033[0m";
                                            break;
                                        case 0x9C:
                                            cout << "\033[38;5;9m" << AL  << "\033[0m";
                                            break;
                                        case 0x9D:
                                            cout << "\033[38;5;13m" << AL  << "\033[0m";
                                            break;
                                        case 0x9E:
                                            cout << "\033[33m" << AL  << "\033[0m";
                                            break;
                                        case 0x9F:
                                            cout << "\033[37m" << AL  << "\033[0m";
                                            break;
                                    }
                                }
                                if ((AH >= 0xA0) && (AH <= 0xAF))
                                {
                                    cout << "\033[48;5;10m";
                                    switch (AH)
                                    {
                                        case 0xA0:
                                            cout << "\033[30m" << AL  << "\033[0m";
                                            break;
                                        case 0xA1:
                                            cout << "\033[34m" << AL  << "\033[0m";
                                            break;
                                        case 0xA2:
                                            cout << "\033[32m" << AL  << "\033[0m"; 
                                            break;
                                        case 0xA3:
                                            cout << "\033[36m" << AL  << "\033[0m"; 
                                            break;
                                        case 0xA4:
                                            cout << "\033[31m" << AL  << "\033[0m"; 
                                            break;
                                        case 0xA5:
                                            cout << "\033[35m" << AL  << "\033[0m"; 
                                            break;
                                        case 0xA6:
                                            cout << "\033[38;5;94m" << AL  << "\033[0m"; 
                                            break;
                                        case 0xA7:
                                            cout << "\033[38;5;249m" << AL  << "\033[0m";
                                            break; 
                                        case 0xA8:
                                            cout << "\033[38;5;240m" << AL  << "\033[0m";
                                            break; 
                                        case 0xA9:
                                            cout << "\033[38;5;12m" << AL  << "\033[0m";
                                            break;
                                        case 0xAA:
                                            cout << "\033[38;5;10m" << AL  << "\033[0m";
                                            break;
                                        case 0xAB:
                                            cout << "\033[38;5;14m" << AL  << "\033[0m";
                                            break;
                                        case 0xAC:
                                            cout << "\033[38;5;9m" << AL  << "\033[0m";
                                            break;
                                        case 0xAD:
                                            cout << "\033[38;5;13m" << AL  << "\033[0m";
                                            break;
                                        case 0xAE:
                                            cout << "\033[33m" << AL  << "\033[0m";
                                            break;
                                        case 0xAF:
                                            cout << "\033[37m" << AL  << "\033[0m";
                                            break;
                                    }
                                }
                                if ((AH >= 0xB0) && (AH <= 0xBF))
                                {
                                    cout << "\033[48;5;14m";
                                    switch (AH)
                                    {
                                        case 0xB0:
                                            cout << "\033[30m" << AL  << "\033[0m";
                                            break;
                                        case 0xB1:
                                            cout << "\033[34m" << AL  << "\033[0m";
                                            break;
                                        case 0xB2:
                                            cout << "\033[32m" << AL  << "\033[0m"; 
                                            break;
                                        case 0xB3:
                                            cout << "\033[36m" << AL  << "\033[0m"; 
                                            break;
                                        case 0xB4:
                                            cout << "\033[31m" << AL  << "\033[0m"; 
                                            break;
                                        case 0xB5:
                                            cout << "\033[35m" << AL  << "\033[0m"; 
                                            break;
                                        case 0xB6:
                                            cout << "\033[38;5;94m" << AL  << "\033[0m"; 
                                            break;
                                        case 0xB7:
                                            cout << "\033[38;5;249m" << AL  << "\033[0m";
                                            break; 
                                        case 0xB8:
                                            cout << "\033[38;5;240m" << AL  << "\033[0m";
                                            break; 
                                        case 0xB9:
                                            cout << "\033[38;5;12m" << AL  << "\033[0m";
                                            break;
                                        case 0xBA:
                                            cout << "\033[38;5;10m" << AL  << "\033[0m";
                                            break;
                                        case 0xBB:
                                            cout << "\033[38;5;14m" << AL  << "\033[0m";
                                            break;
                                        case 0xBC:
                                            cout << "\033[38;5;9m" << AL  << "\033[0m";
                                            break;
                                        case 0xBD:
                                            cout << "\033[38;5;13m" << AL  << "\033[0m";
                                            break;
                                        case 0xBE:
                                            cout << "\033[33m" << AL  << "\033[0m";
                                            break;
                                        case 0xBF:
                                            cout << "\033[37m" << AL  << "\033[0m";
                                            break;
                                    }
                                }
                                if ((AH >= 0xC0) && (AH <= 0xCF))
                                {
                                    cout << "\033[48;5;9m";
                                    switch (AH)
                                    {
                                        case 0xC0:
                                            cout << "\033[30m" << AL  << "\033[0m";
                                            break;
                                        case 0xC1:
                                            cout << "\033[34m" << AL  << "\033[0m";
                                            break;
                                        case 0xC2:
                                            cout << "\033[32m" << AL  << "\033[0m"; 
                                            break;
                                        case 0xC3:
                                            cout << "\033[36m" << AL  << "\033[0m"; 
                                            break;
                                        case 0xC4:
                                            cout << "\033[31m" << AL  << "\033[0m"; 
                                            break;
                                        case 0xC5:
                                            cout << "\033[35m" << AL  << "\033[0m"; 
                                            break;
                                        case 0xC6:
                                            cout << "\033[38;5;94m" << AL  << "\033[0m"; 
                                            break;
                                        case 0xC7:
                                            cout << "\033[38;5;249m" << AL  << "\033[0m";
                                            break; 
                                        case 0xC8:
                                            cout << "\033[38;5;240m" << AL  << "\033[0m";
                                            break; 
                                        case 0xC9:
                                            cout << "\033[38;5;12m" << AL  << "\033[0m";
                                            break;
                                        case 0xCA:
                                            cout << "\033[38;5;10m" << AL  << "\033[0m";
                                            break;
                                        case 0xCB:
                                            cout << "\033[38;5;14m" << AL  << "\033[0m";
                                            break;
                                        case 0xCC:
                                            cout << "\033[38;5;9m" << AL  << "\033[0m";
                                            break;
                                        case 0xCD:
                                            cout << "\033[38;5;13m" << AL  << "\033[0m";
                                            break;
                                        case 0xCE:
                                            cout << "\033[33m" << AL  << "\033[0m";
                                            break;
                                        case 0xCF:
                                            cout << "\033[37m" << AL  << "\033[0m";
                                            break;
                                    }
                                }
                                if ((AH >= 0xD0) && (AH <= 0xDF))
                                {
                                    cout << "\033[48;5;13m";
                                    switch (AH)
                                    {
                                        case 0xD0:
                                            cout << "\033[30m" << AL  << "\033[0m";
                                            break;
                                        case 0xD1:
                                            cout << "\033[34m" << AL  << "\033[0m";
                                            break;
                                        case 0xD2:
                                            cout << "\033[32m" << AL  << "\033[0m"; 
                                            break;
                                        case 0xD3:
                                            cout << "\033[36m" << AL  << "\033[0m"; 
                                            break;
                                        case 0xD4:
                                            cout << "\033[31m" << AL  << "\033[0m"; 
                                            break;
                                        case 0xD5:
                                            cout << "\033[35m" << AL  << "\033[0m"; 
                                            break;
                                        case 0xD6:
                                            cout << "\033[38;5;94m" << AL  << "\033[0m"; 
                                            break;
                                        case 0xD7:
                                            cout << "\033[38;5;249m" << AL  << "\033[0m";
                                            break; 
                                        case 0xD8:
                                            cout << "\033[38;5;240m" << AL  << "\033[0m";
                                            break; 
                                        case 0xD9:
                                            cout << "\033[38;5;12m" << AL  << "\033[0m";
                                            break;
                                        case 0xDA:
                                            cout << "\033[38;5;10m" << AL  << "\033[0m";
                                            break;
                                        case 0xDB:
                                            cout << "\033[38;5;14m" << AL  << "\033[0m";
                                            break;
                                        case 0xDC:
                                            cout << "\033[38;5;9m" << AL  << "\033[0m";
                                            break;
                                        case 0xDD:
                                            cout << "\033[38;5;13m" << AL  << "\033[0m";
                                            break;
                                        case 0xDE:
                                            cout << "\033[33m" << AL  << "\033[0m";
                                            break;
                                        case 0xDF:
                                            cout << "\033[37m" << AL  << "\033[0m";
                                            break;
                                    }
                                }
                                if ((AH >= 0xE0) && (AH <= 0xEF))
                                {
                                    cout << "\033[43m";
                                    switch (AH)
                                    {
                                        case 0xE0:
                                            cout << "\033[30m" << AL  << "\033[0m";
                                            break;
                                        case 0xE1:
                                            cout << "\033[34m" << AL  << "\033[0m";
                                            break;
                                        case 0xE2:
                                            cout << "\033[32m" << AL  << "\033[0m"; 
                                            break;
                                        case 0xE3:
                                            cout << "\033[36m" << AL  << "\033[0m"; 
                                            break;
                                        case 0xE4:
                                            cout << "\033[31m" << AL  << "\033[0m"; 
                                            break;
                                        case 0xE5:
                                            cout << "\033[35m" << AL  << "\033[0m"; 
                                            break;
                                        case 0xE6:
                                            cout << "\033[38;5;94m" << AL  << "\033[0m"; 
                                            break;
                                        case 0xE7:
                                            cout << "\033[38;5;249m" << AL  << "\033[0m";
                                            break; 
                                        case 0xE8:
                                            cout << "\033[38;5;240m" << AL  << "\033[0m";
                                            break; 
                                        case 0xE9:
                                            cout << "\033[38;5;12m" << AL  << "\033[0m";
                                            break;
                                        case 0xEA:
                                            cout << "\033[38;5;10m" << AL  << "\033[0m";
                                            break;
                                        case 0xEB:
                                            cout << "\033[38;5;14m" << AL  << "\033[0m";
                                            break;
                                        case 0xEC:
                                            cout << "\033[38;5;9m" << AL  << "\033[0m";
                                            break;
                                        case 0xED:
                                            cout << "\033[38;5;13m" << AL  << "\033[0m";
                                            break;
                                        case 0xEE:
                                            cout << "\033[33m" << AL  << "\033[0m";
                                            break;
                                        case 0xEF:
                                            cout << "\033[37m" << AL  << "\033[0m";
                                            break;
                                    }
                                }
                                if ((AH >= 0xF0) && (AH <= 0xFF))
                                {
                                    cout << "\033[47m";
                                    switch (AH)
                                    {
                                        case 0xF0:
                                            cout << "\033[30m" << AL  << "\033[0m";
                                            break;
                                        case 0xF1:
                                            cout << "\033[34m" << AL  << "\033[0m";
                                            break;
                                        case 0xF2:
                                            cout << "\033[32m" << AL  << "\033[0m"; 
                                            break;
                                        case 0xF3:
                                            cout << "\033[36m" << AL  << "\033[0m"; 
                                            break;
                                        case 0xF4:
                                            cout << "\033[31m" << AL  << "\033[0m"; 
                                            break;
                                        case 0xF5:
                                            cout << "\033[35m" << AL  << "\033[0m"; 
                                            break;
                                        case 0xF6:
                                            cout << "\033[38;5;94m" << AL  << "\033[0m"; 
                                            break;
                                        case 0xF7:
                                            cout << "\033[38;5;249m" << AL  << "\033[0m";
                                            break; 
                                        case 0xF8:
                                            cout << "\033[38;5;240m" << AL  << "\033[0m";
                                            break; 
                                        case 0xF9:
                                            cout << "\033[38;5;12m" << AL  << "\033[0m";
                                            break;
                                        case 0xFA:
                                            cout << "\033[38;5;10m" << AL  << "\033[0m";
                                            break;
                                        case 0xFB:
                                            cout << "\033[38;5;14m" << AL  << "\033[0m";
                                            break;
                                        case 0xFC:
                                            cout << "\033[38;5;9m" << AL  << "\033[0m";
                                            break;
                                        case 0xFD:
                                            cout << "\033[38;5;13m" << AL  << "\033[0m";
                                            break;
                                        case 0xFE:
                                            cout << "\033[33m" << AL  << "\033[0m";
                                            break;
                                        case 0xFF:
                                            cout << "\033[37m" << AL  << "\033[0m";
                                            break;
                                    }
                                }
                                data.push_back(AL);
                                if (data.size() == 80)
                                {
                                    data.clear();    
                                    cout << endl;
                                }
                            }
                            else
                            {
                                cout << "This will not fit in the 80x25 screen" << endl;
                            }
                            break;

                        case 0x1D:
                            if (CX <= 2000)
                            {
                                BG = (BH & 0xF0) << 1;
                                FG = BH & 0xF;
                                if (BH <= 0xF)
                                {
                                    cout << "\033[40m";
                                    switch (BH)
                                    {
                                        case 0x0:
                                            cout << "\033[30m" << BL  << "\033[0m";
                                            break;
                                        case 0x1:
                                            cout << "\033[34m" << BL  << "\033[0m";
                                            break;
                                        case 0x2:
                                            cout << "\033[32m" << BL  << "\033[0m"; 
                                            break;
                                        case 0x3:
                                            cout << "\033[36m" << BL  << "\033[0m"; 
                                            break;
                                        case 0x4:
                                            cout << "\033[31m" << BL  << "\033[0m"; 
                                            break;
                                        case 0x5:
                                            cout << "\033[35m" << BL  << "\033[0m"; 
                                            break;
                                        case 0x6:
                                            cout << "\033[38;5;94m" << BL  << "\033[0m"; 
                                            break;
                                        case 0x7:
                                            cout << "\033[38;5;249m" << BL  << "\033[0m";
                                            break; 
                                        case 0x8:
                                            cout << "\033[38;5;240m" << BL  << "\033[0m";
                                            break; 
                                        case 0x9:
                                            cout << "\033[38;5;12m" << BL  << "\033[0m";
                                            break;
                                        case 0xA:
                                            cout << "\033[38;5;10m" << BL  << "\033[0m";
                                            break;
                                        case 0xB:
                                            cout << "\033[38;5;14m" << BL  << "\033[0m";
                                            break;
                                        case 0xC:
                                            cout << "\033[38;5;9m" << BL  << "\033[0m";
                                            break;
                                        case 0xD:
                                            cout << "\033[38;5;13m" << BL  << "\033[0m";
                                            break;
                                        case 0xE:
                                            cout << "\033[33m" << BL  << "\033[0m";
                                            break;
                                        case 0xF:
                                            cout << "\033[37m" << BL  << "\033[0m";
                                            break;
                                    }

                                }
                                if ((BH >= 0x10) && (BH <= 0x1F))
                                {
                                    cout << "\033[44m";
                                    switch (BH)
                                    {
                                        case 0x10:
                                            cout << "\033[30m" << BL  << "\033[0m";
                                            break;
                                        case 0x11:
                                            cout << "\033[34m" << BL  << "\033[0m";
                                            break;
                                        case 0x12:
                                            cout << "\033[32m" << BL  << "\033[0m"; 
                                            break;
                                        case 0x13:
                                            cout << "\033[36m" << BL  << "\033[0m"; 
                                            break;
                                        case 0x14:
                                            cout << "\033[31m" << BL  << "\033[0m"; 
                                            break;
                                        case 0x15:
                                            cout << "\033[35m" << BL  << "\033[0m"; 
                                            break;
                                        case 0x16:
                                            cout << "\033[38;5;94m" << BL  << "\033[0m"; 
                                            break;
                                        case 0x17:
                                            cout << "\033[38;5;249m" << BL  << "\033[0m";
                                            break; 
                                        case 0x18:
                                            cout << "\033[38;5;240m" << BL  << "\033[0m";
                                            break; 
                                        case 0x19:
                                            cout << "\033[38;5;12m" << BL  << "\033[0m";
                                            break;
                                        case 0x1A:
                                            cout << "\033[38;5;10m" << BL  << "\033[0m";
                                            break;
                                        case 0x1B:
                                            cout << "\033[38;5;14m" << BL  << "\033[0m";
                                            break;
                                        case 0x1C:
                                            cout << "\033[38;5;9m" << BL  << "\033[0m";
                                            break;
                                        case 0x1D:
                                            cout << "\033[38;5;13m" << BL  << "\033[0m";
                                            break;
                                        case 0x1E:
                                            cout << "\033[33m" << BL  << "\033[0m";
                                            break;
                                        case 0x1F:
                                            cout << "\033[37m" << BL  << "\033[0m";
                                            break;
                                    }
                                }
                                if ((BH >= 0x20) && (BH <= 0x2F))
                                {
                                    cout << "\033[42m";
                                    switch (BH)
                                    {
                                        case 0x20:
                                            cout << "\033[30m" << BL  << "\033[0m";
                                            break;
                                        case 0x21:
                                            cout << "\033[34m" << BL  << "\033[0m";
                                            break;
                                        case 0x22:
                                            cout << "\033[32m" << BL  << "\033[0m"; 
                                            break;
                                        case 0x23:
                                            cout << "\033[36m" << BL  << "\033[0m"; 
                                            break;
                                        case 0x24:
                                            cout << "\033[31m" << BL  << "\033[0m"; 
                                            break;
                                        case 0x25:
                                            cout << "\033[35m" << BL  << "\033[0m"; 
                                            break;
                                        case 0x26:
                                            cout << "\033[38;5;94m" << BL  << "\033[0m"; 
                                            break;
                                        case 0x27:
                                            cout << "\033[38;5;249m" << BL  << "\033[0m";
                                            break; 
                                        case 0x28:
                                            cout << "\033[38;5;240m" << BL  << "\033[0m";
                                            break; 
                                        case 0x29:
                                            cout << "\033[38;5;12m" << BL  << "\033[0m";
                                            break;
                                        case 0x2A:
                                            cout << "\033[38;5;10m" << BL  << "\033[0m";
                                            break;
                                        case 0x2B:
                                            cout << "\033[38;5;14m" << BL  << "\033[0m";
                                            break;
                                        case 0x2C:
                                            cout << "\033[38;5;9m" << BL  << "\033[0m";
                                            break;
                                        case 0x2D:
                                            cout << "\033[38;5;13m" << BL  << "\033[0m";
                                            break;
                                        case 0x2E:
                                            cout << "\033[33m" << BL  << "\033[0m";
                                            break;
                                        case 0x2F:
                                            cout << "\033[37m" << BL  << "\033[0m";
                                            break;
                                    }
                                }
                                if ((BH >= 0x30) && (BH <= 0x3F))
                                {
                                    cout << "\033[46m";
                                    switch (BH)
                                    {
                                        case 0x30:
                                            cout << "\033[30m" << BL  << "\033[0m";
                                            break;
                                        case 0x31:
                                            cout << "\033[34m" << BL  << "\033[0m";
                                            break;
                                        case 0x32:
                                            cout << "\033[32m" << BL  << "\033[0m"; 
                                            break;
                                        case 0x33:
                                            cout << "\033[36m" << BL  << "\033[0m"; 
                                            break;
                                        case 0x34:
                                            cout << "\033[31m" << BL  << "\033[0m"; 
                                            break;
                                        case 0x35:
                                            cout << "\033[35m" << BL  << "\033[0m"; 
                                            break;
                                        case 0x36:
                                            cout << "\033[38;5;94m" << BL  << "\033[0m"; 
                                            break;
                                        case 0x37:
                                            cout << "\033[38;5;249m" << BL  << "\033[0m";
                                            break; 
                                        case 0x38:
                                            cout << "\033[38;5;240m" << BL  << "\033[0m";
                                            break; 
                                        case 0x39:
                                            cout << "\033[38;5;12m" << BL  << "\033[0m";
                                            break;
                                        case 0x3A:
                                            cout << "\033[38;5;10m" << BL  << "\033[0m";
                                            break;
                                        case 0x3B:
                                            cout << "\033[38;5;14m" << BL  << "\033[0m";
                                            break;
                                        case 0x3C:
                                            cout << "\033[38;5;9m" << BL  << "\033[0m";
                                            break;
                                        case 0x3D:
                                            cout << "\033[38;5;13m" << BL  << "\033[0m";
                                            break;
                                        case 0x3E:
                                            cout << "\033[33m" << BL  << "\033[0m";
                                            break;
                                        case 0x3F:
                                            cout << "\033[37m" << BL  << "\033[0m";
                                            break;
                                    }
                                }
                                if ((BH >= 0x40) && (BH <= 0x4F))
                                {
                                    cout << "\033[41m";
                                    switch (BH)
                                    {
                                        case 0x40:
                                            cout << "\033[30m" << BL  << "\033[0m";
                                            break;
                                        case 0x41:
                                            cout << "\033[34m" << BL  << "\033[0m";
                                            break;
                                        case 0x42:
                                            cout << "\033[32m" << BL  << "\033[0m"; 
                                            break;
                                        case 0x43:
                                            cout << "\033[36m" << BL  << "\033[0m"; 
                                            break;
                                        case 0x44:
                                            cout << "\033[31m" << BL  << "\033[0m"; 
                                            break;
                                        case 0x45:
                                            cout << "\033[35m" << BL  << "\033[0m"; 
                                            break;
                                        case 0x46:
                                            cout << "\033[38;5;94m" << BL  << "\033[0m"; 
                                            break;
                                        case 0x47:
                                            cout << "\033[38;5;249m" << BL  << "\033[0m";
                                            break; 
                                        case 0x48:
                                            cout << "\033[38;5;240m" << BL  << "\033[0m";
                                            break; 
                                        case 0x49:
                                            cout << "\033[38;5;12m" << BL  << "\033[0m";
                                            break;
                                        case 0x4A:
                                            cout << "\033[38;5;10m" << BL  << "\033[0m";
                                            break;
                                        case 0x4B:
                                            cout << "\033[38;5;14m" << BL  << "\033[0m";
                                            break;
                                        case 0x4C:
                                            cout << "\033[38;5;9m" << BL  << "\033[0m";
                                            break;
                                        case 0x4D:
                                            cout << "\033[38;5;13m" << BL  << "\033[0m";
                                            break;
                                        case 0x4E:
                                            cout << "\033[33m" << BL  << "\033[0m";
                                            break;
                                        case 0x4F:
                                            cout << "\033[37m" << BL  << "\033[0m";
                                            break;
                                    }
                                }
                                if ((BH >= 0x50) && (BH <= 0x5F))
                                {
                                    cout << "\033[45m";
                                    switch (BH)
                                    {
                                        case 0x50:
                                            cout << "\033[30m" << BL  << "\033[0m";
                                            break;
                                        case 0x51:
                                            cout << "\033[34m" << BL  << "\033[0m";
                                            break;
                                        case 0x52:
                                            cout << "\033[32m" << BL  << "\033[0m"; 
                                            break;
                                        case 0x53:
                                            cout << "\033[36m" << BL  << "\033[0m"; 
                                            break;
                                        case 0x54:
                                            cout << "\033[31m" << BL  << "\033[0m"; 
                                            break;
                                        case 0x55:
                                            cout << "\033[35m" << BL  << "\033[0m"; 
                                            break;
                                        case 0x56:
                                            cout << "\033[38;5;94m" << BL  << "\033[0m"; 
                                            break;
                                        case 0x57:
                                            cout << "\033[38;5;249m" << BL  << "\033[0m";
                                            break; 
                                        case 0x58:
                                            cout << "\033[38;5;240m" << BL  << "\033[0m";
                                            break; 
                                        case 0x59:
                                            cout << "\033[38;5;12m" << BL  << "\033[0m";
                                            break;
                                        case 0x5A:
                                            cout << "\033[38;5;10m" << BL  << "\033[0m";
                                            break;
                                        case 0x5B:
                                            cout << "\033[38;5;14m" << BL  << "\033[0m";
                                            break;
                                        case 0x5C:
                                            cout << "\033[38;5;9m" << BL  << "\033[0m";
                                            break;
                                        case 0x5D:
                                            cout << "\033[38;5;13m" << BL  << "\033[0m";
                                            break;
                                        case 0x5E:
                                            cout << "\033[33m" << BL  << "\033[0m";
                                            break;
                                        case 0x5F:
                                            cout << "\033[37m" << BL  << "\033[0m";
                                            break;
                                    }
                                }
                                if ((BH >= 0x60) && (BH <= 0x6F))
                                {
                                    cout << "\033[48;5;94m";
                                    switch (BH)
                                    {
                                        case 0x60:
                                            cout << "\033[30m" << BL  << "\033[0m";
                                            break;
                                        case 0x61:
                                            cout << "\033[34m" << BL  << "\033[0m";
                                            break;
                                        case 0x62:
                                            cout << "\033[32m" << BL  << "\033[0m"; 
                                            break;
                                        case 0x63:
                                            cout << "\033[36m" << BL  << "\033[0m"; 
                                            break;
                                        case 0x64:
                                            cout << "\033[31m" << BL  << "\033[0m"; 
                                            break;
                                        case 0x65:
                                            cout << "\033[35m" << BL  << "\033[0m"; 
                                            break;
                                        case 0x66:
                                            cout << "\033[38;5;94m" << BL  << "\033[0m"; 
                                            break;
                                        case 0x67:
                                            cout << "\033[38;5;249m" << BL  << "\033[0m";
                                            break; 
                                        case 0x68:
                                            cout << "\033[38;5;240m" << BL  << "\033[0m";
                                            break; 
                                        case 0x69:
                                            cout << "\033[38;5;12m" << BL  << "\033[0m";
                                            break;
                                        case 0x6A:
                                            cout << "\033[38;5;10m" << BL  << "\033[0m";
                                            break;
                                        case 0x6B:
                                            cout << "\033[38;5;14m" << BL  << "\033[0m";
                                            break;
                                        case 0x6C:
                                            cout << "\033[38;5;9m" << BL  << "\033[0m";
                                            break;
                                        case 0x6D:
                                            cout << "\033[38;5;13m" << BL  << "\033[0m";
                                            break;
                                        case 0x6E:
                                            cout << "\033[33m" << BL  << "\033[0m";
                                            break;
                                        case 0x6F:
                                            cout << "\033[37m" << BL  << "\033[0m";
                                            break;
                                    }
                                }
                                if ((BH >= 0x70) && (BH <= 0x7F))
                                {
                                    cout << "\033[48;5;249m";
                                    switch (BH)
                                    {
                                        case 0x70:
                                            cout << "\033[30m" << BL  << "\033[0m";
                                            break;
                                        case 0x71:
                                            cout << "\033[34m" << BL  << "\033[0m";
                                            break;
                                        case 0x72:
                                            cout << "\033[32m" << BL  << "\033[0m"; 
                                            break;
                                        case 0x73:
                                            cout << "\033[36m" << BL  << "\033[0m"; 
                                            break;
                                        case 0x74:
                                            cout << "\033[31m" << BL  << "\033[0m"; 
                                            break;
                                        case 0x75:
                                            cout << "\033[35m" << BL  << "\033[0m"; 
                                            break;
                                        case 0x76:
                                            cout << "\033[38;5;94m" << BL  << "\033[0m"; 
                                            break;
                                        case 0x77:
                                            cout << "\033[38;5;249m" << BL  << "\033[0m";
                                            break; 
                                        case 0x78:
                                            cout << "\033[38;5;240m" << BL  << "\033[0m";
                                            break; 
                                        case 0x79:
                                            cout << "\033[38;5;12m" << BL  << "\033[0m";
                                            break;
                                        case 0x7A:
                                            cout << "\033[38;5;10m" << BL  << "\033[0m";
                                            break;
                                        case 0x7B:
                                            cout << "\033[38;5;14m" << BL  << "\033[0m";
                                            break;
                                        case 0x7C:
                                            cout << "\033[38;5;9m" << BL  << "\033[0m";
                                            break;
                                        case 0x7D:
                                            cout << "\033[38;5;13m" << BL  << "\033[0m";
                                            break;
                                        case 0x7E:
                                            cout << "\033[33m" << BL  << "\033[0m";
                                            break;
                                        case 0x7F:
                                            cout << "\033[37m" << BL  << "\033[0m";
                                            break;
                                    }
                                }
                                if ((BH >= 0x80) && (BH <= 0x8F))
                                {
                                    cout << "\033[48;5;240m";
                                    switch (BH)
                                    {
                                        case 0x20:
                                            cout << "\033[30m" << BL  << "\033[0m";
                                            break;
                                        case 0x21:
                                            cout << "\033[34m" << BL  << "\033[0m";
                                            break;
                                        case 0x22:
                                            cout << "\033[32m" << BL  << "\033[0m"; 
                                            break;
                                        case 0x23:
                                            cout << "\033[36m" << BL  << "\033[0m"; 
                                            break;
                                        case 0x24:
                                            cout << "\033[31m" << BL  << "\033[0m"; 
                                            break;
                                        case 0x25:
                                            cout << "\033[35m" << BL  << "\033[0m"; 
                                            break;
                                        case 0x26:
                                            cout << "\033[38;5;94m" << BL  << "\033[0m"; 
                                            break;
                                        case 0x27:
                                            cout << "\033[38;5;249m" << BL  << "\033[0m";
                                            break; 
                                        case 0x28:
                                            cout << "\033[38;5;240m" << BL  << "\033[0m";
                                            break; 
                                        case 0x29:
                                            cout << "\033[38;5;12m" << BL  << "\033[0m";
                                            break;
                                        case 0x2A:
                                            cout << "\033[38;5;10m" << BL  << "\033[0m";
                                            break;
                                        case 0x2B:
                                            cout << "\033[38;5;14m" << BL  << "\033[0m";
                                            break;
                                        case 0x2C:
                                            cout << "\033[38;5;9m" << BL  << "\033[0m";
                                            break;
                                        case 0x2D:
                                            cout << "\033[38;5;13m" << BL  << "\033[0m";
                                            break;
                                        case 0x2E:
                                            cout << "\033[33m" << BL  << "\033[0m";
                                            break;
                                        case 0x2F:
                                            cout << "\033[37m" << BL  << "\033[0m";
                                            break;
                                    }
                                }
                                if ((BH >= 0x90) && (BH <= 0x9F))
                                {
                                    cout << "\033[48;5;12m";
                                    switch (BH)
                                    {
                                        case 0x90:
                                            cout << "\033[30m" << BL  << "\033[0m";
                                            break;
                                        case 0x91:
                                            cout << "\033[34m" << BL  << "\033[0m";
                                            break;
                                        case 0x92:
                                            cout << "\033[32m" << BL  << "\033[0m"; 
                                            break;
                                        case 0x93:
                                            cout << "\033[36m" << BL  << "\033[0m"; 
                                            break;
                                        case 0x94:
                                            cout << "\033[31m" << BL  << "\033[0m"; 
                                            break;
                                        case 0x95:
                                            cout << "\033[35m" << BL  << "\033[0m"; 
                                            break;
                                        case 0x96:
                                            cout << "\033[38;5;94m" << BL  << "\033[0m"; 
                                            break;
                                        case 0x97:
                                            cout << "\033[38;5;249m" << BL  << "\033[0m";
                                            break; 
                                        case 0x98:
                                            cout << "\033[38;5;240m" << BL  << "\033[0m";
                                            break; 
                                        case 0x99:
                                            cout << "\033[38;5;12m" << BL  << "\033[0m";
                                            break;
                                        case 0x9A:
                                            cout << "\033[38;5;10m" << BL  << "\033[0m";
                                            break;
                                        case 0x9B:
                                            cout << "\033[38;5;14m" << BL  << "\033[0m";
                                            break;
                                        case 0x9C:
                                            cout << "\033[38;5;9m" << BL  << "\033[0m";
                                            break;
                                        case 0x9D:
                                            cout << "\033[38;5;13m" << BL  << "\033[0m";
                                            break;
                                        case 0x9E:
                                            cout << "\033[33m" << BL  << "\033[0m";
                                            break;
                                        case 0x9F:
                                            cout << "\033[37m" << BL  << "\033[0m";
                                            break;
                                    }
                                }
                                if ((BH >= 0xA0) && (BH <= 0xAF))
                                {
                                    cout << "\033[48;5;10m";
                                    switch (BH)
                                    {
                                        case 0xA0:
                                            cout << "\033[30m" << BL  << "\033[0m";
                                            break;
                                        case 0xA1:
                                            cout << "\033[34m" << BL  << "\033[0m";
                                            break;
                                        case 0xA2:
                                            cout << "\033[32m" << BL  << "\033[0m"; 
                                            break;
                                        case 0xA3:
                                            cout << "\033[36m" << BL  << "\033[0m"; 
                                            break;
                                        case 0xA4:
                                            cout << "\033[31m" << BL  << "\033[0m"; 
                                            break;
                                        case 0xA5:
                                            cout << "\033[35m" << BL  << "\033[0m"; 
                                            break;
                                        case 0xA6:
                                            cout << "\033[38;5;94m" << BL  << "\033[0m"; 
                                            break;
                                        case 0xA7:
                                            cout << "\033[38;5;249m" << BL  << "\033[0m";
                                            break; 
                                        case 0xA8:
                                            cout << "\033[38;5;240m" << BL  << "\033[0m";
                                            break; 
                                        case 0xA9:
                                            cout << "\033[38;5;12m" << BL  << "\033[0m";
                                            break;
                                        case 0xAA:
                                            cout << "\033[38;5;10m" << BL  << "\033[0m";
                                            break;
                                        case 0xAB:
                                            cout << "\033[38;5;14m" << BL  << "\033[0m";
                                            break;
                                        case 0xAC:
                                            cout << "\033[38;5;9m" << BL  << "\033[0m";
                                            break;
                                        case 0xAD:
                                            cout << "\033[38;5;13m" << BL  << "\033[0m";
                                            break;
                                        case 0xAE:
                                            cout << "\033[33m" << BL  << "\033[0m";
                                            break;
                                        case 0xAF:
                                            cout << "\033[37m" << BL  << "\033[0m";
                                            break;
                                    }
                                }
                                if ((BH >= 0xB0) && (BH <= 0xBF))
                                {
                                    cout << "\033[48;5;14m";
                                    switch (BH)
                                    {
                                        case 0xB0:
                                            cout << "\033[30m" << BL  << "\033[0m";
                                            break;
                                        case 0xB1:
                                            cout << "\033[34m" << BL  << "\033[0m";
                                            break;
                                        case 0xB2:
                                            cout << "\033[32m" << BL  << "\033[0m"; 
                                            break;
                                        case 0xB3:
                                            cout << "\033[36m" << BL  << "\033[0m"; 
                                            break;
                                        case 0xB4:
                                            cout << "\033[31m" << BL  << "\033[0m"; 
                                            break;
                                        case 0xB5:
                                            cout << "\033[35m" << BL  << "\033[0m"; 
                                            break;
                                        case 0xB6:
                                            cout << "\033[38;5;94m" << BL  << "\033[0m"; 
                                            break;
                                        case 0xB7:
                                            cout << "\033[38;5;249m" << BL  << "\033[0m";
                                            break; 
                                        case 0xB8:
                                            cout << "\033[38;5;240m" << BL  << "\033[0m";
                                            break; 
                                        case 0xB9:
                                            cout << "\033[38;5;12m" << BL  << "\033[0m";
                                            break;
                                        case 0xBA:
                                            cout << "\033[38;5;10m" << BL  << "\033[0m";
                                            break;
                                        case 0xBB:
                                            cout << "\033[38;5;14m" << BL  << "\033[0m";
                                            break;
                                        case 0xBC:
                                            cout << "\033[38;5;9m" << BL  << "\033[0m";
                                            break;
                                        case 0xBD:
                                            cout << "\033[38;5;13m" << BL  << "\033[0m";
                                            break;
                                        case 0xBE:
                                            cout << "\033[33m" << BL  << "\033[0m";
                                            break;
                                        case 0xBF:
                                            cout << "\033[37m" << BL  << "\033[0m";
                                            break;
                                    }
                                }
                                if ((BH >= 0xC0) && (BH <= 0xCF))
                                {
                                    cout << "\033[48;5;9m";
                                    switch (BH)
                                    {
                                        case 0xC0:
                                            cout << "\033[30m" << BL  << "\033[0m";
                                            break;
                                        case 0xC1:
                                            cout << "\033[34m" << BL  << "\033[0m";
                                            break;
                                        case 0xC2:
                                            cout << "\033[32m" << BL  << "\033[0m"; 
                                            break;
                                        case 0xC3:
                                            cout << "\033[36m" << BL  << "\033[0m"; 
                                            break;
                                        case 0xC4:
                                            cout << "\033[31m" << BL  << "\033[0m"; 
                                            break;
                                        case 0xC5:
                                            cout << "\033[35m" << BL  << "\033[0m"; 
                                            break;
                                        case 0xC6:
                                            cout << "\033[38;5;94m" << BL  << "\033[0m"; 
                                            break;
                                        case 0xC7:
                                            cout << "\033[38;5;249m" << BL  << "\033[0m";
                                            break; 
                                        case 0xC8:
                                            cout << "\033[38;5;240m" << BL  << "\033[0m";
                                            break; 
                                        case 0xC9:
                                            cout << "\033[38;5;12m" << BL  << "\033[0m";
                                            break;
                                        case 0xCA:
                                            cout << "\033[38;5;10m" << BL  << "\033[0m";
                                            break;
                                        case 0xCB:
                                            cout << "\033[38;5;14m" << BL  << "\033[0m";
                                            break;
                                        case 0xCC:
                                            cout << "\033[38;5;9m" << BL  << "\033[0m";
                                            break;
                                        case 0xCD:
                                            cout << "\033[38;5;13m" << BL  << "\033[0m";
                                            break;
                                        case 0xCE:
                                            cout << "\033[33m" << BL  << "\033[0m";
                                            break;
                                        case 0xCF:
                                            cout << "\033[37m" << BL  << "\033[0m";
                                            break;
                                    }
                                }
                                if ((BH >= 0xD0) && (BH <= 0xDF))
                                {
                                    cout << "\033[48;5;13m";
                                    switch (BH)
                                    {
                                        case 0xD0:
                                            cout << "\033[30m" << BL  << "\033[0m";
                                            break;
                                        case 0xD1:
                                            cout << "\033[34m" << BL  << "\033[0m";
                                            break;
                                        case 0xD2:
                                            cout << "\033[32m" << BL  << "\033[0m"; 
                                            break;
                                        case 0xD3:
                                            cout << "\033[36m" << BL  << "\033[0m"; 
                                            break;
                                        case 0xD4:
                                            cout << "\033[31m" << BL  << "\033[0m"; 
                                            break;
                                        case 0xD5:
                                            cout << "\033[35m" << BL  << "\033[0m"; 
                                            break;
                                        case 0xD6:
                                            cout << "\033[38;5;94m" << BL  << "\033[0m"; 
                                            break;
                                        case 0xD7:
                                            cout << "\033[38;5;249m" << BL  << "\033[0m";
                                            break; 
                                        case 0xD8:
                                            cout << "\033[38;5;240m" << BL  << "\033[0m";
                                            break; 
                                        case 0xD9:
                                            cout << "\033[38;5;12m" << BL  << "\033[0m";
                                            break;
                                        case 0xDA:
                                            cout << "\033[38;5;10m" << BL  << "\033[0m";
                                            break;
                                        case 0xDB:
                                            cout << "\033[38;5;14m" << BL  << "\033[0m";
                                            break;
                                        case 0xDC:
                                            cout << "\033[38;5;9m" << BL  << "\033[0m";
                                            break;
                                        case 0xDD:
                                            cout << "\033[38;5;13m" << BL  << "\033[0m";
                                            break;
                                        case 0xDE:
                                            cout << "\033[33m" << BL  << "\033[0m";
                                            break;
                                        case 0xDF:
                                            cout << "\033[37m" << BL  << "\033[0m";
                                            break;
                                    }
                                }
                                if ((BH >= 0xE0) && (BH <= 0xEF))
                                {
                                    cout << "\033[43m";
                                    switch (BH)
                                    {
                                        case 0xE0:
                                            cout << "\033[30m" << BL  << "\033[0m";
                                            break;
                                        case 0xE1:
                                            cout << "\033[34m" << BL  << "\033[0m";
                                            break;
                                        case 0xE2:
                                            cout << "\033[32m" << BL  << "\033[0m"; 
                                            break;
                                        case 0xE3:
                                            cout << "\033[36m" << BL  << "\033[0m"; 
                                            break;
                                        case 0xE4:
                                            cout << "\033[31m" << BL  << "\033[0m"; 
                                            break;
                                        case 0xE5:
                                            cout << "\033[35m" << BL  << "\033[0m"; 
                                            break;
                                        case 0xE6:
                                            cout << "\033[38;5;94m" << BL  << "\033[0m"; 
                                            break;
                                        case 0xE7:
                                            cout << "\033[38;5;249m" << BL  << "\033[0m";
                                            break; 
                                        case 0xE8:
                                            cout << "\033[38;5;240m" << BL  << "\033[0m";
                                            break; 
                                        case 0xE9:
                                            cout << "\033[38;5;12m" << BL  << "\033[0m";
                                            break;
                                        case 0xEA:
                                            cout << "\033[38;5;10m" << BL  << "\033[0m";
                                            break;
                                        case 0xEB:
                                            cout << "\033[38;5;14m" << BL  << "\033[0m";
                                            break;
                                        case 0xEC:
                                            cout << "\033[38;5;9m" << BL  << "\033[0m";
                                            break;
                                        case 0xED:
                                            cout << "\033[38;5;13m" << BL  << "\033[0m";
                                            break;
                                        case 0xEE:
                                            cout << "\033[33m" << BL  << "\033[0m";
                                            break;
                                        case 0xEF:
                                            cout << "\033[37m" << BL  << "\033[0m";
                                            break;
                                    }
                                }
                                if ((BH >= 0xF0) && (BH <= 0xFF))
                                {
                                    cout << "\033[47m";
                                    switch (BH)
                                    {
                                        case 0xF0:
                                            cout << "\033[30m" << BL  << "\033[0m";
                                            break;
                                        case 0xF1:
                                            cout << "\033[34m" << BL  << "\033[0m";
                                            break;
                                        case 0xF2:
                                            cout << "\033[32m" << BL  << "\033[0m"; 
                                            break;
                                        case 0xF3:
                                            cout << "\033[36m" << BL  << "\033[0m"; 
                                            break;
                                        case 0xF4:
                                            cout << "\033[31m" << BL  << "\033[0m"; 
                                            break;
                                        case 0xF5:
                                            cout << "\033[35m" << BL  << "\033[0m"; 
                                            break;
                                        case 0xF6:
                                            cout << "\033[38;5;94m" << BL  << "\033[0m"; 
                                            break;
                                        case 0xF7:
                                            cout << "\033[38;5;249m" << BL  << "\033[0m";
                                            break; 
                                        case 0xF8:
                                            cout << "\033[38;5;240m" << BL  << "\033[0m";
                                            break; 
                                        case 0xF9:
                                            cout << "\033[38;5;12m" << BL  << "\033[0m";
                                            break;
                                        case 0xFA:
                                            cout << "\033[38;5;10m" << BL  << "\033[0m";
                                            break;
                                        case 0xFB:
                                            cout << "\033[38;5;14m" << BL  << "\033[0m";
                                            break;
                                        case 0xFC:
                                            cout << "\033[38;5;9m" << BL  << "\033[0m";
                                            break;
                                        case 0xFD:
                                            cout << "\033[38;5;13m" << BL  << "\033[0m";
                                            break;
                                        case 0xFE:
                                            cout << "\033[33m" << BL  << "\033[0m";
                                            break;
                                        case 0xFF:
                                            cout << "\033[37m" << BL  << "\033[0m";
                                            break;
                                    }
                                }
                                data.push_back(BL);
                                if (data.size() == 80)
                                {
                                    data.clear();    
                                    cout << endl;
                                }
                            }
                            else
                            {
                                cout << "This will not fit in the 80x25 screen" << endl;
                            }
                            break;
                    }  
                    break;
                
            }
            break;

        //LOOP
        case 0xE2:
            if (CX > 1) 
			{
            	signed char loopcount = mem[IP++]; 
                CX--;
                UPDATE8 (AX,CX,DX,BX,AL,AH,CL,CH,DL,DH,BL,BH);
                IP += loopcount;
            }
			else 
            {
                CX--;
                UPDATE8 (AX,CX,DX,BX,AL,AH,CL,CH,DL,DH,BL,BH);
                IP++;
            }
			break;
    }
}

int main() 
{
    FILE* myfile;
    string file;
    int choice;
    cout << "****** Welcome to the x86 Emulator ******" << endl;
    readfile:
    cout << "   Please Enter a Number to Select One of the Options Below: " << endl;
    cout << "   [1] Load a .COM file and perform instructions" << endl;
    cout << "   [2] Exit" << endl;
    cout << "   Input: ";
    cin >> choice;
    if (choice == 1)
    {   
        cout << "   Enter your file name: ";
        cin >> file;
        const char *filename = file.c_str();
        myfile = fopen(filename, "rb");
        if (!myfile)
        {   
            cout << "Unable to open file. Make sure the .com file is in the same directory as the program" << endl; 
            return 0;
        }
        else
        {
            fread(mem, 1, 0x100000, myfile);
            cout << "File read successfully!" << endl;
            cout << endl;
            fclose(myfile);
        }
    }
    if (choice == 2)
    {
        return 0;
    }

    IP = 0x00;
	FLAGS = 0x00000000;
    while (1) 
    {
		op = mem[IP++];
		switch (op) 
        {
            //interrupt
            case 0xCD:
                op = mem[IP++];
                cout << endl;
                printf("\nInterrupt Call: %02X\n", op);
                cout << endl;
                if (op == 0x20) //sucessfully ends programs
                {
                    return 0;
                }
                break;

            //nop
            case 0x90:
                break;

            default:
                instructions(op); //call to function containing all instructions
                break;
		}
		//printReg(); //prints registers after every instruction
	}
	return 0;
}