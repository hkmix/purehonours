#include "PureHonours/purehonours.h"
#include "ShoddyRepl/shoddy.h"

#include <cctype>
#include <exception>
#include <iostream>

int main()
{
    Shoddy repl;

    // Get players
    int players = 0;
    while (true) {
        auto input = repl.get_line("How many players? ");
        if (!input.valid) {
            continue;
        }
        try {
            players = std::stoi(input.command);
        } catch (std::invalid_argument&) {
            std::cout << "Invalid player count." << std::endl;
            continue;
        }

        if (players >= 2 && players <= 4) {
            break;
        } else {
            std::cout << "Invalid player count." << std::endl;
        }
    }

    int count = 0;
    std::vector<std::string> player_names;
    while (count < players) {
        const std::string prompt = "Initials for player " + std::to_string(count + 1) + ": ";
        auto input = repl.get_line(prompt);
        if (!input.valid) {
            continue;
        }

        // Prevent "self" from being a name (used in command)
        if (input.command == "self") {
            std::cout << "Players cannot be named \"self\"." << std::endl;
            continue;
        }

        player_names.push_back(input.command);
        ++count;
    }

    // Initialize game
    PureHonours game(players, std::move(player_names));

    // Get fans
    std::cout << std::endl;
    while (true) {
        auto input = repl.get_line("Add fan/score (Enter to finish, \"d\" for default): ");
        if (!input.valid) {
            std::cout << "Invalid input." << std::endl;
            continue;
        } else if (input.command[0] == 'd') {
            game.default_fans();
            break;
        } else if (input.arg_count < 1) {
            std::cout << "Invalid input." << std::endl;
            continue;
        }

        try {
            game.add_fan_score(std::stoi(input.command), std::stoi(input.args[0]));
        } catch (std::invalid_argument&) {
            std::cout << "Invalid input." << std::endl;
            continue;
        }
    }

    // Game loop
    while (true) {
        const std::string prompt = "\nInput command (? for help): ";
        auto input = repl.get_line(prompt);
        if (!input.valid) {
            std::cout << "Invalid command." << std::endl;
            continue;
        }

        if (input.command[0] == 'q') {
            // Quit
            break;
        } else if (input.command[0] == '?') {
            // Help
            std::cout << "Commands:\n"
                      << "  ?\n"
                      << "    Display help\n"
                      << "  q\n"
                      << "    Quit\n"
                      << "  a <winner> <fan> self\n"
                      << "    Add self-touch win by <winner>\n"
                      << "  a <winner> <fan> <loser>\n"
                      << "    Add a win by <winner>, fed by <loser>\n"
                      << "  d\n"
                      << "    Delete the last-entered score\n"
                      << "  d <round_number>\n"
                      << "    Delete the score of a specific round\n"
                      << "  s\n"
                      << "    Print short score report\n"
                      << "  p\n"
                      << "    Print full score report\n"
                      << "  x\n"
                      << "    Export results to file\n";
        } else if (input.command[0] == 'a' && input.arg_count == 3) {
            // Add
            auto winner = input.args[0];
            for (auto &c : winner) {
                c = std::toupper(c);
            }

            // Check winner
            std::size_t winner_index = game.player_index(winner);
            if (winner_index == static_cast<std::size_t>(players)) {
                std::cout << "Invalid winner initials." << std::endl;
                continue;
            }

            // If self-draw, just add it
            int fan = 0;
            try {
                fan = std::stoi(input.args[1]);
            } catch (std::invalid_argument &) {
                std::cout << "Invalid fan value." << std::endl;
                continue;
            }

            if (input.args[2] == "self") {
                game.add_result(winner_index, fan, true);
            } else {
                // Check loser
                auto loser = input.args[2];
                for (auto &c : loser) {
                    c = std::toupper(c);
                }

                std::size_t loser_index = game.player_index(loser);
                if (loser_index == static_cast<std::size_t>(players)) {
                    std::cout << "Invalid loser initials." << std::endl;
                    continue;
                }

                // Add score
                game.add_result(winner_index, fan, false, loser_index);
            }
        } else if (input.command[0] == 'd') {
            // Check for index
            if (input.arg_count > 0) {
                std::size_t index;
                try {
                    index = std::stoul(input.args[0]);
                } catch (std::invalid_argument&) {
                    std::cout << "Invalid round number." << std::endl;
                    continue;
                }
                if (game.delete_score(index)) {
                    std::cout << "Deleted round " << index << "." << std::endl;
                } else {
                    std::cout << "Invalid round number." << std::endl;
                }
            } else {
                if (game.delete_score()) {
                    std::cout << "Deleted last round." << std::endl;
                } else {
                    std::cout << "No entries to delete." << std::endl;
                }
            }
        } else if (input.command[0] == 's') {
            game.print_scores();
        } else if (input.command[0] == 'p') {
            game.print_report();
        } else if (input.command[0] == 'x') {
            game.print_csv();
        } else {
            // Invalid
            std::cout << "Invalid command. Type ? for help." << std::endl;
        }
    }

    return 0;
}

