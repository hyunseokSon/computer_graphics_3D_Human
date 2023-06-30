#include <Windows.h>
#include <iostream>
#include <GL/glut.h>
#include <GL/GL.h>
#include <GL/GLU.h>
#include <math.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h" // �̹��� �ε� ���̺귯��

#define PI 3.141592
#define ZOOM_IN_LIMIT 2
#define ZOOM_SPEED 0.3
#define START_ZOOM 12

// WIDTH, HEIGHT: �ʱ� ������ â ũ��
#define WIDTH 400
#define HEIGHT 400

// ���콺�� ȭ�� �����̴� ����
// 0.01: ���� ������ ~ 0.1: ���� ������
#define MOVE 0.04

int beforeX, beforeY;  // ���콺 ���۽� ���
float vTheta, hTheta;  // gluLookat������ ī�޶� ��ġ�� ����� �� ���
float radius;  // ī�޶� ��ġ�� �� �ִ� ���� �������� ����, zoom in/out ���� ���. (���� �������� �ٸ� �� ��, �ø� �� �ƿ�)
float x, y, z;  // gluLookat ������ ī�޶� ��ġ (eyeX, eyeY, eyeZ)

void InitLight();
void MyReshape(int, int);
void MyKeyboard(unsigned char, int, int);
void MyMouseClick(int, int, int, int);
void MyMouseMove(GLint, GLint);
void MyDisplay();

void createCylinder(GLfloat, GLfloat, GLfloat, GLfloat, GLfloat);
void createRing(GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat);
void createCircle(GLfloat, GLfloat, GLfloat);
void createSphere(GLfloat);
void createHalfSphere(GLfloat, GLfloat, GLfloat, GLfloat);

void drawModel(); // ��ü �� �׸��� �Լ�

void draw_head(); // �Ӹ� �׸��� �Լ�
void draw_leftEar();         // ���� �͸���
void draw_rightEar();        // ������ �͸���
void draw_leftEye();         // ���ʴ�
void draw_leftEyePupil();    // ���� ����
void draw_rightEye();        // ������ ��
void draw_rightEyePupil();   // ������ ����
void draw_leftFace();        // ���� ��
void draw_rightFace();       // ������ ��
void draw_leftEyeBrow();     // ���� ����
void draw_rightEyeBrow();    // ������ ����
void draw_middle();          // ����
void draw_rightArm();        // ������ ��
void draw_leftArm();         // ���� ��
void draw_rightWrist();      // ������ �ո�
void draw_leftWrist();       // ���� �ո�
void draw_rightHand();       // ������ ��
void draw_leftHand();        // ���� ��
void draw_rightLeg();        // ������ �ٸ�
void draw_leftLeg();         // ���� �ٸ�
void draw_righttSock();      // ������ �縻
void draw_leftSock();        // ���� �縻
void draw_rightShoe();       // ������ �Ź�
void draw_leftShoe();        // ���� �Ź�


void InitLight() {
    printf("InitLight\n");

    glClearColor(0.5, 0.5, 0.5, 0.0); // ���� ���� (R, G, B, A)

    // ��迡�� ���� �ε巴�� �̾������� �Ѵ�.
    glShadeModel(GL_SMOOTH);

    // Depth�� ���� ���� �ڷ� �и���, Depth�� ū ���� ������ �������� ����
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_LIGHTING);  // ���� ����ϰڴ� ����
    glEnable(GL_LIGHT0);  // LIGHT 0 ~ 7 ���� �� 8���� light�� ��� ����. �츮�� light 0���� 1���� ���� ���
    glEnable(GL_COLOR_MATERIAL);  // ��ü�� ���� ���� �������� ���� (�̰� ���ٸ� ���� ����θ� ��ü�� ����)

    // ������ �л걤, �ݻ籤, �ֺ����� ����
    GLfloat light_specular[] = { .5, .5, .5, 0.5 };
    GLfloat light_diffuse[] = { .5, .5, .5, 0.5 };
    GLfloat light_ambient[] = { .7, .7, .7, 0.5 };
    GLfloat light_position[] = { -3, 6 ,3.0, 0.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);  // �� ��ġ ����
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);  // �л걤 (��ü�� �ݻ�Ǿ� ���� ���������� ����)
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);  // �ݻ籤 (��ü�� �ݻ�Ǿ� �Ի簢, �ݻ簢�� ������ ���� Ƣ����� ����)
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);  // �ֺ��� (�츮 ������ �����ϰ� �����ϴ� ��)

    // ��ü�� �л걤, �ݻ籤, �ֺ����� ����
    GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_ambient[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_shininess[] = { 15.0 };
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);  // �л걤
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);  // �ݻ籤
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);  // �ֺ���
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);  // ��ü�� ��¦�̴� ������ ����
}

void MyReshape(int w, int h) {
    printf("MyReshape\n");

    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(50, (GLfloat)w / (GLfloat)h, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    radius = START_ZOOM;
    hTheta = 0.0;
    vTheta = 0.0;
}

// Ű����
void MyKeyboard(unsigned char key, int x, int y) {
    printf("MyKeyboard\n");
    switch (key) {
        //case 'q': case 'Q':
        //   exit(0);
        //   break;
    case 'a':
        if (radius > ZOOM_IN_LIMIT) {
            radius -= ZOOM_SPEED;
        }
        break;
    case 'z':
        radius += ZOOM_SPEED;
        break;
    }
    glutPostRedisplay();
}

// ���콺 Ŭ���� ȣ��Ǵ� �Լ�
void MyMouseClick(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        beforeX = x;
        beforeY = y;
    }
}

// ���콺�� Ŭ�� �� ���¿��� ������ �� ȣ��Ǵ� �Լ�
void MyMouseMove(GLint x, GLint y) {
    printf("MyMouseMove\n");
    printf("x: %d, y: %d", x, y);
    if (beforeX > x) {  // ���콺�� �������� �巡��
        hTheta += MOVE;
        if (beforeY > y)  // ���콺�� �Ʒ��� �巡��
            vTheta += MOVE * 2;
        else if (beforeY < y)  // ���콺�� ���� �巡��
            vTheta -= MOVE * 2;
    }
    else if (beforeX < x) {  // ���콺�� ���������� �巡��
        hTheta -= MOVE;
        if (beforeY > y)
            vTheta += MOVE * 2;
        else if (beforeY < y)
            vTheta -= MOVE * 2;
    }

    beforeX = x;
    beforeY = y;

    glutPostRedisplay();

    // 360���� �Ѱ� �ȴٸ� ��ü�� Ȯ ���� �ʵ��� ó��(cf: 360�� = 2 * PI ����)
    if (hTheta > 2.0 * PI)
        hTheta -= (2.0 * PI);
    else if (hTheta < 0.0)
        hTheta += (2.0 * PI);
}

void createCylinder(GLfloat centerx, GLfloat centery, GLfloat centerz, GLfloat radius, GLfloat h)
{
    /* function createCyliner()
    ������� �߽� x,y,z��ǥ, ������, ���̸� �޾� ������� �����ϴ� �Լ�(+z�������� ������ �þ)
    centerx : ����� ���� �߽� x��ǥ
    centery : ����� ���� �߽� y��ǥ
    centerz : ����� ���� �߽� z��ǥ
    radius : ������� ������
    h : ������� ����
    */
    GLfloat x, y, angle;

    glBegin(GL_TRIANGLE_FAN);           //������� ����
    glNormal3f(0.0f, 0.0f, -1.0f);
    glColor3ub(139, 69, 19);
    glVertex3f(centerx, centery, centerz);


    for (angle = 0.0f; angle < (2.0f * PI); angle += (PI / 8.0f))
    {
        x = centerx + radius * sin(angle);
        y = centery + radius * cos(angle);
        glNormal3f(0.0f, 0.0f, -1.0f);
        glVertex3f(x, y, centerz);
    }
    glEnd();

    glBegin(GL_QUAD_STRIP);            //������� ����
    for (angle = 0.0f; angle < (2.0f * PI); angle += (PI / 8.0f))
    {
        x = centerx + radius * sin(angle);
        y = centery + radius * cos(angle);
        glNormal3f(sin(angle), cos(angle), 0.0f);
        glVertex3f(x, y, centerz);
        glVertex3f(x, y, centerz + h);
    }
    glEnd();

    glBegin(GL_TRIANGLE_FAN);           //������� �ظ�
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(centerx, centery, centerz + h);
    for (angle = (2.0f * PI); angle > 0.0f; angle -= (PI / 8.0f))
    {
        x = centerx + radius * sin(angle);
        y = centery + radius * cos(angle);
        glNormal3f(0.0f, 0.0f, 1.0f);
        glVertex3f(x, y, centerz + h);
    }
    glEnd();
}

void createRing(GLfloat centerx, GLfloat centery, GLfloat centerz, GLfloat radius, GLfloat h, GLfloat thick)
{
    /* function createRing()
    ���� �߽� x,y,z��ǥ, ������, ����, �β��� �޾� ���� �����ϴ� �Լ�
    centerx : ���� �߽� x��ǥ
    centery : ���� �߽� y��ǥ
    centerz : ���� �߽� z��ǥ
    radius : ���� ������
    h  : ���� ����
    thick : ���� �β�
    */
    GLfloat x, y, angle;

    glColor3ub(148, 0, 211);
    glBegin(GL_QUAD_STRIP);           //���� ����
    for (angle = (2.0f * PI); angle > 0.0f; angle -= (PI / 8.0f))
    {
        x = centerx + radius * sin(angle);
        y = centery + radius * cos(angle);
        glNormal3f(0.0f, 0.0f, -1.0f);
        glVertex3f(x, y, centerz);
        x = centerx + (radius - thick) * sin(angle);
        y = centery + (radius - thick) * cos(angle);
        glVertex3f(x, y, centerz);
    }
    glEnd();

    int color = 0;
    glBegin(GL_QUAD_STRIP);            //���� �ٱ��� ����
    for (angle = 0.0f; angle < (2.0f * PI); angle += (PI / 8.0f))
    {
        x = centerx + radius * sin(angle);
        y = centery + radius * cos(angle);
        glNormal3f(sin(angle), cos(angle), 0.0f);
        glVertex3f(x, y, centerz);
        glVertex3f(x, y, centerz + h);
        color++;
    }
    glEnd();

    glColor3ub(148, 0, 211);
    glBegin(GL_QUAD_STRIP);            //���� ���� ����
    for (angle = (2.0f * PI); angle > 0.0f; angle -= (PI / 8.0f))
    {
        x = centerx + (radius - thick) * sin(angle);
        y = centery + (radius - thick) * cos(angle);
        glNormal3f(-sin(angle), -cos(angle), 0.0f);
        glVertex3f(x, y, centerz);
        glVertex3f(x, y, centerz + h);
    }
    glEnd();

    glBegin(GL_QUAD_STRIP);           //������� �ظ�
    for (angle = 0.0f; angle < (2.0f * PI); angle += (PI / 8.0f))
    {
        x = centerx + radius * sin(angle);
        y = centery + radius * cos(angle);
        glNormal3f(0.0f, 0.0f, 1.0f);
        glVertex3f(x, y, centerz + h);
        x = centerx + (radius - thick) * sin(angle);
        y = centery + (radius - thick) * cos(angle);
        glVertex3f(x, y, centerz + h);
    }
    glEnd();
}

void createCircle(GLfloat r, GLfloat pos, GLfloat move) {
    GLfloat centerx = 0, centery = 0, centerz = 0;
    GLfloat x, y, angle;

    glBegin(GL_TRIANGLE_FAN);
    glTexCoord2f(0.0f, 0.0f);
    glNormal3f(0.0f, 0.0f, pos);
    glVertex3f(centerx, centery, centerz + move);
    for (angle = (2.0f * PI); angle > 0.0f; angle -= (PI / 8.0f))
    {
        x = centerx + r * sin(angle);
        y = centery + r * cos(angle);
        glTexCoord2f(x, y);
        glNormal3f(0.0f, 0.0f, pos);
        glVertex3f(x, y, centerz + move);
    }
    glEnd();
}

void createSphere(GLfloat r)
{
    GLUquadricObj* sphere = gluNewQuadric(); // Quadrics obj ����

    gluQuadricDrawStyle(sphere, GLU_FILL);
    gluQuadricNormals(sphere, GLU_SMOOTH); // �������� �� ����, GLU_FLAT�� ���� ���ΰ�ó�� ���̵���, GL_SMOOTH�� ���� �ε巴��
    gluQuadricOrientation(sphere, GLU_OUTSIDE);
    gluQuadricTexture(sphere, GL_TRUE);
    glPolygonMode(GL_FRONT, GL_FILL);

    gluSphere(sphere, r, 50, 50); // �������� r�� ���� �����ǰ�, 50�� ���� ������, ������ ���� ���� �ǹ��Ѵ�.
}

void createHalfSphere(GLfloat x, GLfloat y, GLfloat z, GLfloat radius)
{
    /* function createHalfSphere()
    ���� �߽� x, y, z ��ǥ�� �޾� �ݱ��� ����� �Լ�
    x : �ݱ��� �߽� x ��ǥ
    y : �ݱ��� �߽� y ��ǥ
    z : �ݱ��� �߽� z ��ǥ
    raidus : �ݱ��� ������
    */
    GLfloat angley;  //y�� ���� ���ϱ� ���� ����
    GLfloat nexty;  //���� y�� ���� ���ϱ� ���� ����
    GLfloat anglex;  //x, y�� ���� ���ϱ� ���� ����

    glColor3ub(0, 0, 255);  //�ݱ��� �� ����
    glBegin(GL_QUAD_STRIP);
    for (angley = 0.0f; angley <= (0.5f * PI); angley += ((0.5f * PI) / 8.0f))  //�ݱ��� �׷��� �ϹǷ� 0.5����
    {
        y = radius * sin(angley);     //y�� �� ���
        nexty = angley + ((0.5f * PI) / 8.0f);  //���� angley�� ����
        for (anglex = 0.0f; anglex < (2.0f * PI); anglex += (PI / 8.0f))
        {
            x = radius * cos(angley) * sin(anglex);
            z = radius * cos(angley) * cos(anglex);
            glNormal3f(-cos(angley) * sin(anglex), -sin(angley), -cos(angley) * cos(anglex)); //�ݱ��� �������� normal ���� ����
            glVertex3f(x, y, z);

            x = radius * cos(nexty) * sin(anglex);
            z = radius * cos(nexty) * cos(anglex);
            glNormal3f(-cos(nexty) * sin(anglex), -sin(nexty), -cos(nexty) * cos(anglex));
            glVertex3f(x, radius * sin(nexty), z);
        }
    }
    glEnd();
}

void getTexture(char* filename) {
    // �̹��� �ε�
    int width, height, numChannels;
    unsigned char* image = stbi_load(filename, &width, &height, &numChannels, 0);
    if (!image) {
        printf("image file cannot open...\n");
        return;
    }

    // �ؽ�ó ���� �� ����
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(image);

    // �ؽ�ó ���� ����
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glBindTexture(GL_TEXTURE_2D, textureID);
}



void MyDisplay() {
    printf("MyDisplay\n");

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluPerspective(50, 1.0, 1.0, 100.0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // ���콺 �巡�׸� ���� �������� �̵��� ����(hTheta), �������� �̵��� ����(vTheta)�� ����Ͽ�
    // ī�޶��� ��ġ�� up vector�� ����
    x = radius * cos(hTheta) * cos(vTheta);
    y = radius * sin(hTheta) * cos(vTheta);
    z = -radius * sin(vTheta);
    gluLookAt(x, y, z, 0.0, 0.0, 0.0, 0.0, 0.0, cos(vTheta));

    drawModel(); // ĳ���� �׸���

    glFlush();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("OpenGL");

    // ���� ����
    InitLight();

    // Ű���� �̺�Ʈ �߻��� MyKeyboard ȣ���ϵ��� �ݹ� ���
    glutKeyboardFunc(MyKeyboard);

    // ȭ�� ǥ��
    glutDisplayFunc(MyDisplay);

    // ���콺 Ŭ����: MyMouseClick ȣ���ϵ��� �ݹ� ���
    // ���콺 Ŭ���� ���¿��� �����̸�: MyMouseMove ȣ���ϵ��� �ݹ� ���
    glutMouseFunc(MyMouseClick);
    glutMotionFunc(MyMouseMove);

    // Reshape�̺�Ʈ �߻��� MyReshape ȣ���ϵ��� �ݹ� ���
    // 1. ó�� �����츦 �� ��
    // 2. ������ ��ġ�� �ű� ��
    // 3. ������ ũ�⸦ ������ ��
    glutReshapeFunc(MyReshape);

    glutMainLoop();
    return 0;
}

/** ĳ���� �׸��� **/
void drawModel() {
    /** �Ӹ� �κ� ���� ***/
    draw_head();            // �Ӹ�
    draw_leftEar();         // ���� �͸���
    draw_rightEar();        // ������ �͸���
    draw_leftEye();         // ���ʴ�
    draw_leftEyePupil();    // ���� ����
    draw_rightEye();        // ������ ��
    draw_rightEyePupil();   // ������ ����
    draw_leftFace();        // ���� ��
    draw_rightFace();       // ������ ��
    draw_leftEyeBrow();     // ���� ����
    draw_rightEyeBrow();    // ������ ����

    /*** ���� �κ� ���� ****/
    draw_middle();          // ����
    draw_rightArm();        // ������ ��
    draw_leftArm();         // ���� ��
    draw_rightWrist();      // ������ �ո�
    draw_leftWrist();       // ���� �ո�
    draw_rightHand();       // ������ ��
    draw_leftHand();        // ���� ��
    draw_rightLeg();        // ������ �ٸ�
    draw_leftLeg();         // ���� �ٸ�
    draw_righttSock();      // ������ �縻
    draw_leftSock();        // ���� �縻
    draw_rightShoe();       // ������ �Ź�
    draw_leftShoe();        // ���� �Ź�
}

void draw_head() {
    char imageName[30] = "image/face.bmp";

    glPushMatrix(); {
        getTexture(imageName);

        glColor3f(0.901f, 0.69f, 0.545f);
        glTranslatef(0.0, 0.0, 1.8);
        glRotatef(60, 0.0, 0.0, 1.0);
        glScalef(1.0, 1.0, 1.0);

        // �� �׸���
        GLUquadricObj* sphere = gluNewQuadric(); // Quadrics obj ����
        gluQuadricDrawStyle(sphere, GLU_FILL);
        gluQuadricNormals(sphere, GLU_SMOOTH);
        gluQuadricOrientation(sphere, GLU_OUTSIDE);
        gluQuadricTexture(sphere, GL_TRUE);
        gluSphere(sphere, 2.0f, 50, 50); // �������� 2.0�� ���� �����ϰ� �ؽ�ó�� �����մϴ�.
        gluDeleteQuadric(sphere); // Quadrics obj ����

        // �ؽ�ó ���� ��Ȱ��ȭ
        glBindTexture(GL_TEXTURE_2D, 0);

        // ������ ���� �ʱ�ȭ
        glDisable(GL_TEXTURE_2D);
    }
    glPopMatrix();
}

/** ���� �͸��� **/
void draw_leftEar() {
    glPushMatrix(); {
        glTranslatef(0.0, -1.6, 2.0);
        glRotatef(180, 0, 0, 1);
        createHalfSphere(0.0, 0.0, 0.0, 1.0);
    }
    glPopMatrix();
}

/** ������ �͸��� **/
void draw_rightEar() {
    glPushMatrix(); {
        glTranslatef(0.0, 1.6, 2.0);
        createHalfSphere(0.0, 0.0, 0.0, 1.0);
    }
    glPopMatrix();
}

/** ���� �� **/
void draw_leftEye() {
    glPushMatrix(); {
        glColor3f(0, 0, 0);
        glTranslatef(1.9, -0.5, 2.0);
        glScalef(1.0, 1.0, 2.0);
        createSphere(0.17);
    }
    glPopMatrix();
}

/** ���� ���� **/
void draw_leftEyePupil() {
    glPushMatrix(); {
        glColor3f(255, 255, 255);
        glTranslatef(1.99, -0.43, 2.05);
        glScalef(1.0, 1.0, 1.7);
        createSphere(0.1);
    }
    glPopMatrix();
}

/** ������ �� **/
void draw_rightEye() {
    glPushMatrix(); {
        glColor3f(0, 0, 0);
        glTranslatef(1.9, 0.5, 2.0);
        glScalef(1.0, 1.0, 2.0);
        createSphere(0.16);
    }
    glPopMatrix();
}

/** ������ ���� **/
void draw_rightEyePupil() {
    glPushMatrix(); {
        glColor3f(255, 255, 255);
        glTranslatef(1.99, 0.57, 2.05);
        glScalef(1.0, 1.0, 1.7);
        createSphere(0.1);
    }
    glPopMatrix();
}

/** ���� �� **/
void draw_leftFace() {
    glPushMatrix(); {
        glColor3f(255, 0, 0);
        glTranslatef(1.7, -1.1, 1.3);
        glScalef(0.9, 2.0, 1.0);
        createSphere(0.18);
    }
    glPopMatrix();
}

/** ������ �� **/
void draw_rightFace() {
    glPushMatrix(); {
        glColor3f(255, 0, 0);
        glTranslatef(1.7, 1.1, 1.3);
        glScalef(0.9, 2.0, 1.0);
        createSphere(0.18);
    }
    glPopMatrix();
}

/** ���� ���� **/
void draw_leftEyeBrow() {
    {
        glPushMatrix(); {

            glColor3f(0, 0, 0);
            glTranslatef(1.9, -1.2, 2.5);
            glRotatef(45, 1.0, 0.0, 0.0);
            glScalef(1.0, 1.5, 1.0);
            glutSolidCube(0.30);

        }
        glPopMatrix();

        glPushMatrix(); {

            glColor3f(0, 0, 0);
            glTranslatef(1.9, -0.955, 2.71);
            glRotatef(30, 1.0, 0.0, 0.0);
            glScalef(1.0, 1.0, 1.0);
            glutSolidCube(0.29);

        }
        glPopMatrix();

        glPushMatrix(); {

            glColor3f(0, 0, 0);
            glTranslatef(1.9, -0.75, 2.79);
            glRotatef(10, 1.0, 0.0, 0.0);
            glScalef(1.0, 1.0, 1.0);
            glutSolidCube(0.28);

        }
        glPopMatrix();

        glPushMatrix(); {

            glColor3f(0, 0, 0);
            glTranslatef(1.9, -0.65, 2.8);
            glScalef(1.0, 0.6, 1.0);
            glutSolidCube(0.28);

        }
        glPopMatrix();

        glPushMatrix(); {
            glColor3f(0, 0, 0);
            glTranslatef(1.9, -0.55, 2.78);
            glRotatef(70, 1.0, 0.0, 0.0);
            glScalef(1.0, 1.0, 1.0);
            glutSolidCube(0.27);

        }
        glPopMatrix();

        glPushMatrix(); {
            glColor3f(0, 0, 0);
            glTranslatef(1.9, -0.45, 2.732);
            glRotatef(60, 1.0, 0.0, 0.0);
            glScalef(1.0, 1.0, 1.0);
            glutSolidCube(0.26);

        }
        glPopMatrix();

        glPushMatrix(); {
            glColor3f(0, 0, 0);
            glTranslatef(1.9, -0.37, 2.68);
            glRotatef(45, 1.0, 0.0, 0.0);
            glScalef(1.0, 1.0, 1.0);
            glutSolidCube(0.25);

        }
        glPopMatrix();

        glPushMatrix(); {
            glColor3f(0, 0, 0);
            glTranslatef(1.9, -0.3, 2.60);
            glRotatef(30, 1.0, 0.0, 0.0);
            glScalef(1.0, 1.0, 1.0);
            glutSolidCube(0.24);

        }
        glPopMatrix();

        glPushMatrix(); {
            glColor3f(0, 0, 0);
            glTranslatef(1.9, -0.22, 2.42);
            glRotatef(15, 1.0, 0.0, 0.0);
            glScalef(1.0, 1.0, 1.0);
            glutSolidCube(0.23);

        }
        glPopMatrix();

        glPushMatrix(); {
            glColor3f(0, 0, 0);
            glTranslatef(1.9, -0.2, 2.3);
            glScalef(1.0, 1.0, 1.0);
            glRotatef(5, 1.0, 0.0, 0.0);
            glutSolidCube(0.22);

        }
        glPopMatrix();
    }
}

void draw_rightEyeBrow() {
    {
        glPushMatrix(); {

            glColor3f(0, 0, 0);
            glTranslatef(1.9, 1.2, 2.5);
            glRotatef(-45, 1.0, 0.0, 0.0);
            glScalef(1.0, 1.5, 1.0);
            glutSolidCube(0.30);

        }
        glPopMatrix();

        glPushMatrix(); {

            glColor3f(0, 0, 0);
            glTranslatef(1.9, 0.955, 2.71);
            glRotatef(60, 1.0, 0.0, 0.0);
            glScalef(1.0, 1.0, 1.0);
            glutSolidCube(0.29);

        }
        glPopMatrix();

        glPushMatrix(); {

            glColor3f(0, 0, 0);
            glTranslatef(1.9, 0.75, 2.79);
            glRotatef(80, 1.0, 0.0, 0.0);
            glScalef(1.0, 1.0, 1.0);
            glutSolidCube(0.28);

        }
        glPopMatrix();

        glPushMatrix(); {

            glColor3f(0, 0, 0);
            glTranslatef(1.9, 0.65, 2.8);
            glScalef(1.0, 0.6, 1.0);
            glutSolidCube(0.28);

        }
        glPopMatrix();

        glPushMatrix(); {
            glColor3f(0, 0, 0);
            glTranslatef(1.9, 0.55, 2.78);
            glRotatef(20, 1.0, 0.0, 0.0);
            glScalef(1.0, 1.0, 1.0);
            glutSolidCube(0.27);

        }
        glPopMatrix();

        glPushMatrix(); {
            glColor3f(0, 0, 0);
            glTranslatef(1.9, 0.45, 2.732);
            glRotatef(30, 1.0, 0.0, 0.0);
            glScalef(1.0, 1.0, 1.0);
            glutSolidCube(0.26);

        }
        glPopMatrix();

        glPushMatrix(); {
            glColor3f(0, 0, 0);
            glTranslatef(1.9, 0.37, 2.68);
            glRotatef(45, 1.0, 0.0, 0.0);
            glScalef(1.0, 1.0, 1.0);
            glutSolidCube(0.25);

        }
        glPopMatrix();

        glPushMatrix(); {
            glColor3f(0, 0, 0);
            glTranslatef(1.9, 0.3, 2.60);
            glRotatef(60, 1.0, 0.0, 0.0);
            glScalef(1.0, 1.0, 1.0);
            glutSolidCube(0.24);

        }
        glPopMatrix();

        glPushMatrix(); {
            glColor3f(0, 0, 0);
            glTranslatef(1.9, 0.22, 2.42);
            glRotatef(75, 1.0, 0.0, 0.0);
            glScalef(1.0, 1.0, 1.0);
            glutSolidCube(0.23);

        }
        glPopMatrix();

        glPushMatrix(); {
            glColor3f(0, 0, 0);
            glTranslatef(1.9, 0.2, 2.3);
            glScalef(1.0, 1.0, 1.0);
            glRotatef(85, 1.0, 0.0, 0.0);
            glutSolidCube(0.22);

        }
        glPopMatrix();
    }
}

void draw_middle() {
    char imageName[30] = "image/dao_belt.bmp";

    glPushMatrix(); {
        getTexture(imageName);

        glColor3f(0.08f, 0.4f, 0.9f);
        glTranslatef(0.0f, 0.0f, -1.0f);   // z������ -1��ŭ �̵�
        glRotatef(110.0f, 0.5f, -0.5f, 0.9f);
        glScalef(1.0f, 1.0f, 1.0f);        // ũ�� ��ȯ (����)

        // �� �׸���
        GLUquadricObj* sphere = gluNewQuadric(); // Quadrics obj ����
        gluQuadricDrawStyle(sphere, GLU_FILL);
        gluQuadricNormals(sphere, GLU_SMOOTH);
        gluQuadricOrientation(sphere, GLU_OUTSIDE);
        gluQuadricTexture(sphere, GL_TRUE);
        gluSphere(sphere, 1.0f, 50, 50); // �������� 1.0�� ���� �����ϰ� �ؽ�ó�� �����մϴ�.
        gluDeleteQuadric(sphere); // Quadrics obj ����

        // �ؽ�ó ���� ��Ȱ��ȭ
        glBindTexture(GL_TEXTURE_2D, 0);

        // ������ ���� �ʱ�ȭ
        glDisable(GL_TEXTURE_2D);
    }
    glPopMatrix();
}

/** ������ �� **/
void draw_rightArm() {
    glPushMatrix(); {
        GLUquadric* quad = gluNewQuadric();

        glColor3f(0.965f, 0.741f, 0.584f); // ��� ���� ����
        glTranslatef(0.5f, 1.3f, -1.5f); // �̵�
        // �� �κ��� ȸ�� ��ȯ�� ���뿡 �����ϱ� ���� ���뿡 ���� ��ȯ
        glRotatef(-65.0f, 0.0f, 1.0f, 0.0f);
        glRotatef(40.0f, 1.0f, 1.0f, 0.0f);
        gluCylinder(quad, 0.2f, 0.2f, 1.0f, 40, 40); // ��ü, �ϴ�, ���, ����, �ѷ�, ���� ���� ����(���� ��) 
    }
    glPopMatrix();
}

/** ���� �� **/
void draw_leftArm() {
    glPushMatrix(); {
        GLUquadric* quad = gluNewQuadric();

        glColor3f(0.965f, 0.741f, 0.584f); // ��� ���� ����
        glTranslatef(0.5f, -1.3f, -1.5f); // �̵�
        // �� �κ��� ȸ�� ��ȯ�� ���뿡 �����ϱ� ���� ���뿡 ���� ��ȯ
        glRotatef(-45.0f, 1.0f, 1.0f, 0.0f);
        gluCylinder(quad, 0.2f, 0.2f, 1.0f, 40, 40); // ��ü, �ϴ�, ���, ����, �ѷ�, ���� ���� ����(���� ��) 
    }
    glPopMatrix();
}

/** ������ �ո� **/
void draw_rightWrist() {
    glPushMatrix(); {
        GLUquadric* quad = gluNewQuadric();

        glColor3f(1.0f, 1.0f, 1.0f); // ��� ���� ����
        glTranslatef(0.5f, 1.3f, -1.5f); // �̵�
        // �� �κ��� ȸ�� ��ȯ�� ���뿡 �����ϱ� ���� ���뿡 ���� ��ȯ
        glRotatef(-65.0f, 0.0f, 1.0f, 0.0f);
        glRotatef(40.0f, 1.0f, 1.0f, 0.0f);
        gluCylinder(quad, 0.2f, 0.25f, 0.3f, 40, 40); // ��ü, �ϴ�, ���, ����, �ѷ�, ���� ���� ����(���� ��)    
    }
    glPopMatrix();
}

/** ���� �ո� **/
void draw_leftWrist() {
    glPushMatrix(); {
        GLUquadric* quad = gluNewQuadric();

        glColor3f(1.0f, 1.0f, 1.0f); // ��� ���� ����
        glTranslatef(0.5f, -1.3f, -1.5f); // �̵�
        // �� �κ��� ȸ�� ��ȯ�� ���뿡 �����ϱ� ���� ���뿡 ���� ��ȯ
        glRotatef(-45.0f, 1.0f, 1.0f, 0.0f);
        gluCylinder(quad, 0.2f, 0.25f, 0.25f, 40, 40); // ��ü, �ϴ�, ���, ����, �ѷ�, ���� ���� ����(���� ��) 
    }
    glPopMatrix();
}

/** ������ �� **/
void draw_rightHand() {
    glPushMatrix(); {
        GLUquadric* sphere = gluNewQuadric();

        glColor3f(1.0f, 1.0f, 1.0f); // ��� ���� ����

        glTranslatef(0.5f, 1.3f, -1.5f); // �̵�
        // �� �κ��� ȸ�� ��ȯ�� ���뿡 �����ϱ� ���� ���뿡 ���� ��ȯ
        glRotatef(-65.0f, 0.0f, 1.0f, 0.0f);
        glRotatef(40.0f, 1.0f, 1.0f, 0.0f);
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f); // yz ��鿡 ���̵��� ȸ��

        gluQuadricDrawStyle(sphere, GLU_FILL);
        gluQuadricNormals(sphere, GLU_SMOOTH);
        gluQuadricTexture(sphere, GL_TRUE);
        glPolygonMode(GL_FRONT, GL_FILL);
        gluSphere(sphere, 0.3f, 7, 50);
    }
    glPopMatrix();
}

/** ���� �� **/
void draw_leftHand() {
    glPushMatrix(); {
        GLUquadric* sphere = gluNewQuadric();

        glColor3f(1.0f, 1.0f, 1.0f); // ��� ���� ����
        glTranslatef(0.5f, -1.3f, -1.5f); // �̵�
        // �� �κ��� ȸ�� ��ȯ�� ���뿡 �����ϱ� ���� ���뿡 ���� ��ȯ
        glRotatef(-45.0f, 1.0f, 1.0f, 0.0f);
        glRotatef(90.0f, 0.0f, 1.0f, 0.0f); // yz ��鿡 ���̵��� ȸ��

        gluQuadricDrawStyle(sphere, GLU_FILL);
        gluQuadricNormals(sphere, GLU_SMOOTH);
        gluQuadricTexture(sphere, GL_TRUE);
        glPolygonMode(GL_FRONT, GL_FILL);
        gluSphere(sphere, 0.3f, 7, 50);
    }
    glPopMatrix();
}

/** ������ �ٸ� **/
void draw_rightLeg() {
    glPushMatrix(); {
        GLUquadric* quad = gluNewQuadric();

        glColor3f(0.965f, 0.741f, 0.584f); // ��� ���� ����
        glTranslatef(0.0f, 0.4f, -2.2f); // �̵�
        // �� �κ��� ȸ�� ��ȯ�� ���뿡 �����ϱ� ���� ���뿡 ���� ��ȯ
        glRotatef(90.0f, 0.0f, 0.0f, 1.0f); // z��� �����ϰ� ȸ��

        gluCylinder(quad, 0.2f, 0.2f, 0.5f, 40, 40); // ��ü, �ϴ�, ���, ����, �ѷ�, ���� ���� ����(���� ��) 
    }
    glPopMatrix();
}

/** ���� �ٸ� **/
void draw_leftLeg() {
    glPushMatrix(); {
        GLUquadric* quad = gluNewQuadric();

        glColor3f(0.965f, 0.741f, 0.584f); // ��� ���� ����
        glTranslatef(0.0f, -0.4f, -2.2f); // �̵�
        // �� �κ��� ȸ�� ��ȯ�� ���뿡 �����ϱ� ���� ���뿡 ���� ��ȯ
        glRotatef(90.0f, 0.0f, 0.0f, 1.0f); // z��� �����ϰ� ȸ��

        gluCylinder(quad, 0.2f, 0.2f, 0.5f, 40, 40); // ��ü, �ϴ�, ���, ����, �ѷ�, ���� ���� ����(���� ��) 
    }
    glPopMatrix();
}

/** ������ �縻 **/
void draw_righttSock() {
    glPushMatrix(); {
        GLUquadric* quad = gluNewQuadric();

        glColor3f(1.0f, 1.0f, 1.0f); // ��� ���� ����
        glTranslatef(0.0f, 0.4f, -2.2f); // �̵�
        glRotatef(90.0f, 0.0f, 0.0f, 1.0f); // z��� �����ϰ� ȸ��
        gluCylinder(quad, 0.21f, 0.21f, 0.15f, 40, 40); // ��ü, �ϴ�, ���, ����, �ѷ�, ���� ���� ����(���� ��)    
    }
    glPopMatrix();
}

/** ���� �縻 **/
void draw_leftSock() {
    glPushMatrix(); {
        GLUquadric* quad = gluNewQuadric();

        glColor3f(1.0f, 1.0f, 1.0f); // ��� ���� ����
        glTranslatef(0.0f, -0.4f, -2.2f); // �̵�
        glRotatef(90.0f, 0.0f, 0.0f, 1.0f); // z��� �����ϰ� ȸ��
        gluCylinder(quad, 0.21f, 0.21f, 0.15f, 40, 40); // ��ü, �ϴ�, ���, ����, �ѷ�, ���� ���� ����(���� ��)    
    }
    glPopMatrix();
}

/** ������ �Ź� **/
void draw_rightShoe() {
    glPushMatrix(); {
        GLUquadric* quad = gluNewQuadric();
        glColor3f(0.08f, 0.4f, 0.9f);
        glTranslatef(0.0f, 0.4f, -2.35f);  // ������ ����� ���� ��ġ �̵�
        glRotatef(100.0f, 0.0f, 0.0f, 1.0f); // z��� �����ϰ� ȸ��
        gluCylinder(quad, 0.3f, 0.3f, 0.2f, 6, 40);  // ����� ����
    }
    glPopMatrix();
}

/** ���� �Ź� **/
void draw_leftShoe() {
    glPushMatrix(); {
        GLUquadric* quad = gluNewQuadric();
        glColor3f(0.08f, 0.4f, 0.9f);
        glTranslatef(0.0f, -0.4f, -2.35f);  // ������ ����� ���� ��ġ �̵�
        glRotatef(80.0f, 0.0f, 0.0f, 1.0f); // z��� �����ϰ� ȸ��
        gluCylinder(quad, 0.3f, 0.3f, 0.2f, 6, 1);  // ����� ����
    }
    glPopMatrix();
}