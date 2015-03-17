/**
* @author Charlie Bloomfield
*/

typedef struct {
	//something
} Bitmap;

Bitmap* init_bitmap(unsigned int size);
void set_bit(Bitmap b, unsigned int bitNumber);
void clear_bit(Bitmap b, unsigned int bitNumber);
int get_bit(Bitmap b, unsigned int bitNumber);