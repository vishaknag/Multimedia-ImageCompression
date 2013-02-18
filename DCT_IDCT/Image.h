
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

// Class structure of Image 
class MyImage 
{

private:
	int		Width;
	int		Height;
	int		Quant;
	int		CoEff;
	char	ImagePath[_MAX_PATH];

	unsigned char* bytesRGB;
	unsigned char* bytesRGBOUTSTART;
	unsigned char* bytesRGBOUTEND;

	float** floatsYBlock;
	float** floatsUBlock;
	float** floatsVBlock;

	float** floatsYDCTBlock;
	float** floatsUDCTBlock;
	float** floatsVDCTBlock;

	float** floatsYDCTQBlock;
	float** floatsUDCTQBlock;
	float** floatsVDCTQBlock;

	float** floatsYDCTDQBlock;
	float** floatsUDCTDQBlock;
	float** floatsVDCTDQBlock;

	float** floatsYIDCTBlock;
	float** floatsUIDCTBlock;
	float** floatsVIDCTBlock;

	float* floatsYUV;

public:
	MyImage() {};
	~MyImage() { if(bytesRGB) delete bytesRGB; };
	
	// SET methods
	void	setWidth(int w)  { Width = w; }; 
	void	setHeight(int h) { Height = h; }; 
	void	setBytesRGB(unsigned char *img ) { bytesRGB = img; };
	void	setImagePath(char *path) { strcpy(ImagePath, path); }
	void	setQuant(int quant) { Quant = quant; }
	void	setCoEff(int coEff) { CoEff = coEff; }

	// GET methods
	int		getWidth() { return Width; };
	int		getHeight() { return Height; };
	unsigned char*	getBytesRGBSTART() { return bytesRGBOUTSTART; };
	unsigned char*	getBytesRGBEND() { return bytesRGBOUTEND; };
	char*	getImagePath() { return ImagePath; }
	int		getQuant() { return Quant; };
	int		getCoEff() { return CoEff; };

	// Computation methods
	void	FillInputRGBSpace();
	void	GrayScale2YUV();
	void	YUV2RGB();
	void	Quantize(int row, int col);
	void	DeQuantize(int row, int col);
	void	GrayScale();
	void	TwoDArrayForm();
	void	LinearArrayForm();
	void	DCTBasedCompDecomp();
	void	DCT(int rol, int col);
	float	EvaluateDCTEquation(int rowStart, int colStart, int u, int v);
	
	void	IDCTBasedCompDecomp();
	void	IDCT(int rowStart, int colStart);
	float	EvaluateIDCTEquationOld(int rowStart, int colStart, int x, int y);
	float	EvaluateIDCTEquationNew(int rowStart, int colStart, int x, int y);

	void	PrintDCT(int row, int col);
	void	PrintIDCT(int row, int col);

	void	TestZigTheZag();
};

#endif // !defined(AFX_IMAGE_H__E34B6FBB_5FED_457A_B3D1_D4844DCD4951__INCLUDED_)
