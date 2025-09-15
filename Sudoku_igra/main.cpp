#include "pch.h"
#include "main.h"
#include <format>

main_window::main_window() {
	ZeroMemory(&lf, sizeof(lf));
	_tcscpy_s(lf.lfFaceName, _T("Arial"));
	HDC hdc = ::GetDC(0);
	lf.lfHeight = -15 * ::GetDeviceCaps(hdc, LOGPIXELSY) / 72;
	cr = RGB(0, 0, 0);
	::ReleaseDC(0, hdc);
}

void main_window::game_start(int numbers_to_remove)
{
	game.setSolvedSudoku(game.create_solved_sudoku());
	std::vector<std::vector<int>> unsolved_sudoku = game.getSolvedSudoku();
	game.remove_numbers(unsolved_sudoku, numbers_to_remove);

	sudoku_buttons.reserve(IDS_GAME_SIZE);

	for (int i = 0; i < IDS_GAME_SIZE; i++)
	{
		int row = i / 9;
		int col = i % 9;
		if (unsolved_sudoku[row][col] != 0)
			sudoku_buttons.emplace_back(i, std::to_string(unsolved_sudoku[row][col]));
		else
			sudoku_buttons.emplace_back(i, "");
	}

	//kreiranje gumba za brojeve
	for (int i = 0; i < 9; i++)
		number_buttons.emplace_back(hw, IDC_BTN_NUMBER + i, std::to_string(i + 1));
	

	create_buttons();

}
//brise sve gumbe, stavlja vrijednosti na pocetno stanje i resetira igru
void main_window::reset_game()
{
	for (auto& b : sudoku_buttons)
		DestroyWindow(GetDlgItem(hw, b.getId()));
	sudoku_buttons.clear();

	for (auto& b : number_buttons)
		DestroyWindow(GetDlgItem(hw, b.getId()));
	number_buttons.clear();

	for (int i = IDS_BTN_DELETE; i < IDS_BTN_NOTES_OFF + 1; i++)
		DestroyWindow(GetDlgItem(hw, i));

	game.setSudoku_ID(- 1);
	game.setNumber_ID(- 1);
	game.setMistakes(0);
	game.setNotes_ON(false);
	game.setRect_Drawn(false);

	ShowWindow(e_button, SW_SHOW);
	ShowWindow(n_button, SW_SHOW);
	ShowWindow(h_button, SW_SHOW);

	InvalidateRect(hw, 0, true);
	UpdateWindow(hw);
}

void main_window::create_buttons()
{
	delete_button.create(*this, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, load_text(IDS_BTN_DELETE).c_str(), IDS_BTN_DELETE);
	solve_button.create(*this, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, load_text(IDS_BTN_SOLVE).c_str(), IDS_BTN_SOLVE);
	reset_button.create(*this, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, load_text(IDS_BTN_RESET).c_str(), IDS_BTN_RESET);
	notes_button.create(*this, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, load_text(IDS_BTN_NOTES_OFF).c_str(), IDS_BTN_NOTES_OFF);

	int m = game.getMistakes();
	std::string text = std::vformat(load_text(IDS_TXT_MISTAKES), std::make_format_args(m));
	mistakes_text.create(*this, WS_CHILD | WS_VISIBLE | SS_LEFT, text.c_str(), IDS_TXT_MISTAKES);
}
void main_window::paint_sudoku_buttons(HDC hdc)
{
	for (int i = 0; i < sudoku_buttons.size(); ++i)
	{
		RECT r = sudoku_buttons[i].getRect();

		COLORREF button_color = RGB(240, 240, 240);
		if (sudoku_buttons[i].getClickedButton())
			button_color = RGB(180, 180, 240);
		else if (sudoku_buttons[i].getNumberHighlighted())
			button_color = RGB(210, 210, 240);

		HBRUSH brush = CreateSolidBrush(button_color);
		FillRect(hdc, &r, brush);
		DeleteObject(brush);
		FrameRect(hdc, &r, (HBRUSH)GetStockObject(BLACK_BRUSH));
		SetBkMode(hdc, TRANSPARENT);

		//mijenjanje boje teksta ovisno da li je upisan tocan broj
		HFONT font;
		if (sudoku_buttons[i].getText().size() == 1)
		{
			font = (HFONT)SelectObject(hdc, button_font);

			if (sudoku_buttons[i].getText() != std::to_string(game.getSolvedSudoku()[i / 9][i % 9]))
				SetTextColor(hdc, RGB(255, 0, 0));
			else if (sudoku_buttons[i].getEnteredNumber())
				SetTextColor(hdc, RGB(0, 0, 255));
			else
				SetTextColor(hdc, RGB(0, 0, 0));
		}
		else
		{
			font = (HFONT)SelectObject(hdc, notes_font);
			SetTextColor(hdc, RGB(0, 0, 0));
		}
		//centriranje teksta
		RECT cen = r;
		DrawText(hdc, sudoku_buttons[i].getText().c_str(), -1, &cen,
			DT_CENTER | DT_WORDBREAK | DT_CALCRECT | DT_NOPREFIX);

		int vertical_offset = (r.bottom - r.top - (cen.bottom - cen.top)) / 2;
		r.top += vertical_offset;

		DrawText(hdc, sudoku_buttons[i].getText().c_str(), -1, &r,
			DT_CENTER | DT_WORDBREAK | DT_NOPREFIX);

		SelectObject(hdc, font);
	}
}
/*upisuje odabrani broj u sudoku polje, provjerava da li je broj tocan, ako je upisan treci krivi broj
	ispisuje se poruka o kraju igre, a ako je upisan dobar broj, isticu se sva polja sa istim brojem*/
void main_window::input_number()
{
	std::string number = number_buttons[game.getNumber_ID() - IDC_BTN_NUMBER].getText();
	sudoku_buttons[game.getSudoku_ID()].setText(number);
	sudoku_buttons[game.getSudoku_ID()].setEnteredNumber(true);

	for (auto& btn : sudoku_buttons)
		btn.setNumberHighlighted(false);

	for (int i = 0; i < sudoku_buttons.size(); i++)
	{
		if (sudoku_buttons[i].getText() == number && i != game.getSudoku_ID())
			sudoku_buttons[i].setNumberHighlighted(true);
	}

	if (sudoku_buttons[game.getSudoku_ID()].getText() !=
		std::to_string(game.getSolvedSudoku()[game.getSudoku_ID() / 9][game.getSudoku_ID() % 9]))
	{
		game.setMistakes(game.getMistakes() + 1);

		HWND hStatic = GetDlgItem(hw, IDS_TXT_MISTAKES);
		int m = game.getMistakes();
		std::string text = std::vformat(load_text(IDS_TXT_MISTAKES), std::make_format_args(m));
		SetWindowText(hStatic, text.c_str());

		if (game.getMistakes() >= 3)
		{
			InvalidateRect(hw, 0, false);
			if (MessageBox(hw, load_text(IDS_GAME_OVER_MESSAGE).c_str(), load_text(IDS_GAME_OVER_TITLE).c_str(),
				MB_YESNO | MB_ICONERROR) == IDYES)
				reset_game();
			else
				::PostQuitMessage(0);
		}
	}
}
//upisuje brojeve u obliku biljeske
void main_window::input_note()
{
	std::vector<std::string> note_numbers = sudoku_buttons[game.getSudoku_ID()].getNoteNumbers();
	std::string selected_note_number = number_buttons[game.getNumber_ID() - IDC_BTN_NUMBER].getText();

	int idx = stoi(selected_note_number) - 1;
	if (note_numbers[idx] == "")
		note_numbers[idx] = selected_note_number;

	sudoku_buttons[game.getSudoku_ID()].setNoteNumbers(note_numbers);
	sudoku_buttons[game.getSudoku_ID()].setText(
		  note_numbers[0] + "      " + note_numbers[1] + "      " + note_numbers[2] + "\n\n"
		+ note_numbers[3] + "      " + note_numbers[4] + "      " + note_numbers[5] + "\n\n"
		+ note_numbers[6] + "      " + note_numbers[7] + "      " + note_numbers[8]);
}

void main_window::game_won(int correct_counter)
{
	if (correct_counter == IDS_GAME_SIZE)
	{
		InvalidateRect(hw, 0, false);
		if (MessageBox(hw, load_text(IDS_YOU_WIN_MESSAGE).c_str(), load_text(IDS_YOU_WIN_TITLE).c_str(),
			MB_YESNO | MB_ICONINFORMATION) == IDYES)
			reset_game();
		else
			::PostQuitMessage(0);
	}
}
/*ako je odabran broj dok su biljeske iskljucene, upisuje se u sudoku polje u obliku broja, a ako su biljeske ukljucene,
upisuje se u obliku biljeske, takoder, provjerava se da li je korisnik pobjedio*/
void main_window::number_button_clicked(int id)
{
	game.setNumber_ID(id);
	if (game.getSudoku_ID() != -1)
	{
		if (!game.getNotes_ON() && sudoku_buttons[game.getSudoku_ID()].getText() == "")
		{
			input_number();
			int correct_counter = 0;
			for (int i = 0; i < sudoku_buttons.size(); i++)
			{
				if (std::to_string(game.getSolvedSudoku()[i / 9][i % 9]) == sudoku_buttons[i].getText())
					correct_counter++;
			}

			game_won(correct_counter);
		}
		else if (game.getNotes_ON() && sudoku_buttons[game.getSudoku_ID()].getText().size() != 1)
			input_note();
		InvalidateRect(hw, 0, false);
	}
}

//brisanje unesenog broja ili biljeske i isti brojevi vise nisu oznaceni
void main_window::delete_button_clicked()
{
	for (auto& b : sudoku_buttons)
	{
		b.setNumberHighlighted(false);

		if (b.getClickedButton() && b.getEnteredNumber())
			b.setText("");

		if (b.getClickedButton() && b.getText().size() > 1)
		{
			std::vector<std::string> empty_notes(9, "");
			b.setNoteNumbers(empty_notes);
			b.setText("");
		}
	}
	InvalidateRect(hw, 0, false);
}

/* ako je kliknut gumb "solve", prazno polje se ispunjava tocnim brojem, te ako je odabrano zadnje polje bilo
	prazno salje se poruka za kraj igre*/
void main_window::solve_button_clicked()
{
	int correct_counter = 0;
	std::string inserted_value = "";

	for (int i = 0; i < sudoku_buttons.size(); i++)
	{
		sudoku_buttons[i].setNumberHighlighted(false);

		if (sudoku_buttons[i].getClickedButton() && sudoku_buttons[i].getText() == "")
		{
			inserted_value = std::to_string(game.getSolvedSudoku()[i / 9][i % 9]);
			sudoku_buttons[i].setText(inserted_value);
			sudoku_buttons[i].setEnteredNumber(true);
		}
	}

	if (!inserted_value.empty())
	{
		for (auto& b : sudoku_buttons)
		{
			if (b.getText() == inserted_value && !b.getClickedButton())
			{
				b.setNumberHighlighted(true);
				HWND button = GetDlgItem(hw, b.getId());
				InvalidateRect(button, 0, false);
			}
		}
	}

	for (int i = 0; i < sudoku_buttons.size(); i++)
	{
		if (std::to_string(game.getSolvedSudoku()[i / 9][i % 9]) == sudoku_buttons[i].getText())
			correct_counter++;
	}

	game_won(correct_counter);
	InvalidateRect(hw, 0, false);
}

void main_window::difficulty_button_clicked(int difficulty)
{
	game.setRect_Drawn(true);
	InvalidateRect(hw, 0, false);
	game_start(game.create_mode(difficulty));

	ShowWindow(e_button, SW_HIDE);
	ShowWindow(n_button, SW_HIDE);
	ShowWindow(h_button, SW_HIDE);

	RECT r;
	GetClientRect(hw, &r);
	int w = r.right - r.left;
	int h = r.bottom - r.top;
	on_size(w, h);
}

//mijenja velicinu, poziciju i font gumba ovisno o velicini prozora
void main_window::scale_buttons(int w, int h, HFONT scaled_font)
{
	int dif_w = w / 7;
	int dif_h = h / 11;
	int dif_x = (w - dif_w) / 2;
	int dif_y = (h - dif_h) / 2;

	MoveWindow(e_button, dif_x, dif_y * 0.4, dif_w, dif_h, true);
	MoveWindow(n_button, dif_x, dif_y * 0.7, dif_w, dif_h, true);
	MoveWindow(h_button, dif_x, dif_y, dif_w, dif_h, true);

	SendMessage(e_button, WM_SETFONT, (WPARAM)scaled_font, true);
	SendMessage(n_button, WM_SETFONT, (WPARAM)scaled_font, true);
	SendMessage(h_button, WM_SETFONT, (WPARAM)scaled_font, true);

	int btn_w = w / 10;
	int btn_h = h / 10;

	int delete_x = w * 0.7;
	int delete_y = h * 0.25;

	int solve_x = w * 0.9;
	int solve_y = delete_y;

	int reset_x = w * 0.05;
	int reset_y = h * 0.15;

	int notes_x = w * 0.78;
	int notes_y = delete_y;

	int mistakes_x = w * 0.73;
	int mistakes_y = w * 0.07;

	MoveWindow(delete_button, delete_x, delete_y, btn_w / 1.6, btn_h, true);
	MoveWindow(solve_button, solve_x, solve_y, btn_w / 1.6, btn_h, true);
	MoveWindow(reset_button, reset_x, reset_y, btn_w, btn_h, true);
	MoveWindow(notes_button, notes_x, notes_y, btn_w, btn_h, true);
	MoveWindow(mistakes_text, mistakes_x, mistakes_y, btn_w * 2.1, btn_h * 0.4, true);

	SendMessage(delete_button, WM_SETFONT, (WPARAM)scaled_font, true);
	SendMessage(solve_button, WM_SETFONT, (WPARAM)scaled_font, true);
	SendMessage(reset_button, WM_SETFONT, (WPARAM)scaled_font, true);
	SendMessage(notes_button, WM_SETFONT, (WPARAM)scaled_font, true);
	SendMessage(mistakes_text, WM_SETFONT, (WPARAM)scaled_font, true);
}
//mijenja velicinu, poziciju i font sudoku polja ovisno o velicini prozora
void main_window::scale_sudoku_rectangles(int w, int h, int cell_size)
{
	if (button_font)
		DeleteObject(button_font);
	if (notes_font)
		DeleteObject(notes_font);

	int font_size = cell_size * 0.5;
	button_font = CreateFont(-font_size, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, TEXT("Arial"));
	notes_font = CreateFont(-font_size * 0.35, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, TEXT("Arial"));

	int x_start = (w - cell_size) / 4.45;
	int y_start = (h - cell_size) / 8.7;

	for (int i = 0; i < sudoku_buttons.size(); ++i)
	{
		int row = i / 9;
		int col = i % 9;

		int row_space = 0;
		if (row >= 3) row_space += 3;
		if (row >= 6) row_space += 3;
		int col_space = 0;
		if (col >= 3) col_space += 3;
		if (col >= 6) col_space += 3;

		int x = x_start + col * cell_size + col_space;
		int y = y_start + row * cell_size + row_space;

		RECT r;
		r.left = x;
		r.top = y;
		r.right = x + cell_size;
		r.bottom = y + cell_size;
		sudoku_buttons[i].setRect(r);
	}
	InvalidateRect(hw, 0, false);
}

int main_window::on_create(CREATESTRUCT* pcs)
{
	ShowWindow(hw, SW_MAXIMIZE);

	e_button.create(*this, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, load_text(IDS_BTN_EASY).c_str(), IDS_BTN_EASY);
	n_button.create(*this, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, load_text(IDS_BTN_NORMAL).c_str(), IDS_BTN_NORMAL);
	h_button.create(*this, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, load_text(IDS_BTN_HARD).c_str(), IDS_BTN_HARD);

	return 0;
}

void main_window::on_paint(HDC hdc)
{	
	if (game.getRect_Drawn())
	{
		//crtanje okvira i ruba sudoku polja
		RECT r;
		GetClientRect(*this, &r);
		RECT topLeft = sudoku_buttons[0].getRect();
		RECT bottomRight = sudoku_buttons[80].getRect();

		RECT border;
		border.left = topLeft.left;
		border.top = topLeft.top;
		border.right = bottomRight.right;
		border.bottom = bottomRight.bottom;

		HPEN pen = CreatePen(PS_SOLID, 4, RGB(0, 0, 0));
		HPEN oldPen = (HPEN)SelectObject(hdc, pen);

		int y_grid = (border.bottom - border.top) / 3;
		int x_grid = (border.left - border.right) / 3;

		MoveToEx(hdc, border.left, border.top + y_grid, nullptr);
		LineTo(hdc, border.right, border.top + y_grid);
		MoveToEx(hdc, border.left, border.top + y_grid * 2, nullptr);
		LineTo(hdc, border.right, border.top + y_grid * 2);

		MoveToEx(hdc, border.left - 2, border.top - 2, nullptr);
		LineTo(hdc, border.right + 2, border.top - 2);
		MoveToEx(hdc, border.right + 2, border.bottom + 2, nullptr);
		LineTo(hdc, border.left - 2, border.bottom + 2);

		MoveToEx(hdc, border.right + x_grid, border.top, nullptr);
		LineTo(hdc, border.right + x_grid, border.bottom);
		MoveToEx(hdc, border.right + x_grid * 2, border.top, nullptr);
		LineTo(hdc, border.right + x_grid * 2, border.bottom);

		MoveToEx(hdc, border.right + 2, border.top - 2, nullptr);
		LineTo(hdc, border.right + 2, border.bottom + 2);
		MoveToEx(hdc, border.left - 2, border.bottom + 2, nullptr);
		LineTo(hdc, border.left - 2, border.top - 2);

		SelectObject(hdc, oldPen);
		DeleteObject(pen);
	}

	paint_sudoku_buttons(hdc);
}

void main_window::on_command(int id)
{
	if (id >= IDC_BTN_NUMBER && id <= IDC_BTN_NUMBER + 8)
		number_button_clicked(id);
	
	game.setNumber_ID(- 1);
	if (id == IDS_BTN_DELETE)
		delete_button_clicked();
	
	if (id == IDS_BTN_SOLVE)
		solve_button_clicked();
	//ako je kliknut gumb "new game", salje se poruka za potvrdu i ako je odabrano "yes" igra se resetira
	if (id == IDS_BTN_RESET)
	{
		if (MessageBox(hw, load_text(IDS_NEW_GAME_MESSAGE).c_str(), load_text(IDS_NEW_GAME_TITLE).c_str(),
			MB_YESNO | MB_ICONINFORMATION) == IDYES)
			reset_game();
	}
	//ako je kliknut gumb "notes", ukljucuje ili iskljucuje mogucnost unosa biljeski
	if (id == IDS_BTN_NOTES_OFF)
	{
		game.setNotes_ON(!game.getNotes_ON());

		SetWindowText(GetDlgItem(hw, IDS_BTN_NOTES_OFF), load_text(game.getNotes_ON() ?
			IDS_BTN_NOTES_ON : IDS_BTN_NOTES_OFF).c_str());
	}
	if (id == IDS_BTN_EASY)
		difficulty_button_clicked(0);

	if (id == IDS_BTN_NORMAL)
		difficulty_button_clicked(1);

	if (id == IDS_BTN_HARD)
		difficulty_button_clicked(2);
}

//ako je kliknuto sudoku polje, oznacava se i isticu se druga polja sa istim brojevima
void main_window::on_left_button_down(POINT p)
{
	for (auto& b : sudoku_buttons)
	{
		b.setNumberHighlighted(false);
		b.setClickedButton(false);
	}

	for (int i = 0; i < sudoku_buttons.size(); i++)
	{
		if (sudoku_buttons[i].contains(p))
		{
			game.setSudoku_ID(i);
			sudoku_buttons[i].setClickedButton(true);
			for (auto& b : sudoku_buttons)
			{
				if (!sudoku_buttons[i].getText().empty() && b.getText() == sudoku_buttons[i].getText())
					b.setNumberHighlighted(true);
			}
			InvalidateRect(hw, 0, false);
			break;
		}
	}
}

//postavljanje koordinata i velicina gumba, te fontova ovisno o velicini prozora 
void main_window::on_size(int w, int h)
{
	if (scaled_font)
		DeleteObject(scaled_font);

	HDC dc = GetDC(hw);
	int font_h = -MulDiv(h / 25, GetDeviceCaps(dc, LOGPIXELSY), 72) / 1.5;
	ReleaseDC(hw, dc);

	scaled_font = CreateFont(font_h, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"));

	scale_buttons(w, h, scaled_font);

	int cell_w = w / 22;
	int cell_h = h / 11;
	int cell_size = min(cell_w, cell_h);

	int num_x = (w - cell_size) / 1.25;
	int num_y = (h - cell_size) / 2;

	for (int i = 0; i < number_buttons.size(); i++)
	{
		int row = i / 3;
		int col = i % 3;
		int x = num_x + col * cell_size;
		int y = num_y + row * cell_size;

		HWND button = GetDlgItem(hw, number_buttons[i].getId());
		MoveWindow(button, x, y, cell_size, cell_size, true);
	}

	scale_sudoku_rectangles(w, h, cell_size);
}

void main_window::on_destroy()
{
	if (scaled_font)
		DeleteObject(scaled_font);
	::PostQuitMessage(0);
}

int WINAPI WinMain(HINSTANCE hi, HINSTANCE, LPSTR, int)
{
	application app;
	main_window wnd;
	wnd.create(0, WS_OVERLAPPEDWINDOW | WS_VISIBLE, window::load_text(IDS_APP_TITLE).c_str());
	return app.run();
}
