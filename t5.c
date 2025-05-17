#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>

int booleanCounter = 0, helpCounter = 0;

void checkMemoryAllocation(char *pointer)
{
	if (!pointer)
	{
		printf("Ошибка выделения памяти\n");
		exit(1);
	}
}

void checkMemoryAllocationOnPointer(char **pointer)
{
	if (!pointer)
	{
		printf("Ошибка выделения памяти\n");
		exit(1);
	}
}

char* getString()
{
	int sizeString = 20 * sizeof(char), lengthString = 0;
	char *string = malloc(sizeString);
	
	checkMemoryAllocation(string);
		
	while (fgets(string + lengthString, 20, stdin)) 
	{
		lengthString = strlen(string);	
		if (string[lengthString - 1] != '\n')
		{
			sizeString += 20 * sizeof(char);
			string = realloc(string, sizeString);
			checkMemoryAllocation(string);
			continue;
		}
		
		else return string;
	}
	if (!booleanCounter)
	{
		++booleanCounter;
		sizeString += sizeof(char);
		string = realloc(string, sizeString);
		checkMemoryAllocation(string);
		string[lengthString] = '\n';
		return string;
	}
	return NULL;
}

int checkForQuotes(char *string)
{
	int pointerString = 0, amountOfQuotes = 0;
	
	for (pointerString = 0; pointerString < strlen(string); ++pointerString)
		if (string[pointerString] == '"') ++amountOfQuotes;
	
	return amountOfQuotes;
}

void recordWord(char **arrayOfWord, char *word, int *pointerArray)
{
	arrayOfWord[*pointerArray] = word;
	*pointerArray += 1;
}

int checkControlCharacter(char *string, int *pointerString, char **controlCharacterWord)
{
	int lengthString = strlen(string), pointerWord = 0, pointerArray = 0;
	char *word = calloc(2, sizeof(char));
	char arrayOfDoubleControlCharacter[3] = {'&', '|', '>'};
	char arrayOfControlCharacter[4] = {'<', ';', '(', ')'};
	
	checkMemoryAllocation(word);
	
	for (pointerArray = 0; pointerArray < 4; ++pointerArray)
	{
		if ((string[*pointerString] == arrayOfDoubleControlCharacter[pointerArray]) && (pointerArray < 3))
		{
			if ((*pointerString + 1) < (lengthString))
				if (string[*pointerString + 1] == arrayOfDoubleControlCharacter[pointerArray])
				{
					word[pointerWord] = string[*pointerString];
					word[pointerWord + 1] = string[*pointerString + 1];
					*pointerString += 2;
					*controlCharacterWord = word;
					return 1;
				}
			word[pointerWord] = string[*pointerString];
			*pointerString += 1;
			*controlCharacterWord = word;
			return 1;
		}
		
		if (string[*pointerString] == arrayOfControlCharacter[pointerArray])
		{
			word[pointerWord] = string[*pointerString];
			*pointerString += 1;
			*controlCharacterWord = word;
			return 1;
		}
	}
	
	return 0;
}

char* recordWordInsideQuotes(char *string, int *pointerString, int *amountOfQuotes)
{
	int amountOfCharacter = 20, sizeWord = amountOfCharacter * sizeof(char);
	int pointerWord = 0;
	char *word = calloc(amountOfCharacter, sizeof(char));
	
	checkMemoryAllocation(word);
	
	*pointerString += 1;
	while (string[*pointerString] != '"')
	{
		word[pointerWord] = string[*pointerString];
		*pointerString += 1;
		++pointerWord;
		if (*pointerString == amountOfCharacter)
		{
			amountOfCharacter += 20;
			sizeWord = amountOfCharacter * sizeof(char);
			word = realloc(word, sizeWord);
			checkMemoryAllocation(word);
		}
	}
	
	*amountOfQuotes -= 2;
	*pointerString += 1;
	return word;
}

void memoryFree(char **array, int pointerArray)
{
	int pointerOfArray = 0;
	
	for (pointerOfArray = 0; pointerOfArray < pointerArray; ++pointerOfArray) free(array[pointerOfArray]);
	free(array);
}
	
	
char **stringProcessing(char *string, int *argc)
{
	int amountOfElements = 20, sizeArray = amountOfElements * sizeof(char*);
	int amountOfCharacter = 20, sizeWord = amountOfCharacter * sizeof(char);
	int pointerString = 0, pointerArray = 0, pointerWord = 0, amountOfQuotes = 0, helpPointer = 0;
	char **arrayOfWord = malloc(sizeArray);
	char *word = calloc(amountOfCharacter, sizeof(char)), **controlCharacterWord = calloc(1, sizeof(char*)), *helpWord;
	
	checkMemoryAllocationOnPointer(arrayOfWord);
	checkMemoryAllocationOnPointer(controlCharacterWord);
	checkMemoryAllocation(word);
	amountOfQuotes = checkForQuotes(string);
	
	while (string[pointerString] != '\n')
	{
		if (string[pointerString] == ' ')
		{
			if (pointerWord) 
			{
				recordWord(arrayOfWord, word, &pointerArray);
				word = NULL;
				pointerWord = 0;
				amountOfCharacter = 20;
				word = calloc(amountOfCharacter, sizeof(char));
				
				if (pointerArray == amountOfElements)
				{
					amountOfElements += 20;
					sizeArray = amountOfElements * sizeof(char*);
					arrayOfWord = realloc(arrayOfWord, sizeArray);
					checkMemoryAllocationOnPointer(arrayOfWord);
				}
					
			}
			++pointerString;
			continue;
		}
		
		if (checkControlCharacter(string, &pointerString, controlCharacterWord))
		{
			if (pointerWord)
			{
				recordWord(arrayOfWord, word, &pointerArray);
				word = NULL;
				
				if (pointerArray == amountOfElements)
				{
					amountOfElements += 20;
					sizeArray = amountOfElements * sizeof(char*);
					arrayOfWord = realloc(arrayOfWord, sizeArray);
					checkMemoryAllocationOnPointer(arrayOfWord);
				}
			}
			
			recordWord(arrayOfWord, controlCharacterWord[0], &pointerArray);
			controlCharacterWord[0] = NULL;
			
			if (pointerArray == amountOfElements)
			{
				amountOfElements += 20;
				sizeArray = amountOfElements * sizeof(char*);
				arrayOfWord = realloc(arrayOfWord, sizeArray);
				checkMemoryAllocationOnPointer(arrayOfWord);
			}
			
			pointerWord = 0;
			amountOfCharacter = 20;
			word = calloc(amountOfCharacter, sizeof(char));
			continue;
		}
		
		if ((string[pointerString] == '"') && (amountOfQuotes > 1))
		{
			helpWord = recordWordInsideQuotes(string, &pointerString, &amountOfQuotes);
			amountOfCharacter += strlen(helpWord);
			
			sizeWord = amountOfCharacter * sizeof(char);
			word = realloc(word, sizeWord);
			checkMemoryAllocation(word);
			for (helpPointer = 0; helpPointer < strlen(helpWord); ++helpPointer)
			{
				word[pointerWord] = helpWord[helpPointer];
				++pointerWord;
			}
			free(helpWord);
			continue;
		}
		
		word[pointerWord] = string[pointerString];
		++pointerString; 
		++pointerWord;
		if (pointerWord == amountOfCharacter)
		{
			amountOfCharacter += 20;
			sizeWord = amountOfCharacter * sizeof(char);
			word = realloc(word, sizeWord);
			checkMemoryAllocation(word);
		}
	}
	
	if (pointerWord) 
	{
		recordWord(arrayOfWord, word, &pointerArray);
		word = NULL;
		recordWord(arrayOfWord, word, &pointerArray);
		word = calloc(amountOfCharacter, sizeof(char));
	}
	
	*argc = pointerArray;
	free(word);
	free(controlCharacterWord);
	return arrayOfWord;
}

int checkingDirectoryChange(char **arg)
{
	char* home = getenv("HOME");
	
	if (!(strcmp(arg[0], "cd")))
	{
		if (arg[2] != NULL)
		{
			printf("cd: too many arguments\n");
			exit(1);
		}
		if ((arg[1] == NULL) || !(strcmp(arg[1], "~"))) 
			if (!(chdir(home))) perror(home);
			else return 0;
		else if (!(chdir(arg[1]))) perror(arg[1]);
			else return 0;
	}	
	return 1;
}

int commandExecution(char **arg)
{
	int status = 0;
	
	if (fork())
	{
		wait(&status);
		if (!(WIFEXITED(status))) perror(arg[0]);
	}
	else
	{
		execvp(arg[0], arg);
		exit(1);
	}
	return 0;
}
int checkCommand(char **arg)
{	
	int pointerArray = 0;
	while (arg[pointerArray] != NULL)
	{
		if (!(strcmp(arg[pointerArray], "|"))) return 1;
		if (!(strcmp(arg[pointerArray], ">"))) return 1;
		if (!(strcmp(arg[pointerArray], "<"))) return 1;
		if (!(strcmp(arg[pointerArray], "&"))) return 1;
		++pointerArray;
	}
	return 0;
}

int commandIdentifier(char *arg)
{
	int commandNumber = 0;
	if (!(strcmp(arg, "|"))) commandNumber = 1;
	if (!(strcmp(arg, ">"))) commandNumber = 2;
	if (!(strcmp(arg, "<"))) commandNumber = 2;
	if (!(strcmp(arg, ">>"))) commandNumber = 2;
	if (!(strcmp(arg, "&"))) commandNumber = 3;
	return commandNumber;
}

char** getCommandForPipe(char **arg, int commandNumber, int amountOfElement)
{
	int countCommand = 0, countWord = 0, pointerArray = 0;
	char **command = malloc(amountOfElement * sizeof(char*));
 	
 	while ((countCommand+1) != commandNumber)
 	{
   		if (arg[countWord] == NULL) 
   		{
   			free(command);
   			return NULL;
   		}
   		
   		if (strcmp(arg[countWord],"|") == 0) ++countCommand;
   		++countWord;
 	}
 	while ((arg[countWord]!=NULL) && (strcmp(arg[countWord],"|" )!= 0))
   	{
     	command[pointerArray] = arg[countWord];
     	++countWord;
     	++pointerArray;
   }
   command[pointerArray] = NULL;
   return command;
}

int pipeConveyor(char **argv, int argc)
{  
	int countPipe = 0, pid, fd[2];
	char **commandArguments = NULL;
  
  	while((commandArguments = getCommandForPipe(argv, ++countPipe, argc)) != NULL)
  	{
    	pipe(fd);
    	switch ((pid = fork()))
    	{
    		case -1: return 1;
    		case 0: if (getCommandForPipe(argv, countPipe + 1, argc) != NULL) dup2(fd[1],1);
      				close (fd[0]);
      				close (fd[1]);
      				commandExecution(commandArguments);
      				exit(1);
    	}
    dup2 (fd[0],0);
    close(fd[0]);
    close(fd[1]);
    free(commandArguments);
  }
  
  free(commandArguments);
  while(wait(NULL)!= -1);
  return 0;
}

void streamRedirection(char **arguments, char *command, char *filename)
{
	int fd1 = -1;
  
    if (strcmp(">", command) ==0)
    {
    	fd1 = creat( filename, 0666 );
    	if (fd1 < 0) printf("error");
        else dup2( fd1, 1);
  	} 
  	else if (strcmp("<", command) ==0)
  	{
    	fd1 = open( filename, O_RDONLY, 0666 );
    	if (fd1 < 0) printf("error");
        else dup2( fd1, 0);
    } 
    else  if (strcmp(">>", command) ==0)
    {
      fd1 = open( filename,O_RDWR | O_APPEND | O_CREAT, 0664);
      if (fd1 < 0) printf("error");
      else dup2(fd1, 1);
    }
  	close(fd1);
  	commandExecution(arguments);
}

void backgroundProcessing(char **arguments)
{
	int status = 0;
	
	if (fork())
	{
		wait(&status);
		if (!(WIFEXITED(status))) perror(arguments[0]);
	}
	else
	{
		if (fork()) kill(getpid(), SIGKILL);
		else 
		{
			signal(SIGINT, SIG_IGN);
			execvp(arguments[0], arguments);
			exit(1);
		}
		
	}
}

void commandProcessing(char **arg, int argc)
{
	int commandNumber = 0, pointerArray = 0, pointerWord = 0;
	char **arrayOfWord = malloc(argc * sizeof(char*));
	
	while (arg[pointerArray] != NULL)
	{
		commandNumber = commandIdentifier(arg[pointerArray]);
		switch (commandNumber)
		{	
			case 1:	pipeConveyor(arg, argc);
					return;
					
			case 2:	arrayOfWord[pointerWord] = NULL;
					streamRedirection(arrayOfWord, arg[pointerArray], arg[pointerArray + 1]);
					return;
					
			case 3: arrayOfWord[pointerWord] = NULL;
					backgroundProcessing(arrayOfWord);
					return;
			
			default: arrayOfWord[pointerWord] = arg[pointerArray];
					 ++pointerWord;
		}
		++pointerArray;
	}
	
}

int shell()
{
	char *string = NULL;
	char **arg;
	int argc;
		
	while (1)
	{
		string = getString();
		if (string == NULL) break;
		arg = stringProcessing(string, &argc);

		if (!(checkingDirectoryChange(arg)))
		{
			memoryFree(arg, argc);
			continue;
		}
		
		if (checkCommand(arg))
		{
			commandProcessing(arg, argc);
			memoryFree(arg, argc);
			continue;
		}		
				
		if (!(commandExecution(arg)))
		{
			memoryFree(arg, argc);
			continue;
		}
				
		memoryFree(arg, argc);
		free(string);
	}
	
	free(string);
	return 0;
}

int main()
{
	shell();
	return 0;
}
