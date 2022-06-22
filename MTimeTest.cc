// Copyright 2022 ffoxi
#include <stdio.h>
#include "MTime.h"

int main() {
    Mashiro::InitTimeThreads();
    // int i = 0;
    // Mashiro::SetInterval([&](){ i += 10;}, 1000);
    // Mashiro::SetInterval([&](){ i += 1; }, 1000);
    // Mashiro::Sleep(10000);
    // printf("%d\n", i);

    // Mashiro::Timer t;
    // Mashiro::Sleep(10000);
    // auto i = t.ElapsedTime();
    // printf("%f\n", i);
    // Mashiro::Timer t;
    // Mashiro::SetInterval([&](){
    //     printf("%f\n", t.ElapsedTime());
    // }, 1000);
    // Mashiro::Sleep(20000);

    // Mashiro::DestroyTimeThreads();
    Mashiro::TimeLine timeline(1000.0f);
    Mashiro::Timer timer;

    timeline.Play();
    Mashiro::Sleep(5000);
    timeline.Pause();

    printf("%f\n", timer.ElapsedTime());
    printf("%ld\n", timeline.GetCurrentFrame());
}
