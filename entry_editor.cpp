#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <regex>
#include <algorithm>
#include <filesystem>
#include <chrono>
#include <iomanip>
#include <ctime>

namespace fs = std::filesystem;

std::string getTimestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y%m%d_%H%M%S");
    return oss.str();
}

std::vector<std::string> splitSections(const std::string& content) {
    std::vector<std::string> sections;
    std::regex carHeader(R"(\[CAR_\d+\])");

    auto begin = std::sregex_iterator(content.begin(), content.end(), carHeader);
    auto end = std::sregex_iterator();

    std::vector<size_t> positions;
    for (auto it = begin; it != end; ++it) {
        positions.push_back(it->position());
    }

    for (size_t i = 0; i < positions.size(); ++i) {
        size_t start = positions[i];
        size_t len = (i + 1 < positions.size()) ? positions[i + 1] - start : std::string::npos;
        sections.push_back(content.substr(start, len));
    }

    return sections;
}

int carNumber(const std::string& section) {
    std::regex re(R"(\[CAR_(\d+)\])");
    std::smatch m;
    if (std::regex_search(section, m, re)) {
        return std::stoi(m[1].str());
    }
    return 0;
}

std::string removeAILines(const std::string& section) {
    std::istringstream stream(section);
    std::ostringstream result;
    std::string line;
    while (std::getline(stream, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.rfind("AI=", 0) != 0) {
            result << line << "\n";
        }
    }
    return result.str();
}

std::string insertAILine(const std::string& section, const std::string& aiLine) {
    std::istringstream stream(section);
    std::ostringstream result;
    std::string line;
    bool inserted = false;

    while (std::getline(stream, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        result << line << "\n";
        if (!inserted && line.rfind("RESTRICTOR=0", 0) == 0) {
            result << aiLine << "\n";
            inserted = true;
        }
    }
    return result.str();
}

std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    size_t end = s.find_last_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    return s.substr(start, end - start + 1);
}

bool processFile(const fs::path& inputPath) {
    std::cout << "Processing: " << inputPath.filename().string() << "\n";

    std::ifstream inFile(inputPath, std::ios::in);
    if (!inFile) {
        std::cerr << "Could not open input file: " << inputPath << "\n";
        return false;
    }
    std::ostringstream buf;
    buf << inFile.rdbuf();
    std::string content = buf.str();
    inFile.close();

    std::vector<std::string> sections = splitSections(content);
    std::vector<std::string> processed;

    for (auto& section : sections) {
        if (trim(section).empty()) continue;

        section = removeAILines(section);

        std::string aiLine = (section.find("/ADAn") != std::string::npos)
            ? "AI=none"
            : "AI=fixed";
      
        section = insertAILine(section, aiLine);

        processed.push_back(trim(section));
    }

    std::sort(processed.begin(), processed.end(), [](const std::string& a, const std::string& b) {
        return carNumber(a) < carNumber(b);
    });

    std::string output;
    for (size_t i = 0; i < processed.size(); ++i) {
        if (i > 0) output += "\n\n";
        output += processed[i];
    }
    output += "\n";

    fs::path outputDir = inputPath.parent_path();
    std::string timestamp = getTimestamp();
    fs::path outputPath = outputDir / ("entry_list_" + timestamp + ".ini");

    std::ofstream outFile(outputPath, std::ios::out);
    if (!outFile) {
        std::cerr << "Could not write output file: " << outputPath << "\n";
        return false;
    }
    outFile << output;
    outFile.close();

    std::cout << "Output written to: " << outputPath.filename().string() << "\n";
    return true;
}

int main(int argc, char* argv[]) {
    std::cout << "=== Entry List Fixer ===\n";

    if (argc < 2) {
        std::cout << "Usage: Drag and drop a .ini or .txt file onto this executable,\n";
        std::cout << "       or run: entry_list_fixer.exe <input_file>\n";
        std::cout << "\nPress Enter to exit...";
        std::cin.get();
        return 1;
    }

    fs::path inputPath = argv[1];

    if (!fs::exists(inputPath)) {
        std::cerr << "File not found: " << inputPath << "\n";
        std::cout << "\nPress Enter to exit...";
        std::cin.get();
        return 1;
    }

    bool ok = processFile(inputPath);

    std::cout << (ok ? "\nDone!" : "\nFailed.") << "\n";
    std::cout << "Press Enter to exit...";
    std::cin.get();
    return ok ? 0 : 1;
}
