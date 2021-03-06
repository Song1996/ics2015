#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
extern CPU_state cpu;

void cpu_exec(uint32_t);
void find_print_func();

/* We use the ``readline'' library to provide more flexibility to read from stdin. */
char* rl_gets() {
	static char *line_read = NULL;

	if (line_read) {
		free(line_read);
		line_read = NULL;
	}

	line_read = readline("(nemu) ");

	if (line_read && *line_read) {
		add_history(line_read);
	}

	return line_read;
}

static int cmd_c(char *args) {
	cpu_exec(-1);
	return 0;
}

static int cmd_q(char *args) {
	return -1;
}

static int cmd_si(char *args);
static int cmd_help(char *args);
static int cmd_info(char *args);
static int cmd_x(char *args);
static int cmd_p(char *args);
static int cmd_w(char *args);
static int cmd_d(char *args);
static int cmd_bp();

static struct {
	char *name;
	char *description;
	int (*handler) (char *);
} cmd_table [] = {
	{ "help", "Display informations about all supported commands", cmd_help },
	{ "c", "Continue the execution of the program", cmd_c },
	{ "q", "Exit NEMU", cmd_q },
	{ "si", "Dan Bu Tiao Shi", cmd_si },
	{"info","Some information", cmd_info},
	{"x","Scan memory",cmd_x},
	{"p","Calaulate the Polynomial",cmd_p},
	{"w","ADD one Watchpoint",cmd_w},
	{"d","delete one watchpoint",cmd_d},
	{"bp","zhanzhenglian",cmd_bp}
	/* TODO: Add more commands */
};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
	/* extract the first argument */
	char *arg = strtok(NULL, " ");
	int i;

	if(arg == NULL) {
		/* no argument given */
		for(i = 0; i < NR_CMD; i ++) {
			printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
		}
	}
	else {
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(arg, cmd_table[i].name) == 0) {
				printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
				return 0;
			}
		}
		printf("Unknown command '%s'\n", arg);
	}
	return 0;
}

static int cmd_si(char *args){
	if(args==NULL)
		cpu_exec(1);
	else
	{
		int n=0;
		int i=0;
		char t=args[i];
		while(t==' ')
		{
			i++;
			t=args[i];
		}
		while(t!='\0')
		{
			if(t>'9'||t<'0')
			{
				printf("Unknown command\n");
				return 0;
			}
			else
			{
				n=(10*n)+(t-'0');
			}
			i++;
			t=args[i];
		}
		cpu_exec(n);
	}
	return 0;	
}

static int cmd_info(char *args){
	if(args==NULL);
	else
	{
		if(strcmp(args,"r")==0)
		{
			printf("eax		%d	0x%08x\n",cpu.eax,cpu.eax);			
			printf("ecx		%d	0x%08x\n",cpu.ecx,cpu.ecx);			
			printf("edx		%d	0x%08x\n",cpu.edx,cpu.edx);			
			printf("ebx		%d	0x%08x\n",cpu.ebx,cpu.ebx);			
			printf("esp		%d	0x%08x\n",cpu.esp,cpu.esp);			
			printf("ebp		%d	0x%08x\n",cpu.ebp,cpu.ebp);			
			printf("esi		%d	0x%08x\n",cpu.esi,cpu.esi);			
			printf("edi		%d	0x%08x\n",cpu.edi,cpu.edi);
			printf("eip				0x%08x\n",cpu.eip);
			printf("EFLAGS				0x%08x\n",cpu.eflags_val);
						
		}
		else if(strcmp(args,"w")==0)
		{
			WP* watchpoi=WP_returnhead();
			while(watchpoi!=NULL)
			{
				printf("No.%d	%s			%d\n",watchpoi->NO,watchpoi->Expr,watchpoi->Value);
				watchpoi=watchpoi->next;
			}
		}
	}
	return 0;
}

static int cmd_x(char *args)
{
	if(args==NULL);
	else
	{
		int i=0;
		char t=args[i];
		while(t==' ')
		{
			i++;
			t=args[i];
		}
		int n=0;
		while(t!=' ')
		{
			if('0'<=t&&t<='9')
			{
				n=n*10+t-'0';
			}
			else
			{
				printf("Unknown command\n");
				return 0;
			}
			i++;
			t=args[i];
		}
		while(t==' ')
		{
			i++;
			t=args[i];
		}
		uint32_t addr=0;
		/*if(args[i]=='0'&&args[i+1]=='x')
		{
			i+=2;
			t=args[i];
			while(t!='\0')
			{
				if(t<='9'&&t>='0')
					addr=addr*16+t-'0';
				else if(t<='F'&&t>='A')
					addr=addr*16+t-'A'+10;
				else if(t<='f'&&t>='a')
					addr=addr*16+t-'a'+10;
				else
				{
					printf("Unknown command\n");
					return 0;
				}
				i++;
				t=args[i];
			}

			int j=0;
			for(;j<n;j++)
			{
				printf("0x%02x		0x%02x\n",addr+j,swaddr_read(addr+j,1));
			}
		}*/
		bool success = false;
		bool *psuccess=&success;
		addr=expr(args+i,psuccess);
		int j=0;
		for(;j<n;j++)
		{
				printf("0x%02x		0x%02x\n",addr+j,swaddr_read(addr+j,1));

		}

	}
	return 0;
}

uint32_t expr(char *e,bool *success);

static int cmd_p(char *args)
{
	bool success = false;
	bool *psuccess=&success;
	printf("\n%d\n",expr(args,psuccess)); 
	return 0;
}

static int cmd_w(char *args)
{
	WP* watchp=new_wp();
	printf("%d\n",watchp->NO);
	char* te=watchp->Expr;
	char t=args[0];
	int i=0;
	while(t!='\0')
	{
		te[i]=args[i];
		i++;
		t=args[i];
	}
	te[i]=args[i];
	bool success = false;
	bool *psuccess=&success;
	watchp->Value=expr(watchp->Expr,psuccess);
	return 0;
}

static int cmd_d(char *args)
{
	bool success = false;
	bool *psuccess = &success;
	int t=expr(args,psuccess);
	WP* watchpoi=WP_returnhead();
	while(watchpoi!=NULL)
	{
		if(watchpoi->NO==t)
		{
			free_wp(watchpoi);
			return 0;
		}
		else
			watchpoi=watchpoi->next;
	}
	printf("No found %s\n",args);
	return 0;
}

static int cmd_bp()
{
	uint32_t position=cpu.eip;
	uint32_t ebp=cpu.ebp;
	int i=1;
	while(ebp!=0)//swaddr_read(ebp,4)!=0)
	{
		printf("#%d		0x%08x in ",i++,position);
		find_print_func(position);
		printf("\n");
		//printf("%x\n",ebp);
		//if(ebp==0)break;
		position=swaddr_read(ebp+4,4);
		ebp=swaddr_read(ebp,4);
	}
	return 0;
}

void ui_mainloop() {
	while(1) {
		char *str = rl_gets();
		char *str_end = str + strlen(str);

		/* extract the first token as the command */
		char *cmd = strtok(str, " ");
		if(cmd == NULL) { continue; }

		/* treat the remaining string as the arguments,
		 * which may need further parsing
		 */
		char *args = cmd + strlen(cmd) + 1;
		if(args >= str_end) {
			args = NULL;
		}

#ifdef HAS_DEVICE
		extern void sdl_clear_event_queue(void);
		sdl_clear_event_queue();
#endif

		int i;
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(cmd, cmd_table[i].name) == 0) {
				if(cmd_table[i].handler(args) < 0) { return; }
				break;
			}
		}

		if(i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
	}
}
