// Bardina_Manuel
// CS 350, Fall 2014
// Lab 10: SDC
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
#define WMAX 32768
#define WMIN -32768
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
    //global variables
    Address cache0;
    short int cache1;
}  CPU;


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
void jump(CPU *cpu);
void memory(CPU *cpu);
void setRegistrer(CPU *cpu);
void many_instruction_cycles(int nbr_cycles, CPU *cpu);
void one_instruction_cycle(CPU *cpu);
void exec_HLT(CPU *cpu);

// Main program: Initialize the cpu, read initial memory values,
// and execute the read-in program starting at location 00.
//
int main(int argc, char *argv[])
{
    printf("LC3 Simulator:  CS 350 Lab 8\n");
    CPU cpu_value, *cpu = &cpu_value;
    initialize_control_unit(cpu);
    initialize_memory(argc, argv, cpu);
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
	int i, initial = 1;
        // Read in first and succeeding memory values. Stop when we
        // hit a sentinel value, fill up memory, or hit end-of-file.
        //
	for (i = 0; i < MEMLEN; i++)
	  (*cpu).mem[i] = 0;



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
	      if (initial && value_read < MEMLEN && value_read > 0)
		{
		  loc = value_read;
		  initial = 0;
		  (*cpu).pc = value_read;
		}

	      else if(value_read < 0 || value_read >= MEMLEN)
                {
		  printf("sentinel %d found at location %d\n", value_read, loc);
		  break;
		}
	      else if(loc >= MEMLEN)
		{
		  loc = 0;
		  (*cpu).mem[loc++] = value_read;
		}
		else
		  {
		   (*cpu).mem[loc++] = value_read;
		  }
            }
            // set memory value at current location to
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
	printf("Loading %s\n\n", datafile_name);
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
	  printf("CONTROL UNIT:\n");
            printf("pc :  x%04x   IR :  x%04x  cc :  %x  RUNNING : %d", (*cpu).pc, (*cpu).ir, (*cpu).cc, (*cpu).running);
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
	    printf("\n\nMEMORY (addresses x0000 - xFFFF)\n");

    for (i = 0; i < MEMLEN; i++)
    {
      if ((*cpu).mem[i] != 0)
	{
	  printf("x%04hx: x%04hx       %hx\n",i,(*cpu).mem[i], (*cpu).mem[i]);
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
                printf("R%d x%04hx %hx   ", i, (*cpu).reg[i], (*cpu).reg[i]);
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
            char cmd_char, pre;
	    Address loc;
	    short int val;
            size_t words_read;    // number of items read by sscanf call

            int done = 0;	// Should simulator stop?

            bytes_read = getline(&cmd_buffer, &cmd_buffer_len, stdin);
            if (bytes_read == -1)
            {
                done = 1;   // Hit end of file
            }

            words_read = sscanf(cmd_buffer, "%d", &nbr_cycles, "%x", &val);
            // *** ****  If we found a number, do that many
            // instruction cycles.  Otherwise sscanf for a character
            // and call execute_command with it.  (Note the character
            // might be '\n'.)

            if (words_read == 1)
	      many_instruction_cycles(nbr_cycles, cpu);
            else
            {
	      words_read = sscanf(cmd_buffer, "%c %c%hx x%hx", &cmd_char, &pre, &loc, &val);
	      (*cpu).cache0 = loc;
	      (*cpu).cache1 = val;

	      if(cmd_char=='j' && (words_read != 3 || pre !='x'))
		  printf("Jump command should be j adress (in xNNNN format)\n");
	      else if (cmd_char == 'm' && (words_read != 4 || pre != 'x'))
		printf("Memory command should be m addr value (in xNNNN format)\n");
	      else if (cmd_char == 'r' && (words_read != 4 || pre != 'r'))
		printf("SetRegistrer command should be r rN value (in xNNNN format) \n");
		done = execute_command(cmd_char, cpu);
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
		  one_instruction_cycle(cpu);
                }
		else if (cmd_char == 'j')
		{
		  jump(cpu);
		}
		else if (cmd_char == 'm')
	        {
		  memory(cpu);
		}
		else if (cmd_char == 'r')
		{
		    setRegistrer(cpu);
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
                printf("d to dump the control unit and memory\n");
                printf("recently added\n");
                printf("j xNNNN to jump to new location\n");
                printf("m xNNNN xNNNN to assign memory location xNNNN = value xNNNN\n");
                printf("An integer >0 to execute that many instruction cycles\n");
                printf("Or just return, which executes one instruction cycle\n");
            }


// Jump to desired memory location
//
void jump(CPU *cpu)
{
  if ((*cpu).cache0 >= MEMLEN || (*cpu).cache0 < 0)
    printf("invalid location in memory\n");
  else
    {
      (*cpu).pc = (*cpu).cache0;
      printf("Jumped to x%04hx\n", (*cpu).pc);
    }
}

// Store value in memory location
//
void memory(CPU *cpu)
{
  int missxtakes = 0;
  if ((*cpu).cache0 >= MEMLEN || (*cpu).cache0 < 0){
    printf("invalid location in memory\n");
    missxtakes ++;}
  if ((*cpu).cache1 > WMAX || (*cpu).cache1 < WMIN){
    printf("invalid value to store in memory\n");
    missxtakes ++;}
  if(!missxtakes){
    (*cpu).mem[(*cpu).cache0] = (*cpu).cache1;
    (*cpu).running = 1;
    printf("set mem[x%04hx] to x%04hx\n", (*cpu).cache0, (*cpu).cache1);
  }
}

void setRegistrer(CPU *cpu)
{
  //does absolutely nothing.
  int missxtakes = 0;
  if ((*cpu).cache0 >= NREG || ((*cpu).cache0 < 0)){
    printf("invalid location in register\n");
    missxtakes ++;}
  if ((*cpu).cache1 > WMAX || (*cpu).cache1 < WMIN){
    printf("invalid value to store in register\n");
    missxtakes ++;}
  if(!missxtakes){
    (*cpu).reg[(*cpu).cache0] = (*cpu).cache1;
    (*cpu).running = 1;
    printf("set r%hx to x%04hx\n", (*cpu).cache0, (*cpu).cache1);
  }

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
		nbr_cycles = 100;
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
				int opcode,reg_R,addr_MM,instr_sign;
				int posir = abs((*cpu).ir);
				int temp;
				opcode  = posir / 1000;
				reg_R   = (posir /100) % 10;
				addr_MM = posir % 100;
				if((*cpu).ir > 0) instr_sign = 1;
				else	   instr_sign = -1;

				// Echo instruction
				printf("x%04x: x%04x ", instr_loc, (*cpu).mem[instr_loc]);

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
                    printf("invalid instruction\n");
                }
            }




// Execute the halt instruction (make CPU stop running)
//
            void exec_HLT(CPU *cpu)
            {
                printf("HALT\nHalting\n");
                (cpu -> running) = 0;
            }

