// Image.cpp : Defines the entry point for the application.

#include "Image.h"

MyImage myImage;

#define MAX_LOADSTRING 100
#define MAX_COLOR_VARIATIONS 256
#define MAT_OLD 0
#define MAT_NEW 1
#define PI 3.142857

// Image size conditional execution (MUTUALLY EXCLUSIVE)
#define SQUARE 1
#define RECTANGULAR 0
#define WIDTH 512
#define HEIGHT 512

// If only DWT_ACTIVE -> DWT output is displayed
#define DWT_ACTIVE 1

// If DWT_ACTIVE and IDWT_ACTIVE -> IDWT output is displayed
#define IDWT_ACTIVE 1

// If DWT/IDWT are active , then which one to wavelet transform?
#define DECOMP_ROW 1
#define DECOMP_COLUMN 1

// If you are performing wavelet transform, then how many passes to perform along each dimension?
// Passes (works only if the above DECOMP_ROW / DECOMP_COLUMN are active respectively
#define PASS_ROW 1
#define PASS_COL 1
//		^
//		|	both are mutually exclusive in execution
//		v
#define PASS_FULL_ROW 1		// (IF THESE ARE SET, PASS_ROW/PASS_COL HAVE NO EFFECT)
#define PASS_FULL_COLUMN 1		// (TAKES 0 / 1)

// Print into File (ANY COMBINATION)
#define PRINTDWT 0					// Filename : DWT_B4.txt    DWT_AFTR.txt
#define PRINTIDWT 0					// Filename : IDWT_B4.txt	IDWT_AFTR.txt
#define PRINT_UNSORTEDARRAY 0
#define PRINT_SORTEDARRAY 0

// Used for printing DWT/IDWT before and after its processing (NO NEED TO CHANGE VALUES)
#define BEFORE 1
#define AFTER 2
#define AFTER_ZEROFY 3	// Used only for DWT for printing DCT matrix after zerofying the values less than the threshold

// Indicates which dimension is passed for decomposition step (NO NEED TO CHANGE VALUES)
#define ROW 1
#define COLUMN 2

// Convert 2D to 1D array, used in 2 cases (NO NEED TO CHANGE VALUES)
#define LINEAR_FOR_DISPLAY 1
#define LINEAR_FOR_SORT 2

// Sorting Print Array - to indicate which array to print (NO NEED TO CHANGE VALUES)
#define UNSORTED 1
#define SORTED 2

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

	int w, h, biggerDimension = 0, twoPowerDimension = 0, i = 0;
	long int coEff = 0;

	if(SQUARE == 1)
	{
		w = WIDTH;
		h = HEIGHT;
	}
	else if(RECTANGULAR == 1)
	{
		w = WIDTH;
		h = HEIGHT;

		if(w > h)
			biggerDimension = w;
		else
			biggerDimension = h;

		while(twoPowerDimension < biggerDimension)
		{
			twoPowerDimension = pow(2.0, i);
			i++;
		}

		myImage.setPaddedWidth(twoPowerDimension);
		myImage.setPaddedHeight(twoPowerDimension);
		myImage.setTwoPowerDimension(twoPowerDimension);
	}

	char ImagePath[_MAX_PATH];
	sscanf(lpCmdLine, "%s %d %d", &ImagePath, &coEff);

	myImage.setWidth(w);
	myImage.setHeight(h);
	myImage.setCoEff(coEff);
	myImage.setImagePath(ImagePath);
	
	myImage.FillInputRGBSpace();

	myImage.GrayScale2YUV();

	if(DWT_ACTIVE == 1)
		myImage.DWTBasedCompDecomp();

	if(IDWT_ACTIVE == 1)
		myImage.IDWTBasedCompDecomp();

	myImage.YUV2RGB();

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
				sprintf(text, "\n %s -> Gray Scaled -> YUV -> DWT -> Take %d Co-Efficient(s) -> IDWT       ", myImage.getImagePath(), myImage.getCoEff());
				DrawText(hdc, text, strlen(text), &rt, DT_RIGHT);

				// Draw image 
				if(SQUARE == 1)
				{
					SetDIBitsToDevice(hdc,
									  10,
									  50,
									  myImage.getWidth(),
									  myImage.getHeight(),
									  0,
									  0,
									  0,
									  myImage.getHeight(),
									  myImage.getBytesRGBSTART(),&bmi,DIB_RGB_COLORS);
				}
				else if(RECTANGULAR == 1)
				{
										SetDIBitsToDevice(hdc,
									  10,
									  50,
									  myImage.getWidth(),
									  myImage.getHeight(),
									  0,
									  0,
									  0,
									  myImage.getHeight(),
									  myImage.getBytesRGBINPUT(),&bmi,DIB_RGB_COLORS);
				}
				
				SetDIBitsToDevice(hdc,
								  100+myImage.getWidth()+50,
								  50,
								  myImage.getWidth(),
								  myImage.getHeight(),
								  0,
								  0,
								  0,
								  myImage.getHeight(),
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
	if(SQUARE == 1)
	{
		bytesRGBOUTSTART = new unsigned char[Width*Height*3];

		// init to zero
		memset(bytesRGBOUTSTART, 0, Width*Height*3);

		/* Storing Format:
			[BGR][BGR].....[BGR]
		*/
		for (i = 0; i < Height*Width; i++)
		{
			bytesRGBOUTSTART[3*i]	= fgetc(FD);
			bytesRGBOUTSTART[3*i+1]	= bytesRGBOUTSTART[3*i];
			bytesRGBOUTSTART[3*i+2]	= bytesRGBOUTSTART[3*i];
		}
	}
	else if(RECTANGULAR == 1)
	{
		bytesRGBOUTSTART = new unsigned char[getPaddedWidth() * getPaddedHeight() * 3];
		bytesRGBOUTINPUT = new unsigned char[getPaddedWidth() * getPaddedHeight() * 3];

		// init to zero
		memset(bytesRGBOUTSTART, 0, getPaddedWidth() * getPaddedHeight() * 3);

		/* Storing Format:
		[BGR][BGR].....[BGR]
		*/
		for (int row = 0; row < Height; row++)
		{
			for (int col = 0; col < Width; col++)
			{
				unsigned char value = fgetc(FD);
				bytesRGBOUTSTART[3*(row*getPaddedWidth() + col)] = value;
				bytesRGBOUTSTART[3*(row*getPaddedWidth() + col)+1] = value;
				bytesRGBOUTSTART[3*(row*getPaddedWidth() + col)+2] = value;

				bytesRGBOUTINPUT[3*(row*Width + col)] = value;
				bytesRGBOUTINPUT[3*(row*Width + col)+1] = value;
				bytesRGBOUTINPUT[3*(row*Width + col)+2] = value;
			}
		}
	}
	fclose(FD);
}


void MyImage::GrayScale2YUV()
{
	// Storing Format : [YYYYYY.....Y][UUUUUU.....U][VVVVVV.....V]
	if(SQUARE == 1)
	{
		floatsYUV = new float[Width*Height];

		for(int i = 0; i < Height*Width; i++)
		{	
			// Gray scale -> only Y channel is present (luminance) and no U & V (chrominance)
			floatsYUV[i] = bytesRGBOUTSTART[3*i];
		}
	}
	else if(RECTANGULAR == 1)
	{
		floatsYUV = new float[getPaddedWidth() * getPaddedHeight()];

		for(int i = 0; i < ( getPaddedHeight() * getPaddedWidth() ); i++)
		{	
			// Gray scale -> only Y channel is present (luminance) and no U & V (chrominance)
			floatsYUV[i] = bytesRGBOUTSTART[3*i];
		}
	}
}


void MyImage::YUV2RGB()
{	
	if(SQUARE == 1)
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
	else if(RECTANGULAR == 1)
	{
		// Storing Format : [BGR][BGR]....[BGR]
		bytesRGBOUTEND = new unsigned char[getPaddedWidth() * getPaddedHeight() * 3];

		for(int i = 0; i < getPaddedHeight() * getPaddedWidth(); i++)
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
}


/* Convert format from
	From Linear array [YYYY...Y][UUUU...U][VVVV...V]
	To 2D Array [YYYY..Y	[UUUU..U	[VVVV..V
				 YYYY..]	 UUUU..U]	 VVVV..V]
*/
void MyImage::TwoDArrayForm()
{
	int h = 0, w = 0;
	
	if(RECTANGULAR == 1)
	{
		//memory allocated for  elements of each column.
		floatsYDWTBlock = new float *[getPaddedHeight()];
		floatsYIDWTBlock = new float *[getPaddedHeight()];

		for( int i = 0 ; i < getPaddedHeight() ; i++ )
		{
			floatsYDWTBlock[i] = new float[getPaddedWidth()];
			floatsYIDWTBlock[i] = new float[getPaddedWidth()];
		}

		// Linear array into 2D array of values
		for(int i = 0; i < getPaddedHeight() * getPaddedWidth(); i++)
		{
			if(w < getPaddedWidth())
			{
				floatsYDWTBlock[h][w] = floatsYUV[i];
				
				w++;
			}
			else
			{
				h++;
				w = 0;
				
				floatsYDWTBlock[h][w] = floatsYUV[i];
				
				w++;
			}
		}
	}
	else if(SQUARE == 1)
	{
		//memory allocated for  elements of each column.
		floatsYDWTBlock = new float *[Height];
		floatsYIDWTBlock = new float *[Height];

		for( int i = 0 ; i < Height ; i++ )
		{
			floatsYDWTBlock[i] = new float[Width];
			floatsYIDWTBlock[i] = new float[Width];
		}

		// Linear array into 2D array of values
		for(int i = 0; i < Height*Width; i++)
		{
			if(w < Width)
			{
				floatsYDWTBlock[h][w] = floatsYUV[i];
				
				w++;
			}
			else
			{
				h++;
				w = 0;
				
				floatsYDWTBlock[h][w] = floatsYUV[i];
				
				w++;
			}
		}
	}
}

/* Convert format from
	From 2D Array [YYYY..Y	[UUUU..U	[VVVV..V
		    	   YYYY..]	 UUUU..U]	 VVVV..V]
	To Linear array [YYYY...Y][UUUU...U][VVVV...V]
*/
void MyImage::LinearArrayForm(int formatFor)
{
	if(formatFor == LINEAR_FOR_DISPLAY)
	{
		if(IDWT_ACTIVE != 1)
		{
			for(int row = 0; row < Height; row++)
			{
				for(int col = 0; col < Width; col++)
				{
					floatsYUV[(row*Width)+col] = floatsYDWTBlock[row][col];
				}
			}
		}
		else if(IDWT_ACTIVE == 1)
		{
			for(int row = 0; row < Height; row++)
			{
				for(int col = 0; col < Width; col++)
				{
					floatsYUV[(row*Width)+col] = floatsYIDWTBlock[row][col];
				}
			}
		}
	}
	else if(formatFor == LINEAR_FOR_SORT)
	{
		if(SQUARE == 1)
		{
			// floatSortArray will be Height x Width
			for(int row = 0; row < Height; row++)
			{
				for(int col = 0; col < Width; col++)
				{
					floatSortArray[(row*Width)+col] = floatsYDWTBlock[row][col];

					if(floatSortArray[(row*Width)+col] < 0)
						floatSortArray[(row*Width)+col] *= -1;
				}
			}
		}
		else if(RECTANGULAR == 1)
		{
			// floatSortArray will be PaddedHeight x PaddedWidth
			for(int row = 0; row < getPaddedHeight(); row++)
			{
				for(int col = 0; col < getPaddedWidth(); col++)
				{
					floatSortArray[(row*getPaddedWidth())+col] = floatsYDWTBlock[row][col];

					if(floatSortArray[(row*getPaddedWidth())+col] < 0)
						floatSortArray[(row*getPaddedWidth())+col] *= -1;
				}
			}
		}
	}
}

/* Perform DWT on Y channel of the image converted to YUV space

*/
void MyImage::DWTBasedCompDecomp()
{
	// Convert format from
	//From Linear array [YYYY...Y][UUUU...U][VVVV...V]
	//To 2D Array [YYYY..Y	[UUUU..U	[VVVV..V
	//			   YYYY..]	 UUUU..U]	 VVVV..V]

	// If SQUARE -> DWT & IDWT matix will be Height x Width Dimension
	// If RECTANGULAR -> DWT & IDWT matix will be PaddedHeight x PaddedWidth Dimension , with padding contents set to ZEROES
	TwoDArrayForm();
	
	if(PRINTDWT == 1)
	{
		remove("DWT_B4.txt");
		PrintDWT(BEFORE);
	}
	
	//Perform Discrete Wavelet Transform
	if(DECOMP_ROW == 1)
	{
		if(SQUARE == 1)
		{
			for(int row = 0; row < Height; row++)
			{
				Decomposition(row, ROW);
			}
		}
		else if(RECTANGULAR == 1)
		{
			for(int row = 0; row < getPaddedHeight(); row++)
			{
				Decomposition(row, ROW);
			}
		}
	}
	
	if(DECOMP_COLUMN == 1)
	{
		if(SQUARE == 1)
		{
			for(int col = 0; col < Width; col++)
			{
				Decomposition(col, COLUMN);
			}
		}
		else if(RECTANGULAR == 1)
		{
			for(int col = 0; col < getPaddedWidth(); col++)
			{
				Decomposition(col, COLUMN);
			}
		}
	}

	if(PRINTDWT == 1)
	{
		remove("DWT_AFTR.txt");
		PrintDWT(AFTER);
	}
	
	Sort_FindThreshold_ZerofyBeyond();

	if(PRINTDWT == 1)
	{
		remove("DWT_AFTR_ZEROFY.txt");
		PrintDWT(AFTER_ZEROFY);
	}

	// Copy DWT matrix into IDWT matrix for IDWT processing
	if(IDWT_ACTIVE == 1)
	{
		if(SQUARE == 1)
		{
			for(int row = 0; row < Height; row++)
			{
				for(int col = 0; col < Width; col++)
				{
					floatsYIDWTBlock[row][col] = floatsYDWTBlock[row][col];
				}
			}
		}
		else if(RECTANGULAR == 1)
		{
			for(int row = 0; row < getPaddedHeight(); row++)
			{
				for(int col = 0; col < getPaddedWidth(); col++)
				{
					floatsYIDWTBlock[row][col] = floatsYDWTBlock[row][col];
				}
			}
		}
	}

	if(IDWT_ACTIVE != 1)
	{
		// Convert format from 
		//From 2D Array [YYYY..Y	[UUUU..U	[VVVV..V
		//			   YYYY..]	 UUUU..U]	 VVVV..V]
		//To Linear array [YYYY...Y][UUUU...U][VVVV...V]
		LinearArrayForm(LINEAR_FOR_DISPLAY);
	}
}


// Perform Discrete Wavelet transformation on the incoming row/column
void MyImage::Decomposition(int rowcolNumber, int typeMacro)
{
	int u = 0, v = 0, currentWidth = 0, currentHeight = 0;
	
	if(typeMacro == ROW)
	{
		// Normalize input co-efficients
		for(int col = 0; col < Width; col++)
		{
			//floatsYDWTBlock[rowcolNumber][col] /= sqrt( (float)Width ); 
		}

		if(SQUARE == 1)
		{
			currentWidth = Width;
		}
		else if(RECTANGULAR == 1)
		{
			currentWidth = getPaddedWidth();
		}

		if(PASS_FULL_ROW == 1)
		{
			while(currentWidth >= 2 )// 1 = Width    2 = Width/2    3 = Width/4.....Complete = 2
			{
				DecompositionStep(rowcolNumber, currentWidth, typeMacro);
				currentWidth /= 2;
			}
		}
		else if(PASS_FULL_ROW != 1)
		{
			while(currentWidth >= ( Width / pow(2.0, ( PASS_ROW-1 ) ) ) )// 1 = Width    2 = Width/2    3 = Width/4.....Complete = 2
			{
				DecompositionStep(rowcolNumber, currentWidth, typeMacro);
				currentWidth /= 2;
			}
		}
	}
	else if(typeMacro == COLUMN)
	{
		// Normalize input co-efficients
		for(int row = 0; row < Height; row++)
		{
			//floatsYDWTBlock[rowcolNumber][col] /= sqrt( (float)Width ); 
		}

		if(SQUARE == 1)
		{
			currentHeight = Height;
		}
		else if(RECTANGULAR == 1)
		{
			currentHeight = getPaddedHeight();
		}

		if(PASS_FULL_COLUMN == 1)
		{
			while(currentHeight >= 2 )// 1 = Height    2 = Height/2    3 = Height/4.....Complete = 2
			{
				DecompositionStep(rowcolNumber, currentHeight, typeMacro);
				currentHeight /= 2;
			}
		}
		else if(PASS_FULL_COLUMN != 1)
		{
			while(currentHeight >= ( Height / pow(2.0, ( PASS_COL-1 ) ) ) )// 1 = Height    2 = Height/2    3 = Height/4.....Complete = 2
			{
				DecompositionStep(rowcolNumber, currentHeight, typeMacro);
				currentHeight /= 2;
			}
		}
	}
}

// Perform averaging and diffencing on pairs
void MyImage::DecompositionStep(int rowcolNumber, int rowcolLength, int typeMacro)
{
	float *TempRowCol = new float[rowcolLength];

	if(typeMacro == ROW)
	{
		for(int col = 0; col < rowcolLength/2; col++)
		{
			// Sums
			TempRowCol[col] = ( floatsYDWTBlock[rowcolNumber][(2*col)] + floatsYDWTBlock[rowcolNumber][(2*col + 1)] ) / sqrt( (float)2 );

			// Differences
			TempRowCol[rowcolLength/2 + col] = ( floatsYDWTBlock[rowcolNumber][(2*col)] - floatsYDWTBlock[rowcolNumber][(2*col + 1)] ) / sqrt( (float)2 );
		}
		
		// Store the HP + LP from temp array into DWT matrix
		for(int col = 0; col < rowcolLength; col++)
		{
			floatsYDWTBlock[rowcolNumber][col] = TempRowCol[col];
		}
	}
	else if(typeMacro == COLUMN)
	{
		for(int row = 0; row < rowcolLength/2; row++)
		{
			// Sums
			TempRowCol[row] = ( floatsYDWTBlock[(2*row)][rowcolNumber] + floatsYDWTBlock[(2*row + 1)][rowcolNumber] ) / sqrt( (float)2 );

			// Differences
			TempRowCol[rowcolLength/2 + row] = ( floatsYDWTBlock[(2*row)][rowcolNumber] - floatsYDWTBlock[(2*row + 1)][rowcolNumber] ) / sqrt( (float)2 );
		}
		
		// Store the HP + LP from temp array into DWT matrix 
		for(int row = 0; row < rowcolLength; row++)
		{
			floatsYDWTBlock[row][rowcolNumber] = TempRowCol[row];
		}
	}

	delete[] TempRowCol;
}


/* Perform IDWT on Y channel of the image converted to DWT

*/
void MyImage::IDWTBasedCompDecomp()
{
	if(PRINTIDWT == 1)
	{
		remove("IDWT_B4.txt");
		PrintIDWT(BEFORE);
	}

	//Perform Inverse Discrete Wavelet Transform
	if(DECOMP_COLUMN == 1)
	{
		if(SQUARE == 1)
		{
			for(int col = 0; col < Width; col++)
			{
				InverseDecomposition(col, COLUMN);
			}
		}
		else if(RECTANGULAR == 1)
		{
			for(int col = 0; col < getPaddedWidth(); col++)
			{
				InverseDecomposition(col, COLUMN);
			}
		}
	}

	if(DECOMP_ROW == 1)
	{
		if(SQUARE == 1)
		{
			for(int row = 0; row < Height; row++)
			{
				InverseDecomposition(row, ROW);
			}
		}
		else if(RECTANGULAR == 1)
		{
			for(int row = 0; row < getPaddedHeight(); row++)
			{
				InverseDecomposition(row, ROW);
			}
		}
	}

	if(PRINTIDWT == 1)
	{
		remove("IDWT_AFTR.txt");
		PrintIDWT(AFTER);
	}
	
	// Convert format from 
	//From 2D Array [YYYY..Y	[UUUU..U	[VVVV..V
	//			   YYYY..]	 UUUU..U]	 VVVV..V]
	//To Linear array [YYYY...Y][UUUU...U][VVVV...V]
	LinearArrayForm(LINEAR_FOR_DISPLAY);
}


// Perform Inverse Discrete Wavelet transformation on the incoming row/column
void MyImage::InverseDecomposition(int rowcolNumber, int typeMacro)
{
	int u = 0, v = 0, currentWidth = 0, currentHeight = 0;
	
	if(typeMacro == ROW)
	{
		if(PASS_FULL_ROW == 1)
		{
			currentWidth = 2;  // 1 = Width    2 = Width/2    3 = Width/4.....Complete = 2

			if(SQUARE == 1)
			{
				while( currentWidth <= Width ) 
				{
					InverseDecompositionStep(rowcolNumber, currentWidth, typeMacro);
					currentWidth *= 2;
				}
			}
			else if(RECTANGULAR == 1)
			{
				while( currentWidth <= getPaddedWidth() ) 
				{
					InverseDecompositionStep(rowcolNumber, currentWidth, typeMacro);
					currentWidth *= 2;
				}
			}
		}
		else if(PASS_FULL_ROW != 1)
		{
			if(SQUARE == 1)
			{
				currentWidth = ( Width / pow(2.0, ( PASS_ROW-1 ) ) );  // 1 = Width    2 = Width/2    3 = Width/4.....Complete = 2

				while( currentWidth <= Width ) 
				{
					InverseDecompositionStep(rowcolNumber, currentWidth, typeMacro);
					currentWidth *= 2;
				}
			}
			else if(RECTANGULAR == 1)
			{
				currentWidth = ( getPaddedWidth() / pow(2.0, ( PASS_ROW-1 ) ) );  // 1 = Width    2 = Width/2    3 = Width/4.....Complete = 2

				while( currentWidth <= getPaddedWidth() ) 
				{
					InverseDecompositionStep(rowcolNumber, currentWidth, typeMacro);
					currentWidth *= 2;
				}
			}
		}
	}
	else if(typeMacro == COLUMN)
	{
		if(PASS_FULL_COLUMN == 1)
		{
			currentHeight = 2;	// 1 = Height    2 = Height/2    3 = Height/4.....Complete = 2

			if(SQUARE == 1)
			{
				while(currentHeight <= Height )
				{
					InverseDecompositionStep(rowcolNumber, currentHeight, typeMacro);
					currentHeight *= 2;
				}
			}
			else if(RECTANGULAR == 1)
			{
				while(currentHeight <= getPaddedHeight() )
				{
					InverseDecompositionStep(rowcolNumber, currentHeight, typeMacro);
					currentHeight *= 2;
				}
			}
		}
		else if(PASS_FULL_COLUMN != 1)
		{
			if(SQUARE == 1)
			{
				currentHeight = ( Height / pow(2.0, ( PASS_COL-1 ) ) );	// 1 = Height    2 = Height/2    3 = Height/4.....Complete = 2

				while(currentHeight <= Height )
				{
					InverseDecompositionStep(rowcolNumber, currentHeight, typeMacro);
					currentHeight *= 2;
				}
			}
			else if(RECTANGULAR == 1)
			{
				currentHeight = ( getPaddedHeight() / pow(2.0, ( PASS_COL-1 ) ) );	// 1 = Height    2 = Height/2    3 = Height/4.....Complete = 2

				while(currentHeight <= getPaddedHeight() )
				{
					InverseDecompositionStep(rowcolNumber, currentHeight, typeMacro);
					currentHeight *= 2;
				}
			}
		}
	}
}


// Perform reverse averaging and reverse diffencing on pairs (1... , n/2+1...)
void MyImage::InverseDecompositionStep(int rowcolNumber, int rowcolLength, int typeMacro)
{
	float *TempRowCol = new float[rowcolLength];

	if(typeMacro == ROW)
	{
		for(int col = 0; col < rowcolLength/2; col++)
		{
			// Sums
			TempRowCol[2*col] = ( ( floatsYIDWTBlock[rowcolNumber][col] + floatsYIDWTBlock[rowcolNumber][(rowcolLength/2)+col] ) * sqrt( (float)2 ) ) / 2.0;
			
			// Differences
			TempRowCol[2*col + 1] = ( ( floatsYIDWTBlock[rowcolNumber][col] - floatsYIDWTBlock[rowcolNumber][(rowcolLength/2)+col] ) * sqrt( (float)2 ) ) / 2.0;
		}
		
		// Store the HP + LP from temp array into DWT matrix
		for(int col = 0; col < rowcolLength; col++)
		{
			floatsYIDWTBlock[rowcolNumber][col] = TempRowCol[col];
		}
	}
	else if(typeMacro == COLUMN)
	{
		for(int row = 0; row < rowcolLength/2; row++)
		{
			// Sums
			TempRowCol[2*row] = ( ( floatsYIDWTBlock[row][rowcolNumber] + floatsYIDWTBlock[(rowcolLength/2)+row][rowcolNumber] ) * sqrt( (float)2 ) ) / 2.0;

			// Differences
			TempRowCol[2*row + 1] = ( ( floatsYIDWTBlock[row][rowcolNumber] - floatsYIDWTBlock[(rowcolLength/2)+row][rowcolNumber] ) * sqrt( (float)2 ) ) / 2.0;
		}
		
		// Store the HP + LP from temp array into DWT matrix 
		for(int row = 0; row < rowcolLength; row++)
		{
			floatsYIDWTBlock[row][rowcolNumber] = TempRowCol[row];
		}
	}

	delete[] TempRowCol;
}


void MyImage::PrintDWT(int status)
{
	FILE *FD;

	if(status == BEFORE)
	{
		FD = fopen("DWT_B4.txt", "a+");
	}
	else if(status == AFTER)
	{
		FD = fopen("DWT_AFTR.txt", "a+");
	}
	else if(status == AFTER_ZEROFY)
	{
		FD = fopen("DWT_AFTR_ZEROFY.txt", "a+");
	}

	if (FD == NULL)
	{
		fprintf(stderr, "Error");
	}
	else
	{
		fprintf(FD, "\n\n");

		if(SQUARE == 1)
		{
			// floatsYDWTBlock will be Height x Width
			for(int row = 0; row < Height; row++)
			{
				for(int col = 0; col < Width; col++)
				{
					fprintf(FD, "%f   ", floatsYDWTBlock[row][col]);
				}
				
				fprintf(FD, "\n");
			}
		}
		else if(RECTANGULAR == 1)
		{
			// floatsYDWTBlock will be PaddedHeight x PaddedWidth
			for(int row = 0; row < getPaddedHeight(); row++)
			{
				for(int col = 0; col < getPaddedWidth(); col++)
				{
					fprintf(FD, "%f   ", floatsYDWTBlock[row][col]);
				}
				
				fprintf(FD, "\n");
			}
		}
	}
	fclose(FD);
}


void MyImage::PrintIDWT(int status)
{
	FILE *FD;
	
	if(status == BEFORE)
	{
		FD = fopen("IDWT_B4.txt", "a+");
	}
	else if(status == AFTER)
	{
		FD = fopen("IDWT_AFTR.txt", "a+");
	}

	if (FD == NULL)
	{
		fprintf(stderr, "Error");
	}
	else
	{
		fprintf(FD, "\n\n");

		if(SQUARE == 1)
		{
			// floatsYIDWTBlock will be Height x Width
			for(int row = 0; row < Height; row++)
			{
				for(int col = 0; col < Width; col++)
				{
					fprintf(FD, "%f   ", floatsYIDWTBlock[row][col]);
				}
				
				fprintf(FD, "\n");
			}
		}
		else if(RECTANGULAR == 1)
		{
			// floatsYIDWTBlock will be PaddedHeight x PaddedWidth
			for(int row = 0; row < getPaddedHeight(); row++)
			{
				for(int col = 0; col < getPaddedWidth(); col++)
				{
					fprintf(FD, "%f   ", floatsYIDWTBlock[row][col]);
				}
				
				fprintf(FD, "\n");
			}
		}
	}
	fclose(FD);
}


void MyImage::PrintArray(int state)
{
	FILE *FD;

	if(state == UNSORTED)
	{
		remove("Unsorted.txt");
		FD = fopen("Unsorted.txt", "a+");
	}
	else if(state == SORTED)
	{
		remove("Sorted.txt");
		FD = fopen("Sorted.txt", "a+");
	}

	if (FD == NULL)
	{
		fprintf(stderr, "Error");
	}
	else
	{
		fprintf(FD, "\n");
		if(SQUARE == 1)
		{
			// floatSortArray will be Height x Width
			for(long int i = 0; i < Width*Height; i++)
			{
				fprintf(FD, "%f   \n", floatSortArray[i]);
			}
		}
		else if(RECTANGULAR == 1)
		{
			// floatSortArray will be PaddedHeight x PaddedWidth
			for(long int i = 0; i < ( getPaddedWidth() * getPaddedHeight() ); i++)
			{
				fprintf(FD, "%f   \n", floatSortArray[i]);
			}
		}
	}
	fclose(FD);
}


void MyImage::Sort_FindThreshold_ZerofyBeyond()
{
	long int size, thresholdValue = 0;
	
	if(SQUARE == 1)
	{
		size = Width * Height;
	}
	else if(RECTANGULAR == 1)
	{
		size = getPaddedWidth() * getPaddedHeight();
	}

	floatSortArray = new double[size];

	// Convert 2D DCT matrix into 1D array for sorting
	LinearArrayForm(LINEAR_FOR_SORT);

	// Print Array before sorting
	if(PRINT_UNSORTEDARRAY == 1)
		PrintArray(UNSORTED);

	StableSort();

	thresholdValue = floatSortArray[getCoEff()-1];

	// Zerofy all DWT values which are less than the THRESHOLD VALUE obtained from the sorted array
	if(SQUARE == 1)
	{
		for(int row = 0; row < Height; row++)
		{
			for(int col = 0; col < Width; col++)
			{
				float value = floatsYDWTBlock[row][col];

				if(floatsYDWTBlock[row][col] < 0)
					value = floatsYDWTBlock[row][col] * -1.0;

				if(value <= thresholdValue)
				{
					floatsYDWTBlock[row][col] = 0.0;
				}
			}
		}
	}
	else if(RECTANGULAR == 1)
	{
		for(int row = 0; row < getPaddedHeight(); row++)
		{
			for(int col = 0; col < getPaddedWidth(); col++)
			{
				float value = floatsYDWTBlock[row][col];

				if(floatsYDWTBlock[row][col] < 0)
					value = floatsYDWTBlock[row][col] * -1.0;

				if(value <= thresholdValue)
				{
					floatsYDWTBlock[row][col] = 0.0;
				}
			}
		}
	}
		
	// Print Array after sorting
	if(PRINT_SORTEDARRAY == 1)
		PrintArray(SORTED);
}


void MyImage::StableSort()
{
	long int index = 0;

	vector<double> myvector;
	vector<double>::iterator it;

	if(SQUARE == 1)
	{
		myvector.assign( floatSortArray, floatSortArray + ( Height*Width ) );
	}
	else if(RECTANGULAR == 1)
	{
		myvector.assign( floatSortArray, floatSortArray + ( getPaddedHeight() * getPaddedWidth() ) );	
	}

	// STL Library sort call
	stable_sort (myvector.begin(), myvector.end());

	for (it=myvector.end()-1; it!=myvector.begin(); --it)
	{
		floatSortArray[index] = *it;
		index++;
	}
}

