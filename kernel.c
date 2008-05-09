struct machine;
struct word;
typedef struct machine *(*activation_t)(struct word *, struct machine *);
struct word {
    activation_t activate;
};
struct stack {
    struct word *top;
    struct stack *rest;
};
struct machine {
    struct stack *call;
    struct stack *data;
};
struct word *pop(struct stack **stack) {
    struct word *top = (*stack)->top;
    *stack = (*stack)->rest;
    return top;
}
struct stack stack_memory[100];
int stack_index;
struct stack *nil = &stack_memory[0];
void push(struct stack **stack, struct word *word) {
    struct stack *top = &stack_memory[++stack_index];
    top->top = word;
    top->rest = *stack;
    *stack = top;
}
struct machine *interpret(struct machine *machine) {
    struct word *word;
    while (machine->call != nil) {
        word = pop(&machine->call);
        machine = word->activate(word, machine);
    }
    return machine;
}
struct primitive {
    activation_t activate;
};
struct fixnum {
    activation_t activate;
    int number;
};
struct composition {
    activation_t activate;
    struct stack *stack;
};
struct machine *data_push(struct word *self, struct machine *machine) {
    push(&machine->data, self);
    return machine;
}
struct fixnum fixnum_memory[100];
int fixnum_index = 0;
struct word *fixnum_new(int n) {
    struct fixnum *fixnum = &fixnum_memory[fixnum_index++];
    fixnum->activate = data_push;
    fixnum->number = n;
    return (struct word *)fixnum;
}
int fixnum_pop(struct stack **stack) {
    return ((struct fixnum *)pop(stack))->number;
}
struct machine *add(struct word *self, struct machine *machine) {
    int a, b;
    b = fixnum_pop(&machine->data);
    a = fixnum_pop(&machine->data);
    push(&machine->data, fixnum_new(a + b));
    return machine;
}
struct machine *expand(struct word *self, struct machine *machine) {
    struct composition *composition = (struct composition *)self;
    struct stack *definition = composition->stack;
    while (definition != nil)
        push(&machine->call, pop(&definition));
    return machine;
}
struct fixnum two_ = { data_push, 2 };
struct word *two_word = (struct word *)&two_;
struct fixnum four_ = { data_push, 4 };
struct word *four_word = (struct word *)&four_;
struct primitive add_ = { add };
struct word *add_word = (struct word *)&add_;
struct stack *two_plus_definition;
struct composition two_plus_;
struct word *two_plus_word;
int main() {
    struct machine machine = { nil, nil };
    two_plus_definition = nil;
    push(&two_plus_definition, two_word);
    push(&two_plus_definition, add_word);
    two_plus_.activate = expand;
    two_plus_.stack = two_plus_definition;
    two_plus_word = (struct word *)&two_plus_;
    push(&machine.call, two_plus_word);
    push(&machine.call, add_word);
    push(&machine.call, two_word);
    push(&machine.call, four_word);
    machine = *interpret(&machine);
    return ((struct fixnum *)pop(&machine.data))->number;
}
/* new : $ scan-to postpone new ; scan-to $ define parsing
 * symbols: $ ;
 * parsing words: new postpone
 * normal words: scan-to define parsing
 *
 * for debugging: stop shared-memory
 */
