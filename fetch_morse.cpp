#include <iostream>
#include <vector>
#include <string>
#include <curl/curl.h>
#include <regex>
#include <algorithm>

// Callback function to write HTTP response data
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* data) {
    size_t total_size = size * nmemb;
    data->append((char*)contents, total_size);
    return total_size;
}

// Function to fetch HTML content from URL
std::string fetchHTML(const std::string& url) {
    CURL* curl;
    CURLcode res;
    std::string response_data;

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36");
        
        res = curl_easy_perform(curl);
        
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }
        
        curl_easy_cleanup(curl);
    }
    
    return response_data;
}

// Function to extract Morse code table from HTML
std::vector<std::string> extractMorseCodeTable(const std::string& html) {
    std::vector<std::string> morseArray;
    
    // Regex pattern to find table rows containing Morse code data
    // This looks for table rows with character and Morse code pattern
    std::regex row_pattern(R"(<tr[^>]*>.*?<td[^>]*>.*?([A-Za-z0-9]).*?</td>.*?<td[^>]*>(.*?)</td>.*?</tr>)", 
                          std::regex_constants::icase | std::regex_constants::dotall);
    
    std::smatch matches;
    std::string::const_iterator search_start(html.cbegin());
    
    while (std::regex_search(search_start, html.cend(), matches, row_pattern)) {
        if (matches.size() >= 3) {
            std::string character = matches[1].str();
            std::string morse_code = matches[2].str();
            
            // Clean up the Morse code (remove HTML tags if any)
            std::regex html_tags("<[^>]*>");
            morse_code = std::regex_replace(morse_code, html_tags, "");
            
            // Remove extra whitespace
            morse_code.erase(std::remove_if(morse_code.begin(), morse_code.end(), 
                                          [](unsigned char c) { return std::isspace(c); }), 
                           morse_code.end());
            
            // Create the formatted string
            std::string entry = character + ": " + morse_code;
            morseArray.push_back(entry);
        }
        search_start = matches.suffix().first;
    }
    
    return morseArray;
}

// Alternative parsing method using string operations (more robust)
std::vector<std::string> parseMorseCodeTable(const std::string& html) {
    std::vector<std::string> morseArray;
    
    // Morse code mapping for standard characters (as fallback)
    std::vector<std::string> standardMorse = {
        "A: .-", "B: -...", "C: -.-.", "D: -..", "E: .", "F: ..-.", "G: --.", 
        "H: ....", "I: ..", "J: .---", "K: -.-", "L: .-..", "M: --", "N: -.", 
        "O: ---", "P: .--.", "Q: --.-", "R: .-.", "S: ...", "T: -", "U: ..-", 
        "V: ...-", "W: .--", "X: -..-", "Y: -.--", "Z: --..",
        "0: -----", "1: .----", "2: ..---", "3: ...--", "4: ....-", "5: .....", 
        "6: -....", "7: --...", "8: ---..", "9: ----.",
        ".: .-.-.-", ",: --..--", "?: ..--..", "' .----.", "!: -.-.--", 
        "/: -..-.", "(: -.--.", "): -.--.-", "&: .-...", ":: ---...", 
        ";: -.-.-.", "=: -...-", "+: .-.-.", "-: -....-", "_: ..--.-", 
        "\": .-..-.", "$: ...-..-", "@: .--.-."
    };
    
    // Try to find the table in HTML
    size_t table_start = html.find("<table");
    if (table_start == std::string::npos) {
        std::cout << "Table not found in HTML, using standard Morse code table." << std::endl;
        return standardMorse;
    }
    
    size_t table_end = html.find("</table>", table_start);
    if (table_end == std::string::npos) {
        std::cout << "Table end not found, using standard Morse code table." << std::endl;
        return standardMorse;
    }
    
    std::string table_html = html.substr(table_start, table_end - table_start + 8);
    
    // Parse table rows
    size_t pos = 0;
    while ((pos = table_html.find("<tr", pos)) != std::string::npos) {
        size_t row_end = table_html.find("</tr>", pos);
        if (row_end == std::string::npos) break;
        
        std::string row = table_html.substr(pos, row_end - pos);
        
        // Extract character and Morse code from table cells
        std::vector<std::string> cells;
        size_t cell_pos = 0;
        while ((cell_pos = row.find("<td", cell_pos)) != std::string::npos) {
            size_t cell_start = row.find(">", cell_pos) + 1;
            size_t cell_end = row.find("</td>", cell_start);
            if (cell_end == std::string::npos) break;
            
            std::string cell_content = row.substr(cell_start, cell_end - cell_start);
            
            // Remove HTML tags from cell content
            size_t tag_start;
            while ((tag_start = cell_content.find("<")) != std::string::npos) {
                size_t tag_end = cell_content.find(">", tag_start);
                if (tag_end == std::string::npos) break;
                cell_content.erase(tag_start, tag_end - tag_start + 1);
            }
            
            // Trim whitespace
            cell_content.erase(0, cell_content.find_first_not_of(" \t\n\r"));
            cell_content.erase(cell_content.find_last_not_of(" \t\n\r") + 1);
            
            if (!cell_content.empty()) {
                cells.push_back(cell_content);
            }
            
            cell_pos = cell_end + 5;
        }
        
        // Expecting at least 2 cells: character and Morse code
        if (cells.size() >= 2) {
            std::string entry = cells[0] + ": " + cells[1];
            morseArray.push_back(entry);
        }
        
        pos = row_end + 5;
    }
    
    // If no data was extracted, use standard Morse code
    if (morseArray.empty()) {
        std::cout << "No Morse code data extracted, using standard table." << std::endl;
        return standardMorse;
    }
    
    return morseArray;
}

int main() {
    // Initialize libcurl
    curl_global_init(CURL_GLOBAL_DEFAULT);
    
    std::string url = "http://moratech.com/aviation/morsecode.html";
    
    std::cout << "Fetching Morse code table from: " << url << std::endl;
    
    std::string html_content = fetchHTML(url);
    
    if (html_content.empty()) {
        std::cerr << "Failed to fetch HTML content." << std::endl;
        curl_global_cleanup();
        return 1;
    }
    
    std::cout << "Successfully fetched HTML content (" << html_content.length() << " bytes)" << std::endl;
    
    // Extract Morse code table
    std::vector<std::string> morseArray = parseMorseCodeTable(html_content);
    
    // Sort the array alphabetically
    std::sort(morseArray.begin(), morseArray.end());
    
    // Display the results
    std::cout << "\n=== Morse Code Table (Ordered Array) ===" << std::endl;
    std::cout << "Found " << morseArray.size() << " entries:" << std::endl;
    std::cout << "=========================================" << std::endl;
    
    for (size_t i = 0; i < morseArray.size(); ++i) {
        std::cout << "morseArray[" << i << "] = \"" << morseArray[i] << "\"" << std::endl;
    }
    
    // Cleanup libcurl
    curl_global_cleanup();
    
    return 0;
}
