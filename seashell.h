void update_usage(PTable &table);
int check_built_ins(std::vector<std::string> args, PTable &table);
template <class Container>
void parse_input(const std::string& str, Container& cont);
int execute_command(std::vector<std::string> args, PTable &table);
void cppshell();
int main();