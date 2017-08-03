/*=================================================
 * this program is created by Prasanjit Barua
 * Std No 0805090
 * under the supervisation of
 * Tanaeem M. Moosa
 * It implements Lempel-ziv 78
 * version lempel-ziv-welch
 * usage of program is described in readme.txt
 */



#include<stdio.h>
#include<ctype.h>
#include<time.h>

#define MAX_FILE_NAME 1077
#define MAX_BUFFER_LEN 1024

char FileName[ MAX_FILE_NAME +1];			/* name of the file which to be comprss*/
char Directory[ MAX_FILE_NAME +1];          /* name of the decompress file */

char Compress1[ MAX_FILE_NAME+1];           /* name of the compress file which save coded byte */
char Compress2[ MAX_FILE_NAME+1];           /* name of the file which took the tbit value */  

char ReadBuffer [MAX_BUFFER_LEN +1];        /* take the byte of read file */
char WriteBuffer [MAX_BUFFER_LEN +1];		/* take the byte of writen file */

long MaxDicEntry =777777;                   /* the len of the maximum pattern */

extern void Process_Encode( void);
extern void Process_Decode( void);

extern void Set_Folder_Encode( char *Input);
extern void Set_Folder_Decode( char *Input);


int main( int argc,char *argv[])
{
	time_t Start =time( NULL);

	if( argc!=3){
		printf("Error<Mode><Filename>\n");
		return 0;
	}

	if( toupper(*argv[1])=='C') Set_Folder_Encode( argv[2]);
	else if( toupper(*argv[1])=='E') Set_Folder_Decode( argv[2]);
	else{
		printf("Error<Mode><Filename>\n");
		return 0;
	}

	time_t End =time( NULL);
	printf("\nElapsed time: %.3lf s\n",difftime( End, Start));

	return 0;
}