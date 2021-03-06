/* 
 * Kenny Ngo 
 * Winter 2018
 * CS 344
 * Program 2
 * buildrooms
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>

/*global*/
const char* names[] = {"Nehalem", "Westmere", "SndyBrdg", "IvyBridg", "Haswell", 
		 			   "Broadwll", "Skylake", "KabyLake", "CoffeeLk", "IceLake"};

const char* rTypes[] = {"START_ROOM", "MID_ROOM", "END_ROOM"};

struct room
{
	int id;
	char* name;
	int roomType; /* 0 is START_ROOM, 1 is MID_ROOM, 2 is END_ROOM */
	int numOutboundConnections;
	struct room* outboundConnections;
};

void PrintRoomOutboundConnections(struct room input, char* dir)
{	
	/* setting up the file name for the room */
	char* fname;
	fname = malloc(strlen(dir)+1+7+strlen(rTypes[input.roomType]));
	const char* special[] = {"./", dir, "/", "START_ROOM", ".txt"};
	const char* normal[] = {"./", dir, "/", input.name, ".txt"};
	int i;
	for (i = 0; i < 5; i++)
		/* if roomType is START_ROOM, name file START_ROOM.txt */
		if (input.roomType == 0)
			strcat(fname, special[i]);
		else
			strcat(fname, normal[i]);

	/* opening/creating/reporting error for new file */
	FILE *f = fopen(fname, "w");
	if (f == NULL)
	{
		printf("error opening file\n");
		exit(1);
	}

	/* print room info */
	fprintf(f, "ROOM NAME: %s\n", input.name);
	for (i = 0; i < input.numOutboundConnections; i++)
		fprintf(f, "CONNECTION %d: %s\n", i + 1, input.outboundConnections[i].name);

	fprintf(f, "ROOM TYPE: %s\n", rTypes[input.roomType]);

	fclose(f);
}

/* returns true if all rooms have 3 to 6 outbound connections, false otherwise */ 
int IsGraphFull(struct room* rooms)  
{
	int i;
	for (i = 0; i < 7; i++)
		if (rooms[i].numOutboundConnections < 3)
			return 0;
	return 1;
}

/* returns a random room, does NOT validate if connection can be added */ 
struct room* GetRandomRoom(struct room* rooms)
{
	return &rooms[rand() % 7];
}

/* returns true if connection can be added to x (< 6 outbound connections), false otherwise */ 
int CanAddConnectionFrom(struct room x) 
{
	return (x.numOutboundConnections < 6);
}

/* returns true if a connection from x to y already exists, false otherwise */ 
int ConnectionAlreadyExists(struct room x, struct room y)
{
	int i;
	for (i = 0; i < x.numOutboundConnections; i++)
		if (x.outboundConnections[i].id == y.id)
			return 1;
	return 0;
}

/* connects x to y, does not check if this connection is valid */ 
void ConnectRoom(struct room* x, struct room* y) 
{
	int openDoorIndex;
	openDoorIndex = x->numOutboundConnections;
	(*x).outboundConnections[openDoorIndex] = *y;
	(*x).numOutboundConnections++;
}

/* returns true if x and y are the same room, false otherwise */ 
int IsSameRoom(struct room x, struct room y) 
{
    return (x.id == y.id);
}

/* adds a random, valid outbound connection from a room to another room */ 
void AddRandomConnection(struct room* rooms)
{
	struct room* A;
	struct room* B;

	while(1)
	{
		/* get random room */
		A = GetRandomRoom(rooms);

		/* if random room has room for another connection */
		if (CanAddConnectionFrom(*A))
			break;
	}

	do
	{
		/* get 2nd random room */
		B = GetRandomRoom(rooms);
	} while (!CanAddConnectionFrom(*B) || IsSameRoom(*A, *B) || 
			ConnectionAlreadyExists(*A, *B));

	/* when the 2nd room fills out the reqs we can connect them */
	ConnectRoom(A, B);
	ConnectRoom(B, A);
}

int main(int argc, char* argv[])
{
	/* for random numbers */
	srand(time(NULL)); 

	/* to check if name or room type is used by another room */
	int usedName[10] = {0,0,0,0,0,0,0,0,0,0};
	int usedType[2] = {0,0}; /* uT[0] is START_ROOM, uT[1] is END_ROOM */

	/* array containing the rooms so that they are easy to manipulate */
	struct room rooms[7];

	/* initializing values of each room */
	int i;
	for (i = 0; i < 7; i++)
	{
		rooms[i].id = i;
		rooms[i].name = calloc(16, sizeof(char));
		rooms[i].numOutboundConnections = 0;
		rooms[i].outboundConnections = (struct room*)malloc(6 * sizeof(struct room));
		
		int randName;
		randName = rand() % 10;

		while (usedName[randName] == 1) /* finds unused index */
			randName = rand() % 10;
		usedName[randName] = 1; /* used name at this index = true */
		strcpy(rooms[i].name, names[randName]);

		/* attempting to assign room types randomly here */
		int randType = rand() % 3;
		if (randType == 0 && usedType[0] == 0) 
		{
			rooms[i].roomType = 0;
			usedType[0] = 1; /* used room type START_ROOM */
		} else if (randType == 2 && usedType[1] == 0) 
		{
			rooms[i].roomType = 2;
			usedType[1] = 1; /* used room type END_ROOM */
		} else {
			rooms[i].roomType = 1; /* most rooms will be this */
		}
		
		/* this section is used to force START_ROOM or END_ROOM types */
		/* if not already assigned by the 2nd to last iteration */
		if (i == 5 && (usedType[0] < 1 && usedType[1] < 1)) 
		{
			randType = rand() % 2;
			if (randType == 0) 
			{
				rooms[i].roomType = 0;
				usedType[0] = 1;
			}
			else 
			{
				rooms[i].roomType = 2;
				usedType[1] = 1;
			}
		/* on the final iteration, if one type req not met, then force it */
		} else if (i == 6 && usedType[0] < 1) {
			rooms[i].roomType = 0;
		} else if (i == 6 && usedType[1] < 1) {
			rooms[i].roomType = 2;
		}
	}

	/* create all connections in graph */ 
	while (!IsGraphFull(rooms))
	    AddRandomConnection(rooms);
	
	/* creating the directory name */
	char prefix[strlen("ngoken.rooms.")];
	strcpy(prefix, "ngoken.rooms.");
	int pid;
	pid = getpid();
	char dir[strlen(prefix)+1+4];
	snprintf(dir, strlen(prefix)+1+5, "%s%d", prefix, pid);

	/* making the directory */
	int result;
	result = mkdir(dir, 0755);

	/* exit if error on making directory */
	if (result > 0)
	{
		printf("error mking dir\n");
		exit(1);
	}
	/* printing room info to files in dir */
	int j;
	for (j = 0; j < 7; j++)
		PrintRoomOutboundConnections(rooms[j], dir);
}