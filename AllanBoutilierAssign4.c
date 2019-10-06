//Allan Boutilier 3563756
//gcc -pthread -g -o Assign4 AllanBoutilierAssign4.c -std=c99
//  ./Assign4 -c 5 -n 4 -s 100 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
//reading in numbers 
int numWaitChairs, randIn, secondsIn;
int currentTime = 0;
int countInWaitingRoom = 0;
int customerID = 0;
int countWithBarber = 0;



//These were given to us in the assignment PDF to hardcode
int haircutTimeUpper = 7;
int haircutTimeLower = 5;

//functions for threads
void * keepTime();
void * barberWork();
void * waitWork();

//initializations for threading
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

//Piping 
int barberPipe1[2];
int barberPipe2[2];
int barberPipe3[2];


//testing
int waitingRoom[100];
void getLocation();
int main(int argc, char* argv[]) {

	//Setup and read in command line args
	if(argc==6) { 
		printf("\n\n\n******NUMBER OF CPUS WAS NOT GIVEN!!!********\n\n\nCorrect Usage: \nAllanBoutilierAssign2 X < sample_input_a2.txt \nWhere X is number of CPUs\n\n\n");
	}
	numWaitChairs = atoi(argv[2]);
	randIn = atoi(argv[4]);
	secondsIn = atoi(argv[6]);
	
	//generate random seed
	srand(time(NULL));
	//create the thread that keeps time
	pthread_t timeThread;
	pthread_create(&timeThread, NULL, &keepTime, NULL);
	
	//Three pipes to the barbers
	pipe(barberPipe1);
	pipe(barberPipe2);
	pipe(barberPipe3);
	
	//threading for barber
	pthread_t barberThread1, barberThread2, barberThread3;
	pthread_create(&barberThread1, NULL, &barberWork, NULL);
	pthread_create(&barberThread2, NULL, &barberWork, NULL);
	pthread_create(&barberThread3, NULL, &barberWork, NULL);

	//threading for the waiting room
	pthread_t waitThread;
	pthread_create(&waitThread, NULL, &waitWork, NULL);


	

	int count = 0;
	while(currentTime < secondsIn) {
		int num = rand() % randIn + 1; 
    	sleep(num);

    	//pthread_mutex_lock(&mutex);
		
		
		if(countWithBarber < 3) {
			if(countWithBarber == 0) {
				write(barberPipe1[1], &customerID, sizeof(customerID));
				customerID++;
			}
			else if(countWithBarber == 1) {
				write(barberPipe1[1], &customerID, sizeof(customerID));
				customerID++;
			}
			else{
				write(barberPipe1[1], &customerID, sizeof(customerID));
				customerID++;
			}
			
		}
		else if(countInWaitingRoom < numWaitChairs) {
			pthread_mutex_lock(&mutex);
			countInWaitingRoom++;
			pthread_mutex_unlock(&mutex);
			printf("Customer %d is waiting for a haircut.\n", customerID);
			for (int i = 0; i < countInWaitingRoom; ++i)
			{
				if(waitingRoom[i] == 0) {
					waitingRoom[i] = customerID;
				}
				
			}
			customerID++;
		}
		else {
			printf("Customer %d leaves with a long shaggy mop of hair!\n", customerID);
			customerID++;
		}
		
		//pthread_mutex_unlock(&mutex);



	}
	
	pthread_join(timeThread, NULL); 
	pthread_join(waitThread, NULL); 
	pthread_join(barberThread1, NULL); 
	pthread_join(barberThread2, NULL); 
	pthread_join(barberThread3, NULL); 
}



void * keepTime() {
	while(currentTime<=secondsIn) {
		//lock mutex
		pthread_mutex_lock(&mutex);
		currentTime++;
		//unlock mutex
		pthread_mutex_unlock(&mutex);
		sleep(1);
	}	
}

void * barberWork() {
	int num;
	
	while(currentTime <=secondsIn) {	
		if(read(barberPipe1[0], &num, sizeof(num)) != -1) {
			pthread_mutex_lock(&mutex);
			countWithBarber++;
			pthread_mutex_unlock(&mutex);
			sleep(5);
			printf("Customer %d is styling a fresh new haircut!\n", num );
			pthread_mutex_lock(&mutex);
			countWithBarber--;
			//unlock mutex
			pthread_mutex_unlock(&mutex);
		}
	} 	
}

void * waitWork() {
	int temp;
	while(currentTime <=secondsIn) {
		for(int i=0; i<numWaitChairs; i++) {
			if(waitingRoom[i] > 0 && countWithBarber < 3) {
				pthread_mutex_lock(&mutex);
				write(barberPipe1[1], &waitingRoom[i], sizeof(waitingRoom[i]));
				countInWaitingRoom--;
				waitingRoom[i] = 0;
				pthread_mutex_unlock(&mutex);

			}
		}
	}
}

