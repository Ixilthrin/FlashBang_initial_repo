# FlashBang
I created this repository a while back as just a name because I thought 
FlashBang was a great name for a graphics engine + sound project. 

It's the 4th of July so I am going to officially kick off this project today 
with code I've been working on using GLFW, OpenGL 4.5, ...

This repository contains only my custom code for the project.  To build it you 
need to get the glad library, the glfw library (version 3.3), and the glm 
library (version 0.9.9.6).

You need to include glad.c in your project.
You need to include stb_image.h in your project for png loading. 
See github project https://github.com/nothings/stb

This program is composed of multiple sub-programs, each progressively exploring 
OpenGL.  For example, program number 1 simply displays a triangle.
See the main function in main.cpp that has a hard-coded switch for the current 
program.  Change the program number to run the one you want.

Note that initially each program was self-contained.  I wanted to keep most of
the drawing code in a single function so I could conceptually see everything
that is needed to get a simple OpenGL 4.5 app working.  Eventually the rendering
code will be refactored into small individual components.  The refactoring will
be applied to current programs only.  The older programs will not be touched in order
to be able to show the general progression of code as the projects become more
and more complex.

For this project I developed a simple eventing model that takes the raw input 
events and translates them to something more intuitive.  For example a left click 
becomes a selection event  I also created a converter class that converts from screen
space to OpenGL NDC (normalized device coordinates).  This program also loads PNG 
files and uses texture mapping to display them over simple rectangles. 

I am currently working on program 10 that will serve as the prototype for my
"deck of cards" program.  This is analogous to having a deck of index cards that
you write up and then organize on a table to design something or explain something.
I have it working so you can create a PNG file and then save it into the designated 
directory.  When you run the card deck app it will automatically load in all the PNG 
files from that designated directory.

Most recently added the flip side to cards.  If you want a card to have a flip side,
create a png file with the same name appended with "-back".  For example, if you have
a card called mycard.png, the flip side will be mycard-back.png.  To flip a card
perform a right-click on it.

The latest enhancements to the card deck program (program #10) is support for variable-
sized cards (up to a maximum width of 500) and shuffling. So when you save or copy an
image into the card deck directory the cards will be the size of their images (as 
mentioned the max width is currently 500.  The height will remain proportional to
the width.  Also added translucency for pure white (R=1.0, G=1.0, B=1.0).  This works 
well with paint programs that do not support an alpha channel and default to a pure 
white background for images.

