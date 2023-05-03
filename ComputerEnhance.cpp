struct RegisterCode
{
    u8 code;
    u8 w;
    u8 isSegmentRegister;
};

inline bool isAccumulator(RegisterCode& reg)
{
    return reg.code == 0;
}

enum CPUStyle
{
    CPUStyle_8086,
    CPUStyle_8088,
};

internal RegisterCode getRegisterCode(String registerName)
{
    RegisterCode code= {};

    if(stringsAreEqual(registerName, fromC("AL")) || stringsAreEqual(registerName, fromC("al")))
    {
        code.code = 0b000;
        code.w    = 0;
    }
    else if(stringsAreEqual(registerName, fromC("CL")) || stringsAreEqual(registerName, fromC("cl")))
    {
        code.code = 0b001;
        code.w    = 0;
    }
    else if(stringsAreEqual(registerName, fromC("DL")) || stringsAreEqual(registerName, fromC("dl")))
    {
        code.code = 0b010;
        code.w    = 0;
    }
    else if(stringsAreEqual(registerName, fromC("BL")) || stringsAreEqual(registerName, fromC("bl")))
    {
        code.code = 0b011;
        code.w    = 0;
    }
    else if(stringsAreEqual(registerName, fromC("AH")) || stringsAreEqual(registerName, fromC("ah")))
    {
        code.code = 0b100;
        code.w    = 0;
    }
    else if(stringsAreEqual(registerName, fromC("CH")) || stringsAreEqual(registerName, fromC("ch")))
    {
        code.code = 0b101;
        code.w    = 0;
    }
    else if(stringsAreEqual(registerName, fromC("DH")) || stringsAreEqual(registerName, fromC("dh")))
    {
        code.code = 0b110;
        code.w    = 0;
    }
    else if(stringsAreEqual(registerName, fromC("BH")) || stringsAreEqual(registerName, fromC("bh")))
    {
        code.code = 0b111;
        code.w    = 0;
    }
    else if(stringsAreEqual(registerName, fromC("AX")) || stringsAreEqual(registerName, fromC("ax")))
    {
        code.code = 0b000;
        code.w    = 1;
    }
    else if(stringsAreEqual(registerName, fromC("CX")) || stringsAreEqual(registerName, fromC("cx")))
    {
        code.code = 0b001;
        code.w    = 1;
    }
    else if(stringsAreEqual(registerName, fromC("DX")) || stringsAreEqual(registerName, fromC("dx")))
    {
        code.code = 0b010;
        code.w    = 1;
    }
    else if(stringsAreEqual(registerName, fromC("BX")) || stringsAreEqual(registerName, fromC("bx")))
    {
        code.code = 0b011;
        code.w    = 1;
    }
    else if(stringsAreEqual(registerName, fromC("SP")) || stringsAreEqual(registerName, fromC("sp")))
    {
        code.code = 0b100;
        code.w    = 1;
    }
    else if(stringsAreEqual(registerName, fromC("BP")) || stringsAreEqual(registerName, fromC("bp")))
    {
        code.code = 0b101;
        code.w    = 1;
    }
    else if(stringsAreEqual(registerName, fromC("SI")) || stringsAreEqual(registerName, fromC("si")))
    {
        code.code = 0b110;
        code.w    = 1;
    }
    else if(stringsAreEqual(registerName, fromC("DI")) || stringsAreEqual(registerName, fromC("di")))
    {
        code.code = 0b111;
        code.w    = 1;
    }
    else if(stringsAreEqual(registerName, fromC("ES")) || stringsAreEqual(registerName, fromC("es")))
    {
        code.code = 0b00;
        code.w    = 0;
        code.isSegmentRegister = 1;
    }
    else if(stringsAreEqual(registerName, fromC("CS")) || stringsAreEqual(registerName, fromC("cs")))
    {
        code.code = 0b01;
        code.w    = 0;
        code.isSegmentRegister = 1;
    }
    else if(stringsAreEqual(registerName, fromC("SS")) || stringsAreEqual(registerName, fromC("ss")))
    {
        code.code = 0b10;
        code.w    = 0;
        code.isSegmentRegister = 1;
    }
    else if(stringsAreEqual(registerName, fromC("DS")) || stringsAreEqual(registerName, fromC("ds")))
    {
        code.code = 0b11;
        code.w    = 0;
        code.isSegmentRegister = 1;
    }
    return code;
}

globalVariable char* regsW0[] =
{
    "AL", "CL", "DL", "BL", "AH", "CH","DH", "BH",
};

globalVariable char* regsW1[] =
{
    "AX", "CX", "DX", "BX", "SP", "BP","SI", "DI",
};

#define CF_FLAG 0b0000000000000001
#define PF_FLAG 0b0000000000000100
#define AF_FLAG 0b0000000000010000
#define ZF_FLAG 0b0000000001000000
#define SF_FLAG 0b0000000010000000
#define TF_FLAG 0b0000000100000000
#define IF_FLAG 0b0000001000000000
#define DF_FLAG 0b0000010000000000
#define OF_FLAG 0b0000100000000000

#define HIGHEST_16_BIT 0b1000000000000000
#define HIGHEST_8_BIT 0b10000000

enum ASMType
{
    ASMType_None,
    ASMType_Register,
    ASMType_Immediate,
    ASMType_EffectiveAddress,
};  

struct EffectiveAddress
{
    u8  rm;
    u8  mod;
    u16 value;
    bool neg;
};

inline bool isDirectMemory(EffectiveAddress& address)
{
    return address.mod == 0 && address.rm == 0b110;
}

enum ImmediateSize
{
    ImmediateSize_Unknown,
    ImmediateSize_Byte,
    ImmediateSize_Word,
};

struct Immediate
{
    u16  value;
    bool neg;
};

struct ASMAtomic
{
    ASMType       type;
    ImmediateSize size;
    union 
    {
        RegisterCode     reg;
        Immediate        immediate;
        EffectiveAddress effective;
    };
      
};

struct Registers
{
    u16 registers[8];
    u16 segmentRegisters[4];
    u16 ip;
    u16 flags;
};

struct RegisterIndex
{
    u8 index;
    u8 part; // @Note 0 all, 1 low, 2 high
};  

inline RegisterIndex getRegisterIndex(u8 reg, u8 w)
{
    RegisterIndex result = {};
    if(w)
    {
        switch(reg)
        {
            case 3 : result.index = 1; break;
            case 1 : result.index = 2; break;
            case 2 : result.index = 3; break;
            default : result.index = reg; break;
        }
    }
    else
    {
        switch(reg)
        {
            case 0 : result.index = 0; result.part = 1; break;
            case 3 : result.index = 1; result.part = 1; break;
            case 1 : result.index = 2; result.part = 1; break;
            case 2 : result.index = 3; result.part = 1; break;
            case 4 : result.index = 0; result.part = 2; break;
            case 5 : result.index = 2; result.part = 2; break;
            case 6 : result.index = 3; result.part = 2; break;
            case 7 : result.index = 1; result.part = 2; break;
        }
    }

    return result;
}

inline void getRegisterName(u8 reg, u8 w, OutputBuffer& out)
{
    formatStringAdvance(out, w ? regsW1[reg] : regsW0[reg]);
}

inline ASMAtomic getRegisterCode(u8 reg, u8 w)
{
    ASMAtomic code = {};
    code.type      = ASMType_Register;
    code.reg.code  = reg;
    code.reg.w     = w;
    code.size      = w ? ImmediateSize_Word : ImmediateSize_Byte;
    return code;
}

globalVariable char* regsSegment[] =
{
    "ES", "CS", "SS", "DS"
};

inline void getSegmentRegisterName(u8 reg, OutputBuffer& out)
{
    Assert(reg < 4);
    formatStringAdvance(out, regsSegment[reg]);
}

#define registerToRegisterCmpOpcode        0b001110
#define registerToRegisterSubOpcode        0b001010
#define registerToRegisterAddOpcode        0b000000
#define registerToRegisterMovOpcode        0b100010
#define immediateToRegisterMovOpcode       0b1011
#define immediateToMemoryMovOpcode         0b1100011
#define immediateToMemoryAddOpcode         0b100000
#define memoryToAccumulatorMovOpcode       0b1010000
#define accumulatorToMemoryMovOpcode       0b1010001
#define immediateToAccumulatorAddOpCode    0b0000010    
#define immediateToAccumulatorSubOpCode    0b0010110    
#define immediateToAccumulatorCmpOpCode    0b0011110    
#define registerToSegmentRegisterMovOpcode 0b10001110    
#define segmentRegisterToRegisterMovOpcode 0b10001100    


internal ASMAtomic readAtomic(Tokenizer& tokenizer)
{
    ASMAtomic result = {};
    Token tok = getToken(tokenizer);
    start:
    switch(tok.type)
    {
        case Token_Identifier:
        {
            if(stringsAreEqual(tok.text, fromC("byte")))
            {
                tok = getToken(tokenizer);
                Assert(result.size == ImmediateSize_Unknown);
                result.size  = ImmediateSize_Byte;
                goto start;
            }
            else if(stringsAreEqual(tok.text, fromC("word")))
            {
                tok = getToken(tokenizer);
                Assert(result.size == ImmediateSize_Unknown);
                result.size  = ImmediateSize_Word;
                goto start;
            }
            else
            {
                result.reg  = getRegisterCode(tok.text);
                result.type = ASMType_Register;
                result.size = result.reg.w == 1 ? ImmediateSize_Word : ImmediateSize_Byte;
            }
            break;
        }
        case Token_Number:
        {
            result.immediate.neg   = tok.number.value < 0;
            result.immediate.value = (u16)tok.number.value;
            result.type            = ASMType_Immediate;
            if(result.size == ImmediateSize_Unknown)
            {
                result.size = (result.immediate.value <= U8Max) ? ImmediateSize_Byte: ImmediateSize_Word;
            }
            break;
        }
        case Token_OpenBracket:
        {
            Token f = getToken(tokenizer);
            result.type = ASMType_EffectiveAddress;
            switch(f.type)
            {
                case Token_Number: // @Note Direct address
                {
                    Token end = getToken(tokenizer);
                    Assert(end.type == Token_CloseBracket);
                    result.type            = ASMType_EffectiveAddress;
                    result.effective.rm    = 0b110;
                    result.effective.mod   = 0b00;
                    result.effective.value = (u16)f.number.value;
                    break;
                }
                case Token_Identifier:
                {
                    RegisterCode r0  = getRegisterCode(f.text);
                    RegisterCode r1  = {};
                    r1.w = 2; // @Note invalid
                    s16 displacement = 0;
                    Token m = getToken(tokenizer);
                    switch(m.type)
                    {
                        case Token_CloseBracket:
                        {
                            break;
                        }
                        case Token_Minus:
                        case Token_Plus:
                        {
                            
                            Token next = getToken(tokenizer);
                            switch(next.type)
                            {
                                case Token_Number:
                                {
                                    result.effective.neg = m.type == Token_Minus;
                                    Assert(next.number.value >= 0); // @Note elese, we need to adjust the neg field
                                    displacement = (s16)next.number.value;
                                    Token endToken = getToken(tokenizer);
                                    Assert(endToken.type == Token_CloseBracket);
                                    break;
                                }
                                case Token_Identifier:
                                {
                                    r1 = getRegisterCode(next.text);
                                    Token again = getToken(tokenizer);
                                    switch(again.type)
                                    {
                                        case Token_CloseBracket:
                                        {
                                            break;
                                        }
                                        case Token_Number:
                                        {
                                            displacement = (s16)next.number.value;
                                            break;
                                        }
                                        case Token_Minus:
                                        case Token_Plus:
                                        {
                                            result.effective.neg = again.type == Token_Minus;
                                            Token finalToken = getToken(tokenizer);
                                            Assert(finalToken.type == Token_Number);
                                            Token endToken = getToken(tokenizer);
                                            Assert(endToken.type == Token_CloseBracket);
                                            displacement = (s16)finalToken.number.value;
                                            break;
                                        }
                                        InvalidDefaultCase;
                                    }
                                    break;
                                }
                                InvalidDefaultCase;
                            }
                            break;
                        }
                        InvalidDefaultCase;
                    }
                    
                    if(displacement == 0)
                    {
                        result.effective.mod   = 0b00;
                    }
                    else if(displacement <= S8Max)
                    {
                        result.effective.mod   = 0b01;
                    }
                    else
                    {
                        result.effective.mod   = 0b10;
                    }
                    result.effective.value = displacement;

                    u8 concatReg = (r0.code << 3) | (r1.code);
                    switch(concatReg)
                    {
                        case 0b011110: result.effective.rm = 0b000; break;
                        case 0b011111: result.effective.rm = 0b001; break;
                        case 0b101110: result.effective.rm = 0b010; break;
                        case 0b101111: result.effective.rm = 0b011; break;
                        case 0b110000: result.effective.rm = 0b100; break;
                        case 0b111000: result.effective.rm = 0b101; break;
                        case 0b101000: 
                        {
                            result.effective.rm = 0b110; 
                            if(r1.w == 2 && displacement == 0) // @Note we parsed BP explicitely, so add a 0 displacement
                            { 
                                result.effective.mod = 0b01; 
                            }; 
                            break;
                        }
                        case 0b011000: result.effective.rm = 0b111; break;
                        InvalidDefaultCase;
                    } 
                    
                    break;
                }
                InvalidDefaultCase;
            }
            break;
        }
        InvalidDefaultCase;
    }

    return result;
}

internal s32 getEffectiveAddress(Registers& registers, u8 rm, u8 mod, s32 val)
{
    s32 value = val;
    switch(rm)
    {
        case 0b000 : value += registers.registers[1] + registers.registers[6]; break;
        case 0b001 : value += registers.registers[1] + registers.registers[7]; break;
        case 0b010 : value += registers.registers[5] + registers.registers[6]; break;
        case 0b011 : value += registers.registers[5] + registers.registers[7]; break;
        case 0b100 : value += registers.registers[6]; break;
        case 0b101 : value += registers.registers[7]; break;
        case 0b110 : 
        {
            switch(mod)
            {
                case 0b00: break; // @Note direct adressing, the answer is val
                default : 
                {
                    value += registers.registers[5];
                    break;
                }
            }
            break;
        }
        case 0b111 : value += registers.registers[1]; break;
    }

    return value;
}


internal ASMAtomic getEffectiveAddress(u8 rm, u8 mod, s32 value)
{
    Assert(value <= U16Max);
    ASMAtomic in = {};
    in.type            = ASMType_EffectiveAddress;
    in.effective.rm    = rm;
    in.effective.neg   = value < 0;
    in.effective.value = (u16)absolute(value);
    in.effective.mod   = mod; 

    return in;
}

internal void outputEffectiveAddress(OutputBuffer& out, u8 rm, s32 value, bool mod0 = false)
{
    Assert(value <= U16Max);
    formatStringAdvance(out, "[ ");
    switch(rm)
    {
        case 0b000 : formatStringAdvance(out, "BX + SI"); break;
        case 0b001 : formatStringAdvance(out, "BX + DI"); break;
        case 0b010 : formatStringAdvance(out, "BP + SI"); break;
        case 0b011 : formatStringAdvance(out, "BP + DI"); break;
        case 0b100 : formatStringAdvance(out, "SI"); break;
        case 0b101 : formatStringAdvance(out, "DI"); break;
        case 0b110 : 
        {
            if(mod0 == false)
            {
                formatStringAdvance(out, "BP"); 
            }
            else
            {
                formatStringAdvance(out, "%d", value);
            }
            break;
        }
        case 0b111 : formatStringAdvance(out, "BX"); break;
    }

    if(mod0 == false && value)
    {
        if(value < 0)
        {
            formatStringAdvance(out, " - %i",absolute(value));
        }
        else
        {
            formatStringAdvance(out, " + %i", value);
        }
    }
            
    formatStringAdvance(out, " ]");
}

enum OpCodeType
{
    OpCodeType_Mov,
    OpCodeType_Add,
    OpCodeType_Sub,
    OpCodeType_Cmp,
    OpCodeType_Jump,
};

struct OutputFormat
{
    u32 inc;
    ASMAtomic dst;
    ASMAtomic src;
};

struct MemoryAccessCost
{
    u32 ea;
    u32 penalty;
};


internal MemoryAccessCost getMemoryAccesEstimation(Registers& regs, EffectiveAddress& adress, ImmediateSize size, u32 transfers, CPUStyle style)
{
    MemoryAccessCost res = {};

    bool disp = false; 

    switch(adress.mod)
    {
        case 0b00:
        {
            if(adress.rm == 6)
            {
                disp = true;
            }
            else
            {
                disp = false;
            }
            
            break;
        }
        case 0b01:
        {
            disp = true;
            break;
        }
        case 0b10:
        {
            disp = true;
            break;
        }
        case 0b11:
        {
            disp = false;
            break;
        }
    }

    // @Robustness seems true when accessing through [bp], as in the encoding, there is a 0 displacement
    if(adress.value == 0) 
    {
        disp = false;
    }

    switch(adress.rm)
    {
        case 0b000 : 
        { 
            res.ea = disp ? 11 : 7;
            break;
        }
        case 0b001 : res.ea = disp ? 12 : 8; break;
        case 0b010 : res.ea = disp ? 12 : 8; break; // "BP + SI"
        case 0b011 : res.ea = disp ? 11 : 7; break; // "BP + DI"
        case 0b100 : res.ea = disp ? 9 : 5; break; // SI
        case 0b101 : res.ea = disp ? 9 : 5; break; // DI
        case 0b110 : 
        {
            res.ea = disp ? (adress.mod == 0b00 ? 6 : 9) : 5; // BP or displacement only
            break;
        }
        case 0b111 :  res.ea = disp ? 9 : 5; break; // BX
    }

    bool applyPenalty = false;
    switch(style)
    {
        case CPUStyle_8086: 
        {
            u32 offset   = getEffectiveAddress(regs, adress.rm, adress.mod, adress.value);
            applyPenalty = offset % 2 != 0 && size != ImmediateSize_Byte;
            break;
        }
        case CPUStyle_8088:
        {
            applyPenalty = size != ImmediateSize_Byte;
            break;
        }
    }
    
    if(applyPenalty)
    {
        res.penalty = 4 * transfers; // @Note unaligned penalty
    }
    
    return res;
}

internal OutputFormat encodeImmediateToMemory(u8* current, bool sign = false)
{
    OutputFormat result = {};

    u8 w = (*current) & 1;
    ++current;
    u8 mod = *current >> 6;
    u8 rm  = *current & 0b111;
    ++current;
    result.inc += 2;

    switch(mod)
    {
        case 0b00:
        {
            if(rm == 6)
            {
                s32 value = *(u16*)current;
                current += 2;
                result.inc += 2;
                result.dst = getEffectiveAddress(rm, mod, value);
            }
            else
            {
                result.dst = getEffectiveAddress(rm, mod, 0);
            }
            
            break;
        }
        case 0b01:
        {
            s32 value = *(u8*)current;
            ++current;
            result.inc += 1;
            result.dst = getEffectiveAddress(rm, mod, value);
            break;
        }
        case 0b10:
        {
            s32 value = *(u16*)current;
            current += 2;
            result.inc += 2;
            result.dst = getEffectiveAddress(rm, mod, value);
            break;
        }
        case 0b11:
        {
            result.dst = getRegisterCode(rm, w);
            break;
        }
    }

    result.src.type = ASMType_Immediate;
    if(w && sign == 0)
    {
        result.src.size = ImmediateSize_Word;
        result.src.immediate.value = sign ? (s32)*((s16*)current) : (s32)*((u16*)current);
        current += 2;
        result.inc += 2;
    }
    else
    {
        result.src.size = w ? ImmediateSize_Word : ImmediateSize_Byte;
        result.src.immediate.value = sign ? (s32)*((s8*)current) : (s32)*((u8*)current);
        current += 1;
        result.inc += 1;
    }
    
    return result;
}

internal void outputASMAtomic(ASMAtomic& dst, OutputBuffer& out)
{
    switch (dst.type)
    {
    case ASMType_Register:
        getRegisterName(dst.reg.code, dst.reg.w, out);
        break;
    
    case ASMType_EffectiveAddress:
        outputEffectiveAddress(out, dst.effective.rm, dst.effective.neg ? -dst.effective.value : dst.effective.value, dst.effective.mod == 0);
        break;
    }
}

internal void immediateToMemory(OutputFormat& f, OutputBuffer& out)
{
    outputASMAtomic(f.dst, out);
    formatStringAdvance(out, ", ");
    
    if(f.src.size == ImmediateSize_Byte)
    {
        formatStringAdvance(out, "byte ");
    }
    else
    {
        formatStringAdvance(out, "word ");
    }
    formatStringAdvance(out, f.src.immediate.neg ? "-%i" : "%i", f.src.immediate.value);
}

internal OutputFormat encodeRegisterToRegister(u8* current)
{
    OutputFormat result = {};

    u8 d   = ((*current) >> 1) & 1;
    u8 w   = (*current) & 1;
    u8 mod = *(current + 1) >> 6;
    u8 reg = (*(current + 1) >> 3) & 0b111;
    u8 rm  = *(current + 1) & 0b111;
    result.inc +=2;
    s32 value = 0;
    switch(mod)
    {
        case 0b00:
        {
            if(rm == 6)
            {
                value = *(u16*)(current + 2);
                result.inc += 2;
                current    += 2;
            }

            if(d)
            {
                result.dst = getRegisterCode(reg, w);
                result.src = getEffectiveAddress(rm, mod, value);
            }
            else
            {
                result.dst = getEffectiveAddress(rm, mod, value);
                result.src = getRegisterCode(reg, w);
            }
            break;
        }
        case 0b01:
        {
            value = *(s8*)(current + 2);
            ++result.inc;
            if(d)
            {
                result.dst = getRegisterCode(reg, w);
                result.src = getEffectiveAddress(rm, mod, value);
            }
            else
            {
                result.dst = getEffectiveAddress(rm, mod, value);
                result.src = getRegisterCode(reg, w);
            }
            break;
        }
        case 0b10:
        {
            value = *(s16*)(current + 2);
            result.inc += 2;
            if(d)
            {
                result.dst = getRegisterCode(reg, w);
                result.src = getEffectiveAddress(rm, mod, value);
            }
            else
            {
                result.dst = getEffectiveAddress(rm, mod, value);
                result.src = getRegisterCode(reg, w);
            }
            break;
        }
        case 0b11:
        {
            result.dst = getRegisterCode(d ? reg : rm, w);
            result.src = getRegisterCode(d ? rm : reg, w);
            break;
        }
    }

    return result;
}

internal void outputRegisterToRegister(ASMAtomic& dst, ASMAtomic& src, OutputBuffer& out)
{
    outputASMAtomic(dst, out);
    formatStringAdvance(out, ", ");
    outputASMAtomic(src, out);
}


internal void encodeImmediateToRegisterOrMemory(u8 opCode, u8*& current, u8 mod, u8 w, u8 rm, u16 value, u16 memoryValue, bool neg, u8 writeWide, u8 middlePart = 0b000)
{
    current[0] = (opCode << 1) | (w & 1);
    current[1] = (mod << 6) | (middlePart << 3) | (rm);
    current += 2;

    switch(mod)
    {
        case 0b00:
        {
            if(rm == 0b110)
            {
                u16 value = (u16)memoryValue;
                if(neg)
                {
                    value = -value;
                }
                *(u16*)current = value;
                current += 2;
            }
            break;
        }
        case 0b01:
        {
            u8 value = (u8)memoryValue;
            if(neg)
            {
                value = (u8)-value;
            }
            
            *current = value;
            ++current;
            break;
        }
        case 0b10:
        {
            u16 value = (u16)memoryValue;
            if(neg)
            {
                value = -value;
            }
            *(u16*)current = value;
            current += 2;
            break;
        }
    }

    if(writeWide)
    {
        *(u16*)current = value;
        current += 2;
    }
    else
    {
        *current = (u8)value;
        current += 1;
    }
}

internal ImmediateSize getSize(ASMAtomic& r0, ASMAtomic& r1)
{
    ImmediateSize result = ImmediateSize_Word;
    u8 byte = ((r0.size & 0b11) << 2) | (r1.size & 0b11);

    switch(byte)
    {
        case 0b0101:
        case 0b0001:
        case 0b0100: 
        {
            result = ImmediateSize_Byte;
            break;
        }
    }

    return result;
}

internal void encodeImmediateToMemory(u8 opCode, u8*& current, ASMAtomic& r0, ASMAtomic& r1)
{
    u8 w = (getSize(r0, r1) == ImmediateSize_Byte) ? 0 : 1;

    encodeImmediateToRegisterOrMemory(opCode, current, r0.effective.mod, w, r0.effective.rm, r1.immediate.value, r0.effective.value, r0.effective.neg, w);
}

internal void encodeImmediateToRegister(u8 opCode, u8*& current, ASMAtomic& r0, ASMAtomic& r1)
{
    encodeImmediateToRegisterOrMemory(opCode, current, 0b11, r0.reg.w, r0.reg.code, r1.immediate.value, 0, false, r0.reg.w);
}

internal u8 getAddSignedBit(ASMAtomic& r0, ASMAtomic& r1)
{
    Assert(r1.type == ASMType_Immediate);
    u8 w = (getSize(r0, r1) == ImmediateSize_Byte) ? 0 : 1;
    // @Note if value can fit in a byte, but wide is set, set signed to set, setting last 2 bytes to 01
    // the decode will know that it need s to read only onbe byte
    u8 s = r1.immediate.neg ? 1 : ((r1.immediate.value <= U8Max && w == 1) ? 1 : 0);
    return s;
}

internal void updateFlags16(u32 val, Registers& registers)
{
    if(val == 0)
    {
        registers.flags |= ZF_FLAG;
    }
    else
    {
        registers.flags &= ~ZF_FLAG;
    }
    
    if(val & HIGHEST_16_BIT)
    {
        registers.flags |= SF_FLAG;
    }
    else
    {
        registers.flags &= ~SF_FLAG;
    }

    if(val & (HIGHEST_16_BIT >> 1))
    {
        registers.flags |= CF_FLAG;
    }
    else
    {
        registers.flags &= ~CF_FLAG;
    }

    u32 value = *(u8*)&val;
    u8 cnt = Intrinsics::popcnt(value);
    if((cnt % 2) == 0)
    {
        registers.flags |= PF_FLAG;
    }
    else
    {
        registers.flags &= ~PF_FLAG;
    }

    if(val > S16Max)
    {
        registers.flags |= OF_FLAG;
    }
    else
    {
        registers.flags &= ~OF_FLAG;
    }
}

internal void updateFlags8(u32 val, Registers& registers)
{
    if(val == 0)
    {
        registers.flags |= ZF_FLAG;
    }
    else
    {
        registers.flags &= ~ZF_FLAG;
    }
    if(val & HIGHEST_8_BIT)
    {
        registers.flags |= SF_FLAG;
    }
    else
    {
        registers.flags &= ~SF_FLAG;
    }
    u8 cnt = Intrinsics::popcnt(val);
    if((cnt % 2) == 0)
    {
        registers.flags |= PF_FLAG;
    }
    else
    {
        registers.flags &= ~PF_FLAG;
    }

    if(val > S8Max)
    {
        registers.flags |= OF_FLAG;
    }
    else
    {
        registers.flags &= ~OF_FLAG;
    }
}

internal u8* getRegisterOrAddress(ASMAtomic& dst, Registers& registers, u8* simulationMemory)
{
    u8* destination = 0;
    switch(dst.type)
    {
        case ASMType_Register:
        {
            RegisterIndex destIndex = getRegisterIndex(dst.reg.code, dst.reg.w);
            u16* ds                 = registers.registers + destIndex.index;

            if(dst.reg.w)
            {
                destination = (u8*)ds;
            }
            else
            {
                destination = ((u8*)ds + ((destIndex.part == 2) ? 1 : 0));
            }
            break;
        }
        case ASMType_EffectiveAddress:
        {
            u32 offset  = getEffectiveAddress(registers, dst.effective.rm, dst.effective.mod, dst.effective.value);
            destination = simulationMemory + offset;
            break;
        }
    }

    return destination;
}

struct Label
{
    String name;
    u32    address;
};

enum NextParsing
{
    NextParsing_None,
    NextParsing_Operation,
    NextParsing_Label,
};

struct LabelQueueItem
{
    u8*    byte;
    String name;
};

internal void computerEnhanceUpdate(GameRenderCommand& commands, GameAssets& assets, GameMemory& memory)
{
    // String asmContent   = platformApi.readFile(fromC(DATA_PATH"ASM/listing_0055_challenge_rectangle.asm"), memory.frame);
    String asmContent   = platformApi.readFile(fromC(DATA_PATH"ASM/listing_0056_estimating_cycles.asm"), memory.frame);
    // String asmContent   = platformApi.readFile(fromC(DATA_PATH"ASM/listing_0057_challenge_cycles.asm"), memory.frame);
    Tokenizer tokenizer = tokenize(asmContent);

    RenderGroup group = initOrthographic(&commands, &assets);
    pushClear(group, Color_black);
    Rect2 screen      = getWholeScreenInPixels(group);

    // pushLeftTextAtConstrained(group, font, asmContent, screen, 0);
    
    u8 instructions[512];
    u8* current = instructions;

    Label labels[8];
    u32 labelCount = 0;

    u32 estimation              = 0;
    Registers registers         = {};
    u8 simulationMemory[U16Max] = {};
    CPUStyle cpu                = CPUStyle_8088;

    LabelQueueItem labelQueue[64];
    u32 queueCount = 0;

    while(tokenizer.keepParsing)
    {
        Token op = getToken(tokenizer);
        switch(op.type)
        {
            case Token_EOL : break;
            case Token_Semicolon:
            {
                getNextTokenOfType(tokenizer, Token_EOL);
                break;
            }
            case Token_EndOfStream:
            {
                break;
            }
            case Token_Identifier:
            {
                NextParsing next = NextParsing_None;
                OpCodeType type  = OpCodeType_Mov;
                u8 jumpOpCode = 0;
                if(stringsAreEqual(op.text, fromC("bits")))
                {
                    getNextTokenOfType(tokenizer, Token_EOL);
                }
                else if(stringsAreEqual(op.text, fromC("mov")))
                {
                    type = OpCodeType_Mov;
                    next = NextParsing_Operation;
                }
                else if(stringsAreEqual(op.text, fromC("add")))
                {
                    type = OpCodeType_Add;
                    next = NextParsing_Operation;
                }
                else if(stringsAreEqual(op.text, fromC("sub")))
                {
                    type = OpCodeType_Sub;
                    next = NextParsing_Operation;
                }
                else if(stringsAreEqual(op.text, fromC("cmp")))
                {
                    type = OpCodeType_Cmp;
                    next = NextParsing_Operation;
                }
                else if(stringsAreEqual(op.text, fromC("jnz")))
                {
                    type = OpCodeType_Jump;
                    next = NextParsing_Label;
                    jumpOpCode = 0b01110101;
                }
                else if(stringsAreEqual(op.text, fromC("je")))
                {
                    type = OpCodeType_Jump;
                    next = NextParsing_Label;
                    jumpOpCode = 0b01110100;
                }
                else if(stringsAreEqual(op.text, fromC("jl")))
                {
                    type = OpCodeType_Jump;
                    next = NextParsing_Label;
                    jumpOpCode = 0b01111100;
                }
                else if(stringsAreEqual(op.text, fromC("jle")))
                {
                    type = OpCodeType_Jump;
                    next = NextParsing_Label;
                    jumpOpCode = 0b01111110;
                }
                else if(stringsAreEqual(op.text, fromC("jb")))
                {
                    type = OpCodeType_Jump;
                    next = NextParsing_Label;
                    jumpOpCode = 0b01110010;
                }
                else if(stringsAreEqual(op.text, fromC("jbe")))
                {
                    type = OpCodeType_Jump;
                    next = NextParsing_Label;
                    jumpOpCode = 0b01110110;
                }
                else if(stringsAreEqual(op.text, fromC("jp")))
                {
                    type = OpCodeType_Jump;
                    next = NextParsing_Label;
                    jumpOpCode = 0b01111010;
                }
                else if(stringsAreEqual(op.text, fromC("jo")))
                {
                    type = OpCodeType_Jump;
                    next = NextParsing_Label;
                    jumpOpCode = 0b01110000;
                }
                else if(stringsAreEqual(op.text, fromC("js")))
                {
                    type = OpCodeType_Jump;
                    next = NextParsing_Label;
                    jumpOpCode = 0b01111000;
                }
                else if(stringsAreEqual(op.text, fromC("jne")))
                {
                    type = OpCodeType_Jump;
                    next = NextParsing_Label;
                    jumpOpCode = 0b01110101;
                }
                else if(stringsAreEqual(op.text, fromC("jnl")))
                {
                    type = OpCodeType_Jump;
                    next = NextParsing_Label;
                    jumpOpCode = 0b01111101;
                }
                else if(stringsAreEqual(op.text, fromC("jg")))
                {
                    type = OpCodeType_Jump;
                    next = NextParsing_Label;
                    jumpOpCode = 0b01111111;
                }
                else if(stringsAreEqual(op.text, fromC("jnb")))
                {
                    type = OpCodeType_Jump;
                    next = NextParsing_Label;
                    jumpOpCode = 0b01110011;
                }
                else if(stringsAreEqual(op.text, fromC("ja")))
                {
                    type = OpCodeType_Jump;
                    next = NextParsing_Label;
                    jumpOpCode = 0b01110111;
                }
                else if(stringsAreEqual(op.text, fromC("jnp")))
                {
                    type = OpCodeType_Jump;
                    next = NextParsing_Label;
                    jumpOpCode = 0b01111011;
                }
                else if(stringsAreEqual(op.text, fromC("jno")))
                {
                    type = OpCodeType_Jump;
                    next = NextParsing_Label;
                    jumpOpCode = 0b01110001;
                }
                else if(stringsAreEqual(op.text, fromC("jns")))
                {
                    type       = OpCodeType_Jump;
                    next       = NextParsing_Label;
                    jumpOpCode = 0b01111001;
                }
                else if(stringsAreEqual(op.text, fromC("loop")))
                {
                    type       = OpCodeType_Jump;
                    next       = NextParsing_Label;
                    jumpOpCode = 0b11100010;
                }
                else if(stringsAreEqual(op.text, fromC("loopz")))
                {
                    type       = OpCodeType_Jump;
                    next       = NextParsing_Label;
                    jumpOpCode = 0b11100001;
                }
                else if(stringsAreEqual(op.text, fromC("loopnz")))
                {
                    type       = OpCodeType_Jump;
                    next       = NextParsing_Label;
                    jumpOpCode = 0b11100000;
                }
                else if(stringsAreEqual(op.text, fromC("jcxz")))
                {
                    type       = OpCodeType_Jump;
                    next       = NextParsing_Label;
                    jumpOpCode = 0b11100011;
                }
                else 
                {
                    Token nextToken = getToken(tokenizer);
                    Assert(nextToken.type  == Token_Colon);
                    Assert(labelCount < ArrayCount(labels));
                    Label* label = labels + labelCount++;

                    label->name    = op.text;
                    label->address = (u32)(current - instructions);
                }

                switch(next)
                {
                    case NextParsing_Operation:
                    {
                        ASMAtomic r0 = readAtomic(tokenizer);

                        Token comma = getToken(tokenizer);
                        Assert(comma.type == Token_Comma);

                        ASMAtomic r1 = readAtomic(tokenizer);

                        u8 d = 1;
                        if(r1.type == ASMType_Register && r0.type != ASMType_Register)
                        {
                            ASMAtomic tmp = r0;
                            r0 = r1;
                            r1 = tmp;
                            d = 0;
                        }
                        if(r0.type == ASMType_Register)
                        {
                            switch(r1.type)
                            {
                                case ASMType_Register:
                                {
                                    bool segmentEncoding = false;
                                    u8 opCode;
                                    switch(type)
                                    {
                                        case OpCodeType_Mov: 
                                        {
                                            if(r0.reg.isSegmentRegister == 1)
                                            {
                                                opCode = registerToSegmentRegisterMovOpcode;
                                                segmentEncoding = true;

                                                ASMAtomic tmp = r0;
                                                r0 = r1;
                                                r1 = tmp;
                                                d = 0;
                                            }
                                            else if (r1.reg.isSegmentRegister == 1)
                                            {
                                                opCode = segmentRegisterToRegisterMovOpcode; 
                                                segmentEncoding = true;
                                            }
                                            else
                                            {                                                                 
                                                opCode = registerToRegisterMovOpcode; 
                                            }
                                            break;
                                        }
                                        case OpCodeType_Add: opCode = registerToRegisterAddOpcode; break;
                                        case OpCodeType_Sub: opCode = registerToRegisterSubOpcode; break;
                                        case OpCodeType_Cmp: opCode = registerToRegisterCmpOpcode; break;
                                    }

                                    u8 mod    = 0b11;
                                    if(segmentEncoding)
                                    {
                                        current[0] = opCode;
                                        current[1] = (mod << 6) | (r1.reg.code << 3) | (r0.reg.code);
                                        current += 2;
                                    }
                                    else
                                    {
                                        Assert(r0.reg.w == r1.reg.w);
                                        current[0] = (opCode << 2) | (r1.reg.w & 1);
                                        current[1] = (mod << 6) | (r1.reg.code << 3) | (r0.reg.code);
                                        current += 2;
                                    }
                                    break;
                                }
                                case ASMType_Immediate:
                                {
                                    switch(type)
                                    {
                                        case OpCodeType_Mov:
                                        {
                                            u8 opCode = immediateToRegisterMovOpcode;
                                            current[0] = (opCode << 4) | ((r0.reg.w & 1) << 3) | (r0.reg.code & 0b111);
                                            ++current;
                                            if(r0.reg.w)
                                            {
                                                *(u16*)current = (u16)r1.immediate.value;
                                                current += 2;
                                            }
                                            else
                                            {
                                                *(u8*)current = (u8)r1.immediate.value;
                                                current += 1;
                                            }
                                            break;
                                        }
                                        case OpCodeType_Cmp:
                                        case OpCodeType_Sub:
                                        case OpCodeType_Add:
                                        {
                                            if(isAccumulator(r0.reg))
                                            {
                                                u8 opCode;
                                                switch(type)
                                                {
                                                    case OpCodeType_Add: opCode = immediateToAccumulatorAddOpCode; break;
                                                    case OpCodeType_Sub: opCode = immediateToAccumulatorSubOpCode; break;
                                                    case OpCodeType_Cmp: opCode = immediateToAccumulatorCmpOpCode; break;
                                                }
                                                u32 w = r0.reg.w;
                                                current[0] = (opCode << 1) | (w & 1);
                                                ++current;
                                                if(w)
                                                {
                                                    *(u16*)current = r1.immediate.value;
                                                    current += 2;
                                                }
                                                else
                                                {
                                                    *current = (u8)r1.immediate.value;
                                                    current += 1;
                                                }
                                            }
                                            else
                                            {                 
                                                u8 s = getAddSignedBit(r0, r1);
                                                u8 opCode = immediateToMemoryAddOpcode << 1 | s;
                                                u8 w = (getSize(r0, r1) == ImmediateSize_Byte) ? 0 : 1;

                                                u8 middle;
                                                switch(type)
                                                {
                                                    case OpCodeType_Add: middle = 0b000; break;
                                                    case OpCodeType_Sub: middle = 0b101; break;
                                                    case OpCodeType_Cmp: middle = 0b111; break;
                                                }
                                                encodeImmediateToRegisterOrMemory(opCode, current, 0b11, w, r0.reg.code, r1.immediate.value, 0, r1.immediate.neg, (w == 1 && s == 0), middle);
                                            }
                                            
                                            break;
                                        }
                                    }
                                    
                                    break;
                                }
                                case ASMType_EffectiveAddress:
                                {
                                    if(isAccumulator(r0.reg) && isDirectMemory(r1.effective))
                                    {
                                        u8 opCode = d == 1 ? memoryToAccumulatorMovOpcode : accumulatorToMemoryMovOpcode;
                                        current[0] = (opCode << 1) | (r0.reg.w & 1);
                                        ++current;
                                        *(u16*)current = r1.effective.value;
                                        current += 2;
                                    }
                                    else
                                    {
                                        u8 opCode;
                                        switch(type)
                                        {
                                            case OpCodeType_Mov: opCode = registerToRegisterMovOpcode; break;
                                            case OpCodeType_Add: opCode = registerToRegisterAddOpcode; break;
                                            case OpCodeType_Sub: opCode = registerToRegisterSubOpcode; break;
                                            case OpCodeType_Cmp: opCode = registerToRegisterCmpOpcode; break;
                                        } 
                                        u8 mod = r1.effective.mod;
                                        current[0] = (opCode << 2) | (d << 1) | (r0.reg.w & 1);
                                        current[1] = (mod << 6) | (r0.reg.code << 3) | (r1.effective.rm);
                                        current += 2;

                                        switch(mod)
                                        {
                                            case 0b00:
                                            {
                                                if(r1.effective.rm == 0b110)
                                                {
                                                    *(u16*)current = r1.effective.value;
                                                    current += 2;
                                                }
                                                break;
                                            }
                                            case 0b01:
                                            {
                                                u8 value = (u8)r1.effective.value;
                                                if(r1.effective.neg)
                                                {
                                                    value = (u8)-value;
                                                }
                                                
                                                *current = value;
                                                ++current;
                                                break;
                                            }
                                            case 0b10:
                                            {
                                                u16 value = (u16)r1.effective.value;
                                                if(r1.effective.neg)
                                                {
                                                    value = -value;
                                                }
                                                *(u16*)current = value;
                                                current += 2;
                                                break;
                                            }
                                        }
                                    }
                                    break;
                                }
                            }
                        }
                        else if(r0.type == ASMType_EffectiveAddress && r1.type == ASMType_Immediate)
                        {
                            u8 opCode = 0;
                            switch(type)
                            {
                                case OpCodeType_Mov:
                                {
                                    opCode = immediateToMemoryMovOpcode; 
                                    encodeImmediateToMemory(opCode, current, r0, r1);
                                    break;
                                }
                                case OpCodeType_Sub: 
                                case OpCodeType_Add: 
                                case OpCodeType_Cmp: 
                                {
                                    u8 s = getAddSignedBit(r0, r1);
                                    u8 opCode = immediateToMemoryAddOpcode << 1 | s;
                                    u8 w = (getSize(r0, r1) == ImmediateSize_Byte) ? 0 : 1;

                                    u8 middle;
                                    switch(type)
                                    {
                                        case OpCodeType_Add: middle = 0b000; break;
                                        case OpCodeType_Sub: middle = 0b101; break;
                                        case OpCodeType_Cmp: middle = 0b111; break;
                                    }
                                    encodeImmediateToRegisterOrMemory(opCode, current, r0.effective.mod, w, r0.effective.rm, r1.immediate.value, r0.effective.value, r1.immediate.neg, (w == 1 && s == 0), middle);
                                    break;
                                }
                            }
                            
                        } 
                        else
                        {
                            InvalidCodePath;
                        }                   
                        break;
                    }
                    case NextParsing_Label:
                    {
                        Token labelName = getToken(tokenizer);
                        Assert(labelName.type == Token_Identifier);

                        *current = jumpOpCode;
                        ++current;

                        Label* label = 0;
                        for(u32 i = 0; i < labelCount; ++i)
                        {
                            Label* l = labels + i;
                            if(stringsAreEqual(l->name, labelName.text))
                            {
                                label = l;
                                break;
                            }
                        }

                        if(label)
                        {
                            u32 curAddress = (u32)(current - 1 - instructions);
                            s32 jump = label->address - curAddress;
                            *(s8*)current = (s8)jump;
                        }
                        else
                        {
                            Assert(queueCount < ArrayCount(labelQueue));
                            LabelQueueItem* item = labelQueue + queueCount++;
                            item->name = labelName.text;
                            item->byte = current;
                        }

                        ++current;

                        break;
                    }
                }
                
                break;
            }
            InvalidDefaultCase
        } 
    }

    for(u32 j = 0; j < queueCount; ++j)
    {
        LabelQueueItem* item = labelQueue + j;
        Label* label = 0;
        for(u32 i = 0; i < labelCount; ++i)
        {
            Label* l = labels + i;
            if(stringsAreEqual(l->name, item->name))
            {
                label = l;
                break;
            }
        }

        Assert(label);
        u32 curAddress = (u32)(item->byte - 1 - instructions);
        s32 jump = label->address - curAddress;
        *(s8*)item->byte = (s8)jump;
    }

    u32 instructionSize = (u32)(current - instructions);
    if(instructionSize)
    {
        Stream stream = createStream(SubArena(memory.frame, (u32)kilobytes(512)));
        appendChunk(stream, instructions, (u32)(current - instructions));

        PlatformWriteResult res = platformApi.writeStream(stream, fromC("../data/ASM/binaryASM"), memory.frame);

        String full;
        full.size = (u32)kilobytes(512);
        full.content = PushArray(memory.frame, u8, full.size);
        OutputBuffer out = makeOutputBuffer(full);
        for(;;)
        {
            u8* current = instructions + registers.ip;
            if (((*current) >> 2) == registerToRegisterMovOpcode)
            {
                OutputFormat f = encodeRegisterToRegister(current);
                registers.ip  += f.inc;

                ImmediateSize size = getSize(f.dst, f.src);
                // @Note ESTIMATIONS
                switch(f.dst.type)
                {
                    case ASMType_EffectiveAddress:
                    {
                        switch(f.src.type)
                        {
                            case ASMType_Register:
                            {
                                estimation += 9;
                                break;
                            }
                            case ASMType_Immediate:
                            {
                                estimation += 10;
                                break;
                            }
                            InvalidDefaultCase
                        }
                        MemoryAccessCost access = getMemoryAccesEstimation(registers, f.dst.effective, size, 1, cpu);
                        estimation += access.ea;
                        estimation += access.penalty;
                        break;
                    }
                    case ASMType_Register:
                    {
                        switch(f.src.type)
                        {
                            case ASMType_Register:
                            {
                                estimation += 2;
                                break;
                            }
                            case ASMType_Immediate:
                            {
                                estimation += 4;
                                break;
                            }
                            case ASMType_EffectiveAddress:
                            {
                                estimation += 8;
                                MemoryAccessCost access = getMemoryAccesEstimation(registers, f.src.effective, size, 1, cpu);
                                estimation += access.ea;
                                estimation += access.penalty;
                                break;
                            }
                            InvalidDefaultCase
                        }
                        break;
                    }
                    InvalidDefaultCase
                }

                // @Note SIMULATION
                {
                    u8* dst = getRegisterOrAddress(f.dst, registers, simulationMemory);
                    u8* src = getRegisterOrAddress(f.src, registers, simulationMemory);

                    switch(size)
                    {
                        case ImmediateSize_Byte:
                        {
                            *dst = *src;
                            break;
                        }
                        case ImmediateSize_Word:
                        {
                            *(u16*)dst = *(u16*)src;
                            break;
                        }
                    }
                }

                

                formatStringAdvance(out, "mov ");
                outputRegisterToRegister(f.dst, f.src, out);
            }
            else if(*current == registerToSegmentRegisterMovOpcode)
            {
                u8 mod = *(current + 1) >> 6;
                u8 reg = (*(current + 1) >> 3) & 0b111;
                u8 rm  = *(current + 1) & 0b111;
                u8 w   = 1;

                // @Note SIMULATION
                {
                    RegisterIndex srcIndex = getRegisterIndex(rm, w);
                    u16* src = registers.registers + srcIndex.index;
                    Assert(reg < 4);
                    u16* dst = registers.segmentRegisters + reg;
                    *dst = *src;
                }

                estimation += 2;

                formatStringAdvance(out, "mov ");
                getSegmentRegisterName(reg, out);
                formatStringAdvance(out, ", ");
                getRegisterName(rm, w, out);
                registers.ip += 2;
            }
            else if(*current == segmentRegisterToRegisterMovOpcode)
            {
                u8 mod = *(current + 1) >> 6;
                u8 reg = (*(current + 1) >> 3) & 0b111;
                u8 rm  = *(current + 1) & 0b111;
                u8 w = 1;

                // @Note SIMULATION
                {
                    u16* dst = registers.registers + getRegisterIndex(rm, w).index;
                    Assert(reg < 4);
                    u16* src = registers.segmentRegisters + reg;
                    *dst = *src;
                }

                estimation += 2;

                formatStringAdvance(out, "mov ");
                getRegisterName(rm, w, out);
                formatStringAdvance(out, ", ");
                getSegmentRegisterName(reg, out);
                
                registers.ip += 2;
            }
            else if (((*current) >> 2) == registerToRegisterAddOpcode)
            {
                OutputFormat f = encodeRegisterToRegister(current);
                registers.ip  += f.inc;
                ImmediateSize size = getSize(f.dst, f.src);

                // @Note Estimations
                switch(f.dst.type)
                {
                    case ASMType_EffectiveAddress:
                    {
                        switch(f.src.type)
                        {
                            case ASMType_Register:
                            {
                                estimation += 16;
                                break;
                            }
                            case ASMType_Immediate:
                            {
                                estimation += 17;
                                break;
                            }
                            InvalidDefaultCase
                        }
                        MemoryAccessCost a = getMemoryAccesEstimation(registers, f.dst.effective, size, 2, cpu);
                        estimation += a.ea;
                        estimation += a.penalty;
                        break;
                    }
                    case ASMType_Register:
                    {
                        switch(f.src.type)
                        {
                            case ASMType_Register:
                            {
                                estimation += 3;
                                break;
                            }
                            case ASMType_Immediate:
                            {
                                estimation += 4;
                                break;
                            }
                            case ASMType_EffectiveAddress:
                            {
                                estimation += 9;
                                MemoryAccessCost a = getMemoryAccesEstimation(registers, f.src.effective, size, 1, cpu);
                                estimation += a.ea;
                                estimation += a.penalty;
                                break;
                            }
                            InvalidDefaultCase
                        }
                        break;
                    }
                    InvalidDefaultCase
                }
                // @Note SIMULATION 
                {
                    u8* dst            = getRegisterOrAddress(f.dst, registers, simulationMemory);
                    u8* src            = getRegisterOrAddress(f.src, registers, simulationMemory);
                    
                    
                    if(size == ImmediateSize_Word)
                    {
                        *(u16*)dst += *(u16*)src;
                        updateFlags16(*dst, registers);
                    }
                    else
                    {
                        *dst += *src;
                        updateFlags8(*dst, registers);
                    }

                    
                }    

                formatStringAdvance(out, "add ");
                outputRegisterToRegister(f.dst, f.src, out);
            }
            else if (((*current) >> 2) == registerToRegisterSubOpcode)
            {
                OutputFormat f = encodeRegisterToRegister(current);
                registers.ip  += f.inc;
                ImmediateSize size = getSize(f.dst, f.src);

                // @Note Estimations
                switch(f.dst.type)
                {
                    case ASMType_EffectiveAddress:
                    {
                        switch(f.src.type)
                        {
                            case ASMType_Register:
                            {
                                estimation += 16;
                                break;
                            }
                            case ASMType_Immediate:
                            {
                                estimation += 17;
                                break;
                            }
                            InvalidDefaultCase
                        }
                        MemoryAccessCost a = getMemoryAccesEstimation(registers, f.dst.effective, size, 2, cpu);
                        estimation += a.ea;
                        estimation += a.penalty;
                        break;
                    }
                    case ASMType_Register:
                    {
                        switch(f.src.type)
                        {
                            case ASMType_Register:
                            {
                                estimation += 3;
                                break;
                            }
                            case ASMType_Immediate:
                            {
                                estimation += 5;
                                break;
                            }
                            case ASMType_EffectiveAddress:
                            {
                                estimation += 9;
                                MemoryAccessCost a = getMemoryAccesEstimation(registers, f.src.effective, size , 1, cpu);
                                estimation += a.ea;
                                estimation += a.penalty;
                                break;
                            }
                            InvalidDefaultCase
                        }
                        break;
                    }
                    InvalidDefaultCase
                }
                // @Note SIMULATION 
                {
                    u8* dst            = getRegisterOrAddress(f.dst, registers, simulationMemory);
                    u8* src            = getRegisterOrAddress(f.src, registers, simulationMemory);
                    
                    
                    if(size == ImmediateSize_Word)
                    {
                        *(u16*)dst -= *(u16*)src;
                        updateFlags16(*dst, registers);
                    }
                    else
                    {
                        *dst -= *src;
                        updateFlags8(*dst, registers);
                    }

                    
                }    

                formatStringAdvance(out, "sub ");
                outputRegisterToRegister(f.dst, f.src, out);
            }
            else if (((*current) >> 2) == registerToRegisterCmpOpcode)
            {
                OutputFormat f = encodeRegisterToRegister(current);
                registers.ip  += f.inc;
                ImmediateSize size = getSize(f.dst, f.src);

                // @Note estimations
                switch(f.dst.type)
                {
                    case ASMType_EffectiveAddress:
                    {
                        switch(f.src.type)
                        {
                            case ASMType_Register:
                            {
                                estimation += 9;
                                break;
                            }
                            case ASMType_Immediate:
                            {
                                estimation += 10;
                                break;
                            }
                            InvalidDefaultCase
                        }
                        MemoryAccessCost a = getMemoryAccesEstimation(registers, f.dst.effective, size, 1, cpu);
                        estimation += a.ea;
                        estimation += a.penalty;
                        break;
                    }
                    case ASMType_Register:
                    {
                        switch(f.src.type)
                        {
                            case ASMType_Register:
                            {
                                estimation += 3;
                                break;
                            }
                            case ASMType_Immediate:
                            {
                                estimation += 4;
                                break;
                            }
                            case ASMType_EffectiveAddress:
                            {
                                estimation += 9;
                                MemoryAccessCost a = getMemoryAccesEstimation(registers, f.src.effective, size, 1, cpu);
                                estimation += a.ea;
                                estimation += a.penalty;
                                break;
                            }
                            InvalidDefaultCase
                        }
                        break;
                    }
                    InvalidDefaultCase
                }
                // @Note SIMULATION 
                {
                    u8* dst            = getRegisterOrAddress(f.dst, registers, simulationMemory);
                    u8* src            = getRegisterOrAddress(f.src, registers, simulationMemory);
                    
                    
                    if(size == ImmediateSize_Word)
                    {
                        u16 val = *(u16*)dst - *(u16*)src;
                        updateFlags16(val, registers);
                    }
                    else
                    {
                        u8 val = *dst - *src;
                        updateFlags8(val, registers);
                    }
                }    

                formatStringAdvance(out, "cmp ");
                outputRegisterToRegister(f.dst, f.src, out);
            }
            else if( ((*current) >> 4) == immediateToRegisterMovOpcode)
            {
                formatStringAdvance(out, "mov ");
                u8 w = ((*current) >> 3) & 1;
                u8 reg = (*current) & 0b111;
                registers.ip += 1;

                getRegisterName(reg, w, out);
                formatStringAdvance(out, ", ");

                u16 data = 0;
                if(w)
                {
                    data = *((u16*)(current + 1));
                    registers.ip += 2;
                }
                else
                {
                    data = (u16)*((u8*)(current + 1));
                    registers.ip += 1;
                }
                formatStringAdvance(out, "%d", (u32)data);

                // @Note SIMULATION
                {
                    RegisterIndex index = getRegisterIndex(reg, w);
                    u16* dst = registers.registers + index.index;
                    if(w)
                    {
                        *dst = data;
                    }
                    else
                    {
                        *((u8*)dst + ((index.part == 2) ? 1 : 0)) = (u8)data;
                    }

                    estimation += 4;
                }
            }
            else if((*current >> 1) == immediateToMemoryMovOpcode)
            {
                OutputFormat format = encodeImmediateToMemory(current);
                registers.ip += format.inc;

                ImmediateSize size = getSize(format.dst, format.src);

                formatStringAdvance(out, "mov ");
                immediateToMemory(format, out);

                MemoryAccessCost a = getMemoryAccesEstimation(registers, format.dst.effective, size, 1, cpu);
                estimation += a.ea + a.penalty + 10;
                // @Note SIMULATION
                {
                    u8 w = (*current) & 1;
                    u8 mod = *(current + 1) >> 6;
                    u8 rm  = *(current + 1) & 0b111;

                    current += 2;
                    s32 memoryDisplacement = 0;
                    switch(mod)
                    {
                        case 0b00:
                        {
                            if(rm == 6)
                            {
                                memoryDisplacement = getEffectiveAddress(registers, rm, mod, *(s16*)current);
                                current += 2;
                            }
                            else
                            {
                                memoryDisplacement = getEffectiveAddress(registers, rm, mod, 0);
                            }
                            
                            break;
                        }
                        case 0b01:
                        {                        
                            memoryDisplacement = getEffectiveAddress(registers, rm, mod, *(s8*)current);
                            current += 1;
                            break;
                        }
                        case 0b10:
                        {
                            memoryDisplacement = getEffectiveAddress(registers, rm, mod, *(s16*)current);
                            current += 2;
                            break;
                        }
                        case 0b11:
                        {
                            Assert(false);
                            break;
                        }
                    }

                    u8* mem = simulationMemory + memoryDisplacement;
                    if(w)
                    {
                        *(u16*)mem = *((u16*)current);
                    }
                    else
                    {
                        *(u8*)mem = *((u8*)current);
                    }
                }
            }
            else if((*current >> 2) == immediateToMemoryAddOpcode)
            {
                u8 valueIsSigned = (((*current) >> 1) & 1) > 0;
                OutputFormat format = encodeImmediateToMemory(current, valueIsSigned);
                registers.ip += format.inc;
                ImmediateSize size = getSize(format.dst, format.src);
                // @Improve copy paste from other ADD
                switch(format.dst.type)
                {
                    case ASMType_EffectiveAddress:
                    {
                        switch(format.src.type)
                        {
                            case ASMType_Register:
                            {
                                estimation += 16;
                                break;
                            }
                            case ASMType_Immediate:
                            {
                                estimation += 17;
                                break;
                            }
                            InvalidDefaultCase
                        }
                        MemoryAccessCost a = getMemoryAccesEstimation(registers, format.dst.effective, size, 2, cpu);
                        estimation += a.ea;
                        estimation += a.penalty;
                        break;
                    }
                    case ASMType_Register:
                    {
                        switch(format.src.type)
                        {
                            case ASMType_Register:
                            {
                                estimation += 3;
                                break;
                            }
                            case ASMType_Immediate:
                            {
                                estimation += 4;
                                break;
                            }
                            case ASMType_EffectiveAddress:
                            {
                                estimation += 9;
                                MemoryAccessCost a = getMemoryAccesEstimation(registers, format.src.effective, size, 1, cpu);
                                estimation += a.ea;
                                estimation += a.penalty;
                                break;
                            }
                            InvalidDefaultCase
                        }
                        break;
                    }
                    InvalidDefaultCase
                }

                // @Note SIMULATION
                {
                    u8 w   = (*current) & 1;
                    u8 mod = *(current + 1) >> 6;
                    u8 rm  = *(current + 1) & 0b111;

                    switch(mod)
                    {
                        case 0b11:
                        {
                            s32 data = 0;
                            if(w && valueIsSigned == 0)
                            {
                                data = valueIsSigned ? (s32)*((s16*)(current + 2)) : (s32)*((u16*)(current + 2));
                            }
                            else
                            {
                                data = valueIsSigned ? (s32)*((s8*)(current + 2)) : (s32)*((u8*)(current + 2));
                            }

                            RegisterIndex dstIndex = getRegisterIndex(rm, w);
                            u16* dst = registers.registers + dstIndex.index;
                            if(w)
                            {
                                u16 val = 0;
                                switch((current[1] >> 3) & 0b111)
                                {
                                    case  0b000: *dst += (s16)data; val = *dst; break;
                                    case  0b111: val = *dst - (s16)data; break;
                                    default: *dst -= (s16)data; val = *dst;break;
                                }

                                updateFlags16(val, registers);
                            }
                            else
                            {
                                u8 val = 0;
                                u8* ds = (u8*)dst + ((dstIndex.part == 2) ? 1 : 0);
                                switch((current[1] >> 3) & 0b111)
                                {
                                    case  0b000: *ds += (s8)data; val = *ds; break;
                                    case  0b111:  val = *ds - (s8)data; break;
                                    default: *ds -= (s8)data; val = *ds; break;
                                }
                                updateFlags8(val, registers);
                            }
                            break;
                        }
                    }
                }
                if(((current[1] >> 3) & 0b111) == 0b000)
                {
                    formatStringAdvance(out, "add ");
                }
                else if(((current[1] >> 3) & 0b111) == 0b111)
                {
                    formatStringAdvance(out, "cmp ");
                }
                else
                {
                    formatStringAdvance(out, "sub ");
                }
                
                immediateToMemory(format, out);
            }
            else if((*current >> 1) == immediateToAccumulatorAddOpCode 
                || (*current >> 1) == immediateToAccumulatorSubOpCode
                || (*current >> 1) == immediateToAccumulatorCmpOpCode)
            {
                char* operation;
                u8 type = 0;
                switch(*current >> 1)
                {
                    case immediateToAccumulatorAddOpCode: operation = "add"; type = 0; break;
                    case immediateToAccumulatorSubOpCode: operation = "sub"; type = 1; break;
                    case immediateToAccumulatorCmpOpCode: operation = "cmp"; type = 2; break;
                }

                u32 w = (*current) & 1;
                ++current;
                registers.ip += 1;
                s32 value = 0;
                if(w)
                {
                    value = *(s16*)current;
                    current += 2;
                    registers.ip += 2;
                }
                else
                {
                    value = *(s8*)current;
                    current += 1;
                    registers.ip += 1;
                }

                formatStringAdvance(out, "%s %s, %i", operation,  w ? "AX" : "AL", value);

                // @Note SIMULATION 
                {
                    RegisterIndex destIndex = {};
                    destIndex.part = w ? 0 : 1;
                    
                    u16* dst = registers.registers + destIndex.index;
                    if(w)
                    {
                        u16 val = 0;
                        switch(type)
                        {
                            case 0: *dst += (s16)value; val = *dst; break;
                            case 1: *dst -= (s16)value; val = *dst; break;
                            case 2: val = *dst - (s16)value; break;
                        }
                        updateFlags16(val, registers);
                    }
                    else
                    {
                        u8 val = 0;
                        switch(type)
                        {
                            case 0: *(u8*)dst += (s8)value; val = *(u8*)dst; break;
                            case 1: *(u8*)dst -= (s8)value; val = *(u8*)dst;break;
                            case 2: val = *(u8*)dst - (s8)value; break;
                        }
                        updateFlags8(val, registers);
                    }

                    estimation += 4;
                }            
            }
            else if((*current >> 1) == memoryToAccumulatorMovOpcode)
            {
                u8 w = (*current) & 1;
                ++current;
                u16 value  = *(u16*)current;
                current += 2;

                registers.ip += 3;
                
                formatStringAdvance(out, "mov %s, [%d]", w ? "AX" : "AL", value);

                // @Note SIMULATION
                {   
                    u8* mem = simulationMemory + value;
                    if(w)
                    {
                        registers.registers[0] = *(u16*)mem;
                    }
                    else
                    {
                        *(u8*)(registers.registers + 0) = *mem;
                    }

                    estimation += 10;
                }
            }
            else if ((*current >> 1) == accumulatorToMemoryMovOpcode)
            {
                u8 w = (*current) & 1;
                ++current;
                u16 value  = *(u16*)current;
                current += 2;
                registers.ip += 3;
                formatStringAdvance(out, "mov [%d], %s", value, w ? "AX" : "AL");

                // @Note SIMULATION
                {   
                    u8* mem = simulationMemory + value;
                    if(w)
                    {
                        *(u16*)mem = registers.registers[0];
                    }
                    else
                    {
                        *mem = *(u8*)(registers.registers + 0);
                    }

                    estimation += 10;
                }
            }
            else
            {
                u8 jumpType = *current;
                switch(jumpType)
                {
                    case 0b01110100: formatStringAdvance(out, "je "); break;
                    case 0b01111100: formatStringAdvance(out, "jl "); break;
                    case 0b01111110: formatStringAdvance(out, "jle "); break;
                    case 0b01110010: formatStringAdvance(out, "jb "); break;
                    case 0b01110110: formatStringAdvance(out, "jbe "); break;
                    case 0b01111010: formatStringAdvance(out, "jp "); break;
                    case 0b01110000: formatStringAdvance(out, "jo "); break;
                    case 0b01111000: formatStringAdvance(out, "js "); break;
                    case 0b01110101: formatStringAdvance(out, "jne "); break;
                    case 0b01111101: formatStringAdvance(out, "jnl "); break;
                    case 0b01111111: formatStringAdvance(out, "jg "); break;
                    case 0b01110011: formatStringAdvance(out, "jnb "); break;
                    case 0b01110111: formatStringAdvance(out, "ja "); break;
                    case 0b01111011: formatStringAdvance(out, "jnp "); break;
                    case 0b01110001: formatStringAdvance(out, "jno "); break;
                    case 0b01111001: formatStringAdvance(out, "jns "); break;
                    case 0b11100010: formatStringAdvance(out, "loop "); break;
                    case 0b11100001: formatStringAdvance(out, "loopz "); break;
                    case 0b11100000: formatStringAdvance(out, "loopnz "); break;
                    case 0b11100011: formatStringAdvance(out, "jcxz "); break;
                }

                ++current;
                s32 value = *(s8*)current;
                formatStringAdvance(out, "%i", value);
                ++current;
    
                // @Note SIMULATION
                {
                    u32 oldValue = registers.ip;
                    switch(jumpType)
                    {
                        case 0b01110100:  
                        { // @Note JE
                            if(registers.flags & ZF_FLAG)
                            {
                                registers.ip += value;
                            }
                            break;
                        }
                        case 0b01111100:  break;
                        case 0b01111110:  break;
                        case 0b01110010: 
                        { // @Note JB
                            if(registers.flags & CF_FLAG)
                            {
                                registers.ip += value;
                            }
                            break;
                        }
                        case 0b01110110:  break;
                        case 0b01111010:  
                        { // @Note JP
                            if(registers.flags & PF_FLAG)
                            {
                                registers.ip += value;
                            }
                            break;
                        }
                        case 0b01110000:  break;
                        case 0b01111000:  break;
                        case 0b01110101: 
                        { // @Note jne
                            if((registers.flags & ZF_FLAG) == 0)
                            {
                                registers.ip += value;
                            }
                            break;
                        } 
                        case 0b01111101:  break;
                        case 0b01111111:  break;
                        case 0b01110011:  break;
                        case 0b01110111:  break;
                        case 0b01111011:  break;
                        case 0b01110001: break;
                        case 0b01111001: break;
                        case 0b11100010: 
                        {
                            u16* cx = registers.registers + 2;
                            --*cx;          
                            if(*cx > 0)
                            {
                                registers.ip += value;
                            }    
                            break; // @Note loop
                        }
                        case 0b11100001:  break;
                        case 0b11100000:  
                        { // @Note loopnz
                            u16* cx = registers.registers + 2;
                            --*cx;              
                            if(((registers.flags & ZF_FLAG) == 0) && (*cx != 0))
                            {
                                registers.ip += value;
                            }
                            break;
                        }
                        case 0b11100011: break;
                    }

                    if(registers.ip == oldValue)
                    {
                        registers.ip += 2;
                    }
                }
            }
            formatStringAdvance(out, "\n");

            if(registers.ip == instructionSize)
            {
                break;
            }
            Assert(registers.ip < instructionSize);
        }

        full.size = (u32)out.offset;
        platformApi.DEBUGWrite(full, fromC(DATA_PATH"ASM/ASM.asm"), memory.frame);
    }

    globalVariable r32 t;
    t += 0.005f;
    M4x4 r = zRotation(t);

    Vector3 xAxis = r * v3(1, 0, 0);
    Vector3 yAxis = r * v3(0, 1, 0);

    FontInstance font = pushFont(group, Font_PressStart2P, 40);
    TextShapeAllocated shape = textOperation(TextOp_DrawText, 
                                         *font.font,
                                          group,
                                          asmContent.content,
                                          asmContent.size,
                                          toV3(topLeft(getWholeScreenInPixels(group)), 0.f),
                                          memory.frame,
                                          RGBA_PACK(1, 1, 1, 1),
                                          font.pixelHeight,
                                          1.f,
                                          xAxis,
                                          yAxis,
                                         v3(0),
                                         RGBA_PACK_V(Color_black));

    char tmp[64];
    u32 s = formatString(tmp, ArrayCount(tmp), "Cycles for %S : %d", getNameOf(CPUStyle, cpu), estimation);
    pushTextAt(group, font,  fromC(tmp, s), v3(600,0,0), finalizeColor(Color_white));

    // Image image  = {};
    // image.width  = 64;
    // image.height = 64;
    // image.pixels = (u32*)(simulationMemory + 256);

    // globalVariable TextureHandle handle;

    // if(isValid(handle) == false)
    // {
    //     handle = getNextTextureHandle(assets, image.width, image.height);
    // }
    // else
    // {
    //     TextureOperation* op    = beginTextureOp(*assets.textureQueue, handle);
    //     Intrinsics::memcpy(op->data, image.pixels, size(image));
    //     completeTextureOp(*assets.textureQueue, op);

    //     r32 mul = 30;
    //     pushSprite(group, handle, toV3(screen.min, 0.f), v3(64.f * mul, 0.f, 0.f), v3(0.f, 64.f * mul, 0.f), Color_white);
    // }
}

