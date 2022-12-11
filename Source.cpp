#define _USE_MATH_DEFINES
#define GLEW_STATIC
#define FREEGLUT_STATIC
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <cmath>
#include <iostream>
#include <ctime>
#include <conio.h>
#include <GL/freeglut.h>

//----------------------------------------------------------------------------------------------------------------- JAJKO
#define M_PI 3.14159265358979323846 // LICZBA PI
typedef float point3[3];
int n = 20;								// LICZBA, KTÓRA PO PODNIESIENIU DO KWADRATU DA LICZBĘ WIERZCHOŁKÓW
float z = 8.5;

static GLint status = 0;       // stan klawiszy myszy
							   // 0 - nie naciśnięto żadnego klawisza
							   // 1 - naciśnięty został lewy klawisz
							   // 2 - naciśnięty został prawy klawisz

int model = 6;						// MODELEM DOMYŚLNYM JEST JAJKO
									// 1 - jajko

int temp = 1.0f;				// kierunek wektora

static int x_pos_old = 0;       // poprzednia pozycja kursora myszy względem osi X
static int y_pos_old = 0;       // poprzednia pozycja kursora myszy względem osi Y	

static GLfloat pix2angle;


//----------------------------------------------------------------------------------------------------------------- OBSERWATOR
static GLfloat viewer[] = { 0.0, 0.0, 0.0 }; // inicjalizacja położenia obserwatora
float obs_distance = 15.0f;	// domyślna odległość obserwatora od początku układu współrzędnych

static GLfloat theta_x = 0.0;   // kąt obrotu obiektu
static GLfloat theta_y = 0.0;   // kąt obrotu obiektu

static int delta_x = 0;        // różnica pomiędzy pozycją bieżącą i poprzednią kursora myszy
static int delta_y = 0;


//----------------------------------------------------------------------------------------------------------------- ŚWIATŁO
static GLfloat R = 20.0f;
static GLfloat light_position[] = { 30.0, 30.0, R, 3.0 }; // inicjalizacja położenia światła

static GLfloat theta_x_light = 0.0;   // kąt obrotu światła względem osi Y
static GLfloat theta_y_light = 0.0;   // kąt obrotu światła względem osi X


static int delta_y_light = 0;			// różnica pomiędzy pozycją bieżącą i poprzednią kursora myszy względem osi X dla światła
static int delta_x_light = 0;			// różnica pomiędzy pozycją bieżącą i poprzednią kursora myszy względem osi Y dla światła


GLfloat light_ambient[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };


GLfloat att_constant = { 1.0 };
GLfloat att_linear = { 0.05 };
GLfloat att_quadratic = { 0.001 };


//----------------------------------------------------------------------------------------------------------------- TEKSTURA
GLbyte* pBytes;
GLint ImWidth, ImHeight, ImComponents;
GLenum ImFormat;



//----------------------------------------------------------------------------------------------------------------- TABLICA WIERZCHOŁKÓW
float*** NodesTab(int n)
{
	float*** tab = new float** [n];
	float u, v;
	for (int i = 0; i < n; i++)
	{
		tab[i] = new float* [n];
		for (int j = 0; j < n; j++)
		{
			u = (float)i / (float)n;
			v = (float)j / (float)n;
			tab[i][j] = new float[3];
			tab[i][j][0] = (-90.0 * pow(u, 5.0) + 225.0 * pow(u, 4.0) - 270.0 * pow(u, 3.0) + 180.0 * pow(u, 2.0) - 45.0 * u) * cos(M_PI * v); // x(u,v)
			tab[i][j][1] = 160.0 * pow(u, 4.0) - 320.0 * pow(u, 3.0) + 160.0 * pow(u, 2.0) - 5;												   // y(u,v)
			tab[i][j][2] = (-90.0 * pow(u, 5.0) + 225.0 * pow(u, 4.0) - 270.0 * pow(u, 3.0) + 180.0 * pow(u, 2.0) - 45.0 * u) * sin(M_PI * v); // z(u,v)
		}
	}
	return tab;
}

//----------------------------------------------------------------------------------------------------------------- TABLICA WEKTORÓW NORMALNYCH
float*** VectorsTab(int n)
{
	float*** tab = new float** [n];
	float u, v;
	float xu, xv, yu, yv, zu, zv, v_length, X, Y, Z;
	for (int i = 0; i < n; i++)
	{
		tab[i] = new float* [n];
		for (int j = 0; j < n; j++)
		{
			u = (float)i / (float)n;
			v = (float)j / (float)n;
			xu = (-450.0 * pow(u, 4.0) + 900.0 * pow(u, 3) - 810.0 * pow(u, 2) + 360.0 * u - 45.0) * cos(M_PI * v);
			xv = M_PI * (90.0 * pow(u, 5) - 225.0 * pow(u, 4) + 270.0 * pow(u, 3) - 180.0 * pow(u, 2) + 45.0 * u) * sin(M_PI * v);
			yu = 640.0 * pow(u, 3) - 960.0 * pow(u, 2) + 320.0 * u;
			yv = 0;
			zu = (-450.0 * pow(u, 4.0) + 900.0 * pow(u, 3) - 810.0 * pow(u, 2) + 360.0 * u - 45.0) * sin(M_PI * v);
			zv = -M_PI * (90.0 * pow(u, 5) - 225.0 * pow(u, 4) + 270.0 * pow(u, 3) - 180.0 * pow(u, 2) + 45.0 * u) * cos(M_PI * v);

			X = yu * zv - zu * yv;
			Y = zu * xv - xu * zv;
			Z = xu * yv - yu * xv;

			v_length = sqrt(pow(X, 2) + pow(Y, 2) + pow(Z, 2));
			if (v_length == 0) v_length = 1;

			tab[i][j] = new float[3];

			tab[i][j][0] = X / v_length;
			tab[i][j][1] = Y / v_length;
			tab[i][j][2] = Z / v_length;

			if (i > n / 2)
			{
				tab[i][j][0] *= -1;
				tab[i][j][1] *= -1;
				tab[i][j][2] *= -1;
			}
		}
	}
	return tab;
}


//----------------------------------------------------------------------------------------------------------------- JAJKO KOLOROWE
void ColorTrianglesPerNode(float*** pt, float*** vn, GLint w, GLint k)
{
	glBegin(GL_TRIANGLES);

	glNormal3fv(vn[w][k]);
	glVertex3fv(pt[w][k]);
	glNormal3fv(vn[w - 1][k]);
	glVertex3fv(pt[w - 1][k]);
	glNormal3fv(vn[w - 1][k + 1]);
	glVertex3fv(pt[w - 1][k + 1]);

	glEnd();

	glBegin(GL_TRIANGLES);

	glNormal3fv(vn[w][k]);
	glVertex3fv(pt[w][k]);
	glNormal3fv(vn[w][k + 1]);
	glVertex3fv(pt[w][k + 1]);
	glNormal3fv(vn[w - 1][k + 1]);
	glVertex3fv(pt[w - 1][k + 1]);

	glEnd();
}

void ColorTrianglesLastColumn(float*** pt, float*** vn, GLint w, GLint n)
{
	glBegin(GL_TRIANGLES);

	glNormal3fv(vn[w][n - 1]);
	glVertex3fv(pt[w][n - 1]);
	glNormal3fv(vn[w - 1][n - 1]);
	glVertex3fv(pt[w - 1][n - 1]);
	glNormal3fv(vn[n - w][0]);
	glVertex3fv(pt[n - w][0]);

	glEnd();


	glBegin(GL_TRIANGLES);

	glNormal3fv(vn[w][n - 1]);
	glVertex3fv(pt[w][n - 1]);
	glNormal3fv(vn[n - w - 1][0]);
	glVertex3fv(pt[n - w - 1][0]);
	glNormal3fv(vn[n - w][0]);
	glVertex3fv(pt[n - w][0]);

	glEnd();
}

void ColorTrianglesLastRow(float*** pt, float*** vn, GLint n)
{
	for (int k = 0; k < n - 1; k++)
	{
		glBegin(GL_TRIANGLES);

		glNormal3fv(vn[0][k]);
		glVertex3fv(pt[0][k]);
		glNormal3fv(vn[n - 1][k]);
		glVertex3fv(pt[n - 1][k]);
		glNormal3fv(vn[n - 1][k + 1]);
		glVertex3fv(pt[n - 1][k + 1]);

		glEnd();

		glBegin(GL_TRIANGLES);

		glNormal3fv(vn[0][k]);
		glVertex3fv(pt[0][k]);
		glNormal3fv(vn[0][k + 1]);
		glVertex3fv(pt[0][k + 1]);
		glNormal3fv(vn[n - 1][k + 1]);
		glVertex3fv(pt[n - 1][k + 1]);

		glEnd();
	}
}

void ColorEgg(float*** pt, float*** vn, GLint n)
{
	for (int w = 1; w < n; w++)
	{
		for (int k = 0; k < n - 1; k++)
		{
			ColorTrianglesPerNode(pt, vn, w, k);
		}
		ColorTrianglesLastColumn(pt, vn, w, n);
	}
	ColorTrianglesLastRow(pt, vn, n);
}

//----------------------------------------------------------------------------------------------------------------- JAJKO TEKSTUROWANE
void TextureTrianglesPerNode(float*** pt, float*** vn, GLint w, GLint k)
{
	glBegin(GL_TRIANGLES);

	glNormal3fv(vn[w][k]);
	glTexCoord2fv(vn[w][k]);
	glVertex3fv(pt[w][k]);

	glNormal3fv(vn[w - 1][k]);
	glTexCoord2fv(vn[w - 1][k]);
	glVertex3fv(pt[w - 1][k]);

	glNormal3fv(vn[w - 1][k + 1]);
	glTexCoord2fv(vn[w - 1][k + 1]);
	glVertex3fv(pt[w - 1][k + 1]);

	glEnd();

	glBegin(GL_TRIANGLES);

	glNormal3fv(vn[w][k]);
	glTexCoord2fv(vn[w][k]);
	glVertex3fv(pt[w][k]);

	glNormal3fv(vn[w][k + 1]);
	glTexCoord2fv(vn[w][k + 1]);
	glVertex3fv(pt[w][k + 1]);

	glNormal3fv(vn[w - 1][k + 1]);
	glTexCoord2fv(vn[w - 1][k + 1]);
	glVertex3fv(pt[w - 1][k + 1]);

	glEnd();
}

void TextureTrianglesLastColumn(float*** pt, float*** vn, GLint w, GLint n)
{
	glBegin(GL_TRIANGLES);

	glNormal3fv(vn[w][n - 1]);
	glTexCoord2fv(vn[w][n - 1]);
	glVertex3fv(pt[w][n - 1]);

	glNormal3fv(vn[w - 1][n - 1]);
	glTexCoord2fv(vn[w - 1][n - 1]);
	glVertex3fv(pt[w - 1][n - 1]);

	glNormal3fv(vn[n - w][0]);
	glTexCoord2fv(vn[n - w][0]);
	glVertex3fv(pt[n - w][0]);

	glEnd();


	glBegin(GL_TRIANGLES);

	glNormal3fv(vn[w][n - 1]);
	glTexCoord2fv(vn[w][n - 1]);
	glVertex3fv(pt[w][n - 1]);

	glNormal3fv(vn[n - w - 1][0]);
	glTexCoord2fv(vn[n - w - 1][0]);
	glVertex3fv(pt[n - w - 1][0]);

	glNormal3fv(vn[n - w][0]);
	glTexCoord2fv(vn[n - w][0]);
	glVertex3fv(pt[n - w][0]);

	glEnd();
}

void TextureTrianglesLastRow(float*** pt, float*** vn, GLint n)
{
	for (int k = 0; k < n - 1; k++)
	{
		glBegin(GL_TRIANGLES);

		glNormal3fv(vn[0][k]);
		glTexCoord2fv(vn[0][k]);
		glVertex3fv(pt[0][k]);

		glNormal3fv(vn[n - 1][k]);
		glTexCoord2fv(vn[n - 1][k]);
		glVertex3fv(pt[n - 1][k]);

		glNormal3fv(vn[n - 1][k + 1]);
		glTexCoord2fv(vn[n - 1][k + 1]);
		glVertex3fv(pt[n - 1][k + 1]);

		glEnd();

		glBegin(GL_TRIANGLES);

		glNormal3fv(vn[0][k]);
		glTexCoord2fv(vn[0][k]);
		glVertex3fv(pt[0][k]);

		glNormal3fv(vn[0][k + 1]);
		glTexCoord2fv(vn[0][k + 1]);
		glVertex3fv(pt[0][k + 1]);

		glNormal3fv(vn[n - 1][k + 1]);
		glTexCoord2fv(vn[n - 1][k + 1]);
		glVertex3fv(pt[n - 1][k + 1]);

		glEnd();
	}
}

void TextureEgg(float*** pt, float*** vn, GLint n)
{
	for (int w = 1; w < n; w++)
	{
		for (int k = 0; k < n - 1; k++)
		{
			TextureTrianglesPerNode(pt, vn, w, k);
		}
		TextureTrianglesLastColumn(pt, vn, w, n);
	}
	TextureTrianglesLastRow(pt, vn, n);
}


//----------------------------------------------------------------------------------------------------------------- JAJKO TEKSTUROWANE
void PyramidsBase()
{
	glColor3f(1.0f, 1.0f, 1.0f);

	glBegin(GL_POLYGON);
	
	glVertex3f(-5, 0, -5);
	glVertex3f(-5, 0, 5);
	glVertex3f(5, 0, 5);
	glVertex3f(5, 0, -5);

	glEnd();
}

void PyramidsBackWall()
{
	glColor3f(1.0f, 1.0f, 1.0f);

	glBegin(GL_TRIANGLES);

	glVertex3f(-5, 0, -5);
	glVertex3f(-5, 0, 5);
	glVertex3f(0, z, 0);

	glEnd();
}

void PyramidsFrontWall()
{
	glColor3f(1.0f, 1.0f, 1.0f);

	glBegin(GL_TRIANGLES);

	glVertex3f(5, 0, 5);
	glVertex3f(5, 0, -5);
	glVertex3f(0, 1, 0);

	glEnd();
}

void PyramidsRightWall()
{
	glColor3f(1.0f, 1.0f, 1.0f);

	glBegin(GL_TRIANGLES);

	glVertex3f(-5, 0, -5);
	glVertex3f(5, 0, -5);
	glVertex3f(0, z, 0);

	glEnd();
}

void PyramidsLeftWall()
{
	glColor3f(1.0f, 1.0f, 1.0f);

	glBegin(GL_TRIANGLES);

	glVertex3f(5, 0, 5);
	glVertex3f(-5, 0, 5);
	glVertex3f(0, z, 0);

	glEnd();
}


//----------------------------------------------------------------------------------------------------------------- RYSOWANIE OBIEKTU
void DrawObject(int n)
{
	float*** nodesTab = NodesTab(n);
	float*** vectorsTab = VectorsTab(n);

	switch (model)
	{
	case 1:
		ColorEgg(nodesTab, vectorsTab, n);
		break;

	case 2:
		PyramidsBase();
		PyramidsBackWall();
		PyramidsFrontWall();
		PyramidsRightWall();
		PyramidsLeftWall();
		break;

	case 3:
		PyramidsBase();
		PyramidsBackWall();
		PyramidsRightWall();
		PyramidsLeftWall();
		break;

	case 4:
		PyramidsBase();
		PyramidsBackWall();
		PyramidsLeftWall();
		break;

	case 5:
		PyramidsBase();
		PyramidsLeftWall();

		break;

	case 6:
		PyramidsBase();
		break;

	case 7: // trójkąt
		glColor3f(1.0f, 1.0f, 1.0f);

		glBegin(GL_TRIANGLES);

		glVertex3f(0, 0, -5);
		glVertex3f(0, 0, 5);
		glVertex3f(0, 10, 0);

		glEnd();

		break;
	
	case 8:
		TextureEgg(nodesTab, vectorsTab, n);
		break;

	default:
		std::cout << "error" << std::endl;
	}

	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			delete nodesTab[i][j];
		}
		delete nodesTab[i];
	}
	delete[] nodesTab;
}


//----------------------------------------------------------------------------------------------------------------- OSIE		  
void Axes(void)
{

	point3  x_min = { -5.0, 0.0, 0.0 };
	point3  x_max = { 5.0, 0.0, 0.0 };
	// pocz?tek i koniec obrazu osi x

	point3  y_min = { 0.0, -5.0, 0.0 };
	point3  y_max = { 0.0,  5.0, 0.0 };
	// pocz?tek i koniec obrazu osi y

	point3  z_min = { 0.0, 0.0, -5.0 };
	point3  z_max = { 0.0, 0.0,  5.0 };
	//  pocz?tek i koniec obrazu osi y
	glColor3f(1.0f, 0.0f, 0.0f);  // kolor rysowania osi - czerwony
	glBegin(GL_LINES); // rysowanie osi x
	glVertex3fv(x_min);
	glVertex3fv(x_max);
	glEnd();

	glColor3f(0.0f, 1.0f, 0.0f);  // kolor rysowania - zielony
	glBegin(GL_LINES);  // rysowanie osi y

	glVertex3fv(y_min);
	glVertex3fv(y_max);
	glEnd();

	glColor3f(0.0f, 0.0f, 1.0f);  // kolor rysowania - niebieski
	glBegin(GL_LINES); // rysowanie osi z

	glVertex3fv(z_min);
	glVertex3fv(z_max);
	glEnd();

}


//----------------------------------------------------------------------------------------------------------------- MYSZ	
void Mouse(int btn, int state, int x, int y)
{
	if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		x_pos_old = x;         // przypisanie aktualnie odczytanej pozycji kursora jako pozycji poprzedniej
		y_pos_old = y;
		status = 1;          // wcięnięty został lewy klawisz myszy
	}
	else if (btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		y_pos_old = y;         // przypisanie aktualnie odczytanej pozycji kursora jako pozycji poprzedniej
		status = 2;          // wcięnięty został prawy klawisz myszy
	}
	else
		status = 0;          // nie został wcięnięty żaden klawisz
}

void Motion(GLsizei x, GLsizei y)
{

	delta_y = x - x_pos_old;     // obliczenie różnicy położenia kursora myszy
	x_pos_old = x;				 // podstawienie bieżącego położenia jako poprzednie

	delta_x = y - y_pos_old;     // obliczenie różnicy położenia kursora myszy
	y_pos_old = y;				 // podstawienie bieżącego położenia jako poprzednie

	glutPostRedisplay();     // przerysowanie obrazu sceny
}


//----------------------------------------------------------------------------------------------------------------- TEKSTURA
GLbyte* LoadTGAImage(const char* FileName, GLint* ImWidth, GLint* ImHeight, GLint* ImComponents, GLenum* ImFormat)
{

	/*************************************************************************************/

	// Struktura dla nagłówka pliku  TGA


#pragma pack(1)           
	typedef struct
	{
		GLbyte    idlength;
		GLbyte    colormaptype;
		GLbyte    datatypecode;
		unsigned short    colormapstart;
		unsigned short    colormaplength;
		unsigned char     colormapdepth;
		unsigned short    x_orgin;
		unsigned short    y_orgin;
		unsigned short    width;
		unsigned short    height;
		GLbyte    bitsperpixel;
		GLbyte    descriptor;
	}TGAHEADER;
#pragma pack(8)

	FILE* pFile;
	TGAHEADER tgaHeader;
	unsigned long lImageSize;
	short sDepth;
	GLbyte* pbitsperpixel = NULL;


	/*************************************************************************************/

	// Wartości domyślne zwracane w przypadku błędu

	*ImWidth = 0;
	*ImHeight = 0;
	*ImFormat = GL_BGR_EXT;
	*ImComponents = GL_RGB8;

	pFile = fopen(FileName, "rb");
	if (pFile == NULL)
		return NULL;

	/*************************************************************************************/
	// Przeczytanie nagłówka pliku 


	fread(&tgaHeader, sizeof(TGAHEADER), 1, pFile);


	/*************************************************************************************/

	// Odczytanie szerokości, wysokości i głębi obrazu

	*ImWidth = tgaHeader.width;
	*ImHeight = tgaHeader.height;
	sDepth = tgaHeader.bitsperpixel / 8;


	/*************************************************************************************/
	// Sprawdzenie, czy głębia spełnia założone warunki (8, 24, lub 32 bity)

	if (tgaHeader.bitsperpixel != 8 && tgaHeader.bitsperpixel != 24 && tgaHeader.bitsperpixel != 32)
		return NULL;

	/*************************************************************************************/

	// Obliczenie rozmiaru bufora w pamięci


	lImageSize = tgaHeader.width * tgaHeader.height * sDepth;


	/*************************************************************************************/

	// Alokacja pamięci dla danych obrazu


	pbitsperpixel = (GLbyte*)malloc(lImageSize * sizeof(GLbyte));

	if (pbitsperpixel == NULL)
		return NULL;

	if (fread(pbitsperpixel, lImageSize, 1, pFile) != 1)
	{
		free(pbitsperpixel);
		return NULL;
	}


	/*************************************************************************************/

	// Ustawienie formatu OpenGL


	switch (sDepth)

	{

	case 3:

		*ImFormat = GL_BGR_EXT;

		*ImComponents = GL_RGB8;

		break;

	case 4:

		*ImFormat = GL_BGRA_EXT;

		*ImComponents = GL_RGBA8;

		break;

	case 1:

		*ImFormat = GL_LUMINANCE;

		*ImComponents = GL_LUMINANCE8;

		break;

	};



	fclose(pFile);



	return pbitsperpixel;

}


//----------------------------------------------------------------------------------------------------------------- RESZTA
void RenderScene(void)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Czyszczenie okna aktualnym kolorem czyszczącym

	glLoadIdentity();

	if (status == 1)                     // jeśli lewy klawisz myszy jest wcięnięty
	{

		theta_y += delta_y * pix2angle / 50.0f;
		theta_x += delta_x * pix2angle / 50.0f;

		if (theta_x > 2 * M_PI)
			theta_x = 0.0f;

		if (theta_x < 0)
			theta_x = 2 * M_PI;

		if (theta_x > M_PI / 2)
			temp = -1.0f;
		else
			temp = 1.0f;

		if (theta_x > M_PI + (M_PI / 2))
			temp = 1.0f;

	}
	if (status == 2)                     // jeśli prawy klawisz myszy jest wcięnięty
	{
		obs_distance += delta_x * pix2angle;
	}

	viewer[0] = obs_distance * cos(theta_y) * cos(theta_x);
	viewer[1] = obs_distance * sin(theta_x);
	viewer[2] = obs_distance * sin(theta_y) * cos(theta_x);
	gluLookAt(viewer[0], viewer[1], viewer[2], 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);


	Axes();

	DrawObject(n);

	glFlush();
	// Przekazanie poleceń rysujących do wykonania
	glutSwapBuffers();
}

void MyInit(void)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// Kolor czyszczący (wypełnienia okna) ustawiono na czarny


	GLfloat mat_ambient[] = { 0.15, 0.15, 0.15, 0.15 };
	GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_shininess = { 20.0 };
	// współczynnik n opisujący połysk powierzchni



	//--------------------------------------------------------------------- ŚWIATŁO
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	//glLightfv(GL_LIGHT1, GL_POSITION, light1_position);

	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, att_constant);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, att_linear);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, att_quadratic);


	// Ustawienie patrametrów materiału
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);

	// Ustawienie opcji systemu oświetlania sceny
	glShadeModel(GL_SMOOTH); // właczenie łagodnego cieniowania
	glEnable(GL_LIGHTING);   // właczenie systemu oświetlenia sceny
	glEnable(GL_LIGHT0);     // włączenie źródła o numerze 1
	glEnable(GL_DEPTH_TEST); // włączenie mechanizmu z-bufora


	//--------------------------------------------------------------------- TEKSTUROWANIE
	glEnable(GL_CULL_FACE);

	//  Przeczytanie obrazu tekstury z pliku o nazwie tekstura.tga
	pBytes = LoadTGAImage("D3_t.tga", &ImWidth, &ImHeight, &ImComponents, &ImFormat);


	// Zdefiniowanie tekstury 2-D
	glTexImage2D(GL_TEXTURE_2D, 0, ImComponents, ImWidth, ImHeight, 0, ImFormat, GL_UNSIGNED_BYTE, pBytes);

	// Zwolnienie pamięci
	free(pBytes);

	// Włączenie mechanizmu teksturowania
	glEnable(GL_TEXTURE_2D);

	// Ustalenie trybu teksturowania
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	// Określenie sposobu nakładania tekstur
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void ChangeSize(GLsizei horizontal, GLsizei vertical)
{
	pix2angle = 360.0 / (float)horizontal;  // przeliczenie pikseli na stopnie

	glMatrixMode(GL_PROJECTION);
	// Przełączenie macierzy bieżącej na macierz projekcji

	glLoadIdentity();
	// Czyszcznie macierzy bieżącej

	gluPerspective(70, 1.0, 1.0, 30.0);
	// Ustawienie parametrów dla rzutu perspektywicznego


	if (horizontal <= vertical)
		glViewport(0, (vertical - horizontal) / 2, horizontal, horizontal);

	else
		glViewport((horizontal - vertical) / 2, 0, vertical, vertical);
	// Ustawienie wielkości okna okna widoku (viewport) w zależności
	// relacji pomiędzy wysokością i szerokością okna

	glMatrixMode(GL_MODELVIEW);
	// Przełączenie macierzy bieżącej na macierz widoku modelu  

	glLoadIdentity();
	// Czyszczenie macierzy bieżącej

}

void Keys(unsigned char key, int x, int y)
{
	if (key == 'j') model = 1;
	if (key == '4') model = 2;
	if (key == '3') model = 3;
	if (key == '2') model = 4;
	if (key == '1') model = 5;
	if (key == '0') model = 6;
	if (key == 't') model = 7;
	if (key == 'x') model = 8;

	RenderScene(); // przerysowanie obrazu sceny
}

//void main(void)
int main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(700, 700);

	glutCreateWindow("Rzutowanie perspektywiczne");


	/*std::cout << "Podaj liczbe n (wierzcholkow bedzie n^2) <13-1000>" << std::endl;
	std::cin >> n;
	std::cout << "p - punktowo" << std::endl;
	std::cout << "s - siatka" << std::endl;
	std::cout << "t - trojkaty" << std::endl;
	std::cout << "w - czajnik" << std::endl;
	std::cout << "\nMyszka przesuwana poziomo + wcisniety lewy klawisz  --> przesuniecie obserwatora w okol osi Y" << std::endl;
	std::cout << "Myszka przesuwana pionowo + wcisniety lewy klawisz  --> przesuniecie obserwatora blizej / dalej srodka ukladu wspolrzednych" << std::endl;
	std::cout << "Myszka przesuwana pionowo + wcisniety prawy klawisz --> przesuniecie obserwatora w okol osi X" << std::endl;*/


	glutDisplayFunc(RenderScene);
	// Określenie, że funkcja RenderScene będzie funkcją zwrotną
	// (callback function).  Będzie ona wywoływana za każdym razem
	// gdy zajdzie potrzeba przerysowania okna

	glutKeyboardFunc(Keys);
	// Funkcja Keys będzie funkcją zwrotną. Będzie ona wywoływana
	// za każdym razem, gdy zostanie zmieniony model jajka:
	// klawisze p, s lub t
	// w --> czajnik

	glutMouseFunc(Mouse);
	// Ustala funkcję zwrotną odpowiedzialną za badanie stanu myszy

	glutMotionFunc(Motion);
	// Ustala funkcję zwrotną odpowiedzialną za badanie ruchu myszy


	glutReshapeFunc(ChangeSize);
	// Dla aktualnego okna ustala funkcję zwrotną odpowiedzialną
	// za zmiany rozmiaru okna                       


	MyInit();
	// Funkcja MyInit() (zdefiniowana powyżej) wykonuje wszelkie
	// inicjalizacje konieczne  przed przystąpieniem do renderowania
	glEnable(GL_DEPTH_TEST);
	// Włączenie mechanizmu usuwania niewidocznych elementów sceny

	glutMainLoop();
	// Funkcja uruchamia szkielet biblioteki GLUT

	return 0;
}