/*-------------------------------------------------------------------------*
 *---									---*
 *---		wordProc_hash.c						---*
 *---									---*
 *---	  A rudimentary word processor that prints the hash values of	---*
 *---	saved files.							---*
 *---									---*
 *---	----	----	----	----	----	----	----	----	---*
 *---									---*
 *---	Version 1a		2020 January 1		Joseph Phillips	---*
 *---									---*
 *-------------------------------------------------------------------------*/

//
//	Compile with:
//	$ g++ wordProc_hash.c -o wordProc_hash -lncurses -g
//
//	NOTE:
//	If this program crashes then you may not see what you type.
//	If that happens, just type:
//
//		stty sane
//
//	to your terminal, even though you cannot even see it.

//---									---//
//---			Header file inclusions:				---//
//---									---//
#include	<stdlib.h>
#include	<stdio.h>
#include	<string.h>
#include	<unistd.h>	// For alarm()
#include	<sys/socket.h>
#include	<sys/types.h>	// For creat()
#include	<sys/wait.h>	// For creat(), wait()
#include	<sys/stat.h>	// For creat()
#include	<fcntl.h>	// For creat()
#include	<signal.h>
#include	<ncurses.h>


//---									---//
//---			Definitions of constants:			---//
//---									---//
#define		HASH_PROGRAM		"/usr/bin/openssl"
#define		HASH_ARG		"-hex"
#define		STOP_CHARS		"*+-#&"
const char*	HASH_TYPE_ARRAY[]	= {"sha224","sha256"};
const int	NUM_HASH_TYPES		= sizeof(HASH_TYPE_ARRAY)/sizeof(char*);

const int	HEIGHT			= 5;
const int	WIDTH			= 72;
const int	INIT_TEXT_LEN		= 1024;
const int	MAX_TEXT_LEN		= 65536;
const char	STOP_CHAR		= (char)27;
const char	PRINT_CHAR		= (char)0x10;
const int	BUFFER_LEN		= 64 * 16;
const int	TYPING_WINDOW_BAR_Y	= 0;
const int	CHECKING_WINDOW_BAR_Y	= TYPING_WINDOW_BAR_Y   + HEIGHT + 1;
const int	MESSAGE_WINDOW_BAR_Y	= CHECKING_WINDOW_BAR_Y + 1;
const int	NUM_MESSAGE_SECS	= 6;


//---									---//
//---			Definitions of global vars:			---//
//---									---//
WINDOW*		typingWindow;
WINDOW*		checkingWindow;
WINDOW*		messageWindow;
pid_t		childPid;
int		shouldRun	= 1;


//---									---//
//---			Definitions of global fncs:			---//
//---									---//

//  PURPOSE:  To turn 'ncurses' on.  No parameters.  No return value.
void		onNCurses	()
{
  //  I.  Application validity check:

  //  II.  Turn 'ncurses' on:
  initscr();
  cbreak();
  noecho();
  nonl();
//intrflush(stdscr, FALSE);
//keypad(stdscr, TRUE);
  typingWindow		= newwin(HEIGHT,WIDTH,TYPING_WINDOW_BAR_Y+1,1);
  checkingWindow	= newwin(HEIGHT,WIDTH,CHECKING_WINDOW_BAR_Y+1,1);
  messageWindow		= newwin(     2,WIDTH,MESSAGE_WINDOW_BAR_Y,1);
  scrollok(typingWindow,TRUE);
  scrollok(checkingWindow,TRUE);

  mvaddstr(TYPING_WINDOW_BAR_Y,0,"'Esc' to quit. Ctrl-P to compute hashes.");
  mvaddstr(CHECKING_WINDOW_BAR_Y,0,"Hashes:");
  refresh();
  wrefresh(typingWindow);	// moves cursor back to 'typingWindow':

  //  III.  Finished:
}


//  PURPOSE:  To handle 'SIGALRM' signals.  Ignores 'sig' (which will be
//	'SIGALRM').  No return value.
void		sigAlarmHandler	(int		sig
				)
{
  mvwaddstr (messageWindow,0,0,
  	     "                                        "
  	     "                                       "
	    );
  mvwaddstr (messageWindow,1,0,
  	     "                                        "
  	     "                                       "
	    );
  wrefresh(messageWindow);
  wrefresh(typingWindow);	// moves cursor back to 'typingWindow':
}


//  PURPOSE:  To save the 'lineIndex' chars at the beginning of 'line' to
//	to position '*endTextPtrPtr' in buffer '*bufferPtrPtr' of length
//	'*bufferLenPtr' and with end '*endBufferPtrPtr'.  If there is not
//	enough space in '*bufferPtrPtr' then '*bufferLenPtr' will be doubled,
//	and '*bufferPtrPtr' will be 'realloc()'-ed to this new length.
//	No return value.
void		saveLine	(size_t*	bufferLenPtr,
				 char**		bufferPtrPtr,
				 char**		endTextPtrPtr,
				 char**		endBufferPtrPtr,
				 const char*	line,
				 int   		lineIndex
				)
{
  //  I.  Application validity check:

  //  II.  Save 'line' to '*bufferPtrPtr':
  //  II.A.  Allocate more space if needed:
  if  (lineIndex >= (*endBufferPtrPtr - *endTextPtrPtr + 1) )
  {
    size_t	textLen	 = *endTextPtrPtr - *bufferPtrPtr;

    (*bufferLenPtr)	*= 2;
    (*bufferPtrPtr)	 = (char*)realloc(*bufferPtrPtr,*bufferLenPtr);
    (*endTextPtrPtr)	 = *bufferPtrPtr + textLen;
    (*endBufferPtrPtr)	 = *bufferPtrPtr + *bufferLenPtr;
  }

  //  II.B.  Save 'line' to '*bufferPtrPtr':
  memcpy(*endTextPtrPtr,line,lineIndex);
  (*endTextPtrPtr)	+= lineIndex;

  //  III.  Finished:
}



//  PURPOSE:  To attempt to compute the hashes of the text pointed to by
//	'bufferPtr' using the hash functions in 'HASH_TYPE_ARRAY[]'.
//	'endTextPtr' points to one char beyond the end of the text to print
//	in 'bufferPtr'.  No return value.
//
//	SIDE EFFECT: Prints to 'messageWindow' and sets process to receive
//		     'SIGALRM' 'NUM_MESSAGE_SECS' seconds in the future.
//		     This will invoke 'sigAlarmHandler()', which erases the
//		     text in 'messageWindow'.
void		computeHashes	(const char*	bufferPtr,
				 const char*	endTextPtr
				)
{
  //  I.  Application validity check:

  //  II.  Compute hashes:
  //  II.A.  Each iteration computes one hash:
  int hashIndex;

  for  (hashIndex = 0;  hashIndex < NUM_HASH_TYPES;  hashIndex++)
  {
    //  II.A.1.  Create pipes:
    int	toChild[2];
    int	fromChild[2];

    //  YOUR CODE HERE to make 2 pipes
    if(pipe(toChild) < 0){
	fprintf(stderr, "no pipe for you! \n");
	exit(EXIT_FAILURE);
    }
    if (pipe(fromChild) < 0){
	fprintf(stderr, "you get no pipe! \n");
	exit(EXIT_FAILURE);
    }
    //  II.A.2.  Do 'openssl' process work:
    pid_t isChild = fork();
    if (isChild < 0){
	fprintf(stderr, "sorry no child for you\n");
	exit(EXIT_FAILURE);
    } 
    if  (isChild == 0)
   {
      //  YOUR CODE HERE to handle child case
	close(toChild[1]);
	close(fromChild[0]);
	dup2(toChild[0], STDIN_FILENO);
	dup2(fromChild[1], STDOUT_FILENO);
	execl(HASH_PROGRAM,HASH_PROGRAM, HASH_TYPE_ARRAY[hashIndex], HASH_ARG, NULL);
	exit(EXIT_FAILURE);
    }

    //  II.A.3.  Do parent work:
    int		numBytes;
    int		status;
    char	text[INIT_TEXT_LEN];
    const char*	msgCPtr;

    //  YOUR CODE HERE to handle parent case
	close(toChild[0]);
	close(fromChild[1]);
	//if(write(toChild[0],
	//perror("trying to write") 
	write(toChild[1], bufferPtr ,endTextPtr-bufferPtr);
	close(toChild[1]);
	//if(read(fromChild[0])
	if(numBytes = read(fromChild[0],text, INIT_TEXT_LEN)<0)
		msgCPtr = "compute hash failed";
	msgCPtr = strstr(text, "= ");
	
	close(fromChild[0]);
	wait(&status);
    mvwaddstr(messageWindow,hashIndex,0,HASH_TYPE_ARRAY[hashIndex]);
    waddstr(messageWindow,msgCPtr);
    wrefresh(messageWindow);
  }

  wrefresh(typingWindow);	// moves cursor back to 'typingWindow':
  //  YOUR CODE HERE to do one last thing
	alarm(NUM_MESSAGE_SECS);
  //  III.  Finished:
}


//  PURPOSE:  To allow the user to type, display what they type in
//	&apos'typingWindow', and to compute the hash upon pressing 'PRINT_CHAR'.
//	'vPtr' comes in, perhaps pointing to something.  Returns 'NULL'.
void*		type		(void*	vPtr
				)
{
  //  I.  Application validity check:

  //  II.  Handle user typing:
  unsigned int	c;
  char		line[WIDTH+1];
  int		index		= 0;
  size_t	bufferLen	= INIT_TEXT_LEN;
  char*		bufferPtr	= (char*)malloc(bufferLen);
  char*		endTextPtr	= bufferPtr;
  char*		endBufferPtr	= bufferPtr + bufferLen;

  //  II.A.  Each iteration handles another typed char:
  while  ( (c = getch()) != STOP_CHAR )
  {

    //  II.A.1.  Handle special chars:
    if  (c == '\r')
    {
      //  II.A.1.a.  Treat carriage return like newline:
      c = '\n';
    }
    else
    if  ( (c == 0x7) || (c == 127) )
    {
      //  II.A.1.b.  Handle backspace:
      int	col	= getcurx(typingWindow);

      if  (col > 0)
      {
        index--;
        wmove(typingWindow,getcury(typingWindow),col-1);
	wrefresh(typingWindow);
      }

      continue;
    }
    else
    if  (c == PRINT_CHAR)
    {
      size_t	textLen	= endTextPtr - bufferPtr;

      saveLine(&bufferLen,&bufferPtr,&endTextPtr,&endBufferPtr,line,index);
      computeHashes(bufferPtr,endTextPtr);
      endTextPtr	= bufferPtr + textLen;
      continue;
    }
    else
    if  (c == ERR)
    {
      continue;
    }

    //  II.A.2.  Print and record the char:
    waddch(typingWindow,c);
    wrefresh(typingWindow);
    line[index++]	= c;

    //  II.A.3.  Handle when save 'line':
    if  (c == '\n')
    {
      //  II.A.3.a.  Save 'line' when user types newline:
      saveLine(&bufferLen,&bufferPtr,&endTextPtr,&endBufferPtr,line,index);
      index = 0;
    }
    else
    if  (index == WIDTH-1)
    {
      //  II.A.3.b.  Save 'line' when at last column:
      line[index] = '\n';
      index++;
      saveLine(&bufferLen,&bufferPtr,&endTextPtr,&endBufferPtr,line,index);
      index = 0;
      waddch(typingWindow,'\n');
      wrefresh(typingWindow);
    }
  }

  //  III.  Finished:
  saveLine(&bufferLen,&bufferPtr,&endTextPtr,&endBufferPtr,line,index);
//int inFd = creat(TEXT_FILENAME,0640);
//write(inFd,bufferPtr,endTextPtr-bufferPtr);
//close(inFd);
  free(bufferPtr);
  return(NULL);
}


//  PURPOSE:  To turn off 'ncurses'.  No parameters.  No return value.
void		offNCurses	()
{
  sleep(1);
  nl();
  echo();
  refresh();
  delwin(messageWindow);
  delwin(typingWindow);
  delwin(checkingWindow);
  endwin();
}

//  PURPOSE:  To do the spell-checking word-processor.  Ignores command line
  //	arguments.  Return 'EXIT_SUCCESS' to OS.
  int		main		()
  {
    struct sigaction	act;

      // (2) YOUR CODE HERE to install sigAlarmHandler as the handler for SIGALRM
	memset(&act, '\0', sizeof(act));
	act.sa_handler = sigAlarmHandler;
	sigaction(SIGALRM, &act, NULL);
        onNCurses();

        type(NULL);
        offNCurses();
        return(EXIT_SUCCESS);
 }
