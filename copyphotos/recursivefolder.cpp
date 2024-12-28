#include <iostream>
#include <filesystem>
#include <string>
#include <mutex>
#include <condition_variable>
#include <deque>

extern std::mutex mtx;
extern std::condition_variable cv;
extern std::deque<std::string> photoDeque;
extern const std::string watch_dir;

void Recursive() 
{
    for (const auto& entry : std::filesystem::recursive_directory_iterator(watch_dir)) {
        const auto& path = entry.path();
        if (std::filesystem::is_regular_file(path)) {
            std::unique_lock lock(mtx);
            photoDeque.push_front(path.generic_string());
            cv.notify_one();
        }
    }
 
    return;
}