#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

/* Include the rw.h header file for necessary initializations. 
   YOUR CODE GOES HERE. 
*/




/* Global shared data structure */
account account_list[SIZE];


/* number of reader and writer threads */
#define READ_THREADS  7
#define WRITE_THREADS 3

/* Define your global CS access variables for the Reader-writer problem.
   YOUR CODE GOES HERE.
*/


/* Writer thread - will update the account_list data structure. 
   Takes as argument the seed for the srand() function.	
*/
void * writer_thr(void * arg) {
	printf("Writer thread ID %ld\n", pthread_self());	
	srand(*((unsigned int *) arg));		/* set random number seed for this writer */
	
	int i, j;
	int r_idx;
	unsigned char update_flag = TRUE;
	account update_acc[WRITE_ITR];

	/* first create a random data set of account updates */
	for (i = 0; i < WRITE_ITR;i++) {
		r_idx = rand() % SIZE;		/* a random number in the range [0, SIZE) */ 
		update_acc[i].accno = account_list[r_idx].accno;
		update_acc[i].balance = 1000.0 + (float) (rand() % MAX_BALANCE);
	}
	/* open a writer thread log file */
	char thr_fname[64];
	snprintf(thr_fname, 64, "writer_%ld_thr.log", pthread_self());
	FILE* fd = fopen(thr_fname, "w");
	if (!fd) {
		fprintf(stderr,"Failed to open writer log file %s\n", thr_fname);
		pthread_exit(&errno);
	}

	/* The writer thread will now to update the shared account_list data structure. 
	   For each entry 'j' in the update_acc[] array, it will find the corresponding 
       account number in the account_list array and update the balance of that account
	   number with the value stored in update_acc[j]. 	
	*/

	/* YOUR CODE FOR THE WRITER GOES IN HERE */
	
	
	
	fclose(fd);
	return NULL;
}

/* Reader thread - will read the account_list data structure. 
   Takes as argument the seed for the srand() function. 
*/
void * reader_thr(void *arg) {
	printf("Reader thread ID %ld\n", pthread_self());
    srand(*((unsigned int *) arg));   /* set random number seed for this reader */

    int i, j;
	int r_idx;
	unsigned char update_flag = TRUE;
    account read_acc[READ_ITR];

    /* first create a random data set of account updates */
    for (i = 0; i < READ_ITR;i++) {
        r_idx = rand() % SIZE;      /* a random number in the range [0, SIZE) */
        read_acc[i].accno = account_list[r_idx].accno;
        read_acc[i].balance = 0.0;		/* we are going to read in the value */
    }

    /* open a reader thread log file */
	char thr_fname[64];
	snprintf(thr_fname, 64, "reader_%ld_thr.log", pthread_self());
	FILE *fd = fopen(thr_fname, "w");
	if (!fd) {
		fprintf(stderr,"Failed to reader log file %s\n", thr_fname);
		pthread_exit(&errno);
	}
 
    /* The reader thread will now to read the shared account_list data structure.
	   For each entry 'j' in the read_acc[] array, the reader will fetch the 
       corresponding balance from the account_list[] array and store in
       read_acc[j].balance. The reader will then make a log entry in the log file
       using the format: fprintf(fd, "Account number = %d, balance read = %6.2f\n", read_acc[j].accno, read_acc[j].balance);
	 
	   If it is unable to find a account number, then the following log entry must be
	   made: fprintf(fd, "Failed to find account number %d!\n", read_acc[j].accno);
 	*/

	/* YOUR CODE FOR THE READER GOES IN HERE */


    fclose(fd);
    return NULL;
}

/* populate the shared account_list data structure */
void create_testset() {
	time_t t;
	srand(time(&t));
	int i;	
	for (i = 0;i < SIZE;i++) {
		account_list[i].accno = 1000 + rand() % RAND_MAX;
		account_list[i].balance = 100 + rand() % MAX_BALANCE;
	}	
	return;
}


int main(int argc, char *argv[]) {
	time_t t;
	unsigned int seed;
	int i;
	
	/* Generate a list of account informations. This will be used as the input to the Reader/Writer threads. */
	create_testset();

	pthread_t reader_idx[READ_THREADS];		/* holds thread IDs of readers */
	pthread_t writer_idx[WRITE_THREADS];	/* holds thread IDs of writers */
	
	/* create readers */
  	for (i = 0;i < READ_THREADS;i++) {
		seed = (unsigned int) time(&t);
		/* YOUR CODE GOES HERE */
	
	}
  	printf("Done creating reader threads!\n");

	/* create writers */ 
  	for (i = 0;i < WRITE_THREADS;i++) {
		seed = (unsigned int) time(&t);
		/* YOUR CODE GOES HERE */
  	
	}
	printf("Done creating writer threads!\n");

  	/* Join all reader and writer threads.
       YOUR CODE GOES HERE. 
    */

	printf("Reader threads joined.\n");
 
	printf("Writer threads joined.\n");

	/* clean-up - always a good thing to do! */
 	pthread_mutex_destroy(&r_lock);
	pthread_mutex_destroy(&rw_lock);
	
	return 0;
}
	

