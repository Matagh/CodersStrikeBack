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
	bool haveShield = false;

	Pod(float _x, float _y, float _vx, float _vy, float _angle, int _nextCheckpointId) : GameObject(_x, _y, 400), velocity(Vector2(_vx, _vy)), angle(_angle), nextCheckpointId(_nextCheckpointId){}
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

	void Play(vector<Checkpoint*> _checkpoints, vector<Pod*> _allPods)
	{
		Vector2 target = _checkpoints[nextCheckpointId]->position;

		HandleCollisions(_allPods);
		ApplyRotation(target);
		ApplyThrust(thrust, target, _checkpoints);
		ApplyMovement(target);
		//HandleShield(_allPods);
		UpdateNextCheckpoint(_checkpoints);
	}

	void Output(vector<Checkpoint*> _checkpoints)
	{
		if(this->haveShield)
			cout << _checkpoints[nextCheckpointId]->position.x << " " << _checkpoints[nextCheckpointId]->position.y << " SHIELD" << endl;
		else
			cout << _checkpoints[nextCheckpointId]->position.x << " " << _checkpoints[nextCheckpointId]->position.y << " " << thrust << endl;
	}

	bool CheckCollisionWithPod(Pod* _otherPod)
	{
		float distance = Distance(this->position, _otherPod->position);
		float collisionRadius = this->radius + _otherPod->radius +30; //30 = margin error

		if (distance <= collisionRadius) //means they are currently colliding;
		{
			cerr << "Yes there is collision !" << endl;
			return true;
		}

	}

	void ApplyBounce(Pod* _otherPod)
	{
		cerr << "Bounce !" << endl;
		float m1 = this->haveShield ? 10 : 1; // mass of thi pod
		float m2 = _otherPod->haveShield ? 10 : 1; // mass of other pod

		// Momentum conservation formula  : m1.v1 + m2.v2 = m1.v1' + m2.v2'
		// where v1' and v2' are velocity after impact
		// Here we know v1 and v2 and search for v1' and v2' ===> v1' = v1 * (m1-m2)/(m1+m2) +  v2 * 2*m2/(m1+m2)

		Vector2 finalVelocity1 = this->velocity*((m1 - m2) / (m1 + m2)) + _otherPod->velocity*(2 * m2 / (m1 + m2));
		Vector2 finalVelocity2 = this->velocity*(2 * m1 / (m1 + m2)) + _otherPod->velocity * ((m2 - m1) / (m1 + m2));

		// Know we compute impulse forces
		// F = mass * deltaVelocity / t		but here t=1
		Vector2 impulseForce1 = (this->velocity - finalVelocity1) * m1;
		Vector2 impulseForce2 = (_otherPod->velocity - finalVelocity2) * m2;

		//If impact norm < 120 we normalize it to 120
		if (Length(impulseForce1) < 120)
			impulseForce1 = Normalize(impulseForce1) * 120;

		if (Length(impulseForce2) < 120)
			impulseForce2 = Normalize(impulseForce2) * 120;

		// Then we apply impact forces to both velocities
		this->velocity = this->velocity + impulseForce1;
		_otherPod->velocity = _otherPod->velocity + impulseForce2;
	}

	void HandleCollisions(vector<Pod*> _allPods)
	{
		for (int i = 0; i < _allPods.size(); ++i)
		{
			if (_allPods[i] == this) // ignore self
				return;

			if (CheckCollisionWithPod(_allPods[i]))
			{
				cerr << "Collision with other pod !" << endl;
				if (i >= 2) // means collision with opponent
				{
					cerr << "Collision with opponent !" << endl;
					this->haveShield = true;
				}
				ApplyBounce(_allPods[i]);
			}
		}
	}

	void HandleShield(vector<Pod*> _allPods)
	{
		Vector2 futurePos = this->position + this->velocity;

		// We check if a collision will occur in next turn with ennemy pod
		for (int i = 2; i < _allPods.size(); i++)
		{
			Vector2 futureOpponentPos = _allPods[i]->position + _allPods[i]->velocity;
			if (Distance(futurePos, futureOpponentPos) < this->radius * 2) // *2 because all pods have same radius
			{
				cerr << "collision with opponent in next turn at : " << futurePos.x << " " << futurePos.y << endl;
				this->haveShield = true;
				return;
			}
		}

		haveShield = false;
	}
};


int main()
{
    int laps;
    cin >> laps; cin.ignore();
    int checkpointCount;
    cin >> checkpointCount; cin.ignore();

	vector<Checkpoint*> checkPointArray;
	vector<Pod*> playerPods;
	vector<Pod*> opponentPods;
	vector<Pod*> allPods;

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

			allPods.push_back(new Pod(x, y, vx, vy, angle, nextCheckPointId));
        }
        for (int i = 0; i < 2; i++) {
            int x2; // x position of the opponent's pod
            int y2; // y position of the opponent's pod
            int vx2; // x speed of the opponent's pod
            int vy2; // y speed of the opponent's pod
            int angle2; // angle of the opponent's pod
            int nextCheckPointId2; // next check point id of the opponent's pod
            cin >> x2 >> y2 >> vx2 >> vy2 >> angle2 >> nextCheckPointId2; cin.ignore();

			allPods.push_back(new Pod(x2, y2, vx2, vy2, angle2, nextCheckPointId2));
        }

        // Write an action using cout. DON'T FORGET THE "<< endl"
        // To debug: cerr << "Debug messages..." << endl;


        // You have to output the target position
        // followed by the power (0 <= thrust <= 100)
        // i.e.: "x y thrust"
        //cout << "8000 4500 100" << endl;
        //cout << "8000 4500 100" << endl;
		for (int i = 0; i < 2; i++)
		{
			allPods[i]->Play(checkPointArray, allPods);
			allPods[i]->Output(checkPointArray);
		}

		//for (int i = 0; i < allPods.size(); i++)
		//{
		//	delete allPods[i];
		//}
    }
}