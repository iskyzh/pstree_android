#include <pthread.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <unistd.h> 
#include <semaphore.h>
#include <stdarg.h>

#define ENABLE_YIELD
#define RUN_FOR_SECS 1

struct cook {
    pthread_t tid;
    int id;
    char identifier[256];
} *cooks;

struct cashier {
    pthread_t tid;
    int id;
    char identifier[256];
    sem_t queue, ready, serve;
    int customer;
} *cashiers;

struct customer {
    pthread_t tid;
    int id;
    char identifier[256];
} *customers;

sem_t rack_empty;
sem_t rack_full;
pthread_mutex_t info_lock;

int MAX_ON_RACK = 0, MAX_COOK = 0, MAX_CUSTOMER = 0, MAX_CASHIER = 0;

void yield() {
#ifdef ENABLE_YIELD
    usleep(rand() % 5000 + 5000);
#endif
}

/// a synchronized printf for log output
/// therefore logs can be printed out line by line
int printf_log(const char *format, ...) {
    int ret_val;
    va_list args;
    va_start(args, format);

    pthread_mutex_lock(&info_lock);
    ret_val = vprintf(format, args);
    pthread_mutex_unlock(&info_lock);

    va_end(args);

    return ret_val;
}

/// cook thread routine
/// @param _self: struct cook* that holds cook information
void* cook_do(void* _self) {
    struct cook* self = _self;
    yield();
    while (1) {
        sem_wait(&rack_full);
        printf_log("%s make a burger.\n", self->identifier);
        sem_post(&rack_empty);
        yield();
    }
    return NULL;
}

/// cashier thread routine
/// @param _self: struct cashier* that holds cashier information
void* cashier_do(void* _self) {
    struct cashier* self = _self;
    yield();
    while (1) {
        sem_wait(&self->queue);
        printf_log("%s accepts an order.\n", self->identifier);
        sem_post(&self->ready);
        // wait for burger on rack
        sem_wait(&rack_empty);
        // notify customer
        printf_log("%s take a burger to customer.\n", self->identifier);
        sem_post(&rack_full);
        sem_post(&self->serve);
        yield();
    }
    return NULL;
}

/// customer thread routine
/// @param _self: struct customer* that holds customer information
void* customer_do(void* _self) {
    struct customer* self = _self;
    yield();
    while (1) {
        // randomly select one cashier
        struct cashier* c = &cashiers[rand() % MAX_CASHIER];
        printf_log("%s come.\n", self->identifier);
        // wake up the cashier
        sem_post(&c->queue);
        // wait for cashier ready
        sem_wait(&c->ready);
        // wait for cashier serve
        sem_wait(&c->serve);
        printf_log("%s get a burger.\n", self->identifier);
        yield();
    }
    return NULL;
}

/// timeout thread
/// if this thread exits, the process should end
void* timeout_do() {
    int i;
    for (i = 0; i < RUN_FOR_SECS; i++) sleep(1);
    return NULL;
}

  
int main(int argc, char* argv[])  {
    int i;
    pthread_t timeout;

    if (argc == 5) {
        MAX_COOK = atoi(argv[1]);
        MAX_CASHIER = atoi(argv[2]);
        MAX_CUSTOMER = atoi(argv[3]);
        MAX_ON_RACK = atoi(argv[4]);
    } else {
        printf("usage: BBC cookers cashiers customers rack_size\n");
        return 1;
    }

    printf("Cooks [%d], Cashiers [%d], Customers [%d], Rack[%d]\n", MAX_COOK, MAX_CASHIER, MAX_CUSTOMER, MAX_ON_RACK);

    // allocate memory for structs
    cooks = (struct cook*) malloc(MAX_COOK * sizeof (struct cook));
    cashiers = (struct cashier*) malloc(MAX_CASHIER * sizeof (struct cashier));
    customers = (struct customer*) malloc(MAX_CUSTOMER * sizeof (struct customer));

    // initialize mutex for logging
    if (pthread_mutex_init(&info_lock, NULL) != 0) {
        printf("failed to initialize info lock\n");
        return 1;
    }

    if (sem_init(&rack_full, 0, MAX_ON_RACK) != 0) {
        printf_log("failed to initialize rack full semaphore.\n");
        return 1;
    }

    if (sem_init(&rack_empty, 0, 0) != 0) {
        printf_log("failed to initialize rack empty semaphore.\n");
        return 1;
    }

    for (i = 0; i < MAX_COOK; i++) {
        cooks[i].id = i;
        sprintf(cooks[i].identifier, "Cook [%d]", i);
    }

    for (i = 0; i < MAX_CUSTOMER; i++) {
        customers[i].id = i;
        sprintf(customers[i].identifier, "Customer [%d]", i);
    }

    for (i = 0; i < MAX_CASHIER; i++) {
        cashiers[i].id = i;
        cashiers[i].customer = -1;
        sprintf(cashiers[i].identifier, "Cashier [%d]", i);
        sem_init(&cashiers[i].queue, 0, 0);
        sem_init(&cashiers[i].ready, 0, 0);
        sem_init(&cashiers[i].serve, 0, 0);
    }

    printf("Begin run.\n");

    for (i = 0; i < MAX_COOK; i++) {
        pthread_create(&cooks[i].tid, NULL, cook_do, &cooks[i]);
    }

    for (i = 0; i < MAX_CUSTOMER; i++) {
        pthread_create(&customers[i].tid, NULL, customer_do, &customers[i]);
    }

    for (i = 0; i < MAX_CASHIER; i++) {
        pthread_create(&cashiers[i].tid, NULL, cashier_do, &cashiers[i]);
    }

#ifdef RUN_FOR_SECS
    pthread_create(&timeout, NULL, timeout_do, NULL);
    pthread_join(timeout, NULL);
    return 0;
#endif

    for (i = 0; i < MAX_COOK; i++) {
        pthread_join(cooks[i].tid, NULL);
    }
    for (i = 0; i < MAX_CUSTOMER; i++) {
        pthread_join(customers[i].tid, NULL);
    }
    for (i = 0; i < MAX_CASHIER; i++) {
        pthread_join(cashiers[i].tid, NULL);
        sem_destroy(&cashiers[i].queue);
        sem_destroy(&cashiers[i].ready);
        sem_destroy(&cashiers[i].serve);
    }

    sem_destroy(&rack_full);
    sem_destroy(&rack_empty);
    pthread_mutex_destroy(&info_lock);

    return 0; 
}
