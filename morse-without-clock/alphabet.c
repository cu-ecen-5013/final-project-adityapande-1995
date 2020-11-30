#include <string.h>
#include <stdio.h>

typedef struct alphabet{
  int len;
  char* code;
} alphabet;


alphabet get_morse_code(char single_char){
  if (single_char == '-'){
    return (alphabet){1, "-"};
  }

 

  if (single_char == 'A'){
    return (alphabet){2, "od"};
  }
  if (single_char == 'B'){
    return (alphabet){4,"dooo"};
  }
  if (single_char == 'C'){
    return (alphabet){4,"dodo"};
  }
  if (single_char == 'D'){
    return (alphabet){3,"doo"};
  }
  if (single_char == 'E'){
    return (alphabet){1,"o"};
  }
  if (single_char == 'F'){
    return (alphabet){4,"oodo"};
  }
  if (single_char == 'G'){
    return (alphabet){3,"ddo"};
  }
  if (single_char == 'H'){
    return (alphabet){4,"oooo"};
  }
  if (single_char == 'I'){
    return (alphabet){2,"oo"};
  }
  if (single_char == 'J'){
    return (alphabet){4,"oddd"};
  }
  if (single_char == 'K'){
    return (alphabet){3,"dod"};
  }
  if (single_char == 'L'){
    return (alphabet){4,"odoo"};
  }
  if (single_char == 'M'){
    return (alphabet){2,"dd"};
  }
  if (single_char == 'N'){
    return (alphabet){2,"do"};
  }
  if (single_char == 'O'){
    return (alphabet){3,"ddd"};
  }
  if (single_char == 'P'){
    return (alphabet){4,"oddo"};
  }
  if (single_char == 'Q'){
    return (alphabet){4,"ddod"};
  }
  if (single_char == 'R'){
    return (alphabet){3,"odo"};
  }
  if (single_char == 'S'){
    return (alphabet){3,"ooo"};
  }
  if (single_char == 'T'){
    return (alphabet){1,"d"};
  }
  if (single_char == 'U'){
    return (alphabet){3,"ood"};
  }
  if (single_char == 'V'){
    return (alphabet){4,"oood"};
  }
  if (single_char == 'W'){
    return (alphabet){3,"odd"};
  }
  if (single_char == 'X'){
    return (alphabet){4,"dood"};
  }
  if (single_char == 'Y'){
    return (alphabet){4,"dodd"};
  }
  if (single_char == 'Z'){
    return (alphabet){4,"ddoo"};
  }


  /*  Invalid char */
  return (alphabet){-1,NULL};
}

char* dot_to_char(char* code){
	if (strcmp(code, "od") == 0) return "A";
	if (strcmp(code, "dooo") == 0) return "B";
	if (strcmp(code, "dodo") == 0) return "C";
	if (strcmp(code, "doo") == 0) return "D";
	if (strcmp(code, "o") == 0) return "E";
	if (strcmp(code, "oodo") == 0) return "F";
	if (strcmp(code, "ddo") == 0) return "G";
	if (strcmp(code, "oooo") == 0) return "H";

	if (strcmp(code, "oo") == 0) return "I";
	if (strcmp(code, "oddd") == 0) return "J";
	if (strcmp(code, "dod") == 0) return "K";
	if (strcmp(code, "odoo") == 0) return "L";
	if (strcmp(code, "dd") == 0) return "M";
	if (strcmp(code, "do") == 0) return "N";
	if (strcmp(code, "ddd") == 0) return "O";
	if (strcmp(code, "oddo") == 0) return "P";
	if (strcmp(code, "ddod") == 0) return "Q";
	if (strcmp(code, "odo") == 0) return "R";

	if (strcmp(code, "ooo") == 0) return "S";
	if (strcmp(code, "d") == 0) return "T";
	if (strcmp(code, "ood") == 0) return "U";
	if (strcmp(code, "oood") == 0) return "V";
	if (strcmp(code, "odd") == 0) return "W";
	if (strcmp(code, "dood") == 0) return "X";
	if (strcmp(code, "dodd") == 0) return "Y";
	if (strcmp(code, "ddoo") == 0) return "Z";

	return NULL;
}

void decode(int one_zero[], int index){
  printf("\nTotal bits to be read :, %d", index);
  char msg[1000];
  strcpy(msg,"");

  int i = 0;
  // Removing leading zeros
  while (one_zero[i] == 0) i++;

  while (i < index){
    //printf("\nReading message..");
   if(one_zero[i] == 0 && one_zero[i+1] == 0 && one_zero[i+2] == 0){
      //printf("\n Got EOW");
     i += 3;
      strcat(msg, "|");
      continue;
    }

   if(one_zero[i] == 1 && one_zero[i+1] == 1 && one_zero[i+2] == 1 && one_zero[i+3] == 1 &&one_zero[i+4] ==0){
      //printf("\n Got EOC");
     i += 5;
      strcat(msg, "-");
      continue;
    }

    if(one_zero[i] == 1 && one_zero[i+1] == 0){
      //printf("\n Got dot");
      i = i+2;
      strcat(msg, "o");
      continue;
    }
    if(one_zero[i] == 1 && one_zero[i+1] == 1 && one_zero[i+2] == 1 && one_zero[i+3] == 0){
      //printf("\n Got dash");
      i = i+4;
      strcat(msg, "d");
      continue;
    }

    printf("\nCorrupt message received !\n");
    break;

 }


  printf("\nDot dash form : %s\n\n", msg);
  printf("\nText form: ");
  char* token = strtok(msg, "-");
  while (token != NULL){
	  fprintf(stderr,"\n Token: %s, alphabet: %s", token, dot_to_char(token));
	  token = strtok(NULL, "-");
  }
}

