/**
* file_def.h
* Author : Asif Mahmud Shimon
*/
#ifndef OP_FILE_DEF_H
#define OP_FILE_DEF_H

#ifdef __cplusplus
extern "C" {
#endif


#ifdef _WIN32
#define SEPERATOR "\\"
#else
#define SEPERATOR "/"
#endif


/**
* File info structure
*/
typedef struct file_info_t
{
	char name_without_ext[32];
	char name_with_ext[32];
	char name[64];
}file_info_t;

/**
* A Linked list of files in a given directory
*/
typedef struct file_list_t
{
	file_info_t * file;
	struct file_list_t * next;
}file_list_t;


#ifdef __cplusplus
}
#endif

#endif//OP_FILE_DEF_H
