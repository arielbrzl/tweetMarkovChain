//
// Created by ariel on 25/04/2022.
//

#ifndef _EXP_H_
#define _EXP_H_
#include <iostream>
#include <string>

struct Point
{
    // Note - struct so all is public by default

    // ctr - set point coordinates to (x,y)
    Point(int x, int y)
        : _x(x), _y(y) // member initalizer list
    {}

    int get_x() const
    {
      return _x;
    }

    int get_y() const
    {
      return _y;
    }

    int _x, _y; // class members (per instance)
};

// Add b to a and return a refernce to a.
// Note a is reference, b is a const ref
Point add(Point a ,const Point b )
{
  a._x += b._x;
  a._y += b._y;
  return a;
}

int main() {
  Point p1(2,3);
  Point p2(4,5);
  Point p3(0,1);
  add(add(p1,p2), p3); // p1 should be (6,9)
  std::cout << "x = " << add(p1, p2).get_x() // p1 should be (10,14)
            << " ; y = " << p1.get_y() << std::endl;
}
#endif //_EXP_H_
