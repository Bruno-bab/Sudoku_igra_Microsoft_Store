#pragma once

#include "Sudoku_igra.h"

class label : public window
{
	std::string class_name() override { return "static"; }
};
/*main_window klasa handlea korisnicko sucelje : vodi brigu o button-ima, static-ima i fontovima,
povezuje sudoku_game klasu sa suceljem i ostatkom aplikacije*/
class main_window : public window
{
public:
	LOGFONT lf;
	COLORREF cr;
	main_window();
protected:
	int on_create(CREATESTRUCT* pcs) override;
	void on_paint(HDC hdc) override;
	void on_command(int id) override;
	void on_size(int w, int h) override;
	void on_left_button_down(POINT) override;
	void on_destroy() override;
private:
	void game_start(int numbers_to_remove);
	void reset_game();
	void create_buttons();
	void paint_sudoku_buttons(HDC hdc);
	void input_number();
	void input_note();
	void game_won(int correct_counter);
	void number_button_clicked(int id);
	void delete_button_clicked();
	void solve_button_clicked();
	void difficulty_button_clicked(int difficulty);
	void scale_buttons(int w, int h, HFONT scaled_font);
	void scale_sudoku_rectangles(int w, int h, int cell_size);

	sudoku_game game;
	std::vector<sudoku_button> sudoku_buttons;
	std::vector<number_button> number_buttons;
	button delete_button, solve_button, reset_button, notes_button, e_button, n_button, h_button;
	label mistakes_text;
	HFONT button_font, notes_font, scaled_font;
};
