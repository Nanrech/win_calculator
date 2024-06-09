#include <windows.h>

#pragma once


struct calc_button_handles {
  // Numbers
  HWND k_0;
  HWND k_1;
  HWND k_2;
  HWND k_3;
  HWND k_4;
  HWND k_5;
  HWND k_6;
  HWND k_7;
  HWND k_8;
  HWND k_9;
  // Operators
  HWND k_mul; // *
  HWND k_div; // /
  HWND k_sum; // +
  HWND k_min; // -
  HWND k_dot; // .
  // The special ones
  HWND k_inv; // invert input
  HWND k_del; // delete
  HWND k_clr; // escape
  HWND k_equ; // enter
};

// Calculator state
struct calc_state {
  struct calc_button_handles button_handles;
  BOOL has_dot;
  char op;
  char last_op;
  int in_len;
  double last_in;
};
