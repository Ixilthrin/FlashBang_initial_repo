# FlashBang
I started this project as a name because I thought FlashBang was a great name 
for a graphics engine + sound project. 
It's the 4th of July so I am going to officially kick off this project today 
with code from a project I've been working on using GLFW, OpenGL 4.5, ...

This repository contains only my custom code for the project.  To build it you 
need to get the glad library, the glfw library (version 3.3), and the glm 
library (version 0.9.9.6).

You need to include glad.c in your project.
You need to include stb_image.h in your project for png loading. 
See github project https://github.com/nothings/stb

This program is composed of multiple sub-programs, each progressively exploring 
OpenGL.  For example, program number 1 simply display a triangle.
See the main function in main.cpp that has a hard-coded switch for the current 
program.  Change the program number to run the one you want.

I am currently working on program 9 that runs the class BasicMouseSelection.  
I am using this program to build out a more complex app where you can select and
move objects around the screen.  For this program I developed a simple eventing 
model that takes the raw input events and translates them to something more 
intuitive.  For example a left click becomes a selection event  I also created a 
converter class that converts from screen space to OpenGL NDC (normalized device 
coordinates).  This program also loads PNG files and uses texture mapping to display
them over simple rectangles.  The next step I am working on is to have multiple
cards on the screen, each displaying an image.  You will be able to move the cards
around and even stack them and be able to cycle through a stack using a z value for 
each card.
