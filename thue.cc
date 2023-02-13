/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <fstream>
#include <locale>
#include <numeric>
#include <random>
#include <sstream>
#include <utility>
#include <vector>

namespace {

enum class Mode {
	kDefault,
	kLeft,
	kRight
};

enum class RuleType {
	kProduction,
	kInput,
	kOutput,
	kNewline
};

struct Rule {
	RuleType type;
	std::string lhs;
	std::string rhs;
};

void Help(void) {
	std::cout << "thue [-cdlnr | -h] [-s N] FILE" << std::endl
			  << "options:" << std::endl
			  << "  -c          leave CR at EOL as is" << std::endl
			  << "  -d          enable the debug print" << std::endl
			  << "  -l          enable the Left mode: priority given to the leftmost match" << std::endl
			  << "  -n          disable the feature to print a newline by rules of form ...::=~" << std::endl
			  << "  -r          enable the Right mode; priority given to the rightmost match" << std::endl
			  << "  -s N        stop after N steps at most" << std::endl
			  << "  -h, --help  print the help" << std::endl;
}

bool ContainsNonWhitespace(const std::string &s) {
	return std::find_if(s.begin(), s.end(), [](unsigned char ch) {return !std::isspace(ch);}) != s.end();
}

void TrimCrAtEol(std::string &s) {
	auto n = s.length();
	if (n > 0 && s[n-1] == '\r')
		s.resize(n-1);
}

using Match = std::pair<int, const Rule &>;

bool CompareMatch(const Match &x, const Match &y) {
	return x.first < y.first;
}

}

int main(int argc, const char *argv[])
{
	bool debug = false;
	Mode mode = Mode::kDefault;
	bool leave_cr = false;
	bool newline_convention = true;
	int stop = -1;

	if (argc < 2) {
		Help();
		return EXIT_FAILURE;
	}
	int i;
	for (i=1; i<argc-1; i++) {
		if (std::strcmp("-h", argv[i]) == 0 ||
			std::strcmp("--help", argv[i]) == 0) {
			Help();
			return EXIT_SUCCESS;
		}
		if (std::strcmp("-c", argv[i]) == 0) {
			leave_cr = true;
			continue;
		}
		if (std::strcmp("-d", argv[i]) == 0) {
			debug = true;
			continue;
		}
		if (std::strcmp("-l", argv[i]) == 0) {
			if (mode == Mode::kRight) {
				std::cerr << "!error: choose only one mode" << std::endl;
				return EXIT_FAILURE;
			}
			mode = Mode::kLeft;
			continue;
		}
		if (std::strcmp("-n", argv[i]) == 0) {
			newline_convention = false;
			continue;
		}
		if (std::strcmp("-r", argv[i]) == 0) {
			if (mode == Mode::kLeft) {
				std::cerr << "!error: choose only one mode" << std::endl;
				return EXIT_FAILURE;
			}
			mode = Mode::kRight;
			continue;
		}
		if (std::strcmp("-s", argv[i]) == 0) {
			if (++i < argc-1) {
				stop = std::stoi(argv[i]);
				if (stop < 0) {
					std::cerr << "!error: give the -s option a non-negative integer" << std::endl;
					return EXIT_FAILURE;
				}
			} else {
				std::cerr << "!error: give the -s option a non-negative integer" << std::endl;
				return EXIT_FAILURE;
			}
		}
	}
	std::ifstream ifs(argv[i], std::ios::binary);
	if (!ifs) {
		std::cerr << "!error: failed to open " << argv[i] << std::endl;
		return EXIT_FAILURE;
	}

	std::vector<Rule> rules;
	std::string line;
	while (std::getline(ifs, line)) {
		if (!ContainsNonWhitespace(line))
			continue;
		auto n = line.find("::=");
		if (n == std::string::npos) {
			std::cerr << "!error: no production symbol (\"::=\") found" << std::endl;
			return EXIT_FAILURE;
		}
		std::string lhs = line.substr(0, n);
		std::string rhs = line.substr(n + 3);
		if (!ContainsNonWhitespace(lhs) &&
			!ContainsNonWhitespace(rhs))
			break;
		if (!leave_cr)
			TrimCrAtEol(rhs);
		Rule r;
		if (rhs == ":::") {
			r.type = RuleType::kInput;
		} else if (rhs.find("~") == 0) {
			if (newline_convention && rhs.length() == 1)
				r.type = RuleType::kNewline;
			else
				r.type = RuleType::kOutput;
		} else {
			r.type = RuleType::kProduction;
		}
		r.lhs = lhs;
		r.rhs = rhs;
		rules.push_back(r);
	}
	if (debug) {
		std::cerr << "!rules:" << std::endl;
		std::for_each(rules.begin(), rules.end(), [](const Rule &r) {
			std::cerr << "|lhs|" << r.lhs << std::endl;
			std::cerr << "|rhs|" << r.rhs << std::endl;
		});
	}

	std::vector<std::string> data;
	while (std::getline(ifs, line)) {
		if (!leave_cr)
			TrimCrAtEol(line);
		data.push_back(line);
	}
	ifs.close();

	std::string state = std::accumulate(data.begin(), data.end(), std::string(""));
	if (debug)
		std::cerr << "!state:" << std::endl
				  << "|" << state << std::endl;

	std::random_device rd;
	std::mt19937 g(rd());
	std::vector<Match> pos;
	for (int i = 0; stop < 0 || i < stop; ++i) {
		pos.clear();
		for (const auto &r : rules) {
			int n = 0;
			do {
				n = state.find(r.lhs, n);
				if (n == std::string::npos)
					break;
				pos.emplace_back(n++, r);
			} while (n < state.length());
		}
		if (pos.empty())
			break;
		std::vector<Match>::iterator it;
		switch (mode) {
		case Mode::kDefault:
			{
				std::uniform_int_distribution<int> dist(0, pos.size()-1);
				int k = dist(g);
				it = pos.begin() + k;
			}
			break;
		case Mode::kLeft:
			it = std::min_element(pos.begin(), pos.end(), CompareMatch);
			break;
		case Mode::kRight:
			it = std::max_element(pos.begin(), pos.end(), CompareMatch);
			break;
		}
		int n = it->first;
		const auto &r = it->second;
		switch (r.type) {
		case RuleType::kProduction:
			{
				state.replace(n, r.lhs.length(), r.rhs);
				if (debug)
					std::cerr << "|" << state << std::endl;
			}
			break;
		case RuleType::kInput:
			{
				if (!std::getline(std::cin, line)) {
					std::cerr << "!error: failed to read input" << std::endl;
					return EXIT_FAILURE;
				}
				if (!leave_cr)
					TrimCrAtEol(line);
				state.replace(n, r.lhs.length(), line);
				if (debug)
					std::cerr << "|" << state << std::endl;
			}
			break;
		case RuleType::kOutput:
			{
				state.erase(n, r.lhs.length());
				std::cout << r.rhs.substr(1) << std::flush;
				if (debug)
					std::cerr << "|" << state << std::endl;
			}
			break;
		case RuleType::kNewline:
			{
				state.erase(n, r.lhs.length());
				std::cout << std::endl;
				if (debug)
					std::cerr << "|" << state << std::endl;
			}
			break;
		}
	}

	return EXIT_SUCCESS;
}
