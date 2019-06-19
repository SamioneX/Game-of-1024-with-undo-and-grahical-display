#include <SFML/Graphics.hpp> // Needed to access all the SFML graphics libraries
#include <iostream>          // Since we are using multiple libraries, now use std::
                             // in front of every std::cin, std::cout, std::endl, std::setw, and string 
#include <iomanip>           // used for setting output field size using std::setw
#include <cstdio>            // For sprintf, "printing" to a string
#include <cstring>           // For c-string functions such as strlen()  
#include <chrono>            // Used in pausing for some milliseconds using sleep_for(...)
#include <thread>            // Used in pausing for some milliseconds using sleep_for(...)

const int WindowXSize = 800;
const int WindowYSize = 1000;
const int MaxBoardSize = 12;  // Max number of squares per side
const int MaxTileStartValue = 1024;   // Max tile value to start out on a 4x4 board


//---------------------------------------------------------------------------------------
class Square {
	public:
		// Default Constructor 
		Square()
		{
			size = 0;
			xPosition = 0;
			yPosition = 0;
			color = sf::Color::Black;
			isVisible = false;
			isCaptured = false;
			text = "";		
		}
	
		// Fully-qualified constructor, used to set all fields
		Square( int theSize, int theXPosition, int theYPosition, 
			     const sf::Color &theColor, bool theVisibility, std::string theText)
		{
			// Set the class fields
			size = theSize;
			xPosition = theXPosition;
			yPosition = theYPosition;
			color = theColor;
			isVisible = theVisibility;
			isCaptured = false;   // By default squares have not been captured
			text = theText;
			// Use the values to set the display characteristics of theSquare
			theSquare.setSize( sf::Vector2f(theSize, theSize));
			theSquare.setPosition( theXPosition, theYPosition);   // Set the position of the square
			theSquare.setFillColor( theColor);
		}
			
		// Get (accessor) functions
		sf::RectangleShape getTheSquare() { return theSquare; }
		int getSize() { return size; }
		int getXPosition() { return xPosition; }
		int getYPosition() { return yPosition; }
		sf::Color& getColor() { return color; }
		bool getIsVisible() { return isVisible; }
		bool getIsCaptured() { return isCaptured; }
		std::string getText() { return text; }
	
		// Set (mutator) functions
		void setSize( int theSize) { 
			size = theSize; 
			theSquare.setSize( sf::Vector2f(theSize, theSize));
		}
		void setXPosition( int theXPosition) { 
			xPosition = theXPosition; 
			theSquare.setPosition( theXPosition, yPosition);   // Set the position of the square
		}
		void setYPosition( int theYPosition) { 
			yPosition = theYPosition; 
			theSquare.setPosition( xPosition, theYPosition);   // Set the position of the square
		}
		void setColor( sf::Color & theColor) { 
			color = theColor; 
			theSquare.setFillColor( theColor);    // Also update the color on the square itself
		}
		void setColor( int R, int G, int B) {
			sf::Color theNewColor( R, G, B);
			color = theNewColor;
			theSquare.setFillColor( theNewColor);
		}
		void setVisibility( bool theVisibility) { isVisible = theVisibility; }
		void setIsCaptured( bool isCaptured) { this->isCaptured = isCaptured; }
		void setText( std::string theText) { text = theText; }

		// Utility functions
		void displayText( sf::RenderWindow *pWindow, sf::Font theFont, sf::Color theColor, int textSize);
	
	private:
		int size;
		int xPosition;
		int yPosition;
		sf::Color color;
		bool isVisible;
		bool isCaptured;   // Indicates whether or not it is part of the captured area
		std::string text;
		sf::RectangleShape theSquare;

}; //end class Square


//---------------------------------------------------------------------------------------
// Square class utility function to create a sf::Text object to store and display text 
// associated with this Square.
// 
// Assuming we display output in sf::RenderWindow window(...), then call this function using: 
//    aSquare.displayTest( &window);
// or when using an array of Square pointers declared as:  Square *squaresArray[ 4];
// then call it using:  squaresArray[i]->displayText( &window);
void Square::displayText( 
		sf::RenderWindow *pWindow,   // The window into which we draw everything
		sf::Font theFont,            // Font to be used in displaying text   
		sf::Color theColor,          // Color of the font
		int textSize)                // Size of the text to be displayed
{	
	// Create a sf::Text object to draw the text, using a sf::Text constructor
	sf::Text theText( text,        // text is a class data member
					  theFont,     // font from a font file, passed as a parameter
					  textSize);   // this is the size of text to be displayed

	// Text color is the designated one, unless the background is Yellow, in which case the text
	// color gets changed to blue so we can see it, since we can't see white-on-yellow very well
	if( this->getColor() == sf::Color::Yellow) {
		theColor = sf::Color::Blue;
	}
	theText.setColor( theColor);

	// Place text in the corresponding square, centered in both x (horizontally) and y (vertically)
	// For horizontal center, find the center of the square and subtract half the width of the text 
	int theXPosition = xPosition + (size / 2) - ((strlen(text.c_str()) * theText.getCharacterSize()) / 2);
	// For the vertical center, from the top of the square go down the amount: (square size - text size) / 2
	int theYPosition = yPosition + (size - theText.getCharacterSize()) / 2;
	// Use an additional offset to get it centered
	int offset = 5;
	theText.setPosition( theXPosition + offset, theYPosition - offset);

	// Finally draw the Text object in the RenderWindow
	pWindow->draw( theText);
}//end displayText


//---------------------------------------------------------------------------------------
// Initialize the font
void initializeFont( sf::Font &theFont)
{
	// Create the global font object from the font file
	if (!theFont.loadFromFile("arial.ttf"))
	{
		std::cout << "Unable to load font. " << std::endl;
		exit( -1);
	}	
}//end initializeFont

//--------------------------------------------------------------------
// Function to copy a board into another
void copyBoard(
       int* previousBoard, // destination for board copy
       int* board,         // board from which copy will be made
       int squaresPerSide)       // size of the board
{
    for( int row=0; row<squaresPerSide; row++) {
        for( int col=0; col<squaresPerSide; col++ ) {
            int current = row*squaresPerSide + col;  // 1-d index corresponding to row & col
            previousBoard[ current] = board[ current];
        }
    }
}//end copyBoard()

//----------------------------------
class node {
    public:
    int* board;
    int move;
    int score;
    node* next;
    node() {           //constructor
        board = NULL;
        move = 1;
        score = 0;
        next = NULL;
    }
};//end node class

//--------------------------------------------------------------------
class LinkedList {
    private:
    node* head;
    
    public:
    LinkedList() {head = NULL;}   // constructor
    
    // Function to prepend a node to a linked list
    void push(int* theBoard, int squaresPerSide, int theMove, int theScore) {
        node* pNew = new node;      //create a new node
        pNew->board = new int[squaresPerSide*squaresPerSide];   //allocate memory for the board
        copyBoard(pNew->board, theBoard, squaresPerSide);    //copy theBoard into the node's board
        pNew->move = theMove;  //set move
        pNew->score = theScore;  //set score
        pNew->next = head;     //let next of the new node point to the head
        head = pNew;           // make head pointer point to the new node
    }
    // Function to delete a node at beginning of a linked list
    void pop() {
        node* temp = head;     
        head = temp->next;       //advance the head pointer to the next
        delete [] temp->board;   //delete memory allocated for board
        delete temp;             //delete node at head of linked list
    }
    // Function to return node at head of the linked list
    node* top() {return head;}
    
    // Function to print the moveNumbers in the linked list
    void printMoves() {
        std::cout << "        List: ";
        node* temp = head;
        while(temp->next != NULL) {
            std::cout << temp->move << "->";
            temp = temp->next;
        }
        std::cout << temp->move << std::endl << std::endl;
    }
    
    // Function to erase the linked list
    void clear() {
        node* current = head, *next;
        
        //iterate through the linked list and delete each node
        while (current != NULL) {
            next = current->next;        //get the next node
            delete [] current->board;    //delete memory allocated for board
            delete current;              //delete the node
            current = next;
        }
        head = NULL;
    }
};//end LinkedList Class

//--------------------------------------------------------------------
// Display Instructions
void displayInstructions()
{
    std::cout << "Welcome to 1024. \n"
			  << "  \n"
			  << "For each move enter a direction as a letter key, as follows: \n"
			  << "    W    \n"
			  << "  A S D  \n"
			  << "where A=left,W=up, D=right and S=down. \n"
			  << "  \n"
			  << "After a move, when two identical valued tiles come together they    \n"
			  << "join to become a new single tile with the value of the sum of the   \n"
			  << "two originals. This value gets added to the score.  On each move    \n"
			  << "one new randomly chosen value of 2 or 4 is placed in a random open  \n"
			  << "square.  User input of x exits the game.                            \n"
			  << "  \n";
}//end displayInstructions()


//--------------------------------------------------------------------
// Place a randomly selected 2 or 4 into a random open square on
// the board.
void placeRandomPiece(int* board, int squaresPerSide)
{
    // Randomly choose a piece to be placed (2 or 4)
    int pieceToPlace = 2;
    if( rand()%2 == 1) {
        pieceToPlace = 4;
    }
    
    // Find an unoccupied square that currently has a 0
    int index;
    do {
        index = rand() % (squaresPerSide*squaresPerSide);
    }while( board[ index] != 0);
    
    // board at position index is blank, so place piece there
    board[ index] = pieceToPlace;
}//end placeRandomPiece()

//--------------------------------------------------------------------
// Prompt for and get board size, dynamically allocate space for the
// board, initialize the board and set the max tile value that
// corresponds to the board size.
void initializeBoards(
         int* &board,           // Playing board
         Square* &squaresArray, // Graphical board
         int &squaresPerSide,   // size of the board, entered by user
         int &maxTileValue)
{
    //Allocate memory for board and squaresArray
    board = new int[squaresPerSide*squaresPerSide];
    squaresArray = new Square[squaresPerSide*squaresPerSide];
    
    // First initialize the array of int values used to represent the Ascii board
    for( int j=0; j<squaresPerSide*squaresPerSide; j++) {
        board[j] = 0;
    }
    
    // Calculate and display game ending value
    maxTileValue = MaxTileStartValue;        // Reset the value, in case we resize the board to be smaller
    for( int i=4; i<squaresPerSide; i++) {
        maxTileValue = maxTileValue * 2;   // double for each additional board dimension > 4
    }
    std::cout << "Game ends when you reach " << maxTileValue << "." << std::endl;
    
    // Set two random pieces to start game
    placeRandomPiece( board, squaresPerSide);
    placeRandomPiece( board, squaresPerSide);
}//end initializeBoards()

void updateSquareBoard(Square* &squaresArray, int arraySize, int* board, sf::Font &font, sf::RenderWindow &window) {
   
    for( int i=0; i<arraySize; i++) {
        for (int j = 0; j < arraySize; j++) {
            // Allocate each square individually.
            // Store a string in each square which contains its number, in order of creation
            char name[25];
            // Squares with a 0 value should not have a number displayed
            if( board[i*arraySize + j] == 0) {
                strcpy( name, "");   // "print" a blank text string
            }
            else {
               sprintf( name, "%d", board[i*arraySize + j]);   // "print" the square creation order number into a string to be stored in the square
            }
            // Set each array pointer have the address of a new Square, created with a Square constructor
            squaresArray[i*arraySize + j] = Square(50, 50 * j + j*10, 50 * i + i*10, sf::Color::Red, true, name);
            // Draw the square
            window.draw( squaresArray[i*arraySize + j].getTheSquare());
            // Draw the text associated with the Square, in the window with the indicated color and text size
            int red = 255, green = 255, blue = 255;
            squaresArray[i*arraySize + j].displayText( &window, font, sf::Color(red, green, blue), 30);
        }
	}
}

//--------------------------------------------------------------------
// Display the text-based Board
void displayAsciiBoard( int* board, int squaresPerSide, int score, LinkedList &list)
{
    std::cout << "\n"
         << "Score: " << score << std::endl;
    for( int row=0; row<squaresPerSide; row++) {
        std::cout << "   ";
        for( int col=0; col<squaresPerSide; col++ ) {
            int current = row*squaresPerSide + col;  // 1-d index corresponding to row & col
            std::cout << std::setw( 6);    // set output field size to 6 (Requires #include <iomanip> )
            // display '.' if board value is 0
            if( board[ current] == 0) {
               std::cout << '.';
            }
            else {
               std::cout << board[ current];
            }
        }
        std::cout << "\n\n";
    }
    list.printMoves();
}//end displayBoard()


//--------------------------------------------------------------------
// See if board changed this turn. If not, no additional piece
// is randomly added and move number does not increment in main().
// Returns true if boards are different, false otherwise.
bool boardChangedThisTurn( int* previousBoard, int* board, int squaresPerSide)
{
    // Compare element by element.  If one is found that is different
    // then return true, as board was changed.
    for( int row=0; row<squaresPerSide; row++) {
        for( int col=0; col<squaresPerSide; col++ ) {
            int current = row*squaresPerSide + col;  // 1-d index corresponding to row & col
            if( previousBoard[ current] != board[ current]) {
                return true;
            }
        }
    }
    
    return false;  // No board difference was found
}//end boardChangedThisTurn(...)


// While the 4 functions below (slideLeft(), slideRight(), slideUp(), slideDown() ) could
// be all combined into a single function, that single function would be difficult to
// understand, so these 4 functions are left separate.


//--------------------------------------------------------------------
// Slide all tiles left, combining matching values, updating the score
void slideLeft( int* board, int squaresPerSide, int &score)
{
    // handle each row separately
    for( int row=0; row<squaresPerSide; row++) {
        // set index limit for this row to be index of left-most tile on this row
        int limit = row * squaresPerSide;
        
        // Start from the second column and process each element from left to right
        for( int col=1; col<squaresPerSide; col++) {
            
            // get 1-d array index based on row and col
            int current = row * squaresPerSide + col;

            // slide current piece over as far left as possible
            while( current > limit && board[ current-1] == 0) {
                board[ current-1] = board[ current];
                board[ current] = 0;
                current--;
            }
            
            // Combine it with left neighbor if values are the same and non zero.
            // The additional check for (current > limit) ensures a tile can be combined
            // at most once on a move, since limit is moved right every time a combination is made.
            // This ensures a row of:  2 2 4 4   ends up correctly as:  4 8 0 0   and not:  8 4 0 0
            if( (current > limit) && (board[ current-1] == board[ current]) && (board[ current] != 0) ) {
                board[ current-1] = board[ current-1] + board[ current];
                board[ current] = 0;
                limit = current;           // Reset row index limit, to prevent combining a piece more than once
                score += board[ current-1];  // Update score
            }
            
        }//end for( int col...
    }//end for( int row...
    
}//end slideLeft()


//--------------------------------------------------------------------
// Slide all tiles right, combining matching values, updating the score
void slideRight( int* board, int squaresPerSide, int &score)
{
    // handle each row separately
    for( int row=0; row<squaresPerSide; row++) {
        // set index limit for this row to be index of right-most tile on this row
        int limit = row * squaresPerSide + squaresPerSide - 1;
        
        // Start from the second-to-last column and process each element from right to left
        for( int col=squaresPerSide - 1; col>=0; col--) {
            
            // get 1-d array index based on row and col
            int current = row * squaresPerSide + col;
            
            // slide current piece over as far right as possible
            while( current < limit && board[ current+1] == 0) {
                board[ current+1] = board[ current];
                board[ current] = 0;
                current++;
            }
            
            // Combine it with right neighbor if values are the same and non zero.
            // The additional check for (current < limit) ensures a tile can be combined
            // at most once on a move, since limit is moved left every time a combination is made.
            // This ensures a row of:  4 4 2 2   ends up correctly as:  0 0 8 4   and not:  0 0 4 8
            if( (current < limit) && (board[ current+1] == board[ current]) && (board[ current] != 0) ) {
                board[ current+1] = board[ current+1] + board[ current];
                board[ current] = 0;
                limit = current;           // Reset row index limit, to prevent combining a piece more than once
                score += board[ current+1];  // Update score
            }
            
        }//end for( int col...
    }//end for( int row...
    
}//end slideRight()


//--------------------------------------------------------------------
// Slide all tiles up, combining matching values, updating the score
void slideUp( int* board, int squaresPerSide, int &score)
{
    // handle each column separately
    for( int col=0; col<squaresPerSide; col++) {
        // set index limit for this column to be index of top-most tile on this row
        int limit = col;
        
        // Start from the second row and process each element from top to bottom
        for( int row=1; row<squaresPerSide; row++) {
            
            // get 1-d array index based on row and col
            int current = row * squaresPerSide + col;
            
            // slide current piece up as far as possible
            while( (current > limit) && (board[ current-squaresPerSide] == 0) ) {
                board[ current-squaresPerSide] = board[ current];
                board[ current] = 0;
                current = current - squaresPerSide;
            }
            
            // Combine it with upper neighbor if values are the same and non zero.
            // The additional check for (current > limit) ensures a tile can be combined
            // at most once on a move, since limit is moved down every time a combination is made.
            if( (current > limit) && (board[ current-squaresPerSide] == board[ current]) && (board[ current] != 0) ) {
                board[ current-squaresPerSide] = board[ current-squaresPerSide] + board[ current];
                board[ current] = 0;
                limit = current;           // Reset row index limit, to prevent combining a piece more than once
                score += board[ current-squaresPerSide];  // Update score
            }
            
        }//end for( int col...
    }//end for( int row...
    
}//end slideUp()


//--------------------------------------------------------------------
// Slide all tiles down, combining matching values, updating the score
void slideDown( int* board, int squaresPerSide, int &score)
{
    // handle each column separately
    for( int col=0; col<squaresPerSide; col++) {
        // set index limit for this column to be index of bottom-most tile on this row
        int limit = (squaresPerSide - 1) * squaresPerSide + col;
        
        // Start from the next to last row and process each element from bottom to top
        for( int row=squaresPerSide-1; row>=0; row--) {
            
            // get 1-d array index based on row and col
            int current = row * squaresPerSide + col;
            
            // slide current piece down as far as possible
            while( current < limit && board[ current+squaresPerSide] == 0 ) {
                board[ current+squaresPerSide] = board[ current];
                board[ current] = 0;
                current = current + squaresPerSide;
            }
            
            // Combine it with lower neighbor if values are the same and non zero.
            // The additional check for (current < limit) ensures a tile can be combined
            // at most once on a move, since limit is moved up every time a combination is made.
            if( (current < limit) && (board[ current+squaresPerSide] == board[ current]) && (board[ current] != 0) ) {
                board[ current+squaresPerSide] = board[ current+squaresPerSide] + board[ current];
                board[ current] = 0;
                limit = current;           // Reset row index limit, to prevent combining a piece more than once
                score += board[ current+squaresPerSide];  // Update score
            }
            
        }//end for( int col...
    }//end for( int row...
    
}//end slideDown()


//--------------------------------------------------------------------
// Return true if we're done, false if we are not done
//    Game is done if board is full and no more valid moves can be made
//    or if a tile with maxTileValue has been created.
bool gameIsOver( int* board,      // current board
                 int squaresPerSide,    // size of one side of board
                 int maxTileValue) // max tile value for this size board
{
    // See if the maxTileValue is found anywhere on the board.
    // If so, game is over.
    for( int i=0; i<squaresPerSide*squaresPerSide; i++) {
        if( board[ i] == maxTileValue) {
            std::cout << "Congratulations!  You made it to " << maxTileValue << " !!!" << std::endl;
            return true;  // game is over
        }
    }
    
    // See if there are any open squares.  If so return true since we aren't done
    for( int i=0; i<squaresPerSide*squaresPerSide; i++) {
        if( board[ i] == 0) {
            return false;  // there are open squares, so game is not over
        }
    }
    
    // All squares are full.
    // To check if board is done, make a copy of board, then slide left
    // and slide down.  If resulting board is same as original, we are done
    // with game since there are no moves to be made.
    int boardCopy[ squaresPerSide * squaresPerSide];
    copyBoard( boardCopy, board, squaresPerSide);
    int tempScore = 0;   // used as a placeHolder only for function calls below
    slideLeft( boardCopy, squaresPerSide, tempScore);
    slideDown( boardCopy, squaresPerSide, tempScore);
    
    // Compare each square of boards.  If any is different, a move is possible
    for( int i=0; i<squaresPerSide*squaresPerSide; i++) {
        if( boardCopy[ i] != board[ i]) {
            return false;  // Game is not over
        }
    }
    
    std::cout << "\n"
              << "No more available moves.  Game is over.\n"
              << "\n";
    return true;  // Game is over since all squares are full and there are no moves
}//end gameIsOver()

//---------------------------------------------------------------------------------
//Undo a move
void undo(int* &board, int &move, int &score, int &squaresPerSide, LinkedList &list) {
    if (list.top()->move == 1) {
        std::cout << "        *** You cannot undo past the beginning of the game.  Please retry. ***\n";
        return;
    }
    std::cout << "        * Undoing move *\n";
    list.pop();
    copyBoard(board, list.top()->board, squaresPerSide);
    move = list.top()->move;
    score = list.top()->score;
} //end undo()

//---------------------------------------------------------------------------------------
int main()
{	
	int moveNumber = 1;               // User move counter
	int score = 0;                    // Cummulative score, which is sum of combined tiles
    int squaresPerSide = 4;           // User will enter this value.  Set default to 4
	int* board;                       // pointer to the board
    Square* squaresArray;             // pointer an array of Square objects
    LinkedList list;                  // List to store each move
    int maxTileValue = 1024;          // 1024 for 4x4 board, 2048 for 5x5, 4096 for 6x6, etc.
    char userInput = ' ';             // Stores user input
    
	// Create the graphics window
	sf::RenderWindow window(sf::VideoMode(WindowXSize, WindowYSize), "Program 6: 1024 with Undo", sf::Style::Default);
	std::cout << std::endl;
    
    // Create and initialize the font, to be used in displaying text.
	sf::Font font;  
	initializeFont( font);
    
	// Create the messages label at the bottom of the graphics screen, for displaying debugging information
	sf::Text messagesLabel( "Welcome to 1024", font, 20);
    
	// Make a text object from the font
	messagesLabel.setColor( sf::Color(255,255,255));
    
	// Place text at the bottom of the window. Position offsets are x,y from 0,0 in upper-left of window
	messagesLabel.setPosition( 0, WindowYSize - messagesLabel.getCharacterSize() - 5); 
	
	displayInstructions();
    
    // Get the board size, create and initialize the board, and set the max tile value
    initializeBoards( board, squaresArray, squaresPerSide, maxTileValue);
    
    //Store a copy of the board in the linked list
    list.push(board, squaresPerSide, moveNumber, score);

	// Run the program as long as the window is open.  This is known as the "Event loop".
	while (window.isOpen())
	{
        //update the graphical board
        updateSquareBoard(squaresArray, squaresPerSide, board, font, window);
    
		// Display both the graphical and text boards.
		window.display();
        displayAsciiBoard( board, squaresPerSide, score, list);
        
        // Prompt for and handle user input
        std::cout << moveNumber << ". Your move: ";
        std::cin >> userInput;
        switch (userInput) {
            case 'x':
                    std::cout << "Thanks for playing. Exiting program... \n\n";
                    exit( 0);
                    break;
            case 'r':
                   std::cout << "\n"
                              << "Resetting board \n"
                              << "\n";
                    // Prompt for board size
                    std::cout << "Enter the size board you want, between 4 and 12: ";
                    std::cin >> squaresPerSide;
                
                    //delete board, squaresArray and clear the linked list
                    delete [] board;
                    delete [] squaresArray;
                    list.clear();
                
                    //initialize board and squaresArray. Reset moveNumber and Score.
                    //Store a copy of the board in the linked list
                    initializeBoards( board, squaresArray, squaresPerSide, maxTileValue);
                    score = 0;
                    moveNumber = 1;
                    list.push(board, squaresPerSide, moveNumber, score);
                    continue;  // go back up to main loop and restart game
                    break;
            case 'a':
                    slideLeft( board, squaresPerSide, score);  // Slide left
                    break;
            case 's':
                    slideDown( board, squaresPerSide, score);  // Slide down
                    break;
            case 'd':
                    slideRight( board, squaresPerSide, score); // Slide right
                    break;
            case 'w':
                    slideUp( board, squaresPerSide, score);    // Slide up
                    break;
            case 'p':
                    // Place a piece on the board
                    int index;  // 1-d array index location to place piece
                    int value;  // value to be placed
                    std::cin >> index >> value;
                    board[ index] = value;
                
                    // store a copy of the board in the linked list
                    list.push(board, squaresPerSide, moveNumber, score);
                    continue;  // Do not increment move number or place random piece
                    break;
            case 'u':
                    undo(board, moveNumber, score, squaresPerSide, list);
                    continue;
                    break;
            default:
                    std::cout << "Invalid input, please retry.";
                    continue;
                    break;
        }//end switch( userInput)
        
        // If the move resulted in pieces changing position, then it was a valid move
        // so place a new random piece (2 or 4) in a random open square and update move number.
        // Add the new board, moveNumber and score to the linked list.
        if( boardChangedThisTurn(list.top()->board, board, squaresPerSide)) {
            // Place a random piece on board
            placeRandomPiece( board, squaresPerSide);
            // Update move number after a valid move
            moveNumber++;
            // store a copy of the board in the linked list
            list.push(board, squaresPerSide, moveNumber, score);
        }
        
        char aString[25];
        sprintf(aString, "%d. Your move:", moveNumber);
        messagesLabel.setString(aString);
        window.draw(messagesLabel);
        
		// See if we're done
		if( gameIsOver( board, squaresPerSide, maxTileValue)) {
            // Display the final board
            displayAsciiBoard( board, squaresPerSide, score, list);
            // Clear the linked list
            list.clear();
            break;
        }

		system("clear");   // Clear the screen

		// Pause the event loop, so that Codio does not think it is a runaway process and kill it after some time
		std::this_thread::sleep_for(std::chrono::milliseconds( 50));

	}//end while( window.isOpen())

	return 0;
}//end main()
