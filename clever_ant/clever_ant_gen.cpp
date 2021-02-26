#include <string>
#include <utility>
#include <vector>
#include <map>
#include <ctime>
#include <regex>
#include <limits>
#include "clever_ant_gen.h"

class Mutate final {
    int do_funcs;
    int do_states;

public:
    struct Base {
        int steps;
        int states;
        const std::string sequence;

        Base(int steps, int states, std::string sequence)
                : steps(steps), states(states), sequence(std::move(sequence)) {}
    };

    explicit Mutate(int funcs, int states) : do_funcs(funcs), do_states(states) {}

    [[nodiscard]] std::string next() const {
        char buf[20];
        int eat_state = rand() % do_states + 0;
        int not_state = rand() % do_states + 0;
        int fn = rand() % do_funcs + 0;
        snprintf(buf, sizeof(buf), "3.%d.%d.%d", eat_state, fn, not_state);
        return buf;
    }

    [[nodiscard]] std::string mutate(const std::string &sequence) const {
        std::string result;
        const std::regex re(R"(:)");
        int place = rand() % do_states + 0;

        std::vector<std::string> tokenized = tokenize(sequence, re);
        for (int i = tokenized.size(); i < do_states; ++i) {
            tokenized.emplace_back(next());
        }
        tokenized[place] = next();
        join(tokenized, ':', result);
        return result;
    }

    static std::vector<std::string> tokenize(const std::string &str, const std::regex &re) {
        std::sregex_token_iterator it{str.begin(),
                                      str.end(), re, -1};
        std::vector<std::string> tokenized{it, {}};

        tokenized.erase(
                std::remove_if(tokenized.begin(),
                               tokenized.end(),
                               [](std::string const &s) {
                                   return s.empty();
                               }),
                tokenized.end());

        return tokenized;
    }

    static void join(const std::vector<std::string> &v, char c, std::string &s) {
        s.clear();
        for (auto p = v.begin();
             p != v.end(); ++p) {
            s += *p;
            if (p != v.end() - 1)
                s += c;
        }
    }

    static Base parse(const std::string &base) {
        static const std::regex re_steps(R"(-)");
        static const std::regex re_parts(R"(:)");
        int steps;
        std::string sequence;

        auto parts = tokenize(base, re_steps);
        steps = strtol(parts[1].c_str(), 0, 10);
        sequence = parts[0];
        parts = tokenize(sequence, re_parts);
        return Base(steps, parts.size(), sequence);
    }
};


#define CIRCLE_STEPS_MUL 3000

static uint64_t find_next(uint64_t current, std::string &origin, bool *ok, int states = 5) {
    int calculate = INT_MAX;
    std::string sequence;
    Mutate mutate(4, states);
    int circle = 0;

    while (calculate >= current && circle < (states * CIRCLE_STEPS_MUL)) {
        sequence = mutate.mutate(origin);
        calculate = context_next_run(sequence.c_str());
        ++circle;
    }
    *ok = circle != (states * CIRCLE_STEPS_MUL) && current > calculate;
    origin = sequence;
    return calculate;
}

static void print_plant_uml(const std::string &origin) {
    std::string plant = "@startuml\n[*] --> S_0\n";
    const std::regex re_parts(R"(:)");
    const std::regex re_elements(R"(\.)");
    auto parts = Mutate::tokenize(origin, re_parts);

    for (int i = 0; i < parts.size(); ++i) {
        auto &part = parts.at(i);
        auto elements = Mutate::tokenize(part, re_elements);
        const auto &state = "S_" + std::to_string(i);
        const auto &eat_state = "S_" + elements.at(1);
        const auto &not_state = "S_" + elements.at(3);
        long eat_do = strtol(elements.at(0).c_str(), nullptr, 10);
        long not_do = strtol(elements.at(2).c_str(), nullptr, 10);
        plant += state + " --> " + eat_state + ": яблоко/" + context_get_do_name(eat_do) + "\n";
        plant += state + " --> " + not_state + ": пусто/" + context_get_do_name(not_do) + "\n";
    }
    plant += "\n";
    plant += "@enduml\n";
    fprintf(stdout, "%s\n", plant.c_str());
    fflush(stdout);
}

/**
 * 3.0.0.1:3.0.0.2:3.0.0.3:3.0.0.4:3.0.2.0-315
 * 3.0.0.1:3.4.0.2:3.0.0.3:3.0.0.4:3.0.2.0-295
 * 3.0.0.1:3.4.0.2:3.0.0.3:3.4.0.4:3.0.2.0-275
 * 3.0.0.1:3.5.0.2:3.0.0.3:3.0.0.4:3.0.2.0:3.4.2.4-275
 * 3.0.0.1:3.0.0.2:3.0.0.3:3.5.0.6:3.0.2.0:3.6.3.6:3.6.3.0-263
 * 3.0.0.1:3.4.0.2:3.0.0.3:3.5.0.4:3.0.2.0:3.1.2.4-255
 * 3.0.0.1:3.4.0.2:3.0.0.3:3.5.0.4:3.0.2.0:3.4.2.4-255
 * 3.0.0.1:3.4.0.2:3.0.0.3:3.5.0.4:3.0.2.0:3.6.3.4:3.7.3.4:3.7.1.3-249
 * 3.0.0.1:3.5.0.2:3.0.0.3:3.5.0.4:3.0.2.0:3.4.2.4-235
 * 3.0.0.1:3.5.0.2:3.0.0.3:3.5.0.4:3.0.2.0:3.6.2.6:3.4.2.0-231
 * 3.0.0.1:3.5.0.2:3.0.0.3:3.5.0.4:3.0.2.0:3.6.2.4:3.4.0.6-231
 * 3.0.0.1:3.5.0.2:3.0.0.3:3.5.0.4:3.6.3.0:3.4.2.4:3.2.3.4-227
 * 3.0.0.1:3.5.0.2:3.0.0.3:3.5.0.4:3.0.2.0:3.6.2.6:3.7.3.0:3.2.3.6-227
 * 3.0.0.1:3.5.0.2:3.0.0.3:3.5.0.4:3.0.2.0:3.6.2.6:3.7.3.0:3.0.2.6-227
 * 3.0.0.1:3.5.0.2:3.0.0.3:3.5.0.4:3.0.2.0:3.6.2.6:3.7.3.0:3.4.2.6-227
 * 3.0.0.1:3.6.0.2:3.0.0.3:3.5.0.4:3.0.2.0:3.4.2.4:3.3.3.4:3.4.2.4-227
 * 3.0.0.1:3.6.0.2:3.0.0.3:3.5.0.4:3.8.3.0:3.4.2.4:3.3.3.4:3.4.2.4:3.0.1.3-223
 * 3.0.0.1:3.5.0.2:3.0.0.3:3.6.0.9:3.0.2.0:3.4.2.4:3.9.2.9:3.7.3.3:3.2.3.0:3.9.3.0-223
 * 3.0.0.1:3.5.0.2:3.0.0.3:3.11.0.6:3.0.2.0:3.6.2.6:3.4.2.0:3.12.1.12:3.12.0.1:3.9.1.1:3.1.0.8:3.10.2.6:3.6.1.4-223
 * 3.0.0.1:3.6.0.2:3.0.0.3:3.5.0.4:3.0.2.0:3.4.2.4:3.5.3.9:3.4.2.4:3.2.1.3:3.8.3.8:3.8.2.9:3.2.2.8-221
 * 3.0.0.1:3.5.0.2:3.0.0.3:3.5.0.4:3.7.3.9:3.6.2.6:3.4.2.0:3.0.0.11:3.10.3.3:3.8.0.1:3.1.2.0:3.4.1.10-219
 * 3.0.0.1:3.13.0.2:3.0.0.3:3.5.0.4:3.0.2.0:3.4.2.4:3.1.2.8:3.5.3.12:3.7.1.3:3.10.3.2:3.5.1.2:3.5.1.6:3.6.2.0:3.7.2.6:3.3.0.7-217
 * 3.0.0.1:3.13.0.2:3.0.0.3:3.5.0.4:3.0.2.0:3.4.2.4:3.1.2.8:3.8.0.11:3.7.1.3:3.10.3.2:3.5.1.2:3.5.1.6:3.6.2.0:3.7.2.6:3.3.0.7-215
 * 3.0.0.1:3.13.0.2:3.0.0.3:3.15.0.12:3.0.2.0:3.4.2.4:3.1.2.8:3.8.0.11:3.7.1.3:3.10.3.2:3.5.1.2:3.5.1.6:3.6.2.0:3.7.2.6:3.3.0.7:3.1.3.12-205
 * 3.0.0.1:3.13.0.2:3.0.0.3:3.15.0.12:3.0.2.0:3.4.2.4:3.1.2.8:3.8.0.11:3.7.1.17:3.10.3.2:3.5.1.2:3.5.1.6:3.6.2.0:3.7.2.6:3.3.0.7:3.1.3.12:3.3.3.3:3.13.0.12-201
 * 3.0.0.1:3.13.0.2:3.0.0.3:3.15.0.12:3.0.2.0:3.4.2.4:3.1.2.8:3.8.0.11:3.7.1.17:3.10.3.2:3.5.1.2:3.5.1.6:3.6.2.0:3.7.2.6:3.3.0.7:3.15.3.12:3.3.3.3:3.13.0.12-197
 * 3.0.0.1:3.13.0.2:3.0.0.3:3.15.0.12:3.0.2.0:3.4.2.4:3.1.2.8:3.8.0.11:3.7.1.17:3.10.3.2:3.5.1.2:3.5.1.6:3.6.2.0:3.7.2.6:3.3.0.7:3.15.2.4:3.3.3.3:3.13.0.12-197
 * 3.0.0.1:3.13.0.2:3.0.0.3:3.15.0.12:3.2.3.7:3.4.2.4:3.1.2.8:3.8.0.11:3.7.1.17:3.10.3.2:3.5.1.2:3.5.1.6:3.6.2.0:3.7.2.6:3.3.0.7:3.15.2.4:3.3.3.3:3.13.0.12-187
 * 3.0.0.1:3.13.0.2:3.0.0.3:3.15.0.12:3.2.3.7:3.2.2.13:3.1.2.8:3.8.0.11:3.7.1.17:3.10.3.2:3.5.1.2:3.5.1.6:3.6.2.0:3.7.2.6:3.3.0.7:3.15.2.4:3.3.3.3:3.13.0.12-185
 * 3.0.0.1:3.13.0.2:3.0.0.3:3.15.0.12:3.2.3.7:3.2.2.13:3.9.2.19:3.8.0.11:3.7.1.17:3.10.3.2:3.8.3.4:3.5.1.6:3.6.2.0:3.7.2.6:3.3.0.7:3.15.2.4:3.3.3.3:3.13.0.12:3.18.0.2:3.9.1.17-183
 * */
/*http://www.demo.cs.brandeis.edu/papers/ep93.pdf */
int main() {
    const auto &base = Mutate::parse(
            "3.0.0.1:3.13.0.2:3.0.0.3:3.15.0.12:3.2.3.7:3.2.2.13:3.1.2.8:3.8.0.11:3.7.1.17:3.10.3.2:3.5.1.2:3.5.1.6:3.6.2.0:3.7.2.6:3.3.0.7:3.15.2.4:3.3.3.3:3.13.0.12-185");
    int max = base.steps, calculate = max, states = base.states;
    std::string origin = base.sequence;

    print_plant_uml(base.sequence);
    fprintf(stdout, "%s-%llu\n", base.sequence.c_str(), max);
    fflush(stdout);
    srand(time(nullptr));
    for (int i = 0; i < 3000; ++i) {
        std::string last_origin;
        int last_calculate;
        bool ok = false;
        last_calculate = find_next(calculate, origin, &ok, states);
        if (ok) {
            fprintf(stdout, "%s-%llu\n", origin.c_str(), last_calculate);
            fflush(stdout);
            calculate = last_calculate;
            origin = last_origin;
        } else {
            ++states;
        }
    }
    return EXIT_SUCCESS;
}