/********************************************************
 * this program is created by Prasanjit Barua
 * Std No 0805090
 * under the supervisation of
 * Tanaeem M. Moosa
 * Implemented Huffmen coding
 * usage of program is described in readme.txt
 */


#include<stdio.h>
#include<string.h>
#include<ctype.h>


#define MAX_DIGIT 256           /* how many Digit can be made 2 pow8 */
#define MAX_NODE 514			/* node to be formed for Huffmen tree */


struct NODE{
	long Digit;					/* hold the value of block */
	long Freq;					/* freq of the block */
	NODE *Left,*right;			/* left right child of tree */
} *Root,Temp;

extern NODE treeBuffer[ MAX_NODE];

struct Huff_Code{				/* holds the code for index block value */
	char *Code;					/* code given the particular block */
	long Len;					/* len or depth of the code during traver */
} Code_Digit[MAX_DIGIT];

bool COMMAND_FOR_LOAD;
bool COMMAND_FOR_SAVE;

char FileName[FILENAME_MAX];			/* file to be compressed */	
char Directory[FILENAME_MAX];			/* compress file of the folder */

char CompressCode[FILENAME_MAX] ="Huff1";		/* compress file holding code */
char CompressTree[FILENAME_MAX] ="Huff2";		/* compress file holding tree */


extern void Set_Folder_Encode( char *Input);
extern void Set_Folder_Decode( char *Input);


extern void Freq_Calc_Encode( void);
extern void Tree_Construct_Encode( void);
extern void Tree_Traversal_Encode( NODE *r,char *Code,long depth);
extern void Code_File_Encode( void);
extern void Code_Tree_Encode( NODE *r);
extern void Reset_Encode( void);

extern NODE *Tree_Recovery_Decode( void);
extern void Get_Bit_Decode( void);
extern void Write_Digit_Decode( long Digit);
extern void Reset_Decode( void);



/* func whch Encode text file */
void Encode( void)
{
	Freq_Calc_Encode();
	Tree_Construct_Encode();
	/* as Root NODE has no Code so Code value 0 and Len 0 */
	Tree_Traversal_Encode( Root," ",0);
	Code_File_Encode();
	Code_Tree_Encode( Root);
	Reset_Encode();
}


/* func which Decode file */
void Decode( void)
{
	Root =Tree_Recovery_Decode();
	Get_Bit_Decode();
	Reset_Decode();

}

int main( long argc,char *argv[])
{
	if( argc!=3){
		printf("Error<Mode><Filename>\n");
		return 0;
	}

	if( toupper(*argv[1])=='C') Set_Folder_Encode( argv[2]);
	else if( toupper(*argv[1])=='E') Set_Folder_Decode( argv[2]);
	else printf("Error<Mode><Filename>\n");

	return 0;
}

