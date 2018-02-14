#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef enum { false, true } bool;

struct room
{
	int id;
	char* name;
	int roomType; //0 is START_ROOM, 1 is MID_ROOM, 2 is END_ROOM
	int numOutboundConnections;
	struct room* outboundConnections;
};

void PrintRoomOutboundConnections(struct room input)
{
  printf("The rooms connected to (%s/%d) are:\n", input.name, input.id);

  int i;
  for (i = 0; i < input.numOutboundConnections; i++)
    printf("  (%s/%d)\n", input.outboundConnections[i].name,
           input.outboundConnections[i].id);
  return;
}

// Returns true if all rooms have 3 to 6 outbound connections, false otherwise
bool IsGraphFull(struct room* rooms)  
{
    int i;
    for (i = 0; i < 7; i++)
    {
        if (rooms[i].numOutboundConnections < 3)
            return false;
    }
    return true;
}

// Returns a random room, does NOT validate if connection can be added
struct room* GetRandomRoom(struct room* rooms)
{
    return &rooms[rand() % 7];
}

// Returns true if a connection can be added from room x (< 6 outbound connections), false otherwise
bool CanAddConnectionFrom(struct room x) 
{
    return (x.numOutboundConnections < 6);
}

// Returns true if a connection from room x to room y already exists, false otherwise
bool ConnectionAlreadyExists(struct room x, struct room y)
{
    int i;
    for (i = 0; i < x.numOutboundConnections; i++)
    {
    	printf("X's connecting id is %d, Y's id is %d.\n", x.outboundConnections[i].id, y.id);
        if (x.outboundConnections[i].id == y.id)
            return true;
    }
    return false;
}

// Connects rooms x and y together, does not check if this connection is valid
void ConnectRoom(struct room* x, struct room* y) 
{
    int openDoorIndex;
    openDoorIndex = x->numOutboundConnections;
    (*x).outboundConnections[openDoorIndex] = *y;
    (*x).numOutboundConnections++;
}

// Returns true if rooms x and y are the same room, false otherwise
bool IsSameRoom(struct room x, struct room y) 
{
    return (x.id == y.id);
}

// Adds a random, valid outbound connection from a room to another room
void AddRandomConnection(struct room* rooms)
{
    struct room* A;  // Maybe a struct, maybe global arrays of ints
    struct room* B;

    while(true)
    {
        A = GetRandomRoom(rooms);

        if (CanAddConnectionFrom(*A) == true)
            break;
    }

    do
    {
        B = GetRandomRoom(rooms);
    }
    while(CanAddConnectionFrom(*B) == false || IsSameRoom(*A, *B) == true || ConnectionAlreadyExists(*A, *B) == true);

    ConnectRoom(A, B);  // TODO: Add this connection to the real variables,
    ConnectRoom(B, A);  //  because this A and B will be destroyed when this function terminates
}

int main(int argc, char* argv[])
{
	//for random numbers
	srand(time(NULL)); 

	//names of the rooms
	const char* names[] = {"ichi", "ni", "san", "yon", "go", 
			 			   "roku", "nana", "hachi", "kyu", "ju"};
	//room type strings
	const char* rTypes[] = {"START_ROOM", "MID_ROOM", "END_ROOM"};

	//to check if name or room type is used by another room
	int usedName[10] = {0,0,0,0,0,0,0,0,0,0};
	int usedType[2] = {0,0}; //uT[0] is START_ROOM, uT[1] is END_ROOM 

	struct room room1;
	struct room room2;
	struct room room3;
	struct room room4;
	struct room room5;
	struct room room6;
	struct room room7;

	//array containing the rooms so that they are easy to manipulate
	struct room rooms[7] = {room1, room2, room3, room4, room5, room6, room7};

	//initializing values of each room
	int i;
	for (i = 0; i < 7; i++)
	{
		rooms[i].id = i;
		rooms[i].name = calloc(16, sizeof(char));
		rooms[i].numOutboundConnections = 0;
		rooms[i].outboundConnections = (struct room*)malloc(6 * sizeof(struct room));
		
		int randName;
		randName = rand() % 10;

		while (usedName[randName] == 1) //finds unused index
			randName = rand() % 10;
		usedName[randName] = 1; //used name at this index = true
		strcpy(rooms[i].name, names[randName]);

		//attempting to assign room types randomly here
		int randType = rand() % 3;
		if (randType == 0 && usedType[0] == 0) {
			rooms[i].roomType = 0;
			usedType[0] = 1; //used room type START_ROOM
		} else if (randType == 2 && usedType[1] == 0) {
			rooms[i].roomType = 2;
			usedType[1] = 1; //used room type END_ROOM
		} else {
			rooms[i].roomType = 1; //most rooms will be this
		}
		
		//this section is used to force START_ROOM or END_ROOM types
		//if not already assigned by the 2nd to last iteration
		if (i == 5 && (usedType[0] < 1 && usedType[1] < 1)) {
			randType = rand() % 2;
			if (randType == 0) {
				rooms[i].roomType = 0;
				usedType[0] = 1;
			}
			else {
				rooms[i].roomType = 2;
				usedType[1] = 1;
			}
		} else if (i == 6 && usedType[0] < 1) {
			rooms[i].roomType = 0;
		} else if (i == 6 && usedType[1] < 1) {
			rooms[i].roomType = 2;
		}
	}

	//prints information for testing purposes
	int k;
	k = 0;
	// Create all connections in graph
	while (IsGraphFull(rooms) == false)
	{
		printf("NOT FULL YET, RUN # %d\n", k);
		k++;
	    AddRandomConnection(rooms);

	}
	int j;
	for (j = 0; j < 7; j++)
	{
		printf("This room's name and id are: (%s/%d)\n", rooms[j].name, rooms[j].id);
		printf("This room's type is: %s\n", rTypes[rooms[j].roomType]);

		PrintRoomOutboundConnections(rooms[j]);
	}
}