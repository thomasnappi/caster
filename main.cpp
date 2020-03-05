#include <string>
#include <ncurses.h>
#include <cmath>
using std::string;
using std::to_string;

// Define data types
struct player
{
  double x;
  double y;
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
double pi = 3.14159265358979323846;
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
double castRay(window w, player p, double d)
{
  bool hit = false;
  double len = 0;
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

  // Define variables
  window w;
  w.fov = pi / 4.;
  w.w = 80;
  w.h = 40;

  player p;
  p.x = 1.5;
  p.y = 5;
  p.o = 1. * pi;

  while (true)
  {
    erase();
    double resp[w.w];
    for (int f = 0; f < w.w; f += 1)
    {
      double dir = p.o + (f * (w.fov / w.w));
      int hw = w.w / 2;
      double dist = castRay(w, p, dir);
      resp[f] = w.h / pow(dist, 1.2);
    }
    string ch[w.w][w.h];
    for (int x = 0; x < w.w; x += 1)
    {
      int pp = floor((w.h - resp[x]) / 2.);
      for (int y = 0; y < w.h; y += 1)
      {
        if (y < pp || y > w.h - pp)
        {
          ch[x][y] = ".";
        }
        else
        {
          ch[x][y] = "B";
        }
      }
    }
    string s = "";
    for (int y = 0; y < w.h; y += 1)
    {
      for (int x = 0; x < w.w; x += 1)
      {
        s += ch[x][y];
      }
      s += "\n";
    }
    printw(s.c_str());
    refresh();
    int a = getch(); // D = 100; S = 115; A = 97; W = 119; ESC = 27
    if (a == 27)
    {
      endwin();
      return (0);
      break;
    }
    double md = p.o + (w.fov / 2);
    if (a == 119)
    {
      p.y += 0.2 * sin(md);
      p.x += 0.2 * cos(md);
    }
    else if (a == 115)
    {
      p.y -= 0.2 * sin(md);
      p.x -= 0.2 * cos(md);
    }
    else if (a == 100)
    {
      p.o += 0.2;
    }
    else if (a == 97)
    {
      p.o -= 0.2;
    }
    else
    {
    }
  }
}