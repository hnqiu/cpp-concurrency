## execution policy since c++17

execution policies include
  - sequenced execution
  - parallel execution
  - parallel and vectorised execution
  - vectorised execution (since c++20)

See https://en.cppreference.com/w/cpp/algorithm/execution_policy_tag_t

Note not all compliers support execution policy. Currently only supported with Visual C++?

If an exception is thrown during the algorithm call where execution policy is used (no matter it's sequenced policy or others), the `std::terminate()` will be called and the entire program will be terminated. Exception handling won't work.
