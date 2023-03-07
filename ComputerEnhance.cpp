struct RegisterCode
{
    u8 code;
    u8 w;
};

inline bool isAccumulator(RegisterCode& reg)
{
    return reg.code == 0 && reg.w == 1;
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

#define registerToRegisterMovOpcode  0b100010
#define immediateToRegisterMovOpcode 0b1011
#define immediateToMemoryMovOpcode 0b1100011
#define memoryToAccumulatorMovOpcode 0b1010000
#define accumulatorToMemoryMovOpcode 0b1010001

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
    u16           value;
    ImmediateSize size;
};

struct ASMAtomic
{
    ASMType type;
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
    switch(tok.type)
    {
        case Token_Identifier:
        {
            if(stringsAreEqual(tok.text, fromC("byte")))
            {
                Token val = getToken(tokenizer);
                Assert(val.type == Token_Number);
                result.immediate.value = (u16)val.number.value;
                result.type            = ASMType_Immediate;
                result.immediate.size  = ImmediateSize_Byte;
            }
            else if(stringsAreEqual(tok.text, fromC("word")))
            {
                Token val = getToken(tokenizer);
                Assert(val.type == Token_Number);
                result.immediate.value = (u16)val.number.value;
                result.type            = ASMType_Immediate;
                result.immediate.size  = ImmediateSize_Word;
            }
            else
            {
                result.reg = getRegisterCode(tok.text);
                result.type = ASMType_Register;
            }
            break;
        }
        case Token_Number:
        {
            result.immediate.value = (u16)tok.number.value;
            result.type            = ASMType_Immediate;
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
                            if(r1.w != 2) // @Note we parsed BP explicitely, so add a 0 displacement
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

internal void computerEnhanceUpdate(GameMemory& memory)
{
    String asmContent   = platformApi.readFile(fromC(DATA_PATH"ASM/listing_0040_challenge_movs.asm"), memory.frame);
    Tokenizer tokenizer = tokenize(asmContent);
    
    u8 instructions[512];
    u8* current = instructions;

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
                if(stringsAreEqual(op.text, fromC("bits")))
                {
                    getNextTokenOfType(tokenizer, Token_EOL);
                }
                else if(stringsAreEqual(op.text, fromC("mov")))
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
                                u8 opCode = registerToRegisterMovOpcode;
                                u8 mod    = 0b11;
                                Assert(r0.reg.w == r1.reg.w);
                                current[0] = (opCode << 2) | (r1.reg.w & 1);
                                current[1] = (mod << 6) | (r1.reg.code << 3) | (r0.reg.code);

                                current += 2;
                                break;
                            }
                            case ASMType_Immediate:
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
                                    u8 opCode = registerToRegisterMovOpcode;
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
                        u8 opCode = immediateToMemoryMovOpcode;
                        u8 w    = (r1.immediate.size == ImmediateSize_Byte) ? 0 : 1;

                        current[0] = (opCode << 1) | (w & 1);
                        current[1] = (r0.effective.mod << 6) | (0b000 << 3) | (r0.effective.rm);
                        current += 2;

                        switch(r0.effective.mod)
                        {
                            case 0b01:
                            {
                                u8 value = (u8)r0.effective.value;
                                if(r0.effective.neg)
                                {
                                    value = (u8)-value;
                                }
                                
                                *current = value;
                                ++current;
                                break;
                            }
                            case 0b10:
                            {
                                u16 value = (u16)r0.effective.value;
                                if(r0.effective.neg)
                                {
                                    value = -value;
                                }
                                *(u16*)current = value;
                                current += 2;
                                break;
                            }
                        }

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
                        InvalidCodePath;
                    }                   
                }
                else
                {
                    InvalidCodePath;
                }
                break;
            }
            InvalidDefaultCase
        } 
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
            formatStringAdvance(out, "%d\n", data);
        }
        else if((*current >> 1) == immediateToMemoryMovOpcode)
        {
            formatStringAdvance(out, "mov ");
            u8 w = (*current) & 1;
            ++current;
            u8 mod = *current >> 6;
            u8 rm  = *current & 0b111;
            ++current;

            switch(mod)
            {
                case 0b00:
                {
                    outputEffectiveAddress(out, rm, 0);
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
            }
            formatStringAdvance(out, ", ");
            u32 data = 0;
            if(w)
            {
                formatStringAdvance(out, "word ");
                data = *((u16*)current);
                current += 2;
            }
            else
            {
                formatStringAdvance(out, "byte ");
                data = (u16)*((u8*)current);
                current += 1;
            }
            
            formatStringAdvance(out, "%d\n", data);
        }
        else if((*current >> 1) == memoryToAccumulatorMovOpcode)
        {
            ++current;
            u16 value  = *(u16*)current;
            current += 2;
            formatStringAdvance(out, "mov ax, [%d]", value);
        }
        else if ((*current >> 1) == accumulatorToMemoryMovOpcode)
        {
            ++current;
            u16 value  = *(u16*)current;
            current += 2;
            formatStringAdvance(out, "mov [%d], ax", value);
        }
        else
        {
            InvalidCodePath
        }
        formatStringAdvance(out, "\n");

        if((u32)(current - instructions) == instructionSize)
        {
            break;
        }
        Assert((u32)(current - instructions) < instructionSize);
    }

    u32 breakMe = 0;
    ++breakMe;

}
