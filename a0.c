#include <stdio.h>
#include <inttypes.h>

#include "src/go.h"

Value echo()
{
    send(me(),asLong(1));
    return asLong(3);
}

int main()
{
    Channel* ch = go(echo);
    printf("one");
    printf("%ld\n", receive(ch).asLong);
    printf("finish");

    return 0;
}

