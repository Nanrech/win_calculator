# win_cal
![image](readme.png)

### What is this?
A stupid simple calculator based off Windows 10's own default calculator.
This program uses only C and windows.h for its GUI.

I have not tried this on anything other than my machine (Windows 10), but it should hopefully work.

### How does this work?
Read through main.c if you want to learn how the code works! It's all full of comments and I feel like I did a decent job at structuring/documenting the program.

As for the user interface, it behaves similarly to Windows 10's default calculator. It supports both keyboard and mouse input.

Resizing the window is NOT supported. Creating the layout and fitting everything together was a pain (Windows' api provides no utilities for this).

### How to build it yourself
I just used this:

`gcc main.c -std=c99 -mwindows -o win_cal`

### Final remarks
- Inspiration https://mitxela.com/projects/windows_calculator
- Symbols in macros.h https://justine.lol/dox/cp437.txt
- This is pretty much the first time I use the win32 api (like this). Expect nooby coding practices (still wrapping my head around wide characters..).
- The code was originally just in one file (main.c), I decided to split it like this so it would be more legible.
- Don't try this at home.
