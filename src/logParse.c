#include <string.h>
#include "logParse.h"
#include "log_parser/parser.h"

static const int NUMTHREADS = 4;

int main() {
   FILE *log = fopen("log_data/data.log", "r"); 

   workQueue_t *work_Queue = malloc(sizeof(workQueue_t));
   memset(work_Queue, 0, sizeof(workQueue_t)); 

   pthread_mutex_t *lock;
   pthread_mutex_init(&work_Queue->lock, NULL);

   pthread_t thread[NUMTHREADS];

   Node *tail = NULL; 
   Node *head = NULL;

   work_Queue->size = 0;
   work_Queue->head = head;
   work_Queue->tail = tail;
   work_Queue->flag = 0;


   if (log == NULL) {
    perror("Error opening file");
    return 1;
   }

   char buffer[1024];  // Fixed size buffer
   while (fgets(buffer, sizeof(buffer), log) != NULL) {
       char *line = strdup(buffer);
       enqueue(work_Queue, line);
   }
   printf("Size is of queue is at %d\n", work_Queue->size);
   worker_thread((void *) work_Queue);
   fclose(log);
}

void enqueue(workQueue_t *queue, char *line) {
    Node *new_node = malloc(sizeof(Node));
    new_node->line = line;
    new_node->next = NULL;

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
    return line;
}


void *worker_thread(void *arg) {
    workQueue_t *work_queue = (workQueue_t *)arg;
    char *line;

    while((line = dequeue(work_queue)) != NULL) {
        log_entry_t *entry = extract_entry(line);
        printf("Parsing: %s\n", entry->pid);
        free(entry);
        free(line);
    }
    return NULL;
}
