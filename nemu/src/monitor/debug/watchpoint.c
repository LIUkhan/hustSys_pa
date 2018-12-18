#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_,*tail,*freetail;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  tail = NULL;
  free_ = wp_pool;
  freetail = &(wp_pool[NR_WP-1]);
}

/* TODO: Implement the functionality of watchpoint */

//传入什么？
void setWP(char *exprinfo) {
  if(free_ == NULL) {
    printf("You can only set %d watchpoints!\n",NR_WP);
    return;
  }
  // bool success;
  assert(free_ == NULL);
  WP *new_wp = free_;
  if(free_->next == NULL) {
    freetail = NULL;
  }
  free_ = free_->next;
  if(head == NULL) {
    head = new_wp;
    tail = new_wp;
    tail->next = NULL;
  }
  else {
    tail->next = new_wp;
    tail = tail->next;
    tail->next = NULL;
  }
  bool success;
  strcpy(tail->exprbuf,exprinfo);
  tail->oldvalue = expr(exprinfo,&success);
  if(!success)
    printf("Error: The Experation isn't legal!\n");
  else
    printf("WatchPoint %u has set successfully!\n",tail->NO);
}

void deleteWP(uint32_t n) {
  WP *temp = head;
  WP *front = head;
  while(temp != NULL) {
    if(temp->NO == n)
    {
      front->next = temp->next;
      freetail->next = temp;
      freetail = freetail->next;
      freetail->next = NULL;
      printf("Success: remove watchpoint %u\n",n);
      return;
    }
    front = temp;
    temp = temp->next;
  }
  printf("Watchpoint %u is free.Delete Error!\n",n);
}

bool checkWP()
{
  bool change = false;
  bool success;
  WP* temp = head;
  while(temp != NULL) {
    if(temp->oldvalue != expr(temp->exprbuf,&success))
    {
      printf("WatchPoint %u has changed!\n",temp->NO);
      change = true;
    }
  }
  return change;
}

bool hasWP()
{
  if(head == NULL)
    return false;
  else
    return true;
}

void outWPinfo()
{
  WP* temp = head;
  while(temp != NULL) {
    printf("%6u%20s%10u\n",temp->NO,temp->exprbuf,temp->oldvalue);
  }
}