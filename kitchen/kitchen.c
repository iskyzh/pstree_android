#include <pthread.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <unistd.h> 
#include <semaphore.h>
#include <stdarg.h>

#define MAX_COOK 5
#define MAX_CASHIER 3
#define MAX_CUSTOMER 3
#define MAX_ON_RACK 10

#define ENABLE_YIELD

struct cook {
    pthread_t tid;
    int id;
    char identifier[256];
} cooks[MAX_COOK];

struct cashier {
    pthread_t tid;
    int id;
    char identifier[256];
    pthread_cond_t cond;
    pthread_mutex_t lock;
    int customer;
} cashiers[MAX_CASHIER];

struct customer {
    pthread_t tid;
    int id;
    char identifier[256];
    pthread_cond_t cond;
    pthread_mutex_t lock;
    int bought;
} customers[MAX_CUSTOMER];

sem_t rack;
pthread_mutex_t info_lock;

void yield() {
#ifdef ENABLE_YIELD
    usleep(rand() % 500000 + 500000);
#endif
}

int printf_log(const char *format, ...) {
    va_list args;
    va_start(args, format);

    pthread_mutex_lock(&info_lock);
    vprintf(format, args);
    pthread_mutex_unlock(&info_lock);

    va_end(args);
}

void* cook_do(void* _self) {
    struct cook* self = _self;
    /* printf_log("cook #%d\n", self->id); */
    while (1) {
        sem_wait(&rack);
        printf_log("[%15s] made a burger.\n", self->identifier);
        yield();
    }
    return NULL;
}

void* cashier_do(void* _self) {
    struct cashier* self = _self;
    /* printf_log("cashier #%d\n", self->id); */
    while (1) {
        struct customer* c;
        pthread_mutex_lock(&self->lock);
        while (self->customer == -1) pthread_cond_wait(&self->cond, &self->lock);
        c = &customers[self->customer];
        pthread_mutex_unlock(&self->lock);

        printf_log("[%15s] processing order from %s\n", self->identifier, c->identifier);

        sem_post(&rack);

        pthread_mutex_lock(&c->lock);
        pthread_cond_signal(&c->cond);
        c->bought = 1;
        pthread_mutex_unlock(&c->lock);

        pthread_mutex_lock(&self->lock);
        self->customer = -1;
        pthread_mutex_unlock(&self->lock);

        yield();
    }
    return NULL;
}

struct cashier* customer_select_cashier_and_lock() {
    int i;
    while (1) {
        for (i = 0; i < MAX_CASHIER; i++) {
            if (pthread_mutex_trylock(&cashiers[i].lock) == 0) {
                if (cashiers[i].customer == -1) {
                    return &cashiers[i];
                }
                pthread_mutex_unlock(&cashiers[i].lock);
            }
        }
    }
}

void* customer_do(void* _self) {
    struct customer* self = _self;
    /* printf_log("customer #%d\n", self->id); */
    while (1) {
        struct cashier* c = customer_select_cashier_and_lock();
        pthread_cond_signal(&c->cond);
        c->customer = self->id;
        printf_log("[%15s] buy from %s\n", self->identifier, c->identifier);
        self->bought = 0;
        pthread_mutex_unlock(&c->lock);
        
        pthread_mutex_lock(&self->lock);
        while (self->bought == 0) pthread_cond_wait(&self->cond, &self->lock);
        pthread_mutex_unlock(&self->lock);
        printf_log("[%15s] burger get\n", self->identifier);
    }
    return NULL;
}
  
int main()  {
    int i;

    if (pthread_mutex_init(&info_lock, NULL) != 0) {
        printf("failed to initialize info lock\n");
        return 1;
    }

    if (sem_init(&rack, 0, MAX_ON_RACK) != 0) {
        printf_log("failed to initialize rack semaphore.\n");
        return 1;
    }

    for (i = 0; i < MAX_COOK; i++) {
        cooks[i].id = i;
        sprintf(cooks[i].identifier, "cook #%d", i);
    }

    for (i = 0; i < MAX_CUSTOMER; i++) {
        customers[i].id = i;
        pthread_mutex_init(&customers[i].lock, NULL);
        pthread_cond_init(&customers[i].cond, NULL);
        sprintf(customers[i].identifier, "customer #%d", i);
    }

    for (i = 0; i < MAX_CASHIER; i++) {
        cashiers[i].id = i;
        cashiers[i].customer = -1;
        sprintf(cashiers[i].identifier, "cashier #%d", i);
        pthread_mutex_init(&cashiers[i].lock, NULL);
        pthread_cond_init(&cashiers[i].cond, NULL);
    }

    for (i = 0; i < MAX_COOK; i++) {
        pthread_create(&cooks[i].tid, NULL, cook_do, &cooks[i]);
    }

    for (i = 0; i < MAX_CUSTOMER; i++) {
        pthread_create(&customers[i].tid, NULL, customer_do, &customers[i]);
    }

    for (i = 0; i < MAX_CASHIER; i++) {
        pthread_create(&cashiers[i].tid, NULL, cashier_do, &cashiers[i]);
    }

    for (i = 0; i < MAX_COOK; i++) {
        pthread_join(cooks[i].tid, NULL);
    }
    for (i = 0; i < MAX_CUSTOMER; i++) {
        pthread_join(customers[i].tid, NULL);
    }
    for (i = 0; i < MAX_CASHIER; i++) {
        pthread_join(cashiers[i].tid, NULL);
    }

    sem_destroy(&rack);
    pthread_mutex_destroy(&info_lock);

    return 0; 
}
