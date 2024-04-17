/*
  @file gpio.c
  @brief GPIO Manipulation Skeleton Code

 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stddef.h>
#include <time.h>

// there is a snake.c file in the a lower directory called snake/snake.c, also snake.h, include snake.h
#include "gameConstants.h"
#include "snake/snake.h"

// example: LED GPIO Address
#define SWITCH_GPIO_ADDR 0x41210000
#define BTN_GPIO_ADDR 0x41220000 //is BUTTON ADDRESS

// register offsets
#define REG_OFFSET(REG, OFF) ((REG)+(OFF))
#define reg_count 64
#define reg_size sizeof(uint32_t)

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

int getNthBit(int number, int n) {
    return (number >> n) & 0x1;
}

void runConsole(unsigned int *switchregs, unsigned int *btnregs) {

    int isConsoleRunning = 1;
    while (isConsoleRunning == 1)
    {
        int isGameSelected = 0;

        while (isGameSelected == 0) {
            int sw0 = getNthBit(*REG_OFFSET(switchregs, 0), 0);
            int sw1 = getNthBit(*REG_OFFSET(switchregs, 0), 1);
            int sw2 = getNthBit(*REG_OFFSET(switchregs, 0), 2);
            int sw3 = getNthBit(*REG_OFFSET(switchregs, 0), 3);
            int sw4 = getNthBit(*REG_OFFSET(switchregs, 0), 4);
            int sw5 = getNthBit(*REG_OFFSET(switchregs, 0), 5);
            int sw6 = getNthBit(*REG_OFFSET(switchregs, 0), 6);
            int sw7 = getNthBit(*REG_OFFSET(switchregs, 0), 7);
            int sw8 = getNthBit(*REG_OFFSET(switchregs, 0), 8);

            printf("Switches: %d%d%d%d%d%d%d%d%d\n", sw8, sw7, sw6, sw5, sw4, sw3, sw2, sw1, sw0);

            if (sw0 == 1) {
                isGameSelected = 1;

                // gameSetups[0]();
                games[0]();
                // print whatevers in games[0]
                // printf('%s\n', games[0]);
            }

            usleep(100000);
        }

        while (isGameSelected == 1)
        {
            // readback = *REG_OFFSET(btnregs, 0);
            int middle = getNthBit(*REG_OFFSET(btnregs, 0), 0);
            int down = getNthBit(*REG_OFFSET(btnregs, 0), 1);
            int left = getNthBit(*REG_OFFSET(btnregs, 0), 2);
            int right = getNthBit(*REG_OFFSET(btnregs, 0), 3);
            int up = getNthBit(*REG_OFFSET(btnregs, 0), 4);

            printf("Buttons: %d%d%d%d%d\n", down, left, right, up, middle);

            if (middle == 1) {
                isGameSelected = 0;
                isConsoleRunning = 0;
            }

            int direction = 0;
            if (left == 1) {
                direction = LEFT;
            } else if (right == 1) {
                direction = RIGHT;
            } else if (up == 1) {
                direction = UP;
            } else if (down == 1) {
                direction = DOWN;
            }

            gameTicks[0](direction);

            usleep(100000);
        }
    }
}

int main(int argc, char** argv)
{
    unsigned int *switchregs;
    unsigned int *btnregs;

    // try to setup Switches
    if (map_switches(&switchregs, SWITCH_GPIO_ADDR))
    {
        // failed to setup
        return 1;
    }

    if (map_btns(&btnregs, BTN_GPIO_ADDR))
    {
        // failed to setup
        return 1;
    }

    runConsole(switchregs, btnregs);

    return 0;
}