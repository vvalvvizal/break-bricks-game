#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h> // (or others, depending on the system in use)
#include <math.h>
#include <iostream>
#include<vector>
#include <chrono>
#include <sstream>

#define	width 			800
#define	height			800
#define	PI				3.1415
#define	polygon_num		50

std::chrono::time_point<std::chrono::steady_clock> startTime;
const char* stoptext = "  GAME STOP \'ESC\' "; //���� ���� �ȳ� ���� 

bool		 gameStarted = false;
bool		game_over;
bool		startButtonEnabled = false;  // ���� ��ư Ȱ��ȭ ����
bool		AllBricksDestroyed;

int		mode = 0;//�⺻ ���� ��� 
int		left = 0;
int		right = left + width;
int		bottom = 0;
int		top = bottom + height;
int		bar_height = 10;
int		bar_width = 100;
int		collision_count = 1;
int		wall_collision_count = 1;
int		minutes;
int		seconds;

float		ball_radius;

//������ �������� �� ������ 
float		diagonalX1 = width / 2;
float		diagonalY1 = height / 10;
float		diagonalX2 = width * 7 / 8;
float		diagonalY2 = height / 3;
float		diagonalX3 = width * 7 / 8;
float		diagonalY3 = height * 2 / 3;
float		diagonalX4 = width / 2;
float		diagonalY4 = height * 9 / 10;
float		diagonalX5 = width / 8;
float		diagonalY5 = height * 2 / 3;
float		diagonalX6 = width / 8;
float		diagonalY6 = height / 3;
float		bar_velocity;



typedef struct Color {
	float r;
	float g;
	float b;
};
typedef struct Point {
	float	x;
	float	y;
} Point;

typedef struct Rect {
	float x1;
	float y1;
	float x2;
	float y2;
};

//���� ����ü 
typedef struct Brick {
	float x1;
	float y1;
	float x2;
	float y2;
	bool isDestroyed;
} Brick;


Point moving_ball, velocity, movingbar;
Point bar, bar2;
Point bar_radius;
std::vector<Brick> bricks;
GLfloat gamespace_linewidth = 6.0; //������ �β� 
Color ballcolor;


void init(void) {
	bar.x = width / 2; //���� ũ�� ���� 
	bar.y = height / 3;

	bar2.x = width / 2; //�ι�° ���� ũ�� ���� 
	bar2.y = height * 2 / 3;

	ball_radius = 10.0; // ���� ������ ���� 

	//���� �ʱ� ��ġ ����
	moving_ball.x = width / 2;
	moving_ball.y = height / 2;

	//���� �ʱ� �ӵ� ����
	velocity.x = 0;
	velocity.y = -0.1;

	//��� ������ ���������� Ȯ���ϴ� ���� false 
	AllBricksDestroyed = false;

	//�浹 Ƚ��
	collision_count = 1;

	//���� �ʱ� ����
	ballcolor.r = 0.0;
	ballcolor.g = 0.0;
	ballcolor.b = 1.0;

	bricks.clear(); // ���� �ʱ�ȭ

	// �������� �ʱ� ��ġ�� ũ�� ����
	float brickWidth = 40.0;
	float brickHeight = 20.0;
	float startX = 200.0;
	float startY = 500.0;
	int numBricksPerRow = 8;
	int numBrickRows = 4;

	for (int row = 0; row < numBrickRows; row++) {
		for (int col = 0; col < numBricksPerRow; col++) {
			Brick brick;
			brick.x1 = startX + col * (brickWidth + 10);
			brick.y1 = startY - row * (brickHeight + 10);
			brick.x2 = brick.x1 + brickWidth;
			brick.y2 = brick.y1 + brickHeight;
			brick.isDestroyed = false;
			bricks.push_back(brick); // ���� �߰�
		}
	}
}

//�Ÿ� ����ϴ� �Լ�
float distance(float x1, float y1, float x2, float y2) {
	return std::sqrt(std::pow(x2 - x1, 2) + std::pow(y2 - y1, 2));
}

//������ �׸��� �Լ� 
void Modeling_gamespace() {

	glLineWidth(gamespace_linewidth);
	glBegin(GL_LINE_STRIP);

	glVertex2f(diagonalX1, diagonalY1);
	glVertex2f(diagonalX2, diagonalY2);

	glVertex2f(diagonalX3, diagonalY3);

	glVertex2f(diagonalX4, diagonalY4);

	glVertex2f(diagonalX5, diagonalY5);
	glVertex2f(diagonalX6, diagonalY6);

	glVertex2f(diagonalX1, diagonalY1);
	glEnd();

}

void MyReshape(int w, int h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(left, left + width, bottom, bottom + height);
}


//�� �׸��� �Լ�
void Modeling_bar_from_screen(Point CC) { //������ x,y �־��� 
	glBegin(GL_POLYGON);
	glVertex2f(CC.x, CC.y);
	glVertex2f(CC.x + bar_width, CC.y);
	glVertex2f(CC.x, CC.y + bar_height);
	glVertex2f(CC.x + bar_width, CC.y + bar_height);
	//���μ�


	glVertex2f(CC.x, CC.y);
	glVertex2f(CC.x, CC.y + bar_height);
	glVertex2f(CC.x + bar_width, CC.y);
	glVertex2f(CC.x + bar_width, CC.y + bar_height);
	//���μ�
	glEnd();

}

//��� ������ �������� Ȯ���ϴ� �Լ� 
bool AllBricksDestroyed_check() {
	// ������ ��� ��ȸ�ϸ鼭 ���� ������ �ִ��� Ȯ��
	for (int i = 0; i < bricks.size(); i++) {
		if (!bricks[i].isDestroyed) {
			// ������ ���� ������ ������ false ��ȯ
			return false;
		}

	}
	// ��� ������ �������� true ��ȯ
	return true;
}



//���� �׸��� �Լ� 
void Modeling_brick() {
	glColor3f(0.9, 0.0, 0.0);
	for (int i = 0; i < bricks.size(); i++) {
		Brick brick = bricks[i];

		if (!brick.isDestroyed) {
			glBegin(GL_POLYGON);
			glVertex2f(brick.x1, brick.y1);
			glVertex2f(brick.x2, brick.y1);
			glVertex2f(brick.x2, brick.y2);
			glVertex2f(brick.x1, brick.y2);
			glEnd();
		}
	}


}

//�� �׸��� �Լ� 
void	Modeling_Circle(float radius, Point CC) {

	float	delta;

	delta = 2 * PI / polygon_num;
	glBegin(GL_POLYGON);
	for (int i = 0; i < polygon_num; i++)
		glVertex2f(CC.x + radius * cos(delta * i), CC.y + radius * sin(delta * i)); //���� �׸��� �ڵ� 
	glEnd();
}

//�� ���� ���� ���ϴ� �Լ� 
void Chooseball_direction(float lineX1, float lineY1, float lineX2, float lineY2) {

	float slope = (lineY2 - lineY1) / (lineX2 - lineX1);//���� 
	float yIntercept = lineY1 - slope * lineX1;

	float normalX = -slope; //�������� 
	float normalY = 1.0;

	float dotProduct = velocity.x * normalX + velocity.y * normalY;//���� 

	float reflectionX = velocity.x - 2 * dotProduct * normalX;//�ݻ纤�� ���ϱ� 
	float reflectionY = velocity.y - 2 * dotProduct * normalY;

	velocity.x = reflectionX;
	velocity.y = reflectionY;

}

//���� �� ������ �浹 üũ�ϴ� �Լ� 
void Collision_Detection_Between_bar(Point ball, Point bar) {
	movingbar.x = bar_width / 2.0;
	movingbar.y = bar_height / 2.0;

	float bar_radius = sqrt(pow(movingbar.x, 2) + pow(movingbar.y, 2));
	float distance = sqrt((pow(movingbar.x - ball.x, 2) + pow(movingbar.y - ball.y, 2)));

	if (ball.x >= bar.x && ball.x <= bar.x + bar_width && ball.y < bar.y + bar_height && ball.y + ball_radius >= bar.y) {
		velocity.y *= -1;
		collision_count++;
		velocity.x = -velocity.x * 0.04; //���� �ӵ� �����ϴ� ��� 
		velocity.x += bar_velocity * 1;//�ݹ߰�� 

		ballcolor.r = 0.0;
		ballcolor.b = 1.0;
	}
	if (distance <= bar_radius + ball_radius)//�𼭸� �浹 üũ 
	{
		velocity.y *= -1;
		collision_count++;
		ballcolor.r = 0.0;
		ballcolor.b = 1.0;
	}


}

//�ι�° �ٿ� �� �浹üũ �ϴ� �Լ� 
void Collision_Detection_Between_bar2(Point ball, Point bar) {
	movingbar.x = bar_width / 2.0;
	movingbar.y = bar_height / 2.0;

	float bar_radius = sqrt(pow(movingbar.x, 2) + pow(movingbar.y, 2));
	float distance = sqrt((pow(movingbar.x - ball.x, 2) + pow(movingbar.y - ball.y, 2)));

	if (ball.x >= bar.x && ball.x <= bar.x + bar_width && ball.y + ball_radius > bar.y && ball.y - ball_radius <= bar.y + bar_height) {
		velocity.y *= -1;
		collision_count++;
		velocity.x = -velocity.x * 0.04; //���� �ӵ� �����ϴ� ��� 
		velocity.x += bar_velocity * 1;//�ݹ߰�� 
		ballcolor.r = 0.0;
		ballcolor.b = 1.0;
	}
	if (distance <= bar_radius + ball_radius)//�𼭸� �浹 üũ 
	{
		velocity.y *= -1;
		collision_count++;
		ballcolor.r = 0.0;
		ballcolor.b = 1.0;
	}

}

//���� ���� �浹üũ �ϴ� �Լ�
bool checkCollisionWithBricks(Point ball, float radius) {
	for (int i = 0; i < bricks.size(); i++) {
		Brick brick = bricks[i];

		if (!brick.isDestroyed) {
			// ���� ������ �浹 üũ
			if (ball.x + radius >= brick.x1 && ball.x - radius <= brick.x2 &&
				ball.y + radius >= brick.y1 && ball.y - radius <= brick.y2) {
				// ������ ��������� ����
				brick.isDestroyed = true;
				bricks[i] = brick;
				velocity.y *= -1;
				return true;
			}
		}
	}
	return false;
}

// �浹�˻� ��� �������� ���� ����� �Լ� 
void checkCollisionAndRemoveBricks(Point ball, float radius) {
	bool collision = checkCollisionWithBricks(ball, ball_radius);

	if (collision) {
		// �浹�� ��� ���� �����
		bricks.erase(std::remove_if(bricks.begin(), bricks.end(),
			[](const Brick& brick) { return brick.isDestroyed; }),
			bricks.end());
	}
}


// �밢���� ���� �浹 �˻�
bool checkCollision(float diagonalStartX, float diagonalStartY, float diagonalEndX, float diagonalEndY, float circleCenterX, float circleCenterY, float circleRadius) {
	float diagonalLength = distance(diagonalStartX, diagonalStartY, diagonalEndX, diagonalEndY);
	float circleDistance = std::abs((diagonalEndY - diagonalStartY) * circleCenterX - (diagonalEndX - diagonalStartX) * circleCenterY + diagonalEndX * diagonalStartY - diagonalEndY * diagonalStartX) / diagonalLength;

	return circleDistance <= circleRadius;
	//�� ���������� ���� �밢���� �Ÿ��� ������ true ��ȯ, �ƴϸ� false 
}

//���� ������ â�� ���� �浹üũ �ϴ� �Լ� 
void Collision_window(Point& ball2) {
	// Calculate the boundaries of the window
	float windowTop = 0.0;
	float windowBottom = height;
	float windowLeft = 0.0;
	float windowRight = width;

	// Check collision with the top and bottom walls
	if (ball2.y - ball_radius <= windowTop || ball2.y + ball_radius >= windowBottom) {
		velocity.y *= -1; // Reverse the y velocity
	}

	// Check collision with the left and right walls
	if (ball2.x - ball_radius <= windowLeft || ball2.x + ball_radius >= windowRight) {
		velocity.x *= -1; // Reverse the x velocity
	}
}




//�����ǰ� �浹üũ�ϴ� �Լ� 
void Collision_Detection_to_Walls(Point& ball2) {

	if (checkCollision(diagonalX1, diagonalY1, diagonalX2, diagonalY2, ball2.x, ball2.y, ball_radius))
	{
		Chooseball_direction(diagonalX1, diagonalY1, diagonalX2, diagonalY2);
		ballcolor.r = 1.0; //�����ǰ� �浹�ϸ� �� ���� ����� ���� 
		ballcolor.b = 0.0;


	}
	else if (checkCollision(diagonalX3, diagonalY3, diagonalX4, diagonalY4, ball2.x, ball2.y, ball_radius))
	{
		Chooseball_direction(diagonalX3, diagonalY3, diagonalX4, diagonalY4);
		ballcolor.r = 1.0;
		ballcolor.b = 0.0;

	}
	else if (checkCollision(diagonalX5, diagonalY5, diagonalX4, diagonalY4, ball2.x, ball2.y, ball_radius)) {
		Chooseball_direction(diagonalX5, diagonalY5, diagonalX4, diagonalY4);
		ballcolor.r = 1.0;
		ballcolor.b = 0.0;
	}
	else if (checkCollision(diagonalX6, diagonalY6, diagonalX1, diagonalY1, ball2.x, ball2.y, ball_radius)) {
		Chooseball_direction(diagonalX6, diagonalY6, diagonalX1, diagonalY1);
		ballcolor.r = 1.0;
		ballcolor.b = 0.0;
	}

	//������ ������� �浹üũ 
	else if (ball2.x - ball_radius <= diagonalX6 || ball2.x + ball_radius >= diagonalX2) {
		velocity.x *= -1;
		ballcolor.r = 1.0;
		ballcolor.b = 0.0;
	}
}





void SpecialKey(int key, int x, int y) {

	switch (key) {
	case GLUT_KEY_RIGHT:
		if (bar.x <= right) {
			if (bar.x + bar_width <= width * 7 / 8 - 70)
			{
				bar_velocity = 0.03; //�ٰ� ���������� �����̸� ���� �ӵ� ���� ��� 

				bar.x += 10;
				bar2.x += 10;
				break;

			}
		}
	case GLUT_KEY_LEFT:
		if (bar.x >= left) {
			if (bar.x >= width / 8 + 70) {
				bar_velocity = -0.03; //�ٰ� �������� �����̸� ���� �ӵ� ���� ����
				bar.x -= 10;
				bar2.x -= 10;
				break;
			}
		}

	default:
		bar_velocity = 0.0f;
		break;
	}
	glutPostRedisplay();
}
void mouseClick(int button, int state, int x, int y) {
	// ���� ��ư Ŭ�� �� ���� ����
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && !gameStarted) {
		if (startButtonEnabled) {
			if (x >= width / 2 - 100 && x <= width / 2 + 100 && y >= height / 2 - 50 && y <= height / 2 + 50) {
				gameStarted = true;
				std::cout << "Game started!" << std::endl;
				init();
				startTime = std::chrono::steady_clock::now(); //���� �����ϸ� �ð� üũ

			}
		}
		// TODO: ���� ���� ���� �߰�
	}
}
void Keyboard(unsigned char key, int x, int y) {
	if (key == 27) {  // 27�� ESC Ű�� ASCII ���Դϴ�
		std::cout << "Game End!" << std::endl;
		exit(0);
	}
	else if (mode == 0) {  // ��� ���� ������ Ű���� �Է��� ó��
		switch (key) {
		case '1':
			mode = 1;
			startButtonEnabled = true;  // ��� 1 ���� �� ���� ��ư Ȱ��ȭ
			break;
		case '2':
			mode = 2;
			startButtonEnabled = true;  // ��� 2 ���� �� ���� ��ư Ȱ��ȭ
			break;
		}
	}
	glutPostRedisplay();
}

void gameOver() { //���ӿ��� �Լ� 
	game_over = true;
}


void RenderScene(void) {

	glClearColor(1.0, 1.0, 1.0, 0.0); // Set display-window color to Yellow
	glClear(GL_COLOR_BUFFER_BIT);

	glColor3f(0.5f, 0.5f, 0.5f);
	glRasterPos2f(0, height - 30);
	for (const char* c = stoptext; *c != '\0'; ++c) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
	}

	if (gameStarted) {

		if (AllBricksDestroyed_check()) { //���� �� �������� Ȯ���ϴ� �Լ� ���� true�̸� 
			// ������ ��� ���� �ð� ����
			AllBricksDestroyed = true;
			gameStarted = false;
			startButtonEnabled = false;
			mode = 0;
			gameOver();
		}

		glColor3f(0.0, 0.0, 1.0);
		Modeling_gamespace();


		glColor3f(0.0, 0.0, 1.0);
		Modeling_bar_from_screen(bar);

		if (mode == 2) {
			Modeling_bar_from_screen(bar2);
			Collision_Detection_Between_bar2(moving_ball, bar2);
		}

		// �����̴� ���� ��ġ ��ȭ 
		if (velocity.y > 0.2 || velocity.y < -0.2) {
			velocity.y = 0.1;
		}
		moving_ball.x += velocity.x;
		moving_ball.y += velocity.y;

		Collision_Detection_to_Walls(moving_ball);	// ���� ���� �浹üũ �Լ� 

		// �浹 ó�� �κ�
		Collision_Detection_Between_bar(moving_ball, bar); //���� �е��� �浹üũ �Լ� 
		Collision_window(moving_ball);

		// �浹 üũ �� ���� �����
		checkCollisionAndRemoveBricks(moving_ball, ball_radius);

		// ���� �׸���
		Modeling_brick();


		// �����̴� �� �׸��� 
		glColor3f(ballcolor.r, ballcolor.g, ballcolor.b);
		Modeling_Circle(ball_radius, moving_ball);

	}
	else { // ���� ���� �� ������ ���
	 // ���� ȭ�� �׸���


		if (mode == 0) {  // ��� ���� �� ����

			if (game_over) {
				auto currentTime = std::chrono::steady_clock::now();
				auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count();

				minutes = elapsedTime / (1000 * 60); // �и��ʸ� ������ ��ȯ
				seconds = (elapsedTime % (1000 * 60)) / 1000; // �и��ʸ� �ʷ� ��ȯ
				game_over = false;
			}
			// ȭ�鿡 ���
			std::string timeStr = std::to_string(minutes) + " : " + std::to_string(seconds);
			// ȭ�� �߾ӿ� �ð� ���
			glColor3f(0.5f, 0.5f, 0.5f);
			glRasterPos2f(width / 2 - 60, height - 250);
			const char* infotextscore = "your score!    ";
			for (const char* c = infotextscore; *c != '\0'; ++c) {
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
			}
			for (char c : timeStr) {
				glutBitmapCharacter(GLUT_BITMAP_8_BY_13, c);
			}


			glColor3f(0.5f, 0.5f, 0.5f);
			glBegin(GL_QUADS);
			glVertex2f(width / 2 - 100, height / 2 - 50);
			glVertex2f(width / 2 - 100, height / 2 + 50);
			glVertex2f(width / 2 + 100, height / 2 + 50);
			glVertex2f(width / 2 + 100, height / 2 - 50);
			glEnd();


			glColor3f(0.5f, 0.5f, 0.5f);
			glRasterPos2f(width / 2 - 100, height / 3);
			const char* infotext = "Choose mode use keyboard";
			const char* modetext = "1 : default    2 : add bar ";
			for (const char* c = infotext; *c != '\0'; ++c) {
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
			}
			glColor3f(.0f, 0.5f, 0.0f);
			glRasterPos2f(width / 2 - 100, height / 4);
			for (const char* c = modetext; *c != '\0'; ++c) {
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
			}


			glColor3f(1.0f, 1.0f, 1.0f);
			glRasterPos2f(width / 2 - 55, height / 2);
			const char* buttonText = "Choose Mode";
			for (const char* c = buttonText; *c != '\0'; ++c) {
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
			}
		}

		//��尡 ������ٸ� ���� ��ư�� ���� �� �ִ� ȭ�� ���� 
		if (startButtonEnabled) {
			glColor3f(0.5f, 0.5f, 0.5f);
			glBegin(GL_QUADS);
			glVertex2f(width / 2 - 100, height / 2 - 50);
			glVertex2f(width / 2 - 100, height / 2 + 50);
			glVertex2f(width / 2 + 100, height / 2 + 50);
			glVertex2f(width / 2 + 100, height / 2 - 50);
			glEnd();

			glColor3f(1.0f, 1.0f, 1.0f);
			glRasterPos2f(width / 2 - 58, height / 2);
			const char* buttonText = "PRESS START";
			for (const char* c = buttonText; *c != '\0'; ++c) {
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
			}
		}
	}

	glutSwapBuffers();
	glFlush();
}


void main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitWindowPosition(100, 100);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(width, height);
	glutCreateWindow("����������� 20213075 Ȳ����");
	init();
	glClearColor(0.2, 0.2, 0.2, 1.0);
	glutReshapeFunc(MyReshape);
	glutDisplayFunc(RenderScene);
	glutSpecialFunc(SpecialKey);
	glutMouseFunc(mouseClick);
	glutKeyboardFunc(Keyboard);
	glutIdleFunc(RenderScene);
	glutMainLoop();
}