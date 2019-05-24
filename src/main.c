/* Contador de números primos
 *
 * Este programa recebera uma serie de caracteres representando números em sua
 * entrada. Ao receber um caractere fim de linha ('\n'), deve imprimir na tela a
 * quantidade de números primos que recebeu e, após, encerrar.
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#define THREADS 4
#define BUSY 1
#define FREE 0

typedef struct {
  unsigned long value;
  int status;
} thread_data;

pthread_mutex_t lock;
pthread_t workers[THREADS];
int worker_status[THREADS];
int active_threads=0; /* used only for debug */
int primes_found=0;

int prime(unsigned long n);
void feed(unsigned long n);
void *worker(void *data);

int main() {
  unsigned long u;
  int i;
  while(scanf(" %lu", &u)!= -1) {
    while(active_threads>=THREADS);
    //printf("M\tActive threads: %d\n", active_threads);

    /* Create new task */
    feed(u);
  }

  /* wait for all threads to finish */
  for(i=0 ; i<THREADS ; i++) {
    if(worker_status[i]==BUSY) {
      //printf("M\tWaiting for thread %d\n", i);
      pthread_join(workers[i], NULL);
    }
  }
  printf("%d\n", primes_found);
  return 0;
}


void feed(unsigned long n) {
  thread_data *data;
  int i=0;

  // printf("Generating thread for: %lu\n", n);
  data = (thread_data *)malloc(sizeof(thread_data));
  data->value = n;

  pthread_mutex_lock(&lock);
  while(worker_status[i]==BUSY) i++; /* Search for unused id */
  data->status = i;
  worker_status[i] = BUSY;
  
  pthread_create(&(workers[i]), NULL, worker, (void *) data);
  active_threads++;
  pthread_mutex_unlock(&lock);
}

void *worker(void *data) {
  thread_data *info = (thread_data *)data;
  int r = 0;

  r = prime(info->value);
  // printf("T%d\tCalculating for: %lu ... %s\n",info->status, info->value, r? "TRUE":"FALSE");

  pthread_mutex_lock(&lock);
  primes_found += r;
  worker_status[info->status] = FREE;
  active_threads--;
  pthread_mutex_unlock(&lock);

  free(data);
  
  return NULL;
}

int prime(unsigned long n) {
  unsigned d, isPrime;
  d=2;
  isPrime=1;		/* Assume that n is prime at first */
  if (n<=1)
    isPrime=0;
  while (isPrime==1 && d<=n/2) {
    if (n % d==0)
      isPrime=0;
    d=d+1;
  }
  return isPrime;
}
