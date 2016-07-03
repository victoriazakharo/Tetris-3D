#include "Game.h"
#include <ctime>

void Game::UpdateCurrentFigure(bool fill)
{
	Point pos = currentFigure.pos;
	auto figure = figureList[currentFigure.type];
	for (auto i = figure.points[currentFigure.rotation].begin(); i != figure.points[currentFigure.rotation].end(); ++i)
		field[i->y + pos.y][i->x + pos.x][-i->z - pos.z] = figure.color * (fill ? 1 : 0);
}

void Game::Rotate()
{
	if (++currentFigure.rotation == figureList[currentFigure.type].points.size())
		currentFigure.rotation = 0;
	if (!CheckCollisions() && --currentFigure.rotation < 0)
		currentFigure.rotation += figureList[currentFigure.type].points.size();
}

bool Game::CheckCollisions()
{
	Point pos = currentFigure.pos;
	for (auto i = figureList[currentFigure.type].points[currentFigure.rotation].begin(); i != figureList[currentFigure.type].points[currentFigure.rotation].end(); ++i)
		if (i->x + pos.x < 0 || i->x + pos.x >= width ||
			i->y + pos.y < 0 || i->y + pos.y >= height ||
			i->z + pos.z > 0 || i->z + pos.z <= -depth ||
			field[i->y + pos.y][i->x + pos.x][-i->z - pos.z] > 0)
			return false;
	return true;
}

void Game::NewFigure()
{
	currentFigure.type = rand() % figureList.size();
	currentFigure.rotation = rand() % figureList[currentFigure.type].points.size();
	currentFigure.pos = Point(rand() % 7);
	if (!CheckCollisions())
	{
		for (int i = 0; i < height; ++i)
			for (int j = 0; j < width; ++j)
				for (int t = 0; t < depth; ++t)
					field[i][j][t] = 0;
		NewFigure();
	}
}

bool Game::ShiftRows()
{
	int x, y, z, lines = 0;
	for (y = height - 1; y >= 0; y--) {
		x = 0; z = 0;
		for (; x < width; ++x)
			for (; z < depth; ++z)
				if (field[y][x][z] == 0) break;

		if (x == width && z == depth)
			lines++;
		else if (lines > 0)
		{
			for (x = 0; x < width; ++x)
				for (z = 0; z < depth; ++z)
					field[y + lines][x][z] = field[y][x][z];
		}
	}
	for (y = 0; y < lines; ++y)
		for (x = 0; x < width; ++x)
			for (z = 0; z < depth; ++z)
				field[y][x][z] = 0;
	return lines;
}

void Game::Down()
{
	++currentFigure.pos.y;
	if (!CheckCollisions())
	{
		--currentFigure.pos.y;
		UpdateCurrentFigure(true);
		ShiftRows();
		NewFigure();
	}
}

void Game::Move(int dx, int dz)
{
	currentFigure.pos.x += dx;
	if (!CheckCollisions())
		currentFigure.pos.x -= dx;
	currentFigure.pos.z += dz;
	if (!CheckCollisions())
		currentFigure.pos.z -= dz;
}

void Game::Initialize()
{
	figureList = FigureData::ReadFigures();
	srand(time(nullptr)); 
	NewFigure();
}

void Game::InitCubeLists() const
{	
	GLfloat normals[6][3] ={{ -1.f, 0.f,  0.f },
							{ 0.f,  1.f,  0.f },
							{ 1.f,  0.f,  0.f },
							{ 0.f,  -1.f, 0.f },
							{ 0.f,  0.f,  -1.f },
							{ 0.f,  0.f, 1.f } };
	GLint faces[6][4] ={{ 3, 2, 6, 7 },
						{ 4, 5, 1, 0 },
						{ 7, 4, 0, 3 },
						{ 5, 6, 2, 1 },
						{ 0, 1, 2, 3 },
						{ 7, 6, 5, 4 } };
	GLfloat v[8][3] = { { -side, -side, side },
						{ -side, -side, -side },
						{ -side,  side, -side },
						{ -side,  side, side },
						{ side, -side, side },
						{ side, -side, -side },
						{ side, side, -side },
						{ side, side,  side } };	
	glNewList(cube, GL_COMPILE);
	glBegin(GL_QUADS);
	for (int i = 0; i < 6; i++) {
		glNormal3fv(&normals[i][0]);
		for (int j = 0; j < 4; j++)
			glVertex3fv(&v[faces[i][j]][0]);
	}	
	glEnd();
	glColor3f(0.f, 0.f, 0.f);
	glLineWidth(1.5f);
	for (int i = 0; i < 6; i++) {
		glBegin(GL_LINE_LOOP);
		for (int j = 0; j < 4; j++)
			glVertex3fv(&v[faces[i][j]][0]);
		glEnd();
	}
	glEndList();
}

void Game::InitBorderLists() const
{
	GLint margin = 2;
	GLint left = -side * (width + margin);
	GLint top = side * (height + margin);
	GLint bottom = -side * height;
	GLint right = side * width;
	GLint z = -(depth * margin - margin) * side;
	glNewList(borders, GL_COMPILE);
	glLineWidth(3.f);	
	glBegin(GL_LINE_LOOP);
	glVertex3f(left, bottom, 0.f);
	glVertex3f(left, top, 0.f);
	glVertex3f(right, top, 0.f);
	glVertex3f(right, bottom, 0.f);
	glEnd();
	glBegin(GL_LINE_LOOP);
	glVertex3f(left, bottom, z);
	glVertex3f(left, top, z);
	glVertex3f(right, top, z);
	glVertex3f(right, bottom, z);
	glEnd();
	glEndList();
}

void Game::InitGL()
{	
	glShadeModel(GL_SMOOTH);                               
	glClearColor(0.f, 0.f, 0.f, 0.f);                 
	glClearDepth(1.0f);                                    
	glEnable(GL_DEPTH_TEST);                               
	glDepthFunc(GL_LEQUAL);                               

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);    
	glColor3f(1.f, 1.f, 1.f);

	cube = glGenLists(2);
	InitCubeLists();
	borders = cube + 1;
	InitBorderLists();	
}

GLvoid Game::ResizeScene(GLsizei width, GLsizei height)
{
	GLfloat nRange = 100.f;
	if (height == 0)
		height = 1;
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	GLfloat aspectRatio = (GLfloat)width / (GLfloat)height;
	if (width <= height)
		glOrtho(-nRange, nRange, -nRange / aspectRatio, nRange / aspectRatio, 0, nRange*3.f);
	else
		glOrtho(-nRange * aspectRatio, nRange * aspectRatio, -nRange, nRange, 0, nRange*3.f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void Game::Draw()
{
	UpdateCurrentFigure(true);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);     
	glLoadIdentity();                                       

	glTranslatef(0.f, 0.f, -110.f);
	glPushMatrix();

	glRotatef(xRot, 1.f, 0.f, 0.f);
	glRotatef(yRot, 0.f, 1.f, 0.f);
	glRotatef(zRot, 0.f, 0.f, 1.f);

	GLfloat white[] = { 1.f, 1.f, 1.f, 1.f };
	GLfloat black[] = { 0.f, 0.f, 0.f, 1.f };
	glColor3fv(white);
	glCallList(borders);

	glTranslatef(-width * side, height * side, 0.f);

	float dx = 2 * side;	
	for (int z = 0; z < depth; z++) {
		for (int i = 0; i < height; ++i) {
			glPushMatrix();
			for (int j = 0; j < width; ++j) {
				if (field[i][j][z])
				{
					glColor3fv(bcolor[field[i][j][z]]);
					glCallList(cube);					
					glColor3fv(black);
					glCallList(skeleton);
				}
				glTranslatef(dx, 0.f, 0.f);
			}
			glPopMatrix();
			glTranslatef(0.f, -dx, 0.f);
		}
		glTranslatef(0.f, height * dx, -dx);
	}
	glPopMatrix();

	UpdateCurrentFigure(false);
}

void Game::ProcessInput(WPARAM wParam)
{
	switch (wParam)
	{
	case VK_SHIFT: Move(0, 1); break;
	case VK_CONTROL: Move(0, -1); break;
	case VK_LEFT: Move(-1, 0); break;
	case VK_RIGHT: Move(1, 0); break;
	case VK_UP: Rotate(); break;
	case VK_DOWN: Down(); break;
	}
}

void Game::Update()
{
	Down();
}

void Game::Rotate(int x, int y, int z)
{
	xRot += x;
	yRot += y;
	zRot += z;
}

void Game::ResetRotation()
{
	xRot = yRot = zRot = 0.f;
}

Game::Game()
{
	ResetRotation();
}
