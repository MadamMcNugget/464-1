#include "GL/glui.h"
#include "SMFParser.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <fstream>
using namespace std;


#define OPEN_ID              302
#define SAVE_ID              303

GLuint g_uiDisplayID = 0;
GLuint g_uiDisplayID1 = 0;
int g_iWindowID = 0;
int   wireframe = 0;
int   segments = 8;


char strOpenFile[1024] = "smf/sphere.smf";
char strSaveFile[1024] = "smf/sphere.smf";

SMFParser g_Parser;

char *string_list[] = { "flat shaded", "wireframe", "shaded with mesh edges", "smooth shaded" };
int   curr_string = 0;
float view_rotate[16] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
float obj_pos[] = { 0.0, 0.0, -2.0 };

GLfloat light0_ambient[] =  {0.1f, 0.1f, 0.3f, 1.0f};
GLfloat light0_diffuse[] =  {.6f, .6f, 1.0f, 1.0f};
GLfloat light0_position[] = {.5f, .5f, 1.0f, 0.0f};

GLfloat light1_ambient[] =  {0.1f, 0.1f, 0.3f, 1.0f};
GLfloat light1_diffuse[] =  {.9f, .6f, 0.0f, 1.0f};
GLfloat light1_position[] = {-1.0f, -1.0f, 1.0f, 0.0f};

GLfloat lights_rotation[16] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };

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

void keyboard(unsigned char key, int x, int y)
{

}

void changeSize(int width, int height)
{
	int tx, ty;
	GLUI_Master.get_viewport_area( &tx, &ty, &width, &height );
	glViewport( tx, ty, width, height );

	if(height == 0)
		height = 1;

	float ratio = 1.0f * width / height;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90,ratio,0.1,1000);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void renderScene(void) 
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	glTranslatef(obj_pos[0], obj_pos[1], obj_pos[2]);
	glMultMatrixf(view_rotate);
	
	switch (curr_string)
	{
	case 0:
		glEnable(GL_LIGHTING);
		glColor3f(1.f, 0.5f, 1.f);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	case 1:
		glDisable(GL_LIGHTING);
		glColor3f(1.f, 1.f, 1.f);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case 2:
		glEnable(GL_LIGHTING);
		glColor3f(1.f, 0.5f, 1.f);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	case 3:
		glEnable(GL_LIGHTING);
		glColor3f(1.f, 0.5f, 1.f);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	}

	if (g_uiDisplayID > 0 && curr_string != 3)
	{
		glCallList(g_uiDisplayID);

		if (curr_string == 2)
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

	if (g_uiDisplayID1 > 0 && curr_string == 3)
	{
		glCallList(g_uiDisplayID1);
	}

	glutSwapBuffers();
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

void control_cb( int control )
{
	char filePathName[1024] = {0};

	if (control == OPEN_ID)
	{
		//printf("\n\n#########################\n\nPlease Enter Your Mesh File Path Name:\n");
		//scanf("%s", filePathName);

		if (!g_Parser.Parser(strOpenFile))
		{
			printf("Parser SMF Files Failed!\n");
			return;
		}

		initModelDisplay(g_Parser);
		glutPostRedisplay();
	}
	else if (control == SAVE_ID)
	{
		// save to file
		const std::vector<FFace>& faceList = g_Parser.GetFaceList();
		const std::vector<FVector3f>& vertexList = g_Parser.GetVertexList();

		fstream file(strSaveFile, ios::out);
		if (file.good())
		{
			file << "#" << vertexList.size() << " " << faceList.size() << endl;
			for (std::vector<FVector3f>::size_type i = 0; i < vertexList.size(); ++i)
			{
				const FVector3f& vertex = vertexList[i];

				file << "v" << vertex._x << " " << vertex._y << " " << vertex._z << endl;
			}

			for (std::vector<FFace>::size_type i = 0; i < faceList.size(); ++i)
			{
				const FFace& face = faceList[i];

				file << "f" << face._idx0 << " " << face._idx1 << " " << face._idx2 << endl;
			}

			file.close();
		}
		else
		{
			printf("Save To File \"%s\" Failed\n", strSaveFile);
		}
	}
}

void textbox_cb_Open(GLUI_Control *control) {
	strcpy(strOpenFile, control->text.c_str());
}

void textbox_cb_Save(GLUI_Control *control) {
	strcpy(strSaveFile, control->text.c_str());
}

void init() 
{
	if (!g_Parser.Parser("smf/sphere.smf"))
	{
		printf("Parser SMF Files Failed!\n");
		return;
	}

	initModelDisplay(g_Parser);

	glutDisplayFunc(renderScene);
	GLUI_Master.set_glutKeyboardFunc( keyboard );
	GLUI_Master.set_glutReshapeFunc( changeSize );  

	// glui
	GLUI *glui = GLUI_Master.create_glui_subwindow( g_iWindowID, 
		GLUI_SUBWINDOW_RIGHT );

	GLUI_Listbox *list = new GLUI_Listbox( glui, "Shaded:", &curr_string );
	int i;
	for( i=0; i<sizeof(string_list) / sizeof(string_list[0]); i++ )
		list->add_item( i, string_list[i] );

	new GLUI_StaticText( glui, "" );

	GLUI_Panel *ep = new GLUI_Panel(glui,".SMF File",true);
	GLUI_TextBox *moo = new GLUI_TextBox(ep,false,1,textbox_cb_Open);
	moo->set_text(strOpenFile);
	moo->set_h(20);
	moo->enable();
	new GLUI_Button( ep, "Open", OPEN_ID, control_cb );

	new GLUI_StaticText( glui, "" );

	ep = new GLUI_Panel(glui,".SMF File",true);
	moo = new GLUI_TextBox(ep,false,2,textbox_cb_Save);
	moo->set_text(strSaveFile);
	moo->set_h(20);
	moo->enable();
	new GLUI_Button( ep, "Save", SAVE_ID, control_cb );
	
	new GLUI_StaticText( glui, "" );
	new GLUI_Button( glui, "Quit", 0,(GLUI_Update_CB)exit );

	glui->set_main_gfx_window( g_iWindowID );


	GLUI *glui2 = GLUI_Master.create_glui_subwindow( g_iWindowID, 
		GLUI_SUBWINDOW_BOTTOM );
	GLUI_Rotation *view_rot = new GLUI_Rotation(glui2, "Rotation", view_rotate );
		view_rot->set_spin( 1.0 );
	new GLUI_Column( glui2, false );

	GLUI_Translation *trans_x = 
		new GLUI_Translation(glui2, "Objects X", GLUI_TRANSLATION_X, obj_pos );
	trans_x->set_speed( .005 );
	new GLUI_Column( glui2, false );

	GLUI_Translation *trans_y = 
		new GLUI_Translation( glui2, "Objects Y", GLUI_TRANSLATION_Y, &obj_pos[1] );
	trans_y->set_speed( .005 );
	new GLUI_Column( glui2, false );

	GLUI_Translation *trans_z = 
		new GLUI_Translation( glui2, "Objects Z", GLUI_TRANSLATION_Z, &obj_pos[2] );
	trans_z->set_speed( .005 );

	glui2->set_main_gfx_window( g_iWindowID );

	//GLUI_Master.set_glutIdleFunc( myGlutIdle ); 

	glClearColor(0.8f, 0.8f, 0.8f, 1.f);
	glEnable(GL_DEPTH_TEST);

	// lingting
	glEnable(GL_LIGHTING);
	glEnable( GL_NORMALIZE );

	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);

	glEnable(GL_LIGHT1);
	glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, light1_position);

}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(640,360);
	g_iWindowID = glutCreateWindow("smf_viewer");

	init();
	glutMainLoop();

	system("pause");
	return 0;
}