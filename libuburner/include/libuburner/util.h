/**
* util.h
* Author : Asif Mahmud Shimon
*/
#ifndef OP_UTIL_H
#define OP_UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <common.h>
#include <opprog.h>

/**
* String trimming function
* to remove leading and tailing whitespaces.
* @param in : source string
* @param out : destination string
* @return : length of output string
*/
int strtrim(const char* ,char *);

/**
* Hex to Integer converter
* @param hex : char string containg the hex data
* @param len : length of hex
* @return : converted integer, 0 by default
*/
int htoi(const char *, int );

/**
* Read hardware and firmware versions
*/
op_hardware_t * get_hardware_id(opprog_t *);

/**
* An untility function for the programmers.
* If they need the high voltage regulator just use it.
* @param op : opprog_t object
* @param volt : voltage level to be achieved
* @return :if fails -1 else the voltage achieved
*/ 
double start_HV_regulator(opprog_t *,double );

/**
* Function to get current time in millisecond
*/
ULONG get_tick_count();

#ifdef _WIN32
char * strtok_r(char *str, const char *delim, char **save);
char * strcasestr(const char * str1, const char * str2);
#endif

#ifdef __cplusplus
}
#endif

#endif//OP_UTIL_H
