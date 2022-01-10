//  CITS2002 Project 1 2021
//  Name(s):             Baxter-Cooper, Khan-Huyhn
//  Student number(s):   22966805, 22976648

//  compile with:  cc -std=c11 -Wall -Werror -o runcool runcool.c

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

//  THE STACK-BASED MACHINE HAS 2^16 (= 65,536) WORDS OF MAIN MEMORY
#define N_MAIN_MEMORY_WORDS (1<<16)

//  EACH WORD OF MEMORY CAN STORE A 16-bit UNSIGNED value1 (0 to 65535)
#define AWORD               uint16_t
//  OR STORE A 16-bit SIGNED INTEGER (-32,768 to 32,767)
#define IWORD               int16_t

//  THE ARRAY OF 65,536 WORDS OF MAIN MEMORY
AWORD                       main_memory[N_MAIN_MEMORY_WORDS];

//  THE SMALL-BUT-FAST CACHE HAS 32 WORDS OF MEMORY
#define N_CACHE_WORDS       32


//  see:  https://teaching.csse.uwa.edu.au/units/CITS2002/projects/coolinstructions.php
enum INSTRUCTION {
    I_HALT       = 0,
    I_NOP,
    I_ADD,
    I_SUB,
    I_MULT,
    I_DIV,
    I_CALL,
    I_RETURN,
    I_JMP,
    I_JEQ,
    I_PRINTI,
    I_PRINTS,
    I_PUSHC,
    I_PUSHA,
    I_PUSHR,
    I_POPA,
    I_POPR
};

//  USE VALUES OF enum INSTRUCTION TO INDEX THE INSTRUCTION_name[] ARRAY
const char *INSTRUCTION_name[] = {
    "halt",
    "nop",
    "add",
    "sub",
    "mult",
    "div",
    "call",
    "return",
    "jmp",
    "jeq",
    "printi",
    "prints",
    "pushc",
    "pusha",
    "pushr",
    "popa",
    "popr"
};

//  ----  IT IS SAFE TO MODIFY ANYTHING BELOW THIS LINE  --------------


//  THE STATISTICS TO BE ACCUMULATED AND REPORTED
int n_main_memory_reads     = 0;
int n_main_memory_writes    = 0;
int n_cache_memory_hits     = 0;
int n_cache_memory_misses   = 0;

void report_statistics(void)
{
    printf("@number-of-main-memory-reads\t%i\n",    n_main_memory_reads);
    printf("@number-of-main-memory-writes\t%i\n",   n_main_memory_writes);
    printf("@number-of-cache-memory-hits\t%i\n",    n_cache_memory_hits);
    printf("@number-of-cache-memory-misses\t%i\n",  n_cache_memory_misses);
}

//  -------------------------------------------------------------------

//  EVEN THOUGH main_memory[] IS AN ARRAY OF WORDS, IT SHOULD NOT BE ACCESSED DIRECTLY.
//  INSTEAD, USE THESE FUNCTIONS read_memory() and write_memory()
//
//  THIS WILL MAKE THINGS EASIER WHEN WHEN EXTENDING THE CODE TO
//  SUPPORT CACHE MEMORY

struct {
    int valid;
    int location;
    int dirty;
    int data;
} write_back_cache[N_CACHE_WORDS];

// FOR READING UNSIGNED VALUES (NEEDED FOR VALUES > 32,767)
AWORD read_memory_AWORD(AWORD value1)
{
	int slot = value1 % N_CACHE_WORDS; // get cache slot

    // CHECK IF SLOT IS VALID, IF NOT (MISS) GET VALUE FROM MAIN MEMORY & STORE IN CACHE
    if (write_back_cache[slot].valid == false) { 
        write_back_cache[slot].valid = true; // cache slot used
        write_back_cache[slot].location = value1; // add the address to the cache 
        write_back_cache[slot].dirty = false; // cache is in sync
        write_back_cache[slot].data = main_memory[value1]; // add the data into the cache 
        ++ n_cache_memory_misses;
        ++ n_main_memory_reads;

        return write_back_cache[slot].data;
    }

    // ELSE IF VALID, CHECK LOCATION VALUE, IF LOCATION MATCHES VALUE1 (HIT) 
    if (write_back_cache[slot].location == value1) {
        ++ n_cache_memory_hits;
        return write_back_cache[slot].data;
    }

    // ELSE IF LOCATION DOES NOT MATCH, CHECK IF DIRTY, IF DIRTY WRITE CACHE MEMORY TO MAIN MEMORY LOCATION
    if (write_back_cache[slot].dirty == true) {
        main_memory[write_back_cache[slot].location] = write_back_cache[slot].data; // copy cache to memory
        ++ n_main_memory_writes;
    }

    // NOW THAT CACHE IS IN SYNC, WRITE DATA TO CACHE
    write_back_cache[slot].location = value1; // add the address to cache
    write_back_cache[slot].dirty = false; // cache in sync
    write_back_cache[slot].data = main_memory[value1]; // copy data from main memory to cache
    ++ n_cache_memory_misses;
    ++ n_main_memory_reads;

    return write_back_cache[slot].data;
}

// FOR READING SIGNED VALUES (NEEDED FOR NEGATIVE)
IWORD read_memory_IWORD(AWORD value1)
{
	int slot = value1 % N_CACHE_WORDS; // get cache slot

    // CHECK IF SLOT IS VALID, IF NOT (MISS) GET VALUE FROM MAIN MEMORY & STORE IN CACHE
    if (write_back_cache[slot].valid == false) { 
        write_back_cache[slot].valid = true; // cache slot used
        write_back_cache[slot].location = value1; // add the address to the cache 
        write_back_cache[slot].dirty = false; // cache is in sync
        write_back_cache[slot].data = main_memory[value1]; // add the data into the cache 
        ++ n_cache_memory_misses;
        ++ n_main_memory_reads;

        return write_back_cache[slot].data;
    }

    // ELSE IF VALID, CHECK LOCATION VALUE, IF LOCATION MATCHES VALUE1 (HIT) 
    if (write_back_cache[slot].location == value1) {
        ++ n_cache_memory_hits;
        return write_back_cache[slot].data;
    }

    // ELSE IF LOCATION DOES NOT MATCH, CHECK IF DIRTY, IF DIRTY WRITE CACHE MEMORY TO MAIN MEMORY LOCATION
    if (write_back_cache[slot].dirty == true) {
        main_memory[write_back_cache[slot].location] = write_back_cache[slot].data; // copy cache to memory
        ++ n_main_memory_writes;
    }

    // NOW THAT CACHE IS IN SYNC, WRITE DATA TO CACHE
    write_back_cache[slot].location = value1; // add the address to cache
    write_back_cache[slot].dirty = false; // cache in sync
    write_back_cache[slot].data = main_memory[value1]; // copy data from main memory to cache
    ++ n_cache_memory_misses;
    ++ n_main_memory_reads;

    return write_back_cache[slot].data;
}

// FOR READING UNSIGNED VALUES (NEEDED FOR VALUES > 32,767)
void write_memory_AWORD(AWORD value1, AWORD value)
{   
	int slot = value1 % N_CACHE_WORDS; // get cache slot

    // IF CACHE SLOT NOT IN SYNC, COPY CACHE TO MEMEORY
    if (write_back_cache[slot].dirty == true) {
        main_memory[write_back_cache[slot].location] = write_back_cache[slot].data; // copy cache to memory 
        ++ n_main_memory_writes;
    }

    // WRITE DATA TO CACHE
    write_back_cache[slot].valid = true; // cache is valid 
    write_back_cache[slot].location = value1; // add the address to the cache 
    write_back_cache[slot].dirty = true; // cache is out of sync 
    write_back_cache[slot].data = value; // add the data into the cache slot
}

// FOR WRITING SIGNED VALUES (NEEDED FOR NEGATIVE VALUES)
void write_memory_IWORD(AWORD value1, IWORD value)
{   
	int slot = value1 % N_CACHE_WORDS; // get cache slot

    // IF CACHE SLOT NOT IN SYNC, COPY CACHE TO MEMEORY
    if (write_back_cache[slot].dirty == true) {
        main_memory[write_back_cache[slot].location] = write_back_cache[slot].data; // copy cache to memory 
        ++ n_main_memory_writes;
    }

    // WRITE DATA TO CACHE
    write_back_cache[slot].valid = true; // cache is valid 
    write_back_cache[slot].location = value1; // add the address to the cache 
    write_back_cache[slot].dirty = true; // cache is out of sync 
    write_back_cache[slot].data = value; // add the data into the cache slot
}

//  -------------------------------------------------------------------

//  EXECUTE THE INSTRUCTIONS IN main_memory[]
int execute_stackmachine(void)
{
//  THE 3 ON-CPU CONTROL REGISTERS:
    int PC      = 0;                    // 1st instruction is at value1=0
    int SP      = N_MAIN_MEMORY_WORDS;  // initialised to top-of-stack
    int FP      = 0;                    // frame pointer

    while(true) {
		IWORD	value1;
		IWORD 	value2;
//  FETCH THE NEXT INSTRUCTION TO BE EXECUTED

        IWORD instruction = read_memory_AWORD(PC);
        ++PC;

        printf("%s\n", INSTRUCTION_name[instruction]);

        if(instruction == I_HALT) {
            break;
        }

//  SUPPORT OTHER INSTRUCTIONS HERE
//      ....
        switch(instruction) {
            case I_NOP: 
                break;
            case I_ADD: 
                value1 = read_memory_IWORD(SP); // value at TOS 
                ++SP;
                value2 = read_memory_IWORD(SP); // value at TOS 
                write_memory_IWORD(SP, value2 + value1); // value + value to TOS 
                break;
            case I_SUB:
                value1 = read_memory_IWORD(SP); // value at TOS 
                ++SP;
                value2 = read_memory_IWORD(SP); // value at TOS 
                write_memory_IWORD(SP, value2 - value1); // value - value to TOS
                break;
            case I_MULT:
                value1 = read_memory_IWORD(SP); // value at TOS 
                ++SP;
                value2 = read_memory_IWORD(SP); // value at TOS 
                write_memory_IWORD(SP, value2 * value1); // write value2 * value1 to TOS
                break;
            case I_DIV:
                value1 = read_memory_IWORD(SP); // value at TOS
                ++SP; 
                value2 = read_memory_IWORD(SP); // value at TOS
                write_memory_IWORD(SP, value2 / value1); // write value / value to TOS
                break;
            case I_CALL:
                value1 = read_memory_AWORD(PC); // function address
                value2 = PC + 1; // return address
                PC = value1; // execution continues at function address 
                --SP; 
                write_memory_AWORD(SP, value2); // write return address to TOS 
                --SP;
                write_memory_AWORD(SP, FP); // write current FP address to TOS
                FP = SP; // FP now becomes address of TOS
                break;
            case I_RETURN: 
                value1 = read_memory_IWORD(SP); // function return value
                value2 = read_memory_IWORD(PC); // FP offset for return value to be pushed to
                PC = read_memory_AWORD(FP + 1); // execution to continue at return address
                write_memory_IWORD(FP + value2, value1); // Write to FP + FP offset, function return value
                SP = FP + value2; // TOS returns to FP + FP offset (address of where return value has been pushed)
                FP = read_memory_AWORD(FP); // FP returns to previous value 
                break; 
            case I_JMP:
                value1 = read_memory_AWORD(PC); // next instruction address
                PC = value1; // execution continues at instruction address
                break;
            case I_JEQ:
                value1 = read_memory_IWORD(SP); // value at TOS
                ++SP;
                if (value1 == 0) { // check where the value is 0
                    value2 = read_memory_AWORD(PC); // next instruction address
                    PC = value2; // execution continues at instruction address
                } else {
                    PC += 1; // skip next instruction address 
                }
                break;
            case I_PRINTI:
                value1 = read_memory_IWORD(SP); // integer to be printed
                printf("%i", value1); // print integer
                break;
            case I_PRINTS:
                value1 = read_memory_AWORD(PC); // address where string starts
                ++PC;
                int j = 0; // end of string pointer
                char string[BUFSIZ]; // initialise char array to hold characters
                for (int i = value1 - 1; i++;) { // for word starting at start of string
                    IWORD chars = read_memory_AWORD(i); // 16bit variable to hold characters (each word makes up 2 characters)
                    char char1 = (chars << 8) >> 8; // takes first 8 bits in word for first character
                    char char2 = chars >> 8; // takes last 8 bits in word for second character

                    if (char1 == '\0') { // if character is nullbyte, end of string has been reached
                        string[j] = '\0'; // adds nullbyte to end of string to show end of string
                        printf("%s", string); // print string array
                        break;
                    }
                    string[j] = char1; // add character to array
                    j++;
                    if (char2 == '\0') {
                        string[j] = '\0';
                        printf("%s", string);
                        break;
                    }
                    string[j] = char2;
                    j++;
                }
                break;
            case I_PUSHC:
                value1 = read_memory_IWORD(PC); // value to be pushed to TOS
                ++PC;
                --SP;
                write_memory_IWORD(SP, value1); // write value to TOS
                break;
            case I_PUSHA:
                value1 = read_memory_AWORD(PC); // address of value to be pushed to TOS
                ++PC;
                value2 = read_memory_IWORD(value1); // value to be pushed to TOS
                --SP;
                write_memory_IWORD(SP, value2); // write value to TOS
                break;
            case I_PUSHR:
                value1 = read_memory_IWORD(PC); // FP offset
                ++PC;
                value2 = read_memory_IWORD(FP + value1); // value to be pushed to TOS
                --SP;
                write_memory_IWORD(SP, value2); // write value to TOS
                break;
            case I_POPA:
                value1 = read_memory_AWORD(PC); // pop address
                ++PC;
                value2 = read_memory_IWORD(SP); // value at TOS
                write_memory_IWORD(value1, value2); // write to pop address, value at TOS
                ++SP;
                break;
            case I_POPR:
                value1 = read_memory_IWORD(PC); // FP offset
                ++PC;
                value2 = read_memory_IWORD(SP); // value at TOS
                write_memory_IWORD(FP + value1, value2); // write to FP + offset, value at TOS
                --SP; 
                break;
        }
    }

//  THE RESULT OF EXECUTING THE INSTRUCTIONS IS FOUND ON THE TOP-OF-STACK
	return read_memory_IWORD(SP);
}

//  -------------------------------------------------------------------

//  READ THE PROVIDED coolexe FILE INTO main_memory[]
void read_coolexe_file(char filename[])
{
    memset(main_memory, 0, sizeof main_memory);   //  clear all memory

//  READ CONTENTS OF coolexe FILE
	FILE *fp = fopen(filename, "rb"); // open file as bytes

//  ENSURE THAT OPENING FILE HAS BEEN SUCCESSFUL
    if(fp == NULL) {
        printf("cannot open '%s'\n", filename);
        exit(EXIT_FAILURE);
    }

//  COPY WORDS FROM FILE TO MAIN MEMORY
    if(fp != NULL) { // if file open successful
        IWORD buffer[BUFSIZ]; // use IWORD array to hold signed 16bit words
        size_t got; // variable to hold number of words read

        got = fread(buffer, 2, sizeof buffer, fp); // read file and get number of words
        for (int i=0; i < got; i++) {
            main_memory[i] = buffer[i]; // store word into main memory
        }

    }

//  ENSURE THAT WE ONLY CLOSE FILES THAT ARE OPEN
    if(fp != NULL) {
        fclose(fp); // close file
    }
}

//  -------------------------------------------------------------------

int main(int argc, char *argv[])
{
//  CHECK THE NUMBER OF ARGUMENTS
    if(argc != 2) {
        fprintf(stderr, "Usage: %s program.coolexe\n", argv[0]);
        exit(EXIT_FAILURE);
    }

//  READ THE PROVIDED coolexe FILE INTO THE EMULATED MEMORY
    read_coolexe_file(argv[1]);

//  EXECUTE THE INSTRUCTIONS FOUND IN main_memory[]
    int result = execute_stackmachine();

    report_statistics();
    printf("exit(%i)\n", result);
    return result;          // or  exit(result);
}
