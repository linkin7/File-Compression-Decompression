
/*****************************************
 * Implement heap
 * in Array manipulation
 * reference from 
 * Fundamentals of Com. Alg.
 * by Horowitz Sahani Rajasek.
 */


#include<stdlib.h>

#define MAX_NODE 514

extern struct NODE{
	long Digit;
	long Freq;
	NODE *Left,*Right;
} Temp;


struct HEAP{
	NODE Data[MAX_NODE];
	long N;
} H;


void Heap_Push( NODE p)
{
	H.N++;
	H.Data[H.N] =p;
	long i =H.N;
	while( i>1 && H.Data[i/2].Freq >p.Freq){
		H.Data[i] =H.Data[i/2];
		i =i/2;
	}
	H.Data[i] =p;
}


void Adjust( long ind)
{
	long j =2*ind;
	Temp =H.Data[ind];
	while( j<=H.N){

		if( j<H.N && H.Data[j].Freq >H.Data[j+1].Freq)
			j =j+1;
		if( Temp.Freq <=H.Data[j].Freq) break;
		H.Data[j/2] =H.Data[j];
		j =2*j;
	}
	H.Data[j/2] =Temp;
}


NODE *Heap_Pop( void)
{

	if( !H.N) return NULL;

	NODE *t =(NODE*)malloc( sizeof(NODE));
	*t =H.Data[1];
	H.Data[1] =H.Data[H.N];

	H.N--;
	Adjust( 1);
	return t;
}


void Clear_Heap( void)
{
	H.N =0;
}