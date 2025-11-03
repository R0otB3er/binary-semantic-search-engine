#include <iostream>
#include <vector>
#include <OpenXLSX.hpp>
#include <sstream>

using namespace OpenXLSX;

class review {
    public:
        std::string review_id;
        std::string review_text;
        std::string review_title;

        review(std::string id, std::string title, std::string text) {
            review_id = id;
            review_text = text;
            review_title = title;
        }
};

void pullReviews(std::vector<review>& reviews, int amount, std::string docName){
    XLDocument doc;
    doc.open(docName);
    auto table = doc.workbook().worksheet("Sheet1");
    
    for (int i = 0; i < amount; i++) {
        std::string tempA = table.cell("A" + std::to_string(i + 2)).value();
        std::string tempH = table.cell("H" + std::to_string(i + 2)).value();
        std::string tempD = table.cell("D" + std::to_string(i + 2)).value();
        reviews.push_back(review(tempA.substr(1, tempA.length() - 2), tempD.substr(1, tempD.length() - 2), tempH.substr(1, tempH.length() - 2)));
        
        //std::cout << reviews[i].review_id << " : " << reviews[i].review_title << std::endl;
    }

    doc.close();
}

//removes punctuation and converts everything into lowercase
std::string cleanToken(const std::string& word) {
    std::string cleaned;

    for (char c : word ) {
        if (std::isalnum(c)) {
            cleaned += std::tolower(c);
        }
    }

    return cleaned;
}

// splits strings into words

std::vector<std::string> tokenize(const std::string& text){
    std::vector<std::string> tokens;
    std::stringstream ss(text);
    std::string word;

    while (ss >> word) {
        std::string cleaned = cleanToken(word);
        if (!cleaned.empty()){
            tokens.push_back(cleaned);
        }
    }
    return tokens;
}
class SearchEngine {
    private:
        //word -> list of documents
        std::unordered_map<std::string, std::vector<std::string>> invertedIndex;
        std::unordered_map<std::string, std::string> reviewMap;

        std::vector<std::string> getReviews(std::vector<std::string> input){
            std::vector<std::string> output;
            
            for (const std::string& token : input) {
                auto it = invertedIndex.find(token);
                if (it != invertedIndex.end()) {
                    // Merge results (union of postings lists)
                    for (const std::string& review_id : it->second) {
                        output.push_back(review_id);
                    }
                }
            }

            // Remove duplicates
            std::sort(output.begin(), output.end());
            auto last = std::unique(output.begin(), output.end());
            output.erase(last, output.end());

            return output;
        }
    
    public:
        void buildEngine(const std::vector<review>& reviews){
            
            for (const review& cur: reviews) {

                // Tokenize title and text
                auto tokens = tokenize(cur.review_title + " " + cur.review_text);

                std::string cleanReview = cur.review_title + ": " + cur.review_text;

                // Add each review to the DB
                reviewMap[cur.review_id] = cleanReview;

                // Add each token to the index
                for (const std::string& token : tokens) {
                    invertedIndex[token].push_back(cur.review_id);
                }
            }

        };

        std::vector<std::string> search(const std::string& query) {
            std::vector<std::string> results;
            //auto tokens = tokenize(query);

            if (query.find(':') == query.npos){
                std::cout << "improper search structure, semicolon needed" << std::endl;
                return results;
            } else {
                // auto aspects = tokenize(query.substr(0,query.find(':')));
                // auto opinion = tokenize(query.substr(query.find(':') + 1, query.length()));
                std::cout << "aspects: " << query.substr(0,query.find(':')) << std::endl;
                std::cout << "opinion: " << query.substr(query.find(':') + 1, query.length()) << std::endl;
            }

            std::vector<std::string> aspectOut = getReviews(tokenize(query.substr(0,query.find(':'))));
            std::vector<std::string> opinionOut = getReviews(tokenize(query.substr(query.find(':') + 1, query.length())));

            std::set_intersection(aspectOut.begin(),aspectOut.end(),opinionOut.begin(),opinionOut.end(),back_inserter(results));
            

            for (int i = 0; i < results.size(); i++) {
                results[i] = reviewMap[results[i]];
            }

            return results;
        };
};



int main() {
    SearchEngine engine;
    std::vector<review> reviews;

    pullReviews(reviews, 1000, "./reviews_segment.xlsx");
    engine.buildEngine(reviews);

    std::vector<std::string> results = engine.search("battery life: long");

    std::cout << "Reviews containing 'battery life: long':\n";
    
    for (const std::string& review : results) {
        std::cout << "  " << review << std::endl << std::endl;
    }

    return 0;
}