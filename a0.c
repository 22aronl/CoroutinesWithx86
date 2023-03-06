#include <stdio.h>
#include <inttypes.h>

#include "src/go.h"

int counter = 0;

Value h() {
    send(me(), asInt(counter));
    if(counter < 2) {
	counter ++;
	again();
    }
    return asInt(666);
}

int main() {
    Channel* child = go(h);

    int c = receive(child).asInt;
    int d = receive(child).asInt;
    int e = receive(child).asInt;
    printf("%d%d%d\n", c, d, e);
    return 0;
    

}
