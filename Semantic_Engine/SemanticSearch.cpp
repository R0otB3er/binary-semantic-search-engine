#include "glove/glove.h"
#include "glove/hashmap.c"
#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <algorithm>

struct data{
    std::string word;
    float score;

    data(const std::string& w, float v) : word(w), score(v){}
};

struct CompareByScore{
    bool operator()(const data& a, const data& b) const {
        return a.score > b.score;
    }
};


std::vector<data> glove_compare_all_cosine(glove* instance, const char* target_word, int list_size){
    std::priority_queue<data, std::vector<data>, CompareByScore> minHeap;
    std::vector<data> closest_words;
    float* target_embedding = glove_get_embedding(instance, target_word);

    for(size_t i = 0 ; i < instance->table->num_buckets; i++) {
        bucket_item* cur_node = instance->table->buckets[i].next_item;

        while (cur_node){
            float* cur_score;
            
            if(cur_node->word == target_word){
                cur_node = cur_node->next_item;
                continue;
            }
            
            float* cur_embedding = glove_get_embedding(instance, cur_node->word);
            
            glove_compare_cosine_given_embedding(instance, cur_embedding, target_embedding, cur_score);

            if(minHeap.size < list_size){

                minHeap.push(data(*(cur_node->word), *cur_score));
            }
             else {
            
                if (*cur_score > minHeap.top().score){
                    minHeap.pop();
                    minHeap.push(data(*(cur_node->word), *cur_score));
                }
            }

            cur_node = cur_node->next_item;
        }
    }

    while (!minHeap.empty()) {
        closest_words.push_back(minHeap.top());
        minHeap.pop();
    }

    std::reverse(closest_words.begin(), closest_words.end());

    return closest_words;
}

int main(){
    glove* instance = glove_create("./WordVectors.txt", 1200000, 100);
    std::vector<data> closest_words;
    char word1[80];

    std::cout << "Table Made \n";

    std::cout << "Words to compare:";
    
    std::cin >> word1;

    closest_words = glove_compare_all_cosine(instance, word1, 15);

    std::cout << "15 closest words: \n";

    for(int i = 0; i < 15; i++){
        std::cout << closest_words[i].word << ": " << closest_words[i].score << " \n";
    }



    // while(1){
    //     float score;
    //     char response = 'y';
    //     char word1[80];
    //     char word2[80];

    //     std::cout << "Words to compare:";
    //     std::cin >> word1 >> word2;

    //     if(glove_compare_cosine(instance, word1, word2, &score)){
    //         std::cout << "score: " << score << " \ncontinue? : ";
    //         std::cin >> response;  
    //     } else {
    //         std::cout << "Embeddings not found for word(s) \n";
    //     }
    //     std::cout << std::endl;

    //     if(response == 'n'){
    //         break;
    //     }
    // }  

    glove_release(instance);

    return 0;
}