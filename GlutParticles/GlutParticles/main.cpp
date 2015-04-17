#include <vector>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <limits>
#include <string>
#include <GL/glut.h>

using namespace std;

#define LINE_SIZE (256)
#define FRAME_MSEC (17)
#define EMIT_FRAME_DELAY (2)
#define EMIT_AMOUNT (5)
#define MIN_TRI_SIZE (0.3)
#define MAX_TRI_SIZE (0.8)

struct vertex {
	double x;
	double y;
	double z;
};
struct triangle {
	int i1;
	int i2;
	int i3;
};

const vertex MIN_START{
	-3.0,
	2.0,
	0.0
};

const vertex MAX_START{
	2.0,
	2.0,
	0.0
};

// Performance posibility: Have vertices in world space instead of local,
// and disregard position entirely when drawing, saving calculations.
class Triparticle {
public:
	vertex pos;
	vertex v1, v2, v3;
	// Maybe have rotation and angular velocity?  
	vertex velocity;
};

vector<Triparticle*> triparticles;

double randFloatRange(double M, double N)
{
	return M + (rand() / (RAND_MAX / (N - M)));
}


// Returns random particle, xyz within same range
vertex make_random_vertex(double min, double max) {
	return{
		randFloatRange(min, max),
		randFloatRange(min, max),
		randFloatRange(min, max)
	};
}

// Returns random particle, xyz seperate ranges
vertex make_random_vertex(double xMin, double xMax,
	double yMin, double yMax,
	double zMin, double zMax) {
	return{
		randFloatRange(xMin, xMax),
		randFloatRange(yMin, yMax),
		randFloatRange(zMin, zMax)
	};
}


Triparticle* make_random_triparticle(double minSize,
	double maxSize,
	vertex minPos,
	vertex maxPos) {

	Triparticle* t = new Triparticle();

	// Random position between min and max
	t->pos = make_random_vertex(
		minPos.x, maxPos.x,
		minPos.y, maxPos.y,
		minPos.z, maxPos.z);

	t->v1 = make_random_vertex(minSize, maxSize);
	t->v2 = make_random_vertex(minSize, maxSize);
	t->v3 = make_random_vertex(minSize, maxSize);

	// Speed is not random yet, just some downwards direction for now 

	t->velocity = {
		0.0,
		randFloatRange(-0.03, -0.001),
		0.0
	};

	return t;
}

void move_triparticle(Triparticle* t) {
	vertex newPos = t->pos;

	newPos.x += t->velocity.x;
	newPos.y += t->velocity.y;
	newPos.z += t->velocity.z;

	t->pos = newPos;
}

void draw_triparticle(Triparticle* t) {

	// tri verts in world space
	vertex v1, v2, v3;
	// edges
	vertex e1, e2;
	// normal
	vertex n;

	// find world coords for verts
	v1.x = t->pos.x + t->v1.x;
	v1.y = t->pos.y + t->v1.y;
	v1.z = t->pos.z + t->v1.z;

	v2.x = t->pos.x + t->v2.x;
	v2.y = t->pos.y + t->v2.y;
	v2.z = t->pos.z + t->v2.z;

	v3.x = t->pos.x + t->v3.x;
	v3.y = t->pos.y + t->v3.y;
	v3.z = t->pos.z + t->v3.z;


	// e1 is edge from v1 to v2.
	e1.x = v2.x - v1.x;
	e1.y = v2.y - v1.y;
	e1.z = v2.z - v1.z;

	// e2 is edge from v1 to v3.
	e2.x = v3.x - v1.x;
	e2.y = v3.y - v1.y;
	e2.z = v3.z - v1.z;

	// normal is e1 x e2.  (Note: Does not need to be unit length for glNormal.)
	n.x = (e1.y * e2.z) - (e1.z * e2.y);
	n.y = (e1.z * e2.x) - (e1.x * e2.z);
	n.z = (e1.x * e2.y) - (e1.y * e2.x);

	// Draw this triangle.
	glBegin(GL_TRIANGLES);
	glNormal3f(n.x, n.y, n.z);
	glVertex3f(v1.x, v1.y, v1.z);
	glVertex3f(v2.x, v2.y, v2.z);
	glVertex3f(v3.x, v3.y, v3.z);
	glEnd();
}



// Function runs at set interval, (a onFrame function, of sorts)
void onFrame(int value) {
	// emitter stuff
	static int count = 0;
	count++;
	if (count % EMIT_FRAME_DELAY == 0) {
		for (int i = 0; i < EMIT_AMOUNT; i++) {
			Triparticle* t = make_random_triparticle(MIN_TRI_SIZE, MAX_TRI_SIZE, MIN_START, MAX_START);
			triparticles.push_back(t);
		}
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (int i = 0; i < triparticles.size(); ++i) {
		Triparticle* t = triparticles.at(i);

		move_triparticle(t);
		draw_triparticle(t);
	}

	glutSwapBuffers();

	//glRotatef(3, 0.0, 1.0, 0.0);

	// Call this func again after delay
	glutTimerFunc(FRAME_MSEC, onFrame, 0);
}

void init_scene(void)
{
	// Configure a light.
	GLfloat light_ambient[] = { 0.5, 0.5, 0.5, 1.0 }; // ambient light
	GLfloat light_diffuse[] = { 1.0, 0.0, 0.8, 1.0 };  // diffuse light.
	GLfloat light_position[] = { 0.0, 1.0, -2.0, 0.0 };  // Infinite light location.
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);

	// Use depth buffering for hidden surface elimination.
	glEnable(GL_DEPTH_TEST);

	// Configure viewing.  (Assumes object is within [-1,1] in all axes.)
	glMatrixMode(GL_PROJECTION);
	gluPerspective(40.0, 1.0, 1.0, 10.0);
	glMatrixMode(GL_MODELVIEW);
	gluLookAt(0.0, 0.0, 5.0,  // Set eye position, target position, and up direction.
		0.0, 0.0, 0.0,
		0.0, 1.0, 0.);
}

int main(int argc, char **argv)
{
	// Check for proper arguments.
	/*if (argc < 2) {
	cout << "usage: " << argv[0] << " <obj_filename>" << endl;
	exit(0);
	}*/


	// Set up glut.
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutCreateWindow("Triangle Particles");
	glutTimerFunc(FRAME_MSEC, onFrame, 0);

	// Initialize scene.
	init_scene();

	// Hand control over to glut's main loop.
	glutMainLoop();
}
