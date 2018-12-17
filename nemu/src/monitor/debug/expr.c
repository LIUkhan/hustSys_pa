#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
bool valid = true;
bool check_parentheses(uint32_t,uint32_t);
uint32_t findmainop(uint32_t,uint32_t);
bool checklegal(uint32_t,uint32_t);
uint32_t eval(uint32_t,uint32_t);
enum {
  TK_NOTYPE = 256, TK_LP, TK_RP, TK_HNUM, TK_ONUM, TK_REG, TK_EQ

  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */
  {" +", TK_NOTYPE},    // spaces' '
  {"\\(",TK_LP},        //LEFT PARENTHESIS
  {"\\)",TK_RP},        //RIGHT PARENTHESIS
  {"0x[0-9a-fA-F]+|0X[0-9a-fA-F]+u", TK_HNUM}, //16进制数
	{"[0-9]+u", TK_ONUM},  //10进制数
  {"\\$[a-z]+", TK_REG},//没有限制字母数字，不做判断，由软件判断
  
  
  {"\\/", '/'},         // divide '/'
  {"\\*", '*'},         // multiply '*'
  {"\\-", '-'},         // minus '-'
  {"\\+", '+'},         // plus '+'
  {"==", TK_EQ}         // equal '='
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

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

Token tokens[65536];//够用的tokens数组大小 nr_token是下标


uint32_t nr_token;//指示别识别的token数目
//正则表达式一条一条试
static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;//分为开始和结束 start and end of match 

  nr_token = 0;

  while (e[position] != '\0') 
  {
    /* Try all rules one by one. */
    //这里可以看出在rules数组中越先出现的优先级越高
    for (i = 0; i < NR_REGEX; i ++)
    {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) //保证匹配结果精准从字符串开头开始
      {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;
        //匹配子正则表达式成功,e是需要解析的字符串，一个一个匹配正则表达式词法分析开来
        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start); 
            
        position += substr_len;
        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
          default: {
            break;
          }
          case TK_NOTYPE: {
            tokens[nr_token++].type = TK_NOTYPE;
            break;
          }
          case TK_LP: {
            tokens[nr_token++].type = TK_LP;
            break;
          }
          case TK_RP:{
            tokens[nr_token++].type = TK_RP;
            break;
          }
          case TK_HNUM:{
            tokens[nr_token].type = TK_HNUM;
            strncpy(tokens[nr_token++].str,substr_start,substr_len);
            if(substr_len > 31)
              assert(0);
            break;
          }
          case TK_ONUM:{
            tokens[nr_token].type = TK_ONUM;
            strncpy(tokens[nr_token++].str,substr_start,substr_len);
            if(substr_len > 31)
              assert(0);
            break;
          }
          case TK_REG:{
            tokens[nr_token].type = TK_REG;
            strncpy(tokens[nr_token++].str,substr_start,substr_len);
            break;
          }
          case TK_EQ:{
            tokens[nr_token++].type = TK_EQ;
            break;
          }
          case '+':{
            tokens[nr_token++].type = '+';
            break;
          }
          case '-':{
            tokens[nr_token++].type = '-';
            break;
          }
          case '*':{
            tokens[nr_token++].type = '*';
            break;
          }
          case '/':{
            tokens[nr_token++].type = '/';
            break;
          }
        }
        
        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }
  printf("%d\n",position);
  return true;
}

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  // token已经分离出来在tokens数组了，现在要对他们进行解析,自上而下的拆解`
  /* TODO: Insert codes to evaluate the expression. */
  // TODO();
  valid = true;
  uint32_t ret = eval(0,nr_token-1);
  if(ret == 0 && valid == false)
    *success = false;
  else
    *success = true;
  return ret;
}
//用于终止出现非法表达式后函数的执行



//表达式求值函数
uint32_t eval(uint32_t p,uint32_t q)
{
  if(valid == false)
    return 0;
  if(p > q) {
    printf("Error:Bad Expression!\n");//类似于6 7 -这种非法表达式的查错,
    valid = false;
    return 0;
  }
  else if(p == q) { //num hex or oct or err
    int val;
    if(tokens[p].type == TK_HNUM) {
      if(tokens[p].str[1] == 'X')
        sscanf(tokens[p].str,"0X%xu",&val);
      else
        sscanf(tokens[p].str,"0x%xu",&val);
      return val;
    }
    else if(tokens[p].type == TK_ONUM){
      sscanf(tokens[p].str,"%uu",&val);
      return val;
    }
    else {
      printf("Error:Syntax error!");
      valid = false;
      return 0;
    }
  }
  else if(check_parentheses(p,q) == true) {
    return eval(p+1,q-1);
  }
  else {
    if(checklegal(p,q) == false)
    {
      valid = false;
      printf("Error:Too many parentheses!\n");
      return 0;
    }
    uint32_t op = findmainop(p,q);
    int val1 = eval(p,op-1);
    int val2 = eval(op+1,q);

    switch(tokens[op].type)
    {
      case '+': return val1 + val2;
      case '-': return val1 - val2;
      case '*': return val1 * val2;
      case '/': return val1 / val2;
      default: {
        printf("Error:Invalid operation!\n");
        valid = false;
        return 0;
      }
    }
  }
}
//检查满足BNF的括号表达式
bool check_parentheses(uint32_t p,uint32_t q)
{
  //不符合BNF的要求
  if(tokens[p].type != TK_LP || tokens[q].type != TK_RP)
    return false;
  else //外部有括号，保证内部合法即可，不需符合BNF
  {
    p++;q--;
    return checklegal(p,q);
  }
}

//用于检查括号表达式是否合法，但没有检查谁和谁匹配
bool checklegal(uint32_t p,uint32_t q)
{
  uint32_t lcount = 0;
  while(p <= q)
  {
    if(tokens[p].type != TK_LP && tokens[p].type != TK_RP)
      p++;
    else if(tokens[p].type == TK_LP)
    {
      lcount++;p++;
    }
    else
    {
      lcount--;p++;
      if(lcount < 0) //')'过多
        return false;
    }
  }
  if(lcount != 0) //'('过多
    return false;
  return true;
}

//如果要识别负号，在这里实现，主要是依靠两个符号相邻取哪个符号为当前主符号和下一步的步长
uint32_t findmainop(uint32_t p,uint32_t q)
{
  uint32_t position = p;
  while(p < q)
  {
    if(tokens[p].type == TK_LP)
    {
      int lcount = 1; p++;
      while(lcount != 0)
      {
        if(tokens[p].type == TK_LP) {
          lcount++;p++;
        }
        else {
          lcount--;p++;
        }
      }//跳出while时指向匹配的右括号后的第一个字符
    }
    else if(tokens[p].type != '+' && tokens[p].type != '-' && tokens[p].type != '*' && tokens[p].type != '/')
      p++;
    else
    {
      position = p;
      p++;
    }
  }
  return position;
}