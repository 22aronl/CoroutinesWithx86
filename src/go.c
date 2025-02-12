#include "go.h"
#include <stdlib.h>
#include <stdio.h>


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
    bool is_empty;
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

extern void save_rip(Routine *r, uint64_t rsp);
extern void new_function(Routine *from, uint64_t to, Func func);
extern void save_rip_early(Routine *r);

/**
 * This method switches from the current routine "from" to the new routine "to"
 * It assumes that the rip of the to routine is ontop of the stack and will store from rip 
 * onto the from stack
*/
void switch_to(Routine *from, Routine *to)
{
    save_rip(from, to->saved_rsp);
}

/**
 * This switches from this routine "from" to the next routine in the ready q
 * If there are no more routines in ready, it will exit
 */
void switch_from(Routine *from)
{
    Routine *to = removeQ(&ready);
    if (to == 0)
    {
        exit(0);
    }
    else
    {
        *current() = to;
        if (to->is_started) //if the stack has not been instantiated yet
        {
            to->is_started = false;
	    to->saved_rsp = (uint64_t)to->stack + STACK_ENTRIES*sizeof(uint64_t)-8;
            new_function(from, to->saved_rsp, to->func);
        }
        else
        {
            switch_to(from, to);
        }
    }
}

/**
 * switches from current to next ready Q
 */
void next_ready_function()
{
    switch_from(*current());
}

/**
 * Creates a c-routine
 */
Channel *go(Func func)
{
    Routine *r = malloc(sizeof(Routine));
    r->stack[STACK_ENTRIES-1]=(uint64_t)(&next_ready_function);
    r->saved_rsp = (uint64_t)r->stack + STACK_ENTRIES*sizeof(uint64_t)-8;
    r->ch = *channel();
    r->func = func;
    r->is_started = true;
    r->is_empty = true;
    addQ(&ready, r);
    return &r->ch;
}

Channel *me()
{
    return &(*current())->ch;
}

/**
 * Runs the method again
 */
void again()
{
    //special expection if current is main
    if((*current())->func == 0)
    {
	extern int main();
	exit(main());
    }
    (*current())->is_started = true;
    (*current())->saved_rsp = (uint64_t)(*current())->stack + STACK_ENTRIES*sizeof(uint64_t)-8;
    addQ(&ready, *current());
    next_ready_function();
}

/**
 * creates a new channel
 */
Channel *channel()
{
    Channel *ch = malloc(sizeof(Channel));
    ch->q = malloc(sizeof(Queue));
    ch->q->head = 0;
    ch->q->tail = 0;
    ch->receiving = false;
    return ch;
}

__attribute__((constructor)) void init()
{
    (*current())->ch = *channel();
}




Value receive(Channel *ch)
{
    if (ch->q->head == 0 || ch->receiving)
    {
        addQ(ch->q, *current());
        ch->receiving = true;
        switch_from(*current());
        return (*current())->send_value;
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
    if (ch->q->head == 0 || !ch->receiving)
    {
	(*current())->send_value = v;
        addQ(ch->q, *current());
        ch->receiving = false;
        switch_from(*current());
    }
    else
    {
        Routine *r = removeQ(ch->q);
        r->send_value = v;
        addQ(&ready, r);
    }
}
