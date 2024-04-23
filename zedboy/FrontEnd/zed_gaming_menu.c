#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stddef.h>
#include <time.h>

// there is a snake.c file in the a lower directory called snake/snake.c, also snake.h, include snake.h
#include "../utils/gameConstants.h"
#include "zed_gaming_menu.h"
#include "../utils/displayImg.h"
#include "../utils/draw.h"

//include games 
#include "../games/snake/snake.h"

// example: LED GPIO Address
#define SWITCH_GPIO_ADDR 0x41210000
#define BTN_GPIO_ADDR 0x41220000 //is BUTTON ADDRESS

// register offsets
#define REG_OFFSET(REG, OFF) ((REG)+(OFF))
#define reg_count 64
#define reg_size sizeof(uint32_t)



// game icons
#define Tetris_Icon "./assets/tetrisGameCard.pbm"
#define Snake_Icon "./assets/snakeGameCard.pbm"
#define Pong_Icon "./assets/pongGameCard.pbm"
#define Breakout_Icon "./assets/breakoutGameCard.pbm"


//constants 
int oledfd;
unsigned int *switchregs;
unsigned int *btnregs;
ZoledGamingMainMenu *menu;


int getNthBit(int number, int n) {
    return (number >> n) & 0x1;
}

const int xoffsets[] = {0,32,64,96};
int displayMenu(ZoledGamingMainMenu *menu){
  for (size_t i = 0; i < menu->optionsSize; i++)
  {
    if(i == menu->selected){
      //dotted versuion
      printf("printing selected index : i %u \n", i);
      assert(!fbDisplayPBM(menu->optionsLogos[i],xoffsets[i],0,0,32,32,64,1)&& "Failed to display solid border logo");
      continue;
    }

    assert(!fbDisplayPBM(menu->optionsLogos[i],xoffsets[i],0,0,0,32,32,0) && "Failed to display solid border logo");
    /* code */
  }
  //writes to board
  fbFlush(oledfd);

};




int selectOption( ZoledGamingMainMenu *menu){
               menu->isGameSelected = 1;
               menu->isConsoleRunning=0;

                games[menu->selected](oledfd);
                // print whatevers in games[0]
              //printf('%s\n', games[menu->selected]);
              return 0;
};

int progressMenu(int direction){
  switch (direction) {
        case MIDDLE:
                selectOption(menu);
            break;
        case LEFT:
        //left
              menu->selected = abs((menu->selected - 1) % menu->optionsSize);
              displayMenu(menu);
            break;
        case RIGHT:
        //right
              menu->selected = (menu->selected + 1) % menu->optionsSize;
              displayMenu(menu);
            break;
        //case 3://UPP break;
        //case 4: //game->dir = DOWN; break;
        default:
            break;
  }
  return 0;
}


/// @brief chekcs for button inputs, and if a game is running sends input to game.
/// @return 
int progressGame(int direction){
  // readback = *REG_OFFSET(btnregs, 0);
            if (direction == -1) {
                menu->isGameSelected = 0;
                menu->isConsoleRunning = 1;
            }

            gameTicks[menu->selected](direction);
}

int read_btns(){
            int middle = getNthBit(*REG_OFFSET(btnregs, 0), 0);
            int down = getNthBit(*REG_OFFSET(btnregs, 0), 1);
            int left = getNthBit(*REG_OFFSET(btnregs, 0), 2);
            int right = getNthBit(*REG_OFFSET(btnregs, 0), 3);
            int up = getNthBit(*REG_OFFSET(btnregs, 0), 4);

            printf("Buttons: %d%d%d%d%d\n", down, left, right, up, middle);

            //returns button code if the btn is pressed.
              return    middle ? MIDDLE :
                        left ? LEFT : 
                        right ? RIGHT :
                        up ? UP : 
                        down ? DOWN : 0;

              

}


// map GPIO peripheral; base address will be mapped to gpioregs pointer
static int map_switches(unsigned int** switchregs, unsigned int switch_addr)
{
  int fd;

  if (!switchregs || !switch_addr)
    {
      // invalid pointers
      return 1;
    }

  // use mmap here to map the appropriate address (gpio_addr) into the gpioregs pointer passed
  // by the caller.
  // return non-zero codes on errors!
  void* addr;
  long int length_i = reg_count * reg_size;
  size_t length = (size_t) length_i;
  fd = open("/dev/mem", O_RDWR|O_SYNC);
  // Check if file has opened successfully
  // If not, exit with failure
  if(fd < 0) {
    printf("Error opening /dev/mem! \n");
    close(fd);
    return -1;
  }

  addr = mmap(0, length, PROT_READ, MAP_SHARED, fd, switch_addr);
  
  // Assign GPIO address and registers
  *switchregs = (unsigned int*) addr;

  // Check if mmap has mapped successfully
  // If not, exit with failure
  if(addr == MAP_FAILED) {
    printf("Error mapping memory! \n");
    close(fd);
    return -1;
  }

  // success
  return 0;
}

static int map_btns(unsigned int** btnregs, unsigned int btn_addr)
{
  int fd;

  if (!btnregs || !btn_addr)
    {
      // invalid pointers
      return 1;
    }

  // use mmap here to map the appropriate address (gpio_addr) into the gpioregs pointer passed
  // by the caller.
  // return non-zero codes on errors!
  void* addr;
  long int length_i = reg_count * reg_size;
  size_t length = (size_t) length_i;
  fd = open("/dev/mem", O_RDWR|O_SYNC);
  // Check if file has opened successfully
  // If not, exit with failure
  if(fd < 0) {
    printf("Error opening /dev/mem! \n");
    close(fd);
    return -1;
  }

  addr = mmap(0, length, PROT_READ, MAP_SHARED, fd, btn_addr);
  
  // Assign GPIO address and registers
  *btnregs = (unsigned int*) addr;

  // Check if mmap has mapped successfully
  // If not, exit with failure
  if(addr == MAP_FAILED) {
    printf("Error mapping memory! \n");
    close(fd);
    return -1;
  }

  // success
  return 0;
}


void runMenu() {
      printf("attempting to run menu\n");

      while ((menu->isConsoleRunning) == 1)
       {

          progressMenu(read_btns());
          printf("menu btn processed\n");
          usleep(1000);

        }

        while (menu->isGameSelected == 1)
        {
           progressGame(read_btns());
           usleep(1000);
        }
    }





int main(int argc, char** argv)
{
  printf("zedboy starting\n");



  oledfd = oledOpen();




  char * games[]= {Snake_Icon,Tetris_Icon, Pong_Icon,Breakout_Icon};

  printf("%d%d%d%d\n",LEFT,RIGHT,UP,DOWN);

  printf("got icons\n");
  menu = malloc(sizeof(ZoledGamingMainMenu));

  menu->optionsLogos = games;
  menu->selected = 0;
  menu->optionsSize = 4;
  menu->isConsoleRunning = 1;
  menu->isGameSelected = 0;

  printf("menu init\n");


  displayMenu(menu);

    // try to setup Switches
    if (map_switches(&switchregs, SWITCH_GPIO_ADDR))
    {
        // failed to setup
        return 1;
    }
          printf("mapped switches\n");


    if (map_btns(&btnregs, BTN_GPIO_ADDR))
    {
        // failed to setup
        return 1;
    }
       printf("mapped btns\n");


    runMenu();


    oledClose(oledfd);
    return 0;
}