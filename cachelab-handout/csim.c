//liheng
//18-04-01

#include "cachelab.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

typedef struct lru{
    long long tag_id;
    struct lru* next;
}cache_lru;

int hit_sum = 0;
int miss_sum = 0;
int eviction_sum = 0;
int set_num = 0;
int line_num = 0;
int block_size = 0;
long long tag = 0;
bool help_flag = false;
bool trace_flag = false;
long long* cache;
bool* used;
long long set_bin = 0;
long long tag_bin = 0;

cache_lru* head[256];

void update_lru(int x,long long y)
{
    cache_lru* p = head[x];
    cache_lru* pre =NULL;
    while (p != NULL){
        if (p->tag_id == y){
            if (pre == NULL) break;
            pre->next = p->next;
            p->next = head[x];
            head[x] = p;
            return;
        }
        pre = p;
        p = p->next;
    }
}

long long  eviction_lru(int x,long long y)
{
    long long ret=0;
    cache_lru* p = head[x];
    cache_lru* pre = NULL;
    while (p->next != NULL){
        pre = p;
        p = p->next;
    }
    if (pre==NULL){
        ret = p->tag_id;
        p->tag_id = y;
        return ret;
    }else
    {
        ret = p->tag_id;
        pre->next = p->next;
        p->next = head[x];
        p->tag_id  = y;
        head[x] = p;
        return ret;
    }
}

void insert_lru(int x,long long y)
{
    cache_lru* p = (cache_lru*) malloc(sizeof(cache_lru));
    p->tag_id = y;
    p->next = head[x];
    head[x] = p;
    return;
}

void output_hit(char mode,long long address,int offset)
{
    printf("%c %lld %d hit\n",mode,address,offset);
}

void output_miss(char mode,long long address,int offset)
{
    printf("%c %lld %d miss\n",mode,address,offset);
}

void output_hit_hit(char mode,long long address,int offset)
{
    printf("%c %lld %d hit hit\n",mode,address,offset);
}

void output_miss_hit(char mode,long long address,int offset)
{
    printf("%c %lld %d miss hit\n",mode,address,offset);
}

void output_miss_eviction(char mode,long long address,int offset)
{
    printf("%c %lld %d miss eviction\n",mode,address,offset);
}

void output_miss_eviction_hit(char mode,long long address,int offset)
{
    printf("%c %lld %d miss eviction hit\n",mode,address,offset);
}

void cache_sim(char* file_name)
{
    char mode;
    long long address;
    int offset;
    long long set_value;
    long long tag_value;
    FILE* fp = fopen(file_name,"r");
    while (fscanf(fp," %c %llx,%x\n",&mode,&address,&offset)!=EOF){
        if (mode=='L'||mode=='S'){
            set_value = (address & set_bin)>>block_size;
            tag_value = address & tag_bin;
            bool find_flag = false;
            bool empty_flag = true;
            int empty_position = -1;
            for (int i=0;i!=line_num;i++){
                if (empty_flag && used[set_value*line_num+i] == 0){
                    empty_flag = false;
                    empty_position = i;
                }
                if ((cache[set_value*line_num+i]&tag_bin) == tag_value && used[set_value*line_num+i]){
                    find_flag = true;
                    ++hit_sum;
                    update_lru(set_value,tag_value);
                    output_hit(mode,address,offset);
                    break;
                }
            }
            if (!find_flag){
                ++miss_sum;
                if (empty_position == -1){
                    ++eviction_sum;
                    long long replace = eviction_lru(set_value,tag_value);
                    for (int i=0;i!=line_num;i++){
                        if ((cache[set_value*line_num+i]&tag_bin) == replace && used[set_value*line_num+i] == 1){
                            cache[set_value*line_num+i] = (cache[set_value*line_num+i] & (~tag_bin)) | tag_value;
                            output_miss_eviction(mode,address,offset);
                            break;
                        }
                    }
                }else{
                    cache[set_value*line_num+empty_position] = cache[set_value*line_num+empty_position] | tag_value;
                    used[set_value*line_num+empty_position] = 1;
                    insert_lru(set_value,tag_value);
                    output_miss(mode,address,offset);
                }
            }
        }
        if (mode == 'M'){
            set_value = (address & set_bin)>>block_size;
            tag_value = address & tag_bin;
            bool find_flag = false;
            bool empty_flag = true;
            int empty_position = -1;
            for (int i=0;i!=line_num;i++){
                if (empty_flag && used[set_value*line_num+i] == 0){
                    empty_flag = false;
                    empty_position = i;
                }
                if ((cache[set_value*line_num+i]&tag_bin) == tag_value && used[set_value*line_num+i]){
                    find_flag = true;
                    ++hit_sum;
                    update_lru(set_value,tag_value);
                    output_hit_hit(mode,address,offset);
                    break;
                }
            }
            if (!find_flag){
                ++miss_sum;
                if (empty_position == -1){
                    ++eviction_sum;
                    long long replace = eviction_lru(set_value,tag_value);
                    for (int i=0;i!=line_num;i++){
                        if ((cache[set_value*line_num+i]&tag_bin) == replace && used[set_value*line_num+i] == 1){
                            cache[set_value*line_num+i] = (cache[set_value*line_num+i] & (~tag_bin)) | tag_value;
                            output_miss_eviction_hit(mode,address,offset);
                            break;
                        }
                    }
                }else{
                    cache[set_value*line_num+empty_position] = cache[set_value*line_num+empty_position] | tag_value;
                    used[set_value*line_num+empty_position] = 1;
                    insert_lru(set_value,tag_value);
                    output_miss_hit(mode,address,offset);
                }
            }
            set_value = (address & set_bin)>>block_size;
            tag_value = address & tag_bin;
            find_flag = false;
            empty_flag = true;
            empty_position = -1;
            for (int i=0;i!=line_num;i++){
                if (empty_flag && used[set_value*line_num+i] == 0){
                    empty_flag = false;
                    empty_position = i;
                }
                if ((cache[set_value*line_num+i]&tag_bin) == tag_value && used[set_value*line_num+i]){
                    find_flag = true;
                    ++hit_sum;
                    update_lru(set_value,tag_value);
                    break;
                }
            }
            if (!find_flag){
                ++miss_sum;
                if (empty_position == -1){
                    ++eviction_sum;
                    long long replace = eviction_lru(set_value,tag_value);
                    for (int i=0;i!=line_num;i++){
                        if ((cache[set_value*line_num+i]&tag_bin) == replace && used[set_value*line_num+i] == 1){
                            cache[set_value*line_num+i] = (cache[set_value*line_num+i] & (~tag_bin)) | tag_value;
                            break;
                        }
                    }
                }else{
                    cache[set_value*line_num+empty_position] = cache[set_value*line_num+empty_position] | tag_value;
                    used[set_value*line_num+empty_position] = 1;
                    insert_lru(set_value,tag_value);
                }
            }

        }
    }
}

void init_cache()
{
    set_bin = (((0x01<<set_num)-1))<<block_size;
    tag_bin = ((0x01<<(64-set_num-block_size))-1)<<(set_num+block_size);
    int set_start = 0;
    int set_step = 0x01<<block_size;
    cache = (long long *)malloc(sizeof(long long)*(1<<(set_num))*(line_num));
    used = (bool*)malloc(sizeof(bool)*(1<<(set_num))*(line_num));
    memset(used,0,sizeof(bool)*(1<<(set_num))*(line_num));
    memset(cache,0,sizeof(long long)*(1<<(set_num))*(line_num));
    memset(head,0,sizeof(void *)*256);
    for (int i=0;i!=1<<(set_num);i++){
        for (int j=0;j!=line_num;j++){
            cache[i*line_num+j] = set_start;
        }
        set_start+=set_step;
    }
}

int main(int argc,char* argv[])
{
    char* file_name;
    for (int i=1;i<argc;i++){
        if (strcmp(argv[i],"-s")==0 ||strcmp(argv[i],"-S")==0){
            set_num = atoi(argv[i+1]);
            i++;
        }else
        if (strcmp(argv[i],"-e")==0||strcmp(argv[i],"-E")==0){
            line_num = atoi(argv[i+1]);
            i++;
        }else
        if (strcmp(argv[i],"-b")==0||strcmp(argv[i],"-B")==0){
            block_size = atoi(argv[i+1]);
            i++;
        }else
        if (strcmp(argv[i],"-h")==0||strcmp(argv[i],"-H")==0){
            help_flag = true;
        }else
        if (strcmp(argv[i],"-v")==0||strcmp(argv[i],"-V")==0){
            trace_flag = true;
        }else
        if (strcmp(argv[i],"-t")==0||strcmp(argv[i],"-T")==0){
            file_name = argv[i+1];
            i++;
        }

    }
    init_cache();
    cache_sim(file_name);
    printSummary(hit_sum, miss_sum, eviction_sum);
    return 0;
}

