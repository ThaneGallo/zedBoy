#ifndef DEFAULT_MENU
#define DEFAULT_MENU


/// @brief type, for ZoledMenu. optionsLogos, list of paths to fileicons, current option selection
typedef struct
{
  char **optionsLogos; 
  int optionsSize; // size of options in Options zedboy
  int selected; // current selected zedboy item. inciment with %=numOptions
  int isGameRunning;
  int isConsoleRunning;
} ZoledGamingMainMenu;

int selectOption( ZoledGamingMainMenu *zedboy);

/// @brief takes input and navigates zedboy accordinging
/// @param zedboy 
/// @param direction 
/// @return 
int NavMenu( ZoledGamingMainMenu *zedboy ,int input);

/// @brief function TO display the Menu On oled device
/// @param zedboy current zedboy state
/// @return 0 on success
int displayMenu(ZoledGamingMainMenu *zedboy);



// Function to progress the menu based on button inputs
int progressMenu(int direction);

// Function to progress the game based on button inputs
int progressGame(int direction);




#endif //