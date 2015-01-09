/**
* opprog.h
* Author : Asif Mahmud Shimon
*/
#ifndef LIB_OPPROG_H
#define LIB_OPPROG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <hidapi.h>
#include <common.h>

typedef struct hid_device_info hid_dev_info;

typedef struct
{
        USHORT vid;
        USHORT pid;
        wchar_t manufacturer_string[MAX_NAME_LEN];
        wchar_t product_string[MAX_NAME_LEN];
        wchar_t serial_number_string[MAX_NAME_LEN];
        wchar_t indexed_string[MAX_NAME_LEN];
}op_hid_device_info;

/**
* A Structure for libopprog Generic use
* It contains a device handle of USB HID class device, Memory buffer for MCU/Memory Chips and
* necessary functions to send to/ recieve from or search Systems's attached HID class devices.
*/
typedef struct opprog_t
{
	/**
	* When open_device called successfully this structure will be filled up
	* with opened device info.
	*/
    op_hid_device_info * dev_info;

	/**
	* When open_device called successfully this structure will be filled up
	* with opened device handle.
	*/
	hid_device * dev_hnd;

	/**
	* Memory Buffer for a Microcontroller
	* See op_mcu_mem_t in "common.h" for more detail
	* Fill it up for generic use.
	*/
	op_mcu_mem_t * memory;

	/**
	* Find a HID Device with VID and PID
	* @param vid : unsigned short vid of target device
	* @param pid : unsigned short pid of target device
	* @return : if device found 1 else 0
	*/
	OP_FUNC_WRAP( int , find_device)( USHORT, USHORT );


	/**
	* Get a devices info structure.
	* @param vid : vid of the device
	* @param pid : pid of the device
	* @return : hid_dev_info structure else NULL. See hid_enumerate
	* @remark : User should free the structure manually
	*/
    OP_FUNC_WRAP( void , get_device_info )(struct opprog_t *, USHORT, USHORT );
	
	/**
	* Opens a device for further use. Calling this function will fill up or free
	* working_device structure upon success.
	* @param opprog_t : a pointer of opprog_t object
	* @param vid : unsigned short vid of target device
	* @param pid : unsigned short pid of target device 
	* @return : void
	* @remark : opprog_t object must not be NULL
	*/
	OP_FUNC_WRAP( void, open_device )( struct opprog_t *, USHORT, USHORT );

	/**
	* An interface to hidapi to send some bytes to the device
	* @param opprog_t : a pointer of opprog_t object
	* @param buffer : data buffer	
	* @return : actual size of data sent else -1 if error
	*/
	OP_FUNC_WRAP( int, send )( struct opprog_t *, op_buffer_t * );	

	/**
	* An interface to hidapi to read some bytes from the device
	* @param opprog_t : a pointer of opprog_t object
	* @param buffer : data buffer
	* @return : actual size of data sent else -1 if error
	* @remark : buffer->len == number of bytes to be read
	*/
	OP_FUNC_WRAP( int, recv )( struct opprog_t *, op_buffer_t * );

	/**
	* Fill-up Flash, EEPROM or configuration bytes inside the initialized opprog_t object.
	* @param opprog_t : a pointer of opprog_t object
	* @param mem_type : FLASH/EEPROM/CONFIG
	* @param data : data to be filled with
	* @param size : size of the data 
	* @return : 0 if SUCCESS else -1
	*/
	OP_FUNC_WRAP( int, fill_up_memory )( struct opprog_t *, OP_MEM_TYPE, UCHAR *, size_t );

	/**
	* Function to clear memory data. Use it to clear the buffer selected by OP_MEM_TYPE
	* for further use.
	* @param op : a pointer og opprog_t object
	* @param type : type of buffer to be cleared
	*/
	OP_FUNC_WRAP(void, clear_memory)(struct opprog_t *, OP_MEM_TYPE);

	/**
	* Function to append data at the end of a buffer.
	* @param op : a pointer og opprog_t object
	* @param type : type of buffer to be used to append
	* @param data : data to be appended
	* @param len : size of the given data
	*/
	OP_FUNC_WRAP(void, append_data)(struct opprog_t *,OP_MEM_TYPE, UCHAR *, size_t);
}opprog_t;

/**
* START FROM HERE
* before doing anything with the library, initialize it by op_init
* @return : an opprog_t pointer to be used further
*/
opprog_t * op_init();

/**
* FREE UP EVERYTHING 
* after you're finished with opprog_t, free-up it.
* @param obj : opprog_t object pointer to be freed
*/
void op_exit(opprog_t *);

#ifdef __cplusplus
}
#endif

#endif//LIB_OPPROG_H
