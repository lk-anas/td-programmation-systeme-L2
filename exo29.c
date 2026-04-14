// ldd: -lrt
// ldd: -lpthread

#include <stdlib.h>
#include <stdio.h> 
#include <pthread.h>
#include <mqueue.h>
#include <semaphore.h>
#include <fcntl.h>

typedef unsigned int uint;

typedef struct {
  uint len;
  int* val;
} Tab;

void swap (Tab t, uint i, uint j) {
  int tmp = t.val[i];
  t.val[i] = t.val[j];
  t.val[j] = tmp;
}

Tab slice (Tab t, uint first, uint last) {
  Tab s;
  s.len = last - first + 1;
  s.val = t.val + first;
  return s;
}

uint partition (Tab t) {
  int pivot = t.val[(t.len-1)/2];
  uint a = -1;
  uint b = t.len;
  while (1) {
    do { a++; } while (t.val[a] < pivot);
    do { b--; } while (t.val[b] > pivot);
    if (a >= b) { return b; }
    swap(t, a, b);
  }
}

void sort (Tab t) {
  uint p;
  if (t.len > 1) {
    p = partition(t);
    sort(slice(t, 0, p));
    sort(slice(t, p+1, t.len-1));
  }
}

// une tâche = les bornes d'un sous-tableau à partitionner
struct Task {
  uint first;
  uint last;
};

mqd_t queue;
sem_t sem;

pthread_cond_t  cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
int sort_done = 0;

Tab global_tab;

void finalize_one() {
  sem_wait(&sem);

  int val;
  sem_getvalue(&sem, &val);
  if (val == 0) {

    pthread_mutex_lock(&lock);
    sort_done = 1;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&lock);
  }
}

// fonction executué par les threads
void* start(void* arg) {
  (void)arg; // just for warning
  while (1) {
    struct Task task;
      
    // recupère une tache de la file
    mq_receive(queue, (char*)&task, sizeof(struct Task), NULL);

    if (task.first == task.last) {
      finalize_one();
    } else {
      Tab sub = slice(global_tab, task.first, task.last);
      uint p = partition(sub);

      uint left_first  = task.first;
      uint left_last   = task.first + p;
      uint right_first = task.first + p + 1;
      uint right_last  = task.last;

      // si le sous tableau gauche a + d'un élément, on le met en file
      if (left_last > left_first) {
        struct Task t1 = {left_first, left_last};
        mq_send(queue, (char*)&t1, sizeof(struct Task), 0);
      } else {
        finalize_one(); // sinon taille = 1 -> déjà trié
      }

      // idem pour sous tableau droit
      if (right_last > right_first) {
        struct Task t2 = {right_first, right_last};
        mq_send(queue, (char*)&t2, sizeof(struct Task), 0);
      } else {
        finalize_one();
      }
    }
  }
  return NULL;
}

// just for print tab
void printTab(Tab t) {
  printf("[");
  for (uint i = 0; i < t.len; i++) {
    printf("%d", t.val[i]);
    if (i < t.len - 1) printf(", ");
  }
  printf("]\n");
}

int main() {
  int valeurs[] = {5,1,3};
  global_tab.len = 3;
  global_tab.val = valeurs;

  printf("avant : ");
  printTab(global_tab);

  struct mq_attr attr = {.mq_maxmsg = 10, .mq_msgsize = sizeof(struct Task)};

  // initialisation de la file
  mq_unlink("/myqueue");
  queue = mq_open("/myqueue", O_RDWR | O_CREAT | O_EXCL, 0660, &attr);
  if (queue == (mqd_t)-1) {
    perror("mq_open");
    exit(EXIT_FAILURE);
  }
  
  // initialisation du semaphore
  if (sem_init(&sem, 0, global_tab.len) == -1) {
    perror("sem_init");
    exit(EXIT_FAILURE);
  }

  // tache initiale : tout le tableau
  struct Task first = {0, global_tab.len - 1};
  if (mq_send(queue, (char*)&first, sizeof(struct Task), 0) == -1) {
    perror("mq_send");
    exit(EXIT_FAILURE);
  }

  // création des threads
  pthread_t threads[4];
  for (int i = 0; i < 4; i++) {
    if (pthread_create(&threads[i], NULL, start, NULL) != 0) {
      perror("pthread_create");
      exit(EXIT_FAILURE);
    }
  }

  pthread_mutex_lock(&lock);
  while (sort_done != 1) {
    pthread_cond_wait(&cond, &lock);
  }
  pthread_mutex_unlock(&lock);

  printf("après : ");
  printTab(global_tab);
 
  for (int i = 0; i < 4; i++) {
    pthread_cancel(threads[i]);
    pthread_join(threads[i], NULL);
  }

  mq_close(queue);
  mq_unlink("/myqueue");

}




















