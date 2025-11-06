#include <iostream>
#include <vector>
#include <OpenXLSX.hpp>
#include <sstream>
#include <fstream>
#include <cstdlib>

using namespace OpenXLSX;

class review {
    public:
        std::string review_id;
        std::string review_text;
        std::string review_title;
        std::string review_item;

        review(std::string id, std::string title, std::string text, std::string item) {
            review_id = id;
            review_text = text;
            review_title = title;
            review_item = item;
        }
};

void pullReviews(std::vector<review>& reviews, std::string docName){
    XLDocument doc;
    doc.open(docName);
    auto table = doc.workbook().worksheet("Sheet1");
    std::vector<XLCellValue> readValues;
    bool first = true;
    
    for (auto& row : table.rows()) {
        if(first){
            first = false;
        } else {
            readValues = row.values();
            std::string tempID = readValues[0];
            std::string tempItem = readValues[6];
            std::string tempText = readValues[7];
            std::string tempTitle = readValues[3];
            reviews.push_back(review(tempID.substr(1, tempID.length() - 2), tempTitle.substr(1, tempTitle.length() - 2), tempText.substr(1, tempText.length() - 2), tempItem.substr(1, tempItem.length() - 2)));
        }
    
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

void removeSpaces(std::string& input){
    while(input.find(' ') != input.npos){
        input[input.find(' ')] = '_';
    }
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
                auto tokens = tokenize(cur.review_title + " " + cur.review_text +  " " + cur.review_item);

                std::string cleanReview = "("+ cur.review_item + ") " + cur.review_title + ": " + cur.review_text;

                // Add each review to the DB
                reviewMap[cur.review_id] = cleanReview;

                // Add each token to the index
                for (const std::string& token : tokens) {
                    invertedIndex[token].push_back(cur.review_id);
                }
            }

        };

        void search(const std::string& query, bool check_precision) {
            std::vector<std::string> results;
            //auto tokens = tokenize(query);
            std::string aspects;
            std::string opinions;

            if (query.find(':') == query.npos){
                std::cout << "improper search structure, semicolon needed" << std::endl;
                return;
            } else {
                // auto aspects = tokenize(query.substr(0,query.find(':')));
                // auto opinion = tokenize(query.substr(query.find(':') + 1, query.length()));
                aspects = query.substr(0,query.find(':'));
                opinions = query.substr(query.find(':') + 1, query.length());
                std::cout << "aspects: " << aspects << std::endl;
                std::cout << "opinion: " <<  opinions << std::endl;
            }

            std::vector<std::string> aspectOut = getReviews(tokenize(aspects));
            std::vector<std::string> opinionOut = getReviews(tokenize(opinions));
            
            removeSpaces(aspects);

            Test1(&aspectOut, &aspects, check_precision);
            Test2(&aspectOut, &opinionOut, &aspects, check_precision);
            Test3(&aspectOut, &opinionOut, &aspects, check_precision);
            
        };

        void Test1(std::vector<std::string>* aspectOut, std::string* aspects, bool check_precision){
            
            std::ofstream outFile("../../../../Outputs/" + *aspects + "_test1.txt");

            for(auto ID : *aspectOut){
                outFile << ID << "\n";
            }

            outFile.close();

            if(check_precision){
                estimatePrecision(aspectOut);
            }
        }
        
        void Test2(std::vector<std::string>* aspectOut, std::vector<std::string>* opinionOut, std::string* aspects, bool check_precision){
            std::vector<std::string> results;
            results.reserve( aspectOut->size() + opinionOut->size() );
            
            std::set_intersection(aspectOut->begin(),aspectOut->end(),opinionOut->begin(),opinionOut->end(),back_inserter(results));

            std::ofstream outFile("../../../../Outputs/" + *aspects + "_test2.txt");

            for(auto ID : results){
                outFile << ID << "\n";
            }

            outFile.close();

            if(check_precision){
                estimatePrecision(&results);
            }
        }
        
        void Test3(std::vector<std::string>* aspectOut, std::vector<std::string>* opinionOut, std::string* aspects, bool check_precision){
            std::vector<std::string> results;
            results.reserve( aspectOut->size() + opinionOut->size() );
            results.insert( results.end(), aspectOut->begin(), aspectOut->end());
            results.insert( results.end(), opinionOut->begin(), opinionOut->end());

            std::sort(results.begin(), results.end());
            auto last = std::unique(results.begin(), results.end());
            results.erase(last, results.end());

            std::ofstream outFile("../../../../Outputs/" + *aspects + "_test3.txt");

            for(auto ID : results){
                outFile << ID << "\n";
            }

            outFile.close();

            if(check_precision){
                estimatePrecision(&results);
            }
        }

        void estimatePrecision(std::vector<std::string>* results){
            char response;
            bool valid = false;
            int relevant_counter = 0;
            for(int i = 0; i < 100; i++){
                size_t randomID = rand() % results->size();
                std::cout<< reviewMap[(*results)[randomID]] << "\n\nIs this review relevant?(y/n)";

                while(!valid){
                    std::cout<< "\n>";
                    std::cin>> response;

                    if((response == 'y') || (response == 'n')){
                        valid = true;
                    }
                }
                valid = false;

                if(response == 'y'){
                    relevant_counter++;
                }

                std::cout << "\n\n";
            }

            int relevant_reviews = results->size() * relevant_counter / 100;

            std::cout << "calculated precision: " << relevant_counter << "% or " << relevant_reviews << "/" << results->size();
        }
};



int main() {
    SearchEngine engine;
    std::vector<review> reviews;
    // std::vector<std::string> searches = {"phone screen:issues", 
    //                                     "battery life:long",
    //                                     "wifi signal:weak",
    //                                     "mouse button:great",
    //                                     "printer ink:expensive" };

    std::vector<std::string> searches = {"phone screen:issues"};

    pullReviews(reviews, "./reviews_segment.xlsx");
    engine.buildEngine(reviews);

    for(auto search: searches){
        engine.search(search, true);
    }
    

    return 0;
}