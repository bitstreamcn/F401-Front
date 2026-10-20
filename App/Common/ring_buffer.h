/*
 * ring_buffer.h
 *
 *  Created on: May 8, 2026
 *      Author: bitstream
 */
#ifdef __cplusplus
extern "C" {
#endif

#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/*
 * 环形缓冲区
 * 单生产者，单消费者无锁设计
 */

// 环形缓冲区控制结构
typedef struct {
    uint8_t *buffer;            // 指向外部提供的内存
    uint32_t size;              // 缓冲区总字节数
    volatile uint32_t head;     // 生产者写入索引
    volatile uint32_t tail;     // 消费者读取索引
} RingBuffer;

// 初始化：绑定内存和大小
void rb_init(RingBuffer *rb, uint8_t *buf, uint32_t size);

// 生产者写入（中断调用）
uint32_t rb_put(RingBuffer *rb, const uint8_t *data, uint32_t len);

// 消费者读取
uint32_t rb_get(RingBuffer *rb, uint8_t *out, uint32_t max_len);

// 跳过（丢弃）len 字节的数据，返回实际跳过的字节数
uint32_t rb_advance_tail(RingBuffer *rb, uint32_t len);

// 丢弃缓冲区中前面的数据，只保留最后 last_bytes 字节
// 如果可读数据不足 last_bytes，则全部保留
void rb_keep_last(RingBuffer *rb, uint32_t last_bytes);

// 查询可用读/写空间
uint32_t rb_avail(const RingBuffer *rb);
uint32_t rb_free(const RingBuffer *rb);
bool rb_is_empty(const RingBuffer *rb);
bool rb_is_full(const RingBuffer *rb);

void rb_clear(RingBuffer *rb);

#endif

#ifdef __cplusplus
}
#endif
