#ifndef BINARY_SEM_H
#define BINARY_SEM_H

int reserveSem(int semId, int semNum);

int releaseSem(int semId, int semNum);

int initSemAvailable(int semId, int semNum);

int initSemInUse(int semId, int semNum);

int semDeallocate (int semid);

#endif