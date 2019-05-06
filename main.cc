#include <cstdio>
#include <cstdlib>
#include <iostream>

#include <linux/kernel.h>
#include <unistd.h>
#include <linux/aio_abi.h>
#include <sys/utsname.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "mem.h"
#include "elf.h"
#include "functions.h"
#include "sparse_array_3.h"

using namespace std;

#define NUMREG 32

A3 *memory = new A3();

int main(int argc, const char * argv[]) {
    bool debug = true;
    uint64_t reg[NUMREG];
    uint64_t sys_reg[65536];
    unsigned __int128 simd_reg[NUMREG];
    // initializing registers
    for ( int i = 0; i < 32; i++ ) {
        reg[i] = 0;
        simd_reg[i] = 0;
    }
    for ( int i = 0; i < 65536; i++ ) {
        sys_reg[i] = 0;
    }
    const char* fileName = argv[1];
    uint64_t entry = loadElf(fileName); // entry = init value for PC (program counter)
    uint64_t pcLocal = entry;
    uint8_t nzcvLocal = 0;
    // have ALL the cores been terminated yet?
    bool allTerminated = false;
    // syscall variables
    while ( !allTerminated ) {
        // whether to change the PC
        bool pcChange = false;
        uint32_t instr_int = memory_get_32(pcLocal);
        if ( debug ) {
            printf("instruction %x at address %lx ", instr_int, pcLocal);
            printf("X30: %lx", reg[30]);
            printf(" X0: %lx\n", reg[0]);
            if ( pcLocal == 0x408d58 || (pcLocal >= 0x405788 && pcLocal <= 0x405a8c) ) {
                printf("Abort encountered. Stopping execution.\n");
                exit(0);
            }
        }
        // PC-specific debug
        if ( pcLocal == 0x41187c || pcLocal == 0x411884 ) {
            printf(" X2: %lx\n", reg[2]);
        }
        if ( is_instruction(instr_int, 0xfa400800) ) {
            // CCMP immediate 64
            uint64_t n = extract(instr_int, 9, 5);
            uint64_t imm5 = extract(instr_int, 20, 16);
            uint8_t flags = extract(instr_int, 3, 0);
            // uint8_t cond = extract32(instr_int, 15, 13);
            // uint8_t condLast = extract_single32(instr_int, 12);
            uint8_t fullcond = extract32(instr_int, 15, 12);
            // bool condHolds = false;
            // if ( cond == 0 ) {
            //     condHolds = (extract_single32(fullcond, 2) == 1); // Z == 1 <-> EQ or NE
            // } else if ( cond == 1 ) {
            //     condHolds = (extract_single32(fullcond, 1) == 1); // C == 1 <-> CS or CC
            // } else if ( cond == 2 ) {
            //     condHolds = (extract_single32(fullcond, 3) == 1); // N == 1 <-> MI or PL
            // } else if ( cond == 3 ) {
            //     condHolds = (extract_single32(fullcond, 0) == 1); // V == 1 <-> VS or VC
            // } else if ( cond == 4 ) {
            //     condHolds = (extract_single32(fullcond, 1) == 1 && extract_single32(fullcond, 2) == 0); // C == 1 && Z == 0 <-> HI or LS
            // } else if ( cond == 5 ) {
            //     condHolds = (extract_single32(fullcond, 3) == extract_single32(fullcond, 0)); // N == V <-> GE or LT
            // } else if ( cond == 6 ) {
            //     condHolds = (extract_single32(fullcond, 3) == extract_single32(fullcond, 0) && extract_single32(fullcond, 2) == 0); // N == V && Z == 0 <-> GT or LE
            // } else {
            //     condHolds = true; // AL
            // }
            // if ( extract_single32(condLast, 0) == 1 && cond != 7 ) {
            //     condHolds = !condHolds;
            // }
            bool condHolds = conditionHolds(fullcond, fullcond);
            uint64_t operand1 = reg[n];
            uint64_t operand2 = 0;
            uint64_t result_throwaway = 0;
            uint8_t flags_result = 0;
            if ( condHolds ) {
                operand2 = ~imm5;
                add_with_carry64(operand1, operand2, 1, &result_throwaway, &flags_result);
                nzcvLocal = flags_result;
            } else {
                nzcvLocal = flags;
            }
        } else if( is_instruction(instr_int, 0xfa400000) ){
            //ccmp register 64
            //CCMP REGISTER 64
            uint8_t flags = extract(instr_int, 4, 0);
            uint64_t n = extract(instr_int, 9, 5);
            uint64_t cond = extract(instr_int, 15, 12);
            uint64_t m = extract(instr_int, 20, 16);

            uint64_t operand1 = 0;
            uint64_t operand2 = 0;

            if(n != 31){
              operand1 = reg[n];
            }
            if(m != 31){
              operand2 = reg[m];
            }
            uint64_t result_throwaway = 0;
            uint8_t flags_result = 0;
            if(conditionHolds(cond, cond)){
              operand2 = ~operand2;
              add_with_carry64(operand1, operand2, 1, &result_throwaway, &flags_result);
              flags = flags_result;
            }

            nzcvLocal = flags;
        }
        else if ( is_instruction(instr_int, 0xf9400000) ) {
            // LDR immediate unsigned offset 64
            // cout << "LDR immediate unsigned offset 64" << endl;
            uint64_t imm12 = extract(instr_int, 21, 10);
            uint64_t t = extract(instr_int, 4, 0);
            uint64_t n = extract(instr_int, 9, 5);
            uint64_t offset = imm12 << 3;
            uint64_t address = reg[n];
            address = address + offset;
            uint64_t data = memory_get_64(address);
            if ( t != 31 ) {
                reg[t] = data;
            }
        } else if ( is_instruction(instr_int, 0xf9000000) ) {
            // STR immediate unsigned offset 64
            // cout << "STR immediate unsigned offset 64" << endl;
            uint8_t size = extract32(instr_int, 31, 30);
            uint64_t t = extract(instr_int, 4, 0);
            uint64_t n = extract(instr_int, 9, 5);
            uint64_t imm = extract(instr_int, 21, 10);
            uint64_t offset = logical_shift_left(imm, size);
            uint64_t address = reg[n];
            address = address + offset;
            uint64_t data = reg[t];
            memory_set_64(address, data);
        } else if ( is_instruction(instr_int, 0xf8400c00) ) {
            // LDR immediate pre-index 64
            // cout << "LDR immediate pre-index 64" << endl;
            uint64_t imm9 = extract(instr_int, 20, 12);
            uint64_t t = extract(instr_int, 4, 0);
            uint64_t n = extract(instr_int, 9, 5);
            uint64_t offset = sign_extend_64(imm9, 9);
            uint64_t address = 0;
            address = reg[n];
            address = address + offset;
            uint64_t data = memory_get_64(address);
            if ( t != 31 ) {
                reg[t] = data;
            }
            reg[n] = address;
        } else if ( is_instruction(instr_int, 0xf8400400) ) {
            // LDR immediate post-index 64
            // cout << "LDR immediate post-index 64" << endl;
            uint64_t imm9 = extract(instr_int, 20, 12);
            uint64_t t = extract(instr_int, 4, 0);
            uint64_t n = extract(instr_int, 9, 5);
            uint64_t offset = sign_extend_64(imm9, 9);
            uint64_t address = 0;
            address = reg[n];
            uint64_t data = memory_get_64(address);
            if ( t != 31 ) {
                reg[t] = data;
            }
            address = address + offset;
            reg[n] = address;
        } else if ( is_instruction(instr_int, 0xf8000c00) ) {
            // STR immediate pre-index 64
            // cout << "STR immediate pre-index 64" << endl;
            uint64_t t = extract(instr_int, 4, 0);
            uint64_t n = extract(instr_int, 9, 5);
            uint64_t imm = extract(instr_int, 20, 12);
            uint64_t offset = sign_extend_64(imm, 9);
            uint64_t address = reg[n];
            address = address + offset;
            uint64_t data = reg[t];
            memory_set_64(address, data);
            reg[n] = address;
        } else if ( is_instruction(instr_int, 0xF8400000) ) {
            // LDRU 64
            uint64_t imm9 = extract(instr_int, 20, 12);
            uint64_t t = extract(instr_int, 4, 0);
            uint64_t n = extract(instr_int, 9, 5);
            uint64_t offset = sign_extend_32(imm9, 9);
            uint64_t address = reg[n];
            address = address + offset;
            uint64_t data = memory_get_64(address);
            if ( t != 31 ) {
                reg[t] = data;
            }
        } else if( is_instruction(instr_int, 0xf8200800) ) {
          //str shifted register 64
          //STR SHIFTED REGISTER 64
          uint64_t t = extract(instr_int, 4, 0);
          uint64_t n = extract(instr_int, 9, 5);
          uint64_t extend_type = extract(instr_int, 15, 13);
          uint8_t S = extract(instr_int, 12, 12);


          uint64_t m = extract(instr_int, 20, 16);
          uint64_t scale = 3;
          uint64_t shift = S == 1 ? scale : 0;


          uint64_t offset = extend_reg64(m, extend_type, shift, reg);
          uint64_t address = reg[n];
          uint64_t data;
          address += offset;
          data = 0;
          if (t != 31){
            data = reg[t];
          }
          memory_set_64(address, data);

        }
        else if ( is_instruction(instr_int, 0xf8000400) ) {
            // STR immediate post-index 64
            // cout << "STR immediate post-index 64" << endl;
            uint64_t t = extract(instr_int, 4, 0);
            uint64_t n = extract(instr_int, 9, 5);
            uint64_t imm = extract(instr_int, 20, 12);
            uint64_t offset = sign_extend_64(imm, 9);
            uint64_t address = reg[n];
            uint64_t data = reg[t];
            memory_set_64(address, data);
            address = address + offset;
            reg[n] = address;
        } else if( is_instruction(instr_int, 0xF2000000) ){
            //ANDS immediate 64
            uint64_t d = extract(instr_int, 4, 0);
            uint64_t n = extract(instr_int, 9, 5);
            uint64_t imms = extract(instr_int, 15, 10);
            uint64_t immr = extract(instr_int, 21, 16);
            uint64_t N = extract(instr_int, 22, 22);
            uint64_t nah = 0;
            uint64_t imm = 0;
            decode_bit_masks(N, imms, immr, true, &imm, &nah);

            uint64_t operand1 = n == 31 ? 0 : reg[n];
            uint64_t result = operand1 & imm;
            uint32_t lastBit = result >> 63;
            nzcvLocal = (lastBit << 3) | ((result==0) <<2);
            if(d != 31){
              reg[d] = result;
            }
        } else if ( is_instruction(instr_int, 0xf1000000) ) {
            // SUBS immediate 64
            // cout << "SUBS immediate 64" << endl;
            uint64_t d = extract(instr_int, 4, 0);
            uint64_t n = extract(instr_int, 9, 5);
            uint64_t imm = extract(instr_int, 21, 10);
            uint64_t shift = extract_single32(instr_int, 22);
            if ( shift == 1 ) {
                imm = imm << 12;
            }
            uint64_t result = 0;
            uint64_t operand1 = reg[n];
            uint64_t operand2 = ~imm;
            uint8_t nzcv_temp = 0;
            add_with_carry64(operand1, operand2, 1, &result, &nzcv_temp);
            nzcvLocal = nzcv_temp;
            if ( d != 31 ) {
                reg[d] = result;
            }
         } else if ( is_instruction(instr_int, 0xeb000000) ) {
           // SUBS shifted register 64
           uint64_t Rd = extract(instr_int, 4, 0);
           uint64_t Rn = extract(instr_int, 9, 5);
           uint64_t imm6 = extract(instr_int, 15, 10);
           uint64_t Rm = extract(instr_int, 20, 16);
           uint64_t shift = extract(instr_int, 23, 22);

           uint64_t result = 0;
           uint64_t operand1 = Rn == 31 ? 0 : reg[Rn];
           uint64_t operand2 = shift_reg64(Rm == 31 ? 0 : reg[Rm], shift, imm6);

           uint8_t nzcv = 0;

           operand2 = ~(operand2);

           add_with_carry64(operand1, operand2, (uint8_t) 1, &result, &nzcv);

           nzcvLocal = nzcv;

           if ( Rd != 31 ) {
              reg[Rd] = result;
           }

         } else if ( is_instruction(instr_int, 0xea200000) ){
            //bics 64 bit
            //BICS 64
            uint64_t d = extract(instr_int, 4, 0);
            uint64_t n = extract(instr_int, 9, 5);
            uint64_t m = extract(instr_int, 20, 16);

            uint64_t shift_amount = extract(instr_int, 10, 15);
            uint64_t shift_type = extract(instr_int, 23, 22);

            //uint64_t datasize = 32;

            uint64_t operand1 = 0;
            if(n != 31){
                operand1 = reg[n];
            }

            uint64_t operand2 = shift_reg64(m == 31 ? 0 : reg[m], shift_type, shift_amount);
            operand2 = ~operand2;
            uint64_t result = operand1 & operand2;
            nzcvLocal = (extract_single(result, 63) << 3) | ((result == 0) << 2) | 0;

            if(d != 31){
                reg[d] = result;
            }
        } else if( is_instruction(instr_int, 0xdac01000) ){
          //clz 64 bit
          //CLZ 64 bit
          uint64_t d = extract(instr_int, 4, 0);
          uint64_t n = extract(instr_int, 9, 5);
          uint64_t result = 0;
          uint64_t operand1 = 0;
          if(n != 31){
            operand1 = reg[n];
          }
          result = count_leading_zero_bits64(operand1);
          if(d != 31){
            reg[d] = result;
          }
        } else if ( is_instruction(instr_int, 0xdac00800) ) {
            // REV
            uint64_t n = extract(instr_int, 9, 5);
            uint64_t d = extract(instr_int, 4, 0);
            if ( n == 31 && d != 31 ) {
                reg[d] = 0;
            } else if ( n != 31 && d != 31 ) {
                uint64_t orig = reg[n];
                uint64_t rev = 0;
                for ( int i = 0; i < 64; i++ ) {
                    rev |= ((orig >> i) & 1) << (63 - i);
                }
                reg[d] = rev;
            }
        } else if( is_instruction(instr_int, 0xda800000) ){
          //csinv 64 bit
          //CSINV 64 bit
          uint64_t d = extract(instr_int, 4, 0);
          uint64_t n = extract(instr_int, 9, 5);
          uint64_t m = extract(instr_int, 20, 16);
          uint8_t cond = extract(instr_int, 15, 12);
          uint64_t result = 0;
          uint64_t operand1 = 0;
          if(n != 31){
            operand1 = reg[n];
          }
          uint64_t operand2 = 0;
          if(m != 31){
            operand2 = reg[m];
          }
          if(conditionHolds(cond, nzcvLocal)){
            result = operand1;
          }
          else{
            result = ~operand2;
          }
          if(d != 31){
            reg[d] = result;
          }
        }
        else if ( is_instruction(instr_int, 0xd65f0000) ) {
            // RET
            // cout << "RET" << endl;
            uint64_t n = extract(instr_int, 9, 5);
            pcChange = true;
            if ( n != 31 ) {
                pcLocal = reg[n];
            } else {
                pcLocal = 0;
            }
        } else if ( is_instruction(instr_int, 0xd63f0000) ){
          //blr
          //BLR

          uint64_t n = extract(instr_int, 9, 5);
          printf("%ld", n);
          uint64_t target = 0;
          if(n != 31){
            target = reg[n];
          }
          reg[30] = pcLocal + 4;
          pcLocal = target;
          pcChange = true;
        }
        else if ( is_instruction(instr_int, 0xd61f0000) ) {
            // BR
            uint64_t n = extract(instr_int, 9, 5);
            pcChange = true;
            pcLocal = n == 31 ? 0 : reg[n];
        } else if ( is_instruction(instr_int, 0xd5300000) ) {
            // MRS
            uint64_t t = extract(instr_int, 4, 0);
            uint64_t sys_op0 = 2 + extract_single(instr_int, 19);
            uint64_t sys_op1 = extract(instr_int, 18, 16);
            uint64_t sys_op2 = extract(instr_int, 7, 5);
            uint64_t sys_crn = extract(instr_int, 15, 12);
            uint64_t sys_crm = extract(instr_int, 11, 8);
            uint64_t sys_regnum = (sys_op0 << 14) | (sys_crn << 10) | (sys_op1 << 7) | (sys_op2 << 4) | sys_crm;
            // sys_op0 + sys_crn + sys_op1 + sys_op2 + sys_crm
            if ( t != 31 ) {
                reg[t] = sys_reg[sys_regnum];
            }
        } else if ( is_instruction(instr_int, 0xd503201f) ) {
            // NOP
        } else if ( is_instruction(instr_int, 0xd4000001) ) {
            // SVC
            // Supervisor Call
            // uint64_t imm16 = extract(instr_int, 20, 5);
            // if ( imm16 == 0x0 ) {
            //     // I/O setup
            //     // io_setup(nr_events, ctx_idp);
            //     printf("X0: %ld\nX1: %ld\nX2: %ld\nX3: %ld\nX4: %ld\nX5: %ld\n", reg[0], reg[1], reg[2], reg[3], reg[4], reg[5]);
            //     reg[0] = 10000000;
            //     printf("io_setup\n");
            // } else {
            //     printf("\n\n%ld\n\n\n", imm16);
            // }
            // syscall(imm16);
            printf("X8: %lx\n", reg[8]);
            printf("X0: %ld\nX1: %ld\nX2: %ld\nX3: %ld\nX4: %ld\nX5: %ld\n", reg[0], reg[1], reg[2], reg[3], reg[4], reg[5]);
            uint64_t syscall_num = reg[8];
            if ( syscall_num == 0xa0 ) {
                // uname
                uint64_t result = uname((struct utsname*) reg[0]);
                printf("%lx\n", result);
                reg[0] = result;
                // reg[0] = 0;
            } else if ( syscall_num == 0x38 ) {
                // openat
                // uint64_t result = openat((int) reg[0], (const char *) reg[1], (int) reg[2]); //, (mode_t) reg[3]
                uint64_t result = 1; // arbitrarily assign the file descriptor as 1 for now
                printf("%lx\n", result);
                int count = 0;
                while ( memory_get(reg[1] + count) != 0 ) {
                    printf("%c", memory_get(reg[1] + count++));
                }
                printf("\n");
                reg[0] = result;
            } else if ( syscall_num == 0x40 ) {
                // write
                reg[0] = write((int) reg[0], (const void *) reg[1], (size_t) reg[2]);
            } else if ( syscall_num == 0x39 ) {
                // close
                // returns 0 on success
                reg[0] = 0;
            } else if ( syscall_num == 0x3f ) {
                // read
                // give it the number of bytes that it requested
                if ( reg[0] == 1 ) {
                    char linuxversion[] = "4.9.80-v7+";
                    int count = 0;
                    while ( count < 9 ) {
                        memory_set(reg[1] + count, linuxversion[count]);
                        printf("%c", memory_get(reg[1] + count));
                        count++;
                        // printf("%c", memory_get(reg[1] + count++));
                    }
                }
                reg[0] = reg[2];
            }
            else if ( syscall_num == 0xd6 ) {
                // brk
                reg[0] = 0;
            } else if ( syscall_num == 0xae || syscall_num == 0xaf || syscall_num == 0xb0 || syscall_num == 0xb1 ) {
                // getuid32 (ae) and geteuid32 (af)
                // getgid32 (b0) and getegid32 (b1)
                reg[0] = 0;
            } else if ( syscall_num == 0x4e ) {
                // readlinkat
                int count = 0;
                printf("pathname: ");
                while ( memory_get(reg[1] + count) != 0 ) {
                    printf("%c", memory_get(reg[1] + count++));
                }
                printf("\n");
                count = 0;
                printf("buf: ");
                while ( memory_get(reg[2] + count) != 0 ) {
                    printf("%c", memory_get(reg[1] + count++));
                }
                printf("\n");
                FILE* readfile = fopen(fileName, "r");
                fseek(readfile, 0, SEEK_END);
                // reg[0] = ftell(readfile); // get size of file
            }
            // else if ( syscall_num == 0x0 ) {
            //     //
            //     reg[0] = 0;
            // }

        } else if (is_instruction(instr_int, 0xd4200000)) {
            // BRK
            exit(0);
        } else if ( is_instruction(instr_int, 0xd3000000) ) {
            // UBFM 64
            // cout << "UBFM 64" << endl;
            uint64_t N = extract_single(instr_int, 22);
            uint64_t d = extract(instr_int, 4, 0);
            uint64_t n = extract(instr_int, 9, 5);
            uint64_t immr = extract(instr_int, 21, 16);
            uint64_t imms = extract(instr_int, 15, 10);
            uint64_t wmask = 0;
            uint64_t tmask = 0;
            decode_bit_masks(N, imms, immr, false, &wmask, &tmask);
            uint64_t src = 0;
            if ( n != 31 ) {
                src = reg[n];
            }
            uint64_t bot = rotate_right(src, immr) & wmask;
            if ( d != 31 ) {
                reg[d] = bot & tmask;
            }
        } else if ( is_instruction(instr_int, 0xd2800000) ) {
            // MOVZ 64
            // cout << "MOVZ 64" << endl;
            uint64_t d = extract(instr_int, 4, 0);
            uint64_t imm = extract(instr_int, 20, 5);
            uint64_t pos = (extract(instr_int, 22, 21)) << 4;
            uint64_t result = 0;
            uint64_t j = 0;
            for ( uint64_t i = pos; i < pos + 16; i++ ) {
                result |= (extract_single(imm, j++) << i);
            }
            if ( d != 31 ) {
                reg[d] = result;
            }
        } else if ( is_instruction(instr_int, 0xd1000000) ) {
            // SUB immediate 64
            // cout << "SUB immediate 64" << endl;
            uint64_t d = extract(instr_int, 4, 0);
            uint64_t n = extract(instr_int, 9, 5);
            uint64_t imm = extract(instr_int, 21, 10);
            uint64_t shift = extract_single32(instr_int, 22);
            if ( shift == 1 ) {
                imm = imm << 12;
            }
            uint64_t operand1 = reg[n];
            uint64_t operand2 = ~imm;
            uint64_t result = operand1 + operand2 + 1;
            if ( d != 31 ) {
                reg[d] = result;
            }
        } else if ( is_instruction(instr_int, 0xcb000000) ) {
            // SUB shifted register 64
            uint64_t Rd = extract(instr_int, 4, 0);
            uint64_t Rn = extract(instr_int, 9, 5);
            uint64_t imm6 = extract(instr_int, 15, 10);
            uint64_t Rm = extract(instr_int, 20, 16);
            uint64_t shift = extract(instr_int, 23, 22);

            uint64_t result = 0;
            uint64_t operand1 = Rn == 31 ? 0 : reg[Rn];
            uint64_t operand2 = shift_reg64(Rm == 31 ? 0 : reg[Rm], shift, imm6);

            uint8_t nzcv = 0;

            operand2 = ~(operand2);

            add_with_carry64(operand1, operand2, (uint8_t) 1, &result, &nzcv);

            nzcvLocal = nzcv;

            if ( Rd != 31 ) {
               reg[Rd] = result;
            }
        } else if ( is_instruction(instr_int, 0xc85ffc00) ) {
            // LDAXR 64
            uint64_t t = extract(instr_int, 4, 0);
            uint64_t n = extract(instr_int, 9, 5);
            uint64_t address = reg[n];
            uint64_t data = memory_get_64(address);
            if ( t != 31 ) {
                reg[t] = data;
            }
        } else if ( is_instruction(instr_int, 0xc800fc00) ){
            //stlxr 64 bit
            //STLXR 64 bit
            uint64_t t = extract(instr_int, 4, 0);
            uint64_t n = extract(instr_int, 9, 5);
            uint64_t s = extract(instr_int, 20, 16);
            //uint8_t size = extract(instr_int, 31, 30);
            //uint64_t elsize = 64;

            uint64_t address = 0;
            uint64_t data = 0;
            //uint64_t dbytes = 4;

            address = reg[n];

            if(t != 31){
              data = reg[t];
            }

            memory_set_64(address, data);
            if(s != 31){
              reg[s] = 0;
            }

        } else if ( is_instruction(instr_int, 0xc8007c00) ) {
            // STXR 64
            uint64_t t = extract(instr_int, 4, 0);
            uint64_t n = extract(instr_int, 9, 5);
            uint64_t s = extract(instr_int, 20, 16);
            uint64_t address = reg[n];
            uint64_t data = reg[t];
            memory_set_64(address, data);
            if ( s != 31 ) {
                reg[s] = 0;
            }
        } else if ( is_instruction(instr_int, 0xb9400000) ) {
            // LDR immediate unsigned offset 32
            // cout << "LDR immediate unsigned offset 32" << endl;
            uint64_t imm12 = extract(instr_int, 21, 10);
            uint64_t t = extract(instr_int, 4, 0);
            uint64_t n = extract(instr_int, 9, 5);
            uint64_t offset = imm12 << 2;
            uint64_t address = reg[n];
            address = address + offset;
            uint64_t data = memory_get_32(address);
            if ( t != 31 ) {
                reg[t] = data;
            }
        } else if ( is_instruction(instr_int, 0xb9000000) ) {
            // STR immediate unsigned offset 32
            // cout << "STR immediate unsigned offset 32" << endl;
            uint8_t size = extract32(instr_int, 31, 30);
            uint32_t t = extract32(instr_int, 4, 0);
            uint32_t n = extract32(instr_int, 9, 5);
            uint32_t imm = extract32(instr_int, 21, 10);
            uint64_t offset = logical_shift_left32(imm, size);
            uint64_t address = reg[n];
            address = address + offset;
            uint64_t data = reg[t];
            memory_set_32(address, data);
        } else if ( is_instruction(instr_int, 0xb8800400) ) {
            // LDRSW immediate post-index
            // cout << "LDRSW immediate post-index" << endl;

            uint64_t Rt = extract(instr_int, 4, 0);
            uint64_t Rn = extract(instr_int, 9, 5);
            uint64_t imm9 = extract(instr_int, 20, 12);

            uint64_t offset = sign_extend_64(imm9, 9);

            uint64_t address = reg[Rn];
            uint32_t data = memory_get_32(address);

            if (Rt != 31) {
                reg[Rt] = sign_extend_64(data, 32);
            }

            address = address + offset;

            reg[Rn] = address;

        } else if ( is_instruction(instr_int, 0xb8400c00) ) {
            // LDR immediate pre-index 32
            // cout << "LDR immediate pre-index 32" << endl;
            uint64_t imm9 = extract(instr_int, 20, 12);
            uint64_t t = extract(instr_int, 4, 0);
            uint64_t n = extract(instr_int, 9, 5);
            uint64_t offset = sign_extend_32(imm9, 9);
            uint64_t address = 0;
            address = reg[n];
            address = address + offset;
            uint32_t data = memory_get_32(address);
            if ( t != 31 ) {
                reg[t] = data;
            }
            reg[n] = address;
        } else if ( is_instruction(instr_int, 0xb8400400) ) {
            // LDR immediate post-index 32
            // cout << "LDR immediate post-index 32" << endl;
            uint64_t imm9 = extract(instr_int, 20, 12);
            uint64_t t = extract(instr_int, 4, 0);
            uint64_t n = extract(instr_int, 9, 5);
            uint64_t offset = sign_extend_32(imm9, 9);
            uint64_t address = 0;
            address = reg[n];
            uint32_t data = memory_get_32(address);
            if ( t != 31 ) {
                reg[t] = data;
            }
            address = address + offset;
            reg[n] = address;
        } else if ( is_instruction(instr_int, 0xb8400400) ) {
            // LDRU 32
            uint64_t imm9 = extract(instr_int, 20, 12);
            uint64_t t = extract(instr_int, 4, 0);
            uint64_t n = extract(instr_int, 9, 5);
            uint64_t offset = sign_extend_32(imm9, 9);
            uint64_t address = reg[n];
            address = address + offset;
            uint32_t data = memory_get_32(address);
            if ( t != 31 ) {
                reg[t] = data;
            }
        } else if( is_instruction(instr_int, 0xb8200800) ){
            // str shifted register 32
            // STR SHIFTED REGISTER 32
            uint64_t t = extract(instr_int, 4, 0);
            uint64_t n = extract(instr_int, 9, 5);
            uint64_t extend_type = extract(instr_int, 15, 13);
            uint8_t S = extract(instr_int, 12, 12);

            uint64_t m = extract(instr_int, 20, 16);
            uint64_t scale = 2;
            uint64_t shift = S == 1 ? scale : 0;


            uint64_t offset = extend_reg64(m, extend_type, shift, reg);
            uint64_t address = reg[n];
            uint32_t data;
            address += offset;
            data = 0;
            if (t != 31){
              data = reg[t];
            }
            memory_set_32(address, data);
        } else if ( is_instruction(instr_int, 0xb8000c00) ) {
            // STR immediate pre-index 32
            // cout << "STR immediate pre-index 32" << endl;
            uint32_t t = extract32(instr_int, 4, 0);
            uint32_t n = extract32(instr_int, 9, 5);
            uint32_t imm = extract32(instr_int, 20, 12);
            uint64_t offset = sign_extend_64(imm, 9);
            uint64_t address = reg[n];
            address = address + offset;
            uint64_t data = reg[t];
            memory_set_32(address, data);
            reg[n] = address;
        } else if ( is_instruction(instr_int, 0xb8000400) ) {
            // STR immediate post-index 32
            // cout << "STR immediate post-index 32" << endl;
            uint32_t t = extract32(instr_int, 4, 0);
            uint32_t n = extract32(instr_int, 9, 5);
            uint32_t imm = extract32(instr_int, 20, 12);
            uint64_t offset = sign_extend_64(imm, 9);
            uint64_t address = reg[n];

            uint64_t data = reg[t];
            memory_set_32(address, data);
            address = address + offset;
            reg[n] = address;
        } else if ( is_instruction(instr_int, 0xb7000000) ) {
            // TBNZ 64
            uint64_t t = extract(instr_int, 4, 0);
            uint64_t bitpos = (1 << 4) | extract(instr_int, 23, 19);
            uint64_t imm14 = extract(instr_int, 18, 5);
            uint64_t offset = sign_extend_32(imm14 << 2, 16);
            uint64_t regVal = 0;
            if ( t != 31 ) {
                regVal = reg[t];
            }
            bool isZero = true;
            if ( extract_single(regVal, bitpos) != 0 ) {
                isZero = false;
            }
            if ( !isZero ) {
                pcChange = true;
                pcLocal = pcLocal + offset;
            }
        } else if ( is_instruction(instr_int, 0xb6000000) ) {
            // TBZ 64
            uint64_t t = extract(instr_int, 4, 0);
            uint64_t bitpos = (1 << 4) | extract(instr_int, 23, 19);
            uint64_t imm14 = extract(instr_int, 18, 5);
            uint64_t offset = sign_extend_32(imm14 << 2, 16);
            uint64_t regVal = 0;
            if ( t != 31 ) {
                regVal = reg[t];
            }
            bool isZero = false;
            if ( extract_single(regVal, bitpos) == 0 ) {
                isZero = true;
            }
            if ( isZero ) {
                pcChange = true;
                pcLocal = pcLocal + offset;
            }
        } else if ( is_instruction(instr_int, 0xb5000000) ) {
            // CBNZ 64
            // cout << "CBNZ 64" << endl;
            uint64_t imm = extract(instr_int, 23, 5);
            uint64_t t = extract(instr_int, 4, 0);
            uint64_t offset = sign_extend_64(imm << 2, 21);
            uint32_t regVal = 0;
            if ( t != 31 ) {
                regVal = reg[t];
            }
            bool isZero = true;
            if ( regVal != 0 ) {
                isZero = false;
            }
            if ( !isZero ) {
                pcChange = true;
                pcLocal = pcLocal + offset;
            }
        } else if ( is_instruction(instr_int, 0xb4000000) ) {
            // CBZ 64
            // cout << "CBZ 64" << endl;
            uint64_t imm = extract(instr_int, 23, 5);
            uint64_t t = extract(instr_int, 4, 0);
            uint64_t offset = sign_extend_64(imm << 2, 21);
            uint32_t regVal = 0;
            if ( t != 31 ) {
                regVal = reg[t];
            }
            bool isZero = true;
            if ( regVal != 0 ) {
                isZero = false;
            }
            if ( isZero ) {
                pcChange = true;
                pcLocal = pcLocal + offset;
            }
        } else if ( is_instruction(instr_int, 0xb1000000) ) {
            // ADDS immediate 64
            // cout << "ADDS immediate 64" << endl;
            uint64_t d = extract(instr_int, 4, 0);
            uint64_t n = extract(instr_int, 9, 5);
            uint64_t imm = extract(instr_int, 21, 10);
            uint64_t shift = extract_single32(instr_int, 22);
            if ( shift == 1 ) {
                imm = imm << 12;
            }
            uint64_t result = 0;
            uint64_t operand1 = reg[n];
            uint64_t operand2 = imm;
            uint8_t nzcv_temp = 0;
            add_with_carry64(operand1, operand2, 0, &result, &nzcv_temp);
            nzcvLocal = nzcv_temp;
            if ( d != 31 ) {
                reg[d] = result;
            }
        } else if ( is_instruction(instr_int, 0x1b000000) ) {
            // MADD 32
            // cout << "MADD 32" << endl;
            uint64_t d = extract(instr_int, 4, 0);
            uint64_t n = extract(instr_int, 9, 5);
            uint64_t m = extract(instr_int, 20, 16);
            uint64_t a = extract(instr_int, 14, 10);
            uint64_t aval = a != 31 ? reg[a] : 0, nval = n != 31 ? reg[n] : 0, mval = m != 31 ? reg[m] : 0;
            if ( d != 31 ) {
                reg[d] = aval + (nval * mval);
            }
        } else if ( is_instruction(instr_int, 0xab000000) ) {
            // ADDS shifted register 64
            // cout << "ADD shifted register 64" << endl;
            uint64_t d = extract(instr_int, 4, 0);
            uint64_t n = extract(instr_int, 9, 5);
            uint64_t m = extract(instr_int, 20, 16);
            uint64_t imm = extract(instr_int, 15, 10);
            // shift = 0 => LSL
            // shift = 1 => LSR
            // shift = 2 => ASR
            // shift = 3 => ROR
            uint64_t shift_type = extract(instr_int, 23, 22);
            uint64_t shift_amt = imm;
            uint64_t operand1 = 0;
            if ( n != 31 ) {
                operand1 = reg[n];
            }
            uint64_t operand2 = 0;
            if ( m != 31 ) {
                operand2 = reg[m];
            }
            operand2 = shift_reg64(operand2, shift_type, shift_amt);
            uint64_t result = 0;
            uint8_t nzcv_temp = 0; // useless
            add_with_carry64(operand1, operand2, 0, &result, &nzcv_temp);
            nzcvLocal = nzcv_temp;
            if ( d != 31 ) {
                reg[d] = result;
            }
        } else if ( is_instruction(instr_int, 0xaa000000) ) {
            // ORR shifted register 64
            // cout << "ORR shifted register 64" << endl;
            uint64_t d = extract(instr_int, 4, 0);
            uint64_t n = extract(instr_int, 9, 5);
            uint64_t m = extract(instr_int, 20, 16);
            uint64_t imm = extract(instr_int, 15, 10);
            // shift = 0 => LSL
            // shift = 1 => LSR
            // shift = 2 => ASR
            // shift = 3 => ROR
            uint64_t shift_type = extract(instr_int, 23, 22);
            uint64_t shift_amt = imm;
            uint64_t operand1 = 0;
            if ( n != 31 ) {
                operand1 = reg[n];
            }
            uint64_t regVal = 0;
            if ( m != 31 ) {
                regVal = reg[m];
            }
            uint64_t operand2 = shift_reg64(regVal, shift_type, shift_amt);
            uint64_t result = operand1 | operand2;
            if ( d != 31 ) {
                reg[d] = result;
            }
        } else if ( is_instruction(instr_int, 0xa9800000) ) {
            // STP pre-index 64-bit
            uint64_t n = extract(instr_int, 9, 5);
            uint64_t t = extract(instr_int, 4, 0);
            uint64_t t2 = extract(instr_int, 14, 10);
            uint64_t offset = sign_extend_64(extract(instr_int, 21, 15), 7) << 3;
            uint64_t address = reg[n];
            address = address + offset;
            uint64_t data = t == 31 ? 0 : reg[t];
            uint64_t data2 = t2 == 31 ? 0 : reg[t2];
            memory_set_64(address, data);
            memory_set_64(address + 8, data2);
            reg[n] = address;
        } else if ( is_instruction(instr_int, 0xa9400000) ) {
            // LDP signed offset 64-bit
            uint64_t n = extract(instr_int, 9, 5);
            uint64_t t = extract(instr_int, 4, 0);
            uint64_t t2 = extract(instr_int, 14, 10);
            uint64_t offset = sign_extend_64(extract(instr_int, 21, 15), 7) << 3;
            uint64_t address = reg[n];
            address = address + offset;
            uint64_t data = memory_get_64(address);
            uint64_t data2 = memory_get_64(address + 8);
            if ( t != 31 ) {
                reg[t] = data;
            }
            if ( t2 != 31 ) {
                reg[t2] = data2;
            }
        } else if ( is_instruction(instr_int, 0xa9c00000) ) {
            // LDP pre-index 64
            uint64_t n = extract(instr_int, 9, 5);
            uint64_t t = extract(instr_int, 4, 0);
            uint64_t t2 = extract(instr_int, 14, 10);
            uint64_t offset = sign_extend_64(extract(instr_int, 21, 15), 7) << 3;
            uint64_t address = reg[n];
            address = address + offset;
            uint64_t data = memory_get_64(address);
            uint64_t data2 = memory_get_64(address + 8);
            if ( t != 31 ) {
                reg[t] = data;
            }
            if ( t2 != 31 ) {
                reg[t2] = data2;
            }
            reg[n] = address;
        } else if ( is_instruction(instr_int, 0xa9000000) ) {
            // STP signed offset 64-bit
            uint64_t n = extract(instr_int, 9, 5);
            uint64_t t = extract(instr_int, 4, 0);
            uint64_t t2 = extract(instr_int, 14, 10);
            uint64_t offset = sign_extend_64(extract(instr_int, 21, 15), 7) << 3;
            uint64_t address = reg[n];
            address = address + offset;
            uint64_t data = t == 31 ? 0 : reg[t];
            uint64_t data2 = t2 == 31 ? 0 : reg[t2];
            memory_set_64(address, data);
            memory_set_64(address + 8, data2);
        } else if ( is_instruction(instr_int, 0xa8c00000) ) {
            // LDP post-index 64-bit
            uint64_t n = extract(instr_int, 9, 5);
            uint64_t t = extract(instr_int, 4, 0);
            uint64_t t2 = extract(instr_int, 14, 10);
            uint64_t offset = sign_extend_64(extract(instr_int, 21, 15), 7) << 3;
            uint64_t address = reg[n];
            uint64_t data = memory_get_64(address);
            uint64_t data2 = memory_get_64(address + 8);
            if ( t != 31 ) {
                reg[t] = data;
            }
            if ( t2 != 31 ) {
                reg[t2] = data2;
            }
            address = address + offset;
            reg[n] = address;
        } else if ( is_instruction(instr_int, 0xa8800000) ) {
            // STP post-index 64-bit
            uint64_t n = extract(instr_int, 9, 5);
            uint64_t t = extract(instr_int, 4, 0);
            uint64_t t2 = extract(instr_int, 14, 10);
            uint64_t offset = sign_extend_64(extract(instr_int, 21, 15), 7) << 3;
            uint64_t address = reg[n];
            uint64_t data = t == 31 ? 0 : reg[t];
            uint64_t data2 = t2 == 31 ? 0 : reg[t2];
            memory_set_64(address, data);
            memory_set_64(address + 8, data2);
            address = address + offset;
            reg[n] = address;
        } else if ( is_instruction(instr_int, 0x9b000000) ) {
            // MADD 64
            // cout << "MADD 64" << endl;
            uint64_t d = extract(instr_int, 4, 0);
            uint64_t n = extract(instr_int, 9, 5);
            uint64_t m = extract(instr_int, 20, 16);
            uint64_t a = extract(instr_int, 14, 10);
            uint64_t aval = a != 31 ? reg[a] : 0, nval = n != 31 ? reg[n] : 0, mval = m != 31 ? reg[m] : 0;
            if ( d != 31 ) {
                reg[d] = aval + (nval * mval);
            }
        } else if ( is_instruction(instr_int, 0x9a800400) ) {
            // CSINC 64
            uint64_t d = extract(instr_int, 4, 0);
            uint64_t n = extract(instr_int, 9, 5);
            uint64_t m = extract(instr_int, 20, 16);
            uint8_t fullcond = extract32(instr_int, 15, 12);
            uint64_t result = 0;
            uint64_t operand1 = n == 31 ? 0 : reg[n];
            uint64_t operand2 = m == 31 ? 0 : reg[m];
            bool condHolds = conditionHolds(fullcond, nzcvLocal);
            if ( condHolds ) {
                result = operand1;
            } else {
                result = operand2 + 1;
            }
            if ( d != 31 ) {
                reg[d] = result;
            }
        } else if( is_instruction(instr_int, 0x9a800000) ){
            //CSEL 64
            uint64_t d = extract(instr_int, 4, 0);
            uint64_t n = extract(instr_int, 9, 5);
            uint8_t cond = extract(instr_int, 15, 12);
            uint64_t m = extract(instr_int, 20, 16);
            uint64_t result = 0;
            uint64_t operand1 = n == 31 ? 0 : reg[n];
            uint64_t operand2 = m == 31 ? 0 : reg[m];

            if(conditionHolds(cond, nzcvLocal))
              result = operand1;
            else
              result = operand2;
            if ( d != 31 ) {
                reg[d] = result;
            }
        } else if ( is_instruction(instr_int, 0x94000000) ) {
            // BL
            // cout << "BL" << endl;
            uint64_t imm = extract(instr_int, 25, 0);
            imm = sign_extend_64(imm << 2, 28);
            reg[30] = pcLocal + 4;
            pcLocal += imm;
            pcChange = true;
        } else if ( is_instruction(instr_int, 0x93000000) ) {
            // SBFM 64
            // cout << "SBFM 64" << endl;
            uint64_t N = extract_single(instr_int, 22);
            uint64_t d = extract(instr_int, 4, 0);
            uint64_t n = extract(instr_int, 9, 5);
            uint64_t immr = extract(instr_int, 21, 16);
            uint64_t imms = extract(instr_int, 15, 10);
            uint64_t wmask = 0;
            uint64_t tmask = 0;
            decode_bit_masks(N, imms, immr, false, &wmask, &tmask);
            uint64_t src = 0;
            if ( n != 31 ) {
                src = reg[n];
            }
            uint64_t bot = rotate_right(src, immr) & wmask;
            uint64_t top = replicate(extract_single(src, imms), 1, 64);
            if ( d != 31 ) {
                reg[d] = (top & ~tmask) | (bot & tmask);
            }
        } else if ( is_instruction(instr_int, 0x92800000) ) {
            // MOVN 64
            // cout << "MOVN 64" << endl;
            uint32_t d = extract(instr_int, 4, 0);
            uint32_t pos = extract(instr_int, 22, 21);
            uint32_t imm = extract(instr_int, 20, 5);
            pos = pos << 4;
            uint64_t result = imm << pos;
            result = ~result;
            if ( d != 31 ) {
                reg[d] = result;
            }
        } else if ( is_instruction(instr_int, 0x92000000) ) {
            // AND immediate 64
            // cout << "AND immediate 64" << endl;
            uint64_t N = extract_single(instr_int, 22);
            uint64_t d = extract(instr_int, 4, 0);
            uint64_t n = extract(instr_int, 9, 5);
            uint64_t immr = extract(instr_int, 21, 16);
            uint64_t imms = extract(instr_int, 15, 10);
            uint64_t imm = 0;
            uint64_t nah = 0; // throwaway value
            decode_bit_masks(N, imms, immr, true, &imm, &nah);
            if ( d != 31 ) {
                if ( n == 31 ) {
                    reg[d] = 0;
                } else {
                    reg[d] = reg[n] & imm;
                }
            }
        } else if ( is_instruction(instr_int, 0x91000000) ) {
            // ADD immediate 64
            // cout << "ADD immediate 64" << endl;
            uint64_t d = extract(instr_int, 4, 0);
            uint64_t n = extract(instr_int, 9, 5);
            uint64_t imm = extract(instr_int, 21, 10);
            uint64_t shift = extract(instr_int, 23, 22);
            uint64_t source_val = reg[n];
            if ( shift == 1 ) {
                imm = imm << 12;
            }
            uint64_t result = source_val + imm;
            reg[d] = result;
        } else if ( is_instruction(instr_int, 0x90000000) ) {
            // ADRP
            // cout << "ADRP" << endl;
            uint64_t d = extract(instr_int, 4, 0);
            uint64_t immhi = extract(instr_int, 23, 5);
            uint64_t immlo = extract(instr_int, 30, 29);
            uint64_t imm = sign_extend_64(((immhi << 2) | immlo) << 12, 33);
            uint64_t base = pcLocal;
            base = (base >> 12) << 12;
            if ( d != 31 ) {
                reg[d] = base + imm;
            }
        } else if ( is_instruction(instr_int, 0x8b000000) ) {
            // ADD shifted register 64
            // cout << "ADD shifted register 64" << endl;
            uint64_t d = extract(instr_int, 4, 0);
            uint64_t n = extract(instr_int, 9, 5);
            uint64_t m = extract(instr_int, 20, 16);
            uint64_t imm = extract(instr_int, 15, 10);
            // shift = 0 => LSL
            // shift = 1 => LSR
            // shift = 2 => ASR
            // shift = 3 => ROR
            uint64_t shift_type = extract(instr_int, 23, 22);
            uint64_t shift_amt = imm;
            uint64_t operand1 = 0;
            if ( n != 31 ) {
                operand1 = reg[n];
            }
            uint64_t operand2 = 0;
            if ( m != 31 ) {
                operand2 = reg[m];
            }
            operand2 = shift_reg64(operand2, shift_type, shift_amt);
            uint64_t result = operand1 + operand2;
            // uint8_t nzcv_temp = 0; // useless
            // add_with_carry64(operand1, operand2, 0, &result, &nzcv_temp);
            if ( d != 31 ) {
                reg[d] = result;
            }
        } else if(is_instruction(instr_int, 0x8a200000) ){
            //bic 64
            //BIC 64
            uint64_t d = extract(instr_int, 4, 0);
            uint64_t n = extract(instr_int, 9, 5);
            uint64_t m = extract(instr_int, 20, 16);

            uint64_t shift_amount = extract(instr_int, 10, 15);
            uint64_t shift_type = extract(instr_int, 23, 22);

//            uint64_t datasize = 64;

            uint64_t operand1 = 0;
            if(n != 31){
                operand1 = reg[n];
            }

            uint64_t operand2 = shift_reg64(m == 31 ? 0 : reg[m], shift_type, shift_amount);
            operand2 = ~operand2;
            uint64_t result = operand1 & operand2;
            if(d != 31){
                reg[d] = result;
            }
        } else if(is_instruction(instr_int, 0x8a000000) ){
          //and shifted register 64
          //AND SHIFTED REGISTER 64
          uint64_t d = extract(instr_int, 4, 0);
          uint64_t n = extract(instr_int, 9, 5);
          uint64_t m = extract(instr_int, 20, 16);
          uint64_t shift_type = extract(instr_int, 23, 22);
          uint64_t shift_amount = extract(instr_int, 15, 10);

          uint64_t operand1 = 0;
          if(n != 31){
            operand1 = reg[n];
          }
          uint64_t operand2 = shift_reg64(m == 31 ? 0 : reg[m], shift_type, shift_amount);
          uint64_t result = operand1 & operand2;
          if(d != 31){
            reg[d] = result;
          }
        }
        else if ( is_instruction(instr_int, 0x885ffc00) ) {
            // LDAXR 32
            uint64_t t = extract(instr_int, 4, 0);
            uint64_t n = extract(instr_int, 9, 5);
            uint64_t address = reg[n];
            uint32_t data = memory_get_32(address);
            if ( t != 31 ) {
                reg[t] = data;
            }
        } else if( is_instruction(instr_int, 0x8800fc00) ){
          //stlxr 32
          //STLXR 32
          uint64_t t = extract(instr_int, 4, 0);
          uint64_t n = extract(instr_int, 9, 5);
          uint64_t s = extract(instr_int, 20, 16);
          //uint8_t size = extract(instr_int, 31, 30);
          //uint64_t elsize = 32;

          uint64_t address = 0;
          uint32_t data = 0;
          //uint64_t dbytes = 4;

          address = reg[n];

          if(t != 31){
            data = reg[t];
          }

          memory_set_32(address, data);
          if(s != 31){
            reg[s] = 0;
          }
        }

        else if ( is_instruction(instr_int, 0x88007c00) ) {
            // STXR 32
            uint64_t t = extract(instr_int, 4, 0);
            uint64_t n = extract(instr_int, 9, 5);
            uint64_t s = extract(instr_int, 20, 16);
            uint64_t address = reg[n];
            uint32_t data = reg[t];
            memory_set_32(address, data);
            if ( s != 31 ) {
                reg[s] = 0;
            }
        } else if ( is_instruction(instr_int, 0x7a400800) ) {
            // CCMP immediate 32
            uint64_t n = extract(instr_int, 9, 5);
            uint64_t imm5 = extract(instr_int, 20, 16);
            uint8_t flags = extract(instr_int, 3, 0);
            uint8_t fullcond = extract32(instr_int, 15, 12);
            // bool condHolds = false;
            // if ( cond == 0 ) {
            //     condHolds = (extract_single32(nzcvLocal, 2) == 1); // Z == 1 <-> EQ or NE
            // } else if ( cond == 1 ) {
            //     condHolds = (extract_single32(nzcvLocal, 1) == 1); // C == 1 <-> CS or CC
            // } else if ( cond == 2 ) {
            //     condHolds = (extract_single32(nzcvLocal, 3) == 1); // N == 1 <-> MI or PL
            // } else if ( cond == 3 ) {
            //     condHolds = (extract_single32(nzcvLocal, 0) == 1); // V == 1 <-> VS or VC
            // } else if ( cond == 4 ) {
            //     condHolds = (extract_single32(nzcvLocal, 1) == 1 && extract_single32(nzcvLocal, 2) == 0); // C == 1 && Z == 0 <-> HI or LS
            // } else if ( cond == 5 ) {
            //     condHolds = (extract_single32(nzcvLocal, 3) == extract_single32(nzcvLocal, 0)); // N == V <-> GE or LT
            // } else if ( cond == 6 ) {
            //     condHolds = (extract_single32(nzcvLocal, 3) == extract_single32(nzcvLocal, 0) && extract_single32(nzcvLocal, 2) == 0); // N == V && Z == 0 <-> GT or LE
            // } else {
            //     condHolds = true; // AL
            // }
            // if ( extract_single32(condLast, 0) == 1 && cond != 7 ) {
            //     condHolds = !condHolds;
            // }
            bool condHolds = conditionHolds(fullcond, nzcvLocal);
            uint64_t operand1 = reg[n];
            uint64_t operand2 = 0;
            uint32_t result_throwaway = 0;
            uint8_t flags_result = 0;
            if ( condHolds ) {
                operand2 = ~imm5;
                add_with_carry32(operand1, operand2, 1, &result_throwaway, &flags_result);
                nzcvLocal = flags_result;
            } else {
                nzcvLocal = flags;
            }
        } else if( is_instruction(instr_int, 0x7a400000) ){
          //ccmp register 32
          //CCMP REGISTER 32
          uint8_t flags = extract(instr_int, 4, 0);
          uint64_t n = extract(instr_int, 9, 5);
          uint64_t cond = extract(instr_int, 15, 12);
          uint64_t m = extract(instr_int, 20, 16);

          uint32_t operand1 = 0;
          uint32_t operand2 = 0;

          if(n != 31){
            operand1 = reg[n];
          }
          if(m != 31){
            operand2 = reg[m];
          }
          uint32_t result_throwaway = 0;
          uint8_t flags_result = 0;
          if(conditionHolds(cond, cond)){
            operand2 = ~operand2;
            add_with_carry32(operand1, operand2, 1, &result_throwaway, &flags_result);
            flags = flags_result;
          }
          nzcvLocal = flags;
        } else if( is_instruction(instr_int, 0x79000000) ){
            //STRH Immediate unsigned offset
            uint32_t t = extract(instr_int, 4, 0);
            uint32_t n = extract(instr_int, 9, 5);
            uint32_t imm = extract(instr_int, 20, 12);
            uint64_t offset = imm << 1;
            uint64_t addr = reg[n] + offset;
            uint32_t data = t==31 ? 0 : reg[t];
            memory_set(addr, extract(data,7,0));
            memory_set(addr, extract(data,15,8));
        } else if( is_instruction(instr_int, 0x72000000) ){
            //ANDS immediate 32
            uint32_t d = extract(instr_int, 4, 0);
            uint32_t n = extract(instr_int, 9, 5);
            uint32_t imms = extract(instr_int, 15, 10);
            uint32_t immr = extract(instr_int, 21, 16);
            uint32_t N = extract(instr_int, 22, 22);
            uint64_t nah = 0;
            uint64_t imm = 0;
            decode_bit_masks(N, imms, immr, true, &imm, &nah);
            uint32_t operand1 = n == 31 ? 0 : reg[n];
            uint32_t result = operand1 & imm;
            uint32_t lastBit = result >> 31;
            nzcvLocal = (lastBit << 3) | ((result==0) <<2);
            if(d != 31){
              reg[d] = result;
            }
        } else if ( is_instruction(instr_int, 0x71000000) ) {
            // SUBS immediate 32
            // cout << "SUBS immediate 32" << endl;
            uint32_t d = extract32(instr_int, 4, 0);
            uint32_t n = extract32(instr_int, 9, 5);
            uint32_t imm = extract32(instr_int, 21, 10);
            uint32_t shift = extract_single32(instr_int, 22);
            if ( shift == 1 ) {
                imm = imm << 12;
            }
            uint32_t result = 0;
            uint32_t operand1 = reg[n];
            uint32_t operand2 = ~imm;
            uint8_t nzcv_temp = 0;
            add_with_carry32(operand1, operand2, 1, &result, &nzcv_temp);
            nzcvLocal = nzcv_temp;
            if ( d != 31 ) {
                reg[d] = result;
            }
        } else if ( is_instruction(instr_int, 0x6b000000) ) {
            // SUBS shifted register 32

            uint64_t Rd = extract(instr_int, 4, 0);
            uint64_t Rn = extract(instr_int, 9, 5);
            uint64_t imm6 = extract(instr_int, 15, 10);
            uint64_t Rm = extract(instr_int, 20, 16);
            uint64_t shift = extract(instr_int, 23, 22);
            //uint64_t sf = extract(instr_int, 31, 31);

            uint32_t result;
            uint32_t operand1 = reg[Rn];
            uint32_t operand2 = shift_reg32((uint32_t) Rm == 31 ? 0 : reg[Rm], (uint32_t) shift, (int) imm6);

            uint8_t nzcv;

            operand2 = ~(operand2);

            add_with_carry32(operand1, operand2, (uint8_t) 1, &result, &nzcv);

            if ( Rd != 31 ) {
                reg[Rd] = result;
            }

        } else if( is_instruction(instr_int, 0x6a200000) ){
            // bics 32 bit
            // BICS 32 BIT
            uint64_t d = extract(instr_int, 4, 0);
            uint64_t n = extract(instr_int, 9, 5);
            uint64_t m = extract(instr_int, 20, 16);

            uint64_t shift_amount = extract(instr_int, 10, 15);
            uint64_t shift_type = extract(instr_int, 23, 22);

            //uint64_t datasize = 32;

            uint32_t operand1 = 0;
            if(n != 31){
                operand1 = reg[n];
            }

            uint32_t operand2 = shift_reg32(m == 31 ? 0 : reg[m], shift_type, shift_amount);
            operand2 = ~operand2;
            uint32_t result = operand1 & operand2;
            nzcvLocal = (extract_single32(result, 31) << 3) | ((result == 0) << 2) | 0;

            if(d != 31){
                reg[d] = result;
            }
        }
        else if( is_instruction(instr_int, 0x5ac01000) ){
          //clz 32 bit
          //CLZ 32 bit
          uint64_t d = extract(instr_int, 4, 0);
          uint64_t n = extract(instr_int, 9, 5);
          uint32_t result = 0;
          uint32_t operand1 = 0;
          if(n != 31){
            operand1 = reg[n];
          }
          result = count_leading_zero_bits32(operand1);
          if(d != 31){
            reg[d] = result;
          }
        }
        else if( is_instruction(instr_int, 0x5a800000) ){
          //csinv 32 bit
          //CSINV 32 bit
          uint64_t d = extract(instr_int, 4, 0);
          uint64_t n = extract(instr_int, 9, 5);
          uint64_t m = extract(instr_int, 20, 16);
          uint8_t cond = extract(instr_int, 15, 12);
          uint32_t result = 0;
          uint32_t operand1 = 0;
          if(n != 31){
            operand1 = reg[n];
          }
          uint32_t operand2 = 0;
          if(m != 31){
            operand2 = reg[m];
          }
          if(conditionHolds(cond, nzcvLocal)){
            result = operand1;
          }
          else{
            result = ~operand2;
          }
          if(d != 31){
            reg[d] = result;
          }
        } else if ( is_instruction(instr_int, 0x58000000) ) {
            // LDR (literal) 64-bit
            uint64_t imm = extract(instr_int, 23, 5);
            uint64_t t = extract(instr_int, 4, 0);
            uint64_t offset = sign_extend_64(imm << 2, 21);
            uint64_t address = pcLocal + offset;
            uint64_t data = memory_get_64(address);
            if ( t != 31 ) {
                reg[t] = data;
            }
            printf("reg[t]: %lx", reg[t]);
        } else if ( is_instruction(instr_int, 0x54000000) ) {
            // B.cond
            // cout << "B.cond" << endl;
            uint64_t imm = extract(instr_int, 23, 5);
            uint64_t offset = sign_extend_64(imm << 2, 21);
            // uint8_t cond = extract32(instr_int, 3, 1);
            // uint8_t condLast = extract_single32(instr_int, 0);
            uint8_t fullcond = extract32(instr_int, 3, 0);
            // bool condHolds = false;
            // if ( cond == 0 ) {
            //     condHolds = (extract_single32(nzcvLocal, 2) == 1); // Z == 1 <-> EQ or NE
            // } else if ( cond == 1 ) {
            //     condHolds = (extract_single32(nzcvLocal, 1) == 1); // C == 1 <-> CS or CC
            // } else if ( cond == 2 ) {
            //     condHolds = (extract_single32(nzcvLocal, 3) == 1); // N == 1 <-> MI or PL
            // } else if ( cond == 3 ) {
            //     condHolds = (extract_single32(nzcvLocal, 0) == 1); // V == 1 <-> VS or VC
            // } else if ( cond == 4 ) {
            //     condHolds = (extract_single32(nzcvLocal, 1) == 1 && extract_single32(nzcvLocal, 2) == 0); // C == 1 && Z == 0 <-> HI or LS
            // } else if ( cond == 5 ) {
            //     condHolds = (extract_single32(nzcvLocal, 3) == extract_single32(nzcvLocal, 0)); // N == V <-> GE or LT
            // } else if ( cond == 6 ) {
            //     condHolds = (extract_single32(nzcvLocal, 3) == extract_single32(nzcvLocal, 0) && extract_single32(nzcvLocal, 2) == 0); // N == V && Z == 0 <-> GT or LE
            // } else {
            //     condHolds = true; // AL
            // }
            // if ( extract_single32(condLast, 0) == 1 && cond != 7 ) {
            //     condHolds = !condHolds;
            // }
            bool condHolds = conditionHolds(fullcond, nzcvLocal);
            if ( condHolds ) {
                pcChange = true;
                pcLocal = pcLocal + offset;
            }
        } else if ( is_instruction(instr_int, 0x53000000) ) {
            // UBFM 32
            // cout << "UBFM 32" << endl;
            uint32_t N = extract_single32(instr_int, 22);
            uint32_t d = extract32(instr_int, 4, 0);
            uint32_t n = extract32(instr_int, 9, 5);
            uint32_t immr = extract32(instr_int, 21, 16);
            uint32_t imms = extract32(instr_int, 15, 10);
            uint64_t wmask = 0;
            uint64_t tmask = 0;
            decode_bit_masks(N, imms, immr, false, &wmask, &tmask);
            uint64_t src = 0;
            if ( n != 31 ) {
                src = reg[n];
            }
            uint64_t bot = rotate_right32(src, immr) & wmask;
            if ( d != 31 ) {
                reg[d] = set_reg_32(reg[d], bot & tmask);
            }
        } else if ( is_instruction(instr_int, 0x52800000) ) {
            // MOVZ 32
            // cout << "MOVZ 32" << endl;
            uint64_t d = extract(instr_int, 4, 0);
            uint64_t imm = extract(instr_int, 20, 5);
            uint64_t pos = (extract(instr_int, 22, 21)) << 4;
            uint32_t result = 0;
            uint8_t j = 0;
            for ( uint64_t i = pos; i < pos + 16; i++ ) {
                result |= (extract_single32(imm, j++) << i);
            }
            if ( d != 31 ) {
                reg[d] = set_reg_32(result, 0);
            }
        } else if ( is_instruction(instr_int, 0x51000000) ) {
            // SUB immediate 32
            // cout << "SUB immediate 32" << endl;
            uint64_t d = extract(instr_int, 4, 0);
            uint64_t n = extract(instr_int, 9, 5);
            uint64_t imm = extract(instr_int, 21, 10);
            uint64_t shift = extract_single32(instr_int, 22);
            if ( shift == 1 ) {
                imm = imm << 12;
            }
            uint32_t operand1 = reg[n];
            uint32_t operand2 = ~imm;
            uint32_t result = operand1 + operand2 + 1;
            if ( d != 31 ) {
                reg[d] = result;
            }
        } else if ( is_instruction(instr_int, 0x4b000000) ) {
            // SUB shifted register 32
            uint64_t Rd = extract(instr_int, 4, 0);
            uint64_t Rn = extract(instr_int, 9, 5);
            uint64_t imm6 = extract(instr_int, 15, 10);
            uint64_t Rm = extract(instr_int, 20, 16);
            uint64_t shift = extract(instr_int, 23, 22);
            //uint64_t sf = extract(instr_int, 31, 31);

            uint32_t result;
            uint32_t operand1 = Rn == 31 ? 0 : reg[Rn];
            uint32_t operand2 = shift_reg32((uint32_t) Rm == 31 ? 0 : reg[Rm], (uint32_t) shift, (int) imm6);

            uint8_t nzcv;
            operand2 = ~(operand2);

            add_with_carry32(operand1, operand2, (uint8_t) 1, &result, &nzcv);

            if ( Rd != 31 ) {
                reg[Rd] = result;
                if ( debug ) {
                  printf("%lx", reg[Rd]);
                }
            }
        } else if ( is_instruction(instr_int, 0x3d800000) ) {
            // STR immediate unsigned offset SIMD 128
            uint64_t t = extract(instr_int, 4, 0);
            uint64_t n = extract(instr_int, 9, 5);
            uint64_t imm = extract(instr_int, 21, 10);
            uint64_t offset = imm << 4;
            uint64_t address = reg[n];
            address = address + offset;
            unsigned __int128 data = simd_reg[t];
            memory_set_128(address, data);
        } else if ( is_instruction(instr_int, 0x39400000) ) {
            // LDRB immediate unsigned offset 32
            // cout << "LDRB immediate unsigned offset 32" << endl;
            uint64_t imm = extract(instr_int, 21, 10);
            uint64_t n = extract(instr_int, 9, 5);
            uint64_t t = extract(instr_int, 4, 0);
            uint64_t offset = imm;
            uint64_t address = reg[n];
            address = address + offset;
            uint64_t data = memory_get(address);
            if ( t != 31 ) {
                reg[t] = data;
            }
        } else if ( is_instruction(instr_int, 0x39000000) ) {
            // STRB immediate unsigned offset 32
            // cout << "STRB immediate unsigned offset 32" << endl;
            uint64_t imm = extract(instr_int, 21, 10);
            uint64_t n = extract(instr_int, 9, 5);
            uint64_t t = extract(instr_int, 4, 0);
            uint64_t offset = imm;
            uint64_t address = reg[n];
            address = reg[n];
            address = address + offset;
            uint8_t data = t == 31 ? 0 : reg[t];
            memory_set(address, data);
        } else if( is_instruction(instr_int, 0x38600800) ){
            //ldrb register
            //LDRB register
            //LDRB REGISTER
            // how the
            // hell do we do this
            // thats what they said
            uint64_t t = extract(instr_int, 4, 0);
            uint64_t n = extract(instr_int, 9, 5);
            uint64_t m = extract(instr_int, 20, 16);
            uint64_t option = extract(instr_int, 15, 13);
            //uint64_t extend_type = decode_reg_extend(option);
            uint64_t extend_type = option;
            uint64_t offset = extend_reg64(m, extend_type, 0, reg);
            uint64_t address;
            uint8_t data;
            address = reg[n];
            address = address + offset;

            data = memory_get(address);
            if (t != 31) {
              reg[t] = data;
            }
        } else if ( is_instruction(instr_int, 0x38400c00) ) {
            // LDRB immediate pre-index 32
            // cout << "LDRB immediate pre-index 32" << endl;
            uint64_t imm = extract(instr_int, 20, 12);
            uint64_t n = extract(instr_int, 9, 5);
            uint64_t t = extract(instr_int, 4, 0);
            uint64_t offset = sign_extend_64(imm, 9);
            uint64_t address = 0;
            address = reg[n];
            address = address + offset;
            uint64_t data = memory_get(address);
            if ( t != 31 ) {
                reg[t] = data;
            }
            reg[n] = address;
        } else if ( is_instruction(instr_int, 0x38400400) ) {
            // LDRB immediate post-index 32
            // cout << "LDRB immediate post-index 32" << endl;
            uint64_t imm = extract(instr_int, 20, 12);
            uint64_t n = extract(instr_int, 9, 5);
            uint64_t t = extract(instr_int, 4, 0);
            uint64_t offset = sign_extend_64(imm, 9);
            uint64_t address = 0;
            address = reg[n];
            uint64_t data = memory_get(address);
            address = address + offset;
            if ( t != 31 ) {
                reg[t] = data;
            }
            reg[n] = address;
        } else if ( is_instruction(instr_int, 0x37000000) ) {
            // TBNZ 32
            uint64_t t = extract(instr_int, 4, 0);
            uint64_t bitpos = extract(instr_int, 23, 19);
            uint64_t imm14 = extract(instr_int, 18, 5);
            uint64_t offset = sign_extend_32(imm14 << 2, 16);
            uint64_t regVal = 0;
            if ( t != 31 ) {
                regVal = reg[t];
            }
            bool isZero = true;
            if ( extract_single32(regVal, bitpos) != 0 ) {
                isZero = false;
            }
            if ( !isZero ) {
                pcChange = true;
                pcLocal = pcLocal + offset;
            }
        } else if ( is_instruction(instr_int, 0x36000000) ) {
            // TBZ 32
            uint64_t t = extract(instr_int, 4, 0);
            uint64_t bitpos = extract(instr_int, 23, 19);
            uint64_t imm14 = extract(instr_int, 18, 5);
            uint64_t offset = sign_extend_32(imm14 << 2, 16);
            uint64_t regVal = 0;
            if ( t != 31 ) {
                regVal = reg[t];
            }
            bool isZero = false;
            if ( extract_single32(regVal, bitpos) == 0 ) {
                isZero = true;
            }
            if ( isZero ) {
                pcChange = true;
                pcLocal = pcLocal + offset;
            }
        } else if ( is_instruction(instr_int, 0x35000000) ) {
            // CBNZ 32
            // cout << "CBNZ 32" << endl;
            uint64_t imm = extract(instr_int, 23, 5);
            uint64_t t = extract(instr_int, 4, 0);
            uint64_t offset = sign_extend_64(imm << 2, 21);
            uint32_t regVal = 0;
            if ( t != 31 ) {
                regVal = reg[t];
            }
            bool isZero = true;
            if ( regVal != 0 ) {
                isZero = false;
            }
            if ( !isZero ) {
                pcChange = true;
                pcLocal = pcLocal + offset;
            }
        } else if ( is_instruction(instr_int, 0x34000000) ) {
            // CBZ 32
            // cout << "CBZ 32" << endl;
            uint64_t imm = extract(instr_int, 23, 5);
            uint64_t t = extract(instr_int, 4, 0);
            uint64_t offset = sign_extend_64(imm << 2, 21);
            uint32_t regVal = 0;
            if ( t != 31 ) {
                regVal = reg[t];
            }
            bool isZero = true;
            if ( regVal != 0 ) {
                isZero = false;
            }
            if ( isZero ) {
                pcChange = true;
                pcLocal = pcLocal + offset;
            }
        } else if ( is_instruction(instr_int, 0x31000000) ) {
            // ADDS immediate 32
            // cout << "ADDS immediate 32" << endl;
            uint32_t d = extract32(instr_int, 4, 0);
            uint32_t n = extract32(instr_int, 9, 5);
            uint32_t imm = extract32(instr_int, 21, 10);
            uint32_t shift = extract_single32(instr_int, 22);
            if ( shift == 1 ) {
                imm = imm << 12;
            }
            uint32_t result = 0;
            uint32_t operand1 = reg[n];
            uint32_t operand2 = imm;
            uint8_t nzcv_temp = 0;
            add_with_carry32(operand1, operand2, 0, &result, &nzcv_temp);
            nzcvLocal = nzcv_temp;
            if ( d != 31 ) {
                reg[d] = result;
            }
        } else if ( is_instruction(instr_int, 0x2a000000) ) {
            // ORR shifted register 32
            // cout << "ORR shifted register 32" << endl;
            uint32_t d = extract32(instr_int, 4, 0);
            uint32_t n = extract32(instr_int, 9, 5);
            uint32_t m = extract32(instr_int, 20, 16);
            uint32_t imm = extract32(instr_int, 15, 10);
            // shift = 0 => LSL
            // shift = 1 => LSR
            // shift = 2 => ASR
            // shift = 3 => ROR
            uint32_t shift_type = extract32(instr_int, 23, 22);
            uint32_t shift_amt = imm;
            uint32_t operand1 = 0;
            if ( n != 31 ) {
                operand1 = (uint32_t) reg[n];
            }
            uint32_t regVal = 0;
            if ( m != 31 ) {
                regVal = (uint32_t) reg[m];
            }
            uint32_t operand2 = shift_reg32(regVal, shift_type, shift_amt);
            uint32_t result = operand1 | operand2;
            if ( d != 31 ) {
                reg[d] = result;
            }
        } else if ( is_instruction(instr_int, 0x29800000) ) {
            // STP pre-index 32-bit
            uint64_t n = extract(instr_int, 9, 5);
            uint64_t t = extract(instr_int, 4, 0);
            uint64_t t2 = extract(instr_int, 14, 10);
            uint64_t offset = sign_extend_32(extract32(instr_int, 21, 15), 7) << 2;
            uint64_t address = reg[n];
            address = address + offset;
            uint64_t data = t == 31 ? 0 : reg[t];
            uint64_t data2 = t2 == 31 ? 0 : reg[t2];
            memory_set_32(address, data);
            memory_set_32(address + 4, data2);
            reg[n] = address;
        } else if ( is_instruction(instr_int, 0x29c00000) ) {
            // LDP pre-index 32
            uint64_t n = extract(instr_int, 9, 5);
            uint64_t t = extract(instr_int, 4, 0);
            uint64_t t2 = extract(instr_int, 14, 10);
            uint64_t offset = sign_extend_32(extract32(instr_int, 21, 15), 7) << 2;
            uint64_t address = reg[n];
            address = address + offset;
            uint32_t data = memory_get_32(address);
            uint32_t data2 = memory_get_32(address + 4);
            if ( t != 31 ) {
                reg[t] = data;
            }
            if ( t2 != 31 ) {
                reg[t2] = data2;
            }
            reg[n] = address;
        }
        else if ( is_instruction(instr_int, 0x29400000) ) {
            // LDP signed offset 32-bit
            uint64_t n = extract(instr_int, 9, 5);
            uint64_t t = extract(instr_int, 4, 0);
            uint64_t t2 = extract(instr_int, 14, 10);
            uint64_t offset = sign_extend_64(extract(instr_int, 21, 15), 7) << 3;
            uint64_t address = reg[n];
            address = address + offset;
            uint32_t data = memory_get_32(address);
            uint32_t data2 = memory_get_32(address + 8);
            if ( t != 31 ) {
                reg[t] = data;
            }
            if ( t2 != 31 ) {
                reg[t2] = data2;
            }
        }
        else if ( is_instruction(instr_int, 0x29000000) ) {
            // STP signed offset 32-bit
            uint64_t n = extract(instr_int, 9, 5);
            uint64_t t = extract(instr_int, 4, 0);
            uint64_t t2 = extract(instr_int, 14, 10);
            uint64_t offset = sign_extend_32(extract32(instr_int, 21, 15), 7) << 2;
            uint64_t address = reg[n];
            address = address + offset;
            uint64_t data = t == 31 ? 0 : reg[t];
            uint64_t data2 = t2 == 31 ? 0 : reg[t2];
            memory_set_32(address, data);
            memory_set_32(address + 4, data2);
        } else if ( is_instruction(instr_int, 0x2b000000) ) {
            // ADDS shifted register 32
            // cout << "ADD shifted register 32" << endl;
            uint32_t d = extract32(instr_int, 4, 0);
            uint32_t n = extract32(instr_int, 9, 5);
            uint32_t m = extract32(instr_int, 20, 16);
            uint32_t imm = extract32(instr_int, 15, 10);
            // shift = 0 => LSL
            // shift = 1 => LSR
            // shift = 2 => ASR
            // shift = 3 => ROR
            uint32_t shift_type = extract32(instr_int, 23, 22);
            uint32_t shift_amt = imm;
            uint32_t operand1 = 0;
            if ( n != 31 ) {
                operand1 = reg[n];
            }
            uint32_t operand2 = 0;
            if ( m != 31 ) {
                operand2 = reg[m];
            }
            operand2 = shift_reg32(operand2, shift_type, shift_amt);
            uint32_t result = 0;
            uint8_t nzcv_temp = 0;
            add_with_carry32(operand1, operand2, 0, &result, &nzcv_temp);
            nzcvLocal = nzcv_temp;
            if ( d != 31 ) {
                reg[d] = result;
            }
        } else if ( is_instruction(instr_int, 0x28c00000) ) {
            // LDP post-index 32-bit
            uint64_t n = extract(instr_int, 9, 5);
            uint64_t t = extract(instr_int, 4, 0);
            uint64_t t2 = extract(instr_int, 14, 10);
            uint64_t offset = sign_extend_64(extract(instr_int, 21, 15), 7) << 2;
            uint64_t address = reg[n];
            uint64_t data = memory_get_64(address);
            uint64_t data2 = memory_get_64(address + 4);
            if ( t != 31 ) {
                reg[t] = data;
            }
            if ( t2 != 31 ) {
                reg[t2] = data2;
            }
            address = address + offset;
            reg[n] = address;
        } else if ( is_instruction(instr_int, 0x28800000) ) {
            // STP post-index 32
            uint64_t n = extract(instr_int, 9, 5);
            uint64_t t = extract(instr_int, 4, 0);
            uint64_t t2 = extract(instr_int, 14, 10);
            uint64_t offset = sign_extend_32(extract32(instr_int, 21, 15), 7) << 2;
            uint64_t address = reg[n];
            uint64_t data = t == 31 ? 0 : reg[t];
            uint64_t data2 = t2 == 31 ? 0 : reg[t2];
            memory_set_32(address, data);
            memory_set_32(address + 4, data2);
            address = address + offset;
            reg[n] = address;
        } else if ( is_instruction(instr_int, 0x1a800400) ) {
            // CSINC 32
            uint32_t d = extract(instr_int, 4, 0);
            uint32_t n = extract(instr_int, 9, 5);
            uint32_t m = extract(instr_int, 20, 16);
            uint8_t fullcond = extract32(instr_int, 15, 12);
            uint32_t result = 0;
            uint32_t operand1 = n == 31 ? 0 : reg[n];
            uint32_t operand2 = m == 31 ? 0 : reg[m];
            bool condHolds = conditionHolds(fullcond, nzcvLocal);
            if ( condHolds ) {
                result = operand1;
            } else {
                result = operand2 + 1;
            }
            if ( d != 31 ) {
                reg[d] = result;
            }
        } else if( is_instruction(instr_int, 0x1a800000) ){
            //CSEL 32
            uint64_t d = extract(instr_int, 4, 0);
            uint64_t n = extract(instr_int, 9, 5);
            uint64_t cond = extract(instr_int, 15, 12);
            uint64_t m = extract(instr_int, 20, 16);
            uint32_t result = 0;
            uint32_t operand1 = n == 31 ? 0 : reg[n];
            uint32_t operand2 = m == 31 ? 0 : reg[m];

            if(conditionHolds((uint8_t)cond, nzcvLocal))
              result = operand1;
            else
              result = operand2;
            if ( d != 31 ) {
                reg[d] = result;
            }
        } else if ( is_instruction(instr_int, 0x18000000) ) {
            // LDR (literal) 32-bit
            uint64_t imm = extract(instr_int, 23, 5);
            uint64_t t = extract(instr_int, 4, 0);
            uint64_t offset = sign_extend_32(imm << 2, 21);
            uint64_t address = pcLocal + offset;
            uint32_t data = memory_get_32(address);
            if ( t != 31 ) {
                reg[t] = data;
            }
        } else if ( is_instruction(instr_int, 0x14000000) ) {
            // B
            // cout << "B" << endl;
            uint64_t imm = extract(instr_int, 25, 0);
            uint64_t offset = sign_extend_64(imm << 2, 28);
            pcChange = true;
            pcLocal = pcLocal + offset;
        } else if ( is_instruction(instr_int, 0x12800000) ) {
            // MOVN 32
            // cout << "MOVN 32" << endl;
            uint32_t d = extract32(instr_int, 4, 0);
            uint32_t pos = extract32(instr_int, 22, 21);
            uint32_t imm = extract32(instr_int, 20, 5);
            pos = pos << 4;
            uint32_t result = imm << pos;
            result = ~result;
            if ( d != 31 ) {
                reg[d] = result;
            }
        } else if ( is_instruction(instr_int, 0x12000000) ) {
            // AND immediate 32
            // cout << "AND immediate 32" << endl;
            uint32_t N = extract_single32(instr_int, 22);
            uint32_t d = extract32(instr_int, 4, 0);
            uint32_t n = extract32(instr_int, 9, 5);
            uint32_t immr = extract32(instr_int, 21, 16);
            uint32_t imms = extract32(instr_int, 15, 10);
            uint64_t imm = 0;
            uint64_t nah = 0; // throwaway value
            decode_bit_masks(N, imms, immr, true, &imm, &nah);
            uint32_t operand1 = 0;
            if ( n != 31 ) {
                operand1 = reg[n];
            }
            reg[d] = set_reg_32(reg[d], operand1 & imm);
        } else if ( is_instruction(instr_int, 0x11000000) ) {
            // ADD immediate 32
            // cout << "ADD immediate 32" << endl;
            uint64_t d = extract(instr_int, 4, 0);
            uint64_t n = extract(instr_int, 9, 5);
            uint64_t imm = extract(instr_int, 21, 10);
            uint64_t shift = extract(instr_int, 23, 22);
            uint64_t source_val = reg[n];
            if ( shift == 1 ) {
                imm = imm << 12;
            }
            uint32_t result = source_val + imm;
            reg[d] = result;
        } else if ( is_instruction(instr_int, 0xb000000) ) {
            // ADD shifted register 32
            // cout << "ADD shifted register 32" << endl;
            uint32_t d = extract32(instr_int, 4, 0);
            uint32_t n = extract32(instr_int, 9, 5);
            uint32_t m = extract32(instr_int, 20, 16);
            uint32_t imm = extract32(instr_int, 15, 10);
            // shift = 0 => LSL
            // shift = 1 => LSR
            // shift = 2 => ASR
            // shift = 3 => ROR
            uint32_t shift_type = extract32(instr_int, 23, 22);
            uint32_t shift_amt = imm;
            uint32_t operand1 = 0;
            if ( n != 31 ) {
                operand1 = reg[n];
            }
            uint32_t operand2 = 0;
            if ( m != 31 ) {
                operand2 = reg[m];
            }
            operand2 = shift_reg32(operand2, shift_type, shift_amt);
            uint32_t result = operand1 + operand2;
            // uint8_t nzcv_temp = 0; // useless
            // add_with_carry32(operand1, operand2, 0, &result, &nzcv_temp);
            if ( d != 31 ) {
                reg[d] = result;
            }
        }  else if( is_instruction(instr_int, 0xa200000) ){
            //bic 32
            //BIC 32
            uint64_t d = extract(instr_int, 4, 0);
            uint64_t n = extract(instr_int, 9, 5);
            uint64_t m = extract(instr_int, 20, 16);

            uint64_t shift_amount = extract(instr_int, 10, 15);
            uint64_t shift_type = extract(instr_int, 23, 22);

            //uint64_t datasize = 32;

            uint32_t operand1 = 0;
            if(n != 31){
                operand1 = reg[n];
            }

            uint32_t operand2 = shift_reg32(m == 31 ? 0 : reg[m], shift_type, shift_amount);
            operand2 = ~operand2;
            uint32_t result = operand1 & operand2;
            if(d != 31){
                reg[d] = result;
            }
        } else if( is_instruction(instr_int, 0xa000000) ){
            //and shifted register 32
            //AND SHIFTED REGISTER 32
            uint64_t d = extract(instr_int, 4, 0);
            uint64_t n = extract(instr_int, 9, 5);
            uint64_t m = extract(instr_int, 20, 16);
            uint64_t shift_type = extract(instr_int, 23, 22);
            uint64_t shift_amount = extract(instr_int, 15, 10);

            uint32_t operand1 = 0;
            if(n != 31){
              operand1 = reg[n];
            }
            uint32_t operand2 = shift_reg32(m == 31 ? 0 : reg[m], shift_type, shift_amount);
            uint32_t result = operand1 & operand2;
            if(d != 31){
              reg[d] = result;
            }
        } else {
            allTerminated = termin(pcLocal, instr_int, reg, simd_reg, debug);
        }
        /*
        printf("instruction %x at address %lx\n", instr_int, pcLocal);
        for ( int i = 0; i < 32; i++ ) {
            unsigned long val = reg[i];
            if ( i == 31 ) {
                printf("XSP : %016lX\n", val);
            } else {
                printf("X%02d : %016lX\n", i, val);
            }
        }
        */
        if ( !pcChange ) {
            pcLocal += 4;
        }
    }
    return 0;
}
