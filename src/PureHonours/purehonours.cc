#include "purehonours.h"

#include <algorithm>
#include <iostream>
#include <sstream>

// Helper functions for report printing
namespace report
{

// Width of round column
static const std::size_t ROUND_WIDTH = 8;
// Width of player score column
static const std::size_t COLUMN_WIDTH = 10;

/**
 * Pad a string to given length
 * @param input Input string to pad
 * @param length Length to pad to
 * @return Original input if cannot be padded, else a padded string
 */
std::string centre_pad(const std::string &input, std::size_t length)
{
    // Make sure we can actually pad it
    if (length <= input.size()) {
        return input;
    }

    std::size_t space = length - input.size();
    std::size_t left_pad = space / 2;

    return std::string(left_pad, ' ') + input + std::string(space - left_pad, ' ');
}

std::string centre_pad(const std::string &input)
{
    return centre_pad(input, COLUMN_WIDTH);
}

} // namespace report

/**
 * Constructor for game
 * @param player_count Number of players
 * @param player_names Initials of players
 */
PureHonours::PureHonours(int player_count, std::vector<std::string> &&player_names)
: player_names_(player_names)
{
    if (player_count < 0 || player_count > 4) {
        player_count = 4;
    }

    player_count_ = player_count;
}

/**
 * Add a fan/score combination
 * @param fan Number of fan
 * @param score Score for the fan
 */
void PureHonours::add_fan_score(int fan, int score)
{
    // If the fan already exists, replace it, else add it
    if (fan_to_score_.find(fan) == fan_to_score_.end()) {
        fan_to_score_.emplace(fan, score);
        std::cout << "Added ";
    } else {
        fan_to_score_[fan] = score;
        std::cout << "Set ";
    }

    std::cout << fan << " fan = " << score << "." << std::endl;
}

/**
 * Find a fan score
 * @param fan Number of fan
 * @return Score for that many fan
 */
int PureHonours::fan_score(int fan, bool self_draw) const
{
    int result = 0;
    for (auto &pair : fan_to_score_) {
        if (pair.first <= fan) {
            result = pair.second;
        }
    }

    // Divide self-win score (150%) by number of remaining players
    if (self_draw) {
        result = result * 3 / 2 / (player_count_ - 1);
    }

    return result;
}

/**
 * Add a result to the result vector
 * @param winning_player Index of winning player
 * @param self_draw Whether the win was by self-draw
 * @param fan Number of fan of the win
 * @param losing_player Index of losing player (default 0; ignored if self-draw)
 */
void PureHonours::add_result(std::size_t winning_player, int fan, bool self_draw, std::size_t losing_player)
{
    // Check for min fan
    auto score = fan_score(fan, self_draw);
    if (score == 0) {
        std::cout << "No gai woo son." << std::endl;
        return;
    } else if (fan >= fan_to_score_.rbegin()->first) {
        std::cout << "Sick max yo." << std::endl;
    }

    // Add result
    results_.push_back(Result{
        winning_player,
        self_draw,
        fan,
        losing_player,
    });

    // Add score
    std::vector<int> score_set;
    for (std::size_t i = 0; i < static_cast<std::size_t>(player_count_); ++i) {
        if (i == winning_player && self_draw) {
            score_set.push_back((player_count_ - 1) * score);
        } else if (i == winning_player) {
            score_set.push_back(score);
        } else if (i == losing_player || self_draw) {
            score_set.push_back(-score);
        } else {
            score_set.push_back(0);
        }
    }
    scores_.push_back(score_set);

    // Display human-readable result if non-empty (should never be empty)
    if (!results_.empty()) {
        std::cout << human_readable_result(results_.size() - 1) << std::endl;
    }

    // Print scores
    print_scores();
}

/**
 * Output a result in human-readable format
 * @param count Index of the result
 * @return Human-readable result string
 */
std::string PureHonours::human_readable_result(std::size_t count) const
{
    auto result = results_.at(count);

    std::stringstream ss;
    ss << player_names_.at(result.winning_player)
       << " wins " << result.fan << " fan ";

    if (result.self_draw) {
        ss << std::string("by self draw (") << fan_score(result.fan, true) << " from all).";
    } else {
        ss << "from " << player_names_.at(result.losing_player)
           << " (" << fan_score(result.fan) << ").";
    }

    return ss.str();
}

/**
 * Get index of a player
 * @param player_name Player initials
 * @return Index of player in arrays; returns size if failed
 */
std::size_t PureHonours::player_index(const std::string &player_name) const
{
    std::size_t index = 0;
    while (index < player_names_.size()) {
        if (player_names_[index] == player_name) {
            return index;
        }

        ++index;
    }

    return index;
}

/**
 * Tally player scores
 * @return A vector of the four player scores, totalled
 */
std::vector<int> PureHonours::tally() const
{
    const std::size_t len = player_names_.size();
    std::vector<int> tally(len, 0);

    for (auto &set : scores_) {
        for (std::size_t i = 0; i < len; ++i) {
            tally[i] += set[i];
        }
    }

    return tally;
}

/**
 * Output tallied scores
 */
void PureHonours::print_scores() const
{
    const std::size_t len = player_names_.size();
    auto scores = tally();

    for (std::size_t i = 0; i < len; ++i) {
        std::cout << player_names_[i] << ": " << scores[i] << std::endl;
    }
}

/**
 * Use default set of fans
 */
void PureHonours::default_fans()
{
    // Clear if any exists
    if (!fan_to_score_.empty()) {
        fan_to_score_.clear();
        std::cout << "Cleared existing fan/score pairs." << std::endl;
    }

    static const std::vector<std::pair<int, int>> values = {
        {3, 32},
        {4, 48},
        {5, 64},
        {6, 96},
        {7, 128},
        {8, 192},
        {9, 256},
        {10, 384},
        {11, 512},
        {12, 768},
        {13, 1024},
    };

    for (auto &p : values) {
        add_fan_score(p.first, p.second);
    }
}

/**
 * Print overall score report as a table
 */
void PureHonours::print_report() const
{
    // Print title
    std::cout << '+' << std::string(report::ROUND_WIDTH, '-');
    for (auto i = 0; i < player_count_; ++i) {
        std::cout << '+' << std::string(report::COLUMN_WIDTH, '-');
    }
    std::cout << '+' << std::endl;

    std::cout << '|' << report::centre_pad("Round", report::ROUND_WIDTH) << '|';
    for (std::size_t i = 0; i < player_names_.size(); ++i) {
        std::cout << report::centre_pad(player_names_[i]) << '|';
    }
    std::cout << std::endl;

    std::cout << '+' << std::string(report::ROUND_WIDTH, '-');
    for (auto i = 0; i < player_count_; ++i) {
        std::cout << '+' << std::string(report::COLUMN_WIDTH, '-');
    }
    std::cout << '+' << std::endl;

    // Print rows
    for (std::size_t i = 0; i < scores_.size(); ++i) {
        // Print round number
        const auto &score_set = scores_[i];
        std::cout << '|' << report::centre_pad(std::to_string(i + 1), report::ROUND_WIDTH) << '|';

        // Print scores
        for (auto &score : score_set) {
            if (score == 0) {
                std::cout << std::string(report::COLUMN_WIDTH, ' ');
            } else {
                std::cout << report::centre_pad(std::to_string(score));
            }
            std::cout << '|';
        }
        std::cout << std::endl;
    }

    // If we printed at least one row, print an extra row
    if (!scores_.empty()) {
        std::cout << '+' << std::string(report::ROUND_WIDTH, '-');
        for (auto i = 0; i < player_count_; ++i) {
            std::cout << '+' << std::string(report::COLUMN_WIDTH, '-');
        }
        std::cout << '+' << std::endl;

        // Print totals
        std::cout << '|' << report::centre_pad("Sum", report::ROUND_WIDTH) << '|';
        for (auto &total : tally()) {
            std::cout << report::centre_pad(std::to_string(total)) << '|';
        }
        std::cout << std::endl;

        std::cout << '+' << std::string(report::ROUND_WIDTH, '-');
        for (auto i = 0; i < player_count_; ++i) {
            std::cout << '+' << std::string(report::COLUMN_WIDTH, '-');
        }
        std::cout << '+' << std::endl;
    }
}

/**
 * Print CSV version of report for export
 */
void PureHonours::print_csv() const
{
    // Print title row
    std::cout << "Round";
    for (auto &name : player_names_) {
        std::cout << "," << name;
    }
    std::cout << ",Notes" << std::endl;

    // Print scores
    for (std::size_t i = 0; i < scores_.size(); ++i) {
        const auto &score_set = scores_[i];
        std::cout << (i + 1);

        for (auto &score : score_set) {
            std::cout << ",";
            if (score != 0) {
                std::cout << score;
            }
        }

        std::cout << "," << human_readable_result(i) << std::endl;
    }
}

/**
 * Delete an entered score
 * @param index Index to delete
 */
bool PureHonours::delete_score(std::size_t index)
{
    if (index > 0 && index <= scores_.size()) {
        scores_.erase(scores_.begin() + index - 1);
        results_.erase(results_.begin() + index - 1);

        return true;
    }

    return false;
}

/**
 * Delete last-entered score
 * @return True if successful, false otherwise
 */
bool PureHonours::delete_score()
{
    return delete_score(scores_.size());
}
