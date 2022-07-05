#include <stdio.h>
#include <stdint.h>
#include <math.h>

#include "../../includes/pcsx2/pcsx2f_api.h"

//#define NANOPRINTF_IMPLEMENTATION
#include "../../includes/pcsx2/nanoprintf.h"
#include "../../includes/pcsx2/log.h"
#include "../../includes/pcsx2/memalloc.h"
#include "../../includes/pcsx2/patterns.h"
#include "../../includes/pcsx2/mips.h"
#include "../../includes/pcsx2/injector.h"
#include "../../includes/pcsx2/inireader.h"

int CompatibleCRCList[] = { 0x4F32A11F };
char OSDText[OSDStringNum][OSDStringSize] = { {1} };

enum eOpcodeResult
{
    OR_CONTINUE = 0,
    OR_TERMINATE = 1,
    OR_UNDEFINED = -1
};

struct CRunningScript {
    struct CRunningScript* prev;
    struct CRunningScript* next;
    int32_t id;
    union
    {
        uint8_t field_0xc;
        uint8_t field_0xd;
        uint8_t field_0xe;
        uint8_t field_0xf;
        uint32_t m_baseIp;
    };
    uint32_t ip;
    int32_t stack[16];
    int32_t locals[106];
    int32_t locals_pointer;
    uint32_t m_nWakeTime;
    int16_t sp;
    int16_t m_nAndOrState;
    uint8_t m_bIsActive;
    uint8_t m_bCondResult;
    uint8_t m_bIsMissionScript;
    uint8_t m_bSkipWakeTime;
    uint8_t m_bNotFlag;
    uint8_t m_bDeatharrestEnabled;
    uint8_t m_bDeatharrestExecuted;
    char m_abScriptName[8];
    uint8_t m_bMissionFlag;
};

typedef struct
{
    uint32_t script_length;
    uint8_t* script_raw_bytes[];
} raw_script;

uint8_t CLEOScripts[100000];

#define MainScriptNewSize 500000

char CTheScripts__ScriptSpace[MainScriptNewSize] = { {1} };

uintptr_t ScriptSpace()
{
    return &CTheScripts__ScriptSpace;
}

int CTheScripts__Init(int a1)
{
    int(*pCTheScripts__Init)() = (void*)0x255308;
    CRunningScript*(*CTheScripts__StartNewScript)(int a1) = (void*)0x255950;
    int ret = pCTheScripts__Init();

    uintptr_t global_offset = MainScriptNewSize - sizeof(CLEOScripts);
    uintptr_t script_offset = (uintptr_t)(&CLEOScripts);
    uintptr_t script_size = *(uint32_t*)script_offset;
    while (script_size)
    {
        if (script_offset + sizeof(uint32_t) + script_size > ((uintptr_t)(&CLEOScripts) + sizeof(CLEOScripts)))
            break;

        script_offset += sizeof(uint32_t);

        logger.WriteF("Script size: %d", script_size);
        memcpy(&CTheScripts__ScriptSpace[global_offset], (void*)(script_offset), script_size);

        CRunningScript* started = CTheScripts__StartNewScript(global_offset);
        started->m_baseIp = global_offset;

        global_offset += script_size;
        script_offset += script_size;
        script_size = *(uint32_t*)script_offset;
    }
    
    return ret;
}
int* pCTheScripts__MainScriptSize = (int*)0x4CD3D8;
int* pScriptParams = (int*)0x50DEF8;
void (*CScriptThread__getNumberParams)(CRunningScript* a1, uint32_t* a2, int a3, int* a4) = 0x254B90;
void (*CScriptThread__setNumberParams)(CRunningScript* a1, uint32_t* a2, int a3) = 0x254F70;

int opcode_0002(CRunningScript* a1)
{
    CScriptThread__getNumberParams(a1, &a1->ip, 1, pScriptParams);
    int32_t address = pScriptParams[0];
    if (address >= 0)
        a1->ip = address;
    else
    {
        if (a1->m_baseIp)
            a1->ip = (a1->m_baseIp) - address;
        else
        {
            a1->ip = *pCTheScripts__MainScriptSize - address;
        }
    }
    return OR_CONTINUE;
}

int opcode_0021(CRunningScript* a1)
{
    CScriptThread__getNumberParams(a1, &a1->ip, 1, pScriptParams);
    int32_t address = pScriptParams[0];
    if (a1->m_bCondResult)
    {
        if (address >= 0)
            a1->ip = address;
        else
        {
            if (a1->m_baseIp)
                a1->ip = (a1->m_baseIp) - address;
            else
            {
                a1->ip = *pCTheScripts__MainScriptSize - address;
            }
        }
    }
    return OR_CONTINUE;
}

int opcode_0022(CRunningScript* a1)
{
    CScriptThread__getNumberParams(a1, &a1->ip, 1, pScriptParams);
    int32_t address = pScriptParams[0];
    if (!a1->m_bCondResult)
    {
        if (address >= 0)
            a1->ip = address;
        else
        {
            if (a1->m_baseIp)
                a1->ip = (a1->m_baseIp) - address;
            else
            {
                a1->ip = *pCTheScripts__MainScriptSize - address;
            }
        }
    }
    return OR_CONTINUE;
}

int opcode_0025(CRunningScript* a1)
{
    CScriptThread__getNumberParams(a1, &a1->ip, 1, pScriptParams);
    int32_t address = pScriptParams[0];
    uint32_t ip = a1->ip;
    int sp = a1->sp;
    a1->sp = sp + 1;
    a1->stack[sp] = ip;
    if (address >= 0)
        a1->ip = address;
    else
    {
        if (a1->m_baseIp)
            a1->ip = (a1->m_baseIp) - address;
        else
        {
            a1->ip = *pCTheScripts__MainScriptSize - address;
        }
    }
    return OR_CONTINUE;
}

int sub_3B2538(CRunningScript* a1, int a2)
{
    int sp; // $v1
    int* stack; // $a2
    uint32_t ip; // $a0
    int v6; // $a1
    int* v7; // $s5
    uint32_t v8; // $v0
    uint32_t v9; // $s4
    int v10; // $s1
    int v11; // $s2
    uint32_t v12; // $a0
    int v13; // $v1

    sp = a1->sp;
    stack = a1->stack;
    ip = a1->ip;
    if (a2 == 891)
        v6 = 0x800000;
    else
        v6 = 0xC00000;
    v7 = stack;
    a1->sp = sp + 1;
    stack[sp] = ip | v6;
    v8 = a1->ip;
    v9 = v8 + 3;
    v10 = *(char*)(ScriptSpace() + v8);
    a1->ip = v8 + 2;
    v11 = *(char*)(ScriptSpace() + v8 + 2);
    a1->ip = v8 + 3;
    CScriptThread__getNumberParams(a1, &a1->ip, 1, pScriptParams);
    int32_t address = pScriptParams[0];
    if (v10)
        CScriptThread__getNumberParams(a1, &a1->ip, v10, &a1->locals[a1->locals_pointer + v11]);
    v12 = a1->ip;
    v13 = a1->sp - 1;
    a1->locals_pointer += v11;
    v7[v13] |= (v12 - v9) << 24;
    if (address >= 0)
        a1->ip = address;
    else
    {
        if (a1->m_baseIp)
            a1->ip = (a1->m_baseIp) - address;
        else
        {
            a1->ip = *pCTheScripts__MainScriptSize - address;
        }
    }
    return OR_CONTINUE;
}

int opcode_0DD5(CRunningScript* a1)
{
    pScriptParams[0] = 2;
    CScriptThread__setNumberParams(a1, &a1->ip, 1);

    return OR_CONTINUE;
}

int CScriptThread__parseOpcode(CRunningScript* a1)
{
    //uint16_t id = *(uint8_t*)(ScriptSpace() + a1->ip) | (uint16_t)(*(uint8_t*)(ScriptSpace() + a1->ip + 1) << 8);
    uint16_t id = *(uint16_t*)(ScriptSpace() + a1->ip);
    a1->ip += 2;

    if ((id & 0x8000))
        a1->m_bNotFlag = 1;
    else
        a1->m_bNotFlag = 0;

    switch (id)
    {
    case 0x0002:
        return opcode_0002(a1);
    case 0x0021:
        return opcode_0021(a1);
    case 0x0022:
        return opcode_0022(a1);
    case 0x0025:
        return opcode_0025(a1);
    case 0x0DD5:
        return opcode_0DD5(a1);
    default:
        a1->ip -= 2;
        return ((int(*)(CRunningScript*))0x2560D8)(a1);
    }
}

extern "C" void init()
{
    logger.SetBuffer(OSDText, sizeof(OSDText) / sizeof(OSDText[0]), sizeof(OSDText[0]));
    logger.Write("Loading GTAVCS.PCSX2F.CLEO...");

    injector.MakeJAL(0x2554F8, (intptr_t)ScriptSpace);
    injector.MakeJAL(0x3B9EF0, (intptr_t)CTheScripts__Init); //50FB320
    injector.MakeJAL(0x3BA274, (intptr_t)CTheScripts__Init);
    injector.MakeNOP(0x2552D8); //CTheScripts__Shutdown
    
    //injector.MakeJMPwNOP(0x3B1178, (intptr_t)opcode_0002);
    //injector.MakeJMPwNOP(0x3B1C98, (intptr_t)opcode_0021);
    //injector.MakeJMPwNOP(0x3B1D00, (intptr_t)opcode_0022);
    //injector.MakeJMPwNOP(0x3B1E30, (intptr_t)opcode_0025);
    injector.MakeJMPwNOP(0x3B2538, (intptr_t)sub_3B2538);
    
    //injector.MakeJMPwNOP(0x2560D8, (intptr_t)CScriptThread__parseOpcode);
    injector.MakeJAL(0x256030, (intptr_t)CScriptThread__parseOpcode);

    logger.Write("GTAVCS.PCSX2F.CLEO loaded");
    //logger.ClearLog();
}

int main()
{
    return 0;
}
