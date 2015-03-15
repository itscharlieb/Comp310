/**
* @author Charlie Bloomfield
*/

typedef struct {
	//something
} bitmap;

bitmap* init_bitmap(unsigned int size);
void set_bit(bitmap b, unsigned int bitNumber);
void clear_bit(bitmap b, unsigned int bitNumber);
int get_bit(bitmap b, unsigned int bitNumber);