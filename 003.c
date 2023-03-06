#include <stdio.h>

#include "src/go.h"

// this function returns whatever value it receives first + 1 (as an int)
Value test() {
    Value return_value = receive(me());
    return asInt(return_value.asInt + 1);
}

int main() {
    Channel* channel = go(test);
    send(channel, asInt(10));

    // make sure that return provides an infinite source of the value
    // also checks that the send blocks, otherwise the first receive would get back its own value (10 instead of 11)
    for (int i = 0; i < 20; ++i) {
        printf("%d\n", receive(channel).asInt);
    }

    return 0;
}

