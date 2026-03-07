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

std::string toLower(const std::string& s) {
    std::string out = s;
    std::transform(out.begin(), out.end(), out.begin(), ::tolower);
    return out;
}

std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    size_t end = s.find_last_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    return s.substr(start, end - start + 1);
}

std::string determineAIValue(const std::string& section) {
    std::vector<std::string> noneSkinCodes = { "/ADAn", "/ACA3", "/ABAH" };
    for (const auto& code : noneSkinCodes) {
        if (section.find(code) != std::string::npos) {
            return "AI=none";
        }
    }

    std::istringstream stream(section);
    std::string line;
    while (std::getline(stream, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.rfind("MODEL=", 0) == 0) {
            std::string modelValue = line.substr(6);
            if (toLower(modelValue).find("traffic") != std::string::npos) {
                return "AI=fixed";
            }
        }
    }

    return "AI=fixed";
}

bool processFile(const fs::path& rawInputPath) {
    fs::path inputPath = fs::absolute(rawInputPath);

    std::ifstream inFile(inputPath, std::ios::in);
    if (!inFile) return false;

    std::ostringstream buf;
    buf << inFile.rdbuf();
    std::string content = buf.str();
    inFile.close();

    std::vector<std::string> sections = splitSections(content);
    std::vector<std::string> processed;

    for (auto& section : sections) {
        if (trim(section).empty()) continue;
        section = removeAILines(section);
        std::string aiLine = determineAIValue(section);
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
    fs::path outputPath = outputDir / "entry_list.ini";

    bool inputIsOutputName = (fs::canonical(inputPath) == fs::weakly_canonical(outputPath));

    if (inputIsOutputName) {
        fs::path backupPath = outputDir / ("original_entry_list.ini");
        fs::rename(inputPath, backupPath);
    } else {
        fs::path renamedInput = outputDir / ("original_entry_list" + inputPath.extension().string());
        fs::rename(inputPath, renamedInput);

        if (fs::exists(outputPath)) {
            fs::path backupPath = outputDir / ("original_entry_list.ini");
            fs::rename(outputPath, backupPath);
        }
    }

    std::ofstream outFile(outputPath, std::ios::out);
    if (!outFile) return false;
    outFile << output;
    outFile.close();

    return true;
}

int main(int argc, char* argv[]) {
    if (argc < 2) return 1;

    fs::path inputPath = argv[1];
    if (!fs::exists(inputPath)) return 1;

    processFile(inputPath);
    return 0;
}
