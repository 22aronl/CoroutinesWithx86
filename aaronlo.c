#include <stdio.h>
#include "src/go.h"

int j = 10;
int i = 1;
Channel* big;

Value f1() {
    if(j > 0) {
        j--;
        Channel* ch = go(f2);
        send(ch, asInt(9));
    }
    printf("%d\n", receive(me()).asInt);
    return asInt(11);
}

Value f2() {
    Channel* ch = go(f1);
    send(ch, asInt(10));
    return receive(me());
}

int main() {
    Channel* ch = go(f2);
    printf("%d\n", receive(ch).asInt);
    if(i > 0) {
        i--;
        again();
    }
    return 0;
}