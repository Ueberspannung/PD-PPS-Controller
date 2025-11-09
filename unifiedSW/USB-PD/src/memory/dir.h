#ifndef __dir_h__
#define __dir_h__

#include <stdint.h>
#include "SD.h"


class dir_c
{
	public:
		static const uint8_t DOS_FILE_NAME_LENGTH=8;
		static const uint8_t DOS_FILE_EXT_LENGTH=3;
		static const uint8_t DOS_FILE_NAME_EXT_LENGTH=12;
		static const char dot='.';

		dir_c(void);
		
		uint16_t get_file_cnt(void);							// non blocking, result ok when not busy after call
		uint16_t get_current_file_number(void);							
		char * get_file_name(uint16_t number=0);				// default=current file name
																// else blocking call
		
		void set_extension_filter(const char * str);			// ext will be padded with wildcard
		void set_name_filter(const char * str);					// name will be padded with wildcard
		void set_search_name(const char * str);					// name wich need to be matched within filters

		bool next_file(void);									// go to next dir entry, regardless of match
																// true on succes, false if end is reached
		
		bool find_file(void);									// non blcoking file search, false when finished							
		char * get_next_file_name(void);						// get next matching file name, blocking call 

		bool hasSD(void);
		bool isBusy(void);										

		bool is_match(void);									// check if dir entry matches filter

		char *get_name(void); 
		char *get_extension(void);  
		
	private:
	

		static const char wildcard='?';

		char file_name_ext[DOS_FILE_NAME_EXT_LENGTH+1];
		char file_filter[DOS_FILE_NAME_EXT_LENGTH+1];
		char file_search_name[DOS_FILE_NAME_LENGTH+1];

		char temp_file_name[DOS_FILE_NAME_LENGTH+1];
		char temp_file_ext[DOS_FILE_EXT_LENGTH+1];

		bool dir_open;
		bool file_is_dir;
		File	tmp;
		File	DIR;
		
		uint16_t file_cnt;		// number of matching files
		uint16_t file_number;	// current number of matching file
		uint16_t tmp_file_number; // temporary file number storage
		bool	 count_done;
		bool	 find_done;

		void set_str_n(char * str, const char c,uint8_t cnt);		
		void cpy_str_n(char * dest, const char *src, uint8_t cnt);

		void split_file_name_ext(void);
		
		void open_dir(void);
		void close_dir(void);
		
		bool is_match_name(bool exact);			// file matches extension filter, exact: wildcards will be ignored
		bool is_match_name(const char * ref, bool exact); // file matches supplied name
		bool is_match_extension(bool exact);	// file matches name filter, exact: wildcards will be ignored
		bool cmp_str_n(const char * test, const char *ref, uint8_t cnt, bool exact);
};

#endif //__dir_h__
