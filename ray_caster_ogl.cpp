#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>
#include <iostream>

// PI values defn.
#define PI 3.1415926535
#define P2 PI/2
#define P3 3*PI/2

#define DR 0.0174533                    // 1° = 0.0174533 rad

int windowWidth = 1024;
int windowHeight = 512;

const int minFOV = 0;                   // min FOV in degrees
const int maxFOV = 140;                 // max FOV in degrees

float px, py, pa;                        // camera/player main coordinates 
float pdx, pdy, pdxS, pdyS;             // secondary movement-magnitude command variables for x,y elements
float ca = 0.0;                         // the key-binding variables for smooth movement

int keyW = 0, keyS = 0, keyA = 0, keyD = 0, keyQ = 0, keyE = 0, keyJ = 0, keyL = 0; // Key-binding booleans
int MouseX, MouseY;

int mapS = 3;
float rayCount = 120;                   // number of rays to be projected, same value as current FOV
const float rayCharge = 40;             // kind of a multiplier for rayCount, number of rays = rayCount*rayCharge
const float rayStep = 1/rayCharge;      // the step between rays in angles
float lt = 15/rayCharge;                // float lt = windowWidth/(rayCount*rayCharge);

int lastMouseX, lastMouseY;
bool ignoreNextMouse = false;

// the funtion to draw the main player position and angle
void drawPlayer() {
    glColor3f(1,0,0);
    glPointSize(4);
    glBegin(GL_POINTS);
    glVertex2i(px,py);
    glEnd();
}

// key-binding controls when key-pressed
void buttons(unsigned char key, int x, int y) {
    if (key == 'w') keyW = 1;
    if (key == 's') keyS = 1;
    if (key == 'q') keyQ = 1;
    if (key == 'e') keyE = 1;
    if (key == 'a') keyA = 1;
    if (key == 'd') keyD = 1;
    if (key == 'j') keyJ = 1;
    if (key == 'l') keyL = 1;
}

// key-binding controls when key-released
void buttonsUp(unsigned char key, int x, int y) {
    if (key == 'w') keyW = 0;
    if (key == 's') keyS = 0;
    if (key == 'q') keyQ = 0;
    if (key == 'e') keyE = 0;
    if (key == 'a') keyA = 0;
    if (key == 'd') keyD = 0;
    if (key == 'j') keyJ = 0;
    if (key == 'l') keyL = 0;
}

// the function to be fed to OpenGL to adhere with respect to mouse movements for camera-left,right
void mouseLook(int x, int y) {
    // starting with centering the cursor
    int centerX = windowWidth / 2;
    int centerY = windowHeight / 2;

    // a flag varible to restrict recursive mouse centering to avoid clashing and program crashes
    if (ignoreNextMouse) {
        ignoreNextMouse = false;
        return;
    }

    // movement difference, relative to previous mouse position
    int dx = x - centerX;
    int dy = y - centerY;

    // Apply to view angle
    float sensitivity = 0.002;
    pa += dx * sensitivity; // Adjust sensitivity as needed

    // apllying received commands from the mouse to the player/camera
    pa = fmod(pa + 2 * PI, 2 * PI);

    // calculating motion_vectors relative to player angle
    pdx = cos(pa) * 2;
    pdy = sin(pa) * 2;

    // resetting cursor to center
    glutWarpPointer(centerX, centerY);

    // releasing flag for next iteration
    ignoreNextMouse = true;
}

// the map characteristics
// map_unit width, height and size
int mapX = 20, mapY = 20;

// layout
int map[] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,
    0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,
    0,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,0,
    0,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,0,
    0,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,0,
    0,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,0,
    0,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,
    0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,1,1,1,0,
    0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,
    0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,
    0,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,
    0,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,
    0,1,1,1,0,1,1,1,1,1,0,0,0,0,0,0,0,0,1,0,
    0,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,
    0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,
    0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,
    0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,
    0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

// Arbitrary update function to be run when idling and for general updations
void update() {

    /* the key-bindings will modify the value in secondary value variables,
    which is constantly applied to the control variable for the main-player,
    this is done to avoid jittery motion */
    
    // rotation/angle secondary value updation
    if (keyJ) { pa -= 0.025; }
    if (keyL) { pa += 0.025; }

    // effective angle and movement direction calculations
    pa = fmod(pa + 2 * PI, 2 * PI);     // applied angle to the main player
    pdx = cos(pa) * 0.5;                // magnitude of motion in x-dirention based on current angle (pa), front/back
    pdy = sin(pa) * 0.5;                // magnitude of motion in y-dirention based on current angle (pa), front/back
    pdxS = cos(pa+(90*DR))*0.5;         // magnitude of motion in y-dirention based on current angle (pa), front/back
    pdyS = sin(pa+(90*DR))*0.5;         // magnitude of motion in y-dirention based on current angle (pa), front/back

    int Cx = (int)(((int)((px)/mapS)*mapS)/mapS);
    int Cy = (int)(((int)((py)/mapS)*mapS)/mapS);

    int Fx = (int)(((int)((px+pdx)/mapS)*mapS)/mapS);
    int Fy = (int)(((int)((py+pdy)/mapS)*mapS)/mapS);

    int Bx = (int)(((int)((px-pdx)/mapS)*mapS)/mapS);
    int By = (int)(((int)((py-pdy)/mapS)*mapS)/mapS);

    int Rx = (int)(((int)((px+pdxS)/mapS)*mapS)/mapS);
    int Ry = (int)(((int)((py+pdyS)/mapS)*mapS)/mapS);

    int Lx = (int)(((int)((px-pdxS)/mapS)*mapS)/mapS);
    int Ly = (int)(((int)((py-pdyS)/mapS)*mapS)/mapS);
     
    /* forward, backward, left, right movement apllication, calculated from current-camera angle */
    if (keyW) { if(map[Cy*mapX+Fx] == 1){px += pdx;} if(map[Fy*mapX+Cx] == 1){py += pdy;} }
    if (keyS) { if(map[Cy*mapX+Bx] == 1){px -= pdx;} if(map[By*mapX+Cx] == 1){py -= pdy;} }
    if (keyA) { if(map[Cy*mapX+Lx] == 1){px -= pdxS;} if(map[Ly*mapX+Cx] == 1){py -= pdyS;} }
    if (keyD) { if(map[Cy*mapX+Rx] == 1){px += pdxS;} if(map[Ry*mapX+Cx] == 1){py += pdyS;} }

    /* increase or decrease the FOV with q or e 
    jus increasing the rayCount to create more rays with the same angle between*/
    if (keyQ) { if(rayCount>minFOV){rayCount--;} }
    if (keyE) { if(rayCount<maxFOV){rayCount++;} }

    // updating the screen
    glutPostRedisplay();
}

// simple function to calculate the distance between two points, used in drawRays3D primarily
float dist(float ax, float ay, float bx, float by, float ang) {
    return ( sqrt((bx-ax)*(bx-ax) + (by-ay)*(by-ay)) );
}

void drawMap2D() {
    int x,y,xo,yo;

    for(y=0; y<mapY; y++) {
        for(x=0; x<mapX; x++) {
            if (map[y*mapX+x] == 1) { glColor3f(0.4,0.5,0.4); } else {glColor3f(0,0,0);}
            xo = x*mapS; yo = y*mapS;
            glBegin(GL_QUADS);
            glVertex2i(xo      , yo     );
            glVertex2i(xo      , yo+mapS);
            glVertex2i(xo+mapS , yo+mapS);
            glVertex2i(xo+mapS , yo     );
            glEnd();
        }
    }
}

// ray_casting and rendering
void drawRays3D() {

    int r, mx, my, mp, dof;     // map variables
    float rx, ry, ra;           // ray coordinates and angle
    float xo, yo;               // offsets to be used in ray_casting loop
    float disT;                 // distance of ray

    // first ray-offset for a symmetric bunch of rays
    float screen_width_px = lt*rayCharge*rayCount;
    float screen_width_px_b2 = screen_width_px/2;
    float lt_offset= lt/2;
    float screen_depth_px = ((screen_width_px/2)-lt_offset)/(tan((rayCount/2)*DR));
    float screen_displacement = lt/2;
    
    // iterating through rays
    for(r = 0; r<(rayCount*rayCharge); r++) {
        if(r<(rayCount*rayCharge)/2)
        {
            ra = atan2((screen_width_px_b2-screen_displacement), screen_depth_px);
            ra = (pa-ra);
            if(ra<0){ra+=2*PI;} if(ra>2*PI){ra-=2*PI;}
        }else if(r>(rayCount*rayCharge)/2)
        {
            ra = atan2((screen_displacement), screen_depth_px);
            ra = (pa+ra);
            if(ra<0){ra+=2*PI;} if(ra>2*PI){ra-=2*PI;}
        }else if(r==(rayCount*rayCharge)/2)
        {
            ra = pa;
            screen_displacement = lt/2;
            if(ra<0){ra+=2*PI;} if(ra>2*PI){ra-=2*PI;}
        }

        /* checking the horizontal grid lines */

        dof = 0;                                                        // this value for storing depth of feild, here max would be 7, as we have only 8x8 map matrix
        
        float disH = 1000000000;                                        // we eventually choose the sallest of two rays and store it in disH, hence we try to start big
        float hx=px, hy=py;                                             // horizontal ray length

        // The following is the process of finding the exact multiples of the mapS_64 relative to the player/camera position
        if (ra>PI) { ry = 
            (((int)py/mapS)*mapS)                                       // computes the perfect multiples of mapS_64 then...
            - 0.0001;                                                   // ...subtracting a small value so that the ray doesnt go to the next element in the map_matrix
            rx=(py-ry)/(-tan(ra))+px; yo=-mapS; xo=yo/tan(ra); }        // using trig_functions to compute the ray's x and y end-postions
        
        if (ra<PI) { ry = (((int)py/mapS)*mapS)                         // computes the perfect multiples of mapS_64 then...
            + mapS;                                                     // ...adding mapS so that the ray detect 1-element in the map_matrix
            rx=(py-ry)/(-tan(ra))+px; yo= mapS; xo=yo/tan(ra); }        // using trig_functions to compute the ray's x and y end-postions
        
        if (ra==0 || ra==PI) {rx=px; ry=py; dof=mapY;}                  // avoiding continuous looping-search iin said case

        while(dof < mapY) {                                             // calculating the ray's depth in the map, to determine if further searching is required or not
            mx = (int)(rx)/mapS; my = (int)(ry)/mapS; mp=my*mapX+mx;    // mapping the ray end_points to the elements of the map_matrix
            if ( mp>0 && mp<mapX*mapY && map[mp] == 0) {hx=rx; hy=ry; disH=dist(px,py,hx,hy,ra); dof=mapY;} // wall-hit, i.e., the element currently being checked is 1 and the length of the ray disH is calculated
            else { rx+=xo; ry+=yo; dof+=1;}                             // no wall-hit, next line
        }

        /* checking the vertical grid lines */

        dof = 0;
        float disV = 1000000000, vx=px, vy=py;

    
        if (ra>P2 && ra<P3) { rx = (((int)px/mapS)*mapS)-0.0001; ry=py-(px-rx)*tan(ra); xo=-mapS; yo=xo*tan(ra); }
        if (ra<P2 || ra>P3) { rx = (((int)px/mapS)*mapS)+mapS    ; ry=py-(px-rx)*tan(ra); xo= mapS; yo=xo*tan(ra); }
        if (ra==0 || ra==PI) {rx=px; ry=py; dof=mapX;}

        while(dof < mapX) {
            mx = (int)(rx)/mapS; my = (int)(ry)/mapS; mp=my*mapX+mx;
            if ( mp>0 && mp<mapX*mapY && map[mp] == 0) {vx=rx; vy=ry; disV=dist(px,py,vx,vy,ra); dof=mapX;} // wall-hit
            else { rx+=xo; ry+=yo; dof+=1;}                             // no wall-hit, next line
        }

        float render_distance = 100;
        float base_value = 1;
        float base_value2 = 0.8;
        
        if(disV<disH) {
            rx=vx;
            ry=vy;
            disT=disV;
            float factr = 1-disT/render_distance;
            if(disT>=render_distance){glColor3f(0.0,0.0,0.0);}
            else if(disT<render_distance){if(disT<0.5){glColor3f(0.8,0.0,0.0);}else{glColor3f(0.0,base_value*factr,0.0);}}
        }
        else if(disH<disV) {
            rx=hx;
            ry=hy;
            disT=disH;
            float factr = 1-disT/render_distance;
            if(disT>=render_distance){glColor3f(0.0,0.0,0.0);}
            else if(disT<render_distance){if(disT<0.5){glColor3f(0.8,0.0,0.0);}else{glColor3f(0.0,base_value2*factr,0.0);}}
        }

        // data stream output for trouble-shooting or observation
        if(r==0)std::cout<<"RayAngle:"<<ra/DR<<"  CameraAngle:"<<pa/DR<<"  camXmap:"<<(int)((int)((px/mapS)*mapS))/mapS<<"  camYmap:"<<(int)((int)((py/mapS)*mapS))/mapS<<"  rayX:"<<rx<<"  rayY:"<<ry<<"  rayCount:"<<rayCount<<std::endl;

        /* drawing 3D walls */
        
        float cA = pa-ra; if(cA<0){cA+=2*PI;} if(cA>2*PI){cA-=2*PI;} disT=disT*cos(cA); // this is to resolve fisheye
        float lineH = (mapS*windowHeight)/disT; if(lineH>windowHeight) {lineH=windowHeight;}
        float lineO = windowHeight/2-lineH/2;

        // drawing the walls using rectangles for every ray...
        glBegin(GL_QUADS);
        glVertex2i(mapS*mapX+r*lt, lineO);
        glVertex2i(mapS*mapX+r*lt+lt, lineO);
        glVertex2i(mapS*mapX+r*lt+lt, lineH+lineO);
        glVertex2i(mapS*mapX+r*lt, lineH+lineO);
        glEnd();

        // incrementing ra(ray_angle), for the next angle
        
        glColor3f(0.8,0.8,0.0);
        glLineWidth(1);glBegin(GL_LINES);glVertex2f(px,py);glVertex2f(rx, ry);glEnd();
        
        // ra+=DR*rayStep; if(ra<0){ra+=2*PI;} if(ra>2*PI){ra-=2*PI;}
        screen_displacement+=lt;
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3f(0.0,0.0,0.0);
    glBegin(GL_QUADS);
    
    glVertex2i(mapX*mapS,0);
    glVertex2i(mapX*mapS+rayCount*lt*rayCharge,0);
    glVertex2i(mapX*mapS+rayCount*lt*rayCharge, windowHeight/2);
    glVertex2i(mapX*mapS, windowHeight/2);
    
    glEnd();

    glColor3f(0,0,0);
    glBegin(GL_QUADS);
    glVertex2i(mapX*mapS,windowHeight/2);
    glVertex2i(mapX*mapS+rayCount*lt*rayCharge,windowHeight/2);
    glVertex2i(mapX*mapS+rayCount*lt*rayCharge, windowHeight);
    glVertex2i(mapX*mapS, windowHeight);

    glEnd();

    drawMap2D();
    drawRays3D();
    
    drawPlayer();
    glutSwapBuffers();
}

void init() {

    lastMouseX = (windowWidth / 2 )+1;
    lastMouseY = (windowHeight / 2 )+1;
    glutWarpPointer(lastMouseX, lastMouseY);    // Move cursor to center initially
    glutSetCursor(GLUT_CURSOR_NONE);            // Hide the cursor

    glClearColor(0.3, 0.3, 0.3, 0);
    gluOrtho2D(0, 2048, 512, 0);
    pa = 57.0*DR;                               // initiating player angle
    px = 1*mapS, py = 1*mapS;                   // initiating player position
    pdx = cos(pa)*2; pdy = sin(pa)*2;
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(2048,512);
    glutCreateWindow("RayCaster");
    init();

    glutPassiveMotionFunc(mouseLook);
    
    glutKeyboardFunc(buttons);
    glutKeyboardUpFunc(buttonsUp);
    glutIdleFunc(update);
    glutDisplayFunc(display);
    
    glutMainLoop();
}