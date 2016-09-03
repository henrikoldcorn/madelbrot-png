#include <cmath>
#include <string>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "lodepng.h"
using namespace std;
using namespace lodepng;

struct ColorRGB {
	int r;
	int g;
	int b;
};
struct ColorHSV {
	int h;
	int s;
	int v;
};
ColorRGB HSVtoRGB(ColorHSV colorHSV) {
		float r, g, b, h, s, v; //this function works with floats between 0 and 1
		h = colorHSV.h / 256.0;
		s = colorHSV.s / 256.0;
		v = colorHSV.v / 256.0;
	//If saturation is 0, the color is a shade of gray
		if(s == 0) r = g = b = v;
	//If saturation > 0, more complex calculations are needed
		else {
				float f, p, q, t;
				int i;
				h *= 6; //to bring hue to a number between 0 and 6, better for the calculations
				i = int(floor(h));	//e.g. 2.7 becomes 2 and 3.01 becomes 3 or 4.9999 becomes 4
				f = h - i;	//the fractional part of h
				p = v * (1 - s);
				q = v * (1 - (s * f));
				t = v * (1 - (s * (1 - f)));
				switch(i) {
						case 0: r = v; g = t; b = p; break;
						case 1: r = q; g = v; b = p; break;
						case 2: r = p; g = v; b = t; break;
						case 3: r = p; g = q; b = v; break;
						case 4: r = t; g = p; b = v; break;
						case 5: r = v; g = p; b = q; break;
				}
		}
	ColorRGB colorRGB;
		colorRGB.r = int(r * 255.0);
		colorRGB.g = int(g * 255.0);
		colorRGB.b = int(b * 255.0);
		return colorRGB;
}
void encodeOneStep(const char* filename, std::vector<unsigned char>& image, unsigned width, unsigned height) {
	//Encode the image
	unsigned error = lodepng::encode(filename, image, width, height);
	//if there's an error, display it
	if(error) std::cout << "encoder error " << error << ": "<< lodepng_error_text(error) << std::endl;
}
void encodeTwoSteps(const char* filename, std::vector<unsigned char>& image, unsigned width, unsigned height) {
	std::vector<unsigned char> png;
	unsigned error = lodepng::encode(png, image, width, height);
	if(!error) lodepng::save_file(png, filename);
	if(error) std::cout << "encoder error " << error << ": "<< lodepng_error_text(error) << std::endl;
}

void encodeWithState(const char* filename, std::vector<unsigned char>& image, unsigned width, unsigned height) {
	std::vector<unsigned char> png;
	lodepng::State state; //optionally customize this one
	unsigned error = lodepng::encode(png, image, width, height, state);
	if(!error) lodepng::save_file(png, filename);
	if(error) std::cout << "encoder error " << error << ": "<< lodepng_error_text(error) << std::endl;
}


void usage(char *argv[]) {
	printf("Usage: %s [Mandelbrot mode] [width] [height] [iterations] [outfilepath] [colour offset]\n", argv[0]);
	printf("Mandelbrot mode specifies integer exponent, 2-4");
}
/*
void iterate(double *oldRe, double *oldIm) {
	
}
*/
int main(int argc, char *argv[]) {
	if (argc != 7) {
		usage(argv);
		exit(1);
	}
	
	printf("Calculating...\n");
	int power = atoi(argv[1]);
	unsigned width = atoi(argv[2]);
	int w = atoi(argv[2]);
	unsigned height = atoi(argv[3]);
	int h = atoi(argv[3]);
	int maxIterations = atoi(argv[4]);
	const char* filename = argv[5];
	int colourOffset = atoi(argv[6]);
	
	//Me - makes picture proper shape
	float stretcher = w / h;
	//screen(w, h, 0, "Mandelbrot");
	//each iteration, it calculates: new = old*old + c, where c is a constant and old starts at current pixel
	double pr, pi;					 //real and imaginary part of the pixel p
	double newRe, newIm, oldRe, oldIm;	 //real and imaginary parts of new and old
	double zoom = 1, moveX = -0.5, moveY = 0; //you can change these to zoom and change position
	ColorRGB color; //the RGB color value for the pixel
	
	std::vector<unsigned char> image;
	image.resize(w * h * 4);
	//draw the fractal
	int xShifter = 0;
	if (power == 3) xShifter = 3/8 * w;	//cubed has to be move left
	
	for(int y = 0; y < h; y++)
	for(int x = 0; x < w; x++) {
		//calculate the initial real and imaginary part of z, based on the pixel location and zoom and position values
		pr = (x + xShifter - w / 2) / (0.5 * zoom * w) + moveX;
		pi = (1 / stretcher) * (y - h / 2) / (0.5 * zoom * h) + moveY;
		newRe = newIm = oldRe = oldIm = 0; //these should start at 0,0
		//i will represent the number of iterations
		int i;
		//start the iteration process
		for(i = 0; i < maxIterations; i++) {
			//remember value of previous iteration
			oldRe = newRe;
			oldIm = newIm;
			if (power == 2) {
				//squared
				//the actual iteration, the real and imaginary part are calculated
				newRe = oldRe * oldRe - oldIm * oldIm + pr;
				newIm = 2 * oldRe * oldIm + pi;
			}
			else if (power == 3) {
				//cubed
				newRe = oldRe * oldRe * oldRe - 3 * oldRe * oldIm * oldIm + pr;
				newIm = 3 * oldRe * oldRe * oldIm - oldIm * oldIm * oldIm + pi;
			}
			else if (power == 4) {
				newRe = oldRe*oldRe*oldRe*oldRe + oldIm*oldIm*oldIm*oldIm - 
						6*oldRe*oldRe*oldIm*oldIm;
				newIm = 4*oldRe*oldRe*oldRe*oldIm - 4*oldRe*oldIm*oldIm*oldIm;				
			}
			//if the point is outside the circle with radius 2: stop
			if((newRe * newRe + newIm * newIm) > 4) break;
		}
		//use color model conversion to get rainbow palette, make brightness black if maxIterations reached
	ColorHSV hsv;
	hsv.h = (i + colourOffset) % 256;
	hsv.s = 255;
	hsv.v = 255 * (i < maxIterations);
	color = HSVtoRGB(hsv);
		//color = HSVtoRGB(ColorHSV((i + colourOffset) % 256, 255, 255 * (i < maxIterations)));
		//draw the pixel
		//pset(x, y, color);
	if (pr == 1)
		color.r = color.g = color.b = 0;
	image[4 * w * y + 4 * x + 0] = color.r;
	image[4 * w * y + 4 * x + 1] = color.g;
	image[4 * w * y + 4 * x + 2] = color.b;
	image[4 * w * y + 4 * x + 3] = 255;
	/*
	image[4 * width * y + 4 * x + 0] = 255 * !(x & y);
		image[4 * width * y + 4 * x + 1] = x ^ y;
		image[4 * width * y + 4 * x + 2] = x | y;
		image[4 * width * y + 4 * x + 3] = 255;
	*/
	}
	printf("done\nencoding...\n");
	encodeTwoSteps(filename, image, width, height);
	printf("done\n");
	//sleep();
	return 0;
}