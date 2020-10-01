/*
	2017510033 AYŞE ÖZGE ERİŞ
	OPERATING SYSTEMS ASSIGNMENT 2
*/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

  #define KEEPER_COUNT 10
  #define STUDENT_COUNT 100

void * student(void * num);
void * room(void * num);
void waitForAWhile(); //random delay for students
int findMaxRoom(); //which room the student will study in
void PrintRoomCapacity(); //print every room's capacity R1[#Student] R2[#Student] R3[#Student]... R10[Student]
int HowManyStudentsAreThere(); //how many students are there in the room

sem_t order; //unneccesary order for 100 student
sem_t roomCapacity_sem[KEEPER_COUNT]; //each room has 4 seats
sem_t AvailableRoom[KEEPER_COUNT]; //room availability
sem_t studying_sem[KEEPER_COUNT]; //students are studying or not
int allDone = 0; // Flag to stop the room thread when all students have been finished.


int main(int argc, char * argv[]) {
  pthread_t roomID[KEEPER_COUNT], studentID[STUDENT_COUNT]; //Thread declaration

  int i , Student[STUDENT_COUNT],rooms[KEEPER_COUNT];

  printf("*DEU LIBRARY*\n");
  for (i = 0; i < STUDENT_COUNT; i++) {
    Student[i] = i; //Number
  }
  for (i = 0; i < KEEPER_COUNT; i++) {
    rooms[i] = i;
  }
  // Initialize the semaphores with initial values...
  sem_init( & order, 0, STUDENT_COUNT+1);
  for (i = 0; i < KEEPER_COUNT; i++) {
    sem_init( & AvailableRoom[i], 0, 0);
    sem_init( & roomCapacity_sem[i], 0, 4);
    sem_init( & studying_sem[i], 0, 0);
  }

  for (i = 0; i < KEEPER_COUNT; i++) { // Creating 10 rooms.
    pthread_create( & roomID[i], NULL, room, (void * ) & rooms[i]);
  }
  sleep(1); // wait until all rooms are created

  for (i = 0; i < STUDENT_COUNT; i++) { // Creating 100 Students.
    pthread_create( & studentID[i], NULL, student, (void * ) & Student[i]);
    waitForAWhile(); //students coming in random time
  }

  for (i = 0; i < STUDENT_COUNT; i++) { //Wait until all students are dealt with
    pthread_join(studentID[i], NULL);
  }

  // When all of the students are done studying, kill the room thread.
  allDone = 1;
  printf("END\n");
  exit(EXIT_SUCCESS);

  return 0;
}

void *student(void * number) {
    waitForAWhile();
    int snum = * (int * ) number;
    int rnum = findMaxRoom(); //Find the room with maximum capacity
    printf("\n Student%d -> room%d\n", snum, rnum);
    sem_wait( & roomCapacity_sem[rnum]); //Enter the room with highest capacity, decrease the available seat number
    sem_post( & AvailableRoom[rnum]); //Make the students study when the 4 seats are full
    PrintRoomCapacity();
    if(HowManyStudentsAreThere(rnum)==4)
      sem_post( & studying_sem[rnum]); // Wait for the students to finish studying.
  }

void *room(void * number) {
    waitForAWhile();
    int rnum = * (int * ) number;
    while (!allDone) {
      printf ("The room keeper %d is cleaning \n", rnum);
      sem_wait( & AvailableRoom[rnum]); 
      if (!allDone) {
        sem_wait(&studying_sem[rnum]);
        printf("\n The room %d is being used by students\n", rnum);
        waitForAWhile(); // Take a random amount of time to let students study
        waitForAWhile();
        sem_post(&roomCapacity_sem[rnum]); //students are leaving the room
        sem_post(&roomCapacity_sem[rnum]); 
        sem_post(&roomCapacity_sem[rnum]); 
        sem_post(&roomCapacity_sem[rnum]); 
      } else {
        printf("No more students.\n");
      }
    }

  }

void
waitForAWhile() {
  int x = rand() % (50000) + 25000;
  srand(time(NULL));
  usleep(x); //usleep halts execution in specified miliseconds
}

int findMaxRoom() { //finds the room with max capacity to let the arriving student to enter
  int i;
  int maxStudent = 0;
  int maxRoom = rand() % KEEPER_COUNT;
  int val;
  for (i = 0; i < KEEPER_COUNT; i++) {
    val =HowManyStudentsAreThere(i);
    if (val == 4) {
      continue;
    }
    if (val > maxStudent && val != 4) {
      maxStudent = val;
      maxRoom = i;
    }
  }
  return maxRoom;
}

int HowManyStudentsAreThere(int roomNo){
  int sval;
  sem_getvalue( & roomCapacity_sem[roomNo], & sval);//how many seats are available
  sval = 4 - sval; //how many students are there in the room
  return sval;
}

void PrintRoomCapacity() {
  int i;
  int val;
  for (i = 0; i < KEEPER_COUNT; i++) {
    val=HowManyStudentsAreThere(i);
    printf("R%d[#%d] ", i, val); //T1[#0] T2[#0] T3[#0]... T10[#0]
  }
}
