#include "dir.h"
#include "sd_detect.h"
#include "../ASCII/ASCII_ctrl.h"

dir_c::dir_c(void)
{	// dir_c
	dir_open=false;
	file_is_dir=false;
	count_done=false;
	find_done=false;
	file_cnt=0;
	file_number=0;
	set_str_n(file_filter,wildcard,DOS_FILE_NAME_EXT_LENGTH);
	set_str_n(file_name_ext,wildcard,DOS_FILE_NAME_EXT_LENGTH);
	set_str_n(file_search_name,wildcard,DOS_FILE_NAME_LENGTH);
	file_filter[DOS_FILE_NAME_LENGTH]=dot;
	file_name_ext[DOS_FILE_NAME_LENGTH]=dot;
	file_filter[DOS_FILE_NAME_EXT_LENGTH]=NUL;
	file_name_ext[DOS_FILE_NAME_EXT_LENGTH]=NUL;
	file_search_name[DOS_FILE_NAME_LENGTH]=NUL;
}	// dir_c

uint16_t dir_c::get_file_cnt(void)
{	// get_file_cnt
	if (!count_done)
	{	// to do
		next_file();
	}	// to do
	
	return file_cnt;
}	// get_file_cnt

uint16_t dir_c::get_current_file_number(void)
{	// get current file number
	return file_number;
}	// get current file number

char * dir_c::get_file_name(uint16_t number)			// default=current file name
{
	if (number)
	{	// skip to number
		if ((number<=file_cnt) || !count_done)
		{	// file could exist
			if (number<file_number)
			{	// requested file allready passed
				if (dir_open) close_dir();
				file_number=0;
			}	// requested file allready passed
			
			bool bContinue=true;
			while ( (file_number < number) && 	// not yet reached and
					bContinue )					// and data to process
				bContinue=next_file();
		}	// file could exist
	}	// skip to number

	return ((number==0) || ((file_number>0) && (file_number==number)))?(file_name_ext):(NULL);
}


void dir_c::set_extension_filter(const char * str)
{	// set extension filter
	char * ext=&file_filter[DOS_FILE_NAME_LENGTH+1];

	if (!cmp_str_n(str,ext,DOS_FILE_EXT_LENGTH,true))
	{	// different, set new
		set_str_n(ext,wildcard,DOS_FILE_EXT_LENGTH);
		cpy_str_n(ext,str,DOS_FILE_EXT_LENGTH);
		count_done=false;
		find_done=false;
	}	// different, set new
}	// set extension filter

void dir_c::set_name_filter(const char * str)
{	// set name filter
	uint8_t cnt=0;
	
	while (str[cnt] && (str[cnt]!=dot) && (cnt<DOS_FILE_NAME_LENGTH)) cnt++; 

	set_str_n(file_filter,wildcard,DOS_FILE_NAME_LENGTH);
	cpy_str_n(file_filter,str,cnt);
	if (str[cnt]==dot) set_extension_filter(&str[cnt+1]);
	count_done=false;
	find_done=false;
}	// set name filter

void dir_c::set_search_name(const char * str)					// name wich need to be matched within filters
{	// set search name
	uint8_t cnt=0;
	while (str[cnt] && (str[cnt]!=dot) && (cnt<DOS_FILE_NAME_LENGTH)) cnt++; 

	set_str_n(file_search_name,wildcard,DOS_FILE_NAME_LENGTH);
	cpy_str_n(file_search_name,str,cnt);
	find_done=false;
}	// set search name


bool dir_c::next_file(void)									// go to next dir entry, regardless of match
														// true on succes, false if end is reached
{	// next_file
	bool result=false;

	if (!dir_open) open_dir();
	
	tmp=DIR.openNextFile();	// try to get next file
	result=tmp;				// get result
	if (result)
	{	// succes get name
		set_str_n(file_name_ext,NUL,DOS_FILE_NAME_EXT_LENGTH+1);
		cpy_str_n(file_name_ext,tmp.name(),DOS_FILE_NAME_EXT_LENGTH);
		split_file_name_ext();
		file_is_dir=tmp.isDirectory();
		tmp.close();

		if (is_match())
		{	// check match and count
			file_number++;
			
			if (!count_done)
			{	// count files
				file_cnt++;
			}	// count files
		}	// check match and count
	}	// succes get name
	else
	{	// end of dir
		count_done=true;
		close_dir();
	}	// end of dir
	return result;
}	// next_file													


bool dir_c::find_file(void)
{	// find_file
	
	if (!find_done)
	{	// work in progress
		if (!dir_open) tmp_file_number=0;
		
		if (next_file())
		{	// file open
			if ( (!file_is_dir) && 
				 is_match_name(file_search_name,true) &&
				 is_match_extension(true) )
			{	// candidate
				tmp_file_number=file_number;
				find_done=true;
			}	// candidate
		}	// file open
		else
		{	// done
			find_done=true;
			file_number=0;
			close_dir();
		}	// done
	}	// work in progress
	else
	{	// previously finished
		file_number=0;
		find_done=false;
		if (dir_open) close_dir();
	}	// previously finished

	return !find_done;
}	// find_file

char * dir_c::get_next_file_name(void)					// get next matching file name, blocking call 
{	// get_next_file_name
	bool bEnd;

	do
	{	// search
		bEnd=!next_file();
	}	// search
	while (!bEnd && !is_match());

	return (!bEnd)?(file_name_ext):(NULL);
}	// get_next_file_name

bool dir_c::hasSD(void)
{	//	hasSD
	return sd_detect.present();
}	//	hasSD

bool dir_c::isBusy(void)
{	// isBusy
	return !count_done || dir_open;
}	// isBusy									

bool dir_c::is_match(void)									// check if dir entry matches filter
{	// is_match
	
	return is_match_name(false) && is_match_extension(false);
}	// is_match


char *dir_c::get_name(void)
{ 	// get_name
	return temp_file_name; 
}	// get_name

char *dir_c::get_extension(void)
{	// get_extension
	return temp_file_ext; 
} 	// get_extension


/* ******************************************
 * private
 * *******************************************
 */
void dir_c::set_str_n(char * str,const char c,uint8_t cnt)
{	// fill string with char
	while (cnt-->0) *str++=c;
}	// fill string with char

void dir_c::cpy_str_n(char * dest, const char *src, uint8_t cnt)
{	// cpy_str_n
	// cpy  max n chars without trailing '\0'
	while ((cnt--) && (*src)) 
	{ 	// copy char
		*dest++=*src++;
	}	// copy char
}	// cpy_str_n

void dir_c::split_file_name_ext(void)
{	//	split_file_name_ext
	char * src=file_name_ext;
	char * dest=temp_file_name;
	uint8_t cnt=DOS_FILE_NAME_LENGTH;
	
	set_str_n(temp_file_name,NUL,DOS_FILE_NAME_LENGTH+1);
	set_str_n(temp_file_ext,NUL,DOS_FILE_EXT_LENGTH+1);
	
	while ((cnt--) && (*src) && (*src!=dot)) *dest++=*src++;
	
	if (*src==dot)
	{	// there is an extension
		src++;
		dest=temp_file_ext;
		cnt=DOS_FILE_EXT_LENGTH;
		while ((cnt--) && (*src) && (*src!=dot)) *dest++=*src++;
	}	// there is an extension
}	// split_file_name_ext


void dir_c::open_dir(void)
{	// open dir
	DIR=SD.open("/");
	dir_open=true;
	file_number=0;
	if (!count_done) file_cnt=0;
}	// open dir

void dir_c::close_dir(void)
{	// close dir
	dir_open=false;
	DIR.close();
}	// close dir

bool dir_c::is_match_name(bool exact)			// file matches extension filter, exact: wildcards will be ignored
{
	return is_match_name(file_filter,exact);
}

bool dir_c::is_match_name(const char * ref, bool exact)
{
	return cmp_str_n(file_name_ext,ref,DOS_FILE_NAME_LENGTH,exact);
}

bool dir_c::is_match_extension(bool exact)		// file matches name filter, exact: wildcards will be ignored
{
	char *ext=file_name_ext;
	
	while ((*ext) && (*ext!=dot)) ext++;
	
	if (*ext==dot) ext++;
	
	return cmp_str_n(ext,&file_filter[DOS_FILE_NAME_LENGTH+1],DOS_FILE_EXT_LENGTH,exact);
}

bool dir_c::cmp_str_n(const char * test, const char *ref, uint8_t cnt, bool exact)
{	// cmp_str_n
	while (	( (*test==*ref) || 						// match or
			  ( (*ref==wildcard) && !exact)	) &&	// wildcard
			( (*test!=dot) && (*ref!=dot) &&		// and not delimiter
			  (*test) && (*ref) && (cnt) ) )		// and not end
	{	// compare
		test++;
		ref++;
		cnt--;
	}	// compare
	
	/* **************************************
	 * result
	 * cnt==0: 
	 * 		complete match on all n characters or wildcard
	 * (*test==dot || *test==NULL) && *ref==wildcard: 
	 * 		full match on first chars
	 */
	return 	(cnt==0) ||	
			( (cnt!=0) && (*test==dot || *test==NUL) && (*ref==wildcard) );
}	// cmp_str_n
