#pragma once

#include "Arduino.h"

//------------Macros---------------//
#define every(interval)                                                            \
  static uint32_t __every__##interval = millis();                                  \
  if (millis() - __every__##interval >= interval && (__every__##interval = millis()))

//------------Vectors---------------//
class Vector2 {
public:
  float x;
  float y;
  // constructor
  Vector2();
  Vector2(float newX, float newY);

  String toString();
  float magnitude() const;

  // Function to normalize the vector
  void normalize();

  Vector2 operator+(Vector2 val);
  Vector2 operator*(float val);
  Vector2 operator*(Vector2 val);
  Vector2 rotate(int angle, Vector2 pivot);
};

class Vector3 {
public:
  float x;
  float y;
  float z;
  // constructor
  Vector3();
  Vector3(float newX, float newY, float newZ);

  bool operator!=(Vector3 val);
  bool operator==(Vector3 val);

  Vector3 operator*(float val);
  Vector3 operator*(Vector3 val);
  Vector3 operator/(Vector3 val);
  Vector3 operator/(float val);
  Vector3 operator+(Vector3 val);
  Vector3 operator-(Vector3 val);

  String toString();
  Vector3 rotate(int angle, Vector2 pivot);
  float distanceTo(Vector3 v);
};

//------------Bezier Curves---------------//
Vector2 GetPointOnBezierCurve(Vector2* points, int numPoints, float t);
Vector3 GetPointOnBezierCurve(Vector3* points, int numPoints, float t);
int binomialCoefficient(int n, int k);

//------------LERPs---------------//
float lerp(float a, float b, float f);
Vector2 lerp(Vector2 a, Vector2 b, float f);
Vector3 lerp(Vector3 a, Vector3 b, float f);

//------------Math Stuff---------------//
float calculateHypotenuse(float x, float y);
float mapFloat(float x, float in_min, float in_max, float out_min, float out_max);

void print_value(String name, float value, bool newLine);
void print_value(String name, String value, bool newLine);
void print_value(String name, Vector3 value, bool newLine);

