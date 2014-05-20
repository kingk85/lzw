#include <iostream>
#include "C_c.cpp"
#include "fstream"

using namespace std;

int main (int argc, char ** argv) {


unsigned char write_buffer[100000];

//New ulzw Object
ulzw compr;




cout<<"Compression test test.txt will be compressed as test.ulzw "<<endl;
//Compress test
ifstream file_in;
ifstream file_in2;
file_in.open("test.txt",ios::in | ios::binary );

ofstream file_out;
ofstream file_out2;
file_out.open("test.ulzw", ios::out|ios::binary);

int flag = 0;
int chr = 0;
int recv = 0;

while ( (chr = file_in.get()) != -1 )
{

recv = compr.compress_chr( (unsigned char)chr, write_buffer, flag);
if ( recv > 0 )
	file_out.write( (char*)write_buffer, recv );
flag = 1;
}
flag = 2;
recv = compr.compress_chr( 0, write_buffer, flag);
if ( recv > 0 )
file_out.write( (char*)write_buffer, recv );

file_in.close();
file_out.flush();
file_out.close();

cout<<"Done!"<<endl;


cout<<"test.ulzw will be decompressed as test_decompressed.txt "<<endl;
//Decompress test
file_in2.open("test.ulzw",ios::in | ios::binary );
file_out2.open("test_decompressed.txt", ios::out|ios::binary);
flag = 0;
long long processed = 0;
long long total_recv = 0;
while ( (chr = file_in2.get()) != -1 )
{
processed++;
recv = compr.decompress_chr( (unsigned char)chr, write_buffer, flag);
if ( recv > 0 ){
file_out2.write( (char*)write_buffer, recv );
file_out2.flush();
}
total_recv += recv;

if ( processed == 1)	
flag = 1;
if ( processed == 2)
flag = 2;
}
file_in.close();
file_out.close();

cout<<"Done!"<<endl;

//Exit
return 0;
}
