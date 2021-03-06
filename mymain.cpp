//*********************************
//
//  CMPT 464
//  Assignment 1 - SMF Reader
//  
//  Hazel (Hei Man) Yip
//  301175142
//  
//*********************************


/* ***************************

    GIT COMMANDS

    git add -A
    git commit -m "message"
    git push -u origin master

******************************/


/** INCLUDE LIBRARIES **/
#include <GL/glui.h>
#include <iostream>
#include <stdlib.h> 
#include <string.h>   
#include "SMFParser.h"
#include <math.h>
#include <fstream>

/** CALLBACK ID'S **/
#define OPTION_FLAT     100
#define OPTION_SMOOTH   101
#define OPTION_WIRE     102
#define OPTION_COMBO    103
#define LOAD_FILE       104
#define SAVE_FILE       105

char strOpenFile[1024] = "smf/sphere.smf";
char strSaveFile[1024] = "smf/sphere.smf";

GLuint g_uiDisplayID = 0;
GLuint g_uiDisplayID1 = 0;

SMFParser g_Parser;

using namespace::std;

/** LIVE VARIABLES AND INITIAL VALUES USED BY GLUI **/
int     main_window;
int     disp_option=OPTION_FLAT;
float   rotation_array[16] = {1.0,0.0,0.0,0.0,
                              0.0,1.0,0.0,0.0,
                              0.0,0.0,1.0,0.0,   
                              0.0,0.0,0.0,1.0};
float   obj_pos[] = {0.0,0.0,0.0};
float   scale = 0.5;

float xy_aspect; //Variable used in reshape function


GLfloat light_ambient[] =  {0.1f, 0.1f, 0.3f, 1.0f};
GLfloat light_diffuse[] =  {.9f, .6f, 0.0f, 1.0f};
GLfloat light_position[] = {-1.0f, -1.0f, 1.0f, 0.0f};

GLfloat lights_rotation[16] = {1, 0, 0, 0, 
                               0, 1, 0, 0, 
                               0, 0, 1, 0, 
                               0, 0, 0, 1 };


// The following array encodes the geometry of a simple 1x1x1 cube
// Each row corresponds to a single vertex's (x,y,z) coordinates
float testCube[108] = {-1.0f,-1.0f,-1.0f, // triangle 1 : begin (x,y,z)
                            -1.0f,-1.0f, 1.0f,
                            -1.0f, 1.0f, 1.0f, // triangle 1 : end
                            1.0f, 1.0f,-1.0f, // triangle 2 : begin
                            -1.0f,-1.0f,-1.0f,
                            -1.0f, 1.0f,-1.0f, // triangle 2 : end, etc
                            1.0f,-1.0f, 1.0f,
                            -1.0f,-1.0f,-1.0f,
                            1.0f,-1.0f,-1.0f,
                            1.0f, 1.0f,-1.0f,
                            1.0f,-1.0f,-1.0f,
                            -1.0f,-1.0f,-1.0f,
                            -1.0f,-1.0f,-1.0f,
                            -1.0f, 1.0f, 1.0f,
                            -1.0f, 1.0f,-1.0f,
                            1.0f,-1.0f, 1.0f,
                            -1.0f,-1.0f, 1.0f,
                            -1.0f,-1.0f,-1.0f,
                            -1.0f, 1.0f, 1.0f,
                            -1.0f,-1.0f, 1.0f,
                            1.0f,-1.0f, 1.0f,
                            1.0f, 1.0f, 1.0f,
                            1.0f,-1.0f,-1.0f,
                            1.0f, 1.0f,-1.0f,
                            1.0f,-1.0f,-1.0f,
                            1.0f, 1.0f, 1.0f,
                            1.0f,-1.0f, 1.0f,
                            1.0f, 1.0f, 1.0f,
                            1.0f, 1.0f,-1.0f,
                            -1.0f, 1.0f,-1.0f,
                            1.0f, 1.0f, 1.0f,
                            -1.0f, 1.0f,-1.0f,
                            -1.0f, 1.0f, 1.0f,
                            1.0f, 1.0f, 1.0f,
                            -1.0f, 1.0f, 1.0f,
                            1.0f,-1.0f, 1.0f};


/** POINTERS TO GLUI CONTROLS AND SUBWINDOWS **/
GLUI            *right_sub;
GLUI_Panel      *file_panel, *options_panel, *transformation_panel, *translation_panel, *rotation_panel;
GLUI_Button     *bn_load, *bn_save, *bn_quit;
GLUI_EditText   *load_path, *save_path;
GLUI_Listbox    *lst_display_options;
GLUI_Rotation   *ctrl_rotate;
GLUI_Translation *t_xy,*t_x,*t_y,*t_z,*zoom;


/** THIS FUNCTION PRODUCES A WIREFRAME MODEL **/
void renderWire(void) {
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    // Model initially offset slightly to the left, so it appears centered in viewport
    glTranslatef( 0.0, 0.0, -2.5f );
    
    // Translation, scale and rotation are bound to GLUI live variables
    glTranslatef(obj_pos[0], obj_pos[1], -obj_pos[2]);
    glScalef(scale,scale,scale);
    glMultMatrixf(rotation_array);

    // Set line width and color
    glLineWidth(1.2);
    glColor3f(0.8f, 0.8f, 0.8f);

    glDisable(GL_LIGHTING);

    // This tells OpenGL to only draw lines, no filled polygons
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	/**
    Loop through vertices and draw each TRIANGLE */
	for (int i=0; i<12; i++) {
        glBegin(GL_TRIANGLES);
        /*glVertex3f(testCube[9*i],           //Vertex 1 coordinates
                    testCube[9*i+1],
                    testCube[9*i+2]);
        glVertex3f(testCube[9*i+3],         //Vertex 2 coordinates
                    testCube[9*i+4],
                    testCube[9*i+5]);
        glVertex3f(testCube[9*i+6],         //Vertex 3 coordinates
                   testCube[9*i+7],
                   testCube[9*i+8]);*/
        glEnd();
	}

}


FVector3f GetNormalFromThreePosition(const FVector3f& v0, const FVector3f& v1, const FVector3f& v2)
{
    FVector3f res;
    FVector3f ab = {
        v1._x - v0._x,
        v1._y - v0._y,
        v1._z - v0._z
    };
    FVector3f ac = {
        v2._x - v0._x,
        v2._y - v0._y,
        v2._z - v0._z
    };

    res._x = ab._y * ac._z - ab._z * ac._y;
    res._y = ab._z * ac._x - ab._x * ac._z;
    res._z = ab._x * ac._y - ab._y * ac._x;

    return res;
}

FVector3f GetSmoothNormal(int vertexIdx, SMFParser& parser)
{
    FVector3f res = {0.f, 0.f, 0.f};

    for (std::vector<FFace>::size_type i = 0; i < parser.GetFaceList().size(); ++i)
    {
        const FFace& face = parser.GetFaceList()[i];

        if (vertexIdx == face._idx0 || vertexIdx == face._idx1 || vertexIdx == face._idx2)
        {
            const FVector3f& vertex0 = parser.GetVertexList()[face._idx0 - 1];
            const FVector3f& vertex1 = parser.GetVertexList()[face._idx1 - 1];
            const FVector3f& vertex2 = parser.GetVertexList()[face._idx2 - 1];
            FVector3f normal = GetNormalFromThreePosition(vertex0, vertex1, vertex2);
            float fSqure = normal._x * normal._x + normal._y * normal._y + normal._z * normal._z;
            if (fSqure == 0.f)
            {
                fSqure = 1.f;
            }

            fSqure = 1.f / sqrt(fSqure);
            normal._x *= fSqure;
            normal._y *= fSqure;
            normal._z *= fSqure;

            res._x += normal._x;
            res._y += normal._y;
            res._z += normal._z;
        }
    }

    return res;
}

void initModelDisplay(SMFParser& parser)
{
    if (g_uiDisplayID == 0)
    {
        g_uiDisplayID = glGenLists(1);
    }

    if (g_uiDisplayID1 == 0)
    {
        g_uiDisplayID1 = glGenLists(1);
    }

    if (g_uiDisplayID)
    {
        glNewList(g_uiDisplayID, GL_COMPILE);
            glBegin(GL_TRIANGLES);
            for (std::vector<FFace>::size_type i = 0; i < parser.GetFaceList().size(); ++i)
            {
                const FFace& face = parser.GetFaceList()[i];
                const FVector3f& vertex0 = parser.GetVertexList()[face._idx0 - 1];
                const FVector3f& vertex1 = parser.GetVertexList()[face._idx1 - 1];
                const FVector3f& vertex2 = parser.GetVertexList()[face._idx2 - 1];
                FVector3f normal = GetNormalFromThreePosition(vertex0, vertex1, vertex2);

                glNormal3f(normal._x, normal._y, normal._z);
                glVertex3f(vertex0._x, vertex0._y, vertex0._z);
                glVertex3f(vertex1._x, vertex1._y, vertex1._z);
                glVertex3f(vertex2._x, vertex2._y, vertex2._z);
            }
            glEnd();
        glEndList();
    }
    
    if (g_uiDisplayID1)
    {
        glNewList(g_uiDisplayID1, GL_COMPILE);
        glBegin(GL_TRIANGLES);
        for (std::vector<FFace>::size_type i = 0; i < parser.GetFaceList().size(); ++i)
        {
            const FFace& face = parser.GetFaceList()[i];
            const FVector3f& vertex0 = parser.GetVertexList()[face._idx0 - 1];
            const FVector3f& vertex1 = parser.GetVertexList()[face._idx1 - 1];
            const FVector3f& vertex2 = parser.GetVertexList()[face._idx2 - 1];
            FVector3f normal0 = GetSmoothNormal(face._idx0, parser);
            FVector3f normal1 = GetSmoothNormal(face._idx1, parser);
            FVector3f normal2 = GetSmoothNormal(face._idx2, parser);

            glNormal3f(normal0._x, normal0._y, normal0._z);
            glVertex3f(vertex0._x, vertex0._y, vertex0._z);
            glNormal3f(normal1._x, normal1._y, normal1._z);
            glVertex3f(vertex1._x, vertex1._y, vertex1._z);
            glNormal3f(normal2._x, normal2._y, normal2._z);
            glVertex3f(vertex2._x, vertex2._y, vertex2._z);
        }
        glEnd();
        glEndList();
    }
}

/** This function performs the actual rendering, and would invoke the rendering function according to desired shading model. In this example, only wireframe is rendered **/
void renderScene(void) {

    // Set a background color
    glClearColor( .25f, .23f, .23f, 1.0f );                 //NICE DARK GRAY BACKGROUND FOR VIEWPORT
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    // RENDERS MODEL WITH PERSPECTIVE
    glFrustum( -xy_aspect*.04, xy_aspect*.04, -.04, .04, .1, 15.0 ); 


    // IN THIS EXAMPLE WE ONLY HAVE A WIREFRAME RENDERING FUNCTION
    // IN YOUR ASSIGNMENT, YOU SHOULD INVOKE A RENDERING FUNCTION ACCORDING TO THE SHADING MODEL SELECTED BY THE USER
    switch (disp_option)
    {
    case OPTION_FLAT:
        renderWire();
        glEnable(GL_LIGHTING);
        glColor3f(1.f, 0.5f, 1.f);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        break;
    case OPTION_WIRE:
        renderWire();
        glDisable(GL_LIGHTING);
        glColor3f(1.f, 1.f, 1.f);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        break;
    case OPTION_SMOOTH:
        renderWire();
        glEnable(GL_LIGHTING);
        glColor3f(1.f, 0.5f, 1.f);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        break;
    case OPTION_COMBO:
        renderWire();
        glEnable(GL_LIGHTING);
        glColor3f(1.f, 0.5f, 1.f);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        break;
    }

    if (g_uiDisplayID > 0 && disp_option != OPTION_SMOOTH)
    {
        glCallList(g_uiDisplayID);

        if (disp_option == 2)
        {
            glDisable(GL_LIGHTING);
            glColor3f(1.f, 1.f, 1.f);
            glEnable(GL_POLYGON_OFFSET_FILL);
            glPolygonOffset(1.0f,0.0f);
            glLineWidth(2.f);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glCallList(g_uiDisplayID);
            glDisable(GL_POLYGON_OFFSET_FILL);
        }
    }

    if (g_uiDisplayID1 > 0 && disp_option == OPTION_SMOOTH)
    {
        glCallList(g_uiDisplayID1);
    }
    
    glutSwapBuffers();

    glutPostRedisplay();
}

/** RESHAPE WINDOW BEHAVIOR **/
void myGlutReshape(int x, int y) {
    int tx, ty, tw, th;
    GLUI_Master.get_viewport_area( &tx, &ty, &tw, &th );
    glViewport( tx, ty, tw, th );

    xy_aspect = (float)tw / (float)th;

    glutPostRedisplay();
}


/** GLUI Control Callback          ***/
void control_cb(int controlID) {

    //char filePathName[1024] = {0};

    if (controlID == LOAD_FILE)
    {
        //printf("\n\n#########################\n\nPlease Enter Your Mesh File Path Name:\n");
        //scanf("%s", filePathName);

        cout<<"opening "<<load_path->get_text()<<".\n";
        strcpy(strOpenFile, load_path->get_text());

        if (!g_Parser.Parser(strOpenFile))
        {
            printf("Parser SMF Files Failed!\n");
            return;
        }

        initModelDisplay(g_Parser);
        glutPostRedisplay();
    }

    else if (controlID == SAVE_FILE)
    {
        cout<<"save pressed.\n";
    }
}


/******** MY MAIN FUNCTION ******************************************/
int main(int argc, char **argv) {

	// initialize GLUT and create Window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(15,15);
	glutInitWindowSize(800,600);

	main_window = glutCreateWindow("SMF Viewer");

    glutDisplayFunc(renderScene);
    GLUI_Master.set_glutReshapeFunc( myGlutReshape );

    // CREATE A SUBWINDOW ON THE RIGHT
    right_sub = GLUI_Master.create_glui_subwindow(main_window,GLUI_SUBWINDOW_RIGHT);

        // A PANEL WITH MY FILE LOADING AND SAVING BUTTONS
        file_panel = new GLUI_Panel(right_sub,"File Menu", GLUI_PANEL_EMBOSSED);
            load_path = new GLUI_EditText(file_panel,"Path:");
            load_path->set_w(200);
            bn_load = new GLUI_Button(file_panel,"Load",LOAD_FILE,control_cb);
            save_path = new GLUI_EditText(file_panel,"Path:");
            save_path->set_w(200);
            bn_save = new GLUI_Button(file_panel,"Save",SAVE_FILE,control_cb);

        // A PANEL WITH MY DISPLAY OPTIONS
        options_panel = new GLUI_Panel(right_sub,"Display Options", GLUI_PANEL_EMBOSSED);
            lst_display_options = new GLUI_Listbox(options_panel,"Options",&disp_option,OPTION_FLAT,control_cb);
                lst_display_options->add_item(OPTION_FLAT,"Flat Shading");
                lst_display_options->add_item(OPTION_SMOOTH,"Smooth Shading");
                lst_display_options->add_item(OPTION_WIRE,"Wireframe");
                lst_display_options->add_item(OPTION_COMBO,"Shaded with mesh edges");

        // TRANSFORMATION PANEL
        transformation_panel = new GLUI_Panel(right_sub,"Tranformation", GLUI_PANEL_EMBOSSED);

        // TRANSLATION PANEL
        translation_panel = new GLUI_Panel(transformation_panel,"", GLUI_PANEL_EMBOSSED);
            t_xy = new GLUI_Translation(translation_panel,"Translation XY",GLUI_TRANSLATION_XY,obj_pos);
                t_xy->set_speed( .005 );
            new GLUI_Column(translation_panel,false);
            t_z = new GLUI_Translation(translation_panel,"Tranlation Z",GLUI_TRANSLATION_Z,&obj_pos[2]);
                t_z->set_speed( .005 );

        // ROTATION PANEL
        rotation_panel = new GLUI_Panel(transformation_panel,"", GLUI_PANEL_EMBOSSED);
            ctrl_rotate = new GLUI_Rotation(rotation_panel,"Rotation", rotation_array);


        // MAKE SURE THIS GLUI SUBWINDOW KNOWS THE NAME OF MY MAIN GLUT WINDOW
        right_sub->set_main_gfx_window(main_window);
        bn_quit = new GLUI_Button(right_sub,"Quit",0,(GLUI_Update_CB)exit);

    // Lighting
    glClearColor(0.8f, 0.8f, 0.8f, 1.f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable( GL_NORMALIZE );

    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    // Init a model
    if (!g_Parser.Parser("smf/sphere.smf"))
    {
        printf("Parser SMF Files Failed!\n");
    }

    initModelDisplay(g_Parser);

    glutDisplayFunc(renderScene);

	// enter GLUT event processing cycle
	glutMainLoop();

	return 1;
}
