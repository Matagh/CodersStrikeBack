#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <vector>

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

bool ShouldBoost(const Vector2& podPos, const Vector2& opponentPos, const Vector2& nextCheckpointPos, const bool& isBoostAvailable, const float& minBoostRadius, const float& nextCheckpointAngle, const bool& is2ndLap)
{
	if (isBoostAvailable)
	{

		float podToCheckpointDistance = Distance(podPos, nextCheckpointPos);
		float opponentToCheckpointDistance = Distance(opponentPos, nextCheckpointPos);
		float podToOpponentDistance = Distance(podPos, opponentPos);

		// IF opponent is far enough
		// AND opponent is behind us (closer to checkpoint than pod)
		// AND pod is heading straight to checkpoint
		// AND race is at 2nd lap
		if (podToOpponentDistance >= minBoostRadius && podToCheckpointDistance > opponentToCheckpointDistance && nextCheckpointAngle == 0 && is2ndLap)
		{
			return true;
		}
	}
	return false;
}

void Check2ndLap(bool& _is2ndLap, const Vector2& nextCheckpointPosition, vector<Vector2>& _checkpointArray)
{
	if (_is2ndLap)
		return;

	Vector2 newCheckpoint(nextCheckpointPosition);

	if (_checkpointArray.empty()) // means  race just started
	{
		_checkpointArray.push_back(newCheckpoint);
	}
	else if(_checkpointArray.back() != newCheckpoint) // new checkpoint is different from previous one in array
	{
		if (newCheckpoint == _checkpointArray.front()) // means 1st lap is over
		{
			_is2ndLap = true;
		}
		else
			_checkpointArray.push_back(newCheckpoint);
	}
	return;
}

bool CollisionInNextTurn(Vector2 _futurePodPos, Vector2 _futureOpponentPos, float _fieldRadius)
{
}

int main()
{
	const int LimitAngle = 90;
	const int MinSteerAngle = 1;
	const float MinBoostRadius = 2000;
	const float SlowDownRadius = 2000; 
	const float forceFieldRadius = 400 + 30; // 400 + error margin
	const int MaxTurnsMotorDisabled = 3;

	vector<Vector2> checkpointArray;
	bool is2ndLap = false;
	bool isBoostAvailable = true;
	bool isMotorInactive = false;
	int turnsSinceMotorDisabled = 0;

	Vector2 previousPodPos = Vector2(0, 0);
	Vector2 previousOpponentPos = Vector2(0, 0);

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

		Vector2 podVelocity = podPosition - previousPodPos / 1;		//t = 1 for one game turn
		Vector2 opponentVelocity = opponentPosition - previousOpponentPos / 1;

		float opponentThrust = Distance(opponentPosition, previousOpponentPos); // v = d/t;  here t=1 turn

		cerr << "Opponent thrust = " << opponentThrust << endl;
		Vector2 futurePodPos = podPosition + Normalize(podVelocity) * thrust;
		Vector2 futureOpponentPos = opponentPosition + opponentVelocity;

		if (nextCheckpointAngle <= -MinSteerAngle || nextCheckpointAngle >= MinSteerAngle)
		{
			// Seek correct target
			Vector2 wantedVelocity= nextCheckpointPosition - podPosition;

			if (nextCheckpointDist < SlowDownRadius)
			{
				//Inside slowing radius
				thrust = thrust * (nextCheckpointDist / SlowDownRadius);
				
				cerr << "In Slowing radius" << endl;
			}
			else //Outside slowing radius
			{
				thrust = 100;
			}
			
			wantedVelocity = Normalize(wantedVelocity) * thrust;
			Vector2 steeringVector = wantedVelocity - podVelocity;
			nextCheckpointX += steeringVector.x;
			nextCheckpointY += steeringVector.y;

			// Thrusting down (angle condition )
			if (nextCheckpointAngle <= -LimitAngle || nextCheckpointAngle >= LimitAngle)
			{
				thrust = 0;
			}
		}

		// Handle boost
		Check2ndLap(is2ndLap, nextCheckpointPosition, checkpointArray);
		cerr << "Boost available : " << isBoostAvailable << endl;
		cerr << "2nd Lap ? " << is2ndLap << endl;
		if (ShouldBoost(podPosition, opponentPosition, nextCheckpointPosition, isBoostAvailable, MinBoostRadius, nextCheckpointAngle, is2ndLap))
		{
			useBoost = true;
			isBoostAvailable = false;
			cerr << "BOOST ACTIVTED" << endl;
		}

		// Handle Shield
		cerr << "Distance to opponent : " << Distance(podPosition, opponentPosition) << endl;
		cerr << "Future distance to opponent : " << Distance(futurePodPos, futureOpponentPos) << endl;
		if (Distance(futurePodPos, futureOpponentPos) < forceFieldRadius * 2)
		{
			cerr << "SHIELD ACTIVATED" << endl;
			useShield = true;
			//isMotorInactive = true;
		}

        // OUTPUT
		cout << nextCheckpointX << " " << nextCheckpointY << " ";
		if (useBoost)
			cout << "BOOST" << endl;
		else if (useShield)
			cout << "SHIELD" << endl;
		else
			cout << thrust << endl;

		previousPodPos = Vector2(x, y);
		previousOpponentPos = Vector2(opponentX, opponentY);
    }
}