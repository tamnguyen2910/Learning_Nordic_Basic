#ifndef BH1750_READER_H_
#define BH1750_READER_H_

#include <stdint.h>

int bh1750_reader_init(void);
int bh1750_reader_read_lux(int32_t *lux_fixed);

#endif /* BH1750_READER_H_ */
