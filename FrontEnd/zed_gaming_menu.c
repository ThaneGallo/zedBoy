#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#include "zed_gaming_menu.h"
#include "displayImg.h"

#define Tetris_Icon "./assets/tetrisGameCard.pbm"
#define Snake_Icon "./assets/snakeGameCard.pbm"
#define Pong_Icon "./assets/pongGameCard.pbm"





int selectOption( ZoledGamingMainMenu *menu);

int NavMenu( ZoledGamingMainMenu *menu ,int direction){
  switch (direction) {
        case 1:
        //left
              menu->selected = abs((menu->selected - 1) % menu->optionsSize);
            break;
        case 2:
        //right
              menu->selected = (menu->selected + 1) % menu->optionsSize;
            break;
        case 3:
              //selectOption(menu);
            break;
        case 4:
               // game->dir = DOWN;
    
            break;
        default:
            // if stop button is pressed, end the game (DIR = 0 = STOP)
           // game->gameOver = 1;
            break;
  }
  return 0;
}

const int xoffsets[] = {0,32,64,96};

int displayMenu(ZoledGamingMainMenu *menu){
  for (size_t i = 0; i < menu->optionsSize; i++)
  {
    if(i == menu->selected){
      //dotted versuion
      printf("printing selected index : i %ld \n", i);
      fbDisplayPBM(menu->optionsLogos[i],xoffsets[i],0,0,32,32,64);
      continue;
    }
    //dotten version
    fbDisplayPBM(menu->optionsLogos[i],xoffsets[i],0,0,0,32,32);
    /* code */
  }
  


};


int main(){
  ZoledGamingMainMenu menu;

char *logos[] = {Tetris_Icon,Pong_Icon,Snake_Icon};
  menu.optionsLogos = logos;
  menu.selected = 0;
  menu.optionsSize = 3;
  displayMenu(&menu);

  fbPrint();
  printf("\n");



  return 0;
}