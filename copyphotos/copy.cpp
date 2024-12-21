
#include <string>
#include <stdio.h>
#include <iostream>
#include <filesystem>
#include <mutex>
#include <condition_variable>
#include <deque>

const std::string DEST_DIR = "/photos/";
std::mutex mtx;
std::condition_variable cv;
std::deque<std::string> photoDeque;

std::string ExecCmd(const std::string& cmd)
{
    std::string result;
    char buffer[1024];
    FILE* fp = popen(cmd.c_str(), "r");

    while(fgets(buffer, sizeof(buffer), fp)) {
        result += buffer;
    }
    
    pclose(fp);

    return result;
}

std::string GetPhotoCreateTime(const std::string& path)
{
    std::string cmd = "exiftool " + path;
    auto result = ExecCmd(cmd);
    //Create Date 

    std::string time;
    auto index = result.find("Create Date");
    if(index != std::string::npos){
        time =  result.substr(result.find(":", index) + 2, 10);
        time = time.substr(0, 4) + time.substr(5, 2) + time.substr(8, 2);
    }
    
    return time;
}

bool CopyPhoto(const std::string& path) {
    if(!std::filesystem::exists(path)) {
        std::cout << "ERROR: source file " << path << " doesn't exist" << std::endl;
        return false;
    }

    auto createTime = GetPhotoCreateTime(path);

    if(!std::filesystem::exists(path)) {
        std::cout << "ERROR: source file " << path << " doesn't exist" << std::endl;
        return false;
    }

    const std::string destDir = DEST_DIR + createTime;
    if(!std::filesystem::exists(destDir)) {
        if(!std::filesystem::create_directories(destDir)) {
            std::cout << "ERROR: create path " << destDir << " fail" << std::endl;
            return false;
        }
    }

    const std::string dest = DEST_DIR + createTime + "/" + std::filesystem::path(path).filename().generic_string();
    if(std::filesystem::copy_file(path, dest)) {
        std::filesystem::remove(path);
    }
    std::cout << "Success move " << path << " to " << dest << std::endl; 
    return true;
}

bool Copy() 
{
    while(true) {
        std::string path;
        {
            std::unique_lock lock(mtx);
            cv.wait(lock, [] {return photoDeque.size() > 0;});
            path = photoDeque.back();
            photoDeque.pop_back();

        }
        CopyPhoto(path);
    }
}