#include <windows.h>
#include "resource.h"
#include "server.h"



LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK SubDlgProc( HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam );
void Cache_Table(HWND State, HWND Time, HWND Queue, HWND IP, HWND MAC, HWND ATTEMPT, server obj); 
BOOL CALLBACK SubDlgProc2( HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam );
BOOL CALLBACK SubDlgProc3( HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam );


HINSTANCE g_hInst;
LPSTR lpszClass="ARP";
HWND hDlg, handle;
HWND State, Time, Attempt, Queue, IP, MAC;
WNDPROC Old;
HBITMAP Broadcast[6], Unicast1[6], Unicast2[6], Unicast3[6], Expression;
char ti[16];



void CenterCenterWindow( HWND hWnd )
{
	RECT wrt;
	LONG iX, iY, iWidth, iHeight;
	GetWindowRect( hWnd, &wrt );

	iWidth  = wrt.right-wrt.left;
	iHeight = wrt.bottom-wrt.top;
	iX		= LONG( ( GetSystemMetrics( SM_CXSCREEN ) - iWidth ) / 2 );
	iY		= LONG( ( GetSystemMetrics( SM_CYSCREEN ) - iHeight ) / 2 );

	MoveWindow(hWnd, iX, iY, iWidth, iHeight, TRUE);
}


void ModifyStyle( HWND hwnd, UINT Remove, UINT Add )
{
	UINT style = GetWindowLong( hwnd, GWL_STYLE );

	style = style & ~Remove;
	style = style | Add;

	SetWindowLong( hwnd, GWL_STYLE, style );
	SetWindowPos( hwnd, 0, 0, 0, 0, 0, 
				SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_DRAWFRAME);
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	HWND hWnd;
	MSG msg;
	WNDCLASS wc;

	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hInstance=hInstance; // program identifier
	wc.lpfnWndProc = (WNDPROC)WndProc;
	wc.lpszClassName = lpszClass;
	wc.lpszMenuName=NULL;
	wc.style=CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&wc);
	g_hInst = hInstance;

	hWnd = CreateWindow(lpszClass, lpszClass, WS_OVERLAPPEDWINDOW,
		                CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
	                    NULL, (HMENU)NULL, hInstance, NULL);

	ShowWindow(hWnd, nCmdShow);
	while(GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

	PAINTSTRUCT ps;
	HDC hdc, MemDC;
	HBITMAP MyBitmap, OldBitmap;
	BITMAP bit;
	int bx, by;

	switch(msg)
	{

	case WM_PAINT :

		hdc = BeginPaint(hWnd, &ps);
		MemDC = CreateCompatibleDC(hdc);
		MyBitmap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP0));
		OldBitmap = (HBITMAP) SelectObject(MemDC, MyBitmap);
		GetObject(MyBitmap, sizeof(BITMAP), &bit);
		bx = bit.bmWidth;
		by = bit.bmHeight;
		BitBlt(hdc, 0, 0, bx, by, MemDC, 0, 0, SRCCOPY);
		SelectObject(MemDC, OldBitmap);
		DeleteObject(MyBitmap);
		DeleteDC(MemDC);
		EndPaint(hWnd, &ps);
		return 0;

	case WM_DESTROY :
		PostQuitMessage(0);
		return 0;

	case WM_CREATE :
		ModifyStyle( hWnd, WS_CAPTION, 0); // caption
		ModifyStyle( hWnd, 0, WS_EX_TOPMOST);
		ModifyStyle( hWnd, 0, WS_EX_TOOLWINDOW);
		ShowWindow( hWnd, SW_MAXIMIZE);
		return 0;

	
	case WM_LBUTTONDOWN :
		ShowWindow(hWnd, SW_HIDE);
		DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOG1), hDlg, SubDlgProc);
		return 0;


	}
	return (DefWindowProc(hWnd, msg, wParam, lParam));
}


BOOL CALLBACK SubDlgProc( HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam )
{
	static server simul(1, "163.152.221.100", "AC-12-CC-14-D5-11");
	static server obj1(2, "163.152.221.101", "AD-CC-AD-64-DD-B1");
	static server obj2(3, "163.152.221.102", "AC-17-D2-B4-DC-D3");
	static server obj3(4, "163.152.221.103", "AC-A8-3C-34-DD-71");

	PAINTSTRUCT ps;
	HDC hdc, MemDC;
	HBITMAP OldBitmap;

	static int Casting = 0;   // 1 - broadcast 
                          // 2 - unicast 1
                          // 3 - unicast 2
                          // 4 - unicast 3
	static int i = 0;



	switch( iMessage )
	{

		case WM_INITDIALOG:
		{

            // Subclassing (change the identifier(g_hInstane) from main window to dialog)/
			Old = (WNDPROC)SetClassLong(hDlg, GCL_WNDPROC, (LONG)SubDlgProc);

			// Set Timer//////////////////////////////////////////////////////
			SetTimer(hDlg, 1, 3000, NULL); // for the cache table
			SetTimer(hDlg, 2, 300, NULL); // for the EXPRESSION
			SendMessage(hDlg, WM_TIMER, 1, 0); // set dual timer

			// Modify location of window frame///////////////////////////////
			CenterCenterWindow(hDlg);

			// ListBox handle ///////////////////////////////////////////////
			State = GetDlgItem(hDlg, IDC_LIST5);
			Attempt = GetDlgItem(hDlg, IDC_LIST6);
			Queue = GetDlgItem(hDlg, IDC_LIST7);
			Time = GetDlgItem(hDlg, IDC_LIST8);
			IP = GetDlgItem(hDlg, IDC_LIST9);
			MAC = GetDlgItem(hDlg, IDC_LIST10);

			// BitMap Loading (broadcasting)/////////////////////////////////
			Expression = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP1)); // network configure
			Broadcast[0] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP1));
			Broadcast[1] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP2));
			Broadcast[2] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP3));
			Broadcast[3] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP4));
			Broadcast[4] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP5));
			Broadcast[5] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP6));
		
			// BitMap Loading (unicasting 1)///////////////////////////////////
			Unicast1[0] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP7));
			Unicast1[1] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP7_1));
			Unicast1[2] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP7_2));
			Unicast1[3] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP7_3));
			Unicast1[4] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP7_4));
			Unicast1[5] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP7_5));
			

			// BitMap Loading (unicasting 2)///////////////////////////////////
			Unicast2[0] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP8));
			Unicast2[1] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP8_1));
			Unicast2[2] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP8_2));
			Unicast2[3] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP8_3));
			Unicast2[4] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP8_4));
			Unicast2[5] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP8_5));
		
			// BitMap Loading (unicasting 3)///////////////////////////////////
			Unicast3[0] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP9));
			Unicast3[1] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP9_1));
			Unicast3[2] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP9_2));
			Unicast3[3] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP9_3));
			Unicast3[4] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP9_4));
			Unicast3[5] = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP9_5));
		

			return TRUE;
		}

		case WM_TIMER:

			switch(wParam)
			{

			case 1:
				// Cache control module
				if(Casting == 0)
				{
				simul.cache_update(30);
				Cache_Table(State, Time, Queue, IP, MAC, Attempt, simul);
				}
				break;

			case 2:

				//EXPRESSION

				if(Casting == 0) break;
				// Broadcast
				if(Casting == 1) 
				{
					hdc = GetDC(hDlg);
					MemDC = CreateCompatibleDC(hdc);
					OldBitmap = (HBITMAP) SelectObject(MemDC, Broadcast[i]);
					BitBlt(hdc, 55, 70, 600, 281, MemDC, 0, 0, SRCCOPY);
					SelectObject(MemDC, OldBitmap);
					DeleteDC(MemDC);
					ReleaseDC(hDlg, hdc);
					i++;

					if(i == 6)
					{
						i = 0;
						Sleep(200);
						Casting = simul.input(ti, obj1, obj2, obj3);
						if(Casting == 1) Casting = 0;
						strcpy_s(ti, "\0");
						return 0;
					}
				}
				// Unicast1
				if(Casting == 2) 
				{
					hdc = GetDC(hDlg);
					MemDC = CreateCompatibleDC(hdc);
					OldBitmap = (HBITMAP) SelectObject(MemDC, Unicast1[i]);
					BitBlt(hdc, 55, 70, 600, 281, MemDC, 0, 0, SRCCOPY);
					SelectObject(MemDC, OldBitmap);
					DeleteDC(MemDC);
					ReleaseDC(hDlg, hdc);
					if(i == 1 || i == 2) Sleep(900);
					i++;

					if(i == 6)
					{
						i = 0;
						Casting = 0;
						Sleep(500);
						Cache_Table(State, Time, Queue, IP, MAC, Attempt, simul);
					}
				}
				// Unicast2
				if(Casting == 3) 
				{
					hdc = GetDC(hDlg);
					MemDC = CreateCompatibleDC(hdc);
					OldBitmap = (HBITMAP) SelectObject(MemDC, Unicast2[i]);
					BitBlt(hdc, 55, 70, 600, 281, MemDC, 0, 0, SRCCOPY);
					SelectObject(MemDC, OldBitmap);
					DeleteDC(MemDC);
					ReleaseDC(hDlg, hdc);
					if(i == 1 || i == 2) Sleep(900);
					i++;

					if(i == 6)
					{
						i = 0;
						Casting = 0;
						Sleep(500);
						Cache_Table(State, Time, Queue, IP, MAC, Attempt, simul);
					}
				}
				// Unicast3
				if(Casting == 4) 
				{
					hdc = GetDC(hDlg);
					MemDC = CreateCompatibleDC(hdc);
					OldBitmap = (HBITMAP) SelectObject(MemDC, Unicast3[i]);
					BitBlt(hdc, 55, 70, 600, 281, MemDC, 0, 0, SRCCOPY);
					SelectObject(MemDC, OldBitmap);
					DeleteDC(MemDC);
					ReleaseDC(hDlg, hdc);
					if(i == 1 || i == 2) Sleep(900);
					i++;

					if(i == 6)
					{
						i = 0;
						Casting = 0;
						Sleep(500);
						Cache_Table(State, Time, Queue, IP, MAC, Attempt, simul);
					}
				}
			}

			return 0;

		case WM_PAINT :

			// Network config.
			hdc = BeginPaint(hDlg, &ps);
			MemDC = CreateCompatibleDC(hdc);
			OldBitmap = (HBITMAP) SelectObject(MemDC, Expression);
			BitBlt(hdc, 55, 70, 600, 281, MemDC, 0, 0, SRCCOPY);
			SelectObject(MemDC, OldBitmap);
			DeleteDC(MemDC);
			EndPaint(hDlg, &ps);

		case WM_COMMAND: 

			Cache_Table(State, Time, Queue, IP, MAC, Attempt, simul);

			switch( LOWORD( wParam ) )
			{
	
				case ID_MENUITEM_START :

					SetClassLong(hDlg, GCL_WNDPROC, (LONG)Old);
					DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOG2), hDlg, SubDlgProc2);
					if(ti[0] != '\0')
					{
						Casting = simul.output(ti, obj1, obj2, obj3);
						Cache_Table(State, Time, Queue, IP, MAC, Attempt, simul); 
					}
					return 0;

				case ID_MENUITEM_EXIT :
					DeleteObject(Expression);
					PostQuitMessage(1);
					return 0;


				case IDCANCEL:
					DeleteObject(Expression);
					PostQuitMessage(1);
					return 0;

			}
		return 0;

	}
	return 0;
}

BOOL CALLBACK SubDlgProc2( HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam )
{

	switch( iMessage )
	{

		case WM_INITDIALOG:
		{
			return TRUE;
		}


		case WM_COMMAND: 

			switch( LOWORD( wParam ) )
			{
	
				case IDOK :
					GetDlgItemText(hDlg, IDC_EDIT4, ti, 16);
					EndDialog( hDlg, 0);
					return 0;

				case IDCANCEL:
					EndDialog( hDlg, 0);
					return 0;

			}

		return 0;

	}
	return 0;
}

BOOL CALLBACK SubDlgProc3( HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam )
{

	switch( iMessage )
	{

		case WM_INITDIALOG:
		{
			return TRUE;
		}


		case WM_COMMAND: 

			switch( LOWORD( wParam ) )
			{
	
				case IDOK :
					GetDlgItemText(hDlg, IDC_EDIT5, ti, 16);
					EndDialog( hDlg, 0);
					return 0;

				case IDCANCEL:
					EndDialog( hDlg, 0);
					return 0;

			}

		return 0;

	}
	return 0;
}

void Cache_Table(HWND State, HWND Time, HWND Queue, HWND IP, HWND MAC, HWND ATTEMPT, server obj)
{
	int i;
	char ip[10][16];
	char mac[10][18];
	char attempt[10][3];
	char time[10][4];
	char queue[10][2];
	char state[10][10];

	for( i = 0 ; i < 10 ; i++)
	{
		wsprintf(ip[i], obj.table[i].ip);
		wsprintf(mac[i], obj.table[i].mac);
		wsprintf(attempt[i], "%d", obj.table[i].attempt);
		wsprintf(queue[i], "%d", obj.table[i].queue);
		wsprintf(time[i], "%d", obj.table[i].time_out);
		wsprintf(state[i], obj.table[i].state);
		SendMessage(State,LB_RESETCONTENT,0,0);
		SendMessage(Queue,LB_RESETCONTENT,0,0);
		SendMessage(IP,LB_RESETCONTENT,0,0);
		SendMessage(MAC,LB_RESETCONTENT,0,0);
		SendMessage(ATTEMPT,LB_RESETCONTENT,0,0);
		SendMessage(Time,LB_RESETCONTENT,0,0);

	}

	for ( i = 0 ; i < 10 ; i++)
	{
		SendMessage(State,LB_ADDSTRING,0,(LPARAM)state[i]);
		SendMessage(Queue,LB_ADDSTRING,0,(LPARAM)queue[i]);
		SendMessage(IP,LB_ADDSTRING,0,(LPARAM)ip[i]);
		SendMessage(MAC,LB_ADDSTRING,0,(LPARAM)mac[i]);
		SendMessage(ATTEMPT,LB_ADDSTRING,0,(LPARAM)attempt[i]);
		SendMessage(Time,LB_ADDSTRING,0,(LPARAM)time[i]);
	}
}






