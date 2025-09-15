#include "pch.h"
#include "Sudoku_igra.h"
#include <windowsx.h>

int application::run()
{
	MSG msg;
	while (::GetMessage(&msg, NULL, 0, 0)) {
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}

tstring window::class_name()
{
	return {};
}

bool window::register_class(const tstring& name)
{
	WNDCLASS wc{};
	wc.lpfnWndProc = proc;
	wc.lpszClassName = name.c_str();
	wc.cbWndExtra = sizeof(window*);

	wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)::GetStockObject(WHITE_BRUSH);

	return ::RegisterClass(&wc) != 0;
}

tstring window::generate_class_name()
{
	static int n = 1;
	return load_text(IDS_APP_TITLE).c_str() + std::to_string(n++);
}

POINT get_point(LPARAM lp)
{
	POINT p;
	p.x = GET_X_LPARAM(lp);
	p.y = GET_Y_LPARAM(lp);
	return p;
}

bool window::create(HWND parent, DWORD style, LPCTSTR caption, UINT_PTR id_or_menu,
	int x, int y, int width, int height)
{
	tstring cn = class_name();
	if (cn.empty())
		register_class(cn = generate_class_name());
	hw = ::CreateWindow(cn.c_str(), caption, style, x, y, width, height, parent, (HMENU)id_or_menu, 0, this);
	return hw != 0;
}

window::operator HWND()
{
	return hw;
}

LRESULT CALLBACK window::proc(HWND hw, UINT msg, WPARAM wp, LPARAM lp)  
{  
   if (msg == WM_CREATE) {  
       CREATESTRUCT* pcs = reinterpret_cast<CREATESTRUCT*>(lp);  
       window* pw = reinterpret_cast<window*>(pcs->lpCreateParams);  
       ::SetWindowLongPtr(hw, 0, reinterpret_cast<LONG_PTR>(pw));  
       pw->hw = hw;  
       return pw->on_create(pcs);  
   }  

   window* pw = reinterpret_cast<window*>(::GetWindowLongPtr(hw, 0));  
   switch (msg)  
   {  
   case WM_CREATE:  
       pw->on_create(reinterpret_cast<CREATESTRUCT*>(lp));  
       return 0;
   case WM_PAINT:
   {
	   PAINTSTRUCT ps;
	   HDC hdc = ::BeginPaint(hw, &ps);
	   pw->on_paint(hdc);
	   ::EndPaint(hw, &ps);
	   return 0;
   }
   case WM_COMMAND:  
       pw->on_command(LOWORD(wp));  
       return 0;
   case WM_SIZE:
	   pw->on_size(LOWORD(lp), HIWORD(lp));
	   return 0;
   case WM_LBUTTONDOWN:
	   pw->on_left_button_down(get_point(lp));
	   return 0;
   case WM_DESTROY:
	   pw->on_destroy();
	   return 0;
   }  
   return ::DefWindowProc(hw, msg, wp, lp);  
}

//generira rijesenu sudoku igru
std::vector<std::vector<int>> sudoku_game::create_solved_sudoku()
{
	srand(time(0));
	std::vector<std::vector<int>> solved_sudoku = sudokuGenerator(0);
	return solved_sudoku;
}
//uklanja k brojeva iz sudoku igre
void sudoku_game::remove_numbers(std::vector<std::vector<int>>& grid, int k)
{
	int count = 0;
	while (count < k)
	{
		int row = rand() % 9;
		int col = rand() % 9;
		if (grid[row][col] != 0)
		{
			grid[row][col] = 0;
			count++;
		}
	}
}

//uklanja brojeve iz sudoku igre ovisno o odabranoj težini
int sudoku_game::create_mode(int mode)
{
	srand(time(0));
	int numbers_to_remove;
	if (mode == 0)
		numbers_to_remove = rand() % 10 + 29;
	else if (mode == 1)
		numbers_to_remove = rand() % 10 + 40;
	else if (mode == 2)
		numbers_to_remove = rand() % 10 + 51;

	return numbers_to_remove;
}

std::basic_string<TCHAR> window::load_text(int id)
{
	TCHAR text[100];
	LoadString(GetModuleHandle(NULL), id, text, sizeof(text) / sizeof(TCHAR));
	return text;
}

