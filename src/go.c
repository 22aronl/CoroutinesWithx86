#include "go.h"
#include <stdlib.h>
#include <stdio.h>

static void missing(const char *file, int line)
{
    printf("*** missing code at %s:%d\n", file, line);
    exit(-1);
}

#define MISSING() missing(__FILE__, __LINE__)

struct Routine;
typedef struct Routine Routine;

#define STACK_ENTRIES (8192 / sizeof(uint64_t))

typedef struct Queue
{
    struct Routine *head;
    struct Routine *tail;
} Queue;

struct Channel
{
    Value value;
    Queue *q;
    bool receiving;
};

struct Routine
{
    uint64_t saved_rsp;
    // Each c-routine needs its private stack
    uint64_t stack[STACK_ENTRIES];
    Routine *next;
    Channel ch;
    Func func;
    bool is_started;
    Value send_value;
};

/////////

static void addQ(Queue *q, Routine *r)
{
    r->next = 0;
    if (q->tail != 0)
    {
        q->tail->next = r;
    }
    q->tail = r;

    if (q->head == 0)
    {
        q->head = r;
    }
}

static Routine *removeQ(Queue *q)
{
    Routine *r = q->head;
    if (r != 0)
    {
        q->head = r->next;
        if (q->tail == r)
        {
            q->tail = 0;
        }
    }
    return r;
}

///////////////////////////////////////////////////

static Routine *current_ = 0;
static Queue ready = {0, 0};

extern void save_rip(Routine *r, Routine *to);

void save_rsp(Routine *r)
{
    __asm__ volatile("movq %rsp, %0"
                     : "=r"(r));
}

void switch_to(Routine *from, Routine *to)
{
    if (to == 0)
    {
        __asm__ volatile(
            "movq $60, %rax\n"
            "syscall"); // This feels very wrong
    }
    else
    {
        save_rip(from, to);
    }
}

void delete_this_function_when_you_are_done()
{
    addQ(&ready, 0);
    (void)removeQ(&ready);
}

Routine **current()
{
    if (current_ == 0)
    {
        current_ = (Routine *)calloc(sizeof(Routine), 1);
    }
    return &current_;
}

/* OSX prepends _ in front of external symbols */
Routine **_current()
{
    return current();
}

Channel *go(Func func)
{
    Routine *r = malloc(sizeof(Routine));
    r->saved_rsp = (uint64_t)r->stack + STACK_ENTRIES; // TODO: My ordering may be wrong or somethign
    r->ch = *channel();
    r->func = func;
    r->is_started = false;
    addQ(&ready, r);
    return &r->ch;
}

Channel *me()
{
    return &(*current())->ch;
}

void again()
{
    MISSING();
}

Channel *channel()
{
    Channel *ch = malloc(sizeof(Channel));
    ch->q = malloc(sizeof(Queue));
    ch->q->head = 0;
    ch->q->tail = 0;
    ch->receiving = false;
    return ch;
}

Value receive(Channel *ch)
{
    if (ch->q->head == 0 || ch->receiving)
    {
        addQ(ch->q, *current());
        Routine *old_r = *current();
        *current() = removeQ(&ready);
        switch_to(old_r, *current());
        return ch->value;
    }
    else
    {
        Routine *r = removeQ(ch->q);
        addQ(&ready, r);
        return r->send_value;
    }
}

void send(Channel *ch, Value v)
{
    MISSING();
}
