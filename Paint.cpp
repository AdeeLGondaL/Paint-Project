#include<SFML\Graphics.hpp>
#include<fstream>
#include<iostream>
#include<string>
#include <windows.h>

using namespace std;
using namespace sf;

//------------------------------------------------------------------------------------CLASSES------------------------------------------------------------------------------------//

// Ray Casting Algorithm
bool onSegment(Vector2f p, Vector2f q, Vector2f r)
{
	if ((q.x <= max(p.x, r.x)) && (q.x >= min(p.x, r.x)) && (q.y <= max(p.y, r.y)) && (q.y >= min(p.y, r.y)))
	{
		return true;
	}
	return false;
}
int orientation(Vector2f p, Vector2f q, Vector2f r)
{
	int val = ((q.y - p.y) * (r.x - q.x)) - ((q.x - p.x) * (r.y - q.y));

	if (val == 0)
	{
		return 0; // colinear
	}
	return (val > 0) ? 1 : 2; // 1 for clockwise  2 for counterclock wise
}
bool doIntersect(Vector2f p1, Vector2f q1, Vector2f p2, Vector2f q2)
{
	int o1 = orientation(p1, q1, p2);
	int o2 = orientation(p1, q1, q2);
	int o3 = orientation(p2, q2, p1);
	int o4 = orientation(p2, q2, q1);

	// General case
	if (o1 != o2 && o3 != o4)
		return true;

	// Special Cases
	// p1, q1 and p2 are colinear and p2 lies on segment p1q1
	if (o1 == 0 && onSegment(p1, p2, q1)) return true;

	if (o2 == 0 && onSegment(p1, q2, q1)) return true;

	if (o3 == 0 && onSegment(p2, p1, q2)) return true;

	if (o4 == 0 && onSegment(p2, q1, q2)) return true;

	return false; // Doesn't fall in any of the above cases
}
bool isInside(Vector2f* polygon, int n, Vector2f p)
{
	// There must be at least 3 vertices in polygon[]
	if (n < 3) return false;

	// Create a point for line segment from p to infinite
	Vector2f extreme = { 1000, p.y };

	// Count intersections of the above line with sides of polygon
	int count = 0, i = 0;
	do
	{
		int next = (i + 1) % n;

		// Check if the line segment from 'p' to 'extreme' intersects
		// with the line segment from 'polygon[i]' to 'polygon[next]'
		if (doIntersect(polygon[i], polygon[next], p, extreme))
		{
			// If the point 'p' is colinear with line segment 'i-next',
			// then check if it lies on segment. If it lies, return true,
			// otherwise false
			if (orientation(polygon[i], p, polygon[next]) == 0)
			{
				return onSegment(polygon[i], p, polygon[next]);
			}
			count++;
		}
		i = next;
	} while (i != 0);

	// Return true if count is odd, false otherwise
	return (count % 2 == 1);
}
template <typename type1, typename type2>
bool isMouseOver(type1& window, type2& entity)
{
	float mouseX = static_cast<float>(Mouse::getPosition(window).x);
	float mouseY = static_cast<float>(Mouse::getPosition(window).y);

	float entityX = entity.getPosition().x;
	float entityY = entity.getPosition().y;

	float entityWidth = entity.getPosition().x + entity.getLocalBounds().width;
	float entityHeight = entity.getPosition().y + entity.getLocalBounds().height;

	if (mouseX < entityWidth && mouseX>entityX && mouseY<entityHeight && mouseY > entityY)
	{
		return true;
	}
	else
	{
		return false;
	}
}

RenderWindow window(VideoMode(1920, 1080), "Paint");
class AllShapes
{
protected:
	int ID;
	Color Colour;
	Vector2f* Point;
	int NoOfPoints;
	void InitShape()
	{
		this->Point = NULL;
		this->ID = 0;
		Colour = Color::White;
	}
public:
	AllShapes()
	{
		this->InitShape();
	}
	AllShapes(int id, Vector2f* P, int noOfPoints, Color C)
	{
		this->InitShape(); this->Colour = C;
		this->NoOfPoints = noOfPoints; this->ID = id;
		this->Point = new Vector2f[noOfPoints];
		for (int i = 0; i < this->NoOfPoints; i++)
		{
			this->Point[i].setVector(P[i].x, P[i].y);
		}
	}
	virtual void Draw(RenderWindow& window) = 0;
	virtual bool Contains(Vector2f V) = 0;
	void SetID(int id)
	{
		this->ID = id;
	}
	virtual int GetID()
	{
		return 0;
	}
	virtual bool GetStyle()
	{
		return false;
	}
	virtual Color GetColour()
	{
		return Color::White;
	}
	virtual int GetnoOfPoints()
	{
		return 0;
	}
	virtual string  GetString()
	{
		return "...";
	}
	virtual void ChangeColour(Color C) = 0;
	virtual void SetPosition(float X, float Y)
	{
		return;
	}
	virtual void setString(char Char)
	{

	}
	virtual void removeAlphabet()
	{

	}
	virtual Vector2f* GetPoints()
	{
		return NULL;
	}
	virtual ~AllShapes()
	{
		if (Point != NULL)
		{
			delete[] Point; Point = NULL;
		}
		this->InitShape();
	}
	void setAllShapes(int id, Vector2f* P, int Points, Color C)
	{
		this->NoOfPoints = Points; this->ID = id; this->Colour = C;
		if (this->Point != nullptr)
		{
			delete[] Point;
			Point = nullptr;
		}
		this->Point = new Vector2f[this->NoOfPoints];
		for (int i = 0; i < this->NoOfPoints; i++)
		{
			this->Point[i] = P[i];
		}
	}
};

class texts :public AllShapes
{
	Text text_element;
	string TextString;
public:

	texts()
	{
	}
	texts(int id, Vector2f* P, int noOfPoints = 1, Color C = Color::White, string Input = "\0") :AllShapes(id, P, noOfPoints, C)
	{
		this->TextString = Input;
	}
	int GetID()
	{
		return this->ID;
	}
	Color GetColour()
	{
		return this->Colour;
	}
	int GetnoOfPoints()
	{
		return this->NoOfPoints;
	}
	Vector2f* GetPoints()
	{
		return this->Point;
	}
	void Draw(RenderWindow& window)
	{
		Font font_element;
		font_element.loadFromFile("arial.ttf");
		text_element.setCharacterSize(22);
		text_element.setFillColor(this->Colour);
		text_element.setFont(font_element);
		text_element.setString(TextString);
		text_element.setPosition(this->Point[0]);
		window.draw(text_element);
	}
	void setString(char Char)
	{
		TextString += Char;
	}
	void removeAlphabet()
	{
		TextString = TextString.substr(0, TextString.length() - 1);
	}
	void SetPosition(float X, float Y)
	{
		this->Point[0].x = X;
		this->Point[0].y = Y;
	}
	void ChangeColour(Color C)
	{
		this->Colour = C;
	}
	string  GetString()
	{
		return this->TextString;
	}
	bool Contains(Vector2f V)
	{
		return false;
		/*if (isMouseOver(window, text_element))
		{
			return true;
		}
		return false;*/
	}
	~texts()
	{
	}
};

class  OpenShapes :public AllShapes
{
protected:
	bool Style; // 1 for Solid line  0 for Dotted Line
public:
	OpenShapes()
	{
		Style = false;
	}
	OpenShapes(int id, Vector2f* P, int noOfPoints, Color C, bool style) :AllShapes(id, P, noOfPoints, C)
	{
		this->Style = style;
	}
	virtual void ChangeColour(Color C) = 0;
	void SetOpenShape(int id, Vector2f* P, int Points, bool style, Color C = Color::White)
	{
		AllShapes::setAllShapes(id, P, Points, C);
		this->Style = style;
	}
	virtual	~OpenShapes()
	{
	}
};

class Circle :public AllShapes
{
	Vector2f RadiusCoordinates;
	float RadiusLength;
	CircleShape circle;
public:

	Circle()
	{
		RadiusCoordinates.x = 0.0;
		RadiusCoordinates.y = 0.0;
		RadiusLength = 0.0;
	}
	Circle(int id, Vector2f* P, int noOfPoints, Color C = Color::White) :AllShapes(id, P, noOfPoints, C)
	{
		RadiusCoordinates = P[1]; // Second Point will be Radius
		RadiusLength = sqrt(pow(this->Point[0].x - this->Point[1].x, 2) + pow(this->Point[0].y - this->Point[1].y, 2));
	}
	int GetID()
	{
		return this->ID;
	}
	Color GetColour()
	{
		return this->Colour;
	}
	int GetnoOfPoints()
	{
		return this->NoOfPoints;
	}
	Vector2f* GetPoints()
	{
		return this->Point;
	}
	void Draw(RenderWindow& window)
	{
		circle.setRadius(RadiusLength);
		circle.setPosition(Vector2f(Point[0].x, Point[0].y));
		circle.setOrigin(RadiusLength, RadiusLength);
		circle.setFillColor(Colour);
		window.draw(circle);
	}
	bool Contains(Vector2f V)
	{
		if (sqrt(pow(this->Point[0].x - V.x, 2) + pow(this->Point[0].y - V.y, 2)) <= this->RadiusLength)
		{
			return true;
		}
		return false;
	}
	void ChangeColour(Color C)
	{
		this->Colour = C;
	}
	/*void ChangeColour(Color C)
	{
		this->FillColour(C);
	}*/
	~Circle()
	{
	}

	//helper functions
	void setCircle(int id, Vector2f* P, int Points, Color C = Color::White)
	{
		AllShapes::setAllShapes(id, P, Points, C);
		RadiusCoordinates = P[1]; // Second Point will be Radius
		RadiusLength = sqrt(pow(Point[0].x - Point[1].x, 2) + pow(Point[0].y - Point[1].y, 2));
	}
};

class Line :public OpenShapes
{
	VertexArray pixel;
public:
	Line()
	{
	}
	Line(Color C)
	{
		this->Colour = C;
	}
	Line(int id, Vector2f* P, int noOfPoints, bool style, Color C = Color::White) :OpenShapes(id, P, noOfPoints, C, style)
	{
	}
	void Draw(RenderWindow& window)
	{
		if (!this->Style)// 1 for Solid Line
		{
			Vertex line[] = { Vertex(Vector2f(this->Point[0]),this->Colour), Vertex(Vector2f(this->Point[1]),this->Colour) };
			window.draw(line, 2, Lines);
		}
		else
		{
			// Dotted Line
			////int i = 0;
			//int x = this->Point[0].x, y = this->Point[0].y;
			//for (; (x<=this->Point[1].x)||(y <= this->Point[1].y); x++,y++)
			//{
			//	Vertex line[] = { Vertex(Vector2f(x,y),this->Colour), Vertex(Vector2f(x+1,y+1),this->Colour) };
			//	window.draw(line, 2, Lines);
			//}
			////Vertex line[] = { Vertex(Vector2f(x,y),this->Colour), Vertex(Vector2f(x + 1,y + 1),this->Colour) };
			////window.draw(line, 2, Lines);
		}
	}
	void ChangeColour(Color C)
	{
		this->Colour = C;
	}
	bool Contains(Vector2f V)
	{
		return onSegment(this->Point[0], V, this->Point[1]);
	}
	void SetLine(int id, Vector2f* P, int noOfPoints, bool style, Color C = Color::White)
	{
		OpenShapes::SetOpenShape(id, P, noOfPoints, style, C);
	}
	int GetID()
	{
		return this->ID;
	}
	bool GetStyle()
	{
		return this->Style;
	}
	Color GetColour()
	{
		return this->Colour;
	}
	int GetnoOfPoints()
	{
		return this->NoOfPoints;
	}
	Vector2f* GetPoints()
	{
		return this->Point;
	}
	~Line()
	{
	}
};

class curve :public OpenShapes
{
public:
	curve()
	{
	}
	curve(int id, Vector2f* P, int noOfPoints, bool style, Color C = Color::White) :OpenShapes(id, P, noOfPoints, C, style)
	{
	}
	int GetID()
	{
		return this->ID;
	}
	bool GetStyle()
	{
		return this->Style;
	}
	Color GetColour()
	{
		return this->Colour;
	}
	int GetnoOfPoints()
	{
		return this->NoOfPoints;
	}
	Vector2f* GetPoints()
	{
		return this->Point;
	}
	void Draw(RenderWindow& window)
	{
		Line line(this->Colour);
		int i = 0;
		for (; i < this->NoOfPoints - 1; i++)
		{
			Vertex line[] = { Vertex(Vector2f(this->Point[i]),this->Colour), Vertex(Vector2f(this->Point[i + 1]),this->Colour) };
			window.draw(line, 2, Lines);
		}
	}
	void ChangeColour(Color C)
	{
		this->Colour = C;
	}
	bool Contains(Vector2f V)
	{
		for (int i = 0; i < this->NoOfPoints - 1; i++)
		{
			if (onSegment(this->Point[i], V, this->Point[i + 1]))
			{
				return true;
			}
		}
		return false;
	}
	void SetCurve(int id, Vector2f* P, int noOfPoints, bool style, Color C = Color::White)
	{
		OpenShapes::SetOpenShape(id, P, noOfPoints, style, C);
	}
	~curve()
	{
	}
};

class polygon :public AllShapes
{
public:
	polygon()
	{
	}
	polygon(int id, Vector2f* P, int noOfPoints, Color C = Color::Black) :AllShapes(id, P, noOfPoints, C)
	{
	}
	int GetID()
	{
		return this->ID;
	}
	Color GetColour()
	{
		return this->Colour;
	}
	int GetnoOfPoints()
	{
		return this->NoOfPoints;
	}
	Vector2f* GetPoints()
	{
		return this->Point;
	}
	void Draw(RenderWindow& window)
	{
		double RADIUS = 0.0, radiusX = 0.0, radiusY = 0.0, minX = Point[0].x, maxX = 0.0, minY = Point[0].y, maxY = 0.0;
		for (int i = 0; i < this->NoOfPoints; i++)
		{
			if (Point[i].x > maxX)
				maxX = Point[i].x;
			else if (Point[i].x < minX)
				minX = Point[i].x;

			if (Point[i].y > maxY)
				maxY = Point[i].y;
			else if (Point[i].y < minY)
				minY = Point[i].y;
		}
		radiusX = minX - (maxX - minX) / 2;
		radiusY = minY - (maxY - minY) / 2;

		RADIUS = radiusX - Point[0].x;
		if (RADIUS < 0)
			RADIUS *= -1;
		CircleShape circle(RADIUS, this->NoOfPoints);
		circle.setPosition(radiusX, radiusY);
		circle.setFillColor(this->Colour);
		circle.setOutlineColor(Color::White);
		circle.setOutlineThickness(2);
		window.draw(circle);
	}
	void SetPolygon(Vector2f* P, int noOfPoints, Color C = Color::White)
	{
		this->InitShape();
		this->Colour = C;
		this->NoOfPoints = noOfPoints;
		if (this->Point != nullptr)
		{
			delete[] Point;
		}
		this->Point = new Vector2f[this->NoOfPoints];
		for (int i = 0; i < this->NoOfPoints; i++)
		{
			this->Point[i] = P[i];
		}
	}
	bool Contains(Vector2f V)
	{
		return isInside(this->Point, this->NoOfPoints, V);
	}
	void FillColour(Color C)
	{
		this->Colour = C;
	}
	void ChangeColour(Color C)
	{
		this->FillColour(C);
	}
	~polygon()
	{
	}
};

class Rectangles :public polygon
{
	bool changecolorCalled;
private:
	void Calculating_Coordinates()
	{
		this->Point->Extend(this->Point, this->NoOfPoints);
		this->Point[this->NoOfPoints - 1].setVector(this->Point[0].x, this->Point[1].y);
		this->Point->Extend(this->Point, this->NoOfPoints);
		this->Point[this->NoOfPoints - 1].setVector(this->Point[1].x, this->Point[0].y);
		swap(Point[1], Point[3]); swap(Point[2], Point[3]);
	}
	void ChangeColor(RenderWindow& window, Color C)
	{
		if (Point[0].x > Point[1].x)
		{
			swap(Point[0].x, Point[1].x);
			swap(Point[0].y, Point[1].y);
		}
		for (float TempX0 = Point[0].x, TempY0 = Point[0].y, TempY3 = Point[3].y; TempX0 < Point[1].x; TempX0 += 1.0)
		{
			VertexArray line(LinesStrip, 2);
			line[0].position = Vector2f(TempX0, TempY3);
			line[1].position = Vector2f(TempX0, TempY0);
			line[0].color = this->Colour;
			line[1].color = this->Colour;
			window.draw(line);
		}
		swap(Point[0].x, Point[1].x);
		swap(Point[0].y, Point[1].y);
	}
public:
	Rectangles()
	{
	}
	Rectangles(int id, Vector2f* P, int noOfPoints, Color C = Color::White, int Copy = 0) :polygon(id, P, noOfPoints, C)
	{
		if (Copy == 0)
		{
			Calculating_Coordinates();
		}
	}
	int GetID()
	{
		return this->ID;
	}
	Color GetColour()
	{
		return this->Colour;
	}
	int GetnoOfPoints()
	{
		return this->NoOfPoints;
	}
	Vector2f* GetPoints()
	{
		return this->Point;
	}
	void ChangeColour(Color C)
	{
		this->Colour = C;
		this->changecolorCalled = true;
	}
	bool Contains(Vector2f V)
	{
		return isInside(this->Point, this->NoOfPoints, V);
	}
	void  Draw(RenderWindow& window)
	{
		int i = 0;
		Vector2f v1, v2;
		for (; i < this->NoOfPoints - 1; i++)
		{
			Vertex line[] = { Vertex(Vector2f(this->Point[i]),this->Colour), Vertex(Vector2f(this->Point[i + 1]),this->Colour) };
			window.draw(line, 2, Lines);
		}
		Vertex line[] = { Vertex(Vector2f(this->Point[i]),this->Colour), Vertex(Vector2f(this->Point[0]),this->Colour) };
		window.draw(line, 2, Lines);
		ChangeColor(window, this->Colour);
	}
	void SetRectangle(int id, Vector2f* P, int Points, Color C = Color::White)
	{
		AllShapes::setAllShapes(id, P, Points, C);
		Calculating_Coordinates();
	}
	~Rectangles()
	{
	}
};

//------------------------------------------------------------------------------------CLASSES------------------------------------------------------------------------------------//

//------------------------------------------------------------------------------------FUNCTIONS------------------------------------------------------------------------------------//


template <typename type2>
void ShrinkButton(type2& button)
{
	Vector2f scale = button.getScale();
	button.setScale(scale.x / 1.2, scale.y / 1.2);
}

template <typename type2>
void ExpandButton(type2& button)
{
	Vector2f scale = button.getScale();
	button.setScale(scale.x * 1.2, scale.y * 1.2);
}

void Deallocate(AllShapes**& Shapes, int Size)
{
	for (int i = 0; i < Size; i++)
	{
		delete Shapes[i];
	}
	if (Size != 0)
	{
		delete[] Shapes; Shapes = NULL;
	}
}

void Extend(AllShapes**& Shapes, int& Size)
{
	AllShapes** New = new AllShapes * [Size + 1];
	for (int i = 0; i < Size; i++)
	{
		int Id = Shapes[i]->GetID();
		switch (Id)
		{
		case 1: // Line
		{
			New[i] = new Line(Shapes[i]->GetID(), Shapes[i]->GetPoints(), Shapes[i]->GetnoOfPoints(), Shapes[i]->GetStyle(), Shapes[i]->GetColour());
			break;
		}

		case 2: // Rectangle
		{
			//Rectangles(int id, Vector2f * P, int noOfPoints, Color C = Color::White)
			New[i] = new Rectangles(Shapes[i]->GetID(), Shapes[i]->GetPoints(), Shapes[i]->GetnoOfPoints(), Shapes[i]->GetColour(), 1);
			break;
		}
		case 3: // Circle
		{
			//Circle(int id, Vector2f * P, int noOfPoints, Color C = Color::White)
			New[i] = new Circle(Shapes[i]->GetID(), Shapes[i]->GetPoints(), Shapes[i]->GetnoOfPoints(), Shapes[i]->GetColour());
			break;
		}
		case 4: // Polygon
		{
			//polygon(int id, Vector2f * P, int noOfPoints, Color C = Color::Black)
			New[i] = new polygon(Shapes[i]->GetID(), Shapes[i]->GetPoints(), Shapes[i]->GetnoOfPoints(), Shapes[i]->GetColour());

			break;
		}
		case 5:// Curve
		{
			//Curve(int id, Vector2f * P, int noOfPoints, bool style, Color C = Color::White)
			New[i] = new curve(Shapes[i]->GetID(), Shapes[i]->GetPoints(), Shapes[i]->GetnoOfPoints(), Shapes[i]->GetStyle(), Shapes[i]->GetColour());
			break;
		}
		case 6:// Text
		{
			//texts(int id, Vector2f * P, int noOfPoints = 1, Color C = Color::White, string Input = "\0")
			New[i] = new texts(Shapes[i]->GetID(), Shapes[i]->GetPoints(), Shapes[i]->GetnoOfPoints(), Shapes[i]->GetColour(), Shapes[i]->GetString());
			break;
		}
		}
	}
	New[Size] = NULL;
	Deallocate(Shapes, Size);
	Shapes = New;
	Size++;
}

void RemoveShape(AllShapes**& Shapes, int& Size, int index)
{
	AllShapes** New = new AllShapes * [Size - 1];
	for (int i = 0, j = 0; i < Size; i++)
	{
		if (i != index)
		{
			int Id = Shapes[i]->GetID();
			switch (Id)
			{
			case 1: // Line
			{
				New[j] = new Line(Shapes[i]->GetID(), Shapes[i]->GetPoints(), Shapes[i]->GetnoOfPoints(), Shapes[i]->GetStyle(), Shapes[i]->GetColour());
				break;
			}

			case 2: // Rectangle
			{
				//Rectangles(int id, Vector2f * P, int noOfPoints, Color C = Color::White)
				New[j] = new Rectangles(Shapes[i]->GetID(), Shapes[i]->GetPoints(), Shapes[i]->GetnoOfPoints(), Shapes[i]->GetColour(), 1);
				break;
			}
			case 3: // Circle
			{
				//Circle(int id, Vector2f * P, int noOfPoints, Color C = Color::White)
				New[j] = new Circle(Shapes[i]->GetID(), Shapes[i]->GetPoints(), Shapes[i]->GetnoOfPoints(), Shapes[i]->GetColour());
				break;
			}
			case 4: // Polygon
			{
				//polygon(int id, Vector2f * P, int noOfPoints, Color C = Color::Black)
				New[j] = new polygon(Shapes[i]->GetID(), Shapes[i]->GetPoints(), Shapes[i]->GetnoOfPoints(), Shapes[i]->GetColour());

				break;
			}
			case 5:// Curve
			{
				//Curve(int id, Vector2f * P, int noOfPoints, bool style, Color C = Color::White)
				New[j] = new curve(Shapes[i]->GetID(), Shapes[i]->GetPoints(), Shapes[i]->GetnoOfPoints(), Shapes[i]->GetStyle(), Shapes[i]->GetColour());
				break;
			}
			case 6:// Text
			{
				//texts(int id, Vector2f * P, int noOfPoints = 1, Color C = Color::White, string Input = "\0")
				New[j] = new texts(Shapes[i]->GetID(), Shapes[i]->GetPoints(), Shapes[i]->GetnoOfPoints(), Shapes[i]->GetColour(), Shapes[i]->GetString());
				break;
			}
			}
			j++;
		}
	}
	//New[Size - 1] = NULL;
	Deallocate(Shapes, Size);
	Shapes = New;
	New = NULL;
	Size--;
}

void WriteCoordinates(ofstream& out, Vector2f* Points, int NoOfPoints)
{
	for (int i = 0; i < NoOfPoints; i++)
	{
		out << Points[i].x << " " << Points[i].y << " ";
	}
}

int  GetColourID(Color C)
{
	if (C == Color::Red)
	{
		return 9;
	}
	if (C == Color::Green)
	{
		return 10;
	}
	if (C == Color::Blue)
	{
		return 11;
	}
	if (C == Color::Yellow)
	{
		return 12;
	}
	if (C == Color(255, 165, 0))
	{
		return 13;
	}
	if (C == Color::White)
	{
		return 14;
	}
	if (C == Color::Black)
	{
		return 15;
	}
	if (C == Color(255, 140, 0))
	{
		return 16;
	}
}
Color SetColour(int id)
{
	if (id == 9)
	{
		return Color::Red;
	}
	if (id == 10)
	{
		return Color::Green;
	}
	if (id == 11)
	{
		return Color::Blue;
	}
	if (id == 12)
	{
		return Color::Yellow;
	}
	if (id == 13)
	{
		return Color(255, 165, 0);
	}
	if (id == 14)
	{
		return Color::White;
	}
	if (id == 15)
	{
		return Color::Black;
	}
	if (id == 16)
	{
		return Color(255, 140, 0);
	}
}

void Write(AllShapes** Shapes, int Size)
{
	ofstream out;
	out.open("Shapes.txt");
	if (out.is_open())
	{
		out << Size << endl;
		for (int i = 0; i < Size; i++)
		{
			if (Shapes[i]->GetID() == 1) // Line
			{
				//Line(int id, Vector2f * P, int noOfPoints, bool style, Color C = Color::White)
				out << Shapes[i]->GetID() << " " << Shapes[i]->GetnoOfPoints() << " ";
				WriteCoordinates(out, Shapes[i]->GetPoints(), Shapes[i]->GetnoOfPoints());
				out << Shapes[i]->GetStyle() << " " << GetColourID(Shapes[i]->GetColour()) << endl;
			}
			if (Shapes[i]->GetID() == 2)// Rectangle
			{
				//Rectangles(int id, Vector2f * P, int noOfPoints, Color C = Color::White, int Copy = 0)
				out << Shapes[i]->GetID() << " " << Shapes[i]->GetnoOfPoints() << " ";
				WriteCoordinates(out, Shapes[i]->GetPoints(), Shapes[i]->GetnoOfPoints());
				out << GetColourID(Shapes[i]->GetColour()) << endl;
			}
			if (Shapes[i]->GetID() == 3) // Circle
			{
				//Circle(int id, Vector2f * P, int noOfPoints, Color C = Color::White)
				out << Shapes[i]->GetID() << " " << Shapes[i]->GetnoOfPoints() << " ";
				WriteCoordinates(out, Shapes[i]->GetPoints(), Shapes[i]->GetnoOfPoints());
				out << GetColourID(Shapes[i]->GetColour()) << endl;
			}
			if (Shapes[i]->GetID() == 4)//  Polygon
			{
				//polygon(int id, Vector2f * P, int noOfPoints, Color C = Color::Black)
				out << Shapes[i]->GetID() << " " << Shapes[i]->GetnoOfPoints() << " ";
				WriteCoordinates(out, Shapes[i]->GetPoints(), Shapes[i]->GetnoOfPoints());
				out << GetColourID(Shapes[i]->GetColour()) << endl;
			}
			if (Shapes[i]->GetID() == 5)//Curve
			{
				//curve(int id, Vector2f * P, int noOfPoints, bool style, Color C = Color::White)
				out << Shapes[i]->GetID() << " " << Shapes[i]->GetnoOfPoints() << " ";
				WriteCoordinates(out, Shapes[i]->GetPoints(), Shapes[i]->GetnoOfPoints());
				out << Shapes[i]->GetStyle() << " " << GetColourID(Shapes[i]->GetColour()) << endl;
			}
			if (Shapes[i]->GetID() == 6)
			{
				//text(int id, Vector2f * P, int noOfPoints = 1, Color C = Color::White) :AllShapes(id, P, noOfPoints, C)
				out << Shapes[i]->GetID() << " " << Shapes[i]->GetnoOfPoints() << " ";
				WriteCoordinates(out, Shapes[i]->GetPoints(), Shapes[i]->GetnoOfPoints());
				out << Shapes[i]->GetString() << " " << GetColourID(Shapes[i]->GetColour()) << endl;
			}
		}
	}
}

void ReadCoordinates(ifstream& in, Vector2f* Points, int NoOfPoints);

AllShapes** Read(int& NoofShapes)
{
	ifstream in; int TempPoints; bool TempStyle; int ColourId;
	in.open("Shapes.txt");
	if (in.is_open())
	{
		in >> NoofShapes;
		AllShapes** Shapes = new AllShapes * [NoofShapes];
		for (int i = 0; i < NoofShapes; i++)
		{
			int Id; in >> Id;
			switch (Id)
			{
			case 1:
			{
				//Line(int id, Vector2f * P, int noOfPoints, bool style, Color C = Color::White)

				in >> TempPoints;
				Vector2f* P = new Vector2f[TempPoints];
				ReadCoordinates(in, P, TempPoints);
				in >> TempStyle >> ColourId;
				Shapes[i] = new Line(Id, P, TempPoints, TempStyle, SetColour(ColourId));
				delete[] P;
				break;
			}
			case 2:
			{
				//Rectangles(int id, Vector2f * P, int noOfPoints, Color C = Color::White, int Copy = 0)
				in >> TempPoints;
				Vector2f* P = new Vector2f[TempPoints];
				ReadCoordinates(in, P, TempPoints);
				in >> ColourId;
				Shapes[i] = new Rectangles(Id, P, TempPoints, SetColour(ColourId), 1);
				delete[] P;
				break;
			}
			case 3:
			{
				//Circle(int id, Vector2f * P, int noOfPoints, Color C = Color::White)
				in >> TempPoints;
				Vector2f* P = new Vector2f[TempPoints];
				ReadCoordinates(in, P, 2);
				in >> ColourId;
				Shapes[i] = new Circle(Id, P, TempPoints, SetColour(ColourId));
				delete[] P;
				break;
			}
			case 4:
			{
				//polygon(int id, Vector2f * P, int noOfPoints, Color C = Color::Black)
				in >> TempPoints;
				Vector2f* P = new Vector2f[TempPoints];
				ReadCoordinates(in, P, TempPoints);
				in >> ColourId;
				Shapes[i] = new polygon(Id, P, TempPoints, SetColour(ColourId));
				delete[] P;
				break;
			}
			case 5:
			{
				//curve(int id, Vector2f * P, int noOfPoints, bool style, Color C = Color::White)
				in >> TempPoints;
				Vector2f* P = new Vector2f[TempPoints];
				ReadCoordinates(in, P, TempPoints);
				in >> TempStyle >> ColourId;
				Shapes[i] = new curve(Id, P, TempPoints, TempStyle, SetColour(ColourId));
				delete[] P;
				break;
			}
			case 6:
			{
				string Txt;
				//text(int id, Vector2f * P, int noOfPoints = 1, Color C = Color::White, string Input = '\0')
				in >> TempPoints;
				Vector2f* P = new Vector2f[TempPoints];
				ReadCoordinates(in, P, TempPoints);
				in >> ColourId; getline(in, Txt);
				Shapes[i] = new texts(Id, P, TempPoints, SetColour(ColourId), Txt);
				delete[] P;
				break;
			}
			}
		}
		return Shapes;
	}
	else
	{
		cout << " File does not Exist\n";
		NoofShapes = 0;
		return NULL;
	}
}

void ReadCoordinates(ifstream& in, Vector2f* Points, int NoOfPoints)
{
	for (int i = 0; i < NoOfPoints; i++)
	{
		in >> Points[i].x >> Points[i].y;
	}
}

//------------------------------------------------------------------------------------FUNCTIONS------------------------------------------------------------------------------------//

//------------------------------------------------------------------------------------MAIN CODE------------------------------------------------------------------------------------//

int main()
{
	RenderWindow window(VideoMode(1920, 1080), "Paint");
	bool Draw = true; int NoofShapes = 0, Points = 0; Vector2f* V = NULL;
	AllShapes** Shapes = nullptr;
	//Helper Variables
	bool ButtonShrinked = false;
	int button_No = 0;
	int color_No = 0;
	int points_Got = 0;
	Color color(Color::White);
	//Shapes = Read(NoofShapes);
	Vector2f Vtxt[1];
	texts txt(6, Vtxt);

	//top and left panels

	RectangleShape leftPanel(Vector2f(50.f, 1080.f));
	leftPanel.setFillColor(Color(128, 128, 128));

	RectangleShape TopPanel(Vector2f(1920.f, 150.f));
	TopPanel.setFillColor(Color(128, 128, 128));

	//Textures

	Texture texture1, texture2, texture3, texture4, texture5, texture6, texture7, texture8;
	texture1.loadFromFile("Line.png");
	texture2.loadFromFile("Rectangle.png");
	texture3.loadFromFile("Circle.png");
	texture4.loadFromFile("Polygon.png");
	texture5.loadFromFile("Curve.png");
	texture6.loadFromFile("Text.png");
	texture7.loadFromFile("BucketTool.png");
	texture8.loadFromFile("Eraser.png");

	//shapebuttons

	Sprite line;
	line.setTexture(texture1);
	line.setScale(Vector2f(1.f, 1.f));
	line.setPosition(Vector2f(10.f, 232.f));

	Sprite Rectangle;
	Rectangle.setTexture(texture2);
	Rectangle.setScale(Vector2f(1.f, 1.f));
	Rectangle.setPosition(Vector2f(9.5, 282.f));

	Sprite circle;
	circle.setTexture(texture3);
	circle.setScale(Vector2f(0.85, 0.9));
	circle.setPosition(Vector2f(9.f, 332.f));

	Sprite Polygon;
	Polygon.setTexture(texture4);
	Polygon.setScale(Vector2f(1.f, 1.f));
	Polygon.setPosition(Vector2f(10.f, 382.f));

	Sprite Curve;
	Curve.setTexture(texture5);
	Curve.setScale(Vector2f(1.f, 1.f));
	Curve.setPosition(Vector2f(10.f, 430.f));

	Sprite text;
	text.setTexture(texture6);
	text.setScale(Vector2f(0.95, 1.f));
	text.setPosition(Vector2f(9.f, 471.f));

	Sprite BucketTool;
	BucketTool.setTexture(texture7);
	BucketTool.setScale(2.f, 2.f);
	BucketTool.setOrigin(1.f, 1.f);
	BucketTool.setPosition(321.f, 44.f);

	Sprite Eraser;
	Eraser.setTexture(texture8);
	Eraser.setScale(2.1, 2.3);
	Eraser.setOrigin(1.f, 1.f);
	Eraser.setPosition(400.f, 44.f);

	//Color Buttons

	RectangleShape colorRed;
	colorRed.setSize(Vector2f(25.f, 25.f));
	colorRed.setFillColor(Color::Red);
	colorRed.setPosition(700.f, 35.f);

	RectangleShape colorGreen;
	colorGreen.setSize(Vector2f(25.f, 25.f));
	colorGreen.setFillColor(Color::Green);
	colorGreen.setPosition(750.f, 35.f);

	RectangleShape colorBlue;
	colorBlue.setSize(Vector2f(25.f, 25.f));
	colorBlue.setFillColor(Color::Blue);
	colorBlue.setPosition(800.f, 35.f);

	RectangleShape colorYellow;
	colorYellow.setSize(Vector2f(25.f, 25.f));
	colorYellow.setFillColor(Color::Yellow);
	colorYellow.setPosition(850.f, 35.f);

	RectangleShape colorOrange;
	colorOrange.setSize(Vector2f(25.f, 25.f));
	colorOrange.setFillColor(Color(255, 165, 0));
	colorOrange.setPosition(700.f, 70.f);

	RectangleShape colorWhite;
	colorWhite.setSize(Vector2f(25.f, 25.f));
	colorWhite.setFillColor(Color(255, 255, 255));
	colorWhite.setPosition(750.f, 70.f);

	RectangleShape colorBlack;
	colorBlack.setSize(Vector2f(25.f, 25.f));
	colorBlack.setFillColor(Color(0, 0, 0));
	colorBlack.setPosition(800.f, 70.f);

	RectangleShape colorGold;
	colorGold.setSize(Vector2f(25.f, 25.f));
	colorGold.setFillColor(Color(255, 140, 0));
	colorGold.setPosition(850.f, 70.f);

	//Fonts

	Font font_element;
	font_element.loadFromFile("arial.ttf");

	//Texts

	Text text_element;
	text_element.setCharacterSize(22);
	text_element.setFillColor(Color::Cyan);
	text_element.setFont(font_element);
	text_element.setPosition(50.f, 50.f);

	while (window.isOpen())
	{
		window.setKeyRepeatEnabled(false);
		Event event;

		if ((GetKeyState(VK_ESCAPE) & 0x8000))
		{
			cout << " End ";
			Write(Shapes, NoofShapes);
			Deallocate(Shapes, NoofShapes);
			Draw = false;
			exit(0);
		}

		if (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
			{
				Write(Shapes, NoofShapes);
				Deallocate(Shapes, NoofShapes);
				exit(0);
				Draw = false;
			}

			if (Mouse::isButtonPressed(Mouse::Button::Left) || Mouse::isButtonPressed(Mouse::Button::Right))
			{
				if (isMouseOver(window, leftPanel) || isMouseOver(window, BucketTool) || isMouseOver(window, Eraser))
				{
					if (isMouseOver(window, line))
					{
						if (button_No != 1)
							ShrinkButton(line);
						if (ButtonShrinked)
						{
							if (button_No == 2)
								ExpandButton(Rectangle);
							else if (button_No == 3)
								ExpandButton(circle);
							else if (button_No == 4)
								ExpandButton(Polygon);
							else if (button_No == 5)
								ExpandButton(Curve);
							else if (button_No == 6)
								ExpandButton(text);
							else if (button_No == 7)
								ExpandButton(BucketTool);
							else if (button_No == 8)
								ExpandButton(Eraser);
						}
						button_No = 1;
						ButtonShrinked = true;
					}
					else if (isMouseOver(window, Rectangle))
					{
						if (button_No != 2)
							ShrinkButton(Rectangle);
						if (ButtonShrinked)
						{
							if (button_No == 1)
								ExpandButton(line);
							else if (button_No == 3)
								ExpandButton(circle);
							else if (button_No == 4)
								ExpandButton(Polygon);
							else if (button_No == 5)
								ExpandButton(Curve);
							else if (button_No == 6)
								ExpandButton(text);
							else if (button_No == 7)
								ExpandButton(BucketTool);
							else if (button_No == 8)
								ExpandButton(Eraser);
						}
						button_No = 2;
						ButtonShrinked = true;
					}
					else if (isMouseOver(window, circle))
					{
						if (button_No != 3)
							ShrinkButton(circle);
						ButtonShrinked = true;
						if (ButtonShrinked)
						{
							if (button_No == 1)
								ExpandButton(line);
							else if (button_No == 2)
								ExpandButton(Rectangle);
							else if (button_No == 4)
								ExpandButton(Polygon);
							else if (button_No == 5)
								ExpandButton(Curve);
							else if (button_No == 6)
								ExpandButton(text);
							else if (button_No == 7)
								ExpandButton(BucketTool);
							else if (button_No == 8)
								ExpandButton(Eraser);
						}
						button_No = 3;
					}
					else if (isMouseOver(window, Polygon))
					{
						if (button_No != 4)
							ShrinkButton(Polygon);
						if (ButtonShrinked)
						{
							if (button_No == 1)
								ExpandButton(line);
							else if (button_No == 2)
								ExpandButton(Rectangle);
							else if (button_No == 3)
								ExpandButton(circle);
							else if (button_No == 5)
								ExpandButton(Curve);
							else if (button_No == 6)
								ExpandButton(text);
							else if (button_No == 7)
								ExpandButton(BucketTool);
							else if (button_No == 8)
								ExpandButton(Eraser);
						}
						button_No = 4;
						ButtonShrinked = true;
					}
					else if (isMouseOver(window, Curve))
					{
						if (button_No != 5)
							ShrinkButton(Curve);
						if (ButtonShrinked)
						{
							if (button_No == 1)
								ExpandButton(line);
							else if (button_No == 2)
								ExpandButton(Rectangle);
							else if (button_No == 3)
								ExpandButton(circle);
							else if (button_No == 4)
								ExpandButton(Polygon);
							else if (button_No == 6)
								ExpandButton(text);
							else if (button_No == 7)
								ExpandButton(BucketTool);
							else if (button_No == 8)
								ExpandButton(Eraser);
						}
						button_No = 5;
						ButtonShrinked = true;
					}
					else if (isMouseOver(window, text))
					{
						if (button_No != 6)
							ShrinkButton(text);
						if (ButtonShrinked)
						{
							if (button_No == 1)
								ExpandButton(line);
							else if (button_No == 2)
								ExpandButton(Rectangle);
							else if (button_No == 3)
								ExpandButton(circle);
							else if (button_No == 4)
								ExpandButton(Polygon);
							else if (button_No == 5)
								ExpandButton(Curve);
							else if (button_No == 7)
								ExpandButton(BucketTool);
							else if (button_No == 8)
								ExpandButton(Eraser);
						}
						button_No = 6;
						ButtonShrinked = true;
					}
					else if (isMouseOver(window, BucketTool))
					{
						if (button_No != 7)
							ShrinkButton(BucketTool);
						if (ButtonShrinked)
						{
							if (button_No == 1)
								ExpandButton(line);
							else if (button_No == 2)
								ExpandButton(Rectangle);
							else if (button_No == 3)
								ExpandButton(circle);
							else if (button_No == 4)
								ExpandButton(Polygon);
							else if (button_No == 5)
								ExpandButton(Curve);
							else if (button_No == 6)
								ExpandButton(text);
							else if (button_No == 8)
								ExpandButton(Eraser);
						}
						button_No = 7;
						ButtonShrinked = true;
					}
					else if (isMouseOver(window, Eraser))
					{
						if (button_No != 8)
							ShrinkButton(Eraser);
						if (ButtonShrinked)
						{
							if (button_No == 1)
								ExpandButton(line);
							else if (button_No == 2)
								ExpandButton(Rectangle);
							else if (button_No == 3)
								ExpandButton(circle);
							else if (button_No == 4)
								ExpandButton(Polygon);
							else if (button_No == 5)
								ExpandButton(Curve);
							else if (button_No == 6)
								ExpandButton(text);
							else if (button_No == 7)
								ExpandButton(BucketTool);
						}
						button_No = 8;
						ButtonShrinked = true;
					}
				}
				else if (isMouseOver(window, TopPanel))
				{
					if (isMouseOver(window, colorRed))
					{
						if (color_No != 9)
							ShrinkButton(colorRed);
						if (ButtonShrinked)
						{
							if (color_No == 10)
								ExpandButton(colorGreen);
							else if (color_No == 11)
								ExpandButton(colorBlue);
							else if (color_No == 12)
								ExpandButton(colorYellow);
							else if (color_No == 13)
								ExpandButton(colorOrange);
							else if (color_No == 14)
								ExpandButton(colorWhite);
							else if (color_No == 15)
								ExpandButton(colorBlack);
							else if (color_No == 16)
								ExpandButton(colorGold);
						}
						color_No = 9;
						color = Color::Red;
						ButtonShrinked = true;
					}
					else if (isMouseOver(window, colorGreen))
					{
						if (color_No != 10)
							ShrinkButton(colorGreen);
						if (ButtonShrinked)
						{
							if (color_No == 9)
								ExpandButton(colorRed);
							else if (color_No == 11)
								ExpandButton(colorBlue);
							else if (color_No == 12)
								ExpandButton(colorYellow);
							else if (color_No == 13)
								ExpandButton(colorOrange);
							else if (color_No == 14)
								ExpandButton(colorWhite);
							else if (color_No == 15)
								ExpandButton(colorBlack);
							else if (color_No == 16)
								ExpandButton(colorGold);
						}
						color_No = 10;
						color = Color::Green;
						ButtonShrinked = true;
					}
					else if (isMouseOver(window, colorBlue))
					{
						if (button_No != 11)
							ShrinkButton(colorBlue);
						if (ButtonShrinked)
						{
							if (color_No == 9)
								ExpandButton(colorRed);
							else if (color_No == 10)
								ExpandButton(colorGreen);
							else if (color_No == 12)
								ExpandButton(colorYellow);
							else if (color_No == 13)
								ExpandButton(colorOrange);
							else if (color_No == 14)
								ExpandButton(colorWhite);
							else if (color_No == 15)
								ExpandButton(colorBlack);
							else if (color_No == 16)
								ExpandButton(colorGold);
						}
						color_No = 11;
						color = Color::Blue;
						ButtonShrinked = true;
					}
					else if (isMouseOver(window, colorYellow))
					{
						if (color_No != 12)
							ShrinkButton(colorYellow);
						if (ButtonShrinked)
						{
							if (color_No == 9)
								ExpandButton(colorRed);
							else if (color_No == 10)
								ExpandButton(colorGreen);
							else if (color_No == 11)
								ExpandButton(colorBlue);
							else if (color_No == 13)
								ExpandButton(colorOrange);
							else if (color_No == 14)
								ExpandButton(colorWhite);
							else if (color_No == 15)
								ExpandButton(colorBlack);
							else if (color_No == 16)
								ExpandButton(colorGold);
						}
						color_No = 12;
						color = Color::Yellow;
						ButtonShrinked = true;
					}
					else if (isMouseOver(window, colorOrange))
					{
						if (color_No != 13)
							ShrinkButton(colorOrange);
						if (ButtonShrinked)
						{
							if (color_No == 9)
								ExpandButton(colorRed);
							else if (color_No == 10)
								ExpandButton(colorGreen);
							else if (color_No == 11)
								ExpandButton(colorBlue);
							else if (color_No == 12)
								ExpandButton(colorYellow);
							else if (color_No == 14)
								ExpandButton(colorWhite);
							else if (color_No == 15)
								ExpandButton(colorBlack);
							else if (color_No == 16)
								ExpandButton(colorGold);
						}
						color_No = 13;
						color = Color(255, 165, 0);
						ButtonShrinked = true;
					}
					else if (isMouseOver(window, colorWhite))
					{
						if (color_No != 14)
							ShrinkButton(colorWhite);
						if (ButtonShrinked)
						{
							if (color_No == 9)
								ExpandButton(colorRed);
							else if (color_No == 10)
								ExpandButton(colorGreen);
							else if (color_No == 11)
								ExpandButton(colorBlue);
							else if (color_No == 12)
								ExpandButton(colorYellow);
							else if (color_No == 13)
								ExpandButton(colorOrange);
							else if (color_No == 15)
								ExpandButton(colorBlack);
							else if (color_No == 16)
								ExpandButton(colorGold);
						}
						color_No = 14;
						color = Color::White;
						ButtonShrinked = true;
					}
					else if (isMouseOver(window, colorBlack))
					{
						if (color_No != 15)
							ShrinkButton(colorBlack);
						if (ButtonShrinked)
						{
							if (color_No == 9)
								ExpandButton(colorRed);
							else if (color_No == 10)
								ExpandButton(colorGreen);
							else if (color_No == 11)
								ExpandButton(colorBlue);
							else if (color_No == 12)
								ExpandButton(colorYellow);
							else if (color_No == 13)
								ExpandButton(colorOrange);
							else if (color_No == 14)
								ExpandButton(colorWhite);
							else if (color_No == 16)
								ExpandButton(colorGold);
						}
						color_No = 15;
						color = Color::Black;
						ButtonShrinked = true;
					}
					else if (isMouseOver(window, colorGold))
					{
						if (color_No != 16)
							ShrinkButton(colorGold);
						if (ButtonShrinked)
						{
							if (color_No == 9)
								ExpandButton(colorRed);
							else if (color_No == 10)
								ExpandButton(colorGreen);
							else if (color_No == 11)
								ExpandButton(colorBlue);
							else if (color_No == 12)
								ExpandButton(colorYellow);
							else if (color_No == 13)
								ExpandButton(colorOrange);
							else if (color_No == 14)
								ExpandButton(colorWhite);
							else if (color_No == 15)
								ExpandButton(colorBlack);
						}
						color_No = 16;
						color = Color(255, 140, 0);
						ButtonShrinked = true;
					}
				}
				else if (!isMouseOver(window, leftPanel) && !isMouseOver(window, TopPanel))
				{
					if (button_No == 1) // Line
					{
						if (points_Got != 2)
						{
							V->Extend(V, Points);
							V[points_Got].setVector(Mouse::getPosition(window).x, Mouse::getPosition(window).y);

							//VecLine[points_Got].setVector(Mouse::getPosition(window).x, Mouse::getPosition(window).y);
							cout << "Stored ... " << V[points_Got].x << " " << V[points_Got].y << endl;
							points_Got++;
						}
						if (points_Got == 2)
						{
							Extend(Shapes, NoofShapes);
							Shapes[NoofShapes - 1] = new Line(1, V, points_Got, 0, color);
							points_Got = 0;
							V->Deallocate(V, Points);
						}
					}
					if (button_No == 2) // Rectangle
					{
						if (points_Got != 2)
						{
							V->Extend(V, Points);
							V[points_Got].setVector(Mouse::getPosition(window).x, Mouse::getPosition(window).y);
							//VecR[points_Got].setVector(Mouse::getPosition(window).x, Mouse::getPosition(window).y);
							cout << "Stored ... " << V[points_Got].x << " " << V[points_Got].y << endl;
							points_Got++;
						}
						if (points_Got == 2)
						{
							Extend(Shapes, NoofShapes);
							Shapes[NoofShapes - 1] = new Rectangles(2, V, points_Got, color);
							//newRec.SetRectangle(2, VecR, 2, Color::Red);
							points_Got = 0; V->Deallocate(V, Points);
						}
					}
					if (button_No == 3) // Circle
					{
						if (points_Got != 2)
						{
							V->Extend(V, Points);
							V[points_Got].setVector(Mouse::getPosition(window).x, Mouse::getPosition(window).y);
							cout << "Stored ... " << V[points_Got].x << " " << V[points_Got].y << endl;
							points_Got++;
						}
						if (points_Got == 2)
						{
							Extend(Shapes, NoofShapes);
							//Circle(int id, Vector2f* P, int noOfPoints, Color C = Color::White)
							Shapes[NoofShapes - 1] = new Circle(3, V, points_Got, color);
							points_Got = 0; V->Deallocate(V, Points);
						}
					}
					if (button_No == 4)// Polygon
					{
						if (Mouse::isButtonPressed(Mouse::Button::Left))
						{
							V->Extend(V, Points);
							V[points_Got].setVector(Mouse::getPosition(window).x, Mouse::getPosition(window).y);
							cout << "Stored ... " << V[points_Got].x << " " << V[points_Got].y << endl;
							points_Got++;
						}
						if (Mouse::isButtonPressed(Mouse::Button::Right))
						{
							Extend(Shapes, NoofShapes);
							//polygon(int id, Vector2f* P, int noOfPoints, Color C = Color::Black)
							Shapes[NoofShapes - 1] = new polygon(4, V, points_Got, color);
							points_Got = 0; V->Deallocate(V, Points);
						}
					}
					if (button_No == 5) // Curve
					{
						if (Mouse::isButtonPressed(Mouse::Button::Left))
						{
							V->Extend(V, Points);
							V[points_Got].setVector(Mouse::getPosition(window).x, Mouse::getPosition(window).y);
							cout << "Stored ... " << V[points_Got].x << " " << V[points_Got].y << endl;
							points_Got++;
						}
						if (Mouse::isButtonPressed(Mouse::Button::Right))
						{
							Extend(Shapes, NoofShapes);
							//Curve(int id, Vector2f* P, int noOfPoints, bool style, Color C = Color::White)
							Shapes[NoofShapes - 1] = new curve(5, V, points_Got, 0, color);
							points_Got = 0; V->Deallocate(V, Points);
						}
					}
					if (button_No == 6)
					{
						if (Mouse::isButtonPressed(Mouse::Button::Left))
						{
							Extend(Shapes, NoofShapes); V->Extend(V, Points);
							//text(int id, Vector2f* P, int noOfPoints = 1, Color C = Color::White, string Input = '\0')
							float X = Mouse::getPosition(window).x; float Y = Mouse::getPosition(window).y;
							V[0].x = X; V[0].y = Y;
							//V[0].setVector(Mouse::getPosition(window).x, Mouse::getPosition(window).y);
							Shapes[NoofShapes - 1] = new texts(5, V, 1, color);
							// Shapes[NoofShapes - 1]->SetPosition(V[0].x,V[0].y);
							V->Deallocate(V, Points);
						}
					}
					if (button_No == 7)
					{
						for (int i = 0; i < NoofShapes; i++)
						{
							if (Shapes[i]->Contains(Vector2f(Mouse::getPosition(window).x, Mouse::getPosition(window).y)))
							{
								Shapes[i]->ChangeColour(color);
							}
						}
					}
					if (button_No == 8)
					{
						for (int i = 0; i < NoofShapes; i++)
						{
							if (Shapes[i]->Contains(Vector2f(Mouse::getPosition(window).x, Mouse::getPosition(window).y)))
							{
								RemoveShape(Shapes, NoofShapes, i);
							}
						}
					}
				}
				cout << Mouse::getPosition(window).x << " " << Mouse::getPosition(window).y << endl;
			}
			if (event.type == sf::Event::TextEntered)
			{
				if (button_No == 6)
				{
					if (Keyboard::isKeyPressed(Keyboard::Key::Enter))
						Shapes[NoofShapes - 1]->setString('\n');
					else if (Keyboard::isKeyPressed(Keyboard::Key::Space))
						Shapes[NoofShapes - 1]->setString(' ');
					else if (Keyboard::isKeyPressed(Keyboard::Key::Backspace))
						Shapes[NoofShapes - 1]->removeAlphabet();
					else if (Keyboard::isKeyPressed(Keyboard::Key::A))
						Shapes[NoofShapes - 1]->setString('a');
					else if (Keyboard::isKeyPressed(Keyboard::Key::B))
						Shapes[NoofShapes - 1]->setString('b');
					else if (Keyboard::isKeyPressed(Keyboard::Key::C))
						Shapes[NoofShapes - 1]->setString('c');
					else if (Keyboard::isKeyPressed(Keyboard::Key::D))
						Shapes[NoofShapes - 1]->setString('d');
					else if (Keyboard::isKeyPressed(Keyboard::Key::E))
						Shapes[NoofShapes - 1]->setString('e');
					else if (Keyboard::isKeyPressed(Keyboard::Key::F))
						Shapes[NoofShapes - 1]->setString('f');
					else if (Keyboard::isKeyPressed(Keyboard::Key::G))
						Shapes[NoofShapes - 1]->setString('g');
					else if (Keyboard::isKeyPressed(Keyboard::Key::H))
						Shapes[NoofShapes - 1]->setString('h');
					else if (Keyboard::isKeyPressed(Keyboard::Key::I))
						Shapes[NoofShapes - 1]->setString('i');
					else if (Keyboard::isKeyPressed(Keyboard::Key::J))
						Shapes[NoofShapes - 1]->setString('j');
					else if (Keyboard::isKeyPressed(Keyboard::Key::K))
						Shapes[NoofShapes - 1]->setString('k');
					else if (Keyboard::isKeyPressed(Keyboard::Key::L))
						Shapes[NoofShapes - 1]->setString('l');
					else if (Keyboard::isKeyPressed(Keyboard::Key::M))
						Shapes[NoofShapes - 1]->setString('m');
					else if (Keyboard::isKeyPressed(Keyboard::Key::N))
						Shapes[NoofShapes - 1]->setString('n');
					else if (Keyboard::isKeyPressed(Keyboard::Key::O))
						Shapes[NoofShapes - 1]->setString('o');
					else if (Keyboard::isKeyPressed(Keyboard::Key::P))
						Shapes[NoofShapes - 1]->setString('p');
					else if (Keyboard::isKeyPressed(Keyboard::Key::Q))
						Shapes[NoofShapes - 1]->setString('q');
					else if (Keyboard::isKeyPressed(Keyboard::Key::R))
						Shapes[NoofShapes - 1]->setString('r');
					else if (Keyboard::isKeyPressed(Keyboard::Key::S))
						Shapes[NoofShapes - 1]->setString('s');
					else if (Keyboard::isKeyPressed(Keyboard::Key::T))
						Shapes[NoofShapes - 1]->setString('t');
					else if (Keyboard::isKeyPressed(Keyboard::Key::U))
						Shapes[NoofShapes - 1]->setString('u');
					else if (Keyboard::isKeyPressed(Keyboard::Key::V))
						Shapes[NoofShapes - 1]->setString('v');
					else if (Keyboard::isKeyPressed(Keyboard::Key::W))
						Shapes[NoofShapes - 1]->setString('w');
					else if (Keyboard::isKeyPressed(Keyboard::Key::X))
						Shapes[NoofShapes - 1]->setString('x');
					else if (Keyboard::isKeyPressed(Keyboard::Key::Y))
						Shapes[NoofShapes - 1]->setString('y');
					else if (Keyboard::isKeyPressed(Keyboard::Key::Z))
						Shapes[NoofShapes - 1]->setString('z');
					else if (Keyboard::isKeyPressed(Keyboard::Key::Num0))
						Shapes[NoofShapes - 1]->setString('0');
					else if (Keyboard::isKeyPressed(Keyboard::Key::Num1))
						Shapes[NoofShapes - 1]->setString('1');
					else if (Keyboard::isKeyPressed(Keyboard::Key::Num2))
						Shapes[NoofShapes - 1]->setString('2');
					else if (Keyboard::isKeyPressed(Keyboard::Key::Num3))
						Shapes[NoofShapes - 1]->setString('3');
					else if (Keyboard::isKeyPressed(Keyboard::Key::Num4))
						Shapes[NoofShapes - 1]->setString('4');
					else if (Keyboard::isKeyPressed(Keyboard::Key::Num5))
						Shapes[NoofShapes - 1]->setString('5');
					else if (Keyboard::isKeyPressed(Keyboard::Key::Num6))
						Shapes[NoofShapes - 1]->setString('6');
					else if (Keyboard::isKeyPressed(Keyboard::Key::Num7))
						Shapes[NoofShapes - 1]->setString('7');
					else if (Keyboard::isKeyPressed(Keyboard::Key::Num8))
						Shapes[NoofShapes - 1]->setString('8');
					else if (Keyboard::isKeyPressed(Keyboard::Key::Num9))
						Shapes[NoofShapes - 1]->setString('9');
					else if (Keyboard::isKeyPressed(Keyboard::Key::Comma))
						Shapes[NoofShapes - 1]->setString(',');
					else if (Keyboard::isKeyPressed(Keyboard::Key::Dash))
						Shapes[NoofShapes - 1]->setString('-');
					else if (event.text.unicode == 46)
					{
						Shapes[NoofShapes - 1]->setString('.');
					}
				}
			}
		}
		window.clear();
		window.draw(leftPanel);
		window.draw(TopPanel);
		window.draw(line);
		window.draw(Rectangle);
		window.draw(circle);
		window.draw(Polygon);
		window.draw(Curve);
		window.draw(text);
		window.draw(BucketTool);
		window.draw(Eraser);
		window.draw(colorRed);
		window.draw(colorGreen);
		window.draw(colorBlue);
		window.draw(colorYellow);
		window.draw(colorOrange);
		window.draw(colorWhite);
		window.draw(colorBlack);
		window.draw(colorGold);
		if (Draw)
		{
			for (int i = 0; i < NoofShapes; i++)
			{
				Shapes[i]->Draw(window);
			}
		}
		window.display();
	}
	system("pause");
	return 0;
}