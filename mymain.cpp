//*********************************
//
//  CMPT 464
//  Assignment 1 - SMF Reader
//  
//  Hazel (Hei Man) Yip
//  301175142
//  
//*********************************


/** INCLUDE LIBRARIES **/
#include <GL/glui.h>

/** CALLBACK ID'S **/
#define OPTION_FLAT     100
#define OPTION_SMOOTH   101
#define OPTION_WIRE     102
#define OPTION_COMBO    103
#define LOAD_FILE       104
#define SAVE_FILE       105

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
    glTranslatef( 0.0, 0.0, -5.5f );
    
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
        glVertex3f(testCube[9*i],           //Vertex 1 coordinates
                    testCube[9*i+1],
                    testCube[9*i+2]);
        glVertex3f(testCube[9*i+3],         //Vertex 2 coordinates
                    testCube[9*i+4],
                    testCube[9*i+5]);
        glVertex3f(testCube[9*i+6],         //Vertex 3 coordinates
                   testCube[9*i+7],
                   testCube[9*i+8]);
        glEnd();
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
    renderWire();
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
            /*t_x = new GLUI_Translation(translation_panel,"Translation X",GLUI_TRANSLATION_X,obj_pos);
                t_x->set_speed( .005 );
            new GLUI_Column(translation_panel,false);
            t_y = new GLUI_Translation(translation_panel,"Translation Y",GLUI_TRANSLATION_Y,&obj_pos[1]);
                t_y->set_speed( .005 );*/
            t_xy = new GLUI_Translation(translation_panel,"Translation XY",GLUI_TRANSLATION_XY,obj_pos);
                t_xy->set_speed( .005 );
            new GLUI_Column(translation_panel,false);
            t_z = new GLUI_Translation(translation_panel,"Tranlation Z",GLUI_TRANSLATION_Z,&obj_pos[2]);
                t_z->set_speed( .005 );

        // ROTATION PANEL
        rotation_panel = new GLUI_Panel(transformation_panel,"", GLUI_PANEL_EMBOSSED);
            ctrl_rotate = new GLUI_Rotation(rotation_panel,"Rotation", rotation_array);
            // ZOOM BUTTON
            /*new GLUI_Column(rotation_panel,false);
            zoom = new GLUI_Translation(rotation_panel,"Zoom",GLUI_TRANSLATION_Z,&obj_pos[2]);
                zoom->set_speed( .005 );*/
            

        //MAKE SURE THIS GLUI SUBWINDOWS KNOWS THE NAME OF THE MAIN GLUT WINDOW
        //bottom_sub->set_main_gfx_window(main_window);


        // MAKE SURE THIS GLUI SUBWINDOW KNOWS THE NAME OF MY MAIN GLUT WINDOW
        right_sub->set_main_gfx_window(main_window);
        bn_quit = new GLUI_Button(right_sub,"Quit",0,(GLUI_Update_CB)exit);

	// enter GLUT event processing cycle
	glutMainLoop();

	return 1;
}
