#pragma once

#ifndef __STDC_WANT_SECURE_LIB__

// definiujemy bezpieczne wersje f-cji CRT

inline errno_t __cdecl fopen_s(FILE ** f, const char * filename, const char * mode) {
	*f = fopen(filename, mode);
	return *f != 0 ? 0 : EINVAL;
}

inline errno_t _itoa_s(int value,  char *buffer,  size_t sizeInCharacters, int radix) {
	_itoa(value, buffer, radix);
	return errno;
}

errno_t strncpy_s(char *strDest, size_t sizeInBytes, const char *strSource, size_t count) {
	strncpy(strDest, strSource, count);
	return 0;
}


#endif