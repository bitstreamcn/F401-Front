/*
 * ring_buffer.c
 *
 *  Created on: May 8, 2026
 *      Author: bitstream
 */

#include "ring_buffer.h"

// 内存屏障（ARM DSB）
#if defined(__CC_ARM) || defined(__ARMCC_VERSION)
  #define MEM_BARRIER()  __dsb(0xF)
#elif defined(__GNUC__)
  #define MEM_BARRIER()  __asm__ volatile ("dsb 0xF" ::: "memory")
#elif defined(__ICCARM__)
  #define MEM_BARRIER()  __iar_builtin_DSB()
#else
  #error "Unknown compiler, add memory barrier"
#endif

void rb_init(RingBuffer *rb, uint8_t *buf, uint32_t size) {
	assert(size > 1);
    rb->buffer = buf;
    rb->size = size;
    rb->head = 0;
    rb->tail = 0;
}

uint32_t rb_put(RingBuffer *rb, const uint8_t *data, uint32_t len) {
    uint32_t head = rb->head;
    uint32_t tail = rb->tail;
    uint32_t size = rb->size;
    uint32_t free = (tail + size - head - 1) % size;  // 预留1字节防满

    if (len > free) len = free;
    if (len == 0) return 0;

    uint32_t to_end = size - head;
    if (len <= to_end) {
        memcpy(&rb->buffer[head], data, len);
    } else {
        memcpy(&rb->buffer[head], data, to_end);
        memcpy(&rb->buffer[0], data + to_end, len - to_end);
    }

    MEM_BARRIER();
    rb->head = (head + len) % size;
    return len;
}

uint32_t rb_get(RingBuffer *rb, uint8_t *out, uint32_t max_len) {
    uint32_t head = rb->head;
    uint32_t tail = rb->tail;
    uint32_t size = rb->size;
    uint32_t avail = (head + size - tail) % size;

    if (max_len > avail) max_len = avail;
    if (max_len == 0) return 0;

    uint32_t to_end = size - tail;
    if (max_len <= to_end) {
        memcpy(out, &rb->buffer[tail], max_len);
    } else {
        memcpy(out, &rb->buffer[tail], to_end);
        memcpy(out + to_end, &rb->buffer[0], max_len - to_end);
    }

    MEM_BARRIER();
    rb->tail = (tail + max_len) % size;
    return max_len;
}

uint32_t rb_avail(const RingBuffer *rb) {
    uint32_t head = rb->head;
    uint32_t tail = rb->tail;
    return (head + rb->size - tail) % rb->size;
}

uint32_t rb_free(const RingBuffer *rb) {
    uint32_t head = rb->head;
    uint32_t tail = rb->tail;
    return (tail + rb->size - head - 1) % rb->size;
}

bool rb_is_empty(const RingBuffer *rb) {
    return rb->head == rb->tail;
}

bool rb_is_full(const RingBuffer *rb) {
    return ((rb->head + 1) % rb->size) == rb->tail;
}

// 跳过（丢弃）len 字节的数据，返回实际跳过的字节数
uint32_t rb_advance_tail(RingBuffer *rb, uint32_t len) {
    uint32_t avail = rb_avail(rb);
    if (len > avail)
        len = avail;
    if (len == 0) return 0;

    // 消费者唯一修改 tail，无需关中断
    rb->tail = (rb->tail + len) % rb->size;

    // 内存屏障：确保后续读操作看到的数据在 tail 更新之后（生产者检查空间时不会出错）
    MEM_BARRIER();
    return len;
}

// 丢弃缓冲区中前面的数据，只保留最后 last_bytes 字节
// 如果可读数据不足 last_bytes，则全部保留
void rb_keep_last(RingBuffer *rb, uint32_t last_bytes) {
    uint32_t avail = rb_avail(rb);
    if (avail <= last_bytes) return;   // 数据不够，保留所有

    uint32_t discard = avail - last_bytes;
    rb->tail = (rb->tail + discard) % rb->size;
    MEM_BARRIER();
}

void rb_clear(RingBuffer *rb)
{
    rb->head = 0;
    rb->tail = 0;
}
