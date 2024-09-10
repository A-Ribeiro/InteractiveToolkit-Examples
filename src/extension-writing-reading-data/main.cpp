#include <InteractiveToolkit/InteractiveToolkit.h>
#include <InteractiveToolkit-Extension/InteractiveToolkit-Extension.h>

#include <iostream>

int main(int argc, char *argv[])
{
    ITKCommon::Path::setWorkingPath(ITKCommon::Path::getExecutablePath(argv[0]));

    printf("Hello World\n");

    Platform::ObjectBuffer objBuffer;

    {
        ITKExtension::IO::AdvancedWriter advWriter;
        advWriter.write(MathCore::vec2f(2, 3));
        advWriter.write(MathCore::mat2f(
            4, 5,
            6, 7));
        advWriter.write<float>(0.5f);
        advWriter.write<double>(0.75);
        advWriter.write<bool>(true);
        advWriter.write<bool>(false);
        advWriter.write<std::string>("string test");
        std::vector<float> numbs = {8, 9, 10};
        advWriter.write<std::vector<float>>(numbs);
        std::unordered_map<std::string, uint8_t> map;
        map["key1"] = 1;
        map["key2"] = 2;
        advWriter.write<std::unordered_map<std::string, uint8_t>>(map);
        advWriter.write<int64_t>(-98765);
        // advWriter.writeToFile("write_test.bin");
        printf("will write\n");
        advWriter.writeToBuffer(&objBuffer);
        printf("written\n");
    }

    {
        ITKExtension::IO::AdvancedReader advReader;
        // advReader.readFromFile("write_test.bin");

        advReader.readFromBuffer(objBuffer);

        auto vec2 = advReader.read<MathCore::vec2f>();
        auto mat2 = advReader.read<MathCore::mat2f>();
        auto _f = advReader.read<float>();
        auto _d = advReader.read<double>();
        auto _b1 = advReader.read<bool>();
        auto _b2 = advReader.read<bool>();
        auto _str = advReader.read<std::string>();
        auto _vec_f = advReader.read<std::vector<float>>();
        auto _map_str_uint8 = advReader.read<std::unordered_map<std::string, uint8_t>>();
        auto _i64 = advReader.read<int64_t>();

        std::cout << vec2.x << ", " << vec2.y << std::endl;
        std::cout << std::endl;
        std::cout << mat2.a1 << ", " << mat2.b1 << std::endl;
        std::cout << mat2.a2 << ", " << mat2.b2 << std::endl;
        std::cout << std::endl;
        std::cout << _f << std::endl;
        std::cout << _d << std::endl;
        std::cout << _b1 << std::endl;
        std::cout << _b2 << std::endl;
        std::cout << _str << std::endl;
        std::cout << std::endl;
        for (auto v : _vec_f)
            std::cout << v << ", ";
        std::cout << std::endl;
        for (auto v : _map_str_uint8)
            std::cout << "[" << v.first << "] = " << (int)v.second << std::endl;
        std::cout << std::endl;
        std::cout << _i64 << std::endl;
    }

    return 0;
}
