// template demonstration application for camera badge
// new apps need to be added to list in applist.h

#include "cambadge.h"
#include "globals.h"
#include "math.h"

// states used by this application

#define s_start 0
#define s_run 1
#define s_freeze 2

#define WIDTH (128)

static unsigned int menu_pos = 0;
static unsigned int edit_col = 0;

void menu_line(char* text, unsigned int pos, unsigned int low, unsigned int hi) {
  unsigned int i;
  for(i = 0; i <= pos; i++) printf("\n");

  if(menu_pos == pos && edit_col == 0) {
    printf(blu ">");
  } else {
    printf(whi);
  }

  printf(" %s ", text);

  if(edit_col == 1 && menu_pos == pos) {
    printf(">%d< - %d", low, hi);
  } else if(edit_col == 2 && menu_pos == pos) {
    printf("%d - >%d<", low, hi);
  } else {
    printf("%d - %d     ", low, hi);
  }
}

char* pp51dodo(unsigned int action) {
  static unsigned int blink = 0;
  static unsigned int state,colour;

  static unsigned int terms[3][2] = {{ 150, 255}, { 240, 255 }, { 240, 255 }};

  unsigned int x,y,i;

  switch(action) {
    case act_name : return("PP-51 DoDo");
    case act_help : return("Hack the past");
    case act_init :
                    // add any code here that needs to run once at powerup - e.g. hardware detection/initialisation
                    return(0);

    case act_powerdown :
                    // add any code here that needs to run before powerdown
                    return(0);

    case act_start :
                    // called once when app is selected from menu
                    state=s_start;
                    colour=1;
                    return(0);
  } //switch

  if (action!=act_poll) return(0);

  // do anything that needs to be faster than tick here.

  if(!tick) return(0);

  switch(state) {
    case s_start :
      state=s_run;
      menu_pos = 0;


    case s_run :
      printf(top butcol "EXIT             Proc");

      menu_line("RED", 0, terms[0][0], terms[0][1]);
      menu_line("GREEN", 1, terms[1][0], terms[1][1]);
      menu_line("BLUE", 2, terms[2][0], terms[2][1]);

      printf(bot butcol "  " uarr "         " darr"    Next" whi);

      state = s_freeze;

      break;

    case s_freeze :
      if(butpress) state=s_run;
      break;

  } // switch state

  if(butpress & but3) {
    if(++edit_col == 3) edit_col = 0;
  }

  if(butpress & but4) {
    FSchdir("CAMERA");
    loadbmp("CAM0012.BMP", 2);

    unsigned int gi;

    for(gi = 0, i = 0; i < cambufsize; i += 3, gi++) {
      x = gi % WIDTH;
      y = 96 - floor(gi / WIDTH);

      if(
          (cambuffer[i + 2] > terms[0][0] && cambuffer[i + 2] < terms[0][1]) &&
          (cambuffer[i + 1] > terms[1][0] && cambuffer[i + 1] < terms[1][1]) &&
          (cambuffer[i]     > terms[2][0] && cambuffer[i]     < terms[2][1])
      ) {
        plotblock(x, y + 15, 1, 1, primarycol[2]);
      } else {
        plotblock(x, y + 15, 1, 1, primarycol[0]);
        //plotblock(x, y + 15, 1, 1, rgbto16(cambuffer[i + 2], cambuffer[i + 1], cambuffer[i]));
      }
    }

    state = s_freeze;
  }

  printf(bot butcol "  " uarr "         " darr"      Go" whi);

  if(edit_col == 0 && (butpress & but1)) {
    if(--menu_pos > 3) menu_pos = 2;  // clear screen & restart
  } else if(edit_col == 0 && (butpress & but2)) {
    if(++menu_pos == 3) menu_pos = 0;
  } else {
    if(butpress & but1) {
      terms[menu_pos][edit_col - 1]++;
    }

    if(butpress & but2) {
      terms[menu_pos][edit_col - 1]--;
    }
  }

  if(butpress & powerbut) return(""); // exit with nonzero value to indicate we want to quit

  return(0);
}
