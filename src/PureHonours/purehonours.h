#ifndef __PUREHONOURS_H
#define __PUREHONOURS_H

#include <cstddef>
#include <map>
#include <string>
#include <vector>

struct Result {
    std::size_t winning_player;
    bool self_draw;
    int fan;
    std::size_t losing_player;
};

class PureHonours {
public:
    PureHonours(int player_count, std::vector<std::string> &&player_names);

    void add_fan_score(int fan, int score);
    void add_result(std::size_t winning_player, int fan, bool self_draw, std::size_t losing_player = 0);
    std::string human_readable_result(std::size_t count) const;
    void print_scores() const;
    bool delete_score();
    bool delete_score(std::size_t index);
    void print_report() const;
    void print_csv() const;
    void default_fans();
    std::size_t player_index(const std::string &player_name) const;

private:
    int player_count_;
    std::vector<std::string> player_names_;
    std::vector<std::vector<int>> scores_;
    std::map<int, int> fan_to_score_;
    std::vector<Result> results_;

    int fan_score(int fan, bool self_draw = false) const;
    std::vector<int> tally() const;
};

#endif // __PUREHONOURS_H
