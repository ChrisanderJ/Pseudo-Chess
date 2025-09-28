/*********************************************************************************************************
This is to certify that this project is my own work, based on my personal efforts in studying and applying the concepts
learned. I have constructed the functions and their respective algorithms and corresponding code by myself. The
program was run, tested, and debugged by my own efforts. I further certify that I have not copied in part or whole or
otherwise plagiarized the work of other students and/or persons.
<Yu, Chrisander Jervin C.>, DLSU ID# <12318493>
*********************************************************************************************************/

#include <stdio.h>
#include <string.h>
#include <ctype.h>  // for uppercasing and lowercasing

#define MAX_PIECES 32 // number of chess pieces
#define BOARD_SIZE 8  // 8 x 8 chess board
#define MAX_PLAYERS 100
#define MAX_NAME_LEN 100

typedef char PlayerName[100]; 

typedef struct {
    char type;   // 'P' for Pawn, 'R' for Rook, etc.
    char color;  // 'W' for White, 'B' for Black
    int x, y;    // Position on the board
} ChessPiece;

typedef struct {
    char name[MAX_NAME_LEN];
    int score;
} PlayerScores;

void MainMenu();
void PlayMenu();
int printInitialBoard(ChessPiece pieces[]);
void UpdateBoard(ChessPiece pieces[], int count);
void handleTurn(ChessPiece pieces[], int *count, char player1[], char player2[], 
    int *whitePieces, int *blackPieces, char *turn, int *choice,
    PlayerScores localPlayers[]);
void evaluatePMove(ChessPiece pieces[], int *count, char turn, int *whitePieces, int *blackPieces);
void evaluateHMove(ChessPiece pieces[], int *count, char turn, int *whitePieces, int *blackPieces);
void evaluateBMove(ChessPiece pieces[], int *count, char turn, int *whitePieces, int *blackPieces);
void evaluateRMove(ChessPiece pieces[], int *count, char turn, int *whitePieces, int *blackPieces);
void evaluateQMove(ChessPiece pieces[], int *count, char turn, int *whitePieces, int *blackPieces);
void evaluateKMove(ChessPiece pieces[], int *count, char turn, int *whitePieces, int *blackPieces);
int checkSquare(ChessPiece pieces[], int count, int x, int y, char turn, int *captured);
void promotePawn(ChessPiece pieces[], int index);
int hasPiece(ChessPiece pieces[], int count, char turn, char type);
int hasValidMoves(ChessPiece pieces[], int count, char turn, char type);
int playerHasAnyMoves(ChessPiece pieces[], int count, char turn);
int areNamesEqual(char *name1, char *name2);
void capitalizeName(char *name);
void displayHighScores();
void updateScoreFile(char *player1, char *player2, char *winner, int result);
void LocalScores(PlayerScores local[], char *player1, char *player2, char *winner, int result);
void saveGame(ChessPiece pieces[], int *count);
void loadGame(ChessPiece pieces[], int *count);

/*MainMenu displays the main menu and processes user input to navigate to different game options.
@param None
@return None*/
void MainMenu() {
    int choice;      // Stores the user's menu input
    int success;     // Validate user input for menu choice (flag)
    int exitFlag;    // Track when to exit (flag)

    exitFlag = 0;

    while (exitFlag == 0) { // Loop runs until the user chooses to exit
        success = 0;
        
        printf("\nMAIN MENU\n");
        printf("1 - PLAY\n");
        printf("2 - HIGH SCORES\n");
        printf("3 - EXIT\n");
        printf("What would you like to do? ");

        while (success == 0) {
            printf("Enter your choice: ");
            
            if (scanf("%d", &choice) == 1) {
                success = 1; // Valid input received
            } 
            else {
                printf("Invalid input. Please enter a number.\n");
                
                while (getchar() != '\n'); 
            }
        }

        if (choice == 1) {
            PlayMenu(); 
        } 
        else if (choice == 2) {
            displayHighScores(); 
        } 
        else if (choice == 3) {
            exitFlag = 1; // Set flag to exit loop and end program
        } 
        else {
            printf("\nPlease enter a valid option.\n");
        }
    }

    printf("\nThank you for playing!\n");
}


/*printInitialBoard reads chess piece data from a file and prints the initial board.
@param pieces - An array of ChessPiece structures that stores the piece type, color, and position
@return the number of pieces read from the file, returns 0 if the file cannot be opened*/
int printInitialBoard(ChessPiece pieces[]) {
    FILE *pF;                           // file pointer
    int index = 0;                      // tracks the number of pieces read from the file
    char pieceType, pieceColor[10];     // piece type and color
    int row, col;                       // row and column for piece position
    int i, j, k;                        // loop counters
    int found;                          // check if piece is found (flag)

    pF = fopen("chess.txt", "r");
    if (pF == NULL) {
        printf("Error: Could not open chess.txt\n");
        return 0;
    }

    // Read from file
    while (index < MAX_PIECES && fscanf(pF, " %c %s %d %d", &pieceType, pieceColor, &row, &col) == 4) {
        pieces[index].type = pieceType;
        
        if (strcmp(pieceColor, "white") == 0) {
            pieces[index].color = 'W';
        } 
        else {
            pieces[index].color = 'B';
        }
        
        pieces[index].x = row;
        pieces[index].y = col;
        index = index + 1;
    }
    fclose(pF);

    // Print board header
    printf("\n    1   2   3   4   5   6   7   8  \n");
    printf("  +---+---+---+---+---+---+---+---+\n");

    // Print board squares
    i = 0;
    while (i < BOARD_SIZE) {
        printf("%d |", 8 - i);
        
        j = 0;
        while (j < BOARD_SIZE) {
            found = 0;
            
            k = 0;
            while (k < index) {
                if (pieces[k].x == i && pieces[k].y == j) {
                    if (pieces[k].color == 'B') {
                        printf(" \033[91m%c\033[0m |", pieces[k].type);  // Red for black pieces
                    } 
                    else {
                        printf(" %c |", pieces[k].type);  // Normal for white pieces
                    }
                    found = 1;
                }
                k = k + 1;
            }

            if (found == 0) {
                printf("   |");  // Empty square
            }
            j = j + 1;
        }

        printf("\n  +---+---+---+---+---+---+---+---+\n");
        i = i + 1;
    }

    return index;
}


/*UpdateBoard updates and prints the current state of the chessboard based on the given piece positions.
@param pieces - An array of ChessPiece structures representing the pieces currently in play
@param count - The number of pieces in the pieces array
@return None*/
void UpdateBoard(ChessPiece pieces[], int count) {
    char board[BOARD_SIZE][BOARD_SIZE];      // Stores the board state with piece types or empty spaces
    char colorBoard[BOARD_SIZE][BOARD_SIZE]; // Used to store color info ('W' or 'B')
    int i, j;                                // Loop counters

    // Initialize the board to empty spaces and color to ' ' (no color)
    i = 0;
    while (i < BOARD_SIZE) {
        j = 0;
        while (j < BOARD_SIZE) {
            board[i][j] = ' ';
            colorBoard[i][j] = ' ';
            j = j + 1;
        }
        i = i + 1;
    }

    // Place pieces on the board
    i = 0;
    while (i < count) {
        // Ensure the piece is within the bounds of the board
        if (pieces[i].x >= 0 && pieces[i].x < BOARD_SIZE && pieces[i].y >= 0 && pieces[i].y < BOARD_SIZE) {
            board[pieces[i].x][pieces[i].y] = pieces[i].type;
            colorBoard[pieces[i].x][pieces[i].y] = pieces[i].color; // Store color info
        } else {
            printf("Warning: Piece at (%d, %d) is out of bounds and will not be displayed.\n", pieces[i].x, pieces[i].y);
        }
        i = i + 1;
    }

    // Print the updated board
    printf("\n    1   2   3   4   5   6   7   8  \n");
    printf("  +---+---+---+---+---+---+---+---+\n");

    i = 0;
    while (i < BOARD_SIZE) {
        printf("%d |", 8 - i); // Display row numbers in descending order (8 to 1)
        j = 0;
        while (j < BOARD_SIZE) {
            if (colorBoard[i][j] == 'B') {
                printf(" \033[91m%c\033[0m |", board[i][j]); // Red for black pieces
            } else {
                printf(" %c |", board[i][j]); // Normal color for white or empty
            }
            j = j + 1;
        }
        printf("\n  +---+---+---+---+---+---+---+---+\n");
        i = i + 1;
    }
    
}


/*PlayMenu displays the play menu and manages game-related options such as loading a previous game,
starting a new game, and viewing high scores.
@param None
@return None*/
void PlayMenu() {
    int choice = 0;                                      // player's choice in the play menu
    int count = 0;                                       // number of pieces on the board
    int whitePieces = 0;                                 // initial white pieces
    int blackPieces = 0;                                 // initial black pieces
    int success = 0;                                     // used for input validation loop
    char turn = 'W';                                     // current turn, starts with White
    int i = 0;                                           // loop counter
    
    PlayerName player1, player2;                         // names of the two players
    ChessPiece pieces[MAX_PIECES];                       // array to hold current pieces
    PlayerScores localPlayers[2] = { {"", 0}, {"", 0} }; // local session score tracker (initialized)

    // Menu loop
    while (choice != 4) {
        printf("\nPLAY MENU\n");
        printf("1 - Load Previous\n");
        printf("2 - New Game\n");
        printf("3 - High Scores\n");
        printf("4 - Exit\n");

        success = 0;
        while (success == 0) {
            printf("Enter your choice: ");
            
            if (scanf("%d", &choice) == 1) {
                if (choice >= 1 && choice <= 4) {
                    success = 1;
                } else {
                    printf("Invalid input. Please enter a number from 1 to 4.\n");
                }
            } else {
                printf("Invalid input. Please enter a number.\n");
                
                while (getchar() != '\n'); 
            }
        }

        // Load choice
        if (choice == 1) {
            loadGame(pieces, &count);
            whitePieces = 0;
            blackPieces = 0;
            printf("Enter name for first player (White): ");
            scanf(" %99s", player1);
            printf("Enter name for second player (Black): ");
            scanf(" %99s", player2);
            capitalizeName(player1);
            capitalizeName(player2);
            
            // Count the number of white and black pieces on the board
            i = 0;
            while (i < count) {
                if (pieces[i].color == 'W') {
                    whitePieces = whitePieces + 1;
                } else {
                    blackPieces = blackPieces + 1;
                }
                i = i + 1;
            }

            LocalScores(localPlayers, player1, player2, NULL, 0);
            turn = 'W';
            handleTurn(pieces, &count, player1, player2, &whitePieces, &blackPieces, &turn, &choice, localPlayers);
        } 

        // New game 
        else if (choice == 2) {
            printf("Enter name for first player: ");
            scanf(" %99s", player1);
            printf("Enter name for second player: ");
            scanf(" %99s", player2);
            capitalizeName(player1);
            capitalizeName(player2);

            LocalScores(localPlayers, player1, player2, NULL, 0);
            whitePieces = 16;
            blackPieces = 16;
            count = printInitialBoard(pieces);
            turn = 'W';
            handleTurn(pieces, &count, player1, player2, &whitePieces, &blackPieces, &turn, &choice, localPlayers);
        } 

        // High scores
        else if (choice == 3) {
            displayHighScores();
        }
    }
}


/*handleTurn manages player turns, move selection, game state updates, and game-saving/loading options.

@param pieces[] (ChessPiece) – Array of chess pieces on the board
@param count (int*) – Pointer to the number of pieces currently on the board
@param player1[] (char) – Name of the first player (White)
@param player2[] (char) – Name of the second player (Black)
@param whitePieces (int*) – Pointer to the number of White pieces
@param blackPieces (int*) – Pointer to the number of Black pieces
@param turn (char*) – Pointer to the current player's turn ('W' or 'B')
@param choice (int*) – Pointer to the menu choice
@param localPlayers[] (PlayerScores) – Array storing local scores for both players

@return None
*/
void handleTurn(ChessPiece pieces[], int *count, char player1[], char player2[], 
    int *whitePieces, int *blackPieces, char *turn, int *choice,
    PlayerScores localPlayers[]) {
    int validInput;             // used to validate input for piece selection (flag)
    int option = 0;             // stores player's choice for option
    int hasMoves;               // whether the opponent has valid moves (flag)
    char piece;                 // piece type input by the user
    int hasPieceType;           // whether player has selected a piece (flag)
    int canMovePiece;           // whether selected piece has valid moves (flag)
    char opponentColor;         // opponent's color
    int i;                      // loop counter

    // Game loop continues until one player has no pieces or the user chooses to exit
    while (*whitePieces > 0 && *blackPieces > 0 && *choice != 4) {
        if (*turn == 'W') {
            printf("\n%s's turn (W)\n", player1); // White's turn
        } else {
            printf("\n%s's turn (B)\n", player2); // Black's turn
        }

        validInput = 0;
        while (validInput == 0) {
            printf("OPTIONS\n");
            printf("1 - Move\n");
            printf("2 - Save\n");
            printf("3 - Load\n");
            printf("4 - Exit\n");

            printf("Enter option: ");
            if (scanf("%d", &option) == 1) {
                if (option >= 1 && option <= 4) {
                    validInput = 1;
                } else {
                    printf("Invalid choice! Please enter a number from 1 to 4.\n");
                }
            } else {
                printf("Invalid choice! Please enter a number from 1 to 4.\n");
                while (getchar() != '\n'); 
            }
        }

        if (option == 1) {
            printf("White's pieces: %d\n", *whitePieces);
            printf("Black's pieces: %d\n", *blackPieces);

            validInput = 0;
            while (validInput == 0) {
                printf("\nEnter piece to move (P, H, B, R, Q, K): ");
                if (scanf(" %c", &piece) == 1) {
                    piece = toupper(piece);
                    if (piece == 'P' || piece == 'H' || piece == 'B' || 
                        piece == 'R' || piece == 'Q' || piece == 'K') {
                        
                        hasPieceType = hasPiece(pieces, *count, *turn, piece);
                        canMovePiece = hasValidMoves(pieces, *count, *turn, piece);

                        if (hasPieceType != 0 && canMovePiece != 0) {
                            validInput = 1;
                        } else {
                            printf("Invalid move. Either you don't have that piece or it cannot move.\n");
                        }
                    } else {
                        printf("Invalid input. Please enter a valid piece (P, H, B, R, Q, K).\n");
                    }
                } else {
                    printf("Invalid input. Please enter a valid piece (P, H, B, R, Q, K).\n");
                    while (getchar() != '\n'); 
                }
            }


            // Check if the player has any valid moves for the selected piece type. 
            if (piece == 'P') {
                evaluatePMove(pieces, count, *turn, whitePieces, blackPieces);
            } else if (piece == 'H') {
                evaluateHMove(pieces, count, *turn, whitePieces, blackPieces);
            } else if (piece == 'B') {
                evaluateBMove(pieces, count, *turn, whitePieces, blackPieces);
            } else if (piece == 'R') {
                evaluateRMove(pieces, count, *turn, whitePieces, blackPieces);
            } else if (piece == 'Q') {
                evaluateQMove(pieces, count, *turn, whitePieces, blackPieces);
            } else if (piece == 'K') {
                evaluateKMove(pieces, count, *turn, whitePieces, blackPieces);
            }

            UpdateBoard(pieces, *count); // Update the board after the move

            if (*turn == 'W') {
                opponentColor = 'B'; // If the current turn is White, the opponent is Black
            } else {
                opponentColor = 'W'; // Otherwise, the opponent is White
            }
            hasMoves = playerHasAnyMoves(pieces, *count, opponentColor); // Check if the opponent has any valid moves

            // If the opponent has valid moves, switch turns
            if (hasMoves != 0) {
                
                if (*turn == 'W') {
                    *turn = 'B';
                } else {
                    *turn = 'W';
                }                
            }

            // If the opponent has no valid moves, check for game over conditions
            else {
                if (*blackPieces == 0 && *whitePieces > 0) {
                    // If Black has no pieces left and White still has pieces, White wins
                    printf("Game over! %s wins!\n", player1);
                    updateScoreFile(player1, player2, player1, 1); // Update the score file with White as the winner
                    LocalScores(localPlayers, player1, player2, player1, 1); // Update local scores for White's win
                } else if (*whitePieces == 0 && *blackPieces > 0) {
                    // If White has no pieces left and Black still has pieces, Black wins
                    printf("Game over! %s wins!\n", player2);
                    updateScoreFile(player1, player2, player2, 1); // Update the score file with Black as the winner
                    LocalScores(localPlayers, player1, player2, player2, 1); // Update local scores for Black's win                
                } else {
                // If either has pieces but no valid moves, it's a draw
                printf("Game Over! It's a draw!\n");
                updateScoreFile(player1, player2, NULL, 2); // Update the score file with a draw
                LocalScores(localPlayers, player1, player2, NULL, 2); // Update local scores for a draw
            }
           
            }

        } else if (option == 2) {
            saveGame(pieces, count);
        } else if (option == 3) {
            loadGame(pieces, count);
            
            // Reset the count of white and black pieces
            *whitePieces = 0;
            *blackPieces = 0;

            // Recalculate the number of white and black pieces on the board
            i = 0;
            while (i < *count) {
                if (pieces[i].color == 'W') {
                    *whitePieces = *whitePieces + 1;
                } else {
                    *blackPieces = *blackPieces + 1;
                }
                i = i + 1;
            }
            // Reset the turn to White after loading the game
            *turn = 'W';

        } else if (option == 4) {
            *choice = 4;
        }
    }
}


/* GENERAL MOVE EVALUATION ALGORITHM FOR THE PIECE MOVEMENTS:
1) Ask for input for target square.
2) Check for distance according to the capability of each piece.
3) Check for friendly pieces.
4) If capturing, check for possible multiple captures by different pieces, then ask user which piece to move.
5) Put the piece on desired square. If it is a capture, remove the captured piece.
6) Invalid inputs will prompt the user to enter again.
*/


/*  
evaluatePMove prompts the player for a valid target square, checks if it is occupied, and identifies eligible pawns based on legal movement rules.
If multiple pawns can capture, the player selects one.
The chosen pawn then moves, capturing any opponent's piece if present and promoting if applicable.
The function updates the board and modifies piece counts accordingly.  

@param pieces - array of ChessPiece structures representing all pieces on the board  
@param count - pointer to the total number of pieces in play  
@param turn - character representing the current player's turn ('W' for White, 'B' for Black)  
@param whitePieces - pointer to the number of remaining white pieces  
@param blackPieces - pointer to the number of remaining black pieces  

@return none

Pre-condition: The board setup is valid, and there is at least one pawn for the current player.  
*/  
void evaluatePMove(ChessPiece pieces[], int *count, char turn, int *whitePieces, int *blackPieces) {
    int inputValid;              // used to keep asking for row/column input until both are in range 1–8 (flag)
    int x, y;                    // input of chosen coordinates
    int dx, dy;                  // difference in distance between pawn and target square
    int i, j, idx;               // loop counters
    int validMove = 0;           // stays 0 until a valid move was made (flag)
    int stepBlocked;             // checks for two-step obstacle
    int isSquareEmpty;           // checks if target square is empty (boolean flag)
    int captured = -1;           // index of enemy piece at the target square, or -1 if none
    int captureCount = 0;        // how many pawns can capture on the square of target
    int captureIndex = -1;       // chosen capturing pawn
    int choice = 0;              // user choice of pawn to capture when there are multiple options
    int capturingPawns[2];       // list of indexes of capture-eligible pawns (max of 2 possible pawns that can capture)
    int success;                 // indicator of correct type for multiple pawns prompt (flag)
    int occupiedByOwnPiece = 0;  // check if own piece is on the target square (flag)

    // Repeatedly asks the player for a valid target square until a legal pawn move or capture is made.
    while (validMove == 0) {
        captureCount = 0;
        captured = -1;
        captureIndex = -1;

        inputValid = 0;
        while (inputValid == 0) {
            printf("Enter target row (1 - 8): ");
            if (scanf("%d", &x) == 1 && x >= 1 && x <= 8) {
                printf("Enter target column (1 - 8): ");
                if (scanf("%d", &y) == 1 && y >= 1 && y <= 8) {
                    inputValid = 1;
                } else {
                    printf("Invalid column! Must be a number between 1 and 8.\n");
                }
            } else {
                printf("Invalid row! Must be a number between 1 and 8.\n");
            }

            while (getchar() != '\n') {
            }
        }

        // Converts board input to array indexing
        x = 8 - x;
        y = y - 1;

        // Check if the target square is occupied and if it's an enemy or friendly piece
        occupiedByOwnPiece = checkSquare(pieces, *count, x, y, turn, &captured);

        if (occupiedByOwnPiece == 1) {
            printf("Target square is occupied by your own piece! Try again.\n");
        } else {
            // Check all pawns to find valid moves
            i = 0;
            while (i < *count && validMove == 0) {
                if (pieces[i].type == 'P' && pieces[i].color == turn) {
                    dx = x - pieces[i].x;
                    dy = y - pieces[i].y;

                    // Check if the target square is empty or occupied by an enemy piece.
                    if (captured == -1) {
                        isSquareEmpty = 1; 
                    } else {
                        isSquareEmpty = 0;
                    }

                    // Single step forward
                    if (isSquareEmpty == 1 && dy == 0) {
                        if ((turn == 'W' && dx == -1) || (turn == 'B' && dx == 1)) {
                            pieces[i].x = x;
                            pieces[i].y = y;
                            promotePawn(pieces, i); // Check for promotion
                            validMove = 1;
                        }
                    }

                    // Double step forward
                    if (isSquareEmpty == 1 && dy == 0 && validMove == 0) {
                        if ((turn == 'W' && pieces[i].x == 6 && dx == -2) ||
                            (turn == 'B' && pieces[i].x == 1 && dx == 2)) {

                            stepBlocked = 0;
                            j = 0;
                            // Check if there is a piece blocking the pawn's double-step move
                            while (j < *count && stepBlocked == 0) {
                                if (turn == 'W') {
                                    if ((pieces[j].x == pieces[i].x - 1 && pieces[j].y == pieces[i].y) ||
                                        (pieces[j].x == pieces[i].x - 2 && pieces[j].y == pieces[i].y)) {
                                        stepBlocked = 1;
                                    }
                                }
                                if (turn == 'B') {
                                    if ((pieces[j].x == pieces[i].x + 1 && pieces[j].y == pieces[i].y) ||
                                        (pieces[j].x == pieces[i].x + 2 && pieces[j].y == pieces[i].y)) {
                                        stepBlocked = 1;
                                    }
                                }
                                j = j + 1;
                            }

                            if (stepBlocked == 0) {
                                pieces[i].x = x;
                                pieces[i].y = y;
                                validMove = 1;
                            }
                        }
                    }

                    // Diagonal capture
                    if (captured != -1 && validMove == 0) {
                            // Check if the target square contains an enemy piece and no valid move has been made yet
                        if (((turn == 'W' && dx == -1) || (turn == 'B' && dx == 1)) &&
                            (dy == 1 || dy == -1)) {
                            capturingPawns[captureCount] = i; // Store the index of the pawn that can capture
                            captureCount = captureCount + 1; // Increment the count of pawns that can capture
                        }
                    }
                }
                i = i + 1;
            }

            // Handle capture
            if (captureCount > 0) {
                if (captureCount > 1) {
                    printf("Multiple pawns can capture. Choose one:\n");

                    i = 0;
                    while (i < captureCount) {
                        idx = capturingPawns[i]; // Get the index of the pawn that can capture
                        // Display the pawn's position in chessboard coordinates (row 1-8, column 1-8)
                        printf("%d: Pawn at (%d, %d)\n", i + 1, 8 - pieces[idx].x, pieces[idx].y + 1);
                        i = i + 1; // Move to the next capturing pawn
                    }

                    success = 0;
                    while (success == 0) {
                        printf("Enter choice: ");
                        if (scanf("%d", &choice) == 1 && choice >= 1 && choice <= captureCount) {
                            success = 1;
                        } else {
                            printf("Invalid input. Please enter a number from 1 to %d.\n", captureCount);
                            while (getchar() != '\n') {
                            }
                        }
                    }
                    // Assign the chosen pawn's index from the capturingPawns array
                    captureIndex = capturingPawns[choice - 1];
                } else {
                     // If only one pawn can capture, automatically select it
                    captureIndex = capturingPawns[0];
                }

                if (captureIndex != -1 && captured != -1) {
                    // Update the position of the chosen pawn to the target square
                    pieces[captureIndex].x = x;
                    pieces[captureIndex].y = y;
                    // Check if the pawn should be promoted after the move
                    promotePawn(pieces, captureIndex);

                    // Remove the captured piece from the board
                    j = captured;
                    while (j < *count - 1) {
                        pieces[j] = pieces[j + 1]; // Shift pieces
                        j = j + 1;
                    }

                    *count = *count - 1; // Decrease the total number of pieces

                    if (turn == 'W') {
                        *blackPieces = *blackPieces - 1;
                    } else {
                        *whitePieces = *whitePieces - 1;
                    }

                    printf("Pawn captured a piece!\n");
                    validMove = 1;
                }
            }

            if (validMove == 0) {
                printf("Invalid pawn move! Try again.\n");
            }
        }
    }
}


/*  
evaluateHMove prompts the player for a valid target square, checks if it is occupied, and identifies eligible knights based on L-shaped movement.
If multiple knights can move, the player selects one. The chosen knight then moves, capturing any opponent's piece if present.
The function updates the board and modifies piece counts accordingly.  

@param pieces - array of ChessPiece structures representing all pieces on the board  
@param count - pointer to the total number of pieces in play  
@param turn - character representing the current player's turn ('W' for White, 'B' for Black)  
@param whitePieces - pointer to the number of remaining white pieces  
@param blackPieces - pointer to the number of remaining black pieces  

@return void  

Pre-condition: The board setup is valid, and there is at least one knight for the current player.  
*/  

void evaluateHMove(ChessPiece pieces[], int *count, char turn, int *whitePieces, int *blackPieces) {
    int inputValid = 0;            // Flag for valid input
    int x, y;                      // Target square coordinates
    int dx, dy;                    // Difference between knight and target square
    int i, j;                      // Loop counters
    int idx;                       // Index for displaying knight info
    int validMove = 0;             // Flag for a successful move
    int occupiedByOwnPiece = 0;    // Flag for checking if the target square is occupied by own piece
    int moveCount = 0;             // Number of knights that can move/capture to target
    int choice = 0;                // User's choice when multiple knights can move
    int moveIndex = 0;             // Final chosen knight to move
    int isLShape = 0;              // Flag to check knight's movement shape
    int success = 0;               // Flag for valid user input in multiple knights prompt
    int captured = -1;             // Index of captured enemy piece (-1 if none)
    int eligibleKnights[10];       // Stores indexes of knights that can move to target

    // Loop until a valid knight move is made
    while (validMove == 0) {
        // Ask for a valid target square
        inputValid = 0;
        while (inputValid == 0) {
            printf("Enter target row (1 - 8): ");
            if (scanf("%d", &x) == 1 && x >= 1 && x <= 8) {
                printf("Enter target column (1 - 8): ");
                if (scanf("%d", &y) == 1 && y >= 1 && y <= 8) {
                    inputValid = 1;
                } else {
                    printf("Invalid column! Must be a number between 1 and 8.\n");
                }
            } else {
                printf("Invalid row! Must be a number between 1 and 8.\n");
            }

            while (getchar() != '\n'); // Clear input buffer
        }

        // Convert chessboard input to array index
        x = 8 - x;
        y = y - 1;

        // Check if target square is occupied by own or enemy piece
        occupiedByOwnPiece = checkSquare(pieces, *count, x, y, turn, &captured);

        if (occupiedByOwnPiece == 1) {
            printf("Target square is occupied by your own piece! Try again.\n");
        } else {
            // Check which knights can move to the target square
            i = 0;
            moveCount = 0;

            while (i < *count) {
                if (pieces[i].type == 'H' && pieces[i].color == turn) {
                    dx = x - pieces[i].x;
                    dy = y - pieces[i].y;

                    // Validate knight's L-shaped move
                    isLShape = ((dx == 2 || dx == -2) && (dy == 1 || dy == -1)) ||
                               ((dx == 1 || dx == -1) && (dy == 2 || dy == -2));

                    if (isLShape) {
                        // Store the index of the knight that can move to the target square
                        eligibleKnights[moveCount] = i;

                         // Increment the count of knights that can move there
                        moveCount++;
                    }
                }
                i++;
            }

            if (moveCount == 0) {
                printf("Invalid knight move! Try again.\n");
            } else {
                // If multiple knights can move, ask user to choose
                if (moveCount > 1) {
                    printf("Multiple knights can move to the target square. Choose one:\n");
                    // Display all knights that can move to the target square
                    for (i = 0; i < moveCount; i++) {
                        idx = eligibleKnights[i]; // Retrieve the knight's index from the list of eligible knights
                        printf("%d: Knight at (%d, %d)\n", i + 1, 8 - pieces[idx].x, pieces[idx].y + 1);
                    }

                    success = 0;
                    while (success == 0) {
                        printf("Enter choice: ");
                        if (scanf("%d", &choice) == 1 && choice >= 1 && choice <= moveCount) {
                            success = 1;
                        } else {
                            printf("Invalid input. Please enter a number from 1 to %d.\n", moveCount);
                        }

                        while (getchar() != '\n'); // Clear input buffer
                    }

                    // Assign the chosen knight's index from eligibleKnights to moveIndex
                    moveIndex = eligibleKnights[choice - 1];
                } else {
                    moveIndex = eligibleKnights[0]; // Only one knight can move
                }

                // Move knight to the target square
                pieces[moveIndex].x = x;
                pieces[moveIndex].y = y;
                printf("Knight moved to (%d, %d)!\n", 8 - x, y + 1);

                // Handle piece capture
                if (captured != -1) {
                    // Adjust moveIndex if it comes after the captured piece in the array
                    if (moveIndex > captured) {
                        moveIndex--; 
                    }

                    // Shift pieces
                    for (j = captured; j < *count - 1; j++) {
                        pieces[j] = pieces[j + 1]; 
                    }

                    (*count)--;

                    if (turn == 'W') {
                        (*blackPieces)--;
                    } else {
                        (*whitePieces)--;
                    }

                    printf("Knight captured an enemy piece!\n");
                }

                validMove = 1; // Exit loop
            }
        }
    }
}


/*  
evaluateBMove prompts the player for a valid target square, checks if it is occupied, and identifies eligible bishops based on diagonal movement.  
If multiple bishops can move, the player selects one. The chosen bishop then moves, capturing any opponent's piece if present.  
The function updates the board and modifies piece counts accordingly.    

@param pieces - array of ChessPiece structures representing all pieces on the board  
@param count - pointer to the total number of pieces in play  
@param turn - character representing the current player's turn ('W' for White, 'B' for Black)  
@param whitePieces - pointer to the number of remaining white pieces  
@param blackPieces - pointer to the number of remaining black pieces  

@return void  

Pre-condition: The board setup is valid, and there is at least one bishop for the current player.  
*/  

void evaluateBMove(ChessPiece pieces[], int *count, char turn, int *whitePieces, int *blackPieces) {
    int inputValid;                 // used to keep asking for row/column input until both are in range 1–8 (flag)
    int x, y;                       // input of chosen coordinates
    int dx, dy;                     // difference between bishop and target square
    int i, j, k;                    // loop counters
    int idx;                        // temporary index to access bishop from eligibleBishops[]
    int validMove = 0;              // stays 0 until a valid move is made (flag)
    int occupiedByOwnPiece = 0;     // check if own piece is on the target square (flag)
    int moveCount = 0;              // number of bishops that can move to target
    int choice = 0;                 // user choice if multiple bishops can move
    int moveIndex = 0;              // final chosen bishop to move
    int stepX = 0, stepY = 0;       // direction of diagonal movement
    int steps = 0;                  // number of steps to target square
    int checkX = 0, checkY = 0;     // coordinates of square being checked
    int pathClear = 1;              // check if bishop's path is clear (flag)
    int pieceOnPath = 0;            // detect piece in bishop's path (flag)
    int success;                    // indicator of correct type for multiple bishops prompt (flag)
    int captured = -1;              // index of enemy piece at the target square, or -1 if none
    int eligibleBishops[10];        // list of indexes of bishops that can move to target square (max of 10 if all promote)

    // Repeatedly asks the player for a valid target square until a legal bishop move or capture is made.
    while (validMove == 0) {
        inputValid = 0;
        captured = -1;
        moveCount = 0;
        occupiedByOwnPiece = 0;

        // Ensures that the entered row and column are valid board coordinates.
        while (inputValid == 0) {
            printf("Enter target row (1 - 8): ");
            if (scanf("%d", &x) == 1 && x >= 1 && x <= 8) {
                printf("Enter target column (1 - 8): ");
                if (scanf("%d", &y) == 1 && y >= 1 && y <= 8) {
                    inputValid = 1;
                } else {
                    printf("Invalid column! Must be a number between 1 and 8.\n");
                }
            } else {
                printf("Invalid row! Must be a number between 1 and 8.\n");
            }
            while (getchar() != '\n');
        }

        // Converts board input to array indexing
        x = 8 - x;
        y = y - 1;
        
        // Check if the target square is occupied and if it's an enemy or friendly piece
        occupiedByOwnPiece = checkSquare(pieces, *count, x, y, turn, &captured);

        if (occupiedByOwnPiece == 1) {
            printf("Target square is occupied by your own piece! Try again.\n");
        } else {

            // Check all bishops to find valid moves
            i = 0;
            while (i < *count) {
                if (pieces[i].type == 'B' && pieces[i].color == turn) {
                    dx = x - pieces[i].x;
                    dy = y - pieces[i].y;
                    
                    // Bishop moves must be diagonal, (dx equals absolute dy).
                    if (dx == dy || dx == -dy) {
                        
                        // Determine movement direction and number of steps
                        if (dx > 0) {
                            steps = dx;
                            stepX = 1; // Move right  
                        } else {
                            steps = -dx;
                            stepX = -1; // Move left  
                        }
                        
                        if (dy > 0) {
                            stepY = 1; // Move up 
                        } else {
                            stepY = -1; // Move down 
                        }
                        
                        // Check if there are pieces in the bishop's path
                        pathClear = 1;
                        k = 1;
                        while (k < steps) {
                            checkX = pieces[i].x + k * stepX;
                            checkY = pieces[i].y + k * stepY;
                            pieceOnPath = 0;
                            j = 0;
                            while (j < *count) {
                                if (pieces[j].x == checkX && pieces[j].y == checkY) {
                                    pieceOnPath = 1;
                                }
                                j++;
                            }
                            if (pieceOnPath == 1) {
                                pathClear = 0;
                            }
                            k++;
                        }
                        
                        if (pathClear == 1) {
                            eligibleBishops[moveCount] = i; //  Store the index of the current bishop if it can move to the target square
                            moveCount++;
                        }
                    }
                }
                i++;
            }

            // Handle move selection when multiple bishops can reach the target
            if (moveCount == 0) {
                printf("Invalid bishop move! Try again.\n");
            } else {
                if (moveCount > 1) {
                    printf("Multiple bishops can move to the target square. Choose one:\n");
                    for (i = 0; i < moveCount; i++) {
                        idx = eligibleBishops[i]; // Get the index of the bishop that can capture
                        printf("%d: Bishop at (%d, %d)\n", i + 1, 8 - pieces[idx].x, pieces[idx].y + 1);
                    }
                    
                    success = 0;
                    while (success == 0) {
                        printf("Enter choice: ");
                        if (scanf("%d", &choice) == 1 && choice >= 1 && choice <= moveCount) {
                            success = 1;
                        } else {
                            printf("Invalid input. Please enter a number from 1 to %d.\n", moveCount);
                        }
                        while (getchar() != '\n');
                    }
                    moveIndex = eligibleBishops[choice - 1]; // Assign the chosen bishop's index from the eligibleBishops array
                } else {
                    moveIndex = eligibleBishops[0]; // Only one bishop can move
                }

                // Update the position of the chosen bishop to the target square
                pieces[moveIndex].x = x;
                pieces[moveIndex].y = y;
                printf("Bishop moved to (%d, %d)!\n", 8 - x, y + 1);

                // Handle capture if an enemy piece is present
                if (captured != -1) {
                    for (j = captured; j < *count - 1; j++) {
                        pieces[j] = pieces[j + 1]; // Shift pieces 
                    }
                    (*count)--;
                    if (turn == 'W') {
                        (*blackPieces)--;
                    } else {
                        (*whitePieces)--;
                    }
                    printf("Bishop captured an enemy piece!\n");
                }
                validMove = 1;
            }
        }
    }
}


/*  
evaluateRMove prompts the player for a valid target square, checks if it is occupied, and identifies eligible rooks based on straight-line movement.  
If multiple rooks can move, the player selects one. The chosen rook then moves, capturing any opponent's piece if present.  
The function updates the board and modifies piece counts accordingly.  

@param pieces - array of ChessPiece structures representing all pieces on the board  
@param count - pointer to the total number of pieces in play  
@param turn - character representing the current player's turn ('W' for White, 'B' for Black)  
@param whitePieces - pointer to the number of remaining white pieces  
@param blackPieces - pointer to the number of remaining black pieces  

@return void  

Pre-condition: The board setup is valid, and there is at least one rook for the current player.  
*/  

void evaluateRMove(ChessPiece pieces[], int *count, char turn, int *whitePieces, int *blackPieces) {
        int inputValid;                  // used to keep asking for row/column input until both are in range 1–8 (flag)
        int x, y;                        // input of chosen coordinates
        int dx, dy;                      // difference between rook and target square
        int i, j, k;                     // loop counters
        int idx;                         // temporary index to access rook from eligibleRooks[]
        int validMove = 0;               // stays 0 until a valid move is made (flag)
        int occupiedByOwnPiece = 0;      // check if own piece is on the target square (flag)
        int moveCount = 0;               // number of rooks that can move to target
        int choice = 0;                  // user choice if multiple rooks can move
        int moveIndex = 0;               // final chosen rook to move
        int stepX = 0, stepY = 0;        // direction of movement
        int steps = 0;                   // number of steps to target square
        int checkX = 0, checkY = 0;      // coordinates of square being checked
        int pathClear = 1;               // check if rook's path is clear (flag)
        int pieceOnPath = 0;             // detect piece in rook's path (flag)
        int success;                     // indicator of correct type for multiple pawns prompt (flag)
        int captured = -1;               // index of enemy piece at the target square, or -1 if none
        int eligibleRooks[10];           // list of indexes of rooks that can move to target square (max of 10 if all promote)
    
        // Repeatedly asks the player for a valid target square until a legal rook move or capture is made.
        while (validMove == 0) {
            inputValid = 0;
            captured = -1;
            moveCount = 0;
            occupiedByOwnPiece = 0;
    
            while (inputValid == 0) {
                printf("Enter target row (1 - 8): ");
                if (scanf("%d", &x) == 1 && x >= 1 && x <= 8) {
                    printf("Enter target column (1 - 8): ");
                    if (scanf("%d", &y) == 1 && y >= 1 && y <= 8) {
                        inputValid = 1;
                    } else {
                        printf("Invalid column! Must be a number between 1 and 8.\n");
                    }
                } else {
                    printf("Invalid row! Must be a number between 1 and 8.\n");
                }
    
                // Flush any remaining input (stray characters, newlines)
                while (getchar() != '\n') {
                }
            }
    
            // Converts chessboard input to array indexing (e.g., row 8 becomes index 0)
            x = 8 - x;
            y = y - 1;
    
            // Check if the target square is occupied and if it's an enemy or friendly piece
            occupiedByOwnPiece = checkSquare(pieces, *count, x, y, turn, &captured);
    
            if (occupiedByOwnPiece == 1) {
                printf("Target square is occupied by your own piece! Try again.\n");
            } else {
                // Loop through all of the player's rooks to check if they can move to the target square
                i = 0;
                while (i < *count) {
                    if (pieces[i].type == 'R' && pieces[i].color == turn) {
                        dx = x - pieces[i].x;
                        dy = y - pieces[i].y;
    
                        // Rook can move only in straight lines
                        if ((dx == 0 && dy != 0) || (dy == 0 && dx != 0)) {
                            // Determine direction of movement
                            if (dx > 0) {
                                stepX = 1; // Move downward 
                                steps = dx; // Number of rows to move
                            } else if (dx < 0) {
                                stepX = -1; // Move upward 
                                steps = -dx;
                            } else {
                                stepX = 0; // No vertical movement
                            }
    
                            if (dy > 0) {
                                stepY = 1; // Move right
                                steps = dy; // Number of columns to move
                            } else if (dy < 0) {
                                stepY = -1; // Move left
                                steps = -dy;
                            } else if (dx != 0) {
                                stepY = 0; // If dx is non-zero but dy is 0, no horizontal movement
                            }
    
                            pathClear = 1; // Assume path is clear
                            k = 1;
                            while (k < steps) {
                                checkX = pieces[i].x + k * stepX; // (current row of rook) + (step number) * (vertical direction: -1 for up, 1 for down)
                                checkY = pieces[i].y + k * stepY; // (current column of rook) + (step number) * (horizontal direction: -1 for left, 1 for right)
                                pieceOnPath = 0;
    
                                j = 0;
                                // Checks if there is a piece blocking path
                                while (j < *count) {
                                    if (pieces[j].x == checkX && pieces[j].y == checkY) {
                                        pieceOnPath = 1;
                                    }
                                    j = j + 1; // Loops through all pieces on the board.
                                }
    
                                if (pieceOnPath == 1) {
                                    pathClear = 0;
                                }
    
                                k = k + 1; // Move to the next square along the path and repeat.
                            }
    
                            if (pathClear == 1) {
                                eligibleRooks[moveCount] = i; // Store the index of the moving/capturing rook in the eligibleRooks array.
                                moveCount = moveCount + 1;    // Increase the number of rooks that can capture
                            }
                        }
                    }
                    i = i + 1;
                }
    
                if (moveCount == 0) {
                    printf("Invalid rook move! Try again.\n");
                } else {
                    if (moveCount > 1) {
                        printf("Multiple rooks can move to the target square. Choose one:\n");
                        i = 0;
                        while (i < moveCount) {
                            idx = eligibleRooks[i];
                            printf("%d: Rook at (%d, %d)\n", i + 1, 8 - pieces[idx].x, pieces[idx].y + 1);
                            i = i + 1;
                        }
    
                        success = 0;
                        while (success == 0) {
                            printf("Enter choice: ");
                            if (scanf("%d", &choice) == 1 && choice >= 1 && choice <= moveCount) {
                                success = 1; // Valid input
                            } else {
                                printf("Invalid input. Please enter a number from 1 to %d.\n", moveCount);
                            }
    
                            while (getchar() != '\n') {
                            }
                        }
    
                        moveIndex = eligibleRooks[choice - 1]; // Assign the chosen rook's index from the eligibleRooks array
                    } else {
                        moveIndex = eligibleRooks[0]; // Only one rook can move
                    }
    
                    // Move the rook
                    pieces[moveIndex].x = x;
                    pieces[moveIndex].y = y;
                    printf("Rook moved to (%d, %d)!\n", 8 - x, y + 1);
    
                    // Handle capture if any
                    if (captured != -1) {
                        if (moveIndex > captured) {
                            moveIndex = moveIndex - 1; // Adjust moveIndex if it comes after the captured piece in the array
                        }
    
                        j = captured;
                            // Shift pieces
                            while (j < *count - 1) {
                                pieces[j] = pieces[j + 1]; 
                                j = j + 1;
                            }
    
                        *count = *count - 1;
    
                        if (turn == 'W') {
                            *blackPieces = *blackPieces - 1;
                        } else {
                            *whitePieces = *whitePieces - 1;
                        }
    
                        printf("Rook captured an enemy piece!\n");
                    }
    
                    validMove = 1;
                }
            }
        }
    }


/*  
evaluateQMove prompts the player for a valid target square, checks if it is occupied, and identifies eligible queens based on valid movement rules.  
Queens can move in straight lines (horizontally, vertically) or diagonally. 
If multiple queens can move, the player selects one. The chosen queen then moves, capturing any opponent's piece if present.  
The function updates the board state and modifies piece counts accordingly.  

@param pieces - array of ChessPiece structures representing all pieces on the board  
@param count - pointer to the total number of pieces in play  
@param turn - character representing the current player's turn ('W' for White, 'B' for Black)  
@param whitePieces - pointer to the number of remaining white pieces  
@param blackPieces - pointer to the number of remaining black pieces  

@return void  

Pre-condition: The board setup is valid, and there is at least one queen for the current player.  
*/    
void evaluateQMove(ChessPiece pieces[], int *count, char turn, int *whitePieces, int *blackPieces) {
    int inputValid;                  // used to keep asking for row/column input until both are in range 1–8 (flag)
    int x, y;                        // input of chosen coordinates
    int dx, dy;                      // difference between queen and target square
    int i, j, k;                     // loop counters
    int idx;                         // temporary index to access queen from eligibleQueens[]
    int validMove = 0;               // stays 0 until a valid move is made (flag)
    int occupiedByOwnPiece = 0;      // check if own piece is on the target square (flag)
    int moveCount = 0;               // number of queens that can move to target
    int choice = 0;                  // user choice if multiple queens can move
    int moveIndex = 0;               // final chosen queen to move
    int stepX = 0, stepY = 0;        // direction of movement
    int steps = 0;                   // number of steps to target square
    int checkX = 0, checkY = 0;      // coordinates of square being checked
    int pathClear = 1;               // check if queen's path is clear (flag)
    int pieceOnPath = 0;             // detect piece in queen's path (flag)
    int success;                     // indicator of correct type for multiple pawns prompt (flag)
    int captured = -1;               // index of enemy piece at the target square, or -1 if none
    int eligibleQueens[9];           // list of indexes of queens that can move to target square (max of 9 if all promote)

    // Repeatedly asks the player for a valid target square until a legal queen move or capture is made.
    while (validMove == 0) {
        inputValid = 0;
        captured = -1;
        moveCount = 0;
        occupiedByOwnPiece = 0;

        while (inputValid == 0) {
            printf("Enter target row (1 - 8): ");
            if (scanf("%d", &x) == 1 && x >= 1 && x <= 8) {
                printf("Enter target column (1 - 8): ");
                if (scanf("%d", &y) == 1 && y >= 1 && y <= 8) {
                    inputValid = 1;
                } else {
                    printf("Invalid column! Must be a number between 1 and 8.\n");
                }
            } else {
                printf("Invalid row! Must be a number between 1 and 8.\n");
            }

            // Flush any remaining input (stray characters, newlines)
            while (getchar() != '\n') {
            }
        }

        // Converts chessboard input to array indexing (e.g., row 8 becomes index 0)
        x = 8 - x;
        y = y - 1;

        // Check if the target square is occupied and if it's an enemy or friendly piece
        occupiedByOwnPiece = checkSquare(pieces, *count, x, y, turn, &captured);

        if (occupiedByOwnPiece == 1) {
            printf("Target square is occupied by your own piece! Try again.\n");
        } else {
            // Loop through all of the player's queens to check if they can move to the target square
            i = 0;
            while (i < *count) {
                if (pieces[i].type == 'Q' && pieces[i].color == turn) {
                    dx = x - pieces[i].x;
                    dy = y - pieces[i].y;

                    // Queen moves in straight or diagonal lines
                    if ((dx == 0 && dy != 0) || (dy == 0 && dx != 0) || (dx == dy || dx == -dy)) {
                        // Calculate direction and steps
                        if (dx > 0) {
                            stepX = 1; // Move downward
                            steps = dx; // Number of rows to move
                        } else if (dx < 0) {
                            stepX = -1; // Move upward
                            steps = -dx; 
                        } else {
                            stepX = 0; // No vertical movement
                            if (dy > 0) {
                                steps = dy; // Move right
                            } else {
                                steps = -dy; // Move left
                            }
                        }

                        if (dy > 0) {
                            stepY = 1; // Move right
                        } else if (dy < 0) {
                            stepY = -1; // Move left
                        } else {
                            stepY = 0; // No horizontal movement
                        }

                        pathClear = 1; // Assume path is clear
                        k = 1;
                        // Loop to check each square along the path
                        while (k < steps) {
                            checkX = pieces[i].x + k * stepX; // starting row + k * direction
                            checkY = pieces[i].y + k * stepY; // starting column + k * direction
                            pieceOnPath = 0;

                            j = 0;
                            // Checks if there is a piece blocking path
                            while (j < *count) {
                                if (pieces[j].x == checkX && pieces[j].y == checkY) {
                                    pieceOnPath = 1;
                                }
                                j = j + 1;
                            }

                            if (pieceOnPath == 1) {
                                pathClear = 0;
                            }

                            k = k + 1;
                        }

                        if (pathClear == 1) {
                            eligibleQueens[moveCount] = i; // Store index of the valid queen
                            moveCount = moveCount + 1;     // Increase the number of eligible queens
                        }
                    }
                }
                i = i + 1;
            }

            if (moveCount == 0) {
                printf("Invalid queen move! Try again.\n");
            } else {
                // Handle multiple queens
                if (moveCount > 1) {
                    printf("Multiple queens can move to the target square. Choose one:\n");
                    i = 0;
                    while (i < moveCount) {
                        idx = eligibleQueens[i]; // Get the index of the queen that can capture
                        printf("%d: Queen at (%d, %d)\n", i + 1, 8 - pieces[idx].x, pieces[idx].y + 1); 
                        i = i + 1;
                    }

                    success = 0;
                    while (success == 0) {
                        printf("Enter choice: ");
                        if (scanf("%d", &choice) == 1 && choice >= 1 && choice <= moveCount) {
                            success = 1;  // Valid input!
                        } else {
                            printf("Invalid input. Please enter a number from 1 to %d.\n", moveCount);
                        }

                        while (getchar() != '\n') {
                        }
                    }

                    moveIndex = eligibleQueens[choice - 1]; // Assign the chosen queen's index from the eligibleQueens array
                } else {
                    moveIndex = eligibleQueens[0]; // Only one queen can move
                }

                // Move the queen
                pieces[moveIndex].x = x;
                pieces[moveIndex].y = y;
                printf("Queen moved to (%d, %d)!\n", 8 - x, y + 1);

                // Handle capture if any
                if (captured != -1) {
                    if (moveIndex > captured) {
                        moveIndex = moveIndex - 1; // Adjust index if the captured piece is before the queen in the array
                    }

                    j = captured;
                    while (j < *count - 1) {
                        pieces[j] = pieces[j + 1]; // Shift pieces
                        j = j + 1;
                    }

                    *count = *count - 1;

                    if (turn == 'W') {
                        *blackPieces = *blackPieces - 1;
                    } else {
                        *whitePieces = *whitePieces - 1;
                    }

                    printf("Queen captured an enemy piece!\n");
                }

                validMove = 1;
            }
        }
    }
}


/*  
evaluateKMove prompts the player for a valid target square, checks if it is occupied, and ensures the king moves correctly.  
The king can move only one square in any direction (horizontally, vertically, or diagonally).  
If the target square is occupied by an enemy piece, the king captures it.  
The function updates the board state and modifies piece counts accordingly.  

@param pieces - array of ChessPiece structures representing all pieces on the board  
@param count - pointer to the total number of pieces in play  
@param turn - character representing the current player's turn ('W' for White, 'B' for Black)  
@param whitePieces - pointer to the number of remaining white pieces  
@param blackPieces - pointer to the number of remaining black pieces  

@return void  

Pre-condition: The board setup is valid, and there is one king for the current player.  
*/
void evaluateKMove(ChessPiece pieces[], int *count, char turn, int *whitePieces, int *blackPieces) {
    int inputValid = 0;            // used to keep asking for row/column input until both are in range 1–8 (flag)
    int x, y;                      // input of chosen coordinates
    int dx, dy;                    // difference between king and target square
    int i, j;                      // loop counters
    int validMove = 0;             // stays 0 until a valid move is made (flag)
    int occupiedByOwnPiece = 0;    // check if own piece is on the target square (flag)
    int captured = -1;             // index of enemy piece at the target square, or -1 if none
    int moveIndex = -1;            // final chosen king to move

    // Repeatedly asks the player for a valid target square until a legal king move or capture is made.
    while (validMove == 0) {
        inputValid = 0;
        captured = -1;
        occupiedByOwnPiece = 0;

        while (inputValid == 0) {
            printf("Enter target row (1 - 8): ");
            if (scanf("%d", &x) == 1 && x >= 1 && x <= 8) {
                printf("Enter target column (1 - 8): ");
                if (scanf("%d", &y) == 1 && y >= 1 && y <= 8) {
                    inputValid = 1;
                } else {
                    printf("Invalid column! Must be a number between 1 and 8.\n");
                }
            } else {
                printf("Invalid row! Must be a number between 1 and 8.\n");
            }

            // Flush any remaining input
            while (getchar() != '\n') {}
        }

        // Converts chessboard input to array indexing (e.g., row 8 becomes index 0)
        x = 8 - x;
        y = y - 1;

        // Check if the target square is occupied and if it's an enemy or friendly piece
        occupiedByOwnPiece = checkSquare(pieces, *count, x, y, turn, &captured);

        if (occupiedByOwnPiece == 1) {
            printf("Target square is occupied by your own piece! Try again.\n");
        } else {
            // Find the king of the current player
            i = 0;
            moveIndex = -1;
            while (i < *count && moveIndex == -1) {
                if (pieces[i].type == 'K' && pieces[i].color == turn) {
                    dx = x - pieces[i].x;
                    dy = y - pieces[i].y;

                    // King moves only one square in any direction
                    if ((dx >= -1 && dx <= 1) && (dy >= -1 && dy <= 1)) {
                        moveIndex = i; // Found the king that can move to the target square
                    }
                }
                i = i + 1;
            }

            if (moveIndex == -1) {
                printf("Invalid king move! Try again.\n");
            } else {
                // Move the king
                pieces[moveIndex].x = x;
                pieces[moveIndex].y = y;
                printf("King moved to (%d, %d)!\n", 8 - x, y + 1);

                // Handle capture if any
                if (captured != -1) {
                    if (moveIndex > captured) {
                        moveIndex = moveIndex - 1;  // Adjust index if the captured piece is before the king in the array
                    }

                    j = captured;
                    while (j < *count - 1) {
                        pieces[j] = pieces[j + 1]; // Shift pieces
                        j = j + 1;
                    }

                    *count = *count - 1;

                    if (turn == 'W') {
                        *blackPieces = *blackPieces - 1;
                    } else {
                        *whitePieces = *whitePieces - 1;
                    }

                    printf("King captured an enemy piece!\n");
                }

                validMove = 1;
            }
        }
    }
}


/* checkSquare is a helper function that determines the 
status of a given board square in the chess game.

@param pieces - array of ChessPiece structures representing active pieces on the board
@param count - total number of pieces on the board
@param x - x-coordinate of the square to check
@param y - y-coordinate of the square to check
@param turn - current player's color ('W' for White, 'B' for Black)
@param captured - pointer to store the index of an enemy piece if present

@return 1 if the square is occupied by the player's own piece
@return 2 if the square is occupied by an enemy piece (captured index is updated)
@return 0 if the square is empty
Pre-condition: x and y are within the valid board range (0 to BOARD_SIZE - 1)

*/
int checkSquare(ChessPiece pieces[], int count, int x, int y, char turn, int *captured) {
    int j; // loop counter 
    *captured = -1; // no enemy piece captured
    // Check if the square is occupied by a piece
    for (j = 0; j < count; j++) {
        if (pieces[j].x == x && pieces[j].y == y) {
            if (pieces[j].color == turn) {
                return 1; // Occupied by own piece
            } else {
                *captured = j; // Store index of enemy piece
            }
        }
    }
    return 0; // Square is empty
}



/*
hasPiece is a helper function that checks if the current player has at least one piece of a 
specified type on the board.
 
 @param pieces[] – Array representing the chess pieces currently on the board
 @param count – Total number of active pieces in play
 @param turn – The current player's color ('W' for White, 'B' for Black)
 @param type – The type of piece to check (P, H, B, R, Q, K)
 
  @return (int) – Returns 1 if the player has at least one piece of the given type, otherwise returns 0
 */
int hasPiece(ChessPiece pieces[], int count, char turn, char type) {
    int i; // loop counter

    // Loop through all pieces to check for the specified type and color
    for (i = 0; i < count; i++) {
        if (pieces[i].color == turn && pieces[i].type == type) {
            return 1;  // Found at least one piece of this type
        }
    }
    return 0;  // No such piece
}


/* promotePawn allows a player to promote a pawn when it reaches the last rank.
   The player selects a new piece: Queen, Rook, Bishop, or Knight.
   
   @param pieces - an array of ChessPiece structures representing the board pieces
   @param index - the index of the pawn to be promoted in the pieces array

   Pre-condition: pieces[index] is a pawn that has reached the last rank.
*/
void promotePawn(ChessPiece pieces[], int index) {
    char choice;             // store the player's choice for promotion
    int validChoice = 0;     // check if the input is valid (flag)
    int scanResult;          // store the result of scanf

    // Check if the pawn has reached the last rank
    if ((pieces[index].color == 'W' && pieces[index].x == 0) ||
        (pieces[index].color == 'B' && pieces[index].x == 7)) {

        // Ask the player what piece they want to promote to
        printf("Pawn reached the end of the board! Promote to:\n");
        printf("Q - Queen\n");
        printf("R - Rook\n");
        printf("B - Bishop\n");
        printf("H - Knight\n"); // 'H' is used for Knight in this system

        // Input validation loop for promotion choice
        printf("Enter your choice: ");
        while (validChoice == 0) {
            scanResult = scanf(" %c", &choice);
            
            if (scanResult == 1) {
                choice = toupper(choice);
                if (choice == 'Q' || choice == 'R' || choice == 'B' || choice == 'H') {
                    validChoice = 1;
                } else {
                    printf("Invalid input. Choose Q, R, B, or H: ");
                }
            } else {
                printf("Invalid input. Choose Q, R, B, or H: ");
                while (getchar() != '\n');
            }
        }

        // Replace the pawn with the chosen piece
        pieces[index].type = choice;

        // Print confirmation of promotion
        if (choice == 'Q') {
            printf("Pawn promoted to Queen!\n");
        } else if (choice == 'R') {
            printf("Pawn promoted to Rook!\n");
        } else if (choice == 'B') {
            printf("Pawn promoted to Bishop!\n");
        } else {
            printf("Pawn promoted to Knight!\n");
        }
    }
}

/* hasValidMoves checks if a player has any valid moves for a given piece type.
   It scans through all pieces of the specified type and determines whether at least
   one move is possible based on the piece's movement rules.

   @param pieces - an array of ChessPiece structures representing the board pieces
   @param count - the total number of pieces currently on the board
   @param turn - the color of the player ('W' for White, 'B' for Black)
   @param type - the type of piece to check for valid moves ('P', 'H', 'B', 'R', 'Q', 'K')
   @return 1 if at least one valid move is found, 0 otherwise

   Pre-condition: The pieces array contains valid chess pieces with correct positions.
*/
int hasValidMoves(ChessPiece pieces[], int count, char turn, char type) {
    int i, k, m, c, d;    // loop counter
    int x, y;             // current piece position
    int tx, ty;           // temporary position variables
    int stepX, stepY;     // step direction for sliding pieces
    int blocked;          // 1 if path is blocked, 0 if free
    int pieceFound;       // 1 if a piece exists on a square, 0 if empty
    int dir;              // forward direction for pawn (-1 for W, +1 for B)
    int forward;          // pawn's next row if it moves one step
    int hasMove = 0;      // 1 if a valid move is found, 0 otherwise
    int Directnum;        // number of movement directions for the piece
    int Directions[8][2]; // local array for move patterns
    int captureY[2];      // left and right diagonal moves for pawn capture
    int targetX, targetY; // target coordinates for pawn capture

    // Movement patterns for different pieces
    int knightMoves[8][2] = {
        {-2, -1}, {-2, 1}, {-1, -2}, {-1, 2},
        {1, -2},  {1, 2},  {2, -1},  {2, 1}
    }; 

    int bishopMoves[4][2] = {
        {-1, -1}, {-1, 1}, {1, -1}, {1, 1}
    };

    int rookMoves[4][2] = {
        {-1, 0}, {1, 0}, {0, -1}, {0, 1}
    };

    int queenMoves[8][2] = {
        {-1, 0}, {1, 0}, {0, -1}, {0, 1},
        {-1, -1}, {-1, 1}, {1, -1}, {1, 1}
    };

    int kingMoves[8][2] = {
        {-1, -1}, {-1, 0}, {-1, 1},
        {0, -1},          {0, 1},
        {1, -1},  {1, 0}, {1, 1}
    };

    for (i = 0; i < count; i++) {
        if (pieces[i].color == turn && pieces[i].type == type) {
            x = pieces[i].x;
            y = pieces[i].y;

            // Pawn Movement
            if (type == 'P') {
                if (turn == 'W') {
                    dir = -1; // White pawns move upward (decreasing x)  
                } else {
                    dir = 1; // Black pawns move downward (increasing x)  
                }
            
                // Check forward move (unchanged)
                forward = x + dir;
                if (forward >= 0 && forward < 8) {
                    blocked = 0;
                    for (k = 0; k < count; k++) {
                        if (pieces[k].x == forward && pieces[k].y == y) {
                            blocked = 1;
                        }
                    }
                    if (!blocked) {
                        hasMove = 1;
                    }
                }
            
                // Check diagnoals 
                captureY[0] = y - 1; // left diagonal
                captureY[1] = y + 1; // right diagonal
                for (c = 0; c < 2; c++) {
                    targetX = x + dir; // forward move
                    targetY = captureY[c]; // left or right diagonal
                    
                    // Check if diagonal square is on the board
                    if (targetX >= 0 && targetX < 8 && targetY >= 0 && targetY < 8) {
                        // Check if an enemy piece exists there
                        for (k = 0; k < count; k++) {
                            if (pieces[k].x == targetX && 
                                pieces[k].y == targetY && 
                                pieces[k].color != turn) {  // Enemy piece
                                hasMove = 1; // Capture is possible
                            }
                        }
                    }
                }
            }

            // Knight Movement
            if (type == 'H') {
                // Check all possible knight moves
                for (m = 0; m < 8; m++) {
                    tx = x + knightMoves[m][0]; // Calculate target x-coordinate
                    ty = y + knightMoves[m][1]; // Calculate target y-coordinate

                    // Check if the target square is within the board and not occupied by a piece of the same color
                    if (tx >= 0 && tx < 8 && ty >= 0 && ty < 8) {
                        blocked = 0;
                        // Check if the target square is occupied by a piece of the same color
                        for (k = 0; k < count; k++) {
                            if (pieces[k].x == tx && pieces[k].y == ty && pieces[k].color == turn) {
                                blocked = 1;
                            }
                        }
                        // If the target square is empty or occupied by an enemy piece, a valid move is possible
                        if (!blocked) {
                            hasMove = 1;
                        }
                    }
                }
            }

            // Bishop, Rook, Queen Movement
            if (type == 'B' || type == 'R' || type == 'Q') {
                if (type == 'Q') {  
                    Directnum = 8;  // Queen moves in 8 directions (rook + bishop)  
                } else {  
                    Directnum = 4;  // Bishop and Rook move in 4 directions each  
                }  
                
                // Set up movement patterns for Bishop, Rook, and Queen
                for (d = 0; d < Directnum; d++) {
                    if (type == 'B') {
                        Directions[d][0] = bishopMoves[d][0]; // Bishop moves diagonally
                        Directions[d][1] = bishopMoves[d][1]; 
                    } else if (type == 'R') {
                        Directions[d][0] = rookMoves[d][0]; // Rook moves straight
                        Directions[d][1] = rookMoves[d][1];
                    } else {
                        Directions[d][0] = queenMoves[d][0]; // Queen moves in all directions
                        Directions[d][1] = queenMoves[d][1];
                    }
                }

                // Check all possible moves in the defined directions
                for (d = 0; d < Directnum; d++) {
                    stepX = Directions[d][0]; // X direction
                    stepY = Directions[d][1]; // Y direction
                    tx = x + stepX; // Target x-coordinate
                    ty = y + stepY; // Target y-coordinate
                    blocked = 0; 

                    // Loop through the squares in the current direction until blocked or out of bounds
                    while (tx >= 0 && tx < 8 && ty >= 0 && ty < 8 && !blocked) {
                        pieceFound = 0;

                        // Check if the target square is occupied by a piece
                        for (k = 0; k < count; k++) {
                            if (pieces[k].x == tx && pieces[k].y == ty) {
                                pieceFound = 1;
                                if (pieces[k].color == turn) {
                                    blocked = 1;
                                }
                            }
                        }

                        // If the square is empty or occupied by an enemy piece, a valid move is possible
                        if (!pieceFound || !blocked) {
                            hasMove = 1;
                        }
                        // If the square is occupied by a piece of the same color, stop checking further in this direction
                        if (pieceFound) {
                            blocked = 1;
                        }

                        tx += stepX; // Move to the next square in the current direction
                        ty += stepY; // Move to the next square in the current direction
                    }
                }
            }

            // King Movement
            if (type == 'K') {
                for (m = 0; m < 8; m++) {
                    tx = x + kingMoves[m][0]; // Calculate target x-coordinate 
                    ty = y + kingMoves[m][1]; // Calculate target y-coordinate

                    // Check if the target square is within the board and not occupied by a piece of the same color
                    if (tx >= 0 && tx < 8 && ty >= 0 && ty < 8) {
                        blocked = 0;
                        // Check if the target square is occupied by a piece of the same color
                        for (k = 0; k < count; k++) {
                            if (pieces[k].x == tx && pieces[k].y == ty && pieces[k].color == turn) {
                                blocked = 1;
                            }
                        }
                        // If the target square is empty or occupied by an enemy piece, a valid move is possible
                        if (!blocked) {
                            hasMove = 1;
                        }
                    }
                }
            }
        }
    }

    return hasMove;
}


/* playerHasAnyMoves checks if a player has any valid moves available.
   It loops through all piece types and determines if at least one piece 
   of the given type has a valid move.

   @param pieces - an array of ChessPiece structures representing the board pieces
   @param count - the total number of pieces currently on the board
   @param turn - the color of the player ('W' for White, 'B' for Black)
   @return 1 if at least one valid move exists, 0 otherwise

   Pre-condition: The pieces array contains valid chess pieces with correct positions.
*/
int playerHasAnyMoves(ChessPiece pieces[], int count, char turn) {
    char types[] = {'P', 'H', 'B', 'R', 'Q', 'K'}; // Array of piece types
    int i = 0; // loop counter
    int hasMoves = 0; // Flag to track if a valid move exists

    // Loop through all piece types to check if any can move
    while (i < 6 && hasMoves == 0) {
        if (hasPiece(pieces, count, turn, types[i])) {
            if (hasValidMoves(pieces, count, turn, types[i])) {
                hasMoves = 1; // Found a valid move, set flag
            }
        }
        i = i + 1;
    }

    return hasMoves; // Return 1 if moves exist, otherwise 0
}


/* areNamesEqual compares two strings.
   It checks if the two given names are identical regardless of letter case.

   @param name1 - the first string to compare
   @param name2 - the second string to compare
   @return 1 if the strings are equal (ignoring case), 0 otherwise

   Pre-condition: name1 and name2 are valid null-terminated strings.
*/
int areNamesEqual(char *name1, char *name2) {
    int i; // loop counter
    char a, b; // temporary variables for character comparison

    i = 0;

    // Keep looping as long as both strings have characters left to compare
    while (name1[i] != '\0' && name2[i] != '\0') {
        a = tolower(name1[i]);
        b = tolower(name2[i]);

        if (a != b) {
            return 0; // Strings are different
        }
        i = i + 1;
    }

    // After the loop, both strings must end at the same time
    if (name1[i] == '\0' && name2[i] == '\0') {
        return 1; // Strings are equal
    }

    return 0; // One string is longer than the other
}


/* capitalizeName capitalizes a string while having the other letters lowercased.

   @param name - the string to be modified
   @return void (modifies the string in place)

   Pre-condition: name is a non-null, valid null-terminated string.
*/
void capitalizeName(char *name) {
    int i = 0;

    // Only modify if the string is not empty
    if (name[0] != '\0') {
        // Capitalize first letter
        name[0] = toupper(name[0]);

        // Make remaining letters lowercase
        i = 1;
        while (name[i] != '\0') {
            name[i] = tolower(name[i]);
            i = i + 1;
        }
    }
}


/* updateScoreFile updates the scores of players in a file based on the game result.
   It reads the existing scores, updates or adds players, sorts them in descending order,
   and writes the updated scores back to the file.

   @param player1 - name of the first player
   @param player2 - name of the second player
   @param winner - name of the winner (NULL if the game was a draw)
   @param result - game result (1 if there is a winner, otherwise 0 for a draw)
   @return none

   Pre-condition: player1, player2, and winner (if not NULL) are valid null-terminated strings.
*/
void updateScoreFile(char *player1, char *player2, char *winner, int result) {
    FILE *fp;
    PlayerScores players[MAX_PLAYERS];   // array to store player names and scores
    int count = 0;                       // number of players in file
    int i, j;                            // loop counters
    int foundPlayer1 = 0;                // check if player1 exists (flag)
    int foundPlayer2 = 0;                // check if player2 exists (flag)
    int player1Score = 0;                // score increment for player1
    int player2Score = 0;                // score increment for player2
    char tempName[MAX_NAME_LEN];         // temporary variable for sorting
    int tempScore;                       // temporary variable for sorting

    // Determine score increment based on game result
    if (result == 1 && winner != NULL) { // Win condition
        if (areNamesEqual(player1, winner)) {
            player1Score = 1;
        } else {
            player2Score = 1;
        }
    }

    // Read existing scores from file
    fp = fopen("scores.txt", "r");
    if (fp != NULL) {
        while (count < MAX_PLAYERS && fscanf(fp, "%s %d", players[count].name, &players[count].score) == 2) {
            count = count + 1;
        }
        fclose(fp);
    }

    // Update or add player scores
    i = 0;
    while (i < count) {
        // Check if player1 or player2 exists in the file
        if (areNamesEqual(players[i].name, player1)) {
            players[i].score = players[i].score + player1Score; // Update score
            foundPlayer1 = 1; // Mark player1 as found
        }

        // Check if player2 exists in the file
        if (areNamesEqual(players[i].name, player2)) {
            players[i].score = players[i].score + player2Score; // Update score
            foundPlayer2 = 1; // Mark player2 as found
        }
        i = i + 1;
    }

    // Add new players if they were not found
    if (foundPlayer1 == 0 && count < MAX_PLAYERS) {
        strcpy(players[count].name, player1); // Add player1 to the list
        players[count].score = player1Score; // Set score
        count = count + 1; // Increase player count
    }
    if (foundPlayer2 == 0 && count < MAX_PLAYERS) {
        strcpy(players[count].name, player2); // Add player2 to the list
        players[count].score = player2Score; // Set score
        count = count + 1; // Increase player count
    }

    // Sort scores in descending order using Bubble Sort
    i = 0;
    while (i < count - 1) {
        j = 0;
        while (j < count - i - 1) {
            // Sort by highest score; if same, sort alphabetically
            if (players[j].score < players[j + 1].score) {
                tempScore = players[j].score;
                players[j].score = players[j + 1].score;
                players[j + 1].score = tempScore;

                strcpy(tempName, players[j].name);
                strcpy(players[j].name, players[j + 1].name);
                strcpy(players[j + 1].name, tempName);
            }
            j = j + 1;
        }
        i = i + 1;
    }

    // Write updated and sorted scores back to file
    fp = fopen("scores.txt", "w");
    if (fp != NULL) {
        for (i = 0; i < count; i++) {
            fprintf(fp, "%s\n%d\n\n", players[i].name, players[i].score); // write name and score
        }
        fclose(fp);
    } else {
        printf("Error: Unable to write to scores.txt\n");
    }
}


/* displayHighScores reads, merges duplicate players, sorts, and displays high scores.
   It ensures case-insensitive name comparison and updates the file.

   @return void (displays scores and updates "scores.txt")

   Pre-condition: "scores.txt" may or may not exist.
*/
void displayHighScores() {
    FILE *fp;
    char names[MAX_PLAYERS][MAX_NAME_LEN];  // store player names
    int scores[MAX_PLAYERS];                // store scores as integers
    int count = 0;                          // number of valid players read
    int i, j, k;                            // loop counters
    int choice;                             // menu input
    char tempName[MAX_NAME_LEN];            // temp for name swap in sorting
    int tempScore;                          // temp for score swap in sorting
    int fileExists = 1;                     // check if file exists (flag)

    // Open the high scores file for reading
    fp = fopen("scores.txt", "r");
    if (fp == NULL) {
        fileExists = 0;
    } else {
        // Read all players and their scores
        while (count < MAX_PLAYERS && fscanf(fp, "%s %d", names[count], &scores[count]) == 2) {
            count = count + 1; // Increase count of valid entries
        }
        fclose(fp);
    }

    // If file doesn't exist or is empty, return to main menu
    if (fileExists == 0 || count == 0) {
        printf("No high scores available.\n");
        printf("Going back to Main Menu...\n");
        MainMenu();
    } else {
        // Merge duplicate names (case-insensitive), summing their scores
        i = 0;
        while (i < count) {
            j = i + 1;
            while (j < count) {
                if (areNamesEqual(names[i], names[j]) == 1) { // If names match
                    scores[i] = scores[i] + scores[j]; // Merge scores

                    // Shift remaining names and scores left
                    for (k = j; k < count - 1; k++) {
                        strcpy(names[k], names[k + 1]); 
                        scores[k] = scores[k + 1];
                    }
                    count = count - 1;  // Reduce player count
                } else {
                    j = j + 1; 
                }
            }
            i = i + 1;
        }

        // Sort scores in descending order (Bubble Sort)
        i = 0;
        while (i < count - 1) {
            j = 0;
            while (j < count - i - 1) {
                // Sort by highest score; if same, sort alphabetically
                if (scores[j] < scores[j + 1] ||
                    (scores[j] == scores[j + 1] && strcmp(names[j], names[j + 1]) > 0)) {
                    // Swap scores
                    tempScore = scores[j];
                    scores[j] = scores[j + 1];
                    scores[j + 1] = tempScore;

                    // Swap names
                    strcpy(tempName, names[j]);
                    strcpy(names[j], names[j + 1]);
                    strcpy(names[j + 1], tempName);
                }
                j = j + 1;
            }
            i = i + 1;
        }

        // Capitalize player names before saving
        i = 0;
        while (i < count) {
            capitalizeName(names[i]);
            i = i + 1;
        }

        // Rewrite the sorted and cleaned-up high scores list back to file
        fp = fopen("scores.txt", "w");
        if (fp != NULL) {
            i = 0;
            while (i < count) {
                fprintf(fp, "%s\n%d\n\n", names[i], scores[i]);
                i = i + 1;
            }
            fclose(fp);
        } else {
            printf("Warning: Could not update scores.txt.\n");
        }

        // Display high scores on the console
        printf("\n--- HIGH SCORES ---\n");
        i = 0;
        while (i < count) {
            printf("%s: %d\n", names[i], scores[i]);
            i = i + 1;
        }
    }

    // Prompt user to return to Play Menu
    choice = 0;
    while (choice != 1) {
        printf("\nExit? (Press 1)\n");
        printf("Enter choice: ");
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n'); 
            printf("Invalid input. Please enter 1.\n");
        } else if (choice != 1) {
            printf("Invalid input. Please enter 1.\n");
        }
    }
    
}


/* LocalScores updates the local game scores for two players and displays them.  
   It initializes player names if not already set, and updates scores based on  
   the result of the game. If a winner is declared (result == 1), they gain  
   a point. 

   @param local - an array of PlayerScores storing names and scores of both players  
   @param player1 - name of the first player  
   @param player2 - name of the second player  
   @param winner - name of the winner (NULL if draw)  
   @param result - game outcome (1 for a win, 2 for a draw)  

   Pre-condition: local must contain two PlayerScores structures, each representing a player.
*/
void LocalScores(PlayerScores local[], char *player1, char *player2, char *winner, int result) {
    int player1Score = 0; // score increment for player1
    int player2Score = 0; // score increment for player2

    // Check if the local scores array is uninitialized (both player names are empty strings)
    if (local[0].name[0] == '\0' && local[1].name[0] == '\0') {
        
        strcpy(local[0].name, player1); // Initialize the first player's name
        strcpy(local[1].name, player2); // Initialize the second player's name
        
        local[0].score = 0; // Set the first player's score to 0
        local[1].score = 0; // Set the second player's score to 0
    }

    if (result == 1) {
        if (winner != NULL) {
            // Check if the winner is player1 or player2
            if (areNamesEqual(local[0].name, winner) == 1) {
                player1Score = 1;
            } else if (areNamesEqual(local[1].name, winner) == 1) {
                player2Score = 1;
            }
        }
    } 

    local[0].score = local[0].score + player1Score; // Update player1's score
    local[1].score = local[1].score + player2Score; // Update player2's score

    printf("\nScore:\n");
    printf("%s: %d\n", local[0].name, local[0].score);
    printf("%s: %d\n", local[1].name, local[1].score);
}


/* saveGame saves the current game state to a file. Player provides filename.
   It writes each piece's type, color, and position to the file.  

   @param pieces - an array of ChessPiece structures representing the current game state  
   @param count - a pointer to the number of pieces in the game  
)  

   Pre-condition: pieces must be initialized, and count must point to a valid integer.  
*/
void saveGame(ChessPiece pieces[], int *count) {
    FILE *fp;
    char filename[MAX_NAME_LEN];    // array to store the filename entered by the user
    int i;                          // loop counter
    char color[10];                 // temporary string to store the piece's color ("white" or "black")
;  

    printf("Enter filename to save: ");
    scanf(" %99s", filename);

    fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("Error: Could not open file for saving\n");
        return; // Exit early to avoid using an invalid file pointer
    }

    // Write each piece's type, color, and position to the file
    for (i = 0; i < *count; i++) {
        if (pieces[i].color == 'W') {
            strcpy(color, "white");
        } else {
            strcpy(color, "black");
        }

        fprintf(fp, "%c\n%s\n%d\n%d\n\n", 
                pieces[i].type,
                color,
                pieces[i].x,  
                pieces[i].y); 
    }

    fclose(fp);
    printf("Game saved successfully as %s.\n", filename);
}



/* loadGame loads a saved game from a specified file.  
   It reads each piece’s type, color, and position from the file,  
   validates them, and updates the game board accordingly.  

   @param pieces - an array of ChessPiece structures to store the loaded game  
   @param count - a pointer to an integer that stores the number of loaded pieces  

   Pre-condition: pieces must be initialized, count must point to a valid integer,  
   and the file must be formatted correctly with valid piece data.  
*/

void loadGame(ChessPiece pieces[], int *count) {
    FILE *fp;
    char filename[MAX_NAME_LEN]; // array to store the filename entered by the user
    char type;                   // character representing the type of the chess piece
    char color[10];              // string to store the color of the piece ("white" or "black")
    int x, y;                    // integer variables for the piece's board position.
    int i = 0;                   // loop counter 
    int error = 0;               // track errors during file reading (flag)

    // Prompt user for filename to load
    printf("Enter filename to load: ");
    scanf(" %99s", filename);

    fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Error: Could not open file %s\n", filename);
        error = 1;
    }

    if (error == 0) {
        // Read pieces from the file until EOF or maximum pieces reached
        while (i < MAX_PIECES && fscanf(fp, " %c", &type) == 1) {
            // Read color and position of the piece
            if (fscanf(fp, " %9s", color) != 1) {
                error = 1;
            }

            // Read x coordinate
            if (fscanf(fp, " %d", &x) != 1) {
                error = 1;
            }

            // Read y coordinate
            if (fscanf(fp, " %d", &y) != 1) {
                error = 1;
            }

            if (error == 0) {
                // Validate the piece type and ensure its position is within the board boundaries
                if (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE) {
                    pieces[i].type = type; // Store the piece type
            
                    // Validate the color string and assign the corresponding character
                    if (strcmp(color, "white") == 0) {
                        pieces[i].color = 'W'; // Assign white color
            
                    } else if (strcmp(color, "black") == 0) {
                        pieces[i].color = 'B'; // Assign black color
                    } else {
                        printf("Warning: Invalid color %s for piece %c. Skipping.\n", color, type);
                    }
            
                    // Store the valid piece position
                    pieces[i].x = x;
                    pieces[i].y = y;
                    
                    // Increment the piece count after successfully adding the piece
                    i = i + 1;
                } else {
                    // Display a warning if the piece position is outside the board boundaries
                    printf("Warning: Invalid position (%d, %d) for piece %c. Skipping.\n", x, y, type);
                }
            }
        }
    }

    if (fp != NULL) {
        fclose(fp);
    }

    // Successful Loading
    if (error == 0) {
        *count = i;
        printf("Game loaded successfully from %s.\n", filename);
        UpdateBoard(pieces, *count); // Update the board with loaded pieces
    }
}

/* main serves as the entry point for the chess game and the whole program.  
   It displays a welcome message and calls the main menu function.  

   @return 0 to indicate successful program execution.  

   Pre-condition: MainMenu() must be implemented and handle user interactions.  
*/
int main() {
    printf("WELCOME TO AN UGLY CHESS GAME WHERE CHECKS OR CHECKMATES DON'T EXIST!\n");
    MainMenu(); // Call the main menu function to start the game
    return 0;
}
