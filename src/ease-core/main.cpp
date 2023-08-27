#include <InteractiveToolkit/InteractiveToolkit.h>
#include <iostream>
#include "util.h"

using namespace ITKCommon;
using namespace MathCore;
using namespace EaseCore;
using namespace EventCore;

class VirtualDevice
{
public:
    Matrix<char> screen;

    VirtualDevice(const vec2i &size)
    {
        screen.setSize(size);
    }

    vec2i size() const
    {
        return screen.size;
    }

    // commands
    inline void draw_pixel_color(int x, int y, const char &color) noexcept
    {
        if (x >= 0 && x < screen.size.width && y >= 0 && y < screen.size.height)
            screen[y][x] = color;
    }

    void clear()
    {
        screen.clear('+');
    }

    void drawLine(const vec2i &a, const vec2i &b, const char &color)
    {
        AlgorithmCore::Rasterization::BresenhamIterator::RasterWithColor(*this, a, b, color);
    }

    void printToConsole()
    {
        // draw the virtual device to console
        for (int y = 0; y < screen.size.height; y++)
        {
            for (int x = 0; x < screen.size.width; x++)
                printf("%c", screen[y][x]);
            printf("\n");
        }
    }
};

class RenderSystem
{
public:
    VirtualDevice *device;
    mat3f view_transform;

    float current_lrp;
    vec2i pt_a;
    vec2i pt_b;
    bool clear_on_next_frame;
    int sleep_count;

    int current_index;

    Callback<float(float, float, float)> allEase[40] = {
        Easef::inQuad<float>,
        Easef::outQuad<float>,
        Easef::inOutQuad<float>,
        Easef::outInQuad<float>,
        Easef::inCubic<float>,
        Easef::outCubic<float>,
        Easef::inOutCubic<float>,
        Easef::outInCubic<float>,
        Easef::inQuart<float>,
        Easef::outQuart<float>,
        Easef::inOutQuart<float>,
        Easef::outInQuart<float>,
        Easef::inQuint<float>,
        Easef::outQuint<float>,
        Easef::inOutQuint<float>,
        Easef::outInQuint<float>,
        Easef::inSine<float>,
        Easef::outSine<float>,
        Easef::inOutSine<float>,
        Easef::outInSine<float>,
        Easef::inExpo<float>,
        Easef::outExpo<float>,
        Easef::inOutExpo<float>,
        Easef::outInExpo<float>,
        Easef::inCirc<float>,
        Easef::outCirc<float>,
        Easef::inOutCirc<float>,
        Easef::outInCirc<float>,
        Easef::inElastic<float>,
        Easef::outElastic<float>,
        Easef::inOutElastic<float>,
        Easef::outInElastic<float>,
        Easef::inBack<float>,
        Easef::outBack<float>,
        Easef::inOutBack<float>,
        Easef::outInBack<float>,
        Easef::inBounce<float>,
        Easef::outBounce<float>,
        Easef::inOutBounce<float>,
        Easef::outInBounce<float>};

    const char *names[40] = {
        " Easef::inQuad ",
        " Easef::outQuad ",
        " Easef::inOutQuad ",
        " Easef::outInQuad ",
        " Easef::inCubic ",
        " Easef::outCubic ",
        " Easef::inOutCubic ",
        " Easef::outInCubic ",
        " Easef::inQuart ",
        " Easef::outQuart ",
        " Easef::inOutQuart ",
        " Easef::outInQuart ",
        " Easef::inQuint ",
        " Easef::outQuint ",
        " Easef::inOutQuint ",
        " Easef::outInQuint ",
        " Easef::inSine ",
        " Easef::outSine ",
        " Easef::inOutSine ",
        " Easef::outInSine ",
        " Easef::inExpo ",
        " Easef::outExpo ",
        " Easef::inOutExpo ",
        " Easef::outInExpo ",
        " Easef::inCirc ",
        " Easef::outCirc ",
        " Easef::inOutCirc ",
        " Easef::outInCirc ",
        " Easef::inElastic ",
        " Easef::outElastic ",
        " Easef::inOutElastic ",
        " Easef::outInElastic ",
        " Easef::inBack ",
        " Easef::outBack ",
        " Easef::inOutBack ",
        " Easef::outInBack ",
        " Easef::inBounce ",
        " Easef::outBounce ",
        " Easef::inOutBounce ",
        " Easef::outInBounce "};

    RenderSystem(VirtualDevice *_virtualDevice)
    {
        device = _virtualDevice;

        view_transform =
            GEN<mat3f>::translateHomogeneous(0.5f, 0.5f) *
            GEN<mat3f>::scaleHomogeneous((float)(device->screen.size.width - 1), (float)(device->screen.size.height - 1)) *
            GEN<mat3f>::translateHomogeneous(0.5f, 0.5f) *
            GEN<mat3f>::scaleHomogeneous(0.5f, -0.5f);

        current_lrp = 0.0f;
        clear_on_next_frame = true;
        pt_a = computeCurrentPT(0.0f);
        pt_b = computeCurrentPT(0.0f);
        sleep_count = 0;
        current_index = 0;
    }

    vec2i computeCurrentPT(float current_lrp)
    {

        float _x = OP<float>::lerp(-1.0f, 1.0f, current_lrp);
        current_lrp = OP<float>::minimum(current_lrp, 1.0f);
        float _y = allEase[current_index](-0.6f, 0.6f, current_lrp);

        return (vec2i)CVT<vec3f>::toVec2((view_transform * vec3f(_x, _y, 1.0f)));
    }

    void renderCurrentFrame()
    {   
        if (sleep_count > 0)
        {
            sleep_count--;
            return;
        }

        if (clear_on_next_frame)
        {
            device->clear();
            clear_on_next_frame = false;
        }

        device->drawLine(pt_a, pt_b, ' ');

        float elapsed_sec = 0.05f; // 50ms

        current_lrp = OP<float>::move(current_lrp, 1.0f + elapsed_sec * 3.0f, elapsed_sec);

        pt_a = pt_b;
        pt_b = computeCurrentPT(current_lrp);

        device->drawLine(pt_a, pt_b, 'o');

        memcpy(&device->screen[0][0], names[current_index], strlen(names[current_index]));

        if (current_lrp >= 1.0f + elapsed_sec * 2.0f)
        {
            current_lrp = 0;
            clear_on_next_frame = true;
            pt_a = computeCurrentPT(0.0f);
            pt_b = computeCurrentPT(0.0f);
            sleep_count = 50;
            current_index = (current_index + 1) % 40;
        }
    }
};

bool exit_thread = false;

void thread_run()
{

    const int screen_width = 64;
    const int screen_height = 16;

    VirtualDevice virtualDevice(vec2i(screen_width, screen_height));
    RenderSystem renderSystem(&virtualDevice);

    set_show_cursor(false);

    clear_screen();
    int count = 0;
    while (!exit_thread)
    {
        cursor_to_origin();

        renderSystem.renderCurrentFrame();
        virtualDevice.printToConsole();

        printf("<press enter to exit> ");
        printf("frame: %i ", count);
        fflush(stdout);

        count++;
        sleep_ms(50);
    }

    set_show_cursor(true);
}

#include <signal.h>
std::thread *current_thread = nullptr;
std::mutex _mutex;
void force_close(int signal)
{
    {
        std::unique_lock<std::mutex> lck(_mutex);
        if (current_thread != nullptr)
        {
            exit_thread = true;
            current_thread->join();
            current_thread = nullptr;
            printf("\n");
        }
    }
    exit(0);
}

int main(int argc, char *argv[])
{
    Path::setWorkingPath(Path::getExecutablePath(argv[0]));

    std::thread thread(thread_run);
    current_thread = &thread;
    signal(SIGINT, force_close);

    fgetc(stdin);

    force_close(-1);
    /*exit_thread = true;
    thread.join();*/

    return 0;
}
