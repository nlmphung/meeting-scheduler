#include "project.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



// TODO:: implement your project here!

/* 
This is a scheduling system that can be used to schedule one-hour meeting times. A meeting is
represented with the following data:
- description: a string that can have arbitrary length. It may be the same for different meetings
- month: month of meeting time
- day: day of month of meeting time
- hour: hour of the day of the meeting time, which is between 0 and 23.
For the system, each meeting time must be unique. It should be able to support arbitrary number of meetings.

*/

//functions for each command

struct Meeting {
    char *description;
    int month;
    int day;
    int hour;
};

struct Meeting *meetings = NULL;
int meeting_count = 0;

//helper function to validate month, day, and hour
int valid_time(int month, int day, int hour) {
    if (month < 1 || month > 12) {
        printf("Error: Invalid month.\n");
        return 0;
    }

    if (day < 1 || day > 31) {
        printf("Error: Invalid day.\n");
        return 0;
    }

    if (hour < 0 || hour > 23) {
        printf("Error: Invalid hour.\n");
        return 0;
    }
    return 1;
}

//add meeting that adds a meeting with user input format A <description> <month> <day> <hour>
int add_meeting(char *input) {
    char description[1000];
    int month, day, hour;

    //parse the input
    int parsed = sscanf(input, "A %s %d %d %d", description, &month, &day, &hour);
    if (parsed != 4) {
        printf("Error: Wrong format.\n");
        return -1;  
    }

    //check if the month, day, hour are valid
    if (!valid_time(month, day, hour)) {
        return -1;
    }

    //check for time conflicts
    for (int i = 0; i < meeting_count; i++) {
        if (meetings[i].month == month && meetings[i].day == day && 
            meetings[i].hour == hour) {
            printf("Error: Time conflict.\n");
            return -1;
        }
    }

    //alloc for new meeting
    struct Meeting *new = realloc(meetings, sizeof(struct Meeting) * (meeting_count + 1));
    if (new == NULL) {
        printf("Error: Memory allocation failed.\n");
        return -1;
    }
    meetings = new;

    //alloc for description
    int des_length = strlen(description);
    meetings[meeting_count].description = malloc(des_length + 1);
    if (meetings[meeting_count].description == NULL) {
        printf("Error: Memory allocation failed.\n");
        return -1;
    }
    //store description
    strcpy(meetings[meeting_count].description, description);

    //store month, day, hour
    meetings[meeting_count].month = month;
    meetings[meeting_count].day = day;
    meetings[meeting_count].hour = hour;

    meeting_count+= 1;
    return 0;
}

//delete meeting function
int delete_meeting(char *input) {
    int month, day, hour;

    //parse the input
    int parsed = sscanf(input, "D %d %d %d", &month, &day, &hour);
    if (parsed != 3) {
        printf("Error: Wrong format.\n");
        return -1;
    }

    //find the meeting
    int found = 0;
    for (int i = 0; i < meeting_count; i++) {
        if (meetings[i].month == month && meetings[i].day == day 
            && meetings[i].hour == hour) {
            free(meetings[i].description);
            //move the other meetings 
            for (int j = i; j < meeting_count - 1; j++) {
                meetings[j] = meetings[j + 1];
            }
            found = 1;
            meeting_count -= 1;

            //check if meeting_count is 0 then free the meetings array, else realloc
            if (meeting_count == 0) {
                free(meetings);
                meetings = NULL;
            } else {
                struct Meeting *new = realloc(meetings, sizeof(struct Meeting) * meeting_count);
                if (new == NULL) {
                    printf("Memory reallocation failed.\n");
                } else {
                    meetings = new;
                }
            }
            
            return 0;
        }
    }
    //if not found, return error
    if (!found) {
        printf("Error: Meeting not found.\n");
        return -1;
    }

    return -1;
}

//print calendar function
//helper function to compare the meeting times using qsort
int compare_meetings(const void *a, const void *b) {
    struct Meeting *meeting_a = (struct Meeting*) a;
    struct Meeting *meeting_b = (struct Meeting*) b;
    //compare month -> day -> hour
    if (meeting_a->month != meeting_b->month) {
        return meeting_a->month - meeting_b->month;
    }
    if (meeting_a->day != meeting_b->day) {
        return meeting_a->day - meeting_b->day;
    }
    //if all equal, return hour difference
    return meeting_a->hour - meeting_b->hour;
}

int print_calendar(char *input) {
    //check if input is exactly "L\n"
    if (strcmp(input, "L\n") != 0) {
        printf("Error: Wrong format.\n");
        return -1;
    }
    //use qsort to sort the meetings in order
    if (meeting_count > 0) {
        qsort(meetings, meeting_count, sizeof(struct Meeting), compare_meetings);

        //print the meetings in order
        for (int i = 0; i < meeting_count; i++) {
            printf("%s %02d.%02d at %02d\n", meetings[i].description, meetings[i].day, meetings[i].month, meetings[i].hour);
        }
    }
    return 0;
}

//save to file function
int save_to_file(char *input) {
    char filename[1000];
    //parse the input
    int parsed = sscanf(input, "W %s", filename);
    if (parsed != 1) {
        printf("Error: Wrong format.\n");
        return -1;
    }

    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error: Cannot open file.\n");
        return -1;
    }

    if (meeting_count > 0) {
        //sort the meetings in order
        qsort(meetings, meeting_count, sizeof(struct Meeting), compare_meetings);
        //print the meetings to file
        for (int i = 0; i < meeting_count; i++) {
            fprintf(file, "%s %02d.%02d at %02d\n", meetings[i].description, meetings[i].day, 
                    meetings[i].month, meetings[i].hour);
        }
    }

    fclose(file);
    return 0;
}

//load from file function
//NOTE: in my instructions, there is a requirement "If the file is valid and loaded correctly, 
//the current game must be replaced with the entries loaded from the file." I don't understand
//what "game" means in this context, so I assumed it means the meeting array
int load_from_file(char *input) {
    char filename[1000];
    //parse the input
    int parsed = sscanf(input, "O %s", filename);
    if (parsed != 1) {
        printf("Error: Wrong format.\n");
        return -1;
    }

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Cannot open file.\n");
        return -1;
    }

    //array for temporary new meetings
    struct Meeting *new_meetings = NULL;
    int new_meeting_count = 0;
    char line[1000];

    while (fgets(line, sizeof(line), file)) {
        //parse the line
        char description[1000];
        int month, day, hour;
        int parsed = sscanf(line, "%s %d.%d at %d", description, &day, &month, &hour);
        if (parsed != 4) {
            printf("Error: Wrong format.\n");
            fclose(file);
            for (int i = 0; i < new_meeting_count; i++) {
                free(new_meetings[i].description);
            }
            free(new_meetings);
            return -1;
        }

        //alloc for new meeting
        struct Meeting *new_temp = realloc(new_meetings, sizeof(struct Meeting) * (new_meeting_count + 1));
        if (new_temp == NULL) {
            printf("Error: Memory allocation failed.\n");
            fclose(file);
            for (int i = 0; i < new_meeting_count; i++) {
                free(new_meetings[i].description);
            }
            free(new_meetings);
            return -1;
        }
        new_meetings = new_temp;

        //alloc for description
        int des_length = strlen(description);
        new_meetings[new_meeting_count].description = malloc(des_length + 1);
        if (new_meetings[new_meeting_count].description == NULL) {
            printf("Error: Memory allocation failed.\n");
            fclose(file);
            for (int i = 0; i < new_meeting_count; i++) {
                free(new_meetings[i].description);
            }
            free(new_meetings);
            return -1;
        }
        //store description
        strcpy(new_meetings[new_meeting_count].description, description);

        //store month, day, hour
        new_meetings[new_meeting_count].month = month;
        new_meetings[new_meeting_count].day = day;
        new_meetings[new_meeting_count].hour = hour;

        new_meeting_count += 1;
    }
    fclose(file);

    //free the current meeting array
    for (int i = 0; i < meeting_count; i++) {
        free(meetings[i].description);
    }
    free(meetings);

    meetings = new_meetings;
    meeting_count = new_meeting_count;

    return 0;
}

int main(void) {
    // The magic starts here
    // Make an infinite while loop inside your main function. Your application should terminate only when it receives Q command.

    while (1) {
        //read a line from user
        char input[1000];
        if (fgets(input, sizeof(input), stdin) == NULL) {
            for (int i = 0; i < meeting_count; i++) {
                free(meetings[i].description);
            }
            free(meetings);
            break;
        }
        //get the first character 
        char command = input[0];
        switch (command) {
            case 'Q':
                //free all allocated memory
                for (int i = 0; i < meeting_count; i++) {
                    free(meetings[i].description);
                }
                free(meetings);
                printf("SUCCESS\n");
                return 0;

            //add meeting command with format A <description> <month> <day> <hour>
            case 'A':
                if (add_meeting(input) == 0) {
                    printf("SUCCESS\n");
                }
                break;

            case 'D':
                if (delete_meeting(input) == 0) {
                    printf("SUCCESS\n");
                }
                break;

            case 'L':
                if (print_calendar(input) == 0) {
                    printf("SUCCESS\n");
                }
                break;

            case 'W':
                if (save_to_file(input) == 0) {
                    printf("SUCCESS\n");
                }
                break;

            case 'O':
                if (load_from_file(input) == 0) {
                    printf("SUCCESS\n");
                }
                break;

            default:
                //if none of the above commands, print "Invalid command <command>"
                printf("Invalid command %c\n", command);
                break;

        }

    }
}