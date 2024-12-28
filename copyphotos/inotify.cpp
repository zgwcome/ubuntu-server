#include <mutex>
#include <condition_variable>
#include <deque>
#include <iostream>
#include "inotify-cxx.h"
using namespace std;

extern std::mutex mtx;
extern std::condition_variable cv;
extern std::deque<std::string> photoDeque;
extern const std::string watch_dir;

void Watch()
{
    try {
        Inotify notify;

        InotifyWatch watch(watch_dir, IN_CLOSE_WRITE);
        notify.Add(watch);

        cout << "Watching directory " << watch_dir << endl;
        for (;;) {
            notify.WaitForEvents();

            size_t count = notify.GetEventCount();
            while (count > 0) {
                InotifyEvent event;
                bool got_event = notify.GetEvent(&event);

                if (got_event) {
                    string mask_str;
                    event.DumpTypes(mask_str);

                    string filename = event.GetName();

                    cout << "[watch " << watch_dir << "] ";
                    cout << "event mask: \"" << mask_str << "\", ";
                    cout << "filename: \"" << filename << "\"" << endl;

                    {
                        std::unique_lock lock(mtx);
                        photoDeque.push_front(watch_dir + "/" + filename);
                        cv.notify_one();
                    }

                }

                count--;
            }
        }
    } catch (InotifyException &e) {
        cerr << "Inotify exception occured: " << e.GetMessage() << endl;
    } catch (exception &e) {
        cerr << "STL exception occured: " << e.what() << endl;
    } catch (...) {
        cerr << "unknown exception occured" << endl;
    }

    return;
}