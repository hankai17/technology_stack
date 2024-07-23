#include <stdio.h>

struct mpscq_node_t
{
	mpscq_node_t* volatile  next;
};

struct mpscq_t
{
	mpscq_node_t* volatile  head;
	mpscq_node_t*           tail;
	mpscq_node_t            stub;
};

#define MPSCQ_STATIC_INIT(self) {&self.stub, &self.stub, {0}}

void mpscq_create(mpscq_t* self)
{
	self->head = &self->stub;
	self->tail = &self->stub;
	self->stub.next = 0;
}

void mpscq_push(mpscq_t* self, mpscq_node_t* n)
{
	n->next = 0;
	mpscq_node_t* prev = XCHG(&self->head, n);
	//(*)
	prev->next = n;
}

mpscq_node_t* mpscq_pop(mpscq_t* self)
{
	mpscq_node_t* tail = self->tail;
	mpscq_node_t* next = tail->next;
	if (tail == &self->stub) {
		if (0 == next)
			return 0;

		self->tail = next;
		tail = next;
		next = next->next;
	}

	if (next) {
		self->tail = next;
		return tail;
	}

	mpscq_node_t* head = self->head;
	if (tail != head)
		return 0;

	mpscq_push(self, &self->stub);
	next = tail->next;
	if (next) {
		self->tail = next;
		return tail;
	}

	return 0;
} 

int main() {

    return 0;
}

// https://www.1024cores.net/home/lock-free-algorithms/queues/intrusive-mpsc-node-based-queue
