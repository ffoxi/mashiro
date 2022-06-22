// copyright ffoxi 2022
#ifndef MTIME_H_
#define MTIME_H_

#include <stdio.h>
#include <pthread.h>

#include <chrono>  // NOLINT
#include <thread>  // NOLINT
#include <functional>
#include <memory> 
#include <vector>

namespace Mashiro {

class Timer {
public:
    explicit Timer() : time_point_(std::chrono::steady_clock::now()) {
    }
    ~Timer() = default;
    float ElapsedTime() {
        auto time = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - time_point_
        );
        return time.count();
    }
private:
    std::chrono::steady_clock::time_point time_point_;
};

static std::vector<std::thread*>* TimeThreads;

static void InitTimeThreads() {
    TimeThreads = new std::vector<std::thread*>();
}

static void DestroyTimeThreads() {
    for (auto& thread : *Mashiro::TimeThreads) {
        pthread_cancel(thread->native_handle());
        thread->join();
        delete thread;
    }
    std::vector<std::thread*> zerovector;
    TimeThreads->swap(zerovector);
    delete TimeThreads;
}

template<typename T> static void Sleep(T millisecond) {
    std::this_thread::sleep_for(std::chrono::milliseconds(
        static_cast<int64_t>(millisecond)));
}

template<typename T> static std::thread *SetTimeOut(
        std::function<void()> func,
        T millisecond) {
    std::thread *thread = new std::thread([func, millisecond](){
        Mashiro::Sleep(millisecond);
        func();
    });
    // thread->detach();
    // Mashiro::TimeThreads->push_back(thread);
    return thread;
}

template<typename T> static std::thread *SetInterval(
        std::function<void()> func,
        T millisecond) {
    std::thread *thread = new std::thread([func, millisecond](){
        auto sleeped_millisecond = millisecond;  // total sleep time theoretically
        auto sleep_millisecond = millisecond;  // sleep time
        Mashiro::Timer timer;
        // wild thread. need break from outside.
        while (1) {
            Mashiro::Sleep(sleep_millisecond);
            func();
            sleeped_millisecond += millisecond;
            sleep_millisecond = sleeped_millisecond - timer.ElapsedTime();
        }
    });
    // thread->detach();
    // Mashiro::TimeThreads->push_back(thread);
    return thread;
}

class TimeLine {
public:
    explicit TimeLine(const float framerate = 24.0f, const int64_t length = 2147483647) :
            current_frame_(0),
            end_frame_(length),
            keyframes_(new std::vector<int64_t>()),
            state_(State::kPause),
            thread_(nullptr){
        SetFrameRate(framerate);
    }
    TimeLine(const TimeLine&) = delete;
    TimeLine(const TimeLine&&) = delete;
    TimeLine& operator=(const TimeLine&) = delete;
    TimeLine& operator=(const TimeLine&&) = delete;
    ~TimeLine() {
        if (state_ == State::kPlay) {
            Pause();
        }
        delete thread_;
        std::vector<int64_t> zero_vector;
        keyframes_->swap(zero_vector);
        delete keyframes_;
    }

    float GetFrameRate() const {
        return 1000.0f / milliseconds_per_frame_;
    }

    void SetFrameRate(const float framerate) {
        milliseconds_per_frame_ = 1000.0f / framerate; 
    }

    int64_t GetCurrentFrame() {
        return current_frame_;
    }

    void SetCurrentFrame(int64_t frame) {
        current_frame_ = frame;
    }

    void Reset() {
        current_frame_ = 0;
    }

    int64_t GetLength() {
        return end_frame_;
    }

    void SetLength(int64_t length) {
        end_frame_ = length;
    }

    bool IsLoop() {
        return isloop_;
    }

    void SetIsLoop(bool isloop) {
        isloop_ = isloop;
    }

    enum class State {
        kPause,
        kPlay,
        kRewound
    };

    void SetState(State state) {
        state_ = state;
    }

    void Play() {
        SetState(State::kPlay);
        thread_ = Mashiro::SetInterval([&](){
            Update();
        },milliseconds_per_frame_);
    }

    void Pause() {
        SetState(State::kPause);
        pthread_cancel(thread_->native_handle());
        thread_->join();
    }

    void Update() {
        switch (state_) {
        case State::kPause:
            break;
        case State::kPlay:
            if (current_frame_ >= 0 && current_frame_ <= end_frame_) {
                ++current_frame_;
            }
            break;
        case State::kRewound:
            if (current_frame_ >= 0 && current_frame_ <= end_frame_) {
                --current_frame_;
            }
            break;
        default:
            break;
        }
    }

private:
    int64_t current_frame_;
    int64_t end_frame_;
    std::vector<int64_t>* keyframes_;
    std::thread *thread_;
    float milliseconds_per_frame_;
    State state_;
    bool isloop_;
};

}  // namespace Mashiro

#endif  // MTIME_H_
