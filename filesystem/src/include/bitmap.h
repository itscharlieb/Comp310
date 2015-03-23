/**
* @author Charlie Bloomfield
*/

typedef struct {
	byte* bytes;
} Bitmap;

//@pre size must be a multiple of 8
Bitmap* init_bitmap(unsigned int size){
	Bitmap* bitmap = (Bitmap*)malloc(sizeof(Bitmap));

	int numBytes = 
	bitmap->bytes = (byte*)malloc
}

void set_bit(Bitmap b, unsigned int bitNumber);
void clear_bit(Bitmap b, unsigned int bitNumber);
int get_bit(Bitmap b, unsigned int bitNumber);