#include <stdio.h>
#include <stdlib.h>
#include "src/go.h"

// This tests stack and register preservation during c-routine jumps

// Test 0: Registers used to store variables should be saved while jumping////////////////////////////////////////

//Fills all registers and makes sure they don't change when switching
Value fill_registers() {
    int a = rand()/10*10 + 0;
    int b = rand()/10*10 + 1;
    int c = rand()/10*10 + 2;
    int d = rand()/10*10 + 3;
    int e = rand()/10*10 + 4;
    int f = rand()/10*10 + 5;
    int g = rand()/10*10 + 6;
    int h = rand()/10*10 + 7;
    int i = rand()/10*10 + 8;
    int j = rand()/10*10 + 9;
    send(me(),asInt(13));
    //test0 should print 0 to 9 here
    send(me(),asInt(13));
    printf("%d\n", a % 10);  //Prints 0 to 9
    printf("%d\n", b % 10);
    printf("%d\n", c % 10);
    printf("%d\n", d % 10);
    printf("%d\n", e % 10);
    printf("%d\n", f % 10);
    printf("%d\n", g % 10);
    printf("%d\n", h % 10);
    printf("%d\n", i % 10);
    printf("%d\n", j % 10);
    return asLong(13);
}

//Fills all registers and makes sure they don't change when switching
void test0() {
    printf("starting test0\n");
    Channel * ch = go(fill_registers);
    int a = rand()/10*10 + 0;
    int b = rand()/10*10 + 1;
    int c = rand()/10*10 + 2;
    int d = rand()/10*10 + 3;
    int e = rand()/10*10 + 4;
    int f = rand()/10*10 + 5;
    int g = rand()/10*10 + 6;
    int h = rand()/10*10 + 7;
    int i = rand()/10*10 + 8;
    int j = rand()/10*10 + 9;
    receive(ch);
    printf("%d\n", a % 10);  //Prints 0 to 9
    printf("%d\n", b % 10);
    printf("%d\n", c % 10);
    printf("%d\n", d % 10);
    printf("%d\n", e % 10);
    printf("%d\n", f % 10);
    printf("%d\n", g % 10);
    printf("%d\n", h % 10);
    printf("%d\n", i % 10);
    printf("%d\n", j % 10);
    receive(ch);
    // fill_registers should print 0 to 9 here
    receive(ch);
}

//Test 1: Variables on the stack should be saved while jumping ///////////////////////////////////////////////////////

//Fills top 10 items on stack with array and makes sure it doesn't change when switching
Value fill_stack() {
    int arr[10];
    for(int i = 0; i < 10; i++) {
        arr[i] = rand()/10*10 + i;
    }
    send(me(),asInt(13));
    //test0 should print 0 to 9 here
    send(me(),asInt(13));
    for(int i = 0; i < 10; i++) {
        printf("%d\n", arr[i] % 10);  //Prints 0 to 9
    }
    return asLong(13);
}

//Fills top 10 items on stack with array and makes sure it doesn't change when switching
void test1() {
    printf("starting test1\n");
    int arr[10];
    Channel * ch = go(fill_registers);
    for(int i = 0; i < 10; i++) {
        arr[i] = rand()/10*10 + i;
    }
    receive(ch);
    for(int i = 0; i < 10; i++) {
        printf("%d\n", arr[i] % 10);  //Prints 0 to 9
    }
    receive(ch);
    // fill_stack should print 0 to 9 here
    receive(ch);
}

//Test 1: Stack frames should be preserved while jumping  ///////////////////////////////////////////////////////

Channel *ch;

//Creates a stack frame
void receive_then_print(int i) {
    send(ch, asInt(13));
    printf("receive_then_print %d\n", i);  //Prints this for test2 then call_function
    receive(ch);
    return;
}

//Calls receive_then_print then make sure it returns properly
Value call_function() {
    receive(ch);
    receive_then_print(1);
    printf("back to call_function\n");  //prints this 4th
    receive(ch);
    return asInt(0);
}

//Calls receive_then_print then make sure it returns properly
void test2() {
    printf("starting test2\n");  //prints this 1st
    ch = go(call_function);
    receive_then_print(0);
    send(ch, asInt(13));
    send(ch, asInt(13));
    printf("back to test2\n");  //Prints this last
}

int main() {
    test0();
    test1();
    test2();
    return 0;
}
