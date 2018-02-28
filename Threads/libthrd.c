#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include "libthrd.h"

int lanceThread(void* (*fonction)(void*), void* arg, int taille){
    pthread_t tid;
    pthread_attr_t attr;
    thread_param_t* arguments = (thread_param_t*)malloc(sizeof(thread_param_t));
    if(arguments == NULL){
        perror("lanceThread.malloc");
        return -1;
    }
    if(pthread_attr_init(&attr) != 0){
        perror("lanceThread.pthread_attr_init");
        return -1;
    }
    if(pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED) != 0){
        perror("lanceThread.pthread_attr_setdetachstate");
        return -1;
    }
    arguments->arg = arg;
    arguments->taille = taille;
    if(pthread_create(&tid, &attr, fonction, (void*)arguments) != 0){
       perror("lanceThread.pthread_create");
      return -1;
    }
   return 0;
} 

