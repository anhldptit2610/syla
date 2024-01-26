#include "cpu.h"

static const uint8_t instructions_cycles[256] = {
//  0  1   2   3   4   5   6   7   8  9   A   B   C   D   E  F
    4, 10, 7,  5,  5,  5,  7,  4,  4, 10, 7,  5,  5,  5,  7, 4,  // 0
    4, 10, 7,  5,  5,  5,  7,  4,  4, 10, 7,  5,  5,  5,  7, 4,  // 1
    4, 10, 16, 5,  5,  5,  7,  4,  4, 10, 16, 5,  5,  5,  7, 4,  // 2
    4, 10, 13, 5,  10, 10, 10, 4,  4, 10, 13, 5,  5,  5,  7, 4,  // 3
    5, 5,  5,  5,  5,  5,  7,  5,  5, 5,  5,  5,  5,  5,  7, 5,  // 4
    5, 5,  5,  5,  5,  5,  7,  5,  5, 5,  5,  5,  5,  5,  7, 5,  // 5
    5, 5,  5,  5,  5,  5,  7,  5,  5, 5,  5,  5,  5,  5,  7, 5,  // 6
    7, 7,  7,  7,  7,  7,  7,  7,  5, 5,  5,  5,  5,  5,  7, 5,  // 7
    4, 4,  4,  4,  4,  4,  7,  4,  4, 4,  4,  4,  4,  4,  7, 4,  // 8
    4, 4,  4,  4,  4,  4,  7,  4,  4, 4,  4,  4,  4,  4,  7, 4,  // 9
    4, 4,  4,  4,  4,  4,  7,  4,  4, 4,  4,  4,  4,  4,  7, 4,  // A
    4, 4,  4,  4,  4,  4,  7,  4,  4, 4,  4,  4,  4,  4,  7, 4,  // B
    5, 10, 10, 10, 11, 11, 7,  11, 5, 4,  10, 10, 11, 11, 7, 11, // C
    5, 10, 10, 10, 11, 11, 7,  11, 5, 10, 10, 10, 11, 17, 7, 11, // D
    5, 10, 10, 18, 11, 11, 7,  11, 5, 5,  10, 4,  11, 17, 7, 11, // E
    5, 10, 10, 4,  11, 11, 7,  11, 5, 5,  10, 4,  11, 17, 7, 11  // F
};

static const char* DISASSEMBLE_TABLE[] = {"nop", "lxi b,#", "stax b", "inx b",
    "inr b", "dcr b", "mvi b,#", "rlc", "ill", "dad b", "ldax b", "dcx b",
    "inr c", "dcr c", "mvi c,#", "rrc", "ill", "lxi d,#", "stax d", "inx d",
    "inr d", "dcr d", "mvi d,#", "ral", "ill", "dad d", "ldax d", "dcx d",
    "inr e", "dcr e", "mvi e,#", "rar", "ill", "lxi h,#", "shld", "inx h",
    "inr h", "dcr h", "mvi h,#", "daa", "ill", "dad h", "lhld", "dcx h",
    "inr l", "dcr l", "mvi l,#", "cma", "ill", "lxi sp,#", "sta $", "inx sp",
    "inr M", "dcr M", "mvi M,#", "stc", "ill", "dad sp", "lda $", "dcx sp",
    "inr a", "dcr a", "mvi a,#", "cmc", "mov b,b", "mov b,c", "mov b,d",
    "mov b,e", "mov b,h", "mov b,l", "mov b,M", "mov b,a", "mov c,b", "mov c,c",
    "mov c,d", "mov c,e", "mov c,h", "mov c,l", "mov c,M", "mov c,a", "mov d,b",
    "mov d,c", "mov d,d", "mov d,e", "mov d,h", "mov d,l", "mov d,M", "mov d,a",
    "mov e,b", "mov e,c", "mov e,d", "mov e,e", "mov e,h", "mov e,l", "mov e,M",
    "mov e,a", "mov h,b", "mov h,c", "mov h,d", "mov h,e", "mov h,h", "mov h,l",
    "mov h,M", "mov h,a", "mov l,b", "mov l,c", "mov l,d", "mov l,e", "mov l,h",
    "mov l,l", "mov l,M", "mov l,a", "mov M,b", "mov M,c", "mov M,d", "mov M,e",
    "mov M,h", "mov M,l", "hlt", "mov M,a", "mov a,b", "mov a,c", "mov a,d",
    "mov a,e", "mov a,h", "mov a,l", "mov a,M", "mov a,a", "add b", "add c",
    "add d", "add e", "add h", "add l", "add M", "add a", "adc b", "adc c",
    "adc d", "adc e", "adc h", "adc l", "adc M", "adc a", "sub b", "sub c",
    "sub d", "sub e", "sub h", "sub l", "sub M", "sub a", "sbb b", "sbb c",
    "sbb d", "sbb e", "sbb h", "sbb l", "sbb M", "sbb a", "ana b", "ana c",
    "ana d", "ana e", "ana h", "ana l", "ana M", "ana a", "xra b", "xra c",
    "xra d", "xra e", "xra h", "xra l", "xra M", "xra a", "ora b", "ora c",
    "ora d", "ora e", "ora h", "ora l", "ora M", "ora a", "cmp b", "cmp c",
    "cmp d", "cmp e", "cmp h", "cmp l", "cmp M", "cmp a", "rnz", "pop b",
    "jnz $", "jmp $", "cnz $", "push b", "adi #", "rst 0", "rz", "ret", "jz $",
    "ill", "cz $", "call $", "aci #", "rst 1", "rnc", "pop d", "jnc $", "out p",
    "cnc $", "push d", "sui #", "rst 2", "rc", "ill", "jc $", "in p", "cc $",
    "ill", "sbi #", "rst 3", "rpo", "pop h", "jpo $", "xthl", "cpo $", "push h",
    "ani #", "rst 4", "rpe", "pchl", "jpe $", "xchg", "cpe $", "ill", "xri #",
    "rst 5", "rp", "pop psw", "jp $", "di", "cp $", "push psw", "ori #",
    "rst 6", "rm", "sphl", "jm $", "ei", "cm $", "ill", "cpi #", "rst 7"};

typedef enum R16 {
    AF,
    BC,
    DE,
    HL,
    SP,
    PC
} r16_t;

typedef enum R8 {
    A,
    B,
    C,
    D,
    E,
    H,
    L
} r8_t;

int check_parity(uint8_t val)
{
    int zeros = 0;

    while (val > 0) {
        zeros = (val & 0x01) ? zeros + 1 : zeros;
        val >>= 1;
    }
    return (zeros % 2 == 0) ? 1 : 0;
}

uint8_t mem_read(struct i8080 *cpu, uint16_t addr)
{
    // if (addr >= 0x4000)
    //     addr = (addr & 0x1fff) + 0x2000;
    return cpu->mem[addr];
}

void mem_write(struct i8080 *cpu, uint16_t addr, uint8_t val)
{
    // if (addr >= 0x4000)
    //     addr = (addr & 0x1fff) + 0x2000;
    cpu->mem[addr] = val;
}

uint8_t fetch_8(struct i8080 *cpu)
{
    cpu->operand[0] = mem_read(cpu, cpu->pc++);
    return cpu->operand[0];
}

uint16_t fetch_16(struct i8080 *cpu)
{
    cpu->operand[0] = mem_read(cpu, cpu->pc++);
    cpu->operand[1] = mem_read(cpu, cpu->pc++);
    return TO_U16(cpu->operand[0], cpu->operand[1]);
}

void stack_push_8(struct i8080 *cpu, uint8_t val)
{
    cpu->sp--;
    mem_write(cpu, cpu->sp, val);
}

void stack_push_16(struct i8080 *cpu, uint16_t val)
{
    stack_push_8(cpu, MSB(val));
    stack_push_8(cpu, LSB(val));
}

uint8_t stack_pop_8(struct i8080 *cpu)
{
    return mem_read(cpu, cpu->sp++);
}

uint16_t stack_pop_16(struct i8080 *cpu)
{
    uint8_t lsb = stack_pop_8(cpu), msb = stack_pop_8(cpu);

    return TO_U16(lsb, msb);
}

void map_roms_to_memory(struct i8080 *cpu, char *rom_path, uint16_t start_addr)
{
    FILE *fp = fopen(rom_path, "r");
    long file_size;

    if (!fp) {
        fprintf(stderr, "Can't open ROM file\n");
        exit(EXIT_FAILURE);
    }

    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);
    rewind(fp);

    if (fread(&cpu->mem[start_addr], 1, file_size, fp) != file_size) {
        fprintf(stderr, "ROM wasn't read properly\n");
        exit(EXIT_FAILURE);
    }

    fclose(fp);
}

void debug_output(struct i8080 *cpu)
{
    FILE *fp = fopen("log.txt", "a");
    uint8_t opcode = mem_read(cpu, cpu->pc);

    if (!fp) {
        fprintf(stderr, "Can't open/create log file\n");
        return;
    }
    fprintf(fp, "PC: %04X, AF: %04X, BC: %04X, DE: %04X, HL: %04X, SP: %04X, CYC: %lu",
            cpu->pc, cpu->af, cpu->bc, cpu->de, cpu->hl, cpu->sp, cpu->cycles);
    fprintf(fp, " - %s", DISASSEMBLE_TABLE[opcode]);
    fprintf(fp, "\n");
    fclose(fp);
}

/* Data Transfer Group */

static inline void shld_a16(struct i8080 *cpu, uint16_t addr)
{
    mem_write(cpu, addr, cpu->l);
    mem_write(cpu, addr + 1, cpu->h);
}

static inline void lhld_a16(struct i8080 *cpu, uint16_t addr)
{
    cpu->l = mem_read(cpu, addr);
    cpu->h = mem_read(cpu, addr + 1);
}

static inline void xchg(struct i8080 *cpu)
{
    uint8_t tmp;

    // h<->d
    tmp = cpu->h;
    cpu->h = cpu->d;
    cpu->d = tmp;
    // l<->e
    tmp = cpu->l;
    cpu->l = cpu->e;
    cpu->e = tmp;
}

/* Arithmetic Group */
static inline void toggle_zsp(struct i8080 *cpu, uint8_t val)
{
    cpu->ZERO = !val;
    cpu->SIGN = BIT(val, 7);
    cpu->PARITY = check_parity(val);
}

static inline bool check_carry(int n, uint8_t a, uint8_t b, uint8_t c)
{
    uint16_t res = a + b + c, carry = a ^ b ^ res;

    return carry & (1 << n);
}

// the result is stored at register A
static inline void add(struct i8080 *cpu, uint8_t b, uint8_t c)
{
    cpu->CARRY = check_carry(8, cpu->a, b, c);
    cpu->HALFC = check_carry(4, cpu->a, b, c);
    cpu->a = cpu->a + b + c;
    toggle_zsp(cpu, cpu->a);
}

static inline void sub(struct i8080 *cpu, uint8_t b, uint8_t c)
{
    add(cpu, ~b, !c);
    cpu->CARRY = !cpu->CARRY;
}

static inline uint8_t inr(struct i8080 *cpu, uint8_t val)
{
    toggle_zsp(cpu, val + 1);
    cpu->HALFC = check_carry(4, val, 1, 0);
    return val + 1;
}

static inline uint8_t dcr(struct i8080 *cpu, uint8_t val)
{
    toggle_zsp(cpu, val - 1);
    cpu->HALFC = check_carry(4, val, 0xff, 0);
    return val - 1;
}

static inline void inr_indirect_hl(struct i8080 *cpu)
{
    uint16_t val = mem_read(cpu, cpu->hl), res = inr(cpu, val);

    mem_write(cpu, cpu->hl, res);
}

static inline void dcr_indirect_hl(struct i8080 *cpu)
{
    uint16_t val = mem_read(cpu, cpu->hl), res = dcr(cpu, val);

    mem_write(cpu, cpu->hl, res);
}

static inline void dad_r16(struct i8080 *cpu, uint16_t val)
{
    cpu->CARRY = (uint32_t)(cpu->hl + val) >= 0x10000;
    cpu->hl += val;
}

/* credit: https://github.com/superzazu/8080/ */
static inline void daa(struct i8080 *cpu)
{
    bool cy = cpu->CARRY;
    uint8_t correction = 0;

    uint8_t lsb = cpu->a & 0x0F;
    uint8_t msb = cpu->a >> 4;

    if (cpu->HALFC || lsb > 9) {
        correction += 0x06;
    }

    if (cpu->CARRY || msb > 9 || (msb >= 9 && lsb > 9)) {
        correction += 0x60;
        cy = 1;
    }

    cpu->HALFC = ((cpu->a & 0x0f) + (correction & 0x0f)) >= 0x10;
    cpu->CARRY = cy;
    cpu->a += correction;
    cpu->ZERO = !cpu->a;
    cpu->SIGN = BIT(cpu->a, 7);
    cpu->PARITY = check_parity(cpu->a);
}

/* Logical Group */
static inline void and(struct i8080 *cpu, uint8_t val)
{
    uint8_t a = cpu->a;

    cpu->a = a & val;
    toggle_zsp(cpu, cpu->a);
    cpu->CARRY = 0;
    cpu->HALFC = ((a | val) & 0x08) != 0;
//    cpu->HALFC = (((a & 0x0f) + (val & 0x0f)) & 0x10) > 0;
}

static inline void xor(struct i8080 *cpu, uint8_t val)
{
    uint8_t a = cpu->a;

    cpu->a = a ^ val;
    toggle_zsp(cpu, cpu->a);
    cpu->CARRY = 0; 
    cpu->HALFC = 0;
}

static inline void or(struct i8080 *cpu, uint8_t val)
{
    uint8_t a = cpu->a;

    cpu->a = a | val;
    toggle_zsp(cpu, cpu->a);
    cpu->CARRY = 0;
    cpu->HALFC = 0;
}

static inline void cmp(struct i8080 *cpu, uint8_t val)
{
    uint8_t a = cpu->a, res = a - val;

    toggle_zsp(cpu, res);
    cpu->CARRY = a < val;
    cpu->HALFC = (((a & 0x0f) + (~val & 0x0f) + (1 & 0x0f)) & 0x10) == 0x10;
}

static inline void rlc(struct i8080 *cpu)
{
    uint8_t new_c = BIT(cpu->a, 7);

    cpu->a = (cpu->a << 1) | new_c;
    cpu->CARRY = new_c;
}

static inline void rrc(struct i8080 *cpu)
{
    uint8_t new_c = BIT(cpu->a, 0);

    cpu->a = (cpu->a >> 1) | (new_c << 7);
    cpu->CARRY = new_c;
}

static inline void ral(struct i8080 *cpu)
{
    uint8_t old_c = cpu->CARRY, new_c = BIT(cpu->a, 7);

    cpu->a = (cpu->a << 1) | old_c;
    cpu->CARRY = new_c;
}

static inline void rar(struct i8080 *cpu)
{
    uint8_t old_c = cpu->CARRY, new_c = BIT(cpu->a, 0);

    cpu->a = (cpu->a >> 1) | (old_c << 7);
    cpu->CARRY = new_c;
}

/* Branch Group */
static inline void jmp(struct i8080 *cpu, uint16_t addr, bool cond)
{
    if (cond)
        cpu->pc = addr;
}

static inline void call(struct i8080 *cpu, uint16_t addr, bool cond)
{
    if (cond) {
        cpu->cycles += 6;
        stack_push_16(cpu, cpu->pc);
        cpu->pc = addr;
    }
}

void ret(struct i8080 *cpu, bool cond)
{
    if (cond) {
        cpu->cycles += 6;
        cpu->pc = stack_pop_16(cpu);
    }
}

void rst_n(struct i8080 *cpu, uint8_t n)
{
    stack_push_16(cpu, cpu->pc);
    cpu->pc = 8 * n;
}

/* Stack, I/O and Machine Control Group */
void xthl(struct i8080 *cpu)
{
    uint8_t lval = mem_read(cpu, cpu->sp), hval = mem_read(cpu, cpu->sp + 1), tmp;

    // l <-> (sp)
    tmp = cpu->l;
    cpu->l = lval;
    mem_write(cpu, cpu->sp, tmp);

    // h <-> (sp + 1)
    tmp = cpu->h;
    cpu->h = hval;
    mem_write(cpu, cpu->sp + 1, tmp);
}

void in_d8(struct i8080 *cpu)
{
    cpu->a = 0x00;
}

void out_d8(struct i8080 *cpu)
{
    uint8_t port = fetch_8(cpu);

    if (!port) {
        cpu->test_finished = 1;
    } else if (port == 1) {
        if (cpu->c == 2) {
            printf("%c", cpu->e);
        } else if (cpu->c == 9) {
            char *str = &cpu->mem[cpu->de];
            while (*str != '$')
                printf("%c", *str++);
        }
    }
}

void ei(struct i8080 *cpu)
{
    //TODO: EI()
}

void di(struct i8080 *cpu)
{
    // TODO: di
}

void hlt(struct i8080 *cpu)
{
    // TODO: hlt
}

void execute_instruction(struct i8080 *cpu)
{
    uint16_t addr;
    char *str;

    //debug_output(cpu);
    cpu->opcode = mem_read(cpu, cpu->pc++);
    switch (cpu->opcode) {
    case 0x00:                                              break;
    case 0x01: cpu->bc = fetch_16(cpu);                     break;
    case 0x02: mem_write(cpu, cpu->bc, cpu->a);             break;
    case 0x03: cpu->bc = cpu->bc + 1;                       break;
    case 0x04: cpu->b = inr(cpu, cpu->b);                   break;
    case 0x05: cpu->b = dcr(cpu, cpu->b);                   break;
    case 0x06: cpu->b = fetch_8(cpu);                       break;
    case 0x07: rlc(cpu);                                    break;
    case 0x08:                                              break;
    case 0x09: dad_r16(cpu, cpu->bc);                       break;
    case 0x0a: cpu->a = mem_read(cpu, cpu->bc);             break;
    case 0x0b: cpu->bc = cpu->bc - 1;                       break;
    case 0x0c: cpu->c = inr(cpu, cpu->c);                   break;
    case 0x0d: cpu->c = dcr(cpu, cpu->c);                   break;
    case 0x0e: cpu->c = fetch_8(cpu);                       break;
    case 0x0f: rrc(cpu);                                    break;
    case 0x10:                                              break;
    case 0x11: cpu->de = fetch_16(cpu);                     break;
    case 0x12: mem_write(cpu, cpu->de, cpu->a);             break;
    case 0x13: cpu->de = cpu->de + 1;                       break;
    case 0x14: cpu->d = inr(cpu, cpu->d);                   break;
    case 0x15: cpu->d = dcr(cpu, cpu->d);                   break;
    case 0x16: cpu->d = fetch_8(cpu);                       break;
    case 0x17: ral(cpu);                                    break;
    case 0x18:                                              break;
    case 0x19: dad_r16(cpu, cpu->de);                       break;
    case 0x1a: cpu->a = mem_read(cpu, cpu->de);             break;
    case 0x1b: cpu->de = cpu->de - 1;             break;
    case 0x1c: cpu->e = inr(cpu, cpu->e);                   break;
    case 0x1d: cpu->e = dcr(cpu, cpu->e);                   break;
    case 0x1e: cpu->e = fetch_8(cpu);                       break;
    case 0x1f: rar(cpu);                                    break;
    case 0x20:                                              break;
    case 0x21: cpu->hl = fetch_16(cpu);                     break;
    case 0x22: shld_a16(cpu, fetch_16(cpu));                break;
    case 0x23: cpu->hl = cpu->hl + 1;                       break;
    case 0x24: cpu->h = inr(cpu, cpu->h);                   break;
    case 0x25: cpu->h = dcr(cpu, cpu->h);                   break;
    case 0x26: cpu->h = fetch_8(cpu);                       break;
    case 0x27: daa(cpu);                                    break;
    case 0x28:                                              break;
    case 0x29: dad_r16(cpu, cpu->hl);                       break;
    case 0x2a: lhld_a16(cpu, fetch_16(cpu));                break;
    case 0x2b: cpu->hl = cpu->hl - 1;                       break;
    case 0x2c: cpu->l = inr(cpu, cpu->l);                   break;
    case 0x2d: cpu->l = dcr(cpu, cpu->l);                   break;
    case 0x2e: cpu->l = fetch_8(cpu);                       break;
    case 0x2f: cpu->a = ~cpu->a;                            break;
    case 0x30:                                              break;
    case 0x31: cpu->sp = fetch_16(cpu);                     break;
    case 0x32: mem_write(cpu, fetch_16(cpu), cpu->a);       break;
    case 0x33: cpu->sp = cpu->sp + 1;                       break;
    case 0x34: inr_indirect_hl(cpu);                        break;
    case 0x35: dcr_indirect_hl(cpu);                        break;
    case 0x36: mem_write(cpu, cpu->hl, fetch_8(cpu));       break;
    case 0x37: cpu->CARRY = 1;                              break;
    case 0x38:                                              break;
    case 0x39: dad_r16(cpu, cpu->sp);                       break;
    case 0x3a: cpu->a = mem_read(cpu, fetch_16(cpu));       break;
    case 0x3b: cpu->sp = cpu->sp - 1;                       break;
    case 0x3c: cpu->a = inr(cpu, cpu->a);                   break;
    case 0x3d: cpu->a = dcr(cpu, cpu->a);                   break;
    case 0x3e: cpu->a = fetch_8(cpu);                       break;
    case 0x3f: cpu->CARRY = !cpu->CARRY;                    break;
    case 0x40: cpu->b = cpu->b;                             break;
    case 0x41: cpu->b = cpu->c;                             break;
    case 0x42: cpu->b = cpu->d;                             break;
    case 0x43: cpu->b = cpu->e;                             break;
    case 0x44: cpu->b = cpu->h;                             break;
    case 0x45: cpu->b = cpu->l;                             break;
    case 0x46: cpu->b = mem_read(cpu, cpu->hl);             break;
    case 0x47: cpu->b = cpu->a;                             break;
    case 0x48: cpu->c = cpu->b;                             break;
    case 0x49: cpu->c = cpu->c;                             break;
    case 0x4a: cpu->c = cpu->d;                             break;
    case 0x4b: cpu->c = cpu->e;                             break;
    case 0x4c: cpu->c = cpu->h;                             break;
    case 0x4d: cpu->c = cpu->l;                             break;
    case 0x4e: cpu->c = mem_read(cpu, cpu->hl);             break;
    case 0x4f: cpu->c = cpu->a;                             break;
    case 0x50: cpu->d = cpu->b;                             break;
    case 0x51: cpu->d = cpu->c;                             break;
    case 0x52: cpu->d = cpu->d;                             break;
    case 0x53: cpu->d = cpu->e;                             break;
    case 0x54: cpu->d = cpu->h;                             break;
    case 0x55: cpu->d = cpu->l;                             break;
    case 0x56: cpu->d = mem_read(cpu, cpu->hl);             break;
    case 0x57: cpu->d = cpu->a;                             break;
    case 0x58: cpu->e = cpu->b;                             break;
    case 0x59: cpu->e = cpu->c;                             break;
    case 0x5a: cpu->e = cpu->d;                             break;
    case 0x5b: cpu->e = cpu->e;                             break;
    case 0x5c: cpu->e = cpu->h;                             break;
    case 0x5d: cpu->e = cpu->l;                             break;
    case 0x5e: cpu->e = mem_read(cpu, cpu->hl);             break;
    case 0x5f: cpu->e = cpu->a;                             break;
    case 0x60: cpu->h = cpu->b;                             break;
    case 0x61: cpu->h = cpu->c;                             break;
    case 0x62: cpu->h = cpu->d;                             break;
    case 0x63: cpu->h = cpu->e;                             break;
    case 0x64: cpu->h = cpu->h;                             break;
    case 0x65: cpu->h = cpu->l;                             break;
    case 0x66: cpu->h = mem_read(cpu, cpu->hl);             break;
    case 0x67: cpu->h = cpu->a;                             break;
    case 0x68: cpu->l = cpu->b;                             break;
    case 0x69: cpu->l = cpu->c;                             break;
    case 0x6a: cpu->l = cpu->d;                             break;
    case 0x6b: cpu->l = cpu->e;                             break;
    case 0x6c: cpu->l = cpu->h;                             break;
    case 0x6d: cpu->l = cpu->l;                             break;
    case 0x6e: cpu->l = mem_read(cpu, cpu->hl);             break;
    case 0x6f: cpu->l = cpu->a;                             break;
    case 0x70: mem_write(cpu, cpu->hl, cpu->b);             break;
    case 0x71: mem_write(cpu, cpu->hl, cpu->c);             break;
    case 0x72: mem_write(cpu, cpu->hl, cpu->d);             break;
    case 0x73: mem_write(cpu, cpu->hl, cpu->e);             break;
    case 0x74: mem_write(cpu, cpu->hl, cpu->h);             break;
    case 0x75: mem_write(cpu, cpu->hl, cpu->l);             break;
    case 0x76: hlt(cpu);                                    break;
    case 0x77: mem_write(cpu, cpu->hl, cpu->a);             break;
    case 0x78: cpu->a = cpu->b;                             break;
    case 0x79: cpu->a = cpu->c;                             break;
    case 0x7a: cpu->a = cpu->d;                             break;
    case 0x7b: cpu->a = cpu->e;                             break;
    case 0x7c: cpu->a = cpu->h;                             break;
    case 0x7d: cpu->a = cpu->l;                             break;
    case 0x7e: cpu->a = mem_read(cpu, cpu->hl);             break;
    case 0x7f: cpu->a = cpu->a;                             break;
    case 0x80: add(cpu, cpu->b, 0);                         break;
    case 0x81: add(cpu, cpu->c, 0);                         break;
    case 0x82: add(cpu, cpu->d, 0);                         break;
    case 0x83: add(cpu, cpu->e, 0);                         break;
    case 0x84: add(cpu, cpu->h, 0);                         break;
    case 0x85: add(cpu, cpu->l, 0);                         break;
    case 0x86: add(cpu, mem_read(cpu, cpu->hl), 0);         break;
    case 0x87: add(cpu, cpu->a, 0);                         break;
    case 0x88: add(cpu, cpu->b, cpu->CARRY);                break;
    case 0x89: add(cpu, cpu->c, cpu->CARRY);                break;
    case 0x8a: add(cpu, cpu->d, cpu->CARRY);                break;
    case 0x8b: add(cpu, cpu->e, cpu->CARRY);                break;
    case 0x8c: add(cpu, cpu->h, cpu->CARRY);                break;
    case 0x8d: add(cpu, cpu->l, cpu->CARRY);                break;
    case 0x8e: add(cpu, mem_read(cpu, cpu->hl), cpu->CARRY);break;
    case 0x8f: add(cpu, cpu->a, cpu->CARRY);                break;
    case 0x90: sub(cpu, cpu->b, 0);                         break;
    case 0x91: sub(cpu, cpu->c, 0);                         break;
    case 0x92: sub(cpu, cpu->d, 0);                         break;
    case 0x93: sub(cpu, cpu->e, 0);                         break;
    case 0x94: sub(cpu, cpu->h, 0);                         break;
    case 0x95: sub(cpu, cpu->l, 0);                         break;
    case 0x96: sub(cpu, mem_read(cpu, cpu->hl), 0);         break;
    case 0x97: sub(cpu, cpu->a, 0);                         break;
    case 0x98: sub(cpu, cpu->b, cpu->CARRY);                         break;
    case 0x99: sub(cpu, cpu->c, cpu->CARRY);                         break;
    case 0x9a: sub(cpu, cpu->d, cpu->CARRY);                         break;
    case 0x9b: sub(cpu, cpu->e, cpu->CARRY);                         break;
    case 0x9c: sub(cpu, cpu->h, cpu->CARRY);                         break;
    case 0x9d: sub(cpu, cpu->l, cpu->CARRY);                         break;
    case 0x9e: sub(cpu, mem_read(cpu, cpu->hl), cpu->CARRY);         break;
    case 0x9f: sub(cpu, cpu->a, cpu->CARRY);                         break;
    case 0xa0: and(cpu, cpu->b);                            break;
    case 0xa1: and(cpu, cpu->c);                            break;
    case 0xa2: and(cpu, cpu->d);                            break;
    case 0xa3: and(cpu, cpu->e);                            break;
    case 0xa4: and(cpu, cpu->h);                            break;
    case 0xa5: and(cpu, cpu->l);                            break;
    case 0xa6: and(cpu, mem_read(cpu, cpu->hl));            break;
    case 0xa7: and(cpu, cpu->a);                            break;
    case 0xa8: xor(cpu, cpu->b);                            break;
    case 0xa9: xor(cpu, cpu->c);                            break;
    case 0xaa: xor(cpu, cpu->d);                            break;
    case 0xab: xor(cpu, cpu->e);                            break;
    case 0xac: xor(cpu, cpu->h);                            break;
    case 0xad: xor(cpu, cpu->l);                            break;
    case 0xae: xor(cpu, mem_read(cpu, cpu->hl));            break;
    case 0xaf: xor(cpu, cpu->a);                            break;
    case 0xb0: or(cpu, cpu->b);                             break;
    case 0xb1: or(cpu, cpu->c);                             break;
    case 0xb2: or(cpu, cpu->d);                             break;
    case 0xb3: or(cpu, cpu->e);                             break;
    case 0xb4: or(cpu, cpu->h);                             break;
    case 0xb5: or(cpu, cpu->l);                             break;
    case 0xb6: or(cpu, mem_read(cpu, cpu->hl));             break;
    case 0xb7: or(cpu, cpu->a);                             break;
    case 0xb8: cmp(cpu, cpu->b);                            break;
    case 0xb9: cmp(cpu, cpu->c);                            break;
    case 0xba: cmp(cpu, cpu->d);                            break;
    case 0xbb: cmp(cpu, cpu->e);                            break;
    case 0xbc: cmp(cpu, cpu->h);                            break;
    case 0xbd: cmp(cpu, cpu->l);                            break;
    case 0xbe: cmp(cpu, mem_read(cpu, cpu->hl));            break;
    case 0xbf: cmp(cpu, cpu->a);                            break;
    case 0xc0: ret(cpu, !cpu->ZERO);                        break;
    case 0xc1: cpu->bc = stack_pop_16(cpu);                 break;
    case 0xc2: jmp(cpu, fetch_16(cpu), !cpu->ZERO);         break;
    case 0xc3: jmp(cpu, fetch_16(cpu), 1);                  break;
    case 0xc4: call(cpu, fetch_16(cpu), !cpu->ZERO);        break;
    case 0xc5: stack_push_16(cpu, cpu->bc);                 break;
    case 0xc6: add(cpu, fetch_8(cpu), 0);                   break;
    case 0xc7: rst_n(cpu, 0);                               break;
    case 0xc8: ret(cpu, cpu->ZERO);                         break;
    case 0xc9: ret(cpu, 1);                                 break;
    case 0xca: jmp(cpu, fetch_16(cpu), cpu->ZERO);          break;
    case 0xcb:                                              break;
    case 0xcc: call(cpu, fetch_16(cpu), cpu->ZERO);         break;
    case 0xcd: call(cpu, fetch_16(cpu), 1);                 break;
    case 0xce: add(cpu, fetch_8(cpu), cpu->CARRY);          break;
    case 0xcf: rst_n(cpu, 1);                               break;
    case 0xd0: ret(cpu, !cpu->CARRY);                       break;
    case 0xd1: cpu->de = stack_pop_16(cpu);                 break;
    case 0xd2: jmp(cpu, fetch_16(cpu), !cpu->CARRY);        break;
    case 0xd3: out_d8(cpu);                                 break;
    case 0xd4: call(cpu, fetch_16(cpu), !cpu->CARRY);       break;
    case 0xd5: stack_push_16(cpu, cpu->de);                 break;
    case 0xd6: sub(cpu, fetch_8(cpu), 0);                   break;
    case 0xd7: rst_n(cpu, 2);                               break;
    case 0xd8: ret(cpu, cpu->CARRY);                        break;
    case 0xd9:                                              break;
    case 0xda: jmp(cpu, fetch_16(cpu), cpu->CARRY);         break;
    case 0xdb: in_d8(cpu);                                  break;
    case 0xdc: call(cpu, fetch_16(cpu), cpu->CARRY);        break;
    case 0xdd:                                              break;
    case 0xde: sub(cpu, fetch_8(cpu), cpu->CARRY);          break;
    case 0xdf: rst_n(cpu, 3);                               break;
    case 0xe0: ret(cpu, !cpu->PARITY);                      break;
    case 0xe1: cpu->hl = stack_pop_16(cpu);                 break;
    case 0xe2: jmp(cpu, fetch_16(cpu), !cpu->PARITY);       break;
    case 0xe3: xthl(cpu);                                   break;
    case 0xe4: call(cpu, fetch_16(cpu), !cpu->PARITY);      break;
    case 0xe5: stack_push_16(cpu, cpu->hl);                 break;
    case 0xe6: and(cpu, fetch_8(cpu));                      break;
    case 0xe7: rst_n(cpu, 4);                               break;
    case 0xe8: ret(cpu, cpu->PARITY);                       break;
    case 0xe9: cpu->pc = cpu->hl;                           break;
    case 0xea: jmp(cpu, fetch_16(cpu), cpu->PARITY);        break;
    case 0xeb: xchg(cpu);                                   break;
    case 0xec: call(cpu, fetch_16(cpu), cpu->PARITY);       break;
    case 0xed:                                              break;
    case 0xee: xor(cpu, fetch_8(cpu));                      break;
    case 0xef: rst_n(cpu, 5);                               break;
    case 0xf0: ret(cpu, !cpu->SIGN);                        break;
    case 0xf1: cpu->af = stack_pop_16(cpu); cpu->af &= 0xffd5; cpu->af |= 0x02;break;
    case 0xf2: jmp(cpu, fetch_16(cpu), !cpu->SIGN);         break;
    case 0xf3: di(cpu);                                     break;
    case 0xf4: call(cpu, fetch_16(cpu), !cpu->SIGN);        break;
    case 0xf5: stack_push_16(cpu, cpu->af);                 break;
    case 0xf6: or(cpu, fetch_8(cpu));                       break;
    case 0xf7: rst_n(cpu, 6);                               break;
    case 0xf8: ret(cpu, cpu->SIGN);                         break;
    case 0xf9: cpu->sp = cpu->hl;                           break;
    case 0xfa: jmp(cpu, fetch_16(cpu), cpu->SIGN);          break;
    case 0xfb: ei(cpu);                                     break;
    case 0xfc: call(cpu, fetch_16(cpu), cpu->SIGN);         break;
    case 0xfd:                                              break;
    case 0xfe: cmp(cpu, fetch_8(cpu));                      break;
    case 0xff: rst_n(cpu, 7);                               break;
    default:
        fprintf(stderr, "opcode 0x%02x hasn't implemented yet\n", cpu->opcode);
        exit(EXIT_FAILURE);
    }
    cpu->cycles += instructions_cycles[cpu->opcode];
}

void cpu_init(struct i8080 *cpu)
{
    cpu->pc = 0x100;
    cpu->cycles = 0;
    cpu->af = 0x0002;
    cpu->bc = 0;
    cpu->de = 0;
    cpu->hl = 0;
    cpu->sp = 0x0000;

}