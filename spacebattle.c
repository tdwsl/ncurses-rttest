#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define WIDTH 80
#define HEIGHT 24
#define FIELDW WIDTH
#define FIELDH HEIGHT
#define MAX_SPEED 10

static const int dirs[] = {
	0, -1,
	1, -1,
	1, 0,
	1, 1,
	0, 1,
	-1, 1,
	-1, 0,
	-1, -1,
};

struct ship {
	int x, y, xspeed, yspeed, dir, acc;
};

struct ship player;
int tick;

void init_curses() {
	initscr();
	keypad(stdscr, 1);
	curs_set(0);
	timeout(0);
}

void end_curses() {
	curs_set(1);
	timeout(-1);
	keypad(stdscr, 0);
	endwin();
}

void draw_ship(struct ship *s) {
	mvaddch(s->y, s->x, 'o');
	mvaddch(s->y+dirs[s->dir*2+1], s->x+dirs[s->dir*2], '+');
	mvaddch(s->y-dirs[s->dir*2+1], s->x-dirs[s->dir*2], '=');
}

void update_ship(struct ship *s, int tim) {
	int tx = s->acc*dirs[s->dir*2];
	int ty = s->acc*dirs[s->dir*2+1];
	if(s->xspeed < tx)
		s->xspeed++;
	if(s->xspeed > tx)
		s->xspeed--;
	if(s->yspeed < ty)
		s->yspeed++;
	if(s->yspeed > ty)
		s->yspeed--;

	if(!s->xspeed && !s->yspeed)
		return;

	int x = MAX_SPEED - abs(s->xspeed) + 1;
	int y = MAX_SPEED - abs(s->yspeed) + 1;
	if(tim % x == 0) {
		if(s->xspeed < 0)
			s->x--;
		if(s->xspeed > 0)
			s->x++;
	}
	if(tim % y == 0) {
		if(s->yspeed < 0)
			s->y--;
		if(s->yspeed > 0)
			s->y++;
	}

	if(s->x < 0)
		s->x += FIELDW;
	if(s->x >= FIELDW)
		s->x -= FIELDW;
	if(s->y < 0)
		s->y += FIELDH;
	if(s->y >= FIELDH)
		s->y -= FIELDH;
}

void control_ship(struct ship *s) {
	switch(getch()) {
	case KEY_LEFT:
		s->dir--;
		if(s->dir < 0)
			s->dir = 7;
		break;
	case KEY_RIGHT:
		s->dir++;
		s->dir %= 8;
		break;
	case KEY_UP:
		if(s->acc < MAX_SPEED)
			s->acc++;
		break;
	case KEY_DOWN:
		if(s->acc > 0)
			s->acc--;
		break;
	}
}

void update() {
	update_ship(&player, tick);
	tick++;
	tick %= (MAX_SPEED+1);
}

void draw() {
	clear();
	draw_ship(&player);
	refresh();
}

int main() {
	init_curses();

	player.x = FIELDW/2;
	player.y = FIELDH/2;
	player.dir = 0;
	player.acc = 0;
	player.xspeed = 0;
	player.yspeed = 0;

	tick = 0;

	clock_t last_time = clock();
	int redraw = 3;

	while(1) {
		if(redraw >= 3) {
			draw();
			redraw = 0;
		}
		control_ship(&player);

		clock_t current_time = clock();
		float diff = (float)(current_time-last_time)/CLOCKS_PER_SEC;
		if(diff > 0.1) {
			redraw++;
			update();
			last_time = current_time;
		}
	}

	end_curses();
	return 0;
}
