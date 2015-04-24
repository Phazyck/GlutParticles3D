#ifndef PARTICLES
#define PARTICLES

struct triangle {
	int i1;
	int i2;
	int i3;
};

struct vertex {
	double x;
	double y;
	double z;
};

// Performance posibility: Have vertices in world space instead of local,
// and disregard position entirely when drawing, saving calculations.
struct Triparticle {
	vertex pos;
	vertex v1, v2, v3;
	// Maybe have rotation and angular velocity?  
	vertex velocity;
	int lifetime;
};

#endif