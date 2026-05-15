# Ray-Caster-OpenGL
a C++ program utilising OpenGL dependencies to create a Ray Caster

The program is built upon on a Euler-angle based system, where camera's yaw is the basis upon which further translations are calculated henceforth. The rendering of the scene viewed by the camera, is acheived through OpenGL rendering (inbuilt OpenGL functions are not used for any of the computations, only rendering)<br>

The program provides the user with the following controls:<br>
-> Camera Yaw rotation<br>
-> Camera front, back, right, left movements<br>
-> Camera FOV control (in-code)<br>
-> Ray density control (in-code via float rayCharge)<br>

## How to run

```shell
sudo apt install libgl1-mesa-dev libglu1-mesa-dev freeglut3-dev mesa-common-dev

```

libgl1-mesa-dev: Core OpenGL library.<br>
libglu1-mesa-dev: OpenGL Utility Library (GLU).<br>
freeglut3-dev: The FreeGLUT implementation of the GLUT toolkit.<br>
mesa-common-dev: Common Mesa development headers.


then link the dependencies and compile the program into an executable 

```shell
g++ ray_caster_ogl.cpp -o ray -lGL -lGLU -lglut
```

run the executable
```shell
./ray
```

## Rendering

<img width="2076" height="578" alt="Screenshot from 2026-05-15 17-23-56" src="https://github.com/user-attachments/assets/fd842a46-d677-480f-84ec-9c642e4211d9" /><br>
<img width="2076" height="578" alt="Screenshot from 2026-05-15 17-23-40" src="https://github.com/user-attachments/assets/7deb4275-794e-4945-a5db-2f14bb489e7d" />
