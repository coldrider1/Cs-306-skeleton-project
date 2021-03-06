
/* The following code is an example that implements the First Readers-Writers problem.
   Your lab assignment is to write the code in the appropriate places as indicated
   through inline C-style comments/directives.

   SUBMISSION GUIDELEINES:
    1. Save this file rw_skeleton.c as rw.c for the submission.

    2. You are to submit your assignment using the Lab #2 Dropbox on the course D2L website as two files - rw.h and rw.c

    3. Do not zip the file, nor submit any additional, unnecessary files. In particular, do not submit project files created
       by an IDE you may be using (e.g., Eclipse). There is no reason to submit a compressed or archived version, so try to
       properly follow instructions and submit the single C source file, properly named.

    3. Your code must compile, otherwise you get a zero (0). We are not going to try to read through code to assess what parts are written or not.

    4. Code that compiles but fails to run on any test cases will also probably receive a zero.

    5. Points will be deducted for each test case that the code fails to run on.

    6. You should not be getting any warnings from GCC, so make certain you check for this, as we will.

    7. Your program must error check the original call and all library calls.
   
    8. In the case of an error with a library call, print the standard system message (using either perror() or strerror()).
       You may wish to duplicate the format that most Linux/UNIX programs use, which is to prefix the message with the name of
       the program followed by a colon, give an informative message followed by a colon, and end with the system error message.
       E.g., mygrep: cannot open file: no such file).
   
    9. You are free to use any reasonable indentation style, but you must turn in reasonably indented and readable C code, or you will lose points!

    10. Since we still have to look at various parts of your code to grade it, code must be appropriately indented and use a reasonable style.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

#define SLOWNESS 30000
#define INVALID_ACCNO -99999

// sleep function
void rest() {
    //usleep(SLOWNESS * (rand() % (READ_THREADS + WRITE_THREADS)));
    usleep(100);
}


/* Include the rw.h header file for necessary initializations.
   YOUR CODE GOES HERE.
*/
#include "./rw.h"



/* Global shared data structure */
account account_list[SIZE];                /* this is the data structure that the readers and writers will be accessing concurrently.*/


/* Define your global CS access variables for the Reader-writer problem.
   YOUR CODE GOES HERE.
*/
pthread_mutex_t r_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t rw_lock = PTHREAD_MUTEX_INITIALIZER;
int read_count = 0;



/* Writer thread - will update the account_list data structure.
   Takes as argument the seed for the srand() function.   
*/
void * writer_thr(void * arg) {
    printf("Writer thread ID %ld\n", pthread_self());   
    srand(*((unsigned int *) arg));        /* set random number seed for this writer */
   
    int i, j;
    int r_idx;
    unsigned char found;            /* For every update_acc[j], set to TRUE if found in account_list, else set to FALSE */
    account update_acc[WRITE_ITR];

    /* first create a random data set of account updates */
    for (i = 0; i < WRITE_ITR;i++) {
        r_idx = rand() % SIZE;        /* a random number in the range [0, SIZE) */
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

    int temp_accno;
    /* The writer thread will now to update the shared account_list data structure */
    for (j = 0; j < WRITE_ITR;j++) {
        found = FALSE;
        /* Now update */
        for (i = 0; i < SIZE;i++) {
			           rest(); 
            if (account_list[i].accno == update_acc[j].accno) {
                /* lock and update location */
                /* You MUST FIRST TEMPORARILY INVALIDATE the accno by setting account_list[i] = INVALID_ACCNO; before making any updates to the account_list[i].balance.
                   Once the account balance is updated, you MUST put the rest() call in the appropriate place before going for update_acc[j+1].
                   This is to enable detecting race condition with reader threads violating CS entry criteria.
                    For every successful update to the account_list, you must write a log entry using the following format string:
                         fprintf(fd, "Account number = %d [%d]: old balance = %6.2f, new balance = %6.2f\n",
                                        account_list[i].accno, update_acc[j].accno, account_list[i].balance, update_acc[j].balance);
                  
                   Additionally, your code must also introduce checks/test to detect possible corruption due to race condition from CS violations.   
                */
                /* YOUR CODE FOR THE WRITER GOES IN HERE */
					found = TRUE;
                
                    pthread_mutex_lock(&rw_lock);
                    temp_accno = account_list[i].accno;
                    account_list[i].accno = INVALID_ACCNO;
                    account_list[i].balance = update_acc[j].balance;
               
               
                         /* makes the write long duration - PLACE THIS IN THE CORRECT PLACE SO AS TO INTRODUCE LATENCY IN WRITE before going for next 'j' */
                       
                    account_list[i].accno = temp_accno;
                    //account_list[i].accno = update_acc[j+1].accno;
                    fprintf(fd, "Account number = %d [%d]: old balance = %6.2f, new balance = %6.2f\n",account_list[i].accno, update_acc[j].accno, account_list[i].balance, update_acc[j].balance);
                    pthread_mutex_unlock(&rw_lock);

		}
   
        }
        if (!found)
            fprintf(fd, "Failed to find account number %d!\n", update_acc[j].accno);

    }   // end test-set for-loop

    fclose(fd);
    return NULL;
  }


/* Reader thread - will read the account_list data structure.
   Takes as argument the seed for the srand() function.
*/
void * reader_thr(void *arg) {
    printf("Reader thread ID %ld\n", pthread_self());
    srand(* ((unsigned int *) arg));   /* set random number seed for this reader */
    int i, j;
    int r_idx;
    unsigned char found;            /* For every read_acc[j], set to TRUE if found in account_list, else set to FALSE */
    account read_acc[READ_ITR];

    /* first create a random data set of account updates */
    for (i = 0; i < READ_ITR;i++) {
        r_idx = rand() % SIZE;      /* a random number in the range [0, SIZE) */
        read_acc[i].accno = account_list[r_idx].accno;
        read_acc[i].balance = 0.0;        /* we are going to read in the value */
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
       for every successful read from the account_list using the format:
            fprintf(fd, "Account number = %d [%d], balance read = %6.2f\n",
                            account_list[i].accno, read_acc[j].accno, read_acc[j].balance); 
      
       If it is unable to find a account number, then the following log entry must be
       made: fprintf(fd, "Failed to find account number %d!\n", read_acc[j].accno);

       Additionally, your code must also introduce checks/test to detect possible corruption due to race condition from CS violations.   

     */

    /* The reader thread will now to read the shared account_list data structure */
    for (j = 0; j < READ_ITR;j++) {
        /* Now read the shared data structure */
        found = FALSE;
        for (i = 0; i < SIZE;i++) {
            rest();
            if (account_list[i].accno == read_acc[j].accno) {
                /* Now lock and update */
                /* YOUR CODE FOR THE READER GOES IN HERE */
                pthread_mutex_lock(&r_lock);
                read_count++;
                if(read_count == 1){
                    pthread_mutex_lock(&rw_lock);
                }
                pthread_mutex_unlock(&r_lock);
                rest();
               
                found = TRUE;
                read_acc[j].balance = account_list[i].balance;
                fprintf(fd, "Account number = %d [%d], balance read = %6.2f\n", account_list[i].accno, read_acc[j].accno, read_acc[j].balance);
                pthread_mutex_lock(&r_lock);
                read_count--;
                if(read_count == 0){
                  pthread_mutex_unlock(&rw_lock);
                }
                pthread_mutex_unlock(&r_lock);
        }
}
        if (!found)
            fprintf(fd, "Failed to find account number %d!\n", read_acc[j].accno);
    }   // end test-set for-loop

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


void usage(char *str) {
    printf("Usage: %s -r <NUM_READERS> -w <NUM_WRITERS>\n", str);
    return;
}


int main(int argc, char *argv[]) {
    time_t t;
    unsigned int seed;
    void *result;
    int i;

    int READ_THREADS = 0;            /* number of readers to create */
    int WRITE_THREADS = 0;            /* number of writers to create */
   
   
   
   int isDigit (char *str)
		{
			while(*str)
			{
				if(!isdigit(* str++ ))
				{
					usage(argv[0]);
					exit(EXIT_FAILURE);
				}
				}
		return TRUE;
		
	}
	
    /* Generate a list of account informations. This will be used as the input to the Reader/Writer threads. */
    create_testset();

    /* USE getopt() to read as command line arguments the number of readers (READ_THREADS)
       and number of writers (WRITE_THREADS). If any of the arguments are missing, then print
       the usage message using the usage() function defined above and exit.
       For reference on getopt(), see "man getopt(3)"
    */
    /* YOUR CODE GOES HERE */
   
    while((i = getopt(argc, argv, "r:w:")) != -1){
        switch(i){
            case 'r':
				isDigit(optarg);
                READ_THREADS = atoi(optarg);
                break;
            case 'w':
				isDigit(optarg);
                WRITE_THREADS = atoi(optarg);
                break;
            default:
                perror("No correct option");
                usage(argv[0]);
                exit(EXIT_FAILURE);
            }
    }
        if(READ_THREADS < 0 || WRITE_THREADS < 0){
            perror("Choices must be equal to or greater than zero");
            usage(argv[0]);
            exit(EXIT_FAILURE);
        }       



    pthread_t* reader_idx = (pthread_t *) malloc(sizeof(pthread_t) * READ_THREADS);        /* holds thread IDs of readers */
    pthread_t* writer_idx  = (pthread_t *) malloc(sizeof(pthread_t) * WRITE_THREADS);        /* holds thread IDs of writers */
   
   
   
    /* create readers */
      for (i = 0;i < READ_THREADS;++i) {
        seed = (unsigned int) time(&t);
        /* YOUR CODE GOES HERE */
        if(pthread_create(reader_idx + i, NULL, reader_thr, (void *) (intptr_t) (&seed)) != 0){
            perror("pthread create");
            exit(EXIT_FAILURE);
        }
    }
      printf("Done creating reader threads!\n");

    /* create writers */
      for (i = 0;i < WRITE_THREADS;i++) {
        seed = (unsigned int) time(&t);
        /* YOUR CODE GOES HERE */
        if(pthread_create(writer_idx + i, NULL, writer_thr, (void *) (intptr_t) (&seed)) != 0){
            perror("pthread create");
            exit(EXIT_FAILURE);
        }
       


    }
    printf("Done creating writer threads!\n");

      /* Join all reader and writer threads.
       YOUR CODE GOES HERE.
    */
   
        i = 0;
        while(i < READ_THREADS){
            pthread_join(reader_idx[i], &result);
            i++;
        }

        i = 0;
        while(i < WRITE_THREADS){
            pthread_join(writer_idx[i], &result);
            i++;
        }


    printf("Reader threads joined.\n");
 
    printf("Writer threads joined.\n");

    /* clean-up - always a good thing to do! */
        pthread_mutex_destroy(&r_lock);
        pthread_mutex_destroy(&rw_lock);
   
    return 0;
}
