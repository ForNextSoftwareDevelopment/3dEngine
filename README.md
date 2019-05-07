# 3dEngine
Basic 3d engine with support for the HDK2 VR device

This engine has been build with Visual Studio 2017 with the use of the smallest number of libraries as possible.
Needed are (see also the project properties in visual studio):
opengl32.lib
Setupapi.lib
Hid.lib

This program is intended to give a start in developing a standalone application for the HDK2 virtual reality device.
This because OSVR is no longer updated and developing the OSVR with the HDK2 device is quite cumbersome (according to to my view at least).
I like a direct approach so I open the device as a HID (Human Interface Device) and just get the data from there.
For now only directional data since positional data seems much more problematic (with the IR-camera).

You could just look at the HID class in my code to build your own 3d application.
This engine is far from perfect, I made it several years ago (to give a try with OpenGL) and it can read (simple) object (.obj) and material files (.mtl), but for large objects it's not really usable.

Also I'm still busy with the HDK2 device, now there is a glitch when going through the '180 degrees barrier' in the view angle. 
For some reason the 'roll' gets out of control.
If I have time I will look into it, but as I said, it's just a hobby project for me.

Copyright (c) 2019 Dirk Prins

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
