/*
This testcase tests the again() on main by creating two co-routines that get repeatedly called in main
It also tests sending and recieving on channels between to arbritrary co-routines and if they work
f1 sends to f2 who returns the value repeatedely to main after which main recurses using again until global a reaches 9
 */

#include <stdio.h>
#include "src/go.h"

int a = -9;
Channel* ch;

Value f2() {
  return receive(ch);
}

Value f1() {
  send(me(), asInt(++a));
  return asInt(0);
}

int main() {
  ch = go(f1);
  Channel* ch2 = go(f2);
  int k = receive(ch2).asInt; 
  if(k == 10) return 0;
  printf("%d\n", k);
  again();
}
