#include <stdio.h>
#include <ncurses.h>
#include <menu.h>
#include <stdlib.h>
#include <form.h>
#include <string.h>
#include <libpq-fe.h>

#define WIDTH 30
#define HEIGHT 10
#define MAX_STR 64


struct menuChoices{
    char name[MAX_STR];
    char descr[MAX_STR];
};

struct date{
  int month;
  int day;
  int year;
};

struct member{
  char fname[MAX_STR];
  char lname[MAX_STR];
  int weight;
  int height;
  int stepsTaken;
  int caloriesBurnt;
  struct date dob;  
  float avgSleep;


};

void mainMenu();
void loginScreen(char*, char*);
int logregscr();
int regScreen();
int memtrainscr();

int addStudent(PGconn* conn, char* fname, char* lname, char* height, int month, int day, int year){
  char query[256] = "";
  char smonth[MAX_STR];
  char sday[MAX_STR];
  char syear[MAX_STR];
  strcat(query, "INSERT INTO Members(fname, lname, height, dob)");
  strcat(query, " VALUES ('");
  strcat(query, fname);
  strcat(query, "', '");
  strcat(query, lname);
  strcat(query, "', ");
  strcat(query, height);
  strcat(query, ", '");
  snprintf(smonth, 10, "%d", month);
  snprintf(sday, 10, "%d", day);
  snprintf(syear, 10, "%d", year);
  strcat(query, syear);
  strcat(query, "-");
  strcat(query, smonth);
  strcat(query, "-");
  strcat(query, sday);
  strcat(query, "');");
  printf("Query: %s \n", query);
  PGresult *res = PQexec(conn, query);
  if(PQresultStatus(res) != PGRES_TUPLES_OK)
    printf("Failed to add query \n");
  return 0;
} 
void do_exit(PGconn *conn){
    PQfinish(conn);
    exit(1);
}


int main(int argc, char *argv[]){
  PGconn *conn =  PQconnectdb("user=postgres password=postgres dbname=health");
  if(PQstatus(conn) == CONNECTION_BAD){
    printf("unable to connect \n");
    do_exit(conn);
  }

  char userName[64];
  char password[64];
  struct member mem;
  initscr();  // initializes the terminal in curses
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  int logregchoice;
  if(memtrainscr() == 0)
    logregchoice = logregscr();

  if(logregchoice == 0)
    loginScreen(userName, password);
  else if(logregchoice == 1){
    regScreen(&mem);
    char sheight[MAX_STR];
    sprintf(sheight, "%d", mem.height);
    endwin();
    addStudent(conn, mem.fname, mem.lname, sheight, mem.dob.month, mem.dob.day, mem.dob.year);
  };
    
  endwin();
  //printf("%s %s \n", userName, password);
  //mainMenu();
  return 0;

}

void loginScreen(char* userName, char* password){
    FIELD *field[5];
	FORM  *loginForm;
	WINDOW *loginwin;
    WINDOW *formwin;
    char usernameField[MAX_STR] = "";
    char passwordField[MAX_STR] = "";
    int fieldNum = 0;
	int ch, rows, cols;
    int screenx, screeny;
    int windowSizeX, windowSizeY;
    int fieldWidth = 30;
    getmaxyx(stdscr, screeny, screenx);
	


	field[0] = new_field(1, fieldWidth, 6, 1, 0, 0);
	field[1] = new_field(1, fieldWidth, 8, 1, 0, 0);
	field[2] = new_field(1, fieldWidth, 10, 1, 0, 0);
	field[3] = new_field(1, fieldWidth, 12, 1, 0, 0);
	field[4] = NULL;
  
	field_opts_off(field[0], O_ACTIVE);
	field_opts_off(field[0], O_EDIT);
	set_field_back(field[1], A_UNDERLINE);
	field_opts_off(field[1], O_AUTOSKIP); 
	field_opts_off(field[2], O_ACTIVE);
	field_opts_off(field[2], O_EDIT);
	set_field_back(field[3], A_UNDERLINE); 
	field_opts_off(field[3], O_AUTOSKIP);
	
	loginForm = new_form(field);
	
	scale_form(loginForm, &rows, &cols);
    windowSizeY = rows + 4;
    windowSizeX = cols + 4;

    loginwin = newwin(windowSizeY, windowSizeX, screeny / 2 - rows + 4, screenx / 2 - cols + 4);
    keypad(loginwin, TRUE);

    set_form_win(loginForm, loginwin);
    formwin = derwin(loginwin, rows, cols, 2, 2);
    set_form_sub(loginForm, formwin);

    box(loginwin, 0, 0);
	post_form(loginForm);
    mvwprintw(loginwin, 2, windowSizeX / 2 - 2, "Login");
    mvwprintw(formwin, 6, 1, "Username");
    mvwprintw(formwin, 10, 1, "Password");
	wrefresh(loginwin);
    

	while((ch = wgetch(loginwin)) != KEY_F(1))
	{	switch(ch)
		{	case KEY_DOWN:
				form_driver(loginForm, REQ_NEXT_FIELD);
				form_driver(loginForm, REQ_END_LINE);
                if(fieldNum == 0){
                    fieldNum = 1;
                } else 
                    fieldNum = 0;
				break;
			case KEY_UP:
				form_driver(loginForm, REQ_PREV_FIELD);
				form_driver(loginForm, REQ_END_LINE);
                if(fieldNum == 0){
                    fieldNum = 1;
                } else if(fieldNum == 1)
                    fieldNum = 0;

				break;
            case KEY_BACKSPACE:
                form_driver(loginForm, REQ_DEL_PREV);
                if(fieldNum == 0)
                    usernameField[strlen(usernameField)-1] = '\0';
                else if(fieldNum == 1) 
                    passwordField[strlen(passwordField)-1] = '\0';
                break;
            case KEY_STAB:
                form_driver(loginForm, REQ_NEXT_FIELD);
                form_driver(loginForm, REQ_END_LINE);
            case KEY_HOME:
                mvwprintw(loginwin, 2, windowSizeX / 2 - 2, "Enter");
                wrefresh(loginwin);
                strcpy(userName, usernameField);
                strcpy(password, passwordField);
                unpost_form(loginForm);
                free_form(loginForm);
                clear();
                return;
                break;
			default:
				form_driver(loginForm, ch);
                if(fieldNum == 0)
                    strncat(usernameField, (char*)&ch, 1);
                else if(fieldNum == 1)
                    strncat(passwordField, (char*)&ch, 1);
				break;
		}
	}

	unpost_form(loginForm);
	free_form(loginForm);
	free_field(field[0]);
	free_field(field[1]); 


    endwin();

}

int regScreen(struct member* mem){

  char* fieldStr[]={
    "First Name",
    "Last Name",
    "Month (DOB)",
    "Day (DOB)",
    "Year (DOB)",
    "Weight",
    "Height",
    "Average Time Sleep",
  };
  char smonth[MAX_STR] = "";
  char sday[MAX_STR] = "";
  char syear[MAX_STR] = "";
  char sweight[MAX_STR] = "";
  char sheight[MAX_STR] = "";
  char savgslp[MAX_STR] = "";


  int numOfFields = 20;
  int currField = 0;
  FIELD *field[numOfFields + 1];
  FORM  *regform;
  WINDOW *regwin;
  WINDOW *formwin;
  

  struct member newMember;
  strcpy(newMember.fname, "");
  strcpy(newMember.lname, "");
  int ch;
  int rows, cols;
  int fieldWidth = 30;

  // screen size of entire terminal
  int screenx, screeny;

  // Window size of our regwin
  int windowSizeY, windowSizeX;

  getmaxyx(stdscr, screeny, screenx);
  for(int i = 0; i < numOfFields; ++i){
    field[i] = new_field(1, fieldWidth, 6 + (i * 2), 1, 0, 0);
    set_field_back(field[i], A_UNDERLINE);
    field_opts_off(field[i], O_AUTOSKIP); 
    if(i % 2 == 0){
      field_opts_off(field[i], O_ACTIVE);
      field_opts_off(field[i], O_EDIT);
      set_field_back(field[i], A_NORMAL);
    } 

  }


  field[numOfFields] = NULL;

  regform = new_form(field);

  scale_form(regform, &rows, &cols);
  windowSizeX = cols + 4;
  windowSizeY = rows + 4;

  regwin = newwin(windowSizeY, windowSizeX, screeny / 2 - windowSizeY/2, screenx / 2 - windowSizeX/2);
  keypad(regwin, TRUE);

  set_form_win(regform, regwin);
  formwin = derwin(regwin, rows, cols, 2, 2);
  set_form_sub(regform, formwin);

  box(formwin, 0, 0);
  box(regwin, 0, 0);
  post_form(regform);

  mvwprintw(regwin, 2, windowSizeX / 2 - 10, "Member Registration");
  for(int i = 0; i < sizeof(fieldStr)/sizeof(fieldStr[0]); ++i){
      mvwprintw(formwin, 6 + (4 * i), 1, fieldStr[i]);
  }

  wrefresh(regwin);

  ch = '\0';
	while((ch = wgetch(regwin)) != KEY_F(1))
	{	
    switch(ch)
		{	case KEY_DOWN:
				/* Go to next field */
				form_driver(regform, REQ_NEXT_FIELD);
				/* Go to the end of the present buffer */
				/* Leaves nicely at the last character */
				form_driver(regform, REQ_END_LINE);
                switch(currField){
                    case 0:
                      currField = 1;
                      break;
                    case 1:
                      currField = 2;
                      break;
                    case 2:
                      currField = 3;
                      break;
                    case 3:
                      currField = 4;
                      break;
                    case 4:
                      currField = 5;
                      break;
                    case 5:
                      currField = 6;
                      break;
                    case 6:
                      currField = 7;
                      break;
                    case 7:
                      currField = 0;
                      break;
                }
				break;
			case KEY_UP:
				/* Go to previous field */
				form_driver(regform, REQ_PREV_FIELD);
				form_driver(regform, REQ_END_LINE);
                switch(currField){
                    case 0:
                      currField = 7;
                      break;
                    case 1:
                      currField = 0;
                      break;
                    case 2:
                      currField = 1;
                      break;
                    case 3:
                      currField = 2;
                      break;
                    case 4:
                      currField = 3;
                      break;
                    case 5:
                      currField = 4;
                      break;
                    case 6:
                      currField = 5;
                      break;
                    case 7:
                      currField = 6;
                      break;
                }
				break;
            case KEY_BACKSPACE:
                //form_driver(regform, REQ_LEFT_CHAR);
                form_driver(regform, REQ_DEL_PREV);
                switch(currField){
                    case 0:
                      newMember.fname[strlen(newMember.fname) - 1] = '\0';
                      break;
                    case 1:
                      newMember.lname[strlen(newMember.lname) - 1] = '\0';
                      break;
                    case 2:
                      currField = 1;
                      break;
                    case 3:
                      currField = 2;
                      break;
                    case 4:
                      currField = 3;
                      break;
                    case 5:
                      currField = 4;
                      break;
                    case 6:
                      currField = 5;
                      break;
                    case 7:
                      currField = 6;
                      break;
                }
                break;
            case KEY_STAB:
                form_driver(regform, REQ_NEXT_FIELD);
                form_driver(regform, REQ_END_LINE);
            case KEY_HOME:
                //mvwprintw(, 2, windowSizeX / 2 - 2, "Enter");
                wrefresh(regwin);
                strcpy(mem->fname, newMember.fname);
                strcpy(mem->lname, newMember.lname);
                mem->dob.month = atoi(smonth);
                mem->dob.day =  atoi(sday);
                mem->dob.year =  atoi(syear);
                mem->weight = atoi(sweight);
                mem->height = atoi(sheight);
                mem->avgSleep = atoi(savgslp);
                unpost_form(regform);
                free_form(regform);
                clear();
                return 0;
                break;
		  default:
				/* If this is a normal character, it gets */
				/* Printed				  */	
				form_driver(regform, ch);
              if(ch != '\0'){
                switch(currField){
                    case 0:
                      strncat(newMember.fname, (char*)&ch, 1);
                      break;
                    case 1:
                      strncat(newMember.lname, (char*)&ch, 1);
                      break;
                    case 2:
                      strncat(smonth, (char*)&ch, 1);
                      break;
                    case 3:
                      strncat(sday, (char*)&ch, 1);
                      break;
                    case 4:
                      strncat(syear, (char*)&ch, 1);
                      break;
                    case 5:
                      strncat(sweight, (char*)&ch, 1);
                      break;
                    case 6:
                      strncat(sheight, (char*)&ch, 1);
                      break;
                    case 7:
                      strncat(savgslp, (char*)&ch, 1);
                      break;
                }
              }
				break;
		}
	}


}

  

int logregscr(){
  char *choices[] = {
    "Login",
    "Register",
    "Exit",
  };
  int n_choices = 3;
  WINDOW *logregwin;
  MENU *logregmenu;
  ITEM* myitems[4];
  int ch;
  int startx, starty;
  int screenx, screeny;
  int height = 10;
  int width = 40;
  int highlight = 1;
  int choice = 0;
  int c;

  for(int i = 0; i < 3; ++i)
    myitems[i] = new_item(choices[i], "");
  myitems[3] = NULL;

  logregmenu = new_menu(myitems);

  getmaxyx(stdscr, screeny, screenx);
  logregwin = newwin(height, width, screeny / 2 - height, screenx / 2 - width);
  keypad(logregwin, TRUE);

  set_menu_win(logregmenu, logregwin);
  set_menu_sub(logregmenu, derwin(logregwin, 6, 38, 3, 1));

  set_menu_mark(logregmenu, "*");

  box(logregwin, 0, 0);
  refresh();

  post_menu(logregmenu);
  mvwprintw(logregwin, 1, width / 2, "Members");
  wrefresh(logregwin);

  
  while((ch = wgetch(logregwin)) != KEY_F(1)){
    switch(ch){
        case KEY_DOWN:
          menu_driver(logregmenu, REQ_DOWN_ITEM);
          if(choice == 0)
            choice = 1;
          else if(choice == 1)
            choice = 2;
          break;
        case KEY_UP:
          menu_driver(logregmenu, REQ_UP_ITEM);
          if(choice == 1)
            choice = 0;
          else if(choice == 2)
            choice = 1;
          break;
        case KEY_HOME:
          unpost_menu(logregmenu);
          werase(logregwin);
          free_menu(logregmenu);
          return choice;
    }
    wrefresh(logregwin);
  }
   

}

int memtrainscr(){
  char *choices[] = {
    "Members",
    "Trainers",
    "Exit", 
  };
  int n_choices = 3;
  WINDOW *memtrainwin;
  MENU *memtrainmenu;
  ITEM* myitems[4];
  int ch;
  int startx, starty;
  int screenx, screeny;
  int height = 10;
  int width = 40;
  int highlight = 1;
  int choice = 0;
  int c;

  for(int i = 0; i < 3; ++i)
    myitems[i] = new_item(choices[i], "");
  myitems[3] = NULL;

  memtrainmenu = new_menu(myitems);

  getmaxyx(stdscr, screeny, screenx);
  memtrainwin = newwin(height, width, screeny / 2 - height, screenx / 2 - width);
  keypad(memtrainwin, TRUE);

  set_menu_win(memtrainmenu, memtrainwin);
  set_menu_sub(memtrainmenu, derwin(memtrainwin, 6, 38, 3, 1));

  set_menu_mark(memtrainmenu, "*");

  box(memtrainwin, 0, 0);
  refresh();

  post_menu(memtrainmenu);
  mvwprintw(memtrainwin, 1, 3, "Health Fitness Management");
  wrefresh(memtrainwin);

  
  while((ch = wgetch(memtrainwin)) != KEY_F(1)){
    switch(ch){
        case KEY_DOWN:
          menu_driver(memtrainmenu, REQ_DOWN_ITEM);
          if(choice == 0)
            choice = 1;
          else if(choice == 1)
            choice = 2;
          break;
        case KEY_UP:
          menu_driver(memtrainmenu, REQ_UP_ITEM);
          if(choice == 1)
            choice = 0;
          else if(choice == 2)
            choice = 1;
          break;
        case KEY_HOME:
          unpost_menu(memtrainmenu);
          werase(memtrainwin);
          free_menu(memtrainmenu);
          return choice;
    }
    wrefresh(memtrainwin);
  }
   

}





