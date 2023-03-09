#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

LRESULT CALLBACK EventProcessor (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{ return(DefWindowProc(hWnd, uMsg, wParam, lParam)); }

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASS		wc;
	HWND			WindowHandle;
	int				ROWS, COLS, BYTES;
	unsigned char	*dispData;
	BITMAPINFO		*bm_info;
	HDC				hDC;
	int				i,j, SIZE;

	FILE			*fpt;
	char			header[80];
	bool			RGB = false;
	unsigned char	*fileData;
	
	fpt = fopen(__argv[1],"rb");

	if (fpt == NULL) 
	{
		MessageBox(NULL, L"Unable to open file", L"try again", MB_OK | MB_APPLMODAL);
		exit(0);
	}

	i = fscanf(fpt, "%s %d %d %d ", header, &COLS, &ROWS, &BYTES);
	if (i != 4 || (strcmp(header, "P5") != 0 && strcmp(header,"P6") != 0) || BYTES != 255) 
	{
		MessageBox(NULL, L"Incorrect file type! Please use an 8-bit PPM Image!", L"try again", MB_OK | MB_APPLMODAL);
		exit(0);
	} else {
		
	}

	if(strcmp(header, "P5") == 0)
	{
		MessageBox(NULL, L"Using a Greyscale Image!", L"", MB_OK | MB_APPLMODAL);
	} else {
		MessageBox(NULL, L"Using an RGB Image!", L"", MB_OK | MB_APPLMODAL);
		RGB = true;
	}

	if (RGB) { SIZE = ROWS*COLS*3; } else { SIZE = ROWS*COLS; }

	// DYNAMIC ALLOCATION OF fileData
	fileData = (unsigned char *)calloc(SIZE, sizeof(unsigned char));
	if(fileData == NULL) 
	{
		MessageBox(NULL, L"Unable to allocate memory!", L"", MB_OK | MB_APPLMODAL);
		exit(0);
	}

	// READ IN IMAGE DATA
	fread(fileData, 1, SIZE, fpt);
	fclose(fpt);	// done with file, close

	unsigned char tmp;

	if(!RGB)
	{
		// manually reverse pixels
	/*	i = 0;
		j = ROWS*COLS-1;
		unsigned char pix;
		while (i < j) 
		{
			pix = fileData[i];
			fileData[i++] = fileData[j];
			fileData[j--] = pix;
		} */

		dispData = fileData;
	
	} else {
		dispData = (unsigned char *)calloc(ROWS*COLS*2, sizeof(unsigned char));
		for( i = 0; i < COLS*ROWS; i++ )
		{
			tmp = fileData[i*3] & 0xF8;	// red
			dispData[i*2+1] = 0x00; // redundant
			dispData[i*2+1] |= tmp >> 1;
			
			tmp = fileData[i*3+1] & 0xF8; // green
			dispData[i*2+1] |= tmp >> 6;
			dispData[i*2] |= tmp << 2;

			tmp = fileData[i*3+2] & 0xF8; //blue
			dispData[i*2] |= tmp >> 3;
		}
	}

	wc.style=CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc=(WNDPROC)EventProcessor;
	wc.cbClsExtra=wc.cbWndExtra=0;
	wc.hInstance=hInstance;
	wc.hIcon=NULL;
	wc.lpszMenuName=NULL;
	wc.hCursor=LoadCursor(NULL,IDC_ARROW);
	wc.hbrBackground=(HBRUSH)(COLOR_WINDOW+1);
	wc.lpszClassName=L"Image Window Class";
	if(RegisterClass(&wc) == 0)
		exit(0); 
	
	int window_height = ROWS;
	if(RGB) {window_height += 30;}
	WindowHandle=CreateWindow(L"Image Window Class", L"ECE468 Lab1", WS_OVERLAPPEDWINDOW, 10,10,COLS,window_height,NULL,NULL,hInstance,NULL);
	
	if(WindowHandle == NULL)
	{
		MessageBox(NULL, L"No Window", L"try again", MB_OK | MB_APPLMODAL);
		exit(0);
	}
	ShowWindow(WindowHandle, SW_SHOWNORMAL);

	bm_info = (BITMAPINFO *)calloc(1,sizeof(BITMAPINFO) + 256*sizeof(RGBQUAD));
	hDC=GetDC(WindowHandle); 

	/*setup bmiheader */
	bm_info->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bm_info->bmiHeader.biWidth = COLS;
	bm_info->bmiHeader.biHeight = -1*ROWS; //load backwards
	bm_info->bmiHeader.biPlanes = 1;
	bm_info->bmiHeader.biBitCount = 8;
	bm_info->bmiHeader.biCompression = BI_RGB;
	bm_info->bmiHeader.biSizeImage = 0;
	bm_info->bmiHeader.biXPelsPerMeter = 0;
	bm_info->bmiHeader.biYPelsPerMeter = 0;
	bm_info->bmiHeader.biClrUsed = 0;
	bm_info->bmiHeader.biClrImportant = 0;

	if(RGB) 
	{
		bm_info->bmiHeader.biBitCount = 16;
	}

	//colormap
	for(i=0; i<256; i++)
	{
		bm_info->bmiColors[i].rgbBlue=bm_info->bmiColors[i].rgbGreen=bm_info->bmiColors[i].rgbRed=i;
		bm_info->bmiColors[i].rgbReserved=0;
	}
	SetDIBitsToDevice(hDC,0,0,COLS,ROWS,0,0,0,ROWS,dispData,bm_info,DIB_RGB_COLORS);
	ReleaseDC(WindowHandle,hDC);



	free(bm_info);

	free(fileData);
	if(RGB)
		free(dispData);

	//MessageBox(NULL,L"Press OK to continue",L"",MB_OK | MB_APPLMODAL); 
	Sleep(5000);

}