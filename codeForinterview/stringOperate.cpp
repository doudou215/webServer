#include <iostream>
#include <assert.h>
#include <string.h>
using std::cout;
using std::endl;

char *strcpy(char *dst, const char *src) {
	char *ret = dst;
	assert(dst != NULL);
	assert(src != NULL);
	cout<<"mystrcpy"<<endl;
	//memmove(dst, src, strlen(src) + 1);
	int len = strlen(src) + 1;
	if (src <= dst && dst <= src + len - 1) {
		src = src + len - 1;
		dst = dst + len - 1;
		while (len) {
			*dst-- = *src--;
			len--;
		}
	}
	else {
		while (*src) {
			*dst++ = *src++;
		}
		*dst = '\0';
	}
	return ret;
}

char *strcat(char *dst, const char *src) {
	char *ret = dst;
	assert(dst != NULL);
	assert(src != NULL);
	cout<<"my"<<sizeof(src)<<endl;	
	while (*dst) {
		dst++;
	}

	while (*src)
		*dst++ = *src++;
	*dst = NULL;
	return ret;
}

int main() {
	char dst[10];
	char test[100] = "abcd1234";
	char str[10];
	cout<<strcpy(test + 8, test);
	return 0;
}
