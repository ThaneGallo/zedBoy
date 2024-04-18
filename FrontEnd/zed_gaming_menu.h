#ifndef DEFAULT_MENU
#define DEFAULT_MENU


/// @brief type, for ZoledMenu. optionsLogos, list of paths to fileicons, current option selection
typedef struct
{
  char **optionsLogos; 
  int optionsSize; // size of options in Options menu
  int selected; // current selected menu item. inciment with %=numOptions
} ZoledGamingMainMenu;

int selectOption( ZoledGamingMainMenu *menu);

/// @brief takes input and navigates menu accordinging
/// @param menu 
/// @param direction 
/// @return 
int NavMenu( ZoledGamingMainMenu *menu ,int input);

/// @brief Method TO display the Menu On whatever device
/// @param menu current menu state
/// @return 0 on success
int displayMenu(ZoledGamingMainMenu *menu);



#endif //