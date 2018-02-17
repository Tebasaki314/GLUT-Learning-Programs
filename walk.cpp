// teapot.cpp
// 
// ティーポットを描く

#include <GL/glut.h>

#include <cstdio>
#include <cstdlib>

#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979
#endif

// 定数・変数の宣言 //

// ウインドウのサイズ

const int WINDOW_WIDTH = 640;
const int WINDOW_HEIGHT = 480;
const int WINDOW_POSITION_X = 0;
const int WINDOW_POSITION_Y = 0;

// 投資投影変換のパラメータ

const double NEAR_CLIPPINT_LENGTH = 1.0;
const double FAR_CLIPPINT_LENGTH = 10000.0;

const double FIELD_OF_VIEW = 40.0;

// カメラの位置と注視点

const double EYE_X = 30.0;
const double EYE_Y = 20.0;
const double EYE_Z = 50.0;

const double TARGET_X = 0.0;
const double TARGET_Y = 0.0;
const double TARGET_Z = 0.0;

const double UP_X = 0.0;
const double UP_Y = 1.0;
const double UP_Z = 0.0;

// 物体関連

const double ROT_ANGLE_VELOCITY = 2.0;
const double ANGLE_MAX = 40.0;

const double LEG_LENGTH = 5.0;
const double LEG_THICKNESS = 1.0;

const double INITIAL_LEG_ANGLE = 0.0;
const double INITIAL_BODY_X = -30.0;
const double INITIAL_BODY_Y = LEG_LENGTH;
const double INITIAL_BODY_Z = 0.0;
const double INITIAL_BODY_DIR = 0.0;


// グローバル変数

int window_width;
int window_height;
int counter;
int is_moving;

double angle_x;
double angle_y;

double leg_angle;
double body_x, body_y, body_z;
double body_dir;
int on_ground; // 0: left, 1: right


// キャラクタの初期化 //

void InitCharacterPosition(void)
{
	leg_angle =INITIAL_LEG_ANGLE;
	body_x = INITIAL_BODY_X;
	body_y = INITIAL_BODY_Y;
	body_z = INITIAL_BODY_Z;
	body_dir = INITIAL_BODY_DIR;
}


// OpenGL の設定 //

void InitGL(void)
{
	// 背景色の設定

	glClearColor(1.0, 1.0, 1.0, 0.0);

	// シェーディング方法の設定

	glShadeModel(GL_SMOOTH);

	// Zバッファによる隠面消去を有効にする。

	glEnable(GL_DEPTH_TEST);

	// 裏を向いているポリゴンを描かない

	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	// 法線を正規化する

	glEnable(GL_NORMALIZE);
}

// 光源の設定

void SetLight(void)
{
	GLfloat light_position0[] = {20.0, 50.0, 30.0, 1.0};
	GLfloat light_ambient0[] = {0.2, 0.2, 0.2, 1.0};
	GLfloat light_diffuse0[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat light_specular0[] = {1.0, 1.0, 1.0, 1.0};

	glLightfv(GL_LIGHT0, GL_POSITION, light_position0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular0);

	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
}

// 物体の色の設定

void SetMaterial(const double r, const double g, const double b, const double shininess = 50.0)
{
		const double ratio = 0.2;

	GLfloat mat_ambient[] = {r * ratio, g * ratio, b * ratio, 1.0};
	GLfloat mat_diffuse[] = {r, g, b, 1.0};
	GLfloat mat_specular[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat mat_shininess[] = {shininess};

	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
}

// 物体の描画

// パーツを描く

void DrawOneLeg(void)
{
	glPushMatrix();
	glTranslated(0.0, -LEG_LENGTH / 2, 0.0);
	glScaled(LEG_THICKNESS, LEG_LENGTH, LEG_THICKNESS);
	glutSolidCube(1.0);
	glPopMatrix();
}

// ティーポット（顔）を描く

void DrawTeapot(void)
{
	glPushMatrix(); // 一時的に座標系情報を保存

	glScalef(1.0f, 1.0f, -1.0f); // Teapotは面の表裏が一般的な設定となぜか逆
	glutSolidTeapot(LEG_LENGTH / 2);

	glPopMatrix(); // 保存してあった座標系情報を戻す
}

// キャラクタを描く

void DrawCharacter(const double x, const double y, const double z, const double angle, const double dir)
{
	glPushMatrix();
	glTranslated(x, y, z);
	glRotated(dir, 0.0, 1.0, 0.0);

	// 左足

	SetMaterial(0.1, 0.2, 1.0);
	glPushMatrix();
	glTranslated(0.0, 0.0, -LEG_THICKNESS/ 2);
	glRotated(angle / 2, 0.0, 0.0, 1.0);
	DrawOneLeg();
	glPopMatrix();

	// 右足

	SetMaterial(0.9, 0.4, 0.1);
	glPushMatrix();
	glTranslated(0.0, 0.0, LEG_THICKNESS / 2);
	glRotated(-angle / 2, 0.0, 0.0, 1.0);
	DrawOneLeg();
	glPopMatrix();

	// 胴

	SetMaterial(0.9, 0.9, 0.1);
	glPushMatrix();
	glTranslated(0.0, LEG_LENGTH, 0.0);
	DrawOneLeg();
	glPopMatrix();
	
	// 右腕

	SetMaterial(0.1, 0.2, 1.0);
	glPushMatrix();
	glTranslated(0.0, LEG_LENGTH, LEG_THICKNESS);
	glRotated(angle / 2, 0.0, 0.0, 1.0);
	DrawOneLeg();
	glPopMatrix();

	// 左腕

	SetMaterial(0.9, 0.4, 0.1);
	glPushMatrix();
	glTranslated(0.0, LEG_LENGTH, -LEG_THICKNESS);
	glRotated(-angle / 2, 0.0, 0.0, 1.0);
	DrawOneLeg();
	glPopMatrix();

	// 顔

	SetMaterial(0.4, 0.4, 0.4);
	glPushMatrix();
	glTranslated(0.0, LEG_LENGTH * 5 / 4, 0.0);
	DrawTeapot();
	glPopMatrix();
	

	glPopMatrix();
}

// 地面を描く

void DrawGround(void)
{
	const int num = 10;
	const double size = 10.0;

	glPushMatrix();
	glScaled(size, 1.0, size);
	glTranslated(-num / 2.0, 0.0, -num / 2.0);

	glNormal3d(0.0, 1.0, 0.0);
	glBegin(GL_QUADS);
	for (int i = 0; i < num; i++) {
		for (int j = 0; j < num; j ++) {
			if ((i + j) % 2 == 0) {
				SetMaterial(0.9, 0.9, 0.9);
			} else {
				SetMaterial(0.6, 0.6, 1.0);
			}
			glVertex3i(i, 0, j);
			glVertex3i(i, 0, (j + 1));
			glVertex3i((i + 1), 0, (j + 1));
			glVertex3i((i + 1), 0, j);
		}
	}
	glEnd();

	glPopMatrix();
}



// コールバック関数 //

// ウインドウ描画

void Display(void)
{
	// 画面をクリア

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// カメラの位置を設定

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(EYE_X, EYE_Y, EYE_Z, // カメラの位置
		TARGET_X, TARGET_Y, TARGET_Z, //注視点
		UP_X, UP_Y, UP_Z); // カメラ撮像面の上向き方向

	// 光源位置の設定

	SetLight();

	// 物体の配置

	glPushMatrix();

	DrawGround();
	DrawCharacter(body_x, body_y, body_z, leg_angle, body_dir);

	glPopMatrix();

	// バッファの入れ替え

	glutSwapBuffers();
}

// ウインドウリサイズ

void Reshape(int w, int h)
{
	window_width = w;
	window_height = h;

	glViewport(0, 0, window_width, window_height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(FIELD_OF_VIEW, window_width / (double) window_height, NEAR_CLIPPINT_LENGTH, FAR_CLIPPINT_LENGTH);

	glMatrixMode(GL_MODELVIEW);
}

// キー入力

void Keyboard(unsigned char key, int x, int y)
{
	if (key == 'q' || key == 3 || key == 27) { // 3: Ctrl-C, 27: ESC
		exit(0);
	} else if (key == 'r') {
		InitCharacterPosition();
		glutPostRedisplay();
	} else if (key == 'm') {
		body_dir -= ROT_ANGLE_VELOCITY;
	} else if (key == 'n') {
		body_dir += ROT_ANGLE_VELOCITY;
	}
}

// マウスボタン入力

void MouseButton(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		is_moving = 1 - is_moving;
	}
	if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN) {}
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {}
}


// 何も仕事がないときに呼ばれる

void Idle(void)
{
	if (is_moving) {
		counter++;

		switch (on_ground)
		{
		case 0: // left on ground
			leg_angle -= ROT_ANGLE_VELOCITY;
			body_x -= ((LEG_LENGTH * cos(M_PI * (90 - leg_angle / 2) / 180)) - (LEG_LENGTH * cos(M_PI * ( 90 - (leg_angle + ROT_ANGLE_VELOCITY) / 2) / 180))) * cos(M_PI * body_dir / 180);
			body_y += (LEG_LENGTH * sin(M_PI * (90 - leg_angle / 2) / 180)) - (LEG_LENGTH * sin(M_PI * ( 90 - (leg_angle + ROT_ANGLE_VELOCITY) / 2) / 180));
			body_z += ((LEG_LENGTH * cos(M_PI * (90 - leg_angle / 2) / 180)) - (LEG_LENGTH * cos(M_PI * ( 90 - (leg_angle + ROT_ANGLE_VELOCITY) / 2) / 180))) * sin(M_PI * body_dir / 180);
			if (leg_angle < -ANGLE_MAX) {
				on_ground = 1;
			}
			break;
		case 1: // right on ground
			leg_angle += ROT_ANGLE_VELOCITY;
			body_x += ((LEG_LENGTH * cos(M_PI * (90 - leg_angle / 2) / 180) - (LEG_LENGTH * cos(M_PI * (90 - (leg_angle - ROT_ANGLE_VELOCITY) / 2) / 180)))) * cos(M_PI * body_dir / 180);
			body_y += (LEG_LENGTH * sin(M_PI * (90 - leg_angle / 2) / 180) - (LEG_LENGTH * sin(M_PI * (90 - (leg_angle - ROT_ANGLE_VELOCITY) / 2) / 180)));
			body_z -= ((LEG_LENGTH * cos(M_PI * (90 - leg_angle / 2) / 180) - (LEG_LENGTH * cos(M_PI * (90 - (leg_angle - ROT_ANGLE_VELOCITY) / 2) / 180)))) * sin(M_PI * body_dir / 180);
			if (leg_angle > ANGLE_MAX) {
				on_ground = 0;
			}
			break;
		default:
			break;
		}
		
		glutPostRedisplay();
	}
}


// main //

int main(int argc, char **argv)
{
	// 変数の初期化

	window_width = WINDOW_WIDTH;
	window_height = WINDOW_HEIGHT;
	counter = 0;
	is_moving = 1;

	InitCharacterPosition();

	// GLUTの初期化

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(window_width, window_height);
	glutInitWindowPosition(WINDOW_POSITION_X, WINDOW_POSITION_Y);
	glutCreateWindow(argv[0]);

	// コールバック関数の設定

	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(MouseButton);
	glutIdleFunc(Idle);

	// OpenGLの各種初期設定

	InitGL();

	// GLUTに制御を移管

	glutMainLoop();

	return 0;
}
