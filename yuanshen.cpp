#include<iostream>
#include<graphics.h>
#include<conio.h>
#include<windows.h>
#include<string>
#include<vector>
#include<cmath>
#include<cstdlib>
#include<ctime>
#pragma comment(lib, "msimg32.lib") 
#pragma comment(lib,"winmm.lib")
#define delay 1000/144
#define playerspeed 3
#define enemyspeed 2
bool is_game_start;
int idx_current_anim = 0;
int button_width = 192;
int button_height = 75;
bool running = true;
inline void putimage_alpha(int x, int y, IMAGE* img) {
	int w = img->getwidth();
	int h = img->getheight();
	AlphaBlend(GetImageHDC(NULL), x, y, w, h, GetImageHDC(img), 0, 0, w, h, { AC_SRC_OVER,0,255,AC_SRC_ALPHA });
}
class Atlas {
public:
	Atlas(std::string path, int num) {
		std::string full_path;
		for (size_t i = 0; i < num; i++) {
			full_path = path + std::to_string(i) + ".png";
			IMAGE* frame = new IMAGE();
			loadimage(frame,full_path.c_str());
			frame_list.push_back(frame);
		}
	}
	Atlas(Atlas* src, bool is_sketch) {
		for (size_t i = 0; i < src->frame_list.size(); i++) {
			int width = src->frame_list[i]->getwidth();
			int height = src->frame_list[i]->getheight();
			IMAGE* current_frame = new IMAGE();
			Resize(current_frame, width, height);
			DWORD* color_buffer_src_img = GetImageBuffer(src->frame_list[i]);

			DWORD* color_buffer_current_img = GetImageBuffer(current_frame);
			for (int y = 0; y < height * width; y++) {
				if (color_buffer_src_img[y] & 0xFF000000 >> 24) {
					color_buffer_current_img[y] = 0xFFFFFFFF;
				}
				else {
					color_buffer_current_img[y] = 0x00000000;
				}
			}

			frame_list.push_back(current_frame);
		}
	}
	Atlas(Atlas* src) {
		for (size_t i = 0; i < src->frame_list.size(); i++) {
			int width = src->frame_list[i]->getwidth();
			int height = src->frame_list[i]->getheight();
			IMAGE* current_frame = new IMAGE();
			Resize(current_frame, width, height);
			DWORD* color_buffer_left_img = GetImageBuffer(src->frame_list[i]);
			IMAGE* current_raw_img = src->frame_list[i];
			DWORD* color_buffer_right_img = GetImageBuffer(current_frame);
			for (int y = 0; y < height; y++) {

				for (int x = 0; x < width; x++) {
					int idx_buffer_left_img = y * width + x;
					int idx_buffer_right_img = y * width + (width - 1 - x);
					color_buffer_right_img[idx_buffer_right_img] = color_buffer_left_img[idx_buffer_left_img];
				}
			}
			frame_list.push_back(current_frame);
		}
	}
	~Atlas() {
		for (int i = 0; i < frame_list.size(); i++) {
			delete frame_list[i];
		}
	}
	friend class Animation;
	friend class Button;
private:
	std::vector<IMAGE*>  frame_list;
};
Atlas* anim_left_player1;
Atlas* anim_right_player1;
Atlas* anim_left_enemy1;
Atlas* anim_right_enemy1;
Atlas* anim_left_player1_white;
Atlas* anim_right_player1_white;
Atlas* anim_left_enemy1_white;
Atlas* anim_right_enemy1_white;
class Animation {
public:
	Animation(Atlas* image, int interval);
	~Animation() = default;
	void play(int x, int y, int delta) {
		timer += delta;
		if (timer >= interval_ms) {
			idx_frame = (idx_frame + 1) % imagelist->frame_list.size();
			timer = 0;
		}
		putimage_alpha(x, y, imagelist->frame_list[idx_frame]);
	}
private:
	int interval_ms = 0;
	int timer = 0;
	int idx_frame = 0;
	Atlas* imagelist;
};
Animation::Animation(Atlas* image, int interval) {
	interval_ms = interval;
	imagelist = image;
}
class player {

public:

	player() {
		loadimage(&img_shadow, "提瓦特幸存者\\img\\shadow_player.png");
		anim_left_player = new Animation(anim_left_player1, 45);
		anim_right_player = new Animation(anim_right_player1, 45);
		anim_left_player_white = new Animation(anim_left_player1_white, 45);
		anim_right_player_white = new Animation(anim_right_player1_white, 45);
		player_hp = 2;
	}
	~player() {
		delete anim_left_player;
		delete anim_right_player;
	}
	void ProcessEvent(const ExMessage& msg) {

		if (msg.message == WM_KEYDOWN) {
			switch (msg.vkcode) {
			case VK_UP:
				up = true;
				break;
			case VK_DOWN:
				down = true;
				break;
			case VK_LEFT:
				left = true;
				break;
			case VK_RIGHT:
				right = true;
				break;
			}
		}
		else if (msg.message == WM_KEYUP) {
			switch (msg.vkcode) {
			case VK_UP:
				up = false;
				break;
			case VK_DOWN:
				down = false;
				break;
			case VK_LEFT:
				left = false;
				break;
			case VK_RIGHT:
				right = false;
				break;

			}

		}

	}
	void hurt() {
		player_hp--;
		hurt_time = 10;
	}
	void Move() {
		dx = (right ? 1 : 0) - (left ? 1 : 0);
		dy = (down ? 1 : 0) - (up ? 1 : 0);
		if (dx != 0 || dy != 0) {
			double len = sqrt(dx * dx + dy * dy);
			normallize_x += playerspeed * (dx / len);
			normallize_y += playerspeed * (dy / len);
			player_pos.x = normallize_x;
			player_pos.y = normallize_y;
		}
		if (player_pos.x < 0) { player_pos.x = 0; };
		if (player_pos.y < 0) { player_pos.y = 0; };
		if (player_pos.x > 1280 - player_width) {
			player_pos.x = 1280 - player_width;
		};
		if (player_pos.y > 720 - player_height) {
			player_pos.y = 720 - player_height;
		};
	}
	POINT Getposition()const {
		return player_pos;
	}
	int Getplayerwidth()const {
		return player_width;
	}
	int Getplayerheight()const {
		return player_height;
	}
	void Draw(int delta) {
		int shadow_pos_x = player_pos.x + (player_width / 2 - shadow_width / 2);
		int shadow_pos_y = player_pos.y + player_height - 8;
		putimage_alpha(shadow_pos_x, shadow_pos_y, &img_shadow);
		if (dx < 0) {
			facing_left = true;
		}
		else if (dx > 0) {
			facing_left = false;
		}if (hurt_time > 0) {
			if (facing_left) {
				anim_left_player_white->play(player_pos.x, player_pos.y, delta);
			}
			else {
				anim_right_player_white->play(player_pos.x, player_pos.y, delta);
			}
			hurt_time--;
		}
		else {
			if (facing_left) {
				anim_left_player->play(player_pos.x, player_pos.y, delta);
			}
			else {
				anim_right_player->play(player_pos.x, player_pos.y, delta);
			}
		}
	}
	int player_hp = 0;
private:
	double normallize_x = 500.0;
	double normallize_y = 500.0;
	bool up = false, down = false, left = false, right = false;
	bool facing_left = false;
	const int player_width = 80;
	const int player_height = 80;
	const int shadow_width = 32;
	IMAGE img_shadow;
	Animation* anim_left_player = NULL;
	Animation* anim_right_player = NULL;
	Animation* anim_left_player_white = NULL;
	Animation* anim_right_player_white = NULL;
	int dx = 0;
	int dy = 0;
	POINT player_pos{ 500,500 };
	int hurt_time;
};
class bullet {
public:
	bullet() = default;
	~bullet() = default;
	POINT Getbulletpos() {
		return bullet_pos;
	}
	void Draw() {
		setlinecolor(RGB(255, 155, 50));
		setfillcolor(RGB(200, 75, 10));
		fillcircle(bullet_pos.x, bullet_pos.y, radius);
	}
	POINT Getbulletposition()const {
		return bullet_pos;
	}
	void setbullet_pos(int x, int y) {
		bullet_pos.x = x;
		bullet_pos.y = y;
	}
private:
	POINT bullet_pos{ 0,0 };
	const int radius = 10;
};

class Enemy {
public:
	Enemy() { 
		hp = 8;
		loadimage(&img_shadow, "提瓦特幸存者\\img\\shadow_enemy.png");
		anim_left_enemy = new Animation(anim_left_enemy1, 45);
		anim_right_enemy = new Animation(anim_right_enemy1, 45);
		anim_left_enemy_white = new Animation(anim_left_enemy1_white, 45);
		anim_right_enemy_white = new Animation(anim_right_enemy1_white, 45);
		enum class enemy_rand { up = 0, down, left, right };
		enemy_rand m = (enemy_rand)(rand() % 4);
		switch (m) {
		case enemy_rand::up:
			normallize_x = rand() % 1280;
			normallize_y = -enemy_height;
			break;
		case enemy_rand::down:
			normallize_x = rand() % 1280;
			normallize_y = 720;
			break;
		case enemy_rand::left:
			normallize_x = -enemy_width;
			normallize_y = rand() % 720;
			break;
		case enemy_rand::right:
			normallize_x = 1280;
			normallize_y = rand() % 720;
			break;
		default:
			break;
		}

	}
	void hurt()
	{
		hp--;
		hurt_time = 10;
	}
	bool CheckPlayerCollision(const player& a) {
		const POINT& player_pos = a.Getposition();
		const int& player_width = a.Getplayerwidth();
		const int& player_height = a.Getplayerheight();
		int enemy_posx = enemy_pos.x + enemy_width / 2;
		int enemy_posy = enemy_pos.y + enemy_height / 2;
		bool is_overlap_x = enemy_posx >= player_pos.x && enemy_posx <= player_pos.x + player_width;
		bool is_overlap_y = enemy_posy >= player_pos.y && enemy_posy <= player_pos.y + player_height;
		return is_overlap_x && is_overlap_y;
	}
	bool CheckBulletCollision(const bullet& b) {
		const POINT& bullet_pos = b.Getbulletposition();
		bool is_overlap_x = bullet_pos.x >= enemy_pos.x && bullet_pos.x <= enemy_pos.x + enemy_width;
		bool is_overlap_y = bullet_pos.y >= enemy_pos.y && bullet_pos.y <= enemy_pos.y + enemy_height;
		return is_overlap_x && is_overlap_y;
	}
	~Enemy() {
		delete anim_left_enemy;
		delete anim_right_enemy;
		delete anim_left_enemy_white;
		delete anim_right_enemy_white;
	}

	void Move(const player& a) {
		const POINT& p_pos = a.Getposition();
		dx = p_pos.x - enemy_pos.x;
		dy = p_pos.y - enemy_pos.y;
		if (dx != 0 || dy != 0) {
			double len = sqrt(dx * dx + dy * dy);
			normallize_x += enemyspeed * (dx / len);
			normallize_y += enemyspeed * (dy / len);
			enemy_pos.x = normallize_x;
			enemy_pos.y = normallize_y;
		}
		if (enemy_pos.x < 0) { enemy_pos.x = 0; };
		if (enemy_pos.y < 0) { enemy_pos.y = 0; };
		if (enemy_pos.x > 1280 - enemy_width) {
			enemy_pos.x = 1280 - enemy_width;
		};
		if (enemy_pos.y > 720 - enemy_height) {
			enemy_pos.y = 720 - enemy_height;
		};
	}
	void Draw(int delta) {
		int shadow_pos_x = enemy_pos.x + (enemy_width / 2 - shadow_width / 2);
		int shadow_pos_y = enemy_pos.y + enemy_height - 8;
		putimage_alpha(shadow_pos_x, shadow_pos_y, &img_shadow);
		if (hurt_time > 0) {
			if (dx < 0) {
				anim_left_enemy_white->play(enemy_pos.x, enemy_pos.y, delta);
			}
			else {
				anim_right_enemy_white->play(enemy_pos.x, enemy_pos.y, delta);
			}
			hurt_time--;
		}
		else {
			if (dx < 0) {
				anim_left_enemy->play(enemy_pos.x, enemy_pos.y, delta);
			}
			else {
				anim_right_enemy->play(enemy_pos.x, enemy_pos.y, delta);
			}
		}
	}
public:
	int hp;
private:

	double normallize_x = 0.0;
	double normallize_y = 0.0;
	bool up = false, down = false, left = false, right = false;

	const int enemy_width = 80;
	const int enemy_height = 80;
	const int shadow_width = 48;
	IMAGE img_shadow;
	Animation* anim_left_enemy = NULL;
	Animation* anim_right_enemy = NULL;
	Animation* anim_left_enemy_white = NULL;
	Animation* anim_right_enemy_white = NULL;
	int dx = 0;
	int dy = 0;
	POINT enemy_pos{ 0,0 };
	float hurt_time = 0;
};
void TryGenerateEnemy(std::vector<Enemy*>& enemylist, int deltal) {
	const int interval = 2000;
	static int counter = 0;
	if ((counter += deltal) > interval) {
		enemylist.push_back(new Enemy());
		counter = 0;
	}
}
bool is_game_over(std::vector<Enemy*> enemylist, const player& a);
void updatebullets(std::vector<bullet*>& bulletlist, const player& a) {
	const int& player_width = a.Getplayerwidth();
	const int& player_height = a.Getplayerheight();

	const POINT& player_pos = a.Getposition();
	const double radial_speed = 0.0045;
	const double tangent_speed = 0.0055;
	double interval = 2 * 3.14159 / bulletlist.size();
	double radius = 100 + 25 * sin(GetTickCount() * radial_speed);
	for (int i = 0; i < bulletlist.size(); i++) {
		double radian = GetTickCount() * tangent_speed + i * interval;
		int bx = player_pos.x + player_width / 2 + radius * sin(radian);
		int by = player_pos.y + player_height / 2 + radius * cos(radian);
		bulletlist[i]->setbullet_pos(bx, by);
	}
}
class Button {
public:
	Button(RECT rect, std::string path1, std::string path2, std::string path3) {
		region = rect;
		loadimage(&img_idle, path1.c_str());
		loadimage(&img_hovered, path2.c_str());
		loadimage(&img_pushed, path3.c_str());
	}
	~Button() = default;
	void Draw() {
		switch (status) {
		case Status::idle:
			putimage(region.left, region.top, &img_idle);
			break;
		case Status::hovered:
			putimage(region.left, region.top, &img_hovered);
			break;
		case Status::pushed:
			putimage(region.left, region.top, &img_pushed);
			break;
		default:
			break;
		}

	}
	void ProcessEvent(const ExMessage& msg) {
		switch (msg.message) {
		case WM_MOUSEMOVE:
			if (status == Status::idle && checkCursor(msg.x, msg.y)) {
				status = Status::hovered;
			}
			else if (status == Status::hovered && !checkCursor(msg.x, msg.y)) {
				status = Status::idle;
			}
			break;
		case WM_LBUTTONDOWN:
			if (checkCursor(msg.x, msg.y)) {
				status = Status::pushed;
			}
			break;
		case WM_LBUTTONUP:
			if (checkCursor(msg.x, msg.y))
				onclick();
			break;
		default:
			break;
		}

	}
	virtual void onclick() = 0;
private:
	bool checkCursor(int x, int y) {
		return (x >= region.left && x <= region.right && y >= region.top && y <= region.bottom);
	}
	enum class Status {
		idle = 0,
		hovered,
		pushed
	};
	RECT region;
	IMAGE img_idle;
	IMAGE img_hovered;
	IMAGE img_pushed;
	Status status = Status::idle;
};
class start_button :public Button {
public:
	start_button(RECT rect, std::string path1, std::string path2, std::string path3) :Button(rect, path1, path2, path3) {

	}
	~start_button() = default;
	void onclick()override {
		is_game_start = true;
		mciSendString(_T("play bgm repeat"), NULL, 0, NULL);
	}
private:

};
class end_button :public Button {
public:
	end_button(RECT rect, std::string path1, std::string path2, std::string path3) :Button(rect, path1, path2, path3) {

	}
	~end_button() = default;
	void onclick()override {
		running = false;
	}
private:

};

void Draw_scores(int score) {
	std::string text;
	text = "你的得分是：" + std::to_string(score);
	setbkmode(TRANSPARENT);
	settextcolor(RGB(255, 85, 185));
	outtextxy(10, 10, text.c_str());
}
int main(void) {
	srand(time(NULL));
	HWND hwnd=initgraph(1280, 720);
	SetWindowText(hwnd, _T("游戏弱智别喷我"));
	RECT region_btn_start_game, region_btn_end_game;
	region_btn_start_game.left = (1280 - button_width) / 2;
	region_btn_start_game.right = region_btn_start_game.left + button_width;
	region_btn_start_game.top = 430;
	region_btn_start_game.bottom = region_btn_start_game.top + button_height;
	region_btn_end_game.left = (1280 - button_width) / 2;
	region_btn_end_game.right = region_btn_end_game.left + button_width;
	region_btn_end_game.top = 550;
	region_btn_end_game.bottom = region_btn_end_game.top + button_height;
	start_button startbutton(region_btn_start_game, "提瓦特幸存者\\img\\ui_start_idle.png", "提瓦特幸存者\\img\\ui_start_hovered.png", "提瓦特幸存者\\img\\ui_start_pushed.png");
	end_button endbutton(region_btn_end_game, "提瓦特幸存者\\img\\ui_quit_idle.png", "提瓦特幸存者\\img\\ui_quit_hovered.png", "提瓦特幸存者\\img\\ui_quit_pushed.png");
	anim_left_player1 = new Atlas("提瓦特幸存者\\img\\player_left_", 6);
	anim_right_player1 = new Atlas(anim_left_player1);
	anim_left_enemy1 = new Atlas("提瓦特幸存者\\img\\enemy_left_", 6);
	anim_right_enemy1 = new Atlas(anim_left_enemy1);
	anim_left_enemy1_white = new Atlas(anim_left_enemy1, true);
	anim_right_enemy1_white = new Atlas(anim_right_enemy1, true);
	anim_left_player1_white = new Atlas(anim_left_player1, true);
	anim_right_player1_white = new Atlas(anim_right_player1, true);
	mciSendString(_T("open \"提瓦特幸存者\\mus\\bgm.mp3\" alias bgm"), NULL, 0, NULL);
	mciSendString(_T("open \"提瓦特幸存者\\mus\\hit.wav\" alias hurtbgm"), NULL, 0, NULL);
	mciSendString(_T("open 提瓦特幸存者\\mus\\希望有羽毛和翅膀-知更鸟.HOYO-MiX.Chevy#2Aaesb.mp3 alias exbgm"), NULL, 0, NULL);
	BeginBatchDraw();
	ExMessage msg;
	IMAGE img_background;
	IMAGE img_menu;
	loadimage(&img_background, "提瓦特幸存者\\img\\background.png");
	loadimage(&img_menu, "提瓦特幸存者\\img\\menu.png");
	DWORD lasttime = GetTickCount();
	player paimong;
	std::vector<Enemy*> enemylist;
	std::vector<bullet*> bulletlist;
	for (int i = 0; i < 3;i++) {
		bulletlist.push_back(new bullet());
	}
	Enemy* yezhu = new Enemy();
	enemylist.push_back(yezhu);
	int scores = 0;
	while (running) {
		const DWORD start_time = GetTickCount();
		int deltal1 = start_time - lasttime;
		lasttime = start_time;
		int dir_x = 0;
		while (peekmessage(&msg)) {
			if (msg.message == WM_CLOSE) {
				running = false;
			}
			if (is_game_start)
				paimong.ProcessEvent(msg);
			else {
				startbutton.ProcessEvent(msg);
				endbutton.ProcessEvent(msg);
			}
		}
		if (is_game_start) {
			paimong.Move();
			updatebullets(bulletlist, paimong);
			for (int i = 0; i < enemylist.size(); i++) {
				enemylist[i]->Move(paimong);
			}
			for (int i = 0; i < enemylist.size(); i++) {
				for (int j = 0; j < bulletlist.size(); j++) {
					if (enemylist[i]->CheckBulletCollision(*bulletlist[j])) {
						enemylist[i]->hurt();
						mciSendString(_T("play hurtbgm from 0"), NULL, 0, NULL);
						if (enemylist[i]->hp < 0) {
							delete enemylist[i];
							std::swap(enemylist[i], enemylist.back());
							enemylist.pop_back();
							scores += 10;
							if (scores >= 50) {
								mciSendString(_T("stop bgm"), NULL, 0, NULL);
								mciSendString(_T("play exbgm"), NULL, 0, NULL);
							}
							break;
						}
					}
				}
			}
			if (is_game_over(enemylist, paimong)) {
				paimong.hurt();
				if (paimong.player_hp < 0) {
					mciSendString(_T("stop bgm"), NULL, 0, NULL);
					mciSendString(_T("stop hurtbgm"), NULL, 0, NULL);
					mciSendString(_T("stop exbgm"), NULL, 0, NULL);
					if (scores <= 200) {
						int resurt=MessageBox(GetHWnd(), _T("你死了，游戏结束了，傻子，菜就是菜！你想重新开始吗？"), _T("结束界面"), MB_YESNO|MB_ICONQUESTION);
						if (resurt == IDYES) {
							is_game_start = false;
							scores = 0;
							paimong.player_hp = 2;
							for (int i = 0; i < enemylist.size(); i++) {
								delete enemylist[i];
								
							}
							enemylist.clear();
							enemylist.push_back(new Enemy());
						}
						else {
							running = false;
						}
					}
					else {
						MessageBox(GetHWnd(), _T("看来你还是挺厉害的，不愧是你！！嘻嘻，就当夸你了哦"), _T("撒花"),MB_OK);
						running = false;
					}
				}
			}
		}
		if (is_game_start) {
			cleardevice();
			putimage(0, 0, &img_background);
			Draw_scores(scores);
			paimong.Draw(deltal1);
			for (int i = 0; i < bulletlist.size(); i++) {
				bulletlist[i]->Draw();
			}
			for (int i = 0; i < enemylist.size(); i++) {
				enemylist[i]->Draw(deltal1);
			}
			FlushBatchDraw();
			TryGenerateEnemy(enemylist, deltal1);
		}
		else {
			cleardevice();
			putimage(0, 0, &img_menu);
			startbutton.Draw();
			endbutton.Draw();
			FlushBatchDraw();
		}
		const DWORD end_time = GetTickCount() - start_time;
		if (end_time < delay) {
			Sleep(delay - end_time);
		}
	}
	EndBatchDraw();
	closegraph();
	mciSendString(_T("close bgm"), NULL, 0, NULL);
	mciSendString(_T("close hurtbgm"), NULL, 0, NULL);
	mciSendString(_T("close exbgm"), NULL, 0, NULL);
	delete anim_left_player1;
	delete anim_right_player1;
	delete anim_left_enemy1;
	delete anim_right_enemy1;
	return 0;
}
bool is_game_over(std::vector<Enemy*> enemylist, const player& a) {
	for (int i = 0; i < enemylist.size(); i++) {
		if (enemylist[i]->Enemy::CheckPlayerCollision(a)) {
			return true;
		}
	}
	return false;
}