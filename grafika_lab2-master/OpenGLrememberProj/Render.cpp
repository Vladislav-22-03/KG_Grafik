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

//����� ��� ��������� ������
class CustomCamera : public Camera
{
public:
	//��������� ������
	double camDist;
	//���� �������� ������
	double fi1, fi2;

	
	//������� ������ �� ���������
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}

	
	//������� ������� ������, ������ �� ����� ��������, ���������� �������
	void SetUpCamera()
	{
		//�������� �� ������� ������ ������
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
		//������� ��������� ������
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //������� ������ ������


//����� ��� ��������� �����
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//��������� ������� �����
		pos = Vector3(1, 1, 3);
	}

	
	//������ ����� � ����� ��� ���������� �����, ���������� �������
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
			//����� �� ��������� ����� �� ����������
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//������ ���������
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

		// ��������� ��������� �����
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// �������������� ����������� �����
		// ������� ��������� (���������� ����)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// ��������� ������������ �����
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// ��������� ���������� ������������ �����
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //������� �������� �����




//������ ���������� ����
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL *ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//������ ���� ������ ��� ������� ����� ������ ����
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01*dx;
		camera.fi2 += -0.01*dy;
	}

	
	//������� ���� �� ���������, � ����� ��� ����
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
//����������� ����� ������ ��������
void initRender(OpenGL *ogl)
{
	//��������� �������

	//4 ����� �� �������� �������
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//��������� ������ ��������� �������
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//�������� ��������
	glEnable(GL_TEXTURE_2D);
	

	//������ ����������� ���������  (R G B)
	{
		RGBTRIPLE* texarray;

		//������ ��������, (������*������*4      4, ���������   ����, �� ������� ������������ �� 4 ����� �� ������� �������� - R G B A)
		char* texCharArray;
		int texW, texH;
		OpenGL::LoadBMP("texture.bmp", &texW, &texH, &texarray);
		OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);



		//���������� �� ��� ��������
		glGenTextures(1, &texId);
		//������ ��������, ��� ��� ����� ����������� � ���������, ����� ����������� �� ����� ��
		glBindTexture(GL_TEXTURE_2D, texId);

		//��������� �������� � �����������, � ���������� ��� ������  ��� �� �����
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

		//�������� ������
		free(texCharArray);
		free(texarray);

		//������� ����
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}

	{
		RGBTRIPLE* texarray6;

		//������ ��������, (������*������*4      4, ���������   ����, �� ������� ������������ �� 4 ����� �� ������� �������� - R G B A)
		char* texCharArray6;
		int texW6, texH6;
		OpenGL::LoadBMP("texture5.bmp", &texW6, &texH6, &texarray6);
		OpenGL::RGBtoChar(texarray6, texW6, texH6, &texCharArray6);



		//���������� �� ��� ��������
		glGenTextures(1, &tex6Id);
		//������ ��������, ��� ��� ����� ����������� � ���������, ����� ����������� �� ����� ��
		glBindTexture(GL_TEXTURE_2D, tex6Id);

		//��������� �������� � �����������, � ���������� ��� ������  ��� �� �����
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW6, texH6, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray6);

		//�������� ������
		free(texCharArray6);
		free(texarray6);

		//������� ����
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}

	{
		RGBTRIPLE* texarray5;

		//������ ��������, (������*������*4      4, ���������   ����, �� ������� ������������ �� 4 ����� �� ������� �������� - R G B A)
		char* texCharArray5;
		int texW5, texH5;
		OpenGL::LoadBMP("texture4.bmp", &texW5, &texH5, &texarray5);
		OpenGL::RGBtoChar(texarray5, texW5, texH5, &texCharArray5);



		//���������� �� ��� ��������
		glGenTextures(1, &tex5Id);
		//������ ��������, ��� ��� ����� ����������� � ���������, ����� ����������� �� ����� ��
		glBindTexture(GL_TEXTURE_2D, tex5Id);

		//��������� �������� � �����������, � ���������� ��� ������  ��� �� �����
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW5, texH5, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray5);

		//�������� ������
		free(texCharArray5);
		free(texarray5);

		//������� ����
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}

	{
		RGBTRIPLE* texarray4;

		//������ ��������, (������*������*4      4, ���������   ����, �� ������� ������������ �� 4 ����� �� ������� �������� - R G B A)
		char* texCharArray4;
		int texW4, texH4;
		OpenGL::LoadBMP("texture3.bmp", &texW4, &texH4, &texarray4);
		OpenGL::RGBtoChar(texarray4, texW4, texH4, &texCharArray4);



		//���������� �� ��� ��������
		glGenTextures(1, &tex4Id);
		//������ ��������, ��� ��� ����� ����������� � ���������, ����� ����������� �� ����� ��
		glBindTexture(GL_TEXTURE_2D, tex4Id);

		//��������� �������� � �����������, � ���������� ��� ������  ��� �� �����
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW4, texH4, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray4);

		//�������� ������
		free(texCharArray4);
		free(texarray4);

		//������� ����
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}

	{
		RGBTRIPLE* texarray3;

		//������ ��������, (������*������*4      4, ���������   ����, �� ������� ������������ �� 4 ����� �� ������� �������� - R G B A)
		char* texCharArray3;
		int texW3, texH3;
		OpenGL::LoadBMP("texture2.bmp", &texW3, &texH3, &texarray3);
		OpenGL::RGBtoChar(texarray3, texW3, texH3, &texCharArray3);



		//���������� �� ��� ��������
		glGenTextures(1, &tex3Id);
		//������ ��������, ��� ��� ����� ����������� � ���������, ����� ����������� �� ����� ��
		glBindTexture(GL_TEXTURE_2D, tex3Id);

		//��������� �������� � �����������, � ���������� ��� ������  ��� �� �����
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW3, texH3, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray3);

		//�������� ������
		free(texCharArray3);
		free(texarray3);

		//������� ����
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}

	{
		RGBTRIPLE* texarray2;

		//������ ��������, (������*������*4      4, ���������   ����, �� ������� ������������ �� 4 ����� �� ������� �������� - R G B A)
		char* texCharArray2;
		int texW2, texH2;
		OpenGL::LoadBMP("texture1.bmp", &texW2, &texH2, &texarray2);
		OpenGL::RGBtoChar(texarray2, texW2, texH2, &texCharArray2);



		//���������� �� ��� ��������
		glGenTextures(1, &tex2Id);
		//������ ��������, ��� ��� ����� ����������� � ���������, ����� ����������� �� ����� ��
		glBindTexture(GL_TEXTURE_2D, tex2Id);

		//��������� �������� � �����������, � ���������� ��� ������  ��� �� �����
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW2, texH2, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray2);

		//�������� ������
		free(texCharArray2);
		free(texarray2);

		//������� ����
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}
	//������ � ���� ����������� � "������"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// ������������ �������� : �� ����� ����� ����� 1
	glEnable(GL_NORMALIZE);

	// ���������� ������������� ��� �����
	glEnable(GL_LINE_SMOOTH); 


	//   ������ ��������� ���������
	//  �������� GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  ������� � ���������� �������� ���������(�� ���������), 
	//                1 - ������� � ���������� �������������� ������� ��������       
	//                �������������� ������� � ���������� ��������� ����������.    
	//  �������� GL_LIGHT_MODEL_AMBIENT - ������ ������� ���������, 
	//                �� ��������� �� ���������
	// �� ��������� (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	camera.fi1 = -1.3;
	camera.fi2 = 0.8;
}


// ������� ��� ��������� ��������
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

	// ������� � ������ ������ ��������
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

// ������� ��� ��������� ������
void drawCone(float x, float y, float baseRadius, float height, int segments) {
	glBindTexture(GL_TEXTURE_2D, tex3Id);
	// ������ ��������� ������
	glBegin(GL_TRIANGLE_FAN);
	glTexCoord2f(0.5f, 0.5f);
	glVertex3f(x, y, 0.0f); // ����������� ����� ���������
	for (int i = 0; i <= segments; ++i) {
		float theta = i * 2.0f * M_PI / segments;
		float xPos = x + baseRadius * cosf(theta);
		float yPos = y + baseRadius * sinf(theta);
		glTexCoord2f((xPos - x) / (2 * baseRadius) + 0.5f, (yPos - y) / (2 * baseRadius) + 0.5f);
		glVertex3f(xPos, yPos, 0.0f);
	}
	glEnd();

	// ������ ������� ����������� ������
	glBegin(GL_TRIANGLE_FAN);
	glTexCoord2f(0.5f, 0.0f); // ������� ������
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

// ������� ��� ��������� ����� ����
void drawMageTower() {
	float x = 0.0f;
	float y = 5.0f;
	float height = 12.0f;

	// �������� ����� (�������)
	glColor3f(0.5f, 0.5f, 0.5f); // ����� ����
	drawCylinder(x, y, 2.0f, height, 32);

	// ����� ����� (�����)
	glColor3f(0.3f, 0.3f, 0.3f); // �����-����� ����
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


	//��������������
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	//��������� ���������
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;


	//�������
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//��������
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//����������
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); \
		//������ �����
		glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//���� ���� �������, ��� ����������� (����������� ���������)
	glShadeModel(GL_SMOOTH);
	//===================================
	//������� ���  

	//�����
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

	//������
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

	//�����
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

	//�����
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

	//�����
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

		//������������
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

	//�����(���������)
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

	// ������
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


	//����������
	
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
	
	//��������
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

   //��������� ������ ������

	
	glMatrixMode(GL_PROJECTION);	//������ �������� ������� ��������. 
	                                //(���� ��������� ��������, ����� �� ������������.)
	glPushMatrix();   //��������� ������� ������� ������������� (������� ��������� ������������� ��������) � ���� 				    
	glLoadIdentity();	  //��������� ��������� �������
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);	 //������� ����� ������������� ��������

	glMatrixMode(GL_MODELVIEW);		//������������� �� �����-��� �������
	glPushMatrix();			  //��������� ������� ������� � ���� (��������� ������, ����������)
	glLoadIdentity();		  //���������� �� � ������

	glDisable(GL_LIGHTING);



	GuiTextRectangle rec;		   //������� ����� ��������� ��� ������� ������ � �������� ������.
	rec.setSize(300, 150);
	rec.setPosition(10, ogl->getHeight() - 150 - 10);


	std::stringstream ss;
	ss << "T - ���/���� �������" << std::endl;
	ss << "L - ���/���� ���������" << std::endl;
	ss << "F - ���� �� ������" << std::endl;
	ss << "G - ������� ���� �� �����������" << std::endl;
	ss << "G+��� ������� ���� �� ���������" << std::endl;
	ss << "�����. �����: (" << light.pos.X() << ", " << light.pos.Y() << ", " << light.pos.Z() << ")" << std::endl;
	ss << "�����. ������: (" << camera.pos.X() << ", " << camera.pos.Y() << ", " << camera.pos.Z() << ")" << std::endl;
	ss << "��������� ������: R="  << camera.camDist << ", fi1=" << camera.fi1 << ", fi2=" << camera.fi2 << std::endl;
	
	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //��������������� ������� �������� � �����-��� �������� �� �����.
	glPopMatrix();


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}