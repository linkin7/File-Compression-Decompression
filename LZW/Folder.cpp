/* This file implements system
 * Handle to create folder
 * Dependent on
 * Windows platform
 */


#include<stdio.h>
#include<string.h>
#include<windows.h>

extern char FileName[FILENAME_MAX];			/* file to be compressed */	
extern char Directory[FILENAME_MAX];		/* Compress to be kept */

extern char Compress1[FILENAME_MAX];		/* compress file holding code */
extern char Compress2[FILENAME_MAX];        /* compress file holdin tree */

extern void Process_Encode( void);
extern void Process_Decode( void);

bool COMMAND_FOR_LOAD;
bool COMMAND_FOR_SAVE;


void Set_Folder_Encode( char *Input)
{
	char Temp[FILENAME_MAX];
	DWORD Attrib =GetFileAttributes( Input);

	GetFullPathName( Input,FILENAME_MAX-1,Temp,NULL);
	*(strrchr( Temp,'\\')) ='\0';
	strcpy( Directory,Temp);
	strcat( Directory,"\\C");
	CreateDirectory( Directory,NULL);

	WIN32_FIND_DATA Data;
	strcpy( Temp,Input);
	if( Attrib & FILE_ATTRIBUTE_DIRECTORY) strcat( Temp,"\\*.*");
	HANDLE H =FindFirstFile( Temp , &Data);

	if( H!=INVALID_HANDLE_VALUE){		
		do{
			strcpy( FileName ,Input);
			if( Attrib & FILE_ATTRIBUTE_DIRECTORY){
				strcat( FileName,"\\");
				strcat( FileName,Data.cFileName);
			}
					
			strcpy( Temp,Directory);
			strcat( Temp,"\\");
			strcat( Temp,Data.cFileName);
			if( Data.dwFileAttributes!=FILE_ATTRIBUTE_DIRECTORY){
				long Check =CreateDirectory( Temp,NULL);
				if( !Check){
					printf("Error\n");
					return;
				}
			}

			if( Data.dwFileAttributes==FILE_ATTRIBUTE_DIRECTORY) printf("Exclude Folder %s\n",Data.cFileName);
			else  printf("Compressing... %s\n", Data.cFileName);


			strcpy( Compress1,Temp);
			strcat( Compress1,"\\LZW1");

			strcpy( Compress2,Temp);
			strcat( Compress2,"\\LZW2");
			
			if( Data.dwFileAttributes!=FILE_ATTRIBUTE_DIRECTORY){
				/* to init fp pointer */
				COMMAND_FOR_LOAD =COMMAND_FOR_SAVE =true;
				Process_Encode();
			}
		} while( FindNextFile( H,&Data));
	}
	else printf("File Not Found\n");
}


void Set_Folder_Decode( char *Input)
{
	char Temp[FILENAME_MAX];
	DWORD Attrib =GetFileAttributes( Input);

	GetFullPathName( Input,FILENAME_MAX-1,Temp,NULL);
	*(strrchr( Temp,'\\')) ='\0';
	strcpy( Directory,Temp);
	strcat( Directory,"\\E");
	CreateDirectory( Directory,NULL);


	WIN32_FIND_DATA Data;
	strcpy( Temp,Input);
	strcat( Temp,"\\*.*");
	HANDLE H =FindFirstFile( Temp,&Data);

	if( H!=INVALID_HANDLE_VALUE){
		do{
			strcpy( FileName,Directory);
			strcat( FileName,"\\");
			strcat( FileName,Data.cFileName);
			if( *Data.cFileName!='.') printf("Extracting... %s\n",Data.cFileName);

			strcpy( Compress1,Input);
			strcat( Compress1,"\\");
			strcat( Compress1,Data.cFileName);
			strcat( Compress1,"\\LZW1");

			strcpy( Compress2,Input);
			strcat( Compress2,"\\");
			strcat( Compress2,Data.cFileName);
			strcat( Compress2,"\\LZW2");

			if( *Data.cFileName!='.' ){
				/* to init fp pointer */
				COMMAND_FOR_LOAD =COMMAND_FOR_SAVE =true;
				Process_Decode();
			}
		} while( FindNextFile( H,&Data));
	}
	else printf("FIle Not Found\n");
}








