#include <string>
#include <ncurses.h>
#include <cmath>
using std::string;
using std::to_string;

// Define data types
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
float pi = 3.14159265358979323846;
int map[10][10] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 1, 1, 1, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 1, 0, 0, 1},
    {1, 0, 0, 0, 1, 1, 1, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};

// Function that computes rays
float castRay(window w, player p, double d)
{
  bool hit = false;
  float len = 0;
  while (!hit)
  {
    len += 0.1;
    point cast;
    cast.x = floor(p.x + (len * cos(d)));
    cast.y = floor(p.y + (len * sin(d)));
    if (map[cast.x][cast.y] == 1)
    {
      hit = true;
    }
  }
  return len;
}

int main()
{
  // Initialize ncurses
  initscr();
  noecho();

  start_color();

  // unsigned short red = 1;
  // unsigned short green = 2;
  // unsigned short blue = 3;

  // init_pair(red, COLOR_RED, COLOR_RED);
  // init_pair(green, COLOR_GREEN, COLOR_GREEN);
  // init_pair(blue, COLOR_BLUE, COLOR_BLUE);

  init_pair(1, COLOR_GREEN, COLOR_BLACK);
  attron(COLOR_PAIR(1));

  // Define variables
  window w;
  w.fov = pi / 4.;
  w.w = 80;
  w.h = 40;

  player p;
  p.x = 1.5;
  p.y = 5;
  p.o = 0;

  while (true)
  {
    erase();
    float resp[w.w];
    for (int f = 0; f < w.w; f += 1)
    {
      double dir = p.o + (f * (w.fov / w.w));
      int hw = w.w / 2;
      float dist = castRay(w, p, dir);
      resp[f] = w.h / pow(dist, 1.2);
    }
    int ch[w.w][w.h];
    for (int x = 0; x < w.w; x += 1)
    {
      int pp = floor((w.h - resp[x]) / 2.);
      for (int y = 0; y < w.h; y += 1)
      {
        // if (y < pp || y > w.h - pp)
        // {
        //   ch[x][y] = ".";
        // }
        if (y < pp)
        {
          // attron(blue);
          // printw(".");
          // attroff(blue);
          ch[x][y] = 1;
        }
        else if (y > w.h - pp)
        {
          // attron(green);
          // printw(",");
          // attroff(green);
          ch[x][y] = 2;
        }
        else
        {
          ch[x][y] = 3;
          // attron(red);
          // printw("B");
          // attroff(red);
        }
      }
    }
    string s = "";
    for (int y = 0; y < w.h; y += 1)
    {
      for (int x = 0; x < w.w; x += 1)
      {
        // s += ch[x][y];
        switch (ch[x][y])
        {
        case 1:
        {
          // attron(blue);
          printw(" ");
          // attroff(blue);
          break;
        }
        case 2:
        {
          // attron(green);
          printw(",");
          // attroff(green);
          break;
        }
        case 3:
        {
          // attron(red);
          addch(ACS_BLOCK);
          // attroff(red);
          break;
        }
        }
      }
      // s += "\n";
      printw("\n");
    }
    // printw(s.c_str());
    refresh();
    int a = getch(); // D = 100; S = 115; A = 97; W = 119; ESC = 27
    if (a == 27)
    {
      attroff(COLOR_PAIR(1));
      endwin();
      return (0);
      break;
    }
    double md = p.o + (w.fov / 2.);
    if (a == 119)
    {
      fpoint n;
      n.y = p.y + 0.2 * sin(md);
      n.x = p.x + 0.2 * cos(md);
      point fn;
      fn.x = floor(n.x);
      fn.y = floor(n.y);
      if (map[fn.x][fn.y] != 1)
      {
        p.x = n.x;
        p.y = n.y;
      }
    }
    else if (a == 115)
    {
      fpoint n;
      n.y = p.y + (-0.2) * sin(md);
      n.x = p.x + (-0.2) * cos(md);
      point fn;
      fn.x = floor(n.x);
      fn.y = floor(n.y);
      if (map[fn.x][fn.y] != 1)
      {
        p.x = n.x;
        p.y = n.y;
      }
    }
    else if (a == 100)
    {
      p.o += 0.08 * pi;
    }
    else if (a == 97)
    {
      p.o -= 0.08 * pi;
    }
    else
    {
    }
  }
}