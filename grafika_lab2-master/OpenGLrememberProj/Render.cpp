#include <cmath>

#include "Render.h"

#include <sstream>
#include <iostream>

#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"

#include "GUItextRectangle.h"



bool textureMode = true;
bool lightMode = true;

//класс для настройки камеры
class CustomCamera : public Camera
{
public:
	//дистанция камеры
	double camDist;
	//углы поворота камеры
	double fi1, fi2;

	
	//значния масеры по умолчанию
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}

	
	//считает позицию камеры, исходя из углов поворота, вызывается движком
	void SetUpCamera()
	{
		//отвечает за поворот камеры мышкой
		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist*cos(fi2)*cos(fi1),
			camDist*cos(fi2)*sin(fi1),
			camDist*sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		//функция настройки камеры
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //создаем объект камеры


//Класс для настройки света
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//начальная позиция света
		pos = Vector3(1, 1, 3);
	}

	
	//рисует сферу и линии под источником света, вызывается движком
	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);

		
		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale*0.08;
		s.Show();
		
		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			//линия от источника света до окружности
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//рисуем окруность
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale*1.5;
			c.Show();
		}

	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// параметры источника света
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// характеристики излучаемого света
		// фоновое освещение (рассеянный свет)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// диффузная составляющая света
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// зеркально отражаемая составляющая света
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //создаем источник света




//старые координаты мыши
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL *ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//меняем углы камеры при нажатой левой кнопке мыши
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01*dx;
		camera.fi2 += -0.01*dy;
	}

	
	//двигаем свет по плоскости, в точку где мышь
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k*r.direction.X() + r.origin.X();
		y = k*r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02*dy);
	}

	
}

void mouseWheelEvent(OpenGL *ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01*delta;

}

void keyDownEvent(OpenGL *ogl, int key)
{
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}

	if (key == 'R')
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}
}

void keyUpEvent(OpenGL *ogl, int key)
{
	
}



GLuint texId;
GLuint tex2Id;
GLuint tex3Id;
GLuint tex4Id;
GLuint tex5Id;
GLuint tex6Id;
//выполняется перед первым рендером
void initRender(OpenGL *ogl)
{
	//настройка текстур

	//4 байта на хранение пикселя
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//настройка режима наложения текстур
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//включаем текстуры
	glEnable(GL_TEXTURE_2D);
	

	//массив трехбайтных элементов  (R G B)
	{
		RGBTRIPLE* texarray;

		//массив символов, (высота*ширина*4      4, потомучто   выше, мы указали использовать по 4 байта на пиксель текстуры - R G B A)
		char* texCharArray;
		int texW, texH;
		OpenGL::LoadBMP("texture.bmp", &texW, &texH, &texarray);
		OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);



		//генерируем ИД для текстуры
		glGenTextures(1, &texId);
		//биндим айдишник, все что будет происходить с текстурой, будте происходить по этому ИД
		glBindTexture(GL_TEXTURE_2D, texId);

		//загружаем текстуру в видеопямять, в оперативке нам больше  она не нужна
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

		//отчистка памяти
		free(texCharArray);
		free(texarray);

		//наводим шмон
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}

	{
		RGBTRIPLE* texarray6;

		//массив символов, (высота*ширина*4      4, потомучто   выше, мы указали использовать по 4 байта на пиксель текстуры - R G B A)
		char* texCharArray6;
		int texW6, texH6;
		OpenGL::LoadBMP("texture5.bmp", &texW6, &texH6, &texarray6);
		OpenGL::RGBtoChar(texarray6, texW6, texH6, &texCharArray6);



		//генерируем ИД для текстуры
		glGenTextures(1, &tex6Id);
		//биндим айдишник, все что будет происходить с текстурой, будте происходить по этому ИД
		glBindTexture(GL_TEXTURE_2D, tex6Id);

		//загружаем текстуру в видеопямять, в оперативке нам больше  она не нужна
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW6, texH6, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray6);

		//отчистка памяти
		free(texCharArray6);
		free(texarray6);

		//наводим шмон
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}

	{
		RGBTRIPLE* texarray5;

		//массив символов, (высота*ширина*4      4, потомучто   выше, мы указали использовать по 4 байта на пиксель текстуры - R G B A)
		char* texCharArray5;
		int texW5, texH5;
		OpenGL::LoadBMP("texture4.bmp", &texW5, &texH5, &texarray5);
		OpenGL::RGBtoChar(texarray5, texW5, texH5, &texCharArray5);



		//генерируем ИД для текстуры
		glGenTextures(1, &tex5Id);
		//биндим айдишник, все что будет происходить с текстурой, будте происходить по этому ИД
		glBindTexture(GL_TEXTURE_2D, tex5Id);

		//загружаем текстуру в видеопямять, в оперативке нам больше  она не нужна
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW5, texH5, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray5);

		//отчистка памяти
		free(texCharArray5);
		free(texarray5);

		//наводим шмон
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}

	{
		RGBTRIPLE* texarray4;

		//массив символов, (высота*ширина*4      4, потомучто   выше, мы указали использовать по 4 байта на пиксель текстуры - R G B A)
		char* texCharArray4;
		int texW4, texH4;
		OpenGL::LoadBMP("texture3.bmp", &texW4, &texH4, &texarray4);
		OpenGL::RGBtoChar(texarray4, texW4, texH4, &texCharArray4);



		//генерируем ИД для текстуры
		glGenTextures(1, &tex4Id);
		//биндим айдишник, все что будет происходить с текстурой, будте происходить по этому ИД
		glBindTexture(GL_TEXTURE_2D, tex4Id);

		//загружаем текстуру в видеопямять, в оперативке нам больше  она не нужна
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW4, texH4, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray4);

		//отчистка памяти
		free(texCharArray4);
		free(texarray4);

		//наводим шмон
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}

	{
		RGBTRIPLE* texarray3;

		//массив символов, (высота*ширина*4      4, потомучто   выше, мы указали использовать по 4 байта на пиксель текстуры - R G B A)
		char* texCharArray3;
		int texW3, texH3;
		OpenGL::LoadBMP("texture2.bmp", &texW3, &texH3, &texarray3);
		OpenGL::RGBtoChar(texarray3, texW3, texH3, &texCharArray3);



		//генерируем ИД для текстуры
		glGenTextures(1, &tex3Id);
		//биндим айдишник, все что будет происходить с текстурой, будте происходить по этому ИД
		glBindTexture(GL_TEXTURE_2D, tex3Id);

		//загружаем текстуру в видеопямять, в оперативке нам больше  она не нужна
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW3, texH3, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray3);

		//отчистка памяти
		free(texCharArray3);
		free(texarray3);

		//наводим шмон
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}

	{
		RGBTRIPLE* texarray2;

		//массив символов, (высота*ширина*4      4, потомучто   выше, мы указали использовать по 4 байта на пиксель текстуры - R G B A)
		char* texCharArray2;
		int texW2, texH2;
		OpenGL::LoadBMP("texture1.bmp", &texW2, &texH2, &texarray2);
		OpenGL::RGBtoChar(texarray2, texW2, texH2, &texCharArray2);



		//генерируем ИД для текстуры
		glGenTextures(1, &tex2Id);
		//биндим айдишник, все что будет происходить с текстурой, будте происходить по этому ИД
		glBindTexture(GL_TEXTURE_2D, tex2Id);

		//загружаем текстуру в видеопямять, в оперативке нам больше  она не нужна
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW2, texH2, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray2);

		//отчистка памяти
		free(texCharArray2);
		free(texarray2);

		//наводим шмон
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}
	//камеру и свет привязываем к "движку"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// нормализация нормалей : их длины будет равна 1
	glEnable(GL_NORMALIZE);

	// устранение ступенчатости для линий
	glEnable(GL_LINE_SMOOTH); 


	//   задать параметры освещения
	//  параметр GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  лицевые и изнаночные рисуются одинаково(по умолчанию), 
	//                1 - лицевые и изнаночные обрабатываются разными режимами       
	//                соответственно лицевым и изнаночным свойствам материалов.    
	//  параметр GL_LIGHT_MODEL_AMBIENT - задать фоновое освещение, 
	//                не зависящее от сточников
	// по умолчанию (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	camera.fi1 = -1.3;
	camera.fi2 = 0.8;
}


// Функция для рисования цилиндра
void drawCylinder(float x, float y, float radius, float height, int segments) {
	glBindTexture(GL_TEXTURE_2D, texId);
	glBegin(GL_QUAD_STRIP);
	for (int i = 0; i <= segments; ++i) {
		float theta = i * 2.0f * M_PI / segments;
		float xPos = x + radius * cosf(theta);
		float yPos = y + radius * sinf(theta);
		float texCoord = (float)i / segments;
		glTexCoord2f(texCoord, 0.0f);
		glVertex3f(xPos, yPos, 0.0f);
		glTexCoord2f(texCoord, 1.0f);
		glVertex3f(xPos, yPos, height);
	}
	glEnd();

	// Верхняя и нижняя крышки цилиндра
	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(x, y, 0.0f);
	for (int i = 0; i <= segments; ++i) {
		float theta = i * 2.0f * M_PI / segments;
		float xPos = x + radius * cosf(theta);
		float yPos = y + radius * sinf(theta);
		glVertex3f(xPos, yPos, 0.0f);
	}
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(x, y, height);
	for (int i = 0; i <= segments; ++i) {
		float theta = i * 2.0f * M_PI / segments;
		float xPos = x + radius * cosf(theta);
		float yPos = y + radius * sinf(theta);
		glVertex3f(xPos, yPos, height);
	}
	glEnd();
}

// Функция для рисования конуса
void drawCone(float x, float y, float baseRadius, float height, int segments) {
	glBindTexture(GL_TEXTURE_2D, tex3Id);
	// Рисуем основание конуса
	glBegin(GL_TRIANGLE_FAN);
	glTexCoord2f(0.5f, 0.5f);
	glVertex3f(x, y, 0.0f); // Центральная точка основания
	for (int i = 0; i <= segments; ++i) {
		float theta = i * 2.0f * M_PI / segments;
		float xPos = x + baseRadius * cosf(theta);
		float yPos = y + baseRadius * sinf(theta);
		glTexCoord2f((xPos - x) / (2 * baseRadius) + 0.5f, (yPos - y) / (2 * baseRadius) + 0.5f);
		glVertex3f(xPos, yPos, 0.0f);
	}
	glEnd();

	// Рисуем боковую поверхность конуса
	glBegin(GL_TRIANGLE_FAN);
	glTexCoord2f(0.5f, 0.0f); // Вершина конуса
	glVertex3f(x, y, height);
	for (int i = 0; i <= segments; ++i) {
		float theta = i * 2.0f * M_PI / segments;
		float xPos = x + baseRadius * cosf(theta);
		float yPos = y + baseRadius * sinf(theta);
		float texCoordX = (float)i / segments;
		glTexCoord2f(texCoordX, 1.0f);
		glVertex3f(xPos, yPos, 0.0f);
	}
	glEnd();

	glBindTexture(GL_TEXTURE_2D, 0);
}

// Функция для рисования башни мага
void drawMageTower() {
	float x = 0.0f;
	float y = 5.0f;
	float height = 12.0f;

	// Основная башня (цилиндр)
	glColor3f(0.5f, 0.5f, 0.5f); // Серый цвет
	drawCylinder(x, y, 2.0f, height, 32);

	// Крыша башни (конус)
	glColor3f(0.3f, 0.3f, 0.3f); // Темно-серый цвет
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, height);
	drawCone(x, y, 3.0f, 7.0f, 32);
	glPopMatrix();
}

//



void Render(OpenGL* ogl)
{



	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);


	//альфаналожение
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	//настройка материала
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;


	//фоновая
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//дифузная
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//зеркальная
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); \
		//размер блика
		glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//чтоб было красиво, без квадратиков (сглаживание освещения)
	glShadeModel(GL_SMOOTH);
	//===================================
	//Прогать тут  

	//Замок
	glBindTexture(GL_TEXTURE_2D, texId);
	glBegin(GL_QUADS);
	glColor3f(0.5f, 0.5f, 0.5f);
	glTexCoord2d(0, 0);
	glVertex3d(10, -3, 5);
	glTexCoord2d(0, 1);
	glVertex3d(10, -5, 5);
	glTexCoord2d(1, 0);
	glVertex3d(-10, -5, 5);
	glTexCoord2d(1, 1);
	glVertex3d(-10, -3, 5);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, texId);
	glBegin(GL_QUADS);
	glColor3f(0.5f, 0.5f, 0.5f);
	glTexCoord2d(0, 0);
	glVertex3d(10, 17, 5);
	glTexCoord2d(0, 1);
	glVertex3d(10, 15, 5);
	glTexCoord2d(1, 1);
	glVertex3d(-10, 15, 5);
	glTexCoord2d(1, 0);
	glVertex3d(-10, 17, 5);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, texId);
	glBegin(GL_QUADS);
	glColor3f(0.5f, 0.5f, 0.5f);
	glTexCoord2d(0, 1);
	glVertex3d(-10, -5, 5);
	glTexCoord2d(0, 0);
	glVertex3d(-10, -5, 0);
	glTexCoord2d(1, 0);
	glVertex3d(-10, 17, 0);
	glTexCoord2d(1, 1);
	glVertex3d(-10, 17, 5);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, texId);
	glBegin(GL_QUADS);
	glColor3f(0.5f, 0.5f, 0.5f);
	glTexCoord2d(0, 1);
	glVertex3d(10, -5, 5);
	glTexCoord2d(0, 0);
	glVertex3d(10, -5, 0);
	glTexCoord2d(1, 0);
	glVertex3d(10, 17, 0);
	glTexCoord2d(1, 1);
	glVertex3d(10, 17, 5);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, texId);
	glBegin(GL_QUADS);
	glColor3f(0.5f, 0.5f, 0.5f);
	glTexCoord2d(0, 0);
	glVertex3d(10, -5, 0);
	glTexCoord2d(0, 1);
	glVertex3d(10, -5, 5);
	glTexCoord2d(1, 1);
	glVertex3d(-10, -5, 5);
	glTexCoord2d(1, 0);
	glVertex3d(-10, -5, 0);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, texId);
	glBegin(GL_QUADS);
	glColor3f(0.5f, 0.5f, 0.5f);
	glTexCoord2d(0, 1);
	glVertex3d(10, 17, 5);
	glTexCoord2d(0, 0);
	glVertex3d(10, 17, 0);
	glTexCoord2d(1, 0);
	glVertex3d(2, 17, 0);
	glTexCoord2d(1, 1);
	glVertex3d(2, 17, 5);
	glEnd();

	//Ворота
	glBindTexture(GL_TEXTURE_2D, texId);
	glBegin(GL_QUADS);
	glColor3f(0.5f, 0.5f, 0.5f);
	glTexCoord2d(0, 1);
	glVertex3d(-10, 17, 5);
	glTexCoord2d(0, 0);
	glVertex3d(-10, 17, 0);
	glTexCoord2d(1, 0);
	glVertex3d(-2, 17, 0);
	glTexCoord2d(1, 1);
	glVertex3d(-2, 17, 5);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, texId);
	glBegin(GL_QUADS);
	glTexCoord2d(0, 1);
	glVertex3d(2, 17, 5);
	glTexCoord2d(0, 0);
	glVertex3d(2, 17, 4);
	glTexCoord2d(1, 0);
	glVertex3d(-2, 17, 4);
	glTexCoord2d(1, 1);
	glVertex3d(-2, 17, 5);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, tex5Id);
	glBegin(GL_QUADS);
	glTexCoord2d(0, 0);
	glVertex3d(2, 17, 0);
	glTexCoord2d(0, 1);
	glVertex3d(2, 17, 4);
	glTexCoord2d(1, 1);
	glVertex3d(-2, 17, 4);
	glTexCoord2d(1, 0);
	glVertex3d(-2, 17, 0);
	glEnd();

	//Тропа
	glBindTexture(GL_TEXTURE_2D, texId);
	glBegin(GL_QUADS);
	glTexCoord2d(0, 0);
	glVertex3d(10, 17, 5);
	glTexCoord2d(0, 1);
	glVertex3d(8, 17, 5);
	glTexCoord2d(1, 0);
	glVertex3d(8, -5, 5);
	glTexCoord2d(1, 1);
	glVertex3d(10, -5, 5);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, texId);
	glBegin(GL_QUADS);
	glTexCoord2d(0, 0);
	glVertex3d(-10, 17, 5);
	glTexCoord2d(0, 1);
	glVertex3d(-8, 17, 5);
	glTexCoord2d(1, 0);
	glVertex3d(-8, -5, 5);
	glTexCoord2d(1, 1);
	glVertex3d(-10, -5, 5);
	glEnd();

	//башни
	//1
	glBindTexture(GL_TEXTURE_2D, texId);
	glBegin(GL_QUADS);
	glTexCoord2d(0, 0);
	glVertex3d(-10, 17, 5);
	glTexCoord2d(0, 1);
	glVertex3d(-10, 17, 10);
	glTexCoord2d(1, 1);
	glVertex3d(-8, 17, 10);
	glTexCoord2d(1, 0);
	glVertex3d(-8, 17, 5);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, texId);
	glBegin(GL_QUADS);
	glTexCoord2d(0, 0);
	glVertex3d(-10, 15, 5);
	glTexCoord2d(0, 1);
	glVertex3d(-10, 15, 10);
	glTexCoord2d(1, 1);
	glVertex3d(-8, 15, 10);
	glTexCoord2d(1, 0);
	glVertex3d(-8, 15, 5);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, texId);
	glBegin(GL_QUADS);
	glTexCoord2d(0, 0);
	glVertex3d(-10, 15, 5);
	glTexCoord2d(0, 1);
	glVertex3d(-10, 15, 10);
	glTexCoord2d(1, 1);
	glVertex3d(-10, 17, 10);
	glTexCoord2d(1, 0);
	glVertex3d(-10, 17, 5);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, texId);
	glBegin(GL_QUADS);
	glTexCoord2d(0, 0);
	glVertex3d(-8, 15, 5);
	glTexCoord2d(0, 1);
	glVertex3d(-8, 15, 10);
	glTexCoord2d(1, 1);
	glVertex3d(-8, 17, 10);
	glTexCoord2d(1,0);
	glVertex3d(-8, 17, 5);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, tex3Id);
	glBegin(GL_TRIANGLES);
	glTexCoord2d(0, 0);
	glVertex3d(-8, 15, 10);
	glTexCoord2d(0, 1);
	glVertex3d(-8, 17, 10);
	glTexCoord2d(1, 0);
	glVertex3d(-9, 16, 12);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, tex3Id);
	glBegin(GL_TRIANGLES);
	glTexCoord2d(0, 0);
	glVertex3d(-10, 15, 10);
	glTexCoord2d(0, 1);
	glVertex3d(-10, 17, 10);
	glTexCoord2d(1, 0);
	glVertex3d(-9, 16, 12);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, tex3Id);
	glBegin(GL_TRIANGLES);
	glTexCoord2d(0, 0);
	glVertex3d(-10, 17, 10);
	glTexCoord2d(0, 1);
	glVertex3d(-8, 17, 10);
	glTexCoord2d(1, 0);
	glVertex3d(-9, 16, 12);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, tex3Id);
	glBegin(GL_TRIANGLES);
	glTexCoord2d(0, 0);
	glVertex3d(-10, 15, 10);
	glTexCoord2d(0, 1);
	glVertex3d(-8, 15, 10);
	glTexCoord2d(1, 0);
	glVertex3d(-9, 16, 12);
	glEnd();

	glBegin(GL_LINES);
	glTexCoord2d(0, 0);
	glVertex3d(-9,16,12);
	glTexCoord2d(0, 1);
	glVertex3d(-9,16, 13.5);
	glEnd();

	glBegin(GL_TRIANGLES);
	glTexCoord2d(0, 0);
	glVertex3d(-9, 16, 13.5);
	glTexCoord2d(0, 1);
	glVertex3d(-9, 16, 12.5);
	glTexCoord2d(1, 0);
	glVertex3d(-10, 16, 13);
	glEnd();

	//2
	glBindTexture(GL_TEXTURE_2D, texId);
	glBegin(GL_QUADS);
	glTexCoord2d(0, 0);
	glVertex3d(10, 17, 5);
	glTexCoord2d(0, 1);
	glVertex3d(10, 17, 10);
	glTexCoord2d(1, 1);
	glVertex3d(8, 17, 10);
	glTexCoord2d(1, 0);
	glVertex3d(8, 17, 5);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, texId);
	glBegin(GL_QUADS);
	glTexCoord2d(0, 0);
	glVertex3d(10, 15, 5);
	glTexCoord2d(0, 1);
	glVertex3d(10, 15, 10);
	glTexCoord2d(1, 1);
	glVertex3d(8, 15, 10);
	glTexCoord2d(1, 0);
	glVertex3d(8, 15, 5);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, texId);
	glBegin(GL_QUADS);
	glTexCoord2d(0, 0);
	glVertex3d(10, 15, 5);
	glTexCoord2d(1, 0);
	glVertex3d(10, 15, 10);
	glTexCoord2d(1, 1);
	glVertex3d(10, 17, 10);
	glTexCoord2d(1, 0);
	glVertex3d(10, 17, 5);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, texId);
	glBegin(GL_QUADS);
	glTexCoord2d(0, 0);
	glVertex3d(8, 15, 5);
	glTexCoord2d(0, 1);
	glVertex3d(8, 15, 10);
	glTexCoord2d(1, 1);
	glVertex3d(8, 17, 10);
	glTexCoord2d(1, 0);
	glVertex3d(8, 17, 5);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, tex3Id);
	glBegin(GL_TRIANGLES);
	glTexCoord2d(0, 0);
	glVertex3d(8, 15, 10);
	glTexCoord2d(0, 1);
	glVertex3d(8, 17, 10);
	glTexCoord2d(1, 0);
	glVertex3d(9, 16, 12);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, tex3Id);
	glBegin(GL_TRIANGLES);
	glTexCoord2d(0, 0);
	glVertex3d(10, 15, 10);
	glTexCoord2d(0, 1);
	glVertex3d(10, 17, 10);
	glTexCoord2d(1, 0);
	glVertex3d(9, 16, 12);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, tex3Id);
	glBegin(GL_TRIANGLES);
	glTexCoord2d(0, 0);
	glVertex3d(10, 17, 10);
	glTexCoord2d(0, 1);
	glVertex3d(8, 17, 10);
	glTexCoord2d(1, 0);
	glVertex3d(9, 16, 12);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, tex3Id);
	glBegin(GL_TRIANGLES);
	glTexCoord2d(0, 0);
	glVertex3d(10, 15, 10);
	glTexCoord2d(0, 1);
	glVertex3d(8, 15, 10);
	glTexCoord2d(1, 0);
	glVertex3d(9, 16, 12);
	glEnd();

	glBegin(GL_LINES);
	glTexCoord2d(0, 0);
	glVertex3d(9, 16, 12);
	glTexCoord2d(0, 1);
	glVertex3d(9, 16, 13.5);
	glEnd();

	glBegin(GL_TRIANGLES);
	glTexCoord2d(0, 0);
	glVertex3d(9, 16, 13.5);
	glTexCoord2d(0, 1);
	glVertex3d(9, 16, 12.5);
	glTexCoord2d(1, 0);
	glVertex3d(8, 16, 13);
	glEnd();

	//3
	glBindTexture(GL_TEXTURE_2D, texId);
	glBegin(GL_QUADS);
	glTexCoord2d(0, 0);
	glVertex3d(-10, -5, 5);
	glTexCoord2d(0, 1);
	glVertex3d(-10, -5, 10);
	glTexCoord2d(1, 1);
	glVertex3d(-10, -3, 10);
	glTexCoord2d(1, 0);
	glVertex3d(-10, -3, 5);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, texId);
	glBegin(GL_QUADS);
	glTexCoord2d(0, 0);
	glVertex3d(-8, -5, 5);
	glTexCoord2d(0, 1);
	glVertex3d(-8, -5, 10);
	glTexCoord2d(1, 1);
	glVertex3d(-8, -3, 10);
	glTexCoord2d(1, 0);
	glVertex3d(-8, -3, 5);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, texId);
	glBegin(GL_QUADS);
	glTexCoord2d(0, 0);
	glVertex3d(-8, -5, 5);
	glTexCoord2d(0, 1);
	glVertex3d(-10, -5, 5);
	glTexCoord2d(1, 1);
	glVertex3d(-10, -5, 10);
	glTexCoord2d(1, 0);
	glVertex3d(-8, -5, 10);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, texId);
	glBegin(GL_QUADS);
	glTexCoord2d(0, 0);
	glVertex3d(-8, -3, 5);
	glTexCoord2d(0, 1);
	glVertex3d(-10, -3, 5);
	glTexCoord2d(1, 0);
	glVertex3d(-10, -3, 10);
	glTexCoord2d(1, 1);
	glVertex3d(-8, -3, 10);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, tex3Id);
	glBegin(GL_TRIANGLES);
	glTexCoord2d(0, 0);
	glVertex3d(-10, -5, 10);
	glTexCoord2d(0, 1);
	glVertex3d(-8, -5, 10);
	glTexCoord2d(1, 0);
	glVertex3d(-9, -4, 12);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, tex3Id);
	glBegin(GL_TRIANGLES);
	glTexCoord2d(0, 0);
	glVertex3d(-10, -3, 10);
	glTexCoord2d(0, 1);
	glVertex3d(-8, -3, 10);
	glTexCoord2d(1, 0);
	glVertex3d(-9, -4, 12);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, tex3Id);
	glBegin(GL_TRIANGLES);
	glTexCoord2d(0, 0);
	glVertex3d(-10, -5, 10);
	glTexCoord2d(0, 1);
	glVertex3d(-10, -3, 10);
	glTexCoord2d(1, 0);
	glVertex3d(-9, -4, 12);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, tex3Id);
	glBegin(GL_TRIANGLES);
	glTexCoord2d(0, 0);
	glVertex3d(-8, -5, 10);
	glTexCoord2d(0, 1);
	glVertex3d(-8, -3, 10);
	glTexCoord2d(1, 0);
	glVertex3d(-9, -4, 12);
	glEnd();

	glBegin(GL_LINES);
	glTexCoord2d(0, 0);
	glVertex3d(-9, -4, 12);
	glTexCoord2d(0, 1);
	glVertex3d(-9, -4, 13.5);
	glEnd();

	glBegin(GL_TRIANGLES);
	glTexCoord2d(0, 0);
	glVertex3d(-9, -4, 13.5);
	glTexCoord2d(0, 1);
	glVertex3d(-9, -4, 12.5);
	glTexCoord2d(1, 0);
	glVertex3d(-10, -4, 13);
	glEnd();

	//4
	glBindTexture(GL_TEXTURE_2D, texId);
	glBegin(GL_QUADS);
	glTexCoord2d(0, 0);
	glVertex3d(10, -5, 5);
	glTexCoord2d(0, 1);
	glVertex3d(10, -5, 10);
	glTexCoord2d(1, 1);
	glVertex3d(10, -3, 10);
	glTexCoord2d(1, 0);
	glVertex3d(10, -3, 5);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, texId);
	glBegin(GL_QUADS);
	glTexCoord2d(0, 0);
	glVertex3d(8, -5, 5);
	glTexCoord2d(0, 1);
	glVertex3d(8, -5, 10);
	glTexCoord2d(1, 1);
	glVertex3d(8, -3, 10);
	glTexCoord2d(1, 0);
	glVertex3d(8, -3, 5);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, texId);
	glBegin(GL_QUADS);
	glTexCoord2d(0, 0);
	glVertex3d(8, -5, 5);
	glTexCoord2d(1, 0);
	glVertex3d(10, -5, 5);
	glTexCoord2d(1, 1);
	glVertex3d(10, -5, 10);
	glTexCoord2d(0, 1);
	glVertex3d(8, -5, 10);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, texId);
	glBegin(GL_QUADS);
	glTexCoord2d(0, 0);
	glVertex3d(8, -3, 5);
	glTexCoord2d(1, 0);
	glVertex3d(10, -3, 5);
	glTexCoord2d(1, 1);
	glVertex3d(10, -3, 10);
	glTexCoord2d(0, 1);
	glVertex3d(8, -3, 10);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, tex3Id);
	glBegin(GL_TRIANGLES);
	glTexCoord2d(0, 0);
	glVertex3d(10, -5, 10);
	glTexCoord2d(0, 1);
	glVertex3d(8, -5, 10);
	glTexCoord2d(1, 0);
	glVertex3d(9, -4, 12);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, tex3Id);
	glBegin(GL_TRIANGLES);
	glTexCoord2d(0, 0);
	glVertex3d(10, -3, 10);
	glTexCoord2d(0, 1);
	glVertex3d(8, -3, 10);
	glTexCoord2d(1, 0);
	glVertex3d(9, -4, 12);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, tex3Id);
	glBegin(GL_TRIANGLES);
	glTexCoord2d(0, 0);
	glVertex3d(10, -5, 10);
	glTexCoord2d(0, 1);
	glVertex3d(10, -3, 10);
	glTexCoord2d(1, 0);
	glVertex3d(9, -4, 12);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, tex3Id);
	glBegin(GL_TRIANGLES);
	glTexCoord2d(0, 0);
	glVertex3d(8, -5, 10);
	glTexCoord2d(0, 1);
	glVertex3d(8, -3, 10);
	glTexCoord2d(1, 0);
	glVertex3d(9, -4, 12);
	glEnd();

	glBegin(GL_LINES);
	glTexCoord2d(0, 0);
	glVertex3d(9, -4, 12);
	glTexCoord2d(0, 1);
	glVertex3d(9, -4, 13.5);
	glEnd();

	glBegin(GL_TRIANGLES);
	glTexCoord2d(0, 0);
	glVertex3d(9, -4, 13.5);
	glTexCoord2d(0, 1);
	glVertex3d(9, -4, 12.5);
	glTexCoord2d(1, 0);
	glVertex3d(8, -4, 13);
	glEnd();

	//Забор
	int A1 = -2;
	for (int i = 0; i < 6; i++)
	{
		glBindTexture(GL_TEXTURE_2D, texId);
		glBegin(GL_QUADS);
		glTexCoord2d(0, 0);
		glVertex3d(10, A1, 5);
		glTexCoord2d(0, 1);
		glVertex3d(10, A1, 6);
		glTexCoord2d(1, 1);
		glVertex3d(10, A1 + 1, 6);
		glTexCoord2d(1, 0);
		glVertex3d(10, A1 + 1, 5);
		glEnd();

		glBindTexture(GL_TEXTURE_2D, texId);
		glBegin(GL_QUADS);
		glTexCoord2d(0, 0);
		glVertex3d(9, A1, 5);
		glTexCoord2d(0, 1);
		glVertex3d(9, A1, 6);
		glTexCoord2d(1, 1);
		glVertex3d(9, A1 + 1, 6);
		glTexCoord2d(1, 0);
		glVertex3d(9, A1 + 1, 5);
		glEnd();

		glBindTexture(GL_TEXTURE_2D, texId);
		glBegin(GL_QUADS);
		glTexCoord2d(0, 0);
		glVertex3d(10, A1, 5);
		glTexCoord2d(0, 1);
		glVertex3d(10, A1, 6);
		glTexCoord2d(1, 1);
		glVertex3d(9, A1, 6);
		glTexCoord2d(1, 0);
		glVertex3d(9, A1, 5);
		glEnd();

		glBindTexture(GL_TEXTURE_2D, texId);
		glBegin(GL_QUADS);
		glTexCoord2d(0, 0);
		glVertex3d(10, A1 + 1, 5);
		glTexCoord2d(0, 1);
		glVertex3d(10, A1 + 1, 6);
		glTexCoord2d(1, 1);
		glVertex3d(9, A1 + 1, 6);
		glTexCoord2d(1, 0);
		glVertex3d(9, A1 + 1, 5);
		glEnd();

		glBindTexture(GL_TEXTURE_2D, texId);
		glBegin(GL_QUADS);
		glTexCoord2d(0, 0);
		glVertex3d(10, A1, 6);
		glTexCoord2d(0, 1);
		glVertex3d(10, A1 + 1, 6);
		glTexCoord2d(1, 1);
		glVertex3d(9, A1 + 1, 6);
		glTexCoord2d(1, 0);
		glVertex3d(9, A1, 6);
		glEnd();

		glBindTexture(GL_TEXTURE_2D, texId);
		glBegin(GL_QUADS);
		glTexCoord2d(0, 0);
		glVertex3d(-10, A1, 5);
		glTexCoord2d(0, 1);
		glVertex3d(-10, A1, 6);
		glTexCoord2d(1, 1);
		glVertex3d(-10, A1 + 1, 6);
		glTexCoord2d(1, 0);
		glVertex3d(-10, A1 + 1, 5);

		glBindTexture(GL_TEXTURE_2D, texId);
		glBegin(GL_QUADS);
		glTexCoord2d(0, 0);
		glVertex3d(-9, A1, 5);
		glTexCoord2d(0, 1);
		glVertex3d(-9, A1, 6);
		glTexCoord2d(1, 1);
		glVertex3d(-9, A1 + 1, 6);
		glTexCoord2d(1, 0);
		glVertex3d(-9, A1 + 1, 5);

		glBindTexture(GL_TEXTURE_2D, texId);
		glBegin(GL_QUADS);
		glTexCoord2d(0, 0);
		glVertex3d(-10, A1, 5);
		glTexCoord2d(0, 1);
		glVertex3d(-10, A1, 6);
		glTexCoord2d(1, 1);
		glVertex3d(-9, A1, 6);
		glTexCoord2d(1, 0);
		glVertex3d(-9, A1, 5);

		glBindTexture(GL_TEXTURE_2D, texId);
		glBegin(GL_QUADS);
		glTexCoord2d(0, 0);
		glVertex3d(-10, A1 + 1, 5);
		glTexCoord2d(0, 1);
		glVertex3d(-10, A1 + 1, 6);
		glTexCoord2d(1, 1);
		glVertex3d(-9, A1 + 1, 6);
		glTexCoord2d(1, 0);
		glVertex3d(-9, A1 + 1, 5);

		glBindTexture(GL_TEXTURE_2D, texId);
		glBegin(GL_QUADS);
		glTexCoord2d(0, 0);
		glVertex3d(-10, A1 + 1, 6);
		glTexCoord2d(0, 1);
		glVertex3d(-10, A1, 6);
		glTexCoord2d(1, 1);
		glVertex3d(-9, A1, 6);
		glTexCoord2d(1, 0);
		glVertex3d(-9, A1 + 1, 6);

		A1 += 3;
		glEnd();
	}
	int A2 = -7;
	for (int i = 0; i < 5; i++)
	{
		glBegin(GL_QUADS);
		glTexCoord2d(0, 0);
		glVertex3d(A2, 17, 5);
		glTexCoord2d(0, 1);
		glVertex3d(A2, 17, 6);
		glTexCoord2d(1, 1);
		glVertex3d(A2 + 1, 17, 6);
		glTexCoord2d(1, 0);
		glVertex3d(A2 + 1, 17, 5);
		glEnd();

		glBegin(GL_QUADS);
		glTexCoord2d(0, 0);
		glVertex3d(A2, 16, 5);
		glTexCoord2d(0, 1);
		glVertex3d(A2, 16, 6);
		glTexCoord2d(1, 1);
		glVertex3d(A2 + 1, 16, 6);
		glTexCoord2d(1, 0);
		glVertex3d(A2 + 1, 16, 5);
		glEnd();

		glBegin(GL_QUADS);
		glTexCoord2d(0, 0);
		glVertex3d(A2, 16, 5);
		glTexCoord2d(0, 1);
		glVertex3d(A2, 16, 6);
		glTexCoord2d(1, 1);
		glVertex3d(A2, 17, 6);
		glTexCoord2d(1, 0);
		glVertex3d(A2, 17, 5);
		glEnd();

		glBegin(GL_QUADS);
		glTexCoord2d(0, 0);
		glVertex3d(A2 + 1, 16, 5);
		glTexCoord2d(0, 1);
		glVertex3d(A2 + 1, 16, 6);
		glTexCoord2d(1, 1);
		glVertex3d(A2 + 1, 17, 6);
		glTexCoord2d(1, 0);
		glVertex3d(A2 + 1, 17, 5);
		glEnd();

		glBegin(GL_QUADS);
		glTexCoord2d(0, 0);
		glVertex3d(A2, 16, 6);
		glTexCoord2d(0, 1);
		glVertex3d(A2 + 1, 16, 6);
		glTexCoord2d(1, 1);
		glVertex3d(A2 + 1, 17, 6);
		glTexCoord2d(1, 0);
		glVertex3d(A2, 17, 6);
		glEnd();

		glBegin(GL_QUADS);
		glTexCoord2d(0, 0);
		glVertex3d(A2, -5, 5);
		glTexCoord2d(0, 1);
		glVertex3d(A2, -5, 6);
		glTexCoord2d(1, 1);
		glVertex3d(A2 + 1, -5, 6);
		glTexCoord2d(1, 0);
		glVertex3d(A2 + 1, -5, 5);
		glEnd();

		glBegin(GL_QUADS);
		glTexCoord2d(0, 0);
		glVertex3d(A2, -4, 5);
		glTexCoord2d(0, 1);
		glVertex3d(A2, -4, 6);
		glTexCoord2d(1, 1);
		glTexCoord2d(1, 0);
		glVertex3d(A2 + 1, -4, 6);
		glVertex3d(A2 + 1, -4, 5);

		glBegin(GL_QUADS);
		glTexCoord2d(0, 0);
		glVertex3d(A2, -4, 5);
		glTexCoord2d(0, 1);
		glVertex3d(A2, -4, 6);
		glTexCoord2d(1, 1);
		glVertex3d(A2, -5, 6);
		glTexCoord2d(1, 0);
		glVertex3d(A2, -5, 5);

		glBegin(GL_QUADS);
		glTexCoord2d(0, 0);
		glVertex3d(A2 + 1, -4, 5);
		glTexCoord2d(0, 1);
		glVertex3d(A2 + 1, -4, 6);
		glTexCoord2d(1, 1);
		glVertex3d(A2 + 1, -5, 6);
		glTexCoord2d(1, 0);
		glVertex3d(A2 + 1, -5, 5);
		glEnd();

		glBegin(GL_QUADS);
		glTexCoord2d(0, 0);
		glVertex3d(A2, -4, 6);
		glTexCoord2d(0, 1);
		glVertex3d(A2 + 1, -4, 6);
		glTexCoord2d(1, 1);
		glVertex3d(A2 + 1, -5, 6);
		glTexCoord2d(1, 0);
		glVertex3d(A2, -5, 6);
		glEnd();

		A2 += 3;
	}

	float A;
	float B;
	float C[100] = { -20.5, 30, 15, -16, -20, 32 , 15, -25, 20 , - 30 , 32, - 16, -5, 20 , -10, 16, 20, -12, -25 , 0 , 18 , 0 };
	for (int i = 0; i < 12; i++)
	{
		A = C[i];
		B = C[i+1];
		glBegin(GL_QUADS);
		glColor3f(0.545f, 0.271f, 0.075f);
		glVertex3d(A, B, 1);
		glVertex3d(A - 0.5, B, 1);
		glVertex3d(A - 0.5, B - 0.5, 1);
		glVertex3d(A, B - 0.5, 1);
		glEnd();

		glBegin(GL_QUADS);
		glColor3f(0.545f, 0.271f, 0.075f);
		glVertex3d(A, B, 0);
		glVertex3d(A, B, 1);
		glVertex3d(A - 0.5, B, 1);
		glVertex3d(A - 0.5, B, 0);
		glEnd();

		glBegin(GL_QUADS);
		glColor3f(0.545f, 0.271f, 0.075f);
		glVertex3d(A, B, 0);
		glVertex3d(A, B, 1);
		glVertex3d(A, B - 0.5, 1);
		glVertex3d(A, B - 0.5, 0);
		glEnd();

		glBegin(GL_QUADS);
		glColor3f(0.545f, 0.271f, 0.075f);
		glVertex3d(A - 0.5, B - 0.5, 0);
		glVertex3d(A - 0.5, B - 0.5, 1);
		glVertex3d(A - 0.5, B, 1);
		glVertex3d(A - 0.5, B, 0);
		glEnd();

		glBegin(GL_QUADS);
		glColor3f(0.545f, 0.271f, 0.075f);
		glVertex3d(A - 0.5, B - 0.5, 0);
		glVertex3d(A - 0.5, B - 0.5, 1);
		glVertex3d(A, B - 0.5, 1);
		glVertex3d(A, B - 0.5, 0);
		glEnd();

		//Треугольники
		glBindTexture(GL_TEXTURE_2D, tex4Id);
		glBegin(GL_QUADS);
		glColor3f(0.0f, 1.0f, 0.0f);
		glTexCoord2d(0, 0);
		glVertex3d(A - 1, B - 1, 1);
		glTexCoord2d(0, 1);
		glVertex3d(A - 1, B + 0.5, 1);
		glTexCoord2d(1, 1);
		glVertex3d(A + 0.5, B + 0.5, 1);
		glTexCoord2d(1, 0);
		glVertex3d(A + 0.5, B - 1, 1);
		glEnd();

		glBindTexture(GL_TEXTURE_2D, tex4Id);
		glBegin(GL_TRIANGLES);
		glColor3f(0.0f, 1.0f, 0.0f);
		glTexCoord2d(0, 0);
		glVertex3d(A - 1, B - 1, 1);
		glTexCoord2d(0, 1);
		glVertex3d(A - 1, B + 0.5, 1);
		glTexCoord2d(1, 0);
		glVertex3d(A - 0.25, B - 0.25, 3);
		glEnd();

		glBindTexture(GL_TEXTURE_2D, tex4Id);
		glBegin(GL_TRIANGLES);
		glColor3f(0.0f, 1.0f, 0.0f);
		glTexCoord2d(0, 0);
		glVertex3d(A - 1, B - 1, 1);
		glTexCoord2d(0, 1);
		glVertex3d(A + 0.5, B - 1, 1);
		glTexCoord2d(1, 0);
		glVertex3d(A - 0.25, B - 0.25, 3);
		glEnd();

		glBindTexture(GL_TEXTURE_2D, tex4Id);
		glBegin(GL_TRIANGLES);
		glColor3f(0.0f, 1.0f, 0.0f);
		glTexCoord2d(0, 0);
		glVertex3d(A - 1, B + 0.5, 1);
		glTexCoord2d(0, 1);
		glVertex3d(A + 0.5, B + 0.5, 1);
		glTexCoord2d(1, 0);
		glVertex3d(A - 0.25, B - 0.25, 3);
		glEnd();

		glBindTexture(GL_TEXTURE_2D, tex4Id);
		glBegin(GL_TRIANGLES);
		glColor3f(0.0f, 1.0f, 0.0f);
		glTexCoord2d(0, 0);
		glVertex3d(A + 0.5, B + 0.5, 1);
		glTexCoord2d(0, 1);
		glVertex3d(A + 0.5, B - 1, 1);
		glTexCoord2d(1, 0);
		glVertex3d(A - 0.25, B - 0.25, 3);
		glEnd();

		//

		glBindTexture(GL_TEXTURE_2D, tex4Id);
		glBegin(GL_QUADS);
		glColor3f(0.0f, 1.0f, 0.0f);
		glTexCoord2d(0, 0);
		glVertex3d(A - 1, B - 1, 2);
		glTexCoord2d(0, 1);
		glVertex3d(A - 1, B + 0.5, 2);
		glTexCoord2d(1, 1);
		glVertex3d(A + 0.5, B + 0.5, 2);
		glTexCoord2d(1, 0);
		glVertex3d(A + 0.5, B - 1, 2);
		glEnd();

		glBindTexture(GL_TEXTURE_2D, tex4Id);
		glBegin(GL_TRIANGLES);
		glColor3f(0.0f, 1.0f, 0.0f);
		glTexCoord2d(0, 0);
		glVertex3d(A - 1, B - 1, 2);
		glTexCoord2d(0, 1);
		glVertex3d(A - 1, B + 0.5, 2);
		glTexCoord2d(1, 0);
		glVertex3d(A - 0.25, B - 0.25, 4);
		glEnd();

		glBindTexture(GL_TEXTURE_2D, tex4Id);
		glBegin(GL_TRIANGLES);
		glColor3f(0.0f, 1.0f, 0.0f);
		glTexCoord2d(0, 0);
		glVertex3d(A - 1, B - 1, 2);
		glTexCoord2d(0, 1);
		glVertex3d(A + 0.5, B - 1, 2);
		glTexCoord2d(1, 0);
		glVertex3d(A - 0.25, B - 0.25, 4);
		glEnd();

		glBindTexture(GL_TEXTURE_2D, tex4Id);
		glBegin(GL_TRIANGLES);
		glColor3f(0.0f, 1.0f, 0.0f);
		glTexCoord2d(0, 0);
		glVertex3d(A - 1, B + 0.5, 2);
		glTexCoord2d(0, 1);
		glVertex3d(A + 0.5, B + 0.5, 2);
		glTexCoord2d(1, 0);
		glVertex3d(A - 0.25, B - 0.25, 4);
		glEnd();

		glBindTexture(GL_TEXTURE_2D, tex4Id);
		glBegin(GL_TRIANGLES);
		glColor3f(0.0f, 1.0f, 0.0f);
		glTexCoord2d(0, 0);
		glVertex3d(A + 0.5, B + 0.5, 2);
		glTexCoord2d(0, 1);
		glVertex3d(A + 0.5, B - 1, 2);
		glTexCoord2d(1, 0);
		glVertex3d(A - 0.25, B - 0.25, 4);
		glEnd();
		
	}

	//Земля(плоскость)
	glBindTexture(GL_TEXTURE_2D, tex2Id);
	glBegin(GL_QUADS);
	glColor3f(0.545f, 0.271f, 0.075f);
	glTexCoord2d(0, 0);
	glVertex3d(40, 40, 0);
	glTexCoord2d(0, 1);
	glVertex3d(40, -40, 0);
	glTexCoord2d(1, 1);
	glVertex3d(-40, -40, 0);
	glTexCoord2d(1, 0);
	glVertex3d(-40, 40, 0);
	glEnd();

	drawMageTower();

	glBindTexture(GL_TEXTURE_2D, texId);
	glBegin(GL_QUADS);
	glTexCoord2d(0, 0);
	glVertex3d(10, 17, 0.1);
	glTexCoord2d(0, 1);
	glVertex3d(10, -5, 0.1);
	glTexCoord2d(1, 1);
	glVertex3d(-10, -5, 0.1);
	glTexCoord2d(1, 0);
	glVertex3d(-10, 17, 0.1);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, tex3Id);
	glBegin(GL_LINES);
	glTexCoord2d(0, 0);
	glVertex3f(0, 5, 18);
	glTexCoord2d(0, 1);
	glVertex3f(0, 5, 23);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, tex3Id);
	glBegin(GL_TRIANGLES);
	glTexCoord2d(0, 0);
	glVertex3d(0,5,23);
	glTexCoord2d(0, 1);
	glVertex3d(0,5,21);
	glTexCoord2d(1, 0);
	glVertex3d(-3,5,22);
	glEnd();

	// Хижина
	glBindTexture(GL_TEXTURE_2D, texId);
	glBegin(GL_QUADS);
	glTexCoord2d(0, 0);
	glVertex3d(0, -20, 4);
	glTexCoord2d(0, 1);
	glVertex3d(0, -25, 4);
	glTexCoord2d(1, 1);
	glVertex3d(-10, -25, 4);
	glTexCoord2d(1, 0);
	glVertex3d(-10, -20, 4);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, texId);
	glBegin(GL_QUADS);
	glTexCoord2d(0, 0);
	glVertex3d(0, -20, 0);
	glTexCoord2d(0, 1);
	glVertex3d(0, -25, 0);
	glTexCoord2d(1, 1);
	glVertex3d(-10, -25, 0);
	glTexCoord2d(1, 0);
	glVertex3d(-10, -20, 0);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, texId);
	glBegin(GL_QUADS);
	glTexCoord2d(0, 0);
	glVertex3d(0, -20, 0);
	glTexCoord2d(0, 1);
	glVertex3d(0, -25, 0);
	glTexCoord2d(1, 1);
	glVertex3d(0, -25, 4);
	glTexCoord2d(1, 0);
	glVertex3d(0, -20, 4);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, texId);
	glBegin(GL_QUADS);
	glTexCoord2d(0, 0);
	glVertex3d(-10, -20, 0);
	glTexCoord2d(0, 1);
	glVertex3d(-10, -25, 0);
	glTexCoord2d(1, 1);
	glVertex3d(-10, -25, 4);
	glTexCoord2d(1, 0);
	glVertex3d(-10, -20, 4);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, texId);
	glBegin(GL_QUADS);
	glTexCoord2d(0, 0);
	glVertex3d(0, -25, 0);
	glTexCoord2d(0, 1);
	glVertex3d(0, -25, 4);
	glTexCoord2d(1, 1);
	glVertex3d(-10, -25, 4);
	glTexCoord2d(1, 0);
	glVertex3d(-10, -25, 0);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, texId);
	glBegin(GL_QUADS);
	glTexCoord2d(0, 0);
	glVertex3d(0, -20, 0);
	glTexCoord2d(0, 1);
	glVertex3d(0, -20, 4);
	glTexCoord2d(1, 1);
	glVertex3d(-4, -20, 4);
	glTexCoord2d(1, 0);
	glVertex3d(-4, -20, 0);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, texId);
	glBegin(GL_QUADS);
	glTexCoord2d(0, 0);
	glVertex3d(-6, -20, 0);
	glTexCoord2d(0, 1);
	glVertex3d(-6, -20, 4);
	glTexCoord2d(1, 1);
	glVertex3d(-10, -20, 4);
	glTexCoord2d(1, 0);
	glVertex3d(-10, -20, 0);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, tex5Id);
	glBegin(GL_QUADS);
	glTexCoord2d(0, 0);
	glVertex3d(-6, -20, 0);
	glTexCoord2d(0, 1);
	glVertex3d(-6, -20, 4);
	glTexCoord2d(1, 1);
	glVertex3d(-4, -20, 4);
	glTexCoord2d(1, 0);
	glVertex3d(-4, -20, 0);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, tex5Id);
	glBegin(GL_QUADS);
	glTexCoord2d(0, 0);
	glVertex3d(-10, -20, 4);
	glTexCoord2d(0, 1);
	glVertex3d(-10, -22.5, 6);
	glTexCoord2d(1, 1);
	glVertex3d(0, -22.5, 6);
	glTexCoord2d(1, 0);
	glVertex3d(-0, -20, 4);
	glEnd();
	
	glBindTexture(GL_TEXTURE_2D, tex5Id);
	glBegin(GL_QUADS);
	glTexCoord2d(0, 0);
	glVertex3d(-10, -25, 4);
	glTexCoord2d(0, 1);
	glVertex3d(-10, -22.5, 6);
	glTexCoord2d(1, 1);
	glVertex3d(0, -22.5, 6);
	glTexCoord2d(1, 0);
	glVertex3d(-0, -25, 4);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, tex5Id);
	glBegin(GL_TRIANGLES);
	glTexCoord2d(0, 0);
	glVertex3d(-10, -20, 4);
	glTexCoord2d(0, 1);
	glVertex3d(-10, -25, 4);
	glTexCoord2d(1, 0);
	glVertex3d(-10,-22.5,6);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, tex5Id);
	glBegin(GL_TRIANGLES);
	glTexCoord2d(0, 0);
	glVertex3d(0, -20, 4);
	glTexCoord2d(0, 1);
	glVertex3d(0, -25, 4);
	glTexCoord2d(1, 0);
	glVertex3d(0, -22.5, 6);
	glEnd();


	//Ограждение
	
	glBindTexture(GL_TEXTURE_2D, texId);
	glBegin(GL_QUADS);
	glColor3f(0.5f, 0.5f, 0.5f);
	glTexCoord2d(0, 0);
	glVertex3d(-10, 5, 5);
	glTexCoord2d(0, 1);
	glVertex3d(-10, 3, 5);
	glTexCoord2d(1, 1);
	glVertex3d(-40, 3, 5);
	glTexCoord2d(1, 0);
	glVertex3d(-40, 5, 5);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, texId);
	glBegin(GL_QUADS);
	glColor3f(0.5f, 0.5f, 0.5f);
	glTexCoord2d(0, 0);
	glVertex3d(-10, 5, 0);
	glTexCoord2d(0, 1);
	glVertex3d(-10, 5, 5);
	glTexCoord2d(1, 1);
	glVertex3d(-40, 5, 5);
	glTexCoord2d(1, 0);
	glVertex3d(-40, 5, 0);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, texId);
	glBegin(GL_QUADS);
	glColor3f(0.5f, 0.5f, 0.5f);
	glTexCoord2d(0, 0);
	glVertex3d(-10, 3, 0);
	glTexCoord2d(0, 1);
	glVertex3d(-10, 3, 5);
	glTexCoord2d(1, 1);
	glVertex3d(-40, 3, 5);
	glTexCoord2d(1, 0);
	glVertex3d(-40, 3, 0);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, texId);
	glBegin(GL_QUADS);
	glColor3f(0.5f, 0.5f, 0.5f);
	glTexCoord2d(0, 0);
	glVertex3d(-40, 3, 0);
	glTexCoord2d(0, 1);
	glVertex3d(-40, 3, 5);
	glTexCoord2d(1, 1);
	glVertex3d(-40, 5, 5);
	glTexCoord2d(1, 0);
	glVertex3d(-40, 5, 0);
	glEnd();

	A2 = -10;
	for (int i = 0; i < 10; i++)
	{
		glBindTexture(GL_TEXTURE_2D, texId);
		glBegin(GL_QUADS);
		glTexCoord2d(0, 0);
		glVertex3d(A2, 5, 5);
		glTexCoord2d(0, 1);
		glVertex3d(A2, 5, 6);
		glTexCoord2d(1, 1);
		glVertex3d(A2 - 1, 5, 6);
		glTexCoord2d(1, 0);
		glVertex3d(A2 - 1, 5, 5);
		glEnd();

		glBindTexture(GL_TEXTURE_2D, texId);
		glBegin(GL_QUADS);
		glTexCoord2d(0, 0);
		glVertex3d(A2, 5, 6);
		glTexCoord2d(0, 1);
		glVertex3d(A2, 4, 6);
		glTexCoord2d(1, 1);
		glVertex3d(A2-1, 4, 6);
		glTexCoord2d(1, 0);
		glVertex3d(A2 - 1, 5, 6);
		glEnd();

		glBindTexture(GL_TEXTURE_2D, texId);
		glBegin(GL_QUADS);
		glTexCoord2d(0, 0);
		glVertex3d(A2-1, 5, 5);
		glTexCoord2d(0, 1);
		glVertex3d(A2-1, 5, 6);
		glTexCoord2d(1, 1);
		glVertex3d(A2 - 1, 4, 6);
		glTexCoord2d(1, 0);
		glVertex3d(A2 - 1, 4, 5);
		glEnd();

		glBindTexture(GL_TEXTURE_2D, texId);
		glBegin(GL_QUADS);
		glTexCoord2d(0, 0);
		glVertex3d(A2 - 1, 4, 5);
		glTexCoord2d(0, 1);
		glVertex3d(A2 - 1, 4, 6);
		glTexCoord2d(1, 1);
		glVertex3d(A2, 4, 6);
		glTexCoord2d(1, 0);
		glVertex3d(A2, 4, 5);
		glEnd();

		A2 -= 3;
	}

	glBindTexture(GL_TEXTURE_2D, texId);
	glBegin(GL_QUADS);
	glColor3f(0.5f, 0.5f, 0.5f);
	glTexCoord2d(0, 0);
	glVertex3d(10, 5, 5);
	glTexCoord2d(0, 1);
	glVertex3d(10, 3, 5);
	glTexCoord2d(1, 1);
	glVertex3d(40, 3, 5);
	glTexCoord2d(1, 0);
	glVertex3d(40, 5, 5);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, texId);
	glBegin(GL_QUADS);
	glColor3f(0.5f, 0.5f, 0.5f);
	glTexCoord2d(0, 0);
	glVertex3d(10, 5, 0);
	glTexCoord2d(0, 1);
	glVertex3d(10, 5, 5);
	glTexCoord2d(1, 1);
	glVertex3d(40, 5, 5);
	glTexCoord2d(1, 0);
	glVertex3d(40, 5, 0);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, texId);
	glBegin(GL_QUADS);
	glColor3f(0.5f, 0.5f, 0.5f);
	glTexCoord2d(0, 0);
	glVertex3d(10, 3, 0);
	glTexCoord2d(0, 1);
	glVertex3d(10, 3, 5);
	glTexCoord2d(1, 1);
	glVertex3d(40, 3, 5);
	glTexCoord2d(1, 0);
	glVertex3d(40, 3, 0);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, texId);
	glBegin(GL_QUADS);
	glColor3f(0.5f, 0.5f, 0.5f);
	glTexCoord2d(0, 0);
	glVertex3d(40, 3, 0);
	glTexCoord2d(0, 1);
	glVertex3d(40, 3, 5);
	glTexCoord2d(1, 1);
	glVertex3d(40, 5, 5);
	glTexCoord2d(1, 0);
	glVertex3d(40, 5, 0);
	glEnd();

	A2 = 10;
	for (int i = 0; i < 10; i++)
	{
		glBindTexture(GL_TEXTURE_2D, texId);
		glBegin(GL_QUADS);
		glTexCoord2d(0, 0);
		glVertex3d(A2, 5, 5);
		glTexCoord2d(0, 1);
		glVertex3d(A2, 5, 6);
		glTexCoord2d(1, 1);
		glVertex3d(A2 + 1, 5, 6);
		glTexCoord2d(1, 0);
		glVertex3d(A2 + 1, 5, 5);
		glEnd();

		glBindTexture(GL_TEXTURE_2D, texId);
		glBegin(GL_QUADS);
		glTexCoord2d(0, 0);
		glVertex3d(A2, 5, 6);
		glTexCoord2d(0, 1);
		glVertex3d(A2, 4, 6);
		glTexCoord2d(1, 1);
		glVertex3d(A2 + 1, 4, 6);
		glTexCoord2d(1, 0);
		glVertex3d(A2 + 1, 5, 6);
		glEnd();

		glBindTexture(GL_TEXTURE_2D, texId);
		glBegin(GL_QUADS);
		glTexCoord2d(0, 0);
		glVertex3d(A2 + 1, 5, 5);
		glTexCoord2d(0, 1);
		glVertex3d(A2 + 1, 5, 6);
		glTexCoord2d(1, 1);
		glVertex3d(A2 + 1, 4, 6);
		glTexCoord2d(1, 0);
		glVertex3d(A2 + 1, 4, 5);
		glEnd();

		glBindTexture(GL_TEXTURE_2D, texId);
		glBegin(GL_QUADS);
		glTexCoord2d(0, 0);
		glVertex3d(A2 + 1, 4, 5);
		glTexCoord2d(0, 1);
		glVertex3d(A2 + 1, 4, 6);
		glTexCoord2d(1, 1);
		glVertex3d(A2, 4, 6);
		glTexCoord2d(1, 0);
		glVertex3d(A2, 4, 5);
		glEnd();

		A2 += 3;
	}
	
	//Пасхалка
	glBindTexture(GL_TEXTURE_2D, tex5Id);
	glBegin(GL_LINES);
	glTexCoord2d(0, 0);
	glVertex3f(-20,-20, 0);
	glTexCoord2d(0, 0);
	glVertex3f(-20, -20,15);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, tex6Id);
	glBegin(GL_QUADS);
	glTexCoord2d(0, 1);
	glVertex3d(-20,-20,15);
	glTexCoord2d(0, 0);
	glVertex3d(-20,-20,11);
	glTexCoord2d(1, 0);
	glVertex3d(-27,-20,11);
	glTexCoord2d(1, 1);
	glVertex3d(-27, -20, 15);
	glEnd();

   //Сообщение вверху экрана

	
	glMatrixMode(GL_PROJECTION);	//Делаем активной матрицу проекций. 
	                                //(всек матричные операции, будут ее видоизменять.)
	glPushMatrix();   //сохраняем текущую матрицу проецирования (которая описывает перспективную проекцию) в стек 				    
	glLoadIdentity();	  //Загружаем единичную матрицу
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);	 //врубаем режим ортогональной проекции

	glMatrixMode(GL_MODELVIEW);		//переключаемся на модел-вью матрицу
	glPushMatrix();			  //сохраняем текущую матрицу в стек (положение камеры, фактически)
	glLoadIdentity();		  //сбрасываем ее в дефолт

	glDisable(GL_LIGHTING);



	GuiTextRectangle rec;		   //классик моего авторства для удобной работы с рендером текста.
	rec.setSize(300, 150);
	rec.setPosition(10, ogl->getHeight() - 150 - 10);


	std::stringstream ss;
	ss << "T - вкл/выкл текстур" << std::endl;
	ss << "L - вкл/выкл освещение" << std::endl;
	ss << "F - Свет из камеры" << std::endl;
	ss << "G - двигать свет по горизонтали" << std::endl;
	ss << "G+ЛКМ двигать свет по вертекали" << std::endl;
	ss << "Коорд. света: (" << light.pos.X() << ", " << light.pos.Y() << ", " << light.pos.Z() << ")" << std::endl;
	ss << "Коорд. камеры: (" << camera.pos.X() << ", " << camera.pos.Y() << ", " << camera.pos.Z() << ")" << std::endl;
	ss << "Параметры камеры: R="  << camera.camDist << ", fi1=" << camera.fi1 << ", fi2=" << camera.fi2 << std::endl;
	
	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //восстанавливаем матрицы проекции и модел-вью обратьно из стека.
	glPopMatrix();


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}