#ifndef _PROJECT__H_
#define _PROJECT__H_

// struct Meeting {
//     char *description;
//     int month;
//     int day;
//     int hour;
// };

int valid_time(int month, int day, int hour);
int compare_meetings(const void *a, const void *b);
int add_meeting(char *input);
int delete_meeting(char *input);
int print_calendar(char *input);
int save_to_file(char *input);
int load_from_file(char *input);

#endif //! _PROJECT__H_