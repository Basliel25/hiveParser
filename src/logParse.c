#include <string.h>
#include "logParse.h"
#include "log_parser/parser.h"
#include "report_parse/report.h"

// Number of working threads
static const int NUMTHREADS = 8;


int main() {
   FILE *log = fopen("log_data/Linux_2k.log", "r"); 

   workQueue_t *work_Queue = malloc(sizeof(workQueue_t));
   memset(work_Queue, 0, sizeof(workQueue_t)); 

   pthread_t thread[NUMTHREADS];

   Node *tail = NULL; 
   Node *head = NULL;

   work_Queue->size = 0;
   work_Queue->head = head;
   work_Queue->tail = tail;
   work_Queue->flag = 0;
   pthread_mutex_init(&work_Queue->lock, NULL);
   pthread_cond_init(&work_Queue->cond, NULL);

   // **************
   // Populate Queue
   // **************

   if (log == NULL) {
    perror("Error opening file");
    return 1;
   }

   char buffer[1024];  // Fixed size buffer

   // ****************
   // Spin work Threads
   // ****************
   thread_args_t *args[NUMTHREADS];
   for(int i = 0; i < NUMTHREADS; i++) {
       args[i] = malloc(sizeof(thread_args_t));
       args[i]->thread_id = i;
       args[i]->queue = work_Queue;
       report_init(&args[i]->thread_report);
       pthread_create(&thread[i], NULL, worker_thread, (void *)args[i]);
   }

   while (fgets(buffer, sizeof(buffer), log) != NULL) {
       char *line = strdup(buffer);
       pthread_mutex_lock(&work_Queue->lock);
       enqueue(work_Queue, line);
       pthread_cond_signal(&work_Queue->cond);
       pthread_mutex_unlock(&work_Queue->lock);
   }
   fclose(log);

   printf("Size is of queue is at %d\n", work_Queue->size);

   // Signal workers that no more work is coming
   pthread_mutex_lock(&work_Queue->lock);
   work_Queue->flag = 1;
   pthread_cond_broadcast(&work_Queue->cond);
   pthread_mutex_unlock(&work_Queue->lock);

   // Wait for all workers to finish
   for(int i = 0; i < NUMTHREADS; i++) {
       pthread_join(thread[i], NULL);
   }

   // Merge Reports
   report_t global_report;
   report_init(&global_report);

   for(int i = 0; i < NUMTHREADS; i++) {
       merge_report(&global_report, &args[i]->thread_report);  
       free(args[i]);  
   }
   printf("All workers finished\n");
   print_report(&global_report);
   free(work_Queue);
   return 0;
}

void enqueue(workQueue_t *queue, char *line) {
    Node *new_node = malloc(sizeof(Node));
    new_node->line = line;
    new_node->next = NULL;

    // Caller holds the lock - just manipulate the queue
    if(!queue->tail) {
        queue->tail = queue->head = new_node;
    } else {
        queue->tail->next = new_node;
        queue->tail = new_node;
    }
    queue->size++;
}

char *dequeue(workQueue_t *queue) {

    Node *node = queue->head;
    if(queue->size == 0) return NULL;
    queue->head = queue->head->next;
    if (!queue->head) queue->tail = NULL;
    queue->size--;

    char *line = node->line;
    free(node);
    return line;
}


void *worker_thread(void *arg) {
    thread_args_t *args = (thread_args_t *)arg;
    int tid = args->thread_id;
    workQueue_t *work_queue = args->queue;
    char *line;

    printf("[Worker %d] Starting\n", tid);

    while(1) {
        pthread_mutex_lock(&work_queue->lock);

        // Wait until work arrives OR done flag is set
        while (work_queue->size == 0 && !work_queue->flag) {
            pthread_cond_wait(&work_queue->cond, &work_queue->lock);
        }

        // If queue still empty and done, exit
        if (work_queue->size == 0 && work_queue->flag) {
            pthread_mutex_unlock(&work_queue->lock);
            printf("[Worker %d] Exiting\n", tid);
            break;
        }
        line = dequeue(work_queue);
        pthread_mutex_unlock(&work_queue->lock);
        
        log_entry_t *entry = extract_entry(line);
        update_report(entry, &args->thread_report);
        free(entry);
        free(line);
    }
    
    return NULL;
}
