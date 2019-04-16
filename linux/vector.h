// Vector.h
// By Barton Stander
// April 2001
// For CS 3600 Graphics Programming

/*Implement a Point2 class, that holds a 2D point comprised of 2 floats.*/
/*
/*Implement a Bezier class, that consists of and array of 4 Point2 objects.*/
#pragma once
class Point2 {
public:
	Point2();
	Point2(double xin, double yin);
	double getX() const { return x; };
	double getY() const { return y; };
	void setX(double xin) { x = xin; };
	void setY(double yin) { y = yin; };
	Point2 operator*(const double &rhs);
	Point2 operator+(const Point2 &rhs);

	double x, y;
};



class Bezier {
public:
	Bezier() {}

	Bezier(Point2 p0, Point2 p1, Point2 p2, Point2 p3) {
		points[0] = p0; points[1] = p1; points[2] = p2; points[3] = p3; radius = 5;
	}
	/*Point2 Evaluate(float t); // given parameter t between 0 and 1, finds the interpolated point.*/
	Point2 Evaluate(float t); // given parameter t between 0 and 1, finds the interpolated point.
	void DrawCurve(); // draws the curve by approximating it at about 20 evaluation points.
	void DrawControlPoints(); // draws the 4 control points as circles.

	/*Helpers*/
	int IsPicked(float x, float y);

	Point2 points[4];
	double color[3];
	int radius = 5;
};

class Point3
{
public:
	float p[3];
};

class Point4
{
public:
	float p[4];
};

class Vector3
{
public:
	float v[3];
	float Length(); // return vectors length
	float Normalize(); // normalize vector and return length;
};

Vector3 operator-(const Point3 &head, const Point3 &tail);
Vector3 operator-(const Point4 &head, const Point3 &tail);
Vector3 operator*(const Vector3 & v, float s);
Vector3 operator*(float s, const Vector3 & v);
Point3 operator+(const Point3 & p, const Vector3 & v);
Point3 operator+(const Vector3 & v, const Point3 & p);
Point3 operator-(const Point3 & p, const Vector3 & v);
Vector3 CrossProduct(const Vector3 &v1, const Vector3 &v2);
float DotProduct(const Vector3 &v1, const Vector3 &v2);
Point3 AffineSum(const Point3 &start, const Point3 & end, float t);
float DistanceBetweenPoints(const Point3 & p1, const Point3 & p2);
Vector3 operator+(const Vector3 & v1, const Vector3 & v2);

