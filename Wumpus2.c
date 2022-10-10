
#include <ctype.h>
#include <stdbool.h> // For boolean function
#include <stdio.h>
#include <stdlib.h>

//-------------------------------------------------------------------------------
// global constants
#define MAX_LINE_LENGTH 81
#define NUMBER_OF_ROOMS 20

// Used to more conveniently pass all game information between functions.
struct GameInfo {
  int movingcount;
  int playerRoom;
  int wumpusRoom;
  int pit1;
  int pit2;
  int bat1;
  int bat2;
  int arrow;
};

// Function prototype needed to allow calls in any order between
//   functions checkForHazards() and inRoomWithBats()
void checkingHazards(struct GameInfo *theGameInfo, bool *personIsAlive,
                     int **Rooms);

//--------------------------------------------------------------------------------
void displayCave() {
  printf("\n"
         "       ______18______             \n"
         "      /      |       \\           \n"
         "     /      _9__      \\          \n"
         "    /      /    \\      \\        \n"
         "   /      /      \\      \\       \n"
         "  17     8        10     19       \n"
         "  | \\   / \\      /  \\   / |    \n"
         "  |  \\ /   \\    /    \\ /  |    \n"
         "  |   7     1---2     11  |       \n"
         "  |   |    /     \\    |   |      \n"
         "  |   6----5     3---12   |       \n"
         "  |   |     \\   /     |   |      \n"
         "  |   \\       4      /    |      \n"
         "  |    \\      |     /     |      \n"
         "  \\     15---14---13     /       \n"
         "   \\   /            \\   /       \n"
         "    \\ /              \\ /        \n"
         "    16---------------20           \n"
         "\n");
}

//--------------------------------------------------------------------------------
void displayInstructions() {
  printf("Hunt the Wumpus:                                             \n"
         "The Wumpus lives in a completely dark cave of 20 rooms. Each \n"
         "room has 3 tunnels leading to other rooms.                   \n"
         "                                                             \n"
         "Hazards:                                                     \n"
         "1. Two rooms have bottomless pits in them.  If you go there you fall "
         "and die.   \n"
         "2. Two other rooms have super-bats.  If you go there, the bats grab "
         "you and     \n"
         "   fly you to some random room, which could be troublesome.  Then "
         "those bats go \n"
         "   to a new room different from where they came from and from the "
         "other bats.   \n"
         "3. The Wumpus is not bothered by the pits or bats, as he has sucker "
         "feet and    \n"
         "   is too heavy for bats to lift.  Usually he is asleep.  Two things "
         "wake       \n"
         "    him up: Anytime you shoot an arrow, or you entering his room.  "
         "The Wumpus   \n"
         "    will move into the lowest-numbered adjacent room anytime you "
         "shoot an arrow.\n"
         "    When you move into the Wumpus' room, then he wakes and moves if "
         "he is in an \n"
         "    odd-numbered room, but stays still otherwise.  After that, if he "
         "is in your \n"
         "    room, he snaps your neck and you die!                            "
         "           \n"
         "                                                                     "
         "           \n"
         "Moves:                                                               "
         "           \n"
         "On each move you can do the following, where input can be upper or "
         "lower-case:  \n"
         "1. Move into an adjacent room.  To move enter 'M' followed by a "
         "space and       \n"
         "   then a room number.                                               "
         "           \n"
         "2. Shoot your guided arrow through a list of up to three adjacent "
         "rooms, which  \n"
         "   you specify.  Your arrow ends up in the final room.               "
         "           \n"
         "   To shoot enter 'S' followed by the number of rooms (1..3), and "
         "then the      \n"
         "   list of the desired number (up to 3) of adjacent room numbers, "
         "separated     \n"
         "   by spaces. If an arrow can't go a direction because there is no "
         "connecting   \n"
         "   tunnel, it ricochets and moves to the lowest-numbered adjacent "
         "room and      \n"
         "   continues doing this until it has traveled the designated number "
         "of rooms.   \n"
         "   If the arrow hits the Wumpus, you win! If the arrow hits you, you "
         "lose. You  \n"
         "   automatically pick up the arrow if you go through a room with the "
         "arrow in   \n"
         "   it.                                                               "
         "           \n"
         "3. Enter 'R' to reset the person and hazard locations, useful for "
         "testing.      \n"
         "4. Enter 'C' to cheat and display current board positions.           "
         "           \n"
         "5. Enter 'D' to display this set of instructions.                    "
         "           \n"
         "6. Enter 'P' to print the maze room layout.                          "
         "           \n"
         "7. Enter 'X' to exit the game.                                       "
         "           \n"
         "                                                                     "
         "           \n"
         "Good luck!                                                           "
         "           \n"
         " \n\n");
} // end displayInstructions()

//--------------------------------------------------------------------------------

int alreadyFound(int randomValue, int randomNumbers[], int limit) {
  int valueReturn = false;
  for (int i = 0; i < limit; i++) {
    if (randomValue == randomNumbers[i]) {
      valueReturn = true;
      break;
    }
  }

  return valueReturn;
}

//--------------------------------------------------------------------------------

void getUniqueValues(int randomNumbers[], int size) {
  int randomValue = -1;

  for (int i = 0; i < size; i++) {
    do {
      randomValue = rand() % NUMBER_OF_ROOMS + 1; // random number 1..20
    } while (alreadyFound(randomValue, randomNumbers, i));
    randomNumbers[i] = randomValue;
  }
}

//--------------------------------------------------------------------------------

void initializeGame(struct GameInfo *gameInfo) // All game settings variables
{
  // Array of 6 unique values 1..20, to be used in initializing cave hazards
  // locations
  int randomNumbers[7];

  // Initialize cave room connections
  //       ______18______
  //      /      |       \
    //     /      _9__      \
    //    /      /    \      \
    //   /      /      \      \
    //  17     8        10    19
  // |  \   / \      /  \   / |
  // |   \ /   \    /    \ /  |
  // |    7     1---2     11  |
  // |    |    /     \    |   |
  // |    6----5     3---12   |
  // |    |     \   /     |   |
  // |    \       4      /    |
  // |     \      |     /     |
  //  \     15---14---13     /
  //   \   /            \   /
  //    \ /              \ /
  //    16---------------20

  // Select some unique random values 1..20 to be used for 2 bats rooms, 2
  // pits rooms, Wumpus room, and initial player room
  getUniqueValues(randomNumbers, 7);
  // Use the unique random numbers to set initial locations of hazards, which
  //    should be non-overlapping.
  gameInfo->pit1 = randomNumbers[2];
  gameInfo->pit2 = randomNumbers[3];
  gameInfo->wumpusRoom = randomNumbers[1];
  gameInfo->playerRoom = randomNumbers[0];
  gameInfo->bat1 = randomNumbers[4];
  gameInfo->bat2 = randomNumbers[5];
  gameInfo->arrow = randomNumbers[6];

  gameInfo->movingcount = 1;
} // end initializeBoard(...)

//--------------------------------------------------------------------------------
// Returns true if nextRoom is adjacent to current room, else returns false.
int adjacentRoom(int tunnels[3], // Array of adjacent tunnels
                 int nextRoom)   // Desired room to move to
{
  return (tunnels[0] == nextRoom || tunnels[1] == nextRoom ||
          tunnels[2] == nextRoom);
}
int lowestAdjacent(int room1, int room2, int room3) {
  if (room1 < room2 && room1 < room3) {
    return room1;
  } else if (room2 < room1 && room2 < room3) {
    return room2;
  } else {
    return room3;
  }
}

//--------------------------------------------------------------------------------
// Display where everything is on the board.
void cheatingInfo(struct GameInfo gameInfo) {
  printf("Cheating! Game elements are in the following rooms: \n"
         "Player Wumpus Pit1 Pit2 Bats1 Bats2 Arrow  \n"
         "  %d     %d     %d     %d     %d     %d    %d \n\n",
         gameInfo.playerRoom, gameInfo.wumpusRoom, gameInfo.pit1, gameInfo.pit2,
         gameInfo.bat1, gameInfo.bat2, gameInfo.arrow);

} // end displayCheatInfo(...)

//--------------------------------------------------------------------------------

// Display room number and hazards detected
void rommInfodisplay(struct GameInfo gameInfo, int **Rooms) {
  // Retrieve player's current room number and display it
  int currentRoom = gameInfo.playerRoom;

  printf("You are in room %d. ", currentRoom);

  // Retrieve index values of all 3 adjacent rooms
  int room1 = Rooms[currentRoom][0];
  int room2 = Rooms[currentRoom][1];
  int room3 = Rooms[currentRoom][2];

  // Display hazard detection message if Wumpus is in an adjacent room
  int wumpusRoom = gameInfo.wumpusRoom;
  if (room1 == wumpusRoom || room2 == wumpusRoom || room3 == wumpusRoom) {
    printf("You smell a stench. ");
  }

  // Display hazard detection message if a pit is in an adjacent room
  int pit1Room = gameInfo.pit1;
  int pit2Room = gameInfo.pit2;
  if (room1 == pit1Room || room1 == pit2Room || room2 == pit1Room ||
      room2 == pit2Room || room3 == pit1Room || room3 == pit2Room) {
    printf("You feel a draft. ");
  }

  int bat1Room = gameInfo.bat1;
  int bat2Room = gameInfo.bat2;
  if (room1 == bat1Room || room1 == bat2Room || room2 == bat1Room ||
      room2 == bat2Room || room3 == bat1Room || room3 == bat2Room) {
    printf("You hear rustling of bat wings. ");
  }
  printf("\n\n");
} // end rommInfodisplay(...)

//--------------------------------------------------------------------------------

// function to handle shooting of arrow
void arrowShooting(struct GameInfo *gameInfo, int **Rooms, bool *personIsAlive,
                   bool *wumpusIsAlive) {

  // Prompt user to enter input
  if (gameInfo->arrow != -1) {
    // Player picks up arrow
    printf("Sorry, you can't shoot an arrow you don't have.  Go find it.\n");
    return;
  } else {
    printf("Enter the number of rooms (1..3) into which you want to shoot, "
           "followed by the rooms themselves: ");
    int numberOfRooms;
    int j;
    int lowestRoom;
    scanf("%d", &numberOfRooms);
    if (numberOfRooms <= 3) {
      gameInfo->arrow = gameInfo->playerRoom;
      int i = 0;
      while (i < numberOfRooms) {
        scanf("%d", &j);
        if (adjacentRoom(Rooms[gameInfo->arrow], j)) {
          gameInfo->arrow = j;
          if (gameInfo->wumpusRoom == gameInfo->arrow) {
            printf(
                "Wumpus has just been pierced by your deadly arrow! \n"
                "Congratulations on your victory, you awesome hunter you.\n");
            *wumpusIsAlive = false;
            return;
          }
          if (gameInfo->playerRoom == gameInfo->arrow) {
            printf("You just shot yourself.  \n"
                   "Maybe Darwin was right.  You're dead.\n");
            *personIsAlive = false;
            return;
          }
        } else {
          printf("Room %d is not adjacent.  Arrow ricochets...\n", j);
          gameInfo->arrow = Rooms[gameInfo->arrow][0];
          int room1 = Rooms[gameInfo->arrow][0];
          int room2 = Rooms[gameInfo->arrow][1];
          int room3 = Rooms[gameInfo->arrow][2];
          gameInfo->arrow = lowestAdjacent(room1, room2, room3);
          if (gameInfo->wumpusRoom == gameInfo->arrow) {
            printf("Your arrow ricochet killed the Wumpus, you lucky dog! \n"
                   "Accidental victory, but still you win!\n");
            *wumpusIsAlive = false;
            return;
          }
          if (gameInfo->playerRoom == gameInfo->arrow) {
            printf("You just shot yourself, and are dying.\n"
                   "It didn't take long, you are dead.\n");
            *personIsAlive = false;
            return;
          }
        }
        i++;
      }
      gameInfo->wumpusRoom = Rooms[gameInfo->wumpusRoom][0];
      gameInfo->arrow = Rooms[gameInfo->playerRoom][0];
    } else {
      printf("Sorry, the max number of rooms is 3. Setting that value to 3.");
      numberOfRooms = 3;
    }
  }
} // end arrowShooting(...)

//--------------------------------------------------------------------------------
// If the player just moved into a room with a pit, the person dies.
// If the person just moved into the Wumpus room, then if the room number is odd
// the Wumpus moves to a random adjacent room.
void checkingHazards(
    struct GameInfo *gameInfo, // Hazards location and game info
    bool *personIsAlive, int **Rooms)
// Person is alive, but could die depending on the hazards
{
  // retrieve the room the person is in
  int playerRoom = gameInfo->playerRoom;

  // Check whether there is a pit
  if (playerRoom == gameInfo->pit1 || playerRoom == gameInfo->pit2) {
    // Person falls into pit
    printf("Aaaaaaaaahhhhhh....   \n");
    printf("    You fall into a pit and die. \n");
    *personIsAlive = false;
    return;
  }

  // checking for bats
  if (playerRoom == gameInfo->bat1 || playerRoom == gameInfo->bat2) {
    // Person falls into pit
    int oldPlayerLocation = gameInfo->playerRoom;
    int newPlayerLocation = rand() % NUMBER_OF_ROOMS + 1;
    printf("Woah... you're flying! \n");
    printf("You've just been transported by bats to room %d.\n",
           newPlayerLocation); // always prints they are going to room 2

    int randomNewRoom;
    do {
      randomNewRoom = rand() % NUMBER_OF_ROOMS + 1;
    } while (randomNewRoom == newPlayerLocation ||
             randomNewRoom == gameInfo->bat1 ||
             randomNewRoom == gameInfo->bat2);

    if (oldPlayerLocation == gameInfo->bat1) {
      gameInfo->bat1 = randomNewRoom;
    } else {
      gameInfo->bat2 = randomNewRoom;
    }
    gameInfo->playerRoom = newPlayerLocation;
    return;
  }

  // Check for the Wumpus
  if (playerRoom == gameInfo->wumpusRoom) {
    // To make it easier to test, in this version of the program the Wumpus
    // always moves if it is currently in an odd-numbered room, and it moves
    // into the lowest-numbered adjacent room.  In the version that is more fun
    // to play (but harder to test), the Wumpus has a 75% chance of moving, and
    // a 25% chance of staying and killing you.  The "more fun" code is
    // commented out below.

    // Wumpus is there. 75% change of Wumpus moving, 25% chance of it killing
    // you Generate a random number 1..100 if(  (rand() % 100) < 75) {
    if (gameInfo->wumpusRoom % 2 == 1) {
      // You got lucky and the Wumpus moves away
      printf("You hear a slithering sound, as the Wumpus slips away. \n"
             "Whew, that was close! \n");

      // int lowestRoom = lowestAdjacent(room1, room2, room3);
      // Choose a random adjacent room for the Wumpus to go into
      // gameInfo.wumpusRoom = rooms[ playerRoom][ rand() % 3];
      gameInfo->wumpusRoom =
          Rooms[playerRoom][0]; // Choose the lowest-numbered adjacent room
    } else {
      // Wumpus kills you
      printf(
          "You briefly feel a slimy tentacled arm as your neck is snapped. \n"
          "It is over.\n");
      *personIsAlive = false;
      return;
    }
  }
  if (gameInfo->playerRoom == gameInfo->arrow) {
    printf("Congratulations, you found the arrow and can once again shoot.\n");
    gameInfo->arrow = -1;
    return;
  }
} // end checkForHazards(...)

//--------------------------------------------------------------------------------
// Prompt for and reset the positions of the game hazards and the person's
// location, useful for testing.  No error checking is done on these values.
void positionsResetting(struct GameInfo *theGameInfo) {
  printf("Enter the room locations (1..20) for player, wumpus, pit1, pit2, "
         "bats1, bats2, and arrow: \n");
  // In the scanf below note that we put the space at the beginning of the scanf
  // so that any newline left over from a previous input is not read in and used
  // as
  // the next move. Another option is having getchar() after the scanf()
  // statement.
  scanf(" %d %d %d %d %d %d %d", &theGameInfo->playerRoom,
        &theGameInfo->wumpusRoom, &theGameInfo->pit1, &theGameInfo->pit2,
        &theGameInfo->bat1, &theGameInfo->bat2, &theGameInfo->arrow);
  printf("\n");
}

//-------------------------------------------------------------------------------
int main(void) {

  struct GameInfo
      gameInfo; // Used to more easily pass game info variables around
  bool personStatus = true; // Used in checking for end of game
  bool wumpusStatus = true; // Used in checking for end of game // Used in
                            // checking arrow status of wumpus
  char typeOfMove;          // Used to handle user input letter
  int nextRoom; // User input of destination room number, used on a 'M' type
                // move

  // Seed the random number generator.  Change seed to time(0) to change output
  // each time.
  // srand(time(0));
  srand(1);

  // dynamically allocationg memory
  int **Rooms = (int **)malloc(20 * (sizeof(int *)));
  for (int i = 0; i < 21; i++) {
    Rooms[i] = (int *)malloc(3 * sizeof(int));
  }

  Rooms[0][0] = 0;
  Rooms[0][1] = 0;
  Rooms[0][2] = 0;

  Rooms[1][0] = 2;
  Rooms[1][1] = 5;
  Rooms[1][2] = 8;

  Rooms[2][0] = 1;
  Rooms[2][1] = 3;
  Rooms[2][2] = 10;

  Rooms[3][0] = 2;
  Rooms[3][1] = 4;
  Rooms[3][2] = 12;

  Rooms[4][0] = 3;
  Rooms[4][1] = 5;
  Rooms[4][2] = 14;

  Rooms[5][0] = 1;
  Rooms[5][1] = 4;
  Rooms[5][2] = 6;

  Rooms[6][0] = 5;
  Rooms[6][1] = 7;
  Rooms[6][2] = 15;

  Rooms[7][0] = 6;
  Rooms[7][1] = 8;
  Rooms[7][2] = 17;

  Rooms[8][0] = 1;
  Rooms[8][1] = 7;
  Rooms[8][2] = 9;

  Rooms[9][0] = 8;
  Rooms[9][1] = 10;
  Rooms[9][2] = 18;

  Rooms[10][0] = 2;
  Rooms[10][1] = 9;
  Rooms[10][2] = 11;

  Rooms[11][0] = 10;
  Rooms[11][1] = 12;
  Rooms[11][2] = 19;

  Rooms[12][0] = 3;
  Rooms[12][1] = 11;
  Rooms[12][2] = 13;

  Rooms[13][0] = 12;
  Rooms[13][1] = 14;
  Rooms[13][2] = 20;

  Rooms[14][0] = 4;
  Rooms[14][1] = 13;
  Rooms[14][2] = 15;

  Rooms[15][0] = 6;
  Rooms[15][1] = 14;
  Rooms[15][2] = 16;

  Rooms[16][0] = 15;
  Rooms[16][1] = 17;
  Rooms[16][2] = 20;

  Rooms[17][0] = 7;
  Rooms[17][1] = 16;
  Rooms[17][2] = 18;

  Rooms[18][0] = 9;
  Rooms[18][1] = 17;
  Rooms[18][2] = 19;

  Rooms[19][0] = 11;
  Rooms[19][1] = 18;
  Rooms[19][2] = 20;

  Rooms[20][0] = 13;
  Rooms[20][1] = 16;
  Rooms[20][2] = 19;

  // Set random initial values for person, Wumpus, bats and pits
  initializeGame(&gameInfo);

  // Main playing loop.  Break when player dies, or player kills Wumpus
  while (personStatus && wumpusStatus) {

    // Display current room information: Room number, hazards detected
    rommInfodisplay(gameInfo, Rooms);

    // Prompt for and handle move
    printf("%d. Enter your move (or 'D' for directions): ",
           gameInfo.movingcount);
    // Note the extra space in the scanf below between the opening quote " and
    // the %c.
    //    This skips leading white space in the input so that the newline left
    //    over from a previous move is not read in and used as the current move.
    //    An alternative is using getchar() after the scanf() statement.
    scanf(" %c", &typeOfMove);

    typeOfMove = toupper(typeOfMove); // Make uppercase to facilitate checking

    // Check all types of user input and handle them.  This uses if-else-if code
    //   rather than switch-case, so that we can take advantage of break and
    //   continue.
    if (typeOfMove == 'D') {
      displayCave();
      displayInstructions();
      continue; // Loop back up to reprompt for the same move
    } else if (typeOfMove == 'P') {
      // To assist with play, display the cave layout
      displayCave();
      continue; // Loop back up to reprompt for the same move
    } else if (typeOfMove == 'M') {
      // Move to an adjacent room,
      // Note the extra space in the scanf below between the opening quote " and
      // the %c.
      //    This skips leading white space in the input so that the newline left
      //    over from a previous move is not read in and used as the current
      //    move.  An alternative is using getchar() after the scanf()
      //    statement.
      scanf(" %d", &nextRoom);

      if (adjacentRoom(Rooms[gameInfo.playerRoom], nextRoom)) {
        gameInfo.playerRoom = nextRoom; // move to a new room
        // Check if pit or wumpus is present in this new room
        checkingHazards(&gameInfo, &personStatus, Rooms);
      } else {
        printf("Invalid move.  Please retry. \n");
        continue; // Doesn't count as a move, so rectry same move.
      }
    } else if (typeOfMove == 'C') {
      // Display Cheat information
      cheatingInfo(gameInfo);
      continue; // Doesn't count as a move, so retry same move.
    } else if (typeOfMove == 'R') {
      // Specify resetting the hazards and person positions, useful for testing
      positionsResetting(&gameInfo);
      continue; // Doesn't count as a move, so retry same move.
    } else if (typeOfMove == 'G') {
      // Prompt for room number guess and see if user has found the Wumpus
      int wumpusRoomGuess = 0;
      printf("Enter room (1..20) you think Wumpus is in: ");
      scanf(" %d", &wumpusRoomGuess);
      // See if userguess was correct, for a win or loss.
      if (gameInfo.wumpusRoom == wumpusRoomGuess) {
        printf("You won!\n");
      } else {
        printf("You lost.\n");
      }
      break;
    }

    else if (typeOfMove == 'S') {
      ////bool arrow = false;
      arrowShooting(&gameInfo, Rooms, &personStatus, &wumpusStatus);
    }

    else if (typeOfMove == 'X') {
      // Exit program
      personStatus =
          false; // Indicate person is dead as a way to exit playing loop
      break;
    } else {
      printf("error\n");
    }

    // Increment the move number
    gameInfo.movingcount = gameInfo.movingcount + 1;

  } // end while(personIsAlive && wumpusIsAlive)

  printf("\nExiting Program ...\n");

  /*
  // Below are all the messages used throughout the program. You may need to
  change them,
  // or move them to a different place in the code.
  printf("1. Enter your move (or 'D' for directions): "); // prompt for user to
  enter their move printf("Invalid move.  Please retry. \n"); // message for
  invalid move

  // Message for menu option C (cheating). Note that the printf() statement
  below always prints
  // 1, 2, 3, 4, 5, 6, 7


  // All messages possible when player is in a given room
  printf("You are in room 1. ");
  printf("You smell a stench. ");
  printf("You feel a draft. ");

  printf("\n\n");

  // Message when the player dies by falling into a pit
  printf("Aaaaaaaaahhhhhh....   \n");
  printf("    You fall into a pit and die. \n");

  // Message when bats transport player


  ---- All messages related to the arrow ---

  // Attempt to shoot arrow you don't have
  printf("Sorry, you can't shoot an arrow you don't have.  Go find it.\n");



  // User enters more than 3 rooms to shoot arrow through
  printf("Sorry, the max number of rooms is 3.  Setting that value to 3.");

  // Arrow killed Wumpus
  printf( "Wumpus has just been pierced by your deadly arrow! \n"
              "Congratulations on your victory, you awesome hunter you.\n");

  // Arrow killed player
  printf( "You just shot yourself.  \n"
              "Maybe Darwin was right.  You're dead.\n");

  // Attempt to shoot arrow through room that is not adjacent
  printf("Room %d is not adjacent.  Arrow ricochets...\n", 1); // always prints
  room 1

  // Arrow ricochets, and kills person
  printf( "You just shot yourself, and are dying.\n"
              "It didn't take long, you're dead.\n");

  // Arrow ricochets, and kills Wumpus
  printf( "Your arrow ricochet killed the Wumpus, you lucky dog!\n"
              "Accidental victory, but still you win!\n");

  // Player picks up arrow
  printf("Congratulations, you found the arrow and can once again shoot.\n");

  --- End of messages related to the arrow ---

  // Both possible messages when the player enters a room that has the Wumpus
  printf( "You hear a slithering sound, as the Wumpus slips away. \n"
  "Whew, that was close! \n");
  printf( "You briefly feel a slimy tentacled arm as your neck is snapped. \n"
  "It is over.\n");

      // Both possible messages after the user guesses where the Wumpus is, for
  a win or loss. printf("You won!\n"); printf("You lost.\n");

  // Prompt for user when they choose to reset
  printf("Enter the room locations (1..20) for player, wumpus, pit1, pit2,
  bats1, bats2, and arrow: \n");

      // Prompt for guess of where Wumpus room is
      printf("Enter room (1..20) you think Wumpus is in: ");

  // Message that prints at the end of the program
  printf("\nExiting Program ...\n");
  */
  return 0;
}
