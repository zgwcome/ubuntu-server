#include <string>
#include <stdio.h>
#include <iostream>
#include <filesystem>
#include <algorithm>
#include <cctype>
#include <mutex>
#include <condition_variable>
#include <deque>


extern std::string ExecCmd(const std::string& cmd);

std::mutex mtx2Cloud;
std::condition_variable cv2Cloud;
std::deque<std::pair<std::string, std::string>> photoDeque2Cloud;

bool CopyToCloud()
{
    while(true) {
        std::pair<std::string, std::string> path;
        {
            std::unique_lock lock(mtx2Cloud);
            cv2Cloud.wait(lock, [] {return photoDeque2Cloud.size() > 0;});
            path = photoDeque2Cloud.back();
            photoDeque2Cloud.pop_back();

        }

        std::string cmd = "rclone copy --config=/config/config.txt -v " + path.first +" netdisk:T6/backup" + path.second.substr(5);
        std::string result = ExecCmd(cmd);
        if(result.find("Error") == std::string::npos) {
            std::cout << "Upload " << path.first << " to cloud success!";
            std::filesystem::remove(path.first);
        }else{
            std::cout << "Upload " << path.first << " to cloud fail!";
        }
    }
}