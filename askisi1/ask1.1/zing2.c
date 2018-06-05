#include <stdio.h>
#include <unistd.h>

void zing()
{

char* username = getlogin();
printf("Good morning sunshines: ");
printf(username);

}
