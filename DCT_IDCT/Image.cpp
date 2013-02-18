// Image.cpp : Defines the entry point for the application.

#include "Image.h"

MyImage myImage;

#define MAX_LOADSTRING 100
#define MAX_COLOR_VARIATIONS 256
#define MAT_OLD 0
#define MAT_NEW 1
#define PI 3.142857

// Image size conditional execution
#define SQUARE 0
#define RECTANGULAR 1
#define WIDTH 352
#define HEIGHT 288

// If only DCT_ACTIVE -> DCT output is displayed
#define DCT_ACTIVE 1

// If DCT_ACTIVE and IDCT_ACTIVE -> based on QUANT_ACTIVE
//					 QUANT_ACTIVE ->output will be quantized IDCT
//					 QUANT_ACTIVE 0 -> output will be no-quantization IDCT
#define IDCT_ACTIVE 1

// If IDCT_ACTIVE = 0 and QUANT_ACTIVE -> output will be only DCT and quantized with NO IDCT
#define QUANT_ACTIVE 1

// If IDCT_ACTIVE != 1 && QUANT_ACTIVE == 1 && DEQUANT_ACTIVE == 1 -> output will be DCT, quantized and dequantized
#define DEQUANT_ACTIVE 1

// Print into File
#define PRINTDCT 0					// Filename : DCT.txt
#define PRINTIDCT 0					// Filename : IDCT.txt
#define PRINT_ZIGZAG_ACCESS 0		// Filename : Zigzag.txt
#define TESTZIGZAG 0				// Filename : test.txt

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// The title bar text

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	int w, h;
	int quant = 0, coEff = 0;

	if(SQUARE == 1)
	{
		w = WIDTH;
		h = HEIGHT;
	}
	else if(RECTANGULAR == 1)
	{
		w = WIDTH;
		h = HEIGHT;
	}

	char ImagePath[_MAX_PATH];
	sscanf(lpCmdLine, "%s %d %d", &ImagePath, &quant, &coEff);

	if(SQUARE == 1)
	{
		w = 512;
		h = 512;
	}
	else if(RECTANGULAR == 1)
	{
		w = 352;
		h = 288;
	}

	myImage.setWidth(w);
	myImage.setHeight(h);
	myImage.setQuant(quant);
	myImage.setCoEff(coEff);
	myImage.setImagePath(ImagePath);
	
	myImage.FillInputRGBSpace();

	myImage.GrayScale2YUV();

	if(DCT_ACTIVE == 1)
		myImage.DCTBasedCompDecomp();

	if(IDCT_ACTIVE == 1)
	{
		remove("zigzag.txt");
		myImage.IDCTBasedCompDecomp();
	}

	myImage.YUV2RGB();
	
	if(TESTZIGZAG == 1)
	{
		remove("test.txt");
		myImage.TestZigTheZag();
	}

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_IMAGE, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_IMAGE);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage is only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_IMAGE);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCSTR)IDC_IMAGE;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }
   
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);
	
   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	TCHAR szHello[MAX_LOADSTRING];
	LoadString(hInst, IDS_HELLO, szHello, MAX_LOADSTRING);
	
	switch (message)
	{
		case WM_COMMAND:
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
				case IDM_ABOUT:
				   DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
				   break;
				case IDM_EXIT:
				   DestroyWindow(hWnd);
				   break;
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_PAINT:
			{
				hdc = BeginPaint(hWnd, &ps);
				// TODO: Add any drawing code here...
				RECT rt;
				GetClientRect(hWnd, &rt);

				// Top Text
				char text[1000];
				
				// Image Header setup
				BITMAPINFO bmi;
				CBitmap bitmap;
				memset(&bmi,0,sizeof(bmi));
				bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
				bmi.bmiHeader.biWidth = myImage.getWidth();
				bmi.bmiHeader.biHeight = -myImage.getHeight();  // Use negative height.  DIB is top-down.
				bmi.bmiHeader.biPlanes = 1;
				bmi.bmiHeader.biBitCount = 24;
				bmi.bmiHeader.biCompression = BI_RGB;
				bmi.bmiHeader.biSizeImage = myImage.getWidth()*myImage.getHeight();

				// Draw Processed image
				sprintf(text, "\n       %s -> display", myImage.getImagePath());
				DrawText(hdc, text, strlen(text), &rt, DT_LEFT);
				sprintf(text, "\n %s -> Gray Scaled -> YUV -> DCT -> Quant [ %d ] -> Dequant [ %d ] -> IDCT [ %d Co-Efficient(s) ]       ", myImage.getImagePath(), myImage.getQuant(), myImage.getQuant(), myImage.getCoEff());
				DrawText(hdc, text, strlen(text), &rt, DT_RIGHT);

				// Draw image 
				SetDIBitsToDevice(hdc,
								  100,100,myImage.getWidth(),myImage.getHeight(),
								  0,0,0,myImage.getHeight(),
								  myImage.getBytesRGBSTART(),&bmi,DIB_RGB_COLORS);

				SetDIBitsToDevice(hdc,
								  300+myImage.getWidth()+50,100,myImage.getWidth(),myImage.getHeight(),
								  0,0,0,myImage.getHeight(),
								  myImage.getBytesRGBEND(),&bmi,DIB_RGB_COLORS);
				
				EndPaint(hWnd, &ps);
			}
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

// Mesage handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
				return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
    return FALSE;
}


// MyImage functions
void MyImage::FillInputRGBSpace()
{
	FILE *FD;
	int i;

	FD = fopen(ImagePath, "rb");

	if (FD == NULL)
	{
		fprintf(stderr, "Error");
		exit(0);
	}

	// Storing Format : [BGR][BGR]....[BGR]
	bytesRGBOUTSTART = new unsigned char[Width*Height*3];

	/* Storing Format:
		[BGR][BGR].....[BGR]
	*/
	for (i = 0; i < Height*Width; i++)
	{
		bytesRGBOUTSTART[3*i]	= fgetc(FD);
		bytesRGBOUTSTART[3*i+1]	= bytesRGBOUTSTART[3*i];
		bytesRGBOUTSTART[3*i+2]	= bytesRGBOUTSTART[3*i];
	}
	
	fclose(FD);
}


void MyImage::GrayScale2YUV()
{
	// Storing Format : [YYYYYY.....Y][UUUUUU.....U][VVVVVV.....V]
	floatsYUV = new float[Width*Height];
	
	for(int i = 0; i < Height*Width; i++)
	{	
		// Gray scale -> only Y channel is present (luminance) and no U & V (chrominance)
		floatsYUV[i] = bytesRGBOUTSTART[3*i];
	}
}


void MyImage::YUV2RGB()
{	
	// Storing Format : [BGR][BGR]....[BGR]
	bytesRGBOUTEND = new unsigned char[Width*Height*3];
	
	for(int i = 0; i < Height*Width; i++)
	{	
		if(floatsYUV[i] > 255)
			floatsYUV[i] = 255;
		if(floatsYUV[i] < 0)
			floatsYUV[i] = 0;
		
		/*Storing Format:
			[BGR][BGR].....[BGR]
		*/
		bytesRGBOUTEND[3*i+2] = floatsYUV[i];
		bytesRGBOUTEND[3*i+1] = floatsYUV[i];
		bytesRGBOUTEND[3*i] = floatsYUV[i];
	}
}


void MyImage::Quantize(int rowStart, int colStart)
{
	int QuantizationFactor = pow(2.0, getQuant());
	float val = 0, rem = 0;
	
	for(int row = rowStart; row < rowStart+8; row++)
	{
		for(int col = colStart; col < colStart+8; col++)
		{
			val = floatsYDCTBlock[row][col] / QuantizationFactor;

			// Rounding
			rem = fmod(val, (float)1.0);
			if(rem > 0.5)
				val = ceil(val);
			else
				val = floor(val);
			
			floatsYDCTQBlock[row][col] = val;
		}
	}
}


void MyImage::DeQuantize(int rowStart, int colStart)
{
	int DeQuantizationFactor = pow(2.0, getQuant());
	
	for(int row = rowStart; row < rowStart+8; row++)
	{
		for(int col = colStart; col < colStart+8; col++)
		{
			floatsYDCTDQBlock[row][col] = floatsYDCTQBlock[row][col] * DeQuantizationFactor;
		}
	}
}


/* Convert format from
	From Linear array [YYYY...Y][UUUU...U][VVVV...V]
	To 2D Array [YYYY..Y	[UUUU..U	[VVVV..V
				 YYYY..]	 UUUU..U]	 VVVV..V]
*/
void MyImage::TwoDArrayForm()
{
	int h = 0, w = 0;

	//memory allocated for  elements of each column.
	floatsYBlock = new float *[Height];

	for( int i = 0 ; i < Height ; i++ )
	{
		floatsYBlock[i] = new float[Width];
	}
	
	// Linear array into 2D array of values
	for(int i = 0; i < Height*Width; i++)
	{
		if(w < Width)
		{
			floatsYBlock[h][w] = floatsYUV[i];
			
			w++;
		}
		else
		{
			h++;
			w = 0;
			
			floatsYBlock[h][w] = floatsYUV[i];
			
			w++;
		}
	}

}

/* Convert format from
	From 2D Array [YYYY..Y	[UUUU..U	[VVVV..V
		    	   YYYY..]	 UUUU..U]	 VVVV..V]
	To Linear array [YYYY...Y][UUUU...U][VVVV...V]
*/
void MyImage::LinearArrayForm()
{
	if(IDCT_ACTIVE != 1 && QUANT_ACTIVE != 1)
	{
		for(int row = 0; row < Height; row++)
		{
			for(int col = 0; col < Width; col++)
			{
				floatsYUV[(row*Width)+col] = floatsYDCTBlock[row][col];
			}
		}
	}
	else if(IDCT_ACTIVE == 1)
	{
		for(int row = 0; row < Height; row++)
		{
			for(int col = 0; col < Width; col++)
			{
				floatsYUV[(row*Width)+col] = floatsYIDCTBlock[row][col];
			}
		}
	}
	else if((IDCT_ACTIVE != 1 && QUANT_ACTIVE == 1 && DEQUANT_ACTIVE != 1))
	{
		for(int row = 0; row < Height; row++)
		{
			for(int col = 0; col < Width; col++)
			{
				floatsYUV[(row*Width)+col] = floatsYDCTQBlock[row][col];
			}
		}
	}
	else if((IDCT_ACTIVE != 1 && QUANT_ACTIVE == 1 && DEQUANT_ACTIVE == 1))
	{
		for(int row = 0; row < Height; row++)
		{
			for(int col = 0; col < Width; col++)
			{
				floatsYUV[(row*Width)+col] = floatsYDCTDQBlock[row][col];
			}
		}
	}
}

/* Perform DCT on Y channel of the image converted to YUV space

*/
void MyImage::DCTBasedCompDecomp()
{
	//memory allocated for  elements of each column.
	floatsYDCTBlock = new float *[Height];

	for( int i = 0 ; i < Height ; i++ )
	{
		floatsYDCTBlock[i] = new float[Width];
	}
	
	if(QUANT_ACTIVE == 1)
	{
		floatsYDCTQBlock = new float *[Height];

		for( int i = 0 ; i < Height ; i++ )
		{
			floatsYDCTQBlock[i] = new float[Width];
		}
	}

	if(DEQUANT_ACTIVE == 1)
	{
		floatsYDCTDQBlock = new float *[Height];

		for( int i = 0 ; i < Height ; i++ )
		{
			floatsYDCTDQBlock[i] = new float[Width];
		}
	}

	// Convert format from
	//From Linear array [YYYY...Y][UUUU...U][VVVV...V]
	//To 2D Array [YYYY..Y	[UUUU..U	[VVVV..V
	//			   YYYY..]	 UUUU..U]	 VVVV..V]
	TwoDArrayForm();
	
	if(PRINTDCT == 1)
	{
		remove("DCT.txt");
	}
	//Perform Discrete Cosine Transform
	// Converts image from spatial or pixel domain into frequency domain
	for(int row = 0; row < Height; row += 8)
	{
		for(int col = 0; col < Width; col += 8)
		{
			DCT(row, col);
			
			if(PRINTDCT == 1)
			{
				PrintDCT(row, col);
			}

			if(QUANT_ACTIVE == 1)
				Quantize(row, col);

			if(DEQUANT_ACTIVE == 1)
				DeQuantize(row, col);
		}
	}
	
	if(IDCT_ACTIVE != 1)
	{
		// Convert format from 
		//From 2D Array [YYYY..Y	[UUUU..U	[VVVV..V
		//			   YYYY..]	 UUUU..U]	 VVVV..V]
		//To Linear array [YYYY...Y][UUUU...U][VVVV...V]
		LinearArrayForm();
	}
}


// Perform Discrete cosine transformation on the incoming 8x8 block
// Given an input 8x8 input f(x,y), the DCT attempts to compute all 64 basis frequency co-efficients F(u,v),
// such that the weighted combinations of these 64 basis images B(x,y) produces f(x,y)
void MyImage::DCT(int rowStart, int colStart)
{
	int u = 0, v = 0;

	for(int row = rowStart; row < rowStart+8; row++)
	{
		v = 0;
		for(int col = colStart; col < colStart+8; col++)
		{
			floatsYDCTBlock[row][col] = EvaluateDCTEquation(rowStart, colStart, u, v);
			
			v++;
		}

		u++;
	}
}


/* Evaluate DCT equation */
float MyImage::EvaluateDCTEquation(int rowStart, int colStart, int u, int v)
{
	float Fofuv = 0;
	int x = 0, y = 0;

	for(int row = rowStart; row < rowStart+8; row++)
	{
		y = 0;
		for(int col = colStart; col < colStart+8; col++)
		{
			Fofuv += ( floatsYBlock[row][col] * ( cos( (float)( ( (2*x +1)*u*PI )/16 ) ) ) * ( cos( (float)( ( (2*y +1)*v*PI )/16 ) ) ) );

			y++;
		}

		x++;
	}

	if(u == 0)
		Fofuv *= ( 1.0/sqrt( (float)2 ) );
	
	if(v == 0)
		Fofuv *= ( 1.0/sqrt( (float)2 ) );

	Fofuv *= 0.25;

	return Fofuv;
}


/* Perform IDCT on Y channel of the image converted to DCT

*/
void MyImage::IDCTBasedCompDecomp()
{
	//memory allocated for  elements of each column.
	floatsYIDCTBlock = new float *[Height];

	for( int i = 0 ; i < Height ; i++ )
	{
		floatsYIDCTBlock[i] = new float[Width];
	}
	
	if(PRINTIDCT == 1)
	{
		remove("IDCT.txt");
	}

	//Perform Discrete Cosine Transform
	// Converts image from spatial or pixel domain into frequency domain
	for(int row = 0; row < Height; row += 8)
	{
		for(int col = 0; col < Width; col += 8)
		{
			IDCT(row, col);

			if(PRINTIDCT == 1)
			{
				PrintIDCT(row, col);
			}
		}
	}
	
	// Convert format from 
	//From 2D Array [YYYY..Y	[UUUU..U	[VVVV..V
	//			   YYYY..]	 UUUU..U]	 VVVV..V]
	//To Linear array [YYYY...Y][UUUU...U][VVVV...V]
	LinearArrayForm();
}


void MyImage::IDCT(int rowStart, int colStart)
{
	int x = 0, y = 0;

	for(int row = rowStart; row < rowStart+8; row++)
	{
		y = 0;
		for(int col = colStart; col < colStart+8; col++)
		{
			if(getCoEff() == 64)
			{
				floatsYIDCTBlock[row][col] = EvaluateIDCTEquationOld(rowStart, colStart, x, y);
			}
			else
			{
				floatsYIDCTBlock[row][col] = EvaluateIDCTEquationNew(rowStart, colStart, x, y);
			}
		
			y++;
		}

		x++;
	}
}


/* Evaluate IDCT equation */
float MyImage::EvaluateIDCTEquationOld(int rowStart, int colStart, int x, int y)
{
	float fofxy = 0, CuCv = 1;
	int u = 0, v = 0;
	
	for(int row = rowStart; row < rowStart+8; row++)
	{
		v = 0;
		for(int col = colStart; col < colStart+8; col++)
		{
			if(u == 0)
				CuCv *= ( 1.0/sqrt( (float)2 ) );

			if(v == 0)
				CuCv *= ( 1.0/sqrt( (float)2 ) );
			
			if(QUANT_ACTIVE == 1 && DEQUANT_ACTIVE == 0)
				fofxy += (float)( CuCv * floatsYDCTQBlock[row][col] * ( cos( (float)( (2*x +1)*u*PI )/16 ) ) * ( cos( (float)( (2*y +1)*v*PI )/16 ) ) );
			else if(QUANT_ACTIVE == 1 && DEQUANT_ACTIVE == 1)
				fofxy += (float)( CuCv * floatsYDCTDQBlock[row][col] * ( cos( (float)( (2*x +1)*u*PI )/16 ) ) * ( cos( (float)( (2*y +1)*v*PI )/16 ) ) );
			else if(QUANT_ACTIVE == 0 && DEQUANT_ACTIVE == 0)
				fofxy += (float)( CuCv * floatsYDCTBlock[row][col] * ( cos( (float)( (2*x +1)*u*PI )/16 ) ) * ( cos( (float)( (2*y +1)*v*PI )/16 ) ) );

			v++;
			CuCv = 1;
		}
		
		u++;
	}
	
	fofxy *= 0.25;
	
	return fofxy;
}


/* Evaluate IDCT equation */
float MyImage::EvaluateIDCTEquationNew(int rowStart, int colStart, int x, int y)
{
	// Zig the zaggit
	int lineSize = 0;
	int coEffs = getCoEff();
	int countCoEffs = 0;
	float fofxy = 0, CuCv = 1;
	FILE *FD;

	FD = fopen("zigzag.txt", "a+");

	if (FD == NULL)
	{
		fprintf(stderr, "Error");
	}
	else
	{
		// Temp 8x8 2D-array
		float** arr;
		arr = new float *[8];
		for( int i = 0 ; i < 8 ; i++ )
			arr[i] = new float[8];

		for(int row = 0; row < 8; row++)
		{
			for(int col = 0; col < 8; col++)
			{
				if(QUANT_ACTIVE == 1 && DEQUANT_ACTIVE == 0)
					arr[row][col] = floatsYDCTQBlock[rowStart+row][colStart+col];
				else if(QUANT_ACTIVE == 1 && DEQUANT_ACTIVE == 1)
					arr[row][col] = floatsYDCTDQBlock[rowStart+row][colStart+col];
				else if(QUANT_ACTIVE == 0 && DEQUANT_ACTIVE == 0)
					arr[row][col] = floatsYDCTBlock[rowStart+row][colStart+col];
			}
		}
	
		for(int sLineIndex = 0; sLineIndex < 15; sLineIndex++)
		{
			// If Zig-Zag length is equal to the user specified CoEfficients count 
			// then stop accessing further DCT coefficients

			if(sLineIndex < 8)
				lineSize++;
			else
				lineSize--;
			
			int left = 0, right = 0;
			for(int sLineElement = 0; sLineElement < lineSize; sLineElement++)
			{
				CuCv = 1;
				
				if(sLineIndex < 8)
				{
					if( (sLineIndex % 2) == 0)
					{
						if(sLineElement == 0)
						{
							left = sLineIndex;
							right = 0;
						}
						
						if(left == 0)
							CuCv *= ( 1.0/sqrt( (float)2 ) );

						if(right == 0)
							CuCv *= ( 1.0/sqrt( (float)2 ) );

						if(countCoEffs == coEffs)
						{
							if(PRINT_ZIGZAG_ACCESS == 1)
								fprintf(FD, "%f    ", 0.0);

							fofxy += (float)( CuCv * 0 * ( cos( (float)( (2*x +1)*left*PI )/16 ) ) * ( cos( (float)( (2*y +1)*right*PI )/16 ) ) );
						}
						else
						{
							if(PRINT_ZIGZAG_ACCESS == 1)
							fprintf(FD, "%f    ", arr[left][right]);

							countCoEffs++;
							fofxy += (float)( CuCv * arr[left][right] * ( cos( (float)( (2*x +1)*left*PI )/16 ) ) * ( cos( (float)( (2*y +1)*right*PI )/16 ) ) );
						}

						left--;
						right++;

					}
					else
					{
						if(sLineElement == 0)
						{
							left = 0;
							right = sLineIndex;
						}

						if(left == 0)
							CuCv *= ( 1.0/sqrt( (float)2 ) );

						if(right == 0)
							CuCv *= ( 1.0/sqrt( (float)2 ) );

						if(countCoEffs == coEffs)
						{
							if(PRINT_ZIGZAG_ACCESS == 1)
								fprintf(FD, "%f    ", 0.0);

							fofxy += (float)( CuCv * 0 * ( cos( (float)( (2*x +1)*left*PI )/16 ) ) * ( cos( (float)( (2*y +1)*right*PI )/16 ) ) );
						}
						else
						{
							if(PRINT_ZIGZAG_ACCESS == 1)
							fprintf(FD, "%f    ", arr[left][right]);

							countCoEffs++;
							fofxy += (float)( CuCv * arr[left][right] * ( cos( (float)( (2*x +1)*left*PI )/16 ) ) * ( cos( (float)( (2*y +1)*right*PI )/16 ) ) );
						}

						left++;
						right--;
					}
				}
				else
				{
					if( ( ( sLineIndex - 7 ) % 2) == 0)
					{
						if(sLineElement == 0)
						{
							left = sLineIndex - 7;
							right = 7;
						}

						if(left == 0)
							CuCv *= ( 1.0/sqrt( (float)2 ) );

						if(right == 0)
							CuCv *= ( 1.0/sqrt( (float)2 ) );

						if(countCoEffs == coEffs)
						{
							if(PRINT_ZIGZAG_ACCESS == 1)
								fprintf(FD, "%f    ", 0.0);

							fofxy += (float)( CuCv * 0 * ( cos( (float)( (2*x +1)*left*PI )/16 ) ) * ( cos( (float)( (2*y +1)*right*PI )/16 ) ) );
						}
						else
						{
							if(PRINT_ZIGZAG_ACCESS == 1)
							fprintf(FD, "%f    ", arr[left][right]);

							countCoEffs++;
							fofxy += (float)( CuCv * arr[left][right] * ( cos( (float)( (2*x +1)*left*PI )/16 ) ) * ( cos( (float)( (2*y +1)*right*PI )/16 ) ) );
						}

						left++;
						right--;
					}
					else
					{
						if(sLineElement == 0)
						{
							left = 7;
							right = sLineIndex - 7;
						}

						if(left == 0)
							CuCv *= ( 1.0/sqrt( (float)2 ) );

						if(right == 0)
							CuCv *= ( 1.0/sqrt( (float)2 ) );

						if(countCoEffs == coEffs)
						{
							if(PRINT_ZIGZAG_ACCESS == 1)
								fprintf(FD, "%f    ", 0.0);

							fofxy += (float)( CuCv * 0 * ( cos( (float)( (2*x +1)*left*PI )/16 ) ) * ( cos( (float)( (2*y +1)*right*PI )/16 ) ) );
						}
						else
						{
							if(PRINT_ZIGZAG_ACCESS == 1)
							fprintf(FD, "%f    ", arr[left][right]);

							countCoEffs++;
							fofxy += (float)( CuCv * arr[left][right] * ( cos( (float)( (2*x +1)*left*PI )/16 ) ) * ( cos( (float)( (2*y +1)*right*PI )/16 ) ) );
						}
						
						left--;
						right++;
					}
				}	// if else (sLineIndex < 8)
			}	// for sLineElement
			
			if(PRINT_ZIGZAG_ACCESS == 1)
				fprintf(FD, "\n");

		}	// for sLineIndex
		
		if(PRINT_ZIGZAG_ACCESS == 1)
			fprintf(FD, "\n\n");

		fofxy *= 0.25;

		for(int row = 0; row < 8; row++)
		delete[] arr[row];

		delete[] arr;
	}
	fclose(FD);

	return fofxy;
}

void MyImage::PrintDCT(int rowStart, int colStart)
{
	FILE *FD;

	FD = fopen("DCT.txt", "a+");

	if (FD == NULL)
	{
		fprintf(stderr, "Error");
	}
	else
	{
		fprintf(FD, "\n\n\n");

		for(int row = rowStart; row < rowStart+8; row++)
		{
			for(int col = colStart; col < colStart+8; col++)
			{
				fprintf(FD, "%f      ", floatsYDCTBlock[row][col]);
			}
			
			fprintf(FD, "\n");
		}
	}
	fclose(FD);
}

void MyImage::PrintIDCT(int rowStart, int colStart)
{
	FILE *FD;

	FD = fopen("IDCT.txt", "a+");

	if (FD == NULL)
	{
		fprintf(stderr, "Error");
	}
	else
	{
		fprintf(FD, "\n\n\n");

		for(int row = rowStart; row < rowStart+8; row++)
		{
			for(int col = colStart; col < colStart+8; col++)
			{
				fprintf(FD, "%f      ", floatsYIDCTBlock[row][col]);
			}
			
			fprintf(FD, "\n");
		}
	}
	fclose(FD);
}

void MyImage::TestZigTheZag()
{
	int** arr;

	arr = new int *[8];

	for( int i = 0 ; i < 8 ; i++ )
	{
		arr[i] = new int[8];
	}

	for(int row = 0; row < 8; row++)
	{
		for(int col = 0; col < 8; col++)
		{
			arr[row][col] = 8*row + col;
		}
	}

	FILE *FD;

	FD = fopen("test.txt", "a+");

	if (FD == NULL)
	{
		fprintf(stderr, "Error");
	}
	else
	{
		for(int row = 0; row < 8; row++)
		{
			for(int col = 0; col < 8; col++)
			{
				fprintf(FD, "%d    ", arr[row][col]);
			}
			fprintf(FD, "\n");
		}
	}

	fclose(FD);
	
	FD = fopen("test.txt", "a+");

	if (FD == NULL)
	{
		fprintf(stderr, "Error");
	}
	else
	{
	fprintf(FD, "\n\n");
	// Zig the zaggit
	int lineSize = 0;

	for(int sLineIndex = 0; sLineIndex < 15; sLineIndex++)
	{
		if(sLineIndex < 8)
			lineSize++;
		else
			lineSize--;
		
		int left = 0, right = 0;
		for(int sLineElement = 0; sLineElement < lineSize; sLineElement++)
		{
			if(sLineIndex < 8)
			{
				if( (sLineIndex % 2) == 0)
				{
					if(sLineElement == 0)
					{
						left = sLineIndex;
						right = 0;
					}
					
					fprintf(FD, "%d    ", arr[left][right]);
					left--;
					right++;

				}
				else
				{
					if(sLineElement == 0)
					{
						left = 0;
						right = sLineIndex;
					}

					fprintf(FD, "%d    ", arr[left][right]);
					left++;
					right--;
				}
			}
			else
			{
				if( ( ( sLineIndex - 7 ) % 2) == 0)
				{
					if(sLineElement == 0)
					{
						left = sLineIndex - 7;
						right = 7;
					}

					fprintf(FD, "%d    ", arr[left][right]);
					left++;
					right--;
				}
				else
				{
					if(sLineElement == 0)
					{
						left = 7;
						right = sLineIndex - 7;
					}

					fprintf(FD, "%d    ", arr[left][right]);
					left--;
					right++;
				}
			}	// if else (sLineIndex < 8)
		}	// for sLineElement

		fprintf(FD, "\n");

	}	// for sLineIndex

	}
	
	fclose(FD);
}