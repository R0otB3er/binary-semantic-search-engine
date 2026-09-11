<a id="readme-top"></a>


<!-- PROJECT LOGO -->
<br />
<div align="center">

<h1 align="center">Semantic Search Engine</h3>

  <p align="center">
    Final project for NLP 4397
    <br />
</div>


<!-- GETTING STARTED -->
## Getting Started

I'm sorry but getting this code running might be annoying

### Prerequisites

#
#### Absolutely required to run code 
* [Cmake](https://cmake.org/cmake/help/book/mastering-cmake/chapter/Getting%20Started.html): needed to compile and run the code since I used C++
* [Document Embeddings](https://drive.google.com/file/d/1mGLioXNiTmqb2iq0JTmOkXCvrYB76NrE/view?usp=drive_link): Download and unzip file and place the .txt file in the /externals folder as shown below.

#### Needed to generate embeddings for the reviews/queries
* [llama.cpp](https://github.com/ggml-org/llama.cpp/blob/master/docs/install.md): Needed to turn tensor files into .gguf files for the python scripts to run properly
* [Qwen3-0.6B](https://huggingface.co/Qwen/Qwen3-Embedding-0.6B): download from huggingface and use [this walkthrough](https://blog.steelph0enix.dev/posts/llama-cpp-guide/) to turn the tensorfile to .gguf no quantization needed
* [llama-cpp_python](https://github.com/abetlen/llama-cpp-python): used in "embeddings.py" pythons script to generate embeddings for the reviews though its gonna take a while...
* [scentence-transformers](https://pypi.org/project/sentence-transformers/): used in "query_embeddings.py" which is called in the main code to dynamically generate new query embeddings if "QueryVectors_Qwen3-0.6B.txt" file cannot be found in Semantic_Engine/externals

#### Code I used but made by others
- [glove.c](https://github.com/shubham0204/glove.c): the stripped down hashmap I use to compare the documents and query embeddings
- [OpenXSLX](https://github.com/troldal/OpenXLSX/tree/master): Library I use to access the reviews from the Excel files.




### File structure
#
#### Since there is alot of file accessing with all of the scripts I'll layout where the files should be and how they should be named. files/folders marked with a * are not in the .zip file and will need to be placed there by the user

#### Binary Search Engine


    Binary_Engine/
    |
    |---externals/
    |   |--OpenXLSX/
    |   |--reviews_segment.xlsx * [given by the professor]
    |
    |---build/
    |
    |---BinarySearch.cpp
    |---CMakeLists.txt

#### Semantic Search Engine
    Semantic_Engine/
    |
    |---externals/
    |   |---glove/
    |   |---OpenXLSX/
    |   |---Qwen3-Embedding-0.6B/ * [check prerequisites above]
    |   |   |---[other innards that are downloaded via huggingface]
    |   |   |---Qwen3-Embedding-0.6B-F16.gguf * [made by user with llama.cpp]
    |   |
    |   |---DocumentVectors_Qwen3-0.6B_1024.txt * [check the required section for download link]
    |   |---queries.txt [can be changed to add new queries dynamically]
    |   |---QueryVectors_Qwen3-0.6B_1024.txt [delete if queries.txt is changed]
    |   |---reviews_segment.pkl * 
    |   |---reviews_segment.xlsx *
    |
    |---build/
    |
    |---CMakeLists.txt
    |---embeddings.py
    |---query_embeddings.py
    |---query_embeddings_v2.py
    |---SemanticSearch.cpp

### Compiling and running either of the search engines

1. Generate the makefiles, this can also be done using the Cmake gui
    ```sh
    cd build
    cmake ..
    ```
2. build the executable
   ```sh
   make
   ```
3. move to the output directory and run the program
   ```sh
   cd output
   [executable name]
   ```

<p align="right">(<a href="#readme-top">back to top</a>)</p>
