#include <iostream>
#include <iomanip>
#include <random>
#include <list>
#include <queue>
#include <set>
#include <chrono>
#include <algorithm>
#include <folly/FBVector.h>
#include "timer.h"
#include <cstdint>
using namespace std;

// how many times to test performance, so we can generate an average
const int REPEAT = 10;
// How many of the smallest items to find for each test
const int BOTTOM = 10;

// Declare the helper methods for each of the data structures
void test_cStyleArray(uint64_t* array, vector<uint64_t> &, int, double &, double &, double &, vector<uint64_t> &);
void test_vec( vector<uint64_t> &,int, double &, double &, double &, vector<uint64_t> &);
void test_list(vector<uint64_t> &,int, double &, double &, double &, vector<uint64_t> &);
void test_heap(vector<uint64_t> &,int, double &, double &, double &, vector<uint64_t> &);
void test_set(vector<uint64_t> &,int, double &, double &, double &, vector<uint64_t> &);
void test_fbfolly(vector<uint64_t> &,int, double &, double &, double &, vector<uint64_t> &);
bool check_results(const vector<uint64_t>&, vector<uint64_t> &);

int main() {
    // Set up rng
    random_device rd;     //Get a random seed from the OS entropy device, or whatever
    mt19937_64 eng(rd()); //Use the 64-bit Mersenne Twister 19937 generator and seed it with entropy.
    uniform_int_distribution<uint64_t> distr;

    double cstyle_load_time = 0,
            cstyle_unload_time = 0,
            cstyle_sort_time = 0;
    vector<uint64_t> cstyle_results;
    double vec_load_time = 0,
            vec_unload_time = 0,
            vec_sort_time = 0;
    vector<uint64_t> vec_results;
    double list_load_time = 0,
            list_unload_time = 0,
            list_sort_time = 0;
    vector<uint64_t> list_results;
    double heap_load_time = 0,
            heap_unload_time = 0,
            heap_sort_time = 0;
    vector<uint64_t> heap_results;
    double set_load_time = 0,
            set_unload_time = 0,
            set_sort_time = 0;
    vector<uint64_t> set_results;
    double fbvec_load_time = 0,
            fbvec_unload_time = 0,
            fbvec_sort_time = 0;
    vector<uint64_t> fbvec_results;

    // Set the range of data sizes to test
    vector<int> offer_sizes = {10, 100, 1000, 10000, 100000, 1000000};
    vector<uint64_t> offer_values;
    vector<uint64_t> offer_values_sorted;

    // Test a range of sizes for data
    for (int offer_size: offer_sizes) {
        if (offer_size < BOTTOM) continue;

        // Allocate memory for the C-style array outside the loop
        uint64_t* cStyleArray = (uint64_t*)malloc(offer_size * sizeof(uint64_t));

        offer_values.clear();
        offer_values_sorted.clear();

        // Repeat a number of times to help determine an average
        for (int x = 0; x < REPEAT; x++) {
            offer_values.resize(offer_size);
            offer_values_sorted.resize(offer_size);
            // Create a list of random uint64_t values to work with
            for (int i = 0; i < offer_size; i++) {
                offer_values.at(i) = distr(eng);
                while (offer_values.at(i) == 0) offer_values.at(i) = distr(eng);
                offer_values_sorted.at(i) = offer_values.at(i);
            }
            // create a sorted copy to use to check results
            sort(offer_values_sorted.begin(), offer_values_sorted.end());

            // Test vector speed
            test_vec(offer_values, offer_size,
                     vec_load_time,
                     vec_unload_time,
                     vec_sort_time,
                     vec_results);

            // Test C-style array speed
            test_cStyleArray(cStyleArray,offer_values, offer_size,
                             cstyle_load_time,
                             cstyle_unload_time,
                             cstyle_sort_time,
                             cstyle_results);

            // Test linked list speed
            test_list(offer_values, offer_size,
                             list_load_time,
                             list_unload_time,
                             list_sort_time,
                             list_results);

            // Test heap
            test_heap(offer_values, offer_size,
                      heap_load_time,
                      heap_unload_time,
                      heap_sort_time,
                      heap_results);

            // Test set
            test_set(offer_values, offer_size,
                      set_load_time,
                      set_unload_time,
                      set_sort_time,
                      set_results);

            // Test folly vec
            test_fbfolly(offer_values, offer_size,
                     fbvec_load_time,
                     fbvec_unload_time,
                     fbvec_sort_time,
                     fbvec_results);

            if (!check_results(offer_values_sorted, fbvec_results)) {
                cerr << "Folly vec results invalid" << endl;
            }
        }


            cout << fixed << setprecision(9);
            cout << setw(6) << "Size:" << setw(9) << offer_size << setw(15) << "Vector" << setw(15)
                 << "C-Style Array" << setw(15) << "List" << setw(15) << "Heap" << setw(15) << "Set" << setw(25) << "Folly Ordered Vector" << endl;
            cout << setw(15) << "Load: "
                    << setw(15) << vec_load_time / REPEAT
                    << setw(15) << cstyle_load_time / REPEAT
                    << setw(15) << list_load_time / REPEAT
                    << setw(15) << heap_load_time / REPEAT
                    << setw(15) << set_load_time / REPEAT
                    << setw(15) << fbvec_load_time / REPEAT

                    << endl;
            cout << setw(15) << "Unload: "
                    << setw(15) << vec_unload_time / REPEAT
                    << setw(15) << cstyle_unload_time / REPEAT
                    << setw(15) << list_unload_time / REPEAT
                    << setw(15) << heap_unload_time / REPEAT
                    << setw(15) << set_unload_time / REPEAT
                    << setw(15) << fbvec_unload_time / REPEAT

                    << endl;
            cout << setw(15) << "Load & Unload: "
                << setw(15) << vec_load_time / REPEAT +
                                   vec_unload_time / REPEAT
                 << setw(15) << cstyle_load_time / REPEAT +
                                cstyle_unload_time / REPEAT
                    << setw(15) << list_load_time / REPEAT +
                                   list_unload_time / REPEAT
                    << setw(15) << heap_load_time / REPEAT +
                                   heap_unload_time / REPEAT
                    << setw(15) << set_load_time / REPEAT +
                                   set_unload_time / REPEAT
                    << setw(15) << fbvec_load_time / REPEAT +
                                   fbvec_unload_time / REPEAT
                 << endl;
            cout << setw(15) << "Sort: "
                    << setw(15) << vec_sort_time / REPEAT
                    << setw(15) << cstyle_sort_time / REPEAT
                    << setw(15) << list_sort_time / REPEAT
                    << setw(15) << heap_sort_time / REPEAT
                    << setw(15) << set_sort_time / REPEAT
                    << setw(15) << fbvec_sort_time / REPEAT

                    << endl;
            cout << setw(15) << "Load & Sort: "
                << setw(15) << vec_load_time / REPEAT
                                   + vec_sort_time / REPEAT
                 << setw(15) << cstyle_load_time / REPEAT
                                + cstyle_sort_time / REPEAT
                    << setw(15) << list_load_time / REPEAT
                                   + list_sort_time / REPEAT
                    << setw(15) << heap_load_time / REPEAT
                                   + heap_sort_time / REPEAT
                    << setw(15) << set_load_time / REPEAT
                                   + set_sort_time / REPEAT
                    << setw(15) << fbvec_load_time / REPEAT
                                   + fbvec_sort_time / REPEAT
                 << endl;
        free(cStyleArray);
    }

    return 0;
}

    void test_cStyleArray(uint64_t* cStyleArray, vector<uint64_t> &offers, int num_offers, double &load_time, double &unload_time, double &sort_time, vector<uint64_t> &results){
        // Load random values into the C-style array, measure load time
        Timer load_timer;
        load_timer.start(); // Begin the timer
        for (int i = 0; i < num_offers; i++) {
            cStyleArray[i] = offers[i]; // Populate the C-style array with the offers
        }
        load_timer.stop(); // End the timer
        load_time += load_timer.time();

        results.clear(); // Clear the results vector before populating it

        // Find and execute the lowest N orders from the C-style array, measure unload time
        Timer unload_timer;
        unload_timer.start(); // Begin the timer

        for (int n = 0; n < BOTTOM; n++) {
            uint64_t min_value = UINT64_MAX;
            int min_index = -1;
            for (int i = 0; i < num_offers; i++) {
                if (cStyleArray[i] < min_value) {
                    min_value = cStyleArray[i];
                    min_index = i;
                }
            }
            if (min_index != -1) {
                results.push_back(min_value);
                cStyleArray[min_index] = UINT64_MAX; // Mark this value as processed
            }
        }

        unload_timer.stop(); // Stop the timer
        unload_time += unload_timer.time();

        for(int i = 0; i < num_offers; i++) {
            cStyleArray[i] = offers[i];
        }

        // Sort the C-style array using the original values
        Timer sort_timer;
        sort_timer.start(); // Begin the timer

        sort(cStyleArray, cStyleArray + num_offers);

        sort_timer.stop(); // Stop the timer
        sort_time += sort_timer.time();
    }

    void test_vec(vector<uint64_t> &offers, int num_offers, double &load_time, double &unload_time, double &sort_time, vector<uint64_t> &results){
        // declare the vector data structure
        vector<uint64_t> vec;

        // Load random values into the vector, measure load time
        Timer load_timer;
        load_timer.start(); // Begin the timer

        // Simulate loading random values into the vector
        for (int i = 0; i < num_offers; i++) {
            vec.push_back(offers[i]); // Populate the vector with the offers
        }

        load_timer.stop(); // End the timer
        load_time += load_timer.time();

        results.clear(); // Clear the results vector before populating it

        // Find and execute the lowest N orders from the C-style array, measure unload time
        Timer unload_timer;
        unload_timer.start(); // Begin the timer

        for (int n = 0; n < BOTTOM; n++) {
            uint64_t min_value = UINT64_MAX;
            int min_index = -1;
            for (int i = 0; i < num_offers; i++) {
                if (vec[i] < min_value) {
                    min_value = vec[i];
                    min_index = i;
                }
            }
            if (min_index != -1) {
                results.push_back(min_value);
                vec[min_index] = UINT64_MAX; // Mark this value as processed
            }
        }

        unload_timer.stop(); // Stop the timer
        unload_time += unload_timer.time();

        for(int i = 0; i < num_offers; i++) {
            vec[i] = offers[i];
        }

        // Sort the C-style array using the original values
        Timer sort_timer;
        sort_timer.start(); // Begin the timer

        sort(vec.begin(), vec.end());

        sort_timer.stop(); // Stop the timer
        sort_time += sort_timer.time();

        // Clear the vector to release memory
        vec.clear();
    }

    void test_list(vector<uint64_t> &offers, int num_offers, double &load_time, double &unload_time, double &sort_time, vector<uint64_t> &results){
    list<uint64_t> linkedlist;

        // Load random values into the list, measure load time
        Timer load_timer;
        load_timer.start(); // Begin the timer

        // Simulate loading random values into the list
        for (int i = 0; i < num_offers; i++) {
            linkedlist.push_back(offers[i]); // Populate the list with the offers
        }

        load_timer.stop(); // End the timer
        load_time += load_timer.time();

        // Make a copy of the linkedlist
        std::list<uint64_t> linkedlistCopy = linkedlist;

        results.clear(); // Clear the results vector before populating it

        // Find and execute the lowest N orders from the list, measure unload time
        Timer unload_timer;
        unload_timer.start(); // Begin the timer

        for (int n = 0; n < BOTTOM; n++) {
            uint64_t min_value = UINT64_MAX;
            std::list<uint64_t>::iterator minIter;

            for (std::list<uint64_t>::iterator it = linkedlistCopy.begin(); it != linkedlistCopy.end(); ++it) {
                if (*it < min_value) {
                    min_value = *it;
                    minIter = it;
                }
            }
            results.push_back(min_value);
            linkedlistCopy.erase(minIter);
        }

        unload_timer.stop(); // Stop the timer
        unload_time += unload_timer.time();

        // Sort the list using the original values
        Timer sort_timer;
        sort_timer.start(); // Begin the timer

        linkedlist.sort();

        sort_timer.stop(); // Stop the timer
        sort_time += sort_timer.time();
    }

    void test_heap(vector<uint64_t> &offers, int num_offers, double &load_time, double &unload_time, double &sort_time, vector<uint64_t> &results){
        priority_queue<uint64_t, std::vector<uint64_t>, std::greater<uint64_t>> min_heap;

        // Load random values into the list, measure load time
        Timer load_timer;
        load_timer.start(); // Begin the timer

        // loading random values into the heap
        for (int i = 0; i < num_offers; i++) {
            min_heap.push(offers[i]); // Populate the min-heap with the offers
        }

        load_timer.stop(); // End the timer
        load_time += load_timer.time();

        // Make a copy of the heap
        priority_queue<uint64_t, std::vector<uint64_t>, std::greater<uint64_t>> min_heap_copy = min_heap;

        results.clear(); // Clear the results vector before populating it

        // Find and execute the lowest N orders from the list, measure unload time
        Timer unload_timer;
        unload_timer.start(); // Begin the timer

        for (int n = 0; n < BOTTOM; n++) {
            if (!min_heap_copy.empty()) {
                uint64_t min_value = min_heap_copy.top();
                min_heap_copy.pop();
                results.push_back(min_value);
            }
        }

        unload_timer.stop(); // Stop the timer
        unload_time += unload_timer.time();

        // min heap is already sorted
        sort_time += 0;
    }

    void test_set(vector<uint64_t> &offers, int num_offers, double &load_time, double &unload_time, double &sort_time, vector<uint64_t> &results){
    std::set<uint64_t> ordered_set;

    // Load random values into the list, measure load time
    Timer load_timer;
    load_timer.start(); // Begin the timer

    // loading random values into the heap
    for (int i = 0; i < num_offers; i++) {
        ordered_set.insert(offers[i]); // Populate the set with the offers
    }

    load_timer.stop(); // End the timer
    load_time += load_timer.time();

    results.clear(); // Clear the results vector before populating it

    // Find and execute the lowest N orders from the list, measure unload time
    Timer unload_timer;
    unload_timer.start(); // Begin the timer

    int count = 0;
    for (const uint64_t& val : ordered_set) {
        results.push_back(val);
        count++;
        if (count >= BOTTOM) {
            break;
        }
    }

    unload_timer.stop(); // Stop the timer
    unload_time += unload_timer.time();

    // set is already sorted
    sort_time += 0;
}

    void test_fbfolly(vector<uint64_t> &offers, int num_offers, double &load_time, double &unload_time, double &sort_time, vector<uint64_t> &results) {
        folly::fbvector<uint64_t> ordered_vector;

        // Load random values into the list, measure load time
        Timer load_timer;
        load_timer.start(); // Begin the timer

        // Loading random values into the vector
        for (int i = 0; i < num_offers; i++) {
            ordered_vector.push_back(offers[i]); // Populate the vector with the offers
        }

        load_timer.stop(); // End the timer
        load_time += load_timer.time();

        // Create a copy of the vector
        folly::fbvector<uint64_t> ordered_vector_copy = ordered_vector;

        results.clear(); // Clear the results vector before populating it

        // Find and execute the lowest N orders from the list, measure unload time
        Timer unload_timer;
        unload_timer.start(); // Begin the timer

        int count = 0;
        while (count < BOTTOM) {
            // Find the minimum value in the vector
            uint64_t min_value = UINT64_MAX;
            int min_index = -1;
            for (int i = 0; i < ordered_vector_copy.size(); i++) {
                if (ordered_vector_copy[i] < min_value) {
                    min_value = ordered_vector_copy[i];
                    min_index = i;
                }
            }

            // If no minimum value is found, break
            if (min_index == -1) {
                break;
            }

            // Add the minimum value to the results
            results.push_back(min_value);

            // Remove the minimum value from the vector
            ordered_vector_copy.erase(ordered_vector_copy.begin() + min_index);

            count++;
        }

        unload_timer.stop(); // Stop the timer
        unload_time += unload_timer.time();

        Timer sort_timer;
        sort_timer.start(); // Start the timer

        sort(ordered_vector.begin(), ordered_vector.end());

        sort_timer.stop(); // Stop the timer
        sort_time += unload_timer.time();
    }


bool check_results(const vector<uint64_t> & data, vector<uint64_t> & bottom){
        for (int i = 0; i < BOTTOM; i++) {
            if(data.at(i) != bottom.at(i)) {
                cerr << "Expected " << data.at(i)  << "at index " << i << " but got " << bottom.at(i) << endl;
                return false;
            }
        }
        return true;
    }