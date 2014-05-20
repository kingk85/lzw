/*
# This file defines class methods
#
#*/
#include <cstdlib>
#include <iostream>
#include "string.h"
#include <fstream>
#include "C_c.cpp"
#include "c.h"

using namespace std;

struct lzwn * dictionary;
struct bitwrite bitw;

ulzw::ulzw () {
bitw.bit_size = 9;
bitw.bit_position = 0;
givecode_processed = 0;
givecode_outcode = 0;
givecode_rest = 0;
givecode_rest_bits = 0;
decompression_dic_resetted = 0;
setted_dictionary_size = dictionary_size;

reversing_buffer =  (unsigned char *) malloc(setted_dictionary_size);
compress_buffer =  (unsigned char *) malloc(setted_dictionary_size);

dictionary = (lzwn *) malloc( sizeof(lzwn)*setted_dictionary_size );
dictionary_bits = 16;
}




int ulzw::compress_buffer_to_file(unsigned char comp_buffer[], int buffer_len, char save_filename[], int flag, int append)
{
	
	int compressed_chars = 0;
	int compress_buffer_cursor = 0;
	
	if ( flag == 3 && compress_flag == 0)
	{
		output_curs = 0;
		//unsigned char pb[buffer_size_small];
		towrite = print_buffered(primo, pb);
		for (  x = 0; x < towrite; x++ )
			compress_buffer[output_curs++] = pb[x];
	
	//The file is end, let's write a special termination char!
		towrite = print_buffered(get_end_bit(), pb);
		for (  x = 0; x < towrite; x++ )
			compress_buffer[output_curs++] = pb[x];
			 
		 compressed_chars+=output_curs;
		if ( output_curs > 0 )
		 file_out.write((char *)compress_buffer, output_curs); 
			file_out.close();
	return output_curs;
	}
	


	
	
	compress_buffer_begin:

	unsigned char input;
	
	while (compress_buffer_cursor < buffer_len ){
	output_curs = 0;
	input = comp_buffer[compress_buffer_cursor];
	compress_buffer_cursor++;
	
	//Flag == 0, new comrpession!
	if (flag == 0 )
	{
		if ( append == 0 )
		file_out.open(save_filename, ios::out|ios::binary);
		else
		file_out.open(save_filename, ios::out|ios::binary|ios::app);
	

		//Set the flag to compress
		compress_action = 0;
		
		//Normal operation mode
		compress_flag = 0;
		
		//Dic Init
		dic_init();
				
		//set primo with the 1st char value
		primo = input;
		//nothing to write..
		flag = 1;
		goto compress_buffer_begin;
	}
	
	//The compression is not the last
	if (flag == 1 )
	{
		secondo = input;
		find_result = find(primo, secondo);
		
		//Not found case
		if ( find_result == -1 )
		{
		
			//unsigned char pb[buffer_size_small];
			//Add new item into the dictionary, check if we have to increase the bit_size
			 wbinc = put_to_table(primo, secondo);
			if ( wbinc!= 0 )
			{
					if ( wbinc != setted_dictionary_size ){
					  //We have to increase the bit size, output the bit_size increment number
						towrite = print_buffered(wbinc, pb);
						for ( x = 0; x < towrite; x++ )
							compress_buffer[output_curs++] = pb[x];
						bitw.bit_size++;
					}
					else
					{
					//Dictionary is full, we have to reset it discard primo
					//unsigned char pb[buffer_size_small];
					 towrite = print_buffered(primo, pb);
					for (  x = 0; x < towrite; x++ )
						compress_buffer[output_curs++] = pb[x];
						
						//We have to reset the dictionary, write the special char
						towrite = print_buffered(wbinc, pb);
						for (  x = 0; x < towrite; x++ )
							compress_buffer[output_curs++] = pb[x];

						if ( bitw.bit_position != 0 )
						{
						compress_buffer[output_curs++] = bitw.bit_write_buffer[0];
						}
						primo = secondo;

						//Dic Init
						dic_init();
						if ( output_curs > 0 )
						file_out.write((char *)compress_buffer, output_curs);
						 compressed_chars+=output_curs;
						goto compress_buffer_begin;
					}
			}
			
			//Output primo			
			 towrite = print_buffered(primo, pb);
			for (  x = 0; x < towrite; x++ )
			 compress_buffer[output_curs++] = pb[x];
			 
			primo = secondo;
		}
		else //There is an existing item
		{
			primo = find_result;
		}
		if ( output_curs > 0 )
		file_out.write((char *)compress_buffer, output_curs);
		 compressed_chars+=output_curs;
		goto compress_buffer_begin;
	}
	

	
	}
	return compressed_chars;
}

void ulzw::set_dictionary_size(int size)
{
//cout<<"Size "<<size<<endl;
setted_dictionary_size = ( (1 << (size) ) - 1);
//cout<<"setted_dictionary_size "<<setted_dictionary_size<<endl;
free(dictionary);
dictionary = (lzwn *) malloc( sizeof(lzwn)*setted_dictionary_size );
free(reversing_buffer);
reversing_buffer =  (unsigned char *) malloc(setted_dictionary_size);

free(compress_buffer);
compress_buffer =  (unsigned char *) malloc(setted_dictionary_size);


dictionary_bits = (char) size;
}

int ulzw::decoding_printf(unsigned int pcodice, unsigned char out[], int out_cursor)
{
rb_index = 0;

if ( pcodice < 256 ){
	reversing_buffer[rb_index] = pcodice;
	rb_index++;
}
else
{
	while (pcodice >= 256)
	{
		reversing_buffer[rb_index] = dictionary[pcodice].value;
		rb_index++;
		pcodice = dictionary[pcodice].index;
	}
	
	reversing_buffer[rb_index] = pcodice;
	rb_index++;
}

for ( write_cursor = rb_index -1; write_cursor >= 0; write_cursor-- )
{
out[out_cursor++] = reversing_buffer[write_cursor];
}

return rb_index;
}

int ulzw::decompress_chr(unsigned char input, unsigned char output[], int flag)
{

	output_curs = 0;
	
	//the flag zero must be set for 2 time
	if ( flag == 0 ||  flag == 1 || decompression_dic_resetted > 0)
	{
	//reset the dictionary
	if ( flag == 0 )
	{
	dic_init();
	decompression_dic_resetted = 0;
	}
	oldcode = give_code((int)input);
	if ( oldcode == -1 )
		return output_curs;
	else
		output_curs = decoding_printf(oldcode, output, output_curs);
	//Set the flag to decompress
	compress_action = 1;

	chr = oldcode;

	if ( decompression_dic_resetted > 0 )
	decompression_dic_resetted--;

	return output_curs;
	}
	

	//Normal operation
	if ( flag == 2 )
	{
	newcode = give_code((int)input);
	if ( newcode == -1 )
		return output_curs;

	//cout<<newcode<<endl;
	if ( newcode == setted_dictionary_size )
	{
	dic_init();
	decompression_dic_resetted  = 1;

	return output_curs;
	}
	else 
	if ( newcode == get_end_bit() )
	{
	bitw.bit_size++;
	return output_curs;
	}
	
	//Exception, the string does not exist!
	if ( newcode == next_code )
	{
		output_curs = decoding_printf(oldcode, output, output_curs);
		output[output_curs++] = chr;
	}
	else
	{
		output_curs = decoding_printf(newcode, output, output_curs);
	}
	chr = (unsigned int)output[0];
	
    put_to_table(oldcode,chr);
    oldcode = newcode;
	
	return output_curs;
	}
	
	
return 0;
}



int ulzw::decompress_buffer_to_file(unsigned char decompress_buffer[], int buffer_len, char save_filename[], int flag)
{

	if ( flag == 3 )
	{
	file_out.close();
	return 0;
	}

	//cout<<"Buffer len"<<buffer_len;

	char input = 0;
	int buffer_curs = 0;
	int decompressed_size = 0;
	
	decompress_buffer_to_file_begin:
	while ( buffer_curs < buffer_len){
	input = decompress_buffer[buffer_curs];
	 buffer_curs++;
	 
	output_curs = 0;
	
	//the flag zero must be set for 2 time
	if ( flag == 0 ||  flag == 1 || decompression_dic_resetted > 0)
	{
	//reset the dictionary
	if ( flag == 0 )
	{
	file_out.open(save_filename, ios::out|ios::binary);
	dic_init();
	decompression_dic_resetted = 0;	
	//Set the flag to decompress
	compress_action = 1;
	

	}
	if ( flag == 0  )
	flag = 1;
	
	oldcode = give_code((int)input);
	if ( oldcode == -1 )
		goto decompress_buffer_to_file_begin;
	else
		output_curs = decoding_printf(oldcode, compress_buffer, output_curs);
		
	decompressed_size+= output_curs;
	file_out.write( (char*)compress_buffer, output_curs );


	chr = oldcode;

	if ( decompression_dic_resetted > 0 )
	decompression_dic_resetted--;

	if ( flag == 1  )
	flag = 2;

	goto decompress_buffer_to_file_begin;
	}
	

	//Normal operation
	if ( flag == 2 )
	{
	
	
	newcode = give_code((int)input);
	if ( newcode == -1 )
		goto decompress_buffer_to_file_begin;

	//cout<<newcode<<endl;
	if ( newcode == setted_dictionary_size )
	{
	dic_init();
	decompression_dic_resetted  = 1;

	goto decompress_buffer_to_file_begin;
	}
	else 
	if ( newcode == get_end_bit() )
	{
	bitw.bit_size++;
	goto decompress_buffer_to_file_begin;
	}
	
	//Exception, the string does not exist!
	if ( newcode == next_code )
	{
		output_curs = decoding_printf(oldcode, compress_buffer, output_curs);
		compress_buffer[output_curs++] = chr;
		decompressed_size+= output_curs;
		file_out.write( (char*)compress_buffer, output_curs );
	}
	else
	{
		output_curs = decoding_printf(newcode, compress_buffer, output_curs);
		decompressed_size+= output_curs;
		file_out.write( (char*)compress_buffer, output_curs );
	}
	chr = (unsigned int)compress_buffer[0];
	
    put_to_table(oldcode,chr);
    oldcode = newcode;
	
	goto decompress_buffer_to_file_begin;
	}
	
	}
return decompressed_size;
}


int ulzw::give_code(unsigned char ingresso)
{

	adder = ingresso;
		
	if ( givecode_rest_bits != 0)
	{
	givecode_outcode += (givecode_rest<<(bitw.bit_size-givecode_processed-8));
	givecode_processed += givecode_rest_bits;
	givecode_rest_bits = 0;
	givecode_rest = 0;
	}

	
	if (  (givecode_processed + 8) > bitw.bit_size  )
	{
		givecode_outcode += (adder>>(8-(bitw.bit_size-givecode_processed)));
		givecode_rest = (adder<<((bitw.bit_size-givecode_processed)));
		givecode_rest_bits = 8 - (bitw.bit_size-givecode_processed);
		givecode_processed = bitw.bit_size;
	}
	else
	{
		givecode_outcode += ( adder << ( bitw.bit_size-givecode_processed-8)  );
		givecode_processed += 8;
		givecode_rest_bits = 0;
		givecode_rest = 0;
	}
	
	
	//No givecode yet
	if ( givecode_processed != bitw.bit_size )
	{
		return -1;
	}
	else
	{
		toret = givecode_outcode;
		givecode_processed = 0;
		givecode_outcode = 0;
		return toret;
	}
}
/*
int ulzw::compress_buffer_to_file(unsigned char input[], int len, ofstream file_out, int flag)
{
return 0;
}*/

int ulzw::compress_chr(unsigned char input, unsigned char output[], int flag)
{
	output_curs = 0;
	
	//NORMAL OPERATION
	
	//Flag == 0, new comrpession!
	if (flag == 0 )
	{
		//Set the flag to compress
		compress_action = 0;
		
		//Normal operation mode
		compress_flag = 0;
		
		//Dic Init
		dic_init();
				
		//set primo with the 1st char value
		primo = input;
		//nothing to write..
		return output_curs;
	}
	
	//The compression is not the last
	if (flag == 1 )
	{
		secondo = input;
		find_result = find(primo, secondo);
		
		//Not found case
		if ( find_result == -1 )
		{
		
			//unsigned char pb[buffer_size_small];
			//Add new item into the dictionary, check if we have to increase the bit_size
			 wbinc = put_to_table(primo, secondo);
			if ( wbinc!= 0 )
			{
					if ( wbinc != setted_dictionary_size ){
					  //We have to increase the bit size, output the bit_size increment number
						towrite = print_buffered(wbinc, pb);
						for ( x = 0; x < towrite; x++ )
							output[output_curs++] = pb[x];
					
						bitw.bit_size++;
					}
					else
					{
					//Dictionary is full, we have to reset it discard primo
					//unsigned char pb[buffer_size_small];
					 towrite = print_buffered(primo, pb);
					for (  x = 0; x < towrite; x++ )
						output[output_curs++] = pb[x];
						
						//We have to reset the dictionary, write the special char
						towrite = print_buffered(wbinc, pb);
						for (  x = 0; x < towrite; x++ )
							output[output_curs++] = pb[x];

						if ( bitw.bit_position != 0 )
						{
						output[output_curs++] = bitw.bit_write_buffer[0];
						}
						primo = secondo;

						//Dic Init
						dic_init();
						return output_curs;
					}
			}
			
			//Output primo			
			 towrite = print_buffered(primo, pb);
			for (  x = 0; x < towrite; x++ )
			 output[output_curs++] = pb[x];
			 
			primo = secondo;
		}
		else //There is an existing item
		{
			primo = find_result;
		}
		
		return output_curs;
	}
	
	//no more chars to compress
	if (flag == 2  && compress_flag == 0)
	{
		//unsigned char pb[buffer_size_small];
		towrite = print_buffered(primo, pb);
		for (  x = 0; x < towrite; x++ )
			output[output_curs++] = pb[x];
	
	//The file is end, let's write a special termination char!
		towrite = print_buffered(get_end_bit(), pb);
		for (  x = 0; x < towrite; x++ )
			output[output_curs++] = pb[x];
			 
		 return output_curs;
	}
	
	return 0;
}


unsigned int ulzw::get_end_bit()
{
	return ( (1 << (bitw.bit_size) ) - 1);
}

void ulzw::dic_init()
{

for ( int i = 0; i < setted_dictionary_size; i++)
	{
	dictionary[i].index -1; 
	dictionary[i].value = -1;
	dictionary[i].code = -1;
	dictionary[i].next = NULL;

	for ( int spc = 0; spc < search_points ; spc++ )
		dictionary[i].sp[spc] = NULL;
	}

	next_code = 256;
	bitw.bit_size = 9;
	bitw.bit_position = 0;
	bitw.bit_write_buffer_curs = 0;
	bitw.bit_write_buffer[0] = 0;

	if (compress_action == 1) //decomrpess case
	{
	givecode_processed = 0;
	givecode_outcode = 0;
	givecode_rest = 0;
	givecode_rest_bits = 0;
	}

}

void ulzw::print_version () {
cout<<"Ulzw Alpha Build 2"<<endl;
}

//Get bit 0-7 flag return 1 or 0
int ulzw::GetBitFlag( unsigned char MyByte, int BitN )
{
MyByte = MyByte<<(7-BitN);
MyByte = MyByte>>(7-BitN+BitN);
return MyByte;
}

//Set bit 0-7 flag, return a setted Byte..
unsigned char ulzw::SetBitFlag(unsigned char MyByte, int BitN, unsigned char value)
{
	if (GetBitFlag(MyByte, BitN) == value)
		return MyByte; // It’s Already setted
	else //Need to set it!
	{
	unsigned char xors[] = {0x01, 0x02,0x04,0x08,0x10,0x20,0x40,0x80};
		MyByte = ( MyByte ^ xors[BitN]); // Need to set the bit
		return MyByte;
	}
}

int ulzw::print_buffered(unsigned int value, unsigned char buffer[])
{
	//reset the byte cursor
	bitw.bit_write_buffer_curs = 0;
	int to_process = 0;

	for ( i = bitw.bit_size; i > 0; )
	{
		if (i >= 8)
			to_process = 8;
		else
			to_process = i;

	if ( to_process > (8 - bitw.bit_position) )
		to_process = ( 8 - bitw.bit_position );

	//The byte to write into the buffer
	the_byte = ( ( (value<<(32-i)) >> (32-to_process) ) );

	bitw.bit_write_buffer[0] += the_byte;
	
	bitw.bit_position+=to_process;
	
	if (bitw.bit_position == 8 )
	{
		buffer[bitw.bit_write_buffer_curs] = bitw.bit_write_buffer[0] ;
		bitw.bit_write_buffer[0]  = 0;
		bitw.bit_write_buffer_curs++;
		bitw.bit_position = 0;
	}
	else
	{
	bitw.bit_write_buffer[0] = bitw.bit_write_buffer[0] << (8-bitw.bit_position);
	}

	i-= to_process;
	}

return bitw.bit_write_buffer_curs;
}

//Find in the dictionary
int ulzw::find(int index, unsigned char value)
{
	prefisso = index;

	if ( dictionary[prefisso].next == NULL ){
	write_pointer_index = prefisso;
	return -1;
	}
	else
	{
	prefisso = dictionary[prefisso].next->code;

		if ( dictionary[prefisso].value == value )
	return prefisso;
	
	int spc = ( ( (float)value / (float)((float)255/(float)search_points) ));
	if ( spc == search_points)
	spc--;
	
	while( dictionary[prefisso].sp[spc] != NULL ){
	prefisso = dictionary[prefisso].sp[spc]->code;
	if ( dictionary[prefisso].value == value )
	return prefisso;
	}
	}
	write_pointer_index = prefisso;
	return -1;
	

}




int ulzw::put_to_table(unsigned int index, unsigned char value)
{
	to_rt = 0;

	//Compression case
	//Do we have to increase the bit_size?
	if (compress_action == 0 ) {
		if ( next_code == setted_dictionary_size  ) //The next code match with the dictionary size
		{
		return setted_dictionary_size;
		}
		if ( next_code == get_end_bit()   ) //The next code match with the bit size
		{
		to_rt = next_code;
		}
	
	prefisso = index;
	//is the 1st time?
	if ( dictionary[prefisso].next == NULL ){
	dictionary[prefisso].next = &dictionary[next_code];
	}
	else
	{
	prefisso = write_pointer_index;
	int spc = ( ( (float)value / (float)((float)255/(float)search_points) ));
	if ( spc == search_points)
	spc--;
	while( dictionary[prefisso].sp[spc] != NULL )
		prefisso = dictionary[prefisso].sp[spc]->code;
	dictionary[prefisso].sp[spc] = &dictionary[next_code];
	}
	}
	
	dictionary[next_code].value = value;
	dictionary[next_code].index = index;
	dictionary[next_code].code = next_code;
	next_code++;
	return to_rt;
}
