/*********************************************
 * This file implements the
 * Code of extractin compressed
 * file to a File
 * Named in FileName
 */



#include<stdio.h>
#include<stdlib.h>

#define MAX_BUFFER 1777
#define MAX_DIGIT 256
#define MAX_NODE 514


extern struct NODE{
	long Digit;
	long Freq;
	NODE *Left,*Right;
} *Root;

extern char ReadBuffer[MAX_BUFFER];
extern char WriteBuffer[MAX_BUFFER];

NODE TreeBuffer2[MAX_NODE];
extern char FileName[FILENAME_MAX];

extern char CompressCode[FILENAME_MAX];
extern char CompressTree[FILENAME_MAX];

extern bool COMMAND_FOR_LOAD;
extern bool COMMAND_FOR_SAVE;



/*******************************************************
  *it load 1kb text frm encoded file
  */
long Load_Decode( void)
{
	static FILE *fp =fopen( CompressCode,"rb");
	if( COMMAND_FOR_LOAD){
		fp =fopen( CompressCode,"rb");
		COMMAND_FOR_LOAD =false;
	}
	long N =fread( ReadBuffer,sizeof(char),1024,fp);
	return N;
}
/*******************************************************/
/*******************************************************/


/**********************************************************
 *it saves real text in file
 */
void Save_Decode( long N)
{
	static FILE *fp =fopen( FileName,"wb");
	if( COMMAND_FOR_SAVE){
		fp =fopen( FileName,"wb");
		COMMAND_FOR_SAVE =false;
	}
	fwrite( WriteBuffer,sizeof(char),N,fp);
}
/**********************************************************/



/**********************************************************
 *it writes decode Digit in file
 */
static long i2,k2;
void Write_Digit_Decode( long Digit)
{
	if( i2==1024){// check is buffer full
		Save_Decode( 1024);
		i2=0;
	}

	long j2 =128;
	WriteBuffer[i2] =0;
	while(j2){
		/* parsein the digit to put in writebuffer */
		if( Digit & j2) WriteBuffer[i2] =(WriteBuffer[i2]<<1)+1;
		else WriteBuffer[i2] =WriteBuffer[i2]<<1;
		j2/=2;
	}
	i2++;
}
/*************************************************************/



/*************************************************************
 * load bit by bit frn file nd after findin
 * particular Digit frm tree in put Digit
 *in file usein Write_Digit_Decode
 */
static long nBit_Written,nBit_Read;

void Get_Bit_Decode( void)
{
	register long i1,j1;
	long N;
	NODE *r =Root;
	do{
		N =Load_Decode();
		for(i1=0;i1<N && nBit_Written!=nBit_Read;i1++){
			for(j1=128;j1 && nBit_Written!=nBit_Read;j1=j1/2){//printf("here %ld\N",j1);
				/* read bit frm encoded file*/
				if( ReadBuffer[i1] & j1) r =r->Right;// iteratin tree traversal
				else r =r->Left;
				nBit_Read++;

				/* check is it leaf */
				if( r->Digit!=-1){
					Write_Digit_Decode( r->Digit);
					r =Root;
				}
			}
		}
	} while( N==1024);
	Save_Decode( i2);
}
/***************************************************************/




/***************************************************************
 * took the tree by preorder traverse
 * from file 
 */

static long nNode=0;

NODE *Tree_Recovery_Decode()
{
	/* for stating read all the node from file */
	if( nNode==0){
		FILE *fp =fopen( CompressTree,"rb");
		fread( &nBit_Written,sizeof(long),1,fp);
		fread( TreeBuffer2,sizeof( NODE),MAX_NODE,fp);
		fclose( fp);
	}
	

	NODE *Temp =(NODE*)malloc( sizeof(NODE));
	Temp =&TreeBuffer2[ nNode];
	nNode++;
	/* if digit id -1 then this node 
	/* is internal 
	*/
	if( Temp->Digit==-1){
		Temp->Left =Tree_Recovery_Decode();
		Temp->Right =Tree_Recovery_Decode();
	}
	return Temp;
}
/***************************************************************


/***************************************************************
 * reset dat6a for net comnpression 
 */
void Reset_Decode( void)
{
	nNode =0;
	nBit_Written =nBit_Read =0;
	i2 =k2 =0;
}
/***************************************************************
 */