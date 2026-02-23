#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <ctime>

using namespace std;
using namespace chrono;

// --------------------------------------
// SEQUENTIAL FUNCTIONS
// --------------------------------------

long long sequentialSum(const vector<int>& arr) {
    long long sum = 0;
    for (size_t i = 0; i < arr.size(); i++) {
        sum += arr[i];
    }
    return sum;
}

bool sequentialSearch(const vector<int>& arr, int key) {
    for (size_t i = 0; i < arr.size(); i++) {
        if (arr[i] == key)
            return true;
    }
    return false;
}

// --------------------------------------
// THREADED SUM
// --------------------------------------

void partialSum(const vector<int>& arr, int start, int end, long long& result) {
    result = 0;
    for (int i = start; i < end; i++) {
        result += arr[i];
    }
}

// --------------------------------------
// THREADED SEARCH
// --------------------------------------

void partialSearch(const vector<int>& arr, int start, int end, int key, int& found) {
    for (int i = start; i < end; i++) {
        if (arr[i] == key) {
            found = 1;
            return;
        }
    }
}

// --------------------------------------
// MAIN FUNCTION
// --------------------------------------

int main() {

    int n;
    cout << "Enter array size: ";
    cin >> n;

    vector<int> arr(n);

    srand(time(0));

    // Generate random array
    for (int i = 0; i < n; i++) {
        arr[i] = rand() % 100000;
    }

    int key;
    cout << "Enter key to search: ";
    cin >> key;

    // =============================
    // SEQUENTIAL EXECUTION
    // =============================

    auto start1 = high_resolution_clock::now();

    long long seq_sum = sequentialSum(arr);
    bool seq_found = sequentialSearch(arr, key);

    auto end1 = high_resolution_clock::now();
    auto duration1 = duration_cast<milliseconds>(end1 - start1);

    cout << "\n--- Sequential Results ---" << endl;
    cout << "Sum = " << seq_sum << endl;
    cout << "Key Found = " << (seq_found ? "Yes" : "No") << endl;
    cout << "Execution Time = " << duration1.count() << " ms" << endl;

    // =============================
    // THREADED EXECUTION
    // =============================

    int numThreads = 4;   // You can change this
    vector<thread> threads;
    vector<long long> partialResults(numThreads);
    vector<int> found(numThreads, 0);

    int chunkSize = n / numThreads;

    auto start2 = high_resolution_clock::now();

    // -------- Threaded SUM --------

    for (int i = 0; i < numThreads; i++) {
        int start = i * chunkSize;
        int end = (i == numThreads - 1) ? n : start + chunkSize;

        threads.push_back(thread(partialSum, cref(arr), start, end, ref(partialResults[i])));
    }

    for (int i = 0; i < numThreads; i++)
        threads[i].join();

    long long thread_sum = 0;
    for (int i = 0; i < numThreads; i++)
        thread_sum += partialResults[i];

    threads.clear();

    // -------- Threaded SEARCH --------

    for (int i = 0; i < numThreads; i++) {
        int start = i * chunkSize;
        int end = (i == numThreads - 1) ? n : start + chunkSize;

        threads.push_back(thread(partialSearch, cref(arr), start, end, key, ref(found[i])));
    }

    for (int i = 0; i < numThreads; i++)
        threads[i].join();

    bool thread_found = false;
    for (int i = 0; i < numThreads; i++) {
        if (found[i] == 1)
            thread_found = true;
    }

    auto end2 = high_resolution_clock::now();
    auto duration2 = duration_cast<milliseconds>(end2 - start2);

    cout << "\n--- Threaded Results ---" << endl;
    cout << "Sum = " << thread_sum << endl;
    cout << "Key Found = " << (thread_found ? "Yes" : "No") << endl;
    cout << "Execution Time = " << duration2.count() << " ms" << endl;

    // =============================
    // PERFORMANCE ANALYSIS
    // =============================

    double speedup = (double)duration1.count() / duration2.count();
    cout << "\nSpeedup = " << speedup << endl;
    cout << "Efficiency = " << speedup / numThreads << endl;

    return 0;
}