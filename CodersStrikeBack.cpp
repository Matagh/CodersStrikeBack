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

/* Vectors Arithmetic methods*/
float Dot(const Vector2& u, const Vector2& v) {
	return u.x * v.x + u.y * v.y;
}

float Length(const Vector2& v) {
	return sqrt(Dot(v, v));
}

float Distance(const Vector2& u, const Vector2& v) {
	return sqrt(pow(v.x - u.x, 2) + pow(v.y - u.y, 2));
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

bool ShouldBoost(Vector2 podPos, Vector2 opponentPos, Vector2 nextCheckpointPos, bool isBoostAvailable, float minBoostRadius, float nextCheckpointAngle)
{
	if (isBoostAvailable)
	{

		float podToCheckpointDistance = Distance(podPos, nextCheckpointPos);
		float opponentToCheckpointDistance = Distance(opponentPos, nextCheckpointPos);
		float podToOpponentDistance = Distance(podPos, opponentPos);

		// IF opponent is far enough
		// AND opponent is behind us (closer to checkpoint than pod)
		// AND pod is heading straight to checkpoint
		if (podToOpponentDistance >= minBoostRadius && podToCheckpointDistance > opponentToCheckpointDistance && nextCheckpointAngle == 0) //Check if distance is high enough to use boost and if opponent is closer to the next checkpoint
		{
			return true;
		}
	}
	return false;
}
int main()
{
	const int LimitAngle = 90;
	const int MinSteerAngle = 1;
	const float MinBoostRadius = 2000;
	const float forceFieldRadius = 400 + 75; // 400 + error margin
	const int MaxTurnsMotorDisabled = 3;

	bool isBoostAvailable = true;
	bool isMotorInactive = false;
	int turnsSinceMotorDisabled = 0;

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

		Vector2 podPosition = Vector2(x, y);
		Vector2 opponentPosition = Vector2(opponentX, opponentY);
		Vector2 nextCheckpointPosition = Vector2(nextCheckpointX, nextCheckpointY);

		int thrust = 100;
		bool useBoost = false;
		bool useShield = false;

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

		// Handle boost
		cerr << "Boost available : " << isBoostAvailable << endl;
		if (ShouldBoost(podPosition, opponentPosition, nextCheckpointPosition, isBoostAvailable, MinBoostRadius, nextCheckpointAngle))
		{
			useBoost = true;
			isBoostAvailable = false;
			cerr << "BOOST ACTIVTED" << endl;
		}

		// Handle Shield
		if (isMotorInactive)
		{
			if (turnsSinceMotorDisabled < 3)
			{
				turnsSinceMotorDisabled++;
				cerr << "Motor disable since " << turnsSinceMotorDisabled << " turns" << endl;
			}
			else
			{
				turnsSinceMotorDisabled = 0; // reset
				isMotorInactive = false;
			}
		}
		float podToOpponentDist = Distance(podPosition, opponentPosition);
		cerr << "Distance to opponent : " << podToOpponentDist << endl;
		if (podToOpponentDist <= forceFieldRadius * 2 && !isMotorInactive)
		{
			cerr << "SHIELD ACTIVATED" << endl;
			useShield = true;
			isMotorInactive = true;
		}

        // OUTPUT
		cout << nextCheckpointX << " " << nextCheckpointY << " ";
		if (useBoost)
			cout << "BOOST" << endl;
		else if (useShield)
			cout << "SHIELD" << endl;
		else
			cout << thrust << endl;
    }
}