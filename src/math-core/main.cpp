#include <InteractiveToolkit/InteractiveToolkit.h>
#include <iostream>
#include "util.h"

using namespace ITKCommon;
using namespace MathCore;

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
    inline void draw_pixel(int x, int y) noexcept
    {
        if (x >= 0 && x < screen.size.width && y >= 0 && y < screen.size.height)
            screen[y][x] = ' ';
    }

    void clear()
    {
        screen.clear('+');
    }

    void drawLine(const vec2i &a, const vec2i &b)
    {
        AlgorithmCore::Rasterization::BresenhamIterator::Raster(*this, a, b);
    }

    void printToConsole(){
        //draw the virtual device to console
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
    mat4f projection;
    mat3f view_transform;

    float camera_distance;
    float camera_angle_x_rad;
    float camera_angle_y_rad;

    std::vector<vec4f> model_vertex;
    std::vector<uint32_t> model_line_index;

    std::vector<vec2i> device_coord_model_vertex;

    float anim_angle;

    RenderSystem(VirtualDevice *_virtualDevice)
    {
        device = _virtualDevice;

        projection = GEN<mat4f>::projection_perspective_lh_negative_one(
            60.0f,
            (float)device->screen.size.width / (float)device->screen.size.height,
            0.01f,
            10.0f);

        view_transform =
            GEN<mat3f>::scaleHomogeneous((float)device->screen.size.width, (float)device->screen.size.height) *
            GEN<mat3f>::translateHomogeneous(0.5f, 0.5f) *
            GEN<mat3f>::scaleHomogeneous(0.5f, -0.5f) *
            GEN<mat3f>::scaleHomogeneous(34.0f / 16.0f, 1.0f); // pixel aspect correction

        camera_distance = 1.6f;
        camera_angle_x_rad = OP<float>::deg_2_rad(15.0f);
        camera_angle_y_rad = 0;
        anim_angle = 0;

        vec4f vertex_data[] = {
            vec4f(-0.5f, -0.5f, -0.5f, 1.0f),
            vec4f(-0.5f, -0.5f, +0.5f, 1.0f),
            vec4f(-0.5f, +0.5f, -0.5f, 1.0f),
            vec4f(-0.5f, +0.5f, +0.5f, 1.0f),
            vec4f(+0.5f, -0.5f, -0.5f, 1.0f),
            vec4f(+0.5f, -0.5f, +0.5f, 1.0f),
            vec4f(+0.5f, +0.5f, -0.5f, 1.0f),
            vec4f(+0.5f, +0.5f, +0.5f, 1.0f)};

        model_vertex.assign(std::begin(vertex_data), std::end(vertex_data));
        device_coord_model_vertex.resize(model_vertex.size());

        uint32_t line_index[] = {0, 1, 0, 2, 0, 4, 1, 3, 1, 5, 2, 3, 2, 6, 3, 7, 4, 5, 4, 6, 5, 7, 6, 7};
        model_line_index.assign(std::begin(line_index), std::end(line_index));
    }

    void computeAnimation(){
        float elapsed_sec = 0.1f; // 50ms

        // 20 degrees per sec
        const float _deg_step = OP<float>::deg_2_rad(40.0f);
        camera_angle_y_rad = OP<float>::fmod(camera_angle_y_rad + elapsed_sec * _deg_step, CONSTANT<float>::PI * 2.0f);
        camera_angle_x_rad = OP<float>::fmod(camera_angle_x_rad + elapsed_sec * _deg_step, CONSTANT<float>::PI * 2.0f);

        const float _deg_step2 = OP<float>::deg_2_rad(30.0f);
        anim_angle = OP<float>::fmod(anim_angle + elapsed_sec * _deg_step2, CONSTANT<float>::PI * 2.0f);

        float _cos = (OP<float>::cos(anim_angle) + 1.0f) * 0.5f;
        // camera_distance = OP<float>::lerp( 1.6f, 1.0f, _cos);
        camera_distance = OP<float>::lerp(1.8f, 1.2f, _cos);
    }

    void renderCurrentFrame()
    {
        mat4f camera_inv = GEN<mat4f>::translateHomogeneous(0, 0, camera_distance) *
                           GEN<mat4f>::fromEulerHomogeneous(-camera_angle_x_rad, -camera_angle_y_rad, 0);
        mat4f mvp = projection * camera_inv;

        // 1) project -> 2) convert to euclidian space -> 3) convert to screen space
        for (int i = 0; i < (int)model_vertex.size(); i++)
        {
            vec4f vec4_projected_vertex = mvp * model_vertex[i];
            vec3f vec3_homogeneous_point = vec3f(CVT<vec4f>::toVec2_PerspDiv(vec4_projected_vertex), 1.0f);
            vec3f device_point = view_transform * vec3_homogeneous_point;
            device_coord_model_vertex[i] = (vec2i)CVT<vec3f>::toVec2(device_point);
        }

        // render all lines...
        for (int i = 0; i < (int)model_line_index.size(); i += 2)
        {
            uint32_t idx_a = model_line_index[i];
            uint32_t idx_b = model_line_index[i + 1];
            device->drawLine(device_coord_model_vertex[idx_a], device_coord_model_vertex[idx_b]);
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

        virtualDevice.clear();
        renderSystem.computeAnimation();
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
std::mutex  _mutex;
void force_close(int signal)
{
    {
        std::unique_lock<std::mutex> lck(_mutex);
        if (current_thread != nullptr) {
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
