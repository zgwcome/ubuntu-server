
#include <stdio.h>
#include <algorithm>
#include <cctype>
#include <condition_variable>
#include <deque>
#include <filesystem>
#include <iostream>
#include <mutex>
#include <string>

/*
Camera Model Name               : Canon EOS R6m2
Serial Number                   : 244021004274

Camera Model Name               : ILCE-6300
Internal Serial Number          : bffe0000aa0d

Camera Model Name               : HUAWEI RIO-AL00
Camera Model Name               : BLA-AL00
*/

const std::string DEST_DIR_JGP = "/data/Photos/R62/";
const std::string DEST_DIR_CR3 = "/data/ARW/R62/";
const std::string DEST_DIR_MP4 = "/data/MP4/R62/";
std::mutex mtx;
std::condition_variable cv;
std::deque<std::string> photoDeque;

extern std::mutex mtx2Cloud;
extern std::condition_variable cv2Cloud;
extern std::deque<std::pair<std::string, std::string>> photoDeque2Cloud;

std::string ExecCmd(const std::string& cmd) {
  std::string result;
  char buffer[1024];
  FILE* fp = popen(cmd.c_str(), "r");

  while (fgets(buffer, sizeof(buffer), fp)) {
    result += buffer;
  }

  pclose(fp);

  return result;
}

std::string GetPhotoCreateTime(const std::string& path) {
  std::string cmd = "exiftool " + path;
  auto result = ExecCmd(cmd);
  // Create Date

  std::string time;
  auto index = result.find("Create Date");
  if (index != std::string::npos) {
    time = result.substr(result.find(":", index) + 2, 10);
    time = time.substr(0, 4) + time.substr(5, 2) + time.substr(8, 2);
  } else {
    time = "Unknow";
  }

  return time;
}

bool CopyPhoto(const std::string& path) {
  std::cout << "Start to copy " << path << std::endl;
  if (!std::filesystem::exists(path)) {
    std::cout << "ERROR: source file " << path << " doesn't exist" << std::endl;
    return false;
  }

  auto createTime = GetPhotoCreateTime(path);

  std::string ext = std::filesystem::path(path).extension().generic_string();
  std::transform(ext.begin(), ext.end(), ext.begin(),
                 [](unsigned char c) { return std::tolower(c); });

  std::string destDir;
  if (ext == ".jpg") {
    destDir = DEST_DIR_JGP;
  } else if (ext == ".cr3") {
    destDir = DEST_DIR_CR3;
  } else if (ext == ".mp4") {
    destDir = DEST_DIR_MP4;
  } else {
    std::cout << "ERROR: unsupport file format" << std::endl;
    return false;
  }
  destDir += createTime;

  if (!std::filesystem::exists(destDir)) {
    if (!std::filesystem::create_directories(destDir)) {
      std::cout << "ERROR: create path " << destDir << " fail" << std::endl;
      return false;
    }
  }

  const std::string dest =
      destDir + "/" + std::filesystem::path(path).filename().generic_string();
  const auto copyOptions = std::filesystem::copy_options::update_existing;
  try {
    std::filesystem::copy_file(path, dest, copyOptions);
    std::cout << "Success copy " << path << " to " << dest << std::endl;
    std::unique_lock lock(mtx2Cloud);
    photoDeque2Cloud.push_front(std::make_pair(path, dest));
    cv2Cloud.notify_one();
  } catch (const std::filesystem::filesystem_error& e) {
    std::cout << "Copy file " << path << " fail " << e.what() << std::endl;
  } catch (...) {
    std::cout << "Copy file " << path << "unknown fail " << std::endl;
  }

  return true;
}

bool Copy() {
  while (true) {
    std::string path;
    {
      std::unique_lock lock(mtx);
      cv.wait(lock, [] { return photoDeque.size() > 0; });
      path = photoDeque.back();
      photoDeque.pop_back();
    }
    CopyPhoto(path);
  }
}