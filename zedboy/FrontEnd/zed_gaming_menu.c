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
#include "zedboy.h"
#include "../utils/displayImg.h"
#include "../utils/draw.h"

//include games 
#include "../games/snake/snake.h"
#include "../games/breakout/breakout.h"

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
//int oledfd;
unsigned int *switchregs;
unsigned int *btnregs;
ZoledGamingMainMenu *zedboyMainMenu;


int getNthBit(int number, int n) {
    return (number >> n) & 0x1;
}

const int xoffsets[] = {0,32,64,96};
int displayMenu(ZoledGamingMainMenu *zedboy){
  fbClear(fd);
  for (size_t i = 0; i < zedboyMainMenu->optionsSize; i++)
  {
    if(i == zedboyMainMenu->selected){
      //dotted versuion
      printf("printing selected index : i %u \n", i);
      assert(!fbDisplayPBM(zedboyMainMenu->optionsLogos[i],xoffsets[i],0,0,32,32,64,1)&& "Failed to display solid border logo");
      continue;
    }

    assert(!fbDisplayPBM(zedboyMainMenu->optionsLogos[i],xoffsets[i],0,0,0,32,32,0) && "Failed to display solid border logo");
    /* code */
  }
  //writes to board
  fbFlush(fd);
};




int selectOption( ZoledGamingMainMenu *zedboy){
               zedboyMainMenu->isGameRunning = 1;
               zedboyMainMenu->isConsoleRunning=0;

                games[zedboyMainMenu->selected]();
                // print whatevers in games[0]
              //printf('%s\n', games[zedboyMainMenu->selected]);
              return 0;
};

int progressMenu(int direction){
  switch (direction) {
        case MIDDLE:
                selectOption(zedboyMainMenu);
            break;
        case LEFT:
        //left
              zedboyMainMenu->selected = abs((zedboyMainMenu->selected - 1) % zedboyMainMenu->optionsSize);
              displayMenu(zedboyMainMenu);
            break;
        case RIGHT:
        //right
              zedboyMainMenu->selected = (zedboyMainMenu->selected + 1) % zedboyMainMenu->optionsSize;
              displayMenu(zedboyMainMenu);
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
                zedboyMainMenu->isGameRunning = 0;
                zedboyMainMenu->isConsoleRunning = 1;
            }

            gameTicks[zedboyMainMenu->selected](direction);
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
  int swfd;

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
  swfd = open("/dev/mem", O_RDWR|O_SYNC);
  // Check if file has opened successfully
  // If not, exit with failure
  if(swfd < 0) {
    printf("Error opening /dev/mem! \n");
    close(swfd);
    return -1;
  }

  addr = mmap(0, length, PROT_READ, MAP_SHARED, swfd, switch_addr);
  
  // Assign GPIO address and registers
  *switchregs = (unsigned int*) addr;

  // Check if mmap has mapped successfully
  // If not, exit with failure
  if(addr == MAP_FAILED) {
    printf("Error mapping memory! \n");
    close(swfd);
    return -1;
  }

  // success
  return 0;
}

static int map_btns(unsigned int** btnregs, unsigned int btn_addr)
{
  int btnfd;

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
  btnfd = open("/dev/mem", O_RDWR|O_SYNC);
  // Check if file has opened successfully
  // If not, exit with failure
  if(btnfd < 0) {
    printf("Error opening /dev/mem! \n");
    close(btnfd);
    return -1;
  }

  addr = mmap(0, length, PROT_READ, MAP_SHARED, btnfd, btn_addr);
  
  // Assign GPIO address and registers
  *btnregs = (unsigned int*) addr;

  // Check if mmap has mapped successfully
  // If not, exit with failure
  if(addr == MAP_FAILED) {
    printf("Error mapping memory! \n");
    close(btnfd);
    return -1;
  }

  // success
  return 0;
}

void run_zedBoy() {
      printf("attempting to run zedboy\n");
      displayMenu(zedboyMainMenu);
while (1)
  {
  zedboyMainMenu->isConsoleRunning = 1;
  endGame = 0;
  zedboyMainMenu->isGameRunning = 0;

      while ((zedboyMainMenu->isConsoleRunning) == 1)
       {

          progressMenu(read_btns());
          printf("zedboy btn processed\n");
          usleep(1000);

        }

        while (zedboyMainMenu->isGameRunning == 1 && !endGame)
        {
           progressGame(read_btns());
           usleep(1000);
        }

  }
    }





int main(int argc, char** argv)
{
  printf("zedboy starting\n");



  fd = oledOpen();




  char * games[]= {Snake_Icon, Pong_Icon, Breakout_Icon, Tetris_Icon};

  //printf("%d%d%d%d\n",LEFT,RIGHT,UP,DOWN);

  //printf("got icons\n");
  zedboyMainMenu = malloc(sizeof(ZoledGamingMainMenu));

  zedboyMainMenu->optionsLogos = games;
  zedboyMainMenu->selected = 0;
  zedboyMainMenu->optionsSize = 4;
  zedboyMainMenu->isConsoleRunning = 1;
  zedboyMainMenu->isGameRunning = 0;

  printf("zedboy init\n");


  //displayMenu(zedboy);

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


    run_zedBoy();


    oledClose(fd);
    return 0;
}