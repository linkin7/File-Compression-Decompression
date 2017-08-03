

/***********************************************
 * This file implements encodin  of
 * A file to compress format
 * FileName holds the filename to
 * To be compressed
 */





#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define BLOCK 8// as read file by takin 4 Bit as block
#define MAX_BUFFER 1777
#define MAX_DIGIT 256
#define MAX_NODE 514// for complete bin tree it will be 2pow9-1

extern struct NODE{
	long Digit;					/* hold the value of block */
	long Freq;					/* freq of the block */
	NODE *Left,*Right;			/* child of the node  */
} *Root;


/* store Code of a particular Digit */
extern struct Huff_Code{
	char *Code;					/* code for the block gettin by tree */
	long Len;					/* lenght of the code */
} Code_Digit[MAX_DIGIT];

extern char FileName[FILENAME_MAX];

extern char CompressCode[FILENAME_MAX];
extern char CompressTree[FILENAME_MAX];

extern bool COMMAND_FOR_LOAD;	 /* used to take value of fp during new compression */
extern bool COMMAND_FOR_SAVE;




/*************************************
 *heap
 */
extern void Heap_Push( NODE p);
extern NODE *Heap_Pop();
extern void Clear_Heap( void);
/**************************************/


long Freq[MAX_DIGIT];			  /* it strorea Freq of Digit 0-255 as thre are pow(2,8) Digit */

char ReadBuffer[MAX_BUFFER];	  /* store char from file during read */
char WriteBuffer[MAX_BUFFER];	  /* store char from file during write */

NODE TreeBuffer[MAX_NODE];		  /* to write tree node used as temp buffer */





/******************************************************
 * it used fr both load  char in name on mode 
 */
inline long Load_Encode( void)
{
	static FILE *fp =fopen(FileName,"rb");
	if( COMMAND_FOR_LOAD){
		fp =fopen( FileName,"rb");
		COMMAND_FOR_LOAD =false;
	}

	long N =fread( ReadBuffer,sizeof(char),1024,fp);
	
	if( N<1024) rewind(fp);// for later use in encode
	return N;
}
/******************************************************/




/******************************************************
 * read nd Freq calculat by 4 Bit wise
 * iused to count how many char use frm buffer
 * j is used to bitwise operate with char
 * k is used to count block length to create block Code
 */
void Freq_Calc_Encode( void)
{
	register long i,j;
	long N;
	const Limit =128;

	do{
		N =Load_Encode();
		/* extractin char one after anthr*/
		for( i=0;i<N;i++){
			long Digit =0;
			j =Limit;
			while(j){
				/* parsin char to get the block */
				if( ReadBuffer[i] & j) Digit =(Digit<<1)+1;
				else Digit =Digit<<1;
				j/=2;
			}
			Freq[ Digit]++;
		}
	} while( N==1024);
}
/****************************************************************
 */



/*****************************************************************
 *assign each NODE a Digit and its Freq
 */
long Node_Create_Encode( void)
{
	register long i,nNode=0;		/* total non zero node */
	for(i=0;i<MAX_DIGIT;i++){
		if( Freq[i]){

			/* aNode Create For which freq is not null */
			NODE *Temp =(NODE*)malloc( sizeof(NODE));
			Temp->Digit =i;
			Temp->Freq =Freq[i];
			Temp->Left =Temp->Right =NULL;

			Heap_Push( *Temp);
			free( Temp);
			nNode++;
		}
	}
	return nNode;
}
/******************************************************************
 */



/*******************************************************************
 *it constructs tree by merging optimal binary tree
 */
void Tree_Construct_Encode( void)
{
	long i,nNode;
	
	NODE *Temp;	

	nNode =Node_Create_Encode();		/* get the tot non zero node */
	for(i=1;i<=nNode-1;i++){;

		Temp =(NODE*)malloc( sizeof(NODE));
		Temp->Left =Heap_Pop();
		Temp->Right =Heap_Pop();
		Temp->Freq =Temp->Left->Freq + Temp->Right->Freq;
		Temp->Digit =-1;

		Heap_Push( *Temp);
		free( Temp);
	}
	Root =Heap_Pop();
}
/***********************************************************
 */




/*******************************************************************
 *preorder traversal
 *used to create Code by tree traversal
 */
void Tree_Traversal_Encode( NODE *r,char *Code,long Depth)
{
	if( r==NULL) return;
	/* if node is leaf then this represent a block and its code */
	else if( r->Left==NULL && r->Right==NULL){
		long Digit =r->Digit;
		Code_Digit[ Digit].Code =(char*)malloc( sizeof(char)*( 1+Depth+1));
		strcpy( Code_Digit[ Digit].Code,Code);
		Code_Digit[ Digit].Len =Depth;
	}
	else{
		char *p =(char*)malloc( sizeof(char)*( 1+Depth+2));
		strcpy( p,Code);
		p[ Depth+2] ='\0';

		/* for left child 0 is added */
		p[ Depth+1] ='0';
		Tree_Traversal_Encode( r->Left,p,Depth+1);
		/* for right child 1 is added */
		p[ Depth+1] ='1';
		Tree_Traversal_Encode( r->Right,p,Depth+1);

		free(p);
	}
}
/******************************************************************/




/************************************************************
 *write new file by codin
 *first part is like Freq calclation parsein read char
 */
inline void Save_Encode( long N)
{
	static FILE *fp =fopen( CompressCode,"wb");
	if( COMMAND_FOR_SAVE){
		fp =fopen( CompressCode,"wb");
		COMMAND_FOR_SAVE =false;
	}
	fwrite( WriteBuffer,sizeof(char),N,fp);
}
/*************************************************************/


/*************************************************************
 *Counter count how many Bit is taken foe a char
 *i2 count how many char is taken for buffer
 */
long Counter=0,i2=0;
inline void Insert_Bit_Encode( long Bit)
{
	if( Counter==8) Counter=0,i2++;// check is 8bit mean a char is taken
	if( i2==1024) Save_Encode( i2),i2=0;// check is buffer is full

	if( Bit==1) WriteBuffer[i2] =(WriteBuffer[i2]<<1)+1;
	else WriteBuffer[i2] =WriteBuffer[i2]<<1;
	Counter++;
}

long nBit_Written;

void Code_File_Encode( void)
{
	register long i1,j1,N1;// use for input
	register long k2;
	const long Limit =128;

	do{
		N1 =Load_Encode();
		for( i1 =0; i1<N1; i1++){

			long Digit =0;
			j1 =Limit;
			while( j1){
				if( ReadBuffer[i1] & j1) Digit =(Digit<<1)+1;
				else Digit =Digit<<1;
				j1/=2;
			}
			char *Code =Code_Digit[ Digit].Code;

			/* parsein Code to write in com file  */
			for( k2=1; k2 <=Code_Digit[ Digit].Len; k2++){
				Insert_Bit_Encode( Code[k2]-'0');
				nBit_Written++;
			}
			/*********************************************/
		}
	} while( N1==1024);
	/* flushin buffer */
	if( Counter!=8) WriteBuffer[i2] =WriteBuffer[i2]<<(8-Counter);// to avoid extra Bit which has not allocated
	i2++;
	Save_Encode(i2);
}
/***************************************************************/




/***************************************************************
 * save tree by preorder traversal it
 */
long nNode=0;
void Code_Tree_Encode( NODE *r)
{
	if( r){
		TreeBuffer[ nNode++] =*r;
		Code_Tree_Encode( r->Left);
		free( r->Left);
		Code_Tree_Encode( r->Right);
		free( r->Right);
	}
	//write all NODE after traversing all NODE
	if( r==Root){
		free( Root);
		Root =NULL;

		FILE *fp =fopen( CompressTree,"wb");
		fwrite( &nBit_Written,sizeof( long),1,fp);
		nBit_Written =0;
		fwrite( TreeBuffer,sizeof( NODE),nNode,fp);
		nNode =0;
		fclose( fp);
	}
}
/**************************************************************/
/**************************************************************/


/***************************************************************
 * reset data for next comnprsion 
 */
void Reset_Encode( void)
{
	Counter =i2 =0;
	long i;
	for( i=0;i<MAX_DIGIT;i++){
		Freq[i] =0;
	}
	for( i=0;i<MAX_DIGIT;i++){
		if( Code_Digit[i].Len){
			free( Code_Digit[i].Code);
			Code_Digit[i].Code =NULL;
			Code_Digit[i].Len =0;
		}
	}
	Clear_Heap();
}
/***************************************************************
 */