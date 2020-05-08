#include "pong.hpp"

#define DELAY 10000
#define SIZE 5

int a,bounces=1,
  time_tracker=0,
  refresh_rate=100;

struct position { int x; int y;};

position ball;
position player;
position max;
position direction;


int main(){
  screen_setup();
  game_setup();
  
  //close game by pressing q
  while (a!='q') {
    time_tracker++;
    getmaxyx(stdscr,max.y,max.x);

//if ( (time_tracker % 1000) == 0)
//     change_refresh_rate();
    
    check_x();
    check_y();

    if ( ( time_tracker % (refresh_rate/4/bounces) ) == 0)
      move_ball();

    if ( (time_tracker/bounces) % (refresh_rate/bounces) == 0)
      {
	erase();
	a = wgetch(stdscr);
	if ( a == ' ' )
	  pause_game();	

	player_movement();
	print_info();
	print_player();
	print_ball();
	refresh();
	usleep(DELAY);
      }
  }
  
  quit_game();
}

void quit_game()
{
  endwin();
  exit(0);
}

void change_refresh_rate(void)
{
  refresh_rate-=1;
}

void player_movement()
{
    switch(a)
      {
      case(KEY_DOWN):
  	player.y+=1;
  	break;
      case(KEY_UP):
  	player.y-=1;
  	break;
      }

    if(player.y<0)
      player.y=0;
    else if(player.y+SIZE>max.y)
      player.y=max.y-SIZE;
}

void print_player()
{
  for(int i = 0; i<SIZE; i++)
    mvprintw(player.y+i,player.x,"|");
}

void print_info()
{
  mvprintw(0,max.x/2,"(%d,%d) %d",ball.x,ball.y,time_tracker);
  mvprintw(1,max.x/2,"(%d,%d)",player.x,player.y);
}

void check_y()
{  
  if(ball.y > max.y)
    direction.y = -1;
  else if(ball.y-1 < 0)
    direction.y = 1;
}

void check_x()
{
  // is it at x=0 or x=max.x?
  if(ball.x > max.x-1){
	  bounces++;
	  direction.x = -1;
  }
  if ((ball.x-1 < 0) && (direction.x == -1))
    {
      direction.x = 1;
      // if y is not between player.y and
      // player.y+SIZE then game over
      missed();
    }
    timeout(0);
}

void print_debug_info()
{
  mvprintw(max.y/2+1,max.x/2,"ball: (%d,%d)",ball.x,ball.y);
  mvprintw(max.y/2+2,max.x/2,"play: (%d,%d)",player.x,player.y);
}

void missed()
{
  // print_debug_info();
  // refresh();
  // timeout(-1);
  // a = getch();
  // timeout(0);

  if( (ball.y < player.y) || (ball.y >= (player.y + SIZE)))
    gameover();
}

void gameover()
{
  clear();
  mvprintw(max.y/2,max.x/2,"GAME OVER");
  print_debug_info();
  refresh();

  sleep(2);

  timeout(-1);
  a = getch();
  time_tracker = 0;
  timeout(0);
}

void print_ball()
{
  mvprintw(ball.y,ball.x,"o");
}

void move_ball()
{
  ball.y += direction.y;
  ball.x += direction.x;
}

void screen_setup()
{
  initscr();
  noecho();
  curs_set(FALSE);
  getmaxyx(stdscr,max.y,max.x);
  keypad(stdscr,TRUE);
}

void game_setup()
{
  timeout(0);
  ball = {.x = max.x, .y = (rand()%max.y)};
  player = {.x=0, .y=0};
  direction = {.x=-1, .y=-1};
}

void pause_game()
{
  char pause_msg[] = "Paused";

  position pause_sz = {.x=30, .y=10};
  
  WINDOW*pause_win = derwin(stdscr,pause_sz.y,pause_sz.x,
			    (max.y-pause_sz.y)/2,
			    (max.x-pause_sz.x)/2);
  box(pause_win,'|','-');
  position pause_max;
  getmaxyx(pause_win,pause_max.y,pause_max.x);
  mvwprintw(pause_win,pause_max.y/2,
	    pause_max.x/2-strlen(pause_msg)/2,"%s",
	    pause_msg);
  wrefresh(pause_win);

  wtimeout(pause_win,-1);
  for (int i = 0;;i++)
  {
    do{a = getch();}while(a==ERR);

    wrefresh(stdscr);
    if (a==' '){
      break;
      // goto LEAVE;
    }
    if (a=='q')
      quit_game();
  }

  delwin(pause_win);
  timeout(0);
}
