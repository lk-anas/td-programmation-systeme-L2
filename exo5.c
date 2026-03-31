#include <stdio.h>
#include <time.h>

typedef enum {MON = 1, TUE = 2, WED = 3, THU = 4, FRI = 5, SAT = 6, SUN = 7} dayname;

typedef enum {JAN = 1, FEB = 2, MAR = 3,  APR = 4, MAY = 5, JUN = 6, JUL = 7,
              AUG = 8, SEP = 9, OCT = 10, NOV = 11, DEC = 12} monthname;

typedef struct {
  dayname weekday : 3;
  unsigned int day : 5;
  monthname month : 4;
  int year : 20;
} date;

// 1) fonction renvoyant le nom du jour qu'on lui passe en paramètre
char* dayname_str(dayname day){
  char* res;
  
  switch (day){
    case MON:
      res = "Monday";
      break;
    case TUE:
      res = "Tuesday";
      break;
    case WED:
      res = "Wednesday";
      break;
    case THU:
      res = "Thursday";
      break;
    case FRI:
      res = "Friday";
      break;
    case SAT:
      res = "Saturday";
      break;
    case SUN:
      res = "Sunday";
      break;
    default:
      res = "error";
  }
  
  return res;
}

// 2) fonction renvoyant le nom du mois qu'on lui passe en paramètre
char* monthname_str(monthname month){
    char* res;
  
  switch (month){
    case JAN:
      res = "January";
      break;
    case FEB:
      res = "Febreuary";
      break;
    case MAR:
      res = "March";
      break;
    case APR:
      res = "April";
      break;
    case MAY:
      res = "May";
      break;
    case JUN:
      res = "June";
      break;
    case JUL:
      res = "July";
      break;
    case AUG:
      res = "August";
      break;
    case SEP:
      res = "September";
      break;
    case OCT:
      res = "October";
      break;
    case NOV:
      res = "November";
      break;
    case DEC:
      res = "December";
      break;
    default:
      res = "error";
  }
  return res;
}

// 3) fonction renvoyant le jour de la semaine correspondant à la date depuis l'epoch
dayname weekday(time_t when){
  unsigned int days_since1970 = when/86400;
  dayname res = ((days_since1970 + 4 - 1) % 7) + 1;
  return res;
}

// 4) fonction indiquant si une année passé en paramètre est bysextille
int leapyear(unsigned int year){
  if( (year%4 == 0 && year % 100 != 0) || (year%400 == 0) ){
    return 1;
  } else {
    return 0;
  }
}

// 5) fonction calculant la date depuis l'epoch
date from_time(time_t when) {
  date res;
  
  unsigned int days_since1970 = when / 86400;

  unsigned int year;
  unsigned int jours_annee;
  for (year = 1970; 1; year++) {
    
    if(leapyear(year)){
      jours_annee = 366;
    } else {
      jours_annee = 365;
    }
    
    if (days_since1970 >= jours_annee) {
      days_since1970 -= jours_annee;
    } else {
      break;
    }
  }
  res.year = year;

  unsigned int jours_par_mois[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
  if (leapyear(year)){
    jours_par_mois[2] = 29;
  }
  
  monthname month;
  for (month = 1; month <= 12; month++) {
    if (days_since1970 >= jours_par_mois[month]) {
      days_since1970 -= jours_par_mois[month];
    } else {
      break;
    }
  }
  res.month = month;

  res.day = days_since1970 + 1;

  res.weekday = weekday(when);

  return res;
}

  
// 6) un main qui affiche la date du jour      
int main(void){
  date ajd = from_time(time(NULL));
  printf("%s %s %u %u", dayname_str(ajd.weekday), monthname_str(ajd.month), ajd.day, ajd.year);
}














