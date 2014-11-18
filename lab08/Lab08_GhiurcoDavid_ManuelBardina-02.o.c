// Bardina_Manuel, Ghiurco_David
// CS 350, Fall 2014
// Lab 7: SDC Simulator
//
// Illinois Institute of Technology, (c) 2014, James Sasaki

#include <stdio.h>
#include <stdlib.h>       // For error exit()

// CPU Declarations -- a CPU is a structure with fields for the
// different parts of the CPU.
//
typedef short int Word;          // type that represents a word of SDC memory
typedef unsigned short int Address;   // type that represents an SDC address

#define MEMLEN 65536
#define NREG 8

typedef struct
{
    Word mem[MEMLEN];
    Word reg[NREG];      // Note: "register" is a reserved word
    Address pc;
    int cc;          // Program Counter
    int running;         // running = 1 iff CPU is executing instructions
    Word ir;             // Instruction Register
    int instr_sign;
    int opcode;
    int reg_R;
    int addr_MM;
} CPU;


// Prototypes [note the functions are also declared in this order]
//
int main(int argc, char *argv[]);
void initialize_control_unit(CPU *cpu);
void initialize_memory(int argc, char *argv[], CPU *cpu);
FILE *get_datafile(int argc, char *argv[]);

void dump_control_unit(CPU *cpu);
void dump_memory(CPU *cpu);
void dump_registers(CPU *cpu);

int read_execute_command(CPU *cpu);
int execute_command(char cmd_char, CPU *cpu);
void help_message(void);
void many_instruction_cycles(int nbr_cycles, CPU *cpu);
void one_instruction_cycle(CPU *cpu);
void exec_HLT(CPU *cpu);

// Main program: Initialize the cpu, read initial memory values,
// and execute the read-in program starting at location 00.
//
int main(int argc, char *argv[])
{
    printf("LC3 Simulator skeleton: CS 350 Lab 7\n");
    CPU cpu_value, *cpu = &cpu_value;
    initialize_control_unit(cpu);
    printf("pass cpu init\n"); 
    initialize_memory(argc, argv, cpu);
    printf("pass mem init\n");
    dump_control_unit(cpu);
    dump_memory(cpu);
    char *prompt = "> ";
    printf("\nBeginning execution; type h for help\n%s", prompt);

    int done = read_execute_command(cpu);
    while (!done)
    {
        printf("%s", prompt);
        done = read_execute_command(cpu);
    }
    return 0;
}


// Initialize the control registers (pc, ir, running flag) and
// the general-purpose registers
//
void initialize_control_unit(CPU *cpu)
{
    (*cpu).pc = 0;
    (*cpu).ir = 0;
    (*cpu).running = 1;
    int i;
    for (i = 0; i < NREG; i++)
    {
        cpu->reg[i] = 0;
    }
        printf("\nInitial control unit:\n");
        dump_control_unit(cpu);
        printf("\n");
    
}
// Read and dump initial values for memory
//
    void initialize_memory(int argc, char *argv[], CPU *cpu)
    {
           FILE *datafile = get_datafile(argc, argv);

        // Will read the next line (words_read = 1 if it started
        // with a memory value). Will set memory location loc to
        // value_read
        //
           int value_read, words_read, loc = 0, done = 0;
        // Each getline automatically reallocates buffer and
        // updates buffer_len so that we can read in the whole line
        // of input. bytes_read is 0 at end-of-file.  Note we must
        // free the buffer once we're done with this file.
        //
        // See linux command man 3 getline for details.
        //
        char *buffer = NULL;
        size_t buffer_len = 0, bytes_read = 0;

        // Read in first and succeeding memory values. Stop when we
        // hit a sentinel value, fill up memory, or hit end-of-file.
        //
 


	
       bytes_read = getline(&buffer, &buffer_len, datafile);
        while (bytes_read != -1 && !done)
        {
            // If the line of input begins with an integer, treat
            // it as the memory value to read in.  Ignore junk
            // after the number and ignore blank lines and lines
            // that don't begin with a number.
            //
            words_read = sscanf(buffer, "%x", &value_read);


            if(words_read == 1)
            {
                if(loc >= MEMLEN  || loc < 0)
                {
                    printf("location is out of range \n");
                    break;
                }
                else if(value_read < -9999 || value_read > 9999)
                {
                    printf("sentinel %d found at location %d\n", value_read, loc);
                    break;
                }
                else (*cpu).mem[loc++] = value_read;
            }
            // *** STUB *** set memory value at current location to
            // value_read and increment location.  Exceptions: If
            // loc is out of range, complain and quit the loop. If
            // value_read is outside -9999...9999, then it's a
            // sentinel and we should say so and quit the loop.

            // Get next line and continue the loop
            //
            bytes_read = getline(&buffer, &buffer_len, datafile);
        }
       
	     free(buffer);  // return buffer to OS




        // Initialize rest of memory
        //
      while (loc < MEMLEN-1)
	 {
	  cpu -> mem[loc++] = 0;
	 }
        dump_memory(cpu);
    }

// Get the data file to initialize memory with.  If it was
// specified on the command line as argv[1], use that file
// otherwise use default.sdc.  If file opening fails, complain
// and terminate program execution with an error.
// See linux command man 3 exit for details.
//
    FILE *get_datafile(int argc, char *argv[])
    {
        char *default_datafile_name = "program.hex";
        char *datafile_name;

        if(!argv[1])
            datafile_name = default_datafile_name;
        else
            datafile_name = argv[1];
	//         *datafile_nameglobe=*datafile_name;
        FILE *datafile = fopen(datafile_name, "r");
        if(!datafile)
        {
            printf("File failed to open \n");
            exit(EXIT_FAILURE);

            // exit(EXIT_FAILURE); to quit the entire program

            return datafile;
        }
}
// dump_control_unit(CPU *cpu): Print out the control and
// general-purpose registers
//
        void dump_control_unit(CPU *cpu)
        {
            printf("pc :  %02d   IR :  %04d  cc :  %02d  RUNNING : %d\n", (*cpu).pc, (*cpu).ir, (*cpu).cc, (*cpu).running);
            dump_registers(cpu);
        }

// dump_memory(CPU *cpu): Print memory values in a table, ten per
// row, with a space between each group of five columns and with
// a header column of memory locations.
//
        void dump_memory(CPU *cpu)
        {
            int loc = 0;
            int row, col, i;
            // ***  ****
//            for (row = 0; row < 100; row +=10)
//            {
//                printf("%02d:",row);
//                for(col=row; col < row +10; col++)
//                {
//                    if(col%10 == 5) printf(" ");
//                    printf("%6d",(*cpu).mem[col]);
//                }
//                printf("\n");
//            }
    for (i = 0; i < MEMLEN-1; i++)
    {
      //        while((*cpu).mem[i] == 0) i++;
      if ((*cpu).mem[i] != 0)
	{
	col ++;
        if (col > 10) {col = 0; printf("\n");}
        if (col%10 == 5) printf(" ");
        printf("%4x",(*cpu).mem[i]);
	}
    }
        }

// dump_registers(CPU *cpu): Print register values in two rows of
// five.
//
        void dump_registers(CPU *cpu)
        {
            // *** ****
            int i;
            for (i = 0; i < NREG; i++)
            {
                if(i%4 == 0) printf("\n");
                printf("R%d :%4x  ", i, (*cpu).reg[i]);
            }

        }

// Read a simulator command from the keyboard ("h", "?", "d", number,
// or empty line) and execute it.  Return true if we hit end-of-input
// or execute_command told us to quit.  Otherwise return false.
//
        int read_execute_command(CPU *cpu)
        {
            // Buffer for the command line from the keyboard, plus its size
            //
            char *cmd_buffer = NULL;
            size_t cmd_buffer_len = 0, bytes_read = 0;

            // Values read using sscanf of command line
            //
            int nbr_cycles;
            char cmd_char;
            size_t words_read;    // number of items read by sscanf call

            int done = 0;	// Should simulator stop?

            bytes_read = getline(&cmd_buffer, &cmd_buffer_len, stdin);
            if (bytes_read == -1)
            {
                done = 1;   // Hit end of file
            }

            words_read = sscanf(cmd_buffer, "%d", &nbr_cycles);
            // *** ****  If we found a number, do that many
            if (words_read == 1)
                many_instruction_cycles(nbr_cycles, cpu);
            else
            {
                words_read = sscanf(cmd_buffer, "%c", &cmd_char);
                done = execute_command(cmd_char, cpu);
                // instruction cycles.  Otherwise sscanf for a character
                // and call execute_command with it.  (Note the character
                // might be '\n'.)

                free(cmd_buffer);
                return done;
            }
        }

// Execute a nonnumeric command; complain if it's not 'h', '?', 'd', 'q' or '\n'
// Return true for the q command, false otherwise
//
            int execute_command(char cmd_char, CPU *cpu)
            {
                if (cmd_char == '?' || cmd_char == 'h')
                {
                    help_message();
                }
                else if(cmd_char=='q')
                {
                    printf("Quit \n");
                    return 1;
                }
                else if(cmd_char =='d')
                {
                    dump_control_unit(cpu);
                    dump_memory(cpu);
                }
                else if (cmd_char == '\n')
                {
                    printf("CPU halted\n");
                }
                else
                    printf("%c not a valid command \n",cmd_char);
                return 0;
            }

// Print standard message for simulator help command ('h' or '?')
//
            void help_message(void)
            {
                printf("h or ? for help (prints this message)\n");
                printf("q to quit\n");
                printf("d to dump the control unit and mammary\n");
                printf("\nNot Suppoerted yet:\n");
                printf("j xNNNN to jump to new location\n");
                printf("m xNNNNN xMMMM to assign memory location xNNNN = value xMMMM\n");
                printf("An integer >0 to execute that many instruction cycles\n");
                printf("Or just return, which executes one instruction cycle\n");
            }

// Execute a number of instruction cycles.  Exceptions: If the
// number of cycles is <= 0, complain and return; if the CPU is
// not running, say so and return; if the number of cycles is
// insanely large, complain and substitute a saner limit.
//
// If, as we execute the many cycles, the CPU stops running,
// then return.
//
            void many_instruction_cycles(int nbr_cycles, CPU *cpu)
            {
               if(nbr_cycles <= 0) {
				printf("Number of instruction cycles > 0\n");
				return;
			}
	if((*cpu).running == 0)
	{
		printf("CPU has been halted\n");
		return;
	}
	if(nbr_cycles > 100)
	{
		printf("%d is too large for cycle; doing %d \n",nbr_cycles,MEMLEN);
	}
	int count;
	for(count = 0; count < nbr_cycles; count++)
	{
		if((*cpu).running == 0) break;

		one_instruction_cycle(cpu);
	}
	return;
            }

// Execute one instruction cycle
//



//*   temporary commented

            void one_instruction_cycle(CPU *cpu)
            {
                // If the CPU isn't running, say so and return.
                // If the pc is out of range, complain and stop running the CPU.
                if((*cpu).running == 0)
					{
						printf("CPU has been Halted\n");
						return;
					}
				if((*cpu).pc >= MEMLEN)
					{
						printf("pc is out of range.\n");
						cpu->running = 0;
						return;
					}

                // Get instruction and increment pc
                //
                int instr_loc = cpu -> pc;  // Instruction's location (pc before increment)
                cpu -> ir = cpu -> mem[cpu -> pc++];

                // Decode instruction into opcode, reg_R, addr_MM, and instruction sign
                // *** STUB ****
				int opcode,reg_R,addr_MM,instr_sign;
				int posir = abs((*cpu).ir);
				int temp;
				opcode  = posir / 1000;
				reg_R   = (posir /100) % 10;
				addr_MM = posir % 100;
				if((*cpu).ir > 0) instr_sign = 1;
				else	   instr_sign = -1;

                // Echo instruction
                printf("At %02d instr %d %d %02d: ", instr_loc, cpu -> opcode, cpu -> reg_R, cpu -> addr_MM);

                switch (cpu -> opcode)
                {
            case 0:
                    exec_HLT(cpu);
                    break;
			case 1: cpu -> reg[reg_R] = cpu -> mem[addr_MM];
				printf("LD   R%d <- M[%d] = %d\n",reg_R,addr_MM, (*cpu).mem[addr_MM]);break;
			case 2: cpu -> mem[addr_MM] = cpu -> reg[reg_R];
				printf("ST   M[%d] <- R%d = %d\n",addr_MM,reg_R,cpu->reg[reg_R]);break;
			case 3: temp = cpu -> reg[reg_R];
				cpu->reg[reg_R] += cpu->mem[addr_MM];
				printf("ADD  R%d <- R%d + M[%d] = %d + %d = %d\n",reg_R,reg_R,addr_MM,temp,cpu->mem[addr_MM],temp + cpu->mem[addr_MM]);break;
			case 4: cpu->reg[reg_R] = -(cpu->reg[reg_R]);
				printf("NEG  R%d <- -R%d = %d\n",reg_R,reg_R,cpu->reg[reg_R]);break;
			case 5: cpu->reg[reg_R] = instr_sign * addr_MM;
				printf("LDM  R%d <- %d\n",reg_R,cpu->reg[reg_R]);break;
			case 6: temp = cpu->reg[reg_R];
				cpu->reg[reg_R] += (instr_sign * addr_MM);
				printf("ADDM R%d <- R%d + %d = %d + %d = %d\n",reg_R,reg_R,instr_sign * addr_MM,temp,instr_sign * addr_MM,temp+(instr_sign * addr_MM));break;
			case 7: cpu->pc = addr_MM;
				printf("BR   %d\n",addr_MM);break;
			case 8: if(cpu->ir > 0)
			{
				if(cpu->reg[reg_R] > 0)
				{
					cpu->pc = addr_MM;
					printf("BRC  %d if R%d = %d > 0: Yes\n",addr_MM,reg_R,cpu->reg[reg_R]);
				}
				else printf("BRC  %d if R%d = %d > 0: No\n",addr_MM,reg_R,cpu->reg[reg_R]);
			}
			else if (cpu->ir < 0)
			{
				if(cpu->reg[reg_R] < 0)
				{
					cpu->pc = addr_MM;
					printf("BRC  %d if R%d = %d < 0: Yes\n",addr_MM,reg_R,cpu->reg[reg_R]);
				}
				else printf("BRC  %d if R%d = %d < 0: No\n",addr_MM,reg_R,cpu->reg[reg_R]);
			}
			break;
			case 9: if(reg_R == 0)
			{
				char tem;
				printf("I/O  Read char\n");
				printf("Enter a char (and/or press return):");
				scanf("%c",&tem);
				cpu->reg[0]=(int)tem;
				printf("R0 <- %d\n",cpu->reg[0]);
			}
			else if (reg_R == 1)
			{
				printf("I/O  1: Print char in R0 (= %d): %c\n",cpu->reg[0],cpu->reg[0]);
			}
			else if (reg_R == 2)
			{
				int temp = addr_MM;
				printf("I/O  2: Print string: ");
				while(cpu->mem[temp] != 0) printf("%c",cpu->mem[temp++]);
				printf("\n");
			}
			else if (reg_R == 3)
			{
				printf("I/O  3: Dump Control Unit:\n");
				dump_control_unit(cpu);
			}
			else if (reg_R == 4)
			{
				printf("I/O  4: Dump Memory:\n");
				dump_memory(cpu);
			}
			else
			{
				printf("Unknown I/O; skipped\n");
			}
			break;


                default:
                    printf("Bad opcode!? %d\n", cpu -> opcode);
                }
            }




// Execute the halt instruction (make CPU stop running)
//
            void exec_HLT(CPU *cpu)
            {
                printf("HALT\nHalting\n");
                (cpu -> running) = 0;
            }

