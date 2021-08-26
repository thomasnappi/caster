#include <string>
#include <ncurses.h>
#include <cmath>
using std::string;
using std::to_string;

// Define data types we will use here
struct player
{
  float x;
  float y;
  double o;
};
struct window
{
  int h;
  int w;
  double fov;
};
struct point
{
  int x;
  int y;
};
struct fpoint
{
  float x;
  float y;
};
// Define a floating point pi for ease of use
const float pi = 3.14159265358979f;

// The dimensions of our map
const unsigned short mx = 10;
const unsigned short my = 10;

// The a transpose of map we will see.  In this implementation, it does
// not matter if there are walls around the edges as rays have a maximum
// distance.  Movement being bound by the array is an option as well.
const unsigned short map[my][mx] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 1, 0, 1, 0, 0, 0, 0, 1},
    {1, 0, 1, 0, 1, 0, 0, 0, 0, 1},
    {1, 1, 1, 0, 1, 1, 1, 1, 1, 1}};

// The maximum length a ray will travel before a forced collision.
// Should be just above the maximum distance between map elements;
// if you want to "see the horizon" it's a better idea to change
// castRay so that it just returns a high value as soon as the ray
// goes out of bounds.
const unsigned short maxRayDist = 25;

// Whether the player has collision with the edges of the array
const bool mustBeBound = true;

// Function that computes rays
float castRay(window w, player p, double d)
{
  // Length will be the distance to the wall.  We start at the
  // camera location, and move away along the direction of the ray
  float len = 0;
  while (true)
  {
    // Move the ray a little further and check again
    len += 0.1;
    // Get rounded coordinates for checking against the map
    point cast;
    cast.x = floor(p.x + (len * cos(d)));
    cast.y = floor(p.y + (len * sin(d)));
    bool hitMap = false;
    {
      // Check if the rounded coordinates are in the map
      // (Only a problem if the map has openings at the edges)
      bool xOutOfBounds = cast.x < 0 || cast.x >= mx;
      bool yOutOfBounds = cast.y < 0 || cast.y >= my;
      bool outOfBounds = xOutOfBounds || yOutOfBounds;
      // If we're out of the map, we don't want to check against it.
      if (!outOfBounds)
      {
        hitMap = map[cast.x][cast.y] == 1;
      }
    }
    // If we hit a point on the map, or have gone too far, return len
    if (hitMap || len > maxRayDist)
    {
      return len;
    }
  }
}

int main()
{
  // Initialize ncurses
  initscr();

  // Prevent keypresses from appearing on the screen
  noecho();

  // Initialize our color scheme, green text on black background
  start_color();
  init_pair(1, COLOR_GREEN, COLOR_BLACK);
  attron(COLOR_PAIR(1));

  // Define our "window", not related to the ncurses window;
  // Just a way of storing a width, height, and field of view
  window w;
  w.fov = pi / 4.;
  w.w = 80;
  w.h = 40;

  // Define a player object with a point and orientation, to
  // use for calculating where and which direction to place the view.
  player p;
  p.x = 1.5;
  p.y = 5;
  p.o = 0;

  // Our display loop
  while (true)
  {
    //////////////////////
    // Casting the rays //
    //////////////////////

    // Cast our rays & determine line height
    float resp[w.w];
    for (int f = 0; f < w.w; f += 1)
    {
      // Calculate the direction the ray will travel in
      double dir = p.o + (f * (w.fov / w.w));
      // Calculate the distance to the nearest wall
      float dist = castRay(w, p, dir);
      // Invert dist wrt screen height; distort dist slightly to lessen
      // fisheye.
      resp[f] = w.h / pow(dist, 1.2);
      if (resp[f] < 1)
      {
        resp[f] = 0;
      }
    }

    // Create an array representing what will be displayed on screen
    // This is necessary so we can traverse cols -> rows
    int ch[w.w][w.h];
    for (int x = 0; x < w.w; x += 1)
    {
      int pp = floor((w.h - resp[x]) / 2.);
      for (int y = 0; y < w.h; y += 1)
      {
        // Sky
        if (y <= pp)
        {
          ch[x][y] = 1;
        }
        // Grass
        else if (y >= w.h - pp)
        {
          ch[x][y] = 2;
        }
        // Hedge (Wall)
        else
        {
          ch[x][y] = 3;
        }
      }
    }

    ////////////////////////////
    // Printing to the screen //
    ////////////////////////////

    // Clear the screen so we can print new values
    move(0,0);

    // Print from the values we just evaluated, but traverse rows -> cols
    for (int y = 0; y < w.h; y += 1)
    {
      for (int x = 0; x < w.w; x += 1)
      {
        switch (ch[x][y])
        {
          // Sky
        case 1:
        {
          printw(" ");
          break;
        }
        // Grass
        case 2:
        {
          printw(",");
          break;
        }
        // Hedge (Wall)
        case 3:
        {
          // This will be slightly different depending on environment;
          // we let ncurses determine the best char to use.
          addch(ACS_BLOCK);
          break;
        }
        }
      }
      // Move to the next horizontal line
      printw("\n");
    }

    // Push changes to the screen
    refresh();

    ////////////////////////////////////
    // Deal with input and collisions //
    ////////////////////////////////////

    // Wait for a keypress to update
    int a = getch(); // D = 100; S = 115; A = 97; W = 119; ESC = 27

    // If escape is pressed, undo our terminal changes and exit
    if (a == 27)
    {
      attroff(COLOR_PAIR(1));
      endwin();
      return (0);
      break;
    }

    // The direction to move in; due to offset of rays being cast,
    // this is different from p.o
    double md = p.o + (w.fov / 2.);

    // If W key is pressed, move forwards (or collide)
    if (a == 119)
    {
      // Calculate the next point
      fpoint n;
      n.y = p.y + 0.2 * sin(md);
      n.x = p.x + 0.2 * cos(md);
      // Determine what section of the map we will land on
      point fn;
      fn.x = floor(n.x);
      fn.y = floor(n.y);
      // Check if we are constrained by bounds
      bool boundBlock;
      {
        bool xOutOfBounds = fn.x < 0 || fn.x >= mx;
        bool yOutOfBounds = fn.y < 0 || fn.y >= my;
        boundBlock = (!mustBeBound || (!yOutOfBounds && !xOutOfBounds));
      }
      // If we do not collide, move the player
      if (map[fn.x][fn.y] != 1 && boundBlock)
      {
        p.x = n.x;
        p.y = n.y;
      }
    }
    // If S key is pressed, move backwards (or collide)
    else if (a == 115)
    {
      // Calculate the next point
      fpoint n;
      n.y = p.y + (-0.2) * sin(md);
      n.x = p.x + (-0.2) * cos(md);
      // Determine what section of the map we will land on
      point fn;
      fn.x = floor(n.x);
      fn.y = floor(n.y);
      // Check if we are constrained by bounds
      bool boundBlock;
      {
        bool xOutOfBounds = fn.x < 0 || fn.x >= mx;
        bool yOutOfBounds = fn.y < 0 || fn.y >= my;
        boundBlock = (!mustBeBound || (!yOutOfBounds && !xOutOfBounds));
      }
      // If we do not collide, move the player
      if (map[fn.x][fn.y] != 1 && boundBlock)
      {
        p.x = n.x;
        p.y = n.y;
      }
    }
    // If D key is pressed, turn right
    else if (a == 100)
    {
      p.o += 0.08 * pi;
    }
    // If A key is pressed, turn left
    else if (a == 97)
    {
      p.o -= 0.08 * pi;
    }
    else
    {
      // Nothing, we don't care about other keys.
    }
    if (mustBeBound && (p.x < 0 || p.y < 0))
    {
      exit(1000);
    }
  }
}