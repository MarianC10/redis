#ifndef _COMMONH_H_
#define _COMMON_H_

size_t k_max_msg = 4096;

void die(const char * msg);
void error(const char *msg);

int32_t read_all(int fd, char *buf, size_t n);
int32_t write_all(int fd, const char *buf, size_t n);

#endif
