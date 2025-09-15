#pragma once  
#include "Resource.h"
#include "Number_generator.h"  
#include <windows.h>  
#include <string>  
#include <tchar.h>
#include <list>

using tstring = std::basic_string<TCHAR>;

class application
{
public:
    int run();
};

/*window klasa je bazna klasa aplikacije: handlea izradu prozora(koristeci create), koristenje HWND membera i 
funkcije procedure prozora*/
class window
{
protected:
    HWND hw{ 0 };
    virtual tstring class_name();
    bool register_class(const tstring& name);
    tstring generate_class_name();

public:
    bool create(HWND parent, DWORD style, LPCTSTR caption = 0, UINT_PTR id_or_menu = 0, int x = CW_USEDEFAULT,
        int y = CW_USEDEFAULT, int width = CW_USEDEFAULT, int height = CW_USEDEFAULT);

    operator HWND();
    static LRESULT CALLBACK proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    static std::basic_string<TCHAR> load_text(int id);

protected:
    virtual int on_create(CREATESTRUCT*) { return 0; }
    virtual void on_command(int) {}
    virtual void on_destroy() {}
    virtual void on_size(int w, int h) {}
    virtual void on_paint(HDC hdc) {}
    virtual void on_left_button_down(POINT) {}
};

class button : public window
{
    std::string class_name() override { return "button"; }
};

class number_button : public button
{
private:
    int id;
    std::string text;
public: 
    number_button(HWND hw, int ID, const std::string& txt): id(ID), text(txt)
    {
        create(hw, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, txt.c_str(), ID);
    }
    std::string getText() const { return text; }
    int getId() const { return id; }
};

class sudoku_button 
{
private:
    bool entered_number = false;
    bool clicked_button = false;
    bool number_highlighted = false;
    int id;
    std::string text;
    std::vector<std::string> note_numbers;
    RECT rect;
public:
    std::string getText() const { return text; }
    bool getEnteredNumber() const { return entered_number; }
    bool getClickedButton() const { return clicked_button; }
    bool getNumberHighlighted() const { return number_highlighted; }
    int getId() const { return id; }
    std::vector<std::string> getNoteNumbers() const { return note_numbers; }
    RECT getRect() const { return rect; }

    void setText(std::string txt) { text = txt; }
    void setEnteredNumber(bool en) { entered_number = en; }
    void setClickedButton(bool cb) { clicked_button = cb; }
    void setNumberHighlighted(bool nh) { number_highlighted = nh; }
    void setNoteNumbers(std::vector<std::string> nn) { note_numbers = nn; }
    void setRect(RECT r) { rect = r; }

    bool contains(POINT p) { return PtInRect(&rect, p); }

    sudoku_button(int ID, const std::string& txt) : id(ID), text(txt)
    {
        note_numbers = std::vector<std::string>(9, "");
    }
};

/*sudoku_game klasa je bazna klasa "Sudoku" igre: sadrzi sve vazne varijable vezane uz pravila i stanje igre,
te njihove gettere i settere*/
class sudoku_game
{
private:
    std::vector<std::vector<int>> solved_sudoku;
    int selected_sudoku_id = -1;
    int selected_number_id = -1;
    int mistakes = 0;
    bool notes_on = false;
    bool rect_drawn = false;
public:
    std::vector<std::vector<int>> create_solved_sudoku();
    void remove_numbers(std::vector<std::vector<int>>& grid, int k);
    int create_mode(int mode);

    std::vector<std::vector<int>> getSolvedSudoku() const { return solved_sudoku; }
    int getSudoku_ID() const { return selected_sudoku_id; }
    int getNumber_ID() const { return selected_number_id; }
    int getMistakes() const { return mistakes; }
    bool getNotes_ON() const { return notes_on; }
    bool getRect_Drawn() const { return rect_drawn; }

    void setSolvedSudoku(std::vector<std::vector<int>> ss) { solved_sudoku = ss; }
    void setSudoku_ID(int ss_id) { selected_sudoku_id = ss_id; }
    void setNumber_ID(int sn_id) { selected_number_id = sn_id; }
    void setMistakes(int m) { mistakes = m; }
    void setNotes_ON(bool n) { notes_on = n; }
    void setRect_Drawn(bool rd) { rect_drawn = rd; }
};


