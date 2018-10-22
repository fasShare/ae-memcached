#include <stdio.h>
#include <time.h>
#include <string.h>

#include "assoc.h"
#include "mem_cache.h"
#include "mem_cache.h"
#include "logger.h"

#define DEFAULT_KEY_SIZE 30
#define DEFAULT_KEY_PAD 256
#define DEFAULT_VALUE_PAD DEFAULT_KEY_PAD

extern mem_cache_ptr mem_cache;

static inline uint64_t time_now(void) {
    struct timespec time;
    clock_gettime(CLOCK_MONOTONIC, &time);
    return ((uint64_t) time.tv_sec) * 1000000000 + time.tv_nsec;
}

void test_assoc_performance(size_t items, size_t value_size) {
    if (items == 0) {
        LOG_WARN("Items less than zero!\n");
        return;
    }

    size_t chunk_size = value_size + 150;
    mem_cache = mem_cache_create(chunk_size, 
                                    1.25, 
                                    40, 
                                    chunk_size * 1024, 
                                    512 * DEFAULT_POWER_BLOCK * 6, 
                                    1);

    if (!mem_cache) {
        LOG_ERROR("Create mem_cache failed!\n");
        return;
    }
    char *key_prefix = (char *)malloc(DEFAULT_KEY_SIZE),
         *value_prefix = (char *)malloc(value_size);
    char *key = (char *)malloc(DEFAULT_KEY_PAD),
         *value = (char *)malloc(value_size + DEFAULT_VALUE_PAD);
    if (!key_prefix || !value_prefix || !key || !value) {
        LOG_ERROR("Malloc key or value space failed!\n");
        //key_prefix ? free(key_prefix) :;
        //value_prefix ? free(value_prefix) :;
        //key ? free(key) :;
        //value ? free(value) :;
        return;
    }
    
    memset(key_prefix, 'K', DEFAULT_KEY_SIZE - 1);
    memset(value_prefix, 'V', value_size - 1);
    key_prefix[DEFAULT_KEY_SIZE - 1] = 0;
    value_prefix[value_size - 1] = 0;

    // 初始化hash表
    assoc_init();

    uint64_t start_set = time_now() / 1000;
    size_t i = 0;
    for (; i < items; ++i) {
        int key_size = sprintf(key, "%s%d", key_prefix, i);
        int value_size = sprintf(value, "%s%s", key, value_prefix);
        item *it = item_alloc(key, 0, 0, value_size);
        if (it == NULL) {
            LOG_ERROR_F1("Insert key %s error!\n", key);
            break;
        }
        assoc_insert(key, it);
    }
    uint64_t end_set = time_now() / 1000;
    size_t j = 0;
    for (; j < i; ++j) {
        int key_size = sprintf(key, "%s%d", key_prefix, j);
        item *it = assoc_find(key);
        if (it == NULL) {
            LOG_ERROR_F1("Find key %s error!\n", key);
            break;
        }
    }
    uint64_t end_get = time_now() / 1000;
    
    int64_t insert_time = end_set - start_set;
    int64_t find_time = end_get - end_set;
    int64_t insert_per_sec = i * 1000000 / insert_time;
    int64_t find_per_sec = i * 1000000/ find_time;

    printf("datalen:%u times:%u insert_time:%lld insert_per_sec:%lld find_time:%lld find_per_sec:%lld\n", 
            value_size, items, insert_time, insert_per_sec, find_time, find_per_sec);
}

int main (int argc, char **argv) {
    if (argc < 3) {
        LOG_WARN("Usage:exec datalen reqs\n");
        return -1;
    }
    int reqs = atoi(argv[2]);
    int datalen = atoi(argv[1]);
    if (reqs < 0 || datalen < 0) {
        LOG_ERROR("error:reqs < 0 || datalen < 0\n");
        return -1;
    }
    test_assoc_performance(reqs, datalen);
    return 0;
}
