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
#define HEIGHT (96)

#define BADGE_HEIGHT (16)
#define BADGE_WIDTH  ( 8)

static unsigned int menu_pos = 0;
static unsigned int edit_col = 0;

static int bounds[2][2] = {{ 0, 0 }, { 0, 0 }};
static uint8_t pixel_height = 0;
static uint8_t pixel_width = 0;

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

unsigned int lineartox(unsigned int i) {
  return i % WIDTH;
}

unsigned int lineartoy(unsigned int i) {
  return 96 + 15 - floor(i / WIDTH);
}

unsigned int xytolinear(unsigned int x, unsigned int y) {
  return (96 - y) * WIDTH + x;
}

void erode_pixel(i) {
  uint8_t weight = 5;
  uint8_t pos = 0;

  int hits[9] = {
    i - WIDTH - 1, i - WIDTH, i - WIDTH + 1,
    i - 1,         i,         i + 1,
    i + WIDTH - 1, i + WIDTH, i + WIDTH + 1
  };

  for(pos = 0; pos < 9; pos++) {
    if(cambuffer[hits[pos]] == 1) weight++;
    else weight--;
  }

  for(pos = 0; pos < 9; pos++) {
    if(weight < 5)
      plotblock(lineartox(hits[pos]), lineartoy(hits[pos]), 1, 1, primarycol[0]);
  }
}

// pixel per pixel
// figure out how much area a pixel takes up
void calculate_ppp() {
  pixel_height = (bounds[0][1] - bounds[0][0]) / BADGE_HEIGHT;
  pixel_width = (bounds[0][1] - bounds[0][0]) / BADGE_WIDTH;
}

void do_erosion() {
  unsigned int x;
  unsigned int y;

  for(x = 1; x < HEIGHT - 1; x += 3) {
    for(y = 1; y < WIDTH - 1; y += 3) {
      erode_pixel(x * WIDTH + y);
    }
  }

}

void detect_pixels() {
  unsigned int x, y;
  uint8_t hheight = floor(pixel_height / 2);
  uint8_t hwidth = floor(pixel_width / 2);

  for(x = bounds[0][0] + 2; x < bounds[0][1]; x += pixel_height) {
    for(y = bounds[1][0] + hheight; y < bounds[1][1]; y += pixel_height) {
      if(cambuffer[xytolinear(x, y)] == 1) {
        plotblock(x, y + 15, 1, 1, primarycol[1]);
      } else {
        plotblock(x, y + 15, 1, 1, cya);
      }
    }
  }
}

char* pp51dodo(unsigned int action) {
  static unsigned int state;

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
    loadbmp("CAM0014.BMP", 2);

    unsigned int gi;

    for(gi = 0, i = 0; i < 128*96*3; i += 3, gi++) {
      x = gi % WIDTH;
      y = 96 - floor(gi / WIDTH);

      if(
          (cambuffer[i + 2] > terms[0][0] && cambuffer[i + 2] < terms[0][1]) &&
          (cambuffer[i + 1] > terms[1][0] && cambuffer[i + 1] < terms[1][1]) &&
          (cambuffer[i]     > terms[2][0] && cambuffer[i]     < terms[2][1])
      ) {
        if(bounds[0][0] == 0 || x < bounds[0][0]) bounds[0][0] = x;
        if(bounds[0][1] == 0 || x > bounds[0][1]) bounds[0][1] = x;
        if(bounds[1][0] == 0 || y < bounds[1][0]) bounds[1][0] = y;
        if(bounds[1][1] == 0 || y > bounds[1][1]) bounds[1][1] = y;

        cambuffer[gi] = 1;

        plotblock(x, y + 15, 1, 1, primarycol[2]);
      } else {
        cambuffer[gi] = 0;

        plotblock(x, y + 15, 1, 1, primarycol[0]);
        //plotblock(x, y + 15, 1, 1, rgbto16(cambuffer[i + 2], cambuffer[i + 1], cambuffer[i]));
      }
    }

    do_erosion();

    calculate_ppp();

    // should be 6x13
    printf("  %d %d", pixel_height, pixel_width);

    plotblock(
      bounds[0][0], bounds[1][0] + 15,
      bounds[0][1] - bounds[0][0],
      bounds[1][1] - bounds[1][0],
      primarycol[1]
    );

    detect_pixels();

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
