/* 
 * Kenny Ngo 
 * Winter 2018
 * CS 344
 * Program 2
 * adventure
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <pthread.h>
#include <fcntl.h>

/*global*/
pthread_mutex_t mutex;
char* startName; /* room name of the start room */
char* roomName;  /* current room name */
char* roomType;  /* type of current room */
char* folder;    /* latest working directory name */

/* grab the latest rooms directory */
void getDir()
{
	char* resultD;
	resultD = malloc(sizeof(char) * 128);
	DIR* d;
	d = opendir("."); /* checking within current directory */

	struct dirent* dp;
	struct stat* stats;
	time_t tiempo; /* will store the directory's mod time */
	int new = 0; /* the pivot for checking which directory is newer */

	stats = (struct stat*)malloc(sizeof(struct stat));
	dp = malloc(sizeof(struct dirent));

	if (d != NULL) 
	{
		while (dp = readdir(d))
		{
			if ((stat(dp->d_name, stats) == 0) && 
				(S_ISDIR(stats->st_mode) && 
				(strncmp(dp->d_name, "ngoken.rooms", 12) == 0)))
			{
				/* saving the current directory's mod time*/
				tiempo = stats->st_mtime;

				/* checking current mod time against past mod times */
				if (tiempo >= new)
				{
					/* update last highest mod time with current mod time */
					new = tiempo;

					/* set result as current directory name */
					strcpy(resultD, dp->d_name);
				}
			}
		}
		closedir(d);
	}
	/* assign the folder variable the value of the latest directory name */
	strcpy(folder, resultD);
}

/* prints the room information */
void getRooms(char rname[32])
{
	char fname[128];
	sprintf(fname, "./%s/%s", folder, rname);
	strcat(fname, ".txt");
	/* building the file name using folder name and rname */
	FILE* f;
	f = fopen(fname, "r");

	char line[128];
	fgets(line, 128, f);

	/* grabbing the room name to print */
	char* name = strtok(line, " ");
	name = strtok(NULL, " ");
	name = strtok(NULL, "\n");

	/* since the start room is not called its room name */
	/* we have to apply special rules on how to handle its names */
	if (strcmp(rname, "START_ROOM") == 0)
		strcpy(startName, name);
	if (strcmp(roomName, startName) == 1)
		strcpy(roomName, name);

	/* printing the required information */
	printf("CURRENT LOCATION: %s\n", name);
	printf("POSSIBLE CONNECTIONS:");
	int i;
	i = 0;

	/* iterate through the lines of the file to get/print connections */
	while(fgets(line, 128, f) != NULL) 
	{
		char* search = strtok(line, " ");
		if (strcmp(search, "CONNECTION") == 0)
		{
			search = strtok(NULL, ":");
			search = strtok(NULL, "\n");
			/* using i to print commas after the first connection */
			if (i > 0)
				printf(",");
			printf("%s", search);

		/* also using this opportunity to grab the room type */
		} else if (strcmp(search, "ROOM") == 0)
		{
			search = strtok(NULL, ":");
			search = strtok(NULL, " \n");
			strcpy(roomType, search);
		}

		i++;
	}
	printf(".\n"); /* finishing the output line with '.' and '\n'*/
}

/* reiterates through the file to see if input is a valid connection */
int validate(char rname[32], char input[32])
{
	char fname[128];
	sprintf(fname, "./%s/%s", folder, rname);
	strcat(fname, ".txt");
	/* building the file name using folder name and rname */
	FILE* f;
	f = fopen(fname, "r");
	if (f == NULL)
		return 0;

	char line[128];
	fgets(line, 128, f);

	while(fgets(line, 128, f) != NULL) 
	{
		char* search = strtok(line, " ");

		/* same code as getRooms but this one returns 1 if input found */
		if (strcmp(search, "CONNECTION") == 0)
		{
			search = strtok(NULL, " ");
			search = strtok(NULL, " \n");
			if (strcmp(search, input) == 0){
				return 1;
			}
		}
	}
	return 0;
}

/* write current time to file */
void* getTime()
{
	FILE* f;
	f = fopen("currentTime.txt", "w+");

	/* strOut to hold the string to output */
	char* strOut;
	strOut = malloc(sizeof(char) * 128);

	/* time properties */
	struct tm* tiempo;
	time_t now = time(0);
	tiempo = localtime(&now);
	
	/* saving and writing current time to file using */
	/* the format H:Mam/pm, Day, Month DD YYYY */
	strftime(strOut, 128, "%I:%M%P, %A, %B %d %G\n", tiempo);
	fputs(strOut, f);
	fclose(f);
}

/* prints the contents of the currentTime.txt file */
void writeTime()
{
	FILE* f;
	f = fopen("currentTime.txt", "r");

	/* holds the string being read in from file */
	char* strIn;
	strIn = malloc(sizeof(char) * 128);
	
	fgets(strIn, 128, f);
	printf("%s\n", strIn);
	fclose(f);
}

/* opens new thread and writes the current time to a file using getTime */
void thread()
{
	pthread_t thred; /* initialize the thread */
	pthread_mutex_init(&mutex, NULL);

	pthread_mutex_lock(&mutex); /* lock thread */
	
	/* run getTime to write the time to the file */
	int tid = pthread_create(&thred, NULL, getTime, NULL);
	
	pthread_mutex_unlock(&mutex); /* unlock and remove thread */
	pthread_mutex_destroy(&mutex);
}

/* game playing function, stops when at end room */
void playGame(char current[64])
{
	/* initializing values for endgame records */
	int turns;
	turns = 0;
	char* path[128];

	int success; /* flag for game over */
	success = 0;

	char input[128]; /* stores user input */

	strcpy(roomName, current);
	getRooms(roomName); /* print the starting room info */

	while (success != 1) 
	{
		printf("WHERE TO? >");
		scanf("%s", input);
		printf("\n");

		/* user asks for the time */
		if (strcmp(input, "time") == 0)
		{
			thread();
			writeTime();

		/* validates input */	
		} else if (!validate(roomName, input))
		{
			printf("HUH? I DON’T UNDERSTAND THAT ROOM. TRY AGAIN.\n\n");
			getRooms(roomName); /* reprint current room info */
		
		/* input is valid, continue to input room */
		} else
		{
			path[turns] = malloc(sizeof(char) * 8);;
			strcpy(path[turns], roomName); /* add to visited rooms */
			turns++; /* increment turns taken */
			strcpy(roomName, input); /* set current room to input */

			/* in case the user asked for the start room again */
			if (strcmp(roomName, startName) == 0)
				strcpy(roomName, "START_ROOM"); 

			/* print next rooms info */
			getRooms(roomName);
		}
		
		/* check room type, end game if true */
		if (strcmp(roomType, "END_ROOM") == 0)
			success = 1;
	}
	/* printing endgame info */
	printf("\nYOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
	printf("YOU TOOK %d STEPS. ", turns);
	printf("YOUR PATH TO VICTORY WAS:\n");
	
	int j;
	/* start at j = 1 because path[0] is start room */
	for (j = 1; j < turns; j++)
	{
		/* check if starting room was revisited, print its name if so */
		if (strcmp(path[j], "START_ROOM") == 0)
			printf("%s\n", startName);
		else
			printf("%s\n", path[j]);
	}
	/* append end room name to path */
	printf("%s\n", roomName);
}

/* main function that initializes and starts the game */
int main(int argc, char* argv[])
{
	/* initializing all global variables */
	folder = malloc(sizeof(char) * 32);
	roomName = malloc(sizeof(char) * 8);
	roomType = malloc(sizeof(char) * 16);
	startName = malloc(sizeof(char) * 16);

	/* because the starting room will be named START_ROOM */
	/* finding the starting room is not needed and can start immediately */
	char start[10];
	strcpy(start, "START_ROOM");

	/* set the current directory to the latest rooms folder */
	getDir();

	/* begin the game */
	playGame(start);

	/* for '$ echo $?' */
	return 0;
}