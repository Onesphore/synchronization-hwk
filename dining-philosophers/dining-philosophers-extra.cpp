#include <stdio.h>
#include <pthread.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

#include <iostream>
#include <string>

#define ERROR(msg)            \
	do {                  \
          perror(msg);        \
	  exit(EXIT_FAILURE); \
	} while(0)            \


#define tableState_signal SIGINT
#define pause_thread_signal SIGUSR2

using std::cout;
using std::endl;
using std::string;

enum ph_STATE {LEFT_CH, RIGHT_CH, EATING, THINKING};

struct ph_arg_t {
  int id;
  pthread_mutex_t* less_ch;  // left chopstick
  pthread_mutex_t* higher_ch; // right chopstick

  int* less_ch_owner;
  int* higher_ch_owner;
};

pthread_t* philosophers;
int* philosophers_count;
int* chopsticks_owner;
enum ph_STATE* states;

// the function each thread will run
static void* philosopher_func(void* arg);
// TODO: short description
static void getTableState_sighandler(int);

int main(int argc, char* argv[]) {
  int ph_num;

  assert((ph_num = atoi(argv[1])) > 2); // sanity check

  pthread_mutex_t chopsticks[ph_num];
  ph_arg_t ph_args[ph_num];

  philosophers = (pthread_t*) malloc(ph_num * sizeof(pthread_t));
  philosophers_count = (int*) malloc(1 * sizeof(int));
  *philosophers_count = ph_num;
  chopsticks_owner = (int*) malloc(ph_num * sizeof(int));
  states = (enum ph_STATE*) malloc(ph_num * sizeof(enum ph_STATE));

  // install signal handlers
  if (signal(tableState_signal, getTableState_sighandler) == SIG_ERR) {
    ERROR("signal()");
  }

  // initialize mutexes, owners, and states
  for (int i=0; i<ph_num; ++i) {
    pthread_mutex_init(&chopsticks[i], NULL);
    chopsticks_owner[i] = -1;
    *(states+i) = THINKING;
  }

  // launch the threads
  ph_args[0] = {
    id : 0,
    less_ch : &chopsticks[0],
    higher_ch : &chopsticks[ph_num-1],
    less_ch_owner : &chopsticks_owner[0],
    higher_ch_owner : &chopsticks_owner[ph_num-1],
    // states_ptr : states
  };

  if (pthread_create(&philosophers[0], NULL, 
                        philosopher_func, &ph_args[0]) != 0) {
    ERROR("pthread_create()");
  }
  for (int i=1; i<ph_num; ++i) {
    ph_args[i] = {
      id : i,
      less_ch : &chopsticks[i-1],
      higher_ch : &chopsticks[i],
      less_ch_owner : &chopsticks_owner[i-1],
      higher_ch_owner : &chopsticks_owner[i],
    };
    if (pthread_create(&philosophers[i], NULL, 
	                  philosopher_func, &ph_args[i]) != 0) {
      ERROR("pthread_create()");
    }
  }

  while(1); // wait for the thread

  return 0;
}

static void*
philosopher_func(void* arg) {
  ph_arg_t* ph_arg = (ph_arg_t*) arg;
  int id = ph_arg->id;
  pthread_mutex_t* less_ch = ph_arg->less_ch;
  pthread_mutex_t* higher_ch = ph_arg->higher_ch;
  int* less_ch_owner = ph_arg->less_ch_owner;
  int* higher_ch_owner = ph_arg->higher_ch_owner;
  // enum ph_STATE* states_ptr = ph_arg->states_ptr;

  while (1) {
    // Try grabing both chopsticks
    *(states+id) = THINKING;
    pthread_mutex_lock(less_ch);
    if (id == 0)
      *(states+id) = RIGHT_CH;
    else
      *(states+id) = LEFT_CH;
    *less_ch_owner = id;
    pthread_mutex_lock(higher_ch);
    *(states+id) = EATING;
    *higher_ch_owner = id;
    // Eat
    sleep(2);

    // Release the chopsticks
    *less_ch_owner = -1;
    pthread_mutex_unlock(less_ch);
    *higher_ch_owner = -1;
    pthread_mutex_unlock(higher_ch);
    *(states+id) = THINKING;
    // Think
    sleep(rand() % 5);

    // break;
  }
  return NULL;
}

static void 
getTableState_sighandler(int _signal) {
  assert(*philosophers_count > 0); // sanity check

  // chopsticks owners
  for (int i=0; i<*philosophers_count; ++i) {
    cout << "chopstick " << i << " holder: " 
	 << chopsticks_owner[i] << endl;
  }
  // philosophers states
  string state;
  for (int i=0; i<*philosophers_count; ++i) {
    switch (*(states+i)) {
      case LEFT_CH:
        state = "has only chopstick on the left";
	break;
      case RIGHT_CH:
	state = "has only chopstick on the right";
	break;
      case EATING:
	state = "is eating (has both chopsticks)";
	break;
      case THINKING:
	state = "is thinking (has no chopstick)";
	break;
      default:
	state = "weird state";
    }

    cout << "philosopher " << i << " state: "
	 << state << endl;
  }
  return;
}
