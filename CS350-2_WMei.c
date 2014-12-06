//CS350-02 Fall 2014
//Lab10 Final Project Part 2
//Wei Shao Mei

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
  typedef short int Word;
  typedef unsigned short int Address;
  #define MEMLEN 65536
  #define NREG 8

  typedef struct {
    Word mem[MEMLEN];
    Word reg[NREG];
    Address pc;
    int cc;
    int running;
    Word ir;

    int instr_sign;
    int opcode;
    int reg_R;
    int addr_MM;
  }CPU;

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
	
	int exec_Jump(char c,int i, CPU *cpu);
	int exec_MemLoad(char c, int i, int j, CPU *cpu);
	int exec_RegLoad(char c, int i, int j, CPU *cpu);

 	void instr_ADD (CPU *cpu);
	void instr_AND (CPU *cpu);
	void instr_BR  (CPU *cpu);
	void instr_err (CPU *cpu);
	void instr_JMP (CPU *cpu);
	void instr_JSR (CPU *cpu);
	void instr_LD  (CPU *cpu);
	void instr_LDI (CPU *cpu);
	void instr_LDR (CPU *cpu);
	void instr_LEA (CPU *cpu);
	void instr_NOT (CPU *cpu);
	void instr_RTI (CPU *cpu);
	void instr_ST  (CPU *cpu);
	void instr_STI (CPU *cpu);
	void instr_STR (CPU *cpu);
	void instr_TRAP(CPU *cpu);
	int  read_char();             // Read a character for GETC, IN
	void trap_GETC  (CPU*cpu);
	void trap_OUT   (CPU*cpu);
	void trap_PUTS  (CPU*cpu);
	void trap_IN    (CPU*cpu);
	void trap_PUTSP (CPU*cpu);
	void trap_HALT  (CPU*cpu);//Main Program: Initialize the cpu, read in a program, and e
//
int main(int argc, char *argv[]){
   printf("LC-3 Simulator - Wei Shao Mei - CS350-02 \n");
  CPU cpu_value;
  CPU *cpu =&cpu_value;
  initialize_control_unit(cpu);
  initialize_memory(argc, argv, cpu);
  //dump_control_unit(cpu);
  dump_memory(cpu);

  char *prompt = "> ";
  printf("Beginning execution; type h for help\n%s", prompt);

  int done = read_execute_command(cpu);
  while(!done){
    printf("%s", prompt);
    done = read_execute_command(cpu);
  }return 0;

}

void initialize_control_unit(CPU *cpu){
	cpu->pc =0;
	cpu->ir=0;
	cpu->running=1;
	int i;
	for(i=0; i<NREG; i++)
	{cpu->reg[i]=0;}

	printf("\n initial control unit: \n");
	dump_control_unit(cpu);
	printf("\n");
}

void initialize_memory(int argc, char *argv[], CPU *cpu) {
	FILE *datafile = get_datafile(argc, argv);

	int value_read, words_read, loc=0, done=0;

	char *buffer=NULL;
	size_t buffer_len =0, bytes_read=0;
	int i, initial =1;

	for(i =0; i<MEMLEN; i++)
	  cpu->mem[i]=0;
	bytes_read= getline(&buffer,&buffer_len, datafile);
	while (bytes_read != -1 && !done){

		words_read = sscanf(buffer, "%x", &value_read);
		if(words_read ==1)
		{if (initial && value_read < MEMLEN && value_read > 0)
		  {loc =value_read;
		    initial=0;
		    cpu->pc =value_read;
		}
		  else if(value_read <0 || value_read >= MEMLEN)
		  {
		    printf("sentinel %d found at location %d\n", value_read, loc);
		    break;
		  }
		  else if(loc>=MEMLEN)
		  {
		    loc=0;
		    cpu->mem[loc++]= value_read;
		  }
		  else{cpu->mem[loc++]= value_read;}
		}
		
		bytes_read = getline(&buffer, &buffer_len, datafile);
		//done=!bytes_read;
	}
	free(buffer);
	//while (loc < MEMLEN) {
	//  cpu->mem[loc++]=0x0000;
//	}
//	dump_memory(cpu);
}

FILE *get_datafile(int argc, char *argv[]){
  char *default_datafile_name="program.hex";
  char *datafile_name;

  if(!argv[1])
    datafile_name=default_datafile_name;
  else
    datafile_name=argv[1];

  FILE *datafile = fopen(datafile_name, "r");

  if(!datafile){
    printf("File failed to open \n");
    exit(EXIT_FAILURE);
  }
  return datafile;
}

void dump_control_unit(CPU *cpu){
  printf("pc :x%04x    CC: %x  IR :x%04x    RUNNING : %d\n", cpu->pc,cpu->cc, cpu->ir, cpu->running);
  dump_registers(cpu);
}

void dump_memory(CPU *cpu){
 
 
  int loc =0;
  int row,col;
  int j;

  printf("\nMEMORY  (addresses x0000 - xFFFF):\n");
  for(j=0;j<MEMLEN;j++){
    if(cpu->mem[j]!=0)
     printf("x%04x  x%04x \n",j , cpu->mem[j]);
}}

void dump_registers(CPU *cpu){
  int i;
  for(i=0; i<NREG;i++){
    if(i%4==0) printf("\n");
    printf("R%d:%04x ",i, cpu->reg[i]);
  }
}
//read a simulator command from keyboard ("h","?","d",number, or empty line) and execute it.
//return true if we hit end-of-input or execute_command told us to quit. Otherwise return true.
//
int read_execute_command(CPU *cpu){
  char *cmd_buffer=NULL;
  size_t cmd_buffer_len=0,bytes_read=0;

  int nbr_cycles ,n, m;
  char cmd_char;
  size_t words_read;

  int done=0;

  bytes_read=getline(&cmd_buffer, &cmd_buffer_len, stdin);
  if(bytes_read == -1){
    done=1;	//hits end of file
  }

  words_read = sscanf(cmd_buffer, "%d", &nbr_cycles);

  // checks the length of the line read from the user input
  // if between 7 and 8 then will do a Jump if 12-14 will load memory
  // then checks if an int or else and deals with it accordingly.
  if(bytes_read >=7 && bytes_read<9){
    if(bytes_read == 8)
    {words_read= sscanf(cmd_buffer, "%c x%x",&cmd_char, &n);
    }else {words_read=sscanf(cmd_buffer, "%c %x", &cmd_char, &n);}
   done= exec_Jump(cmd_char,n,cpu);
  }else if(bytes_read >=12 && bytes_read <15){
    if(bytes_read==14){
      words_read=sscanf(cmd_buffer, "%c x%x x%x", &cmd_char, &n, &m);
    }else{
      words_read=sscanf(cmd_buffer, "%c %x %x", &cmd_char, &n, &m);
    }
    done= exec_MemLoad(cmd_char, n, m, cpu);
  }else if (bytes_read == 10 ){
    words_read= sscanf(cmd_buffer, "%c %d x%x", &cmd_char, &n, &m);
   done= exec_RegLoad(cmd_char, n, m, cpu);
  }  else if(words_read==1)
  many_instruction_cycles(nbr_cycles, cpu);
  else {
    words_read=sscanf(cmd_buffer, "%c", &cmd_char);
    done=execute_command(cmd_char, cpu);
  

  free(cmd_buffer);
  return done;
  }
  
}

int execute_command(char cmd_char, CPU *cpu){
  if (cmd_char == '?' || cmd_char == 'h'){
    help_message();
  }
  else if(cmd_char=='q'){
    printf("quit \n");
    return 1;
  }
  else if(cmd_char == 'd'){
    dump_control_unit(cpu);
    dump_memory(cpu);
  }
  else if((cmd_char == 'j')|| (cmd_char== 'm'||(cmd_char== 'r'))){  //if j or m get to here it means the input is missing a few digits or arguments.
    printf("Command is missing or doesn't have the proper arguments\n");
  }
  else
    printf("%c not a valid command \n", cmd_char);

 return 0;
}

void help_message(void){
  printf("h or ? for help (prints this message) \n");
  printf("d to dump the control unit and memory \n");
  printf("j xNNNN to jump to new location \n");
  printf("m xNNNN xMMMM to assign memory location xNNNN = value xMMMM \n");
  printf("An integer > 0 to execute that many isntruction cycles \n");
  printf("or just return, which executes one instruction cycle \n");

}

void many_instruction_cycles(int nbr_cycles, CPU *cpu){
if(nbr_cycles <=0) {
printf("Number of instruction cycles > 0\n");
return;
}
if (cpu->running == 0)
{printf("CPU has been halted\n");
  return;}

if (nbr_cycles >= MEMLEN)
{printf("%d is too large for cycle; doing %d \n", nbr_cycles, MEMLEN);}
int count;
for (count = 0; count < nbr_cycles; count++)
{if(cpu->running==0) break;
one_instruction_cycle(cpu);
}
return;
}

void one_instruction_cycle(CPU *cpu){
  if(cpu->running ==0)
  {
    printf("CPU has been Halted\n");
    return;
  }
  if(cpu->pc >= MEMLEN)
  {
  printf("pc is out of range.\n");
  cpu->running =0;
  return;
  }
  int instr_loc =cpu->pc;
  cpu->ir=cpu->mem[cpu->pc++];

  int instr_sign;
  int posir =abs(cpu->ir);
  int temp;
  cpu->opcode = posir>>12; // bitshift right by 12 to get opcode 
 
  
  if(cpu->ir >0) cpu->instr_sign =1;
  else cpu->instr_sign= -1;

  printf("At %04x instr %x : \n", instr_loc, cpu->opcode);
  

  //instruction calculations are called from other methods to reduce clutter in the switches.
  switch (cpu->opcode)
  {
    case 0: 
    exec_HLT(cpu);
    break;

  case 1:
    instr_ADD(cpu);
    break;

  case 2:
    instr_LD(cpu);
    break;

  case 3:
    instr_ST(cpu);
    break;

  case 4:
    instr_JSR(cpu);
    break;

  case 5:
    instr_AND(cpu);
    break;

  case 6:
    instr_LDR(cpu);
    break;

  case 7:
    instr_STR(cpu);
    break;

  case 8:
    instr_RTI(cpu);
    break;

  case 9:
    instr_NOT(cpu);
    break;

  case 10:
    instr_LDI(cpu);
    break;

  case 11:
    instr_STI(cpu);
    break;

  case 12:
    instr_JMP(cpu);
    break;
    
  case 13:
    instr_err(cpu);
    break;
    
  case 14:
    instr_LEA(cpu);
    break;

  case 15:
    instr_TRAP(cpu);
    break;

  default:
    printf("Bad opcode: %x; quitting\n", cpu->opcode);
    cpu->running=0;


  }
  printf("IR: %d \n", cpu->ir);
}

void exec_HLT(CPU *cpu){
  printf("HALT \n Halting \n");
  (cpu ->running)= 0;
}
// instruction routines
void instr_ADD(CPU *cpu){
  
  int dst = (cpu->ir >>9)&0x7; //shift the bit string to save dst bits
  int src1 = (cpu->ir>>6)&0x7;  //shift the bit string to save the src1 bits 
  
  //checks if its an immed ADD
  if((cpu->ir>>5)&0x1){
   short int immed5;
   if((cpu->ir>>4)&0x0001){
     immed5=cpu->ir&0x001f+0xff00;
   }
   else{
     immed5=cpu->ir&0x001fl;
   }
  } 
 
}
void instr_LD(CPU *cpu){
  int dst = (cpu->ir>>9)&0x0007;
  int offset9;
  if((cpu->ir>>8)&0x0001){
    offset9= cpu->ir&0x01ff+0xf000;
  
  }
  else{
    offset9= cpu->ir&0x01ff;
  }

}
void instr_ST(CPU *cpu){

 
}
void instr_JSR(CPU *cpu){


}
void instr_AND(CPU *cpu){

 
}
void instr_LDR(CPU *cpu){

 
}
void instr_STR(CPU *cpu){

 
}
void instr_RTI(CPU *cpu){

 
}
void instr_NOT(CPU *cpu){


}
void instr_LDI(CPU *cpu){


}
void instr_STI(CPU *cpu){


}
void instr_JMP(CPU *cpu){


}
void instr_err(CPU *cpu){


}
void instr_LEA(CPU *cpu){


}
void instr_TRAP(CPU *cpu){
  short int trap= cpu->ir & 0x00ff;
  switch(trap){
    case 0x0020:
      trap_GETC(cpu);
      break;
    case 0x0021:
      trap_OUT(cpu);
      break;
    case 0x0022:
      trap_PUTS(cpu);
      break;
    case 0x0023:
      trap_IN(cpu);
      break;
    case 0x0024:
      trap_PUTSP(cpu);
      break;  
    case 0x0025:
      trap_HALT(cpu);
      break;
    default:
      printf("invalid trap operation\n");
      break;
  }
}

void trap_GETC(CPU *cpu){}
void trap_OUT(CPU *cpu){}
void trap_PUTS(CPU *cpu){}
void trap_IN(CPU *cpu){}
void trap_PUTSP(CPU *cpu){}
void trap_HALT(CPU *cpu){}

int exec_Jump(char c, int i, CPU *cpu){
  if (i>=MEMLEN || i<0)
	printf("invalid location in mem \n");
	else 
	{
	  cpu->pc = i;
	  printf("Jumping to x%04x \n", cpu->pc);
	}
  return 0;
  }
int exec_MemLoad(char c, int i, int j, CPU *cpu){
  int count =0;


  if(i>= MEMLEN || i<0){
  printf("invalid location in mem \n");
  count++;
  }
  if (i < 32768 || i > (-32768)){
    printf("invalid value to store in mem \n");
    count++;
  }
  if(!count){
  cpu->mem[i]= j;
  cpu->running =1;
  printf("storing %04hx to M[%hx] \n",j, i);
  }
  return 0;
}

int exec_RegLoad(char c, int i, int j, CPU *cpu){
int count =0;
  if(i>NREG || i<0){
  printf("invalid Register \n");
  count++;
  }
if (j>32768 || j< (-32768)){
  printf("invalid value to store in reg\n");
  count++;
}
if(!count){
  cpu->reg[i]=j;
  cpu->running =1;
  printf("loads %x to R[%d]\n",j, i);
}
return 0;

}

int read_char(){
  char buffer[3] ="";
  printf("Enter a char for GETC or IN traps");
  fgets(buffer, sizeof buffer, stdin);
  return buffer[0];

}
