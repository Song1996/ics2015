#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

bool check_parentheses(int p,int q);
uint32_t eval(int p,int q);
int prior(int t);
uint32_t variable_value(char* s);	
		
enum {
	NOTYPE = 256, EQ,NUM,HEXNUM,NQ,AND,OR,REG,DEFER,VAR

	/* TODO: Add more token types */

};

static struct rule {
	char *regex;
	int token_type;
} rules[] = {

	/* TODO: Add more rules.
	 * Pay attention to the precedence level of different rules.
	 */

	{" +",	NOTYPE},				// spaces
	{"\\+", '+'},					// plus
	{"-",'-'},						//minus
	{"\\*",'*'},					//multiply
	{"/",'/'},						//divide
	{"\\(",'('},					//zuo kuo hao
	{"\\)",')'},					//you kuo hao
	{"==", EQ},						// equal
	{"!=",NQ},						//not equal
	{"!",'!'},						//not
	{"\\&{2}",AND},					//AND
	{"\\|{2}",OR},					//OR	
	{"0[xX][0-9a-fA-F]+",HEXNUM},	//HEXNUM
	{"[0-9]+",NUM},					// Decimal number
	{"(\\$e?[abcd]x)|(\\$e?[sd]i)|(\\$e?[sb]p)|(\\$[abcd][lh])|(\\$eip)",REG},	//registers
	{"[0-9a-zA-Z_]+",VAR}			//Variable

};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
	int i;
	char error_msg[128];
	int ret;

	for(i = 0; i < NR_REGEX; i ++) {
		ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
		if(ret != 0) {
			regerror(ret, &re[i], error_msg, 128);
			Assert(ret == 0, "regex compilation failed: %s\n%s", error_msg, rules[i].regex);
		}
	}
}

typedef struct token {
	int type;
	char str[32];
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
	int position = 0;
	int i;
	regmatch_t pmatch;
	
	nr_token = 0;

	while(e[position] != '\0') {
		/* Try all rules one by one. */
		for(i = 0; i < NR_REGEX; i ++) {
			if(regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
				char *substr_start = e + position;
				int substr_len = pmatch.rm_eo;

				Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i, rules[i].regex, position, substr_len, substr_len, substr_start);
				position += substr_len;

				/* TODO: Now a new token is recognized with rules[i]. Add codes
				 * to record the token in the array ``tokens''. For certain 
				 * types of tokens, some extra actions should be performed.
				 */

				switch(rules[i].token_type) {
					case  NOTYPE: break;
					case '+': tokens[ nr_token ++ ].type='+';break;
					case EQ: tokens[ nr_token ++ ].type=EQ;break;
					case '*': tokens[ nr_token ++ ].type='*';break;
					case '/': tokens[ nr_token ++ ].type='/';break;
					case '-': tokens[ nr_token ++ ].type='-';break;
					case '(': tokens[ nr_token ++ ].type='(';break;
					case ')': tokens[ nr_token ++ ].type=')';break;
					case AND: tokens[ nr_token ++ ].type=AND;break;
					case OR: tokens[ nr_token ++ ].type=OR;break;
					case NQ: tokens[ nr_token ++ ].type=NQ;break;
					case '!': tokens[ nr_token ++ ].type='!';break;
					case NUM: tokens[ nr_token ].type=NUM; 
							 int j = 0;
							 for(;j<substr_len;j++)
								tokens[ nr_token ].str[j]=substr_start[j];
							 tokens[nr_token].str[j]='\0';
							 nr_token++;break;//printf("\n%s\n%s",tokens[nr_token-1].str,e);break;
					case HEXNUM:tokens[ nr_token ].type=HEXNUM;
								for(j=2;j<substr_len;j++)
									tokens[ nr_token ].str[j-2]=substr_start[j];
								tokens[ nr_token ++ ].str[j-2]='\0';break;
					case REG: tokens[ nr_token ].type=REG;
							  for(j=1;j<substr_len;j++)
								  tokens[ nr_token ].str[j-1]=substr_start[j];
							  tokens[ nr_token ++ ].str[j-1]='\0';break;
					case VAR: tokens[ nr_token ].type=VAR;
							  for(j=0;j<substr_len;j++)
								  tokens[ nr_token ].str[j]=substr_start[j];
							  tokens[nr_token].str[j]='\0';
							  nr_token++;break;
					default: panic("please implement me");
				}
				break;
			}
		}

		if(i == NR_REGEX) {
			printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
			return false;
		}
	}

	return true; 
}


int prior(int t)
{
	switch(t)
	{
		case OR:return 1;
		case AND:return 2;
		case EQ:case NQ:return 3;
		case '+':case '-':return 4;
		case '*':case '/':return 5;
		case '!':case DEFER:return 6;
		default:return 10;
	}
}


uint32_t eval(int p,int q)
{
	int z;
	int strp;
	char t;
	if(p>q)
	{
		//panic("wrong expr");
		return 0;
	}
	else if(p==q)
	{
		if(tokens[p].type==NUM)
		{
			z=0;
			strp=0;
			t=tokens[p].str[strp];
			while(t!='\0')
			{
				z=z*10+(t-'0');
				strp++;
				t=tokens[p].str[strp];
			}
			return z;
		}
		else if(tokens[p].type==HEXNUM)
		{
			z=0;
			strp=0;
			t=tokens[p].str[strp];
			while(t!='\0')
			{
				if(t>='0'&&t<='9')
					z=z*16+t-'0';
				else if(t>='a'&&t<='f')
					z=z*16+t-'a'+10;
				else if(t>'A'&&t<='F')
					z=z*16+t-'A'+10;
				strp++;
				t=tokens[p].str[strp];
			}
			return z;
		}
		else if(tokens[p].type==REG)
		{

			int i;
			for(i=0;i<8;i++)
			{
				if(strcmp(tokens[p].str,regsl[i])==0) return reg_l(i);
				if(strcmp(tokens[p].str,regsw[i])==0) return reg_w(i);
				if(strcmp(tokens[p].str,regsb[i])==0) return reg_b(i);
			}
			if(strcmp(tokens[p].str,"eip")==0)
				return cpu.eip;
			assert(0);
			return 0;
		}
		else if(tokens[p].type==VAR)
			return variable_value(tokens[p].str);
		else
		{
			panic("wrong expr");
			return -1;
		}
	}	
	else if(check_parentheses(p,q))
	{
		return eval(p+1,q-1);
	}
	else
	{
		int np=0;
		int i=0;
		int k=0;
		for(i=p;i<q+1;i++)
			if(tokens[i].type=='(')np++;
			else if(tokens[i].type==')')np--;
			else if(np==0)
				{
					//if(tokens[k].type=='('||tokens[k].type==NUM)
					//	k=i;
					if(prior(tokens[i].type)<prior(tokens[k].type))
						k=i;
				}
		switch(tokens[k].type)
		{
		case '+':return eval(p,k-1)+eval(k+1,q);
		case '-':return eval(p,k-1)-eval(k+1,q);
		case '*':return eval(p,k-1)*eval(k+1,q);
		case '/':return eval(p,k-1)/eval(k+1,q);
		case AND:return eval(p,k-1)&&eval(k+1,q);
		case OR:return eval(p,k-1)||eval(k+1,q);
		case EQ:return eval(p,k-1)==eval(k+1,q);
		case NQ:return eval(p,k-1)!=eval(k+1,q);
		case '!':return !eval(k+1,q);
		case DEFER: return swaddr_read(eval(k+1,q),4);
		default: assert(0);return -1;
		}
	}
}
bool check_parentheses(int p,int q)
{
	int i=p;
	int np=0;
	for(i=p;i<q+1;i++)
	{
		if(tokens[i].type=='(')
			np++;
		else if(tokens[i].type==')')
			np--;
		if(np<0)
		{
			panic("wrong expr");
			return false;
		}
	}
	if(np==0&&tokens[p].type=='('&&tokens[q].type==')')
		return true;
	else
		return false;
}

uint32_t expr(char *e, bool *success) {
	if(!make_token(e)) {
		*success = false;
		return 0;
	}
	int i;
	for(i = 0; i < nr_token; i ++)
   	{
		if(tokens[i].type == '*' && (i == 0 ||( tokens[i - 1].type!=NUM && tokens[i-1].type!=HEXNUM && tokens[i-1].type!=REG && tokens[i-1].type!=')') ) )
	   	{
			tokens[i].type = DEFER;
		}
	}
	int q=nr_token-1;

	uint32_t f=eval(0,q);
	//printf("\n%d\n",f);	
	/* TODO: Insert codes to evaluate the expression. */
	//panic("please implement me");
	return f;
}

