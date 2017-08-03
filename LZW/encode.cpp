/*==================================================
 * This file implements encodin  of
 * A file to compress format
 * FileName holds the filename to
 * To be compressed
 */



#include<stdio.h>
#include<stdlib.h>


#define MAX_BUFFER_LEN 1024
#define MAX_FILENAME 1077

#define mSHOW_READ_BIT		/* used to show all the bit read from file */
#define mSHOW_WRITE_BIT		/* used to shoew all the encdin bit */
#define mSHOW_TREE			/* to show the tree */

extern long MaxDicEntry;					  /* maximum entry of the dictionary which is MaxDicEntry+1*/

extern char FileName[ MAX_FILENAME +1];       /* name of the file to be compressed */

extern char ReadBuffer [MAX_BUFFER_LEN +1];   /* array to take the byte of read file */
extern char WriteBuffer [MAX_BUFFER_LEN +1];  /* array to take the byte of wrriten file */

extern char Compress1[ MAX_FILENAME+1];      /* name of the compress file contain all code byte */
extern char Compress2[ MAX_FILENAME+1];	     /* name of the compress file where value of tBit will b saved */

extern bool COMMAND_FOR_LOAD;   /* to change the value of fp in Read_Buffer_Encode() */
extern bool COMMAND_FOR_SAVE;	/* to change the value of fp in Write_Buffer_Encode() */

bool COMMAND_FOR_CHANGE1;		/* to chenge the value of static var in Get_Bit_Encode() */
bool COMMAND_FOR_CHANGE2;       /* to change the value of static ver of Write_Code_Encode() */

/*used to check which child is used*/
enum SIDE{
	Left,Right
};


/*used to create tree */
struct NODE{
	long Code;       /* The code given to the node during encodein*/
	long Len;		 /* len of the code */
	long Dep;        /* depth of the node in tree */
	NODE *lChild,*rChild;
} *Root;



/*all the encode function prototype**/
void Process_Encode( void);

long Read_Buffer_Encode( void);
void Write_Buffer_Encode( long N);

char Get_Bit_Encode( void);
NODE *Get_Node_Encode( void);

void Write_Code_Encode( long Code,long Len ,bool Flush);
void Add_Node_Encode( NODE *p, SIDE S);

void Reset_Tree_Encode( NODE *r);


long tBit;     /* total bit wriiten in file*/
long MaxLen;   /* keeps the len of code has found*/
long MaxCode;    /* how many vode pattern id found */



#if defined SHOW_TREE

/* ========================================================================
 *preorder traversal of tree
 */
void PreOrder( NODE *p,long Dep)
{
	if( p){
		printf("%ld %ld %ld\n",p->Code,p->Len,p->Dep);
		PreOrder( p->lChild);
		PreOrder( p->rChild);
	}
}

#endif


/* ========================================================================
 *it processes all the encodein by takin one
 * by one bit and traversin code tree
 * if find any leaf then write the code
 * and add node new pattern
 */
void Process_Encode( void)
{
	COMMAND_FOR_CHANGE1 =COMMAND_FOR_CHANGE2 =true;

	/**** initialize Tree */
	Root =Get_Node_Encode();
	Root->Code =-1;
	Root->Len =1;
	Root->Dep =0;

	/** left node get 0 code **/
	Root->lChild =Get_Node_Encode();
	Root->lChild->Code =0;
	Root->lChild->Len =1;
	Root->lChild->Dep =1;

	/** right node get 1 code **/
	Root->rChild =Get_Node_Encode();
	Root->rChild->Code =1;
	Root->rChild->Len =1;
	Root->rChild->Dep =1;

	MaxCode =2;       /* as two patern will b initialy in tree*/
	MaxLen =1;        /* len of two pattern is 1 */


	long b;           /* b is used to take next bit*/
	NODE *p =Root;    /* to travers tree */


	/* continue untill enf file */
	while( (b =Get_Bit_Encode()) !=EOF){

#ifdef SHOW_READ_BIT
	printf("%ld",b);
#endif
		/* check for bit */
		if( b==1){			
			/* continuin traversin */
			if( p->rChild ) p =p->rChild;
			/**if a leaf is found then write the code and add node**/
			else {
				Write_Code_Encode( p->Code,p->Len ,false);
				/* to limit Dic Paterrn or code entry*/
				if( MaxCode <MaxDicEntry ) Add_Node_Encode( p,Right);			
				p =Root->rChild;    /* init*/
			}
			/* continuin traversin */
		}
		/** same process for right child*/
		else{
			if( p->lChild ) p =p->lChild;
			else {
				Write_Code_Encode( p->Code,p->Len ,false);
				if( MaxCode <MaxDicEntry ) Add_Node_Encode( p,Left);
				p =Root->lChild;
			}
		}

#ifdef SHOW_TREE
	/* to show the tree */
	printf("Startin\n");
	PreOrder( Root,0);
	printf("end\n\n");
#endif

	}
	Write_Code_Encode( p->Code,p->Len,false);
	/* flushin buffer */
	Write_Code_Encode( 0,0,true);

	/* wrte the hw many bit is written */
	FILE *fp =fopen(Compress2,"wb");
	fwrite( &tBit, sizeof(long),1,fp);
	/* reset for Next Operation */
	tBit =0;
	/* freeing tree node */
	Reset_Tree_Encode( Root);

}
/* ========================================================================
 */




/* ========================================================================
 *read from file using static fp
 */
long Read_Buffer_Encode( void)
{
	static FILE *fp =fopen( FileName ,"rb");
	if( COMMAND_FOR_LOAD){
		fp =fopen( FileName ,"rb");
		COMMAND_FOR_LOAD =false;
	}
	return fread( ReadBuffer, sizeof( char), MAX_BUFFER_LEN,fp);
}
/* ========================================================================
 */



/* ========================================================================
 *write into comprsed file file using static fp
 */
void Write_Buffer_Encode( long N)
{
	static FILE *fp =fopen( Compress1,"wb");
	if( COMMAND_FOR_SAVE){
		fp =fopen( Compress1,"wb");
		COMMAND_FOR_SAVE =false;
	}
	fwrite( WriteBuffer ,sizeof( char), N,fp);
}
/* ========================================================================
 */


/* ========================================================================
 * function used to get 1 bit from file
 * retuen EOF if N is 0 that means
 * no more bit is existed in file
 */
char Get_Bit_Encode( void)
{
	static long i =0;		/* used for counting to check is buffer full*/
	static unsigned V =1;   /* V used to compare value for parsing bit*/
	static long N =0;       /* used to find hw many char is found in file */

	/* init the static var */
	if( COMMAND_FOR_CHANGE1){
		i =0;
		V =1;
		N =0;
		COMMAND_FOR_CHANGE1 =false;
	}


	/* V is increased one right shift to use with & oper */
	V >>=1;	
	/* j will be 0 if whole char is parsin bit wise*/
	if( !(char)V){
		/* check is ReadBuffer is all read */
		i++;
		if( i >=N){
			N =Read_Buffer_Encode();
			if( !N) return EOF;
			i =0;
		}
		
		V =128;
	}

	if( ReadBuffer[i] & V) return 1;
	else return 0;
}
/* ========================================================================
 */




/* ========================================================================
 *used to get initialized Node
 */
NODE *Get_Node_Encode( void)
{
	NODE *p =(NODE*) malloc( sizeof( NODE));
	p->lChild =p->rChild =NULL;
	return p;
}
/* ========================================================================
 */




/* ========================================================================
 *just macro func use to procced next char and
 *init i,j,charBuffer
 */
#define PROCEED_BUFFER( i,j,CharValue) {  \
			WriteBuffer[i] =CharValue; \
			i++; \
			if( i>=MAX_BUFFER_LEN){ \
				Write_Buffer_Encode( MAX_BUFFER_LEN); \
				i =0; \
			} \
			j =0; \
			CharValue =0; \
		}




/* ========================================================================
 * function used to write code 
 * first of all it write all the extra len than maxlen
 * all the leading value will be 0 bit
 * then parse code and took it in charvalue
 * if flush flag is tru then it just write the pre 
 * code in the file and return
 */
void Write_Code_Encode( long Code ,long Len ,bool Flush)
{
	static long i =0;         /* used to count the array index of WriteBuffer */	
	static long j =0;         /* used to count is 8 bit written */
	static char CharValue =0; /* it took the value of 8bit during parsein */
	const long Limit =8;

	/* init the new var for new com */
	if( COMMAND_FOR_CHANGE2){
		i =j =0;
		CharValue =0;
		COMMAND_FOR_CHANGE2 =false;
	}

	if( Flush){
		/* proceed charvalue in higher bit*/
		CharValue <<= Limit -j;
		WriteBuffer[i] =CharValue;
		/* wrte in file com */
		Write_Buffer_Encode( i+1);
		return;
	}



	long V =1<<(Len-1);      /* v is ued to compare with code to get the bit*/
	while( Len <MaxLen){
		/** add extra 0 bit to make code equal to Maxlen */
		CharValue =CharValue<<1;

#ifdef SHOW_WRITE_BIT
	printf("0");
#endif
		/* value incrin */
		tBit++;
		Len++;
		j++;
		/*  a char is filled so cahck next index*/
		if( j==Limit) PROCEED_BUFFER( i,j,CharValue);
	}

	while( V){
		if( Code & V){
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
		V >>=1;

		tBit++;
		j++;
		if( j==Limit) PROCEED_BUFFER( i,j,CharValue);

	}
}	
/* ========================================================================
 */




/* ========================================================================
 * add a node left or right of node p
 * then add the len of the code
 * also increase the MaxLen of Code
 */
void Add_Node_Encode( NODE *p, SIDE S)
{
	if( S==Left){
		p->lChild =Get_Node_Encode();
		p->lChild->Dep =p->Dep+1;
		p =p->lChild;
	}
	else{
		p->rChild =Get_Node_Encode();
		p->rChild->Dep =p->Dep+1;
		p =p->rChild;
	}

	p->Code =MaxCode++;// Num of Code increase
	p->Len =0;
	long V =p->Code;
	/* find out hte len of Code
	* by continously divide it binary */
	while( V){
		V =V>>1;
		p->Len++;
	}
	if( MaxLen<p->Len) MaxLen =p->Len;
}
/* ========================================================================
 */




/* ========================================================================
 * Post order traversing
 * to free allocating memory
 * al lat it free root
 */
void Reset_Tree_Encode( NODE *r)
{
	if(r){
		Reset_Tree_Encode( r->rChild);
		Reset_Tree_Encode( r->lChild);
		/* free root */
		free(r);
	}
}
		
/* ========================================================================
 */