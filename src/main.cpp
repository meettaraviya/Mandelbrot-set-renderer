#include <stdio.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <math.h>

const double ZOOM_FACTOR = 1.1;
const int SCREEN_WIDTH = 1920, SCREEN_HEIGHT = 1080;
const double INIT_SCALE = 0.01;
const double COLOR_PALETTE[6][3] = {
	{1., 1., 0.},
	{1., 0., 0.},
	{1., 0., 1.},
	{0., 0., 1.},
	{0., 1., 1.},
	{0., 1., 0.},
};

int max_iter = 128;
// double scale = INIT_SCALE;
// double o_x = -scale*SCREEN_WIDTH/2, o_y = -scale*SCREEN_HEIGHT/2;
double scale=1.247200e-04, o_x=1.677900e-01, o_y=-6.519759e-01;
// GLubyte bitmap[SCREEN_WIDTH * SCREEN_HEIGHT / 8];
float bitmap[SCREEN_HEIGHT][SCREEN_WIDTH][3];

void init(void)
{
	glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
	glClearColor (0.0, 0.0, 0.0, 0.0);
}

// void paint(double c_x, double c_y, GLubyte &retval, int m){
// 	bool escaped = false;
// 	int i = 0;
// 	double x2 = 0., y2 = 0., x = 0., y=0.;
// 	while(!escaped and i<max_iter){
// 		escaped = escaped or (x2 + y2 > 4);
// 		i++;
// 		y = (x+x)*y + c_y;
// 		x = x2 - y2 + c_x;
// 		x2 = x*x;
// 		y2 = y*y;
// 	}
// 	if(!escaped){
// 		retval |= (1 << (7 - m));
// 	}
// }

void paint(double c_x, double c_y, float &r, float &g, float &b){
	bool escaped = false;
	int i = 0;
	double x2 = 0., y2 = 0., x = 0., y=0.;
	while(!escaped and i<max_iter){
		escaped = escaped or (x2 + y2 > 4);
		i++;
		y = (x+x)*y + c_y;
		x = x2 - y2 + c_x;
		x2 = x*x;
		y2 = y*y;
	}
	if(!escaped){
		r = 0.; g = 0.; b = 0.;
	}
	else{
		float i_cont = log(log(x2 + y2) / 2 / log(2)) / log(2);
		// float v = ((i < max_iter ? ((float) i)+1-nu : ((float) i)) / max_iter) * 0.9 + (((float) x2+y2) / 10) * 0.1;
		// r = v; g = v; b = 0.;
		i_cont = (i < max_iter ? ((double) i)+1-i_cont : ((double) i));
		int c_i = ((int) floor(i_cont)) % 6;
		r = COLOR_PALETTE[(c_i+1)%6][0] * fmod(i_cont, 1) + COLOR_PALETTE[c_i][0] * (1. - fmod(i_cont, 1));
		g = COLOR_PALETTE[(c_i+1)%6][1] * fmod(i_cont, 1) + COLOR_PALETTE[c_i][1] * (1. - fmod(i_cont, 1));
		b = COLOR_PALETTE[(c_i+1)%6][2] * fmod(i_cont, 1) + COLOR_PALETTE[c_i][2] * (1. - fmod(i_cont, 1));
	}
}

void display(void)
{
	printf("scale=%.6e, o_x=%.6e, o_y=%.6e\n", scale, o_x, o_y);
	for(int y=0; y<SCREEN_HEIGHT; y++){
		for(int x=0; x<SCREEN_WIDTH; x++){
				// int ix = (SCREEN_HEIGHT - 1 - y)*SCREEN_WIDTH + x;
				// if(ix%8==0) bitmap[ix/8] = 0x00;
				// paint(scale*x+origin_x, scale*y+origin_y, bitmap[ix/8], ix%8);
				paint(scale*x+o_x, scale*y+o_y, bitmap[SCREEN_HEIGHT - 1 - y][x][0], bitmap[SCREEN_HEIGHT - 1 - y][x][1], bitmap[SCREEN_HEIGHT - 1 - y][x][2]);
		}
	}
	glClear(GL_COLOR_BUFFER_BIT);
	// glRasterPos2i (0, 0);
   // glBitmap(SCREEN_WIDTH, SCREEN_HEIGHT, 0.0, 0.0, 0.0, 0.0, bitmap);
	// glFlush();
	glDrawPixels(SCREEN_WIDTH, SCREEN_HEIGHT, GL_RGB, GL_FLOAT, bitmap);
   glutSwapBuffers();
}

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho (0, w, 0, h, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
		case 'r':
			scale = INIT_SCALE;
			o_x = -scale*SCREEN_WIDTH/2;
			o_y = -scale*SCREEN_HEIGHT/2;
			display();
			break;
		case '=':
			max_iter *= 2;
			display();
			break;
		case '-':
			if(max_iter>0){
				max_iter /= 2;
				display();
			}
			break;
		case 27:
			exit(0);
	}
}

void mouse(int button, int state, int x, int y)
{
	// Wheel reports as button 3(scroll up) and button 4(scroll down)
	if (((button == 3) || (button == 4)) && state == GLUT_DOWN) // It's a wheel event
	{
		double multiplier = button == 3 ? 1./ZOOM_FACTOR : ZOOM_FACTOR;
		o_x += (1. - multiplier)*x*scale;
		o_y += (1. - multiplier)*y*scale;
		scale *= multiplier;
		display();
	}
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	// glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Mandelbrot fractal");
	glutFullScreen();
	init();
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutDisplayFunc(display);
	glutMainLoop();
	return 0;
}