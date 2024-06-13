#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <stdio.h>
#include <math.h>       // floor.h

#include "constants.h"  // INP_CONSTANTS, UI_CONSTANTS, CAL_CONSTANTS
#include "macros.h"     // CREATEBUTTON, REDIRECT_TO_CLICK
#include "state.h"      // calculator_state


/*
  This, `c_state`, is the calculator state.
  I am not too fond of global variables but it's either this or
  - winapi's terrible pointer method
  - keeping a static variable inside WindowProc
  So I'm keeping it like this. Nice and simple.
*/

struct calc_state g_calc_state = {};


/*
  WindowProc is where you handle all the window's events. (basically the entire program).
  Admittedly, I probs. should've separated these events into their own functions, but
  this is a really simple program so it doesn't really matter too much.

  In short:
  - WM_CREATE
      This is where everything inside the window is created and initialized.
      "Everything" being the 2 text buffers and all the buttons.
  - WM_CHAR
      This event captures all character inputs from the keyboard.
      I use this event to capture most key inputs.
  - WM_KEYDOWN
      In this event, the keyboard inputs that WM_CHAR ignores are captured.
      These are DELETE, ESCAPE and ENTER.
      For both of these events, whenever a key/char is detected, the
      input is captured and redirected as a button press.
      (e.g. typing '0' --> clicking the '0' button on the UI)
  - WM_COMMAND
      Every button input (and redirected keyboard input) is processed here.
      These inputs are:
      - Numbers
      - Decimal dot/period
      - Operators
      - Commands
      Look inside this block to see how each case is handled.
  - WM_CLOSE
      Default Windows event. Calls DestroyWindow.
  - WM_DESTROY
      Default Windows event. Calls PostQuitMessage.
  - default
      Every other event is handled by the default Windows event handler.
*/

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  switch (uMsg) {
    case WM_CREATE: {
      /*
        Don't let this one scare you away. It is ugly, but it must be done.
      */

      // Honestly no idea what this thing is, but it is needed
      const HINSTANCE window_hInstance = (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE);

      /*
        Buffer screen
      */
      CreateWindow(
        L"EDIT",
        NULL,
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_READONLY,
        0, 0,
        CAL_T_WIDTH, CAL_T_HEIGHT / 2,
        hwnd,
        (HMENU)ID_CAL_PREV,
        window_hInstance,
        NULL
      );

      /*
        Input screen
      */
      CreateWindow(
        L"EDIT",
        NULL,
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_READONLY,
        0, 25,
        CAL_T_WIDTH, CAL_T_HEIGHT / 2,
        hwnd,
        (HMENU)ID_CAL_INP,
        window_hInstance,
        NULL
      );

      // Set initial calculator state
      SetDlgItemTextW(hwnd, ID_CAL_PREV, L">\0");
      SetDlgItemTextW(hwnd, ID_CAL_INP, L"0\0");

      g_calc_state.has_dot = FALSE;
      g_calc_state.op = '_';
      g_calc_state.last_op = '_';
      g_calc_state.in_len = 0;
      g_calc_state.last_in = NAN;

      /*
        Buttons.
        Check macros.h to learn how I create this layout.
      */
      g_calc_state.button_handles.k_7   = CREATE_BUTTON(L"7",   0, 0, ID_7);
      g_calc_state.button_handles.k_8   = CREATE_BUTTON(L"8",   1, 0, ID_8);
      g_calc_state.button_handles.k_9   = CREATE_BUTTON(L"9",   2, 0, ID_9);
      g_calc_state.button_handles.k_del = CREATE_BUTTON(L"DEL", 3, 0, ID_DEL);
      g_calc_state.button_handles.k_clr = CREATE_BUTTON(L"AC",  4, 0, ID_AC);

      g_calc_state.button_handles.k_4   = CREATE_BUTTON(L"4",   0, 1, ID_4);
      g_calc_state.button_handles.k_5   = CREATE_BUTTON(L"5",   1, 1, ID_5);
      g_calc_state.button_handles.k_6   = CREATE_BUTTON(L"6",   2, 1, ID_6);
      g_calc_state.button_handles.k_mul = CREATE_BUTTON(L"x",   3, 1, ID_MUL);
      g_calc_state.button_handles.k_div = CREATE_BUTTON(L"/",   4, 1, ID_DIV);

      g_calc_state.button_handles.k_1   = CREATE_BUTTON(L"1",   0, 2, ID_1);
      g_calc_state.button_handles.k_2   = CREATE_BUTTON(L"2",   1, 2, ID_2);
      g_calc_state.button_handles.k_3   = CREATE_BUTTON(L"3",   2, 2, ID_3);
      g_calc_state.button_handles.k_sum = CREATE_BUTTON(L"+",   3, 2, ID_SUM);
      g_calc_state.button_handles.k_min = CREATE_BUTTON(L"-",   4, 2, ID_MIN);

      // 0 is double wide
      g_calc_state.button_handles.k_0   = CreateWindow(
        L"BUTTON", L"0", WS_VISIBLE | WS_CHILD | BS_CENTER | BS_PUSHBUTTON,
        BUTTON_WIDTH * 0, CAL_T_HEIGHT + BUTTON_HEIGHT * 3,
        BUTTON_WIDTH * 2, BUTTON_HEIGHT,
        hwnd, (HMENU)ID_0, window_hInstance, NULL
      );
      g_calc_state.button_handles.k_inv = CREATE_BUTTON(L"+/-", 2, 3, ID_INV);
      g_calc_state.button_handles.k_dot = CREATE_BUTTON(L".",   3, 3, ID_DOT);
      g_calc_state.button_handles.k_equ = CREATE_BUTTON(L"=",   4, 3, ID_EQ);

      return 0;
    }

    case WM_CHAR: {
      /*
        Whenever app receives keyboard input for ANYTHING except
        - DELETE
        - ESCAPE
        - ENTER
        that input is re-routed to WM_COMMAND from here.
        This event catches stuff like * and /, so I don't have to
        manually check for stuff like 'shift' + '7' = '/'.

        Very useful.

        See macros.h to see what REDIRECT_TO_CLICK actually looks like.
      */

      switch (wParam) {
        case '0': {
          REDIRECT_TO_CLICK(g_calc_state.button_handles.k_0);
          return 0;
        }
        case '1': {
          REDIRECT_TO_CLICK(g_calc_state.button_handles.k_1);
          return 0;
        }
        case '2': {
          REDIRECT_TO_CLICK(g_calc_state.button_handles.k_2);
          return 0;
        }
        case '3': {
          REDIRECT_TO_CLICK(g_calc_state.button_handles.k_3);
          return 0;
        }
        case '4': {
          REDIRECT_TO_CLICK(g_calc_state.button_handles.k_4);
          return 0;
        }
        case '5': {
          REDIRECT_TO_CLICK(g_calc_state.button_handles.k_5);
          return 0;
        }
        case '6': {
          REDIRECT_TO_CLICK(g_calc_state.button_handles.k_6);
          return 0;
        }
        case '7': {
          REDIRECT_TO_CLICK(g_calc_state.button_handles.k_7);
          return 0;
        }
        case '8': {
          REDIRECT_TO_CLICK(g_calc_state.button_handles.k_8);
          return 0;
        }
        case '9': {
          REDIRECT_TO_CLICK(g_calc_state.button_handles.k_9);
          return 0;
        }
        case '*': {
          REDIRECT_TO_CLICK(g_calc_state.button_handles.k_mul);
          return 0;
        }
        case '/': {
          REDIRECT_TO_CLICK(g_calc_state.button_handles.k_div);
          return 0;
        }
        case '+': {
          REDIRECT_TO_CLICK(g_calc_state.button_handles.k_sum);
          return 0;
        }
        case '-': {
          REDIRECT_TO_CLICK(g_calc_state.button_handles.k_min);
          return 0;
        }
        case '.': {
          REDIRECT_TO_CLICK(g_calc_state.button_handles.k_dot);
          return 0;
        }
        case '=': {
          REDIRECT_TO_CLICK(g_calc_state.button_handles.k_equ);
          return 0;
        }
      }
      return 0;
    }

    case WM_KEYDOWN: {
      /*
        These three are easier to capture here than in WM_CHAR.
      */

      if (wParam == VK_BACK) {
        REDIRECT_TO_CLICK(g_calc_state.button_handles.k_del);
      }
      else if (wParam == VK_ESCAPE) {
        REDIRECT_TO_CLICK(g_calc_state.button_handles.k_clr);
      }
      else if (wParam == VK_RETURN) {
        REDIRECT_TO_CLICK(g_calc_state.button_handles.k_equ);
      }

      return 0;
    }

    case WM_COMMAND: {
      /*
        Deal with each type of input.
        - Numbers
        - Decimal dot/period
        - Operators
        - Commands
      */

      // Ignore all other commands
      // (low word contains the id)
      if (LOWORD(wParam) < ID_0 || LOWORD(wParam) > ID_EQ) {
        return 0;
      }

      // Set focus back to the parent window (away from the button)
      // so keyboard inputs keep working.
      // Also removes bad-looking border from buttons. Win-win.
      SetFocus(hwnd);

      // Load input text
      wchar_t in_str[CAL_T_LEN + 2];
      GetDlgItemTextW(hwnd, ID_CAL_INP, in_str, CAL_T_LEN + 2);
      // Load buffer text
      wchar_t prev_str[CAL_T_LEN + 2];
      GetDlgItemTextW(hwnd, ID_CAL_PREV, prev_str, CAL_T_LEN + 2);

      /*
        Process input.
      */

      // Numbers
      if (wParam >= ID_0 && wParam <= ID_9) {
        // Length cap
        if (g_calc_state.in_len > CAL_T_LEN) {
          return 0;
        }

        // Append new number to input buffer.
        in_str[g_calc_state.in_len] = '0' + (wParam - ID_0);
        in_str[++g_calc_state.in_len] = '\0';

        SetDlgItemTextW(hwnd, ID_CAL_INP, in_str);

        // Also invalidate previous calculator state.
        // Starting to type a new number = Starting a new operation.
        g_calc_state.last_op = '_';
        g_calc_state.last_in = NAN;

        return 0;
      }

      // Decimal dot/period
      if (wParam == ID_DOT) {
        // Length cap, only one dot in input at all times, dot cannot be first input
        if (g_calc_state.in_len > CAL_T_LEN || g_calc_state.has_dot || g_calc_state.in_len == 0) {
          return 0;
        }

        // Append dot to input buffer.
        in_str[g_calc_state.in_len] = '.';
        in_str[++g_calc_state.in_len] = '\0';

        SetDlgItemTextW(hwnd, ID_CAL_INP, in_str);

        g_calc_state.has_dot = TRUE;

        return 0;
      }

      // Operators
      if (wParam >= ID_MUL && wParam <= ID_MIN) {
        // If prev is empty, or we are receiving a new input:
        // Swap the buffers and reset input.
        if (prev_str[0] == '>' || !(in_str[0] == '0' && in_str[1] == '\000')) {
          SetDlgItemTextW(hwnd, ID_CAL_PREV, in_str);
          SetDlgItemTextW(hwnd, ID_CAL_INP, L"0\0");

          g_calc_state.has_dot = FALSE;
          g_calc_state.in_len = 0;
        }

        // Reset button states.
        SetDlgItemTextW(hwnd, ID_MUL, L"x");
        SetDlgItemTextW(hwnd, ID_DIV, L"/");
        SetDlgItemTextW(hwnd, ID_SUM, L"+");
        SetDlgItemTextW(hwnd, ID_MIN, L"-");

        // Set current op and highlight the corresponding button
        switch (wParam) {
          case ID_MUL: {
            g_calc_state.op = 'x';
            SetDlgItemTextW(hwnd, ID_MUL, L"[ x ]");
            return 0;
          }
          case ID_DIV: {
            g_calc_state.op = '/';
            SetDlgItemTextW(hwnd, ID_DIV, L"[ / ]");
            return 0;
          }
          case ID_SUM: {
            g_calc_state.op = '+';
            SetDlgItemTextW(hwnd, ID_SUM, L"[ + ]");
            return 0;
          }
          case ID_MIN: {
            g_calc_state.op = '-';
            SetDlgItemTextW(hwnd, ID_MIN, L"[ - ]");
            return 0;
          }
        }

        return 0;
      }

      // Commands
      if (wParam >= ID_DEL && wParam <= ID_EQ) {
        /*
          - Delete
          - All clear
          - Invert input
          - Execute
        */

        switch (wParam) {
          // Delete
          case ID_DEL: {
            // Length cap
            if (g_calc_state.in_len <= 0) {
              return 0;
            }

            // If we deleted the decimal dot
            if (in_str[--g_calc_state.in_len] == '.') {
              g_calc_state.has_dot = FALSE;
            }

            // Remove last character
            in_str[g_calc_state.in_len] = '\0';

            SetDlgItemTextW(hwnd, ID_CAL_INP, in_str);

            return 0;
          }

          // All clear
          case ID_AC: {
            // Reset text buffers and all button states
            SetDlgItemTextW(hwnd, ID_CAL_PREV, L">\0");
            SetDlgItemTextW(hwnd, ID_CAL_INP, L"0\0");
            SetDlgItemTextW(hwnd, ID_MUL, L"x");
            SetDlgItemTextW(hwnd, ID_DIV, L"/");
            SetDlgItemTextW(hwnd, ID_SUM, L"+");
            SetDlgItemTextW(hwnd, ID_MIN, L"-");

            // Reset calculator state
            g_calc_state.has_dot = FALSE;
            g_calc_state.op = '_';
            g_calc_state.last_op = '_';
            g_calc_state.in_len = 0;
            g_calc_state.last_in = NAN;

            return 0;
          }

          // Invert input
          case ID_INV: {
            int num_new_chars = 0;

            double in_number = _wtof(in_str);
            in_number *= -1;

            // Check if we have to render decimals
            if (floor(in_number) == in_number) {
              num_new_chars = swprintf(in_str, CAL_T_LEN, L"%.0lf", in_number);
            }
            else {
              num_new_chars = swprintf(in_str, CAL_T_LEN, L"%.4lf", in_number);
            }

            // If the resulting number is greater than CALC_T_LEN - 1
            // (swprintf returns total characters except \0)
            // The number is too long and we are going to overflow
            // (though it seems the result overflows before this can happen)
            if (num_new_chars > CAL_T_LEN - 1) {
                MessageBox(hwnd, L"Result has overflown.", L"Error", MB_OK | MB_ICONERROR);
                // vvv (This is equivalent to pressing the reset button)
                SendMessage(hwnd, WM_COMMAND, ID_AC, 0);
                return 0;
            }

            // If the number is now negative, the string is now one char longer
            // (n * -1) < 0 | n > 0
            if (in_number < 0) {
              g_calc_state.in_len += 1;
            }
            // If the number is now positive, the string is now one char shorter
            // (n * -1) > 0 | n < 0
            else {
              g_calc_state.in_len -= 1;
            }
            // If it's zero, nothing changed
            // (n * -1) = 0 | n = 0

            // Set new text
            SetDlgItemTextW(hwnd, ID_CAL_INP, in_str);

            return 0;
          }

          // Execute
          case ID_EQ: {
            // If the last character in input is the dot, reject.
            if (in_str[g_calc_state.in_len - 1] == '.' || prev_str[0] == '>') {
              return 0;
            }

            // Reset button state
            SetDlgItemTextW(hwnd, ID_MUL, L"x");
            SetDlgItemTextW(hwnd, ID_DIV, L"/");
            SetDlgItemTextW(hwnd, ID_SUM, L"+");
            SetDlgItemTextW(hwnd, ID_MIN, L"-");

            // If there is no operator selected, swap buffers and reset input.
            if (g_calc_state.op == '_' && g_calc_state.last_op == '_') {
              SetDlgItemTextW(hwnd, ID_CAL_PREV, in_str);
              SetDlgItemTextW(hwnd, ID_CAL_INP, L"0\0");

              g_calc_state.has_dot = FALSE;
              g_calc_state.in_len = 0;

              return 0;
            }

            // If we have to execute an operation
            if ((g_calc_state.op != '_') || (g_calc_state.last_op != '_' && g_calc_state.last_in != NAN)) {
              // These are the variables we're working with
              int num_new_chars = 0;
              double prev_number = _wtof(prev_str);
              char op = '_';
              double in_number = 0;
              double result = 0;

              // And this one is just so I don't have to do the same check a bunch of times
              BOOL is_new_operation = g_calc_state.op != '_';

              // If it's a new operation:
              if (is_new_operation) {
                // Set the variables.
                op = g_calc_state.op;
                in_number = _wtof(in_str); // Get input buffer as a number

                // Clear input buffer.
                SetDlgItemTextW(hwnd, ID_CAL_INP, L"0\0");

                g_calc_state.has_dot = FALSE;
                g_calc_state.in_len = 0;
              }

              // If we're repeating the last operation:
              else {
                // Set the variables.
                op = g_calc_state.last_op;
                in_number = g_calc_state.last_in;

                // We do nothing to the calculator state.
              }

              // Assert we're in a valid state. Otherwise reset.
              if (op == '_' || in_number == NAN) {
                MessageBox(hwnd, L"Invalid state reached. Aborting.\nTried to compute with null operator or NAN operand.", L"Error", MB_OK | MB_ICONERROR);
                // vvv (This is equivalent to pressing the reset button)
                SendMessage(hwnd, WM_COMMAND, ID_AC, 0);
                return 0;
              }

              switch (op) {
                case 'x': {
                  result = prev_number * in_number;
                  break;
                }
                case '+': {
                  result = prev_number + in_number;
                  break;
                }
                case '-': {
                  result = prev_number - in_number;
                  break;
                }
                case '/': {
                  if (in_number == 0) {
                    // Division by zero causes the whole calculator to reset.
                    MessageBox(hwnd, L"Attempted division by zero. Resetting.", L"Error!", MB_OK | MB_ICONEXCLAMATION);
                    SendMessage(hwnd, WM_COMMAND, ID_AC, 0);

                    return 0;
                  }
                  result = prev_number / in_number;
                  break;
                }
              }

              // Set buffer text to the result and reset current operator.
              // Also check if result has any decimals.
              if (floor(result) == result) {
                num_new_chars = swprintf(prev_str, CAL_T_LEN, L"%.0lf", result);
              }
              else {
                num_new_chars = swprintf(prev_str, CAL_T_LEN, L"%.4lf", result);
              }

              // If the resulting number is greater than CALC_T_LEN - 1
              // (swprintf returns total characters except \0)
              // The number is too long and we are going to overflow
              if (num_new_chars > CAL_T_LEN - 1) {
                  MessageBox(hwnd, L"Result has overflown.", L"Error", MB_OK | MB_ICONERROR);
                  // vvv (This is equivalent to pressing the reset button)
                  SendMessage(hwnd, WM_COMMAND, ID_AC, 0);
                  return 0;
              }

              SetDlgItemTextW(hwnd, ID_CAL_PREV, prev_str);

              // If we just executed in a new operation:
              if (is_new_operation) {
                // Set 'latest' state in case user presses execute again
                g_calc_state.last_op = g_calc_state.op;
                g_calc_state.last_in = in_number;

                // Reset op
                g_calc_state.op = '_';
              }

              return 0;
            }

            return 0;
          }
        }
      }

      return 0;
    }

    case WM_CLOSE: {
      DestroyWindow(hwnd);
      return 0;
    }

    case WM_DESTROY: {
      PostQuitMessage(0);
      return 0;
    }

    default:
      return DefWindowProc(hwnd, uMsg, wParam, lParam);
  }
}

/*
  WinMain is down here for aesthetic reasons.
  This is all mostly uninsteresting code copied straight from the docs.
*/

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
  // 1. Register the window class. Idk the naming rules/conventions.
  const wchar_t CLASS_NAME[]  = L"CalculatorWindowClassThing";

  WNDCLASS wc = {
    .lpfnWndProc = WindowProc,
    .hInstance = hInstance,
    .lpszClassName = CLASS_NAME,
    .hIcon = LoadImageA(hInstance, "../calc_icon.ico", IMAGE_ICON, 16, 16, LR_LOADFROMFILE)
  };

  RegisterClass(&wc);

  // 2. Create the window.
  HWND hwnd = CreateWindowEx(
    0,                                                        // Extended window styles
    CLASS_NAME,                                               // Window class
    L"Calculator",                                            // Window name
    WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, // Window style
    CW_USEDEFAULT, CW_USEDEFAULT,                             // X, Y
    381, 279,                                                 // Width, Height. Don't ask.
    NULL, NULL, hInstance, NULL                               // Parent window, menu, instance handle, etc
  );

  if (hwnd == NULL) {
    MessageBox(NULL, L"Failed to create window", L"Error", MB_OK | MB_ICONERROR);
    return 0;
  }

  ShowWindow(hwnd, nCmdShow);

  // 3. Run the message loop.
  MSG msg = { };

  while (GetMessage(&msg, NULL, 0, 0) > 0) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  DestroyWindow(hwnd);

  return 0;
}
