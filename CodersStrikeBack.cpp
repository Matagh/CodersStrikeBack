#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>

using namespace std;
#define PI 3.14159265
/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/
struct Vector2
{
	float x;
	float y;

	Vector2(float x, float y) : 
		x(x), y(y) {}

	// Arithmetic operators
	Vector2 operator + (const Vector2& v) const { return Vector2(x + v.x, y + v.y); }
	Vector2 operator - (const Vector2& v) const	{ return Vector2(x - v.x, y - v.y); }
	Vector2 operator * (const float s) const { return Vector2(s*x, s*y); }
	Vector2 operator * (const Vector2& v) const	{ return Vector2(x*v.x, y*v.y); }
	Vector2 operator / (const float s) const {
		float r = 1 / s;
		return *this * r;
	}

	bool operator == (const Vector2& v) { return (x == v.x) && (y == v.y); }
	bool operator != (const Vector2& v) { return (x != v.x) || (y != v.y); }
};

/* Arithmetic methods*/
float Dot(const Vector2& u, const Vector2& v) {
	return u.x * v.x + u.y * v.y;
}

float Length(const Vector2& v) {
	return sqrt(Dot(v, v));
}

Vector2 Normalize(const Vector2& v) {
	return v / Length(v);
}

Vector2 Rotate(const Vector2& v, float angle) {
	float rad = angle * PI / 180;
	double cosAngle = cos(rad);
	double sinAngle = sin(rad);

	return Vector2(v.x * cosAngle - v.y * sinAngle, v.y * cosAngle + v.x * sinAngle);
}
int main()
{
	const int LimitAngle = 90;
	const int MinSteerAngle = 1;

    // game loop
    while (1) {
        int x;
        int y;
        int nextCheckpointX; // x position of the next check point
        int nextCheckpointY; // y position of the next check point
        int nextCheckpointDist; // distance to the next checkpoint
        int nextCheckpointAngle; // angle between your pod orientation and the direction of the next checkpoint
        cin >> x >> y >> nextCheckpointX >> nextCheckpointY >> nextCheckpointDist >> nextCheckpointAngle; cin.ignore();
        int opponentX;
        int opponentY;
        cin >> opponentX >> opponentY; cin.ignore();

		int thrust = 100;

		if (nextCheckpointAngle <= -MinSteerAngle || nextCheckpointAngle >= MinSteerAngle)
		{
			// Seek correct target
			Vector2 wantedDirection(nextCheckpointX - x, nextCheckpointY - y);
			wantedDirection = Normalize(wantedDirection);

			Vector2 currentDirection = Rotate(wantedDirection, -nextCheckpointAngle);
			currentDirection = Normalize(currentDirection);

			Vector2 steeringDirection = wantedDirection - currentDirection;
			steeringDirection = Normalize(steeringDirection) * 100;

			nextCheckpointX += steeringDirection.x;
			nextCheckpointY += steeringDirection.y;

			// Thrusting down
			if (nextCheckpointAngle <= -LimitAngle || nextCheckpointAngle >= LimitAngle)
			{
				thrust = 0;
			}
		}
        // You have to output the target position
        // followed by the power (0 <= thrust <= 100)
        // i.e.: "x y thrust"
        cout << nextCheckpointX << " " << nextCheckpointY << " " << thrust << endl;
    }
}