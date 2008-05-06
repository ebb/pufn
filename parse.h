/* public domain */

object_t parse_file(machine_t *, const char *);
int parse_is_parsing_word(object_t);
machine_t *parse_quote(machine_t *);
machine_t *parse_definition(machine_t *);
object_t parse_until_word(machine_t *, object_t);
void parse_define(object_t, object_t);
int parse_still_parsing();
int parse_still_parsing_line();
int parse_column_is_blank();
void parse_skip_blank();
void parse_skip_word();
void parse_next_line();
int parse_token(object_t *);
int parse_scan(machine_t *, object_t *);