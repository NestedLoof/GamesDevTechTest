// Written by Luther Williamson 2024

#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <numbers>
using namespace std;

class Point {
public:
	int x, y, num;
	string direction;
};

vector<Point> GetPointsFromFile();
vector<Point> VisiblePoints(int num, float angle, float distance);
float GetStartingAngle(string direction);
void DisplayVisiblePoints(vector<Point>& visiblePoints);
void PromptStartAgain();
bool IsPointVisible(Point givenPoint, Point pointToCheck, float maxDistance, bool rangeCrossesZero, float minAngle, float maxAngle);

int main()
{
	//Get the point number, angle, and distance from the user
	int givenNum;
	cout << "Please enter the point number (between 1 and 20 inclusive): ";
	cin >> givenNum;
	while (givenNum < 1 || givenNum > 20 || cin.fail())
	{
		cin.clear();
		cin.ignore(256, '\n');
		cout << "Invalid input. Please enter the point number (between 1 and 20 inclusive): ";
		cin >> givenNum;
	}

	float givenAngle;
	cout << "Please enter the angle (between 0 and 180 exclusive): ";
	cin >> givenAngle;
	while (givenAngle <= 0 || givenAngle >= 180 || cin.fail())
	{
		cin.clear();
		cin.ignore(256, '\n');
		cout << "Invalid input. Please enter the angle (between 0 and 180 exclusive): ";
		cin >> givenAngle;
	}

	float givenDistance;
	cout << "Please enter the distance (between 0 and 50 inclusive): ";
	cin >> givenDistance;
	while (givenDistance < 0 || givenDistance > 50 || cin.fail())
	{
		cin.clear();
		cin.ignore(256, '\n');
		cout << "Invalid input. Please enter the distance (between 0 and 50 inclusive): ";
		cin >> givenDistance;
	}

	// find and display all of the points visible to the given point
	vector<Point> visiblePoints = VisiblePoints(givenNum, givenAngle, givenDistance);
	DisplayVisiblePoints(visiblePoints);
	PromptStartAgain();
	return 0;
}

void PromptStartAgain()
{
	cout << endl;
	cout << "Try another point? (y/n): ";
	char tryAgain;
	cin >> tryAgain;
	while (tryAgain != 'y' && tryAgain != 'n')
	{
		cout << "Invalid input. Try another point? (y/n): ";
		cin >> tryAgain;
	}

	if (tryAgain == 'y')
	{
		main();
	}
}

void DisplayVisiblePoints(vector<Point>& visiblePoints)
{
	cout << "Visible Points: " << endl;

	for (int i = 0; i < visiblePoints.size(); i++)
	{
		cout << "(" << visiblePoints[i].x << "," << visiblePoints[i].y << "," << visiblePoints[i].num << "," << visiblePoints[i].direction << ")" << endl;
	}
}

vector<Point> GetPointsFromFile()
{
	string fileName = "Points.txt";
	cout << "Reading in " << fileName << endl;

	ifstream file(fileName);

	if (file.is_open())
	{
		vector<Point> points = vector<Point>{};
		int x, y, num;
		string direction;

		int i = 0;
		while (file >> x >> y >> num >> direction)
		{
			points.insert(points.begin() + i, Point());
			points[i].x = x;
			points[i].y = y;
			points[i].num = num;
			points[i].direction = direction;
			i++;
		}

		file.close();
		return points;
	}
	else
	{
		cout << "Error: File not found" << endl;
	}

	return vector<Point>{};
}

vector<Point> VisiblePoints(int num, float angle, float distance)
{
	// get the points from the file
	vector<Point> points = GetPointsFromFile();
	if (points.empty())
	{
		// couldn't open file
		return vector<Point>{};
	}

	// find the point that the user is interested in
	Point givenPoint;
	for (int i = 0; i < points.size(); i++)
	{
		if (points[i].num == num)
		{
			givenPoint = points[i];
			break;
		}
	}

	if (givenPoint.num < 0)
	{
		cout << "Error: Point not found" << endl;
		return vector<Point>{};
	}

	// find the starting direction of the given point
	float startingAngle = GetStartingAngle(givenPoint.direction);
	if (startingAngle == -1)
	{
		cout << "Error: Invalid direction" << endl;
		return vector<Point>{};
	}

	// find the minimum and maximum angles in degrees from 0 to 360
	bool rangeCrossesZero = startingAngle < angle;
	float minAngle = fmod(startingAngle - angle + 360, 360);
	float maxAngle = fmod(startingAngle + angle + 360, 360);

	vector<Point> visiblePoints = vector<Point>{};
	int visiblePointsIndex = 0;

	// go through each point and add to the array if visible
	for (int i = 0; i < points.size(); i++)
	{
		// don't include the given point
		if (points[i].num == num)
		{
			continue;
		}

		 if (IsPointVisible(givenPoint, points[i], distance, rangeCrossesZero, minAngle, maxAngle))
		 {
			visiblePoints.insert(visiblePoints.begin() + visiblePointsIndex, points[i]);
			visiblePointsIndex++;
		}
	}

	return visiblePoints;
}

float GetStartingAngle(string direction)
{
	// East is 0 degrees, since it lies along the x-axis (arctan(0) = 0). The angle increases in the counter-clockwise direction.
	if (direction == "East")
	{
		return 0;
	}
	else if (direction == "North")
	{
		return 90;
	}
	else if (direction == "West")
	{
		return 180;
	}
	else if (direction == "South")
	{
		return 270;
	}
	else
	{
		return -1;
	}
}

bool IsPointVisible(Point givenPoint, Point pointToCheck, float maxDistance, bool rangeCrossesZero, float minAngle, float maxAngle)
{
	// check distance first using Pythagorus' theorem...
	float distanceToPoint = sqrt(pow(pointToCheck.x - givenPoint.x, 2) + pow(pointToCheck.y - givenPoint.y, 2));
	if (distanceToPoint > maxDistance)
	{
		return false;
	}

	// ...then angle using arctangent(dy/dx). Both must be within the given bounds for the point to be visible.
	float angleToPointRadians = atan2(pointToCheck.y - givenPoint.y, pointToCheck.x - givenPoint.x);
	float angleToPoint = fmod((angleToPointRadians * 180 / M_PI) + 360, 360);

	if (rangeCrossesZero)
	{
		return (0 <= angleToPoint && angleToPoint <= maxAngle)
			|| (minAngle <= angleToPoint && angleToPoint <= 360);
	}
	else
	{
		return minAngle <= angleToPoint && angleToPoint <= maxAngle;
	}
}
