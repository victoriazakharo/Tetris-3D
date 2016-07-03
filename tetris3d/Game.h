#pragma once
#include <Windows.h>
#include <GL/gl.h>
#include "Figure.h"

class Game
{
public:
	Game();
	void Initialize();
	void InitGL();
	static GLvoid ResizeScene(GLsizei width, GLsizei height);
	void Draw();
	void ProcessInput(WPARAM wParam);
	void Update();
	void Rotate(int x, int y, int z);
	void ResetRotation();
private:
	GLfloat xRot, yRot, zRot;
	GLuint cube, borders, skeleton;
	GLfloat side = 4.5f;
	GLint width = 10, height = 16, depth = 10;
	GLfloat bcolor[7][4] = {1.f, 1.f, 1.f, 1.f,
							1.f, 0.f, 0.f, 1.f,
							1.f, 0.f, 1.f, 1.f,
							1.f, 1.f, 0.f, 1.f,
							0.f, 1.f, 1.f, 1.f,
							0.f, 0.f, 1.f, 1.f,
							0.f, 1.f, 0.f, 1.f };
										  
	int field[16][10][10] = {};
	std::vector<FigureData> figureList;
	Figure currentFigure;

	void UpdateCurrentFigure(bool fill);
	void Rotate();
	bool CheckCollisions();
	void NewFigure();
	bool ShiftRows();
	void Down();
	void Move(int dx, int dz);
	void InitCubeLists() const;
	void InitBorderLists() const;
};