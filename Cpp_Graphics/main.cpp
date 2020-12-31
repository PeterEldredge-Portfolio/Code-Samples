// This was my final assignment for my CS430 (Into to Computer Graphics) course.
// It takes in up to 3 SMF files along with a bunch of command line options, and outputs a ppm (needs to be piped).
// The project is not my own solution. It is essentially my implementation of several well established algorithms.
// I am proud of this assignment, although it is not my best work, and if I go back there are several things I would do differently.
// The project is contained to this one file, which is not ideal, even though it is well organized.
// I also want to clean up some of the algorithms when I get a chance, specifically the Bresenham Line Drawing.
// The reason I am showing this even though it is not my absolute best work, is that it is the most substantial
// c++ project I have done, and shows a level of at least remote competance with the language (I hope).
// To see what I consider more quality and functional code, check out the C# code I have included.

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <math.h>
#include <algorithm>
#include <Eigen/Dense> //Eigen Main Page: http://eigen.tuxfamily.org/index.php?title=Main_Page

//File Processing Reference: https://studiofreya.com/2015/09/12/read-and-write-to-file-in-c/#:~:text=C%2B%2B%20read%20file.%20C%2B%2B%20uses%20file%20streams%20for,to%20first%20attach%20the%20stream%20to%20a%20file.
//GeneralizedBresenham Reference: https://www.cs.drexel.edu/~david/Classes/CS430/Lectures/L-02_Lines.pdf
//CohenSutherland Reference: https://www.geeksforgeeks.org/line-clipping-set-1-cohen-sutherland-algorithm/
//SutherlandHodgman Reference: https://www.geeksforgeeks.org/polygon-clipping-sutherland-hodgman-algorithm-please-change-bmp-images-jpeg-png/
//Scan Line Filling Reference: https://www.geeksforgeeks.org/scan-line-polygon-filling-using-opengl-c/

//Input
std::string FILE_NAME_1 = "bound-sprellpsd.smf";
std::string FILE_NAME_2 = "";
std::string FILE_NAME_3 = "";

int CURRENT_FILE = 0;

float SCALE_FACTOR = 1.0f;

float ROTATION_DEGREES = 0.0f;
float ROTATION_RADIANS = 0.0f;

float X_TRANSLATION = 0.0f;
float Y_TRANSLATION = 0.0f;

//Worldwindow
float WW_X_LOWER_BOUND = 0.0f;
float WW_Y_LOWER_BOUND = 0.0f;
float WW_X_UPPER_BOUND = 250.0f;
float WW_Y_UPPER_BOUND = 250.0f;

//Viewport
float VP_X_LOWER_BOUND = 0.0f;
float VP_Y_LOWER_BOUND = 0.0f;
float VP_X_UPPER_BOUND = 500.0f;
float VP_Y_UPPER_BOUND = 500.0f;

//Projection Reference Point
float PRP_U = 0.0f;
float PRP_V = 0.0f;
float PRP_N = 1.0f;

//View Reference Point
float VRP_X = 0.0f;
float VRP_Y = 0.0f;
float VRP_Z = 0.0f;

//View Plane Normal
float VPN_X = 0.0f;
float VPN_Y = 0.0f;
float VPN_Z = -1.0f;

//View Up Vector
float VUP_X = 0.0f;
float VUP_Y = 1.0f;
float VUP_Z = 0.0f;

//VRC Window
float VRC_U_MIN = -0.7f;
float VRC_V_MIN = -0.7f;
float VRC_U_MAX = 0.7f;
float VRC_V_MAX = 0.7f;

//Front/Back clipping
float CLIP_FRONT = 0.6f;
float CLIP_BACK = -0.6f;

float CLIP_FRONT_POST_PROJECTION = 0.0f;
float CLIP_BACK_POST_PROJECTION = -1.0f;
float CLIP_DIFF_POST_PROJECTION = 1.2f;

//Projection
bool IS_PARALLEL_PROJECTION = false;

//Resolution
int X_RESOLUTION = 501;
int Y_RESOLUTION = 501;

//Internal
float MAX_RGB_VAL = 255.0f;
std::string PIXEL_SPACER = "   ";

int COLUMNS = 0;
int ROWS = 0;

//Constant
const std::string POSTSCRIPT_BEGIN = "%%%BEGIN";
const std::string POSTSCRIPT_END = "%%%END";

const std::string POSTSCRIPT_MOVETO = "moveto";
const std::string POSTSCRIPT_LINETO = "lineto";
const std::string POSTSCRIPT_STROKE = "stroke";

const float PI = 3.1415926535;

//Region Codes
const int INSIDE = 0; // 0000
const int LEFT = 1;	  // 0001
const int RIGHT = 2;  // 0010
const int BOTTOM = 4; // 0100
const int TOP = 8;	  // 1000

#pragma region Classes

class Pixel
{
public:
	Pixel()
	{
		Z = -1.0f;

		R = 0;
		G = 0;
		B = 0;
	}

	Pixel(float z, int r, int g, int b)
	{
		Z = z;

		R = r;
		G = g;
		B = b;
	}

	std::string ToString()
	{
		std::string str;

		if(R < 100)
		{
			str += " ";
		}
		if(R < 10)
		{
			str += " ";
		}

		str += std::to_string(R);
		str += " ";

		if (G < 100)
		{
			str += " ";
		}
		if (G < 10)
		{
			str += " ";
		}

		str += std::to_string(G);
		str += " ";

		if (B < 100)
		{
			str += " ";
		}
		if (B < 10)
		{
			str += " ";
		}

		str += std::to_string(B);

		return str;
	}

	float Z;
	int R, G, B;
};

class Point
{
public:
	Point()
	{
		X = 0;
		Y = 0;
		Z = 0;
	}

	Point(int x, int y)
	{
		X = x;
		Y = y;
		Z = 0;
	}

	Point(int x, int y, int z)
	{
		X = x;
		Y = y;
		Z = z;
	}

	int X, Y, Z;
};

class FloatPoint
{
public:
	FloatPoint()
	{
		X = 0;
		Y = 0;
		Z = 0;
	}

	FloatPoint(float x, float y)
	{
		X = x;
		Y = y;
		Z = 0;
	}

	FloatPoint(float x, float y, float z)
	{
		X = x;
		Y = y;
		Z = z;
	}

	float X, Y, Z;
};

class Line
{
public:
	Line()
	{
		A = Point();
		B = Point();
	}

	Line(Point a, Point b)
	{
		A = a;
		B = b;
	}

	std::string ToString()
	{
		return "(" + std::to_string(A.X) + ", " + std::to_string(A.Y) + ") (" + std::to_string(B.X) + ", " + std::to_string(B.Y) + ")";
	}

	Point A, B;
};

class FloatLine
{
public:
	FloatLine()
	{
		A = FloatPoint();
		B = FloatPoint();
	}

	FloatLine(FloatPoint a, FloatPoint b)
	{
		A = a;
		B = b;
	}

	std::string ToString()
	{
		return "(" + std::to_string(A.X) + ", " + std::to_string(A.Y) + ", " + std::to_string(A.Z) + ") (" + std::to_string(B.X) + ", " + std::to_string(B.Y) + ", " + std::to_string(B.Z) + ")";
	}

	FloatPoint A, B;
};

class Polygon
{
public:
	Polygon()
	{

	}

	Polygon(std::vector<FloatPoint> points)
	{
		Points = points;
	}

	Polygon(std::vector<FloatLine> lines)
	{
		for (int i = 0; i < lines.size(); i++)
		{
			Points.push_back(lines[i].A);
		}
	}

	std::vector<FloatLine> GetLines()
	{
		std::vector<FloatLine> lines;

		for (int i = 0; i < Points.size(); i++)
		{
			int k = (i + 1) % Points.size();

			lines.push_back(FloatLine(Points[i], Points[k]));
		}

		return lines;
	}

	std::string ToString()
	{
		std::vector<FloatLine> Lines = GetLines();
		std::string str;

		for (int i = 0; i < Lines.size(); i++)
		{
			if (i != 0)
				str += "\n";

			str += Lines[i].ToString();
		}

		return str;
	}

	std::vector<FloatPoint> Points;
};

class EdgeBucket
{
public:
	EdgeBucket()
	{
		YMax = 0;
		XOfYMin = 0;
		SlopeInverse = 0;
		BaseLine = FloatLine();
	}

	EdgeBucket(int yMax, float xOfYMin, float slopeInverse, FloatLine baseLine)
	{
		YMax = yMax;
		XOfYMin = xOfYMin;
		SlopeInverse = slopeInverse;
		BaseLine = baseLine;
	}

	int YMax;
	float XOfYMin;
	float SlopeInverse;
	FloatLine BaseLine;
};

class Vertex
{
public:
	Vertex()
	{
		X = 0.0f;
		Y = 0.0f;
		Z = 0.0f;
	}

	Vertex(float x, float y, float z)
	{
		X = x;
		Y = y;
		Z = z;
	}

	float X, Y, Z;
};

class Face
{
public:
	Face(int a, int b, int c)
	{
		A = a;
		B = b;
		C = c;
	}

	int A, B, C;
};

int RoundFloatToInt(float f)
{
	return (int)(f + .5f);
}

Point RoundFloatPointToInt(FloatPoint point)
{
	int x = RoundFloatToInt(point.X);
	int y = RoundFloatToInt(point.Y);
	int z = RoundFloatToInt(point.Z);

	return Point(x, y, z);
}

#pragma endregion

Pixel **PIXEL_BUFFER;

#pragma region Input

void HandleInput(int argc, char *argv[])
{
	for (int i = 1; i < argc; i++)
	{
		if (argv[i][0] != '-') continue;

		switch (argv[i][1])
		{
		case 'f':
			FILE_NAME_1 = (std::string)argv[i + 1];
			break;
		case 'g':
			FILE_NAME_2 = (std::string)argv[i + 1];
			break;
		case 'i':
			FILE_NAME_3 = (std::string)argv[i + 1];
			break;
		case 's':
			SCALE_FACTOR = std::atof(argv[i + 1]);
			break;
		//case 'r':
		//	ROTATION_DEGREES = std::atof(argv[i + 1]);
		//	ROTATION_RADIANS = (float)ROTATION_DEGREES * PI / 180;
		//	break;
		case 'm':
			X_TRANSLATION = std::atof(argv[i + 1]);
			break;
		case 'n':
			Y_TRANSLATION = std::atof(argv[i + 1]);
			break;
		case 'a':
			WW_X_LOWER_BOUND = std::atof(argv[i + 1]);
			break;
		case 'b':
			WW_Y_LOWER_BOUND = std::atof(argv[i + 1]);
			break;
		case 'c':
			WW_X_UPPER_BOUND = std::atof(argv[i + 1]);
			break;
		case 'd':
			WW_Y_UPPER_BOUND = std::atof(argv[i + 1]);
			break;
		case 'j':
			VP_X_LOWER_BOUND = std::atof(argv[i + 1]);
			break;
		case 'k':
			VP_Y_LOWER_BOUND = std::atof(argv[i + 1]);
			break;
		case 'o':
			VP_X_UPPER_BOUND = std::atof(argv[i + 1]);
			break;
		case 'p':
			VP_Y_UPPER_BOUND = std::atof(argv[i + 1]);
			break;
		case 'x':
			PRP_U = std::atof(argv[i + 1]);
			break;
		case 'y':
			PRP_V = std::atof(argv[i + 1]);
			break;
		case 'z':
			PRP_N = std::atof(argv[i + 1]);
			break;
		case 'X':
			VRP_X = std::atof(argv[i + 1]);
			break;
		case 'Y':
			VRP_Y = std::atof(argv[i + 1]);
			break;
		case 'Z':
			VRP_Z = std::atof(argv[i + 1]);
			break;
		case 'q':
			VPN_X = std::atof(argv[i + 1]);
			break;
		case 'r':
			VPN_Y = std::atof(argv[i + 1]);
			break;
		case 'w':
			VPN_Z = std::atof(argv[i + 1]);
			break;
		case 'Q':
			VUP_X = std::atof(argv[i + 1]);
			break;
		case 'R':
			VUP_Y = std::atof(argv[i + 1]);
			break;
		case 'W':
			VUP_Z = std::atof(argv[i + 1]);
			break;
		case 'u':
			VRC_U_MIN = std::atof(argv[i + 1]);
			break;
		case 'v':
			VRC_V_MIN = std::atof(argv[i + 1]);
			break;
		case 'U':
			VRC_U_MAX = std::atof(argv[i + 1]);
			break;
		case 'V':
			VRC_V_MAX = std::atof(argv[i + 1]);
			break;
		case 'F':
			CLIP_FRONT = std::atof(argv[i + 1]);
			break;
		case 'B':
			CLIP_BACK = std::atof(argv[i + 1]);
			break;
		case 'P':
			IS_PARALLEL_PROJECTION = true;
			break;
		}
	}
}

#pragma endregion

#pragma region Pixel Buffer

void ClearPixelBuffer()
{
	for (int i = 0; i < COLUMNS; i++)
	{
		for (int j = 0; j < ROWS; j++)
		{
			PIXEL_BUFFER[i][j] = Pixel();
		}
	}
}

void PrintPixelBuffer()
{
	std::cout << "P3" << std::endl;
	std::cout << "# Generated PBM for " << FILE_NAME_1 << std::endl;
	std::cout << COLUMNS << " " << ROWS << std::endl;
	std::cout << MAX_RGB_VAL << std::endl;

	for (int j = ROWS - 1; j >= 0; j--)
	{
		for (int i = 0; i < COLUMNS; i++)
		{
			if (i > 0)
				std::cout << PIXEL_SPACER;
			std::cout << PIXEL_BUFFER[i][j].ToString();
		}

		std::cout << std::endl;
	}
}

void CreatePixelBuffer()
{
	COLUMNS = X_RESOLUTION;
	ROWS = Y_RESOLUTION;

	PIXEL_BUFFER = new Pixel *[COLUMNS];

	for (int i = 0; i < COLUMNS; i++)
	{
		PIXEL_BUFFER[i] = new Pixel[ROWS];
	}

	ClearPixelBuffer();
}

void WritePixel(int x, int y, float z)
{
	if (z < CLIP_FRONT_POST_PROJECTION && z > PIXEL_BUFFER[x][y].Z)
	{
		float color = MAX_RGB_VAL * (z - CLIP_BACK_POST_PROJECTION) / CLIP_DIFF_POST_PROJECTION;

		PIXEL_BUFFER[x][y] = Pixel(z,
								   RoundFloatToInt(color * (CURRENT_FILE == 1)),
								   RoundFloatToInt(color * (CURRENT_FILE == 2)),
								   RoundFloatToInt(color * (CURRENT_FILE == 3)));
	}
}

void WritePixel(Point point, float z)
{
	WritePixel(point.X, point.Y, z);
}

void Print(std::string str)
{
	std::cout << str << std::endl;
}

#pragma endregion

#pragma region Transformations

void Scale(FloatLine &line)
{
	line.A.X = line.A.X * SCALE_FACTOR;
	line.A.Y = line.A.Y * SCALE_FACTOR;

	line.B.X = line.B.X * SCALE_FACTOR;
	line.B.Y = line.B.Y * SCALE_FACTOR;
}

void Rotate(FloatLine &line)
{
	float x1 = line.A.X;
	float y1 = line.A.Y;
	float x2 = line.B.X;
	float y2 = line.B.Y;

	line.A = FloatPoint(x1 * cos(ROTATION_RADIANS) - y1 * sin(ROTATION_RADIANS),
				   		x1 * sin(ROTATION_RADIANS) + y1 * cos(ROTATION_RADIANS));
	line.B = FloatPoint(x2 * cos(ROTATION_RADIANS) - y2 * sin(ROTATION_RADIANS),
						x2 * sin(ROTATION_RADIANS) + y2 * cos(ROTATION_RADIANS));
}

void Translate(FloatLine &line)
{
	line.A.X += X_TRANSLATION;
	line.A.Y += Y_TRANSLATION;

	line.B.X += X_TRANSLATION;
	line.B.Y += Y_TRANSLATION;
}

void WorldToViewPort(FloatPoint &point)
{
	float xScale = (VP_X_UPPER_BOUND - VP_X_LOWER_BOUND) / (WW_X_UPPER_BOUND - WW_X_LOWER_BOUND);
	float yScale = (VP_Y_UPPER_BOUND - VP_Y_LOWER_BOUND) / (WW_Y_UPPER_BOUND - WW_Y_LOWER_BOUND);

	point.X = VP_X_LOWER_BOUND + (point.X - WW_X_LOWER_BOUND) * xScale;
	point.Y = VP_Y_LOWER_BOUND + (point.Y - WW_Y_LOWER_BOUND) * yScale;
}

void WorldToViewPort(Polygon &polygon)
{
	for(int i = 0; i < polygon.Points.size(); i++)
	{
		WorldToViewPort(polygon.Points[i]);
	}
}

void ApplyTransformations(FloatLine &line)
{
	Scale(line);
	Rotate(line);
	Translate(line);
}

#pragma region Perspective

Eigen::Matrix4f TranslatePer()
{
	Eigen::Matrix4f matrix = Eigen::Matrix4f().Zero(4, 4);

	matrix(0, 0) = 1;
	matrix(1, 1) = 1;
	matrix(2, 2) = 1;
	matrix(3, 3) = 1;

	matrix(0, 3) = -VRP_X;
	matrix(1, 3) = -VRP_Y;
	matrix(2, 3) = -VRP_Z;

	return matrix;
}

Eigen::Matrix4f RotatePer()
{
	Eigen::Vector3f vpnVector(VPN_X, VPN_Y, VPN_Z);
	Eigen::Vector3f vupVector(VUP_X, VUP_Y, VUP_Z);

	Eigen::Vector3f rZ = vpnVector;
	rZ.normalize();

	Eigen::Vector3f rX = vupVector.cross(rZ);
	rX.normalize();

	Eigen::Vector3f rY = rZ.cross(rX);

	Eigen::Matrix4f matrix = Eigen::Matrix4f().Zero();

	matrix(0, 0) = rX[0];
	matrix(0, 1) = rX[1];
	matrix(0, 2) = rX[2];

	matrix(1, 0) = rY[0];
	matrix(1, 1) = rY[1];
	matrix(1, 2) = rY[2];

	matrix(2, 0) = rZ[0];
	matrix(2, 1) = rZ[1];
	matrix(2, 2) = rZ[2];

	matrix(3, 3) = 1;

	return matrix;
}

Eigen::Matrix4f OriginTranslatePer()
{
	Eigen::Matrix4f matrix = Eigen::Matrix4f().Zero();

	matrix(0, 0) = 1;
	matrix(1, 1) = 1;
	matrix(2, 2) = 1;
	matrix(3, 3) = 1;

	matrix(0, 3) = -PRP_U;
	matrix(1, 3) = -PRP_V;
	matrix(2, 3) = -PRP_N;

	return matrix;
}

Eigen::Matrix4f ShearPer()
{
	Eigen::Matrix4f matrix = Eigen::Matrix4f().Zero();

	matrix(0, 0) = 1;
	matrix(1, 1) = 1;
	matrix(2, 2) = 1;
	matrix(3, 3) = 1;

	matrix(0, 2) = (0.5f * (VRC_U_MAX + VRC_U_MIN) - PRP_U) / PRP_N;
	matrix(1, 2) = (0.5f * (VRC_V_MAX + VRC_V_MIN) - PRP_V) / PRP_N;

	return matrix;
}

Eigen::Matrix4f ScalePer()
{
	Eigen::Matrix4f matrix = Eigen::Matrix4f().Zero();

	matrix(0, 0) = (2.0f * (PRP_N)) / ((VRC_U_MAX - VRC_U_MIN) * (PRP_N - CLIP_BACK));
	matrix(1, 1) = (2.0f * (PRP_N)) / ((VRC_V_MAX - VRC_V_MIN) * (PRP_N - CLIP_BACK));
	matrix(2, 2) = 1.0f / (PRP_N - CLIP_BACK);
	matrix(3, 3) = 1;

	return matrix;
}

Eigen::Matrix4f CreateNormalizingTransformationPer()
{
	return ScalePer() * ShearPer() * OriginTranslatePer() * RotatePer() * TranslatePer();
}

#pragma endregion

#pragma region Parallel

Eigen::Matrix4f TranslatePar()
{
	Eigen::Matrix4f matrix = Eigen::Matrix4f().Zero(4, 4);

	matrix(0, 0) = 1;
	matrix(1, 1) = 1;
	matrix(2, 2) = 1;
	matrix(3, 3) = 1;

	matrix(0, 3) = -VRP_X;
	matrix(1, 3) = -VRP_Y;
	matrix(2, 3) = -VRP_Z;

	return matrix;
}

Eigen::Matrix4f RotatePar()
{
	Eigen::Vector3f vpnVector(VPN_X, VPN_Y, VPN_Z);
	Eigen::Vector3f vupVector(VUP_X, VUP_Y, VUP_Z);

	Eigen::Vector3f rZ = vpnVector;
	rZ.normalize();

	Eigen::Vector3f rX = vupVector.cross(rZ);
	rX.normalize();
	
	Eigen::Vector3f rY = rZ.cross(rX);

	Eigen::Matrix4f matrix = Eigen::Matrix4f().Zero();

	matrix(0, 0) = rX[0];
	matrix(0, 1) = rX[1];
	matrix(0, 2) = rX[2];

	matrix(1, 0) = rY[0];
	matrix(1, 1) = rY[1];
	matrix(1, 2) = rY[2];

	matrix(2, 0) = rZ[0];
	matrix(2, 1) = rZ[1];
	matrix(2, 2) = rZ[2];

	matrix(3, 3) = 1;

	return matrix;
}

Eigen::Matrix4f ShearPar()
{
	Eigen::Matrix4f matrix = Eigen::Matrix4f().Zero();

	matrix(0, 0) = 1;
	matrix(1, 1) = 1;
	matrix(2, 2) = 1;
	matrix(3, 3) = 1;

	matrix(0, 2) = (0.5f * (VRC_U_MAX + VRC_U_MIN) - PRP_U) / PRP_N;
	matrix(1, 2) = (0.5f * (VRC_V_MAX + VRC_V_MIN) - PRP_V) / PRP_N;

	return matrix;
}

Eigen::Matrix4f OriginTranslatePar()
{
	Eigen::Matrix4f matrix = Eigen::Matrix4f().Zero();

	matrix(0, 0) = 1;
	matrix(1, 1) = 1;
	matrix(2, 2) = 1;
	matrix(3, 3) = 1;

	matrix(0, 3) = (VRC_U_MAX + VRC_U_MIN) / -2.0f;
	matrix(1, 3) = (VRC_V_MAX + VRC_V_MIN) / -2.0f;
	matrix(2, 3) = -CLIP_FRONT;

	return matrix;
}

Eigen::Matrix4f ScalePar()
{
	Eigen::Matrix4f matrix = Eigen::Matrix4f().Zero();

	matrix(0, 0) = 2.0f / (VRC_U_MAX - VRC_U_MIN);
	matrix(1, 1) = 2.0f / (VRC_V_MAX - VRC_V_MIN);
	matrix(2, 2) = 1.0f / (CLIP_FRONT - CLIP_BACK);
	matrix(3, 3) = 1;

	return matrix;
}

Eigen::Matrix4f CreateNormalizingTransformationPar()
{
	return ScalePar() * OriginTranslatePar() * ShearPar() * RotatePar() * TranslatePar();
}

#pragma endregion

void ApplyNormalizingTransformations(std::vector<Vertex> &vertecies)
{
	Eigen::Matrix4f transformationMatrix;

	if(IS_PARALLEL_PROJECTION)
	{
		transformationMatrix = CreateNormalizingTransformationPar();
	}
	else
	{
		transformationMatrix = CreateNormalizingTransformationPer();
	}

	Eigen::Vector4f tempVector;

	for(int i = 0; i < vertecies.size(); i++)
	{
		tempVector = Eigen::Vector4f(vertecies[i].X, vertecies[i].Y, vertecies[i].Z, 1);
		tempVector = transformationMatrix * tempVector;

		vertecies[i] = Vertex(tempVector[0], tempVector[1], tempVector[2]);
	}
}

std::vector<FloatPoint> GetPerspectiveProjectionPoints(std::vector<Vertex> &vertecies)
{
	std::vector<FloatPoint> points;
	float demoninator;

	float zProj = PRP_N / (CLIP_BACK - PRP_N);

	for (int i = 0; i < vertecies.size(); i++)
	{
		demoninator = vertecies[i].Z / zProj;

		points.push_back(FloatPoint(vertecies[i].X / demoninator, vertecies[i].Y / demoninator, vertecies[i].Z));
	}

	WW_X_LOWER_BOUND = -std::abs(zProj);
	WW_Y_LOWER_BOUND = -std::abs(zProj);
	WW_X_UPPER_BOUND = std::abs(zProj);
	WW_Y_UPPER_BOUND = std::abs(zProj);

	return points;
}

std::vector<FloatPoint> GetParallelProjectionPoints(std::vector<Vertex> &vertecies)
{
	std::vector<FloatPoint> points;

	for(int i = 0; i < vertecies.size(); i++)
	{
		points.push_back(FloatPoint(vertecies[i].X, vertecies[i].Y, vertecies[i].Z));
	}

	WW_X_LOWER_BOUND = -1.0f;
	WW_Y_LOWER_BOUND = -1.0f;
	WW_X_UPPER_BOUND = 1.0f;
	WW_Y_UPPER_BOUND = 1.0f;

	return points;
}

#pragma endregion

#pragma region Bresenham Line Drawing

void GeneralizedBresenham(FloatPoint startingPointF, FloatPoint endingPointF)
{
	int dx, dy, D, x, y;

	float startZ, endZ, zp;

	Point startingPoint = RoundFloatPointToInt(startingPointF);
	Point endingPoint = RoundFloatPointToInt(endingPointF);

	startZ = startingPointF.Z;
	endZ = endingPointF.Z;

	if (startingPoint.X > endingPoint.X) //If qx > rx, swap points
	{
		Point temp = startingPoint;

		startingPoint = endingPoint;
		endingPoint = temp;
	}

	dx = endingPoint.X - startingPoint.X;
	dy = endingPoint.Y - startingPoint.Y;
	y = startingPoint.Y;
	x = startingPoint.X;

	if (dx == 0) //Vertical line special case
	{
		if (startingPoint.Y > endingPoint.Y)
		{
			Point temp = startingPoint;

			startingPoint = endingPoint;
			endingPoint = temp;
		}

		for (y = startingPoint.Y; y <= endingPoint.Y; y++)
		{
			zp = endZ - (endZ - startZ) * ((float)endingPoint.X - (float)x) / ((float)endingPoint.X - (float)startingPoint.X);

			WritePixel(x, y, zp);
		}
	}
	else
	{
		float slope = (float)dy / (float)dx;

		if (slope > 1)
		{
			D = 2 * dx - dy;

			for (y = startingPoint.Y; y <= endingPoint.Y; y++)
			{
				zp = endZ - (endZ - startZ) * ((float)endingPoint.X - (float)x) / ((float)endingPoint.X - (float)startingPoint.X);

				WritePixel(x, y, zp);

				if (D <= 0)
				{
					D += 2 * dx;
				}
				else
				{
					D += 2 * (dx - dy);
					x++;
				}
			}
		}
		else if (slope >= -1 && slope < 0)
		{
			dy *= -1;
			D = 2 * dy - dx;

			for (x = startingPoint.X; x <= endingPoint.X; x++)
			{
				zp = endZ - (endZ - startZ) * ((float)endingPoint.X - (float)x) / ((float)endingPoint.X - (float)startingPoint.X);

				WritePixel(x, y, zp);

				if (D <= 0)
				{
					D += 2 * dy;
				}
				else
				{
					D += 2 * (dy - dx);
					y--;
				}
			}
		}
		else if (slope < -1)
		{
			dy *= -1;
			D = 2 * dx - dy;

			for (y = startingPoint.Y; y >= endingPoint.Y; y--)
			{
				zp = endZ - (endZ - startZ) * ((float)endingPoint.X - (float)x) / ((float)endingPoint.X - (float)startingPoint.X);

				WritePixel(x, y, zp);

				if (D <= 0)
				{
					D += 2 * dx;
				}
				else
				{
					D += 2 * (dx - dy);
					x++;
				}
			}
		}
		else
		{
			D = 2 * dy - dx;

			for (x = startingPoint.X; x <= endingPoint.X; x++)
			{
				zp = endZ - (endZ - startZ) * ((float)endingPoint.X - (float)x) / ((float)endingPoint.X - (float)startingPoint.X);

				WritePixel(x, y, zp);

				if (D <= 0)
				{
					D += 2 * dy;
				}
				else
				{
					D += 2 * (dy - dx);
					y++;
				}
			}
		}
	}
}

void GeneralizedBresenham(FloatLine line)
{
	GeneralizedBresenham(line.A, line.B);
}

void GeneralizedBresenham(Polygon polygon)
{
	std::vector<FloatLine> lines = polygon.GetLines();

	for(int i = 0; i < lines.size(); i++)
	{
		GeneralizedBresenham(lines[i].A, lines[i].B);
	}
}

#pragma endregion

#pragma region Scanline Fill

bool XOfYMinComparison(EdgeBucket A, EdgeBucket B)
{
	return A.XOfYMin < B.XOfYMin;
}

std::vector<std::vector<EdgeBucket>> SetEdgeTable(Polygon polygon)
{
	std::vector<std::vector<EdgeBucket>> edgeTable(Y_RESOLUTION);

	std::vector<FloatLine> lines = polygon.GetLines();

	float slopeInverse;
	int scanline;

	for (int i = 0; i < lines.size(); i++)
	{
		if (lines[i].A.X == lines[i].B.X)
		{
			slopeInverse = 0;
		}
		else
		{
			if (lines[i].A.Y == lines[i].B.Y) continue;

			FloatPoint A = FloatPoint((float)lines[i].A.X, (float)lines[i].A.Y);
			FloatPoint B = FloatPoint((float)lines[i].B.X, (float)lines[i].B.Y);

			slopeInverse = 1 / ((B.Y - A.Y) / (B.X - A.X));
		}

		if (lines[i].A.Y > lines[i].B.Y)
		{
			scanline = RoundFloatToInt(lines[i].B.Y);
			edgeTable[scanline].push_back(EdgeBucket(RoundFloatToInt(lines[i].A.Y), lines[i].B.X, slopeInverse, lines[i]));
		}
		else
		{
			scanline = RoundFloatToInt(lines[i].A.Y);
			edgeTable[scanline].push_back(EdgeBucket(RoundFloatToInt(lines[i].B.Y), lines[i].A.X, slopeInverse, lines[i]));
		}

		std::sort(edgeTable[scanline].begin(), edgeTable[scanline].end(), XOfYMinComparison);
	}

	return edgeTable;
}

void RemoveEdgeByYMax(std::vector<EdgeBucket> &edges, int y)
{
	for (int i = 0; i < edges.size(); i++)
	{
		if (edges[i].YMax == y)
		{
			edges.erase(edges.begin() + i);

			i--;
		}
	}
}

void UpdateXBySlope(std::vector<EdgeBucket> &edges)
{
	for (int i = 0; i < edges.size(); i++)
	{
		edges[i].XOfYMin += edges[i].SlopeInverse;
	}
}

void ScanlineFill(Polygon polygon)
{
	std::vector<EdgeBucket> activeEdges;
	std::vector<std::vector<EdgeBucket>> edgeTable = SetEdgeTable(polygon);

	FloatPoint a;
	FloatPoint b;

	FloatLine tempLine;
	float tempZ;

	for (int i = 0; i < edgeTable.size(); i++)
	{
		a = FloatPoint();
		b = FloatPoint();

		for (int j = 0; j < edgeTable[i].size(); j++)
		{
			activeEdges.push_back(edgeTable[i][j]);
		}

		RemoveEdgeByYMax(activeEdges, i);

		std::sort(activeEdges.begin(), activeEdges.end(), XOfYMinComparison);

		for (int j = 0; j < activeEdges.size(); j++)
		{
			tempLine = activeEdges[j].BaseLine;
			tempZ = tempLine.A.Z - (tempLine.A.Z - tempLine.B.Z) * (tempLine.A.Y - (float)i) / (tempLine.A.Y - tempLine.B.Y);

			if (j % 2 == 0)
			{
				a = FloatPoint(activeEdges[j].XOfYMin, activeEdges[j].YMax, tempZ);

				if (a.X == b.X)
				{
					if ((a.X == a.Y) && (b.X != b.Y) ||
						(a.X != a.Y) && (b.X == b.Y))
					{
						b = a;
					}
				}
			}
			else
			{
				b = FloatPoint(activeEdges[j].XOfYMin, activeEdges[j].YMax, tempZ);

				if (a.X == b.X)
				{
					if ((a.X == a.Y) && (b.X != b.Y) ||
						(a.X != a.Y) && (b.X == b.Y))
					{
						a = b;
					}
					else
					{
						GeneralizedBresenham(FloatPoint(a.X, i, a.Z), FloatPoint(b.X, i, b.Z));
					}
				}
				else
				{
					GeneralizedBresenham(FloatPoint(a.X, i, a.Z), FloatPoint(b.X, i, b.Z));
				}
			}
		}

		UpdateXBySlope(activeEdges);
	}
}

#pragma endregion

#pragma region Cohen Sutherland Clipping

int CalculateCode(Point point)
{
	int code = INSIDE;

	if (point.X < WW_X_LOWER_BOUND)
	{
		code |= LEFT;
	}
	else if (point.X > WW_X_UPPER_BOUND)
	{
		code |= RIGHT;
	}
	if (point.Y < WW_Y_LOWER_BOUND)
	{
		code |= BOTTOM;
	}
	else if (point.Y > WW_Y_UPPER_BOUND)
	{
		code |= TOP;
	}

	return code;
}

void CohenSutherland(Point startingPoint, Point endingPoint)
{
	int startCode = CalculateCode(startingPoint);
	int endCode = CalculateCode(endingPoint);

	FloatPoint startingPointD = FloatPoint((float)startingPoint.X, (float)startingPoint.Y);
	FloatPoint endingPointD = FloatPoint((float)endingPoint.X, (float)endingPoint.Y);

	bool validated = false;

	while (true)
	{
		if ((startCode == INSIDE) && (endCode == INSIDE))
		{
			validated = true;
			break;
		}
		else if (startCode & endCode)
		{
			break;
		}
		else
		{
			int currentCode;
			float currentX, currentY;

			if (startCode != INSIDE)
			{
				currentCode = startCode;
			}
			else
			{
				currentCode = endCode;
			}

			if (currentCode & TOP)
			{
				currentX = startingPointD.X + (endingPointD.X - startingPointD.X) * (WW_Y_UPPER_BOUND - startingPointD.Y) / (endingPointD.Y - startingPointD.Y);
				currentY = WW_Y_UPPER_BOUND;
			}
			else if (currentCode & BOTTOM)
			{
				currentX = startingPointD.X + (endingPointD.X - startingPointD.X) * (WW_Y_LOWER_BOUND - startingPointD.Y) / (endingPointD.Y - startingPointD.Y);
				currentY = WW_Y_LOWER_BOUND;
			}
			else if (currentCode & LEFT)
			{
				currentX = WW_X_LOWER_BOUND;
				currentY = startingPointD.Y + (endingPointD.Y - startingPointD.Y) * (WW_X_LOWER_BOUND - startingPointD.X) / (endingPointD.X - startingPointD.X);
			}
			else if (currentCode & RIGHT)
			{
				currentX = WW_X_UPPER_BOUND;
				currentY = startingPointD.Y + (endingPointD.Y - startingPointD.Y) * (WW_X_UPPER_BOUND - startingPointD.X) / (endingPointD.X - startingPointD.X);
			}

			if (currentCode == startCode)
			{
				startingPointD.X = currentX;
				startingPointD.Y = currentY;

				startCode = CalculateCode(RoundFloatPointToInt(startingPointD));
			}
			else
			{
				endingPointD.X = currentX;
				endingPointD.Y = currentY;

				endCode = CalculateCode(RoundFloatPointToInt(endingPointD));
			}
		}
	}

	if (validated)
	{
		GeneralizedBresenham(startingPointD, endingPointD);
	}
}

#pragma endregion

#pragma region Sutherland Hodgman Clipping

float XIntersection(FloatLine line1, FloatLine line2)
{
	float numerator = (line1.A.X * line1.B.Y - line1.A.Y * line1.B.X) * (line2.A.X - line2.B.X) -
					  (line2.A.X * line2.B.Y - line2.A.Y * line2.B.X) * (line1.A.X - line1.B.X);

	float denominator = (line1.A.X - line1.B.X) * (line2.A.Y - line2.B.Y) -
						(line1.A.Y - line1.B.Y) * (line2.A.X - line2.B.X);

	return numerator / denominator;
}

float YIntersection(FloatLine line1, FloatLine line2)
{
	float numerator = (line1.A.X * line1.B.Y - line1.A.Y * line1.B.X) * (line2.A.Y - line2.B.Y) -
					  (line2.A.X * line2.B.Y - line2.A.Y * line2.B.X) * (line1.A.Y - line1.B.Y);

	float denominator = (line1.A.X - line1.B.X) * (line2.A.Y - line2.B.Y) -
						(line1.A.Y - line1.B.Y) * (line2.A.X - line2.B.X);

	return numerator / denominator;
}

void EdgeClip(Polygon &polygon, FloatLine clipper)
{
	std::vector<FloatLine> polygonLines = polygon.GetLines();
	std::vector<FloatPoint> updatedPoints;
	FloatLine currentLine;

	for (int i = 0; i < polygonLines.size(); i++)
	{
		currentLine = polygonLines[i];

		float pos1 = (clipper.B.X - clipper.A.X) * (currentLine.A.Y - clipper.A.Y) -
				   	 (clipper.B.Y - clipper.A.Y) * (currentLine.A.X - clipper.A.X);

		float pos2 = (clipper.B.X - clipper.A.X) * (currentLine.B.Y - clipper.A.Y) -
				   	 (clipper.B.Y - clipper.A.Y) * (currentLine.B.X - clipper.A.X);

		if (pos1 < 0 && pos2 < 0)
		{
			updatedPoints.push_back(FloatPoint(currentLine.B.X, currentLine.B.Y));
		}
		else if (pos1 >= 0 && pos2 < 0)
		{
			updatedPoints.push_back(FloatPoint(XIntersection(clipper, currentLine),
											   YIntersection(clipper, currentLine)));

			updatedPoints.push_back(FloatPoint(currentLine.B.X, currentLine.B.Y));
		}
		else if (pos1 < 0 && pos2 >= 0)
		{
			updatedPoints.push_back(FloatPoint(XIntersection(clipper, currentLine),
											   YIntersection(clipper, currentLine)));
		}
	}

	polygon.Points = updatedPoints;
}

void SutherlandHodgman(Polygon polygon)
{
	std::vector<FloatPoint> clipperPoints;

	clipperPoints.push_back(FloatPoint(WW_X_LOWER_BOUND, WW_Y_LOWER_BOUND));
	clipperPoints.push_back(FloatPoint(WW_X_LOWER_BOUND, WW_Y_UPPER_BOUND));
	clipperPoints.push_back(FloatPoint(WW_X_UPPER_BOUND, WW_Y_UPPER_BOUND));
	clipperPoints.push_back(FloatPoint(WW_X_UPPER_BOUND, WW_Y_LOWER_BOUND));

	Polygon clipperShape = Polygon(clipperPoints);
	std::vector<FloatLine> clipperLines = clipperShape.GetLines();

	for (int i = 0; i < clipperLines.size(); i++)
	{
		EdgeClip(polygon, clipperLines[i]);
	}

	WorldToViewPort(polygon);

	ScanlineFill(polygon);

	//GeneralizedBresenham(polygon);
}

#pragma endregion

#pragma region Drawing

void DrawPolygon(Polygon polygon)
{
	std::vector<FloatLine> polygonLines = polygon.GetLines();

	for (int i = 0; i < polygonLines.size(); i++)
	{
		ApplyTransformations(polygonLines[i]);
	}

	Polygon transformedPolygon = Polygon(polygonLines);

	SutherlandHodgman(transformedPolygon);
}

#pragma endregion

#pragma region File Processing

void ProcessPostscript()
{
	std::ifstream inStream(FILE_NAME_1, std::ios::out);
	std::string strBuffer;

	FloatPoint currentPoint;

	std::vector<FloatPoint> points;

	while (std::getline(inStream, strBuffer))
	{
		if (strBuffer.compare(POSTSCRIPT_BEGIN) == 0)
		{
			break;
		}
	}

	while (std::getline(inStream, strBuffer))
	{
		if (strBuffer.compare(POSTSCRIPT_END) == 0)
		{
			break;
		}
		else
		{
			std::stringstream strStream(strBuffer);
			std::string token;

			std::vector<std::string> tokens;

			while (std::getline(strStream, token, ' '))
			{
				if(token.size() > 0)
				{
					tokens.push_back(token);
				}
			}

			if (tokens.size() == 0) continue;

			if (tokens[0] == POSTSCRIPT_STROKE)
			{
				DrawPolygon(Polygon(points));

				points.clear();
			}
			else if (tokens[2] == POSTSCRIPT_MOVETO)
			{
				currentPoint = FloatPoint(std::stof(tokens[0]), std::stof(tokens[1]));
			}
			else if (tokens[2] == POSTSCRIPT_LINETO)
			{
				points.push_back(currentPoint);

				currentPoint = FloatPoint(std::stof(tokens[0]), std::stof(tokens[1]));
			}
		}
	}

	inStream.close();
}

void ProcessSMF(std::string filename)
{
	CURRENT_FILE++;

	if(filename.size() < 1) return;

	std::ifstream inStream(filename, std::ios::out);
	std::string strBuffer;

	std::vector<Vertex> vertices;
	std::vector<FloatPoint> points;
	std::vector<Face> faces;

	std::vector<FloatPoint> tempPoints(3);

	Polygon tempPolygon;

	while (std::getline(inStream, strBuffer))
	{
		std::stringstream strStream(strBuffer);
		std::string token;

		std::vector<std::string> tokens;

		while (std::getline(strStream, token, ' '))
		{
			if (token.size() > 0)
			{
				tokens.push_back(token);
			}
		}

		if (tokens.size() == 0)
			continue;

		if (tokens[0] == "v")
		{
			vertices.push_back(Vertex(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3])));
		}
		else if (tokens[0] == "f")
		{
			faces.push_back(Face(std::stoi(tokens[1]) - 1, std::stoi(tokens[2]) - 1, std::stoi(tokens[3]) - 1));
		}
	}

	inStream.close();

	ApplyNormalizingTransformations(vertices);

	if (IS_PARALLEL_PROJECTION)
	{
		points = GetParallelProjectionPoints(vertices);

		CLIP_FRONT_POST_PROJECTION = 0.0f;
		CLIP_BACK_POST_PROJECTION = -1.0f;
	}
	else
	{
		points = GetPerspectiveProjectionPoints(vertices);

		CLIP_FRONT_POST_PROJECTION = (PRP_N - CLIP_FRONT) / (CLIP_BACK - PRP_N);
		CLIP_BACK_POST_PROJECTION = -1.0f;
	}

	CLIP_DIFF_POST_PROJECTION = CLIP_FRONT_POST_PROJECTION - CLIP_BACK_POST_PROJECTION;

	for(int i = 0; i < faces.size(); i++)
	{
		tempPoints[0] = points[faces[i].A];
		tempPoints[1] = points[faces[i].B];
		tempPoints[2] = points[faces[i].C];

		//SutherlandHodgman(Polygon(tempPoints));

		tempPolygon = Polygon(tempPoints);

		WorldToViewPort(tempPolygon);

		ScanlineFill(tempPolygon);
	}
}

#pragma endregion

int main(int argc, char *argv[])
{
	HandleInput(argc, argv);
	CreatePixelBuffer();
	ProcessSMF(FILE_NAME_1);
	ProcessSMF(FILE_NAME_2);
	ProcessSMF(FILE_NAME_3);
	PrintPixelBuffer();
}