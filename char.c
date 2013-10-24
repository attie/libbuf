#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "internal.h"

EXPORT int buf_putc(buf_t *buf, int c) {
	buf_chunk_t **p;
	buf_chunk_t *ch;
	size_t len;

	if (!buf) {
		errno = EINVAL;
		return -1;
	}

	pthread_mutex_lock(&(buf->mutex));
	for (p = &(buf->head); p && *p; p = &((*p)->next));
	if (p && (*p) && (*p)->size > (*p)->pos + (*p)->len) {
		(*p)->data[(*p)->pos] = c;
		(*p)->pos++;
		(*p)->len++;
		pthread_mutex_unlock(&(buf->mutex));
		return c;
	}
	pthread_mutex_unlock(&(buf->mutex));

	len = getpagesize();
	if (len > 4096) len = 4096;
	if ((ch = malloc(sizeof(*ch) + len)) == NULL) {
		errno = ENOMEM;
		return EOF;
	}

	ch->next = NULL;
	ch->size = len;
	ch->len = 1;
	ch->pos = 0;
	ch->data[ch->pos] = c;

	pthread_mutex_lock(&(buf->mutex));
	for (p = &(buf->head); p && *p; p = &((*p)->next));
	if (p) *p = ch;
	pthread_mutex_unlock(&(buf->mutex));
	
	if (!p) {
		free(ch);
		errno = EFAULT;
		return -1;
	}

	return c;
}

EXPORT int buf_getc(buf_t *buf) {
	int ret;

	if (!buf) {
		errno = EINVAL;
		return -1;
	}

	pthread_mutex_lock(&(buf->mutex));
	ret = EOF;
	while (buf->head && (ret == EOF || buf->head->len == 0)) {
		if (buf->head->len == 0) {
			buf_chunk_t *c;
			c = buf->head;
			buf->head = c->next;
			free(c);
		} else {
			ret = buf->head->data[buf->head->pos];
			buf->head->pos++;
			buf->head->len--;
		}
	}
	pthread_mutex_unlock(&(buf->mutex));

	return ret;
}
