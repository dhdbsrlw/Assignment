#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

void* magic_box(void* arg) {
    printf("multiplying %d by 6...\n", (int)arg);
    pthread_exit((void *)((int)arg * 6)); // modified
}


int main(int argc, char* argv[]){
    pthread_t tid;
    int* new_number;
    int my_num = 10;

    printf("Hey magic box, multiply 10 by 6\n");

    int status = pthread_create(&tid, NULL, magic_box , (void *)(intptr_t)my_num); // </1> </2>

    if(status!=0){
        printf("error");
        return -1;
    }

    pthread_join(tid, (void **)&new_number);  // </3.1> </3.2>
    printf("the new number is %d\n", new_number); 

    return 0;
}

/*
Expected output:

Hey magic box, multiply 10 by 6
multiplying 10 by 6...
the new number is 60
*/

