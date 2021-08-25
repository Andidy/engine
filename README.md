# engine
This is my game engine hobby project. It's going to be the basis of experiments in game development.

## Engine Design
The goal is to be able to be cross-platform by creating an abstraction layer for the game to exist on above the OS and Rendering framework.
This way the OS code and the rendering code could potentially be swapped out, ex Win32 -> Linux or DX11 -> DX12 or Vulkan or Software Rendering, without having to change any of the game code.

How I hope to achieve this is with 4 main "modules": the game, the renderer, the platform, and a "universal" module.
The platform is at the bottom of the stack as its how the game will interact with most of the computer and is the largest dependency in some sense.
The renderer is on top of the platform because it is often heavily interwined with the platform as it needs to make calls to the hardware or platform and is the second largest dependency.

The game is on top of both and should be as isolated as possible from the lower layers only making calls into them through specific functions.
Which is where the universal module comes to help out. The universal module has structures and functions that any of the modules may wish to call,
but are often dependant on the code from a lower layer. For instance, the Input structure and its internal KeyboardState and MouseState are highly dependant on platform code as to how they are filled out,
but they provide a simple interface through the associated "keyXXXX" functions for the rendering code (perhaps for debugging purposes)
and gameplay code (the primary user) to get keyboard and mouse input with.

Currently the project is focused on features and developed on Win32 and DirectX11.

## NOTE: 
OBS WINDOW CAPTURES OF THE ENGINE SEEMS TO SET THE CAPTURE MODE TO WINDOWS 7 BITBLT MODE IF THE CAPTURE MODE IS SET TO AUTOMATIC, IF YOU'RE CAPTURING THIS WAY CHANGE CAPTURE MODE TO WINDOWS GRAPHICS CAPTURE (WINDOWS 10 1903 and Later)
