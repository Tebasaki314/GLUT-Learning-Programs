// arm2.cpp
//
// 2自由度のロボットハンド

#include <GL/glut.h>

#include <cstdio>
#include <cstdlib>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979
#endif



// 定数・変数の宣言 ///////////////////////////////////////////////////////////

// ウィンドウのサイズ

const int WINDOW_WIDTH = 640;
const int WINDOW_HEIGHT = 480;
const int WINDOW_POSITION_X = 0;
const int WINDOW_POSITION_Y = 0;

// 透視投影変換のパラメータ

const double NEAR_CLIPPING_LENGTH = 1.0;
const double FAR_CLIPPING_LENGTH = 10000.0;

const double FIELD_OF_VIEW = 40.0;

// カメラの位置と注視点

const double EYE_X = 0.0;
const double EYE_Y = 15.0;
const double EYE_Z = 50.0;

const double TARGET_X = 0.0;
const double TARGET_Y = 10.0;
const double TARGET_Z = 0.0;

const double UP_X = 0.0;
const double UP_Y = 1.0;
const double UP_Z = 0.0;

// 物体関連

const double ARM_LENGTH1 = 10.0;
const double ARM_LENGTH2 = 12.0;
const double ARM_LENGTH3 = 8.0;
const double ARM_THICKNESS = 1.0;

const double TARGET_RADIUS = 2.0;



// グローバル変数

int window_width;
int window_height;
int counter;
int is_moving;

int mouse_button_down;

double arm_angle1, arm_angle2, arm_angle3;
double base_x, base_y, base_z;

double target_x, target_y, target_z;


// オブジェクトの初期化 ///////////////////////////////////////////////////////

void InitArmPosition(void)
{
	arm_angle1 = 30.0;
	arm_angle2 = 120.0;
	arm_angle3 = 30.0;
	base_x = 0.0;
	base_y = 0.0;
	base_z = 0.0;

	target_x = 0.0;
	target_y = (ARM_LENGTH1 + ARM_LENGTH2) * 0.8;
	target_z = 0.0;
}



// OpenGLの設定 ///////////////////////////////////////////////////////////////

// 初期設定

void InitGL(void)
{
	// 背景色の設定

	glClearColor(1.0, 1.0, 1.0, 1.0);

	// シェーディング方法の設定

	glShadeModel(GL_SMOOTH);

	// Zバッファによる隠面消去を有効にする

	glEnable(GL_DEPTH_TEST);

	// 裏を向いているポリゴンを描かない

	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	// 法線を正規化(長さ1に)する

	glEnable(GL_NORMALIZE);
}

// 光源の設定

void SetLight(void)
{
	GLfloat light_position0[] = {-20.0, 50.0, 40.0, 0.0};
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

void SetMaterial(const double r, const double g, const double b,
	const double shininess = 50.0)
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



// 物体の描画 /////////////////////////////////////////////////////////////////

// 腕を描く

void DrawOneArm(const double length, const double thickness)
{
	glPushMatrix();
	glTranslated(length / 2, 0.0, 0.0);
	glScaled(length, thickness, thickness);
	glutSolidCube(1.0);
	glPopMatrix();
}

// アーム全体を描く

void DrawArm(void)
{
	glPushMatrix();

	// 土台の位置を移動

	glTranslated(base_x, base_y, base_z);

	// ジョイント1

	SetMaterial(0.6, 0.6, 0.6);
	glutSolidSphere(ARM_THICKNESS, 16, 8);

	// アーム1

	SetMaterial(0.1, 0.2, 1.0);
	glRotated(arm_angle1, 0.0, 0.0, 1.0);
	DrawOneArm(ARM_LENGTH1, ARM_THICKNESS);

	// アームの長さだけ座標系を移動

	glTranslated(ARM_LENGTH1, 0.0, 0.0);

	// ジョイント2

	SetMaterial(0.6, 0.6, 0.6);
	glutSolidSphere(ARM_THICKNESS, 16, 8);

	// アーム2

	SetMaterial(0.9, 0.2, 0.1);
	glRotated(arm_angle2, 0.0, 0.0, 1.0);
	DrawOneArm(ARM_LENGTH2, ARM_THICKNESS);

	// アームの長さだけ座標系を移動

	glTranslated(ARM_LENGTH2, 0.0, 0.0);

	// ジョイント3

	SetMaterial(0.6, 0.6, 0.6);
	glutSolidSphere(ARM_THICKNESS, 16, 8);

	// アーム3

	SetMaterial(0.2, 0.9, 0.1);
	glRotated(arm_angle3, 0.0, 0.0, 1.0);
	DrawOneArm(ARM_LENGTH3, ARM_THICKNESS);

	glPopMatrix();
}

// ターゲットを描く

void DrawTarget(void)
{
	glPushMatrix();
	glTranslated(target_x, target_y, target_z);

	SetMaterial(0.2, 1.0, 0.2);
	glutSolidSphere(TARGET_RADIUS, 16, 8);
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
		for (int j = 0; j < num; j++) {
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



// スクリーン座標からオブジェクト座標系への変換 ///////////////////////////////

GLdouble modelview_matrix[16];
GLdouble projection_matrix[16];
GLint viewport[4];

// 現在のオブジェクト座標系を保存

void SaveCurrentTransform(void)
{
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview_matrix);
	glGetDoublev(GL_PROJECTION_MATRIX, projection_matrix);
	glGetIntegerv(GL_VIEWPORT, viewport);
}

// 任意の平面との交点を求める

int UnProject(const int x, const int y,
	const double a, const double b, const double c, const double d,
	double *p_obj_x, double *p_obj_y, double *p_obj_z)
{
	// 視体積の両端の点を求める

	double x0, y0, z0;
	double x1, y1, z1;
	gluUnProject(x, y, 0.0, modelview_matrix, projection_matrix, viewport,
		&x0, &y0, &z0);
	gluUnProject(x, y, 1.0, modelview_matrix, projection_matrix, viewport,
		&x1, &y1, &z1);

	// 二点(x0,y0,z0)と(x1,y1,z1)を結ぶ直線と，
	// 平面ax+by+cz+d=0との交点を求める

	double k = a * (x1 - x0) + b * (y1 - y0) + c * (z1 - z0);

	// もし，kが0だったら，直線は平面と交差しないので，解は無い．

	int is_error;

	if (k != 0) {
		double t = -(a * x0 + b * y0 + c * z0 + d) / k;
		*p_obj_x = (1.0 - t) * x0 + t * x1;
		*p_obj_y = (1.0 - t) * y0 + t * y1;
		*p_obj_z = (1.0 - t) * z0 + t * z1;
		is_error = 0;
	} else {
		is_error = 1;
	}

	return is_error;
}

double toRadian(double x){
	return x * M_PI / 180.0;
}

double toDegree(double x){
	return x * 180.0 / M_PI;
}

// 逆運動学に基づいてアームの姿勢を制御 ///////////////////////////////////////

void UpdateArmStatus(void)
{
	// 現在のエンドエフェクタの位置を計算する

	double endEffecter_x = ARM_LENGTH1 * cos(toRadian(arm_angle1)) + ARM_LENGTH2 * cos(toRadian(arm_angle1 + arm_angle2)) + ARM_LENGTH3 * cos(toRadian(arm_angle1 + arm_angle2 + arm_angle3));
	double endEffecter_y = ARM_LENGTH1 * sin(toRadian(arm_angle1)) + ARM_LENGTH2 * sin(toRadian(arm_angle1 + arm_angle2)) + ARM_LENGTH3 * sin(toRadian(arm_angle1 + arm_angle2 + arm_angle3));

	// 現在の位置から目標の位置へのベクトルを求める

	double endToTarget_x = target_x - endEffecter_x;
	double endToTarget_y = target_y - endEffecter_y;

	// エンドエフェクタの変位を計算する
	double endToTarget_dx, endToTarget_dy;
	double length = sqrt(endToTarget_x * endToTarget_x + endToTarget_y * endToTarget_y);

	if (length > 2.0){
		endToTarget_dx = (endToTarget_x / length) * 2.0;
		endToTarget_dy = (endToTarget_y / length) * 2.0;
	} else {
		endToTarget_dx = endToTarget_x;
		endToTarget_dy = endToTarget_y;
	}
	
	// ヤコビアンを計算する

	double ja[2][3];
	ja[0][0] = -ARM_LENGTH1 * sin(toRadian(arm_angle1)) - ARM_LENGTH2 * sin(toRadian(arm_angle1 + arm_angle2)) - ARM_LENGTH3 * sin(toRadian(arm_angle1 + arm_angle2 + arm_angle3));
	ja[0][1] = -ARM_LENGTH2 * sin(toRadian(arm_angle1 + arm_angle2)) - ARM_LENGTH3 * sin(toRadian(arm_angle1 + arm_angle2 + arm_angle3));
	ja[0][2] = -ARM_LENGTH3 * sin(toRadian(arm_angle1 + arm_angle2 + arm_angle3));
	ja[1][0] = ARM_LENGTH1 * cos(toRadian(arm_angle1)) + ARM_LENGTH2 * cos(toRadian(arm_angle1 + arm_angle2)) + ARM_LENGTH3 * cos(toRadian(arm_angle1 + arm_angle2 + arm_angle3));
	ja[1][1] = ARM_LENGTH2 * cos(toRadian(arm_angle1 + arm_angle2)) + ARM_LENGTH3 * cos(toRadian(arm_angle1 + arm_angle2 + arm_angle3));
	ja[1][2] = ARM_LENGTH3 * cos(toRadian(arm_angle1 + arm_angle2 + arm_angle3));
	
	// ヤコビアンの擬似逆行列を求める

	// - ヤコビアンの転置行列を求める
	double jaT[3][2];
	for(int i = 0; i < 3; i++){
		for(int j = 0; j < 2; j++){
			jaT[i][j] = ja[j][i];
		}
	}
	
	double jajaT[2][2];

	for(int i = 0; i < 2; i++){
		for(int j = 0; j < 2; j++){
			jajaT[i][j] = ja[i][0] * jaT[0][j] + ja[i][1] * jaT[1][j] + ja[i][2] * jaT[2][j];
		}
	}
	
	// - jajaTの逆行列を求める

	double det_jajaT = jajaT[0][0] * jajaT[1][1] - jajaT[0][1] * jajaT[1][0];
	double jajaT_inv[2][2];

	if (det_jajaT == 0) {
		// 逆行列が存在しないならば
		jajaT_inv[0][0] = 0.0;
		jajaT_inv[0][1] = 0.0;
		jajaT_inv[1][0] = 0.0;
		jajaT_inv[1][1] = 0.0;
	} else {
		// 逆行列が存在するならば
		jajaT_inv[0][0] = jajaT[1][1];
		jajaT_inv[0][1] = -jajaT[0][1];
		jajaT_inv[1][0] = -jajaT[1][0];
		jajaT_inv[1][1] = jajaT[0][0];

		for(int i = 0; i < 2; i++){
			for(int j = 0; j < 2; j++){
				jajaT_inv[i][j] /= det_jajaT;
			}
		}
	}

	// - ヤコビアンの擬似逆行列を求める
	double ja_sharp[3][2];
	for(int i = 0; i < 3; i++){
		for(int j = 0; j < 2; j++){
			ja_sharp[i][j] = jaT[i][0] * jajaT_inv[0][j] + jaT[i][1] * jajaT_inv[1][j];
		}
	}

	// 角度の変化量を計算する
	
	double d_arm_angle_1 = ja_sharp[0][0] * endToTarget_dx + ja_sharp[0][1] * endToTarget_dy;
	double d_arm_angle_2 = ja_sharp[1][0] * endToTarget_dx + ja_sharp[1][1] * endToTarget_dy;
	double d_arm_angle_3 = ja_sharp[2][0] * endToTarget_dx + ja_sharp[2][1] * endToTarget_dy;
	
	// 現在の角度に加える

	arm_angle1 += toDegree(d_arm_angle_1);
	arm_angle2 += toDegree(d_arm_angle_2);
	arm_angle3 += toDegree(d_arm_angle_3);
}



// コールバック関数 ///////////////////////////////////////////////////////////

void Display(void)
{
	// 画面をクリア

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// カメラの位置を設定

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(EYE_X, EYE_Y, EYE_Z, // カメラの位置
		TARGET_X, TARGET_Y, TARGET_Z, // 注視点
		UP_X, UP_Y, UP_Z); // カメラ撮像面の上向き方向

	// ここでマウスでのターゲット位置指定のために，OpenGLの座標変換情報を保存

	SaveCurrentTransform();

	// 光源位置の設定

	SetLight();

	// 物体の配置

	glPushMatrix();

	DrawGround();
	DrawArm();
	DrawTarget();

	glPopMatrix();

	// バッファの入れ替え

	glutSwapBuffers();
}

// ウィンドウリサイズ

void Reshape(int w, int h)
{
	window_width = w;
	window_height = h;

	glViewport(0, 0, window_width, window_height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(FIELD_OF_VIEW, window_width / (double) window_height,
		NEAR_CLIPPING_LENGTH, FAR_CLIPPING_LENGTH);

	glMatrixMode(GL_MODELVIEW);
}

// キー入力

void Keyboard(unsigned char key, int x, int y)
{
	if (key == 'q' || key == 3 || key == 27) { // 3: Ctrl-C, 27: ESC
		exit(0);
	} else if (key == 'r') {
		InitArmPosition();
	} else if (key == 'a') {
		arm_angle2 += 1.0;
	} else if (key == 's') {
		arm_angle2 -= 1.0;
	} else if (key == 'z') {
		arm_angle1 += 1.0;
	} else if (key == 'x') {
		arm_angle1 -= 1.0;
	} else if (key == ' ') {
		is_moving = 1 - is_moving;
	}
	glutPostRedisplay();
}

// マウスボタン入力

void MouseButton(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			mouse_button_down = 1;
			UnProject(x, (window_height - 1) - y, // スクリーン座標
				0.0, 0.0, 1.0, -base_z, // 平面 z - base_z = 0
				&target_x, &target_y, &target_z); // オブジェクト座標
			glutPostRedisplay();
		} else {
			mouse_button_down = 0;
		}
	}
	if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN) {}
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {}
}

// マウスドラッグ

void MouseMotion(int x, int y)
{
	if (mouse_button_down) {
		UnProject(x, (window_height - 1) - y,
			0.0, 0.0, 1.0, -base_z,
			&target_x, &target_y, &target_z);
		glutPostRedisplay();
	}
}

// 何も仕事が無いときに呼ばれる

void Idle(void)
{
	if (is_moving) {
		UpdateArmStatus();
		glutPostRedisplay();
	}
}



// mainはここから /////////////////////////////////////////////////////////////

int main(int argc, char **argv)
{
	// 変数の初期化

	window_width = WINDOW_WIDTH;
	window_height = WINDOW_HEIGHT;
	counter = 0;
	is_moving = 1;
	mouse_button_down = 0;

	InitArmPosition();

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
	glutMotionFunc(MouseMotion);
	glutIdleFunc(Idle);

	// OpenGLの各種初期設定

	InitGL();

	// GLUTに制御を移管

	glutMainLoop();

	// ここには到達しない

	return 0;
}
