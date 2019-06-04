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
	Vector2 operator - (const Vector2& v) const { return Vector2(x - v.x, y - v.y); }
	Vector2 operator * (const float s) const { return Vector2(s*x, s*y); }
	Vector2 operator * (const Vector2& v) const { return Vector2(x*v.x, y*v.y); }
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

class GameObject
{
public:
	Vector2 position;
	float radius;

	GameObject(float _x, float _y, float _radius) : position(Vector2(_x, _y)), radius(_radius) {}
	~GameObject(){}
};

class Checkpoint : public GameObject
{
public:
	int id;

	Checkpoint(int _id, float _x, float _y) : id(_id), GameObject(_x, _y, 600){}
	~Checkpoint(){}

};
class Pod : GameObject
{
private:
	const float slowingRadius = 2000;
public:
	Vector2 velocity;
	int thrust = 100;
	float angle;
	int nextCheckpointId;
	Pod* partnerPod;
	bool haveShield = false;

	Pod(float _x, float _y, float _vx, float _vy, float _angle, int _nextCheckpointId, Pod* _partner = nullptr) : GameObject(_x, _y, 400), velocity(Vector2(_vx, _vy)), angle(_angle), nextCheckpointId(_nextCheckpointId), partnerPod(_partner){}
	~Pod() {}

	float GetAngleToTarget(Vector2 _target)
	{
		float angleToTarget = atan2(_target.y, _target.x) - atan2(position.y, position.x);
		angleToTarget = angleToTarget * 180 / PI; //convert radian to degree

		if (angleToTarget < 0)
		{
			angleToTarget = 360 - (-angleToTarget);
		}
		return angleToTarget;
	}

	float GetRotationAngle(Vector2 _target)
	{
		float angleToTarget = GetAngleToTarget(_target);

		// We need to know if we should turn clockwise or anticlockwise 
		float clockwise, anticlockwise;
		
		clockwise = fmod(angleToTarget - this->angle, 360);
		anticlockwise = fmod(this->angle - angleToTarget, 360);

		if (clockwise < 0)
			clockwise += 360;

		if (anticlockwise < 0)
			anticlockwise += 360;

		// We keep the smallest angle
		if (clockwise <= anticlockwise)
			return clockwise;
		else
			return -anticlockwise; //negative angle to make pod rotate anticlockwise

	}

	void ApplyRotation(Vector2 _target)
	{
		float rotationToAdd = GetRotationAngle(_target);

		//We can rotate 18 degree per turn
		if (rotationToAdd < -18)
			rotationToAdd = -18;
		else if (rotationToAdd > 18)
			rotationToAdd = 18;

		//Set new rotation
		angle = fmod(angle + rotationToAdd, 360);
		if (angle < 0)
			angle += 360;
	}

	void ApplyThrust(int _thrust, Vector2 _target, vector<Checkpoint*> _checkPointArray)
	{
		if (haveShield) // can't thrust when shield activated
			return;
		
		float nextCheckpointDist = Distance(this->position, _checkPointArray[nextCheckpointId]->position);

		if (nextCheckpointDist <= slowingRadius) // Inside slowing radius
		{
			thrust = thrust * (nextCheckpointDist / slowingRadius);
		}
		else
			thrust = 100;

		Vector2 direction = _target - position;
		velocity = velocity + Normalize(direction) * _thrust;
	}

	void ApplyMovement(Vector2 _target)
	{
		// Update position
		position = position + velocity;

		// TODO: handle collisions

		// Friction
		velocity = velocity * 0.85;

		// Truncate speed
		velocity.x = trunc(velocity.x);
		velocity.y = trunc(velocity.y);

		// Round position
		position.x = round(position.x);
		position.y = round(position.y);
	}

	void UpdateNextCheckpoint(vector<Checkpoint*> _checkPointArray)
	{
		Checkpoint* nextCheckpoint = _checkPointArray[nextCheckpointId];

		if (Distance(nextCheckpoint->position, this->position) <= nextCheckpoint->radius)
		{
			nextCheckpointId = (nextCheckpointId + 1) % _checkPointArray.size();
		}
	}

	void Play(vector<Checkpoint*> _checkpoints)
	{
		Vector2 target = _checkpoints[nextCheckpointId]->position;

		ApplyRotation(target);
		ApplyThrust(thrust, target, _checkpoints);
		ApplyMovement(target);
		UpdateNextCheckpoint(_checkpoints);
	}

	void Output(vector<Checkpoint*> _checkpoints)
	{
		cout << _checkpoints[nextCheckpointId]->position.x << " " << _checkpoints[nextCheckpointId]->position.y << " " << thrust << endl;
	}
};


int main()
{
    int laps;
    cin >> laps; cin.ignore();
    int checkpointCount;
    cin >> checkpointCount; cin.ignore();

	vector<Checkpoint*> checkPointArray;
	Pod* pod1;
	Pod* pod2;
	Pod* opponent1;
	Pod* opponent2;

    for (int i = 0; i < checkpointCount; i++) {
        int checkpointX;
        int checkpointY;
        cin >> checkpointX >> checkpointY; cin.ignore();
		checkPointArray.push_back(new Checkpoint(i, checkpointX, checkpointY));
    }

    // game loop
    while (1) {
        for (int i = 0; i < 2; i++) {
            int x; // x position of your pod
            int y; // y position of your pod
            int vx; // x speed of your pod
            int vy; // y speed of your pod
            int angle; // angle of your pod
            int nextCheckPointId; // next check point id of your pod
            cin >> x >> y >> vx >> vy >> angle >> nextCheckPointId; cin.ignore();

			if (i == 0)
				pod1 = new Pod(x, y, vx, vy, angle, nextCheckPointId, pod2);
			else
				pod2 = new Pod(x, y, vx, vy, angle, nextCheckPointId, pod1);
        }
        for (int i = 0; i < 2; i++) {
            int x2; // x position of the opponent's pod
            int y2; // y position of the opponent's pod
            int vx2; // x speed of the opponent's pod
            int vy2; // y speed of the opponent's pod
            int angle2; // angle of the opponent's pod
            int nextCheckPointId2; // next check point id of the opponent's pod
            cin >> x2 >> y2 >> vx2 >> vy2 >> angle2 >> nextCheckPointId2; cin.ignore();

			if(i==0)
				opponent1 = new Pod(x2, y2, vx2, vy2, angle2, nextCheckPointId2, opponent2);
			else
				opponent2 = new Pod(x2, y2, vx2, vy2, angle2, nextCheckPointId2, opponent1);
        }

        // Write an action using cout. DON'T FORGET THE "<< endl"
        // To debug: cerr << "Debug messages..." << endl;


        // You have to output the target position
        // followed by the power (0 <= thrust <= 100)
        // i.e.: "x y thrust"
        //cout << "8000 4500 100" << endl;
        //cout << "8000 4500 100" << endl;
		pod1->Play(checkPointArray);
		pod2->Play(checkPointArray);

		pod1->Output(checkPointArray);
		pod2->Output(checkPointArray);

		delete pod1;
		delete pod2;
    }
}