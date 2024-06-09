# win_cal
![image](readme.png)

### What is this?
A stupid simple calculator based off Windows 10's own default calculator.
This program uses only C and windows.h for its GUI.

I have not tried this on anything other than my machine (Windows 10), but it should hopefully work.

### How does this work?
Read through main.c if you want to learn how the code works! It's all full of comments and I feel like I did a decent job at structuring/documenting the program. Do note that I moved WinMain to the bottom of the file, since event handling is the most important part of this project.

(Admittedly, main.c looks a bit bloated. But that's mostly because of all the comments and repetitive statements.)

As for the user interface, it behaves similarly to Windows 10's default calculator. It supports both keyboard and mouse (clicking buttons) inputs.

Resizing the window is NOT supported. Creating the layout and fitting everything together was a pain (Windows' api provides no utilities for this). I know resizing could be implemented fairly easily (through WM_SIZE and a bunch of easy math) but I don't think I want to go through that right now.

### How to build it yourself
I just used this throughout development.

Using C99 is a personal choice, `-mwindows` is so the executable doesn't generate a cmd window when it's ran.

GCC v. 13.1.0 (MinGW).
`gcc main.c -std=c99 -mwindows -o win_cal`

### Final remarks
- Inspiration https://mitxela.com/projects/windows_calculator
- Symbols in macros.h https://justine.lol/dox/cp437.txt
- This is pretty much the first time I use the win32 api (like this). Expect nooby coding practices (still wrapping my head around wide characters..).
- The code was originally just in one file (main.c), I only decided to split it like this so it would be more legible.
- The icon is 16x16 and it was made by me using pixilart.com and an online converting (don't remember which).
- Don't try this at home.
