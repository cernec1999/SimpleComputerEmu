/**
 *  Register storage
 */
enum {
    R0,
    R1,
    R2,
    R3,
    R4,
    R5,
    R6,
    R7,
    PC,
    IR,
    STATUS,
    R_COUNT
};


/**
 *  Status bits
 */
enum {
    // Overflow
    FL_V = 1 << 3,
    // Zero
    FL_Z = 1 << 2,
    // Carry
    FL_C = 1 << 1,
    // Negative
    FL_N = 1 << 0
};


/**
 *  65536 memory locations
 */
uint16_t memory[UINT16_MAX];


/**
 *  Instruction memory
 */
uint16_t instr[UINT16_MAX];


/**
 *  Register storage
 */
uint16_t reg[R_COUNT];


/**
 *  Single bit control flow variables
 */
bool BC, JB, PL, MW, RW, MD, MB;

/**
 *  Function select
 */
unsigned int FS;

/**
 *  Registers
 */
uint16_t BA, AA, DA;

/*
 *  Reset
 */
bool reset = true;
bool running = true;

uint16_t get_bit_range (uint16_t num, int high, int low);
bool get_bit (uint16_t num, int bit);
int read_file_write_mem (uint16_t arr[], char *file);
void branch_control();
void instruction_decoder();
void print_decoder_output();
void print_memory(uint16_t);
void print_reg();
uint16_t function_unit(uint16_t operand_A, uint16_t operand_B);
