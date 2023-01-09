 #include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>

// void *printId(void *vargp) {
//     int id = *((int *)vargp);
//     printf("%d\n", id);
//     return NULL;
// }

// int main() {
//     pthread_t tid[2];
//     int i;
//     for (i = 0; i < 2; i++)
//         pthread_create(&tid[i], NULL, printId, &i);
//     pthread_join(tid[0], NULL);
//     pthread_join(tid[1], NULL);
// }






// void change(int* xy){
//     *xy = 10;
// }

// typedef struct
// {

//     int x;
//     int y;
// } Position;

// Position *posArr;
// void initPosArr (Position *pos){
//     for(size_t x = 0; x < 10; x++){
//         Position p;
//         p.x = -1;
//         p.y = -1;
//         pos[x] = p;
//     }

// }
// Position *pposArr;
// void initDubPosArr (Position **pos){
//     for(size_t x = 0; x < 10; x++){
//         Position *p;
//         p->x = -1;
//         p->y = -1;
//         pos[x] = p;
//     }

// }
// int main(int argc, char** argv)
// {
//     // char *name = argv[1];
//     // struct hostent *host = gethostbyname(name);
//     // puts(host->h_addr_list[0]);

//     // int *as = malloc(10*sizeof(int));
//     // as[0] = 1;
//     // if(as[0] == NULL)
//     //     printf("hi");

//     // int xy = -1;
//     // // int* xy = malloc(sizeof(int));
//     // // *xy = 1;
//     // change(&xy);
//     // posArr = malloc(sizeof(Position)*10);
//     // initPosArr (posArr);
//     // for(size_t x = 0; x < 10; x++){
//     //     printf("%ld: %d, %d\n",x, posArr[x].x, posArr[x].y);
//     // }
//     // Position pp;
//     // posArr[0] = pp;
//     // pp.x = 1;
//     // pp.y = 1;
//     // printf("\n");
//     // for(size_t x = 0; x < 10; x++){
//     //     printf("%ld: %d, %d\n",x, posArr[x].x, posArr[x].y);
//     // }
//     // Position pp1;
//     // pp1.x = 1;
//     // pp1.y = 1;
//     // posArr[0] = pp1;
//     // printf("\n");
//     // for(size_t x = 0; x < 10; x++){
//     //     printf("%ld: %d, %d\n",x, posArr[x].x, posArr[x].y);
//     // }


//     while(1){
//         char buf[256];
//         if((fgets(buf, 256, stdin))){
//             printf("ha");
//             if(buf[0] == 'c'){
//                 printf("h");
//             break;}
//         }
        
//         // break;
//     }

    
//     // printf("xy:%d ", xy);
//     // posArr  = malloc(sizeof(Position)* 10);
//     // posArr[10]
//     // printf("%ld", sizeof(Position));





//     // int *heap = malloc(10000* sizeof(int));

//     // heap[9999] = 10100;
//     // printf("%d", heap[9999]);
//     // int size = 9;
//     // int numBlock = size % 8 == 0 ? (size / 8) : (int)(size / 8) + 1;

//     // printf("%d\n", numBlock);
//     // int index = 1;
//     // int *heap = malloc(18 * sizeof(int));
//     // heap[index] = numBlock * 8;
//     // // 1 means it is now occoupied
//     // int end = index + (numBlock * 8) - 1;
//     // printf("%d\n", end);
//     // heap[end] = 1;
//     // // int *heap1[end + 2];
//     // int heap1[end-1];
//     // for(int x= 0; x < end; x++)
//     //     heap1[x] = heap [index++];
    
//     // void *ptr = &heap1[0];
//     // int *ptr1 = ptr;
//     // for (size_t x = 0; x < end ; x++)
//     // {
//     //     printf("c: %d\n", ptr1[x]);
//     //     // heap1[x] = heap[index++];
//     // }
//     // printf("%d\n %d", heap[16], ptr1[end+1]);

//     // size_t *heap = malloc(10);
//     // heap[0] = 5;
//     // size_t *freeHeadPtr = &heap[0]; //sets this as free block head
//     // size_t *ptr = freeHeadPtr;
//     // // if(ptr == NULL)
//     // //     printf("NULL");
//     // // printf("%ld", *ptr & -2);
//     // // ptr = (size_t *)(*(ptr + 2));
//     // printf("%ld\n", *freeHeadPtr);
//     // *freeHeadPtr = 6;

//     //     printf("%ld", *freeHeadPtr);

//     return 0;


// }



// /*
//  * client.c
//  * Version 20161003
//  * Written by Harry Wong (RedAndBlueEraser)
//  */

// // #include <netdb.h>
// // #include <stdio.h>
// // #include <stdlib.h>
// // #include <string.h>
// // #include <sys/types.h>
// // #include <sys/socket.h>
// // #include <unistd.h>

// // #define SERVER_NAME_LEN_MAX 255

// // int main(int argc, char *argv[]) {
// //     char server_name[SERVER_NAME_LEN_MAX + 1] = { 0 };
// //     int server_port, socket_fd;
// //     struct hostent *server_host;
// //     struct sockaddr_in server_address;

// //     /* Get server name from command line arguments or stdin. */
// //     if (argc > 1) {
// //         strncpy(server_name, argv[1], SERVER_NAME_LEN_MAX);
// //     } else {
// //         printf("Enter Server Name: ");
// //         scanf("%s", server_name);
// //     }

// //     /* Get server port from command line arguments or stdin. */
// //     server_port = argc > 2 ? atoi(argv[2]) : 0;
// //     if (!server_port) {
// //         printf("Enter Port: ");
// //         scanf("%d", &server_port);
// //     }

// //     /* Get server host from server name. */
// //     server_host = gethostbyname(server_name);

// //     /* Initialise IPv4 server address with server host. */
// //     memset(&server_address, 0, sizeof server_address);
// //     server_address.sin_family = AF_INET;
// //     server_address.sin_port = htons(server_port);
// //     memcpy(&server_address.sin_addr.s_addr, server_host->h_addr, server_host->h_length);

// //     /* Create TCP socket. */
// //     if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
// //         perror("socket");
// //         exit(1);
// //     }

// //     /* Connect to socket with server address. */
// //     if (connect(socket_fd, (struct sockaddr *)&server_address, sizeof server_address) == -1) {
// // 		perror("connect");
// //         exit(1);
// // 	}

// //     /* TODO: Put server interaction code here. For example, use
// //      * write(socket_fd,,) and read(socket_fd,,) to send and receive messages
// //      * with the client.
// //      */

// //     close(socket_fd);
// //     return 0;
// // }