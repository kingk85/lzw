/*
# This file defines the class
#
#*/

#include "c.h"
#include <fstream>

struct lzwn {
//The byte value!
unsigned char value;
//The index/prefix value
unsigned int index;
//The value code === node index
unsigned int code;
//Search pointers
struct lzwn *sp[search_points];
struct lzwn *next;
};

struct bitwrite
{
unsigned char bit_size; //The data format
unsigned char bit_write_buffer[bit_write_buffer_size]; //The byte buffer
unsigned char bit_write_buffer_curs;
unsigned char bit_position;
};


//Class ulzw
class ulzw {

//Private vars
private:
			unsigned int primo, secondo, next_code, oldcode, code, newcode, chr;
			int compress_action, compress_flag;
			int givecode_processed, givecode_rest_bits;
			unsigned int givecode_outcode;
			unsigned char givecode_rest;
			int decompress_flag, decompression_reset_dic, decompression_dic_resetted, compression_dic_resetted;
			int setted_dictionary_size;
			unsigned char * reversing_buffer;
			unsigned char * compress_buffer;

			//functions vars
			int prefisso, to_rt, i, x, towrite, wbinc, find_result,  toret, output_curs, write_cursor, rb_index;
			//unsigned char xors[] = {0x01, 0x02,0x04,0x08,0x10,0x20,0x40,0x80};
			unsigned char ilbit, the_byte;
			unsigned char pb[buffer_size_small];
			unsigned int adder;

			int write_pointer_index;
			std::ofstream file_out;
//Public vars
public:
		//Class constructor
		ulzw();
		void print_version();
		int GetBitFlag( unsigned char , int  );
		unsigned char SetBitFlag(unsigned char, int, unsigned char);
		int print_buffered(unsigned int, unsigned char[]);
		int find(int, unsigned char);
		int compress_chr(unsigned char, unsigned char[], int);
		int decompress_chr(unsigned char, unsigned char[], int);
		int decompress_buffer_to_file(unsigned char[], int, char[], int);
		int compress_buffer_to_file(unsigned char[], int, char[], int, int);
		void dic_init();
		int put_to_table(unsigned int, unsigned char);
		unsigned int get_end_bit();
		int give_code(unsigned char);
		void set_dictionary_size(int);
		int decoding_printf(unsigned int, unsigned char[], int);
		char dictionary_bits;
};
