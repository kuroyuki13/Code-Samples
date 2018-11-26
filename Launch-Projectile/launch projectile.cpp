#include <math.h>

struct vector3 // placeholder structure for vector3. would normaly use the libraries relevant for the project.
{
	float X, Y, Z;

	inline vector3d(void) {}
	inline vector3d(const float x, const float y, const float z)
	{
		X = x; Y = y; Z = z;
	}

	inline vector3 operator + (const vector3& A) const
	{
		return vector3d(X + A.X, Y + A.Y, Z + A.Z);
	}
	inline vector3 operator - (const vector3& A) const
	{
		return vector3d(X - A.X, Y - A.Y, Z - A.Z);
	}

	inline vector3 operator + (const float A) const
	{
		return vector3d(X + A, Y + A, Z + A);
	}
	inline vector3 operator - (const float A) const
	{
		return vector3d(X - A, Y - A, Z - A);
	}

	inline float Dot(const vector3d& A) const
	{
		return A.X*X + A.Y*Y + A.Z*Z;
	}
};

struct ProjectileInfo //structure that holds values to be returned after calculating the projectile info
{
	float Angle;
	float HorizontalDistance;
	float VerticalDistance;
};

ProjectileInfo CalculateProjectileAngle(vector3 _reciever, vector3 _origin_of_shot, float _launch_velocity)
{
	ProjectileInfo P;  // create holder for projectile information
	float G = 9.81 //gravity. normaly a game setting for gravity would be used.

	vector3 Distance = _reciever - _origin_of_shot; //calculate distance between shooter and target

	//store horizontal and verical distance in floats. Short names are used for mathematical equations. Will later be stored in P
	float X = Distance.X;
	float H = Distance.Y;

	// function for calculating angle is broken up in seperate parts for readability.
	// the formula for calculating the angle theta is:  theta = arctan ( (  (v^2 +- (sqrt ( v^4 - g(gx^2 + 2yv^2)) / gx )

	// squared velocity is needed multiple times, so its pre-calculated.
	float SqV = _launch_velocity*_launch_velocity; 
	// this is the part 2yv^2 , y is the height, V velocity. thus variable name HV
	float HV = (2 * H)*SqV; 
	// this is the quared part under the root of the equation.
	float square_Root = sqrt(powf(HV, 4) - ((G * (X*X) * G)); 
	// the last parts of the equation, both + and -, since there are two possible angles to throw at
	float angle1 = atan((SqV + square_Root) / (G + X));
	float angle2 = atan((SqV - square_Root) / (G + X));
	// want the lowest angle to throw at, since that will reach the target the fastest
	float Shoot_angle = 999;
	if (abs(angle1) <= abs(angle2))
	{
		Shoot_angle = angle1;
	}
	else
	{
		Shoot_angle = angle2;
	}

	if (Shoot_angle = 999)
	{
		//throw execption, something went wrong
	}

	P.Angle = Shoot_angle;
	P.HorizontalDistance = X;
	P.VerticalDistance = H;

	return P;

}