
#if !defined(AFX_IMAGE_H__E34B6FBB_5FED_457A_B3D1_D4844DCD4951__INCLUDED_)
#define AFX_IMAGE_H__E34B6FBB_5FED_457A_B3D1_D4844DCD4951__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#include "stdafx.h"
#include "resource.h"
#include <math.h>
#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <vector>
using namespace std;

// Class structure of Image 
class MyImage 
{

private:
	int		Width;
	int		Height;
	int		paddedWidth;
	int		paddedHeight;
	int		twoPowerDimension;
	long int CoEff;
	char	ImagePath[_MAX_PATH];

	unsigned char* bytesRGB;
	unsigned char* bytesRGBOUTSTART;
	unsigned char* bytesRGBOUTEND;
	unsigned char* bytesRGBOUTINPUT;

	float** floatsYDWTBlock;

	float** floatsYIDWTBlock;

	float* floatsYUV;

	double* floatSortArray;

public:
	MyImage() {};
	~MyImage() { if(bytesRGB) delete bytesRGB; };
	
	// SET methods
	void	setWidth(int w)  { Width = w; }; 
	void	setHeight(int h) { Height = h; }; 
	void	setPaddedWidth(int w)  { paddedWidth = w; }; 
	void	setPaddedHeight(int h) { paddedHeight = h; }; 
	void	setTwoPowerDimension(int d) { twoPowerDimension = d; };
	void	setBytesRGB(unsigned char *img ) { bytesRGB = img; };
	void	setImagePath(char *path) { strcpy(ImagePath, path); }
	void	setCoEff(int coEff) { CoEff = coEff; }

	// GET methods
	int		getPaddedWidth() { return paddedWidth; };
	int		getPaddedHeight() { return paddedHeight; };
	int		getTwoPowerDimension() { return twoPowerDimension; };
	int		getWidth() { return Width; };
	int		getHeight() { return Height; };
	unsigned char*	getBytesRGBSTART() { return bytesRGBOUTSTART; };
	unsigned char*	getBytesRGBEND() { return bytesRGBOUTEND; };
	unsigned char*	getBytesRGBINPUT() { return bytesRGBOUTINPUT; };
	char*	getImagePath() { return ImagePath; }
	int		getCoEff() { return CoEff; };

	// Computation methods
	void	FillInputRGBSpace();
	void	GrayScale2YUV();
	void	YUV2RGB();
	void	GrayScale();
	void	TwoDArrayForm();
	void	LinearArrayForm(int formatFor);

	// DWT
	void	DWTBasedCompDecomp();
	void	Decomposition(int rowcolNumber, int typeMacro);
	void	DecompositionStep(int rowcolNumber, int rowcolLength, int typeMacro);
	
	// IDWT
	void	IDWTBasedCompDecomp();
	void	InverseDecomposition(int rowcolNumber, int typeMacro);
	void	InverseDecompositionStep(int rowcolNumber, int rowcolLength, int typeMacro);
	
	// Sorting
	void	Sort_FindThreshold_ZerofyBeyond();
	void	StableSort();

	void	PrintDWT(int status);
	void	PrintIDWT(int status);
	void	PrintArray(int state);
};

#endif // !defined(AFX_IMAGE_H__E34B6FBB_5FED_457A_B3D1_D4844DCD4951__INCLUDED_)
