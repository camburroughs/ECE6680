#include <windows.h>

LRESULT CALLBACK EventProcessor (HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{ return(DefWindowProc(hWnd,uMsg,wParam,lParam)); }

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,
                   LPSTR     lpCmdLine,int nCmdShow)
{
WNDCLASS		wc;
HWND			WindowHandle;
int			ROWS,COLS;
unsigned char	*dispData;
BITMAPINFO		*bm_info;
HDC			hDC;

wc.style=CS_HREDRAW | CS_VREDRAW;
wc.lpfnWndProc=(WNDPROC)EventProcessor;
wc.cbClsExtra=wc.cbWndExtra=0;
wc.hInstance=hInstance;
wc.hIcon=wc.lpszMenuName=NULL;
wc.hCursor=LoadCursor(NULL,IDC_ARROW);
wc.hbrBackground=(HBRUSH)(COLOR_WINDOW+1);
wc.lpszClassName="Image Window Class";
if (RegisterClass(&wc) == 0)
  exit(0);

WindowHandle=CreateWindow("Image Window Class","ECE468 Lab1",
						  WS_OVERLAPPEDWINDOW,
						  10,10,COLS,ROWS,
						  NULL,NULL,hInstance,NULL);
if (WindowHandle == NULL)
  {
  MessageBox(NULL,"No window","Try again",MB_OK | MB_APPLMODAL);
  exit(0);
  }
ShowWindow (WindowHandle, SW_SHOWNORMAL);

bm_info=(BITMAPINFO *)calloc(1,sizeof(BITMAPINFO) + 256*sizeof(RGBQUAD));
hDC=GetDC(WindowHandle);

/* ... set up bmiHeader field of bm_info ... */

for (i=0; i<256; i++)	/* colormap */
  {
  bm_info->bmiColors[i].rgbBlue=bm_info->bmiColors[i].rgbGreen=bm_info->bmiColors[i].rgbRed=i;
  bm_info->bmiColors[i].rgbReserved=0;
  }
SetDIBitsToDevice(hDC,0,0,COLS,ROWS,0,0,
			  0, /* first scan line */
			  COLS, /* number of scan lines */
			  dispData,bm_info,DIB_RGB_COLORS);
ReleaseDC(WindowHandle,hDC);
free(bm_info);
MessageBox(NULL,"Press OK to continue","",MB_OK | MB_APPLMODAL);
}
