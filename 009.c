#include <stdio.h>
#include "src/go.h"

// This test checks if your implementation can handle multiple send/receives followed by an again(). This test also ensures that you can get an infinite number of the return value from a C-routine's associated function when it returns.

int j = 0;

Value f1(){
    for(int i = 0; i < 5; i++){
        send(me(), asInt(i));
    }
    if(j < 5){
        j += 1;
        again();
    }
    return asInt(-1);
}

int main() {
    Channel* ch = go(f1);
    while(1){
        Value v = receive(ch);
        if(v.asInt == -1){
            for(int i = 0; i < 100; i++){
                v = receive(ch);
                if(v.asInt != -1){
                    printf("The channel does not contain an infinite supply of f1's return value\n");
                }
            }
            printf("DONE!\n");
            return 0;
        }
        printf("%d\n", v.asInt);
    }
    printf("You should NOT see this statement.\n");
    return 0;
}
