/**
 * @file bmp.hpp
 * @brief
 * @author Nicolas Ciferri
 * @date 05.19.17
 */

#ifndef _BMP_H
#define _BMP_H
#include <iostream>
#include <cstdio>
#include <fstream>
#include <cstring>
#include <cassert>
#include <cstdlib>
#include <cmath>
#include <limits>
#include <vector>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sstream>
using namespace std;

typedef struct Pixel
{
	unsigned char r,g,b;
} Pixel;

typedef struct Image
{
	int w,h;
	Pixel* dat;
} Image;

Image* Charger(const char* fichier);
int Sauver(Image*,const char* filename, const char* type, const int nb);
Image* NouvelleImage(int w,int h);
Image* CopieImage(Image*);
void SetPixel(Image*,int i,int j,Pixel p);
Pixel GetPixel(Image*,int i,int j);
void DelImage(Image*);

#endif
 
