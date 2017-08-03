
/* This file implements the
 * Code of extractin compressed
 * file to a File
 * Named in FileName
 */


#include<stdio.h>
#include<stdlib.h>

#define MAX_FILE_NAME 1077
#define MAX_BUFFER_LEN 1024

#define mSHOW_READ_BIT			/* to show the readin bit from file */
#define mSHOW_WRITE_BIT			/* to show the writen code in comprss file */


extern long MaxDicEntry;

extern char FileName[ MAX_FILE_NAME+1];

extern char Compress1[ MAX_FILE_NAME+1];
extern char Compress2[ MAX_FILE_NAME+1];

extern char ReadBuffer[ MAX_BUFFER_LEN+1];
extern char WriteBuffer[ MAX_BUFFER_LEN+1];


extern bool COMMAND_FOR_LOAD;			/* to change the value of fp in Read_Buffer_Decode() */
extern bool COMMAND_FOR_SAVE;			/* to change the value of fp in Write_Buffer_Decode() */

static bool COMMAND_FOR_CHANGE1;		/* to chenge the value of static var in Get_Bit_Decode() */
static bool COMMAND_FOR_CHANGE2;        /* to change the value of static ver of Write_Pattern_Decode() */



/* ========================================================================
 * it saves the all the pattern
 * by creatin array dic
 */
struct CODE{
	char *Pattern;				/* pattern which will be writen in file */
	char FirstBit;				/* First Bit of the Pattern */
	long Len;					/* Len ot the Pattern */
} *Dic;
/* ========================================================================
 */

extern void Process_Decode( void);

long Read_Buffer_Decode( void);
void Write_Buffer_Decode( long N);

char Get_Bit_Decode( void);
void Add_Code_Decode( char *Pattern,char FirstBit,long Len);
void Write_Pattern_Decode( char* Pattern,long Len);

void Reset_Dic_Decode( void);


static long tBit;			/* it take the value of how many bit has to be take */
static long MaxLen;			/* it denotes the len of the next code */
static long MaxCode;		/* it denotes the len of array Dictionary */
static long iValue;			/* it keeps the value of i in wrriten pattern file*/



/* ========================================================================
 * it takes bit by bit
 * if find any bit pattern of MaxLen
 * that means the Code
 * then write the Pattern for the Code
 * and Add a new code the New pattern
 * reverin process of Encode
 */
void Process_Decode( void)
{
	COMMAND_FOR_CHANGE1 =COMMAND_FOR_CHANGE2 =true;

	FILE *fp =fopen( Compress2,"rb");
	fread( &tBit, sizeof( long),1,fp);

	/* check is file empty */
	if( !tBit){
		Write_Buffer_Decode( 0);
		return;
	}


	/* inittialization  */
	Dic =(CODE*)malloc( sizeof( CODE)*2);
	Dic[0].Pattern =(char*)malloc( sizeof(char)*1);
	Dic[0].Pattern[0] =0;
	Dic[0].FirstBit =0;
	Dic[0].Len =1;

	Dic[1].Pattern =(char*)malloc( sizeof(char)*1);
	Dic[1].Pattern[0] =1;
	Dic[1].FirstBit =1;
	Dic[1].Len =1;

	CODE PreCode;    /* it save the previous pattern or code to use in get new code */
	PreCode.Pattern =(char*)malloc( sizeof(char)*1);
	PreCode.Pattern[0] =Get_Bit_Decode();
	tBit--;
	PreCode.Len =1;
	PreCode.FirstBit =PreCode.Pattern[0];
	Write_Pattern_Decode( PreCode.Pattern ,1);

#ifdef SHOW_READ_BIT
	printf("%ld",PreCode.Pattern[0]);
#endif


	MaxLen =2;       /* maxlen will be 1 for pattern 0 or 1*/
	MaxCode =2;		 /* Maxcode is 2 for two pattern */

	
	long b;			 /* it take all the bit */
	long Code =0;	 /* it buffer the bit to Find Next Code */ 
	long Len =0;     /* len is used to find the Len of code*/


	/* continue until end file or all the bit*/
	while( (b=Get_Bit_Decode())!=EOF && tBit){
		/* buuferin the code from file */
		Code =(Code<<1) +b;
		Len++;		 

		/* if len is equal to Maxlen then it denote 
		 * a code is taken from compreaa File
		 */
		if( Len==MaxLen){//printf("here\n");
			/* check is the code is in the Dictinary */
			if( Code<MaxCode){
				/* write the pattern */
				Write_Pattern_Decode( Dic[ Code].Pattern ,Dic[ Code].Len);
				/* check is dic is full*/
				if( MaxCode < MaxDicEntry ){
					char *NewPattern =(char*)malloc( sizeof(char)*(PreCode.Len+1));
					/* new Code retrive by adding 
					 *  first bit of the wrriten pattern
					 * with the previou pattren
					 * reverin the process of encode
					 */
					for( long i=0;i<PreCode.Len;i++) NewPattern[i] =PreCode.Pattern[i];
					NewPattern[PreCode.Len] =Dic[Code].FirstBit;
					/* add New Pattern */
					Add_Code_Decode( NewPattern ,PreCode.FirstBit ,PreCode.Len+1);
				}
			}
			/* special case*/
			else{
				/* at this case first of all the nonexisting
				 * code has to be retrived
				 * by addin first bit of the pre pattern in the 
				 * last of pre pattern
				 */
				char *NewPattern =(char*)malloc( sizeof(char)*(PreCode.Len+1));
				for( long i=0;i<PreCode.Len;i++) NewPattern[i] =PreCode.Pattern[i];
				NewPattern[PreCode.Len] =PreCode.FirstBit;
				Add_Code_Decode( NewPattern ,PreCode.FirstBit ,PreCode.Len+1);
				Write_Pattern_Decode( Dic[ Code].Pattern ,Dic[ Code].Len);
			}
			/* save the written pattern or code */
			PreCode =Dic[Code];
			Len =0;
			Code =0;
		}

#ifdef SHOW_READ_BIT
	printf("%ld",b);
#endif
		tBit--;
	}
	/* flushin Buffer*/
	if( Len) Write_Pattern_Decode( Dic[Code].Pattern ,Dic[Code].Len);
	Write_Buffer_Decode( iValue);
	iValue =0;
	/* make dic Empty */
	Reset_Dic_Decode();
}
/* ========================================================================
 */



/* ========================================================================
 *read from compress file using static fp
 */
long Read_Buffer_Decode( void)
{
	static FILE *fp =fopen( Compress1 ,"rb");
	if( COMMAND_FOR_LOAD){
		fp =fopen( Compress1 ,"rb");
		COMMAND_FOR_LOAD =false;
	}
	return fread( ReadBuffer, sizeof( char), MAX_BUFFER_LEN,fp);
}
/* ========================================================================
 */


/* ========================================================================
 *write into main file file using static fp
 */
void Write_Buffer_Decode( long N)
{
	static FILE *fp =fopen( FileName,"wb");
	if( COMMAND_FOR_SAVE){
		fp =fopen( FileName,"wb");
		COMMAND_FOR_SAVE =false;
	}
	fwrite( WriteBuffer ,sizeof( char), N,fp);
}
/* ========================================================================
 */





/* ========================================================================
 *function used to get bit from file
 * retuen EOF if N is 0 that means
 * no more bit is existed in file
 */
char Get_Bit_Decode( void)
{
	static long i =0;			/* used for counting to check is buffer full*/
	static unsigned V =1;		/* V used to compare value for parsing bit*/
	static long N =0;			/* used to find hw many char is found in file */

	/* init val of static var for new compression */
	if( COMMAND_FOR_CHANGE1){
		i =0;
		V =1;
		N =0;
		COMMAND_FOR_CHANGE1 =false;
	}

	V >>=1;	
	/* j will be 0 if whole char is parsin bit wise*/
	if( !(char)V){
		/* check is ReadBuffer is all read */
		i++;
		if( i >=N){
			N =Read_Buffer_Decode();
			if( !N) return EOF;
			i =0;
		}
		/* again make v 10000000*/
		V =128;
	}

	if( ReadBuffer[i] & V) return 1;
	else return 0;
}
/* ========================================================================
 */




/* ========================================================================
 * Add new item in the dictionary
 * and check the len of the new code
 */
void Add_Code_Decode( char *Pattern,char FirstBit,long Len)
{
	Dic =(CODE*)realloc( Dic,sizeof(CODE)*(MaxCode+1));
	/* all the new pattern is assigned to New Item in Dic*/
	Dic[ MaxCode].Pattern =Pattern;
	Dic[ MaxCode].FirstBit =FirstBit;
	Dic[ MaxCode].Len =Len;

	MaxCode++;
	if( MaxCode==MaxDicEntry) return;
	long V =MaxCode;
	Len =0;
	/* findin the Len of New Code Not Pattern */
	while( V){
		Len++;
		V /=2;
	}
	if( Len>MaxLen) MaxLen =Len; /* relaxation of maxlen to*/
}
/* ========================================================================
 */



/* ========================================================================
 *just macro func use to procced next char and
 *init i,j,CharValue
 */
#define PROCEED_BUFFER( i,j,CharValue) {  \
			WriteBuffer[i] =CharValue; \
			i++; \
			if( i>=MAX_BUFFER_LEN){ \
				Write_Buffer_Decode( MAX_BUFFER_LEN); \
				i =0; \
			} \
			j =0; \
			CharValue =0; \
		}




/* ========================================================================
 * write the pattern for of len length
 */
void Write_Pattern_Decode( char *Pattern,long Len)
{
	static long i =0;				/* used to count the array index of WriteBuffer */	
	static long j =0;				/* used to count is 8 bit written */
	static char CharValue =0;		/* it took the value of 8bit during parsein */
	const long Limit =8;

	/* init val of static var for new compression */
	if( COMMAND_FOR_CHANGE2){
		i =j =0;
		CharValue =0;
		COMMAND_FOR_CHANGE2 =false;
	}

	/* v used to comapre pattern to find bit */
	for( long k =0;k<Len;k++){
		if( Pattern[k]){
			CharValue =(CharValue<<1) +1;

#ifdef SHOW_WRITE_BIT
	printf("1");
#endif

		}
		else{
			CharValue =CharValue<<1;

#ifdef SHOW_WRITE_BIT
	printf("0");
#endif
		}
		/** to take next bit of char shift V to one Bit*/
		j++;
		if( j==Limit) PROCEED_BUFFER( i,j,CharValue);
	}

	iValue =i;
}
/* ========================================================================
 */




/* ========================================================================
 * just free mamory
 */
void Reset_Dic_Decode( void)
{
	long i;
	for( i=0;i<MaxCode;i++){
		free( Dic[i].Pattern);
	}
	free( Dic);
}
/* ========================================================================
 */