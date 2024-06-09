/*

  This is where I hide my macros. They are ugly.

*/

#pragma once

/*
This macro creates a button. It's all wrapped in {} so I can use the return value (the button handle).
[name] --> "name" is a parameter.
<name> --> "name" is a constant.


This window:
- is a button
- contains the text [text]
- has a bunch of style flags (visible, child, centered text, push button)
- its x position is determined via [row]. See below.
- its y position is determined via [column]. See below.
- has a width and height defined in main.c
- is a child of the main window, has id = [id], not sure what window_hInstance is, etc.

Rows and columns.
To construct this app's GUI, I defined a sort of 4x5 "matrix".
Ignore the text buffers (their heights are treated as padding).

┌───────────────────┐
│ >                 │ < Buffer
├───────────────────┤
│ 0                 │ < Input
├───┬───┬───┬───┬───┤
│0,0│1,0│2,0│3,0│4,0│
├───┼───┼───┼───┼───│
│0,1│1,1│2,1│3,1│4,1│
├───┼───┼───┼───┼───│
│0,2│1,2│2,2│3,2│4,2│
├───┴───┼───┼───┼───│
│  0,3  │2,3│3,3│4,3│
└───────┴───┴───┴───┘

Each button's position is:
x = [row]    * <button_width>
y = [column] * <button_height> + <y_padding>

It's not a horrible system, considering win32 provides no layout utilities.
But the macro... it's really ugly....

*/
#define CREATE_BUTTON(text, row, column, id) ({\
  CreateWindow(L"BUTTON",  text, \
  WS_VISIBLE | WS_CHILD | BS_CENTER | BS_PUSHBUTTON, \
  BUTTON_WIDTH * row, \
  CAL_T_HEIGHT + BUTTON_HEIGHT * column, \
  BUTTON_WIDTH, BUTTON_HEIGHT, \
  hwnd, (HMENU)id, window_hInstance, NULL);\
})

// The Sleep(25) is just there so the buttons will actually sink down
// for a moment, as if they had been clicked.
#define REDIRECT_TO_CLICK(handle) \
        SendMessage(handle, BM_CLICK, 0, 0);\
        SendMessage(handle, BM_SETSTATE, TRUE, 0);\
        Sleep(25);\
        SendMessage(handle, BM_SETSTATE, FALSE, 0);
