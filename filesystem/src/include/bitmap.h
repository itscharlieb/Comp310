/**
* @author Charlie Bloomfield
*/

typedef struct {
	byte* bits;
	unsigned int size;
} Bitmap;

//@pre size must be a multiple of 8
Bitmap* init_bitmap(unsigned int size);

void set_bit(Bitmap b, unsigned int bitNumber);
void clear_bit(Bitmap b, unsigned int bitNumber);
int get_bit(Bitmap b, unsigned int bitNumber);