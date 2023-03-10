struct RegisterCode
{
    u8 code;
    u8 w;
};

inline bool isAccumulator(RegisterCode& reg)
{
    return reg.code == 0;
}

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
    return code;
}

globalVariable char* regsW0[] =
{
    "AL", "CL", "DL", "BL", "AH", "CH","DH", "BH"
};

globalVariable char* regsW1[] =
{
    "AX", "CX", "DX", "BX", "SP", "BP","SI", "DI"
};

inline void getRegisterName(u8 reg, u8 w, OutputBuffer& out)
{
    formatStringAdvance(out, w ? regsW1[reg] : regsW0[reg]);
}

#define registerToRegisterCmpOpcode  0b001110
#define registerToRegisterSubOpcode  0b001010
#define registerToRegisterAddOpcode  0b000000
#define registerToRegisterMovOpcode  0b100010
#define immediateToRegisterMovOpcode 0b1011
#define immediateToMemoryMovOpcode   0b1100011
#define immediateToMemoryAddOpcode   0b100000
#define memoryToAccumulatorMovOpcode 0b1010000
#define accumulatorToMemoryMovOpcode 0b1010001
#define immediateToAccumulatorAddOpCode 0b0000010    
#define immediateToAccumulatorSubOpCode 0b0010110    
#define immediateToAccumulatorCmpOpCode 0b0011110    

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
                    u16 displacement = 0;
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
                                    displacement = (u16)next.number.value;
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
                                            displacement = (u16)next.number.value;
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
                                            displacement = (u16)finalToken.number.value;
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
                    else if(displacement <= U8Max)
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
                            if(r1.w == 2) // @Note we parsed BP explicitely, so add a 0 displacement
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

internal u32 outputEffectiveAddress(OutputBuffer& out, u8 rm, s32 value, bool mod0 = false)
{
    u32 increment = 0;
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
                increment = 2;
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
    return increment;
}

enum OpCodeType
{
    OpCodeType_Mov,
    OpCodeType_Add,
    OpCodeType_Sub,
    OpCodeType_Cmp,
    OpCodeType_Jump,
};

internal void immediateToMemory(u8*& current, OutputBuffer& out, bool valueIsSigned = false)
{
    u8 w = (*current) & 1;
    ++current;
    u8 mod = *current >> 6;
    u8 rm  = *current & 0b111;
    ++current;

    switch(mod)
    {
        case 0b00:
        {
            if(rm == 6)
            {
                s32 value = *(s16*)current;
                current += 2;
                outputEffectiveAddress(out, rm, value, true);
            }
            else
            {
                outputEffectiveAddress(out, rm, 0);
            }
            
            break;
        }
        case 0b01:
        {
            s32 value = *(s8*)current;
            ++current;
            outputEffectiveAddress(out, rm, value);
            break;
        }
        case 0b10:
        {
            s32 value = *(s16*)current;
            current += 2;
            outputEffectiveAddress(out, rm, value);
            break;
        }
        case 0b11:
        {
            getRegisterName(rm, w, out);
            break;
        }
    }
    formatStringAdvance(out, ", ");
    s32 data = 0;
    if(mod != 0b11) // @Note else size is implied by register
    {
        if(w)
        {
            formatStringAdvance(out, "word ");
        }
        else
        {
             formatStringAdvance(out, "byte ");
        }
    }

    if(w && valueIsSigned == 0)
    {
        data = valueIsSigned ? (s32)*((s16*)current) : (s32)*((u16*)current);
        current += 2;
    }
    else
    {
        data = valueIsSigned ? (s32)*((s8*)current) : (s32)*((u8*)current);
        current += 1;
    }
    
    formatStringAdvance(out, "%i", data);
}

internal void outputRegisterToRegister(u8*& current, OutputBuffer& out)
{
    u8 d   = ((*current) >> 1) & 1;
    u8 w   = (*current) & 1;
    u8 mod = *(current + 1) >> 6;
    u8 reg = (*(current + 1) >> 3) & 0b111;
    u8 rm  = *(current + 1) & 0b111;
    current +=2;
    s32 value = 0;
    switch(mod)
    {
        case 0b00:
        {
            value = *(u16*)current;
            if(d)
            {
                getRegisterName(reg, w, out);
                formatStringAdvance(out, ", ");
                current += outputEffectiveAddress(out, rm, value, true);
            }
            else
            {
                current += outputEffectiveAddress(out, rm, value, true);
                formatStringAdvance(out, ", ");
                getRegisterName(reg, w, out);
            }
            break;
        }
        case 0b01:
        {
            value = *(s8*)current;
            ++current;
            if(d)
            {
                getRegisterName(reg, w, out);
                formatStringAdvance(out, ", ");
                outputEffectiveAddress(out, rm, value);
            }
            else
            {
                outputEffectiveAddress(out, rm, value);
                formatStringAdvance(out, ", ");
                getRegisterName(reg, w, out);
            }
            break;
        }
        case 0b10:
        {
            value = *(s16*)current;
            current += 2;
            if(d)
            {
                getRegisterName(reg, w, out);
                formatStringAdvance(out, ", ");
                outputEffectiveAddress(out, rm, value);
            }
            else
            {
                outputEffectiveAddress(out, rm, value);
                formatStringAdvance(out, ", ");
                getRegisterName(reg, w, out);
            }
            break;
        }
        case 0b11:
        {
            getRegisterName(d ? reg : rm, w, out);
            formatStringAdvance(out, ", ");
            getRegisterName(d ? rm : reg, w, out);
            break;
        }
    }
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

internal void computerEnhanceUpdate(GameMemory& memory)
{
    String asmContent   = platformApi.readFile(fromC(DATA_PATH"ASM/listing_0041_add_sub_cmp_jnz.asm"), memory.frame);
    Tokenizer tokenizer = tokenize(asmContent);
    
    u8 instructions[512];
    u8* current = instructions;

    struct Label
    {
        String name;
        u32    address;
    };
    Label labels[8];
    u32 labelCount = 0;

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
                                    u8 opCode;
                                    switch(type)
                                    {
                                        case OpCodeType_Mov: opCode = registerToRegisterMovOpcode; break;
                                        case OpCodeType_Add: opCode = registerToRegisterAddOpcode; break;
                                        case OpCodeType_Sub: opCode = registerToRegisterSubOpcode; break;
                                        case OpCodeType_Cmp: opCode = registerToRegisterCmpOpcode; break;
                                    }
                                    u8 mod    = 0b11;
                                    Assert(r0.reg.w == r1.reg.w);
                                    current[0] = (opCode << 2) | (r1.reg.w & 1);
                                    current[1] = (mod << 6) | (r1.reg.code << 3) | (r0.reg.code);

                                    current += 2;
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
                            u32 curAddress = (u32)(current - instructions);
                            s32 jump = label->address - (curAddress + 1);
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
        u32 curAddress = (u32)(item->byte - instructions);
        s32 jump = label->address - (curAddress + 1);
        *(s8*)item->byte = (s8)jump;
    }

    u32 instructionSize = (u32)(current - instructions);

    Stream stream = createStream(SubArena(memory.frame, (u32)kilobytes(2)));
    appendChunk(stream, instructions, (u32)(current - instructions));

    PlatformWriteResult res = platformApi.writeStream(stream, fromC("../data/ASM/binaryASM"), memory.frame);
    platformApi.closeFile(res.handle);

    String full;
    full.size = (u32)kilobytes(2);
    full.content = PushArray(memory.frame, u8, full.size);
    OutputBuffer out = makeOutputBuffer(full);
    current = instructions;
    for(;;)
    {
        if (((*current) >> 2) == registerToRegisterMovOpcode)
        {
            formatStringAdvance(out, "mov ");
            outputRegisterToRegister(current, out);
        }
        else if (((*current) >> 2) == registerToRegisterAddOpcode)
        {
            formatStringAdvance(out, "add ");
            outputRegisterToRegister(current, out);
        }
        else if (((*current) >> 2) == registerToRegisterSubOpcode)
        {
            formatStringAdvance(out, "sub ");
            outputRegisterToRegister(current, out);
        }
        else if (((*current) >> 2) == registerToRegisterCmpOpcode)
        {
            formatStringAdvance(out, "cmp ");
            outputRegisterToRegister(current, out);
        }
        else if( ((*current) >> 4) == immediateToRegisterMovOpcode)
        {
            formatStringAdvance(out, "mov ");
            u8 w = ((*current) >> 3) & 1;
            u8 reg = (*current) & 0b111;
            ++current;

            getRegisterName(reg, w, out);
            formatStringAdvance(out, ", ");

            u32 data = 0;
            if(w)
            {
                data = *((u16*)current);
                current += 2;
            }
            else
            {
                data = (u16)*((u8*)current);
                current += 1;
            }
            formatStringAdvance(out, "%d", data);
        }
        else if((*current >> 1) == immediateToMemoryMovOpcode)
        {
            formatStringAdvance(out, "mov ");
            immediateToMemory(current, out);
        }
        else if((*current >> 2) == immediateToMemoryAddOpcode)
        {
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
            u8 sign = ((*current) >> 1) & 1;
            immediateToMemory(current, out, sign > 0);
        }
        else if((*current >> 1) == immediateToAccumulatorAddOpCode 
             || (*current >> 1) == immediateToAccumulatorSubOpCode
             || (*current >> 1) == immediateToAccumulatorCmpOpCode)
        {
            char* operation;
            switch(*current >> 1)
            {
                case immediateToAccumulatorAddOpCode: operation = "add"; break;
                case immediateToAccumulatorSubOpCode: operation = "sub"; break;
                case immediateToAccumulatorCmpOpCode: operation = "cmp"; break;
            }
            u32 w = (*current) & 1;
            ++current;
            s32 value = 0;
            if(w)
            {
                value = *(s16*)current;
                current += 2;
            }
            else
            {
                value = *(s8*)current;
                current += 1;
            }
            
            formatStringAdvance(out, "%s %s, %i", operation,  w ? "AX" : "AL", value);
        }
        else if((*current >> 1) == memoryToAccumulatorMovOpcode)
        {
            u8 w = (*current) & 1;
            ++current;
            u16 value  = *(u16*)current;
            current += 2;
            
            formatStringAdvance(out, "mov %s, [%d]", w ? "AX" : "AL", value);
        }
        else if ((*current >> 1) == accumulatorToMemoryMovOpcode)
        {
            u8 w = (*current) & 1;
            ++current;
            u16 value  = *(u16*)current;
            current += 2;
            formatStringAdvance(out, "mov [%d], %s", value, w ? "AX" : "AL");
        }
        else
        {
            switch(*current)
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
        }
        formatStringAdvance(out, "\n");

        u32 s = (u32)(current - instructions);
        if(s == instructionSize)
        {
            break;
        }
        Assert((u32)(current - instructions) < instructionSize);
    }

    full.size = (u32)out.offset;
    platformApi.DEBUGWrite(full, fromC(DATA_PATH"ASM/ASM.asm"), memory.frame);
}
