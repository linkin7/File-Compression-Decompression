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

extern char CompressCode[FILENAME_MAX];		/* compress file holding code */
extern char CompressTree[FILENAME_MAX];     /* compress file holdin tree */

extern void Encode( void);
extern void Decode( void);

extern bool COMMAND_FOR_LOAD;
extern bool COMMAND_FOR_SAVE;


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
			else  printf("Compressing... %s\n", FileName);


			strcpy( CompressCode,Temp);
			strcat( CompressCode,"\\Huff1");

			strcpy( CompressTree,Temp);
			strcat( CompressTree,"\\Huff2");
			
			if( Data.dwFileAttributes!=FILE_ATTRIBUTE_DIRECTORY){
				COMMAND_FOR_LOAD =COMMAND_FOR_SAVE =true;
				Encode();
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

			strcpy( CompressCode,Input);
			strcat( CompressCode,"\\");
			strcat( CompressCode,Data.cFileName);
			strcat( CompressCode,"\\Huff1");

			strcpy( CompressTree,Input);
			strcat( CompressTree,"\\");
			strcat( CompressTree,Data.cFileName);
			strcat( CompressTree,"\\Huff2");

			if( *Data.cFileName!='.'){
				COMMAND_FOR_LOAD =COMMAND_FOR_SAVE =true;
				Decode();
			}
		} while( FindNextFile( H,&Data));
	}
	else printf("FIle Not Found\n");
}








