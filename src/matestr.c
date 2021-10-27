/*
 * matestr
 *
 * Last revision: 24 Oct 2021
 * Author(s): Amchik
 *
 * Release notes:
 *   - Change wchar (char) to matechar (see matestr.h)
 *     (see matechar and __matecharlen)
 *   - matechar set to char
 *
 * Release notes (12 Oct 2021):
 *   - Change code style
 *   - Change char to wchar
 *   - Add matestr_find, matestr_finds
 *   - Add matestr_take
 */

#include <stdlib.h>
#include <wchar.h>
#include <string.h>

#include "./include/matestr.h"

/*
 * Creates new instance of matestr.
 */
matestr
matestralloc(size_t n) {
	matestr _new;

	_new.cstr = malloc(n * sizeof(matechar));
	_new.length = 0;
	_new.__allocated = n;
	_new.cstr[0] = '\0';

	return(_new);
}

/*
 * Creates new instance of matestr.
 * Calculates length of cstr, if cstr == 0,
 * passes empty allocated string.
 * If len == 0 it will be ignored
 */
matestr
matestr_new(const matechar* cstr, size_t len) {
	matestr _new;

	if (cstr == 0) {
		_new.cstr = malloc(128 * sizeof(matechar));
		_new.length = 0;
		_new.__allocated = 128;
		_new.cstr[0] = L'\0';
	} else {
		_new.length = len == 0 ? __matecharlen(cstr) : len;
		_new.__allocated = _new.length + 1;
		_new.cstr = malloc(_new.__allocated * sizeof(matechar));
		memcpy(_new.cstr, cstr, _new.__allocated * sizeof(matechar));
		_new.cstr[_new.length] = L'\0';
	}

	return(_new);
}
/*
 * Normalizes cstr in matestr
 */
void
matestr_normalize(matestr* self) {
	self->cstr[self->length] = '\0';

	return;
}
/*
 * Optimizes memory in matestr
 */
void
matestr_optimize(matestr* self) {
	if (self->length + 1 == self->__allocated) {
		return;
	}

	self->__allocated = self->length + 1;
	self->cstr = realloc(self->cstr, self->__allocated * sizeof(matechar));

	return;
}
/*
 * Copy string to matestr
 */
void
matestr_append(matestr* self, const matechar* cstr, size_t len) {
	u_int32_t cstr_len;

	cstr_len = len == 0 ? __matecharlen(cstr) : len;
	if (self->__allocated <= (cstr_len + self->length)) {
		self->__allocated += cstr_len;
		self->cstr = realloc(self->cstr, self->__allocated * sizeof(matechar));
	}

	memcpy(self->cstr + self->length, cstr, cstr_len);
	self->length += cstr_len;
	self->cstr[self->length] = '\0'; /* normalize */

	return;
}
/*
 * Allocs matestr for n bytes
 */
void
matestr_appendm(matestr* self, size_t n) {
	self->__allocated += n;
	self->cstr = realloc(self->cstr, self->__allocated * sizeof(matechar));

	return;
}
void
matestr_replacec(matestr* self, matechar oldc, matechar newc) {
	u_int32_t i;
	for (i = 0; i < self->length; i++) {
		if (self->cstr[i] == oldc) {
			self->cstr[i] = newc;
		}
	}
}
u_int32_t
matestr_find(matestr* self, matechar what, u_int32_t start) {
	u_int32_t i;

	for (i = start; i < self->length; i++) {
		if (self->cstr[i] == what) {
			return(i);
		}
	}
	return(-1);
}
u_int32_t
matestr_finds(matestr* self, matechar *what, u_int32_t start) {
	u_int32_t i, p, len;

	len = __matecharlen(what) - 1;
	p = 0;
	for (i = start; i < self->length; i++) {
		if (self->cstr[i] == what[p]) {
			p++;
			if (p == len) {
				return(i - p + 1);
			}
		} else if (p != 0) {
			p = 0;
		}
	}
	return(-1);
}

matestr
matestr_take(matestr *self, u_int32_t start, u_int32_t end) {
	matestr answer;

	answer.__allocated = 0;
	answer.length = 0;

	if (end < start || start > self->length) {
		return(answer);
	}

	answer = matestr_new(self->cstr + start, end - start);

	return(answer);
}

