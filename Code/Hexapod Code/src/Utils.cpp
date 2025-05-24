#include "Utils.h"

//------------Vectors---------------//

Vector2::Vector2() {
  x = 0;
  y = 0;
}

Vector2::Vector2(float newX, float newY) {
  x = newX;
  y = newY;
}

String Vector2::toString() {
  String xs = String(x);
  String ys = String(y);

  String ret = "(" + xs + "," + ys + ")";
  return ret;
}

float Vector2::magnitude() const {
  return sqrt(x * x + y * y);
}

// Function to normalize the vector
void Vector2::normalize() {
  float mag = magnitude();
  if (mag > 0) {
    x /= mag;
    y /= mag;
  }
}

Vector2 Vector2::operator+(Vector2 val) {
  // Implement the multiply operator
  Vector2 result;
  result.x = x + val.x;
  result.y = y + val.y;
  return result;
}

Vector2 Vector2::operator*(float val) {
  // Implement the multiply operator
  Vector2 result;
  result.x = x * val;
  result.y = y * val;
  return result;
}

Vector2 Vector2::operator*(Vector2 val) {
  // Implement the multiply operator
  Vector2 result;
  result.x = x * val.x;
  result.y = y * val.y;
  return result;
}

Vector2 Vector2::rotate(int angle, Vector2 pivot) {
  // Translate line so pivot point is at the origin
  x -= pivot.x;
  y -= pivot.y;

  // Rotate point by angle
  int x_rotated = x * cos(angle) - y * sin(angle);
  int y_rotated = x * sin(angle) + y * cos(angle);

  // Translate point back to original position
  x = x_rotated + pivot.x;
  y = y_rotated + pivot.y;
  return Vector2(x, y);
}

Vector3::Vector3() {
  x = 0;
  y = 0;
  z = 0;
}

Vector3::Vector3(float newX, float newY, float newZ) {
  x = newX;
  y = newY;
  z = newZ;
}

bool Vector3::operator!=(Vector3 val) {
  return (x != val.x || y != val.y || z != val.z);
}

bool Vector3::operator==(Vector3 val) {
  return (x == val.x && y == val.y && z == val.z);
}

Vector3 Vector3::operator*(float val) {
  return Vector3(x * val, y * val, z * val);
}

Vector3 Vector3::operator*(Vector3 val) {
  return Vector3(x * val.x, y * val.y, z * val.z);
}

Vector3 Vector3::operator/(Vector3 val) {
  return Vector3(x / val.x, y / val.y, z / val.z);
}

Vector3 Vector3::operator/(float val) {
  return Vector3(x / val, y / val, z / val);
}

Vector3 Vector3::operator+(Vector3 val) {
  return Vector3(x + val.x, y + val.y, z + val.z);
}

Vector3 Vector3::operator-(Vector3 val) {
  return Vector3(x - val.x, y - val.y, z - val.z);
}

String Vector3::toString() {
  String xs = String(x);
  String ys = String(y);
  String zs = String(z);

  String ret = "(" + xs + "," + ys + "," + zs + ")";
  return ret;
}

Vector3 Vector3::rotate(int angle, Vector2 pivot) {
  // Translate line so pivot point is at the origin
  if (angle == 0)
    return Vector3(x, y, z);

  x -= pivot.x;
  y -= pivot.y;
  float angleRad = radians(angle);

  // Rotate point by angle
  int x_rotated = x * cos(angleRad) - y * sin(angleRad);
  int y_rotated = x * sin(angleRad) + y * cos(angleRad);

  // Translate point back to original position
  x = x_rotated + pivot.x;
  y = y_rotated + pivot.y;

  return Vector3(x, y, z);
}

float Vector3::distanceTo(Vector3 v) {
  double dx = v.x - x;
  double dy = v.y - y;
  double dz = v.z - z;
  return sqrt(dx * dx + dy * dy + dz * dz);
}

//------------Matrix 3x3---------------//

Matrix3x3 makeRotationMatrix(float roll, float pitch, float yaw) {
  float cx = cos(roll), sx = sin(roll);
  float cy = cos(pitch), sy = sin(pitch);
  float cz = cos(yaw), sz = sin(yaw);

  Matrix3x3 R;
  R.m[0][0] = cz * cy;
  R.m[0][1] = cz * sy * sx - sz * cx;
  R.m[0][2] = cz * sy * cx + sz * sx;
  R.m[1][0] = sz * cy;
  R.m[1][1] = sz * sy * sx + cz * cx;
  R.m[1][2] = sz * sy * cx - cz * sx;
  R.m[2][0] = -sy;
  R.m[2][1] = cy * sx;
  R.m[2][2] = cy * cx;
  return R;
}

void printMatrix(const Matrix3x3& m) {
  Serial.println("Rotation Matrix:");
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      Serial.print(m.m[i][j], 4); // Print with 4 decimal places
      Serial.print("\t");
    }
    Serial.println();
  }
}

//------------Bezier Curves---------------//

Vector2 GetPointOnBezierCurve(vector<Vector2>& controlPoints, float t) {
  Vector2 pos;
  int totalPoints = controlPoints.size(); //total number of control points
  if (totalPoints == 0) return pos; // Return zero vector if no points are provided

  for (int i = 0; i < totalPoints; i++) {
    float b = binomialCoefficient(totalPoints - 1, i) * pow(1 - t, totalPoints - 1 - i) * pow(t, i);
    pos.x += b * controlPoints[i].x;
    pos.y += b * controlPoints[i].y;
  }

  return pos;
}

Vector3 GetPointOnBezierCurve(vector<Vector3>& controlPoints, float t) {
  Vector3 pos;
  int totalPoints = controlPoints.size(); //total number of control points
  if (totalPoints == 0) return pos; // Return zero vector if no points are provided

  for (int i = 0; i < totalPoints; i++) {
    float b = binomialCoefficient(totalPoints - 1, i) * pow(1 - t, totalPoints - 1 - i) * pow(t, i);
    pos.x += b * controlPoints[i].x;
    pos.y += b * controlPoints[i].y;
    pos.z += b * controlPoints[i].z;
  }

  return pos;
}

int binomialCoefficient(int n, int k) {
  int result = 1;

  // Calculate the binomial coefficient using the formula:
  // (n!) / (k! * (n - k)!)
  for (int i = 1; i <= k; i++) {
    result *= (n - (k - i));
    result /= i;
  }

  return result;
}

//------------LERPs---------------//
float lerp(float a, float b, float f) {
  return a * (1.0 - f) + (b * f);
}

Vector2 lerp(Vector2 a, Vector2 b, float f) {
  return Vector2(lerp(a.x, b.x, f), lerp(a.y, b.y, f));
}

Vector3 lerp(Vector3 a, Vector3 b, float f) {
  return Vector3(lerp(a.x, b.x, f), lerp(a.y, b.y, f), lerp(a.z, b.z, f));
}

//------------Math Stuff---------------//
float calculateHypotenuse(float x, float y) {
  float result = sqrt(pow(x, 2) + pow(y, 2));
  return result;
}

float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void print_value(String name, float value, bool newLine){
  Serial.print(name + ": ");

  if(newLine)Serial.println(value);
  else Serial.print(value);
  
}

void print_value(String name, String value, bool newLine){
  Serial.print(name + ": ");
  if(newLine)Serial.println(value);
  else Serial.print(value);
}

void print_value(String name, Vector2 value, bool newLine){
  Serial.print(name + ": ");
  if(newLine)Serial.println(value.toString());
  else Serial.print(value.toString());
}

void print_value(String name, Vector3 value, bool newLine){
  Serial.print(name + ": ");
  if(newLine)Serial.println(value.toString());
  else Serial.print(value.toString());
}

