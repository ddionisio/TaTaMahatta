#ifndef _script_basics_h
#define _script_basics_h

//This holds all the basic variables and functions provided by Script Tease
//Don't include this header, it has no use for you

//
// Variables
//

PROTECTED long FloatCallback(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);
PROTECTED long IntCallback(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);
PROTECTED long StringCallback(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

PUBLIC long VectorCallback(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//file someFile (string)
typedef struct _tFile {
	FILE *fp;
	bool bOpen;
} tFile;

PROTECTED long FileCallback(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//
// Functions
//

//integers

//increment integer inc_i (int)
PROTECTED long inc_iCallback(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//decrement integer dec_i (int)
PROTECTED long dec_iCallback(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//add two integers add_i (int out, int i1, int i2)
//out = i1 + i2
PROTECTED long add_iCallback(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//sub two integers sub_i (int out, int i1, int i2)
//out = i1 - i2
PROTECTED long sub_iCallback(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//mult two integers add_i (int out, int i1, int i2)
//out = i1 * i2
PROTECTED long mult_iCallback(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//div two integers div_i (int out, int i1, int i2)
//out = i1 / i2
PROTECTED long div_iCallback(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//mod two integers mod_i (int out, int i1, int i2)
//out = i1 % i2
PROTECTED long mod_iCallback(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//randomize between two integer random_i (int out, int min, int max)
//out = random [min, max]
PROTECTED long random_iCallback(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//floats

//increment float inc_f (float)
PROTECTED long inc_fCallback(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//decrement float dec_f (float)
PROTECTED long dec_fCallback(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//add two floats add_f (float out, float f1, float f2)
//out = f1 + f2
PROTECTED long add_fCallback(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//sub two floats sub_f (float out, float f1, float f2)
//out = f1 - f2
PROTECTED long sub_fCallback(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//mult two floats mult_f (float out, float f1, float f2)
//out = f1 * f2
PROTECTED long mult_fCallback(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//div two floats div_f (float out, float f1, float f2)
//out = f1 / f2
PROTECTED long div_fCallback(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//sin (float out, float radian)
//out = sin(radian)
PROTECTED long sinCallback(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//cos (float out, float radian)
//out = cos(radian)
PROTECTED long cosCallback(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//tan (float out, float radian)
//out = tan(radian)
PROTECTED long tanCallback(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//asin (float out, float f)
//out = asin(f)
PROTECTED long asinCallback(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//acos (float out, float f)
//out = acos(f)
PROTECTED long acosCallback(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//atan (float out, float f)
//out = atan(f)
PROTECTED long atanCallback(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//sqrt (float out, float f)
//out = sqrt(f)
PROTECTED long sqrtCallback(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//degtorad (float out, float deg)
//out = deg * (PI / 180)
PROTECTED long degtoradCallback(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//radtodeg (float out, float rad)
//out = rad * (180 / PI)
PROTECTED long radtodegCallback(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//randomize between two floats random_f (float out, float min, float max)
//out = random [min, max]
PROTECTED long random_fCallback(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//strings

//concatinate two strings strcat (string out, string str)
PROTECTED long strcatCallback(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//files

//prints integer file_print_int (file, int)
PROTECTED long file_print_intCallback(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//prints float file_print_float (file, float)
PROTECTED long file_print_floatCallback(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

//prints string file_print_string (file, string)
PROTECTED long file_print_stringCallback(void *owner, void *handle, int msg, WPARAM wparam, LPARAM lparam);

#endif