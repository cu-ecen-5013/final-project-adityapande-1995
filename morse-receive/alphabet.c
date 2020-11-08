
typedef struct alphabet{
  int len;
  char* code;
} alphabet;


alphabet get_morse_code(char single_char){

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

